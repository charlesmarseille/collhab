import numpy as _np
import rawpy as _rawpy
from glob import glob as _glob
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

def exif_read(fname,raw=False):
    with _ExifTool() as et:
        exif = et.get_metadata(fname)

    if raw:
        return exif

    gen = [
        "Make","Model","LensModel",
        "ImageWidth", "ImageHeight",
        "ExposureTime", "ISO",
        "ShutterSpeedValue"
    ]
    keys = { k:"EXIF:"+k for k in gen }

    make = exif[keys['Make']]
    if make == "SONY":
        maker = {
            "ShutterSpeedValue" : "MakerNotes:SonyExposureTime2",
            "ImageHeight" : "MakerNotes:SonyImageHeightMax",
            "ImageWidth" : "MakerNotes:SonyImageWidthMax"
        }
    elif make == "DJI":
        maker = {
            "LensModel" : None
        }

    keys.update(maker)
    info = { k : exif[v] if v is not None else '----' \
        for k,v in keys.items() }

    return info


def compute_stats(fnames):
    # Taken from https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
    def _update(existingAggregate, newValue):
        (count, mean, M2) = existingAggregate
        count += 1
        delta = newValue - mean
        mean += delta / count
        delta2 = newValue - mean
        M2 += delta * delta2
        return (count, mean, M2)

    def _finalize(existingAggregate):
        (count, mean, M2) = existingAggregate
        return (mean, M2 / count)

    rgb = open_raw(fnames[0]).astype(_np.float64)
    aggregate = (1,rgb,_np.zeros_like(rgb))
    for fname in fnames[1:]:
        rgb = open_raw(fname)
        aggregate = _update(aggregate,rgb)
    mean, variance = _finalize(aggregate)
    return mean, _np.sqrt(variance)

def open_clipped(fnames,mean=None,stdev=None,sigclip=5):
    basename = ""
    if type(fnames) == str:
        basename = fnames.replace("*","$").replace("?","&").replace(".","!")+".npy"
        if _os.path.isfile(basename):
            print(f"Opening {fnames} from cache")
            return _np.load(basename)
        fnames = glob_types(fnames)
    if mean is None or stdev is None:
        print("Computing statistics...")
        mean,stdev = compute_stats(fnames)
    print("Clipping files...")
    arr = open_raw(fnames[0]).astype(_np.float64)
    for fname in fnames[1:]:
        rgb = open_raw(fname).astype(_np.float64)
        rgb[_np.abs(rgb-mean) > stdev*sigclip] = _np.nan
        arr = _np.nansum([arr,rgb],0)
    out = _np.round(arr/len(fnames)).astype(_np.uint16)
    if basename:
        _np.save(basename,out)
    return out

def cosmicray_removal(image,**kwargs):
    if "sigclip" not in kwargs:
        kwargs['sigclip'] = 25
    if image.ndim == 3:
        new_data = _np.stack( [
            _detect_cosmics(image[:,:,i],**kwargs)[1] \
            for i in range(3)
        ], axis=2 )
    else:
        new_data = _detect_cosmics(image,**kwargs)[1]
    return new_data

def sub(frame,dark):
    return frame.astype(_np.int32)-dark

def cycle_mod(x,a=2*_np.pi):
    pos = x%a
    neg = x%-a
    return _np.where(_np.abs(neg) < pos, neg, pos)

def glob_types(pattern="*",types=["ARW","arw"]):
    return sum( (_glob(f"{pattern}.{t}") for t in types), [] )

def circle_mask(x,y,shape,r):
    Y,X = _np.ogrid[:shape[0],:shape[1]]
    return (X-x)**2 + (Y-y)**2 < r**2

def correct_linearity(data,lin_data="linearity.csv"):
    if type(lin_data) == str:
        lin_data = _pd.read_csv(lin_data)

    dat = _np.array([
        _np.interp(
            data[:,:,i],
            lin_data[band][::-1],
            lin_data["Exposure"][::-1]
        ) for i,band in enumerate("RGB")
    ])

    return dat.transpose(1,2,0)

def correct_flat(data,flat_data="flatfield.npy"):
    if type(flat_data) == str:
        flat_data = _np.load(flat_data)

    return data / (flat_data / flat_data.max((0,1)))