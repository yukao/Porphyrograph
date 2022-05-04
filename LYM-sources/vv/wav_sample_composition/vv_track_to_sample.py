#!/usr/bin/python3 

# launch command for the time conversion of a scenario
# (c) Yukao Nagemi

import sys
import wave
import pydub
from pydub import AudioSegment
from signal import signal, SIGINT
import os
import random
import struct

def random_sample_generation(track_files_list, duration, incay, decay, sample_dir, sample_file_name, sample_index) :
	out_sample_name = os.path.join(sample_dir, sample_file_name + "%05d" % sample_index + ".wav")
	in_track = random.choice(track_files_list)
	print(out_sample_name, "<-", in_track)

	# WAV FILE DATA
	sound = AudioSegment.from_file(in_track, format="wav")
	nb_channels = sound.channels
	if nb_channels != 2:
		print("only stereo files are processed")
		sys.exit(0)
	bytes_per_sample = sound.sample_width
	if bytes_per_sample != 2:
		print("only 16bits wav files are processed")
		sys.exit(0)
	# print(f"number of channels = {nb_channels}")
	frames_per_second = sound.frame_rate
	soundtrack_duration = len(sound)
	# print(f"sound file duration (sec) = {soundtrack_duration/1000}s")

	# random length
	sample_length =  random.uniform(0.5 * duration, 1.5 * duration)
	# random start
	sample_start_time =  random.uniform(0, soundtrack_duration - 2 * sample_length) 

	# copies wanted frames
	sample = sound[sample_start_time:sample_start_time + sample_length]
	normalized_sample = sample.apply_gain(-sample.max_dBFS)
	loudness = normalized_sample.rms
	normalized_sample_fade_in = normalized_sample.fade(from_gain=-120.0, start=0, duration=int(incay * sample_length / duration))
	normalized_sample_fade_in_out = normalized_sample_fade_in.fade(to_gain=-120.0, end=0, duration=int(decay * sample_length / duration))

	# print(f"sample file start and duration (msec) = {sample_start_time}ms {sample_length}ms Loudness {loudness}dB")

	# simple export
	file_handle = normalized_sample_fade_in_out.export(out_sample_name, format="wav")
	return 1
