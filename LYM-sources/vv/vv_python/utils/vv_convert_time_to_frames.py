#!/usr/bin/python3 

# vv_convert_time_to_frames.py
# transforming csv columns into frame numbers

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import random

import getopt
import sys
from signal import signal, SIGINT
import re

from vv_lib import to_num

import csv

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_production_weaving-2.py -i scenario_file
	ARGUMENTS:
	-i: input scenariofile
'''

IN_FPS = 50
OUT_FPS = 30

##################################################################
# MAIN
##################################################################
def main(main_args):
	##################################################################
	# ARGUMENTS
	##################################################################
	csv_in_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			csv_in_file_name = arg
		elif opt in ("-o", "--outputfile"):
			csv_out_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', csv_in_file_name)

	try:
		FILEin = open(csv_in_file_name, "rt")
	except IOError:
		print("File not found :", csv_in_file_name, " or path is incorrect")

	try:
		FILEout = open(csv_out_file_name, "wt")
	except IOError:
		print("File not opened :", csv_out_file_name, " or path is incorrect")

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCVS = csv.reader(FILEin, delimiter=',')
	writeCVS = csv.writer(FILEout, delimiter=',')
	
	line =  next(readCVS) 
	writeCVS.writerow(line)

	line =  next(readCVS) 
	writeCVS.writerow(line)

	#next lines: lines to process
	for line_to_process in readCVS:
		line_to_output = []
		next_column = None
		# processes the column of the line
		for column in line_to_process:
			# builds the output line
			if(next_column != None):
				# outputs the line
				line_to_output.append(next_column)
				next_column = None
			else:
				# scans the input line
				if(re.search(r'\:', column) != None) :
					video_time = re.split(r':' , column)
					if(len(video_time) != 4) :
						print("4-components video time expected ",len(video_time)," ",video_time)
					real_time_in_sec = to_num(video_time[0]) * 3600 + to_num(video_time[1]) * 60 \
						+ to_num(video_time[2]) + (to_num(video_time[3]) / float(IN_FPS))
					real_time_out_fps = int(real_time_in_sec * float(OUT_FPS))
					next_column = str(real_time_out_fps)
				else:
					next_column = None
				# outputs the line
				line_to_output.append(column)
		# writes the line to output
		writeCVS.writerow(line_to_output)
	return 1

	FILEin.close()
	FILEout.close()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
