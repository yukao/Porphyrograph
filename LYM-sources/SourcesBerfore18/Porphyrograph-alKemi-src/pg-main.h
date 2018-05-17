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
#ifndef _VISUAL_STUDIO
#define MAXFLOAT                        3.40282347e+38F
#else
#define MAXFLOAT                        3.40282346e+38F
#endif
#endif

#define KEY_ESC 27
#define PI 3.1415926535898

#define   StringLength   1024

#define     NULL_ID   0


extern char ScreenMessage[StringLength];
extern GLfloat messageTransparency;
extern bool NewScreenMessage;
extern bool ScreenMessageInitialized;

extern int Step;

extern bool DisplayFramePerSecond;

extern double CurrentClockTime;
extern int FramePerSecond;
extern double LastFrameTime;

/// image number 
extern int      FrameNo;

// current window & user  ---  TEMPORARY

class pg_Window;
extern pg_Window    *CurrentWindow;
extern bool           windowDisplayed;

extern int leftWindowWidth;
extern int rightWindowWidth;
extern int rightWindowVMargin;
extern int doubleWindowWidth;

/// trace
extern FILE    *fileLog;

/// error message and input buffer
extern char *ErrorStr;

// Input network message string
extern char *Input_Message_String;
extern char *Input_Message_Local_Commande_String;
extern char *Output_Message_String;
extern char *Output_Message_Pattern;
extern char *Output_Message_ArgList;
extern char *Output_Message_OSC;

/// current mouse location
extern int CurrentMousePos_x, CurrentMousePos_y;
extern int CurrentCursorHooverPos_x, CurrentCursorHooverPos_y;
// current tablet pen pressure and orientation
extern float tabletPressureRadius;
extern float tabletAzimutRadius;
extern float tabletInclinationRadius;

/// initial time
extern double InitialRealTime;
extern double LastDecayTime;

/*! \name Group size_view_volume
 *  \brief size of the view volume
 */
// @{
extern char  **input_message_stack;
extern int   *input_message_length_stack;
// @}

/// OpenGL background color
extern float OpenGLBGColor[4];

int main(int argc, char **argv);

/// screen_message initialization
void pg_init_screen_message( void );

/// OpenGL and glut initialization
void OpenGLInit( void );

/// scene initialization
void pg_init_scene( void );

/*! 
 * \brief computes the projection matrix from the frustum
 * \param indWindow			to be added
 */
void projectionMatrix( void );
/*! 
 * \brief computes the inverse modelViewMatrix of the camera
 * \param indWindow			to be added
 */
void cameraToRootMVMatrix( bool withPushMatrix );
void pg_screenMessage_update( void );
void pg_draw_scene( DrawingMode mode );

/// main window initialization
void initGlutWindows( void );
void window_idle_browse( void );
void window_idle_browse( int step );

void pg_process_special_key( int key );
void pg_process_key( int key );

/// scene termination
void terminates_and_exits( void );

/* void window_idle_browse1( int step ); */
void window_display_blank( void );
void window_display( void );
void window_reshape(GLsizei width, GLsizei height); 

void window_key_browse(unsigned char key, int x, int y) ;
void currentViewpoint( void );
void window_key_browse(unsigned char key, int x, int y) ;
void window_special_key_browse(int key, int x, int y) ;

#ifdef WACOM_TABLET
void window_wacom_tablet_browse(int x, int y, float press, float az, float incl, int twist );
#endif

void updateMouseEnvironmentVariablesAndTables( double theTime );
void window_mouseFunc_browse(int button, int state, int x, int y) ;
void window_motionFunc_browse(int x, int y) ;
void window_passiveMotionFunc_browse(int x, int y);
void window_idle_browse( int step );

void Sauve_Environnement( void );
void Restaure_Environnement( void );
void OpenGLinit( void );
void CursorInit( void );

#endif
