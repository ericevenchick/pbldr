import serial


ser = serial.Serial(3, 115200, timeout=0.1)

print "Opening port:", ser.portstr

ready = False

for i in range(0,100):
    ser.write('S')               # request flash start
    if (ser.readline() == "OK"): # check for response
        print "Starting Flash..."
        ready = True
        break;

if not ready:
    print "No response from target!"
    quit()

with open("file.hex", 'rb') as f:
    byte = f.read(1)
    while byte != "":
        ser.write(byte)
        print byte
        byte = f.read(1)        # get the next byte
