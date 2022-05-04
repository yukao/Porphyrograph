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
from time import time

from datetime import datetime

# import asyncio
import threading

import logging
import csv


# OSC
from osc4py3.as_eventloop import *
from osc4py3 import oscmethod as osm
from osc4py3 import oscbuildparse

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
TouchOSC_IP = "192.168.137.120" # the IPaddress on the phone
TouchOSC_Port = 9000 # for TouchOSC on the phone
Spat_IP = "127.0.0.1" # for MAX/MSP (local, doesnt have to be changed)
Spat_Port = 7000 # for MAX/MSP (reciever in the max patch)
OSCServer_Port = 8000  # python server

OSC_emission_timeStep = 0.05
SourceSpeedFactor = 0.1
GlobalSourceSpeed = 0.
TrajectorySpeedFactor = 0.1
GlobalTrajectorySpeed = 0.
GlobalLastReload = 0

GlobalInterpolationDuration = 5.
GlobalStartInterpolation = 0.
GlobalEndInterpolation = 0.

##################################################################
# GLOBAL VARIABLES
##################################################################
Sources = dict()

##################################################################
# AN OBJECT
##################################################################
def circle(x_center, y_center, radius, source_angle):
	x = x_center + radius * cos(source_angle)
	y = y_center + radius * sin(source_angle)
	return [x,y]

def ellipse(x_center, y_center, radius, ratio, source_angle, trajectory_angle):
	x = x_center + radius * cos(source_angle)
	y = y_center + ratio * radius * sin(source_angle)
	X = x * cos(trajectory_angle) - y  * sin(trajectory_angle)
	Y = x * sin(trajectory_angle) + y  * cos(trajectory_angle)
	return [X,Y]

class Source:
	# The system.
	def __init__(self, ind):
		# Construct a system.
		self.x = 0
		self.y = 0
		self.source_angle_ini = 0
		self.trajectory_angle_ini = 0

		self.sourcespeed = 0.
		self.trajectoryspeed = 0.
		self.radius = 0.5
		self.ratio = 0.5
		self.source_angle = 0
		self.trajectory_angle = 0

		self.old_sourcespeed = 0.
		self.old_trajectoryspeed = 0.
		self.old_radius = 0.5
		self.old_ratio = 0.5
		self.old_source_angle = 0
		self.old_trajectory_angle = 0
		self.old_source_angle_ini = 0
		self.old_trajectory_angle_ini = 0

		self.indSource = ind + 1
		self.onTouchOSC = False
		self.freeze = False

	# object updating
	def step(self, indStep):
		global GlobalSourceSpeed
		global GlobalTrajectorySpeed
		global GlobalStartInterpolation
		global GlobalEndInterpolation

		if(not self.freeze) :
			currentTime = time()
			if(currentTime >= GlobalStartInterpolation and currentTime <= GlobalEndInterpolation) :
				percentage = float(currentTime - GlobalStartInterpolation) / (GlobalEndInterpolation - GlobalStartInterpolation)
				message = oscbuildparse.OSCMessage("/reload_countdown", None, [int(GlobalEndInterpolation - currentTime)])
				# print("message:", "/soure/"+str(self.indSource)+"/xy", self.x, " ", self.y)
				osc_send(message, "TouchOSC_client")

				# updates source_angle
				self.old_source_angle = self.old_source_angle + (self.old_sourcespeed * GlobalSourceSpeed)
				# updates trajectory_angle
				self.old_trajectory_angle = self.old_trajectory_angle + (self.old_trajectoryspeed * GlobalTrajectorySpeed)
				# updates position
				old_x, old_y = ellipse(0, 0, self.old_radius, self.old_ratio, \
					self.old_source_angle + self.old_source_angle_ini, self.old_trajectory_angle + self.old_trajectory_angle_ini)
				# updates source_angle
				self.source_angle = self.source_angle + (self.sourcespeed * GlobalSourceSpeed)
				# updates trajectory_angle
				self.trajectory_angle = self.trajectory_angle + (self.trajectoryspeed * GlobalTrajectorySpeed)
				# updates position
				new_x, new_y = ellipse(0, 0, self.radius, self.ratio, \
					self.source_angle + self.source_angle_ini, self.trajectory_angle + self.trajectory_angle_ini)
				self.x = (1 - percentage) * old_x + percentage * new_x
				self.y = (1 - percentage) * old_y + percentage * new_y
			else :
				# updates source_angle
				self.source_angle = self.source_angle + (self.sourcespeed * GlobalSourceSpeed)
				# updates trajectory_angle
				self.trajectory_angle = self.trajectory_angle + (self.trajectoryspeed * GlobalTrajectorySpeed)
				# updates position
				self.x, self.y = ellipse(0, 0, self.radius, self.ratio, \
					self.source_angle + self.source_angle_ini, self.trajectory_angle + self.trajectory_angle_ini)
			# print("coordinates:", self.x, " ", self.y)

		# sends OSC message
		if(indStep % 4 == 0 and self.onTouchOSC == True) :
			message = oscbuildparse.OSCMessage("/source/"+str(self.indSource)+"/xy", None, [self.x, self.y])
			# print("message:", "/soure/"+str(self.indSource)+"/xy", self.x, " ", self.y)
			osc_send(message, "TouchOSC_client")

		# builds OSC message
		self.messageToSpat = oscbuildparse.OSCMessage("/source/"+str(self.indSource)+"/xyz", None, [self.y, self.x, 0])

