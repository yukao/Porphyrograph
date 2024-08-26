/*! \file pg-main.h
 *
 * 
 *     File pg-main.h
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

#ifndef PG_MAIN_H
#define PG_MAIN_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAXFLOAT 
#ifndef _WIN32
#define MAXFLOAT                        3.40282347e+38F
#else
#define MAXFLOAT                        3.40282346e+38F
#endif
#endif

#define KEY_ESC 27
#define PI 3.1415926535898

#define   StringLength   1024

enum DrawingMode { pg_enum_render_Display = 0, pg_enum_render_Svg, pg_enum_render_Png, pg_enum_render_Jpg, pg_enum_render_Video };

#define pg_Stylus  1
#define pg_Rubber -1
#define PG_IDLE_CURSOR -5

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// number of loaded scenarios
extern int			   pg_NbScenarios;
extern unsigned int pg_ind_scenario;

// the width of the working window
// half of the display in case of double window (doubling is made in the Master shader in this case)
// full display otherwise
extern int pg_workingWindow_width;
extern int pg_rightWindowVMargin;
// the smallest powers of 2 for width and height
extern int pg_window_width_powerOf2;
extern int pg_window_height_powerOf2;
extern int pg_workingWindow_width_powerOf2;
extern float pg_window_height_powerOf2_ratio;
extern float pg_workingWindow_width_powerOf2_ratio;

// second screen muting
extern bool pg_muteRightScreen;
// whether or not display the window content
extern bool           pg_windowDisplayed;


// particle target frame number 
extern long      pg_ParticleTargetFrameNo;
// last frame with camera parameter change
extern long      pg_LastCameraParameterChange_Frame;

// aux string
extern char pg_AuxString[1024];

// frame number 
extern long   pg_FrameNo;
extern long   pg_ConfigurationFrameNo;
// FPS display
extern bool pg_DisplayFramePerSecond;

// current time
extern double pg_CurrentClockTime;
extern double pg_PrecedingClockTime;
// date string stream
extern std::stringstream  pg_date_stringStream;
/// initial time
extern double pg_InitialRealTime;

/// Error string
extern char* pg_errorStr;

/// trace
extern FILE* pg_csv_log_file;

// Input network message string
extern char* pg_input_message_string;
extern char* pg_input_message_local_command_string;

// Perlin noise seed
extern float pg_seed_pulsePerlinNoise[4 * 2];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

/// scene termination
void pg_quit(void);

/// OpenGL and glut initialization
void pg_OpenGLInit(void);


#endif
