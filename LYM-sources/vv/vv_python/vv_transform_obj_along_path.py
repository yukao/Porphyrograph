#!/usr/bin/python3 

# vv_transform_obj_along_path.py
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

import vv_scenario_movie_generation_from_path

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_transform_obj_along_path.py --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
vv_transform_obj_along_path.py --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation
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
layer_no = 0
nb_layers = 0

scale_x_obj_layer1 = 1
scale_y_obj_layer1 = 1
scale_x_obj_layer2 = 1
scale_y_obj_layer2 = 1
interval_obj_layer1 = 8
interval_obj_layer2 = 8
growth_obj_layer1 = 0.1
growth_obj_layer2 = 0.1

rand_scale_x_obj_layer1 = 0
rand_scale_x_obj_layer2 = 0
rand_rotation_obj_layer1 = 0
rand_rotation_obj_layer2 = 0

sun_x = 0
sun_y = 0

sound_volume = 0

#################################################################
# POSITIONING OBJECTS ALONG A PATH AND ADDITIONAL OBJECTS RELATED TO THESE ONES
##################################################################

##################################################################
# POSITIONING OBJECTS ALONG A PATH 
def store_initial_object_data(local_csv_storage_file_name, selected_paths_IDs, local_interval_between_objects, local_scale_x_obj_layer, local_rand_scale_x_obj_layer, local_rand_rotation_obj_layer, loc_SVG_file_lines) :
	# print("first_frame_input")

	##################################################################
	# CREATES THE CSV FILE FOR OBJECT DATA STORAGE
	##################################################################
	# node positions and normals
	try:
		local_storage_FILEObjectData_out = open(local_csv_storage_file_name, "wt")
	except IOError:
		print("File not opened :", local_csv_storage_file_name, " or path is incorrect")
	  
	local_storage_writeObjectDataCSV = csv.writer(local_storage_FILEObjectData_out, delimiter=',')
	# header
	local_storage_writeObjectDataCSV.writerow(["ID", "style", "number_of_located_objects", "interval_between_objects"])

	# counts the path lines
	local_layer_no = 0

	##################################################################
	# reads the drawing paths and generates the poitions and normals of the objects along the path
	##################################################################
	for line_string in loc_SVG_file_lines :

		##########################################################################
		# PATH LINE RODUCED
		##########################################################################
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
			if result_ID and result_path and result_style :
				if result_ID.group(1) in selected_paths_IDs :
					# counts the path lines
					local_layer_no += 1

					index = min(local_layer_no - 1, len(local_interval_between_objects) - 1)
					interval_between_objects = local_interval_between_objects[index]
					index = min(local_layer_no - 1, len(local_rand_rotation_obj_layer) - 1)
					rand_rotation_obj_layer = local_rand_rotation_obj_layer[index]
					index = min(local_layer_no - 1, len(local_scale_x_obj_layer) - 1)
					scale_x_obj_layer = local_scale_x_obj_layer[index]
					index = min(local_layer_no - 1, len(local_rand_scale_x_obj_layer) - 1)
					rand_scale_x_obj_layer = local_rand_scale_x_obj_layer[index]

					# print("ID:", result_ID.group(1))
					path_data_string = result_path.group(1)
					(total_length, curve_lengths, curve_nodes) = vv_scenario_movie_generation_from_path.path_curves_and_lengths(line_string)
					# print("total_length:", total_length)
					# print("curve_lengths:", curve_lengths[0:10], "...")
					# print("curve_nodes:", curve_nodes[0:10], "...")

					# positions of the objects along the paths and uniformly distributed
					# print("interval_between_objects:", interval_between_objects)
					number_of_objects = int(floor(total_length/interval_between_objects))
					# print("number_of_objects:", number_of_objects)

					# list of curve point coordinates (as many as frames)
					object_points = []
					# list of curve normal coordinates (as many as frames)
					object_normals = []
					# list objects random length
					object_scale_x = []
					# list additional objects
					object_added_through_growth = []

					# length covered by objects
					# starts at half length
					object_length = 0.5 * interval_between_objects
					# sets a distance so that it starts placing the first
					# object at 0.5 * interval_between_objects
					dist_to_last_object = 0.5 * interval_between_objects
					nb_tot_obj_along_curve = 0
					for this_curve_nodes, this_curve_length in zip(curve_nodes, curve_lengths) :
						# calculates the number of objects on this curve
						available_length = dist_to_last_object + this_curve_length
						nb_obj = int(floor(available_length/interval_between_objects))
						if nb_obj > 0 and this_curve_length > 0 :
							# builds the curve that will be used by the bezier lib
							curve = bezier.Curve(this_curve_nodes, degree=3)
							for ind_objet in range(nb_obj) :
								t = (((ind_objet + 1) * interval_between_objects) - dist_to_last_object) / this_curve_length
								# print("t", t)
								if t < 0 or t > 1 :
									print("error t out of bounds:", t)
									t = max(1, min(0, t))
									os.sys.exit(0)
								point_array = curve.evaluate(t)
								tangent_array = curve.evaluate_hodograph(t)
								# tangent normalization
								tangent_array = tangent_array / np.sqrt(np.sum(tangent_array**2))
								# tangent random orientation
								random_orientation = ((random.random() - 0.5) * rand_rotation_obj_layer * pi)
								rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
								tangent_array = np.dot(rotation_matrix, tangent_array)
								# + 90° rotation
								normal_array = np.array([[-tangent_array[1,0]], [tangent_array[0,0]]])
								object_points.extend(list(itertools.chain.from_iterable(point_array.tolist())))
								object_normals.extend(list(itertools.chain.from_iterable(normal_array.tolist())))
								object_scale_x.append(scale_x_obj_layer + ((random.random() - 0.5) * rand_scale_x_obj_layer * scale_x_obj_layer))
								object_added_through_growth.append(0)
								nb_tot_obj_along_curve += 1
							dist_to_last_object = this_curve_length - (nb_obj * interval_between_objects - dist_to_last_object)
							if dist_to_last_object < 0 or dist_to_last_object > interval_between_objects :
								print("error dist_to_last_object out of bounds:", dist_to_last_object)
								t = max(interval_between_objects, min(0, dist_to_last_object))
								os.sys.exit(0)
						else:
							# updates distance to last object
							dist_to_last_object += this_curve_length
							if dist_to_last_object < 0 or dist_to_last_object > interval_between_objects :
								print("error dist_to_last_object out of bounds:", dist_to_last_object)
								t = max(interval_between_objects, min(0, dist_to_last_object))
								os.sys.exit(0)
					# print("number_of_located_objects:", nb_tot_obj_along_curve)
					local_storage_writeObjectDataCSV.writerow([result_ID.group(1), result_style.group(1), "{0:d}".format(nb_tot_obj_along_curve), "{0:f}".format(interval_between_objects)])
					# print("object_points:", object_points[0:10], "...")
					local_storage_writeObjectDataCSV.writerow(object_points)
					# print("object_normals:", object_normals[0:10], "...")
					local_storage_writeObjectDataCSV.writerow(object_normals)
					# print("object_scale_x:", object_scale_x[0:10], "...")
					local_storage_writeObjectDataCSV.writerow(object_scale_x)
					# print("object_added_through_growth:", object_added_through_growth[0:10], "...")
					local_storage_writeObjectDataCSV.writerow(object_added_through_growth)

	local_storage_FILEObjectData_out.close()

