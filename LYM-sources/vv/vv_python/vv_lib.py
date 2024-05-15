#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math
import os
import csv

import colorsys

from math import log
from math import pow
from math import floor

from collections import OrderedDict
from operator import itemgetter

from signal import signal, SIGINT
from statistics import mean
from scipy.signal import hilbert, chirp, find_peaks

import wave
import wavfile
import struct
from pydub import AudioSegment

import random

import numpy as np
import matplotlib.pyplot as plt

##################################################################
# GLOBAL VARIABLE
##################################################################
stored_soundtrack_directory = ''
stored_soundtrack_envelope_file = ''
stored_soundtrack_peaked_envelope_file = ''
stored_soundtrack_smoothed_envelope_file = ''
stored_onsets_data_file = ''

Signal0 = []
Envelope_Signal0 = []
Peaked_Envelope_Signal0 = []
Smoothed_Envelope_Signal0 = []
Signal1 = []
Envelope_Signal1 = []
Peaked_Envelope_Signal1 = []
Smoothed_Envelope_Signal1 = []

Timestamp_current_index = 0
Envelope_Timestamp_current_index = 0
Peaked_Envelope_Timestamp_current_index = 0
Smoothed_Envelope_Timestamp_current_index = 0

Timestamp = []
Envelope_timestamp = []
Peaked_Envelope_timestamp = []
Smoothed_Envelope_timestamp = []

current_Timestamp = 0
Envelope_current_Timestamp = 0
Peaked_Envelope_current_Timestamp = 0
Smoothed_Envelope_current_Timestamp = 0

Onset0_timestamp = []
Onset1_timestamp = []
Onset0_current_Timestamp = -10000
Onset0_previous_Timestamp = -10000
Onset0_Timestamp_current_index = 0
Onset1_current_Timestamp = -10000
Onset1_previous_Timestamp = -10000
Onset1_Timestamp_current_index = 0

soundtrack_duration = 0
looped_soundtrack = False

min_abs_X = 1000000
min_abs_Y = 1000000
max_abs_X = -1000000
max_abs_Y = -1000000
absolute_current_point_X = 0
absolute_current_point_Y = 0
subpath_initial_point_X = 0
subpath_initial_point_Y = 0
new_path = 1

##################################################################
# DISTANCE BETWEEN TWO POINTS
##################################################################
def length(x,y,X,Y) :
	return math.sqrt((X-x)*(X-x) + (Y-y)*(Y-y))
def length2(u,v) :
	return math.sqrt((v[0]-u[0])*(v[0]-u[0]) + (v[1]-u[1])*(v[1]-u[1]))
def norm(u) :
	return math.sqrt(u[0]*u[0] + u[1]*u[1])

##################################################################
# CHECKS WHETHER A STRING IS A FLOAT
##################################################################
def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

