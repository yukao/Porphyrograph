#!/usr/bin/perl 

# use strict
# use warnings

# use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/'
# use SVG_scenario_interpolation_lib

# SVG_movie_scenario_rasterization.pm
# reads a rasterization scenario and executes it

import csv
import os

import getopt
import sys

import SVG_scenario_interpolation_lib

USAGE = ''' /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_rasterization/SVG_rasterization/SVG_movie_scenario_rasterization.pm -i scenario_file

ARGUMENTS:
	 -i: input scenariofile
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	full_svg_input_file_name = ""
	full_png_output_file_name = ""
	tmp_dir = ""
	nb_scenes = 0
	scene_ID = ""
	line = ""

	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()

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
	line =	next(readCSV) 
	# print( "line1 ", line	)
	line =	next(readCSV) 
	# print( "line2 ", line	)

	#3rd line: variable types
	line_types =	next(readCSV)

	#4th line: variable names
	var_names =	next(readCSV) 
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
	values_line =	next(readCSV)
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
		values_line =	next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =	next(readCSV)
		# var IDs line
		line =	next(readCSV)
	
		# variable initial value
		values_line =	next(readCSV)
		values_line.pop(0)
		indVar = 0
		print(values_line)
		for value_init in values_line:
			val_init[var_names[indVar]] = SVG_scenario_interpolation_lib.type_cast_num(value_init)
			indVar += 1

		# variable final value
		values_line =	next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = SVG_scenario_interpolation_lib.type_cast_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =	next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp		
			indVar += 1

		print("Nb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n" % (nb_scenes, scene_ID, val_init["svg_input_file_name"], val_init["imageInputIndex"], val_fin["imageInputIndex"], val_init["imageOutputIndex"], val_fin["imageOutputIndex"]))

		# VIDEO RASTERIZATION

		# from video to images
		# creates the directories in which the PNG files are saved
		if( not os.path.isdir(val_init["png_output_directory"])	):
			try:
				os.mkdir(val_init["png_output_directory"])
			except OSError:
				print ("Creation of the directory %s failed" % val_init["png_output_directory"])
			else:
				print ("Successfully created the directory %s " % val_init["png_output_directory"])				
		
		full_svg_input_file_name = val_init["svg_input_directory"] + val_init["svg_input_file_name"]
		full_png_output_file_name = val_init["png_output_directory"] + val_init["png_output_file_name"]

		# count the images from SVG files
		dirListing = os.listdir(val_init["svg_input_directory"])
		nb_files = len(dirListing)
		print("Number of files ", nb_files)

		##################################################################
		# SCANS ALL THE SVG FRAMES IN THE MOVIE
		##################################################################
		# transformation of the vector video of SVG files into PNG files
		indImageOutput = val_init["imageOutputIndex"]
		for indImageInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + 1, 1) :
				countInput = "%04d" % indImageInput
				countOutput = "%04d" % indImageOutput
				tmp_dir = val_init["tmp_directory"]

				scene_percentage = 0.0
				if(val_fin["imageInputIndex"] != val_init["imageInputIndex"]) :
					scene_percentage = (indImageInput - val_init["imageInputIndex"]) / (val_fin["imageInputIndex"] - val_init["imageInputIndex"])
				
				scene_percentage = SVG_scenario_interpolation_lib.clamp(scene_percentage)

				for var_name in var_names :
					val_interp[var_name] = SVG_scenario_interpolation_lib.interpolated_value_calculation(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				
				# converts the SVG into a bitmap, larger than the final image for a better quality and to crop the border
				# background color should not be visible because a rectangle covering the whole page as been added in the fill and stroke step
				print( "inkscape -z -w 3840 -h 2160 --export-area-page -b \\#FFFFFFFF -e "+str(tmp_dir)+"tmp_large.png "+str(full_svg_input_file_name)+"_"+str(countInput)+".svg")
				os.system( "inkscape -z -w 3840 -h 2160 --export-area-page -b \\#FFFFFFFF -e "+str(tmp_dir)+"tmp_large.png "+str(full_svg_input_file_name)+"_"+str(countInput)+".svg")

				# cropping and downscale of the png inmages
				crop_offset_x = int(val_interp["crop_offset_x"])
				crop_offset_y = int(val_interp["crop_offset_y"])
				crop_size_x = int(val_interp["crop_size_x"])
				crop_size_y = int(val_interp["crop_size_y"])
				print( "convert "+str(tmp_dir)+"tmp_large.png -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 "+str(full_png_output_file_name)+"_"+str(countOutput)+".png")
				os.system("convert "+str(tmp_dir)+"tmp_large.png -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -resize 1920x1080 -background white -alpha off -type TrueColor -define png:color-type=2 "+str(full_png_output_file_name)+"_"+str(countOutput)+".png")

				indImageOutput += 1
		
		# /scene line
		values_line =	next(readCSV)
		if values_line[0] != "/scene":
			print( "Expected tag \"/scene\" not found!" )
			return 0
		
	# scenes
	return 1

if __name__ == "__main__":
		import sys
		main(sys.argv[1:])