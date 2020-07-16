#!/usr/bin/python3 

# use strict
# use warnings

# package SVG_movie_posterize

# use lib '/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization-scenario/'
# use SVG_movie_layer_composition

# SVG_movie_posterize.pm
# composition of color paths into a single file

import os

import getopt
import sys

import SVG_scenario_interpolation_lib
import SVG_movie_layer_composition

USAGE = '''usage: SVG_movie_posterize.pm  --nb-layers <integer>  --dimming <dimming_coefficient> -o <file_name> -i <file_name> 
/mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/SVG_movie_posterize.pm  --tmp /mnt/d/sync.com/Sync/LYM-sources/SVG_movie/SVG_posterization/tmp  --nb-layers 4  --dimming 1 -o /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_SVGs/VID_20200307_152223/VID_20200307_152223_0300.svg -i /mnt/d/sync.com/Sync/YN-works/YN_BibosClip_2020/SVG_movies/movie_PNGs/VID_20200307_152223/VID_20200307_152223_0300.png

ARGUMENTS:
    --nb_layers: number of layers for posterization
    --dimming: coefficient for dimming the threshold values of the layers
   -o: ouput files 
   -i: input files
    --tmp: tmp directory
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	output_format = ""
	nb_layers = 0
	dimming = 0
	# output_file_name_PATHs
	output_file_name_SVGs = ""
	input_file_name = ""
	tmp_dir = ""

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=","dimming=","tmp="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name_SVGs = arg
		elif opt in ("--nb-layers"):
			nb_layers = SVG_scenario_interpolation_lib.type_force_num(arg)
		elif opt in ("--dimming"):
			dimming = SVG_scenario_interpolation_lib.type_force_num(arg)
		elif opt in ("--tmp"):
			tmp_dir = arg
		# SVG path translation
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)		

	os.system( "convert "+input_file_name+" -crop 1888x1062+16+9 -resize 1920x1080 "+tmp_dir+"/tmp.bmp" ) 
	print( "convert "+input_file_name+" -crop 1888x1062+16+9 -resize 1920x1080 "+tmp_dir+"/tmp.bmp" )

	################################################
	# PATHS GENERATION
	if(nb_layers == 4) :
		# -t n means that the pixel darker than this value are taken
		# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
		# indLayer = 0
		indLayer = 3
		interval = dimming / (nb_layers + 1)
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp1.pbm -x -t "+str(threshold)+" -s 2" ) 
		# indLayer++
		indLayer -= 1
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp2.pbm -x -t "+str(threshold)+" -s 2" ) 
		# indLayer++
		indLayer -= 1
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp3.pbm -x -t "+str(threshold)+" -s 2" ) 
		# indLayer++
		indLayer -= 1
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp4.pbm -x -t "+str(threshold)+" -s 2" ) 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		os.system( "potrace "+tmp_dir+"/tmp1.pbm -o "+tmp_dir+"/tmp1.svg  --svg -t 24 -O 2  --flat" ) 
		os.system( "potrace "+tmp_dir+"/tmp2.pbm -o "+tmp_dir+"/tmp2.svg  --svg -t 24 -O 2  --flat" ) 
		os.system( "potrace "+tmp_dir+"/tmp3.pbm -o "+tmp_dir+"/tmp3.svg  --svg -t 24 -O 2  --flat" ) 
		os.system( "potrace "+tmp_dir+"/tmp4.pbm -o "+tmp_dir+"/tmp4.svg  --svg -t 24 -O 2  --flat" ) 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		# print( "SVG_movie_layer_composition","--output-format", "SVG", \
			# "--nb-files", 4, \
			# "-o", output_file_name_SVGs, \
			# "-i", tmp_dir+"/tmp1.svg "+tmp_dir+"/tmp2.svg "+tmp_dir+"/tmp3.svg "+tmp_dir+"/tmp4.svg", \
			# "-c", "#c0c0c0 #808080 #404040 #000000" )
		if( SVG_movie_layer_composition.main(["--output-format", "SVG", \
			"--nb-files", 4, \
			"-o", output_file_name_SVGs, \
			"-i", tmp_dir+"/tmp1.svg "+tmp_dir+"/tmp2.svg "+tmp_dir+"/tmp3.svg "+tmp_dir+"/tmp4.svg", \
			"-c", "#c0c0c0 #808080 #404040 #000000" ]) == 0) :
			return 0
		
	
	elif(nb_layers == 2) :
		# -t n means that the pixel darker than this value are taken
		# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
		# indLayer = 0
		indLayer = 1
		interval = dimming / (nb_layers + 1)
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp1.pbm -x -t "+str(threshold)+" -s 2" ) 
		# indLayer++
		indLayer -= 1
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp2.pbm -x -t "+str(threshold)+" -s 2" ) 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		os.system( "potrace "+tmp_dir+"/tmp1.pbm -o "+tmp_dir+"/tmp1.svg  --svg -t 24 -O 2  --flat" ) 
		os.system( "potrace "+tmp_dir+"/tmp2.pbm -o "+tmp_dir+"/tmp2.svg  --svg -t 24 -O 2  --flat" ) 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		print( "SVG_movie_layer_composition","--output-format", "SVG", \
			"--nb-files", 2, \
			"-o", output_file_name_SVGs, \
			"-i", tmp_dir+"/tmp1.svg "+tmp_dir+"/tmp2.svg", \
			"-c", "#808080 #000000"  )
		if( SVG_movie_layer_composition.main(["--output-format", "SVG", \
			"--nb-files", 2, \
			"-o", output_file_name_SVGs, \
			"-i", tmp_dir+"/tmp1.svg "+tmp_dir+"/tmp2.svg", \
			"-c", "#808080 #000000" ]) == 0) :
			return 0
		
	
	elif(nb_layers == 1) :
		# -t n means that the pixel darker than this value are taken
		# portable pixmap format (PPM), portable graymap format (PGM) portable bitmap format (PBM). 
		# indLayer = 0
		indLayer = 1
		interval = dimming / (nb_layers + 1)
		indLayer -= 1
		# threshold = 1. - (indLayer + 1) * interval
		threshold = (indLayer + 1) * interval
		os.system( "mkbitmap "+tmp_dir+"/tmp.bmp -o "+tmp_dir+"/tmp1.pbm -x -t "+str(threshold)+" -s 2" ) 
		# go from larger and grighter areas to smaller and darker 
		# the layers should be piled in this order in SVG and colored from brighter to darker
		os.system( "potrace "+tmp_dir+"/tmp1.pbm -o "+tmp_dir+"/tmp1.svg  --svg -t 24 -O 2  --flat" ) 
		# concatenates to either a sequence of paths that will be fed to the GPU or to an SVG file
		# to be edited or to its conversion into png or jpg to make a movie from
		print( "SVG_movie_layer_composition", "--output-format", "SVG", \
			"--nb-files", 1, \
			"-o", output_file_name_SVGs, \
			"-i", tmp_dir+"/tmp1.svg", \
			"-c", "#000000" )
		if( SVG_movie_layer_composition.main(["--output-format", "SVG", \
			"--nb-files", 1, \
			"-o", output_file_name_SVGs, \
			"-i", tmp_dir+"/tmp1.svg", \
			"-c", "#000000"] ) == 0) :
			return 0
		
	
	else :
		print( "Unprocessed number of layers "+nb_layers+", currently only 1, 2, or 4 layers are accepted")
		return 0
	
	return 1

if __name__ == "__main__":
	import sys
	main(sys.argv[1:])