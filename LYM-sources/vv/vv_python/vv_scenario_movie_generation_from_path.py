#!/usr/bin/python3 

# vv_scenario_movie_generation_from_path.py
# scenario for scalings of vector video frames (both initial scaling before crossfading and final scalings after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate
from vv_lib import length
from vv_lib import current_sound_volume
from vv_lib import current_onset_volume
from vv_lib import smoothed_current_sound_volume
from vv_lib import read_sound_data
from vv_lib import read_write_sound_data
from vv_lib import read_onset_data

from vv_lib import normalize_file_paths
from vv_lib import absolute_coordinates_file_paths
from vv_lib import path_curves_and_lengths

import vv_transform

import math

import bezier

import csv
import re
import getopt
import sys
from signal import signal, SIGINT

import numpy as np
import cv2 as cv

from noise import pnoise1

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_movie_generation_from_path.py -i scenario_file
ARGUMENTS:
	 -i: input scenariofile
'''


##################################################################
# GLOBALS
##################################################################
view_box_h = 0
view_box_w = 0
view_box_x = 0
view_box_y = 0
absoluteCoords_SVG_curves_drawing = []
absoluteCoords_target_SVG_curves_drawing = []

# SCENARIO VARIABLES
var_names = []
var_types = dict()
val_init = dict()
val_fin = dict()
val_interp = dict()
interp = dict()

# interpolated variables
scaling_factor = 1
rotation = 0
translation_x = 0
translation_y = 0
percent_transf1 = 0

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
soundtrack_working_channels = 1

##################################################################
# ONLY KEEP DRAWING PATHS
##################################################################
def keep_only_drawing_lines(input_lines, list_of_drawing_paths_IDs) :
	output_lines = []
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
				print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
				print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
				print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) in list_of_drawing_paths_IDs :
					output_lines.append( line_string )
		elif(re.search(r'<text', line_string) == None) :
			output_lines.append( line_string )
	return output_lines

def keep_only_drawing_lines_and_camera_path(input_lines, list_of_drawing_paths_IDs, camera_path_ID) :
	output_lines = []
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
				print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
				print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
				print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) in list_of_drawing_paths_IDs or result_ID.group(1) == camera_path_ID :
					output_lines.append( line_string )
		elif(re.search(r'<text', line_string) == None) :
			output_lines.append( line_string )
	return output_lines

##################################################################
# RETURNS SELECTED PATH
##################################################################
def load_camera_path_from_lines(input_lines, camera_path_ID) :
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
				print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
				print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
				print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) == camera_path_ID :
					return line_string
	return ''

##################################################################
# RETURNS view box
##################################################################
#  viewBox="0 0 1016 571.50002"
def load_view_box_from_lines(input_lines) :
	for line_string in input_lines :
		result_viewbox = re.search(r'\sviewBox="([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\"', line_string)
		if(result_viewbox != None):
			return [force_num(result_viewbox.group(1)), force_num(result_viewbox.group(2)), force_num(result_viewbox.group(3)), force_num(result_viewbox.group(4))]
	return [0, 0, 0, 0]

##################################################################
# GENERATES ALL THE FRAMES IN THE MOVIE FOR A GIVEN TRANSFORMATION
##################################################################
def interpolate_paths(local_line_string, local_line_target_string, local_percent_transf1) :
	# only keeps the ID and the path data
	result_path = re.search(r'\sd="([^\"]*)\"', local_line_string)
	# only keeps the ID and the path data
	result_path_target = re.search(r'\sd="([^\"]*)\"', local_line_target_string)
	if not result_path:
		print('Expected path not found')
	result_ID = re.search(r'\sid="([^\"]*)\"', local_line_string)
	if not result_ID:
		print('Expected ID not found')
	result_style = re.search(r'\sstyle="([^\"]*)\"', local_line_string)
	if not result_style:
		print('Expected style not found')
	# print('result_ID', result_ID.group(1))
	result_object = re.search(r'objects', result_ID.group(1))
	# interpolation only occurs for paths with 'objects' inside their ID
	if not result_object :
		return local_line_string
	if result_ID and result_path and result_path_target and result_style :
		local_path_data = re.split(r' +' , result_path.group(1).rstrip())
		local_path_data_size = len(local_path_data)

		local_path_target_data = re.split(r' +' , result_path_target.group(1).rstrip())
		local_path_target_data_size = len(local_path_target_data)

		if local_path_data_size != local_path_target_data_size :
			print('The two paths do not have the same size', local_path_data_size, local_path_target_data_size)
			# print(local_path_data[0:30])
			# print(local_path_target_data[0:30])
			# sys.exit(2)

		# scales the path centrally =
		index = 0
		index_target = 0
		while(index < local_path_data_size) :
			if(local_path_data[index] == "M" and index + 3 < local_path_data_size and local_path_data[index + 3] == "M" \
				and index_target + 3 < local_path_target_data_size and local_path_target_data[index_target + 3] != "M") :
				index += 3

			elif(local_path_data[index] == "M" or local_path_data[index] == "L") :
				if(local_path_target_data[index_target] != "M" and local_path_target_data[index_target] != "L") :
					print('M or L expected in target path', index_target, local_path_target_data[index_target])
					print(local_path_data[index-6:index + 30])
					print(local_path_target_data[index_target-6:index_target + 30])
					sys.exit(2)
				index += 1
				index_target += 1
				# processes the next moveto and lineto
				while(True) :
					local_path_data[index] = (1 - local_percent_transf1) * force_num(local_path_data[index]) + local_percent_transf1 * force_num(local_path_target_data[index_target])
					local_path_data[index + 1] = (1 - local_percent_transf1) * force_num(local_path_data[index + 1]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 1])
					index += 2
					index_target += 2
				
					if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
						break

			elif(local_path_data[index] == "C") :
				if(local_path_target_data[index_target] != "C") :
					print('C expected in target path', index_target, local_path_target_data[index_target])
					print(local_path_data[index-6:index + 30])
					print(local_path_target_data[index_target-6:index_target + 30])
					sys.exit(2)
				index += 1
				index_target += 1
				# processes the next curve segments
				# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
				while(True) :
					local_path_data[index] = (1 - local_percent_transf1) * force_num(local_path_data[index]) + local_percent_transf1 * force_num(local_path_target_data[index_target])
					local_path_data[index + 1] = (1 - local_percent_transf1) * force_num(local_path_data[index + 1]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 1])
					local_path_data[index + 2] = (1 - local_percent_transf1) * force_num(local_path_data[index + 2]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 2])
					local_path_data[index + 3] = (1 - local_percent_transf1) * force_num(local_path_data[index + 3]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 3])
					local_path_data[index + 4] = (1 - local_percent_transf1) * force_num(local_path_data[index + 4]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 4])
					local_path_data[index + 5] = (1 - local_percent_transf1) * force_num(local_path_data[index + 5]) + local_percent_transf1 * force_num(local_path_target_data[index_target + 5])
					index += 6
					index_target += 6
				
					if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
						break
			
			elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
				if(local_path_target_data[index_target] != "Z" and local_path_target_data[index_target] != "z") :
					print('Z or z expected in target path', index_target, local_path_target_data[index_target])
					print(local_path_data[index-6:index + 30])
					print(local_path_target_data[index_target-6:index_target + 30])
					sys.exit(2)
				index += 1
				index_target += 1
			
			else :
				print( "Line: ", local_path_data_string)
				print( "interpolate_paths Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
				return ""

		separator = ' '
		return '<path id="{0:s}" style="{1:s}" d="{2:s}" />'.format(result_ID.group(1), result_style.group(1), separator.join(map(str, local_path_data)))


def generate_movie_frames(local_full_svg_output_file_name, local_output_frame_no, local_camera_path_ID, local_scaling, local_rotation, local_translation_x, local_translation_y, local_ind_scene) :
	global view_box_h
	global view_box_w
	global view_box_x
	global view_box_y
	global absoluteCoords_SVG_curves_drawing
	global absoluteCoords_target_SVG_curves_drawing
	global percent_transf1

	# if (local_output_frame_no != 12332) :
	# if not((local_ind_scene == 9 and local_output_frame_no % 10 == 0) or (local_ind_scene == 10 and local_output_frame_no % 40 == 0) or (local_ind_scene == 11 and local_output_frame_no % 40 == 0))  :
	#  	return
	if local_output_frame_no <= 2680 or local_output_frame_no >= 2702 :
	 	return

	group_line = '<g transform="translate({0:.7f} {1:.7f}) scale({2:.7f} {2:.7f}) rotate({3:.7f}) translate({4:.7f} {5:.7f})" style="display:inline" id="layer1">'.format((-view_box_x + view_box_w/2.), (-view_box_y + view_box_h/2.), local_scaling, local_rotation, local_translation_x, local_translation_y)

	##################################################################
	# GENERATES ALL THE FRAMES IN THE MOVIE
	countOutput = "%05d" % local_output_frame_no
	local_output_file_name = local_full_svg_output_file_name+"_"+countOutput+".svg"

	print("path {0:s} frame {1:s} scaling {2:.3f}".format(local_camera_path_ID, local_output_file_name, local_scaling))

	##################################################################
	# NORMALIZED VECTOR IMAGE FILE OPENING
	try:
		FILEout = open(local_output_file_name, "wt")
	except IOError:
		print("File not created :", local_output_file_name, " or path is incorrect")

	group_line_written = False
	if absoluteCoords_target_SVG_curves_drawing == [] :
		for line_string in absoluteCoords_SVG_curves_drawing :
			if(re.search(r'<g', line_string) != None) :
				# writes the transformation
				FILEout.write(group_line+"\n")
				group_line_written = True
			elif(re.search(r'</svg', line_string) != None) :
				# writes the transformation
				FILEout.write('</g>'+"\n")
				FILEout.write(str(line_string)+"\n")
			else :
				# write the other lines
				FILEout.write(str(line_string)+"\n")
	else :
		for (line_string, line_target_string) in zip(absoluteCoords_SVG_curves_drawing, absoluteCoords_target_SVG_curves_drawing) :
			if(re.search(r'<g', line_string) != None) :
				# writes the transformation
				FILEout.write(group_line+"\n")
				group_line_written = True
			elif(re.search(r'</svg', line_string) != None) :
				# writes the transformation
				FILEout.write('</g>'+"\n")
				FILEout.write(str(line_string)+"\n")
			elif(re.search(r'<path', line_string) != None) :
				# interpolates the object lines according to transformation value before printing
				line_to_write = interpolate_paths(line_string, line_target_string, percent_transf1)
				# write the line
				FILEout.write(str(line_to_write)+"\n")
			else :
				# write the other lines
				FILEout.write(str(line_string)+"\n")
	FILEout.close()

	if not group_line_written :
		print("expected group tag not found")

##################################################################
# SCENARIO VARIABLE INTERPOLATION ACCORDING TO SCENE PERCENTAGE
##################################################################
def interpolate_variables(local_scene_percentage, local_output_frame_no) :
	# SCENARIO VARIABLES
	global var_names
	global var_types
	global val_init
	global val_fin
	global val_interp
	global interp

	global scaling_factor
	global rotation
	global translation_x
	global translation_y
	global percent_transf1

	global sound_volume
	global fps
	global with_sound_data
	global with_onset_data
	global pulse_coef
	global smoothed_pulse_coef
	global onset_pulse_coef
	global onset_decay_time
	global soundtrack_working_channels

	for var_name in var_names :
		val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], local_scene_percentage)
		# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])
	
	sound_volume = 0
	if with_sound_data :
		if "pulse_coef" in val_interp :
			pulse_coef = val_interp["pulse_coef"]
		else :
			pulse_coef = 0.0
		if "smoothed_pulse_coef" in val_interp :
			smoothed_pulse_coef = val_interp["smoothed_pulse_coef"]
		else :
			smoothed_pulse_coef = 0.0
		if "fps" in val_interp :
			fps = val_interp["fps"]
		else :
			fps = 30.0
		# print("ind frame output", local_output_frame_no)	
		sound_volume += current_sound_volume(local_output_frame_no, fps, pulse_coef, soundtrack_working_channels) + smoothed_current_sound_volume(local_output_frame_no, fps, smoothed_pulse_coef, soundtrack_working_channels)
	if with_onset_data :
		if "onset_decay_time" in val_interp :
			onset_decay_time = val_interp["onset_decay_time"]
		else :
			onset_decay_time = 0.1
		if "onset_pulse_coef" in val_interp :
			onset_pulse_coef = val_interp["onset_pulse_coef"]
		else :
			onset_pulse_coef = 0.0
		if "fps" in val_interp :
			fps = val_interp["fps"]
		else :
			fps = 30.0
		# print("ind frame output", local_output_frame_no)	
		(onset_sound_volume, new_onset) = current_onset_volume(local_output_frame_no, fps, onset_pulse_coef, onset_decay_time, soundtrack_working_channels)
		sound_volume += onset_sound_volume

	# scaling ID
	if("scaling_factor" in val_interp) :
		# percentage of scaling
		scaling_factor = val_interp["scaling_factor"]
		if(with_sound_data and "pulsed_scaling_factor" in val_interp) :
			interp_pulsed_scaling_factor = val_interp["pulsed_scaling_factor"]
			scaling_factor = interpolate(var_types["scaling_factor"], scaling_factor, interp_pulsed_scaling_factor, 'l', sound_volume)
	# absolute rotation if no path or relative rotation when following a path
	if("rotation" in val_interp) :
		rotation = val_interp["rotation"]
		if(with_sound_data and "pulsed_rotation" in val_interp) :
			interp_pulsed_rotation = val_interp["pulsed_rotation"]
			rotation = interpolate(var_types["rotation"], rotation, interp_pulsed_rotation, 'l', sound_volume)
	# translation if not given by path (NULL path)
	if("translation_x" in val_interp) :
		translation_x = val_interp["translation_x"]
		if(with_sound_data and "pulsed_translation_x" in val_interp) :
			interp_pulsed_translation_x = val_interp["pulsed_translation_x"]
			translation_x = interpolate(var_types["translation_x"], translation_x, interp_pulsed_translation_x, 'l', sound_volume)
	if("translation_y" in val_interp) :
		translation_y = val_interp["translation_y"]
		if(with_sound_data and "pulsed_translation_y" in val_interp) :
			interp_pulsed_translation_y = val_interp["pulsed_translation_y"]
			translation_y = interpolate(var_types["translation_y"], translation_y, interp_pulsed_translation_y, 'l', sound_volume)
	# percent transformation
	if("percent_transf1" in val_interp) :
		# percentage of scaling
		percent_transf1 = val_interp["percent_transf1"]
		if(with_sound_data and "pulsed_percent_transf1" in val_interp) :
			interp_pulsed_percent_transf1 = val_interp["pulsed_percent_transf1"]
			percent_transf1 = interpolate(var_types["percent_transf1"], percent_transf1, interp_pulsed_percent_transf1, 'l', sound_volume)

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	##################################################################
	# GLOBALS
	##################################################################
	global view_box_h
	global view_box_w
	global view_box_x
	global view_box_y
	global absoluteCoords_SVG_curves_drawing
	global absoluteCoords_target_SVG_curves_drawing

	# SCENARIO VARIABLES
	global var_names
	global var_types
	global val_init
	global val_fin
	global val_interp
	global interp

	# interpolated scenario variables
	global scaling_factor
	global rotation
	global translation_x
	global translation_y
	global percent_transf1

	global sound_volume
	global fps
	global with_sound_data
	global pulse_coef
	global smoothed_pulse_coef

	trace_only = False

	scenario_chapter_no = 0
	with_fill_and_stroke = True

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', scenario_file_name)

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
			print("File not found (scenario_file_name):", scenario_file_name, " or path is incorrect")
	
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
		# print("scene", ind_scene)
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
		
		if not(ind_scene == 0)  :
		# if not(ind_scene == 9 or ind_scene == 10 or ind_scene == 11)  :
			continue

		# applies possible scalings on the SVG files
		# creates the directories in which the paths files are saved
		if( not os.path.isdir(val_init["svg_output_directory"])  ):
			try:
					os.mkdir(val_init["svg_output_directory"])
			except OSError:
					print ("Creation of the directory %s failed" % val_init["svg_output_directory"])
			else:
					print ("Successfully created the directory %s " % val_init["svg_output_directory"])
		# else:
		# 	print ("Directory %s already exists" % val_init["svg_output_directory"])

		soundtrack_working_channels = 1
		if "soundtrack_working_channels" in val_init :
			soundtrack_working_channels = int(to_num(val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)

		# opening the sound file, in CSV mode, and retrieving the first value
		# only the first time to store the values in the csv files
		# to be called if either sound data file or hilbert or smoothing windows change
		if False :
			print("read write sound data")
			(fps, pulse_coef, smoothed_pulse_coef, with_sound_data) = read_write_sound_data(val_init)
		with_sound_data = read_sound_data(val_init)
		with_onset_data = read_onset_data(val_init)

		##################################################################
		# GENERATES ALL THE FRAMES IN THE MOVIE
		##################################################################
		indFrameOutput_init = val_init["imageOutputIndex"]
		indFrameOutput_fin = val_fin["imageOutputIndex"]
		number_frames_input = indFrameOutput_fin - indFrameOutput_init + 1
		tmp_dir = val_init["tmp_directory"]
		drawing_paths_IDs = val_init["drawing_paths_IDs"]
		drawing_paths_IDs =  re.split(r'/', drawing_paths_IDs.rstrip())
		camera_path_ID = val_init["camera_path_ID"]

		# SVG input file name
		full_svg_input_drawing_file_name = os.path.join(val_init["svg_input_directory"], val_init["svg_input_file_name"])
		if "svg_input_file_target_name" in val_init and val_init["svg_input_file_target_name"] != "NULL" :
			full_svg_input_drawing_file_target_name = os.path.join(val_init["svg_input_directory"], val_init["svg_input_file_target_name"])
		else :
			full_svg_input_drawing_file_target_name = "NULL"
		# SVG output file name
		full_svg_output_file_name = os.path.join(val_init["svg_output_directory"], val_init["svg_output_file_name"])

		##################################################################
		# VECTOR IMAGE FILE OPENING
		try:
			FILEin = open(full_svg_input_drawing_file_name+".svg", "rt")
		except IOError:
			print("File not found (full_svg_input_drawing_file_name):", full_svg_input_drawing_file_name, " or path is incorrect")
		FILEin_target = None
		if full_svg_input_drawing_file_target_name != "NULL" :
			try:
				FILEin_target = open(full_svg_input_drawing_file_target_name+".svg", "rt")
			except IOError:
				print("File not found (full_svg_input_drawing_file_target_name):", full_svg_input_drawing_file_target_name, " or path is incorrect")

		# STORES THE LINES IN AN ARRAY
		contents = FILEin.read()
		Unormalized_SVG_file_lines = contents.splitlines()
		# VECTOR IMAGE FILE CLOSING
		FILEin.close()
		if FILEin_target != None :
			# STORES THE LINES IN AN ARRAY
			contents_target = FILEin_target.read()
			Unormalized_SVG_file_target_lines = contents_target.splitlines()
			# VECTOR IMAGE FILE CLOSING
			FILEin_target.close()

		print("Image and paths loading...")

		# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
		SVG_file_lines = normalize_file_paths(Unormalized_SVG_file_lines)
		if FILEin_target != None :
			SVG_file_target_lines = normalize_file_paths(Unormalized_SVG_file_target_lines)

		# MAKES ABSOLUTE COORDINATES
		absoluteCoords_SVG_curves = absolute_coordinates_file_paths(SVG_file_lines)
		if FILEin_target != None :
			absoluteCoords_target_SVG_curves = absolute_coordinates_file_paths(SVG_file_target_lines)

		# ONLY KEEPS DRAWING
		print("Only keep paths", drawing_paths_IDs)
		# absoluteCoords_SVG_curves_drawing = keep_only_drawing_lines_and_camera_path(absoluteCoords_SVG_curves, drawing_paths_IDs, camera_path_ID)
		absoluteCoords_SVG_curves_drawing = keep_only_drawing_lines(absoluteCoords_SVG_curves, drawing_paths_IDs)
		if FILEin_target != None :
			absoluteCoords_target_SVG_curves_drawing = keep_only_drawing_lines(absoluteCoords_target_SVG_curves, drawing_paths_IDs)

		# loads a camera path
		print('camera_path_ID', camera_path_ID)
		if camera_path_ID != "NULL" :
			camera_path = load_camera_path_from_lines(absoluteCoords_SVG_curves, camera_path_ID)
			print("camera path", camera_path)
			if( not camera_path ) :
				print("Camera path not found in the SVG file", camera_path_ID, full_svg_input_drawing_file_name+".svg")
				exit()
		else :
			camera_path = 'NULL'
			print("No camera path provided by the scenario file")

		# loads drawing dimensions
		(view_box_x, view_box_y, view_box_w, view_box_h) = load_view_box_from_lines(absoluteCoords_SVG_curves)

		# walks along the camera path
		if camera_path != 'NULL' :

			# calculates the piecewise length of the path, the length of each curve and the control points
			(camera_path_length, camera_curve_lengths, camera_curve_nodes) = path_curves_and_lengths(camera_path)
			# distributes the frames on the curves according to their lenghts
			camera_curve_frames = list(map(lambda x: int(round((x/camera_path_length) * number_frames_input)), camera_curve_lengths))
			# corrects the distribution on first and last curve so that the summ of the frames correspond to the required frame number
			delta_frames = number_frames_input - sum(camera_curve_frames)
			half_delta_frames = int(delta_frames/2)
			camera_curve_frames[0] += half_delta_frames
			camera_curve_frames[-1] += delta_frames - half_delta_frames
			# prints the results
			print("path length", camera_path_length)
			print("path curve lengths", ["%.2f" % e for e in camera_curve_lengths])
			print("nb tot frame required, provided", number_frames_input, sum(camera_curve_frames))
			print("path curve nb frames", camera_curve_frames)
			# print("path curves\n", "\n".join(map(str,camera_curve_nodes)))
			# print("camera path", camera_path)

			# camera trajectory along a path with centering and scaling around the current point
			# and orientation around the tangent
			ind_frames_cumul = 0
			# walks all the paths of the current drawing
			# for each curve, has a number of frames and the nodes (control points) of the Bezier curve
			for nodes, frames in zip(camera_curve_nodes, camera_curve_frames) :
				# list of curve point coordinates (as many as frames)
				curve_points = []
				# list of curve trangent coordinates (as many as frames)
				curve_tangents = []
				# stores starting frame no
				ind_frames_init = ind_frames_cumul
				# builds the curve that will be used by the bezier lib
				curve = bezier.Curve(nodes, degree=3)
				# calculates points and tangents
				for frame in range(frames) :
					t = float(frame) / frames
					point_array = curve.evaluate(t)
					tangent_array = curve.evaluate_hodograph(t)
					curve_points.append(point_array)
					curve_tangents.append(tangent_array)
					ind_frames_cumul += 1
				# if ind_frames_cumul < 100 :
				# 	print("PLOT frames", ind_frames_init, "to", ind_frames_cumul - 1, ":", curve_points)

				# stores starting frame no
				ind_frames_cumul = ind_frames_init
				# uses points and tangents to change translation and orientation so that
				# the current curve point is in the center of the screen and the drawing is oriented rightwards towards the tangent
				for point, tangent in zip( curve_points, curve_tangents) :
					if(trace_only) :
						print("Ind image %d Scene percentage %.2f percent_scaling_factor1 %.2f\n" % indFrameInput, scene_percentage, val_interp["percent_scaling_factor1"])

					# variable interpolation
					scene_percentage = float(ind_frames_cumul) /(indFrameOutput_fin - indFrameOutput_init)
					#clamping
					scene_percentage = clamp(scene_percentage)
					interpolate_variables(scene_percentage, (ind_frames_cumul + indFrameOutput_init))

					# rotation/scale around the current point (preceded and followed by translation to this pont)
					# print("point {} {} tangent {} {} ".format(point[0,0], point[1,0], tangent[0,0], tangent[1,0]))
					# 1: tangent normalization 
					tangent = tangent / np.sqrt(np.sum(tangent**2))
					angle = np.arccos(tangent[0,0])
					if(tangent[1,0] < 0) :
						angle = -angle
					angle = -np.degrees(angle)
					angle += 90

					# 2. orientation with respect to the tangent (the basic orientation is traveling from to bottom when aligned with the tangent)
					angle += rotation

					# 3. position on the curve modified by the translation with respect to the normal
					# if both x and y vary with sound, the direction of the variation is a continous 2D Perlin noise 
					ind_frame = ind_frames_cumul + indFrameOutput_init
					ind_frame_Perlin_x = ind_frame % 10000
					ind_frame_Perlin_y = (16201 - ind_frame) % 10000
					position_x = -point[0,0] + translation_x *  pnoise1(ind_frame_Perlin_x/float(1000), octaves=8, persistence=1.0, lacunarity=1.8, repeat = 10000, base=0)
					position_y = -point[1,0] + translation_y *  pnoise1(ind_frame_Perlin_y/float(1000), octaves=8, persistence=1.0, lacunarity=2.2, repeat = 10000, base=0)

					# if (ind_frames_cumul + indFrameOutput_init) % 100 != 0 :
					# 	ind_frames_cumul += 1
					# 	continue
					
					generate_movie_frames(full_svg_output_file_name, (ind_frames_cumul + indFrameOutput_init), camera_path_ID, scaling_factor, angle, position_x, position_y, ind_scene)

					ind_frames_cumul += 1

					# stops at the end of the required frames
					if ind_frames_cumul > (indFrameOutput_fin - indFrameOutput_init) :
						break
				# # only first bezier curve
				# break

		elif camera_path == 'NULL' :
			for frame_no in range(number_frames_input) :
				scene_percentage = float(frame_no) / (number_frames_input - 1)
				#clamping
				scene_percentage = clamp(scene_percentage)
				interpolate_variables(scene_percentage, (frame_no + indFrameOutput_init))

				if(trace_only) :
					print("Ind image %d Scene percentage %.2f percent_scaling_factor1 %.2f fill_opacity_l1 %.2f fill_color_l4 %s fill_color_bg %s\n" % \
						indFrameInput, scene_percentage, val_interp["percent_scaling_factor1"], val_interp["fill_opacity_l1"], val_interp["fill_color_l4"], val_interp["fill_color_bg"])

				generate_movie_frames(full_svg_output_file_name, (frame_no + indFrameOutput_init), camera_path_ID, scaling_factor, rotation, translation_x, translation_y, ind_scene)

		else :
			print("camera path", camera_path_ID, "not found and transformation coordinates not provided (translation and rotation)", translation_x, translation_y, rotation)

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
