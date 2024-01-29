/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-VP.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec3 uniform_Mesh_fs_3fv_light;

// from original file Maison Sensible uniform ivec2 uniform_MeshDisplay_vp_2i_FBOwh;

out vec2 decalCoords;
out vec3 positionOut; // position
out vec3 normalOut; // normal
out vec4 glpositionOut; // position

void main() {
	glpositionOut = vp_projMatrix * vp_viewMatrix 
	                *  vp_modelMatrix * vec4( position, 1.0 );

	gl_Position = glpositionOut;

	// passe les coordonnees de texture (decalCoords), 
	// from original file Maison Sensible  decalCoords = texcoord * uniform_MeshDisplay_vp_2i_FBOwh;
	decalCoords = texcoord;

	// passes position to the fragment shader
	positionOut = position;
	// passes normal to the fragment shader
	normalOut = normal;
}
