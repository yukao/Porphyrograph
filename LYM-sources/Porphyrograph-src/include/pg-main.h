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

 /*! \name Group texture_ mode
  */
  // \{
  /// pg_enum_render_Display : rendering,
  /// pg_enum_render_Jpg : framebuffer output to jpg file
  /// pg_enum_render_Png : framebuffer output to png file
  /// pg_enum_render_Svg : framebuffer output to svg file
enum DrawingMode { pg_enum_render_Display = 0, pg_enum_render_Svg, pg_enum_render_Png, pg_enum_render_Jpg, pg_enum_render_Video };


extern GLfloat pg_messageTransparency;
extern bool pg_newScreenMessage;

extern int	pg_NbConfigurations;

extern int pg_nbFramesDisplayed;

extern bool pg_DisplayFramePerSecond;

extern double pg_CurrentClockTime;
extern int pg_FramePerSecond;

extern std::stringstream  pg_date_stringStream;

// frame number 
extern long   pg_FrameNo;
extern long      pg_ConfigurationFrameNo;
// particle target frame number 
extern long      pg_ParticleTargetFrameNo;
// last frame with camera parameter change
extern long      pg_LastCameraParameterChange_Frame;

// current window & user  ---  TEMPORARY

class pg_WindowData;
extern pg_WindowData    *pg_CurrentWindow;
extern bool           pg_windowDisplayed;

// the width of the working window
// half of the display in case of double window (doubling is made in the Master shader in this case)
// full display otherwise
extern int pg_workingWindow_width;
extern int pg_workingWindow_width_powerOf2;
extern float pg_workingWindow_width_powerOf2_ratio;
extern bool pg_muteRightScreen;

// the smallest powers of 2 for width and height
extern int pg_window_width_powerOf2;
extern int pg_window_height_powerOf2;
extern float pg_window_width_powerOf2_ratio;
extern float pg_window_height_powerOf2_ratio;

extern int pg_rightWindowVMargin;

/// trace
extern FILE    *pg_csv_log_file;

/// error message and input buffer
extern char *pg_errorStr;

// Input network message string
extern char *pg_input_message_string;
extern char *pg_input_message_local_command_string;
extern char *pg_output_message_string;

/// current mouse location
extern int pg_CurrentCursorPos_x[PG_NB_CURSORS_MAX], pg_CurrentCursorPos_y[PG_NB_CURSORS_MAX];
extern int pg_Pulsed_CurrentCursorPos_x[PG_NB_CURSORS_MAX], pg_Pulsed_CurrentCursorPos_y[PG_NB_CURSORS_MAX];
extern float pg_LastCursorPositionUpdate[PG_NB_CURSORS_MAX];
extern int pg_CurrentStylusHooverPos_x, pg_CurrentStylusHooverPos_y;
extern int pg_CurrentStylus_StylusvsRubber;
#define pg_Stylus  1
#define pg_Rubber -1
#define PG_IDLE_CURSOR -5
// current tablet pen pressure and orientation
extern float pg_CurrentStylusPresuse;
extern float pg_CurrentStylusAzimut;
extern float pg_CurrentStylusInclination;

/// initial time
extern double pg_InitialRealTime;
extern double pg_LastScreenMessageDecayTime;

/// OpenGL background color
extern float pg_OpenGLBGcolor[4];

/// Error string
extern char* pg_errorStr;
// current time
extern double pg_CurrentClockTime;
extern double pg_PrecedingClockTime;

// +++++++++++++++++++++++ Aux Var +++++++++++++++++++++++++++++++++
extern char pg_AuxString[1024];


// error report
void pg_ReportError(char* errorString);

int main(int argc, char **argv);

/// screen_message initialization
void pg_init_screen_message( void );

/// OpenGL and glut initialization
void pg_OpenGLInit( void );

/// scene initialization
void pg_init_scene( void );

void pg_draw_scene( DrawingMode mode );

/// main window initialization
void pg_initGLutWindows( void );
void pg_window_idle_browse( void );
void pg_window_idle_browse( int step );

void pg_process_special_key( int key );
void pg_process_key( int key );

/// scene termination
void pg_quit( void );

/* void pg_window_idle_browse1( int step ); */
void pg_window_reshape(GLsizei width, GLsizei height); 

void pg_window_key_browse(unsigned char key, int x, int y) ;
void pg_window_key_browse(unsigned char key, int x, int y) ;
void pg_window_special_key_browse(int key, int x, int y) ;

#if defined(PG_WACOM_TABLET)
void pg_window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist , int cursor );
#endif

void pg_window_mouseFunc_browse(int button, int state, int x, int y) ;
void pg_window_motionFunc_browse(int x, int y) ;
void pg_window_passiveMotionFunc_browse(int x, int y);
void pg_window_idle_browse( int step );

void pg_CursorInit( void );

#endif
