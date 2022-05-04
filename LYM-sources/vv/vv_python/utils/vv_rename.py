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

def main(main_args):
	#  	 the images from PNG files
	dir_ini_name = "/mnt/d/LYM-videos-sources/YN_LaLectureDesPierres_2021/SVG_movies/movie_SVGs/LLDP_054/"
	dir_name = "/mnt/d/LYM-videos-sources/YN_LaLectureDesPierres_2021/SVG_movies/movie_SVGs/LLDP_054/"

	for i in range(1, 451, 1) :
		count = "%05d" % i
		new_count = "%05d" % i
		print("mv -f " + dir_ini_name + "LLDP_054_" + count + "_filled.svg " + dir_name + "LLDP_054_filled_" + new_count + ".svg" )
		os.system("mv -f " + dir_ini_name + "LLDP_054_" + count + "_filled.svg " + dir_name + "LLDP_054_filled_" + new_count + ".svg" )
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main(sys.argv[1:])
