#!/usr/bin/python3 

# pg_sensors.py
# sensors capture and visual graphics through MSSsent to porphyrograph
# (c) Yukao Nagemi

# usage: python3 pg_sensors

from mbientlab.metawear import MetaWear, libmetawear, parse_value
from mbientlab.metawear.cbindings import *

from time import sleep
from time import time
from threading import Event

from signal import signal, SIGINT
from sys import exit

import getopt
import platform
import sys

# CVS file reading line by line
import csv

# library to convert between strings and nums
from vv_lib import to_num

# OSC
from pythonosc.udp_client import SimpleUDPClient
from pythonosc.osc_server import AsyncIOOSCUDPServer
from pythonosc.dispatcher import Dispatcher
import asyncio

# mass spring system
import pg_mss

if sys.version_info[0] == 2:
	range = xrange

##################################################################
# USAGE
##################################################################
USAGE = '''pg_sensors.py -s sensor_configuration_file -i MSS_configuration_file
	ARGUMENTS:
	-s: sensor configuration_file
	-i: MSS configuration_file
'''

###############################################
# GLOBAL VARIABLES
###############################################
SensorSet_current = None
pg_MSS_current = None
OSC_dispatcher = None

##################################################################
# SENSOR
##################################################################
class Sensor:
	def __init__(self, MAC_address, min_conn_interval, max_conn_interval, latency, timeout, sensor_acc_factor):
		self.sensor_MAC_address = MAC_address
		self.min_conn_interval = min_conn_interval
		self.max_conn_interval = max_conn_interval
		self.latency = latency
		self.timeout = timeout
		self.sensor_acc_factor = sensor_acc_factor

##################################################################
# OSC SERVERS AND CLIENTS
##################################################################
class UDP_server:
	def __init__(self, Port):
		self.port = Port

class UDP_client:
	def __init__(self, IP_address, Port):
		self.IP_address = IP_address
		self.port = Port
		self.client = SimpleUDPClient(IP_address, Port)

##################################################################
# SET OF SENSORS
##################################################################
class SensorSet:
	# The system.
	def __init__(self, sample_frequency, UDP_sensor_state_fowarding, MSS_sensor_state_fowarding):
		# Construct a system.
		self.sensors_sample_frequency = sample_frequency
		self.UDP_sensor_state_fowarding = UDP_sensor_state_fowarding
		self.MSS_sensor_state_fowarding = MSS_sensor_state_fowarding
		self.sensors = dict()
		self.UDP_servers = dict()
		self.UDP_clients = dict()

# sensors_mac_addresses = ['E9:0F:0A:C5:4D:66', 'C7:7D:6A:41:B4:15']
# sensors_ids = ['mw1', mw2']
# sensors_sample_frequency = 0.15 # 60 FPS

