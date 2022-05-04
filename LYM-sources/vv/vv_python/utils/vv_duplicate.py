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
	dir_name = "/mnt/d/LYM-videos-sources/YN_LaLectureDesPierres_2021/SVG_movies/movie_PNGs/LLDP_022/"
	dir_name2 = "/mnt/d/LYM-videos-sources/YN_LaLectureDesPierres_2021/SVG_movies/movie_PNGs/LLDP_009/"

	for i in range(900, 901, 1) :
		new_count = "%05d" % i
		print("cp " + dir_name + "LLDP_022.png " + dir_name + "LLDP_022_" + new_count + ".png" )
		os.system("cp " + dir_name + "LLDP_022.png " + dir_name + "LLDP_022_" + new_count + ".png" )
		print("cp " + dir_name2 + "LLDP_009.png " + dir_name2 + "LLDP_009_" + new_count + ".png" )
		os.system("cp " + dir_name2 + "LLDP_009.png " + dir_name2 + "LLDP_009_" + new_count + ".png" )
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
