#!/usr/bin/python3 

# osc_in_out.py
# osc input/output + TouchOSC interfacing
# (c) Yukao Nagemi

from math import sin
from math import cos
from math import pi

import getopt
import sys

# control C interruption
from signal import signal, SIGINT

from time import sleep

# OSC
from pythonosc.udp_client import SimpleUDPClient
from pythonosc.osc_server import AsyncIOOSCUDPServer
from pythonosc.dispatcher import Dispatcher
import asyncio

# library to convert between strings and nums
sys.path.append('D:/sync.com/Sync/LYM-sources/vv/vv_python/')
from vv_lib import to_num

##################################################################
# USAGE
##################################################################
USAGE = '''osc_in_out.py -i configuration_file
	ARGUMENTS:
	-i: input configuration_file
'''

##################################################################
# PARAMETERS THAT COULD BE INCLUDED IN A CONFIGURATION FILE
##################################################################
TouchOSC_IP = "192.168.43.1"
TouchOSC_Port = 8000
OSCClient_IP = "127.0.0.1"
OSCClient_Port = 7000
OSCServer_Port = 9000

OSC_emission_timeStep = 0.02

##################################################################
# GLOBAL VARIABLES
##################################################################
Object_current = None
OSC_dispatcher = None

UDP_client_TouchOSC = None
UDP_client_OSCClient = None
UDP_server_OSCServer = None

##################################################################
# GEOMETRY CLASSES
##################################################################
class Vector:
	def __init__(self, x, y, z):
		self.vx = x
		self.vy = y
		self.vz = z

	def __add__(self, other):
		return Vector(self.vx + other.vx, self.vy + other.vy, self.vz + other.vz)

	def __sub__(self, other):
		return Vector(self.vx - other.vx, self.vy - other.vy, self.vz - other.vz)

	def __mul__(self, other):
		return Vector(self.vx * other, self.vy * other, self.vz * other)

	def dot(self, v):
		return self.vx * v.vx + self.vy * v.vy + self.vz * v.vz

	def norm(self):
		return sqrt(self.dot(self))

	def normalize(self):
		n = self.norm()
		if(n != 0):
			self.vx /= n
			self.vy /= n
			self.vz /= n
		return self

	def mul(self, k):
		self.vx *= k
		self.vy *= k
		self.vz *= k
		return self

class Point:
	def __init__(self, x, y, z):
		self.px = x
		self.py = y
		self.pz = z

	def __add__(self, v):
		return Point(self.px + v.vx, self.py + v.vy, self.pz + v.vz)

	def __sub__(self, other):
		return Vector(self.px - other.px, self.py - other.py, self.pz - other.pz)

##################################################################
# AN OBJECT
##################################################################
def circle(x_center, y_center, radius, angle):
	x = x_center + radius * cos(angle)
	y = y_center + radius * sin(angle)
	return [x,y]

class Object:
	# The system.
	def __init__(self):
		# Construct a system.
		self.x = 0
		self.y = 0
		self.angle = 0
		self.radius = 0.5
		self.angularspeed = 0.01

	# object updating
	def step(self):
		# updates angle
		self.angle = self.angle + self.angularspeed
		# updates position
		self.x, self.y = circle(0.5, 0.5, self.radius, self.angle)
		# sends OSC message
		UDP_emit("/pad", "udp_TouchOSC_send", (self.x, self.y))

##################################################################
# OSC COMMUNICATION
##################################################################
##################################################################
# OSC SERVERS AND CLIENTS
class UDP_server:
	def __init__(self, Port):
		self.port = Port

class UDP_client:
	def __init__(self, IP_address, Port):
		self.IP_address = IP_address
		self.port = Port
		self.client = SimpleUDPClient(IP_address, Port)

##################################################################
# SENSOR DISCONNECTION BEFORE EXITING
def disconnect():
	global UDP_server_OSCServer
	# OSC server closing
	UDP_server_OSCServer.transport.close()  # Clean up serve endpoint

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
##################################################################
##################################################################
# SYSTEM MESSAGES PROCESSING
def UDP_receive_quit_handler(address, *args):
	print("received OSC {", address, "}: {", args, "}")
	disconnect()
	exit(0)

##################################################################
# OBJECT MESSAGES PROCESSING
def UDP_receive_radius_handler(address, *args):
	global  Object_current
	print("received OSC {", address, "}: {", args, "}")
	Object_current.radius = args[0]
	UDP_emit(address, "udp_TouchOSC_send", Object_current.radius)

