/***********************************************************************
File: alKemi/shaders/LYM_Sensor_geneMuse-FS.cg

LYM alKemi & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

// Main shader.

// obtained from Vertex Program
in vec2 decalCoords;  // coord text

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal; // previous pass output

/////////////////////////////////////
// UNIFORMS
// passed by the C program
// per sensor variable
uniform vec4 uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency;
// variable for all sensors
// uniform vec2 uniform_Sensor_fs_2fv_frameno_invert;

/////////////////////////////////////
// VIDEO FRAME COLOR OUTPUT
out vec4 outColor0;

void main() {
  vec4 sensorColor = texture(fs_decal, decalCoords );
  float outgray = mix( sensorColor.r , sensorColor.g , 
          uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.w );
  outColor0 = vec4(1,0,0,outgray);
  /*
  if( uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.z > 0 
    && uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.y > 0 ) {
     outColor0 = texture(fs_decal, decalCoords + vec2(100,0));
  }
  */
/*  else if( uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.y > 0 ) {
     outColor0 = texture(fs_decal, decalCoords + vec2(200,0));
  }
*/ 
/* else {
     outColor0 = texture(fs_decal, decalCoords );
  }
  */

  // if( uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.y > 0 ) {
  // 	outColor0.a *= abs(sin(uniform_Sensor_fs_2fv_frameno_invert.x/10.0));
  // }	
  /*
  if( uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.x > 0 ) {
		outColor0.rgb *= vec3(1,0,0);
  }
  else {
		outColor0.rgb *= vec3(0,1,0);
  }
  */
   // outColor0.a *= uniform_Sensor_fs_4fv_onOff_isCurrentSensor_isFollowMouse_transparency.w;
}
