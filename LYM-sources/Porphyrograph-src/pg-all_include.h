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
 * but WITHOUT ANY WARRANTY; without even the implied warranty webcam
 * 
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

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <filesystem>
namespace fs = std::filesystem;

#define _USE_MATH_DEFINES
#include <math.h>    // math constants such as M_PI
// define the round function for Visual Studio (not include in math.h)
#if defined(_WIN32)
	#include <conio.h>
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
#include <random>
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>    // std::max / min


#if defined(_WIN32)
// memory leak tracing
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define rand_0_1 (float(rand()) / float(RAND_MAX))
template<typename T>
constexpr auto rand_0_x(T x) { return (x * rand_0_1); }
template<typename T>
constexpr auto rand_x(T x) { return ( x * (2 * rand_0_1 - 1.f)); }

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
#if defined(_WIN32)
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
#if defined(_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#include <sys/stat.h>
#endif

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
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
#define HAVE_OPENCV_CALIB3D
#define HAVE_OPENCV_HIGHGUI
#define HAVE_OPENCV_IMGPROC
#define HAVE_OPENCV_VIDEO
#define HAVE_OPENCV_VIDEOIO
#include <opencv2/opencv.hpp>
#endif
using namespace cv;

// oscpp library
#include <oscpp/server.hpp>
#include <oscpp/print.hpp>
#include <oscpp/client.hpp>
#include <memory>
#include <stdexcept>

#include <list>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
using std::list;
using std::vector;
using std::array;
using std::map;
using std::string;
using std::ios;
using std::cerr;
using std::ifstream;

//GLEW
#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#endif

#define  GL_GLEXT_LEGACY

#if defined(_WIN32)
#ifndef PG_WACOM_TABLET
#define PG_WACOM_TABLET
#endif
#endif // _WIN32

#if defined(VOLUSPA)
#define PG_WITH_BLUR
#endif

#if defined(ARAKNIT) || defined(LIGHT) || defined(CORE)
#if defined(var_light1_dimmer)
// light variables inside scenario
#define PG_LIGHTS_CONTROL_IN_PG
#else
// curve based control in Python
#define PG_LIGHTS_CONTROL_IN_PYTHON
#endif
#if defined(ARAKNIT) || defined(LIGHT) || defined(CORE)
// light DMX command in porphyrograpa
#define PG_LIGHTS_DMX_IN_PG
#include <controller_library.h>
#else
// light DMX command in Python
#define PG_LIGHTS_DMX_IN_PYTHON
#endif
#endif

#define PG_WITH_PORTAUDIO

#define PG_MAX_OSC_ARGUMENTS 96

#if defined(PG_WITH_MIDI)
#define PG_MIDI
#include "portmidi.h"
#include "pg-MIDI.h"
#endif


//#if defined(CRITON) || defined(LIGHT)
//// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 3 WITH THE CURRENT VARIABLES) 
//#define PG_NB_PATHS 3   // **** ALSO TO BE CHANGED IN UPDATE FRAGMENT SHADER ****
//#elif defined(CORE)
//// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 11 WITH THE CURRENT VARIABLES) 
//#define PG_NB_PATHS 11   // **** ALSO TO BE CHANGED IN UPDATE AND PARTICLE ANIMATION FRAGMENT SHADER ****
//#else
//// NB PATHS (0=CURRENT DRAWING HAS TO BE <= 7 WITH THE CURRENT VARIABLES) 
//#define PG_NB_PATHS 7   // **** ALSO TO BE CHANGED IN UPDATE FRAGMENT SHADER ****
//#endif

// for a tactile entry, there is no mouseover and the path has to be broken if the distance between two consecutive points is too big
#if defined(var_fingers)
#define PG_TACTILE_TABLET
#endif


// REST DELAY FOR CAMERA AFTER CHANGE OF PARAMETERS
#define PG_CAMERA_REST_DELAY 240

