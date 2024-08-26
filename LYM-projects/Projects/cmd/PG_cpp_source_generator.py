#!/usr/bin/python3 

# PG_cpp_source_generator.py
# command to generate cpp files from full scenario

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

Header_Input_name = ""
FullScenario_InputCsv_name = ""

# C++ included file names (only one file cumulating the data 
# from the first configuration file and all the scenario files)
ScriptHeader_name = ""
ScriptHeader = None

ScriptBody_name = ""
ScriptBody = None

ShaderBodyBind_name = ""
ShaderBodyBind = None

RenderBody_name = ""
RenderBody = None

# full scenario: dictionaries of scenario variables + memory of rank inside scenario
full_scenario_vars_specs_dict = {}
full_varID_from_index = {}
full_nb_vars = 0

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
	print("CPP code generation from full scenario: End of code generation\n\n")
	sys.exit(0)

USAGE = '''	print("CPP code generation from full scenario: Usage: python PG_cpp_source_generator.py --header <pg-header.h> --full_scenario <pg_full_scenario.csv> --script_header_out <pg_script_header_Core.h> --script_body_out <pg_script_body_Core.cpp>  --shader_header_out <pg_shader_header_Core.h> --shader_body_decl_out <pg_shader_body_decl_Core.cpp> --shader_body_bind_out <pg_shader_body_bind_Core.cpp> --render_body_out <pg_render_body_Core.cpp>")
PG_cpp_source_generator.py: A program for generating C++ code for Porphyrograph")

'''

##################################################################
# READS THE CONFIGURATION VARIABLES FROM THE CSV CONFIGURATION FILE 
##################################################################
def read_headerConst(header_fileName) :
	global header_const_dict

	try:
		headerIn = open(header_fileName, "rt")
	except IOError:
		print("CPP code generator: File not found:", header_fileName, " or path is incorrect")
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
# READS THE SCENARIO FILE TOP: VARIABLES 
##################################################################
def expect_tag(line_scenario, tag_string) :
	if(line_scenario[0] != tag_string) :
		print(tag_string, "first column string expected not [",line_scenario[0],"]!")
		print(line_scenario)
		print("CPP code generator: End of code generation\n\n")
		sys.exit(0)
def check_tag(line_scenario, tag_string) :
	return (line_scenario[0] == tag_string)

def read_full_scenario_variables(scenarioCSV, indScenario) :
	global full_scenario_vars_specs_dict
	global full_varID_from_index
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
	return

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
	print("CPP code generator: Unknown scenario variable type ", scenario_type)
	print("CPP code generator: End of code generation\n\n")
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
	print("CPP code generator: Unknown scenario variable type ", scenario_type)
	print("CPP code generator: End of code generation\n\n")
	sys.exit(0)