##################################################################
# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
##################################################################
def normalize_file_paths(input_lines) :
	output_lines = []
	# print("Initial file path: ", input_file_name)
	# print("Initial file [", input_lines, "]")
	indLine = 0
	while( indLine < len(input_lines)) :
		line_string = input_lines[indLine]
		indLine += 1	
		if(re.search(r'<path', line_string) != None) :
			line_string = re.sub(r'^[\s]+<path', r'<path', line_string)
			while(re.search(r'/>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1	
			# transforms , into space
			line_string = re.sub(r'\,', r' ', line_string)

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
				line_string = '<path id="{0!s}" style="{1!s}" d="{2!s}"/>'.format(result_ID.group(1), result_style.group(1), result_path.group(1))
		elif(re.search(r'<g', line_string) != None) :
			line_string = re.sub(r'^[\s]+<g', r'<g', line_string)
			while(re.search(r'>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1
		elif(re.search(r'<text', line_string) != None) :
			line_string = re.sub(r'^[\s]+<text', r'<text', line_string)
			while(re.search(r'</text>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1	
		output_lines.append( line_string )
	return output_lines

##################################################################
# MAKES ABSOLUTE COORDINATES USED BY EXTERNAL PROGRAMS
# NOT USED INSIDE THIS PROGRAM BECAUSE OTHER DATA ARE STORED WITH THE LINE
##################################################################
def absolute_coordinates_file_paths(input_lines) :
	output_lines = []
	for line_string in input_lines :
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		# print("Initial line_string [", line_string, "]")
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
				##################################################################
				# transforms relative coordinates of base video into absolute ones
				##################################################################
				path_data_string = result_path.group(1)
				# print("Path line_string [", path_data_string, "]")
				# adds space after move or curve and before z
				path_data_string = re.sub(r'([MmCcLlHhVv])', r'\1 ', path_data_string)
				path_data_string = re.sub(r'([Zz])', r' \1 ', path_data_string)
				# print("Relative coordinates [", path_data_string, "]")

				# transformation into absolute coordinates
				# print(path_data_string[0:100])
				path_data_string = relative_to_absolute_coordinates(path_data_string, False)

				#storage
				line_string = '<path id="{0!s}" style="{1!s}" d="{2!s}"/>'.format(result_ID.group(1), result_style.group(1), path_data_string)
		output_lines.append( line_string )
		# print("Absolute coordinates [", line_string, "]")
	return output_lines

#################################################################
# NORMALIZATION FUNCTIONS
##################################################################
##################################################################
# TRANSFORMS RELATIVE COORDINATES OF A SVG PATH INTO ABSOLUTE ONES
##################################################################
# stores the rectangular bounding box of the drawing
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

# stores the first coordinates of a path, so that the
# pat can be closed for the Z/z command
def subpath_initial_coordinates() :
	global subpath_initial_point_X
	global subpath_initial_point_Y
	global new_path
	if new_path == 1 :
		subpath_initial_point_X = absolute_current_point_X
		subpath_initial_point_Y = absolute_current_point_Y
		new_path = 0

# transforms all the commands into absolute coordinates
def relative_to_absolute_coordinates(local_path_data_string, keep_lines):
	global min_abs_X
	global min_abs_Y
	global max_abs_X
	global max_abs_Y
	global absolute_current_point_X
	global absolute_current_point_Y
	global subpath_initial_point_X
	global subpath_initial_point_Y
	global new_path

	# print("relative_to_absolute_coordinates [", local_path_data_string, "]")
	if local_path_data_string == '' :
		return ''

	local_path_data = re.split(r' +' , local_path_data_string.rstrip())
	local_path_data_size = len(local_path_data)
	first_move = 1
	new_path = 1

	# print("local_path_data_string (", local_path_data_string, ")")
	# print("local_path_data", local_path_data)
	# print("local_path_data_size", local_path_data_size)
	index = 0
	while( index < local_path_data_size) :
		if(local_path_data[index] == "M") :
			local_path_data[index] = "M"
			index += 1
			# processes the next moveto and possible following lineto
			while(True) :
				absolute_current_point_X = force_num(local_path_data[index])
				absolute_current_point_Y = force_num(local_path_data[index + 1])
				minMaxUpdate()
				subpath_initial_coordinates()
				index += 2

				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break

			first_move = 0
		
		elif(local_path_data[index] == "m") :
			local_path_data[index] = "M"
			index += 1
			# processes the next moveto and possible following lineto
			while(True) :
				# the first move even if relative (m), has absolute coordinates as first coordinates
				# possibly following moves have relative coordinates
				if first_move :
					absolute_current_point_X = force_num(local_path_data[index])
					absolute_current_point_Y = force_num(local_path_data[index + 1])
				else :
					local_path_data[index] = absolute_current_point_X + force_num(local_path_data[index])
					local_path_data[index + 1] = absolute_current_point_Y + force_num(local_path_data[index + 1])
					absolute_current_point_X = local_path_data[index]
					absolute_current_point_Y = local_path_data[index + 1]

				minMaxUpdate()
				subpath_initial_coordinates()
				index += 2

				first_move = 0

				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
					
		elif local_path_data[index] == "L" or local_path_data[index] == "l" :
			SVG_path_tag = local_path_data[index]
			if keep_lines :
				local_path_data[index] = "L"
				index += 1
				# processes the next lineto
				while(True) :
					if(SVG_path_tag == "L") :
						absolute_current_point_X = force_num(local_path_data[index])
						absolute_current_point_Y = force_num(local_path_data[index + 1])
					
					else :  # if(SVG_path_tag == "H")
						local_path_data[index] = absolute_current_point_X + force_num(local_path_data[index])
						local_path_data[index + 1] = absolute_current_point_Y + force_num(local_path_data[index + 1])
						absolute_current_point_X = local_path_data[index]
						absolute_current_point_Y = local_path_data[index + 1]
					minMaxUpdate()
					subpath_initial_coordinates()
					index += 2

					if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
						break
			else : # transform line into curve
				# definition of the replacing line segment
				local_path_data[index] = "C"
				index += 1
				# processes the next line segments
				while(True) :
					# L requires 6 values instead of 2 -> insertion of 4 values inside the array of the path
					# and shift of the path of 4 value towards the end
					# adds 4 zeros in position local_path_data_size to add 4 elements to the list
					local_path_data.extend([0, 0, 0, 0])
					for i in range(local_path_data_size - 1 + 4, index + 4 + 1, -1) :
						local_path_data[i] = local_path_data[i - 4]
						# size - 1 -> size + 3
						# size - 2 -> size + 2
						# ...
						# index + 3 -> index + 7
						# index + 2 -> index + 6
						# (index, index + 1): former line end
						# (index, index + 1): control point # 1
						# (index, index + 1): line end
						# (index, index + 1): line end
					local_path_data_size += 4

					initial_absolute_current_point_X = absolute_current_point_X
					initial_absolute_current_point_Y = absolute_current_point_Y
					if(SVG_path_tag == "L") :
						absolute_current_point_X = force_num(local_path_data[index])
						absolute_current_point_Y = force_num(local_path_data[index + 1])
					
					else :  # if(SVG_path_tag == "H")
						absolute_current_point_X = absolute_current_point_X + force_num(local_path_data[index])
						absolute_current_point_Y = absolute_current_point_Y + force_num(local_path_data[index + 1])

					local_path_data[index] = 0.67 * initial_absolute_current_point_X + 0.33 * absolute_current_point_X
					local_path_data[index + 1] = 0.67 * initial_absolute_current_point_Y + 0.33 * absolute_current_point_Y
					local_path_data[index + 2] = 0.33 * initial_absolute_current_point_X + 0.67 * absolute_current_point_X
					local_path_data[index + 3] = 0.33 * initial_absolute_current_point_Y + 0.67 * absolute_current_point_Y
					local_path_data[index + 4] = absolute_current_point_X
					local_path_data[index + 5] = absolute_current_point_Y
					minMaxUpdate()
					subpath_initial_coordinates()
					index += 6
				
					if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
						break

		elif(local_path_data[index] == "C") :
			index += 1
			# processes the next curve segments
			while(True) :
				absolute_current_point_X = force_num(local_path_data[index + 4])
				absolute_current_point_Y = force_num(local_path_data[index + 5])
				minMaxUpdate()
				subpath_initial_coordinates()
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		elif(local_path_data[index] == "c") :
			local_path_data[index] = "C"
			index += 1
			# processes the next curve segments
			while(True) :
				for i in range(index, index + 6, 2) :
					local_path_data[i] = absolute_current_point_X + force_num(local_path_data[i])
					local_path_data[i + 1] = absolute_current_point_Y + force_num(local_path_data[i + 1])
				absolute_current_point_X = local_path_data[index + 4]
				absolute_current_point_Y = local_path_data[index + 5]
				minMaxUpdate()
				subpath_initial_coordinates()
				index += 6
			
				if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
					break
		
		# horizontal line h/H x and vertical line v/V y, are transformed into a curve 
		# prec_point middle middle next_point
		elif(local_path_data[index] == "H" or local_path_data[index] == "h"
			or local_path_data[index] == "V" or local_path_data[index] == "v") :
			SVG_path_tag = local_path_data[index]
			if keep_lines :
				# definition of the replacing line segment
				local_path_data[index] = "L"
				index += 1
				while(True) :
					# coordinates of new point (absolute_current_point_Y unchanged)
					if(SVG_path_tag == "H" or SVG_path_tag == "h") :
						if(SVG_path_tag == "H") :
							absolute_current_point_X = force_num(local_path_data[index])						
						else :  # if(SVG_path_tag == "H")
							absolute_current_point_X = absolute_current_point_X + force_num(local_path_data[index])
					# coordinates of new point (absolute_current_point_X unchanged)
					else : # if(SVG_path_tag == "V" or SVG_path_tag == "v")
						if(SVG_path_tag == "V") :
							absolute_current_point_Y = force_num(local_path_data[index])				
						else :  # if(SVG_path_tag == "V")
							absolute_current_point_Y = absolute_current_point_Y + force_num(local_path_data[index])

					# L requires 2 values instead of 1 -> insertion of 1 value inside the array of the path
					# and shift of the path of 1 value towards the end
					# adds zero in position local_path_data_size to add 1 element to the list
					local_path_data.append(0)
					for i in range(local_path_data_size, index + 1, -1) :
						local_path_data[i] = local_path_data[i - 1]
					local_path_data_size += 1

					# coordinates of the line segment end point for the next 2 values
					local_path_data[index] = absolute_current_point_X
					local_path_data[index + 1] = absolute_current_point_Y
					minMaxUpdate()
					subpath_initial_coordinates()
					index += 2
			
					if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
						break
			else : # transform line into curve
				# definition of the replacing line segment
				local_path_data[index] = "C"
				index += 1
				# processes the next line segments
				while(True) :
					# L requires 6 values instead of 1 -> insertion of 5 values inside the array of the path
					# and shift of the path of 5 value towards the end
					# adds 5 zeros in position local_path_data_size to add 5 elements to the list
					local_path_data.extend([0, 0, 0, 0, 0])
					for i in range(local_path_data_size - 1 + 5, index + 5, -1) :
						local_path_data[i] = local_path_data[i - 5]
						# size - 1 -> size + 4
						# size - 2 -> size + 3
						# ...
						# index + 2 -> index + 7
						# index + 1 -> index + 6
						# (index, index + 1): control point # 1
						# (index + 2, index + 3): control point # 2
						# (index + 4, index + 5): line end
					local_path_data_size += 5

					initial_absolute_current_point_X = absolute_current_point_X
					initial_absolute_current_point_Y = absolute_current_point_Y
					# coordinates of new point (absolute_current_point_Y unchanged)
					if(SVG_path_tag == "H" or SVG_path_tag == "h") :
						if(SVG_path_tag == "H") :
							absolute_current_point_X = force_num(local_path_data[index])						
						else :  # if(SVG_path_tag == "H")
							absolute_current_point_X = absolute_current_point_X + force_num(local_path_data[index])
					# coordinates of new point (absolute_current_point_X unchanged)
					else : # if(SVG_path_tag == "V" or SVG_path_tag == "v")
						if(SVG_path_tag == "V") :
							absolute_current_point_Y = force_num(local_path_data[index])				
						else :  # if(SVG_path_tag == "V")
							absolute_current_point_Y = absolute_current_point_Y + force_num(local_path_data[index])

					local_path_data[index] = 0.67 * initial_absolute_current_point_X + 0.33 * absolute_current_point_X
					local_path_data[index + 1] = 0.67 * initial_absolute_current_point_Y + 0.33 * absolute_current_point_Y
					local_path_data[index + 2] = 0.33 * initial_absolute_current_point_X + 0.67 * absolute_current_point_X
					local_path_data[index + 3] = 0.33 * initial_absolute_current_point_Y + 0.67 * absolute_current_point_Y
					local_path_data[index + 4] = absolute_current_point_X
					local_path_data[index + 5] = absolute_current_point_Y
					minMaxUpdate()
					subpath_initial_coordinates()
					index += 6
				
					if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
						break

		elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
			absolute_current_point_X = subpath_initial_point_X
			absolute_current_point_Y = subpath_initial_point_Y
			new_path = 1
			index += 1
		
		else :
			print( "Line: ", local_path_data_string)
			print( "relative_to_absolute_coordinates Unexpected sign at index "+str(index)+": ("+str(local_path_data[index])+")")
			sys.exit(2)

	# printf("Image min (min_abs_X,min_abs_Y) max (max_abs_X,max_abs_Y)\n")
	# print("local_path_data :(",separator.join(map(str, local_path_data)),")")
	string_path_data = []
	for x in local_path_data :
		if type(x) == str:
			string_path_data.append(x)
		else:
			string_path_data.append("%.5f" % x)
	separator = ' '
	return separator.join(map(str, string_path_data))

##################################################################
# TRANSFORMS EACH PATH INTO A SET OF ELEMENTARY CURVES: 
# polylines and polycurves are split into elementary bezier curves
##################################################################
# transforms a list of elementary curves into filled paths by
# connecting curves with common end (1st cruve) and  begin (2nd curve) points
def filled_string_from_curves(local_elem_result_path, local_id, local_style, local_nb_elementary_curves) :
	# second: the individual paths connected everytime it is possible. In this case the filling is correct
	ind_elem = 0
	filled_elem_bezier_st = '<path id="{0!s}" style="{1!s}" vv_length="{2:d}" d="'.format(local_id, local_style, local_nb_elementary_curves)
	for elem_curve in local_elem_result_path :
		if ind_elem == 0 :
			filled_elem_bezier_st = filled_elem_bezier_st + "M {0:.5f} {1:.5f} C {2:.5f} {3:.5f} {4:.5f} {5:.5f} {6:.5f} {7:.5f} ".format(*elem_curve)
		else :
			if local_elem_result_path[ind_elem - 1][6] == elem_curve[0] and local_elem_result_path[ind_elem - 1][7] == elem_curve[1] :
				filled_elem_bezier_st = filled_elem_bezier_st + "{2:.5f} {3:.5f} {4:.5f} {5:.5f} {6:.5f} {7:.5f} ".format(*elem_curve)
			else :
				filled_elem_bezier_st = filled_elem_bezier_st + "M {0:.5f} {1:.5f} C {2:.5f} {3:.5f} {4:.5f} {5:.5f} {6:.5f} {7:.5f} ".format(*elem_curve)
		ind_elem += 1
	filled_elem_bezier_st = filled_elem_bezier_st + '"/>'
	return filled_elem_bezier_st

def elementary_bezier_file_paths(input_lines) :
	elementary_bezier_string_out = []
	filled_elementary_bezier_string_out = []
	for line_string in input_lines :
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		# print("Initial line_string [", line_string, "]")
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
				print('Expected path not found')
			else :
				path_data_string = result_path.group(1)
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
				print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
				print('Expected style not found')
			elem_result_path = []
			if result_ID and result_path and path_data_string != '' :
				##################################################################
				# transforms relative coordinates of base video into absolute ones
				##################################################################	
				# print("relative_to_absolute_coordinates [", local_path_data_string, "]")
				local_path_data = re.split(r' +' , path_data_string.rstrip())
				local_path_data_size = len(local_path_data)

				index = 0
				total_length = 0
				curve_lengths = []
				curve_nodes = []
				absolute_current_point_X = 0
				absolute_current_point_Y = 0
				while( index < local_path_data_size) :
					# move: ignored
					if(local_path_data[index] == "M" and index + 3 < local_path_data_size and local_path_data[index + 3] == "M") :
						absolute_preceding_point_X = absolute_current_point_X
						absolute_preceding_point_Y = absolute_current_point_Y
						absolute_current_point_X = force_num(local_path_data[index])
						absolute_current_point_Y = force_num(local_path_data[index + 1])
						index += 3
					elif(local_path_data[index] == "M") :
						index += 1
						# processes the next moveto and possible following lineto
						initial_index = index
						while(True) :
							absolute_preceding_point_X = absolute_current_point_X
							absolute_preceding_point_Y = absolute_current_point_Y
							absolute_current_point_X = force_num(local_path_data[index])
							absolute_current_point_Y = force_num(local_path_data[index + 1])
							subpath_initial_point_X = absolute_current_point_X
							subpath_initial_point_Y = absolute_current_point_Y
							# if it is not the first move, builds a segment 
							if index != initial_index :
								elem_result_path.append([absolute_preceding_point_X, absolute_preceding_point_Y, \
													.75 * absolute_preceding_point_X + .25 * absolute_current_point_X, .75 * absolute_preceding_point_Y + .25 * absolute_current_point_Y, \
													.25 * absolute_preceding_point_X + .75 * absolute_current_point_X, .25 * absolute_preceding_point_Y + .75 * absolute_current_point_Y, \
													absolute_current_point_X, absolute_current_point_Y])
							index += 2

							if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
								break
										
					elif(local_path_data[index] == "C") :
						index += 1
						# processes the next curve segments
						while(True) :
							# updates the current point for the next curve
							absolute_preceding_point_X = absolute_current_point_X
							absolute_preceding_point_Y = absolute_current_point_Y
							absolute_current_point_X = force_num(local_path_data[index + 4])
							absolute_current_point_Y = force_num(local_path_data[index + 5])
							elem_result_path.append([absolute_preceding_point_X, absolute_preceding_point_Y, \
												force_num(local_path_data[index + 0]), force_num(local_path_data[index + 1]), \
												force_num(local_path_data[index + 2]), force_num(local_path_data[index + 3]), \
												absolute_current_point_X, absolute_current_point_Y])
							index += 6
						
							if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
								break
					
					elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
						absolute_preceding_point_X = absolute_current_point_X
						absolute_preceding_point_Y = absolute_current_point_Y
						absolute_current_point_X = subpath_initial_point_X
						absolute_current_point_Y = subpath_initial_point_Y
						elem_result_path.append([absolute_preceding_point_X, absolute_preceding_point_Y, \
											.75 * absolute_preceding_point_X + .25 * absolute_current_point_X, .75 * absolute_preceding_point_Y + .25 * absolute_current_point_Y, \
											.25 * absolute_preceding_point_X + .75 * absolute_current_point_X, .25 * absolute_preceding_point_Y + .75 * absolute_current_point_Y, \
											absolute_current_point_X, absolute_current_point_Y])
						index += 1
					
					else :
						print( "Line: ", path_data_string)
						print( "elementary_bezier_file_paths Unexpected sign at index "+str(index)+": ("+str(local_path_data[index])+")")
						sys.exit(2)

			# builds the path of the additional paths
			# first: the individual paths separated -> the filling is not correct but the data are easier to process curve by curve for the interpolation
			separator = ' '
			nb_elementary_curves = len(elem_result_path)
			path_data_string = separator.join(list(map(lambda x: "M {0:.5f} {1:.5f} C {2:.5f} {3:.5f} {4:.5f} {5:.5f} {6:.5f} {7:.5f}".format(*x), elem_result_path)))
			elementary_bezier_string = '<path id="{0!s}" style="{1!s}" vv_length="{2:d}" d="{3!s}"/>'.format(result_ID.group(1), result_style.group(1), nb_elementary_curves, path_data_string)
			

			# second: the individual paths connected everytime it is possible. In this case the filling is correct
			filled_elementary_bezier_string = filled_string_from_curves(elem_result_path, result_ID.group(1), result_style.group(1), nb_elementary_curves)

			# output both lines
			elementary_bezier_string_out.append( elementary_bezier_string )
			filled_elementary_bezier_string_out.append( filled_elementary_bezier_string )

		else :
			# non path is output without modifications
			elementary_bezier_string_out.append( line_string )
			filled_elementary_bezier_string_out.append( line_string )

		# print("Absolute coordinates [", line_string, "]")
	return [elementary_bezier_string_out, filled_elementary_bezier_string_out]

##################################################################
# SVG file normalization, absolute coordinates and transformations into elementary bezier curves
def piecewise_curve_file_paths(local_input_file_name, local_output_file_name) :
	# VECTOR IMAGE FILE OPENING
	try:
		FILEin = open(local_input_file_name, "rt")
	except IOError:
		print("File not found :", local_input_file_name, " or path is incorrect")

	# STORES THE LINES IN AN ARRAY
	contents = FILEin.read()
	Unormalized_SVG_file_lines = contents.splitlines()
	# VECTOR IMAGE FILE CLOSING
	FILEin.close()

	# try:
	# 	FILEout = open(re.sub(r'.svg', r'unnormalized.svg', local_output_file_name), "wt")
	# except IOError:
	# 	print("File not created :", re.sub(r'.svg', r'unnormalized.svg', local_output_file_name), "for piecewise_curve_file_paths")
	# # COPIES THE LINES IN THE OUTPUT FILE
	# for line_string in Unormalized_SVG_file_lines :
	# 	# print("transformed line_string (",line_string,")")
	# 	FILEout.write(str(line_string)+"\n")
	# FILEout.close()

	# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
	SVG_file_lines = normalize_file_paths(Unormalized_SVG_file_lines)

	# try:
	# 	FILEout = open(re.sub(r'.svg', r'normalized.svg', local_output_file_name), "wt")
	# except IOError:
	# 	print("File not created :", re.sub(r'.svg', r'normalized.svg', local_output_file_name), "for piecewise_curve_file_paths")
	# # COPIES THE LINES IN THE OUTPUT FILE
	# for line_string in SVG_file_lines :
	# 	# print("transformed line_string (",line_string,")")
	# 	FILEout.write(str(line_string)+"\n")
	# FILEout.close()

	# MAKES ABSOLUTE COORDINATES
	absoluteCoords_SVG_curves = absolute_coordinates_file_paths(SVG_file_lines)

	# TRANSFORMS EACH PATH INTO A SET OF ELEMENTARY CURVES: polylines and polycurves are split into elementary bezier curves
	elementary_bezier_SVG_curves, filled_elementary_bezier_SVG_curves = elementary_bezier_file_paths(absoluteCoords_SVG_curves)

	# POSSIBLY WRITES THE FILE IN CASE IT IS USED SEVERAL TIMES
	if local_output_file_name != None :
		try:
			FILEout = open(local_output_file_name, "wt")
		except IOError:
			print("File not created :", local_output_file_name, "for piecewise_curve_file_paths")
		# COPIES THE LINES IN THE OUTPUT FILE
		for line_string in elementary_bezier_SVG_curves :
			# print("transformed line_string (",line_string,")")
			FILEout.write(str(line_string)+"\n")
		FILEout.close()

		filled_file_name = re.sub(r'(_[0-9]+)\.svg$', r'_filled\1.svg', local_output_file_name)
		try:
			FILEout = open(filled_file_name, "wt")
		except IOError:
			print("File not created :", filled_file_name, "for filled piecewise_curve_file_paths")
		# COPIES THE LINES IN THE OUTPUT FILE
		for line_string in filled_elementary_bezier_SVG_curves :
			# print("transformed line_string (",line_string,")")
			FILEout.write(str(line_string)+"\n")
		FILEout.close()
		return None
	else :
		return [elementary_bezier_SVG_curves, filled_elementary_bezier_SVG_curves]

##################################################################
# piecewise segments length calculation of a path
##################################################################
def path_curves_and_lengths(a_path_made_of_curves) :
	##################################################################
	# reads the path and transform it into absolute coordinates
	##################################################################
	print("Initial path [", a_path_made_of_curves[0:100], "]")
	if( re.search(r'^<path', a_path_made_of_curves) != None):
		# only keeps the ID and the path data
		result_path = re.search(r'\sd="([^\"]*)\"', a_path_made_of_curves)
		if not result_path:
			print('Expected path not found')
		else :
			path_data_string = result_path.group(1)

			# print("relative_to_absolute_coordinates [", local_path_data_string, "]")
			local_path_data = re.split(r' +' , path_data_string.rstrip())
			local_path_data_size = len(local_path_data)

			index = 0
			total_length = 0
			curve_lengths = []
			curve_nodes = []
			while( index < local_path_data_size) :
				if(local_path_data[index] == "M") :
					index += 1
					# processes the next moveto and possible following lineto
					while(True) :
						absolute_current_point_X = force_num(local_path_data[index])
						absolute_current_point_Y = force_num(local_path_data[index + 1])
						subpath_initial_point_X = absolute_current_point_X
						subpath_initial_point_Y = absolute_current_point_Y
						index += 2

						if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
							break
									
				elif(local_path_data[index] == "C") :
					index += 1
					# processes the next curve segments
					while(True) :
						# calculate length
						one_curve_length = 0
						one_curve_length += length(absolute_current_point_X, absolute_current_point_Y, force_num(local_path_data[index]), force_num(local_path_data[index + 1]))
						one_curve_length += length(force_num(local_path_data[index]), force_num(local_path_data[index + 1]), force_num(local_path_data[index + 2]), force_num(local_path_data[index + 3]))
						one_curve_length += length(force_num(local_path_data[index + 2]), force_num(local_path_data[index + 3]), force_num(local_path_data[index + 4]), force_num(local_path_data[index + 5]))
						# and stores it into a list
						curve_lengths.append(one_curve_length)
						# stores the list of the nodes as arrays for the Bezier lib
						curve_nodes.append(np.asfortranarray([[absolute_current_point_X, force_num(local_path_data[index]), force_num(local_path_data[index + 2]), force_num(local_path_data[index + 4])], [absolute_current_point_Y, force_num(local_path_data[index + 1]), force_num(local_path_data[index + 3]), force_num(local_path_data[index + 5])]]))
						# cumulates the length
						total_length += one_curve_length
						# updates the current point for the next curve
						absolute_current_point_X = force_num(local_path_data[index + 4])
						absolute_current_point_Y = force_num(local_path_data[index + 5])
						index += 6
					
						if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
							break
				
				elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
					absolute_current_point_X = subpath_initial_point_X
					absolute_current_point_Y = subpath_initial_point_Y
					index += 1
				
				else :
					print( "Line: ", local_path_data_string)
					print( "path_curves_and_lengths Unexpected sign at index "+str(index)+": ("+str(local_path_data[index])+")")
					sys.exit(2)

			return [total_length, curve_lengths, curve_nodes]

##################################################################
# SOUND DATA MANAGEMENT
##################################################################
##################################################################
# RAW SOUND DATA STORAGE (WAV FILE TO WAV.CSV)
def read_wav_data(local_val_init) :
	# if it is given, opens the file with the audio data for pulsed colors
	local_csv_FILEsoundtrackout = None
	local_wav_FILEsoundtrackin = None
	nb_audio_samples = 0
	soundtrack_working_channels = 1
	if "soundtrack_directory" in local_val_init and "soundtrack_data_file" in local_val_init and "soundsample_window" in local_val_init and "soundsmoothing_window" in local_val_init and "fps" in local_val_init:
		soundtrack_directory = local_val_init["soundtrack_directory"]
		soundtrack_data_file = local_val_init["soundtrack_data_file"]
		if "soundtrack_working_channels" in local_val_init :
			soundtrack_working_channels = int(to_num(local_val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		if soundtrack_data_file != "None" :
			soundtrack_wav_file = re.sub('.csv$', '', soundtrack_data_file)
			print("opening wav file", os.path.join(soundtrack_directory, soundtrack_wav_file))
			print("opening wav.csv file", os.path.join(soundtrack_directory, soundtrack_data_file))

			################################################
			# WRITES SIGNAL VALUES EXTRACTED FROM WAV FILE
			# opens the csv file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_wav_FILEsoundtrackin = wave.open(os.path.join(soundtrack_directory, soundtrack_wav_file), 'rb')
			except IOError:
				print("File not found:", os.path.join(soundtrack_directory, soundtrack_wav_file), " or path is incorrect")
			try:
				local_csv_FILEsoundtrackout = open(os.path.join(soundtrack_directory, soundtrack_data_file), "wt")
			except IOError:
				print("File not opened:", os.path.join(soundtrack_directory, soundtrack_data_file), " or path is incorrect")

			# CSV WRITER
			local_CSVwriter = csv.writer(local_csv_FILEsoundtrackout, delimiter=',')
			# WAV FILE DATA
			samplerate = local_wav_FILEsoundtrackin.getframerate()
			nb_channels = local_wav_FILEsoundtrackin.getnchannels()
			nb_audio_samples = int(local_wav_FILEsoundtrackin.getnframes())
			if nb_channels != 1 and nb_channels != 2:
				print("only mono or stereo files are processed")
				sys.exit(0)
			print(f"number of channels = {nb_channels}")
			soundtrack_duration = nb_audio_samples / samplerate
			print(f"sound file duration (sec) = {soundtrack_duration}s")
			# READS THE SOUND FILE DATA LINE BY LINE
			# writes lists [timestamp, value]
			# header with nb_channels, sample_rate, and duration
			local_CSVwriter.writerow(["nb_channels", "{:d}".format(nb_channels)])
			local_CSVwriter.writerow(["sample_rate", "{:d}".format(samplerate)])
			local_CSVwriter.writerow(["duration", "{:.4f}".format(soundtrack_duration)])
			for ind in range(nb_audio_samples) :
				timestamp = float(ind) / float(samplerate)
				string = local_wav_FILEsoundtrackin.readframes(1)
				if soundtrack_working_channels == 1 :
					if nb_channels == 2 : # stereo to mono
						sample = struct.unpack('hh', string)
						sample_value = float(sample[0] + sample[1]) / 2.0 / 32768.0
					else :
						sample_value = float(struct.unpack('h', string)) / 32768.0
					timestamp_string = "{:.4f}".format(timestamp)
					sample_value_string = "{:.6f}".format(sample_value)
					local_CSVwriter.writerow([timestamp_string, sample_value_string])
				else : # soundtrack_working_channels == 2
					if nb_channels == 2 :
						sample = struct.unpack('hh', string)
						sample_value0 = sample[0] / 32768.0
						sample_value1 = sample[1] / 32768.0
					else :  # mono to stereo
						sample_value0 = float(struct.unpack('h', string)) / 32768.0
						sample_value1 = sample_value0
					timestamp_string = "{:.6f}".format(timestamp)
					sample_value0_string = "{:.6f}".format(sample_value0)
					sample_value1_string = "{:.6f}".format(sample_value1)
					local_CSVwriter.writerow([timestamp_string, sample_value0_string, sample_value1_string])
			local_wav_FILEsoundtrackin.close()
			local_csv_FILEsoundtrackout.close()

	return nb_audio_samples

##################################################################
# COMPUTED SOUND DATA ANALYSIS AND STORAGE (WAV.CSV TO WAV_ENVELOPE_XX AND WAV_ENVELOPE_XX_SMOOTHED_YYY)
def max_value_in_window(local_soundtrack_value_window) :
	# update the maximal value in the window
	# print(local_soundtrack_value_window)
	analytic_signal = hilbert(local_soundtrack_value_window)
	amplitude_envelope = np.abs(analytic_signal)
	# print("amplitude_envelope", amplitude_envelope)
	soundtrack_envelope_max = max(amplitude_envelope)
	soundtrack_envelope_min = min(amplitude_envelope)
	instant_envelope = max(abs(soundtrack_envelope_min), abs(soundtrack_envelope_max))
	return instant_envelope

def interpolate_between_peak_and_min_values(Envelope_Signal, peakindices, Envelope_index, next_peak) :
	# current signal value
	current_value = Envelope_Signal[Envelope_index]
	if next_peak > 0 :
		# minimal value between the two surrounding peaks
		previous_peak_value = Envelope_Signal[peakindices[next_peak - 1]]
		next_peak_value = Envelope_Signal[peakindices[next_peak]]

		# if the two surrounding peaks are not side by side
		# index of the minimal value between the two surrounding peaks
		if(peakindices[next_peak - 1] + 1 < peakindices[next_peak]) :
			inbetweenpeak_minimal_value = min(Envelope_Signal[peakindices[next_peak - 1] + 1:peakindices[next_peak]])
			min_index = Envelope_Signal[peakindices[next_peak - 1] + 1:peakindices[next_peak]].index(inbetweenpeak_minimal_value)
			min_index += peakindices[next_peak - 1] + 1
		# otherwise the minimal value is the one after the previous peak (== the next peak)
		else :
			inbetweenpeak_minimal_value = Envelope_Signal[peakindices[next_peak - 1] + 1]
			min_index = peakindices[next_peak - 1] + 1

		# interpolates the value between the minimal value and the peak on the other side of the current peak
		if Envelope_index < min_index and previous_peak_value > inbetweenpeak_minimal_value:
			interpolated_value = (current_value - inbetweenpeak_minimal_value) / (previous_peak_value - inbetweenpeak_minimal_value)
		elif Envelope_index >= min_index and next_peak_value > inbetweenpeak_minimal_value :
			interpolated_value = (current_value - inbetweenpeak_minimal_value) / (next_peak_value - inbetweenpeak_minimal_value)
		else :
			interpolated_value = current_value
	else :
		interpolated_value = current_value
	return interpolated_value


def collect_smoothed_envelope_at_onsets(soundtrack_directory, soundtrack_file, soundtrack_working_channels, soundtrack_smoothed_envelope_file, soundtrack_onsets_file0, soundtrack_onsets_file1, soundtrack_onset_envelope_file) :
	Smoothed_Envelope_Signal0 = []
	Smoothed_Envelope_Signal1 = []
	Timestamp = []
	Onset_Timestamps0 = []
	Onset_Timestamps1 = []

	################################################
	# READS SMOOTHED ENVELOPPE VALUES 
	# opens the file and reads in the CSV mode (mono 2 columns [timestamp, value], stereo 3 columns [timestamp, value, value], one sample/line)
	try:
		local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), "rt")
	except IOError:
		print("File not found :", os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), " or path is incorrect")

	# CSV READER
	local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
	# READS THE SOUND FILE DATA LINE BY LINE
	# Pass reader object to list() to get a list of lists [timestamp, value]
	basic_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
	local_FILEsoundtrackin.close()

	# separates list of timestamps and list of values
	for timestamp_signal in basic_Timestamp_Signal_sound_data[3:] :
		Timestamp.append(timestamp_signal[0])
		if soundtrack_working_channels == 1 :
			Smoothed_Envelope_Signal0.append(timestamp_signal[1])
		else :
			Smoothed_Envelope_Signal0.append(timestamp_signal[1])
			Smoothed_Envelope_Signal1.append(timestamp_signal[2])
	local_FILEsoundtrackin.close()

	################################################
	# READS ONSETS 
	# opens the file and reads in the CSV mode (mono 1 column [timestamp])
	try:
		local_FILEonsetin0 = open(os.path.join(soundtrack_directory, soundtrack_onsets_file0), "rt")
	except IOError:
		print("File not found :", os.path.join(soundtrack_directory, soundtrack_onsets_file0), " or path is incorrect")
	# CSV READER
	local_readonsetCSV0 = csv.reader(local_FILEonsetin0, delimiter=',')
	# READS THE SOUND FILE DATA LINE BY LINE
	# Pass reader object to list() to get a list of lists [timestamps]
	Onset_Timestamps0 = list(local_readonsetCSV0)
	Onset_Timestamps0 = [item[0] for item in Onset_Timestamps0]
	# print(Onset_Timestamps0)
	local_FILEonsetin0.close()

	if soundtrack_working_channels == 2 :
		try:
			local_FILEonsetin1 = open(os.path.join(soundtrack_directory, soundtrack_onsets_file1), "rt")
		except IOError:
			print("File not found :", os.path.join(soundtrack_directory, soundtrack_onsets_file1), " or path is incorrect")
		# CSV READER
		local_readonsetCSV1 = csv.reader(local_FILEonsetin1, delimiter=',')
		# READS THE SOUND FILE DATA LINE BY LINE
		# Pass reader object to list() to get a list of lists [timestamps]
		Onset_Timestamps1 = list(local_readonsetCSV1)
		Onset_Timestamps1 = [item[0] for item in Onset_Timestamps1]
		local_FILEonsetin1.close()

	# opens the file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
	try:
		local_FILEonsetSignalout = open(os.path.join(soundtrack_directory, soundtrack_onset_envelope_file), "wt")
	except IOError:
		print("File not found :", os.path.join(soundtrack_directory, soundtrack_onset_envelope_file), " or path is incorrect")
	local_CSVwriter = csv.writer(local_FILEonsetSignalout, delimiter=',')

	# time stamps and associated peaked values
	if soundtrack_working_channels == 1 :
		# processes indices in ascending order in parallel
		Onset_rank0 = 0
		Onset_rank1 = 0
		Timestamp_rank = 0
		while Onset_rank0 < len(Onset_Timestamps0) and Timestamp_rank < len(Timestamp) :
			if(to_num(Onset_Timestamps0[Onset_rank0]) <= to_num(Timestamp[Timestamp_rank])) :
				local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], Smoothed_Envelope_Signal0[Timestamp_rank]])
				Onset_rank0 += 1
			else : # Onset_Timestamps0[Onset_rank0] > Timestamp[Timestamp_rank]
				Timestamp_rank += 1
		while (Onset_rank0 < len(Onset_Timestamps0)) : # Timestamp_rank >= len(Timestamp) :
				local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], Smoothed_Envelope_Signal0[Timestamp_rank - 1]])
				Onset_rank0 += 1
	else :
		# processes indices in ascending order in parallel
		Onset_rank0 = 0
		Onset_rank1 = 0
		Timestamp_rank = 0
		while Onset_rank0 < len(Onset_Timestamps0) and Onset_rank1 < len(Onset_Timestamps1) :
			# print(Onset_rank0, len(Onset_Timestamps0), Onset_rank1, len(Onset_Timestamps1), Timestamp_rank, len(Timestamp), len(Smoothed_Envelope_Signal0), len(Smoothed_Envelope_Signal1))
			if(to_num(Onset_Timestamps0[Onset_rank0]) == to_num(Onset_Timestamps1[Onset_rank1]) and to_num(Onset_Timestamps0[Onset_rank0]) <= to_num(Timestamp[Timestamp_rank])) :
				local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], Onset_Timestamps1[Onset_rank1], Smoothed_Envelope_Signal0[Timestamp_rank], Smoothed_Envelope_Signal1[Timestamp_rank]])
				Onset_rank0 += 1
				Onset_rank1 += 1
			elif(to_num(Onset_Timestamps0[Onset_rank0]) < to_num(Onset_Timestamps1[Onset_rank1]) and to_num(Onset_Timestamps0[Onset_rank0]) <= to_num(Timestamp[Timestamp_rank])) :
				local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], -1, Smoothed_Envelope_Signal0[Timestamp_rank], Smoothed_Envelope_Signal1[Timestamp_rank]])
				Onset_rank0 += 1
			elif(to_num(Onset_Timestamps1[Onset_rank1]) <= to_num(Timestamp[Timestamp_rank])) : # Onset_Timestamps0[Onset_rank0] > Onset_Timestamps1[Onset_rank1]
				local_CSVwriter.writerow([-1, Onset_Timestamps1[Onset_rank1], Smoothed_Envelope_Signal0[Timestamp_rank], Smoothed_Envelope_Signal1[Timestamp_rank]])
				Onset_rank1 += 1
			else : # Onset_Timestamps0[Onset_rank0] > Timestamp[Timestamp_rank] and Onset_Timestamps1[Onset_rank1] > Timestamp[Timestamp_rank]
				if( Timestamp_rank < len(Timestamp) - 1) :
					Timestamp_rank += 1
		if(Onset_rank0 >= len(Onset_Timestamps0)) :
			while Onset_rank1 < len(Onset_Timestamps1) and Timestamp_rank < len(Timestamp) :
				if(to_num(Onset_Timestamps1[Onset_rank1]) <= to_num(Timestamp[Timestamp_rank])) :
					local_CSVwriter.writerow([-1, Onset_Timestamps1[Onset_rank1], Smoothed_Envelope_Signal0[Timestamp_rank], Smoothed_Envelope_Signal1[Timestamp_rank]])
					Onset_rank1 += 1
				else : # Onset_Timestamps1[Onset_rank1] > Timestamp[Timestamp_rank]
					Timestamp_rank += 1
			while (Onset_rank1 < len(Onset_Timestamps1)) : # Timestamp_rank >= len(Timestamp) :
					local_CSVwriter.writerow([-1, Onset_Timestamps1[Onset_rank1], Smoothed_Envelope_Signal0[Timestamp_rank - 1], Smoothed_Envelope_Signal1[Timestamp_rank - 1]])
					Onset_rank1 += 1
		else :
			while Onset_rank0 < len(Onset_Timestamps0) and Timestamp_rank < len(Timestamp) :
				if(to_num(Onset_Timestamps0[Onset_rank0]) <= to_num(Timestamp[Timestamp_rank])) :
					local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], -1, Smoothed_Envelope_Signal0[Timestamp_rank], Smoothed_Envelope_Signal0[Timestamp_rank]])
					Onset_rank0 += 1
				else : # Onset_Timestamps0[Onset_rank0] > Timestamp[Timestamp_rank]
					Timestamp_rank += 1
			while (Onset_rank0 < len(Onset_Timestamps0)) : # Timestamp_rank >= len(Timestamp) :
					local_CSVwriter.writerow([Onset_Timestamps0[Onset_rank0], -1, Smoothed_Envelope_Signal0[Timestamp_rank - 1], Smoothed_Envelope_Signal0[Timestamp_rank - 1]])
					Onset_rank0 += 1
	local_FILEonsetSignalout.close()
	

