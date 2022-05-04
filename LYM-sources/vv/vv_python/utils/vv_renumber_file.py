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

def renumber_files_from_dir(dir_ini, dir_target) :
	ind_file = 1
	for file in os.listdir(dir_ini):
		count3 = "%03d" % ind_file
		# print(dir_ini+file)
		# print(dir_target+'Annika_Rebirth_'+count3)
		print("mkdir "+dir_target+'Annika_Rebirth_'+count3)
		os.system("mkdir "+dir_target+'Annika_Rebirth_'+count3)
		# print(dir_target+'Annika_Rebirth_'+count3+'/Annika_Rebirth_'+count3+'_0001.svg')
		print("mv "+dir_ini+file+" "+dir_target+'Annika_Rebirth_'+count3+'/Annika_Rebirth_'+count3+'_0001.svg')
		os.system("mv "+dir_ini+file+" "+dir_target+'Annika_Rebirth_'+count3+'/Annika_Rebirth_'+count3+'_0001.svg')
		# print("mv -f " + dir_ini_name + "test_" + count + ".png " + dir_name + "dawn_" + new_count + ".png" )
		# os.system("mv -f " + dir_ini_name + "test_" + count + ".png " + dir_name + "dawn_" + new_count + ".png" )
		ind_file += 1

def renumber_files_from_indices(dir_ini) :
	for ind_file in range(6000):
		ind_used = ind_file + 1
		count_ini = "%04d" % (ind_used * 5)
		count_end = "%04d" % (ind_used)
		print("mv "+dir_ini+'silhouette_'+count_ini+'.png '+dir_ini+'silhouette_'+count_end+'.png ')
		os.system("mv "+dir_ini+'silhouette_'+count_ini+'.png '+dir_ini+'silhouette_'+count_end+'.png ')

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	# renumber_files_from_dir('/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/SVG_movie_PNGs/silhouette/','/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/SVG_movie_PNGs/silhouette')

	# Tell Python to run the handler() function when SIGINT is recieved
	renumber_files_from_indices('/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Araknit_2020/SVG_movies/SVG_movie_PNGs/silhouette/')
