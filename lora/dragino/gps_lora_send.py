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


def get_gps():
    while 1:
        valid=False
        x=ser.readline().decode('utf8')
        try:
            ind = x.find('GPGGA')
            valid = True
        except: 
            pass
        if valid:
            try:
                lat = x[17:26]
                lon = x[31:40]
                print(lat,lon)
                return lat,lon
            except:
                print('bad message')

def send_lora(message):
    lat,lon = message
    loralib.send(bytes(str(lat)+', '+str(lon), encoding='utf8'))


loralib.init(0,915000000,7)


while True:
    latlon = get_gps()
    send_lora(latlon)

