#!/usr/bin/perl 

# use strict
# use warnings

# package SVG_movie_layer_composition

# use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/'
# use SVG_scenario_interpolation_lib

# SVG_movie_layer_composition_image.pm
# SVG_movie_layer_compositions an image into another image or into a SVG vector graphics

# usage
# SVG_movie_layer_composition_image.pm --output-format SVG --nb_files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg 

import os

import getopt
import sys

import re

import SVG_scenario_interpolation_lib


USAGE = '''usage: SVG_movie_layer_composition_image.pm --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_movie_layer_composition_image.pm --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"#cbcbcb\" \"#9a9a9a\" \"#6b6b6b\" \"#1c1c1c\"

ARGUMENTS:
   --output-format: output format of the composited image (SVG, paths for GPU shipping, image)
   --nb_files: number of input images
   -o: ouput files 
   -i: input files
   -c: input colors
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	SVG  = 1
	PNG  = 2
	JPG  = 3
	PATHS  = 4
	
	file_format = { \
	   "SVG" : SVG,\
	   "PNG" : PNG,\
	   "JPG" : JPG,\
	   "PATHS" : PATHS }

	output_format = SVG
	nb_files = 0
	output_file_name = ""
	input_file_names = []
	path_colors = []

	try:
		opts, args = getopt.getopt(main_args,"i:o:c:",["inputfile=","outputfile=","nb-files=","output-format=","colors="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_names_string = arg
			input_file_names = re.split(r' +',input_file_names_string.rstrip())
		elif opt in ("-c", "--colors"):
			path_colors_string = arg
			path_colors = re.split(r' +',path_colors_string.rstrip())
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb-files"):
			nb_files = SVG_scenario_interpolation_lib.type_cast_num(arg)
		elif opt in ("--output-format"):
			output_format_string = arg
			if not(output_format_string in file_format.keys()) :
				print( USAGE )
				print( "Unknown file format ",output_format_string )
				return 0
			output_format = file_format[output_format_string]
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)

	if(nb_files != len(input_file_names)):
		assert False, "number of input files does not correspond to parameter --nb-files"
		print(USAGE)
		sys.exit(2)

	if(nb_files != len(path_colors)):
		assert False, "number of input files does not correspond to parameter colors -c"
		print(USAGE)
		sys.exit(2)

	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name)

	print("Generating from ", input_file_names_string, " file ", output_file_name," nb layers: ", nb_files)

	if(output_format == SVG) :
		FILEout.write( '''<?xml version=\"1.0\" standalone=\"no\"?>
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"
 \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">
<svg version=\"1.0\" xmlns=\"http://www.w3.org/2000/svg\"
 width=\"3860.000000pt\" height=\"2180.000000pt\" viewBox=\"0 0 3860.000000 2180.000000\"
 preserveAspectRatio=\"xMidYMid meet\">
<metadata>
Created by potrace 1.14, written by Peter Selinger 2001-2017
</metadata>
''')
	
	line = ""
	for i in range(nb_files) :
		try:
			FILEin = open(input_file_names[i], "rt")
		except IOError:
			print("File not created :", input_file_names[i])
		
		line = FILEin.readline()
		line = line.rstrip()
		while(line):
			if(re.search(r'<g', line) != None) :
				line = line.rstrip()
				if(re.search(r'fill\=\"\#000000\"', line) != None) :
					line = line.replace(r'fill="#000000"', ' style="fill:'+path_colors[i]+';"')
				
				if(output_format == SVG) :
					if( re.search(r' ', line) == None) :
						line = line + " "
					FILEout.write(line)
				
				while( line and re.search(r'>', line) == None) :
					line = FILEin.readline()
					line = line.rstrip()
					if(re.search(r'fill\=\"\#000000\"', line) != None) :
						line = line.replace(r'fill="#000000"', ' style="fill:'+path_colors[i]+';"')
					
					if(output_format == SVG) :
						FILEout.write(line)
				
				FILEout.write("\n")
			
			elif(re.search(r'<path', line) != None) :
				if(output_format == PATHS) :
					line = line.replace(r'<path', '<path style="fill:'+path_colors[i]+';"')
				
				if(output_format == PATHS or output_format == SVG) :
					line = line.rstrip()
					FILEout.write(line)
				
				while( line and re.search(r'/>', line) == None) :
					line = FILEin.readline()
					line = line.rstrip()
					if(output_format == PATHS or output_format == SVG) :
						line = line.rstrip()
						FILEout.write(" "+line)

				if(output_format == PATHS or output_format == SVG) :
					FILEout.write("\n")
				
			
			elif(re.search(r'</g>', line) != None) :
				if(output_format == SVG) :
					FILEout.write(line+"\n")
		
			# next line
			line = FILEin.readline()
			line = line.rstrip()
		
		FILEin.close()
	
	if(output_format == SVG) :
		FILEout.write("</svg>\n")
	
	FILEout.close()
	return 1


if __name__ == "__main__":
	import sys
	main(sys.argv[1:])