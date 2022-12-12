from __future__ import division  # we need floating division
from svg.path import Path, Line, Arc, CubicBezier, QuadraticBezier, parse_path
import pygame
import sys
import re
import math
import pyautogui as pag
from time import time

""" demo of using a great python module svg.path by Lennart Regebro
    see site: https:#pypi.org/project/svg.path/
    to draw svg in pygame
"""

# return_svg_attributes=True, or with the convenience function svg2paths2
from svgpathtools import svg2paths
from svgpathtools import parse_path
import xml.dom.minidom

from decimal import Decimal

# import asyncio
import threading
# OSC
from osc4py3.as_eventloop import *
from osc4py3 import oscmethod as osm
from osc4py3 import oscbuildparse

# ENTEC controller
from DMXEnttecPro import Controller

import csv
from os import environ

import argparse

##################################################################
# PARAMETERS THAT COULD BE INCLUDED IN A CONFIGURATION FILE

# DMX in Python (this program) or in porphyrograph
DMX_in_Python = False   
DMX1_Port = 'COM3'
# DMX2_Port = 'COM5'

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
OSC_emission_timeStep = 0.05

# create a font object.
# 1st parameter is the font file
# which is present in pygame.
# 2nd parameter is size of the font
pygame.font.init()
font_huge = pygame.font.SysFont('Noto Mono', 24)
font_large = pygame.font.SysFont('Noto Mono', 18)
font_medium = pygame.font.SysFont('Noto Mono', 14)
font_small = pygame.font.SysFont('Noto Mono', 12)
horiz_scale = 10
vertic_scale = 3
horiz_translate = 0
vertic_translate = 0
FPS = 10
frame_speed = 1./FPS
preceding_frame_speed = 0
time_out = time()
preceding_time_out = time_out
preceding_time_out_frame = 0

# arguments from bat file
svg_curves_file_name = ''
scenario_with_lights_file_name = ''
parser = argparse.ArgumentParser()
parser.add_argument("svg_curves_file_name")
parser.add_argument("scenario_with_lights_file_name")
args = parser.parse_args()
svg_curves_file_name = args.svg_curves_file_name
scenario_with_lights_file_name = args.scenario_with_lights_file_name
print("argument files", svg_curves_file_name, scenario_with_lights_file_name)
scenario_lights = []
scenario_palette_colors = []
scenario_palette_colors_names =[]
scenario_nb_palette_colors = 0
scenario_nb_light_groups = 0
scenario_light_groups = []

# all curve paths
curve_paths = []
# all curve intervals (a curve interval is an initial and a final x value)
curve_intervals = []
curve_inverval_is_reversed = []
# all curve labels
curve_labels = []
# all curve colors in the svg file
curve_colors = []
# all curve ranges (a curve range is a minimal and a maximal value)
curve_ranges = []
# curve_last_frame_update = []
# curve_previous_value_for_smooth_interpolation = []
currentCurveIndexInScene = 0

unique_curve_ranges = []

FrameNo = 0
intro_console_printed = False

######################################################
# scene data lists indexed by scene number

# the labels of the scenes from the SVG file
scene_labels = [] # each value is the label of the scene

# scene intervals sorted according to the first value of the uniong of the intervals
sorted_curve_scenes_intervals = [] # each value is a list of intervals from curve_intervals

# scene curve indices corresponding to the sorted scene intervales
sorted_curve_scenes_indices = [] # each value is a list of indices in curve_paths list

group_IDs = {}

########################################################
# current aniamtion scene management
active_scene = -1
alpha_union_intervals = 1.
cursor_location = 0

current_time = 0
active_scene_time_ini = 0
active_scene_time_end = 0

light_commands = ["dimmer", "strobe", "zoom", "pan", "tilt", "hue", "red", "green", "blue", "grey", "palette_color"]
# def command_rank(command_string) :
#     if(command_string == "dimmer"):
#         return _dimmer
#     elif(command_string == "strobe"):
#         return _strobe
#     elif(command_string == "zoom"):
#         return _zoom
#     elif(command_string == "pan"):
#         return _pan
#     elif(command_string == "tilt"):
#         return _tilt
#     elif(command_string == "hue"):
#         return _hue
#     elif(command_string == "red"):
#         return _red
#     elif(command_string == "green"):
#         return _green
#     elif(command_string == "blue"):
#         return _blue
#     elif(command_string == "grey"):
#         return _grey
#     elif(command_string == "palette_color"):
#         return _palette_color
#     return -1


