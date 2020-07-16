import re
import sys

# package vv_lib

##################################################################
# COLOR MIXTURE
##################################################################
def mix_color (initial_color, target_color, local_percentage):
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
	mixed_color = mix_red + mix_green + mix_blue
	# print "target_color initial_color local_percentage: target_color, initial_color, local_percentage / mixed_color: mixed_color \n"
	return mixed_color

def type_force_num(val_init):
	try:
		return_value = float(val_init)
	except ValueError:
		try:
			return_value = int(val_init)
		except ValueError:
			return_value = 0
	return return_value

def type_cast_num(val_init):
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
def interpolated_value_calculation (local_var_type, local_var_init, local_var_fin, local_interpolator, local_scene_percentage):
	interpolated_value = 0

	# extraction of initial and final color in case of color interpolation
	color_initial = ""
	color_final = ""
	if(local_var_type == "hexcolor") :
		# color calculation as an interpolation between 2 colors with a blending coef
		color_initial = local_var_init
		color_initial = re.sub( '^\#', '', color_initial)
		# removes 1st char
		color_final = local_var_fin
		color_final = re.sub( '^\#', '', color_final)
		# removes 1st char

	#interpolation calculation
	if(local_interpolator == "l") :
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_scene_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = '#' + mix_color(color_initial, color_final, local_scene_percentage)
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
			interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
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
			interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
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
			interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
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
			interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
		
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
		
	# non-centered Gaussian
	elif re.search( '^i', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(interpolation_values[1])
		local_percentage = NonCentered_Gaussian_01_01(local_scene_percentage, center)
		if(local_var_type == "float") :
			interpolated_value = local_var_init + local_percentage * (local_var_fin - local_var_init)
		elif(local_var_type == "hexcolor") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
		elif(local_var_type == "int") :
			# color calculation as an interpolation between 2 colors with a blending coef
			interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
		
	# linear on a def part
	elif re.search( '^L', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(interpolation_values[1])
		perc_fin =  clamp(interpolation_values[1] + interpolation_values[2])
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
				interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Bezier smooth 00 01 10 11 on a subpart
	elif re.search( '^B', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(interpolation_values[1])
		perc_fin =  clamp(interpolation_values[1] + interpolation_values[2])
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
				interpolated_value = '#' . mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Bezier steep 00 10 01 11 on a subpart
	elif re.search( '^T', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(interpolation_values[1])
		perc_fin =  clamp(interpolation_values[1] + interpolation_values[2])
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
				interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Gaussian on a subpart
	elif re.search( '^G', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(interpolation_values[1])
		perc_fin =  clamp(interpolation_values[1] + interpolation_values[2])
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
				interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	# Gaussian steep on a subpart
	elif re.search( '^H', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		perc_ini =  clamp(interpolation_values[1])
		perc_fin =  clamp(interpolation_values[1] + interpolation_values[2])
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
				interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
	
	# non-centered Gaussian on a subpart
	elif re.search( '^I', local_interpolator ) != None :
		interpolation_values = re.split(r' +',local_interpolator.rstrip())
		center =  clamp(interpolation_values[1])
		perc_ini =  clamp(interpolation_values[2])
		perc_fin =  clamp(interpolation_values[2] + interpolation_values[3])
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
				interpolated_value = '#' + mix_color(color_initial, color_final, local_percentage)
			elif(local_var_type == "int") :
				# color calculation as an interpolation between 2 colors with a blending coef
				interpolated_value = int(local_var_init + local_scene_percentage * (local_var_fin - local_var_init))
			
	elif(local_interpolator == "s") :
		interpolated_value = local_var_init

	else :
		interpolated_value = local_var_init
	
	return interpolated_value
