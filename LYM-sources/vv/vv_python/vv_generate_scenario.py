#!/usr/bin/python3 

# vv_generate_scenario.py
# extract color tables from svg files 

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import random

import getopt
import sys
from signal import signal, SIGINT
import re

from vv_lib import to_num

import csv

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_generate_scenario.py -i scenario_file -o scenario_file
	ARGUMENTS:
	-i: input scenariofile
	-o: output scenariofile
'''

IN_FPS = 50
OUT_FPS = 30

first_lines=[["RANK","0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59","60","61","62"],\
	["VERBATIM","svg_input_directory","svg_input_file_name","svg_output_directory","svg_output_file_name","tmp_directory","imageInputIndex","imageOutputIndex","nb_posterization_layers","transf1_ID","percent_transf1","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","fill_opacity_l1","fill_opacity_l2","fill_opacity_l3","fill_opacity_l4","fill_opacity_l5","fill_opacity_l6","fill_opacity_l7","fill_opacity_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","stroke_opacity_l1","stroke_opacity_l2","stroke_opacity_l3","stroke_opacity_l4","stroke_opacity_l5","stroke_opacity_l6","stroke_opacity_l7","stroke_opacity_l8","stroke_width_l1","stroke_width_l2","stroke_width_l3","stroke_width_l4","stroke_width_l5","stroke_width_l6","stroke_width_l7","stroke_width_l8","displacement_map_directory","displacement_map_file_name","displacement_map_inputIndex","blur_l1","blur_l2","blur_l3","blur_l4","blur_l5","blur_l6","blur_l7","blur_l8","temperature"],\
	["TYPE","string","string","string","string","string","int","int","int","string","float","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","float","float","float","float","float","float","float","float","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","float","float","float","float","float","float","float","float","float","float","float","float","float","float","float","float","string","string","int","float","float","float","float","float","float","float","float","float"],\
	["ID","svg_input_directory","svg_input_file_name","svg_output_directory","svg_output_file_name","tmp_directory","imageInputIndex","imageOutputIndex","nb_posterization_layers","transf1_ID","percent_transf1","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","fill_opacity_l1","fill_opacity_l2","fill_opacity_l3","fill_opacity_l4","fill_opacity_l5","fill_opacity_l6","fill_opacity_l7","fill_opacity_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","stroke_opacity_l1","stroke_opacity_l2","stroke_opacity_l3","stroke_opacity_l4","stroke_opacity_l5","stroke_opacity_l6","stroke_opacity_l7","stroke_opacity_l8","stroke_width_l1","stroke_width_l2","stroke_width_l3","stroke_width_l4","stroke_width_l5","stroke_width_l6","stroke_width_l7","stroke_width_l8","displacement_map_directory","displacement_map_file_name","displacement_map_inputIndex","blur_l1","blur_l2","blur_l3","blur_l4","blur_l5","blur_l6","blur_l7","blur_l8","temperature"]]


##################################################################
# MAIN
##################################################################
def main(main_args):
	##################################################################
	# ARGUMENTS
	##################################################################
	dir_of_svg_subdirs = ''
	csv_out_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputscenarioschema=","outputscenario="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputscenarioschema"):
			inputscenarioschema = arg
		elif opt in ("-o", "--outputscenario"):
			outputscenario = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', dir_of_svg_subdirs)

	# initial scenario schema
	try:
		FILEin = open(inputscenarioschema, "rt")
	except IOError:
		print("File not found :", inputscenarioschema, " or path is incorrect")

	# final scenario
	try:
		FILEout = open(outputscenario, "wt")
	except IOError:
		print("File not opened :", outputscenario, " or path is incorrect")

	##################################################################
	# READS THE SUBFOLDERS, EXTRACT THE MIDFILE, AND GET THE COLOR TABLE
	##################################################################
	# first lines
	writeCVS = csv.writer(FILEout, delimiter=',')
	for line_to_output in first_lines:
		# writes the line to output
		writeCVS.writerow(line_to_output)
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	# scenario line
	data_readCSV = list(readCSV)
	nb_tot_scenes = str(len(data_readCSV) - 1)
	next_line = ["scenario",nb_tot_scenes]
	writeCVS.writerow(next_line)

	# print "line1 ", line 
	FILEin.seek(0)
	line = next(readCSV) 

	# CONSTANTS
	min_temperature_ini = 40000
	min_temperature_end = 40000
	max_temperature_ini = 6600
	max_temperature_end = 1000

	min_optical_flow_ini = 0
	min_optical_flow_end = 0.3
	max_optical_flow_ini = 0.1
	max_optical_flow_end = 1
	optical_flow_markers = [1,145,289,440,591,742,893,1044,1195,1343,1490]

	# blur
	min_blur_ini = [0,0,0,0,0,0,0,0]
	min_blur_end = [0,2,4,8,16,32,48,64]
	max_blur_ini = [0,0,0,0,0,0,0,0]
	max_blur_end = [0,5,10,20,40,80,120,160]

	ratio_blur_to_stroke_width = 6
	first_of_scene = 0

	# next line
	line = next(readCSV) 
	scene_no = 1
	while(line):
		clip_name = line[1]
		if clip_name.endswith(".MP4"):
			clip_name = clip_name[:-len(".MP4")]
		scene_ID = ("%03d" % scene_no)+"_"+clip_name
		# scene line
		next_line = ["scene",scene_ID]
		writeCVS.writerow(next_line)

		# scene ID line
		writeCVS.writerow( [scene_ID] * len(first_lines[0]) )

		# ID line
		writeCVS.writerow( first_lines[3] )

		# story progress
		movie_coef = float(scene_no) / float(nb_tot_scenes)

		# optical flow image indices
		second_of_scene = first_of_scene + 1 # modulo unuseful because first_of_scene is not the last one in the list
		optical_flow_marker_ini = str(optical_flow_markers[first_of_scene])
		optical_flow_marker_end = str(optical_flow_markers[second_of_scene] - 1)

		# blur
		min_blur = []
		max_blur = []
		for b, B in zip(min_blur_ini, min_blur_end):
			min_blur.append((1-movie_coef) * b + movie_coef * B)
		for b, B in zip(max_blur_ini, max_blur_end):
			max_blur.append((1-movie_coef) * b + movie_coef * B)
		min_stroke_width = [i * ratio_blur_to_stroke_width for i in min_blur]
		max_stroke_width = [i * ratio_blur_to_stroke_width for i in max_blur]
		if(scene_no % 2 == 1) :
			blur_ini = [("%.3f" % i) for i in min_blur]
			blur_end = [("%.3f" % i) for i in max_blur]
		else :
			blur_ini = [("%.3f" % i) for i in max_blur]
			blur_end = [("%.3f" % i) for i in min_blur]
		if(scene_no % 2 == 1) :
			stroke_width_ini = [("%.3f" % i) for i in min_stroke_width]
			stroke_width_end = [("%.3f" % i) for i in max_stroke_width]
		else :
			stroke_width_ini = [("%.3f" % i) for i in max_stroke_width]
			stroke_width_end = [("%.3f" % i) for i in min_stroke_width]

		# OF
		min_optical_flow = (1-movie_coef) * min_optical_flow_ini + movie_coef * min_optical_flow_end
		max_optical_flow = (1-movie_coef) * max_optical_flow_ini + movie_coef * max_optical_flow_end
		if(scene_no % 2 == 1) :
			of_ini = ("%.3f" % min_optical_flow)
			of_end = ("%.3f" % max_optical_flow)
		else :
			of_ini = ("%.3f" % max_optical_flow)
			of_end = ("%.3f" % min_optical_flow)

		# color temperature
		min_temperature = (1-movie_coef) * min_temperature_ini + movie_coef * min_temperature_end
		max_temperature = (1-movie_coef) * max_temperature_ini + movie_coef * max_temperature_end
		if(scene_no % 2 == 1) :
			temperature_ini = ("%.3f" % min_temperature)
			temperature_end = ("%.3f" % max_temperature)
		else :
			temperature_ini = ("%.3f" % max_temperature)
			temperature_end = ("%.3f" % min_temperature)

		# initial values line
		next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_SVGs/"+clip_name+"/",clip_name,\
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/tmp/",\
		str(line[2]), str(line[4]), str(line[6]), 'DISPLACEMENT_MAP', of_ini, '#000000', \
		str(line[7]), str(line[8]), str(line[9]), str(line[10]), str(line[11]), str(line[12]), str(line[13]), str(line[14]),\
		"1", "1", "1", "1", "1", "1", "1", "1", \
		str(line[7]), str(line[8]), str(line[9]), str(line[10]), str(line[11]), str(line[12]), str(line[13]), str(line[14]),\
		"1", "1", "1", "1", "1", "1", "1", "1", \
		stroke_width_ini[0], stroke_width_ini[1], stroke_width_ini[2], stroke_width_ini[3], stroke_width_ini[4], stroke_width_ini[5], stroke_width_ini[6], stroke_width_ini[7], \
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/", "breathing_RMI_OF_smooth", \
		optical_flow_marker_ini,
		blur_ini[0], blur_ini[1], blur_ini[2], blur_ini[3], blur_ini[4], blur_ini[5], blur_ini[6], blur_ini[7], \
		temperature_ini]
		writeCVS.writerow( next_line )

		# final values line
		next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_SVGs/"+clip_name+"/",clip_name,\
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/tmp/",\
		str(line[3]), str(line[5]), str(line[6]), 'DISPLACEMENT_MAP', of_end, '#000000', \
		str(line[7]), str(line[8]), str(line[9]), str(line[10]), str(line[11]), str(line[12]), str(line[13]), str(line[14]),\
		"1", "1", "1", "1", "1", "1", "1", "1", \
		str(line[7]), str(line[8]), str(line[9]), str(line[10]), str(line[11]), str(line[12]), str(line[13]), str(line[14]),\
		"1", "1", "1", "1", "1", "1", "1", "1", \
		stroke_width_end[0], stroke_width_end[1], stroke_width_end[2], stroke_width_end[3], stroke_width_end[4], stroke_width_end[5], stroke_width_end[6], stroke_width_end[7], \
		"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/", "breathing_RMI_OF_smooth", \
		optical_flow_marker_end,
		blur_end[0], blur_end[1], blur_end[2], blur_end[3], blur_end[4], blur_end[5], blur_end[6], blur_end[7], \
		temperature_end]
		writeCVS.writerow( next_line )

		# interpolator line
		next_line = ["", "s", "s",\
		"s", "s",\
		"s",\
		"s", "s", "s", "s", "l", "l", \
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"s", "s",\
		"l",\
		"l", "l", "l", "l", "l", "l", "l", "l", \
		"l"]
		writeCVS.writerow( next_line )

		# /scene line
		next_line = ["/scene"]
		writeCVS.writerow(next_line)

		# next line
		try:
			line = next(readCSV) 
		except StopIteration:
			break

		# next scene
		scene_no += 1

		# next optical flow scene
		first_of_scene += 1
		# takes the scene modulo the size of the list
		# if it is the last one in the list, takes the first one to make 
		# sure that the next value is the next in the list
		if(first_of_scene == len(optical_flow_markers) - 1) :
			first_of_scene = 0


	# /scenario line
	next_line = ["/scenario"]
	writeCVS.writerow(next_line)

	FILEin.close()
	FILEout.close()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
