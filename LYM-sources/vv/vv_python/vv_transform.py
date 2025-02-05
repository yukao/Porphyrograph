#!/usr/bin/python3 

# vv_transform.py
# transformations of vector video frames (both initial transformation before crossfading and final transformations after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import math
import random

import getopt
import sys
from signal import signal, SIGINT

import re
import operator

from vv_lib import force_num
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import bilinear_interpolation_pixel
from vv_lib import relative_to_absolute_coordinates
from vv_lib import min_abs_X
from vv_lib import min_abs_Y
from vv_lib import max_abs_X
from vv_lib import max_abs_Y
from vv_lib import absolute_current_point_X
from vv_lib import absolute_current_point_Y
from vv_lib import subpath_initial_point_X
from vv_lib import subpath_initial_point_Y
from vv_lib import new_path


from math import sqrt

from vv_const import *

import numpy as np
import cv2 as cv
import os

import csv

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_transform.py --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
vv_transform.py --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation
ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --transformation: transformation function
   --percent: percentage of transformation
optional
   --translation: x, y coordinates of translation
'''

##################################################################
# GLOBAL VARIABLES
##################################################################
blur_filters = ['blur2' , 'blur5' , 'blur10' , 'blur20' , 'blur30']

chapter_no = 0
percent_transf = dict()
percent_links = 0
stroke_color_links = '#000000'
transformed_SVG_file_line_out = []
number_of_paths = 0
nb_layers = 1
path_rank = 0

# data dedicated to LINKS transformation: the production of links between images
# on different movies that have been crossfaded into a single multilayer image
# list of curve endpoints used for the first set of link (with preceding layer)
LINKS_first_pass_halfpoints = []

# displacement maps in x and y
displacement_map_1 = None
displacement_map_2 = None
displacement_map_center_translation = [0,0]

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	single_transf_strings = []
	single_transfs = []

	COPY = 1
	V_SCALE = 2
	V_SCALE2 = 3
	V_SCALE3 = 4
	C_SCALE = 5
	C_SCALE_BY_LAYERS = 6
	VH_RANDOM = 7
	V_FLATTEN = 8
	SMOOTHING = 9
	BLUR = 10
	REFRAME_TOP_LEFT = 11
	COPY_TOP_LEFT = 12
	C_LINES = 13
	TRANSLATE = 14
	ROTATE = 15
	SCALE = 16
	H_SYMMETRY = 17
	V_SYMMETRY = 18
	BEZIER_CURVE = 19
	ARCHIMEDE_SPIRAL = 20
	C_POLYGONS = 21
	LINKS = 22
	DISPLACEMENT_MAP = 23
	SCALE_TO_CIRCLE = 24
	V_SCALE4 = 25

	single_transf_hash = {\
		"COPY" : COPY,\
		"V_SCALE" : V_SCALE,\
		"V_SCALE2" : V_SCALE2,\
		"V_SCALE3" : V_SCALE3,\
		"V_SCALE4" : V_SCALE4,\
		"C_SCALE" : C_SCALE,\
		"C_SCALE_BY_LAYERS" : C_SCALE_BY_LAYERS,\
		"VH_RANDOM" : VH_RANDOM,\
		"V_FLATTEN" : V_FLATTEN,\
		"SMOOTHING" : SMOOTHING,\
		"BLUR" : BLUR,\
		"REFRAME_TOP_LEFT" : REFRAME_TOP_LEFT,\
		"COPY_TOP_LEFT" : COPY_TOP_LEFT,\
		"C_LINES" : C_LINES,\
		"TRANSLATE" : TRANSLATE,\
		"ROTATE" : ROTATE,\
		"SCALE" : SCALE,\
		"SCALE_TO_CIRCLE" : SCALE_TO_CIRCLE,\
		"H_SYMMETRY" : H_SYMMETRY,\
		"V_SYMMETRY" : V_SYMMETRY,\
		"BEZIER_CURVE" : BEZIER_CURVE,\
		"ARCHIMEDE_SPIRAL" : ARCHIMEDE_SPIRAL,\
		"C_POLYGONS" : C_POLYGONS, \
		"LINKS" : LINKS, \
		"DISPLACEMENT_MAP" : DISPLACEMENT_MAP, \
		 }

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################
	global chapter_no
	global percent_transf
	global percent_links
	global stroke_color_links
	global number_of_paths
	global transformed_SVG_file_line_out
	global displacement_map_1
	global displacement_map_2
	global displacement_map_center_translation
	global nb_layers

	scenario_file_name = ''
	prec_path_line_rank = -1
	x_absolute_translation = dict()
	y_absolute_translation = dict()
	x_absolute_rotation_center = dict()
	y_absolute_rotation_center = dict()
	rotation_angle = dict()
	x_absolute_scaling_center = dict()
	y_absolute_scaling_center = dict()
	scaling_factor = dict()
	b0_x = 0
	b0_y = 0
	b1_x = 0
	b1_y = 0
	b2_x = 0
	b2_y = 0
	b3_x = 0
	b3_y = 0
	spiral_center_x = 0
	spiral_center_y = 0
	spiral_radius_step = 0
	spiral_angle_step = 0
	spiral_nb_steps = 0
	displacement_map_1 = ""
	displacement_map_2 = ""
	displacement_map_factor = 1.0
	displacement_map_center_file_name = ""
	displacement_map_center_translation = [0,0]

	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb_layers=","chapter_no=",\
			"transformation=","percent_links=","stroke_color_links=","nb_frames=","percent_life=",\
			"percent_transf=","translation=","rotation=","scale=",\
			"percent_transf1=","translation1=","rotation1=","scale1=",\
			"percent_transf2=","translation2=","rotation2=","scale2=",\
			"percent_transf3=","translation3=","rotation3=","scale3=",\
			"percent_transf4=","translation4=","rotation4=","scale4=",\
			"percent_transf5=","translation5=","rotation5=","scale5=",\
			"bezier_curve=","archimede_spiral=",\
			"displacement_map=","displacement_map_factor=","displacement_map_center="])
	except getopt.GetoptError:
		print("incorrect option in command ", main_args)
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt == "--nb_layers":
			nb_layers = force_num(arg)
		elif opt == "--chapter_no":
			chapter_no = force_num(arg)
		#  SVG path transformation
		elif opt == "--transformation":
			transf_string = arg
			single_transf_strings = re.split(r'/', transf_string.rstrip())
			for single_transf_string in single_transf_strings :
				if not(single_transf_string in single_transf_hash) :
					print( main_args )
					print( USAGE )
					print( single_transf_string," does not exist in the single_transf_hash dictionary")
					sys.exit(2)
				single_transfs.append(single_transf_hash[single_transf_string])
		#  SVG path transformation percentage
		elif opt in ["--percent_transf1", "--percent_transf"]:
			percent_transf[1] = max(min(force_num(arg),1),0)
		elif opt == "--percent_transf2":
			percent_transf[2] = max(min(force_num(arg),1),0)
		elif opt == "--percent_transf3":
			percent_transf[3] = max(min(force_num(arg),1),0)
		elif opt == "--percent_transf4":
			percent_transf[4] = max(min(force_num(arg),1),0)
		elif opt == "--percent_transf5":
			percent_transf[5] = max(min(force_num(arg),1),0)
		#  SVG path links percentage
		elif opt == "--percent_links":
			percent_links = force_num(arg)
			percent_links = max(min(percent_links,1),0)
		#  SVG path links percentage
		elif opt == "--stroke_color_links":
			stroke_color_links = arg
		#  SVG path transformation life time in frames
		elif opt == "--nb_frames":
			nb_frames_life = int(force_num(arg))
		#  SVG path transformation remaining frames of life until the end of the scene
		elif opt == "--percent_life":
			percent_life = int(force_num(arg))
		# SVG path translation
		elif opt in ["--translation1","--translation"] and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation[1] = force_num(abs_trans[0])
			y_absolute_translation[1] = force_num(abs_trans[1])
		elif opt in ("--translation2") and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation[2] = force_num(abs_trans[0])
			y_absolute_translation[2] = force_num(abs_trans[1])
		elif opt in ("--translation3") and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation[3] = force_num(abs_trans[0])
			y_absolute_translation[3] = force_num(abs_trans[1])
		elif opt in ("--translation4") and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation[4] = force_num(abs_trans[0])
			y_absolute_translation[4] = force_num(abs_trans[1])
		elif opt in ("--translation5") and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation[5] = force_num(abs_trans[0])
			y_absolute_translation[5] = force_num(abs_trans[1])
		# SVG path rotation
		elif opt in ["--rotation1", "--rotation"] and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center[1] = force_num(abs_rot[0])
			y_absolute_rotation_center[1] = force_num(abs_rot[1])
			rotation_angle[1] = force_num(abs_rot[2])
		elif opt in ("--rotation2") and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center[2] = force_num(abs_rot[0])
			y_absolute_rotation_center[2] = force_num(abs_rot[1])
			rotation_angle[2] = force_num(abs_rot[2])
		elif opt in ("--rotation3") and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center[3] = force_num(abs_rot[0])
			y_absolute_rotation_center[3] = force_num(abs_rot[1])
			rotation_angle[3] = force_num(abs_rot[2])
		elif opt in ("--rotation4") and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center[4] = force_num(abs_rot[0])
			y_absolute_rotation_center[4] = force_num(abs_rot[1])
			rotation_angle[4] = force_num(abs_rot[2])
		elif opt in ("--rotation5") and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center[5] = force_num(abs_rot[0])
			y_absolute_rotation_center[5] = force_num(abs_rot[1])
			rotation_angle[5] = force_num(abs_rot[2])
		# SVG path scaling
		elif opt in ["--scale1", "--scale"] and  (SCALE in single_transfs or C_SCALE in single_transfs or C_SCALE_BY_LAYERS in single_transfs or SCALE_TO_CIRCLE in single_transfs):
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center[1] = force_num(abs_scale[0])
			y_absolute_scaling_center[1] = force_num(abs_scale[1])
			scaling_factor[1] = force_num(abs_scale[2])
		elif opt in ("--scale2") and  (SCALE in single_transfs or C_SCALE in single_transfs or C_SCALE_BY_LAYERS in single_transfs or SCALE_TO_CIRCLE in single_transfs):
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center[2] = force_num(abs_scale[0])
			y_absolute_scaling_center[2] = force_num(abs_scale[1])
			scaling_factor[2] = force_num(abs_scale[2])
		elif opt in ("--scale3") and  (SCALE in single_transfs or C_SCALE in single_transfs or C_SCALE_BY_LAYERS in single_transfs or SCALE_TO_CIRCLE in single_transfs):
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center[3] = force_num(abs_scale[0])
			y_absolute_scaling_center[3] = force_num(abs_scale[1])
			scaling_factor[3] = force_num(abs_scale[2])
		elif opt in ("--scale4") and  (SCALE in single_transfs or C_SCALE in single_transfs or C_SCALE_BY_LAYERS in single_transfs or SCALE_TO_CIRCLE in single_transfs):
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center[4] = force_num(abs_scale[0])
			y_absolute_scaling_center[4] = force_num(abs_scale[1])
			scaling_factor[4] = force_num(abs_scale[2])
		elif opt in ("--scale5") and  (SCALE in single_transfs or C_SCALE in single_transfs or C_SCALE_BY_LAYERS in single_transfs or SCALE_TO_CIRCLE in single_transfs):
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center[5] = force_num(abs_scale[0])
			y_absolute_scaling_center[5] = force_num(abs_scale[1])
			scaling_factor[5] = force_num(abs_scale[2])
		# SVG path control points
		elif opt in ("--bezier_curve") and  BEZIER_CURVE in single_transfs:
			abs_bez = re.split(r'x', arg.rstrip())
			b0_x = force_num(abs_bez[0])
			b0_y = force_num(abs_bez[1])
			b1_x = force_num(abs_bez[2])
			b1_y = force_num(abs_bez[3])
			b2_x = force_num(abs_bez[4])
			b2_y = force_num(abs_bez[5])
			b3_x = force_num(abs_bez[6])
			b3_y = force_num(abs_bez[7])
		# spiral parameters
		elif opt in ("--archimede_spiral") and  ARCHIMEDE_SPIRAL in single_transfs:
			abs_spir = re.split(r'x', arg.rstrip())
			spiral_center_x = force_num(abs_spir[0])
			spiral_center_y = force_num(abs_spir[1])
			spiral_radius_step = force_num(abs_spir[2])
			spiral_angle_step = force_num(abs_spir[3])
			spiral_nb_steps = force_num(abs_spir[4])
		elif opt in ("--displacement_map") and  DISPLACEMENT_MAP in single_transfs:
			displacement_map = re.split(r'\&', arg.rstrip())
			displacement_map_1 = displacement_map[0]
			displacement_map_2 = displacement_map[1]
		elif opt in ("--displacement_map_factor") and  DISPLACEMENT_MAP in single_transfs:
			displacement_map_factor = force_num(arg)
		elif opt in ("--displacement_map_center") and  DISPLACEMENT_MAP in single_transfs:
			displacement_map_center_file_name = arg
		else:
			msg = "unhandled option ["+opt+"]"
			assert False, msg
			print( main_args )
			print(USAGE)
			sys.exit(2)

	# print("Transformations :", single_transfs)
	# print("translation :", str(x_absolute_translation), " ", str(y_absolute_translation))
	# print("scale :", str(x_absolute_scaling_center), " ",str(y_absolute_scaling_center), " ", str(scaling_factor))

	##################################################################
	##################################################################
	# MAIN PROGRAM
	##################################################################
	##################################################################
	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name, " for transformation "+transf_string)
	  
	try:
		FILEin = open(input_file_name, "rt")
	except IOError:
		print("File not found :", input_file_name, " for transformation "+transf_string)
	  
	path_data = []
	path_data_size = 0
	path_beam_data_string = ""
	prec_path_data_string = ""
	path_data_string = ""

	# parameters of the initial transformation retrieved for the final transformation
	firstpass_translation = ""
	firstpass_scale = ""
	firstpass_nb_frames = ""
	firstpass_percent_life = ""
	firstpass_group_no = -1

	##################################################################
	# STORES THE LINES IN AN ARRAY
	##################################################################
	contents = FILEin.read()
	Unormalized_SVG_file_lines = contents.splitlines()

	##################################################################
	# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
	##################################################################
	SVG_file_lines = []
	# print("Initial file path: ", input_file_name)
	# print("Initial file [", Unormalized_SVG_file_lines, "]")
	indLine = 0
	while( indLine < len(Unormalized_SVG_file_lines)) :
		line_string = Unormalized_SVG_file_lines[indLine]
		indLine += 1	
		if(re.search(r'<path', line_string) != None) :
			line_string = re.sub(r'^[\s]+<path', r'<path', line_string)
			while(re.search(r'/>', line_string) == None and indLine < len(Unormalized_SVG_file_lines)) :
				line_string = line_string + Unormalized_SVG_file_lines[indLine]
				indLine += 1	
			# transforms , into space
			line_string = re.sub(r'\,', r' ', line_string)
			# only keeps the path data
			# line_string = re.sub(r'<path.*\sd="([^\"]*)\".*/>', r'<path d="\1" />', line_string)
		elif(re.search(r'<g', line_string) != None) :
			line_string = re.sub(r'^[\s]+<g', r'<g', line_string)
			while(re.search(r'>', line_string) == None and indLine < len(Unormalized_SVG_file_lines)) :
				line_string = line_string + Unormalized_SVG_file_lines[indLine]
				indLine += 1	
		SVG_file_lines.append( line_string )

	##################################################################
	# MAKES ABSOLUTE COORDINATES
	##################################################################
	absoluteCoords_SVG_line_and_paths = []
	number_of_paths = 0
	number_of_groups = 0
	for line_string in SVG_file_lines :
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		# print("Initial line_string [", line_string, "]")
		if( re.search(r'^<path', line_string) != None):
			search_result = re.search(r'\sd\=\"([^\"]+)\"', line_string)	
			# a path without data is discarded
			if(search_result != None):
				##################################################################
				# transforms relative coordinates of base video into absolute ones
				##################################################################
				prec_path_data_string = path_data_string
				path_data_string = search_result.group(1)
				# print("Path line_string [", path_data_string, "]")
				# adds space after move or curve and before z
				path_data_string = re.sub(r'([MmCcLlHhVv])', r'\1 ', path_data_string)
				path_data_string = re.sub(r'([Zz])', r' \1 ', path_data_string)
				# print("Relative coordinates [", path_data_string, "]")

				# transformation into absolute coordinates
				path_data_string = relative_to_absolute_coordinates(path_data_string, True)

				#storage
				result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
				# if not result_ID:
				#     print('Expected ID not found')
				result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
				# if not result_style:
				#     print('Expected style not found')
				if result_ID and result_style :
					line_string = '<path id="{0!s}" style="{1!s}" d="{2!s}"/>'.format(result_ID.group(1), result_style.group(1), path_data_string)
				else :
					line_string = '<path d="{0!s}"/>'.format(path_data_string)
				absoluteCoords_SVG_line_and_paths.append( [line_string, path_data_string, firstpass_translation, firstpass_scale, \
														   firstpass_nb_frames, firstpass_percent_life, firstpass_group_no] )
				number_of_paths += 1
				# print("Absolute coordinates [", line_string, "]")
		else:
			# if it is a group, the parameters of the transformations are stored
			if (re.search(r'<g ', line_string) != None) :
				number_of_groups += 1
				# parameters of the initial transformation retrieved for the final transformation
				firstpass_translation = ""
				firstpass_scale = ""
				firstpass_nb_frames = ""
				firstpass_percent_life = ""
				firstpass_group_no = -1

				# LINKS transformation keeps a memory of the initial transformation: 
				# stores the characteristics of the initial transformation (such as an initial translation)
				# so that they can be used in the final transformation as well
				if(re.search(r' vv-transformation=', line_string) == None) :
					SVG_field_transformations = ""
					for opt, arg in opts:
						if opt.startswith("--") :
							for opt_string in ["transformation","translation","nb_frames","percent_life"] :
								if opt[len("--"):].startswith(opt_string) :
									SVG_field_transformations = SVG_field_transformations + "vv-" + opt_string + '="' + str(arg) + '" '
							if opt[len("--"):].startswith("scale") :
								suffix = opt[len("--scale"):]
								if(suffix == "") :
									indscale == 1
								else :
									ind_scale = int(to_num(suffix))
								if len(single_transfs) >= ind_scale and single_transfs[ind_scale - 1] == SCALE :
									SVG_field_transformations = SVG_field_transformations + 'vv-scale="' + str(arg) + '" '
					SVG_field_transformations = SVG_field_transformations + 'vv-group_no="' + str(number_of_groups) + '" '
					# print("SVG_field_transformations", SVG_field_transformations)
					line_string = line_string.replace(r'<g ', '<g '+SVG_field_transformations)

				# final transformation: retrieves the characteristics of the initial transformation
				else :
					if(LINKS in single_transfs) :
						# 	print("group :", line_string)
						search_result = re.search(r' vv-translation="([^\"]+)"', line_string)
						if(search_result != None) :
							firstpass_translation = search_result.group(1)
						search_result = re.search(r' vv-scale="([^\"]+)"', line_string)
						if(search_result != None) :
							firstpass_scale = search_result.group(1)
						search_result = re.search(r' vv-nb_frames="([^\"]+)"', line_string)
						if(search_result != None) :
							firstpass_nb_frames = search_result.group(1)
						search_result = re.search(r' vv-percent_life="([^\"]+)"', line_string)
						if(search_result != None) :
							firstpass_percent_life = search_result.group(1)
						search_result = re.search(r' vv-group_no="([^\"]+)"', line_string)
						if(search_result != None) :
							firstpass_group_no = int(force_num(search_result.group(1)))
							# print("second pass group no", firstpass_group_no)

			absoluteCoords_SVG_line_and_paths.append( [line_string, "", firstpass_translation, firstpass_scale, \
													   firstpass_nb_frames, firstpass_percent_life, firstpass_group_no] )

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	transformed_SVG_file_line_out = []
	transformation_no = 1
	for single_transf in single_transfs :
		######################################################################
		# FIRST MAKES ALL WHAT HAS TO BE MADE FOR ALL THE PATHS

		# copies the output of the preceding transformation
		if(transformation_no > 1) :
			absoluteCoords_SVG_line_and_paths = transformed_SVG_file_line_out
		
		# creates a log file
		if(single_transf == COPY_TOP_LEFT) :
			log_file_name = input_file_name
			log_file_name = re.sub(r'_[0-9]+\.svg', r'_log.csv', log_file_name)
			try:
				FILE_log = open(log_file_name, "at")
			except IOError:
				print("Log file not appended :", log_file_name )

		# opens the displacement maps
		if(single_transf == DISPLACEMENT_MAP) :
			# loads the displacement maps data (a 2D array of the displacement vectors)
			if os.path.isfile(displacement_map_1):
				displacement_map_1 = cv.imread(displacement_map_1, cv.IMREAD_UNCHANGED)
				# print("displacement map #1 opened: ", displacement_map_1)
			else:
				print("displacement map #1 not opened: ", displacement_map_1)
				sys.exit(2)
			if os.path.isfile(displacement_map_2):
				displacement_map_2 = cv.imread(displacement_map_2, cv.IMREAD_UNCHANGED)
				# print("displacement map #2 opened: ", displacement_map_2)
			else:
				print("displacement map #2 not opened: ", displacement_map_2)
				sys.exit(2)
			if os.path.isfile(displacement_map_center_file_name):
				print("opening displacement map center:", displacement_map_center_file_name)
				try:
					FILEin = open(displacement_map_center_file_name, "rt")
				except IOError:
					print("File not found :", displacement_map_center_file_name, " for center of displacement map")
				# print("File found :", displacement_map_center_file_name, " for center of displacement map")
				readCSV = csv.reader(FILEin, delimiter=',')
				# reads the CVS until finding the center for the current movie
				while(True):
					line = next(readCSV)
					movie_name = input_file_name
					movie_name = re.sub(r'.*\/','',movie_name)
					movie_name = re.sub(r'_.*','',movie_name)
					# print("movie name", movie_name)
					if(line[0] == movie_name) :
						displacement_map_center_coords = list(map(int, line[1:3]))
						# print("displacement_map_center_coords in HD", displacement_map_center_coords)
						displacement_map_height, displacement_map_width = displacement_map_1.shape
						# print("displacement_map_height, displacement_map_width", displacement_map_height, " ", displacement_map_width)
						displacement_map_center_coords[0] *= displacement_map_width/float(VV_MOVIE_IMAGE_WIDTH)
						displacement_map_center_coords[1] *= displacement_map_height/float(VV_MOVIE_IMAGE_HEIGHT)
						displacement_map_center_translation \
							= [displacement_map_width/2 - displacement_map_center_coords[0], \
								displacement_map_height/2 - displacement_map_center_coords[1]]
						# print("displacement_map_center_coords", displacement_map_center_coords)
						# print("displacement_map_center_translation", displacement_map_center_translation)
						break
					if(readCSV == None):
						break
			else:
				print("displacement map center not opened: ", displacement_map_center_file_name)
				sys.exit(2)

		# the list of transformed lines (including non <path lines)
		transformed_SVG_file_line_out = []
		# the  rank of the current line_string in the SVG file before the nth transformation
		transformed_line_rank = 0
		# the  rank of the current <path line_string in the SVG file before the nth transformation
		path_rank = 0
		# the rank of the preceding line_string that is a <path
		prec_path_line_rank = -1
		# full line_string
		line_string = ""
		# path data string in case of a <path string (the string inside d="(.*)")
		path_data_string = ""
		# additional path data string in case of addition of a new <path string (spider web)
		path_beam_data_string = ""
		# additional path data string in case of addition of a new <path string (link between two images)
		list_of_link_paths_strings = []

		##################################################################
		# appplies the transformation on each path
		##################################################################
		for line_path in absoluteCoords_SVG_line_and_paths :
			# print("line_string :[",line_string,"]")
			line_string, path_data_string, firstpass_translation, firstpass_scale, \
			firstpass_nb_frames, firstpass_percent_life, firstpass_group_no = line_path
			
			##########################################################################
			# PATH LINE RODUCED
			##########################################################################
			if( path_data_string != "" ):
				##################################################################
				# COPY TRANSFORMATION
				##################################################################
				if(single_transf == COPY) :
					# nothing to be done
					path_data_string = path_data_string
				
				##################################################################
				# COPY TRANSFORMATION AND STORES TOP LEFT
				##################################################################
				elif(single_transf == COPY_TOP_LEFT) :
					# nothing to be done
					path_data_string = path_data_string
					COPY_TOP_LEFT_transformation()
				
				##################################################################
				# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
				##################################################################
				elif(single_transf == V_SCALE) :
					path_data_string = V_SCALE_transformation(path_data_string, path_rank, percent_transf[transformation_no])
				
				##################################################################
				# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
				##################################################################
				elif(single_transf == V_SCALE2) :
					path_data_string = V_SCALE2_transformation(path_data_string, path_rank, percent_transf[transformation_no])
				
				##################################################################
				# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
				# similar to V_SCALE2 but with double length for the first part wrt the second part
				##################################################################
				elif(single_transf == V_SCALE3) :
					path_data_string = V_SCALE3_transformation(path_data_string, path_rank, percent_transf[transformation_no])
				
				##################################################################
				# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
				# similar to V_SCALE3 but from top instead of from bottom
				##################################################################
				elif(single_transf == V_SCALE4) :
					path_data_string = V_SCALE4_transformation(path_data_string, path_rank, percent_transf[transformation_no])
				
				##################################################################
				# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
				##################################################################
				elif(single_transf == C_SCALE) :
					# print("transformation_no", transformation_no)
					# print("scaling_factor", scaling_factor)
					path_data_string = C_SCALE_transformation(path_data_string, \
						scaling_factor[transformation_no], \
						x_absolute_scaling_center[transformation_no]/float(VV_MOVIE_IMAGE_WIDTH)*float(VV_SVG_PAGE_WIDTH), \
						y_absolute_scaling_center[transformation_no]/float(VV_MOVIE_IMAGE_HEIGHT)*float(VV_SVG_PAGE_HEIGHT))
				
				##################################################################
				# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
				##################################################################
				elif(single_transf == SCALE_TO_CIRCLE) :
					# print("transformation_no", transformation_no)
					path_data_string = SCALE_TO_CIRCLE_transformation(path_data_string, \
						scaling_factor[transformation_no], \
						x_absolute_scaling_center[transformation_no]/float(VV_MOVIE_IMAGE_WIDTH)*float(VV_SVG_PAGE_WIDTH), \
						y_absolute_scaling_center[transformation_no]/float(VV_MOVIE_IMAGE_HEIGHT)*float(VV_SVG_PAGE_HEIGHT))
				
				##################################################################
				# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
				##################################################################
				elif(single_transf == C_SCALE_BY_LAYERS) :
					path_data_string = C_SCALE_BY_LAYERS_transformation(path_data_string, \
						path_rank, scaling_factor[transformation_no] )
				
				##################################################################
				# ALL DIRECTIONS RANDOMNESS
				##################################################################
				elif(single_transf == VH_RANDOM) :
					path_data_string = VH_RANDOM_transformation(path_data_string, percent_transf[transformation_no])
				
				##################################################################
				# VERTICAL FLATNESS
				##################################################################
				elif(single_transf == V_FLATTEN) :
					path_data_string = V_FLATTEN_transformation(path_data_string, percent_transf[transformation_no])
				
				##################################################################
				# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
				##################################################################
				elif(single_transf == SMOOTHING	or single_transf == BLUR) :
					path_data_string = V_FLATTEN_or_BLUR_transformation(path_data_string, percent_transf[transformation_no])
				
				##################################################################
				# PLACES THE TOP LEFT POINT OF THE DRAWING AT THE TOP LEFT POINT OF THE PAGE
				##################################################################
				elif(single_transf == REFRAME_TOP_LEFT) :
					path_data_string = REFRAME_TOP_LEFT_transformation(path_data_string)
				
				##################################################################
				# TRANSLATES THE FULL IMAGE
				##################################################################
				elif(single_transf == TRANSLATE) :
					path_data_string = TRANSLATE_transformation(path_data_string, path_rank, x_absolute_translation[transformation_no], y_absolute_translation[transformation_no])
				
				##################################################################
				# ROTATES THE FULL IMAGE
				##################################################################
				elif(single_transf == ROTATE) :
					path_data_string = ROTATE_transformation(path_data_string, x_absolute_rotation_center[transformation_no], y_absolute_rotation_center[transformation_no], rotation_angle[transformation_no])
				
				##################################################################
				# SCALES THE FULL IMAGE
				##################################################################
				elif(single_transf == SCALE) :
					# print("transformation_no", transformation_no)
					# print("scaling_factor", scaling_factor[transformation_no])
					# print("x_absolute_scaling_center", x_absolute_scaling_center[transformation_no])
					# print("y_absolute_scaling_center", y_absolute_scaling_center[transformation_no])
					path_data_string = SCALE_transformation(path_data_string, \
						scaling_factor[transformation_no], \
						x_absolute_scaling_center[transformation_no], \
						y_absolute_scaling_center[transformation_no] )
				
				##################################################################
				# SYMMETRIES
				##################################################################
				elif(single_transf == H_SYMMETRY) :
					path_data_string = SYMMETRY_transformation(path_data_string,  VV_SVG_PAGE_WIDTH/2, -1)
				
				elif(single_transf == V_SYMMETRY) :
					path_data_string = SYMMETRY_transformation(path_data_string, -1, VV_SVG_PAGE_HEIGHT/2)
				
				##################################################################
				# WEB LINE
				##################################################################
				elif(single_transf == BEZIER_CURVE) :
					path_data_string = "M "+str(b0_x)+" "+str(b0_y)+" C "+str(b1_x)+" "+str(b1_y)+" "+str(b2_x)+" "+str(b2_y)+" "+str(b3_x)+" "+str(b3_y)
				
				##################################################################
				# SPIRAL
				##################################################################
				elif(single_transf == ARCHIMEDE_SPIRAL) :
					path_data_string = ARCHIMEDE_SPIRAL_transformation(spiral_center_x, spiral_center_y, spiral_angle_step, spiral_radius_step)
				
				##################################################################
				# CONCENTRIC POLYGONES
				##################################################################
				elif(single_transf == C_POLYGONS) :
					path_data_string = C_POLYGONS_transformation(spiral_center_x, spiral_center_y, spiral_angle_step, spiral_radius_step)

				##################################################################
				# ADDS RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
				##################################################################
				elif(single_transf == C_LINES) :
					path_beam_data_string = C_LINES_transformation(path_data_string, path_rank, percent_transf[transformation_no])
					# print ("************ Beam path path_beam_data_string\n"
				
				##################################################################
				# CREATE BEZIER CURVES BETWEEN TWO IMAGES (THEIR PATHS)
				##################################################################
				elif(single_transf == LINKS) :
					# print("arguments for LINKS_transformation : firstpass_nb_frames",firstpass_nb_frames )
					path_data_string, list_of_link_paths_strings = \
						LINKS_transformation(path_data_string, transformed_line_rank, prec_path_line_rank, firstpass_translation, firstpass_scale, \
											 firstpass_nb_frames, firstpass_percent_life, path_rank, percent_transf[transformation_no], firstpass_group_no)
				
				##################################################################
				# TRANSLATION COPIED FROM AN OPTICAL FLOW MAP
				##################################################################
				elif(single_transf == DISPLACEMENT_MAP) :
					path_data_string = DISPLACEMENT_MAP_transformation(path_data_string, displacement_map_factor, percent_transf[transformation_no])
				
				##################################################################
				# UNKNOWN TRANSFORMATION
				##################################################################
				else :
					print( "Unknown transformation #", single_transf)
					return 0
				
				##################################################################
				# PRINTS THE TRANSFORMED PATH
				# line_string = "<path d=\"" + path_data_string + "\"/>"
				line_string = re.sub(r'\sd="([^\"]*)\"', r' d="'+path_data_string+'"', line_string)
				transformed_SVG_file_line_out.append([line_string, path_data_string, firstpass_translation, firstpass_scale, \
													  firstpass_nb_frames, firstpass_percent_life, firstpass_group_no])
				# it is a path line, the memory of its rank is kept for the next path line
				prec_path_line_rank = transformed_line_rank
				transformed_line_rank += 1

				##################################################################
				# IN THE CASE OF LINKS_transformation STORES THE LINKS PATHS
				if single_transf == LINKS and list_of_link_paths_strings != [] :
					for link_paths_strings in list_of_link_paths_strings:
						transformed_SVG_file_line_out.append([link_paths_strings, "", firstpass_translation, firstpass_scale, \
															  firstpass_nb_frames, firstpass_percent_life, firstpass_group_no])
						transformed_line_rank += 1
					list_of_link_paths_strings = []

				# counts the path lines
				path_rank += 1
			
			##########################################################################
			# NO PATH LINE REPRODUCED
			##########################################################################
			else :
				##################################################################
				# IN THE CASE OF C_LINES_transformation STORES THE LINKS PATHS
				if (re.search(r'</svg>', line_string) != None) and (path_beam_data_string != "") :
					transformed_SVG_file_line_out.append( ["<g transform=\"translate(0.000000,2160.000000) scale(0.100000,-0.100000)\" style=\"stroke:url(#radialGradient001); stroke-opacity:1;\">", "", "", "", "","",-1])
					transformed_line_rank += 1
					line_string = "<path d=\"" + path_beam_data_string + "\" id=\"path116\" style=\"stroke:url(#radialGradient001); stroke-opacity:1;\"/>"
					transformed_SVG_file_line_out.append( [line_string, path_beam_data_string, "", "", "", "",-1])
					transformed_line_rank += 1
					transformed_SVG_file_line_out.append( ["</g>", "", "", "", "", "",-1] )
					transformed_line_rank += 1
				
				# reproducing a non <path line
				transformed_SVG_file_line_out.append( [line_string, "", firstpass_translation, firstpass_scale, \
													   firstpass_nb_frames, firstpass_percent_life, firstpass_group_no] )
				transformed_line_rank += 1

			# loop on all the files of the absoluteCoords_SVG_line_and_paths list

		if(single_transf == COPY_TOP_LEFT) :
			FILE_log.close()
		
		transformation_no += 1
	
	##########################################################################
	# COPIES THE LINES IN THE OUTPUT FILE
	##########################################################################
	for line_path in transformed_SVG_file_line_out :
		line_string, path_data_string, initial_translation, firstpass_scale, \
			firstpass_nb_frames, firstpass_percent_life, firstpass_group_no = line_path
		# print("transformed line_string (",line_string,")")
		FILEout.write(str(line_string)+"\n")

	FILEin.close()
	FILEout.close()
	return 1


##################################################################
##################################################################
# TRANSFORMATION FUNCTIONS
##################################################################
##################################################################

##################################################################
# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
def V_SCALE_transformation(local_path_data_string, local_path_no, local_percent_transf):
	global nb_layers

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)
	
	# horizontal base line according to the number of colors
	y_reference = (VV_SVG_PAGE_HEIGHT / (nb_layers - 1)) * local_path_no
	# scales the path vertically according to the number of colors
	index = 0
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = y_reference + (1 - local_percent_transf) * (force_num(local_path_data[index + 1]) - y_reference)
			index += 2 
		else :
			index += 1
		
	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
def V_SCALE2_transformation(local_path_data_string, local_path_no, local_percent_transf):
	global nb_layers

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# arbirary height of the initially visible part
	h_ini = VV_SVG_PAGE_HEIGHT/3
	# height of the whole image
	h_final = VV_SVG_PAGE_HEIGHT
	# scaling of the layers to the initially visible part
	initial_scale = h_ini/h_final
	# scaling of the layers to the initially visible part
	mid_scale = 1.0/nb_layers
	# scaling of the layers to the full image
	final_scale = 1
	# initial translation: no translation, only scaling down to the visible part
	y_initial_translation = 0
	# middle translation: scaling is kept down to the visible part but layers are brought towards the top
	# according to the color rank
	y_mid_translation = (h_final / nb_layers) * local_path_no
	# final translation is no translation again but the image has the final size
	y_final_translation = 0
	# 2 steps: first translation towards the top according to layer rank and scaling according to the layer so as to make the layers visible
	# second scaling towards 1 and translation towards 0 
	# local_percent_transf varies from 1 to 0
	index = 0
	local_percent = 0
	local_translation = 0
	local_scale = 0
	# first part, local_percent_transf from 1 to 0.5, 1-percant from 0 to 0.5, local_percent from 0 to 1
	if((1 - local_percent_transf) < 0.5) :
		local_percent = (1 - local_percent_transf) * 2
		local_translation = (1.0 - local_percent) * y_initial_translation + local_percent * y_mid_translation
		local_scale = (1.0 - local_percent) * initial_scalec + local_percent * mid_scale
	# second part, local_percent_transf from 0.5 to 0. 1-local_percent_transf from 0.5 to 1, Local local_percent_transf from 0 to 1
	else :
		local_percent = (1 - local_percent_transf) * 2 - 1.0
		local_translation = (1.0 - local_percent) * y_mid_translation + local_percent * y_final_translation
		local_scale = (1.0 - local_percent) * mid_scale + local_percent * final_scale
	
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = local_translation + local_scale * force_num(local_path_data[index + 1])
			index += 2
		else :
			index += 1
	
	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
# similar to V_SCALE2 but with double length for the first part wrt the second part
def V_SCALE3_transformation(local_path_data_string, local_path_no, local_percent_transf):
	global nb_layers

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# arbirary height of the initially visible part
	h_ini = VV_SVG_PAGE_HEIGHT/3
	# height of the whole image
	h_final = VV_SVG_PAGE_HEIGHT
	# scaling of the layers to the initially visible part
	initial_scale = h_ini/h_final
	# scaling of the layers to the initially visible part
	mid_scale = 1.0/nb_layers
	# scaling of the layers to the full image
	final_scale = 1
	# initial translation: no translation, only scaling down to the visible part
	y_initial_translation = 0
	# middle translation: scaling is kept down to the visible part but layers are brought towards the top
	# according to the color rank
	y_mid_translation = (h_final / nb_layers) * local_path_no
	# final translation is no translation again but the image has the final size
	y_final_translation = 0
	# 2 steps: first translation towards the top according to layer rank and scaling according to the layer so as to make the layers visible
	# second scaling towards 1 and translation towards 0 
	# local_percent_transf varies from 1 to 0
	index = 0
	local_percent = 0
	local_translation = 0
	local_scale = 0
	# first part, local_percent_transf from 1 to 0.33 (1-local_percent_transf varies from 0 to 0.66)
	# local_percent varies from 0 to 1
	if((1 - local_percent_transf) < 0.66) :
		local_percent = (1 - local_percent_transf) / 0.66 # from 0 to 0.66/0.66
		local_translation = (1.0 - local_percent) * y_initial_translation + local_percent * y_mid_translation
		local_scale = (1.0 - local_percent) * initial_scale + local_percent * mid_scale
	# second part, local_percent_transf from 0.33 to 0. (1-local_percent_transf varies from 0.66 to 1.)
	# local_percent varies from 0 to 1
	else :
		local_percent = 1 - (local_percent_transf / (1 - 0.66)) # from 1 - (1 - 0.66)/(1 - 0.66) = 0 to 1 - (0/(1 - 0.66)) = 1
		local_translation = (1.0 - local_percent) * y_mid_translation + local_percent * y_final_translation
		local_scale = (1.0 - local_percent) * mid_scale + local_percent * final_scale
	
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = local_translation + local_scale * force_num(local_path_data[index + 1])
			index += 2
		else :
			index += 1

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
# similar to V_SCALE3 but from top instead of from bottom
def V_SCALE4_transformation(local_path_data_string, local_path_no, local_percent_transf):
	global nb_layers

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# arbirary height of the initially visible part
	h_ini = VV_SVG_PAGE_HEIGHT/3
	# height of the whole image
	h_final = VV_SVG_PAGE_HEIGHT
	# scaling of the layers to the initially visible part
	initial_scale = h_ini/h_final
	# scaling of the layers to the initially visible part
	mid_scale = 1.0/nb_layers
	# scaling of the layers to the full image
	final_scale = 1
	# initial translation: layers are aligned to the top, only scaling down to the visible part
	y_initial_translation = (h_final / nb_layers) * (nb_layers - 1)
	# middle translation: scaling is kept down to the visible part but layers are brought towards the top
	# according to the color rank
	y_mid_translation = (h_final / nb_layers) * (nb_layers - 1 - local_path_no)
	# final translation is no translation again but the image has the final size
	y_final_translation = 0
	# 2 steps: first translation towards the top according to layer rank and scaling according to the layer so as to make the layers visible
	# second scaling towards 1 and translation towards 0 
	# local_percent_transf varies from 1 to 0
	index = 0
	local_percent = 0
	local_translation = 0
	local_scale = 0
	# first part, local_percent_transf from 1 to 0.33 (1-local_percent_transf varies from 0 to 0.66)
	# local_percent varies from 0 to 1
	if((1 - local_percent_transf) < 0.66) :
		local_percent = (1 - local_percent_transf) / 0.66 # from 0 to 0.66/0.66
		local_translation = (1.0 - local_percent) * y_initial_translation + local_percent * y_mid_translation
		local_scale = (1.0 - local_percent) * initial_scale + local_percent * mid_scale
	# second part, local_percent_transf from 0.33 to 0. (1-local_percent_transf varies from 0.66 to 1.)
	# local_percent varies from 0 to 1
	else :
		local_percent = 1 - (local_percent_transf / (1 - 0.66)) # from 1 - (1 - 0.66)/(1 - 0.66) = 0 to 1 - (0/(1 - 0.66)) = 1
		local_translation = (1.0 - local_percent) * y_mid_translation + local_percent * y_final_translation
		local_scale = (1.0 - local_percent) * mid_scale + local_percent * final_scale
	
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = local_translation + local_scale * force_num(local_path_data[index + 1])
			index += 2
		else :
			index += 1

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# ALL DIRECTIONS RANDOMNESS
def VH_RANDOM_transformation(local_path_data_string, local_percent_transf):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	if(local_percent_transf <= 0) :
		return local_path_data_string

	# adds all directions randomness with weight on the control point rank
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = force_num(local_path_data[index]) + local_percent_transf * (random.random() * 3000)
				local_path_data[index + 1] = force_num(local_path_data[index + 1]) + local_percent_transf * (random.random() * 3000)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			while(True) :
				for i in range( index, index + 6, 2) :
					control_point_rank = int(math.floor((i - index)/2)) + 1
					weight = control_point_rank / 3.0
					local_path_data[i] = force_num(local_path_data[i]) + local_percent_transf * weight * (random.random() * 3000)
					local_path_data[i + 1] = force_num(local_path_data[i + 1]) + local_percent_transf * weight * (random.random() * 3000)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("VH_RANDOM_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL FLATNESS
def V_FLATTEN_transformation(local_path_data_string, local_percent_transf):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# scales the path centrally according to the number of colors
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			defined_y_reference = False
			# processes the next moveto and lineto
			while(True) :
				if(defined_y_reference) :
					local_path_data[index + 1] = y_reference + (1 - local_percent_transf) * (force_num(local_path_data[index + 1]) - y_reference)
				else :
					y_reference = force_num([index + 1])
					defined_y_reference = True
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			defined_y_reference = False
			while(True) :
				for i in range( index, index + 6, 2) :
					control_point_rank = int(math.floor((i - index)/2)) + 1
					weight = control_point_rank / 3.0
					if(defined_y_reference) :
						local_path_data[i + 1] = y_reference + (1 - local_percent_transf * weight) * (force_num(local_path_data[i + 1]) - y_reference)
					else :
						y_reference = force_num(local_path_data[i + 1])
						defined_y_reference = True
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("V_FLATTEN_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
def norm(vec2) :
	return sqr(vec2[0]*vec2[0]+vec2[1]*vec2[1])
def dot(vec1, vec2) :
	return (vec1[0]*vec2[0]+vec1[1]*vec2[1])

def V_FLATTEN_or_BLUR_transformation(local_path_data_string, local_percent_transf):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	index = 0
	smooting_percentage = local_percent_transf

	# simplifies the curve by removing segments according to percentage
	if(local_single_transf == BLUR):
		# a percentage of 1.0 gives a blank image
		if(local_percent_transf > 0.9) :
			local_percent_transf = 0.9
		
		simplified_path_data = []
		preserved_multiple_curve_segment
		for ind in range(1, 21, 1) :
			if(local_percent_transf < ind * 0.05) :
				preserved_multiple_curve_segment = ind + 1
				smooting_percentage = 1 - 20 * (local_percent_transf - (ind - 1) * 0.05)
				last
		
		while(index < local_path_data_size) :
			if(local_path_data[index] == "M" or local_path_data[index] == "L") :
				simplified_path_data.append(local_path_data[index])
				index += 1
				# copies the next moveto and lineto
				while(True) :
					simplified_path_data.append(force_num(local_path_data[index]))
					index += 1
					simplified_path_data.append(force_num(local_path_data[index]))
					index += 1
				
					if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
						break
			
			elif(local_path_data[index] == "C") :
				simplified_path_data.append(local_path_data[index])
				index += 1
				curve_rank = 0
				while(True) :
					# copies the next curve segments
					# if it has the rank to be preserved (the n-1th if every nth is preserved)
					# or if it is the last one
					if( ((int(curve_rank) % int(preserved_multiple_curve_segment)) == (int(preserved_multiple_curve_segment) - 1))\
						or ((index + 6 < local_path_data_size) and (re.search(r'^[MCLZz]',local_path_data[index + 6]) != None)) ) :
						for ind in range(6) :
							simplified_path_data.append(force_num(local_path_data[index]))
							index += 1
					# discards the next curve segments
					else :
						index += 6
					
					curve_rank += 1
				
					if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
						break
			
			elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
				simplified_path_data.append(force_num([index]))
				index += 1
			
			else :
				print ("Line: ", local_path_data_string)
				print ("V_FLATTEN_or_BLUR_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
				return ""
			
		local_path_data = simplified_path_data
		local_path_data_size = len(local_path_data)

	# smoothes the curve with symmetrical control points around a final control point
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# passes the next moveto and lineto, no smoothing takes lace here
			while(True) :
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			initial_polycurve_index = index
			# processes the preceding curve segments (and requires two previous curve segments)
			while(True) :
				if((index - initial_polycurve_index) >= 12):
					# the vectors joining the last control points of 2 neighboring segments
					vec_In_1In = [force_num(local_path_data[index]) - force_num(local_path_data[index - 6]), force_num(local_path_data[index + 1]) - force_num(local_path_data[index - 6 + 1])]
					vec_In_1In_2 = [force_num(local_path_data[index - 12]) - force_num(local_path_data[index - 6]), force_num(local_path_data[index - 12 + 1]) - force_num(local_path_data[index - 6 + 1])]
					# and their norms
					norm_In_1In = norm(vec_In_1In)
					norm_In_1In_2 = norm(vec_In_1In_2)
					# the central last control point
					point_In = [force_num(local_path_data[index - 6]), force_num(local_path_data[index - 6 + 1])]
					# if the 3 last control points are distinct
					if(norm_In_1In * norm_In_1In_2 != 0) :
						# calculates the normal vector at the central point by summing the normalized two vectors
						vec_normal = [(vec_In_1In[0] / norm_In_1In) + (vec_In_1In_2[0] / norm_In_1In_2), (vec_In_1In[1] / norm_In_1In) + (vec_In_1In_2[1] / norm_In_1In_2)]
						# normalizes it
						norm_normal = norm(vec_normal)
						if(norm_normal != 0) :
							vec_normal = [vec_normal[0] / norm_normal, vec_normal[1] / norm_normal]
							# calculates the tangent by giving it the average third length of the distance between the last control points
							vec_tang_In = [vec_normal[1] * (norm_In_1In + norm_In_1In_2) / 6.0, -vec_normal[0] * (norm_In_1In + norm_In_1In_2) / 6.0]
							# aligns the tangent with the vector In-1 In-2
							if(dot(vec_tang_In, vec_In_1In_2) < 0) :
								vec_tang_In = [-vec_tang_In[0],-vec_tang_In[1]]
							
							# calculates the new control points around the central point In-1
							# the control point before In-1
							local_path_data[index - 6 - 2] = (1 - smooting_percentage) * force_num(local_path_data[index - 6 - 2]) + smooting_percentage * (point_In[0] + vec_tang_In[0])
							local_path_data[index - 6 - 2 + 1] = (1 - smooting_percentage) * force_num(local_path_data[index - 6 - 2 + 1]) + smooting_percentage * (point_In[1] + vec_tang_In[1])
							# the control point after In-1
							local_path_data[index - 6 + 2] = (1 - smooting_percentage) * force_num(local_path_data[index - 6 + 2]) + smooting_percentage * (point_In[0] - vec_tang_In[0])
							local_path_data[index - 6 + 2 + 1] = (1 - smooting_percentage) * force_num(local_path_data[index - 6 + 2 + 1]) + smooting_percentage * (point_In[1] - vec_tang_In[1])
						
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("V_FLATTEN_or_BLUR_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# PLACES THE TOP LEFT POINT OF THE DRAWING AT THE TOP LEFT POINT OF THE PAGE
def REFRAME_TOP_LEFT_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# top left and bottom right of the drawing have been stored by 
	# relative_to_absolute_coordinates
	# min_abs_X
	# min_abs_Y
	# max_abs_X
	# max_abs_Y

	# translates the drawing to the top left
	(x_transl, y_transl) = (-min_abs_X, VV_SVG_PAGE_HEIGHT-max_abs_Y) 
	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_transl + force_num(local_path_data[index])
				local_path_data[index + 1] = y_transl + force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] = x_transl + force_num(local_path_data[i])
					local_path_data[i + 1] = y_transl + force_num(local_path_data[i + 1])
				
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("REFRAME_TOP_LEFT_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# TRNASLATION TRANSFORMATION
def TRANSLATE_transformation(local_path_data_string, loc_path_rank, x_transl, y_transl):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_transl + force_num(local_path_data[index])
				local_path_data[index + 1] = y_transl + force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] = x_transl + force_num(local_path_data[i])
					local_path_data[i + 1] = y_transl + force_num(local_path_data[i + 1])
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("TRANSLATE_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# ROTATION TRANSFORMATION
def apply_rotation(x, y, x_center, y_center, angle) :
	x_ret
	y_ret
	x_ret = x_center + cos(angle) * (x - x_center) - sin(angle) * (y - y_center)
	y_ret = y_center + sin(angle) * (x - x_center) + cos(angle) * (y - y_center)
	return [x_ret, y_ret]

def ROTATE_transformation(local_path_data_string, x_center, y_center, angle):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] , local_path_data[index + 1] = apply_rotation(force_num(local_path_data[index]) , force_num(local_path_data[index + 1]), x_center, y_center, angle)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] , local_path_data[i + 1] = apply_rotation(force_num(local_path_data[i]) , force_num(local_path_data[i + 1]), x_center, y_center, angle)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("ROTATE_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""	

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE), default center is screen center
def C_SCALE_transformation(local_path_data_string, local_scaling_factor, x_reference = VV_SVG_PAGE_WIDTH / 2, y_reference = VV_SVG_PAGE_HEIGHT / 2, scaling_ratio = 1.0/3.0):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# scales the path centrally =
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_reference + (1 - local_scaling_factor) * (force_num(local_path_data[index]) - x_reference)
				local_path_data[index + 1] = y_reference + (1 - local_scaling_factor) * (force_num(local_path_data[index + 1]) - y_reference)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break

		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			while(True) :
				for i in range(index, index + 6, 2) :
					control_point_rank = int(math.floor((i - index)/2)) + 1
					weight = control_point_rank * scaling_ratio
					local_path_data[i] = x_reference + (1 - local_scaling_factor * weight) * (force_num(local_path_data[i]) - x_reference)
					local_path_data[i + 1] = y_reference + (1 - local_scaling_factor * weight) * (force_num(local_path_data[i + 1]) - y_reference)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print( "Line: ", local_path_data_string)
			print( "V_SCALE_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE), default center is screen center
def C_SCALE_BY_LAYERS_transformation(local_path_data_string, local_path_no, local_scaling_factor, x_reference = VV_SVG_PAGE_WIDTH / 2, y_reference = VV_SVG_PAGE_HEIGHT / 2):
	global nb_layers

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# scales the path centrally =
	index = 0
	# percentage depends on the color so that first the brightest layers are scaled
	# color_percentage = (local_scaling_factor / (nb_layers - 1)) * local_path_no

	# percentage scaled by the number of layers so that black is between 0 and 1, dark gray between 1 an 2...
	# or before scaling 0-1/nb-layers scales the brightest layer from 0 to 1, 
	# 1/nb-layers to 2/nb-layers scales
	color_percentage = local_scaling_factor * nb_layers

	# number of layers as a distance from the darkest layer (the last one)
	path_from_black = nb_layers - 1 - local_path_no

	# the current layer is scaled between path_from_black (path_from_black/nb_layers before scaling)
	# and path_from_black + 1 ((path_from_black + 1)/nb_layers before scaling)
	# for lower percentages, the layer is not scaled and for higher, the layer is fully scaled
	if(color_percentage >= path_from_black and color_percentage <= path_from_black + 1) :
		color_percentage = color_percentage - path_from_black
	
	elif(color_percentage > path_from_black + 1) :
		color_percentage = 1
	
	elif(color_percentage < path_from_black) :
		color_percentage = 0
	
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_reference + color_percentage * (force_num(local_path_data[index]) - x_reference)
				local_path_data[index + 1] = y_reference + color_percentage * (force_num(local_path_data[index + 1]) - y_reference)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					control_point_rank = int(math.floor((i - index)/2)) + 1
					# quicker scaling for the control points
					exponential_percentage = color_percentage
					if(control_point_rank == 1 or control_point_rank == 2) :
						exponential_percentage = color_percentage ** 3
					local_path_data[i] = x_reference + exponential_percentage * (force_num(local_path_data[i]) - x_reference)
					local_path_data[i + 1] = y_reference + exponential_percentage * (force_num(local_path_data[i + 1]) - y_reference)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("C_SCALE_BY_LAYERS_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# SCALE TRANSFORMATION
def apply_scaling(x, y, x_center, y_center, factor) :
	x_ret = x_center + factor * (x - x_center)
	y_ret = y_center + factor * (y - y_center)
	return [x_ret, y_ret]

def SCALE_transformation(local_path_data_string, local_scaling_factor, x_reference = VV_SVG_PAGE_WIDTH / 2, y_reference = VV_SVG_PAGE_HEIGHT / 2):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] , local_path_data[index + 1] = apply_scaling(force_num(local_path_data[index]) , force_num(local_path_data[index + 1]), x_reference, y_reference, local_scaling_factor)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] , local_path_data[i + 1] = apply_scaling(force_num(local_path_data[i]) , force_num(local_path_data[i + 1]), x_reference, y_reference, local_scaling_factor)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("SCALE_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# SCALE_TO_CIRCLE TRANSFORMATION
def apply_scaling_to_circle(x, y, x_center, y_center, factor, final_radius) :
	# from x to x_center + final_radius * (x - x_center)
	R0 =  sqrt((x - x_center) * (x - x_center) + (y - y_center) * (y - y_center))
	if R0 != 0 :
		current_radius = (1 - factor) + factor * (final_radius / R0)
	else :
		current_radius = 0
	x_ret = x_center + (x - x_center) *  current_radius
	y_ret = y_center + (y - y_center) *  current_radius
	return [x_ret, y_ret]

def SCALE_TO_CIRCLE_transformation(local_path_data_string, local_scaling_factor, x_reference = VV_SVG_PAGE_WIDTH / 2, y_reference = VV_SVG_PAGE_HEIGHT / 2, final_radius = VV_SVG_PAGE_HEIGHT / 100):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] , local_path_data[index + 1] = apply_scaling_to_circle(force_num(local_path_data[index]) , force_num(local_path_data[index + 1]), x_reference, y_reference, local_scaling_factor, final_radius)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] , local_path_data[i + 1] = apply_scaling_to_circle(force_num(local_path_data[i]) , force_num(local_path_data[i + 1]), x_reference, y_reference, local_scaling_factor, final_radius)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("SCALE_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# SYMMETRY TRANSFORMATION
def SYMMETRY_transformation(local_path_data_string, x_center, y_center):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				if(x_center >= 0) :
					local_path_data[index] = 2 * x_center - force_num(local_path_data[index])
				if(y_center >= 0) :
					local_path_data[index + 1] = 2 * y_center - force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					if(x_center >= 0) :
						local_path_data[i] = 2 * x_center - force_num(local_path_data[i])
					if(y_center >= 0) :
						local_path_data[i + 1] = 2 * y_center - force_num(local_path_data[i + 1])
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("SYMMETRY_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# SPIRAL
def ARCHIMEDE_SPIRAL_transformation(spiral_center_x, spiral_center_y, spiral_angle_step, spiral_radius_step):
	x = spiral_center_x
	y = spiral_center_y
	path_data_string = "M "+str(spiral_center_x)+" "+str(spiral_center_y)+" "
	for i in range(spiral_nb_steps):
		angle = i * spiral_angle_step
		x = spiral_center_x + (spiral_radius_step * (angle/360)) * cos(radians(angle))
		y = spiral_center_y + (spiral_radius_step * (angle/360)) * sin(radians(angle))
		path_data_string = path_data_string + "L "+str(x)+" "+str(y)+" "
	
	return path_data_string

##################################################################
# CONCENTRIC POLYGONES
def C_POLYGONS_transformation(spiral_center_x, spiral_center_y, spiral_angle_step, spiral_radius_step):
	x = spiral_center_x
	y = spiral_center_y
	nb_steps_per_polygon = int(360/spiral_angle_step)
	path_data_string = "M "+str(spiral_center_x)+" "+str(spiral_center_y)+" "
	for i in range(spiral_nb_steps):
		angle = i * spiral_angle_step
		x = spiral_center_x + (spiral_radius_step * (int(math.floor(angle/360))+1)) * cos(radians(angle))
		y = spiral_center_y + (spiral_radius_step * (int(math.floor(angle/360))+1)) * sin(radians(angle))
		path_data_string = path_data_string + "L "+str(x)+" "+str(y)+" "
		# closes the polygon
		if(i % nb_steps_per_polygon == nb_steps_per_polygon - 1) :
			x = spiral_center_x + (spiral_radius_step * (int(math.floor(angle/360))+1)) * cos(radians(angle+spiral_angle_step))
			y = spiral_center_y + (spiral_radius_step * (int(math.floor(angle/360))+1)) * sin(radians(angle+spiral_angle_step))
			path_data_string = path_data_string + "L "+str(x)+" "+str(y)+" "
		
	return path_data_string

##################################################################
# COPY TRANSFORMATION AND STORES TOP LEFT
# copies the coordinates of the top left point of the drawing
def COPY_TOP_LEFT_transformation() :
	# translates the drawing to the top left
	FILE_log.write(VV_SVG_PAGE_WIDTH,",",VV_SVG_PAGE_HEIGHT,",",int(min_abs_X),",",int(max_abs_Y),",",int(max_abs_X),",",int(min_abs_Y),"\n")

##################################################################
# ADDS LINE RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
def C_LINES_transformation(local_path_data_string, local_percent_transf):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# coordinates of the curve points from which start the lines
	line_starting_points = []
	# scales the path centrally =
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "L") :
			index += 1
			# processes the next lineto: pushes the end point(s)
			while(True) :
				line_starting_points.append(force_num(local_path_data[index]))
				line_starting_points.append(force_num(local_path_data[index + 1]))
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "M") :
			index += 1
			# processes the next moveto: nothing to be done
			while(True) :
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			# pushes only the end point(s), not the control points
			while(True) :
				line_starting_points.append(force_num(local_path_data[index + 4]))
				line_starting_points.append(force_num(local_path_data[index + 5]))
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("C_LINES_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	# screen center
	x_reference = VV_SVG_PAGE_WIDTH / 2
	y_reference = VV_SVG_PAGE_HEIGHT / 2
	# points selected on curves as candidates for lines
	number_of_starting_points = int(len(line_starting_points)/2)
	# the number of lines increases with percentage
	number_of_lines = int(number_of_starting_points * local_percent_transf)
	# how many points between two connected points to the center
	intervals_between_lines = 100000
	if(number_of_lines > 0 ) :
		intervals_between_lines = int(number_of_starting_points/number_of_lines)
	
	if(intervals_between_lines < 1 ) :
		intervals_between_lines = 1
	
	ind_point = 1
	local_beam_path_data = []
	while(len(line_starting_points) > 0) :
		line_X = line_starting_points.pop(0)
		line_Y = line_starting_points.pop(0)
		if((ind_point % intervals_between_lines) == 0) :
			local_beam_path_data.append("M")
			local_beam_path_data.append(str(x_reference))
			local_beam_path_data.append(str(y_reference))
			local_beam_path_data.append("L")
			local_beam_path_data.append(str(line_X))
			local_beam_path_data.append(str(line_Y))
		ind_point += 1
	local_beam_path_data.append("Z")

	separator = ' '
	return separator.join(map(str, local_beam_path_data))

##################################################################
# CREATE BEZIER CURVES BETWEEN TWO IMAGES (THEIR PATHS)
# extract indices for endpoints of links, returns the indices of these points and
def extract_half_image_bezier_end_points(left_half, image_axis, local_path_data, local_path_data_size) :
	index = 0
	return_cur_prev_next = []
	while(index < len(local_path_data)) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				# is on the expected half of the image and is an endpoint of 2 consecutive Bezier curves
				if(left_half == (force_num(local_path_data[index + 4]) > image_axis) and re.search(r'^[MLZz]',local_path_data[index + 6]) == None) :
					if(re.search(r'^[MCLZz]', local_path_data[index + 6]) != None) : # followed by another tag
						return_cur_prev_next.append([index + 4, index + 2, index + 7])
					else : # continued by another bezier
						return_cur_prev_next.append([index + 4, index + 2, index + 6])
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			separator = ' '
			print ("Line: ", separator.join(map(str, local_path_data)))
			print ("LINKS_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""
	return return_cur_prev_next

def extract_all_image_bezier_end_points(local_path_data, local_path_data_size) :
	index = 0
	return_cur_prev_next = []
	while(index < len(local_path_data)) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				# is on the expected half of the image and is an endpoint of 2 consecutive Bezier curves
				if(re.search(r'^[MLZz]',local_path_data[index + 6]) == None) :
					if(re.search(r'^[MCLZz]', local_path_data[index + 6]) != None) : # followed by another tag
						return_cur_prev_next.append([index + 4, index + 2, index + 7])
					else : # continued by another bezier
						return_cur_prev_next.append([index + 4, index + 2, index + 6])
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			separator = ' '
			print ("Line: ", separator.join(map(str, local_path_data)))
			print ("LINKS_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""
	return return_cur_prev_next

def extract_by_intervals(size, nb_points, points_selected_at_preceding_pass) :
	interval = int(math.floor(size / (nb_points + 1)))
	interval = max(1, interval)
	count_point = 0
	return_index_list = []
	for i in range(0, size, interval) :
		chosen_i = i
		while((chosen_i < i + interval - 1) or (chosen_i in points_selected_at_preceding_pass)) :
			chosen_i = chosen_i + 1
		return_index_list.append(chosen_i % size)
		count_point += 1
		if(count_point == nb_points) :
			break
	if(count_point < nb_points) :
		for i in range(count_point, nb_points) :
			return_index_list.append(int(random.random() * size) % size)
	return return_index_list

def path_point(index, path_data) :
	return[force_num(path_data[index]), force_num(path_data[index + 1])]

def norm(vector) :
	return math.sqrt(vector[0]*vector[0] + vector[1]*vector[1])

def normalize(vector) :
	vec_norm = norm(vector)
	if vec_norm != 0:
		return [x / vec_norm for x in vector]
	return vector

def point_translation(point, normed_vector, length) :
	translation_vector =  [x * length for x in normed_vector]
	return list(map(operator.add, point, translation_vector))

def end_point_normal(P3, P4, P5) :
	tang1 = list(map(operator.sub, P4, P3))
	tang2 = list(map(operator.sub, P4, P5))
	tang1 = normalize( tang1 )
	tang2 = normalize( tang2 )
	normal = list(map(operator.add, tang1, tang2))
	return normalize(normal)

def link_path(P0, P1, P2, P3, local_stroke_color_links) :
	return "<path d=\"M " + str(P0[0]) + " " + str(P0[1]) + " C " + str(P1[0]) + " " + str(P1[1]) + " " + str(P2[0]) + " " + str(P2[1]) + " " + str(P3[0]) + " " + str(P3[1]) \
		+ '" style="fill:#000000; fill-opacity:0; stroke:'+local_stroke_color_links+'; stroke-opacity:1;" />'

def LINKS_transformation(current_path_data_string, current_line_and_paths_rank, preceding_line_and_paths_rank, \
	current_firstpass_translation, current_firstpass_scale, current_firstpass_nb_frames, current_firstpass_percent_life, \
	current_path_rank, local_percent_transf, current_firstpass_group_no):
	current_path_data = re.split(r' +' , current_path_data_string.rstrip())
	current_path_data_size = len(current_path_data)

	global transformed_SVG_file_line_out
	global number_of_paths
	global chapter_no
	global percent_links
	global stroke_color_links

	# print("percent_links", percent_links)

	# list of curve endpoints used for the first set of link (with preceding layer)
	global LINKS_first_pass_halfpoints

	val_current_firstpass_nb_frames = int(force_num(current_firstpass_nb_frames))
	val_current_firstpass_percent_life = force_num(current_firstpass_percent_life)

	# no link during the first frames of a new layer before flashing 
	if(chapter_no == 0 and val_current_firstpass_nb_frames < VV_NB_FRAMES_BEFORE_CAPTURE - 5) :
		return [current_path_data_string, []]

	# first layer: no other layer to which to be linked
	if(preceding_line_and_paths_rank < 0) :
		return [current_path_data_string, []]

	# accesses the data of the preceding path to which it is linked
	# print("index :", str(preceding_line_and_paths_rank), " taille :",str(len(transformed_SVG_file_line_out)))
	preceding_line_string, preceding_path_data_string, preceding_firstpass_translation, preceding_firstpass_scale, \
		preceding_firstpass_nb_frames, preceding_firstpass_percent_life, preceding_firstpass_group_no = transformed_SVG_file_line_out[preceding_line_and_paths_rank]
	preceding_path_data = re.split(r' +' , preceding_path_data_string.rstrip())
	preceding_path_data_size = len(preceding_path_data)

	# group != 1, no link, only between group 1 layers
	if current_firstpass_group_no != 1 or preceding_firstpass_group_no != 1 :
		return [current_path_data_string, []]

	# otherwise links are created between layers and the shapes are modified at the link anchors

	#######################################################
	# PRECEDING AND CURRENT LAYER TRANSLATIONS MADE BEFORE LAYER CROSSFADING (AND IMAGE AXIS CALCULATION)
	# print(preceding_firstpass_translation, "/",preceding_firstpass_scale, "/", preceding_firstpass_percent_life, "/",preceding_firstpass_group_no)

	# retrieves horizontal translation and scaling associated with the current and the preceding path
	preceding_x_translation = force_num(re.split(r'x' , preceding_firstpass_translation)[0])
	preceding_image_axis = VV_SVG_PAGE_WIDTH/2 + preceding_x_translation
	# print("preceding scale :", preceding_firstpass_scale)
	preceding_scale = re.split(r'x' , preceding_firstpass_scale)[2]
	if(preceding_x_translation > 0) :
		preceding_position_left = True
	else :
		preceding_position_left = False

	current_x_translation = force_num(re.split(r'x' , current_firstpass_translation)[0])
	current_image_axis = VV_SVG_PAGE_WIDTH/2 + current_x_translation
	current_scale = re.split(r'x' , current_firstpass_scale)[2]
	if(current_x_translation > 0) :
		current_position_left = True
	else :
		current_position_left = False

	# print("preceding firstpass translation/scale and path", preceding_x_translation, "/", preceding_scale, ": ", preceding_path_data_string[0:100])
	# print("current firstpass translation/scale and path", current_x_translation, "/", current_scale, ": ", current_path_data_string[0:100])

	#######################################################
	# NUMBER OF LINKS CALCULATION ACCORDING TO PERCENTAGE IN SCENE ADVANCE

	# defines the number of links between the two paths according to percentage of their own life (not the scene):
	current_percent_links = percent_links
	if(chapter_no == 0) :
		# with varying number of links: no links until the flash
		# links appear after the flash and getting more active afterwards
		# they increase until it reaches the interpolated value from scenario at the end of the chapter
		current_percent_links = percent_links * val_current_firstpass_percent_life
		# print("scene_percentage clip_percent_links",scene_percentage, " ", clip_percent_links)
	number_of_links = int(VV_MAX_LINK_PATH * current_percent_links)
	number_of_links = max(number_of_links, 0)

	#######################################################
	# POINT DISPLACEMENT LENGTH ACCORDING TO PERCENTAGE IN SCENE ADVANCE AND TO THE SCALE OF THE LAYER
	scale1_point_displacement = VV_POINT_DISPLACEMENT_SCALE * local_percent_transf
	current_point_displacement = force_num(current_scale) * scale1_point_displacement
	preceding_point_displacement = force_num(preceding_scale) * scale1_point_displacement

	#######################################################
	# BEZIER CURVE ENDPOINTS EXTRACTION

	# extracts the indices and coordinates of the extremities of the Bezier curves that are in the chosen half of the image
	# print("axis :", current_image_axis, " path:",current_path_data_string)
	current_half_points = extract_half_image_bezier_end_points(not current_position_left, current_image_axis, current_path_data, current_path_data_size)
	preceding_half_points = extract_half_image_bezier_end_points(not preceding_position_left, preceding_image_axis, preceding_path_data, preceding_path_data_size)

	# size of the endpoints lists
	current_half_points_size = len(current_half_points)
	preceding_half_points_size = len(preceding_half_points)

	# extracts all points whatever their position
	if(current_half_points_size < number_of_links / 2) :
		current_half_points = extract_all_image_bezier_end_points(current_path_data, current_path_data_size)
		# size of the endpoints lists
		current_half_points_size = len(current_half_points)
	if(preceding_half_points_size < number_of_links / 2) :
		preceding_half_points = extract_all_image_bezier_end_points(preceding_path_data, preceding_path_data_size)
		# size of the endpoints lists
		preceding_half_points_size = len(preceding_half_points)

	if(preceding_half_points_size <= 0 or current_half_points_size <= 0) :
		# print("Not found enough half points", preceding_half_points_size, current_half_points_size)
		# print(preceding_path_data[:10])
		# print(current_path_data[:10])
		separator = ' '
		return [separator.join(map(str, current_path_data)), []]

	#######################################################
	# ANCHORING POINT SELECTION

	# select the indices of these points that will be used as the anchors for the links
	indices_selected_at_first_pass = []
	size_list = len(LINKS_first_pass_halfpoints)
	if(size_list > 0) :
		indices_selected_at_first_pass = LINKS_first_pass_halfpoints[size_list -1]
	selected_preceding_half_points = extract_by_intervals(preceding_half_points_size, number_of_links, indices_selected_at_first_pass)

	# selects the points in the preceding path to which the links are anchored
	selected_current_half_points = extract_by_intervals(current_half_points_size, number_of_links, [])
	LINKS_first_pass_halfpoints.append(selected_current_half_points)

	returned_list_of_link_paths_strings = []
	# print("number_of_links :", number_of_links)
	for ind_link in range(number_of_links) :
		# retrieval of indices in path associated with link end points and their preceding and following control points on the curves
		current_index, current_prev_index, current_next_index = current_half_points[selected_current_half_points[ind_link]]
		preceding_index, preceding_prev_index, preceding_next_index = preceding_half_points[selected_preceding_half_points[ind_link] % len(selected_preceding_half_points)]

		# preceding and following control points of the selected end points
		current_P4 = path_point(current_index, current_path_data)
		current_P3 = path_point(current_prev_index, current_path_data)
		current_P5 = path_point(current_next_index, current_path_data)
		preceding_P4 = path_point(preceding_index, preceding_path_data)
		preceding_P3 = path_point(preceding_prev_index, preceding_path_data)
		preceding_P5 = path_point(preceding_next_index, preceding_path_data)

		# mid point between link end points
		mid_point = [(preceding_P4[0] + current_P4[0]) / 2, (preceding_P4[1] + current_P4[1]) / 2]
		# link vector
		link = list(map(operator.sub, preceding_P4, current_P4))
		link_length = norm(link)

		# normal on end points obtained from preceding and following control points
		current_normal = end_point_normal(current_P3, current_P4, current_P5)
		preceding_normal = end_point_normal(preceding_P3, preceding_P4, preceding_P5)
		if(dot(preceding_normal, link) > 0):
			preceding_normal = [x * -1 for x in preceding_normal]
		if(dot(current_normal, link) < 0):
			current_normal = [x * -1 for x in current_normal]

		# displaces the endpoint and its two neighboring control points in the current_path_data
		new_current_P4 = point_translation(current_P4, current_normal, current_point_displacement)
		new_current_P3 = point_translation(current_P3, current_normal, current_point_displacement / 2)
		new_current_P5 = point_translation(current_P5, current_normal, current_point_displacement / 2)

		# updates the current path data
		current_path_data[current_index] = str(new_current_P4[0])
		current_path_data[current_index + 1] = str(new_current_P4[1])
		current_path_data[current_prev_index] = str(new_current_P3[0])
		current_path_data[current_prev_index + 1] = str(new_current_P3[1])
		current_path_data[current_next_index] = str(new_current_P5[0])
		current_path_data[current_next_index + 1] = str(new_current_P5[1])

		# displaces the endpoint and its two neighboring control points in the preceding_path_data
		# displacement is only made if the point was not displaced in the first pass, so that the links
		# that were starting from this point are still well located
		if(not(selected_preceding_half_points[ind_link] in indices_selected_at_first_pass)) :
			new_preceding_P4 = point_translation(preceding_P4, preceding_normal, preceding_point_displacement)
			new_preceding_P3 = point_translation(preceding_P3, preceding_normal, preceding_point_displacement / 2)
			new_preceding_P5 = point_translation(preceding_P5, preceding_normal, preceding_point_displacement / 2)

			# updates the preceding path data
			preceding_path_data[preceding_index] = str(new_preceding_P4[0])
			preceding_path_data[preceding_index + 1] = str(new_preceding_P4[1])
			preceding_path_data[preceding_prev_index] = str(new_preceding_P3[0])
			preceding_path_data[preceding_prev_index + 1] = str(new_preceding_P3[1])
			preceding_path_data[preceding_next_index] = str(new_preceding_P5[0])
			preceding_path_data[preceding_next_index + 1] = str(new_preceding_P5[1])
		else :
			new_preceding_P4 = preceding_P4
			new_preceding_P3 = preceding_P3
			new_preceding_P5 = preceding_P5

		# finds the control points of the link by followong the normal over a fraction of the link length
		link_P0 = new_current_P4
		link_P1 = [new_current_P4[0] + 0.2 * link_length * current_normal[0], new_current_P4[1] + 0.2 * link_length * current_normal[1]]
		link_P2 = [new_preceding_P4[0] + 0.2 * link_length * preceding_normal[0], new_preceding_P4[1] + 0.2 * link_length * preceding_normal[1]]
		link_P3 = new_preceding_P4

		# produces the link <path string
		returned_list_of_link_paths_strings.append(link_path(link_P0, link_P1, link_P2, link_P3, stroke_color_links))

	# stores the data of the preceding path after modification
	# print("index :", str(preceding_line_and_paths_rank), " taille :",str(len(transformed_SVG_file_line_out)))
	separator = ' '
	preceding_path_data_string = separator.join(map(str, preceding_path_data))
	preceding_line_string = "<path d=\"" + preceding_path_data_string + "\"/>"
	transformed_SVG_file_line_out[preceding_line_and_paths_rank] = [preceding_line_string, preceding_path_data_string, preceding_firstpass_translation, preceding_firstpass_scale, \
																	preceding_firstpass_nb_frames, preceding_firstpass_percent_life, preceding_firstpass_group_no]

	separator = ' '
	return [separator.join(map(str, current_path_data)), returned_list_of_link_paths_strings]

##################################################################
# TRANSLATION COPIED FROM A DISPLACEMENT MAP SUCH AS AN OPTICAL FLOW MAP
def DISPLACEMENT_MAP_transformation(local_path_data_string, local_displacement_map_factor, local_percent_transf):
	# print("DISPLACEMENT_MAP_transformation"	, local_displacement_map_factor, local_percent_transf)
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent_transf
	global displacement_map_1
	global displacement_map_2
	global displacement_map_center_translation

	if(local_percent_transf <= 0) :
		return local_path_data_string

	# both displacement maps have the same size, _1_ has x coordinates and _2_ y coordinates
	# the size of the displacement maps is height x width, and they should be accessed by row x column
	# whereas SVG coordinates are first x (column) and y (row)
	displacement_map_height, displacement_map_width = displacement_map_1.shape
	x_ratio = displacement_map_width/VV_SVG_PAGE_WIDTH
	y_ratio = displacement_map_height/VV_SVG_PAGE_HEIGHT
	# print("hd wh dm w h x_ratio y_ratio ",VV_SVG_PAGE_WIDTH, " ", VV_SVG_PAGE_HEIGHT, " ",\
	# 	displacement_map_width, " ", displacement_map_height, " ", x_ratio, " ", y_ratio)
	# return local_path_data_string
	displacement_map_scale = local_percent_transf * local_displacement_map_factor
	print("DISPLACEMENT_MAP_transformation ", displacement_map_scale)
 
	# adds DM directions with weight on the control point rank and percentage of transformation
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				x = force_num(local_path_data[index])
				y = force_num(local_path_data[index + 1])
				x_dm = x * x_ratio + displacement_map_center_translation[0]
				y_dm = y * y_ratio + displacement_map_center_translation[1]
				if(x_dm	>= 0 and x_dm < displacement_map_width and y_dm >= 0 and y_dm < displacement_map_height):
					dm_delta_x = bilinear_interpolation_pixel(x_dm, y_dm, displacement_map_1, displacement_map_1.shape)
					dm_delta_y = bilinear_interpolation_pixel(x_dm, y_dm, displacement_map_2, displacement_map_2.shape)
					local_path_data[index] = x + displacement_map_scale * dm_delta_x
					local_path_data[index + 1] = y + displacement_map_scale * dm_delta_y
				else:
					dm_delta_x = 0
					dm_delta_y = 0
					local_path_data[index] = x
					local_path_data[index + 1] = y
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			while(True) :
				for i in range( index, index + 6, 2) :
					control_point_rank = int(math.floor((i - index)/2)) + 1
					# weight = control_point_rank / 3.0
					x = force_num(local_path_data[i])
					y = force_num(local_path_data[i + 1])
					x_dm = x * x_ratio + displacement_map_center_translation[0]
					y_dm = y * y_ratio + displacement_map_center_translation[1]
					if(x_dm	>= 0 and x_dm < displacement_map_width and y_dm >= 0 and y_dm < displacement_map_height):
						dm_delta_x = bilinear_interpolation_pixel(x_dm, y_dm, displacement_map_1, displacement_map_1.shape)
						dm_delta_y = bilinear_interpolation_pixel(x_dm, y_dm, displacement_map_2, displacement_map_2.shape)
						local_path_data[i] = x + displacement_map_scale * dm_delta_x
						local_path_data[i + 1] = y + displacement_map_scale * dm_delta_y
					else:
						dm_delta_x = 0
						dm_delta_y = 0
						local_path_data[i] = x
						local_path_data[i + 1] = y
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("DISPLACEMENT_MAP_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))



if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
