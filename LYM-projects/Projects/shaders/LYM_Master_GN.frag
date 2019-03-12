/***********************************************************************
File: alKemi/shaders/LYM_Master_GN.frag

LYM alKemi & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 1

#include_declarations

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Master_texture_fs_Render_curr; // previous pass output
layout (binding = 1) uniform samplerRect uniform_Master_texture_fs_CA;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Drawing pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Drawing pass track 3 step n (FBO attachment 6)
#endif
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_LYMlogo;  // LYM logo

/////////////////////////////////////
// UNIFORMS
// passed by the C program
// uniform vec4 uniform_Master_fs_4fv_transparency_scale_leftwidth_rightVMargin;

uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;

uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  vec2 coords = decalCoords;
  float leftWindowWidth = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.x;
  float rightWindowVMargin = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.z;
  float height = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.y;

  if( coords.x > leftWindowWidth ) {
    if( coords.x < leftWindowWidth + rightWindowVMargin
        || coords.x > 2 * leftWindowWidth + rightWindowVMargin ) {
      outColor0 = vec4(0,0,0,1);
      return;
    }
    coords.x = coords.x - (leftWindowWidth + rightWindowVMargin);
  }
  
  //   // FX scaling
  // vec2 center = vec2(leftWindowWidth, height) / 2.0;
  // float scale = uniform_Master_fs_4fv_transparency_scale_leftwidth_rightVMargin.y;
  // vec2 scaled_decalCoords 
  //   = (coords - center) / scale + center;
  // scaled_decalCoords = floor( scaled_decalCoords );

  // FX pixellization
  //int nbPixels = int(scale); 
  // scaled_decalCoords = float(nbPixels) * vec2(ivec2(scaled_decalCoords) / nbPixels);

  // mute screen
  if(mute_second_screen && decalCoords.x > leftWindowWidth) {
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
  if( mouse_x < leftWindowWidth && mouse_x > 0 
      && length(vec2(decalCoords.x - mouse_x , height - decalCoords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  // logo in the end
  outColor0.rgb = mix( texture(uniform_Master_texture_fs_LYMlogo, coords ).rgb , outColor0.rgb , 
           master );
}