// NB OF POSSIBLE CURSORS THROUGH HAND INTERFACE
#if defined(var_fingers)
#define PG_NB_CURSORS_MAX 5
#else
#define PG_NB_CURSORS_MAX 1
#endif

#if !defined(LIGHT)
#define PG_WITH_SOUND_LEVELS_GUI_FEEDBACK
#endif

// HSV color system instead of Palettes + grey
#if !defined(var_fingers)
#define PG_NB_CURSORS_MAX 1
#endif

// NB NOISE TYPES
#define PG_NB_NOISE_TYPES 4
// regular Perlin noise
// sun ray Perlin noise
// camera noise
// video noise

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
#define PG_NB_PIXEL_MODES 3
// no pixels
// pixels
// firework


// NB CA SUB-TYPES
#define PG_NB_CA_SUBTYPES 20

// CURVE VS SPLAT PARTICLES
#if defined(DASEIN)
#define CURVE_PARTICLES
#elif defined(VOLUSPA) || defined(CAAUDIO) || defined(ARAKNIT)
#define LINE_SPLAT_PARTICLES
#else
#define TEXTURED_QUAD_PARTICLES
#endif

#if defined(CURVE_PARTICLES)
#define PG_PARTICLE_CURVE_DEGREE 3
#endif

// CLIPS
// different clips on right and left screens
#define PG_NB_PARALLEL_CLIPS 2

// MASTER's MASK
#if defined(ARAKNIT) || defined(VOLUSPA) ||  defined(CORE)
#define PG_WITH_MASTER_MASK
#endif

// SENSORS
#if defined(CAAUDIO)
#define BEAT_DURATION (0.1f)
#define PG_PUREDATA_SOUND
#endif

#define PG_NB_SENSORS 16
#define PG_NB_MAX_SENSOR_ACTIVATIONS 6
#define BEAT_DURATION (1.0f)
#define PG_RENOISE

#if defined(TEMPETE)
#define PG_SECOND_MESH_CAMERA
#endif

#define PG_SENSOR_TEXTURE_WIDTH 100
#define PG_SENSOR_GEOMETRY_WIDTH 10
#define PG_NB_MAX_SENSOR_LAYOUTS 4
#define PG_NB_MAX_SAMPLE_SETUPS 3

// BEZIER CURVES INSTEAD OF LINES FOR PEN
#if defined(KOMPARTSD) || defined(VOLUSPA) || defined(ARAKNIT) || defined(LIGHT) || defined(CORE)
#define PG_BEZIER_PATHS
#endif

#if defined(_WIN32)
	#include "../share/freeglut-wacom/include/GL/freeglut.h"
#endif
#if defined(LINUX)
	#include <GL/freeglut.h>
	#include <GL/freeglut_ext.h>

    #define   GLX_GLXEXT_LEGACY
	#include <GL/glx.h>
#endif
#if defined(__APPLE_CC__)
	#include <GLUT/glut.h>
#endif // __APPLE_CC__

#if defined(CRITON)
#include "pg_script_header_Criton.h"
#endif
#if defined(KOMPARTSD)
#include "pg_script_header_KompartSD.h"
#endif
#if defined(LIGHT)
#include "pg_script_header_Light.h"
#endif
#if defined(CORE)
#include "pg_script_header_Core.h"
#endif
#if defined(VOLUSPA)
#include "pg_script_header_voluspa.h"
#endif
#if defined(ARAKNIT)
#include "pg_script_header_araknit.h"
#endif
#if defined(CAAUDIO)
#include "pg_script_header_CAaudio.h"
#endif

#include "pg-light.h"
#include "pg-init.h"
#include "pg-udp.h"
#include "pg-mesh.h"
#include "pg-script.h"
#include "pg-update.h"
#include "pg-texture.h"

#include "pg-shader.h"
#include "pg-stroke.h"
#include "pg-clip-art.h"
#include "pg-main.h"
#include "pg-conf.h"

#if defined(PG_WITH_PORTAUDIO)
#include "pg-audio.h"
#endif


// end of standard included files
///////////////////////////////////////////////////////