##################################################################
# ADDING OBJECTS RELATED TO THE ONES ALREADY IN PLACE
def update_growth_object_data(local_csv_storage_file_name, selected_paths_IDs, local_growth_obj_layer, local_scale_x_obj_layer, local_rand_scale_x_obj_layer, local_rand_rotation_obj_layer, loc_SVG_file_lines) :
	##################################################################
	# COPIES THE CSV FILE FOR OBJECT DATA STORAGE INTO A TEMPORARY FILE
	# AND OVERWRITES THE PREVIOUS VERSION OF THE CSV FILE
	##################################################################
	aux_csv_storage_file_name = re.sub('.csv$', '-aux.csv', local_csv_storage_file_name)
	# print("cp -f " + local_csv_storage_file_name + " " + aux_csv_storage_file_name)
	os.system("cp -f " + local_csv_storage_file_name + " " + aux_csv_storage_file_name)

	# node positions and normals
	try:
		local_FILEObjectData_out = open(local_csv_storage_file_name, "wt")
	except IOError:
		print("File not opened :", local_csv_storage_file_name, " or path is incorrect")

	# node positions and normals
	try:
		local_FILEObjectData_in = open(aux_csv_storage_file_name, "rt")
	except IOError:
		print("File not opened :", aux_csv_storage_file_name, " or path is incorrect")


	local_writeObjectDataCSV = csv.writer(local_FILEObjectData_out, delimiter=',')
	local_readObjectDataCSV = csv.reader(local_FILEObjectData_in, delimiter=',')

	# header
	line = next(local_readObjectDataCSV)
	local_writeObjectDataCSV.writerow(line)

	# counts the path lines
	local_layer_no = 0

	##################################################################
	# reads the drawing paths and generates the poitions and normals of the objects along the path
	##################################################################
	for line_string in loc_SVG_file_lines :

		##########################################################################
		# PATH LINE RODUCED
		##########################################################################
		if( re.search(r'^<path', line_string) != None):
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			if result_ID :
				if result_ID.group(1) in selected_paths_IDs :
					# counts the path lines
					local_layer_no += 1

					index = min(local_layer_no - 1, len(local_growth_obj_layer) - 1)
					growth_obj_layer = local_growth_obj_layer[index]
					index = min(local_layer_no - 1, len(local_rand_rotation_obj_layer) - 1)
					rand_rotation_obj_layer = local_rand_rotation_obj_layer[index]
					index = min(local_layer_no - 1, len(local_scale_x_obj_layer) - 1)
					scale_x_obj_layer = local_scale_x_obj_layer[index]
					index = min(local_layer_no - 1, len(local_rand_scale_x_obj_layer) - 1)
					rand_scale_x_obj_layer = local_rand_scale_x_obj_layer[index]

					# [result_ID.group(1), result_style.group(1), "{0:d}".format(nb_tot_obj_along_curve), "{0:f}".format(interval_between_objects)]
					line = next(local_readObjectDataCSV)
					local_writeObjectDataCSV.writerow(line)
					# object_points
					line = next(local_readObjectDataCSV)
					local_writeObjectDataCSV.writerow(line)
					# object_normals
					line = next(local_readObjectDataCSV)
					local_writeObjectDataCSV.writerow(line)
					# object_scale_x
					line = next(local_readObjectDataCSV)
					local_writeObjectDataCSV.writerow(line)
					# updates object_added_through_growth according to growth
					object_added_through_growth = next(local_readObjectDataCSV)
					nb_objets = int(len(object_added_through_growth))
					nb_growing_objects = int(growth_obj_layer * nb_objets)
					for ind_growing_object in range(nb_growing_objects) :
						# selection of a random index among all the objects
						selected_index = min(int(random.random() * nb_objets), nb_objets - 1)
						# addition of an object
						object_added_through_growth[selected_index] = str(int(force_num(object_added_through_growth[selected_index] )) + 1)
					local_writeObjectDataCSV.writerow(object_added_through_growth)

	local_FILEObjectData_out.close()
	local_FILEObjectData_in.close()

#################################################################
# TRANSFORMATION FUNCTIONS
##################################################################

##################################################################
# STRETCHES THE END OF OBJECTS CHAINS ALONG THE PATH TO EVOKE SUNRISE
# a chain is made of (M x0y0 M x1y1 x2y2 x3y3 M x2y2 M x4y4 x5y5 x6y6 M x5y5 M ...)
# 0      1         2      0     1         2          3         4          5         6     7
# 8      9         10
# M -704.80584 452.67886- M 704.82039 452.66514 *704.13889 451.97141* 704.79129 452.69258 Z 
# M *704.13889 451.97141* M 704.15444 451.95883 *703.19413 450.80391* 704.12334 451.98399 Z 
# M *703.19413 450.80391* M 703.20917 450.79073 *702.20429 449.67455* 703.17909 450.81709 Z 
# M *702.20429 449.67455* M 702.21864 449.66062 *701.15988 448.59873* 702.18994 449.68848 Z 
# M *701.15988 448.59873* M 701.17392 448.58449 *700.49937 447.94777* 701.14585 448.61298 Z 
# M -705.19829 451.99685- M 705.21457 451.98523 *704.28956 450.72350* 705.18201 452.00847 Z 
# M *704.28956 450.72350* M 704.30493 450.71071 *703.59629 449.89026* 704.27419 450.73629 Z 
def SUNRISE_transformation(local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf) :
	global sun_x
	global sun_y

	local_path_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_path_data_size = len(local_path_path_data)
	index = 0
	sunrised_path_line_data = []
	while( index < local_path_path_data_size) :
		# move
		if(local_path_path_data[index] == "M" and index + 3 < local_path_path_data_size and local_path_path_data[index + 3] == "M") :
			sunrised_path_line_data.append(local_path_path_data[index])
			index += 1
			sunrised_path_line_data.append([force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])])
			index += 2
		# path
		elif(local_path_path_data[index] == "M" and ((index + 10 < local_path_path_data_size and local_path_path_data[index + 8] == "M") \
		                                             or index + 8 == local_path_path_data_size) \
												and local_path_path_data[index + 7] == "Z") :
			# last object in the path or last object in a chain
			object_top = [force_num(local_path_path_data[index + 3]), force_num(local_path_path_data[index + 4])]
			next_move = [-1, -1]
			if index + 10 < local_path_path_data_size  and local_path_path_data[index + 8] == "M" :
				next_move = [force_num(local_path_path_data[index + 9]), force_num(local_path_path_data[index + 10])]
			end_of_chain = False
			if length2(next_move, object_top) > 0.001 :
				end_of_chain = True

			first_point = [force_num(local_path_path_data[index + 1]), force_num(local_path_path_data[index + 2])]
			third_point = [force_num(local_path_path_data[index + 5]), force_num(local_path_path_data[index + 6])]
			if length2(first_point, third_point) > 3 :
				print("SUNRISE_transformation non standard object", local_path_path_data[index:(index + 8)])
				sys.exit(0)

			# M
			sunrised_path_line_data.append(local_path_path_data[index])
			# 1st path point
			sunrised_path_line_data.append(first_point)
			# 2nd path point, moved towards the sun if it is the last path in the chain
			if end_of_chain : 
				new_end_x = interpolate("float", force_num(local_path_path_data[index + 3]), sun_x, "l", local_percent_transf)
				new_end_y = interpolate("float", force_num(local_path_path_data[index + 4]), sun_y, "l", local_percent_transf)
				sunrised_path_line_data.append([new_end_x, new_end_y])
			else :
				sunrised_path_line_data.append([force_num(local_path_path_data[index + 3]), force_num(local_path_path_data[index + 4])])
			# 3rd path point
			sunrised_path_line_data.append(third_point)
			# Z
			sunrised_path_line_data.append(local_path_path_data[index + 7])
			index += 8
		# unexpected moves
		# elif(local_path_path_data[index] == "M") :
		# 	sunrised_path_line_data.append(local_path_path_data[index])
		# 	index += 1
		# 	while(True) :
		# 		sunrised_path_line_data.append([force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])])
		# 		index += 2

		# 		if(index >= local_path_path_data_size) or (re.search(r'^[MZz]',local_path_path_data[index]) != None):
		# 			break
		elif(local_path_path_data[index] == "Z") :
			sunrised_path_line_data.append(local_path_path_data[index])
			index += 1
		# non expected move sequence
		elif(local_path_path_data[index] == "M") :
			# print("non expected move sequence")
			index += 1
			while(True) :
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break
		# non expected curve sequence
		elif(local_path_path_data[index] == "C") :
			# print("non expected curve sequence")
			index += 1
			while(True) :
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break
		else :
			print("move or closepath expected as only command for path", local_ID_data_string, local_path_path_data[index:index + 10])
			sys.exit(0)

	# builds the path of the additional paths
	separator = ' '
	# print("Sunrise object path after transformation", sunrised_path_line_data[0:100])
	sunrised_path_line_data_string = separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), sunrised_path_line_data)))
	returned_path_string = '<path id="' + local_ID_data_string + '" style="' + local_style_data_string + '" d="' + sunrised_path_line_data_string + '" />'
	return returned_path_string

