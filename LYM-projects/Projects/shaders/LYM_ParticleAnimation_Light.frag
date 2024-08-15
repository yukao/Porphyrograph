/***********************************************************************
File: song/shaders/LYM_ParticleAnimation_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#include_declarations

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
#define PG_WITH_CAMERA_CAPTURE
#define PG_NB_TRACKS 4
#define PG_NB_PATHS 12

#define PG_PATH_ANIM_POS              0
#define PG_PATH_ANIM_RAD              1
#define PG_MAX_PATH_ANIM_DATA         2

// VIDEO UPDATE
vec2 movieWH;
#ifdef PG_WITH_CAMERA_CAPTURE
  vec2 cameraWH;
#endif

////////////////////////////////////
// PIXEL UPDATE
// center of the translation of acceleration values
// used for shifting pixel motion in a direction
vec2 pixel_acc_center = vec2(0.5,0.5);

vec4 randomPart;
vec2 pixelTextureCoordinatesXY; // the POT coordinates of the

// #define GENERATIVE_MOVIE

///////////////////////////////////////////////////////////////////
// PARTICLE IMAGE INITIALIZATION
#define PG_NB_PARTICLE_INITIAL_IMAGES 9

const uint pg_Part_pos_speed_FBO_ParticleAnimation_attcht = 0;
const uint pg_Part_col_rad_FBO_ParticleAnimation_attcht = 1;
const uint pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht = 2;

///////////////////////////////////////////////////////////////////
// PARTICLES

// MOTION
// const uint PARTMOVE_NONE = 0;
// const uint PARTMOVE_REACH_GRID = 1; // reaches grid positions (regular grid over the image)
// const uint PARTMOVE_RANDOM = 2; // random motion
// const uint PARTMOVE_FIELD = 1; // follows the noise field (cf pixels)
// const uint PARTMOVE_PARALLEL = 2; // all move same direction (pen direction)
// const uint PARTMOVE_DIVERGENT = 3; // reaches nearest edge
// const uint PARTMOVE_CONVERGENT = 4; // reaches for screen center
// const uint PARTMOVE_FOLLOW_ONE = 5; // follow the first particle
// const uint PARTMOVE_FOLLOW_NEXT = 6; // follow the next particle
// const uint PARTMOVE_FOLLOW_RANDOM = 7; // follow a random particle
// const uint PARTMOVE_REACH_RANDOM = 12; // reaches random (but fixed) positions

// REPOP
const uint PARTREPOP_NONE = 0;

// EXIT
const uint PARTEXIT_NONE = 0; // lost when exits
const uint PARTEXIT_OPPOSITE = 1; // when going out right/left/top/bottom reappers left/right/bottom/top
const uint PARTEXIT_BOUNCE = 2; // bounces on edges

// DRAWING INTERACTION
const uint PARTSTROKE_NONE = 0; 
const uint PARTSTROKE_AGGREG = 1; // aggregates on drawing
const uint PARTSTROKE_BOUNCE = 2; // bounces on drawing
const uint PARTSTROKE_DAMP = 3; // slows down on drawing

// COLOR MODE
const uint PARTCOLOR_WHITE = 0; // white particles
const uint PARTCOLOR_MAX = 1; // maximum of flash color and particle
const uint PARTCOLOR_COPY = 2; // copy flash color


////////////////////////////////////////////////////////////////////
// pixel mode
const uint PIXEL_NONE = 0;
const uint PIXEL_UNIQUE = 1;
const uint PIXEL_FIREWORK = 2;

////////////////////////////////////////////////////////////////////
// drawing modes
const uint DRAWING_VOID = 0;
const uint DRAWING_LINE = 1;

////////////////////////////////////////////////////////////////////
// MACROS
#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/////////////////////////////////////
// screen & FBO size
float height;
float width;

// frame number
int frameNo;
// target frame number
int targetFrameNo;

/////////////////////////////////////
// 3 FBO OUTPUT: CA, PIXELS AND PARTICLES
/* const uint pg_Part_pos_speed_FBO_ParticleAnimation_attcht = 0;
const uint pg_Part_col_rad_FBO_ParticleAnimation_attcht = 1;
const uint pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht = 2; */
vec4 out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht + 1];

////////////////////////////////////
// PARTICLE UPDATE

// PARTICLE PASS OUTPUT
vec4 out_particlesRendering;

// PARTICLE FLASH CUMUL
vec3 flashToPartCumul = vec3(0);

// out particle returned values
vec4 out_position_speed_particle = vec4(0);
vec4 out_color_radius_particle = vec4(1);
vec4 out_target_position_color_radius_particle = vec4(1);

// number of particles
int nbParticles = 0;

///////////////////////////////////////
// REPOPULATION OF PARTICLES: DENSITY OF REPOPULATION
float repop_density_weight = 1;
vec3 textureDensityValue = vec3(0);

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// VARYINGS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// UNIFORMS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// passed by the C program
uniform vec4 uniform_ParticleAnimation_path_data[PG_MAX_PATH_ANIM_DATA * (PG_NB_PATHS)];

uniform vec4 uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo; // 
uniform vec4 uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo; // 
uniform vec4 uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_partSizeUnpulsed; // 
uniform vec4 uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH; //
 
uniform vec4 uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts;   // 

/////////////////////////////////////
// INPUT
/////////////////////////////////////
// INPUT
                      // particle initialization from images
