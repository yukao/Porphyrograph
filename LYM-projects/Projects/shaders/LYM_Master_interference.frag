/***********************************************************************
File: song/shaders/LYM_Master_song-FS.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 3

const float PI_4 = 0.785398163397448309616;

#include_declarations

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
layout (binding = 7) uniform samplerRect uniform_Master_texture_fs_LYMlogo;  // LYM logo

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Master_fs_4fv_xy_frameno_pulsedShift;
uniform vec4 uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart;

uniform vec4 uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteLow_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb;
uniform vec3 uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

/////////////////////////////////////
// PERSPECTIVE MATRIX
mat3 PerspMatrix;

///////////////////////////////////////
// CENTER
const vec2 center = vec2(512 , 384);

void main() {
#include_initializations

  float width = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.x;
  float height = uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.y;
  vec2 coords = vec2( (decalCoords.x > width ? decalCoords.x - width : decalCoords.x) , 
                      decalCoords.y);
  vec2 initialCoords = coords;

  ///////////////////////////////////////////////////////////////////
  // perspective transformation for 2 points
  // from GIMP code (gimp-transform-utils.c)
  // GIMP - The GNU Image Manipulation Program
  // Copyright (C) 1995-2001 Spencer Kimball, Peter Mattis, and others
  // REFERENCE
  // float X1 = 0;
  // float Y1 = 0;
  // float X2 = 1;
  // float Y2 = 0;
  // float X3 = 0;
  // float Y3 = 1;
  // float X4 = 1;
  // float Y4 = 1;
  // PERSPECTIVE DISTORTION OF THE UNIT CUBE
  // float X1 = 0;
  // float Y1 = 0;
  // float X2 = 1;
  // float Y2 = 0;
  // float X3 = 0;
  // float Y3 = 1;
  // float X4 = 1;
  // float Y4 = 1;
  /*float dx1, dx2, dx3, dy1, dy2, dy3;
  float det1, det2;

  dx1 = X2 - X4;
  dx2 = X3 - X4;
  dx3 = X1 - X2 + X4 - X3;

  dy1 = Y2 - Y4;
  dy2 = Y3 - Y4;
  dy3 = Y1 - Y2 + Y4 - Y3;

  det1 = dx3 * dy2 - dy3 * dx2;
  det2 = dx1 * dy2 - dy1 * dx2;

  PerspMatrix[2][0] = (det2 == 0.0) ? 1.0 : det1 / det2;

  det1 = dx1 * dy3 - dy1 * dx3;

  PerspMatrix[2][1] = (det2 == 0.0) ? 1.0 : det1 / det2;

  PerspMatrix[0][0] = X2 - X1 + PerspMatrix[2][0] * X2;
  PerspMatrix[0][1] = X3 - X1 + PerspMatrix[2][1] * X3;
  PerspMatrix[0][2] = X1;

  PerspMatrix[1][0] = Y2 - Y1 + PerspMatrix[2][0] * Y2;
  PerspMatrix[1][1] = Y3 - Y1 + PerspMatrix[2][1] * Y3;
  PerspMatrix[1][2] = Y1;
  
  PerspMatrix[2][2] = 1.0;

  vec3 transfCoords = (PerspMatrix 
  	* vec3(coords/uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.xy,1));
  coords = transfCoords.xy*uniform_Master_fs_4fv_width_height_rightWindowVMargin_timeFromStart.xy;
*/
  // kaleidoscope
  if(kaleidoscope) {
	float  radius;
	float  angle;
	if( coords.x > width) {
		coords.x -= width;
	}
	vec2 polCoords = vec2(coords - center);
	radius = length(polCoords);
	if( polCoords.x == 0 ) {
		angle = 0;
	}
	else {
		angle = abs(atan(polCoords.y , polCoords.x));
		int quadrant = int(angle / PI_4);
		if(quadrant > 0) {
			angle = angle / quadrant;
		}
	}
	coords = radius * vec2(cos(angle),sin(angle)) + center;
  }

  // vertical mirror
  // TO UNCOMMENT FOR DAR LASRAM  
  coords.y = height - coords.y;
  // ST OUEN horizontal mirror
  //coords.x = width - coords.x;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // mute screen
  if(mute_screen && decalCoords.x > width) {
    outColor0 = vec4(0, 0, 0, 1);
    return;
  }

  // interface
  if(interfaceOnScreen && decalCoords.x < 540 && decalCoords.y < 100) {
    if(decalCoords.x < 100) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteLow_rgb, 1);
    }
    else if(decalCoords.x < 200) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteMedium_rgb, 1);
    }
    else if(decalCoords.x < 300) {
      outColor0 = vec4(uniform_Master_fs_3fv_interpolatedPaletteHigh_rgb, 1);
    }
    else if(decalCoords.x > 320 && decalCoords.x < 420) {
      outColor0 = vec4(vec3(uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey.w), 1);
    }
    else if(decalCoords.x > 440 && decalCoords.x < 540) {
      outColor0 = vec4(uniform_Master_fs_4fv_pulsedColor_rgb_pen_grey.rgb, 1);
    }
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
  vec3 NonEchoedColor
    = vec3(track0_color.rgb) * trackMasterWeight_0
#if PG_NB_TRACKS >= 2
    + vec3(track1_color.rgb) * trackMasterWeight_1
#endif
#if PG_NB_TRACKS >= 3
    + vec3(track2_color.rgb) * trackMasterWeight_2
#endif
#if PG_NB_TRACKS >= 4
    + vec3(track3_color.rgb) * trackMasterWeight_3
#endif
    + CA_color.rgb * CAMasterWeight
    + particle_color.rgb * PartMasterWeight * particle_color.a
    ;

  ////////////////////////////////////////////////////////////////////
  // mix of echoed and non-echoed layer mixes
  // and clamping
  outColor0 
    = vec4( clamp( MixingColor.rgb + NonEchoedColor , 0.0 , 1.0 ) , 1.0 );

  ////////////////////////////////////////////////////////////////////
  // invert
  if( invertAllLayers ) {
     outColor0.rgb = vec3(1,1,1) - outColor0.rgb;
  }

  ////////////////////////////////////////////////////////////////////
  // image mask
  outColor0.rgb *= texture(uniform_Master_texture_fs_LYMlogo, 
  					vec2(initialCoords.x,height-initialCoords.y) ).r;

  ////////////////////////////////////////////////////////////////////
  // blinking cursor 1 pixel wide under the mouse (except for hide)
  float mouse_x = uniform_Master_fs_4fv_xy_frameno_pulsedShift.x;
  float mouse_y = uniform_Master_fs_4fv_xy_frameno_pulsedShift.y;
  float frameno = uniform_Master_fs_4fv_xy_frameno_pulsedShift.z;

  // possible double cursor
  float coordX = decalCoords.x;

  // comment for single cursor
  if( coordX > width) {
    coordX -= width;
  }
  // comment for single cursor

  // vertical mirror
  // TO UNCOMMENT FOR DAR LASRAM  
  coords.y = height - coords.y;
  // ST OUEN horizontal mirror
  // coordX = width - coordX;
  // double mirror
  //   coords.y = height - coords.y;
  //   coords.x = width - coords.x;

  // cursor
  if( !hide
      && mouse_x < width && mouse_x > 0 
      && length(vec2(coordX - mouse_x , coords.y - mouse_y)) 
      < cursorSize ) { 
    outColor0.rgb = mix( outColor0.rgb , (vec3(1,1,1) - outColor0.rgb) , abs(sin(frameno/10.0)) );
  }

  outColor0.rgb *= blendTransp;
}
