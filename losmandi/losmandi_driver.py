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
		self.timeout = 1
		self.message = None
		self.port = os.popen('python -m serial.tools.list_ports').read().split('\n')[1][:5]
		self.tz = '-5'			# interval RTC uses UTC, timezone is defined by the offset from UTC
		self.date = time.strftime('%m/%Y/%d', time.localtime(time.time()))
		self.time = time.strftime('%H:%M:%S', time.localtime(time.time()))		#sync with pc local time
		self.lat = '045*62' 	# UdeS SIRENE station lat
		self.lon = '-071*15'		# UdeS SIRENE station lon

	def connect(self):
		self.mount = serial.Serial(self.port, self.baud, timeout=self.timeout)

	def close(self):
		self.mount.close()

	def write(self, mess=None, as_bytes=True):
		if mess:
			if as_bytes:
				self.mount.write(bytes(mess, 'ascii'))
			else: 
				self.mount.write(mess)
		else: 
			self.mount.write(bytes(self.message, 'ascii'))
			
		self.mount.write(bytes('\r\n', 'ascii'))

	def read(self):
		self.message = self.mount.read(100) 	#read up to 100 bytes
		print(self.message)

	def readline(self):
		self.message = self.mount.readline()	# read a '\n' terminated line
		print(self.message)

	def initiate(self):
		ans = input('Use custom location and time settings (defaults to Sherbrooke SIRENE station)? (y,[n])')
		if ans=='y':
			self.tz = input('Timezone as offset from UTC (format: int): \t')
			self.date = input('Local date (format: mm/dd/yy): \t')
			self.time = input('Local time (format: hh:mm:ss): \t')
			self.lat = input('Latitude (format: +-ddd*mm): \t')
			self.lon = input('Longitude (format: +-ddd*mm): \t')

		self.write(mess=f':SG{self.tz}#')
		print('tz')
		time.sleep(1)
		self.write(mess=f':SC{self.date}#')
		print('date')
		time.sleep(1)
		self.write(mess=f':SL{self.time}#')
		print('time')
		time.sleep(1)
		self.write(mess=f':St{self.lat}#')
		print('lat')
		time.sleep(1)
		self.write(mess=f':Sg{self.lon}#')
		print('lon')


mnt = losmandi_mount()
mnt.connect()
mnt.initiate()
