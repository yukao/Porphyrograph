#!/usr/bin/python3 

# osc_in_out.py
# osc input/output + wx interfacing
# (c) Yukao Nagemi

from math import sin
from math import cos
from math import pi

import getopt
import sys
sys.path.append('C:/sync.com/Sync/LYM-sources/vv/vv_python/')

# control C interruption
from signal import signal, SIGINT

from time import sleep
from time import time

from datetime import datetime

# import asyncio
import threading

import logging
import csv

import os

import wx
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
Spat_IP = "127.0.0.1" # for MAX/MSP (local, doesnt have to be changed)
Spat_Port = 7000 # for MAX/MSP (reciever in the max patch)

NumberOfSources = 5
NumberOfRooms = 3

SpeakerRadius = 1

SetupRecordPath = 'F:/Ableton/TouchOSC_2020/spat_default_setups/'
# SetupRecordPath = 'C:/sync.com/Sync/LYM-sources/Python/spat/'
SetupReplayFile = 'F:/Ableton/TouchOSC_2020/spat_default_setups/setup_record_08-08-2020_19-00-21_reallynice_with3spirals2circles.csv'
# SetupReplayFile = 'C:/sync.com/Sync/LYM-sources/Python/spat/setup_record_08-08-2020_19-00-21_reallynice_with3spirals2circles.csv'

OSC_emission_timeStep = 0.05
SourceSpeedFactor = 0.1
GlobalSourceSpeed = 0.
TrajectorySpeedFactor = 0.1
GlobalTrajectorySpeed = 0.
GlobalLastReload = 0

GlobalInterpolationDuration = 5.
GlobalStartInterpolation = 0.
GlobalEndInterpolation = 0.

frame = None
panel = None
indstep = 0

##################################################################
# GLOBAL VARIABLES
##################################################################
Sources = dict()
Rooms = dict()

##################################################################
# AN OBJECT
##################################################################
def circle(x_center, y_center, radius, source_angle):
	x = x_center + radius * cos(source_angle) * SpeakerRadius
	y = y_center + radius * sin(source_angle) * SpeakerRadius
	return [x,y]

def ellipse(x_center, y_center, radius, ratio, source_angle, trajectory_angle):
	x = x_center + radius * cos(source_angle) * SpeakerRadius
	y = y_center + ratio * radius * sin(source_angle) * SpeakerRadius
	X = x * cos(trajectory_angle) - y  * sin(trajectory_angle)
	Y = x * sin(trajectory_angle) + y  * cos(trajectory_angle)
	return [X,Y]

class Room:
	# The system.
	def __init__(self, ind):
		self.roomsize = 0.
		self.reverb = 0.
		self.heaviness = 0.
		self.liveness = 0.

class Source:
	# The system.
	def __init__(self, ind):
		# Construct a system.
		self.x = 0
		self.y = 0
		self.source_angle_ini = 0
		self.trajectory_angle_ini = 0

		self.sourcespeed = 0
		self.trajectoryspeed = 0
		self.radius = 0.5
		self.ratio = 0.5
		self.source_angle = 0
		self.trajectory_angle = 0

		self.old_sourcespeed = 0
		self.old_trajectoryspeed = 0
		self.old_radius = 0.5
		self.old_ratio = 0.5
		self.old_source_angle = 0
		self.old_trajectory_angle = 0
		self.old_source_angle_ini = 0
		self.old_trajectory_angle_ini = 0

		self.indSource = ind + 1
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
				# message = oscbuildparse.OSCMessage("/reload_countdown", None, [int(GlobalEndInterpolation - currentTime)])
				# # print("message:", "/soure/"+str(self.indSource)+"/xy", self.x, " ", self.y)
				# osc_send(message, "TouchOSC_client")
				panel.t6_2.SetValue(str(int(GlobalEndInterpolation - currentTime)))

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

		# builds OSC message
		self.messageToSpat = oscbuildparse.OSCMessage("/source/"+str(self.indSource)+"/xyz", None, [self.y, self.x, 0])

##################################################################
# INTERFACE
##################################################################
class MyFrame(wx.Frame):
	def __init__(self, parent, title):
		super(MyFrame, self).__init__(parent, title =title, size = (800,600))


		self.panel = MyPanel(self)


