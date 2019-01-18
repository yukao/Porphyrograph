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
	// TODO: Now config.h is also generated for Windows.
	#include "config.h"
#endif // _VISUAL_STUDIO

/*! \name Group GLee_header
 *  \brief GLee file for Visual Studio
 */
// \{
#ifndef _VISUAL_STUDIO
#include <dirent.h>
#endif // !_VISUAL_STUDIO
// \}

/*! \name Group function_defintion
 *  \brief  function definition
 */
// \{
#ifdef _VISUAL_STUDIO
	#define PG_INVALID_DIR INVALID_HANDLE_VALUE

	#define pg_readFirstFile(dir, path, file) {dir = FindFirstFile(path, &file);}
	#define pg_readNextFile(dir, file) FindNextFile(dir, &file)
	#define pg_closeDir(dir) FindClose(dir)

	#define pg_getFileName(file) file.cFileName
#else // _VISUAL_STUDIO
	typedef struct dirent * PG_FILE;
	typedef DIR * PG_DIR;

	#define PG_INVALID_DIR NULL

	#define pg_readFirstFile(dir, path, file) {if(dir == NULL) dir = opendir(path); else rewinddir(dir); file = readdir(dir);}
	#define pg_readNextFile(dir, file) (file = readdir(dir))
	#define pg_closeDir(dir) closedir(dir)
	#define pg_getFileName(file) file->d_name
#endif // _VISUAL_STUDIO
// \}

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
#define _USE_MATH_DEFINES
#include <math.h>    // math constants such as M_PI
// define the round function for Visual Studio (not include in math.h)
#ifdef _VISUAL_STUDIO
	#define myround(x) (x >= 0 ? floor(x + 0.5) : ceil(x - 0.5))
	//double round(double x) { return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5); }
#endif // _VISUAL_STUDIO
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string.h>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>

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
#endif // _WIN32
// \}


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

#include <list>
#include <vector>
#include <map>
using std::list;
using std::vector;
using std::map;
using std::string;
using std::ios;
using std::cerr;
using std::ifstream;
#include <fstream>
#include <GL/glew.h>

#define  GL_GLEXT_LEGACY
//#include <GL/gl.h>

#ifdef _WIN32
	#ifdef _VISUAL_STUDIO
#include "../../share/freeglut-3.0.0/include/GL/freeglut.h"
 // #include "C:/Users/christian/Desktop/MaisonSensibleLogiciels/maisonSensible-libraries/freeglut-2.8.1/include/GL/freeglut.h"
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

#define PG_NB_TRACKS 1
#define PG_NB_MEMORIZED_PASS 2
#define PG_FBO_WIDTH 1024
#define PG_FBO_HEIGHT 512

#include "pg_script_header.h"

#include "pg-mesh.h"
#include "pg-udp.h"
#include "pg-script.h"
#include "pg-draw.h"

#include "pg-shader.h"
#include "pg-stroke.h"

#include "pg-main.h"
#include "pg-conf.h"
// end of standard included files
///////////////////////////////////////////////////////
