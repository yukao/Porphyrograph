/*! \file dm-main.h
 *  \brief the header file of dm-main.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-main.h
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

#ifndef DM_MAIN_H
#define DM_MAIN_H

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

#define DM_NB_MAX_EMBEDDED_XML_FILES 16

/// XML files
extern FILE    *fileXML[ DM_NB_MAX_EMBEDDED_XML_FILES ];
extern int     lastChar[ DM_NB_MAX_EMBEDDED_XML_FILES ];

/// mouse constants
enum MouseButtonLocation { DM_BUTTON_LEFT = 0, DM_BUTTON_MIDDLE, DM_BUTTON_RIGHT , DM_WHEEL_UP , DM_WHEEL_DOWN , DM_BUTTON_EMPTY };
extern const char *MouseButtonLocationString[DM_BUTTON_EMPTY + 1];

enum ButtonPosition { DM_BUTTON_UP = 1, DM_BUTTON_DOWN, DM_BUTTON_OCCUPIED };

/// constants in edit mode
enum ListePositions{ _Libre = 0, _Canonique };
enum RotationTranslationModes{ Rotation = 0, Translation };

#define _Max(b,c) ((b)>(c)? (b):(c))
#define _Min(b,c) ((b)<(c)? (b):(c))
#define _Max3(a,b,c) _Max(a,_Max(b,c))
#define _Min3(a,b,c) _Min(a,_Min(b,c))

#define InputBufferSize                 4096

extern dm_SceneNode   * envtNode;

typedef class dm_Viewpoint *   PViewpoint;

/*! \name Group message_drawing
 *  \brief dm_Text message drawing
 */
// @{
extern char ScreenMessage[XMLAttrValLength];
extern int ScreenMessageX;
extern int ScreenMessageY;
extern GLfloat ScreenMessageColor;
extern bool NewScreenMessage;
extern bool ScreenMessageInitialized;
// @}

extern strAllSceneObject* AllSceneObject;
extern strAllScript* AllScript;
extern strAllTexture* AllTexture;

extern int Step;
extern float CurrentRadianAngle;

extern dm_Composition *DM_CompositionNode;

extern int Position;
extern RotationTranslationModes RotationTranslation;

/// elementary moving steps
extern float ElementaryRotationAngle;

extern dm_SceneNode **knownTargets;
extern int         nbKnownTargets;
extern bool DisplayFramePerSecond;

extern int user_id;

extern int GenSym;
extern double CurrentClockTime;
extern int FramePerSecond;
extern double LastFrameTime;

/*! \name Group script_command_frame_output
 *  \brief Script command for current frame output
 */
// @{
extern bool WriteCurrentFrame;
extern int  WriteCurrentFrameIndex;
// @}

/// image number 
extern int      FrameNo;

// current window & user  ---  TEMPORARY

class dm_Window;
class dm_User;
extern dm_Window    *CurrentWindow;
extern dm_User      *CurrentUser;
extern int           CurrentIndUserNode;
extern int           CurrentIndViewPoint;
extern dm_Viewpoint  *CurrentViewPoint;
extern dm_SceneNode  *CurrentViewPointNode;
extern dm_SceneNode  *WindowViewPointWithMouseDown;
extern float         *CurrentViewPointMatrix;
extern dm_Frustum    *CurrentFrustum;
extern dm_Ortho      *CurrentOrtho;
extern dm_SceneNode  *CurrentNearestNode;

/*! \name Group current_height_spherical
 *  \brief current height in spherical coordinates
 */
// @{
extern float CurrentHeight;
extern int depthTransformation;
// @}

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

/*! \name Group rotation_around_horizontal_axis
 *  \brief rotation around an horizontal axis
 */
// @{
extern float spin_x, spin_y;
extern float transl_x, transl_y, transl_z;
// @}

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

/// garbage collection
extern dm_SceneNode *deletedNodeList;
extern dm_Scalar *deletedScalarList;
extern dm_Table *deletedTableList;
extern dm_Matrix *deletedMatrixList;

