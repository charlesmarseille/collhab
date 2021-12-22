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
		self.timeout = 0.01
		self.message = None
		self.port = os.popen('python -m serial.tools.list_ports').read().split('\n')[1][:5]
		self.tz = '-5'			# interval RTC uses UTC, timezone is defined by the offset from UTC
		self.date = time.strftime('%m/%d/%y', time.localtime(time.time()))
		self.time = time.strftime('%H:%M:%S', time.localtime(time.time()))		#sync with pc local time
		self.lat = '045*62' 	# UdeS SIRENE station lat
		self.lon = '-071*15'		# UdeS SIRENE station lon
		self.test = None
		self.lib = {'tz':'GG',
			   		'date':'GC',
			   		'time':'GL',
			   		'lat':'Gt',
			   		'lon':'Gg'}

	def connect(self):
		print(f'Connecting on port {self.port}')
		self.mount = serial.Serial(self.port, self.baud, timeout=self.timeout)

	def disconnect(self):
		self.mount.close()

	def write(self, message):
		self.mount.write(bytes(message, 'ascii'))
		time.sleep(0.2)
		self.message = self.read()

	def read(self):
		while self.mount.in_waiting:
			self.message = self.mount.readline().decode('ascii', 'replace')
			print(self.message)

	def get(self, val):
		self.write(f':{self.lib[val]}#')		#output is in self.message variable

	def set(self, val):


	def initiate(self):
		# ans = input('Use custom location and time settings (defaults to Sherbrooke SIRENE station)? (y,[n])')
		# if ans=='y':
		# 	self.tz = input('Timezone as offset from UTC (format: int): \t')
		# 	self.date = input('Local date (format: mm/dd/yy): \t')
		# 	self.time = input('Local time (format: hh:mm:ss): \t')
		# 	self.lat = input('Latitude (format: +-ddd*mm): \t')
		# 	self.lon = input('Longitude (format: +-ddd*mm): \t')

		print('tz')
		self.write(f':SG{self.tz}#')
		self.get('tz')
		print('date')
		self.write(f':SC{self.date}#')
		self.get('date')
		print('time')
		self.write(f':SL{self.time}#')
		self.get('time')
		print('lat')
		self.write(f':St{self.lat}#')
		self.get('lat')
		print('lon')
		self.write(f':Sg{self.lon}#')
		self.get('lon')


mnt = losmandi_mount()
mnt.connect()
mnt.initiate()