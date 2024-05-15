#!/usr/bin/python3 

# extract_image_urls_from_google_search_page.py
# replaces strings in subdirs

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
import shutil
from signal import signal, SIGINT

import re

from PIL import Image

##################################################################
# MAIN SUB
##################################################################
def main():
	# get image size
	im = Image.open(full_image_file_name)
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
				crop_offset_x = (w - crop_size_x) / 2.0

		# scaling and conversion to dds
		full_image_file_name_dds = image_file_name + "dds"
		print("convert "+full_image_file_name+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background -resize 1920x1080  -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + full_image_file_name_dds)
		os.system("convert "+full_image_file_name+" -crop "+str(crop_size_x)+"x"+str(crop_size_y)+"+"+str(crop_offset_x)+"+"+str(crop_offset_y)+" -background White -alpha Background -resize 1920x1080 -gravity northwest -extent 2048x2048 -define dds:mipmaps=0 -define dds:compression=dxt5 " + full_image_file_name_dds)