##################################################################
# RANDOM POSITION NOISE OF THE POINTS DEFINING OBJECTS + SMALL DRIFT (TRANSLATION FROM SUN (CENTER OF SCREEN) AND ROTATION)
# a chain is made of (M x0y0 M x1y1 x2y2 x3y3 M x2y2 M x4y4 x5y5 x6y6 M x5y5 M ...)
# 0      1         2      0     1         2          3         4          5         6     7
# 8      9         10
# M -704.80584 452.67886- M 704.82039 452.66514 *704.13889 451.97141* 704.79129 452.69258 Z 
# M *704.13889 451.97141* M 704.15444 451.95883 *703.19413 450.80391* 704.12334 451.98399 Z 
# M *703.19413 450.80391* M 703.20917 450.79073 *702.20429 449.67455* 703.17909 450.81709 Z 
# M *702.20429 449.67455* M 702.21864 449.66062 *701.15988 448.59873* 702.18994 449.68848 Z 
# M *701.15988 448.59873* M 701.17392 448.58449 *700.49937 447.94777* 701.14585 448.61298 Z 
# M -705.19829 451.99685- M 705.21457 451.98523 *704.28956 450.72350* 705.18201 452.00847 Z 
# M *704.28956 450.72350* M 704.30493 450.71071 *703.59629 449.89026* 704.27419 450.73629 Z 
def OBJECTS_NOISE_AND_SHRINK_transformation(local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf, local_layer_no) :
	global sun_x
	global sun_y

	global rand_scale_x_obj_layer1
	global rand_scale_x_obj_layer2
	global rand_rotation_obj_layer1
	global rand_rotation_obj_layer2

	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2

	local_path_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_path_data_size = len(local_path_path_data)
	index = 0
	transformed_path_line_data = []
	while( index < local_path_path_data_size) :
		# move
		if(local_path_path_data[index] == "M" and index + 3 < local_path_path_data_size and local_path_path_data[index + 3] == "M") :
			transformed_path_line_data.append(local_path_path_data[index])
			index += 1
			transformed_path_line_data.append([force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])])
			index += 2
		# triangular path
		elif(local_path_path_data[index] == "M" and ((index + 10 < local_path_path_data_size and local_path_path_data[index + 8] == "M") \
		                                             or index + 8 == local_path_path_data_size) \
												and local_path_path_data[index + 7] == "Z") :
			first_point = np.array([force_num(local_path_path_data[index + 1]), force_num(local_path_path_data[index + 2])])
			second_point = np.array([force_num(local_path_path_data[index + 3]), force_num(local_path_path_data[index + 4])])
			third_point =np.array([force_num(local_path_path_data[index + 5]), force_num(local_path_path_data[index + 6])])
			# print("init", list(first_point), list(second_point), list(third_point))
			# if length2(first_point, third_point) > 3 :
			# 	print("OBJECTS_NOISE_AND_SHRINK_transformation non standard object", local_path_path_data[index:(index + 8)])
			# 	sys.exit(0)

			# shrinking: ocnverging towards the center of gravity according to smoothed sound volume
			# new scaling, same randomness as the initial one
			if local_layer_no == 1:
				scale_y_obj = scale_y_obj_layer1
			else:
				scale_y_obj = scale_y_obj_layer2
			weight_center = (1./3.) * (first_point + second_point + third_point)
			new_first_point = weight_center + scale_y_obj * (first_point - weight_center)
			new_second_point = weight_center + scale_y_obj * (second_point - weight_center)
			new_third_point = weight_center + scale_y_obj * (third_point - weight_center)

			############################################################
			# TRANSFORMATIONS APPLIED TO ADD NOISE TO THE TWO BASE ELEMENTS THAT ARE BROUGHT FURTHER AWAY FROM EACH OTHER
			# noise in CRACKS_transformation that should be used as a starting point for noise here
			# translation of the basis of the triangle along the direction defined by these two points
			AC = np.array(new_third_point) - np.array(new_first_point)
			mid_point = 0.5 * (np.array(new_first_point) + np.array(new_third_point))
			random_orientation = (random.random() - 0.5) * pi * local_percent_transf
			random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
			# new scaling, same randomness as the initial one
			if local_layer_no == 1:
				scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
			else:
				scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)
			new_first_point = mid_point + random.random() * scale_x_obj * np.dot(random_rotation_matrix, AC)

			random_orientation = (random.random() - 0.5) * pi * local_percent_transf
			random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
			# new scaling, same randomness as the initial one
			if local_layer_no == 1:
				scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
			else:
				scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)
			new_third_point = mid_point - random.random() * scale_x_obj * np.dot(random_rotation_matrix, AC)

			random_orientation = (random.random() - 0.5) * pi
			random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
			# new scaling, same randomness as the initial one
			if local_layer_no == 1:
				scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
			else:
				scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)
			new_second_point = new_second_point + random.random() * scale_x_obj * np.dot(random_rotation_matrix, AC) * 0.5

			new_first_point = list(new_first_point)
			new_second_point = list(new_second_point)
			new_third_point = list(new_third_point)
			# print("end", new_first_point, new_second_point, new_third_point)

			# M
			transformed_path_line_data.append(local_path_path_data[index])
			# 1st path point
			transformed_path_line_data.append(new_first_point)
			# 2nd path point
			transformed_path_line_data.append(new_second_point)
			# 3rd path point
			transformed_path_line_data.append(new_third_point)
			# Z
			transformed_path_line_data.append(local_path_path_data[index + 7])
			index += 8

		elif(local_path_path_data[index] == "Z") :
			transformed_path_line_data.append(local_path_path_data[index])
			index += 1

		# non expected move sequence
		elif(local_path_path_data[index] == "M") :
			# print("non expected move sequence")
			index += 1
			while(True) :
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break

		# non expected curve sequence
		elif(local_path_path_data[index] == "C") :
			# print("non expected curve sequence")
			index += 1
			while(True) :
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break

		else :
			print("move or closepath expected as only command for path", local_ID_data_string, local_path_path_data[index:index + 10])
			sys.exit(0)

	# builds the path of the additional paths
	separator = ' '
	# print("Sunrise object path after transformation", transformed_path_line_data[0:100])
	transformed_path_line_data_string = separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), transformed_path_line_data)))
	returned_path_string = '<path id="' + local_ID_data_string + '" style="' + local_style_data_string + '" d="' + transformed_path_line_data_string + '" />'
	return returned_path_string