def read_write_sound_data(local_val_init) :
	global Signal0
	global Envelope_Signal0
	global Peaked_Envelope_Signal0
	global Smoothed_Envelope_Signal0
	global Signal1
	global Envelope_Signal1
	global Peaked_Envelope_Signal1
	global Smoothed_Envelope_Signal1

	global Timestamp_current_index
	global Envelope_Timestamp_current_index
	global Smoothed_Envelope_Timestamp_current_index

	global Timestamp
	global Envelope_timestamp
	global Smoothed_Envelope_timestamp

	global current_Timestamp
	global Envelope_current_Timestamp
	global Smoothed_Envelope_current_Timestamp


	# if it is given, opens the file with the audio data for pulsed colors
	returned_fps = 30
	returned_pulse_coef = 1.0
	returned_with_sound_data = False
	local_FILEsoundtrackin = None
	local_readsoundtrackCSV = None
	soundtrack_onsets_threshold = 0.3
	soundtrack_min_inter_onset_interval  = 0.02
	soundtrack_onsets_method = 'hfc'
	soundtrack_working_channels = 1
	if "soundtrack_directory" in local_val_init and "soundtrack_data_file" in local_val_init and "soundsample_window" in local_val_init and "soundsmoothing_window" in local_val_init and "fps" in local_val_init:
		soundtrack_directory = local_val_init["soundtrack_directory"]
		soundtrack_data_file = local_val_init["soundtrack_data_file"]
		if "soundtrack_working_channels" in local_val_init :
			soundtrack_working_channels = int(to_num(local_val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		returned_fps = local_val_init["fps"]
		if soundtrack_data_file != "None" :
			print("opening sound file", os.path.join(soundtrack_directory, soundtrack_data_file))
			soundsample_window = int(to_num(local_val_init["soundsample_window"]))
			soundtrack_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '.csv', soundtrack_data_file)
			soundsmoothing_window = int(to_num(local_val_init["soundsmoothing_window"]))
			soundtrack_smoothed_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '_smoothed_' + str(soundsmoothing_window) + '.csv', soundtrack_data_file)
			if "soundtrack_onsets_threshold" in local_val_init :
				soundtrack_onsets_threshold = to_num(local_val_init["soundtrack_onsets_threshold"])
			if "soundtrack_min_inter_onset_interval" in local_val_init :
				soundtrack_min_inter_onset_interval = to_num(local_val_init["soundtrack_min_inter_onset_interval"])
			if "soundtrack_onsets_method" in local_val_init :
				soundtrack_onsets_method = local_val_init["soundtrack_onsets_method"]
			soundtrack_onsets_threshold_string = "{:.3f}".format(soundtrack_onsets_threshold)
			soundtrack_min_inter_onset_interval_string = "{:.3f}".format(soundtrack_min_inter_onset_interval)
			if soundtrack_working_channels == 1 :
				soundtrack_onsets_file0 = re.sub('.csv$', '_onsets_' + soundtrack_onsets_method + '_' + soundtrack_onsets_threshold_string + '_' + soundtrack_min_inter_onset_interval_string + '.csv', soundtrack_data_file)
				soundtrack_onsets_file1 = ""
			else :
				soundtrack_onsets_file0 = re.sub('.csv$', '_onsets0_' + soundtrack_onsets_method + '_' + soundtrack_onsets_threshold_string + '_' + soundtrack_min_inter_onset_interval_string + '.csv', soundtrack_data_file)
				soundtrack_onsets_file1 = re.sub('.csv$', '_onsets1_' + soundtrack_onsets_method + '_' + soundtrack_onsets_threshold_string + '_' + soundtrack_min_inter_onset_interval_string + '.csv', soundtrack_data_file)
			soundtrack_onset_envelope_file = soundtrack_onsets_file0.replace(".csv", "_envelope.csv")
			soundtrack_file = re.sub('.csv$', '', soundtrack_data_file)
			soundtrack_envelope_peak_file = re.sub('.csv$', '_envelope_peak_' + str(soundsample_window) + '.csv', soundtrack_data_file)
			soundtrack_peaked_envelope_file = re.sub('.csv$', '_peaked_envelope_' + str(soundsample_window) + '.csv', soundtrack_data_file)

			################################################
			# READS SIGNAL VALUES EXTRACTED FROM WAV FILE
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			print("Read wav file...", soundtrack_data_file)
			try:
				local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_data_file), "rt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_data_file), " or path is incorrect")

			# CSV READER
			local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
			# READS THE SOUND FILE DATA LINE BY LINE
			# Pass reader object to list() to get a list of lists [timestamp, value]
			basic_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
			local_FILEsoundtrackin.close()

			# separates list of timestamps and list of values
			for timestamp_signal in basic_Timestamp_Signal_sound_data[3:] :
				Timestamp.append(timestamp_signal[0])
				if soundtrack_working_channels == 1 :
					Signal0.append(timestamp_signal[1])
				else :
					Signal0.append(timestamp_signal[1])
					Signal1.append(timestamp_signal[2])

			################################################
			# CALCULATES AND STORES ENVELOPES
			print("Envelope calculation...")
			Envelope_Signal0 = []
			Envelope_Signal1 = []
			Envelope_timestamp = []
			Envelope_max_value0 = 0.
			Envelope_max_value1 = 0.
			half_soundsample_window = int(soundsample_window / 2)
			for index in range(half_soundsample_window, len(Signal0) - half_soundsample_window, 8) :
				if soundtrack_working_channels == 1 :
					local_soundtrack0_value_window = Signal0[index - half_soundsample_window: index + half_soundsample_window]
					instant_envelope0 = max_value_in_window(local_soundtrack0_value_window)
					if instant_envelope0 > Envelope_max_value0 :
						Envelope_max_value0 = instant_envelope0
					Envelope_Signal0.append(instant_envelope0)
				else :
					local_soundtrack0_value_window = Signal0[index - half_soundsample_window: index + half_soundsample_window]
					local_soundtrack1_value_window = Signal1[index - half_soundsample_window: index + half_soundsample_window]
					instant_envelope0 = max_value_in_window(local_soundtrack0_value_window)
					instant_envelope1 = max_value_in_window(local_soundtrack1_value_window)
					if instant_envelope0 > Envelope_max_value0 :
						Envelope_max_value0 = instant_envelope0
					if instant_envelope1 > Envelope_max_value1 :
						Envelope_max_value1 = instant_envelope1
					Envelope_Signal0.append(instant_envelope0)
					Envelope_Signal1.append(instant_envelope1)
				Envelope_timestamp.append(Timestamp[index])

			# opens the file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsoundtrackout = open(os.path.join(soundtrack_directory, soundtrack_envelope_file), "wt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_envelope_file), " or path is incorrect")
			local_CSVwriter = csv.writer(local_FILEsoundtrackout, delimiter=',')

			# header with nb_channels, sample_rate, and duration
			for timestamp_signal in basic_Timestamp_Signal_sound_data[0:3] :
				local_CSVwriter.writerow([timestamp_signal[0], timestamp_signal[1]])
			# envelope normalization 
			if soundtrack_working_channels == 1 :
				if Envelope_max_value0 != 0. :
					Envelope_Signal0 = [i / Envelope_max_value0 for i in Envelope_Signal0]
				# time stamps and associated normalized values
				for (timestamp_string, value0) in zip(Envelope_timestamp, Envelope_Signal0):
					value0_string = "{:.6f}".format(value0)
					local_CSVwriter.writerow([timestamp_string, value_string0])
			else :
				if Envelope_max_value0 != 0. :
					Envelope_Signal0 = [i / Envelope_max_value0 for i in Envelope_Signal0]
				if Envelope_max_value1 != 0. :
					Envelope_Signal1 = [i / Envelope_max_value1 for i in Envelope_Signal1]
				# time stamps and associated normalized values
				for (timestamp_string, value0, value1) in zip(Envelope_timestamp, Envelope_Signal0, Envelope_Signal1):
					value0_string = "{:.6f}".format(value0)
					value1_string = "{:.6f}".format(value1)
					local_CSVwriter.writerow([timestamp_string, value0_string, value1_string])
			local_FILEsoundtrackout.close()

			################################################
			# CALCULATES AND STORES PEAKS of the envelope
			# the envelope is sampled 44100/8 = 5512, 
			# the distance between two peaks is every 6th of seconds (5 frames) ~ 920 samples
			# peakindices, _ = find_peaks(Envelope_Signal, distance=920)
			# the distance between two peaks is every 0.05s ~ 270 samples
			print("Envelope peaks calculation...")
			if soundtrack_working_channels == 1 :
				peakindices0, _ = find_peaks(Envelope_Signal0, distance=270, prominence=0.15)
			else :
				peakindices0, _ = find_peaks(Envelope_Signal0, distance=270, prominence=0.15)
				peakindices1, _ = find_peaks(Envelope_Signal1, distance=270, prominence=0.15)
			# opens the file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsoundtrackout = open(os.path.join(soundtrack_directory, soundtrack_envelope_peak_file), "wt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_envelope_peak_file), " or path is incorrect")
			local_CSVwriter = csv.writer(local_FILEsoundtrackout, delimiter=',')

			# header with nb_channels, sample_rate, and duration
			for timestamp_signal in basic_Timestamp_Signal_sound_data[0:3] :
				local_CSVwriter.writerow([timestamp_signal[0], timestamp_signal[1]])

			Peaked_timestamp_values = []
			peakindices = []

			# time stamps and associated normalized values
			if soundtrack_working_channels == 1 :
				for peak_index0 in peakindices0 :
					Peaked_timestamp_values.append([Envelope_timestamp[peak_index0], Envelope_Signal0[peak_index0]])
					peakindices.append(peak_index0)
				for (timestamp, signal0) in Peaked_timestamp_values :
					local_CSVwriter.writerow([timestamp, "{:.6f}".format(signal0)])
			else :
				# processes indices in ascending order in parallel
				peak_rank0 = 0
				peak_rank1 = 0
				while peak_rank0 < len(peakindices0) and peak_rank1 < len(peakindices1) :
					if(peakindices0[peak_rank0] == peakindices1[peak_rank1]) :
						Peaked_timestamp_values.append([Envelope_timestamp[peakindices0[peak_rank0]], Envelope_Signal0[peakindices0[peak_rank0]], Envelope_Signal1[peakindices1[peak_rank1]]])
						peakindices.append(peakindices0[peak_rank0])
						peak_rank0 += 1
						peak_rank1 += 1
					elif(peakindices0[peak_rank0] < peakindices1[peak_rank1]) :
						Peaked_timestamp_values.append([Envelope_timestamp[peakindices0[peak_rank0]], Envelope_Signal0[peakindices0[peak_rank0]], Envelope_Signal1[peakindices0[peak_rank0]]])
						peakindices.append(peakindices0[peak_rank0])
						peak_rank0 += 1
					else :
						Peaked_timestamp_values.append([Envelope_timestamp[peakindices1[peak_rank1]], Envelope_Signal0[peakindices1[peak_rank1]], Envelope_Signal1[peakindices1[peak_rank1]]])
						peakindices.append(peakindices1[peak_rank1])
						peak_rank1 += 1
				# processes the indices of the highest rank
				while peak_rank0 < len(peakindices0) :
					Peaked_timestamp_values.append([Envelope_timestamp[peakindices0[peak_rank0]], Envelope_Signal0[peakindices0[peak_rank0]], Envelope_Signal1[peakindices0[peak_rank0]]])
					peakindices.append(peakindices0[peak_rank0])
					peak_rank0 += 1
				while peak_rank1 < len(peakindices1) :
					Peaked_timestamp_values.append([Envelope_timestamp[peakindices1[peak_rank1]], Envelope_Signal0[peakindices1[peak_rank1]], Envelope_Signal1[peakindices1[peak_rank1]]])
					peakindices.append(peakindices1[peak_rank1])
					peak_rank1 += 1
				for (timestamp, signal0, signal1) in Peaked_timestamp_values :
					local_CSVwriter.writerow([timestamp, "{:.6f}".format(signal0), "{:.6f}".format(signal1)])
			local_FILEsoundtrackout.close()

			################################################
			# CALCULATES AND STORES PEAKED ENVELOPE (with values interpolated between peaks and intermediary minimal values)
			# time stamps and associated normalized values
			Peaked_Envelope_Signal0 = []
			Peaked_Envelope_Signal1 = []

			previous_peak_value = 0.
			next_peak_value = 0.

			next_peak = 0

			for (timestamp_string, Envelope_index) in zip(Envelope_timestamp, range(len(Envelope_timestamp))):
				# locates the next (and the preceding) peak according to the current timestamp of the Envelope
				curr_timestamp = to_num(timestamp_string)
				while to_num(Envelope_timestamp[peakindices[next_peak]]) < curr_timestamp and next_peak < len(peakindices) - 1 :
					next_peak += 1

				# calculate the envelope signal at the peak by interpolating the minimal value between the two surrounding peak, 
				# the current one, and the peak on the other side of the current one wrt the min value
				interpolated_value0 = interpolate_between_peak_and_min_values(Envelope_Signal0, peakindices, Envelope_index, next_peak)
				Peaked_Envelope_Signal0.append(interpolated_value0)
				if soundtrack_working_channels == 2 :
					interpolated_value1 = interpolate_between_peak_and_min_values(Envelope_Signal1, peakindices, Envelope_index, next_peak)
					Peaked_Envelope_Signal1.append(interpolated_value1)

			# opens the file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsoundtrackout = open(os.path.join(soundtrack_directory, soundtrack_peaked_envelope_file), "wt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_peaked_envelope_file), " or path is incorrect")
			local_CSVwriter = csv.writer(local_FILEsoundtrackout, delimiter=',')

			# header with nb_channels, sample_rate, and duration
			for timestamp_signal in basic_Timestamp_Signal_sound_data[0:3] :
				local_CSVwriter.writerow([timestamp_signal[0], timestamp_signal[1]])
			# time stamps and associated peaked values
			if soundtrack_working_channels == 1 :
				for (timestamp_string, value0) in zip(Envelope_timestamp, Peaked_Envelope_Signal0):
					local_CSVwriter.writerow([timestamp_string, "{:.6f}".format(value0)])
			else :
				for (timestamp_string, value0, value1) in zip(Envelope_timestamp, Peaked_Envelope_Signal0, Peaked_Envelope_Signal1):
					local_CSVwriter.writerow([timestamp_string, "{:.6f}".format(value0), "{:.6f}".format(value1)])
			local_FILEsoundtrackout.close()

			################################################
			# CALCULATES AND STORES SMOOTHED ENVELOPES
			print("Smoothed envelope calculation...")
			Smoothed_Envelope_Signal0 = []
			Smoothed_Envelope_Signal1 = []
			Smoothed_Envelope_timestamp = []
			Smoothed_Envelope_max_value0 = 0.
			Smoothed_Envelope_max_value1 = 0.
			half_soundsmoothing_window = int(soundsmoothing_window / 2)
			for index in range(half_soundsmoothing_window, len(Envelope_Signal0) - half_soundsmoothing_window) :
				# time stamp
				Smoothed_Envelope_timestamp.append(Envelope_timestamp[index])
				# value
				local_soundtrack_value_window0 = Envelope_Signal0[index - half_soundsmoothing_window: index + half_soundsmoothing_window]
				# update the maximal value in the window
				# print(local_soundtrack_value_window)
				instant_smoothed_envelope0 = mean(local_soundtrack_value_window0)
				if instant_smoothed_envelope0 > Smoothed_Envelope_max_value0 :
					Smoothed_Envelope_max_value0 = instant_smoothed_envelope0
				Smoothed_Envelope_Signal0.append(instant_smoothed_envelope0)
				if soundtrack_working_channels == 2 :
					# value
					local_soundtrack_value_window1 = Envelope_Signal1[index - half_soundsmoothing_window: index + half_soundsmoothing_window]
					# update the maximal value in the window
					# print(local_soundtrack_value_window)
					instant_smoothed_envelope1 = mean(local_soundtrack_value_window1)
					if instant_smoothed_envelope1 > Smoothed_Envelope_max_value1 :
						Smoothed_Envelope_max_value1 = instant_smoothed_envelope1
					Smoothed_Envelope_Signal1.append(instant_smoothed_envelope1)

			# opens the file and writes in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsoundtrackout = open(os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), "wt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), " or path is incorrect")
			local_CSVwriter = csv.writer(local_FILEsoundtrackout, delimiter=',')

			# header with nb_channels, sample_rate, and duration
			for timestamp_signal in basic_Timestamp_Signal_sound_data[0:3] :
				local_CSVwriter.writerow([timestamp_signal[0], timestamp_signal[1]])

			# envelope normalization 
			if Smoothed_Envelope_max_value0 != 0. :
				Smoothed_Envelope_Signal0 = [i / Smoothed_Envelope_max_value0 for i in Smoothed_Envelope_Signal0]
			if soundtrack_working_channels == 1 :
				# time stamps and associated normalized values
				for (timestamp_string, value0) in zip(Smoothed_Envelope_timestamp, Smoothed_Envelope_Signal0):
					local_CSVwriter.writerow([timestamp_string, "{:.6f}".format(value0)])
			else :
				if Smoothed_Envelope_max_value1 != 0. :
					Smoothed_Envelope_Signal1 = [i / Smoothed_Envelope_max_value1 for i in Smoothed_Envelope_Signal1]
				# time stamps and associated normalized values
				for (timestamp_string, value0, value1) in zip(Smoothed_Envelope_timestamp, Smoothed_Envelope_Signal0, Smoothed_Envelope_Signal1):
					local_CSVwriter.writerow([timestamp_string, "{:.6f}".format(value0), "{:.6f}".format(value1)])
			local_FILEsoundtrackout.close()

			################################################
			# CALCULATES AND STORES ONSETS THROUGH AUBIO LIBRARY
			print("Onset calculation...")
			soundtrack_onsets_threshold_string = "{:.6f}".format(soundtrack_onsets_threshold)
			soundtrack_min_inter_onset_interval_string = "{:.6f}".format(soundtrack_min_inter_onset_interval)
			if soundtrack_working_channels == 1 :
				os.system('aubioonset -i ' + os.path.join(soundtrack_directory, soundtrack_file) + ' -O ' + soundtrack_onsets_method + ' -t ' + soundtrack_onsets_threshold_string + ' -M ' + soundtrack_min_inter_onset_interval_string + ' > ' + os.path.join(soundtrack_directory, soundtrack_onsets_file0))
			else :
				print("split channels of audio file", os.path.join(soundtrack_directory, soundtrack_file))
				# Open the stereo audio file as an AudioSegment instance
				stereo_audio = AudioSegment.from_file(os.path.join(soundtrack_directory, soundtrack_file), format="wav")
				# Calling the split_to_mono method on the stereo audio file
				mono_audios = stereo_audio.split_to_mono()
				# Exporting/Saving the two mono audio files present at index 0(left) and index 1(right) of list returned
				# by split_to_mono method
				mono_left = mono_audios[0].export(os.path.join(soundtrack_directory, 'tmp_channel_1.wav'), format="wav")
				mono_right = mono_audios[1].export(os.path.join(soundtrack_directory, 'tmp_channel_2.wav'), format="wav")
				os.system('aubioonset -i ' + os.path.join(soundtrack_directory, 'tmp_channel_1.wav') + ' -O ' + soundtrack_onsets_method + ' -t ' + soundtrack_onsets_threshold_string + ' -M ' + soundtrack_min_inter_onset_interval_string + ' > ' + os.path.join(soundtrack_directory, soundtrack_onsets_file0))
				os.system('aubioonset -i ' + os.path.join(soundtrack_directory, 'tmp_channel_2.wav') + ' -O ' + soundtrack_onsets_method + ' -t ' + soundtrack_onsets_threshold_string + ' -M ' + soundtrack_min_inter_onset_interval_string + ' > ' + os.path.join(soundtrack_directory, soundtrack_onsets_file1))
				os.system('rm -f ' + os.path.join(soundtrack_directory, 'tmp_channel_1.wav'))
				os.system('rm -f ' + os.path.join(soundtrack_directory, 'tmp_channel_2.wav'))

			collect_smoothed_envelope_at_onsets(soundtrack_directory, soundtrack_file, soundtrack_working_channels, soundtrack_smoothed_envelope_file, soundtrack_onsets_file0, soundtrack_onsets_file1, soundtrack_onset_envelope_file)

			################################################
			# RETURNED VALUES
			if "pulse_coef" in local_val_init :
				returned_pulse_coef = to_num(local_val_init["pulse_coef"])
			else :
				returned_pulse_coef = 0.0
			if "smoothed_pulse_coef" in local_val_init :
				returned_smoothed_pulse_coef = to_num(local_val_init["smoothed_pulse_coef"])
			else :
				returned_smoothed_pulse_coef = 0.0

			################################################
			# INITIALIZED VALUES
			# READS THE SOUND FILE DATA FIRST VALUE
			Timestamp_current_index = 0
			current_Timestamp = to_num(Timestamp[Timestamp_current_index])
			Envelope_Timestamp_current_index = 0
			Envelope_current_Timestamp = to_num(Envelope_timestamp[Envelope_Timestamp_current_index])
			Smoothed_Envelope_Timestamp_current_index = 0
			Smoothed_Envelope_current_Timestamp = to_num(Smoothed_Envelope_timestamp[Smoothed_Envelope_Timestamp_current_index])

			returned_with_sound_data = True
	return [returned_fps, returned_pulse_coef, returned_smoothed_pulse_coef, returned_with_sound_data]

