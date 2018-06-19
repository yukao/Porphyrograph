/***********************************************************************
File: dcONT/shaders/LYM_Composition_dcONT-VP.cg

LYM dcONT & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 150
uniform mat4 vp_viewMatrix, vp_projMatrix;

in vec3 position;                     // espace objet
in vec3 normal;                       // espace objet
in vec2 texcoord;
in vec4 color;

out vec2 decalCoords;
out vec2 decalCoordsBG;

void main() {
    // calcul de la position en coordonnees homogenes
    // (dans le cube canonique: l'espace de clipping) pour le rasteriseur
  gl_Position = vp_projMatrix * vp_viewMatrix *  vec4( position, 1.0 );

  // passe les coordonnees de texture (decalCoords), 
  decalCoords = texcoord;
}
