#!/usr/bin/python3 

# launch command for the final rasterization of a vector video
# (c) Yukao Nagemi

import sys
sys.path.append('/mnt/c/home/LYM-sources/Python/chataigne/')
import vv_scenario_to_chataigne
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( vv_scenario_to_chataigne.main(["-i", "/mnt/c/home/LYM-projects/Projects/scenarios/YN_AteliersDu5-scenario-v2.csv","-o", "/mnt/c/home/LYM-sources/Chataigne/YN_AteliersDu5-scenario-v2.lilnut"]) == 0):
		return 0

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
