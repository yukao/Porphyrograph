/*! \file dm-texture.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-texture.cpp
 * 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "dm-all_include.h"

//////////////////////////////////////////////////////////////////////
// TEXT
//////////////////////////////////////////////////////////////////////

GLbyte stb__arial_10_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_10_usascii_y[95]={ 8,1,1,1,1,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_10_usascii_w[95]={ 0,2,3,5,5,8,6,2,3,3,4,5,2,3,
2,3,5,4,5,5,5,5,5,5,5,5,2,2,5,5,5,5,9,7,6,7,6,6,6,7,6,2,4,6,
5,7,6,7,6,7,7,6,6,6,6,9,6,6,6,3,3,2,4,7,3,5,5,5,5,5,3,5,5,2,
3,5,2,7,5,5,5,5,4,5,3,5,5,7,5,5,5,3,2,3,5, };
GLubyte stb__arial_10_usascii_h[95]={ 0,7,3,8,8,8,8,3,9,9,4,5,3,2,
1,8,8,7,7,8,7,8,8,7,8,8,5,7,6,4,6,7,9,7,7,8,7,7,7,8,7,7,8,7,
7,7,7,8,7,8,7,8,7,8,7,7,7,7,7,9,8,9,4,1,2,6,8,6,8,6,7,7,7,7,
9,7,7,5,5,6,7,7,5,6,8,6,5,5,5,7,5,9,9,9,2, };
GLubyte stb__arial_10_usascii_s[95]={ 127,80,80,58,76,82,91,84,1,37,72,
95,77,93,101,110,24,104,13,36,109,70,98,60,114,104,73,123,25,121,31,
7,20,115,66,46,97,90,83,9,73,29,53,53,47,39,32,41,22,1,14,
120,1,17,113,103,96,89,82,30,42,34,67,104,97,37,64,49,30,43,66,
70,60,120,16,8,123,113,107,61,54,76,76,19,49,55,101,87,67,1,81,
12,9,5,87, };
GLubyte stb__arial_10_usascii_t[95]={ 1,20,34,1,1,1,1,34,1,1,34,
28,34,34,34,1,11,20,28,11,20,1,1,20,1,1,28,20,28,28,28,
28,1,20,20,11,20,20,20,11,20,20,1,20,20,20,20,1,20,11,20,
1,20,11,11,11,11,11,11,1,11,1,34,34,34,28,1,28,11,28,11,
11,11,11,1,20,11,28,28,28,11,11,28,28,1,28,28,28,28,28,28,
1,1,1,34, };
GLubyte stb__arial_10_usascii_a[95]={ 40,40,51,80,80,127,96,27,
48,48,56,84,40,48,40,40,80,80,80,80,80,80,80,80,
80,80,40,40,84,84,84,80,145,96,96,103,103,96,87,111,
103,40,72,96,80,119,103,111,96,111,103,96,87,103,96,135,
96,96,87,40,40,40,67,80,48,80,80,72,80,80,40,80,
80,32,32,72,32,119,80,80,80,80,48,72,40,80,72,103,
72,72,72,48,37,48,84, };

GLbyte stb__arial_11_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_11_usascii_y[95]={ 8,0,0,0,0,0,0,0,0,0,0,2,7,5,
7,0,0,0,0,0,0,1,0,1,0,0,2,2,2,3,2,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,2,0,2,0,2,0,2,0,0,
0,0,0,2,2,2,2,2,2,2,1,2,2,2,2,2,2,0,0,0,3, };
GLubyte stb__arial_11_usascii_w[95]={ 0,2,4,6,6,9,7,2,3,3,4,6,2,3,
2,3,6,3,5,6,6,6,6,6,6,6,2,2,6,6,6,5,10,8,7,7,7,7,6,8,7,2,5,7,
6,8,7,8,7,8,7,7,6,7,7,10,7,7,6,3,3,3,5,7,3,6,6,5,5,6,4,5,5,2,
3,5,2,8,5,6,6,5,4,5,3,5,5,8,5,5,5,4,2,4,6, };
GLubyte stb__arial_11_usascii_h[95]={ 0,8,4,9,10,9,9,4,11,11,4,5,3,1,
1,9,9,8,8,9,8,8,9,7,9,9,6,8,5,3,5,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,10,9,10,5,1,3,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,8,8,6,7,8,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_11_usascii_s[95]={ 125,88,95,67,41,88,98,100,1,33,90,
76,107,124,125,106,39,9,36,52,29,22,81,60,110,1,125,122,69,110,83,
42,22,13,107,117,1,91,115,69,99,53,63,80,73,64,56,58,45,30,37,
8,24,16,12,1,117,109,102,48,59,37,63,103,103,73,74,67,46,87,78,
24,83,125,18,31,125,100,115,80,89,96,121,48,20,54,94,48,109,52,57,
13,10,5,117, };
GLubyte stb__arial_11_usascii_t[95]={ 10,23,40,1,1,1,1,40,1,1,40,
40,40,40,30,1,13,32,32,13,32,32,1,32,1,13,23,23,40,40,40,
32,1,32,23,1,32,23,23,13,23,23,13,23,23,23,23,1,23,13,23,
13,23,13,23,23,13,13,13,1,13,1,40,44,40,32,1,32,13,32,13,
13,13,1,1,23,13,32,32,32,13,13,32,32,23,32,32,40,32,1,40,
1,1,1,40, };
GLubyte stb__arial_11_usascii_a[95]={ 44,44,56,88,88,140,105,30,
52,52,61,92,44,52,44,44,88,88,88,88,88,88,88,88,
88,88,44,44,92,92,92,88,160,105,105,114,114,105,96,123,
114,44,79,105,88,131,114,123,105,123,114,105,96,114,105,149,
105,105,96,44,44,44,74,88,52,88,88,79,88,88,44,88,
88,35,35,79,35,131,88,88,88,88,52,79,44,88,79,114,
79,79,79,53,41,53,92, };


GLbyte stb__arial_12_usascii_x[95]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, };
GLubyte stb__arial_12_usascii_y[95]={ 9,1,1,1,0,1,1,1,1,1,1,2,7,5,
7,1,1,1,1,1,1,1,1,1,1,1,3,3,2,3,2,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_12_usascii_w[95]={ 0,3,4,6,6,9,7,2,4,4,4,6,3,4,
3,3,6,4,6,6,6,6,6,6,6,6,3,3,6,6,6,6,11,9,7,8,8,7,7,8,7,2,5,8,
6,9,7,8,7,8,8,7,7,7,8,11,8,8,7,3,3,3,5,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,6,6,6,4,5,3,6,6,8,6,6,6,4,2,4,6, };
GLubyte stb__arial_12_usascii_h[95]={ 0,8,4,9,11,9,9,4,11,11,4,6,4,2,
2,9,9,8,8,9,8,9,9,8,9,9,6,8,6,4,6,8,11,8,8,9,8,8,8,9,8,8,9,8,
8,8,8,9,8,9,8,9,8,9,8,8,8,8,8,11,9,11,5,2,2,7,9,7,9,7,8,9,8,8,
11,8,8,6,6,7,9,9,6,7,9,7,6,6,6,9,6,11,11,11,3, };
GLubyte stb__arial_12_usascii_s[95]={ 125,116,98,89,1,96,106,95,50,29,86,
55,91,110,123,121,41,1,56,55,70,69,114,31,85,10,62,77,66,79,18,
63,34,46,38,1,22,14,6,76,120,113,100,104,97,87,79,66,71,17,55,
33,47,92,31,19,10,1,115,46,106,21,73,114,110,88,62,101,48,115,110,
26,40,125,25,64,123,8,1,108,82,75,122,95,55,81,41,25,48,59,34,
16,13,8,103, };
GLubyte stb__arial_12_usascii_t[95]={ 10,21,41,1,1,1,1,41,1,1,41,
41,41,44,41,1,13,32,32,11,32,11,1,32,11,13,41,32,41,41,41,
32,1,32,32,13,32,32,32,11,21,21,11,21,21,21,21,1,21,13,21,
13,23,11,23,23,23,23,11,1,11,1,41,41,41,32,11,32,13,32,11,
13,23,1,1,21,11,41,41,32,1,1,32,32,1,32,41,41,41,1,41,
1,1,1,41, };
GLubyte stb__arial_12_usascii_a[95]={ 48,48,61,96,96,153,115,33,
57,57,67,100,48,57,48,48,96,96,96,96,96,96,96,96,
96,96,48,48,100,100,100,96,174,115,115,124,124,115,105,134,
124,48,86,115,96,143,124,134,115,134,124,115,105,124,115,162,
115,115,105,48,48,48,81,96,57,96,96,86,96,96,48,96,
96,38,38,86,38,143,96,96,96,96,57,86,48,96,86,124,
86,86,86,57,45,57,100, };

GLbyte stb__arial_13_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_13_usascii_y[95]={ 10,1,1,1,0,1,1,1,1,1,1,3,8,6,
8,1,1,1,1,1,1,1,1,1,1,1,3,3,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,11,1,3,1,3,1,3,1,3,1,1,
1,1,1,3,3,3,3,3,3,3,1,3,3,3,3,3,3,1,1,1,4, };
GLubyte stb__arial_13_usascii_w[95]={ 0,2,4,7,6,10,8,2,4,4,5,7,3,4,
2,4,6,4,6,6,6,7,6,6,6,6,2,3,7,7,7,6,12,9,8,8,8,8,7,9,8,2,5,8,
7,9,8,9,8,9,9,8,7,8,8,11,8,8,7,4,4,3,6,8,3,6,6,6,6,6,4,6,6,2,
3,6,2,9,6,7,7,6,5,6,4,6,6,9,6,6,6,4,1,4,7, };
GLubyte stb__arial_13_usascii_h[95]={ 0,9,4,10,12,10,10,4,12,12,5,6,4,2,
2,10,10,9,9,10,9,10,10,9,10,10,7,9,6,4,6,9,12,9,9,10,9,9,9,10,9,9,10,9,
9,9,9,10,9,10,9,10,9,10,9,9,9,9,9,12,10,12,6,2,3,8,10,8,10,8,9,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,12,12,12,3, };
GLubyte stb__arial_13_usascii_s[95]={ 56,125,1,93,1,101,112,123,51,28,105,
97,111,30,27,10,48,19,76,62,90,76,121,50,94,15,47,97,89,115,81,
83,33,66,57,1,41,32,24,84,10,122,110,1,114,104,95,68,83,22,66,
39,55,101,39,27,18,9,1,46,116,20,74,18,6,108,69,1,55,16,121,
32,48,63,24,76,92,30,23,8,85,78,122,115,56,101,40,50,67,61,60,
15,13,8,10, };
GLubyte stb__arial_13_usascii_t[95]={ 12,25,54,1,1,1,1,45,1,1,45,
45,45,54,54,14,14,35,35,14,35,14,1,35,14,14,45,35,45,45,45,
35,1,35,35,14,35,35,35,14,35,25,14,35,25,25,25,1,25,14,25,
14,25,14,25,25,25,25,25,1,14,1,45,54,54,35,14,45,14,45,14,
14,25,25,1,25,25,45,45,45,1,1,35,35,1,35,45,45,45,1,45,
1,1,1,54, };
GLubyte stb__arial_13_usascii_a[95]={ 52,52,66,104,104,166,124,36,
62,62,72,109,52,62,52,52,104,104,104,104,104,104,104,104,
104,104,52,52,109,109,109,104,189,124,124,134,134,124,114,145,
134,52,93,124,104,155,134,145,124,145,134,124,114,134,124,176,
124,124,114,52,52,52,87,104,62,104,104,93,104,104,52,104,
104,41,41,93,41,155,104,104,104,104,62,93,52,104,93,134,
93,93,93,62,48,62,109, };

GLbyte stb__arial_14_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,0,0,1,0,1,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_14_usascii_y[95]={ 11,2,2,1,1,1,1,2,1,1,1,3,9,7,
9,1,1,1,1,1,2,2,1,2,1,1,4,4,3,4,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,12,1,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_14_usascii_w[95]={ 0,2,4,7,7,11,9,2,4,4,5,7,2,4,
2,4,7,4,7,7,7,7,7,7,7,7,2,2,7,7,7,7,13,10,8,9,9,8,7,9,8,2,6,9,
7,10,9,10,8,10,9,8,8,9,9,12,9,9,8,4,4,3,6,9,3,7,7,7,7,7,4,7,7,2,
3,7,2,10,7,7,7,7,5,6,4,7,7,9,7,7,6,4,2,4,7, };
GLubyte stb__arial_14_usascii_h[95]={ 0,9,4,11,12,11,11,4,13,13,5,7,4,2,
2,11,11,10,10,11,9,10,11,9,11,11,7,9,7,5,7,10,13,9,9,11,9,9,9,11,9,9,10,9,
9,9,9,11,9,11,9,11,9,10,9,9,9,9,9,12,11,12,6,2,3,8,10,8,10,8,10,10,9,9,
12,9,9,7,7,8,10,10,7,8,10,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_14_usascii_s[95]={ 59,120,123,70,42,78,108,57,1,19,48,
104,54,123,72,118,1,9,22,9,1,1,17,110,90,45,59,107,62,40,88,
115,24,49,98,98,80,71,63,53,40,123,108,30,22,11,1,59,111,25,88,
36,69,30,49,98,78,59,40,50,123,55,33,75,60,27,84,51,76,12,71,
63,90,60,38,118,9,112,12,35,92,100,20,20,123,43,70,78,96,14,26,
14,11,6,64, };
GLubyte stb__arial_14_usascii_t[95]={ 13,27,48,1,1,1,1,57,1,1,57,
48,57,53,57,1,15,27,27,15,48,27,15,37,1,15,48,37,48,57,48,
15,1,37,37,1,37,37,37,15,37,27,15,38,38,38,38,1,27,15,27,
15,27,27,27,27,27,27,27,1,1,1,57,57,57,48,15,48,15,48,15,
15,37,37,1,37,48,48,57,48,15,15,57,48,15,48,48,48,48,27,57,
1,1,1,57, };
GLubyte stb__arial_14_usascii_a[95]={ 56,56,71,112,112,178,134,38,
67,67,78,117,56,67,56,56,112,112,112,112,112,112,112,112,
112,112,56,56,117,117,117,112,204,134,134,145,145,134,122,156,
145,56,100,134,112,167,145,156,134,156,145,134,122,145,134,189,
134,134,122,56,56,56,94,112,67,112,112,100,112,112,56,112,
112,45,45,100,45,167,112,112,112,112,67,100,56,112,100,145,
100,100,100,67,52,67,117, };

GLbyte stb__arial_15_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,1,1,1,0,1,1,0,0,
0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_15_usascii_y[95]={ 12,2,2,2,1,2,2,2,2,2,2,4,10,7,
10,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,13,2,4,2,4,2,4,2,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,2,2,2,6, };
GLubyte stb__arial_15_usascii_w[95]={ 0,2,5,8,7,12,9,2,4,4,5,8,2,5,
2,4,7,5,7,7,7,7,7,7,7,7,2,2,8,8,8,7,14,10,9,10,8,8,7,10,8,2,6,9,
7,11,8,10,8,10,9,9,8,8,9,13,9,9,8,4,4,3,6,9,4,7,7,7,7,7,5,7,7,3,
4,7,3,11,7,7,7,7,5,7,4,7,7,10,7,7,7,5,2,5,8, };
GLubyte stb__arial_15_usascii_h[95]={ 0,10,4,11,13,11,11,4,13,13,5,7,4,3,
2,11,11,10,10,11,10,11,11,10,11,11,7,9,7,5,7,10,13,10,10,11,10,10,10,11,10,10,11,10,
10,10,10,11,10,11,10,11,10,11,10,10,10,10,10,13,11,13,6,2,3,9,11,9,11,9,10,11,10,10,
13,10,10,8,8,9,11,11,8,9,11,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_15_usascii_s[95]={ 127,68,60,79,1,104,117,66,58,29,54,
1,69,72,102,1,25,12,105,67,97,83,59,43,109,33,100,123,21,45,103,
18,34,1,113,14,88,79,71,10,59,1,102,33,25,13,4,68,117,91,103,
41,86,1,72,58,48,38,29,49,117,54,38,92,78,42,6,34,75,58,122,
51,21,82,24,95,113,66,92,50,96,88,78,26,63,84,30,10,112,51,120,
18,15,9,83, };
GLubyte stb__arial_15_usascii_t[95]={ 1,39,61,1,1,1,1,61,1,1,61,
61,61,61,61,15,15,50,39,15,39,15,15,39,15,15,50,39,61,61,50,
50,1,50,39,15,39,39,39,27,39,39,15,39,39,39,39,1,27,15,27,
15,27,27,27,27,27,27,27,1,15,1,61,61,61,50,15,50,15,50,15,
15,27,27,1,27,27,50,50,50,1,1,50,50,1,50,61,61,50,39,50,
1,1,1,61, };
GLubyte stb__arial_15_usascii_a[95]={ 60,60,76,119,119,191,143,41,
72,72,84,125,60,72,60,60,119,119,119,119,119,119,119,119,
119,119,60,60,125,125,125,119,218,143,143,155,155,143,131,167,
155,60,107,143,119,179,155,167,143,167,155,143,131,155,143,203,
143,143,131,60,60,60,101,119,72,119,119,107,119,119,60,119,
119,48,48,107,48,179,119,119,119,119,72,107,60,119,107,155,
107,107,107,72,56,72,125, };

GLbyte stb__arial_16_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_16_usascii_y[95]={ 12,1,1,1,0,1,1,1,1,1,1,3,10,7,
10,1,1,1,1,1,1,1,1,1,1,1,4,4,3,4,3,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,13,1,4,1,4,1,4,1,4,1,1,
1,1,1,4,4,4,4,4,4,4,1,4,4,4,4,4,4,1,1,1,5, };
GLubyte stb__arial_16_usascii_w[95]={ 0,2,5,8,8,12,10,3,5,5,6,8,2,5,
2,4,8,5,8,8,8,8,8,8,8,8,2,2,8,8,8,8,15,11,8,10,9,8,8,11,9,2,7,9,
7,10,9,11,8,11,10,9,9,9,10,14,10,10,9,4,4,4,7,10,4,8,8,8,7,8,5,8,7,3,
4,8,3,12,7,8,8,7,5,7,4,7,7,11,8,8,7,5,2,5,8, };
GLubyte stb__arial_16_usascii_h[95]={ 0,11,5,12,14,12,12,5,15,15,5,8,5,2,
2,12,12,11,11,12,11,12,12,11,12,12,8,11,8,6,8,11,15,11,11,12,11,11,11,12,11,11,12,11,
11,11,11,12,11,12,11,12,11,12,11,11,11,11,11,14,12,14,7,2,3,9,12,9,12,9,11,12,11,11,
15,11,11,8,8,9,11,11,8,9,12,9,8,8,8,12,8,15,15,15,4, };
GLubyte stb__arial_16_usascii_s[95]={ 125,125,109,111,54,1,14,105,1,43,95,
60,102,82,79,120,85,93,31,102,49,13,94,1,25,45,124,125,69,86,14,
40,27,19,10,34,117,108,99,1,83,125,103,73,65,54,44,82,31,54,11,
75,1,22,102,87,76,65,55,63,120,49,78,68,115,66,111,83,94,101,41,
66,113,121,22,22,40,1,110,92,32,47,118,75,68,58,43,23,51,73,35,
16,13,7,115, };
GLubyte stb__arial_16_usascii_t[95]={ 13,17,67,1,1,17,17,67,1,1,67,
67,67,14,14,1,17,43,55,17,55,30,1,55,17,17,55,29,67,67,67,
55,1,55,55,17,43,43,43,30,43,1,1,43,43,43,43,1,43,17,43,
17,43,30,30,30,30,30,30,1,17,1,67,14,72,55,17,55,17,55,30,
17,30,30,1,43,43,67,55,55,30,30,55,55,1,55,67,67,67,1,67,
1,1,1,67, };
GLubyte stb__arial_16_usascii_a[95]={ 64,64,81,127,127,204,153,44,
76,76,89,134,64,76,64,64,127,127,127,127,127,127,127,127,
127,127,64,64,134,134,134,127,233,153,153,165,165,153,140,178,
165,64,115,153,127,191,165,178,153,178,165,153,140,165,153,216,
153,153,140,64,64,64,108,127,76,127,127,115,127,127,64,127,
127,51,51,115,51,191,127,127,127,127,76,115,64,127,115,165,
115,115,115,77,60,77,134, };

GLbyte stb__arial_17_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,1,
-1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_17_usascii_y[95]={ 13,2,2,1,1,1,1,2,1,1,1,4,11,8,
11,1,2,2,2,2,2,2,2,2,2,2,5,5,3,5,3,1,1,2,2,1,2,2,2,1,2,2,2,2,
2,2,2,1,2,1,2,1,2,2,2,2,2,2,2,2,1,2,1,15,2,4,2,4,2,4,1,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,1,1,1,6, };
GLubyte stb__arial_17_usascii_w[95]={ 0,2,5,9,8,13,10,3,5,5,6,9,2,5,
2,5,8,5,8,8,8,8,8,8,8,8,2,2,9,9,9,8,15,12,9,11,10,9,8,11,9,2,7,10,
7,11,9,12,9,12,10,10,9,9,11,15,11,11,9,3,5,4,7,10,4,8,8,8,8,8,5,8,7,2,
4,7,3,11,7,8,7,8,6,8,5,8,8,11,8,8,8,5,2,5,9, };
GLubyte stb__arial_17_usascii_h[95]={ 0,11,4,13,14,13,13,4,16,16,6,8,5,2,
2,13,12,11,11,12,11,12,12,11,12,12,8,11,9,5,9,12,16,11,11,13,11,11,11,13,11,11,12,11,
11,11,11,13,11,13,11,13,11,12,11,11,11,11,11,15,13,15,7,2,3,10,12,10,12,10,12,13,11,11,
15,11,11,9,9,10,13,13,9,10,12,9,8,8,8,12,8,16,16,16,3, };
GLubyte stb__arial_17_usascii_s[95]={ 127,71,121,80,58,90,1,11,1,22,114,
78,125,30,125,69,62,42,45,19,63,43,89,54,107,1,125,122,1,1,20,
28,28,32,22,21,11,1,113,33,99,124,75,78,70,58,89,67,32,45,13,
58,114,52,102,86,74,1,48,54,121,49,106,36,25,116,116,107,98,98,83,
12,72,125,44,24,109,30,49,89,113,104,42,80,37,11,97,66,57,10,88,
16,13,7,15, };
GLubyte stb__arial_17_usascii_t[95]={ 1,32,69,1,1,1,18,79,1,1,69,
69,54,79,60,18,32,45,57,32,57,32,18,57,18,32,45,45,69,79,69,
32,1,57,57,18,57,57,45,18,45,32,18,45,45,45,45,1,45,18,45,
18,32,32,32,32,32,45,45,1,1,1,69,79,79,57,18,57,18,57,18,
18,57,18,1,45,45,69,69,57,1,1,69,57,32,69,69,69,69,32,69,
1,1,1,79, };
GLubyte stb__arial_17_usascii_a[95]={ 68,68,86,135,135,216,162,46,
81,81,95,142,68,81,68,68,135,135,135,135,135,135,135,135,
135,135,68,68,142,142,142,135,247,162,162,176,176,162,149,189,
176,68,122,162,135,203,176,189,162,189,176,162,149,176,162,230,
162,162,149,68,68,68,114,135,81,135,135,122,135,135,68,135,
135,54,54,122,54,203,135,135,135,135,81,122,68,135,122,176,
122,122,122,81,63,81,142, };

GLbyte stb__arial_18_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,1,0,1,1,1,0,1,1,0,1,
1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,1,0,0,0,0,0,1,1,
-1,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_18_usascii_y[95]={ 14,2,2,2,1,2,2,2,2,2,2,4,12,9,
12,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,16,2,5,2,5,2,5,2,5,2,2,
2,2,2,5,5,5,5,5,5,5,2,5,5,5,5,5,5,2,2,2,7, };
GLubyte stb__arial_18_usascii_w[95]={ 0,3,5,9,9,14,11,3,5,5,6,9,3,5,
3,5,9,6,9,9,9,9,9,9,9,9,3,3,9,9,9,9,16,12,9,11,10,9,9,12,10,3,7,10,
8,12,10,12,10,12,11,10,10,10,11,16,11,11,10,4,5,4,8,11,4,9,8,8,8,9,6,8,7,2,
4,7,2,12,7,9,8,8,5,8,5,7,8,12,8,8,8,6,2,5,9, };
GLubyte stb__arial_18_usascii_h[95]={ 0,12,5,13,15,13,13,5,16,16,6,9,5,2,
2,13,13,12,12,13,12,13,13,12,13,13,9,12,9,6,9,12,16,12,12,13,12,12,12,13,12,12,13,12,
12,12,12,13,12,13,12,13,12,13,12,12,12,12,12,16,13,16,7,2,3,10,13,10,13,10,12,13,12,12,
16,12,12,9,9,10,13,13,9,10,13,10,9,9,9,13,9,16,16,16,3, };
GLubyte stb__arial_18_usascii_s[95]={ 127,117,49,117,61,1,16,45,1,38,34,
116,41,86,82,50,99,121,76,118,96,10,28,43,33,56,1,106,5,24,66,
86,44,63,53,38,32,22,12,20,1,113,54,102,93,80,69,86,55,66,35,
88,21,43,1,110,98,86,75,33,62,23,15,70,55,118,1,10,109,29,68,
79,13,32,28,47,66,47,39,19,108,99,60,1,71,110,98,76,107,77,89,
16,13,7,60, };
GLubyte stb__arial_18_usascii_t[95]={ 1,46,83,1,1,18,18,83,1,1,83,
72,83,83,83,18,18,46,59,18,59,32,18,59,32,18,83,59,83,83,72,
59,1,59,59,18,59,59,59,32,59,46,32,46,46,46,46,1,46,18,46,
18,46,32,46,32,32,32,32,1,32,1,83,83,83,59,32,72,18,72,32,
18,46,46,1,46,46,72,72,72,1,1,72,72,1,59,72,72,72,1,72,
1,1,1,83, };
GLubyte stb__arial_18_usascii_a[95]={ 72,72,92,143,143,229,172,49,
86,86,100,151,72,86,72,72,143,143,143,143,143,143,143,143,
143,143,72,72,151,151,151,143,255,172,172,186,186,172,157,201,
// 143,143,72,72,151,151,151,143,262,172,172,186,186,172,157,201,
186,72,129,172,143,215,186,201,172,201,186,172,157,186,172,243,
172,172,157,72,72,72,121,143,86,143,143,129,143,143,72,143,
143,57,57,129,57,215,143,143,143,143,86,129,72,143,129,186,
129,129,129,86,67,86,151, };

//////////////////////////////////////////////////////////////////////
// LOAD IMAGE
//////////////////////////////////////////////////////////////////////


// load image data according to image encoding
void LoadImageData(Pchar strFileName, TextureEncoding *theTextureEncoding, 
		   int * sizeX, int * sizeY, int * bytesPerPixel, 
		   unsigned char ** bitmap , bool invert )
{
  // load image texture
  cv::Mat img = cv::imread(strFileName, CV_LOAD_IMAGE_UNCHANGED);
  if( !img.data ) {
    // If we can't load the file, quit!
    sprintf(ErrorStr, "Texture [%s] allocation error!" , strFileName);
    ReportError(ErrorStr);
    throw 43;
  }
  
  // printf( "Texture channels [%d] bytes [%d]\n" , 
  // 	  img.channels() , *bytesPerPixel );
  
  int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA : 
    (img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
  // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
  cv::cvtColor(img, img, colorTransform);
  cv::Mat result;
  if( invert ) 
    cv::flip(img, result , 0); // vertical flip
  else
    result = img;
  

  *sizeX = result.cols;
  *sizeY = result.rows;
  // *bitmap = result.data;
  *bytesPerPixel = (result.channels() == 4) ? 4 : 3;
  if( *theTextureEncoding == PNG_GRAY
      || *theTextureEncoding == PNGA_GRAY ) {
    *theTextureEncoding = PNG;
  }
  // printf( "Texture channels [%d] bytes [%d]\n" , 
  // 	  result.channels() , *bytesPerPixel );

  if( result.channels() == 4 && *theTextureEncoding != PNGA ) {
    sprintf( ErrorStr , "Error: PNG image [%s] should be RGB with Alpha channel!" , strFileName ); ReportError( ErrorStr ); throw 335;
  }
  if( result.channels() == 3 && *theTextureEncoding == PNGA ) {
    sprintf( ErrorStr , "Error: PNG image [%s] should be RGB without Alpha channel!" , strFileName ); ReportError( ErrorStr ); throw 335;
  }

  *bitmap = new unsigned char[result.cols * result.rows * (*bytesPerPixel) ];
  memcpy( *bitmap , result.data , result.cols * result.rows * (*bytesPerPixel) );
}

//////////////////////////////////////////////////////////////////////
// SAVE IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to save PNG to file; returns 0 on success, non-zero on error. */

