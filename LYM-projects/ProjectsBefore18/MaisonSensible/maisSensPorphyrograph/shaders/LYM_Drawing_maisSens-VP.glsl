/***********************************************************************
File: alK/shaders/LYM_Drawing_alK-VP.cg

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 330

in vec3 position;                     // espace objet
in vec2 texcoord;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec2 uniform_Drawing_vp_2fv_width_height;

out vec2 decalCoords;
out vec2 decalCoordsPOT;

void main() {
    // calcul de la position en coordonnees homogenes
    // (dans le cube canonique: l'espace de clipping) pour le rasteriseur
  gl_Position = vp_projMatrix * vp_viewMatrix *  vp_modelMatrix * vec4( position, 1.0 );

  // passe les coordonnees de texture (decalCoords), 
  decalCoords = texcoord;
  // rotated texture coordinates for the 5 layers
  decalCoordsPOT = texcoord/uniform_Drawing_vp_2fv_width_height;
}
