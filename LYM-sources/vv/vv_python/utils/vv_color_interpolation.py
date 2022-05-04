#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
import math

import colorsys

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

def mix_color (local_initial_color, local_target_color, local_percentage):
	# print( "initial_color target_color local_percentage:", local_initial_color,  "/", local_target_color, "/", local_percentage )
	initial_color = re.sub( '^\#', '', local_initial_color)
	target_color = re.sub( '^\#', '', local_target_color)
	target_red = '0x' + target_color[ 0 : 2 ]
	target_green = '0x' + target_color[ 2 : 4 ]
	target_blue = '0x' + target_color[ 4 : 6 ]
	initial_red = '0x' + initial_color[ 0 : 2 ]
	initial_green = '0x' + initial_color[ 2 : 4 ]
	initial_blue = '0x' + initial_color[ 4 : 6 ]
	mix_red = int((1 - local_percentage) *  int(initial_red, 0) + local_percentage *  int(target_red, 0))
	mix_green = int((1 - local_percentage) *  int(initial_green, 0) + local_percentage *  int(target_green, 0))
	mix_blue = int((1 - local_percentage) *  int(initial_blue, 0) + local_percentage *  int(target_blue, 0))
	mix_red = "%02x" % mix_red
	mix_green = "%02x" % mix_green
	mix_blue = "%02x" % mix_blue
	mixed_color = '#' + mix_red + mix_green + mix_blue
	return mixed_color

	# colortable_8_P1050715.txt
    # 544561: (  6,  2,  8) #060208 srgb(6,2,8)
    # 498341: ( 12, 13, 65) #0C0D41 srgb(12,13,65)
    # 759839: ( 31, 15,  5) #1F0F05 srgb(31,15,5)
    #  22412: ( 53, 38,100) #352664 srgb(53,38,100)
    #  76053: ( 67, 60,132) #433C84 srgb(67,60,132)
    #  18144: ( 85, 66,117) #554275 srgb(85,66,117)
    # 107829: (124, 84, 32) #7C5420 srgb(124,84,32)
    #  46421: (133,116, 73) #857449 srgb(133,116,73)
def main():
	for i in range(7):
		percentage = float(i)/6.0
		print( mix_color('#857449', '#8B4308', percentage), ',', mix_color('#7C5420', '#60546A', percentage), ',', mix_color('#554275', '#502002', percentage), ',', \
			mix_color('#433C84', '#46563F', percentage), ',', mix_color('#352664', '#3958A9', percentage), ',', mix_color('#1F0F05', '#1C0A01', percentage), ',', \
			mix_color('#0C0D41', '#071762', percentage) )

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	main()