int writepng(char *filename, char *description,
	     int x, int y, int width, int height ) {

  cv::Mat img( height, width, CV_8UC3 );

  // OpenGL's default 4 byte pack alignment would leave extra bytes at the
  //   end of each image row so that each full row contained a number of bytes
  //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
  //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
  //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
  //   the rows are packed as tight as possible (no row padding), set the pack
  //   alignment to 1.
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data);

  cv::Mat result;
  cv::flip(img, result , 0); // vertical flip
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_PNG_COMPRESSION);
  params.push_back(9);
  params.push_back(0);
  cv::imwrite( filename, result );

  return 0;
}


/* Attempts to save JPG to file; returns 0 on success, non-zero on error. */
int writejpg(char *filename, char *description,
	     int x, int y, int width, int height, int compression) {
  cv::Mat img( height, width, CV_8UC3 );

  // OpenGL's default 4 byte pack alignment would leave extra bytes at the
  //   end of each image row so that each full row contained a number of bytes
  //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
  //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
  //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
  //   the rows are packed as tight as possible (no row padding), set the pack
  //   alignment to 1. 
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data);

  cv::Mat result;
  cv::flip(img, result , 0); // vertical flip
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_JPEG_QUALITY);
  params.push_back(70);   // that's percent, so 100 == no compression, 1 == full 
  cv::imwrite( filename, result );

  return 0;
}


