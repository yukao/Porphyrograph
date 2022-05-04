#!/usr/bin/python3 

# vv_transform_interpolation.py
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
from vv_lib import length2
from vv_lib import norm
from vv_lib import interpolate
from vv_lib import is_number

from vv_lib import normalize_file_paths
from vv_lib import absolute_coordinates_file_paths
from vv_lib import elementary_bezier_file_paths
from vv_lib import filled_string_from_curves

from math import sqrt
from math import floor
from math import pi
from math import cos
from math import sin

from vv_const import *

import numpy as np
import cv2 as cv
import os

import csv
import bezier
import itertools

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_transform_interpolation.py --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
vv_transform_interpolation.py --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation
ARGUMENTS:
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
chapter_no = 0
percent_transf = dict()
transformed_SVG_file_line_out = []
added_SVG_file_line_out = []
nb_layers = 0

sound_volume = 0

target_file_name = ''


##################################################################
# INTERPOLATION BETWEEN TWO IMAGES WITH DIFFERENT NUMBER OF LAYERS
# EACH IMAGE PATH IS TURNED INTO A LIST OF ELEMENTARY BEZIER CURVES [M X0 Y0 C X1 Y1 X2 Y2 X3 Y3]
def curve_subdivision(local_curve, local_nb_sub_curves) :
	global transformed_SVG_file_line_out

	nodes = np.asfortranarray([local_curve[0::2], local_curve[1::2]])
	curve = bezier.Curve(nodes, degree=(len(local_curve) // 2 - 1))
	nb_points = local_nb_sub_curves + 1
	positions = list(map(lambda x: x / (nb_points - 1), range(nb_points) ))
	positions_coordinates = list(map(lambda x: curve.evaluate(x), positions ))
	tangent_coordinates = list(map(lambda x: curve.evaluate_hodograph(x), positions ))
	tangent_norms = list(map(lambda x: np.sqrt(np.sum(x**2)), tangent_coordinates ))
	normalized_tangents = []
	for tg, norm in zip(tangent_coordinates, tangent_norms) :
		normalized_tangents.append(np.array(list(map(lambda comp: (comp/norm) if(norm != 0) else comp, tg))))
	returned_curves = []
	tangent_length = curve.length/(3 * local_nb_sub_curves)
	for ind_curve in range(local_nb_sub_curves) :
		point0 = positions_coordinates[ind_curve]
		point3 = positions_coordinates[ind_curve + 1]
		point1 = point0 + tangent_length * normalized_tangents[ind_curve]
		point2 = point3 - tangent_length * normalized_tangents[ind_curve + 1]
		returned_curves.append([point0[0,0], point0[1,0], point1[0,0], point1[1,0], point2[0,0], point2[1,0], point3[0,0], point3[1,0]])
	return returned_curves

def IMAGE_INTERPOLATION_transformation(local_elementary_path, local_path_length, local_ID_data_string, local_style_data_string, local_elementary_target_path, local_target_path_length, local_target_style_data_string, local_percent_transf) :
	# the target path exists (it is not provided if the local_percent_transf is null)
	if local_target_path_length > 0 and local_path_length > 0 :
		# builds the bins that will represent the correspondences between 
		# 1 to n elementary curve(s) in the source and 1 in the target 
		# (if there are more curves in the source than in the target)
		# or between 1 curve in the source and 1 to n in the target otherwise
		# bins are represented as a list of pairs of indices
		# the nth [first,last] pair corresponds to the indices of the curves 
		# in the longest path associated with the nth curve in the shortest path
		longest_path = max(local_path_length, local_target_path_length)
		shortest_path = min(local_path_length, local_target_path_length)
		# print("local_path_length, local_target_path_length", local_path_length, local_target_path_length)
		# if longest_path == local_path_length :
		# 	print("longest SOURCE {0:d} shortest target {1:d}".format( local_path_length, local_target_path_length))
		# else :
		# 	print("longest TARGET {0:d} shortest source {1:d}".format( local_target_path_length, local_path_length))
		bins = []
		if longest_path == shortest_path :
			bins = list(map(lambda x: [x,x], range(longest_path)))
		else :
			nb_curves_per_bins = longest_path // shortest_path
			remaining_curves = longest_path % shortest_path
			# print("nb_curves_per_bins remaining_curves", nb_curves_per_bins, remaining_curves)
			if remaining_curves > shortest_path / 2 :
				if (shortest_path - remaining_curves) > 0 :
					size_of_filled_sequence = shortest_path // (shortest_path - remaining_curves) + 1
				else :
					size_of_filled_sequence = shortest_path - 1
				# print("remaining curves is more than half of shortest: size_of_filled_sequence", size_of_filled_sequence)
				index_in_longest = 0
				remaining_taken = 0
				nb_small_bins = 0
				nb_large_bins = 0
				nb_bins = 0
				for i in range(shortest_path) :
					if i % size_of_filled_sequence == size_of_filled_sequence - 1 or remaining_taken >= remaining_curves :
						bins.append([index_in_longest, index_in_longest + nb_curves_per_bins - 1])
						index_in_longest += nb_curves_per_bins
						nb_small_bins += 1
					else :
						bins.append([index_in_longest, index_in_longest + nb_curves_per_bins])
						index_in_longest += nb_curves_per_bins + 1
						remaining_taken += 1
						nb_large_bins += 1
					nb_bins += 1
				# print("nb_bins nb_small_bins (n) nb_large_bins (n+1)", nb_bins, nb_small_bins, nb_curves_per_bins, nb_large_bins, nb_curves_per_bins + 1)
				# print("High rest: nb bins {0:d} small bins {1:d}x{2:d}={3:d} large bins {4:d}x{5:d}={6:d} total {7:d}/{8:d}".format( nb_bins, nb_small_bins, nb_curves_per_bins, nb_small_bins * nb_curves_per_bins, nb_large_bins, (nb_curves_per_bins + 1), nb_large_bins * (nb_curves_per_bins + 1), nb_small_bins * nb_curves_per_bins + nb_large_bins * (nb_curves_per_bins + 1), longest_path ))
				# print("index_in_longest", index_in_longest)
			else :
				if remaining_curves > 0 :
					size_of_filled_sequence = shortest_path // remaining_curves - 1
				else :
					size_of_filled_sequence = shortest_path - 1
				# print("remaining curves is less than half of shortest: size_of_filled_sequence", size_of_filled_sequence)
				index_in_longest = 0
				remaining_taken = 0
				nb_small_bins = 0
				nb_large_bins = 0
				nb_bins = 0
				for i in range(shortest_path) :
					if i % size_of_filled_sequence == size_of_filled_sequence - 1 and remaining_taken < remaining_curves :
						bins.append([index_in_longest, index_in_longest + nb_curves_per_bins])
						index_in_longest += nb_curves_per_bins + 1
						remaining_taken += 1
						nb_large_bins += 1
					else :
						bins.append([index_in_longest, index_in_longest + nb_curves_per_bins - 1])
						index_in_longest += nb_curves_per_bins
						nb_small_bins += 1
					nb_bins += 1
				# print("nb_bins nb_small_bins (n) nb_large_bins (n+1)", nb_bins, nb_small_bins, nb_curves_per_bins, nb_large_bins, nb_curves_per_bins + 1)
				# print("Low rest: nb bins {0:d} small bins {1:d}x{2:d}={3:d} large bins {4:d}x{5:d}={6:d} total {7:d}/{8:d}".format( nb_bins, nb_small_bins, nb_curves_per_bins, nb_small_bins * nb_curves_per_bins, nb_large_bins, (nb_curves_per_bins + 1), nb_large_bins * (nb_curves_per_bins + 1), nb_small_bins * nb_curves_per_bins + nb_large_bins * (nb_curves_per_bins + 1), longest_path ))
				# print("index_in_longest", index_in_longest)

			if index_in_longest != longest_path :
				print("******* not all curves in longest path are in a bin", index_in_longest, longest_path)
				sys.exit(0)
			if remaining_taken != remaining_curves :
				print("******* not all remaining curves in longest path are in a bin", remaining_taken, remaining_curves)
				sys.exit(0)

		bin_index = 0
		result_curves = []
		for a_bin in bins :
			first_index, last_index = a_bin
			if longest_path == local_path_length :
				# there are more curves in the source path
				# the curves in the target path should be subdivided 
				# according to the number of source curves in each bin
				curves_in_longest_path = local_elementary_path[first_index:last_index + 1]
				if first_index == last_index :
					curves_in_shortest_path = local_elementary_target_path[bin_index:bin_index + 1]
				else :
					curves_in_shortest_path = curve_subdivision(local_elementary_target_path[bin_index], last_index + 1 - first_index)

				# interpolates the corresponding curves and add them to the result
				for source_curve, target_curve in zip(curves_in_longest_path, curves_in_shortest_path) :
					result_curves.append(list(map(lambda x: (1.0 - local_percent_transf ) * x[0] + local_percent_transf * x[1], zip(source_curve, target_curve))))
			else :
				# there are more curves in the target path
				curves_in_longest_path = local_elementary_target_path[first_index:last_index + 1]
				if first_index == last_index :
					curves_in_shortest_path = local_elementary_path[bin_index:bin_index + 1]
				else :
					curves_in_shortest_path = curve_subdivision(local_elementary_path[bin_index], last_index + 1 - first_index)

				# interpolates the corresponding curves and add them to the result
				for source_curve, target_curve in zip(curves_in_shortest_path, curves_in_longest_path) :
					result_curves.append(list(map(lambda x: (1.0 - local_percent_transf) * x[0] + local_percent_transf * x[1], zip(source_curve, target_curve))))

			bin_index += 1
	# one of the paths is empty keeps the other path
	elif local_path_length <= 0 :
		result_curves = local_elementary_target_path
		longest_path = local_target_path_length
	else : # local_target_path_length <= 0
		result_curves = local_elementary_path
		longest_path = local_path_length

	# fills the path by connecting the curves that end and begin at the same point
	group_result_fill = re.search(r'fill:(#[0-9A-F]+);', local_style_data_string)
	fill_color = ""
	if group_result_fill:
		fill_color = group_result_fill.group(1)
	group_target_fill = re.search(r'fill:(#[0-9A-F]+);', local_target_style_data_string)
	target_fill_color = ""
	if group_target_fill:
		target_fill_color = group_target_fill.group(1)
	# print("fill_color target_fill_color", fill_color, target_fill_color)
	if fill_color != "" and target_fill_color != "" :
		new_color = interpolate('hexcolor', fill_color, target_fill_color, 'l', local_percent_transf)
	elif fill_color != "" :
		new_color = fill_color
	elif target_fill_color != "" :
		new_color = target_fill_color
	else :
		new_color = "#000000"
	new_style = "fill:"+new_color+";"
	# print("local_style_data_string local_target_style_data_string local_percent_transf -> new_style", local_style_data_string, local_target_style_data_string, local_percent_transf, new_style)

	returned_path_string = filled_string_from_curves(result_curves, local_ID_data_string, new_style, longest_path)
	return returned_path_string


##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	COPY = 1
	IMAGE_INTERPOLATION = 2
	IMAGE_INTERPOLATION_ONSET_SWITCH = 3
	IMAGE_INTERPOLATION_SOUND_SWITCH = 4
	IMAGE_INTERPOLATION_RANDOM_SWITCH = 5
	IMAGE_INTERPOLATION_PEAK_SWITCH = 6

	single_transf_hash = {\
		"COPY" : COPY,\
		"IMAGE_INTERPOLATION" : IMAGE_INTERPOLATION,\
		"IMAGE_INTERPOLATION_ONSET_SWITCH" : IMAGE_INTERPOLATION_ONSET_SWITCH,\
		"IMAGE_INTERPOLATION_SOUND_SWITCH" : IMAGE_INTERPOLATION_SOUND_SWITCH,\
		"IMAGE_INTERPOLATION_RANDOM_SWITCH" : IMAGE_INTERPOLATION_RANDOM_SWITCH,\
		"IMAGE_INTERPOLATION_PEAK_SWITCH" : IMAGE_INTERPOLATION_PEAK_SWITCH,\
	 }

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################
	global chapter_no
	global percent_transf
	global layer_no
	global nb_layers

	global sound_volume

	global transformed_SVG_file_line_out

	scenario_file_name = ''
	single_transfs = []
	first_frame_input = False
	single_objects_paths = []

	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb_layers=","chapter_no=",\
			"transformation=","percent_transf=","percent_transf1=","sound_volume=","target_file_name="])
	except getopt.GetoptError:
		print("incorrect option in command ", main_args)
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--target_file_name"):
			target_file_name = arg
		elif opt in ("--nb_layers"):
			nb_layers = force_num(arg)
		elif opt in ("--chapter_no"):
			chapter_no = force_num(arg)
		elif opt in ("--transformation"):
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
		elif opt in ("--sound_volume"):
			sound_volume = force_num(arg)
		else:
			msg = "unhandled option ["+opt+"]"
			assert False, msg
			print( main_args )
			print(USAGE)
			sys.exit(2)

	# print("Transformations :", single_transfs)

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

	path_data_string = ""
	elementary_path = []

	##################################################################
	# STORES THE LINES IN AN ARRAY
	##################################################################
	# print("input_file_name", input_file_name)
	contents = FILEin.read()
	SVG_source_file_lines = contents.splitlines()

	##################################################################
	# CSV FILE OF THE TARGET: SELECTS THE UNIQUE PATH
	##################################################################
	non_null_transf = False
	# checks whether there is a real transformation
	transformation_no = 1
	for single_transf in single_transfs :
		if percent_transf[transformation_no] > 0 :
			non_null_transf = True
		transformation_no += 1
	# looks for the target file and its path
	if non_null_transf and (IMAGE_INTERPOLATION in single_transfs or IMAGE_INTERPOLATION_ONSET_SWITCH in single_transfs or IMAGE_INTERPOLATION_SOUND_SWITCH in single_transfs or IMAGE_INTERPOLATION_RANDOM_SWITCH in single_transfs or IMAGE_INTERPOLATION_PEAK_SWITCH in single_transfs) :
		##################################################################
		# OPENS THE TARGET IMAGE
		try:
			FILETargetPath_in = open(target_file_name, "rt")
		except IOError:
			print("Target file not opened :", target_file_name, " or path is incorrect")
		contents = FILETargetPath_in.read()
		SVG_target_file_lines = contents.splitlines()
		FILETargetPath_in.close()

		all_target_ID_data_string = {}
		all_target_style_data_string = {}
		all_elementary_target_path = {}
		all_target_path_length = {}
		path_rank = 0
		for target_line_string in SVG_target_file_lines :
			# the group
			# if the group contains id and style and if the path does not, they can be used for the embedded path(s)
			if( re.search(r'^<g', target_line_string) != None):
				group_path_ID_data_string = None
				group_style_data_string = None
				# only keeps the ID and the style data
				group_result_ID = re.search(r'\sid="([^\"]*)\"', target_line_string)
				if group_result_ID:
					group_path_ID_data_string = group_result_ID.group(1)
				group_result_style = re.search(r'\sstyle="([^\"]*)\"', target_line_string)
				if group_result_style:
					group_style_data_string = group_result_style.group(1)
				# prints the line
				transformed_SVG_file_line_out.append(target_line_string)
			# the path
			elif( re.search(r'^<path', target_line_string) != None):
				# only keeps the ID , the style, and the path data
				# the first two can be obtained from the group
				all_target_ID_data_string[path_rank] = ""
				result_ID = re.search(r'\sid="([^\"]*)\"', target_line_string)
				if not result_ID:
					if group_path_ID_data_string:
						all_target_ID_data_string[path_rank] = group_path_ID_data_string
					else :
						print('Expected ID not found', target_line_string[0:200])
				else :
					all_target_ID_data_string[path_rank] = result_ID.group(1)
				
				all_target_style_data_string[path_rank] = ""
				result_style = re.search(r'\sstyle="([^\"]*)\"', target_line_string)
				if not result_style:
					if group_style_data_string:
						all_target_style_data_string[path_rank] = group_style_data_string
					else :
						print('Expected style not found', target_line_string[0:200])
				else :
					all_target_style_data_string[path_rank] = result_style.group(1)

				result_path = re.search(r'\sd="([^\"]*)\"', target_line_string)
				if not result_path:
				    print('Expected path not found')

				# looks for the length
				result_length = re.search(r'\svv_length="([0-9]+)\"', target_line_string)
				if not result_length:
				    print('Expected length not found')
				    exit(0)

				if all_target_ID_data_string[path_rank] and all_target_style_data_string[path_rank] and result_path and result_length :
					target_path = result_path.group(1)
					all_target_path_length[path_rank] = int(force_num(result_length.group(1)))
					# print("target path:", all_target_path_length[path_rank], target_path)

					if(target_path != '') :
						target_path_data = re.split(r' +' , target_path.rstrip())
						target_path_data_size = len(target_path_data)
					else:
						target_path_data_size = 0
					# turn each path into a list of list of 8 coordinates, one list for each elementary bezier curve
					index = 0
					target_path_nb_curves = 0
					all_elementary_target_path[path_rank] = []
					while( index < target_path_data_size) :
						# move + Bezier
						if(target_path_data[index] == "M" and index + 3 < target_path_data_size and target_path_data[index + 3] == "C") :
							all_elementary_target_path[path_rank].append([force_num(target_path_data[index + 1]), force_num(target_path_data[index + 2]), \
														force_num(target_path_data[index + 4]), force_num(target_path_data[index + 5]), \
														force_num(target_path_data[index + 6]), force_num(target_path_data[index + 7]), \
														force_num(target_path_data[index + 8]), force_num(target_path_data[index + 9])])
							index += 10
							target_path_nb_curves += 1
						# Bezier in the continuation of a preceding curve
						elif(index > 0 and is_number(target_path_data[index])) :
							all_elementary_target_path[path_rank].append([force_num(target_path_data[index - 2]), force_num(target_path_data[index - 1]), \
														force_num(target_path_data[index]), force_num(target_path_data[index + 1]), \
														force_num(target_path_data[index + 2]), force_num(target_path_data[index + 3]), \
														force_num(target_path_data[index + 4]), force_num(target_path_data[index + 5])])
							index += 6
							target_path_nb_curves += 1
						else :
							print("target path data size", target_path_data_size)
							print("M X0 Y0 C X1 Y1 X2 Y2 X3 Y3 expected as only command for target path", target_path_data[index:index + 30])
							sys.exit(0)
					if all_target_path_length[path_rank] != target_path_nb_curves :
						print("unexpected target path length", all_target_path_length[path_rank], target_path_nb_curves)
				path_rank +=  1

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	transformation_no = 1
	transformed_SVG_file_line_out = []
	added_SVG_file_line_out = []
	for single_transf in single_transfs :
		##################################################################
		# appplies the transformation on each path
		##################################################################
		layer_no = 0
		for line_string in SVG_source_file_lines :

			##########################################################################
			# PATH LINE PRODUCED
			##########################################################################
			# the group
			# if the group contains id and style and if the path does not, they can be used for the embedded path(s)
			if( re.search(r'^<g', line_string) != None):
				group_path_ID_data_string = None
				group_style_data_string = None
				# only keeps the ID and the style data
				group_result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
				if group_result_ID:
					group_path_ID_data_string = group_result_ID.group(1)
				group_result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
				if group_result_style:
					group_style_data_string = group_result_style.group(1)
				# prints the line
				transformed_SVG_file_line_out.append(line_string)
			# the path
			elif( re.search(r'^<path', line_string) != None):
				# only keeps the ID , the style, and the path data
				# the first two can be obtained from the group
				path_ID_data_string = ""
				result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
				if not result_ID:
					if group_path_ID_data_string:
						path_ID_data_string = group_path_ID_data_string
					else :
						print('Expected ID not found', line_string[0:200])
				else :
					path_ID_data_string = result_ID.group(1)
				path_style_data_string = ""
				result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
				if not result_style:
					if group_style_data_string:
						path_style_data_string = group_style_data_string
					else :
						print('Expected style not found', line_string[0:200])
				else :
					path_style_data_string = result_style.group(1)
				result_path = re.search(r'\sd="([^\"]*)\"', line_string)
				if not result_path:
					print('Expected path not found', line_string[0:200])

				# looks for the length
				result_length = re.search(r'\svv_length="([0-9]+)\"', line_string)
				if not result_length:
				    print('Expected length not found', line_string[0:200])
				    exit(0)

				if path_ID_data_string and path_style_data_string and result_path :
					source_path = result_path.group(1)
					path_length = int(force_num(result_length.group(1)))
					# print("source path:", path_length, source_path)

					##################################################################
					# INTERPOLATES BETWEEN A PATH OF THE SOURCE FILE AND A PATH OF THE TARGET FILE
					##################################################################
					if single_transf == IMAGE_INTERPOLATION or single_transf == IMAGE_INTERPOLATION_ONSET_SWITCH or single_transf == IMAGE_INTERPOLATION_SOUND_SWITCH or single_transf == IMAGE_INTERPOLATION_RANDOM_SWITCH or single_transf == IMAGE_INTERPOLATION_PEAK_SWITCH :
						# split the paths into commands or numbers
						# print("split the paths into commands or numbers")
						if(source_path != '') :
							path_data = re.split(r' +' , source_path.rstrip())
							path_data_size = len(path_data)
						else:
							path_data_size = 0

						# turn each path into a list of list of 8 coordinates, one list for each elementary bezier curve
						index = 0
						path_nb_curves = 0
						elementary_path = []
						while( index < path_data_size) :
							# move + Bezier
							if(path_data[index] == "M" and index + 3 < path_data_size and path_data[index + 3] == "C") :
								elementary_path.append([force_num(path_data[index + 1]), force_num(path_data[index + 2]), \
															force_num(path_data[index + 4]), force_num(path_data[index + 5]), \
															force_num(path_data[index + 6]), force_num(path_data[index + 7]), \
															force_num(path_data[index + 8]), force_num(path_data[index + 9])])
								index += 10
								path_nb_curves += 1
							# Bezier in the continuation of a preceding curve
							elif(index > 0 and is_number(path_data[index])) :
								elementary_path.append([force_num(path_data[index - 2]), force_num(path_data[index - 1]), \
															force_num(path_data[index]), force_num(path_data[index + 1]), \
															force_num(path_data[index + 2]), force_num(path_data[index + 3]), \
															force_num(path_data[index + 4]), force_num(path_data[index + 5])])
								index += 6
								path_nb_curves += 1
							else :
								print("source path data size", path_data_size)
								print("M X0 Y0 C X1 Y1 X2 Y2 X3 Y3 expected as only command for target path", path_ID_data_string, path_data[index:index + 30])
								sys.exit(0)
						if path_length != path_nb_curves :
							print("unexpected source path length", path_length, path_nb_curves)

						if percent_transf[transformation_no] > 0 :
							print("IMAGE_INTERPOLATION_transformation", elementary_path[0:2], path_length, path_ID_data_string, path_style_data_string, all_elementary_target_path[layer_no][0:2], all_target_path_length[layer_no], all_target_style_data_string[layer_no], percent_transf[transformation_no])
							# print("layer", layer_no)
							new_path_data_string = IMAGE_INTERPOLATION_transformation(elementary_path, path_length, path_ID_data_string, path_style_data_string, all_elementary_target_path[layer_no], all_target_path_length[layer_no], all_target_style_data_string[layer_no], percent_transf[transformation_no])
						else :
							# only fills the path without any interpolation
							new_path_data_string = IMAGE_INTERPOLATION_transformation(elementary_path, path_length, path_ID_data_string, path_style_data_string, None, 0, "", 0)

						# adds it to the list of new paths to be added at the end of the file so that they are on top
						transformed_SVG_file_line_out.append(new_path_data_string)

						# counts the path lines
						layer_no += 1
					
					##################################################################
					# UNKNOWN TRANSFORMATION
					##################################################################
					else :
						print( "Unknown transformation #", single_transf)
						return 0
			elif( re.search(r'^</g>', line_string) != None):
				# adds the spike as last layers to keep the 4 layers stable (for fill and stroke)
				transformed_SVG_file_line_out.extend(added_SVG_file_line_out)
				# prints the end group line
				transformed_SVG_file_line_out.append(line_string)
			else :
				# prints any other line
				transformed_SVG_file_line_out.append(line_string)

		transformation_no += 1
	
	##########################################################################
	# COPIES THE LINES IN THE OUTPUT FILE
	##########################################################################
	for line_string in transformed_SVG_file_line_out :
		# print("transformed line_string (",line_string,")")
		FILEout.write(str(line_string)+"\n")

	FILEin.close()
	FILEout.close()
	return 1

##################################################################
##################################################################
# MAIN FUNCTION CALL
##################################################################
##################################################################
if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
