#!/usr/bin/python3 

# vv_scenario_rasterize.py
# final scenario for posterization of vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import csv
import os

import getopt
import sys
from signal import signal, SIGINT

from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_rasterize.py -i scenario_file
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

	tmp_png_w = 3840
	tmp_png_h = 2160

	final_png_w = 1920
	final_png_h = 1080

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
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line =	next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin)
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
		
		full_svg_input_file_name = os.path.join(val_init["svg_input_directory"], val_init["svg_input_file_name"])
		full_png_output_file_name = os.path.join(val_init["png_output_directory"], val_init["png_output_file_name"])

		# count the images from SVG files
		dirListing = os.listdir(val_init["svg_input_directory"])
		nb_files = len(dirListing)
		print("Number of files ", nb_files)

		##################################################################
		# SCANS ALL THE SVG FRAMES IN THE MOVIE
		##################################################################
		# transformation of the vector video of SVG files into PNG files
		indImageOutput = val_init["imageOutputIndex"]
		input_step = 1
		if "input_step" in val_init :
			input_step = int(val_init["input_step"])
		output_step = 1
		if "output_step" in val_init :
			output_step = int(val_init["output_step"])
		output_ini = 0
		if "output_ini" in val_init :
			output_ini = int(to_num(val_init["output_ini"]))

		for indImageInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + 1, input_step) :
				countInput = "%05d" % indImageInput
				countOutput = "%05d" % indImageOutput 
				tmp_dir = val_init["tmp_directory"]

				# if(indImageInput > 8) :
				# 	return

				scene_percentage = 0.0
				if(val_fin["imageInputIndex"] != val_init["imageInputIndex"]) :
					scene_percentage = float(indImageInput - val_init["imageInputIndex"]) / (val_fin["imageInputIndex"] - val_init["imageInputIndex"])
				
				scene_percentage = clamp(scene_percentage)

				for var_name in var_names :
					val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
				
				if indImageOutput % output_step == output_ini :
					#input file
					input_file_name = str(full_svg_input_file_name)+"_"+str(countInput)+".svg"
					fin = open(input_file_name, "rt")
					#output file to write the result to
					tmp_svg_output_file_name = os.path.join(tmp_dir, "tmp.svg")
					fout = open(tmp_svg_output_file_name, "wt")
					#for each line in the input file
					for line in fin:
						#read replace the string and write to output file
						fout.write(line.replace('stroke="none"', 'stroke="black" stroke-width="40"'))
					#close input and output files
					fin.close()
					fout.close()

					tmp_png_output_file_name = os.path.join(tmp_dir, "tmp_large.png")
					print ("inkscape/convert: ", input_file_name, "->", val_init["png_output_file_name"]+"_"+str(countOutput)+".png")
					# converts the SVG into a bitmap, larger than the final image for a better quality and to crop the border
					# background color should not be visible because a rectangle covering the whole page as been added in the fill and stroke step
					if "tmp_png_w" in val_interp :
						tmp_png_w = int(val_interp["tmp_png_w"])
					if "tmp_png_h" in val_interp :
						tmp_png_h = int(val_interp["tmp_png_h"])
					print("inkscape -w "+str(tmp_png_w)+" -h "+str(tmp_png_h)+" --export-area-page -b \\" + val_interp["background_color"] + " -y " + str(val_interp["background_opacity"]) + " --export-filename="+tmp_png_output_file_name+" "+tmp_svg_output_file_name+" 1> "+os.path.join(tmp_dir, "tmp_inkscape_output.txt")+" 2>&1")
					os.system( "inkscape -w "+str(tmp_png_w)+" -h "+str(tmp_png_h)+" --export-area-page -b \\" + val_interp["background_color"] + " -y " + str(val_interp["background_opacity"]) + " --export-filename="+tmp_png_output_file_name+" "+tmp_svg_output_file_name+" 1> "+os.path.join(tmp_dir, "tmp_inkscape_output.txt")+" 2>&1")

					# cropping and downscale of the png inmages
					output_file_name = str(full_png_output_file_name)+"_"+str(countOutput)+".png"
					crop_offset_x = int(val_interp["crop_offset_x"])
					crop_offset_y = int(val_interp["crop_offset_y"])
					crop_size_x = int(val_interp["crop_size_x"])
					crop_size_y = int(val_interp["crop_size_y"])
					if "final_png_w" in val_interp :
						final_png_w = int(val_interp["final_png_w"])
					if "final_png_h" in val_interp :
						final_png_h = int(val_interp["final_png_h"])
					# print("convert "+tmp_png_output_file_name+ " -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -resize "+str(final_png_w)+"x"+str(final_png_h)+"! -background white -alpha off -type TrueColor -define png:color-type=2 "+output_file_name)
					os.system("convert "+tmp_png_output_file_name+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -resize "+str(final_png_w)+"x"+str(final_png_h)+"! -background white -alpha off -type TrueColor -define png:color-type=2 "+output_file_name)

					# removes input frame
					# print ("rm: ", str(full_svg_input_file_name)+"_"+str(countInput)+".svg")
					# os.system("rm -f " + str(full_svg_input_file_name)+"_"+str(countInput)+".svg")

				indImageOutput += output_step
		
		# /scene line
		values_line =	next(readCSV)
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
