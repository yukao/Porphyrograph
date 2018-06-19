/***********************************************************************
File: alKemi/shaders/LYM_Composition_alK-VP.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 330

in vec3 position;                     // espace objet
in vec2 texcoord;

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
