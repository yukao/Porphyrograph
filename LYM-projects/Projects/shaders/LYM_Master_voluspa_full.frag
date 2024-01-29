/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 3
// #define WHITE_CIRCLE
// #define FULL_VERSION
//#define MIRRORED_PROJECTION
// #define ROTATED_PROJECTION

bool	  mute_second_screen;
bool	  invertAllLayers;
int		cursorSize;
float	 master;
float	 CAMasterWeight;
float	 PartMasterWeight;
float	 trackMasterWeight_0;
float	 trackMasterWeight_1;
float	 trackMasterWeight_2;
float	 trackMasterWeight_3;
float	 master_mask_scale;
float	 master_mask_scale_ratio;
float	 master_mask_offsetX;
float	 master_mask_offsetY;
bool	  interfaceOnScreen;
uniform float uniform_Master_scenario_var_data[15];

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
#if PG_NB_TRACKS == 1
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 2
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 3
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 4
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_Mask;  // mask for master output
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec3 uniform_Master_fs_3fv_width_height_timeFromStart;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;

/*uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec4 uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene;
uniform vec4 uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;
*/
/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
  mute_second_screen = (uniform_Master_scenario_var_data[0] > 0 ? true : false);
  invertAllLayers = (uniform_Master_scenario_var_data[1] > 0 ? true : false);
  cursorSize = int(uniform_Master_scenario_var_data[2]);
  master = uniform_Master_scenario_var_data[3];
  CAMasterWeight = uniform_Master_scenario_var_data[4];
  PartMasterWeight = uniform_Master_scenario_var_data[5];
  trackMasterWeight_0 = uniform_Master_scenario_var_data[6];
  trackMasterWeight_1 = uniform_Master_scenario_var_data[7];
  trackMasterWeight_2 = uniform_Master_scenario_var_data[8];
  trackMasterWeight_3 = uniform_Master_scenario_var_data[9];
  master_mask_scale = uniform_Master_scenario_var_data[10];
  master_mask_scale_ratio = uniform_Master_scenario_var_data[11];
  master_mask_offsetX = uniform_Master_scenario_var_data[12];
  master_mask_offsetY = uniform_Master_scenario_var_data[13];
  interfaceOnScreen = (uniform_Master_scenario_var_data[14] > 0 ? true : false);

  float width = uniform_Master_fs_3fv_width_height_timeFromStart.x;
  float height = uniform_Master_fs_3fv_width_height_timeFromStart.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);

#ifdef ROTATED_PROJECTION
  float old_coordX = coords.x;
  coords.x = coords.y;
  coords.y = width - old_coordX;
  coords.y -= (width - height)/2;
#endif
  vec2 initialCoords = coords;
  vec2 centerCoords = vec2(width/2, height/2);

  int currentScene = uniform_Master_fs_2iv_mobile_cursor_currentScene.y;

  // vertical mirror
  // TO UNCOMMENT FOR CHATEAU  
  coords.x = width - coords.x;
   coords.y = height - coords.y;
#ifdef MIRRORED_PROJECTION
  // ST OUEN horizontal mirror
  //coords.y = height - coords.y;
#endif
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // mute screen
/*  if(mute_second_screen && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }
*/
  // interface
/*  if(interfaceOnScreen && decalCoords.x < 540 && decalCoords.y < 100) {
    if(decalCoords.x < 100) {
      outColor0 = vec4(uniform_Master_fs_4fv_interpolatedPaletteLow_rgb_currentScene.rgb, 1);
    }
    else if(decalCoords.x < 200) {
      outColor0 = vec4(uniform_Master_fs_4fv_interpolatedPaletteMedium_rgb_mobile_cursor.rgb, 1);
    }
    else if(decalCoords.x < 300) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb, 1);
    }
    else if(decalCoords.x > 320 && decalCoords.x < 420) {
      outColor0 = vec4(vec3(uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey.w), 1);
    }
    else if(decalCoords.x > 440 && decalCoords.x < 540) {
      outColor0 = vec4(uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey.rgb, 1);
    }
    return;
  }
*/
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
mouse_x = width - mouse_x;
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
  float timeFromStart = uniform_Master_fs_3fv_width_height_timeFromStart.z;
  // transition invvert début frozen clear
#ifdef FULL_VERSION
  if(timeFromStart > 1020 && timeFromStart < 1024) {
    outColor0.rgb = clamp(outColor0.rgb + vec3((2 - abs(timeFromStart - 1022))/2.0), 0.0, 1.0);
  }
#endif
  // pendant frozen clear à 17'19" du début et à 19" debut frozen clea
  // événement musical
  /*
  else if(timeFromStart > 1035 && timeFromStart < 1043) {
    outColor0.rgb = clamp(outColor0.rgb + vec3(4 - abs(timeFromStart - 1039)), 0.0, 1.0);
  }
  */
  // transition invvert début ragnarok
#ifdef FULL_VERSION
  else if(timeFromStart > 1436 && timeFromStart < 1444) {
    outColor0.rgb = clamp(outColor0.rgb + vec3((4 - abs(timeFromStart - 1440))/4.0), 0.0, 1.0);
  }
#else
  if(timeFromStart > 596 && timeFromStart < 604) {
    outColor0.rgb = clamp(outColor0.rgb + vec3((4 - abs(timeFromStart - 600))/4.0), 0.0, 1.0);
  }
#endif
  /*
  // central circle
  int  radiusCircle;
#ifndef ROTATED_PROJECTION
  if( coordX > width) {
    coordX -= width;
  }
  radiusCircle = int(length(vec2(coordX - 700 , coords.y - 525)));
#else
  if( coords.y > width) {
    coords.y -= width;
  }
  radiusCircle = int(length(vec2(coords.x - 525 , coords.y - 525)));
#endif

  if( radiusCircle > 525 ) {
    outColor0.rgb = vec3(0);
  }
#ifdef WHITE_CIRCLE
   else if( radiusCircle <= 518 && radiusCircle >= 513 ) {
    outColor0.rgb = vec3(1);
  }
#endif
  else if( radiusCircle > 510 ) {
    outColor0.rgb *= ((525 - radiusCircle)/15.0);
  }
  */

  ////////////////////////////////////////////////////////////////////
  // image mask
  vec2 ratioed_scale = vec2(master_mask_scale, master_mask_scale * master_mask_scale_ratio);
  vec2 coordsWRTcenter = (initialCoords - centerCoords) / ratioed_scale;
  vec4 maskColor;
/*  if(currentScene >= 0) {*/
    maskColor = texture(uniform_Master_texture_fs_Mask, 
          coordsWRTcenter + (centerCoords + vec2(master_mask_offsetX, master_mask_offsetY) / ratioed_scale));
    outColor0.rgb *= maskColor.r;
/*  }
*/

  // cursor
#ifndef ROTATED_PROJECTION
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coordX - mouse_x , height - coords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
#else
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
#endif

  outColor0.rgb *= master;
}