def UDP_receive_angularspeed_handler(address, *args):
	global  Object_current
	print("received OSC {", address, "}: {", args, "}")
	Object_current.angularspeed = args[0]
	UDP_emit(address, "udp_TouchOSC_send", Object_current.angularspeed)

##################################################################
# OSC MESSAGE RECEPTION DISPATCHING
def OSC_dispatecher_mapping():
	global OSC_dispatcher
	# SYSTEM MESSAGES PROCESSING
	OSC_dispatcher.map("/quit", UDP_receive_quit_handler)
	# OBJECT MESSAGES PROCESSING
	OSC_dispatcher.map("/radius", UDP_receive_radius_handler)
	OSC_dispatcher.map("/angularspeed", UDP_receive_angularspeed_handler)

##################################################################
# OSC COMMUNICATION
##################################################################
def	UDP_emit(address, UDP_client_ID, arg_list) :
	global UDP_client_TouchOSC
	global UDP_client_OSCClient
	if(UDP_client_ID == "udp_TouchOSC_send"):
		UDP_client_TouchOSC.client.send_message(address, arg_list)						
	elif(UDP_client_ID == "udp_OSCClient_send"):
		UDP_client_OSCClient.client.send_message(address, arg_list)						
	else:
		print('missing "', UDP_client_ID, '" UDP client')
		exit(0)

##################################################################
# OSC MESSAGE EMISSION DISPATCHING
def OSC_emitter_resend_all():
	global Object_current
	UDP_emit("/radius", "udp_TouchOSC_send", Object_current.radius)
	UDP_emit("/angularspeed", "udp_TouchOSC_send", Object_current.angularspeed)

##################################################################
# OSC ASYNC MESSAGE RECEPTION LOOP
async def OSC_loop():
	global Object_current
	global OSC_emission_timeStep

	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	"""Example main loop that only runs for 10 iterations before finishing"""
	while(True):
		# print("OSC loop")
		Object_current.step()
		await asyncio.sleep(OSC_emission_timeStep)

##################################################################
# OSC MAIN SUB
async def OSC_main():
	global OSC_dispatcher
	global UDP_server_OSCServer

	UDP_server_OSCServer.OSCServer = AsyncIOOSCUDPServer(('0.0.0.0', UDP_server_OSCServer.port), OSC_dispatcher, asyncio.get_event_loop())
	UDP_server_OSCServer.transport, UDP_server_OSCServer.protocol = await UDP_server_OSCServer.OSCServer.create_serve_endpoint()  # Create datagram endpoint and start serving
	print("Opening OSC server on Port: %d" % (UDP_server_OSCServer.port))

	await OSC_loop()  # Enter main loop of program

	UDP_server_OSCServer.transport.close()  # Clean up serve endpoint

##################################################################
# MAIN SUB
##################################################################
def handlerForInterruption(signal_received, frame):
	# Handle breaks
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	disconnect()
	exit(0)

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global Object_current
	global OSC_dispatcher

	global UDP_client_TouchOSC
	global UDP_client_OSCClient
	global UDP_server_OSCServer

	global TouchOSC_IP
	global TouchOSC_Port
	global OSCClient_IP
	global OSCClient_Port
	global OSCServer_Port

	##################################################################
	# ARGUMENTS: CONFIGURATION FILE
	##################################################################
	configuration_file_name = ''
	# scanning arguments in command line
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			configuration_file_name = arg
		else:
			assert False, "unhandled option"
	# if an input file is provided
	if(configuration_file_name != ""):
		print( 'Configuration file is ', configuration_file_name)
		try:
			FILEin = open(configuration_file_name, "rt")
		except IOError:
			print("File not found :", configuration_file_name, " or path is incorrect")

	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	##################################################################
	# INITIALIZATIONS
	# 
	Object_current = Object()
	UDP_client_TouchOSC = UDP_client(TouchOSC_IP, TouchOSC_Port)
	UDP_client_OSCClient = UDP_client(OSCClient_IP, 7000)
	UDP_server_OSCServer = UDP_server(OSCServer_Port)

	##################################################################
	# CALLBACKS FOR OSC MESSAGES
	# creates the OSC messages dispatcher used to define the servers in the sensor set initialization
	OSC_dispatcher = Dispatcher()
	# defines the callbacks for OSC messages
	OSC_dispatecher_mapping()

	##################################################################
	# INIIALIZES TouchOSC with the setup parameter values 
	OSC_emitter_resend_all()

	##################################################################
	# Runs the loop for receiving async OSC messages 
	asyncio.run(OSC_main())

	##################################################################
	# DISCONNECTION
	disconnect()

if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)
	main(sys.argv[1:])
