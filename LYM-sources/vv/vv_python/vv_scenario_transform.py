#!/usr/bin/python3 

# vv_scenario_transform.py
# scenario for transformations of vector video frames (both initial transformation before crossfading and final transformations after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import vv_transform
import vv_transform_obj_along_path
import vv_transform_interpolation
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
from vv_lib import random_target_regular_expression_name

import csv
import re
import getopt
import sys
import random
from signal import signal, SIGINT

import numpy as np
import cv2 as cv

from math import sin, cos

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_transform.py -i scenario_file
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
	with_fill_and_stroke = True

	fps = 30
	pulse_coef = 0
	smoothed_pulse_coef = 0
	with_sound_data = False
	onset_pulse_coef = 0
	onset_decay_time = 0
	with_onset_data = False
	new_onset = False
	input_loop_size = 0

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:s:",["inputfile=", "chapter_no=", "with_fill_and_stroke="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		elif opt == "--chapter_no":
			scenario_chapter_no = arg
		elif opt == "--with_fill_and_stroke":
			with_fill_and_stroke = (arg == "True" or arg == "true" or arg == "TRUE" or arg == True)
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
		
		# if not(ind_scene == 0) :
		# # if not((ind_scene == 11) or (ind_scene == 12) or (ind_scene == 13) or (ind_scene == 14)) :
		# 	continue

		# printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", nb_scenes, scene_ID,
		#   val_init["svg_input_file_name"], val_init["imageInputIndex"], 
		#   val_fin:"imageInputIndex", val_init["imageOutputIndex"], 
		#   val_fin:"imageOutputIndex")

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
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		# print("read sound data")
		with_sound_data = read_sound_data(val_init)
		with_onset_data = read_onset_data(val_init)
		# print("with_sound_data with_onset_data", with_sound_data, with_onset_data)

		##################################################################
		# SCANS ALL THE FRAMES IN THE MOVIE
		##################################################################
		# if the input has only one frame, it is an image and the index
		# of the input images is fixex while the index of the output images
		# is guided by the imageOutputIndex 
		start_of_counting = val_init["imageInputIndex"]
		end_of_counting = val_fin["imageInputIndex"] 
		input_is_leading = True 
		if(start_of_counting == end_of_counting) :
			start_of_counting = val_init["imageOutputIndex"]
			end_of_counting = val_fin["imageOutputIndex"] 
			input_is_leading = False 

		if start_of_counting < end_of_counting:
			pas = 1
		else :
			pas = -1

		indFrameOutput = val_init["imageOutputIndex"]

		if "input_loop_size" in val_init :
			input_loop_size = val_init["input_loop_size"]
		nb_layers = val_init["nb_posterization_layers"]
		tmp_dir = val_init["tmp_directory"]
		switched_target_file_name = None
		switched_target_next_indFrameInput = -1
		# print("nb frames", start_of_counting, end_of_counting)

		# transformation ID
		transformation_function_string = val_init["transf1_ID"]

		# loop over all the frames in the scene
		for indCurrentFrame in range(start_of_counting, end_of_counting + pas, pas):
			if(input_is_leading == True) :
				indFrameInput = indCurrentFrame
			else :
				indFrameInput = val_init["imageInputIndex"]

			countInput = "%05d" % indFrameInput
			countOutput = "%05d" % indFrameOutput
			# print(countInput, countOutput)

			# if (indFrameInput != 12332) :
			# if not((ind_scene == 11 and indFrameInput % 10 == 0) or (ind_scene == 12 and indFrameInput % 10 == 0) or (ind_scene == 13 and indFrameInput % 40 == 0) or (ind_scene == 14 and indFrameInput % 40 == 0))  :
			# 	indFrameOutput += 1
			# 	continue
			# if indFrameInput <= 2680 or indFrameInput >= 2702 :
			# 	indFrameOutput += 1
			# 	continue

			# scene percentage at this frame 
			scene_percentage = 0
			if pas != 0 and (end_of_counting - start_of_counting) != 0 :
				scene_percentage = float(indCurrentFrame - start_of_counting)/(end_of_counting - start_of_counting)
			#clamping
			scene_percentage = clamp(scene_percentage)

			# interpolating all the variables
			for var_name in var_names :
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])
			
			if(trace_only) :
				print("Ind image %d Scene percentage %.2f percent_transf1 %.2f fill_opacity_l1 %.2f fill_color_l4 %s fill_color_bg %s\n" % \
					indCurrentFrame, scene_percentage, val_interp["percent_transf1"], val_interp["fill_opacity_l1"], val_interp["fill_color_l4"], val_interp["fill_color_bg"])

			# single image or video frames
			if(("svg_input_directory" in val_interp) and (("svg_input_image_file_name" in val_interp) or ("svg_input_file_name" in val_interp))) :
				svg_input_directory = val_interp["svg_input_directory"]
				# single image 
				if ("svg_input_image_file_name" in val_interp):
					svg_input_image_file_name = val_interp["svg_input_image_file_name"] + ".svg"
					full_svg_input_file_name = os.path.join(svg_input_directory, svg_input_image_file_name)
				# video frames
				else:
					svg_input_image_file_name_without_extension = val_interp["svg_input_file_name"]
					svg_input_image_file_name = svg_input_image_file_name_without_extension+"_"+countInput+".svg"
					# SVG input file name
					full_svg_input_file_name_without_extension = os.path.join(svg_input_directory, svg_input_image_file_name_without_extension)
					full_svg_input_file_name = full_svg_input_file_name_without_extension+"_"+countInput+".svg"
					if input_is_leading == True and not os.path.isfile(full_svg_input_file_name) :
						# print("input_loop_size {0:d} {1:d} {2:d}".format(input_loop_size, indCurrentFrame, (indCurrentFrame - 1) % input_loop_size + 1))
						if input_loop_size > 0 :
							indFrameInput_looped = (indFrameInput - 1) % input_loop_size + 1
							countInput = "%05d" % indFrameInput_looped
							full_svg_input_file_name = full_svg_input_file_name_without_extension+"_"+countInput+".svg"
						if not os.path.isfile(full_svg_input_file_name) :
							print("Incorrect frame input number {0:s} (full name {1:s})".format(countInput, full_svg_input_file_name))
							sys.exit(0)
			else :
				print("svg_input_directory or svg_input_image_file_name or svg_input_file_name not among the provided variables")
				sys.exit(0)

			# SVG output file name
			full_svg_output_file_name = os.path.join(val_init["svg_output_directory"], val_init["svg_output_file_name"])
			full_svg_output_file_name = full_svg_output_file_name+"_"+countOutput+".svg"

			# reads the sound volume corresponding to the soundtrack at the timecode of the current frame
			envelope_sound_volume = 0
			smoothed_envelope_sound_volume = 0
			onset_sound_volume = 0
			sound_volume = 0
			new_onset = False
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
				# print("current_onset_volume",indFrameOutput, fps, onset_pulse_coef, onset_decay_time)
				(onset_sound_volume, new_onset) = current_onset_volume(indFrameOutput, fps, onset_pulse_coef, onset_decay_time, soundtrack_working_channels)
				# print("onset volume", onset_sound_volume, new_onset)

			sound_volume = 0.
			if with_sound_data :
				sound_volume +=  envelope_sound_volume + peaked_envelope_sound_volume + smoothed_envelope_sound_volume
			if with_onset_data :
				sound_volume += onset_sound_volume
			if with_sound_data or with_onset_data :
				if "sound_formula" in val_interp :
					def sound_volume_fct(x) :
						return eval(val_interp["sound_formula"])
					sound_volume_init = sound_volume
					sound_volume = sound_volume_fct(sound_volume)
					# print(sound_volume_init, sound_volume, val_interp["sound_formula"])

			if trace_only :
				print("{0:.5f},{1:.3f},{2:.3f},{3:.3f},{4:.3f},{5:.3f}".format(indFrameOutput/30., sound_volume, envelope_sound_volume, peaked_envelope_sound_volume, smoothed_envelope_sound_volume, onset_sound_volume))

			percent_argts = []
			if("percent_transf1" in val_interp) :
				# percentage of transformation
				percent_transf = val_interp["percent_transf1"]
				if((with_sound_data or with_onset_data) and "pulsed_percent_transf1" in val_interp) :
					pulsed_percent_transf = val_interp["pulsed_percent_transf1"]
					percent_transf = interpolate(var_types["percent_transf1"], percent_transf, pulsed_percent_transf, 'l', sound_volume)
				#clamping
				percent_transf1 = clamp(percent_transf)
				percent_argts = ["--percent_transf1", percent_transf1 ]
			if(re.search(r'/', transformation_function_string)):
				single_transf_strings = re.split(r'/', transformation_function_string.rstrip())
				for ind in range(1, len(single_transf_strings), 1):
					if("percent_transf"+str(ind+1) in val_interp):
						# percentage of transformation
						percent_transf = val_interp["percent_transf"+str(ind+1)]
						if(with_sound_data and "pulsed_percent_transf"+str(ind+1) in val_interp) :
							pulsed_percent_transf = val_interp["pulsed_percent_transf"+str(ind+1)]
							percent_transf = interpolate(var_types["percent_transf"+str(ind+1)], percent_transf, pulsed_percent_transf, 'l', sound_volume)
						#clamping
						percent_transf = clamp(percent_transf)
						# concatenation
						percent_argts.extend(["--percent_transf"+str(ind+1), percent_transf])
			# print("from scenario percent_transf ", percent_transf)

			if not trace_only and indFrameInput % 1 == 0 : # and indFrameInput >= 6885 and indFrameInput <= 6900
				transformation_argts = ["--nb_layers", nb_layers, \
						"-o", os.path.join(tmp_dir, "tmp_transformed.svg"), \
						"-i", full_svg_input_file_name, \
						"--transformation", transformation_function_string, \
						"--chapter_no", scenario_chapter_no] + percent_argts
				# print( "Transformed SVG file scenario, percentage and arguments", scenario_file_name, percent_transf, transformation_argts)

				######################################################
				# DISPLACEMENT MAPS
				# displacement map file name
				if(("displacement_map_directory" in val_init)  and ("displacement_map_file_name" in val_init) \
					and ("displacement_map_inputIndex" in val_interp) and ("displacement_map_factor" in val_interp)) :
					# displacement map file name
					displacement_map_file_name = os.path.join(val_init["displacement_map_directory"], val_init["displacement_map_file_name"])
					displacement_map_count = "%04d" % int(val_interp["displacement_map_inputIndex"])
					displacement_map_1_file_name = str(displacement_map_file_name)+"_1_"+displacement_map_count+".exr"
					displacement_map_2_file_name = str(displacement_map_file_name)+"_2_"+displacement_map_count+".exr"
					transformation_argts.extend(["--displacement_map", str(displacement_map_1_file_name)+"&"+str(displacement_map_2_file_name), \
						"--displacement_map_factor", format(val_interp["displacement_map_factor"], '.8f'),"--displacement_map_center", str(val_interp["displacement_map_center"])])


				######################################################
				# LIFE DURATION OF A CLIP
				# number of frames of life of a layer
				if(("nb_frames_transf1" in val_interp) ) :
					nb_frames_transf = val_interp["nb_frames_transf1"]
					transformation_argts.extend(["--nb_frames", nb_frames_transf])

				# percentage of life until the end of the chapter for a layer
				if(("percent_life_transf1" in val_interp) ) :
					percent_life_transf = val_interp["percent_life_transf1"]
					transformation_argts.extend(["--percent_life", percent_life_transf])

				######################################################
				# LINKS BETWEEN CLIPS
				if(("percent_links1" in val_interp) ) :
					# percentage of links
					percent_links = val_interp["percent_links1"]
					#clamping
					percent_links = clamp(percent_links)
					transformation_argts.extend(["--percent_links", percent_links])

				# passes color links arguments from the scenario file
				if(("stroke_color_links1" in val_interp) ) :
					stroke_color_links = val_interp["stroke_color_links1"]
					transformation_argts.extend(["--stroke_color_links", stroke_color_links])

				######################################################
				# TRANSLATION
				# passes translation arguments from the scenario file: translation vector
				if ("x_translation" in val_interp) and ("y_translation" in val_interp):
					transformation_argts.extend(["--translation1", format(val_interp["x_translation"], '.8f')+"x"+format(val_interp["y_translation"], '.8f')])
				elif ("x_translation1" in val_interp) and ("y_translation1" in val_interp):
					transformation_argts.extend(["--translation1", format(val_interp["x_translation"], '.8f')+"x"+format(val_interp["y_translation"], '.8f')])
				if(re.search(r'/', transformation_function_string)):
					single_transf_strings = re.split(r'/', transformation_function_string.rstrip())
					for ind in range(1,len(single_transf_strings)):
						if(("x_translation"+str(ind+1) in val_interp) and ("y_translation"+str(ind+1) in val_interp)):
							# concatenation
							transformation_argts.extend(["--translation"+str(ind+1), format(val_interp["x_translation"+str(ind+1)], '.8f')+"x"+ format(val_interp["y_translation"+str(ind+1)], '.8f')])

				######################################################
				# ROTATION
				# passes rotation arguments from the scenario file: translation vector
				if(("x_rotation_center" in val_interp) and ("y_rotation_center" in val_interp) and ("rotation_angle" in val_interp)) :
					transformation_argts.extend(["--rotation1", format(val_interp["x_rotation_center"], '.8f')+"x"+ format(val_interp["y_rotation_center"], '.8f')+"x"+ format(val_interp["rotation_angle"], '.8f')])
				elif(("x_rotation_center1" in val_interp) and ("y_rotation_center1" in val_interp) and ("rotation_angle1" in val_interp)) :
					transformation_argts.extend(["--rotation1", format(val_interp["x_rotation_center1"], '.8f')+"x"+ format(val_interp["y_rotation_center1"], '.8f')+"x"+ format(val_interp["rotation_angle1"], '.8f')])
				if(re.search(r'/', transformation_function_string)):
					single_transf_strings = re.split(r'/', transformation_function_string.rstrip())
					for ind in range(1,len(single_transf_strings)):
						if(("x_rotation_center"+str(ind+1) in val_interp) and ("y_rotation_center"+str(ind+1) in val_interp) and ("rotation_angle"+str(ind+1) in val_interp)):
							# concatenation
							transformation_argts.extend(["--rotation"+str(ind+1), format(val_interp["x_rotation_center"+str(ind+1)], '.8f')+"x"+ format(val_interp["y_rotation_center"+str(ind+1)], '.8f')+"x"+ format(val_interp["rotation_angle"+str(ind+1)], '.8f')])
				
				######################################################
				# SCALING
				# passes scaling arguments from the scenario file: translation vector
				if(("x_scaling_center" in val_interp) and ("y_scaling_center" in val_interp) and ("scaling_factor" in val_interp)) :
					transformation_argts.extend(["--scale1", format(val_interp["x_scaling_center"], '.8f')+"x"+ format(val_interp["y_scaling_center"], '.8f')+"x"+ format(val_interp["scaling_factor"], '.8f')])
				elif(("x_scaling_center1" in val_interp) and ("y_scaling_center1" in val_interp) and ("scaling_factor1" in val_interp)) :
					transformation_argts.extend(["--scale1", format(val_interp["x_scaling_center1"], '.8f')+"x"+ format(val_interp["y_scaling_center1"], '.8f')+"x"+ format(val_interp["scaling_factor1"], '.8f')])
				if(re.search(r'/', transformation_function_string)):
					single_transf_strings = re.split(r'/', transformation_function_string.rstrip())
					for ind in range(1,len(single_transf_strings)):
						if(("x_scaling_center"+str(ind+1) in val_interp) and ("y_scaling_center"+str(ind+1) in val_interp) and ("scaling_factor"+str(ind+1) in val_interp)):
							# concatenation
							# print(["--scale"+str(ind+1), str(val_interp["x_scaling_center"+str(ind+1)])+"x"+ str(val_interp["y_scaling_center"+str(ind+1)])+"x"+ str(val_interp["scaling_factor"+str(ind+1)])])
							transformation_argts.extend(["--scale"+str(ind+1), format(val_interp["x_scaling_center"+str(ind+1)], '.8f')+"x"+ format(val_interp["y_scaling_center"+str(ind+1)], '.8f')+"x"+ format(val_interp["scaling_factor"+str(ind+1)], '.8f')])
					
				######################################################
				# BEZIER CURVES
				# passes Bezier arguments from the scenario file: control points
				if(("b0_x" in val_interp) and ("b0_y" in val_interp) and ("b1_x" in val_interp) and ("b1_y" in val_interp) and ("b2_x" in val_interp) and ("b2_y" in val_interp) and ("b3_x" in val_interp) and ("b3_y" in val_interp)) :
					interp_b0_x = val_interp["b0_x"]
					interp_b0_y = val_interp["b0_y"]
					interp_b1_x = val_interp["b1_x"]
					interp_b1_y = val_interp["b1_y"]
					interp_b2_x = val_interp["b2_x"]
					interp_b2_y = val_interp["b2_y"]
					interp_b3_x = val_interp["b3_x"]
					interp_b3_y = val_interp["b3_y"]
					# empty input file for string substitution, does not take frame count in consideration
					transformation_argts.extend(["--bezier_curve", format(interp_b0_x, '.8f')+"x"+ format(interp_b0_y, '.8f')+"x"+ format(interp_b1_x, '.8f')+"x"+ format(interp_b1_y, '.8f')+"x"+ \
						format(interp_b2_x, '.8f')+"x"+ format(interp_b2_y, '.8f')+"x"+ format(interp_b3_x, '.8f')+"x"+ format(interp_b3_y, '.8f')])
					
				######################################################
				# SPIRALS
				# passes Archimedean spiral arguments from the scenario file: center, radial expansion, angular step 
				# and number of steps
				if(("spiral_center_x" in val_interp) and ("spiral_center_y" in val_interp)
					 and ("spiral_radius_step" in val_interp) and ("spiral_angle_step" in val_interp)
					 and ("spiral_nb_steps" in val_interp)) :
					interp_spiral_center_x = val_interp["spiral_center_x"]
					interp_spiral_center_y = val_interp["spiral_center_y"]
					interp_spiral_radius_step = val_interp["spiral_radius_step"]
					interp_spiral_angle_step = val_interp["spiral_angle_step"]
					interp_spiral_nb_steps = val_interp["spiral_nb_steps"]
					# empty input file for string substitution, does not take frame count in consideration
					transformation_argts.extend(["--archimede_spiral", interp_spiral_center_x+"x"+ interp_spiral_center_y+"x"+ \
						 format(interp_spiral_radius_step, '.8f')+"x"+  format(interp_spiral_angle_step, '.8f')+"x"+  format(interp_spiral_nb_steps, '.8f')])
					
				######################################################
				# OBJECTS ALONG PATH
				if(("drawing_path_IDs" in val_interp) and ("objects_directory" in val_interp) \
					and ("objects_file" in val_interp) and ("objects_IDs" in val_interp) and ("scale_x_obj_layer1" in val_interp) and ("scale_y_obj_layer1" in val_interp)\
					 and ("scale_x_obj_layer2" in val_interp) and ("scale_y_obj_layer2" in val_interp) \
					 and ("interval_obj_layer1" in val_interp) and ("interval_obj_layer2" in val_interp)\
					 and ("growth_obj_layer1" in val_interp) and ("growth_obj_layer2" in val_interp)\
					 and ("rand_scale_x_obj_layer1" in val_interp) and ("rand_scale_x_obj_layer2" in val_interp) \
					 and ("rand_rotation_obj_layer1" in val_interp) and ("rand_rotation_obj_layer2" in val_interp) \
					 and ("sun_x" in val_interp) and ("sun_y" in val_interp) \
					 ) :
					drawing_path_IDs = val_interp["drawing_path_IDs"]
					objects_directory = val_interp["objects_directory"]
					objects_file = val_interp["objects_file"]
					objects_IDs = val_interp["objects_IDs"]
					scale_x_obj_layer1 = val_interp["scale_x_obj_layer1"]
					scale_y_obj_layer1 = val_interp["scale_y_obj_layer1"]
					scale_x_obj_layer2 = val_interp["scale_x_obj_layer2"]
					scale_y_obj_layer2 = val_interp["scale_y_obj_layer2"]
					interval_obj_layer1 = val_interp["interval_obj_layer1"]
					interval_obj_layer2 = val_interp["interval_obj_layer2"]
					growth_obj_layer1 = val_interp["growth_obj_layer1"]
					growth_obj_layer2 = val_interp["growth_obj_layer2"]

					rand_scale_x_obj_layer1 = val_interp["rand_scale_x_obj_layer1"]
					if(with_sound_data and "pulsed_rand_scale_x_obj_layer1" in val_interp) :
						pulsed_rand_scale_x_obj_layer1 = val_interp["pulsed_rand_scale_x_obj_layer1"]
						rand_scale_x_obj_layer1 = interpolate(var_types["rand_scale_x_obj_layer1"], rand_scale_x_obj_layer1, pulsed_rand_scale_x_obj_layer1, 'l', sound_volume)

					rand_scale_x_obj_layer2 = val_interp["rand_scale_x_obj_layer2"]
					if(with_sound_data and "pulsed_rand_scale_x_obj_layer2" in val_interp) :
						pulsed_rand_scale_x_obj_layer2 = val_interp["pulsed_rand_scale_x_obj_layer2"]
						rand_scale_x_obj_layer2 = interpolate(var_types["rand_scale_x_obj_layer2"], rand_scale_x_obj_layer2, pulsed_rand_scale_x_obj_layer2, 'l', sound_volume)

					rand_rotation_obj_layer1 = val_interp["rand_rotation_obj_layer1"]
					if(with_sound_data and "pulsed_rand_rotation_obj_layer1" in val_interp) :
						pulsed_rand_rotation_obj_layer1 = val_interp["pulsed_rand_rotation_obj_layer1"]
						rand_rotation_obj_layer1 = interpolate(var_types["rand_rotation_obj_layer1"], rand_rotation_obj_layer1, pulsed_rand_rotation_obj_layer1, 'l', sound_volume)

					rand_rotation_obj_layer2 = val_interp["rand_rotation_obj_layer2"]
					if(with_sound_data and "pulsed_rand_rotation_obj_layer2" in val_interp) :
						pulsed_rand_rotation_obj_layer2 = val_interp["pulsed_rand_rotation_obj_layer2"]
						rand_rotation_obj_layer2 = interpolate(var_types["rand_rotation_obj_layer2"], rand_rotation_obj_layer2, pulsed_rand_rotation_obj_layer2, 'l', sound_volume)

					sun_x = val_interp["sun_x"]
					sun_y = val_interp["sun_y"]
					drawing_path_IDs =  re.split(r'/', drawing_path_IDs.rstrip())
					csv_obj_data_storage = os.path.join(tmp_dir, "tmp_obj_paramteters_storage.csv")
					# print("first frame input", indFrameInput, val_init["imageInputIndex"])
					first_frame_input = indFrameInput == val_init["imageInputIndex"]
					transformation_argts.extend(["--drawing_path_IDs", drawing_path_IDs, "--csv_obj_data_storage", csv_obj_data_storage, \
												"--objects_directory", objects_directory, "--objects_file", objects_file, "--objects_IDs", objects_IDs, \
												"--first_frame_input", first_frame_input, \
												"--scale_x_obj_layer1", scale_x_obj_layer1, "--scale_y_obj_layer1", scale_y_obj_layer1, \
												"--scale_x_obj_layer2", scale_x_obj_layer2, "--scale_y_obj_layer2", scale_y_obj_layer2, \
												"--interval_obj_layer1", interval_obj_layer1, "--interval_obj_layer2", interval_obj_layer2, \
												"--growth_obj_layer1", growth_obj_layer1, "--growth_obj_layer2", growth_obj_layer2, \
												"--rand_scale_x_obj_layer1", rand_scale_x_obj_layer1, "--rand_scale_x_obj_layer2", rand_scale_x_obj_layer2, \
												"--rand_rotation_obj_layer1", rand_rotation_obj_layer1, "--rand_rotation_obj_layer2", rand_rotation_obj_layer2, \
												"--sun_x", sun_x, "--sun_y", sun_y, \
												])

				######################################################
				# SOUND
				if with_sound_data :
					transformation_argts.extend(["--sound_volume", sound_volume])

				######################################################
				# IMAGE GENERATION (FOR ISSKJUTNING) WITH POSSIBLE TARGET IMAGE SWITCH ACCORDING TO MUSIC
				# no additional arguments to the transformation
				# print ("vv_transform: ", transformation_function_string, transformation_argts)
				# print ("vv_transform/vv_fillAndStroke: ", full_svg_input_file_name, "->", full_svg_output_file_name, "(", transformation_function_string, ")")
				if transformation_function_string == "OBJECT_ALONG_PATH" or transformation_function_string == "SUNRISE" or transformation_function_string == "SPIKES" \
				   or transformation_function_string == "SECOND_ORDER_SPIKES" or transformation_function_string == "CRACKS" or transformation_function_string == "CRACKS_AND_NOISE" \
				   or transformation_function_string == "OBJECTS_NOISE_AND_SHRINK" :
					print ("vv_transform_obj_along_path: ", svg_input_image_file_name, "->", val_init["svg_output_file_name"]+"_"+countOutput+".svg (", transformation_function_string, ") " + "%.3f" % percent_transf + " % " + " vol: %.3f" % sound_volume)
					# print("arguments", transformation_argts)
					if( vv_transform_obj_along_path.main(transformation_argts) == 0) :
						return 0

				######################################################
				# IMAGE INTERPOLATION WITH POSSIBLE TARGET IMAGE SWITCH ACCORDING TO MUSIC
				elif transformation_function_string == "IMAGE_INTERPOLATION" or transformation_function_string == "IMAGE_INTERPOLATION_ONSET_SWITCH" or transformation_function_string == "IMAGE_INTERPOLATION_SOUND_SWITCH" or transformation_function_string == "IMAGE_INTERPOLATION_RANDOM_SWITCH" or transformation_function_string == "IMAGE_INTERPOLATION_PEAK_SWITCH" :

					# single image or video frames
					if(("target_image_directory" in val_interp) and (("target_image_file_name" in val_interp) or ("target_file_name" in val_interp))) :
						target_image_directory = val_interp["target_image_directory"]
						# single image with possible random selection inside a directory
						if ("target_image_file_name" in val_interp):
							target_image_file_name = val_interp["target_image_file_name"]
						# video frames
						else:
							target_image_file_name = val_interp["target_file_name"]+"_"+countInput+".svg"
					else :
						print("target_image_directory or target_image_file_name or target_file not among the provided variables")
						sys.exit(0)

					# genuine transformation
					if percent_transf > 0 :
						# single image with possible random selection inside a directory
						if ("target_image_file_name" in val_interp):
							######################################################
							# TARGET IMAGE FOR VIDEO/IMAGE INTERPOLATION
							# selection of the target image
							# random selection at the beginning
							# new random selection everytime there is an onset (INTERPOLATION_ONSET_SWITCH)
							#                      everytime the soiund reaches above a threshold (INTERPOLATION_SOUND_SWITCH)
							#                      everytime a random time interval has passed (INTERPOLATION_RANDOM_SWITCH)
							if with_sound_data and transformation_function_string == "IMAGE_INTERPOLATION_ONSET_SWITCH" :
								if new_onset or switched_target_file_name == None :
									switched_target_file_name = random_target_regular_expression_name(target_image_directory, target_image_file_name)
							elif with_sound_data and transformation_function_string == "IMAGE_INTERPOLATION_SOUND_SWITCH" :
								if envelope_sound_volume + smoothed_envelope_sound_volume >= 1 or switched_target_file_name == None :
									switched_target_file_name = random_target_regular_expression_name(target_image_directory, target_image_file_name)
							elif with_sound_data and transformation_function_string == "IMAGE_INTERPOLATION_RANDOM_SWITCH" :
								if indFrameInput > switched_target_next_indFrameInput  or switched_target_file_name == None :
									switched_target_file_name = random_target_regular_expression_name(target_image_directory, target_image_file_name)
									switched_target_next_indFrameInput = indFrameInput + int(random.random(10))
							elif with_sound_data and transformation_function_string == "IMAGE_INTERPOLATION_PEAK_SWITCH" :
								if peaked_envelope_sound_volume >= 1 or switched_target_file_name == None :
									switched_target_file_name = random_target_regular_expression_name(target_image_directory, target_image_file_name)
							else :
								switched_target_file_name = target_image_file_name + ".svg"
						# video frames
						else:
							switched_target_file_name = target_image_file_name

						if(("target_image_directory" in val_interp) and (("target_image_file_name" in val_interp) or ("target_file_name" in val_interp)) \
							 ) :
							transformation_argts.extend(["--target_file_name", os.path.join(target_image_directory, switched_target_file_name), \
													]) 
						print("vv_transform_interpolation: ", svg_input_image_file_name, " & ", switched_target_file_name, " ->", val_init["svg_output_file_name"]+"_"+countOutput+".svg (", transformation_function_string, ") " + "%.3f" % percent_transf + " % " + " vol: %.3f" % sound_volume)
						print(transformation_argts)
						if( vv_transform_interpolation.main(transformation_argts) == 0) :
							return 0
					# 0% transformation: only fills
					else :
						print("vv_fill: ", svg_input_image_file_name, " & ", target_image_file_name, ".svg -> ", val_init["svg_output_file_name"]+"_"+countOutput+".svg (", transformation_function_string, ") " + "%.3f" % percent_transf + " % " + " vol: %.3f" % sound_volume)
						if( vv_transform_interpolation.main(transformation_argts) == 0) :
							return 0

				######################################################
				# TRANSFORMATION DEFINED INSIDE vv_transform.py
				elif transformation_function_string != "COPY" :
					# print ("vv_transform: ", svg_input_image_file_name, "->", val_init["svg_output_file_name"]+"_"+countOutput+".svg (", transformation_function_string, ")" + "%.3f" % percent_transf + "%")
					if( vv_transform.main(transformation_argts) == 0) :
						return 

				######################################################
				# IMAGE COPY (NO TRANSFORMATION)
				else : #COPY
					print ("cp: ", svg_input_image_file_name, "->", val_init["svg_output_file_name"]+"_"+countOutput+".svg (", transformation_function_string, ")")
					os.system("cp -f " + full_svg_input_file_name + " " + os.path.join(tmp_dir, "tmp_transformed.svg"))
					
			
				##############################################################################################
				##############################################################################################
				# POSSIBLE COLOR MODIFICATION (CAN ALSO BE ADDED LATER THROUGH vv_scenario_fillAndStroke.py
				##############################################################################################
				##############################################################################################
				
				if with_fill_and_stroke :
					fillAndStroke_argts = ["--nb-layers", nb_layers, \
						"-o", full_svg_output_file_name, \
						"-i", os.path.join(tmp_dir, "tmp_transformed.svg")]

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
					fillAndStroke_argts.extend(["--fill_color_bg", interp_fill_color_bg])
					if("fill_color_l1" in val_interp) :
						interp_fill_color_l1 = val_interp["fill_color_l1"]
					fillAndStroke_argts.extend(["--fill_color_l1", interp_fill_color_l1])
					if("fill_color_l2" in val_interp) :
						interp_fill_color_l2 = val_interp["fill_color_l2"]
					fillAndStroke_argts.extend(["--fill_color_l2", interp_fill_color_l2])
					if("fill_color_l3" in val_interp) :
						interp_fill_color_l3 = val_interp["fill_color_l3"]
					fillAndStroke_argts.extend(["--fill_color_l3", interp_fill_color_l3])
					if("fill_color_l4" in val_interp) :
						interp_fill_color_l4 = val_interp["fill_color_l4"]
					fillAndStroke_argts.extend(["--fill_color_l4", interp_fill_color_l4])
					if("fill_color_l5" in val_interp) :
						interp_fill_color_l5 = val_interp["fill_color_l5"]
					fillAndStroke_argts.extend(["--fill_color_l5", interp_fill_color_l5])
					if("fill_color_l6" in val_interp) :
						interp_fill_color_l6 = val_interp["fill_color_l6"]
					fillAndStroke_argts.extend(["--fill_color_l6", interp_fill_color_l6])
					if("fill_color_l7" in val_interp) :
						interp_fill_color_l7 = val_interp["fill_color_l7"]
					fillAndStroke_argts.extend(["--fill_color_l7", interp_fill_color_l7])
					if("fill_color_l8" in val_interp) :
						interp_fill_color_l8 = val_interp["fill_color_l8"]
					fillAndStroke_argts.extend(["--fill_color_l8", interp_fill_color_l8])
					if("fill_opacity_l1" in val_interp) :
						interp_fill_opacity_l1 = format(val_interp["fill_opacity_l1"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l1", interp_fill_opacity_l1])
					if("fill_opacity_l2" in val_interp) :
						interp_fill_opacity_l2 = format(val_interp["fill_opacity_l2"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l2", interp_fill_opacity_l2])
					if("fill_opacity_l3" in val_interp) :
						interp_fill_opacity_l3 = format(val_interp["fill_opacity_l3"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l3", interp_fill_opacity_l3])
					if("fill_opacity_l4" in val_interp) :
						interp_fill_opacity_l4 = format(val_interp["fill_opacity_l4"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l4", interp_fill_opacity_l4])
					if("fill_opacity_l5" in val_interp) :
						interp_fill_opacity_l5 = format(val_interp["fill_opacity_l5"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l5", interp_fill_opacity_l5])
					if("fill_opacity_l6" in val_interp) :
						interp_fill_opacity_l6 = format(val_interp["fill_opacity_l6"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l6", interp_fill_opacity_l6])
					if("fill_opacity_l7" in val_interp) :
						interp_fill_opacity_l7 = format(val_interp["fill_opacity_l7"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l7", interp_fill_opacity_l7])
					if("fill_opacity_l8" in val_interp) :
						interp_fill_opacity_l8 = format(val_interp["fill_opacity_l8"], '.8f')
					fillAndStroke_argts.extend(["--fill_opacity_l8", interp_fill_opacity_l8])
					
					# STROKES COLOR, WIDTH, AND OPACITY
					interp_stroke_color_bg = '#000000'
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
						fillAndStroke_argts.extend(["--stroke_color_l1", interp_stroke_color_l1])
					if("stroke_color_l2" in val_interp) :
						interp_stroke_color_l2 = val_interp["stroke_color_l2"]
						fillAndStroke_argts.extend(["--stroke_color_l2", interp_stroke_color_l2])
					if("stroke_color_l3" in val_interp) :
						interp_stroke_color_l3 = val_interp["stroke_color_l3"]
						fillAndStroke_argts.extend(["--stroke_color_l3", interp_stroke_color_l3])
					if("stroke_color_l4" in val_interp) :
						interp_stroke_color_l4 = val_interp["stroke_color_l4"]
						fillAndStroke_argts.extend(["--stroke_color_l4", interp_stroke_color_l4])
					if("stroke_color_l5" in val_interp) :
						interp_stroke_color_l5 = val_interp["stroke_color_l5"]
						fillAndStroke_argts.extend(["--stroke_color_l5", interp_stroke_color_l5])
					if("stroke_color_l6" in val_interp) :
						interp_stroke_color_l6 = val_interp["stroke_color_l6"]
						fillAndStroke_argts.extend(["--stroke_color_l6", interp_stroke_color_l6])
					if("stroke_color_l7" in val_interp) :
						interp_stroke_color_l7 = val_interp["stroke_color_l7"]
						fillAndStroke_argts.extend(["--stroke_color_l7", interp_stroke_color_l7])
					if("stroke_color_l8" in val_interp) :
						interp_stroke_color_l8 = val_interp["stroke_color_l8"]
						fillAndStroke_argts.extend(["--stroke_color_l8", interp_stroke_color_l8])
					if("stroke_opacity_l1" in val_interp) :
						interp_stroke_opacity_l1 = format(val_interp["stroke_opacity_l1"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l1", interp_stroke_opacity_l1])
					if("stroke_opacity_l2" in val_interp) :
						interp_stroke_opacity_l2 = format(val_interp["stroke_opacity_l2"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l2", interp_stroke_opacity_l2])
					if("stroke_opacity_l3" in val_interp) :
						interp_stroke_opacity_l3 = format(val_interp["stroke_opacity_l3"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l3", interp_stroke_opacity_l3])
					if("stroke_opacity_l4" in val_interp) :
						interp_stroke_opacity_l4 = format(val_interp["stroke_opacity_l4"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l4", interp_stroke_opacity_l4])
					if("stroke_opacity_l5" in val_interp) :
						interp_stroke_opacity_l5 = format(val_interp["stroke_opacity_l5"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l5", interp_stroke_opacity_l5])
					if("stroke_opacity_l6" in val_interp) :
						interp_stroke_opacity_l6 = format(val_interp["stroke_opacity_l6"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l6", interp_stroke_opacity_l6])
					if("stroke_opacity_l7" in val_interp) :
						interp_stroke_opacity_l7 = format(val_interp["stroke_opacity_l7"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l7", interp_stroke_opacity_l7])
					if("stroke_opacity_l8" in val_interp) :
						interp_stroke_opacity_l8 = format(val_interp["stroke_opacity_l8"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity_l8", interp_stroke_opacity_l8])
					if("stroke_width_l1" in val_interp) :
						interp_stroke_width_l1 = format(val_interp["stroke_width_l1"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l1", interp_stroke_width_l1])
					if("stroke_width_l2" in val_interp) :
						interp_stroke_width_l2 = format(val_interp["stroke_width_l2"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l2", interp_stroke_width_l2])
					if("stroke_width_l3" in val_interp) :
						interp_stroke_width_l3 = format(val_interp["stroke_width_l3"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l3", interp_stroke_width_l3])
					if("stroke_width_l4" in val_interp) :
						interp_stroke_width_l4 = format(val_interp["stroke_width_l4"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l4", interp_stroke_width_l4])
					if("stroke_width_l5" in val_interp) :
						interp_stroke_width_l5 = format(val_interp["stroke_width_l5"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l5", interp_stroke_width_l5])
					if("stroke_width_l6" in val_interp) :
						interp_stroke_width_l6 = format(val_interp["stroke_width_l6"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l6", interp_stroke_width_l6])
					if("stroke_width_l7" in val_interp) :
						interp_stroke_width_l7 = format(val_interp["stroke_width_l7"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l7", interp_stroke_width_l7])
					if("stroke_width_l8" in val_interp) :
						interp_stroke_width_l8 = format(val_interp["stroke_width_l8"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width_l8", interp_stroke_width_l8])

					if("stroke_color" in val_interp) :
						interp_stroke_color = val_interp["stroke_color"]
						fillAndStroke_argts.extend(["--stroke_color", interp_stroke_color])
					if("stroke_opacity" in val_interp) :
						interp_stroke_opacity = format(val_interp["stroke_opacity"], '.8f')
						fillAndStroke_argts.extend(["--stroke_opacity", interp_stroke_opacity])
					if("stroke_width" in val_interp) :
						interp_stroke_width = format(val_interp["stroke_width"], '.8f')
						fillAndStroke_argts.extend(["--stroke_width", interp_stroke_width])

					# image temperature, saturation or brightness used to balance the colors of the layers
					temperature = "6600"
					if("temperature" in val_interp) :
						temperature = format(val_interp["temperature"], '.8f')
						fillAndStroke_argts.extend(["--temperature", temperature])
					if("saturation" in val_interp) :
						saturation = format(val_interp["saturation"], '.8f')
						fillAndStroke_argts.extend(["--saturation", saturation])
					if("brightness" in val_interp) :
						brightness = format(val_interp["brightness"], '.8f')
						fillAndStroke_argts.extend(["--brightness", brightness])

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
				
				else :
					os.system("cp "+os.path.join(tmp_dir, "tmp_transformed.svg")+" "+full_svg_output_file_name)

			# moves forward on output frames
			indFrameOutput += 1

			# removes input frame
			# print ("rm: ", svg_input_image_file_name)
			# os.system("rm -f " + full_svg_input_file_name)

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
