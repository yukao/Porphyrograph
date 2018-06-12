/*! \file pg-all-include.h
 * 
 * 
 *     File pg-all_include.h
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

#ifndef _VISUAL_STUDIO
#include "config.h"
#include <dirent.h>
#endif // !_VISUAL_STUDIO

/// UNIX included files 
#ifdef UNIX

	/// included constants for application profile
	/// included constants for graphic extensions
	/// could become profiles in the future
	#define PG_STENCILOP

	/// included X files
	#include <X11/keysym.h>
#endif // UNIX

#define PG_TEXTURE3D

/*! \name Group standard_includes
 *  \brief standard included files
 */
// \{

#include <stdio.h>
#include <iostream>
#include <iomanip>

#define _USE_MATH_DEFINES
#include <math.h>    // math constants such as M_PI
// define the round function for Visual Studio (not include in math.h)
#ifdef _VISUAL_STUDIO
	#define round(x) (x >= 0 ? floor(x + 0.5) : ceil(x - 0.5))
	//double round(double x) { return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5); }
#endif // _VISUAL_STUDIO
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string.h>
#include <cstdio>
#include <cstring>
#include <dirent.h>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <random>
#include <algorithm>    // std::max / min

#ifndef INT_MAX
        #include <limits.h>
#endif

#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
        #include <sys/resource.h>
	#include <netdb.h>
	#include <unistd.h>
#else // !_WIN32
	//#define socklen_t int
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	//#include <wspiapi.h>
#endif // _WIN32
// \}

/*! \name Group time_management
 *  \brief time management
*/
//\ {
#ifdef _WIN32
	#include <time.h>
	#include <Winbase.h>
	#include <Windows.h>

	// transparent windows
	#include <windowsx.h>
	// transparent windows
	#include <dwmapi.h>
	
	#ifndef _VISUAL_STUDIO
		#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
			#define _TIMEVAL_DEFINED
			struct timeval {
			  unsigned long int tv_usec;
			  unsigned long int tv_sec;
			};
		#endif /* !_TIMEVAL_DEFINED */
	#endif /* !_VISUAL_STUDIO */
#else // _WIN32
	#include <sys/time.h>
	#include <pthread.h>
#endif // _WIN32
// \}

////////////////////////////////////////////
// current directory 
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define OPENCV_3

#ifndef OPENCV_3
// opencv include files
#include <opencv/cv.h>
#include <opencv/highgui.h>
#else
// opencv include files
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#endif
using namespace cv;

#include <list>
#include <vector>
#include <map>
#include <unordered_map>
using std::list;
using std::vector;
using std::map;
using std::string;
using std::ios;
using std::cerr;
using std::ifstream;
#include <fstream>
#include <GL/glew.h>
#include <GL/wglew.h>

#define  GL_GLEXT_LEGACY

#ifdef _WIN32
#ifndef PG_WACOM_TABLET
#define PG_WACOM_TABLET
#endif
#endif // _VISUAL_STUDIO

#if defined (TVW) || defined (CRITON)
// NB TRACKS -> ***** ALSO TO BE CHANGED IN DRAWING AND COMPOSITION FRAGMENT SHADERS ****
#define PG_NB_TRACKS 2   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#elif defined (GN) || defined (MALAUSSENA)
// NB TRACKS -> ***** ALSO TO BE CHANGED IN DRAWING AND COMPOSITION FRAGMENT SHADERS ****
#define PG_NB_TRACKS 1   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#else
// NB TRACKS -> ***** ALSO TO BE CHANGED IN DRAWING AND COMPOSITION FRAGMENT SHADERS ****
#define PG_NB_TRACKS 3   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#endif

#if defined (TVW) || defined (GN) || defined (CRITON)
// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 3 WITH THE CURRENT VARIABLES) 
#define PG_NB_PATHS 3   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#else
// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 3 WITH THE CURRENT VARIABLES) 
#define PG_NB_PATHS 7   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#endif

#define PG_NB_PARTICLE_INITIAL_IMAGES 6 // number of images used for particle initialization

// REST DELAY FOR CAMERA AFTER CHANGE OF PARAMETERS
#define PG_CAMERA_REST_DELAY 240

// NB NOISE TYPES
#define PG_NB_NOISE_TYPES 4
// regular Perlin noise
// sun ray Perlin noise
// camera noise
// video noise

// NB CAMERA IMAGE CUMUL MODES
#ifndef MALAUSSENA
#define PG_WITH_CAMERA_CAPTURE
#endif

#define PG_NB_VIDEO_CUMUL_MODES 4

// no cumul
// add
// stamp
// xor

// NB PIXELS MODES
#define PG_NB_PIXEL_MODES 3
// no pixels
// pixels
// firework

// NB PARTICLE MODES
#define PG_NB_PARTMOVE_MODES 5

#define PG_NB_PARTREPOP_MODES 2
#define PARTREPOP_GRID 1

