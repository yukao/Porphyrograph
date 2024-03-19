#!/usr/bin/python3

# vv_lib.py
# utilities for vector video

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

USAGE = '''usage: create_diaporama_directories.py -i <dds_images_dir_name>
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	formatting = "crop"
	try:
		opts, args = getopt.getopt(main_args,"i:o:f:",["input_images_dir_name=","diaporama_output_dir_name=","formatting="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--jpeg_images_dir_name"):
			input_images_dir_name = arg
		elif opt in ("-o", "--diaporama_output_dir_name"):
			diaporama_output_dir_name = arg
		elif opt in ("-f", "--formatting"):
			formatting = arg
		else:
			assert False, "unhandled option"
			print("option: ", opt)
			print(USAGE)
			sys.exit(2)

	# crates the diaporama directory if it does not exist
	if(not os.path.exists(diaporama_output_dir_name)) :
		print("mkdir " + diaporama_output_dir_name)
		os.system("mkdir " + diaporama_output_dir_name)

	# the jpeg files
	print("input_images_dir_name " + input_images_dir_name)
	count_images = 0
	for root, dirs, files in os.walk(input_images_dir_name):
			for file_name in files:
				print ("file_name", file_name)
				if(re.search(r'\.jpeg$', file_name) != None or re.search(r'\.jpg$', file_name) != None) :
					# get image size
					im = Image.open(os.path.join(input_images_dir_name, file_name))
					w, h = im.size
					print('width: ', w)
					print('height:', h)

					if(w > 0 and h > 0) :
						target_ratio = float(1920) / float(1080)
						current_ratio = float(w)/float(h)
						crop_size_x = w
						crop_size_y = h
						crop_offset_x = 0
						crop_offset_y = 0
						if(formatting == "crop") :
							if(current_ratio < target_ratio) :
								crop_size_x = w
								crop_size_y = w / target_ratio
								crop_offset_x = 0
								crop_offset_y = (h - crop_size_y) / 2.0
							else :
								if(current_ratio > target_ratio) :
									crop_size_y = h
									crop_size_x = h * target_ratio
									crop_offset_y = 0
									crop_offset_x = (w - crop_size_x) / 2.
							# scaling and conversion to dds
							file_name_dds = os.path.splitext(file_name)[0]+'.dds'
							print("convert "+os.path.join(input_images_dir_name, file_name)+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background -resize 1920x1080  -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + os.path.join(input_images_dir_name, file_name_dds))
							os.system("convert "+os.path.join(input_images_dir_name, file_name)+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background -resize 1920x1080 -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + os.path.join(input_images_dir_name, file_name_dds))
							file_name = file_name_dds
						else : # formatting ==  "expand"
							if(current_ratio < target_ratio) :
								new_width = 1080 * target_ratio
								new_height = 1080 
							else :
								new_width = 1920 
								new_height = 1920 / target_ratio 
							# scaling and conversion to dds
							file_name_without_extension = os.path.splitext(file_name)[0]
							countInput_expected_ending = "%03d" % count_images
							if(not file_name_without_extension.endswith(countInput_expected_ending)) :
								file_name_without_extension = file_name_without_extension + "_" + countInput_expected_ending
							file_name_dds = file_name_without_extension+'.dds'
							print("convert "+os.path.join(input_images_dir_name, file_name)+" -resize "+str(new_width)+"x"+str(new_height)+" -background White -alpha Background  -gravity center -extent 1920x1080  -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + os.path.join(input_images_dir_name, file_name_dds))
							os.system("convert "+os.path.join(input_images_dir_name, file_name)+" -resize "+str(new_width)+"x"+str(new_height)+" -background White -alpha Background -gravity center -extent 1920x1080 -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + os.path.join(input_images_dir_name, file_name_dds))
							file_name = file_name_dds


				if(re.search(r'\.dds$', file_name) != None) :
					print ("dds file: ", os.path.join(root, file_name))

					new_file_name = file_name
					new_file_name = new_file_name.replace(".dds", "")
					new_dir_name = os.path.join(diaporama_output_dir_name, new_file_name)

					# crates the image directory if it odes not exist
					if(not os.path.exists(new_dir_name)) :
						print("mkdir " + new_dir_name)
						os.system("mkdir " + new_dir_name)

					# and copies two samples of the current image into this directory
					new_image_name = new_file_name + "_000.dds"
					new_image_name = os.path.join(new_dir_name, new_image_name)
					print("cp -f " + os.path.join(root, file_name) + " " + new_image_name )
					os.system("cp -f " + os.path.join(root, file_name) + " " + new_image_name )
					new_image_name = new_file_name + "_001.dds"
					new_image_name = os.path.join(new_dir_name, new_image_name)
					print("cp -f " + os.path.join(root, file_name) + " " + new_image_name )
					os.system("cp -f " + os.path.join(root, file_name) + " " + new_image_name )
					os.system("rm -f " + os.path.join(root, file_name))

					count_images = count_images + 1

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main(sys.argv[1:])
