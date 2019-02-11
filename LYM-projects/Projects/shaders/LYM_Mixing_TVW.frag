/***********************************************************************
File: effe/shaders/LYM_Composition_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 2
#define PG_WITH_CA

#define graylevel(col) ((col.r+col.g+col.b)/3.0)

#include_declarations

// Main shader

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec2 uniform_Mixing_fs_2fv_height_flashCameraTrkWght;
uniform vec3 uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

// uniform vec4 uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha;
// uniform vec2 uniform_Composition_fs_2fv_flashCameraTrkWght_flashVideoThreshold;


/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Mixing_texture_fs_CA; // 2-cycle ping-pong Update pass CA step n + 1 (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Mixing_texture_fs_Part_render; // Particles step n
layout (binding = 2) uniform samplerRect uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot for echo
// FONT
layout (binding = 3) uniform samplerRect uniform_Mixing_texture_fs_Screen_Font;
// SCREEN MESSAGE
layout (binding = 4) uniform samplerRect uniform_Mixing_texture_fs_Screen_Message;

// FONT
layout (binding = 3) uniform samplerRect uniform_Mixing_texture_fs_Display_Font;
// DISPLAY MESSAGE
layout (binding = 5) uniform samplerRect uniform_Mixing_texture_fs_Display_Message1;
layout (binding = 6) uniform samplerRect uniform_Mixing_texture_fs_Display_Message2;

// pos FBO
layout (binding = 7) uniform samplerRect uniform_Mixing_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 track step n + 1 (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 8) uniform samplerRect uniform_Mixing_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 9) uniform samplerRect uniform_Mixing_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n + 1 (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 10) uniform samplerRect uniform_Mixing_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n + 1 (FBO attachment 6)
#endif

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  float height = uniform_Mixing_fs_2fv_height_flashCameraTrkWght.x;

  vec4 bg_color = texture(uniform_Mixing_texture_fs_Trk0, decalCoords);
#ifdef PG_WITH_CA
  vec4 CA_color = texture(uniform_Mixing_texture_fs_CA, decalCoords);
#endif

  vec4 particle_color = texture(uniform_Mixing_texture_fs_Part_render, decalCoords);

#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(uniform_Mixing_texture_fs_Trk1, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(uniform_Mixing_texture_fs_Trk2, decalCoords);
#endif
  vec3 localColor
    = vec3(bg_color.rgb) * trackMixingWeight_0;
#if PG_NB_TRACKS >= 2
  if(graylevel(localColor) == 0) {
    localColor += vec3(track1_color.rgb) * trackMixingWeight_1;
  }
#endif
#if PG_NB_TRACKS >= 3
  localColor += vec3(track2_color.rgb) * trackMixingWeight_2;
#endif
    ;

  ////////////////////////////////////////////////////////////////////
  // Sobel on track 1 color
  // float kernelX = -3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,1)))
  //                 + 3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,1)))
  //                 + -10 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,0)))
  //                 + 10 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,0)))
  //                 + -3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,-1)))
  //                 + 3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,-1)));
  // float kernelY = -3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,1)))
  //                 + 3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,-1)))
  //                 + -10 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(0,1)))
  //                 + 10 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(0,-1)))
  //                 + -3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,1)))
  //                 + 3 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,-1)));
  // float kernelX = -1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,1)))
  //                 + 1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,1)))
  //                 + -2 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,0)))
  //                 + 2 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,0)))
  //                 + -1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,-1)))
  //                 + 1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,-1)));
  // float kernelY = -1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,1)))
  //                 + 1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(-1,-1)))
  //                 + -2 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(0,1)))
  //                 + 2 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(0,-1)))
  //                 + -1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,1)))
  //                 + 1 * graylevel(texture(uniform_Mixing_texture_fs_Trk1, decalCoords + vec2(1,-1)));
  // float gradient = sqrt( kernelX * kernelX + kernelY * kernelY);
  // if( gradient > sobel_threshold ) {
  //   localColor = vec3(1)-localColor;
  // }

#ifdef PG_WITH_CA
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
#endif

  ////////////////////////////////////////////////////////////////////
  // drawing / GOL interpolated combination: should be parameterized
  // drawing and GOL combination

#ifdef PG_WITH_CA
      // brigher CA at the beginning of a flash
      float flashCameraTrkWght = uniform_Mixing_fs_2fv_height_flashCameraTrkWght.y;
      // float flashVideoThreshold = uniform_Composition_fs_2fv_flashCameraTrkWght_flashVideoThreshold.y;
      // if( flashCameraTrkWght > flashVideoThreshold / 2 && flashCameraTrkWght < flashVideoThreshold ) { // flash video
      //   CAMixingWeight += (1.0 - CAMixingWeight) * (flashCameraTrkWght - flashVideoThreshold / 2) / (flashVideoThreshold / 2);
      // }
      if( flashCameraTrkWght > 0 ) { // flash video
        if(flashCameraTrkWght < 0.3) {
          CAMixingWeight += (1.0 - CAMixingWeight) * flashCameraTrkWght / 0.3;          
        }
        else {
          CAMixingWeight = 1.0;          
        }
      }
      // if( flashCameraTrkWght > 0 ) { // flash video
      //   CAMixingWeight = 1.0;
      // }
#endif

  vec3 combined_color 
    = clamp( localColor 
#ifdef PG_WITH_CA
	     +  CAMixingWeight * CA_color.rgb
#endif
       + particle_color.rgb * PartMixingWeight * particle_color.a
      , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation
  vec4 previous_snapshot_color 
       = texture(uniform_Mixing_texture_fs_Render_prec, decalCoords);
  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * echo + (1.0  - echoNeg) * previous_snapshot_color.rgb , 
		 0.0 , 1.0 );
  }
     
  ////////////////////////////////////
  // SMALL SYSTEM MESSAGE
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

  ////////////////////////////////////
  // LARGE DISPLAY MESSAGE
  float alphaText1 = uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha.y;
  float alphaText2 = uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha.z;
  if( ( alphaText1 > 0 || alphaText2 > 0 ) && decalCoords.y < 60 ) { // && displayMsgPosition >= 0 ) {
    combined_color *= 0.5;
    vec2 locDecalCoord = vec2(decalCoords.x , 45 - decalCoords.y);
    vec4 offset1 = texture(uniform_Mixing_texture_fs_Display_Message1, vec2(locDecalCoord.x,0) ) * 255.0;
    vec4 offset2 = texture(uniform_Mixing_texture_fs_Display_Message2, vec2(locDecalCoord.x,0) ) * 255.0;
    // offset == (s texture coordinate, t texture coordinates, height and vertical offset)
    vec3 fontC1 = vec3(0,0,0);
    vec3 fontC2 = vec3(0,0,0);
    if( locDecalCoord.y > offset1.w && locDecalCoord.y < offset1.w + offset1.z && locDecalCoord.x >= 0 ) {
       fontC1 = texture(uniform_Mixing_texture_fs_Display_Font, vec2(offset1.x , locDecalCoord.y - offset1.w + offset1.y)).rgb;
    }
    if( locDecalCoord.y > offset2.w && locDecalCoord.y < offset2.w + offset2.z  && locDecalCoord.x >= 0 ) {
      fontC2 = texture(uniform_Mixing_texture_fs_Display_Font, vec2(offset2.x , locDecalCoord.y - offset2.w + offset2.y)).rgb;
    }

    // combined_color = vec3(0,0,0);
    if( offset1.w > 0 ) { // non nul height (null height is only for space character)
      combined_color = mix( combined_color , fontC1 , alphaText1 * fontC1.r );
    }
    if( offset2.w > 0 ) { // non nul height (null height is only for space character)
      combined_color = mix( combined_color , fontC2 , alphaText2 * fontC2.r );
    }
    outColor0 = vec4(combined_color , 1.0);
  }
  else {
    outColor0 = vec4(combined_color , 1.0);
    // outColor0 = outColor0*0.001 + texture(uniform_Mixing_texture_fs_CA, decalCoords);
    // outColor0.rgb = outColor0.rgb *0.01 + 0.99 * vec3(1);
  }
 //outColor0.rgb = outColor0.rgb *0.01 + 0.99 * vec3(0);
}