##################################################################
# PLOTS WAV, WAV_ENVELOPE_XX, AND WAV_ENVELOPE_XX_SMOOTHED_YYY)
def plot_sound_data(local_val_init) :
	global Signal0
	global Envelope_Signal0
	global Peaked_Envelope_Signal0
	global Smoothed_Envelope_Signal0
	global Signal1
	global Envelope_Signal1
	global Peaked_Envelope_Signal1
	global Smoothed_Envelope_Signal1

	global Timestamp
	global Envelope_timestamp
	global Smoothed_Envelope_timestamp

	# if it is given, opens the file with the audio data for pulsed colors
	local_FILEsoundtrackin = None
	local_readsoundtrackCSV = None
	if "soundtrack_directory" in local_val_init and "soundtrack_data_file" in local_val_init and "soundsample_window" in local_val_init and "soundsmoothing_window" in local_val_init and "fps" in local_val_init:
		soundtrack_directory = local_val_init["soundtrack_directory"]
		soundtrack_data_file = local_val_init["soundtrack_data_file"]
		returned_fps = local_val_init["fps"]
		if soundtrack_data_file != "None" :
			print("opening sound file", os.path.join(soundtrack_directory, soundtrack_data_file))
			soundsample_window = int(to_num(local_val_init["soundsample_window"]))
			soundtrack_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '.csv', soundtrack_data_file)
			soundsmoothing_window = int(to_num(local_val_init["soundsmoothing_window"]))
			soundtrack_smoothed_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '_smoothed_' + str(soundsmoothing_window) + '.csv', soundtrack_data_file)

			################################################
			# READS SIGNAL VALUES EXTRACTED FROM WAV FILE
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_data_file), "rt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_data_file), " or path is incorrect")

			# CSV READER
			local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
			# READS THE SOUND FILE DATA LINE BY LINE
			# Pass reader object to list() to get a list of lists [timestamp, value]
			basic_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
			local_FILEsoundtrackin.close()
			# separates list of timestamps and list of values
			timestamp, signal = map(list, zip(*basic_Timestamp_Signal_sound_data))
			print("plotting sound file")
			plt.plot(range(1000), signal[1000:2000])
			plt.ylabel('sound')
			# plt.show()
			plt.savefig("sound.png")  #savefig, don't show

			################################################
			# READS SIGNAL ENVELOPE VALUES
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEenvelope_soundtrackin = open(os.path.join(soundtrack_directory, soundtrack_envelope_file), "rt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_envelope_file), " or path is incorrect")
			local_readsoundtrackCSV = csv.reader(local_FILEenvelope_soundtrackin, delimiter=',')
			# READS THE SOUND FILE DATA LINE BY LINE
			# Pass reader object to list() to get a list of lists [timestamp, value]
			envelope_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
			local_FILEenvelope_soundtrackin.close()
			# separates list of timestamps and list of values
			envelope_timestamp, envelope_signal = map(list, zip(*envelope_Timestamp_Signal_sound_data)) 
			print("plotting sound envelope file")
			plt.plot(range(1000), envelope_signal[1000:2000])
			plt.ylabel('sound envelope')
			# plt.show()
			plt.savefig("sound_envelope.png")  #savefig, don't show

			################################################
			# READS SIGNAL SMOOTHED ENVELOPE VALUES
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			try:
				local_FILEsmoothed_envelope_soundtrackin = open(os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), "rt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), " or path is incorrect")
			local_readsoundtrackCSV = csv.reader(local_FILEsmoothed_envelope_soundtrackin, delimiter=',')
			# READS THE SOUND FILE DATA LINE BY LINE
			# Pass reader object to list() to get a list of lists [timestamp, value]
			smoothed_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
			local_FILEsmoothed_envelope_soundtrackin.close()
			# separates list of timestamps and list of values
			smoothed_envelope_timestamp, smoothed_envelope_signal = map(list, zip(*smoothed_Timestamp_Signal_sound_data)) 
			print("plotting smoothed sound envelope file")
			plt.plot(range(1000), smoothed_envelope_signal[1000:2000])
			plt.ylabel('smoothed sound envelope')
			# plt.show()
			plt.savefig("smoothed_sound_envelope.png")  #savefig, don't show
	return

##################################################################
# SOUND DATA LOADING
def read_sound_data(local_val_init) :
	global Signal0
	global Envelope_Signal0
	global Peaked_Envelope_Signal0
	global Smoothed_Envelope_Signal0
	global Signal1
	global Envelope_Signal1
	global Peaked_Envelope_Signal1
	global Smoothed_Envelope_Signal1

	global Timestamp_current_index
	global Envelope_Timestamp_current_index
	global Peaked_Envelope_Timestamp_current_index
	global Smoothed_Envelope_Timestamp_current_index

	global Timestamp
	global Envelope_timestamp
	global Peaked_Envelope_timestamp
	global Smoothed_Envelope_timestamp

	global current_Timestamp
	global Envelope_current_Timestamp
	global Smoothed_Envelope_current_Timestamp

	global looped_soundtrack
	global soundtrack_duration

	global stored_soundtrack_directory
	global stored_soundtrack_envelope_file
	global stored_soundtrack_peaked_envelope_file
	global stored_soundtrack_smoothed_envelope_file

	# if it is given, opens the file with the audio data for pulsed colors
	returned_with_sound_data = False
	looped_soundtrack = False

	local_FILEsoundtrackin = None
	local_readsoundtrackCSV = None
	soundtrack_working_channels = 1
	if "soundtrack_directory" in local_val_init and "soundtrack_data_file" in local_val_init and "soundsample_window" in local_val_init and "soundsmoothing_window" in local_val_init and "fps" in local_val_init:
		soundtrack_directory = local_val_init["soundtrack_directory"]
		soundtrack_data_file = local_val_init["soundtrack_data_file"]
		returned_fps = local_val_init["fps"]
		if "soundtrack_working_channels" in local_val_init :
			soundtrack_working_channels = int(to_num(local_val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		if soundtrack_data_file != "None" :
			print("opening sound file", os.path.join(soundtrack_directory, soundtrack_data_file))
			soundsample_window = int(to_num(local_val_init["soundsample_window"]))
			soundtrack_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '.csv', soundtrack_data_file)
			soundsmoothing_window = int(to_num(local_val_init["soundsmoothing_window"]))
			soundtrack_smoothed_envelope_file = re.sub('.csv$', '_envelope_' + str(soundsample_window) + '_smoothed_' + str(soundsmoothing_window) + '.csv', soundtrack_data_file)
			soundtrack_peaked_envelope_file = re.sub('.csv$', '_peaked_envelope_' + str(soundsample_window) + '.csv', soundtrack_data_file)

			################################################
			# READS SIGNAL VALUES EXTRACTED FROM WAV FILE
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			# not useful because only envelope and smoothed envelope are used to calculate sound volume
			if False :
				try:
					local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_data_file), "rt")
				except IOError:
					print("File not found :", os.path.join(soundtrack_directory, soundtrack_data_file), " or path is incorrect")

				# CSV READER
				local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
				# READS THE SOUND FILE DATA LINE BY LINE
				# Pass reader object to list() to get a list of lists [timestamp, value]
				basic_Timestamp_Signal_sound_data = list(local_readsoundtrackCSV)
				local_FILEsoundtrackin.close()
				# separates list of timestamps and list of values and jumps over header
				for timestamp_signal in basic_Timestamp_Signal_sound_data[3:] :
					Timestamp.append(timestamp_signal[0])
					Signal0.append(timestamp_signal[1])
					if soundtrack_working_channels == 2 :
						Signal1.append(timestamp_signal[2])

				# READS THE SOUND FILE DATA FIRST VALUE
				Timestamp_current_index = 0
				current_Timestamp = to_num(Timestamp[Timestamp_current_index])
				soundtrack_duration = force_num(basic_Timestamp_Signal_sound_data[2][1])

			################################################
			# READS ENVELOPE VALUES CALCULATED FROM SIGNAL
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			if soundtrack_directory != stored_soundtrack_directory or soundtrack_envelope_file != stored_soundtrack_envelope_file :
				print("READS ENVELOPE VALUES CALCULATED FROM SIGNAL", soundtrack_envelope_file)
				stored_soundtrack_directory = soundtrack_directory
				stored_soundtrack_envelope_file = soundtrack_envelope_file
				try:
					local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_envelope_file), "rt")
				except IOError:
					print("File not found (soundtrack_envelope_file):", os.path.join(soundtrack_directory, soundtrack_envelope_file), " or path is incorrect")

				# CSV READER
				local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
				# READS THE SOUND FILE DATA LINE BY LINE
				# Pass reader object to list() to get a list of lists [timestamp, value]
				tmp_Envelope_timestamp = list(local_readsoundtrackCSV)
				local_FILEsoundtrackin.close()
				# separates list of timestamps and list of values and jumps over header
				for timestamp_envelope in tmp_Envelope_timestamp[3:] :
					Envelope_timestamp.append(timestamp_envelope[0])
					Envelope_Signal0.append(timestamp_envelope[1])
					if soundtrack_working_channels == 2 :
						Envelope_Signal1.append(timestamp_envelope[2])

				# READS THE SOUND FILE DATA FIRST VALUE
				Envelope_Timestamp_current_index = 0
				Envelope_current_Timestamp = to_num(Envelope_timestamp[Envelope_Timestamp_current_index])
				soundtrack_duration = force_num(tmp_Envelope_timestamp[2][1])

			################################################
			# READS PEAKED ENVELOPE VALUES CALCULATED FROM SIGNAL
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			if soundtrack_directory != stored_soundtrack_directory or stored_soundtrack_peaked_envelope_file != soundtrack_peaked_envelope_file:
				print("READS PEAKED ENVELOPE VALUES CALCULATED FROM SIGNAL", soundtrack_peaked_envelope_file)
				stored_soundtrack_directory = soundtrack_directory
				stored_soundtrack_peaked_envelope_file = soundtrack_peaked_envelope_file
				try:
					local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_peaked_envelope_file), "rt")
				except IOError:
					print("File not found (soundtrack_peaked_envelope_file):", os.path.join(soundtrack_directory, soundtrack_peaked_envelope_file), " or path is incorrect")

				# CSV READER
				local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
				# READS THE SOUND FILE DATA LINE BY LINE
				# Pass reader object to list() to get a list of lists [timestamp, value]
				tmp_Peaked_Envelope_timestamp = list(local_readsoundtrackCSV)
				local_FILEsoundtrackin.close()
				# separates list of timestamps and list of values and jumps over header
				for timestamp_envelope in tmp_Peaked_Envelope_timestamp[3:] :
					Peaked_Envelope_timestamp.append(timestamp_envelope[0])
					Peaked_Envelope_Signal0.append(timestamp_envelope[1])
					if soundtrack_working_channels == 2 :
						Peaked_Envelope_Signal1.append(timestamp_envelope[2])


				# READS THE SOUND FILE DATA FIRST VALUE
				Peaked_Envelope_Timestamp_current_index = 0
				Peaked_Envelope_current_Timestamp = to_num(Peaked_Envelope_timestamp[Peaked_Envelope_Timestamp_current_index])

			################################################
			# READS SMOOTHED ENVELOPE VALUES CALCULATED FROM ENVELOPE VALUES
			# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
			if soundtrack_directory != stored_soundtrack_directory or soundtrack_smoothed_envelope_file != stored_soundtrack_smoothed_envelope_file :
				print("READS SMOOTHED ENVELOPE VALUES CALCULATED FROM ENVELOPE VALUES", soundtrack_smoothed_envelope_file)
				stored_soundtrack_directory = soundtrack_directory
				stored_soundtrack_smoothed_envelope_file = soundtrack_smoothed_envelope_file
				try:
					local_FILEsoundtrackin = open(os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), "rt")
				except IOError:
					print("File not found (soundtrack_smoothed_envelope_file):", os.path.join(soundtrack_directory, soundtrack_smoothed_envelope_file), " or path is incorrect")

				# CSV READER
				local_readsoundtrackCSV = csv.reader(local_FILEsoundtrackin, delimiter=',')
				# READS THE SOUND FILE DATA LINE BY LINE
				# Pass reader object to list() to get a list of lists [timestamp, value]
				tmp_Smoothed_Envelope_timestamp = list(local_readsoundtrackCSV)
				local_FILEsoundtrackin.close()
				# separates list of timestamps and list of values
				for timestamp_smoothed_envelope in tmp_Smoothed_Envelope_timestamp[3:] :
					Smoothed_Envelope_timestamp.append(timestamp_smoothed_envelope[0])
					Smoothed_Envelope_Signal0.append(timestamp_smoothed_envelope[1])
					if soundtrack_working_channels == 2 :
						Smoothed_Envelope_Signal1.append(timestamp_smoothed_envelope[2])
				soundtrack_duration = force_num(tmp_Smoothed_Envelope_timestamp[2][1])

				# READS THE SOUND FILE DATA FIRST VALUE
				Smoothed_Envelope_Timestamp_current_index = 0
				Smoothed_Envelope_current_Timestamp = to_num(Smoothed_Envelope_timestamp[Smoothed_Envelope_Timestamp_current_index])

			print("All sound data stored, soundtrack_duration:",soundtrack_duration)

			returned_with_sound_data = True
			if "looped_soundtrack" in local_val_init :
				if local_val_init["looped_soundtrack"] == "True" :
					looped_soundtrack = True

	return returned_with_sound_data

