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
#define PG_VIDEO_ACTIVE
#define PG_NB_TRACKS 4
#define PG_NB_PATHS 11

#define PG_PATH_ANIM_POS              0
#define PG_PATH_ANIM_RAD              1
#define PG_MAX_PATH_ANIM_DATA         2

// VIDEO UPDATE
#ifdef PG_VIDEO_ACTIVE
  vec2 movieWH;
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
#define PG_NB_PARTICLE_INITIAL_IMAGES 6

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
uniform vec4 uniform_ParticleAnimation_path_data[PG_MAX_PATH_ANIM_DATA * (PG_NB_PATHS + 1)];

uniform vec4 uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo; // 
uniform vec4 uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo; // 
uniform vec4 uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit; // 
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
#ifdef PG_VIDEO_ACTIVE
layout (binding = 9) uniform samplerRect uniform_ParticleAnimation_texture_fs_Camera_frame;  // camera texture
layout (binding = 10) uniform samplerRect uniform_ParticleAnimation_texture_fs_Movie_frame;  // movie textures
#endif
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
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PARTICLES UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


void particle_out( void ) {
/* #ifdef CURVE_PARTICLES
  // index of the control point: 0 for the head and 1, 2, or 3 for the tail
  int index = int(decalCoords.x) % 4;
#endif
 */
/* #ifdef CURVE_PARTICLES
  ////////////////////////////////////////////////////////////////////
  // FIRST VERTEX OF THE PRIMITIVE
  // for the head of the curve, takes acceleration from noise strength field
  if( index == 0 ) {
#endif
 */

  //////////////////////////////////////////////////////////////////
  // COLOR AND RADIUS UPDATE VARIABLE
  // copies color and radius at preceding frame
  out_color_radius_particle = out_attachment_FBO[pg_Part_col_rad_FBO_ParticleAnimation_attcht];
  out_target_position_color_radius_particle = out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht];

  //////////////////////////////////////////////////////////////////
  // SPPED AND POSITION UPDATE VARIABLE
  // copies position and speed at preceding frame
  out_position_speed_particle = out_attachment_FBO[pg_Part_pos_speed_FBO_ParticleAnimation_attcht];

  ////////////////////////////////////////////////////////////////////
  // INITIALIZATION
  // random initialization of the particle positions (initial speed is null currently)
  if(part_initialization >= 0) {
    /////////////////////////////////////////////////////////////////////
    // RANDOM INITIALIZATION
    // head and tail are initialized with the same values
    // reaches for regular grid position
    // uniform_ParticleAnimation_texture_fs_Part_init_pos_speed
    // float rank = (decalCoords.x + decalCoords.y * width) / float(nbParticles);
    // int rankGrid = int(rank * width * height);

    if(part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
      vec4 target_color_radius = texture( uniform_ParticleAnimation_texture_fs_Part_init_col_rad , decalCoords );
      if(partMove_target) { // reaches a target in several steps
          out_target_position_color_radius_particle.xy
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_pos_speed , decalCoords ).xy;
          out_target_position_color_radius_particle.z
            = target_color_radius.r * 255. + target_color_radius.g * 65025. + target_color_radius.b * 16581375.;
          out_target_position_color_radius_particle.w = part_size;
      }
      else { // instant positioning on the target
          out_position_speed_particle
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_pos_speed , decalCoords );
          // out_position_speed_particle.xy *= vec2(width,height);
          // out_position_speed_particle = vec4(decalCoords,1,1);
          out_color_radius_particle = vec4(target_color_radius.rgb, part_size);
          // out_color_radius_particle = vec4(1,1,1,10);
      }
      return;
    }
    // camera
    else if(part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES ) {
        if(partMove_target) { // reaches a target in several steps
          out_target_position_color_radius_particle.xy
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_col_rad , decalCoords ).xy;
          vec2 cameraCoord = out_position_speed_particle.xy / vec2(width, height) * cameraWH;
          vec4 target_color = texture( uniform_ParticleAnimation_texture_fs_Camera_frame , vec2(cameraCoord.x , cameraWH.y - cameraCoord.y) );
          out_target_position_color_radius_particle.z
            = target_color.r * 255. + target_color.g * 65025. + target_color.b * 16581375.;
          out_target_position_color_radius_particle.w = part_size;
      }
      else { // instant positioning on the target
          out_position_speed_particle
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_pos_speed , decalCoords );
          out_position_speed_particle.zw = vec2(0,0);
          vec2 cameraCoord = out_position_speed_particle.xy / vec2(width, height) * cameraWH;
          vec4 target_color = texture( uniform_ParticleAnimation_texture_fs_Camera_frame , vec2(cameraCoord.x , cameraWH.y - cameraCoord.y) );
          out_color_radius_particle = vec4(target_color.rgb, part_size);
      }
    }
    // movie
    else if(part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES + 1 ) {
      if(partMove_target) { // reaches a target in several steps
          out_target_position_color_radius_particle.xy
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_pos_speed , decalCoords ).xy;
          vec2 movieCoord = out_position_speed_particle.xy / vec2(width, height) * movieWH;
          vec4 target_color = texture( uniform_ParticleAnimation_texture_fs_Movie_frame , vec2(movieCoord.x, movieWH.y - movieCoord.y)  );
          out_target_position_color_radius_particle.z
            = target_color.r * 255. + target_color.g * 65025. + target_color.b * 16581375.;
          out_target_position_color_radius_particle.w = part_size;
      }
      else { // instant positioning on the target
          out_position_speed_particle
                = texture( uniform_ParticleAnimation_texture_fs_Part_init_pos_speed , decalCoords );
          out_position_speed_particle.zw = vec2(0,0);
          vec2 movieCoord = out_position_speed_particle.xy / vec2(width, height) * movieWH;
          vec4 target_color = texture( uniform_ParticleAnimation_texture_fs_Movie_frame , vec2(movieCoord.x, movieWH.y - movieCoord.y)  );
          out_color_radius_particle = vec4(target_color.rgb, part_size);
      }
    }
    return;
  }
  // random initialization of the particle positions (initial speed is null currently)

  //////////////////////////////////////////////////////////////////////
  // REPOP ALONG PATHS (A RANDOM PATH AMONG THE ACTIVE ONES IS CHOSEN AT EACH FRAME)
  // SINCE REPOPULATION IS AN ADDITIVE PROCESS, SEVERAL SOURCES OF PARTICLES CAN
  // EXIST SIMULTANEOUSLY
  int repopChannel = int(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo.z);
  vec4 randomValue = texture( uniform_ParticleAnimation_texture_fs_Noise , vec3( decalCoordsPOT , 0.75 ) );
  vec4 radius_random = uniform_ParticleAnimation_path_data[repopChannel * PG_MAX_PATH_ANIM_DATA + PG_PATH_ANIM_RAD];
  if( repop_path > 0
    && rand3D(vec3(decalCoordsPOT, radius_random.y), repop_path) != 0) {
    vec4 pen_pos_prev_cur 
      = uniform_ParticleAnimation_path_data[repopChannel * PG_MAX_PATH_ANIM_DATA + PG_PATH_ANIM_POS];
    if( repopChannel >= 0 && pen_pos_prev_cur.z > 0 && pen_pos_prev_cur.w > 0 ) {
     /////////////////////////////////////////////////////////////////////
      vec2 pen_prev = vec2( pen_pos_prev_cur.x, height - pen_pos_prev_cur.y);
      vec2 pen_curr = vec2( pen_pos_prev_cur.z, height - pen_pos_prev_cur.w);
      vec2 orth = pen_prev - pen_curr;
      if(length(orth) != 0) {
        orth.xy = orth.yx;
        orth.y *= -1;
        orth = normalize(orth);
      }
      out_position_speed_particle.xy 
        = mix(pen_curr, pen_prev, randomPart.z ); // linear position
      out_position_speed_particle.xy += orth * 2 * (randomPart.w - 0.5) * radius_random.x; // thickness
      out_position_speed_particle.zw = (randomValue.xy - vec2(0.5)) * vec2(0.1); // speed
      out_color_radius_particle 
          = vec4( uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo.xyz , 
                  part_size);
    }
  }

  //////////////////////////////////////////////////////////////////
  // // REPOP
  // Uniform repopulation
  if( out_position_speed_particle.x == -10000) {
    // if the pixel noise is equal to frame % 8500 the cell is repopulated with a pixel
    
    vec4 randomValue = texture( uniform_ParticleAnimation_texture_fs_Noise , vec3( decalCoordsPOT , 0.25 ) );
    vec4 radius_random = uniform_ParticleAnimation_path_data[0 * PG_MAX_PATH_ANIM_DATA + PG_PATH_ANIM_RAD];
    if( Part_repop_density >= 0 && repop_part > 0) {
          repop_density_weight = texture(uniform_ParticleAnimation_texture_fs_RepopDensity,decalCoords).r;
    }
    // particle "ADDITION"
    if( repop_part > 0
        && rand3D(vec3(decalCoordsPOT, radius_random.z), repop_part * repop_density_weight) != 0) {
         /////////////////////////////////////////////////////////////////////
        // RANDOM INITIALIZATION
        // head and tail are initialized with the same values
        // reaches for regular grid position
        float rank = (decalCoords.x + decalCoords.y * width) / float(nbParticles);
        int rankGrid = int(rank * width * height);
        out_position_speed_particle.xy 
          = vec2( rankGrid % int(width) , rankGrid / int(width) ); // position
        out_position_speed_particle.zw = (randomValue.xy - vec2(0.5)) * vec2(0.1); // speed
        out_color_radius_particle 
          = vec4(uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo.xyz,part_size);
    }
  }

  //////////////////////////////////////////////////////////////////
  // FLASH CALCULATION AT PARTICLE PRECEDING POSITION
  // track 0 flash on particles
  flashToPartCumul = uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[0] *
                       texture( uniform_ParticleAnimation_texture_fs_Trk0 , out_position_speed_particle.xy ).rgb;
