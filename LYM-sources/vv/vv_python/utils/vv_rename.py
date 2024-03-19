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
	dir_ini_name = "/mnt/e/LYM-videos-sources/YN_Rivets_2021/Videos-rivets/Crain_video_pont_de_photos/SVG_movies/movie_PNGs/pont_crain/"
	dir_name = "/mnt/e/LYM-videos-sources/YN_Rivets_2021/Videos-rivets/Crain_video_pont_de_photos/SVG_movies/movie_PNGs/pont_crain/"

	for i in range(0, 6060, 1) :
		count = "%03d" % i
		new_count = "%05d" % i
		print("mv -f " + dir_ini_name + "pont_crain_" + count + ".png " + dir_name + "pont_crain_" + count + ".png " )
		os.system("mv -f " + dir_ini_name + "pont_crain_" + count + ".png " + dir_name + "pont_crain_" + new_count + ".png " )
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main(sys.argv[1:])
