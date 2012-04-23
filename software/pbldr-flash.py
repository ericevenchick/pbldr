import serial
import binascii

ser = serial.Serial(3, 115200, timeout=1)

print "Opening port:", ser.portstr

ready = False

for i in range(0,10):
    if (ser.readline() == "OK\n"): # check for response
        print "Starting Flash..."
        ready = True
        break;
if not ready:
    print "No response from target!"
    quit()

with open("file.hex", 'rb') as f:
    for line in f.readlines()[1:]:
            for byte in binascii.unhexlify(line[9:len(line)-4]):
                ser.write(byte)
                print binascii.hexlify(byte)

for i in range(1,1000):
    ser.write("DONE")