##################################################################
# TRANSFORMS THE ICE (OBJECT CHAINS) BY ADDING CRACK DISTORSION
# translates the two vertices of the base along the line that they define on either side and randomly to evoke thunder
# a chain is made of (M x0y0 M x1y1 x2y2 x3y3 M x2y2 M x4y4 x5y5 x6y6 M x5y5 M ...)
# 0      1         2      0     1         2          3         4          5         6     7
# 8      9         10
# M -704.80584 452.67886- M 704.82039 452.66514 *704.13889 451.97141* 704.79129 452.69258 Z 
# M *704.13889 451.97141* M 704.15444 451.95883 *703.19413 450.80391* 704.12334 451.98399 Z 
# M *703.19413 450.80391* M 703.20917 450.79073 *702.20429 449.67455* 703.17909 450.81709 Z 
# M *702.20429 449.67455* M 702.21864 449.66062 *701.15988 448.59873* 702.18994 449.68848 Z 
# M *701.15988 448.59873* M 701.17392 448.58449 *700.49937 447.94777* 701.14585 448.61298 Z 
# M -705.19829 451.99685- M 705.21457 451.98523 *704.28956 450.72350* 705.18201 452.00847 Z 
# M *704.28956 450.72350* M 704.30493 450.71071 *703.59629 449.89026* 704.27419 450.73629 Z 
def CRACKS_transformation(local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf, local_with_noise, local_layer_no) :
	global rand_scale_x_obj_layer1
	global rand_scale_x_obj_layer2
	global rand_rotation_obj_layer1
	global rand_rotation_obj_layer2

	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2

	global sound_volume

	local_path_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_path_data_size = len(local_path_path_data)
	index = 0
	prec_object_top = None
	prec_new_third_point = None
	object_top = None
	new_third_point = None
	cracked_path_line_data = []

	############################################################
	# TRANSFORMATIONS APPLIED TO ADD NOISE TO THE TWO BASE ELEMENTS THAT ARE BROUGHT FURTHER AWAY FROM EACH OTHER
	# new scaling, same randomness as the initial one
	if local_layer_no == 1:
		scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
	else:
		scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)


	############################################################
	# READS THE PATH AND MAKES TRANSFORMATION: 1st and 3rd point TRANSLATION + POSSIBLE NOISE
	while( index < local_path_path_data_size) :
		# move
		if(local_path_path_data[index] == "M" and index + 3 < local_path_path_data_size and local_path_path_data[index + 3] == "M") :
			index += 1
			move_point = [force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])]
			index += 2
		# path (the vertices (index+1,index+2) and (index+5,index+6) are the basis of the triangle)
		elif(local_path_path_data[index] == "M" and ((index + 10 < local_path_path_data_size and local_path_path_data[index + 8] == "M") \
		                                             or index + 8 == local_path_path_data_size) \
												and local_path_path_data[index + 7] == "Z") :
			# memory of the last two points of the preceding chain
			prec_object_top = object_top
			prec_new_third_point = new_third_point

			# last object in the path or last object in a chain
			first_point = [force_num(local_path_path_data[index + 1]), force_num(local_path_path_data[index + 2])]
			object_top = [force_num(local_path_path_data[index + 3]), force_num(local_path_path_data[index + 4])]
			third_point = [force_num(local_path_path_data[index + 5]), force_num(local_path_path_data[index + 6])]

			# end of chain corresponds to a move away from last object top
			next_move = [-1, -1]
			if index + 10 < local_path_path_data_size  and local_path_path_data[index + 8] == "M" :
				next_move = [force_num(local_path_path_data[index + 9]), force_num(local_path_path_data[index + 10])]
			end_of_chain = False
			if length2(next_move, object_top) > 0.001 :
				end_of_chain = True

			# first and third points (the base of the triangle) are expected to be close to each other
			norm_AB = length2(third_point, first_point)
			# if norm_AB > 4 :
			# 	print("CRACKS_transformation non standard object", local_path_path_data[index:(index + 8)])
			# 	sys.exit(0)

			# translation of the basis of the triangle along the direction defined by these two points
			AB = np.array(third_point) - np.array(first_point)
			translation_direction = np.array([0,0])
			if norm_AB != 0 :
				translation_direction = (AB / norm_AB)
			# length of the object
			object_length = length2(move_point, object_top)
			# value of the translation at the scale of the percentage of transformation timed by volume 
			# the volume is the smoothed envelope of the sound signal
			displacement = (random.random() - 0.5) * object_length * sound_volume * local_percent_transf
			translation = displacement * translation_direction
			new_first_point = np.array(first_point) + translation
			new_third_point = np.array(third_point) + translation

			# if there is an additional noise, these two points are brought away from each other
			# depending on the current noise given by scale_x_obj
			# choice of a random angle and length function of noise
			# the angle is between -PI/2 and Pi/2 from the middle
			if local_with_noise :
				new_mid_point = 0.5 * (np.array(new_first_point) + np.array(new_third_point))
				random_orientation = (random.random() - 0.5) * pi
				random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
				new_first_point = new_mid_point + random.random() * scale_x_obj * np.dot(random_rotation_matrix, AB)
				random_orientation = (random.random() - 0.5) * pi
				random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
				new_third_point = new_mid_point - random.random() * scale_x_obj * np.dot(random_rotation_matrix, AB)

			new_first_point = list(new_first_point)
			new_third_point = list(new_third_point)

			#end of the preceding object if it was not the end of a chain or if it's not the first object
			if prec_object_top != None and prec_new_third_point != None :
				# 2nd path point
				new_prec_object_top = list(np.array(prec_object_top) + translation)
				cracked_path_line_data.append(new_prec_object_top)
				# 3rd path point
				cracked_path_line_data.append(prec_new_third_point)
				# Z
				cracked_path_line_data.append('Z')

			# beggining of the current object
			# M
			cracked_path_line_data.append('M')
			# modified move and modified preceding top
			new_move_point = list(np.array(move_point) + translation)
			cracked_path_line_data.append(new_move_point)
			# M
			cracked_path_line_data.append('M')
			# 1st path point
			cracked_path_line_data.append(new_first_point)
			if end_of_chain :
				#end of the current object
				# 2nd path point
				cracked_path_line_data.append(object_top)
				# 3rd path point
				cracked_path_line_data.append(new_third_point)
				# Z
				cracked_path_line_data.append('Z')
				object_top = None
				new_third_point = None	

			index += 8
		elif(local_path_path_data[index] == "Z") :
			# print("non expected closepath", local_path_path_data[index-10:min(index+100, local_path_path_data_size)])
			#end of the last object
			if object_top != None and new_third_point != None :
				# 2nd path point
				cracked_path_line_data.append(object_top)
				# 3rd path point
				cracked_path_line_data.append(new_third_point)
				# Z
				cracked_path_line_data.append('Z')
				object_top = None
				new_third_point = None	
			cracked_path_line_data.append('Z')
			index += 1
		# non expected move sequence
		elif(local_path_path_data[index] == "M") :
			# print("non expected move sequence", local_path_path_data[index-10:min(index+100, local_path_path_data_size)])
			#end of the last object
			if object_top != None and new_third_point != None :
				# 2nd path point
				cracked_path_line_data.append(object_top)
				# 3rd path point
				cracked_path_line_data.append(new_third_point)
				# Z
				cracked_path_line_data.append('Z')
				object_top = None
				new_third_point = None	
			cracked_path_line_data.append('M')
			index += 1
			while(True) :
				cracked_path_line_data.append([force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])])
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break
		# non expected curve sequence
		elif(local_path_path_data[index] == "C") :
			# print("non expected curve sequence", local_path_path_data[index-10:min(index+100, local_path_path_data_size)])
			#end of the last object
			if object_top != None and new_third_point != None :
				# 2nd path point
				cracked_path_line_data.append(object_top)
				# 3rd path point
				cracked_path_line_data.append(new_third_point)
				# Z
				cracked_path_line_data.append('Z')
				object_top = None
				new_third_point = None	
			cracked_path_line_data.append('C')
			index += 1
			while(True) :
				cracked_path_line_data.append([force_num(local_path_path_data[index]), force_num(local_path_path_data[index + 1])])
				index += 2

				if(index >= local_path_path_data_size) or (re.search(r'^[MCZz]',local_path_path_data[index]) != None):
					break
		else :
			print("move or closepath expected as only command for path", local_ID_data_string, local_path_path_data[index:index + 10])
			sys.exit(0)

	#end of the last object
	if object_top != None and new_third_point != None :
		# 2nd path point
		cracked_path_line_data.append(object_top)
		# 3rd path point
		cracked_path_line_data.append(new_third_point)
		# Z
		cracked_path_line_data.append('Z')
	
	# builds the path of the additional paths
	separator = ' '
	# print("crack object path after transformation", cracked_path_line_data)
	cracked_path_line_data_string = separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), cracked_path_line_data)))
	returned_path_string = '<path id="' + local_ID_data_string + '" style="' + local_style_data_string + '" d="' + cracked_path_line_data_string + '" />'
	return returned_path_string

