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

# 
Nb_Configurations = 0
nb_scenes = []
scene_names = [[]]

Configuration_InputCsv_names = []
Scenario_InputCsv_names = []

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

# scenario variables
scenario_var_ids = []
scenario_var_indConfig = []
scenario_var_types = []
scenario_var_callBacks = []
scenario_var_messageStrings = []

scenario_var_target_shaders = []
scenario_var_pulsing_modes = []
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
def ReadConfigurationVariables(configurationFileName) :
	global ScriptHeader

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

##################################################################
# READS THE SHADER FILE NAMES FROM THE CSV CONFIGURATION FILE 
##################################################################
def ReadConfigurationShaderNames(configurationFileName, indConfiguration) :
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
# READS THE CONFIGURATION VARIABLES FROM THE CSV CONFIGURATION FILE 
##################################################################
def ReadScenarioVariables(scenarioFileName, indConfiguration) :
	global scenario_var_ids
	global scenario_var_indConfig
	global scenario_var_types
	global scenario_var_callBacks
	global scenario_var_messageStrings

	global scenario_var_target_shaders
	global scenario_var_pulsing_modes
	global scenario_var_pulsing_modes_dict

	global ScriptHeader
	global Scenario_InputCsv_names
	global Nb_Configurations

	global nb_scenes
	global scene_names

	line_ranks = []
	line_verbatim = []

	try:
		Scenario_InputCsv = open(scenarioFileName, "rt")
	except IOError:
		print("Configuration generator: File not found:", scenarioFileName, " or path is incorrect")

	readCSV = csv.reader(Scenario_InputCsv, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 

	ind_scene = 1
	while (True) :
		if(line[0] == "RANK") :
			mode = SPECIFICATION
		elif(line[0] == "scenario") :
			mode = SCENARIO
		elif(line[0] == "scene") :
			mode = SCENE
		elif(line[0] == "initial_values") :
			mode = INITIALIZATION
		elif(line[0] == "/scenario") :
			# variable rank -> comment
			ScriptHeader.write("// SCENARIO VARIABLES #"+str(indConfiguration)+"\n")
			ScriptHeader.write("// " + space_char.join(map(str, line_ranks)) + "\n")
			# variable verbatim -> comment in the header file
			ScriptHeader.write("// " + space_char.join(map(str, line_verbatim)) + "\n")
			Scenario_InputCsv.close()
			return()
		else :
			print("Configuration generator: Unknown scenario mode [",line,"] !")
			sys.exit(0)

		if(mode == INITIALIZATION) :
			# opening tag
			line = next(readCSV)

			# initial values
			line = next(readCSV)

			# closing tag
			line = next(readCSV)
		
		elif(mode == SCENARIO) :
			nb_scenes.append(int(line[1]))
			scene_names.insert(indConfiguration, [])
			print("Configuration generator: scenes:", nb_scenes[indConfiguration])
			line = next(readCSV)
		
		elif(mode == SCENE) :
			# scene name and duration
			scene_names[indConfiguration].append(line[1])
			print("Configuration generator: scene", ind_scene, ":", scene_names[indConfiguration][ind_scene - 1])
			ind_scene += 1
			line = next(readCSV)

			# scene comments
			line = next(readCSV)

			# scene comments
			line = next(readCSV)

			# initial values
			line = next(readCSV)

			# final values
			line = next(readCSV)

			# interpolation commands
			line = next(readCSV)

			# final tag
			line = next(readCSV)
		
		elif(mode == SPECIFICATION) :
			# first scenario file
			if(indConfiguration == 0) :
				# variable rank -> comment
				line_ranks = line[1:]
				line = next(readCSV)

				# variable verbatim
				# -> nb of variables 
				nb_variables = len(line) - 1
				# print("Configuration generator: scenario #",0 ,"has", nb_variables, "variables")
				# variable verbatim -> comment in the header file
				line_verbatim = line[1:]
				line = next(readCSV)

				# variable types (used for declarations)
				scenario_var_types = line[1:]
				if(line[0] != "TYPE") :
					print("Configuration generator: TYPE line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				# variable strings (used for alias commands from PD 
				# and for variable % initial values declarations)
				scenario_var_ids = line[1:]
				if(line[0] != "ID") :
					print("Configuration generator: ID line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)
				for ident in scenario_var_ids:
					# initializes with Nb_Configurations False (variable in no scenario)
					scenario_var_indConfig.append([False] * Nb_Configurations,)
					# variable in first configuration
					scenario_var_indConfig[-1][0] = True

				# function callBack pointers for specific additional updates
				scenario_var_callBacks = line[1:]
				if(line[0] != "FUNCTION") :
					print("Configuration generator: FUNCTION line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				# string for message display on GUI
				scenario_var_messageStrings = line[1:]
				if(line[0] != "GUI") :
					print("Configuration generator: GUI line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				#######################################################################
				#  WRITES THE SCENARIO VARIABLES AS SHADER INPUTS 
				# destination shader for variables
				scenario_var_target_shaders = line[1:]
				if(line[0] != "SHADER") :
					print("Configuration generator: SHADER line expected not [",line[0],"]!")
					sys.exit(0)			
				line = next(readCSV)

				# pulsed variables declaration
				scenario_var_pulsing_modes = line[1:]
				if(line[0] != "PULSE") :
					print("Configuration generator: PULSE line expected not [",line[0],"]!")
					sys.exit(0)
				
				ind = 0
				for pulsing_mode in scenario_var_pulsing_modes :
					scenario_var_pulsing_modes_dict[scenario_var_ids[ind]] = pulsing_mode
					ind  += 1
				line = next(readCSV)
			# not first scenario file
			else :
				# variable rank -> comment
				new_line_ranks = line[1:]
				line = next(readCSV)

				# variable verbatim
				# -> nb of variables 
				new_nb_variables = len(line) - 1
				# print("Configuration generator: scenario #",str(indConfiguration) ,"has", new_nb_variables, "variables")
				# variable verbatim -> comment in the header file
				new_line_verbatim = line[1:]
				line = next(readCSV)

				# variable types (used for declarations)
				new_scenario_var_types = line[1:]
				if(line[0] != "TYPE") :
					print("Configuration generator: TYPE line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				# variable strings (used for alias commands from PD 
				# and for variable % initial values declarations)
				new_scenario_var_ids = line[1:]
				if(line[0] != "ID") :
					print("Configuration generator: ID line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				# function callBack pointers for specific additional updates
				new_scenario_var_callBacks = line[1:]
				if(line[0] != "FUNCTION") :
					print("Configuration generator: FUNCTION line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				# string for message display on GUI
				new_scenario_var_messageStrings = line[1:]
				if(line[0] != "GUI") :
					print("Configuration generator: GUI line expected not [",line[0],"]!")
					sys.exit(0)
				line = next(readCSV)

				#######################################################################
				#  WRITES THE SCENARIO VARIABLES AS SHADER INPUTS 
				# destination shader for variables
				new_scenario_var_target_shaders = line[1:]
				if(line[0] != "SHADER") :
					print("Configuration generator: SHADER line expected not [",line[0],"]!")
					sys.exit(0)			
				line = next(readCSV)

				# pulsed variables declaration
				new_scenario_var_pulsing_modes = line[1:]
				if(line[0] != "PULSE") :
					print("Configuration generator: PULSE line expected not [",line[0],"]!")
					sys.exit(0)
				
				#######################################################################
				# ONLY ADDS THE NEW VARIABLES WHICH WERE NOT IN THE PRECEDING SCENARIOS
				new_index = 0
				for var_id, var_type, var_calBacks, var_messageStrings, var_rank, var_verbatim, var_shaders, var_pulsing_mode in \
						zip(new_scenario_var_ids, new_scenario_var_types, new_scenario_var_callBacks, new_scenario_var_messageStrings, \
							new_line_ranks, new_line_verbatim, new_scenario_var_target_shaders, new_scenario_var_pulsing_modes) :
					if(var_id in scenario_var_ids) :
						# variable is already known, checks that it is coherent with what is already stored
						index_in_mem = scenario_var_ids.index(var_id)
						# print(scenario_var_ids)
						# print(scenario_var_pulsing_modes)
						# print("Configuration generator: scenario_var_ids",len(scenario_var_ids))
						# print("Configuration generator: scenario_var_pulsing_modes",len(scenario_var_pulsing_modes))
						# print("Configuration generator: var_id",var_id,"index", index_in_mem,"vs new index",new_index)
						if(var_type != scenario_var_types[index_in_mem]) :
							print("Configuration generator: var",var_id,"has inconsistent types in scenario",Scenario_InputCsv_names[indConfiguration],\
								"index",new_index,"(",var_type,") and in scenario",Scenario_InputCsv_names[scenario_var_indConfig[index_in_mem].index(True)],"index",index_in_mem,"(",scenario_var_types[index_in_mem],")!")
							sys.exit(0)
						if(var_calBacks != scenario_var_callBacks[index_in_mem]) :
							print("Configuration generator: var",var_id,"has inconsistent callBacks in scenario",Scenario_InputCsv_names[indConfiguration],\
								"index",new_index,"(",var_calBacks,") and in scenario",Scenario_InputCsv_names[scenario_var_indConfig[index_in_mem].index(True)],"index",index_in_mem,"(",scenario_var_callBacks[index_in_mem],")!")
							sys.exit(0)
						if(var_messageStrings != scenario_var_messageStrings[index_in_mem]) :
							print("Configuration generator: var",var_id,"has inconsistent types in scenario",Scenario_InputCsv_names[indConfiguration],\
								"index",new_index,"(",var_messageStrings,") and in scenario",Scenario_InputCsv_names[scenario_var_indConfig[index_in_mem].index(True)],"index",index_in_mem,"(",scenario_var_messageStrings[index_in_mem],")!")
							sys.exit(0)
						if(var_shaders != scenario_var_target_shaders[index_in_mem]) :
							print("Configuration generator: var",var_id,"has inconsistent target shaders in scenario",Scenario_InputCsv_names[indConfiguration],\
								"index",new_index,"(",var_shaders,") and in scenario",Scenario_InputCsv_names[scenario_var_indConfig[index_in_mem].index(True)],"index",index_in_mem,"(",scenario_var_target_shaders[index_in_mem],")!")
							sys.exit(0)
						if(var_pulsing_mode != scenario_var_pulsing_modes[index_in_mem]) :
							print("Configuration generator: var",var_id,"has inconsistent pulsing modes in scenario",Scenario_InputCsv_names[indConfiguration],\
								"index",new_index,"(",var_pulsing_mode,") and in scenario",Scenario_InputCsv_names[scenario_var_indConfig[index_in_mem].index(True)],"index",index_in_mem,"(",scenario_var_pulsing_modes[index_in_mem],")!")
							sys.exit(0)

						# variable in indConfiguration scenario
						scenario_var_indConfig[index_in_mem][indConfiguration] = True
					else :
						# new variable 
						scenario_var_ids.append(var_id)
						# initializes with Nb_Configurations False (variable in no scenario)
						scenario_var_indConfig.append([False] * Nb_Configurations, )
						# variable in indConfiguration scenario
						scenario_var_indConfig[-1][indConfiguration] = True
						scenario_var_types.append(var_type)
						scenario_var_callBacks.append(var_calBacks)
						scenario_var_messageStrings.append(var_messageStrings)
						line_ranks.append(var_verbatim)
						line_verbatim.append(var_verbatim)
						scenario_var_target_shaders.append(var_shaders)
						scenario_var_pulsing_modes.append(var_pulsing_mode)
						scenario_var_pulsing_modes_dict[var_id] = var_pulsing_mode

					new_index = new_index + 1

				# jumps to next line
				line = next(readCSV)

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
def ScriptHeaderAndBodyWrite() :
	global ScriptHeader
	global ScriptBody
	global ShaderBodyDecl

	global scenario_var_ids
	global scenario_var_indConfig
	global scenario_var_types
	global scenario_var_callBacks
	global scenario_var_messageStrings

	global scenario_var_pulsing_modes
	global Nb_Configurations

	############################### constant declarations
	# number of scene variables
	for id_string in scenario_var_ids :
		ScriptHeader.write("#define var_%s\n" % id_string)
	ScriptHeader.write("enum InterpVarIDs{ \n")
	# scene variable initialization
	ScriptHeader.write("  _%s = 0,\n" % scenario_var_ids[0])
	for id_string in scenario_var_ids[1:] :
		ScriptHeader.write("  _%s,\n" % id_string)
		# print("Configuration generator:   _%s,", scenario_var_ids
	ScriptHeader.write("  _MaxInterpVarIDs};\n")
	ScriptHeader.write("#define _NbConfigurations %d\n" % Nb_Configurations)

	############################### scenario variable declarations
	# interpolation cancelation variable declaration
	# ScriptBody.write("bool BrokenInterpolationVar[ _MaxInterpVarIDs ]"
	# scenario variable declarations
	ScriptBody.write("// SCENARIO VARIABLES\n")
	for id_string, type_string in zip(scenario_var_ids, scenario_var_types):
		if(type_string == "sign") :
			type_string = "float"			
		elif(type_string == "path") :
			type_string = "bool"
		ScriptBody.write("%-5s %-20s;\n" % (type_string , id_string))
	ScriptBody.write("bool  ScenarioVarConfigurations[_MaxInterpVarIDs][%d] = {\n" % Nb_Configurations)
	for configurations in scenario_var_indConfig:
		ScriptBody.write("  {")
		for boolConf in configurations :
			if(boolConf) :
				ScriptBody.write("true, ")
			else :
				ScriptBody.write("false, ")
		ScriptBody.write("},\n")
	ScriptBody.write("};\n")

	# extern scenario variable declaration in the header file
	for id_string, type_string in zip(scenario_var_ids, scenario_var_types):
		if(type_string == "sign") :
			type_string = "float"			
		elif(type_string == "path") :
			type_string = "bool"
		ScriptHeader.write("extern %-5s %-20s;\n" % (type_string , id_string))
	ScriptHeader.write("extern bool  ScenarioVarConfigurations[_MaxInterpVarIDs][%d];\n" % Nb_Configurations)

	# extern scenario variable types, pointers and messages declaration in the header file
	ScriptHeader.write("enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path , _pg_string };\n")
	ScriptHeader.write("enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };\n")
	ScriptHeader.write("extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern void * ScenarioVarPointers[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *ScenarioVarMessages[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *ScenarioVarStrings[_MaxInterpVarIDs];\n")

	# scenario variable types declarations
	ScriptBody.write("VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { \n")
	for type_string in scenario_var_types :
		ScriptBody.write("	_pg_%s,\n" % type_string)
	ScriptBody.write("};\n")

	# scenario variable pointers declarations
	ScriptBody.write("void * ScenarioVarPointers[_MaxInterpVarIDs] = { \n")
	for id_string in scenario_var_ids :
		ScriptBody.write("	(void *)&%s,\n" % id_string)
	ScriptBody.write("};\n")

	# ScriptBody.write("union scenarioValue {\n")
	# ScriptBody.write("	double val_num;\n")
	# ScriptBody.write("	string val_string;\n")
	# ScriptBody.write("~ScenarioValue() {}};\n")

	# variable callback pointers declarations
	for callBack_id_string, type_string in zip(scenario_var_callBacks, scenario_var_types):
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
			else :
				print("Configuration generator: Unknown callback ",callBack_id_string," parameter type [",type_string,"]")
				sys.exit(0)
			ScriptBody.write("}\n")

	ScriptBody.write("void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue) = { \n")
	for callBack_id_string in scenario_var_callBacks :
		callBack_generic_id_string = callBack_id_string + "_generic"
		if(callBack_id_string == "NULL") :
			ScriptBody.write("	NULL,\n")
		else :
			ScriptBody.write("	&%s,\n" % callBack_generic_id_string)
	ScriptBody.write("};\n")

	ScriptBody.write("char *ScenarioVarMessages[_MaxInterpVarIDs] = { \n")
	for id_string in scenario_var_messageStrings :
		if(id_string == "NULL") :
			ScriptBody.write("  (char *)\"\",\n")
		else :
			ScriptBody.write("  (char *)\""+id_string+"\",\n")
	ScriptBody.write("};\n")

	# ScriptHeader.write("enum PulseTypes : _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none "
	ScriptBody.write("PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = { ")
	for pulsing_mode in scenario_var_pulsing_modes :
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
	for idstring in scenario_var_ids :
		ScriptBody.write("  (char *)\""+idstring+"\",\n")
	ScriptBody.write("};\n")

	ShaderBodyDecl.write("GLint uniform_ParticleAnimation_scenario_var_data[%d] = {NULL};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Update_scenario_var_data[%d] = {NULL};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Mixing_scenario_var_data[%d] = {NULL};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_ParticleRender_scenario_var_data[%d] = {NULL};\n" % Nb_Configurations)
	ShaderBodyDecl.write("GLint uniform_Master_scenario_var_data[%d] = {NULL};\n" % Nb_Configurations)
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
def ShaderHeaderBodyAndBindingVariables(indConfiguration) :
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

	global scenario_var_indConfig

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

	ParticleRender_fs_IDs = dict()
	Master_fs_IDs = dict()
	ParticleAnimation_fs_IDs = dict()
	Update_fs_IDs = dict()
	Mixing_fs_IDs = dict()

	ParticleRender_fs_types = dict()
	Master_fs_types = dict()
	ParticleAnimation_fs_types = dict()
	Update_fs_types = dict()
	Mixing_fs_types = dict()

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
	for target_shader_list, id_string, type_string, configurations in zip(scenario_var_target_shaders,scenario_var_ids,scenario_var_types,scenario_var_indConfig) : 
		if(target_shader_list != "*" and configurations[indConfiguration] == True) :
			# "*" nothing is done, the variable is not a shader parameter or the variable is not in the scenario of this configuration
			if(withParticleShaders[indConfiguration]) :
				if(re.search(r'ParticleAnimation_fs', target_shader_list) != None) :
					# ParticleAnimation_fs parameter
					ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "#define var_" + id_string + "\n"
					ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] = id_string
					ParticleAnimation_fs_types[ParticleAnimation_fs_index] = type_string
					if(type_string == "bool" or type_string == "path") :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "bool	  " + id_string + ";\n"
					elif(type_string == "int") :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "int		" + id_string + ";\n"
					elif(type_string == "float") :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "float	 " + id_string + ";\n"
					else :
						print("Configuration generator: Unknown particle animation shader parameter type [%s]" % type_string)
						sys.exit(0)
					
					if(ParticleAnimation_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleAnimation_scenario_var_data[%d];\n" % Nb_Configurations)
						ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_ParticleAnimation_scenario_var_data, \"uniform_ParticleAnimation_scenario_var_data\", _pg_shader_ParticleAnimation);\n")
					
					ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "  ParticleAnimation_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleAnimation_fs_index) + "] = "

					if(ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "bool" or ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "path") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
					elif(ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "int") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n"
					elif(ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "float") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
					else :
						print("Configuration generator: Unknown particle animation shader parameter type [%s]" % ParticleAnimation_fs_types[ParticleAnimation_fs_index])
						sys.exit(0)
					
					pulsing_mode = scenario_var_pulsing_modes_dict[ParticleAnimation_fs_IDs[ParticleAnimation_fs_index]]
					if(pulsing_mode == "*") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " * (1.f + pulse_average * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " + pulse_average * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " * (pulse_average - pulse_average_prec) * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse;\n"
					else :
						print("Configuration generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				 	
					ParticleAnimation_fs_index  += 1
			
			if(re.search(r'Update_fs', target_shader_list) != None) :
				# Update_fs parameter
				Update_head_glsl = Update_head_glsl + "#define var_" + id_string + "\n"
				Update_fs_IDs[Update_fs_index] = id_string
				Update_fs_types[Update_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Update_head_glsl = Update_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Update_head_glsl = Update_head_glsl + "int		" + id_string + ";\n"
				elif(type_string == "float") :
					Update_head_glsl = Update_head_glsl + "float	 " + id_string + ";\n"
				else :
					print("Configuration generator: Unknown update shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Update_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Update_scenario_var_data[%d];\n" % Nb_Configurations);
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Update_scenario_var_data, \"uniform_Update_scenario_var_data\", _pg_shader_Update);\n")

				Update_bindingString_cpp = Update_bindingString_cpp + "  Update_scenario_var_data["+str(indConfiguration)+"][" + str(Update_fs_index) + "] = "
				if(Update_fs_types[Update_fs_index] == "bool" or Update_fs_types[Update_fs_index] == "path") :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
				elif(Update_fs_types[Update_fs_index] == "int") :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n"
				elif(Update_fs_types[Update_fs_index] == "float") :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
				else :
					print("Configuration generator: Unknown update shader parameter type [%s]" % Update_fs_types[Update_fs_index])
					sys.exit(0)
				
				pulsing_mode = scenario_var_pulsing_modes_dict[Update_fs_IDs[Update_fs_index]]
				if(pulsing_mode == "*") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " * (1.f + pulse_average * " + Update_fs_IDs[Update_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " + pulse_average * " + Update_fs_IDs[Update_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " * (pulse_average - pulse_average_prec) * " + Update_fs_IDs[Update_fs_index] + "_pulse;\n"
				else :
					print("Configuration generator: Unknown Update pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
				
				Update_fs_index += 1
			
			if(re.search(r'Mixing_fs', target_shader_list) != None) :
				# Mixing_fs parameter
				Mixing_head_glsl = Mixing_head_glsl + "#define var_" + id_string + "\n"
				Mixing_fs_IDs[Mixing_fs_index] = id_string
				Mixing_fs_types[Mixing_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Mixing_head_glsl = Mixing_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Mixing_head_glsl = Mixing_head_glsl + "int		" + id_string + ";\n"
				elif(type_string == "float") :
					Mixing_head_glsl = Mixing_head_glsl + "float	 " + id_string + ";\n"
				else :
					print("Configuration generator: Unknown mixing shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Mixing_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Mixing_scenario_var_data[%d];\n" % Nb_Configurations)
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Mixing_scenario_var_data, \"uniform_Mixing_scenario_var_data\", _pg_shader_Mixing);\n")
				
				Mixing_bindingString_cpp = Mixing_bindingString_cpp + "  Mixing_scenario_var_data["+str(indConfiguration)+"][" + str(Mixing_fs_index) + "] = "
				if(Mixing_fs_types[Mixing_fs_index] == "bool" or Mixing_fs_types[Mixing_fs_index] == "path") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
				elif(Mixing_fs_types[Mixing_fs_index] == "int") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n"
				elif(Mixing_fs_types[Mixing_fs_index] == "float") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
				else :
					print("Configuration generator: Unknown mixing shader parameter type [%s]" % Mixing_fs_types[Mixing_fs_index])
					sys.exit(0)
				
				pulsing_mode = scenario_var_pulsing_modes_dict[Mixing_fs_IDs[Mixing_fs_index]]
				if(pulsing_mode == "*") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + " * (1.f + pulse_average * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + " + pulse_average * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Mixing_fs_index] + " * (pulse_average - pulse_average_prec) * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse;\n"
				else :
					print("Configuration generator: Unknown Mixing pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
			 	
				Mixing_fs_index += 1
			

			if(withParticleShaders[indConfiguration]) :
				if(re.search(r'ParticleRender_fs', target_shader_list) != None) :
					# ParticleRender_fs parameter
					ParticleRender_head_glsl = ParticleRender_head_glsl + "#define var_" + id_string + "\n"
					ParticleRender_fs_IDs[ParticleRender_fs_index] = id_string
					ParticleRender_fs_types[ParticleRender_fs_index] = type_string
					if(type_string == "bool" or type_string == "path") :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "bool	  " + id_string + ";\n"
					elif(type_string == "int") :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "int		" + id_string + ";\n"
					elif(type_string == "float") :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "float	 " + id_string + ";\n"
					else :
						print("Configuration generator: Unknown particle shader parameter type [%s]" % type_string)
						sys.exit(0)

					if(ParticleRender_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleRender_scenario_var_data[%d];\n" % Nb_Configurations)
						ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_ParticleRender_scenario_var_data, \"uniform_ParticleRender_scenario_var_data\", _pg_shader_Particle);\n")
					

					ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + " ParticleRender_scenario_var_data["+str(indConfiguration)+"][" + str(ParticleRender_fs_index) + "] = "
					if(ParticleRender_fs_types[ParticleRender_fs_index] == "bool" or ParticleRender_fs_types[ParticleRender_fs_index] == "path") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
					elif(ParticleRender_fs_types[ParticleRender_fs_index] == "int") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n"
					elif(ParticleRender_fs_types[ParticleRender_fs_index] == "float") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
					else :
						print("Configuration generator: Unknown particle shader parameter type [%s]" % ParticleRender_fs_types[ParticleRender_fs_index])
						sys.exit(0)
					
					pulsing_mode = scenario_var_pulsing_modes_dict[ParticleRender_fs_IDs[ParticleRender_fs_index]]
					if(pulsing_mode == "*") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " * (1.f + pulse_average * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " + pulse_average * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " * (pulse_average - pulse_average_prec) * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse;\n"
					else :
						print("Configuration generator: Unknown ParticleRender pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				 	
					ParticleRender_fs_index += 1
			
			if(re.search(r'Master_fs', target_shader_list) != None) :
				# Master_fs parameter
				Master_head_glsl = Master_head_glsl + "#define var_" + id_string + "\n"
				Master_fs_IDs[Master_fs_index] = id_string
				Master_fs_types[Master_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Master_head_glsl = Master_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Master_head_glsl = Master_head_glsl + "int		" + id_string + ";\n"
				elif(type_string == "float") :
					Master_head_glsl = Master_head_glsl + "float	 " + id_string + ";\n"
				else :
					print("Configuration generator: Unknown master shader parameter type [%s]" % type_string)
					sys.exit(0)

				if(Master_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Master_scenario_var_data[%d];\n" % Nb_Configurations)
					ShaderBodyBind.write("	pg_allocateBindAndCheckUniform("+str(indConfiguration)+",  uniform_Master_scenario_var_data, \"uniform_Master_scenario_var_data\", _pg_shader_Master);\n")
				
				Master_bindingString_cpp = Master_bindingString_cpp + "  Master_scenario_var_data["+str(indConfiguration)+"][" + str(Master_fs_index) + "] = "
				if(Master_fs_types[Master_fs_index] == "bool" or Master_fs_types[Master_fs_index] == "path") :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
				elif(Master_fs_types[Master_fs_index] == "int") :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = int(uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n"
				elif(Master_fs_types[Master_fs_index] == "float") :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
				else :
					print("Configuration generator: Unknown master shader parameter type [%s]" % Master_fs_types[Master_fs_index])
					sys.exit(0)
				
				pulsing_mode = scenario_var_pulsing_modes_dict[Master_fs_IDs[Master_fs_index]]
				if(pulsing_mode == "*") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " * (1.f + pulse_average * " + Master_fs_IDs[Master_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " + pulse_average * " + Master_fs_IDs[Master_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " * (pulse_average - pulse_average_prec) * " + Master_fs_IDs[Master_fs_index] + "_pulse;\n"
				else :
					print("Configuration generator: Unknown Master pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
				
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

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"n:c:s:",["nb_configurations=","configuration=", "scenario=", "particleAnimation_in=", "particleAnimation_out=", "Update_in=", "Update_out=", "Mixing_in=", "Mixing_out=", "ParticleSplat_in=", "ParticleSplat_out=", "Master_in=", "Master_out=", "script_header_out=", "script_body_out=", "shader_header_out=", "shader_body_decl_out=", "shader_body_bind_out=", "update_body_out="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:

		# CSV PARAMETER INPUT FILES
		if opt in ("-n", "--nb_configurations"):
			Nb_Configurations = int(arg)
		elif opt in ("-c", "--configuration"):
			Configuration_InputCsv_names = re.split(r",", arg)
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
	ReadConfigurationVariables(Configuration_InputCsv_names[0])

	# all the configuration files are used to load the shader file names
	for configurationFileName, indConfig in zip(Configuration_InputCsv_names, range(Nb_Configurations)) :
		ReadConfigurationShaderNames(configurationFileName, indConfig)

	##################################################################
	# READS THE CSV SCENARIO FILE
	##################################################################
	for scenarioFileName, indConfig in zip(Scenario_InputCsv_names, range(Nb_Configurations)) :
	 	ReadScenarioVariables(scenarioFileName, indConfig)
	 	# print(scenario_var_indConfig)
	 	# print(indConfig)

	#####################################################
	##### WRITES A TOUCHOSC FILE WITH ALL THE SCENES FOR ALL THE CONFIGURATIONS
	#####################################################
	WriteTouchOSCScene()

	#####################################################
	##### WRITES THE SCENARIO VARIABLES % CALLBACKS IN THE SCRIPT HEADER AND BODY FILES
	#####################################################
	ScriptHeaderAndBodyWrite()

	###############################################################################
	# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
	###############################################################################
	withParticleShaders = [False] * Nb_Configurations
	for indConfig in range(Nb_Configurations) :
	 	ShaderHeaderBodyAndBindingVariables(indConfig)

	UpdateBody.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