##################################################################
# SENSOR EVENTS MANAGEMENT
##################################################################
class State:
	def __init__(self, device, ID, sensor_data, sample_frequency, \
		UDP_sensor_state_fowarding, MSS_sensor_state_fowarding, pg_UDP_client):
		self.device = device
		self.ID = ID
		self.min_conn_interval = sensor_data.min_conn_interval
		self.max_conn_interval = sensor_data.max_conn_interval
		self.latency = sensor_data.latency
		self.timeout = sensor_data.timeout
		self.sensor_acc_factor = sensor_data.sensor_acc_factor
		self.pg_UDP_client = pg_UDP_client
		self.UDP_sensor_state_fowarding = UDP_sensor_state_fowarding
		self.MSS_sensor_state_fowarding = MSS_sensor_state_fowarding
		self.sample_frequency = sample_frequency

		self.nb_samples_EULER_ANGLE = 0
		self.last_posting_time_EULER_ANGLE = time()
		self.sensor_eulerAngle = pg_mss.Vector(0., 0., 0.)
		self.sensor_heading = 0.

		self.nb_samples_LINEAR_ACC = 0
		self.last_posting_time_LINEAR_ACC = time()
		self.sensor_linAcc = pg_mss.Vector(0., 0., 0.)
		# SENSOR FUSION
		self.callback = FnVoid_VoidP_DataP(self.data_handler)

	##################################################################
	# SENSOR VALUES CALLBACK
	def data_handler(self, ctx, data):
		global pg_MSS_current
		global SensorSet_current

		# from cbindings: 
		# class EULERAngles(Structure):
		#	 _fields_ = [
		#		 ("heading" , c_float),
		#		 ("pitch" , c_float),
		#		 ("roll" , c_float),
		#		 ("yaw" , c_float)
		#	 ]
		# class CartesianFloat(Structure):
		#	 _fields_ = [
		#		 ("x" , c_float),
		#		 ("y" , c_float),
		#		 ("z" , c_float)
		#	 ]
		values = parse_value(data, n_elem = 1)
		curTime = time()
		# EULER ANGLES
		if values.__repr__().find("heading") >= 0:
			post_interval_EULER_ANGLE = curTime - self.last_posting_time_EULER_ANGLE
			if post_interval_EULER_ANGLE >= self.sample_frequency:
				# updates last sampling time
				self.last_posting_time_EULER_ANGLE = curTime;
				# update count
				self.nb_samples_EULER_ANGLE += 1

				#reads the values
				self.sensor_eulerAngle = pg_mss.Vector(values.pitch, values.roll, values.yaw)
				self.sensor_heading = values.heading

				# print("device: %s, EULER %s\n" % (self.device.address, values))
				# console printing
				if(self.nb_samples_EULER_ANGLE % 10 == 0):
					print("device: %s, EULER %f %f %f %f" % (self.ID, self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz))

				# porphyrograph OSC messages					
				if(self.UDP_sensor_state_fowarding == True):
					self.pg_UDP_client.send_message("/mw_euler", [self.ID, self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz])						

				# MSS values updating (nothing for the moment)
				if(pg_MSS_current != None and self.MSS_sensor_state_fowarding == True):
					pg_MSS_current.update_Euler(self.ID, self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz)						

		# LINEAR ACCELERATION
		else:
			post_interval_LINEAR_ACC = curTime - self.last_posting_time_LINEAR_ACC
			if post_interval_LINEAR_ACC >= self.sample_frequency:
				# updates last sampling time
				self.last_posting_time_LINEAR_ACC = curTime;
				# update count
				self.nb_samples_LINEAR_ACC += 1

				#reads the values
				self.sensor_linAcc = pg_mss.Vector(values.x, values.y, values.z) * self.sensor_acc_factor

				# print("device: %s, ACCEL %s\n" % (self.device.address, values))
				# console printing
				if(self.nb_samples_LINEAR_ACC % 10 == 0):
					print("device: %s, ACCEL %f %f %f" % (self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz))

				# porphyrograph OSC messages					
				if(self.UDP_sensor_state_fowarding == True):
					self.pg_UDP_client.send_message("/mw_linAcc", [self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz])

				# MSS values updating (updates the forces on the masses linked to a sensor)
				if(pg_MSS_current != None and self.MSS_sensor_state_fowarding == True):
					pg_MSS_current.update_linAcc(self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz)
					# MSS dynamics, mass/spring positions updating, and mass position forwarding to porphyrograph for drawing
					pg_MSS_current.step()			

	##################################################################
	# SENSORS CONFIGURATION FOR DATA FUSTION 
	# AND COLLECTION OF ORIENTATION (EULER ANGLES) + LINEAR ACCELERATION 
	def sensor_data_fusion_configuration(self):
		print("Configuring device")
		# libmetawear.mbl_mw_settings_set_connection_parameters(self.device.board, 7.5, 7.5, 0, 6000)
		print("Connection parameters: ", self.min_conn_interval, " ", self.max_conn_interval, " ", self.latency, " ", self.timeout)
		libmetawear.mbl_mw_settings_set_connection_parameters(self.device.board, self.min_conn_interval, self.max_conn_interval, self.latency, self.timeout)
		sleep(1.5)

		libmetawear.mbl_mw_sensor_fusion_set_mode(self.device.board, SensorFusionMode.NDOF);
		# * Supported acceleration ranges (sensitivity of the sensor)
		#	 MBL_MW_SENSOR_FUSION_ACC_RANGE_2G = 0,
		#	 MBL_MW_SENSOR_FUSION_ACC_RANGE_4G,
		#	 MBL_MW_SENSOR_FUSION_ACC_RANGE_8G,
		#	 MBL_MW_SENSOR_FUSION_ACC_RANGE_16G
		libmetawear.mbl_mw_sensor_fusion_set_acc_range(self.device.board, SensorFusionAccRange._8G)
		# * Supported rotation ranges (degrees per second)
		#	 MBL_MW_SENSOR_FUSION_GYRO_RANGE_2000DPS = 0,
		#	 MBL_MW_SENSOR_FUSION_GYRO_RANGE_1000DPS,
		#	 MBL_MW_SENSOR_FUSION_GYRO_RANGE_500DPS,
		#	 MBL_MW_SENSOR_FUSION_GYRO_RANGE_250DPS
		libmetawear.mbl_mw_sensor_fusion_set_gyro_range(self.device.board, SensorFusionGyroRange._2000DPS)
		libmetawear.mbl_mw_sensor_fusion_write_config(self.device.board)

		signal_EULER_ANGLE = libmetawear.mbl_mw_sensor_fusion_get_data_signal(self.device.board, SensorFusionData.EULER_ANGLE);
		libmetawear.mbl_mw_datasignal_subscribe(signal_EULER_ANGLE, None, self.callback)

		signal_LINEAR_ACC = libmetawear.mbl_mw_sensor_fusion_get_data_signal(self.device.board, SensorFusionData.LINEAR_ACC);
		libmetawear.mbl_mw_datasignal_subscribe(signal_LINEAR_ACC, None, self.callback)

		# SensorFusionData.CORRECTED_ACC: False,
		# SensorFusionData.CORRECTED_GYRO: False,
		# SensorFusionData.CORRECTED_MAG: False,
		# SensorFusionData.QUATERNION: False,
		# SensorFusionData.EULER_ANGLE: False,
		# SensorFusionData.GRAVITY_VECTOR: False
		# SensorFusionData.LINEAR_ACC: False,
		libmetawear.mbl_mw_sensor_fusion_enable_data(self.device.board, SensorFusionData.EULER_ANGLE);
		libmetawear.mbl_mw_sensor_fusion_enable_data(self.device.board, SensorFusionData.LINEAR_ACC);

		# launches the thread for listening to the board and collecting the values with the callback
		libmetawear.mbl_mw_sensor_fusion_start(self.device.board);

	##################################################################
	# SENSOR DISCONNECTION BEFORE EXITING
	def sensor_disconnect(self):
		libmetawear.mbl_mw_sensor_fusion_stop(self.device.board);

		signal_EULER_ANGLE = libmetawear.mbl_mw_sensor_fusion_get_data_signal(self.device.board, SensorFusionData.EULER_ANGLE);
		signal_LINEAR_ACC = libmetawear.mbl_mw_sensor_fusion_get_data_signal(self.device.board, SensorFusionData.LINEAR_ACC);
		libmetawear.mbl_mw_datasignal_unsubscribe(signal_EULER_ANGLE)
		libmetawear.mbl_mw_datasignal_unsubscribe(signal_LINEAR_ACC)
		libmetawear.mbl_mw_debug_disconnect(self.device.board)

