#!/usr/bin/python3 

# vv_transform.py
# transformations of vector video frames (both initial transformation before crossfading and final transformations after crossfading)

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import math
import random

import getopt
import sys
from signal import signal, SIGINT

import re
import operator

import wave
import pydub
from pydub import AudioSegment

import struct
import numpy as np
import cv2 as cv
import os

import csv

from vv_lib import clamp
from vv_lib import to_num
from vv_lib import force_num
from vv_lib import interpolate

def handler(signal_received, frame):
	# Handle any cleanup here
	print('SIGINT or CTRL-C detected. Exiting gracefully')
	sys.exit(0)

USAGE = '''usage: vv_transform.py --nb-layers <integer> -o <file_name> -i <file_name> --transform <single_transf>
vv_transform.py --nb-layers 4 -o mer.svg -i mer.svg --transformation COPY --percent 0.7 --translation
ARGUMENTS:
   --nb_layers: number of layers for posterization
   -o: ouput files 
   -i: input files
   --transformation: transformation function
   --percent: percentage of transformation
optional
   --translation: x, y coordinates of translation
'''

##################################################################
# GLOBAL VARIABLES
##################################################################

##################################################################
# MAIN SUB
##################################################################
def main(main_args) :
	# SCENARIO VARIABLES
	var_names = []
	var_types = dict()
	val_init = dict()
	val_fin = dict()
	val_interp = dict()
	interp = dict()

	##################################################################
	# ARGUMENTS
	##################################################################
	scenario_file_name = ''
	try:
		opts, args = getopt.getopt(main_args,"i:",["inputfile=","plot=","extrema="])
	except getopt.GetoptError:
		print(USAGE)
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--inputfile"):
			scenario_file_name = arg
		elif opt in ("--plot"):
			is_plot = arg
		elif opt in ("--extrema"):
			is_extrema = arg
		else:
			assert False, "unhandled option"
	# print( 'Input scenario file is ', scenario_file_name)

	try:
		FILEin = open(scenario_file_name, "rt")
	except IOError:
			print("File not found :", scenario_file_name, " or path is incorrect")
	
	##################################################################
	# READS THE SCENARIO LINE BY LINE
	##################################################################
	readCSV = csv.reader(FILEin, delimiter=',')
	line = next(readCSV) 
	# print "line1 ", line 
	line = next(readCSV) 
	# print "line2 ", line 

	#3rd line: variable types
	line_types =  next(readCSV)

	#4th line: variable names
	var_names =  next(readCSV) 
	#removes the first one
	var_names.pop(0)

	#finishes the types now that the variable strings are known
	#removes the first one
	line_types.pop(0)
	indVar = 0
	for line_type in line_types :
		var_types[var_names[indVar]] = line_type
		indVar += 1

	# scenario line
	values_line =  next(readCSV)
	if values_line[0] != "scenario":
		print( "Expected tag \"scenario\" not found in line:", values_line)
		return 0
	
	nb_scenes = to_num(values_line[1])
	# print( "nb scenes ", nb_scenes)

	##################################################################
	# READING AND EXECUTING A SCENE
	##################################################################
	for ind_scene in range(nb_scenes) :
		# scene line
		values_line =  next(readCSV)
		if values_line[0] != "scene":
			print( "Expected tag \"scene\" not found!" )
			return 0
		scene_ID = values_line[1]

		# scene ID line
		line =  next(readCSV)
		# var IDs line
		line =  next(readCSV)

		# variable initial value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_init in values_line:
			val_init[var_names[indVar]] = to_num(value_init)
			indVar += 1

		# variable final value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_fin in values_line:
			val_fin[var_names[indVar]] = to_num(value_fin)
			indVar += 1

		# variable interpolation value
		values_line =  next(readCSV)
		values_line.pop(0)
		indVar = 0
		for value_interp in values_line:
			interp[var_names[indVar]] = value_interp	
			indVar += 1

		# /scene line
		values_line = next(readCSV)
		if(values_line[0] != "/scene") :
			print( "Expected tag \"/scene\" not found!\n" )
			return 0
		
		##################################################################
		# STORES THE LINES IN AN ARRAY
		##################################################################
		soundtrack_working_channels = 1
		expected_variables = ["soundtrack_directory", "soundtrack_onsets_envelopes_file", "soundtrack_working_channels", "samples_directory", "sample_files", "short_sample_files", "nb_sample_files", "nb_short_sample_files", "hammer_files", "nb_hammer_files", "selected_hammer_files", "repeated_hammer_files", "hammer_threshold", "short_voice_threshold", "new_soundtrack_file"]
		if all(elem in val_init for elem in expected_variables) :
			soundtrack_directory = val_init["soundtrack_directory"]
			soundtrack_onsets_envelopes_file = val_init["soundtrack_onsets_envelopes_file"]
			print("Loading onsets/envelopes from", os.path.join(soundtrack_directory, soundtrack_onsets_envelopes_file))
			soundtrack_working_channels = int(to_num(val_init["soundtrack_working_channels"]))
			if soundtrack_working_channels != 1 and soundtrack_working_channels != 2:
				print("only mono or stereo soundtrack working channels are possible")
				sys.exit(0)

			################################################
			# READS ONSETS AND ASSOCIATED SMOOTHED ENVELOPPE VALUES 
			# left channel onset time (or -1 if no onset) / right channel onset time (or -1 if no onset) / left channel smoothed envelope value / right channel smoothed envelope value
			# opens the file and reads in the CSV mode (4 columns [timestamp, timestamp, value, value])
			try:
				local_FILEsoundtrack_onsets_envelopes = open(os.path.join(soundtrack_directory, soundtrack_onsets_envelopes_file), "rt")
			except IOError:
				print("File not found :", os.path.join(soundtrack_directory, soundtrack_onsets_envelopes_file), " or path is incorrect")
				return 0

			# CSV READER
			local_readsoundtrack_onsets_envelopesCSV = csv.reader(local_FILEsoundtrack_onsets_envelopes, delimiter=',')
			# READS THE SOUND FILE DATA LINE BY LINE
			# Pass reader object to list() to get a list of lists [timestamp, value]
			Onset_Timestamp_Envelope_sound_data = list(local_readsoundtrack_onsets_envelopesCSV)
			local_FILEsoundtrack_onsets_envelopes.close()

			# separates list of timestamps and list of values
			TimestampLeft =[]
			EnvelopeLeft =[]
			TimestampRight =[]
			EnvelopeRight =[]
			for timestamp_signal in Onset_Timestamp_Envelope_sound_data :
				if soundtrack_working_channels == 1 :
					TimestampLeft.append(to_num(timestamp_signal[0]) * 1000)
					EnvelopeLeft.append(to_num(timestamp_signal[1]))
				else :
					TimestampLeft.append(to_num(timestamp_signal[0]) * 1000)
					TimestampRight.append(to_num(timestamp_signal[1]) * 1000)
					EnvelopeLeft.append(to_num(timestamp_signal[2]))
					EnvelopeRight.append(to_num(timestamp_signal[3]))
			# max enveloppe left
			max_vol_left = max(EnvelopeLeft)
			if soundtrack_working_channels == 2 :
				# max enveloppe right
				max_vol_right = max(EnvelopeRight)

			nb_beats_intro = 20
			period_intro = 250
			phase_intro = 180
			nb_beats_intro_fadingOut = 36
			if phase_intro > period_intro :
				print("phase_intro", phase_intro, "should be less than period_intro", period_intro)
				sys.exit(0)

			nb_beats_break = 20
			period_break = 250
			cut_break = 75228.867
			nb_beats_break_fadingIn = 12
			nb_beats_break_solo = 36
			nb_beats_break_fadingOut = 12

			nb_beats_ending = 20
			period_ending = 250
			cut_ending = 151642.929
			nb_beats_ending_fadingIn = 12
			nb_beats_ending_solo = 36

			##################################################################
			# VARIABLES FROM SCENARIO
			##################################################################
			samples_directory = val_init["samples_directory"]
			sample_files = val_init["sample_files"]
			short_sample_files = val_init["short_sample_files"]
			hammer_files = val_init["hammer_files"]
			# print("hammer_threshold", val_init["hammer_threshold"], hammer_threshold)
			new_soundtrack_file = val_init["new_soundtrack_file"]
			full_new_soundtrack_file_name = os.path.join(soundtrack_directory, new_soundtrack_file)

			##################################################################
			# GENERATING AN EMPTY AUDIO FILE WHICH WILL BE BUILT THROUGH OVERLAYS
			##################################################################
			# new soundtrack length = last onset + 1 sec
			new_soundtrack_length = max(max(TimestampLeft), max(TimestampRight)) + 1000
			# + intro made of nb_beats_intro of period_intro
			intro_duration = nb_beats_intro * period_intro
			new_soundtrack_length = new_soundtrack_length + intro_duration
			break_duration = nb_beats_break * period_break
			new_soundtrack_length = new_soundtrack_length + break_duration
			ending_duration = nb_beats_ending * period_ending
			new_soundtrack_length = new_soundtrack_length + ending_duration

			print("creates soundtrack of ", new_soundtrack_length, "ms")
			new_soundtrack = AudioSegment.silent(duration=new_soundtrack_length)

			# SAMPLES FOR INTRO/BREAK/ENDING
			repeated_hammer_files_string = val_init["repeated_hammer_files"]
			repeated_hammer_files = list(map(lambda x: to_num(x), repeated_hammer_files_string.split('/')))
			print("repeated_hammer_files", repeated_hammer_files)
			##################################################################
			# INTRODUCTION
			##################################################################
			# the first part of the piece is a repeating drum of the same sound
			# in rhythm with what will follow up in the piece
			# the period is 0.250 (four times a second)
			# the phase is 0.18. 
			intro_hammer_no = repeated_hammer_files[0] # random.choice(range(1,nb_hammer_files + 1))
			intro_hammer_file_name = os.path.join(samples_directory, hammer_files+"%05d" % intro_hammer_no+".wav")
			intro_hammer_sound = AudioSegment.from_file(intro_hammer_file_name, format="wav")
			for ind in range(nb_beats_intro) :
				new_soundtrack = new_soundtrack.overlay(intro_hammer_sound, position= ind * period_intro + phase_intro)

			##################################################################
			# BREAK
			##################################################################
			# a part in the middle of the piece which is is a repeating drum of the same sound
			# the period is 0.250 (four times a second)
			# the phase is 75.228867 (wrt initial beginning without the break) 
			break_hammer_no = repeated_hammer_files[1] # random.choice(range(1,nb_hammer_files + 1))
			break_hammer_file_name = os.path.join(samples_directory, hammer_files+"%05d" % break_hammer_no+".wav")
			break_hammer_sound = AudioSegment.from_file(break_hammer_file_name, format="wav")
			for ind in range(nb_beats_break) :
				new_soundtrack = new_soundtrack.overlay(break_hammer_sound, position= ind * period_break + cut_break + intro_duration)

			##################################################################
			# ENDING
			##################################################################
			# end of the piece which is is a repeating drum of the same sound
			# the period is 0.250 (four times a second)
			ending_hammer_no = repeated_hammer_files[2] # random.choice(range(1,nb_hammer_files + 1))
			ending_hammer_file_name = os.path.join(samples_directory, hammer_files+"%05d" % ending_hammer_no+".wav")
			ending_hammer_sound = AudioSegment.from_file(ending_hammer_file_name, format="wav")
			for ind in range(nb_beats_ending) :
				new_soundtrack = new_soundtrack.overlay(ending_hammer_sound, position= ind * period_ending + cut_ending + intro_duration + break_duration)

			# SAMPLE HAMMER FILES FOR PERCUSSIONS
			selected_hammer_files_string = val_init["selected_hammer_files"]
			selected_hammer_files = list(map(lambda x: to_num(x), selected_hammer_files_string.split('/')))
			print("selected_hammer_files", selected_hammer_files)

			##################################################################
			# SAMPLE FILES
			##################################################################
			onset_index = 0
			next_overlay = -1
			max_time_stamp_left = max(TimestampLeft)
			max_time_stamp_right = max(TimestampRight)
			for (time_stamp_left, time_stamp_right, envelope_left, envelope_right) in zip(TimestampLeft, TimestampRight, EnvelopeLeft, EnvelopeRight):
				# scene percentage at this frame 
				scene_percentage = 0
				if time_stamp_left >= 0 :
					scene_percentage = time_stamp_left/max_time_stamp_left
				elif time_stamp_right >= 0 :
					scene_percentage = time_stamp_right/max_time_stamp_right
				#clamping
				scene_percentage = clamp(scene_percentage)

				# interpolating all the variables
				for var_name in var_names :
					val_interp[var_name] = interpolate(var_types[var_name], val_init[var_name], val_fin[var_name], interp[var_name], scene_percentage)
					# print( "var name [",var_name,"] value: ",val_init[var_name], " ", val_fin[var_name], " ", interp[var_name], " ", val_interp[var_name])

				if(onset_index % 300 == 0 ) :
					print(time_stamp_left, "/", time_stamp_right, "/", scene_percentage)
				onset_index += 1

				nb_sample_files = to_num(val_interp["nb_sample_files"])
				nb_short_sample_files = to_num(val_interp["nb_short_sample_files"])
				nb_hammer_files = to_num(val_interp["nb_hammer_files"])
				next_sample_no = random.choice(range(nb_sample_files)) + 1
				next_short_sample_no = random.choice(range(nb_short_sample_files)) + 1
				next_hammer_no = random.choice(selected_hammer_files)

				hammer_threshold = force_num(val_interp["hammer_threshold"])
				short_voice_threshold = force_num(val_interp["short_voice_threshold"])

				# loads sample wav file data
				sample_file_name = os.path.join(samples_directory, sample_files+"%05d" % next_sample_no+".wav")
				sample_sound = AudioSegment.from_file(sample_file_name, format="wav")
				short_sample_file_name = os.path.join(samples_directory, short_sample_files+"%05d" % next_short_sample_no+".wav")
				short_sample_sound = AudioSegment.from_file(short_sample_file_name, format="wav")
				hammer_file_name = os.path.join(samples_directory, hammer_files+"%05d" % next_hammer_no+".wav")
				hammer_sound = AudioSegment.from_file(hammer_file_name, format="wav")
				if sample_sound.sample_width != 2 or hammer_sound.sample_width != 2 :
					print("only 16bits wav files are processed")
					sys.exit(0)
				# Split stereo to mono 
				split_sample_sound = sample_sound.split_to_mono() 
				split_short_sample_sound = short_sample_sound.split_to_mono() 
				split_hammer_sound = hammer_sound.split_to_mono() 

				# checks the compatibility between sample and soundtrack channel numbers
				if soundtrack_working_channels == 1 :
					if short_sample_sound.channels != soundtrack_working_channels or sample_sound.channels != soundtrack_working_channels or hammer_sound.channels != soundtrack_working_channels:
						print("working with mono sound, samples must be mono as well")
						sys.exit(0)
				else :
					if short_sample_sound.channels != soundtrack_working_channels or sample_sound.channels != soundtrack_working_channels or hammer_sound.channels != soundtrack_working_channels:
						print("working with stereo sound, samples must be stereo as well")
						sys.exit(0)

				# dims the sound on the side where there is not a timestamp (there can be timestamp on both)
				time_stamp = max(time_stamp_left, time_stamp_right)
				if(time_stamp_left < 0) : # no timestamp on left channel
					left_sample = split_sample_sound[0] - 8 - (1 - envelope_left/max_vol_left) * 10
					left_short_sample = split_short_sample_sound[0] - 8 - (1 - envelope_left/max_vol_left) * 10
					left_hammer = split_hammer_sound[0]
				else : # timestamp on left channel
					left_sample = split_sample_sound[0] - (1 - envelope_left/max_vol_left) * 10
					dimmed_sample_sound = left_sample
					left_short_sample = split_short_sample_sound[0] - (1 - envelope_left/max_vol_left) * 10
					dimmed_short_sample_sound = left_short_sample
					left_hammer = split_hammer_sound[0]
					dimmed_hammer_sound = left_hammer
				if soundtrack_working_channels == 2 :
					if(time_stamp_right < 0) : # no timestamp on right channel
						right_sample = split_sample_sound[1] - 8 - (1 - envelope_right/max_vol_right) * 10
						right_short_sample = split_short_sample_sound[1] - 8 - (1 - envelope_right/max_vol_right) * 10
						right_hammer = split_hammer_sound[1] - (1 - envelope_right/max_vol_right) * 5
					else : # timestamp on right channel
						right_sample = split_sample_sound[1] - (1 - envelope_right/max_vol_right) * 10
						right_short_sample = split_short_sample_sound[1] - (1 - envelope_right/max_vol_right) * 10
						right_hammer = split_hammer_sound[1] - (1 - envelope_right/max_vol_right) * 5
					dimmed_sample_sound = AudioSegment.from_mono_audiosegments(left_sample, right_sample)
					dimmed_short_sample_sound = AudioSegment.from_mono_audiosegments(left_short_sample, right_short_sample)
					dimmed_hammer_sound = AudioSegment.from_mono_audiosegments(left_hammer, right_hammer)

				# during intro crossfading, each sample is blended with the intro hammer sound
				intro_crossfading_duration = nb_beats_intro_fadingOut * period_intro + phase_intro
				if(time_stamp < intro_crossfading_duration) :
					new_sample = AudioSegment.silent(duration=max(len(dimmed_sample_sound), len(intro_hammer_sound)))
					dimmed_sample_sound = dimmed_sample_sound - 10 * ((intro_crossfading_duration - time_stamp)/intro_crossfading_duration)
					dimmed_intro_hammer_sound = intro_hammer_sound - 10 * (1.0 - (intro_crossfading_duration - time_stamp)/intro_crossfading_duration)
					dimmed_sample_sound = (new_sample.overlay(dimmed_sample_sound)).overlay(dimmed_intro_hammer_sound)
					new_short_sample = AudioSegment.silent(duration=max(len(dimmed_short_sample_sound), len(intro_hammer_sound)))
					dimmed_short_sample_sound = dimmed_short_sample_sound - 10 * ((intro_crossfading_duration - time_stamp)/intro_crossfading_duration)
					dimmed_intro_hammer_sound = intro_hammer_sound - 10 * (1.0 - (intro_crossfading_duration - time_stamp)/intro_crossfading_duration)
					dimmed_short_sample_sound = (new_short_sample.overlay(dimmed_short_sample_sound)).overlay(dimmed_intro_hammer_sound)

				# during break fadingIn, each sample is blended with the break hammer sound
				break_fadingIn_duration = nb_beats_break_fadingIn * period_break 
				if(time_stamp < cut_break and time_stamp >= cut_break - break_fadingIn_duration) :
					new_sample = AudioSegment.silent(duration=max(len(dimmed_sample_sound), len(break_hammer_sound)))
					dimmed_sample_sound = dimmed_sample_sound - 10 * (1.0 - (cut_break - time_stamp)/break_fadingIn_duration)
					dimmed_break_hammer_sound = break_hammer_sound - 10 * ((cut_break - time_stamp)/break_fadingIn_duration)
					dimmed_sample_sound = (new_sample.overlay(dimmed_sample_sound)).overlay(dimmed_break_hammer_sound)
					new_short_sample = AudioSegment.silent(duration=max(len(dimmed_short_sample_sound), len(break_hammer_sound)))
					dimmed_short_sample_sound = dimmed_short_sample_sound - 10 * (1.0 - (cut_break - time_stamp)/break_fadingIn_duration)
					dimmed_break_hammer_sound = break_hammer_sound - 10 * ((cut_break - time_stamp)/break_fadingIn_duration)
					dimmed_short_sample_sound = (new_short_sample.overlay(dimmed_short_sample_sound)).overlay(dimmed_break_hammer_sound)

				# during break fadingOut, each sample is blended with the break hammer sound
				break_fadingOut_duration = nb_beats_break_fadingOut * period_break 
				if(time_stamp >= cut_break and time_stamp <= cut_break + break_fadingOut_duration) :
					new_sample = AudioSegment.silent(duration=max(len(dimmed_sample_sound), len(break_hammer_sound)))
					dimmed_sample_sound = dimmed_sample_sound - 10 * (1.0 - (time_stamp - cut_break)/break_fadingOut_duration)
					dimmed_break_hammer_sound = break_hammer_sound - 10 * ((time_stamp - cut_break)/break_fadingOut_duration)
					dimmed_sample_sound = (new_sample.overlay(dimmed_sample_sound)).overlay(dimmed_break_hammer_sound)
					new_short_sample = AudioSegment.silent(duration=max(len(dimmed_short_sample_sound), len(break_hammer_sound)))
					dimmed_short_sample_sound = dimmed_short_sample_sound - 10 * (1.0 - (time_stamp - cut_break)/break_fadingOut_duration)
					dimmed_break_hammer_sound = break_hammer_sound - 10 * ((time_stamp - cut_break)/break_fadingOut_duration)
					dimmed_short_sample_sound = (new_short_sample.overlay(dimmed_short_sample_sound)).overlay(dimmed_break_hammer_sound)

				# during ending fadingIn, each sample is blended with the ending hammer sound
				ending_fadingIn_duration = nb_beats_ending_fadingIn * period_ending 
				if(time_stamp < cut_ending and time_stamp >= cut_ending - ending_fadingIn_duration) :
					new_sample = AudioSegment.silent(duration=max(len(dimmed_sample_sound), len(ending_hammer_sound)))
					dimmed_sample_sound = dimmed_sample_sound - 10 * (1.0 - (cut_ending - time_stamp)/ending_fadingIn_duration)
					dimmed_ending_hammer_sound = ending_hammer_sound - 10 * ((cut_ending - time_stamp)/ending_fadingIn_duration)
					dimmed_sample_sound = (new_sample.overlay(dimmed_sample_sound)).overlay(dimmed_ending_hammer_sound)
					new_short_sample = AudioSegment.silent(duration=max(len(dimmed_short_sample_sound), len(ending_hammer_sound)))
					dimmed_short_sample_sound = dimmed_short_sample_sound - 10 * (1.0 - (cut_ending - time_stamp)/ending_fadingIn_duration)
					dimmed_ending_hammer_sound = ending_hammer_sound - 10 * ((cut_ending - time_stamp)/ending_fadingIn_duration)
					dimmed_short_sample_sound = (new_short_sample.overlay(dimmed_short_sample_sound)).overlay(dimmed_ending_hammer_sound)

				# adds a sample at the onset(s) position
				# length of the current output track
				# print("overlays sample at time", time_stamp, "ms of duration", len(dimmed_sample_sound), "ms")
				if(time_stamp < new_soundtrack_length and time_stamp > next_overlay) :
					if(time_stamp < cut_break) :
						sample_position = time_stamp + intro_duration
					else :
						sample_position = time_stamp + intro_duration + break_duration
					# overlays the sample at the onset position over what has already been rendered in the new soundtrack
					if soundtrack_working_channels == 1 :
						# hammer sounds are used when the smoothed envelope at onset is above hammer_threshold
						if envelope_left > hammer_threshold * max_vol_left :
							# fades out the current track to avoid too much overlay between percussions and voices
							new_soundtrack.fade(to_gain=-5, start=sample_position - 10, duration=10)
							new_soundtrack.fade(from_gain=-5, start=sample_position, duration=10)
							new_soundtrack = new_soundtrack.overlay(dimmed_hammer_sound, position=sample_position)
							next_overlay = time_stamp + 50
						elif envelope_left > short_voice_threshold * max_vol_left :
							new_soundtrack = new_soundtrack.overlay(dimmed_short_sample_sound, position=sample_position)
						else :
							new_soundtrack = new_soundtrack.overlay(dimmed_sample_sound, position=sample_position)
					else :
						if envelope_left > hammer_threshold * max_vol_left or envelope_right > hammer_threshold * max_vol_right :
							new_soundtrack.fade(to_gain=-5, start=sample_position - 10, duration=10)
							new_soundtrack.fade(from_gain=-5, start=sample_position, duration=10)
							new_soundtrack = new_soundtrack.overlay(dimmed_hammer_sound, position=sample_position)
							next_overlay = time_stamp + 50
						elif envelope_left > short_voice_threshold * max_vol_left or envelope_right > short_voice_threshold * max_vol_right :
							new_soundtrack = new_soundtrack.overlay(dimmed_short_sample_sound, position=sample_position)
						else :
							new_soundtrack = new_soundtrack.overlay(dimmed_sample_sound, position=sample_position)

			# saves the soundtrack
			new_soundtrack.export(full_new_soundtrack_file_name, format="wav")


	# /scenario line
	values_line = next(readCSV)
	if(values_line[0] != "/scenario") :
		print( "Expected tag \"/scenario\" not found!\n" )
		return 0

	return 1

if __name__ == "__main__":
	import sys
	
	# Tell Python to run the handler() function when SIGINT is recieved
	signal(SIGINT, handler)
	main(sys.argv[1:])
