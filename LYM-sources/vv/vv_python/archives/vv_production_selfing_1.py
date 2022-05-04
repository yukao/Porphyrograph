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

import vv_scenario_transform
import vv_scenario_layer_crossfade
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import interpolate
from vv_lib import variable_dict_to_CVS_file
from vv_lib import variable_dict_append_CVS_file

from vv_const import *

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_production_weaving-2.py -i scenario_file
	ARGUMENTS:
	-i: input scenariofile
'''

##################################################################
# HASHES FOR CLIP management
##################################################################

# LINUX COMMAND to retrieve the number of files in subdirectories
# find . -type f | awk '{dir=gensub(/(.+\/).+/,"\\1","g",$0); dir_list[dir]++} END {for (d in dir_list) printf "%s %s\n",d,dir_list[d]}' | sort
selfies_clip_sizes = { \
	"Cunningham_Looks_000" : [118, False], \
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
	"Cunningham_Looks_069" : [470, False] }

rebirth_clip_sizes = { \
	"Annika_Rebirth_0001" : [1, False], \
	"Annika_Rebirth_0002" : [1, False], \
	"Annika_Rebirth_0003" : [1, False], \
	"Annika_Rebirth_0004" : [1, False], \
	"Annika_Rebirth_0005" : [1, False], \
	"Annika_Rebirth_0006" : [1, False], \
	"Annika_Rebirth_0007" : [1, False], \
	"Annika_Rebirth_0008" : [1, False], \
	"Annika_Rebirth_0009" : [1, False], \
	"Annika_Rebirth_0010" : [1, False], \
	"Annika_Rebirth_0011" : [1, False], \
	"Annika_Rebirth_0012" : [1, False], \
	"Annika_Rebirth_0013" : [1, False], \
	"Annika_Rebirth_0014" : [1, False], \
	"Annika_Rebirth_0015" : [1, False], \
	"Annika_Rebirth_0016" : [1, False], \
	"Annika_Rebirth_0017" : [1, False], \
	"Annika_Rebirth_0018" : [1, False], \
	"Annika_Rebirth_0019" : [1, False], \
	"Annika_Rebirth_0020" : [1, False], \
	"Annika_Rebirth_0021" : [1, False], \
	"Annika_Rebirth_0022" : [1, False], \
	"Annika_Rebirth_0023" : [1, False], \
	"Annika_Rebirth_0024" : [1, False], \
	"Annika_Rebirth_0025" : [1, False], \
	"Annika_Rebirth_0026" : [1, False], \
	"Annika_Rebirth_0027" : [1, False], \
	"Annika_Rebirth_0028" : [1, False], \
	"Annika_Rebirth_0029" : [1, False], \
	"Annika_Rebirth_0030" : [1, False], \
	"Annika_Rebirth_0031" : [1, False], \
	"Annika_Rebirth_0032" : [1, False], \
	"Annika_Rebirth_0033" : [1, False] }

chamane_clip_sizes = ["Annika_silhouette_000", 3213]

class ActiveClip:
	def __init__(self, clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
		clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
		clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation):
		update_activeClip_data(self, clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
		clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
		clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation)

	def update_activeClip_data(self, clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
		clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
		clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation):
		self.clip_name = clip_name 
		self.clip_size = clip_size 
		self.clip_first_frame_no = clip_first_frame_no 
		self.clip_fill_color = clip_fill_color
		self.clip_stroke_color = clip_stroke_color 
		self.clip_fill_opacity = clip_fill_opacity 
		self.clip_stroke_opacity = clip_stroke_opacity 
		self.clip_stroke_width = clip_stroke_width
		self.clip_scaling_factor = clip_scaling_factor 
		self.clip_scaling_center_x = clip_scaling_center_x 
		self.clip_scaling_center_y = clip_scaling_center_y 
		self.clip_x_translation = clip_x_translation 
		self.clip_y_translation = clip_y_translation

# dict of lists, each list contains
# clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
# 	clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
# 	clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation
# the key is the clip name
active_performing_clips = []

transformation_scenario_file_name = ""
crossfading_scenario_file_name = ""
line = ""

tmp_dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/"
transformed_clip_dir = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_transformed_SVGs/silhouette/"

# SCENARIO VARIABLES
var_names = []
var_types = dict()
val_init = dict()
val_fin = dict()
val_interp = dict()
interp = dict()


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

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
		print("File not found :", scenario_file_name, " or path is incorrect")

	##################################################################
	# RANDOM SEEDING
	##################################################################
	random.seed(VV_RANDOM_SEED)

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCVS = csv.reader(FILEin, delimiter=',')
	line =  next(readCVS) 
	# print( "line1 ", line  )
	line =  next(readCVS) 
	# print( "line2 ", line  )

	#3rd line: variable types
	line_types =  next(readCVS)

	#4th line: variable names
	var_names =  next(readCVS) 
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
	values_line =  next(readCVS)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found in line: ", values_line)
		return 0
	
	nb_chapters = int(values_line[1])
	# print( "nb chapters ", nb_chapters)

	##################################################################
	# READING AND EXECUTING A CHAPTER
	##################################################################
	for ind_chapter in range(nb_chapters) :
		# scene line
		values_line =  next(readCVS)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =  next(readCVS)
		# var IDs line
		line =  next(readCVS)
	
		# variable initial value
		values_line =  next(readCVS)
		values_line.pop(0)
		indVar = 0
		print(values_line)
		for value_init in values_line:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCVS)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =  next(readCVS)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp    
			indVar += 1

		pas = 1
		if val_init["imageInputIndex"] < val_fin["imageInputIndex"]:
			pas = 1
		elif val_init["imageInputIndex"] > val_fin["imageInputIndex"]:
			pas = -1
		else :
			pas = 0
		
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
		for indImageInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"], pas):
			countInput = "%04d" % indImageInput
			countOutput = "%04d" % current_crossfaded_frame_out

			scene_percentage = 0
			scene_nb_frames = val_fin["imageInputIndex"] - val_init["imageInputIndex"]
			if pas != 0 and (scene_nb_frames) != 0 :
				scene_percentage = (indImageInput - val_init["imageInputIndex"])/scene_nb_frames
			
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
			percent_transf = val_interp["percent_transf"]

			# LINK PERCENTAGE management
			percent_links = val_interp["percent_links"]

			# LINK COLOR management
			stroke_color_links = val_interp["stroke_color_links"]

			# FILL COLOR management (chapters 2 and 3)
			fill_color_l1 = val_interp["fill_color_l1"]

			# STROKE COLOR management (chapters 2 and 3)
			stroke_color = val_interp["stroke_color"]

			# STROKE WIDTH management
			stroke_width = val_interp["stroke_width"]

			# OPACITY
			fill_opacity = val_interp["fill_opacity_l1"]

			# NUMBER OF ACTIVE CLIPS management
			nb_transformed_performing_clips = val_interp["nb_active_clips"]

			##################################################################
			# 2: management OF CLIP ACTIVATION
			##################################################################

			if(ind_chapter == 0) :
				##################################################################
				# ACTIVATING NEW PERFORMING CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
				# new clip activation
				# print( "current expected number of clips " , nb_transformed_performing_clips , "\n" )
				for indNewClipActivation in range(len(active_performing_clips), nb_transformed_performing_clips) :
					# stroke and fill parameters selection
					# random fill color
					mix_red = "%02x" % min(int(random.random() * 255), 255)
					mix_green = "%02x" %  min(int(random.random() * 255), 255)
					mix_blue = "%02x" %  min(int(random.random() * 255), 255)
					clip_fill_color = '#' + mix_red + mix_green + mix_blue
					# random translation in either halves of the images
					if( indNewClipActivation % 2 == 0 ) :
						clip_x_translation = random.random() * (VV_SVG_PAGE_WIDTH / 2 - 200)
					else :
						clip_x_translation = - random.random() * (VV_SVG_PAGE_WIDTH / 2 - 200)
					clip_y_translation = 0

					# fixed values
					clip_stroke_opacity = 1.0

					# modified when used
					clip_stroke_color = '#000000'
					clip_fill_opacity = 1.0
					clip_scaling_factor = 1.0

					# random choice of a clip in the list
					while(True) :
						newActivatedClipID = random.choice(list(selfies_clip_sizes))
						newActivatedClipSize, already_used = selfies_clip_sizes[newActivatedClipID]
						if(not already_used) :
							selfies_clip_sizes[newActivatedClipID] = [newActivatedClipSize, True]
							break

					# new clip activation
					active_performing_clips.append(ActiveClip(newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, \
							clip_fill_color, clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, \
							stroke_width, clip_scaling_factor, VV_SVG_PAGE_WIDTH / 2, 0, clip_x_translation, clip_y_translation))

			elif(ind_chapter == 1 and indImageInput == val_init["imageInputIndex"]) :
				##################################################################
				# SECOND SCENE FIRST IMAGE: ACTIVATES THE SHAMAN TRAVERSAL
				newActivatedClipID = chamane_clip_sizes[0]
				newActivatedClipSize = chamane_clip_sizes[1]
				clip_fill_color = '#FFFFFF'
				clip_fill_opacity = 0.7
				clip_stroke_color = '#FF0000'
				clip_stroke_opacity = 1.0
				shaman_stroke_width = 2 * stroke_width
				clip_scaling_factor = 1.0

				# new clip activation, this clip is not translated to have the full traversal of the screen
				active_performing_clips.append(ActiveClip(newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, \
						clip_fill_color, clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, \
						shaman_stroke_width, clip_scaling_factor, VV_SVG_PAGE_WIDTH / 2, 0, 0, 0))

			elif(ind_chapter == 1 and indImageInput == int(val_init["imageInputIndex"] + (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) / 2) ) :
				##################################################################
				# SECOND SCENE MID IMAGE: REPLACES THE SELFIES BY THE REBIRTH IMAGES EXCEPT THE LAST ONE: THE SHAMAN
				# loop on the active clips to replace the clip ID and initial frame
				for clip_index in range(len(active_performing_clips) - 1) :
					activeCLip = active_performing_clips[clip_index]
					clip_name = activeClip.clip_name 
					clip_size = activeClip.clip_size 
					clip_first_frame_no = activeClip.clip_first_frame_no 
					clip_fill_color = activeClip.clip_fill_color
					clip_stroke_color = activeClip.clip_stroke_color 
					clip_fill_opacity = activeClip.clip_fill_opacity 
					clip_stroke_opacity = activeClip.clip_stroke_opacity 
					clip_stroke_width = activeClip.clip_stroke_width
					clip_scaling_factor = activeClip.clip_scaling_factor 
					clip_scaling_center_x = activeClip.clip_scaling_center_x 
					clip_scaling_center_y = activeClip.clip_scaling_center_y 
					clip_x_translation = activeClip.clip_x_translation 
					clip_y_translation = activeClip.clip_y_translation

					# random choice of a clip in the list
					while(True) :
						newActivatedClipID = random.choice(list(rebirth_clip_sizes))
						newActivatedClipSize, already_used = rebirth_clip_sizes[newActivatedClipID]
						if(not already_used) :
							rebirth_clip_sizes[newActivatedClipID] = [newActivatedClipSize, True]
							break

					# replaces the data of the former clip with the one of the new one
					active_performing_clips[clip_index].update_activeClip_data(newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, clip_fill_color, \
						clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
						clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation)

			elif(ind_chapter == 2 and indImageInput ==  val_init["imageInputIndex"]) :
				##################################################################
				# SECOND SCENE FIRST IMAGE: REMOVES THE SHAMAN TRAVERSAL
				active_performing_clips.pop()


			##################################################################
			# 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################
			initial_transformed_frame_out = 1
			final_transformed_frame_out = 1
			indActiveClip = 1
			if not TRACE_ONLY:
				print( "Nb active clips ", len(active_performing_clips))
			
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
				activeCLip = active_performing_clips[clip_index]
				clip_name = activeClip.clip_name 
				clip_size = activeClip.clip_size 
				clip_first_frame_no = activeClip.clip_first_frame_no 
				clip_fill_color = activeClip.clip_fill_color
				clip_stroke_color = activeClip.clip_stroke_color 
				clip_fill_opacity = activeClip.clip_fill_opacity 
				clip_stroke_opacity = activeClip.clip_stroke_opacity 
				clip_stroke_width = activeClip.clip_stroke_width
				clip_scaling_factor = activeClip.clip_scaling_factor 
				clip_scaling_center_x = activeClip.clip_scaling_center_x 
				clip_scaling_center_y = activeClip.clip_scaling_center_y 
				clip_x_translation = activeClip.clip_x_translation 
				clip_y_translation = activeClip.clip_y_translation

				# clip directory
				clip_dir_name = clip_dir_name  + clip_name + "/"

				# modulo is used to loop the clip
				nb_frames_transf1 = current_crossfaded_frame_out - clip_first_frame_no
				percent_life_transf1 = 0
				if(val_fin["imageOutputIndex"] - clip_first_frame_no) :
					percent_life_transf1 = (current_crossfaded_frame_out - clip_first_frame_no)/(val_fin["imageOutputIndex"]- clip_first_frame_no)
				initial_frame_in = (nb_frames_transf1 + 1) % clip_size + 1

				# TMP: the shaman clip is speeded up for the short movie otherwise, it will not be visible
				if(ind_chapter == 1 and clip_name == chamane_clip_sizes[0]) :
					initial_frame_in = 5 * (nb_frames_transf1 + 1) % clip_size + 1

				current_fill_opacity = clip_fill_opacity

				if(ind_chapter == 0) :
					#####################################################################
					# life of a character in the first chapter
					# and evolution of its characteristics during its life
					# with decreasing size (kind of perspective size of an object moving away at a constant speed)
					life_as_percent_of_scene = 0
					if (nb_frames_transf1) >= VV_NB_FRAMES_BEFORE_CAPTURE and scene_nb_frames != 0 :
						life_as_percent_of_scene = (nb_frames_transf1 - VV_NB_FRAMES_BEFORE_CAPTURE)/scene_nb_frames
					dist = 1 + life_as_percent_of_scene * 10
					clip_scaling_factor *= 1 / dist
					# with varying color: transparent with a rim at the beginning
					# white opaque with a black rim during the flash
					# semi-transparent after the flash and getting more opaque afterwards
					if (nb_frames_transf1) < VV_NB_FRAMES_BEFORE_CAPTURE - 5 :
						# FILL COLOR
						clip_fill_color =  "#FFFFFF"
						# FILL OPACITY
						current_fill_opacity = 0
					elif (nb_frames_transf1) < VV_NB_FRAMES_BEFORE_CAPTURE :
						# FILL COLOR
						clip_fill_color =  "#FFFFFF"
						# FILL OPACITY
						current_fill_opacity = 1
					else :
						# FILL COLOR
						# additional saturation from scenario
						percent_fill_color_saturation = val_init["percent_fill_color_saturation"]
						if(percent_fill_color_saturation > 0) :
							clip_fill_color = saturate_color(clip_fill_color, percent_fill_color_saturation)
						# FILL OPACITY
						if scene_nb_frames != 0 :
							life_as_percent_of_scene = (nb_frames_transf1 - VV_NB_FRAMES_BEFORE_CAPTURE + 1)/scene_nb_frames
						# autonomous progressive opacification
						current_fill_opacity = min(0.3+(life_as_percent_of_scene * 2), 1)
						# reaches progressively the opacification from scenario at the end of the first chapter
						current_fill_opacity = current_fill_opacity * (1 - scene_percentage) + scene_percentage * fill_opacity

					# with varying noise: no noise until the flash
					# noise starts after the flash and getting more active afterwards
					if (nb_frames_transf1) <= VV_NB_FRAMES_BEFORE_CAPTURE :
						clip_percent_transf = 0
					else :
						# and then increases until it reaches the interpolated value from scenario at the end of the chapter
						clip_percent_transf = clip_percent_transf * (1 - scene_percentage) + scene_percentage * percent_transf

					# stroke width tuned with scaling
					current_stroke_width = clip_stroke_width * clip_scaling_factor

					#####################################################################
					# stores the values back for the last frame so that they can be preserved unchanged in the next chapter
					if(indImageInput == val_fin["imageInputIndex"] - 1) :
						active_performing_clips[clip_index].update_activeClip_data(clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
							clip_stroke_color, current_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
							clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation)


				elif(ind_chapter == 1) :
					# clip_scaling_factor unchanged
					# clip_fill_color unchanged
					# clip_stroke_width unchanged
					# clip_percent_transf has the interpolated value from scenario as value
					clip_percent_transf = percent_transf

					# clip_fill_opacity has the interpolated value from scenario as value
					# except for the last layer (the shaman)
					if(clip_index != len(active_performing_clips) - 1) :
						current_fill_opacity = fill_opacity
					else :
						current_fill_opacity = clip_fill_opacity

					# no noise for the chamane
					if(clip_name == chamane_clip_sizes[0]) :
						clip_percent_transf = 0

					# fill color reaches progressively the interpolated value from scenario at the end of the chapter
					clip_fill_color = interpolate('hexcolor', clip_fill_color, fill_color_l1, 'l', scene_percentage)

					# stroke color reaches progressively the interpolated value from scenario at the end of the chapter
					# except for the last layer (the shaman)
					if(clip_index != len(active_performing_clips) - 1) :
						clip_stroke_color = interpolate('hexcolor', clip_stroke_color, stroke_color, 'l', scene_percentage)

					# stroke width tuned with scaling
					current_stroke_width = clip_stroke_width * clip_scaling_factor

					#####################################################################
					# stores the values back for the last frame so that they can be preserved unchanged in the next chapter
					if(indImageInput == val_fin["imageInputIndex"] - 1) :
						active_performing_clips[clip_index].update_activeClip_data(clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
							clip_stroke_color, clip_fill_opacity, clip_stroke_opacity, clip_stroke_width, \
							clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y, clip_x_translation, clip_y_translation)

				elif(ind_chapter == 2) :
					# clip_scaling_factor reaches 1 at the end of the chapter
					clip_scaling_factor = clip_scaling_factor * (1 - scene_percentage) + scene_percentage
					# clip_fill_opacity has the interpolated value from scenario as value
					current_fill_opacity = fill_opacity
					# stroke width tuned with scaling
					current_stroke_width = clip_stroke_width * clip_scaling_factor
					# clip_percent_transf has the interpolated value from scenario as value
					clip_percent_transf = percent_transf
					# fill color has the interpolated value from scenario as value
					clip_fill_color = fill_color_l1
					# fill color has the interpolated value from scenario as value
					clip_stroke_color = stroke_color

				# clip_stroke_color_links has the interpolated value from scenario as value
				clip_stroke_color_links = stroke_color_links

				final_frame_in = initial_frame_in
				transformed_clip_name = val_init["svg_input_file_name"]+"-"+countActiveClip

				#######################################################
				# building the transformation scenario file for each added frame
				transformation_scenario_file_name = tmp_dir_name + "LYM_Arachnid_scenario_transformations-single_clip.csv"

				# print( "Transformation scenario file ", transformation_scenario_file_name,"\n" )
				try:
					FILEout = open(transformation_scenario_file_name, "wt")
				except IOError:
					print("File not created :", transformation_scenario_file_name, " or path is incorrect")

				transformation_scenario_string = ""
				# print( "Input frame ", transformation_scenario_file_name,"\n" )
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
					'nb_posterization_layers' : ['int', 1, 1, 's'] , \
					'transf1_ID' : ['string', transformation, transformation, 's'] , \
					'percent_transf1' : ['float', str(clip_percent_transf), str(clip_percent_transf), 'l'] , \
					'percent_links1' : ['float', str(percent_links), str(percent_links), 'l'] , \
					'stroke_color_links1' : ['hexcolor', str(clip_stroke_color_links), str(clip_stroke_color_links), 'l'] , \
					# number of frames of existence for a single layer
					'nb_frames_transf1' : ['int', str(nb_frames_transf1), str(nb_frames_transf1), 'l'] , \
					# percentage of its life in a scene for a single layer
					'percent_life_transf1' : ['float', str(percent_life_transf1), str(percent_life_transf1), 'l'] , \
					'x_scaling_center' : ['float', str(clip_scaling_center_x), str(clip_scaling_center_x), 'l'] , \
					'y_scaling_center' : ['float', str(clip_scaling_center_y), str(clip_scaling_center_y), 'l'] , \
					'scaling_factor' : ['float', str(clip_scaling_factor), str(clip_scaling_factor), 'l'] , \
					'x_translation' : ['float', str(clip_x_translation), str(clip_x_translation), 'l'] , \
					'y_translation' : ['float', str(clip_y_translation), str(clip_y_translation), 'l'] , \
					'fill_color_bg' : ['hexcolor', '#ffffff', '#ffffff', 'l'] , \
					'fill_color_l1' : ['hexcolor', clip_fill_color, clip_fill_color, 'l'] , \
					'fill_color_l2' : ['hexcolor', '#000000', '#000000', 'l'] , \
					'fill_color_l3' : ['hexcolor', '#000000', '#000000', 'l'] , \
					'fill_color_l4' : ['hexcolor', '#000000', '#000000', 'l'] , \
					'fill_opacity_l1' : ['float', str(current_fill_opacity), str(current_fill_opacity), 'l'] , \
					'fill_opacity_l2' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l3' : ['float', 1, 1, 'l'] , \
					'fill_opacity_l4' : ['float', 1, 1, 'l'] , \
					'stroke_color' : ['hexcolor', str(clip_stroke_color), str(clip_stroke_color), 'l'] , \
					'stroke_opacity' : ['float',  str(clip_stroke_opacity), str(clip_stroke_opacity), 'l'] , \
					'stroke_width' : ['float', str(current_stroke_width), str(current_stroke_width), 'l'] }

				transformation_scenario_string_list = variable_dict_to_CVS_file(scenario_variables)

				# transformation_scenario_string = ["RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29", \
				# 	"VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1,x_scaling_center,y_scaling_center,scaling_factor,x_translation,y_translation,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
				# 	"TYPE,string,string,string,string,string,int,int,int,string,float,float,hexcolor,int,float,float,float,float,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float",\
				# 	"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1,x_scaling_center,y_scaling_center,scaling_factor,x_translation,y_translation,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
				# 	"scenario,1,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", \
				# 	"scene,transf_scene,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", \
				# 	"transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene", \
				# 	"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1,x_scaling_center,y_scaling_center,scaling_factor,x_translation,y_translation,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
				# 	"Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_percent_transf_ZZZ,ZZZ_percent_links_ZZZ,ZZZ_stroke_color_links_ZZZ,ZZZ_transf_nb_frames_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,ZZZ_transf_x_translation_ZZZ,ZZZ_transf_y_translation_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,ZZZ_fill_opacity_ZZZ,1,1,1,ZZZ_stroke_color_ZZZ,ZZZ_stroke_opacity_ZZZ,ZZZ_stroke_width_ZZZ", \
				# 	"Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_percent_transf_ZZZ,ZZZ_percent_links_ZZZ,ZZZ_stroke_color_links_ZZZ,ZZZ_transf_nb_frames_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,ZZZ_transf_x_translation_ZZZ,ZZZ_transf_y_translation_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,ZZZ_fill_opacity_ZZZ,1,1,1,ZZZ_stroke_color_ZZZ,ZZZ_stroke_opacity_ZZZ,ZZZ_stroke_width_ZZZ", \
				# 	"Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l", \
				# 	"/scene,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,", \
				# 	"/scenario,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,"]

				# # replaces the names of the variables in the string containing the scenario table (lines 8 and 9 that have the variable values)
				# for i in range(8, 10, 1):
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_clip_dir_name_ZZZ", clip_dir_name)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_clip_name_ZZZ", clip_name)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformed_clip_dir_ZZZ", transformed_clip_dir)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformed_clip_name_ZZZ", transformed_clip_name)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_tmp_dir_name_ZZZ", tmp_dir_name)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_percent_transf_ZZZ", str(clip_percent_transf))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_percent_links_ZZZ", str(clip_percent_links))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_stroke_color_links_ZZZ", str(clip_stroke_color_links))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_nb_frames_ZZZ", str(nb_frames_transf1))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_fill_color_ZZZ", clip_fill_color)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_fill_opacity_ZZZ", str(clip_fill_opacity))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_stroke_color_ZZZ", clip_stroke_color)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_stroke_opacity_ZZZ", str(clip_stroke_opacity))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_stroke_width_ZZZ", str(clip_stroke_width))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_initial_frame_in_ZZZ", str(initial_frame_in))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_final_frame_in_ZZZ", str(final_frame_in))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_initial_transformed_frame_out_ZZZ", str(initial_transformed_frame_out))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_final_transformed_frame_out_ZZZ", str(final_transformed_frame_out))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformation_ZZZ", transformation)
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_x_scaling_center_ZZZ", str(clip_scaling_center_x))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_y_scaling_center_ZZZ", str(clip_scaling_center_y))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_scaling_factor_ZZZ", str(clip_scaling_factor))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_x_translation_ZZZ", str(clip_x_translation))
				# 	transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_y_translation_ZZZ", str(clip_y_translation))
				
				#######################################################
				# applying the transformation with the current scenario file for each added frame
				for line in transformation_scenario_string_list:
					FILEout.write( line+"\n" )
					# print(line)
				FILEout.close()

				if not TRACE_ONLY:
					# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n" )
					if vv_scenario_transform.main(["-i", \
						transformation_scenario_file_name, "--chapter_no", ind_chapter]) == 0:
						return 0
					
				indActiveClip += 1
				# end of loop on the active clips to apply a transformation

			##################################################################
			# 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################

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

			# lines 9 and 10 for the values of the output file name and directory, number of composed files and frame out number
			nb_active_clips = len(active_performing_clips)

			scenario_variables =  { \
				'svg_output_directory' : ['string', crossfaded_clip_dir, crossfaded_clip_dir, 's'] , \
				'svg_output_file_name' : ['string', crossfaded_clip_name, crossfaded_clip_name, 's'] , \
				'composition_mode' : ['string', 'PARALLEL', 'PARALLEL', 's'] , \
				'nb_posterization_layers' : ['int', 1, 1, 's'] , \
				'nb_composed_files' : ['int', str(nb_active_clips), str(nb_active_clips), 's'] , \
				'imageOutputIndex' : ['int', 1, 1, 's']  }

			crossfading_scenario_string_list = variable_dict_to_CVS_file(scenario_variables)

			# crossfading_scenario_string = [ \
			# 	"RANK,0,1,2,3,4,5", \
			# 	"VERBATIM,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
			# 	"TYPE,string,string,string,int,int,int", \
			# 	"ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
			# 	"scenario,1,,,,,", \
			# 	"scene,crossfade_scene,,,,,", \
			# 	"crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene", \
			# 	"ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
			# 	"Initial,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_initial_crossfaded_frame_out_ZZZ", \
			# 	"Final,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_final_crossfaded_frame_out_ZZZ", \
			# 	"Interp,s,s,s,s,s,s", \
			# 	"/scene,,,,,,", \
			# 	"/scenario,,,,,," ]

			# for i in range(8, 10, 1):
			# 	crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_crossfaded_clip_dir_ZZZ", crossfaded_clip_dir)
			# 	crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_crossfaded_clip_name_ZZZ", crossfaded_clip_name)
			# 	crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_nb_composed_files_ZZZ", str(nb_active_clips))
			# 	crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_initial_crossfaded_frame_out_ZZZ", "1")
			# 	crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_final_crossfaded_frame_out_ZZZ", "1")

			#######################################################
			# addition of the columns of all the frames
			# loop on the active clips to add them to the scenario file
			# the first clips come last
			rank = 6
			for indActiveClip in range(len(active_performing_clips)) :
				countActiveClip = "%03d" % (indActiveClip + 1)

				additional_scenario_variables =  { \
					'svg_input_directory_'+countActiveClip : ['string', transformed_clip_dir, transformed_clip_dir, 's'] , \
					'svg_input_file_name_'+countActiveClip : ['string', val_init["svg_input_file_name"]+"-"+countActiveClip, val_init["svg_input_file_name"]+"-"+countActiveClip, 's'] , \
					'imageInputIndex_'+countActiveClip : ['int', 1, 1, 's'] }

				crossfading_scenario_string_list = variable_dict_append_CVS_file(additional_scenario_variables, crossfading_scenario_string_list)

				# ind_line = 0
				# for i in range(0, 3):
				# 	crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+str(rank)
				# 	rank += 1
				
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",string,string,int"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",crossfade_scene,crossfade_scene,crossfade_scene"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				# ind_line += 1

				# transformed_clip_name
				# transformed_clip_name = val_init["svg_input_file_name"]+"-"+countActiveClip
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+transformed_clip_dir+","+transformed_clip_name+","+str(initial_transformed_frame_out)
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+transformed_clip_dir+","+transformed_clip_name+","+str(final_transformed_frame_out)
				# ind_line += 1

				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",s,s,s"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				# ind_line += 1
				# crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				# ind_line += 1

			# loop on the active clips to add them to the scenario file

			for line in crossfading_scenario_string_list:
				FILEout.write( line+"\n" )

			FILEout.close()

			#######################################################
			# applying the transformation with the current scenario file for each added frame
			if not TRACE_ONLY:
				# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/crossfade-movie.sh\n" )
				if vv_scenario_layer_crossfade.main(["-i", \
						crossfading_scenario_file_name]) == 0:
					return 0

			##################################################################
			# 5: FINAL TRANSFORMATIONS OF THE CROSSFADED FRAMES
			##################################################################

			end_transformation = val_init["transfFin_ID"]

			# lowercase variable: initial thread - UPPERCASE VARIABLE: FINAL THREAD
			clip_dir_name = tmp_dir_name
			clip_name = "crossfaded-"+val_init["svg_input_file_name"]

			end_transformed_clip_dir = val_init["svg_output_directory"]
			countOutpput = "%04d" % current_crossfaded_frame_out
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
				'nb_posterization_layers' : ['int', 1, 1, 's'] , \
				'transf1_ID' : ['string', end_transformation, end_transformation, 's'] , \
				# noise
				'percent_transf1' : ['float', str(clip_percent_transf), str(clip_percent_transf), 'l'] , \
				# global number of links (individual percentages are managed in chapter 1 through percent_life_transf1)
				'percent_links1' : ['float', str(percent_links), str(percent_links), 'l'] , \
				'stroke_color_links1' : ['hexcolor', str(clip_stroke_color_links), str(clip_stroke_color_links), 'l'] , \
				# next 2 used only in first pass in layer by layer transformation
				'nb_frames_transf1' : ['int', str(indImageInput), str(indImageInput), 'l'], \
				'percent_life_transf1' : ['float', str(0), str(1), 'l'] }

			end_transformation_scenario_string_list = variable_dict_to_CVS_file(scenario_variables)

			# end_transformation_scenario_string = [ \
			# 	"RANK,0,1,2,3,4,5,6,7,8,9,10,11,12", \
			# 	"VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1", \
			# 	"TYPE,string,string,string,string,string,int,int,int,string,float,float,hexcolor,int", \
			# 	"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1", \
			# 	"scenario,1,,,,,,,,,,,,", \
			# 	"scene,transf_scene,,,,,,,,,,,,", \
			# 	"transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene", \
			# 	"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,percent_links1,stroke_color_links1,nb_frames_transf1", \
			# 	"Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_percent_transf_ZZZ,ZZZ_percent_links_ZZZ,ZZZ_stroke_color_links_ZZZ,ZZZ_transf_nb_frames_ZZZ", \
			# 	"Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_percent_transf_ZZZ,ZZZ_percent_links_ZZZ,ZZZ_stroke_color_links_ZZZ,ZZZ_transf_nb_frames_ZZZ", \
			# 	"Interp,s,s,s,s,s,s,s,s,s,l,l,l,l", \
			# 	"/scene,,,,,,,,,,,,,", \
			# 	"/scenario,,,,,,,,,,,,,"]
			# for i in range(8, 10, 1):
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_clip_dir_name_ZZZ", clip_dir_name)
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_clip_name_ZZZ", clip_name)
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transformed_clip_dir_ZZZ", end_transformed_clip_dir)
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transformed_clip_name_ZZZ", end_transformed_clip_name)
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_tmp_dir_name_ZZZ", tmp_dir_name)
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_percent_transf_ZZZ", str(clip_percent_transf))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_percent_links_ZZZ", str(clip_percent_links))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_stroke_color_links_ZZZ", str(clip_stroke_color_links))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transf_nb_frames_ZZZ", str(indImageInput))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_initial_frame_in_ZZZ", str(initial_frame_in))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_final_frame_in_ZZZ", str(final_frame_in))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_initial_transformed_frame_out_ZZZ", str(initial_end_transform_frame_out))
			#  	nd_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_final_transformed_frame_out_ZZZ", str(final_end_transform_frame_out))
			# 	end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_end_transformation_ZZZ", end_transformation)
			
			#######################################################
			# applying the end_transformation with the current scenario file for each added frame
			for line in end_transformation_scenario_string_list:
				FILEout.write( line+"\n" )
			
			FILEout.close()

			if not TRACE_ONLY:
				# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n" )
				count = "%04d" % initial_end_transform_frame_out
				print( "Output SVG file ", end_transformed_clip_dir+end_transformed_clip_name+"_"+count+".svg","\n" )
				if vv_scenario_transform.main(["-i", \
						end_transformation_scenario_file_name, "--chapter_no", ind_chapter]) == 0:
					return 0

			current_crossfaded_frame_out += 1
			# end of SCANS ALL THE FRAMES IN THE MOVIE

		# /scene line
		values_line =  next(readCVS)
		if values_line[0] != "/scene":
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
	# chapters
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
