/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-FS.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text
in vec3 normalOut; // normale
in vec4 gl_FragCoord; // position

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_Mesh_texture_fs_decal; // texture color
layout (binding = 1) uniform samplerRect fs_Mesh_texture_fs_BG; // master color

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// UNIFORMS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// passed by the C program
uniform vec4 uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene;
uniform vec4 uniform_Mesh_fs_4fv_textureFrontier;
uniform vec4 uniform_Mesh_fs_4fv_textureFrontier_width;
uniform vec4 uniform_Mesh_fs_4fv_textureScaleTransl;
uniform vec4 uniform_Mesh_fs_4fv_color;
uniform vec4 uniform_Mesh_fs_4fv_color_master_photo_weight_bg;

void main() {

  vec4 photoColor = vec4(0,0,0,1);
  vec4 MasterColor = vec4(0,0,0,1);
  vec2 textCoord = vec2(1 - decalCoords.x, decalCoords.y);
  int indMeshFile = int(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.z);

  // photo texture
  vec2 textPhotoCoord = vec2(1 - decalCoords.x, 1 - decalCoords.y);
  vec2 locCoords = textPhotoCoord * vec2(1080,1617);
  photoColor = texture(fs_Mesh_texture_fs_decal, locCoords);

  // Master rendering FBO
  // horizontal offset
  int xOffSet = 0;
  // broken glass
  if(indMeshFile == 0) {
    xOffSet = 960;
  }
  // plane lighting the photo
  else if(indMeshFile == 1) {
    xOffSet = 240;
  }
  // plane lighting the white paper
  else if(indMeshFile == 2) {
    xOffSet = 960;
  }
  // background plane (black)
  else if(indMeshFile == 3) {
    xOffSet = 0;
  }
  vec2 locMasterCoords = textCoord * vec2(720,1080) + vec2(xOffSet, 0);
  MasterColor =  texture(fs_Mesh_texture_fs_BG, locMasterCoords);

  vec4 meshColor;
  // broken glass / wall
  if(indMeshFile == 0) {
    meshColor = uniform_Mesh_fs_4fv_color_master_photo_weight_bg.z * photoColor;
  }
  // plane lighting the photo / WHITE_LEFT
  else if(indMeshFile == 1) {
    if(uniform_Mesh_fs_4fv_color_master_photo_weight_bg.y >= 0) {
      meshColor = uniform_Mesh_fs_4fv_color_master_photo_weight_bg.x * uniform_Mesh_fs_4fv_color 
      + uniform_Mesh_fs_4fv_color_master_photo_weight_bg.y * MasterColor 
      + uniform_Mesh_fs_4fv_color_master_photo_weight_bg.z * photoColor;
    }
    else {
      meshColor = uniform_Mesh_fs_4fv_color_master_photo_weight_bg.x * uniform_Mesh_fs_4fv_color 
      - uniform_Mesh_fs_4fv_color_master_photo_weight_bg.y * vec4((vec3(1) - MasterColor.rgb), 1);
      + uniform_Mesh_fs_4fv_color_master_photo_weight_bg.z * photoColor;
    }
  }
  // plane lighting the white paper / WHITE_RIGHT
  else if(indMeshFile == 2) {
    meshColor = uniform_Mesh_fs_4fv_color_master_photo_weight_bg.x * uniform_Mesh_fs_4fv_color 
    + uniform_Mesh_fs_4fv_color_master_photo_weight_bg.y * MasterColor 
    + uniform_Mesh_fs_4fv_color_master_photo_weight_bg.z * photoColor;
  }
  // background plane (black) / BLACK
  else if(indMeshFile == 3) {
    meshColor = vec4(vec3(uniform_Mesh_fs_4fv_color_master_photo_weight_bg.w), 1);
  }

  // filled triangle
  if(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene.y == 0) {
      outColor0 = meshColor;
  }
  // line display
  else {
      outColor0 = vec4(vec3(1) - photoColor.rgb, 1.0);
  }

  // vertical and horizontal vignetting and crop
  if(locMasterCoords.y < uniform_Mesh_fs_4fv_textureFrontier.z || locMasterCoords.y > uniform_Mesh_fs_4fv_textureFrontier.w) {
      outColor0 = vec4(0,0,0,1);
  }
  if(locMasterCoords.y < uniform_Mesh_fs_4fv_textureFrontier.z + uniform_Mesh_fs_4fv_textureFrontier_width.z) {
    float alpha = (locMasterCoords.y - uniform_Mesh_fs_4fv_textureFrontier.z) / uniform_Mesh_fs_4fv_textureFrontier_width.z;
    outColor0 = vec4(alpha * outColor0.rgb,1);
  }
  if(locMasterCoords.y > uniform_Mesh_fs_4fv_textureFrontier.w - uniform_Mesh_fs_4fv_textureFrontier_width.z) {
    float alpha = (uniform_Mesh_fs_4fv_textureFrontier.w - locMasterCoords.y) / uniform_Mesh_fs_4fv_textureFrontier_width.z;
    outColor0 = vec4(alpha * outColor0.rgb,1);
  }
  if(indMeshFile == 1 && locMasterCoords.x < xOffSet + uniform_Mesh_fs_4fv_textureFrontier_width.x) {
    float alpha = (locMasterCoords.x - xOffSet) / uniform_Mesh_fs_4fv_textureFrontier_width.x;
    outColor0 = vec4(alpha * outColor0.rgb,1);
  }
  if(indMeshFile == 2 && locMasterCoords.x > xOffSet + 720 - uniform_Mesh_fs_4fv_textureFrontier_width.x) {
    float alpha = (xOffSet + 720 - locMasterCoords.x) / uniform_Mesh_fs_4fv_textureFrontier_width.x;
    outColor0 = vec4(alpha * outColor0.rgb,1);
  }



}