#define PG_NB_PARTEXIT_MODES 3
#define PG_NB_PARTSTROKE_MODES 4
#define PG_NB_PARTCOLOR_MODES 3

// NB CA TYPES
#if defined(GN)
#define PG_NB_CA_TYPES 8
//#define CA_SQUENCR                0
//#define CA_TOTALISTIC             1
//#define CA_GENERATION             2
//#define CA_GAL_BIN_MOORE          3
//#define CA_GAL_BIN_NEUMANN        4
//#define CA_NEUMANN_BINARY         5
#endif
#if defined(MALAUSSENA)
#define PG_NB_CA_TYPES 7
//#define CA_TOTALISTIC             0
//#define CA_GENERATION             1
//#define CA_GAL_BIN_MOORE			2
//#define CA_GAL_BIN_NEUMANN		3
//#define CA_NEUMANN_BINARY         4
#endif
#if defined(TVW) || defined (CRITON)
#define PG_NB_CA_TYPES 5
//#define CA_PREY                    0
//#define CA_DISLOCATION             1
//#define CA_PROTOCELLS              2
//#define CA_SOCIAL_PD               3
//#define CA_PATHS                   4
//#define CA_LETSGO                  5
#endif
#if !defined(GN) && !defined(MALAUSSENA) && !defined(TVW) && !defined(CRITON)
#define PG_NB_CA_TYPES 4
//const uint CA_CYCLIC = 0;
//const uint CA_CYCLIC_1 = 1;
//const uint GOL_1 = 2;
//const uint CA_PROTOCELLS = 3;
#endif

// NB CA SUB-TYPES
#define PG_NB_CA_SUBTYPES 20

// CURVE VS SPLAT PARTICLES
// #define BLURRED_SPLAT_PARTICLES
#if defined(TVW) || defined (CRITON)
#define BLURRED_SPLAT_PARTICLES
#elif defined DASEIN
#define CURVE_PARTICLES
#else
#define LINE_SPLAT_PARTICLES
#endif

#ifdef CURVE_PARTICLES
#define PG_PARTICLE_CURVE_DEGREE 3
#endif

// SENSORS
#if defined (MALAUSSENA)
// #define PG_SENSORS
// #define PG_NB_SENSORS 1
// #define PG_NB_MAX_SENSOR_ACTIVATIONS 2
#define BEAT_DURATION (0.1f)
#define PG_PUREDATA_SOUND
#endif
#if defined (DEMO) || defined (GN)
#define PG_SENSORS
#define PG_NB_SENSORS 16
#define PG_NB_MAX_SENSOR_ACTIVATIONS 6
#define BEAT_DURATION (1.0f)
#define PG_RENOISE
#endif
#ifdef PG_SENSORS
#define PG_SENSOR_TEXTURE_WIDTH 100
#define PG_SENSOR_GEOMETRY_WIDTH 10
#define PG_NB_MAX_SENSOR_LAYOUTS 4
#define PG_NB_MAX_SAMPLE_SETUPS 3
#endif
#ifdef SUPERCOLLIDER
#define PG_NB_SENSOR_GROUPS 12
#endif

#ifdef _WIN32
	#ifdef _VISUAL_STUDIO
	//#ifdef PG_WACOM_TABLET
		#include "../share/freeglut-wacom/include/GL/freeglut.h"
	//#else // PG_WACOM_TABLET
	//       #include "../share/freeglut-3.0.0/include/GL/freeglut.h"
	//#endif // _VISUAL_STUDIO
	#else // _VISUAL_STUDIO
		#include <glut.h>
	#endif // _VISUAL_STUDIO
#else // LINUX

	#include <GL/glu.h> 

        #define   GLX_GLXEXT_LEGACY
	#include <GL/glx.h>

	#ifdef __APPLE_CC__
		#include <GLUT/glut.h>
	#else // __APPLE_CC__
		#include <GL/freeglut.h>
		#include <GL/freeglut_ext.h>
	#endif // __APPLE_CC__

#endif // _WIN32

#ifdef GN
#include "pg_script_header_GN.h"
#endif
#if defined(TVW)
#include "pg_script_header_TVW.h"
#endif
#if defined (CRITON)
#include "pg_script_header_Criton.h"
#endif
#ifdef effe
#include "pg_script_header_effe.h"
#endif
#ifdef DEMO
#include "pg_script_header_demo.h"
#endif
#ifdef MALAUSSENA
#include "pg_script_header_Malaussena.h"
#endif
#ifdef DASEIN
#include "pg_script_header_dasein.h"
#endif

#include "pg-udp.h"
#include "pg-script.h"
#include "pg-update.h"
#include "pg-texture.h"

#include "pg-shader.h"
#include "pg-stroke.h"
#include "pg-init.h"

#include "pg-main.h"
#include "pg-conf.h"
// end of standard included files
///////////////////////////////////////////////////////
