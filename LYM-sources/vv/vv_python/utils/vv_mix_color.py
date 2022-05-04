#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import re
import sys
import math

import colorsys

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

def mix_color(local_initial_color, local_target_color, local_percentage) :
	# print("initial_color target_color local_percentage:", local_initial_color,  "/", local_target_color, "/", local_percentage)
	initial_color = re.sub('^\#', '', local_initial_color)
	target_color = re.sub('^\#', '', local_target_color)
	target_red = '0x' + target_color[ 0 : 2 ]
	target_green = '0x' + target_color[ 2 : 4 ]
	target_blue = '0x' + target_color[ 4 : 6 ]
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	mix_red = int((1 - local_percentage) *  int(initial_red, 0) + local_percentage *  int(target_red, 0))
	mix_green = int((1 - local_percentage) *  int(initial_green, 0) + local_percentage *  int(target_green, 0))
	mix_blue = int((1 - local_percentage) *  int(initial_blue, 0) + local_percentage *  int(target_blue, 0))
	mix_red = "%02x" % mix_red
	mix_green = "%02x" % mix_green
	mix_blue = "%02x" % mix_blue
	mixed_color = '#' + mix_red.upper() + mix_green.upper() + mix_blue.upper()
	print(mixed_color)

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	renumber_files('/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/movie_SVGs/Annika_Rebirth_all/','/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/movie_SVGs/')
