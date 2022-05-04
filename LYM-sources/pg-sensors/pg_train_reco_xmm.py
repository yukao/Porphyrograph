#!/usr/bin/env python
# encoding: utf-8
"""
test_gmm.py

Simple Test File for GMMs

Contact:
- Jules Francoise <jules.francoise@ircam.fr>

This code has been initially authored by Jules Francoise
<http://julesfrancoise.com> during his PhD thesis, supervised by Frederic
Bevilacqua <href="http://frederic-bevilacqua.net>, in the Sound Music
Movement Interaction team <http://ismm.ircam.fr> of the
STMS Lab - IRCAM, CNRS, UPMC (2011-2015).

Copyright (C) 2015 UPMC, Ircam-Centre Pompidou.

This File is part of XMM.

XMM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

XMM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with XMM.  If not, see <http://www.gnu.org/licenses/>.
"""

import numpy as np
import matplotlib.pyplot as plt
import xmm
import shutil

from time import time

from os import listdir, makedirs, remove
from os.path import isfile, join, isdir, dirname, exists

# library to convert between strings and nums
from vv_lib import to_num

# OSC
import pg_OSC

WITH_PLOT = False

pg_gmm = None

##################################################################
# GMM FOR TRAINING AND RECOGNITION
##################################################################
class GMM_data:
	# GMM management with on-the-fly samples
	def __init__(self, connected_sensors_IDs, DIR_TrainingData_name, loc_XMM_likelihood_window, loc_XMM_training_labels, loc_XMM_training_labels_ClipArts, loc_XMM_likelihood_threshold, loc_XMM_likelihood_minimal_duration, loc_XMM_forward_gesture_recognition, loc_XMM_forward_web_animation, loc_XMM_forward_sensor_masses_animation) :
		# Construct a training sample.
		self.FILE_TrainingData_name = dict()
		self.DIR_TrainingData_name = DIR_TrainingData_name
		self.XMM_likelihood_window = loc_XMM_likelihood_window
		self.XMM_likelihood_threshold = loc_XMM_likelihood_threshold
		self.XMM_likelihood_minimal_duration = loc_XMM_likelihood_minimal_duration
		self.XMM_forward_gesture_recognition = loc_XMM_forward_gesture_recognition
		self.XMM_forward_web_animation = loc_XMM_forward_web_animation
		self.XMM_forward_sensor_masses_animation = loc_XMM_forward_sensor_masses_animation
		self.data_dimension = 7 * len(connected_sensors_IDs)
		self.connected_sensors_IDs = connected_sensors_IDs
		self.received_sensors_data = dict()
		self.new_sensors_data = dict()
		for id in self.connected_sensors_IDs :
			self.received_sensors_data[id] = np.zeros(7)
			self.new_sensors_data[id] = False
		self.FILE_TestSet_name = []
		self.XMM_training_labels = loc_XMM_training_labels
		self.XMM_training_labels_ClipArts = loc_XMM_training_labels_ClipArts

		self.TrainingSet = None
		self.DynamicTrainingData = dict()
		self.TestSet = np.array([])
		self.GMM_Model = None
		self.GMM = None
		self.nb_samples_reco = 0
		self.current_max_likelihood_index = -1
		self.first_max_likelihood_for_index= -1

		# global variables
		self.xmm_training_label = ""
		self.xmm_train = False
		self.xmm_reco = False

		if self.DIR_TrainingData_name != '' :
			subdirs = [join(self.DIR_TrainingData_name, o) for o in listdir(self.DIR_TrainingData_name) if isdir(join(self.DIR_TrainingData_name,o))]
			for subdir in subdirs :
				training_label = dirname(subdir)
				self.FILE_TrainingData_name[training_label] = []
				DIR_TrainingData_name_subdir = join(self.DIR_TrainingData_name,subdir)
				for f in listdir(DIR_TrainingData_name_subdir) :
					file_TrainingData_path = DIR_TrainingData_name_subdir + f
					if isfile(file_TrainingData_path) :
						self.FILE_TrainingData_name[training_label].append(file_TrainingData_path)
		else :
			print("DIR_TrainingData_name empty")
		# print("training file name", self.FILE_TrainingData_name)

	################################################################################################
	# USING A TEST SET (CURRENTLY NOT USED WITH ON THE FLY TESTING)
	################################################################################################

	# Test set loading: one file/full set
	def gmm_load_one_test_set(self, fileName) :
		# read test data (concatenation of 3 test examples labeled 1, 2, 3)
		if self.TestSet.size == 0 :
			self.TestSet = np.genfromtxt(fileName)
		else :
			self.TestSet = np.vstack((self.TestSet, np.genfromtxt(fileName)))

	def gmm_load_full_test_set(self) :
		# loads full test sets
		if self.FILE_TestSet_name :
			for file_TestData in self.FILE_TestSet_name :
				self.gmm_load_one_test_set(file_TestData)

	################################################################################################
	# BUILD THE TRAINING SET
	################################################################################################
	# 2 functions to relate index and label of a recorded training set
	def xmm_training_data_index_to_label(self, local_training_index) :
		if local_training_index < len(self.XMM_training_labels) :
			return self.XMM_training_labels[local_training_index]
		else :
			return str(local_training_index + 1)

	def xmm_training_data_label_to_index(self, local_training_label) :
		if local_training_label in XMM_training_labels :
			return self.XMM_training_labels.index(local_training_label)
		else :
			return to_num(local_training_label) - 1

	################################
	# TRAIN FROM RECORDED DATA
	# train the GMM from the training set that has been loaded either dynamically or from data files
	def gmm_train_model_from_data(self, num_gaussians=4, varianceoffset=[1., 0.01]):
		""" Create and Traing a GMM from the given training set
		Args:
			num_gaussians -- Number of Gaussian Components
			varianceoffset -- rel/abs variance offset
		"""
		# Create a GMM Group (handles multiples labels for recognition)
		if self.GMM == None :
			self.GMM = xmm.GMM()
			# Set parameters
			self.GMM.configuration.gaussians.set(num_gaussians)
			self.GMM.configuration.relative_regularization.set(varianceoffset[0])
			self.GMM.configuration.absolute_regularization.set(varianceoffset[1])

		# Train all models
		self.GMM.train(self.TrainingSet)

		# defines the recognition window
		# likelihood_window = 40 # 50
		# Set Size of the likelihood Window (samples)
		self.GMM.shared_parameters.likelihood_window.set(self.XMM_likelihood_window)

	################################
	# START/STOP RECORDING LIVE DATA FOR TRAINING PURPOSES
	# starts building a training set
	def gmm_on_off_train1(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(0)
	def gmm_on_off_train2(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(1)
	def gmm_on_off_train3(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(2)
	def gmm_on_off_train4(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(3)
	def gmm_on_off_train5(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(4)
	def gmm_on_off_train6(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		self.gmm_on_off_train(5)

	def gmm_on_off_train(self, xmm_GMM_new_index) :
		if xmm_GMM_new_index >= 0 :
			self.xmm_reco = False

			# already training this GMM -> stops training
			new_xmm_training_label = self.xmm_training_data_index_to_label(xmm_GMM_new_index)
			if self.xmm_train == True and new_xmm_training_label == self.xmm_training_label :
				print("stop training", self.xmm_training_label)
				self.xmm_train = False
				pg_OSC.UDP_emit("/xmm_on_off_train"+str(xmm_GMM_index+1), "udp_TouchOSC_send", [0])
				return

			# starts training
			print("start training", new_xmm_training_label)
			self.xmm_training_label = new_xmm_training_label
			self.xmm_train = True
			pg_OSC.UDP_emit("/xmm_on_off_train"+str(xmm_GMM_new_index+1), "udp_TouchOSC_send", [1])

		else:
			print("start training fails", xmm_GMM_new_index)
			return

		# resets the data
		if not (self.xmm_training_label in self.DynamicTrainingData) :
			self.DynamicTrainingData[self.xmm_training_label] = [[]]
		else :
			self.DynamicTrainingData[self.xmm_training_label].append([])

	################################
	# STOP RECORDING ONGOING TRAINING DATA
	# stops building the ongoing training set (if there is one)
	def gmm_stop_train(self, address, argt) :
		# print("received OSC {", address, "}: {", argt, "}")
		if self.xmm_train == True and self.xmm_training_label != '' :
			print("stop training", self.xmm_training_label)
			self.xmm_train = False
			training_index = self.xmm_training_data_label_to_index(self.xmm_training_label)
			pg_OSC.UDP_emit("/xmm_on_off_train"+str(training_index+1), "udp_TouchOSC_send", [0])
		else :
			print("no ongoing training")

	################################
	# STREAMING OF TRAINING DATA INTO THE CURRENT RECORDING
	# adding 1 line data to a training set
	def gmm_train_on_the_fly(self, sensor_ID, data_array):
		""" Records the training data for building a simple GMM recognizer
			training_set -- Unimodal Training Set
		"""
		self.received_sensors_data[sensor_ID] = data_array
		self.new_sensors_data[sensor_ID] = True
		# Records 1 line of training data
		if data_array.size != (self.data_dimension / len(self.connected_sensors_IDs)) :
			print("incorrect training data size given {} expected {}".format(data_array.size, self.data_dimension / len(self.connected_sensors_IDs)))
			return
		# the data are recorded inside the GMM only after saving the training data to files 
		# and loading them from the files
		# self.TrainingSet.getPhrase(ind_GMM).record(data_array)

		# if all data is true, send the training data and sets new to false
		all_data_new = True
		for id in self.connected_sensors_IDs :
			if self.new_sensors_data[id] == False :
				all_data_new = False
				break

		# all sensor data is new
		if all_data_new == True :
			# concatenates all the data from all the sensors
			all_data = np.array([])
			for id in self.connected_sensors_IDs :
				all_data = np.concatenate((all_data, self.received_sensors_data[id]))
				self.new_sensors_data[id] = False

			# use this data for training the GMM
			self.DynamicTrainingData[self.xmm_training_label][-1].append(all_data.tolist())

	################################
	# TRAINING SET CLEARING
	# resets full training sets
	def gmm_clear_train(self, address, argt) :
		self.gmm_stop_train(address, argt)

		# resets full training sets
		for training_label in self.DynamicTrainingData.keys() :
			# resets the data
			self.DynamicTrainingData[training_label] = [[]]

	################################################################################################
	# SAVE/LOAD THE TRAINING SET INTO FILES
	################################################################################################
	####################################################
	# saves full training data and loads them for subsequent classification
	def gmm_save_and_load_train(self, address, argt) :
		self.gmm_save_train(address, argt)
		self.gmm_load_train(address, argt)

	####################################################
	# saves full training data 
	def gmm_save_train(self, address, argt) :
		if self.xmm_train == True :
			print("end training")
			self.xmm_train = False

		print("clean previous training set")
		# loads full training sets
		if self.DIR_TrainingData_name != '' :
			subdirs = [sd for sd in listdir(self.DIR_TrainingData_name) if isdir(join(self.DIR_TrainingData_name,sd))]
			print("subdirs", subdirs)
			for subdir in subdirs :
				if subdir in self.XMM_training_labels :
					DIR_TrainingData_name_subdir = join(self.DIR_TrainingData_name,subdir)
					# cleans the directory
					print("cleans the directory", DIR_TrainingData_name_subdir)
					for files in listdir(DIR_TrainingData_name_subdir):
						path = join(DIR_TrainingData_name_subdir, files)
						try:
							shutil.rmtree(path)
						except OSError:
							remove(path)

		print("save training data")
		# saves full training sets
		if self.DIR_TrainingData_name != '' :
			if not exists(self.DIR_TrainingData_name):
				makedirs(self.DIR_TrainingData_name)
			for training_label, training_data_values in self.DynamicTrainingData.items() :
				DIR_TrainingData_name_subdir = self.DIR_TrainingData_name + training_label + '\\'
				if not exists(DIR_TrainingData_name_subdir):
					makedirs(DIR_TrainingData_name_subdir)
				self.FILE_TrainingData_name[training_label] = []
				# print("label values", training_label, len(training_data_values))
				for indTrainingSet in range(len(training_data_values)) :
					file_TrainingData_path = join(DIR_TrainingData_name_subdir, 'gmm_training_data' + str(indTrainingSet + 1) + '.txt')
					print("save training set", file_TrainingData_path, "(", training_label + '\\' + str(indTrainingSet + 1) + ")")
					np.savetxt(file_TrainingData_path, np.array(training_data_values[indTrainingSet]), delimiter=' ', fmt="%.5f")
					self.FILE_TrainingData_name[training_label].append(file_TrainingData_path)
		else :
			print("DIR_TrainingData_name empty")

	####################################################
	# Loads full training data for subsequent classification
	# Training set loading: one file/full set
	def gmm_load_one_training_set(self, fileName, ind_GMM, training_label) :
		# Loads data phrases
		# print("training file", fileName)
		phrase = np.genfromtxt(fileName)
		print("training set", ind_GMM, training_label)
		self.TrainingSet.addPhrase(ind_GMM, training_label)
		for frame in phrase:
			# Append data frame to the phrase ind_GMM
			self.TrainingSet.getPhrase(ind_GMM).record(frame)

	# loads full training sets
	def gmm_load_train(self, address, argt) :
		print("load training set")
		# loads full training sets
		if self.DIR_TrainingData_name != '' :
			subdirs = [sd for sd in listdir(self.DIR_TrainingData_name) if isdir(join(self.DIR_TrainingData_name,sd))]
			self.xmm_GMM_index = 0
			for subdir in subdirs :
				if subdir in self.XMM_training_labels :
					training_label = subdir
					self.FILE_TrainingData_name[training_label] = []
					DIR_TrainingData_name_subdir = join(self.DIR_TrainingData_name,subdir)

					# Create the training set
					if self.TrainingSet == None :
						self.TrainingSet = xmm.TrainingSet()
						self.TrainingSet.dimension.set(self.data_dimension) # dimension of data in this example

					subfiles = [f for f in listdir(DIR_TrainingData_name_subdir) if isfile(join(DIR_TrainingData_name_subdir,f))]
					# one index per GMM (and per learned gesture)
					if subfiles != [] :
						for f in subfiles :
							DIR_TrainingData_name_subdir_file = join(DIR_TrainingData_name_subdir, f)
							self.FILE_TrainingData_name[training_label].append(DIR_TrainingData_name_subdir_file)

							print("load training set", DIR_TrainingData_name_subdir_file, "(", training_label + '\\' + str(self.xmm_GMM_index), ")")
							self.gmm_load_one_training_set(DIR_TrainingData_name_subdir_file, self.xmm_GMM_index, training_label)

						self.xmm_GMM_index += 1

			# uses these data to train a model
			self.GMM_Model = self.gmm_train_model_from_data(4, [0.01, 0.0001])
		else :
			print("DIR_TrainingData_name empty")

	################################################################################################
	# USES THE TRAINING SET FOR CLASSIFICATION
	################################################################################################

	# start/stops recognition mode
	def gmm_start_stop_reco(self, address, argt) :
		if self.xmm_reco == False :
			self.gmm_start_reco(address, argt)
		else :
			self.gmm_stop_reco(address, argt)

	# start recognition mode
	def gmm_start_reco(self, address, argt) :
		if self.xmm_train == True :
			print("end training")
			self.xmm_train = False

			print("record training")
			self.gmm_record_train(address, argt)

			print("load training set")
			self.gmm_load_train(address, argt)

		self.xmm_reco = True
		print("start reco")
		pg_OSC.UDP_emit("/xmm_start_stop_reco", "udp_TouchOSC_send", [1])

	# stops recognition mode
	def gmm_stop_reco(self, address, argt) :
		print("end reco")
		self.xmm_reco = False
		pg_OSC.UDP_emit("/xmm_start_stop_reco", "udp_TouchOSC_send", [0])

	# calculate likelihood values from one line data
	def gmm_reco_on_the_fly(self, sensor_ID, data_array):
		""" Simple Test Function for Gaussian Mixture Models.
		Args:
			likelihood_window -- size of the smoothing window for recognition
		"""
		self.received_sensors_data[sensor_ID] = data_array
		self.new_sensors_data[sensor_ID] = True
		# Performance: Play test data and record the likelihoods of the modes
		# loads 1 line of test data
		if data_array.size != (self.data_dimension / len(self.connected_sensors_IDs)) :
			print("incorrect test data size given {} expected {}".format(data_array.size, self.data_dimension))
			return

		# if all data is true, use the testing data and set all data to false
		all_data_new = True
		for id in self.connected_sensors_IDs :
			if self.new_sensors_data[id] == False :
				all_data_new = False
				break

		# all sensor data is new
		if all_data_new == True :
			# concatenates all the data from all the sensors and sets new to false
			all_data = np.array([])
			for id in self.connected_sensors_IDs :
				all_data = np.concatenate((all_data, self.received_sensors_data[id]))
				self.new_sensors_data[id] = False

			# use this data for testing based on the trained GMM
			# initialization of the likelihoods for each modelq 	
			# print("calculate likelihoods")
			# print("self.GMM.size()",self.GMM.size())
			instantaneous_likelihoods = np.zeros(self.GMM.size(), float)
			normalized_likelihoods = np.zeros(self.GMM.size(), float)
			log_likelihoods = np.zeros(self.GMM.size(), float)
			self.nb_samples_reco += 1

			# the GMM receives the new parameter values
			self.GMM.filter(all_data)

			# and we can collect the corresponding instant likelihood values
			instantaneous_likelihoods[:] = np.array(self.GMM.results.instant_likelihoods)
			normalized_likelihoods[:] = np.array(self.GMM.results.smoothed_normalized_likelihoods)
			log_likelihoods[:] = np.array(self.GMM.results.smoothed_log_likelihoods)

			rank = 0
			max_likelihood = -1
			max_likelihood_index = -1
			for normed_likelihood in normalized_likelihoods :
				# interface updating
				if self.nb_samples_reco % 10 == 0 :
					pg_OSC.UDP_emit("/xmm_likelihood_"+str(rank+1), "udp_TouchOSC_send",  [float(normed_likelihood)])
				# selects the maximal value
				if normed_likelihood > max_likelihood :
					max_likelihood = normed_likelihood
					max_likelihood_index = rank
				rank += 1
				# print("#1:{:.2f} #2:{:.2f} #3:{:.2f}".format(normalized_likelihoofirst_max_likelihood_for_indexds[0],normalized_likelihoods[1],normalized_likelihoods[2]))
			##############################
			# looks for sustained likelihood above threshold during a minimal time
			# the model with the highest likelihood is the same as the preceding one
			if self.current_max_likelihood_index == max_likelihood_index :
				# value above threshold
				if max_likelihood >= self.XMM_likelihood_threshold :
					# the starting time of the sustain has already been set 
					if self.first_max_likelihood_for_index >= 0 :
						# has reached the minimal duration, porphyrograph is informed so that it can possibly display the corresponding visual clue
						if time() - self.first_max_likelihood_for_index > self.XMM_likelihood_minimal_duration :
							if self.XMM_forward_gesture_recognition :
								pg_OSC.UDP_emit("/ClipArt_on", "udp_porphyrograph_send", [self.XMM_training_labels_ClipArts[max_likelihood_index] + 1])
								# print("/ClipArt_on", "udp_porphyrograph_send", [self.XMM_training_labels_ClipArts[max_likelihood_index] + 1])
						# otherwise nothing changes, just ageing as index with a max value until it reaches the minimal duration of the sustain period
					# the starting time has not been set, should be now
					else :
					 	self.first_max_likelihood_for_index = time()
				# value under threshold: the max starting time is reset
				else :
					if self.XMM_forward_gesture_recognition :
						pg_OSC.UDP_emit("/ClipArt_off", "udp_porphyrograph_send", [self.XMM_training_labels_ClipArts[max_likelihood_index] + 1])
					self.first_max_likelihood_for_index = -1
			# the highest likelihood is attached to a model different from the preceding one
			else :
				if self.XMM_forward_gesture_recognition :
					pg_OSC.UDP_emit("/ClipArt_off", "udp_porphyrograph_send", [self.XMM_training_labels_ClipArts[self.current_max_likelihood_index] + 1])
				# stores the current max likelihood index
				self.current_max_likelihood_index = max_likelihood_index
				# stores the initial time if it is above threshold
				if max_likelihood >= self.XMM_likelihood_threshold :
				 	self.first_max_likelihood_for_index = time()
				# otherwise just waits until it reaches above threshold to start counting time above threshold
				else :
					self.first_max_likelihood_for_index = -1

			return normalized_likelihoods
		else :
			return np.zeros(self.GMM.size(), float)

################################################################################################
# ACCESS TO THE TRAINING CLASS
################################################################################################
def getGMM() :
    global pg_gmm
    return pg_gmm

def setGMM(loc_XMM_sensors_ID, loc_XMM_training_data_dir_name, loc_XMM_likelihood_window, loc_XMM_training_labels, loc_XMM_training_labels_ClipArts, loc_XMM_likelihood_threshold, loc_XMM_likelihood_minimal_duration, loc_XMM_forward_gesture_recognition, loc_XMM_forward_web_animation, loc_XMM_forward_sensor_masses_animation) :
	global pg_gmm
	pg_gmm = GMM_data(loc_XMM_sensors_ID, loc_XMM_training_data_dir_name, loc_XMM_likelihood_window, loc_XMM_training_labels, loc_XMM_training_labels_ClipArts, loc_XMM_likelihood_threshold, loc_XMM_likelihood_minimal_duration, loc_XMM_forward_gesture_recognition, loc_XMM_forward_web_animation, loc_XMM_forward_sensor_masses_animation)

if __name__ == '__main__':
	pre_recorded_training_and_test()
