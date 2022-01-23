import losmandi_driver
import numpy as np

mnt = losmandi_driver.losmandi_mount()

mnt.connect()
mnt.initiate()

for delay in np.linspace(1e-5,1e-4,3):
	print('delay: ', delay)
	mnt.delay = delay
	for _ in range(5):
		mnt.get('date')
		mnt.get('tz')
		mnt.get('lat')
		mnt.get('lon')

for i in range(10):
	print(1*i)
	mnt.slew(1*i,'e')
mnt.slew(0,'e')