##############################################################################
# LIGHT CLASS
class Light:
    ID = ""
    group = 1
    DMX_port = 1
    address = 0
    nb_channels = 1
    red_channel = 0     
    red_channel_fine = 0     
    green_channel = 0     
    green_channel_fine = 0     
    blue_channel = 0     
    blue_channel_fine = 0     
    grey_channel = 0     
    grey_channel_fine = 0     
    dimmer_channel = 0     
    dimmer_channel_fine = 0     
    strobe_channel = 0
    strobe_channel_fine = 0
    zoom_channel = 0
    zoom_channel_fine = 0
    pan_channel = 0
    pan_channel_fine = 0
    tilt_channel = 0
    tilt_channel_fine = 0
    hue_channel = 0
    hue_channel_fine = 0

    # working variables
    palette_color_value = 0. 
    grey_value = 0. 

    # init method or constructor 
    def __init__(self, id, gr, port, add, nb_chann, r_chann_string, g_chann_string, b_chann_string, grey_chann_string, dimm_chann_string, \
                 strobe_chann_string, zoom_chann_string, pan_chann_string, tilt_chann_string, hue_chann_string):
        self.ID = id
        self.group = gr - 1
        group_IDs[gr] = id
        self.DMX_port = port
        self.address = add
        self.nb_channels = nb_chann
        (self.red_channel, self.red_channel_fine) = SVG_channel_string_to_channel_no(r_chann_string)
        (self.green_channel, self.green_channel_fine) = SVG_channel_string_to_channel_no(g_chann_string)
        (self.blue_channel, self.blue_channel_fine) = SVG_channel_string_to_channel_no(b_chann_string)
        (self.grey_channel, self.grey_channel_fine) = SVG_channel_string_to_channel_no(grey_chann_string)
        (self.dimmer_channel, self.dimmer_channel_fine) = SVG_channel_string_to_channel_no(dimm_chann_string)
        (self.strobe_channel, self.strobe_channel_fine) = SVG_channel_string_to_channel_no(strobe_chann_string)
        (self.zoom_channel, self.zoom_channel_fine) = SVG_channel_string_to_channel_no(zoom_chann_string)
        (self.pan_channel, self.pan_channel_fine) = SVG_channel_string_to_channel_no(pan_chann_string)
        (self.tilt_channel, self.tilt_channel_fine) = SVG_channel_string_to_channel_no(tilt_chann_string)
        (self.hue_channel, self.hue_channel_fine) = SVG_channel_string_to_channel_no(hue_chann_string)
        self.palette_color_value = 0. 
        self.grey_value = 0. 

    def dmx_light_set_dimmer(self, dimmer):
        if (self.dimmer_channel > 0) :
            DMX_set_channel(self.dimmer_channel, self.dimmer_channel_fine, dimmer, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_strobe(self, strobe):
        if (self.strobe_channel > 0) :
            DMX_set_channel(self.strobe_channel, self.strobe_channel_fine, strobe, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_zoom(self, zoom):
        if (self.zoom_channel > 0) :
            DMX_set_channel(self.zoom_channel, self.zoom_channel_fine, zoom, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_pan(self, pan):
        if (self.pan_channel > 0) :
            DMX_set_channel(self.pan_channel, self.pan_channel_fine, pan, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_tilt(self, tilt):
        if (self.tilt_channel > 0) :
            DMX_set_channel(self.tilt_channel, self.tilt_channel_fine, tilt, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_hue(self, hue):
        if (self.hue_channel > 0) :
            DMX_set_channel(self.hue_channel, self.hue_channel_fine, hue, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_red(self, red):
        if (self.red_channel > 0) :
            DMX_set_channel(self.red_channel, self.red_channel_fine, red, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_green(self, green):
        if (self.green_green > 0) :
            DMX_set_channel(self.green_channel, self.green_channel_fine, green, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_blue(self, blue):
        if (self.blue_channel > 0) :
            DMX_set_channel(self.blue_channel, self.blue_channel_fine, blue, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_grey(self, grey):
        self.grey_value = grey
        if (self.grey_channel > 0) :
            DMX_set_channel(self.grey_channel, self.grey_channel_fine, grey, self.DMX_port)  # Sets DMX channel 1 to max 255
        [r, g, b] = compute_pulsed_palette_color(self.palette_color_value, grey)
        if (self.red_channel > 0) :
            DMX_set_channel(self.red_channel, self.red_channel_fine, r, self.DMX_port)  # Sets DMX channel 1 to max 255
        if (self.green_channel > 0) :
            DMX_set_channel(self.green_channel, self.green_channel_fine, g, self.DMX_port)  # Sets DMX channel 1 to max 255
        if (self.blue_channel > 0) :
            DMX_set_channel(self.blue_channel, self.blue_channel_fine, b, self.DMX_port)  # Sets DMX channel 1 to max 255
    def dmx_light_set_palette_color(self, palette_color):
        self.palette_color_value = palette_color
        [r, g, b] = compute_pulsed_palette_color(palette_color, self.grey_value)
        if (self.red_channel > 0) :
            DMX_set_channel(self.red_channel, self.red_channel_fine, r, self.DMX_port)  # Sets DMX channel 1 to max 255
        if (self.green_channel > 0) :
            DMX_set_channel(self.green_channel, self.green_channel_fine, g, self.DMX_port)  # Sets DMX channel 1 to max 255
        if (self.blue_channel > 0) :
            DMX_set_channel(self.blue_channel, self.blue_channel_fine, b, self.DMX_port)  # Sets DMX channel 1 to max 255
    def light_get_ID(self):
        return self.ID
    def light_get_group(self):
        return self.group
    def light_get_DMX_port(self):
        return self.DMX_port
 
##############################################################################
# LIGHT GROUP CLASS
class LightGroup:
    frozen_channels = None
    lights = None

    # init method or constructor 
    def __init__(self):
        self.frozen_channels = [False] * len(light_commands) 
        self.lights = []

    def freeze(self, channel):
        if not(channel in light_commands) :
            print("cannot freeze channel", channel)
        self.frozen_channels[channel] = True

    def is_frozen(self, channel):
        if(channel != "palette_color"): 
            return self.frozen_channels[channel]
        else: 
            return self.frozen_channels["red"]

##################################################################
# UTILITIES
def is_number_tryexcept(s):
    """ Returns True is string is a number. """
    try:
        float(s)
        return True
    except ValueError:
        return False

def non_empty_interval_intersection(i1, i2) :
    return max(i1[0], i2[0]) < min(i1[1], i2[1])

def to_num(val_init) :
    try:
        return_value = int(val_init)
    except ValueError:
        try:
            return_value = float(val_init)
        except ValueError:
            return_value = val_init
    return return_value

# computation of a RGB color if the value is a palette
def compute_pulsed_palette_color(palette_color, grey = 0) :
    global scenario_nb_palette_colors
    global scenario_palette_colors
    global scenario_palette_colors_names

    lowPalette = [0, 0, 0]
    indLowPalette = [0, 0, 0]
    indUpperPalette = [0, 0, 0]
    palette_color_percentage = [0, 0, 0]
    bandpass_3color_palette = [[0, 0, 0], [0, 0, 0], [0, 0, 0]]
    rgb_result_color = [0, 0, 0]

    for indColorBandPass in range(3):
        interpolation_color = palette_color
        interpolation_color *= scenario_nb_palette_colors
        palette_color_percentage[indColorBandPass] = interpolation_color - math.floor(interpolation_color) # decimal part of interpolation_color
        lowPalette[indColorBandPass] = math.floor(interpolation_color) # integer part of interpolation_color
        while (lowPalette[indColorBandPass] < 0) :
            lowPalette[indColorBandPass] += scenario_nb_palette_colors
        indLowPalette[indColorBandPass] = int(lowPalette[indColorBandPass]) % scenario_nb_palette_colors
        indUpperPalette[indColorBandPass] = (indLowPalette[indColorBandPass] + 1) % scenario_nb_palette_colors

    # the base palette is obtained by interpolating the two closest palettes according to palette_color
    for indColorBandPass in range(3):
        for indChannel in range(3):
            bandpass_3color_palette[indColorBandPass][indChannel] =  \
                (1. - palette_color_percentage[indColorBandPass]) * scenario_palette_colors[indLowPalette[indColorBandPass]][indColorBandPass * 3 + indChannel] \
                + (palette_color_percentage[indColorBandPass]) * scenario_palette_colors[indUpperPalette[indColorBandPass]][indColorBandPass * 3 + indChannel]
            bandpass_3color_palette[indColorBandPass][indChannel] = max(min(1., bandpass_3color_palette[indColorBandPass][indChannel]), 0.)

    # calculating the color from base luminance + palette colors modulated by the three frequence ranges
    for indChannel in range(3):
        # adding a base luminance
        rgb_result_color[indChannel] = grey
        for indColorBandPass in range(3):
            rgb_result_color[indChannel] += bandpass_3color_palette[indColorBandPass][indChannel] / 3.
        rgb_result_color[indChannel] = max(min(1., rgb_result_color[indChannel]), 0)

    return rgb_result_color

# SPLITS /light/GROUP/COMMAND INTO GROUP AND COMMAND
def split_address_into_elements(a_full_address) :
    global scenario_nb_light_groups

    if(a_full_address.startswith("/")) :
        stripped_address = a_full_address[1:]
    else :
        stripped_address = a_full_address
    [a_light_string, group_no_string, a_command] = stripped_address.split("/")

    # check prefix
    if(a_light_string != "light") :
        print("incorrect command (address) {:s}, should begin with \"/light\"".format(a_full_address))

    # check group no
    a_group_no = to_num(group_no_string)
    if(a_group_no <= 0 or a_group_no > scenario_nb_light_groups) :
        print("incorrect address {:s}, group_no should be >= 1 and <= {:d}".format(a_full_address, scenario_nb_light_groups))
        a_group_no = 0

    if not(a_command in light_commands) :
        print("incorrect address {:s}, unknown command {:s}".format(a_full_address, a_command))
        a_command = ""

    return [a_group_no, a_command]



##################################################################
# DMX IN PYTHON OR IN PG - VALUES SHIPPING
##################################################################

##################################################################
# sets channel value
def DMX_set_channel(channel_no, channel_fine_no, float_channel_value, universe_no) :
    global one_DMX1_value_changed
    global one_DMX2_value_changed
    global DMX_in_Python

    # print("DMX set channel", channel_no, channel_fine_no, float_channel_value, universe_no)

    channel = int(channel_no)
    universe = int(universe_no)
    if( channel >= 1 and channel <= 512) :
        channel_fine = int(channel_fine_no)
        if(channel_fine == 0) :
            char_channel_value =  max(0, min(255, int(float_channel_value * 255)))
            if(DMX_in_Python == True) :
                if(universe == 1) :
                    one_DMX1_value_changed = True
                    dmx1.set_channel(channel, char_channel_value)
                else :
                    one_DMX2_value_changed = True
                    # dmx2.set_channel(channel, char_channel_value)
            else :
                message = oscbuildparse.OSCMessage("/light_control/channel_value", ",ifi", [channel, float_channel_value, universe])
                osc_send(message, "porphyrograph_client") # porphyrograph forwards the message to touchOSC
                for i in range(0, 2):
                    osc_process()  
        else:
            ch_int = max(0, min(65535, int(float_channel_value * 65535)))
            char_channel_value =  ch_int >> 8
            char_channel_fine_value =  ch_int - char_channel_value * 256
            if(DMX_in_Python == True) :
                if(universe == 1) :
                    one_DMX1_value_changed = True
                    dmx1.set_channel(channel, char_channel_value)
                    dmx1.set_channel(channel_fine, char_channel_fine_value)
                else :
                    one_DMX2_value_changed = True
                    # dmx2.set_channel(channel, char_channel_value)
                    # dmx2.set_channel(channel_fine, char_channel_fine_value)
            else :
                message = oscbuildparse.OSCMessage("/light_control/channel_fine_value", ",ifi", [channel, float_channel_value, universe])
                osc_send(message, "porphyrograph_client") # porphyrograph forwards the message to touchOSC
                for i in range(0, 2):
                    osc_process()  

##################################################################
# DMX: SHIPS THE CURRENT VALUE
def DMX_value_shipping(address, output_value_list, is_color) :
    # builds the message for porphyrograph or touchOSC
    # 3 colors
    if(is_color) : 
        OSC_message_to_pg = oscbuildparse.OSCMessage("{:s}".format(address), ",fff", output_value_list)
    # one float
    else :
        OSC_message_to_pg = oscbuildparse.OSCMessage("{:s}".format(address), ",f", output_value_list)
    # print("memory", address, output_value_list)
    if(DMX_in_Python == True) :
        # the message is sent to touchOSC for feedback
        osc_send(OSC_message_to_pg, "touchOSC_client")
        if touchOSC_second :
            osc_send(OSC_message_to_pg, "touchOSC_second_client")
        for i in range(0, 2):
            osc_process()  
        [group_no, command] = split_address_into_elements(address)
        if(command != "") :
            DMX_setLightGroup_value(group_no, command, msg_art, True)
        else :
            print("Unknown light command in address", address)
    else :
        # print(OSC_message_to_pg, "porphyrograph_client") # porphyrograph forwards the message to touchOSC
        osc_send(OSC_message_to_pg, "porphyrograph_client") # porphyrograph forwards the message to touchOSC
        for i in range(0, 2):
            osc_process()  

##################################################################
# DMX: SETS THE VALUE OF A LIGHT GROUP
def DMX_setLightGroup_value(group_no, command, msg_arglist, is_animation) :
    global scenario_light_groups

    for light in scenario_light_groups[group_no - 1].lights :
        if(command == "dimmer") :
            light.dmx_light_set_dimmer(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "strobe") :
            light.dmx_light_set_strobe(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "zoom") :
            light.dmx_light_set_zoom(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "pan") :
            light.dmx_light_set_pan(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "tilt") :
            light.dmx_light_set_tilt(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "hue") :
            light.dmx_light_set_hue(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "red") :
            light.dmx_light_set_red(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "green") :
            light.dmx_light_set_green(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "blue") :
            light.dmx_light_set_blue(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "grey") :
            light.dmx_light_set_grey(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze(command)
        elif(command == "palette_color") :
            light.dmx_light_set_palette_color(msg_arglist[0])
            if not is_animation :
                scenario_light_groups[group_no - 1].freeze("red")



##################################################################
# ANIMATION - VALUES CALCULATION
##################################################################

##################################################################
# ANIMATION: DMX CHANNEL INITAL VALUES STORAGE
def DMX_initial_value_storage(curve_y_value, address) :
    global previous_address_value
    global previous_address_value_update_frame
    global FrameNo

    is_color = address.endswith("palette_color")

    # builds the values for the message from the current value given by the curve position
    # and turns it into a string for the display
    if(is_color) : 
        output_value_list = compute_pulsed_palette_color(curve_y_value)
    else : 
        output_value_list = [curve_y_value]

    # takes note of preceding and current value and memorizes the current value
    previous_address_value[address] = curve_y_value
    previous_address_value_update_frame[address] = FrameNo

##################################################################
# ANIMATION: INTERPOLATION FROM INITIAL VALUES TO THE BEGINNING OF A CURVE TO AVOID JUMPS IN VALUES
# def ANIMATION_Smooth_Interpolation_at_Curve_Beginning(is_new_value, prec_value, cur_value, is_color) :
#     global curve_labels
#     global curve_last_frame_update
#     global curve_previous_value_for_smooth_interpolation
#     global active_scene_time_ini
#     global active_scene_time_end
#     global current_time
#     global currentCurveIndexInScene

#     # manages an initial interpolation during the 5 first seconds of a new scene when the initial values
#     # do not correspond to the first values of the curves
#     # in order to avoid sudden lighting changes
#     # first memorizes the current value of a variable at the begginning of a scene (new value after a pause with no new value) 
#     if(is_new_value == True and curve_last_frame_update[currentCurveIndexInScene] == False) :
#         if(is_color) : 
#             if(max((abs(prec_value[0] - cur_value[0]), abs(prec_value[1] - cur_value[1])), abs(prec_value[2] - cur_value[2])) > 0.001) :
#                 # print("New value for curve {:s} at {:.2f} prec/cur {:.3f} {:.3f} {:.3f}/{:.3f} {:.3f} {:.3f}".format(curve_labels[currentCurveIndexInScene], cursor_location, prec_value[0], prec_value[1], prec_value[2], cur_value[0], cur_value[1], cur_value[2]))
#                 curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] = prec_value
#             else :
#                 # print("Ignored new value for curve {:s} at {:.2f} prec/cur {:.3f} {:.3f} {:.3f}/{:.3f} {:.3f} {:.3f}".format(curve_labels[currentCurveIndexInScene], cursor_location, prec_value[0], prec_value[1], prec_value[2], cur_value[0], cur_value[1], cur_value[2]))
#                 curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] = None
#         else : 
#             if(abs(prec_value[0] - cur_value[0]) > 0.001) :
#                 # print("New value for curve {:s} at {:.2f} prec/cur {:.3f}/{:.3f}".format(curve_labels[currentCurveIndexInScene], cursor_location, prec_value[0], cur_value[0]))
#                 curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] = prec_value
#             else :
#                 # print("Ignored new value for curve {:s} at {:.2f} prec/cur {:.3f}/{:.3f}".format(curve_labels[currentCurveIndexInScene], cursor_location, prec_value[0], cur_value[0]))
#                 curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] = None

#         curve_last_frame_update[currentCurveIndexInScene] = is_new_value

#     # interpolation in case of different initial scene value and initial var value
#     # print("current time {:.2f} scene time ini {:.2f} curve prev value {} ",current_time, active_scene_time_ini, curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] )
#     interpolated_value = cur_value
#     if current_time >= active_scene_time_ini and current_time <= active_scene_time_ini + 5 and curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene] != None:
#         alpha = (current_time - active_scene_time_ini) / 5
#         source = curve_previous_value_for_smooth_interpolation[currentCurveIndexInScene]
#         target = cur_value
#         is_new_value = True
#         if(is_color) : 
#             interpolated_value = [0] * 3
#             for ind in range(3) :
#                 interpolated_value[ind] = source[ind] * (1 - alpha) + target[ind] * alpha
#         else : 
#             interpolated_value = [source[0] * (1 - alpha) + target[0] * alpha]
    
#     return interpolated_value

##################################################################
# ANIMATION: COMPUTES THE VALUE OF A CURRENT POINT FROM A CURRENT PATH AND ITS INTERVAL 
# THIS INTERVAL IS BOUNDED BY THE MIN AND MAX VALUES OF THE CURRENT PATH
def ANIMATION_curvePath_current_x_y_values(current_interval, current_path) :
    global cursor_location
    global curve_inverval_is_reversed
    global curve_labels
    global curve_previous_value_for_smooth_interpolation
    global active_scene_time_ini
    global active_scene_time_end
    global currentCurveIndexInScene

    # current value is sent to porphyrograph for DMX message generation
    # computes the alpha value for the current curve interval as a function 
    # of the cursor location inside this interval
    alpha_bezier = 0
    x_interval_percent = (cursor_location - current_interval[0])/(current_interval[1] - current_interval[0])
    if(current_interval[1] > current_interval[0]) :
        alpha_bezier = x_interval_percent
        alpha_bezier = max(min(alpha_bezier, 1), 0)
    if(curve_inverval_is_reversed[currentCurveIndexInScene]) :
        alpha_bezier = 1. - alpha_bezier

    # print("alpha_bezier", alpha_bezier)

    # if we are inside the curve interval
    if(alpha_bezier != 0 and alpha_bezier != 1) :
        # binary search of the alpha value corresponding to the current abscisse
        # and corresponding y value
        cur_point = current_path.point(alpha_bezier)
        ini_point = current_path.point(0)
        end_point = current_path.point(1)

        # initialization
        alpha_low = 0.
        alpha_high = 1.
        if not curve_inverval_is_reversed[currentCurveIndexInScene] :
            if(cur_point.real > cursor_location) :
                alpha_low = 0.
                alpha_high = alpha_bezier  
            else :
                alpha_low = alpha_bezier
                alpha_high = 1.
        else :
            if(cur_point.real < cursor_location) :
                alpha_low = 0.
                alpha_high = alpha_bezier  
            else :
                alpha_low = alpha_bezier
                alpha_high = 1.

        n = 0
        # iterative search
        alpha_mid = alpha_bezier
        # print("new search %s" %(curve_labels[currentCurveIndexInScene]))
        while abs(cur_point.real - cursor_location) > 0.01 and n < 20 :
            alpha_mid = alpha_low + (alpha_high - alpha_low)/2
            # print("low mid high cur_point cursor %.4f %.4f %.4f %.4f %.4f " % (alpha_low, alpha_mid, alpha_high, cur_point.real, cursor_location))    
            cur_point = current_path.point(alpha_mid)
            if not curve_inverval_is_reversed[currentCurveIndexInScene] :
                if(cur_point.real > cursor_location) :
                    alpha_high = alpha_mid  
                else:
                    alpha_low = alpha_mid
            else :
                if(cur_point.real < cursor_location) :
                    alpha_high = alpha_mid  
                else:
                    alpha_low = alpha_mid
            n += 1

        # if(n >= 100) :
        #     sys.exit()
        alpha_bezier = alpha_mid

    cur_point = current_path.point(alpha_bezier)
    # print("current point", cur_point)
    # cur_point_y = 1. - cur_point.imag/10.

    # y_value = max(0, min(1.0, cur_point_y))

    cur_point_y = cur_point.imag/10.
    # print("cur_point_y", cur_point_y)
    cur_point_y_int = math.floor(cur_point_y)
    cur_point_y_dec = (cur_point_y - cur_point_y_int)
    # deals with the special case of 0 and 1 values which, since they are 
    # both integer, both yield 0 with cur_point_y - cur_point_y_int
    y_value = 0.
    # small values above unit are ignored due to possible lack of precision in the computation
    if(cur_point_y_dec > 0.00001) :
        y_value = 1. - cur_point_y_dec
        # print("dec", cur_point_y_dec)
    elif int(cur_point_y_int) % 2 == 0 :
        # print("int", int(cur_point_y_int))
        y_value = 1.
    else :
        # print("int", int(cur_point_y_int))
        y_value = 0.

    return(x_interval_percent, y_value)

##################################################################
# ANIMATION AND DISPLAY: DRAWING (SCENE RANGES, CURVE RANGES, CURVES, INITIAL INFO
def ANIMATION_DISPLAY_update_values_and_redraw() :
    global sorted_curve_scenes_intervals
    global sorted_curve_scenes_indices
    global active_scene
    global alpha_union_intervals
    global horiz_translate
    global horiz_scale
    global vertic_translate
    global font_small
    global font_medium
    global font_large
    global unique_curve_ranges
    global scene_labels
    global cursor_location
    global curve_ranges
    global curve_intervals
    global curve_labels
    global curve_paths
    global curve_colors
    global current_time
    global active_scene_time_ini
    global active_scene_time_end
    global currentCurveIndexInScene
    global previous_address_value
    global previous_address_value_update_frame
    global FrameNo
    global frame_speed
    global preceding_frame_speed
    global time_out
    global preceding_time_out
    global time_out_frame
    global preceding_time_out_frame
    global scene_verbatims_screen
    global scene_verbatims_console
    global intro_console_printed

    # drawing scenes ranges
    for indScene, an_interval in enumerate(sorted_curve_scenes_intervals):
        if indScene == active_scene:
            if alpha_union_intervals < 1 :
                pygame.draw.rect( DISPLAYSURF, pygame.Color(30, 15, 16, 64), 
                                    pygame.Rect(horiz_translate + an_interval[0] * horiz_scale, 0, (an_interval[1] - an_interval[0]) * horiz_scale, screen_height * 0.97), 
                                    width=0)
                pygame.draw.rect( DISPLAYSURF, pygame.Color(90, 16, 64), 
                                    pygame.Rect(horiz_translate + an_interval[0] * horiz_scale, 0, (an_interval[1] - an_interval[0]) * horiz_scale, screen_height * 0.97), 
                                    width=1)
            else :
                pygame.draw.rect( DISPLAYSURF, pygame.Color(30, 15, 30, 64), 
                                    pygame.Rect(horiz_translate + an_interval[0] * horiz_scale, 0, (an_interval[1] - an_interval[0]) * horiz_scale, screen_height * 0.97), 
                                    width=0)
                pygame.draw.rect( DISPLAYSURF, pygame.Color(90, 15, 30, 64), 
                                    pygame.Rect(horiz_translate + an_interval[0] * horiz_scale, 0, (an_interval[1] - an_interval[0]) * horiz_scale, screen_height * 0.97), 
                                    width=1)
        else:
            pygame.draw.rect( DISPLAYSURF, pygame.Color(30, 30, 15, 64), 
                                pygame.Rect(horiz_translate + an_interval[0] * horiz_scale, 0, (an_interval[1] - an_interval[0]) * horiz_scale, screen_height * 0.97), 
                                width=0)
        if indScene == active_scene:
            length = an_interval[1] - an_interval[0]
            text = "{:.2f} ({:.2f})".format(length, length * (1.0 - alpha_union_intervals))
            if alpha_union_intervals < 1 :
                text_img = font_small.render(text, True, (255,200,200))
            else :
                text_img = font_small.render(text, True, (255,200,255))
        else :
            text = "{:.2f}".format(an_interval[1] - an_interval[0])
            text_img = font_small.render(text, True, (128,128,128))
        DISPLAYSURF.blit(text_img, [horiz_translate + an_interval[0] * horiz_scale + 50, screen_height * 0.94])

    # drawing curve ranges
    for curve_range in unique_curve_ranges:
        pygame.draw.rect( DISPLAYSURF, pygame.Color(30, 30, 60, 128), 
                            pygame.Rect(0, curve_range[0] * vertic_scale + vertic_translate, screen_width, (curve_range[1] - curve_range[0]) * vertic_scale), 
                            width=0)
        pygame.draw.rect( DISPLAYSURF, pygame.Color(60, 60, 60, 128), 
                            pygame.Rect(0, curve_range[0] * vertic_scale + vertic_translate, screen_width, (curve_range[1] - curve_range[0]) * vertic_scale), 
                            width=1)

    # drawing scenes, highlighting the active one, sending the values of the active scene
    nb_displayed_messages = 0
    for indScene, (a_curve_scene_indices, an_interval, scene_label) in enumerate(zip(sorted_curve_scenes_indices, sorted_curve_scenes_intervals, scene_labels)):
        # location of the cursor in the active scene
        if indScene == active_scene:
            alpha_union_intervals = 0
            if(active_scene_time_end > active_scene_time_ini) :
                alpha_union_intervals = max(min((current_time - active_scene_time_ini)/(active_scene_time_end - active_scene_time_ini), 1), 0)
            cursor_location = an_interval[0] + alpha_union_intervals * (an_interval[1] - an_interval[0])
            # sends scene percentage to touchOSC
            message = oscbuildparse.OSCMessage("/light_control/light_scene_percent", ",f", [int(alpha_union_intervals * 100)])
            osc_send(message, "touchOSC_client") # resets the speed down control
            if touchOSC_second :
                osc_send(message, "touchOSC_second_client")
            for i in range(0, 2):
                osc_process()  
            # notes the time if the end of the current scene is reached
            if(alpha_union_intervals >= 1 or alpha_union_intervals <= 0) :
                if(preceding_time_out_frame != FrameNo - 1) :
                    time_out = time()
                    print("time", time_out - preceding_time_out)
                    preceding_time_out = time_out
                    preceding_frame_speed = frame_speed
                preceding_time_out_frame = FrameNo

        # scene label
        if indScene == active_scene:
            if alpha_union_intervals < 1 :
                text_img = font_huge.render(scene_label, True, (255,200,200))
            else :
                text_img = font_huge.render(scene_label, True, (255,200,255))
        else :
            text_img = font_huge.render(scene_label, True, (128,128,128))
        x = horiz_translate + (an_interval[0]+(an_interval[1] - an_interval[0]) * 0.3) * horiz_scale
        first_curve_index = a_curve_scene_indices[0]
        # vertical position is computed fr
        y_max = curve_ranges[first_curve_index][0]
        for curveIndInScene in a_curve_scene_indices[1:]:
            y_max = max(y_max, curve_ranges[curveIndInScene][0])
        y_max = (y_max + (curve_ranges[first_curve_index][1] - curve_ranges[first_curve_index][0]) * 1.5)
        if indScene == active_scene:
            if(y_max * vertic_scale > screen_height/2) :
                vertic_translate = - ((y_max * vertic_scale - screen_height * 0.75) // 30 * 30)
            else :
                vertic_translate = 0
        DISPLAYSURF.blit(text_img, [x, vertic_translate + y_max * vertic_scale])

        # drawing curves 
        # print("scene: ", an_interval)
        for curveIndInScene in a_curve_scene_indices:
            # sets the global variable of the curve index in the current scene
            currentCurveIndexInScene = curveIndInScene
            interval = curve_intervals[currentCurveIndexInScene]
            currentCurvePath = curve_paths[currentCurveIndexInScene]
            line_color = curve_colors[currentCurveIndexInScene]
            if indScene == active_scene:
                if alpha_union_intervals < 1 :
                    line_color = (255,100,100)
                else :
                    line_color = (255,100,255)

            # svg.currentCurvePath point method returns a complex number p, p.real and p.imag can pull the x, and y
            n = 100  # number of line segments to draw
            pts = [ (horiz_translate + p.real*horiz_scale, vertic_translate + p.imag*vertic_scale) for p in (currentCurvePath.point(i/n) for i in range(0, n+1))]  
            pygame.draw.aalines( DISPLAYSURF,pygame.Color(line_color), False, pts) #, width=5)  # False is no closing
            x = horiz_translate + interval[0] * horiz_scale
            y = vertic_translate + (curve_ranges[currentCurveIndexInScene][0] + 3) * vertic_scale
            text = curve_labels[currentCurveIndexInScene]
            if indScene == active_scene:
                if alpha_union_intervals < 1 :
                    text_img = font_small.render(text, True, (255,200,200))
                else :
                    text_img = font_small.render(text, True, (255,200,255))
            else :
                text_img = font_small.render(text, True, (128,128,128))
            DISPLAYSURF.blit(text_img, [x, y])

            # animation: sending the values of parameters of the active scene
            # by computing the current light channel values
            if indScene == active_scene:
                # print("curve indices, curve index, label", a_curve_scene_indices, currentCurveIndexInScene, curve_labels[currentCurveIndexInScene])
                (x_interval_percent, y_value) = ANIMATION_curvePath_current_x_y_values(interval, currentCurvePath)
                # print("animation", x_interval_percent, y_value, currentCurvePath)
                # DMX channel value modification
                # or OSC message if DMX is in porphyrograph
                address = curve_labels[currentCurveIndexInScene]
                (is_new_value, is_color, output_value_list, string_value, address_value) = DISPLAY_current_x_y_values_string(x_interval_percent, y_value, address, True, False)
                if(is_new_value) :
                    previous_address_value[address] = address_value
                    DMX_value_shipping(address, output_value_list, is_color)
                    # print("shipping", address, output_value_list, is_color)
                previous_address_value_update_frame[address] = FrameNo


                x = screen_width * 0.01
                y = screen_height * 0.9 - nb_displayed_messages * 24
                astrripped_label = curve_labels[currentCurveIndexInScene][1:]
                [a_light_string, group_no_string, a_command] = astrripped_label.split("/")
                text = "{:<2}: {:<20} {:<7} {:<3}".format(group_no_string, group_IDs[to_num(group_no_string)], a_command, string_value)

                # displays the value at the bottom of the screen
                if is_new_value :
                    text_img = font_medium.render(text, True, (255,255,255))
                # static value only displayed greyed
                else :
                    text_img = font_medium.render(text, True, (200,200,200))

                DISPLAYSURF.blit(text_img, [x, y])
                nb_displayed_messages += 1

    # infos on file
    if active_scene == -1:
        text="File "+svg_curves_file_name+" loaded"
        text_img = font_medium.render(text, True, (255,255,255))
        DISPLAYSURF.blit(text_img, [screen_width * 0.05, screen_height * 0.63])
        if(not intro_console_printed) :
            print(text)
        text='{:d} scenes'.format(len(sorted_curve_scenes_intervals))
        text_img = font_medium.render(text, True, (255,255,255))
        DISPLAYSURF.blit(text_img, [screen_width * 0.05, screen_height * 0.66])
        if(not intro_console_printed) :
            print(text)
        for indScene, (text_screen, text_console) in enumerate(zip(scene_verbatims_screen, scene_verbatims_console)):
            text_img = font_small.render(text_screen, True, (255,255,255))
            last_line = screen_height * (0.70 + 0.02 * indScene)
            DISPLAYSURF.blit(text_img, [screen_width * 0.05, last_line])
            if(not intro_console_printed) :
                print(text_console)
        intro_console_printed = True

        line_no = 2
        text_img = font_small.render("space bar: start/stop, jump to next cue", True, (200,200,255))
        DISPLAYSURF.blit(text_img, [screen_width * 0.25, screen_height * 0.66 + screen_height * 0.04 * line_no])
        line_no += 1
        text_img = font_small.render("right/left arrow: jump to next/preceding cue", True, (200,200,255))
        DISPLAYSURF.blit(text_img, [screen_width * 0.25, screen_height * 0.66 + screen_height * 0.04 * line_no])
        line_no += 1
        text_img = font_small   .render("up/down arrow: speed up/down", True, (200,200,255))
        DISPLAYSURF.blit(text_img, [screen_width * 0.25, screen_height * 0.66 + screen_height * 0.04 * line_no])
        line_no += 1

    # time cursor
    pygame.draw.line( DISPLAYSURF,pygame.Color('green'), [horiz_translate + cursor_location * horiz_scale, 0], [horiz_translate + cursor_location * horiz_scale, screen_height]) #, width=5)  # False is no closing
    pygame.display.update() # copy DISPLAYSURF to display


##################################################################
# DISPLAY - INTERFACE VISUALIZATION
##################################################################

##################################################################
# DISPLAY: BUILDS THE STRING OF THE DISPLAY OF CURRENT VALUE
def DISPLAY_current_x_y_values_string(x_interval_percent, curve_y_value, address, is_animation, is_first_value) :
    global DMX_in_Python
    global previous_address_value
    global scenario_light_groups
    global currentCurveIndexInScene

    # print("DISPLAY_current_x_y_values_string", value, address, is_animation, is_first_value)

    is_color = address.endswith("palette_color")
    [group_no, command] = split_address_into_elements(address)

    # takes note of preceding and current value and memorizes the current value
    preceding_value = 0.
    if(address in previous_address_value.keys()) :
        preceding_value = previous_address_value[address]
        # print("preceding value ", preceding_value)
    preceding_value_Frame = FrameNo - 1
    if(address in previous_address_value_update_frame.keys()) :
        preceding_value_Frame = previous_address_value_update_frame[address]
        # print("preceding value frame", preceding_value_Frame)

    # this value is not changed for this address, displayed in grey and not sent
    is_new_value = False
    # if it is a new value, ships it to touchOSC or porphyrograph depending on whether DMX is implemented in python or pg
    # a new value is either a first value or a value 
    output_value = curve_y_value
    if(is_animation) :
        is_new_value = (curve_y_value != preceding_value)
        # print("curve value", curve_y_value)
        # smooth interpolation in case of discontinuity
        if(abs(curve_y_value - preceding_value) > maximal_value_step_between_two_frames and preceding_value_Frame == FrameNo - 1) :
            if(curve_y_value > preceding_value) :
                output_value = preceding_value + maximal_value_step_between_two_frames
            else :
                output_value = preceding_value - maximal_value_step_between_two_frames
            # print("smooth interp address, is_new_value, preceding_value, curve_y_value, output_value", address, is_new_value, preceding_value, curve_y_value, output_value)
        # print("smooth interp address, is_new_value, preceding_value, output_value_list, is_color", address, is_new_value, preceding_value, output_value_list, is_color)
        # output_value_list = ANIMATION_Smooth_Interpolation_at_Curve_Beginning(is_new_value, preceding_value, output_value_list, is_color)
    else :
        is_new_value = (is_first_value == True) or (curve_y_value != preceding_value and not scenario_light_groups[group_no - 1].is_frozen(command))

    x_interval_percent = min(1.,max(0.,x_interval_percent))
    # builds the values for the message from the current value given by the curve position
    # and turns it into a string for the display
    if(is_color) : 
        output_value_list = compute_pulsed_palette_color(output_value)
        string_value = "{:.3f} {:.3f} {:.3f}".format(output_value_list[0], output_value_list[1], output_value_list[2])
    else : 
        output_value_list = [output_value]
        string_value = "{:.3f}".format(output_value)
    # builds the string to be displayed
    if(is_color) : 
        string_value = "{:.3f} {:.3f} {:.3f} ({:d}%)".format(output_value_list[0], output_value_list[1], output_value_list[2], int(x_interval_percent*100))
    else : 
        string_value = "{:.3f} ({:d}%)".format(output_value_list[0], int(x_interval_percent*100))

    return (is_new_value, is_color, output_value_list, string_value, output_value)

##################################################################
# OSC MESSAGES MANAGEMENT
##################################################################

##################################################################
# OSC MESSSAGE RECEPTION: DISPATCHER
def OSC_dispatcher_mapping():
    # Associate Python functions with message address patterns, using default
    # argument scheme OSCARG_DATAUNPACK.
    # SYSTEM MESSAGES PROCESSING
    osc_method("/light_control/*", OSC_receive_light_control_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATA)
    osc_method("/light/*", OSC_receive_light_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)
    osc_method("/light_pulse/*", OSC_receive_light_pulse_handler, argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_DATAUNPACK)

##################################################################
# OSC MESSSAGE RECEPTION: FUNCTIONS
def OSC_receive_light_control_handler(address, args):
    global scenario_nb_light_groups
    global previous_address_value
    global previous_address_value_update_frame
    global FrameNo

    # print("OSC light control command", address, args)
    split_add = address[1:].split('/')
    command = split_add[1]
    # print("command", command)
    # ################# commands for local light scene management
    if(command == "quit"):
        # print("quit")
        sys.exit()
    elif (command == "play_pause"):
        # print("OSC_play_pause")
        OSC_play_pause()
    elif (command == "light_scene_minus"):
        # print("light_scene_minus")
        OSC_previous_light_scene()
    elif (command == "light_scene_plus"):
        # print("light_scene_plus")
        OSC_next_light_scene(False)
    elif (command == "light_scene_plus_start"):
        # print("light_scene_plus")
        OSC_next_light_scene(True)
    elif (command == "speed_up_step"):
        # print("OSC_speed_up_step")
        OSC_speed_up_step()
    elif (command == "speed_down_step"):
        # print("OSC_speed_down_step")
        OSC_speed_down_step()
    elif (command == "speed_up"):
        # print("OSC_speed_up")
        OSC_speed_up(args[0])
        message = oscbuildparse.OSCMessage("/light_control/speed_down", ",f", [1.])
        osc_send(message, "touchOSC_client") # resets the speed down control
        if touchOSC_second :
            osc_send(message, "touchOSC_second_client")
        for i in range(0, 2):
            osc_process()  
    elif (command == "speed_down"):
        # print("OSC_speed_down")
        OSC_speed_down(args[0])
        message = oscbuildparse.OSCMessage("/light_control/speed_up", ",f", [1.])
        osc_send(message, "touchOSC_client") # resets the speed up control
        if touchOSC_second :
            osc_send(message, "touchOSC_second_client")
        for i in range(0, 2):
            osc_process()  
    elif (command == "light_scene"):
        # print("light_scene", int(args[0]))
        OSC_go_to_light_scene(int(args[0]), False)
    elif (command == "light_scene_start"):
        # print("light_scene", int(args[0]))
        OSC_go_to_light_scene(int(args[0]), False)
        # print("OSC_play_pause")
        OSC_play_pause()
    # ################# commands forwarded to porphyrograph
    # DMX reset to 0
    elif (command == "zero_light"):
        OSC_zero_light()
    # direct channel value for tests
    elif (command == "channel_value"):
        if(len(args) == 3) :
            DMX_set_channel(args[0], 0, args[1], args[2])
    elif (command == "channel_fine_value"):
        if(len(args) == 3) :
            DMX_set_channel(args[0], args[0] + 1, args[1], args[2])
    # direct light group values for test also
    # reships all the values of the light group
    elif (command == "light_group"):
        group_no = int(args[0]) + 1
        if group_no >= 1:
            # print(previous_address_value)
            # reships all the light command values of this light group
            for light_command in light_commands :
                is_color = (light_command == "palette_color")
                address = "/light/{:d}/{:s}".format(group_no, light_command)
                if address in previous_address_value.keys() :
                    print("previous_address_value[address][0]", previous_address_value[address][0], address)
                    (is_new_value, is_color, output_value_list, string_value, address_value) \
                        = DISPLAY_current_x_y_values_string(0, previous_address_value[address][0], address, False, True)
                else :
                    (is_new_value, is_color, output_value_list, string_value, address_value) \
                        = DISPLAY_current_x_y_values_string(0, 0, address, False, True)
                if(is_new_value) :
                    previous_address_value[address] = address_value
                    DMX_value_shipping(address, output_value_list, is_color)
                    # print("shipping light_group", address, output_value_list, is_color)
                previous_address_value_update_frame[address] = FrameNo
   # direct light group values for test also
    elif (command.startswith("random_")):
        DMX_value_shipping(address, args, False) 
        # print("shipping random_", address, args)

def OSC_receive_light_handler(address, arg):
    global DMX_in_Python
    global previous_address_value
    global previous_address_value_update_frame
    global FrameNo

    # print("OSC light channel value command", address, arg)
    (is_new_value, is_color, output_value_list, string_value) = DISPLAY_current_x_y_values_string(0, arg, address, False, False)
    if(is_new_value) :
        previous_address_value[address] = address_value
        DMX_value_shipping(address, output_value_list, is_color)
        # print("shipping light handler", address, output_value_list, is_color)
    previous_address_value_update_frame[address] = FrameNo
    
def OSC_receive_light_pulse_handler(address, arg):
    # print("shipping light pulse handler", address, [arg])
    DMX_value_shipping(address, [arg], False)

##################################################################
# OSC: RECEPTION OF CONTROLS FROM INTERFACE
def OSC_go_to_light_scene(ind_scene, start) :
    global active_scene
    global scene_labels
    global frame_speed
    global preceding_frame_speed
    global time_out
    global preceding_time_out
    global alpha_union_intervals
    global current_union_interval
    global active_scene_time_ini
    global active_scene_time_end
    global current_time
    global sorted_curve_scenes_intervals
    global sorted_curve_scenes_indices
    if ind_scene < len(sorted_curve_scenes_indices):
        active_scene = ind_scene
        current_union_interval = sorted_curve_scenes_intervals[active_scene]
        active_scene_time_ini = current_time
        active_scene_time_end = current_time + current_union_interval[1] - current_union_interval[0]
        alpha_union_intervals = 0
        if(start == True):
            frame_speed = 1./FPS
        else:
            frame_speed = 0
        if preceding_frame_speed != frame_speed :
            time_out = time()
            print("time", time_out - preceding_time_out)
            preceding_time_out = time_out
            preceding_frame_speed = frame_speed

        # if(DMX_in_Python == True) :
        OSC_message_to_touchOSC = oscbuildparse.OSCMessage("/light_control/light_scene_label", ",s", [scene_labels[active_scene]])
        osc_send(OSC_message_to_touchOSC, "touchOSC_client")
        if touchOSC_second :
            osc_send(OSC_message_to_touchOSC, "touchOSC_second_client")
        if(ind_scene < len(sorted_curve_scenes_indices) - 1) :
            OSC_message_to_touchOSC = oscbuildparse.OSCMessage("/light_control/light_scene_label_next", ",s", [scene_labels[active_scene + 1]])
        else :
            OSC_message_to_touchOSC = oscbuildparse.OSCMessage("/light_control/light_scene_label_next", ",s", "END")
        osc_send(OSC_message_to_touchOSC, "touchOSC_client")
        if touchOSC_second :
            osc_send(OSC_message_to_touchOSC, "touchOSC_second_client")
        # print("touchOSC", OSC_message_to_touchOSC)
        for i in range(0, 2):
            osc_process()  

def OSC_speed_up_step() :
    global frame_speed
    frame_speed /= 0.9

def OSC_speed_down_step() :
    global frame_speed
    frame_speed *= 0.9

def OSC_speed_up(factor) :
    global frame_speed
    if(factor >= 1) :
        frame_speed = (1./FPS) * factor

def OSC_speed_down(factor) :
    global frame_speed
    if(factor >= 1) :
        frame_speed = (1./FPS) / factor

def OSC_previous_light_scene() :
    global active_scene
    if active_scene > 0 :
        OSC_go_to_light_scene(active_scene - 1, False) 

def OSC_next_light_scene(start) :
    global active_scene
    OSC_go_to_light_scene(active_scene + 1, start)

def OSC_play_pause() :
    global alpha_union_intervals
    global frame_speed
    global preceding_frame_speed
    global time_out
    global preceding_time_out
    global FPS
    # at the end of scene goes to next
    if alpha_union_intervals == 1:
        OSC_next_light_scene(True)
    # inside scene starts/stops
    else:
        if frame_speed != 0 :
            frame_speed = 0
        else :
            frame_speed = 1./FPS
        if preceding_frame_speed != frame_speed :
            time_out = time()
            print("time", time_out - preceding_time_out)
            preceding_time_out = time_out
            preceding_frame_speed = frame_speed

def OSC_zero_light() :
    global one_DMX1_value_changed
    global one_DMX2_value_changed
    global scenario_lights

    if(DMX_in_Python == True) :
        for light in scenario_lights:
            light.dmx_light_set_palette_color(0.)
            light.dmx_light_set_grey(0.)
            light.dmx_light_set_red(0.)
            light.dmx_light_set_green(0.)
            light.dmx_light_set_blue(0.)
            light.dmx_light_set_dimmer(0.)
            light.dmx_light_set_strobe(0.)
            light.dmx_light_set_zoom(0.)
            light.dmx_light_set_pan(0.5)
            light.dmx_light_set_tilt(0.5)
            light.dmx_light_set_hue(0.)
        one_DMX1_value_changed = True
    else :
        message = oscbuildparse.OSCMessage("/light_control/OSC_zero_light", ",f", [0.])
        osc_send(message, "porphyrograph_client")
        for i in range(0, 2):
            osc_process()  

##################################################################
# SVG (CURVES) AND CSV (SCENARIO) FILES LOADING 
##################################################################

##################################################################
# SVG CURVE SCENARIO FILE READING
def SVG_channel_string_to_channel_no(a_string) :
    if(a_string.find('/')!=-1) :
        split_chann = a_string.split('/')
        if(len(split_chann) != 2) :
            print("Unexpected number of channels (2 expected)", a_string)
        return (to_num(split_chann[0]), to_num(split_chann[1]))
    else :
        return (to_num(a_string), 0)

##################################################################
# SVG parse the curves and their labels
def SVG_parse_light_channel_curves() :
    global curve_paths
    global curve_intervals
    global curve_labels
    global curve_colors
    global curve_ranges
    # global curve_last_frame_update
    # global curve_previous_value_for_smooth_interpolation
    global scene_labels
    global curve_inverval_is_reversed
    global sorted_curve_scenes_intervals
    global sorted_curve_scenes_indices

    paths, attributes = svg2paths(svg_curves_file_name)

    # looks for all the paths which have been found in the SVG file
    for indPath, (path, att) in enumerate(zip(paths, attributes)):
        # print("Path #", indPath, ", ID: ", att['inkscape:label'], ", path: ", att['d'])  # print d-string of indPath-th path in SVG
        # print("ID: ", att['id'])  # print d-string of indPath-th path in SVG
        # only selects the genuine paths, not the rectangles which are also returned
        if(not att['id'].startswith("rect")) :
            # print("Label: ", att['inkscape:label'])  # print d-string of indPath-th path in SVG
            svgpath = att['d']
            path = parse_path(svgpath)
            # print(svgpath)
            # print(path)
            curve_paths.append(path)
            curve_labels.append(att['inkscape:label'])
            # curve_last_frame_update.append(False)
            # curve_previous_value_for_smooth_interpolation.append(None)
            match = re.findall("stroke\:\#([0-9a-fA-F]+)\;", att['style'])
            if(match != None) :
                rgb = tuple(int(match[0][i:i+2], 16) for i in (0, 2, 4))
                curve_colors.append(rgb)
            else:
                curve_colors.append((255, 255, 255))

            # extacts the list of the control points (and line points and H/V line points)
            # and makes a list of them
            point_list = []
            point_string_list = svgpath.split(" ")
            new_point = (0, 0)
            cur_point = (0, 0)
            horiz = False
            vertic = False
            absolute = True
            is_first_point = True
            for substring in point_string_list :
                # genuine point (two coordinates separated by a comma)
                if(substring.find(',') != -1):
                    a_point = substring.split(",")
                    new_point = tuple(map(lambda x: float(x), a_point))
                    if is_first_point or absolute :
                        new_point = new_point # nothing to do
                    else :
                        new_point = (cur_point[0]+new_point[0], cur_point[1]+new_point[1])
                    point_list.append(new_point)
                    is_first_point = False
                # horizontal line, only X coordinate
                elif (substring == "H"):
                    horiz = True
                    vertic = False
                    absolute = True
                    cur_point = new_point
                # vertical line, only Y coordinate
                elif (substring == "V"):
                    horiz = False
                    vertic = True
                    absolute = True
                    cur_point = new_point
                # horizontal line, only X coordinate
                elif (substring == "h"):
                    horiz = True
                    vertic = False
                    absolute = False
                    cur_point = new_point
                # vertical line, only Y coordinate
                elif (substring == "v"):
                    horiz = False
                    vertic = True
                    absolute = False
                    cur_point = new_point
                # relative move or curve
                elif (substring == "m" or substring == "c" or substring == "l" ):
                    horiz = False
                    vertic = False
                    absolute = False
                    cur_point = new_point
                # absolute move or curve
                elif (substring == "M" or substring == "C" or substring == "L" ):
                    horiz = False
                    vertic = False
                    absolute = True
                    cur_point = new_point
                # single number for horizontal or vertical line
                elif is_number_tryexcept(substring) == True:
                    if(horiz):
                        if is_first_point or absolute :
                            new_point = (float(substring), cur_point[1])
                        else :
                            new_point = (cur_point[0]+float(substring), cur_point[1])
                        point_list.append(new_point)
                        is_first_point = False
                    elif(vertic):
                        if is_first_point or absolute :
                            new_point = (cur_point[0], float(substring))
                        else :
                            new_point = (cur_point[0], cur_point[1]+float(substring))
                        point_list.append(new_point)
                        is_first_point = False
                # nothing of the other ones (Move, Curve...)
                else:
                    print("*********** Unexpected symbol ********** :", substring)
                    vertic = False
                    horiz = False

            # checks whether the curve is with an increasing X order 
            # if not, reverses the list and exchanges first and last coordinates so that 
            # the interval is in an increasing order
            first_point_x = point_list[0][0]
            last_point_x = point_list[-1][0]
            is_reversed = False
            # list is reversed if it is in the decreasing abscisse order
            if(first_point_x > last_point_x):
                point_list.reverse()
                # print(point_list)
                first_point_x = point_list[0][0]
                last_point_x = point_list[-1][0]
                is_reversed = True
            # memory for the computation of the current value according to the current_time or 1-current_time 
            # parameter depending on the order of the points
            curve_inverval_is_reversed.append(is_reversed)

            # the interval of the curve which will be used to group curves with non empty
            # interval intersection
            interval = (first_point_x, last_point_x)
            curve_intervals.append(interval)

            # print("rev", is_reversed, "list", point_list)
            # the vertical range of values
            min_y = point_list[0][1]
            max_y = point_list[0][1]
            # print("first point", point_list[0][1])
            for point in point_list[1:]:
                min_y = min(min_y, point[1])
                max_y = max(max_y, point[1])
                # print("next point", point[1])
            if(max_y - min_y > 10.) :
                print("******** curve {:s} has a too large value span ({:.2f}) [{:.2f}-{:.2f}] (should be 10 max) ******"
                    .format(att['inkscape:label'], max_y - min_y, min_y, max_y))
            min_dec_y = int(math.floor(min_y / 10.) * 10)
            max_dec_y = int(math.ceil(max_y / 10.) * 10)
            if(max_dec_y == min_dec_y) : # the values are inside [i * 10, (i+1) + 10[
                # i should be even
                if(min_dec_y / 10 % 2 == 0):
                    max_dec_y += 10
                else:
                    min_dec_y -= 10
            if(max_dec_y - min_dec_y > 10.) :
                print("******** curve {:s} has a too large value span ({:d}) [{:d}-{:d}] (should be 10 max) ******"
                    .format(att['inkscape:label'], max_dec_y - min_dec_y, min_dec_y, max_dec_y))
            if(min_dec_y / 10 % 2 != 0):
                print("******** curve {:s} an incorrect value span [{:d}-{:d}] (should be [odd * 10, (odd+1) * 10] ******"
                    .format(att['inkscape:label'], min_dec_y, max_dec_y))
            # print("curve", att['inkscape:label'], "min/max delta", min_dec_y, max_dec_y, max_dec_y - min_dec_y)
            curve_ranges.append((min_dec_y, max_dec_y))

##################################################################
# SVG parse the curves and their labels
# groups the curve into scenes (by clustering curves with overlapping intervals)
def SVG_cluster_curves_into_scenes() :
    global sorted_curve_scenes_indices
    global sorted_curve_scenes_intervals
    global unique_curve_ranges
    global curve_paths
    global curve_intervals

    # grouping curves with overlapping intervals into scenes
    is_curve_in_scene = [False] * len(curve_intervals)
    curve_scenes_indices = []
    for k1, (interval1, path1) in enumerate(zip(curve_intervals, curve_paths)):
        if(not is_curve_in_scene[k1]) :
            this_curve_scene_indices = [k1]
            is_curve_in_scene[k1] = True
            for k2, (interval2, path2) in enumerate(zip(curve_intervals, curve_paths)):
                if(not is_curve_in_scene[k2] and non_empty_interval_intersection(interval1, interval2)) :
                    this_curve_scene_indices.append(k2)
                    is_curve_in_scene[k2] = True
            curve_scenes_indices.append(this_curve_scene_indices)

    # union of intervals for each scene, which defines the full interval of the scene
    curve_scenes_intervals = []
    for a_curve_scene_indices in curve_scenes_indices :
        interval_union = [sys.float_info.max, sys.float_info.min]
        for currentCurveIndexInScene in a_curve_scene_indices:
            interval_union[0] = min(curve_intervals[currentCurveIndexInScene][0], interval_union[0])
            interval_union[1] = max(curve_intervals[currentCurveIndexInScene][1], interval_union[1])
        curve_scenes_intervals.append(interval_union)
    # removes duplicates in curve ranges
    unique_curve_ranges = list( dict.fromkeys(curve_ranges) )
    # print("curve_scenes_intervals", curve_scenes_intervals)
    # print("curve_labels", curve_labels)
    # print("curve_colors", curve_colors)
    # print("curve_ranges", curve_ranges)

    # sorts the scenes and the intervals according to the first value of the intervals
    intBegin_and_scene_and_int = zip(list(map(lambda x: x[0], curve_scenes_intervals)), curve_scenes_indices, curve_scenes_intervals)
    sorted_intBegin_and_scene_and_int = sorted(intBegin_and_scene_and_int)
    sorted_curve_scenes_indices = [x[1] for x in sorted_intBegin_and_scene_and_int]
    sorted_curve_scenes_intervals = [x[2] for x in sorted_intBegin_and_scene_and_int]
    # print("sorted_curve_scenes_indices", sorted_curve_scenes_indices)
    # print("sorted_curve_scenes_intervals", sorted_curve_scenes_intervals)

##################################################################
# CSV scenario parse the palettes and lights
def CSV_scenario_palettes_and_lights_parsing(scenario_file_name):
    global scenario_lights
    global scenario_nb_palette_colors
    global scenario_palette_colors
    global scenario_palette_colors_names
    global scenario_nb_light_groups

    try:
        FILEin = open(scenario_file_name, "rt")
    except IOError:
            print("File not found :", scenario_file_name, " or path is incorrect")
    line_count = len(FILEin.readlines())
    FILEin.seek(0)
    readCSV = csv.reader(FILEin, delimiter=',')
    # line_count = sum(1 for row in count_readCSV)  # fileObject is your csv.reader
    # print("nb lines", line_count)
    indLine = 0
    while(indLine < line_count) :
        values_line = next(readCSV)
        # print(values_line)
        indLine += 1
        if(values_line[0] == "palette_colors") :
            scenario_nb_palette_colors = to_num(values_line[1])
            # print("scenario_nb_palette_colors", scenario_nb_palette_colors)
            for indLine in range(scenario_nb_palette_colors):
                values_line = next(readCSV)
                indLine += 1
                scenario_palette_colors.append([to_num(values_line[2])/255., to_num(values_line[3])/255.,
                    to_num(values_line[4])/255., to_num(values_line[5])/255., to_num(values_line[6])/255., 
                    to_num(values_line[7])/255., to_num(values_line[8])/255., to_num(values_line[9])/255., to_num(values_line[10])/255.])
                scenario_palette_colors_names.append(values_line[1])
            # last line
            values_line =  next(readCSV)
            indLine += 1
            if values_line[0] != "/palette_colors":
                print( "Expected tag \"/palette_colors\" not found in line:", values_line)
                return
        if(values_line[0] == "lights") :
            nb_lights = to_num(values_line[1])
            # print("nb lights", nb_lights)
            for indLine in range(nb_lights):
                values_line =  next(readCSV)
                indLine += 1
                new_light = Light(values_line[1], to_num(values_line[2]), to_num(values_line[3]), \
                    values_line[4], values_line[5], values_line[6], values_line[7], \
                    values_line[8], values_line[9], values_line[10], values_line[11], \
                    values_line[12], values_line[13], values_line[14], values_line[15])
                scenario_lights.append(new_light)
                # print("current nb groups / new group / new nb groups", scenario_nb_light_groups, new_light.light_get_group(), max(scenario_nb_light_groups, new_light.light_get_group() + 1))
                scenario_nb_light_groups = max(scenario_nb_light_groups, new_light.light_get_group() + 1)
            # last line
            values_line =  next(readCSV)
            indLine += 1
            if values_line[0] != "/lights":
                print( "Expected tag \"/lights\" not found in line:", values_line)
                return
            FILEin.close()
            # print("lights in scenario file:", len(scenario_lights))
            # for light in scenario_lights :
            #     print(light.ID)
            return

##################################################################
##################################################################
# MAIN PROGRAM
##################################################################
##################################################################
# INITIALIZATIONS
# clock initialization
clock = pygame.time.Clock()

# OSC initialization
osc_startup()

# ENTEC DMX SB Pro intialization
if(DMX_in_Python == True) :
    # universe 0
    print("Open DMX 0 port", DMX1_Port)
    dmx1 = Controller(DMX1_Port, dmx_size=512)  # Typical of Windows, The size of the DMX universe defaults to 512 channels. 
                                              # There are apparently occasions where finer timescales can be achieved 
    dmx1.set_channel(1, 255)  # Sets DMX channel 1 to max 255
    dmx1.submit()  # Sends the update to the controller

    # # universe 2
    # print("Open DMX 2 port", DMX2_Port)
    # dmx2 = Controller(DMX2_Port, dmx_size=512)  # Typical of Windows, The size of the DMX universe defaults to 512 channels. 
    #                                           # There are apparently occasions where finer timescales can be achieved 
    # dmx2.set_channel(1, 255)  # Sets DMX channel 1 to max 255
    # dmx2.submit()  # Sends the update to the controller

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

# graphics initialization
pygame.init()                                  # init pygame
screen_width=1920
screen_height=500
screen_pos_x=0
screen_pos_y=0
DISPLAYSURF=pygame.display.set_mode(size=(screen_width, screen_height))
environ['SDL_VIDEO_WINDOW_POS'] = '%i,%i' % (screen_pos_x,screen_pos_y) # Set pygame window location
# full screen DISPLAYSURF = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
DISPLAYSURF.fill(pygame.Color('black'))        # set background to black

###########################################################
# SCENARIO FILE WITH LIGHTS SPECIFICATIONS
CSV_scenario_palettes_and_lights_parsing(scenario_with_lights_file_name)
# print("-> scenario_nb_palette_colors", scenario_nb_palette_colors)
# print("scenario_palette_colors", scenario_palette_colors)
# for ind in range (14):
#     print("color", ind/14., compute_pulsed_palette_color(ind/14.))
# for indLight, light in enumerate(scenario_lights):
#     print("light", indLight, light.ID, light.address)

# create a list of light groups
scenario_light_groups = [LightGroup() for i in range(scenario_nb_light_groups)]
# and assign to this group the list of lights which have this group number
for light in scenario_lights:
    scenario_light_groups[light.group - 1].lights.append(light)

###########################################################
# SVG CUrVES FILE READING AND PATH VALUE ANALYSIS
SVG_parse_light_channel_curves()

###########################################################
# CLURTERING BY TIME INTERVALS WITH VARYING VALUES
SVG_cluster_curves_into_scenes()

###########################################################
# SVG FILE READING AND TEXT LABEL VALUE ANALYSIS
# looks for a label for each scene, otherwise gives a standard label
doc = xml.dom.minidom.parse(svg_curves_file_name)
text_elements = doc.getElementsByTagName('text')

scene_labels = []
for indScene, an_interval in enumerate(sorted_curve_scenes_intervals) :
    text_found = False
    for text_element in text_elements:
        # if (text_element.attributes['style'].value.find('font-style:italic') != -1):
        text_x = to_num(text_element.attributes['x'].value)
        if text_x >= an_interval[0] and text_x <= an_interval[1] :
            scene_labels.append("#{:d}: {:s}".format((indScene + 1), text_element.firstChild.firstChild.nodeValue))
            text_found = True
            break
    if not text_found :
        scene_labels.append("#{:d}".format(indScene + 1))

###########################################################
# SCENE PRINTING
# prints the scenes
scene_verbatims_console = []
scene_verbatims_screen = []
for (a_curve_scene_indices, an_interval, scene_label) in zip(sorted_curve_scenes_indices, sorted_curve_scenes_intervals, scene_labels):
    text_screen = "scene {:<20s} [{:3.0f}, {:3.0f}] {:>2d} curve(s)".\
                format(scene_label, an_interval[0], an_interval[1], len(a_curve_scene_indices))
    text_console = text_screen + "\n"
    for currentCurveIndexInScene in a_curve_scene_indices:
        address = curve_labels[currentCurveIndexInScene]
        if(curve_inverval_is_reversed[currentCurveIndexInScene]) :
            text_console = text_console + "         {:<3d} {:<20s}*\n".format(currentCurveIndexInScene, address)
        else :
            text_console = text_console + "         {:<3d} {:<20s} \n".format(currentCurveIndexInScene, address)
    # print(scene_verbatim)
    scene_verbatims_console.append(text_console)
    scene_verbatims_screen.append(text_screen)

###########################################################
# ANIMATION
# intializes the previous values
previous_address_value = {}
previous_address_value_update_frame = {}
FrameNo = 0
maximal_value_step_between_two_frames = 0.05
one_DMX1_value_changed = False
one_DMX2_value_changed = False
for a_curve_scene_indices in sorted_curve_scenes_indices:
    for currentCurveIndexInScene in a_curve_scene_indices:
        # OSC message construction and screen message display
        DMX_initial_value_storage(0., curve_labels[currentCurveIndexInScene])

# sends the values 
current_union_interval = [0, 0]
# OSC_receive_light_control_handler("/light_control/light_group", (4,))
# OSC_receive_light_control_handler("/light_control/light_group", (2,))
while True:  # loop to wait till window close
    clock.tick(FPS)
    current_time += frame_speed 
    DISPLAYSURF.fill((0,0,0))
    one_DMX1_value_changed = False
    one_DMX2_value_changed = False

    # horizontal translation as a function of time cursor position
    if(cursor_location * horiz_scale > screen_width/2) :
        horiz_translate = - ((cursor_location * horiz_scale - screen_width/2) // 30 * 30)
    else :
        horiz_translate = 0

    # DRAWING (SCENE RANGES, CURVE RANGES, CURVES, INITIAL INFO
    ANIMATION_DISPLAY_update_values_and_redraw()

    # osc messages reception
    for i in range(5) :
        osc_process()

    # DMX message emission
    if(DMX_in_Python and one_DMX1_value_changed) :
        # print("DMX #1 update")
        dmx1.submit()  # Sends the update to the controller
        one_DMX1_value_changed = False
    elif(DMX_in_Python and one_DMX2_value_changed) :
        # print("DMX #2 update")
        # dmx2.submit()  # Sends the update to the controller
        one_DMX2_value_changed = False

    # event managemnt
    for event in pygame.event.get():
        if (event.type == pygame.KEYDOWN):
            if(event.key == pygame.K_ESCAPE):
                sys.exit()
            elif (event.key == pygame.K_SPACE):
                OSC_play_pause()
            elif (event.key == pygame.K_RIGHT):
                OSC_next_light_scene(False)
            elif (event.key == pygame.K_LEFT):
                OSC_previous_light_scene()
            elif (event.key == pygame.K_UP):
                OSC_speed_up_step()
            elif (event.key == pygame.K_DOWN):
                OSC_speed_down_step()
            elif (event.key == pygame.K_0):
                OSC_zero_light()
            elif (event.key in  range(pygame.K_1, pygame.K_9 + 1)):
                OSC_go_to_light_scene(int(event.key - pygame.K_1), False)

    FrameNo += 1
