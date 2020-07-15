# usage: python stream_acc.py [mac1] [mac2] ... [mac(n)]
from __future__ import print_function
from mbientlab.metawear import MetaWear, libmetawear, parse_value
from mbientlab.metawear.cbindings import *
from time import sleep
from threading import Event

from signal import signal, SIGINT
from sys import exit

import platform
import sys

#OSC
from pythonosc import udp_client

if sys.version_info[0] == 2:
    range = xrange

class State:
    def __init__(self, device):
        self.device = device
        self.samples = 0
        self.callback = FnVoid_VoidP_DataP(self.data_handler)

    def data_handler(self, ctx, data):
        locData = parse_value(data)
        print("%s => %.4f %.4f %.4f " % (self.device.address, locData.x, locData.y, locData.z))
        # if self.samples % 100 == 0:
        #     print ('.', end = '')
        client.send_message("/mw_acc", [locData.x, locData.y, locData.z])
        self.samples += 1

def handler(signal_received, frame):
    # Handle any cleanup here
    print('SIGINT or CTRL-C detected. Exiting gracefully')
    disconnect()
    exit(0)

def disconnect():
	for s in states:
	    libmetawear.mbl_mw_acc_stop(s.device.board)
	    libmetawear.mbl_mw_acc_disable_acceleration_sampling(s.device.board)

	    signal = libmetawear.mbl_mw_acc_get_acceleration_data_signal(s.device.board)
	    libmetawear.mbl_mw_datasignal_unsubscribe(signal)
	    libmetawear.mbl_mw_debug_disconnect(s.device.board)

	print("Total Samples Received")
	for s in states:
	    print("%s -> %d" % (s.device.address, s.samples))

if __name__ == '__main__':
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)

	client = udp_client.SimpleUDPClient('127.0.0.1', 9000)

	states = []
	MAC_addresses = ['E9:0F:0A:C5:4D:66', 'C7:7D:6A:41:B4:15']
	for i in range(len(MAC_addresses)):
	    d = MetaWear(MAC_addresses[i])
	    d.connect()
	    print("Connected to " + d.address)
	    states.append(State(d))

	for s in states:
	    print("Configuring device ", s.device.address)
	    libmetawear.mbl_mw_settings_set_connection_parameters(s.device.board, 7.5, 7.5, 0, 6000)
	    sleep(1.5)

	    libmetawear.mbl_mw_acc_set_odr(s.device.board, 100.0)
	    libmetawear.mbl_mw_acc_set_range(s.device.board, 16.0)
	    libmetawear.mbl_mw_acc_write_acceleration_config(s.device.board)

	    signal = libmetawear.mbl_mw_acc_get_acceleration_data_signal(s.device.board)
	    libmetawear.mbl_mw_datasignal_subscribe(signal, None, s.callback)

	    libmetawear.mbl_mw_acc_enable_acceleration_sampling(s.device.board)
	    libmetawear.mbl_mw_acc_start(s.device.board)

	sleep(30000.0)

	disconnect()