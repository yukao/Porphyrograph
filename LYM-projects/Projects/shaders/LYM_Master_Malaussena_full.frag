/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 1
#define ATELIERS_PORTATIFS

float     blendTransp;
bool      invertAllLayers;
int       cursorSize;
float     CAMasterWeight;
uniform vec4 uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight;
float     PartMasterWeight;
float     trackMasterWeight_0;
float     trackMasterWeight_1;
float     trackMasterWeight_2;
uniform vec4 uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2;
float     trackMasterWeight_3;
bool      hide;
bool      mute_screen;
uniform vec3 uniform_Master_fs_3fv_trackMasterWeight_3_hide_mute_screen;

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
uniform vec3 uniform_Master_fs_3fv_width_height_rightWindowVMargin;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
  blendTransp = uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[0];
  invertAllLayers = (uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[1] > 0 ? true : false);
  cursorSize = int(uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[2]);
  CAMasterWeight = uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[3];
  PartMasterWeight = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[0];
  trackMasterWeight_0 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[1];
  trackMasterWeight_1 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[2];
  trackMasterWeight_2 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[3];
  trackMasterWeight_3 = uniform_Master_fs_3fv_trackMasterWeight_3_hide_mute_screen[0];
  hide = (uniform_Master_fs_3fv_trackMasterWeight_3_hide_mute_screen[1] > 0 ? true : false);
  mute_screen = (uniform_Master_fs_3fv_trackMasterWeight_3_hide_mute_screen[2] > 0 ? true : false);

  float width = uniform_Master_fs_3fv_width_height_rightWindowVMargin.x;
  float height = uniform_Master_fs_3fv_width_height_rightWindowVMargin.y;
#ifdef ATELIERS_PORTATIFS
  float pulsed_shift = uniform_Master_fs_4fv_xy_frameno_pulsedShift.w;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) + pulsed_shift, 
                      decalCoords.y);
#else
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);
#endif

// vertical mirror
//  coords.y = height - coords.y;
// ST OUEN horizontal mirror
// coords.x = width - coords.x;
// double mirror
//   coords.y = height - coords.y;
//   coords.x = width - coords.x;

// mute screen
if(mute_screen && decalCoords.x > width) {
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
  if( coordX > width) {
    coordX -= width;
  }
  // comment for single cursor

// vertical mirror
//   coords.y = height - coords.y;
// ST OUEN horizontal mirror
// coordX = width - coordX;
// double mirror
//   coords.y = height - coords.y;
//   coords.x = width - coords.x;

  if( !hide
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coordX - mouse_x , height - coords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }
  // outColor0.rgb *= blendTransp;
}
