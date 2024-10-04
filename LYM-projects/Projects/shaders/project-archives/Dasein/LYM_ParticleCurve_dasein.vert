/***********************************************************************
File: song/shaders/LYM_Drawing_song-VP.cg

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

// vertex position update
layout (binding = 0) uniform samplerRect vp_decal; // control point positions
// vertex position update
layout (binding = 0) uniform samplerRect uniform_ParticleCurve_texture_vp_Part_pos_speed; // 2-cycle ping-pong Update pass speed/position of particles
layout (binding = 1) uniform samplerRect uniform_ParticleCurve_texture_vp_Part_col_rad;  // 2-cycle ping-pong Update pass color/radius of particles

/////////////////////////////////////
// INPUT
layout(location = 0) in vec3 position;                     // espace objet
uniform vec3 uniform_ParticleCurve_vp_3fv_trackReplay_xy_height;

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
	vec4 posSpeed = texture(uniform_ParticleCurve_texture_vp_Part_pos_speed , coordinates );
	vec2 newPosition = posSpeed.rg;
    gl_Position = vec4( newPosition, 0., 1. );

	// updated color and radius for the control (tesselator) shader
	// xyz: color, w: radius
	vec4 colorRadius = texture(uniform_ParticleCurve_texture_vp_Part_col_rad , coordinates );
	VertexOut.color = colorRadius.xyz;
	VertexOut.radius = colorRadius.w;
	VertexOut.speed = posSpeed.ba;

	if( uniform_ParticleCurve_vp_3fv_trackReplay_xy_height.x >= 0
		&& uniform_ParticleCurve_vp_3fv_trackReplay_xy_height.y >= 0 ) {
		VertexOut.replayTrackLocation = 
			vec2(uniform_ParticleCurve_vp_3fv_trackReplay_xy_height.x, 
				uniform_ParticleCurve_vp_3fv_trackReplay_xy_height.z
				- uniform_ParticleCurve_vp_3fv_trackReplay_xy_height.y);
	}
	else {
		VertexOut.replayTrackLocation = vec2(-1000);
	}
}