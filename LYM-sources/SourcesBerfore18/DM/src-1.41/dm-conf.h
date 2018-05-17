/*! \file dm-conf.h
 *  \brief the header files for classes ( \a dm_Window, \a dm_Environment ) 
 *  and structures (\a Cursor_Shape, \a WindowRatio, and \a WindowType)
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-conf.h
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

#ifndef DM_CONF_H
#define DM_CONF_H

/// constants for cursor shapes
enum Cursor_Shape{ NoCursor = 0, LeftArrowCursor , RightArrowCursor , EmptyCursorShape };
extern const char *Cursor_ShapeString[EmptyCursorShape + 1];

/// controlling W/H ratio (the largest dimension is maximized)
///   values:
///   - window_ratio: uses initial window size as ratio w/h
/// DEPRECATED difference between FrustumWindowRatio and OrthoWindowRatio
/// now both are assimilated to ViewVolumeWindowRatio
/// the projection type is defined by the user associated with the window
///   - view_volume_ratio 
///   - no_ratio (default)
enum WindowRatio{ NoWindowRatio = 0 , WindowWindowRatio , FrustumWindowRatio , OrthoWindowRatio , ViewVolumeWindowRatio , EmptyWindowRatio };

extern const char *WindowRatioString[EmptyWindowRatio + 1];

class dm_Environment;
extern dm_Environment * DM_EnvironmentNode;

class dm_User;

/*!
 * \brief display window
 */
class dm_Window {
 protected:

  /*! \name Group values_topwindow_subwindow
   *  \brief exact values for top window or scale for subwindows
   */
  // \{
  int                     windowHeight;
  int                     windowWidth;
  // \}

  /*! \name Group initial_window_height_width
   *  \brief initial window height and width used for computing. 
   * the size ratio in the window_ratio value of the ratio parameter
   */
  // \{
  int                     windowHeightIni;
  int                     windowWidthIni;
  int                     window_x;
  int                     window_y;
 // \}

 public:
  WindowRatio             ratio;

  /// glut ID for the window
  int                     glutID;

  /// texture ID in case of monitor window
  GLuint                  texID;

  char                   *id;
  char                   *user_id;
  int                     userRank;

  int                     windowNonFullScreenHeight;
  int                     windowNonFullScreenWidth;

  dm_Window ();
  ~dm_Window();

  void                     InitWindowVC( void );

  int                      getWindowHeight( void );
  int                      getWindowWidth( void );
  int                      getWindow_x( void );
  int                      getWindow_y( void );
  void                     setWindowHeight( float val );
  void                     setWindowWidth( float val );
  void                     setWindow_x( float val );
  void                     setWindow_y( float val );
  void                     manageRatio( dm_Frustum *currentFrustum , 
					dm_Ortho *currentOrtho );
  void                     position( float x , float y );

  /*! 
   * \brief compoutes a normalized mouse position
   * \param x				the x coordinate of the inuput mouse
   * \param y				the y coordinate of the input mouse
   * 
   * this method will output: position between xMin and xMax on x
   */
  void relativeMousePosition( float * x , float * y );

  /*! 
   * \brief compoutes a normalized mouse variation
   * \param x				the x coordinate of the input: mouse
   * \param y				the y coordinate of the input mouse
   * 
   * this method will output: position between xMax - xMin on x
   */
  void relativeMouseVariation( float * x , float * y );

  void print( FILE *file , int isLong , int depth );
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  void operator=(dm_Window& w);
};

/*!
 * \brief main environment class
 */
class dm_Environment: public dm_NonGeometricalObject {
 public:

  /*! \name composition_output_file
   *  \brief composition_output_file
   */
  // \{
  /// composition output after reading initial scenes
  char                   *composition_output_file_name;
  
  /// text output file name in the edition mode
  char                   *default_print_file_name;
  // \}

  /*! \name Group windows
   *  \brief windows
   */
  // \{
  dm_Window              *DM_Window;
  /// working variable to keep track of displayed windows
  bool                    windowDisplayed;   
  // \}

  /// default user. will be copied into composition->user[ 0 ];
  dm_User                 *default_user;

  /*! \name png_rendering_file
    *  \brief output for screen shots (Png)
	*/
  // \{
  char                    *Png_file_name;
  char                    *Png_shot_dir_name;
  int                      beginPng;
  int                      endPng;
  int                      stepPng;
  bool                     outputPng;
  // \}

  /*! \name svg_rendering_file
    *  \brief output for screen shots (Svg)
	*/
  // \{
  char                    *Svg_file_name;
  char                    *Svg_shot_dir_name;
  int                      beginSvg;
  int                      endSvg;
  int                      stepSvg;
  bool                     outputSvg;
  // \}

  /*! \name jpg_rendering_file
    *  \brief output for screen shots (Jpg)
	*/
  // \{
  char                    *Jpg_file_name;
  char                    *Jpg_shot_dir_name;
  int                      beginJpg;
  int                      endJpg;
  int                      stepJpg;
  bool                     outputJpg;
  // \}

