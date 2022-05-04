#!/usr/bin/python3 

# vv_fillAndStroke.py
# changes fill and stroke properties of multilayer vector video frame

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import getopt
import sys
from signal import signal, SIGINT

import re

from vv_lib import to_num
from vv_lib import temperature_balance
from vv_lib import color_hsv_balance

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_fillAndStroke.py --nb-layers <integer> -o <file_name> -i <file_name> --style <color>5 <float>4 <color> <float>2
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
	style = ""
	fill_color =     ["","#000000","#000000","#000000","#000000","#000000","#000000","#000000","#000000"]
	fill_opacity =   ['0','1','1','1','1','1','1','1','1']
	stroke_color =   ["","#000000","#000000","#000000","#000000","#000000","#000000","#000000","#000000"]
	stroke_opacity = ['0','1','1','1','1','1','1','1','1']
	stroke_width =   ['0','0','0','0','0','0','0','0','0']
	blur =           ['0','0','0','0','0','0','0','0','0']
	nb_layers = 8
	temperature = 6600.0
	saturation = 0.0
	brightness = 0.0
	fill_and_stroke_mode = "by_groups"

	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=","outputfile=","nb-layers=",\
			"fill_color_bg=","fill_color_l1=","fill_color_l2=","fill_color_l3=","fill_color_l4=","fill_color_l5=","fill_color_l6=","fill_color_l7=","fill_color_l8=",\
			"fill_opacity_l1=","fill_opacity_l2=","fill_opacity_l3=","fill_opacity_l4=","fill_opacity_l5=","fill_opacity_l6=","fill_opacity_l7=","fill_opacity_l8=",\
			"stroke_color_bg=","stroke_color_l1=","stroke_color_l2=","stroke_color_l3=","stroke_color_l4=","stroke_color_l5=","stroke_color_l6=","stroke_color_l7=","stroke_color_l8=",\
			"stroke_opacity_l1=","stroke_opacity_l2=","stroke_opacity_l3=","stroke_opacity_l4=","stroke_opacity_l5=","stroke_opacity_l6=","stroke_opacity_l7=","stroke_opacity_l8=",\
			"stroke_width_l1=","stroke_width_l2=","stroke_width_l3=","stroke_width_l4=","stroke_width_l5=","stroke_width_l6=","stroke_width_l7=","stroke_width_l8=",\
			"temperature=","saturation=","brightness=",\
			"blur_l1=","blur_l2=","blur_l3=","blur_l4=","blur_l5=","blur_l6=","blur_l7=","blur_l8=",\
			"stroke_color=","stroke_opacity=","stroke_width=","fill_and_stroke_mode="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			input_file_name = arg
		elif opt in ("-o", "--outputfile"):
			output_file_name = arg
		elif opt in ("--nb-layers"):
			nb_layers = to_num(arg)
		elif opt in ("--fill_color_bg"):
			fill_color_bg = arg
		elif opt in ("--fill_color_l1"):
			fill_color[1] = arg
		elif opt in ("--fill_color_l2"):
			fill_color[2] = arg
		elif opt in ("--fill_color_l3"):
			fill_color[3] = arg
		elif opt in ("--fill_color_l4"):
			fill_color[4] = arg
		elif opt in ("--fill_color_l5"):
			fill_color[5] = arg
		elif opt in ("--fill_color_l6"):
			fill_color[6] = arg
		elif opt in ("--fill_color_l7"):
			fill_color[7] = arg
		elif opt in ("--fill_color_l8"):
			fill_color[8] = arg
		elif opt in ("--fill_opacity_l1"):
			fill_opacity[1] = to_num(arg)
		elif opt in ("--fill_opacity_l2"):
			fill_opacity[2] = to_num(arg)
		elif opt in ("--fill_opacity_l3"):
			fill_opacity[3] = to_num(arg)
		elif opt in ("--fill_opacity_l4"):
			fill_opacity[4] = to_num(arg)
		elif opt in ("--fill_opacity_l5"):
			fill_opacity[5] = to_num(arg)
		elif opt in ("--fill_opacity_l6"):
			fill_opacity[6] = to_num(arg)
		elif opt in ("--fill_opacity_l7"):
			fill_opacity[7] = to_num(arg)
		elif opt in ("--fill_opacity_l8"):
			fill_opacity[8] = to_num(arg)
		elif opt in ("--stroke_color_l1"):
			stroke_color[1] = arg
		elif opt in ("--stroke_color_l2"):
			stroke_color[2] = arg
		elif opt in ("--stroke_color_l3"):
			stroke_color[3] = arg
		elif opt in ("--stroke_color_l4"):
			stroke_color[4] = arg
		elif opt in ("--stroke_color_l5"):
			stroke_color[5] = arg
		elif opt in ("--stroke_color_l6"):
			stroke_color[6] = arg
		elif opt in ("--stroke_color_l7"):
			stroke_color[7] = arg
		elif opt in ("--stroke_color_l8"):
			stroke_color[8] = arg
		elif opt in ("--stroke_opacity_l1"):
			stroke_opacity[1] = to_num(arg)
		elif opt in ("--stroke_opacity_l2"):
			stroke_opacity[2] = to_num(arg)
		elif opt in ("--stroke_opacity_l3"):
			stroke_opacity[3] = to_num(arg)
		elif opt in ("--stroke_opacity_l4"):
			stroke_opacity[4] = to_num(arg)
		elif opt in ("--stroke_opacity_l5"):
			stroke_opacity[5] = to_num(arg)
		elif opt in ("--stroke_opacity_l6"):
			stroke_opacity[6] = to_num(arg)
		elif opt in ("--stroke_opacity_l7"):
			stroke_opacity[7] = to_num(arg)
		elif opt in ("--stroke_opacity_l8"):
			stroke_opacity[8] = to_num(arg)
		elif opt in ("--stroke_width_l1"):
			stroke_width[1] = to_num(arg)
		elif opt in ("--stroke_width_l2"):
			stroke_width[2] = to_num(arg)
		elif opt in ("--stroke_width_l3"):
			stroke_width[3] = to_num(arg)
		elif opt in ("--stroke_width_l4"):
			stroke_width[4] = to_num(arg)
		elif opt in ("--stroke_width_l5"):
			stroke_width[5] = to_num(arg)
		elif opt in ("--stroke_width_l6"):
			stroke_width[6] = to_num(arg)
		elif opt in ("--stroke_width_l7"):
			stroke_width[7] = to_num(arg)
		elif opt in ("--stroke_width_l8"):
			stroke_width[8] = to_num(arg)
		elif opt in ("--blur_l1"):
			blur[1] = to_num(arg)
		elif opt in ("--blur_l2"):
			blur[2] = to_num(arg)
		elif opt in ("--blur_l3"):
			blur[3] = to_num(arg)
		elif opt in ("--blur_l4"):
			blur[4] = to_num(arg)
		elif opt in ("--blur_l5"):
			blur[5] = to_num(arg)
		elif opt in ("--blur_l6"):
			blur[6] = to_num(arg)
		elif opt in ("--blur_l7"):
			blur[7] = to_num(arg)
		elif opt in ("--blur_l8"):
			blur[8] = to_num(arg)
		elif opt in ("--stroke_color"):
			for ind_layer in range(nb_layers) :
				stroke_color[ind_layer+1] = arg
		elif opt in ("--stroke_opacity"):
			for ind_layer in range(nb_layers) :
				stroke_opacity[ind_layer+1] = to_num(arg)
		elif opt in ("--stroke_width"):
			for ind_layer in range(nb_layers) :
				stroke_width[ind_layer+1] = to_num(arg)
		elif opt in ("--temperature"):
			temperature = to_num(arg)
		elif opt in ("--saturation"):
			saturation = to_num(arg)
		elif opt in ("--brightness"):
			brightness = to_num(arg)
		elif opt in ("--fill_and_stroke_mode"):
			if arg == "by_groups" :
				fill_and_stroke_mode = "by_groups"
			elif arg == "by_paths" :
				fill_and_stroke_mode = "by_paths"
			else :
				print("argts", main_args)
				assert False, "unhandled fill_and_stroke_mode "+arg
				print(USAGE)
				sys.exit(2)		
		else:
			print("argts", main_args)
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)		

	# print("fill_color", fill_color)

	# file opening
	try:
		FILEout = open(output_file_name, "wt")
	except IOError:
		print("File not created :", output_file_name, " (fill and stroke)")

	try:
		FILEin = open(input_file_name, "rt")
	except IOError:
		print("File not found :", input_file_name, " (fill and stroke)")

	group_no = 1
	path_no = 1
	contents = FILEin.read()
	file_lines = contents.splitlines()
	##################################################################
	# ALL THE LINES IN A SVG FILE
	for line in file_lines:
		##################################################################
		# METADATA LINE: adds <defs> for the blur filters
		if( re.search(r'<metadata>', line) != None ) :
			one_def = False
			for ind_layer in range(nb_layers) :
				if(len(blur) > (ind_layer + 2) and blur[ind_layer + 1] > 0):
					if(not one_def):
						# prints the defs opening
						FILEout.write('<defs id="defs_blurs">\n')
						one_def = True
					# prints the blur filter
					FILEout.write('    <filter id=\"blur'+str(ind_layer + 1)+'\">\n')
					FILEout.write('        <feGaussianBlur stdDeviation=\"'+format(blur[ind_layer + 1], '.8f')+'\" />\n')
					FILEout.write('    </filter>\n')

			if(one_def):
				# prints the defs closing
				FILEout.write('</defs>\n')

			# prints the line
			FILEout.write(line+"\n")

		##################################################################
		# GROUP LINE: changes blur and, fill and stroke if "by_groups" mode
		elif( re.search(r'^<g ', line) != None and group_no <= 8) :
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
			
			if fill_and_stroke_mode == "by_groups" :
				# sets stroke color, stroke width and stroke opacity 
				sel_stroke_color = stroke_color[group_no]
				# transforms color according to temperature
				if(temperature != 6600):			
					sel_stroke_color = temperature_balance(sel_stroke_color, temperature)
				if(brightness != 0 or saturation != 0):			
					sel_stroke_color = color_hsv_balance (sel_stroke_color, 0.0, saturation, brightness) # hue not changed (this is managed through temperature)
				sel_stroke_opacity = stroke_opacity[group_no]
				sel_stroke_width = stroke_width[group_no]
				line = re.sub(r'stroke="none"', '', line)
				line = re.sub(r'stroke:[^;]+;', '', line)
				line = re.sub(r'stroke-width:[^;]+;', '', line)
				line = re.sub(r'stroke-opacity:[^;]+;', '', line)
				# print("sel_stroke_color sel_stroke_width sel_stroke_opacity", sel_stroke_color, " ", sel_stroke_width, " ", sel_stroke_opacity)
				line = re.sub(r'fill:', 'stroke:'+str(sel_stroke_color)+'; stroke-width:'+format(float(sel_stroke_width), '.8f')+'px; stroke-opacity:'+format(float(sel_stroke_opacity), '.8f')+'; fill:', line)
				
				# sets fill color, fill opacity and possible blur
				sel_fill_color = fill_color[group_no]
				if(temperature != 6600):			
					sel_fill_color = temperature_balance(sel_fill_color, temperature)
				if(brightness != 0 or saturation != 0):			
					sel_fill_color = color_hsv_balance (sel_fill_color, 0.0, saturation, brightness) # hue not changed (this is managed through temperature)
				sel_fill_opacity = fill_opacity[group_no]
				sel_blur = blur[group_no]
				if(sel_blur <= 0):
					line = re.sub(r'fill-opacity:[^;]+;', '', line)
					line = re.sub(r'fill:#([0-9a-fA-F]+);', 'fill:'+str(sel_fill_color)+'; fill-opacity:'+format(sel_fill_opacity, '.8f')+'; ', line)
				else:
					line = re.sub(r'fill-opacity:[^;]+;', '', line)
					line = re.sub(r'fill:#([0-9a-fA-F]+);', 'fill:'+str(sel_fill_color)+'; fill-opacity:'+format(sel_fill_opacity, '.8f')+'; filter:url(#blur'+str(group_no)+');', line)
				
				# group for a line beam from the C_LINES transformation
				line = re.sub(r'(stroke\:url\(\#radialGradient001\);)', r'\1 stroke-width:15; ', line)
			
			# prints the modified group
			FILEout.write(line+"\n")
			group_no += 1
		
		##################################################################
		# PATH LINE: changes blur and, fill and stroke if "by_paths" mode
		elif( re.search(r'^<path ', line) != None and path_no <= 8) :
			
			# typical path line
			# <path id="drawing_blue" style="fill:#56b3e4;stroke-width:0.245929" d="M 504.85047..." />
			
			if fill_and_stroke_mode == "by_paths" :
				# removes possible previous values
				line = re.sub(r'stroke:[^;]+;', '', line)
				line = re.sub(r'stroke="[^\"]"', '\"', line)
				line = re.sub(r'stroke-width:[^;]+;', '', line)
				line = re.sub(r'stroke-width:[^\"]+\"', '\"', line)
				line = re.sub(r'stroke-opacity:[^;]+;', '', line)
				line = re.sub(r'stroke-opacity:[^\"]+\"', '\"', line)
				line = re.sub(r'fill-opacity:[^;]+;', '', line)
				line = re.sub(r'fill-opacity:[^\"]+\"', '\"', line)
				line = re.sub(r'fill:[^;]+;', '', line)
				line = re.sub(r'fill:[^\"]+\"', '\"', line)

				# sets stroke color, stroke width and stroke opacity 
				sel_stroke_color = stroke_color[path_no]
				# transforms color according to temperature
				if(temperature != 6600):			
					sel_stroke_color = temperature_balance(sel_stroke_color, temperature)
				if(brightness != 0 or saturation != 0):			
					sel_stroke_color = color_hsv_balance (sel_stroke_color, 0.0, saturation, brightness) # hue not changed (this is managed through temperature)
				sel_stroke_opacity = stroke_opacity[path_no]
				sel_stroke_width = stroke_width[path_no]
				
				# sets fill color, fill opacity and possible blur
				sel_fill_color = fill_color[path_no]
				if(temperature != 6600):			
					sel_fill_color = temperature_balance(sel_fill_color, temperature)
				if(brightness != 0 or saturation != 0):			
					sel_fill_color = color_hsv_balance (sel_fill_color, 0.0, saturation, brightness) # hue not changed (this is managed through temperature)
				sel_fill_opacity = fill_opacity[path_no]
				sel_blur = blur[path_no]

				line = re.sub(r'\sstyle="', ' style="stroke:'+str(sel_stroke_color)+'; stroke-width:'+format(sel_stroke_width, '.8f')+'px; stroke-opacity:'+format(sel_stroke_opacity, '.8f')+'; ', line)
				if(sel_blur <= 0):
					line = re.sub(r'\sstyle="', ' style="fill:'+str(sel_fill_color)+'; fill-opacity:'+format(sel_fill_opacity, '.8f')+'; ', line)
				else:
					line = re.sub(r'\sstyle="', ' style="fill:'+str(sel_fill_color)+'; fill-opacity:'+format(sel_fill_opacity, '.8f')+'; filter:url(#blur'+str(path_no)+');', line)

			# prints the modified group
			FILEout.write(line+"\n")
			path_no += 1
		
		##########################################################################
		# non metadata and non group line reprinted
		else :
			FILEout.write(line+"\n")
	
	FILEin.close()
	FILEout.close()
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
