#!/usr/bin/python3 

# pg_sensors.py
# sensors capture and visual graphics through MSSsent to porphyrograph
# (c) Yukao Nagemi

# usage: python3 pg_sensors

from mbientlab.metawear import *
from mbientlab.metawear.cbindings import *
from mbientlab.warble import * 
import six

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
from vv_lib import force_num

from pg_lib import Vector
from pg_lib import Point

# mass spring system
import pg_mss

# OSC
import pg_OSC

# KEYBOARD HOT KEYS
import keyboard

# OSC
from osc4py3 import oscmethod as osm

# XMM
import pg_train_reco_xmm

# NumPy
import numpy as np

if sys.version_info[0] == 2:
	range = xrange

OSC_emission_timeStep = 0.05

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
ConnectedSensorStates = []

##################################################################
# SENSOR
##################################################################
class SensorConfigParams:
	def __init__(self, sensor_ID, MAC_address, min_conn_interval, max_conn_interval, latency, timeout, sensor_acc_factor):
		self.sensor_ID = sensor_ID
		self.sensor_MAC_address = MAC_address
		self.min_conn_interval = min_conn_interval
		self.max_conn_interval = max_conn_interval
		self.latency = latency
		self.timeout = timeout
		self.sensor_acc_factor = sensor_acc_factor

