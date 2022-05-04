#!/usr/bin/python3 

# vv_scenario_transform.py
# scenario for transformations of vector video frames (both initial transformation before crossfading and final transformations after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import vv_fillAndStroke
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import interpolate
from vv_lib import current_sound_volume
from vv_lib import peaked_current_sound_volume
from vv_lib import current_onset_volume
from vv_lib import smoothed_current_sound_volume
from vv_lib import read_sound_data
from vv_lib import read_onset_data

import csv
import re
import getopt
import sys
from signal import signal, SIGINT
from scipy.signal import hilbert, chirp

import numpy as np

from math import sin, cos

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_fillAndStroke.py -i scenario_file
ARGUMENTS:
	 -i: input scenariofile
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	# SCENARIO VARIABLES
	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()

	trace_only = False

	scenario_chapter_no = 0

	fps = 30
	pulse_coef = 0
	smoothed_pulse_coef = 0
	with_sound_data = False
	onset_pulse_coef = 0
	onset_decay_time = 0
	with_onset_data = False
	new_onset = False

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:s:",["inputfile=", "chapter_no="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		elif opt in ("--chapter_no"):
			scenario_chapter_no = arg
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
		
		# if not((ind_scene == 9) or (ind_scene == 10) or (ind_scene == 11)) :
		if not(ind_scene == 0) :
			continue

		# printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", nb_scenes, scene_ID,
		#   val_init["svg_input_file_name"], val_init["imageInputIndex"], 
		#   val_fin:"imageInputIndex", val_init["imageOutputIndex"], 
		#   val_fin:"imageOutputIndex")

		if val_init["imageInputIndex"] < val_fin["imageInputIndex"]:
			pas = 1
		else :
			pas = -1

		# applies possible transformations on the SVG files
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

		# opening the sound file, in CSV mode, and retrieving the first value
		soundtrack_working_channels = 1
		if "soundtrack_working_channels" in val_init :
			soundtrack_working_channels = int(to_num(val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2 :
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		print("read sound data")
		with_sound_data = read_sound_data(val_init)
		with_onset_data = read_onset_data(val_init)

		##################################################################
		# SCANS ALL THE FRAMES IN THE MOVIE
		##################################################################
		indFrameOutput = val_init["imageOutputIndex"]
		nb_layers = val_init["nb_posterization_layers"]
		tmp_dir = val_init["tmp_directory"]
		for indFrameInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + pas, pas):
			countInput = "%05d" % indFrameInput
			countOutput = "%05d" % indFrameOutput

			# if (indFrameInput < 5135 or indFrameInput > 5136) :
			# if (indFrameInput != 14500 and indFrameInput != 14700) :
			# if (indFrameInput <= 14700 or indFrameInput > 14900) :
				# moves forward on output frames
			# if not((ind_scene == 9 and indFrameInput % 10 == 0) or (ind_scene == 10 and indFrameInput % 40 == 0) or (ind_scene == 11 and indFrameInput % 40 == 0))  :
			# 	indFrameOutput += 1
			# 	continue
			if indFrameInput <= 2680 or indFrameInput >= 2702 :
				indFrameOutput += 1
				continue

			# scene percentage at this frame 
			scene_percentage = 0
			if pas != 0 and (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) != 0 :
				scene_percentage = float(indFrameInput - val_init["imageInputIndex"])/(val_fin["imageInputIndex"] - val_init["imageInputIndex"])
			#clamping
			scene_percentage = clamp(scene_percentage)

			# interpolating all the variables
			for var_name in var_names :
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])
			
			# if(trace_only) :
			# 	print("Ind image {0:d} Scene percentage {0:.2f}".format(indFrameInput, scene_percentage))

			# SVG input file name
			full_svg_input_file_name = os.path.join(val_init["svg_input_directory"], val_init["svg_input_file_name"])
			full_svg_input_file_name = full_svg_input_file_name+"_"+countInput+".svg"
			# SVG output file name
			full_svg_output_file_name = os.path.join(val_init["svg_output_directory"], val_init["svg_output_file_name"])
			full_svg_output_file_name = full_svg_output_file_name+"_"+countOutput+".svg"

			
			# reads the sound volume corresponding to the soundtrack at the timecode of the current frame
			envelope_sound_volume = 0
			smoothed_envelope_sound_volume = 0
			onset_sound_volume = 0
			sound_volume = 0
			if with_sound_data :
				if "pulse_coef" in val_interp :
					pulse_coef = val_interp["pulse_coef"]
				else :
					pulse_coef = 0.0
				if "peaked_pulse_coef" in val_interp :
					peaked_pulse_coef = val_interp["peaked_pulse_coef"]
				else :
					peaked_pulse_coef = 0.0
				if "smoothed_pulse_coef" in val_interp :
					smoothed_pulse_coef = val_interp["smoothed_pulse_coef"]
				else :
					smoothed_pulse_coef = 0.0
				if "fps" in val_interp :
					fps = val_interp["fps"]
				else :
					fps = 30.0
				envelope_sound_volume = current_sound_volume(indFrameOutput, fps, pulse_coef, soundtrack_working_channels)
				peaked_envelope_sound_volume = peaked_current_sound_volume(indFrameOutput, fps, peaked_pulse_coef, soundtrack_working_channels)
				smoothed_envelope_sound_volume = smoothed_current_sound_volume(indFrameOutput, fps, smoothed_pulse_coef, soundtrack_working_channels)
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
				(onset_sound_volume, new_onset) = current_onset_volume(indFrameOutput, fps, onset_pulse_coef, onset_decay_time, soundtrack_working_channels)
			sound_volume =  envelope_sound_volume + peaked_envelope_sound_volume + smoothed_envelope_sound_volume + onset_sound_volume
			if "sound_formula" in val_interp :
				def sound_volume_fct(x) :
					return eval(val_interp["sound_formula"])
				sound_volume_init = sound_volume
				sound_volume = sound_volume_fct(sound_volume)

			if trace_only :
				print("{0:.5f},{1:.3f},{2:.3f},{3:.3f},{4:.3f},{5:.3f}".format(indFrameOutput/30., sound_volume, envelope_sound_volume, peaked_envelope_sound_volume, smoothed_envelope_sound_volume, onset_sound_volume))

			if(not trace_only) and indFrameInput % 1 == 0 :
				print("fillAndStroke {0:s}{1:s} -> {2:s}{3:s} (sound_volume {4:.3f}:({5:.3f}+{6:.3f}+{7:.3f}+{8:.3f}))".format(val_init["svg_input_file_name"], "_"+countInput+".svg", val_init["svg_output_file_name"], "_"+countOutput+".svg", sound_volume, envelope_sound_volume, smoothed_envelope_sound_volume, peaked_envelope_sound_volume, onset_sound_volume))
				fillAndStroke_argts = ["--nb-layers", nb_layers, \
					"-o", full_svg_output_file_name, \
					"-i", full_svg_input_file_name]

				# LAYERS COLOR AND OPACITY
				interp_fill_color_bg = '#000000'
				interp_fill_color_l1 = '#000000'
				interp_fill_color_l2 = '#000000'
				interp_fill_color_l3 = '#000000'
				interp_fill_color_l4 = '#000000'
				interp_fill_color_l5 = '#000000'
				interp_fill_color_l6 = '#000000'
				interp_fill_color_l7 = '#000000'
				interp_fill_color_l8 = '#000000'
				interp_fill_opacity_l1 = '1.0'
				interp_fill_opacity_l2 = '1.0'
				interp_fill_opacity_l3 = '1.0'
				interp_fill_opacity_l4 = '1.0'
				interp_fill_opacity_l5 = '1.0'
				interp_fill_opacity_l6 = '1.0'
				interp_fill_opacity_l7 = '1.0'
				interp_fill_opacity_l8 = '1.0'
				# background rectangle when the layers do not cover the whole pictures
				# as in a grayscale image where layers span from black to light gray
				if("fill_color_bg" in val_interp) :
					interp_fill_color_bg = val_interp["fill_color_bg"]
					if(with_sound_data and "pulsed_fill_color_bg" in val_interp) :
						interp_pulsed_fill_color_bg = val_interp["pulsed_fill_color_bg"]
						interp_fill_color_bg = interpolate(var_types["fill_color_bg"], interp_fill_color_bg, interp_pulsed_fill_color_bg, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_bg", interp_fill_color_bg])
				if("fill_color_l1" in val_interp) :
					interp_fill_color_l1 = val_interp["fill_color_l1"]
					if(with_sound_data and "pulsed_fill_color_l1" in val_interp) :
						interp_pulsed_fill_color_l1 = val_interp["pulsed_fill_color_l1"]
						interp_fill_color_l1 = interpolate(var_types["fill_color_l1"], interp_fill_color_l1, interp_pulsed_fill_color_l1, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l1", interp_fill_color_l1])
				if("fill_color_l2" in val_interp) :
					interp_fill_color_l2 = val_interp["fill_color_l2"]
					if(with_sound_data and "pulsed_fill_color_l2" in val_interp) :
						interp_pulsed_fill_color_l2 = val_interp["pulsed_fill_color_l2"]
						interp_fill_color_l2 = interpolate(var_types["fill_color_l2"], interp_fill_color_l2, interp_pulsed_fill_color_l2, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l2", interp_fill_color_l2])
				if("fill_color_l3" in val_interp) :
					interp_fill_color_l3 = val_interp["fill_color_l3"]
					if(with_sound_data and "pulsed_fill_color_l3" in val_interp) :
						interp_pulsed_fill_color_l3 = val_interp["pulsed_fill_color_l3"]
						interp_fill_color_l3 = interpolate(var_types["fill_color_l3"], interp_fill_color_l3, interp_pulsed_fill_color_l3, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l3", interp_fill_color_l3])
				if("fill_color_l4" in val_interp) :
					interp_fill_color_l4 = val_interp["fill_color_l4"]
					if(with_sound_data and "pulsed_fill_color_l4" in val_interp) :
						interp_pulsed_fill_color_l4 = val_interp["pulsed_fill_color_l4"]
						interp_fill_color_l4 = interpolate(var_types["fill_color_l4"], interp_fill_color_l4, interp_pulsed_fill_color_l4, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l4", interp_fill_color_l4])
				if("fill_color_l5" in val_interp) :
					interp_fill_color_l5 = val_interp["fill_color_l5"]
					if(with_sound_data and "pulsed_fill_color_l5" in val_interp) :
						interp_pulsed_fill_color_l5 = val_interp["pulsed_fill_color_l5"]
						interp_fill_color_l5 = interpolate(var_types["fill_color_l5"], interp_fill_color_l5, interp_pulsed_fill_color_l5, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l5", interp_fill_color_l5])
				if("fill_color_l6" in val_interp) :
					interp_fill_color_l6 = val_interp["fill_color_l6"]
					if(with_sound_data and "pulsed_fill_color_l6" in val_interp) :
						interp_pulsed_fill_color_l6 = val_interp["pulsed_fill_color_l6"]
						interp_fill_color_l6 = interpolate(var_types["fill_color_l6"], interp_fill_color_l6, interp_pulsed_fill_color_l6, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l6", interp_fill_color_l6])
				if("fill_color_l7" in val_interp) :
					interp_fill_color_l7 = val_interp["fill_color_l7"]
					if(with_sound_data and "pulsed_fill_color_l7" in val_interp) :
						interp_pulsed_fill_color_l7 = val_interp["pulsed_fill_color_l7"]
						interp_fill_color_l7 = interpolate(var_types["fill_color_l7"], interp_fill_color_l7, interp_pulsed_fill_color_l7, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l7", interp_fill_color_l7])
				if("fill_color_l8" in val_interp) :
					interp_fill_color_l8 = val_interp["fill_color_l8"]
					if(with_sound_data and "pulsed_fill_color_l8" in val_interp) :
						interp_pulsed_fill_color_l8 = val_interp["pulsed_fill_color_l8"]
						interp_fill_color_l8 = interpolate(var_types["fill_color_l8"], interp_fill_color_l8, interp_pulsed_fill_color_l8, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_color_l8", interp_fill_color_l8])
				if("fill_opacity_l1" in val_interp) :
					interp_fill_opacity_l1 = val_interp["fill_opacity_l1"]
					if(with_sound_data and "pulsed_fill_opacity_l1" in val_interp) :
						interp_pulsed_fill_opacity_l1 = val_interp["pulsed_fill_opacity_l1"]
						interp_fill_opacity_l1 = interpolate(var_types["fill_opacity_l1"], interp_fill_opacity_l1, interp_pulsed_fill_opacity_l1, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l1", format(interp_fill_opacity_l1, '.8f')])
				if("fill_opacity_l2" in val_interp) :
					interp_fill_opacity_l2 = val_interp["fill_opacity_l2"]
					if(with_sound_data and "pulsed_fill_opacity_l2" in val_interp) :
						interp_pulsed_fill_opacity_l2 = val_interp["pulsed_fill_opacity_l2"]
						interp_fill_opacity_l2 = interpolate(var_types["fill_opacity_l2"], interp_fill_opacity_l2, interp_pulsed_fill_opacity_l2, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l2", format(interp_fill_opacity_l2, '.8f')])
				if("fill_opacity_l3" in val_interp) :
					interp_fill_opacity_l3 = val_interp["fill_opacity_l3"]
					if(with_sound_data and "pulsed_fill_opacity_l3" in val_interp) :
						interp_pulsed_fill_opacity_l3 = val_interp["pulsed_fill_opacity_l3"]
						interp_fill_opacity_l3 = interpolate(var_types["fill_opacity_l3"], interp_fill_opacity_l3, interp_pulsed_fill_opacity_l3, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l3", format(interp_fill_opacity_l3, '.8f')])
				if("fill_opacity_l4" in val_interp) :
					interp_fill_opacity_l4 = val_interp["fill_opacity_l4"]
					if(with_sound_data and "pulsed_fill_opacity_l4" in val_interp) :
						interp_pulsed_fill_opacity_l4 = val_interp["pulsed_fill_opacity_l4"]
						interp_fill_opacity_l4 = interpolate(var_types["fill_opacity_l4"], interp_fill_opacity_l4, interp_pulsed_fill_opacity_l4, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l4", format(interp_fill_opacity_l4, '.8f')])
				if("fill_opacity_l5" in val_interp) :
					interp_fill_opacity_l5 = val_interp["fill_opacity_l5"]
					if(with_sound_data and "pulsed_fill_opacity_l5" in val_interp) :
						interp_pulsed_fill_opacity_l5 = val_interp["pulsed_fill_opacity_l5"]
						interp_fill_opacity_l5 = interpolate(var_types["fill_opacity_l5"], interp_fill_opacity_l5, interp_pulsed_fill_opacity_l5, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l5", format(interp_fill_opacity_l5, '.8f')])
				if("fill_opacity_l6" in val_interp) :
					interp_fill_opacity_l6 = val_interp["fill_opacity_l6"]
					if(with_sound_data and "pulsed_fill_opacity_l6" in val_interp) :
						interp_pulsed_fill_opacity_l6 = val_interp["pulsed_fill_opacity_l6"]
						interp_fill_opacity_l6 = interpolate(var_types["fill_opacity_l6"], interp_fill_opacity_l6, interp_pulsed_fill_opacity_l6, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l6", format(interp_fill_opacity_l6, '.8f')])
				if("fill_opacity_l7" in val_interp) :
					interp_fill_opacity_l7 = val_interp["fill_opacity_l7"]
					if(with_sound_data and "pulsed_fill_opacity_l7" in val_interp) :
						interp_pulsed_fill_opacity_l7 = val_interp["pulsed_fill_opacity_l7"]
						interp_fill_opacity_l7 = interpolate(var_types["fill_opacity_l7"], interp_fill_opacity_l7, interp_pulsed_fill_opacity_l7, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l7", format(interp_fill_opacity_l7, '.8f')])
				if("fill_opacity_l8" in val_interp) :
					interp_fill_opacity_l8 = val_interp["fill_opacity_l8"]
					if(with_sound_data and "pulsed_fill_opacity_l8" in val_interp) :
						interp_pulsed_fill_opacity_l8 = val_interp["pulsed_fill_opacity_l8"]
						interp_fill_opacity_l8 = interpolate(var_types["fill_opacity_l8"], interp_fill_opacity_l8, interp_pulsed_fill_opacity_l8, 'l', sound_volume)
					fillAndStroke_argts.extend(["--fill_opacity_l8", format(interp_fill_opacity_l8, '.8f')])

				
				# STROKES COLOR, WIDTH, AND OPACITY
				interp_stroke_color_l1 = '#000000'
				interp_stroke_color_l2 = '#000000'
				interp_stroke_color_l3 = '#000000'
				interp_stroke_color_l4 = '#000000'
				interp_stroke_color_l5 = '#000000'
				interp_stroke_color_l6 = '#000000'
				interp_stroke_color_l7 = '#000000'
				interp_stroke_color_l8 = '#000000'
				interp_stroke_opacity_l1 = '1.0'
				interp_stroke_opacity_l2 = '1.0'
				interp_stroke_opacity_l3 = '1.0'
				interp_stroke_opacity_l4 = '1.0'
				interp_stroke_opacity_l5 = '1.0'
				interp_stroke_opacity_l6 = '1.0'
				interp_stroke_opacity_l7 = '1.0'
				interp_stroke_opacity_l8 = '1.0'
				interp_stroke_width_l1 = '0.0'
				interp_stroke_width_l2 = '0.0'
				interp_stroke_width_l3 = '0.0'
				interp_stroke_width_l4 = '0.0'
				interp_stroke_width_l5 = '0.0'
				interp_stroke_width_l6 = '0.0'
				interp_stroke_width_l7 = '0.0'
				interp_stroke_width_l8 = '0.0'
				interp_stroke_color = '#000000'
				interp_stroke_opacity = '1.0'
				interp_stroke_width = '0.0'
				# stroke color, width and opacity can be provided as a single and unified value for all layers
				# or in a layer by layer mode
				if("stroke_color_l1" in val_interp) :
					interp_stroke_color_l1 = val_interp["stroke_color_l1"]
					if(with_sound_data and "pulsed_stroke_color_l1" in val_interp) :
						interp_pulsed_stroke_color_l1 = val_interp["pulsed_stroke_color_l1"]
						interp_stroke_color_l1 = interpolate(var_types["stroke_color_l1"], interp_stroke_color_l1, interp_pulsed_stroke_color_l1, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l1", interp_stroke_color_l1])
				if("stroke_color_l2" in val_interp) :
					interp_stroke_color_l2 = val_interp["stroke_color_l2"]
					if(with_sound_data and "pulsed_stroke_color_l2" in val_interp) :
						interp_pulsed_stroke_color_l2 = val_interp["pulsed_stroke_color_l2"]
						interp_stroke_color_l2 = interpolate(var_types["stroke_color_l2"], interp_stroke_color_l2, interp_pulsed_stroke_color_l2, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l2", interp_stroke_color_l2])
				if("stroke_color_l3" in val_interp) :
					interp_stroke_color_l3 = val_interp["stroke_color_l3"]
					if(with_sound_data and "pulsed_stroke_color_l3" in val_interp) :
						interp_pulsed_stroke_color_l3 = val_interp["pulsed_stroke_color_l3"]
						interp_stroke_color_l3 = interpolate(var_types["stroke_color_l3"], interp_stroke_color_l3, interp_pulsed_stroke_color_l3, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l3", interp_stroke_color_l3])
				if("stroke_color_l4" in val_interp) :
					interp_stroke_color_l4 = val_interp["stroke_color_l4"]
					if(with_sound_data and "pulsed_stroke_color_l4" in val_interp) :
						interp_pulsed_stroke_color_l4 = val_interp["pulsed_stroke_color_l4"]
						interp_stroke_color_l4 = interpolate(var_types["stroke_color_l4"], interp_stroke_color_l4, interp_pulsed_stroke_color_l4, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l4", interp_stroke_color_l4])
				if("stroke_color_l5" in val_interp) :
					interp_stroke_color_l5 = val_interp["stroke_color_l5"]
					if(with_sound_data and "pulsed_stroke_color_l5" in val_interp) :
						interp_pulsed_stroke_color_l5 = val_interp["pulsed_stroke_color_l5"]
						interp_stroke_color_l5 = interpolate(var_types["stroke_color_l5"], interp_stroke_color_l5, interp_pulsed_stroke_color_l5, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l5", interp_stroke_color_l5])
				if("stroke_color_l6" in val_interp) :
					interp_stroke_color_l6 = val_interp["stroke_color_l6"]
					if(with_sound_data and "pulsed_stroke_color_l6" in val_interp) :
						interp_pulsed_stroke_color_l6 = val_interp["pulsed_stroke_color_l6"]
						interp_stroke_color_l6 = interpolate(var_types["stroke_color_l6"], interp_stroke_color_l6, interp_pulsed_stroke_color_l6, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l6", interp_stroke_color_l6])
				if("stroke_color_l7" in val_interp) :
					interp_stroke_color_l7 = val_interp["stroke_color_l7"]
					if(with_sound_data and "pulsed_stroke_color_l7" in val_interp) :
						interp_pulsed_stroke_color_l7 = val_interp["pulsed_stroke_color_l7"]
						interp_stroke_color_l7 = interpolate(var_types["stroke_color_l7"], interp_stroke_color_l7, interp_pulsed_stroke_color_l7, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l7", interp_stroke_color_l7])
				if("stroke_color_l8" in val_interp) :
					interp_stroke_color_l8 = val_interp["stroke_color_l8"]
					if(with_sound_data and "pulsed_stroke_color_l8" in val_interp) :
						interp_pulsed_stroke_color_l8 = val_interp["pulsed_stroke_color_l8"]
						interp_stroke_color_l8 = interpolate(var_types["stroke_color_l8"], interp_stroke_color_l8, interp_pulsed_stroke_color_l8, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color_l8", interp_stroke_color_l8])
				if("stroke_opacity_l1" in val_interp) :
					interp_stroke_opacity_l1 = val_interp["stroke_opacity_l1"]
					if(with_sound_data and "pulsed_stroke_opacity_l1" in val_interp) :
						interp_pulsed_stroke_opacity_l1 = val_interp["pulsed_stroke_opacity_l1"]
						interp_stroke_opacity_l1 = interpolate(var_types["stroke_opacity_l1"], interp_stroke_opacity_l1, interp_pulsed_stroke_opacity_l1, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l1", format(interp_stroke_opacity_l1, '.8f')])
				if("stroke_opacity_l2" in val_interp) :
					interp_stroke_opacity_l2 = val_interp["stroke_opacity_l2"]
					if(with_sound_data and "pulsed_stroke_opacity_l2" in val_interp) :
						interp_pulsed_stroke_opacity_l2 = val_interp["pulsed_stroke_opacity_l2"]
						interp_stroke_opacity_l2 = interpolate(var_types["stroke_opacity_l2"], interp_stroke_opacity_l2, interp_pulsed_stroke_opacity_l2, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l2", format(interp_stroke_opacity_l2, '.8f')])
				if("stroke_opacity_l3" in val_interp) :
					interp_stroke_opacity_l3 = val_interp["stroke_opacity_l3"]
					if(with_sound_data and "pulsed_stroke_opacity_l3" in val_interp) :
						interp_pulsed_stroke_opacity_l3 = val_interp["pulsed_stroke_opacity_l3"]
						interp_stroke_opacity_l3 = interpolate(var_types["stroke_opacity_l3"], interp_stroke_opacity_l3, interp_pulsed_stroke_opacity_l3, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l3", format(interp_stroke_opacity_l3, '.8f')])
				if("stroke_opacity_l4" in val_interp) :
					interp_stroke_opacity_l4 = val_interp["stroke_opacity_l4"]
					if(with_sound_data and "pulsed_stroke_opacity_l4" in val_interp) :
						interp_pulsed_stroke_opacity_l4 = val_interp["pulsed_stroke_opacity_l4"]
						interp_stroke_opacity_l4 = interpolate(var_types["stroke_opacity_l4"], interp_stroke_opacity_l4, interp_pulsed_stroke_opacity_l4, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l4", format(interp_stroke_opacity_l4, '.8f')])
				if("stroke_opacity_l5" in val_interp) :
					interp_stroke_opacity_l5 = val_interp["stroke_opacity_l5"]
					if(with_sound_data and "pulsed_stroke_opacity_l5" in val_interp) :
						interp_pulsed_stroke_opacity_l5 = val_interp["pulsed_stroke_opacity_l5"]
						interp_stroke_opacity_l5 = interpolate(var_types["stroke_opacity_l5"], interp_stroke_opacity_l5, interp_pulsed_stroke_opacity_l5, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l5", format(interp_stroke_opacity_l5, '.8f')])
				if("stroke_opacity_l6" in val_interp) :
					interp_stroke_opacity_l6 = val_interp["stroke_opacity_l6"]
					if(with_sound_data and "pulsed_stroke_opacity_l6" in val_interp) :
						interp_pulsed_stroke_opacity_l6 = val_interp["pulsed_stroke_opacity_l6"]
						interp_stroke_opacity_l6 = interpolate(var_types["stroke_opacity_l6"], interp_stroke_opacity_l6, interp_pulsed_stroke_opacity_l6, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l6", format(interp_stroke_opacity_l6, '.8f')])
				if("stroke_opacity_l7" in val_interp) :
					interp_stroke_opacity_l7 = val_interp["stroke_opacity_l7"]
					if(with_sound_data and "pulsed_stroke_opacity_l7" in val_interp) :
						interp_pulsed_stroke_opacity_l7 = val_interp["pulsed_stroke_opacity_l7"]
						interp_stroke_opacity_l7 = interpolate(var_types["stroke_opacity_l7"], interp_stroke_opacity_l7, interp_pulsed_stroke_opacity_l7, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l7", format(interp_stroke_opacity_l7, '.8f')])
				if("stroke_opacity_l8" in val_interp) :
					interp_stroke_opacity_l8 = val_interp["stroke_opacity_l8"]
					if(with_sound_data and "pulsed_stroke_opacity_l8" in val_interp) :
						interp_pulsed_stroke_opacity_l8 = val_interp["pulsed_stroke_opacity_l8"]
						interp_stroke_opacity_l8 = interpolate(var_types["stroke_opacity_l8"], interp_stroke_opacity_l8, interp_pulsed_stroke_opacity_l8, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity_l8", format(interp_stroke_opacity_l8, '.8f')])
				if("stroke_width_l1" in val_interp) :
					interp_stroke_width_l1 = val_interp["stroke_width_l1"]
					if(with_sound_data and "pulsed_stroke_width_l1" in val_interp) :
						interp_pulsed_stroke_width_l1 = val_interp["pulsed_stroke_width_l1"]
						interp_stroke_width_l1 = interpolate(var_types["stroke_width_l1"], interp_stroke_width_l1, interp_pulsed_stroke_width_l1, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l1", format(interp_stroke_width_l1, '.8f')])
				if("stroke_width_l2" in val_interp) :
					interp_stroke_width_l2 = val_interp["stroke_width_l2"]
					if(with_sound_data and "pulsed_stroke_width_l2" in val_interp) :
						interp_pulsed_stroke_width_l2 = val_interp["pulsed_stroke_width_l2"]
						interp_stroke_width_l2 = interpolate(var_types["stroke_width_l2"], interp_stroke_width_l2, interp_pulsed_stroke_width_l2, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l2", format(interp_stroke_width_l2, '.8f')])
				if("stroke_width_l3" in val_interp) :
					interp_stroke_width_l3 = val_interp["stroke_width_l3"]
					if(with_sound_data and "pulsed_stroke_width_l3" in val_interp) :
						interp_pulsed_stroke_width_l3 = val_interp["pulsed_stroke_width_l3"]
						interp_stroke_width_l3 = interpolate(var_types["stroke_width_l3"], interp_stroke_width_l3, interp_pulsed_stroke_width_l3, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l3", format(interp_stroke_width_l3, '.8f')])
				if("stroke_width_l4" in val_interp) :
					interp_stroke_width_l4 = val_interp["stroke_width_l4"]
					if(with_sound_data and "pulsed_stroke_width_l4" in val_interp) :
						interp_pulsed_stroke_width_l4 = val_interp["pulsed_stroke_width_l4"]
						interp_stroke_width_l4 = interpolate(var_types["stroke_width_l4"], interp_stroke_width_l4, interp_pulsed_stroke_width_l4, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l4", format(interp_stroke_width_l4, '.8f')])
				if("stroke_width_l5" in val_interp) :
					interp_stroke_width_l5 = val_interp["stroke_width_l5"]
					if(with_sound_data and "pulsed_stroke_width_l5" in val_interp) :
						interp_pulsed_stroke_width_l5 = val_interp["pulsed_stroke_width_l5"]
						interp_stroke_width_l5 = interpolate(var_types["stroke_width_l5"], interp_stroke_width_l5, interp_pulsed_stroke_width_l5, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l5", format(interp_stroke_width_l5, '.8f')])
				if("stroke_width_l6" in val_interp) :
					interp_stroke_width_l6 = val_interp["stroke_width_l6"]
					if(with_sound_data and "pulsed_stroke_width_l6" in val_interp) :
						interp_pulsed_stroke_width_l6 = val_interp["pulsed_stroke_width_l6"]
						interp_stroke_width_l6 = interpolate(var_types["stroke_width_l6"], interp_stroke_width_l6, interp_pulsed_stroke_width_l6, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l6", format(interp_stroke_width_l6, '.8f')])
				if("stroke_width_l7" in val_interp) :
					interp_stroke_width_l7 = val_interp["stroke_width_l7"]
					if(with_sound_data and "pulsed_stroke_width_l7" in val_interp) :
						interp_pulsed_stroke_width_l7 = val_interp["pulsed_stroke_width_l7"]
						interp_stroke_width_l7 = interpolate(var_types["stroke_width_l7"], interp_stroke_width_l7, interp_pulsed_stroke_width_l7, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l7", format(interp_stroke_width_l7, '.8f')])
				if("stroke_width_l8" in val_interp) :
					interp_stroke_width_l8 = val_interp["stroke_width_l8"]
					if(with_sound_data and "pulsed_stroke_width_l8" in val_interp) :
						interp_pulsed_stroke_width_l8 = val_interp["pulsed_stroke_width_l8"]
						interp_stroke_width_l8 = interpolate(var_types["stroke_width_l8"], interp_stroke_width_l8, interp_pulsed_stroke_width_l8, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width_l8", format(interp_stroke_width_l8, '.8f')])

				if("stroke_color" in val_interp) :
					interp_stroke_color = val_interp["stroke_color"]
					if(with_sound_data and "pulsed_stroke_color" in val_interp) :
						interp_pulsed_stroke_color = val_interp["pulsed_stroke_color"]
						interp_stroke_color = interpolate(var_types["stroke_color"], interp_stroke_color, interp_pulsed_stroke_color, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_color", format(interp_stroke_color, '.8f')])
				if("stroke_opacity" in val_interp) :
					interp_stroke_opacity = val_interp["stroke_opacity"]
					if(with_sound_data and "pulsed_stroke_opacity" in val_interp) :
						interp_pulsed_stroke_opacity = val_interp["pulsed_stroke_opacity"]
						interp_stroke_opacity = interpolate(var_types["stroke_opacity"], interp_stroke_opacity, interp_pulsed_stroke_opacity, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_opacity", format(interp_stroke_opacity, '.8f')])
				if("stroke_width" in val_interp) :
					interp_stroke_width = val_interp["stroke_width"]
					if(with_sound_data and "pulsed_stroke_width" in val_interp) :
						interp_pulsed_stroke_width = val_interp["pulsed_stroke_width"]
						interp_stroke_width = interpolate(var_types["stroke_width"], interp_stroke_width, interp_pulsed_stroke_width, 'l', sound_volume)
					fillAndStroke_argts.extend(["--stroke_width", format(interp_stroke_width, '.8f')])

				# image temperature, saturation or brightness used to balance the colors of the layers
				temperature = "6600"
				if("temperature" in val_interp) :
					interp_temperature = val_interp["temperature"]
					if(with_sound_data and "pulsed_temperature" in val_interp) :
						interp_pulsed_temperature = val_interp["pulsed_temperature"]
						interp_temperature = interpolate(var_types["temperature"], interp_temperature, interp_pulsed_temperature, 'l', sound_volume)
					fillAndStroke_argts.extend(["--temperature", format(interp_temperature, '.8f')])
				if("saturation" in val_interp) :
					interp_saturation = val_interp["saturation"]
					if(with_sound_data and "pulsed_saturation" in val_interp) :
						interp_pulsed_saturation = val_interp["pulsed_saturation"]
						interp_saturation = interpolate(var_types["saturation"], interp_saturation, interp_pulsed_saturation, 'l', sound_volume)
					fillAndStroke_argts.extend(["--saturation", format(interp_saturation, '.8f')])
				if("brightness" in val_interp) :
					interp_brightness = val_interp["brightness"]
					if(with_sound_data and "pulsed_brightness" in val_interp) :
						interp_pulsed_brightness = val_interp["pulsed_brightness"]
						interp_brightness = interpolate(var_types["brightness"], interp_brightness, interp_pulsed_brightness, 'l', sound_volume)
					fillAndStroke_argts.extend(["--brightness", format(interp_brightness, '.8f')])

				# LAYERS BLUR
				interp_blur_l1 = '0.0'
				interp_blur_l2 = '0.0'
				interp_blur_l3 = '0.0'
				interp_blur_l4 = '0.0'
				interp_blur_l5 = '0.0'
				interp_blur_l6 = '0.0'
				interp_blur_l7 = '0.0'
				interp_blur_l8 = '0.0'
				if("blur_l1" in val_interp) :
					interp_blur_l1 = format(val_interp["blur_l1"], '.8f')
				fillAndStroke_argts.extend(["--blur_l1", interp_blur_l1])
				if("blur_l2" in val_interp) :
					interp_blur_l2 = format(val_interp["blur_l2"], '.8f')
				fillAndStroke_argts.extend(["--blur_l2", interp_blur_l2])
				if("blur_l3" in val_interp) :
					interp_blur_l3 = format(val_interp["blur_l3"], '.8f')
				fillAndStroke_argts.extend(["--blur_l3", interp_blur_l3])
				if("blur_l4" in val_interp) :
					interp_blur_l4 = format(val_interp["blur_l4"], '.8f')
				fillAndStroke_argts.extend(["--blur_l4", interp_blur_l4])
				if("blur_l5" in val_interp) :
					interp_blur_l5 = format(val_interp["blur_l5"], '.8f')
				fillAndStroke_argts.extend(["--blur_l5", interp_blur_l5])
				if("blur_l6" in val_interp) :
					interp_blur_l6 = format(val_interp["blur_l6"], '.8f')
				fillAndStroke_argts.extend(["--blur_l6", interp_blur_l6])
				if("blur_l7" in val_interp) :
					interp_blur_l7 = format(val_interp["blur_l7"], '.8f')
				fillAndStroke_argts.extend(["--blur_l7", interp_blur_l7])
				if("blur_l8" in val_interp) :
					interp_blur_l8 = format(val_interp["blur_l8"], '.8f')
				fillAndStroke_argts.extend(["--blur_l8", interp_blur_l8])

				if("fill_and_stroke_mode" in val_interp) :
					fillAndStroke_argts.extend(["--fill_and_stroke_mode", str(val_interp["fill_and_stroke_mode"])])

				# print("vv_fillAndStroke: ", fillAndStroke_argts)
				# print("Image number ", indFrameInput)
				if( vv_fillAndStroke.main(fillAndStroke_argts) == 0) :
					return 0
			
			# else :
			# 	print("cp "+os.path.join(tmp_dir+"tmp_transformed.svg")+" "+full_svg_output_file_name)
				# os.system("cp "+os.path.join(tmp_dir+"tmp_transformed.svg")+" "+full_svg_output_file_name)

			# removes input frame
			print ("rm: ", full_svg_input_file_name)
			os.system("rm -f " + full_svg_input_file_name)

			# moves forward on output frames
			indFrameOutput += 1

			# only one frame if frame ini and frame end are equal
			if (pas == 0):
				break

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
