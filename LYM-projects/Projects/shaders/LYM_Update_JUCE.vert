/***********************************************************************
File: effe/shaders/LYM_Update_effe-VP.cg

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec2 uniform_Update_vp_2fv_width_height;

out vec2 decalCoords;
out vec2 decalCoordsPOT;

void main() {
    // calcul de la position en coordonnees homogenes
    // (dans le cube canonique: l'espace de clipping) pour le rasteriseur
  gl_Position = vp_projMatrix * vp_viewMatrix *  vp_modelMatrix * vec4( position, 1.0 );

  // passe les coordonnees de texture (decalCoords), 
  decalCoords = texcoord;
  // rotated texture coordinates for the 5 layers
  decalCoordsPOT = texcoord/uniform_Update_vp_2fv_width_height;
}