##################################################################
# BEAT DATA LOADING
def read_onset_data(local_val_init) :
	global Onset0_timestamp
	global Onset1_timestamp
	global Onset0_Timestamp_current_index
	global Onset0_current_Timestamp
	global Onset0_previous_Timestamp
	global Onset1_Timestamp_current_index
	global Onset1_current_Timestamp
	global Onset1_previous_Timestamp

	global stored_soundtrack_directory
	global stored_onsets_data_file

	# if it is given, opens the file with the audio data for pulsed colors
	returned_fps = 30
	returned_onset_pulse_coef = 1.0
	returned_with_onset_data = False
	local_FILEonset_track_in = None
	local_read_onset_trackCSV = None
	soundtrack_working_channels = 1
	if "soundtrack_directory" in local_val_init and "onsets_data_file" in local_val_init and "onset_decay_time" in local_val_init :
		soundtrack_directory = local_val_init["soundtrack_directory"]
		onsets_data_file = local_val_init["onsets_data_file"]
		if "soundtrack_working_channels" in local_val_init :
			soundtrack_working_channels = int(to_num(local_val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)
		if onsets_data_file != "None" :
			################################################
			# READS ENVELOPE VALUES CALCULATED FROM SIGNAL
			if soundtrack_directory != stored_soundtrack_directory or onsets_data_file != stored_onsets_data_file :
				stored_soundtrack_directory = soundtrack_directory
				stored_onsets_data_file = onsets_data_file

				if soundtrack_working_channels == 1 :
					# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
					print("opening onset file", os.path.join(stored_soundtrack_directory, stored_onsets_data_file))
					try:
						local_FILEonset_track_in = open(os.path.join(stored_soundtrack_directory, stored_onsets_data_file), "rt")
					except IOError:
						print("File not found (onsets_data_file):", os.path.join(stored_soundtrack_directory, stored_onsets_data_file), " or path is incorrect")

					# CSV READER
					local_read_onset_trackCSV = csv.reader(local_FILEonset_track_in, delimiter=',')
					# READS THE SOUND FILE DATA LINE BY LINE
					# Pass reader object to list() to get a list of lists [timestamp, value]
					tmp_Timestamp_onsets_sound_data = list(local_read_onset_trackCSV)
					local_FILEonset_track_in.close()
					# separates list of timestamps and list of values
					for timestamp_onset in tmp_Timestamp_onsets_sound_data :
						Onset0_timestamp.append(timestamp_onset[0])
				else :
					stored_onsets_data_file0 = stored_onsets_data_file.replace("_onsets_", "_onsets0_", 1)
					stored_onsets_data_file1 = stored_onsets_data_file.replace("_onsets_", "_onsets1_", 1)
					# opens the file and reads in the CSV mode (2 columns [timestamp, value], one sample/line)
					print("opening onset file", os.path.join(stored_soundtrack_directory, stored_onsets_data_file0))
					try:
						local_FILEonset_track_in0 = open(os.path.join(stored_soundtrack_directory, stored_onsets_data_file0), "rt")
					except IOError:
						print("File not found (onsets_data_file #0):", os.path.join(stored_soundtrack_directory, stored_onsets_data_file0), " or path is incorrect")
					print("opening onset file", os.path.join(stored_soundtrack_directory, stored_onsets_data_file1))
					try:
						local_FILEonset_track_in1 = open(os.path.join(stored_soundtrack_directory, stored_onsets_data_file1), "rt")
					except IOError:
						print("File not found (onsets_data_file #1):", os.path.join(stored_soundtrack_directory, stored_onsets_data_file1), " or path is incorrect")

					# CSV READER
					# FIRST CHANNEL
					local_read_onset_trackCSV = csv.reader(local_FILEonset_track_in0, delimiter=',')
					# READS THE SOUND FILE DATA LINE BY LINE
					# Pass reader object to list() to get a list of lists [timestamp, value]
					tmp_Timestamp_onsets_sound_data = list(local_read_onset_trackCSV)
					local_FILEonset_track_in0.close()
					# separates list of timestamps and list of values
					for timestamp_onset in tmp_Timestamp_onsets_sound_data :
						Onset0_timestamp.append(timestamp_onset[0])
					# SECOND CHANNEL
					local_read_onset_trackCSV = csv.reader(local_FILEonset_track_in1, delimiter=',')
					# READS THE SOUND FILE DATA LINE BY LINE
					# Pass reader object to list() to get a list of lists [timestamp, value]
					tmp_Timestamp_onsets_sound_data = list(local_read_onset_trackCSV)
					local_FILEonset_track_in1.close()
					# separates list of timestamps and list of values
					for timestamp_onset in tmp_Timestamp_onsets_sound_data :
						Onset1_timestamp.append(timestamp_onset[0])

				# READS THE SOUND FILE DATA FIRST VALUE
				Onset0_Timestamp_current_index = 0
				Onset0_previous_Timestamp = Onset0_current_Timestamp
				Onset0_current_Timestamp = to_num(Onset0_timestamp[Onset0_Timestamp_current_index])
				if soundtrack_working_channels == 2 :
					Onset1_Timestamp_current_index = 0
					Onset1_previous_Timestamp = Onset1_current_Timestamp
					Onset1_current_Timestamp = to_num(Onset1_timestamp[Onset1_Timestamp_current_index])

			returned_with_onset_data = True
	return returned_with_onset_data

##################################################################
# SOUND DATA CURRENT VALUE
def current_sound_volume(local_indFrameInput, local_fps, local_pulse_coef, soundtrack_working_channels) :
	global Envelope_timestamp

	global Envelope_Signal0
	global Envelope_Signal1
	global Envelope_Timestamp_current_index
	global Envelope_current_Timestamp

	global looped_soundtrack
	global soundtrack_duration

	# calculates the sound data value for the current time stamp by averaging on the n preceding samples
	image_time_stamp = float(to_num(local_indFrameInput)) / float(local_fps)
	sound_time_stamp = image_time_stamp
	if looped_soundtrack :
		sound_time_stamp = sound_time_stamp - floor(sound_time_stamp/soundtrack_duration) * soundtrack_duration
	elif sound_time_stamp > soundtrack_duration :
		return 0.

	if Envelope_current_Timestamp >= sound_time_stamp :
		Envelope_Timestamp_current_index = 0
		Envelope_current_Timestamp = to_num(Smoothed_Envelope_timestamp[Envelope_Timestamp_current_index])

	sound_envelope0 = to_num(Envelope_Signal0[Envelope_Timestamp_current_index])
	if soundtrack_working_channels == 2 :
		sound_envelope1 = to_num(Envelope_Signal1[Envelope_Timestamp_current_index])
	while Envelope_current_Timestamp < sound_time_stamp  and Envelope_Timestamp_current_index < len(Envelope_timestamp) :
		Envelope_current_Timestamp = to_num(Envelope_timestamp[Envelope_Timestamp_current_index])
		if soundtrack_working_channels == 1 :
			sound_envelope = to_num(Envelope_Signal0[Envelope_Timestamp_current_index])
		else :
			sound_envelope = to_num(0.5 * (Envelope_Signal0[Envelope_Timestamp_current_index] + Envelope_Signal1[Envelope_Timestamp_current_index]))
		Envelope_Timestamp_current_index += 1

	sound_volume = max(0, min(1, sound_envelope * local_pulse_coef))
	return sound_volume

def peaked_current_sound_volume(local_indFrameInput, local_fps, local_pulse_coef, soundtrack_working_channels) :
	global Peaked_Envelope_timestamp

	global Peaked_Envelope_Signal0
	global Peaked_Envelope_Signal1
	global Peaked_Envelope_Timestamp_current_index
	global Peaked_Envelope_current_Timestamp

	global looped_soundtrack
	global soundtrack_duration

	# calculates the sound data value for the current time stamp by averaging on the n preceding samples
	image_time_stamp = float(to_num(local_indFrameInput)) / float(local_fps)
	sound_time_stamp = image_time_stamp
	if looped_soundtrack :
		sound_time_stamp = sound_time_stamp - floor(sound_time_stamp/soundtrack_duration) * soundtrack_duration
	elif sound_time_stamp > soundtrack_duration :
		return 0.

	if Peaked_Envelope_current_Timestamp >= sound_time_stamp :
		Peaked_Envelope_Timestamp_current_index = 0
		Peaked_Envelope_current_Timestamp = to_num(Peaked_Envelope_timestamp[Peaked_Envelope_Timestamp_current_index])

	if soundtrack_working_channels == 1 :
		sound_envelope = to_num(Peaked_Envelope_Signal0[Peaked_Envelope_Timestamp_current_index])
	else :
		sound_envelope = to_num(0.5 * (Peaked_Envelope_Signal0[Peaked_Envelope_Timestamp_current_index] + Peaked_Envelope_Signal1[Peaked_Envelope_Timestamp_current_index]))
	first_image_after_peak = False
	first_image_after_valley = False
	while Peaked_Envelope_current_Timestamp < sound_time_stamp  and Envelope_Timestamp_current_index < len(Peaked_Envelope_timestamp) :
		Peaked_Envelope_current_Timestamp = to_num(Peaked_Envelope_timestamp[Peaked_Envelope_Timestamp_current_index])
		if soundtrack_working_channels == 1 :
			sound_envelope = to_num(Peaked_Envelope_Signal0[Peaked_Envelope_Timestamp_current_index])
		else :
			sound_envelope = to_num(0.5 * (Peaked_Envelope_Signal0[Peaked_Envelope_Timestamp_current_index] + Peaked_Envelope_Signal1[Peaked_Envelope_Timestamp_current_index]))
		# if the value 0 or 1 is encountered between preceding sound_envelope and current one, assign 0 or 1 to the current sound volume value
		if sound_envelope >= .999 :
			first_image_after_peak = True
		elif sound_envelope <= .001 :
			first_image_after_valley = True
		Peaked_Envelope_Timestamp_current_index += 1

	if first_image_after_peak :
		sound_envelope = 1.
	elif first_image_after_valley :
		sound_envelope = 0.
	sound_volume = max(0, min(1, sound_envelope * local_pulse_coef))
	return sound_volume

def smoothed_current_sound_volume(local_indFrameInput, local_fps, local_smoothed_pulse_coef, soundtrack_working_channels) :
	global Smoothed_Envelope_timestamp

	global Smoothed_Envelope_Signal0
	global Smoothed_Envelope_Signal1
	global Smoothed_Envelope_Timestamp_current_index
	global Smoothed_Envelope_current_Timestamp

	global looped_soundtrack
	global soundtrack_duration

	# calculates the sound data value for the current time stamp by averaging on the n preceding samples
	image_time_stamp = float(to_num(local_indFrameInput)) / float(local_fps)
	sound_time_stamp = image_time_stamp
	if looped_soundtrack :
		sound_time_stamp = sound_time_stamp - floor(sound_time_stamp/soundtrack_duration) * soundtrack_duration
	elif sound_time_stamp > soundtrack_duration :
		return 0.

	if Smoothed_Envelope_current_Timestamp >= sound_time_stamp :
		Smoothed_Envelope_Timestamp_current_index = 0
		Smoothed_Envelope_current_Timestamp = to_num(Smoothed_Envelope_timestamp[Smoothed_Envelope_Timestamp_current_index])

	if soundtrack_working_channels == 1 :
		sound_envelope = to_num(Smoothed_Envelope_Signal0[Smoothed_Envelope_Timestamp_current_index])
	else :
		sound_envelope = to_num(0.5 * (Smoothed_Envelope_Signal0[Smoothed_Envelope_Timestamp_current_index] + Smoothed_Envelope_Signal1[Smoothed_Envelope_Timestamp_current_index]))
	while Smoothed_Envelope_current_Timestamp < sound_time_stamp  \
			and Smoothed_Envelope_Timestamp_current_index < len(Smoothed_Envelope_timestamp) :
		Smoothed_Envelope_current_Timestamp = to_num(Smoothed_Envelope_timestamp[Smoothed_Envelope_Timestamp_current_index])
		if soundtrack_working_channels == 1 :
			sound_envelope = to_num(Smoothed_Envelope_Signal0[Smoothed_Envelope_Timestamp_current_index])
		else :
			sound_envelope = to_num(0.5 * (Smoothed_Envelope_Signal0[Smoothed_Envelope_Timestamp_current_index] + Smoothed_Envelope_Signal1[Smoothed_Envelope_Timestamp_current_index]))
		Smoothed_Envelope_Timestamp_current_index += 1

	smoothed_sound_volume = max(0, min(1, sound_envelope * local_smoothed_pulse_coef))
	return smoothed_sound_volume

def sigmoid(z, local_sampling_interval):
	# varies from 1 to 0 for z in [0..1]
	# increase the sampling interval (currently 8) for a steeper curve
	return 1/(1 + np.exp((local_sampling_interval/2) - (1-z) * local_sampling_interval))

def current_onset_volume(local_indFrameInput, local_fps, local_onset_pulse_coef, local_onset_decay_time, soundtrack_working_channels) :
	global Onset0_timestamp
	global Onset0_current_Timestamp
	global Onset0_previous_Timestamp
	global Onset0_Timestamp_current_index
	global Onset1_timestamp
	global Onset1_current_Timestamp
	global Onset1_previous_Timestamp
	global Onset1_Timestamp_current_index

	global looped_soundtrack
	global soundtrack_duration

	# print("current_onset_volume", local_indFrameInput, local_fps, local_onset_pulse_coef, local_onset_decay_time) 

	# calculates the sound data value for the current time stamp by averaging on the n preceding samples
	image_time_stamp = float(to_num(local_indFrameInput)) / float(local_fps)
	sound_time_stamp = image_time_stamp
	if looped_soundtrack :
		sound_time_stamp = sound_time_stamp - floor(sound_time_stamp/soundtrack_duration) * soundtrack_duration
	elif sound_time_stamp > soundtrack_duration :
		print("Image and sound time stamp exceeds soundtrack duration", sound_time_stamp, soundtrack_duration)
		exit()
		return 0.

	# print("image_time_stamp, sound_time_stamp, Onset0_current_Timestamp", image_time_stamp, sound_time_stamp, Onset0_current_Timestamp)

	# if we have looped on the sound file, the onset is the first one in the track 
	# or if the onset index has not been updated recently
	if sound_time_stamp < Onset0_previous_Timestamp :
		Onset0_Timestamp_current_index = 0
		Onset0_current_Timestamp = to_num(Onset0_timestamp[Onset0_Timestamp_current_index])
		Onset0_previous_Timestamp = Onset0_current_Timestamp
		# print("new Onset0_current_Timestamp in case of loop", Onset0_current_Timestamp)
	if soundtrack_working_channels == 2 :
		if sound_time_stamp < Onset1_previous_Timestamp :
			Onset1_Timestamp_current_index = 0
			Onset1_current_Timestamp = to_num(Onset1_timestamp[Onset1_Timestamp_current_index])
			Onset1_previous_Timestamp = Onset1_current_Timestamp
			# print("new Onset1_current_Timestamp in case of loop", Onset1_current_Timestamp)

	# if we have looped on the sound file, the onset is the first one in the track 
	new_onset = False
	while Onset0_current_Timestamp < sound_time_stamp  and Onset0_Timestamp_current_index < len(Onset0_timestamp) :
		Onset0_previous_Timestamp = Onset0_current_Timestamp
		Onset0_current_Timestamp = to_num(Onset0_timestamp[Onset0_Timestamp_current_index])
		new_onset = True
		Onset0_Timestamp_current_index += 1
	if soundtrack_working_channels == 2 :
		while Onset1_current_Timestamp < sound_time_stamp  and Onset1_Timestamp_current_index < len(Onset1_timestamp) :
			Onset1_previous_Timestamp = Onset1_current_Timestamp
			Onset1_current_Timestamp = to_num(Onset1_timestamp[Onset1_Timestamp_current_index])
			new_onset = True
			Onset1_Timestamp_current_index += 1

	# print("previous / new Onset, new_onset (bool)", Onset0_previous_Timestamp, Onset0_current_Timestamp, new_onset)

	if soundtrack_working_channels == 1 :
		time_since_last_onset = sound_time_stamp - Onset0_previous_Timestamp
		if time_since_last_onset < local_onset_decay_time :
			sound_volume = max(0, min(1, sigmoid(time_since_last_onset / local_onset_decay_time, 8) * local_onset_pulse_coef))
		else :
			sound_volume = 0.
	else :
		sound_volume = 0.
		time_since_last_onset = sound_time_stamp - Onset0_previous_Timestamp
		if time_since_last_onset < local_onset_decay_time :
			sound_volume += max(0, min(1, sigmoid(time_since_last_onset / local_onset_decay_time, 8) * local_onset_pulse_coef))
		time_since_last_onset = sound_time_stamp - Onset1_previous_Timestamp
		if time_since_last_onset < local_onset_decay_time :
			sound_volume += max(0, min(1, sigmoid(time_since_last_onset / local_onset_decay_time, 8) * local_onset_pulse_coef))
		sound_volume *= 0.5

	# print("time_since_last_onset, sound_volume, new_onset", time_since_last_onset, sound_volume, new_onset)
	return [sound_volume, new_onset]


##################################################################
# FILE NAME WITH REGULAR EXPRESSION FOR RANDOM CHOICE
##################################################################
def random_target_regular_expression_name(local_target_image_directory, local_target_image_file_name) :
	files_in_dir = [f for f in os.listdir(local_target_image_directory) if os.path.isfile(os.path.join(local_target_image_directory, f))]
	matching_files_in_dir = [f for f in files_in_dir if re.match(local_target_image_file_name+"$", f)]
	# no file match, takes any in the dir
	if len(matching_files_in_dir) > 0 :
		selected_file_index = int(random.random() * len(matching_files_in_dir))
		print("   ------> selected target: {0:s}".format(matching_files_in_dir[selected_file_index]))
		return matching_files_in_dir[selected_file_index]
	# matching file found, takes a rendom one amont the matching ones
	else :
		print("*** warning: no file matches the regexp file name, random file chosen in target directory", os.jon((local_target_image_directory, local_target_image_file_name)))
		selected_file_index = int(random.random() * len(files_in_dir))
		return files_in_dir[selected_file_index]

##################################################################
# COLOR MIXTURE
##################################################################
def bilinear_interpolation(x, y, z00, z01, z10, z11) :
	
	def linear_interpolation(x, z0, z1) :
		return z0 * (1 - x) + z1 * x

	return linear_interpolation(y, linear_interpolation(x, z00, z01),
								   linear_interpolation(x, z10, z11))

def bilinear_interpolation_pixel(x_float, y_float, array_dim2, array_h_w) :
	h, w = array_h_w
	y1 = min(h-1, int(math.floor(y_float)))
	y2 = min(h-1, y1 + 1)
	x1 = min(w-1, int(math.floor(x_float)))
	x2 = min(w-1, x1 + 1)
	z00 = array_dim2[y1,x1]
	z10 = array_dim2[y2,x1]
	z01 = array_dim2[y1,x2]
	z11 = array_dim2[y2,x2]
	return bilinear_interpolation(y_float - y1, x_float - x1, z01, z11, z00, z10)

##################################################################
# COLOR MIXTURE
##################################################################
def saturate_color(local_initial_color, local_percentage) :
	rgb = re.sub('^\#', '', local_initial_color)
	h, s, v = colorsys.rgb_to_hsv(int('0x' + rgb[ 0 : 2 ], 0)/255.0, int('0x' + rgb[ 2 : 4 ], 0)/255.0, int('0x' + rgb[ 4 : 6 ], 0)/255.0)
	s = s * (1 - local_percentage) + local_percentage
	r, g, b = colorsys.hsv_to_rgb(h, s, v)
	sat_red = "%02x" % int(r * 255.0)
	sat_green = "%02x" % int(g * 255.0)
	sat_blue = "%02x" % int(b * 255.0)
	saturated_color = '#' + sat_red + sat_green + sat_blue
	# print "target_color initial_color local_percentage: target_color, initial_color, local_percentage / sated_color: sated_color \n"
	return saturated_color

def color_hsv_balance(local_initial_color, local_hue_percentage, local_saturation_percentage, local_brightness_percentage) :
	# h 1 = red(1) / -1 = red(0) / 0 unchanged
	# s 1 = saturated(1) / -1 = grayscale(0) / 0 unchanged
	# v 1 = white(1) / -1 = black(0) / 0 unchanged
	rgb = re.sub('^\#', '', local_initial_color)
	h, s, v = colorsys.rgb_to_hsv(int('0x' + rgb[ 0 : 2 ], 0)/255.0, int('0x' + rgb[ 2 : 4 ], 0)/255.0, int('0x' + rgb[ 4 : 6 ], 0)/255.0)
	if(local_hue_percentage >= 0) :
		h = h * (1 - local_hue_percentage) + local_hue_percentage
	else:
		h = h * (1 + local_hue_percentage)
	if(local_saturation_percentage >= 0) :
		s = s * (1 - local_saturation_percentage) + local_saturation_percentage
	else:
		s = s * (1 + local_saturation_percentage)
	if(local_brightness_percentage >= 0) :
		v = v * (1 - local_brightness_percentage) + local_brightness_percentage
	else:
		v = v * (1 + local_brightness_percentage)
	r, g, b = colorsys.hsv_to_rgb(h, s, v)
	balanced_red = "%02x" % int(r * 255.0)
	balanced_green = "%02x" % int(g * 255.0)
	balanced_blue = "%02x" % int(b * 255.0)
	balanced_color = '#' + balanced_red + balanced_green + balanced_blue
	# print "target_color initial_color local_percentage: target_color, initial_color, local_percentage / sated_color: sated_color \n"
	return balanced_color

def mix_color(local_initial_color, local_target_color, local_percentage) :
	# print("initial_color target_color local_percentage:", local_initial_color,  "/", local_target_color, "/", local_percentage)
	initial_color = re.sub('^\#', '', local_initial_color)
	target_color = re.sub('^\#', '', local_target_color)
	target_red = '0x' + target_color[ 0 : 2 ]
	target_green = '0x' + target_color[ 2 : 4 ]
	target_blue = '0x' + target_color[ 4 : 6 ]
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	mix_red = int((1 - local_percentage) *  int(initial_red, 0) + local_percentage *  int(target_red, 0))
	mix_green = int((1 - local_percentage) *  int(initial_green, 0) + local_percentage *  int(target_green, 0))
	mix_blue = int((1 - local_percentage) *  int(initial_blue, 0) + local_percentage *  int(target_blue, 0))
	mix_red = "%02x" % mix_red
	mix_green = "%02x" % mix_green
	mix_blue = "%02x" % mix_blue
	mixed_color = '#' + mix_red.upper() + mix_green.upper() + mix_blue.upper()
	return mixed_color

def color_to_gray_level(local_initial_color) :
	initial_color = re.sub('^\#', '', local_initial_color)
	initial_red = int('0x' + initial_color[ 0 : 2 ], 0)
	initial_green = int('0x' + initial_color[ 2 : 4 ], 0)
	initial_blue = int('0x' + initial_color[ 4 : 6 ], 0)
	return float(initial_red + initial_green + initial_blue)/(255.0 * 3)

def color_to_max_gray(local_initial_color) :
	initial_color = re.sub('^\#', '', local_initial_color)
	initial_red = int('0x' + initial_color[ 0 : 2 ], 0)
	initial_green = int('0x' + initial_color[ 2 : 4 ], 0)
	initial_blue = int('0x' + initial_color[ 4 : 6 ], 0)
	return float(max(max(initial_red, initial_green), initial_blue)/255.0)

##################################################################
# COLOR TEMPERATURE
##################################################################
# Color temperature (kelvin) to RGB subroutine ported from VB.NET (!) 
# code by Tanner Helland (see code header for credits).
# http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
# color space functions translated from HLSL versions on Chilli Ant (by Ian Taylor)
# http://www.chilliant.com/rgb2hsv.html
# https://www.shadertoy.com/view/lsSXW1

def red_sup_6600(temp) :
	return 1.29293618606274509804 * pow(temp/100.0, -0.1332047592)

def green_between_1000_and_6600(temp) :
	return 0.39008157876901960784 * log(temp/100.0) - 0.63184144378862745098

def green_sup_6600(temp) :
	return 1.12989086089529411765 * pow(temp/100.0, -0.0755148492)

def blue_between_2000_and_6600(temp) :
	return 0.54320678911019607843 * log(temp/100.0 - 10.0) - 1.19625408914

def rgb_gain(temp) :
	temp = min(40000,max(1000,temp))
	#red
	if(temp < 6600) :
		r = 1
	else:
		r = red_sup_6600(temp)
	# green
	if(temp <= 6600) :
		g = green_between_1000_and_6600(temp)
	elif(temp > 6600) :
		g = green_sup_6600(temp)
	# blue
	if(temp < 2000) :
		b = 0
	elif(temp >= 2000 and temp <= 6600) :
		b = blue_between_2000_and_6600(temp)
	else:
		b = 1
	return [min(max(r,0),1),min(max(g,0),1),min(max(b,0),1)]

def temperature_balance(input_color, temperature) :
	initial_color = re.sub('^\#', '', input_color)
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	gain_red, gain_green, gain_blue = rgb_gain(to_num(temperature))
	balanced_red = min(max(int(gain_red * int(initial_red, 0)),0),255)
	balanced_green = min(max(int(gain_green * int(initial_green, 0)),0),255)
	balanced_blue = min(max(int(gain_blue * int(initial_blue, 0)),0),255)
	balanced_red = "%02x" % balanced_red
	balanced_green = "%02x" % balanced_green
	balanced_blue = "%02x" % balanced_blue
	return '#' + balanced_red + balanced_green + balanced_blue
def brightness_balance(input_color, brightness) :
	initial_color = re.sub('^\#', '', input_color)
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	gain_red, gain_green, gain_blue = rgb_gain(to_num(temperature))
	balanced_red = min(max(int(gain_red * int(initial_red, 0)),0),255)
	balanced_green = min(max(int(gain_green * int(initial_green, 0)),0),255)
	balanced_blue = min(max(int(gain_blue * int(initial_blue, 0)),0),255)
	balanced_red = "%02x" % balanced_red
	balanced_green = "%02x" % balanced_green
	balanced_blue = "%02x" % balanced_blue
	return '#' + balanced_red + balanced_green + balanced_blue
def saturation_balance(input_color, saturation) :
	initial_color = re.sub('^\#', '', input_color)
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]

	d[i] = gray * value + d[i] * (1-value);
	d[i+1] = gray * value + d[i+1] * (1-value);
	d[i+2] = gray * value + d[i+2] * (1-value);	gain_red, gain_green, gain_blue = rgb_gain(to_num(temperature))
	balanced_red = min(max(int(gain_red * int(initial_red, 0)),0),255)
	balanced_green = min(max(int(gain_green * int(initial_green, 0)),0),255)
	balanced_blue = min(max(int(gain_blue * int(initial_blue, 0)),0),255)
	balanced_red = "%02x" % balanced_red
	balanced_green = "%02x" % balanced_green
	balanced_blue = "%02x" % balanced_blue
	return '#' + balanced_red + balanced_green + balanced_blue

