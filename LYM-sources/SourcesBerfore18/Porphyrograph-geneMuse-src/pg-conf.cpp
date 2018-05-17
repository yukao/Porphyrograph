/*! \file pg-conf.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-conf.cpp
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
#include "pg-all_include.h"

/*! \name Group constants
 *  \brief Enumerated constants
 */
// \{
const char *Cursor_ShapeString[EmptyCursorShape + 1] = { "no_cursor" , "left_arrow" , "right_arrow" , "EmptyCursorShape" };

const char *ExternalMessageFormatString[EmptyExternalMessageFormat + 1] = { "Plain" , "OSC" , "EmptyExternalMessageFormat" };

/////////////////////////////////////////////////////
// global variable for screenshot file name
/////////////////////////////////////////////////////
char currentFilename[512];

void indent( FILE *file , int depth ) {
  for( int ind = 0 ; ind < depth ; ind++ ) {
    fprintf( file , "  " );
  }
}

/////////////////////////////////////////////////////
// Default values for global variables
/////////////////////////////////////////////////////


pg_Window::pg_Window( void ) {
  id = NULL;
  InitWindowVC();
}

pg_Window::~pg_Window(void) {
  if (id) {
    delete [] id;
    id = NULL;
  }

}

void pg_Window::InitWindowVC( void ) {
  glutID = -1;
  texID = 0;
  InitString( &id );
  windowHeight = HEIGHT;
  windowWidth = WIDTH;
  windowNonFullScreenHeight = HEIGHT;
  windowNonFullScreenWidth = WIDTH;
  windowHeightIni = -1;
  windowWidthIni = -1;
  window_x = 0;
  window_y = 0;
}

int pg_Window::getWindowHeight( void ){
  return windowHeight;
}
int pg_Window::getWindowWidth( void ){
  return windowWidth;
}
int pg_Window::getWindow_x( void ){
  return window_x;
}
int pg_Window::getWindow_y( void ){
  return window_y;
}
void pg_Window::setWindowHeight( float val ){
  windowHeight = (int)val;
  // printf( "windowHeight win %s %d\n" , id , windowHeight );
  if( windowHeightIni < 0 ) {
    windowHeightIni = windowHeight;
  }
}
void pg_Window::setWindowWidth( float val ){
  windowWidth = (int)val;
  // printf( "windowWidth win %s %d\n" , id , windowWidth );
  if( windowWidthIni < 0 ) {
    windowWidthIni = windowWidth;
  }
}
void pg_Window::setWindow_x( float val ){
  window_x = (int)val;
  // printf( "setWindow id %s window_x %d\n" , id , window_x );
}

void pg_Window::setWindow_y( float val ){
  window_y = (int)val;
}

void pg_Window::position( float x , float y ) {
  setWindow_x( x );
  setWindow_y( y );
}
void pg_Window::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<window id=\"%s\" width=\"%d\" height=\"%d\" x=\"%d\" y=\"%d\" >\n" , id , windowWidth , windowHeight , window_x , window_y );
  indent( file , depth );
  fprintf( file , "</window>\n" );
}

void pg_Window::setParameters( pg_XMLTag *XMLtag , bool atRunTime ,
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

}

void pg_Window::operator=(pg_Window& w) {
  StrCpy( &id , w.id );
  glutID = w.glutID;
  texID = w.texID;
  // do not need to be reallocated 
  // are just internal pointers
  windowHeight = w.windowHeight;
  windowWidth = w.windowWidth;
  windowNonFullScreenHeight = w.windowNonFullScreenHeight;
  windowNonFullScreenWidth = w.windowNonFullScreenWidth;
  window_x = w.window_x;
  window_y = w.window_y;
}