def write_script_header_and_body() :
	global ScriptHeader
	global ScriptBodywrite_script_header_and_body

	global full_scenario_vars_specs_dict

	ScriptHeader.write("// FULL SCENARIO VARIABLES\n")

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
		# print("CPP code generator:   _%s,", scenario_var_ids
	ScriptHeader.write("  _MaxInterpVarIDs};\n")

	############################### scenario variable declarations
	# interpolation cancelation variable declaration
	# ScriptBody.write("bool BrokenInterpolationVar[ _MaxInterpVarIDs ]"
	ScriptBody.write("// FULL SCENARIO VARIABLES\n")

	# scenario variable declarations
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = scenario_to_cpp_type(full_specs[2])
		index_range = scenario_to_cpp_range(full_specs[2])
		if(index_range == []) :
			if(full_specs[7] == "") :
				print("CPP code generator: Variable", var_ID, "without initial value (type [",type_string,"])")
				print("CPP code generator: End of code generation\n\n")
				sys.exit(0)
			if(type_string == "string") :
				ScriptBody.write("%-5s %-20s = \"%s\";\n" % (type_string , var_ID, full_specs[7]))
			elif(type_string == "float") :
				ScriptBody.write("%-5s %-20s = float(%s);\n" % (type_string , var_ID, full_specs[7]))
			else :
				ScriptBody.write("%-5s %-20s = %s;\n" % (type_string , var_ID, full_specs[7]))
		else :
			val_init = full_specs[7].split('/')
			nb_expected_vals = header_const_value(index_range[1]) - int(index_range[0])
			if(len(val_init) != nb_expected_vals) :
				print("CPP code generator: Array variable", var_ID, "expects", nb_expected_vals, " initial value (of type [",type_string,"])", "not" , len(val_init))
				print("CPP code generator: End of code generation\n\n")
				sys.exit(0)
			initial_value = ""
			for index in range(0, int(index_range[0])) :
				if(type_string.startswith("string")) :
					initial_value = initial_value + "\"\", "
				elif(type_string.startswith("float")) :
					initial_value = initial_value + "0.f, "
				else :
					initial_value = initial_value + "0, "
			for index in range(int(index_range[0]), header_const_value(index_range[1])) :
				value = val_init[index - int(index_range[0])]
				if(type_string.startswith("string")) :
					initial_value = initial_value + value + ", "
				elif(type_string.startswith("float")) :
					if(value.find(".") != -1) :
						initial_value = initial_value + value + "f, "
					else :
						initial_value = initial_value + value + ".f, "
				else :
					initial_value = initial_value + value + ", "
			ScriptBody.write("%-5s %-20s[%s] = %s;\n" % (type_string , var_ID, index_range[1], "{"+initial_value+"}"))


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
	ScriptHeader.write("extern VarTypes pg_FullScenarioVarTypes[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern int pg_FullScenarioVarIndiceRanges[_MaxInterpVarIDs][2];\n")
	ScriptHeader.write("extern void * pg_FullScenarioVarPointers[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern std::string pg_FullScenarioVarMessages[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern std::string pg_FullScenarioVarStrings[_MaxInterpVarIDs];\n")
	ScriptHeader.write("extern void (*pg_FullScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue);\n")
	ScriptHeader.write("extern void (*pg_FullScenarioArrayVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue, int);\n")
	ScriptHeader.write("extern PulseTypes ScenarioVarPulse[_MaxInterpVarIDs];\n")

	# scenario variable types declarations
	ScriptBody.write("VarTypes pg_FullScenarioVarTypes[_MaxInterpVarIDs] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		type_string = scenario_to_cpp_type(full_specs[2])
		ScriptBody.write("	_pg_%s,\n" % type_string)
	ScriptBody.write("};\n")

	# scenario variable ranges declarations
	ScriptBody.write("int pg_FullScenarioVarIndiceRanges[_MaxInterpVarIDs][2] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		index_range = scenario_to_cpp_range(full_specs[2])
		if(index_range == []) :
			ScriptBody.write("	{-1, -1},\n")
		else :
			ScriptBody.write('	{{{0}, {1}}},\n'.format(index_range[0], index_range[1]))
	ScriptBody.write("};\n")

	# scenario variable pointers declarations
	ScriptBody.write("void * pg_FullScenarioVarPointers[_MaxInterpVarIDs] = { \n")
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
		index_range = scenario_to_cpp_range(type_string)
		if(callBack_id_string != "NULL") :
			callBack_generic_id_string = callBack_id_string + "_generic"
			if(type_string == "bool") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));\n".format(callBack_id_string))
			elif(type_string == "sign") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, double_to_sign(scenario_or_gui_command_value.val_num));\n".format(callBack_id_string))
			elif(type_string == "path") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));\n".format(callBack_id_string))
			elif(type_string == "float") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, float(scenario_or_gui_command_value.val_num));\n".format(callBack_id_string))
			elif(type_string == "int") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, int(scenario_or_gui_command_value.val_num));\n".format(callBack_id_string))
			elif(type_string == "string") :
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(param_input_type, scenario_or_gui_command_value.val_string);\n".format(callBack_id_string))
			elif(type_string.startswith("int")) :
				ScriptBody.write("void {}(int array_index, pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value, int array_index) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(array_index, param_input_type, int(scenario_or_gui_command_value.val_array[array_index]));\n".format(callBack_id_string))
			elif(type_string.startswith("bool")) :
				ScriptBody.write("void {}(int array_index, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value, int array_index) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(array_index, param_input_type, bool(scenario_or_gui_command_value.val_array[array_index]));\n".format(callBack_id_string))
			elif(type_string.startswith("float")) :
				ScriptBody.write("void {}(int array_index, pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);\n".format(callBack_id_string))
				ScriptBody.write("void {}(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value, int array_index) {{\n".format(callBack_generic_id_string))
				ScriptBody.write("	{}(array_index, param_input_type, scenario_or_gui_command_value.val_array[array_index]);\n".format(callBack_id_string))
			else :
				print("CPP code generator: Unknown callback {} parameter type [{}]".format(callBack_id_string, type_string))
				print("CPP code generator: End of code generation\n\n")
				sys.exit(0)
			ScriptBody.write("}\n")

	ScriptBody.write("void (*pg_FullScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue) = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		callBack_id_string = full_specs[3]
		callBack_generic_id_string = callBack_id_string + "_generic"
		type_string = full_specs[2]
		index_range = scenario_to_cpp_range(type_string)
		if(callBack_id_string == "NULL" or index_range != []) :
			ScriptBody.write("	NULL,\n")
		else :
			ScriptBody.write("	&%s,\n" % callBack_generic_id_string)
	ScriptBody.write("};\n")

	ScriptBody.write("void (*pg_FullScenarioArrayVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue, int) = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		callBack_id_string = full_specs[3]
		callBack_generic_id_string = callBack_id_string + "_generic"
		type_string = full_specs[2]
		index_range = scenario_to_cpp_range(type_string)
		if(callBack_id_string == "NULL" or index_range == []) :
			ScriptBody.write("	NULL,\n")
		else :
			ScriptBody.write("	&%s,\n" % callBack_generic_id_string)
	ScriptBody.write("};\n")

	ScriptBody.write("std::string pg_FullScenarioVarMessages[_MaxInterpVarIDs] = { \n")
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	for var_ID, full_specs in full_scenario_vars_specs_dict.items():
		msg_string = full_specs[4]
		if(msg_string == "NULL") :
			ScriptBody.write("  \"\",\n")
		else :
			ScriptBody.write("  \""+msg_string+"\",\n")
	ScriptBody.write("};\n")

	# ScriptHeader.write("enum PulseTypes : _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none "
	ScriptBody.write("PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = { \n")
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
	ScriptBody.write("std::string pg_FullScenarioVarStrings[_MaxInterpVarIDs] = { \n")
	for var_ID in full_scenario_vars_specs_dict:
		ScriptBody.write("  \""+var_ID+"\",\n")
	ScriptBody.write("};\n")

	ScriptHeader.close() 
	ScriptBody.close() 

###############################################################################
# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
###############################################################################
def write_binding_vars_header_and_body() :
	global ShaderBodyBind_name
	global ShaderBodyBind

	global RenderBody_name
	global RenderBody

	global scenario_active_vars

	##################################################################
	# SHADER VARIABLES
	# WRITES SHADERS DECLARATIONS AND VARIABLE VALUES
	##################################################################

	ParticleRender_tableFillingString_cpp = ""
	Master_tableFillingString_cpp = ""
	ParticleAnimation_tableFillingString_cpp = ""
	Update_tableFillingString_cpp = ""
	Mixing_tableFillingString_cpp = ""
	ParticleRender_bindingString_cpp = ""
	Master_bindingString_cpp = ""
	ParticleAnimation_bindingString_cpp = ""
	Update_bindingString_cpp = ""
	Mixing_bindingString_cpp = ""

	ShaderBodyBind.write("\n"); 

	# scans the line of the full scenario file that indicates what is the target shader for each variable
	# variables are grouped by 4 to be declared as vec4
	# target_shader_list is * / ParticleRender_fs / Master_fs / Update_fs / Mixing_fs / ParticleAnimation_fs
	# full_scenario_vars_specs_dict: [ind_var, varVerbatim, varType, varCallBack, varGUI, varShader, varPulse, varInitial]
	#############################
	# FILLS THE TABLES FORWARDING SCENARIO DATA TO SHADERS
	ParticleRender_fs_index = 0
	Master_fs_index = 0
	ParticleAnimation_fs_index = 0
	Update_fs_index = 0
	Mixing_fs_index = 0
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
				if(not re.findall(r'\[', type_string)) :
					ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] = "
					if(pulsing_mode == "*") :
						ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
						print("CPP code generator: End of code generation\n\n")
						sys.exit(0)
				else:
					# ParticleAnimation_fs_index -= int(header_const_value(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "ParticleAnimation_scenario_var_data[" + str(ParticleAnimation_fs_index) + "] = "
						if(pulsing_mode == "*") :
							ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							ParticleAnimation_tableFillingString_cpp = ParticleAnimation_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							print("CPP code generator: End of code generation\n\n")
							sys.exit(0)
						ParticleAnimation_fs_index  += 1
					ParticleAnimation_fs_index  -= 1

				ParticleAnimation_fs_index  += 1

			if(re.search(r'Update_fs', target_shader_list) != None) :
				if(not re.findall(r'\[', type_string)) :
					Update_tableFillingString_cpp = Update_tableFillingString_cpp + "Update_scenario_var_data[" + str(Update_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("CPP code generator: Unknown Update pulsing mode [%s]" % pulsing_mode)
						print("CPP code generator: End of code generation\n\n")
						sys.exit(0)
				else:
					# Update_fs_index -= int(header_const_value(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Update_tableFillingString_cpp = Update_tableFillingString_cpp + "Update_scenario_var_data[" + str(Update_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Update_tableFillingString_cpp = Update_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							print("CPP code generator: End of code generation\n\n")
							sys.exit(0)
						Update_fs_index  += 1
					Update_fs_index  -= 1

				Update_fs_index += 1

			if(re.search(r'Mixing_fs', target_shader_list) != None) :
				if(not re.findall(r'\[', type_string)) :
					Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("CPP code generator: Unknown Mixing pulsing mode [%s]" % pulsing_mode)
						print("CPP code generator: End of code generation\n\n")
						sys.exit(0)
				else:
					# Mixing_fs_index -= int(header_const_value(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "Mixing_scenario_var_data[" + str(Mixing_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Mixing_tableFillingString_cpp = Mixing_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							print("CPP code generator: End of code generation\n\n")
							sys.exit(0)
						Mixing_fs_index  += 1
					Mixing_fs_index  -= 1
			 	
				Mixing_fs_index += 1
			
			if(re.search(r'ParticleRender_fs', target_shader_list) != None) :
				if(not re.findall(r'\[', type_string)) :
					ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] = "
					if(pulsing_mode == "*") :
						ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("CPP code generator: Unknown ParticleRender pulsing mode [%s]" % pulsing_mode)
						print("CPP code generator: End of code generation\n\n")
						sys.exit(0)
				else:
					# ParticleRender_fs_index -= int(header_const_value(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "ParticleRender_scenario_var_data[" + str(ParticleRender_fs_index) + "] = "
						if(pulsing_mode == "*") :
							ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							ParticleRender_tableFillingString_cpp = ParticleRender_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							print("CPP code generator: End of code generation\n\n")
							sys.exit(0)
						ParticleRender_fs_index  += 1
					ParticleRender_fs_index  -= 1
			 	
				ParticleRender_fs_index += 1
			
			if(re.search(r'Master_fs', target_shader_list) != None) :
				if(not re.findall(r'\[', type_string)) :
					Master_tableFillingString_cpp = Master_tableFillingString_cpp + "Master_scenario_var_data[" + str(Master_fs_index) + "] = "
					if(pulsing_mode == "*") :
						Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + ";\n"
					elif(pulsing_mode == "pulsed_absolute") :
						Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (1.f + pulse_average * " + var_ID + "_pulse);\n"
					elif(pulsing_mode == "pulsed_uniform") :
						Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + " + pulse_average * " + var_ID + "_pulse;\n"
					elif(pulsing_mode == "pulsed_differential") :
						Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + " * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse;\n"
					else :
						print("CPP code generator: Unknown Master pulsing mode [%s]" % pulsing_mode)
						print("CPP code generator: End of code generation\n\n")
						sys.exit(0)
				else:
					# Master_fs_index -= int(header_const_value(index_range[1])) - 1 - int(index_range[0])
					for index in range(int(index_range[0]), header_const_value(index_range[1])) :
						Master_tableFillingString_cpp = Master_tableFillingString_cpp + "Master_scenario_var_data[" + str(Master_fs_index) + "] = "
						if(pulsing_mode == "*") :
							Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_absolute") :
							Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (1.f + pulse_average * " + var_ID + "_pulse[" + str(index) + "]);\n"
						elif(pulsing_mode == "pulsed_uniform") :
							Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] + pulse_average * " + var_ID + "_pulse[" + str(index) + "];\n"
						elif(pulsing_mode == "pulsed_differential") :
							Master_tableFillingString_cpp = Master_tableFillingString_cpp + "(GLfloat)" + var_ID + "[" + str(index) + "] * (pulse_average - pulse_average_prec) * " + var_ID + "_pulse[" + str(index) + "];\n"
						else :
							print("CPP code generator: Unknown ParticleAnimation pulsing mode [%s]" % pulsing_mode)
							print("CPP code generator: End of code generation\n\n")
							sys.exit(0)
						Master_fs_index  += 1
					Master_fs_index  -= 1
				
				Master_fs_index += 1

	#############################
	# SEND TABLE DATA TO SHADERS
	# once the variables have been scanned, C++ and glsl tables are created to pass the values from CPU to GPU. There is a table per shader.
	if(ParticleAnimation_fs_index > 0) :
		ParticleAnimation_bindingString_cpp = ParticleAnimation_bindingString_cpp + "  glUniform1fv(uniform_ParticleAnimation_scenario_var_data[pg_ind_scenario], " + str(ParticleAnimation_fs_index) + ", ParticleAnimation_scenario_var_data);\n"
		ShaderBodyBind.write("	ParticleAnimation_scenario_var_data  = new float[" + str(ParticleAnimation_fs_index) + "];\n")
		
	if(Update_fs_index > 0) :
		Update_bindingString_cpp = Update_bindingString_cpp + "  glUniform1fv(uniform_Update_scenario_var_data[pg_ind_scenario], " + str(Update_fs_index) + ", Update_scenario_var_data);\n"
		ShaderBodyBind.write("	Update_scenario_var_data  = new float[" + str(Update_fs_index) + "];\n")

	if(Mixing_fs_index > 0) :
		Mixing_bindingString_cpp = Mixing_bindingString_cpp + "  glUniform1fv(uniform_Mixing_scenario_var_data[pg_ind_scenario], " + str(Mixing_fs_index) + ", Mixing_scenario_var_data);\n"
		ShaderBodyBind.write("	Mixing_scenario_var_data  = new float[" + str(Mixing_fs_index) + "];\n")

	if(ParticleRender_fs_index > 0) :
		ParticleRender_bindingString_cpp = ParticleRender_bindingString_cpp + "  glUniform1fv(uniform_ParticleRender_scenario_var_data[pg_ind_scenario], " + str(ParticleRender_fs_index) + ", ParticleRender_scenario_var_data);\n"
		ShaderBodyBind.write("	ParticleRender_scenario_var_data  = new float[" + str(ParticleAnimation_fs_index) + "];\n")

	if(Master_fs_index > 0) :
		Master_bindingString_cpp = Master_bindingString_cpp + "  glUniform1fv(uniform_Master_scenario_var_data[pg_ind_scenario], " + str(Master_fs_index) + ", Master_scenario_var_data);\n"
		ShaderBodyBind.write("	Master_scenario_var_data  = new float[" + str(Master_fs_index) + "];\n")

	#############################
	# WRITE TABLE FILLING AND TABLE DATA FORWARDING 
	# draws the bindings grouped by shader in the draw.cpp file
	if(ParticleAnimation_tableFillingString_cpp != "") :
		RenderBody.write(ParticleAnimation_tableFillingString_cpp + "\n")
	if(Mixing_tableFillingString_cpp != "") :
		RenderBody.write(Mixing_tableFillingString_cpp + "\n")
	if(Update_tableFillingString_cpp != "") :
		RenderBody.write(Update_tableFillingString_cpp + "\n")
	if(ParticleRender_tableFillingString_cpp != "") :
		RenderBody.write(ParticleRender_tableFillingString_cpp + "\n")
	if(Master_tableFillingString_cpp != "") :
		RenderBody.write(Master_tableFillingString_cpp + "\n")

	RenderBody.write("\nif (pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]) {\n")
	RenderBody.write("  glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleAnimation]);\n" + ParticleAnimation_bindingString_cpp)
	RenderBody.write("}\n")

	RenderBody.write("\nif (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]) {\n" )
	RenderBody.write("  glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Update]);\n" + Update_bindingString_cpp)
	RenderBody.write("}\n")

	RenderBody.write("\nif (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]) {\n")
	RenderBody.write("  glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mixing]);\n" + Mixing_bindingString_cpp)
	RenderBody.write("}\n")

	RenderBody.write("\nif (pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]) {\n")
	RenderBody.write("  glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ParticleRender]);\n" + ParticleRender_bindingString_cpp)
	RenderBody.write("}\n")

	RenderBody.write("\nif (pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]) {\n")
	RenderBody.write("  glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Master]);\n" + Master_bindingString_cpp)
	RenderBody.write("}\n")

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global Header_Input_name
	global FullScenario_InputCsv_name

	global scenario_vars_columns_list

	global ScriptHeader_name
	global ScriptHeader

	global ScriptBody_name
	global ScriptBody

	global ShaderBodyBind_name
	global ShaderBodyBind

	global RenderBody_name
	global RenderBody

	global full_scenario_vars_specs_dict

	##################################################################
	# ARGUMENTS
	##################################################################
	try:
		opts, args = getopt.getopt(main_args,"c:f:",["header=", "full_scenario=", "script_header_out=", "script_body_out=", "shader_body_bind_out=", "render_body_out="])
	except getopt.GetoptError:
		print(USAGE)
		print("CPP code generator: End of code generation\n\n")
		sys.exit(2)
	for opt, arg in opts:

		# ARGTS
		if opt in ("-c", "--header"):
			Header_Input_name = arg
		elif opt in ("-f", "--full_scenario"):
			FullScenario_InputCsv_name = arg
		# C++ OUTPUT INCLUDED FILES
		elif opt in ("--script_header_out"):
			ScriptHeader_name = arg
		elif opt in ("--script_body_out"):
			ScriptBody_name = arg
		elif opt in ("--shader_body_bind_out"):
			ShaderBodyBind_name = arg
		elif opt in ("--render_body_out"):
			RenderBody_name = arg
		else:
			print("CPP code generator: option (", opt, ")")
			assert False, "unhandled option"

	###############################################################################
	# C++ OUTPUT INCLUDED FILES
	try:
		ScriptHeader = open(ScriptHeader_name, "wt")
	except IOError:
		print("CPP code generator: File not found:", ScriptHeader_name, " or path is incorrect")
	try:
		ScriptBody = open(ScriptBody_name, "wt")
	except IOError:
		print("CPP code generator: File not found:", ScriptBody_name, " or path is incorrect")
	try:
		ShaderBodyBind = open(ShaderBodyBind_name, "wt")
	except IOError:
		print("CPP code generator: File not found:", ShaderBodyBind_name, " or path is incorrect")
	try:
		RenderBody = open(RenderBody_name, "wt")
	except IOError:
		print("CPP code generator: File not found:", RenderBody_name, " or path is incorrect")

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
	read_full_scenario_variables(readFullCSV, -1)
	input_full_scenario_fileCsv.close()

	#####################################################
	##### WRITES THE SCENARIO VARIABLES % CALLBACKS IN THE SCRIPT HEADER AND BODY FILES
	#####################################################
	write_script_header_and_body()

	###############################################################################
	# WRITE SHADER HEADER BODY AND BINDING AND C++ UPDATE
	###############################################################################
	write_binding_vars_header_and_body()

	RenderBody.close()

	###############################################################################
	# END OF CPP SOURCE GENERATOR
	###############################################################################
	print("\n*************************************************************\n")
	print("Successfull CPP code generation from full scenario\n")
	print("*************************************************************\n\n")

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
