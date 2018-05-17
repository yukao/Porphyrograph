/*! \file dm-conf.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-conf.cpp
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
#include "dm-all_include.h"

/*! \name Group constants
 *  \brief Enumerated constants
 */
// \{
const char *Cursor_ShapeString[EmptyCursorShape + 1] = { "no_cursor" , "left_arrow" , "right_arrow" , "EmptyCursorShape" };

const char *ExternalMessageFormatString[EmptyExternalMessageFormat + 1] = { "Plain" , "OSC" , "EmptyExternalMessageFormat" };

// constant strings for glut window types
const char *WindowRatioString[EmptyWindowRatio + 1] = { "no_ratio" , "window_ratio" , "frustum_ratio" , "ortho_ratio" , "view_volume_ratio" , "emptywindowratio" };
// \}

/////////////////////////////////////////////////////
// global variable for screenshot file name
/////////////////////////////////////////////////////
char currentFilename[512];
char description[512];

/////////////////////////////////////////////////////
// Default values for global variables
/////////////////////////////////////////////////////


dm_Window::dm_Window( void ) {
  id = NULL;
  user_id = NULL;
  InitWindowVC();
}

dm_Window::~dm_Window(void) {
  if (id) {
    delete [] id;
    id = NULL;
  }

  if (user_id) {
    delete [] user_id;
    user_id = NULL;
  }

}

void dm_Window::InitWindowVC( void ) {
  ratio = EmptyWindowRatio;
  glutID = -1;
  texID = 0;
  InitString( &id );
  InitString( &user_id );
  userRank = -1;
  windowHeight = HEIGHT;
  windowWidth = WIDTH;
  windowNonFullScreenHeight = HEIGHT;
  windowNonFullScreenWidth = WIDTH;
  windowHeightIni = -1;
  windowWidthIni = -1;
  window_x = 0;
  window_y = 0;
}

int dm_Window::getWindowHeight( void ){
  return windowHeight;
}
int dm_Window::getWindowWidth( void ){
  return windowWidth;
}
int dm_Window::getWindow_x( void ){
  return window_x;
}
int dm_Window::getWindow_y( void ){
  return window_y;
}
void dm_Window::setWindowHeight( float val ){
  windowHeight = (int)val;
  // printf( "windowHeight win %s %d\n" , id , windowHeight );
  if( windowHeightIni < 0 ) {
    windowHeightIni = windowHeight;
  }
}
void dm_Window::setWindowWidth( float val ){
  windowWidth = (int)val;
  // printf( "windowWidth win %s %d\n" , id , windowWidth );
  if( windowWidthIni < 0 ) {
    windowWidthIni = windowWidth;
  }
}
void dm_Window::setWindow_x( float val ){
  window_x = (int)val;
  // printf( "setWindow id %s window_x %d\n" , id , window_x );
}

void dm_Window::setWindow_y( float val ){
  window_y = (int)val;
}
void dm_Window::manageRatio( dm_Frustum *currentFrustum , 
			       dm_Ortho *currentOrtho ){
  //printf( "window ratio win %s %d\n" , id , ratio );
  if( ratio == ViewVolumeWindowRatio ) {
    if( currentFrustum ) {
      float frustumWidth = currentFrustum->xMax - currentFrustum->xMin;
      float frustumHeight = currentFrustum->yMax - currentFrustum->yMin;
      // printf( "window before ratio management win %s %d %d frustum %.2f %.2f\n" , id , windowWidth , windowHeight , frustumWidth , frustumHeight );
      if( windowWidth && windowHeight ) {
	float ratioW = (float)frustumWidth/(float)windowWidth;
	float ratioH = (float)frustumHeight/(float)windowHeight;
	// frustum height is closer to window height 
	if( ratioW > ratioH ) {
	  windowHeight = (int)(windowWidth 
			       * ((float)frustumHeight / (float)frustumWidth));
	}
	else {
	  windowWidth = (int)(windowHeight 
			      * ((float)frustumWidth / (float)frustumHeight));
	}
	// printf( "window after ratio management win %s %d %d ratios %.6f %.6f frustum %.2f %.2f\n" , id , windowWidth , windowHeight , ratioW , ratioH , frustumWidth , frustumHeight);
      }
    }
    else if( currentOrtho ) {
      //printf( "window before ratio management win %s %d %d\n" , id , windowWidth , windowHeight );
      float orthoWidth = currentOrtho->xMax - currentOrtho->xMin;
      float orthoHeight = currentOrtho->yMax - currentOrtho->yMin;
      if( windowWidth && windowHeight ) {
	float ratioW = (float)orthoWidth/(float)windowWidth;
	float ratioH = (float)orthoHeight/(float)windowHeight;
	// ortho height is closer to window height 
	if( ratioW > ratioH ) {
	  windowHeight = (int)(windowWidth 
			       * ((float)orthoHeight / (float)orthoWidth));
	}
	else {
	  windowWidth = (int)(windowHeight 
			      * ((float)orthoWidth / (float)orthoHeight));
	}
      }
    }
  }
  else if( ratio == WindowWindowRatio ) {
    //printf( "window before ratio management win %s %d %d\n" , id , windowWidth , windowHeight );
    if( windowWidth && windowHeight ) {
      float ratioW = (float)windowWidthIni/(float)windowWidth;
      float ratioH = (float)windowHeightIni/(float)windowHeight;
      // winIni height is closer to window height 
      if( ratioW > ratioH ) {
	windowHeight = (int)(windowWidth 
			     * ((float)windowHeightIni / (float)windowWidthIni));
      }
      else {
	windowWidth = (int)(windowHeight 
			    * ((float)windowWidthIni / (float)windowHeightIni));
      }
    }
    //printf( "window after ratio management win %s %d %d\n" , id , windowWidth , windowHeight );
  }
//   printf( "windo wWidth /Height win %s %d %d\n" , id , windowWidth , windowHeight );
}
void dm_Window::position( float x , float y ) {
  setWindow_x( x );
  setWindow_y( y );
}
void dm_Window::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<window id=\"%s\" width=\"%d\" height=\"%d\" x=\"%d\" y=\"%d\" user_id=\"%s\" ratio=\"%s\" >\n" , id , windowWidth , windowHeight , window_x , window_y , user_id , WindowRatioString[ ratio ] );
  indent( file , depth );
  fprintf( file , "</window>\n" );
}

void dm_Window::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			       FloatBinOp oper ,
			       IntBinOp operI ) {
  // window id
  if( !atRunTime ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &id ,
	      XMLtag->getAttributeStringValue( "id" ) );
    }
  }

  // size / location
  if( XMLtag->hasAttribute( "height" ) ) {
    setWindowHeight( 
      (float)XMLtag->getAttributeDoubleValue( "height" , atRunTime ) );
  }
  if( XMLtag->hasAttribute( "width" ) ) {
    setWindowWidth( 
      (float)XMLtag->getAttributeDoubleValue( "width" , atRunTime ) );
  }
  if( XMLtag->hasAttribute( "x" ) ) {
    setWindow_x( 
      (float)XMLtag->getAttributeDoubleValue( "x" , atRunTime ) );
  }
  if( XMLtag->hasAttribute( "y" ) ) {
    setWindow_y(
      (float)XMLtag->getAttributeDoubleValue( "y" , atRunTime ) );
  }

  // user_id: the user associated with this window for rendering
  if( XMLtag->hasAttribute( "user_id" ) ) {
    if( !atRunTime ) {
      StrCpy( &user_id ,
	      XMLtag->getAttributeStringValue( "user_id" ) );
    }
    else {
      // printf( "User ID modification: user %s for win %s\n" , XMLtag->getAttributeStringValue( "user_id" ) , id );

      // retrieves user node from user ID
      dm_SceneNode * associatedUser 
	= DM_CompositionNode->FindUserReference( XMLtag->getAttributeStringValue( "user_id" ) );
      
      if( associatedUser ) {
	// printf("    targeted Nodes %s\n" , associatedUser->id );
	userRank = -1;
	for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	     ind++ ) {
	  if( DM_CompositionNode->tabUsers[ ind ] == associatedUser ) {
	    userRank = ind;
	    break;
	  }
	}
      }
    }
    
  }

  // controlling W/H ratio
  ratio = NoWindowRatio; 
  if( XMLtag->hasAttribute( "ratio" ) ) {
    char *theratioString = XMLtag->getAttributeStringValue( "ratio" );
    WindowRatio theRatio = EmptyWindowRatio;
    for( int ind = 0 ; ind < EmptyWindowRatio ; ind++ ) {
      if( strcmp( theratioString , WindowRatioString[ind] ) == 0 ) {
	theRatio = (WindowRatio)ind;
	break;
      }
    }
    if( theRatio == EmptyWindowRatio ) {
      sprintf( ErrorStr , "Error: unknown window ratio [%s] in configuration file!" , theratioString ); ReportError( ErrorStr ); throw 330;
    }
    ratio = theRatio;
  }

}

