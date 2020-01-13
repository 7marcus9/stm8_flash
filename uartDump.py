#!/usr/bin/env python3
import serial
import sys
ser = serial.Serial('/dev/ttyUSB0', 500000)

running = True
fp = open(sys.argv[1], "wb")
cnt = 0
while running:
	inB = ser.read()
	inBx = ser.read()
	running = (ord(inB) == (ord(inBx) ^ 0xff))
	if running:
		fp.write(inB)
		cnt = cnt + 1
		if (cnt % 5000 == 0):
			print(cnt)
	else:
		print("EXIT")
		print("INB")
		print(ord(inB))
		print("INBX")
		print(ord(inBx))
fp.close()
print(cnt)
