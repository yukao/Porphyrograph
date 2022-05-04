#!/usr/bin/python3 

# pg_mss.py
# mass spring system physics
# (c) Yukao Nagemi

# code modified from http://kineticskit.sourceforge.net/

from math import pow
from math import sqrt
from math import pi
from math import floor

import getopt
import sys
from signal import signal, SIGINT
from time import sleep
from time import time

import re

from random import random

# CVS file reading line by line
import csv

# library to convert between strings and nums
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate
from pg_lib import Vector
from pg_lib import Point

# OSC
from osc4py3 import oscmethod as osm

# sensors
import pg_sensors

# XMM
import pg_train_reco_xmm

# OSC
import pg_OSC

##################################################################
# GLOBAL VARIABLE
##################################################################
pg_MSS_current = None

MASSES_NONE = 0
MASSES_ALL = 1
MASSES_NON_FIXED = 2
MASSES_WITH_SENSORS = 3
MASS_SUBSETS = ["MASSES_NONE","MASSES_ALL","MASSES_NON_FIXED","MASSES_WITH_SENSORS"]

##################################################################
# USAGE
##################################################################
USAGE = '''pg_mss.py -i configuration_file
	ARGUMENTS:
	-i: input configuration_file
'''

##################################################################
# MASS IN A MSS
##################################################################
class Mass:
	def __init__(self, mass_ID, mass, pos_ini, pos_std, is_fixed=False, velocity=Vector(0., 0., 0.)):
		# Construct a mass.
		self.mass_ID = mass_ID
		self.mass = float(mass)
		self.center = pos_ini
		self.std_center = pos_std # positon at equilibrum without gravity
		self.is_fixed = is_fixed
		self.velocity = velocity
		self.sumForces = Vector(0., 0., 0.)
		self.sensorForce = Vector(0., 0., 0.)
		self.ind_sensor = -1
		self.next_activity_update = -1

	def calcGravityForce(self, g, mass_factor):
		# Calculate the gravity sumForces.
		# gforce = mass * g
		self.sumForces = self.sumForces - Vector(0, self.mass * g * mass_factor, 0)

	def calcViscosityForce(self, viscosity):
		# Calculate the viscosity sumForces.
		# Fviscosity = - velocity * viscosityFactor
		# the viscosity should at most cancel the forces
		viscosity_force = (self.velocity * viscosity)
		if viscosity_force.norm() > self.sumForces.norm() :
			viscosity_force = viscosity_force.normalize()
			viscosity_force = viscosity_force * self.sumForces.norm()
		self.sumForces = self.sumForces - viscosity_force

	def calcActivityForce(self, activity_level):
		global pg_MSS_current
		# Calculate the activty sumForces.
		if(self.next_activity_update == -1) :
			lapse = random() * 5
			self.next_activity_update = lapse + pg_MSS_current.elapsed_time
			# print("shake", self.mass_ID, "first shake in", lapse)
		elif(pg_MSS_current.elapsed_time >= self.next_activity_update) :
			shaking_force = (Vector(random() * activity_level, random() * activity_level, random() * activity_level) * 0.1)
			self.sumForces = self.sumForces + shaking_force
			lapse = random() * (10. / activity_level)
			self.next_activity_update = lapse + pg_MSS_current.elapsed_time
			# print("shake", self.mass_ID, "force", shaking_force.vx, shaking_force.vy, shaking_force.vz, "dur", lapse)

	def calcNewLocation(self, dt):
		# first adds the possible force from sensor
		self.sumForces = self.sumForces + self.sensorForce
		# Calculate the new location of the mass.
		# sumForces = mass * a = mass * dv / dt  =>  dv = sumForces * dt / mass
		dv = self.sumForces * (dt / self.mass)
		#  velocity update: v' = v + dv
		self.velocity = self.velocity + dv
		# position update: velocity = dx / dt  =>  dx = velocity * dt
		# and x' = x + dx
		self.center = self.center + (self.velocity * dt)

	def clearSpringAndSensorForces(self):
		# Clear the Spring Force before an update and sumForces summation
		self.sumForces = Vector(0., 0., 0.)
		# Clear the Sensor Force until a new update is received
		self.sensorForce = Vector(0., 0., 0.)