//////////////////////////////////////////////////////////////
/// IMAGE UTILITIES
//////////////////////////////////////////////////////////////

void CheckImageDimensions( int sizeX , int sizeY , LPSTR strFileName) {  
  // Check the image width
  int comp = 1;
  bool isValid = false;
  for(int i = 0; i < 16; i++) {
    if(sizeX == comp)
      isValid = true;
    comp <<= 1;
  }
  if(!isValid) {
    sprintf( ErrorStr , "Error: The image width (%d) is not a power of 2 [%s]!" , sizeX , strFileName ); ReportError( ErrorStr ); throw 341;
  }  
  
  // Check the image height
  comp = 1;
  isValid = false;
  for(int i = 0; i < 16; i++) {
    if(sizeY == comp)
      isValid = true;
    comp <<= 1;
  }
  if(!isValid) {
    sprintf( ErrorStr , "Error: The image height (%d) is not a power of 2 [%s]!" , sizeY , strFileName ); ReportError( ErrorStr ); throw 342;
  }  
}

//////////////////////////////////////////////////////////////////////
// GLOBAL TEXTURE MANAGEMENT (AT COMPOSITION LEVEL)
//////////////////////////////////////////////////////////////////////

void dm_Composition::LoadTextureInRAM( Pchar strFileName ,  
				       int textureNo , 
				       TextureEncoding *theTextureEncoding  ,
				       int textureDimension ,
				       bool is_rectangle ,
				       Texturing texturing ,
				       bool invert ) {
  // Return from the function if no file name was passed in
  if(!strFileName)		
    return;

  int w, h, bytesPerPixel;

  // printf("%s encoding texture No %d [%s]\n", strFileName, textureNo, TextureEncodingString[theTextureEncoding]);

  // 1D or 2D textures
  if(textureDimension <= 2) {
    // Load the image
    
    // Store the data
    switch(*theTextureEncoding) {
    case JPG:
      pImageJPG[textureNo] = new tImageJPG;
      LoadImageData(strFileName, theTextureEncoding, &w, &h, 
		    &bytesPerPixel, &(pImageJPG[textureNo]->data), invert);
      pImageJPG[textureNo]->rowSpan = w * bytesPerPixel;
      pImageJPG[textureNo]->sizeX = w;
      pImageJPG[textureNo]->sizeY = h;
      // pImageJPG[textureNo]->sizeZ = 0;
      break;
    case PNG:
    case PNGA:
    case PNG_GRAY:
    case PNGA_GRAY:
      pImagePNG[textureNo] = new tImagePNG;
      LoadImageData(strFileName, theTextureEncoding, &w, &h, &bytesPerPixel, &(pImagePNG[textureNo]->data), invert);
      pImagePNG[textureNo]->bytesPerPixel = bytesPerPixel;
      pImagePNG[textureNo]->sizeX = w;
      pImagePNG[textureNo]->sizeY = h;
      pImagePNG[textureNo]->sizeZ = 0;
      break;
    case RGB:
    case RAW:
    case EmptyTextureEncoding:
    default:
      sprintf(ErrorStr, "Error: unknown file type [%d] for texture [%s]!", *theTextureEncoding, strFileName);
      ReportError(ErrorStr);
      throw 255;
      break;
    }
  }
  // 3D texture
  else {
    int x_tmp, y_tmp, px_tmp;
    long int ind = 0, size = 0;
    int nb_files = 0;
    unsigned char * data = NULL;

    // hypothesis: the given name is "Something.ext"
    // with ext == "jpg" or ext == "png"   
    // we look for "Something_001.ext", "Something_002.ext"
    w = -1;
    h = -1;
    bytesPerPixel = -1;
    
    // COUNTS THE FILES
    do {
      FILE *fp;
      char tempVal[XMLAttrValLength];
      strcpy( tempVal , strFileName );
      tempVal[ strlen( tempVal ) - 4 ] = 0;
      sprintf( tempVal , "%s_%03d%s" ,  tempVal , nb_files + 1 ,  
	       &strFileName[ strlen( strFileName ) - 4 ] );
      fp = fopen(tempVal,"r");
      if( !fp ) {
	break;
      }
      fclose( fp );
      nb_files++;
    } while( true );
    
    char ** filename = new char*[nb_files];

    for(int num_file = 0; num_file < nb_files; num_file++) {
      filename[num_file] = new char[XMLAttrValLength];
      strcpy( filename[num_file] , strFileName );
      filename[num_file][ strlen( filename[num_file] ) - 4 ] = 0;
      sprintf( filename[num_file] , "%s_%03d%s" ,  filename[num_file] , 
	       num_file + 1 ,  
	       &strFileName[ strlen( strFileName ) - 4 ] );
      // printf("Loads 3D texture component %s\n" , filename[num_file] );
    }
    
    // CHECKS ALL 2D TEXTURES HAVE THE SAME DIMENSION

    // loads texture #0
    LoadImageData(filename[0], theTextureEncoding, &w, &h, &bytesPerPixel, &data, invert);
    delete [] data;
    data = NULL;
    
    if(w == -1 || h == -1 || bytesPerPixel == -1) {
      sprintf(ErrorStr, "Error: error while reading texture size for 3D texture [%s]!" , filename[0]);
      ReportError(ErrorStr);
      throw 255;
    }
    
    // loop over the texture files
    // loads texture #1 -> #nb_files - 1
    for(int num_file = 1; num_file < nb_files; num_file++) {
      x_tmp = -1;
      y_tmp = -1;
      px_tmp = -1;
      
      LoadImageData(filename[num_file], theTextureEncoding, &x_tmp, &y_tmp, &px_tmp, &data, invert);
      delete [] data;
      data = NULL;
      
      if(x_tmp == -1 || y_tmp == -1 || px_tmp == -1) {
	sprintf(ErrorStr, "Error: error while reading texture size for 3D texture [%s]!", filename[num_file]);
	ReportError(ErrorStr);
	throw 255;
      }
      
      if(x_tmp != w || y_tmp != h || px_tmp != bytesPerPixel) {
	sprintf(ErrorStr, "Error: 2D textures do not have the same dimension for 3D texture [%s]!", filename[num_file]);
	ReportError(ErrorStr);
	throw 255;
      }
    }

    // BUILDS THE 3D TEXTURE

    size = w * h * bytesPerPixel;
    unsigned char * bitmap = new unsigned char[size * nb_files];
    
    // If we can't allocate data, quit!
    if(bitmap == NULL) {		  
      strcpy(ErrorStr, "Texture allocation error!");
      ReportError(ErrorStr);
      throw 425;
    }
      
    ind = 0;
    // copies the series of 2D textures into the 3D texure
    for(int num_file = 0; num_file < nb_files; num_file++) {
      x_tmp = -1;
      y_tmp = -1;
      px_tmp = -1;
      
      // GLubyte * data_tmp 
      printf("Load %s texture (%s)\n", TextureEncodingString[*theTextureEncoding], filename[num_file]);
      LoadImageData(filename[num_file], theTextureEncoding, &x_tmp, &y_tmp, &px_tmp, &data, invert);
      
      if(data == NULL) {
	strcpy(ErrorStr, "Texture allocation error!");
	ReportError(ErrorStr);
	throw 425;
      }
      
      if(x_tmp == -1 || y_tmp == -1 || px_tmp == -1) {
	sprintf(ErrorStr, "Error: error while reading texture size for 3D texture [%s]!", filename[num_file]);
	ReportError(ErrorStr);
	throw 255;
      }
      
      if(x_tmp != w || y_tmp != h || px_tmp != bytesPerPixel) {
	sprintf(ErrorStr, "Error: 2D textures do not have the same dimension for 3D texture [%s]!", filename[num_file]);
	ReportError(ErrorStr);
	throw 255;
      }
      
      for(long int i = 0; i < size ; i++)
	bitmap[ind++] = data[i];
      
      // memory release
      if(data) {
	delete [] data;
	data = NULL;
      }
    }

    switch(*theTextureEncoding) {
    case JPG:
      pImageJPG[textureNo] = new tImageJPG;
      pImageJPG[textureNo]->data = bitmap;
      pImageJPG[textureNo]->rowSpan = w * bytesPerPixel;
      pImageJPG[textureNo]->sizeX = w;
      pImageJPG[textureNo]->sizeY = h;
      // pImagePNG[textureNo]->sizeZ = nb_files;
      // printf("Texture pointer %d\n", pImageJPG[textureNo]->data);
      break;
    case PNG:
    case PNGA:
    case PNG_GRAY:
    case PNGA_GRAY:
      pImagePNG[textureNo] = new tImagePNG;
      pImagePNG[textureNo]->bytesPerPixel = bytesPerPixel;
      pImagePNG[textureNo]->data = bitmap;
      pImagePNG[textureNo]->sizeX = w;
      pImagePNG[textureNo]->sizeY = h;
      pImagePNG[textureNo]->sizeZ = nb_files;
      // printf("Texture pointer %d\n", pImagePNG[textureNo]->data);
      break;
    case RGB:
    case RAW:
    case EmptyTextureEncoding:
    default:
      sprintf(ErrorStr, "Error: unknown file type [%d] for texture [%s]!", *theTextureEncoding, strFileName);
      ReportError(ErrorStr);
      throw 255;
      break;
    } 
    
    if(ind != (size * nb_files)) {		  
      strcpy(ErrorStr, "Error: incomplete 3D texture load!");
      ReportError(ErrorStr);
	throw 425;
    }

    // printf("3D texture loaded: %d 2D bitmaps (%dx%d pixels)\n", nb_files, w, h);
    // printf("texture No: %d ID: %d\n", textureNo, (int)pImagePNG[textureNo]->data);
    for(int num_file = 0; num_file < nb_files; num_file++) {
      free(filename[num_file]);
    }
    free(filename);
  }

  if( !is_rectangle ) {
    CheckImageDimensions(w, h, strFileName);
  }
}

