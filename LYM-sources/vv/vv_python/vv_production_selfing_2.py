#!/usr/bin/python3 

# vv_production_weaving-2.py
# production of vector video (excluding initial posterization and final rasterization)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import random

import getopt
import sys
from signal import signal, SIGINT

import csv

import re
import operator


import vv_scenario_transform
import vv_scenario_layer_crossfade
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate
from vv_lib import variable_dict_to_CSV_file
from vv_lib import variable_dict_append_CSV_file
from vv_lib import saturate_color

from vv_const import *

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_production_selfing.py -i scenario_file
	ARGUMENTS:
	-i: input scenariofile
'''

##################################################################
# HASHES FOR CLIP management
##################################################################

# LINUX COMMAND to retrieve the number of files in subdirectories
# find . -type f | awk '{dir=gensub(/(.+\/).+/,"\\1","g",$0); dir_list[dir]++} END {for (d in dir_list) printf "%s %s\n",d,dir_list[d]}' | sort
selfieMovies_clip_sizes = { \
	"Cunningham_Looks_001" : [366, False], \
	"Cunningham_Looks_002" : [342, False], \
	"Cunningham_Looks_003" : [347, False], \
	"Cunningham_Looks_004" : [196, False], \
	"Cunningham_Looks_005" : [157, False], \
	"Cunningham_Looks_006" : [177, False], \
	"Cunningham_Looks_007" : [195, False], \
	"Cunningham_Looks_008" : [330, False], \
	"Cunningham_Looks_009" : [195, False], \
	"Cunningham_Looks_010" : [178, False], \
	"Cunningham_Looks_011" : [207, False], \
	"Cunningham_Looks_012" : [308, False], \
	"Cunningham_Looks_013" : [196, False], \
	"Cunningham_Looks_014" : [171, False], \
	"Cunningham_Looks_015" : [105, False], \
	"Cunningham_Looks_016" : [244, False], \
	"Cunningham_Looks_017" : [96, False], \
	"Cunningham_Looks_018" : [266, False], \
	"Cunningham_Looks_019" : [312, False], \
	"Cunningham_Looks_020" : [398, False], \
	"Cunningham_Looks_021" : [331, False], \
	"Cunningham_Looks_022" : [189, False], \
	"Cunningham_Looks_023" : [158, False], \
	"Cunningham_Looks_024" : [360, False], \
	"Cunningham_Looks_025" : [302, False], \
	"Cunningham_Looks_026" : [142, False], \
	"Cunningham_Looks_027" : [196, False], \
	# "Cunningham_Looks_028" : [168, False], \ # not well centered
	"Cunningham_Looks_029" : [163, False], \
	"Cunningham_Looks_030" : [128, False], \
	"Cunningham_Looks_031" : [119, False], \
	"Cunningham_Looks_032" : [499, False], \
	# "Cunningham_Looks_033" : [327, False], \ # 2 poses
	"Cunningham_Looks_034" : [245, False], \
	"Cunningham_Looks_035" : [238, False], \
	"Cunningham_Looks_036" : [246, False], \
	"Cunningham_Looks_037" : [249, False], \
	"Cunningham_Looks_038" : [207, False], \
	# "Cunningham_Looks_039" : [329, False], \ # not static
	# "Cunningham_Looks_040" : [370, False], \ # 2 poses
	# "Cunningham_Looks_041" : [78, False], \ # too short
	"Cunningham_Looks_042" : [240, False], \
	# "Cunningham_Looks_043" : [299, False], \ # not static
	# "Cunningham_Looks_044" : [386, False], \ # not well centered
	# "Cunningham_Looks_045" : [190, False], \ # not well centered
	# "Cunningham_Looks_046" : [589, False], \ # not well centered and not well centered
	"Cunningham_Looks_047" : [412, False], \
	# "Cunningham_Looks_048" : [384, False], \ # not static
	"Cunningham_Looks_049" : [362, False], \
	"Cunningham_Looks_050" : [449, False], \
	"Cunningham_Looks_051" : [191, False], \
	"Cunningham_Looks_052" : [316, False], \
	"Cunningham_Looks_053" : [454, False], \
	# "Cunningham_Looks_054" : [422, False], \ # not static
	"Cunningham_Looks_055" : [319, False], \
	"Cunningham_Looks_056" : [233, False], \
	"Cunningham_Looks_057" : [222, False], \
	"Cunningham_Looks_058" : [276, False], \
	"Cunningham_Looks_059" : [342, False], \
	"Cunningham_Looks_060" : [211, False], \
	"Cunningham_Looks_061" : [113, False], \
	"Cunningham_Looks_062" : [284, False], \
	"Cunningham_Looks_063" : [369, False], \
	"Cunningham_Looks_064" : [200, False], \
	"Cunningham_Looks_065" : [320, False], \
	"Cunningham_Looks_066" : [310, False], \
	"Cunningham_Looks_067" : [252, False], \
	"Cunningham_Looks_068" : [328, False], \
	"Cunningham_Looks_069" : [470, False], \
	"Cunningham_Looks_070" : [118, False] }

selfieStills_clip_sizes = { \
	"Cunningham_Looks_still_001" : [1, False], \
	"Cunningham_Looks_still_002" : [1, False], \
	"Cunningham_Looks_still_003" : [1, False], \
	"Cunningham_Looks_still_004" : [1, False], \
	"Cunningham_Looks_still_005" : [1, False], \
	"Cunningham_Looks_still_006" : [1, False], \
	"Cunningham_Looks_still_007" : [1, False], \
	"Cunningham_Looks_still_008" : [1, False], \
	"Cunningham_Looks_still_009" : [1, False], \
	"Cunningham_Looks_still_010" : [1, False], \
	"Cunningham_Looks_still_011" : [1, False], \
	"Cunningham_Looks_still_012" : [1, False], \
	"Cunningham_Looks_still_013" : [1, False], \
	"Cunningham_Looks_still_014" : [1, False], \
	"Cunningham_Looks_still_015" : [1, False], \
	"Cunningham_Looks_still_016" : [1, False], \
	"Cunningham_Looks_still_017" : [1, False], \
	"Cunningham_Looks_still_018" : [1, False], \
	"Cunningham_Looks_still_019" : [1, False], \
	"Cunningham_Looks_still_020" : [1, False], \
	"Cunningham_Looks_still_021" : [1, False], \
	"Cunningham_Looks_still_022" : [1, False], \
	"Cunningham_Looks_still_023" : [1, False], \
	"Cunningham_Looks_still_024" : [1, False], \
	"Cunningham_Looks_still_025" : [1, False], \
	"Cunningham_Looks_still_026" : [1, False], \
	"Cunningham_Looks_still_027" : [1, False], \
	# "Cunningham_Looks_still_028" : [1, False], \ # not well centered
	"Cunningham_Looks_still_029" : [1, False], \
	"Cunningham_Looks_still_030" : [1, False], \
	"Cunningham_Looks_still_031" : [1, False], \
	"Cunningham_Looks_still_032" : [1, False], \
	# "Cunningham_Looks_still_033" : [1, False], \ # 2 poses
	"Cunningham_Looks_still_034" : [1, False], \
	"Cunningham_Looks_still_035" : [1, False], \
	"Cunningham_Looks_still_036" : [1, False], \
	"Cunningham_Looks_still_037" : [1, False], \
	"Cunningham_Looks_still_038" : [1, False], \
	# "Cunningham_Looks_still_039" : [1, False], \ # not static
	# "Cunningham_Looks_still_040" : [1, False], \ # 2 poses
	# "Cunningham_Looks_still_041" : [1, False], \ # too short
	"Cunningham_Looks_still_042" : [1, False], \
	# "Cunningham_Looks_still_043" : [1, False], \ # not static
	# "Cunningham_Looks_still_044" : [1, False], \ # not well centered
	# "Cunningham_Looks_still_045" : [1, False], \ # not well centered
	# "Cunningham_Looks_still_046" : [1, False], \ # not well centered and not well centered
	"Cunningham_Looks_still_047" : [1, False], \
	# "Cunningham_Looks_still_048" : [1, False], \ # not static
	"Cunningham_Looks_still_049" : [1, False], \
	"Cunningham_Looks_still_050" : [1, False], \
	"Cunningham_Looks_still_051" : [1, False], \
	"Cunningham_Looks_still_052" : [1, False], \
	"Cunningham_Looks_still_053" : [1, False], \
	# "Cunningham_Looks_still_054" : [1, False], \ # not static
	"Cunningham_Looks_still_055" : [1, False], \
	"Cunningham_Looks_still_056" : [1, False], \
	"Cunningham_Looks_still_057" : [1, False], \
	"Cunningham_Looks_still_058" : [1, False], \
	"Cunningham_Looks_still_059" : [1, False], \
	"Cunningham_Looks_still_060" : [1, False], \
	"Cunningham_Looks_still_061" : [1, False], \
	"Cunningham_Looks_still_062" : [1, False], \
	"Cunningham_Looks_still_063" : [1, False], \
	"Cunningham_Looks_still_064" : [1, False], \
	"Cunningham_Looks_still_065" : [1, False], \
	"Cunningham_Looks_still_066" : [1, False], \
	"Cunningham_Looks_still_067" : [1, False], \
	"Cunningham_Looks_still_068" : [1, False], \
	"Cunningham_Looks_still_069" : [1, False], \
	"Cunningham_Looks_still_070" : [1, False] }

chamane_clip_sizes = ["Annika_silhouette_000", 3213]

class ActiveClip:
	def __init__(self, clip_name, clip_size, clip_first_frame_no, clip_fill_color_l1, \
		clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
		clip_scaling_center_x, clip_scaling_center_y, clip_x_translation_ini, clip_y_translation_ini):
		self.update_activeClip_data(clip_name, clip_size, clip_first_frame_no, clip_fill_color_l1, \
		clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
		clip_scaling_center_x, clip_scaling_center_y, clip_x_translation_ini, clip_y_translation_ini)

	def update_activeClip_data(self, clip_name, clip_size, clip_first_frame_no, clip_fill_color_l1, \
		clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
		clip_scaling_center_x, clip_scaling_center_y, clip_x_translation_ini, clip_y_translation_ini):
		self.clip_name = clip_name 
		self.clip_size = clip_size 
		self.clip_first_frame_no = clip_first_frame_no 
		self.clip_fill_color_l1 = clip_fill_color_l1
		self.clip_stroke_color_l1 = clip_stroke_color_l1 
		self.clip_fill_opacity = clip_fill_opacity 
		self.clip_stroke_opacity_l1 = clip_stroke_opacity_l1 
		self.clip_stroke_width = clip_stroke_width
		self.clip_scaling_center_x = clip_scaling_center_x 
		self.clip_scaling_center_y = clip_scaling_center_y 
		self.clip_x_translation_ini = clip_x_translation_ini 
		self.clip_y_translation_ini = clip_y_translation_ini

	def update_activeClip_ID(self, clip_name, clip_size):
		self.clip_name = clip_name 
		self.clip_size = clip_size 

# dict of lists, each list contains
# clip_name, clip_size, clip_first_frame_no, clip_fill_color_bg, clip_fill_color_l1, \
# 	clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
# 	clip_scaling_center_x, clip_scaling_center_y, clip_x_translation_ini, clip_y_translation_ini
# the key is the clip name
active_performing_clips = []

transformation_scenario_file_name = ""
crossfading_scenario_file_name = ""
prod_scenario_row = ""

tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/tmp/"
transformed_clip_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/movie_transformed_SVGs/silhouette/"
spider_web_positions_file_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/logs/LYM_araknit_log_liftShake_seed2020_2x.csv"

# SCENARIO VARIABLES
var_names = []
var_types = dict()
val_init = dict()
val_fin = dict()
val_interp = dict()
interp = dict()

##################################################################
# FLASH MANAGEMENT
##################################################################
def vv_blur_and_fill(input_file_name_tmp, output_file_name, blur_radius, fill_color) :
	#  <defs
	#	   <filter id=\"blur_flash\">
	#		   <feGaussianBlur stdDeviation=\"'+str(blur_radius)+'\" />
	#	   </filter>
	#  </defs>
	# <g vv-transformation="SCALE/TRANSLATE/VH_RANDOM" vv-nb_frames="" vv-percent_life="" vv-translation="-3550.249318525104x0" 
	#   vv-scale="19200.0x0x1.0" transform="translate(0.000000,2160.000000) scale(0.100000,-0.100000)" 
	#	style="stroke:#000000; stroke-width:20.0px; stroke-opacity:1.0; fill:#FFFFFF; fill-opacity:0; " >
	# -> style="filter:url(#blur_flash); stroke:#000000; stroke-width:0; stroke-opacity:1.0; fill:#ff0000; fill-opacity:0; " >
	try:
		FILEin_tmp = open(input_file_name_tmp, "rt")
	except IOError:
		print("File not found:", input_file_name_tmp, " or path is incorrect")

	try:
		FILEout_blur = open(output_file_name, "wt")
	except IOError:
		print("File not opened:", output_file_name, " or path is incorrect")

	filter_line_string = '''  <filter id=\"blur_flash\">
	  <feGaussianBlur stdDeviation=\"'''+str(blur_radius)+'''\" />
  </filter>
