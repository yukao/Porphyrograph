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
uniform vec4 uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha;
uniform vec3 uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // 2-cycle ping-pong Drawing pass BG track step n + 1 (FBO attachment 1)
// pos FBO
#ifdef PG_WITH_CA
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass CA step n + 1 (FBO attachment 3) */
#endif
#if PG_NB_TRACKS >= 2
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass track 1 step n + 1 (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 2 step n + 1 (FBO attachment 5)
#endif
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // preceding snapshot for echo
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // captured frame texture
// SCREEN MESSAGE
layout (binding = 6) uniform samplerRect fs_lookupTable_screen_font;
layout (binding = 7) uniform samplerRect fs_lookupTable_screen_msg_offset;
// DISPLAY MESSAGE
layout (binding = 8) uniform samplerRect fs_lookupTable_displayMsg_font;
layout (binding = 9) uniform samplerRect fs_lookupTable_displayMsg1_offset;
layout (binding = 10) uniform samplerRect fs_lookupTable_displayMsg2_offset;
// DISPLAY MESSAGE
layout (binding = 11) uniform samplerRect fs_lookupTable_hashtag_font;
layout (binding = 12) uniform samplerRect fs_lookupTable_hashtag_offset;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  float height = uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha.x;

  vec4 bg_color = texture(fs_decal, decalCoords);
#ifdef PG_WITH_CA
  vec4 CA_color = texture(fs_lookupTable1, decalCoords);
#endif
#if PG_NB_TRACKS >= 2
  vec4 track1_color = texture(fs_lookupTable2, decalCoords);
#endif
#if PG_NB_TRACKS >= 3
  vec4 track2_color = texture(fs_lookupTable3, decalCoords);
#endif
  vec3 localColor
    = vec3(bg_color.rgb) * trackCompositionWeight_0;
#if PG_NB_TRACKS >= 2
  if(graylevel(localColor) == 0) {
    localColor += vec3(track1_color.rgb) * trackCompositionWeight_1;
  }
#endif
#if PG_NB_TRACKS >= 3
  localColor += vec3(track2_color.rgb) * trackCompositionWeight_2;
#endif
 
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
      float flashVideoCoef = uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width.x;
      float flashVideoThreshold = uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width.y;
      // if( flashVideoCoef > flashVideoThreshold / 2 && flashVideoCoef < flashVideoThreshold ) { // flash video
      //   CACompositionWeight += (1.0 - CACompositionWeight) * (flashVideoCoef - flashVideoThreshold / 2) / (flashVideoThreshold / 2);
      // }
      if( flashVideoCoef > 0 ) { // flash video
        if(flashVideoCoef < 0.3) {
          CACompositionWeight += (1.0 - CACompositionWeight) * flashVideoCoef / 0.3;          
        }
        else {
          CACompositionWeight = 1.0;          
        }
      }
      // if( flashVideoCoef > 0 ) { // flash video
      //   CACompositionWeight = 1.0;
      // }
#endif

  vec3 combined_color 
    = clamp( localColor 
#ifdef PG_WITH_CA
	     +  CACompositionWeight * CA_color.rgb
#endif
      , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation
  vec4 previous_snapshot_color 
       = texture(fs_lookupTable4, decalCoords);
  if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * echo + (1.0  - echoNeg) * previous_snapshot_color.rgb , 
		 0.0 , 1.0 );
  }
     
  combined_color.rgb 
      = combined_color.rgb + texture(fs_lookupTable5, decalCoords).rgb;

  // INSTALLATION: BLACK SCREEN ON THE RIGHT
  if( decalCoords.x > uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width.z / 2) {
    combined_color.rgb = vec3(0);
  }

  ////////////////////////////////////
  // SMALL SYSTEM MESSAGE
  if( decalCoords.x < 128.0 && decalCoords.y < height && decalCoords.y > height - 14 
    && uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha.y > 0 ) {
    vec2 locDecalCoord = vec2(decalCoords.x ,  height - decalCoords.y);
    vec4 offset = texture(fs_lookupTable_screen_msg_offset, vec2(locDecalCoord.x,0) ) * 255.0;
    vec3 fontC = vec3(0,0,0);
    vec3 messageColor = vec3(0,0,0);
    if( locDecalCoord.y > offset.w && locDecalCoord.y < offset.w + offset.z ) {
      fontC = texture(fs_lookupTable_screen_font, vec2(offset.x, locDecalCoord.y - offset.w + offset.y)).rgb;
    }

    if( offset.w > 0 ) { // non nul height
      messageColor = fontC;
    }
    else  {
      messageColor = vec3(0,0,0);
    }
    combined_color = mix( combined_color ,  messageColor , 
       uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha.y );
  }

  ////////////////////////////////////
  // LARGE DISPLAY MESSAGE
  float alphaText1 = uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha.z;
  float alphaText2 = uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha.w;
  if( ( alphaText1 > 0 || alphaText2 > 0 ) && decalCoords.y < 60 ) { // && displayMsgPosition >= 0 ) {
    if( decalCoords.x < uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width.z / 2) {
      combined_color *= 0.5;
    }
    else {
      combined_color += 0.1;
    }

    vec2 locDecalCoord = vec2(decalCoords.x , 45 - decalCoords.y);
    vec4 offset1 = texture(fs_lookupTable_displayMsg1_offset, vec2(locDecalCoord.x,0) ) * 255.0;
    vec4 offset2 = texture(fs_lookupTable_displayMsg2_offset, vec2(locDecalCoord.x,0) ) * 255.0;
    // offset == (s texture coordinate, t texture coordinates, height and vertical offset)
    vec3 fontC1 = vec3(0,0,0);
    vec3 fontC2 = vec3(0,0,0);
    if( locDecalCoord.y > offset1.w && locDecalCoord.y < offset1.w + offset1.z && locDecalCoord.x >= 0 ) {
       fontC1 = texture(fs_lookupTable_displayMsg_font, vec2(offset1.x , locDecalCoord.y - offset1.w + offset1.y)).rgb;
    }
    if( locDecalCoord.y > offset2.w && locDecalCoord.y < offset2.w + offset2.z  && locDecalCoord.x >= 0 ) {
      fontC2 = texture(fs_lookupTable_displayMsg_font, vec2(offset2.x , locDecalCoord.y - offset2.w + offset2.y)).rgb;
    }

    // combined_color = vec3(0,0,0);
    if( offset1.w > 0 ) { // non nul height (null height is only for space character)
      combined_color = mix( combined_color , fontC1 , alphaText1 * fontC1.r );
    }
    if( offset2.w > 0 ) { // non nul height (null height is only for space character)
      combined_color = mix( combined_color , fontC2 , alphaText2 * fontC2.r );
    }
  }

  ////////////////////////////////////
  // INSTALLATION: HASHTAGS
  if( decalCoords.y >= 515 && decalCoords.y <= 565 ) { // && displayMsgPosition >= 0 ) {
    vec2 locDecalCoord = vec2(decalCoords.x , 565 - decalCoords.y);
    vec4 offset = texture(fs_lookupTable_hashtag_offset, vec2(locDecalCoord.x, 0) ) * 255.0;
    vec4 color = texture(fs_lookupTable_hashtag_offset, vec2(locDecalCoord.x, 1) );
    // offset == (s texture coordinate, t texture coordinates, height )
    vec3 fontC = vec3(0,0,0);
    if( locDecalCoord.x >= 0 ) {
       fontC = texture(fs_lookupTable_hashtag_font, 
              vec2((offset.x + offset.y * 256), 
                    locDecalCoord.y + (offset.z + offset.w * 256))).rgb;
    }

    // combined_color = vec3(0,0,0);
    combined_color *= fontC;
    if( fontC.r < 1.0 ) { // the letters are black
      combined_color = mix(color.rgb,combined_color,fontC.r);
    }
  }


  outColor0 = vec4(combined_color , 1.0);
  // outColor0 = outColor0*0.001 + texture(fs_lookupTable1, decalCoords);
  // outColor0.rgb = outColor0.rgb *0.01 + 0.99 * vec3(1);
  //outColor0.rgb = outColor0.rgb *0.01 + 0.99 * vec3(0);
}
