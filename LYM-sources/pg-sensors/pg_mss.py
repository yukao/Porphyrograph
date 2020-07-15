#!/usr/bin/python3 

# pg_mss.py
# mass spring system physics
# (c) Yukao Nagemi

# code modified from http://kineticskit.sourceforge.net/

from math import pow
from math import sqrt
from math import pi

import getopt
import sys
from signal import signal, SIGINT
from time import sleep

# CVS file reading line by line
import csv

# library to convert between strings and nums
from vv_lib import to_num

# library to convert between strings and nums
import pg_sensors

##################################################################
# GLOBAL VARIABLES
##################################################################
pg_MSS_current = None

##################################################################
# USAGE
##################################################################
USAGE = '''pg_mss.py -i configuration_file
	ARGUMENTS:
	-i: input configuration_file
'''

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
# MASS IN A MSS
##################################################################
class Mass:
	#rho = 8900 # Cu
	rho = 23.8732414637845 # for backwards compatibility
	factor = 3. / (4 * pi * rho)

	def __init__(self, mass, mass_factor, pos, is_fixed=False, velocity=Vector(0., 0., 0.)):
		# Construct a mass.
		self.mass = float(mass)
		# possible modifications through interactive messaging
		self.mass_factor = float(mass_factor)

		self.center = pos
		self.is_fixed = is_fixed
		self.velocity = velocity
		self.sumForces = Vector(0., 0., 0.)
		self.sensorForce = Vector(0., 0., 0.)
		self.ind_sensor = -1

	def calcGravityForce(self, g):
		# Calculate the gravity sumForces.
		# gforce = mass * g
		self.sumForces = self.sumForces - Vector(0, self.mass * g, 0)

	def calcViscosityForce(self, viscosity):
		# Calculate the viscosity sumForces.
		# Fviscosity = - velocity * viscosityFactor
		self.sumForces = self.sumForces - (self.velocity * viscosity)

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

	def __init__(self, m0, m1, k, l0, damping, k_factor, l0_factor, damping_factor):
		# Construct a spring.
		self.m0 = m0
		self.m1 = m1
		self.k = k
		self.k_factor = k_factor
		# rest length
		if l0:
			self.l0 = l0
		else:
			self.l0 = (self.m1.center - self.m0.center).norm()

		# possible modifications through interactive messaging
		self.l0_factor = l0_factor
		self.damping = damping
		self.damping_factor = damping_factor

		# normalized orientation along vec(m0, m1)
		self.orientation = (self.m1.center - self.m0.center).normalize()
		self.Fspring = Vector(0., 0., 0.)
		self.elongation = 0.

	def calcSpringForceAndUpdateMassForces(self):
		# Calculate the spring sumForces from its elongation (difference between current length and rest length)
		delta = self.m1.center - self.m0.center
		self.elongation = delta.norm() - (self.l0 * self.l0_factor)
		self.orientation = delta.normalize()
		# Fspring = (l - l0) * k * unitary vector along the axis connecting the 2 extremities
		# Hook's law: the sumForces is prportional to the elongation and directed towards the othe mass 
		# for a positive elongation
		self.Fspring = self.orientation * (self.elongation * (self.k * self.k_factor))
		# updates the sum of forces on each end mass
		self.m0.sumForces = self.m0.sumForces + self.Fspring
		self.m1.sumForces = self.m1.sumForces - self.Fspring

	def calcDampingForce(self):
		# Calculate the damping sumForces.
		# Fdamping = velocity in elongation * dampingFactor
		if(self.damping * self.damping_factor):
			Fdamping = self.orientation * self.orientation.dot((self.m1.velocity - self.m0.velocity)) * (self.damping * self.damping_factor)
		else:
			Fdamping = 0
		self.m0.sumForces = self.m0.sumForces + Fdamping
		self.m1.sumForces = self.m1.sumForces - Fdamping

