#!/usr/bin/python3 

# vv_scenario_create_sound_data_files.py
# scenario for creating csv sound files (with and without smoothing) from wav files

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate
from vv_lib import read_wav_data
from vv_lib import read_write_sound_data
from vv_lib import plot_sound_data

import vv_transform

import math

import csv
import re
import getopt
import sys
from signal import signal, SIGINT

import numpy as np

# sound data
sound_volume = 0
fps = 30
pulse_coef = 0
smoothed_pulse_coef = 0
with_sound_data = False
onset_pulse_coef = 0
onset_decay_time = 0
with_onset_data = False
new_onset = False

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global sound_volume
	global fps
	global with_sound_data
	global pulse_coef
	global smoothed_pulse_coef

	# SCENARIO VARIABLES
	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()
	is_plot = False
	is_extrema = False

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile=","plot=","extrema="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		elif opt == "--plot":
			is_plot = arg
		elif opt == "--extrema":
			is_extrema = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', scenario_file_name)

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
			print("File not found :", scenario_file_name, " or path is incorrect")
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 
	line = next(readCSV) 
	# print "line2 ", line 

	#3rd line: variable types
	line_types =  next(readCSV)

	#4th line: variable names
	var_names =  next(readCSV) 
	#removes the first one
	var_names.pop(0)

	#finishes the types now that the variable strings are known
	#removes the first one
	line_types.pop(0)
	indVar = 0
	for line_type in line_types :
		var_types[var_names[indVar]] = line_type
		indVar += 1

	# scenario line
	values_line =  next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found in line:", values_line)
		return 0
	
	nb_scenes = to_num(values_line[1])
	# print( "nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		# scene line
		values_line =  next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =  next(readCSV)
		# var IDs line
		line =  next(readCSV)

		# variable initial value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_init in values_line:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp	
			indVar += 1

		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
		if not is_plot :
			# store the values in the csv files
			# to be called if either sound data file or hilbert envelope window (soundsample_window)
			# or smoothing window change (soundsmoothing_window)
			print("read wav file")
			nb_audio_samples = read_wav_data(val_init)
			print("wav file contains", nb_audio_samples, "samples")
			print("read write sound data")
			(fps, pulse_coef, smoothed_pulse_coef, with_sound_data) = read_write_sound_data(val_init)
		else :
			print("plot sound data")
			plot_sound_data(val_init)

	# /scenario line
	values_line = next(readCSV)
	if(values_line[0] != "/scenario") :
		print( "Expected tag \"/scenario\" not found!\n" )
		return 0

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
