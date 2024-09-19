/***********************************************************************
File: maisonSens/shaders/LYM_Mesh_maisonSens-VP.glsl

LYM maisonSens & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

layout(location = 0) in vec3 position;                     // espace objet
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in ivec4 boneIndex;
layout(location = 4) in vec4 boneWeights;

/////////////////////////////////////
// UNIFORMS
uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec3 uniform_Mesh_fs_3fv_light;
uniform mat4 uniform_Mesh_bones_matrices[20];
uniform vec2 uniform_Mesh_vp_2fv_dilate_explode;


out vec2 decalCoords;
out vec3 normalOut; // normal
out vec3 lightOut;

void main() {
	float explode = uniform_Mesh_vp_2fv_dilate_explode.y;
	vec3 cur_position = position;
	if(explode > 0.0001) {
		cur_position = cur_position + explode * normalOut;
	}

	vec4 transfVertex  = vec4( 0.0 );
	vec4 transfNormal  = vec4( 0.0 );
	bool transf = false;
	float w_tot = boneWeights[0] + boneWeights[1] + boneWeights[2] + boneWeights[3];
	if(boneIndex[0] >= 0) {
        transfVertex += (uniform_Mesh_bones_matrices[ boneIndex[0] ] * vec4(cur_position, 1.0)) * boneWeights[0];
        transfNormal += (uniform_Mesh_bones_matrices[ boneIndex[0] ] * vec4(normal, 0.0)) * boneWeights[0];
	 	transf = true;
	}
	if(boneIndex[1] >= 0) {
        transfVertex += (uniform_Mesh_bones_matrices[ boneIndex[1] ] * vec4(cur_position, 1.0)) * boneWeights[1];
        transfNormal += (uniform_Mesh_bones_matrices[ boneIndex[1] ] * vec4(normal, 0.0)) * boneWeights[1];
	 	transf = true;
	}
	if(boneIndex[2] >= 0) {
        transfVertex += (uniform_Mesh_bones_matrices[ boneIndex[2] ] * vec4(cur_position, 1.0)) * boneWeights[2];
        transfNormal += (uniform_Mesh_bones_matrices[ boneIndex[2] ] * vec4(normal, 0.0)) * boneWeights[2];
	 	transf = true;
	}
	if(boneIndex[3] >= 0) {
        transfVertex += (uniform_Mesh_bones_matrices[ boneIndex[3] ] * vec4(cur_position, 1.0)) * boneWeights[3];
        transfNormal += (uniform_Mesh_bones_matrices[ boneIndex[3] ] * vec4(normal, 0.0)) * boneWeights[3];
	 	transf = true;
	}
	if(transf == false) {
		transfVertex = vec4( cur_position, 1.0 );
		transfNormal.xyz = normal;
	}
	else {
		transfVertex /= w_tot;
		transfVertex.w = 1.;
	}
	transfVertex.xyz += uniform_Mesh_vp_2fv_dilate_explode.x * transfNormal.xyz;

	// bones
	if(uniform_Mesh_fs_3fv_light == 0) {
		transfVertex = vec4( cur_position, 1.0 );
	}

	mat4 MV = vp_viewMatrix * vp_modelMatrix;

    // transforms light from world coordinates to model coordinates
    lightOut = (inverse(MV) * vec4(uniform_Mesh_fs_3fv_light,0.0)).xyz;

	gl_Position = vp_projMatrix * MV * transfVertex;

	// passe les coordonnees de texture (decalCoords), 
	// from original file Maison Sensible  decalCoords = texcoord * uniform_MeshDisplay_vp_2i_FBOwh;
	decalCoords = texcoord;

	// passes normal to the fragment shader
	normalOut = transfNormal.xyz;
}
