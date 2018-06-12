/***********************************************************************
File: effe/shaders/LYM_Final_effe-FS.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

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
bool      interfaceOnScreen;
bool      mute_screen;
uniform vec2 uniform_Master_fs_2fv_interfaceOnScreen_mute_screen;

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
  blendTransp = uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[0];
  invertAllLayers = (uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[1] > 0 ? true : false);
  cursorSize = int(uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[2]);
  CAMasterWeight = uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight[3];
  PartMasterWeight = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[0];
  trackMasterWeight_0 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[1];
  trackMasterWeight_1 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[2];
  trackMasterWeight_2 = uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2[3];
  interfaceOnScreen = (uniform_Master_fs_2fv_interfaceOnScreen_mute_screen[0] > 0 ? true : false);
  mute_screen = (uniform_Master_fs_2fv_interfaceOnScreen_mute_screen[1] > 0 ? true : false);

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