##################################################################
# MASS/SPRING SYSTEM
##################################################################
class MSS:
	# The system.
	def __init__(self, timestep, scale_x, scale_y, oversample, gravity, viscosity, spring_mode):
		# Construct a system.
		self.timestep = timestep
		self.oversample = oversample
		self.gravity = gravity
		self.viscosity = viscosity
		self.spring_mode = int(spring_mode)

		self.scale_x = scale_x
		self.scale_y = scale_y

		self.masses = dict()
		self.mass_sensors = dict()
		self.springs = []

	# update of mass Orientation data according to sensor values
	def update_Euler(self, sensor_ID, sensor_heading, sensor_pitch, sensor_roll, sensor_yaw):
		# updates speed along direction (in addition to regular speed update through forces and acceleration)
		mass_sensor = self.mass_sensors[sensor_ID]
		if(mass_sensor != None):
			# NOTHING FOR THE MOMENT - TO BE UPDATED LATER
			mass_sensor.velocity = mass_sensor.velocity + Vector(0., 0., 0.)
		else :
			print( "Sensor not connected to a mass: ", sensor_ID)

	# update of mass acceleration data according to sensor values
	def update_linAcc(self, sensor_ID, sensor_x, sensor_y, sensor_z):
		# updates sumForces according to acceleration (in addition to regular spring forces)
		mass_sensor = self.mass_sensors[sensor_ID]
		if(mass_sensor != None):
			if(mass_sensor.is_fixed):
				print( "Sensor connected to a fixed mass: ", sensor_ID)
			if(mass_sensor.mass > 0):
				mass_sensor.sumForces = mass_sensor.sumForces + (Vector(sensor_x, sensor_y, sensor_z) * mass_sensor.mass)
		else :
			print( "Sensor not connected to a mass: ", sensor_ID)

	def advance(self):
		# Perform one Iteration of the system by advancing one timestep.
		microstep = self.timestep / self.oversample
		# if oversampling several updates are made at each step
		for i in range(self.oversample):
			# calculates spring forces according to elongation
			# adds the forces to each extremity mass
			for spring in self.springs:
				spring.calcSpringForceAndUpdateMassForces()
				# print("spring: elong %.5f strength %.5f %.5f %.5f " \
				# 	% (spring.elongation, spring.Fspring.vx, spring.Fspring.vy, spring.Fspring.vz))
				if(spring.damping * spring.damping_factor):
					spring.calcDampingForce()

			# updates mass location according to applied forces
			for mass in self.masses.values():
				if not mass.is_fixed:
					if self.gravity:
						mass.calcGravityForce(self.gravity)
					if self.viscosity:
						mass.calcViscosityForce(self.viscosity)
					mass.calcNewLocation(microstep)

				# print("mass: (%s), Position  %.5f %.5f %.5f sumForces  %.5f %.5f %.5f velocity  %.5f %.5f %.5f " \
				# 	% (mass_ID, mass.center.px, mass.center.py, mass.center.pz, mass.sumForces.vx, mass.sumForces.vy, mass.sumForces.vz, mass.velocity.vx, mass.velocity.vy, mass.velocity.vz))


			# updates mass location according to applied forces
			for mass in self.mass_sensors.values():
				if(mass.ind_sensor == 0):
					print("mass: (%d), Position  %.5f %.5f %.5f" % (mass.ind_sensor, mass.center.px * self.scale_x, mass.center.py * self.scale_y, mass.center.pz))
					pg_sensors.UDP_emit("/mss_pos", "udp_porphyrograph_send", [mass.ind_sensor, mass.center.px * self.scale_x, mass.center.py * self.scale_y, mass.center.pz])

			# clears mass applied forces
			for mass in self.masses.values():
				# clear mass forces after position update in preparation 
				# of next update
				mass.clearSpringAndSensorForces()

	def step(self):
		# Perform one step.  This is a convenience method.
		self.advance()

##################################################################
# OSC COMMUNICATION
##################################################################
##################################################################
# OSC RECEIVED MESSAGES PROCESSING
def UDP_receive_mss_mass_mass_factor_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	for mass in pg_MSS_current.masses.values():
		mass.mass_factor = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_mass_mass_factor_value())