void dm_Window::operator=(dm_Window& w) {
  StrCpy( &id , w.id );
  StrCpy( &user_id , w.user_id );
  ratio = w.ratio;
  glutID = w.glutID;
  texID = w.texID;
  userRank = w.userRank;
  // do not need to be reallocated 
  // are just internal pointers
  windowHeight = w.windowHeight;
  windowWidth = w.windowWidth;
  windowNonFullScreenHeight = w.windowNonFullScreenHeight;
  windowNonFullScreenWidth = w.windowNonFullScreenWidth;
  window_x = w.window_x;
  window_y = w.window_y;
}

dm_Environment::dm_Environment( void ): dm_NonGeometricalObject() {
  // --------- composition_output_file ----------- //
  composition_output_file_name = NULL;
  StrCpy( &composition_output_file_name , "worlds-out.xml" );
  default_print_file_name = NULL;
  StrCpy( &default_print_file_name , "DM-out.xml" );

  // --------- windows ----------- //
  DM_Window = NULL;

  // --------- default_user ----------- //
  // will be copied into DM_CompositionNode->tabUsers[ 0 ];
  default_user = new dm_User();

  // --------- png_rendering_file ----------- //
  Png_file_name = NULL;
  Png_shot_dir_name = NULL;

  InitString( &Png_file_name );
  InitString( &Png_shot_dir_name );
  beginPng = 0;
  endPng = 10;
  stepPng = 1;
  outputPng = false;

  // --------- svg_rendering_file ----------- //
  Svg_file_name = NULL;
  Svg_shot_dir_name = NULL;

  InitString( &Svg_file_name );
  InitString( &Svg_shot_dir_name );
  beginSvg = 0;
  endSvg = 10;
  stepSvg = 1;
  outputSvg = false;

  // --------- jpg_rendering_file ----------- //
  Jpg_file_name = NULL;
  Jpg_shot_dir_name = NULL;

  InitString( &Jpg_file_name );
  InitString( &Jpg_shot_dir_name );
  beginJpg = 0;
  endJpg = 10;
  stepJpg = 1;
  outputJpg = false;

  // --------- video_rendering_file ----------- //
  Video_file_name = NULL;
  InitString( &Video_file_name );
  beginVideo_file = 0;
  endVideo_file = 10;
  stepVideo_file = 1;
  outputVideo_file = false;
#ifdef DM_HAVE_FFMPEG
  audioVideoOutData = NULL;
#endif


  // --------- frame_rate ----------- //
  minimal_interframe_latency = 40;
  vsync = false;
  InterpolationRefreshingVariation = 0.01F;

  // --------- log_file ----------- //
  log_file_name = NULL;
  log_date_output = false;
  StrCpy( &log_file_name , "DM-main.log" );

  // --------- font_file ----------- //
  font_file_name = NULL;
 // 2018 MODIFICATION CJ
#ifdef dcONT
  StrCpy(&font_file_name, "./usascii/arial/stb_font_arial_18_usascii.png");
#endif
#ifdef CityWalk
  StrCpy(&font_file_name, "./ProjectsBefore18/dcONT/textures/usascii/arial/stb_font_arial_18_usascii.png");
#endif
  font_file_encoding = NULL;
  StrCpy( &font_file_encoding , "png" );
  font_texture_encoding = PNG;
  font_size = 12;
  font_tex_no = -1;
  message_tex_no = -1;
  message_pixel_length = 128;

  // --------- ip hosts ----------- //
  IP_Servers = NULL;
  nb_IP_Servers = 0;

  IP_Clients = NULL;
  nb_IP_Clients = 0;

  // --------- scene_schedule ----------- //
  initial_time = 0.0;
  time_step = 0.0;
  real_time = false;

  // configuration file scalar variables

  // current mouse location (also used for displaying the cursor)
  mouse_x_scalar =  new dm_Scalar();
  mouse_x_scalar->SetId( "mouse_x" );
  mouse_x_scalar->SetType( DataTypeDouble );
  mouse_x_scalar->SetValue( 0.0f );

  mouse_y_scalar =  new dm_Scalar();
  mouse_y_scalar->SetId( "mouse_y" );
  mouse_y_scalar->SetType( DataTypeDouble );
  mouse_y_scalar->SetValue( 0.0f );
  mouse_x_scalar->nextVariable = mouse_y_scalar;

  mouse_x_prev_scalar =  new dm_Scalar();
  mouse_x_prev_scalar->SetId( "mouse_x_prev" );
  mouse_x_prev_scalar->SetType( DataTypeDouble );
  mouse_x_prev_scalar->SetValue( 0.0f );
  mouse_y_scalar->nextVariable = mouse_x_prev_scalar;

  mouse_y_prev_scalar =  new dm_Scalar();
  mouse_y_prev_scalar->SetId( "mouse_y_prev" );
  mouse_y_prev_scalar->SetType( DataTypeDouble );
  mouse_y_prev_scalar->SetValue( 0.0f );
  mouse_x_prev_scalar->nextVariable = mouse_y_prev_scalar;

  mouse_x_deviation_scalar =  new dm_Scalar();
  mouse_x_deviation_scalar->SetId( "mouse_x_deviation" );
  mouse_x_deviation_scalar->SetType( DataTypeDouble );
  mouse_x_deviation_scalar->SetValue( 0.0f );
  mouse_y_prev_scalar->nextVariable = mouse_x_deviation_scalar;

  mouse_y_deviation_scalar =  new dm_Scalar();
  mouse_y_deviation_scalar->SetId( "mouse_y_deviation" );
  mouse_y_deviation_scalar->SetType( DataTypeDouble );
  mouse_y_deviation_scalar->SetValue( 0.0f );
  mouse_x_deviation_scalar->nextVariable = mouse_y_deviation_scalar;

  pixel_color_r_scalar =  new dm_Scalar();
  pixel_color_r_scalar->SetId( "pixel_color_r" );
  pixel_color_r_scalar->SetType( DataTypeDouble );
  pixel_color_r_scalar->SetValue( 0.0f );
  mouse_y_deviation_scalar->nextVariable = pixel_color_r_scalar;

  pixel_color_g_scalar =  new dm_Scalar();
  pixel_color_g_scalar->SetId( "pixel_color_g" );
  pixel_color_g_scalar->SetType( DataTypeDouble );
  pixel_color_g_scalar->SetValue( 0.0f );
  pixel_color_r_scalar->nextVariable = pixel_color_g_scalar;

  pixel_color_b_scalar =  new dm_Scalar();
  pixel_color_b_scalar->SetId( "pixel_color_b" );
  pixel_color_b_scalar->SetType( DataTypeDouble );
  pixel_color_b_scalar->SetValue( 0.0f );
  pixel_color_g_scalar->nextVariable = pixel_color_b_scalar;

  window_width_scalar =  new dm_Scalar();
  window_width_scalar->SetId( "window_width" );
  window_width_scalar->SetType( DataTypeDouble );
  window_width_scalar->SetValue( 0.0f );
  pixel_color_g_scalar->nextVariable = window_width_scalar;

  window_height_scalar =  new dm_Scalar();
  window_height_scalar->SetId( "window_height" );
  window_height_scalar->SetType( DataTypeDouble );
  window_height_scalar->SetValue( 0.0f );
  window_width_scalar->nextVariable = window_height_scalar;

  current_frame_scalar =  new dm_Scalar();
  current_frame_scalar->SetId( "current_frame" );
  current_frame_scalar->SetType( DataTypeInt );
  current_frame_scalar->SetValue( 0.0f );
  window_height_scalar->nextVariable = current_frame_scalar;

  // configuration file table variables
  frame_color_r_table =  new dm_Table(1);
  frame_color_r_table->SetId( "frame_color_r" );
  frame_color_r_table->SetValue( 0.0f , 0 );

  frame_color_g_table =  new dm_Table(1);
  frame_color_g_table->SetId( "frame_color_g" );
  frame_color_g_table->SetValue( 0.0f , 0 );
  frame_color_r_table->nextVariable = frame_color_g_table;

  frame_color_b_table =  new dm_Table(1);
  frame_color_b_table->SetId( "frame_color_b" );
  frame_color_b_table->SetValue( 0.0f , 0 );
  frame_color_g_table->nextVariable = frame_color_b_table;

  // --------- cursor ----------- //  _size = 10;

  cursor_shape = NoCursor;

  // ---------- interpolation ----------- //
  slerp_shortest_path = false;

  // --------- texture_display ----------- //
  texture_displayed = true;

  shader_programs = NULL;
  InitString( &shader_programs );

  // --------- trace ----------- //
  trace_input_file = false;
  trace_output_frame_number = false;
  trace_input_events = false;
  trace_time = false;
  first_frame_number = 0;
  last_frame_number = INT_MAX;

  // --------- display ----------- //
  full_screen = true;
  game_mode = false;
  echo = 0.0;

  // --------- parameters ----------- //
  NbParameters = 0;
  ParameterValues = NULL;
  ParameterStrings = NULL;

  // --------- max_values ----------- //
  Nb_max_textures = 50;
  Nb_max_shaders = 50;
  Nb_max_display_lists = 100000;
  Nb_max_displays = 8;
  Nb_max_path_components = 1000;
  Nb_max_targets = 1000;
  Nb_max_embedded_parameters = 32;
  Nb_max_scenes = 100;
  Nb_max_model_view_matrix = 10000;
  Nb_max_tracks = 10;
  Nb_max_mouse_recording_frames = 1000;
  Max_network_message_length = 65535;
}

