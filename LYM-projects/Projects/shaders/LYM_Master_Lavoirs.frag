/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define PG_NB_TRACKS 4

#include_declarations

#define graylevel(col) ((col.r+col.g+col.b)/3.0)

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Master_texture_fs_Render_curr; // Master pass output with possible echo
layout (binding = 1) uniform samplerRect uniform_Master_texture_fs_CA;  // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_ClipArt_render;  // Particle step n
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Particle_render;  // Particle step n
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Update pass BG track step n (FBO attachment 3)
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
layout (binding = 8) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;
uniform vec3 uniform_Master_fs_3fv_BGcolor_rgb;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations

  float width = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.x;
  float height = uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);
  vec2 initialCoords = coords;
  vec2 centerCoords = vec2(width/2, height/2);

  int currentScene = uniform_Master_fs_2iv_mobile_cursor_currentScene.y;

  // vertical mirror
  // coords.y = height - coords.y;
  // BRON mirror
  // coords.x = width - coords.x;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // mute screen
  if(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.w != 0
     && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }

  ////////////////////////////////////////////////////////////////////
  // mix of echoed layers according to Mixing weights
  vec4 MixingColor = texture(uniform_Master_texture_fs_Render_curr, coords );

  ////////////////////////////////////////////////////////////////////
  // non-echoed layers
  vec4 CA_color = texture(uniform_Master_texture_fs_CA, coords);
  if( CA_color.a < 0 ) {
    CA_color = vec4(0.0);
  }
  vec4 ClipArt_color = texture(uniform_Master_texture_fs_ClipArt_render, coords);
  vec4 particle_color = texture(uniform_Master_texture_fs_Particle_render, coords);

  vec4 track0_color = texture(uniform_Master_texture_fs_Trk0, coords);
  vec4 track1_color = texture(uniform_Master_texture_fs_Trk1, coords);
  vec4 track2_color = texture(uniform_Master_texture_fs_Trk2, coords);
  vec4 track3_color = texture(uniform_Master_texture_fs_Trk3, coords);

    ////////////////////////////////////////////////////////////////////
  // image mask
  vec2 ratioed_scale = vec2(master_mask_scale, master_mask_scale * master_mask_scale_ratio);
  vec2 coordsWRTcenter = (initialCoords - centerCoords) / ratioed_scale;
  vec2 scaled_coords = coordsWRTcenter + (centerCoords + vec2(master_offsetX, master_offsetY) / ratioed_scale);

  float maskZoneOpacity[4] = {0, 0, 0, 0};
  for(int indZone = 0 ; indZone < 4 ; indZone++){
      maskZoneOpacity[indZone] = texture(uniform_Master_texture_fs_Mask, 
        vec3(vec2(scaled_coords.x,scaled_coords.y)/vec2(2048,2048), 
        0.125 + indZone * 0.25)).g;
      maskZoneOpacity[indZone] = clamp(maskZoneOpacity[indZone], 0, 1);
  }

  ////////////////////////////////////////////////////////////////////
  // mix of non echoed layers according to final weights
  // mask is used as a local change of the mix
  // a track is used to mask other ones: can be used to reveal a
  // track by drawing on another one
  float trackMaskOpacity = 1.0;
  if (currentMaskTrack == 0) {
    trackMasterWeight[0] = 0; trackMaskOpacity = graylevel(track0_color.rgb); 
  }
  else if (currentMaskTrack == 1) {
    trackMasterWeight[1] = 0; trackMaskOpacity = graylevel(track1_color.rgb);
  }
  else if (currentMaskTrack == 2) {
    trackMasterWeight[2] = 0; trackMaskOpacity = graylevel(track2_color.rgb);
  }
  else if (currentMaskTrack == 3) {
    trackMasterWeight[3] = 0; trackMaskOpacity = graylevel(track3_color.rgb);
  }


  float layerMaskOpacity[6] = {0, 0, 0, 0, 0, 0};
  for(int indLayer = 0 ; indLayer < 6 ; indLayer++){
    layerMaskOpacity[indLayer] += lavoirs_mask_opacity_wall[indLayer] * maskZoneOpacity[0];
    layerMaskOpacity[indLayer] += lavoirs_mask_opacity_posts[indLayer] * maskZoneOpacity[1];
    layerMaskOpacity[indLayer] += lavoirs_mask_opacity_roof[indLayer] * maskZoneOpacity[2];
    layerMaskOpacity[indLayer] += lavoirs_mask_opacity_ground[indLayer] * maskZoneOpacity[3];
    if(indLayer < 4) {
      trackMasterWeight[indLayer] *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
    else if(indLayer == 4) {
      CAMasterWeight *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
    else if(indLayer == 5) {
      PartMasterWeight *= clamp(layerMaskOpacity[indLayer], 0, 1);
    }
  }

  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight[0]
    + vec3(track1_color.rgb) * trackMasterWeight[1]
    + vec3(track2_color.rgb) * trackMasterWeight[2]
    + vec3(track3_color.rgb) * trackMasterWeight[3]
    + CA_color.rgb * CAMasterWeight
    + ClipArt_color.rgb * ClipArtMasterWeight * ClipArt_color.a
    + particle_color.rgb * PartMasterWeight * particle_color.a
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );

  // background color management if it is relevant
  if(uniform_Master_fs_3fv_BGcolor_rgb.r + uniform_Master_fs_3fv_BGcolor_rgb.g + uniform_Master_fs_3fv_BGcolor_rgb.b > 0) {
    if(graylevel(outColor0) < 0.3) {
      outColor0.rgb = clamp(uniform_Master_fs_3fv_BGcolor_rgb.rgb, 0, 1);
    }
    else {
      outColor0.rgb = vec3(1) - outColor0.rgb;
    }
  }

  // masking with the opacity of tracks
#ifdef var_currentMaskTrack
  if (currentMaskTrack >= 0) {
   outColor0.rgb *= trackMaskOpacity;
  }
#endif

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }


////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse (except for hide)
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;

  // vertical mirror
  //  coords.y = height - coords.y;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // master level
  outColor0.rgb *= master;

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }
  // outColor0.rgb = outColor0.rgb * 0.0001 + vec3(layerMaskOpacity[0]);
}
