import sys
import pyautogui as pag
from time import time
from subprocess import Popen
import subprocess

from decimal import Decimal

# import asyncio
import threading
# OSC
from osc4py3.as_eventloop import *
from osc4py3 import oscmethod as osm
from osc4py3 import oscbuildparse

import argparse

##################################################################
# PARAMETERS THAT COULD BE INCLUDED IN A CONFIGURATION FILE

# OSC for pg
porphyrograph_IP = "127.0.0.1" # for porphyrograph
porphyrograph_Port = 9000 # for porphyrograph

# OSC for touchOSC 
# restore for Liepaja: touchOSC_IP = "192.168.0.102" # for touchOSC
touchOSC_IP = "127.0.0.1" # for touchOSC
touchOSC_Port = 8000 # for touchOSC 
# in case of two devices with touchOSC
# restore for Liepaja: touchOSC_second_IP = "192.168.0.103" # for touchOSC
touchOSC_second_IP = "" # for touchOSC
touchOSC_second = (touchOSC_second_IP != "")
touchOSC_second_Port = 8000 # for touchOSC 

# OSC server
OSCServer_Port = 7000  # python server

##################################################################
# OSC MESSAGES MANAGEMENT
##################################################################

##################################################################
# OSC MESSSAGE RECEPTION: DISPATCHER
def OSC_dispatcher_mapping():
    # Associate Python functions with message address patterns, using default
    # argument scheme OSCARG_DATAUNPACK.
    # SYSTEM MESSAGES PROCESSING
    osc_method("/launcher/*", OSC_receive_launcher_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATA)

##################################################################
# OSC MESSSAGE RECEPTION: FUNCTIONS
def OSC_receive_launcher_handler(address, args):
    # print("OSC launcher command", address, args)
    split_add = address[1:].split('/')
    command = split_add[1]
    # print("command", command)
    # ################# commands for launching apps
    if (command == "alkemi"):
        print("run alKemi")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/FermeBlanche-alkemi.PG-single.bat")
    elif (command == "CityWalk"):
        print("run CityWalk")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/FermeBlanche-CityWalk.PG-single.bat")
    elif (command == "Song"):
        print("run Song")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/FermeBlanche-Song.PG-single.bat")
    elif (command == "GN"):
        print("run GN")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/FermeBlanche-GN.PG-single.bat")
    elif (command == "Renoise"):
        print("run Renoise")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/launch_GN.renoise.bat")
    elif (command == "PureData"):
        print("run PureData")
        run_batch_file("C:/sync.com/Sync/LYM-projects/batFiles/launch_core.PD.bat")
    elif(command == "quit"):
        print("quit")
        # sys.exit()
    else:
        print("Unknown command", address, args)



##################################################################
##################################################################
# MAIN PROGRAM
##################################################################
##################################################################
def run_batch_file(file_path):
    Popen(file_path,creationflags=subprocess.CREATE_NEW_CONSOLE)

# OSC initialization
osc_startup()

# clients
print("The python program opens OSC client porphyrograph on Address %s Port: %d" % (porphyrograph_IP, porphyrograph_Port))
osc_udp_client(porphyrograph_IP, porphyrograph_Port, "porphyrograph_client")
print("The python program opens OSC client touchOSC on Address %s Port: %d" % (touchOSC_IP, touchOSC_Port))
osc_udp_client(touchOSC_IP, touchOSC_Port, "touchOSC_client")
if(touchOSC_second_IP != "") :
    print("The python program opens OSC client touchOSC_second on Address %s Port: %d" % (touchOSC_second_IP, touchOSC_second_Port))
    osc_udp_client(touchOSC_second_IP, touchOSC_second_Port, "touchOSC_second_client")
    touchOSC_second = True
# server
osc_udp_server("0.0.0.0", OSCServer_Port, "Local_server")
print("The python program opens OSC server on Port: %d" % (OSCServer_Port))

# CALLBACKS FOR OSC MESSAGES
# defines the callbacks for OSC messages
OSC_dispatcher_mapping()

# OSC_receive_light_control_handler("/light_control/light_group", (4,))
# OSC_receive_light_control_handler("/light_control/light_group", (2,))
while True:  # loop to wait till window close
    # osc messages reception
    for i in range(5) :
        osc_process()
