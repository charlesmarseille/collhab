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
					'lon':'g'}

	def connect(self):
		print(f'Connecting on port {self.port}')
		self.mount = serial.Serial(self.port, self.baud, timeout=self.timeout)
		self.mount.flush()

	def disconnect(self):
		self.mount.close()

	def write(self, message):
		self.mount.write(bytes(message, 'ascii'))
		time.sleep(0.1)
		self.read()

	def read(self):
		while self.mount.in_waiting:
			self.message = self.mount.readline().decode('ascii', 'replace')

	def get(self, var):
		self.write(f':G{self.lib[var]}#')		#output is in self.message variable
		print(f'{var}: {self.message}')

	def set(self, var, val):
		self.write(f':S{self.lib[var]}{val}#')
		self.get(var)

	def initiate(self):
		self.set('tz', self.tz)
		self.set('date', self.date)
		self.set('time', self.time)
		self.set('lat', self.lat)
		self.set('lon', self.lon)


###
# Test code

mnt = losmandi_mount()
mnt.connect()
mnt.initiate()