void dm_Composition::GenerateTextureInRAM( int vertexShaderParameterSize , 
					   int textureNo ,
					   bool is_rectangle ) {

  // Return from the function if no file name was passed in
  // Load the image and store the data
  pImageRaw[ textureNo ].dataFloat 
    = new float[ vertexShaderParameterSize * 4 ]; 
  pImageRaw[ textureNo ].dataByte = NULL; 
  pImageRaw[ textureNo ].sizeX = vertexShaderParameterSize; 
  pImageRaw[ textureNo ].sizeY = 1; 
  pImageRaw[ textureNo ].sizeZ = 0; 
  pImageRaw[ textureNo ].bytesPerPixel = 4; 

  // If we can't load the file, quit!
  if(pImageRaw[ textureNo ].dataFloat == NULL) {		  
    strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
  }
  
  // printf( "color_stripe generated\n" );
  CheckImageDimensions( vertexShaderParameterSize , 1 , (char *)"vertex_shader_texture param" );
}

// frame buffer capture texture allocation
void dm_Composition::GenerateTextureInRAM( int textureNo ,
					   bool is_rectangle ,
					   TextureFormat texture_format ,
					   int nb_attachments ) {
  // rgb POT raw image allocation (without alpha channel)
  int sizeX ;
  int sizeY ;

  if( !is_rectangle ) {
    sizeX 
      = smallestPOT( DM_EnvironmentNode->DM_Window->getWindowWidth() );
    sizeY 
      = smallestPOT( DM_EnvironmentNode->DM_Window->getWindowHeight() );
  }
  else {
    sizeX 
      = DM_EnvironmentNode->DM_Window->getWindowWidth();
    sizeY 
      = DM_EnvironmentNode->DM_Window->getWindowHeight();
  }

  // printf( "Frame buffer Window %d %d texture %dx%d textureNo %d nb_attachments %d\n" ,  
  // 	  DM_EnvironmentNode->DM_Window->getWindowWidth() , 
  // 	  DM_EnvironmentNode->DM_Window->getWindowHeight() , 
  // 	  sizeX , sizeY , textureNo , nb_attachments );

  // Return from the function if no file name was passed in
  // Load the image and store the data
  for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
    if( texture_format == byte_tex_format ) {
      pImageRaw[ textureNo + ind ].dataByte = new GLubyte[ sizeX * sizeY * 4 ]; 
      pImageRaw[ textureNo + ind ].dataFloat = NULL; 
      // If we can't load the file, quit!
      if(pImageRaw[ textureNo + ind ].dataByte == NULL) {		  
	strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
      }
      GLubyte *pt = pImageRaw[ textureNo + ind ].dataByte;
      int indTex = 0;
      for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
	pt[ indTex ] = (GLubyte)(OpenGLBGColor[0]*255);
	pt[ indTex + 1 ] = (GLubyte)(OpenGLBGColor[1]*255);
	pt[ indTex + 2 ] = (GLubyte)(OpenGLBGColor[2]*255);
	pt[ indTex + 3 ] = (GLubyte)255;
	indTex += 4;
      }
    }
    else {
      pImageRaw[ textureNo + ind ].dataFloat = new float[ sizeX * sizeY * 4 ]; 
      pImageRaw[ textureNo + ind ].dataByte = NULL; 
      // If we can't load the file, quit!
      if(pImageRaw[ textureNo + ind ].dataFloat == NULL) {		  
	strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
      }
      float *pt = pImageRaw[ textureNo + ind ].dataFloat;
      int indTex = 0;
      for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
	pt[ indTex ] = (OpenGLBGColor[0]);
	pt[ indTex + 1 ] = (OpenGLBGColor[1]);
	pt[ indTex + 2 ] = (OpenGLBGColor[2]);
	pt[ indTex + 3 ] = 1.0;
	indTex += 4;
      }
    }
    pImageRaw[ textureNo + ind ].sizeX = sizeX; 
    pImageRaw[ textureNo + ind ].sizeY = sizeY; 
    pImageRaw[ textureNo + ind ].sizeZ = 0; 
    pImageRaw[ textureNo + ind ].bytesPerPixel = 4; 
  }
   
  // printf( "color_stripe generated\n" );
  if( !is_rectangle ) {
    CheckImageDimensions( sizeX , sizeY , (char *)"frame_buffer" );
  }

}

// general texture allocation
void dm_Composition::GenerateTextureInRAM( int textureNo ,
					   TextureFormat texture_format ,
					   int nb_attachments ,
					   int sizeX ,
					   int sizeY ) {
  // rgb POT raw image allocation (without alpha channel)
  // printf( "Texture %dx%d textureNo %d nb_attachments %d\n" ,  
  // 	  sizeX , sizeY , textureNo , nb_attachments );

  // Return from the function if no file name was passed in
  // Load the image and store the data
  for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
    if( texture_format == byte_tex_format ) {
      pImageRaw[ textureNo + ind ].dataByte = new GLubyte[ sizeX * sizeY * 4 ]; 
      pImageRaw[ textureNo + ind ].dataFloat = NULL; 
      // If we can't load the file, quit!
      if(pImageRaw[ textureNo + ind ].dataByte == NULL) {		  
	strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
      }
      GLubyte *pt = pImageRaw[ textureNo + ind ].dataByte;
      int indTex = 0;
      for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
	pt[ indTex ] = (GLubyte)(OpenGLBGColor[0]*255);
	pt[ indTex + 1 ] = (GLubyte)(OpenGLBGColor[1]*255);
	pt[ indTex + 2 ] = (GLubyte)(OpenGLBGColor[2]*255);
	pt[ indTex + 3 ] = (GLubyte)255;
	indTex += 4;
      }
    }
    else {
      pImageRaw[ textureNo + ind ].dataFloat = new float[ sizeX * sizeY * 4 ]; 
      pImageRaw[ textureNo + ind ].dataByte = NULL; 
      // If we can't load the file, quit!
      if(pImageRaw[ textureNo + ind ].dataFloat == NULL) {		  
	strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
      }
      float *pt = pImageRaw[ textureNo + ind ].dataFloat;
      int indTex = 0;
      for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
	pt[ indTex ] = (OpenGLBGColor[0]);
	pt[ indTex + 1 ] = (OpenGLBGColor[1]);
	pt[ indTex + 2 ] = (OpenGLBGColor[2]);
	pt[ indTex + 3 ] = 1.0;
	indTex += 4;
      }
    }
    pImageRaw[ textureNo + ind ].sizeX = sizeX; 
    pImageRaw[ textureNo + ind ].sizeY = sizeY; 
    pImageRaw[ textureNo + ind ].sizeZ = 0; 
    pImageRaw[ textureNo + ind ].bytesPerPixel = 4; 
  }
}

