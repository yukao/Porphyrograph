#!/usr/bin/perl 

# use strict
# use warnings

# package SVG_movie_fillAndStroke

# SVG_movie_fillAndStroke.pm
# composition of color paths into a single file

# usage
# /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/SVG_movie_fillAndStroke.pm --nb-layers 4 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_transformed_SVGs/TawTra/Intro_0011.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/tmp/tmp_transformed.svg --style #FEFEFE #509D76 #CAB835 #A11A1B #000000 0.791666666666667 0.791666666666667 0.791666666666667 1 #000000 1 2.08333333333333

import getopt
import sys

import re

import vv_lib

USAGE = '''usage: SVG_movie_fillAndStroke.pm --nb-layers <integer> -o <file_name> -i <file_name> --style <color>5 <float>4 <color> <float>2

ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --style: fill and stroke style
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :

	##################################################################
	# ARGUMENTS OF THE COMMAND
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","style="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb-layers"):
			nb_layers = arg
		elif opt in ("--style"):
			style = arg
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)		

	fill_color = ["","#000000","#000000","#000000","#000000"]
	fill_opacity = [0,1,1,1,1]

	style_argts = re.split('x', style.rstrip())
	indArgt = 0
	fill_color_bg = style_argts[indArgt] 
	indArgt += 1
	fill_color[1] = style_argts[indArgt] 
	indArgt += 1
	fill_color[2] = style_argts[indArgt] 
	indArgt += 1
	fill_color[3] = style_argts[indArgt] 
	indArgt += 1
	fill_color[4] = style_argts[indArgt] 
	indArgt += 1
	fill_opacity[1] = style_argts[indArgt] 
	indArgt += 1
	fill_opacity[2] = style_argts[indArgt] 
	indArgt += 1
	fill_opacity[3] = style_argts[indArgt] 
	indArgt += 1
	fill_opacity[4] = style_argts[indArgt] 
	indArgt += 1
	stroke_color = style_argts[indArgt] 
	indArgt += 1
	stroke_opacity = style_argts[indArgt] 
	indArgt += 1
	stroke_width = style_argts[indArgt] 
	indArgt += 1

	# file opening
	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name, " for transformation "+transf_string)

	try:
		FILEin = open(input_file_name, "rt")
	except IOError:
		print("File not found :", input_file_name, " for transformation "+transf_string)

	group_no = 1
	contents = FILEin.read()
	file_lines = contents.splitlines()
	for line in file_lines:
		##################################################################
		# changes fill and stroke
		##################################################################
		if( re.search(r'^<g ', line) != None ) :
			# background rectangle
			if(group_no == 1) :
				FILEout.write('''<rect
		y=\"-10\"
		x=\"-10\"
		height=\"2190\"
		width=\"3870\"
		id=\"rect834\"
		style=\"opacity:1; fill:'''+fill_color_bg+'''; fill-opacity:1; stroke:none;\" />\n''')
			
			# typical group line
			# <g transform="translate(0.000000,2160.000000) scale(0.100000,-0.100000)" style="fill:#c0c0c0" stroke="none">
			line = re.sub(r'stroke="none"', r'stroke="'+str(stroke_color)+'" stroke-width="'+str(stroke_width)+'" stroke-opacity="'+str(stroke_opacity)+'"', line)
			sel_fill_color = fill_color[group_no]
			sel_fill_opacity = fill_opacity[group_no]
			line = re.sub(r'fill:#([0-9a-fA-F]+);', 'fill:'+sel_fill_color+'; fill-opacity:'+sel_fill_opacity+'; ', line)
			# group for a line beam from the C_LINES transformation
			line = re.sub(r'(stroke\:url\(\#radialGradient001\);)', r'\1 stroke-width:15; ', line)
			# prints the modified group
			FILEout.write(line+"\n")
			group_no += 1
		
		##########################################################################
		# non path and non group line reprinted
		##########################################################################
		else :
			FILEout.write(line+"\n")
	
	FILEin.close()
	FILEout.close()
	return 1

if __name__ == "__main__":
    import sys
    main(sys.argv[1:])