'''

	##################################################################
	# STORES THE LINES IN AN ARRAY
	##################################################################
	input_lines = FILEin_tmp.readlines()
	with_def = False
	is_rect = False
	for line_string in input_lines: 
		if(re.search(r'<rect', line_string) != None) :
			is_rect = True
		if(re.search(r'<defs', line_string) != None) :
			with_def = True
			FILEout_blur.write(line_string)
			line_string = filter_line_string
		elif(re.search(r'<metadata', line_string) != None and (not with_def)) :
			FILEout_blur.write('<defs>\n')
			FILEout_blur.write(filter_line_string)
			FILEout_blur.write('</defs>\n')
		if(not is_rect and re.search(r'style\=\"', line_string) != None) :
			line_string = re.sub(r'style\=\"stroke', r'style="filter:url(#blur_flash); stroke', line_string)
		if(not is_rect and re.search(r'stroke\-width\:[^;]+\;', line_string) != None) :
			line_string = re.sub(r'stroke\-width\:[^;]+\;', r'stroke-width:0;', line_string)
		if(not is_rect and re.search(r'fill\:[^;]+\;', line_string) != None) :
			line_string = re.sub(r'fill\:[^;]+\;', r'fill:#ff0000;', line_string)
		if(is_rect and re.search(r'>', line_string) != None) :
			is_rect = False
		FILEout_blur.write(line_string)
	FILEin_tmp.close()
	FILEout_blur.close()


# recuperer la translation du fichier de l'avant derniere couche (le fichier du dessus de pile et dernier de la liste)
# vv-translation="6085.332129993109x0"
# et le substituer dans toutes les couches du fichier still
# transform="translate(608.50,
# à la place de transform="translate(0.000000,
# (le dizième est choisi car il y a un changement d'échelle de 0.1 dajà fait)
def vv_translate_top_layer_as_lower_one(top_transformed_clip_name_and_path, still_file_name) :
	# print("vv_translate_top_layer_as_lower_one",top_transformed_clip_name_and_path, still_file_name)
	##################################################################
	# STORES THE LINES IN AN ARRAY AND LOOKS FOR TRANSLATION
	##################################################################
	try:
		FILEin_top = open(top_transformed_clip_name_and_path, "rt")
	except IOError:
		print("File not found:", top_transformed_clip_name_and_path, " or path is incorrect")
	top_file_translation = 0.
	input_lines = FILEin_top.readlines()
	with_def = False
	for line_string in input_lines: 
		search_result = re.search(r'vv-translation="([^x]+)x', line_string)	
		if(search_result != None) :
			top_file_translation = force_num(search_result.group(1))
			break
	FILEin_top.close()
	# print("vv-translation", top_file_translation)

	##################################################################
	# TRANSLATE EACH LAYER OF THE STILL ACCORDING TO THE TRANSLATION
	##################################################################
	try:
		FILEin_still = open(still_file_name, "rt")
	except IOError:
		print("File not found:", still_file_name, " or path is incorrect")
	try:
		FILEout_new_top = open(top_transformed_clip_name_and_path, "wt")
	except IOError:
		print("File not opened:", top_transformed_clip_name_and_path, " or path is incorrect")
	input_lines = FILEin_still.readlines()
	is_rect = False
	for line_string in input_lines: 
		# transation of groups similar to the translation of the silhouette
		if(re.search(r'transform="translate\(([^)]+)\,', line_string)	 != None) :
			# translation is divided by 10.0 due to the 0.1 scaling from posterization
			line_string = re.sub(r'transform="translate\(([^)]+)\,', r'transform="translate('+str(top_file_translation/10.)+',', line_string)
		# the first rectangle (the background), should be white, not black
		if(re.search(r'<rect', line_string)	!= None) :
			is_rect = True
		if(is_rect) :
			if(re.search(r'fill:#[0-9a-fA-F]+\;', line_string) != None) :
				# background is white
				line_string = re.sub(r'fill:#[0-9a-fA-F]+\;', r'fill:#FFFFFF;', line_string)
		if(re.search(r'>', line_string) != None) :
			is_rect = False
		FILEout_new_top.write(line_string)
	FILEin_still.close()
	FILEout_new_top.close()

##################################################################
# MAIN
##################################################################
def main(main_args):
	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', scenario_file_name)

	##################################################################
	# RANDOM SEEDING
	##################################################################
	random.seed(VV_RANDOM_SEED)

	##################################################################
	# FIRST READ OF THE SCENARIO TO CAPTURE SOME GLOBAL VARIABLES
	##################################################################
	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
		print("File not found:", scenario_file_name, " or path is incorrect")

	line_no = 1
	scene_no = -1
	scenarioCSV = csv.reader(FILEin, delimiter=',')
	index_imageInputIndex = -1
	imageInputIndex_Initial = -1
	imageInputIndex_Final = -1
	index_imageOutputIndex = -1
	imageOutputIndex_Initial = -1
	imageOutputIndex_Final = -1
	scene_duration = []
	scene_imageInputIndex_Initial = []
	scene_imageInputIndex_Final = []
	scene_imageOutputIndex_Initial = []
	scene_imageOutputIndex_Final = []
	for prod_scenario_row in scenarioCSV:
		if(prod_scenario_row[0] == "ID") :
			index_imageInputIndex = prod_scenario_row.index("imageInputIndex")
			index_imageOutputIndex = prod_scenario_row.index("imageOutputIndex")
		if(prod_scenario_row[0] == "scene") :
			scene_no += 1
			imageInputIndex_Initial = -1
			imageInputIndex_Final = -1
			imageOutputIndex_Initial = -1
			imageOutputIndex_Final = -1
		if(index_imageInputIndex > 0 and prod_scenario_row[0] == "Initial") :
			imageInputIndex_Initial = int(force_num(prod_scenario_row[index_imageInputIndex]))
			scene_imageInputIndex_Initial.append(imageInputIndex_Initial)
		if(index_imageInputIndex > 0 and prod_scenario_row[0] == "Final") :
			imageInputIndex_Final = int(force_num(prod_scenario_row[index_imageInputIndex]))
			scene_imageInputIndex_Final.append(imageInputIndex_Final)
			if(imageInputIndex_Initial >= 0) :
				scene_duration.append(imageInputIndex_Final - imageInputIndex_Initial + 1)
		if(index_imageOutputIndex > 0 and prod_scenario_row[0] == "Initial") :
			imageOutputIndex_Initial = int(force_num(prod_scenario_row[index_imageOutputIndex]))
			scene_imageOutputIndex_Initial.append(imageOutputIndex_Initial)
		if(index_imageOutputIndex > 0 and prod_scenario_row[0] == "Final") :
			imageOutputIndex_Final = int(force_num(prod_scenario_row[index_imageOutputIndex]))
			scene_imageOutputIndex_Final.append(imageOutputIndex_Final)
		line_no += 1

	if(scene_no != 3 and len(scene_imageInputIndex_Initial) != 4 or len(scene_imageInputIndex_Final) != 4 or len(scene_duration) != 4) :
		print("Incomplete reading of scene durations",scene_no, len(scene_duration))

	FILEin.close()

	scene_no = -1
	for val_ini, val_end, val_dur, valout_ini, valout_end in zip(scene_imageInputIndex_Initial, scene_imageInputIndex_Final, scene_duration, scene_imageOutputIndex_Initial, scene_imageOutputIndex_Final) :
		scene_no += 1
		print("scene no ini fin dur out ini fin", scene_no, val_ini, val_end, val_dur, valout_ini, valout_ini, valout_end)

	##################################################################
	# OPENS THE SCENARIO FILE + THE WEB POSITIONS
	# AND EXECUTES THE PRODUCTION SCENARIO ACCORDING TO THE SCENARIO FILE
	##################################################################
	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
		print("File not found:", scenario_file_name, " or path is incorrect")

	try:
		FILEweb_positions_in = open(spider_web_positions_file_name, "rt")
	except IOError:
		print("File not opened:", spider_web_positions_file_name, " or path is incorrect")

	##################################################################
	# READS THE POSITIONS OF THE SILHOUETTE ON THE WEB FOR LAST SCENES
	##################################################################
	read_web_positions_CSV = csv.reader(FILEweb_positions_in, delimiter=',')
	#reads the header
	header_web_positions =  next(read_web_positions_CSV)
	# deduces the number of available positions on the web
	nb_positions_on_web = int(int(len(header_web_positions) - 1) / 3)
	# creates the dictionnary of positions of clips on the web
	selfie_stills_indices_on_web = dict()
	position_on_web_is_available = [True] * nb_positions_on_web
	nb_available_positions_on_web = nb_positions_on_web
	#reads the first row
	line_web_positions =  next(read_web_positions_CSV) 

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	scenarioCSV = csv.reader(FILEin, delimiter=',')
	prod_scenario_row =  next(scenarioCSV) 
	# print( "line1 ", prod_scenario_row  )
	prod_scenario_row =  next(scenarioCSV) 
	# print( "line2 ", prod_scenario_row  )

	#3rd row: variable types
	line_types =  next(scenarioCSV)

	#4th row: variable names
	var_names =  next(scenarioCSV) 
	#removes the first one
	var_names.pop(0)

	#finishes the types now that the variable strings are known
	#removes the first one
	line_types.pop(0)
	indVar = 0
	for line_type in line_types :
		var_types[var_names[indVar]] = line_type
		indVar += 1
	
	# scenario row
	prod_scenario_row =  next(scenarioCSV)
	if prod_scenario_row[0] != "scenario":
		print( "Expected tag \"scenario\" not found in row: ", prod_scenario_row)
		return 0
	
	nb_scenes = int(prod_scenario_row[1])
	# print( "nb_scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A CHAPTER
	##################################################################
	for ind_scene in range(nb_scenes) :
		# scene row
		prod_scenario_row =  next(scenarioCSV)
		if prod_scenario_row[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = prod_scenario_row[1]

		# scene ID row
		prod_scenario_row =  next(scenarioCSV)
		# var IDs row
		prod_scenario_row =  next(scenarioCSV)
	
		# variable initial value
		prod_scenario_row =  next(scenarioCSV)
		prod_scenario_row.pop(0)
		indVar = 0
		print(prod_scenario_row)
		for value_init in prod_scenario_row:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		prod_scenario_row =  next(scenarioCSV)
		prod_scenario_row.pop(0)
		indVar = 0
		for value_fin in prod_scenario_row:
			val_fin[var_names[indVar]] = to_num(value_fin)
			indVar += 1

		# variable interpolation value
		prod_scenario_row =  next(scenarioCSV)
		prod_scenario_row.pop(0)
		indVar = 0
		for value_interp in prod_scenario_row:
			interp[var_names[indVar]] = value_interp	
			indVar += 1

		ind_chapter = val_init["chapter"]
		
		# applies possible transformations on the SVG files
		# creates the directories in which the paths files are saved
		if( not os.path.isdir(val_init["svg_output_directory"])  ):
			try:
				os.mkdir(val_init["svg_output_directory"])
			except OSError:
				print ("Creation of the directory %s failed" % val_init["svg_output_directory"])
			else:
				print ("Successfully created the directory %s " % val_init["svg_output_directory"])

		##################################################################
		# SCANS ALL THE FRAMES IN THE MOVIE
		##################################################################
		current_crossfaded_frame_out = val_init["imageOutputIndex"]
		nb_layers = val_init["nb_posterization_layers"]
		tmp_dir = val_init["tmp_directory"]
		# steps of rendering 
		output_step = 1
		if "output_step" in val_init :
			output_step = int(to_num(val_init["output_step"]))
		output_ini = 0
		if "output_ini" in val_init :
			output_ini = int(to_num(val_init["output_ini"]))

		for indImageInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + 1, 1):
			countInput = "%04d" % indImageInput
			countOutput = "%04d" % current_crossfaded_frame_out

			# if(current_crossfaded_frame_out != 6200 or current_crossfaded_frame_out % 20 != 0) :
			# if(current_crossfaded_frame_out != 4200 and current_crossfaded_frame_out != 4201) :
			# 	TRACE_ONLY = True
			# else :
			# 	TRACE_ONLY = False
			# if(current_crossfaded_frame_out == 4202) :
			# 	return

			scene_percentage = 0
			scene_nb_frames = val_fin["imageInputIndex"] - val_init["imageInputIndex"]
			if scene_nb_frames != 0 :
				scene_percentage = float(indImageInput - val_init["imageInputIndex"])/scene_nb_frames
			
			#clamping
			scene_percentage = max(min(scene_percentage, 1.0), 0.0)
			# interpolating all the variables
			for var_name in var_names :
				# print( "var name [",var_name,"]\n" )
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)

			##################################################################
			##################################################################
			# FOR THE WHOLE MOVIE: FRAME BY FRAM CLIP COMPOSITION 
			# 1: PARAMETER INTERPOLATION
			# 2: MANAGEMENT OF CLIP ACTIVATION
			# 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
			# 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
			# 5: TRANSFORMATIONS OF THE CROSSFADED FRAMES
			##################################################################
			##################################################################
			# FOR ALL THE FRAMES IN THE MOVIES

			# number of transformed performing clips
			nb_transformed_performing_clips = 0

			##################################################################
			# 1: PARAMETERS INTERPOLATION
			##################################################################
			# TRANSFORMATION PERCENTAGE management
			percent_transf3 = val_interp["percent_transf3"]

			# LINK PERCENTAGE management
			percent_links = val_interp["percent_links"]

			# LINK COLOR management
			stroke_color_links = val_interp["stroke_color_links"]

			# FILL COLOR management (chapters 2, 3 and 4)
			fill_color_bg = val_interp["fill_color_bg"]
			fill_color_l1 = val_interp["fill_color_l1"]
			fill_color_l2 = val_interp["fill_color_l2"]
			fill_color_l3 = val_interp["fill_color_l3"]
			fill_color_l4 = val_interp["fill_color_l4"]
			fill_color_l5 = val_interp["fill_color_l5"]
			fill_color_l6 = val_interp["fill_color_l6"]
			fill_color_l7 = val_interp["fill_color_l7"]
			fill_color_l8 = val_interp["fill_color_l8"]

			# STROKE COLOR management (chapters 2 and 3)
			stroke_color = val_interp["stroke_color_l1"]

			# STROKE WIDTH management
			stroke_width_l1 = val_interp["stroke_width"]

			# OPACITY
			fill_opacity = val_interp["fill_opacity_l1"]

			# NUMBER OF ACTIVE CLIPS management
			nb_transformed_performing_clips = val_interp["nb_active_clips"]

			##################################################################
			# 2: management OF CLIP ACTIVATION
			##################################################################

			##################################################################
			# FIRST SCENE: ACTIVATING NEW PERFORMING CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
			if(ind_chapter == 0) :
				# new clip activation
				# print( "current expected number of clips " , nb_transformed_performing_clips , "\n" )
				for indNewClipActivation in range(len(active_performing_clips), nb_transformed_performing_clips) :
					# stroke and fill parameters selection
					# random fill color
					mix_red = "%02x" % min(int(random.random() * 255), 255)
					mix_green = "%02x" %  min(int(random.random() * 255), 255)
					mix_blue = "%02x" %  min(int(random.random() * 255), 255)
					clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
					clip_fill_color_l1 = '#' + mix_red + mix_green + mix_blue
					clip_fill_color_l2 = '#000000'
					clip_fill_color_l3 = '#000000'
					clip_fill_color_l4 = '#000000'
					clip_fill_color_l5 = '#000000'
					clip_fill_color_l6 = '#000000'
					clip_fill_color_l7 = '#000000'
					clip_fill_color_l8 = '#000000'
					# random translation in either halves of the images
					if( indNewClipActivation % 2 == 0 ) :
						clip_x_translation_ini = random.random() * (VV_SVG_PAGE_WIDTH / 2 - 200)
					else :
						clip_x_translation_ini = - random.random() * (VV_SVG_PAGE_WIDTH / 2 - 200)
					clip_y_translation_ini = 0.1 * VV_SVG_PAGE_HEIGHT

					# fixed values
					clip_stroke_opacity_l1 = 1.0

					# modified when used
					clip_stroke_color_l1 = '#000000'
					clip_fill_opacity = 1.0

					# random choice of a clip in the list
					while(True) :
						newActivatedClipID = random.choice(list(selfieMovies_clip_sizes))
						newActivatedClipSize, already_used = selfieMovies_clip_sizes[newActivatedClipID]
						if(not already_used) :
							selfieMovies_clip_sizes[newActivatedClipID] = [newActivatedClipSize, True]
							break

					# new clip activation
					active_performing_clips.append(ActiveClip(newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, \
							clip_fill_color_l1, clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, \
							stroke_width_l1, VV_SVG_PAGE_WIDTH / 2, 0, clip_x_translation_ini, clip_y_translation_ini))

			##################################################################
			# SECOND SCENE FIRST IMAGE: ACTIVATES THE SHAMAN TRAVERSAL
			# AND ASSIGNS SELFIES TO POSITIONS ON THE WEB
			elif(ind_chapter == 1 and indImageInput == val_init["imageInputIndex"]) :
				##################################################################
				# SECOND SCENE FIRST IMAGE: ACTIVATES THE SHAMAN TRAVERSAL
				newActivatedClipID = chamane_clip_sizes[0]
				newActivatedClipSize = chamane_clip_sizes[1]
				clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
				clip_fill_color_l1 = '#FFFFFF'
				clip_fill_color_l2 = '#FFFFFF'
				clip_fill_color_l3 = '#FFFFFF'
				clip_fill_color_l4 = '#FFFFFF'
				clip_fill_color_l5 = '#FFFFFF'
				clip_fill_color_l6 = '#FFFFFF'
				clip_fill_color_l7 = '#FFFFFF'
				clip_fill_color_l8 = '#FFFFFF'
				clip_fill_opacity = 0.7
				clip_stroke_color_l1 = '#FF0000'
				clip_stroke_opacity_l1 = 1.0
				shaman_stroke_width = stroke_width_l1

				# SECOND SCENE FIRST IMAGE: SHAMAN CLIP ACTIVATION
				# new clip activation, this clip is not translated to have the full traversal of the screen
				active_performing_clips.append(ActiveClip(newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, \
						clip_fill_color_l1, clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, \
						shaman_stroke_width, VV_SVG_PAGE_WIDTH / 2, 0, 0, 0))

				##################################################################
				# SECOND SCENE FIRST IMAGE: ASSIGNS SELFIES TO POSITIONS ON THE WEB
				# loop on the active clips to replace the clip ID and initial frame (except the last one, the shamane)
				for clip_index in range(len(active_performing_clips) - 1) :
					# assigns selfies to positions on the web
					# creates the dictionnary of positions of clips on the web
					if(nb_available_positions_on_web > 0) :
						rand_jumps = int(random.random() * nb_positions_on_web)
						for i in range(nb_positions_on_web) :
							cur_position_index = (rand_jumps + i) % nb_positions_on_web
							if(position_on_web_is_available[cur_position_index] == True) :
								position_on_web_is_available[cur_position_index] = False
								selfie_stills_indices_on_web[clip_index] = cur_position_index
								nb_available_positions_on_web -= 1
								break
					else :
						selfie_stills_indices_on_web[clip_index] = -1

			##################################################################
			# SECOND SCENE MID IMAGE: REPLACES THE SELFIES MOVIES BY THE SELFIES STILLS EXCEPT THE LAST ONE: THE SHAMAN
			# loop on the active clips to replace the clip ID and initial frame
			elif(ind_chapter == 1 and indImageInput == int(val_init["imageInputIndex"] + (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) / 2) ) :
				for clip_index in range(len(active_performing_clips) - 1) :
					activeClip = active_performing_clips[clip_index]

					# random choice of a clip in the list
					while(True) :
						newActivatedClipID = random.choice(list(selfieStills_clip_sizes))
						newActivatedClipSize, already_used = selfieStills_clip_sizes[newActivatedClipID]
						if(not already_used) :
							selfieStills_clip_sizes[newActivatedClipID] = [newActivatedClipSize, True]
							break

					# replaces the data of the former clip with the one of the new one
					activeClip.update_activeClip_ID(newActivatedClipID, newActivatedClipSize)

			##################################################################
			# THIRD SCENE FIRST IMAGE: REMOVES THE SHAMAN TRAVERSAL
			elif(ind_chapter == 2 and indImageInput ==  val_init["imageInputIndex"]) :
				active_performing_clips.pop()


			##################################################################
			# 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################
			initial_transformed_frame_out = 1
			final_transformed_frame_out = 1
			indActiveClip = 1
			# if TRACE_ONLY:
			# 	print( "Nb active clips ", len(active_performing_clips))
			
			# loop on the active clips to apply a transformation
			for clip_index in range(len(active_performing_clips)) :
				countActiveClip = "%03d" % indActiveClip

				#######################################################
				# collecting current clip parameters
				clip_name = ""

				transformation = "COPY"
				# INITIAL TRANSFORMATION management
				if "transfIni_ID" in val_init:
						transformation = val_init["transfIni_ID"]
				else :
					print("SVG_production_movie scenario file "+scenario_file_name+" missing val_init variable")
					return 0

				#######################################################
				# variables for the transformation scenario file
				clip_dir_name = val_init["svg_input_directory"]

				# current clip data
				activeClip = active_performing_clips[clip_index]
				clip_name = activeClip.clip_name 
				clip_size = activeClip.clip_size 
				clip_first_frame_no = activeClip.clip_first_frame_no 
				clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
				clip_fill_color_l1 = activeClip.clip_fill_color_l1
				clip_stroke_color_l1 = activeClip.clip_stroke_color_l1 
				clip_fill_opacity = activeClip.clip_fill_opacity 
				clip_stroke_opacity_l1 = activeClip.clip_stroke_opacity_l1 
				clip_stroke_width = activeClip.clip_stroke_width
				clip_scaling_center_x = activeClip.clip_scaling_center_x 
				clip_scaling_center_y = activeClip.clip_scaling_center_y 
				clip_x_translation_ini = activeClip.clip_x_translation_ini 
				clip_y_translation_ini = activeClip.clip_y_translation_ini

				# clip directory
				clip_dir_name = clip_dir_name  + clip_name + "/"

				# modulo is used to loop the clip
				clip_nb_frames_of_life = current_crossfaded_frame_out - clip_first_frame_no
				initial_frame_in = (clip_nb_frames_of_life + 1) % clip_size + 1

				# with decreasing size (kind of perspective size of an object moving away at a constant speed)
				percent_life = 0
				if(ind_chapter <= 2) :
					percent_life_until_web = 0
					if clip_nb_frames_of_life >= VV_NB_FRAMES_BEFORE_CAPTURE and (scene_imageInputIndex_Final[2] - clip_first_frame_no) != 0 :
						percent_life_until_web = (clip_nb_frames_of_life - VV_NB_FRAMES_BEFORE_CAPTURE)/(scene_imageInputIndex_Final[2] - clip_first_frame_no - VV_NB_FRAMES_BEFORE_CAPTURE)
					distance = percent_life_until_web * 15
					if(distance > 0) :
						clip_scaling_factor = min(1.0, 1 / distance)
					else :
						clip_scaling_factor = 1.0
					percent_life = percent_life_until_web
					# the shamane is not scaled
					if(ind_chapter == 1 and clip_index == len(active_performing_clips) - 1) :
						clip_scaling_factor = 1.0
				elif ind_chapter == 3 :
					if current_crossfaded_frame_out >= (scene_imageInputIndex_Final[2] - clip_first_frame_no) :
						percent_life_until_web_end = (current_crossfaded_frame_out - scene_imageInputIndex_Initial[2])/((scene_imageInputIndex_Final[2] - clip_first_frame_no) - scene_imageInputIndex_Initial[2])
					else :
						percent_life_until_web_end = 1.
					# nothing is done for clip_scaling_factor, the last value from chapter 2 is preserved
					percent_life = percent_life_until_web_end

				# clip translation: initially the random translation chosen when the clip is activated
				clip_x_translation = activeClip.clip_x_translation_ini
				clip_y_translation = activeClip.clip_y_translation_ini
				# modified in chapters 2 and 3 due to web positions

				# TMP: the shaman clip is speeded up for the short movie otherwise, it will not be visible
				if(ind_chapter == 1 and clip_name == chamane_clip_sizes[0]) :
					initial_frame_in = 5 * (clip_nb_frames_of_life + 1) % clip_size + 1

				current_fill_opacity = clip_fill_opacity

				# dancing selfies
				if(ind_chapter == 0) :
					#####################################################################
					# life of a character in the first chapter
					# and evolution of its characteristics during its life
					# with varying color: transparent with a rim at the beginning
					# white opaque with a black rim during the flash
					# semi-transparent after the flash and getting more opaque afterwards
					if (clip_nb_frames_of_life) < VV_NB_FRAMES_BEFORE_CAPTURE - 5 :
						# FILL COLOR
						clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
						clip_fill_color_l1 = "#FFFFFF"
						clip_fill_color_l2 = '#000000'
						clip_fill_color_l3 = '#000000'
						clip_fill_color_l4 = '#000000'
						clip_fill_color_l5 = '#000000'
						clip_fill_color_l6 = '#000000'
						clip_fill_color_l7 = '#000000'
						clip_fill_color_l8 = '#000000'
						# FILL OPACITY
						current_fill_opacity = 0
					elif (clip_nb_frames_of_life) < VV_NB_FRAMES_BEFORE_CAPTURE :
						# FILL COLOR
						clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
						clip_fill_color_l1 = "#FFFFFF"
						clip_fill_color_l2 = '#000000'
						clip_fill_color_l3 = '#000000'
						clip_fill_color_l4 = '#000000'
						clip_fill_color_l5 = '#000000'
						clip_fill_color_l6 = '#000000'
						clip_fill_color_l7 = '#000000'
						clip_fill_color_l8 = '#000000'
						# FILL OPACITY
						current_fill_opacity = 1
					else :
						# FILL COLOR
						# additional saturation from scenario
						percent_fill_color_saturation = val_interp["percent_fill_color_saturation"]
						clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
						clip_fill_color_l1 = saturate_color(clip_fill_color_l1, percent_fill_color_saturation)
						clip_fill_color_l2 = '#000000'
						clip_fill_color_l3 = '#000000'
						clip_fill_color_l4 = '#000000'
						clip_fill_color_l5 = '#000000'
						clip_fill_color_l6 = '#000000'
						clip_fill_color_l7 = '#000000'
						clip_fill_color_l8 = '#000000'
						# FILL OPACITY
						if scene_nb_frames != 0 :
							life_as_percent_of_scene = (clip_nb_frames_of_life - VV_NB_FRAMES_BEFORE_CAPTURE + 1)/scene_nb_frames
						# autonomous progressive opacification
						current_fill_opacity = min(0.3+(life_as_percent_of_scene * 2), 1)
						# reaches progressively the opacification from scenario at the end of the first chapter
						current_fill_opacity = current_fill_opacity * (1 - scene_percentage) + scene_percentage * fill_opacity

					# with varying noise: no noise until the flash
					# noise starts after the flash and getting more active afterwards
					if (clip_nb_frames_of_life) <= VV_NB_FRAMES_BEFORE_CAPTURE :
						clip_percent_transf3 = 0
					else :
						# and then increases until it reaches the interpolated value from scenario at the end of the chapter
						clip_percent_transf3 = clip_percent_transf3 * (1 - scene_percentage) + scene_percentage * percent_transf3

					# stroke width tuned with scaling
					current_stroke_width_l1 = clip_stroke_width * (clip_scaling_factor * 1.3)

					#####################################################################
					# stores the values back for the last frame so that they can be preserved unchanged in the next chapter
					if(indImageInput == val_fin["imageInputIndex"]) :
						active_performing_clips[clip_index].update_activeClip_data(clip_name, clip_size, clip_first_frame_no, clip_fill_color_l1, \
							clip_stroke_color_l1, current_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
							clip_scaling_center_x, clip_scaling_center_y, activeClip.clip_x_translation_ini, activeClip.clip_y_translation_ini)

				# shamane traversal
				elif(ind_chapter == 1) :
					# clip_percent_transf3 has the interpolated value from scenario as value
					clip_percent_transf3 = percent_transf3

					# clip_fill_opacity has the interpolated value from scenario as value
					# except for the last layer (the shaman)
					if(clip_index != len(active_performing_clips) - 1) :
						current_fill_opacity = fill_opacity
					else :
						current_fill_opacity = clip_fill_opacity

					# no noise for the chamane
					if(clip_name == chamane_clip_sizes[0]) :
						clip_percent_transf3 = 0

					# fill color reaches progressively the interpolated value from scenario at the end of the chapter
					clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
					percent_fill_color_saturation = val_interp["percent_fill_color_saturation"]
					clip_fill_color_l1_layer_1 = saturate_color(clip_fill_color_l1, percent_fill_color_saturation)
					clip_fill_color_l1 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l1, 'l', scene_percentage)
					clip_fill_color_l2 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l2, 'l', scene_percentage)
					clip_fill_color_l3 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l3, 'l', scene_percentage)
					clip_fill_color_l4 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l4, 'l', scene_percentage)
					clip_fill_color_l5 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l5, 'l', scene_percentage)
					clip_fill_color_l6 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l6, 'l', scene_percentage)
					clip_fill_color_l7 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l7, 'l', scene_percentage)
					clip_fill_color_l8 = interpolate('hexcolor', clip_fill_color_l1_layer_1, fill_color_l8, 'l', scene_percentage)

					# stroke color reaches progressively the interpolated value from scenario at the end of the chapter
					# except for the last layer (the shaman)
					if(clip_index != len(active_performing_clips) - 1) :
						clip_stroke_color_l1 = interpolate('hexcolor', clip_stroke_color_l1, stroke_color, 'l', scene_percentage)

					# stroke width tuned with scaling
					current_stroke_width_l1 = clip_stroke_width * (clip_scaling_factor * 1.3)

					#####################################################################
					# stores the values back for the last frame so that they can be preserved unchanged in the next chapter
					if(indImageInput == val_fin["imageInputIndex"]) :
						active_performing_clips[clip_index].update_activeClip_data(clip_name, clip_size, clip_first_frame_no, clip_fill_color_l1, \
							clip_stroke_color_l1, clip_fill_opacity, clip_stroke_opacity_l1, clip_stroke_width, \
							clip_scaling_center_x, clip_scaling_center_y, activeClip.clip_x_translation_ini, activeClip.clip_y_translation_ini)

				# the shaman traversal has finished and the stills reach their starting position and a scale of 0.1
				elif(ind_chapter == 2) :
					# clip_fill_opacity has the interpolated value from scenario as value
					current_fill_opacity = fill_opacity
					# stroke width tuned with scaling
					current_stroke_width_l1 = clip_stroke_width * (clip_scaling_factor * 1.3)
					# clip_percent_transf3 has the interpolated value from scenario as value
					clip_percent_transf3 = percent_transf3
					# fill color has the interpolated value from scenario as value
					clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
					clip_fill_color_l1 = fill_color_l1
					clip_fill_color_l2 = fill_color_l2
					clip_fill_color_l3 = fill_color_l3
					clip_fill_color_l4 = fill_color_l4
					clip_fill_color_l5 = fill_color_l5
					clip_fill_color_l6 = fill_color_l6
					clip_fill_color_l7 = fill_color_l7
					clip_fill_color_l8 = fill_color_l8
					# fill color has the interpolated value from scenario as value
					clip_stroke_color_l1 = stroke_color

					######################################################################
					# progressively places the stills in the starting position on the spider web (from half to end of scene)
					# except the last one, the shamane
					if(scene_percentage >= 0.5) :
						if clip_index < len(active_performing_clips) :
							activeClip = active_performing_clips[clip_index]
							translation_percentage = (scene_percentage - 0.5) / 0.5
							clip_x_translation_ini = activeClip.clip_x_translation_ini 
							clip_y_translation_ini = activeClip.clip_y_translation_ini
							# print("clip_index", clip_index)
							# print("selfie_stills_indices_on_web", selfie_stills_indices_on_web)
							column_x_translation = selfie_stills_indices_on_web[clip_index] * 3 + 1 # first column is the time
							clip_x_translation_fin = force_num(line_web_positions[column_x_translation]) * VV_SVG_PAGE_WIDTH - (VV_SVG_PAGE_WIDTH/2)
							clip_y_translation_fin = force_num(line_web_positions[column_x_translation + 1]) * VV_SVG_PAGE_HEIGHT
							clip_x_translation = (1 - translation_percentage) * clip_x_translation_ini + translation_percentage * clip_x_translation_fin
							clip_y_translation = (1 - translation_percentage) * clip_y_translation_ini + translation_percentage * clip_y_translation_fin

				# web spider animation
				elif(ind_chapter == 3) :
					# clip_fill_opacity has the interpolated value from scenario as value
					current_fill_opacity = fill_opacity
					# stroke width tuned with scaling
					current_stroke_width_l1 = clip_stroke_width * (clip_scaling_factor * 1.3)
					# clip_percent_transf3 has the interpolated value from scenario as value
					clip_percent_transf3 = percent_transf3
					# fill color has the interpolated value from scenario as value
					clip_fill_color_bg = fill_color_bg # interpolated value from the scenario
					clip_fill_color_l1 = fill_color_l1
					clip_fill_color_l2 = fill_color_l2
					clip_fill_color_l3 = fill_color_l3
					clip_fill_color_l4 = fill_color_l4
					clip_fill_color_l5 = fill_color_l5
					clip_fill_color_l6 = fill_color_l6
					clip_fill_color_l7 = fill_color_l7
					clip_fill_color_l8 = fill_color_l8
					# fill color has the interpolated value from scenario as value
					clip_stroke_color_l1 = stroke_color
					# the translations are given by the spider web animation logs
					# count frames from the beginning of chapter 3
					nb_frames_from_chapter_start = indImageInput - val_init["imageInputIndex"]
					nb_seconds_from_chapter_start = float(nb_frames_from_chapter_start) / 30.
					while(len(line_web_positions) == len(header_web_positions) and force_num(line_web_positions[0]) < nb_seconds_from_chapter_start) :
						#reads the first row
						line_web_positions =  next(read_web_positions_CSV) 
					column_x_translation = selfie_stills_indices_on_web[clip_index] * 3 + 1
					clip_x_translation = force_num(line_web_positions[column_x_translation]) * VV_SVG_PAGE_WIDTH - (VV_SVG_PAGE_WIDTH/2)
					clip_y_translation = force_num(line_web_positions[column_x_translation + 1]) * VV_SVG_PAGE_HEIGHT

				# clip_stroke_color_links has the interpolated value from scenario as value
				clip_stroke_color_links = stroke_color_links

				final_frame_in = initial_frame_in
				transformed_clip_name = val_init["svg_input_file_name"]+"-"+countActiveClip

				# increases the number of layers for the second half of the first chapter when the 
				# dancing silhouettes are replaced by the stills
				if(ind_chapter == 1 and indImageInput == int(val_init["imageInputIndex"] + (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) / 2) ) :
					nb_layers = 8

				#######################################################
				# building the transformation scenario file for each added frame
				transformation_scenario_file_name = tmp_dir_name + "LYM_Arachnid_scenario_transformations-single_clip_"+str(clip_index)+".csv"

				# print( "Transformation scenario file ", transformation_scenario_file_name,"\n" )
				try:
					FILEout = open(transformation_scenario_file_name, "wt")
				except IOError:
					print("File not created :", transformation_scenario_file_name, " or path is incorrect")

				transformation_scenario_string = ""
				# if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
				# 	print( "Input frame ", clip_dir_name + clip_name + "_" + countInput + ".svg" )
				countInput = "%04d" % initial_frame_in
				full_frame_name = clip_dir_name + clip_name + "_" + countInput + ".svg"
				if not os.path.exists(full_frame_name):
					print("Can't open frame ", full_frame_name, " from clip " ,  clip_name ,  " at frame no " ,  initial_frame_in)
					raise NameError("File not found")
				
				scenario_variables =  { \
					'svg_input_directory' : ['string', clip_dir_name, clip_dir_name, 's'] , \
					'svg_input_file_name' : ['string', clip_name, clip_name, 's'] , \
					'svg_output_directory' : ['string', transformed_clip_dir, transformed_clip_dir, 's'] , \
					'svg_output_file_name' : ['string', transformed_clip_name, transformed_clip_name, 's'] , \
					'tmp_directory' : ['string', tmp_dir_name, tmp_dir_name, 's'] , \
					'imageInputIndex' : ['int', str(initial_frame_in), str(final_frame_in), 's'] , \
					'imageOutputIndex' : ['int', str(initial_transformed_frame_out), str(final_transformed_frame_out), 's'] , \
					'nb_posterization_layers' : ['int', nb_layers, nb_layers, 's'] , \
					'transf1_ID' : ['string', transformation, transformation, 's'] , \
					'percent_transf3' : ['float', str(clip_percent_transf3), str(clip_percent_transf3), 'l'] , \
					'percent_links1' : ['float', str(percent_links), str(percent_links), 'l'] , \
					'stroke_color_links1' : ['hexcolor', str(clip_stroke_color_links), str(clip_stroke_color_links), 'l'] , \
					# number of frames of existence for a single layer
					'nb_frames_transf1' : ['int', str(clip_nb_frames_of_life), str(clip_nb_frames_of_life), 'l'] , \
					# percentage of its life before the web or in the web
					'percent_life_transf1' : ['float', str(percent_life), str(percent_life), 'l'] , \
					'fill_color_bg' : ['hexcolor', clip_fill_color_bg, clip_fill_color_bg, 'l'] , \
					'fill_color_l1' : ['hexcolor', clip_fill_color_l1, clip_fill_color_l1, 'l'] , \
					'fill_color_l2' : ['hexcolor', clip_fill_color_l2, clip_fill_color_l2, 'l'] , \
					'fill_color_l3' : ['hexcolor', clip_fill_color_l3, clip_fill_color_l3, 'l'] , \
					'fill_color_l4' : ['hexcolor', clip_fill_color_l4, clip_fill_color_l4, 'l'] , \
					'fill_color_l5' : ['hexcolor', clip_fill_color_l5, clip_fill_color_l5, 'l'] , \
					'fill_color_l6' : ['hexcolor', clip_fill_color_l6, clip_fill_color_l6, 'l'] , \
					'fill_color_l7' : ['hexcolor', clip_fill_color_l7, clip_fill_color_l7, 'l'] , \
					'fill_color_l8' : ['hexcolor', clip_fill_color_l8, clip_fill_color_l8, 'l'] , \
					'fill_opacity_l1' : ['float', str(current_fill_opacity), str(current_fill_opacity), 'l'] , \
					'fill_opacity_l2' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l3' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l4' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l5' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l6' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l7' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l8' : ['float', 1, 1, 'l'] , \
					'stroke_color_l1' : ['hexcolor', str(clip_stroke_color_l1), str(clip_stroke_color_l1), 'l'] , \
					'stroke_opacity_l1' : ['float',  str(clip_stroke_opacity_l1), str(clip_stroke_opacity_l1), 'l'] , \
					'stroke_width_l1' : ['float', str(current_stroke_width_l1), str(current_stroke_width_l1), 'l'] }
				# SCALE and TRANSLATION in scenes 1 to 3
				if(ind_chapter <= 2) :
					scenario_variables['x_scaling_center1'] = ['float', str(clip_scaling_center_x), str(clip_scaling_center_x), 'l']
					scenario_variables['y_scaling_center1'] = ['float', str(clip_scaling_center_y), str(clip_scaling_center_y), 'l']
					scenario_variables['scaling_factor1'] = ['float', str(clip_scaling_factor), str(clip_scaling_factor), 'l']
					scenario_variables['x_translation2'] = ['float', str(clip_x_translation), str(clip_x_translation), 'l']
					scenario_variables['y_translation2'] = ['float', str(clip_y_translation), str(clip_y_translation), 'l']
				else :
					# direct transmission of variables in the scenario of the movie to the scenario of the transformation
					# SCALE_TO_CIRCLE transformation in scene 4
					if ind_chapter == 3 and "x_scaling_center1" in val_interp and "y_scaling_center1" in val_interp and "scaling_factor1" in val_interp :
						var_name = "x_scaling_center1"
						scenario_variables[var_name] = [var_types[var_name], val_interp[var_name], val_interp[var_name], 'l'] 
						var_name = "y_scaling_center1"
						scenario_variables[var_name] = [var_types[var_name], val_interp[var_name], val_interp[var_name], 'l'] 
						var_name = "scaling_factor1"
						scenario_variables[var_name] = [var_types[var_name], val_interp[var_name], val_interp[var_name], 'l'] 
					# SCALE and TRANSLATION in scene 4
					if ind_chapter == 3 :
						scenario_variables['x_scaling_center2'] = ['float', str(clip_scaling_center_x), str(clip_scaling_center_x), 'l']
						scenario_variables['y_scaling_center2'] = ['float', str(clip_scaling_center_y), str(clip_scaling_center_y), 'l']
						scenario_variables['scaling_factor2'] = ['float', str(clip_scaling_factor), str(clip_scaling_factor), 'l']
						scenario_variables['x_translation3'] = ['float', str(clip_x_translation), str(clip_x_translation), 'l']
						scenario_variables['y_translation3'] = ['float', str(clip_y_translation), str(clip_y_translation), 'l']

				transformation_scenario_string_list = variable_dict_to_CSV_file(scenario_variables)
				
				#######################################################
				# applying the transformation with the current scenario file for each added frame
				for local_row in transformation_scenario_string_list:
					FILEout.write( local_row+"\n" )
					# print(local_row)
				FILEout.close()

				if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/cmd/transform-movie.sh\n" )
					if vv_scenario_transform.main(["-i", \
						transformation_scenario_file_name, "--chapter_no", ind_chapter, "--with_fill_and_stroke", "True"]) == 0:
						return 0
					
				indActiveClip += 1
				# end of loop on the active clips to apply a transformation

			##################################################################
			# 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################

			#######################################################
			# POSSIBLE FLASH
			# duplicates the last clip image so that there is (1) a flash halo and (2) on top of it the normal image
			with_Flash = False
			# blurred red rim
			if (ind_chapter == 0 and clip_nb_frames_of_life >= (VV_NB_FRAMES_BEFORE_CAPTURE - 5) and clip_nb_frames_of_life < VV_NB_FRAMES_BEFORE_CAPTURE):
				with_Flash = True

				# the image at rank (len(active_performing_clips) - 1) is copied to image at rank len(active_performing_clips)
				# rank of the output file (1)
				countOutput_flash = "%04d" % 1
				# name of the top transformed file
				top_transformed_file_name_and_path = transformed_clip_dir + transformed_clip_name + "_"+countOutput_flash+".svg"

				# name of the duplicated image (the image added on top of the top image)
				count_new_top_clip = "%03d" % (len(active_performing_clips) + 1)
				new_top_clip_name = val_init["svg_input_file_name"]+"-"+count_new_top_clip
				new_top_clip_name_and_path = transformed_clip_dir + new_top_clip_name + "_"+countOutput_flash+".svg"
				if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					os.system("cp " + top_transformed_file_name_and_path + " " + new_top_clip_name_and_path)

				# the image at rank (len(active_performing_clips) - 1) is copied to tmpdir before being transformed from tmpdir back to it
				# name of the tmp top transformed file
				top_transformed_file_name_tmp = tmp_dir_name + transformed_clip_name + "_"+countOutput_flash+".svg"
				if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					os.system("cp " + top_transformed_file_name_and_path + " " + top_transformed_file_name_tmp)


				# the image at rank (len(active_performing_clips)) is filled with red, 
				# with 0 stroke and blur according to before flash rank (because crossfading reverses file order)
				blur_filters = [50, 75, 150, 300, 600]
				blur_index = max(min(clip_nb_frames_of_life - (VV_NB_FRAMES_BEFORE_CAPTURE - 5), len(blur_filters)),0)
				if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					vv_blur_and_fill(top_transformed_file_name_tmp, new_top_clip_name_and_path, blur_filters[blur_index], '#FFOOOO')

			# multicolored still image
			elif (ind_chapter == 0 and clip_nb_frames_of_life == VV_NB_FRAMES_BEFORE_CAPTURE):
				with_Flash = False
				nb_layers = 8

				# the image at rank (len(active_performing_clips) - 1) read to collect its translation and is then replaced
				# by the still images
				# rank of the output file (1)
				countOutput_flash = "%04d" % 1
				# name of the top transformed file
				top_transformed_clip_name_and_path = transformed_clip_dir + transformed_clip_name + "_"+countOutput_flash+".svg"

				# the multicolored still image is copied to image at rank len(active_performing_clips) - 1
				# rank of the still (1)
				countOutput_flash = "%04d" % 1
				# name of the multicolored still image file
				still_clip_name = val_init["svg_input_file_name"]+"_still" + clip_name[len(clip_name) - 4:] 
				still_file_name = val_init["svg_input_directory"] + clip_name[:len(clip_name) - 4] + "_still" + clip_name[len(clip_name) - 4:] + "/" + still_clip_name + "_"+countOutput_flash+".svg"
				
				# the still image is copied at rank len(active_performing_clips) and scaled according to image at rank (len(active_performing_clips) - 1)
				if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					vv_translate_top_layer_as_lower_one(top_transformed_clip_name_and_path, still_file_name)

				# recuperer la translation du fichier de l'avant derniere couche (le fichier du dessus de pile et dernier de la liste)
				# vv-translation="6085.332129993109x0"
				# et le substituer dans toutes les couches du fichier still
				# transform="translate(608.50,
				# à la place de transform="translate(0.000000,
				# (le dizième est choisi car il y a un changement d'échelle de 0.1 dajà fait)

			#######################################################
			# buiding the crossfading scenario file with the list of added frames
			crossfaded_clip_dir = tmp_dir_name
			crossfaded_clip_name = "crossfaded-" + val_init["svg_input_file_name"]

			crossfading_scenario_file_name = tmp_dir_name + "/LYM_Arachnid_scenario_layer_crossfading-movie.csv"
			# print( "Crossfading scenario file ", crossfading_scenario_file_name,"\n" )
			try:
				FILEout = open(crossfading_scenario_file_name, "wt")
			except IOError:
				print("File not created :", crossfading_scenario_file_name, " or path is incorrect")

			# additional blur layer in case of flash
			nb_active_clips = len(active_performing_clips)
			if with_Flash :
				nb_active_clips += 1

			scenario_variables =  { \
				'svg_output_directory' : ['string', crossfaded_clip_dir, crossfaded_clip_dir, 's'] , \
				'svg_output_file_name' : ['string', crossfaded_clip_name, crossfaded_clip_name, 's'] , \
				'composition_mode' : ['string', 'PARALLEL', 'PARALLEL', 's'] , \
				'nb_posterization_layers' : ['int', nb_layers, nb_layers, 's'] , \
				'nb_composed_files' : ['int', str(nb_active_clips), str(nb_active_clips), 's'] , \
				'imageOutputIndex' : ['int', 1, 1, 's']  }

			crossfading_scenario_string_list = variable_dict_to_CSV_file(scenario_variables)

			#######################################################
			# addition of the columns of all the frames
			# loop on the active clips to add them to the scenario file
			# the first clips come last
			for indActiveClip in range(nb_active_clips) :
				countActiveClip = "%03d" % (indActiveClip + 1)
				# print("Xfaded clip:", val_init["svg_input_file_name"]+"-"+countActiveClip)

				additional_scenario_variables =  { \
					'svg_input_directory_'+countActiveClip : ['string', transformed_clip_dir, transformed_clip_dir, 's'] , \
					'svg_input_file_name_'+countActiveClip : ['string', val_init["svg_input_file_name"]+"-"+countActiveClip, val_init["svg_input_file_name"]+"-"+countActiveClip, 's'] , \
					'imageInputIndex_'+countActiveClip : ['int', 1, 1, 's'] }

				crossfading_scenario_string_list = variable_dict_append_CSV_file(additional_scenario_variables, crossfading_scenario_string_list)

			# loop on the active clips to add them to the scenario file

			for local_row in crossfading_scenario_string_list:
				FILEout.write( local_row+"\n" )

			FILEout.close()

			#######################################################
			# crossfading the transformed active clips
			if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
					# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/cmd/crossfade-movie.sh\n" )
				if vv_scenario_layer_crossfade.main(["-i", crossfading_scenario_file_name]) == 0:
					return 0

			##################################################################
			# 5: FINAL TRANSFORMATIONS OF THE CROSSFADED FRAMES
			##################################################################

			end_transformation = val_init["end_transformation"]

			# lowercase variable: initial thread - UPPERCASE VARIABLE: FINAL THREAD
			clip_dir_name = tmp_dir_name
			clip_name = "crossfaded-"+val_init["svg_input_file_name"]

			end_transformed_clip_dir = val_init["svg_output_directory"]
			end_transformed_clip_name = val_init["svg_output_file_name"]

			initial_frame_in = 1
			final_frame_in = 1
			initial_end_transform_frame_out = current_crossfaded_frame_out
			final_end_transform_frame_out = current_crossfaded_frame_out

			#######################################################
			# building the end_transformation scenario file for the cross-faded frame
			end_transformation_scenario_file_name = tmp_dir_name + "LYM_Arachnid_scenario_end_transformations-movie.csv"

			# print( "Transformation scenario file ", end_transformation_scenario_file_name,"\n" )
			try:
				FILEout = open(end_transformation_scenario_file_name, "wt")
			except IOError:
				print("File not created :", end_transformation_scenario_file_name, " or path is incorrect")

			scenario_variables =  { \
				'svg_input_directory' : ['string', clip_dir_name, clip_dir_name, 's'] , \
				'svg_input_file_name' : ['string', clip_name, clip_name, 's'] , \
				'svg_output_directory' : ['string', end_transformed_clip_dir, end_transformed_clip_dir, 's'] , \
				'svg_output_file_name' : ['string', end_transformed_clip_name, end_transformed_clip_name, 's'] , \
				'tmp_directory' : ['string', tmp_dir_name, tmp_dir_name, 's'] , \
				'imageInputIndex' : ['int', str(initial_frame_in), str(final_frame_in), 's'] , \
				'imageOutputIndex' : ['int', str(initial_end_transform_frame_out), str(final_end_transform_frame_out), 's'] , \
				'nb_posterization_layers' : ['int', nb_layers, nb_layers, 's'] , \
				'transf1_ID' : ['string', end_transformation, end_transformation, 's'] , \
				# global number of links (individual percentages are managed in chapter 1 through percent_life_transf1)
				'percent_links1' : ['float', str(percent_links), str(percent_links), 'l'] , \
				'stroke_color_links1' : ['hexcolor', str(clip_stroke_color_links), str(clip_stroke_color_links), 'l'] , \
				# next 2 used only in first pass in layer by layer transformation
				'nb_frames_transf1' : ['int', str(indImageInput), str(indImageInput), 'l'], \
				'percent_life_transf1' : ['float', str(0), str(1), 'l'] }

			if(ind_chapter <= 3) :
				# the noise is also appplied to link distorsion
				scenario_variables['percent_transf1'] = ['float', str(clip_percent_transf3), str(clip_percent_transf3), 'l']
			else :
				# no noise in chapter 4
				scenario_variables['percent_transf1'] = ['float', str(0.), str(0.), 'l']

			end_transformation_scenario_string_list = variable_dict_to_CSV_file(scenario_variables)
			
			#######################################################
			# applying the end_transformation with the current scenario file for each added frame
			for local_row in end_transformation_scenario_string_list:
				FILEout.write( local_row+"\n" )
			
			FILEout.close()

			count = "%04d" % initial_end_transform_frame_out
			if not TRACE_ONLY and current_crossfaded_frame_out % output_step == output_ini:
				# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/cmd/transform-movie.sh\n" )
				print( "Output SVG file ", end_transformed_clip_dir+end_transformed_clip_name+"_"+count+".svg","\n" )
				if vv_scenario_transform.main(["-i", \
					end_transformation_scenario_file_name, "--chapter_no", ind_chapter, "--with_fill_and_stroke", "False"]) == 0:
					return 0

				# for a test without final transformation
				if( False) :
					print("Without final transformation: cp "+clip_dir_name+clip_name+"_0001.svg "+end_transformed_clip_dir+end_transformed_clip_name+"_"+count+".svg")
					os.system("cp "+clip_dir_name+clip_name+"_0001.svg "+end_transformed_clip_dir+end_transformed_clip_name+"_"+count+".svg")

			current_crossfaded_frame_out += 1
			# end of SCANS ALL THE FRAMES IN THE MOVIE

			# if(current_crossfaded_frame_out >= 152) :
			# 	return

		# /scene row
		prod_scenario_row =  next(scenarioCSV)
		if prod_scenario_row[0] != "/scene":
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
	# chapters
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
