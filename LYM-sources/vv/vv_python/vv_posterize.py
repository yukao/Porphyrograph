#!/usr/bin/python3

# vv_posterize.py
# initial posterization of vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
from signal import signal, SIGINT

import re

import vv_layer_compose
from vv_lib import force_num
from vv_lib import color_to_gray_level
from vv_lib import color_to_max_gray
from vv_lib import to_num

from vv_lib import piecewise_curve_file_paths

def handler(signal_received, frame) :
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_posterize.py  --nb-layers <integer>  --dimming <dimming_coefficient> -o <file_name> -i <file_name>
vv_posterize.py  --tmp /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/tmp  --nb-layers 4  --dimming 1 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/VID_20200307_152223/VID_20200307_152223_0300.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_PNGs/VID_20200307_152223/VID_20200307_152223_0300.png
ARGUMENTS:
    --nb_layers: number of layers for posterization
    --dimming: coefficient for dimming the threshold values of the layers
    -o: ouput files
    -i: input files
    --tmp: tmp directory
    --image_mode: (color|wb)
    --color_map: colormap file (eg png)
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	output_format = ""
	nb_layers = 0
	dimming = 0
	# output_file_name_PATHs
	output_file_name_SVGs = ""
	input_file_name = ""
	tmp_dir = ""
	image_mode = "wb"
	crop = "1888x1062+16+9"
	resize = "1920x1080"
	color_map = None
	invert = False


	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","dimming=","with_piecewise_curve=",\
			"tmp=","image_mode=","color_map=","invert=","crop=","resize="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name_SVGs = arg
		elif opt == "--nb-layers":
			nb_layers = int(force_num(arg))
			if(nb_layers < 1) :
				print("Unprocessed number of layers "+nb_layers+", currently only 1 or more layers are accepted")
				sys.exit(2)
		elif opt == "--dimming":
			dimming = force_num(arg)
		elif opt == "--with_piecewise_curve":
			with_piecewise_curve = arg
		elif opt == "--invert":
			invert = (force_num(arg) != 0)
		elif opt == "--tmp":
			tmp_dir = arg
		elif opt == "--crop":
			crop = arg
		elif opt == "--resize":
			resize = arg
		elif opt == "--image_mode":
			image_mode = arg
			if(image_mode != "color" and image_mode != "wb") :
				print("image mmode should be either \"color\" or \"wb\"")
				sys.exit(2)
		elif opt == "--color_map":
			color_map = arg
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)	

	################################################
	# svg image size
	dimensions = resize.split("x")
	width = 2 * to_num(dimensions[0])
	height = 2 * to_num(dimensions[1])
	# print("w x h", width, height)

	################################################
	# possible color inversion
	if(invert == True) :
		os.system("convert "+input_file_name+" -negate "+os.path.join(tmp_dir, "inverted.png"))
		input_file_name = os.path.join(tmp_dir, "inverted.png")

	################################################
	# IMAGE QUANTIZATION (COLORS)
	if(image_mode == "color") :
		# extraction of the nb_layers colors from the color table
		try:
			FILEin = open(os.path.join(tmp_dir, "color_table.txt"), "rt")
		except IOError:
			print("File not found :", os.path.join(tmp_dir, "color_table.txt"), " or path is incorrect")

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
		if(len(color_list) != nb_layers) :
			print("Color table unexpected size:", len(color_list), "instead of ", nb_layers)
			sys.exit(2)
		# else :
		# 	print("Color table expected size:", nb_layers)
		FILEin.close()

		# possible dimming towards the brightest color
		if(dimming != 1.0) :
			max_brightness = color_to_max_gray(color_list[-1])
			brightness = (1.0 - dimming) * (max_brightness * (1 + 1.0/(nb_layers-1)))  * 100.0
			# print("color_list_max color_list", color_list[-1], color_list)
			# print("max_brightness brightness dimming", max_brightness, brightness, dimming)
			os.system("convert "+input_file_name+" -brightness-contrast +"+str(brightness)+" "+os.path.join(tmp_dir, "dimmed.png"))
			new_input_file_name = os.path.join(tmp_dir, "dimmed.png")
		else :
			new_input_file_name = input_file_name

		# converts another image with the color map of a first one (for color coherence within movie shots)
		if(color_map != None) :
			# print("color map exists convert "+new_input_file_name+" -quantize YUV -dither None -colors "+str(nb_layers)+" -remap "+color_map+" "+os.path.join(tmp_dir, "quant.png"))
			os.system("convert "+new_input_file_name+" -dither None -colors "+str(nb_layers)+" -remap "+color_map+" "+os.path.join(tmp_dir, "quant.png"))
		else:
			# print("convert "+new_input_file_name+" -quantize YUV -dither None -colors "+str(nb_layers)+" "+os.path.join(tmp_dir, "quant.png"))
			os.system("convert "+new_input_file_name+" -quantize YUV -dither None -colors "+str(nb_layers)+" "+os.path.join(tmp_dir, "quant.png"))
			# generate color table
			os.system("color map recalculated convert "+os.path.join(tmp_dir, "quant.png")+" -format %c -depth 8 histogram:info:->"+os.path.join(tmp_dir, "color_table.txt"))
			os.system("convert "+os.path.join(tmp_dir, "quant.png")+" -format %c -depth 8 histogram:info:->"+os.path.join(tmp_dir, "color_table.txt"))

		# separate the image into nb_layers masks that correspond to each color
		# from the table and the brightest colors
		# colors are retrieved from the table ranged from the darkest to the brightest
		# creates BW images for further tracing with potrace + recolorization afterwards
		# each mask corresponds to the current color and all the darker ones
		# the traced images will be layered from the brightest (the largest one because showing all colors, normally covering the whole image)
		# to the darkest (the smallest one because only showing its color) (see lim1-4-quant-YUV-nodith.xcf)
		for layerNo in range(nb_layers) :
			ind_color = 0
			for single_color in color_list:
				if(ind_color == 0) :
					command_line = "convert "+os.path.join(tmp_dir, "quant.png")+" "
				else :
					command_line = command_line + "miff:- | convert - "
				if(ind_color <= layerNo) :
					command_line = command_line + " -fill black -opaque '" + single_color + "' "
				else:
					command_line = command_line + " -fill white -opaque '" + single_color + "' "
				ind_color += 1
			command_line = command_line + os.path.join(tmp_dir, "quant"+str(layerNo+1)+".png")+" "
			# print(command_line)
			os.system(command_line)

		# sys.exit(2)

	################################################
	# IMAGE RESIZE (COLORS)
	# eliminates borders and converts to raw forman bmp
	if(image_mode == "color") :
		for layerNo in range(nb_layers) :
			os.system("convert "+os.path.join(tmp_dir, "quant"+str(layerNo+1)+".png")+" -crop "+crop+" -resize "+resize+" "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".bmp"))
			# print("convert "+tmp_dir+"/quant"+str(layerNo+1)+".png -crop 1888x1062+16+9 -resize 1920x1080 "+tmp_dir+"/tmp"+str(layerNo+1)+".bmp")

	################################################
	# IMAGE RESIZE (GRAY LEVELS)
	# eliminates borders and converts to raw forman bmp
	elif(image_mode == "wb") :
		os.system("convert "+input_file_name+" -crop "+crop+" -resize "+resize+" "+os.path.join(tmp_dir, "tmp.bmp"))
		print("convert "+input_file_name+" -crop "+crop+" -resize "+resize+" "+os.path.join(tmp_dir, "tmp.bmp"))

	################################################
	# TRACING: PATHS GENERATION (GRAY LEVELS)
	if(image_mode == "wb") :
		interval = dimming / (nb_layers + 1)
		for layerNo in range(nb_layers) :
			indLayer = nb_layers - 1 - layerNo
			threshold = (indLayer + 1) * interval
			# -t n means that the pixel darker than this value are taken
			# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM).
			os.system("mkbitmap "+os.path.join(tmp_dir, "tmp.bmp")+" -o "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".pbm")+" -x -t "+str(threshold)+" -s 2")
			os.system("potrace "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".pbm")+" -o "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".svg")+"  --svg -t 24 -O 2  --flat")

		input_files = ""
		for layerNo in range(nb_layers) :
			input_files = input_files + os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".svg")+" "

		colors = "#000000"
		color_interval = int(255.0/nb_layers)
		for layerNo in range(1, nb_layers, 1) :
			gray_level = layerNo * color_interval
			new_gray = "%02x" % gray_level
			new_color = '#' + new_gray + new_gray + new_gray
			colors = new_color + " " + colors

		if with_piecewise_curve :
			if(vv_layer_compose.main(["--output-format", "SVG", \
				"--nb-files", nb_layers, \
				"-o", os.path.join(tmp_dir, "tmp-composed.svg"), \
				"-i", input_files, \
				"-c", colors, \
				"--width", width, \
				"--height", height, \
				"--add_id", True, \
				"--fill_and_stroke_from_group", True]) == 0) :
				return 0
			print("Composed SVG file normalization ->", output_file_name_SVGs)
			piecewise_curve_file_paths(os.path.join(tmp_dir, "tmp-composed.svg"), output_file_name_SVGs)
		else :
			if(vv_layer_compose.main(["--output-format", "SVG", \
				"--nb-files", nb_layers, \
				"-o", output_file_name_SVGs, \
				"-i", input_files, \
				"--width", width, \
				"--height", height, \
				"-c", colors ]) == 0) :
				return 0

	################################################
	# TRACING: PATHS GENERATION (COLORS)
	elif(image_mode == "color") :
		for layerNo in range(nb_layers) :
			os.system("mkbitmap "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".bmp")+" -o "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".pbm")+" -x -t 0.5 -s 2")
			os.system("potrace "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".pbm")+" -o "+os.path.join(tmp_dir, "tmp"+str(layerNo+1)+".svg")+"  --svg -t 24 -O 2  --flat")

		input_files = ""
		for layerNo in range(nb_layers) :
			input_files = tmp_dir+"/tmp"+str(layerNo+1)+".svg " + input_files

		rev_color_list = color_list
		rev_color_list.reverse()
		separator = ' '
		colors = separator.join(map(str, rev_color_list))

		if with_piecewise_curve :
			if(vv_layer_compose.main(["--output-format", "SVG", \
				"--nb-files", nb_layers, \
				"-o", os.path.join(tmp_dir, "tmp-composed.svg"), \
				"-i", input_files, \
				"-c", colors, \
				"--width", width, \
				"--height", height, \
				"--add_id", True, \
				"--fill_and_stroke_from_group", True]) == 0) :
				return 0
			print("composed SVG file normalization ->", output_file_name_SVGs)
			piecewise_curve_file_paths(os.path.join(tmp_dir, "tmp-composed.svg"), output_file_name_SVGs)
		else :
			if(vv_layer_compose.main(["--output-format", "SVG", \
				"--nb-files", nb_layers, \
				"-o", output_file_name_SVGs, \
				"-i", input_files, \
				"--width", width, \
				"--height", height, \
				"-c", colors ]) == 0) :
				return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
