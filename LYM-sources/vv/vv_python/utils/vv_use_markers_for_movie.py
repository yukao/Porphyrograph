#!/usr/bin/python3 

# vv_use_markers_for_movie.py
# use markers to replace frames by other images

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os


import csv
import re
import getopt
import sys
import random
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_use_markers_for_movie.py -i marker_file -w war_directory -W nb_war_images -f factory_directory -F nb_factory_images -e empty_frame -s storage_dir_for_drawing_frames -t total_frame_number
ARGUMENTS:
	-i marker_file -w war_directory -W nb_war_images -f factory_directory -F nb_factory_images -e empty_frame -s storage_dir_for_drawing_frames -t total_frame_number
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	try:
		opts, args = getopt.getopt(main_args,"i:w:W:f:F:e:s:t:",["marker_file=","war_directory=","nb_war_images=","factory_directory=","nb_factory_images=","empty_frame=","storage_dir_for_drawing_frames=","total_frame_number="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--marker_file"):
			marker_file = arg
		elif opt in ("-w", "--war_directory"):
			war_directory = arg
		elif opt in ("-W", "--nb_war_images"):
			nb_war_images = arg
		elif opt in ("-f", "--factory_directory"):
			factory_directory = arg
		elif opt in ("-F", "--nb_factory_images"):
			nb_factory_images = arg
		elif opt in ("-e", "--empty_frame"):
			empty_frame = arg
		elif opt in ("-s", "--storage_dir_for_drawing_frames"):
			storage_dir_for_drawing_frames = arg
		elif opt in ("-t", "--total_frame_number"):
			total_frame_number = arg
		else:
			assert False, "unhandled option"
			print("option: ", opt)
			print(USAGE)
			sys.exit(2)

	##################################################################
	# READING THE MARKER POSITIONS
	##################################################################
	try:
		FILEin = open(marker_file, "rt")
	except IOError:
			print("File not found :", marker_file, " or path is incorrect")
	csv_reader = csv.reader(FILEin)
	last_marker_frame_no = -1
	for idx, row in enumerate(csv_reader):
		# marker
		(marker_no,mn,sec,frame,marker_frame_no) = row
		marker_frame_no = int(marker_frame_no)
		if(marker_frame_no <= last_marker_frame_no):
			marker_frame_no = last_marker_frame_no + 1
		print(marker_no,mn,sec,frame,marker_frame_no)
		for indFrame in range(last_marker_frame_no + 1, marker_frame_no) :
			frame_no_string = "_%03d" % indFrame
			os.system("cp -f "+empty_frame+" "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+frame_no_string+".png"))
		# factory at the beginning + few wars, and the reverse at the end
		percentage = float(marker_frame_no)/float(total_frame_number)
		print(percentage)
		if(random.random() > percentage):
			# select factory image
			ind_image = (int(random.random() * nb_factory_images) % nb_factory_images) + 1
			image_no = "_%02d" % ind_image
			image_file_name = os.path.join(factory_directory,"factory"+image_no+".png")
		else:
			# select war image
			ind_image = (int(random.random() * nb_war_images) % nb_war_images) + 1
			image_no = "_%02d" % ind_image
			image_file_name = os.path.join(war_directory,"war"+image_no+".png")
		# first frame opaque
		marker_frame_no_string = "_%03d" % (marker_frame_no)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate -channel A -evaluate multiply 0.25 +channel "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 1)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate -channel A -evaluate multiply 0.5 +channel "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 2)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate -channel A -evaluate multiply 0.75 +channel "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 3)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 4)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 5)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 6)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 7)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate -channel A -evaluate multiply 0.66 +channel "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		marker_frame_no_string = "_%03d" % (marker_frame_no + 8)
		os.system("convert "+image_file_name+"  -resize 1920x1080 -colorspace RGB -channel RGB -negate -channel A -evaluate multiply 0.33 +channel "+os.path.join(storage_dir_for_drawing_frames,"drawing_frame"+marker_frame_no_string+".png"))
		last_marker_frame_no = marker_frame_no + 8

		# if(idx == 5):
		# 	return 1
	
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
