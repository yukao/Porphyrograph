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
from vv_lib import mix_color

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

WITH_CROSSFADE_COLOR_BLEND = True
WITH_CROSSFADE_COLOR_LIST = False

first_transformation_lines=[["RANK","0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59","60","61","62","63","64"],\
	["VERBATIM","svg_input_directory","svg_input_file_name","svg_output_directory","svg_output_file_name","tmp_directory","imageInputIndex","imageOutputIndex","nb_posterization_layers","transf1_ID","percent_transf1","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","fill_opacity_l1","fill_opacity_l2","fill_opacity_l3","fill_opacity_l4","fill_opacity_l5","fill_opacity_l6","fill_opacity_l7","fill_opacity_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","stroke_opacity_l1","stroke_opacity_l2","stroke_opacity_l3","stroke_opacity_l4","stroke_opacity_l5","stroke_opacity_l6","stroke_opacity_l7","stroke_opacity_l8","stroke_width_l1","stroke_width_l2","stroke_width_l3","stroke_width_l4","stroke_width_l5","stroke_width_l6","stroke_width_l7","stroke_width_l8","displacement_map_directory","displacement_map_file_name","displacement_map_inputIndex","displacement_map_factor","displacement_map_center","blur_l1","blur_l2","blur_l3","blur_l4","blur_l5","blur_l6","blur_l7","blur_l8","temperature"],\
	["TYPE","string","string","string","string","string","int","int","int","string","float","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","float","float","float","float","float","float","float","float","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","hexcolor","float","float","float","float","float","float","float","float","float","float","float","float","float","float","float","float","string","string","int","float","string","float","float","float","float","float","float","float","float","float"],\
	["ID","svg_input_directory","svg_input_file_name","svg_output_directory","svg_output_file_name","tmp_directory","imageInputIndex","imageOutputIndex","nb_posterization_layers","transf1_ID","percent_transf1","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","fill_opacity_l1","fill_opacity_l2","fill_opacity_l3","fill_opacity_l4","fill_opacity_l5","fill_opacity_l6","fill_opacity_l7","fill_opacity_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","stroke_opacity_l1","stroke_opacity_l2","stroke_opacity_l3","stroke_opacity_l4","stroke_opacity_l5","stroke_opacity_l6","stroke_opacity_l7","stroke_opacity_l8","stroke_width_l1","stroke_width_l2","stroke_width_l3","stroke_width_l4","stroke_width_l5","stroke_width_l6","stroke_width_l7","stroke_width_l8","displacement_map_directory","displacement_map_file_name","displacement_map_inputIndex","displacement_map_factor","displacement_map_center","blur_l1","blur_l2","blur_l3","blur_l4","blur_l5","blur_l6","blur_l7","blur_l8","temperature"]]

first_crossfading_lines = []
if(WITH_CROSSFADE_COLOR_BLEND) :
	first_crossfading_lines=[["RANK","0","1","2","3","4","5","6","7","8","9","10","11","12","13"],\
	["VERBATIM","svg_output_directory","svg_output_file_name","composition_mode","nb_posterization_layers","nb_composed_files","imageOutputIndex","svg_input_directory_001","svg_input_file_name_001","imageInputIndex_001","svg_input_directory_002","svg_input_file_name_002","imageInputIndex_002","file1_file2_color_blend","ind_old_clip"],\
	["TYPE","string","string","string","int","int","int","string","string","int","string","string","int","float","int"],\
	["ID","svg_output_directory","svg_output_file_name","composition_mode","nb_posterization_layers","nb_composed_files","imageOutputIndex","svg_input_directory_001","svg_input_file_name_001","imageInputIndex_001","svg_input_directory_002","svg_input_file_name_002","imageInputIndex_002","file1_file2_color_blend","ind_old_clip"]]
