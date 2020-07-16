#!/usr/bin/python3 

# vv_scenario_transform.py
# scenario for transformations of vector video frames (both initial transformation before crossfading and final transformations after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import vv_transform
import vv_fillAndStroke
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import interpolate

import csv
import getopt
import sys
from signal import signal, SIGINT

import numpy as np
import cv2 as cv

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

		##################################################################
		# SCANS ALL THE FRAMES IN THE MOVIE
		##################################################################
		indFrameOutput = val_init["imageOutputIndex"]
		nb_layers = val_init["nb_posterization_layers"]
		tmp_dir = val_init["tmp_directory"]
		for indFrameInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + pas, pas):
			countInput = "%04d" % indFrameInput
			countOutput = "%04d" % indFrameOutput

			# scene percentage at this frame 
			scene_percentage = 0
			if pas != 0 and (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) != 0 :
				scene_percentage = (indFrameInput - val_init["imageInputIndex"])/(val_fin["imageInputIndex"] - val_init["imageInputIndex"])
			#clamping
			scene_percentage = clamp(scene_percentage)

			# interpolating all the variables
			for var_name in var_names :
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])
			
			if(trace_only) :
				print("Ind image %d Scene percentage %.2f percent_transf1 %.2f fill_opacity_l1 %.2f fill_color_l4 %s fill_color_bg %s\n" % \
					indFrameInput, scene_percentage, val_interp["percent_transf1"], val_interp["fill_opacity_l1"], val_interp["fill_color_l4"], val_interp["fill_color_bg"])
	
			# percentage of transformation
			percent_transf = val_interp["percent_transf1"]
			#clamping
			percent_transf = clamp(percent_transf)

			# SVG input file name
			full_svg_input_file_name = val_init["svg_input_directory"] + val_init["svg_input_file_name"]
			full_svg_input_file_name = full_svg_input_file_name+"_"+countInput+".svg"
			# SVG output file name
			full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"]
			full_svg_output_file_name = full_svg_output_file_name+"_"+countOutput+".svg"

			# transformation ID
			transformation_function_string = val_init["transf1_ID"]

			if(not trace_only) :
				# print( "Transformed SVG file percentage ", percent_transf)
				transformation_argts = ["--nb_layers", nb_layers, \
						"-o", tmp_dir+"tmp_transformed.svg", \
						"-i", full_svg_input_file_name, \
						"--transformation", transformation_function_string, \
						"--chapter_no", scenario_chapter_no, \
						"--percent_transf", percent_transf ]

				######################################################
				# DISPLACEMENT MAPS
				# displacement map file name
				if(("displacement_map_directory" in val_init)  and ("displacement_map_file_name" in val_init) \
					and ("displacement_map_inputIndex" in val_interp)) :
					# displacement map file name
					displacement_map_file_name = val_init["displacement_map_directory"] + val_init["displacement_map_file_name"]
					displacement_map_count = "%04d" % int(val_interp["displacement_map_inputIndex"])
					displacement_map_1_file_name = str(displacement_map_file_name)+"_1_"+displacement_map_count+".exr"
					displacement_map_2_file_name = str(displacement_map_file_name)+"_2_"+displacement_map_count+".exr"
					transformation_argts.extend(["--displacement_map", str(displacement_map_1_file_name)+"&"+str(displacement_map_2_file_name)])


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
					interp_x_translation = val_interp["x_translation"]
					interp_y_translation = val_interp["y_translation"]
					transformation_argts.extend(["--translation", str(interp_x_translation)+"x"+str(interp_y_translation)])

				######################################################
				# ROTATION
				# passes rotation arguments from the scenario file: translation vector
				if(("x_rotation_center" in val_interp) and ("y_rotation_center" in val_interp) and ("rotation_angle" in val_interp)) :
					interp_x_rotation_center = val_interp["x_rotation_center"]
					interp_y_rotation_center = val_interp["y_rotation_center"]
					interp_rotation_angle = val_interp["rotation_angle"]
					transformation_argts.extend(["--rotation", str(interp_x_rotation_center)+"x"+ str(interp_y_rotation_center)+"x"+ str(interp_rotation_angle)])
				
				######################################################
				# SCALING
				# passes scaling arguments from the scenario file: translation vector
				if(("x_scaling_center" in val_interp) and ("y_scaling_center" in val_interp) and ("scaling_factor" in val_interp)) :
					interp_x_scaling_center = val_interp["x_scaling_center"]
					interp_y_scaling_center = val_interp["y_scaling_center"]
					interp_scaling_factor = val_interp["scaling_factor"]
					transformation_argts.extend(["--scale", str(interp_x_scaling_center)+"x"+ str(interp_y_scaling_center)+"x"+ str(interp_scaling_factor)])
					
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
					transformation_argts.extend(["--bezier_curve", str(interp_b0_x)+"x"+ str(interp_b0_y)+"x"+ str(interp_b1_x)+"x"+ str(interp_b1_y)+"x"+ \
						str(interp_b2_x)+"x"+ str(interp_b2_y)+"x"+ str(interp_b3_x)+"x"+ str(interp_b3_y)])
					
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
						 str(interp_spiral_radius_step)+"x"+  str(interp_spiral_angle_step)+"x"+  str(interp_spiral_nb_steps)])
					
				# no additional arguments to the transformation
				print ("vv_transform: ", transformation_argts)
				if( vv_transform.main(transformation_argts) == 0) :
					return 0
					
			if(not trace_only) :
				fillAndStroke_argts = ["--nb-layers", nb_layers, \
					"-o", full_svg_output_file_name, \
					"-i", tmp_dir+"tmp_transformed.svg"]

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
					interp_fill_opacity_l1 = str(val_interp["fill_opacity_l1"])
				fillAndStroke_argts.extend(["--fill_opacity_l1", interp_fill_opacity_l1])
				if("fill_opacity_l2" in val_interp) :
					interp_fill_opacity_l2 = str(val_interp["fill_opacity_l2"])
				fillAndStroke_argts.extend(["--fill_opacity_l2", interp_fill_opacity_l2])
				if("fill_opacity_l3" in val_interp) :
					interp_fill_opacity_l3 = str(val_interp["fill_opacity_l3"])
				fillAndStroke_argts.extend(["--fill_opacity_l3", interp_fill_opacity_l3])
				if("fill_opacity_l4" in val_interp) :
					interp_fill_opacity_l4 = str(val_interp["fill_opacity_l4"])
				fillAndStroke_argts.extend(["--fill_opacity_l4", interp_fill_opacity_l4])
				if("fill_opacity_l5" in val_interp) :
					interp_fill_opacity_l5 = str(val_interp["fill_opacity_l5"])
				fillAndStroke_argts.extend(["--fill_opacity_l5", interp_fill_opacity_l5])
				if("fill_opacity_l6" in val_interp) :
					interp_fill_opacity_l6 = str(val_interp["fill_opacity_l6"])
				fillAndStroke_argts.extend(["--fill_opacity_l6", interp_fill_opacity_l6])
				if("fill_opacity_l7" in val_interp) :
					interp_fill_opacity_l7 = str(val_interp["fill_opacity_l7"])
				fillAndStroke_argts.extend(["--fill_opacity_l7", interp_fill_opacity_l7])
				if("fill_opacity_l8" in val_interp) :
					interp_fill_opacity_l8 = str(val_interp["fill_opacity_l8"])
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
					interp_stroke_opacity_l1 = str(val_interp["stroke_opacity_l1"])
					fillAndStroke_argts.extend(["--stroke_opacity_l1", interp_stroke_opacity_l1])
				if("stroke_opacity_l2" in val_interp) :
					interp_stroke_opacity_l2 = str(val_interp["stroke_opacity_l2"])
					fillAndStroke_argts.extend(["--stroke_opacity_l2", interp_stroke_opacity_l2])
				if("stroke_opacity_l3" in val_interp) :
					interp_stroke_opacity_l3 = str(val_interp["stroke_opacity_l3"])
					fillAndStroke_argts.extend(["--stroke_opacity_l3", interp_stroke_opacity_l3])
				if("stroke_opacity_l4" in val_interp) :
					interp_stroke_opacity_l4 = str(val_interp["stroke_opacity_l4"])
					fillAndStroke_argts.extend(["--stroke_opacity_l4", interp_stroke_opacity_l4])
				if("stroke_opacity_l5" in val_interp) :
					interp_stroke_opacity_l5 = str(val_interp["stroke_opacity_l5"])
					fillAndStroke_argts.extend(["--stroke_opacity_l5", interp_stroke_opacity_l5])
				if("stroke_opacity_l6" in val_interp) :
					interp_stroke_opacity_l6 = str(val_interp["stroke_opacity_l6"])
					fillAndStroke_argts.extend(["--stroke_opacity_l6", interp_stroke_opacity_l6])
				if("stroke_opacity_l7" in val_interp) :
					interp_stroke_opacity_l7 = str(val_interp["stroke_opacity_l7"])
					fillAndStroke_argts.extend(["--stroke_opacity_l7", interp_stroke_opacity_l7])
				if("stroke_opacity_l8" in val_interp) :
					interp_stroke_opacity_l8 = str(val_interp["stroke_opacity_l8"])
					fillAndStroke_argts.extend(["--stroke_opacity_l8", interp_stroke_opacity_l8])
				if("stroke_width_l1" in val_interp) :
					interp_stroke_width_l1 = str(val_interp["stroke_width_l1"])
					fillAndStroke_argts.extend(["--stroke_width_l1", interp_stroke_width_l1])
				if("stroke_width_l2" in val_interp) :
					interp_stroke_width_l2 = str(val_interp["stroke_width_l2"])
					fillAndStroke_argts.extend(["--stroke_width_l2", interp_stroke_width_l2])
				if("stroke_width_l3" in val_interp) :
					interp_stroke_width_l3 = str(val_interp["stroke_width_l3"])
					fillAndStroke_argts.extend(["--stroke_width_l3", interp_stroke_width_l3])
				if("stroke_width_l4" in val_interp) :
					interp_stroke_width_l4 = str(val_interp["stroke_width_l4"])
					fillAndStroke_argts.extend(["--stroke_width_l4", interp_stroke_width_l4])
				if("stroke_width_l5" in val_interp) :
					interp_stroke_width_l5 = str(val_interp["stroke_width_l5"])
					fillAndStroke_argts.extend(["--stroke_width_l5", interp_stroke_width_l5])
				if("stroke_width_l6" in val_interp) :
					interp_stroke_width_l6 = str(val_interp["stroke_width_l6"])
					fillAndStroke_argts.extend(["--stroke_width_l6", interp_stroke_width_l6])
				if("stroke_width_l7" in val_interp) :
					interp_stroke_width_l7 = str(val_interp["stroke_width_l7"])
					fillAndStroke_argts.extend(["--stroke_width_l7", interp_stroke_width_l7])
				if("stroke_width_l8" in val_interp) :
					interp_stroke_width_l8 = str(val_interp["stroke_width_l8"])
					fillAndStroke_argts.extend(["--stroke_width_l8", interp_stroke_width_l8])

				if("stroke_color" in val_interp) :
					interp_stroke_color = val_interp["stroke_color"]
					fillAndStroke_argts.extend(["--stroke_color", interp_stroke_color])
				if("stroke_opacity" in val_interp) :
					interp_stroke_opacity = str(val_interp["stroke_opacity"])
					fillAndStroke_argts.extend(["--stroke_opacity", interp_stroke_opacity])
				if("stroke_width" in val_interp) :
					interp_stroke_width = str(val_interp["stroke_width"])
					fillAndStroke_argts.extend(["--stroke_width", interp_stroke_width])

				# image temperature used to balance the colors of the layers
				temperature = "6600"
				if("temperature" in val_interp) :
					temperature = str(val_interp["temperature"])
					fillAndStroke_argts.extend(["--temperature", temperature])

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
					interp_blur_l1 = str(val_interp["blur_l1"])
				fillAndStroke_argts.extend(["--blur_l1", interp_blur_l1])
				if("blur_l2" in val_interp) :
					interp_blur_l2 = str(val_interp["blur_l2"])
				fillAndStroke_argts.extend(["--blur_l2", interp_blur_l2])
				if("blur_l3" in val_interp) :
					interp_blur_l3 = str(val_interp["blur_l3"])
				fillAndStroke_argts.extend(["--blur_l3", interp_blur_l3])
				if("blur_l4" in val_interp) :
					interp_blur_l4 = str(val_interp["blur_l4"])
				fillAndStroke_argts.extend(["--blur_l4", interp_blur_l4])
				if("blur_l5" in val_interp) :
					interp_blur_l5 = str(val_interp["blur_l5"])
				fillAndStroke_argts.extend(["--blur_l5", interp_blur_l5])
				if("blur_l6" in val_interp) :
					interp_blur_l6 = str(val_interp["blur_l6"])
				fillAndStroke_argts.extend(["--blur_l6", interp_blur_l6])
				if("blur_l7" in val_interp) :
					interp_blur_l7 = str(val_interp["blur_l7"])
				fillAndStroke_argts.extend(["--blur_l7", interp_blur_l7])
				if("blur_l8" in val_interp) :
					interp_blur_l8 = str(val_interp["blur_l8"])
				fillAndStroke_argts.extend(["--blur_l8", interp_blur_l8])

				# print("vv_fillAndStroke: ", fillAndStroke_argts)
				if( vv_fillAndStroke.main(fillAndStroke_argts) == 0) :
					return 0
			
			else :
				os.system("cp "+tmp_dir+"tmp_transformed.svg "+full_svg_output_file_name)

			# moves forward on output frames
			indFrameOutput += 1

			# only one frame if frame ini and frame end are equal
			if (pas == 0):
				break

		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
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