##################################################################
# OSC COMMUNICATION
##################################################################
##################################################################
# SENSOR DISCONNECTION BEFORE EXITING
def disconnect():
	# Properly close the system.
	osc_terminate()

##################################################################
# MAIN SUB
##################################################################
def record_params() :
	global GlobalSourceSpeed
	global GlobalTrajectorySpeed

	now = datetime.now() # current date and time
	date_time = now.strftime("_%m-%d-%Y_%H-%M-%S_")
	print("date and time:",date_time)
	print("log",'F:/Ableton/TouchOSC_2020/spat_default_setups/setup_record'+date_time+'.csv')
	with open('F:/Ableton/TouchOSC_2020/spat_default_setups/setup_record'+date_time+'.csv', 'wt', newline='') as csvfile:
		logwriter = csv.writer(csvfile, delimiter=',')
		values = ['globalsourcespeed', 'globaltrajectoryspeed']
		for source in list(Sources.values()) :
			values = values + ['sourcespeed'+str(source.indSource), 'trajectoryspeed'+str(source.indSource), \
			'radius'+str(source.indSource), 'ratio'+str(source.indSource), 'source_angle'+str(source.indSource), \
			'trajectory_angle'+str(source.indSource)]
		logwriter.writerow(values)
		values = ["%.3f" % GlobalSourceSpeed, "%.3f" %  GlobalTrajectorySpeed]
		for source in list(Sources.values()) :
			values = values + ["%.3f" % source.sourcespeed, "%.3f" % source.trajectoryspeed, \
			"%.3f" % source.radius, "%.3f" % source.ratio, "%.3f" % source.source_angle, \
			"%.3f" % source.trajectory_angle]
		logwriter.writerow(values)
	csvfile.close()

def reload_params(indParam) :
	global GlobalSourceSpeed
	global GlobalTrajectorySpeed
	global GlobalStartInterpolation
	global GlobalEndInterpolation

	print("log",'F:/Ableton/TouchOSC_2020/spat_default_setups/setup.csv')
	with open( 'F:/Ableton/TouchOSC_2020/spat_default_setups/setup.csv', 'rt', newline='') as csvfile:
		logreader = csv.reader(csvfile, delimiter=',')
		lineNo = 0
		for row in logreader:
			if(lineNo == indParam) :
				GlobalSourceSpeed = to_num(row[0])
				GlobalTrajectorySpeed = to_num(row[1])
				indSource = 0
				for source in list(Sources.values()) :
					source.old_sourcespeed = source.sourcespeed
					source.old_trajectoryspeed = source.trajectoryspeed
					source.old_radius = source.radius
					source.old_ratio = source.ratio
					source.old_source_angle = source.source_angle
					source.old_trajectory_angle = source.trajectory_angle
					source.old_source_angle_ini = source.source_angle_ini
					source.old_trajectory_angle_ini = source.trajectory_angle_ini
					source.sourcespeed = to_num(row[indSource * 6 + 2 + 0])
					source.trajectoryspeed = to_num(row[indSource * 6 + 2 + 1])
					source.radius = to_num(row[indSource * 6 + 2 + 2])
					source.ratio = to_num(row[indSource * 6 + 2 + 3])
					source.source_angle = to_num(row[indSource * 6 + 2 + 4])
					source.trajectory_angle = to_num(row[indSource * 6 + 2 + 5])
					indSource += 1
				OSC_emitter_resend_all()
				GlobalStartInterpolation = time()
				GlobalEndInterpolation = GlobalStartInterpolation + GlobalInterpolationDuration
				break
			lineNo += 1
	csvfile.close()
	print('reloaded')

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
##################################################################
##################################################################
# SYSTEM MESSAGES PROCESSING
def UDP_receive_quit_handler(address, arg):
	# print("received OSC radius: {", address, " ", arg, "}")
	disconnect()
	sys.exit(0)
def UDP_receive_record_handler(address, arg):
	print("received record: {", address, " ", arg, "}")
	if(to_num(arg) == 1) :
		record_params()
