/***********************************************************************
File: song/shaders/LYM_Mixing_song-FS.glsl

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 4

#include_declarations

// Main shader.

////////////////////////////////////////////////////////////////////
// MACROS
#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))
#define delta(gray) (gray>0.5?1:0)

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght;
uniform vec3 uniform_Mixing_fs_3fv_screenMsgTransp_Text1_2_Alpha;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Mixing_texture_fs_CA; // 2-cycle ping-pong Update pass CA step n + 1 (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Mixing_texture_fs_ClipArt_render; // ClipArt step n
layout (binding = 2) uniform samplerRect uniform_Mixing_texture_fs_Particle_render; // Particles step n
layout (binding = 3) uniform samplerRect uniform_Mixing_texture_fs_Render_prec;  // preceding snapshot for echo
// FONT
layout (binding = 4) uniform samplerRect uniform_Mixing_texture_fs_Screen_Font;
layout (binding = 5) uniform samplerRect uniform_Mixing_texture_fs_Screen_Message;
// pos FBO
layout (binding = 6) uniform samplerRect uniform_Mixing_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 track step n + 1 (FBO attachment 6)
#if PG_NB_TRACKS >= 2
layout (binding = 7) uniform samplerRect uniform_Mixing_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n + 1 (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 8) uniform samplerRect uniform_Mixing_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n + 1 (FBO attachment 8)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 9) uniform samplerRect uniform_Mixing_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n + 1 (FBO attachment 9)
#endif
layout (binding = 10) uniform sampler3D uniform_Mixing_texture_fs_Mask;  // mask for master output

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  float height = uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght.x;


  ////////////////////////////////////////////////////////////////////
  // CA color according to CA state and possible flash video
  vec4 CA_color = texture(uniform_Mixing_texture_fs_CA, decalCoords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
   
  // brigher CA at the beginning of a flash
  float flashCameraCoef = uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght.y;
  float flashPhotoCoef = uniform_Mixing_fs_3fv_height_flashCameraTrkWght_flashPhotoTrkWght.z;
  if( flashCameraCoef > 0 && flashCameraCoef > flashPhotoCoef) { // flash camera
    if(
      flashCameraCoef < 0.3) {
      CAMixingWeight += (1.0 - CAMixingWeight) * flashCameraCoef / 0.3;          
    }
    else {
      CAMixingWeight = 1.0;          
    }
  }
  if( flashPhotoCoef > 0 && flashPhotoCoef > flashCameraCoef ) { // flash photo
    if(flashPhotoCoef < 0.3) {
      CAMixingWeight += (1.0 - CAMixingWeight) * flashPhotoCoef / 0.3;          
    }
    else {
      CAMixingWeight = 1.0;          
    }
  }

  vec4 ClipArt_color = texture(uniform_Mixing_texture_fs_ClipArt_render, decalCoords);
  vec4 particle_color = texture(uniform_Mixing_texture_fs_Particle_render, decalCoords);
 
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
  // image mask
  float maskZoneOpacity[4] = {0, 0, 0, 0};
  for(int indZone = 0 ; indZone < 4 ; indZone++){
      maskZoneOpacity[indZone] = texture(uniform_Mixing_texture_fs_Mask, 
        vec3(vec2(decalCoords.x,decalCoords.y)/vec2(2048,2048), 
        0.125 + indZone * 0.25)).g;
      maskZoneOpacity[indZone] = clamp(maskZoneOpacity[indZone], 0, 1);
  }

  float layerMaskOpacity[6] = {0, 0, 0, 0, 0, 0};
  for(int indLayer = 0 ; indLayer < 6 ; indLayer++){
    layerMaskOpacity[indLayer] += lavoirs_mask_Mixing_wall[indLayer] * maskZoneOpacity[0];
    layerMaskOpacity[indLayer] += lavoirs_mask_Mixing_posts[indLayer] * maskZoneOpacity[1];
    layerMaskOpacity[indLayer] += lavoirs_mask_Mixing_roof[indLayer] * maskZoneOpacity[2];
    layerMaskOpacity[indLayer] += lavoirs_mask_Mixing_ground[indLayer] * maskZoneOpacity[3];
    if(indLayer < 4) {
      trackMixingWeight[indLayer] *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
    else if(indLayer == 4) {
      CAMixingWeight *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
    else if(indLayer == 5) {
      PartMixingWeight *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
  }

  ////////////////////////////////////////////////////////////////////
  // mix of current layers according to composition weights
  vec3 localColor
    = vec3(track0_color.rgb) * trackMixingWeight[0]
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMixingWeight[1]
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMixingWeight[2]
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMixingWeight[3]
#endif
    + CA_color.rgb * CAMixingWeight
#if defined(var_ClipArtMixingWeight)
    + ClipArt_color.rgb * ClipArtMixingWeight * ClipArt_color.a
#endif
#if defined(var_PartMixingWeight)
    + particle_color.rgb * PartMixingWeight * particle_color.a
#endif
    ;

  // clamping
  vec3 combined_color = clamp( localColor , 0.0 , 1.0 );
  
  ////////////////////////////////////////////////////////////////////
  // accumulation: mix of current layers and echo
  vec4 previous_snapshot_color = texture(uniform_Mixing_texture_fs_Render_prec, decalCoords);
/*  
    // brightness echo
    if( echo + echoNeg > 0.0 ) {
    combined_color = clamp( combined_color  * (1.0 - echoNeg) + echo * previous_snapshot_color.rgb , 
                            0.0 , 1.0 );
  }
*/
  // saturation echo
  // a new version of echo that only saturates the color and does not change brightness
  if( echo + echoNeg > 0.0 ) {
    vec3 echoedColor = combined_color  * (1.0 - echoNeg) + echo * previous_snapshot_color.rgb;
    float maxEchoedColor = maxCol( echoedColor );
    if( maxEchoedColor <= 1.0 ) {
      combined_color = clamp( echoedColor , 0.0 , 1.0 );
    }
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