##################################################################
# ADDING OBJECTS ALONG A PATH
def projection_on_line(line_point_A, line_point_B, source_point_M) :
	norm_AB = length2(line_point_A,line_point_B)
	if norm_AB == 0 :
		return source_point
	AB = np.array(line_point_B) - np.array(line_point_A)
	AB_unit = AB / norm_AB
	AM = np.array(source_point_M) - np.array(line_point_A)
	abscisse_projection_Am = np.dot(AM,AB_unit)
	point_m = np.array(line_point_A) + abscisse_projection_Am * AB_unit
	return point_m

def spike_and_subspikes(local_object_path_line_data, control_point_prev, control_point_cur, control_point_next, local_layer_no, local_number_of_subspikes) :
	global rand_scale_x_obj_layer1
	global rand_scale_x_obj_layer2
	global rand_rotation_obj_layer1
	global rand_rotation_obj_layer2

	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2

	AB = np.array(control_point_cur) - np.array(control_point_prev)
	BC = np.array(control_point_next) - np.array(control_point_cur)
	norm_AB = length2(control_point_prev, control_point_cur)
	norm_BC = length2(control_point_cur, control_point_next)
	if norm_AB == 0 and norm_BC == 0 :
		stablepoint = ['M', control_point_cur, control_point_cur, control_point_cur]
		# print("exception Out", stablepoint)
		return stablepoint
	elif norm_AB == 0 :
		tangent = (BC / norm_BC)
	elif norm_BC == 0 :
		tangent = (AB / norm_AB)
	else :
		tangent = (AB / norm_AB) + (BC / norm_BC)
	norm_tangent = norm(tangent)
	if norm_tangent != 0 :
		tangent = tangent /norm_tangent

	############################################################
	# main spike (the trunk)
	# defines the coordinates of the transformed objects according to these geometrical data
	if local_layer_no == 1:
		rotation_matrix = np.array([[tangent[1], tangent[0]], [-tangent[0], tangent[1]]])
	else:
		rotation_matrix = np.array([[-tangent[1], -tangent[0]], [tangent[0], tangent[1]]])
	# rotation randomness addition
	if local_layer_no == 1:
		rand_rotation_obj_layer = rand_rotation_obj_layer1
	else :
		rand_rotation_obj_layer = rand_rotation_obj_layer2
	# the randomness of this orientation is made smaller by using pi/4 instead of pi
	random_orientation = ((random.random() - 0.5) * rand_rotation_obj_layer * pi / 4)
	random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
	rotation_matrix = np.dot(random_rotation_matrix, rotation_matrix)
	# new scaling, same randomness as the initial one
	# object_scale_x.append(scale_x_obj_layer + ((random.random() - 0.5) * rand_scale_x_obj_layer * scale_x_obj_layer))
	if local_layer_no == 1:
		scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
		scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer1]])
	else:
		scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)
		scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer2]])
	added_object_path_line_data = list(map(lambda x: x if type(x) == str else list(np.add(control_point_cur, np.dot(rotation_matrix, np.dot(scaling_matrix, x)))), local_object_path_line_data))

	############################################################
	# secondary spikes (the branches) > 0 :
	if local_number_of_subspikes > 0 :
		cos_60 = cos(pi/6)
		sin_60 = cos(pi/6)
		rotation_matrix_plus = np.dot(np.array([[cos_60, -sin_60], [sin_60, cos_60]]), rotation_matrix)
		rotation_matrix_minus = np.dot(np.array([[cos_60, sin_60], [-sin_60, cos_60]]), rotation_matrix)
		if local_layer_no == 1:
			translation_unit = scale_x_obj * np.array([tangent[1], -tangent[0]])
		else:
			translation_unit = scale_x_obj * np.array([-tangent[1], tangent[0]])
		for ind_subspike in range(local_number_of_subspikes) :
			scale_x_subobj = scale_x_obj * (local_number_of_subspikes - ind_subspike) / local_number_of_subspikes * 0.3
			scaling_matrix = np.array([[scale_x_subobj, 0], [0, 2 * scale_x_subobj]])
			added_object_path_line_data += list(map(lambda x: x if type(x) == str else list(np.add(control_point_cur + (ind_subspike + 1) * translation_unit, np.dot(rotation_matrix_plus, np.dot(scaling_matrix, x)))), local_object_path_line_data))
			added_object_path_line_data += list(map(lambda x: x if type(x) == str else list(np.add(control_point_cur + (ind_subspike + 1) * translation_unit, np.dot(rotation_matrix_minus, np.dot(scaling_matrix, x)))), local_object_path_line_data))
		# print("2nd", added_object_path_line_data)

	return added_object_path_line_data

