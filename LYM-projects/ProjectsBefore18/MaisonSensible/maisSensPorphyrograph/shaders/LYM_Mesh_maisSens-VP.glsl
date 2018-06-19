/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-VP.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 330

in vec3 position;                     // espace objet
in vec2 texcoord;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projectionMatrix;
uniform ivec2 uniform_MeshDisplay_vp_2i_FBOwh;

out vec2 decalCoords;
out vec3 positionOut;
out vec4 glpositionOut; // position

void main() {
	glpositionOut = vp_projectionMatrix * vp_viewMatrix 
	                *  vp_modelMatrix * vec4( position, 1.0 );

	gl_Position = glpositionOut;

	// passe les coordonnees de texture (decalCoords), 
	decalCoords = texcoord * uniform_MeshDisplay_vp_2i_FBOwh;

	// passes position to the fragment shader
	positionOut = position;
}