##################################################################
# SENSOR DISCONNECTION BEFORE EXITING
##################################################################
def disconnect():
	global SensorSet_current

	for s in SensorSet_current.states:
		s.sensor_disconnect()

	print("Total samples emitted")
	for s in SensorSet_current.states:
		print("	%s -> angle %d acc %d tot %d" % (s.ID, s.nb_samples_EULER_ANGLE, s.nb_samples_LINEAR_ACC, s.nb_samples_EULER_ANGLE+s.nb_samples_LINEAR_ACC))

	# OSC server closing
	for server in SensorSet_current.UDP_servers.values():
		server.transport.close()  # Clean up serve endpoint

##################################################################
# READS THE CONFIGURATION LINE BY LINE
##################################################################
def SensorSet_init(FILEin):
	readCVS = csv.reader(FILEin, delimiter=',')

	##################################################################
	# SENSORS INITIALIZATION
	line = next(readCVS)
	if line[0] != "SensorSet":
		print( "Expected tag \"SensorSet\" not found in line: ", line)
		sys.exit(2)
	
	line = next(readCVS) 
	# print( "line1 ", line )

	SensorSet_line = next(readCVS) 
	# print( "line2 ", MSS_line )

	nb_sensors = to_num(SensorSet_line[0])
	SensorSet_sample_frequency = to_num(SensorSet_line[1])

	nb_UDP_servers = to_num(SensorSet_line[2])
	nb_UDP_clients = to_num(SensorSet_line[3])

	UDP_sensor_state_fowarding = (SensorSet_line[4] == 'True')
	MSS_sensor_state_fowarding = (SensorSet_line[5] == 'True')

	# /MSS line
	line = next(readCVS)
	if line[0] != "/SensorSet":
		print( "Expected tag \"/SensorSet\" not found in line: ", line)
		sys.exit(2)

	# MSS initialization
	SensorSet_return = SensorSet(SensorSet_sample_frequency, UDP_sensor_state_fowarding, MSS_sensor_state_fowarding)

	################################################
	# sensor lines
	line = next(readCVS)
	if line[0] != "sensor":
		print( "Expected tag \"sensor\" not found in line: ", line)
		sys.exit(2)
	
	line = next(readCVS) 
	# print( "line1 ", line )

	for ind_sensor in range(nb_sensors):
		sensor_line = next(readCVS) 
		# print( "line2 ", spring_line )
		sensor_ID = sensor_line[0]
		sensor_MAC_address = sensor_line[1]
		min_conn_interval = to_num(sensor_line[2])
		max_conn_interval = to_num(sensor_line[3])
		latency = to_num(sensor_line[4])
		timeout = to_num(sensor_line[5])
		sensor_acc_factor = to_num(sensor_line[6])

		if(not(sensor_ID in SensorSet_return.sensors.keys())):
			SensorSet_return.sensors[sensor_ID] = Sensor(sensor_MAC_address, min_conn_interval, max_conn_interval, latency, timeout, sensor_acc_factor)
		else:
			print("Duplicate sensor ID: ", sensor_ID)
			sys.exit(2)

	# /sensor line
	line = next(readCVS)
	if line[0] != "/sensor":
		print( "Expected tag \"/sensor\" not found in line: ", line)
		sys.exit(2)

	################################################
	# UDP_server lines
	line = next(readCVS)
	if line[0] != "UDP_server":
		print( "Expected tag \"UDP_server\" not found in line: ", line)
		sys.exit(2)
	
	line = next(readCVS) 
	# print( "line1 ", line )

	for ind_UDP_server in range(nb_UDP_servers):
		UDP_server_line = next(readCVS) 
		# print( "line2 ", UDP_server_line )
		UDP_server_ID = UDP_server_line[0]
		UDP_server_port = int(to_num(UDP_server_line[1]))
		if(not(UDP_server_ID in SensorSet_return.UDP_servers.keys())):
			SensorSet_return.UDP_servers[UDP_server_ID] = UDP_server(UDP_server_port)
		else:
			print("Duplicate UDP_server ID: ", UDP_server_ID)
			sys.exit(2)

	# /UDP_server line
	line = next(readCVS)
	if line[0] != "/UDP_server":
		print( "Expected tag \"/UDP_server\" not found in line: ", line)
		sys.exit(2)

	################################################
	# UDP_client lines
	line = next(readCVS)
	if line[0] != "UDP_client":
		print( "Expected tag \"UDP_client\" not found in line: ", line)
		sys.exit(2)
	
	line = next(readCVS) 
	# print( "line1 ", line )

	for ind_UDP_client in range(nb_UDP_clients):
		UDP_client_line = next(readCVS) 
		# print( "line2 ", UDP_client_line )
		UDP_client_ID = UDP_client_line[0]
		UDP_client_IP_address = UDP_client_line[1]
		UDP_client_port = to_num(UDP_client_line[2])
		if(not(UDP_client_ID in SensorSet_return.UDP_clients.keys())):
			SensorSet_return.UDP_clients[UDP_client_ID] = UDP_client(UDP_client_IP_address, UDP_client_port)
		else:
			print("Duplicate UDP_client ID: ", UDP_client_ID)
			sys.exit(2)

	# /UDP_client line
	line = next(readCVS)
	if line[0] != "/UDP_client":
		print( "Expected tag \"/UDP_client\" not found in line: ", line)
		sys.exit(2)

	################################################
	# retunrs the SensorSet
	return SensorSet_return