dm_Environment::~dm_Environment( void ) {	
  if (composition_output_file_name) {
    delete [] composition_output_file_name;
    composition_output_file_name = NULL;
  }

  if (default_print_file_name) {
    delete [] default_print_file_name;
    default_print_file_name = NULL;
  }

  if (DM_Window)  {
    delete DM_Window;
    DM_Window = NULL;
  }

  if (Svg_file_name) {
    delete [] Svg_file_name;
    Svg_file_name = NULL;
  }

  if (Svg_shot_dir_name) {
    delete [] Svg_shot_dir_name;
    Svg_shot_dir_name = NULL;
  }

  if (Png_file_name) {
    delete [] Png_file_name;
    Png_file_name = NULL;
  }

  if (Png_shot_dir_name) {
    delete [] Png_shot_dir_name;
    Png_shot_dir_name = NULL;
  }

  if (Jpg_file_name) {
    delete [] Jpg_file_name;
    Jpg_file_name = NULL;
  }

  if (Jpg_shot_dir_name) {
    delete [] Jpg_shot_dir_name;
    Jpg_shot_dir_name = NULL;
  }

  if (Video_file_name) {
    delete [] Video_file_name;
    Video_file_name = NULL;
  }

  if (log_file_name) {
    delete [] log_file_name;
    log_file_name = NULL;
  }

  if (font_file_name) {
    delete [] font_file_name;
    font_file_name = NULL;
  }

  if (font_file_encoding) {
    delete [] font_file_encoding;
    font_file_encoding = NULL;
  }

  if (IP_Servers)  {
    for (int ind = 0; ind < nb_IP_Servers; ind++) {
      delete IP_Servers[ind];
      IP_Servers[ind] = NULL;
    }
    
    delete [] IP_Servers;
    IP_Servers = NULL;
    nb_IP_Servers = 0;
  }
  
  if (IP_Clients)  {
    for (int ind = 0; ind < nb_IP_Clients; ind++) {
      delete IP_Clients[ind];
      IP_Clients[ind] = NULL;
    }
    
    delete [] IP_Clients;
    IP_Clients = NULL;
    nb_IP_Clients = 0;
  }

  if (shader_programs ) {
    delete [] shader_programs;
    shader_programs = NULL;
  }

  if (ParameterStrings) {
    delete [] ParameterStrings;
    ParameterStrings = NULL;
  }

  if (ParameterValues) {
    delete [] ParameterValues;
    ParameterValues = NULL;
  }
}

void dm_Environment::initWindows( void ) {
  DM_Window = new dm_Window();
  windowDisplayed = false;
}

void dm_Environment::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<environment id=\"%s\">\n" , id );
  indent( file , depth + 1 );
  fprintf( file , "<composition_output_file id=\"%s\" default_print_file=\"%s\"/>\n" , composition_output_file_name , default_print_file_name );

  indent( file , depth + 1 );
  fprintf( file , "<windows>\n" );
  DM_Window->print( file , isLong , depth + 2 );
  indent( file , depth + 1 );
  fprintf( file , "</windows>\n" );

  indent( file , depth + 1 );
  fprintf( file , "<svg_rendering_file begin=\"%d\" end=\"%d\" step=\"%d\" id=\"%s\" shots=\"%s\"/>\n" , beginSvg , endSvg , stepSvg , Svg_file_name ,  Svg_shot_dir_name );
  indent( file , depth + 1 );
  fprintf( file , "<png_rendering_file begin=\"%d\" end=\"%d\" step=\"%d\" id=\"%s\" shots=\"%s\"/>\n" , beginPng , endPng , stepPng , Png_file_name ,  Png_shot_dir_name );
  indent( file , depth + 1 );
  fprintf( file , "<jpg_rendering_file begin=\"%d\" end=\"%d\" step=\"%d\" id=\"%s\" shots=\"%s\"/>\n" , beginJpg , endJpg , stepJpg , Jpg_file_name ,  Jpg_shot_dir_name );
  indent( file , depth + 1 );
  fprintf( file , "<video_rendering_file begin=\"%d\" end=\"%d\" step=\"%d\" id=\"%s\"/>\n" , beginVideo_file , endVideo_file , stepVideo_file , Video_file_name );
  indent( file , depth + 1 );
  fprintf( file , "<frame_rate minimal_interframe_latency=\"%.5f\" interpolation_refreshing_variation=\"%.5f\" vsync=\"%s\"/>\n" ,  minimal_interframe_latency , InterpolationRefreshingVariation , BooleanString[BooleanToInt(vsync)] );
  indent( file , depth + 1 );
  fprintf( file , "<log_file id=\"%s\" log_date_output=\"%s\"/>\n" ,
	   log_file_name , BooleanString[BooleanToInt(log_date_output)] );
  indent( file , depth + 1 );
  fprintf( file , "<font id=\"%s\" size=\"%d\" length=\"%d\" encoding=\"%s\"/>\n" ,
	   font_file_name , font_size , message_pixel_length , font_file_encoding );
  indent( file , depth + 1 );
  fprintf( file , "<ip_hosts nb_ip_servers=\"%d\" nb_ip_clients=\"%d\">\n" , nb_IP_Servers , nb_IP_Clients );
  for( int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
    IP_Servers[ ind ]->print( file , isLong , depth + 2 );     
  }
  for( int ind = 0 ; ind < nb_IP_Clients ; ind++ ) {
    IP_Clients[ ind ]->print( file , isLong , depth + 2 );     
  }
  indent( file , depth + 1 );
  fprintf( file , "</ip_hosts>\n" );
  indent( file , depth + 1 );
  fprintf( file , "<trace trace_input_file=\"%s\" trace_output_frame_number=\"%s\" trace_input_events=\"%s\" trace_time=\"%s\" first_frame_number=\"%d\" last_frame_number=\"%d\"/>\n" , BooleanString[BooleanToInt(trace_input_file)] , BooleanString[BooleanToInt(trace_output_frame_number)] , BooleanString[BooleanToInt(trace_input_events)] , BooleanString[BooleanToInt(trace_time)] , first_frame_number , last_frame_number );
  indent( file , depth + 1 );
  fprintf( file , "<display full_screen=\"%s\" game_mode=\"%s\" echo=\"%.5f\"/>\n" , BooleanString[BooleanToInt(full_screen)] , BooleanString[BooleanToInt(game_mode)] , echo );
  indent( file , depth + 1 );
  fprintf( file , "<scene_schedule initial_time=\"%.5f\" time_step=\"%.5f\" real_time=\"%s\" />\n" , initial_time , time_step , BooleanString[BooleanToInt(real_time)] );
  indent( file , depth + 1 );
  fprintf( file , "<cursor shape=\"%s\" size=\"%d\"/>\n" , Cursor_ShapeString[cursor_shape] , cursor_size );
  indent( file , depth + 1 );
  fprintf( file , "<interpolation slerp_shortest_path=\"%s\"/>\n" , BooleanString[BooleanToInt(slerp_shortest_path)] );
  fprintf( file , "<texture_display texture_displayed=\"%s\" shader_programs=\"%s\"/>\n" , BooleanString[BooleanToInt(texture_displayed)] , shader_programs );
  indent( file , depth + 1 );
  fprintf( file , "<max_values max_textures=\"%d\" max_shaders=\"%d\" max_display_lists=\"%d\" max_displays=\"%d\" max_path_components=\"%d\" max_targets=\"%d\" max_embedded_parameters=\"%d\" max_scenes=\"%d\" max_model_view_matrices=\"%d\" max_tracks=\"%d\" max_mouse_recording_frames=\"%d\" max_network_message_length=\"%d\"/>\n" , Nb_max_textures , Nb_max_shaders , Nb_max_display_lists , Nb_max_displays , Nb_max_path_components , Nb_max_targets , Nb_max_embedded_parameters , Nb_max_scenes , Nb_max_model_view_matrix , Nb_max_tracks , Nb_max_mouse_recording_frames , Max_network_message_length );
  indent( file , depth );
  fprintf( file , "</environment>\n" );
}

