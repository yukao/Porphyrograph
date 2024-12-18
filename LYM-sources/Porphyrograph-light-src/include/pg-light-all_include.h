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

#if defined(pg_Project_Voluspa)
#define PG_WITH_BLUR
#endif

// light DMX command in porphyrograph
#include <controller_library.h>

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
#include "pg-light-header.h"
#if defined(CORE)
#include "pg_script_header_Core.h"
#endif
#if defined(pg_Project_Voluspa)
#include "pg_script_header_voluspa.h"
#endif
#if defined(pg_Project_araKnit)
#include "pg_script_header_araknit.h"
#endif

#include "pg-light-init.h"
#include "pg-light-main.h"
#include "pg-udp.h"
#include "pg-light-callBack.h"
#include "pg-color.h"
#include "pg-light-scenarioDyn.h"
#include "pg-light-script.h"
#include "pg-light-update.h"
#include "pg-light-gui.h"

#include "pg-light-conf.h"

#include "pg-light.h"

#include "pg-template.hpp"

// end of standard included files
///////////////////////////////////////////////////////
