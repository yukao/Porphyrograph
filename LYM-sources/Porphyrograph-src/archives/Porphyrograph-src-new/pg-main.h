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
  /// _Render : rendering,
  /// _Jpg : framebuffer output to jpg file
  /// _Png : framebuffer output to png file
  /// _Svg : framebuffer output to svg file
enum DrawingMode { _Render = 0, _Svg, _Png, _Jpg, _Video };


extern string ScreenMessage;
extern GLfloat messageTransparency;
extern bool NewScreenMessage;
extern bool ScreenMessageInitialized;

extern int Step;

extern bool DisplayFramePerSecond;

extern double pg_CurrentClockTime;
extern int FramePerSecond;
extern float LastImageSwap;

extern std::stringstream  date_stringStream;

// frame number 
extern int      pg_FrameNo;
extern int      pg_ConfigurationFrameNo;
// particle target frame number 
extern int      pg_ParticleTargetFrameNo;
// last frame with camera parameter change
extern int      pg_LastCameraParameterChange_Frame;

// current window & user  ---  TEMPORARY

class pg_Window;
extern pg_Window    *CurrentWindow;
extern bool           windowDisplayed;

extern int tablet_prec_x;
extern int tablet_prec_y;

// the width of the working window
// half of the display in case of double window (doubling is made in the Master shader in this case)
// full display otherwise
extern int workingWindow_width;
extern int workingWindow_width_powerOf2;
extern float workingWindow_width_powerOf2_ratio;
extern bool muteRightScreen;

// the smallest powers of 2 for width and height
extern int window_width_powerOf2;
extern int window_height_powerOf2;
extern float window_width_powerOf2_ratio;
extern float window_height_powerOf2_ratio;

extern int rightWindowVMargin;

/// trace
extern FILE    *pg_csv_log_file;

/// error message and input buffer
extern char *ErrorStr;

// Input network message string
extern char *Input_Message_String;
extern char *Input_Message_Local_Commande_String;
extern char *Output_Message_String;

/// current mouse location
extern int CurrentMousePos_x[PG_NB_CURSORS_MAX], CurrentMousePos_y[PG_NB_CURSORS_MAX];
extern int Pulsed_CurrentMousePos_x[PG_NB_CURSORS_MAX], Pulsed_CurrentMousePos_y[PG_NB_CURSORS_MAX];
extern float LastCursorPositionUpdate[PG_NB_CURSORS_MAX];
extern float paths_x_forGPU[PG_NB_CURSORS_MAX], paths_y_forGPU[PG_NB_CURSORS_MAX];
extern float paths_x_forGPU_prev[PG_NB_CURSORS_MAX], paths_y_forGPU_prev[PG_NB_CURSORS_MAX];
extern float paths_x_forGPU_last_pos_time[PG_NB_CURSORS_MAX];
extern float paths_x_begin_position[PG_NB_CURSORS_MAX], paths_y_begin_position[PG_NB_CURSORS_MAX];
extern int CurrentCursorHooverPos_x, CurrentCursorHooverPos_y;
extern int CurrentCursorStylusvsRubber;
#define pg_Stylus  1
#define pg_Rubber -1
#define PG_IDLE_CURSOR -5
// current tablet pen pressure and orientation
extern float tabletPressureRadius;
extern float tabletAzimutRadius;
extern float tabletInclinationRadius;

/// initial time
extern double InitialRealTime;
extern double LastScreenMessageDecayTime;

/*! \name Group size_view_volume
 *  \brief size of the view volume
 */
// @{
extern int  *input_argc_stack;
extern char  **input_message_stack;
extern int   *input_message_length_stack;
// @}

/// OpenGL background color
extern float OpenGLBGcolor[4];

/// Error string
extern char* ErrorStr;
// current time
extern double pg_CurrentClockTime;
extern double PrecedingClockTime;
// error report
void ReportError(char* errorString);


int main(int argc, char **argv);

/// screen_message initialization
void pg_init_screen_message( void );

/// OpenGL and glut initialization
void OpenGLInit( void );

/// scene initialization
void pg_init_scene( void );

void pg_draw_scene( DrawingMode mode );

/// main window initialization
void initGlutWindows( void );
void window_idle_browse( void );
void window_idle_browse( int step );

void pg_process_special_key( int key );
void pg_process_key( int key );

/// scene termination
void pg_quit( void );

/* void window_idle_browse1( int step ); */
void window_display( void );
void window_reshape(GLsizei width, GLsizei height); 

void window_key_browse(unsigned char key, int x, int y) ;
void window_key_browse(unsigned char key, int x, int y) ;
void window_special_key_browse(int key, int x, int y) ;

#if defined(PG_WACOM_TABLET)
void window_PG_WACOM_TABLET_browse(int x, int y, float press, float az, float incl, int twist , int cursor );
#endif

void window_mouseFunc_browse(int button, int state, int x, int y) ;
void window_motionFunc_browse(int x, int y) ;
void window_passiveMotionFunc_browse(int x, int y);
void window_idle_browse( int step );

void CursorInit( void );

#endif
