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
	dir_name = "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/SVG_movies/movie_SVGs/P1050704/"
	dirListing = os.listdir(dir_name)
	nb_files = len(dirListing)
	nb_files = 796
	print("Number of files ", nb_files)

	ind_copy = nb_files + 1
	for i in reversed(range(nb_files)) :
		if(ind_copy >= 1256) :
			count =  "%04d" % (i+1)
			count_from_end =  "%04d" % ind_copy
			print("cp ", dir_name + "P1050704_" + count + ".svg", dir_name + "P1050704_" + count_from_end + ".svg" )
			os.system("cp " + dir_name + "P1050704_" + count + ".svg " + dir_name + "P1050704_" + count_from_end + ".svg" )
		ind_copy += 1
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
