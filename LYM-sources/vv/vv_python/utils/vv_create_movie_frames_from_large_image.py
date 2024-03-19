#!/usr/bin/python3

# vv_create_movie_frames_from_large_image.py
# this program takes a large horizontal image and progressively crops it fromleft to right to create a video
# the image is resized to frame height

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math

import os
import sys

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

import getopt
import fnmatch

import shutil
from signal import signal, SIGINT

from PIL import Image

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_create_movie_frames_from_large_image.py -i <large_input_image> -w <frame_width> -h <frame_height> -p <nb_pixels_per_frame> -d <frame_directory_name> -o <frame_name>
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	try:
		opts, args = getopt.getopt(main_args,"i:w:h:p:d:o:",["large_input_image=","frame_width=","frame_height=","nb_pixels_per_frame=","frame_directory_name=","frame_name="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--large_input_image"):
			large_input_image = arg
		elif opt in ("-w", "--frame_width"):
			frame_width = arg
		elif opt in ("-h", "--frame_height"):
			frame_height = arg
		elif opt in ("-p", "--nb_pixels_per_frame"):
			nb_pixels_per_frame = arg
		elif opt in ("-d", "--frame_directory_name"):
			frame_directory_name = arg
		elif opt in ("-o", "--frame_name"):
			frame_name = arg
		else:
			assert False, "unhandled option"
			print("option: ", opt)
			print(USAGE)
			sys.exit(2)

	# crates the movie frame directory if it does not exist
	if(not os.path.exists(frame_directory_name)) :
		print("mkdir " + frame_directory_name)
		os.system("mkdir " + frame_directory_name)

	# the jpeg files
	count_images = 0
	im = Image.open(large_input_image)
	w, h = im.size
	print('width: ', w)
	print('height:', h)
	if(w > 0 and h > 0) :
		scale = 1080.0/float(h)
		crop_size_x = 1920
		crop_size_y = 1080
		crop_offset_x = 0
		crop_offset_y = 0
		scaled_width = w * scale
		nb_frames = scaled_width / float(nb_pixels_per_frame)
		print('nb_frames:', nb_frames)

		for indFrame in range(int(nb_frames)) :
			frame_no = "_%03d" % indFrame
			# scaling and conversion to dds
			frame_file_name_png = os.path.join(frame_directory_name, frame_name+frame_no+".png")
			print("convert "+large_input_image+" -resize "+str(scaled_width)+"x"+str(1080)+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background  " + frame_file_name_png)
			os.system("convert "+large_input_image+" -resize "+str(scaled_width)+"x"+str(1080)+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background  " + frame_file_name_png)

			crop_offset_x = crop_offset_x + nb_pixels_per_frame

			if(indFrame >= 6060):
				return 1

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main(sys.argv[1:])