// defines texture management parameters (function attached to the class
// dm_Composition that manages the global data for a composition)
void dm_Composition::TextureParameters( int              *width,
					int              *height,
					int              *depth,
					bool             *has_alpha,
					const void      **data,
					GLint            *components,
					GLenum           *format,
					GLenum           *datatype,
					GLenum           *texturefilter,
					GLenum           *mipmapfilter,
					int               textureNo , 
					TextureEncoding   encoding,
					TextureFormat     texture_format ,
					TextureFilter     texture_min_filter ,
					TextureFilter     texture_max_filter ) {

  // LOADS THE TEXTURE IN THE GPU
  // printf("load texture %d (%s)\n" , textureNo);1
  // printf( "Loading texture No %d ID %d (%s)\n" , textureNo , DM_CompositionNode->GetTextureID( textureNo ) , DM_CompositionNode->texture_names[textureNo] );
  if( encoding == JPG ) {
    *width = pImageJPG[ textureNo ]->sizeX;
    *height = pImageJPG[ textureNo ]->sizeY;
    *depth = 1;
    *has_alpha = false;
    *data = (const void *)(pImageJPG[ textureNo ]->data);
    *components = GL_RGB8;
    *format = GL_RGB;
    *datatype = GL_UNSIGNED_BYTE;
  }
  else if( encoding == PNG_GRAY ) {
    *width = pImagePNG[ textureNo ]->sizeX;
    *height = pImagePNG[ textureNo ]->sizeY;
    *depth = pImagePNG[ textureNo ]->sizeZ;
    *has_alpha = false;
    *data = (const void *)(pImagePNG[ textureNo ]->data);
    *components = GL_RGB8;
    *format = GL_LUMINANCE;
    *datatype = GL_UNSIGNED_BYTE;
  }
  else if( encoding == PNG ) {
    *width = pImagePNG[ textureNo ]->sizeX;
    *height = pImagePNG[ textureNo ]->sizeY;
    *depth = pImagePNG[ textureNo ]->sizeZ;
    *has_alpha = false;
    *data = (const void *)(pImagePNG[ textureNo ]->data);
    *components = GL_RGB8;
    *format = GL_RGB;
    *datatype = GL_UNSIGNED_BYTE;
  }
  else if( encoding == PNGA ) {
    *width = pImagePNG[ textureNo ]->sizeX;
    *height = pImagePNG[ textureNo ]->sizeY;
    *depth = pImagePNG[ textureNo ]->sizeZ;
    *has_alpha = true;
    *data = (const void *)(pImagePNG[ textureNo ]->data);
    *components = GL_RGBA8;
    *format = GL_RGBA;
    *datatype = GL_UNSIGNED_BYTE;
  }
  else if( encoding == PNGA_GRAY ) {
    *width = pImagePNG[ textureNo ]->sizeX;
    *height = pImagePNG[ textureNo ]->sizeY;
    *depth = pImagePNG[ textureNo ]->sizeZ;
    *has_alpha = true;
    *data = (const void *)(pImagePNG[ textureNo ]->data);
    *components = GL_RGBA8;
    *format = GL_RGBA;
    *datatype = GL_UNSIGNED_BYTE;
  }
  else if( encoding == RAW ) {
    *width = pImageRaw[ textureNo ].sizeX;
    *height = pImageRaw[ textureNo ].sizeY;
    *depth = pImageRaw[ textureNo ].sizeZ;
    *has_alpha = true;
    if( pImageRaw[ textureNo ].dataByte ) {
      // printf( "Byte texture %s\n" , DM_CompositionNode->texture_names[textureNo] );
      *data = (const void *)(pImageRaw[ textureNo ].dataByte);
      *components = GL_RGBA8;
      *format = GL_RGBA;
      if( texture_format == float_tex_format ) {
	sprintf( ErrorStr , "Incoherent texture format (byte texture typed as float) [%s]!" , DM_CompositionNode->texture_names[textureNo] ); ReportError( ErrorStr ); throw 25;
      }
      *datatype = GL_UNSIGNED_BYTE;
    }
    else if( pImageRaw[ textureNo ].dataFloat ) {
      // printf( "Float texture %s\n" , DM_CompositionNode->texture_names[textureNo] );
      *data = (const void *)(pImageRaw[ textureNo ].dataFloat);
      *components = GL_RGBA8;
      *format = GL_RGBA;
      if( texture_format == byte_tex_format ) {
	sprintf( ErrorStr , "Incoherent texture format (float texture typed as byte) [%s]!" , DM_CompositionNode->texture_names[textureNo] ); ReportError( ErrorStr ); throw 25;
      }
      *datatype = GL_FLOAT;
    }
  }
  if( texture_min_filter == linear_filter ) {
    *mipmapfilter = GL_LINEAR_MIPMAP_LINEAR;
    *texturefilter = GL_LINEAR;
  }
  else // if ( texture_min_filter == nearest_filter ) 
    {
    *mipmapfilter = GL_NEAREST_MIPMAP_NEAREST;
    *texturefilter = GL_NEAREST;
  }

}

// transfers a texture from CPU to GPU (function attached to the class
// dm_Composition that manages the global data for a composition)
void dm_Composition::LoadTextureInGPUMemory( int textureNo,
					     int attachment,
					     int nb_attachments,
					     TextureEncoding encoding , 
					     TextureGenerationMode textureGenerationMode ,
					     int isMipMapped ,
					     int is_rectangle ,
					     TextureFormat texture_format ,
					     TextureFilter  texture_min_filter ,
					     TextureFilter  texture_max_filter ,
					     GLenum textureTarget ) {
  int               width = 0;
  int               height = 0;
  int               depth = 0;
  const void *      data = NULL;
  bool              has_alpha = true;
  GLint             components = GL_RGBA8;
  GLenum            format = GL_RGBA;
  GLenum            datatype = GL_UNSIGNED_BYTE;
  GLenum            texturefilter = GL_LINEAR;
  GLenum            mipmapfilter = GL_LINEAR_MIPMAP_LINEAR;

  // printf( ">>> Loading texture No %d attachment %d ID %d (%s)\n" , textureNo , attachment , DM_CompositionNode->GetTextureID( textureNo ) , DM_CompositionNode->texture_names[textureNo] );

  TextureParameters( &width , &height, &depth , &has_alpha , 
		     &data , &components , &format , &datatype ,
		     &texturefilter , &mipmapfilter , 
		     textureNo , encoding , texture_format ,
		     texture_min_filter , texture_max_filter );

  // no image - quit
  if( !data ) {
    // printf( "Empty texture (%d)\n" , textureNo );
    return;
  }

  // GENERATES THE TEXTURE ID
  // Generate a texture with the associative texture ID stored in the array
  // unless the texture already has an ID (can happen for an updated
  // keypoint texture)
  if( GetTextureID( textureNo ) == NULL_ID ) {
    if( nb_attachments > 1 && attachment > 0 ) {
      sprintf( ErrorStr , "Error: first attachment should be allocated before the other ones texture no [%d] texture name [%s]!" , textureNo , DM_CompositionNode->texture_names[textureNo] ); ReportError( ErrorStr ); throw 422;
    }

    GLuint texID = 0;
    for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
      glGenTextures(1, &texID);
      SetTextureID( textureNo + ind , texID );
    }
  }

  // printf( "Texture %dx%dx%d ID [%d,%d]\n" , width,height,depth,textureNo,GetTextureID( textureNo ) );

  
  // BINDS THE TEXTURE 
  // Bind the texture to the texture arrays index and initXMLTag the texture

  ////////////
  // 1D textures
  if( textureTarget == GL_TEXTURE_1D ) {
    glBindTexture(GL_TEXTURE_1D, GetTextureID( textureNo + attachment ) );
    //printf( "texture No %d\n" , textureNo );
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    // DEFINES THE TEXTURE COPY FUNCTION
    // mipmapping
    if( isMipMapped ) {
      // we need to tell OpenGL the quality of our texture map.
      // is the smoothest.  GL_LINEAR_MIPMAP_NEAREST is faster than
      // GL_LINEAR_MIPMAP_LINEAR, but looks blochy and pixilated.  Good
      // for slower computers though.

      // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,  
		      mipmapfilter );
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, 
		      mipmapfilter );
      
      // printf( "Loads gluBuild2DMipmaps texture No %d internal_format=%d (%s)\n" , textureNo ,components  , DM_CompositionNode->texture_names[textureNo] );
      gluBuild1DMipmaps(GL_TEXTURE_1D, components ,
			width ,
			format, datatype, data );
    }
  }

  ////////////
  // 2D textures
  else if( textureTarget == GL_TEXTURE_2D ) {
    for( int ind_attachment = 0 ; ind_attachment < nb_attachments ;
	 ind_attachment++ ) {
      if( !is_rectangle ) {
	glBindTexture(GL_TEXTURE_2D, GetTextureID( textureNo + ind_attachment ) );
	// printf( "Bind texture 2D textureNo %d ID %d attachment %d\n" , textureNo , GetTextureID( textureNo  + ind_attachment) , ind_attachment + 1 );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      }
      else {
	glBindTexture(GL_TEXTURE_RECTANGLE, GetTextureID( textureNo + ind_attachment ));
	// printf( "Bind texture RECT textureNo %d ID %d attachment %d\n" , textureNo , GetTextureID( textureNo  + ind_attachment), ind_attachment + 1 );
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
      }
      
      // DEFINES THE TEXTURE COPY FUNCTION
      // mipmapping
      if( isMipMapped ) {
	// we need to tell OpenGL the quality of our texture map.
	// is the smoothest.  GL_LINEAR_MIPMAP_NEAREST is faster than
	// GL_LINEAR_MIPMAP_LINEAR, but looks blochy and pixilated.  Good
	// for slower computers though.
	
	// POT texture
	if( !is_rectangle ) {
	  // printf( "Mipmapping non rectangle 2D texture %s filter %d %d\n" , DM_CompositionNode->texture_names[GetTextureID( textureNo )] , mipmapfilter , GL_LINEAR_MIPMAP_LINEAR );
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
			  mipmapfilter );
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
			  mipmapfilter );
	  
	  // printf( "Loads gluBuild2DMipmaps 2D texture No %d  OpenGlID %d internal_format=%d (%s)\n" , textureNo ,GetTextureID( textureNo ) ,format  , DM_CompositionNode->texture_names[textureNo] );
	  gluBuild2DMipmaps(GL_TEXTURE_2D, components ,
			    width , height ,
			    format, datatype, data );
	}
	// rectangle texture
	else {
	  if( texture_format == float_tex_format ) {
	    // no mipmapping for float textures that are used with FBO
	    // mipmapping is made by the last parameter of glFramebufferTexture2DEXT
	    // and with GenerateMipmapEXT(GL_TEXTURE_2D)
	    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
			    texturefilter );
	    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
			    texturefilter );
	    glTexImage2D(  GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F ,
			   width, height, 0, GL_RGBA, GL_FLOAT, 0 );
	  }
	  else
	    if( texture_format == depth_tex_format ) {
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
			      texturefilter );
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
			      texturefilter );
	      glTexImage2D(  GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32,
			     width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );
	    }
	    else 
	      {
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
				texturefilter );
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
				texturefilter );
		// printf( "Loads gluBuild2DMipmaps RECT texture No %d ID %d %dx%d internal_format=%d (%s)\n" , textureNo , GetTextureID( textureNo ) , width , height , format  , DM_CompositionNode->texture_names[textureNo] );
		
		// using mipmaps with rectangle textures
		// does not seem to be meaningful...
		// gluBuild2DMipmaps( GL_TEXTURE_RECTANGLE, components,
		// 		       width , height ,
		// 		       format, datatype, data );
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, components,
			     width , height , 0 ,
			     format, datatype, data );
	      }
	}
	// rectangle
      }
      // mipmapped
      // copy not mipmapped
      else {
	if( !is_rectangle ) {
	  // printf( "Copy (non mipmamping) non rectangle 2D texture %s\n" , DM_CompositionNode->texture_names[GetTextureID( textureNo )]);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
			  texturefilter );
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
			  texturefilter );
	  
	  // printf( "Loads glTexImage2D 2D GL_RGB GL_RGBA_FLOAT32_ATI texture No %d internal_format=%d (%s)\n" , textureNo , format , DM_CompositionNode->texture_names[textureNo] );
	  // printf( "GPU transfer %s [%dx%d]\n" , DM_CompositionNode->texture_names[textureNo] , width , height );
	  glTexImage2D(GL_TEXTURE_2D, 0, components ,
		       width , height , 0 ,
		       format, datatype, data );
	}
	// rectangle texture
	else {
	  // printf( "Loads glTexImage2D No %d internal_format=%d texturefilter=%d (%s)\n" , textureNo , format , texturefilter , DM_CompositionNode->texture_names[textureNo] );
	  if( texture_format == float_tex_format ) {
	    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
			    texturefilter );
	    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
			    texturefilter );
	    glTexImage2D(  GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F ,
			   width, height, 0, GL_RGBA, GL_FLOAT, 0 );
	    // printf( "GPU transfer %s\n" , DM_CompositionNode->texture_names[textureNo] );
	  }
	  else
	    if( texture_format == depth_tex_format ) {
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
			      texturefilter );
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
			      texturefilter );
	      glTexImage2D(  GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32,
			     width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0 );
	    }
	    else {
	      // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
			      texturefilter);
	      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
			      texturefilter);
	      
	      // printf( "Loads glTexImage2D RECTANGLE No %d internal_format=%d (%s) %dx%d data type %d ptr %d\n" , textureNo , format , DM_CompositionNode->texture_names[textureNo] , width , height , datatype, (unsigned int)data);
	      // printf( "GPU transfer %s [%dx%d]\n" , DM_CompositionNode->texture_names[textureNo] , width , height );
	      glTexImage2D(GL_TEXTURE_RECTANGLE, 0, components ,
			   width , height , 0 ,
			   format, datatype, data );
	      // printf( "Loaded glTexImage2D RECTANGLE GL_RGB GL_RGBA_FLOAT32_ATI texture No %d internal_format=%d (%s)\n" , textureNo , format , DM_CompositionNode->texture_names[textureNo] );
	    }
	}
	// rectangle texture
      }
      // not VTF 
    }
    // all attachments
  }
  // 2-dimensional texture

  // 3D textures
