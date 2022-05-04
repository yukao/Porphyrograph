#!/usr/bin/python3 

# pg_sensors.py
# sensors capture and visual graphics through MSSsent to porphyrograph
# (c) Yukao Nagemi

# usage: python3 pg_sensors

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

from pg_lib import Vector
from pg_lib import Point

# OSC
from osc4py3.as_eventloop import *
from osc4py3 import oscmethod as osm
from osc4py3 import oscbuildparse

# mass spring system
import pg_mss

import logging

if sys.version_info[0] == 2:
	range = xrange

###############################################
# LOGGINIG
###############################################
LOG_EVENTS =False

###############################################
# GLOBAL VARIABLES
###############################################
UDP_servers = dict()
UDP_clients = dict()
nb_UDP_servers = 0
nb_UDP_clients = 0

##################################################################
# OSC SERVERS AND CLIENTS
##################################################################
class UDP_server:
	def __init__(self, ID, Port):
		self.port = Port
		print("Opens OSC server %s on Port: %d" % (ID, Port))
		osc_udp_server("0.0.0.0", Port, ID)

class UDP_client:
	def __init__(self, ID, IP_address, Port):
		self.IP_address = IP_address
		self.port = Port
		print("Opens OSC client %s on Address %s Port: %d" % (ID, IP_address, Port))
		osc_udp_client(IP_address, Port, ID)

##################################################################
# OSC COMMUNICATION
##################################################################
def	UDP_emit(address, UDP_client_ID, arg_list) :
	global UDP_clients
	if(UDP_client_ID in UDP_clients.keys()):
		# print(address, UDP_client_ID, arg_list)
		msg = oscbuildparse.OSCMessage(address, None, arg_list)
		osc_send(msg, UDP_client_ID)
	else:
		print('missing "', UDP_client_ID, '" UDP client')
		exit(0)

##################################################################
# READS THE CONFIGURATION LINE BY LINE
##################################################################
def OSC_init(readCSV):
	global UDP_servers
	global UDP_clients
	global nb_UDP_servers
	global nb_UDP_clients
	global OSC_dispatcher
	global LOG_EVENTS

	# Start the system.
	if LOG_EVENTS == True :
		logging.basicConfig(format='%(asctime)s - %(threadName)s ø %(name)s - '
	    '%(levelname)s - %(message)s')
		logger = logging.getLogger("osc")
		logger.setLevel(logging.DEBUG)
		osc_startup(logger=logger)
	else :
		osc_startup()

	################################################
	# UDP_server lines
	line = next(readCSV)
	if line[0] != "UDP_server":
		print( "Expected tag \"UDP_server\" not found in line: ", line)
		sys.exit(2)

	nb_UDP_servers = to_num(line[1])
	
	line = next(readCSV) 
	# print( "line1 ", line )

	for ind_UDP_server in range(nb_UDP_servers):
		UDP_server_line = next(readCSV) 
		# print( "line2 ", UDP_server_line )
		UDP_server_ID = UDP_server_line[0]
		UDP_server_port = int(to_num(UDP_server_line[1]))
		if(not(UDP_server_ID in UDP_servers.keys())):
			UDP_servers[UDP_server_ID] = UDP_server(UDP_server_ID, UDP_server_port)
		else:
			print("Duplicate UDP_server ID: ", UDP_server_ID)
			sys.exit(2)

	# /UDP_server line
	line = next(readCSV)
	if line[0] != "/UDP_server":
		print( "Expected tag \"/UDP_server\" not found in line: ", line)
		sys.exit(2)

	################################################
	# UDP_client lines
	line = next(readCSV)
	if line[0] != "UDP_client":
		print( "Expected tag \"UDP_client\" not found in line: ", line)
		sys.exit(2)
	
	nb_UDP_clients = to_num(line[1])

	line = next(readCSV) 
	# print( "line1 ", line )

	for ind_UDP_client in range(nb_UDP_clients):
		UDP_client_line = next(readCSV) 
		# print( "line2 ", UDP_client_line )
		UDP_client_ID = UDP_client_line[0]
		UDP_client_IP_address = UDP_client_line[1]
		UDP_client_port = int(to_num(UDP_client_line[2]))
		if(not(UDP_client_ID in UDP_clients.keys())):
			UDP_clients[UDP_client_ID] = UDP_client(UDP_client_ID, UDP_client_IP_address, UDP_client_port)
		else:
			print("Duplicate UDP_client ID: ", UDP_client_ID)
			sys.exit(2)

	# /UDP_client line
	line = next(readCSV)
	if line[0] != "/UDP_client":
		print( "Expected tag \"/UDP_client\" not found in line: ", line)
		sys.exit(2)



