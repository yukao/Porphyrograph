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
	# the parameters are used to define
	# - the list of sensors that the system will try to connect to for drawing based gesture and gesture recognition
	# - the list of gestures that will be trained and afterwards identified
	# - the list of clip art indices in porphyrograph scenario that will be displayed when a gesture is recognized
	# - the possible forwarding of data to porphyrograph: the web animation 
	#   (the positions of the masses at the web nodes), the gesture recognition (on/off of clip art
	#   to indicate which gesture has been recognized), and the positions of the masses bound to sensors
	#   as drawing from gesture tool
	# - the possible forwarding of sensor acceleration to LIVE
	# - the parameters for learning (likelihood window), and for triggering gesture recognition
	#   (minimal threshold of likelihood and minimal duration of keeping the value above this threshold)
	# here the parameters are made to forward the gesture recognition and the positions of the 2 sensor controlled mass (last 3 booleans)
	if( pg_sensors.main(["-c", "C:\\sync.com\\Sync\\LYM-projects\\Projects\\configuration\\metawear_sensor_web_animation_configuration.csv", "--scenario", "C:\\sync.com\\Sync\\LYM-projects\\Projects\\scenarios\\metawear_sensor_web_animation_scenario-4scenes.csv", "--training-data", "C:\\sync.com\\Sync\\LYM-projects\\Data\\core-data\\XMM-training\\CavernePlaton\\", "--training-sensors-ID", ['mw1','mw2'], "--sensors-sample-frequency", 0.15, "--likelihood-window", 40, "--training-labels", ['gesture1','gesture2','gesture3','gesture4','gesture5','gesture6'], "--training-label-ClipArts", [0, 1, 2, 3, 4, 5], "--likelihood-threshold", 0.7, "--likelihood-minimal-duration", 1.0, "--forward-web-animation", False, "--forward-gesture-recognition", True, "--forward-sensor-masses-animation", True, "--forward-sensor-acceleration", True]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
