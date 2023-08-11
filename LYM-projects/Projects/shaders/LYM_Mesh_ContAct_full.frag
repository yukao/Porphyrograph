/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-FS.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

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
uniform vec4 uniform_Mesh_fs_4fv_color_palette;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

#define graylevel(col) ((col.r+col.g+col.b)/3.0)

void main() {
  vec3 normedNormal = normalize(normalOut);


  // vec4 meshColor = texture(fs_Mesh_texture_fs_decal, decalCoords * vec2(1024,1024));

  float diffuse = clamp( dot(normedNormal, normalize(lightOut)) , 0, 1);
  // mesh
  if(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.y < 0) {
    vec3 col = 0.9 * vec3(diffuse) + 0.1 * uniform_Mesh_fs_4fv_color_palette.rgb;
    float poster_x = uniform_Mesh_fs_4fv_color_palette.w / 10.0;
    outColor0 = texture(fs_Mesh_texture_fs_decal, 1024 * vec2(poster_x,graylevel(col))); 
    outColor0.w = 1;
  }
  // bone
  else {
    outColor0 = vec4(0, 0, 1, 1);
  }
}