#!/usr/bin/python3 

# PG_confGenerator-v1.py
# command to regenerate cpp files + shaders: to be called after each change of variables in the scenario

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

ShadersDir = "./Projects/shaders/"
# command to regenerate cpp files + shaders: to be called after each change of variables in the scenario

# number of configuration and for each the number of scenes and the names of the scenes
Nb_Configurations = 0
nb_scenes = []
scene_names = [[]]

Configuration_InputCsv_names = []
Scenario_InputCsv_names = []
FullScenario_InputCsv_name = ""

# shader input file names (as many of each type as pairs of configuration/scenario files)
ParticleAnimation_InputShader_name = []
Update_InputShader_name = []
Mixing_InputShader_name = []
ParticleRendering_InputShader_name = []
Master_InputShader_name = []

#shader output file names with included declarations & codes of scenario variables
ParticleAnimation_OutputShader_name = []
Update_OutputShader_name = []
Mixing_OutputShader_name = []
ParticleRendering_OutputShader_name = []
Master_OutputShader_name = []

withParticleShaders = []

# C++ included file names (only one file cumulating the data 
# from the first configuration file and all the scenario files)
ScriptHeader_name = ""
ScriptHeader = None

ScriptBody_name = ""
ScriptBody = None

ShaderHeader_name = ""
ShaderHeader = None

ShaderBodyDecl_name = ""
ShaderBodyDecl = None

ShaderBodyBind_name = ""
ShaderBodyBind = None

UpdateBody_name = ""
UpdateBody = None

TouchOSCScene_name = "..\\LYM-sources\\TouchOSC\\scenes.tosc"
TouchOSCScene = None

# full scenario: dictionaries of scenario variables + memory of rank inside scenario
full_scenario_vars_specs_dict = {}
full_varID_from_index = {}
full_nb_vars = 0

# header variable lines to access their value
config_ids = None
config_types = None
config_initial_values = None

# ordinary scenario: dictionaries of scenario variables + memory of rank inside scenario
scenario_vars_specs_dict = []
varID_from_index = {}
nb_vars = []

scenario_vars_columns_list = []
out_scenario_footer = []

# booleans whether a full scenario variable is in the local scenario
# built empty and filled when loading the scenarios in PG
scenario_active_vars = {}

scenario_var_pulsing_modes_dict = dict()

def handler(signal_received, frame) :
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''	print("Configuration generator: Usage: python PG_confGenerator_song-v1.py song/configuration/LYM_song-head-single.csv song/scenarios/LYM_song-scenario-v1.csv song/shaders/LYM_ParticleAnimation_song.frag song/shaders/LYM_ParticleAnimation_song_full.frag song/shaders/LYM_Update_song.frag song/shaders/LYM_Update_song_full.frag song/shaders/LYM_Mixing_song.frag song/shaders/LYM_Mixing_song_full.frag song/shaders/LYM_ParticleRender_song.frag song/shaders/LYM_ParticleRender_song_full.frag song/shaders/LYM_Master_song.frag song/shaders/LYM_Master_song_full.frag ../Porphyrograph/Porphyrograph-song-src/pg_script_header.h ../Porphyrograph/Porphyrograph-song-src/pg_script_body.cpp  ../Porphyrograph/Porphyrograph-song-src/pg_shader_header.h ../Porphyrograph/Porphyrograph-song-src/pg_shader_body_decl.cpp ../Porphyrograph/Porphyrograph-song-src/pg_shader_body_bind.cpp ../Porphyrograph/Porphyrograph-song-src/pg_update_body.cpp")
PG_confGenerator.py: A program for generating shader file & C++ code for Porphyrograph")

