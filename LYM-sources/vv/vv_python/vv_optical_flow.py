#!/usr/bin/python3 

# optical_flow.py
# dense optical flow calculation from  
# https://docs.opencv.org/3.4/d4/dee/tutorial_optical_flow.html

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
from signal import signal, SIGINT

import re

import vv_layer_compose
from vv_lib import force_num

from scipy import signal

import numpy as np
import cv2 as cv

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_posterize.py  --nb-layers <integer>  --dimming <dimming_coefficient> -o <file_name> -i <file_name> 
vv_posterize.py  --tmp /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/tmp  --nb-layers 4  --dimming 1 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/VID_20200307_152223/VID_20200307_152223_0300.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_PNGs/VID_20200307_152223/VID_20200307_152223_0300.png
ARGUMENTS:
'''

DOWN_SCALE = 4
SMOOTHING_WINDOW_SIZE = 5
MAX_IMAGE_COUNT = 1490

resized_height = 0
resized_width = 0
cap = cv.VideoCapture(cv.samples.findFile("../breathing_MRI/breathing_RMI.mp4"))
dim_resized = (0,0)
dim_HD = (0,0)
prvs_resized = None
frame1 = None


##################################################################
# TIRANGULAR SMOOTHING
##################################################################
# up then down, [::-1] means all elements reversed
triangle = np.concatenate((np.arange(SMOOTHING_WINDOW_SIZE + 1), np.arange(SMOOTHING_WINDOW_SIZE)[::-1])) 
def smoothTriangle(data, degree):
	global triangle
	point = data[0:0 + len(triangle)] * triangle
	smoothed = np.sum(point)/np.sum(triangle)
	return smoothed

##################################################################
# OPTICAL FLOW STORAGE
##################################################################
def opticalFlowStorage():
	global resized_height
	global resized_width
	global cap
	global dim_resized
	global prvs_resized
	global frame1

	# creates an empty HSV images of the same size
	hsv = np.zeros_like(frame1)
	hsv = cv.resize(hsv, dim_resized, interpolation = cv.INTER_AREA)
	hsv[...,1] = 255
	indImage = 1
	while(1):
		count = "%04d" % indImage
		ret, frame2 = cap.read()

		if(frame2.size == 0):
			break

		# OF works on three frames the next one
		next = cv.cvtColor(frame2,cv.COLOR_BGR2GRAY)
		next_resized = cv.resize(next, dim_resized, interpolation = cv.INTER_AREA)

		# dense optical flow calculaiton
		flow = cv.calcOpticalFlowFarneback(prvs_resized, next_resized, None, 0.5, 3, 15, 3, 5, 1.2, 0)

		# turns vectors into colors through hsv conversion
		mag, ang = cv.cartToPolar(flow[...,0], flow[...,1])
		hsv[...,0] = ang*180/np.pi/2
		hsv[...,2] = cv.normalize(mag,None,0,255,cv.NORM_MINMAX)
		bgr = cv.cvtColor(hsv,cv.COLOR_HSV2BGR)

		# stores the video frame and the colored version of the optical flow
		print("write color and OF files "+count)
		# stores the non resizedframe
		# cv.imwrite('../breathing_MRI/frames_CV/breathing_RMI_'+count+'.png',frame2)
		# stores a colored version of the optical flow
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_hsv/breathing_RMI_OF_'+count+'.png',bgr)

		# stores the optical flow vectors that will be used to animate the vector images
		flow_1, flow_2 = cv.split(flow); # transforms the CV_32FC2 flow Mat into two CV_32FC1 Mats
		# because exr cannot store 2-channel data
		# stores the optical flow in two separate files
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow/breathing_RMI_OF_1_'+count+'.exr',flow_1)
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow/breathing_RMI_OF_2_'+count+'.exr',flow_2)

		# print("OF storage size ", flow_1.shape," ", flow_2.shape)

		# checks that the reloaded optical flows are identical to storage
		# flow_1_reloaded = cv.imread('../breathing_MRI/optical_flow_data/opticalflow/breathing_RMI_OF_1_'+count+'.exr', cv.IMREAD_UNCHANGED)
		# flow_2_reloaded = cv.imread('../breathing_MRI/optical_flow_data/opticalflow/breathing_RMI_OF_2_'+count+'.exr', cv.IMREAD_UNCHANGED)
		# # loop over the image, pixel by pixel
		# for y in range(0, resized_height):
		# 	for x in range(0, resized_width):
		# 		channel1, channel2 = flow[y, x]
		# 		channel1_reloaded = flow_1_reloaded[y, x]
		# 		channel2_reloaded = flow_2_reloaded[y, x] 
		# 		# print('channels 1 and 2 reloaded channels 1 and 2 ',channel1,' ',channel2,' ',channel1_reloaded,' ',channel2_reloaded)
		# 		if(channel1 != channel1_reloaded):
		# 			print('channels 1 differernt at ', y, ' ', x, ': ', channel1, channel1_reloaded)
		# 		if(channel2 != channel2_reloaded):
		# 			print('channels 2 differernt at ', y, ' ', x, ': ', channel2, channel2_reloaded)

		prvs_resized = next_resized
		indImage += 1
		if(indImage >= MAX_IMAGE_COUNT):
			break

##################################################################
# OPTICAL FLOW SMOOTHING
##################################################################
def opticalFlowSmoothing():
	global resized_height
	global resized_width

	optical_flow_storage_1 = np.zeros((resized_height, resized_width, 2 * SMOOTHING_WINDOW_SIZE + 1))
	optical_flow_storage_2 = np.zeros((resized_height, resized_width, 2 * SMOOTHING_WINDOW_SIZE + 1))
	print("triangle: ", triangle)
	indImage = 1
	while(1):
		count = "%04d" % indImage

		# reloads the optical flow data
		flow_1_reloaded = cv.imread('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow/breathing_RMI_OF_1_'+count+'.exr', cv.IMREAD_UNCHANGED)
		flow_2_reloaded = cv.imread('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow/breathing_RMI_OF_2_'+count+'.exr', cv.IMREAD_UNCHANGED)

		# loop over the image, pixel by pixel
		for y in range(0, resized_height):
			for x in range(0, resized_width):
				optical_flow_storage_1[y,x] = np.roll(optical_flow_storage_1[y,x],-1)
				optical_flow_storage_2[y,x] = np.roll(optical_flow_storage_2[y,x],-1)
				optical_flow_storage_1[y,x,2 * SMOOTHING_WINDOW_SIZE] = flow_1_reloaded[y, x]
				optical_flow_storage_2[y,x,2 * SMOOTHING_WINDOW_SIZE] = flow_2_reloaded[y, x]
				flow_1_reloaded[y, x] = smoothTriangle(optical_flow_storage_1[y,x], SMOOTHING_WINDOW_SIZE)
				flow_2_reloaded[y, x] = smoothTriangle(optical_flow_storage_2[y,x], SMOOTHING_WINDOW_SIZE)

		# stores the smoothed version of the optical flow
		print("write smoothed OF files "+count)
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/breathing_RMI_OF_smooth_1_'+count+'.exr',flow_1_reloaded)
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/breathing_RMI_OF_smooth_2_'+count+'.exr',flow_2_reloaded)

		indImage += 1
		if(indImage >= MAX_IMAGE_COUNT):
			break

##################################################################
# SMOOTHED OPTICAL FLOW TO HSV
##################################################################
def smoothedOpticalFlowDisplay():
	global resized_height
	global resized_width
	global cap
	global dim_resized
	global dim_HD
	global prvs_resized
	global frame1

	# creates an empty HSV images of the same size
	hsv = np.zeros_like(frame1)
	hsv[...,1] = 255
	indImage = 1
	while(1):
		count = "%04d" % indImage

		# reloads the smoothed optical flow data
		flow_1_reloaded = cv.imread('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/breathing_RMI_OF_smooth_1_'+count+'.exr', cv.IMREAD_UNCHANGED)
		flow_2_reloaded = cv.imread('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smooth/breathing_RMI_OF_smooth_2_'+count+'.exr', cv.IMREAD_UNCHANGED)
		flow_reloaded = cv.merge((flow_1_reloaded,flow_2_reloaded));

		# OF works on three frames the next one
		flow_reloaded_HD = cv.resize(flow_reloaded, dim_HD, interpolation = cv.INTER_AREA)

		# turns vectors into colors through hsv conversion
		mag, ang = cv.cartToPolar(flow_reloaded_HD[...,0], flow_reloaded_HD[...,1])
		hsv[...,0] = ang*180/np.pi/2
		hsv[...,2] = cv.normalize(mag,None,0,255,cv.NORM_MINMAX)
		bgr = cv.cvtColor(hsv,cv.COLOR_HSV2BGR)

		# stores the video frame and the colored version of the optical flow
		print("write color of smoothed OF files "+count)
		# stores the non resizedframe
		# cv.imwrite('../breathing_MRI/optical_flow_data/frames_CV/breathing_RMI_'+count+'.png',frame2)
		# stores a colored version of the optical flow
		cv.imwrite('../breathing_MRI/optical_flow_data/breathing_RMI_opticalflow_smoothed_hsv/breathing_RMI_OF_'+count+'.png',bgr)

		indImage += 1
		if(indImage >= MAX_IMAGE_COUNT):
			break

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	global resized_height
	global resized_width
	global dim_resized
	global dim_HD
	global prvs_resized
	global frame1

	ret, frame1 = cap.read()

	# OF works on two frames the previous one (identical to the current one at the beginning)
	prvs = cv.cvtColor(frame1,cv.COLOR_BGR2GRAY)
	# frames are resized due to poor image definition
	dim_HD = (int(prvs.shape[1]), int(prvs.shape[0])) 
	dim_resized = (int(prvs.shape[1] / DOWN_SCALE), int(prvs.shape[0] / DOWN_SCALE)) 
	prvs_resized = cv.resize(prvs, dim_resized, interpolation = cv.INTER_AREA)

	# dimensions of the optical flow data
	print("dimensions of the optical flow data: ", dim_resized)

	# grab the image dimensions
	resized_height = prvs_resized.shape[0]
	resized_width = prvs_resized.shape[1]

	# dimensions of the resized image
	print("dimensions of the resized image: ", resized_width, resized_height)

	################################################
	# OPTICAL FLOW STORAGE
	if(0):
		opticalFlowStorage()

	################################################
	# OPTICAL FLOW SMOOTHING
	if(0):
		opticalFlowSmoothing()

	################################################
	# SMOOTHED OPTICAL FLOW DISPLAY
	if(0):
		smoothedOpticalFlowDisplay()

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
