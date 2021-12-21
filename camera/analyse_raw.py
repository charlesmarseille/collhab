from lisc import open_raw, exif_read
from glob import glob
import numpy as np
import matplotlib.pyplot as plt

fnames = glob('C:/Users/charl/Documents/imx462_photos')
fnames.sort()

ims = np.array([lisc.open_raw(fname) for fname in fnames])
exifs = np.array([lisc.exif_read(im) for im in ims])

for im in ims:
	plt.figure()
	plt.imshow(im)

