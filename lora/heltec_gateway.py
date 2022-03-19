import serial
ser = serial.Serial('/dev/ttyUSB0',115200)

while True:
    mess = ser.readline().decode()
    print(mess)
    with open('heltec_gateway_output.txt', 'a+') as f:
        f.write(mess)