int main(int argc, char **argv);

/// screen_message initialization
void dm_init_screen_message( void );

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
#ifdef _WIN32
void dm_init_scene( int argc, char **argv );
#else
void dm_init_scene( void );
#endif
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
void draw_GUI( void );
void draw_scene( DrawingMode mode );

/// main window initialization
void initGlutWindows( void );
void window_idle_browse( void );
void window_idle_browse( int step );

void dm_process_special_key( int key );
void dm_process_key( int key );

/// scene termination
void terminates_and_exits( void );

/* void window_idle_browse1( int step ); */
void window_display_blank( void );
void window_display( void );

/*! \name Group resize
 *  \brief  window resize
 */
// @{
/// main window
void window_reshape(GLsizei width, GLsizei height); 

/*! 
 * \brief keyboard events (interactive mode)
 * \param key				to be added
 * \param x					to be added
 * \param y					to be added
 */
void window_key_browse(unsigned char key, int x, int y) ;

/*! 
 * \brief current camera
 *
 * choice of the current viewpoint according to the set of shots
 */
void currentViewpoint( void );
void window_key_browse(unsigned char key, int x, int y) ;

/*! 
 * \brief special keys events (interactive mode)
 * \param key				to be added
 * \param x					to be added
 * \param y					to be added
 */
void window_special_key_browse(int key, int x, int y) ;

void updateMouseEnvironmentVariablesAndTables( double theTime );

/*! 
 * \brief mouse click events (interactive mode)
 * \param button			to be added
 * \param state			to be added
 * \param x					to be added
 * \param y					to be added
 */

void window_mouseFunc_browse(int button, int state, int x, int y) ;

/*! 
 * \brief tablet buttons handling
 * \param button		    	to be added
 * \param state				to be added
 * \param x						to be added
 * \param y						to be added
 */
void window_tabletButton(int button, int state, int x, int y ) ;

/*! 
 * \brief tablet motion handling
 * \param x						to be added
 * \param y						to be added
 */
void window_tabletMotion(int x, int y ) ;

/*! 
 * \brief mouse motion events (interactive mode)
 * \param x					to be added
 * \param y					to be added
 */
void window_motionFunc_browse(int x, int y) ;

/*! 
 * \brief mouse passive events (if selected in configuration window tag)
 * \param x					to be added
 * \param y					to be added
 */
void window_passiveMotionFunc_browse(int x, int y);

/*! 
 * \brief mouse passive events (if not selected in configuration window tag)
 * \param x					to be added
 * \param y					to be added
 */
/* void window_passiveCaptureMouseLocation(int x, int y); */
void window_idle_browse( int step );

void Sauve_Environnement( void );
void Restaure_Environnement( void );
void OpenGLinit( void );
void CursorInit( void );

/*!
 *  \brief release scene objects in all scene graphs
 *  \param allObjects		the input entrance of all the objects in the scene
 */
void releaseSceneObjects( strAllSceneObject* allObjects );

/*!
 * \brief release scripts in all scene files
 * \param allScript				the input entrance of all script link
 */
void releaseAllScript(strAllScript* allScript);

/*!
 * \brief release all scalars defined in all the scene graphs
 * \param listScalar			the input entrance of all list scalar
 */
void releaseScalarList(dm_Scalar* listScalar);


/*!
 * \brief release all scalars defined in all the scene graphs
 * \param listScalar			the input entrance of all list scalar
 */
void releaseTableList(dm_Table* listTable);

/*!
 * \brief release all scalars defined in all the scene graphs
 * \param listScalar			the input entrance of all list scalar
 */
void releaseMatrixList(dm_Matrix* listMatrix);

/*!
 * \brief release all textures defined in all the scene graphs
 * \param allTexture			the input entrance of all textures defined
 */
void releaseAllTextures(strAllTexture* allTexture);

#endif