def SPIKES_transformation(local_object_data_string, local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf, local_layer_no, local_number_of_subspikes) :
	global sound_volume

	object_path_line_data = objectPathData(local_object_data_string)

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)

	index = 0
	# the current <path/> curve with modified control points to turn the Bezier curves into lines
	spiked_path_line_data = []
	# new spikes added as a new <path/>
	added_spikes_path_line_data = []
	# keeping track of the current and initial points to predict the next control point at the end of a curve
	current_point = [0,0]
	initial_point = [0,0]
	control_point = [0,0] * 4
	new_control_point = [0,0] * 4
	# control points 1 and 2 are projected on the segment of control points 0 and 3 to turn the Bezier curve into a line
	projected_control_point = [0,0] * 4
	while(index < local_path_data_size) :
		if(local_path_data[index] == "M" or local_path_data[index] == "L") :
			spiked_path_line_data.append(local_path_data[index])
			index += 1
			# processes the next moveto and lineto
			while(True) :
				# updates current and initial point. Initial point is used for closepath (zZ)
				if local_path_data[index - 1] == "L" :
					initial_point = current_point
				current_point = [force_num(local_path_data[index]), force_num(local_path_data[index + 1])]

				# stores the data
				spiked_path_line_data.append(current_point)

				# moves forward along the path
				index += 2
			
				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break

		elif(local_path_data[index] == "C") :
			spiked_path_line_data.append(local_path_data[index])
			index += 1
			# processes the next curve segments
			# scaling is only 1/3 and 2/3 for the first and second control point and full for the last one
			while(True) :
				control_point_rank = 1
				for i in range(index, index + 6, 2) :
					# updates first, current and initial point. Initial point is used for closepath (zZ)
					if control_point_rank == 3 :
						current_point = [force_num(local_path_data[i]), force_num(local_path_data[i + 1])]
					if control_point_rank == 1 :
						if local_path_data[index - 1] == "C" :
							initial_point = current_point
							control_point[0] = [force_num(local_path_data[i]), force_num(local_path_data[i + 1])]
						else :
							control_point[0] = control_point[3]
					control_point[control_point_rank] = [force_num(local_path_data[i]), force_num(local_path_data[i + 1])]
					control_point_rank += 1

				# stores the linearized curve
				projected_control_point[1] = projection_on_line(control_point[0], control_point[3], control_point[1])
				projected_control_point[2] = projection_on_line(control_point[0], control_point[3], control_point[2])
				new_control_point[1] = list((1 -local_percent_transf) * np.array(control_point[1]) + local_percent_transf * projected_control_point[1])
				new_control_point[2] = list((1 -local_percent_transf) * np.array(control_point[2]) + local_percent_transf * projected_control_point[2])
				new_control_point[3] = control_point[3]

				for control_point_rank in range(1, 4, 1) :
					spiked_path_line_data.append(new_control_point[control_point_rank])
					control_point_rank += 1

				# moves forward along the path
				index += 6

				# stores the spike
				# 1. looks for the next control point
				if index < local_path_data_size :
					if local_path_data[index] == "C" :
						# uses the first control point of the next curve as next control point
						next_control_point = [force_num(local_path_data[index + 1]), force_num(local_path_data[index + 2])]
					elif local_path_data[index] == "z" or local_path_data[index] == "Z" :
						# uses the initial point as next control point
						next_control_point = initial_point
					elif local_path_data[index] == "L" :
						# uses the first line to point as next control point
						next_control_point = [force_num(local_path_data[index + 1]), force_num(local_path_data[index + 2])]
					elif local_path_data[index] == "M" :
						# basic symmetric control point (also used in the case of moveto)
						next_control_point = list(np.array(control_point[3]) + (np.array(control_point[3]) - np.array(control_point[2])))
					elif index + 1 < local_path_data_size and re.search(r'^[\-0-9\.]+$',local_path_data[index]) != None and re.search(r'^[\-0-9\.]+$',local_path_data[index + 1]) != None :
						# next point on the curve
						next_control_point = [force_num(local_path_data[index]), force_num(local_path_data[index + 1])]
					else :
						print("unexpected command in drawing", local_path_data[index:index+10])
						sys.exit(0)

					added_spikes_path_line_data.extend(spike_and_subspikes(object_path_line_data, control_point[2], control_point[3], next_control_point, local_layer_no, local_number_of_subspikes))

				if(index >= local_path_data_size) or (re.search(r'^[MCLZz]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			# updates current point with initial point
			current_point = initial_point

			# stores the data
			spiked_path_line_data.append(local_path_data[index])

			# moves forward along the path
			index += 1
		
		else :
			print( "Line: ", local_path_data_string)
			print( "SPIKES_transformation Unexpected sign at index "+str(index)+": "+str(local_path_data[index]))
			return ["",""]

	# print("Spikes object path after transformation", spiked_path_line_data[0:100])
	separator = ' '
	spiked_path_line_data_string = separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), spiked_path_line_data)))
	returned_path_string = '<path id="' + local_ID_data_string + '" style="' + local_style_data_string + '" d="' + spiked_path_line_data_string + '" />'
	added_spikes_path_line_data_string = separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), added_spikes_path_line_data)))
	returned_added_spikes_path_string = '<path id="' + local_ID_data_string + '_spikes" style="' + local_style_data_string + '" d="' + added_spikes_path_line_data_string + '" />'
	return  [returned_path_string, returned_added_spikes_path_string]

def SECOND_ORDER_SPIKES_transformation(local_object_data_string, local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf, local_layer_no) :
	global sound_volume

	# number of subspikes connected directly to sound (mostly beat sound)
	number_of_subspikes = int(random.random() * sound_volume * 12)
	# print("number_of_subspikes", number_of_subspikes)
	return SPIKES_transformation(local_object_data_string, local_path_data_string, local_ID_data_string, local_style_data_string, local_percent_transf, local_layer_no, number_of_subspikes)

##################################################################
# ADDING OBJECTS ALONG A PATH
def objectPathData(local_object_data_string) :
	local_object_path_data = re.split(r' +' , local_object_data_string.rstrip())
	local_object_path_data_size = len(local_object_path_data)
	index = 0
	returned_object_path_line_data = []
	while( index < local_object_path_data_size) :
		if(local_object_path_data[index] == "M") :
			returned_object_path_line_data.append(local_object_path_data[index])
			index += 1
			while(True) :
				returned_object_path_line_data.append([force_num(local_object_path_data[index]), force_num(local_object_path_data[index + 1])])
				index += 2

				if(index >= local_object_path_data_size) or (re.search(r'^[Zz]',local_object_path_data[index]) != None):
					break
		elif(local_object_path_data[index] == "Z") :
			returned_object_path_line_data.append(local_object_path_data[index])
			index += 1
		else :
			print("move or closepath expected as only command for object")
			sys.exit(0)
	return 	returned_object_path_line_data