#ifdef DM_TEXTURE3D
  // printf( "Texture RAW ID [%d]\n" , textureNo );
  else if( textureTarget == GL_TEXTURE_3D ) {
    // printf( "Load 3D texture in GPU No [%d]\n" , textureNo );
    for( int ind_attachment = 0 ; ind_attachment < nb_attachments ;
	 ind_attachment++ ) {
      glBindTexture(GL_TEXTURE_3D, GetTextureID( textureNo  + ind_attachment) );
      // printf( "Bind 3D texture ID %d attachment %d (mipmapped: %d)\n" , 
      //       GetTextureID( textureNo  + ind_attachment) , ind_attachment , isMipMapped );
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
      
      if( encoding == RAW ) {
	// DEFINES THE TEXTURE COPY FUNCTION
	if( isMipMapped ) {
	  // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,  
			  mipmapfilter );
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, 
			  mipmapfilter );
	  
	  // Build Mipmaps (builds different versions of the picture for
	  // distances - looks better)
#ifndef _VISUAL_STUDIO
#ifndef _WIN32
	  gluBuild3DMipmaps(GL_TEXTURE_3D, GL_RGBA8,
			    width , height , depth , 
			    GL_RGBA, datatype, 
			    data );
#endif
#else
#ifndef _WIN32
	  glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
#endif
#endif
	}
	else {
	  // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,  
			  texturefilter );
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, 
			  texturefilter );
	  
	}
      }
      else if( encoding == PNG || encoding == PNGA ) {
	// printf( "Load 3D PNG texture ID [%d]\n" , GetTextureID( textureNo ) );
	// printf( "Texture pointer %d\n" , data); 
	// printf( "Is mipmapped %d\n" , isMipMapped); 
	
	// DEFINES THE TEXTURE COPY FUNCTION
	if( isMipMapped ) {
	  // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,  
			  mipmapfilter );
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, 
			  mipmapfilter );
	  
	  // printf( "Loads gluBuild3DMipmaps texture No %d internal_format=%d (%s)\n" , textureNo ,components  , DM_CompositionNode->texture_names[textureNo] );
#ifndef _VISUAL_STUDIO
#ifndef _WIN32
	  gluBuild3DMipmaps(GL_TEXTURE_3D, components ,
			    width , height , depth ,
			    format, datatype, 
			    data );
#endif
#else
#ifndef _WIN32
	  glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	  glTexImage3D(GL_TEXTURE_3D, 0, components ,
		       width , height , depth , 0 ,
		       format, datatype, 
		       data );
#endif
#endif
	}
	else {
	  // printf( "Linear interpolation %d\n" , GetTextureID( textureNo ));
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,  
			  texturefilter );
	  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, 
			  texturefilter );
	  
	  // printf( "Loads glTexImage3D wxhxd %d %d %d data %d texture No %d ID:%d internal_format=%d(%d) format=%d(%d) filter %d (lin %d)(repeat %d) (%s)\n" , width , height , depth , (int)data, textureNo ,GetTextureID( textureNo  + ind_attachment),components ,GL_RGBA8 , format, GL_RGBA, texturefilter, GL_LINEAR , GL_REPEAT, DM_CompositionNode->texture_names[textureNo] );

	  glTexImage3D(GL_TEXTURE_3D, 0, components ,
		       width , height , depth , 0 ,
		       format, datatype, 
		       data );
	}
      }
    }
  }
#endif // #ifdef DM_TEXTURE3D

  else {
    sprintf( ErrorStr , "Error: unknown texture target [%d]!" , textureTarget ); ReportError( ErrorStr ); throw 422;
  }
}


// release a texture (function attached to the classe dm_Composition
// that manages the global data for a composition
void dm_Composition::FreeTextureInRAM( TextureEncoding encoding , 
				       int textureNo ,
				       int nb_attachments ) {
  // Now we need to free the image data that we loaded since OpenGL
  // stored it as a texture
  if( encoding == JPG ) {
    if (pImageJPG[ textureNo ]) {		// If we loaded the image
      if (pImageJPG[ textureNo ]->data) {	// If there is texture data
	free(pImageJPG[ textureNo ]->data);	// Free the texture data, 
	pImageJPG[ textureNo ]->data = NULL;
	// we don't need it anymore
      }
      free(pImageJPG[ textureNo ]);		// Free the image structure
    }
  }
  // Now we need to free the image data that we loaded since OpenGL
  // stored it as a texture
  else if( encoding == PNG || encoding == PNGA ) {
    if (pImagePNG[ textureNo ]) {		// If we loaded the image
      if (pImagePNG[ textureNo ]->data) {	// If there is texture data
	free(pImagePNG[ textureNo ]->data);	// Free the texture data, 
	pImagePNG[ textureNo ]->data = NULL;
	// we don't need it anymore
      }
      free(pImagePNG[ textureNo ]);		// Free the image structure
    }
  }
  // Now we need to free the image data that we loaded since OpenGL
  // stored it as a texture
  else if( encoding == RAW ) {
    for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
      if (pImageRaw[ textureNo + ind ].dataByte) {		// If we loaded the image
	free(pImageRaw[ textureNo + ind ].dataByte);	// Free the texture data, 
	pImageRaw[ textureNo + ind ].dataByte = NULL;
	// we don't need it anymore
      }
      if (pImageRaw[ textureNo + ind ].dataFloat) {		// If we loaded the image
	free(pImageRaw[ textureNo + ind ].dataFloat);	// Free the texture data, 
	pImageRaw[ textureNo + ind ].dataFloat = NULL;
	// we don't need it anymore
      }
    }
  }
  // #endif
}

///////////////////////////////////////////////////////////////
// TEXTURE CLASS
///////////////////////////////////////////////////////////////
dm_Texture::dm_Texture( void ) {
  // save the new texture into the header part of AllTexture for later removal
  strAllTexture* newTexture = new strAllTexture;
  newTexture->curTex = this;
  newTexture->nextTex = NULL;
  
  newTexture->nextTex = AllTexture->nextTex;
  AllTexture->nextTex = newTexture;
  
  init();
}

void dm_Texture::init( void ) {
  textureNo = -1;
  environmentMapTextureNo = -1;
  ptTextureNo = NULL;
  encoding = EmptyTextureEncoding;

  *variableNodeName = 0;
  *variableName = 0;

  textureTile_s = 1.0;
  textureTile_t = 1.0;
  textureTile_u = 1.0;
  textureOffset_s = 0.0;
  textureOffset_t = 0.0;
  textureOffset_u = 0.0;
  textureRepeat_s = true;
  textureRepeat_t = true;
  textureRepeat_u = true;
  textureEnvMode = DECAL;
  mipmapped = true;
  rectangle = false;
  nb_attachments = 1;
  attachment = 0;
  texture_format = byte_tex_format;
  texture_min_filter = linear_filter ,
  texture_max_filter = linear_filter ,
  textureTarget = GL_TEXTURE_2D;

  textureGenerationMode = EmptyTextureGenerationMode;

}

dm_Texture::~dm_Texture(void) {
  // causes a memory fault
}


bool dm_Texture::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				bool videoTexture ,
				FloatBinOp oper ,
				IntBinOp operI ) {
  bool modified_geometry = false;

  // texture encoding
  if( !videoTexture ) {
    if( !atRunTime 
	|| XMLtag->hasAttribute( "encoding" ) ) {
      char *encoding_str = XMLtag->getAttributeStringValue( "encoding" );
      encoding = EmptyTextureEncoding;
      for( int ind = 0 ; ind < EmptyTextureEncoding ; ind++ ) {
	//printf( "encoding [%s]\n" , TextureEncodingString[ind] );
	if( strcmp( encoding_str , TextureEncodingString[ind] ) == 0 ) {
	  encoding = (TextureEncoding)ind;
	  break;
	}
      }
      if( encoding == EmptyTextureEncoding ) {
	sprintf( ErrorStr , "Error: unknown image encoding [%s]!" , encoding_str ); ReportError( ErrorStr ); throw 10;
	encoding = EmptyTextureEncoding;
      }
    }
  }
  else {
    encoding = RAW;
  }
  
  // texture unit (s parameter)
  if( !atRunTime ) {
    textureTile_s = 1.0;
  }
  if( XMLtag->hasAttribute( "tile_s" ) ) {
    oper( textureTile_s , (float)XMLtag->getAttributeDoubleValue( "tile_s" , 
							   atRunTime ) );
    modified_geometry = true;
  }

  // texture unit (t parameter)
  if( !atRunTime ) {
    textureTile_t = 1.0;
  }
  if( XMLtag->hasAttribute( "tile_t" ) ) {
    oper( textureTile_t , (float)XMLtag->getAttributeDoubleValue( "tile_t" , 
							   atRunTime ) );
    modified_geometry = true;
  }

  // texture unit (u parameter)
  if( !atRunTime ) {
    textureTile_u = 1.0;
  }
  if( XMLtag->hasAttribute( "tile_u" ) ) {
    oper( textureTile_u , (float)XMLtag->getAttributeDoubleValue( "tile_u" , 
							   atRunTime ) );
    modified_geometry = true;
  }

  // texture offset (s parameter)
  if( !atRunTime ) {
    textureOffset_s = 0.0;
  }
  if( XMLtag->hasAttribute( "offset_s" ) ) {
    oper( textureOffset_s , (float)XMLtag->getAttributeDoubleValue( "offset_s" , 
							     atRunTime ) );
    modified_geometry = true;
  }
  
  // texture offset (t parameter)
  if( !atRunTime ) {
    textureOffset_t = 0.0;
  }
  if( XMLtag->hasAttribute( "offset_t" ) ) {
    oper( textureOffset_t , (float)XMLtag->getAttributeDoubleValue( "offset_t" , 
							     atRunTime ) );
    modified_geometry = true;
  }

  // texture offset (u parameter)
  if( !atRunTime ) {
    textureOffset_u = 0.0;
  }
  if( XMLtag->hasAttribute( "offset_u" ) ) {
    oper( textureOffset_u , (float)XMLtag->getAttributeDoubleValue( "offset_u" , 
							     atRunTime ) );
    modified_geometry = true;
  }

  // texture repetition (s parameter)
  if( !atRunTime ) {
    textureRepeat_s = true;
  }
  if( XMLtag->hasAttribute( "repeat_s" ) ) {
    XMLtag->BooleanFieldValue( "repeat_s" ,
			       &(textureRepeat_s) );
    modified_geometry = true;
  }

  // texture repetition (t parameter)
  if( !atRunTime ) {
    textureRepeat_t = true;
  }
  if( XMLtag->hasAttribute( "repeat_t" ) ) {
    XMLtag->BooleanFieldValue( "repeat_t" ,
			       &(textureRepeat_t) );
    modified_geometry = true;
  }

  // texture repetition (u parameter)
  if( !atRunTime ) {
    textureRepeat_u = true;
  }
  if( XMLtag->hasAttribute( "repeat_u" ) ) {
    XMLtag->BooleanFieldValue( "repeat_u" ,
			       &(textureRepeat_u) );
    modified_geometry = true;
  }

  // texture attachments (for FBO texture variable)
  if( !atRunTime ) {
    attachment = 0;
  }
  if( XMLtag->hasAttribute( "attachment" ) ) {
    operI( attachment , XMLtag->getAttributeIntValue( "attachment" , 
						      atRunTime ) - 1 );
  }

  // texture environnement mode
  if( XMLtag->hasAttribute( "env_mode" ) ) {
    char *mode_str = XMLtag->getAttributeStringValue( "env_mode" );
    textureEnvMode = EmptyEnvMode;
    for( int ind = 0 ; ind < EmptyEnvMode ; ind++ ) {
      if( strcmp( mode_str , EnvModeString[ind] ) == 0 ) {
	textureEnvMode = (EnvMode)ind;
	// printf( "mode [%s]\n" , EnvModeString[ind] );
	break;
      }
    }
    if( textureEnvMode == EmptyEnvMode ) {
      sprintf( ErrorStr , "Error: unknown texture environnement mode [%s]!" , mode_str ); ReportError( ErrorStr ); throw 127;
    }
  }

  // texture mipmapping
  if( !atRunTime ) {
    mipmapped = true;
  }
  if( XMLtag->hasAttribute( "mipmapped" ) ) {
    XMLtag->BooleanFieldValue( "mipmapped" ,
			       &(mipmapped) );
  }

  // rectangular textures 
  if( !atRunTime ) {
    rectangle = false;
  }
  if( XMLtag->hasAttribute( "rectangle" ) ) {
    XMLtag->BooleanFieldValue( "rectangle" ,
			       &(rectangle) );
  }

  // texture format
  if( !atRunTime ) {
    texture_format = byte_tex_format;
  }
  if( XMLtag->hasAttribute( "texture_format" ) ) {
    char *texture_format_str = XMLtag->getAttributeStringValue( "texture_format" );
    texture_format = EmptyTextureFormat;
    for( int ind = 0 ; ind < EmptyTextureFormat ; ind++ ) {
      if( strcmp( texture_format_str , TextureFormatString[ind] ) == 0 ) {
	texture_format = (TextureFormat)ind;
	break;
      }
    }
    if( texture_format == EmptyTextureFormat ) {
      sprintf( ErrorStr , "Error: unknown texture format [%s]!" , texture_format_str ); ReportError( ErrorStr ); throw 127;
    }
  }

  // texture format
  if( !atRunTime ) {
    texture_min_filter = linear_filter;
    texture_max_filter = linear_filter;
  }
  if( XMLtag->hasAttribute( "texture_min_filter" ) ) {
    char *texture_filter_str = XMLtag->getAttributeStringValue( "texture_min_filter" );
    texture_min_filter = EmptyTextureFilter;
    for( int ind = 0 ; ind < EmptyTextureFilter ; ind++ ) {
      if( strcmp( texture_filter_str , TextureFilterString[ind] ) == 0 ) {
	texture_min_filter = (TextureFilter)ind;
	break;
      }
    }
    if( texture_min_filter == EmptyTextureFilter ) {
      sprintf( ErrorStr , "Error: unknown texture filter [%s]!" , texture_filter_str ); ReportError( ErrorStr ); throw 127;
    }
  }
  if( XMLtag->hasAttribute( "texture_max_filter" ) ) {
    char *texture_filter_str = XMLtag->getAttributeStringValue( "texture_max_filter" );
    texture_max_filter = EmptyTextureFilter;
    for( int ind = 0 ; ind < EmptyTextureFilter ; ind++ ) {
      if( strcmp( texture_filter_str , TextureFilterString[ind] ) == 0 ) {
	texture_max_filter = (TextureFilter)ind;
	break;
      }
    }
    if( texture_max_filter == EmptyTextureFilter ) {
      sprintf( ErrorStr , "Error: unknown texture filter [%s]!" , texture_filter_str ); ReportError( ErrorStr ); throw 127;
    }
  }

  // texture dimension
  if( !atRunTime ) {
    textureTarget = GL_TEXTURE_2D;
    if( XMLtag->hasAttribute( "dimension" ) ) {
      int dimension = XMLtag->getAttributeIntValue( "dimension" , false );
      if( dimension == 1 ) {
	textureTarget = GL_TEXTURE_1D;
      }
      else if( dimension == 2 ) {
	textureTarget = GL_TEXTURE_2D;
      }
      else if( dimension == 3 ) {
	textureTarget = GL_TEXTURE_3D;
      }
    }
  }

  // texture generation type: bitmap, video, noise...
  if( !videoTexture ) {
    if( !atRunTime ) {
      textureGenerationMode = DM_BITMAP_TEXTURE;
    }
    if( XMLtag->hasAttribute( "type" ) ) {
      char *type_str = XMLtag->getAttributeStringValue( "type" );
      textureGenerationMode = EmptyTextureGenerationMode;
      for( int ind = 0 ; ind < EmptyTextureGenerationMode ; ind++ ) {
	if( strcmp( type_str , TextureGenerationModeString[ind] ) == 0 ) {
	  textureGenerationMode = (TextureGenerationMode)ind;
	  // printf( "textureGenerationMode [%s]\n" ,TextureGenerationModeString[ind] );
	  break;
	}
      }
      if( textureGenerationMode == EmptyTextureGenerationMode ) {
	sprintf( ErrorStr , "Error: unknown texture type [%s]!" , type_str ); ReportError( ErrorStr ); throw 127;
      }
    }
  }
  else {
    textureGenerationMode = DM_BITMAP_TEXTURE;
  }

  return modified_geometry;
}

