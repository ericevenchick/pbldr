import serial
import binascii
from time import sleep

def build_mem_map(f):
    memmap = {}

    for line in f.readlines():
        count = int(line[1:3], 16)
        addr = int(line[3:7], 16)
        linetype = int(line[7:9], 16)
        data = line[9:(9+count*2)]

#        print "c: %d, a: %x, t: %d, d: %s" % (count,addr,linetype,data)
        if addr < 0x800 or addr > 0xffff:
            print "Address %x out of range!" % addr
            continue

        for byte in data.decode('hex'):
            memmap[addr] = byte
            addr = addr + 1
    return memmap

def get_last_addr(f):
    lastaddr = 0
    for line in f.readlines():
        addr = int(line[3:7], 16)
        if addr > lastaddr and addr >= 0x800 and addr <= 0xffff:
            lastaddr = addr
    return lastaddr

with open("blink.hex") as f:
    memmap = build_mem_map(f)
    lastaddr = get_last_addr(f)

ser = serial.Serial(3, 115200, timeout=0.2)

print "Opening port:", ser.portstr

ready = False

print "Waiting for target..."

for i in range(0,50):
    ser.write('S')
    if (ser.readline() == "OK\n"): # check for response
        print "Flashing target",
        ready = True
        break;
if not ready:
    print "No response from target!"
    quit()

count = 1
with open("file.hex", 'rb') as f:
    for line in f.readlines():
            for byte in (line[9:len(line)-4]):
                ser.write(byte)
                if count == 64:
                    ser.read(1)
                    count = 1
                    print ".",
                else:
                    count = count + 1

for i in range(0,64):
    ser.write('D')

print "Flash Complete!"