def OBJECT_ALONG_PATH_transformation(local_object_data_string, local_ID_data_string, local_style_data_string, local_layer_no, local_percent_transf, local_number_of_located_objects, local_point_coords_list, local_tangent_coords_list, local_scale_x_list, local_object_added_through_growth):
	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2
	global interval_obj_layer1
	global interval_obj_layer2

	object_path_line_data = objectPathData(local_object_data_string)

	if len(local_point_coords_list) != 2 * local_number_of_located_objects :
		print('unexpected point coordinates size', len(local_point_coords_list), "expected", 2 * local_number_of_located_objects)
	if len(local_tangent_coords_list) != 2 * local_number_of_located_objects :
		print('unexpected point coordinates size', len(local_tangent_coords_list), "expected", 2 * local_number_of_located_objects)
	nb_of_growth_objets = 0
	# builds the path of the additional objects
	returned_path_string = '<path id="' + local_ID_data_string + '_objects" style="' + local_style_data_string + '" d="'
	for ind_added_objet in range(local_number_of_located_objects) :

		# retrieves the position, the orientation, the scaling and the number of additional objects from the CSV strorage file
		curve_point_coords = np.array(list(map(force_num, local_point_coords_list[(2 * ind_added_objet):(2 * ind_added_objet + 2)])))
		curve_tangent_coords_x = force_num(local_tangent_coords_list[(2 * ind_added_objet)])
		curve_tangent_coords_y = force_num(local_tangent_coords_list[(2 * ind_added_objet + 1)])
		scale_x_obj = force_num(local_scale_x_list[ind_added_objet])
		object_added_through_growth = int(force_num(local_object_added_through_growth[ind_added_objet]))

		# defines the coordinates of the transformed objects according to these geometrical data
		if local_layer_no == 1:
			rotation_matrix = np.array([[curve_tangent_coords_x, -curve_tangent_coords_y], [curve_tangent_coords_y, curve_tangent_coords_x]])
			scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer1]])
		else:
			rotation_matrix = np.array([[-curve_tangent_coords_x, curve_tangent_coords_y], [-curve_tangent_coords_y, -curve_tangent_coords_x]])
			scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer2]])
		added_object_path_line_data = list(map(lambda x: x if type(x) == str else np.add(curve_point_coords, np.dot(rotation_matrix, np.dot(scaling_matrix, x))), object_path_line_data))
		separator = ' '
		# we dont change anything because the object is supposed to have its origin at [0,0]
		added_object_path_move_data = curve_point_coords

		# builds the additional path of this object according to its transformed coordinates
		added_object_string = "M {0:.5f} {1:.5f} ".format(added_object_path_move_data[0],added_object_path_move_data[1]) 
		added_object_string += separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), added_object_path_line_data)))
		added_object_string += " "
		returned_path_string += added_object_string

		first_growth_added_objects = False
		if nb_of_growth_objets == 0 and object_added_through_growth > 0 :
			# print("Base object:", added_object_string)
			first_growth_added_objects = True

		# adds possible additional objects and defines them randomly at each frame, contrary to the root object, they will not be stable
		for ind_growth_objet in range(object_added_through_growth) :
			# print("growth_object_number", ind_growth_objet)
			nb_of_growth_objets += 1

			# places the object at the end of the preceding one
			if ind_growth_objet == 0 :
				end_of_added_object_point_coords = added_object_path_line_data[2]
			else :
				end_of_added_object_point_coords = growth_object_path_line_data[2]
			# we dont change anything because the object is supposed to have its origin at [0,0]
			growth_object_path_move_data = end_of_added_object_point_coords

			# defines the new object own size and orientation
			# normal random orientation with respect of the preceding one (but with a half amplitude)
			previous_rotation_matrix = rotation_matrix
			# new scaling
			if local_layer_no == 1:
				rand_rotation_obj_layer = rand_rotation_obj_layer1
			else :
				rand_rotation_obj_layer = rand_rotation_obj_layer2
			# the randomness of this orientation is made smaller by using pi/4 instead of pi
			random_orientation = ((random.random() - 0.5) * rand_rotation_obj_layer * pi / 4)
			random_rotation_matrix = np.array([[cos(random_orientation), -sin(random_orientation)], [sin(random_orientation), cos(random_orientation)]])
			rotation_matrix = np.dot(random_rotation_matrix, previous_rotation_matrix)
			# new scaling, same randomness as the initial one
			# object_scale_x.append(scale_x_obj_layer + ((random.random() - 0.5) * rand_scale_x_obj_layer * scale_x_obj_layer))
			if local_layer_no == 1:
				scale_x_obj = scale_x_obj_layer1 + ((random.random() - 0.5) * rand_scale_x_obj_layer1 * scale_x_obj_layer1)
				scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer1]])
			else:
				scale_x_obj = scale_x_obj_layer2 + ((random.random() - 0.5) * rand_scale_x_obj_layer2 * scale_x_obj_layer2)
				scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer2]])
			growth_object_path_line_data = list(map(lambda x: x if type(x) == str else np.add(end_of_added_object_point_coords, np.dot(rotation_matrix, np.dot(scaling_matrix, x))), object_path_line_data))

			# builds the additional path of this object according to its transformed coordinates
			growth_object_string = "M {0:.5f} {1:.5f} ".format(growth_object_path_move_data[0],growth_object_path_move_data[1]) 
			growth_object_string += separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), growth_object_path_line_data)))
			growth_object_string += " "
			returned_path_string += growth_object_string

			# if first_growth_added_objects :
			# 	print("Added object:", added_object_string)

		# if ind_added_objet == 0 :
		# 	print("*********************")
		# 	print("translated_object_path_data", added_object_path_line_data[0:100])
		# 	print("returned_path_string", returned_path_string[0:100])
		# 	print("*********************")

	# separator = ' '
	# return separator.join(map(str, local_object_path_data))
	returned_path_string += '" />'
	return returned_path_string

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	COPY = 1
	OBJECT_ALONG_PATH = 2
	SUNRISE = 3
	SPIKES = 4
	SECOND_ORDER_SPIKES = 5
	CRACKS = 6
	CRACKS_AND_NOISE = 7
	OBJECTS_NOISE_AND_SHRINK = 8

	single_transf_hash = {\
		"COPY" : COPY,\
		"OBJECT_ALONG_PATH" : OBJECT_ALONG_PATH,\
		"SUNRISE" : SUNRISE,\
		"SPIKES" : SPIKES,\
		"SECOND_ORDER_SPIKES" : SECOND_ORDER_SPIKES,\
		"CRACKS" : CRACKS,\
		"CRACKS_AND_NOISE" : CRACKS_AND_NOISE,\
		"OBJECTS_NOISE_AND_SHRINK" : OBJECTS_NOISE_AND_SHRINK,\
	 }

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################
	global chapter_no
	global percent_transf
	global layer_no
	global nb_layers

	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2
	global interval_obj_layer1
	global interval_obj_layer2
	global growth_obj_layer1
	global growth_obj_layer2

	global rand_scale_x_obj_layer1
	global rand_scale_x_obj_layer2
	global rand_rotation_obj_layer1
	global rand_rotation_obj_layer2

	global sun_x
	global sun_y

	global sound_volume

	scenario_file_name = ''
	single_transfs = []
	drawing_path_IDs = []
	first_frame_input = False
	single_objects_paths = []

	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb_layers=","chapter_no=",\
			"transformation=","percent_transf=","percent_transf1=","drawing_path_IDs=","csv_obj_data_storage=","first_frame_input=",\
			"objects_directory=","objects_file=","objects_IDs=", \
			"scale_x_obj_layer1=","scale_y_obj_layer1=","scale_x_obj_layer2=","scale_y_obj_layer2=",\
			"interval_obj_layer1=","interval_obj_layer2=", "growth_obj_layer1=","growth_obj_layer2=",\
			"rand_scale_x_obj_layer1=","rand_scale_x_obj_layer2=","rand_rotation_obj_layer1=","rand_rotation_obj_layer2=","sun_x=","sun_y=","sound_volume="])
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
		elif opt == "--drawing_path_IDs":
			drawing_path_IDs = arg
		elif opt == "--csv_obj_data_storage":
			csv_obj_data_storage = arg
		elif opt == "--first_frame_input":
			first_frame_input = arg
		elif opt == "--objects_directory":
			objects_directory = arg
		elif opt == "--objects_file":
			objects_file = arg
		elif opt == "--objects_IDs":
			objects_IDs = arg
			objects_IDs_strings = re.split(r'/', objects_IDs.rstrip())
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
		elif opt == "--scale_x_obj_layer1":
			scale_x_obj_layer1 = force_num(arg)
		elif opt == "--scale_y_obj_layer1":
			scale_y_obj_layer1 = force_num(arg)
		elif opt == "--scale_x_obj_layer2":
			scale_x_obj_layer2 = force_num(arg)
		elif opt == "--scale_y_obj_layer2":
			scale_y_obj_layer2 = force_num(arg)
		elif opt == "--interval_obj_layer1":
			interval_obj_layer1 = force_num(arg)
		elif opt == "--interval_obj_layer2":
			interval_obj_layer2 = force_num(arg)
		elif opt == "--growth_obj_layer1":
			growth_obj_layer1 = force_num(arg)
		elif opt == "--growth_obj_layer2":
			growth_obj_layer2 = force_num(arg)
		elif opt == "--rand_scale_x_obj_layer1":
			rand_scale_x_obj_layer1 = force_num(arg)
		elif opt == "--rand_scale_x_obj_layer2":
			rand_scale_x_obj_layer2 = force_num(arg)
		elif opt == "--rand_rotation_obj_layer1":
			rand_rotation_obj_layer1 = force_num(arg)
		elif opt == "--rand_rotation_obj_layer2":
			rand_rotation_obj_layer2 = force_num(arg)
		elif opt == "--sun_x":
			sun_x = force_num(arg)
		elif opt == "--sun_y":
			sun_y = force_num(arg)
		elif opt == "--sound_volume":
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

	##################################################################
	# STORES THE LINES IN AN ARRAY
	##################################################################
	# print("input_file_name", input_file_name)
	contents = FILEin.read()
	SVG_file_lines = contents.splitlines()

	##################################################################
	# CSV FILES WITH OBJECT POSITIONS ALONG THE CURVES
	##################################################################
	if OBJECT_ALONG_PATH in single_transfs :
		##################################################################
		# GENERATES THE CSV FILES WITH OBJECT POSITIONS ALONG THE CURVES
		if first_frame_input :
			store_initial_object_data(csv_obj_data_storage, drawing_path_IDs, [interval_obj_layer1, interval_obj_layer2], [scale_x_obj_layer1, scale_x_obj_layer2], [rand_scale_x_obj_layer1, rand_scale_x_obj_layer2], [rand_rotation_obj_layer1, rand_rotation_obj_layer2], SVG_file_lines)
		else :
			update_growth_object_data(csv_obj_data_storage, drawing_path_IDs, [growth_obj_layer1, growth_obj_layer2], [scale_x_obj_layer1, scale_x_obj_layer2], [rand_scale_x_obj_layer1, rand_scale_x_obj_layer2], [rand_rotation_obj_layer1, rand_rotation_obj_layer2], SVG_file_lines)

		##################################################################
		# OPENS THE CSV FILE WITH OBJECT DATA STORAGE
		# node positions and normals
		try:
			FILEObjectData_in = open(csv_obj_data_storage, "rt")
		except IOError:
			print("File not opened :", csv_obj_data_storage, " or path is incorrect")
		  
		readObjectDataCSV = csv.reader(FILEObjectData_in, delimiter=',')
		# header
		line = next(readObjectDataCSV)

	if OBJECT_ALONG_PATH in single_transfs or SPIKES in single_transfs or SECOND_ORDER_SPIKES in single_transfs or CRACKS in single_transfs or CRACKS_AND_NOISE in single_transfs or OBJECTS_NOISE_AND_SHRINK in single_transfs :
		##################################################################
		# OPENS THE SVG FILE WITH SINGLE OBJECTS PATHS
		# node positions and normals
		try:
			FILEObjectPath_in = open(os.path.join(objects_directory, objects_file), "rt")
		except IOError:
			print("File not opened :", os.path.join(objects_directory, objects_file), " or path is incorrect")
		contents = FILEObjectPath_in.read()
		SVG_objects_file_lines = contents.splitlines()

		for object_line_string in SVG_objects_file_lines :
			if( re.search(r'<path', object_line_string) != None):
				# only keeps the ID and the path data
				object_result_path = re.search(r'\sd="([^\"]*)\"', object_line_string)
				if not object_result_path:
				    print('Expected path not found')
				object_result_ID = re.search(r'\sid="([^\"]*)\"', object_line_string)
				if not object_result_ID:
				    print('Expected ID not found')
				if object_result_ID and object_result_path :
					# print("IDs", result_ID.group(1), objects_IDs_strings)
					if object_result_ID.group(1) in objects_IDs_strings :
						single_objects_paths.append(object_result_path.group(1))
		# print("Object paths:", single_objects_paths)
		FILEObjectPath_in.close()

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	# additional_SVG_file_line_out = []
	transformation_no = 1
	transformed_SVG_file_line_out = []
	added_SVG_file_line_out = []
	for single_transf in single_transfs :
		##################################################################
		# appplies the transformation on each path
		##################################################################
		for line_string in SVG_file_lines :

			##########################################################################
			# PATH LINE RODUCED
			##########################################################################
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
					if result_ID.group(1) in drawing_path_IDs :
						# counts the path lines
						layer_no += 1

						path_data_string = result_path.group(1)
						style_data_string = result_style.group(1)
						path_ID_data_string = result_ID.group(1)

						##################################################################
						# BUILDS A SINGLE PATH FROM ALL THE OBJECTS ALONG THE PATH
						##################################################################
						if(single_transf == OBJECT_ALONG_PATH) :
							# prints the path line
							transformed_SVG_file_line_out.append(line_string)

							# parameters
							(previous_path_ID, path_style, number_of_located_objects, interval_between_objects) = next(readObjectDataCSV)
							number_of_located_objects = int(force_num(number_of_located_objects))
							interval_between_objects = force_num(interval_between_objects)
							# points
							point_coords_list = next(readObjectDataCSV)
							# tangents
							tangent_coords_list = next(readObjectDataCSV)
							# x scale
							scale_x_list = next(readObjectDataCSV)
							# objects added through growth
							object_added_through_growth = next(readObjectDataCSV)
							new_path_data_string = OBJECT_ALONG_PATH_transformation(single_objects_paths[0], previous_path_ID, path_style, layer_no, percent_transf[transformation_no], number_of_located_objects, point_coords_list, tangent_coords_list, scale_x_list, object_added_through_growth)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)

						##################################################################
						# STRETCHES THE END OF OBJECTS CHAINS ALONG THE PATH TO EVOKE SUNRISE
						##################################################################
						elif(single_transf == SUNRISE) :
							# print("Sunrise object path percent", "%0.3f" % percent_transf[transformation_no])
							new_path_data_string = SUNRISE_transformation(path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no])
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
						
						##################################################################
						# TRANSFORMS THE ICE (OBJECT CHAINS) BY ADDING CRACK DISTORSION ON THE ORIENTATION 
						##################################################################
						elif(single_transf == CRACKS) :
							new_path_data_string = CRACKS_transformation(path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no], False, layer_no)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
						
						##################################################################
						# TRANSFORMS THE ICE (OBJECT CHAINS) BY ADDING CRACK DISTORSION ON THE ORIENTATION + NOISE ON THE WIDTH
						##################################################################
						elif(single_transf == CRACKS_AND_NOISE) :
							new_path_data_string = CRACKS_transformation(path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no], True, layer_no)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
						
						##################################################################
						# TRANSFORMS THE ICE (OBJECT CHAINS) BY ADDING CRACK DISTORSION ON THE ORIENTATION + NOISE ON THE WIDTH + MOTION
						##################################################################
						elif(single_transf == OBJECTS_NOISE_AND_SHRINK) :
							new_path_data_string = OBJECTS_NOISE_AND_SHRINK_transformation(path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no], layer_no)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
						
						##################################################################
						# TRANSFORMS THE CURVES INTO LINES AND ADD TRIANGULAR SPIKES AT THE JUNCTION BETWEEN CURVES
						##################################################################
						elif(single_transf == SPIKES) :
							(new_path_data_string, added_spikes_data_string) = SPIKES_transformation(single_objects_paths[0], path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no], layer_no, 0)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
							added_SVG_file_line_out.append(added_spikes_data_string)
						
						##################################################################
						# TRANSFORMS THE CURVES INTO LINES AND ADD TRIANGULAR SECOND_ORDER_SPIKES AT THE JUNCTION BETWEEN CURVES
						##################################################################
						elif(single_transf == SECOND_ORDER_SPIKES) :
							(new_path_data_string, added_spikes_data_string) = SECOND_ORDER_SPIKES_transformation(single_objects_paths[0], path_data_string, path_ID_data_string, style_data_string, percent_transf[transformation_no], layer_no)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
							added_SVG_file_line_out.append(added_spikes_data_string)
						
						##################################################################
						# UNKNOWN TRANSFORMATION
						##################################################################
						else :
							print( "Unknown transformation #", single_transf)
							return 0
					# non drawing paths are ignored
					else :
						if single_transf == OBJECT_ALONG_PATH :
							# ignore path, do nothing
							ignore_path = True
						elif single_transf == SUNRISE or single_transf == SPIKES or single_transf == SECOND_ORDER_SPIKES or single_transf == CRACKS or single_transf == CRACKS_AND_NOISE  or single_transf == OBJECTS_NOISE_AND_SHRINK :
							# copy path
							transformed_SVG_file_line_out.append(line_string)
						# does not count the drawing path lines
						layer_no += 0
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
	if OBJECT_ALONG_PATH in single_transfs :
		FILEObjectData_in.close()
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
