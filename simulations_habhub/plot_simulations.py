import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from glob import glob

# get filenames
#fnames = glob('Downloads/flight*.csv')

# load data for selected file in filenames (select wanted file with corresponding index)
df = pd.read_csv('Downloads/flight_path (3).csv')
ts, lat, lon, alt = df.values.T

# plot graph of altitude vs time
plt.figure()
plt.plot(ts, alt)

asc_mask = np.diff(alt)>0	# determine ascent
des_mask = np.diff(alt)<0	# determine descent

lat_rad = lat/180*np.pi
lon_rad = lon/180*np.pi
# 
delta_x = np.array([2*6371000*np.arcsin(np.sqrt(np.sin((lat_rad[i+1]-lat_rad[i])/2)**2+np.cos(lat_rad[i])*np.cos(lat_rad[i+1])*np.sin((lon_rad[i+1]-lon_rad[i])/2)**2)) for i in range(lat_rad.shape[0]-1)])

# plot cumulative distance
d_cumul = np.zeros(delta_x.shape[0]-1)
for i in range(delta_x.shape[0]-1):
	d_cumul[i] = d_cumul[i-1]+delta_x[i]
plt.figure()
plt.plot(np.arange(delta_x.shape[0]-1)*50/60, d_cumul/1000)
plt.xlabel('Time interval (s)')
plt.ylabel('Horizontal distance (km)')

# compute 3d speed with pythagorean gps lat,long to meters
v_asc = np.mean(np.sqrt(np.diff(d_cumul[asc_mask[:-1]])**2+np.diff(alt[:-1][asc_mask])**2))/50
v_des = np.mean(np.sqrt(np.diff(d_cumul[des_mask[:-1]])**2+np.diff(alt[:-1][des_mask][:-1])**2))/50
