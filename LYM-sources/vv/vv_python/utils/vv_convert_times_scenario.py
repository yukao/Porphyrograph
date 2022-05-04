#!/usr/bin/python3 

# launch command for the time conversion of a scenario
# (c) Yukao Nagemi

import sys
sys.path.append('/mnt/d/sync.com/Sync/LYM-sources/vv/vv_python/')
import vv_convert_time_to_frames
from signal import signal, SIGINT

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

def main():
	if( vv_convert_time_to_frames.main(["-i", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/montage_mp4/Windows_markers.csv",\
		"-o", "/mnt/d/sync.com/Sync/LYM-videos-sources/YN_dawn_2020/montage_mp4/Windows_markers_frameNo.csv"]) == 0):
		return 0
	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main()
