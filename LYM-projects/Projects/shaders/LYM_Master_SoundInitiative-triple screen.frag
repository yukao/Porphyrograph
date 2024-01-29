/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 3
// #define ATELIERS_PORTATIFS
#define CAVERNEPLATON

#include_declarations

#define graylevel(col) ((col.r+col.g+col.b)/3.0)

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
uniform vec4 uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;


uniform vec3 uniform_Master_fs_3fv_Caverne_BackColor_rgb;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
#include_initializations

  float width = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.x;
  float height = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.y;
  float pulsed_shift = uniform_Master_fs_4fv_xy_frameno_pulsedShift.w;
  vec2 coords = vec2(decalCoords.x / 0.66666666666666666666666, decalCoords.y);
  if(coords.x > width * 0.66666666666666666666666) {
    coords.x = width * 1.3333333333333333 - coords.x;
  }
/*  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);*/
  // NEMOURS: overlay between two videoprojectors
  vec2 initialCoords = coords;

  int currentScene = uniform_Master_fs_2iv_mobile_cursor_currentScene.y;

  // vertical mirror
  // coords.y = height - coords.y;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // mute screen
  if(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.w && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }

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

  // first screen
  if(decalCoords.x < width/2) {
    ////////////////////////////////////////////////////////////////////
    // mix of non echoed layers according to final weights
    vec3 NonEchoedColor = vec3(0);
    NonEchoedColor
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
      + particle_color.rgb * PartMasterWeight * particle_color.a;
    ////////////////////////////////////////////////////////////////////
    // mix of echoed and non-echoed layer mixes
    // and clamping
    outColor0 
      = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );
  }
  // second screen
  else {
    ////////////////////////////////////////////////////////////////////
    // mix of non echoed layers according to final weights
    vec3 NonEchoedColor = vec3(0);
    NonEchoedColor
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
      + particle_color.rgb * PartMasterWeight * particle_color.a;
    ////////////////////////////////////////////////////////////////////
    // mix of echoed and non-echoed layer mixes
    // and clamping
    outColor0 
      = vec4( clamp( SecondMasterMixingWeight * MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );
  }


  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
  vec4 maskColor_1;
  maskColor_1 = texture(uniform_Master_texture_fs_Mask, coords);
  outColor0.rgb *= maskColor_1.r * master_mask_opacity_1;

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
//  coords.y = height - coords.y;
// double mirror
//   coords.y = height - coords.y;
//   coords.x = width - coords.x;

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  // NEMOURS: overlay between two videoprojectors
  // outColor0.rgb *= master * alpha;
  outColor0.rgb *= master;
}
