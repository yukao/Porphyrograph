#!/usr/bin/python3 

# vv_replace_in_subdirs.py
# replaces strings in subdirs

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
import shutil
from signal import signal, SIGINT

import re

sys.path.append('/mnt/c/home/LYM-sources/vv/vv_python/')
import csv

from vv_lib import to_num

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_replace_in_subdirs.py -i <file_name> -t <file_name>
ARGUMENTS:
   -t: temp file
   -i: input dir
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	try:
		opts, args = getopt.getopt(main_args,"i:t:",["inputdir=","tempfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputdir"):
			inputdir = arg
		elif opt in ("-t", "--tempfile"):
			tempfile = arg
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)

	for root, dirs, files in os.walk(inputdir):
		for name in files:
			if name.endswith((".csv")):
				if root.endswith(("/")):
					full_name = root+name
				else:
					full_name = root+"/"+name

				# copy file to temp
				if(full_name != tempfile) :
					shutil.copyfile(full_name, tempfile)

				# open file
				try:
					FILEin = open(tempfile, "rt")
				except IOError:
					print("Configuration generator: File not found:", tempfile, " or path is incorrect")
				scenarioCSV = csv.reader(FILEin, delimiter=',')

				# open file
				try:
					FILEout = open(full_name, "wt")
				except IOError:
					print("File not created :", full_name)
				writerCSV = csv.writer(FILEout, delimiter=',')

				# scanning the variable declarations and initial values
				print("scan of ", full_name)
				# RANK
				loc_rank = next(scenarioCSV)
				# VERBATIM
				loc_variable_verbatims = next(scenarioCSV)
				# TYPE
				loc_variable_types = next(scenarioCSV)
				# ID
				loc_variable_IDs = next(scenarioCSV)
				# FUNCTION
				loc_variable_callBacks = next(scenarioCSV)
				# GUI
				loc_variable_GUIs = next(scenarioCSV)
				# SHADER
				loc_variable_shaders = next(scenarioCSV)
				# PULSE
				loc_variable_pulses = next(scenarioCSV)
				# initial_values
				loc_initial_values = next(scenarioCSV)
				# list of initial values
				loc_variable_initial_values = next(scenarioCSV)
				# /initial_values
				loc_end_initial_values = next(scenarioCSV)

				# RANK
				new_rank = []
				# VERBATIM
				new_variable_verbatims = []
				# TYPE
				new_variable_types = []
				# ID
				new_variable_IDs = []
				# FUNCTION
				new_variable_callBacks = []
				# GUI
				new_variable_GUIs = []
				# SHADER
				new_variable_shaders = []
				# PULSE
				new_variable_pulses = []
				# initial_values
				new_initial_values = []
				# list of initial values
				new_variable_initial_values = []
				# /initial_values
				new_end_initial_values = []

				part_path_follow_indices = []
				part_path_repop_indices = []
				part_path_repulse_indices = []
				path_record_indices = []
				path_replay_trackNo_indices = []
				trackMasterWeight_indices = []
				trackMasterWeight_pulse_indices = []
				trackMixingWeight_indices = []
				trackMixingWeight_pulse_indices = []
				CAParams_indices = []
				CAParams_pulse_indices = []
				master_mask_opacity_indices = []
				for index, var_rank, var_verbatim, var_type, var_ID, var_callBack, var_GUI, \
					var_shader, var_pulse, var_initial, var_val, var_end_initial \
					in zip(range(len(loc_variable_IDs)), loc_rank, loc_variable_verbatims, \
						loc_variable_types, loc_variable_IDs, loc_variable_callBacks, loc_variable_GUIs, \
						loc_variable_shaders, loc_variable_pulses, loc_initial_values, loc_variable_initial_values, loc_end_initial_values) :

					merged = False
					pulse_merged = False

					if var_ID.startswith("part_path_follow") and part_path_follow_indices == [] :
						part_path_follow_indices.append(index)
						var_verbatim = "paths"
						var_type = "bool[0..PG_NB_PATHS]"
						var_ID = "part_path_follow"
						var_callBack = "part_path_follow_callBack"
						var_GUI = "part_path_follow"
					elif var_ID.startswith("part_path_follow") :
						part_path_follow_indices.append(index)
						merged = True

					if var_ID.startswith("part_path_repop") and part_path_repop_indices == [] :
						part_path_repop_indices.append(index)
						var_verbatim = "paths"
						var_type = "bool[0..PG_NB_PATHS]"
						var_ID = "part_path_repop"
						var_callBack = "NULL"
						var_GUI = "part_path_repop"
					elif var_ID.startswith("part_path_repop") :
						part_path_repop_indices.append(index)
						merged = True

					if var_ID.startswith("part_path_repulse") and part_path_repulse_indices == [] :
						part_path_repulse_indices.append(index)
						var_verbatim = "paths"
						var_type = "bool[0..PG_NB_PATHS]"
						var_ID = "part_path_repulse"
						var_callBack = "part_path_repulse_callBack"
						var_GUI = "part_path_repulse"
					elif var_ID.startswith("part_path_repulse") :
						part_path_repulse_indices.append(index)
						merged = True

					if var_ID.startswith("path_record") and path_record_indices == [] :
						path_record_indices.append(index)
						var_verbatim = "paths"
						var_type = "bool[1..PG_NB_PATHS]"
						var_ID = "path_record"
						var_callBack = "path_record_callBack"
						var_GUI = "path_record"
					elif var_ID.startswith("path_record") :
						path_record_indices.append(index)
						merged = True

					if var_ID.startswith("path_replay_trackNo") and path_replay_trackNo_indices == [] :
						path_replay_trackNo_indices.append(index)
						var_verbatim = "paths"
						var_type = "int[1..PG_NB_PATHS]"
						var_ID = "path_replay_trackNo"
						var_callBack = "path_replay_trackNo_callBack"
						var_GUI = "path_replay_trackNo"
					elif var_ID.startswith("path_replay_trackNo") :
						path_replay_trackNo_indices.append(index)
						merged = True

					z = re.match("^trackMasterWeight_([0-9]+)$",var_ID)
					if z:
						if var_ID and trackMasterWeight_indices == [] :
							trackMasterWeight_indices.append(index)
							var_verbatim = "track_levels"
							var_type = "float[0..PG_NB_TRACKS]"
							var_ID = "trackMasterWeight"
							var_callBack = "NULL"
							var_GUI = "trackMasterWeight"
						elif var_ID :
							trackMasterWeight_indices.append(index)
							pulse_merged = True

					z = re.match("^trackMasterWeight_([0-9]+)_pulse$",var_ID)
					if z:
						if var_ID and trackMasterWeight_pulse_indices == [] :
							trackMasterWeight_pulse_indices.append(index)
							var_verbatim = "track_levels"
							var_type = "float[0..PG_NB_TRACKS]"
							var_ID = "trackMasterWeight_pulse"
							var_callBack = "NULL"
							var_GUI = "trackMasterWeight_pulse"
						elif var_ID :
							trackMasterWeight_pulse_indices.append(index)
							merged = True

					z = re.match("^trackMixingWeight_([0-9]+)$",var_ID)
					if z:
						if var_ID and trackMixingWeight_indices == [] :
							trackMixingWeight_indices.append(index)
							var_verbatim = "track_levels"
							var_type = "float[0..PG_NB_TRACKS]"
							var_ID = "trackMixingWeight"
							var_callBack = "NULL"
							var_GUI = "trackMixingWeight"
						elif var_ID :
							trackMixingWeight_indices.append(index)
							pulse_merged = True

					z = re.match("^trackMixingWeight_([0-9]+)_pulse$",var_ID)
					if z:
						if var_ID and trackMixingWeight_pulse_indices == [] :
							trackMixingWeight_pulse_indices.append(index)
							var_verbatim = "track_levels"
							var_type = "float[0..PG_NB_TRACKS]"
							var_ID = "trackMixingWeight_pulse"
							var_callBack = "NULL"
							var_GUI = "trackMixingWeight_pulse"
						elif var_ID :
							trackMixingWeight_pulse_indices.append(index)
							merged = True

					z = re.match("^CAParams([0-9]+)$",var_ID)
					if z:
						if var_ID and CAParams_indices == [] :
							CAParams_indices.append(index)
							var_verbatim = "CA"
							var_type = "float[1..(PG_NB_CA_PARAMS+1)]"
							var_ID = "CAParams"
							var_callBack = "NULL"
							var_GUI = "CAParams"
						elif var_ID :
							CAParams_indices.append(index)
							pulse_merged = True

					z = re.match("^CAParams([0-9]+)_pulse$",var_ID)
					if z:
						if var_ID and CAParams_pulse_indices == [] :
							CAParams_pulse_indices.append(index)
							var_verbatim = "CA"
							var_type = "float[1..(PG_NB_CA_PARAMS+1)]"
							var_ID = "CAParams_pulse"
							var_callBack = "NULL"
							var_GUI = "CAParams_pulse"
						elif var_ID :
							CAParams_pulse_indices.append(index)
							merged = True

					z = re.match("^master_mask_opacity_([0-9]+)$",var_ID)
					if z:
						if var_ID and master_mask_opacity_indices == [] :
							master_mask_opacity_indices.append(index)
							var_verbatim = "CA"
							var_type = "float[1..(PG_NB_MASTER_MASKS+1)]"
							var_ID = "master_mask_opacity"
							var_callBack = "NULL"
							var_GUI = "master_mask_opacity"
						elif var_ID :
							master_mask_opacity_indices.append(index)
							merged = True

					if(not merged and not pulse_merged) :
						# VERBATIM
						new_variable_verbatims.append(var_verbatim)
						# TYPE
						new_variable_types.append(var_type)
						# ID
						new_variable_IDs.append(var_ID)
						# FUNCTION
						new_variable_callBacks.append(var_callBack)
						# GUI
						new_variable_GUIs.append(var_GUI)
						# SHADER
						new_variable_shaders.append(var_shader)
						# PULSE
						new_variable_pulses.append(var_pulse)
						# list of initial values
						new_variable_initial_values.append(var_val)
					elif(merged) :
						new_variable_initial_values[-1] = new_variable_initial_values[-1]+"/"+var_val
					elif(pulse_merged) :
						new_variable_initial_values[-2] = new_variable_initial_values[-2]+"/"+var_val

				# RANK
				new_rank = ["RANK"] + list(range(1,len(new_variable_IDs)))
				# initial_values
				new_initial_values = loc_initial_values
				# /initial_values
				new_end_initial_values = loc_end_initial_values

				# RANK
				writerCSV.writerow(new_rank)
				# VERBATIM
				writerCSV.writerow(new_variable_verbatims)
				# TYPE
				writerCSV.writerow(new_variable_types)
				# ID
				writerCSV.writerow(new_variable_IDs)
				# FUNCTION
				writerCSV.writerow(new_variable_callBacks)
				# GUI
				writerCSV.writerow(new_variable_GUIs)
				# SHADER
				writerCSV.writerow(new_variable_shaders)
				# PULSE
				writerCSV.writerow(new_variable_pulses)
				# initial_values
				writerCSV.writerow(new_initial_values)
				# list of initial values
				writerCSV.writerow(new_variable_initial_values)
				# /initial_values
				writerCSV.writerow(new_end_initial_values)

				try:
					lineScenario = next(scenarioCSV)
				except StopIteration:
					continue
				writerCSV.writerow(lineScenario)

				while (True) :
					line_scene_header = next(scenarioCSV)
					writerCSV.writerow(line_scene_header)

					if(line_scene_header[0] == "scene") :
						# scene comments
						line_comments1 = next(scenarioCSV)
						# scene comments
						line_comments2 = next(scenarioCSV)
						# initial values
						line_initial_values = next(scenarioCSV)
						# final values
						line_final_values = next(scenarioCSV)
						# interpolation commands
						line_interpolations = next(scenarioCSV)

						# scene comments
						new_line_comments1 = []
						# scene comments
						new_line_comments2 = []
						# initial values
						new_line_initial_values = []
						# final values
						new_line_final_values = []
						# interpolation commands
						new_line_interpolations = []

						for idx, comm1, comm2, val_init, val_fin, interp in zip(range(len(loc_variable_IDs)), line_comments1, line_comments2, line_initial_values, line_final_values, line_interpolations) :
							if(not(idx in part_path_follow_indices[1:] or idx in part_path_repop_indices[1:] or idx in part_path_repulse_indices[1:] or idx in path_record_indices[1:] \
								or idx in path_replay_trackNo_indices[1:] or idx in trackMasterWeight_indices[1:] or idx in trackMasterWeight_pulse_indices[1:] \
								or idx in trackMixingWeight_indices[1:] or idx in trackMixingWeight_pulse_indices[1:] \
								or idx in CAParams_indices[1:] or idx in CAParams_pulse_indices[1:] or idx in master_mask_opacity_indices[1:])) :
								# scene comments
								new_line_comments1.append(comm1)
								# scene comments
								new_line_comments2.append(comm2)
								# initial values
								new_line_initial_values.append(val_init)
								# final values
								new_line_final_values.append(val_fin)
								# interpolation commands
								new_line_interpolations.append(interp)
							elif(not(idx in trackMasterWeight_indices[1:] or idx in trackMixingWeight_indices[1:] or idx in CAParams_indices[1:])): # merge successive values
								new_line_initial_values[-1] = new_line_initial_values[-1]+"/"+val_init
								new_line_final_values[-1] = new_line_final_values[-1]+"/"+val_fin
							else : # merge before last successive values
								new_line_initial_values[-2] = new_line_initial_values[-2]+"/"+val_init
								new_line_final_values[-2] = new_line_final_values[-2]+"/"+val_fin

						# scene comments
						writerCSV.writerow(new_line_comments1)
						# scene comments
						writerCSV.writerow(new_line_comments2)
						# initial values
						writerCSV.writerow(new_line_initial_values)
						# final values
						writerCSV.writerow(new_line_final_values)
						# interpolation commands
						writerCSV.writerow(new_line_interpolations)

						# final tag
						line_scene_footer = next(scenarioCSV)
						writerCSV.writerow(line_scene_footer)

					elif(line_scene_header[0] == "/scenario") :
						break

					else :
						print("Unknown scenario line [",lineScenario,"] !")
						sys.exit(0)

					
				while(True):
					try: 
						lineScenario = next(scenarioCSV)
					except StopIteration:
						break
					writerCSV.writerow(lineScenario)

				FILEin.close()
				FILEout.close()
		break   #prevent descending into subfolders

	return 1


if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])