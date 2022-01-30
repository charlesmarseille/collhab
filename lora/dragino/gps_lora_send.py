
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
    timeout=1)


def get_gps():
    valid=False
    while ser.in_waiting:
        x=ser.readline().decode('utf8')
#        print(x)
        ind = x.find('GPGGA')
        if ind != -1:
            try:
                lat = x[ind+17:26]
                lon = x[ind+30:40]
                print(lat,lon)
                return lat,lon
            except:
                pass
#                print('bad message')

def send_lora(message):
#    lat,lon = message
    val = message
#    loralib.send(bytes(str(lat)+', '+str(lon), encoding='utf8'))
    loralib.send(bytes(str(message), encoding='utf8'))

loralib.init(1,915000000,9)             #rx or tx (0,1), freq (Hz), spread factor (7-12)


while True:
    latlon = get_gps()
    send_lora(latlon)