##################################################################
# STRING TO NUM
##################################################################
def force_num(val_init) :
	try:
		return_value = float(val_init)
	except ValueError:
		try:
			return_value = int(val_init)
		except ValueError:
			return_value = 0
	return return_value

def to_num(val_init) :
	try:
		return_value = int(val_init)
	except ValueError:
		try:
			return_value = float(val_init)
		except ValueError:
			return_value = val_init
	return return_value

def to_num_or_string(val_init) :
	try:
		return_value = int(val_init)
	except ValueError:
		try:
			return_value = float(val_init)
		except ValueError:
			return_value = '"'+val_init+'"'
	return return_value

##################################################################
# VALUES INTERPOLATION
##################################################################
def clamp(value) :
	if(value > 1) :
		value = 1
	elif(value < 0.0) :
		value = 0
	return value

def Bezier_01_01(value) :
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	else :
		return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value


def Bezier_steep_01_01(value) :
	P0 = [0,0]
	P1 = [0,1]
	P2 = [1,0]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	else :
		return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value

# double Bezier
def Gaussian_01_01(value) :
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]
	
	if(value > 1) :
		return_value = 0
	elif(value < 0.0) :
		return_value = 0
	else :
		if(value <= 0.5) :
			value = 2 * value
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = 2 * (1 - value)
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 

	return return_value

