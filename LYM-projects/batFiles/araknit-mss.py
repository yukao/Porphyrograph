#!/usr/bin/python3 

# launch command for the animation of a MSS
# (c) Yukao Nagemi

import sys
# sys.path.append('/mnt/d/sync.com/Sync/LYM-sources/vv/vv_python/')
sys.path.append('D:\\sync.com\\Sync\\LYM-sources\\vv\\vv_python\\')
sys.path.append('D:\\sync.com\\Sync\\LYM-sources\\pg-sensors')
import pg_sensors
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( pg_sensors.main(["-s", "D:\\sync.com\\Sync\\LYM-projects\\Projects\\configuration\\LYM_araknit_sensors.csv", "-i", "D:\\sync.com\\Sync\\LYM-projects\\Projects\\configuration\\LYM_araknit_mss.csv"]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
