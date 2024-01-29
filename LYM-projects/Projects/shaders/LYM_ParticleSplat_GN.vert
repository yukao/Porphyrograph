/***********************************************************************
File: song/shaders/LYM_Drawing_song-VP.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

/////////////////////////////////////
// INPUT UNIFORMS
// vertex position update
layout (binding = 0) uniform samplerRect uniform_ParticleSplat_texture_vp_Part_pos_speed; // 2-cycle ping-pong Update pass speed/position of particles
layout (binding = 1) uniform samplerRect uniform_ParticleSplat_texture_vp_Part_col_rad;  // 2-cycle ping-pong Update pass color/radius of particles

uniform mat4 vp_modelMatrix, vp_viewMatrix, vp_projMatrix;
uniform vec2 uniform_ParticleSplat_vp_2fv_trackReplay_xy;

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
    vec2 replayTrackLocation;
} VertexOut;

void main()
{
	// texture coordinates
	vec2 coordinates = vec2(int(position.x),int(position.y));

	// updated position
	// rg: postion ba: speed
	vec4 posSpeed = texture(uniform_ParticleSplat_texture_vp_Part_pos_speed , coordinates );
	vec2 newPosition = posSpeed.rg;

	// updated color and radius
	// xyz: color, w: radius
	vec4 colorRadius = texture(uniform_ParticleSplat_texture_vp_Part_col_rad , coordinates );
	VertexOut.color = colorRadius.xyz;
	VertexOut.radius = colorRadius.w;
	VertexOut.speed = posSpeed.ba;

	gl_Position = vp_projMatrix * vp_viewMatrix *  vp_modelMatrix 
		* vec4(newPosition, 0, 1);
	if( uniform_ParticleSplat_vp_2fv_trackReplay_xy.x >= 0
		&& uniform_ParticleSplat_vp_2fv_trackReplay_xy.y >= 0 ) {
		VertexOut.replayTrackLocation = (vp_projMatrix * vp_viewMatrix *  vp_modelMatrix 
			* vec4(uniform_ParticleSplat_vp_2fv_trackReplay_xy.x, 768-uniform_ParticleSplat_vp_2fv_trackReplay_xy.y, 0, 1)).xy;
	}
	else {
		VertexOut.replayTrackLocation = vec2(-1000);
	}
}