/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 4

#define var_master_crop_width
float	 master_crop_width;
#define var_master_crop_x
float	 master_crop_x;
#define var_master_crop_y
float	 master_crop_y;
#define var_master_mask
float	 master_mask;
#define var_master_mask_offsetX
float	 master_mask_offsetX;
#define var_master_mask_offsetY
float	 master_mask_offsetY;
#define var_master_mask_opacity
float	 master_mask_opacity[7];
#define var_master_mask_scale
float	 master_mask_scale;
#define var_master_mask_scale_ratio
float	 master_mask_scale_ratio;
#define var_master_offsetX
float	 master_offsetX;
#define var_master_offsetY
float	 master_offsetY;
#define var_master_scale
float	 master_scale;
#define var_master_scale_pulse
float	 master_scale_pulse;
#define var_master_scale_ratio
float	 master_scale_ratio;
#define var_cursorSize
int		cursorSize;
#define var_invertAllLayers
bool	  invertAllLayers;
#define var_master
float	 master;
#define var_mute_second_screen
bool	  mute_second_screen;
#define var_CAMasterWeight
float	 CAMasterWeight;
#define var_ClipArtMasterWeight
float	 ClipArtMasterWeight;
#define var_PartMasterWeight
float	 PartMasterWeight;
#define var_SecondMasterMixingWeight
float	 SecondMasterMixingWeight;
#define var_trackMasterWeight
float	 trackMasterWeight[4];
#define var_currentMaskTrack
int		currentMaskTrack;
uniform float uniform_Master_scenario_var_data[32];

#define graylevel(col) ((col.r+col.g+col.b)/3.0)

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Master_texture_fs_Render_curr; // Master pass output with possible echo
layout (binding = 1) uniform samplerRect uniform_Master_texture_fs_CA;  // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_ClipArt_render;  // Particle step n
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Particle_render;  // Particle step n
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Update pass BG track step n (FBO attachment 3)
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
layout (binding = 8) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;

#if defined(var_flashchange_BGcolor_freq)
 uniform vec3 uniform_Master_fs_3fv_BG_color_rgb;
#endif

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  master_crop_width = uniform_Master_scenario_var_data[0];
  master_crop_x = uniform_Master_scenario_var_data[1];
  master_crop_y = uniform_Master_scenario_var_data[2];
  master_mask = uniform_Master_scenario_var_data[3];
  master_mask_offsetX = uniform_Master_scenario_var_data[4];
  master_mask_offsetY = uniform_Master_scenario_var_data[5];
  master_mask_opacity[1] = (uniform_Master_scenario_var_data[6]);
  master_mask_opacity[2] = (uniform_Master_scenario_var_data[7]);
  master_mask_opacity[3] = (uniform_Master_scenario_var_data[8]);
  master_mask_opacity[4] = (uniform_Master_scenario_var_data[9]);
  master_mask_opacity[5] = (uniform_Master_scenario_var_data[10]);
  master_mask_opacity[6] = (uniform_Master_scenario_var_data[11]);
  master_mask_scale = uniform_Master_scenario_var_data[12];
  master_mask_scale_ratio = uniform_Master_scenario_var_data[13];
  master_offsetX = uniform_Master_scenario_var_data[14];
  master_offsetY = uniform_Master_scenario_var_data[15];
  master_scale = uniform_Master_scenario_var_data[16];
  master_scale_pulse = uniform_Master_scenario_var_data[17];
  master_scale_ratio = uniform_Master_scenario_var_data[18];
  cursorSize = int(uniform_Master_scenario_var_data[19]);
  invertAllLayers = (uniform_Master_scenario_var_data[20] > 0 ? true : false);
  master = uniform_Master_scenario_var_data[21];
  mute_second_screen = (uniform_Master_scenario_var_data[22] > 0 ? true : false);
  CAMasterWeight = uniform_Master_scenario_var_data[23];
  ClipArtMasterWeight = uniform_Master_scenario_var_data[24];
  PartMasterWeight = uniform_Master_scenario_var_data[25];
  SecondMasterMixingWeight = uniform_Master_scenario_var_data[26];
  trackMasterWeight[0] = (uniform_Master_scenario_var_data[27]);
  trackMasterWeight[1] = (uniform_Master_scenario_var_data[28]);
  trackMasterWeight[2] = (uniform_Master_scenario_var_data[29]);
  trackMasterWeight[3] = (uniform_Master_scenario_var_data[30]);
  currentMaskTrack = int(uniform_Master_scenario_var_data[31]);

  float width = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.x;
  float height = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);
  vec2 centerCoords = vec2(width/2, height/2);

  int currentScene = uniform_Master_fs_2iv_mobile_cursor_currentScene.y;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed layers according to Mixing weights
  vec4 MixingColor = texture(uniform_Master_texture_fs_Render_curr, coords );

  ////////////////////////////////////////////////////////////////////
  // non-echoed layers
  vec4 CA_color = texture(uniform_Master_texture_fs_CA, coords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
  vec4 ClipArt_color = texture(uniform_Master_texture_fs_ClipArt_render, coords);
  vec4 particle_color = texture(uniform_Master_texture_fs_Particle_render, coords);

  vec4 track0_color = texture(uniform_Master_texture_fs_Trk0, coords);
  vec4 track1_color = texture(uniform_Master_texture_fs_Trk1, coords);
  vec4 track2_color = texture(uniform_Master_texture_fs_Trk2, coords);
  vec4 track3_color = texture(uniform_Master_texture_fs_Trk3, coords);

  ////////////////////////////////////////////////////////////////////
  // mix of non echoed layers according to final weights
  // mask is used as a local change of the mix
  // a track is used to mask other ones: can be used to reveal a
  // track by drawing on another one
  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight[0]
    + vec3(track1_color.rgb) * trackMasterWeight[1]
    + vec3(track2_color.rgb) * trackMasterWeight[2]
    + CA_color.rgb * CAMasterWeight
    + particle_color.rgb * PartMasterWeight * particle_color.a
    ;
  // for track based composition, we do not display track 3 on the left screen, 
  // so that we car record independently what is produced above the video
  vec3 NonEchoedColorRight = NonEchoedColor;
  vec3 NonEchoedColorLeft = NonEchoedColor
    + vec3(track3_color.rgb) * trackMasterWeight[3];

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  if(decalCoords.x < width) {
    outColor0 
      = vec4( clamp( MixingColor.rgb + NonEchoedColorLeft , 0.0 , 1.0 ) , 1.0 );
  }
  else {
    // for track based composition, we do not display track 3 on the left screen, 
    // so that we car record independently what is produced above the video
    outColor0 
      = vec4( clamp( MixingColor.rgb + NonEchoedColorRight , 0.0 , 1.0 ) , 1.0 );
  }
  // masking

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse (except for hide)
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;

  // vertical mirror
  //  coords.y = height - coords.y;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && decalCoords.x < width
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  // master level
  outColor0.rgb *= master;
}
