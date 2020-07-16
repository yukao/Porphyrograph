#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math

import colorsys

from math import log
from math import pow

##################################################################
# COLOR MIXTURE
##################################################################
def bilinear_interpolation(x, y, z00, z01, z10, z11):
	
	def linear_interpolation(x, z0, z1):
		return z0 * (1 - x) + z1 * x

	return linear_interpolation(y, linear_interpolation(x, z00, z01),
								   linear_interpolation(x, z10, z11))

def bilinear_interpolation_pixel(x_float, y_float, array_dim2, array_h_w):
	h, w = array_h_w
	y1 = min(h-1, int(math.floor(y_float)))
	y2 = min(h-1, y1 + 1)
	x1 = min(w-1, int(math.floor(x_float)))
	x2 = min(w-1, x1 + 1)
	z00 = array_dim2[y1,x1]
	z10 = array_dim2[y2,x1]
	z01 = array_dim2[y1,x2]
	z11 = array_dim2[y2,x2]
	return bilinear_interpolation(y_float - y1, x_float - x1, z01, z11, z00, z10)

##################################################################
# COLOR MIXTURE
##################################################################
def saturate_color (local_initial_color, local_percentage):
	rgb = re.sub( '^\#', '', local_initial_color)
	h, s, v = colorsys.rgb_to_hsv(float('0x' + initial_color[ 0 : 2 ], 0)/255, float('0x' + initial_color[ 2 : 4 ], 0)/255, float('0x' + initial_color[ 4 : 6 ], 0)/255)
	s = s * (1 - local_percentage) + local_percentage
	r, g, b = colorsys.hsv_to_rgb(h, s, v)
	sat_red = "%02x" % int(r * 255)
	sat_green = "%02x" % int(g * 255)
	sat_blue = "%02x" % int(b * 255)
	saturated_color = '#' + sat_red + sat_green + sat_blue
	# print "target_color initial_color local_percentage: target_color, initial_color, local_percentage / sated_color: sated_color \n"
	return saturated_color

def mix_color (local_initial_color, local_target_color, local_percentage):
	# print( "initial_color target_color local_percentage:", local_initial_color,  "/", local_target_color, "/", local_percentage )
	initial_color = re.sub( '^\#', '', local_initial_color)
	target_color = re.sub( '^\#', '', local_target_color)
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
	mixed_color = '#' + mix_red + mix_green + mix_blue
	return mixed_color

##################################################################
# COLOR TEMPERATURE
##################################################################
# Color temperature (kelvin) to RGB subroutine ported from VB.NET (!) 
# code by Tanner Helland (see code header for credits).
# http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
# color space functions translated from HLSL versions on Chilli Ant (by Ian Taylor)
# http://www.chilliant.com/rgb2hsv.html
# https://www.shadertoy.com/view/lsSXW1

def red_sup_6600(temp):
	return 1.29293618606274509804 * pow(temp/100.0, -0.1332047592)

def green_between_1000_and_6600(temp):
	return 0.39008157876901960784 * log(temp/100.0) - 0.63184144378862745098

def green_sup_6600(temp):
	return 1.12989086089529411765 * pow(temp/100.0, -0.0755148492)

def blue_between_2000_and_6600(temp):
	return 0.54320678911019607843 * log(temp/100.0 - 10.0) - 1.19625408914

def rgb_gain(temp):
	temp = min(40000,max(1000,temp))
	#red
	if(temp < 6600):
		r = 1
	else:
		r = red_sup_6600(temp)
	# green
	if(temp <= 6600):
		g = green_between_1000_and_6600(temp)
	elif(temp > 6600):
		g = green_sup_6600(temp)
	# blue
	if(temp < 2000):
		b = 0
	elif(temp >= 2000 and temp <= 6600):
		b = blue_between_2000_and_6600(temp)
	else:
		b = 1
	return [min(max(r,0),1),min(max(g,0),1),min(max(b,0),1)]

def temperature_modification(input_color, temperature):
	initial_color = re.sub( '^\#', '', input_color)
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	gain_red, gain_green, gain_blue = rgb_gain(to_num(temperature))
	balanced_red = min(max(int(gain_red * int(initial_red, 0)),0),255)
	balanced_green = min(max(int(gain_green * int(initial_green, 0)),0),255)
	balanced_blue = min(max(int(gain_blue * int(initial_blue, 0)),0),255)
	balanced_red = "%02x" % balanced_red
	balanced_green = "%02x" % balanced_green
	balanced_blue = "%02x" % balanced_blue
	return '#' + balanced_red + balanced_green + balanced_blue