##################################################################
# SPRING IN A MSS
##################################################################
class Spring:
	# Physical model of a spring.
	def __init__(self, m0, m1, k, l0, damping):
		# Construct a spring.
		self.m0 = m0
		self.m1 = m1
		self.k = k
		# rest length
		if l0 != 0:
			self.l0 = l0
		else:
			self.l0 = (self.m1.center - self.m0.center).norm()

		# possible modifications through interactive messaging
		self.damping = damping

		# normalized orientation along vec(m0, m1)
		self.orientation = (self.m1.center - self.m0.center).normalize()
		self.Fspring = Vector(0., 0., 0.)
		self.elongation = 0.

	def calcSpringForceAndUpdateMassForces(self, l0_factor, k_factor):
		# Calculate the spring sumForces from its elongation (difference between current length and rest length)
		delta = self.m1.center - self.m0.center
		self.elongation = delta.norm() - self.l0 * l0_factor
		# for spider web, we only consider stretching for creating a force
		if(self.elongation > 0) :
			self.orientation = delta.normalize()
			# Fspring = (l - l0) * k * unitary vector along the axis connecting the 2 extremities
			# Hook's law: the sumForces is prportional to the elongation and directed towards the othe mass 
			# for a positive elongation
			self.Fspring = self.orientation * (self.elongation * self.k * k_factor)
			# updates the sum of forces on each end mass
			self.m0.sumForces = self.m0.sumForces + self.Fspring
			self.m1.sumForces = self.m1.sumForces - self.Fspring

	def calcDampingForce(self, damping_factor):
		# Calculate the damping sumForces.
		# Fdamping = velocity in elongation * dampingFactor
		if(self.damping != 0):
			Fdamping = self.orientation * self.orientation.dot((self.m1.velocity - self.m0.velocity)) * self.damping * damping_factor
		else:
			Fdamping = 0
		self.m0.sumForces = self.m0.sumForces + Fdamping
		self.m1.sumForces = self.m1.sumForces - Fdamping