##################################################################
# OSC COMMUNICATION
##################################################################
def	UDP_emit(address, UDP_client_ID, arg_list) :
	if(UDP_client_ID in SensorSet_current.UDP_clients.keys()):
		SensorSet_current.UDP_clients[UDP_client_ID].client.send_message(address, arg_list)						
	else:
		print('missing "', UDP_client_ID, '" UDP client')
		exit(0)

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
def UDP_receive_sensor_acc_factor_handler(address, *args):
	global SensorSet_current
	print("received OSC {", address, "}: {", args, "}")
	for sensor in SensorSet_current.sensors.values() :
		sensor.sensor_acc_factor = to_num(args[0])
	UDP_emit(address, "udp_TouchOSC_send", sensor_acc_factor_value())
def sensor_acc_factor_value():
	global SensorSet_current
	if(SensorSet_current.sensors.values()) :
		sensor = next(iter(SensorSet_current.sensors.values()))
		return [sensor.sensor_acc_factor]
	return [1]

##################################################################
# OSC MESSAGE RECEPTION DISPATCHING
def OSC_dispatecher_mapping():
	OSC_dispatcher.map("/mss_mass_mass_factor", pg_mss.UDP_receive_mss_mass_mass_factor_handler)
	OSC_dispatcher.map("/mss_mass_visc", pg_mss.UDP_receive_mss_mass_visc_handler)
	OSC_dispatcher.map("/mss_mass_grav", pg_mss.UDP_receive_mss_mass_grav_handler)
	OSC_dispatcher.map("/mss_spring_k_factor", pg_mss.UDP_receive_mss_spring_k_factor_handler)
	OSC_dispatcher.map("/mss_spring_l0_factor", pg_mss.UDP_receive_mss_spring_l0_factor_handler)
	OSC_dispatcher.map("/mss_spring_damp_factor", pg_mss.UDP_receive_mss_spring_damp_factor_handler)
	OSC_dispatcher.map("/mss_sensor_acc_factor", UDP_receive_sensor_acc_factor_handler)
	OSC_dispatcher.map("/mss_spring_mode", pg_mss.UDP_receive_mss_spring_mode_handler)

