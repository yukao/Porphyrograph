#!/usr/bin/python3 

# test.py
# osc input/output + TouchOSC interfacing
# (c) Yukao Nagemi

from math import sin
from math import cos
from math import pi
from math import floor

import getopt
import sys

# control C interruption
from signal import signal, SIGINT

from time import sleep


##################################################################
# USAGE
##################################################################
USAGE = '''test.py
'''

##################################################################
# INTERRUPTION HANDLER
##################################################################
def handlerForInterruption(signal_received, frame):
	# Handle breaks
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	exit(0)

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	##################################################################
	# Tell Python to run the handlerForInterruption() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)

	frequency = float(main_args[0])
	print("value:", frequency);
	frame_lapse = 1
	if(frequency != 0) :
		frame_lapse = int(1 / frequency)


	print("value:", frequency, "frame_lapse:", frame_lapse);


if __name__ == "__main__":
	import sys

	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handlerForInterruption)
	main(sys.argv[1:])
