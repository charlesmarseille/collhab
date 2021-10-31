# Code to automatically take a picture on the rpi with the imx462 and download on local, then delete on remote.
# ISO = GAIN*100, gain from 1 to 200, 200->20000 100->
# SHUTTER = 1000/shutter_val, 200000->1/5s 150000->1/7 100000->1/10 10000->1/100 1000->1/1008 500->1/2049 100->1/11364

import os
import time
from datetime import datetime as dt
import numpy as np
import sys

user = 'pi'
host = '192.168.2.78'

gains = sys.argv[1]
shutters = sys.argv[2]
#print(f'{gains=}')
#print(f'{shutters=}')

#gains = gains.split(',')
#gains = np.arange(*gains)

#shutters = gains.split(',')
#shutters = np.arange(int(*shutters))

gains = [1, 10, 50, 100, 200]
shutter = [100, 500, 1000, 10000, 15000, 20000]

for gain in gains:
	for shutter in shutters:
		now = str(dt.now())
		fname = (now[:10]+'_'+now[11:19]).replace(':','')
		print(f'shutter: {shutter}')
		print(f'gain: {gain}')
		print('SSH into remote pi')
		os.system(f'ssh -l {user} {host} libcamera-still -t 1 -o {fname}.jpg --gain 100 --shutter {shutter} --flush --nopreview --denoise off --rawfull --raw --autofocus off')
		print(f'File saved as: raw/{fname}.dng')
		print('SCP into caller')
		if not os.path.exists('raw'):
			os.mkdir('raw')
		os.system(f'scp {user}@{host}:/home/{user}/2021* ./raw/')
		os.system(f'ssh -l {user} {host} rm *.dng *.jpg')

		#libcamera-still -t 1 -o 1.jpg --gain 10 --shutter 200 --flush --nopreview --denoise off --rawfull --raw --autofocus off