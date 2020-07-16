#!/usr/bin/python3 

# vv_scenario_posterize.py
# scenario for initial posterization of vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import csv
import os

import getopt
import sys
from signal import signal, SIGINT
import re

import vv_posterize
from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_posterize.py -i scenario_file
ARGUMENTS:
	 -i: input scenariofile
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	full_input_file_name = ""
	full_png_output_file_name = ""
	full_svg_output_file_name = ""
	tmp_dir = ""
	start_time = ""
	end_time = ""
	fps = 0
	nb_layers = 0
	nb_scenes = 0
	scene_ID = ""

	line = ""

	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
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

	line = next(readCSV) 
	# print( "line1 ", line )
	line = next(readCSV) 
	# print( "line2 ", line )

	#3rd line: variable types
	line_types = next(readCSV)

	#4th line: variable names
	var_names = next(readCSV) 
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
	values_line = next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found!")
		return 0
	
	nb_scenes = int(values_line[1])
	# print( "nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		values_line = next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line = next(readCSV)
		# var IDs line
		line = next(readCSV)
	
		# variable initial value
		values_line = next(readCSV)
		values_line.pop(0)
		indVar = 0
		print(values_line)
		for value_init in values_line:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line = next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line = next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp 
			indVar += 1

		print("Nb scenes %d scene ID %s input_file_name %s start-end %s %s\n\n" % (nb_scenes, scene_ID, val_init["input_file_name"], val_init["position"], val_fin["position"]))

		##################################################################
		# VIDEO POSTERIZATION
		##################################################################

		# from video to images
		# creates the directories in which the PNG files are saved
		if( not os.path.isdir(val_init["png_output_directory"]) ):
			try:
				os.mkdir(val_init["png_output_directory"])
			except OSError:
				print ("Creation of the directory %s failed" % val_init["png_output_directory"])
			else:
				print ("Successfully created the directory %s " % val_init["png_output_directory"])
		
		full_input_file_name = val_init["input_directory"] + val_init["input_file_name"]
		full_png_output_file_name = val_init["png_output_directory"] + val_init["png_output_file_name"]
		start_time = val_init["position"]
		end_time = val_fin["position"]
		fps = force_num(val_init["fps"])

		# ffmpeg conversion temporarily commented
		print( "ffmpeg -i "+full_input_file_name+" -ss "+start_time+" -to "+end_time+" -r "+str(fps)+" "+full_png_output_file_name+"_%4d.png")
		os.system( "ffmpeg -i "+full_input_file_name+" -ss "+start_time+" -to "+end_time+" -r "+str(fps)+" "+full_png_output_file_name+"_%4d.png")

		#  	 the images from PNG files
		dirListing = os.listdir(val_init["png_output_directory"])
		nb_files = len(dirListing)
		# nb_files = 3
		print("Number of files ", nb_files)

		fade_in_length = force_num(val_init["fade_in_length"])
		fade_out_length = force_num(val_init["fade_out_length"])

		# image composition: generally addtion of a mask above the initial video to make a zone such as rock or sky in a uniform color
		# creates the directories in which the paths files are saved
		# if ( !-d "movie_composed_PNGs/:output_file_name_wo_extension" ) :
		#		 mkdir "movie_composed_PNGs/:output_file_name_wo_extension" or die "Failed to create path: movie_composed_PNGs/:output_file_name_wo_extension"
		# 
		# for(i = 1	i <= nb_files	i++) :
		#		 count = "%04d", i
		#		 # movie_PATHs/:output_file_name_wo_extension/:output_file_name_wo_extension_"+str(count)+".path
		#		 if(composition_function == OVERLAY) :
		#			 print "composite :composition_file movie_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_"+str(count)+".png movie_composed_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_composed_"+str(count)+".png\n"
		#			 os.system("composite :composition_file movie_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_"+str(count)+".png movie_composed_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_composed_"+str(count)+".png")
		#		 
		#		 elif(composition_function == OVERLAY_INVERT) :
		#			 print "composite :composition_file movie_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_"+str(count)+".png tmp/overlaid.png\n"
		#			 os.system("composite :composition_file movie_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_"+str(count)+".png tmp/overlaid.png")
		#			 print "convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_composed_"+str(count)+".png\n"
		#			 os.system("convert tmp/overlaid.png -channel RGB -negate movie_composed_PNGs/:output_file_name_wo_extension/:output_file_name_wo_extension_composed_"+str(count)+".png")
		#		 
		# 

		# transformation of the video into a vector video of SVG files
		# creates the directories in which the paths SVG files are saved
		if( not os.path.isdir(val_init["svg_output_directory"]) ):
			try:
				os.mkdir(val_init["svg_output_directory"])
			except OSError:
				print ("Creation of the directory %s failed" % val_init["svg_output_directory"])
			else:
				print ("Successfully created the directory %s " % val_init["svg_output_directory"])
		
		nb_layers = force_num(val_init["nb_posterization_layers"])
		tmp_dir = val_init["tmp_directory"]
		full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"]
		for indImage in range(1, nb_files + 1, 1) :
			count = "%04d" % indImage

			# possible image dimming (eg for fade in/out)
			dimming = 1.0
			if(fade_in_length > 0 and count <= (fade_in_length * fps)) :
				dimming = (count - 1)/(fade_in_length * fps)
			elif(fade_out_length > 0 and count >=	(nb_files - (fade_out_length * fps))) :
				dimming = (nb_files - count)/(fade_out_length * fps)
						#clamping
			if(dimming > 1.0) :
				dimming = 1.0
			elif(dimming < 0.0) :
				dimming = 0.0

			vv_posterize_arguments = ["--dimming", dimming, \
				"-o", full_svg_output_file_name+"_"+str(count)+".svg", \
				"-i", full_png_output_file_name+"_"+str(count)+".png", \
				"--tmp", tmp_dir]

			if(val_init["image_mode"] == "color" or val_init["image_mode"] == "wb"):
				vv_posterize_arguments.extend(["--image_mode", val_init["image_mode"]])

			# predefined color map
			if(val_init["color_map"] != None and val_init["color_map"] != "auto"):
				vv_posterize_arguments.extend(["--color_map", val_init["color_map"], \
					"--nb-layers", nb_layers])
			# color map deduced from the quantization of the first image in the movie
			# and kept all along the movie for every frame
			else:
				# first image used to define the color map
				# if the number of colors in the color map does not correspond to the 
				# number of layers in the scenario file, the number of layer is adapted
				# however it can happen that by relaunching the extraction of the frames
				# and the quantization of the first image, that the required number is obtained	
				if(indImage == 1):
					countMidImage = "%04d" % int(nb_files/2)
					print("Build color map from image number ", countMidImage)
					print("convert "+full_png_output_file_name+"_"+str(countMidImage)+".png -quantize YUV -dither None -colors "+str(nb_layers)+" "+tmp_dir+"/color_map.png")
					os.system("convert "+full_png_output_file_name+"_"+str(countMidImage)+".png -quantize YUV -dither None -colors "+str(nb_layers)+" "+tmp_dir+"/color_map.png")
					# generate color table
					os.system( "convert "+tmp_dir+"/color_map.png -format %c -depth 8 histogram:info:->"+tmp_dir+"/colortable.txt")

					# extraction of the nb_layers colors from the color table
					try:
						FILEin = open(tmp_dir+"/colortable.txt", "rt")
					except IOError:
						print("File not found :", tmp_dir+"/colortable.txt", " or path is incorrect")
					line = FILEin.readline()
					line = line.rstrip()
					color_list = []
					while(line):
						search_result = re.search(r' (\#[0-9A-Fa-f]{6}) ', line)
						if(search_result != None) :
							color_list.append(search_result.group(1))
						else :
							print("Color not found in color table:", line)
							sys.exit(2)
						line = FILEin.readline()
						line = line.rstrip()
					if(len(color_list) != nb_layers) :
						print("Color table unexpected size:", len(color_list), "instead of ", nb_layers)
						nb_layers = len(color_list)
					# memorizes the color table
					os.system( "cp "+tmp_dir+"/colortable.txt "+tmp_dir+"/colortable_"+str(int(nb_layers))+"_"+val_init["svg_output_file_name"]+".txt")						
					FILEin.close()

				# used for all the images from the movie
				vv_posterize_arguments.extend(["--color_map", tmp_dir+"/color_map.png", \
					"--nb-layers", nb_layers])

			print("vv_posterize ", vv_posterize_arguments, "\n")
			if( vv_posterize.main(vv_posterize_arguments) == 0) :
				return 0
				
		# /scene line
		values_line = next(readCSV)
		if values_line[0] != "/scene":
			print( "Expected tag \"/scene\" not found!" )
			return 0
		
	# scenes
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
