/***********************************************************************
File: effe/shaders/LYM_Final_effe-FS.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 3

float     master;
bool      invertAllLayers;
int       cursorSize;
float     CAFinalWeight;
uniform vec4 uniform_Final_fs_4fv_master_invertAllLayers_cursorSize_CAFinalWeight;
float     trackFinalWeight_0;
float     trackFinalWeight_1;
float     trackFinalWeight_2;
uniform vec3 uniform_Final_fs_3fv_trackFinalWeight_0_trackFinalWeight_1_trackFinalWeight_2;

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // Final pass output with possible echo
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // 2-cycle ping-pong Drawing pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // 2-cycle ping-pong Drawing pass track 3 step n (FBO attachment 6)
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Final_fs_3fv_xy_frameno;
uniform vec2 uniform_Final_fs_2fv_width_height;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;


void main() {
  master = uniform_Final_fs_4fv_master_invertAllLayers_cursorSize_CAFinalWeight[0];
  invertAllLayers = (uniform_Final_fs_4fv_master_invertAllLayers_cursorSize_CAFinalWeight[1] > 0 ? true : false);
  cursorSize = int(uniform_Final_fs_4fv_master_invertAllLayers_cursorSize_CAFinalWeight[2]);
  CAFinalWeight = uniform_Final_fs_4fv_master_invertAllLayers_cursorSize_CAFinalWeight[3];
  trackFinalWeight_0 = uniform_Final_fs_3fv_trackFinalWeight_0_trackFinalWeight_1_trackFinalWeight_2[0];
  trackFinalWeight_1 = uniform_Final_fs_3fv_trackFinalWeight_0_trackFinalWeight_1_trackFinalWeight_2[1];
  trackFinalWeight_2 = uniform_Final_fs_3fv_trackFinalWeight_0_trackFinalWeight_1_trackFinalWeight_2[2];

  float width = uniform_Final_fs_2fv_width_height.x;
  float height = uniform_Final_fs_2fv_width_height.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);

  ////////////////////////////////////////////////////////////////////
  // mix of echoed layers according to composition weights
  vec4 CompositionColor = texture(fs_decal, coords );

  ////////////////////////////////////////////////////////////////////
  // non-echoed layers
  vec4 bg_color = texture(fs_lookupTable1, coords);
  vec4 CA_color = texture(fs_lookupTable2, coords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(fs_lookupTable3, coords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(fs_lookupTable4, coords);
#endif
#if PG_NB_TRACKS >= 4
  vec4 track3_color = texture(fs_lookupTable5, coords);
#endif

  ////////////////////////////////////////////////////////////////////
  // mix of non echoed layers according to final weights
  vec3 localColor
    = vec3(bg_color.rgb) * trackFinalWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackFinalWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackFinalWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackFinalWeight_3
#endif
    +  CAFinalWeight * CA_color.rgb 
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( CompositionColor.rgb + localColor , 0.0 , 1.0 ) , 1.0 );

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse
  float mouse_x = uniform_Final_fs_3fv_xy_frameno.x;
  float mouse_y = uniform_Final_fs_3fv_xy_frameno.y;
  float frameno = uniform_Final_fs_3fv_xy_frameno.z;
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
