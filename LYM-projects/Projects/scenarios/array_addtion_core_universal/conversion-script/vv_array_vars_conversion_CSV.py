#!/usr/bin/python3 

# launch command for the initial posterization of a vector video
# (c) Yukao Nagemi

import sys
sys.path.append('/mnt/c/home/LYM-sources/vv/vv_python/')
sys.path.append('/mnt/c/home/LYM-projects/Projects/scenarios/array_addtion_core_universal/conversion-script/')
import vv_array_vars_conversion_CSV_subdirs
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( vv_array_vars_conversion_CSV_subdirs.main(["-i", "/mnt/c/home/LYM-projects/Projects/scenarios/",\
		"-t", "/mnt/c/home/LYM-projects/Projects/scenarios/tmp.txt"]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
