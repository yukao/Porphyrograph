#!/usr/bin/python3 

# vv_obj_along_path.py
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

import vv_movie_generation_from_path

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_obj_along_path.py --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
vv_obj_along_path.py --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation
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
		FILEObjectData_out = open(local_csv_storage_file_name, "wt")
	except IOError:
		print("File not opened :", local_csv_storage_file_name, " or path is incorrect")
	  
	writeObjectDataCSV = csv.writer(FILEObjectData_out, delimiter=',')
	# header
	writeObjectDataCSV.writerow(["ID", "style", "number_of_located_objects", "interval_between_objects"])

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
					(total_length, curve_lengths, curve_nodes) = vv_movie_generation_from_path.path_curves_and_lengths(line_string)
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
					writeObjectDataCSV.writerow([result_ID.group(1), result_style.group(1), "{0:d}".format(nb_tot_obj_along_curve), "{0:f}".format(interval_between_objects)])
					# print("object_points:", object_points[0:10], "...")
					writeObjectDataCSV.writerow(object_points)
					# print("object_normals:", object_normals[0:10], "...")
					writeObjectDataCSV.writerow(object_normals)
					# print("object_scale_x:", object_scale_x[0:10], "...")
					writeObjectDataCSV.writerow(object_scale_x)
					# print("object_added_through_growth:", object_added_through_growth[0:10], "...")
					writeObjectDataCSV.writerow(object_added_through_growth)

	FILEObjectData_out.close()

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
		FILEObjectData_out = open(local_csv_storage_file_name, "wt")
	except IOError:
		print("File not opened :", local_csv_storage_file_name, " or path is incorrect")

	# node positions and normals
	try:
		FILEObjectData_in = open(aux_csv_storage_file_name, "rt")
	except IOError:
		print("File not opened :", aux_csv_storage_file_name, " or path is incorrect")


	writeObjectDataCSV = csv.writer(FILEObjectData_out, delimiter=',')
	readObjectDataCSV = csv.reader(FILEObjectData_in, delimiter=',')

	# header
	line = next(readObjectDataCSV)
	writeObjectDataCSV.writerow(line)

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
					line = next(readObjectDataCSV)
					writeObjectDataCSV.writerow(line)
					# object_points
					line = next(readObjectDataCSV)
					writeObjectDataCSV.writerow(line)
					# object_normals
					line = next(readObjectDataCSV)
					writeObjectDataCSV.writerow(line)
					# object_scale_x
					line = next(readObjectDataCSV)
					writeObjectDataCSV.writerow(line)
					# updates object_added_through_growth according to growth
					object_added_through_growth = next(readObjectDataCSV)
					nb_objets = int(len(object_added_through_growth))
					nb_growing_objects = int(growth_obj_layer * nb_objets)
					for ind_growing_object in range(nb_growing_objects) :
						# selection of a random index among all the objects
						selected_index = min(int(random.random() * nb_objets), nb_objets - 1)
						# addition of an object
						object_added_through_growth[selected_index] = str(int(force_num(object_added_through_growth[selected_index] )) + 1)
					writeObjectDataCSV.writerow(object_added_through_growth)

	FILEObjectData_out.close()
	FILEObjectData_in.close()

#################################################################
# TRANSFORMATION FUNCTIONS
##################################################################

