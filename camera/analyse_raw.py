from lisc import open_raw, exif_read
from glob import glob

fnames = glob('C:/Users/charl/Documents/imx462_photos')

ims = np.array([lisc.open_raw(fname) for fname in fnames])
exifs = np.array([lisc.exif_read(im) for im in ims])

