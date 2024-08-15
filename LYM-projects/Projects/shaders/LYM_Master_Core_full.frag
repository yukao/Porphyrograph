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
#if PG_NB_TRACKS >= 2
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS == 1
layout (binding = 5) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 2
layout (binding = 6) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 3
layout (binding = 7) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 4
layout (binding = 8) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;
uniform vec3 uniform_Master_fs_3fv_BGcolor_rgb;

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

  // mute screen
  if(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.w != 0
     && decalCoords.x > width) {
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
  vec4 ClipArt_color = texture(uniform_Master_texture_fs_ClipArt_render, coords);
  vec4 particle_color = texture(uniform_Master_texture_fs_Particle_render, coords);

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
  float maskGrey = 1.0;
#ifdef var_currentMaskTrack
  if (currentMaskTrack == 0) {
    trackMasterWeight[0] = 0; maskGrey = graylevel(track0_color.rgb); 
  }
#if PG_NB_TRACKS >= 2
  else if (currentMaskTrack == 1) {
    trackMasterWeight[1] = 0; maskGrey = graylevel(track1_color.rgb);
  }
#endif
#if PG_NB_TRACKS >= 3
  else if (currentMaskTrack == 2) {
    trackMasterWeight[2] = 0; maskGrey = graylevel(track2_color.rgb);
  }
#endif
#if PG_NB_TRACKS >= 4
  else if (currentMaskTrack == 3) {
    trackMasterWeight[3] = 0; maskGrey = graylevel(track3_color.rgb);
  }
#endif
#endif

  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight[0]
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMasterWeight[1]
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMasterWeight[2]
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMasterWeight[3]
#endif
    + CA_color.rgb * CAMasterWeight
#if defined(var_ClipArtMasterWeight)
    + ClipArt_color.rgb * ClipArtMasterWeight * ClipArt_color.a
#endif
#if defined(var_PartMasterWeight)
    + particle_color.rgb * PartMasterWeight * particle_color.a
#endif
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );
  if(uniform_Master_fs_3fv_BGcolor_rgb.r + uniform_Master_fs_3fv_BGcolor_rgb.g + uniform_Master_fs_3fv_BGcolor_rgb.b > 0) {
    if(graylevel(outColor0) < 0.3) {
      outColor0.rgb = clamp(uniform_Master_fs_3fv_BGcolor_rgb.rgb, 0, 1);
    }
    else {
      outColor0.rgb = vec3(1) - outColor0.rgb;
    }
  }

  // masking
#ifdef var_currentMaskTrack
  if (currentMaskTrack >= 0) {
   outColor0.rgb *= maskGrey;
  }
#endif

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
#if defined(var_master_mask_scale) && defined(var_master_mask_scale_ratio) && defined(var_master_offsetX) && defined(var_master_offsetY)
  vec2 ratioed_scale = vec2(master_mask_scale, master_mask_scale * master_mask_scale_ratio);
  vec2 coordsWRTcenter = (initialCoords - centerCoords) / ratioed_scale;
  vec2 scaled_coords = coordsWRTcenter + (centerCoords + vec2(master_offsetX, master_offsetY) / ratioed_scale);

  float maskColor = 0;
  for(int indLayer = 1 ; indLayer <= 6 ; indLayer++){
    if(master_mask_opacity[indLayer] > 0) {
      maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
        0.0833333333 + (indLayer - 1) * 0.1666666667)).g * master_mask_opacity[indLayer];
    }
  }
  maskColor = clamp(maskColor, 0, 1);
  outColor0.rgb *= maskColor;
#endif

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

  // master level
  outColor0.rgb *= master;

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
}
