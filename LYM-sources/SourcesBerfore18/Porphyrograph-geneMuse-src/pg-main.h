/*! \file pg-main.h
 *  \brief the header file of pg-main.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
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
#define Nil                             -1

#define WIDTH  800
#define HEIGHT 600

#define KEY_ESC 27
#define PI 3.1415926535898

#define PG_NB_MAX_EMBEDDED_XML_FILES 16

#define     NULL_ID   0

/// XML files
extern FILE    *fileXML[ PG_NB_MAX_EMBEDDED_XML_FILES ];
extern int     lastChar[ PG_NB_MAX_EMBEDDED_XML_FILES ];

/// mouse constants
enum MouseButtonLocation { PG_BUTTON_LEFT = 0, PG_BUTTON_MIDDLE, PG_BUTTON_RIGHT , PG_WHEEL_UP , PG_WHEEL_DOWN , PG_BUTTON_EMPTY };
extern const char *MouseButtonLocationString[PG_BUTTON_EMPTY + 1];

enum ButtonPosition { PG_BUTTON_UP = 1, PG_BUTTON_DOWN, PG_BUTTON_OCCUPIED };

/// constants in edit mode
enum ListePositions{ _Libre = 0, _Canonique };
enum RotationTranslationModes{ Rotation = 0, Translation };

#define _Max(b,c) ((b)>(c)? (b):(c))
#define _Min(b,c) ((b)<(c)? (b):(c))
#define _Max3(a,b,c) _Max(a,_Max(b,c))
#define _Min3(a,b,c) _Min(a,_Min(b,c))

#define InputBufferSize                 4096

typedef class pg_Viewpoint *   PViewpoint;

/*! \name Group message_drawing
 *  \brief pg_Text message drawing
 */
// @{
extern char ScreenMessage[XMLAttrValLength];
extern int ScreenMessageX;
extern int ScreenMessageY;
extern GLfloat messageTransparency;
extern bool NewScreenMessage;
extern bool ScreenMessageInitialized;
// @}

extern int Step;
extern float CurrentRadianAngle;

extern int Position;
extern RotationTranslationModes RotationTranslation;

/// elementary moving steps
extern float ElementaryRotationAngle;

extern int         nbKnownTargets;
extern bool DisplayFramePerSecond;

extern int user_id;

extern int GenSym;
extern double CurrentClockTime;
extern int FramePerSecond;
extern double LastFrameTime;

/// image number 
extern int      FrameNo;

// current window & user  ---  TEMPORARY

class pg_Window;
class pg_User;
extern pg_Window    *CurrentWindow;

extern int singleWindowWidth;
extern int doubleWindowWidth;
extern int windowHeight;

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

/*! \name Group cyclic_input_buffer
 *  \brief cyclic input buffer
 */
// @{
extern char *InputBuffer;
extern int   InputBufferCurrentBegin;
extern int   InputBufferCurrentEnd;
// @}

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
extern int   nbUDP_input_message;
// @}

/// OpenGL background color
extern float OpenGLBGColor[4];

int main(int argc, char **argv);

/// screen_message initialization
void pg_init_screen_message( void );

/*!
 * \brief window and user managemet
 * \param indWindow			to be added
 */
void SetUserAndWindow( void );

/// sets current user and window according to the ID of the current window
void GetUserAndWindow( void );

/*! 
 * \brief arbitrary sets current user and window (should be avoided)
 * \param indWindow			to be added
 */
void SetGlutWindow( void );

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
#ifdef WACOM_TABLET
void window_wacom_tablet_browse(int x, int y, float press, float az, float incl, int twist );
#endif

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