bool dm_Texture::getParameters( char attribute[XMLAttrChLength] , 
				DataType * dataType ,
				double * valDouble ,
				int * valInt ,
				bool * valBool ,
				char ** valString ,
				int * tableOrMatrixSize ,
				dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "tile_s" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureTile_s;
    return true;
  }
  if( strcmp( attribute , "tile_t" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureTile_t;
    return true;
  }
  if( strcmp( attribute , "tile_u" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureTile_u;
    return true;
  }
  if( strcmp( attribute , "offset_s" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureOffset_s;
    return true;
  }
  if( strcmp( attribute , "offset_t" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureOffset_t;
    return true;
  }
  if( strcmp( attribute , "offset_u" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)textureOffset_u;
    return true;
  }
  if( strcmp( attribute , "atachment" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = attachment;
    return true;
  }
  // Read only variable, set by texture variable nb_attachments 
  // attribute for FBO multitexture binding
  if( strcmp( attribute , "nb_atachments" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_attachments;
    return true;
  }
  if( strcmp( attribute , "repeat_s" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = textureRepeat_s;
    return true;
  }
  if( strcmp( attribute , "repeat_t" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = textureRepeat_t;
    return true;
  }
  if( strcmp( attribute , "repeat_u" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = textureRepeat_u;
    return true;
  }
  if( strcmp( attribute , "mipmapped" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = mipmapped;
    return true;
  }
  if( strcmp( attribute , "rectangle" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = rectangle;
    return true;
  }
  return false;
}

int dm_Texture::GetTextureNo( void ) {
  if( textureGenerationMode == DM_VARIABLE_TEXTURE ) {
    if( ptTextureNo ) {
      return *ptTextureNo;
    }
    else {
      return -1;
    }
  }
  else {
    return textureNo;
  }
}

void dm_Texture::SetTextureNo( int texNo ) {
  textureNo = texNo;
}

////////////////////////////////////////////////////////////
////////// printing material properties
////////////////////////////////////////////////////////////
void dm_Texture::ResolveReferences( dm_SceneNode *rootNode ) {
  if( textureGenerationMode == DM_VARIABLE_TEXTURE ) {
    // texture name  
    int nbRefs = 0;
    dm_SceneNode **auxNode;
    auxNode = findAllReferences( rootNode , 
				 variableNodeName , &nbRefs , true , true );
    dm_SceneNode *referenceNode = NULL;
    if( auxNode ) {
      // allocates a list of target nodes that
      // will be instantiated by the actual pointers in
      // the scene graph
      referenceNode = auxNode[ 0 ];
      // printf( "ReferenceNodeAttributeValue %s\n" , referenceNode->id );
      
      // throws away the nodes that were allocated during
      // the resolution of the reference
      delete [] auxNode;
      auxNode = NULL;
    }
    else {
      sprintf( ErrorStr , "Error: unknown node [%s] on variable texture!" , variableNodeName ); ReportError( ErrorStr ); throw 206;
    }
    
    // points to the material properties carried by the scalar variable
    // searches among the variables
    // scalar variables
    dm_Scalar *scalar = referenceNode->getScalarFromName( variableName );
    // printf( "scalar %d\n" , scalar );
    // scalars are expected to be given before textures they
    // represent in the XML file (could be improved by a
    // 2-pass approach to be order insensitive)
    if( scalar ) {
      ptTextureNo = &(scalar->GetValue()->materialTexture->textureNo);
      texture_format = scalar->GetValue()->materialTexture->texture_format;
      nb_attachments = scalar->GetValue()->materialTexture->nb_attachments;
      texture_max_filter = scalar->GetValue()->materialTexture->texture_max_filter;
      texture_min_filter = scalar->GetValue()->materialTexture->texture_min_filter;
      // printf( "textureNo %d texture format %s textureName %s\n" , *(ptTextureNo) , TextureFormatString[texture_format] , variableName );
    }
    else {
      sprintf( ErrorStr , "Error: unknown variable texture [%s] on node [%s] !" , variableName , variableNodeName ); ReportError( ErrorStr ); throw 206;
    }
  }
}

////////////////////////////////////////////////////////////
////////// finds the pointer to the texture if it exists
////////// and allocates the bitmap
////////////////////////////////////////////////////////////
void dm_Texture::LoadOrGenerateTexture( char *textureName ,
					int *fileDepth , int *p_c ,
					char *ID , dm_XMLTag *XMLtag ,
					bool invert ) {
  // texture no is given by the texture allocator or by 
  // referencing a previously loaded texture
  if( textureGenerationMode == DM_BITMAP_TEXTURE 
      && textureName && *(textureName) ) {
    bool newText;
    textureNo 
      = DM_CompositionNode->FindTexture( textureName , 
					 &encoding ,
					 textureDimension( textureTarget ) , 
					 rectangle ,
					 DM_DIRECT_TEXTURING , 
					 &newText ,
					 invert );
    // printf( "textureNo %d textureName %s\n" , textureNo , textureName );
  }
  else if( textureGenerationMode == DM_FRAME_BUFFER_CLEARED
	   || textureGenerationMode == DM_FRAME_BUFFER_UNCLEARED ) {
    // printf("frame buffer\n");
    encoding = RAW; 
    textureNo 
      = DM_CompositionNode->GenerateTexture( rectangle ,
					     texture_format ,
					     1 );
  }
  else {
    sprintf( ErrorStr , "Error: texture has no name in component ID [%s]!" , ID ); ReportError( ErrorStr ); throw 422;
  }
}

////////////////////////////////////////////////////////////
////////// printing material properties
////////////////////////////////////////////////////////////

int dm_Texture::textureDimension( GLenum textureTarget ) {
  if( textureTarget == GL_TEXTURE_1D ) {
    return 1;
  }
  else if( textureTarget == GL_TEXTURE_2D ) {
    return 2;
  }
  else if( textureTarget == GL_TEXTURE_3D ) {
    return 3;
  }
  return 2;
}

void dm_Texture::print( FILE *file , int depth ) {
  indent( file , depth );
  char * name;
  name = DM_CompositionNode->texture_names[GetTextureNo()];

  fprintf( file , "<texture type=\"%s\" encoding=\"%s\" xlink:href=\"%s\" tile_s=\"%.2f\"  tile_t=\"%.2f\" tile_u=\"%.2f\" offset_s=\"%.2f\"  offset_t=\"%.2f\" offset_u=\"%.2f\" nb_attachments=\"%d\" attachment=\"%d\" repeat_s=\"%s\"  repeat_t=\"%s\" repeat_u=\"%s\" env_mode=\"%s\" dimension=\"%d\" mipmapped=\"%s\" rectangle=\"%s\" texture_format=\"%s\" texture_min_filter=\"%s\" texture_max_filter=\"%s\"\n" , 
	   TextureGenerationModeString[ textureGenerationMode ] ,
	   TextureEncodingString[encoding] , 
	   name , 
	   textureTile_s , textureTile_t , textureTile_u , 
	   textureOffset_s , textureOffset_t , textureOffset_u ,
	   nb_attachments , attachment + 1 , 
	   BooleanString[BooleanToInt(textureRepeat_s)] , BooleanString[BooleanToInt(textureRepeat_t)] , BooleanString[BooleanToInt(textureRepeat_u)] , 
	   EnvModeString[textureEnvMode] ,
	   textureDimension( textureTarget ) ,
	   BooleanString[BooleanToInt(mipmapped)] ,
	   BooleanString[BooleanToInt(rectangle)] ,
	   TextureFormatString[texture_format] ,
	   TextureFilterString[texture_min_filter] ,
	   TextureFilterString[texture_max_filter] 
	   );
  fprintf( file , " />\n" );

}

void dm_Texture::bindFrameBufferTexture( bool *invalidDL ,
					 bool customSize , int x0 , int y0 , 
					 int width , int height ) {
  int sizeX ;
  int sizeY ;
  if( !rectangle ) {
    sizeX 
      = smallestPOT( DM_EnvironmentNode->DM_Window->getWindowWidth() );
    sizeY 
      = smallestPOT( DM_EnvironmentNode->DM_Window->getWindowHeight() );
  }
  else {
    sizeX 
      = DM_EnvironmentNode->DM_Window->getWindowWidth();
    sizeY 
      = DM_EnvironmentNode->DM_Window->getWindowHeight();
  }
  
  if( customSize && (x0 + width > sizeX || y0 + height > sizeY) ) {
    sprintf( ErrorStr , "Error: texture binding size exceeds screen size X=(%d,%d) Y=(%d,%d)!" , x0 + width , sizeX , y0 + height , sizeY ); ReportError( ErrorStr ); throw 420;
  }
  
  // frame buffer to texture and window resize: texture buffer
  // must be reallocated
  if( ( textureTarget == GL_TEXTURE_2D
	&& (textureGenerationMode == DM_FRAME_BUFFER_CLEARED
	    || textureGenerationMode == DM_FRAME_BUFFER_UNCLEARED ) )
      && (DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX != sizeX
	  || DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY != sizeY) ) {
    delete [] DM_CompositionNode->pImageRaw[ GetTextureNo() ].dataByte;
    DM_CompositionNode->pImageRaw[ GetTextureNo() ].dataByte = NULL;
    
    //   GLuint texID = 0;
    DM_CompositionNode->GenerateTextureInRAM( GetTextureNo()  , 
					      rectangle ,
					      texture_format ,
					      nb_attachments );
  }
  
  if( textureTarget == GL_TEXTURE_1D ) {
    glBindTexture(GL_TEXTURE_1D, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
  }
  else if( textureTarget == GL_TEXTURE_2D ) {
    if( !rectangle ) {
      glBindTexture(GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
    }
    else {
      glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
    }
  }
  else if( textureTarget == GL_TEXTURE_3D ) {
    glBindTexture(GL_TEXTURE_3D, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
    // printf("bind texture 3D\n");
  }
  
  // printf( "textureGenerationMode %s No %d ID %d (%s)\n" , TextureGenerationModeString[textureGenerationMode] , GetTextureNo(),DM_CompositionNode->GetTextureID( GetTextureNo() ),DM_CompositionNode->texture_names[GetTextureNo()]);
  
  // frame buffer to texture
  if( textureTarget == GL_TEXTURE_2D
      && (textureGenerationMode == DM_FRAME_BUFFER_CLEARED
	  || textureGenerationMode == DM_FRAME_BUFFER_UNCLEARED ) ) {
    if( !rectangle ) {
      // printf( "glCopyTexImage2D\n" );
      glBindTexture( GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
      // printf( "FRAME_BUFFER glBindTexture No [%d] ID [%d] attachment %d\n" , GetTextureNo() , DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment - 1), attachment + 1);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      
      //       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      //       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glReadBuffer(GL_BACK);
      
      //printf( "glCopyTexImage2D dim %d texture[%d/%d] <- [%d,%d] tex No %d \n" , dimension , DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY , CurrentWindow->getWindowWidth() , CurrentWindow->getWindowHeight(),DM_CompositionNode->GetTextureID( GetTextureNo() ));
      if( texture_format == byte_tex_format ) {
	if( customSize ) {
	    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
			      x0 , y0 , width , height , 0 );
	}
	else {
	  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
			    0 , 0 , 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY, 0 );
	  }
      }
      else {
	if( customSize ) {
	  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI,
			    x0 , y0 , width , height , 0 );
	}
	else {
	  glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI,
			    0 , 0 , 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY, 0 );
	}
      }
    }	
    else {
      // printf( "glCopyTexImage2D\n" );
      glBindTexture( GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment));
      // printf( "FRAME_BUFFER glBindTexture No [%d] ID [%d] attachment %d\n" , GetTextureNo() , DM_CompositionNode->GetTextureID( GetTextureNo()  + attachment) , attachment + 1);
      
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      
      //       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      //       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glReadBuffer(GL_BACK);
      
      //printf( "glCopyTexImage2D dim %d texture[%d/%d] <- [%d,%d] tex No %d \n" , dimension , DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY , CurrentWindow->getWindowWidth() , CurrentWindow->getWindowHeight(),DM_CompositionNode->GetTextureID( GetTextureNo() ));
      if( texture_format == byte_tex_format ) {
	if( customSize ) {
	  glCopyTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA,
			    x0 , y0 , width , height , 0 );
	}
	else {
	  glCopyTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA,
			    0 , 0 , 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY, 0 );
	}
      }	
      else {
	if( customSize ) {
	  glCopyTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA_FLOAT32_ATI,
			    x0 , y0 , width , height , 0 );
	}
	else {
	  glCopyTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA_FLOAT32_ATI,
			    0 , 0 , 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeX, 
			    DM_CompositionNode->pImageRaw[ GetTextureNo() ].sizeY, 0 );
	}
      }
    }
    
    if( textureGenerationMode == DM_FRAME_BUFFER_CLEARED ) {
      // printf( "glScissor [%d/%d] - [%d,%d] \n" , CurrentWindow->getWindow_x() , CurrentWindow->getWindow_y() , CurrentWindow->getWindowWidth() , CurrentWindow->getWindowHeight());
      if( customSize ) {
	glScissor( CurrentWindow->getWindow_x() + x0 ,
		   CurrentWindow->getWindow_y() + y0 , 
		   width , height );
      }
      else {
	glScissor( CurrentWindow->getWindow_x() ,
		   CurrentWindow->getWindow_y() , 
		   CurrentWindow->getWindowWidth() , 
		   CurrentWindow->getWindowHeight() );
      }
      
      glEnable( GL_SCISSOR_TEST );
      
      // buffer reset
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      // background color
      glClearColor (OpenGLBGColor[0], OpenGLBGColor[1], OpenGLBGColor[2], OpenGLBGColor[3]);
      
      glDisable( GL_SCISSOR_TEST );
    }
    // glBindTexture(GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( GetTextureNo() ));
    
    *invalidDL = true;
  }
}

void dm_Texture::textureMode( bool *invalidDL ) {
  // textures are loaded dynamically
  // when a new texture is loaded, the
  // display list must be recompiled a
  // second time so that the texture is not
  // repeatedly loaded in GPU at each 
  // display list call
  if( DM_CompositionNode->GetTextureID( GetTextureNo() ) == NULL_ID ) {
    // printf( "=>  on demand load texture (in texture mode) (%s) attachment %d , nb_attachments %d No [%d] ID [%d]\n" , DM_CompositionNode->texture_names[GetTextureNo() + attachment] , attachment + 1 , nb_attachments , GetTextureNo() + attachment , DM_CompositionNode->GetTextureID( GetTextureNo() ) );

    DM_CompositionNode->LoadTextureInGPUMemory( GetTextureNo(), 
  						attachment ,
  						nb_attachments ,
  						encoding , 
  						textureGenerationMode ,
  						mipmapped , 
  						rectangle , 
  						texture_format ,
  						texture_min_filter ,
  						texture_max_filter ,
  						textureTarget );
    // printf( "=> load texture (%s) No [%d] ID [%d]\n" , DM_CompositionNode->texture_names[GetTextureNo()] , GetTextureNo() , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    // texture memory could be free 
    // after all the windows have been displayed
    // 	  DM_CompositionNode->FreeTextureInRAM( encoding , GetTextureNo() );
    *invalidDL = true;
  }

  // printf( "Texture mode for texture [%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
  // bindTexture( invalidDL , false , 0 , 0 , 0 , 0 );  
  
  // texture painting
  if( textureEnvMode == MODULATE ) {
    //printf( "MODULATE texture[%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    glDisable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
  }
  else if( textureEnvMode == DECAL ) {
    //printf( "DECAL texture[%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    glDisable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_COLOR, GL_ZERO); 
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); 
  }
  else if( textureEnvMode == REPLACE ) {
    // printf( "REPLACE texture[%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    glDisable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_COLOR, GL_ZERO); 
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); 
  }
  else if( textureEnvMode == BLEND ) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
  }
  else if( textureEnvMode == BLEND_FRAGMENT ) {
    // printf( "BLEND_FRAGMENT texture[%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white_transparent );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); 
  }
  else if( textureEnvMode == BLEND_CUMULATIVE ) {
    // printf( "BLEND_CUMULATIVE texture[%d]\n" , DM_CompositionNode->GetTextureID( GetTextureNo() ) );
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); 
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, black_transparent );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE); 
  }

  // texture mapping
  if( textureRepeat_s ) {
    switch( textureTarget ) {
    case GL_TEXTURE_1D:
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      break;
    case GL_TEXTURE_2D:
      if( !rectangle ) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      }
      else {
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
      }
      break;
    case GL_TEXTURE_3D:
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      break;
    }
  }
  else {
    switch( textureTarget ) {
    case GL_TEXTURE_1D:
      glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      break;
    case GL_TEXTURE_2D:
      if( !rectangle ) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      }
      else {
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
      }
      break;
    case GL_TEXTURE_3D:
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      break;
    }
  }

  // texture mapping
  if( textureTarget == GL_TEXTURE_2D || textureTarget == GL_TEXTURE_3D ) {
    if( textureRepeat_t ) {
      switch( textureTarget) {
      case GL_TEXTURE_2D:
	if( !rectangle ) {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	break;
      case GL_TEXTURE_3D:
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	break;
      }
    }
    else {
      switch( textureTarget) {
      case GL_TEXTURE_2D:
	if( !rectangle ) {
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	break;
      case GL_TEXTURE_3D:
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	break;
      }
    }
  }

  // texture mapping
  if( textureTarget == GL_TEXTURE_3D ) {
    if( textureRepeat_u ) {
      // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_U, GL_REPEAT);
    }
    else {
      // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_U, GL_CLAMP);
    }
  }
}

void dm_Texture::close( void ) {
  glDisable( GL_TEXTURE_1D );
  if( !rectangle ) {
    glDisable( GL_TEXTURE_2D );
  }
  else {
    if( texture_format == float_tex_format ) {
      glBindTexture( GL_TEXTURE_RECTANGLE, 0);
    }
    glDisable( GL_TEXTURE_RECTANGLE );
  }
  glDisable( GL_TEXTURE_3D );
  // glDisable(GL_TEXTURE_RECTANGLE_NV);
  
  glDisable( GL_COLOR_MATERIAL );
  
  glDisable(GL_BLEND);
  
  glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, 
	      black_transparent );
  
  glColor4fv( white_opaque );
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
}

