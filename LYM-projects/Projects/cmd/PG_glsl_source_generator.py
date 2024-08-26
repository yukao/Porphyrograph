#!/usr/bin/python3 

# PG_glsl_source_generator.py
# command to regenerate glsl shader code, and TouchOSC interface and standardized scenarios from from full scenario and local scenarios for Porphyrograph
# full scenario provides variables and optional shader attachment
# local scenarios provide shader file names and scenes

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import csv
import re
import getopt
import sys
import numpy as np
from signal import signal, SIGINT

import AutomaticSceneGeneration_strings

# reading modes
COMMENT = 1
HTML = 2
SPECIFICATION = 3
SCENE = 4
INITIALIZATION = 5
RENDERING_FILES = 6
UDP_LOCAL_SERVER = 7
UDP_REMOTE_CLIENT = 8
SCENARIO = 9

scene_buttons_colors = [[188, 40, 255],[155, 0, 130],[0, 0, 255],[0, 255, 0],[255, 255, 0],[255, 127, 0],[255, 0 , 0]]
	
space_char = ','
# command to regenerate cpp files + shaders: to be called after each change of variables in the scenario

# number of configuration and for each the number of scenes and the names of the scenes
Nb_Scenarios = 0
nb_scenes = []
scene_names = [[]]

Header_Input_name = ""
Scenario_InputCsv_names = []
FullScenario_InputCsv_name = ""
OutputScenario_OutputCsv_name = ""

# shader input file names (as many of each type as pairs of configuration/scenario files)
ShadersDir = []
ParticleAnimation_InputShader_names = []
Update_InputShader_names = []
Mixing_InputShader_names = []
ParticleRendering_InputShader_names = []
Master_InputShader_names = []

#shader output file names with included declarations & codes of scenario variables
ParticleAnimation_OutputShader_names = []
Update_OutputShader_names = []
Mixing_OutputShader_names = []
ParticleRendering_OutputShader_names = []
Master_OutputShader_names = []

TouchOSCScene_name = ""
TouchOSCScene = None

# full scenario: dictionaries of scenario variables + memory of rank inside scenario
full_scenario_vars_specs_dict = {}
full_varID_from_index = {}
full_nb_vars = 0

# header variable lines to access their value
# config_ids = None
# config_types = None
# config_initial_values = None

# ordinary scenario: dictionaries of scenario variables + memory of rank inside scenario
scenario_vars_specs_dict = []
varID_from_index = {}
nb_vars = []

header_const_dict = dict()

scenario_vars_columns_list = []
out_scenario_footer = []

# booleans whether a full scenario variable is in the local scenario
# built empty and filled when loading the scenarios in PG
scenario_active_vars = {}

scenario_var_pulsing_modes_dict = dict()

def handler(signal_received, frame) :
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	print("GLSL code generator: End of configuration generation\n\n")
	sys.exit(0)

USAGE = '''	print("CPP code generation from full scenario: Usage: python PG_glsl_source_generator.py --header <pg-header.h> --nb_scenarios <int> --scenarios <pg_local_scenario1.csv> --full_scenario <pg_full_scenario.csv> --output_scenario <new_scenario.csv> --TouchOSC_interface <scenes.tosc>")
PG_cpp_source_generator.py: A program for generating glsl shader code, and TouchOSC interface and standardized scenarios from from full scenario and local scenarios for Porphyrograph")

'''

##################################################################
# READS THE CONFIGURATION VARIABLES FROM THE CSV CONFIGURATION FILE 
##################################################################
def read_headerConst(header_fileName) :
	global header_const_dict

	try:
		headerIn = open(header_fileName, "rt")
	except IOError:
		print("GLSL code generator: File not found:", header_fileName, " or path is incorrect")
	for line in headerIn:
		fields = re.split(r"[ \t;=]+", line.rstrip('\n'))
		if(len(fields) >= 4) :
			if(fields[0] == "constexpr") :
				header_const_dict[fields[2]] = fields[3]
				# print(fields[2], fields[3])

	# print(header_const_dict)


def header_const_value(var_ID) :
	# global config_ids
	# global config_types
	# global config_initial_values
	global header_const_dict

	for header_var_ID in header_const_dict.keys() :
		if(var_ID == header_var_ID) :
			return int(header_const_dict[header_var_ID])
		elif(var_ID == "("+header_var_ID+"+1)") :
			return( int(header_const_dict[header_var_ID]) + 1)
		elif(re.match(r"[0-9]+", var_ID)) :
			return( int(var_ID) )
			
	# for config_id, config_type, config_init in zip(config_ids, config_types, config_initial_values):
	# 	if(config_type == "const" and config_id == var_ID) :
	# 		return int(config_init)
	return -1

##################################################################
# READS THE SHADER FILE NAMES FROM THE CSV CONFIGURATION FILE 
##################################################################
def read_scenario_shader_names(local_scenario_footer, indScenario) :
	global ParticleAnimation_InputShader_names
	global Update_InputShader_names
	global ParticleRendering_InputShader_names
	global Mixing_InputShader_names
	global Master_InputShader_names

	global ParticleAnimation_OutputShader_names
	global Update_OutputShader_names
	global Mixing_OutputShader_names
	global ParticleRendering_OutputShader_names
	global Master_OutputShader_names

	line_index = 0
	shader_names_line = False
	line = local_scenario_footer[line_index] 
	# print "line1 ", line 

	mode = COMMENT
	value = 0
	while (line_index < len(local_scenario_footer)) :
		if(line[0] == "shader_files") :
			shader_names_line = True
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "ParticleAnimation" and shader_names_line == True) :
			if(line[1] != "NULL") :
				ParticleAnimation_InputShader_names.append(line[1]+".frag")
				ParticleAnimation_OutputShader_names.append(line[1]+"_full.frag")
			else :
				ParticleAnimation_InputShader_names.append("NULL")
				ParticleAnimation_OutputShader_names.append("NULL")
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "Update" and shader_names_line == True) :
			# print("GLSL code generator: Update shader")
			if(line[1] != "NULL") :
				Update_InputShader_names.append(line[1]+".frag")
				Update_OutputShader_names.append(line[1]+"_full.frag")
			else :
				Update_InputShader_names.append("NULL")
				Update_OutputShader_names.append("NULL")
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "ParticleRender" and shader_names_line == True) :
			if(line[1] != "NULL") :
				ParticleRendering_InputShader_names.append(line[1]+".frag")
				ParticleRendering_OutputShader_names.append(line[1]+"_full.frag")
			else :
				ParticleRendering_InputShader_names.append("NULL")
				ParticleRendering_OutputShader_names.append("NULL")
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "Mixing" and shader_names_line == True) :
			if(line[1] != "NULL") :
				Mixing_InputShader_names.append(line[1]+".frag")
				Mixing_OutputShader_names.append(line[1]+"_full.frag")
			else :
				Mixing_InputShader_names.append("NULL")
				Mixing_OutputShader_names.append("NULL")
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "Master" and shader_names_line == True) :
			if(line[1] != "NULL") :
				Master_InputShader_names.append(line[1]+".frag")
				Master_OutputShader_names.append(line[1]+"_full.frag")
			else :
				Master_InputShader_names.append("NULL")
				Master_OutputShader_names.append("NULL")
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "/shader_files") :
			return()

		else :
			#next line
			line_index += 1
			line = local_scenario_footer[line_index] 
	return()

