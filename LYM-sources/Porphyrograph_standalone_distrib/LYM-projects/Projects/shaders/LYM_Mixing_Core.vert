/***********************************************************************
File: effe/shaders/LYM_Composition_effe-VP.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;

out vec2 decalCoords;
out vec2 decalCoordsBG;

void main() {
  // calcul de la position en coordonnees homogenes
  // (dans le cube canonique: l'espace de clipping) pour le rasteriseur
  gl_Position = vp_projMatrix * vp_viewMatrix * vp_modelMatrix * vec4( position, 1.0 );

  // passe les coordonnees de texture (decalCoords), 
  decalCoords = texcoord;
}
