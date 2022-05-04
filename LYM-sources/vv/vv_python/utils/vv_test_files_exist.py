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

def main():
	#  	 the images from PNG files
	dir_name = "/mnt/c/sync.com/Sync/LYM-videos-sources/LYM_Isskjutning_2020/SVG-movies/SVG_movie_PNGs/camera-stroll/"

	for i in range(1, 15508, 1) :
		count = "%05d" % i
		file_name = os.path.join(dir_name, "camera-stroll_" + count + ".png")
		# print("test " + file_name )
		if not os.path.isfile(file_name) :
			print ("File "+file_name+" does not exist")
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
