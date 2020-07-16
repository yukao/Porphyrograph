#!/usr/bin/perl 

# use strict
# use warnings

# package SVG_movie_layer_crossfading

# SVG_movie_layer_crossfading.pl
# SVG_movie_layer_compositions an image into another image or into a SVG vector graphics

# usage
# SVG_movie_layer_crossfading.pl --output-format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

# IN
#
#			  n paths

# OUT 
#
#			  coposited and colored image

import os
import getopt
import re

import vv_lib

USAGE = '''usage: SVG_movie_layer_crossfading.pl --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_layer_crossfading.pl --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"#cbcbcb\" \"#9a9a9a\" \"#6b6b6b\" \"#1c1c1c\"

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

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","composition-mode=","nb-files=","translation=","rotation=","scale=","bezier_curve=","archimede_spiral="])
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
			nb_layers = vv_lib.type_cast_num(arg)
		#  SVG path transformation
		elif opt in ("--composition-mode"):
			composition_mode_string = arg
			if not(composition_mode_string in composition_mode_list.keys()) :
				print( USAGE )
				print( "Unknown composition mode",composition_mode_string )
				return 0
			composition_mode = composition_mode_list[composition_mode_string]
		#  SVG path transformation percentage
		elif opt in ("--nb-files"):
			nb_files = vv_lib.type_cast_num(arg)
		# SVG path translation
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)		
	
	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name, " for transformation "+transf_string)
	  
	if(nb_files != len(input_file_name_list)):
		print( "incoherent arguments to main: announced number of input files: ",nb_files," number of file names provided :", len(input_file_name_list),"!")
		for inFile  in input_file_name_list :
			print("Input file (", inFile, ")")
		return 0
	
	# radial gradient is for the beam lines of the transformation C_LINES
	FILEout.write( '''<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>
	<svg
	   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"
	   xmlns:cc=\"http://creativecommons.org/ns#\"
	   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"
	   xmlns:svg=\"http://www.w3.org/2000/svg\"
	   xmlns=\"http://www.w3.org/2000/svg\"
	   xmlns:xlink=\"http://www.w3.org/1999/xlink\"
	   id=\"svg284\"
	   preserveAspectRatio=\"xMidYMid meet\"
	   viewBox=\"0 0 3860.000000 2180.000000\"
	   height=\"2180.000000pt\"
	   width=\"3860.000000pt\"
	   version=\"1.0\">
	  <defs
		 id=\"defs124\">
		<linearGradient
		   id=\"linearGradient001\">
		  <stop
			 id=\"stop951\"
			 offset=\"0\"
			 style=\"stop-color:#000000stop-opacity:0.15\" />
		  <stop
			 id=\"stop953\"
			 offset=\"1\"
			 style=\"stop-color:#000000stop-opacity:1\" />
		</linearGradient>
		<radialGradient
		   gradientUnits=\"userSpaceOnUse\"
		   gradientTransform=\"matrix(1.3700451,-0.02534341,0.01423842,0.76971741,-7276.1383,2027.7884)\"
		   r=\"14030.874\"
		   fy=\"12028.837\"
		   fx=\"19200\"
		   cy=\"12028.837\"
		   cx=\"19200\"
		   id=\"radialGradient001\"
		   xlink:href=\"#linearGradient001\" />
	  </defs>
	  <metadata id=\"metadata7\">
	Created by potrace 1.13, written by Peter Selinger 2001-2015
	</metadata>
	''')

	nb_groups = 0
	parallel_lines = []
	sequenced_lines = ""
	# lines produced by C_LINES transformation
	sequenced_beam_lines = ""
	is_beam_group = 0
	for i in range(nb_files) :
		# print("input file i: ", input_file_name_list[i])
		try:
			FILEin = open(input_file_name_list[i], "rt")
		except IOError:
			print("File not found :", input_file_name_list[i], " for index "+str(i))

		# composition in sequence: the image paths are piled: first the paths of image 1 from gray
		# to black then the paths of image 2, etc.
		# the underlying images can fill the blank spaces of the upper ones
		if( composition_mode == SEQUENCE) :
			line = FILEin.readline()
			line = line.rstrip()
			while(line):
				if(re.search(r'<g', line) != None) :
					out_line = ""
					is_beam_group = 0
					if(re.search(r' $', line) == None) :
						line = line + " "
					
					# lines produced by C_LINES transformation have a radial gradient
					if(re.search(r'radialGradient', line) != None) :
						is_beam_group = 1
					
					out_line = out_line + line

					while( line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						# lines produced by C_LINES transformation have a radial gradient
						if(re.search(r'radialGradient', line) != None) :
							is_beam_group = 1
						
						out_line = out_line + line
					
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + out_line + "\n"
					else :
						sequenced_lines = sequenced_lines + out_line + "\n"
				
				elif(re.search(r'<path', line) != None) :
					out_line = ""
					out_line = out_line + line
					while( line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						out_line = out_line + " " + line
					
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + out_line + "\n"
					else :
						sequenced_lines = sequenced_lines + out_line + "\n"
				
				elif(re.search(r'</g>', line) != None) :
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + line
					else :
						sequenced_lines = sequenced_lines + line
				line = FILEin.readline()
				line = line.rstrip()
			
			FILEin.close()
		
		# the composition mode in parallel leads to a different result: 
		# each color layer is combined between all the files.
		elif( composition_mode == PARALLEL) :
			ind_group = 0
			line = FILEin.readline()
			line = line.rstrip()
			group_parallel_line  = ""
			while(line):
				# print("crossfaded line : (",line,")")
				if(re.search(r'<g', line) != None) :
					out_line = ""
					is_beam_group = 0
					# string initialization
					group_parallel_line  = ""
					if(re.search(r' $', line) == None) :
						line = line + " "
					
					# lines produced by C_LINES transformation have a radial gradient
					if(re.search(r'radialGradient', line) != None) :
						is_beam_group = 1
					
					# print FILEout line
					out_line = out_line + line

					while( line and re.search(r'>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()

						# lines produced by C_LINES transformation have a radial gradient
						if(re.search(r'radialGradient', line) != None) :
							is_beam_group = 1
						
						# print FILEout line
						out_line = out_line + line						
					
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + out_line + "\n"
					else :
						group_parallel_line = group_parallel_line + out_line + "\n"
					
					# print FILEout "\n"
				
				elif(re.search(r'<path', line) != None) :
					out_line = ""
					# print FILEout line
					out_line = out_line + line
					while( line and re.search(r'/>', line) == None) :
						line = FILEin.readline()
						line = line.rstrip()
						out_line = out_line + " " + line
						# print FILEout " " , line
					
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + out_line + "\n"
					else :
						group_parallel_line = group_parallel_line + out_line + "\n"
					
					# print FILEout "\n"
				
				elif(re.search(r'</g>', line) != None) :
					if(is_beam_group) :
						sequenced_beam_lines = sequenced_beam_lines + line + "\n"
					else :
						group_parallel_line = group_parallel_line + line + "\n"
					
					# print FILEout line
					parallel_lines.append(group_parallel_line)
					ind_group += 1

				line = FILEin.readline()
				line = line.rstrip()
				
			FILEin.close()
			if( nb_groups < ind_group ) :
				nb_groups = ind_group
		
	
	# lines produced by C_LINES transformation are displayed first so that the characters are on top of them
	FILEout.write( sequenced_beam_lines )
	if( composition_mode == PARALLEL) :
		for ind_group in range(len(parallel_lines)) :
			FILEout.write(parallel_lines[ind_group])
	elif( composition_mode == SEQUENCE) :
		FILEout.write(sequenced_lines)
	
	FILEout .write("</svg>\n")
	FILEout.close()

	return 1
