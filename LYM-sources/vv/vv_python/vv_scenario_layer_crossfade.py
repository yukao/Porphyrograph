#!/usr/bin/python3 

# vv_scenario_layer_crossfade.py
# scenario for crossfading multilayer frames of vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import csv
import getopt
import sys

import os
from signal import signal, SIGINT

import vv_layer_crossfade
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_layer_crossfade.py -i scenario_file
ARGUMENTS:
	 -i: input scenariofile
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()

	trace_only = 0

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
	line = next(readCSV) 
	line = next(readCSV) 

	#3rd line: variable types
	line_types = next(readCSV) 

	#4th line: variable names
	var_names =  next(readCSV) 
	#removes the first one
	var_names.pop(0)
	# print"Var names [" , join("-", @var_names),"]\n"

	#finishes the types now that the variable strings are known
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
	
	nb_scenes = to_num(values_line[1])
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
		for value_init in values_line:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin) 
			indVar += 1

		# variable interpolation value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp    
			indVar += 1

		# printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", nb_scenes, scene_ID,
		#   val_init["svg_input_file_name"], val_init["imageInputIndex"], 
		#   val_fin:"imageInputIndex", val_init["imageOutputIndex"], 
		#   val_fin["imageOutputIndex"])

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

		nb_layers = val_init["nb_posterization_layers"]
		nb_composed_files = val_init["nb_composed_files"]

		composition_mode_string = val_init["composition_mode"]

		##################################################################
		# CHeCKING INPUT FILES
		##################################################################
		# print("crossfade nb_composed_files:", nb_composed_files)
		for indInputFile in range(1, nb_composed_files + 1, 1) :
			countInputFile = "%03d" % indInputFile
			if((val_fin["imageInputIndex_"+countInputFile] - val_init["imageInputIndex_"+countInputFile]) < (val_fin["imageOutputIndex"] - val_init["imageOutputIndex"])) :
				print( "Not enough input images in input file #"+indInputFile)
				return 0
			
		##################################################################
		# PRODUCING OUTPUT FILES
		##################################################################
		image_rank = 0
		for indImageOutput in range(val_init["imageOutputIndex"], val_fin["imageOutputIndex"]+1, 1):
			# scene percentage at this frame 
			scene_percentage = 0
			if (val_fin["imageOutputIndex"] - val_init["imageOutputIndex"]) != 0 :
				scene_percentage = float(indImageOutput - val_init["imageOutputIndex"])/(val_fin["imageOutputIndex"] - val_init["imageOutputIndex"])
			#clamping
			scene_percentage = clamp(scene_percentage)

			# interpolating all the variables
			for var_name in var_names :
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])

			# scans all the files and collects the paths to compose them
			string_all_input_files = ""
			for indInputFile  in range(1, nb_composed_files + 1, 1) :
				countInputFile = "%03d" % indInputFile
				full_svg_input_file_name = val_init["svg_input_directory_"+countInputFile] + val_init["svg_input_file_name_"+countInputFile]

				indImageInput = image_rank + val_init["imageInputIndex_"+countInputFile]
				if( indImageInput > val_fin["imageInputIndex_"+countInputFile]) :
					indImageInput = val_fin["imageInputIndex_"+countInputFile]
				
				countInputImage = "%05d" % indImageInput
				string_all_input_files = string_all_input_files + full_svg_input_file_name +"_"+countInputImage+".svg "
			

			countOutputImage = "%05d" % indImageOutput
			full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"] + "_" + countOutputImage + ".svg"

			crossfade_argts_extension = []
			# background rectangle when the layers do not cover the whole pictures
			# as in a grayscale image where layers span from black to light gray
			if("fill_color_bg" in val_interp) :
				interp_fill_color_bg = val_interp["fill_color_bg"]
				crossfade_argts_extension.extend(["--fill_color_bg", interp_fill_color_bg])
			if("fill_color_l1" in val_interp) :
				interp_fill_color_l1 = val_interp["fill_color_l1"]
				crossfade_argts_extension.extend(["--fill_color_l1", interp_fill_color_l1])
			if("fill_color_l2" in val_interp) :
				interp_fill_color_l2 = val_interp["fill_color_l2"]
				crossfade_argts_extension.extend(["--fill_color_l2", interp_fill_color_l2])
			if("fill_color_l3" in val_interp) :
				interp_fill_color_l3 = val_interp["fill_color_l3"]
				crossfade_argts_extension.extend(["--fill_color_l3", interp_fill_color_l3])
			if("fill_color_l4" in val_interp) :
				interp_fill_color_l4 = val_interp["fill_color_l4"]
				crossfade_argts_extension.extend(["--fill_color_l4", interp_fill_color_l4])
			if("fill_color_l5" in val_interp) :
				interp_fill_color_l5 = val_interp["fill_color_l5"]
				crossfade_argts_extension.extend(["--fill_color_l5", interp_fill_color_l5])
			if("fill_color_l6" in val_interp) :
				interp_fill_color_l6 = val_interp["fill_color_l6"]
				crossfade_argts_extension.extend(["--fill_color_l6", interp_fill_color_l6])
			if("fill_color_l7" in val_interp) :
				interp_fill_color_l7 = val_interp["fill_color_l7"]
				crossfade_argts_extension.extend(["--fill_color_l7", interp_fill_color_l7])
			if("fill_color_l8" in val_interp) :
				interp_fill_color_l8 = val_interp["fill_color_l8"]
				crossfade_argts_extension.extend(["--fill_color_l8", interp_fill_color_l8])
			# stroke color, width and opacity can be provided as a single and unified value for all layers
			# or in a layer by layer mode
			if("stroke_color_l1" in val_interp) :
				interp_stroke_color_l1 = val_interp["stroke_color_l1"]
				crossfade_argts_extension.extend(["--stroke_color_l1", interp_stroke_color_l1])
			if("stroke_color_l2" in val_interp) :
				interp_stroke_color_l2 = val_interp["stroke_color_l2"]
				crossfade_argts_extension.extend(["--stroke_color_l2", interp_stroke_color_l2])
			if("stroke_color_l3" in val_interp) :
				interp_stroke_color_l3 = val_interp["stroke_color_l3"]
				crossfade_argts_extension.extend(["--stroke_color_l3", interp_stroke_color_l3])
			if("stroke_color_l4" in val_interp) :
				interp_stroke_color_l4 = val_interp["stroke_color_l4"]
				crossfade_argts_extension.extend(["--stroke_color_l4", interp_stroke_color_l4])
			if("stroke_color_l5" in val_interp) :
				interp_stroke_color_l5 = val_interp["stroke_color_l5"]
				crossfade_argts_extension.extend(["--stroke_color_l5", interp_stroke_color_l5])
			if("stroke_color_l6" in val_interp) :
				interp_stroke_color_l6 = val_interp["stroke_color_l6"]
				crossfade_argts_extension.extend(["--stroke_color_l6", interp_stroke_color_l6])
			if("stroke_color_l7" in val_interp) :
				interp_stroke_color_l7 = val_interp["stroke_color_l7"]
				crossfade_argts_extension.extend(["--stroke_color_l7", interp_stroke_color_l7])
			if("stroke_color_l8" in val_interp) :
				interp_stroke_color_l8 = val_interp["stroke_color_l8"]
				crossfade_argts_extension.extend(["--stroke_color_l8", interp_stroke_color_l8])

			# color interpolation for stroke and fill for 2 files
			if("file1_file2_color_blend" in val_interp and nb_composed_files == 2) :
				file1_file2_color_blend = val_interp["file1_file2_color_blend"]
				crossfade_argts_extension.extend(["--file1_file2_color_blend", file1_file2_color_blend])

			# old clip index
			if("ind_old_clip" in val_interp and nb_composed_files == 2) :
				ind_old_clip = val_interp["file1_file2_color_blend"]
				crossfade_argts_extension.extend(["--ind_old_clip", ind_old_clip])

			if(not trace_only) :
				input_list = []
				for indInputFile  in range(1, nb_composed_files + 1, 1) :
					countInputFile = "%03d" % indInputFile
					indImageInput = image_rank + val_init["imageInputIndex_"+countInputFile]
					countInputImage = "%05d" % indImageInput
					input_list.append(val_init["svg_input_file_name_"+countInputFile] +"_"+countInputImage+".svg ")
				# print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/vv_layer_crossfade.pl --composition-mode :composition_mode_string --nb-files :nb_composed_files --nb-layers :nb_layers -o :full_svg_output_file_name -i :string_all_input_files\n"
				print("vv_layer_crossfade", input_list, "->", val_init["svg_output_file_name"] + "_" + countOutputImage + ".svg")
				if(vv_layer_crossfade.main(["--composition-mode", composition_mode_string,\
					"--nb-files", nb_composed_files, "--nb-layers", nb_layers, \
					"-o", full_svg_output_file_name, "-i", string_all_input_files]+crossfade_argts_extension) == 0) :
					return 0

			# moves forward on output frames
			image_rank += 1
		
		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
	# /scenario line
	values_line = next(readCSV)
	if(values_line[0] != "/scenario") :
		print( "Expected tag \"/scenario\" not found!\n" )
		return 0
	
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