##################################################################
# STRING TO NUM
##################################################################
def force_num(val_init):
	try:
		return_value = float(val_init)
	except ValueError:
		try:
			return_value = int(val_init)
		except ValueError:
			return_value = 0
	return return_value

def to_num(val_init):
	try:
		return_value = int(val_init)
	except ValueError:
		try:
			return_value = float(val_init)
		except ValueError:
			return_value = val_init
	return return_value

##################################################################
# VALUES INTERPOLATION
##################################################################
def clamp (value):
	if(value > 1) :
		value = 1
	elif(value < 0.0) :
		value = 0

	return value

def Bezier_01_01 (value):
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	else :
		return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value


def Bezier_steep_01_01 (value):
	P0 = [0,0]
	P1 = [0,1]
	P2 = [1,0]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	else :
		return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
	
	return return_value

# double Bezier
def Gaussian_01_01 (value):
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]
	
	if(value > 1) :
		return_value = 1
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

# double Bezier
def Gaussian_steep_01_01 (value):
	P0 = [0,0]
	P1 = [0,1]
	P2 = [1,0]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
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

# double Bezier
def NonCentered_Gaussian_01_01 (value,curveCenter):
	P0 = [0,0]
	P1 = [1,0]
	P2 = [0,1]
	P3 = [1,1]

	if(value > 1) :
		return_value = 1
	elif(value < 0.0) :
		return_value = 0
	elif(curveCenter <= 0.0) :
		return_value = Bezier_01_01(1 - value)
	elif(curveCenter >= 1.0) :
		return_value = Bezier_01_01(value)
	else :
		if(value <= curveCenter) :
			value = value/curveCenter
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		else :
			value = (1 - value)/(1 - curveCenter)
			return_value = ((1-value) ** 3) * P0[1] + 3 * value * ((1-value) ** 2) * P1[1] + 3 * (value ** 2) * (1-value) * P2[1] + (value ** 3) * P3[1] 
		
	return return_value