  /*! \name Group video_rendering_file
   * \brief output for screen shots (Video)
   */
  // \{
  char                    *Video_file_name;
  int                      beginVideo_file;
  int                      endVideo_file;
  int                      stepVideo_file;
  bool                     outputVideo_file;
#ifdef DM_HAVE_FFMPEG
  dm_AudioVideoOutData    *audioVideoOutData;
#endif
 // \}

  /*! \name Group frame_rate
   *  \brief frame_rate speed adjustments to hardware
   */
  // \{
  float                    minimal_interframe_latency;
  float                    InterpolationRefreshingVariation;
  bool                     vsync;
  // \}

  /*! \name Group scene_schedule
   *  \brief scene schedule
   */
  // \{
  float                    initial_time;
  float                    time_step;
  bool                     real_time;
  // \}

  /// the name of log file
  char                    *log_file_name;
  bool                     log_date_output;

  /// the name of font file
  char                    *font_file_name;
  char                    *font_file_encoding;
  TextureEncoding          font_texture_encoding;
  int                      font_size;
  int                      font_tex_no;
  int                      message_tex_no;
  int                      message_pixel_length;

  /*! \name Group udp_hosts_clients
   *  \brief udp hosts and clients
   */
  // \{
  dm_IPServer            **IP_Servers;
  int                      nb_IP_Servers;

  dm_IPClient            **IP_Clients;
  int                      nb_IP_Clients;
 // \}

  /*! \name Group trance
   *  \brief trace
   */
  // \{
  bool                     trace_input_file;
  bool                     trace_output_frame_number;
  bool                     trace_input_events;
  bool                     trace_time;
  int                      first_frame_number;
  int                      last_frame_number;
  // \}

  /*! \name Group display
   *  \brief display
   */
  // \{
  bool                     full_screen;
  bool                     game_mode;
  float                    echo;
  // \}

  /// interpolation
  bool                     slerp_shortest_path;

  /*! \name Group texture_display
   *  \brief texture display
   */
  // \{
  bool                     texture_displayed;
  bool                     shader_displayed;
  char                    *shader_programs;
  // \}

   /*! \name Group parameters
    *  \brief parameters
	*/
  // \{
  int                      NbParameters;
  char                    *ParameterValues;
  char                    *ParameterStrings;
  // \}

  /*! \name Group max_values
   *  \brief max values
   */
  // \{
  int                      Nb_max_textures;
  int                      Nb_max_shaders;
  int                      Nb_max_display_lists;
  int                      Nb_max_displays;
  int                      Nb_max_path_components;
  int                      Nb_max_targets;
  int                      Nb_max_embedded_parameters;
  int                      Nb_max_scenes;
  int                      Nb_max_model_view_matrix;
  int                      Nb_max_tracks;
  int                      Nb_max_mouse_recording_frames;
  int                      Max_network_message_length;
  // \}

  /*! \name Group cursor
   *  \brief cursor
   */
  // \{
  int                      cursor_size;
  Cursor_Shape             cursor_shape;
  // \}

  /*! \name Group scalar for modifiers
   *  \brief scalar for modifiers
   */
  // \{
  // current mouse location (also used for displaying the cursor)
  dm_Scalar               *mouse_x_scalar;
  dm_Scalar               *mouse_y_scalar;
  dm_Scalar               *mouse_x_prev_scalar;
  dm_Scalar               *mouse_y_prev_scalar;
  dm_Scalar               *mouse_x_deviation_scalar;
  dm_Scalar               *mouse_y_deviation_scalar;

  dm_Scalar               *pixel_color_r_scalar;
  dm_Scalar               *pixel_color_g_scalar;
  dm_Scalar               *pixel_color_b_scalar;

  dm_Table                *frame_color_r_table;
  dm_Table                *frame_color_g_table;
  dm_Table                *frame_color_b_table;

  dm_Table                *tracks_replay_table;
  dm_Table                *tracks_x_table;
  dm_Table                *tracks_y_table;
  dm_Table                *tracks_x_prev_table;
  dm_Table                *tracks_y_prev_table;
  dm_Table                *tracks_Color_r_table;
  dm_Table                *tracks_Color_g_table;
  dm_Table                *tracks_Color_b_table;
  dm_Table                *tracks_Color_a_table;
  dm_Table                *tracks_BrushID_table;
  dm_Table                *tracks_Radius_table;

  dm_Scalar               *window_width_scalar;
  dm_Scalar               *window_height_scalar;
  dm_Scalar               *current_frame_scalar;
  // \}

  /// allocates environment and sets default values
  dm_Environment();
  ~dm_Environment();

  void initWindows( void );

  void print( FILE *file , int isLong , int depth );
  void display( DrawingMode mode , bool *invalidDL );
  void operator=(dm_Environment& e);
  void parseWindow( int *fileDepth , int *p_c ,
		    dm_XMLTag *XMLtag );
  void parse( int *fileDepth , int *p_c );
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		      IntBinOp operI );
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );
  void setApplicationParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				 FloatBinOp oper ,
				 IntBinOp operI );

  
/*! 
 * \brief environment modifier variables modification
 * \param activeShift						to be added
 * \param activeAlt						to be added
 * \param activeControl				to be added
 */
 void dm_process_modifiers( int activeShift , int activeAlt , 
			     int activeControl );
};

void defaultValues( void );
void LoadConfigurationFile( const char * fileName );

#endif