#if PG_NB_TRACKS >= 2
  // track 1 flash on particles
  flashToPartCumul += uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[1] *
                       texture( uniform_ParticleAnimation_texture_fs_Trk1 , out_position_speed_particle.xy ).rgb;
#endif
#if PG_NB_TRACKS >= 3
  // track 2 flash on particles
  flashToPartCumul += uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[2] *
                       texture( uniform_ParticleAnimation_texture_fs_Trk2 , out_position_speed_particle.xy ).rgb;
#endif
#if PG_NB_TRACKS >= 4
  // track 3 flash on particles
  flashToPartCumul += uniform_ParticleAnimation_fs_4fv_flashTrkPartWghts[3] *
                       texture( uniform_ParticleAnimation_texture_fs_Trk3 , out_position_speed_particle.xy ).rgb;
#endif
  // CA flash on particles
  flashToPartCumul += uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit.x
                    * texture( uniform_ParticleAnimation_texture_fs_CA , out_position_speed_particle.xy ).rgb;

  //////////////////////////////////////////////////////////////////
  // // COLOR MODE
  // const uint PARTCOLOR_WHITE = 0; // white particles
  // const uint PARTCOLOR_MAX = 1; // maximum of flash color and particle
  // const uint PARTCOLOR_COPY = 2; // copy flash color
  float grayFlash = graylevel(flashToPartCumul);
  if(partColor_mode == PARTCOLOR_WHITE) { // white particles
    out_color_radius_particle.rgb = vec3(1);
  }
  else if(partColor_mode == PARTCOLOR_MAX && grayFlash > 0) { // maximum of flash color and particle
    out_color_radius_particle.rgb = max(flashToPartCumul,out_color_radius_particle.rgb);
  }
  else if(partColor_mode == PARTCOLOR_COPY && grayFlash > 0) { // copy flash color
    out_color_radius_particle.rgb = flashToPartCumul;
  }

  //////////////////////////////////////////////////////////////////
  // STROKE MODE (DRAWING INTERACTION)
  // const uint PARTSTROKE_NONE = 0; 
  // const uint PARTSTROKE_AGGREG = 1; // aggregates on drawing
  // const uint PARTSTROKE_BOUNCE = 2; // bounces on drawing
  // const uint PARTSTROKE_DAMP = 3; // slows down on drawing

  // flash management
  // if(partStroke_mode == PARTSTROKE_NONE) { // no stroke effect
  // }
  // else 
  if(partStroke_mode == PARTSTROKE_AGGREG) { // aggregation
    if(grayFlash > 0.01)  {
        out_position_speed_particle.zw = vec2(0); 
        return;
    }
  }
  else if(partStroke_mode == PARTSTROKE_BOUNCE) { // repulses
    if(grayFlash > 0.01)  {
        out_position_speed_particle.zw *= vec2(-1); 
    }
  }
  else if(partStroke_mode == PARTSTROKE_DAMP) { // slows down on drawing
    if(grayFlash > 0.01)  {
        out_position_speed_particle.zw 
          -= grayFlash * 0.1 * out_position_speed_particle.zw;
    }
  }

  //////////////////////////////////////////////////////////////////
  // MOTION MODE
  // particle speed and position management (complemented by instructions in the 
  // geometry shader that allow for a coherent positioning of head and tail)    

  // THERE IS A UNIQUE TARGET FOR EACH PARTICLE 
  // THE TARGET CAN BE COLLECTIVE (EG THE PEN) AND EACH PARTICLE JOINS THE SAME TARGET
  // OR THE TARGET CAN BE INDIVIDUAL (EG AN INDIVIDUAL LOCATION ON A SCREEN GRID)
  // THE TIME TO REACH THE TARGET IS COUNTED DOWN

  ///////////////////////////////////////////////////////////////////
  // builds a path_follow or repulse vector so that it can be used in the for loop
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
  bvec4 path_follow03 = bvec4(part_path_follow_0,part_path_follow_1,part_path_follow_2,part_path_follow_3);
  bvec4 path_repulse03 = bvec4(part_path_repulse_0,part_path_repulse_1,part_path_repulse_2,part_path_repulse_3);
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
  bvec4 path_follow47 = bvec4(part_path_follow_4,part_path_follow_5,part_path_follow_6,part_path_follow_7);
  bvec4 path_repulse47 = bvec4(part_path_repulse_4,part_path_repulse_5,part_path_repulse_6,part_path_repulse_7);