if(WITH_CROSSFADE_COLOR_LIST) :
	first_crossfading_lines=[["RANK","0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29"],\
	["VERBATIM","svg_output_directory","svg_output_file_name","composition_mode","nb_posterization_layers","nb_composed_files","imageOutputIndex","svg_input_directory_001","svg_input_file_name_001","imageInputIndex_001","svg_input_directory_002","svg_input_file_name_002","imageInputIndex_002","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","ind_old_clip"],\
	["TYPE","string","string","string","int","int","int","string","string","int","string","string","int","string","string","string","string","string","string","string","string","string","string","string","string","string","string","string","string","string","int"],\
	["ID","svg_output_directory","svg_output_file_name","composition_mode","nb_posterization_layers","nb_composed_files","imageOutputIndex","svg_input_directory_001","svg_input_file_name_001","imageInputIndex_001","svg_input_directory_002","svg_input_file_name_002","imageInputIndex_002","fill_color_bg","fill_color_l1","fill_color_l2","fill_color_l3","fill_color_l4","fill_color_l5","fill_color_l6","fill_color_l7","fill_color_l8","stroke_color_l1","stroke_color_l2","stroke_color_l3","stroke_color_l4","stroke_color_l5","stroke_color_l6","stroke_color_l7","stroke_color_l8","ind_old_clip"]]

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
		opts, args = getopt.getopt(main_args,"i:o:c:",["input_scenario_schema=","output_transformation_scenario=","output_crossfade_scenario="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--input_scenario_schema"):
			input_scenario_schema = arg
		elif opt in ("-o", "--output_transformation_scenario"):
			output_transformation_scenario = arg
		elif opt in ("-c", "--output_crossfade_scenario"):
			output_crossfade_scenario = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', dir_of_svg_subdirs)

	# initial scenario schema
	try:
		FILEin = open(input_scenario_schema, "rt")
	except IOError:
		print("File not found :", input_scenario_schema, " or path is incorrect")

	# transformation scenario
	try:
		FILETransformation_out = open(output_transformation_scenario, "wt")
	except IOError:
		print("File not opened :", output_transformation_scenario, " or path is incorrect")

	# crossfade scenario
	try:
		FILECrossfade_out = open(output_crossfade_scenario, "wt")
	except IOError:
		print("File not opened :", output_crossfade_scenario, " or path is incorrect")

	##################################################################
	# READS THE SUBFOLDERS, EXTRACT THE MIDFILE, AND GET THE COLOR TABLE
	##################################################################
	# first lines
	writeTransformationCSV = csv.writer(FILETransformation_out, delimiter=',')
	writeCrossfadeCSV = csv.writer(FILECrossfade_out, delimiter=',')
	# writes the heeader lines to output
	for line_to_output in first_transformation_lines:
		writeTransformationCSV.writerow(line_to_output)
	for line_to_output in first_crossfading_lines:
		writeCrossfadeCSV.writerow(line_to_output)
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	# scenario line
	data_readCSV = list(readCSV)
	# number of scenes (some of them are with one clip, some of them are crossfaded double clip)
	nb_tot_scenes = len(data_readCSV) - 1
	# number of crossfaded scenes
	nb_crossfaded_scenes = 0
	for line in data_readCSV[1:] :
		if(line[1] != "" and line[2] != "") :
			nb_crossfaded_scenes += 1
	# number total of transformations
	nb_tot_transformations = nb_tot_scenes + nb_crossfaded_scenes

	next_line = ["scenario", nb_tot_transformations]
	writeTransformationCSV.writerow(next_line)
	next_line = ["scenario", nb_crossfaded_scenes]
	writeCrossfadeCSV.writerow(next_line)

	# print "line1 ", line 
	FILEin.seek(0)
	line = next(readCSV) 

	# CONSTANTS
	min_temperature_ini = 6600
	min_temperature_end = 1000
	max_temperature_ini = 6600
	max_temperature_end = 6600

	optical_flow_percent_ini = 0.0
	optical_flow_percent_end = 1.0
	optical_flow_factor = 250.0
	# optical_flow_breathing_markers = [1,145,289,440,591,742,893,1044,1195,1343,1490]
	optical_flow_heart_beat_markers = [269,405,509,609,689,789,905,1029,1149,1225,1317,1417,1529,1645,1761,1857,1989,2121,2253,2337,2441,2605,2721,2821,2933,3037,3129,3213,3313,3445,3545,3645,3729,3837,3957,4061,4173,4273,4365,4497,4617,4737,4817,4921,5025,5121,5237,5357,5469,5597,5709,5797]

	# blur
	blur_ini_values = [0,0,0,0,0,0,0,0]
	blur_end_values = [0,0,0,0,0,0,0,0]

	ratio_blur_to_stroke_width = 6
	first_of_scene = 0

	# next line
	line = next(readCSV) 
	# transformation scene number, possibly doubled for crossfading
	sceneTransformation_no = 1 # (<= nb_tot_transformations)
	# crossfade scene number
	sceneCrossfade_no = 1
	# scene number used to calculate varying values such as blur, displacement map indices, color temperatures...
	scene_no = 1 # (<= nb_tot_scenes)
	# whether the preceding scene was crossfaded
	successive_scene_crossfade = 0
	while(line):
		double_clip = False
		clip_names = ["",""]
		if(line[1] != "") :
			clip_names[0] = line[1]
			if clip_names[0].endswith(".MP4") :
				clip_names[0] = clip_names[0][:-len(".MP4")]
			if(clip_names[0] != "") :
				double_clip = True
		if(line[2] != "") :
			clip_names[1] = line[2]
			if clip_names[1].endswith(".MP4") :
				clip_names[1] = clip_names[1][:-len(".MP4")]
			if(clip_names[1] != "") :
				double_clip = True

		fill_colors = [[],[]]
		nb_layers = [8,8]

		# TRANSFORMATIONS
		# write clip transformations (possibly two clips if it is at a crossfading location)
		for ind_clip in range(2) :
			if(clip_names[ind_clip] != "") :
				print("scene", scene_no, "transformation", sceneTransformation_no)

				# current clip name
				clip_name = clip_names[ind_clip]

				scene_ID = ("%03d" % sceneTransformation_no)+"_"+clip_name
				# scene line
				next_line = ["scene",scene_ID]
				writeTransformationCSV.writerow(next_line)

				# scene ID line
				writeTransformationCSV.writerow( [scene_ID] * len(first_transformation_lines[0]) )

				# ID line
				writeTransformationCSV.writerow( first_transformation_lines[3] )

				# story progress
				movie_coef = float(scene_no) / float(nb_tot_scenes)
				next_movie_coef = float(scene_no + 1) / float(nb_tot_scenes)
				prec_movie_coef = float(scene_no - 1) / float(nb_tot_scenes)

				# optical flow image indices
				second_of_scene = first_of_scene + 1 # modulo unuseful because first_of_scene is not the last one in the list
				optical_flow_marker_ini = str(optical_flow_heart_beat_markers[first_of_scene])
				optical_flow_marker_end = str(optical_flow_heart_beat_markers[second_of_scene] - 1)

				# blur
				min_blur = []
				max_blur = []
				for b, B in zip(blur_ini_values, blur_end_values):
					min_blur.append((1-prec_movie_coef) * b + prec_movie_coef * B)
					max_blur.append((1-movie_coef) * b + movie_coef * B)
				min_stroke_width = [i * ratio_blur_to_stroke_width for i in min_blur]
				max_stroke_width = [i * ratio_blur_to_stroke_width for i in max_blur]
				blur_ini = [("%.3f" % i) for i in min_blur]
				blur_end = [("%.3f" % i) for i in max_blur]
				stroke_width_ini = [("%.3f" % i) for i in min_stroke_width]
				stroke_width_end = [("%.3f" % i) for i in max_stroke_width]

				# OF
				of_ini = (1-prec_movie_coef) * optical_flow_percent_ini + prec_movie_coef * optical_flow_percent_end
				of_end = (1-movie_coef) * optical_flow_percent_ini + movie_coef * optical_flow_percent_end
				of_ini = ("%.3f" % of_ini)
				of_end = ("%.3f" % of_end)
				optical_flow_center = "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/svgs_windows_centers/windows_centers_positions.csv"

				# color temperature
				min_temperature = (1-prec_movie_coef) * min_temperature_ini + prec_movie_coef * min_temperature_end
				max_temperature = (1-prec_movie_coef) * max_temperature_ini + prec_movie_coef * max_temperature_end
				temperature_ini = ("%.3f" % min_temperature)
				temperature_end = ("%.3f" % max_temperature)

				# number of layers and lookup table
				# collects the color tale file name together with the number of layers from the file name
				color_table_directory = line[9]
				nb_layers[ind_clip] = 0
				for root, dirs, files in os.walk(color_table_directory) :
					for basename in files :
						lookup = re.search('color_table_'+clip_name+'_([0-9]+)\.txt$', basename)
						if lookup != None :
							nb_layers[ind_clip] = int(lookup.group(1))
							print("Found color table", basename)
							break
				if(nb_layers[ind_clip] <= 0) :
					print("no valid color table found for clip", clip_name, "in dir", color_table_directory)
					sys.exit(2)
				# extraction of the nb_layers[ind_clip] colors from the color table
				try:
					FILEin = open(color_table_directory+basename, "rt")
				except IOError:
					print("File not found :", color_table_directory+basename, " or path is incorrect")
				table_line = FILEin.readline()
				table_line = table_line.rstrip()
				color_list = []
				while(table_line) :
					search_result = re.search(r' (\#[0-9A-Fa-f]{6}) ', table_line)
					if(search_result != None) :
						color_list.append(search_result.group(1))
					else :
						print("Color not found in color table:", table_line)
						sys.exit(2)
					table_line = FILEin.readline()
					table_line = table_line.rstrip()
				if(len(color_list) != nb_layers[ind_clip]) :
					print("Color table unexpected size:", len(color_list), "instead of ", nb_layers[ind_clip])
					sys.exit(2)
				FILEin.close()
				# completes the color list to 8 values with black on top empty layers
				color_list.reverse()
				fill_colors[ind_clip] = ['#000000' for i in range(8)]
				fill_colors[ind_clip][0:len(color_list)] = color_list

				# output name is dawn for single clips that do not require crossfading 
				# otherwise it is the clip name
				if(double_clip == True) :
					svg_output_file_name = clip_name
				else :
					svg_output_file_name = "dawn"

				# initial values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_SVGs/"+clip_name+"/",clip_name,\
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/",svg_output_file_name,\
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/tmp/",\
				str(line[3+ind_clip]), str(line[7]), str(nb_layers[ind_clip]), 'DISPLACEMENT_MAP', of_ini, '#000000', \
				# str(line[7]), str(line[8]), str(line[9]), str(line[10]), str(line[11]), str(line[12]), str(line[13]), str(line[14]),\
				str(fill_colors[ind_clip][0]), str(fill_colors[ind_clip][1]), str(fill_colors[ind_clip][2]), str(fill_colors[ind_clip][3]), str(fill_colors[ind_clip][4]), str(fill_colors[ind_clip][5]), str(fill_colors[ind_clip][6]), str(fill_colors[ind_clip][7]),\
				"1", "1", "1", "1", "1", "1", "1", "1", \
				str(fill_colors[ind_clip][0]), str(fill_colors[ind_clip][1]), str(fill_colors[ind_clip][2]), str(fill_colors[ind_clip][3]), str(fill_colors[ind_clip][4]), str(fill_colors[ind_clip][5]), str(fill_colors[ind_clip][6]), str(fill_colors[ind_clip][7]),\
				"1", "1", "1", "1", "1", "1", "1", "1", \
				stroke_width_ini[0], stroke_width_ini[1], stroke_width_ini[2], stroke_width_ini[3], stroke_width_ini[4], stroke_width_ini[5], stroke_width_ini[6], stroke_width_ini[7], \
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth_recentered/", "breathing_RMI_OF_smooth_recentered", \
				optical_flow_marker_ini, str(optical_flow_factor), str(optical_flow_center),
				blur_ini[0], blur_ini[1], blur_ini[2], blur_ini[3], blur_ini[4], blur_ini[5], blur_ini[6], blur_ini[7], \
				temperature_ini]
				writeTransformationCSV.writerow( next_line )

				# final values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_SVGs/"+clip_name+"/",clip_name,\
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/",svg_output_file_name,\
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/tmp/",\
				str(line[5+ind_clip]), str(line[8]), str(nb_layers[ind_clip]), 'DISPLACEMENT_MAP', of_end, '#000000', \
				str(fill_colors[ind_clip][0]), str(fill_colors[ind_clip][1]), str(fill_colors[ind_clip][2]), str(fill_colors[ind_clip][3]), str(fill_colors[ind_clip][4]), str(fill_colors[ind_clip][5]), str(fill_colors[ind_clip][6]), str(fill_colors[ind_clip][7]),\
				"1", "1", "1", "1", "1", "1", "1", "1", \
				str(fill_colors[ind_clip][0]), str(fill_colors[ind_clip][1]), str(fill_colors[ind_clip][2]), str(fill_colors[ind_clip][3]), str(fill_colors[ind_clip][4]), str(fill_colors[ind_clip][5]), str(fill_colors[ind_clip][6]), str(fill_colors[ind_clip][7]),\
				"1", "1", "1", "1", "1", "1", "1", "1", \
				stroke_width_end[0], stroke_width_end[1], stroke_width_end[2], stroke_width_end[3], stroke_width_end[4], stroke_width_end[5], stroke_width_end[6], stroke_width_end[7], \
				"/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth_recentered/", "breathing_RMI_OF_smooth_recentered", \
				optical_flow_marker_end,str(optical_flow_factor), str(optical_flow_center),
				blur_end[0], blur_end[1], blur_end[2], blur_end[3], blur_end[4], blur_end[5], blur_end[6], blur_end[7], \
				temperature_end]
				writeTransformationCSV.writerow( next_line )

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
				"l", "l", "s",\
				"l", "l", "l", "l", "l", "l", "l", "l", \
				"i"]
				writeTransformationCSV.writerow( next_line )

				# /scene line
				next_line = ["/scene"]
				writeTransformationCSV.writerow(next_line)

				# next transformation scene
				sceneTransformation_no += 1

		# CROSSFADING
		if(clip_names[0] != "" and clip_names[1] != "") :
			# current clip name
			scene_ID = ("%03d" % sceneCrossfade_no)+"_"+clip_names[0]+"_"+clip_names[1]
			# scene line
			next_line = ["scene",scene_ID]
			writeCrossfadeCSV.writerow(next_line)

			# scene ID line
			writeCrossfadeCSV.writerow( [scene_ID] * len(first_crossfading_lines[0]) )

			# ID line
			writeCrossfadeCSV.writerow( first_crossfading_lines[3] )

			#selects the color this way
			# first crossfade
			# colors old clip to average colors new and old
			# second crossfade
			# average colors new and old to colors new clip
			if(successive_scene_crossfade == 0) :
				if(line[3] == "1") :
					ind_new_clip = 0
				else :
					ind_new_clip = 1
				ind_old_clip = 1 - ind_new_clip

			if(WITH_CROSSFADE_COLOR_LIST) :
				# calculation of the average colors
				average_colors = list(map(lambda x, y :mix_color(x, y, 0.5), fill_colors[0], fill_colors[1]))
				# first crossfade
				if(successive_scene_crossfade == 0) :
					colors_ini = fill_colors[ind_old_clip]
					colors_end = average_colors
				# second crossfade
				else :
					colors_ini = average_colors
					colors_end = fill_colors[ind_new_clip]

				# initial values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
				"PARALLEL", "8", "2", str(line[7]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[0], str(line[7]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[1], str(line[7]),
				'#000000', str(colors_ini[0]), str(colors_ini[1]), str(colors_ini[2]), str(colors_ini[3]), str(colors_ini[4]), str(colors_ini[5]), str(colors_ini[6]), str(colors_ini[7]),
				str(colors_ini[0]), str(colors_ini[1]), str(colors_ini[2]), str(colors_ini[3]), str(colors_ini[4]), str(colors_ini[5]), str(colors_ini[6]), str(colors_ini[7]), str(ind_old_clip)]
				writeCrossfadeCSV.writerow( next_line )

				# final values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
				"PARALLEL", "8", "2", str(line[8]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[0], str(line[8]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[1], str(line[8]),
				'#000000', str(colors_end[0]), str(colors_end[1]), str(colors_end[2]), str(colors_end[3]), str(colors_end[4]), str(colors_end[5]), str(colors_end[6]), str(colors_end[7]),
				str(colors_end[0]), str(colors_end[1]), str(colors_end[2]), str(colors_end[3]), str(colors_end[4]), str(colors_end[5]), str(colors_end[6]), str(colors_end[7]),str(ind_old_clip)]
				writeCrossfadeCSV.writerow( next_line )

			if(WITH_CROSSFADE_COLOR_BLEND) :
				# first crossfade
				if(successive_scene_crossfade == 0) :
					blend_ini = ind_old_clip
					blend_end = 0.5
				# second crossfade
				else :
					blend_ini = 0.5
					blend_end = ind_new_clip

				# initial values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
				"PARALLEL", "8", "2", str(line[7]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[0], str(line[7]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[1], str(line[7]),str(blend_ini), str(ind_old_clip)]
				writeCrossfadeCSV.writerow( next_line )

				# final values line
				next_line = ["", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/","dawn",\
				"PARALLEL", "8", "2", str(line[8]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[0], str(line[8]), \
				'/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/', clip_names[1], str(line[8]),str(blend_end), str(ind_old_clip)]
				writeCrossfadeCSV.writerow( next_line )

			# interpolator line
			next_line = []
			if(WITH_CROSSFADE_COLOR_BLEND) :
				next_line = ["","s","s","s","s","s","s","s","s","s","s","s","s","l","s"]
			if(WITH_CROSSFADE_COLOR_LIST) :
				next_line = ["","s","s","s","s","s","s","s","s","s","s","s","s","l","l","l","l","l","l","l","l","l","l","l","l","l","l","l","l","l","s"]
			writeCrossfadeCSV.writerow( next_line )

			# /scene line
			next_line = ["/scene"]
			writeCrossfadeCSV.writerow(next_line)

			# next scene
			sceneCrossfade_no += 1
			successive_scene_crossfade += 1
		else :
			successive_scene_crossfade = 0

		# next transformation scene
		scene_no += 1

		# next optical flow scene
		first_of_scene += 1
		# takes the scene modulo the size of the list
		# if it is the last one in the list, takes the first one to make 
		# sure that the next value is the next in the list
		if(first_of_scene == len(optical_flow_heart_beat_markers) - 1) :
			first_of_scene = 0

		# next line
		try:
			line = next(readCSV) 
		except StopIteration:
			break


	# /scenario line
	next_line = ["/scenario"]
	writeTransformationCSV.writerow(next_line)
	writeCrossfadeCSV.writerow(next_line)

	FILEin.close()
	FILETransformation_out.close()
	FILECrossfade_out.close()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
