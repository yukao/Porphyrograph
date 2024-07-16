#!/usr/bin/python3 

# vv_extract_audio_timecodes_from_davinci_resolve_xml.py
# uses DaVinci Resolve xml export to extract audio timecodes

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import os


import csv
import re
import getopt
import sys
import random
from signal import signal, SIGINT
from lxml import etree

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''vv_extract_audio_timecodes_from_davinci_resolve_xml.py -i resolve_xml_file -o audio_timecodes
ARGUMENTS:
	-i resolve_xml_file -o audio_timecodes
'''

##################################################################
# MAIN SUB
##################################################################
def framesToMnSec(nbFrames):
	loc_nbFrames = int(nbFrames)
	if(loc_nbFrames >= 0) :
		nbMn = loc_nbFrames // (60 * 30)
		nbSec = (loc_nbFrames - nbMn	* 60 * 30) / 30.0
		return "{:d}'{:.1f}".format(nbMn,nbSec)
	else :
		return nbFrames	

def main(main_args) :
	try:
		opts, args = getopt.getopt(main_args,"i:o:",["resolve_xml_file=","audio_timecodes="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--resolve_xml_file"):
			resolve_xml_file = arg
		elif opt in ("-o", "--audio_timecodes"):
			audio_timecodes = arg
		else:
			assert False, "unhandled option"
			print("option: ", opt)
			print(USAGE)
			sys.exit(2)

	##################################################################
	# READING THE XPATHS 
	##################################################################
	try:
		FILEin = open(resolve_xml_file, "rt")
	except IOError:
			print("File not found :", resolve_xml_file, " or path is incorrect")
	try:
		FILEout = open(audio_timecodes, "wt")
	except IOError:
			print("File not opened :", audio_timecodes, " or path is incorrect")

	doc = etree.parse(FILEin)

	print("************ name")
	e = doc.findall('sequence/media/audio/track/clipitem/name')
	for i in e:
		print(i.text)

	for Node in doc.xpath('//*'):
		if not Node.getchildren() and Node.text:
			xpath = doc.getpath(Node)
			# print(xpath, Node.text)
			res = re.match(r"/xmeml/sequence/media/audio/track\[([0-9]+)\]/clipitem\[([0-9]+)\]/([a-z]+)$", xpath)
			if(res) :
				print("track",res.groups()[0],"clip",res.groups()[1], "val", res.groups()[2],":", Node.text)
	
	line_string = "Track1,,,,,,,,Track2,,,,,,,,\n"
	FILEout.write(line_string)
	line_string = "clip name,duration,start,end,in,out,fx1,fx2,clip name,duration,start,end,in,out,fx1,fx2"
	for Node in doc.xpath('//*'):
		if not Node.getchildren() and Node.text:
			xpath = doc.getpath(Node)
			# print(xpath, Node.text)
			res = re.match(r"/xmeml/sequence/media/audio/track\[([0-9]+)\]/clipitem\[([0-9]+)\]/([a-z]+)$", xpath)
			if(res) :
				# print("track",res.groups()[0],"clip",res.groups()[1], "val", res.groups()[2],":", Node.text)
				if(res.groups()[2] == "name") :
					FILEout.write(line_string + "\n")
					#new track
					if(res.groups()[0] == "2") :
						line_string = ",,,,,,,,"
					else:
						line_string = ""
					line_string = line_string + Node.text
				elif(res.groups()[2] == "duration" or res.groups()[2] == "start" or res.groups()[2] == "end" or res.groups()[2] == "in" or res.groups()[2] == "out" ) :
					line_string = line_string + "," + framesToMnSec(Node.text)
	FILEout.write(line_string)
	FILEout.close()

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