##################################################################
# MASS/SPRING SYSTEM
##################################################################
class MSS:
	# The system.
	def __init__(self, timestep, scale_x, scale_y, oversample, gravity, viscosity, spring_mode, mass_factor, k_factor, l0_factor, damping_factor, logged_masses, udp_msg_masses_clipart, udp_msg_masses_stroke):
		# Construct a system.
		self.timestep = timestep
		self.oversample = oversample
		self.spring_mode = int(spring_mode)

		self.scale_x = scale_x
		self.scale_y = scale_y

		try:
			idx = MASS_SUBSETS.index(logged_masses)
		except ValueError:
			print("unknown mass subset value", logged_masses, "only", MASS_SUBSETS)
			sys.exit(2)
		self.logged_masses = idx
		try:
			idx = MASS_SUBSETS.index(udp_msg_masses_clipart)
		except ValueError:
			print("unknown mass subset value", udp_msg_masses_clipart, "only", MASS_SUBSETS)
			sys.exit(2)
		self.udp_msg_masses_clipart = idx
		try:
			idx = MASS_SUBSETS.index(udp_msg_masses_stroke)
		except ValueError:
			print("unknown mass subset value", udp_msg_masses_stroke, "only", MASS_SUBSETS)
			sys.exit(2)
		self.udp_msg_masses_stroke = idx

		# mss structure
		self.masses = dict()
		self.mass_sensors = dict()
		self.springs = []

		# scenario scenes
		self.mss_nb_scenes = 0
		self.mss_scene_ID = dict()
		self.mss_scene_beginning = dict()
		self.mss_scene_end = dict()
		self.mss_scene_duration = dict()

		# mss paramester control through scenario
		self.param_IDs = ["gravity", "viscosity", "mass_factor", "k_factor", "l0_factor", "damping_factor"]
		self.val_params = dict()
		self.val_params_last_update = dict()
		self.mss_var_types = dict()
		self.mss_val_init = dict()
		self.mss_val_fin = dict()
		self.mss_interpolator = dict()
		self.mss_var_break_interpolation = dict()

		# params initialization from  function arguments (taken from mss configuration file)
		self.val_params["gravity"] = gravity
		self.val_params["viscosity"] = viscosity
		self.val_params["mass_factor"] = mass_factor
		self.val_params["k_factor"] = k_factor
		self.val_params["l0_factor"] = l0_factor
		self.val_params["damping_factor"] = damping_factor
		self.val_params_last_update["gravity"] = gravity
		self.val_params_last_update["viscosity"] = viscosity
		self.val_params_last_update["mass_factor"] = mass_factor
		self.val_params_last_update["k_factor"] = k_factor
		self.val_params_last_update["l0_factor"] = l0_factor
		self.val_params_last_update["damping_factor"] = damping_factor

		# scenario dynamics management
		self.nb_steps = 0
		self.current_scene = -1
		self.initial_time = 0.
		self.elapsed_time = 0.
		self.launched = False
		self.terminated = False
		self.OSC_trace = False

		# utilitary variable
		self.mass_mobile = None
		self.sorted_mass_keys = None
		self.log_header_printed = False

	# update of mass Orientation data according to sensor values
	# called in pg_sensors.py
	def update_Euler(self, sensor_ID, sensor_heading, sensor_pitch, sensor_roll, sensor_yaw):
		# updates speed along direction (in addition to regular speed update through forces and acceleration)
		mass_sensor = self.mass_sensors[sensor_ID]
		if(mass_sensor != None):
			# NOTHING FOR THE MOMENT - TO BE UPDATED LATER
			# print( "Sensor updates mass Euler to be done: ", mass_sensor.mass_ID)
			mass_sensor.velocity = mass_sensor.velocity + Vector(0., 0., 0.)
		else :
			print( "Sensor not connected to a mass: ", sensor_ID)

	# update of mass acceleration data according to sensor values
	# called in pg_sensors.py
	def update_linAcc(self, sensor_ID, sensor_x, sensor_y, sensor_z):
		# updates sumForces according to acceleration (in addition to regular spring forces)
		mass_sensor = self.mass_sensors[sensor_ID]
		if(mass_sensor != None):
			if(mass_sensor.is_fixed):
				print( "Sensor connected to a fixed mass: ", sensor_ID)
			if(mass_sensor.mass > 0):
				# print( "Sensor updates mass acc: ", mass_sensor.mass_ID)
				mass_sensor.sumForces = mass_sensor.sumForces + (Vector(sensor_x, sensor_y, sensor_z) * mass_sensor.mass)
		else :
			print( "Sensor not connected to a mass: ", sensor_ID)

	# update of mass positions
	def step(self):
		if(self.terminated == True):
				return;

		if(self.launched and self.mss_nb_scenes > 0) :
			# updates variables according to scenario
			self.nb_steps += 1
			if(self.nb_steps == 1) :
				self.initial_time = time()
			self.elapsed_time = time() - self.initial_time
			elapsed_minutes = int(floor(self.elapsed_time/60))
			elapsed_seconds = int(floor(self.elapsed_time - elapsed_minutes * 60.))
			time_string_for_TouchOSC = str("%02d" % elapsed_minutes) + ":" + ("%02d" % elapsed_seconds)
			pg_OSC.UDP_emit("/mss_time", "udp_TouchOSC_send", [time_string_for_TouchOSC])

			if(self.elapsed_time >= self.mss_scene_end[self.mss_nb_scenes - 1]) :
				self.terminated = True
				print("MSS quits")
				return

			# looks for the current scene and the position in this scene
			ind_current_scene = -1
			scene_percentage = 0
			for ind_scene in range(self.mss_nb_scenes) :
				if(self.elapsed_time >= self.mss_scene_beginning[ind_scene] and self.elapsed_time <= self.mss_scene_end[ind_scene]) :
					scene_percentage = float(self.elapsed_time - self.mss_scene_beginning[ind_scene])/(self.mss_scene_end[ind_scene] - self.mss_scene_beginning[ind_scene])
					ind_current_scene = ind_scene
					break

			# no scene found, we have passed the end of the last scene and we are at the end of the scenario
			if(ind_current_scene < 0) :
				ind_current_scene = self.mss_nb_scenes - 1
				scene_percentage = 1
				self.current_scene = -1
				self.launched = False

			# updates the current scene and prints it on screen
			if(self.current_scene != ind_current_scene) :
				self.current_scene = ind_current_scene
				if(ind_current_scene < self.mss_nb_scenes) :
					print("Scene:", ind_current_scene)
					pg_OSC.UDP_emit("/mss_setup", "udp_TouchOSC_send", [self.mss_scene_ID[ind_current_scene]])
				else :
					print("End of scenario")
					pg_OSC.UDP_emit("/mss_setup", "udp_TouchOSC_send", ["End of scenario"])

			# calculates the interpolated values
			# log_vars = ""
			scene_percentage = clamp(scene_percentage)
			for var_name in self.param_IDs :
				if(self.mss_var_break_interpolation[var_name] == False) :
					scene_var_key = (ind_current_scene, var_name)
					self.val_params[var_name] = interpolate(self.mss_var_types[var_name], \
						self.mss_val_init[scene_var_key], self.mss_val_fin[scene_var_key], self.mss_interpolator[scene_var_key], scene_percentage)
					if(self.val_params[var_name] != self.val_params_last_update[var_name]) :
						pg_OSC.UDP_emit("/mss_param/" + var_name, "udp_TouchOSC_send", [self.val_params[var_name]])
						self.val_params_last_update[var_name] = self.val_params[var_name]
			# 	log_vars += var_name + ":" + ("%.2f" % self.val_params[var_name] ) + " "
			# print(log_vars)

			# print("val parameters at ", "%.2f" % self.elapsed_time, "scene %", ind_current_scene, "%.2f" % scene_percentage, self.val_params)

		if(self.launched) :
			# Perform one Iteration of the system by advancing one timestep.
			microstep = self.timestep / self.oversample
			# if oversampling several updates are made at each step
			for i in range(self.oversample):
				# calculates spring forces according to elongation
				# adds the forces to each extremity mass
				for spring in self.springs:
					spring.calcSpringForceAndUpdateMassForces(self.val_params["l0_factor"], self.val_params["k_factor"])
					# print("spring: elong %.5f strength %.5f %.5f %.5f " \
					# 	% (spring.elongation, spring.Fspring.vx, spring.Fspring.vy, spring.Fspring.vz))
					if(spring.damping != 0):
						spring.calcDampingForce(self.val_params["damping_factor"])

				# updates mass location according to applied forces
				for mass in self.masses.values():
					if not mass.is_fixed:
						if self.val_params["gravity"] != 0 :
							mass.calcGravityForce(self.val_params["gravity"], self.val_params["mass_factor"])
						if self.val_params["viscosity"] != 0 :
							mass.calcViscosityForce(self.val_params["viscosity"])
						if(self.val_params[mass.mass_ID + "_activ"] > 0) :
							mass.calcActivityForce(self.val_params[mass.mass_ID + "_activ"])
						mass.calcNewLocation(microstep)
						# if mass.mass_ID == "14_r1" :
						# 	print("14_r1:", mass.center.px, mass.center.py, mass.center.pz)
						# floor value for the position of the masses so that they remain visible vertically (fx under the influence of gravity)
						mass.center.py = max(mass.center.py, 0.1)

					# print("mass: (%s), Position  %.5f %.5f %.5f sumForces  %.5f %.5f %.5f velocity  %.5f %.5f %.5f " \
					# 	% (mass_ID, mass.center.px, mass.center.py, mass.center.pz, mass.sumForces.vx, mass.sumForces.vy, mass.sumForces.vz, mass.velocity.vx, mass.velocity.vy, mass.velocity.vz))

				# clears mass applied forces
				for mass in self.masses.values():
					# clear mass forces after position update in preparation 
					# of next update
					mass.clearSpringAndSensorForces()

	def log_mass_positions_header(self, log_file) :
		global MASSES_NONE
		global MASSES_ALL
		global MASSES_NON_FIXED
		global MASSES_WITH_SENSORS

		if (self.logged_masses == MASSES_NONE) :
			return

		if not self.log_header_printed :
			self.log_header_printed = True
			to_print = ""
			if(self.sorted_mass_keys == None) :
				self.sorted_mass_keys = list(self.masses.keys())
				self.sorted_mass_keys.sort()
			values = ["time_stamp"]
			for mass_ID in self.sorted_mass_keys :
				mass = self.masses[mass_ID]
				if (self.logged_masses == MASSES_ALL or (self.logged_masses == MASSES_NON_FIXED and not mass.is_fixed) or (self.logged_masses == MASSES_WITH_SENSORS and mass.ind_sensor >= 0)) :
					values = values + [mass_ID + "_x", mass_ID + "_y", mass_ID + "_z"]
			log_file.writerow(values)

	def log_mass_positions_values(self, log_file) :
		global MASSES_NONE
		global MASSES_ALL
		global MASSES_NON_FIXED
		global MASSES_WITH_SENSORS

		if (self.logged_masses == MASSES_NONE) :
			return

		if self.launched :
			to_print = ""
			if(self.sorted_mass_keys == None) :
				self.sorted_mass_keys = list(self.masses.keys())
				self.sorted_mass_keys.sort()
			values = ["%.5f" % self.elapsed_time]
			for mass_ID in self.sorted_mass_keys :
				mass = self.masses[mass_ID]
				if (self.logged_masses == MASSES_ALL or (self.logged_masses == MASSES_NON_FIXED and not mass.is_fixed) or (self.logged_masses == MASSES_WITH_SENSORS and mass.ind_sensor >= 0)) :
					values = values + ["%.5f" % mass.center.px, "%.5f" % mass.center.py, "%.5f" % mass.center.pz]
			log_file.writerow(values)

	def send_mass_positions_values(self) :
		global MASSES_NONE
		global MASSES_ALL
		global MASSES_NON_FIXED
		global MASSES_WITH_SENSORS

		if(self.sorted_mass_keys == None) :
			self.sorted_mass_keys = list(self.masses.keys())
			self.sorted_mass_keys.sort()
		# updates clipart mass locations according to applied forces
		if pg_train_reco_xmm.getGMM().XMM_forward_web_animation :
			if (self.udp_msg_masses_clipart == MASSES_ALL or self.udp_msg_masses_clipart == MASSES_NON_FIXED or self.logged_masses == MASSES_WITH_SENSORS) :
				xy_positions = []
				for mass_ID in self.sorted_mass_keys :
					mss_mass = self.masses[mass_ID]
					if (self.udp_msg_masses_clipart == MASSES_ALL or (self.udp_msg_masses_clipart == MASSES_NON_FIXED and not mss_mass.is_fixed) or (self.logged_masses == MASSES_WITH_SENSORS and mass.ind_sensor >= 0)) :
						# if(mss_mass.is_fixed == False) :
						xy_positions = xy_positions + [mss_mass.center.px, (1.0 - mss_mass.center.py)]
				# for visualizing the web masses in porphyrograph
				pg_OSC.UDP_emit("/ClipArt_translations", "udp_porphyrograph_send", xy_positions)
				# print("/ClipArt_translations", "udp_porphyrograph_send", xy_positions)

		# stroke can only be made on sensor linked masses
		if pg_train_reco_xmm.getGMM().XMM_forward_sensor_masses_animation :
			if (self.udp_msg_masses_stroke == MASSES_ALL or self.udp_msg_masses_stroke == MASSES_WITH_SENSORS) :
				# updates sensor mass location according to applied forces
				for mass in self.masses.values():
					if(mass.ind_sensor >= 0):
						# for using the sensor driven mass(es) a drawing tools in porphyrograph
						# print("mass: /mw_mss_pos", "udp_porphyrograph_send", [mass.ind_sensor, mass.center.px * self.scale_x, mass.center.py * self.scale_y, mass.center.pz])
						pg_OSC.UDP_emit("/mw_mss_pos", "udp_porphyrograph_send", [mass.ind_sensor, mass.center.px * self.scale_x, (1.0 - mass.center.py) * self.scale_y, mass.center.pz])
						# source control in SPAT
						if True :
							pg_OSC.UDP_emit("/source/"+str(mass.ind_sensor)+"/xyz", "udp_SPAT_send", [(mass.center.px - 0.5) * 10., (mass.center.py - 0.5) * 10., (mass.center.pz - 0.5) * 2])
						# effect control in LIVE
						if False :
							pg_OSC.UDP_emit("/sensor/"+str(mass.ind_sensor), "udp_LIVE_send", [sqrt(mass.center.px * mass.center.px + mass.center.py * mass.center.py + mass.center.pz * mass.center.pz )])

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	def MSS_scenario(self, scenario_file_name):
		line = ""

		global pg_MSS_current

		try:
			FILEin = open(scenario_file_name, "rt")
		except IOError:
			print("File not found :", scenario_file_name, " or path is incorrect")

		readCSV = csv.reader(FILEin, delimiter=',')

		line = next(readCSV) 
		# print("line1 ", line)
		line = next(readCSV) 
		# print("line2 ", line)

		#3rd line: variable types
		line_types = next(readCSV)

		#4th line: variable names
		mss_var_names = next(readCSV) 
		# removes the first one
		mss_var_names.pop(0)
		for var_name in mss_var_names :
			if not (var_name in self.param_IDs) :
				print("unexpected variable name in scenario", var_name)
				print("expected:", self.param_IDs)
				sys.exit(2)

		#finishes the types now that the variable strings are known
		#removes the first one
		line_types.pop(0)
		indVar = 0
		for line_type in line_types :
			self.mss_var_types[mss_var_names[indVar]] = line_type
			self.mss_var_break_interpolation[mss_var_names[indVar]] = False
			indVar += 1
		
		# scenario line
		values_line = next(readCSV)
		if values_line[0] != "scenario":
			print("Expected tag \"scenario\" not found!")
			return 0
		
		self.mss_nb_scenes = int(values_line[1])
		# print("nb scenes ", self.mss_nb_scenes)

		##################################################################
		# READING A SCENE
		##################################################################
		total_duration = 0
		for ind_scene in range(self.mss_nb_scenes) :
			values_line = next(readCSV)
			if values_line[0] != "scene":
				print("Expected tag \"scene\" not found! (scene:", ind_scene, ")")
				return 0
			self.mss_scene_ID[ind_scene] = values_line[1]
			self.mss_scene_duration[ind_scene] = force_num(values_line[2])
			if(self.mss_scene_duration[ind_scene] <= 0) :
				println("Incorrect scene length", ind_scene, self.mss_scene_ID[ind_scene], self.mss_scene_duration[ind_scene])
				sys.exit(2)

			self.mss_scene_beginning[ind_scene] = total_duration
			total_duration += self.mss_scene_duration[ind_scene]
			self.mss_scene_end[ind_scene] = total_duration

			# scene ID line
			line = next(readCSV)

			# var IDs line
			line = next(readCSV)
		
			# variable initial value
			values_line = next(readCSV)
			values_line.pop(0)
			indVar = 0
			for value_init in values_line:
				self.mss_val_init[(ind_scene,mss_var_names[indVar])] = to_num(value_init)
				indVar += 1

			# variable final value
			values_line = next(readCSV)
			values_line.pop(0)
			indVar = 0
			for value_fin in values_line:
				self.mss_val_fin[(ind_scene,mss_var_names[indVar])] = to_num(value_fin)
				indVar += 1

			# variable interpolation value
			values_line = next(readCSV)
			values_line.pop(0)
			indVar = 0
			for value_interp in values_line:
				self.mss_interpolator[(ind_scene,mss_var_names[indVar])] = value_interp 
				indVar += 1

			print("Scenes nb: %d scene ID %s start-end %.1f %.1f" % (ind_scene, self.mss_scene_ID[ind_scene], self.mss_scene_beginning[ind_scene], self.mss_scene_end[ind_scene]))

			# /scene line
			values_line = next(readCSV)
			if values_line[0] != "/scene":
				print("Expected tag \"/scene\" not found!")
				sys.exit(2)
		
		# /scene line
		values_line = next(readCSV)
		if values_line[0] != "/scenario":
			print("Expected tag \"/scenario\" not found!")
			sys.exit(2)
		
		FILEin.close()

		print("Scenario {} variables, {} scenes".format(len(self.param_IDs), self.mss_nb_scenes))

		return 1

