#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math

from os import listdir
from os import system
from os.path import isfile, join
import sys

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

def main():
	#  	 the images from PNG files
	dir_name = "/mnt/c/Users/yukao/OneDrive/Desktop"
	# dir_name = "/mnt/c/Users/yukao/OneDrive/Desktop/small"

	if False :
		for i in range(16845, 16988, 1) :
			count = "%04d" % i
			new_count = "%04d" % (i-157)
			print("cp " + dir_name + "P1050704_" + count + ".svg " + dir_name + "dawn_" + new_count + ".svg" )
			system("cp " + dir_name + "P1050704_" + count + ".svg " + dir_name + "dawn_" + new_count + ".svg" )
		return 1
	if True :
		for f in listdir(dir_name) :
			file_name = join(dir_name, f)
			if isfile(file_name) :
				resized_file_name = re.sub(r'.png', r'_resized.jpg', file_name)
				print("convert " + file_name + " -resize 960x1080  -quality 75 " + resized_file_name )
				system("convert " + file_name + " -resize 960x1080  -quality 75 " + resized_file_name )
	if False :
		system("mkdir ",dirname+"_resized")
		for f in listdir(dir_name) :
			file_name = join(dir_name, f)
			if isfile(file_name) :
				resized_file_name = re.sub(r'.jpg', r'_resized.jpg', file_name)
				print("convert " + file_name + " -resize 66%  -quality 75 " + resized_file_name )
				system("convert " + file_name + " -resize 66%  -quality 75 -gravity center -background black -extent 1920x1080 " + resized_file_name )

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
