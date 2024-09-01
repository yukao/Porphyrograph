/***********************************************************************
File: effe/shaders/LYM_Final_effe-FS.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

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

#define PG_NB_TRACKS 2
#define PG_WITH_CA

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

uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

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

  float width = uniform_Master_fs_3fv_width_height_rightWindowVMargin.x;
  float height = uniform_Master_fs_3fv_width_height_rightWindowVMargin.y;
  float margin = uniform_Master_fs_3fv_width_height_rightWindowVMargin.z;
  vec2 coords = vec2( (decalCoords.x >= width + margin ? decalCoords.x - width - margin : decalCoords.x) , 
                      decalCoords.y);
  vec4 CompositionAndTrackDisplayColor = texture(uniform_Master_texture_fs_Render_curr, coords );

  // mute
  if(mute_screen && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }
  // margin
  if(margin > 0 
      && (decalCoords.x >= 2 * width + margin 
          || (decalCoords.x >= width && decalCoords.x < width + margin))) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }

  // interface
  if(interfaceOnScreen && decalCoords.x < 540 && decalCoords.y < 100) {
    if(decalCoords.x < 100) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteLow_rgb, 1);
    }
    else if(decalCoords.x < 200) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb, 1);
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

#ifdef PG_WITH_CA
  vec4 CA_color = texture(uniform_Master_texture_fs_CA, coords);
#endif
  vec4 particle_color = texture(uniform_Master_texture_fs_Part_render, coords);

  vec4 bg_color = texture(uniform_Master_texture_fs_Trk0, coords);
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(uniform_Master_texture_fs_Trk1, coords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(uniform_Master_texture_fs_Trk2, coords);
#endif

  vec3 localColor
    = vec3(bg_color.rgb) * trackMasterWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMasterWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMasterWeight_2
#endif
    ;

#ifdef PG_WITH_CA
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
#endif

  ////////////////////////////////////////////////////////////////////
  // drawing / GOL interpolated combination: should be parameterized
  // drawing and GOL combination
  // should be reworked
  outColor0 
    = vec4( clamp( CompositionAndTrackDisplayColor.rgb + localColor 
#ifdef PG_WITH_CA
       +  CAMasterWeight * CA_color.rgb
#endif
    + particle_color.rgb * PartMasterWeight * particle_color.a
      , 0.0 , 1.0 ) , 1.0);

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;
   if( length(vec2(decalCoords.x - mouse_x , height - decalCoords.y - mouse_y)) 
      < 3 /* cursorSize */ ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  outColor0.rgb *= blendTransp;
/*  if( decalCoords.x > 100 || decalCoords.y < 660) {
    outColor0.rgb = mix( outColor0.rgb , vec3(0),0.99);
  }
*/}
