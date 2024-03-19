#!/usr/bin/python3 

# vv_convert_time_to_frames.py
# transforming svg columns into frame numbers

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import random

import getopt
import sys
from signal import signal, SIGINT
import re

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_one_SVG_tag_per_line -i <input> -o <output>
	ARGUMENTS:
	-i: input scenariofile
'''


##################################################################
# MAIN
##################################################################
def main(main_args):
	##################################################################
	# ARGUMENTS
	##################################################################
	svg_in_file_name = ''
	svg_out_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			svg_in_file_name = arg
		elif opt in ("-o", "--outputfile"):
			svg_out_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', svg_in_file_name)

	try:
		FILEin = open(svg_in_file_name, "rt")
	except IOError:
		print("File not found :", svg_in_file_name, " or path is incorrect")

	try:
		FILEout = open(svg_out_file_name, "wt")
	except IOError:
		print("File not opened :", svg_out_file_name, " or path is incorrect")

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################

	#next lines: lines to process
	Lines = FILEin.readlines()
	for line_to_process in Lines:
		# scans the input line
		line_to_output = line_to_process.replace('><','>\n<')
		# writes the line to output
		FILEout.write(line_to_output)
	return 1

	FILEin.close()
	FILEout.close()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
