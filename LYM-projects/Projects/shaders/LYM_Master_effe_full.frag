/***********************************************************************
File: effe/shaders/LYM_Master_effe-FS.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 4

bool      mute_second_screen;
bool      invertAllLayers;
int       cursorSize;
float     master;
uniform vec4 uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master;
float     CAMasterWeight;
float     PartMasterWeight;
float     trackMasterWeight_0;
float     trackMasterWeight_1;
uniform vec4 uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1;
float     trackMasterWeight_2;
float     trackMasterWeight_3;
bool      interfaceOnScreen;
uniform vec3 uniform_Master_fs_3fv_trackMasterWeight_2_trackMasterWeight_3_interfaceOnScreen;

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Master_texture_fs_Render_curr; // Master pass output with possible echo
layout (binding = 1) uniform samplerRect uniform_Master_texture_fs_CA;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_Part_render;  // Particle step n
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
#if PG_NB_TRACKS >= 2
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Drawing pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Drawing pass track 3 step n (FBO attachment 6)
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;

uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
  mute_second_screen = (uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master[0] > 0 ? true : false);
  invertAllLayers = (uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master[1] > 0 ? true : false);
  cursorSize = int(uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master[2]);
  master = uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master[3];
  CAMasterWeight = uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1[0];
  PartMasterWeight = uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1[1];
  trackMasterWeight_0 = uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1[2];
  trackMasterWeight_1 = uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1[3];
  trackMasterWeight_2 = uniform_Master_fs_3fv_trackMasterWeight_2_trackMasterWeight_3_interfaceOnScreen[0];
  trackMasterWeight_3 = uniform_Master_fs_3fv_trackMasterWeight_2_trackMasterWeight_3_interfaceOnScreen[1];
  interfaceOnScreen = (uniform_Master_fs_3fv_trackMasterWeight_2_trackMasterWeight_3_interfaceOnScreen[2] > 0 ? true : false);

  float width = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.x;
  float height = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);

  if(mute_second_screen && decalCoords.x > width) {
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

  ////////////////////////////////////////////////////////////////////
  // mix of echoed layers according to composition weights
  vec4 CompositionColor = texture(uniform_Master_texture_fs_Render_curr, coords );

  ////////////////////////////////////////////////////////////////////
  // non-echoed layers
  vec4 bg_color = texture(uniform_Master_texture_fs_Trk0, coords);
  vec4 CA_color = texture(uniform_Master_texture_fs_CA, coords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
  vec4 particle_color = texture(uniform_Master_texture_fs_Part_render, coords);
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
  vec3 localColor
    = clamp( vec3(bg_color.rgb) * trackMasterWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMasterWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMasterWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMasterWeight_3
#endif
    +  CAMasterWeight * CA_color.rgb 
    + particle_color.rgb * PartMasterWeight * particle_color.a
      , 0.0 , 1.0 );
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( CompositionColor.rgb + localColor , 0.0 , 1.0 ) , 1.0 );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;
  if( mouse_x < width && mouse_x > 0 
      && length(vec2(decalCoords.x - mouse_x , height - decalCoords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }
  outColor0.rgb *= master;
}
