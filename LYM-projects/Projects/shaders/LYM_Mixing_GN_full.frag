/***********************************************************************
File: alKemi/shaders/LYM_Mixing_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 1

float     CAMixingWeight;
float     trackMixingWeight_0;
float     echo;
float     echoNeg;
uniform vec4 uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg;

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec2 uniform_Mixing_fs_2fv_height_flashCameraTrkWght;
uniform vec3 uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Mixing_texture_fs_CA;  // 3-cycle ping-pong CA step n + 1 (FBO attachment 3)
layout (binding = 1) uniform samplerRect uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot for echo
// FONT
layout (binding = 2) uniform samplerRect uniform_Mixing_texture_fs_Screen_Font;
layout (binding = 3) uniform samplerRect uniform_Mixing_texture_fs_Screen_Message;
// pos FBO
layout (binding = 4) uniform samplerRect uniform_Mixing_texture_fs_Trk0; // 3-cycle ping-pong localColor step n + 1 (FBO attachment 1)
#if PG_NB_TRACKS >= 2
layout (binding = 5) uniform samplerRect uniform_Mixing_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 6) uniform samplerRect uniform_Mixing_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n + 1 (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 7) uniform samplerRect uniform_Mixing_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n + 1 (FBO attachment 6)
#endif

// layout (binding = 5) uniform samplerRect fs_lookupTable5;  // captured frame texture

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  CAMixingWeight = uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg[0];
  trackMixingWeight_0 = uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg[1];
  echo = uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg[2];
  echoNeg = uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg[3];

  float height = uniform_Mixing_fs_2fv_height_flashCameraTrkWght.x;

  vec4 track0_color = texture(uniform_Mixing_texture_fs_Trk0, decalCoords);
  vec4 CA_color = texture(uniform_Mixing_texture_fs_CA, decalCoords);
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(uniform_Mixing_texture_fs_Trk1, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(uniform_Mixing_texture_fs_Trk2, decalCoords);
#endif
  vec3 localColor
    = vec3(track0_color.rgb) * trackMixingWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMixingWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMixingWeight_2
#endif
    + CA_color.rgb * CAMixingWeight
    ;

  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }

  ////////////////////////////////////////////////////////////////////
  // drawing / GOL interpolated combination: should be parameterized
  // drawing and GOL combination
  // should be reworked
  vec3 combined_color 
    = clamp( localColor , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation
  vec4 previous_snapshot_color 
       = texture(uniform_Mixing_texture_fs_Render_prec, decalCoords);

  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * (1.0 - echoNeg) + echo * previous_snapshot_color.rgb , 
                            0.0 , 1.0 );
  }

/*   combined_color.rgb 
      = combined_color.rgb 
      + uniform_Mixing_fs_4fv_trackweight_FBCaptureWeight.w * texture(fs_lookupTable5, decalCoords).rgb;
 */

  ////////////////////////////////////
  // MESSAGE
  if( decalCoords.x < 128.0 && decalCoords.y < height && decalCoords.y > height - 14 
		&& uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha.x > 0 ) {
    vec2 locDecalCoord = vec2(decalCoords.x ,  height - decalCoords.y);
    vec4 offset = texture(uniform_Mixing_texture_fs_Screen_Message, vec2(locDecalCoord.x,0) ) * 255.0;
    vec3 fontC = vec3(0,0,0);
  	vec3 messageColor = vec3(0,0,0);
    if( locDecalCoord.y > offset.w && locDecalCoord.y < offset.w + offset.z ) {
      fontC = texture(uniform_Mixing_texture_fs_Screen_Font, vec2(offset.x, locDecalCoord.y - offset.w + offset.y)).rgb;
    }

    if( offset.w > 0 ) { // non nul height
      messageColor = fontC;
  	}
  	else  {
  	  messageColor = vec3(0,0,0);
  	}
  	combined_color = mix( combined_color ,  messageColor , 
  	   uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha.x );
  }

  outColor0 = vec4(combined_color , 1.0);
}