#endif
#if PG_NB_PATHS == 11
  bvec4 path_follow811 = bvec4(part_path_follow_8,part_path_follow_9,part_path_follow_10,part_path_follow_11);
  bvec4 path_repulse811 = bvec4(part_path_repulse_8,part_path_repulse_9,part_path_repulse_10,part_path_repulse_11);
#endif

  dvec2 part_acceleration = dvec2(0);
  double dist_to_target = 1000;

  ///////////////////////////////////////////////////////////////////
  // PARTICLE: PATH FOLLOW OR REPULSE
  for( int indPath = 0 ; indPath <= PG_NB_PATHS ; indPath++ ) {
    vec4 pen_pos_prev_cur 
      = uniform_ParticleAnimation_path_data[indPath * PG_MAX_PATH_ANIM_DATA + PG_PATH_ANIM_POS];
    dvec2 curPos = dvec2( pen_pos_prev_cur.z, height - pen_pos_prev_cur.w );
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
    if( indPath < 4 && path_follow03[indPath] ) {
      // reaches for pen position
      part_acceleration 
        = dvec2(curPos - out_position_speed_particle.xy);
      dist_to_target = length(part_acceleration);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
    }
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
    else if( indPath >= 4 && path_follow47[indPath - 4] ) {
      // reaches for pen position
      part_acceleration 
        = dvec2(curPos - out_position_speed_particle.xy);
      dist_to_target = length(part_acceleration);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
   }
#endif
#if PG_NB_PATHS == 11
    else if( indPath >= 8 && path_follow47[indPath - 8] ) {
      // reaches for pen position
      part_acceleration 
        = dvec2(curPos - out_position_speed_particle.xy);
      dist_to_target = length(part_acceleration);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
   }
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
    if( indPath < 4 && path_repulse03[indPath] ) {
      // escapes from pen position
      part_acceleration 
        = dvec2(out_position_speed_particle.xy - curPos);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
    }
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
    else if( indPath >= 4 && path_repulse47[indPath - 4] ) {
      // reaches for pen position
      // escapes from pen position
      part_acceleration 
        = dvec2(out_position_speed_particle.xy - curPos);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
    }
#endif
#if PG_NB_PATHS == 11
    else if( indPath >= 8 && path_repulse47[indPath - 8] ) {
      // reaches for pen position
      // escapes from pen position
      part_acceleration 
        = dvec2(out_position_speed_particle.xy - curPos);
      // adds some field disturbance
      part_acceleration += dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
    }
#endif
  }

  ///////////////////////////////////////////////////////////////////
  // PARTICLE: MOTION TOWARDS A TARGET
  if(partMove_target && frameNo < targetFrameNo && part_initialization < 0) { // reaches a grid
    // reaches for a target in a certain number of steps
    // float rank = (decalCoords.x + decalCoords.y * width) / float(nbParticles);
    // int rankGrid = int(rank * width * height);
    part_acceleration 
      = dvec2(out_target_position_color_radius_particle.xy
                  - out_position_speed_particle.xy);// * (1. - (targetFrameNo - frameNo)/part_timeToTargt);
    dist_to_target = length(part_acceleration);

    int color = int(out_target_position_color_radius_particle.z);

    vec4 targetColorRadius 
          = vec4((color%255)/255.,((color/255)%255)/255.,((color/65025)%255)/255.,
                  out_target_position_color_radius_particle.w);
    out_color_radius_particle 
          = mix(targetColorRadius,out_color_radius_particle,(targetFrameNo - frameNo)/part_timeToTargt);

    // ALSO INTERPOLATE COLOR
  }

  ///////////////////////////////////////////////////////////////////
  // PARTICLE INITIALIZATION TOWARDS A TARGET OR AN INSTANT POSITIONING
  // NO INITIAL ACCELERATION
  if(part_initialization >= 0) {
    part_acceleration = dvec2(0);
  }

  ///////////////////////////////////////////////////////////////////
  // PARTICLE: RANDOM MOTION
  if(partMove_rand) { // random motion
    // random motion
    part_acceleration = dvec2(randomPart.xy - dvec2(0.55));
  }

  ///////////////////////////////////////////////////////////////////
  // PARTICLE: GRAVITY
  if(part_gravity != 0) { // random motion
    // random motion
    part_acceleration += dvec2(0, part_gravity);
  }

  if(length(part_acceleration) > 0) {
    part_acceleration = normalize(part_acceleration);
  }

  //////////////////////////////////////////////////////////////////
  // SPEED UPDATE FROM ACCELERATION AND DAMPING
  dvec2 speed2D;
  // texture based acceleration shift
  if(part_image_acceleration >= 0) {
    float rot_angle = part_field_weight * texture( uniform_ParticleAnimation_texture_fs_Part_acc , out_position_speed_particle.xy ).r;
    float cosa = cos(rot_angle);
    float sina = sin(rot_angle);
    part_acceleration  
      = dmat2(cosa, -sina, sina, cosa) * part_acceleration;
  }
  // speed update from acceleration vector part_acceleration 
  // (with a factor equal to part_acc controlled by interface or scenario)
  speed2D 
    = out_position_speed_particle.zw + dvec2(part_acc * part_acceleration);
  // damping with a factor equal to part_damp controlled by interface or scenario
  speed2D
    -= dvec2(part_damp * speed2D);

  // reading the noise value for acceleration 
  double speed = length(speed2D);
  if(part_image_acceleration < 0) {
  speed2D  
    = normalize(speed2D 
      + part_field_weight * dvec2(generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center)) * speed;
  }

  // speed damping to reach a target
  if( dist_to_target < part_damp_targtRad ) {
      speed2D -= 0.01 * dvec2((part_damp_targtRad - dist_to_target) / part_damp_targtRad) * speed2D;
  }

  // position update from new speed value
  // with speed limit
  speed =length(speed2D);
  if(speed > 50) {
    speed2D *= double(50/speed);
  }
  if(speed < 0.0001) {
    speed2D = dvec2(generativeNoise(pixelTextureCoordinatesXY));
  }

  out_position_speed_particle.zw = vec2(speed2D);
  out_position_speed_particle.xy += out_position_speed_particle.zw;

  //////////////////////////////////////////////////////////////////
  // EXIT MODE
  // const uint PARTEXIT_NONE = 0; // lost when exits
  // const uint PARTEXIT_OPPOSITE = 1; // when going out right/left/top/bottom reappers left/right/bottom/top
  // const uint PARTEXIT_BOUNCE = 2; // bounces on edges
  // off screen particle management (complemented by instructions in the geometry shader
  // that allow for a coherent positioning of head and tail)    
  if(partExit_mode == PARTEXIT_NONE) { // lost when exits
    if(out_position_speed_particle.x < -3000 || out_position_speed_particle.x > width + 2000
      || out_position_speed_particle.y < -3000  || out_position_speed_particle.y > height + 2000) {
      out_position_speed_particle.xy = vec2(-10000); 
      out_position_speed_particle.zw = vec2(0); 
      return;
    }
  }
  if(partExit_mode == PARTEXIT_OPPOSITE) { // reflection on the edge of the screen
    if(out_position_speed_particle.x < 1) {
      out_position_speed_particle.x = width - 2;
    }
    if(out_position_speed_particle.x > width - 2) {
      out_position_speed_particle.x = 1;
    }
    if(out_position_speed_particle.y < 1) {
      out_position_speed_particle.y = height - 2;
    }
    if(out_position_speed_particle.y > height - 2) {
      out_position_speed_particle.y = 1;
    }
  }
  if(partExit_mode == PARTEXIT_BOUNCE) { // enters the opposite side of where it leaves
    if(out_position_speed_particle.x < 1) {
        out_position_speed_particle.z *= -1;
        out_position_speed_particle.x = 1;
    }
    if(out_position_speed_particle.x > width - 2) {
        out_position_speed_particle.z *= -1;
        out_position_speed_particle.x = width - 2;
    }
    if(out_position_speed_particle.y < 1) {
        out_position_speed_particle.w *= -1;
        out_position_speed_particle.y = 1;
    }
    if(out_position_speed_particle.y > height - 2) {
        out_position_speed_particle.w *= -1;
        out_position_speed_particle.y = height - 2;
    }
    return;
  }

  ////////////////////////////////////////////////////////////////////
  // NON HEAD VERTICES POSITION UPDATE ACCORDING TO HEAD POSITION 
  // IN THE SAME PRIMITIVE (CURVE)
