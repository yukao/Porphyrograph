#!/usr/bin/python3 

# vv_posterize-images_from_dir.py
# crop and resize images 

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
from signal import signal, SIGINT

import re

import cv2

from os import listdir
from os.path import isfile, join

import sys
sys.path.append('/mnt/c/sync.com/Sync/LYM-sources/vv/vv_python/')
import vv_layer_compose

from vv_lib import piecewise_curve_file_paths

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_posterize-images_from_dir.py -i input_dir_name -o output_jpg_dir_name -t tmp_dir_name
ARGUMENTS:
   -i: input dir
   -o: output dir JPEG
   -t: temp dir
'''

##################################################################
# MAIN SUB
##################################################################
with_piecewise_curve = False

def main(main_args):
	try:
		opts, args = getopt.getopt(main_args,"i:o:O:t:",["input_dir_name=","output_svg_dir_name=","output_jpg_dir_name=","tmp_dir_name=","with_piecewise_curve="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--input_dir_name"):
			input_dir_name = arg
		elif opt in ("-o", "--output_jpg_dir_name"):
			output_jpg_dir_name = arg
		elif opt in ("-t", "--tmp_dir_name"):
			tmp_dir_name = arg
		elif opt in ("--with_piecewise_curve"):
			if arg == "True" or arg == "true" :
				with_piecewise_curve = True
			else :
				with_piecewise_curve = False
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)
	
	ind_file = 1
	for file_name in listdir(input_dir_name) :
		full_file_name = input_dir_name+file_name
		if isfile(full_file_name) :
			if re.search(r'jpg',full_file_name) != None :
				SVG_file_name = re.sub(r'jpg', r'svg', file_name)
				output_file_name_SVG =  os.path.join(tmp_dir_name, "tmp.svg")
				output_file_name_jpg =  os.path.join(output_jpg_dir_name, file_name)
			elif re.search(r'png',full_file_name) != None :
				SVG_file_name = re.sub(r'png', r'svg', file_name)
				output_file_name_SVG =  os.path.join(output_jpg_dir_name, SVG_file_name)

			print("convert "+full_file_name+" -resize 1920x1080 "+tmp_dir_name+"tmp.bmp")
			os.system("convert "+full_file_name+" -resize 1920x1080 "+tmp_dir_name+"tmp.bmp")

			################################################
			# TRACING: PATHS GENERATION (GRAY LEVELS)
			# image with 8 gray levels
			if re.search(r'jpg',full_file_name) != None :
				nb_layers = 8
				dimming = 1
				interval = dimming / (nb_layers + 1)
				for layerNo in range(nb_layers) :
					indLayer = nb_layers - 1 - layerNo
					threshold = (indLayer + 1) * interval
					# -t n means that the pixel darker than this value are taken
					# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM).
					os.system("mkbitmap "+os.path.join(tmp_dir_name, "tmp.bmp")+" -o "+os.path.join(tmp_dir_name, "tmp"+str(layerNo+1)+".pbm")+" -x -t "+str(threshold)+" -s 2")
					os.system("potrace "+os.path.join(tmp_dir_name, "tmp"+str(layerNo+1)+".pbm")+" -o "+os.path.join(tmp_dir_name, "tmp"+str(layerNo+1)+".svg")+"  --svg -t 24 -O 2  --flat")
				input_files = ""
				for layerNo in range(nb_layers) :
					input_files = input_files + os.path.join(tmp_dir_name, "tmp"+str(layerNo+1)+".svg") + " "

				colors = "#000000"
				color_interval = int(255.0/nb_layers)
				for layerNo in range(1, nb_layers, 1) :
					gray_level = layerNo * color_interval
					new_gray = "%02x" % gray_level
					new_color = '#' + new_gray + "00" + "00"
					colors = new_color + " " + colors

				if with_piecewise_curve :
					if(vv_layer_compose.main(["--output-format", "SVG", \
						"--nb-files", nb_layers, \
						"-o", os.path.join(tmp_dir_name, "tmp-composed.svg"), \
						"-i", input_files, \
						"-c", colors ]) == 0) :
						return 0
					piecewise_curve_file_paths(os.path.join(tmp_dir_name, "tmp-composed.svg"), output_file_name_SVG)
				else :
					if(vv_layer_compose.main(["--output-format", "SVG", \
						"--nb-files", nb_layers, \
						"-o", output_file_name_SVG, \
						"-i", input_files, \
						"-c", colors ]) == 0) :
						return 0

			# B/W image (only one layer)
			elif re.search(r'png',full_file_name) != None :
				# only one layer for a 
				print("mkbitmap "+os.path.join(tmp_dir_name, "tmp.bmp")+" -o "+os.path.join(tmp_dir_name, "tmp.pbm")+" -x -t "+str(0.8)+" -s 2")
				os.system("mkbitmap "+os.path.join(tmp_dir_name, "tmp.bmp")+" -o "+os.path.join(tmp_dir_name, "tmp.pbm")+" -x -t "+str(0.8)+" -s 2")
				print("potrace "+os.path.join(tmp_dir_name, "tmp.pbm")+" -o "+os.path.join(tmp_dir_name, "tmp.svg")+" --svg -t 24 -O 2  --flat")
				os.system("potrace "+os.path.join(tmp_dir_name, "tmp.pbm")+" -o "+os.path.join(tmp_dir_name, "tmp.svg")+" --svg -t 24 -O 2  --flat")
				print("layer compose ->", os.path.join(tmp_dir_name, "tmp-composed.svg"))
				if with_piecewise_curve :
					if(vv_layer_compose.main(["--output-format", "SVG", \
						"--nb-files", 1, \
						"-o", os.path.join(tmp_dir_name, "tmp-composed.svg"), \
						"-i", os.path.join(tmp_dir_name, "tmp.svg"), \
						"-c", "#000000", \
						"--add_id", True, \
						"--fill_and_stroke_from_group", True]) == 0) :
						return 0
					piecewise_curve_file_paths(os.path.join(tmp_dir_name, "tmp-composed.svg"), output_file_name_SVG)
				else :
					if(vv_layer_compose.main(["--output-format", "SVG", \
						"--nb-files", 1, \
						"-o", output_file_name_SVG, \
						"-i", os.path.join(tmp_dir_name, "tmp.svg"), \
						"-c", "#000000", \
						"--add_id", True, \
						"--fill_and_stroke_from_group", True]) == 0) :
						return 0

			################################################
			# CONVERSION BACK TO RASTER IMAGE
			# conversion back from svg to png
			if re.search(r'jpg',full_file_name) != None :
				os.system( "inkscape -z -w 1920 -h 1080 --export-area-page -b \\#FF0000 -y 1.0 -e "+str(tmp_dir_name)+"tmp.png "+str(output_file_name_SVG)+" 1> "+str(tmp_dir_name)+"tmp_inkscape_output.txt 2>&1")
				print( "convert "+os.path.join(tmp_dir_name, "tmp.png")+" "+str(output_file_name_jpg))
				os.system( "convert "+os.path.join(tmp_dir_name, "tmp.png")+" "+str(output_file_name_jpg))

			ind_file += 1
			# if(ind_file == 2) :
			# 	sys.exit(2)

	return 1


if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])