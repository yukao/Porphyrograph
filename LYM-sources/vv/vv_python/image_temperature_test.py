#!/usr/bin/python3 

# launch command for the initial posterization of a vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import sys
sys.path.append('/mnt/d/sync.com/Sync/LYM-sources/vv/vv_python/')
import vv_scenario_transform
from signal import signal, SIGINT

from math import log

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def red_sup_6600(temp):
	return 1.29293618606274509804 * pow((temp/100.0 - 60.0), -0.1332047592)

def green_between_1000_and_6600(temp):
	return 0.39008157876901960784 * log(temp/100.0) - 0.63184144378862745098

def green_sup_6600(temp):
	return 1.12989086089529411765 * pow((temp/100.0 - 60.0), -0.0755148492)

def blue_between_2000_and_6600(temp):
	return 0.54320678911019607843 * log(temp/100.0 - 10.0) - 1.19625408914

def rgb_gain(temp):
	temp = min(40000,max(1000,temp))
	#red
	if(temp < 6600):
		r = 1
	else:
		r = red_sup_6600(temp)
	# green
	if(temp <= 6600):
		g = green_between_1000_and_6600(temp)
	elif(temp > 6600):
		g = green_sup_6600(temp)
	# blue
	if(temp < 2000):
		b = 0
	elif(temp >= 2000 and temp <= 6600):
		b = blue_between_2000_and_6600(temp)
	else:
		b = 1
	return [min(max(r,0),1),min(max(g,0),1),min(max(b,0),1)]

def main():
	for temp in range(1000, 40000, 100):
		r, g, b = rgb_gain(temp)
		print("t: %d (r,g,b) (%.3f,%.3f,%.3f)" % (temp, r, g, b))
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
