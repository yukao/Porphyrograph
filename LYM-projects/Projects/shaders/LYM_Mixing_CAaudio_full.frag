/***********************************************************************
File: song/shaders/LYM_Mixing_song-FS.glsl

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 1
#define ATELIERS_PORTATIFS

float     CAMixingWeight;
float     PartMixingWeight;
float     trackMixingWeight_0;
float     trackMixingWeight_1;
float     trackMixingWeight_2;
float     trackMixingWeight_3;
float     echo;
float     echoNeg;
uniform float uniform_Mixing_scenario_var_data[8];

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
layout (binding = 0) uniform samplerRect uniform_Mixing_texture_fs_CA; // 2-cycle ping-pong Update pass CA step n + 1 (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Mixing_texture_fs_Part_render; // Particles step n
layout (binding = 2) uniform samplerRect uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot for echo
// FONT
layout (binding = 3) uniform samplerRect uniform_Mixing_texture_fs_Screen_Font;
layout (binding = 4) uniform samplerRect uniform_Mixing_texture_fs_Screen_Message;
// pos FBO
layout (binding = 5) uniform samplerRect uniform_Mixing_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 track step n + 1 (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 6) uniform samplerRect uniform_Mixing_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 7) uniform samplerRect uniform_Mixing_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n + 1 (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 8) uniform samplerRect uniform_Mixing_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n + 1 (FBO attachment 6)
#endif

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  CAMixingWeight = uniform_Mixing_scenario_var_data[0];
  PartMixingWeight = uniform_Mixing_scenario_var_data[1];
  trackMixingWeight_0 = uniform_Mixing_scenario_var_data[2];
  trackMixingWeight_1 = uniform_Mixing_scenario_var_data[3];
  trackMixingWeight_2 = uniform_Mixing_scenario_var_data[4];
  trackMixingWeight_3 = uniform_Mixing_scenario_var_data[5];
  echo = uniform_Mixing_scenario_var_data[6];
  echoNeg = uniform_Mixing_scenario_var_data[7];

  float height = uniform_Mixing_fs_2fv_height_flashCameraTrkWght.x;


  ////////////////////////////////////////////////////////////////////
  // CA color according to CA state and possible flash video
  vec4 CA_color = texture(uniform_Mixing_texture_fs_CA, decalCoords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }

  // brigher CA at the beginning of a flash
  float flashCameraCoef = uniform_Mixing_fs_2fv_height_flashCameraTrkWght.y;
  if( flashCameraCoef > 0 ) { // flash video
    if(flashCameraCoef < 0.3) {
      CAMixingWeight += (1.0 - CAMixingWeight) * flashCameraCoef / 0.3;          
    }
    else {
      CAMixingWeight = 1.0;          
    }
  }

  vec4 particle_color = texture(uniform_Mixing_texture_fs_Part_render, decalCoords);
 
   vec4 track0_color = texture(uniform_Mixing_texture_fs_Trk0, decalCoords);
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(uniform_Mixing_texture_fs_Trk1, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(uniform_Mixing_texture_fs_Trk2, decalCoords);
#endif
#if PG_NB_TRACKS >= 4
  vec4 track3_color = texture(uniform_Mixing_texture_fs_Trk3, decalCoords);
#endif

  ////////////////////////////////////////////////////////////////////
  // mix of current layers according to composition weights
  vec3 localColor
    = vec3(track0_color.rgb) * trackMixingWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMixingWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMixingWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMixingWeight_3
#endif
    + CA_color.rgb * CAMixingWeight
    + particle_color.rgb * PartMixingWeight * particle_color.a
    ;

  // clamping
  vec3 combined_color = clamp( localColor , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation: mix of current layers and echo
  vec4 previous_snapshot_color = texture(uniform_Mixing_texture_fs_Render_prec, decalCoords);

  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * (1.0 - echoNeg) + echo * previous_snapshot_color.rgb , 
		                        0.0 , 1.0 );
  }
     
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
