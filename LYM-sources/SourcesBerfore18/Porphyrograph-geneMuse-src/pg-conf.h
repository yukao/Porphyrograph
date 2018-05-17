/*! \file pg-conf.h
 *  \brief the header files for classes ( \a pg_Window, \a pg_Environment ) 
 *  and structures (\a cursorShape, and \a WindowType)
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-conf.h
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

#ifndef PG_CONF_H
#define PG_CONF_H

/// constants for cursor shapes
enum Cursor_Shape{ NoCursor = 0, LeftArrowCursor , RightArrowCursor , EmptyCursorShape };
extern const char *Cursor_ShapeString[EmptyCursorShape + 1];

enum TagType{ OpeningTag = 0, ClosingTag , EmptyTag };

enum InterpolationType{ pg_linear_interpolation = 0, pg_cosine_interpolation , pg_stepwise_interpolation , EmptyInterpolationType };

class pg_Environment;
extern pg_Environment * PG_EnvironmentNode;

/*!
 * \brief display window
 */
struct pg_Interpolation
{
  InterpolationType interpolation_mode;
  float offSet;
  float duration;
};

class pg_Window {
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
   */
  // \{
  int                     windowHeightIni;
  int                     windowWidthIni;
  int                     window_x;
  int                     window_y;
 // \}

 public:
  /// glut ID for the window
  int                     glutID;

  /// texture ID in case of monitor window
  GLuint                  texID;

  char                   *id;

  int                     windowNonFullScreenHeight;
  int                     windowNonFullScreenWidth;

  pg_Window ();
  ~pg_Window();

  void                     InitWindowVC( void );

  int                      getWindowHeight( void );
  int                      getWindowWidth( void );
  int                      getWindow_x( void );
  int                      getWindow_y( void );
  void                     setWindowHeight( float val );
  void                     setWindowWidth( float val );
  void                     setWindow_x( float val );
  void                     setWindow_y( float val );
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
  void setParameters( pg_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  void operator=(pg_Window& w);
};

/*!
 * \brief main environment class
 */
class pg_Environment {
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
  pg_Window              *PG_Window;
  /// working variable to keep track of displayed windows
  bool                    windowDisplayed;  
  bool                    double_window;
  // \}

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
#ifdef PG_HAVE_FFMPEG
  pg_AudioVideoOutData    *audioVideoOutData;
#endif
 // \}

  /*! \name Group frame_rate
   *  \brief frame_rate speed adjustments to hardware
   */
  // \{
  float                    minimal_interframe_latency;
  float                    time_scale;
  // \}

  /*! \name Group scene_schedule
   *  \brief scene schedule
   */
  // \{
  float                    initial_time;
  // \}

  /// the name of log file
  char                    *log_file_name;
  bool                     log_date_output;

  /// audio parameters: input_volume
  float                    input_volume;

  /// the name of font file
  char                    *font_file_name;
  char                    *font_file_encoding;
  TextureEncoding          font_texture_encoding;
  int                      font_size;
  int                      message_pixel_length;

  /*! \name Group udp_hosts_clients
   *  \brief udp hosts and clients
   */
  // \{
  pg_IPServer            **IP_Servers;
  int                      nb_IP_Servers;

  pg_IPClient            **IP_Clients;
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
  // \}

  /*! \name Group max_values
   *  \brief max values
   */
  // \{
  int                      Nb_max_mouse_recording_frames;
  int                      Max_network_message_length;
  // \}

  /*! \name Group cursor
   *  \brief cursor
   */
  // \{
  int                      cursorSize;
  Cursor_Shape             cursorShape;
  // \}

  int                      nb_scenes;
  float                   *scene_durations;
  float                   *scene_originalDurations;
  float                   *scene_initial_times;
  float                   *scene_originalInitial_times;
  float                   *scene_final_times;
  float                   *scene_originalFinal_times;
  char                   **scene_IDs;
  float                  **scene_initial_parameters;
  float                  **scene_final_parameters;
  pg_Interpolation       **scene_interpolations;

  /// allocates environment and sets default values
  pg_Environment();
  ~pg_Environment();

  void initWindows( void );

  void print( FILE *file , int isLong , int depth );
  void display( DrawingMode mode , bool *invalidDL );
  void operator=(pg_Environment& e);
  void parseWindow( int *fileDepth , int *p_c ,
		    pg_XMLTag *XMLtag );
  void parse( int *fileDepth , int *p_c );
  void setParameters( pg_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		      IntBinOp operI );
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize );
  void setApplicationParameters( pg_XMLTag *XMLtag , bool atRunTime ,
				 FloatBinOp oper ,
				 IntBinOp operI );

  void saveInitialTimesAndDurations( void );
  void restoreInitialTimesAndDurations( void );

  
/*! 
 * \brief environment modifier variables modification
 * \param activeShift						to be added
 * \param activeAlt						to be added
 * \param activeControl				to be added
 */
 void pg_process_modifiers( int activeShift , int activeAlt , 
			     int activeControl );
};

/*! \name Group miscellaneous
 *  \biref miscellaneous functions
 */
// \{
void indent( FILE *file , int depth );

void defaultValues( void );
void LoadConfigurationFile( const char * fileName );

#endif
