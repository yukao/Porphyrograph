#!/usr/bin/python3 

# vv_png_to_dds.py
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

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

import getopt

def handler(signal_received, frame) :
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_png_to_dds.py -i inputdir -o outputdir
ARGUMENTS:
	 -i: input dir
	 -o: output dir
'''

##################################################################
# MAIN SUB
##################################################################
def main(argv) :
	#  	 the images from PNG files
	nb_files = 0
	# source_dir_name = "/mnt/e/LYM-videos-sources/YN_Argenteuil_2023/FeteConservatoire_DJJC_2023/SVG_movie_PNGs/"
	# target_dir_name = "/mnt/e/LYM-videos-sources/YN_Argenteuil_2023/FeteConservatoire_DJJC_2023/SVG_movie_DDSs/"

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(argv,"hi:o:",["inputfile=","outputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt == '-h':
			print (USAGE)
			sys.exit()
		elif opt in ("-i", "--inputdir") :
			source_dir_name = arg
		elif opt in ("-o", "--outputdir") :
			target_dir_name = arg
		else:
			assert False, "unhandled option"

	# print ('Input dir is ', source_dir_name)
	# print ('Output dir is ', target_dir_name)
	# return

	for d in listdir(source_dir_name) :
		compressed_d_name = re.sub(r'_', r'_clip_', d)
		system("mkdir " + join(target_dir_name, compressed_d_name) )
		print("mkdir " + join(target_dir_name, compressed_d_name) )
		for f in listdir(join(source_dir_name, d)) :
			file_name = join(join(source_dir_name, d), f)
			compressed_f_name = re.sub(r'.png', r'.dds', f)
			compressed_file_name = join(join(target_dir_name, compressed_d_name), compressed_f_name)
			if isfile(file_name) :
				system("convert " + file_name + " -resize 1820x1024  -background Black -gravity northwest -extent 2048x1024 -define dds:mipmaps=0 -define dds:compression=dxt5 " + compressed_file_name )
				print("convert " + file_name + " -resize 1820x1024  -background Black -gravity northwest -extent 2048x1024 -define dds:mipmaps=0 -define dds:mipmaps=0 -define dds:compression=dxt5 " + compressed_file_name )
				nb_files += 1
				# if(nb_files > 10) :
				# 	return

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main(sys.argv[1:])