void dm_Texture::operator=(dm_Texture& p) {
  // because it corresponds to a pointer in a table
  // otherwise all the copied objects must have the
  // same texture no
  textureGenerationMode = p.textureGenerationMode;
  textureNo = p.textureNo;
  environmentMapTextureNo = p.environmentMapTextureNo;
  ptTextureNo = p.ptTextureNo;

  strcpy( variableNodeName , p.variableNodeName );
  strcpy( variableName , p.variableName );

  textureTile_s = p.textureTile_s;
  textureTile_t = p.textureTile_t;
  textureTile_u = p.textureTile_u;
  textureOffset_s = p.textureOffset_s;
  textureOffset_t = p.textureOffset_t;
  textureOffset_u = p.textureOffset_u;
  textureRepeat_s = p.textureRepeat_s;
  textureRepeat_t = p.textureRepeat_t;
  textureRepeat_u = p.textureRepeat_u;
  textureEnvMode = p.textureEnvMode;
  nb_attachments = p.nb_attachments;
  attachment = p.attachment;

  encoding = p.encoding;
  mipmapped = p.mipmapped;
  rectangle = p.rectangle;
  texture_format = p.texture_format;
  texture_min_filter = p.texture_min_filter;
  texture_max_filter = p.texture_max_filter;
  textureTarget = p.textureTarget;
}

