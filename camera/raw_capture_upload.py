# Code to automatically take a picture on the rpi with the imx462 and download on local, then delete on remote.
# ISO = GAIN*100, gain from 1 to 200, 200->20000 100->
# SHUTTER = 1000/shutter_val, 200000->1/5s 150000->1/7 100000->1/10 10000->1/100 1000->1/1008 500->1/2049 100->1/11364, 1->

import os
import time
from datetime import datetime as dt
import numpy as np
import sys
#from lisc import open_raw
import rawpy
import matplotlib.pyplot as plt
import exiftool

gains = [1, 5, 10, 50]
shutters = [1000, 10000, 100000, 200000]

for gain in gains:
    for shutter in shutters:
        now = str(dt.now())
        fname = (now[:10]+'_'+now[11:19]).replace(':','')
        print(f'shutter: {shutter}')
        print(f'gain: {gain}')
        print('SSH into remote pi')
        os.system(f'libcamera-still -o {fname}.jpg --analoggain {gain} --shutter {shutter} --flush --nopreview --denoise off --rawfull --raw --autofocus off --awbgains 1,1')
        print(f'File saved as: {fname}.dng')
        path = fname+'.dng'
        with rawpy.imread(path) as raw:
            rgb = raw.postprocess(gamma=(1,1), no_auto_bright=True, output_bps=16)
#        with exiftool.ExifTool() as et:
#            metadata = et.get_metadata_batch(f'{fname}.dng')
#        print(metadata)
        plt.imshow(rgb[:,:,2])
        plt.show()
        print(rgb.max())




# while True:
# 	now = str(dt.now())
# 	fname = (now[:10]+'_'+now[11:19]).replace(':','')
# 	print(f'shutter: {shutter}')
# 	print(f'gain: {gain}')
# 	print('SSH into remote pi')
# 	os.system(f'ssh -l {user} {host} libcamera-still -t 1 -o {fname}.jpg --gain {gain} --shutter {shutter} --flush --nopreview --denoise off --rawfull --raw --autofocus off')
# 	print(f'File saved as: raw/{fname}.dng')
# 	print('SCP into caller')
# 	if not os.path.exists('raw'):
# 		os.mkdir('raw')
# 	os.system(f'scp {user}@{host}:/home/{user}/2021* ./raw/')
# 	os.system(f'ssh -l {user} {host} rm *.dng *.jpg')