def mss_mass_mass_factor_value():
	global pg_MSS_current
	if(pg_MSS_current.masses.values()) :
		mass = next(iter(pg_MSS_current.masses.values()))
		return [mass.mass_factor]
	return [1]

def UDP_receive_mss_mass_visc_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	pg_MSS_current.viscosity = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_mass_visc_value())
def mss_mass_visc_value():
	global SensorSet_current
	return [pg_MSS_current.viscosity]

def UDP_receive_mss_mass_grav_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	pg_MSS_current.gravity = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_mass_grav_value())
def mss_mass_grav_value():
	global SensorSet_current
	return [pg_MSS_current.gravity]

def UDP_receive_mss_spring_k_factor_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	for spring in pg_MSS_current.springs:
		spring.k_factor = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_spring_k_factor_value())
def mss_spring_k_factor_value():
	global SensorSet_current
	if(pg_MSS_current.springs) :
		spring = pg_MSS_current.springs[0]
		return [spring.k_factor]
	return [1]

def UDP_receive_mss_spring_l0_factor_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	for spring in pg_MSS_current.springs:
		spring.l0_factor = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_spring_l0_factor_value())
def mss_spring_l0_factor_value():
	global SensorSet_current
	if(pg_MSS_current.springs) :
		spring = pg_MSS_current.springs[0]
		return [spring.l0_factor]
	return [1]

def UDP_receive_mss_spring_damp_factor_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	for spring in pg_MSS_current.springs:
		spring.damping_factor = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_spring_damp_factor_value())
def mss_spring_damp_factor_value():
	global SensorSet_current
	if(pg_MSS_current.springs) :
		spring = pg_MSS_current.springs[0]
		return [spring.damping_factor]
	return [1]

def UDP_receive_mss_spring_mode_handler(address, *args):
	global pg_MSS_current
	print("received OSC {", address, "}: {", args, "}")
	pg_MSS_current.spring_mode = to_num(args[0])
	pg_sensors.UDP_emit(address, "udp_TouchOSC_send", mss_spring_mode_value())
def mss_spring_mode_value():
	global SensorSet_current
	return [pg_MSS_current.spring_mode]

