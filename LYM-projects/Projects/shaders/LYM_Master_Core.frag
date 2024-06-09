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
#if PG_NB_TRACKS >= 2
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS == 1
layout (binding = 5) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 2
layout (binding = 6) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 3
layout (binding = 7) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 4
layout (binding = 8) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen;
uniform ivec2 uniform_Master_fs_2iv_mobile_cursor_currentScene;

#if defined(var_flashchange_BGcolor_freq)
 uniform vec3 uniform_Master_fs_3fv_BG_color_rgb;
#endif

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
  // mask is used as a local change of the mix
  // a track is used to mask other ones: can be used to reveal a
  // track by drawing on another one
  float maskGrey = 1.0;
#ifdef var_currentMaskTrack
  if (currentMaskTrack == 0) {
    trackMasterWeight[0] = 0; maskGrey = graylevel(track0_color.rgb); 
  }
#if PG_NB_TRACKS >= 2
  else if (currentMaskTrack == 1) {
    trackMasterWeight[1] = 0; maskGrey = graylevel(track1_color.rgb);
  }
#endif
#if PG_NB_TRACKS >= 3
  else if (currentMaskTrack == 2) {
    trackMasterWeight[2] = 0; maskGrey = graylevel(track2_color.rgb);
  }
#endif
#if PG_NB_TRACKS >= 4
  else if (currentMaskTrack == 3) {
    trackMasterWeight[3] = 0; maskGrey = graylevel(track3_color.rgb);
  }
#endif
#endif

  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight[0]
#if PG_NB_TRACKS >= 2 && defined(var_trackMasterWeight_1) && defined(var_trackMasterWeight_1)
    + vec3(track1_color.rgb) * trackMasterWeight[1]
#endif
#if PG_NB_TRACKS >= 3 && defined(var_trackMasterWeight_2)
    + vec3(track2_color.rgb) * trackMasterWeight[2]
#endif
#if PG_NB_TRACKS >= 4 && defined(var_trackMasterWeight_3)
    + vec3(track3_color.rgb) * trackMasterWeight[3]
#endif
    + CA_color.rgb * CAMasterWeight
#if defined(var_ClipArtMasterWeight)
    + ClipArt_color.rgb * ClipArtMasterWeight * ClipArt_color.a
#endif
#if defined(var_PartMasterWeight)
    + particle_color.rgb * PartMasterWeight * particle_color.a
#endif
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );
#if defined(var_flashchange_BGcolor_freq)
  outColor0 
    = vec4( clamp( outColor0.rgb + uniform_Master_fs_3fv_BG_color_rgb , 0.0 , 1.0 ) , 1.0 );
#endif
  // masking
#ifdef var_currentMaskTrack
  if (currentMaskTrack >= 0) {
   outColor0.rgb *= maskGrey;
  }
#endif

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
#if defined(var_master_mask_scale) && defined(var_master_mask_scale_ratio) && defined(var_master_offsetX) && defined(var_master_offsetY)
  vec2 ratioed_scale = vec2(master_mask_scale, master_mask_scale * master_mask_scale_ratio);
  vec2 coordsWRTcenter = (initialCoords - centerCoords) / ratioed_scale;
  vec2 scaled_coords = coordsWRTcenter + (centerCoords + vec2(master_offsetX, master_offsetY) / ratioed_scale);

  float maskColor = 0;
#ifdef var_master_mask_opacity_1
  if(master_mask_opacity_1 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.0833333333)).g * master_mask_opacity_1;
  }
#endif
#ifdef var_master_mask_opacity_2
  if(master_mask_opacity_2 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.25)).g * master_mask_opacity_2;
  }
#endif
#ifdef var_master_mask_opacity_3
  if(master_mask_opacity_3 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.4166666667)).g * master_mask_opacity_3;
  }
#endif
#ifdef var_master_mask_opacity_4
  if(master_mask_opacity_4 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.5833333333)).g * master_mask_opacity_4;
  }
#endif
#ifdef var_master_mask_opacity_5
  if(master_mask_opacity_5 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.75)).g * master_mask_opacity_5;
  }
#endif
#ifdef var_master_mask_opacity_6
  if(master_mask_opacity_6 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.9166666667)).g * master_mask_opacity_6;
  }
#endif

  maskColor = clamp(maskColor, 0, 1);
  outColor0.rgb *= maskColor;
#endif

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
}
