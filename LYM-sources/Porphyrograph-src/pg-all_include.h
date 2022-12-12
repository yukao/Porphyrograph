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

#ifndef _WIN32
#include "config.h"
#include <dirent.h>
#endif // !_WIN32

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
#include <filesystem>
namespace fs = std::filesystem;

#define _USE_MATH_DEFINES
#include <math.h>    // math constants such as M_PI
// define the round function for Visual Studio (not include in math.h)
#ifdef _WIN32
	//#define round(x) (x >= 0 ? floor(x + 0.5) : ceil(x - 0.5))
	//double round(double x) { return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5); }
#endif // _WIN32
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string.h>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <regex>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <conio.h>
#include <random>
#include <algorithm>    // std::max / min

#include <stdarg.h>

#define rand_0_1 (float(rand()) / float(RAND_MAX))

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
	
	#ifndef _WIN32
		#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
			#define _TIMEVAL_DEFINED
			struct timeval {
			  unsigned long int tv_usec;
			  unsigned long int tv_sec;
			};
		#endif /* !_TIMEVAL_DEFINED */
	#endif /* !_WIN32 */
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
#include <sys/stat.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/norm.hpp>

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
#include <opencv2/calib3d.hpp>
#endif
using namespace cv;

// liblo OSC library
#include "lo/lo.h"

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
#ifdef _WIN32
#include <GL/wglew.h>
#endif

#define  GL_GLEXT_LEGACY

#ifdef _WIN32
#ifndef PG_WACOM_TABLET
// #define PG_WACOM_TABLET
#endif
#endif // _WIN32

#if defined(ALKEMI)
#define PG_FBO_PINGPONG_SIZE 2
#define PG_SLOW_TRACK_TRANSLATION 1
#else
#define PG_FBO_PINGPONG_SIZE 2
#endif

#if defined (TVW) || defined (KOMPARTSD) || defined(LIGHT) // || defined (REUTLINGEN)
#define PG_NB_TRACKS 2   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#elif defined (GN) || defined (CAAUDIO) || defined (CRITON)
#define PG_NB_TRACKS 1   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#elif defined (BONNOTTE) || defined(SONG) || defined(CORE)
#define PG_NB_TRACKS 4   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#else
// NB TRACKS -> ***** ALSO TO BE CHANGED IN DRAWING AND COMPOSITION FRAGMENT SHADERS ****
#define PG_NB_TRACKS 3   // **** ALSO TO BE CHANGED IN UPDATE, MASTER AND COMPOSITION FRAGMENT SHADER ****
#endif

#if !defined (TVW) && !defined (REUTLINGEN) && !defined (BICHES)
#define PG_WITH_PHOTO_DIAPORAMA
#endif

#if defined (VOLUSPA)
#define PG_WITH_BLUR
#endif

#if defined (ARAKNIT) || defined(CAVERNEPLATON) || defined(SONG) || defined(FORET) || defined(LIGHT) || defined(PIERRES) || defined(CORE)
#if defined (ARAKNIT) || defined(CAVERNEPLATON)
// light variables inside scenario
#define PG_LIGHTS_CONTROL_IN_PG
#else
// curve based control in Python
#define PG_LIGHTS_CONTROL_IN_PYTHON
#endif
#if defined (ARAKNIT) || defined(CAVERNEPLATON) || defined(SONG) || defined(FORET) || defined(LIGHT) || defined(PIERRES) || defined(CORE)
// light DMX command in porphyrograpa
#define PG_LIGHTS_DMX_IN_PG
#include <controller_library.h>
#include "pg-light.h"
#else
// light DMX command in Python
#define PG_LIGHTS_DMX_IN_PYTHON
#endif
#endif


#if defined(SOMETHING)
#define PG_MIDI
#include "portmidi.h"
#include "pg-MIDI.h"
#endif


#if defined (TVW) || defined (GN) || defined (CRITON) || defined(LIGHT)
// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 3 WITH THE CURRENT VARIABLES) 
#define PG_NB_PATHS 3   // **** ALSO TO BE CHANGED IN UPDATE FRAGMENT SHADER ****
#elif defined (CAVERNEPLATON) || defined (PIERRES) || defined (ENSO) || defined (ATELIERSENFANTS) || defined (SONG) \
	 || defined(ALKEMI) || defined(FORET) || defined (SOUNDINITIATIVE) || defined(CORE)
// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 11 WITH THE CURRENT VARIABLES) 
#define PG_NB_PATHS 11   // **** ALSO TO BE CHANGED IN UPDATE AND PARTICLE ANIMATION FRAGMENT SHADER ****
#else
// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 7 WITH THE CURRENT VARIABLES) 
#define PG_NB_PATHS 7   // **** ALSO TO BE CHANGED IN UPDATE FRAGMENT SHADER ****
#endif

// for a tactile entry, there is no mouseover and the path has to be broken if the distance between two consecutive points is too big
#if defined (ATELIERSENFANTS)
#define PG_TACTILE_TABLET
#endif


// REST DELAY FOR CAMERA AFTER CHANGE OF PARAMETERS
#define PG_CAMERA_REST_DELAY 240

// NB OF POSSIBLE CURSORS THROUGH HAND INTERFACE
#if defined (ATELIERSENFANTS)
#define PG_NB_CURSORS_MAX 5
#else
#define PG_NB_CURSORS_MAX 1
#endif

#if !defined(LIGHT)
#define PG_WITH_SOUND_LEVELS_GUI_FEEDBACK
#endif

// HSV color system instead of Palettes + grey
#if defined (ATELIERSENFANTS)
#define PEN_HSV
#define PART_HSV
#else
#define PG_NB_CURSORS_MAX 1
#endif

// NB NOISE TYPES
#define PG_NB_NOISE_TYPES 4
// regular Perlin noise
// sun ray Perlin noise
// camera noise
// video noise

// WITHOUT USE OF WEBCAM
#if !defined (CAAUDIO) && !defined (TEMPETE) && !defined (DAWN) && !defined (RIVETS) && !defined (KOMPARTSD) \
	&& !defined (BICHES) && !defined (ARAKNIT)  && !defined (PIERRES) && !defined (FORET) && !defined(LIGHT) && !defined(ALKEMI)
#define PG_WITH_CAMERA_CAPTURE
#endif

#ifndef CRITON
#define PG_WITH_PUREDATA
#else
#define PG_WITH_JUCE
#endif

#define PG_NB_CAMERA_CUMUL_MODES 4
// no cumul
// add
// stamp
// xor

// NB PIXELS MODES
#if defined(KOMPARTSD) || defined(LIGHT)
#undef PG_NB_PIXEL_MODES
#else
#define PG_NB_PIXEL_MODES 3
#if !defined(ALKEMI) && !defined(PIERRES) && !defined(ENSO) && !defined(FORET) && !defined(GN)
#define PG_WITH_CLIP_ART
#endif
// no pixels
// pixels
// firework
#endif

#if defined(SOUNDINITIATIVE)
//#define PG_MIRRORED_SECOND_SCREEN
#endif

// NB CA TYPES
#if defined(GN) || defined(ALKEMI)
#define PG_NB_CA_TYPES 8
//#define CA_SQUENCR                0
//#define CA_TOTALISTIC             1
//#define CA_GENERATION             2
//#define CA_GAL_BIN_MOORE          3
//#define CA_GAL_BIN_NEUMANN        4
//#define CA_NEUMANN_BINARY         5
#elif defined(CAAUDIO) || defined(RIVETS)
#define PG_NB_CA_TYPES 7
//#define CA_TOTALISTIC             0
//#define CA_GENERATION             1
//#define CA_GAL_BIN_MOORE			2
//#define CA_GAL_BIN_NEUMANN		3
//#define CA_NEUMANN_BINARY         4
#elif defined(TVW)
#define PG_NB_CA_TYPES 5
//#define CA_PREY                    0
//#define CA_DISLOCATION             1
//#define CA_PROTOCELLS              2
//#define CA_SOCIAL_PD               3
//#define CA_PATHS                   4
//#define CA_LETSGO                  5
#elif defined(CRITON)
#define PG_NB_CA_TYPES 1
//#define CA_PROTOCELLS                    0
#elif defined(KOMPARTSD) || defined(LIGHT)
#undef PG_NB_CA_TYPES
#elif defined(CAVERNEPLATON) || defined(PIERRES) || defined (ENSO) || defined(SONG) || defined(FORET)|| defined(SOUNDINITIATIVE) || defined(CORE)
#define PG_NB_CA_TYPES 7
#else
#define PG_NB_CA_TYPES 4
//const uint CA_CYCLIC = 0;
//const uint CA_CYCLIC_1 = 1;
//const uint GOL_1 = 2;
//const uint CA_PROTOCELLS = 3;
#endif

// NB CA SUB-TYPES
#define PG_NB_CA_SUBTYPES 20