##################################################################
# OSC MESSAGE EMISSION DISPATCHING
def OSC_emitter_resend_all():
	UDP_emit("/mss_mass_mass_factor", "udp_TouchOSC_send", pg_mss.mss_mass_mass_factor_value())
	UDP_emit("/mss_mass_visc", "udp_TouchOSC_send", pg_mss.mss_mass_visc_value())
	UDP_emit("/mss_mass_grav", "udp_TouchOSC_send", pg_mss.mss_mass_grav_value())
	UDP_emit("/mss_spring_k_factor", "udp_TouchOSC_send", pg_mss.mss_spring_k_factor_value())
	UDP_emit("/mss_spring_l0_factor", "udp_TouchOSC_send", pg_mss.mss_spring_l0_factor_value())
	UDP_emit("/mss_spring_damp_factor", "udp_TouchOSC_send", pg_mss.mss_spring_damp_factor_value())
	UDP_emit("/mss_sensor_acc_factor", "udp_TouchOSC_send", sensor_acc_factor_value())
	UDP_emit("/mss_spring_mode", "udp_TouchOSC_send", pg_mss.mss_spring_mode_value())

##################################################################
# OSC ASYNC MESSAGE RECEPTION LOOP
async def OSC_loop():
	global SensorSet_current

	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	"""Example main loop that only runs for 10 iterations before finishing"""
	while(True):
		# print("OSC loop")
		await asyncio.sleep(SensorSet_current.sensors_sample_frequency/2.0)