'''

##################################################################
# READS THE CONFIGURATION VARIABLES FROM THE CSV CONFIGURATION FILE 
##################################################################
def const_configuration_var_value(var_ID) :
	global config_ids
	global config_types
	global config_initial_values

	for config_id, config_type, config_init in zip(config_ids, config_types, config_initial_values):
		if(config_type == "const" and config_id == var_ID) :
			return int(config_init)
	return -1

def read_configuration_vars(configurationFileName) :
	global ScriptHeader
	global config_ids
	global config_types
	global config_initial_values

	try:
		Configuration_InputCsv = open(configurationFileName, "rt")
	except IOError:
		print("Configuration generator: File not found:", configurationFileName, " or path is incorrect")

	# print("Configuration generator: read variables in configuration file", configurationFileName)

	readCSV = csv.reader(Configuration_InputCsv, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 

	mode = COMMENT
	value = 0
	while (True) :
		if(line[0] == "RANK") :
			# variable rank -> comment
			ScriptHeader.write("// CONFIGURATION CONSTANTS\n")
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable verbatim
			# -> nb of variables 
			nb_variables = len(line) - 1
			# print("Configuration generator: configuration variables:", int(nb_variables))
			# -> comment in the header file
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable types (used for declarations)
			if(line[0] != "TYPE") :
				print("Configuration generator: TYPE line expected not [",line[0],"]!")
				sys.exit(0)
			config_types = line[1:]
			line = next(readCSV)

			# variable strings (used for alias commands from PD 
			# and for variable & initial values declarations)
			if(line[0] != "ID") :
				print("Configuration generator: ID line expected not [",line[0],"]!")
				sys.exit(0)			
			config_ids = line[1:]
			line = next(readCSV)

		elif(line[0] == "initial_values") :
			# initial values
			line = next(readCSV)
			config_initial_values = line[1:]

			#closing tag
			line = next(readCSV)

			#next line
			line = next(readCSV)
		
			############################### configuration variable declarations
			# extern configuration variable declaration in the header file
			for config_id, config_type, config_init in zip(config_ids, config_types, config_initial_values):
				if(config_type == "sign") :
					config_type = "float"
				elif(config_type == "path") :
					config_type = "bool"
				if(config_type == "bool"):
					if(config_init != "0") :
						ScriptHeader.write("constexpr auto %-30s = %-10s;\n" % (config_id, "true"))
					else :
						ScriptHeader.write("constexpr auto %-30s = %-10s;\n" % (config_id, "false"))
				elif(config_type == "string") :
					ScriptHeader.write("constexpr auto %-30s = \"%s\";\n" % (config_id, config_init))
				elif(config_type == "const") :
					ScriptHeader.write("#define %-30s %-10s\n" % (config_id, config_init))
				else :
					ScriptHeader.write("constexpr auto %-30s = %-10s;\n" % (config_id, config_init))
			
			############################### constant declarations
			# number of scene variables
			for config_id in config_ids :
				ScriptHeader.write("#define var_%s\n" % config_id)
			ScriptHeader.write("enum ConfigurationVarIDs {\n")
			# scene variable initialization
			ScriptHeader.write("  _%s = 0," % config_ids[0] + "\n")
			for config_id in config_ids[1:] :
				ScriptHeader.write("  _%s," % config_id + "\n")
				# print("Configuration generator:   _%s,", config_ids
			
			ScriptHeader.write("  _MaxConfigurationVarIDs};\n")
			Configuration_InputCsv.close()
			return()

		else :
			#next line
			line = next(readCSV)

	Configuration_InputCsv.close()
	return()

def configurationVarValue(var_ID) :
	global config_ids
	global config_types
	global config_initial_values

	for config_id, config_type, config_init in zip(config_ids, config_types, config_initial_values):
		if(config_id == var_ID) :
			if(config_type == "const") :
				return( int(config_init) )
			elif(config_type == "int") :
				return( int(config_init) )
			elif(config_type == "float") :
				return( float(config_init) )
			else :
				return( config_init )
	return(0)

##################################################################
# READS THE SHADER FILE NAMES FROM THE CSV CONFIGURATION FILE 
##################################################################
def read_configuration_shader_names(configurationFileName, indConfiguration) :
	global ParticleAnimation_InputShader_name
	global Update_InputShader_name
	global ParticleRendering_InputShader_name
	global Mixing_InputShader_name
	global Master_InputShader_name

	global ParticleAnimation_OutputShader_name
	global Update_OutputShader_name
	global Mixing_OutputShader_name
	global ParticleRendering_OutputShader_name
	global Master_OutputShader_name

	try:
		Configuration_InputCsv = open(configurationFileName, "rt")
	except IOError:
		print("Configuration generator: File not found:", configurationFileName, " or path is incorrect")

	# print("Configuration generator: read shaders in configuration file", configurationFileName, "index", indConfiguration)
	readCSV = csv.reader(Configuration_InputCsv, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 

	mode = COMMENT
	value = 0
	while (True) :
		if(line[0] == "ParticleAnimation") :
			if(line[1] != "NULL") :
				ParticleAnimation_InputShader_name.append(line[1]+".frag")
				ParticleAnimation_OutputShader_name.append(line[1]+"_full_"+str(indConfiguration)+".frag")
			else :
				ParticleAnimation_InputShader_name.append("NULL")
				ParticleAnimation_OutputShader_name.append("NULL")
			line = next(readCSV)

		elif(line[0] == "Update") :
			# print("Configuration generator: Update shader")
			if(line[1] != "NULL") :
				Update_InputShader_name.append(line[1]+".frag")
				Update_OutputShader_name.append(line[1]+"_full_"+str(indConfiguration)+".frag")
			else :
				Update_InputShader_name.append("NULL")
				Update_OutputShader_name.append("NULL")
			line = next(readCSV)

		elif(line[0] == "ParticleRender") :
			if(line[1] != "NULL") :
				ParticleRendering_InputShader_name.append(line[1]+".frag")
				ParticleRendering_OutputShader_name.append(line[1]+"_full_"+str(indConfiguration)+".frag")
			else :
				ParticleRendering_InputShader_name.append("NULL")
				ParticleRendering_OutputShader_name.append("NULL")
			line = next(readCSV)

		elif(line[0] == "Mixing") :
			if(line[1] != "NULL") :
				Mixing_InputShader_name.append(line[1]+".frag")
				Mixing_OutputShader_name.append(line[1]+"_full_"+str(indConfiguration)+".frag")
			else :
				Mixing_InputShader_name.append("NULL")
				Mixing_OutputShader_name.append("NULL")
			line = next(readCSV)

		elif(line[0] == "Master") :
			if(line[1] != "NULL") :
				Master_InputShader_name.append(line[1]+".frag")
				Master_OutputShader_name.append(line[1]+"_full_"+str(indConfiguration)+".frag")
			else :
				Master_InputShader_name.append("NULL")
				Master_OutputShader_name.append("NULL")
			line = next(readCSV)

		elif(line[0] == "/shader_files") :
			Configuration_InputCsv.close()
			return()

		else :
			#next line
			line = next(readCSV)

	Configuration_InputCsv.close()
	return()

##################################################################
# READS THE SCENARIO FILE TOP: VARIABLES 
##################################################################
def expect_tag(line_scenario, tag_string) :
	if(line_scenario[0] != tag_string) :
		print(tag_string, "first column string expected not [",line_scenario[0],"]!")
		print(line_scenario)
		sys.exit(0)
def check_tag(line_scenario, tag_string) :
	return (line_scenario[0] == tag_string)

def read_scenario_variables(scenarioCSV, indConfiguration) :
	global full_scenario_vars_specs_dict
	global full_varID_from_index
	global scenario_vars_specs_dict
	global varID_from_index
	global Nb_Configurations
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
	if(indConfiguration == -1) :
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

	elif(indConfiguration < Nb_Configurations) :
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
		print("scenario #"+str(indConfiguration)+", nb vars:", nb_loc_variables)
		nb_vars.append(nb_loc_variables)

	return

def var_index_in_scenario(var_ID, indConfig) :
	global scenario_vars_specs_dict
	if var_ID in scenario_vars_specs_dict[indConfig] :
		return scenario_vars_specs_dict[indConfig][var_ID][0]
	else :
		return -1

def var_index_in_full_scenario(var_ID) :
	global full_scenario_vars_specs_dict
	if var_ID in full_scenario_vars_specs_dict :
		return full_scenario_vars_specs_dict[var_ID][0]
	else :
		return -1

##################################################################
# READS THE SCENARIO VARIABLES FROM THE CSV SCENARIO FILE 
##################################################################
def read_scenario_scenes(readCSV, indConfiguration) :
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
			scene_names[indConfiguration].append(line_scene_header[1])
			print("Configuration generator: scene", loc_nb_scenes + 1, ": ", scene_names[indConfiguration][loc_nb_scenes])

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
			for idx, head, comm1, comm2, val_init, val_fin, interp in zip(range(1, nb_vars[indConfiguration] + 1), line_scene_header[1:], line_comments1[1:], line_comments2[1:], line_initial_values[1:], line_final_values[1:], line_interpolations[1:]) :
					one_scene_var_value_column_list.append([idx, head, comm1, comm2, val_init, val_fin, interp])

			# append the values of this scene to the preceding scenes
			local_scenario_vars_columns_list.append(one_scene_var_value_column_list)

		elif(line_scene_header[0] == "/scenario") :
			break

		else :
			print("Configuration generator: Unknown scenario line [",lineScenario,"] !")
			sys.exit(0)

		loc_nb_scenes = loc_nb_scenes + 1
		
	local_out_scenario_footer = []
	while(True):
		try: 
			lineScenario = next(readCSV)
			local_out_scenario_footer.append(lineScenario)
		except StopIteration:
			break

	out_scenario_footer.append(local_out_scenario_footer)
	scenario_vars_columns_list.append(local_scenario_vars_columns_list)
	nb_scenes.append(loc_nb_scenes)
	print("Configuration generator: scenes:", nb_scenes[indConfiguration])

##################################################################
# GENERATES THE NEW SCENARIO ACCORDING TO THE FULL SCENARIO TEMPLATE
# SEPARATE FULL SCENARIO VARIABLES INTO ACTIVE AND PASSIVE ONES
##################################################################
def post_reading_scenarios() :
	global full_scenario_vars_specs_dict
	global scenario_vars_specs_dict

	global Nb_Configurations

	global scenario_active_vars

	global Scenario_InputCsv_names
	global FullScenario_InputCsv_name

	global scenario_vars_columns_list
	global out_scenario_footer

	scenario_unknown_vars_specs_dict = {}

	# CHECKS THE COMPLETENESS OF THE FULL SCENARIO
	# creates a dict with all the variables in the local scenarios which are
	# not inside the full scenario
	for indConfiguration in range(Nb_Configurations) :
		for var_ID in scenario_vars_specs_dict[indConfiguration] :
			if(var_ID != "" and var_index_in_full_scenario(var_ID) < 0) :
				print("Unknown variable:", var_ID, ", specs:", scenario_vars_specs_dict[indConfiguration][var_ID])
				scenario_unknown_vars_specs_dict[var_ID] = [indConfiguration] + scenario_vars_specs_dict[indConfiguration][var_ID]

	# and exists if all the variables are not inside the full scenario, 
	# so that they can be added there
	if(len(scenario_unknown_vars_specs_dict) != 0) :
		print(scenario_unknown_vars_specs_dict)
		unknown_vars_scenario_name = Scenario_InputCsv_names[indConfiguration]
		path = re.findall(r'^(.*[/\\])[^/\\\.]+\.csv$', unknown_vars_scenario_name)
		if(path != []) :
			unknown_vars_scenario_name = path[0] + f'''scenario_{indConfiguration}.csv'''
		else :
			unknown_vars_scenario_name = f'''scenario_{indConfiguration}.csv'''

		try:
			unknown_vars_scenario_fileCsv = open(unknown_vars_scenario_name, "wt", newline='')
		except IOError:
			print("Configuration generator: New Scenario File not opened:", unknown_vars_scenario_name, " or path is incorrect")
		print("Some variables in scenarios should be declared inside the full scenario")
		print("See file:", unknown_vars_scenario_name)

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
		sys.exit(0)

	# TAKES NOTE OF THE CONFIGURATIONS IN WHICH THE VARIABLES ARE KNOWN
	# creates a vector of Nb_Configurations boobleans indicating whether or not 
	# a variable of the full scenario is in one of the configurations
	for var_ID in full_scenario_vars_specs_dict:
		# initializes with Nb_Configurations False (variable in no scenario) of size configurationVarValue("PG_MAX_CONFIGURATIONS")
		scenario_active_vars[var_ID] = [False] * configurationVarValue("PG_MAX_CONFIGURATIONS")
		# print("initializes False (variable in no scenario) of size ", configurationVarValue (PG_MAX_CONFIGURATIONS), ".")

	# checks the presence of the vars in one of the scenarios
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		for indConfiguration in range(Nb_Configurations):
			if(var_index_in_scenario(var_ID, indConfiguration) >= 0) :
				scenario_active_vars[var_ID][indConfiguration] = True

				# checks that the parameters in the local scenario are the same as in the full scenario
				# variable is already known, checks that it is coherent with what is already stored
				# [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
				local_specs = scenario_vars_specs_dict[indConfiguration][var_ID]

				if(local_specs[2] != full_specs[2]) :
					print("Configuration generator: var",var_ID,"has inconsistent types in full scenario",	FullScenario_InputCsv_name,\
						"(",full_specs[2],") and in scenario",Scenario_InputCsv_names[indConfiguration],"(",local_specs[2],")!")
					sys.exit(0)
				if(local_specs[3] != full_specs[3]) :
					print("Configuration generator: var",var_ID,"has inconsistent callBacks in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[3],") and in scenario",Scenario_InputCsv_names[indConfiguration],"(",local_specs[3],")!")
					sys.exit(0)
				if(local_specs[5] != full_specs[5]) :
					print("Configuration generator: var",var_ID,"has inconsistent target shaders in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[5],") and in scenario",Scenario_InputCsv_names[indConfiguration],"(",local_specs[5],")!")
					sys.exit(0)
				if(local_specs[6] != full_specs[6]) :
					print("Configuration generator: var",var_ID,"has inconsistent pulsing modes in scenario",FullScenario_InputCsv_name,\
						"(",full_specs[6],") and in scenario",Scenario_InputCsv_names[indConfiguration],"(",local_specs[6],")!")
					sys.exit(0)

	# generates new scenarios in which the variables are ranked in the same order as in the full scenario
	# these scenarios are the ones which will be loaded by pg
	for indConfiguration in range(Nb_Configurations):
		new_scenario_name = Scenario_InputCsv_names[indConfiguration]
		path = re.findall(r'^(.*[/\\])[^/\\\.]+\.csv$', new_scenario_name)
		file_rank = indConfiguration + 1
		if(path != []) :
			new_scenario_name = path[0] + f'''scenario_{file_rank}.csv'''
		else :
			new_scenario_name = f'''scenario_{file_rank}.csv'''

		try:
			output_scenario_fileCsv = open(new_scenario_name, "wt", newline='')
		except IOError:
			print("Configuration generator: New Scenario File not opened:", new_scenario_name, " or path is incorrect")

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
			if(scenario_active_vars[var_ID][indConfiguration] == True) :
				varVerbatim.append(full_specs[1])
				varType.append(full_specs[2])
				varCallBack.append(full_specs[3])
				varGUI.append(full_specs[4])
				varShader.append(full_specs[5])
				varPulse.append(full_specs[6])
				# initial value taken for scenario
				local_specs = scenario_vars_specs_dict[indConfiguration][var_ID]
				varInitial.append(local_specs[7])
				varID.append(var_ID)

		CSVwriter = csv.writer(output_scenario_fileCsv, delimiter=',')
		CSVwriter.writerow(["RANK"] + list(range(1, len(full_scenario_vars_specs_dict.keys()) + 1)))
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
		for scene_columns in scenario_vars_columns_list[indConfiguration] :
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
				if(scenario_active_vars[var_ID][indConfiguration] == True) :
					# index of the variable inside the current scenario to retrieve its scene values 
					# (except the first line which is depending on the rank of the variable in the new scenario)
					var_index = var_index_in_scenario(var_ID, indConfiguration)
					if(var_index < 0 or var_index >= len(scene_columns)) :
						print("Configuration generator: var",var_ID,"scenes not found in scenario",Scenario_InputCsv_names[indConfiguration],"(config: ",indConfiguration,")!")
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
		for out_scenario_footer_line in out_scenario_footer[indConfiguration]:
			CSVwriter.writerow(out_scenario_footer_line)

		output_scenario_fileCsv.close()

##################################################################
# WRITES A TOUCHOSC FILE WITH ALL THE SCENES FOR ALL THE CONFIGURATIONS 
##################################################################
def WriteTouchOSCScene() :
	global Nb_Configurations
	global nb_scenes
	global scene_names

	global TouchOSCScene


	button_height = 70
	TouchOSCScene.write(AutomaticSceneGeneration_strings.scene_head_string.format('group_ID_0'))
	TouchOSCScene.write(AutomaticSceneGeneration_strings.configuration_radio_label_string.format('configuration_radio_ID_0', (button_height + 20) * Nb_Configurations, Nb_Configurations, 'configuration_label_ID_0'))
	current_height = 10
	for indConfig in range(Nb_Configurations) :
		if(nb_scenes[indConfig] <= 8) :
			button_width = (1800 / nb_scenes[indConfig]) - 30
		else :
			button_width = (1800 / 8) - 30
		current_width = 120
		for indScene in range(nb_scenes[indConfig]) :
			TouchOSCScene.write(AutomaticSceneGeneration_strings.button_string.format('button_ID' + '_' + str(indConfig) + '_' + str(indScene), current_width, current_height, button_width, button_height, scene_names[indConfig][indScene],scene_buttons_colors[indConfig % 7][0]/255.0,scene_buttons_colors[indConfig % 7][1]/255.0,scene_buttons_colors[indConfig % 7][2]/255.0))
			TouchOSCScene.write(AutomaticSceneGeneration_strings.label_string.format('label_ID' + '_' + str(indConfig) + '_' + str(indScene), current_width, current_height, button_width, button_height, scene_names[indConfig][indScene]))
			current_width += button_width + 30
			if(indScene > 0 and indScene < nb_scenes[indConfig] - 1 and (indScene + 1) % 8 == 0) :
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
	print("Configuration generator: Unknown scenario variable type ", scenario_type)
	sys.exit(0)

def scenario_to_cpp_range(scenario_type) :
	if(scenario_type == "sign" or scenario_type == "path" or scenario_type == "bool" or scenario_type == "int" or scenario_type == "float" or scenario_type == "string") :
		return []
	elif(scenario_type.startswith("bool") or scenario_type.startswith("int") or scenario_type.startswith("float")) :
		indices = re.findall(r'.*\[([0-9]+)\.\.([^/[]+)\]$', scenario_type)
		if(len(indices) == 1 and len(indices[0]) == 2) :
			return [indices[0][0], indices[0][1]]
	print("Configuration generator: Unknown scenario variable type ", scenario_type)
	sys.exit(0)


def write_script_header_and_body() :
	global ScriptHeader
	global ScriptBody
	global ShaderBodyDecl

	global full_scenario_vars_specs_dict

	ScriptHeader.write("// SCENARIO VARIABLES # FULL SCENARIO\n")

	############################### constant declarations
	# number of scene variables
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID in full_scenario_vars_specs_dict:
		ScriptHeader.write("#define var_%s\n" % var_ID)
	ScriptHeader.write("enum InterpVarIDs{ \n")
	# scene variable initialization
	scenario_var_ids = list(full_scenario_vars_specs_dict.keys())
	ScriptHeader.write("  _%s = 0,\n" % scenario_var_ids[0])
	for var_ID in scenario_var_ids[1:] :
		ScriptHeader.write("  _%s,\n" % var_ID)
		# print("Configuration generator:   _%s,", scenario_var_ids
	ScriptHeader.write("  _MaxInterpVarIDs};\n")
	# OLD: ScriptHeader.write("#define _NbConfigurations %d\n" % Nb_Configurations)

	############################### scenario variable declarations
	# interpolation cancelation variable declaration
	# ScriptBody.write("bool BrokenInterpolationVar[ _MaxInterpVarIDs ]"
	ScriptBody.write("// SCENARIO VARIABLES\n")

	# scenario variable declarations
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = scenario_to_cpp_type(full_specs[2])
		index_range = scenario_to_cpp_range(full_specs[2])
		if(index_range == []) :
			ScriptBody.write("%-5s %-20s;\n" % (type_string , var_ID))
		else :
			ScriptBody.write("%-5s %-20s[%s];\n" % (type_string , var_ID, index_range[1]))

	# extern scenario variable declaration in the header file
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = scenario_to_cpp_type(full_specs[2])
		index_range = scenario_to_cpp_range(full_specs[2])
		if(index_range == []) :
			ScriptHeader.write("extern %-5s %-20s;\n" % (type_string , var_ID))
		else :
			ScriptHeader.write("extern %-5s %-20s[%s];\n" % (type_string , var_ID, index_range[1]))

	# extern scenario variable types, pointers and messages declaration in the header file
	ScriptHeader.write("enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path , _pg_string };\n")
	ScriptHeader.write("enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };\n")
	ScriptHeader.write("extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern int ScenarioVarIndiceRanges[_MaxInterpVarIDs][2];\n")
	ScriptHeader.write("extern void * ScenarioVarPointers[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *ScenarioVarMessages[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *ScenarioVarStrings[_MaxInterpVarIDs];\n")

	# scenario variable types declarations
	ScriptBody.write("VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = scenario_to_cpp_type(full_specs[2])
		ScriptBody.write("	_pg_%s,\n" % type_string)
	ScriptBody.write("};\n")

	# scenario variable ranges declarations
	ScriptBody.write("int ScenarioVarIndiceRanges[_MaxInterpVarIDs][2] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		index_range = scenario_to_cpp_range(full_specs[2])
		if(index_range == []) :
			ScriptBody.write("	{-1, -1},\n")
		else :
			ScriptBody.write('	{{{0}, {1}}},\n'.format(index_range[0], index_range[1]))
	ScriptBody.write("};\n")

	# scenario variable pointers declarations
	ScriptBody.write("void * ScenarioVarPointers[_MaxInterpVarIDs] = { \n")
	for var_ID in full_scenario_vars_specs_dict:
		ScriptBody.write("	(void *)&%s,\n" % var_ID)
	ScriptBody.write("};\n")

	# ScriptBody.write("union scenarioValue {\n")
	# ScriptBody.write("	double val_num;\n")
	# ScriptBody.write("	string val_string;\n")
	# ScriptBody.write("~ScenarioValue() {}};\n")

	# variable callback pointers declarations	
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		callBack_id_string = full_specs[3]
		type_string = full_specs[2]
		if(callBack_id_string != "NULL") :
			callBack_generic_id_string = callBack_id_string + "_generic"
			if(type_string == "bool") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));\n" % callBack_id_string)
			elif(type_string == "sign") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, double_to_sign(scenario_or_gui_command_value.val_num));\n" % callBack_id_string)
			elif(type_string == "path") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));\n" % callBack_id_string)
			elif(type_string == "float") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, float(scenario_or_gui_command_value.val_num));\n" % callBack_id_string)
			elif(type_string == "int") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, int(scenario_or_gui_command_value.val_num));\n" % callBack_id_string)
			elif(type_string == "string") :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, scenario_or_gui_command_value.val_string);\n" % callBack_id_string)
			elif(type_string.startswith("int")) :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, double *scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, scenario_or_gui_command_value.val_array);\n" % callBack_id_string)
			elif(type_string.startswith("bool")) :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, double *scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, scenario_or_gui_command_value.val_array);\n" % callBack_id_string)
			elif(type_string.startswith("float")) :
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, double *scenario_or_gui_command_value);\n" % callBack_id_string)
				ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {\n" % callBack_generic_id_string)
				ScriptBody.write("	%s(param_input_type, scenario_or_gui_command_value.val_array);\n" % callBack_id_string)
			else :
				print("Configuration generator: Unknown callback ",callBack_id_string," parameter type [",type_string,"]")
				sys.exit(0)
			ScriptBody.write("}\n")

	ScriptBody.write("void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue) = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		callBack_id_string = full_specs[3]
		callBack_generic_id_string = callBack_id_string + "_generic"
		if(callBack_id_string == "NULL") :
			ScriptBody.write("	NULL,\n")
		else :
			ScriptBody.write("	&%s,\n" % callBack_generic_id_string)
	ScriptBody.write("};\n")

	ScriptBody.write("char *ScenarioVarMessages[_MaxInterpVarIDs] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		msg_string = full_specs[4]
		if(msg_string == "NULL") :
			ScriptBody.write("  (char *)\"\",\n")
		else :
			ScriptBody.write("  (char *)\""+msg_string+"\",\n")
	ScriptBody.write("};\n")

	# ScriptHeader.write("enum PulseTypes : _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none "
	ScriptBody.write("PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = { ")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		pulsing_mode = full_specs[6]
		if(pulsing_mode == "*") :
			ScriptBody.write("  _pg_pulsed_none,\n")
		elif(pulsing_mode == "pulsed_absolute") :
			ScriptBody.write("  _pg_pulsed_absolute,\n")
		elif(pulsing_mode == "pulsed_uniform") :
			ScriptBody.write("  _pg_pulsed_uniform,\n")
		elif(pulsing_mode == "pulsed_differential") :
			ScriptBody.write("  _pg_pulsed_differential,\n")
		elif(pulsing_mode == "pulsed_special") :
			ScriptBody.write("  _pg_pulsed_special,\n")
	ScriptBody.write("};\n")

	# alias string list declarations (commands received from PD)
	ScriptBody.write("char *ScenarioVarStrings[_MaxInterpVarIDs] = { \n")
	for var_ID in full_scenario_vars_specs_dict:
		ScriptBody.write("  (char *)\""+var_ID+"\",\n")
	ScriptBody.write("};\n")

	ShaderBodyDecl.write("GLint uniform_ParticleAnimation_scenario_var_data[%d] = {0};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Update_scenario_var_data[%d] = {0};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Mixing_scenario_var_data[%d] = {0};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_ParticleRender_scenario_var_data[%d] = {0};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Master_scenario_var_data[%d] = {0};\n" % Nb_Configurations)
	ShaderBodyDecl.write("float *ParticleAnimation_scenario_var_data[" + str(Nb_Configurations) + "];\n")
	ShaderBodyDecl.write("float *Update_scenario_var_data[" + str(Nb_Configurations) + "];\n")
	ShaderBodyDecl.write("float *Mixing_scenario_var_data[" + str(Nb_Configurations) + "];\n")
	ShaderBodyDecl.write("float *ParticleRender_scenario_var_data[" + str(Nb_Configurations) + "];\n")
	ShaderBodyDecl.write("float *Master_scenario_var_data[" + str(Nb_Configurations) + "];\n")


	ScriptHeader.close() 
	ScriptBody.close() 

###############################################################################
# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
###############################################################################
def write_binding_vars_header_and_body(indConfiguration) :
	# shader input file names (as many of each type as pairs of configuration/scenario files)
	global ParticleAnimation_InputShader_name
	global Update_InputShader_name
	global Mixing_InputShader_name
	global ParticleRendering_InputShader_name
	global Master_InputShader_name

	#shader output file names with included declarations & codes of scenario variables
	global ParticleAnimation_OutputShader_name
	global Update_OutputShader_name
	global Mixing_OutputShader_name
	global ParticleRendering_OutputShader_name
	global Master_OutputShader_name


	global Nb_Configurations
	global Scenario_InputCsv_names

	global ShaderHeader_name
	global ShaderHeader

	global ShaderBodyBind_name
	global ShaderBodyBind

	global UpdateBody_name
	global UpdateBody

	global scenario_var_target_shaders

	global ShadersDir

	global withParticleShaders

	global scenario_active_vars

	try:
		Update_InputShader = open(ShadersDir+Update_InputShader_name[indConfiguration], "rt")
	except IOError:
		print("Configuration generator: Update_InputShader File not found:", ShadersDir+Update_InputShader_name[indConfiguration], " or path is incorrect")
	try:
		Update_OutputShader = open(ShadersDir+Update_OutputShader_name[indConfiguration], "wt")
	except IOError:
		print("Configuration generator: Update_OutputShader File not opened:", ShadersDir+Update_OutputShader_name[indConfiguration], " or path is incorrect")
	try:
		Mixing_InputShader = open(ShadersDir+Mixing_InputShader_name[indConfiguration], "rt")
	except IOError:
		print("Configuration generator: Mixing_InputShader File not found:", ShadersDir+Mixing_InputShader_name[indConfiguration], " or path is incorrect")
	try:
		Mixing_OutputShader = open(ShadersDir+Mixing_OutputShader_name[indConfiguration], "wt")
	except IOError:
		print("Configuration generator: Mixing_OutputShader File not opened:", ShadersDir+Mixing_OutputShader_name[indConfiguration], " or path is incorrect")
	try:
		Master_InputShader = open(ShadersDir+Master_InputShader_name[indConfiguration], "rt")
	except IOError:
		print("Configuration generator: Master_InputShader File not found:", ShadersDir+Master_InputShader_name[indConfiguration], " or path is incorrect")
	try:
		Master_OutputShader = open(ShadersDir+Master_OutputShader_name[indConfiguration], "wt")
	except IOError:
		print("Configuration generator: Master_OutputShader File not opened:", ShadersDir+Master_OutputShader_name[indConfiguration], " or path is incorrect")

	# GLSL OPTIONAL PARTICLE SHADER INPUT AND OUTPUT FILES
	if(ParticleAnimation_InputShader_name[indConfiguration] != "NULL") :
		withParticleShaders[indConfiguration] = True
		try:
			ParticleAnimation_InputShader = open(ShadersDir+ParticleAnimation_InputShader_name[indConfiguration], "rt")
		except IOError:
			print("Configuration generator: ParticleAnimation_InputShader File not found:", ShadersDir+ParticleAnimation_InputShader_name[indConfiguration], " or path is incorrect")
		try:
			ParticleAnimation_OutputShader = open(ShadersDir+ParticleAnimation_OutputShader_name[indConfiguration], "wt")
		except IOError:
			print("Configuration generator: ParticleAnimation_OutputShader File not opened:", ShadersDir+ParticleAnimation_OutputShader_name[indConfiguration], " or path is incorrect")
		try:
			ParticleRendering_InputShader = open(ShadersDir+ParticleRendering_InputShader_name[indConfiguration], "rt")
		except IOError:
			print("Configuration generator: ParticleRendering_InputShader File not found:", ShadersDir+ParticleRendering_InputShader_name[indConfiguration], " or path is incorrect")
		try:
			ParticleRendering_OutputShader = open(ShadersDir+ParticleRendering_OutputShader_name[indConfiguration], "wt")
		except IOError:
			print("Configuration generator: ParticleRendering_OutputShader File not opened:", ShadersDir+ParticleRendering_OutputShader_name[indConfiguration], " or path is incorrect")
	else:
		withParticleShaders[indConfiguration] = False

	##################################################################
	# SHADER VARIABLES
	# WRITES SHADERS DECLARATIONS AND VARIABLE VALUES
	##################################################################
	ParticleRender_fs_index = 0
	Master_fs_index = 0
	ParticleAnimation_fs_index = 0
	Update_fs_index = 0
	Mixing_fs_index = 0

	ParticleRender_bindingString_cpp = ""
	Master_bindingString_cpp = ""
	ParticleAnimation_bindingString_cpp = ""
	Update_bindingString_cpp = ""
	Mixing_bindingString_cpp = ""

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

	ShaderBodyBind.write("\n"); 

	# scans the line of the scenario file that indicates what is the target shader for each variable
	# variables are grouped by 4 to be declared as vec4
	# target_shader_list is * / ParticleRender_fs / Master_fs / Update_fs / Mixing_fs / ParticleAnimation_fs
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = full_specs[2]
		index_range = scenario_to_cpp_range(type_string)
		target_shader_list = full_specs[5]
		pulsing_mode = full_specs[6]
		active_var_in_config = scenario_active_vars[var_ID]
		if(target_shader_list != "*" and active_var_in_config[indConfiguration] == True) :
			# "*" nothing is done, the variable is not a shader parameter or the variable is not in the scenario of this configuration
			if(withParticleShaders[indConfiguration]) :
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
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "bool	  " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					elif(type_string.startswith("int")) :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "int		" + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					elif(type_string.startswith("float")) :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "float	 " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					else :
						print("Configuration generator: Unknown particle animation shader parameter type [%s]" % type_string)
						sys.exit(0)
					
					if(ParticleAnimation_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleAnimation_scenario_var_data[%d];\n" % Nb_Configurations)
						ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_ParticleAnimation_scenario_var_data, \"uniform_ParticleAnimation_scenario_var_data\", _pg_shader_ParticleAnimation);\n")
					
					if(type_string == "bool" or type_string == "path") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
					elif(type_string == "int") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n"
					elif(type_string == "float") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + " = uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
					elif(type_string.startswith("bool")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
							ParticleAnimation_fs_index  += 1
						ParticleAnimation_fs_index  -= 1
					elif(type_string.startswith("int")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n";
							ParticleAnimation_fs_index  += 1
						ParticleAnimation_fs_index  -= 1
					elif(type_string.startswith("float")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n";
							ParticleAnimation_fs_index  += 1
						ParticleAnimation_fs_index  -= 1
					else :
						print("Configuration generator: Unknown particle animation shader parameter type [%s]" % type_string)
						sys.exit(0)
					
					if(not re.findall(r'\[', type_string)) :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "  ParticleAnimation_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleAnimation_fs_index) + "] = "
						if(pulsing_mode == "*") :
							ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + ";\n"
						elif(pulsing_mode == "pulsed_absolute") :
							ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
						elif(pulsing_mode == "pulsed_differential") :
							ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
						else :
							print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							sys.exit(0)
					else:
						ParticleAnimation_fs_index -= int(configurationVarValue(index_range[1])) - 1 - int(index_range[0])
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "  ParticleAnimation_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleAnimation_fs_index) + "] = "
							if(pulsing_mode == "*") :
								ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
							elif(pulsing_mode == "pulsed_absolute") :
								ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
							elif(pulsing_mode == "pulsed_uniform") :
								ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
							elif(pulsing_mode == "pulsed_differential") :
								ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
							else :
								print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
								sys.exit(0)
							ParticleAnimation_fs_index  += 1
						ParticleAnimation_fs_index  -= 1
				 	
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
					Update_head_glsl = Update_head_glsl + "bool	  " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Update_head_glsl = Update_head_glsl + "int		" + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Update_head_glsl = Update_head_glsl + "float	 " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				else :
					print("Configuration generator: Unknown update shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Update_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Update_scenario_var_data[%d];\n" % Nb_Configurations);
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Update_scenario_var_data, \"uniform_Update_scenario_var_data\", _pg_shader_Update);\n")

				if(type_string == "bool" or type_string == "path") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n"
				elif(type_string == "float") :
					Update_body_glsl = Update_body_glsl + "  " + var_ID + " = uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Update_body_glsl = Update_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n";
						Update_fs_index  += 1
					Update_fs_index  -= 1
				else :
					print("Configuration generator: Unknown update shader parameter type [%s]" % type_string)
					sys.exit(0)
				
				if(not re.findall(r'\[', type_string)) :
					Update_bindingString_cpp = Update_bindingString_cpp + "  Update_scenario_var_data["+str(indConfiguration)+"][" + str(Update_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("Configuration generator: Unknown Update pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				else:
					Update_fs_index -= int(configurationVarValue(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Update_bindingString_cpp = Update_bindingString_cpp + "  Update_scenario_var_data["+str(indConfiguration)+"][" + str(Update_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							sys.exit(0)
						Update_fs_index  += 1
					Update_fs_index  -= 1
				
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
					Mixing_head_glsl = Mixing_head_glsl + "bool	  " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Mixing_head_glsl = Mixing_head_glsl + "int		" + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Mixing_head_glsl = Mixing_head_glsl + "float	 " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				else :
					print("Configuration generator: Unknown mixing shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Mixing_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Mixing_scenario_var_data[%d];\n" % Nb_Configurations)
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Mixing_scenario_var_data, \"uniform_Mixing_scenario_var_data\", _pg_shader_Mixing);\n")
				
				if(type_string == "bool" or type_string == "path") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n"
				elif(type_string == "float") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + " = uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				elif(type_string.startswith("int")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				elif(type_string.startswith("float")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Mixing_body_glsl = Mixing_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n";
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
				else :
					print("Configuration generator: Unknown mixing shader parameter type [%s]" % type_string)
					sys.exit(0)
				
				if(not re.findall(r'\[', type_string)) :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "  Mixing_scenario_var_data["+str(indConfiguration)+"][" + str(Mixing_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("Configuration generator: Unknown Mixing pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				else:
					Mixing_fs_index -= int(configurationVarValue(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Mixing_bindingString_cpp = Mixing_bindingString_cpp + "  Mixing_scenario_var_data["+str(indConfiguration)+"][" + str(Mixing_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							sys.exit(0)
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
			 	
				Mixing_fs_index += 1
			

			if(withParticleShaders[indConfiguration]) :
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
						ParticleRender_head_glsl = ParticleRender_head_glsl + "bool	  " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					elif(type_string.startswith("int")) :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "int		" + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					elif(type_string.startswith("float")) :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "float	 " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
					else :
						print("Configuration generator: Unknown particle shader parameter type [%s]" % type_string)
						sys.exit(0)

					if(ParticleRender_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleRender_scenario_var_data[%d];\n" % Nb_Configurations)
						ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_ParticleRender_scenario_var_data, \"uniform_ParticleRender_scenario_var_data\", _pg_shader_Particle);\n")
					
					if(type_string == "bool" or type_string == "path") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
					elif(type_string == "int") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n"
					elif(type_string == "float") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + " = uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
					elif(type_string.startswith("bool")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
							ParticleRender_fs_index  += 1
						ParticleRender_fs_index  -= 1
					elif(type_string.startswith("int")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n";
							ParticleRender_fs_index  += 1
						ParticleRender_fs_index  -= 1
					elif(type_string.startswith("float")) :
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n";
							ParticleRender_fs_index  += 1
						ParticleRender_fs_index  -= 1
					else :
						print("Configuration generator: Unknown particle shader parameter type [%s]" % type_string)
						sys.exit(0)
					
					if(not re.findall(r'\[', type_string)) :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + " ParticleRender_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleRender_fs_index) + "] = "
						if(pulsing_mode == "*") :
							ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + ";\n"
						elif(pulsing_mode == "pulsed_absolute") :
							ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
						elif(pulsing_mode == "pulsed_differential") :
							ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
						else :
							print("Configuration generator: Unknown ParticleRender pulsing mode [%s]" % pulsing_mode)
							sys.exit(0)
					else:
						ParticleRender_fs_index -= int(configurationVarValue(index_range[1])) - 1 - int(index_range[0])
						for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
							ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "  Mixing_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleRender_fs_index) + "] = "
							if(pulsing_mode == "*") :
								ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
							elif(pulsing_mode == "pulsed_absolute") :
								ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
							elif(pulsing_mode == "pulsed_uniform") :
								ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
							elif(pulsing_mode == "pulsed_differential") :
								ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
							else :
								print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
								sys.exit(0)
							ParticleRender_fs_index  += 1
						ParticleRender_fs_index  -= 1
				 	
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
					Master_head_glsl = Master_head_glsl + "bool	  " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("int")) :
					Master_head_glsl = Master_head_glsl + "int		" + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				elif(type_string.startswith("float")) :
					Master_head_glsl = Master_head_glsl + "float	 " + var_ID + "[" + str(configurationVarValue(index_range[1])) + "];\n"
				else :
					print("Configuration generator: Unknown master shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Master_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Master_scenario_var_data[%d];\n" % Nb_Configurations)
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Master_scenario_var_data, \"uniform_Master_scenario_var_data\", _pg_shader_Master);\n")
				
				if(type_string == "bool" or type_string == "path") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
				elif(type_string == "int") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = int(uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n"
				elif(type_string == "float") :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
				elif(type_string.startswith("bool")) :
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				elif(type_string.startswith("int")) :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = int(uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n"
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				elif(type_string.startswith("float")) :
					Master_body_glsl = Master_body_glsl + "  " + var_ID + " = uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Master_body_glsl = Master_body_glsl + "  " + var_ID + "[" + str(index) + "] = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n";
						Master_fs_index  += 1
					Master_fs_index  -= 1
				else :
					print("Configuration generator: Unknown master shader parameter type [%s]" % type_string)
					sys.exit(0)
				
				if(not re.findall(r'\[', type_string)) :
					Master_bindingString_cpp = Master_bindingString_cpp + "  Master_scenario_var_data["+str(indConfiguration)+"][" + str(Master_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("Configuration generator: Unknown Master pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				else:
					Master_fs_index -= int(configurationVarValue(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), configurationVarValue(index_range[1])) :
						Master_bindingString_cpp = Master_bindingString_cpp + "  Mixing_scenario_var_data["+str(indConfiguration)+"][" + str(Master_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							sys.exit(0)
						Master_fs_index  += 1
					Master_fs_index  -= 1
				
				Master_fs_index += 1


	# once the varaiabes have been scanned and grouped by 4, the remaining ones are transformed into variables
	if(withParticleShaders[indConfiguration]) :
		if(ParticleAnimation_fs_index > 0) :
			ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "uniform float uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
			ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "  glUniform1fv(uniform_ParticleAnimation_scenario_var_data["+str(indConfiguration)+"], " + str(ParticleAnimation_fs_index) + ", ParticleAnimation_scenario_var_data["+str(indConfiguration)+"]);\n"
			ShaderHeader.write("extern float *ParticleAnimation_scenario_var_data[" + str(Nb_Configurations) + "];\n")
			ShaderBodyBind.write("	ParticleAnimation_scenario_var_data["+str(indConfiguration)+"]  = new float[" + str(ParticleAnimation_fs_index) + "];\n")
		
	if(Update_fs_index > 0) :
		Update_head_glsl = Update_head_glsl + "uniform float uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
		Update_bindingString_cpp = Update_bindingString_cpp + "  glUniform1fv(uniform_Update_scenario_var_data["+str(indConfiguration)+"], " + str(Update_fs_index) + ", Update_scenario_var_data["+str(indConfiguration)+"]);\n"
		ShaderHeader.write("extern float *Update_scenario_var_data[" + str(Nb_Configurations) + "];\n")
		ShaderBodyBind.write("	Update_scenario_var_data["+str(indConfiguration)+"]  = new float[" + str(Update_fs_index) + "];\n")

	if(Mixing_fs_index > 0) :
		Mixing_head_glsl = Mixing_head_glsl + "uniform float uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
		Mixing_bindingString_cpp = Mixing_bindingString_cpp + "  glUniform1fv(uniform_Mixing_scenario_var_data["+str(indConfiguration)+"], " + str(Mixing_fs_index) + ", Mixing_scenario_var_data["+str(indConfiguration)+"]);\n"
		ShaderHeader.write("extern float *Mixing_scenario_var_data[" + str(Nb_Configurations) + "];\n")
		ShaderBodyBind.write("	Mixing_scenario_var_data["+str(indConfiguration)+"]  = new float[" + str(Mixing_fs_index) + "];\n")

	if(withParticleShaders[indConfiguration]) :
		if(ParticleRender_fs_index > 0) :
			ParticleRender_head_glsl = ParticleRender_head_glsl + "uniform float uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
			ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "  glUniform1fv(uniform_ParticleRender_scenario_var_data["+str(indConfiguration)+"], " + str(ParticleRender_fs_index) + ", ParticleRender_scenario_var_data["+str(indConfiguration)+"]);\n"
			ShaderHeader.write("extern float *ParticleRender_scenario_var_data[" + str(Nb_Configurations) + "];\n")
			ShaderBodyBind.write("	ParticleRender_scenario_var_data["+str(indConfiguration)+"]  = new float[" + str(ParticleAnimation_fs_index) + "];\n")

	if(Master_fs_index > 0) :
		Master_head_glsl = Master_head_glsl + "uniform float uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
		Master_bindingString_cpp = Master_bindingString_cpp + "  glUniform1fv(uniform_Master_scenario_var_data["+str(indConfiguration)+"], " + str(Master_fs_index) + ", Master_scenario_var_data["+str(indConfiguration)+"]);\n"
		ShaderHeader.write("extern float *Master_scenario_var_data[" + str(Nb_Configurations) + "];\n")
		ShaderBodyBind.write("	Master_scenario_var_data["+str(indConfiguration)+"]  = new float[" + str(Master_fs_index) + "];\n")

	# draws the bindings grouped by shader in the draw.cpp file

	UpdateBody.write("\n if(pg_current_configuration_rank == "+str(indConfiguration)+") {\n")
	if(withParticleShaders[indConfiguration]) :
		UpdateBody.write("\n  glUseProgram(pg_shader_programme["+str(indConfiguration)+"][_pg_shader_ParticleAnimation]);\n" + ParticleAnimation_bindingString_cpp)

	UpdateBody.write("\n  glUseProgram(pg_shader_programme["+str(indConfiguration)+"][_pg_shader_Update]);\n" + Update_bindingString_cpp)
	UpdateBody.write("\n  glUseProgram(pg_shader_programme["+str(indConfiguration)+"][_pg_shader_Mixing]);\n" + Mixing_bindingString_cpp)
	if(withParticleShaders[indConfiguration]) :
		UpdateBody.write("\n  glUseProgram(pg_shader_programme["+str(indConfiguration)+"][_pg_shader_ParticleRender]);\n" + ParticleRender_bindingString_cpp)

	UpdateBody.write("\n  glUseProgram(pg_shader_programme["+str(indConfiguration)+"][_pg_shader_Master]);\n" + Master_bindingString_cpp)
	UpdateBody.write("\n }\n")

	# manages the inclusions inside the shader files to generate the linearized shader files
	if(withParticleShaders[indConfiguration]) :
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

	if(withParticleShaders[indConfiguration]) :
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

	if(withParticleShaders[indConfiguration]) :
		ParticleAnimation_InputShader.close()
		ParticleAnimation_OutputShader.close()

	Update_InputShader.close()
	Update_OutputShader.close()
	Mixing_InputShader.close()
	Mixing_OutputShader.close()
	if(withParticleShaders[indConfiguration]) :
		ParticleRendering_InputShader.close() 
		ParticleRendering_OutputShader.close() 

	Master_InputShader.close() 
	Master_OutputShader.close() 

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global Nb_Configurations
	global Configuration_InputCsv_names
	global Scenario_InputCsv_names
	global FullScenario_InputCsv_name

	global scenario_vars_columns_list

	global ScriptHeader_name
	global ScriptHeader

	global ScriptBody_name
	global ScriptBody

	global ShaderHeader_name
	global ShaderHeader

	global ShaderBodyDecl_name
	global ShaderBodyDecl

	global ShaderBodyBind_name
	global ShaderBodyBind

	global UpdateBody_name
	global UpdateBody

	global TouchOSCScene_name
	global TouchOSCScene

	global scenario_var_target_shaders
	global withParticleShaders

	global full_scenario_vars_specs_dict
	global scenario_vars_specs_dict

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"n:c:f:s:",["nb_configurations=","configuration=", "full_scenario=", "scenario=", "particleAnimation_in=", "particleAnimation_out=", "Update_in=", "Update_out=", "Mixing_in=", "Mixing_out=", "ParticleSplat_in=", "ParticleSplat_out=", "Master_in=", "Master_out=", "script_header_out=", "script_body_out=", "shader_header_out=", "shader_body_decl_out=", "shader_body_bind_out=", "update_body_out="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:

		# CSV PARAMETER INPUT FILES
		if opt in ("-n", "--nb_configurations"):
			Nb_Configurations = int(arg)
		elif opt in ("-c", "--configuration"):
			Configuration_InputCsv_names = re.split(r",", arg)
		elif opt in ("-f", "--full_scenario"):
			FullScenario_InputCsv_name = arg
		elif opt in ("-s", "--scenario"):
			Scenario_InputCsv_names  = re.split(r",", arg)
		# C++ OUTPUT INCLUDED FILES
		elif opt in ("--script_header_out"):
			ScriptHeader_name = arg
		elif opt in ("--script_body_out"):
			ScriptBody_name = arg
		elif opt in ("--shader_header_out"):
			ShaderHeader_name = arg
		elif opt in ("--shader_body_decl_out"):
			ShaderBodyDecl_name = arg
		elif opt in ("--shader_body_bind_out"):
			ShaderBodyBind_name = arg
		elif opt in ("--update_body_out"):
			UpdateBody_name = arg
		else:
			print("Configuration generator: option (", opt, ")")
			assert False, "unhandled option"

		# GLSL SHADER INPUT AND OUTPUT FILES
		# elif opt in ("--Update_in"):
		# 	Update_InputShader_name = arg
		# elif opt in ("--Update_out"):
		# 	Update_OutputShader_name = arg
		# elif opt in ("--Mixing_in"):
		# 	Mixing_InputShader_name = arg
		# elif opt in ("--Mixing_out"):
		# 	Mixing_OutputShader_name = arg
		# elif opt in ("--Master_in"):
		# 	Master_InputShader_name = arg
		# elif opt in ("--Master_out"):
		# 	Master_OutputShader_name = arg
		# # GLSL OPTIONAL PARTICE SHADER INPUT AND OUTPUT FILES
		# elif opt in ("--particleAnimation_in"):
		# 	ParticleAnimation_InputShader_name = arg
		# elif opt in ("--particleAnimation_out"):
		# 	ParticleAnimation_OutputShader_name = arg
		# elif opt in ("--ParticleSplat_in"):
		# 	ParticleRendering_InputShader_name = arg
		# elif opt in ("--ParticleSplat_out"):
		# 	ParticleRendering_OutputShader_name = arg

	###############################################################################
	# C++ OUTPUT INCLUDED FILES
	try:
		ScriptHeader = open(ScriptHeader_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ScriptHeader_name, " or path is incorrect")
	try:
		ScriptBody = open(ScriptBody_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ScriptBody_name, " or path is incorrect")
	try:
		ShaderHeader = open(ShaderHeader_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ShaderHeader_name, " or path is incorrect")
	try:
		ShaderBodyDecl = open(ShaderBodyDecl_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ShaderBodyDecl_name, " or path is incorrect")
	try:
		ShaderBodyBind = open(ShaderBodyBind_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ShaderBodyBind_name, " or path is incorrect")
	try:
		UpdateBody = open(UpdateBody_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", UpdateBody_name, " or path is incorrect")
	try:
		TouchOSCScene = open(TouchOSCScene_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", TouchOSCScene_name, " or path is incorrect")

	###############################################################################
	# CSV PARAMETER INPUT FILES
	if(len(Configuration_InputCsv_names) != Nb_Configurations) :
		print("Configuration generator: Expected number of configuration files incorrect: ",len(Configuration_InputCsv_names)," instead of ", Nb_Configurations)
		sys.exit(0)
	if(len(Scenario_InputCsv_names) != Nb_Configurations) :
		print("Configuration generator: Expected number of scenario files incorrect: ",len(Scenario_InputCsv_names)," instead of ", Nb_Configurations)
		sys.exit(0)

	try:
		ShaderHeader = open(ShaderHeader_name, "wt")
	except IOError:
		print("Configuration generator: File not found:", ShaderHeader_name, " or path is incorrect")

	##################################################################
	# READS THE CSV HEADER FILE
	##################################################################
	# the first configuration file is used for variables
	# each configuration file is used for shader files
	# the headers of the same shader files across several 
	# configuration files can be different (they depend on each scenario)
	read_configuration_vars(Configuration_InputCsv_names[0])

	# all the configuration files are used to load the shader file names
	for configurationFileName, indConfig in zip(Configuration_InputCsv_names, range(Nb_Configurations)) :
		read_configuration_shader_names(configurationFileName, indConfig)

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
	##################################################################line_scene_header
	scenario_vars_columns_list = []
	for scenarioFileName, indConfig in zip(Scenario_InputCsv_names, range(Nb_Configurations)) :
		try:
			input_scenario_fileCsv = open(scenarioFileName, "rt")
		except IOError:
			print("Input scenario File (configuration: ", indConfig, ") not found:", scenarioFileName, " or path is incorrect")
		readScenarioCSV = csv.reader(input_scenario_fileCsv, delimiter=',')
		read_scenario_variables(readScenarioCSV, indConfig)
		read_scenario_scenes(readScenarioCSV, indConfig)
		input_scenario_fileCsv.close()
		# print(indConfig)
	post_reading_scenarios()

	#####################################################
	##### WRITES A TOUCHOSC FILE WITH ALL THE SCENES FOR ALL THE CONFIGURATIONS
	#####################################################
	WriteTouchOSCScene()

	#####################################################
	##### WRITES THE SCENARIO VARIABLES % CALLBACKS IN THE SCRIPT HEADER AND BODY FILES
	#####################################################
	write_script_header_and_body()

	###############################################################################
	# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
	###############################################################################
	withParticleShaders = [False] * Nb_Configurations
	for indConfig in range(Nb_Configurations) :
	 	write_binding_vars_header_and_body(indConfig)

	UpdateBody.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
