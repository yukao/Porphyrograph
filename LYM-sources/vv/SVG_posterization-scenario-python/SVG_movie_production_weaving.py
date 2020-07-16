#!/usr/bin/python3
# use strict
# use warnings

# use Math::Trig

# package SVG_movie_production_weaving

# use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/'
# use SVG_scenario_interpolation_lib
# use SVG_movie_scenario_transformation
# use SVG_movie_scenario_layer_crossfading

import csv
import os
from random import seed
from random import random
import getopt
import sys

import SVG_scenario_interpolation_lib
import SVG_movie_scenario_transformation
import SVG_movie_scenario_layer_crossfading

# SVG_movie_production_weaving-1.pl
# reads a transformation scenario and executes it

##################################################################
# CONSTANTS
##################################################################
NB_FRAMES_MOVIE = 120 * 30 # 2 mminutes video
# NB_FRAMES_MOVIE = 20 * 30 # 20 sec video
NB_FRAMES_MOVIE = 10
DELAY_BETWEEN_CLIP = 1 * 30 # 1 s between the end of a clip and the choice of another one

TRACE_ONLY = False

SVG_PAGE_WIDTH = 38400
SVG_PAGE_HEIGHT = 21600

NUMBER_OF_THREADS_IN_WEB = 36
WEB_RADIUS = 30000

NUMBER_OF_SPIRAL_TURNS_IN_WEB = 10

##################################################################
# GLOBAL VARIABLES
##################################################################
used_web_threads = []
for i in range(NUMBER_OF_THREADS_IN_WEB):
	used_web_threads.append(-1)

##################################################################
# HASHES FOR CLIP management
##################################################################
performing_clip_sizes = { \
	"silhouette-01" : 66, \
	"silhouette-02" : 108, \
	"silhouette-03" : 265, \
	"silhouette-04" : 144, \
	"silhouette-05" : 43, \
	"silhouette-06" : 141, \
	"silhouette-07" : 99, \
	"silhouette-08" : 110, \
	"silhouette-09" : 67, \
	"silhouette-10" : 108, \
	"silhouette-11" : 266, \
	"silhouette-12" : 145, \
	"silhouette-13" : 44, \
	"silhouette-14" : 142, \
	"silhouette-15" : 100, \
	"silhouette-16" : 110}

clip_probability_weight_performing_dance = { \
	"silhouette-01" : 2/28, \
	"silhouette-02" : 2/28, \
	"silhouette-03" : 1/28, \
	"silhouette-04" : 1/28, \
	"silhouette-05" : 2/28, \
	"silhouette-06" : 2/28, \
	"silhouette-07" : 2/28, \
	"silhouette-08" : 2/28, \
	"silhouette-09" : 2/28, \
	"silhouette-10" : 2/28, \
	"silhouette-11" : 1/28, \
	"silhouette-12" : 1/28, \
	"silhouette-13" : 2/28, \
	"silhouette-14" : 2/28, \
	"silhouette-15" : 2/28, \
	"silhouette-16" : 2/28}

clip_probability_weight_performing_weaving = { \
	"silhouette-01" : 2/64, \
	"silhouette-02" : 2/64, \
	"silhouette-03" : 10/64, \
	"silhouette-04" : 10/64, \
	"silhouette-05" : 2/64, \
	"silhouette-06" : 2/64, \
	"silhouette-07" : 2/64, \
	"silhouette-08" : 2/64, \
	"silhouette-09" : 2/64, \
	"silhouette-10" : 2/64, \
	"silhouette-11" : 10/64, \
	"silhouette-12" : 10/64, \
	"silhouette-13" : 2/64, \
	"silhouette-14" : 2/64, \
	"silhouette-15" : 2/64, \
	"silhouette-16" : 2/64}

clip_probability_weight = { \
	"silhouette-01" : 0, \
	"silhouette-02" : 0, \
	"silhouette-03" : 0, \
	"silhouette-04" : 0, \
	"silhouette-05" : 0, \
	"silhouette-06" : 0, \
	"silhouette-07" : 0, \
	"silhouette-08" : 0, \
	"silhouette-09" : 0, \
	"silhouette-10" : 0, \
	"silhouette-11" : 0, \
	"silhouette-12" : 0, \
	"silhouette-13" : 0, \
	"silhouette-14" : 0, \
	"silhouette-15" : 0, \
	"silhouette-16" : 0}

