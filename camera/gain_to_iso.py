# Code to find the gain-iso relationship for camera imx462 starvis on rpi
# Must install exiftool beforehand (sudo apt-get install exifttool)

import os
from glob import glob
import numpy as np
import matplotlib.pyplot as plt

fnames = glob('raw/*.dng')
gains = np.arange(100,2000,100)
print(f'{gains=} {gains.dtype=}')

isos = np.array([float(os.popen(f'exiftool {fname} | grep ISO').read().split(':')[-1]) for fname in fnames])
print(f'{isos=} {isos.dtype=}')

plt.figure(figsize=(9,6), dpi=200)
plt.plot(gains,isos)
plt.scatter(gains, isos)
plt.xlabel('Gain')
plt.ylabel('ISO')
plt.savefig('gains_iso.png')

np.savetxt('gains_to_iso.txt', np.array([gains, isos]))
