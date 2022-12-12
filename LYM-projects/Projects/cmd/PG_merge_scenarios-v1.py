#!/usr/bin/python3 

# PG_merge_scenarios_standardForm_Projects-v1.py
# command to merge scenes from various scenarios into a new scenario which combines all the variables and adds default values to variables which are not found into all the scenes

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import csv
import re
import getopt
import sys
from signal import signal, SIGINT

full_trace = False

# control C break
def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''	print("Usage: python PG_merge_scenarios_standardForm_Projects-v1.py -i scenes-file.csv -o scenario-file.csv")
PG_merge_scenarios_standardForm_Projects-v1.py: A program for merging different Porphyrograph scenarios")

'''

def expect_tag(line_scenario, tag_string) :
		if(line_scenario[0] != tag_string) :
			print(tag_string, "first column string expected not [",line_scenario[0],"]!")
			print(line_scenario)
			sys.exit(0)
def check_tag(line_scenario, tag_string) :
		return (line_scenario[0] == tag_string)

def read_scenario_header(scenarioCSV) :
	################ scenario header
	# RANK
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "RANK")

	# VERBATIM
	loc_variable_verbatims = next(scenarioCSV)
	expect_tag(loc_variable_verbatims, "VERBATIM")
	loc_variable_verbatims = loc_variable_verbatims[1:]

	# TYPE
	loc_variable_types = next(scenarioCSV)
	expect_tag(loc_variable_types, "TYPE")
	loc_variable_types = loc_variable_types[1:]

	# varialbe IDs
	loc_variable_IDs = next(scenarioCSV)
	expect_tag(loc_variable_IDs, "ID")
	loc_variable_IDs = loc_variable_IDs[1:]

	# -> nb of loc_variables 
	nb_loc_variables = len(loc_variable_IDs)
	print("scenario loc_variables:", nb_loc_variables, end =", ")

	# FUNCTION
	loc_variable_callBacks = next(scenarioCSV)
	expect_tag(loc_variable_callBacks, "FUNCTION")
	loc_variable_callBacks = loc_variable_callBacks[1:]

	# GUI
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "GUI")

	# SHADER
	loc_variable_shaders = next(scenarioCSV)
	expect_tag(loc_variable_shaders, "SHADER")
	loc_variable_shaders = loc_variable_shaders[1:]

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

	# scenario
	lineScenario = next(scenarioCSV)
	expect_tag(lineScenario, "scenario")

	nb_scenes = lineScenario[1]
	print("scenes:", nb_scenes)

	return [loc_variable_IDs, loc_variable_verbatims, loc_variable_types, loc_variable_callBacks, loc_variable_shaders, loc_variable_pulses, loc_variable_initial_values]


##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	global full_trace

	##################################################################
	# ARGUMENTS
	##################################################################
	input_scenes_file_name = ''
	input_footer_scenario_file_name = ''
	input_full_scenario_file_name = ''
	output_scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:I:J:o:",["input_scenes_file_name=", "input_footer_scenario_file_name=", "input_full_scenario_file_name=", "output_scenario_file_name="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)

	for opt, arg in opts:
		# CSV PARAMETER INPUT FILES
		if opt in ("-i", "--input_scenes_file_name"):
			input_list_scenes_file_name = arg
		elif opt in ("-I", "--input_footer_scenario_file_name"):
			input_footer_scenario_file_name = arg
		elif opt in ("-J", "--input_full_scenario_file_name"):
			input_full_scenario_file_name = arg
		elif opt in ("-o", "--output_scenario_file_name"):
			output_scenario_file_name = arg

	# print('Input scenes file is ', input_scenes_file_name)
	# print('Input footer scenario file is ', input_footer_scenario_file_name)
	# print('Input full scenario file is ', input_full_scenario_file_name)
	# print('Output scenario file is ', output_scenario_file_name)

	out_scenario_footer = []

	###############################################################################
	# CSV PARAMETER INPUT FILES
	try:
		input_list_scenes_fileCsv = open(input_list_scenes_file_name, "rt")
	except IOError:
		print("Input file/scene list File not found:", input_list_scenes_file_name, " or path is incorrect")
	try:
		input_full_scenario_fileCsv = open(input_full_scenario_file_name, "rt")
	except IOError:
		print("Input full scenario File not found:", input_full_scenario_file_name, " or path is incorrect")
	try:
		output_scenario_fileCsv = open(output_scenario_file_name, "wt", newline='')
	except IOError:
		print("Output File not opened:", output_scenario_file_name, " or path is incorrect")

	##################################################################
	# READS THE FULL SCENARIO AND MEMORIZES THE HEADER
	##################################################################
	########### reads the header of the full scenario which is supposed to contain all the variables
	# print("reads the header of the full scenario")
	readFullCSV = csv.reader(input_full_scenario_fileCsv, delimiter=',')
	variable_full_scenario_header_IDs, variable_full_scenario_verbatims, variable_full_scenario_types, variable_full_scenario_callBacks, variable_full_scenario_shaders, variable_full_scenario_pulses, variable_full_scenario_initial_values = read_scenario_header(readFullCSV)
	input_full_scenario_fileCsv.close()
	# dictionaries of scenario variable definition
	# build dictionary for the variable of the full scenario
	variable_full_scenario_header_dictionary = dict()
	for var_ID, varVerbatim, varType, varCallBack, varShader, varPulse, varDefault in zip(variable_full_scenario_header_IDs, variable_full_scenario_verbatims, variable_full_scenario_types, variable_full_scenario_callBacks, variable_full_scenario_shaders, variable_full_scenario_pulses, variable_full_scenario_initial_values) :
		variable_full_scenario_header_dictionary[var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]

	##################################################################
	# READS THE CSV SCENARIO/SCENE/NEW NAME FILE LINE BY LINE
	##################################################################
	scenario_scene_list = []
	readCSV = csv.reader(input_list_scenes_fileCsv, delimiter=',')
	# print "line1 ", line 

	while (True) :
		try: 
			line = next(readCSV)
			if (full_trace == True) :
				print(line)
			input_scenario_file = line[0]
			input_scene_fileName = line[1]
			output_scene_fileName = line[2]
			scenario_scene_list.append([input_scenario_file, input_scene_fileName, output_scene_fileName])
		except StopIteration:
			break
	print("end of file")
	input_list_scenes_fileCsv.close()

	##################################################################
	# READS THE SCENE FILE FOR EACH LINE OF THE SCENARIO/SCENE FILE
	##################################################################
	# dictionaries of scene variable values
	variable_scene_values_dictionaries = []
	# scene lines
	variable_scene_descriptions = []
	for file_name_scene_rank in scenario_scene_list:
		# reads scenario file name and scene rank
		input_scenario_file, input_scene_fileName, output_scene_fileName = file_name_scene_rank

		##################################################################
		# READS THE CSV SCENARIO FILE LINE BY LINE
		##################################################################
		try:
			input_scenario_fileCsv = open(input_scenario_file, "rt")
		except IOError:
			print("Scenario File not found:", input_scenario_file, " or path is incorrect")
		readScenarioCSV = csv.reader(input_scenario_fileCsv, delimiter=',')

		########### reads the header of the current scenario
		print("reads the header of the current scenario")
		variable_IDs, variable_verbatims, variable_types, variable_callBacks, variable_shaders, variable_pulses, variable_initial_values = read_scenario_header(readScenarioCSV)

		########### checks whether the header of the full scenario contains all the variables of the current scene
		########### if not, it reports an error
		missing_vars_in_full_scenario = []
		for varID in variable_IDs :
			if not varID in variable_full_scenario_header_IDs :
				missing_vars_in_full_scenario.append(varID)
		if(missing_vars_in_full_scenario != []) :
			print("the following variables are missing in the full scenario", input_full_scenario_file_name, ":", missing_vars_in_full_scenario)
			exit(0)

		########### reads all the scenes until the one that is looked for or the end of the scene list
		ind_scene = 1
		scene_found = False
		while (True) :
			# scene
			variable_scene_description = next(readScenarioCSV)
			if( not check_tag(variable_scene_description, "scene") ) :
				expect_tag(variable_scene_description, "/scenario")
				break

			# scene name and duration
			scene_name = variable_scene_description[1]
			scene_duration = variable_scene_description[2]
			scene_changing_mode = variable_scene_description[3]
			scene_comment_1 = variable_scene_description[4]
			scene_comment_2 = variable_scene_description[5]
			scene_comment_3 = variable_scene_description[6]

			if (full_trace == True) :
				print("scene", ind_scene, ":", scene_name)

			# scene_ID
			lineScenario = next(readScenarioCSV)

			# variable IDs
			lineScenario = next(readScenarioCSV)

			# values at scene beginning
			variable_start_values = next(readScenarioCSV)
			variable_start_values = variable_start_values[1:]

			# values at scene ending
			variable_end_values = next(readScenarioCSV)
			variable_end_values = variable_end_values[1:]

			# interpolator
			variable_interpolators = next(readScenarioCSV)
			variable_interpolators = variable_interpolators[1:]

			# /scene
			lineScenario = next(readScenarioCSV)
			expect_tag(lineScenario, "/scene")

			if(scene_name == input_scene_fileName) :
				scene_found = True

				# replaces the name of the scene with its new name in the scene description
				variable_scene_description[1] = output_scene_fileName

				# build dictionaries for the variable values of the selected scene
				variable_scene_values_dictionary = dict()
				for var_ID, varStartValue, varEndValue, varInterpolator in zip(variable_IDs, variable_start_values, variable_end_values, variable_interpolators) :
					variable_scene_values_dictionary[var_ID] = [varStartValue, varEndValue, varInterpolator]
				variable_scene_values_dictionaries.append(variable_scene_values_dictionary)
				# memorizes the scene description
				variable_scene_descriptions.append(variable_scene_description)

				# reads until the end of the scenario
				while( not check_tag(lineScenario, "/scenario") ) :
					lineScenario = next(readScenarioCSV)
					# print(lineScenario)
				break

			ind_scene += 1

		if not scene_found :
			print("Scene #", input_scene_fileName, "not found inside scenario",input_scenario_file, "!")
			sys.exit(0)

		########### reads the footer of the scenario
		if(input_footer_scenario_file_name == input_scenario_file) :
			out_scenario_footer = []
			while(True):
				try: 
					lineScenario = next(readScenarioCSV)
					out_scenario_footer.append(lineScenario)
				except StopIteration:
					break

		input_scenario_fileCsv.close()

	########### checks whether the footer scenario has been found so that its footer is collected
	if out_scenario_footer == [] :
		# collects the scenario names
		all_scenario_names = list(map(lambda c: c[0], scenario_scene_list))
		# removes duplicates
		all_scenario_names = list(dict.fromkeys(all_scenario_names))
		print("Main scenario file", input_footer_scenario_file_name, "not found among the list of scenario/scenes",all_scenario_names , "!")
		sys.exit(0)

	########### discards the variables in the full scenario var ID list which are not found in the current scenes
	vars_used_in_full_scenario = []
	vars_discarded_from_full_scenario = []
	for varID in variable_full_scenario_header_IDs :
		varID_found = False
		# looks if the full scenario variable is found in at least one of the scene variable dictionaries
		for variable_scene_values_dictionary in variable_scene_values_dictionaries :
			if varID in variable_scene_values_dictionary.keys() :
				varID_found = True
		if not varID_found :
			# the variable is discarded
			vars_discarded_from_full_scenario.append(varID)
		else :
			# the variable is kept
			vars_used_in_full_scenario.append(varID)
	# removes from the list of variable IDS the variable which are discarded
	# and the corresponding entries in the dictionary of the full header are also removed for coherence
	print("discarded full scenario variables", vars_discarded_from_full_scenario)
	for varID in vars_discarded_from_full_scenario :
		variable_full_scenario_header_IDs.remove(varID)
		del variable_full_scenario_header_dictionary[varID]

	##################################################################
	# WRITES THE SCENE HEADER FROM THE FULL SCENARIO
	##################################################################
	# CSV WRITER output_scenario_fileCsv
	# basic writing version without variable reordering and default value addition for the non common variables
	CSVwriter = csv.writer(output_scenario_fileCsv, delimiter=',')
	CSVwriter.writerow(["RANK"] + list(range(len(variable_full_scenario_header_IDs))))
	# variable_full_scenario_header_dictionary[var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
	CSVwriter.writerow(["VERBATIM"] + list(map(lambda x: variable_full_scenario_header_dictionary[x][0] , variable_full_scenario_header_IDs)))
	CSVwriter.writerow(["TYPE"] +  list(map(lambda x: variable_full_scenario_header_dictionary[x][1] , variable_full_scenario_header_IDs)))
	CSVwriter.writerow(["ID"] + variable_full_scenario_header_IDs)
	CSVwriter.writerow(["FUNCTION"] +  list(map(lambda x: variable_full_scenario_header_dictionary[x][2] , variable_full_scenario_header_IDs)))
	CSVwriter.writerow(["GUI"] + variable_full_scenario_header_IDs)
	CSVwriter.writerow(["SHADER"] +  list(map(lambda x: variable_full_scenario_header_dictionary[x][3] , variable_full_scenario_header_IDs)))
	CSVwriter.writerow(["PULSE"] +  list(map(lambda x: variable_full_scenario_header_dictionary[x][4] , variable_full_scenario_header_IDs)))

	##################################################################
	# WRITES THE INITIAL VALUES FROM THE FIRST SCENE
	##################################################################
	CSVwriter.writerow(["initial_values"])
	# the first scene is used for the variable initial values, or default values from the full scenario initial values are chosen
	first_scene_variable_dictionary = variable_scene_values_dictionaries[0]
	# for each variable sets the initial value if it is found in the scene, otherwise sets the default value from the full scenario
	# variable_full_scenario_header_dictionary[var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
	CSVwriter.writerow([""] + list(map(lambda x: first_scene_variable_dictionary[x][0] if(x in first_scene_variable_dictionary.keys()) else variable_full_scenario_header_dictionary[x][5] , variable_full_scenario_header_IDs)))
	CSVwriter.writerow(["/initial_values"])

	##################################################################
	# WRITES THE SCENES
	##################################################################
	CSVwriter.writerow(["scenario"] + [len(scenario_scene_list)])
	ind_scene = 1
	for variable_scene_values_dictionary, variable_scene_description in zip(variable_scene_values_dictionaries, variable_scene_descriptions):
		CSVwriter.writerow(variable_scene_description)
		CSVwriter.writerow([variable_scene_description[1]] * (len(variable_full_scenario_header_IDs) + 1))
		CSVwriter.writerow(["ID"] + variable_full_scenario_header_IDs)
		# for each variable sets the initial value if it is found in the scene, otherwise sets the default value from the full scenario
		# variable_full_scenario_header_dictionary[var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
		CSVwriter.writerow([""] + list(map(lambda x: variable_scene_values_dictionary[x][0] if(x in variable_scene_values_dictionary.keys()) else variable_full_scenario_header_dictionary[x][5] , variable_full_scenario_header_IDs)))
		# for each variable sets the final value if it is found in the scene, otherwise sets the default value from the full scenario
		# variable_full_scenario_header_dictionary[var_ID] = [varVerbatim, varType, varCallBack, varShader, varPulse, varDefault]
		CSVwriter.writerow([""] + list(map(lambda x: variable_scene_values_dictionary[x][1] if(x in variable_scene_values_dictionary.keys()) else variable_full_scenario_header_dictionary[x][5] , variable_full_scenario_header_IDs)))
		# for each variable sets the interpolation value if it is found in the scene, otherwise sets the default 's' fixed (default) value
		CSVwriter.writerow([""] + list(map(lambda x: variable_scene_values_dictionary[x][2] if(x in variable_scene_values_dictionary.keys()) else 's' , variable_full_scenario_header_IDs)))
		CSVwriter.writerow(["/scene"])
		if (full_trace == True) :
			defaulted_vars = []
			for varID in variable_full_scenario_header_IDs :
				if not(varID in variable_scene_values_dictionary.keys()) :
					defaulted_vars.append(varID)
			print("defaulted variables in scene", ind_scene, defaulted_vars)
		ind_scene += 1
	CSVwriter.writerow(["/scenario"])

	##################################################################
	# WRITES THE FOOTER
	##################################################################
	for out_scenario_footer_line in out_scenario_footer:
		CSVwriter.writerow(out_scenario_footer_line)

	output_scenario_fileCsv.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