##################################################################
# READS THE SHADER DIR NAMES FROM THE CSV CONFIGURATION FILE 
##################################################################
def read_scenario_shader_dir(local_scenario_footer, indScenario) :
	global ShadersDir

	line_index = 0
	dir_names_line = False
	line = local_scenario_footer[line_index] 
	# print "line1 ", line 

	mode = COMMENT
	value = 0
	while (line_index < len(local_scenario_footer)) :
		if(line[0] == "directories") :
			dir_names_line = True
			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "shaders" and dir_names_line == True) :
			if(line[1] != "NULL") :
				ShadersDir.append(line[1])
			else :
				print("GLSL code generator: empty shader string [",lineScenario,"] !")
				print("GLSL code generator: End of configuration generation\n\n")
				sys.exit(0)

			line_index += 1
			line = local_scenario_footer[line_index] 

		elif(line[0] == "/directories") :
			return()

		else :
			#next line
			line_index += 1
			line = local_scenario_footer[line_index] 
	return()

##################################################################
# READS THE FULL AND LOCAL SCENARIO FILE TOP: VARIABLES 
##################################################################
def expect_tag(line_scenario, tag_string) :
	if(line_scenario[0] != tag_string) :
		print(tag_string, "first column string expected not [",line_scenario[0],"]!")
		print(line_scenario)
		print("GLSL code generator: End of configuration generation\n\n")
		sys.exit(0)
def check_tag(line_scenario, tag_string) :
	return (line_scenario[0] == tag_string)

