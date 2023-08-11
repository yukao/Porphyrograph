/***********************************************************************
File: effe/shaders/LYM_Final_effe-FS.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#define PG_NB_TRACKS 3
#define graylevel(col) ((col.r+col.g+col.b)/3.0)

#include_declarations

// Main shader.

// obtained from Vertex Program
in vec2 decalCoordsPOT;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // Final pass output with possible echo
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // BG subtraction

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Camera_fs_4fv_W_H;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
#include_initializations
  float videoGray = 0.0;

  vec2 cameraCoord = vec2(0.0);
  vec3 cameraImage = vec3( 0.0 );

  cameraCoord = vec2(1.0-decalCoordsPOT.x , decalCoordsPOT.y )
               * vec2( uniform_Camera_fs_4fv_W_H.x,
                       uniform_Camera_fs_4fv_W_H.y );
  cameraImage = texture(fs_decal, cameraCoord ).rgb;

  // gamma correction
  cameraImage = vec3( pow(cameraImage.r,cameraGamma) , pow(cameraImage.g,cameraGamma) , pow(cameraImage.b,cameraGamma) );

  if( BGSubtr ) {
    cameraImage = abs(cameraImage - texture(fs_lookupTable1, cameraCoord ).rgb); // initial background subtraction
  }

  videoGray = graylevel(cameraImage);
  if( videoGray < cameraThreshold ) {
    cameraImage = vec3(0.0);
  }

  outColor0 = vec4(cameraImage,1);
}
