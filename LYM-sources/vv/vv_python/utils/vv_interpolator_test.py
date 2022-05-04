#!/usr/bin/python3 

# launch command for the initial posterization of a vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import sys
from signal import signal, SIGINT

from numpy import arange

from math import log
from time import sleep
import matplotlib.pyplot as plt

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

# double Bezier
def Gaussian_steep_01_01 (value):
	P0 = [0,0]
	P1 = [0,1]
	P2 = [1,0]
	P3 = [1,1]

	if(value > 1) :
		return_value = 0
	elif(value < 0.0) :
		return_value = 0
	else :
		if(value <= 0.5) :
			value = 2 * value
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = 2 * (1 - value)
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value

def Gaussian_antisteep_01_01 (value):
	P0 = [0,0]
	P1 = [0,1]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value_y = 0
	elif(value < 0) :
		return_value_y = 0
	else :
		if(value <= 0.5) :
			value = 2 * value
			return_value_y = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = 2 * (1 - value)
			return_value_y = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value_y


def main():
	local_var_init = 2500
	local_var_fin = 6600
	valx = []
	valy = []
	for local_scene_percentage in arange(0, 2.05, 0.05):
		scene_percentage = local_scene_percentage
		if(scene_percentage >1):
			scene_percentage -= 1
		local_percentage = Gaussian_steep_01_01(scene_percentage)
		interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		print("x: %.3f y: %.3f" % (local_scene_percentage, interpolated_value))
		valx.append(local_scene_percentage)
		valy.append(interpolated_value)
		print("x: %.3f y: %.3f" % (local_scene_percentage, interpolated_value))

	print("----------")

	val2x = []
	val2y = []
	for local_scene_percentage in arange(0, 2.05, 0.05):
		scene_percentage = local_scene_percentage
		if(scene_percentage >1):
			scene_percentage -= 1
		local_percentage = Gaussian_antisteep_01_01(scene_percentage)
		interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		print("x: %.3f y: %.3f" % (local_scene_percentage, interpolated_value))
		val2x.append(local_scene_percentage)
		val2y.append(interpolated_value)
		print("x: %.3f y: %.3f" % (local_scene_percentage, interpolated_value))

	# red dashes, blue squares and green triangles
	print("Plot")
	plt.plot(valx, valy, 'b-', val2x, val2y, 'r-')
	plt.show()
	sleep(30000)
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