##################################################################
# READS THE CONFIGURATION LINE BY LINE
##################################################################
def MSS_init(FILEin):
	readCVS = csv.reader(FILEin, delimiter=',')

	##################################################################
	# MSS INITIALIZATION
	line =  next(readCVS)
	if line[0] != "MSS":
		print( "Expected tag \"MSS\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCVS) 
	# print( "line1 ", line  )

	MSS_line =  next(readCVS) 
	# print( "line2 ", MSS_line  )

	nb_mass = to_num(MSS_line[0])
	nb_spring = to_num(MSS_line[1])

	MSS_time_step = to_num(MSS_line[2])
	MSS_nb_oversample = to_num(MSS_line[3])
	MSS_gravity = to_num(MSS_line[4])
	MSS_viscosity = to_num(MSS_line[5])

	MSS_scale_x = to_num(MSS_line[6])
	MSS_scale_y = to_num(MSS_line[7])

	MSS_spring_mode = to_num(MSS_line[7])

	# /MSS line
	line =  next(readCVS)
	if line[0] != "/MSS":
		print( "Expected tag \"/MSS\" not found in line: ", line)
		sys.exit(2)

	# MSS initialization
	MSS_return = MSS(MSS_time_step, MSS_scale_x, MSS_scale_y, MSS_nb_oversample, MSS_gravity, MSS_viscosity, MSS_spring_mode)

	################################################
	# mass lines
	line =  next(readCVS)
	if line[0] != "mass":
		print( "Expected tag \"mass\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCVS) 
	# print( "line1 ", line  )

	for ind_mass in range(nb_mass):
		mass_line = next(readCVS) 
		# print( "line2 ", spring_line  )
		mass_ID = mass_line[0]
		mass_mass = to_num(mass_line[1])
		mass_fixed = (mass_line[2] == 'True')
		mass_sensor_ID = mass_line[3]
		mass_x = to_num(mass_line[4])
		mass_y = to_num(mass_line[5])
		mass_z = to_num(mass_line[6])
		mass_vx = to_num(mass_line[7])
		mass_vy = to_num(mass_line[8])
		mass_vz = to_num(mass_line[9])
		mass_mass_factor = to_num(mass_line[10])
		if(not(mass_ID in MSS_return.masses.keys())):
			MSS_return.masses[mass_ID] = Mass(mass_mass, mass_mass_factor, Point(mass_x, mass_y, mass_z), mass_fixed, Vector(mass_vx, mass_vy, mass_vz))
		else:
			print("Duplicate mass ID: ", mass_ID)
			sys.exit(2)
		if(mass_sensor_ID != 'None'): 
			if not(mass_sensor_ID in MSS_return.mass_sensors.keys()):
				MSS_return.mass_sensors[mass_sensor_ID] = MSS_return.masses[mass_ID]
				MSS_return.masses[mass_ID].ind_sensor = int(mass_sensor_ID[2:3:]) - 1 # sensor IDs are mw1, mw2, mw3...
			else:
				print("Duplicate mass sensor ID: ", mass_sensor_ID)
				sys.exit(2)

	# /mass line
	line =  next(readCVS)
	if line[0] != "/mass":
		print( "Expected tag \"/mass\" not found in line: ", line)
		sys.exit(2)

	################################################
	# spring lines
	line =  next(readCVS)
	if line[0] != "spring":
		print( "Expected tag \"spring\" not found in line: ", line)
		sys.exit(2)
	
	line =  next(readCVS) 
	# print( "line1 ", line  )

	for ind_spring in range(nb_spring):
		spring_line = next(readCVS) 
		# print( "line2 ", spring_line  )
		spring_mass_ini_ID = spring_line[0]
		if(not(spring_mass_ini_ID in MSS_return.masses.keys())):
			print("Unknown spring ini mass ID: ", spring_mass_ini_ID)
			sys.exit(2)
		spring_mass_end_ID = spring_line[1]
		if(not(spring_mass_end_ID in MSS_return.masses.keys())):
			print("Unknown spring end mass ID: ", spring_mass_end_ID)
			sys.exit(2)
		spring_k = to_num(spring_line[2])
		spring_l0 = to_num(spring_line[3])
		spring_damping = to_num(spring_line[4])
		spring_k_factor = to_num(spring_line[5])
		spring_l0_factor = to_num(spring_line[6])
		spring_damping_factor = to_num(spring_line[7])
		MSS_return.springs.append(Spring(MSS_return.masses[spring_mass_ini_ID], \
			MSS_return.masses[spring_mass_end_ID], spring_k, spring_l0, spring_damping ,\
			spring_k_factor, spring_l0_factor, spring_damping_factor))

	# /spring line
	line =  next(readCVS)
	if line[0] != "/spring":
		print( "Expected tag \"/spring\" not found in line: ", line)
		sys.exit(2)

	return MSS_return


##################################################################
# MAIN SUB
##################################################################
def handlerForInterruption(signal_received, frame):
	# Handle breaks
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	exit(0)

def main(main_args) :
	global  pg_MSS_current

	##################################################################
	# ARGUMENTS
	##################################################################
	configuration_file_name = ''
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
	# print( 'Input scenario file is ', configuration_file_name)

	try:
		FILEin = open(configuration_file_name, "rt")
	except IOError:
		print("File not found :", configuration_file_name, " or path is incorrect")

	pg_MSS_current = MSS_init(FILEin)

	return pg_MSS_current

	# ##################################################################
	# # Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	# ##################################################################
	# signal(SIGINT, handlerForInterruption)

	# ##################################################################
	# # MSS ANIMATION
	# ##################################################################
	# nb_steps = 0
	# while(True) :
	# 	nb_steps += 1
	# 	for a_mass in pg_MSS_current.masses.values():
	# 		a_center = a_mass.center
	# 	pg_MSS_current.step()
	# 	print("")
	# 	sleep(0.1)


if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)
	main(sys.argv[1:])