void dm_Environment::display( DrawingMode mode , bool *invalidDL ) {
}

void dm_Environment::operator=(dm_Environment& e) {
  this->dm_NonGeometricalObject::operator=(e);

  // --------- composition_output_file ----------- //
  // composition output after reading initial scenes
  StrCpy( &composition_output_file_name , e.composition_output_file_name );
  // text output file name in the edition mode
  StrCpy( &default_print_file_name , e.default_print_file_name );

  // --------- windows ----------- //
  if( DM_Window ) {
    delete [] DM_Window;
    DM_Window = NULL;
  }

  // contents are copied for safe deallocation
  DM_Window = new dm_Window();
  *(DM_Window) = *(e.DM_Window);
  windowDisplayed = false;

  // --------- svg_rendering_file ----------- //
  // output for screen shots (Svg)
  // output Svg
  StrCpy( &Svg_file_name , e.Svg_file_name );
  StrCpy( &Svg_shot_dir_name , e.Svg_shot_dir_name );
  beginSvg = e.beginSvg;
  endSvg = e.endSvg;
  stepSvg = e.stepSvg;
  outputSvg = e.outputSvg;

  // --------- png_rendering_file ----------- //
  // output for screen shots (Png)
  // output Png
  StrCpy( &Png_file_name , e.Png_file_name );
  StrCpy( &Png_shot_dir_name , e.Png_shot_dir_name );
  beginPng = e.beginPng;
  endPng = e.endPng;
  stepPng = e.stepPng;
  outputPng = e.outputPng;

  // --------- jpg_rendering_file ----------- //
  // output for screen shots (Jpg)
  // output Jpg
  StrCpy( &Jpg_file_name , e.Jpg_file_name );
  StrCpy( &Jpg_shot_dir_name , e.Jpg_shot_dir_name );
  beginJpg = e.beginJpg;
  endJpg = e.endJpg;
  stepJpg = e.stepJpg;
  outputJpg = e.outputJpg;

  // --------- video_rendering_file ----------- //
  // output for screen shots (Video)
  // output Video
  StrCpy( &Video_file_name , e.Video_file_name );
  beginVideo_file = e.beginVideo_file;
  endVideo_file = e.endVideo_file;
  stepVideo_file = e.stepVideo_file;
  outputVideo_file = e.outputVideo_file;
#ifdef DM_HAVE_FFMPEG
  audioVideoOutData = e.audioVideoOutData;
#endif


  // --------- frame_rate ----------- //
  // frame_rate speed adjustments to hardware
  minimal_interframe_latency = e.minimal_interframe_latency;
  vsync = e.vsync;
  InterpolationRefreshingVariation = e.InterpolationRefreshingVariation;

  // --------- log_file ----------- //
  StrCpy( &log_file_name , e.log_file_name );
  log_date_output = e.log_date_output;

  // --------- font ----------- //
  StrCpy( &font_file_name , e.font_file_name );
  StrCpy( &font_file_encoding , e.font_file_encoding );
  font_texture_encoding = e.font_texture_encoding;
  font_size = e.font_size;
  font_tex_no = e.font_tex_no;
  message_tex_no = e.message_tex_no;
  message_pixel_length = e.message_pixel_length;

  // --------- --------- ----------- //
  // --------- ip_hosts ----------- //
  // --------- ip_servers ----------- //
  if( IP_Servers ) {
    for(int ind = 0 ; ind < nb_IP_Servers ; ind++) {
      delete IP_Servers[ind];
      IP_Servers[ind] = NULL;
    }
    delete [] IP_Servers;
    IP_Servers = NULL;
  }
  nb_IP_Servers = e.nb_IP_Servers;
  IP_Servers = new dm_IPServer * [ nb_IP_Servers ];
  // contents are copied for safe deallocation
  for(int ind = 0 ; ind < nb_IP_Servers ; ind++) {
    IP_Servers[ ind ] = new dm_IPServer();
    *(IP_Servers[ ind ]) = *(e.IP_Servers[ ind ]);
  }

  // --------- ip clients ----------- //
  if( IP_Clients ) {
    for(int ind = 0 ; ind < nb_IP_Clients ; ind++) {
      delete IP_Clients[ind];
      IP_Clients[ind] = NULL;
    }
    delete [] IP_Clients;
    IP_Clients = NULL;
  }
  nb_IP_Clients = e.nb_IP_Clients;
  IP_Clients = new dm_IPClient * [ nb_IP_Clients ];
  // contents are copied for safe deallocation
  for(int ind = 0 ; ind < nb_IP_Clients ; ind++) {
    IP_Clients[ ind ] = new dm_IPClient();
    *(IP_Clients[ ind ]) = *(e.IP_Clients[ ind ]);
  }

  // --------- scene_schedule ----------- //
  initial_time = e.initial_time;
  time_step = e.time_step;
  real_time = e.real_time;

  // ----- scalar for modifiers ----- //
  mouse_x_scalar = e.mouse_x_scalar;
  mouse_y_scalar = e.mouse_y_scalar;
  pixel_color_r_scalar = e.pixel_color_r_scalar;
  pixel_color_g_scalar = e.pixel_color_g_scalar;
  pixel_color_b_scalar = e.pixel_color_b_scalar;
  frame_color_r_table = e.frame_color_r_table;
  frame_color_g_table = e.frame_color_g_table;
  frame_color_b_table = e.frame_color_b_table;
  window_width_scalar = e.window_width_scalar;
  window_height_scalar = e.window_height_scalar;
  current_frame_scalar = e.current_frame_scalar;

  tracks_replay_table = e.tracks_replay_table;
  tracks_x_table = e.tracks_x_table;
  tracks_y_table = e.tracks_y_table;
  tracks_x_prev_table = e.tracks_x_table;
  tracks_y_prev_table = e.tracks_y_table;
  tracks_Color_r_table = e.tracks_Color_r_table;
  tracks_Color_g_table = e.tracks_Color_g_table;
  tracks_Color_b_table = e.tracks_Color_b_table;
  tracks_Color_a_table = e.tracks_Color_a_table;
  tracks_BrushID_table = e.tracks_BrushID_table;
  tracks_Radius_table = e.tracks_Radius_table;

  // --------- cursor ----------- //
  cursor_shape = e.cursor_shape;
  cursor_size = e.cursor_size;

  // ---------- interpolation ----------- //
  slerp_shortest_path = e.slerp_shortest_path;

  // --------- texture_display ----------- //
  texture_displayed = e.texture_displayed;
  StrCpy( &shader_programs , e.shader_programs );

  // --------- trace ----------- //
  trace_input_file = e.trace_input_file;
  trace_output_frame_number = e.trace_output_frame_number;
  trace_input_events = e.trace_input_events;
  trace_time = e.trace_time;
  first_frame_number = e.first_frame_number;
  last_frame_number = e.last_frame_number;

  // --------- display ----------- //
  full_screen = e.full_screen;
  game_mode = e.game_mode;
  echo = e.echo;

  // --------- parameters ----------- //
  NbParameters = e.NbParameters;
  ParameterValues = e.ParameterValues;
  ParameterStrings = e.ParameterStrings;

  // --------- max_values ----------- //
  Nb_max_textures = e.Nb_max_textures;
  Nb_max_shaders = e.Nb_max_shaders;
  Nb_max_display_lists = e.Nb_max_display_lists;
  Nb_max_displays = e.Nb_max_displays;
  Nb_max_path_components = e.Nb_max_path_components;
  Nb_max_targets = e.Nb_max_targets;
  Nb_max_embedded_parameters = e.Nb_max_embedded_parameters;
  Nb_max_scenes = e.Nb_max_scenes;
  Nb_max_model_view_matrix = e.Nb_max_model_view_matrix;
  Nb_max_tracks = e.Nb_max_tracks;
  Nb_max_mouse_recording_frames = e.Nb_max_mouse_recording_frames;
  Max_network_message_length = e.Max_network_message_length;
}

