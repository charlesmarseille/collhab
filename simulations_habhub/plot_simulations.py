import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from glob import glob
import haversine as hs				#"pip install haversine", library used to compute distance between gps coordinates


# Create an example array (just for testing, not used in code)
#a = np.array([1,2,100,3,-10,0.111])
print('hello')

# get filenames
fnames = glob('simulations_habhub/simulations_habhub_csv/Profile_vol_nuit/Guillaume_simu/profile_vol_nuit/*/flight*.csv')


# load data for selected files in filenames
dfs = [pd.read_csv(fname, header=None) for fname in fnames]
ts, lat, lon, alt = df.values.T


# Example plot graph of lat/lon vs time
plt.figure()
plt.plot(ts, lat, label='latitude')
plt.plot(ts, lon, label='longitude')
plt.legend(loc='upper right')
plt.show()

asc_mask = np.diff(alt)>0	# determine ascent
des_mask = np.diff(alt)<0	# determine descent


alt[1:][asc_mask]			# check all altitudes that are growing (ignore first altitude value)
alt[:-1][asc_mask]			# check all altitudes that are growing (ignore last altitude value)


# Example compute mean latitude ascent vs mean lat descent
mean_lat_asc = np.mean(np.diff(lat[1:][asc_mask]))
mean_lat_des = np.mean(np.diff(lat[1:][des_mask]))
lat_variation_percent = mean_lat_des/mean_lat_asc

# Example of using haversine to determine distance from 2 gps coords:
#loc1=(28.426846,77.088834)
#loc2=(28.394231,77.050308)
#hs.haversine(loc1,loc2)

latlon = [(lat[i],lon[i]) for i in range(lat.shape[0])]

delta_x = np.array([hs.haversine(latlon[i], latlon[i+1], unit=hs.Unit.METERS) for i in range(len(latlon)-1)])



plt.figure()
plt.plot(ts, gps_speed, label='gps speed')
plt.legend(loc='upper right')
plt.show()











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