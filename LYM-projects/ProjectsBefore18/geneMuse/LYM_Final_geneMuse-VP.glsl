/***********************************************************************
File: alKemi/shaders/LYM_Final_geneMuse-VP.cg

LYM alKemi & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;

out vec2 decalCoords;

void main() {
    // calcul de la position en coordonnees homogenes
    // (dans le cube canonique: l'espace de clipping) pour le rasteriseur
  gl_Position = vp_projMatrix * vp_viewMatrix *  vp_modelMatrix * vec4( position, 1.0 );

  // passe les coordonnees de texture (decalCoords), 
  decalCoords = texcoord;
}
