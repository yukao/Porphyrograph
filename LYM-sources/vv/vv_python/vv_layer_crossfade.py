#!/usr/bin/python3 

# vv_layer_crossfade.py
# crossfading multilayer frames of vector video into a single frame with two modes: sequential or parallel

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os
import getopt
import re

import sys
from signal import signal, SIGINT

from vv_lib import to_num
from vv_lib import force_num
from vv_lib import clamp
from vv_lib import mix_color

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_layer_crossfade.py --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
vv_layer_crossfade.py --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"(#[0-9A-Fa-f]{6})\" \"(#[0-9A-Fa-f]{6})\" \"(#[0-9A-Fa-f]{6})\" \"(#[0-9A-Fa-f]{6})\"
ARGUMENTS:
   --composition-mode: composition mode of the composited image (SEQUENCE or PARALLEL)
   --nb-files: number of input SVG files
   --nb-layers: number of layers in the SVG files
   -o: ouput file 
   -i: input file
'''

##################################################################
# MAIN SUB
##################################################################
# 

def main(main_args) :
	# print("main arts (",main_args,")")
	SEQUENCE = 1
	PARALLEL = 2
	
	composition_mode_list = {\
	   "SEQUENCE" : SEQUENCE,\
	   "PARALLEL" : PARALLEL}

	nb_files = 0
	nb_layers = 0
	composition_mode = 1
	output_file_name = ""
	input_file_names = []
	file1_file2_color_blend = -1
	ind_old_clip = 0
	arg_fill_color = dict()
	arg_stroke_color = dict()

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","composition-mode=","nb-files=",\
			"translation=","rotation=","scale=","bezier_curve=","archimede_spiral=",\
			"fill_color_bg=","fill_color_l1=","fill_color_l2=","fill_color_l3=",\
			"fill_color_l4=","fill_color_l5=","fill_color_l6=","fill_color_l7=",\
			"fill_color_l8=",\
			"stroke_color_l1=","stroke_color_l2=","stroke_color_l3=","stroke_color_l4=",\
			"stroke_color_l5=","stroke_color_l6=","stroke_color_l7=","stroke_color_l8=",\
			"file1_file2_color_blend=","ind_old_clip="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name_list = arg
			input_file_name_list = re.split(r' +',input_file_name_list.rstrip())
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb-layers"):
			nb_layers = int(force_num(arg))
		#  SVG path transformation
		elif opt in ("--composition-mode"):
			composition_mode_string = arg
			if not(composition_mode_string in composition_mode_list.keys()) :
				print(USAGE)
				print("Unknown composition mode",composition_mode_string)
				return 0
			composition_mode = composition_mode_list[composition_mode_string]
		#  SVG path transformation percentage
		elif opt in ("--nb-files"):
			nb_files = int(force_num(arg))
		elif opt in ("--fill_color_bg"):
			arg_fill_color[0]  = arg
		elif opt in ("--fill_color_l1"):
			arg_fill_color[1] = arg
		elif opt in ("--fill_color_l2"):
			arg_fill_color[2] = arg
		elif opt in ("--fill_color_l3"):
			arg_fill_color[3] = arg
		elif opt in ("--fill_color_l4"):
			arg_fill_color[4] = arg
		elif opt in ("--fill_color_l5"):
			arg_fill_color[5] = arg
		elif opt in ("--fill_color_l6"):
			arg_fill_color[6] = arg
		elif opt in ("--fill_color_l7"):
			arg_fill_color[7] = arg
		elif opt in ("--fill_color_l8"):
			arg_fill_color[8] = arg
		elif opt in ("--stroke_color_l1"):
			arg_stroke_color[1] = arg
		elif opt in ("--stroke_color_l2"):
			arg_stroke_color[2] = arg
		elif opt in ("--stroke_color_l3"):
			arg_stroke_color[3] = arg
		elif opt in ("--stroke_color_l4"):
			arg_stroke_color[4] = arg
		elif opt in ("--stroke_color_l5"):
			arg_stroke_color[5] = arg
		elif opt in ("--stroke_color_l6"):
			arg_stroke_color[6] = arg
		elif opt in ("--stroke_color_l7"):
			arg_stroke_color[7] = arg
		elif opt in ("--stroke_color_l8"):
			arg_stroke_color[8] = arg
		elif opt in ("--file1_file2_color_blend"):
			file1_file2_color_blend = clamp(force_num(arg))
		elif opt in ("--ind_old_clip"):
			ind_old_clip = int(force_num(arg))
		# SVG path translation
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)		
	
	# colors of the layers of the first file possibly interpolated with the colors of the second file
	# if file1_file2_color_blend is defined for nb_layers + 1 (nb_layers levels of color + 1 bg color)
	fill_colors_file1_file2 = [['' for i in range(nb_layers+1)] for j in range(nb_files)]
	blended_colors_file1_file2 = ['' for i in range(nb_layers+1)]
	fill_color =     ['' for i in range(nb_layers+1)]
	stroke_color =   ['' for i in range(nb_layers+1)]
	for i in range(nb_layers+1) :
		if(i in arg_fill_color) :
			fill_color[i] = arg_fill_color[i]
		if(i in arg_stroke_color) :
			stroke_color[i] = arg_stroke_color[i]

	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name, " for transformation "+transf_string)
	  
	if(nb_files != len(input_file_name_list)):
		print("incoherent arguments to main: announced number of input files: ",nb_files," number of file names provided :", len(input_file_name_list),"!")
		for inFile  in input_file_name_list :
			print("Input file (", inFile, ")")
		return 0
	
	# radial gradient is for the beam lines of the transformation C_LINES
	nb_groups = 0
	parallel_lines = dict()
	sequenced_lines = []
	rectangle_lines = []
	# lines produced by C_LINES transformation
	sequenced_beam_lines = []
	is_beam_group = 0
	header_lines = []
	defs_lines = []
	metadata_lines = []
	for ind_file in range(nb_files) :
		# print("input file ind_file: ", input_file_name_list[ind_file])
		try:
			FILEin = open(input_file_name_list[ind_file], "rt")
		except IOError:
			print("File not found :", input_file_name_list[ind_file], " for index "+str(ind_file))

		###########################################################
		# reads the header
		# <?xml version="1.0" standalone="no"?>
		# <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 20010904//EN"
		#  "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">
		# <svg version="1.0" xmlns="http://www.w3.org/2000/svg"
		#  width="3840.000000pt" height="2160.000000pt" viewBox="0 0 3840.000000 2160.000000"
		#  preserveAspectRatio="xMidYMid meet">
		# stores header when reading the first file
		line = FILEin.readline()
		line = line.rstrip()
		if(ind_file == 0) :
			header_lines = [line]

		while(line) :
			# stores separately defs and metadata of all files
			if(re.search(r'<defs', line) != None) :
				# removes opening tag from header
				if(ind_file == 0) :
					header_lines.pop()
				break
			elif(re.search(r'<metadata', line) != None) :
				# removes opening tag from header
				if(ind_file == 0) :
					header_lines.pop()
				break
			# reads next line
			line = FILEin.readline()
			line = line.rstrip()
			# stores header when reading the first file
			if(ind_file == 0) :
				header_lines.append(line)

		###########################################################
		# with defs
		with_def = False
		if(re.search(r'<defs', line) != None) :
			with_def = True
			defs_lines.append(line)
			###########################################################
			# reads the defs
			# <defs id="defs_blurs">
			#     <filter id="blur1">
			#         <feGaussianBlur stdDeviation="0.6401739130434783" />
			#     </filter>
			the_filename = os.path.basename(input_file_name_list[ind_file])
			the_filename = os.path.splitext(the_filename)[0]
			while(line) :
				# closing tag
				if(re.search(r'</defs>', line) != None) :
					defs_lines.pop()
					line = FILEin.readline()
					line = line.rstrip()
					break
				# stores the defs of each file
				line = FILEin.readline()
				line = line.rstrip()
				# transforms the ID according to the file name so as to avoid conflicts
				line = re.sub(r'xlink:href="#', r'xlink:href="#'+the_filename, line)
				line = re.sub(r'id="', r'id="'+the_filename, line)
				defs_lines.append(line)

		###########################################################
		# metadata storing
		if(re.search(r'<metadata', line) != None) :
			while(line) :
				if(re.search(r'<\/metadata>', line) != None) :
					metadata_lines.pop()
					break
				line = FILEin.readline()
				line = line.rstrip()
				# stores the metadata 
				metadata_lines.append(line)

		# composition in sequence: the image paths are piled: first the paths of image 1 from gray
		# to black then the paths of image 2, etc.
		# the underlying images can fill the blank spaces of the upper ones
		if(composition_mode == SEQUENCE) :
			ind_group = 1
			line = FILEin.readline()
			line = line.rstrip()
			if(with_def == True) :
				# transforms the defs IDs according to the file name so as to avoid conflicts
				line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
				line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
			while(line):
				# print("crossfaded line : (",line,")")
				if(re.search(r'<rect', line) != None) :
					# possible change of layer color defined in the scenario						
					if(fill_color[0] != "") :
						line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[0]+';', line)

					# possible fill color memory if file1_file2_color_blend is defined
					if(file1_file2_color_blend >= 0) :
						match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
						if(match  != None) :
							fill_colors_file1_file2[ind_file][0] = match.group(1)

					# string initialization
					out_line  = ""
					if(re.search(r' $', line) == None) :
						line = line + " "
					
					# print FILEout line
					out_line = out_line + line + "\n"

					while(line and re.search(r'>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()

						# possible change of layer color defined in the scenario						
						if(fill_color[0] != "") :
							line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[0]+';', line)

						# possible fill color memory if file1_file2_color_blend is defined
						if(file1_file2_color_blend >= 0) :
							match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
							if(match  != None) :
								fill_colors_file1_file2[ind_file][0] = match.group(1)

						# print FILEout line
						out_line = out_line + line + "\n"
										
					# print FILEout line
					out_line = out_line.rstrip()
					rectangle_lines.append([out_line,ind_group])

				elif(re.search(r'<g', line) != None) :					
					out_line = ""
					is_beam_group = 0
					if(re.search(r' $', line) == None) :
						line = line + " "
					if(with_def == True) :
						# transforms the defs IDs according to the file name so as to avoid conflicts
						line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
						line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)

					# lines produced by C_LINES transformation have a radial gradient
					if(re.search(r'\#radialGradient', line) != None) :
						is_beam_group = 1
					
					# possible change of layer color defined in the scenario						
					if(ind_group <= nb_layers and fill_color[ind_group] != "") :
						line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[ind_group]+';', line)
					if(ind_group <= nb_layers and stroke_color[ind_group] != "") :
						line = re.sub(r'stroke="none"', '', line)
						line = re.sub(r'fill:', 'stroke:'+str(stroke_color[ind_group])+'; fill:', line)

					# possible fill color memory if file1_file2_color_blend is defined
					if(file1_file2_color_blend >= 0) :
						match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
						if(match  != None) :
							fill_colors_file1_file2[ind_file][ind_group] = match.group(1)

					out_line = out_line + line

					while(line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						if(with_def == True) :
							# transforms the defs IDs according to the file name so as to avoid conflicts
							line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
							line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)

						# lines produced by C_LINES transformation have a radial gradient
						if(re.search(r'\#radialGradient', line) != None) :
							is_beam_group = 1
						
						# possible change of layer color defined in the scenario						
						if(ind_group <= nb_layers and fill_color[ind_group] != "") :
							line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[ind_group]+';', line)
						if(ind_group <= nb_layers and stroke_color[ind_group] != "") :
							line = re.sub(r'stroke="none"', '', line)
							line = re.sub(r'fill:', 'stroke:'+str(stroke_color[ind_group])+'; fill:', line)

						# possible fill color memory if file1_file2_color_blend is defined
						if(file1_file2_color_blend >= 0) :
							match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
							if(ind_group <= nb_layers and match  != None) :
								fill_colors_file1_file2[ind_file][ind_group] = match.group(1)

						out_line = out_line + line
					
					if(is_beam_group) :
						sequenced_beam_lines.append(out_line)
					else :
						sequenced_lines.append([out_line,ind_group])
				
				elif(re.search(r'<path', line) != None) :
					out_line = ""
					out_line = out_line + line
					while(line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						if(with_def == True) :
							# transforms the defs IDs according to the file name so as to avoid conflicts
							line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
							line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
						out_line = out_line + " " + line
					
					if(is_beam_group) :
						sequenced_beam_lines.append(out_line)
					else :
						sequenced_lines.append([out_line,ind_group])
				
				elif(re.search(r'</g>', line) != None) :
					if(is_beam_group) :
						sequenced_beam_lines.append(line)
					else :
						sequenced_lines.append([out_line,ind_group])

					ind_group += 1

				line = FILEin.readline()
				line = line.rstrip()
				# transforms the ID according to the file name so as to avoid conflicts
				line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
				line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
			
			FILEin.close()
		
		# the composition mode in parallel leads to a different result: 
		# each color layer is combined between all the files.
		elif(composition_mode == PARALLEL) :
			ind_group = 1
			line = FILEin.readline()
			line = line.rstrip()
			if(with_def == True) :
				# transforms the defs IDs according to the file name so as to avoid conflicts
				line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
				line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
			group_parallel_line  = ""
			rectangle_parallel_line  = ""
			while(line):
				# print("crossfaded line : (",line,")")
				if(re.search(r'<rect', line) != None) :
					# possible change of layer color defined in the scenario						
					if(fill_color[0] != "") :
						line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[0]+';', line)

					# possible fill color memory if file1_file2_color_blend is defined
					if(file1_file2_color_blend >= 0) :
						match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
						if(match  != None) :
							fill_colors_file1_file2[ind_file][0] = match.group(1)

					# string initialization
					rectangle_parallel_line  = ""
					if(re.search(r' $', line) == None) :
						line = line + " "
					
					# print FILEout line
					rectangle_parallel_line = rectangle_parallel_line + line + "\n"

					while(line and re.search(r'>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()

						# possible change of layer color defined in the scenario						
						if(fill_color[0] != "") :
							line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[0]+';', line)

						# possible fill color memory if file1_file2_color_blend is defined
						if(file1_file2_color_blend >= 0) :
							match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
							if(match  != None) :
								fill_colors_file1_file2[ind_file][0] = match.group(1)

						# print FILEout line
						rectangle_parallel_line = rectangle_parallel_line + line + "\n"
										
					# print FILEout line
					rectangle_parallel_line = rectangle_parallel_line.rstrip()
					if(0 in parallel_lines) :
						parallel_lines[0].append(rectangle_parallel_line)
					else :
						parallel_lines[0] = [rectangle_parallel_line]

					# print FILEout "\n"
				# print("crossfaded line : (",line,")")
				elif(re.search(r'<g', line) != None) :
					out_line = ""
					is_beam_group = 0
					# string initialization
					group_parallel_line  = ""
					if(re.search(r' $', line) == None) :
						line = line + " "
					
					# lines produced by C_LINES transformation have a radial gradient
					if(re.search(r'\#radialGradient', line) != None) :
						is_beam_group = 1
					
					# possible change of layer color defined in the scenario
					if(ind_group <= nb_layers and fill_color[ind_group] != "") :
						line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[ind_group]+';', line)
					if(ind_group <= nb_layers and stroke_color[ind_group] != "") :
						line = re.sub(r'stroke="none"', '', line)
						line = re.sub(r'fill:', 'stroke:'+str(stroke_color[ind_group])+'; fill:', line)

					# possible fill color memory if file1_file2_color_blend is defined
					if(file1_file2_color_blend >= 0) :
						match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
						if(ind_group <= nb_layers and match  != None) :
							fill_colors_file1_file2[ind_file][ind_group] = match.group(1)

					# print FILEout line
					out_line = out_line + line

					while(line and re.search(r'>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						if(with_def == True) :
							# transforms the defs IDs according to the file name so as to avoid conflicts
							line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
							line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)

						# lines produced by C_LINES transformation have a radial gradient
						if(re.search(r'\#radialGradient', line) != None) :
							is_beam_group = 1

						# possible change of layer color defined in the scenario						
						if(ind_group <= nb_layers and fill_color[ind_group] != "") :
							line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+fill_color[ind_group]+';', line)
						if(ind_group <= nb_layers and stroke_color[ind_group] != "") :
							line = re.sub(r'stroke="none"', '', line)
							line = re.sub(r'fill:', 'stroke:'+str(stroke_color[ind_group])+'; fill:', line)

						# possible fill color memory if file1_file2_color_blend is defined
						if(file1_file2_color_blend >= 0) :
							match = re.search(r'fill:(\#[0-9A-Fa-f]{6});', line)
							if(ind_group <= nb_layers and match  != None) :
								fill_colors_file1_file2[ind_file][ind_group] = match.group(1)

						# print FILEout line
						out_line = out_line + line						
					
					if(is_beam_group) :
						sequenced_beam_lines.append(out_line + "\n")
					else :
						group_parallel_line = group_parallel_line + out_line + "\n"
					
					# print FILEout "\n"
				
				elif(re.search(r'<path', line) != None) :
					out_line = ""
					# print FILEout line
					out_line = out_line + line
					while(line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						if(with_def == True) :
							# transforms the defs IDs according to the file name so as to avoid conflicts
							line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
							line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
						out_line = out_line + " " + line
						# print FILEout " " , line
					
					if(is_beam_group) :
						sequenced_beam_lines.append(out_line + "\n")
					else :
						group_parallel_line = group_parallel_line + out_line + "\n"
					
					# print FILEout "\n"
				
				elif(re.search(r'</g>', line) != None) :
					if(is_beam_group) :
						sequenced_beam_lines.append(line)
					else :
						group_parallel_line = group_parallel_line + line
					
					# print FILEout line
					if(ind_group in parallel_lines) :
						parallel_lines[ind_group].append(group_parallel_line)
					else :
						parallel_lines[ind_group] = [group_parallel_line]
					ind_group += 1

				elif(re.search(r'</svg>', line) != None) :
					break

				line = FILEin.readline()
				line = line.rstrip()
				if(with_def == True) :
					# transforms the defs IDs according to the file name so as to avoid conflicts
					line = re.sub(r'filter\:url\(\#', r'filter:url(#'+the_filename, line)
					line = re.sub(r'fill\:url\(\#', r'fill:url(#'+the_filename, line)
				
			FILEin.close()
			if(nb_groups < ind_group) :
				nb_groups = ind_group
		
	
	# lines produced by C_LINES transformation are displayed first so that the characters are on top of them
	for header_line in header_lines :
		FILEout.write(header_line+"\n")
	if(len(defs_lines) > 0) :
		for defs_line in defs_lines :
			FILEout.write(defs_line+"\n")
		FILEout.write("</defs>\n")
	if(len(metadata_lines) > 0) :
		FILEout.write("<metadata>\n")
		for metadata_line in metadata_lines :
			FILEout.write(metadata_line+"\n")
		FILEout.write("</metadata>\n")
	for sequenced_beam_line in sequenced_beam_lines :
		FILEout.write(sequenced_beam_line+"\n")

	# possible fill color memory if file1_file2_color_blend is defined
	if(file1_file2_color_blend >= 0) :
		for indGroup in range(min(nb_groups, nb_layers + 1)) :
			color1 = fill_colors_file1_file2[0][indGroup]
			color2 = fill_colors_file1_file2[1][indGroup]
			if(color1 != '' and color2 != '') :
				blended_colors_file1_file2[indGroup] = mix_color(color1, color2, file1_file2_color_blend)
				# print("ind group col1 col2 blend colblend ", indGroup, color1, color2, file1_file2_color_blend, blended_colors_file1_file2[indGroup])
	if(composition_mode == PARALLEL) :
		# print("ind_old_clip", ind_old_clip)
		for indGroup in range(min(nb_groups, nb_layers + 1)) :
			# the layers of the oldest clip should come last to be more visible
			if(indGroup in parallel_lines) :
				if(ind_old_clip == 0 and indGroup >= 1):
					parallel_lines[indGroup].reverse()
				for parallel_line in parallel_lines[indGroup] :
					# print("group",indGroup,"parallel_line", parallel_line[0:200])
					# print("ind_group:", indGroup)
					if(blended_colors_file1_file2[indGroup] != '') :
						parallel_line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+blended_colors_file1_file2[indGroup]+';', parallel_line)
					FILEout.write(parallel_line+"\n")
	elif(composition_mode == SEQUENCE) :
		for rectangle_line, ind_group in rectangle_lines :
			if(ind_group <= nb_layers and blended_colors_file1_file2[ind_group] != '') :
				rectangle_line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+blended_colors_file1_file2[ind_group]+';', parallel_line)
			FILEout.write(rectangle_line+"\n")
		for sequenced_line, ind_group in sequenced_lines :
			if(ind_group <= nb_layers and blended_colors_file1_file2[ind_group] != '') :
				sequenced_line = re.sub(r'fill:#[0-9a-fA-F]+;', 'fill:'+blended_colors_file1_file2[ind_group]+';', parallel_line)
			FILEout.write(sequenced_line+"\n")

	FILEout .write("</svg>\n")
	FILEout.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
