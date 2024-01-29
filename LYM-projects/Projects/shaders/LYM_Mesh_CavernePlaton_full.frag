/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-FS.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text
in vec3 normalOut; // normale
in vec3 lightOut; // light in model coordinates

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_Mesh_texture_fs_decal; // previous pass output

/////////////////////////////////////
// UNIFORMS
uniform vec4 uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec3 normedNormal = normalize(normalOut);

  vec4 meshColor = texture(fs_Mesh_texture_fs_decal, decalCoords * vec2(1024,1024));

  float diffuse = clamp( dot(normedNormal, normalize(lightOut)) , 0, 1);
  if(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.y == 0) {
    outColor0 = vec4( 0.9 * vec3(diffuse) + 0.1 * meshColor.rgb, 1);
  }
  else {
    outColor0 = vec4(0,0,0,1);
  }
}