/* #ifdef CURVE_PARTICLES
  }
  // for the rest of the curve, it follows the head
  else {
    vec4 headout_position_speed_particle 
          = texture(uniform_ParticleAnimation_texture_fs_Part_pos_speed, decalCoords-vec2(index,0));

    float headCurrentSpeed = length( headout_position_speed_particle.zw);
    vec2  lookatHead = headout_position_speed_particle.xy - out_position_speed_particle.xy;
    vec2 normalizedLookAt = normalize(lookatHead);

    // particle cannot be stretched too much
    if( length(lookatHead) > 10) {
      out_position_speed_particle.xy += normalizedLookAt * (length(lookatHead) - 10);
    }
    // speed calculation from head speed and position
    out_position_speed_particle.zw = headCurrentSpeed * (0.3 * index) * normalizedLookAt;
    // oscillation of non tail or head points
    // if(index ==1 || index==2) {
    //   out_position_speed_particle.zw += (snoise( decalCoords + vec2(9.4872,2.0937) , noiseParticleScale * 100 ) - 0.5)
    //     * vec2(lookatHead.y,-lookatHead.x);
    // }

    // position update
    out_position_speed_particle.xy += out_position_speed_particle.zw;
  }
#endif
 */
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN OF UPDATE PASS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void main() {
#include_initializations

  //////////////////////////
  // variables 

  // pixels position speed update parameters
  pixel_acc_center = vec2(pixel_acc_shiftX,pixel_acc_shiftY);

  // frame number
  frameNo = int(round(uniform_ParticleAnimation_fs_4fv_repop_Color_frameNo.w));

  // target Frame number
  targetFrameNo = int(round(uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo.w));

  // working variables for screen dimension
  width = uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo.x;
  height = uniform_ParticleAnimation_fs_4fv_W_H_repopChannel_targetFrameNo.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // pixel texture + z offset according to the chosen texture
  vec2 position = vec2( 1.0 + sin(frameNo/50000.0),
                        1.0 + cos(frameNo/37000.0));
  vec2 noisePositionOffset = vec2(snoise( position , noiseParticleScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  vec2  pixelTextureCoordinatesXY = decalCoordsPOT + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 

  // noise for CA: random value
  randomPart = texture( uniform_ParticleAnimation_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.0 ) );

  nbParticles = int(uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit.y);

  
  
  

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING 
  // AND COPY OF PRECEDING FRAME FBO VALUES
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////

  if(frameNo <= 10 || uniform_ParticleAnimation_fs_4fv_flashCAPartWght_nbPart_clear_nbPartInit.z > 0) {
    out_ParticleAnimation_FBO_fs_Part_pos_speed = vec4(-10000,-10000,0,0);  // particle position / speed
    out_ParticleAnimation_FBO_fs_Part_col_rad = vec4(1,1,1,1);  // particle color / radius
    out_ParticleAnimation_FBO_fs_Part_Target_pos_col_rad = vec4(-10000,-10000,16646655,1);  // particle target position / color / radius
    return;
  }

  ///////////////////////////////////////////////////
  // COPY OF PRECEDING FRAME FBO VALUES

  // CA, pixels and particle parameters FBO copy
  out_attachment_FBO[pg_Part_pos_speed_FBO_ParticleAnimation_attcht]
    = texture( uniform_ParticleAnimation_texture_fs_Part_pos_speed , decalCoords );
  out_attachment_FBO[pg_Part_col_rad_FBO_ParticleAnimation_attcht]
    = texture( uniform_ParticleAnimation_texture_fs_Part_col_rad , decalCoords );
  out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht]
    = texture( uniform_ParticleAnimation_texture_fs_Part_Target_pos_col_rad , decalCoords );

  if( freeze ) {
    // FBO OUTPUTS
    out_ParticleAnimation_FBO_fs_Part_pos_speed = out_attachment_FBO[pg_Part_pos_speed_FBO_ParticleAnimation_attcht];  // particle position / speed
    out_ParticleAnimation_FBO_fs_Part_col_rad = out_attachment_FBO[pg_Part_col_rad_FBO_ParticleAnimation_attcht];  // particle color / radius
    out_ParticleAnimation_FBO_fs_Part_Target_pos_col_rad = out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht];  // particle target position / color / radius
      return;
  }


  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // VIDEO UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // each track possibly covers the previous color

  // VIDEO FRAME CAPTURE AND RENDERING
  // movie size
  movieWH = uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH.zw;
  // camera size
  cameraWH = uniform_ParticleAnimation_fs_4fv_Camera_W_H_movieWH.xy;

 
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // PARTICLE POSITION/SPEED UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  if(decalCoords.y * width + decalCoords.x < nbParticles) {
    // updates particle according to mode of updating
    // flashes of track and CA on Particles
    particle_out();

    //////////////////////////////////////////////
    // particle decay
    if( graylevel(out_color_radius_particle.rgb) > 0 ) {
      vec3 decayedColor = out_color_radius_particle.rgb - vec3(partDecay);
      float maxDecayedColor = maxCol( decayedColor );
      if( maxDecayedColor <= 1.0 ) {
        out_color_radius_particle.rgb = decayedColor;
      }
    }
    out_color_radius_particle.rgb 
      = clamp( out_color_radius_particle.rgb , 0.0 , 1.0 );

    //////////////////////////////////////////////
    // resets pixel speed and position if trackO returns to black
    if( graylevel(out_color_radius_particle.rgb) <= 0 ) {
      out_position_speed_particle.xy = vec2(-10000); 
      out_position_speed_particle.zw = vec2(0); 
    }

    out_attachment_FBO[pg_Part_pos_speed_FBO_ParticleAnimation_attcht] = out_position_speed_particle;
    out_attachment_FBO[pg_Part_col_rad_FBO_ParticleAnimation_attcht] = out_color_radius_particle;
    out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht] = out_target_position_color_radius_particle;
  }


  //////////////////////////////////////////////
  //////////////////////////////////////////////
  // FBO OUTPUTS 
  //////////////////////////////////////////////
  //////////////////////////////////////////////
  out_ParticleAnimation_FBO_fs_Part_pos_speed = out_attachment_FBO[pg_Part_pos_speed_FBO_ParticleAnimation_attcht];
  out_ParticleAnimation_FBO_fs_Part_col_rad = out_attachment_FBO[pg_Part_col_rad_FBO_ParticleAnimation_attcht];
  out_ParticleAnimation_FBO_fs_Part_Target_pos_col_rad = out_attachment_FBO[pg_Part_Target_pos_col_rad_FBO_ParticleAnimation_attcht];
}
