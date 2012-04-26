import serial
import binascii
from time import sleep

#port = int(raw_input("COM Port Number?"))

ser = serial.Serial(4, 115200, timeout=0.2)

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