pg_Environment::pg_Environment( void ) {
  // --------- composition_output_file ----------- //
  composition_output_file_name = NULL;
  StrCpy( &composition_output_file_name , "worlds-out.xml" );
  default_print_file_name = NULL;
  StrCpy( &default_print_file_name , "PG-out.xml" );

  // --------- windows ----------- //
  PG_Window = NULL;

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
#ifdef PG_HAVE_FFMPEG
  audioVideoOutData = NULL;
#endif

  // --------- frame_rate ----------- //
  minimal_interframe_latency = 40.0f;
  time_scale = 1.0f;

  // --------- audio ----------- //
  input_volume = 0.5f;

  // --------- log_file ----------- //
  log_file_name = NULL;
  log_date_output = false;
  StrCpy( &log_file_name , "PG-main.log" );

  // --------- font_file ----------- //
  font_file_name = NULL;
  StrCpy( &font_file_name , "./usascii/arial/stb_font_arial_18_usascii.png" );
  font_file_encoding = NULL;
  StrCpy( &font_file_encoding , "png" );
  font_texture_encoding = PNG;
  font_size = 12;
  message_pixel_length = 128;

  // --------- ip hosts ----------- //
  IP_Servers = NULL;
  nb_IP_Servers = 0;

  IP_Clients = NULL;
  nb_IP_Clients = 0;

  // --------- scene_schedule ----------- //
  initial_time = 0.0;

  // configuration file scalar variables

  // --------- cursor ----------- //  _size = 10;
  cursorShape = NoCursor;

  // ---------- interpolation ----------- //
  nb_scenes = 0;
  scene_initial_times = NULL;
  scene_final_times = NULL;
  scene_durations = NULL;
  scene_originalInitial_times = NULL;
  scene_originalFinal_times = NULL;
  scene_originalDurations = NULL;
  scene_IDs = NULL;
  scene_initial_parameters = NULL;
  scene_final_parameters = NULL;
  scene_interpolations = NULL;

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

  // --------- max_values ----------- //
  Nb_max_mouse_recording_frames = 1024;
  Max_network_message_length = 65535;
}

pg_Environment::~pg_Environment( void ) {	
  if (composition_output_file_name) {
    delete [] composition_output_file_name;
    composition_output_file_name = NULL;
  }

  if (default_print_file_name) {
    delete [] default_print_file_name;
    default_print_file_name = NULL;
  }

  if (PG_Window)  {
    delete PG_Window;
    PG_Window = NULL;
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

}

void pg_Environment::initWindows( void ) {
  PG_Window = new pg_Window();
  windowDisplayed = false;
  double_window = false;
}

void pg_Environment::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<environment>\n" );
  indent( file , depth + 1 );
  fprintf( file , "<composition_output_file id=\"%s\" default_print_file=\"%s\"/>\n" , composition_output_file_name , default_print_file_name );

  indent( file , depth + 1 );
  fprintf( file , "<windows>\n" );
  PG_Window->print( file , isLong , depth + 2 );
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
  fprintf( file , "<frame_rate minimal_interframe_latency=\"%.5f\" time_scale=\"%.5f\"/>\n" ,  minimal_interframe_latency , time_scale );
  indent( file , depth + 1 );
  fprintf( file , "<audio input_volume=\"%.5f\"/>\n" , input_volume );
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
  fprintf( file , "<display full_screen=\"%s\" game_mode=\"%s\"/>\n" , BooleanString[BooleanToInt(full_screen)] , BooleanString[BooleanToInt(game_mode)] );
  indent( file , depth + 1 );
  fprintf( file , "<scene_schedule initial_time=\"%.5f\"/>\n" , initial_time );
  indent( file , depth + 1 );
  fprintf( file , "<cursor shape=\"%s\" cursorSize=\"%d\"/>\n" , Cursor_ShapeString[cursorShape] , cursorSize );
  indent( file , depth + 1 );
  fprintf( file , "<max_values max_mouse_recording_frames=\"%d\" max_network_message_length=\"%d\"/>\n" , Nb_max_mouse_recording_frames , Max_network_message_length );
  indent( file , depth );
  fprintf( file , "</environment>\n" );
}

void pg_Environment::display( DrawingMode mode , bool *invalidDL ) {
}

