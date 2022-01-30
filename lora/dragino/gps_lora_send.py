import loralib
import os
import time
import serial

ser = serial.Serial(
    port='/dev/ttyAMA0',
    baudrate = 9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)


def get_gps():
    valid=False
    x=ser.readline().decode('utf8')
    print(x)
    try:
        ind = x.find('GPGGA')
        valid = True
        print('ind', ind)
        lat = x[17:26]
        lon = x[31:40]
        print(lat,lon)
        return lat,lon
    except:
        print('bad message')

def send_lora(message):
    lat,lon = message
    loralib.send(bytes(str(lat)+', '+str(lon), encoding='utf8'))


loralib.init(1,915000000,9)             #rx or tx (0,1), freq (Hz), spread factor (7-12)


while True:
    latlon = get_gps()
    send_lora(latlon)