# double Bezier
def Gaussian_steep_01_01(value) :
	P0 = [0,0]
	P1 = [0,1]
	P2 = [1,0]
	P3 = [1,1]

	if(value > 1) :
		return_value = 0
	elif(value < 0.0) :
		return_value = 0
	else :
		if(value <= 0.5) :
			value = 2 * value
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = 2 * (1 - value)
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value

def Gaussian_smoothedsteep_01_01(value) :
	P0 = [0,0]
	P1 = [0,1]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value_y = 0
	elif(value < 0) :
		return_value_y = 0
	else :
		if(value <= 0.5) :
			value = 2 * value
			return_value_y = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = 2 * (1 - value)
			return_value_y = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value_y

# double Bezier
def NonCentered_Gaussian_01_01(value,curveCenter) :
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	elif(curveCenter <= 0.0) :
		return_value = Bezier_01_01(1 - value)
	elif(curveCenter >= 1.0) :
		return_value = Bezier_01_01(value)
	else :
		if(value <= curveCenter) :
			value = value/curveCenter
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = (1 - value)/(1 - curveCenter)
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		
	return return_value


# calculation of interpolation value
def interpolate(local_var_type, local_var_init, local_var_fin, local_interpolator, local_scene_percentage) :
	interpolated_value = 0

	# a string
	if(local_var_type == "string") :
		return local_var_init

	# extraction of initial and final color in case of color interpolation
	color_initial = ""
	color_final = ""
	if(local_var_type ==  "hexcolor") :
		# color calculation as an interpolation between 2 colors with a blending coef
		color_initial = local_var_init
		color_final = local_var_fin

	#interpolation calculation
	if(local_interpolator == "l") :
		local_percentage = local_scene_percentage
		
	# Bezier smooth 00 01 10 11
	elif(local_interpolator == "b") :
		local_percentage = Bezier_01_01(local_scene_percentage)
	
	# Bezier steep 00 10 01 11
	elif(local_interpolator == "t") :
		local_percentage = Bezier_steep_01_01(local_scene_percentage)
	
	# Gaussian
	elif(local_interpolator == "g") :
		local_percentage = Gaussian_01_01(local_scene_percentage)

	# Gaussian steep
	elif(local_interpolator == "h") :
		local_percentage = Gaussian_steep_01_01(local_scene_percentage)
		
	# Gaussian smoothed steep
	elif(local_interpolator == "i") :
		local_percentage = Gaussian_smoothedsteep_01_01(local_scene_percentage)
		
	# non-centered Gaussian
	elif re.search('^i', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(force_num(interpolation_values[1]))
		local_percentage = NonCentered_Gaussian_01_01(local_scene_percentage, center)
		
	# linear on a def part
	elif re.search('^L', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %+2f %+2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 1.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			
	# Bezier smooth 00 01 10 11 on a subpart
	elif re.search('^B', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 1.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Bezier_01_01(local_percentage)
		
# Bezier steep 00 10 01 11 on a subpart
	elif re.search('^T', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 1.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Bezier_steep_01_01(local_percentage)
			
	# Gaussian on a subpart
	elif re.search('^G', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 0.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Gaussian_01_01(local_percentage)
			
	# Gaussian steep on a subpart
	elif re.search('^H', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 0.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Gaussian_steep_01_01(local_percentage)
	
	# non-centered Gaussian on a subpart
	elif re.search('^I', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(force_num(interpolation_values[1]))
		perc_ini =  clamp(force_num(interpolation_values[2]))
		perc_fin =  clamp(force_num(interpolation_values[2]) + interpolation_values[3])
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			local_percentage = 0.
		elif(local_scene_percentage >= perc_fin) :
			local_percentage = 0.
		elif(perc_ini >= perc_fin) :
			local_percentage = 0.
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_center = perc_ini + center * (perc_fin - perc_ini)
			local_percentage = NonCentered_Gaussian_01_01(local_percentage, local_center)
			
	elif(local_interpolator == "s") :
			local_percentage = 0.

	# linear on a def part
	elif re.search('^S', local_interpolator) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_limit =  clamp(force_num(interpolation_values[1]))
		if(local_scene_percentage <= perc_limit) :
			local_percentage = 0.
		else :
			local_percentage = 1.

	else :
		local_percentage = 0.
	
	if(local_var_type == "float") :
		interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
	elif(local_var_type == "hexcolor") :
		# color calculation as an interpolation between 2 colors with a blending coef
		interpolated_value = mix_color(color_initial, color_final, local_percentage)
	elif(local_var_type == "int") :
		# color calculation as an interpolation between 2 colors with a blending coef
		interpolated_value = int(math.floor(local_var_init + local_percentage * (local_var_fin - local_var_init)))
	elif(local_var_type == "formula") :
		# formula interpolation by interpolating the coeficients
		# print("interpolate", local_var_init, local_var_fin, local_interpolator, local_percentage)
		interpolated_value = formula_interpolate(local_var_init, local_var_fin, local_interpolator, local_percentage)

	return interpolated_value

def formula_interpolate(local_var_init, local_var_fin, local_interpolator, local_percentage) :
	init_formula_elements = re.split(r' +' , local_var_init.rstrip())
	fin_formula_elements = re.split(r' +' , local_var_fin.rstrip())
	new_elements = []
	for (init_element, fin_element) in zip(init_formula_elements, fin_formula_elements) :
		if re.sub('^-', '', init_element.replace('.','',1)).isdigit() and re.sub('^-', '', fin_element.replace('.','',1)).isdigit() :
			if init_element.find('.') >= 0 or fin_element.find('.') >= 0 :
				interpolated_element = interpolate("float", to_num(init_element), to_num(fin_element), "l", local_percentage)
			else :
				interpolated_element = interpolate("int", to_num(init_element), to_num(fin_element), "l", local_percentage)
		else :
			interpolated_element = init_element
		new_elements.append(interpolated_element)
	separator = ' '
	return separator.join(map(str, new_elements))


##################################################################
# CSV FILE STRING CONSTRUCTION FROM VARIABLE DICIONARY
##################################################################
# dictionary
# key: variable name
# values: a list made of [type,  initial value, final value, interpolation mode]
def variable_dict_to_CSV_file(local_scenario_variables) :
	return variable_dict_to_CSV_lines(local_scenario_variables, [])

def variable_dict_append_CSV_file(local_scenario_variables, local_scenario_string_list) :
	return variable_dict_to_CSV_lines(local_scenario_variables, local_scenario_string_list)

def variable_dict_to_CSV_lines(local_scenario_variables, local_scenario_string_list) :
	nb_vars = len(local_scenario_variables)
	returned_string_list = local_scenario_string_list
	sorted_variables = sorted(local_scenario_variables)

	# empty string, creats a list of empty strings
	emptyFile = returned_string_list == []

	if(emptyFile) :
		returned_string_list = ["" for x in range(13)]
	elif(len(returned_string_list) != 13) :
		print("expeted 13 scenario lines")
		sys.exit(2)

	indLine = 0

	# ranks
	if(emptyFile) :
		line_string = 'RANK,'
	else :
		line_string = ','
	for index in range(nb_vars) :
		line_string = line_string + str(index) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1

	# verbatim
	if(emptyFile) :
		line_string = 'VERBATIM,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# types
	if(emptyFile) :
		line_string = 'TYPE,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + local_scenario_variables[var_name][0] + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# IDs
	if(emptyFile) :
		line_string = 'ID,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scenario
	if(emptyFile) :
		line_string = 'scenario,1,'
	else :
		line_string = ','
	for index in range(nb_vars - 2) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene
	if(emptyFile) :
		line_string = 'scene,transf_scene,'
	else :
		line_string = ','
	for index in range(nb_vars - 2) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene ID
	if(emptyFile) :
		line_string = 'transf_scene,'
	else :
		line_string = ','
	for index in range(nb_vars) :
		line_string = line_string + 'transf_scene,'
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# IDs
	if(emptyFile) :
		line_string = 'ID,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Initial values
	if(emptyFile) :
		line_string = 'Initial,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + str(local_scenario_variables[var_name][1]) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Final values
	if(emptyFile) :
		line_string = 'Final,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + str(local_scenario_variables[var_name][2]) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Interpolation modes
	if(emptyFile) :
		line_string = 'Interp,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + local_scenario_variables[var_name][3] + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene
	if(emptyFile) :
		line_string = '/scene,'
	else :
		line_string = ','
	for index in range(nb_vars - 1) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scenario
	if(emptyFile) :
		line_string = '/scenario,'
	else :
		line_string = ','
	for index in range(nb_vars - 1) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1

	return returned_string_list