void pg_Environment::operator=(pg_Environment& e) {
  // --------- composition_output_file ----------- //
  // composition output after reading initial scenes
  StrCpy( &composition_output_file_name , e.composition_output_file_name );
  // text output file name in the edition mode
  StrCpy( &default_print_file_name , e.default_print_file_name );

  // --------- windows ----------- //
  if( PG_Window ) {
    delete [] PG_Window;
    PG_Window = NULL;
  }

  // contents are copied for safe deallocation
  PG_Window = new pg_Window();
  *(PG_Window) = *(e.PG_Window);
  windowDisplayed = false;
  double_window = e.double_window;


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
#ifdef PG_HAVE_FFMPEG
  audioVideoOutData = e.audioVideoOutData;
#endif

  // --------- audio ----------- //
  input_volume = e.input_volume;

  // --------- frame_rate ----------- //
  // frame_rate speed adjustments to hardware
  minimal_interframe_latency = e.minimal_interframe_latency;
  time_scale = e.time_scale;

  // --------- log_file ----------- //
  StrCpy( &log_file_name , e.log_file_name );
  log_date_output = e.log_date_output;

  // --------- font ----------- //
  StrCpy( &font_file_name , e.font_file_name );
  StrCpy( &font_file_encoding , e.font_file_encoding );
  font_texture_encoding = e.font_texture_encoding;
  font_size = e.font_size;
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
  IP_Servers = new pg_IPServer * [ nb_IP_Servers ];
  // contents are copied for safe deallocation
  for(int ind = 0 ; ind < nb_IP_Servers ; ind++) {
    IP_Servers[ ind ] = new pg_IPServer();
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
  IP_Clients = new pg_IPClient * [ nb_IP_Clients ];
  // contents are copied for safe deallocation
  for(int ind = 0 ; ind < nb_IP_Clients ; ind++) {
    IP_Clients[ ind ] = new pg_IPClient();
    *(IP_Clients[ ind ]) = *(e.IP_Clients[ ind ]);
  }

  // --------- scene_schedule ----------- //
  initial_time = e.initial_time;

  // --------- cursor ----------- //
  cursorShape = e.cursorShape;
  cursorSize = e.cursorSize;

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

  // --------- max_values ----------- //
  Nb_max_mouse_recording_frames = e.Nb_max_mouse_recording_frames;
  Max_network_message_length = e.Max_network_message_length;
}

/////////////////////////////////////////////////////
// environment parsing from configuration file
/////////////////////////////////////////////////////

void pg_Environment::setParameters( pg_XMLTag *XMLtag , bool atRunTime ,
				    FloatBinOp oper ,
				    IntBinOp operI ) {
}

bool pg_Environment::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
				    int * tableOrMatrixSize ) {
  if( strncmp( attribute , "svg_rendering_file" , 
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

void pg_Environment::setApplicationParameters( pg_XMLTag *XMLtag , 
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
    if( XMLtag->hasAttribute( "double_window" ) ) {
      XMLtag->BooleanFieldValue( "double_window" ,
				 &(double_window) );
    }
  }
  // windows node content is parsed in calling function pg_Environment::parse
   
  // dynamic single window layout
  else if( strcmp( XMLtag->tag , "window" ) == 0 && atRunTime ) {
    if( XMLtag->hasAttribute( "id" ) ) {
      char* idWin = XMLtag->getAttributeStringValue( "id" );
    
      if( strcmp( PG_EnvironmentNode->PG_Window->id , 
		  idWin ) == 0 ) {
	PG_EnvironmentNode->PG_Window->setParameters( XMLtag ,
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
    if( XMLtag->hasAttribute( "time_scale" ) ) {
      time_scale 
	= (float)XMLtag->getAttributeDoubleValue( "time_scale" , 
					  atRunTime );
    }
  }
 
  // --------- audio ----------- //
  else if( strcmp( XMLtag->tag , "audio" ) == 0 ) {
    if( XMLtag->hasAttribute( "input_volume" ) ) {
      input_volume 
	= (float)XMLtag->getAttributeDoubleValue( "input_volume" , 
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

  // scenario
  else if( strcmp( XMLtag->tag , "scenario" ) == 0 ) {
    // printf( "Parse scenario \n" );
    if( XMLtag->hasAttribute( "nb_scenes" ) ) {
      nb_scenes
	= XMLtag->getAttributeIntValue( "nb_scenes" , atRunTime );
    }
    else {
      sprintf( ErrorStr , "Error: missing attribute of scenario: nb_scenes!" ); ReportError( ErrorStr ); throw 10;
    }
 }
  // scenario node content is parsed in calling function pg_Environment::parse

  // schedule
  else if( strcmp( XMLtag->tag , "scene_schedule" ) == 0 ) {
    if( XMLtag->hasAttribute( "initial_time" ) ) {
      initial_time = (float)XMLtag->getAttributeDoubleValue( "initial_time" , 
					   atRunTime );
    }
  }

  // cursor
  else if( strcmp( XMLtag->tag , "cursor" ) == 0 ) {
    if( XMLtag->hasAttribute( "shape" ) ) {
      char *cursor_str = XMLtag->getAttributeStringValue( "shape" );
      cursorShape = EmptyCursorShape;
      for( int ind = 0 ; ind < EmptyCursorShape ; ind++ ) {
	if( strcmp( cursor_str , Cursor_ShapeString[ind] ) == 0 ) {
	  cursorShape = (Cursor_Shape)ind;
	  break;
	}
      }
      if( cursorShape == EmptyCursorShape ) {
	sprintf( ErrorStr , "Error: unknown cursor shape  [%s] in configuration file!" , cursor_str ); ReportError( ErrorStr ); throw 345;
      }
    }
    if( XMLtag->hasAttribute( "cursorSize" ) ) {
		cursorSize = XMLtag->getAttributeIntValue("cursorSize",
				atRunTime );
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

  }

  // max parameter values
  else if( strcmp( XMLtag->tag , "max_values" ) == 0 ) {
    if( XMLtag->hasAttribute( "max_mouse_recording_frames" ) ) {
      Nb_max_mouse_recording_frames = XMLtag->getAttributeIntValue( "max_mouse_recording_frames" , 
							       atRunTime );
      int pot_value = smallestPOT( Nb_max_mouse_recording_frames );
      if( pot_value != Nb_max_mouse_recording_frames ) {
	printf("Max recording frames is not POT -> new value %d\n" , pot_value );
	Nb_max_mouse_recording_frames = pot_value;
      }
    }

    if( XMLtag->hasAttribute( "max_network_message_length" ) ) {
      Max_network_message_length = XMLtag->getAttributeIntValue( "max_network_message_length" , 
							       atRunTime );
    }
  }

  ////// Unknown tag
  else {
    sprintf( ErrorStr , "Error: unknown environment tag [%s] in environment!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
  }

}

void pg_Environment::parseWindow( int *fileDepth , int *p_c ,
				  pg_XMLTag *XMLtag ) {

  // printf( "Parse windows \n" );
  PG_Window->setParameters( XMLtag , false ,
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

void pg_Environment::parse( int *fileDepth , int *p_c ) {
  pg_XMLTag *XMLtag = new pg_XMLTag();

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
      zero_values = new double[ 1 ];
      memset(zero_values, 0, 1 * sizeof(double));
      double *minus_one_values;
      minus_one_values = new double[ 1 ];
      memset(minus_one_values, -1, 1 * sizeof(double));
      
      free( (void * )zero_values );
      free( (void * )minus_one_values );

      return;
    }

    setApplicationParameters( XMLtag , false , assign , assignI );

    ////////////////////////////
    ////// windowsx
    if( strcmp( XMLtag->tag , "windows" ) == 0  && XMLtag->tagType == OpeningTag ) {
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
      IP_Servers = new pg_IPServer *[ nb_IP_Servers ];
      IP_Clients = new pg_IPClient *[ nb_IP_Clients ];
      
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
	  IP_Servers[ ind_IP_Server ] = new pg_IPServer();
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
	  IP_Clients[ ind_IP_Client ] = new pg_IPClient();
	  IP_Clients[ ind_IP_Client ]->setParameters( XMLtag , false ,
						      assign , assignI );
	  ind_IP_Client++;
	  //printf( "scan remote_client\n" );
	}

	else {
	  sprintf( ErrorStr , "Error: unknown  tag [%s] in IP hosts (ip_server or ip_client empty tag expected)!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
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
    ////// scenario
    else if( (strcmp( XMLtag->tag , "scenario" ) == 0)
	     && XMLtag->tagType == OpeningTag ) {
      char openingTag[16];

      strcpy( openingTag , XMLtag->tag );
      scene_IDs = new char *[ nb_scenes ];
      for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_IDs[ indScene ] = NULL;
	InitString( &(scene_IDs[ indScene ]) );
      }

      scene_durations = new float[ nb_scenes ];
      scene_initial_times = new float[ nb_scenes ];
      scene_final_times = new float[ nb_scenes ];
      scene_originalDurations = new float[ nb_scenes ];
      scene_originalInitial_times = new float[ nb_scenes ];
      scene_originalFinal_times = new float[ nb_scenes ];
      for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_durations[ indScene ] = 0.0;
	scene_initial_times[ indScene ] = 0.0;
	scene_final_times[ indScene ] = 0.0;
	scene_originalDurations[ indScene ] = 0.0;
	scene_originalInitial_times[ indScene ] = 0.0;
	scene_originalFinal_times[ indScene ] = 0.0;
      }

      scene_initial_parameters = new float *[ nb_scenes ];
      scene_final_parameters = new float *[ nb_scenes ];
      for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_initial_parameters[ indScene ] = NULL;
	scene_final_parameters[ indScene ] = NULL;
      }

      scene_interpolations = new pg_Interpolation *[ nb_scenes ];
      for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_interpolations[ indScene ] = NULL;
      }
      printf("Loading %d scenes with %d variables\n" , nb_scenes , _MaxInterpVarIDs );
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
      if( (strcmp( XMLtag->tag , "initial_values" ) == 0 )
	  && XMLtag->tagType == OpeningTag ) {
	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    float val;
	    val = ReadFloat( fileDepth , p_c  ,
			     true , NULL , NULL , false );
	    InitialValuesVar[ indP ] = val;
	  }

          // expecting scenario closing tag
          ExpectClosingTagXML( fileDepth , p_c ,  "initial_values" , NULL , NULL );
     }
     for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

	if( (strcmp( XMLtag->tag , "scene" ) == 0 )
	    && XMLtag->tagType == OpeningTag ) {
	  StrCpy( &(scene_IDs[ indScene ]) , XMLtag->getAttributeStringValue( "id" ) );
	  scene_durations[ indScene ] 
	    = (float)XMLtag->getAttributeDoubleValue( "duration" , false );
	  if( scene_durations[ indScene ]  <= 0.0 ) {
	    sprintf( ErrorStr , "Error: null scene #%d duration [%f]!" , indScene + 1 , scene_durations[ indScene ]  ); ReportError( ErrorStr ); throw 50;
	  }
	  if( indScene > 0 ) {
	    scene_initial_times[ indScene ] = scene_final_times[ indScene - 1 ];
	  }
	  else {
	    scene_initial_times[ indScene ] = 0.0;
	  }
	  scene_final_times[ indScene ] 
	    = scene_initial_times[ indScene ] + scene_durations[ indScene ];

	  scene_initial_parameters[ indScene ] = new float[ _MaxInterpVarIDs ];
	  scene_final_parameters[ indScene ] = new float[ _MaxInterpVarIDs ];
	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    scene_initial_parameters[ indScene ][ indP ] = 0.0;
	    scene_final_parameters[ indScene ][ indP ] = 0.0;
	  }
	  scene_interpolations[ indScene ] 
	    = new pg_Interpolation[ _MaxInterpVarIDs ];
	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    scene_interpolations[ indScene ][ indP ].interpolation_mode 
	      = pg_linear_interpolation;
	    scene_interpolations[ indScene ][ indP ].offSet = 0.0;
	    scene_interpolations[ indScene ][ indP ].duration = 1.0;
	  }
	  
	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    float val;
	    val = ReadFloat( fileDepth , p_c  ,
			     true , NULL , NULL , false );
	    scene_initial_parameters[ indScene ][ indP ] = val;
	  }
	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    float val;
	    val = ReadFloat( fileDepth , p_c  ,
			     true , NULL , NULL , false );
	    scene_final_parameters[ indScene ][ indP ] = val;
	  }

	  for(int indP = 0 ; indP < _MaxInterpVarIDs ; indP++ ) {
	    float val,val2;
	    char valCh;

	    scene_interpolations[ indScene ][ indP ].offSet = 0.0; 
	    scene_interpolations[ indScene ][ indP ].duration = 1.0; 

	    valCh = *p_c;
	    *p_c = ReadChar(fileDepth,NULL,NULL);
	    SpaceCommentsInclude( fileDepth , p_c , NULL,NULL);

	    switch( valCh ) {
	    // l: value interpolates linearly between initial and final value from 0.0% to 1.0%
	    // L: value is initial from 0.0% until offset, 
	    // interpolates linearly between initial and final value from offset to offset + duration
	    // is final value between offset + duration and 1.0%
	    case 'l':
	    case 'L':
	      scene_interpolations[ indScene ][ indP ].interpolation_mode 
		= pg_linear_interpolation;
	      if( valCh == 'L' ) {
		val = ReadFloat( fileDepth , p_c  ,
				 true , NULL , NULL , false );
		val2 = ReadFloat( fileDepth , p_c  ,
				 true , NULL , NULL , false );
		if( val < 0.0 || val2 < 0.0 ) {
	          sprintf( ErrorStr , "Error: one of values of L(inear) interpolationn #%d lower than 0.0: %.3f %.3f\n" , indP + 1 , val , val2 ); ReportError( ErrorStr ); throw 50;
		}
		if( val <= 1.0 ) {
		  scene_interpolations[ indScene ][ indP ].offSet = val;
	   	  if( val + val2 <= 1.00001 ) {
		    // deals with approximate values that can summ above 1.0
		    if( val + val2 > 1.0 ) {
		       val2 = 1.0f - val;
		     }
		     scene_interpolations[ indScene ][ indP ].duration = val2;
	 	     if( scene_interpolations[ indScene ][ indP ].duration  <= 0.0 ) {
		        sprintf( ErrorStr , "Error: null L(inear) interpolation #%d duration [%f]!" , indP + 1 , scene_interpolations[ indScene ][ indP ].duration  ); ReportError( ErrorStr ); throw 50;
		     }
		  }
		  else {
	            sprintf( ErrorStr , "Error: total duration of L(inear) interpolation #%d greater than 1.0: %.3f + %.3f\n" , indP + 1 , val , val2 ); ReportError( ErrorStr ); throw 50;
		  }
		}
		else {
	          sprintf( ErrorStr , "Error: offset value L(inear) interpolation #%d greater than 1.0: %.3f\n" , indP + 1 , val ); ReportError( ErrorStr ); throw 50;
		}
	      }
	      break;
	    case 'c':
	    case 'C':
	      scene_interpolations[ indScene ][ indP ].interpolation_mode
		= pg_cosine_interpolation;
	      if( valCh == 'C' ) {
		val = ReadFloat( fileDepth , p_c  ,
				 true , NULL , NULL , false );
		val2 = ReadFloat( fileDepth , p_c  ,
				 true , NULL , NULL , false );
		if( val < 0.0 || val2 < 0.0 ) {
	          sprintf( ErrorStr , "Error: one of values of C(osine) interpolation #%d lower than 0.0: %.3f %.3f\n" , indP + 1 , val , val2 ); ReportError( ErrorStr ); throw 50;
		}
		if( val <= 1.0 ) {
		  scene_interpolations[ indScene ][ indP ].offSet = val;
  	          // deals with approximate values that can summ above 1.0
	   	  if( val + val2 <= 1.00001 ) {
		     if( val + val2 > 1.0 ) {
		       val2 = 1.0f - val;
		     }
		     scene_interpolations[ indScene ][ indP ].duration = val2;
	 	     if( scene_interpolations[ indScene ][ indP ].duration  <= 0.0 ) {
		        sprintf( ErrorStr , "Error: null C(osine) interpolation #%d duration [%f]!" , indP + 1 , scene_interpolations[ indScene ][ indP ].duration  ); ReportError( ErrorStr ); throw 50;
		     }
		  }
		  else {
	            sprintf( ErrorStr , "Error: total duration of C(osine) interpolation #%d greater than 1.0: %.3f + %.3f\n" , indP + 1 , val , val2 ); ReportError( ErrorStr ); throw 50;
		  }
		}
		else {
	          sprintf( ErrorStr , "Error: offset value C(osine) interpolation #%d greater than 1.0: %.3f\n" , indP + 1 , val ); ReportError( ErrorStr ); throw 50;
		}
	      }
	      break;
	    case 's':
	    case 'S':
	      scene_interpolations[ indScene ][ indP ].interpolation_mode 
		= pg_stepwise_interpolation;
	      scene_interpolations[ indScene ][ indP ].offSet = 1.0;
	      scene_interpolations[ indScene ][ indP ].duration = 0.0; 
	      if( valCh == 'S' ) {
		val = ReadFloat( fileDepth , p_c  ,
				 true , NULL , NULL , false );
		if( val < 0.0 ) {
	          sprintf( ErrorStr , "Error: offset values of S(tepwise) interpolation #%d lower than 0.0: %.3f\n" , indP + 1 , val ); ReportError( ErrorStr ); throw 50;
		}
		if( val <= 1.0 ) {
		  scene_interpolations[ indScene ][ indP ].offSet = val;
		}
		else {
	          sprintf( ErrorStr , "Error: offset value of S(tepwise) interpolation #%d greater than 1.0: %.3f\n" , indP + 1 , val ); ReportError( ErrorStr ); throw 50;
		}
	      }
	      break;
	    default:
	      sprintf( ErrorStr , "Error: unknown  interpolation mode [%c] in scene %d parameter %d!" , valCh , indScene + 1 , indP + 1 ); ReportError( ErrorStr ); throw 50;
	      break;
	    }
	  }
	    
	  ExpectClosingTagXML( fileDepth , p_c ,  "scene" , NULL , NULL );
	}

	else {
	  sprintf( ErrorStr , "Error: unknown  tag [%s] in IP hosts (ip_server or ip_client empty tag expected)!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
	}
      }

      // expecting scenario closing tag
      // printf( "closing tag (%s)\n" , openingTag );
      ExpectClosingTagXML( fileDepth , p_c ,  openingTag , NULL , NULL );

      // saves the original durations
      saveInitialTimesAndDurations();
    }

    ////////////////////////////
    ////// unknown tag
    else if( XMLtag->tagType == OpeningTag ) {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unknown environment component tag [%s] in environment!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

// to be called once after parsing
void pg_Environment::saveInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_originalDurations[ indScene ] = scene_durations[ indScene ];
	scene_originalInitial_times[ indScene ] = scene_initial_times[ indScene ];
	scene_originalFinal_times[ indScene ] = scene_final_times[ indScene ];
 }
}
// to be called before setup change
void pg_Environment::restoreInitialTimesAndDurations( void ) {
 for(int indScene = 0 ; indScene < nb_scenes ; indScene++ ) {
	scene_durations[ indScene ] = scene_originalDurations[ indScene ];
	scene_initial_times[ indScene ] = scene_originalInitial_times[ indScene ];
	scene_final_times[ indScene ] = scene_originalFinal_times[ indScene ];
 }
}

void LoadConfigurationFile( const char * fileName ) {
  int            curChar;

  pg_XMLTag     *XMLtag = new pg_XMLTag();
  bool           tagLoaded;

  PG_EnvironmentNode = NULL;

  printf( "Loading %s\n" , fileName );

  int fileDepth = 0;
  for( int i = 0 ; i < PG_NB_MAX_EMBEDDED_XML_FILES ; i++ ) {
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

    // elementary component: the root node(s) for a scene file
    // elementary component: the environment node for a configuration file
    if( strcmp( XMLtag->tag , "node" ) == 0 
	&& XMLtag->tagType == OpeningTag ) {
      // configuration file == a single environment node
      if( PG_EnvironmentNode ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: configuration file must be made of a single node (environment node or environment interpolator node!" ); ReportError( ErrorStr ); throw 62;
      }
      
      PG_EnvironmentNode = ParseNode( &fileDepth , &curChar , true , XMLtag );

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