/////////////////////////////////////////////////////
// environment parsing from configuration file
/////////////////////////////////////////////////////

void dm_Environment::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				    FloatBinOp oper ,
				    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }
}

bool dm_Environment::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
				    int * tableOrMatrixSize ,
				    dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "echo" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)echo;
    return true;
  }
  else if( strncmp( attribute , "svg_rendering_file" , 
		    strlen( "svg_rendering_file" ) ) == 0
	   && strchr( attribute , ':' ) ) {
    char *sub_attribute = strchr( attribute , ':' ) + 1;
    if( *sub_attribute ) {
      if( strcmp( sub_attribute , "id" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Svg_file_name;
        return true;
      }
      else if( strcmp( sub_attribute , "shots" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Svg_shot_dir_name;
        return true;
      }
      else if( strcmp( sub_attribute , "begin" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)beginSvg;
        return true;
      }
      else if( strcmp( sub_attribute , "end" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)endSvg;
        return true;
      }
      else if( strcmp( sub_attribute , "step" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)stepSvg;
        return true;
      }
    }
  }
  else if( strncmp( attribute , "png_rendering_file" , 
		    strlen( "png_rendering_file" ) ) == 0
	   && strchr( attribute , ':' ) ) {
    char *sub_attribute = strchr( attribute , ':' ) + 1;
    if( *sub_attribute ) {
      if( strcmp( sub_attribute , "id" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Png_file_name;
        return true;
      }
      else if( strcmp( sub_attribute , "shots" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Png_shot_dir_name;
        return true;
      }
      else if( strcmp( sub_attribute , "begin" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)beginPng;
        return true;
      }
      else if( strcmp( sub_attribute , "end" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)endPng;
        return true;
      }
      else if( strcmp( sub_attribute , "step" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)stepPng;
        return true;
      }
    }
  }
  else if( strncmp( attribute , "jpg_rendering_file" , 
		    strlen( "jpg_rendering_file" ) ) == 0
	   && strchr( attribute , ':' ) ) {
    char *sub_attribute = strchr( attribute , ':' ) + 1;
    if( *sub_attribute ) {
      if( strcmp( sub_attribute , "id" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Jpg_file_name;
        return true;
      }
      else if( strcmp( sub_attribute , "shots" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Jpg_shot_dir_name;
        return true;
      }
      else if( strcmp( sub_attribute , "begin" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)beginJpg;
        return true;
      }
      else if( strcmp( sub_attribute , "end" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)endJpg;
        return true;
      }
      else if( strcmp( sub_attribute , "step" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)stepJpg;
        return true;
      }
    }
  }
  else if( strncmp( attribute , "video_rendering_file" , 
		    strlen( "video_rendering_file" ) ) == 0
	   && strchr( attribute , ':' ) ) {
    char *sub_attribute = strchr( attribute , ':' ) + 1;
    if( *sub_attribute ) {
      if( strcmp( sub_attribute , "id" ) == 0 ) {
	*dataType = DataTypeString;
	*valString = Video_file_name;
        return true;
      }
      else if( strcmp( sub_attribute , "begin" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)beginVideo_file;
        return true;
      }
      else if( strcmp( sub_attribute , "end" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)endVideo_file;
        return true;
      }
      else if( strcmp( sub_attribute , "step" ) == 0 ) {
	*dataType = DataTypeDouble;
	*valDouble = (double)stepVideo_file;
        return true;
      }
    }
  }
  return false;
}

void dm_Environment::setApplicationParameters( dm_XMLTag *XMLtag , 
					       bool atRunTime ,
					       FloatBinOp oper ,
					       IntBinOp operI ) {
  // composition output file
  // text output file name in the edition mode
  if( strcmp( XMLtag->tag , "composition_output_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &composition_output_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
    }

    if( XMLtag->hasAttribute( "default_print_file" ) ) {
      StrCpy( &default_print_file_name ,
	       XMLtag->getAttributeStringValue( "default_print_file" ) );
    }
  }
   
  // number of windows and parameters
  else if( strcmp( XMLtag->tag , "windows" ) == 0 && !atRunTime ) {
    initWindows();
  }
  // windows node content is parsed in calling function dm_Environment::parse
   
  // dynamic single window layout
  else if( strcmp( XMLtag->tag , "window" ) == 0 && atRunTime ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      char* idWin = XMLtag->getAttributeStringValue( "id" );
    
      if( strcmp( DM_EnvironmentNode->DM_Window->id , 
		  idWin ) == 0 ) {
	DM_EnvironmentNode->DM_Window->setParameters( XMLtag ,
						      atRunTime ,
						      oper ,
						      operI );
      }
    }
    else {
      sprintf( ErrorStr , "Error: run time modification of window parameter must provide window \"id\" attribute value!" ); ReportError( ErrorStr ); 
    }
  }
   
  // screen shots Svg
  else if( strcmp( XMLtag->tag , "svg_rendering_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &Svg_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
      if( (*(Svg_file_name)) ) {
	outputSvg = true;
      }
    }
    if( XMLtag->hasAttribute( "shots" ) ) {
      StrCpy( &Svg_shot_dir_name ,
	       XMLtag->getAttributeStringValue( "shots" ) );
    }
    if( XMLtag->hasAttribute( "begin" ) ) {
      beginSvg = XMLtag->getAttributeIntValue( "begin" , atRunTime );
    }
    if( XMLtag->hasAttribute( "end" ) ) {
      endSvg = XMLtag->getAttributeIntValue( "end" , atRunTime );
    }
    if( XMLtag->hasAttribute( "step" ) ) {
      stepSvg = XMLtag->getAttributeIntValue( "step" , atRunTime );
    }
  }

  // screen shots Png
  else if( strcmp( XMLtag->tag , "png_rendering_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &Png_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
      if( (*(Png_file_name)) ) {
	outputPng = true;
      }
    }
    if( XMLtag->hasAttribute( "shots" ) ) {
      StrCpy( &Png_shot_dir_name ,
	       XMLtag->getAttributeStringValue( "shots" ) );
    }
    if( XMLtag->hasAttribute( "begin" ) ) {
      beginPng = XMLtag->getAttributeIntValue( "begin" , atRunTime );
    }
    if( XMLtag->hasAttribute( "end" ) ) {
      endPng = XMLtag->getAttributeIntValue( "end" , atRunTime );
    }
    if( XMLtag->hasAttribute( "step" ) ) {
      stepPng = XMLtag->getAttributeIntValue( "step" , atRunTime );
    }
  }

  // screen shots Jpg
  else if( strcmp( XMLtag->tag , "jpg_rendering_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &Jpg_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
      if( (*(Jpg_file_name)) ) {
	outputJpg = true;
      }
    }
    if( XMLtag->hasAttribute( "shots" ) ) {
      StrCpy( &Jpg_shot_dir_name ,
	       XMLtag->getAttributeStringValue( "shots" ) );
    }
    if( XMLtag->hasAttribute( "begin" ) ) {
      beginJpg = XMLtag->getAttributeIntValue( "begin" , atRunTime );
    }
    if( XMLtag->hasAttribute( "end" ) ) {
      endJpg = XMLtag->getAttributeIntValue( "end" , atRunTime );
    }
    if( XMLtag->hasAttribute( "step" ) ) {
      stepJpg = XMLtag->getAttributeIntValue( "step" , atRunTime );
    }
  }

  // screen shots Video
  else if( strcmp( XMLtag->tag , "video_rendering_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &Video_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
      if( (*(Video_file_name)) ) {
	outputVideo_file = true;
      }
    }
    if( XMLtag->hasAttribute( "begin" ) ) {
      beginVideo_file = XMLtag->getAttributeIntValue( "begin" , atRunTime );
    }
    if( XMLtag->hasAttribute( "end" ) ) {
      endVideo_file = XMLtag->getAttributeIntValue( "end" , atRunTime );
    }
    if( XMLtag->hasAttribute( "step" ) ) {
      stepVideo_file = XMLtag->getAttributeIntValue( "step" , atRunTime );
    }
  }

  // frame_rate speed adjustments
  else if( strcmp( XMLtag->tag , "frame_rate" ) == 0 ) {
    if( XMLtag->hasAttribute( "minimal_interframe_latency" ) ) {
      minimal_interframe_latency 
	= (float)XMLtag->getAttributeDoubleValue( "minimal_interframe_latency" , 
					  atRunTime );
#ifdef _WIN32
	  if( minimal_interframe_latency < 1.0 ) {
			minimal_interframe_latency = 1.0;
	  }
#endif
    }
    if( XMLtag->hasAttribute( "vsync" ) ) {
      XMLtag->BooleanFieldValue( "vsync" ,
				 &vsync );
    }
    if( XMLtag->hasAttribute( "interpolation_refreshing_variation" ) ) {
      InterpolationRefreshingVariation 
	= (float)XMLtag->getAttributeDoubleValue( "interpolation_refreshing_variation" , 
					  atRunTime );
    }
  }
 
  // log file name
  else if( strcmp( XMLtag->tag , "log_file" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &log_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
    }
    if( XMLtag->hasAttribute( "log_date_output" ) ) {
      XMLtag->BooleanFieldValue( "log_date_output" ,
				 &log_date_output );
    }
  }
 
  // font name
  else if( strcmp( XMLtag->tag , "font" ) == 0 ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      StrCpy( &font_file_name ,
	       XMLtag->getAttributeStringValue( "id" ) );
    }
    if( XMLtag->hasAttribute( "encoding" ) ) {
      StrCpy( &font_file_encoding ,
	       XMLtag->getAttributeStringValue( "encoding" ) );
      font_texture_encoding = EmptyTextureEncoding;
      for( int ind = 0 ; ind < EmptyTextureEncoding ; ind++ ) {
	//printf( "encoding [%s]\n" , TextureEncodingString[ind] );
	if( strcmp( font_file_encoding , TextureEncodingString[ind] ) == 0 ) {
	  font_texture_encoding = (TextureEncoding)ind;
	  break;
	}
      }
      if( font_texture_encoding == EmptyTextureEncoding ) {
	sprintf( ErrorStr , "Error: unknown image encoding [%s]!" , font_file_encoding ); ReportError( ErrorStr ); throw 10;
	font_texture_encoding = EmptyTextureEncoding;
      }
    }
    if( XMLtag->hasAttribute( "size" ) ) {
      font_size
	= XMLtag->getAttributeIntValue( "size" , atRunTime );
    }
    if( XMLtag->hasAttribute( "length" ) ) {
      message_pixel_length
	= XMLtag->getAttributeIntValue( "length" , atRunTime );
    }
  }
 
  // udp_hosts
  else if( strcmp( XMLtag->tag , "udp_hosts" ) == 0 
	   || strcmp( XMLtag->tag , "ip_hosts" ) == 0 ) {
    if( XMLtag->hasAttribute( "nb_udp_local_servers" ) ) {
      nb_IP_Servers
	= XMLtag->getAttributeIntValue( "nb_udp_local_servers" , atRunTime );
    }
    else if( XMLtag->hasAttribute( "nb_ip_servers" ) ) {
      nb_IP_Servers
	= XMLtag->getAttributeIntValue( "nb_ip_servers" , atRunTime );
    }
    else 
      nb_IP_Servers = 1;

    if( XMLtag->hasAttribute( "nb_udp_remote_clients" ) ) {
      nb_IP_Clients
	= XMLtag->getAttributeIntValue( "nb_udp_remote_clients" , atRunTime );
    }
    else if( XMLtag->hasAttribute( "nb_ip_clients" ) ) {
      nb_IP_Clients
	= XMLtag->getAttributeIntValue( "nb_ip_clients" , atRunTime );
    }
    else 
      nb_IP_Clients = 1;
  }
  // udp_hosts node content is parsed in calling function dm_Environment::parse

  // schedule
  else if( strcmp( XMLtag->tag , "scene_schedule" ) == 0 ) {
    if( XMLtag->hasAttribute( "initial_time" ) ) {
      initial_time = (float)XMLtag->getAttributeDoubleValue( "initial_time" , 
					   atRunTime );
    }
    if( XMLtag->hasAttribute( "time_step" ) ) {
      time_step = (float)XMLtag->getAttributeDoubleValue( "time_step" , 
					   atRunTime );
    }
    if( XMLtag->hasAttribute( "real_time" ) ) {
      XMLtag->BooleanFieldValue( "real_time" , &real_time );
    }
  }

  // cursor
  else if( strcmp( XMLtag->tag , "cursor" ) == 0 ) {
    if( XMLtag->hasAttribute( "shape" ) ) {
      char *cursor_str = XMLtag->getAttributeStringValue( "shape" );
      cursor_shape = EmptyCursorShape;
      for( int ind = 0 ; ind < EmptyCursorShape ; ind++ ) {
	if( strcmp( cursor_str , Cursor_ShapeString[ind] ) == 0 ) {
	  cursor_shape = (Cursor_Shape)ind;
	  break;
	}
      }
      if( cursor_shape == EmptyCursorShape ) {
	sprintf( ErrorStr , "Error: unknown cursor shape  [%s] in configuration file!" , cursor_str ); ReportError( ErrorStr ); throw 345;
      }
    }
    if( XMLtag->hasAttribute( "size" ) ) {
      cursor_size = XMLtag->getAttributeIntValue( "size" ,
				atRunTime );
    }
  }

  // ---------- interpolation ----------- //
  else if( strcmp( XMLtag->tag , "interpolation" ) == 0 ) {
    if( XMLtag->hasAttribute( "slerp_shortest_path" ) ) {
      XMLtag->BooleanFieldValue( "slerp_shortest_path" ,
			  &slerp_shortest_path );
    }
  }

  // texture display
  else if( strcmp( XMLtag->tag , "texture_display" ) == 0 ) {
    if( XMLtag->hasAttribute( "texture_displayed" ) ) {
      XMLtag->BooleanFieldValue( "texture_displayed" ,
			  &texture_displayed );
    }
    if( XMLtag->hasAttribute( "shader_programs" ) ) {
      StrCpy( &shader_programs ,
	       XMLtag->getAttributeStringValue( "shader_programs" ) );
    }
  }

  // output trace
  else if( strcmp( XMLtag->tag , "trace" ) == 0 ) {
    if( XMLtag->hasAttribute( "trace_input_file" ) ) {
      XMLtag->BooleanFieldValue( "trace_input_file" ,
			  &trace_input_file );
    }
    if( XMLtag->hasAttribute( "trace_output_frame_number" ) ) {
      XMLtag->BooleanFieldValue( "trace_output_frame_number" ,
			  &trace_output_frame_number );
    }
    if( XMLtag->hasAttribute( "trace_input_events" ) ) {
      XMLtag->BooleanFieldValue( "trace_input_events" ,
			  &trace_input_events );
    }
    if( XMLtag->hasAttribute( "trace_time" ) ) {
      XMLtag->BooleanFieldValue( "trace_time" ,
			  &trace_time );
    }
    if( XMLtag->hasAttribute( "first_frame_number" ) ) {
      first_frame_number 
	= XMLtag->getAttributeIntValue( "first_frame_number" , 
				atRunTime );
    }
    if( XMLtag->hasAttribute( "last_frame_number" ) ) {
      last_frame_number 
	= XMLtag->getAttributeIntValue( "last_frame_number" , 
				atRunTime );
    }
  }

  // screen display
  else if( strcmp( XMLtag->tag , "display" ) == 0 ) {

    if( XMLtag->hasAttribute( "full_screen" ) ) {
      XMLtag->BooleanFieldValue( "full_screen" ,
			  &full_screen );
    }

    if( XMLtag->hasAttribute( "game_mode" ) ) {
      XMLtag->BooleanFieldValue( "game_mode" ,
			  &game_mode );
    }

    if( XMLtag->hasAttribute( "echo" ) ) {
      echo = (float)XMLtag->getAttributeDoubleValue( "echo" , 
						     atRunTime );
      //printf( "echo %.2f\n" , echo );
    }
  }

  // max parameter values
  else if( strcmp( XMLtag->tag , "max_values" ) == 0 ) {
    if( XMLtag->hasAttribute( "max_textures" ) ) {
      Nb_max_textures = XMLtag->getAttributeIntValue( "max_textures" , 
						      atRunTime );
    }
 
    if( XMLtag->hasAttribute( "max_shaders" ) ) {
      Nb_max_shaders = XMLtag->getAttributeIntValue( "max_shaders" , 
						     atRunTime );
    }
 
    if( XMLtag->hasAttribute( "max_display_lists" ) ) {
      Nb_max_display_lists = XMLtag->getAttributeIntValue( "max_display_lists" , 
							   atRunTime );
    }
 
    if( XMLtag->hasAttribute( "max_displays" ) ) {
      Nb_max_displays = XMLtag->getAttributeIntValue( "max_displays" , 
						      atRunTime );
    }

    if( XMLtag->hasAttribute( "max_path_components" ) ) {
      Nb_max_path_components = XMLtag->getAttributeIntValue( "max_path_components" , 
							     atRunTime );
    }

    if( XMLtag->hasAttribute( "max_targets" ) ) {
      Nb_max_targets = XMLtag->getAttributeIntValue( "max_targets" , 
						     atRunTime );
    }

    if( XMLtag->hasAttribute( "max_embedded_parameters" ) ) {
      Nb_max_embedded_parameters = XMLtag->getAttributeIntValue( "max_embedded_parameters" , 
								 atRunTime );
    }

    if( XMLtag->hasAttribute( "max_scenes" ) ) {
      Nb_max_scenes = XMLtag->getAttributeIntValue( "max_scenes" , 
						    atRunTime );
    }

    if( XMLtag->hasAttribute( "max_model_view_matrices" ) ) {
      Nb_max_model_view_matrix = XMLtag->getAttributeIntValue( "max_model_view_matrices" , 
							       atRunTime );
    }

    if( XMLtag->hasAttribute( "max_tracks" ) ) {
      Nb_max_tracks = XMLtag->getAttributeIntValue( "max_tracks" , 
							       atRunTime );
    }

    if( XMLtag->hasAttribute( "max_mouse_recording_frames" ) ) {
      Nb_max_mouse_recording_frames = XMLtag->getAttributeIntValue( "max_mouse_recording_frames" , 
							       atRunTime );
    }

    if( XMLtag->hasAttribute( "max_network_message_length" ) ) {
      Max_network_message_length = XMLtag->getAttributeIntValue( "max_network_message_length" , 
							       atRunTime );
    }
  }

  ////// Unknown tag
  else {
    sprintf( ErrorStr , "Error: unknown environment component tag [%s] in environment ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); throw 50;
  }
}

void dm_Environment::parseWindow( int *fileDepth , int *p_c ,
				  dm_XMLTag *XMLtag ) {

  // printf( "Parse windows \n" );
  DM_Window->setParameters( XMLtag , false ,
			    assign , assignI );
  if( XMLtag->tagType == EmptyTag ) {
    return;
  }

  while( *p_c != EOF ) {
    // new tag XML : environment component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    
    // environment closing tag
    if( strcmp( XMLtag->tag , "window" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {
      return;
    }
    else {
      sprintf( ErrorStr , "Error: unknown parameter in <window> parsing %s!" , XMLtag->tag ); ReportError( ErrorStr ); throw 60;
    }
  }
}

void dm_Environment::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  // printf( "Parse environment \n" );
  while( *p_c != EOF ) {
    // new tag XML : environment component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // environment closing tag
    if( strcmp( XMLtag->tag , "environment" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
      XMLtag = NULL;

      double *zero_values;
      zero_values = new double[ (Nb_max_tracks + 1) ];
      memset(zero_values, 0, (Nb_max_tracks + 1) * sizeof(double));
      double *minus_one_values;
      minus_one_values = new double[ (Nb_max_tracks + 1) ];
      memset(minus_one_values, -1, Nb_max_tracks * sizeof(double));
      
      tracks_replay_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_replay_table->SetId( "tracks_replay" );
      tracks_replay_table->SetValue( zero_values );
      frame_color_b_table->nextVariable = tracks_replay_table;
      
      tracks_x_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_x_table->SetId( "tracks_x" );
      tracks_x_table->SetValue( minus_one_values );
      tracks_replay_table->nextVariable = tracks_x_table;
      
      tracks_y_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_y_table->SetId( "tracks_y" );
      tracks_y_table->SetValue( minus_one_values );
      tracks_x_table->nextVariable = tracks_y_table;

      tracks_x_prev_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_x_prev_table->SetId( "tracks_x_prev" );
      tracks_x_prev_table->SetValue( minus_one_values );
      tracks_y_table->nextVariable = tracks_x_prev_table;
      
      tracks_y_prev_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_y_prev_table->SetId( "tracks_y_prev" );
      tracks_y_prev_table->SetValue( minus_one_values );
      tracks_x_prev_table->nextVariable = tracks_y_prev_table;

      tracks_Color_r_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_Color_r_table->SetId( "tracks_Color_r" );
      tracks_Color_r_table->SetValue( zero_values );
      tracks_y_prev_table->nextVariable = tracks_Color_r_table;

      tracks_Color_g_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_Color_g_table->SetId( "tracks_Color_g" );
      tracks_Color_g_table->SetValue( zero_values );
      tracks_Color_r_table->nextVariable = tracks_Color_g_table;

      tracks_Color_b_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_Color_b_table->SetId( "tracks_Color_b" );
      tracks_Color_b_table->SetValue( zero_values );
      tracks_Color_g_table->nextVariable = tracks_Color_b_table;

      tracks_Color_a_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_Color_a_table->SetId( "tracks_Color_a" );
      tracks_Color_a_table->SetValue( zero_values );
      tracks_Color_b_table->nextVariable = tracks_Color_a_table;

      tracks_BrushID_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_BrushID_table->SetId( "tracks_BrushID" );
      tracks_BrushID_table->SetValue( zero_values );
      tracks_Color_a_table->nextVariable = tracks_BrushID_table;

      tracks_Radius_table =  new dm_Table(Nb_max_tracks + 1);
      tracks_Radius_table->SetId( "tracks_Radius" );
      tracks_Radius_table->SetValue( zero_values );
      tracks_BrushID_table->nextVariable = tracks_Radius_table;

      free( (void * )zero_values );
      free( (void * )minus_one_values );

      return;
    }

    setApplicationParameters( XMLtag , false , assign , assignI );

    //////////////// NON EMPTY TAGS
    //////////////// PARSING SUB ELEMENTS

    ////////////////////////////
    ////// viewpoints
    if( (strcmp( XMLtag->tag , "viewpoints" ) == 0  
	|| strcmp( XMLtag->tag , "cameras" ) == 0 ) && XMLtag->tagType == OpeningTag ) {
      default_user->shotIndViewpoint = new int[default_user->nbViewpointShots];
      default_user->shotIni = new float[default_user->nbViewpointShots];
      default_user->shotEnd = new float[default_user->nbViewpointShots];
      
      // parsing the shot intervals and associated viewpoints
      for( int ind = 0; ind < default_user->nbViewpointShots ; ind++ ) {
	default_user->shotIndViewpoint[ind] 
	  = (int)ReadExpression( fileDepth , p_c , 
				 true , NULL , NULL , false , NULL );
	default_user->shotIni[ind] 
	  = (float)ReadExpression( fileDepth , p_c , 
				   true , NULL , NULL , false , NULL );
	default_user->shotEnd[ind] 
	  = (float)ReadExpression( fileDepth , p_c , 
				   true , NULL , NULL , false , NULL );
      }

      // expecting viewpoints closing tag
      ExpectClosingTagXML( fileDepth , p_c , XMLtag->tag , NULL , NULL );
    }

    ////////////////////////////
    ////// windowsx
    else if( strcmp( XMLtag->tag , "windows" ) == 0  && XMLtag->tagType == OpeningTag ) {
      // printf( "Parse windows \n" );
      while( *p_c != EOF ) {
	// new tag XML : environment component or closing tag
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
	
	// environment closing tag
	if( strcmp( XMLtag->tag , "windows" ) == 0 
	    && (XMLtag->tagType == ClosingTag) ) {
	  break;
	}
	else if( strcmp( XMLtag->tag , "window" ) == 0 ) {
	  parseWindow( fileDepth , p_c , XMLtag );
	}
	else {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: unknown tag %s during <windows> parsing!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
	}
      }
    }

    ////////////////////////////
    ////// UDP hosts
    else if( (strcmp( XMLtag->tag , "udp_hosts" ) == 0  
	      || strcmp( XMLtag->tag , "ip_hosts" ) == 0)
	     && XMLtag->tagType == OpeningTag ) {
      char openingTag[16];
      strcpy( openingTag , XMLtag->tag );
      IP_Servers = new dm_IPServer *[ nb_IP_Servers ];
      IP_Clients = new dm_IPClient *[ nb_IP_Clients ];
      
      int ind_IP_Server = 0;
      int ind_IP_Client = 0;
      for(int ind = 0 ; ind < nb_IP_Servers + nb_IP_Clients ; ind++ ) {
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

	if( (strcmp( XMLtag->tag , "udp_local_server" ) == 0 
	     || strcmp( XMLtag->tag , "ip_server" ) == 0 )
	    && XMLtag->tagType == EmptyTag ) {
	  if( ind_IP_Server >= nb_IP_Servers ) {
	    delete XMLtag;
		XMLtag = NULL;

	    sprintf( ErrorStr , "Error: excessive number of servers!" ); ReportError( ErrorStr ); throw 50;
	  }
	  IP_Servers[ ind_IP_Server ] = new dm_IPServer();
	  IP_Servers[ ind_IP_Server ]->setParameters( XMLtag , false ,
						      assign , assignI );
	  ind_IP_Server++;
	  //printf( "scan udp_local_server\n" );
	}

	else if( (strcmp( XMLtag->tag , "udp_remote_client" ) == 0 
		  || strcmp( XMLtag->tag , "ip_client" ) == 0 )
		 && XMLtag->tagType == EmptyTag ) {
	  if( ind_IP_Client >= nb_IP_Clients ) {
	    delete XMLtag;
		XMLtag = NULL;

	    sprintf( ErrorStr , "Error: excessive number of clients!" ); ReportError( ErrorStr ); throw 50;
	  }
	  IP_Clients[ ind_IP_Client ] = new dm_IPClient();
	  IP_Clients[ ind_IP_Client ]->setParameters( XMLtag , false ,
						      assign , assignI );
	  ind_IP_Client++;
	  //printf( "scan remote_client\n" );
	}

	else {
	  sprintf( ErrorStr , "Error: unknown  tag [%s] in IP hosts (ip_server or ip_client empty tag expected)!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
	  delete XMLtag;
	  XMLtag = NULL;
	}
      }

      // replaces the carbon copies by the pointer to the client
      for(int ind = 0 ; ind < nb_IP_Servers ; ind++ ) {
	if( *(IP_Servers[ ind ]->carbon_copyID) ) {
	  for(int indClient = 0 ; indClient < nb_IP_Clients ; indClient++ ) {
	    if( strcmp( IP_Servers[ ind ]->carbon_copyID ,
			IP_Clients[ indClient ]->id ) == 0 ) {
	      IP_Servers[ ind ]->carbon_copyIPClient = IP_Clients[ indClient ];
	      break;
	    }
	  }
	  if( IP_Servers[ ind ]->carbon_copyIPClient == NULL ) {
	    delete XMLtag;
		XMLtag = NULL;

	    sprintf( ErrorStr , "Error: unknown  carbon_copy remote host [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
	  }
	}
      }

      // expecting udp_hosts closing tag
      // printf( "closing tag (%s)\n" , openingTag );
      ExpectClosingTagXML( fileDepth , p_c ,  openingTag , NULL , NULL );
    }

    ////////////////////////////
    ////// unknown tag
    else if( XMLtag->tagType == OpeningTag ) {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unknown environment component tag [%s] in environment ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}


void LoadConfigurationFile( const char * fileName ) {
  dm_SceneNode  *currentNode;
  int            curChar;

  dm_XMLTag     *XMLtag = new dm_XMLTag();
  bool           tagLoaded;

  DM_EnvironmentNode = NULL;

  printf( "Loading %s\n" , fileName );

  int fileDepth = 0;
  for( int i = 0 ; i < DM_NB_MAX_EMBEDDED_XML_FILES ; i++ ) {
    fileXML[ i ] = NULL;
  }
  fileXML[ fileDepth ] = fopen( fileName , "rb" );
  if( !fileXML[ fileDepth ] ) {
    delete XMLtag;
    XMLtag = NULL;

    sprintf( ErrorStr , "Error: scene file [%s] not found!" , fileName ); ReportError( ErrorStr ); throw 11;
  }

  // reading the XML file
  curChar = ReadChar( &fileDepth , NULL , NULL);
  SpaceCommentsInclude( &fileDepth , &curChar , NULL , NULL );
  
  // reading the tag of a component
  tagLoaded = false;
  if( curChar != EOF ) {
    do {
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( &fileDepth , &curChar , true , NULL , NULL );
      if( *(XMLtag->tag) != '?' ) {
	tagLoaded = true;
      }
    } while( !tagLoaded );
  }

  while( curChar != EOF ) {
    // reading the tag of a component
    if( !tagLoaded ) {
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( &fileDepth , &curChar , true , NULL , NULL );
    }
    else {
      tagLoaded = false;
    }

    // global parameter
    if( strcmp( XMLtag->tag , "set" ) == 0 && XMLtag->tagType == EmptyTag ) {
      if( !DM_EnvironmentNode ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: environment must be defined prior to parameter definitions!" ); ReportError( ErrorStr ); throw 10;
      }

      // reading a set node
      int nbReturnedNodes;
      currentNode = ParseSet( &fileDepth , &curChar , 
			      XMLtag , &nbReturnedNodes );
    }

    // elementary component: the root node(s) for a scene file
    // elementary component: the environment node for a configuration file
    else if( strcmp( XMLtag->tag , "node" ) == 0 
	     && XMLtag->tagType == OpeningTag ) {
      // printf( "parse node\n" );
      currentNode = ParseNode( &fileDepth , &curChar , true , XMLtag );

      // configuration file == a single environment node
      if( DM_EnvironmentNode ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: configuration file must be made of a single node (environment node or environment interpolator node!" ); ReportError( ErrorStr ); throw 62;
      }
      
      // the node that carries the environment
      envtNode = currentNode;
      
      if( envtNode->type == AEnvironment ) {
	DM_EnvironmentNode = (dm_Environment *)(currentNode->sceneObject);
      }
      else {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: environment node expected in configuration file (%s)!" , NodeTypeString[envtNode->type] ); ReportError( ErrorStr ); throw 10;
      }
      
      // parameters defined by def, repeat and set tags
      DM_EnvironmentNode->ParameterStrings = new char[
		  DM_EnvironmentNode->Nb_max_embedded_parameters * XMLAttrValLength];
      DM_EnvironmentNode->ParameterValues = new char[
		  DM_EnvironmentNode->Nb_max_embedded_parameters * XMLAttrValLength];
      
      // envt node has a pointer to scalars alt/control/shift scalars
      // on the environment object node
      envtNode->lisScalars = DM_EnvironmentNode->mouse_x_scalar;

      //DM_EnvironmentNode->print( stdout , 1 , 1 );
      envtNode->lisTables = DM_EnvironmentNode->frame_color_r_table;
    }
    
    else if( XMLtag->tagType == ClosingTag ) {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unbalanced tag [%s] (excessive number of closing tags)!" , XMLtag->tag ); ReportError( ErrorStr ); throw 62;
    }

    else {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unknown root tag [%s] (node expected)!" , XMLtag->tag ); ReportError( ErrorStr ); throw 62;
    }
  }
  //printf("scene parsed\n" );

  // returned value
  delete XMLtag;
  XMLtag = NULL;

  fclose( fileXML[ fileDepth ] );
}

