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

def main():
	#  	 the images from PNG files
	dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_transformed_SVGs/dawn/"

	for i in range(16845, 16988, 1) :
		count = "%04d" % i
		new_count = "%04d" % (i-157)
		print("cp " + dir_name + "P1050704_" + count + ".svg " + dir_name + "dawn_" + new_count + ".svg" )
		os.system("cp " + dir_name + "P1050704_" + count + ".svg " + dir_name + "dawn_" + new_count + ".svg" )
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
