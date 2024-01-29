/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 3
/*#define ATELIERS_PORTATIFS
*/// #define WHITE_CIRCLE
// #define MIRRORED_PROJECTION

#include_declarations

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Master_texture_fs_Render_curr; // Master pass output with possible echo
layout (binding = 1) uniform samplerRect uniform_Master_texture_fs_CA;  // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_Part_render;  // Particle step n
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Update pass BG track step n (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;

/*uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec4 uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;
*/
/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
#include_initializations

  float width = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.x;
  float height = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);

  // vertical mirror
  // TO UNCOMMENT FOR CHATEAU  
  //    coords.y = height - coords.y;
#ifdef MIRRORED_PROJECTION
  // ST OUEN horizontal mirror
  // coords.x = width - coords.x;
  coords.y = height - coords.y;
#endif
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed layers according to Mixing weights
  vec4 MixingColor = texture(uniform_Master_texture_fs_Render_curr, coords );

  ////////////////////////////////////////////////////////////////////
  // non-echoed layers
  vec4 CA_color = texture(uniform_Master_texture_fs_CA, coords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
  vec4 particle_color = texture(uniform_Master_texture_fs_Part_render, coords);

  vec4 track0_color = texture(uniform_Master_texture_fs_Trk0, coords);
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(uniform_Master_texture_fs_Trk1, coords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(uniform_Master_texture_fs_Trk2, coords);
#endif
#if PG_NB_TRACKS >= 4
  vec4 track3_color = texture(uniform_Master_texture_fs_Trk3, coords);
#endif

  ////////////////////////////////////////////////////////////////////
  // mix of non echoed layers according to final weights
  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMasterWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMasterWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMasterWeight_3
#endif
    + CA_color.rgb * CAMasterWeight
    + particle_color.rgb * PartMasterWeight * particle_color.a
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse (except for hide)
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;

  // possible double cursor
  float coordX = decalCoords.x;

  // comment for single cursor
/*  if( coordX > width) {
    coordX -= width;
  }
*/  // comment for single cursor

// vertical mirror
//   coords.y = height - coords.y
#ifdef MIRRORED_PROJECTION
// ST OUEN horizontal mirror
 // coordX = width - coordX;
#endif
// double mirror
//   coords.y = height - coords.y;
//   coords.x = width - coords.x;

  // invert
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  // white flash
  float timeFromStart = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.w;
  // transition invvert début frozen clear
  if(timeFromStart > 1020 && timeFromStart < 1024) {
    outColor0.rgb = clamp(outColor0.rgb + vec3((2 - abs(timeFromStart - 1022))/2.0), 0.0, 1.0);
  }
  // pendant frozen clear à 17'19" du début et à 19" debut frozen clea
  // événement musical
  /*
  else if(timeFromStart > 1035 && timeFromStart < 1043) {
    outColor0.rgb = clamp(outColor0.rgb + vec3(4 - abs(timeFromStart - 1039)), 0.0, 1.0);
  }
  */
  // transition invvert début ragnarok
  else if(timeFromStart > 1436 && timeFromStart < 1444) {
    outColor0.rgb = clamp(outColor0.rgb + vec3((4 - abs(timeFromStart - 1440))/4.0), 0.0, 1.0);
  }

  // central circle
  int  radiusCircle;
  if( coordX > width) {
    coordX -= width;
  }
  radiusCircle = int(length(vec2(coordX - 700 , coords.y - 525)));
  if( radiusCircle > 525  ) {
    outColor0.rgb = vec3(0);
  }
  #ifdef WHITE_CIRCLE
   else if( radiusCircle == 525 ) {
    outColor0.rgb = vec3(1);
  }
  #endif
  else if( radiusCircle > 490 ) {
    outColor0.rgb *= ((525 - radiusCircle)/35.0);
  }

  // cursor
  if( mouse_x < width && mouse_x > 0 
      && length(vec2(coordX - mouse_x , height - coords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  outColor0.rgb *= master;
}
