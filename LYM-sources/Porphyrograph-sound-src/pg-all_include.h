////////////////////////////////////////////////////////////////////
// 
// 
//     File pg-all_include.h
// 
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#ifndef _VISUAL_STUDIO
	#include "config.h"
	#include <dirent.h>
#endif // !_VISUAL_STUDIO

// standard included files
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sndfile.hh>
#include <portaudio.h>

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
#include <algorithm>
using std::min;
using std::max;

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
// #include <random>

#ifndef INT_MAX
        #include <limits.h>
#endif

// SOCKETS
#ifndef _WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
        #include <sys/resource.h>
	#include <netdb.h>
	#include <stdint.h>
        #include<sys/stat.h>
        #include<unistd.h>
#else // !_WIN32
	#include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <io.h>
        #include<sys/types.h>
        #include<sys/stat.h>
#endif // _WIN32

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
	#include <pthread.h>
        #include<sys/stat.h>
        #include<unistd.h>
#endif // _WIN32
// \}


// STEINBERG SDK
#include "aeffect.h"
#include "aeffectx.h"
#include "vstfxstore.h"

// AQUILA FFT
// #include "aquila/global.h"
// #include "aquila/transform/FftFactory.h"
#include <algorithm>
#include <functional>
#include <memory>

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
#include <sstream>

// #define PG_TERRAINS_VAGUES

// #define STOPPABLE_SEQUENCES

#include "audioeffectx.h"
#include "Filter.h"
#include "MVerb.h"
#include "mdaDelay.h"
#include "pg-FX.h"

#include "pg_conf_header.h"

#include "pg-utils.h"
#include "pg-udp.h"
#include "pg-main.h"
#include "pg-sample.h"
#include "pg-terrains-vagues.h"
#include "pg-conf.h"
#include "pg-audio_IO.h"
// end of standard included files
///////////////////////////////////////////////////////
