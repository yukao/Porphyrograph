#!/usr/bin/python3 

# vv_scenario_posterize.py
# scenario for initial posterization of vector video
# (c) Yukao Nagemi

import csv
import getopt
import sys

import os

import vv_lib
import vv_layer_crossfading

USAGE = '''vv_scenario_layer_crossfading.pl -i scenario_file
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
	
	nb_scenes = vv_lib.type_cast_num(values_line[1])
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
			val_init[var_names[indVar]] = vv_lib.type_cast_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = vv_lib.type_cast_num(value_fin) 
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
			# 	scene_percentage = 0
			# 	if((val_fin["imageOutputIndex"] - val_init["imageOutputIndex"]) > 0) :
			# 		scene_percentage = (indImageOutput - val_init["imageOutputIndex"])/(val_fin["imageOutputIndex"] - val_init["imageOutputIndex"])
				
			# 	#clamping
			# 	scene_percentage = vv_lib.clamp(scene_percentage)

			# 	for var_name in var_names :
			# 		val_interp[var_name] = vv_lib.interpolated_value_calculation(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				

			# 	if(trace_only) :
			# 		 printf("Ind image %d Scene percentage %.2f percent_transf1 %.2f" % indImageOutput, scene_percentage, val_interp["percent_transf1"])
			

			# scans all the files and collects the paths to compose them
			string_all_input_files = ""
			for indInputFile  in range(1, nb_composed_files + 1, 1) :
				countInputFile = "%03d" % indInputFile
				full_svg_input_file_name = val_init["svg_input_directory_"+countInputFile] + val_init["svg_input_file_name_"+countInputFile]

				indImageInput = image_rank + val_init["imageInputIndex_"+countInputFile]
				if( indImageInput > val_fin["imageInputIndex_"+countInputFile]) :
					indImageInput = val_fin["imageInputIndex_"+countInputFile]
				
				countInputImage = "%04d" % indImageInput
				string_all_input_files = string_all_input_files + full_svg_input_file_name +"_"+countInputImage+".svg "
			

			countOutputImage = "%04d" % indImageOutput
			full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"] + "_" + countOutputImage + ".svg"

			if(not trace_only) :
				# print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/vv_layer_crossfading.pl --composition-mode :composition_mode_string --nb-files :nb_composed_files --nb-layers :nb_layers -o :full_svg_output_file_name -i :string_all_input_files\n"
				if(vv_layer_crossfading.main(["--composition-mode", composition_mode_string,\
					"--nb-files", nb_composed_files, "--nb-layers", nb_layers, \
					"-o", full_svg_output_file_name, "-i", string_all_input_files]) == 0) :
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
    main(sys.argv[1:])
