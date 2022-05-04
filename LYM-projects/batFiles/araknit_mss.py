#!\\usr\\bin\\python3 

# launch command for the initial posterization of a vector video
# (c) Yukao Nagemi

import sys
sys.path.append('C:\\sync.com\\Sync\\LYM-sources\\vv\\vv_python\\')
sys.path.append('C:\\sync.com\\Sync\\LYM-sources\\pg-sensors\\')
# sys.path.append('pg_OSC.UDP_clientssync.com\\Sync\\LYM-sources\\share\\MetaWear-SDK-Python/')

import pg_sensors
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( pg_sensors.main(["-c", "C:\\sync.com\\Sync\\LYM-projects\\Projects\\configuration\\LYM_araknit_configuration_mss.csv","--scenario", "C:\\sync.com\\Sync\\LYM-projects\\Projects\\scenarios\\LYM_araknit-scenario_mss.csv","--training-data", "C:\\sync.com\\Sync\\LYM-projects\\Data\\core-data\\XMM-training\\CavernePlaton\\","--training-sensors-ID", ['mw1','mw2'],"--likelihood-window", 40,"--training-labels", ['gesture1','gesture2','gesture3']]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
