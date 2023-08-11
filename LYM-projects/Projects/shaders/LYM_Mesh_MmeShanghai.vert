/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-VP.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;

// from original file Maison Sensible uniform ivec2 uniform_MeshDisplay_vp_2i_FBOwh;

out vec2 decalCoords;
out vec3 normalOut; // normal

void main() {
	gl_Position = vp_projMatrix * vp_viewMatrix 
	                *  vp_modelMatrix * vec4( position, 1.0 );

	// passe les coordonnees de texture (decalCoords), 
	// from original file Maison Sensible  decalCoords = texcoord * uniform_MeshDisplay_vp_2i_FBOwh;
	decalCoords = texcoord;

	// passes normal to the fragment shader
	normalOut = normal;
}