##################################################################
# ADDING OBJECTS ALONG A PATH
def OBJECT_ALONG_PATH_transformation(local_object_data_string, local_ID_data_string, local_style_data_string, local_layer_no, local_percent_transf, local_number_of_located_objects, local_point_coords_list, local_tangent_coords_list, local_scale_x_list, local_object_added_through_growth):
	global scale_x_obj_layer1
	global scale_y_obj_layer1
	global scale_x_obj_layer2
	global scale_y_obj_layer2
	global interval_obj_layer1
	global interval_obj_layer2

	local_object_path_data = re.split(r' +' , local_object_data_string.rstrip())
	local_object_path_data_size = len(local_object_path_data)
	index = 0
	if(local_object_path_data[index] == "M") :
		index += 1
		object_path_move_data = [force_num(local_object_path_data[index]), force_num(local_object_path_data[index + 1])]
		index += 2
	else :
		print("move expected as first command for object")
		sys.exit(0)

	object_path_line_data = []
	while( index < local_object_path_data_size) :
		if local_object_path_data[index] == "L" :
			object_path_line_data.append(local_object_path_data[index])
			index += 1
			# processes the next lineto
			while(True) :
				object_path_line_data.append([force_num(local_object_path_data[index]), force_num(local_object_path_data[index + 1])])
				index += 2

				if(index >= local_object_path_data_size) or (re.search(r'^[MCZL]',local_object_path_data[index]) != None):
					break
		elif(local_object_path_data[index] == "C") :
			object_path_line_data.append(local_object_path_data[index])
			index += 1
			# processes the next curve segments
			while(True) :
				object_path_line_data.append([force_num(local_object_path_data[index]), force_num(local_object_path_data[index + 1])])
				object_path_line_data.append([force_num(local_object_path_data[index + 2]), force_num(local_object_path_data[index + 3])])
				object_path_line_data.append([force_num(local_object_path_data[index + 4]), force_num(local_object_path_data[index + 5])])
				index += 6
			
				if(index >= local_object_path_data_size) or (re.search(r'^[MCZL]',local_object_path_data[index]) != None):
					break
		
		elif(local_object_path_data[index] == "Z") :
			object_path_line_data.append(local_object_path_data[index])
			index += 1
		
		else :
			print( "Line: ", local_object_path_data_string[0:500])
			print( "OBJECT_ALONG_PATH_transformation Unexpected sign at index "+str(index)+": ("+str(local_object_path_data[index])+")")
			sys.exit(2)

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
			rotation_matrix = np.array([[-curve_tangent_coords_x, curve_tangent_coords_y], [-curve_tangent_coords_y, -curve_tangent_coords_x]])
			scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer1]])
		else:
			rotation_matrix = np.array([[curve_tangent_coords_x, -curve_tangent_coords_y], [curve_tangent_coords_y, curve_tangent_coords_x]])
			scaling_matrix = np.array([[scale_x_obj, 0], [0, scale_y_obj_layer2]])
		added_object_path_line_data = list(map(lambda x: x if type(x) == str else np.add(curve_point_coords, np.dot(rotation_matrix, np.dot(scaling_matrix, x))), object_path_line_data))
		separator = ' '
		added_object_path_move_data = np.add(curve_point_coords, object_path_move_data)

		# builds the additional path of this object according to its transformed coordinates
		added_object_string = "M {0:.5f} {1:.5f} ".format(added_object_path_move_data[0],added_object_path_move_data[1]) 
		added_object_string += separator.join(list(map(lambda x: x if type(x) == str else "{0:.5f} {1:.5f}".format(x[0],x[1]), added_object_path_line_data)))
		added_object_string += " "
		returned_path_string += added_object_string

		first_growth_added_objects = False
		if nb_of_growth_objets == 0 and object_added_through_growth > 0 :
			print("Base object:", added_object_string)
			first_growth_added_objects = True

		# adds possible additional objects and defines them randomly at each frame, contrary to the root object, they will not be stable
		for ind_growth_objet in range(object_added_through_growth) :
			# print("growth_object_number", ind_growth_objet)
			nb_of_growth_objets += 1

			# places the object at the end of the preceding one
			if ind_growth_objet == 0 :
				end_of_added_object_point_coords = added_object_path_line_data[1]
			else :
				end_of_added_object_point_coords = growth_object_path_line_data[1]
			growth_object_path_move_data = np.add(end_of_added_object_point_coords, object_path_move_data)

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

	single_transf_hash = {\
		"COPY" : COPY,\
		"OBJECT_ALONG_PATH" : OBJECT_ALONG_PATH,\
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
			"rand_scale_x_obj_layer1=","rand_scale_x_obj_layer2=","rand_rotation_obj_layer1=","rand_rotation_obj_layer2="])
	except getopt.GetoptError:
		print("incorrect option in command ", main_args)
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb_layers"):
			nb_layers = force_num(arg)
		elif opt in ("--chapter_no"):
			chapter_no = force_num(arg)
		elif opt in ("--drawing_path_IDs"):
			drawing_path_IDs = arg
		elif opt in ("--csv_obj_data_storage"):
			csv_obj_data_storage = arg
		elif opt in ("--first_frame_input"):
			first_frame_input = arg
		elif opt in ("--objects_directory"):
			objects_directory = arg
		elif opt in ("--objects_file"):
			objects_file = arg
		elif opt in ("--objects_IDs"):
			objects_IDs = arg
			objects_IDs_strings = re.split(r'/', objects_IDs.rstrip())
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
		elif opt in ("--scale_x_obj_layer1"):
			scale_x_obj_layer1 = force_num(arg)
		elif opt in ("--scale_y_obj_layer1"):
			scale_y_obj_layer1 = force_num(arg)
		elif opt in ("--scale_x_obj_layer2"):
			scale_x_obj_layer2 = force_num(arg)
		elif opt in ("--scale_y_obj_layer2"):
			scale_y_obj_layer2 = force_num(arg)
		elif opt in ("--interval_obj_layer1"):
			interval_obj_layer1 = force_num(arg)
		elif opt in ("--interval_obj_layer2"):
			interval_obj_layer2 = force_num(arg)
		elif opt in ("--growth_obj_layer1"):
			growth_obj_layer1 = force_num(arg)
		elif opt in ("--growth_obj_layer2"):
			growth_obj_layer2 = force_num(arg)
		elif opt in ("--rand_scale_x_obj_layer1"):
			rand_scale_x_obj_layer1 = force_num(arg)
		elif opt in ("--rand_scale_x_obj_layer2"):
			rand_scale_x_obj_layer2 = force_num(arg)
		elif opt in ("--rand_rotation_obj_layer1"):
			rand_rotation_obj_layer1 = force_num(arg)
		elif opt in ("--rand_rotation_obj_layer2"):
			rand_rotation_obj_layer2 = force_num(arg)
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
	contents = FILEin.read()
	SVG_file_lines = contents.splitlines()

	##################################################################
	# GENERATES THE CSV FILES WITH OBJECT POSITIONS ALONG THE CURVES
	##################################################################
	if first_frame_input :
		store_initial_object_data(csv_obj_data_storage, drawing_path_IDs, [interval_obj_layer1, interval_obj_layer2], [scale_x_obj_layer1, scale_x_obj_layer2], [rand_scale_x_obj_layer1, rand_scale_x_obj_layer2], [rand_rotation_obj_layer1, rand_rotation_obj_layer2], SVG_file_lines)
	else :
		update_growth_object_data(csv_obj_data_storage, drawing_path_IDs, [growth_obj_layer1, growth_obj_layer2], [scale_x_obj_layer1, scale_x_obj_layer2], [rand_scale_x_obj_layer1, rand_scale_x_obj_layer2], [rand_rotation_obj_layer1, rand_rotation_obj_layer2], SVG_file_lines)

	##################################################################
	# OPENS THE CSV FILE WITH OBJECT DATA STORAGE
	##################################################################
	# node positions and normals
	try:
		FILEObjectData_in = open(csv_obj_data_storage, "rt")
	except IOError:
		print("File not opened :", csv_obj_data_storage, " or path is incorrect")
	  
	readObjectDataCSV = csv.reader(FILEObjectData_in, delimiter=',')
	# header
	line = next(readObjectDataCSV)

	##################################################################
	# OPENS THE SVG FILE WITH SINGLE OBJECTS PATHS
	##################################################################
	# node positions and normals
	try:
		FILEObjectPath_in = open(os.path.join(objects_directory, objects_file), "rt")
	except IOError:
		print("File not opened :", os.path.join(objects_directory, objects_file), " or path is incorrect")
	contents = FILEObjectPath_in.read()
	SVG_objects_file_lines = contents.splitlines()

	for line_string in SVG_objects_file_lines :
		if( re.search(r'<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			if result_ID and result_path :
				# print("IDs", result_ID.group(1), objects_IDs_strings)
				if result_ID.group(1) in objects_IDs_strings :
					single_objects_paths.append(result_path.group(1))
	# print("Object paths:", single_objects_paths)
	FILEObjectPath_in.close()

	##################################################################
	# ITERATES OVER ALL THE TRANSFORMATIONS FOR EACH PATH
	##################################################################
	# additional_SVG_file_line_out = []
	transformation_no = 1
	transformed_SVG_file_line_out = []
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

						##################################################################
						# COPY TRANSFORMATION
						##################################################################
						if(single_transf == COPY) :
							# nothing to be done
							additional_SVG_file_line_out = additional_SVG_file_line_out
						
						##################################################################
						# BUILDS A SINGLE PATH FROM ALL THE OBJECTS ALONG THE PATH
						##################################################################
						elif(single_transf == OBJECT_ALONG_PATH) :
							# prints the path line
							transformed_SVG_file_line_out.append(line_string)

							# parameters
							(path_ID, path_style, number_of_located_objects, interval_between_objects) = next(readObjectDataCSV)
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
							new_path_data_string = OBJECT_ALONG_PATH_transformation(single_objects_paths[0], path_ID, path_style, layer_no, percent_transf[transformation_no], number_of_located_objects, point_coords_list, tangent_coords_list, scale_x_list, object_added_through_growth)
							# adds it to the list of new paths to be added at the end of the file so that they are on top
							transformed_SVG_file_line_out.append(new_path_data_string)
						
						##################################################################
						# UNKNOWN TRANSFORMATION
						##################################################################
						else :
							print( "Unknown transformation #", single_transf)
							return 0
					# non drawing paths are ignored
					else :
						# ignore path
						# does not count the path lines
						layer_no += 0
			elif( re.search(r'^</g>', line_string) != None):
				# for line in additional_SVG_file_line_out :
				# 	transformed_SVG_file_line_out.append(line)
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