def UDP_receive_reload_handler(address, arg):
	global GlobalLastReload

	print("received reload: {", address, " ", arg, "}")
	GlobalLastReload = to_num(arg)
	if(to_num(arg) >= 1) :
		reload_params(GlobalLastReload)
	msg = oscbuildparse.OSCMessage(address, None, [(GlobalLastReload + 1)])
	osc_send(msg, "TouchOSC_client")

##################################################################
# SPAT MESSAGES
def UDP_receive_room_handler(address, arg):
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")

##################################################################
# OBJECT MESSAGES PROCESSING
def UDP_receive_radius_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSrc = int(split_add[2])
	# print("address arg source", address, float(arg), indSrc)
	Sources[indSrc - 1].radius = float(arg)
	msg = oscbuildparse.OSCMessage(address, None, [Sources[indSrc - 1].radius])
	osc_send(msg, "TouchOSC_client")

def UDP_receive_ratio_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSrc = int(split_add[2])
	Sources[indSrc - 1].ratio = float(arg)
	msg = oscbuildparse.OSCMessage(address, None, [Sources[indSrc - 1].ratio])
	osc_send(msg, "TouchOSC_client")

def UDP_receive_sourcespeed_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSrc = int(split_add[2])
	Sources[indSrc - 1].sourcespeed = float(arg)
	msg = oscbuildparse.OSCMessage(address, None, [Sources[indSrc - 1].sourcespeed])
	osc_send(msg, "TouchOSC_client")
	# print("source speed",indSrc, Sources[indSrc - 1].sourcespeed)

def UDP_globalsourcespeed(address, arg):
	global  Sources
	global GlobalSourceSpeed
	GlobalSourceSpeed = float(arg) * SourceSpeedFactor
	msg = oscbuildparse.OSCMessage(address, None, [GlobalSourceSpeed / SourceSpeedFactor])
	osc_send(msg, "TouchOSC_client")
	# print(GlobalSourceSpeed / SourceSpeedFactor)

def UDP_receive_trajectoryspeed_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSrc = int(split_add[2])
	Sources[indSrc - 1].trajectoryspeed = float(arg)
	msg = oscbuildparse.OSCMessage(address, None, [Sources[indSrc - 1].trajectoryspeed])
	osc_send(msg, "TouchOSC_client")
	# print("trajectory speed",indSrc, Sources[indSrc - 1].trajectoryspeed)

def UDP_globaltrajectoryspeed(address, arg):
	global  Sources
	global GlobalTrajectorySpeed
	GlobalTrajectorySpeed = float(arg) * TrajectorySpeedFactor
	msg = oscbuildparse.OSCMessage(address, None, [GlobalTrajectorySpeed / TrajectorySpeedFactor])
	osc_send(msg, "TouchOSC_client")

def UDP_receive_onTouchOSC_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSource = int(split_add[2])
	for indSrc in range(5) :
		Sources[indSrc].onTouchOSC = False
	Sources[indSource - 1].onTouchOSC = True
	# print("Source ON", indSource)
	msg = oscbuildparse.OSCMessage(address, None, [Sources[indSrc - 1].onTouchOSC])
	osc_send(msg, "TouchOSC_client")

def UDP_receive_freeze_handler(address, arg):
	global  Sources
	split_add = address.split('/')
	indSource = int(split_add[2])
	Sources[indSource - 1].freeze = not Sources[indSource - 1].freeze
	# print("Source ON", indSource)
	msg = oscbuildparse.OSCMessage(address, None, [int(Sources[indSrc - 1].freeze)])
	osc_send(msg, "TouchOSC_client")

def UDP_receive_freeze_all_handler(address, arg):
	global  Sources
	for indSrc in range(5) :
		Sources[indSrc].freeze = True
		msg = oscbuildparse.OSCMessage("/source/"+str(indSrc + 1)+"/freeze", None, [1])
		osc_send(msg, "TouchOSC_client")

def UDP_receive_defreeze_all_handler(address, arg):
	global  Sources
	for indSrc in range(5) :
		Sources[indSrc].freeze = False
		msg = oscbuildparse.OSCMessage("/source/"+str(indSrc + 1)+"/freeze", None, [0])
		osc_send(msg, "TouchOSC_client")

def UDP_receive_xy_handler(address, arg1, arg2, arg3):
	global  Sources
	print("received OSC spat: {", address, " ", arg1, arg2, arg3, "}")

