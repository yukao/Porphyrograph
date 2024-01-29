/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 4
// #define ATELIERS_PORTATIFS
#define CAVERNEPLATON

#define var_mute_second_screen
bool	  mute_second_screen;
#define var_invertAllLayers
bool	  invertAllLayers;
#define var_cursorSize
int		cursorSize;
#define var_master
float	 master;
#define var_CAMasterWeight
float	 CAMasterWeight;
#define var_PartMasterWeight
float	 PartMasterWeight;
#define var_trackMasterWeight_0
float	 trackMasterWeight_0;
#define var_trackMasterWeight_1
float	 trackMasterWeight_1;
#define var_trackMasterWeight_2
float	 trackMasterWeight_2;
#define var_trackMasterWeight_3
float	 trackMasterWeight_3;
#define var_currentMaskTrack
int		currentMaskTrack;
#define var_master_scale
float	 master_scale;
#define var_master_scale_pulse
float	 master_scale_pulse;
#define var_master_scale_ratio
float	 master_scale_ratio;
#define var_master_offsetX
float	 master_offsetX;
#define var_master_offsetY
float	 master_offsetY;
#define var_master_mask_opacity_1
float	 master_mask_opacity_1;
#define var_master_mask_opacity_2
float	 master_mask_opacity_2;
#define var_master_mask_opacity_3
float	 master_mask_opacity_3;
#define var_master_mask_opacity_4
float	 master_mask_opacity_4;
#define var_master_mask_opacity_5
float	 master_mask_opacity_5;
#define var_master_crop_x
float	 master_crop_x;
#define var_master_crop_y
float	 master_crop_y;
#define var_master_crop_width
float	 master_crop_width;
#define var_master_mask
float	 master_mask;
#define var_master_mask_scale
float	 master_mask_scale;
#define var_master_mask_scale_ratio
float	 master_mask_scale_ratio;
#define var_master_mask_offsetX
float	 master_mask_offsetX;
#define var_master_mask_offsetY
float	 master_mask_offsetY;
#define var_interfaceOnScreen
bool	  interfaceOnScreen;
uniform float uniform_Master_scenario_var_data[30];

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
uniform vec4 uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;


uniform vec3 uniform_Master_fs_3fv_Caverne_BackColor_rgb;

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
  currentMaskTrack = int(uniform_Master_scenario_var_data[10]);
  master_scale = uniform_Master_scenario_var_data[11];
  master_scale_pulse = uniform_Master_scenario_var_data[12];
  master_scale_ratio = uniform_Master_scenario_var_data[13];
  master_offsetX = uniform_Master_scenario_var_data[14];
  master_offsetY = uniform_Master_scenario_var_data[15];
  master_mask_opacity_1 = uniform_Master_scenario_var_data[16];
  master_mask_opacity_2 = uniform_Master_scenario_var_data[17];
  master_mask_opacity_3 = uniform_Master_scenario_var_data[18];
  master_mask_opacity_4 = uniform_Master_scenario_var_data[19];
  master_mask_opacity_5 = uniform_Master_scenario_var_data[20];
  master_crop_x = uniform_Master_scenario_var_data[21];
  master_crop_y = uniform_Master_scenario_var_data[22];
  master_crop_width = uniform_Master_scenario_var_data[23];
  master_mask = uniform_Master_scenario_var_data[24];
  master_mask_scale = uniform_Master_scenario_var_data[25];
  master_mask_scale_ratio = uniform_Master_scenario_var_data[26];
  master_mask_offsetX = uniform_Master_scenario_var_data[27];
  master_mask_offsetY = uniform_Master_scenario_var_data[28];
  interfaceOnScreen = (uniform_Master_scenario_var_data[29] > 0 ? true : false);

  float width = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.x;
  float height = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);

  vec2 initialCoords = coords;
  vec2 centerCoords = vec2(width/2, height/2);

  int currentScene = uniform_Master_fs_2iv_mobile_cursor_currentScene.y;

  // vertical mirror
  // coords.y = height - coords.y;
  // BRON mirror
  // coords.x = width - coords.x;
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
  // mask is used as a local change of the mix
  // a track is used to mask other ones: can be used to reveal a
  // track by drawing on another one
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
  // masking

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
  vec4 maskColor_1;
  maskColor_1 = texture(uniform_Master_texture_fs_Mask, initialCoords);
  // outColor0.rgb *= maskColor_1.r;

  ////////////////////////////////////////////////////////////////////
  // back color for the battle
  if(uniform_Master_fs_3fv_Caverne_BackColor_rgb.r + uniform_Master_fs_3fv_Caverne_BackColor_rgb.g + uniform_Master_fs_3fv_Caverne_BackColor_rgb.b > 0) {
    if(graylevel(outColor0) < 0.3) {
      outColor0.rgb = clamp(uniform_Master_fs_3fv_Caverne_BackColor_rgb.rgb, 0, 1);
    }
    else {
      outColor0.rgb = vec3(1) - outColor0.rgb;
    }
  }

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
// BRON mirror
//  coordX = width - coordX;
// double mirror
//   coords.y = height - coords.y;
//   coords.x = width - coords.x;

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , (height - coords.y) - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  outColor0.rgb *= master;
}