class MyPanel(wx.Panel):
	def __init__(self, parent):
		global  Sources
		global  Rooms

		super(MyPanel, self).__init__(parent)
		"""
		class MyPanel creates a panel with 2 sliders on it, inherits wx.Panel
		putting your components/widgets on a panel gives additional versatility
		"""
		self.SetBackgroundColour("white")

		self.timer = wx.Timer(self)
		self.Bind(wx.EVT_TIMER, self.update, self.timer)
		self.timer.Start(10)

		fontLarge = wx.Font(32, family = wx.DEFAULT, style = wx.NORMAL, weight = wx.BOLD, faceName = 'Consolas')
		fontMedium = wx.Font(16, family = wx.DEFAULT, style = wx.NORMAL, weight = wx.NORMAL, faceName = 'Consolas')
		fontSmall = wx.Font(12, family = wx.DEFAULT, style = wx.NORMAL, weight = wx.NORMAL, faceName = 'Consolas')

		self.t1 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 0, 0), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t1.SetValue("RADIUS")
		self.t1.SetFont(fontSmall)
		self.t2 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 1, 0), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t2.SetValue("SOURCE SPEED")
		self.t2.SetFont(fontSmall)
		self.t3 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 2, 0), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t3.SetValue("TRAJECTORY SPEED")
		self.t3.SetFont(fontSmall)
		self.t4 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 3, 0), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t4.SetValue("RATIO")
		self.t4.SetFont(fontSmall)

		# wx.SL_VERTICAL  displays the slider vertically
		# wx.SL_HORIZONTAL  displays the slider horizontally
		# wx.SL_AUTOTICKS  displays tick marks
		# wx.SL_LABELS  displays minimum, maximum and value labels
		# initial value = 50, min value = 0, max value = 100
		self.sliders_radius = []
		self.sliders_sourcespeed = []
		self.sliders_trajectoryspeed = []
		self.sliders_ratio = []
		for indSrc in range(NumberOfSources) :
			self.sliders_radius.append(wx.Slider(self, -1, int(Sources[indSrc].radius * 100), 0, 100, (10 + 320 * 0, 30 + 50 * indSrc), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_sourcespeed.append(wx.Slider(self, -1, int(Sources[indSrc].sourcespeed), 2, 9, (10 + 320 * 1, 30 + 50 * indSrc), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_trajectoryspeed.append(wx.Slider(self, -1, int(Sources[indSrc].trajectoryspeed), 2, 9, (10 + 320 * 2, 30 + 50 * indSrc), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_ratio.append(wx.Slider(self, -1, int(Sources[indSrc].ratio * 100), 0, 100, (10 + 320 * 3, 30 + 50 * indSrc), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))

		self.t5 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 1, 50 + 50 * (NumberOfSources + 0)), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t5.SetValue("SOURCE SPEED SCALE")
		self.t5.SetFont(fontSmall)
		self.t6 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 2, 50 + 50 * (NumberOfSources + 0)), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t6.SetValue("TRAJECTORY SPEED SCALE")
		self.t6.SetFont(fontSmall)

		self.slider_sourcespeed_scale = wx.Slider(self, -1, int(GlobalSourceSpeed * 1000), 0, 100, (10 + 320 * 1, 30 + 50 * (NumberOfSources + 1)), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS)
		self.slider_trajectoryspeed_scale = wx.Slider(self, -1, int(GlobalTrajectorySpeed * 1000), 0, 100, (10 + 320 * 2, 30 + 50 * (NumberOfSources + 1)), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS)

		# self.btn1 = wx.Button(self, -1, "QUIT", size = wx.Size(300, 50), pos = (10 + 320 * 0, 30 + 50 * (NumberOfSources + 2)))
		# self.btn1.SetBackgroundColour("red")
		self.btn2 = wx.Button(self, -1, "FREEZE", size = wx.Size(150, 50), pos = (10 + 170 * 0, 30 + 50 * (NumberOfSources + 1)))
		self.btn2.SetBackgroundColour("blue")
		self.btn2.SetFont(fontMedium)
		self.btn3 = wx.Button(self, -1, "DE-FREEZE", size = wx.Size(150, 50), pos = (10 + 170 * 0, 30 + 50 * (NumberOfSources + 2)))
		self.btn3.SetBackgroundColour("yellow")
		self.btn3.SetFont(fontMedium)
		self.t6_3 = wx.TextCtrl(self, -1, size = wx.Size(150, 20), pos = (10 + 170 * 1, 60 + 50 * (NumberOfSources + 1)), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t6_3.SetValue("COUNTDOWN")
		self.t6_3.SetFont(fontSmall)
		self.t6_2 = wx.TextCtrl(self, -1, size = wx.Size(150, 50), pos = (10 + 170 * 1, 30 + 50 * (NumberOfSources + 2)), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t6_2.SetBackgroundColour("purple")
		self.t6_2.SetFont(fontLarge)
		self.t6_2.SetValue("0")

		self.btn4 = wx.Button(self, -1, "RECORD", size = wx.Size(150, 50), pos = (10 + 170 * 0, 30 + 50 * (NumberOfSources + 3)))
		self.btn4.SetBackgroundColour("green")
		self.btn4.SetFont(fontMedium)
		self.btn5 = wx.Button(self, -1, "RELOAD 1", size = wx.Size(150, 50), pos = (10 + 170 * 1, 30 + 50 * (NumberOfSources + 3)))
		self.btn5.SetBackgroundColour("white")
		self.btn5.SetFont(fontMedium)
		self.btn6 = wx.Button(self, -1, "RELOAD 2", size = wx.Size(150, 50), pos = (10 + 170 * 2, 30 + 50 * (NumberOfSources + 3)))
		self.btn6.SetBackgroundColour("white")
		self.btn6.SetFont(fontMedium)
		self.btn7 = wx.Button(self, -1, "RELOAD 3", size = wx.Size(150, 50), pos = (10 + 170 * 3, 30 + 50 * (NumberOfSources + 3)))
		self.btn7.SetBackgroundColour("white")
		self.btn7.SetFont(fontMedium)
		self.btn8 = wx.Button(self, -1, "RELOAD 4", size = wx.Size(150, 50), pos = (10 + 170 * 4, 30 + 50 * (NumberOfSources + 3)))
		self.btn8.SetBackgroundColour("white")
		self.btn8.SetFont(fontMedium)
		self.btn9 = wx.Button(self, -1, "RELOAD 5", size = wx.Size(150, 50), pos = (10 + 170 * 5, 30 + 50 * (NumberOfSources + 3)))
		self.btn9.SetBackgroundColour("white")
		self.btn9.SetFont(fontMedium)
		self.btn10 = wx.Button(self, -1, "RELOAD 6", size = wx.Size(150, 50), pos = (10 + 170 * 6, 30 + 50 * (NumberOfSources + 3)))
		self.btn10.SetBackgroundColour("white")
		self.btn10.SetFont(fontMedium)

		self.t7 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 0, 510), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t7.SetValue("ROOM SIZE")
		self.t7.SetFont(fontSmall)
		self.t8 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 1, 510), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t8.SetValue("REVERB")
		self.t8.SetFont(fontSmall)
		self.t9 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 2, 510), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t9.SetValue("HEAVINESS")
		self.t9.SetFont(fontSmall)
		self.t10 = wx.TextCtrl(self, -1, size = wx.Size(300, 20), pos = (10 + 320 * 3, 510), style = wx.TE_READONLY | wx.TE_CENTER | wx.BORDER_NONE)
		self.t10.SetValue("LIVENESS")
		self.t10.SetFont(fontSmall)

		self.sliders_roomsize = []
		self.sliders_reverb = []
		self.sliders_heaviness = []
		self.sliders_liveness = []
		for indRoom in range(NumberOfRooms) :
			self.sliders_roomsize.append(wx.Slider(self, -1, int(Rooms[indRoom].roomsize * 100), 10, 15000, (10 + 320 * 0, 545 + 50 * indRoom), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_reverb.append(wx.Slider(self, -1, int(Rooms[indRoom].reverb), 0, 100, (10 + 320 * 1, 545 + 50 * indRoom), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_heaviness.append(wx.Slider(self, -1, int(Rooms[indRoom].heaviness), 0, 50, (10 + 320 * 2, 545 + 50 * indRoom), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))
			self.sliders_liveness.append(wx.Slider(self, -1, int(Rooms[indRoom].liveness * 100), 0, 50, (10 + 320 * 3, 545 + 50 * indRoom), (300, 50), wx.SL_HORIZONTAL | wx.SL_AUTOTICKS | wx.SL_LABELS))

		# respond to changes in slider position ...
		self.Bind(wx.EVT_SLIDER, self.sliderUpdate)

		# respond to changes in buttons ...
		self.Bind(wx.EVT_BUTTON,self.OnClicked)

	def sliderUpdate(self, event):
		for indSrc in range(NumberOfSources) :
			wx_slider_radius_handler(self.sliders_radius[indSrc].GetValue()/100.0, indSrc)
			wx_slider_sourcespeed_handler(self.sliders_sourcespeed[indSrc].GetValue(), indSrc)
			wx_slider_trajectoryspeed_handler(self.sliders_trajectoryspeed[indSrc].GetValue(), indSrc)
			wx_slider_ratio_handler(self.sliders_ratio[indSrc].GetValue()/100.0, indSrc)
		wx_slider_globalsourcespeed_handler(self.slider_sourcespeed_scale.GetValue()/1000.0)
		wx_slider_globaltrajectoryspeed_handler(self.slider_trajectoryspeed_scale.GetValue()/1000.0)
		for indRoom in range(NumberOfRooms) :
			wx_slider_roomsize_handler(self.sliders_roomsize[indRoom].GetValue(), indRoom)
			wx_slider_reverb_handler(self.sliders_reverb[indRoom].GetValue(), indRoom)
			wx_slider_heaviness_handler(self.sliders_heaviness[indRoom].GetValue(), indRoom)
			wx_slider_liveness_handler(self.sliders_liveness[indRoom].GetValue(), indRoom)

	def sliderIsUpdated(self):
		for indSrc in range(NumberOfSources) :
			self.sliders_radius[indSrc].SetValue(int(Sources[indSrc].radius*100.0))
			self.sliders_sourcespeed[indSrc].SetValue(int(Sources[indSrc].sourcespeed))
			self.sliders_trajectoryspeed[indSrc].SetValue(int(Sources[indSrc].trajectoryspeed))
			self.sliders_ratio[indSrc].SetValue(int(Sources[indSrc].ratio*100.0))
		self.slider_sourcespeed_scale.SetValue(int(GlobalSourceSpeed*1000.0))
		self.slider_trajectoryspeed_scale.SetValue(int(GlobalTrajectorySpeed*1000.0))
		for indRoom in range(NumberOfRooms) :
			self.sliders_roomsize[indRoom].SetValue(int(Rooms[indRoom].roomsize))
			self.sliders_reverb[indRoom].SetValue(int(Rooms[indRoom].reverb))
			self.sliders_heaviness[indRoom].SetValue(int(Rooms[indRoom].heaviness))
			self.sliders_liveness[indRoom].SetValue(int(Rooms[indRoom].liveness))

	def OnClicked(self, event):
		btn = event.GetEventObject().GetLabel()
		print("Label of pressed button = ", btn)
		if(btn == "FREEZE"):
			wx_slider_freeze_all_handler()
		if(btn == "DE-FREEZE"):
			wx_slider_defreeze_all_handler()
		if(btn == "QUIT"):
			wx_slider_receive_quit_handler()
		if(btn == "RECORD"):
			wx_slider_receive_record_handler()
		if(btn == "RELOAD 1"):
			wx_slider_receive_reload_handler(1)
		if(btn == "RELOAD 2"):
			wx_slider_receive_reload_handler(2)
		if(btn == "RELOAD 3"):
			wx_slider_receive_reload_handler(3)
		if(btn == "RELOAD 4"):
			wx_slider_receive_reload_handler(4)
		if(btn == "RELOAD 5"):
			wx_slider_receive_reload_handler(5)
		if(btn == "RELOAD 6"):
			wx_slider_receive_reload_handler(6)

	def update(self, evt):
		global indstep
		msg_list = []
		for source in list(Sources.values()) :
			source.step(indstep)
			msg_list.append(source.messageToSpat)
			# print("messageToSpat:", source.messageToSpat)
		exectime = time() - 10.0 # execute in 10 seconds
		bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
		osc_send(bun, "Spat_client")

		for i in range(30) :
			osc_process()
		sleep(OSC_emission_timeStep)
		indstep += 1

		self.timer.Stop()
		self.timer.Start(10)

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
	print("log",os.path.join(SetupRecordPath,'setup_record'+date_time+'.csv'))
	with open(os.path.join(SetupRecordPath,'setup_record'+date_time+'.csv'), 'wt', newline='') as csvfile:
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
	global panel

	print("log",SetupReplayFile)
	with open( SetupReplayFile, 'rt', newline='') as csvfile:
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
				GlobalStartInterpolation = time()
				GlobalEndInterpolation = GlobalStartInterpolation + GlobalInterpolationDuration
				break
			lineNo += 1
	csvfile.close()
	print('reloaded')
	panel.sliderIsUpdated()

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
##################################################################
##################################################################
# SYSTEM MESSAGES PROCESSING
def wx_slider_receive_quit_handler():
	# print("received OSC radius: {", address, " ", arg, "}")
	disconnect()
	sys.exit(0)
def wx_slider_receive_record_handler():
	record_params()
def wx_slider_receive_reload_handler(arg):
	global GlobalLastReload
	GlobalLastReload = to_num(arg)
	if(to_num(arg) >= 1) :
		reload_params(GlobalLastReload)

##################################################################
# SPAT MESSAGES
def wx_slider_receive_room_handler(address, arg):
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")

##################################################################
# OBJECT MESSAGES PROCESSING

############ SOURCE SLIDERS
def wx_slider_radius_handler(arg, indSrc):
	global  Sources
	# print("address arg source", address, float(arg), indSrc)
	Sources[indSrc].radius = float(arg)

def wx_slider_sourcespeed_handler(arg, indSrc):
	global  Sources
	Sources[indSrc].sourcespeed = arg
	# print("source speed",indSrc, Sources[indSrc].sourcespeed)

def wx_slider_trajectoryspeed_handler(arg, indSrc):
	global  Sources
	Sources[indSrc].trajectoryspeed = arg
	# print("trajectory speed",indSrc, Sources[indSrc].trajectoryspeed)

def wx_slider_ratio_handler(arg, indSrc):
	global  Sources
	Sources[indSrc].ratio = float(arg)

def wx_slider_globaltrajectoryspeed_handler(arg):
	global  Sources
	global GlobalTrajectorySpeed
	GlobalTrajectorySpeed = float(arg)

def wx_slider_globalsourcespeed_handler(arg):
	global  Sources
	global GlobalSourceSpeed
	GlobalSourceSpeed = float(arg)

############ ROOM SLIDERS
def wx_slider_roomsize_handler(arg, indRoom):
	address = '/room/'+str(indRoom + 1)+'/reverb/roomsize'
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")
	# print("messageToSpat:", msg_list)

def wx_slider_reverb_handler(arg, indRoom):
	address = '/room/'+str(indRoom + 1)+'/reverberance'
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")
	# print("messageToSpat:", msg_list)

def wx_slider_heaviness_handler(arg, indRoom):
	address = '/room/'+str(indRoom + 1)+'/heaviness'
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")
	# print("messageToSpat:", msg_list)

def wx_slider_liveness_handler(arg, indRoom):
	address = '/room/'+str(indRoom + 1)+'/liveness'
	msg = oscbuildparse.OSCMessage(address, None, [arg])
	msg_list = [msg]
	exectime = time() - 10.0 # execute in 10 seconds
	bun = oscbuildparse.OSCBundle(oscbuildparse.unixtime2timetag(exectime), msg_list)
	osc_send(bun, "Spat_client")
	# print("messageToSpat:", msg_list)

############ BUTTONS
def wx_slider_freeze_handler(arg, indSrc):
	global  Sources
	Sources[indSrc].freeze = not Sources[indSrc].freeze
	# print("Source ON", indSource)

def wx_slider_freeze_all_handler():
	global  Sources
	for indSrc in range(NumberOfSources) :
		Sources[indSrc].freeze = True
	
def wx_slider_defreeze_all_handler():
	global  Sources
	for indSrc in range(NumberOfSources) :
		Sources[indSrc].freeze = False
	
def wx_slider_xy_handler(address, arg1, arg2, arg3):
	global  Sources
	print("received OSC spat: {", address, " ", arg1, arg2, arg3, "}")

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
	global Rooms
	global panel

	global Spat_IP
	global Spat_Port

	global indstep

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
	for ind in range(NumberOfSources) :
		Sources[ind] = Source(ind)

	for ind in range(NumberOfRooms) :
		Rooms[ind] = Room(ind)

	##################################################################
	# Start OSC event processing.
	osc_startup()

	##################################################################
	# builds the interface
	print("The python program opens OSC client Spat on Address %s Port: %d" % (Spat_IP, Spat_Port))
	osc_udp_client(Spat_IP, Spat_Port, "Spat_client")

	app = wx.App(False)
	# create a window/frame, no parent, -1 is default ID, title, size
	frame = wx.Frame(None, -1, "SPAT CONTROL", size = (1300, 800), pos = (0, 0))
	# call the derived class, -1 is default ID
	panel = MyPanel(frame)
	# show the frame
	frame.Show(True)
	
	##################################################################
	# start the event loop
	print("The python program runs... type control C in console or Stop in interface to stop the program")
	indstep = 0
	app.MainLoop()

	##################################################################
	# DISCONNECTION
	disconnect()

if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	main(sys.argv[1:])
