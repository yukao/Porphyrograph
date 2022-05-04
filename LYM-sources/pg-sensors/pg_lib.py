#!/usr/bin/python3 

# vv_lib.py
# utilities for vector video

# licensed and released under Creative Commons 3.0 Attribution
# https://creativecommons.org/licenses/by/3.0/
# code by Yukao Nagemi

import re
import sys
from math import pow
from math import sqrt
from math import pi

import colorsys

from math import log
from math import pow

from collections import OrderedDict
from operator import itemgetter

##################################################################
# GEOMETRY CLASSES
##################################################################
class Vector:
	def __init__(self, x, y, z):
		self.vx = x
		self.vy = y
		self.vz = z

	def __add__(self, other):
		return Vector(self.vx + other.vx, self.vy + other.vy, self.vz + other.vz)

	def __sub__(self, other):
		return Vector(self.vx - other.vx, self.vy - other.vy, self.vz - other.vz)

	def __mul__(self, other):
		return Vector(self.vx * other, self.vy * other, self.vz * other)

	def dot(self, v):
		return self.vx * v.vx + self.vy * v.vy + self.vz * v.vz

	def norm(self):
		return sqrt(self.dot(self))

	def normalize(self):
		n = self.norm()
		if(n != 0):
			self.vx /= n
			self.vy /= n
			self.vz /= n
		return self

	def mul(self, k):
		self.vx *= k
		self.vy *= k
		self.vz *= k
		return self

class Point:
	def __init__(self, x, y, z):
		self.px = x
		self.py = y
		self.pz = z

	def __add__(self, v):
		return Point(self.px + v.vx, self.py + v.vy, self.pz + v.vz)

	def __sub__(self, other):
		return Vector(self.px - other.px, self.py - other.py, self.pz - other.pz)

