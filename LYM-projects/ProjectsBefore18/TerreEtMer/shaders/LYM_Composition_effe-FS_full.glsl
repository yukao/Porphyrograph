/***********************************************************************
File: effe/shaders/LYM_Composition_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 4

float     echo;
float     echoNeg;
float     CACompositionWeight;
float     trackCompositionWeight_0;
uniform vec4 uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0;
float     trackCompositionWeight_1;
float     trackCompositionWeight_2;
float     trackCompositionWeight_3;
uniform vec3 uniform_Composition_fs_3fv_trackCompositionWeight_1_trackCompositionWeight_2_trackCompositionWeight_3;

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Composition_fs_3fv_width_height_messageTransparency;
uniform float uniform_Composition_fs_1fv_flashCameraCoef;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // 2-cycle ping-pong Drawing pass BG track step n + 1 (FBO attachment 1)
// pos FBO
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass CA step n + 1 (FBO attachment 2)
#if PG_NB_TRACKS >= 2
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass track 1 step n + 1 (FBO attachment 3)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 2 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // 2-cycle ping-pong Drawing pass track 3 step n + 1 (FBO attachment 5)
#endif
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // preceding snapshot for echo
// FONT
layout (binding = 6) uniform samplerRect fs_lookupTable_font;
layout (binding = 7) uniform samplerRect fs_lookupTable_message;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  echo = uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0[0];
  echoNeg = uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0[1];
  CACompositionWeight = uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0[2];
  trackCompositionWeight_0 = uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0[3];
  trackCompositionWeight_1 = uniform_Composition_fs_3fv_trackCompositionWeight_1_trackCompositionWeight_2_trackCompositionWeight_3[0];
  trackCompositionWeight_2 = uniform_Composition_fs_3fv_trackCompositionWeight_1_trackCompositionWeight_2_trackCompositionWeight_3[1];
  trackCompositionWeight_3 = uniform_Composition_fs_3fv_trackCompositionWeight_1_trackCompositionWeight_2_trackCompositionWeight_3[2];

  float height = uniform_Composition_fs_3fv_width_height_messageTransparency.y;

  vec4 bg_color = texture(fs_decal, decalCoords);

  ////////////////////////////////////////////////////////////////////
  // CA color according to CA state and possible flash video
  vec4 CA_color = texture(fs_lookupTable1, decalCoords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }

  // brigher CA at the beginning of a flash
  float flashCameraCoef = uniform_Composition_fs_1fv_flashCameraCoef.x;
  if( flashCameraCoef > 0 ) { // flash video
    if(flashCameraCoef < 0.3) {
      CACompositionWeight += (1.0 - CACompositionWeight) * flashCameraCoef / 0.3;          
    }
    else {
      CACompositionWeight = 1.0;          
    }
  }

#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(fs_lookupTable2, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(fs_lookupTable3, decalCoords);
#endif
#if PG_NB_TRACKS >= 4
  vec4 track3_color = texture(fs_lookupTable4, decalCoords);
#endif

  ////////////////////////////////////////////////////////////////////
  // mix of current layers according to composition weights
  vec3 localColor
    = vec3(bg_color.rgb) * trackCompositionWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackCompositionWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackCompositionWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackCompositionWeight_3
#endif
    +  CACompositionWeight * CA_color.rgb
    ;

  // clamping
  vec3 combined_color = clamp( localColor , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation: mix of current layers and echo
  vec4 previous_snapshot_color 
       = texture(fs_lookupTable5, decalCoords);
  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * echo + (1.0  - echoNeg) * previous_snapshot_color.rgb , 
		 0.0 , 1.0 );
  }
     
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
}
