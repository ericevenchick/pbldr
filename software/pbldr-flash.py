import serial, sys
from time import sleep

filename = sys.argv[1]
debug = True;

# returns a dictionary that maps each byte of program memory
# to a byte of opcode read from an INHX32 formatted file
def build_mem_map(f):
    memmap = {}
    f.seek(0)
    for line in f.readlines():
        count = int(line[1:3], 16)
        addr = int(line[3:7], 16)
        linetype = int(line[7:9], 16)
        data = line[9:(9+count*2)]

        # only flash addresses within range
        if addr < 0x800 or addr > 0xffff:
            continue

        for byte in data.decode('hex'):
            memmap[addr] = byte
            addr = addr + 1
    return memmap

# finds the last address used by an INHX32 formatted file
def get_last_addr(f):
    lastaddr = 0
    f.seek(0)
    for line in f.readlines():
        addr = int(line[3:7], 16)
        if addr > lastaddr and addr >= 0x800 and addr <= 0xffff:
            lastaddr = addr
    return lastaddr

# read the INHX32 file
with open(filename) as f:
    memmap = build_mem_map(f)
    lastaddr = get_last_addr(f)


ser = serial.Serial(4, 115200, timeout=0.2)
print "Opening port:", ser.portstr
ready = False

print "Waiting for target..."

# send the target data until a response is received
for i in range(0,50):
    ser.write('S')
    if (ser.readline() == "OK\n"): # check for response
        ready = True
        break;

# a timeout occured, give up
if not ready:
    print "No response from target!"
    quit()

print "Flashing region %x - %x" % (0x800, lastaddr)

count = 1
for i in range(0x800,lastaddr):
    if memmap.has_key(i):
        ser.write(memmap[i].encode('hex').upper())
        if debug:
            print "%x: %s" % (i, memmap[i].encode('hex').upper())
    else:
        ser.write("FF")
        if debug:
            print "%x: FF" % (i)

    # wait for acknowledgement after 64 bytes
    if count == 64:
        while ser.read(1) != 'K':
            pass
        count = 1
        print ".",
    else:
        count = count + 1

# indicate end of programming
for i in range(0,64):
    ser.write('k')

print "Flash Complete!"
