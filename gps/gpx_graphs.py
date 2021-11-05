import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import gpxpy
import gpxpy.gpx

# Open and parse the gpx file
gpx_file = open('20211024.gpx', 'r')	# open the file

gpx = gpxpy.parse(gpx_file)		# parse the file and create gpx object

points = gpx.tracks[0].segments[0].points	# get points from the first segment from the first track

# Remove the bad points from the start of the file (depends on the gps track)
points = points[122:]

# Get relevant gpx values (columns)
ts = np.array([point.time for point in points], dtype=(np.datetime64))		# get time objects and format as datetime
lat, lon, ele, hdop, vdop = np.array([[point.latitude, point.longitude, point.elevation, point.position_dilution, point.vertical_dilution] for point in points]).T	# parse interesting values

# Make graphs
plt.figure()
plt.plot(ts, ele)
plt.plot(ts, vdop)
plt.show()
