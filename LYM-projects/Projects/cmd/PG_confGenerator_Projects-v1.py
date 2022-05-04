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
from signal import signal, SIGINT

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

space_char = ','

# command to regenerate cpp files + shaders: to be called after each change of variables in the scenario

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''	print("Usage: python PG_confGenerator_song-v1.py song/configuration/LYM_song-head-single.csv song/scenarios/LYM_song-scenario-v1.csv song/shaders/LYM_ParticleAnimation_song.frag song/shaders/LYM_ParticleAnimation_song_full.frag song/shaders/LYM_Update_song.frag song/shaders/LYM_Update_song_full.frag song/shaders/LYM_Mixing_song.frag song/shaders/LYM_Mixing_song_full.frag song/shaders/LYM_ParticleRender_song.frag song/shaders/LYM_ParticleRender_song_full.frag song/shaders/LYM_Master_song.frag song/shaders/LYM_Master_song_full.frag ../Porphyrograph/Porphyrograph-song-src/pg_script_header.h ../Porphyrograph/Porphyrograph-song-src/pg_script_body.cpp  ../Porphyrograph/Porphyrograph-song-src/pg_shader_header.h ../Porphyrograph/Porphyrograph-song-src/pg_shader_body_decl.cpp ../Porphyrograph/Porphyrograph-song-src/pg_shader_body_bind.cpp ../Porphyrograph/Porphyrograph-song-src/pg_update_body.cpp")
PG_confGenerator.py: A program for generating shader file & C++ code for Porphyrograph")