##################################################################
# READS THE CONFIGURATION LINE BY LINE
##################################################################
def MSS_init(readCSV):
	##################################################################
	# MSS INITIALIZATION
	line =  next(readCSV)
	if line[0] != "MSS":
		print( "Expected tag \"MSS\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCSV) 
	# print( "line1 ", line  )

	MSS_line =  next(readCSV) 
	# print( "line2 ", MSS_line  )

	nb_mass = to_num(MSS_line[0])
	nb_spring = to_num(MSS_line[1])

	MSS_time_step = to_num(MSS_line[2])
	MSS_nb_oversample = to_num(MSS_line[3])
	MSS_gravity = to_num(MSS_line[4])
	MSS_viscosity = to_num(MSS_line[5])

	MSS_scale_x = to_num(MSS_line[6])
	MSS_scale_y = to_num(MSS_line[7])

	MSS_spring_mode = to_num(MSS_line[8])

	MSS_mass_factor	= to_num(MSS_line[9])
	MSS_k_factor = to_num(MSS_line[10])
	MSS_l0_factor = to_num(MSS_line[11])
	MSS_damping_factor = to_num(MSS_line[12])

	MSS_logged_masses = to_num(MSS_line[13])
	MSS_udp_msg_masses_clipart = to_num(MSS_line[14])
	MSS_udp_msg_masses_stroke = to_num(MSS_line[15])

	# /MSS line
	line =  next(readCSV)
	if line[0] != "/MSS":
		print( "Expected tag \"/MSS\" not found in line: ", line)
		sys.exit(2)

	# MSS initialization
	pg_MSS_current = MSS(MSS_time_step, MSS_scale_x, MSS_scale_y, MSS_nb_oversample, MSS_gravity, \
		MSS_viscosity, MSS_spring_mode, MSS_mass_factor, MSS_k_factor, MSS_l0_factor, MSS_damping_factor, \
		MSS_logged_masses, MSS_udp_msg_masses_clipart, MSS_udp_msg_masses_stroke)

	################################################
	# mass lines
	line =  next(readCSV)
	if line[0] != "mass":
		print( "Expected tag \"mass\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCSV) 
	# print( "line1 ", line  )

	for ind_mass in range(nb_mass):
		mass_line = next(readCSV) 
		# print( "line2 ", spring_line  )
		mass_ID = mass_line[0]
		mass_mass = to_num(mass_line[1])
		mass_fixed = (mass_line[2] == 'True' or mass_line[2] == 'TRUE' or mass_line[2] == 'true')
		mass_sensor_IDs = mass_line[3]
		mass_x_std = to_num(mass_line[4])
		mass_y_std = to_num(mass_line[5])
		mass_z_std = to_num(mass_line[6])
		mass_x_ini = to_num(mass_line[7])
		mass_y_ini = to_num(mass_line[8])
		mass_z_ini = to_num(mass_line[9])
		mass_vx = to_num(mass_line[10])
		mass_vy = to_num(mass_line[11])
		mass_vz = to_num(mass_line[12])
		if(not(mass_ID in pg_MSS_current.masses)):
			pg_MSS_current.masses[mass_ID] = Mass(mass_ID, mass_mass, Point(mass_x_ini, mass_y_ini, mass_z_ini), Point(mass_x_std, mass_y_std, mass_z_std), mass_fixed, Vector(mass_vx, mass_vy, mass_vz))
		else:
			print("Duplicate mass ID: ", mass_ID)
			sys.exit(2)
		if(mass_sensor_IDs != 'None'): 
			list_of_mass_sensor_IDs = re.split(r'/', mass_sensor_IDs)
			for mass_sensor_ID in list_of_mass_sensor_IDs :
				if not(mass_sensor_ID in pg_MSS_current.mass_sensors):
					pg_MSS_current.mass_sensors[mass_sensor_ID] = pg_MSS_current.masses[mass_ID]
					pg_MSS_current.masses[mass_ID].ind_sensor = int(mass_sensor_ID[2:3:]) - 1 # sensor IDs are mw1, mw2, mw3...
					print("Mass sensor: mass ID {}, sensor ID {}, sensor #{}".format(mass_ID, mass_sensor_ID, pg_MSS_current.masses[mass_ID].ind_sensor))
				else:
					print("Duplicate mass sensor ID: ", mass_sensor_ID)
					sys.exit(2)

	if("center" in pg_MSS_current.masses) :
		pg_MSS_current.mass_mobile = pg_MSS_current.masses["center"]
	else :
		pg_MSS_current.mass_mobile = pg_MSS_current.masses[mass_ID]

	# /mass line
	line =  next(readCSV)
	if line[0] != "/mass":
		print( "Expected tag \"/mass\" not found in line: ", line)
		sys.exit(2)

	# adds the masses activities to the parameters that can be controlled through scenario
	if(pg_MSS_current.sorted_mass_keys == None) :
		pg_MSS_current.sorted_mass_keys = list(pg_MSS_current.masses.keys())
		pg_MSS_current.sorted_mass_keys.sort()
	for mass_ID in pg_MSS_current.sorted_mass_keys :
		if(pg_MSS_current.masses[mass_ID].is_fixed == False) :
			pg_MSS_current.param_IDs += [mass_ID + "_activ"]
			pg_MSS_current.val_params[mass_ID + "_activ"] = 0.
			pg_MSS_current.val_params_last_update[mass_ID + "_activ"] = 0.

	################################################
	# spring lines
	line =  next(readCSV)
	if line[0] != "spring":
		print( "Expected tag \"spring\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCSV) 
	# print( "line1 ", line  )

	for ind_spring in range(nb_spring):
		spring_line = next(readCSV) 
		# print( "line2 ", spring_line  )
		spring_mass_ini_ID = spring_line[0]
		if(not(spring_mass_ini_ID in pg_MSS_current.masses.keys())):
			print("Unknown spring ini mass ID: ", spring_mass_ini_ID)
			sys.exit(2)
		spring_mass_end_ID = spring_line[1]
		if(not(spring_mass_end_ID in pg_MSS_current.masses.keys())):
			print("Unknown spring end mass ID: ", spring_mass_end_ID)
			sys.exit(2)
		spring_k = to_num(spring_line[2])
		if(spring_line[3] != "auto") :
			spring_l0 = to_num(spring_line[3])
		else :
			delta = pg_MSS_current.masses[spring_mass_ini_ID].std_center - pg_MSS_current.masses[spring_mass_end_ID].std_center
			spring_l0 = abs(delta.norm() * 0.7)
		spring_damping = to_num(spring_line[4])
		pg_MSS_current.springs.append(Spring(pg_MSS_current.masses[spring_mass_ini_ID], \
			pg_MSS_current.masses[spring_mass_end_ID], spring_k, spring_l0, spring_damping))

	# /spring line
	line =  next(readCSV)
	if line[0] != "/spring":
		print( "Expected tag \"/spring\" not found in line: ", line)
		sys.exit(2)

	print( "MSS {} masses & {} springs".format(nb_mass, nb_spring) )

	return pg_MSS_current

##################################################################
# OSC COMMUNICATION
##################################################################
##################################################################
# OSC MESSAGE RECEPTION DISPATCHING
def OSC_dispatcher_mapping():
	pg_OSC.osc_method("/mss_param/*", UDP_receive_mss_param_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_spring_mode", UDP_receive_mss_spring_mode_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_mass_mobile_no", UDP_receive_mss_mass_mobile_no_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_mass_mobile_delta", UDP_receive_mss_mass_mobile_delta_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_mass_sensor_acc", UDP_receive_mss_mass_acc_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_launch", UDP_receive_mss_launch_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_quit", UDP_receive_mss_quit_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/mss_sensor_acc_factor", pg_sensors.UDP_receive_sensor_acc_factor_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train1", pg_train_reco_xmm.getGMM().gmm_on_off_train1, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train2", pg_train_reco_xmm.getGMM().gmm_on_off_train2, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train3", pg_train_reco_xmm.getGMM().gmm_on_off_train3, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train4", pg_train_reco_xmm.getGMM().gmm_on_off_train4, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train5", pg_train_reco_xmm.getGMM().gmm_on_off_train5, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_on_off_train6", pg_train_reco_xmm.getGMM().gmm_on_off_train6, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_stop_train", pg_train_reco_xmm.getGMM().gmm_stop_train, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_clear_train", pg_train_reco_xmm.getGMM().gmm_clear_train, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_start_stop_reco", pg_train_reco_xmm.getGMM().gmm_start_stop_reco, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_save_and_load_train", pg_train_reco_xmm.getGMM().gmm_save_and_load_train, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	pg_OSC.osc_method("/xmm_load_train", pg_train_reco_xmm.getGMM().gmm_load_train, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
	# pg_OSC.osc_method("/*", UDP_receive_mss_all_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)

##################################################################
# OSC MESSAGE EMISSION DISPATCHING
def OSC_emitter_resend_all():
	global pg_MSS_current
	for param_string in pg_MSS_current.param_IDs :
		pg_OSC.UDP_emit("/mss_param/" + param_string, "udp_TouchOSC_send", mss_param_value(param_string))
	pg_OSC.UDP_emit("/mss_mode", "udp_TouchOSC_send", mss_spring_mode_value())
	pg_OSC.UDP_emit("/mss_sensor_acc_factor", "udp_TouchOSC_send",  [pg_sensors.sensor_acc_factor_value()])
	current_gmm =pg_train_reco_xmm.getGMM()
	for ind in range(6):
		if current_gmm.xmm_train == True and current_gmm.xmm_training_label == current_gmm.xmm_training_data_index_to_label(ind) :
			pg_OSC.UDP_emit("/xmm_on_off_train"+str(ind+1), "udp_TouchOSC_send", [1])
		else :
			pg_OSC.UDP_emit("/xmm_on_off_train"+str(ind+1), "udp_TouchOSC_send", [0])
	if current_gmm.xmm_reco == True :
		pg_OSC.UDP_emit("/xmm_start_stop_reco", "udp_TouchOSC_send", [1])
	else :
		pg_OSC.UDP_emit("/xmm_start_stop_reco", "udp_TouchOSC_send", [0])

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
def mss_param_value(param_string):
	global pg_MSS_current
	return [pg_MSS_current.val_params[param_string]]

def UDP_receive_mss_param_handler(address, arg):
	global pg_MSS_current
	param_string = address[len("/mss_param/"):]
	pg_MSS_current.val_params[param_string] = force_num(arg)
	pg_MSS_current.mss_var_break_interpolation[param_string] = True
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg, "}", param_string, "=", pg_MSS_current.val_params[param_string])
	pg_OSC.UDP_emit(address, "udp_TouchOSC_send", [mss_param_value(param_string)])

def UDP_receive_mss_spring_mode_handler(address, arg):
	global pg_MSS_current
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg, "}")
	pg_MSS_current.spring_mode = to_num(arg)
	pg_OSC.UDP_emit(address, "udp_TouchOSC_send", [mss_spring_mode_value()])

def mss_spring_mode_value():
	global pg_MSS_current
	return [pg_MSS_current.spring_mode]

def UDP_receive_mss_mass_mobile_no_handler(address, arg):
	global pg_MSS_current
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg, "}")
	mass_no = int(force_num(arg))
	list_masses = list(pg_MSS_current.masses.keys())
	if(mass_no < len(list_masses)) :
		pg_MSS_current.mass_mobile = pg_MSS_current.masses[list_masses[mass_no]];
		pg_OSC.UDP_emit(address, "udp_TouchOSC_send", [mss_spring_mode_value()])

def UDP_receive_mss_mass_mobile_delta_handler(address, arg1, arg2):
	global pg_MSS_current
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg1, arg2, "}")
	if(pg_MSS_current.mass_mobile != None) :
		pg_MSS_current.mass_mobile.center.px += arg1;
		pg_MSS_current.mass_mobile.center.py += arg2;
		pg_OSC.UDP_emit(address, "udp_TouchOSC_send", [0,0])

