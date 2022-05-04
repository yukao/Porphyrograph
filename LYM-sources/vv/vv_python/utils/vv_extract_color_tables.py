#!/usr/bin/python3 

# vv_extract_color_tables.py
# extract color tables from svg files 

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
	dir_of_svg_subdirs = ''
	csv_out_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputdirectories=","outputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputdirectories"):
			dir_of_svg_subdirs = arg
		elif opt in ("-o", "--outputfile"):
			csv_out_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', dir_of_svg_subdirs)

	try:
		FILEout = open(csv_out_file_name, "wt")
	except IOError:
		print("File not opened :", csv_out_file_name, " or path is incorrect")

	##################################################################
	# READS THE SUBFOLDERS, EXTRACT THE MIDFILE, AND GET THE COLOR TABLE
	##################################################################
	writeCVS = csv.writer(FILEout, delimiter=',')
	
	print("extracting first svg files from subfolders of ", dir_of_svg_subdirs)
	subfolders = [ [f.path, f.name] for f in os.scandir(dir_of_svg_subdirs) if f.is_dir() ]
	for subfolder_path, subfolder_name in subfolders:
		# the images from SVG files
		countFirstImage = "%04d" % int(1)

		image_name = subfolder_path+"/"+subfolder_name+"_"+countFirstImage+".svg"
		print("Build color map from image number ", countFirstImage, " ", image_name)

		# extraction of the colors from the SVG image
		try:
			FILEin = open(image_name, "rt")
		except IOError:
			print("File not found :", image_name, " or path is incorrect")
		line = FILEin.readline()
		line = line.rstrip()
		color_list = []
		while(line):
			search_result = re.search(r'<g .*fill\:(\#[0-9A-Fa-f]{6})\;', line)
			if(search_result != None) :
				color_list.append(search_result.group(1))
			line = FILEin.readline()
			line = line.rstrip()

		# builds the line: nb layer and list of colors
		line_to_output = []
		line_to_output.append(subfolder_name)
		line_to_output.append(str(len(color_list)))
		line_to_output = line_to_output + color_list

		# writes the line to output
		writeCVS.writerow(line_to_output)
	return 1

	FILEout.close()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
