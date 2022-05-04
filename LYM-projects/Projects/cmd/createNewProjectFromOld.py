#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math

import os
import sys


def main():
	old_project_name = "effe"
	new_project_name = "Song"
	command = "cp ../shaders/LYM_Update_{0!s}.vert ../shaders/LYM_Update_{1!s}.vert".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Update_{0!s}.frag ../shaders/LYM_Update_{1!s}.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Update_{0!s}_full.frag ../shaders/LYM_Update_{1!s}_full.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleSplat_{0!s}.vert ../shaders/LYM_ParticleSplat_{1!s}.vert".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleSplat_{0!s}.frag ../shaders/LYM_ParticleSplat_{1!s}.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleSplat_{0!s}_full.frag ../shaders/LYM_ParticleSplat_{1!s}_full.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleSplat_{0!s}.geom ../shaders/LYM_ParticleSplat_{1!s}.geom".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleAnimation_{0!s}.vert ../shaders/LYM_ParticleAnimation_{1!s}.vert".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleAnimation_{0!s}.frag ../shaders/LYM_ParticleAnimation_{1!s}.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_ParticleAnimation_{0!s}_full.frag ../shaders/LYM_ParticleAnimation_{1!s}_full.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Mixing_{0!s}.vert ../shaders/LYM_Mixing_{1!s}.vert".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Mixing_{0!s}.frag ../shaders/LYM_Mixing_{1!s}.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Mixing_{0!s}_full.frag ../shaders/LYM_Mixing_{1!s}_full.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Master_{0!s}.vert ../shaders/LYM_Master_{1!s}.vert".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Master_{0!s}.frag ../shaders/LYM_Master_{1!s}.frag".format(old_project_name, new_project_name)
	os.system( command )
	command = "cp ../shaders/LYM_Master_{0!s}_full.frag ../shaders/LYM_Master_{1!s}_full.frag".format(old_project_name, new_project_name)
	os.system( command )
	# command = "cp ../scenarios/LYM_{0!s}-scenario-v1.csv ../scenarios/LYM_{1!s}-scenario-v1.csv".format(old_project_name, new_project_name)
	# os.system( command )
	# command = "cp ../configuration/LYM_{0!s}-head-single.csv ../configuration/LYM_{1!s}-head-single.csv".format(old_project_name, new_project_name)
	# os.system( command )
	# command = "cp ../batFiles/{0!s}.PG-single.bat ../batFiles/{1!s}.PG-single.bat".format(old_project_name, new_project_name)
	# os.system( command )
	# command = "cp ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{0!s}.user ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{1!s}.user".format(old_project_name, new_project_name)
	# os.system( command )
	# command = "cp ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{0!s}.vcxproj ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{1!s}.vcxproj".format(old_project_name, new_project_name)
	# os.system( command )
	# command = "cp ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{0!s}.vcxproj.user ../../../LYM-sources/Porphyrograph-VC17/Porphyrograph_core_{1!s}.vcxproj.user".format(old_project_name, new_project_name)
	# os.system( command )

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()

