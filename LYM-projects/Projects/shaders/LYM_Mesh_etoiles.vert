/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-VP.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;

/////////////////////////////////////
// UNIFORMS
uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec3 uniform_Mesh_fs_3fv_light;

out vec2 decalCoords;
out vec3 normalOut; // normal
out vec3 lightOut;

void main() {
	mat4 MV = vp_viewMatrix *  vp_modelMatrix;
    // transforms light from world coordinates to model coordinates
    lightOut = (inverse(MV) * vec4(uniform_Mesh_fs_3fv_light,0.0)).xyz;

	gl_Position = vp_projMatrix * MV * vec4( position, 1.0 );

	// passe les coordonnees de texture (decalCoords), 
	// from original file Maison Sensible  decalCoords = texcoord * uniform_MeshDisplay_vp_2i_FBOwh;
	decalCoords = vec2(texcoord.x,texcoord.y);

	// passes normal to the fragment shader
	normalOut = normal;
}
