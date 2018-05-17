/***********************************************************************
File: effe/shaders/LYM_Composition_core-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 3

#include_declarations

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Composition_fs_3fv_width_height_messageTransparency;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // 2-cycle ping-pong Drawing pass BG track step n + 1 (FBO attachment 1)
// pos FBO
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass CA step n + 1 (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass track 1 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 2 step n + 1 (FBO attachment 5)
#endif
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // preceding snapshot for echo
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // captured frame texture
// FONT
layout (binding = 6) uniform samplerRect fs_lookupTable_font;
layout (binding = 7) uniform samplerRect fs_lookupTable_message;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  float height = uniform_Composition_fs_3fv_width_height_messageTransparency.y;

  vec4 bg_color = texture(fs_decal, decalCoords);
  vec4 CA_color = texture(fs_lookupTable1, decalCoords);
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(fs_lookupTable2, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(fs_lookupTable3, decalCoords);
#endif
  vec3 localColor
    = vec3(bg_color.rgb) * trackCompositionWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackCompositionWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackCompositionWeight_2
#endif
    ;

  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }

  ////////////////////////////////////////////////////////////////////
  // drawing / GOL interpolated combination: should be parameterized
  // drawing and GOL combination
  // should be reworked
  vec3 combined_color 
    = clamp( localColor 
	     +  CACompositionWeight * CA_color.rgb , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation
  vec4 previous_snapshot_color 
       = texture(fs_lookupTable4, decalCoords);
  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * echo + (1.0  - echoNeg) * previous_snapshot_color.rgb , 
		 0.0 , 1.0 );
  }
     
  combined_color.rgb 
      = combined_color.rgb + FBCaptureWeight * texture(fs_lookupTable5, decalCoords).rgb;

////////////////////////////////////
  // MESSAGE
  if( decalCoords.x < 128.0 && decalCoords.y < height && decalCoords.y > height - 14 
		&& uniform_Composition_fs_3fv_width_height_messageTransparency.z > 0 ) {
    vec2 locDecalCoord = vec2(decalCoords.x ,  height - decalCoords.y);
    vec4 offset = texture(fs_lookupTable_message, vec2(locDecalCoord.x,0) ) * 255.0;
    vec3 fontC = vec3(0,0,0);
	vec3 messageColor = vec3(0,0,0);
    if( locDecalCoord.y > offset.w && locDecalCoord.y < offset.w + offset.z ) {
      fontC = texture(fs_lookupTable_font, vec2(offset.x, locDecalCoord.y - offset.w + offset.y)).rgb;
    }

    if( offset.w > 0 ) { // non nul height
      messageColor = fontC;
	}
	else  {
	  messageColor = vec3(0,0,0);
	}
	combined_color = mix( combined_color ,  messageColor , 
	   uniform_Composition_fs_3fv_width_height_messageTransparency.z );
  }

  outColor0 = vec4(combined_color , 1.0);
  // outColor0 = outColor0*0.001 + texture(fs_lookupTable1, decalCoords);
  // outColor0.rgb = outColor0.rgb *0.01 + 0.9 * bg_color.rgb;
}
