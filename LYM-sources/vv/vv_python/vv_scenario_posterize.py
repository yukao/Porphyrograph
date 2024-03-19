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
from vv_lib import interpolate

def handler(signal_received, frame) :
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
def main(main_args) :
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
	val_interp = dict()
	interp = dict()

	with_piecewise_curve = False

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
		if opt in ("-i", "--inputfile") :
			scenario_file_name = arg
		else:
			assert False, "unhandled option"
	# print('Input scenario file is ', scenario_file_name)

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
		print("File not found :", scenario_file_name, " or path is incorrect")

	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')

	line = next(readCSV) 
	# print("line1 ", line)
	line = next(readCSV) 
	# print("line2 ", line)

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
		print("Expected tag \"scenario\" not found!")
		return 0
	
	nb_scenes = int(values_line[1])
	# print("nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		values_line = next(readCSV)
		if values_line[0] != "scene":
			print("Expected tag \"scene\" not found!")
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
		# print(values_line)
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

		# /scene line
		values_line = next(readCSV)
		if values_line[0] != "/scene":
			print("Expected tag \"/scene\" not found!")
			return 0
		
		# if ind_scene != 1 :
		# 	continue

		print("Nb scenes %d scene ID %s input_file_name %s start-end %s %s\n\n" % (nb_scenes, scene_ID, val_init["input_file_name"], val_init["position"], val_fin["position"]))

		##################################################################
		# VIDEO POSTERIZATION
		##################################################################

		# from video to images
		# creates the directories in which the PNG files are saved
		if(not os.path.isdir(val_init["png_output_directory"])) :
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
		if "with_piecewise_curve" in val_init :
			with_piecewise_curve = (val_init["with_piecewise_curve"] == "True")
		print("with_piecewise_curve: ", with_piecewise_curve)

		# Get last 3 character
		file_extension = full_input_file_name[-3:]
		print("file name:", full_input_file_name, " ext:", file_extension)
		if(file_extension == "mp4") :
			# ffmpeg conversion 
			print("ffmpeg -i "+" -ss "+start_time+" -to "+end_time+" -r "+str(fps)+" "+full_png_output_file_name+"_%5d.png")
			os.system("ffmpeg -i "+full_input_file_name+" -ss "+start_time+" -to "+end_time+" -r "+str(fps)+" "+full_png_output_file_name+"_%5d.png")

		fade_in_length = force_num(val_init["fade_in_length"])
		fade_out_length = force_num(val_init["fade_out_length"])

		# transformation of the video into a vector video of SVG files
		# creates the directories in which the paths SVG files are saved
		if(not os.path.isdir(val_init["svg_output_directory"])) :
			try:
				os.mkdir(val_init["svg_output_directory"])
			except OSError:
				print ("Creation of the directory %s failed" % val_init["svg_output_directory"])
			else:
				print ("Successfully created the directory %s " % val_init["svg_output_directory"])
		
		nb_layers = int(force_num(val_init["nb_posterization_layers"]))

		tmp_dir = val_init["tmp_directory"]
		print("tmp directory", tmp_dir)
		full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"]

		nb_files = 0
		if(file_extension == "mp4") :
			# count the images from PNG files
			dirListing = os.listdir(val_init["png_output_directory"])
			nb_files = int(len(dirListing))
		else :
			if(file_extension == "png") :
				start_time_list = start_time.split(":")
				end_time_list = end_time.split(":")
				start_time_sec = int(start_time_list[0]) * 3600 + int(start_time_list[1]) * 60 + int(start_time_list[2])
				end_time_sec = int(end_time_list[0]) * 3600 + int(end_time_list[1]) * 60 + int(end_time_list[2])
				duration_sec = end_time_sec - start_time_sec
				nb_files = int(duration_sec * fps)

		print("Number of files ", nb_files)
		for indImage in range(1, nb_files + 1, 1) :
			count = "%05d" % indImage

			# if indImage > 3 :
			# 	return 0
			if indImage <= 58 :
				continue

			scene_percentage = 0.0
			if(nb_files > 1) :
				scene_percentage = float(indImage - 1) / (nb_files - 1)
			scene_percentage = clamp(scene_percentage)

			for var_name in var_names :
				# print(var_name)
				val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)

			# possible image dimming (eg for fade in/out)
			dimming = 1.0
			if(fade_in_length > 0 and indImage <= (fade_in_length * fps)) :
				dimming = clamp(float(indImage - 1)/(fade_in_length * fps))
			elif(fade_out_length > 0 and indImage >= (nb_files - (fade_out_length * fps))) :
				dimming = clamp(float(nb_files - indImage)/(fade_out_length * fps))

			input_file_name = ""
			if(file_extension == "mp4") :
				input_file_name = full_png_output_file_name+"_"+str(count)+".png"
			else :
				if(file_extension == "png") :
					input_file_name = full_png_output_file_name+".png"

			vv_posterize_arguments = ["--dimming", dimming, \
				"--with_piecewise_curve", with_piecewise_curve, \
				"-o", full_svg_output_file_name+"_"+str(count)+".svg", \
				"-i", input_file_name, \
				"--tmp", tmp_dir]

			if(val_init["image_mode"] == "color" or val_init["image_mode"] == "wb") :
				vv_posterize_arguments.extend(["--image_mode", val_init["image_mode"]])

			if("invert" in val_interp) :
				vv_posterize_arguments.extend(["--invert", val_interp["invert"]])

			if("crop" in val_interp) :
				vv_posterize_arguments.extend(["--crop", val_interp["crop"]])
			else :
				vv_posterize_arguments.extend(["--crop", "1888x1062+16+9"])

			if("resize" in val_interp) :
				vv_posterize_arguments.extend(["--resize", val_interp["resize"]])
			else :
				vv_posterize_arguments.extend(["--resize", "1920x1080"])

			# color table storage directory
			if(val_init["color_table_directory"] != "None") :
				color_table_directory = val_init["color_table_directory"]
			else :
				color_table_directory = tmp_dir

			# predefined color map
			if(val_init["color_map"] != None and val_init["color_map"] != "auto") :
				vv_posterize_arguments.extend(["--color_map", os.path.join(color_table_directory, val_init["color_map"]), \
					"--nb-layers", nb_layers])
			# color map deduced from the quantization of the first image in the movie
			# and kept all along the movie for every frame
			else:
				# if the number of colors in the color map does not correspond to the 
				# number of layers in the scenario file, the number of layer is adapted
				# however it can happen that by relaunching the extraction of the frames
				# and the quantization of the first image, that the required number is obtained	
				if(indImage == 1) :
					source_image = ""
					if(file_extension == "mp4") :
						countMidImage = "%05d" % int(nb_files/2+1)
						source_image = full_png_output_file_name+"_"+str(countMidImage)+".png"
					else :
						if(file_extension == "png") :
							source_image = input_file_name
					print("Build color table from image ", source_image, "with", nb_layers, "colors")
					nb_required_colors = nb_layers
					while True :
						# print("number of required colors", nb_required_colors)
						# print("convert "+source_image+" -quantize YUV -dither None -colors "+str(nb_required_colors)+" "+os.path.join(tmp_dir,"color_map.png"))
						os.system("convert "+source_image+" -quantize YUV -dither None -colors "+str(nb_required_colors)+" "+os.path.join(tmp_dir,"color_map.png"))
						
						# generate color table
						# print("convert "+os.path.join(tmp_dir,"color_map.png")+" -format %c -depth 8 histogram:info:->"+os.path.join(tmp_dir,"color_table.txt"))
						os.system("convert "+os.path.join(tmp_dir,"color_map.png")+" -format %c -depth 8 histogram:info:->"+os.path.join(tmp_dir,"color_table.txt"))

						# extraction of the nb_layers colors from the color table
						try:
							FILEin = open(tmp_dir+"/color_table.txt", "rt")
						except IOError:
							print("File not found :", tmp_dir+"/color_table.txt", " or path is incorrect")
						line = FILEin.readline()
						line = line.rstrip()
						color_list = []
						while(line) :
							search_result = re.search(r' (\#[0-9A-Fa-f]{6}) ', line)
							if(search_result != None) :
								color_list.append(search_result.group(1))
							else :
								print("Color not found in color table:", line)
								sys.exit(2)
							line = FILEin.readline()
							line = line.rstrip()
						# the parameter -colors for quantization guarantees a maximal number of colors, but it may be less
						# the only way to get the expected number of colors is to make a loop on an increasing number of maximal colors until reaching the required number of colors
						if len(color_list) < nb_layers :
							print("Color table unexpected size:", len(color_list), "instead of ", nb_layers, "retries with", nb_required_colors + 1, "colors")
							nb_required_colors += 1
						elif len(color_list) == nb_layers :
							# success: the quantized image has the exact expected number of colors
							break
						else :
							print("did not succeed to reach the amount of expected colors", nb_layers, "exiting...")
							sys.exit(0)

					# memorizes the color table
					count_layers = "%02d" % int(nb_layers)
					print("memorize color_table "+os.path.join(color_table_directory,"color_table_"+val_init["svg_output_file_name"]+"_"+count_layers+".txt"))					
					os.system("cp "+os.path.join(tmp_dir,"color_table.txt")+" "+os.path.join(color_table_directory,"color_table_"+val_init["svg_output_file_name"]+"_"+count_layers+".txt"))						
					FILEin.close()

				# used for all the images from the movie when no predefined color map is provided
				vv_posterize_arguments.extend(["--color_map", os.path.join(tmp_dir,"color_map.png"), \
					"--nb-layers", nb_layers])

			print("vv_posterize ", val_init["png_output_file_name"]+"_"+str(count)+".png", "->", val_init["svg_output_file_name"]+"_"+str(count)+".svg")

			# if indImage < 334 :
			# 	continue

			if(vv_posterize.main(vv_posterize_arguments) == 0) :
				return 0
				
	# scenes
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
