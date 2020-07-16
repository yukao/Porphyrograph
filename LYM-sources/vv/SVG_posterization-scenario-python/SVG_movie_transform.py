#!/usr/bin/perl 

# use strict
# use warnings

# use POSIX

# use Math::Vector::Real
# use Math::Trig
# use List::Util qw[min max]

# package SVG_movie_transform

# SVG_movie_transform.pm
# paths transformations in a SVG file

# usage
# SVG_movie_transform.pm --format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

import math
import random

import getopt
import sys

import re
import vv_lib


USAGE = '''usage: SVG_movie_transform.pm --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_transform.pm --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation

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
# MAIN SUB
##################################################################
SVG_PAGE_WIDTH = 38400
SVG_PAGE_HEIGHT = 21600

LEFT = 0
RIGHT = 1

percent = 0
absoluteCoords_SVG_line_and_paths = []
number_of_paths = 0

# data dedicated to PAIR_LINKS transformation: the production of links between images
# on different movies that have been crossfaded into a single multilayer image
# list of curve endpoints used for the first set of link (with preceding layer)
pair_links_first_pass_halfpoints = []


def main(main_args) :
	single_transf_strings = []
	single_transfs = []

	COPY = 1
	V_SCALE = 2
	V_SCALE2 = 3
	V_SCALE3 = 4
	C_SCALE = 5
	C_SCALE2 = 6
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
	PAIR_LINKS = 22,

	single_transf_hash = {\
		"COPY" : COPY,\
		"V_SCALE" : V_SCALE,\
		"V_SCALE2" : V_SCALE2,\
		"V_SCALE3" : V_SCALE3,\
		"C_SCALE" : C_SCALE,\
		"C_SCALE2" : C_SCALE2,\
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
		"H_SYMMETRY" : H_SYMMETRY,\
		"V_SYMMETRY" : V_SYMMETRY,\
		"BEZIER_CURVE" : BEZIER_CURVE,\
		"ARCHIMEDE_SPIRAL" : ARCHIMEDE_SPIRAL,\
		"C_POLYGONS" : C_POLYGONS, \
		"PAIR_LINKS" : PAIR_LINKS }

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################
	global percent
	global number_of_paths
	global absoluteCoords_SVG_line_and_paths

	scenario_file_name = ''
	prec_path_line_rank = -1
	x_absolute_translation = 0
	y_absolute_translation = 0
	x_absolute_rotation_center = 0
	y_absolute_rotation_center = 0
	rotation_angle = 0
	x_absolute_scaling_center = 0
	y_absolute_scaling_center = 0
	scaling_factor = 0
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

	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","transformation=","percent=","translation=","rotation=","scale=","bezier_curve=","archimede_spiral="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb-layers"):
			nb_layers = vv_lib.type_force_num(arg)
		#  SVG path transformation
		elif opt in ("--transformation"):
			transf_string = arg
			single_transf_strings = re.split(r'/', transf_string.rstrip())
			for single_transf_string in single_transf_strings :
				if not(single_transf_string in single_transf_hash) :
					print( USAGE )
					print( single_transf_string," does not exist in the single_transf_hash dictionary")
					sys.exit(2)
				single_transfs.append( single_transf_hash[single_transf_string])
		#  SVG path transformation percentage
		elif opt in ("--percent"):
			percent = vv_lib.type_force_num(arg)
			percent = max(min(percent,1),0)
		# SVG path translation
		elif opt in ("--translation") and  TRANSLATE in single_transfs:
			abs_trans = re.split(r'x', arg.rstrip())
			x_absolute_translation = vv_lib.type_force_num(abs_trans[0])
			y_absolute_translation = vv_lib.type_force_num(abs_trans[1])
		# SVG path rotation
		elif opt in ("--rotation") and  ROTATE in single_transfs:
			abs_rot = re.split(r'x', arg.rstrip())
			x_absolute_rotation_center = vv_lib.type_force_num(abs_rot[0])
			y_absolute_rotation_center = vv_lib.type_force_num(abs_rot[1])
			rotation_angle = vv_lib.type_force_num(abs_rot[2])
		# SVG path scaling
		elif opt in ("--scale") and  SCALE in single_transfs:
			abs_scale = re.split(r'x', arg.rstrip())
			x_absolute_scaling_center = vv_lib.type_force_num(abs_scale[0])
			y_absolute_scaling_center = vv_lib.type_force_num(abs_scale[1])
			scaling_factor = vv_lib.type_force_num(abs_scale[2])
		# SVG path control points
		elif opt in ("--bezier_curve") and  BEZIER_CURVE in single_transfs:
			abs_bez = re.split(r'x', arg.rstrip())
			b0_x = vv_lib.type_force_num(abs_bez[0])
			b0_y = vv_lib.type_force_num(abs_bez[1])
			b1_x = vv_lib.type_force_num(abs_bez[2])
			b1_y = vv_lib.type_force_num(abs_bez[3])
			b2_x = vv_lib.type_force_num(abs_bez[4])
			b2_y = vv_lib.type_force_num(abs_bez[5])
			b3_x = vv_lib.type_force_num(abs_bez[6])
			b3_y = vv_lib.type_force_num(abs_bez[7])
		# spiral parameters
		elif opt in ("--archimede_spiral") and  ARCHIMEDE_SPIRAL in single_transfs:
			abs_spir = re.split(r'x', arg.rstrip())
			spiral_center_x = vv_lib.type_force_num(abs_spir[0])
			spiral_center_y = vv_lib.type_force_num(abs_spir[1])
			spiral_radius_step = vv_lib.type_force_num(abs_spir[2])
			spiral_angle_step = vv_lib.type_force_num(abs_spir[3])
			spiral_nb_steps = vv_lib.type_force_num(abs_spir[4])
		else:
			msg = "unhandled option ["+opt+"]"
			assert False, msg
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
	initial_translation = ""
	initial_scale = ""

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
			line_string = re.sub(r'<path.*\sd="([^\"]*)\".*/>', r'<path d="\1" />', line_string)
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
	for line_string in SVG_file_lines :
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		# print("Initial line_string [", line_string, "]")
		if( re.search(r'^<path', line_string) != None):
			search_result = re.search(r'd\=\"(.+)\"', line_string)	
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
				path_data_string = relative_to_absolute_coordinates(path_data_string)

				#storage
				line_string = "<path d=\"" + path_data_string + "\"/>"
				absoluteCoords_SVG_line_and_paths.append( [line_string, path_data_string, initial_translation, initial_scale] )
				number_of_paths += 1
				# print("Absolute coordinates [", line_string, "]")
		else:
			# if it is a group, the parameters of the transformations are stored
			if (re.search(r'<g ', line_string) != None) :
				# parameters of the initial transformation retrieved for the final transformation
				initial_translation = ""
				initial_scale = ""

				# initial transformation: stores the characteristics of the initial transformation
				if(re.search(r' vv-transformation=', line_string) == None) :
					SVG_field_transformations = ""
					for opt, arg in opts:
						if (re.search(r'^--', opt) != None) :
							opt_string = re.sub(r'^--', r'', opt)
							SVG_field_transformations = SVG_field_transformations + "vv-" + opt_string + '="' + str(arg) + '" '
					line_string = line_string.replace(r'<g ', '<g '+SVG_field_transformations)

				# final transformation: retrieves the characteristics of the initial transformation
				else :
					# if(single_transf == PAIR_LINKS) :
					# 	print("group :", line_string)
					search_result = re.search(r' vv-translation="([^\"]+)"', line_string)
					if(search_result != None) :
						initial_translation = search_result.group(1)
						# if(single_transfs[0] == PAIR_LINKS) :
						# 	print("initial_translation :", initial_translation)
					search_result = re.search(r' vv-scale="([^\"]+)"', line_string)
					if(search_result != None) :
						initial_scale = search_result.group(1)
						# if(single_transfs[0] == PAIR_LINKS) :
						# 	print("initial_scale :", initial_scale)

			absoluteCoords_SVG_line_and_paths.append( [line_string, "", initial_translation, initial_scale] )

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	transformed_SVG_file_line_and_paths = []
	nb_transformations = 0
	for single_transf in single_transfs :
		# copies the output of the preceding transformation
		if(nb_transformations > 0) :
			absoluteCoords_SVG_line_and_paths = transformed_SVG_file_line_and_paths
		
		if(single_transf == COPY_TOP_LEFT) :
			log_file_name = input_file_name
			log_file_name = re.sub(r'_[0-9]+\.svg', r'_log.csv', log_file_name)
			try:
				FILE_log = open(log_file_name, "at")
			except IOError:
				print("Log file not appended :", log_file_name )		

		# the list of transformed lines (including non <path lines)
		transformed_SVG_file_line_and_paths = []
		# the  rank of the current line_string in the SVG file before the nth transformation
		line_rank = 0
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
		path_link_data_string_list = []
		for line_path in absoluteCoords_SVG_line_and_paths :
			##################################################################
			# appplies the transformation on each path
			##################################################################
			# print("line_string :[",line_string,"]")
			line_string, path_data_string, previous_initial_translation, previous_initial_scale = line_path
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
					path_data_string = V_SCALE_transformation(path_data_string, path_rank)
				
				##################################################################
				# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
				##################################################################
				elif(single_transf == V_SCALE2) :
					path_data_string = V_SCALE2_transformation(path_data_string, path_rank)
				
				##################################################################
				# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
				# similar to V_SCALE2 but with double length for the first part wrt the second part
				##################################################################
				elif(single_transf == V_SCALE3) :
					path_data_string = V_SCALE3_transformation(path_data_string, path_rank)
				
				##################################################################
				# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
				##################################################################
				elif(single_transf == C_SCALE) :
					path_data_string = C_SCALE_transformation(path_data_string,)
				
				##################################################################
				# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
				##################################################################
				elif(single_transf == C_SCALE2) :
					path_data_string = C_SCALE2_transformation(path_data_string, path_rank)
				
				##################################################################
				# ALL DIRECTIONS RANDOMNESS
				##################################################################
				elif(single_transf == VH_RANDOM) :
					path_data_string = VH_RANDOM_transformation(path_data_string)
				
				##################################################################
				# VERTICAL FLATNESS
				##################################################################
				elif(single_transf == V_FLATTEN) :
					path_data_string = V_FLATTEN_transformation(path_data_string)
				
				##################################################################
				# SMOOTHING AND/OR SIMPLIFICATION OF THE BEZIER CURVES
				##################################################################
				elif(single_transf == SMOOTHING	or single_transf == BLUR) :
					path_data_string = V_FLATTEN_or_BLUR_transformation(path_data_string)
				
				##################################################################
				# PLACES THE TOP LEFT POINT OF THE DRAWING AT THE TOP LEFT POINT OF THE PAGE
				##################################################################
				elif(single_transf == REFRAME_TOP_LEFT) :
					path_data_string = REFRAME_TOP_LEFT_transformation(path_data_string)
				
				##################################################################
				# ADDS RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
				##################################################################
				elif(single_transf == C_LINES) :
					path_beam_data_string = C_LINES_transformation(path_data_string, path_rank)
					# print ("************ Beam path path_beam_data_string\n"
				
				##################################################################
				# TRANSLATES THE FULL IMAGE
				##################################################################
				elif(single_transf == TRANSLATE) :
					path_data_string = TRANSLATE_transformation(path_data_string, x_absolute_translation, y_absolute_translation)
				
				##################################################################
				# ROTATES THE FULL IMAGE
				##################################################################
				elif(single_transf == ROTATE) :
					path_data_string = ROTATE_transformation(path_data_string, x_absolute_rotation_center, y_absolute_rotation_center, rotation_angle)
				
				##################################################################
				# SCALES THE FULL IMAGE
				##################################################################
				elif(single_transf == SCALE) :
					path_data_string = SCALE_transformation(path_data_string, x_absolute_scaling_center, y_absolute_scaling_center, scaling_factor)
				
				##################################################################
				# SYMMETRIES
				##################################################################
				elif(single_transf == H_SYMMETRY) :
					path_data_string = SYMMETRY_transformation(path_data_string,  SVG_PAGE_WIDTH/2, -1)
				
				elif(single_transf == V_SYMMETRY) :
					path_data_string = SYMMETRY_transformation(path_data_string, -1, SVG_PAGE_HEIGHT/2)
				
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
				# CREATE BEZIER CURVES BETWEEN TWO IMAGES (THEIR PATHS)
				##################################################################
				elif(single_transf == PAIR_LINKS) :
					if(prec_path_line_rank >= 0) :
						# print("arguments for PAIR_LINKS_transformation :", path_data_string, "><", line_rank, "><", prec_path_line_rank, "><", previous_initial_translation, "><", previous_initial_scale, "><", path_rank)
						path_link_data_string_list = PAIR_LINKS_transformation(path_data_string, line_rank, prec_path_line_rank, previous_initial_translation, previous_initial_scale, path_rank)					
				
				##################################################################
				# UNKNOWN TRANSFORMATION
				##################################################################
				else :
					print( "Unknown transformation #", single_transf)
					return 0
				
				# if(single_transf in [TRANSLATE,SCALE]) :
				# 	print("path after transformation  #", single_transf, " :", path_data_string)
				# prints the modified path
				line_string = "<path d=\"" + path_data_string + "\"/>"
				transformed_SVG_file_line_and_paths.append([line_string, path_data_string, previous_initial_translation, previous_initial_scale])
				for path_link_data_string in path_link_data_string_list:
					transformed_SVG_file_line_and_paths.append([path_link_data_string, "", previous_initial_translation, previous_initial_scale])
				path_link_data_string_list = []

				# counts the path lines
				path_rank += 1

				# it is a path line, the memory of its rank is kept for the next path line
				prec_path_line_rank = line_rank
			
			##########################################################################
			# non path line reproduced
			##########################################################################
			else :
				##################################################################
				# stores the lines produced by C_LINES transformation
				##################################################################
				if (re.search(r'</svg>', line_string) != None) and (path_beam_data_string != "") :
					transformed_SVG_file_line_and_paths.append( ["<g transform=\"translate(0.000000,2160.000000) scale(0.100000,-0.100000)\" style=\"stroke:url(#radialGradient001); stroke-opacity:1;\">", "", "", ""])
					line_string = "<path d=\"" + path_beam_data_string + "\" id=\"path116\" style=\"stroke:url(#radialGradient001); stroke-opacity:1;\"/>"
					transformed_SVG_file_line_and_paths.append( [line_string, path_beam_data_string, "", ""])
					transformed_SVG_file_line_and_paths.append( ["</g>", "", "", ""] )
				
				# reproducing a non <path line
				transformed_SVG_file_line_and_paths.append( [line_string, "", previous_initial_translation, previous_initial_scale] )

			# loop on all the files of the absoluteCoords_SVG_line_and_paths list
			line_rank += 1

		if(single_transf == COPY_TOP_LEFT) :
			FILE_log.close()
		
		nb_transformations += 1
	
	##########################################################################
	# COPIES THE LINES IN THE OUTPUT FILE
	##########################################################################
	for line_path in transformed_SVG_file_line_and_paths :
		line_string, path_data_string, initial_translation, initial_scale = line_path
		# print("transformed line_string (",line_string,")")
		FILEout.write(str(line_string)+"\n")

	FILEin.close()
	FILEout.close()
	return 1


##################################################################
# TRANSFORMS RELATIVE COORDINATES OF A SVG PATH INTO ABSOLUTE ONES
##################################################################
min_abs_X = 1000000
min_abs_Y = 1000000
max_abs_X = -1000000
max_abs_Y = -1000000
absolute_current_point_X = 0
absolute_current_point_Y = 0

def minMaxUpdate() :
	global min_abs_X
	global min_abs_Y
	global max_abs_X
	global max_abs_Y
	global absolute_current_point_X
	global absolute_current_point_Y
	min_abs_X = min(min_abs_X, absolute_current_point_X)
	min_abs_Y = min(min_abs_Y, absolute_current_point_Y)
	max_abs_X = max(max_abs_X, absolute_current_point_X)
	max_abs_Y = max(max_abs_Y, absolute_current_point_Y)

def relative_to_absolute_coordinates(local_path_data_string):
	global min_abs_X
	global min_abs_Y
	global max_abs_X
	global max_abs_Y
	global absolute_current_point_X
	global absolute_current_point_Y

	# print("relative_to_absolute_coordinates [", local_path_data_string, "]")
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	index = 0
	while( index < local_path_data_size) :
		if(local_path_data[index] == "M") :
			local_path_data[index] = "M"
			index += 1
			# processes the next moveto and possible following lineto
			while(True) :
				absolute_current_point_X = vv_lib.type_force_num(local_path_data[index])
				absolute_current_point_Y = vv_lib.type_force_num(local_path_data[index + 1])
				minMaxUpdate()
				index += 2
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "m") :
			# the first move even if relative (m), has absolute coordinates as first coordinates
			# possibly following moves have relative coordinates
			first_move = 0
			if(index == 0) :
				first_move = 1
			

			local_path_data[index] = "M"
			index += 1
			# processes the next moveto and possible following lineto
			first_loop = 1
			while(True) :
				if(first_move and first_loop) :
					absolute_current_point_X = vv_lib.type_force_num(local_path_data[index])
					absolute_current_point_Y = vv_lib.type_force_num(local_path_data[index + 1])
				
				else :
					local_path_data[index] = absolute_current_point_X + vv_lib.type_force_num(local_path_data[index])
					local_path_data[index + 1] = absolute_current_point_Y + vv_lib.type_force_num(local_path_data[index + 1])
					absolute_current_point_X = local_path_data[index]
					absolute_current_point_Y = local_path_data[index + 1]
				
				minMaxUpdate()
				index += 2
				first_loop = 0

				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
					
		elif(local_path_data[index] == "L") :
			index += 1
			# processes the next lineto
			while(True) :
				absolute_current_point_X = vv_lib.type_force_num(local_path_data[index])
				absolute_current_point_Y = vv_lib.type_force_num(local_path_data[index + 1])
				minMaxUpdate()
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "l") :
			local_path_data[index] = "L"
			index += 1
			# processes the next lineto
			while(True) :
				local_path_data[index] = absolute_current_point_X + vv_lib.type_force_num(local_path_data[index])
				local_path_data[index + 1] = absolute_current_point_Y + vv_lib.type_force_num(local_path_data[index + 1])
				absolute_current_point_X = local_path_data[index]
				absolute_current_point_Y = local_path_data[index + 1]
				minMaxUpdate()
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				absolute_current_point_X = vv_lib.type_force_num(local_path_data[index + 4])
				absolute_current_point_Y = vv_lib.type_force_num(local_path_data[index + 5])
				minMaxUpdate()
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "c") :
			local_path_data[index] = "C"
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range(index, index + 6, 2) :
					local_path_data[i] = absolute_current_point_X + vv_lib.type_force_num(local_path_data[i])
					local_path_data[i + 1] = absolute_current_point_Y + vv_lib.type_force_num(local_path_data[i + 1])
				
				absolute_current_point_X = local_path_data[index + 4]
				absolute_current_point_Y = local_path_data[index + 5]
				minMaxUpdate()
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		# horizontal line h/H x and vertical line v/V y, are transformed into a curve 
		# prec_point middle middle next_point
		elif(local_path_data[index] == "H" or local_path_data[index] == "h"
			or local_path_data[index] == "V" or local_path_data[index] == "v") :
			SVG_path_tag = local_path_data[index]
			# definition of the replacing line segment
			local_path_data[index] = "L"
			index += 1
			while(True) :
				# coordinates of new point
				if(SVG_path_tag == "H" or SVG_path_tag == "h") :
					if(SVG_path_tag == "h") :
						absolute_current_point_X = absolute_current_point_X + vv_lib.type_force_num(local_path_data[index])
					
					else :  # if(SVG_path_tag == "H")
						absolute_current_point_X = vv_lib.type_force_num(local_path_data[index])
					
				
				else : # if(SVG_path_tag == "V" or SVG_path_tag == "v")
					if(SVG_path_tag == "v") :
						absolute_current_point_Y = absolute_current_point_Y + vv_lib.type_force_num(local_path_data[index])
					
					else :  # if(SVG_path_tag == "V")
						absolute_current_point_Y = vv_lib.type_force_num(local_path_data[index])
					
				

				# h delta -> L absolute_current_point_X absolute_current_point_Y 
				# -> insertion of 1 value inside the array of the path
				for i in range(local_path_data_size, index + 1, -1) :
					local_path_data[i] = local_path_data[i - 1]
				
				local_path_data_size += 1

				# coordinates of the line segment end point
				local_path_data[index] = absolute_current_point_X
				local_path_data[index + 1] = absolute_current_point_Y
				minMaxUpdate()
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print( "Line: ", local_path_data_string)
			print( "relative_to_absolute_coordinates Unexpected sign at index "+str(index)+": ("+str(local_path_data[index])+")")
			sys.exit(2)

	# printf("Image min (min_abs_X,min_abs_Y) max (max_abs_X,max_abs_Y)\n")
	separator = ' '
	# print("local_path_data :(",separator.join(map(str, local_path_data)),")")
	return separator.join(map(str, local_path_data))

##################################################################
##################################################################
# TRANSFORMATION FUNCTIONS
##################################################################
##################################################################

##################################################################
# VERTICAL SCALING (WITH AS MANY LEVELS AS LAYERS)
def V_SCALE_transformation(local_path_data_string, local_path_no):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)
	
	global percent

	# horizontal base line according to the number of colors
	y_reference = (SVG_PAGE_HEIGHT / (nb_layers - 1)) * local_path_no
	# scales the path vertically according to the number of colors
	index = 0
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = y_reference + (1 - percent) * (vv_lib.type_force_num(local_path_data[index + 1]) - y_reference)
			index += 2 
		else :
			index += 1
		
	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS)
def V_SCALE2_transformation(local_path_data_string, local_path_no):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# arbirary height of the initially visible part
	h_ini = SVG_PAGE_HEIGHT/3
	# height of the whole image
	h_final = SVG_PAGE_HEIGHT
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
	# percent varies from 1 to 0
	index = 0
	local_percent = 0
	local_translation = 0
	local_scale = 0
	# first part, percent from 1 to 0.5, 1-percant from 0 to 0.5, local_percent from 0 to 1
	if((1 - percent) < 0.5) :
		local_percent = (1 - percent) * 2
		local_translation = (1.0 - local_percent) * y_initial_translation + local_percent * y_mid_translation
		local_scale = (1.0 - local_percent) * initial_scalec + local_percent * mid_scale
	# second part, percent from 0.5 to 0. 1-percent from 0.5 to 1, Local percent from 0 to 1
	else :
		local_percent = (1 - percent) * 2 - 1.0
		local_translation = (1.0 - local_percent) * y_mid_translation + local_percent * y_final_translation
		local_scale = (1.0 - local_percent) * mid_scale + local_percent * final_scale
	
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = local_translation + local_scale * vv_lib.type_force_num(local_path_data[index + 1])
			index += 2
		else :
			index += 1
	
	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# VERTICAL SCALING FOR LA TEMPETE (WITH AS MANY LEVELS AS LAYERS) 
# similar to V_SCALE2 but with double length for the first part wrt the second part
def V_SCALE3_transformation(local_path_data_string, local_path_no):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# arbirary height of the initially visible part
	h_ini = SVG_PAGE_HEIGHT/3
	# height of the whole image
	h_final = SVG_PAGE_HEIGHT
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
	# percent varies from 1 to 0
	index = 0
	local_percent = 0
	local_translation = 0
	local_scale = 0
	# first part, percent from 1 to 0.33 (1-percent varies from 0 to 0.66)
	# local_percent varies from 0 to 1
	if((1 - percent) < 0.66) :
		local_percent = (1 - percent) / 0.66 # from 0 to 0.66/0.66
		local_translation = (1.0 - local_percent) * y_initial_translation + local_percent * y_mid_translation
		local_scale = (1.0 - local_percent) * initial_scale + local_percent * mid_scale
	# second part, percent from 0.33 to 0. (1-percent varies from 0.66 to 1.)
	# local_percent varies from 0 to 1
	else :
		local_percent = 1 - (percent / (1 - 0.66)) # from 1 - (1 - 0.66)/(1 - 0.66) = 0 to 1 - (0/(1 - 0.66)) = 1
		local_translation = (1.0 - local_percent) * y_mid_translation + local_percent * y_final_translation
		local_scale = (1.0 - local_percent) * mid_scale + local_percent * final_scale
	
	while(index < local_path_data_size) :
		if(re.search(r'^[MmCcZzLl]', local_path_data[index]) == None) :
			local_path_data[index + 1] = local_translation + local_scale * vv_lib.type_force_num(local_path_data[index + 1])
			index += 2
		else :
			index += 1

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# CENTRAL SCALING (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
def C_SCALE_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# screen center
	x_reference = SVG_PAGE_WIDTH / 2
	y_reference = SVG_PAGE_HEIGHT / 2
	# scales the path centrally =
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_reference + (1 - percent) * (vv_lib.type_force_num(local_path_data[index]) - x_reference)
				local_path_data[index + 1] = y_reference + (1 - percent) * (vv_lib.type_force_num(local_path_data[index + 1]) - y_reference)
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
					weight = control_point_rank / 3.0
					local_path_data[i] = x_reference + (1 - percent * weight) * (vv_lib.type_force_num(local_path_data[i]) - x_reference)
					local_path_data[i + 1] = y_reference + (1 - percent * weight) * (vv_lib.type_force_num(local_path_data[i + 1]) - y_reference)
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
# CENTRAL SCALING FOR LA TEMPETE (WITH AS PROGRESSIVE SCALING ALONG A CURVE)
def C_SCALE2_transformation(local_path_data_string, local_path_no):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# screen center
	x_reference = SVG_PAGE_WIDTH / 2
	y_reference = SVG_PAGE_HEIGHT / 2
	# scales the path centrally =
	index = 0
	# percentage depends on the color
	# color_percentage = (percent / (nb_layers - 1)) * local_path_no
	color_percentage = percent * nb_layers
	path_from_black = nb_layers - 1 - local_path_no
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
				local_path_data[index] = x_reference + color_percentage * (vv_lib.type_force_num(local_path_data[index]) - x_reference)
				local_path_data[index + 1] = y_reference + color_percentage * (vv_lib.type_force_num(local_path_data[index + 1]) - y_reference)
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
					local_path_data[i] = x_reference + exponential_percentage * (vv_lib.type_force_num(local_path_data[i]) - x_reference)
					local_path_data[i + 1] = y_reference + exponential_percentage * (vv_lib.type_force_num(local_path_data[i + 1]) - y_reference)
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("C_SCALE2_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""

	separator = ' '
	return separator.join(map(str, local_path_data))

##################################################################
# ALL DIRECTIONS RANDOMNESS
def VH_RANDOM_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# adds all directions randomness with weight on the control point rank
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = vv_lib.type_force_num(local_path_data[index]) + percent * rand(3000)
				local_path_data[index + 1] = vv_lib.type_force_num(local_path_data[index + 1]) + percent * rand(3000)
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
					local_path_data[i] = vv_lib.type_force_num(local_path_data[i]) + percent * weight * rand(3000)
					local_path_data[i + 1] = vv_lib.type_force_num(local_path_data[i + 1]) + percent * weight * rand(3000)
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
def V_FLATTEN_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# scales the path centrally according to the number of colors
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			defined_y_reference = False
			# processes the next moveto and lineto
			while(True) :
				if(defined_y_reference) :
					local_path_data[index + 1] = y_reference + (1 - percent) * (vv_lib.type_force_num(local_path_data[index + 1]) - y_reference)
				else :
					y_reference = vv_lib.type_force_num([index + 1])
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
						local_path_data[i + 1] = y_reference + (1 - percent * weight) * (vv_lib.type_force_num(local_path_data[i + 1]) - y_reference)
					else :
						y_reference = vv_lib.type_force_num(local_path_data[i + 1])
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

def V_FLATTEN_or_BLUR_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	index = 0
	smooting_percentage = percent

	# simplifies the curve by removing segments according to percentage
	if(local_single_transf == BLUR):
		# a percentage of 1.0 gives a blank image
		if(percent > 0.9) :
			percent = 0.9
		
		simplified_path_data = []
		preserved_multiple_curve_segment
		for ind in range(1, 21, 1) :
			if(percent < ind * 0.05) :
				preserved_multiple_curve_segment = ind + 1
				smooting_percentage = 1 - 20 * (percent - (ind - 1) * 0.05)
				last
		
		while(index < local_path_data_size) :
			if(local_path_data[index] == "M" or local_path_data[index] == "L") :
				simplified_path_data.append(local_path_data[index])
				index += 1
				# copies the next moveto and lineto
				while(True) :
					simplified_path_data.append(vv_lib.type_force_num(local_path_data[index]))
					index += 1
					simplified_path_data.append(vv_lib.type_force_num(local_path_data[index]))
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
							simplified_path_data.append(vv_lib.type_force_num(local_path_data[index]))
							index += 1
					# discards the next curve segments
					else :
						index += 6
					
					curve_rank += 1
				
					if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
						break
			
			elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
				simplified_path_data.append(vv_lib.type_force_num([index]))
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
					vec_In_1In = [vv_lib.type_force_num(local_path_data[index]) - vv_lib.type_force_num(local_path_data[index - 6]), vv_lib.type_force_num(local_path_data[index + 1]) - vv_lib.type_force_num(local_path_data[index - 6 + 1])]
					vec_In_1In_2 = [vv_lib.type_force_num(local_path_data[index - 12]) - vv_lib.type_force_num(local_path_data[index - 6]), vv_lib.type_force_num(local_path_data[index - 12 + 1]) - vv_lib.type_force_num(local_path_data[index - 6 + 1])]
					# and their norms
					norm_In_1In = norm(vec_In_1In)
					norm_In_1In_2 = norm(vec_In_1In_2)
					# the central last control point
					point_In = [vv_lib.type_force_num(local_path_data[index - 6]), vv_lib.type_force_num(local_path_data[index - 6 + 1])]
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
							local_path_data[index - 6 - 2] = (1 - smooting_percentage) * vv_lib.type_force_num(local_path_data[index - 6 - 2]) + smooting_percentage * (point_In[0] + vec_tang_In[0])
							local_path_data[index - 6 - 2 + 1] = (1 - smooting_percentage) * vv_lib.type_force_num(local_path_data[index - 6 - 2 + 1]) + smooting_percentage * (point_In[1] + vec_tang_In[1])
							# the control point after In-1
							local_path_data[index - 6 + 2] = (1 - smooting_percentage) * vv_lib.type_force_num(local_path_data[index - 6 + 2]) + smooting_percentage * (point_In[0] - vec_tang_In[0])
							local_path_data[index - 6 + 2 + 1] = (1 - smooting_percentage) * vv_lib.type_force_num(local_path_data[index - 6 + 2 + 1]) + smooting_percentage * (point_In[1] - vec_tang_In[1])
						
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
	# SVG_PAGE_WIDTH = 38400,
	# SVG_PAGE_HEIGHT = 21600,
	(x_transl, y_transl) = (-min_abs_X, SVG_PAGE_HEIGHT-max_abs_Y) 
	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_transl + vv_lib.type_force_num(local_path_data[index])
				local_path_data[index + 1] = y_transl + vv_lib.type_force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] = x_transl + vv_lib.type_force_num(local_path_data[i])
					local_path_data[i + 1] = y_transl + vv_lib.type_force_num(local_path_data[i + 1])
				
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
def TRANSLATE_transformation(local_path_data_string, x_transl, y_transl):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] = x_transl + vv_lib.type_force_num(local_path_data[index])
				local_path_data[index + 1] = y_transl + vv_lib.type_force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] = x_transl + vv_lib.type_force_num(local_path_data[i])
					local_path_data[i + 1] = y_transl + vv_lib.type_force_num(local_path_data[i + 1])
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
				local_path_data[index] , local_path_data[index + 1] = apply_rotation(vv_lib.type_force_num(local_path_data[index]) , vv_lib.type_force_num(local_path_data[index + 1]), x_center, y_center, angle)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] , local_path_data[i + 1] = apply_rotation(vv_lib.type_force_num(local_path_data[i]) , vv_lib.type_force_num(local_path_data[i + 1]), x_center, y_center, angle)
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
# SCALE TRANSFORMATION
def apply_scaling(x, y, x_center, y_center, factor) :
	x_ret = x_center + factor * (x - x_center)
	y_ret = y_center + factor * (y - y_center)
	return [x_ret, y_ret]

def SCALE_transformation(local_path_data_string, x_center, y_center, factor):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	# print ("\n************ Translation x_transl y_transl\n\n"
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				local_path_data[index] , local_path_data[index + 1] = apply_scaling(vv_lib.type_force_num(local_path_data[index]) , vv_lib.type_force_num(local_path_data[index + 1]), x_center, y_center, factor)
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					local_path_data[i] , local_path_data[i + 1] = apply_scaling(vv_lib.type_force_num(local_path_data[i]) , vv_lib.type_force_num(local_path_data[i + 1]), x_center, y_center, factor)
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
					local_path_data[index] = 2 * x_center - vv_lib.type_force_num(local_path_data[index])
				if(y_center >= 0) :
					local_path_data[index + 1] = 2 * y_center - vv_lib.type_force_num(local_path_data[index + 1])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range( index, index + 6, 2) :
					if(x_center >= 0) :
						local_path_data[i] = 2 * x_center - vv_lib.type_force_num(local_path_data[i])
					if(y_center >= 0) :
						local_path_data[i + 1] = 2 * y_center - vv_lib.type_force_num(local_path_data[i + 1])
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
	# SVG_PAGE_WIDTH = 38400,
	# SVG_PAGE_HEIGHT = 21600,
	FILE_log.write(SVG_PAGE_WIDTH,",",SVG_PAGE_HEIGHT,",",int(min_abs_X),",",int(max_abs_Y),",",int(max_abs_X),",",int(min_abs_Y),"\n")

##################################################################
# ADDS LINE RAYS BETWEEN THE POINTS AND THE CENTER OF THE IMAGE
def C_LINES_transformation(local_path_data_string):
	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	global percent

	# coordinates of the curve points from which start the lines
	line_starting_points = []
	# scales the path centrally =
	index = 0
	while(index < local_path_data_size) :
		if(local_path_data[index] == "L") :
			index += 1
			# processes the next lineto: pushes the end point(s)
			while(True) :
				line_starting_points.append(vv_lib.type_force_num(local_path_data[index]))
				line_starting_points.append(vv_lib.type_force_num(local_path_data[index + 1]))
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
				line_starting_points.append(vv_lib.type_force_num(local_path_data[index + 4]))
				line_starting_points.append(vv_lib.type_force_num(local_path_data[index + 5]))
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
	x_reference = SVG_PAGE_WIDTH / 2
	y_reference = SVG_PAGE_HEIGHT / 2
	# points selected on curves as candidates for lines
	number_of_starting_points = int(len(line_starting_points)/2)
	# the number of lines increases with percentage
	number_of_lines = int(number_of_starting_points * percent)
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
def extract_half_image_control_points(left_half, image_axis, local_path_data, local_path_data_size) :
	index = 0
	return_list_index_x_y = []
	while(index < len(local_path_data)) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			index += 1
			# processes the next moveto and lineto
			while(True) :
				if(left_half == (vv_lib.type_force_num(local_path_data[index]) < image_axis)) :
					return_list_index_x_y.append([index, local_path_data[index], local_path_data[index + 1]])
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				if(left_half == (vv_lib.type_force_num(local_path_data[index + 4]) > image_axis)) :
					return_list_index_x_y.append([index + 4, local_path_data[index + 4], local_path_data[index + 5]])
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			index += 1
		
		else :
			print ("Line: ", local_path_data_string)
			print ("PAIR_LINKS_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ""
	return return_list_index_x_y

def extract_by_intervals(size, nb_points, points_selected_at_preceding_pass) :
	interval = int(math.floor(size / (nb_points + 1)))
	interval = max(1, interval)
	count_point = 0
	return_index_list = []
	for i in range(0, size, interval) 
		chosen_i = i
		while(chosen_i < i + interval - 1 and chosen_i in points_selected_at_preceding_pass)
			chosen_i = chosen_i + 1
		return_index_list .append(chosen_i)
		count_point += 1
		if(count_point == nb_points) :
			break
	if(count_point < nb_points) :
		for i in range(count_point, nb_points) :
			return_index_list.append(int(random.random() * size) % size)
	return return_index_list

def generate_bezier_curve(preceding_x, preceding_y, current_x, current_y) :
	mid_point_x = (preceding_x + current_x) / 2
	return "<path d=\"M " + str(preceding_x) + " " + str(preceding_y) + " C " + str(mid_point_x) + " " + str(preceding_y) + " " + str(mid_point_x) + " " + str(current_y) + " " + str(current_x) + " " + str(current_y) + '" style="fill:#000000; fill-opacity:0; "' + "/>"

def PAIR_LINKS_transformation(current_path_data_string, current_line_and_paths_rank, preceding_line_and_paths_rank, current_initial_translation, current_initial_scale, current_path_rank):
	current_path_data = re.split(r' +' , current_path_data_string.rstrip())
	current_path_data_size = len(current_path_data)

	global absoluteCoords_SVG_line_and_paths
	global number_of_paths
	global percent

	# list of curve endpoints used for the first set of link (with preceding layer)
	global pair_links_first_pass_halfpoints

	# accesses the data of the preceding path to which it is linked
	# print("index :", str(preceding_line_and_paths_rank), " taille :",str(len(absoluteCoords_SVG_line_and_paths)))
	preceding_line_string, preceding_path_data_string, preceding_initial_translation, preceding_initial_scale = absoluteCoords_SVG_line_and_paths[preceding_line_and_paths_rank]
	preceding_path_data = re.split(r' +' , preceding_path_data_string.rstrip())
	preceding_path_data_size = len(preceding_path_data)

	#######################################################
	# PRECEDING AND CURRENT LAYER TRANSLATIONS MADE BEFORE LAYER CROSSFADING (AND IMAGE AXIS CALCULATION)

	# retrieves horizontal translation and scaling associated with the current and the preceding path
	preceding_x_translation = vv_lib.type_force_num(re.split(r'x' , preceding_initial_translation)[0])
	preceding_image_axis = SVG_PAGE_WIDTH/2 + preceding_x_translation
	# print("preceding scale :", preceding_initial_scale)
	preceding_scale = re.split(r'x' , preceding_initial_scale)[2]
	if(preceding_x_translation > 0) :
		preceding_position_left = True
	else :
		preceding_position_left = False

	current_x_translation = vv_lib.type_force_num(re.split(r'x' , current_initial_translation)[0])
	current_image_axis = SVG_PAGE_WIDTH/2 + current_x_translation
	current_scale = re.split(r'x' , current_initial_scale)[2]
	if(current_x_translation > 0) :
		current_position_left = True
	else :
		current_position_left = False

	# print("preceding initial translation/scale and path", preceding_x_translation, "/", preceding_scale, ": ", preceding_path_data_string[0:100])
	# print("current initial translation/scale and path", current_x_translation, "/", current_scale, ": ", current_path_data_string[0:100])

	#######################################################
	# NUMBER OF LINKS CALCULATION ACCORDING TO PERCENTAGE IN SCENE ADVANCE

	# defines the number of links between the two paths according to percentage and the rank:
	# from 1 to 20 for the 1 pair
	# from 1 to 10 for the last pair
	max_link_first_path = 10
	max_link_last_path = 20
	current_max_link_first_path = int(max_link_first_path * percent)
	current_max_link_first_path = int(max_link_first_path * percent)
	if(number_of_paths > 0) :
		number_of_links = current_max_link_first_path + int((current_path_rank / number_of_paths) * (current_max_link_first_path - current_max_link_first_path))
	else :
		number_of_links = (current_max_link_first_path + current_max_link_first_path) / 2
	number_of_links = max(number_of_links, 1)

	#######################################################
	# BEZIER CURVE ENDPOINTS EXTRACTION

	# extracts the indices and coordinates of the extremities of the Bezier curves that are in the chosen half of the image
	# print("axis :", current_image_axis, " path:",current_path_data_string)
	current_half_points = extract_half_image_control_points(not current_position_left, current_image_axis, current_path_data, current_path_data_size)
	preceding_half_points = extract_half_image_control_points(not preceding_position_left, preceding_image_axis, preceding_path_data, preceding_path_data_size)

	#######################################################
	# ANCHORING POINT SELECTION

	# size of the endpoints lists
	current_half_points_size = len(current_half_points)
	preceding_half_points_size = len(preceding_half_points)

	# select the indices of these points that will be used as the anchors for the links
	indices_selected_at_first_pass = []
	size_pair_links_first_pass_halfpoints = len(pair_links_first_pass_halfpoints)
	if(size_pair_links_first_pass_halfpoints > 0)
		indices_selected_at_first_pass = pair_links_first_pass_halfpoints[size_pair_links_first_pass_halfpoints -1]
	selected_preceding_half_points = extract_by_intervals(preceding_half_points_size, number_of_links, indices_selected_at_first_pass)

	# selects the points in the preceding path to which the links are anchored
	selected_current_half_points = extract_by_intervals(current_half_points_size, number_of_links, [])
	pair_links_first_pass_halfpoints.append(selected_current_half_points)

	returned_strings = []
	# print("number_of_links :", number_of_links, " percent:", percent)
	for ind_link in range(number_of_links) :
		current_index, current_x, current_y = current_half_points[selected_current_half_points[ind_link]]
		preceding_index, preceding_x, preceding_y = preceding_half_points[selected_preceding_half_points[ind_link]]
		returned_strings.append(generate_bezier_curve(vv_lib.type_force_num(preceding_x), vv_lib.type_force_num(preceding_y), vv_lib.type_force_num(current_x), vv_lib.type_force_num(current_y)))

	return returned_strings	

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])