'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	Configuration_InputCsv_name = ""
	Scenario_InputCsv_name = ""
	ParticleAnimation_InputShader_name = ""
	ParticleAnimation_OutputShader_name = ""
	Update_InputShader_name = ""
	Update_OutputShader_name = ""
	Mixing_InputShader_name = ""
	Mixing_OutputShader_name = ""
	ParticleRendering_InputShader_name = ""
	ParticleRendering_OutputShader_name = ""
	Master_InputShader_name = ""
	Master_OutputShader_name = ""
	ScriptHeader_name = ""
	ScriptBody_name = ""
	ShaderHeader_name = ""
	ShaderBodyDecl_name = ""
	ShaderBodyBind_name = ""
	UpdateBody_name = ""

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"c:s:",["configuration=", "scenario=", "particleAnimation_in=", "particleAnimation_out=", "Update_in=", "Update_out=", "Mixing_in=", "Mixing_out=", "ParticleSplat_in=", "ParticleSplat_out=", "Master_in=", "Master_out=", "script_header_out=", "script_body_out=", "shader_header_out=", "shader_body_decl_out=", "shader_body_bind_out=", "update_body_out="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:

		# CSV PARAMETER INPUT FILES
		if opt in ("-c", "--configuration"):
			Configuration_InputCsv_name = arg
		elif opt in ("-s", "--scenario"):
			Scenario_InputCsv_name = arg

		# GLSL SHADER INPUT AND OUTPUT FILES
		elif opt in ("--Update_in"):
			Update_InputShader_name = arg
		elif opt in ("--Update_out"):
			Update_OutputShader_name = arg
		elif opt in ("--Mixing_in"):
			Mixing_InputShader_name = arg
		elif opt in ("--Mixing_out"):
			Mixing_OutputShader_name = arg
		elif opt in ("--Master_in"):
			Master_InputShader_name = arg
		elif opt in ("--Master_out"):
			Master_OutputShader_name = arg
		# GLSL OPTIONAL PARTICE SHADER INPUT AND OUTPUT FILES
		elif opt in ("--particleAnimation_in"):
			ParticleAnimation_InputShader_name = arg
		elif opt in ("--particleAnimation_out"):
			ParticleAnimation_OutputShader_name = arg
		elif opt in ("--ParticleSplat_in"):
			ParticleRendering_InputShader_name = arg
		elif opt in ("--ParticleSplat_out"):
			ParticleRendering_OutputShader_name = arg

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
			print("option (", opt, ")")
			assert False, "unhandled option"
	# print('Input scenario file is ', scenario_file_name)

	###############################################################################
	# CSV PARAMETER INPUT FILES
	try:
		Configuration_InputCsv = open(Configuration_InputCsv_name, "rt")
	except IOError:
		print("File not found:", Configuration_InputCsv_name, " or path is incorrect")
	try:
		Scenario_InputCsv = open(Scenario_InputCsv_name, "rt")
	except IOError:
		print("File not found:", Scenario_InputCsv_name, " or path is incorrect")

	###############################################################################
	# GLSL SHADER INPUT AND OUTPUT FILES
	try:
		Update_InputShader = open(Update_InputShader_name, "rt")
	except IOError:
		print("File not found:", Update_InputShader_name, " or path is incorrect")
	try:
		Update_OutputShader = open(Update_OutputShader_name, "wt")
	except IOError:
		print("File not found:", Update_OutputShader_name, " or path is incorrect")
	try:
		Mixing_InputShader = open(Mixing_InputShader_name, "rt")
	except IOError:
		print("File not found:", Mixing_InputShader_name, " or path is incorrect")
	try:
		Mixing_OutputShader = open(Mixing_OutputShader_name, "wt")
	except IOError:
		print("File not found:", Mixing_OutputShader_name, " or path is incorrect")
	try:
		Master_InputShader = open(Master_InputShader_name, "rt")
	except IOError:
		print("File not found:", Master_InputShader_name, " or path is incorrect")
	try:
		Master_OutputShader = open(Master_OutputShader_name, "wt")
	except IOError:
		print("File not found:", Master_OutputShader_name, " or path is incorrect")

	# GLSL OPTIONAL PARTICLE SHADER INPUT AND OUTPUT FILES
	if(ParticleAnimation_InputShader_name != "") :
		withParticleShaders = 1
		try:
			ParticleAnimation_InputShader = open(ParticleAnimation_InputShader_name, "rt")
		except IOError:
			print("File not found:", ParticleAnimation_InputShader_name, " or path is incorrect")
		try:
			ParticleAnimation_OutputShader = open(ParticleAnimation_OutputShader_name, "wt")
		except IOError:
			print("File not found:", ParticleAnimation_OutputShader_name, " or path is incorrect")
		try:
			ParticleRendering_InputShader = open(ParticleRendering_InputShader_name, "rt")
		except IOError:
			print("File not found:", ParticleRendering_InputShader_name, " or path is incorrect")
		try:
			ParticleRendering_OutputShader = open(ParticleRendering_OutputShader_name, "wt")
		except IOError:
			print("File not found:", ParticleRendering_OutputShader_name, " or path is incorrect")
	else:
		withParticleShaders = 0

	###############################################################################
	# C++ OUTPUT INCLUDED FILES
	try:
		ScriptHeader = open(ScriptHeader_name, "wt")
	except IOError:
		print("File not found:", ScriptHeader_name, " or path is incorrect")
	try:
		ScriptBody = open(ScriptBody_name, "wt")
	except IOError:
		print("File not found:", ScriptBody_name, " or path is incorrect")
	try:
		ShaderHeader = open(ShaderHeader_name, "wt")
	except IOError:
		print("File not found:", ShaderHeader_name, " or path is incorrect")
	try:
		ShaderBodyDecl = open(ShaderBodyDecl_name, "wt")
	except IOError:
		print("File not found:", ShaderBodyDecl_name, " or path is incorrect")
	try:
		ShaderBodyBind = open(ShaderBodyBind_name, "wt")
	except IOError:
		print("File not found:", ShaderBodyBind_name, " or path is incorrect")
	try:
		UpdateBody = open(UpdateBody_name, "wt")
	except IOError:
		print("File not found:", UpdateBody_name, " or path is incorrect")

	##################################################################
	# READS THE CSV CONFIGURATION FILE LINE BY LINE
	##################################################################
	readCSV = csv.reader(Configuration_InputCsv, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 

	mode = COMMENT
	value = 0
	while (True) :
		if(line[0] == "RANK") :
			mode = SPECIFICATION
		elif(line[0] == "rendering_files") :
			mode = RENDERING_FILES
		elif(line[0] == "initial_values") :
			mode = INITIALIZATION
		elif(line[0] == "udp_local_server") :
			mode = UDP_LOCAL_SERVER
		elif(line[0] == "udp_remote_client") :
			mode = UDP_REMOTE_CLIENT
		elif(line[0] == "/head") :
			print("")
			break
		else :
			print("Unknown head mode [",line,"] !")
			sys.exit(0)

		if(mode == INITIALIZATION) :
			# initial values
			line = next(readCSV)

			#closing tag
			line = next(readCSV)

			#next line
			line = next(readCSV)
		
		elif(mode == RENDERING_FILES) :
			line = next(readCSV)

			# variable types
			line = next(readCSV)

			# variable IDs
			line = next(readCSV)

			while(1) :
				if(line[0] == "VIDEO" or line[0] == "SVG" 
					or line[0] == "PNG" or line[0] == "JPG") :
					line = next(readCSV)
				else :
					break

			# closing tag
			line = next(readCSV)
		
		elif(mode == UDP_LOCAL_SERVER) :
			nb_servers = int(line[1])
			print("servers:", nb_servers, end =", ")
			line = next(readCSV)

			# verbatim
			line = next(readCSV)

			# variable types
			line = next(readCSV)

			# variable IDs
			line = next(readCSV)

			for ind in range(nb_servers) :
				if(line[0] == "server") :
					line = next(readCSV)
				else :
					print("server line expected!", line)
					sys.exit(0)
			
			# closing tag
			line = next(readCSV)
		
		elif(mode == UDP_REMOTE_CLIENT) :
			nb_clients = int(line[1])
			print("clients:", nb_clients, end =", ")
			line = next(readCSV)

			# verbatim
			line = next(readCSV)

			# variable types
			line = next(readCSV)

			# variable IDs
			line = next(readCSV)

			for ind in range(nb_clients) :
				if(line[0] == "client") :
					line = next(readCSV)
				else :
					print("client line expected!", line)
					sys.exit(0)
				
			# closing tag
			line = next(readCSV)
			print("")
			break
		
		elif(mode == SPECIFICATION) :
			# variable rank -> comment
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable verbatim
			# -> nb of variables 
			nb_variables = len(line) - 1
			print("configuration variables:", int(nb_variables), end =", ")
			# -> comment in the header file
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable types (used for declarations)
			if(line[0] != "TYPE") :
				print("TYPE line expected not [",line[0],"]!")
				sys.exit(0)
			config_types = line[1:]
			line = next(readCSV)

			# variable strings (used for alias commands from PD 
			# and for variable & initial values declarations)
			if(line[0] != "ID") :
				print("ID line expected not [",line[0],"]!")
				sys.exit(0)			
			config_ids = line[1:]
			line = next(readCSV)
			
			############################### constant declarations
			# number of scene variables
			ScriptHeader.write("enum ConfigurationVarIDs {\n")
			# scene variable initialization
			ScriptHeader.write("  _%s = 0," % config_ids[0] + "\n")
			for id_var in config_ids[1:] :
				ScriptHeader.write("  _%s," % id_var + "\n")
				# print("  _%s,", config_ids
			
			ScriptHeader.write("  _MaxConfigurationVarIDs};\n")

			############################### configuration variable declarations
			# extern configuration variable declaration in the header file
			for config_id, config_type in zip(config_ids, config_types):
				if(config_type == "sign") :
						config_type = "float"
				elif(config_type == "path") :
						config_type = "bool"
				ScriptHeader.write("extern %-5s %-20s;\n" % (config_type , config_id))
			
			# configuration variable declaration
			for config_id, config_type in zip(config_ids, config_types):
				if(config_type == "sign") :
						config_type = "float"					
				elif(config_type == "path") :
						config_type = "bool"
				ScriptBody.write("%-5s %-20s;\n" % (config_type , config_id))

			############################### configuration variable declarations
			# extern configuration variable types % pointers declarations
			ScriptHeader.write("enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path };\n")
			ScriptHeader.write("enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };\n")
			ScriptHeader.write("extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];\n")
			ScriptHeader.write("extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];\n")

			# scenario variable types declarations
			ScriptBody.write("VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs] = { \n")
			for config_type in config_types :
				ScriptBody.write("	_pg_%s,\n" % config_type)
			ScriptBody.write("};\n")

			# scenario variable pointers declarations
			ScriptBody.write("void * ConfigurationVarPointers[_MaxConfigurationVarIDs] = { \n")
			for config_id in config_ids :
				ScriptBody.write("	(void *)&%s,\n" % config_id)
			ScriptBody.write("};\n")

	Configuration_InputCsv.close()

	ids = []
	types = []
	callBacks = []
	messageStrings = []
	ids = []
	target_shaders = []
	pulsing_modes = []
	pulsing_modes_dict = dict()

	##################################################################
	# READS THE CSV SCENARIO FILE LINE BY LINE
	##################################################################
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
			break
		else :
			print("Unknown scenario mode [",line,"] !")
			sys.exit(0)

		if(mode == INITIALIZATION) :
			# opening tag
			line = next(readCSV)

			# initial values
			line = next(readCSV)

			# closing tag
			line = next(readCSV)
		
		elif(mode == SCENARIO) :
			nb_scenes = line[1]
			print("scenes:", nb_scenes)
			line = next(readCSV)
		
		elif(mode == SCENE) :
			# scene name and duration
			print("scene", ind_scene, ":", line[1])
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
			# variable rank -> comment
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable verbatim
			# -> nb of variables 
			nb_variables = len(line) - 1
			print("scenario variables:", nb_variables, end =", ")
			# -> comment in the header file
			ScriptHeader.write("// " + space_char.join(map(str, line)) + "\n")
			line = next(readCSV)

			# variable types (used for declarations)
			types = line[1:]
			if(line[0] != "TYPE") :
				print("TYPE line expected not [",line[0],"]!")
				sys.exit(0)
			line = next(readCSV)

			# variable strings (used for alias commands from PD 
			# and for variable % initial values declarations)
			ids = line[1:]
			if(line[0] != "ID") :
				print("ID line expected not [",line[0],"]!")
				sys.exit(0)
			line = next(readCSV)

			# function callBack pointers for specific additional updates
			callBacks = line[1:]
			if(line[0] != "FUNCTION") :
				print("FUNCTION line expected not [",line[0],"]!")
				sys.exit(0)
			line = next(readCSV)

			# string for message display on GUI
			messageStrings = line[1:]
			if(line[0] != "GUI") :
				print("GUI line expected not [",line[0],"]!")
				sys.exit(0)
			line = next(readCSV)

			#######################################################################
			#  WRITES THE SCENARIO VARIABLES AS SHADER INPUTS 
			# destination shader for variables
			target_shaders = line[1:]
			if(line[0] != "SHADER") :
				print("SHADER line expected not [",line[0],"]!")
				sys.exit(0)			
			line = next(readCSV)

			# pulsed variables declaration
			pulsing_modes = line[1:]
			if(line[0] != "PULSE") :
				print("PULSE line expected not [",line[0],"]!")
				sys.exit(0)
			
			ind = 0
			for plusing_mode in pulsing_modes :
				pulsing_modes_dict[ids[ind]] = plusing_mode
				ind  += 1
			line = next(readCSV)

	#####################################################
	##### WRITES THE SCENARIO VARIABLES % CALLBACKS IN THE SCRIPT HEADER AND BODY FILES
	#####################################################

	############################### constant declarations
	# number of scene variables
	ScriptHeader.write("enum InterpVarIDs{ \n")
	# scene variable initialization
	ScriptHeader.write("  _%s = 0,\n" % ids[0])
	for id_string in ids[1:] :
		ScriptHeader.write("  _%s,\n" % id_string)
		# print("  _%s,", ids
	ScriptHeader.write("  _MaxInterpVarIDs};\n")

	############################### scenario variable declarations
	# interpolation cancelation variable declaration
	# ScriptBody.write("bool BrokenInterpolationVar[ _MaxInterpVarIDs ]"
	# scenario variable declarations
	for id_string, type_string in zip(ids, types):
		if(type_string == "sign") :
			type_string = "float"			
		elif(type_string == "path") :
			type_string = "bool"
		ScriptBody.write("%-5s %-20s;\n" % (type_string , id_string))

	# extern scenario variable declaration in the header file
	for id_string, type_string in zip(ids, types):
		if(type_string == "sign") :
			type_string = "float"			
		elif(type_string == "path") :
			type_string = "bool"
		ScriptHeader.write("extern %-5s %-20s;\n" % (type_string , id_string))

	# extern scenario variable types, pointers and messages declaration in the header file
	ScriptHeader.write("extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern void * ScenarioVarPointers[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *ScenarioVarMessages[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern char *CmdString[_MaxInterpVarIDs];\n")

	# scenario variable types declarations
	ScriptBody.write("VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { \n")
	for type_string in types :
		ScriptBody.write("	_pg_%s,\n" % type_string)
	ScriptBody.write("};\n")

	# scenario variable pointers declarations
	ScriptBody.write("void * ScenarioVarPointers[_MaxInterpVarIDs] = { \n")
	for id_string in ids :
		ScriptBody.write("	(void *)&%s,\n" % id_string)
	ScriptBody.write("};\n")

	# variable callback pointers declarations
	for id_string in callBacks :
		if(id_string != "NULL") :
			ScriptBody.write("void %s(pg_Parameter_Input_Type param_input_type, double scenario_or_gui_command_value);\n" % id_string)

	ScriptBody.write("void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, double) = { \n")
	for id_string in callBacks :
		if(id_string == "NULL") :
			ScriptBody.write("	NULL,\n")
		else :
			ScriptBody.write("	&%s,\n" % id_string)
	ScriptBody.write("};\n")

	ScriptBody.write("char *ScenarioVarMessages[_MaxInterpVarIDs] = { \n")
	for id_string in messageStrings :
		if(id_string == "NULL") :
			ScriptBody.write("  (char *)\"\",\n")
		else :
			ScriptBody.write("  (char *)\""+id_string+"\",\n")
	ScriptBody.write("};\n")

	# ScriptHeader.write("enum PulseTypes : _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none "
	ScriptBody.write("PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = { ")
	for pulsing_mode in pulsing_modes :
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
	ScriptBody.write("char *CmdString[_MaxInterpVarIDs] = { \n")
	for idstring in ids :
		ScriptBody.write("  (char *)\""+idstring+"\",\n")
	ScriptBody.write("};\n")

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

	# scans the line of the scenario file that indicates what is the target shader for each variable
	# variables are grouped by 4 to be declared as vec4
	# target_shader_list is * / ParticleRender_fs / Master_fs / Update_fs / Mixing_fs / ParticleAnimation_fs
	for target_shader_list, id_string, type_string in zip(target_shaders,ids,types) : 
		if(target_shader_list != "*") :
			# "*" nothing is done, the variable is not a shader parameter
			if(withParticleShaders) :
				if(re.search(r'ParticleAnimation_fs', target_shader_list) != None) :
					# ParticleAnimation_fs parameter
					ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] = id_string
					ParticleAnimation_fs_types[ParticleAnimation_fs_index] = type_string
					if(type_string == "bool" or type_string == "path") :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "bool	  " + id_string + ";\n"
					elif(type_string == "int") :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "int		" + id_string + ";\n"
					else :
						ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "float	 " + id_string + ";\n"
					
					if(ParticleAnimation_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleAnimation_scenario_var_data;\n")
						ShaderBodyDecl.write("GLint uniform_ParticleAnimation_scenario_var_data;\n")
						ShaderBodyBind.write("  bindAndCheckUniform(&uniform_ParticleAnimation_scenario_var_data, \"uniform_ParticleAnimation_scenario_var_data\", pg_shader_ParticleAnimation);\n")
					
					ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + " ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] = "

					if(ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "bool" or ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "path") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = (uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] > 0 ? true : false);\n";
					elif(ParticleAnimation_fs_types[ParticleAnimation_fs_index] == "int") :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = int(uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "]);\n"
					else :
						ParticleAnimation_body_glsl = ParticleAnimation_body_glsl + "  " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " = uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
					
					pulsing_mode = pulsing_modes_dict[ParticleAnimation_fs_IDs[ParticleAnimation_fs_index]]
					if(pulsing_mode == "*") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " * (1.f + pulse_average * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " + pulse_average * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "(GLfloat)" + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + " * (pulse_average - pulse_average_prec) * " + ParticleAnimation_fs_IDs[ParticleAnimation_fs_index] + "_pulse;\n"
					else :
						print("Unknown pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				 	
					ParticleAnimation_fs_index  += 1
			
			if(re.search(r'Update_fs', target_shader_list) != None) :
				# Update_fs parameter
				Update_fs_IDs[Update_fs_index] = id_string
				Update_fs_types[Update_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Update_head_glsl = Update_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Update_head_glsl = Update_head_glsl + "int		" + id_string + ";\n"
				else :
					Update_head_glsl = Update_head_glsl + "float	 " + id_string + ";\n"

				if(Update_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Update_scenario_var_data;\n")
					ShaderBodyDecl.write("GLint uniform_Update_scenario_var_data;\n")
					ShaderBodyBind.write("  bindAndCheckUniform(&uniform_Update_scenario_var_data, \"uniform_Update_scenario_var_data\", pg_shader_Update);\n")

				Update_bindingString_cpp = Update_bindingString_cpp + " Update_scenario_var_data[" + str(Update_fs_index) + "] = "
				if(Update_fs_types[Update_fs_index] == "bool" or Update_fs_types[Update_fs_index] == "path") :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = (uniform_Update_scenario_var_data[" + str(Update_fs_index) + "] > 0 ? true : false);\n";
				elif(Update_fs_types[Update_fs_index] == "int") :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = int(uniform_Update_scenario_var_data[" + str(Update_fs_index) + "]);\n"
				else :
					Update_body_glsl = Update_body_glsl + "  " + Update_fs_IDs[Update_fs_index] + " = uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
				
				pulsing_mode = pulsing_modes_dict[Update_fs_IDs[Update_fs_index]]
				if(pulsing_mode == "*") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " * (1.f + pulse_average * " + Update_fs_IDs[Update_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " + pulse_average * " + Update_fs_IDs[Update_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Update_bindingString_cpp = Update_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Update_fs_index] + " * (pulse_average - pulse_average_prec) * " + Update_fs_IDs[Update_fs_index] + "_pulse;\n"
				else :
					print("Unknown pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
				
				Update_fs_index += 1
			
			if(re.search(r'Mixing_fs', target_shader_list) != None) :
				# Mixing_fs parameter
				Mixing_fs_IDs[Mixing_fs_index] = id_string
				Mixing_fs_types[Mixing_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Mixing_head_glsl = Mixing_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Mixing_head_glsl = Mixing_head_glsl + "int		" + id_string + ";\n"
				else :
					Mixing_head_glsl = Mixing_head_glsl + "float	 " + id_string + ";\n"

				if(Mixing_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Mixing_scenario_var_data;\n")
					ShaderBodyDecl.write("GLint uniform_Mixing_scenario_var_data;\n")
					ShaderBodyBind.write("  bindAndCheckUniform(&uniform_Mixing_scenario_var_data, \"uniform_Mixing_scenario_var_data\", pg_shader_Mixing);\n")
				
				Mixing_bindingString_cpp = Mixing_bindingString_cpp + " Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] = "
				if(Mixing_fs_types[Mixing_fs_index] == "bool" or Mixing_fs_types[Mixing_fs_index] == "path") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = (uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] > 0 ? true : false);\n";
				elif(Mixing_fs_types[Mixing_fs_index] == "int") :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = int(uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "]);\n"
				else :
					Mixing_body_glsl = Mixing_body_glsl + "  " + Mixing_fs_IDs[Mixing_fs_index] + " = uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
				
				pulsing_mode = pulsing_modes_dict[Mixing_fs_IDs[Mixing_fs_index]]
				if(pulsing_mode == "*") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + " * (1.f + pulse_average * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Mixing_fs_IDs[Mixing_fs_index] + " + pulse_average * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Mixing_bindingString_cpp = Mixing_bindingString_cpp + "(GLfloat)" + Update_fs_IDs[Mixing_fs_index] + " * (pulse_average - pulse_average_prec) * " + Mixing_fs_IDs[Mixing_fs_index] + "_pulse;\n"
				else :
					print("Unknown pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
			 	
				Mixing_fs_index += 1
			

			if(withParticleShaders) :
				if(re.search(r'ParticleRender_fs', target_shader_list) != None) :
					# ParticleRender_fs parameter
					ParticleRender_fs_IDs[ParticleRender_fs_index] = id_string
					ParticleRender_fs_types[ParticleRender_fs_index] = type_string
					if(type_string == "bool" or type_string == "path") :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "bool	  " + id_string + ";\n"
					elif(type_string == "int") :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "int		" + id_string + ";\n"
					else :
						ParticleRender_head_glsl = ParticleRender_head_glsl + "float	 " + id_string + ";\n"

					if(ParticleRender_fs_index == 0) :
						ShaderHeader.write("extern GLint uniform_ParticleRender_scenario_var_data\n")
						ShaderBodyDecl.write("GLint uniform_ParticleRender_scenario_var_data\n")
						ShaderBodyBind.write("  bindAndCheckUniform(&uniform_ParticleRender_scenario_var_data, \"uniform_ParticleRender_scenario_var_data\", pg_shader_Particle);\n")
					

					ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + " ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] = "
					if(ParticleRender_fs_types[ParticleRender_fs_index] == "bool" or ParticleRender_fs_types[ParticleRender_fs_index] == "path") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = (uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] > 0 ? true : false);\n";
					elif(ParticleRender_fs_types[ParticleRender_fs_index] == "int") :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = int(uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "]);\n"
					else :
						ParticleRender_body_glsl = ParticleRender_body_glsl + "  " + ParticleRender_fs_IDs[ParticleRender_fs_index] + " = uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
					
					pulsing_mode = pulsing_modes_dict[ParticleRender_fs_IDs[ParticleRender_fs_index]]
					if(pulsing_mode == "*") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " * (1.f + pulse_average * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " + pulse_average * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "(GLfloat)" + ParticleRender_fs_IDs[ParticleRender_fs_index] + " * (pulse_average - pulse_average_prec) * " + ParticleRender_fs_IDs[ParticleRender_fs_index] + "_pulse;\n"
					else :
						print("Unknown pulsing mode [%s]" % pulsing_mode)
						sys.exit(0)
				 	
					ParticleRender_fs_index += 1
			
			if(re.search(r'Master_fs', target_shader_list) != None) :
				# Master_fs parameter
				Master_fs_IDs[Master_fs_index] = id_string
				Master_fs_types[Master_fs_index] = type_string
				if(type_string == "bool" or type_string == "path") :
					Master_head_glsl = Master_head_glsl + "bool	  " + id_string + ";\n"
				elif(type_string == "int") :
					Master_head_glsl = Master_head_glsl + "int		" + id_string + ";\n"
				else :
					Master_head_glsl = Master_head_glsl + "float	 " + id_string + ";\n"

				if(Master_fs_index == 0) :
					ShaderHeader.write("extern GLint uniform_Master_scenario_var_data;\n")
					ShaderBodyDecl.write("GLint uniform_Master_scenario_var_data;\n")
					ShaderBodyBind.write("  bindAndCheckUniform(&uniform_Master_scenario_var_data, \"uniform_Master_scenario_var_data\", pg_shader_Master);\n")
				
				Master_bindingString_cpp = Master_bindingString_cpp + " Master_scenario_var_data[" + str(Master_fs_index) + "] = "
				if(Master_fs_types[Master_fs_index] == "bool" or Master_fs_types[Master_fs_index] == "path") :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = (uniform_Master_scenario_var_data[" + str(Master_fs_index) + "] > 0 ? true : false);\n";
				elif(Master_fs_types[Master_fs_index] == "int") :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = int(uniform_Master_scenario_var_data[" + str(Master_fs_index) + "]);\n"
				else :
					Master_body_glsl = Master_body_glsl + "  " + Master_fs_IDs[Master_fs_index] + " = uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
				
				pulsing_mode = pulsing_modes_dict[Master_fs_IDs[Master_fs_index]]
				if(pulsing_mode == "*") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + ";\n"
				elif(pulsing_mode == "pulsed_absolute") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " * (1.f + pulse_average * " + Master_fs_IDs[Master_fs_index] + "_pulse);\n"
				elif(pulsing_mode == "pulsed_uniform") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " + pulse_average * " + Master_fs_IDs[Master_fs_index] + "_pulse;\n"
				elif(pulsing_mode == "pulsed_differential") :
					Master_bindingString_cpp = Master_bindingString_cpp + "(GLfloat)" + Master_fs_IDs[Master_fs_index] + " * (pulse_average - pulse_average_prec) * " + Master_fs_IDs[Master_fs_index] + "_pulse;\n"
				else :
					print("Unknown pulsing mode [%s]" % pulsing_mode)
					sys.exit(0)
				
				Master_fs_index += 1


	# once the varaiabes have been scanned and grouped by 4, the remaining ones are transformed into variables
	if(withParticleShaders) :
		if(ParticleAnimation_fs_index > 0) :
			ParticleAnimation_head_glsl = ParticleAnimation_head_glsl + "uniform float uniform_ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n"
			ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + " glUniform1fv(uniform_ParticleAnimation_scenario_var_data, " + str(ParticleAnimation_fs_index) + ", ParticleAnimation_scenario_var_data);\n"
			ShaderBodyDecl.write("float ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n")
			ShaderHeader.write("extern float ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "];\n")
		


	if(Update_fs_index > 0) :
		Update_head_glsl = Update_head_glsl + "uniform float uniform_Update_scenario_var_data[" + str(Update_fs_index) + "];\n"
		Update_bindingString_cpp = Update_bindingString_cpp + " glUniform1fv(uniform_Update_scenario_var_data, " + str(Update_fs_index) + ", Update_scenario_var_data);\n"
		ShaderBodyDecl.write("float Update_scenario_var_data[" + str(Update_fs_index) + "];\n")
		ShaderHeader.write("extern float Update_scenario_var_data[" + str(Update_fs_index) + "];\n")

	if(Mixing_fs_index > 0) :
		Mixing_head_glsl = Mixing_head_glsl + "uniform float uniform_Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n"
		Mixing_bindingString_cpp = Mixing_bindingString_cpp + " glUniform1fv(uniform_Mixing_scenario_var_data, " + str(Mixing_fs_index) + ", Mixing_scenario_var_data);\n"
		ShaderBodyDecl.write("float Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n")
		ShaderHeader.write("extern float Mixing_scenario_var_data[" + str(Mixing_fs_index) + "];\n")

	if(withParticleShaders) :
		if(ParticleRender_fs_index > 0) :
			ParticleRender_head_glsl = ParticleRender_head_glsl + "uniform float uniform_ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "];\n"
			ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + " glUniform1fv(uniform_ParticleRender_scenario_var_data, " + str(ParticleRender_fs_index) + ", ParticleRender_scenario_var_data);\n"
			ShaderBodyDecl.write("float ParticleRender_scenario_var_data[ParticleRender_fs_index];\n")
			ShaderHeader.write("extern float ParticleRender_scenario_var_data[ParticleRender_fs_index];\n")
		

	if(Master_fs_index > 0) :
		Master_head_glsl = Master_head_glsl + "uniform float uniform_Master_scenario_var_data[" + str(Master_fs_index) + "];\n"
		Master_bindingString_cpp = Master_bindingString_cpp + " glUniform1fv(uniform_Master_scenario_var_data, " + str(Master_fs_index) + ", Master_scenario_var_data);\n"
		ShaderBodyDecl.write("float Master_scenario_var_data[" + str(Master_fs_index) + "];\n")
		ShaderHeader.write("extern float Master_scenario_var_data[" + str(Master_fs_index) + "];\n")

	# draws the bindings grouped by shader in the draw.cpp file

	if(withParticleShaders) :
		UpdateBody.write("\n glUseProgram(shader_programme[pg_shader_ParticleAnimation]);\n" + ParticleAnimation_bindingString_cpp)

	UpdateBody.write("\n glUseProgram(shader_programme[pg_shader_Update]);\n" + Update_bindingString_cpp)
	UpdateBody.write("\n glUseProgram(shader_programme[pg_shader_Mixing]);\n" + Mixing_bindingString_cpp)
	if(withParticleShaders) :
		UpdateBody.write("\n glUseProgram(shader_programme[pg_shader_ParticleRender]);\n" + ParticleRender_bindingString_cpp)

	UpdateBody.write("\n glUseProgram(shader_programme[pg_shader_Master]);\n" + Master_bindingString_cpp)

	# manages the inclusions inside the shader files to generate the linearized shader files
	if(withParticleShaders) :
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

	if(withParticleShaders) :
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

	Configuration_InputCsv.close()
	Scenario_InputCsv.close()
	if(withParticleShaders) :
		ParticleAnimation_InputShader.close()
		ParticleAnimation_OutputShader.close()

	Update_InputShader.close()
	Update_OutputShader.close()
	Mixing_InputShader.close()
	Mixing_OutputShader.close()
	if(withParticleShaders) :
		ParticleRendering_InputShader.close() 
		ParticleRendering_OutputShader.close() 

	Master_InputShader.close() 
	Master_OutputShader.close() 

	ScriptHeader.close() 
	ScriptBody.close() 
	ShaderHeader.close() 
	ShaderBodyDecl.close() 
	ShaderBodyBind.close() 
	UpdateBody.close() 

	print("")

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
