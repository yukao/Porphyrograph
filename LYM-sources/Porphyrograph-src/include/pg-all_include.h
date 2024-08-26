/*! \file pg-all-include.h
 * 
 * 
 *     File pg-light-all_include.h
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

#include <filesystem>
namespace fs = std::filesystem;

#define _USE_MATH_DEFINES
#include <math.h>    // math constants such as M_PI

#include <cstdlib>      // std::rand, std::srand
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <regex>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <random>
#include <algorithm>    // std::max / min

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
using std::ifstream;


#define rand_0_1 (float(rand()) / float(RAND_MAX))
template<typename T>
constexpr auto rand_0_x(T x) { return (x * rand_0_1); }
template<typename T>
constexpr auto rand_x(T x) { return (x * (2 * rand_0_1 - 1.f)); }

#ifndef INT_MAX
#include <limits.h>
#endif

#if defined(_WIN32)
	// memory leak tracing
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>

	// networking
	#include <winsock2.h>
	#include <Ws2tcpip.h>

	// time reading
	#include <time.h>

	// directory
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <sys/types.h>

	// networking
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/resource.h>
	#include <netdb.h>
	#include <unistd.h>

	#include "config.h"
	#include <dirent.h>

	// time reading
	#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
	#define _TIMEVAL_DEFINED
	#endif
	#include <sys/time.h>
	struct timeval {
		unsigned long int tv_usec;
		unsigned long int tv_sec;
	};

	// threads
	#include <pthread.h>

	// directory
	#include <unistd.h>
	#define GetCurrentDir getcwd
	#include <sys/stat.h>
#endif // !_WIN32

// geometry
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/norm.hpp>

// opencv include files
#define OPENCV_3
#define HAVE_OPENCV_CALIB3D
#define HAVE_OPENCV_HIGHGUI
#define HAVE_OPENCV_IMGPROC
#define HAVE_OPENCV_VIDEO
#define HAVE_OPENCV_VIDEOIO
#include <opencv2/opencv.hpp>
using namespace cv;

// oscpp library
#include <oscpp/server.hpp>
#include <oscpp/print.hpp>
#include <oscpp/client.hpp>
#include <memory>
#include <stdexcept>

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

// Update shader blurring
#if defined(pg_Project_Voluspa)
#define PG_WITH_BLUR
#endif

// light DMX commands
#include <controller_library.h>

// JUCE lib for sound analysis
#if defined(pg_Project_Criton)
#define PG_WITH_JUCE
#endif

// SENSORS
#if defined(pg_Project_CAaudio)
#define PG_PUREDATA_SOUND
#endif

#define PG_RENOISE

#if defined(pg_Project_Tempete)
#define PG_SECOND_MESH_CAMERA
#endif

#define     NULL_ID   0

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

enum pg_Parameter_Input_Type { pg_enum_PG_KEYSTROKE = 0, pg_enum_PG_GUI_COMMAND, pg_enum_PG_SCENARIO, pg_enum_PG_INPUT_VOID };
class ScenarioValue;
#include "pg-header.h"
#if defined(CORE)
#include "pg_script_header_Core.h"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_script_header_voluspa.h"
#endif
#if defined(pg_Project_araKnit)
#include "pg_script_header_araknit.h"
#endif

//////////////////////////////////////////
// module headers
#include "pg-init.h"
#include "pg-main.h"
#include "pg-path.h"
#include "pg-udp.h"
#include "pg-mesh.h"
#include "pg-callBack.h"
#include "pg-photoClip.h"
#include "pg-movieCamera.h"
#include "pg-color.h"
#include "pg-messages.h"
#include "pg-scenarioDyn.h"
#include "pg-flash.h"
#include "pg-script.h"
#include "pg-audio.h"
#include "pg-render.h"
#include "pg-texture.h"
#include "pg-gui.h"

#include "pg-shader.h"
#include "pg-clipArt.h"
#include "pg-conf.h"

#include "pg-light.h"

#include "pg-sensor.h"
#include "pg-template.hpp"

// end of standard included files
///////////////////////////////////////////////////////