// CURVE VS SPLAT PARTICLES
#if defined(TVW) || defined (BONNOTTE) || defined (TEMPETE) || defined (DAWN) || defined (RIVETS) || defined (ETOILES) \
		|| defined (BICHES)  || defined (ATELIERSENFANTS) || defined (CAVERNEPLATON) || defined (PIERRES) || defined (ENSO) || defined(SONG) \
		|| defined(FORET) || defined(SOUNDINITIATIVE) || defined(CORE)
#define TEXTURED_QUAD_PARTICLES
#elif defined (DASEIN)
#define CURVE_PARTICLES
#elif defined (DEMO) || defined (DEMO_BEZIER) || defined (VOLUSPA)|| defined (INTERFERENCE) \
		|| defined (CAAUDIO) || defined (REUTLINGEN) || defined (ULM) || defined (ARAKNIT)
#define LINE_SPLAT_PARTICLES
#endif

#ifdef CURVE_PARTICLES
#define PG_PARTICLE_CURVE_DEGREE 3
#endif

#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
// NB PARTICLE MODES
#define PG_NB_PARTMOVE_MODES 5

#define PG_NB_PARTREPOP_MODES 2
#define PARTREPOP_GRID 1

#define PG_NB_PARTEXIT_MODES 3
#define PG_NB_PARTSTROKE_MODES 4
#define PG_NB_PARTCOLOR_MODES 3
#endif

// CLIPS
#if defined (SOUNDINITIATIVE) || defined(CORE)
#define PG_WITH_CLIPS
// different clips on right and left screens
#define PG_NB_PARALLEL_CLIPS 2
#endif

// MASTER's MASK
#if defined (CAVERNEPLATON) || defined (INTERFERENCE) || defined (PIERRES) || defined (ENSO) || defined (ARAKNIT) || defined (VOLUSPA) \
		|| defined (SONG) || defined (FORET) || defined (SOUNDINITIATIVE) || defined(CORE)
#define PG_WITH_MASTER_MASK
#endif

// BURST's MASK
#if defined (PIERRES)
#define PG_WITH_BURST_MASK
//#define PG_WITH_PHOTO_HOMOGRAPHY
#endif

// SENSORS
#if defined (CAAUDIO)
// #define PG_SENSORS
// #define PG_NB_SENSORS 1
// #define PG_NB_MAX_SENSOR_ACTIVATIONS 2
#define BEAT_DURATION (0.1f)
#define PG_PUREDATA_SOUND
#endif
#if defined (DEMO) || defined (DEMO_BEZIER) || defined (GN) || defined (REUTLINGEN) \
		|| defined (BICHES) || defined (ATELIERSENFANTS) || defined (CAVERNEPLATON) || defined(FORET) || defined(CORE)
#define PG_SENSORS
#define PG_NB_SENSORS 16
#define PG_NB_MAX_SENSOR_ACTIVATIONS 6
#define BEAT_DURATION (1.0f)
#define PG_RENOISE
#endif
#if defined (CAVERNEPLATON) || defined (TEMPETE) || defined (ENSO) || defined (ETOILES) || defined (CORE)
#define PG_MESHES
#if defined (TEMPETE) || defined(ENSO)
// FBO capture of Master to be displayed on a mesh for augmented reality
#define PG_AUGMENTED_REALITY
#if defined (TEMPETE)
#define PG_SECOND_MESH_CAMERA
#endif
#endif
#endif
//#if defined (ARAKNIT) // || defined(CAVERNEPLATON)
//#define PG_METAWEAR
//#endif
#ifdef PG_SENSORS
#define PG_SENSOR_TEXTURE_WIDTH 100
#define PG_SENSOR_GEOMETRY_WIDTH 10
#define PG_NB_MAX_SENSOR_LAYOUTS 4
#define PG_NB_MAX_SAMPLE_SETUPS 3
#endif
#ifdef SUPERCOLLIDER
#define PG_NB_SENSOR_GROUPS 12
#endif

// BEZIER CURVES INSTEAD OF LINES FOR PEN
#if defined(KOMPARTSD) || defined (DEMO_BEZIER) || defined (CAVERNEPLATON) || defined (SONG) || defined (FORET) || defined (SOUNDINITIATIVE) \
		|| defined (PIERRES) || defined (ENSO) || defined (TEMPETE) || defined (DAWN) ||defined (RIVETS) || defined (ULM) || defined (BICHES) \
		|| defined (ATELIERSENFANTS) || defined (VOLUSPA) || defined (ETOILES) || defined (ARAKNIT) || defined (LIGHT) || defined(ALKEMI) || defined(CORE)
