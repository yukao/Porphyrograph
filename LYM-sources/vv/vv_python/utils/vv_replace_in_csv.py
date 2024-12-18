#!/usr/bin/python3 

# launch command for the initial posterization of a vector video
# (c) Yukao Nagemi

import sys
sys.path.append('/mnt/c/home/LYM-sources/vv/vv_python/')
import vv_replace_in_CSV_subdirs
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( vv_replace_in_CSV_subdirs.main(["-i", "/mnt/c/home/LYM-projects/Projects/configuration/",\
		"-t", "/mnt/c/home/LYM-projects/Projects/configuration/tmp.txt"]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
