import serial
import numpy as np
import time
import io
import os

class losmandi_mount():
	def __init__(self):
		self.mount = None
		self.is_connected = False
		self.baud = 9600
		self.timeout = 0.0000001
		self.delay = 0.05
		self.message = None
		self.port = os.popen('python -m serial.tools.list_ports').read().split('\n')[1][:5]
		self.tz = '-5'			# interval RTC uses UTC, timezone is defined by the offset from UTC
		self.date = time.strftime('%m/%d/%y', time.localtime(time.time()))
		self.time = time.strftime('%H:%M:%S', time.localtime(time.time()))		#sync with pc local time
		self.lat = '045*22' 	# UdeS SIRENE station lat
		self.lon = '-071*15'		# UdeS SIRENE station lon
		self.test = None
		self.lib = {'tz':'G',
					'date':'C',
					'time':'L',
					'lat':'t',
					'lon':'g',
					'rate': 'w',
					'ra': 'R',
					'dec': 'D'}

	def connect(self):
		print(f'Connecting on port {self.port}')
		self.mount = serial.Serial(self.port, self.baud, timeout=self.timeout)
		self.mount.flush()

	def disconnect(self):
		self.mount.close()

	def write(self, message, delay=None):
		self.mount.write(bytes(message, 'ascii'))
		self.mount.reset_input_buffer()
		if delay:
			time.sleep(delay)
		else:
			time.sleep(self.delay)
		self.read()

	def read(self):
		while self.mount.in_waiting:
			self.message = self.mount.readline().decode('ascii', 'replace')
		self.mount.reset_input_buffer()

	def get(self, var, delay=None):
		self.write(f':G{self.lib[var]}#', delay=delay)		#output is in self.message variable
		print(f'{var}: {self.message}')

	def set(self, var, val, delay=None):
		self.write(f':S{self.lib[var]}{val}#')
		self.get(var)

	def initiate(self):
		delay = 0.1
		self.set('tz', self.tz, delay=delay)
		self.get('date')
		self.set('date', self.date, delay=delay)
		self.set('time', self.time, delay=delay)
		self.set('lat', self.lat, delay=delay)
		self.set('lon', self.lon, delay=delay)

	def slew(self, rate, direction):		# Direction is e,w,n,s
		self.set('rate', rate)				# Slew rate is an int from XXX to XXX
		self.write(':RS#')
		self.write(f':M{direction}')


###
## Example code to initialize

mnt.disconnect()
mnt = losmandi_mount()
mnt.connect()
mnt.initiate()


for i in range(10):
	print(1*i)
	ra,dec = np.array([mnt.get('ra'), mnt.get('dec')]).T
	mnt.slew(1*i,'e')
	#time.sleep(0.5)









#		mnt.set('tz', mnt.tz)
		mnt.get('date')
		mnt.get('time')
		mnt.get('lat')
		mnt.get('lon')


		mnt.set('tz', mnt.tz)
		mnt.set('date', mnt.date)
		mnt.set('time', mnt.time)
		mnt.set('lat', mnt.lat)
		mnt.set('lon', mnt.lon)