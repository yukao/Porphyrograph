/***********************************************************************
File: song/shaders/LYM_Drawing_song-VP.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

// vertex position update
layout (binding = 0) uniform samplerRect vp_decal; // control point positions

/////////////////////////////////////
// INPUT
layout(location = 0) in vec3 position;                     // espace objet

out vec2 decalCoords;

void main()
{
	// could  be .ba instead of .rg
	vec2 newPosition = texture(vp_decal , 
		                       vec2(int(position.x),int(position.y))).rg;
    gl_Position = vec4( newPosition, 0., 1. );
}