##################################################################
# OSC MAIN SUB
async def OSC_main():
	global SensorSet_current
	global OSC_dispatcher

	for server in SensorSet_current.UDP_servers.values():
		server.OSC_server = AsyncIOOSCUDPServer(('0.0.0.0', server.port), OSC_dispatcher, asyncio.get_event_loop())
		server.transport, server.protocol = await server.OSC_server.create_serve_endpoint()  # Create datagram endpoint and start serving
		print("Opening OSC server on Port: %d" % (server.port))

	await OSC_loop()  # Enter main loop of program

	for server in SensorSet_current.UDP_servers.values():
   		server.transport.close()  # Clean up serve endpoint

##################################################################
# MAIN SUB
##################################################################
def handlerForInterruption(signal_received, frame):
	# Handle breaks
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	disconnect()
	exit(0)

def main(main_args) :
	global SensorSet_current
	global pg_MSS_current
	global OSC_dispatcher

	##################################################################
	# ARGUMENTS
	##################################################################
	configuration_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:s:",["inputfile=","sensors="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-s", "--sensors"):
			sensor_configuration_file_name = arg
		elif opt in ("-i", "--inputfile"):
			MSS_configuration_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', configuration_file_name)

	try:
		FILEin = open(sensor_configuration_file_name, "rt")
	except IOError:
		print("File not found :", sensor_configuration_file_name, " or path is incorrect")

	##################################################################
	# creates the OSC messages dispatcher used to define the servers in the sensor set initialization
	OSC_dispatcher = Dispatcher()

	##################################################################
	# SENSOR SET INITIALIZATION
	SensorSet_current = SensorSet_init(FILEin)

	##################################################################
	# MSS INITIALIZATION
	pg_MSS_current = pg_mss.main(["-i", MSS_configuration_file_name])

	##################################################################
	# SENSOR SET CALLBACKS FOR OSC MESSAGES (BOTH FOR SENSORS AND FOR MSS)
	OSC_dispatecher_mapping()

	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	##################################################################
	# SENSORS CONNECTION AND UDP LINK TO PORPHYROGRAPH (SENSOR VALUE AND/OR MSS VALUES)
	SensorSet_current.states = []
	for sensor_ID, sensor_data in SensorSet_current.sensors.items():
		print( "connect to sensor: ", sensor_ID, " address: ", sensor_data.sensor_MAC_address)
		d = MetaWear(sensor_data.sensor_MAC_address)
		d.connect()
		print("Connected to " + d.address)
		if("udp_porphyrograph_send" in SensorSet_current.UDP_clients.keys()):
			SensorSet_current.states.append(State(d, sensor_ID, sensor_data, SensorSet_current.sensors_sample_frequency, \
				SensorSet_current.UDP_sensor_state_fowarding, SensorSet_current.MSS_sensor_state_fowarding, \
				SensorSet_current.UDP_clients["udp_porphyrograph_send"].client))
		else:
			print('missing "udp_porphyrograph_send" UDP client')
			exit(0)

	##################################################################
	# SENSORS CONFIGURATION FOR DATA FUSTION 
	# AND COLLECTION OF ORIENTATION (EULER ANGLES) + LINEAR ACCELERATION 
	for s in SensorSet_current.states:
		s.sensor_data_fusion_configuration()

		# used for sampling rate
		s.last_posting_time_EULER_ANGLE = time();
		s.last_posting_time_LINEAR_ACC = time();

	##################################################################
	# INIIALIZES TouchOSC with the setup parameter values 
	OSC_emitter_resend_all()

	##################################################################
	# Runs the loop for receiving async OSC messages 
	asyncio.run(OSC_main())

	##################################################################
	# Sleeps while the threads run
	sleep(30000.0)
	disconnect()

if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
