/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

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
layout (binding = 2) uniform samplerRect uniform_Master_texture_fs_Part_render;  // Particle step n
layout (binding = 3) uniform samplerRect uniform_Master_texture_fs_Trk0;  // 2-cycle ping-pong Update pass BG track step n (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 4) uniform samplerRect uniform_Master_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 5) uniform samplerRect uniform_Master_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 6) uniform samplerRect uniform_Master_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS == 1
layout (binding = 4) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 2
layout (binding = 5) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 3
layout (binding = 6) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
#endif
#if PG_NB_TRACKS == 4
layout (binding = 7) uniform sampler3D uniform_Master_texture_fs_Mask;  // mask for master output
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
  if(Argenteuil_bugs > 0) {
    vec4 noise = texture(uniform_Master_texture_fs_Mask, vec3(vec2(0,int(decalCoords.y/5))/vec2(width,height), 
      0.9166666667));
    coords.x += Argenteuil_bugs * (graylevel(noise.rgb) - 0.25) * 100; 
    coords.x = min(max(0,coords.x),1919);
  }
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
/*  if(uniform_Master_fs_4fv_width_height_timeFromStart_muteRightScreen.w != 0 && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }
*/
  if(currentScene < 22 && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }
  else if(currentScene >= 22 && decalCoords.x <= width) {
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
  vec4 particle_color = texture(uniform_Master_texture_fs_Part_render, coords);

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
  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight_0
#if PG_NB_TRACKS >= 2 && defined(var_trackMasterWeight_1) && defined(var_trackMasterWeight_1)
    + vec3(track1_color.rgb) * trackMasterWeight_1
#endif
#if PG_NB_TRACKS >= 3 && defined(var_trackMasterWeight_2)
    + vec3(track2_color.rgb) * trackMasterWeight_2
#endif
#if PG_NB_TRACKS >= 4 && defined(var_trackMasterWeight_3)
    + vec3(track3_color.rgb) * trackMasterWeight_3
#endif
    + CA_color.rgb * CAMasterWeight
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

  ////////////////////////////////////////////////////////////////////
  // inverted image
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
#if defined(var_master_mask_scale) && defined(var_master_mask_scale_ratio) && defined(var_master_offsetX) && defined(var_master_offsetY)
  vec2 scaled_coords = initialCoords;

  float maskColor = 0;
#ifdef var_master_mask_opacity_1
  if(master_mask_opacity_1 > 0) {
    maskColor += texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.0833333333)).g * master_mask_opacity_1;
  }
#endif
#ifdef var_master_mask_opacity_2
  if(master_mask_opacity_2 > 0) {
    maskColor = texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.25)).g * master_mask_opacity_2;
    maskColor += maskColor;
  }
#endif
#ifdef var_master_mask_opacity_3
  if(master_mask_opacity_3 > 0) {
    maskColor = texture(uniform_Master_texture_fs_Mask, vec3(scaled_coords/vec2(2048,2048), 
      0.4166666667)).g * master_mask_opacity_3;
    maskColor += maskColor;
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

  // blinking cursor
  if( uniform_Master_fs_2iv_mobile_cursor_currentScene.x != 0 
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coords.x - mouse_x , height - coords.y - mouse_y)) < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  // master level
  outColor0.rgb *= master;
}
