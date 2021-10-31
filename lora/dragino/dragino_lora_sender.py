import loralib
import os
import time
import serial

ser = serial.Serial(
        port='/dev/ttyS0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)


def loop():
	while 1:
		x=ser.readline().decode('utf8')
		ind = x.find('GPGGA')
		if ind>=0:
			lat = x[17, 26]
			lon = x[31, 40]

while True:
	loop()






            #print(type(start), type(stop))
            try:
                lat = float(x[start-8:start-3])
                lon = float(x[start+4:stop+9])
                print(lat,lon)
                return lat,lon