layout (binding = 0) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_init_pos_speed;  // couples of textures for particle initialization through photo or video: position/speed
layout (binding = 1) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_init_col_rad;  // couples of textures for particle initialization through photo: color/radius
layout (binding = 2) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_acc;  // texture for particle acceleration shift through photo
layout (binding = 3) uniform samplerRect uniform_ParticleAnimation_texture_fs_CA;         // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 4) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_pos_speed;  // 2-cycle ping-pong ParticleAnimation pass position/speed of Particles step n (FBO attachment 2)
layout (binding = 5) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_col_rad;  // 2-cycle ping-pong ParticleAnimation pass color/radius of Particles step n (FBO attachment 3)
layout (binding = 6) uniform samplerRect uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad;  // 2-cycle ping-pong ParticleAnimation pass target position/color/radius of Particles step n (FBO attachment 4)
// noise
layout (binding = 7) uniform sampler3D   uniform_ParticleAnimation_texture_fs_Noise;  // noise texture
layout (binding = 8)  uniform samplerRect uniform_ParticleAnimation_texture_fs_RepopDensity;  // repop density texture
layout (binding = 9) uniform samplerRect uniform_ParticleAnimation_texture_fs_Camera_frame;  // camera texture
layout (binding = 10) uniform samplerRect uniform_ParticleAnimation_texture_fs_Movie_frame;  // movie textures
layout (binding = 11) uniform samplerRect uniform_ParticleAnimation_texture_fs_Trk0;  // 2-cycle ping-pong ParticleAnimation pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 12) uniform samplerRect uniform_ParticleAnimation_texture_fs_Trk1;  // 2-cycle ping-pong ParticleAnimation pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 13) uniform samplerRect uniform_ParticleAnimation_texture_fs_Trk2;  // 2-cycle ping-pong ParticleAnimation pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 14) uniform samplerRect uniform_ParticleAnimation_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif

/////////////////////////////////////
// PARTICLE OUTPUT
// POSITION / SPEED
layout (location = 0) out vec4 out_ParticleAnimation_FBO_fs_Part_pos_speed;
// COLOR / RADIUS
layout (location = 1) out vec4 out_ParticleAnimation_FBO_fs_Part_col_rad;
// TARGET POSITION / COLOR / RADIUS
layout (location = 2) out vec4 out_ParticleAnimation_FBO_fs_Part_Target_pos_col_rad;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// PERLIN NOISE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// Some useful functions
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

// Description : GLSL 2D simplex noise function
//      Author : Ian McEwan, Ashima Arts
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : 
//  Copyright (C) 2011 Ashima Arts. All rights reserved.
//  Distributed under the MIT License. See LICENSE file.
//  https://github.com/ashima/webgl-noise
// 
float snoise(vec2 v , float noiseParticleScale) {
  v *= noiseParticleScale;

  // Precompute values for skewed triangular grid
  const vec4 C = vec4(0.211324865405187,
                      // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  
                      // 0.5*(sqrt(3.0)-1.0)
                      -0.577350269189626,  
                      // -1.0 + 2.0 * C.x
                      0.024390243902439); 
                      // 1.0 / 41.0

  // First corner (x0)
  vec2 i  = floor(v + dot(v, C.yy));
  vec2 x0 = v - i + dot(i, C.xx);

  // Other two corners (x1, x2)
  vec2 i1 = vec2(0.0);
  i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
  vec2 x1 = x0.xy + C.xx - i1;
  vec2 x2 = x0.xy + C.zz;

  // Do some permutations to avoid
  // truncation effects in permutation
  i = mod289(i);
  vec3 p = permute(
          permute( i.y + vec3(0.0, i1.y, 1.0))
              + i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(
                      dot(x0,x0), 
                      dot(x1,x1), 
                      dot(x2,x2)
                      ), 0.0);

  m = m*m ;
  m = m*m ;

  // Gradients: 
  //  41 pts uniformly over a line, mapped onto a diamond
  //  The ring size 17*17 = 289 is close to a multiple 
  //      of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

  // Normalise gradients implicitly by scaling m
  // Approximation of: m *= inversesqrt(a0*a0 + h*h);
  m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

  // Compute final noise value at P
  vec3 g = vec3(0.0);
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
  return (130.0 * dot(m, g) + 1.0) / 2.0;
}

vec2 generativeNoise(vec2 texCoordLoc) {
  // FLAT
  return vec2(snoise( texCoordLoc , noiseParticleScale * 100 ),
                          snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseParticleScale * 100 ));
}

// random noise
// https://www.ronja-tutorials.com/2018/09/02/white-noise.html
// after (c) Ronja Böhringer
//get a scalar random value from a 3d value
// 2-step computation due to lack of precision
int rand3D(vec3 value, float threshold){
  //make value smaller to avoid artefacts
  vec3 smallValue = sin(value);
  //get scalar value from 3d vector
  double random = sin(dot(smallValue, vec3(12.9898, 78.233, 37.719))) * 1437.585453;
  //make value more random by making it bigger and then taking teh factional part
  random = 100 * fract(random);
  if(random < threshold) {
    //get scalar value from 3d vector
    double random2 = sin(dot(smallValue, vec3(47.2903, 12.0989, 28.2381))) * 2639.832872;
    //make value more random by making it bigger and then taking teh factional part
    random2 = (100 * fract(random2));
    if(random2 < threshold) {
      return 1;
    }
    else {
      return 0;
    }
  }
  return 0;
}

int rand3D_new(vec3 value, float threshold){
  if( snoise( value.xy + vec2(2.0937,9.4872) , noiseParticleScale * 100 )  < threshold) {
    return 1;
  }
  else {
    return 0;
  }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PARTICLES UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void particle_out( void ) {
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN OF UPDATE PASS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void main() {
#include_initializations

  //////////////////
  out_ParticleAnimation_FBO_fs_Part_pos_speed = vec4(0);
  out_ParticleAnimation_FBO_fs_Part_col_rad = vec4(0);
  out_ParticleAnimation_FBO_fs_Part_Target_pos_col_rad = vec4(0);
}