# calculation of interpolation value
def interpolate (local_var_type, local_var_init, local_var_fin, local_interpolator, local_scene_percentage):
	interpolated_value = 0

	# a string
	if(local_var_type == "string") :
		interpolated_value = local_var_init

	# extraction of initial and final color in case of color interpolation
	color_initial = ""
	color_final = ""
	if(local_var_type == "hexcolor") :
		# color calculation as an interpolation between 2 colors with a blending coef
		color_initial = local_var_init
		color_final = local_var_fin

	#interpolation calculation
	if(local_interpolator == "l") :
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_scene_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_scene_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
		
	# Bezier smooth 00 01 10 11
	elif(local_interpolator == "b") :
		local_percentage = Bezier_01_01(local_scene_percentage)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
	
	# Bezier steep 00 10 01 11
	elif(local_interpolator == "t") :
		local_percentage = Bezier_steep_01_01(local_scene_percentage)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
	
	# Gaussian
	elif(local_interpolator == "g") :
		local_percentage = Gaussian_01_01(local_scene_percentage)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
	
	# Gaussian steep
	elif(local_interpolator == "h") :
		local_percentage = Gaussian_steep_01_01(local_scene_percentage)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_percentage)
		
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
		
	# non-centered Gaussian
	elif re.search( '^i', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(force_num(interpolation_values[1]))
		local_percentage = NonCentered_Gaussian_01_01(local_scene_percentage, center)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = mix_color(color_initial, color_final, local_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
		
	# linear on a def part
	elif re.search( '^L', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %+2f %+2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_fin
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Bezier smooth 00 01 10 11 on a subpart
	elif re.search( '^B', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(interpolation_values[1] + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_fin
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Bezier_01_01(local_percentage)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Bezier steep 00 10 01 11 on a subpart
	elif re.search( '^T', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_fin
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Bezier_steep_01_01(local_percentage)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Gaussian on a subpart
	elif re.search( '^G', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_init
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Gaussian_01_01(local_percentage)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Gaussian steep on a subpart
	elif re.search( '^H', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(force_num(interpolation_values[1]))
		perc_fin =  clamp(force_num(interpolation_values[1]) + force_num(interpolation_values[2]))
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_init
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_percentage = Gaussian_steep_01_01(local_percentage)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
	
	# non-centered Gaussian on a subpart
	elif re.search( '^I', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(force_num(interpolation_values[1]))
		perc_ini =  clamp(force_num(interpolation_values[2]))
		perc_fin =  clamp(force_num(interpolation_values[2]) + interpolation_values[3])
		# printf "perc ini fin %.2f %.2f\n", perc_ini, perc_fin
		if(local_scene_percentage <= perc_ini) :
			interpolated_value = local_var_init
		elif(local_scene_percentage >= perc_fin) :
			interpolated_value = local_var_init
		elif(perc_ini >= perc_fin) :
			interpolated_value = local_var_init
		else :
			local_percentage = (local_scene_percentage - perc_ini)/(perc_fin - perc_ini)
			local_center = perc_ini + center * (perc_fin - perc_ini)
			local_percentage = NonCentered_Gaussian_01_01(local_percentage, local_center)
			if(local_var_type == "float") :
				interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
			elif(local_var_type == "hexcolor") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	elif(local_interpolator == "s") :
		interpolated_value = local_var_init

	else :
		interpolated_value = local_var_init
	
	return interpolated_value

##################################################################
# CVS FILE STRING CONSTRUCTION FROM VARIABLE DICIONARY
##################################################################
# dictionary
# key: variable name
# values: a list made of [type,  initial value, final value, interpolation mode]
def variable_dict_to_CVS_file(local_scenario_variables) :
	return variable_dict_to_CVS_lines(local_scenario_variables, [])

def variable_dict_append_CVS_file(local_scenario_variables, local_scenario_string_list) :
	return variable_dict_to_CVS_lines(local_scenario_variables, local_scenario_string_list)

def variable_dict_to_CVS_lines(local_scenario_variables, local_scenario_string_list) :
	nb_vars = len(local_scenario_variables)
	returned_string_list = local_scenario_string_list
	sorted_variables = sorted(local_scenario_variables)

	# empty string, creats a list of empty strings
	emptyFile = returned_string_list == []
	if(emptyFile) :
		returned_string_list = ["" for x in range(13)]
	elif(len(returned_string_list) != 13) :
		print("expeted 13 scenario lines")
		sys.exit(2)

	indLine = 0

	# ranks
	if(emptyFile) :
		line_string = 'RANK,'
	else :
		line_string = ','
	for index in range(nb_vars) :
		line_string = line_string + str(index) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1

	# verbatim
	if(emptyFile) :
		line_string = 'VERBATIM,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# types
	if(emptyFile) :
		line_string = 'TYPE,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + local_scenario_variables[var_name][0] + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# IDs
	if(emptyFile) :
		line_string = 'ID,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scenario
	if(emptyFile) :
		line_string = 'scenario,1,'
	else :
		line_string = ','
	for index in range(nb_vars - 2) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene
	if(emptyFile) :
		line_string = 'scene,transf_scene,'
	else :
		line_string = ','
	for index in range(nb_vars - 2) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene ID
	if(emptyFile) :
		line_string = 'transf_scene,'
	else :
		line_string = ','
	for index in range(nb_vars) :
		line_string = line_string + 'transf_scene,'
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# IDs
	if(emptyFile) :
		line_string = 'ID,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + var_name + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Initial values
	if(emptyFile) :
		line_string = 'Initial,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + str(local_scenario_variables[var_name][1]) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Final values
	if(emptyFile) :
		line_string = 'Final,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + str(local_scenario_variables[var_name][2]) + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# Interpolation modes
	if(emptyFile) :
		line_string = 'Interp,'
	else :
		line_string = ','
	for var_name in sorted_variables :
		line_string = line_string + local_scenario_variables[var_name][3] + ','
	line_string = line_string[:-1]
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scene
	if(emptyFile) :
		line_string = '/scene,'
	else :
		line_string = ','
	for index in range(nb_vars - 1) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1


	# scenario
	if(emptyFile) :
		line_string = '/scenario,'
	else :
		line_string = ','
	for index in range(nb_vars - 1) :
		line_string = line_string + ','
	returned_string_list[indLine] = returned_string_list[indLine] + line_string
	indLine += 1

	return returned_string_list