#define PG_BEZIER_PATHS
#endif

// PHOTO FLASH ON CA IN ADDITION TO CAMERA FLASH
#if defined (CAVERNEPLATON) || defined (SONG) || defined(FORET) || defined (SOUNDINITIATIVE) || defined (PIERRES) || defined (ENSO) || defined (ULM) \
		|| defined (TEMPETE) || defined (ETOILES) || defined(CORE)
#define PG_WITH_PHOTO_FLASH
#endif

// REPOP DENSITY TEXTURE FOR NON HOMOGENEOUS REPOP
#if defined (ETOILES) || defined (BICHES) || defined (CAVERNEPLATON) || defined (SONG) || defined(FORET) \
		|| defined (SOUNDINITIATIVE) || defined (ENSO) || defined(CORE)
#define PG_WITH_REPOP_DENSITY
#endif


#ifdef _WIN32
	//#ifdef PG_WACOM_TABLET
		#include "../share/freeglut-wacom/include/GL/freeglut.h"
	//#else // PG_WACOM_TABLET
	//       #include "../share/freeglut-3.0.0/include/GL/freeglut.h"
	//#endif // _WIN32
#endif
#ifdef LINUX
	#include <GL/freeglut.h>
	#include <GL/freeglut_ext.h>

    #define   GLX_GLXEXT_LEGACY
	#include <GL/glx.h>
#endif
#ifdef __APPLE_CC__
	#include <GLUT/glut.h>
#endif // __APPLE_CC__

#ifdef GN
#include "pg_script_header_GN.h"
#endif
#if defined(TVW)
#include "pg_script_header_TVW.h"
#endif
#if defined (CRITON)
#include "pg_script_header_Criton.h"
#endif
#if defined (KOMPARTSD)
#include "pg_script_header_KompartSD.h"
#endif
#if defined (LIGHT)
#include "pg_script_header_Light.h"
#endif
#if defined (REUTLINGEN)
#include "pg_script_header_Reutlingen.h"
#endif
#if defined (BICHES)
#include "pg_script_header_Biches.h"
#endif
#if defined (ATELIERSENFANTS)
#include "pg_script_header_AteliersEnfants.h"
#endif
#if defined (CAVERNEPLATON)
#include "pg_script_header_CavernePlaton.h"
#endif
#if defined (PIERRES)
#include "pg_script_header_Pierres.h"
#endif
#if defined (ENSO)
#include "pg_script_header_Enso.h"
#endif
#if defined (TEMPETE)
#include "pg_script_header_Tempete.h"
#endif
#if defined (DAWN)
#include "pg_script_header_Dawn.h"
#endif
#if defined (RIVETS)
#include "pg_script_header_Rivets.h"
#endif
#if defined (ULM)
#include "pg_script_header_Ulm.h"
#endif
#ifdef SONG
#include "pg_script_header_Song.h"
#endif
#ifdef CORE
#include "pg_script_header_Core.h"
#endif
#ifdef FORET
#include "pg_script_header_Foret.h"
#endif
#ifdef SOUNDINITIATIVE
#include "pg_script_header_SoundInitiative.h"
#endif
#ifdef ALKEMI
#include "pg_script_header_alKemi.h"
#endif
#if defined (DEMO) || defined (DEMO_BEZIER)
#include "pg_script_header_demo.h"
#endif
#ifdef VOLUSPA
#include "pg_script_header_voluspa.h"
#endif
#ifdef ARAKNIT
#include "pg_script_header_araknit.h"
#endif
#if defined (ETOILES)
#include "pg_script_header_etoiles.h"
#endif
#ifdef INTERFERENCE
#include "pg_script_header_interference.h"
#endif
#ifdef CAAUDIO
#include "pg_script_header_CAaudio.h"
#endif
#ifdef DASEIN
#include "pg_script_header_dasein.h"
#endif
#ifdef BONNOTTE
#include "pg_script_header_bonnotte.h"
#endif

#include "pg-udp.h"
//#include "pg-USB.h"
#ifdef PG_MESHES
#include "pg-mesh.h"
#endif
#include "pg-script.h"
#include "pg-update.h"
#include "pg-texture.h"

#include "pg-shader.h"
#include "pg-stroke.h"
#ifdef PG_WITH_CLIP_ART
#include "pg-svg-clip-art.h"
#endif
#include "pg-init.h"

#include "pg-main.h"
#include "pg-conf.h"

#include "pg-gstreamer.h"


// end of standard included files
///////////////////////////////////////////////////////