def read_scenario_variables(scenarioCSV, indScenario) :
	global full_scenario_vars_specs_dict
	global full_varID_from_index
	global scenario_vars_specs_dict
	global varID_from_index
	global Nb_Scenarios
	global nb_vars
	global full_nb_vars

	################ scenario header
	# RANK
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "RANK")

	# comment used to store variable GROUP
	# VERBATIM
	loc_variable_verbatims = next(scenarioCSV)
	expect_tag(loc_variable_verbatims, "VERBATIM")
	loc_variable_verbatims = loc_variable_verbatims[1:]

	# variable types (used for declarations)
	# TYPE
	loc_variable_types = next(scenarioCSV)
	expect_tag(loc_variable_types, "TYPE")
	loc_variable_types = loc_variable_types[1:]

	# variable IDs (used for OSC address commands and C++ variable names)
	# ID
	loc_variable_IDs = next(scenarioCSV)
	expect_tag(loc_variable_IDs, "ID")
	loc_variable_IDs = loc_variable_IDs[1:]

	# function callBack pointers for specific additional updates
	# FUNCTION
	loc_variable_callBacks = next(scenarioCSV)
	expect_tag(loc_variable_callBacks, "FUNCTION")
	loc_variable_callBacks = loc_variable_callBacks[1:]

	# string for message display on GUI
	# GUI
	loc_variable_GUIs = next(scenarioCSV)
	expect_tag(loc_variable_GUIs, "GUI")
	loc_variable_GUIs = loc_variable_GUIs[1:]

	# destination shader for variables
	# SHADER
	loc_variable_shaders = next(scenarioCSV)
	expect_tag(loc_variable_shaders, "SHADER")
	loc_variable_shaders = loc_variable_shaders[1:]

	# variable pulsing type
	# PULSE
	loc_variable_pulses = next(scenarioCSV)
	expect_tag(loc_variable_pulses, "PULSE")
	loc_variable_pulses = loc_variable_pulses[1:]

	# initial_values
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "initial_values")
	# list of initial values
	loc_variable_initial_values = next(scenarioCSV)
	loc_variable_initial_values = loc_variable_initial_values[1:]

	# /initial_values
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "/initial_values")

	# dictionaries of scenario variable definition
	# build dictionary for the variable of the full scenario
	if(indScenario == -1) :
		full_scenario_vars_specs_dict = dict()
		full_varID_from_index = dict()
		ind_var = 0
		for var_ID, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial in zip(loc_variable_IDs, loc_variable_verbatims, loc_variable_types, loc_variable_callBacks, loc_variable_GUIs, loc_variable_shaders, loc_variable_pulses, loc_variable_initial_values) :
			full_scenario_vars_specs_dict[var_ID] = [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
			full_varID_from_index[ind_var] = var_ID
			ind_var = ind_var + 1

		# -> nb of loc_variables 
		nb_loc_variables = len(loc_variable_IDs)
		print("full scenario, nb vars:", nb_loc_variables)
		full_nb_vars = nb_loc_variables

	elif(indScenario < Nb_Scenarios) :
		local_scenario_vars_specs_dict = {}
		varID_from_index = dict()
		ind_var = 0
		for var_ID, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial in zip(loc_variable_IDs, loc_variable_verbatims, loc_variable_types, loc_variable_callBacks, loc_variable_GUIs, loc_variable_shaders, loc_variable_pulses, loc_variable_initial_values) :
			local_scenario_vars_specs_dict[var_ID] = [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
			varID_from_index[ind_var] = var_ID
			ind_var = ind_var + 1

		scenario_vars_specs_dict.append(local_scenario_vars_specs_dict)
		
		# -> nb of loc_variables 
		nb_loc_variables = len(loc_variable_IDs)
		print("scenario #"+str(indScenario)+", nb vars:", nb_loc_variables)
		nb_vars.append(nb_loc_variables)

	return

def var_index_in_scenario(var_ID, indScenario) :
	global scenario_vars_specs_dict
	if var_ID in scenario_vars_specs_dict[indScenario] :
		return scenario_vars_specs_dict[indScenario][var_ID][0]
	else :
		return -1

def var_index_in_full_scenario(var_ID) :
	global full_scenario_vars_specs_dict
	if var_ID in full_scenario_vars_specs_dict :
		return full_scenario_vars_specs_dict[var_ID][0]
	else :
		return -1

##################################################################
# READS THE SCENARIO SCENES FROM THE CSV SCENARIO FILE 
##################################################################
def read_scenario_scenes(readCSV, indScenario) :
	global nb_scenes
	global scene_names

	global scenario_vars_columns_list
	global out_scenario_footer

	global nb_vars

	################################################################
	# SCENES IN SCENARIO
	# jumps to next lineScenario
	lineScenario = next(readCSV)
	expect_tag(lineScenario, "scenario")

	loc_nb_scenes = 0
	# store the column and will write it according to full scenario order
	local_scenario_vars_columns_list = []
	while (True) :
		line_scene_header = next(readCSV)
		if(line_scene_header[0] == "scene") :
			# scene name and duration
			scene_names[indScenario].append(line_scene_header[1])
			print("GLSL code generator: scene", loc_nb_scenes + 1, ": ", scene_names[indScenario][loc_nb_scenes])

			# scene comments
			line_comments1 = next(readCSV)

			# scene comments
			line_comments2 = next(readCSV)

			# initial values
			line_initial_values = next(readCSV)

			# final values
			line_final_values = next(readCSV)

			# interpolation commands
			line_interpolations = next(readCSV)

			# final tag
			line_scene_footer = next(readCSV)

			# stores the columns of the vars for this scene (only starts from the second column)
			one_scene_var_value_column_list =[]
			for idx, head, comm1, comm2, val_init, val_fin, interp in zip(range(1, nb_vars[indScenario] + 1), line_scene_header[1:], line_comments1[1:], line_comments2[1:], line_initial_values[1:], line_final_values[1:], line_interpolations[1:]) :
					one_scene_var_value_column_list.append([idx, head, comm1, comm2, val_init, val_fin, interp])

			# append the values of this scene to the preceding scenes
			local_scenario_vars_columns_list.append(one_scene_var_value_column_list)

		elif(line_scene_header[0] == "/scenario") :
			break

		else :
			print("GLSL code generator: Unknown scenario line [",lineScenario,"] !")
			print("GLSL code generator: End of configuration generation\n\n")
			sys.exit(0)

		loc_nb_scenes = loc_nb_scenes + 1
		
	local_out_scenario_footer = []
	while(True):
		try: 
			lineScenario = next(readCSV)
			local_out_scenario_footer.append(lineScenario)
		except StopIteration:
			break

	read_scenario_shader_names(local_out_scenario_footer, indScenario)
	read_scenario_shader_dir(local_out_scenario_footer, indScenario)

	out_scenario_footer.append(local_out_scenario_footer)
	scenario_vars_columns_list.append(local_scenario_vars_columns_list)
	nb_scenes.append(loc_nb_scenes)
	print("GLSL code generator: scenes:", nb_scenes[indScenario])

##################################################################
# GENERATES THE NEW SCENARIO ACCORDING TO THE FULL SCENARIO TEMPLATE
# SEPARATE FULL SCENARIO VARIABLES INTO ACTIVE AND PASSIVE ONES
##################################################################
def post_reading_scenarios() :
	global full_scenario_vars_specs_dict
	global scenario_vars_specs_dict

	global Nb_Scenarios

	global scenario_active_vars

	global Scenario_InputCsv_names
	global FullScenario_InputCsv_name
	global OutputScenario_OutputCsv_name

	global scenario_vars_columns_list
	global out_scenario_footer

	scenario_unknown_vars_specs_dict = {}

	# CHECKS THE COMPLETENESS OF THE FULL SCENARIO
	# creates a dict with all the variables in the local scenarios which are
	# not inside the full scenario
	for indScenario in range(Nb_Scenarios) :
		for var_ID in scenario_vars_specs_dict[indScenario] :
			if(var_ID != "" and var_index_in_full_scenario(var_ID) < 0) :
				print("Unknown variable:", var_ID, ", specs:", scenario_vars_specs_dict[indScenario][var_ID])
				scenario_unknown_vars_specs_dict[var_ID] = [indScenario] + scenario_vars_specs_dict[indScenario][var_ID]

	# and exists if all the variables are not inside the full scenario, 
	# so that they can be added there
	if(len(scenario_unknown_vars_specs_dict) != 0) :
		# print(scenario_unknown_vars_specs_dict)
		unknown_vars_scenario_name = Scenario_InputCsv_names[indScenario]
		path = re.findall(r'^(.*[/\\])[^/\\\.]+\.csv$', unknown_vars_scenario_name)
		if(path != []) :
			unknown_vars_scenario_name = path[0] + f'''scenario_{indScenario}.csv'''
		else :
			unknown_vars_scenario_name = f'''scenario_{indScenario}.csv'''

		try:
			unknown_vars_scenario_fileCsv = open(unknown_vars_scenario_name, "wt", newline='')
		except IOError:
			print("GLSL code generator: New Scenario File not opened:", unknown_vars_scenario_name, " or path is incorrect")
		print("GLSL code generator: Some variables in scenarios should be declared inside the full scenario")
		print("GLSL code generator: See file:", unknown_vars_scenario_name)

		varVerbatim = []
		varType = []
		varID = []
		varCallBack = []
		varGUI = []
		varShader = []
		varPulse = []
		# [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
		for var_ID, full_specs in scenario_unknown_vars_specs_dict.items() :
			varVerbatim.append(full_specs[2])
			varType.append(full_specs[3])
			varID.append(var_ID)
			varCallBack.append(full_specs[4])
			varGUI.append(full_specs[5])
			varShader.append(full_specs[6])
			varPulse.append(full_specs[7])

		CSVwriter = csv.writer(unknown_vars_scenario_fileCsv, delimiter=',')
		CSVwriter.writerow(["RANK"] + list(range(len(scenario_unknown_vars_specs_dict.keys()))))
		# variable_full_scenario_header_dictionary[current_scenario_var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
		CSVwriter.writerow(["VERBATIM"] + varVerbatim)
		CSVwriter.writerow(["TYPE"] +  varType)
		CSVwriter.writerow(["ID"] + varID)
		CSVwriter.writerow(["FUNCTION"] +  varCallBack)
		CSVwriter.writerow(["GUI"] + varGUI)
		CSVwriter.writerow(["SHADER"] +  varShader)
		CSVwriter.writerow(["PULSE"] +  varPulse)
		unknown_vars_scenario_fileCsv.close()
		print("GLSL code generator: End of configuration generation\n\n")
		sys.exit(0)

	# TAKES NOTE OF THE CONFIGURATIONS IN WHICH THE VARIABLES ARE KNOWN
	# creates a vector of Nb_Scenarios boobleans indicating whether or not 
	# a variable of the full scenario is in one of the configurations
	for var_ID in full_scenario_vars_specs_dict:
		# initializes with Nb_Scenarios False (variable in no scenario)
		scenario_active_vars[var_ID] = [False] * Nb_Scenarios
		# print("initializes False (variable in no scenario) of size ", Nb_Scenarios, ".")

	# checks the presence of the vars in one of the scenarios
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		for indScenario in range(Nb_Scenarios):
			if(var_index_in_scenario(var_ID, indScenario) >= 0) :
				scenario_active_vars[var_ID][indScenario] = True

				# checks that the parameters in the local scenario are the same as in the full scenario
				# variable is already known, checks that it is coherent with what is already stored
				# [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
				local_specs = scenario_vars_specs_dict[indScenario][var_ID]

				if(local_specs[2] != full_specs[2]) :
					print("GLSL code generator: var",var_ID,"has inconsistent types in full scenario",	FullScenario_InputCsv_name,\
						"(",full_specs[2],") and in scenario",Scenario_InputCsv_names[indScenario],"(",local_specs[2],")!")
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
				if(local_specs[3] != full_specs[3]) :
					print("GLSL code generator: var",var_ID,"has inconsistent callBacks in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[3],") and in scenario",Scenario_InputCsv_names[indScenario],"(",local_specs[3],")!")
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
				if(local_specs[5] != full_specs[5]) :
					print("GLSL code generator: var",var_ID,"has inconsistent target shaders in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[5],") and in scenario",Scenario_InputCsv_names[indScenario],"(",local_specs[5],")!")
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
				if(local_specs[6] != full_specs[6]) :
					print("GLSL code generator: var",var_ID,"has inconsistent pulsing modes in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[6],") and in scenario",Scenario_InputCsv_names[indScenario],"(",local_specs[6],")!")
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

	# generates new scenarios in which the variables are ranked in the same order as in the full scenario
	# these scenarios are the ones which will be loaded by pg
	for indScenario in range(Nb_Scenarios):
		new_scenario_name = OutputScenario_OutputCsv_name
		path = re.findall(r'^(.*[/\\])([^/\\\.]+)\.csv$', new_scenario_name)
		file_rank = indScenario + 1
		if(path != []) :
			new_scenario_name = path[0][0] + f'''{path[0][1]}_{file_rank}.csv'''
		else :
			new_scenario_name = f'''{OutputScenario_OutputCsv_name}_{file_rank}.csv'''

		try:
			output_scenario_fileCsv = open(new_scenario_name, "wt", newline='')
		except IOError:
			print("GLSL code generator: New Scenario File not opened:", new_scenario_name, " or path is incorrect")
		print("GLSL code generator: New Scenario File opened:", new_scenario_name)

		varVerbatim = []
		varType = []
		varID = []
		varCallBack = []
		varGUI = []
		varShader = []
		varPulse = []
		varInitial = []
		# [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
		for var_ID, full_specs in full_scenario_vars_specs_dict.items():
			if(scenario_active_vars[var_ID][indScenario] == True) :
				varVerbatim.append(full_specs[1])
				varType.append(full_specs[2])
				varCallBack.append(full_specs[3])
				varGUI.append(full_specs[4])
				varShader.append(full_specs[5])
				varPulse.append(full_specs[6])
				# initial value taken for scenario
				local_specs = scenario_vars_specs_dict[indScenario][var_ID]
				varInitial.append(local_specs[7])
				varID.append(var_ID)

		CSVwriter = csv.writer(output_scenario_fileCsv, delimiter=',')
		# print("rank",len(full_scenario_vars_specs_dict.keys()),"varID",len(varID))
		CSVwriter.writerow(["RANK"] + list(range(1, len(varID) + 1)))
		# variable_full_scenario_header_dictionary[current_scenario_var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
		CSVwriter.writerow(["VERBATIM"] + varVerbatim)
		CSVwriter.writerow(["TYPE"] +  varType)
		CSVwriter.writerow(["ID"] + varID)
		CSVwriter.writerow(["FUNCTION"] +  varCallBack)
		CSVwriter.writerow(["GUI"] + varGUI)
		CSVwriter.writerow(["SHADER"] +  varShader)
		CSVwriter.writerow(["PULSE"] +  varPulse)

		##################################################################
		# WRITES THE INITIAL VALUES FROM THE SCENARIO
		##################################################################
		CSVwriter.writerow(["initial_values"])
		CSVwriter.writerow([""] + varInitial)
		CSVwriter.writerow(["/initial_values"])

		##################################################################
		# WRITES THE SCENES
		##################################################################
		CSVwriter.writerow(["scenario"])
		# scans all the scenes of the current scenario
		for scene_columns in scenario_vars_columns_list[indScenario] :
			# scans all the columns
			rank_in_new_scenario = 0
			sceneHead = ["scene"]
			sceneComments1 = []
			sceneComments2 = []
			sceneInit = []
			sceneFin = []
			sceneInterp = []
			for var_ID, full_specs in full_scenario_vars_specs_dict.items():
				# the var of the full scenario is inside the current scene
				if(scenario_active_vars[var_ID][indScenario] == True) :
					# index of the variable inside the current scenario to retrieve its scene values 
					# (except the first line which is depending on the rank of the variable in the new scenario)
					var_index = var_index_in_scenario(var_ID, indScenario)
					if(var_index < 0 or var_index >= len(scene_columns)) :
						print("GLSL code generator: var",var_ID,"scenes not found in scenario",Scenario_InputCsv_names[indScenario],"(config: ",indScenario,")!")
						print("GLSL code generator: End of configuration generation\n\n")
						sys.exit(0)

					# retrieves the column of the current variable from the original scenario  file
					# [idx, head, comm1, comm2, val_init, val_fin, interp]
					scene_col = scene_columns[var_index]
					sceneComments1.append(scene_col[2])
					sceneComments2.append(scene_col[3])
					sceneInit.append(scene_col[4])
					sceneFin.append(scene_col[5])
					sceneInterp.append(scene_col[6])

					# the head of the scene has the same elements as in the original scenario (the reordering does not modify anything)
					if(rank_in_new_scenario < len(scene_columns)) :
						scene_col_absolute = scene_columns[rank_in_new_scenario]
						sceneHead.append(scene_col_absolute[1])

					rank_in_new_scenario += 1
			CSVwriter.writerow(sceneHead)
			CSVwriter.writerow([sceneComments1[0]] + sceneComments1)
			CSVwriter.writerow(["ID"] + sceneComments2)
			# for each variable sets the initial value
			CSVwriter.writerow([""] + sceneInit)
			# for each variable sets the final value
			CSVwriter.writerow([""] + sceneFin)
			# for each variable sets the interpolation value
			CSVwriter.writerow([""] + sceneInterp)
			CSVwriter.writerow(["/scene"])
		CSVwriter.writerow(["/scenario"])

		##################################################################
		# WRITES THE FOOTER
		##################################################################
		for out_scenario_footer_line in out_scenario_footer[indScenario]:
			CSVwriter.writerow(out_scenario_footer_line)

		output_scenario_fileCsv.close()

##################################################################
# WRITES A TOUCHOSC FILE WITH ALL THE SCENES OF ALL THE LOCAL SCENARIOS 
##################################################################
def WriteTouchOSCScene() :
	global Nb_Scenarios
	global nb_scenes
	global scene_names

	global TouchOSCScene


	button_height = 70
	TouchOSCScene.write(AutomaticSceneGeneration_strings.scene_head_string.format('group_ID_0'))
	TouchOSCScene.write(AutomaticSceneGeneration_strings.configuration_radio_label_string.format('configuration_radio_ID_0', (button_height + 20) * Nb_Scenarios, Nb_Scenarios, 'configuration_label_ID_0'))
	current_height = 10
	for indScenario in range(Nb_Scenarios) :
		if(nb_scenes[indScenario] <= 8) :
			button_width = (1800 / nb_scenes[indScenario]) - 30
		else :
			button_width = (1800 / 8) - 30
		current_width = 120
		for indScene in range(nb_scenes[indScenario]) :
			TouchOSCScene.write(AutomaticSceneGeneration_strings.button_string.format('button_ID' + '_' + str(indScenario) + '_' + str(indScene), current_width, current_height, button_width, button_height, scene_names[indScenario][indScene],scene_buttons_colors[indScenario % 7][0]/255.0,scene_buttons_colors[indScenario % 7][1]/255.0,scene_buttons_colors[indScenario % 7][2]/255.0))
			TouchOSCScene.write(AutomaticSceneGeneration_strings.label_string.format('label_ID' + '_' + str(indScenario) + '_' + str(indScene), current_width, current_height, button_width, button_height, scene_names[indScenario][indScene]))
			current_width += button_width + 30
			if(indScene > 0 and indScene < nb_scenes[indScenario] - 1 and (indScene + 1) % 8 == 0) :
				current_height += button_height + 20
				current_width = 120
		current_height += button_height + 20
	TouchOSCScene.write(AutomaticSceneGeneration_strings.scene_tail_string)
	TouchOSCScene.close()

#####################################################
##### WRITES THE SCENARIO VARIABLES % CALLBACKS IN THE SCRIPT HEADER AND BODY FILES
##### AND IN THE SHADER BODY DECLARATIONS
#####################################################
def scenario_to_cpp_type(scenario_type) :
	if(scenario_type == "sign") :
		return "float"			
	elif(scenario_type == "path") :
		return "bool"
	elif(scenario_type == "string") :
		return "string"
	elif(scenario_type.startswith("bool")) :
		return "bool"
	elif(scenario_type.startswith("int")) :
		return "int"
	elif(scenario_type.startswith("float")) :
		return "float"
	print("GLSL code generator: Unknown scenario variable type ", scenario_type)
	print("GLSL code generator: End of configuration generation\n\n")
	sys.exit(0)

def scenario_to_cpp_range(scenario_type) :
	if(scenario_type == "sign" or scenario_type == "path" or scenario_type == "bool" or scenario_type == "int" or scenario_type == "float" or scenario_type == "string") :
		return []
	elif(scenario_type.startswith("bool") or scenario_type.startswith("int") or scenario_type.startswith("float")) :
		indices = re.findall(r'.*\[([0-9]+)\.\.([^/[]+)\]$', scenario_type)
		# print(indices)
		if(len(indices) == 1 and len(indices[0]) == 2) :
			# print(indices[0][0], indices[0][1])
			return [indices[0][0], indices[0][1]]
	print("GLSL code generator: Unknown scenario variable type ", scenario_type)
	print("GLSL code generator: End of configuration generation\n\n")
	sys.exit(0)

###############################################################################
# WRITE SHADER HEADER BODY AND BINDING (IN PARALLEL WITH C++ DATA GENERATED BY CPP GENERATOR)
###############################################################################
def write_binding_vars_header_and_body(indScenario) :
	# shader input file names (as many of each type as pairs of configuration/scenario files)
	global ParticleAnimation_InputShader_names
	global Update_InputShader_names
	global Mixing_InputShader_names
	global ParticleRendering_InputShader_names
	global Master_InputShader_names

	#shader output file names with included declarations & codes of scenario variables
	global ParticleAnimation_OutputShader_names
	global Update_OutputShader_names
	global Mixing_OutputShader_names
	global ParticleRendering_OutputShader_names
	global Master_OutputShader_names


	global Nb_Scenarios
	global Scenario_InputCsv_names

	global scenario_var_target_shaders

	global ShadersDir

	global scenario_active_vars

	try:
		Update_InputShader = open(ShadersDir[indScenario]+Update_InputShader_names[indScenario], "rt")
	except IOError:
		print("GLSL code generator: Update_InputShader File not found:", ShadersDir[indScenario]+Update_InputShader_names[indScenario], " or path is incorrect")
	try:
		Update_OutputShader = open(ShadersDir[indScenario]+Update_OutputShader_names[indScenario], "wt")
	except IOError:
		print("GLSL code generator: Update_OutputShader File not opened:", ShadersDir[indScenario]+Update_OutputShader_names[indScenario], " or path is incorrect")
	try:
		Mixing_InputShader = open(ShadersDir[indScenario]+Mixing_InputShader_names[indScenario], "rt")
	except IOError:
		print("GLSL code generator: Mixing_InputShader File not found:", ShadersDir[indScenario]+Mixing_InputShader_names[indScenario], " or path is incorrect")
	try:
		Mixing_OutputShader = open(ShadersDir[indScenario]+Mixing_OutputShader_names[indScenario], "wt")
	except IOError:
		print("GLSL code generator: Mixing_OutputShader File not opened:", ShadersDir[indScenario]+Mixing_OutputShader_names[indScenario], " or path is incorrect")
	try:
		Master_InputShader = open(ShadersDir[indScenario]+Master_InputShader_names[indScenario], "rt")
	except IOError:
		print("GLSL code generator: Master_InputShader File not found:", ShadersDir[indScenario]+Master_InputShader_names[indScenario], " or path is incorrect")
	try:
		Master_OutputShader = open(ShadersDir[indScenario]+Master_OutputShader_names[indScenario], "wt")
	except IOError:
		print("GLSL code generator: Master_OutputShader File not opened:", ShadersDir[indScenario]+Master_OutputShader_names[indScenario], " or path is incorrect")

	# GLSL OPTIONAL PARTICLE SHADER INPUT AND OUTPUT FILES
	if(ParticleAnimation_InputShader_names[indScenario] != "NULL") :
		try:
			ParticleAnimation_InputShader = open(ShadersDir[indScenario]+ParticleAnimation_InputShader_names[indScenario], "rt")
		except IOError:
			print("GLSL code generator: ParticleAnimation_InputShader File not found:", ShadersDir[indScenario]+ParticleAnimation_InputShader_names[indScenario], " or path is incorrect")
		try:
			ParticleAnimation_OutputShader = open(ShadersDir[indScenario]+ParticleAnimation_OutputShader_names[indScenario], "wt")
		except IOError:
			print("GLSL code generator: ParticleAnimation_OutputShader File not opened:", ShadersDir[indScenario]+ParticleAnimation_OutputShader_names[indScenario], " or path is incorrect")
		try:
			ParticleRendering_InputShader = open(ShadersDir[indScenario]+ParticleRendering_InputShader_names[indScenario], "rt")
		except IOError:
			print("GLSL code generator: ParticleRendering_InputShader File not found:", ShadersDir[indScenario]+ParticleRendering_InputShader_names[indScenario], " or path is incorrect")
		try:
			ParticleRendering_OutputShader = open(ShadersDir[indScenario]+ParticleRendering_OutputShader_names[indScenario], "wt")
		except IOError:
			print("GLSL code generator: ParticleRendering_OutputShader File not opened:", ShadersDir[indScenario]+ParticleRendering_OutputShader_names[indScenario], " or path is incorrect")

	##################################################################
	# SHADER VARIABLES
	# WRITES SHADERS DECLARATIONS AND VARIABLE VALUES
	##################################################################
	ParticleRender_fs_index = 0
	Master_fs_index = 0
	ParticleAnimation_fs_index = 0
	Update_fs_index = 0
	Mixing_fs_index = 0

	ParticleRender_fs_var_string = ""
	Master_fs_var_string = ""
	ParticleAnimation_fs_var_string = ""
	Mixing_fs_var_string = ""

	ParticleRender_head_glsl = ""
	Master_head_glsl = ""
	ParticleAnimation_head_glsl = ""
	Update_head_glsl = ""
	Mixing_head_glsl = ""

	ParticleRender_body_glsl = ""
	Master_body_glsl = ""
	ParticleAnimation_body_glsl = ""
	Update_body_glsl = ""
	Mixing_body_glsl = ""

	# scans the line of the full scenario file that indicates what is the target shader for each variable
	# variables are grouped by 4 to be declared as vec4
	# target_shader_list is * / ParticleRender_fs / Master_fs / Update_fs / Mixing_fs / ParticleAnimation_fs
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = full_specs[2]
		index_range = scenario_to_cpp_range(type_string)
		target_shader_list = full_specs[5]
		pulsing_mode = full_specs[6]
		# all vars in full scenario are taken not only active ones. 
		# active_var_in_config = scenario_active_vars[var_ID]
		# if(target_shader_list != "*" and active_var_in_config[indScenario] == True) :
		if(target_shader_list != "*") :
			# "*" nothing is done, the variable is not a shader parameter or the variable is not in the scenario of this configuration
			if(re.search(r'ParticleAnimation_fs', target_shader_list) != None) :
				# ParticleAnimation_fs parameter
				ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "#define var_" + var_ID + "\n"
				if(type_string == "bool" or type_string == "path") :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "bool	  " + var_ID + ";\n"
				elif(type_string == "int") :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "int		" + var_ID + ";\n"
				elif(type_string == "float") :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "float	 " + var_ID + ";\n"
				elif(type_string.startswith("bool")) :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "bool	  " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "int		" + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "float	 " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				else :
					print("GLSL code generator: Unknown particle animation shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
								
				if(type_string == "bool" or type_string == "path") :
					ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n"
				elif(type_string == "float") :
					ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
						ParticleAnimation_fs_index  += 1
					ParticleAnimation_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n";
						ParticleAnimation_fs_index  += 1
					ParticleAnimation_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n";
						ParticleAnimation_fs_index  += 1
					ParticleAnimation_fs_index  -= 1
				else :
					print("GLSL code generator: Unknown particle animation shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

				ParticleAnimation_fs_index  += 1
		
			if(re.search(r'Update_fs', target_shader_list) != None) :
				# Update_fs parameter
				Update_head_glsl = Update_head_glsl + "#define var_" + var_ID + "\n"
				if(type_string == "bool" or type_string == "path") :
					Update_head_glsl = Update_head_glsl + "bool	  " + var_ID + ";\n"
				elif(type_string == "int") :
					Update_head_glsl = Update_head_glsl + "int		" + var_ID + ";\n"
				elif(type_string == "float") :
					Update_head_glsl = Update_head_glsl + "float	 " + var_ID + ";\n"
				elif(type_string.startswith("bool")) :
					Update_head_glsl = Update_head_glsl + "bool	  " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Update_head_glsl = Update_head_glsl + "int		" + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Update_head_glsl = Update_head_glsl + "float	 " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				else :
					print("GLSL code generator: Unknown update shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

				if(type_string == "bool" or type_string == "path") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n"
				elif(type_string == "float") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				else :
					print("GLSL code generator: Unknown update shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
				
				Update_fs_index += 1
			
			if(re.search(r'Mixing_fs', target_shader_list) != None) :
				# Mixing_fs parameter
				Mixing_head_glsl = Mixing_head_glsl + "#define var_" + var_ID + "\n"
				if(type_string == "bool" or type_string == "path") :
					Mixing_head_glsl = Mixing_head_glsl + "bool	  " + var_ID + ";\n"
				elif(type_string == "int") :
					Mixing_head_glsl = Mixing_head_glsl + "int		" + var_ID + ";\n"
				elif(type_string == "float") :
					Mixing_head_glsl = Mixing_head_glsl + "float	 " + var_ID + ";\n"
				elif(type_string.startswith("bool")) :
					Mixing_head_glsl = Mixing_head_glsl + "bool	  " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Mixing_head_glsl = Mixing_head_glsl + "int		" + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Mixing_head_glsl = Mixing_head_glsl + "float	 " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				else :
					print("GLSL code generator: Unknown mixing shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

				if(type_string == "bool" or type_string == "path") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n"
				elif(type_string == "float") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				else :
					print("GLSL code generator: Unknown mixing shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
			 	
				Mixing_fs_index += 1
			

			if(re.search(r'ParticleRender_fs', target_shader_list) != None) :
				# ParticleRender_fs parameter
				ParticleRender_head_glsl = ParticleRender_head_glsl + "#define var_" + var_ID + "\n"
				if(type_string == "bool" or type_string == "path") :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "bool	  " + var_ID + ";\n"
				elif(type_string == "int") :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "int		" + var_ID + ";\n"
				elif(type_string == "float") :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "float	 " + var_ID + ";\n"
				elif(type_string.startswith("bool")) :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "bool	  " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "int		" + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					ParticleRender_head_glsl = ParticleRender_head_glsl + "float	 " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				else :
					print("GLSL code generator: Unknown particle shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

				if(type_string == "bool" or type_string == "path") :
					ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n"
				elif(type_string == "float") :
					ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
						ParticleRender_fs_index  += 1
					ParticleRender_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n";
						ParticleRender_fs_index  += 1
					ParticleRender_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n";
						ParticleRender_fs_index  += 1
					ParticleRender_fs_index  -= 1
				else :
					print("GLSL code generator: Unknown particle shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
							 	
				ParticleRender_fs_index += 1
			
			if(re.search(r'Master_fs', target_shader_list) != None) :
				# Master_fs parameter
				Master_head_glsl = Master_head_glsl + "#define var_" + var_ID + "\n"
				if(type_string == "bool" or type_string == "path") :
					Master_head_glsl = Master_head_glsl + "bool	  " + var_ID + ";\n"
				elif(type_string == "int") :
					Master_head_glsl = Master_head_glsl + "int		" + var_ID + ";\n"
				elif(type_string == "float") :
					Master_head_glsl = Master_head_glsl + "float	 " + var_ID + ";\n"
				elif(type_string.startswith("bool")) :
					Master_head_glsl = Master_head_glsl + "bool	  " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Master_head_glsl = Master_head_glsl + "int		" + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Master_head_glsl = Master_head_glsl + "float	 " + var_ID + "[" + str(header_const_value(index_range[1])) + "];\n"
				else :
					print("GLSL code generator: Unknown master shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)

				if(type_string == "bool" or type_string == "path") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = int(uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n"
				elif(type_string == "float") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				else :
					print("GLSL code generator: Unknown master shader parameter type [%s]" % type_string)
					print("GLSL code generator: End of configuration generation\n\n")
					sys.exit(0)
				
				Master_fs_index += 1


	# once the variables have been scanned, C++ and glsl tables are created to pass the values from CPU to GPU. There is a table per shader.
	if(ParticleAnimation_fs_index > 0) :
		ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "uniform float uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
		
	if(Update_fs_index > 0) :
		Update_head_glsl = Update_head_glsl + "uniform float uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"

	if(Mixing_fs_index > 0) :
		Mixing_head_glsl = Mixing_head_glsl + "uniform float uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"

	if(ParticleRender_fs_index > 0) :
		ParticleRender_head_glsl = ParticleRender_head_glsl + "uniform float uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"

	if(Master_fs_index > 0) :
		Master_head_glsl = Master_head_glsl + "uniform float uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"

	# manages the inclusions inside the shader files to generate the linearized shader files
	for line in ParticleAnimation_InputShader.readlines() :
		if(re.search(r'\#include_declarations', line) != None) :
			ParticleAnimation_OutputShader.write(ParticleAnimation_head_glsl)
		elif(re.search(r'\#include_initializations', line) != None) :
			ParticleAnimation_OutputShader.write(ParticleAnimation_body_glsl)
		else :
			ParticleAnimation_OutputShader.write(line)

	for line in Update_InputShader.readlines() :
		if(re.search(r'\#include_declarations', line) != None) :
			Update_OutputShader.write(Update_head_glsl)
		elif(re.search(r'\#include_initializations', line) != None) :
			Update_OutputShader.write(Update_body_glsl)
		else :
			Update_OutputShader.write(line)

	for line in Mixing_InputShader.readlines() :
		if(re.search(r'\#include_declarations', line) != None) :
			Mixing_OutputShader.write(Mixing_head_glsl)
		elif(re.search(r'\#include_initializations', line) != None) :
			Mixing_OutputShader.write(Mixing_body_glsl)
		else :
			Mixing_OutputShader.write(line)

	for line in ParticleRendering_InputShader.readlines() :
		if(re.search(r'\#include_declarations', line) != None) :
			ParticleRendering_OutputShader.write(ParticleRender_head_glsl)
		elif(re.search(r'\#include_initializations', line) != None) :
			ParticleRendering_OutputShader.write(ParticleRender_body_glsl)
		else :
			ParticleRendering_OutputShader.write(line)

	for line in Master_InputShader.readlines() :
		if(re.search(r'\#include_declarations', line) != None) :
			Master_OutputShader.write(Master_head_glsl)
		elif(re.search(r'\#include_initializations', line) != None) :
			Master_OutputShader.write(Master_body_glsl)
		else :
			Master_OutputShader.write(line)

	ParticleAnimation_InputShader.close()
	ParticleAnimation_OutputShader.close()
	Update_InputShader.close()
	Update_OutputShader.close()
	Mixing_InputShader.close()
	Mixing_OutputShader.close()
	ParticleRendering_InputShader.close() 
	ParticleRendering_OutputShader.close() 
	Master_InputShader.close() 
	Master_OutputShader.close() 

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global Nb_Scenarios
	global Header_Input_name
	global Scenario_InputCsv_names
	global FullScenario_InputCsv_name
	global OutputScenario_OutputCsv_name

	global scenario_vars_columns_list

	global TouchOSCScene_name
	global TouchOSCScene

	global scenario_var_target_shaders

	global full_scenario_vars_specs_dict
	global scenario_vars_specs_dict

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"n:c:f:o:s:t:",["nb_scenarios=","header=", "full_scenario=", "output_scenario=", "scenarios=", "TouchOSC_interface="])
	except getopt.GetoptError:
		print(USAGE)
		print("GLSL code generator: End of configuration generation\n\n")
		sys.exit(2)
	for opt, arg in opts:

		# ARGTS
		if opt in ("-n", "--nb_scenarios"):
			Nb_Scenarios = int(arg)
		elif opt in ("-c", "--header"):
			Header_Input_name = arg
		elif opt in ("-f", "--full_scenario"):
			FullScenario_InputCsv_name = arg
		elif opt in ("-o", "--output_scenario"):
			OutputScenario_OutputCsv_name = arg
		elif opt in ("-s", "--scenarios"):
			Scenario_InputCsv_names  = re.split(r",", arg)
		elif opt in ("-t", "--TouchOSC_interface"):
			TouchOSCScene_name = arg
		else:
			print("GLSL code generator: option (", opt, ")")
			assert False, "unhandled option"

	###############################################################################
	# TOUCHOSC SCENE NAME
	try:
		TouchOSCScene = open(TouchOSCScene_name, "wt")
	except IOError:
		print("GLSL code generator: File not found:", TouchOSCScene_name, " or path is incorrect")

	###############################################################################
	# CSV PARAMETER INPUT FILES
	if(len(Scenario_InputCsv_names) != Nb_Scenarios) :
		print("GLSL code generator: Expected number of scenario files incorrect: ",len(Scenario_InputCsv_names)," instead of ", Nb_Scenarios)
		print("GLSL code generator: End of configuration generation\n\n")
		sys.exit(0)

	##################################################################
	# READS THE CSV HEADER FILE
	##################################################################
	# the first configuration file is used for variables
	# each configuration file is used for shader files
	# the headers of the same shader files across several 
	# configuration files can be different (they depend on each scenario)
	# read_configuration_vars(Header_Input_name[0])
	read_headerConst(Header_Input_name)

	##################################################################
	# READS THE FULL SCENARIO AND MEMORIZES THE HEADER
	##################################################################
	########### reads the header of the full scenario which is supposed to contain all the variables
	# print("reads the header of the full scenario")
	try:
		input_full_scenario_fileCsv = open(FullScenario_InputCsv_name, "rt")
	except IOError:
		print("Input full scenario File not found:", FullScenario_InputCsv_name, " or path is incorrect")
	readFullCSV = csv.reader(input_full_scenario_fileCsv, delimiter=',')
	read_scenario_variables(readFullCSV, -1)
	input_full_scenario_fileCsv.close()

	##################################################################
	# READS THE CSV SCENARIO FILE
	##################################################################
	scenario_vars_columns_list = []
	for scenarioFileName, indScenario in zip(Scenario_InputCsv_names, range(Nb_Scenarios)) :
		try:
			input_scenario_fileCsv = open(scenarioFileName, "rt")
		except IOError:
			print("Input scenario File (configuration: ", indScenario, ") not found:", scenarioFileName, " or path is incorrect")
		readScenarioCSV = csv.reader(input_scenario_fileCsv, delimiter=',')
		read_scenario_variables(readScenarioCSV, indScenario)
		read_scenario_scenes(readScenarioCSV, indScenario)
		input_scenario_fileCsv.close()
		# print(indScenario)
	post_reading_scenarios()

	#####################################################
	##### WRITES A TOUCHOSC FILE WITH ALL THE SCENES FOR ALL THE CONFIGURATIONS
	#####################################################
	WriteTouchOSCScene()

	###############################################################################
	# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
	###############################################################################
	for indScenario in range(Nb_Scenarios) :
	 	write_binding_vars_header_and_body(indScenario)

	###############################################################################
	# END OF GLSL code generator
	###############################################################################
	print("\n*************************************************************\n")
	print("Successfull GLSL code generation and TouchOSC interface from full scenario and local scenarios\n")
	print("*************************************************************\n\n")

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
