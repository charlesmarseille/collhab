# Code to find the gain-iso relationship for camera imx462 starvis on rpi
# Must install exiftool beforehand (sudo apt-get install exifttool)

import matplotlib.pyplot as plt
import numpy as _np
import rawpy as _rawpy
from glob import glob
import os as _os
import pandas as _pd
from exiftool import ExifTool as _ExifTool


def open_raw(fname, normalize=False):
    print(f"Opening raw file '{fname}'")
    raw = _rawpy.imread(fname)
    rgb = raw.postprocess(
        gamma = (1,1),
        output_bps = 16,
        no_auto_bright = True,
        user_flip = 0,
        demosaic_algorithm = _rawpy.DemosaicAlgorithm(0)
    )

    if normalize:
        rgb = rgb / (2**16 - 1)

    return rgb

# def exif_read(fname,raw=False):
#     with _ExifTool() as et:
#         exif = et.get_metadata(fname)

#     if raw:
#         return exif

#     gen = [
#         "Make","Model","LensModel",
#         "ImageWidth", "ImageHeight",
#         "ExposureTime", "ISO",
#         "ShutterSpeedValue"
#     ]
#     keys = { k:"EXIF:"+k for k in gen }

#     make = exif[keys['Make']]
#     if make == "SONY":
#         maker = {
#             "ShutterSpeedValue" : "MakerNotes:SonyExposureTime2",
#             "ImageHeight" : "MakerNotes:SonyImageHeightMax",
#             "ImageWidth" : "MakerNotes:SonyImageWidthMax"
#         }
#     elif make == "DJI":
#         maker = {
#             "LensModel" : None
#         }

#     keys.update(maker)
#     info = { k : exif[v] if v is not None else '----' \
#         for k,v in keys.items() }

#     return info



fnames = glob('*.dng')
fnames.sort()
f_gain = [fnames[i] for i in [0,6,12,18,24]]
f_shutter = fnames[:6]
gains = [1, 10, 50, 100, 200]
shutters_in = [100, 500, 1000, 10000, 15000, 20000]
print(f'{gains=}')

isos = np.array([float(_os.popen(f'exiftool {fname} | findstr ISO').read().split(':')[-1]) for fname in f_gain])
shutters_out = np.array([1/float(_os.popen(f'exiftool {fname} | findstr /n /C:"Shutter Speed"').read().split(':')[-1][3:-1]) for fname in f_shutter])

print(f'{isos=} {isos.dtype=}')

plt.figure(figsize=(9,6), dpi=200)
plt.plot(gains,isos)
plt.scatter(gains, isos)
plt.xlabel('Gain')
plt.ylabel('ISO')
plt.savefig('gains_iso.png')

plt.figure(figsize=(9,6), dpi=200)
plt.plot(shutters_in, shutters_out)
plt.xlabel('shutter_in')
plt.ylabel('shutter_out')
plt.savefig('shutters.png')

ims = np.array([open_raw(fname, normalize=True) for fname in fnames])


np.savetxt('gains_to_iso.txt', np.array([gains, isos]))
