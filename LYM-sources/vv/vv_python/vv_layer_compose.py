#!/usr/bin/python3 

# vv_layer_compose.py
# builds a single vector video frame from several layers

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
from signal import signal, SIGINT

import re

from vv_lib import to_num

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_layer_compose.py --output-format (SVG|PATHS|JPG|PNG) --nb-files <integer> -o <file_name> -i <file_name>+
vv_layer_compose.py --output-format SVG --nb-files 4 -o mer_calme.svg -i mer_calme1.svg mer_calme2.svg mer_calme3.svg mer_calme4.svg -c \"#cbcbcb\" \"#9a9a9a\" \"#6b6b6b\" \"#1c1c1c\"
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
	fill_and_stroke_from_group = False
	add_id = False
	width = 3840
	height = 2160

	try:
		opts, args = getopt.getopt(main_args,"i:o:c:",["inputfile=","outputfile=","nb-files=","output-format=","colors=","add_id=","width=","height=","fill_and_stroke_from_group="])
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
			nb_files = to_num(arg)
		elif opt in ("--fill_and_stroke_from_group"):
			fill_and_stroke_from_group = arg
		elif opt in ("--add_id"):
			add_id = arg
		elif opt in ("--width"):
			width = arg
		elif opt in ("--height"):
			height = arg
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

	# print("Generating from ", input_file_names_string, " file ", output_file_name," nb layers: ", nb_files)

	if(output_format == SVG) :
		FILEout.write( '''<?xml version=\"1.0\" standalone=\"no\"?>
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\"
 \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">
<svg version=\"1.0\" xmlns=\"http://www.w3.org/2000/svg\"
 width=\"{0}pt\" height=\"{1}pt\" viewBox=\"0 0 {0} {1}\"
 preserveAspectRatio=\"xMidYMid meet\">
<metadata>
Created by potrace 1.14, written by Peter Selinger 2001-2017
</metadata>
'''.format(width, height))
	
	line = ""
	path_no = 1
	for i in range(nb_files) :
		try:
			FILEin = open(input_file_names[i], "rt")
		except IOError:
			print("File not created :", input_file_names[i])
		
		line = FILEin.readline()
		line = line.rstrip()
		group_fill = ''
		group_stroke = ''
		while(line):
			if(re.search(r'<g', line) != None) :
				group_fill = ''
				group_stroke = ''

				line = line.rstrip()

				if(re.search(r'fill\=\"\#000000\"', line) != None) :
					line = line.replace(r'fill="#000000"', ' style="fill:'+path_colors[i]+';"')

				result_fill = re.search(r'fill\=\"([^\"]*)\"', line)
				if result_fill != None :
					group_fill = 'style="fill:'+result_fill.group(1)+';"'
				else :
					result_fill = re.search(r'style\=\"fill\:([^\;]*)\;\"', line)
					if result_fill != None :
						group_fill = 'style="fill:'+result_fill.group(1)+';"'
				result_stroke = re.search(r'\s(stroke=\"[^\"]*\")', line)
				if result_stroke != None :
					group_stroke = result_stroke.group(1)

				if(output_format == SVG) :
					if( re.search(r' ', line) == None) :
						line = line + " "
					FILEout.write(line)
				
				while( line and re.search(r'>', line) == None) :
					line = FILEin.readline()
					line = line.rstrip()

					if(re.search(r'fill\=\"\#000000\"', line) != None) :
						line = line.replace(r'fill="#000000"', ' style="fill:'+path_colors[i]+';"')
					
					result_fill = re.search(r'fill\=\"([^\"]*)\"', line)
					if result_fill != None :
						group_fill = 'style="fill:'+result_fill.group(1)+';"'
					else :
						result_fill = re.search(r'style\=\"fill\:([^\;]*)\;\"', line)
						if result_fill != None :
							group_fill = 'style="fill:'+result_fill.group(1)+';"'
					result_stroke = re.search(r'\s(stroke=\"[^\"]*\")', line)
					if result_stroke != None :
						group_stroke = result_stroke.group(1)

					if(output_format == SVG) :
						FILEout.write(line)
								
				FILEout.write("\n")
			
			elif(re.search(r'<path', line) != None) :
				if(output_format == PATHS) :
					line = line.replace(r'<path', '<path style="fill:'+path_colors[i]+';"')

				if fill_and_stroke_from_group == True :
					separator = ' '
					line = line.replace(r'<path', separator.join(['<path', group_fill, group_stroke]))

				if add_id == True :
					separator = ' '
					line = line.replace(r'<path', separator.join(['<path', 'id="path_'+str(path_no)+'"']))

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

				path_no += 1
				
			
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
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])