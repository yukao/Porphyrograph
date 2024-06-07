#!/usr/bin/python3 

# vv_scenario_to_chataigne.py
# automatic generation of chataigne components from porphyrograph scenario

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import sys
sys.path.append('/mnt/c/home/LYM-sources/vv/vv_python/')

from vv_lib import clamp
from vv_lib import to_num
from vv_lib import to_num_or_string
from vv_lib import interpolate
from vv_lib import current_sound_volume
from vv_lib import peaked_current_sound_volume
from vv_lib import current_onset_volume
from vv_lib import smoothed_current_sound_volume
from vv_lib import read_sound_data
from vv_lib import read_onset_data
from vv_lib import random_target_regular_expression_name

import csv
import re
import getopt
import sys
import random
from signal import signal, SIGINT

import numpy as np

from math import sin, cos

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_scenario_to_chataigne.py -i scenario_file -o lilnut_file
ARGUMENTS:
	 -i: input scenariofile -d chataigne_data_directory
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	# DECIDES WHETHER OR NOT TO MANAGE ONLY VARYING VARIABLES
	ONLY_VARYING_VARIABLES = True

	# SCENE VARIABLES
	scene_names = []
	scene_durations = []
	
	# SCENARIO VARIABLES
	nb_vars = 0
	var_names = []
	var_types = dict()

	# GROUPS OF SCENARIO VARIABLES
	group_vars = dict()
	nb_var_groups = 0

	# STARTUP VALUES AND INITIAL/FINAL VALUES IN SCENES
	var_val_startups = dict()
	var_val_inits = []
	var_val_finals = []
	var_one_variation_in_scenario = dict()

	# INTERPOLATION MODE FOR VARIABLES IN SCENES
	val_interp = []

	outout_string = ""

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["inputfile=", "lilnut_file="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		elif opt in ("-o", "--lilnut_file"):
			lilnut_file_name = arg
		else:
			msg = "unhandled option ["+opt+"]"
			assert False, msg
			print( main_args )
			print(USAGE)
			sys.exit(2)

	# print( 'Input scenario file is ', scenario_file_name)

	# in scenario file
	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
			print("File not found :", scenario_file_name, " or path is incorrect")
	# counts the scene number
	readCSV = csv.reader(FILEin, delimiter=',')
	line = next(readCSV) 
	nb_scenes = 0
	while (True) :
		if(line[0] == "scene") :
			nb_scenes += 1
		elif(line[0] == "/scenario") :
			break
	print( "nb scenes ", nb_scenes)
	# restarts from beginning
	FILEin.seek(0)

	# out chataigne file
	try:
		FILEout = open(lilnut_file_name, "wt")
	except IOError:
			print("File not opened :", lilnut_file_name, " or path is incorrect")
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	# 2nd line, var ranks
	line = next(readCSV) 
	# print "line1 ", line 

	# 2nd line, var groups
	var_groups = next(readCSV) 
	#removes the first one
	var_groups.pop(0)

	#3rd line: variable types
	var_types_line =  next(readCSV)
	#removes the first one
	var_types_line.pop(0)

	#4th line: variable names
	var_names =  next(readCSV) 
	#removes the first one
	var_names.pop(0)

	#5th line: callbacks
	line = next(readCSV) 
	#6th line: GUI
	line = next(readCSV) 
	#7th line: shader target
	line = next(readCSV) 
	#8th line: pulse
	line = next(readCSV) 
	#9, 10, 11 lines: initial values
	line = next(readCSV) 
	val_startup_line =  next(readCSV) 
	#removes the first one
	val_startup_line.pop(0)
	line = next(readCSV) 

	# var count
	nb_vars = len(var_names)

	#finishes the groups now that the variable names are known
	indVar = 0
	for var_group in  var_groups:
		if not(var_group in group_vars) :
			group_vars[var_group] = []
		group_vars[var_group].append(var_names[indVar])
		indVar += 1
	if(indVar != nb_vars) :
		print("incorrect number of var groups", indVar, "vs nb of vars", nb_vars)
		sys.exit(0)

	# var groups count
	nb_var_groups = len(group_vars.keys())

	#finishes the types now that the variable strings are known
	indVar = 0
	for var_type in var_types_line :
		var_types[var_names[indVar]] = var_type
		indVar += 1
	if(indVar != nb_vars) :
		print("incorrect number of var types", indVar, "vs nb of vars", nb_vars)
		sys.exit(0)

	#finishes the startup values now that the variable strings are known
	indVar = 0
	for val_startup in val_startup_line :
		var_val_startups[var_names[indVar]] = to_num_or_string(val_startup)
		var_one_variation_in_scenario[var_names[indVar]] = False
		indVar += 1
	if(indVar != nb_vars) :
		print("incorrect number of var startup values", indVar, "vs nb of vars", nb_vars)
		sys.exit(0)

	# 12th line, scenario line
	values_line =  next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found in line:", values_line)
		return 0
	if(indVar != nb_vars) :
		print("incorrect number of var initial", indVar, "vs nb of vars", nb_vars)
		sys.exit(0)
	
	# nb_scenes = to_num(values_line[1])

	outout_string = f'''{{
  "modules": [
    {{
      "parameters": [
        {{
          "value": true,
          "controlAddress": "/logIncoming"
        }},
        {{
          "value": true,
          "controlAddress": "/logOutgoing"
        }}
      ],
      "niceName": "OSC",
      "type": "OSC",
      "scripts": {{
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
      "params": {{
        "containers": {{
          "oscInput": {{
            "parameters": [
              {{
                "value": 9000,
                "hexMode": false,
                "controlAddress": "/localPort"
              }}
            ]
          }},
          "oscOutputs": {{
            "items": [
              {{
                "parameters": [
                  {{
                    "value": "127.0.0.1",
                    "controlAddress": "/remoteHost"
                  }},
                  {{
                    "value": 8000,
                    "hexMode": false,
                    "controlAddress": "/remotePort"
                  }}
                ],
                "niceName": "OSC Output",
                "type": "BaseItem"
              }}
            ],
            "viewOffset": [
              0,
              0
            ],
            "viewZoom": 1.0
          }},
          "pass_through": {{
            "editorIsCollapsed": true
          }}
        }}
      }},
      "templates": {{
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
      "values": {{
      }}
    }}
  ],
'''

	##################################################################
	# READING PORPHYROGRAPH SCENES
	##################################################################
	var_val_inits = [dict() for x in range(nb_scenes)]
	var_val_finals = [dict() for x in range(nb_scenes)]
	val_interp = [dict() for x in range(nb_scenes)]
	scene_names = [""] * nb_scenes
	scene_durations = [0] * nb_scenes
	end_scene = nb_scenes
	for ind_scene in range(nb_scenes)[0:end_scene] :
		# scene line
		values_line =  next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_names[ind_scene] = values_line[1]
		scene_durations[ind_scene] = values_line[2]

		# scene ID line
		line =  next(readCSV)
		# var IDs line
		line =  next(readCSV)

		# variable initial value
		values_line =  next(readCSV)
		#removes the first one
		values_line.pop(0)
		indVar = 0
		for value_init in values_line:
			var_val_inits[ind_scene][var_names[indVar]] = to_num_or_string(value_init)
			if(var_val_inits[ind_scene][var_names[indVar]] != var_val_startups[var_names[indVar]]) :
						var_one_variation_in_scenario[var_names[indVar]] = True
			indVar += 1
		if(indVar != nb_vars) :
			print("incorrect number of initial values", indVar, "vs nb of vars", nb_vars)
			sys.exit(0)

		# variable final value
		values_line =  next(readCSV)
		#removes the first one
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			var_val_finals[ind_scene][var_names[indVar]] = to_num_or_string(value_fin)
			if(var_val_finals[ind_scene][var_names[indVar]] != var_val_startups[var_names[indVar]]) :
						var_one_variation_in_scenario[var_names[indVar]] = True
			indVar += 1
		if(indVar != nb_vars) :
			print("incorrect number of final values", indVar, "vs nb of vars", nb_vars)
			sys.exit(0)

		# variable interpolation value
		values_line =  next(readCSV)
		#removes the first one
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			val_interp[ind_scene][var_names[indVar]] = value_interp    
			indVar += 1
		if(indVar != nb_vars) :
			print("incorrect number of interpolation parameters", indVar, "vs nb of vars", nb_vars)
			sys.exit(0)

		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0

	# /scenario line
	values_line = next(readCSV)
	if(values_line[0] != "/scenario") :
		print( "Expected tag \"/scenario\" not found!\n" )
		# return 0

	##################################################################
	# ONLY KEEPS IN GROUPS VARYING VARIABLES
	##################################################################
	if(ONLY_VARYING_VARIABLES == True) :
		discarded_vars = "Discarded vars:"
		discarded_groups = "Discarded groups:"
		full_group_vars = group_vars
		group_vars = dict()
		for group_ID, group_var_lists in full_group_vars.items():
			# copies the vars in the group
			original_vars = group_var_lists
			# makes a new list of vars by only taking the varying vars
			varying_vars = []
			for var_name in original_vars:
				if(var_one_variation_in_scenario[var_name] == True) :
					varying_vars.append(var_name)
				else:
					discarded_vars = discarded_vars + var_name + ", "

			# assigns this list to the group
			if(varying_vars != []) :
				group_vars[group_ID] = varying_vars
			else:
				discarded_groups = discarded_groups + group_ID + ", "

	# feedback
	print(discarded_vars)
	print(discarded_groups)

	##################################################################
	# TRANSLATING SCENES INTO VARIABLE GROUPS AND PRESETS
	# ASSOCIATED WITH SCENES
	##################################################################
	outout_string = outout_string + '''  "customVariables": [
'''
	# ALL GROUPS: 
	last_group = list(group_vars.keys())[-1]
	for unique_var_group in group_vars.keys() :
	# ONE GROUP for unique_var_group in list(group_vars.keys())[:1] :
		group_separator = ","
		if(unique_var_group == last_group) :
			group_separator = ""
		outout_string = outout_string + f'''    {{
      "niceName": "{unique_var_group}",
      "type": "CVGroup",
      "params": {{
        "parameters": [
          {{
            "value": "Weights",
            "controlAddress": "/controlMode"
          }}
        ]
      }},
'''

		##################################################################
		# DECLARES THE VARIABLES IN THE GROUP
		outout_string = outout_string + f'''      "variables": {{
        "items": [
'''
		last_var = group_vars[unique_var_group][-1]
		for var_name in group_vars[unique_var_group] :
			var_control_address = var_name[0].lower()+var_name[1:]
			var_initial_value = var_val_startups[var_name]
			variable_type = "Float"
			if(isinstance(var_initial_value, str)) :
				if(var_initial_value[0] != '"') :
					var_initial_value = '"'+var_initial_value+'"'
				variable_type = "String"
			separator = ","
			if(var_name == last_var) :
				separator = ""
			outout_string = outout_string + f'''          {{
            "parameters": [
              {{
                "value": {var_initial_value},
                "controlAddress": "/{var_control_address}",
                "feedbackOnly": false,
                "type": "{variable_type}",
                "niceName": "{var_name}",
                "customizable": true,
                "removable": false,
                "description": "Custom control of type {variable_type}",
                "hideInEditor": false
              }}
            ],
            "niceName": "{var_name}",
            "editorIsCollapsed": true,
            "type": "{variable_type} Parameter"
          }}{separator}
'''

		outout_string = outout_string + f'''        ],
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
'''

		##################################################################
		# DECLARES THE PRESETS ASSOCIATED WITH THE VARIABLES IN THE GROUP
		# EACH SCENE PRODUCES TWO PRESETS: ONE FOR INITIAL AND ONE FOR FINAL VALUES
		outout_string = outout_string + f'''      "presets": {{
        "items": [
'''

		# SCANS ALL THE SCENES
		end_scene = nb_scenes
		for ind_scene in range(nb_scenes)[0:end_scene] :
			end_separator = ","
			if(ind_scene == end_scene - 1) :
				end_separator = ""
			sc_ID = scene_names[ind_scene]

			# intitial values of the scene
			outout_string = outout_string + f'''          {{
            "parameters": [
              {{
                "value": [
                  0.8627451062202454,
                  0.0,
                  1.0,
                  1.0
                ],
                "controlAddress": "/color"
              }}
            ],
            "niceName": "{sc_ID}_Ini",
            "type": "MorphTarget",
            "values": {{
              "containers": {{
'''

			last_var = group_vars[unique_var_group][-1]
			for var_name in group_vars[unique_var_group] :
				var_initial_value = var_val_inits[ind_scene][var_name]
				separator = ","
				if(var_name == last_var) :
					separator = ""
				var_control_address = var_name[0].lower()+var_name[1:]
				outout_string = outout_string + f'''                "{var_control_address}": {{
                  "parameters": [
                    {{
                      "value": {var_initial_value},
                      "controlAddress": "/{var_control_address}"
                    }}
                  ],
                  "owned": true,
                  "niceName": "{var_name}"
                }}{separator}
'''

			outout_string = outout_string + f'''              }}
            }}
          }},
'''

			# final values of the scene
			outout_string = outout_string + f'''          {{
            "parameters": [
              {{
                "value": [
                  0.8627451062202454,
                  0.0,
                  1.0,
                  1.0
                ],
                "controlAddress": "/color"
              }}
            ],
            "niceName": "{sc_ID}_Final",
            "type": "MorphTarget",
            "values": {{
              "containers": {{
'''

			last_var = group_vars[unique_var_group][-1]
			for var_name in group_vars[unique_var_group] :
				var_control_address = var_name[0].lower()+var_name[1:]
				var_final_value = var_val_finals[ind_scene][var_name]
				separator = ","
				if(var_name == last_var) :
					separator = ""
				outout_string = outout_string + f'''                "{var_control_address}": {{
                  "parameters": [
                    {{
                      "value": {var_final_value},
                      "controlAddress": "/{var_control_address}"
                    }}
                  ],
                  "owned": true,
                  "niceName": "{var_name}"
                }}{separator}
'''

			outout_string = outout_string + f'''              }}
            }}
          }}{end_separator}
'''

		outout_string = outout_string + f'''        ],
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }}
    }}{group_separator}
'''
	outout_string = outout_string + f'''  ],
'''

	##################################################################
	# DECLARES THE SEQUENCES ASSOCIATED WITH EACH SCENE
	# EACH SEQUENCE HAS AS MANY MAPPINGS AS GROUPS
	outout_string = outout_string + f'''  "states": null,
'''
	outout_string = outout_string + f'''  "sequences": [
'''
	# ALL SCENES, ONE SEQUENCE PER SCENE
	end_scene = nb_scenes
	for ind_scene in range(nb_scenes)[0:end_scene] :
		end_separator = ","
		if(ind_scene == end_scene - 1) :
			end_separator = ""

		is_enabled = "false"
		if(ind_scene == 0) :
			is_enabled = "true"

		sc_ID = scene_names[ind_scene]
		scene_duration = scene_durations[ind_scene]

		outout_string = outout_string + f'''    {{
      "parameters": [
        {{
          "value": {is_enabled},
          "controlAddress": "/enabled"
        }},
        {{
          "value": {scene_duration},
          "controlAddress": "/totalTime"
        }},
        {{
          "value": 0.0,
          "controlAddress": "/viewStartTime"
        }},
        {{
          "value": {scene_duration},
          "controlAddress": "/viewEndTime"
        }},
        {{
          "value": "",
          "controlAddress": "/ltcSyncModule",
          "enabled": false
        }}
      ],
      "niceName": "{sc_ID}_seq",
      "type": "Sequence",
      "layers": {{
        "hideInEditor": true,
        "items": [
'''
		# ALL GROUPS: 
		last_group = list(group_vars.keys())[-1]
		for unique_var_group in group_vars.keys() :
		# ONE GROUP for unique_var_group in list(group_vars.keys())[:1] :
			group_separator = ","
			if(unique_var_group == last_group) :
				group_separator = ""
			group_ID = unique_var_group.lower()
			outout_string = outout_string + f'''          {{
            "parameters": [
              {{
                "value": 120.0,
                "controlAddress": "/listSize"
              }},
              {{
                "value": 120,
                "hexMode": false,
                "controlAddress": "/uiHeight"
              }}
            ],
            "niceName": "{unique_var_group}_mapping",
            "containers": {{
              "automation": {{
                "parameters": [
                  {{
                    "value": {scene_duration},
                    "controlAddress": "/length"
                  }},
                  {{
                    "value": [
                      0.0,
                      1.0
                    ],
                    "controlAddress": "/viewValueRange"
                  }},
                  {{
                    "value": [
                      0.0,
                      1.0
                    ],
                    "controlAddress": "/range",
                    "enabled": true
                  }}
                ],
                "hideInEditor": true,
                "hideInRemoteControl": false,
                "items": [
                  {{
                    "parameters": [
                      {{
                        "value": 0.0,
                        "controlAddress": "/position"
                      }},
                      {{
                        "value": 0.0,
                        "controlAddress": "/value"
                      }},
                      {{
                        "value": "Bezier",
                        "controlAddress": "/easingType"
                      }}
                    ],
                    "niceName": "Key",
                    "containers": {{
                      "easing": {{
                        "parameters": [
                          {{
                            "value": [
                              7.134147167205811,
                              0.0
                            ],
                            "controlAddress": "/anchor1"
                          }},
                          {{
                            "value": [
                              -7.134147167205811,
                              0.0
                            ],
                            "controlAddress": "/anchor2"
                          }}
                        ]
                      }}
                    }},
                    "type": "Key"
                  }},
                  {{
                    "parameters": [
                      {{
                        "value": {scene_duration},
                        "controlAddress": "/position"
                      }},
                      {{
                        "value": 1.0,
                        "controlAddress": "/value"
                      }},
                      {{
                        "value": "Bezier",
                        "controlAddress": "/easingType"
                      }}
                    ],
                    "niceName": "Key 1",
                    "containers": {{
                      "easing": {{
                      }}
                    }},
                    "type": "Key"
                  }}
                ],
                "viewOffset": [
                  0,
                  0
                ],
                "viewZoom": 1.0
              }},
              "recorder": {{
                "parameters": [
                  {{
                    "value": "",
                    "controlAddress": "/inputValue"
                  }}
                ],
                "editorIsCollapsed": true
              }},
              "mapping": {{
                "niceName": "{unique_var_group}_mapping",
                "type": "Mapping",
                "im": {{
                  "hideInEditor": true,
                  "items": [
                    {{
                      "parameters": [
                        {{
                          "value": "",
                          "controlAddress": "/inputValue"
                        }}
                      ],
                      "niceName": "Input Value",
                      "type": "Input Value"
                    }}
                  ],
                  "viewOffset": [
                    0,
                    0
                  ],
                  "viewZoom": 1.0
                }},
                "params": {{
                  "parameters": [
                    {{
                      "value": 50,
                      "hexMode": false,
                      "controlAddress": "/updateRate"
                    }},
                    {{
                      "value": false,
                      "controlAddress": "/sendOnOutputChangeOnly"
                    }}
                  ],
                  "editorIsCollapsed": true
                }},
                "filters": {{
                  "viewOffset": [
                    0,
                    0
                  ],
                  "viewZoom": 1.0
                }},
                "outputs": {{
                  "items": [
                    {{
                      "niceName": "{unique_var_group}_mapping_Output",
                      "type": "BaseItem",
                      "commandModule": "customVariables",
                      "commandPath": "",
                      "commandType": "Interpolate presets",
                      "command": {{
                        "parameters": [
                          {{
                            "value": "/{group_ID}/presets/{sc_ID}_Ini",
                            "controlAddress": "/targetPreset"
                          }},
                          {{
                            "value": "/{group_ID}/presets/{sc_ID}_Final",
                            "controlAddress": "/targetPreset2"
                          }}
                        ],
                        "paramLinks": {{
                          "value": {{
                            "linkType": 1,
                            "mappingValueIndex": 0
                          }}
                        }}
                      }}
                    }}
                  ],
                  "viewOffset": [
                    0,
                    0
                  ],
                  "viewZoom": 1.0
                }}
              }}
            }},
            "type": "Mapping"
          }}{group_separator}
'''
		outout_string = outout_string + f'''        ],
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
      "cues": {{
        "hideInEditor": true,
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
      "editing": true
    }}{end_separator}
'''

	outout_string = outout_string + f'''  ],
'''

	##################################################################
	# ROUTER BETWEEN VARIABLES AND OSC MESSAGES
	##################################################################
	outout_string = outout_string + f'''  "routers": [
    {{
      "niceName": "Router Track Levels",
      "type": "BaseItem",
      "sourceModule": "customVariables",
      "sourceValues": {{
        "items": [
'''

	# ALL GROUPS: 
	last_group = list(group_vars.keys())[-1]
	for unique_var_group in group_vars.keys() :
	# ONE GROUP for unique_var_group in list(group_vars.keys())[:1] :
		last_var = group_vars[unique_var_group][-1]
		for var_name in group_vars[unique_var_group] :
			group_separator = ","
			if(unique_var_group == last_group and var_name == last_var) :
				group_separator = ""
			outout_string = outout_string + f'''          {{
            "niceName": "{var_name}",
            "type": "BaseItem",
            "customName": "{var_name}",
            "routeParams": {{
              "parameters": [
                {{
                  "value": "/{var_name}",
                  "controlAddress": "/address"
                }}
              ]
            }}
          }}{group_separator}
'''

	outout_string = outout_string + f'''        ],
        "viewOffset": [
          0,
          0
        ],
        "viewZoom": 1.0
      }},
      "destModule": "osc"
    }}
  ]
}}
'''

	FILEout.write(outout_string)
	FILEout.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
