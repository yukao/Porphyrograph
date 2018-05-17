/***********************************************************************
File: effe/shaders/LYM_TrackDisplay_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

// obtained from Vertex Program
in vec4 vertex_color;     // color of the pixel
in vec2 decalCoords;      // coord text
in float brush;           // brush ID

/////////////////////////////////////
// INPUT
layout (binding = 4) uniform sampler2D fs_decal; // brush color

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform float uniform_TrackDisplay_fs_1fv_transparency;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  // texture choice according to brush
  vec4 brushColor = texture(fs_decal, vec2(0.1 + 0.2 * brush,decalCoords.y));
  outColor0 = vec4( vertex_color.rgb , brushColor.r ) * uniform_TrackDisplay_fs_1fv_transparency;
}