def UDP_receive_mss_mass_acc_handler(address, arg1, arg2, arg3):
	global pg_MSS_current
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg1, arg2, arg3, "}")
	# if arg2 != 0. and arg3 != 0 :
	# 	print("update_linAcc", arg2, arg3, 0.)
	pg_MSS_current.update_linAcc(arg1, arg2, arg3, 0.)

def UDP_receive_mss_launch_handler(address, arg):
	global pg_MSS_current
	if pg_MSS_current.OSC_trace :
		print("received OSC {", address, "}: {", arg, "}")
	pg_MSS_current.launched = not pg_MSS_current.launched
	if pg_MSS_current.launched :
		print("MSS scenario launched")
	else :
		print("MSS scenario stopped")
	pg_OSC.UDP_emit(address, "udp_TouchOSC_send", int(pg_MSS_current.launched))

def UDP_receive_mss_quit_handler(address, arg):
	global pg_MSS_current
	pg_MSS_current.terminated = True
	print("MSS quits")

def UDP_receive_mss_all_handler(address):
	global pg_MSS_current
	print("unexpected received OSC {", address, "}")
def UDP_receive_mss_all_handler(address, arg):
	global pg_MSS_current
	print("unexpected received OSC {", address, "}: {", arg, "}")
def UDP_receive_mss_all_handler(address, arg1, arg2):
	global pg_MSS_current
	print("unexpected received OSC {", address, "}: {", arg1, arg2, "}")
def UDP_receive_mss_all_handler(address, arg1, arg2, arg3):
	global pg_MSS_current
	print("unexpected received OSC {", address, "}: {", arg1, arg2, arg3, "}")