##################################################################
# SENSOR EVENTS MANAGEMENT
##################################################################
class State:
	def __init__(self, device, ID, sensor_data, sensors_sample_frequency, \
		METAWEAR_forward_sensor_acceleration):
		self.device = device
		self.ID = ID
		self.min_conn_interval = sensor_data.min_conn_interval
		self.max_conn_interval = sensor_data.max_conn_interval
		self.latency = sensor_data.latency
		self.timeout = sensor_data.timeout
		self.sensor_acc_factor = sensor_data.sensor_acc_factor
		self.METAWEAR_forward_sensor_acceleration = METAWEAR_forward_sensor_acceleration
		self.sensors_sample_frequency = sensors_sample_frequency

		self.nb_samples_EULER_ANGLE = 0
		self.last_posting_time_EULER_ANGLE = time()
		self.sensor_eulerAngle = Vector(0., 0., 0.)
		self.sensor_heading = 0.

		self.nb_samples_LINEAR_ACC = 0
		self.last_posting_time_LINEAR_ACC = time()
		self.sensor_linAcc_base = Vector(0., 0., 0.)
		self.sensor_linAcc = Vector(0., 0., 0.)
		# SENSOR FUSION
		self.callback = FnVoid_VoidP_DataP(self.data_handler)

	##################################################################
	# SENSOR VALUES CALLBACK
	def data_handler(self, ctx, data):
		values = parse_value(data, n_elem = 1)
		curTime = time()
		# EULER ANGLES
		if values.__repr__().find("heading") >= 0:
			post_interval_EULER_ANGLE = curTime - self.last_posting_time_EULER_ANGLE
			if post_interval_EULER_ANGLE >= self.sensors_sample_frequency:
				# updates last sampling time
				self.last_posting_time_EULER_ANGLE = curTime;
				# update count
				self.nb_samples_EULER_ANGLE += 1

				#reads the values
				self.sensor_eulerAngle = Vector(values.pitch, values.roll, values.yaw)
				self.sensor_heading = values.heading

				# print("device: %s, EULER %s\n" % (self.device.address, values))
				# console printing
				if pg_train_reco_xmm.getGMM() != None :
					if self.nb_samples_EULER_ANGLE % 10 == 0 and pg_train_reco_xmm.getGMM().xmm_train == True :
						print("device: %s, EULER %f %f %f %f" % (self.ID, self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz))

				# LIVE OSC messages					
				# if self.METAWEAR_forward_sensor_acceleration == True :
				# 	pg_OSC.UDP_emit("/mw_euler", "udp_porphyrograph_send", [self.ID, self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz])

		# LINEAR ACCELERATION
		else:
			post_interval_LINEAR_ACC = curTime - self.last_posting_time_LINEAR_ACC
			if post_interval_LINEAR_ACC >= self.sensors_sample_frequency:
				# updates last sampling time
				self.last_posting_time_LINEAR_ACC = curTime;
				# update count
				self.nb_samples_LINEAR_ACC += 1

				#reads the values
				self.sensor_linAcc_base = Vector(values.x, values.y, values.z)
				# multiplies the sensor acceleration by an impact factor
				self.sensor_linAcc = self.sensor_linAcc_base * self.sensor_acc_factor

				# print("device: %s, ACCEL %s\n" % (self.device.address, values))
				# console printing
				if pg_train_reco_xmm.getGMM() != None :
					if self.nb_samples_LINEAR_ACC % 10 == 0 and pg_train_reco_xmm.getGMM().xmm_train == True :
						print("device: %s, ACCEL %f %f %f" % (self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz))

				# LIVE OSC messages					
				# if self.METAWEAR_forward_sensor_acceleration == True :
				# 	pg_OSC.UDP_emit("/mw_linAcc", "udp_porphyrograph_send", [self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz])
				if self.METAWEAR_forward_sensor_acceleration == True :
					if self.ID == "mw1" :
						acc_norm = self.sensor_linAcc.norm()
						pg_OSC.UDP_emit("/mw/linAcc1", "udp_LIVE_send", [acc_norm])
						pg_OSC.UDP_emit("/globaltrajectoryspeed", "udp_PYTHON_send", [(acc_norm*10.0)])
						# print("/globaltrajectoryspeed", "udp_PYTHON_send", [(acc_norm*10.0)])
					elif self.ID == "mw2" :
						acc_norm = self.sensor_linAcc.norm()
						pg_OSC.UDP_emit("/mw/linAcc2", "udp_LIVE_send", [acc_norm])
						pg_OSC.UDP_emit("/globalsourcespeed", "udp_PYTHON_send", [(acc_norm*10.0)])
						# print("/globalsourcespeed", "udp_PYTHON_send", [(acc_norm*10.0)])

				# MSS values updating (updates the forces on the masses linked to a sensor)
				if pg_mss.pg_MSS_current != None :
					pg_mss.pg_MSS_current.update_linAcc(self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz)
					# print("MSS updating: %s, ACCEL %f %f %f" % (self.ID, self.sensor_linAcc.vx, self.sensor_linAcc.vy, self.sensor_linAcc.vz))

			##################################################################
			# sends it to the XMM library if training or recognition is on
			if pg_train_reco_xmm.getGMM() != None :
				if pg_train_reco_xmm.getGMM().xmm_train == True :
					pg_train_reco_xmm.getGMM().gmm_train_on_the_fly(self.ID, np.array([self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz, self.sensor_linAcc_base.vx, self.sensor_linAcc_base.vy, self.sensor_linAcc_base.vz]))
				elif pg_train_reco_xmm.getGMM().xmm_reco == True :
					pg_train_reco_xmm.getGMM().gmm_reco_on_the_fly(self.ID, np.array([self.sensor_heading, self.sensor_eulerAngle.vx, self.sensor_eulerAngle.vy, self.sensor_eulerAngle.vz, self.sensor_linAcc_base.vx, self.sensor_linAcc_base.vy, self.sensor_linAcc_base.vz]))

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
	global ConnectedSensorStates

	for s in ConnectedSensorStates:
		s.sensor_disconnect()

	print("Total samples emitted")
	for s in ConnectedSensorStates:
		print("	%s -> angle %d acc %d tot %d" % (s.ID, s.nb_samples_EULER_ANGLE, s.nb_samples_LINEAR_ACC, s.nb_samples_EULER_ANGLE+s.nb_samples_LINEAR_ACC))

	# OSC closing.
	pg_OSC.osc_terminate()

