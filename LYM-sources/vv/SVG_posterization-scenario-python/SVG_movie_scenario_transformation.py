#!/usr/bin/perl 

# use strict

# use warnings

# package SVG_movie_scenario_transformation

# use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/'
import os

import vv_lib
import SVG_movie_transform
import SVG_movie_fillAndStroke

import csv
import getopt
import sys

# SVG_movie_scenario_transformation.pl
# reads a transformation scenario and executes it

USAGE = '''/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_scenario_transformation.pl -i scenario_file

ARGUMENTS:
	 -i: input scenariofile
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	# SCENARIO VARIABLES
	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()

	trace_only = False

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', scenario_file_name)

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
			print("File not found :", scenario_file_name, " or path is incorrect")
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 
	line = next(readCSV) 
	# print "line2 ", line 

	#3rd line: variable types
	line_types =  next(readCSV)

	#4th line: variable names
	var_names =  next(readCSV) 
	#removes the first one
	var_names.pop(0)

	#finishes the types now that the variable strings are known
	#removes the first one
	line_types.pop(0)
	indVar = 0
	for line_type in line_types :
		var_types[var_names[indVar]] = line_type
		indVar += 1

	# scenario line
	values_line =  next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found!")
		return 0
	
	nb_scenes = vv_lib.type_cast_num(values_line[1])
	# print( "nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		# scene line
		values_line =  next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =  next(readCSV)
		# var IDs line
		line =  next(readCSV)

		# variable initial value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_init in values_line:
			val_init[var_names[indVar]] = vv_lib.type_cast_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = vv_lib.type_cast_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp    
			indVar += 1

		# printf("\nNb scenes %d scene ID %s input_file_name %s IN start-end %s %s OUT start-end %s %s\n\n", nb_scenes, scene_ID,
		#   val_init["svg_input_file_name"], val_init["imageInputIndex"], 
		#   val_fin:"imageInputIndex", val_init["imageOutputIndex"], 
		#   val_fin:"imageOutputIndex")

		if val_init["imageInputIndex"] < val_fin["imageInputIndex"]:
			pas = 1
		else :
			pas = -1

		# applies possible transformations on the SVG files
		# creates the directories in which the paths files are saved
		if( not os.path.isdir(val_init["svg_output_directory"])  ):
			try:
					os.mkdir(val_init["svg_output_directory"])
			except OSError:
					print ("Creation of the directory %s failed" % val_init["svg_output_directory"])
			else:
					print ("Successfully created the directory %s " % val_init["svg_output_directory"])
		# else:
		# 	print ("Directory %s already exists" % val_init["svg_output_directory"])

		##################################################################
		# SCANS ALL THE FRAMES IN THE MOVIE
		##################################################################
		indImageOutput = val_init["imageOutputIndex"]
		nb_layers = val_init["nb_posterization_layers"]
		tmp_dir = val_init["tmp_directory"]
		for indImageInput in range(val_init["imageInputIndex"], val_fin["imageInputIndex"] + pas, pas):
			countInput = "%04d" % indImageInput
			countOutput = "%04d" % indImageOutput

			scene_percentage = 0
			if pas != 0 and (val_fin["imageInputIndex"] - val_init["imageInputIndex"]) != 0 :
				scene_percentage = (indImageInput - val_init["imageInputIndex"])/(val_fin["imageInputIndex"] - val_init["imageInputIndex"])
				
			#clamping
			scene_percentage = vv_lib.clamp(scene_percentage)
			# interpolating all the variables
			for var_name in var_names :
				# print( "var name [",var_name,"]\n" )
				val_interp[var_name] = vv_lib.interpolated_value_calculation(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
			
			if(trace_only) :
				print("Ind image %d Scene percentage %.2f percent_transf1 %.2f fill_opacity_l1 %.2f fill_color_l4 %s fill_color_bg %s\n" % \
					indImageInput, scene_percentage, val_interp["percent_transf1"], val_interp["fill_opacity_l1"], val_interp["fill_color_l4"], val_interp["fill_color_bg"])
	
			transf_percentage = val_interp["percent_transf1"]
			#clamping
			transf_percentage = vv_lib.clamp(transf_percentage)

			full_svg_input_file_name = val_init["svg_input_directory"] + val_init["svg_input_file_name"]
			full_svg_output_file_name = val_init["svg_output_directory"] + val_init["svg_output_file_name"]
			transformation_function_string = val_init["transf1_ID"]

			if(not trace_only) :
				# print  "/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_transform.pl --nb-layers nb_layers -o :tmp_dirtmp_transformed.svg -i :full_svg_input_file_name_:countInput.svg --transformation transformation_function_string --percent transf_percentage \n"
				
				# print( "Transformed SVG file percentage ", transf_percentage)
				transformation_argts = ["--nb-layers", nb_layers, \
						"-o", tmp_dir+"tmp_transformed.svg", \
						"-i", full_svg_input_file_name+"_"+countInput+".svg", \
						"--transformation", transformation_function_string, \
						"--percent", transf_percentage]

				# passes translation arguments from the scenario file: translation vector
				if ("x_translation" in val_interp) and ("y_translation" in val_interp):
					interp_x_translation = val_interp["x_translation"]
					interp_y_translation = val_interp["y_translation"]
					transformation_argts.extend(["--translation", str(interp_x_translation)+"x"+str(interp_y_translation)])

				# passes rotation arguments from the scenario file: translation vector
				if(("x_rotation_center" in val_interp) and ("y_rotation_center" in val_interp) and ("rotation_angle" in val_interp)) :
					interp_x_rotation_center = val_interp["x_rotation_center"]
					interp_y_rotation_center = val_interp["y_rotation_center"]
					interp_rotation_angle = val_interp["rotation_angle"]
					transformation_argts.extend(["--rotation", str(interp_x_rotation_center)+"x"+ str(interp_y_rotation_center)+"x"+ str(interp_rotation_angle)])
				
				# passes scaling arguments from the scenario file: translation vector
				if(("x_scaling_center" in val_interp) and ("y_scaling_center" in val_interp) and ("scaling_factor" in val_interp)) :
					interp_x_scaling_center = val_interp["x_scaling_center"]
					interp_y_scaling_center = val_interp["y_scaling_center"]
					interp_scaling_factor = val_interp["scaling_factor"]
					# print "SVG_movie_transform.main(--nb-layers", nb_layers, 
					#   "-o", tmp_dir+"tmp_transformed.svg", 
					#   "-i", full_svg_input_file_name+"_"+countInput+".svg", 
					#   "--transformation", transformation_function_string, 
					#   "--percent", transf_percentage, 
					#   "--scale", interp_x_scaling_center, interp_y_scaling_center, "\n"
					transformation_argts.extend(["--scale", str(interp_x_scaling_center)+"x"+ str(interp_y_scaling_center)+"x"+ str(interp_scaling_factor)])
					
				# passes Bezier arguments from the scenario file: control points
				if(("b0_x" in val_interp) and ("b0_y" in val_interp) and ("b1_x" in val_interp) and ("b1_y" in val_interp) and ("b2_x" in val_interp) and ("b2_y" in val_interp) and ("b3_x" in val_interp) and ("b3_y" in val_interp)) :
					interp_b0_x = val_interp["b0_x"]
					interp_b0_y = val_interp["b0_y"]
					interp_b1_x = val_interp["b1_x"]
					interp_b1_y = val_interp["b1_y"]
					interp_b2_x = val_interp["b2_x"]
					interp_b2_y = val_interp["b2_y"]
					interp_b3_x = val_interp["b3_x"]
					interp_b3_y = val_interp["b3_y"]
					# empty input file for string substitution, does not take frame count in consideration
					transformation_argts.extend(["--bezier_curve", str(interp_b0_x)+"x"+ str(interp_b0_y)+"x"+ str(interp_b1_x)+"x"+ str(interp_b1_y)+"x"+ \
						str(interp_b2_x)+"x"+ str(interp_b2_y)+"x"+ str(interp_b3_x)+"x"+ str(interp_b3_y)])
					
				# passes Archimedean spiral arguments from the scenario file: center, radial expansion, angular step 
				# and number of steps
				if(("spiral_center_x" in val_interp) and ("spiral_center_y" in val_interp)
					 and ("spiral_radius_step" in val_interp) and ("spiral_angle_step" in val_interp)
					 and ("spiral_nb_steps" in val_interp)) :
					interp_spiral_center_x = val_interp["spiral_center_x"]
					interp_spiral_center_y = val_interp["spiral_center_y"]
					interp_spiral_radius_step = val_interp["spiral_radius_step"]
					interp_spiral_angle_step = val_interp["spiral_angle_step"]
					interp_spiral_nb_steps = val_interp["spiral_nb_steps"]
					# empty input file for string substitution, does not take frame count in consideration
					transformation_argts.extend(["--archimede_spiral", interp_spiral_center_x+"x"+ interp_spiral_center_y+"x"+ \
						 str(interp_spiral_radius_step)+"x"+  str(interp_spiral_angle_step)+"x"+  str(interp_spiral_nb_steps)])
					
				# no additional arguments to the transformation
				# print ("transformation arguments: ", transformation_argts)
				if( SVG_movie_transform.main(transformation_argts) == 0) :
					return 0
					
			if(("fill_color_bg" in val_interp) and ("fill_color_l1" in val_interp)\
					 and ("fill_color_l2" in val_interp) and ("fill_color_l3" in val_interp)\
					 and ("fill_color_l4" in val_interp) and ("fill_opacity_l1" in val_interp)\
					 and ("fill_opacity_l2" in val_interp) and ("fill_opacity_l3" in val_interp)\
					 and ("fill_opacity_l4" in val_interp) and ("stroke_color" in val_interp) \
					 and ("stroke_opacity" in val_interp) and ("stroke_width" in val_interp) ) :
				interp_fill_color_bg = val_interp["fill_color_bg"]
				interp_fill_color_l1 = val_interp["fill_color_l1"]
				interp_fill_color_l2 = val_interp["fill_color_l2"]
				interp_fill_color_l3 = val_interp["fill_color_l3"]
				interp_fill_color_l4 = val_interp["fill_color_l4"]
				interp_fill_opacity_l1 = val_interp["fill_opacity_l1"]
				interp_fill_opacity_l2 = val_interp["fill_opacity_l2"]
				interp_fill_opacity_l3 = val_interp["fill_opacity_l3"]
				interp_fill_opacity_l4 = val_interp["fill_opacity_l4"]
				interp_stroke_color = val_interp["stroke_color"]
				interp_stroke_opacity = val_interp["stroke_opacity"]
				interp_stroke_width = val_interp["stroke_width"]

				if(not trace_only) :
					# print "Fill and stroke SVG file ", :full_svg_output_file_name."_".:countOutput.".svg"  ,"\n"

					if( SVG_movie_fillAndStroke.main(["--nb-layers", nb_layers, \
						"-o", full_svg_output_file_name+"_"+countOutput+".svg", \
						"-i", tmp_dir+"tmp_transformed.svg", \
						"--style", str(interp_fill_color_bg)+"x"+ \
						str(interp_fill_color_l1)+"x"+ str(interp_fill_color_l2)+"x"+ \
						str(interp_fill_color_l3)+"x"+ str(interp_fill_color_l4)+"x"+ \
						str(interp_fill_opacity_l1)+"x"+ str(interp_fill_opacity_l2)+"x"+ \
						str(interp_fill_opacity_l3)+"x"+ str(interp_fill_opacity_l4)+"x"+ \
						str(interp_stroke_color)+"x"+ str(interp_stroke_opacity)+"x"+ str(interp_stroke_width)]) == 0) :
						return 0
			
			else :
				os.system("cp "+tmp_dir+"tmp_transformed.svg "+full_svg_output_file_name+"_"+countOutput+".svg")

			# moves forward on output frames
			indImageOutput += 1

			# only one frame if frame ini and frame end are equal
			if (pas == 0):
				break

		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
	# /scenario line
	values_line = next(readCSV)
	if(values_line[0] != "/scenario") :
		print( "Expected tag \"/scenario\" not found!\n" )
		return 0
	
	return 1

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])
