import urllib.request
import numpy as np
import time
from datetime import datetime as dt

file_exists = False
header = 'timestamp,vbat'

def run():
    #Parse data from node webserver
    fp = urllib.request.urlopen("http://192.168.1.1", timeout=10)
    mybytes = fp.read()
    mystr = mybytes.decode("utf8")
    fp.close()
  
    #Get required data
    out = mystr.split('<tr>')
    lines = [line.split('color=*>') for line in out]
    try:
        outval = float(lines[10][0].split('color=green>')[1].split('</font>')[0])   #battery enough power
    except:
        outval = float(lines[10][0].split('color=red>')[1].split('</font>')[0])     #battery power low
    now = dt.now()
    out_line = f'{now.timestamp()},{outval}\n'
    print(out_line)
    
    global file_exists
    #Save data to file
    with open("lora_gateway_log.txt", "a+") as f:
        if not file_exists:
            f.write(header)
            file_exists = True
        f.write(out_line)

    time.sleep(1)
 
while True:
    run()