##################################################################
# READS THE CONFIGURATION LINE BY LINE
##################################################################
def SensorConfigurationData(readCSV):
	# Sensor initialization
	sensor_config_param = dict()

	################################################
	# sensor lines
	line = next(readCSV)
	if line[0] != "sensor":
		print( "Expected tag \"sensor\" not found in line: ", line)
		sys.exit(2)
	nb_sensors = to_num(line[1])
	
	line = next(readCSV) 
	# print( "line1 ", line )

	for ind_sensor in range(nb_sensors):
		sensor_line = next(readCSV) 
		sensor_ID = sensor_line[0]
		sensor_MAC_address = sensor_line[1]
		min_conn_interval = to_num(sensor_line[2])
		max_conn_interval = to_num(sensor_line[3])
		latency = to_num(sensor_line[4])
		timeout = to_num(sensor_line[5])
		sensor_acc_factor = to_num(sensor_line[6])

		if(not(sensor_ID in sensor_config_param.keys())):
			sensor_config_param[sensor_ID] = SensorConfigParams(sensor_ID, sensor_MAC_address, min_conn_interval, max_conn_interval, latency, timeout, sensor_acc_factor)
		else:
			print("Duplicate sensor ID: ", sensor_ID)
			sys.exit(2)

		print( "Sensor #{} id {}".format(ind_sensor+1, sensor_ID) )

	# /sensor line
	line = next(readCSV)
	if line[0] != "/sensor":
		print( "Expected tag \"/sensor\" not found in line: ", line)
		sys.exit(2)

	################################################
	# retunrs the SensorParams
	return sensor_config_param

##################################################################
# OSC RECEIVED MESSAGES PROCESSING
def UDP_receive_sensor_acc_factor_handler(address, arg):
	global ConnectedSensorStates

	# print("received OSC {", address, "}: {", arg, "} {", force_num(arg), "}")
	for s in ConnectedSensorStates:
		s.sensor_acc_factor = force_num(arg)
	pg_OSC.UDP_emit(address, "udp_TouchOSC_send", [sensor_acc_factor_value()])
	# print("New value {", sensor_acc_factor_value(), "}")

def sensor_acc_factor_value():
	global ConnectedSensorStates
	if ConnectedSensorStates != [] :
		return ConnectedSensorStates[0].sensor_acc_factor
	return 1

##################################################################
# MAIN SUB
##################################################################
def handlerForInterruption(signal_received, frame):
	# Handle breaks
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	disconnect()
	exit(0)

