#!/usr/bin/python3 

# vv_generate_movie_from_path.py
# generate images from a path over a vector graphics

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
from signal import signal, SIGINT

import re
import math

import vv_const
from vv_lib import force_num

import numpy as np

import bezier

sys.path.append('/mnt/c/sync.com/Sync/LYM-sources/vv/vv_python/')
import vv_transform

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_generate_movie_from_path.py --drawing-paths-IDs [<ID1>,<ID1>,....]  --camera-path-ID <ID>  --nb-frames <frame-number> -o <file_name> -i <file_name> 
ARGUMENTS:
'''

def length(x,y,X,Y) :
	return math.sqrt((X-x)*(X-x) + (Y-y)*(Y-y))

##################################################################
# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
##################################################################
def normalize_file_paths(input_lines) :
	output_lines = []
	# print("Initial file path: ", input_file_name)
	# print("Initial file [", input_lines, "]")
	indLine = 0
	while( indLine < len(input_lines)) :
		line_string = input_lines[indLine]
		indLine += 1	
		if(re.search(r'<path', line_string) != None) :
			line_string = re.sub(r'^[\s]+<path', r'<path', line_string)
			while(re.search(r'/>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1	
			# transforms , into space
			line_string = re.sub(r'\,', r' ', line_string)

			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
			    print('Expected style not found')
			if result_ID and result_path :
				line_string = '<path id="{0!s}" style="{1!s}" d="{2!s}"/>'.format(result_ID.group(1), result_style.group(1), result_path.group(1))
		elif(re.search(r'<g', line_string) != None) :
			line_string = re.sub(r'^[\s]+<g', r'<g', line_string)
			while(re.search(r'>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1
		elif(re.search(r'<text', line_string) != None) :
			line_string = re.sub(r'^[\s]+<text', r'<text', line_string)
			while(re.search(r'</text>', line_string) == None and indLine < len(input_lines)) :
				line_string = line_string + input_lines[indLine]
				indLine += 1	
		output_lines.append( line_string )
	return output_lines

##################################################################
# MAKES ABSOLUTE COORDINATES USED BY EXTERNAL PROGRAMS
# NOT USED INSIDE THIS PROGRAM BECAUSE OTHER DATA ARE STORED WITH THE LINE
##################################################################
def absolute_coordinates_file_paths(input_lines) :
	output_lines = []
	for line_string in input_lines :
		##################################################################
		# reads the path and transform it into absolute coordinates
		##################################################################
		# print("Initial line_string [", line_string, "]")
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
			    print('Expected style not found')
			if result_ID and result_path :
				##################################################################
				# transforms relative coordinates of base video into absolute ones
				##################################################################
				path_data_string = result_path.group(1)
				# print("Path line_string [", path_data_string, "]")
				# adds space after move or curve and before z
				path_data_string = re.sub(r'([MmCcLlHhVv])', r'\1 ', path_data_string)
				path_data_string = re.sub(r'([Zz])', r' \1 ', path_data_string)
				# print("Relative coordinates [", path_data_string, "]")

				# transformation into absolute coordinates
				# print(path_data_string[0:100])
				path_data_string = vv_transform.relative_to_absolute_coordinates(path_data_string, False)

				#storage
				line_string = '<path id="{0!s}" style="{1!s}" d="{2!s}"/>'.format(result_ID.group(1), result_style.group(1), path_data_string)
		output_lines.append( line_string )
		# print("Absolute coordinates [", line_string, "]")
	return output_lines

##################################################################
# ONLY KEEP DRAWING PATHS
##################################################################
def keep_only_drawing_lines(input_lines, list_of_drawing_paths_IDs) :
	output_lines = []
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
			    print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) in list_of_drawing_paths_IDs :
					output_lines.append( line_string )
		elif(re.search(r'<text', line_string) == None) :
			output_lines.append( line_string )
	return output_lines

def keep_only_drawing_lines_and_camera_path(input_lines, list_of_drawing_paths_IDs, camera_path_ID) :
	output_lines = []
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
			    print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) in list_of_drawing_paths_IDs or result_ID.group(1) == camera_path_ID :
					output_lines.append( line_string )
		elif(re.search(r'<text', line_string) == None) :
			output_lines.append( line_string )
	return output_lines

##################################################################
# RETURNS SELECTED PATH
##################################################################
def load_camera_path_from_lines(input_lines, camera_path_ID) :
	for line_string in input_lines :
		if( re.search(r'^<path', line_string) != None):
			# only keeps the ID and the path data
			result_path = re.search(r'\sd="([^\"]*)\"', line_string)
			if not result_path:
			    print('Expected path not found')
			result_ID = re.search(r'\sid="([^\"]*)\"', line_string)
			if not result_ID:
			    print('Expected ID not found')
			result_style = re.search(r'\sstyle="([^\"]*)\"', line_string)
			if not result_style:
			    print('Expected style not found')
			if result_ID and result_path :
				if result_ID.group(1) == camera_path_ID :
					return line_string
	return ''

##################################################################
# RETURNS view box
##################################################################
#  viewBox="0 0 1016 571.50002"
def load_view_box_from_lines(input_lines) :
	for line_string in input_lines :
		result_viewbox = re.search(r'\sviewBox="([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\s+([0-9\.]+)\"', line_string)
		if(result_viewbox != None):
			return [force_num(result_viewbox.group(1)), force_num(result_viewbox.group(2)), force_num(result_viewbox.group(3)), force_num(result_viewbox.group(4))]
	return [0, 0, 0, 0]

##################################################################
# piecewise segments length calculation of a path
##################################################################
def path_curves_and_lengths(a_path_made_of_curves) :
	##################################################################
	# reads the path and transform it into absolute coordinates
	##################################################################
	print("Initial path [", a_path_made_of_curves[0:100], "]")
	if( re.search(r'^<path', a_path_made_of_curves) != None):
		# only keeps the ID and the path data
		result_path = re.search(r'\sd="([^\"]*)\"', a_path_made_of_curves)
		if not result_path:
		    print('Expected path not found')
		else :
			path_data_string = result_path.group(1)

			# print("relative_to_absolute_coordinates [", local_path_data_string, "]")
			local_path_data = re.split(r' +' , path_data_string.rstrip())
			local_path_data_size = len(local_path_data)

			index = 0
			total_length = 0
			curve_lengths = []
			curve_nodes = []
			while( index < local_path_data_size) :
				if(local_path_data[index] == "M") :
					index += 1
					# processes the next moveto and possible following lineto
					while(True) :
						absolute_current_point_X = force_num(local_path_data[index])
						absolute_current_point_Y = force_num(local_path_data[index + 1])
						subpath_initial_point_X = absolute_current_point_X
						subpath_initial_point_Y = absolute_current_point_Y
						index += 2

						if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
							break
									
				elif(local_path_data[index] == "C") :
					index += 1
					# processes the next curve segments
					while(True) :
						# calculate length
						one_curve_length = 0
						one_curve_length += length(absolute_current_point_X, absolute_current_point_Y, force_num(local_path_data[index]), force_num(local_path_data[index + 1]))
						one_curve_length += length(force_num(local_path_data[index]), force_num(local_path_data[index + 1]), force_num(local_path_data[index + 2]), force_num(local_path_data[index + 3]))
						one_curve_length += length(force_num(local_path_data[index + 2]), force_num(local_path_data[index + 3]), force_num(local_path_data[index + 4]), force_num(local_path_data[index + 5]))
						# and stores it into a list
						curve_lengths.append(one_curve_length)
						# stores the list of the nodes as arrays for the Bezier lib
						curve_nodes.append(np.asfortranarray([[absolute_current_point_X, force_num(local_path_data[index]), force_num(local_path_data[index + 2]), force_num(local_path_data[index + 4])], [absolute_current_point_Y, force_num(local_path_data[index + 1]), force_num(local_path_data[index + 3]), force_num(local_path_data[index + 5])]]))
						# cumulates the length
						total_length += one_curve_length
						# updates the current point for the next curve
						absolute_current_point_X = force_num(local_path_data[index + 4])
						absolute_current_point_Y = force_num(local_path_data[index + 5])
						index += 6
					
						if(index >= local_path_data_size) or (re.search(r'^[MmCcZzLlHhVv]',local_path_data[index]) != None):
							break
				
				elif(local_path_data[index] == "Z" or local_path_data[index] == "z") :
					absolute_current_point_X = subpath_initial_point_X
					absolute_current_point_Y = subpath_initial_point_Y
					new_path = 1
					index += 1
				
				else :
					print( "Line: ", local_path_data_string)
					print( "relative_to_absolute_coordinates Unexpected sign at index "+str(index)+": ("+str(local_path_data[index])+")")
					sys.exit(2)

			return [total_length, curve_lengths, curve_nodes]

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	##################################################################
	# ARGUMENTS
	##################################################################
	drawing_file_name = ''
	image_file_name = ''
	tmp_dir = ''
	drawing_paths_IDs = []
	camera_path_ID = ''
	number_frames_input = 0
	scaling = 1
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["drawing-file-name=", "svg-output-file-name=", "drawing-paths-IDs=", "camera-path-ID=", "number-frames-input=", "ind-frame-output-init=", "ind-frame-output-fin=", "scaling=", "tmp-dir="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--drawing-file-name"):
			drawing_file_name = arg
		elif opt in ("-o", "--svg-output-file-name"):
			svg_output_file_name = arg
		elif opt in ("--drawing-paths-IDs"):
			drawing_paths_IDs = arg
		elif opt in ("--camera-path-ID"):
			camera_path_ID = arg
		elif opt in ("--number-frames-input"):
			number_frames_input = arg
		elif opt in ("--ind-frame-output-init"):
			ind_frame_output_init = arg
		elif opt in ("--ind-frame-output-fin"):
			ind_frame_output_fin = arg
		elif opt in ("--scaling"):
			scaling = arg
		elif opt in ("--tmp-dir"):
			tmp_dir = arg
		else:
			assert False, "unhandled option " + opt
	# print( 'Input scenario file is ', configuration_file_name)

	##################################################################
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)

	##################################################################
	# VECTOR IMAGE FILE OPENING
	try:
		FILEin = open(drawing_file_name+".svg", "rt")
	except IOError:
		print("File not found :", drawing_file_name, " or path is incorrect")

	# STORES THE LINES IN AN ARRAY
	contents = FILEin.read()
	Unormalized_SVG_file_lines = contents.splitlines()
	# VECTOR IMAGE FILE CLOSING
	FILEin.close()

	# NORMALIZES THE SVG FILE BY HAVING TAGS ON A SINGLE LINE
	SVG_file_lines = normalize_file_paths(Unormalized_SVG_file_lines)

	# MAKES ABSOLUTE COORDINATES
	absoluteCoords_SVG_curves = absolute_coordinates_file_paths(SVG_file_lines)

	# ONLY KEEPS DRAWING
	print("Only keep paths", drawing_paths_IDs)
	# absoluteCoords_SVG_curves_drawing = keep_only_drawing_lines_and_camera_path(absoluteCoords_SVG_curves, drawing_paths_IDs, camera_path_ID)
	absoluteCoords_SVG_curves_drawing = keep_only_drawing_lines(absoluteCoords_SVG_curves, drawing_paths_IDs)

	# loads a camera path
	print('camera_path_ID', camera_path_ID)
	camera_path = load_camera_path_from_lines(absoluteCoords_SVG_curves, camera_path_ID)

	# loads drawing dimensions
	(view_box_x, view_box_y, view_box_w, view_box_h) = load_view_box_from_lines(absoluteCoords_SVG_curves)

	# walks along the camera path
	if camera_path != '' :

		# calculates the piecewise length of the path, the length of each curve and the control points
		(camera_path_length, camera_curve_lengths, camera_curve_nodes) = path_curves_and_lengths(camera_path)
		# distributes the frames on the curves according to their lenghts
		camera_curve_frames = list(map(lambda x: int(round((x/camera_path_length) * number_frames_input)), camera_curve_lengths))
		# corrects the distribution on first and last curve so that the summ of the frames correspond to the required frame number
		delta_frames = number_frames_input - sum(camera_curve_frames)
		half_delta_frames = int(delta_frames/2)
		camera_curve_frames[0] += half_delta_frames
		camera_curve_frames[-1] += delta_frames - half_delta_frames
		# prints the results
		print("path length", camera_path_length)
		print("path curve lengths", ["%.2f" % e for e in camera_curve_lengths])
		print("nb tot frame required, provided", number_frames_input, sum(camera_curve_frames))
		print("path curve nb frames", camera_curve_frames)
		# print("path curves\n", "\n".join(map(str,camera_curve_nodes)))
		# print("camera path", camera_path)

		# camera trajectory along a path with centering and scaling around the current point
		# and orientation around the tangent
		ind_frames_cumul = 0
		# walks all the paths of the current drawing
		# for each curve, has a number of frames and the nodes (control points) of the Bezier curve
		for nodes, frames in zip(camera_curve_nodes, camera_curve_frames) :
			# list of curve point coordinates (as many as frames)
			curve_points = []
			# list of curve trangent coordinates (as many as frames)
			curve_tangents = []
			# stores starting frame no
			ind_frames_init = ind_frames_cumul
			# builds the curve that will be used by the bezier lib
			curve = bezier.Curve(nodes, degree=3)
			# calculates points and tangents
			for frame in range(frames) :
				t = float(frame) / frames
				point_array = curve.evaluate(t)
				tangent_array = curve.evaluate_hodograph(t)
				curve_points.append(point_array)
				curve_tangents.append(tangent_array)
				ind_frames_cumul += 1
			# if ind_frames_cumul < 100 :
			# 	print("PLOT frames", ind_frames_init, "to", ind_frames_cumul - 1, ":", curve_points)

			# stores starting frame no
			ind_frames_cumul = ind_frames_init
			# uses points and tangents to change translation and orientation so that
			# the current curve point is in the center of the screen and the drawing is oriented rightwards towards the tangent
			for point, tangent in zip( curve_points, curve_tangents) :
				# rotation/scale around the current point (preceded and followed by translation to this pont)
				# backward translation should be scaled accordingly

				# print("point {} {} tangent {} {} ".format(point[0,0], point[1,0], tangent[0,0], tangent[1,0]))

				# 1: tangent normalization 
				tangent = tangent / np.sqrt(np.sum(tangent**2))
				angle = np.arccos(tangent[0,0])
				if(tangent[1,0] < 0) :
					angle = -angle
				angle = -np.degrees(angle)
				angle -= 90

				group_line = '<g transform="translate({0:.7f} {1:.7f}) scale({2:.7f} {2:.7f}) rotate({3:.7f}) translate({4:.7f} {5:.7f})" style="display:inline" id="layer1">'.format((-view_box_x + view_box_w/2.), (-view_box_y + view_box_h/2.), scaling, angle, -point[0,0], -point[1,0])

				##################################################################
				# GENERATES ALL THE FRAMES IN THE MOVIE
				countOutput = "%05d" % (ind_frames_cumul + ind_frame_output_init)
				full_svg_output_file_name = svg_output_file_name+"_"+countOutput+".svg"

				print("path {} frame {}".format(camera_path_ID, full_svg_output_file_name))

				##################################################################
				# NORMALIZED VECTOR IMAGE FILE OPENING
				try:
					FILEout = open(full_svg_output_file_name, "wt")
				except IOError:
					print("File not created :", os.path.join(tmp_dir, "drawing-tmp.svg"), " or path is incorrect")

				for line_string in absoluteCoords_SVG_curves_drawing :
					if(re.search(r'<g', line_string) != None) :
						# writes the transformation
						FILEout.write(group_line+"\n")
					elif(re.search(r'</svg', line_string) != None) :
						# writes the transformation
						FILEout.write('</g>'+"\n")
						FILEout.write(str(line_string)+"\n")
					else :
						# write the other lines
						FILEout.write(str(line_string)+"\n")
				FILEout.close()

				ind_frames_cumul += 1

				# stops at the end of the required frames
				if ind_frames_cumul > (ind_frame_output_fin - ind_frame_output_init) :
					break
			# # only first bezier curve
			# break

	else :
		print("camera path", camera_path_ID, "not found")

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
