#!/usr/bin/python3 

# vv_replace_in_subdirs.py
# replaces strings in subdirs

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os

import getopt
import sys
import shutil
from signal import signal, SIGINT

import re

from vv_lib import to_num

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_replace_in_subdirs.py -i <file_name> -t <file_name>
ARGUMENTS:
   -t: temp file
   -i: input dir
'''

##################################################################
# MAIN SUB
##################################################################
def main(main_args):
	try:
		opts, args = getopt.getopt(main_args,"i:t:",["inputdir=","tempfile="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputdir"):
			inputdir = arg
		elif opt in ("-t", "--tempfile"):
			tempfile = arg
		else:
			assert False, "unhandled option"
			print(USAGE)
			sys.exit(2)

	for root, dirs, files in os.walk(inputdir):
		for name in files:
			if name.endswith((".csv")):
				if root.endswith(("/")):
					full_name = root+name
				else:
					full_name = root+"/"+name

				# copy file to tempo
				if(full_name != tempfile) :
					shutil.copyfile(full_name, tempfile)

				# open file
				try:
					FILEin = open(tempfile, "rt")
				except IOError:
					print("File not created :", tempfile)

				# open file
				try:
					FILEout = open(full_name, "wt")
				except IOError:
					print("File not created :", full_name)

				# scanning the file
				print("scan of ", full_name)
				line = FILEin.readline()
				line = line.rstrip()
				while(line):
					line = line.rstrip()
					if(re.search(r'22,23,24', line) != None) :
						line = re.sub(r'22,23,24', '22,23,24,25',line)
					if(re.search(r'nb of configs,nb CA parameters,nb Master Masks', line) != None) :
						line = re.sub(r'nb of configs,nb CA parameters,nb Master Masks', 'nb of configs,nb CA parameters,nb Master Masks,nb ClipArt layers',line)
					if(re.search(r'const,const,const', line) != None) :
						line = re.sub(r'const,const,const', 'const,const,const,const',line)
					if(re.search(r'PG_MAX_CONFIGURATIONS,PG_NB_CA_PARAMS,PG_NB_MASTER_MASKS', line) != None) :
						line = re.sub(r'PG_MAX_CONFIGURATIONS,PG_NB_CA_PARAMS,PG_NB_MASTER_MASKS', 'PG_MAX_CONFIGURATIONS,PG_NB_CA_PARAMS,PG_NB_MASTER_MASKS,PG_NB_CLIPART_LAYERS',line)
					if(re.search(r'10,8,6', line) != None) :
						line = re.sub(r'10,8,6', '10,8,6,8',line)
					
					FILEout.write(line)
					FILEout.write("\n")
			
					# next line
					line = FILEin.readline()
					line = line.rstrip()
				
				FILEin.close()
				FILEout.close()
		break
	
	return 1


if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])