def main(main_args) :
	global ConnectedSensorStates

	##################################################################
	# ARGUMENTS
	##################################################################
	OSC_sensor_MSS_configuration_file_name = ''
	# sensor_configuration_file_name = ''
	# MSS_configuration_file_name = ''
	# OSC_configuration_file_name = ''
	MSS_scenario_file_name = ''
	XMM_training_data_dir_name = ''
	sensors_sample_frequency = 0.15
	XMM_likelihood_window = 40
	XMM_likelihood_threshold = 0.7
	XMM_likelihood_minimal_duration = 1.0
	XMM_forward_gesture_recognition = True
	XMM_forward_web_animation = True
	XMM_forward_sensor_masses_animation = True
	METAWEAR_forward_sensor_acceleration = True

	try:
		opts, args = getopt.getopt(main_args,"c:s:",["configurationfile=","scenario=","training-data=","training-sensors-ID=","sensors-sample-frequency=","likelihood-window=","training-labels=","training-label-ClipArts=","likelihood-threshold=","likelihood-minimal-duration=","forward-gesture-recognition=","forward-web-animation=","forward-sensor-masses-animation=","forward-sensor-acceleration="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-c", "--configurationfile"):
			OSC_sensor_MSS_configuration_file_name = arg
		elif opt in ("--scenario"):
			MSS_scenario_file_name = arg
		elif opt in ("--training-sensors-ID"):
			XMM_sensors_ID = arg
		elif opt in ("--sensors-sample-frequency"):
			sensors_sample_frequency = arg
		elif opt in ("--training-data"):
			XMM_training_data_dir_name = arg
		elif opt in ("--likelihood-window"):
			XMM_likelihood_window = arg
		elif opt in ("--training-labels"):
			XMM_training_labels = arg
		elif opt in ("--training-label-ClipArts"):
			XMM_training_labels_ClipArts = arg
		elif opt in ("--likelihood-threshold"):
			XMM_likelihood_threshold = arg
		elif opt in ("--likelihood-minimal-duration"):
			XMM_likelihood_minimal_duration = arg
		elif opt in ("--forward-gesture-recognition"):
			XMM_forward_gesture_recognition = arg
		elif opt in ("--forward-web-animation"):
			XMM_forward_web_animation = arg
		elif opt in ("--forward-sensor-masses-animation"):
			XMM_forward_sensor_masses_animation = arg
		elif opt in ("--forward-sensor-acceleration"):
			METAWEAR_forward_sensor_acceleration = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', configuration_file_name)

	if len(XMM_training_labels) !=  len(XMM_training_labels_ClipArts) :
		print("the size of the training labels should be the same as the size of the associated ClipArt indices")
		sys.exit(0)

	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	##################################################################
	# CONFIGURATION FILE OPENING
	try:
		FILEin = open(OSC_sensor_MSS_configuration_file_name, "rt")
	except IOError:
		print("File not found :", OSC_configuration_file_name, " or path is incorrect")

	OSC_sensor_MSS_readCSV = csv.reader(FILEin, delimiter=',')

	####################
	# OSC INITIALIZATION
	pg_OSC.OSC_init(OSC_sensor_MSS_readCSV)

	###########################
	# SENSOR SET INITIALIZATION
	sensor_config_param = SensorConfigurationData(OSC_sensor_MSS_readCSV)

	####################
	# MSS INITIALIZATION
	pg_mss.pg_MSS_current = pg_mss.MSS_init(OSC_sensor_MSS_readCSV)

	# CONFIGURATION FILE CLOSING
	FILEin.close()

	##################################################################
	# MSS SCENARIO
	if(MSS_scenario_file_name != '') :
		pg_mss.pg_MSS_current.MSS_scenario(MSS_scenario_file_name)
	print("scenario loaded")

	##################################################################
	# SENSORS CONNECTION AND UDP LINK TO PORPHYROGRAPH (SENSOR VALUE AND/OR MSS VALUES)
	print("scanning for devices...")
	devices = {}
	def handler(result):
		devices[result.mac] = result.name

	BleScanner.set_handler(handler)
	BleScanner.start()

	sleep(10.0)
	BleScanner.stop()

	# scans the BLE devices around and reports their IDs and MAC addresses
	i = 0
	list_of_connectable_sensors = []
	for address, name in six.iteritems(devices):
		print("[%d] %s (%s)" % (i, address, name))
		if name == "MetaWear" :
			for sensor_ID, one_sensor_config_param in sensor_config_param.items():
				if one_sensor_config_param.sensor_MAC_address == address :
					list_of_connectable_sensors.append([sensor_ID, one_sensor_config_param])
		i += 1

	# tries to connect to the available MetaWear devices among the ones given in the configuration file
	ConnectedSensorStates = []
	ConnectedSensorIDs = []
	for sensor_ID, one_sensor_config_param in list_of_connectable_sensors :
		if sensor_ID in XMM_sensors_ID :
			print( "connect to sensor: ", sensor_ID, " address: ", one_sensor_config_param.sensor_MAC_address)
			d = MetaWear(one_sensor_config_param.sensor_MAC_address)
			try_count = 0
			while True:
				# due to the likely connection failure, the exception is caught so that to have the possibility to retry
				try :
					d.connect()
					print("Connected to " + d.address)
					if("udp_PYTHON_send" in pg_OSC.UDP_clients.keys()):
						s = State(d, sensor_ID, one_sensor_config_param, sensors_sample_frequency, \
							METAWEAR_forward_sensor_acceleration)
						ConnectedSensorStates.append(s)
						ConnectedSensorIDs.append(s.ID)
						s.sensor_data_fusion_configuration()
						# used for sampling rate
						s.last_posting_time_EULER_ANGLE = time();
						s.last_posting_time_LINEAR_ACC = time();

						pattern= LedPattern(repeat_count= 2)
						libmetawear.mbl_mw_led_load_preset_pattern(byref(pattern), LedPreset.BLINK)
						libmetawear.mbl_mw_led_write_pattern(d.board, byref(pattern), LedColor.GREEN)
						libmetawear.mbl_mw_led_play(d.board)
					else:
						print('missing "udp_PYTHON_send" UDP client')
						exit(0)
					break
				# in case of failure tries 3 connections in a row
				except WarbleException :
					try_count += 1
					if try_count >= 2 :
						print("**** Failed connection to " + d.address + "  ****")
						break
					else :
						print("Failed connection, retries...")
						sleep(1.)
			sleep(1.)
		# only one connected sensor is used currently due to the difficulty of doubling the connection
		if len(ConnectedSensorStates) > 0 :
			break

	if len(ConnectedSensorStates) <= 0 :
		print("No sensor connected")
		sys.exit(0)

	##################################################################
	# INIIALIZES  with the setup parameter values 
	print("******************* ", ConnectedSensorIDs[0] , " ***************************")
	print("New GMM for sensor IDs", ConnectedSensorIDs)
	pg_train_reco_xmm.setGMM(ConnectedSensorIDs, XMM_training_data_dir_name, XMM_likelihood_window, XMM_training_labels, XMM_training_labels_ClipArts, XMM_likelihood_threshold, XMM_likelihood_minimal_duration, XMM_forward_gesture_recognition, XMM_forward_web_animation, XMM_forward_sensor_masses_animation)
	if pg_train_reco_xmm.getGMM() == None :
		print("GMM initialization failed")
		sys.exit(0)
	else :
		print("GMM initialization succeeded")
	# pg_train_reco_xmm.getGMM().gmm_on_off_train1("/train1", "1")

	##################################################################
	# SENSORS CONFIGURATION FOR DATA FUSTION 
	# AND COLLECTION OF ORIENTATION (EULER ANGLES) + LINEAR ACCELERATION 
	# for s in ConnectedSensorStates:
		# s.sensor_data_fusion_configuration()

		# # used for sampling rate
		# s.last_posting_time_EULER_ANGLE = time();
		# s.last_posting_time_LINEAR_ACC = time();
	print("sensors configured")

	##################################################################
	# OPENS the log file 
	if(False) :
		try:
			csvfile = open(MSS_log_file_name, "wt", newline='')
		except IOError:
			print("File not created :", transformation_scenario_file_name, " or path is incorrect")
		logwriter = csv.writer(csvfile, delimiter=',')
		pg_mss.pg_MSS_current.log_mass_positions_header(logwriter)

	##################################################################
	# SENSOR SET CALLBACKS FOR OSC MESSAGES (BOTH FOR SENSORS AND FOR MSS)
	pg_mss.OSC_dispatcher_mapping()

	##################################################################
	# INIIALIZES TouchOSC with the setup parameter values 
	pg_mss.OSC_emitter_resend_all()

	##################################################################
	# DEFINES hot keys
	# keyboard.add_hotkey('l', pg_mss.UDP_receive_mss_launch_handler, args=['/mss_launch', 1])
	# keyboard.add_hotkey('q', pg_mss.UDP_receive_mss_quit_handler, args=['/mss_quit', 1])

	##################################################################
	# launches the MSS scenario 
	# (optional, can also be commented and launched with 'l' hotkey)
	pg_mss.UDP_receive_mss_launch_handler('/mss_launch', [])

 	##################################################################
	# main loop
	indstep = 0
	while(not pg_mss.pg_MSS_current.terminated) :
		##################################################################
		# SENSOR SET CALLBACKS FOR OSC MESSAGES (BOTH FOR SENSORS AND FOR MSS)
		# MSS dynamics, mass/spring positions updating, and mass position forwarding to porphyrograph for drawing
		pg_mss.pg_MSS_current.step()	
		pg_mss.pg_MSS_current.send_mass_positions_values()	
		if indstep % 20 == 0:	
			pg_mss.OSC_emitter_resend_all()		
		if(False) :
			pg_mss.pg_MSS_current.log_mass_positions_values(logwriter)		

		for i in range(30) :
			pg_OSC.osc_process()
		sleep(OSC_emission_timeStep)
		indstep += 1

	##################################################################
	# DISCONNECTION
	disconnect()

if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