##################################################################
# OSC MESSAGE RECEPTION DISPATCHING
def OSC_dispatcher_mapping():
	# Associate Python functions with message address patterns, using default
	# argument scheme OSCARG_DATAUNPACK.
	# SYSTEM MESSAGES PROCESSING
	osc_method("/quit", UDP_receive_quit_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/record", UDP_receive_record_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/reload", UDP_receive_reload_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	# OBJECT MESSAGES PROCESSING
	osc_method("/source/*/radius", UDP_receive_radius_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/ratio", UDP_receive_ratio_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/sourcespeed", UDP_receive_sourcespeed_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/globalsourcespeed", UDP_globalsourcespeed, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/trajectoryspeed", UDP_receive_trajectoryspeed_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/globaltrajectoryspeed", UDP_globaltrajectoryspeed, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/xy", UDP_receive_xy_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/onTouchOSC", UDP_receive_onTouchOSC_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/freeze", UDP_receive_freeze_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/freeze_all", UDP_receive_freeze_all_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/defreeze_all", UDP_receive_defreeze_all_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/room/*", UDP_receive_room_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	osc_method("/source/*/room/*", UDP_receive_room_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)

##################################################################
# OSC COMMUNICATION
##################################################################
##################################################################
# OSC MESSAGE EMISSION DISPATCHING
def OSC_emitter_resend_all():
	global Sources
	global GlobalSourceSpeed
	global SourceSpeedFactor
	global GlobalTrajectorySpeed
	global GlobalLastReload
	for indSrc, source in Sources.items() :
		msg = oscbuildparse.OSCMessage("/source/"+str(source.indSource)+"/radius", None, [source.radius])
		osc_send(msg, "TouchOSC_client")
		msg = oscbuildparse.OSCMessage("/source/"+str(source.indSource)+"/ratio", None, [source.ratio])
		osc_send(msg, "TouchOSC_client")
		msg = oscbuildparse.OSCMessage("/source/"+str(source.indSource)+"/sourcespeed", None, [source.sourcespeed])
		osc_send(msg, "TouchOSC_client")
		msg = oscbuildparse.OSCMessage("/source/"+str(source.indSource)+"/trajectoryspeed", None, [source.trajectoryspeed])
		osc_send(msg, "TouchOSC_client")
		msg = oscbuildparse.OSCMessage("/source/"+str(source.indSource)+"/freeze", None, [int(source.freeze)])
		osc_send(msg, "TouchOSC_client")
	msg = oscbuildparse.OSCMessage("/globalsourcespeed", None, [GlobalSourceSpeed / SourceSpeedFactor])
	osc_send(msg, "TouchOSC_client")
	msg = oscbuildparse.OSCMessage("/globaltrajectoryspeed", None, [GlobalTrajectorySpeed / TrajectorySpeedFactor])
	osc_send(msg, "TouchOSC_client")
	msg = oscbuildparse.OSCMessage("/reload", None, [(GlobalLastReload + 1)])
	osc_send(msg, "TouchOSC_client")

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
	global Sources
	global OSC_dispatcher

	global TouchOSC_IP
	global TouchOSC_Port
	global Spat_IP
	global Spat_Port
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
	for ind in range(5) :
		Sources[ind] = Source(ind)

	# Start the system.
	osc_startup()

	# clients
	print("The python program opens OSC client TouchOSC on Address %s Port: %d" % (TouchOSC_IP, TouchOSC_Port))
	osc_udp_client(TouchOSC_IP, TouchOSC_Port, "TouchOSC_client")
	print("The python program opens OSC client Spat on Address %s Port: %d" % (Spat_IP, Spat_Port))
	osc_udp_client(Spat_IP, Spat_Port, "Spat_client")

	# server
	osc_udp_server("0.0.0.0", OSCServer_Port, "Local_server")
	print("The python program opens OSC server on Port: %d" % (OSCServer_Port))

	##################################################################
	# CALLBACKS FOR OSC MESSAGES
	# defines the callbacks for OSC messages
	OSC_dispatcher_mapping()

	##################################################################
	# Runs the loop for receiving async OSC messages 
	print("The python program runs... type control C in console or Stop in interface to stop the program")
	# msg = oscbuildparse.OSCMessage("/radius", None, [0.5])
	# osc_send(msg, "TouchOSC_client")

	##################################################################
	# INIIALIZES TouchOSC with the setup parameter values 
	OSC_emitter_resend_all()

	
	"""Example main loop that only runs for 10 iterations before finishing"""
	indstep = 0
	while(True):
		# print("OSC loop")
		msg_list = []
		for source in list(Sources.values()) :
			source.step(indstep)
			msg_list.append(source.messageToSpat)
		exectime = time() - 10.0 # execute in 10 seconds
		bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
		osc_send(bun, "Spat_client")

		for i in range(30) :
			osc_process()
		sleep(OSC_emission_timeStep)
		indstep += 1

	##################################################################
	# DISCONNECTION
	disconnect()

if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)
	main(sys.argv[1:])
