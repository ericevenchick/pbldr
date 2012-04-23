import serial
import binascii
from time import sleep

ser = serial.Serial(3, 115200, timeout=0.2)

print "Opening port:", ser.portstr

ready = False

print "Waiting for target..."

for i in range(0,50):
    ser.write('S')
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


ser.write("DONE")

print "Flash Complete!"
