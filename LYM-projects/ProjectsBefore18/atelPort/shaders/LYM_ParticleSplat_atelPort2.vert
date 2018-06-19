/***********************************************************************
File: song/shaders/LYM_Drawing_song-VP.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

/////////////////////////////////////
// INPUT UNIFORMS
// vertex position update
layout (binding = 0) uniform samplerRect vp_decal; // 2-cycle ping-pong Update pass speed/position of particles
layout (binding = 1) uniform samplerRect vp_lookupTable1;  // 2-cycle ping-pong Update pass color/radius of particles

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;

/////////////////////////////////////
// INPUT VARYINGS
layout(location = 0) in vec3 position;                     // model space
layout(location = 1) in float radius;                      // radius
layout(location = 2) in vec3 color;                        // blend color

/////////////////////////////////////
// OUTPUT VARYINGS
out VertexData {
    float radius;
    vec3 color;
    vec2 speed;
} VertexOut;

void main()
{
	// texture coordinates
	vec2 coordinates = vec2(int(position.x),int(position.y));

	// updated position
	// rg: postion ba: speed
	vec4 posSpeed = texture(vp_decal , coordinates );
	vec2 newPosition = posSpeed.rg;

	// updated color and radius
	// xyz: color, w: radius
	vec4 colorRadius = texture(vp_lookupTable1 , coordinates );
	VertexOut.color = colorRadius.xyz;
	VertexOut.radius = colorRadius.w;
	VertexOut.speed = posSpeed.ba;

	gl_Position = vp_projMatrix * vp_viewMatrix *  vp_modelMatrix 
		* vec4(newPosition, 0, 1);
}