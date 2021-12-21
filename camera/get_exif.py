from glob import glob
import exiftool
import os

fnames = glob('*.dng')
fnames.sort()

def get_exif(fname):
	with exiftool.ExifTool() as et:
		meta = et.get_metadata(fname)
	return meta

metas = np.array([get_exif(fname) for fname in fnames])

isos = np.array([meta['EXIF:ISO'] for meta in metas])
shutters = np.array([meta['EXIF:ExposureTime'] for meta in metas])

for i in len(fnames):
	os.system(f'mv {fnames[i]} {fnames[i][:-4]}_{isos[i]}_{shutters[i]}.dng')