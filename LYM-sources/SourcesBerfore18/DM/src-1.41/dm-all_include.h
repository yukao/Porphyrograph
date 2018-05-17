/*! \file dm-all-include.h
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-all_include.h
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
	#define DM_INVALID_DIR INVALID_HANDLE_VALUE

	#define dm_readFirstFile(dir, path, file) {dir = FindFirstFile(path, &file);}
	#define dm_readNextFile(dir, file) FindNextFile(dir, &file)
	#define dm_closeDir(dir) FindClose(dir)

	#define dm_getFileName(file) file.cFileName
#else // _VISUAL_STUDIO
	typedef struct dirent * DM_FILE;
	typedef DIR * DM_DIR;

	#define DM_INVALID_DIR NULL

	#define dm_readFirstFile(dir, path, file) {if(dir == NULL) dir = opendir(path); else rewinddir(dir); file = readdir(dir);}
	#define dm_readNextFile(dir, file) (file = readdir(dir))
	#define dm_closeDir(dir) closedir(dir)
	#define dm_getFileName(file) file->d_name
#endif // _VISUAL_STUDIO
// \}

/// UNIX included files 
#ifdef UNIX

	/// included constants for application profile
	/// included constants for graphic extensions
	/// could become profiles in the future
	#define DM_STENCILOP

	/// included X files
	#include <X11/keysym.h>
#endif // UNIX

#define DM_TEXTURE3D

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
	#define round(x) (x >= 0 ? floor(x + 0.5) : ceil(x - 0.5))
	//double round(double x) { return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5); }
#endif // _VISUAL_STUDIO
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string.h>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

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

// libjpeg include files
/*
#ifndef _WIN32
	extern "C" {
	#include <jpeglib.h>
	#include <jerror.h>
	}
#else // !_WIN32
	//#define JPEG_INTERNALS     // Hard includes instead of using the JPEG6-LIBRARY:    
	//============ TEST: Avoid warning about FAR macro redefinition...
	// #undef FAR
        #define boolean bool
	extern "C" {
	#include <jpeglib.h>
	#include <jerror.h>
	}
#endif // !_WIN32
*/

// libpng include files
// #include <png.h>

// opencv include files
#include <opencv/cv.h>
#include <opencv/highgui.h>

#ifdef _VISUAL_STUDIO
	// zlib for image streaming
	// With CMake, it directly find and directory including zlib.
	//#include <zlib/zlib.h>
	//#include <zlib.h>
//	typedef unsigned long png_uint_32;
#endif // _VISUAL_STUDIO

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
// \}

/*! \name GL_includes
 *  \breif OpenGL, GLU & GLUT include files
 */
// \{
// OpenGL extensions for 


#ifdef _WIN32
	//TODO: This should be automatically done in CMakeLists.txt file.
	#ifndef _VISUAL_STUDIO
	// openGL extensions such as Frame Buffer Objects: 
	// glBindFramebufferEXT, glCheckFramebufferStatusEXT, glGenFramebuffersEXT
	#ifndef __MINGW32__
	#define __MINGW32__
	#endif // __MINGW32__
	#endif // _VISUAL_STUDIO
#endif

#include <GL/glew.h>

#define  GL_GLEXT_LEGACY
//#include <GL/gl.h>


#ifdef _WIN32

	// #  include <windows.h>
	// #  include <vfw.h>
	// #  pragma warning(disable:4244)   // No warnings on precision truncation
	// #  pragma warning(disable:4305)   // No warnings on precision truncation
	// #  pragma warning(disable:4786)   // stupid symbol size limitation
	#ifdef _VISUAL_STUDIO
		#include <GL/freeglut.h>
	#else // _VISUAL_STUDIO
		#include <glut.h>
	#endif // _VISUAL_STUDIO

#else // _WIN32

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

#ifdef DM_EXTERN_LIVE_VIDEO
// code for external video
#include "Grabber.h"
#endif

//Define this somewhere in your header file
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

/*! \name Group ffmpeg_includes
 *  \brief ffmpeg includes
 */
// \{
#ifdef DM_HAVE_FFMPEG

	extern "C"
	{
	  // under linux, the header files are expected
	  // in the /usr/include/ffmpeg directory
	  // configure file should provide this directory 
	  // as FFMPEG_INC_DIR
//LP: Now done in CMake via a compiler option.
//	#define __STDC_CONSTANT_MACROS
	#define __STDC_LIMIT_MACROS
	#ifdef _VISUAL_STUDIO
		#ifdef DM_BUILD_WITH_CMAKE
			//LP: ffmpeg directory is identified by CMake, linke under unix
			#include <avformat.h>
			#include <avcodec.h>
			#include <avdevice.h>
			// Include ffmpeg swscale module to support colorspace conversions.
			#include <swscale.h>
			#include <vfw.h>
		#else // DM_BUILD_WITH_CMAKE
			#include <ffmpeg/avformat.h>
			#include <ffmpeg/avcodec.h>
		    #ifndef _WIN32
			  #include <ffmpeg/avdevice.h>
			#endif
			#include <ffmpeg/swscale.h>
			#include <vfw.h>
		#endif // DM_BUILD_WITH_CMAKE
	#else // _VISUAL_STUDIO
		#include "avformat.h"
		#include "avcodec.h"
        #include "avdevice.h"
		#include "swscale.h"
	#endif // _VISUAL_STUDIO
	}
#endif // DM_HAVE_FFMPEG
// \}

/*! \name Group application_specfic_includes
 *  \brief application specific includes
 */
// \{
#include "dm-strings.h"
#include "dm-variables.h"
#include "dm-lib.h"
#include "dm-material.h"
#include "dm-geom-objects.h"
#include "dm-non-geom-objects.h"
#include "dm-interpolator.h"
#include "dm-texture.h"
#include "dm-scene.h"
#include "dm-IOfile.h"
#include "dm-script.h"
#include "dm-udp.h"
#include "dm-draw.h"
#include "dm-composition.h"
#include "dm-shader.h"
#include "dm-stroke.h"

#ifdef DM_HAVE_FFMPEG
	#include "dm-video.h"
#endif // DM_HAVE_FFMPEG

#include "dm-main.h"
#include "dm-conf.h"
// end of standard included files
///////////////////////////////////////////////////////