# hash of arrays, each array contains
# (clip_name, clip_size, clip_first_frame_no, clip_fill_color, 
# clip_silhouette_color, clip_stroke_width, clip_scaling_factor,
# clip_scaling_center_x,clip_scaling_center_y)
# the key is the clip name
active_performing_clips = dict()

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
	USAGE = '''/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_production_weaving-2.pm -i scenario_file

	ARGUMENTS:
	-i: input scenariofile
	'''

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
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	line =  next(readCSV) 
	# print( "line1 ", line  )
	line =  next(readCSV) 
	# print( "line2 ", line  )

	#3rd line: variable types
	line_types =  next(readCSV)

	#4th line: variable names
	var_names =  next(readCSV) 
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
	values_line =  next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found!")
		return 0
	
	nb_scenes = int(values_line[1])
	# print( "nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		# scene line
		values_line =  next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =  next(readCSV)
		# var IDs line
		line =  next(readCSV)
	
		# variable initial value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		print(values_line)
		for value_init in values_line:
			val_init[var_names[indVar]] = SVG_scenario_interpolation_lib.type_cast_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = SVG_scenario_interpolation_lib.type_cast_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =  next(readCSV)
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
		# else:
		# 	print ("Directory %s already exists" % val_init["svg_output_directory"])

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
			if pas != 0 and (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) != 0 :
				scene_percentage = (indImageInput - val_init["imageInputIndex"])/(val_fin["imageInputIndex"] - val_init["imageInputIndex"])
			
			#clamping
			scene_percentage = max(min(scene_percentage, 1.0), 0.0)
			# interpolating all the variables
			for var_name in var_names :
				# print( "var name [",var_name,"]\n" )
				val_interp[var_name] = SVG_scenario_interpolation_lib.interpolated_value_calculation(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)

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

			fill_color_min = ""
			stroke_color_min = ""
			stroke_width_min = 0
			scale_min = 0

			fill_color_max = ""
			stroke_color_max = ""
			stroke_width_max = 0
			scale_max = 0

			weaving_clip_weight = 0.

			##################################################################
			# 1: PARAMETERS INTERPOLATION
			##################################################################
			# FILL COLOR management
			fill_color_min = val_interp["fill_color_min"]
			fill_color_max = val_interp["fill_color_max"]

			# STROKE COLOR management
			stroke_color_min = val_interp["stroke_color_min"]
			stroke_color_max = val_interp["stroke_color_max"]

			# SCALING management
			scale_min = val_interp["scale_min"]
			scale_max = val_interp["scale_max"]

			# STROKE WIDTH management
			stroke_width_min = val_interp["stroke_width_min"]
			stroke_width_max = val_interp["stroke_width_max"]

			# CLIP WEIGHT management
			weaving_clip_weight = val_interp["weaving_clip_weight"]

			# NUMBER OF ACTIVE CLIPS management
			nb_transformed_performing_clips = val_interp["nb_active_clips"]

			##################################################################
			# 2: management OF CLIP ACTIVATION
			##################################################################

			##################################################################
			# RELEASING CLIPS THAT HAVE BEEN PLAYED UNTIL THE END
			for key, value in active_performing_clips.items():
				#print( key, value )
				clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
						clip_silhouette_color, clip_stroke_width, \
						clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y = value 
				# clip deactivation
				if current_crossfaded_frame_out - clip_first_frame_no + 1 > clip_size:
					del active_performing_clips[key]
				
			

			##################################################################
			# ACTIVATING NEW PERFORMING CLIPS UNTIL THE EXPECTED QUANTITY OF ACTIVE CLIPS IS REACHED
			# new clip activation
			# print( "current expected number of clips " , nb_transformed_performing_clips , "\n" )
			for indNewClipActivation in range(len(active_performing_clips), nb_transformed_performing_clips) :
				# strke and fill parameters selection
				fill_color = SVG_scenario_interpolation_lib.interpolated_value_calculation ("hexcolor", fill_color_min, fill_color_max, "l", random())
				stroke_color = SVG_scenario_interpolation_lib.interpolated_value_calculation ("hexcolor", stroke_color_min, stroke_color_max, "l", random())
				randVal = random()
				stroke_width = (1 - randVal) * stroke_width_min + randVal * stroke_width_max
				randVal = random()
				scaling_factor = (1 - randVal) * scale_min + randVal * scale_max

				# print("percent %.2f silhouette %.2f colors F %s C %s" %  (scaling_factor, stroke_width, fill_color, stroke_color))
				
				# random choice of a clip in the list
				for item in clip_probability_weight.items() : 
					key, weight = item
					clip_probability_weight[key ] = (1 - weaving_clip_weight) * clip_probability_weight_performing_dance[key] \
							+ weaving_clip_weight * clip_probability_weight_performing_weaving[key]
				
				newActivatedClipID = ""
				# to avoid floating point inaccuracies
				while(True):
					rand_value = random()
					for item in clip_probability_weight.items() : 
						key, weight = item
						rand_value -= weight
						if (rand_value < 0) :
							newActivatedClipID = key
							break
					if newActivatedClipID != "":
						break

				newActivatedClipSize = 0
				if newActivatedClipID in performing_clip_sizes :
					newActivatedClipSize = performing_clip_sizes[newActivatedClipID]
				else:
					raise NameError("Unknown clip "+newActivatedClipID)
				
				# looks for an available key for the newly active clip
				indKeyNewActiveClip = 1
				keyNewActiveClip = "key-%02d" % indKeyNewActiveClip
				while keyNewActiveClip in active_performing_clips:
					indKeyNewActiveClip += 1
					keyNewActiveClip = "key-%02d" % indKeyNewActiveClip
				
				# new clip activation
				active_performing_clips[keyNewActiveClip] \
					= [newActivatedClipID, newActivatedClipSize, current_crossfaded_frame_out, \
						fill_color, stroke_color, stroke_width, scaling_factor, SVG_PAGE_WIDTH / 2, 0]

			##################################################################
			# 3: TRANSFORMATIONS OF THE CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################
			initial_transformed_frame_out = 1
			final_transformed_frame_out = 1
			indActiveClip = 1
			if not TRACE_ONLY:
				print( "Nb active clips ", len(active_performing_clips),"\n" )
			
			# loop on the active clips to apply a transformation
			for key in sorted(active_performing_clips):
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

				if indActiveClip <= len(active_performing_clips) :
					clip_name, clip_size, clip_first_frame_no, clip_fill_color, \
						clip_silhouette_color, clip_stroke_width, \
						clip_scaling_factor, clip_scaling_center_x, clip_scaling_center_y \
						= active_performing_clips[key]

					clip_dir_name = clip_dir_name  + clip_name + "/"
					initial_frame_in = current_crossfaded_frame_out - clip_first_frame_no + 1
				

				final_frame_in = initial_frame_in
				transformed_clip_name = val_init["svg_input_file_name"]+"-"+countActiveClip

				#######################################################
				# buiding the transformation scenario file for each added frame
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
					raise NameError("Can't open frame "+full_frame_name+" from clip " + clip_name + " at frame no " + initial_frame_in)
				
				transformation_scenario_string = ["RANK,0,1,2,3,4,5,6,7,8,9,,,,10,11,12,13,14,15,16,17,18,19,20,21", \
					"VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
					"TYPE,string,string,string,string,string,int,int,int,string,float,float,float,float,hexcolor,hexcolor,hexcolor,hexcolor,hexcolor,float,float,float,float,hexcolor,float,float",\
					"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
					"scenario,1,,,,,,,,,,,,,,,,,,,,,,,,", \
					"scene,transf_scene,,,,,,,,,,,,,,,,,,,,,,,,", \
					"transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene", \
					"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1,x_scaling_center,y_scaling_center,scaling_factor,fill_color_bg,fill_color_l1,fill_color_l2,fill_color_l3,fill_color_l4,fill_opacity_l1,fill_opacity_l2,fill_opacity_l3,fill_opacity_l4,stroke_color,stroke_opacity,stroke_width",\
					"Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_percent_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_silhouette_color_ZZZ,1,ZZZ_stroke_width_ZZZ", \
					"Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_transformation_ZZZ,ZZZ_transf_percent_ZZZ,ZZZ_transf_x_scaling_center_ZZZ,ZZZ_transf_y_scaling_center_ZZZ,ZZZ_transf_scaling_factor_ZZZ,#ffffff,ZZZ_fill_color_ZZZ,#000000,#000000,#000000,1,1,1,1,ZZZ_silhouette_color_ZZZ,1,ZZZ_stroke_width_ZZZ", \
					"Interp,s,s,s,s,s,s,s,s,s,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l", \
					"/scene,,,,,,,,,,,,,,,,,,,,,,,,,", \
					"/scenario,,,,,,,,,,,,,,,,,,,,,,,,,"]

				# replaces the names of the variables in the string containing the scenario table (lines 8 and 9 that have the variable values)
				for i in range(8, 10, 1):
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_clip_dir_name_ZZZ", clip_dir_name)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_clip_name_ZZZ", clip_name)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformed_clip_dir_ZZZ", transformed_clip_dir)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformed_clip_name_ZZZ", transformed_clip_name)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_tmp_dir_name_ZZZ", tmp_dir_name)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_percent_ZZZ", "1.0")
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_fill_color_ZZZ", clip_fill_color)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_silhouette_color_ZZZ", clip_silhouette_color)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_stroke_width_ZZZ", str(clip_stroke_width))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_initial_frame_in_ZZZ", str(initial_frame_in))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_final_frame_in_ZZZ", str(final_frame_in))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_initial_transformed_frame_out_ZZZ", str(initial_transformed_frame_out))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_final_transformed_frame_out_ZZZ", str(final_transformed_frame_out))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transformation_ZZZ", transformation)
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_x_scaling_center_ZZZ", str(clip_scaling_center_x))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_y_scaling_center_ZZZ", str(clip_scaling_center_y))
					transformation_scenario_string[i] = transformation_scenario_string[i].replace("ZZZ_transf_scaling_factor_ZZZ", str(clip_scaling_factor))
				
				#######################################################
				# applying the transformation with the current scenario file for each added frame
				for line in transformation_scenario_string:
					FILEout.write( line+"\n" )
				FILEout.close()

				if not TRACE_ONLY:
					# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n" )
					if SVG_movie_scenario_transformation.main(["-i", \
							"/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_transformations-single_clip.csv"]) == 0:
						return 0
					
				indActiveClip += 1
				# end of loop on the active clips to apply a transformation

			##################################################################
			# 4: CROSSFADING OF THE TRANSFORMED CURRENT FRAMES OF THE ACTIVE MOVIES
			##################################################################

			#######################################################
			# buiding the crossfading scenario file with the list of added frames
			crossfading_scenario_string = [ \
				"RANK,0,1,2,3,4,5", \
				"VERBATIM,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
				"TYPE,string,string,string,int,int,int", \
				"ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
				"scenario,1,,,,,", \
				"scene,crossfade_scene,,,,,", \
				"crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene,crossfade_scene", \
				"ID,svg_output_directory,svg_output_file_name,composition_mode,nb_posterization_layers,nb_composed_files,imageOutputIndex", \
				"Initial,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_initial_crossfaded_frame_out_ZZZ", \
				"Final,ZZZ_crossfaded_clip_dir_ZZZ,ZZZ_crossfaded_clip_name_ZZZ,PARALLEL,1,ZZZ_nb_composed_files_ZZZ,ZZZ_final_crossfaded_frame_out_ZZZ", \
				"Interp,s,s,s,s,s,s", \
				"/scene,,,,,,", \
				"/scenario,,,,,," ]

			rank = 6
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
			for i in range(8, 10, 1):
				crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_crossfaded_clip_dir_ZZZ", crossfaded_clip_dir)
				crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_crossfaded_clip_name_ZZZ", crossfaded_clip_name)
				crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_nb_composed_files_ZZZ", str(nb_active_clips))
				crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_initial_crossfaded_frame_out_ZZZ", "1")
				crossfading_scenario_string[i] = crossfading_scenario_string[i].replace("ZZZ_final_crossfaded_frame_out_ZZZ", "1")

			#######################################################
			# addition of the columns of all the frames
			indActiveClip = 1
			# loop on the active clips to add them to the scenario file
			for key in sorted(active_performing_clips):
				countActiveClip = "%03d" % indActiveClip

				ind_line = 0
				for i in range(0, 3):
					crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+str(rank)
					rank += 1
				
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",string,string,int"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",crossfade_scene,crossfade_scene,crossfade_scene"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",svg_input_directory_"+countActiveClip+",svg_input_file_name_"+countActiveClip+",imageInputIndex_"+countActiveClip+""
				ind_line += 1

				transformed_clip_name
				transformed_clip_name = val_init["svg_input_file_name"]+"-"+countActiveClip
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+transformed_clip_dir+","+transformed_clip_name+","+str(initial_transformed_frame_out)
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+","+transformed_clip_dir+","+transformed_clip_name+","+str(final_transformed_frame_out)
				ind_line += 1

				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",s,s,s"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				ind_line += 1
				crossfading_scenario_string[ind_line] = crossfading_scenario_string[ind_line]+",,,"
				ind_line += 1

				indActiveClip += 1
			# loop on the active clips to add them to the scenario file

			for line in crossfading_scenario_string:
				FILEout.write( line+"\n" )

			FILEout.close()

			#######################################################
			# applying the transformation with the current scenario file for each added frame
			if not TRACE_ONLY:
				# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/crossfade-movie.sh\n" )
				if SVG_movie_scenario_layer_crossfading.main(["-i", \
						"/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_layer_crossfading-movie.csv"]) == 0:
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
			# buiding the end_transformation scenario file for the cross-faded frame
			end_transformation_scenario_file_name = tmp_dir_name + "LYM_Arachnid_scenario_end_transformations-movie.csv"

			# print( "Transformation scenario file ", end_transformation_scenario_file_name,"\n" )
			try:
				FILEout = open(end_transformation_scenario_file_name, "wt")
			except IOError:
				print("File not created :", end_transformation_scenario_file_name, " or path is incorrect")

			end_transformation_scenario_string = [ \
				"RANK,0,1,2,3,4,5,6,7,8,9", \
				"VERBATIM,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1", \
				"TYPE,string,string,string,string,string,int,int,int,string,float", \
				"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1", \
				"scenario,1,,,,,,,,,", \
				"scene,transf_scene,,,,,,,,,", \
				"transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene,transf_scene", \
				"ID,svg_input_directory,svg_input_file_name,svg_output_directory,svg_output_file_name,tmp_directory,imageInputIndex,imageOutputIndex,nb_posterization_layers,transf1_ID,percent_transf1", \
				"Initial,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_initial_frame_in_ZZZ,ZZZ_initial_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_transf_percent_ZZZ", \
				"Final,ZZZ_clip_dir_name_ZZZ,ZZZ_clip_name_ZZZ,ZZZ_transformed_clip_dir_ZZZ,ZZZ_transformed_clip_name_ZZZ,ZZZ_tmp_dir_name_ZZZ,ZZZ_final_frame_in_ZZZ,ZZZ_final_transformed_frame_out_ZZZ,1,ZZZ_end_transformation_ZZZ,ZZZ_transf_percent_ZZZ", \
				"Interp,s,s,s,s,s,s,s,s,s,l", \
				"/scene,,,,,,,,,,", \
				"/scenario,,,,,,,,,,"]
			for i in range(8, 10, 1):
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_clip_dir_name_ZZZ", clip_dir_name)
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_clip_name_ZZZ", clip_name)
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transformed_clip_dir_ZZZ", end_transformed_clip_dir)
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transformed_clip_name_ZZZ", end_transformed_clip_name)
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_tmp_dir_name_ZZZ", tmp_dir_name)
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_transf_percent_ZZZ", str(scene_percentage))
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_initial_frame_in_ZZZ", str(initial_frame_in))
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_final_frame_in_ZZZ", str(final_frame_in))
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_initial_transformed_frame_out_ZZZ", str(initial_end_transform_frame_out))
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_final_transformed_frame_out_ZZZ", str(final_end_transform_frame_out))
				end_transformation_scenario_string[i] = end_transformation_scenario_string[i].replace("ZZZ_end_transformation_ZZZ", end_transformation)
			
			#######################################################
			# applying the end_transformation with the current scenario file for each added frame
			for line in end_transformation_scenario_string:
				FILEout.write( line+"\n" )
			
			FILEout.close()

			if not TRACE_ONLY:
				# print(  "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/cmd/transform-movie.sh\n" )
				count = "%04d" % initial_end_transform_frame_out
				print( "Output SVG file ", end_transformed_clip_dir+end_transformed_clip_name+"_"+count+".svg","\n\n" )
				if SVG_movie_scenario_transformation.main(["-i", \
						"/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/tmp/LYM_Arachnid_scenario_end_transformations-movie.csv"]) == 0:
					return 0

			current_crossfaded_frame_out += 1
			# end of SCANS ALL THE FRAMES IN THE MOVIE

		# /scene line
		values_line =  next(readCSV)
		if values_line[0] != "/scene":
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
	# scenes
	return 1

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])