/***********************************************************************
File: effe/shaders/LYM_Drawing_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#include_declarations

#define PG_VIDEO_ACTIVE
#define PG_NB_TRACKS 4
#define PG_WITH_CA

const float PI = 3.1415926535897932384626433832795;

// #define GENERATIVE_MOVIE

float dist_to_mousePos = 0;
float unsigned_rx;
float unsigned_ry;
float signed_rx;
float signed_ry;
vec3 out_drawn_filled;

// center of the translation of acceleration values
// used for shifting particle motion in a direction
vec2 partAccCenter = vec2(0.5,0.5);

// out_particle returned values
vec3 out_color_particle;
vec2 out_position_particle = vec2(0,0);
vec2 out_speed_particle = vec2(0,0);

#ifdef PG_WITH_CA
vec4 out4_CA;
float CAdecay = 0.0;
bool CA_on_off;

// CA types
#define CA_PREY                    0
#define CA_DISLOCATION             1
#define CA_PROTOCELLS              2
#define CA_SOCIAL_PD               3
#define CA_PATHS                   4
#define CA_COLOR                   5
#define CA_NOTYPE                  6
#endif

#ifdef PG_VIDEO_ACTIVE
  vec3 cameraImage = vec3( 0.0 );
  vec3 movieImage = vec3( 0.0 );
  vec2 movieCoord = vec2(0.0);
#endif

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or particles
bool newly_drawn = false;

// cursor type (+1 for stylus , -1 for rubber )
float Cursor = 1;

vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesHV[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
                                    vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// Particle variables
float acceleration_factor;

float height;
float width;

vec2 movieWH;

vec2 particleTextureCoordinatesXY; // the POT coordinates of the
// particle texture + z offset according to the chosen texture
vec2 noiseParticles;
float radiusParticle;
vec4 randomCA;
vec3 randomCA2;

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))
#define delta(gray) (gray>0.5?1:0)

// SOBEL OFFSETS
vec2 offsetsVideo[9] = {vec2( -1, 1 ),vec2( 0, 1 ),vec2( 1, 1 ),
                      vec2( 0, -1 ),vec2( 0, 0 ),vec2( 0, 1 ),
                      vec2( -1, -1 ),vec2( 0, -1 ),vec2( 1, -1 )};
float sobelMatrixX[9] = {-1,0,1,-2,0,2,-1,0,1};
float sobelMatrixY[9] = {1,2,1,0,0,0,-1,-2,-1};


// CA OFFSETS
const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E NE N NW
                                 {1,1},{-1,1},{-1,-1},{1,-1}};  // W SW S SE
const vec2 neighborOffsetsPaths[8] = {{1,0},{2,0},{0,1},{0,2},      // EE EE N NN
                                      {-1,0},{-2,0},{0,-1},{0,-2}};  // W WW S SS
const vec2 neighborOffsetsNonPaths[16] = {{1,1},{2,1},{1,2},{2,2},
                                          {-1,-1},{-2,-1},{-1,-2},{-2,-2},
                                          {1,-1},{2,-1},{1,-2},{2,-2},
                                          {-1,1},{-2,1},{-1,2},{-2,2}};  // W WW S SS
const float neighborDistances[8]={1.0,0.707,1.0,0.707,1.0,0.707,1.0,0.707};


#ifdef PG_WITH_CA
const float shadePrey[5] = {0,0.4,              // dead predator/prey
                                 0.6, 0.8,  1.0};       // prey hungry/full predator
const float inverseShadePrey[5] = {0.0,2.5,              // dead predator/prey
                                 1.67,1.25,  1.0};       // prey hungry/full predator
// const vec3 colorPrey[5] = {{0,0,0},{0.25,0.25,0.25},   // dead predator/prey
//                            {0.75,0.75,0.75},           // prey
//                            {0.5,0.5,0.5},{1,1,1}};  // hungry/full predator
const float shadeDisloc[5] = {0,0.4,              // empty new/old border
                                 0.6, 0.8,  1.0};       // new/old nucleus
const float inverseShadeDisloc[5] = {0.0,2.5,              // empty new/old border
                                 1.67,1.25,  1.0};       // new/old nucleus

// const vec3 colorDisloc[5] = {{0,0,0},     // empty
//                              {1.0,1.0,1.0},{0.6,0.6,0.6},   // new/old border
//                              {1.0,1.0,1.0},{0.3,0.3,0.3}};  // new/old nucleus

const float shadeProtocell[4] = {0,               // empty
                                 0.4,0.8,         // X Y
                                 1.0};            // Z
const float inverseShadeProtocell[4] = {0,                 // empty
                                        2.5, 1.25,         // X Y
                                        1.0};            // Z
const float shadeSocial[3] = {0,               // empty
                                 0.5, 1.0};    // Coorperator / Defector
const float inverseShadeSocial[3] = {0,                 // empty
                                        2.0 , 1.0};     // Coorperator / Defector
#endif

// frame number
int frameNo;

// PARTICLE mode
#define PARTICLE_NONE   0
#define PARTICLE_UNIQUE 1
#define PARTICLE_FUR    2
#define PARTICLE_MULTIPLY    3

// drawing modes
#define DRAWING_VOID   0
#define DRAWING_LINE   1
#define DRAWING_POINT  2

int drawingStroke;

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_x;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_x_prev;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y_prev;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_r;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_g;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_b;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_a;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_BrushID;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusX;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusY;
uniform vec3 uniform_Drawing_fs_3fv_flashTrack0Coefs;  
uniform vec4 uniform_Drawing_fs_4fv_flashCACoefs;  
uniform vec4 uniform_Drawing_fs_4fv_trackdecay;
uniform vec3 uniform_Drawing_fs_3fv_CAdecay_frameno_Cursor;
uniform vec4 uniform_Drawing_fs_4fv_partAcc_clearAllLayers_clearCA_radiusPart;
uniform vec4 uniform_Drawing_fs_4fv_pulse;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec4 uniform_Drawing_fs_4fv_movieWH_flashCameCoef_cpTrack;
uniform vec4 uniform_Drawing_fs_4fv_flashBackCoef_repop_Color;
uniform vec4 uniform_Drawing_fs_4fv_isClearLayer_flashPart;
uniform vec4 uniform_Drawing_fs_4fv_photo01_wh;
uniform vec2 uniform_Drawing_fs_2fv_photo01_weights;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal;         // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass speed/position of particles step n (FBO attachment 2)
#ifdef PG_WITH_CA
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
#endif
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // 2-cycle ping-pong Drawing pass track 2 step n (FBO attachment 5)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // 2-cycle ping-pong Drawing pass track 3 step n (FBO attachment 6)
#endif
layout (binding = 6) uniform sampler3D   fs_lookupTable6;  // pen patterns
#ifdef PG_VIDEO_ACTIVE
layout (binding = 7) uniform samplerRect fs_lookupTable7;  // camera texture
layout (binding = 8) uniform samplerRect fs_lookupTable8;  // movie texture
#endif
layout (binding = 9) uniform sampler3D   fs_lookupTable9;  // noise texture
layout (binding = 10) uniform samplerRect fs_lookupTable10;  // photo_0 texture
layout (binding = 11) uniform samplerRect fs_lookupTable11;  // photo_1 texture

/////////////////////////////////////
// BACKGROUND COLOR OUTPUT
layout (location = 0) out vec4 outColor0;
// the local color copied from the ping pong color FBO
// the out_track_FBO[0] and out_CA_FBO are copied from the preceding buffer
// value unless we are in the CA mode where it is averaged
// between neighbors
// the ALPHA canal contains the CA gray level 
// the RGB channels contain the local color

/////////////////////////////////////
// PARTICLE OUTPUT
layout (location = 1) out vec4 outColor1;      // 
// the RG channels contain the particle speed
// the BA channels contain the particle position

#ifdef PG_WITH_CA
/////////////////////////////////////
// CA & TRACK COLOR OUTPUT
layout (location = 2) out vec4 outColor2;      // 
#if PG_NB_TRACKS >= 2
layout (location = 3) out vec4 outColor3;      // 
#endif
#if PG_NB_TRACKS >= 3
layout (location = 4) out vec4 outColor4;      // 
#endif
#if PG_NB_TRACKS >= 4
layout (location = 5) out vec4 outColor5;      // 
#endif
// the RGB channels contains the track Color
#else
/////////////////////////////////////
// TRACK COLOR OUTPUT
#if PG_NB_TRACKS >= 2
layout (location = 2) out vec4 outColor2;      // 
#endif
#if PG_NB_TRACKS >= 3
layout (location = 3) out vec4 outColor3;      // 
#endif
#if PG_NB_TRACKS >= 4
layout (location = 4) out vec4 outColor4;      // 
#endif
#endif

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
float snoise(vec2 v , float noiseScale) {
    v *= noiseScale;

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


float Line( float x1 , float y1 , 
	    float mouse_x , float mouse_y , float interp_mouse_x ) {
  return mouse_y + (y1 - mouse_y)/(x1 - mouse_x) * (interp_mouse_x - mouse_x);
}

#ifdef PG_WITH_CA
float payoffCalc( float state1 , float state2) {
  if(state1==1.0) {
    if (state2==1.0) {
      return CAParams1;
    }
    else {
      return CAParams3;
    }
  }
  else {
    if (state2==1.0) {
      return CAParams2;
    }
    else {
      return CAParams4;
    }
  }
}

void CA_out( vec2 decalCoords , vec4 currentCA ) {
  // the ALPHA canal of fs_lookupTable2 contains > 0 if it is a live cell

  // gets Moore neighbor values
  neighborValues[0] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[0] );
  neighborValues[1] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[1] );
  neighborValues[2] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[2] );
  neighborValues[3] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[3] );
  neighborValues[4] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[4] );
  neighborValues[5] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[5] );
  neighborValues[6] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[6] );
  neighborValues[7] = texture(fs_lookupTable2, 
            decalCoords + neighborOffsets[7] );

#ifndef GENERATIVE_MOVIE
  //////////////////////////////////////////////////////
  // PREY/PREDATOR
  if( CAType == CA_PREY ) {
    int nbSurroundingHungryPredators =
      (round(neighborValues[0].a) == 3? 1:0) +
      (round(neighborValues[1].a) == 3? 1:0) +
      (round(neighborValues[2].a) == 3? 1:0) +
      (round(neighborValues[3].a) == 3? 1:0) +
      (round(neighborValues[4].a) == 3? 1:0) +
      (round(neighborValues[5].a) == 3? 1:0) +
      (round(neighborValues[6].a) == 3? 1:0) +
      (round(neighborValues[7].a) == 3? 1:0);
    
    int nbSurroundingFullPredators =
      (round(neighborValues[0].a) == 4? 1:0) +
      (round(neighborValues[1].a) == 4? 1:0) +
      (round(neighborValues[2].a) == 4? 1:0) +
      (round(neighborValues[3].a) == 4? 1:0) +
      (round(neighborValues[4].a) == 4? 1:0) +
      (round(neighborValues[5].a) == 4? 1:0) +
      (round(neighborValues[6].a) == 4? 1:0) +
      (round(neighborValues[7].a) == 4? 1:0);
    
    int nbSurroundingPreys =
      (round(neighborValues[0].a) == 2? 1:0) +
      (round(neighborValues[1].a) == 2? 1:0) +
      (round(neighborValues[2].a) == 2? 1:0) +
      (round(neighborValues[3].a) == 2? 1:0) +
      (round(neighborValues[4].a) == 2? 1:0) +
      (round(neighborValues[5].a) == 2? 1:0) +
      (round(neighborValues[6].a) == 2? 1:0) +
      (round(neighborValues[7].a) == 2? 1:0);
    
    int nbNeighbors =
      (round(neighborValues[0].a) != 0? 1:0) +
      (round(neighborValues[1].a) != 0? 1:0) +
      (round(neighborValues[2].a) != 0? 1:0) +
      (round(neighborValues[3].a) != 0? 1:0) +
      (round(neighborValues[4].a) != 0? 1:0) +
      (round(neighborValues[5].a) != 0? 1:0) +
      (round(neighborValues[6].a) != 0? 1:0) +
      (round(neighborValues[7].a) != 0? 1:0);
    
    uint nbStates = 5;
    uint newState = 0;
    
     vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadePrey[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadePrey[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadePrey[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadePrey[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadePrey[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadePrey[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadePrey[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadePrey[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadePrey[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadePrey[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadePrey[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

   // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = round(randomCA.x * (nbStates+1)); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      out4_CA.rgb = shadePrey[newState].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;

      // dead predator 
      if( state == 0 ) {
        // prey birth probability if preys are in neighborhood
        if( nbSurroundingPreys > 0
            && pow( randomCA.x , nbSurroundingPreys ) 
               < CAParams1 ) {
          newState = 2; // becomes a prey
        }
        // else {
        //   out4_CA.a = currentCA.a; // remains dead predator
        // }
      }
      // dead prey 
      else if( state == 1 ) {
        // predator birth probability if full predators are in neighborhood
        if( nbSurroundingFullPredators > 0
            && pow( randomCA.x , nbSurroundingFullPredators ) 
               < CAParams2 ) {
          newState = 3; // becomes a hungry predator
        }
        // else {
        //   newState = currentCA.a; // remains dead prey
        // }
      }
      // prey 
      else if( state == 2 ) {
        // death probability if hungry predators are in neighborhood
        if( nbSurroundingHungryPredators > 0
            && pow( randomCA.x , nbSurroundingHungryPredators ) 
               < CAParams3 ) {
          newState = 1; // dead prey
        }
        // else {
        //   newState = currentCA.a; // survives
        // }
      }
      // hungry predator 
      else if( state == 3 ) {
        // probability of eating if preys are in the neighborhood
        if( nbSurroundingPreys > 0
            && pow( randomCA.x , nbSurroundingPreys ) 
               < CAParams3 ) {
          newState = 4; // survives in state full
        }
        else { // dies with a certain probability
          if( randomCA.x < CAParams4 ) {
            newState = 0;  // dead predator
          }
          // else {
          //   newState = currentCA.a; // survives
          // }
        }
      }
      // full predator 
      else if( state == 4 ) {
        // probability of becoming hungry
        if( randomCA.x < CAParams5 ) {
          newState = 3; // becomes hungry
        }
        // else { 
        //   newState = currentCA.a; // stays in state full
        // }
      }
      else {
        newState = 0;
      }
      // newState = state;
      // out4_CA.a = float(newState);
      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadePrey[newState] * averageSurrounding;
      // out4_CA.rgb = vec3(shadePrey[newState]);
    }
  }

  //////////////////////////////////////////////////////
  // DISLOCATION
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_DISLOCATION ) {
    int nbSurroundingNewBorders =
      (neighborValues[0].a == 1? 1:0) +
      (neighborValues[1].a == 1? 1:0) +
      (neighborValues[2].a == 1? 1:0) +
      (neighborValues[3].a == 1? 1:0) +
      (neighborValues[4].a == 1? 1:0) +
      (neighborValues[5].a == 1? 1:0) +
      (neighborValues[6].a == 1? 1:0) +
      (neighborValues[7].a == 1? 1:0);
    
    int nbSurroundingOldBorders =
      (neighborValues[0].a == 2? 1:0) +
      (neighborValues[1].a == 2? 1:0) +
      (neighborValues[2].a == 2? 1:0) +
      (neighborValues[3].a == 2? 1:0) +
      (neighborValues[4].a == 2? 1:0) +
      (neighborValues[5].a == 2? 1:0) +
      (neighborValues[6].a == 2? 1:0) +
      (neighborValues[7].a == 2? 1:0);

    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeDisloc[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeDisloc[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeDisloc[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeDisloc[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeDisloc[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeDisloc[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeDisloc[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeDisloc[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeDisloc[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeDisloc[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeDisloc[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    int nbNeighbors = nbSurroundingNewBorders + nbSurroundingOldBorders;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    // 0 empty
    // 1 new border
    // 2 old border: border at preceding state
    // 3 new nucleus
    // 4 new nucleus at preceding step
    uint nbStates = 5;
    uint newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      // newState = 3;
      out4_CA.rgb = shadeDisloc[newState].r * currentCA.rgb;
    }
    else {
      int indexOld = -1;
      int firstindex = int(randomCA.x * 8);
      // looks for an old border around
      for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
        if( neighborValues[ind % 8].a == 2 ) {
          indexOld = ind;
        }
      }
      int indexNew = -1;
      firstindex = int(randomCA.y * 8);
      // looks for a new border around
      for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
        if( neighborValues[ind % 8].a == 1 ) {
          indexNew = ind;
        }
      }

      uint state = int(clamp(currentCA.a,0,nbStates));
      // parameters
      // w: fluidity (penetration of old borders between cells)
      // z : celle density
      // y : permeability
      // x : freezing/dissolve

      // empty cell transformation
      if( state ==  0 ) {
        newState = 0; // stays empty
        // found a new border at indexNew
        if( nbSurroundingNewBorders == 1  ) {
          // prolongs the border
          newState = 1; // new border
        }
        else if( nbSurroundingOldBorders == 1 
                && randomCA.y < CAParams2 ) {
          // prolongs the border
          newState = 2; // old border
        }
        else if( nbSurroundingNewBorders >= CAParams1 * 8.0  ) {
          // creates a nucleus
          newState = 3; // nucleus
        }
      }
      // new border growth: new border pushed by old border
      else if( state ==  1 ) {
        newState = 1; // stays a border
        if( nbSurroundingNewBorders >= CAParams5 * 10.0 ) {
          newState = 3; //makes a nucleus
        }
        // found an old border at indexOld
        else if( indexOld >= 0 && neighborValues[(indexOld + 4) % 8].a == 0 ) {
          // a new border can be established in diametrically opposed cell
          newState = 2; // becomes an old border
        }
      }
      // border growth: old border pulled by new border
      else if( state ==  2 ) {
        newState = 2; // stays old border
        if( nbSurroundingOldBorders >= CAParams6 * 10.0 ) {
          newState = 0; //makes a hole
        }
        else if( indexNew >= 0 ) {
          newState = 0; // becomes an empty cell
        }
      }
       // nucleus
      else if( state ==  3 ) {
        newState = 3; // stays a nucleus
        if( nbSurroundingOldBorders + nbSurroundingNewBorders 
             >= CAParams4 * 16.0 ) {
          newState = 0; //makes a hole
        }
        else 
          if( randomCA.y < CAParams3 ) {
          newState = 1; // becomes a border
        }
      }
      // newState = state;
      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeDisloc[newState] * averageSurrounding;
      // out4_CA.rgb = colorDisloc[newState];
    }
    // const vec3 colorDisloc[5] = {{0.3,0.3,0.3},     // emptry
    //                              {0,1,0},{0,0,1},   // new/old border
    //                              {1,0,0},{1,1,0}};  // new/old nucleus
  }

  //////////////////////////////////////////////////////
  // PROTOCELLS
  else 
#endif

    if( CAType == CA_PROTOCELLS ) {

    int nbNeighbors =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);
/*
    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeProtocell[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeProtocell[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeProtocell[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeProtocell[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeProtocell[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeProtocell[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeProtocell[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeProtocell[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeProtocell[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeProtocell[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeProtocell[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      // if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      // }
      // else {
      //   averageSurrounding = gatherColorFactors / nbNeighbors;
      // }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }
*/
    vec3 maxSurrounding = (neighborValues[0].rgb);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb;
      if( graylevel(newColorFactor) > graylevel(maxSurrounding) ) {
        maxSurrounding = newColorFactor;
      }
    }
    if( graylevel(currentCA.rgb) > graylevel(maxSurrounding) ) {
      maxSurrounding = currentCA.rgb;
    }

    vec3 gatherColorFactors =
      (neighborValues[0].rgb) +
      (neighborValues[1].rgb) +
      (neighborValues[2].rgb) +
      (neighborValues[3].rgb) +
      (neighborValues[4].rgb) +
      (neighborValues[5].rgb) +
      (neighborValues[6].rgb) +
      (neighborValues[7].rgb);

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      // if( CAcolorSpread ) {
        averageSurrounding = (gatherColorFactors + currentCA.rgb)/8;
      // }
      // else {
      //   averageSurrounding = gatherColorFactors / nbNeighbors;
      // }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    // 0 empty
    // 1 X
    // 2 Y
    // 3 Z
    uint nbStates = 4;
    uint newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      newState = 3;
      out4_CA.rgb = shadeProtocell[newState].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams1: px - X duplication from X+Y (randomCA.x)
      // CAParams2: py - Y duplication from Y+Z (randomCA.y)
      // CAParams3: pz - Z duplication from Z+X (randomCA.z)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(randomCA.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && randomCA.y < CAParams1) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(randomCA.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && randomCA.w < CAParams2) {
              newState = 2; // new Y
            }
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Z and a neighboring X slot
          int firstindex = clamp(int(randomCA2.x * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && randomCA2.y < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(randomCA2.z * 8),0,8);
          newState = 0;
          // looks for an old border around
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a != 0 ) {
              newState = int(neighborValues[ind % 8].a);
            }
          }
        }
      }
      // CAParams5: px - X/Y/Z degradation (randomCA.w)
      // randomCA2.x choice of a first neighbor
      // degradation or migration of X
      else if( randomCA.x < CAParams4 
        || nbNeighbors < 8 * CAParams5 ) { 
        newState = 0; 
      }

      // if( state ==  1 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA.w < CAParams4
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      //    out4_CA.a = float(newState);
      // }
      // // degradation or migration of Y
      // else if( state ==  2 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.x < CAParams5
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      //    out4_CA.a = float(newState);
      // }
      // // degradation or migration of Z
      // else if( state ==  3 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.y < CAParams6
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      // }


    out4_CA.a = float(newState);
    // state change: takes the color of the current stage
    if( currentCA.a != out4_CA.a ) {
      out4_CA.rgb = shadeProtocell[newState].r * currentCA.rgb;
    }
    else {
      //averageSurrounding  maxSurrounding
      maxSurrounding = clamp( maxSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeProtocell[newState] * maxSurrounding;
      // out4_CA.rgb = vec3(shadeProtocell[newState]);
    }
  }
}

#ifndef GENERATIVE_MOVIE
  //////////////////////////////////////////////////////
  // SOCIAL COLLABORATION: PRISONNER'S DILEMNA
  else if( CAType == CA_SOCIAL_PD ) {
    // NW  N  NE
    // S   *  E
    // SW  S  SE
    //
    // 3  2  1
    // 4  *  0
    // 5  6  7
    //
    // in the Moore neighborhood, each neighbor has 3 neighbors
    // N has NE NW & Center
    // NE has N E & Center...
    // the payoff for each neighbor is calculated for these 3 neighbors
    // 3 payoffs: 
    //     . CAParams1 Cooperation
    //     . CAParams2 Defect (with Coop)
    //     . CAParams3 Coop (with Defect)
    //     . CAParams4 double defect
    // 2 states 1: coop - 2: defect
    // first state is state of player, second state is state of partner

    // const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
    //                                  {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    uint nbStates = 3;
    uint newState = 0;

    int nbNeighbors =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);

    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeSocial[int(max(0,neighborValues[0].a))]) +
      (neighborValues[1].rgb * inverseShadeSocial[int(max(0,neighborValues[1].a))]) +
      (neighborValues[2].rgb * inverseShadeSocial[int(max(0,neighborValues[2].a))]) +
      (neighborValues[3].rgb * inverseShadeSocial[int(max(0,neighborValues[3].a))]) +
      (neighborValues[4].rgb * inverseShadeSocial[int(max(0,neighborValues[4].a))]) +
      (neighborValues[5].rgb * inverseShadeSocial[int(max(0,neighborValues[5].a))]) +
      (neighborValues[6].rgb * inverseShadeSocial[int(max(0,neighborValues[6].a))]) +
      (neighborValues[7].rgb * inverseShadeSocial[int(max(0,neighborValues[7].a))]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeSocial[int(max(0,currentCA.a))]) +
      (neighborValues[0].rgb * inverseShadeSocial[int(max(0,neighborValues[0].a))]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeSocial[int(max(0,neighborValues[ind].a))];
      if( graylevel(newColorFactor) > graylevel(maxColorFactors) ) {
        maxColorFactors = newColorFactor;
      }
    }

    vec3 averageSurrounding;
    if( nbNeighbors > 0 ) {
      if( CAcolorSpread ) {
        averageSurrounding = maxColorFactors;
      }
      else {
        averageSurrounding = gatherColorFactors / nbNeighbors;
      }
      // averageSurrounding = gatherSurroundingLives;
      // averageSurrounding = vec3(0.4);
    }
    else {
      averageSurrounding = currentCA.rgb;//vec3(0.0);
    }

    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      // newState = 3;
      out4_CA.rgb = shadeSocial[newState].r * currentCA.rgb;
    }
    else {
      // float payoffs[8];
      float state = clamp(currentCA.a,0,nbStates);
      int indMax = -1;
      float maxPayoff = -1;
      float sumPayoff = 0;
      int firstIndex = int(randomCA.y * 9)%8;
      for( int i = firstIndex ; i < firstIndex + 8 ; i++ ) {
        int curInd = i%8;
        float curPayoff = payoffCalc(neighborValues[curInd].a,state)
                    + payoffCalc(neighborValues[curInd].a,neighborValues[(i+1)%8].a)
                    + payoffCalc(neighborValues[curInd].a,neighborValues[(i+7)%8].a);
        sumPayoff += curPayoff;
        if( curPayoff > maxPayoff ) {
          maxPayoff = curPayoff;
          indMax = curInd;
        }
      }
      newState = int(state);

      // BASIC : adopts behavior of most successful neighbor
      if( CASubType == 1 ) {
        newState = int(neighborValues[indMax].a);
      }
      // VARIANT 2 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // otherwise dies or adopts opposite behavior with a certain probability
      // ****** payoffCalc uses parameters 1 to 4
      else if( CASubType == 2 ) {
        if( sumPayoff > 24 * CAParams5 ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          if( randomCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
            newState = 3 - int(neighborValues[indMax].a);
          }
          else {
            newState = 0;
          }
        }
      }
      // VARIANT 3 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // otherwise if alive dies or stays as it is with a certain probability
      // if dead copies most succesful neighbor with a certain probability
      else if( CASubType == 3 ) {
        if( maxPayoff > 24 * CAParams5 ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          // alive and adopts opposite behavior
          if( neighborValues[indMax].a > 0 ) {
              if( randomCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
                newState = 3 - int(neighborValues[indMax].a);
              }
              // or stays in its current state
          }
          //dead
          else {
              if( randomCA.z > CAParams7 ) {
                newState = int(neighborValues[indMax].a);
              }
              // or stays dead
          }
        }
      }
      // VARIANT 4 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // or the opposite behavior with a given probability
      else if( CASubType == 4 ) {
        if( sumPayoff > 24 * CAParams5 ) {
          if( randomCA.z > CAParams6 ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
                newState = 3 - int(neighborValues[indMax].a);
          }
        }
        else {
          if( randomCA.z > 1 - CAParams7 ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
             newState = 3 - int(neighborValues[indMax].a);
          }
        }
      }

      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeSocial[newState] * averageSurrounding;
    }
  }
#endif

  //////////////////////////////////////////////////////
  // PATHS
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_PATHS ) {

    for( int ind = 0 ; ind < 8 ; ind++ ) {
      neighborValuesHV[ind] = texture(fs_lookupTable2, 
                  decalCoords + neighborOffsetsPaths[ind] );
    }
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      neighborValuesDiag[ind] = texture(fs_lookupTable2, 
                  decalCoords + neighborOffsetsNonPaths[ind] );
      // existsPathsDiag *= neighborValuesDiag[ind].a;
    }

    // const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
    //                                  {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
    int nbSurroundingPathsHV =
      (neighborValuesHV[0].a + neighborValuesHV[1].a > 0? 1:0) +
      (neighborValuesHV[2].a + neighborValuesHV[3].a > 0? 1:0) +
      (neighborValuesHV[4].a + neighborValuesHV[5].a > 0? 1:0) +
      (neighborValuesHV[6].a + neighborValuesHV[7].a > 0? 1:0);
    int existsPathsDiag =
      (neighborValuesDiag[0].a * neighborValuesDiag[1].a *
       neighborValuesDiag[2].a * neighborValuesDiag[3].a *
       neighborValuesDiag[4].a * neighborValuesDiag[5].a *
       neighborValuesDiag[6].a * neighborValuesDiag[7].a *
       neighborValuesDiag[8].a * neighborValuesDiag[9].a *
       neighborValuesDiag[10].a * neighborValuesDiag[11].a *
       neighborValuesDiag[12].a * neighborValuesDiag[13].a *
       neighborValuesDiag[14].a * neighborValuesDiag[15].a > 0? 1:0);

    // 0 empty
    // 1 paths
    uint nbStates = 2;
    float newState = 0;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      if( randomCA.x > 0.5 ) {
        out4_CA.a = 1; 
        newState = 1;
        out4_CA.rgb = vec3(1);
      }
      else {
        out4_CA.a = 0; 
        newState = 0;
        out4_CA.rgb = vec3(0);
      }
    }
    else {
      float state = clamp(currentCA.a,0,1);
      if(existsPathsDiag == 0) {
        // continues a path H or V (1)
        // or joins a path H or V or with a square angle (2)
        if(nbSurroundingPathsHV == 1){ //|| nbSurroundingPathsHV == 2) {
          if( state > 0 ) {
            newState = state - CAdecay; // keeps the same state to take decay into account
          }
          else {
            newState = 1; 
          }
        }
        // 3 -> would make a T
        // 4 -> would make a X
        else {
          newState = state - CAdecay;
        }
      }
      else {
        newState = state - CAdecay; // is empty, otherwise would create a diagonal line
      }
      // newState = state;
      out4_CA.a = newState;
      if( newState > 0 ) {
        out4_CA.rgb = vec3(newState);
      }
      else {
        out4_CA.rgb = vec3(0);
      }
    }
  }

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = clamp(vec4(currentCA.rgb-vec3(CAdecay),currentCA.a),0,1); // ready
  }

}
#endif


void particle_out( vec2 decalCoords ) {
  // fills by cumulating the neighboring particles that
  // can reach the pixel at this frame (come into a circle of radius radiusParticle)
  // with the pixel seen as length unit.
  out_color_particle = vec3(0,0,0);
  int nb_cumultated_particles = 0;

  vec2 usedNeighborOffset;

  // looks in the 4 neighbors (S,N,E,W) whether one of them is 
  // going to reach the current cell
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    usedNeighborOffset = neighborOffsets[ind];
    vec2 newDecalCoord = decalCoords + usedNeighborOffset;
    if( newDecalCoord.x >= 0 && newDecalCoord.y >= 0 
      	&& newDecalCoord.x < width && newDecalCoord.y < height ) {
      vec4 surrParticle_localColor;
      surrParticle_localColor = texture( fs_decal, newDecalCoord );

      // if the particle is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the loclation of the
      // particle in the coordinates system of the current pixel
      // expected to be in [-radiusParticle,+radiusParticle]x[-radiusParticle,+radiusParticle]
      if( graylevel(surrParticle_localColor) >  0 ) {
        vec2 surrParticle_speed;
        vec2 surrParticle_position;
        vec2 surrParticle_nextPosition;

        vec4 surrParticle_speed_position = texture( fs_lookupTable1,
						    newDecalCoord );
        surrParticle_speed = surrParticle_speed_position.xy;
        surrParticle_position = surrParticle_speed_position.zw;
        vec2 double_accelerations;
        vec2 partTexCoordLoc = particleTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        // FLAT
        if(noiseType == 0 )  {
          double_accelerations = vec2(snoise( partTexCoordLoc , noiseScale * 100 ),
                                  snoise( partTexCoordLoc + vec2(2.0937,9.4872) , noiseScale * 100 ));
        }
        // SUN RAYS
        else if(noiseType == 1 ) {
          vec2 pos = vec2( atan((noiseCenter_0-partTexCoordLoc.x)/(noiseCenter_1-partTexCoordLoc.y)) * (noiseAngleScale * 10),
                           length(vec2(noiseCenter_0,noiseCenter_1) - partTexCoordLoc) / (noiseLineScale) );
          double_accelerations = vec2(snoise( pos , noiseScale * 10 ) ,
                                  snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
        }
        // CAMERA
        else if(noiseType == 2 ) {
          double_accelerations = texture(fs_lookupTable7, decalCoords + usedNeighborOffset ).rg;
        }
        // MOVIE
        else {
          double_accelerations 
          = texture(fs_lookupTable8, movieCoord + usedNeighborOffset/ movieWH ).rg;
        }

      	vec2 acceleration;
      	acceleration = double_accelerations - partAccCenter;
      	if( acceleration_factor > 0 ) {
      	  // acceleration
      	  surrParticle_speed += acceleration_factor * acceleration;
      	}
      	else {
      	  // damping
      	  surrParticle_speed += acceleration_factor * surrParticle_speed;
      	}
      	surrParticle_nextPosition 
      			     = usedNeighborOffset + surrParticle_position + surrParticle_speed; 
      	// the current step is added to the position

        if( abs(surrParticle_nextPosition.x) <= 0.5 
                  && abs(surrParticle_nextPosition.y) <= 0.5 ) {
          out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += surrParticle_speed;
          // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
        // radius particle extension for (S,N,E,W) neighbors
        else if( abs(surrParticle_nextPosition.x) <= (radiusParticle - randomCA.z)
                  && abs(surrParticle_nextPosition.y) <= (radiusParticle - randomCA.w) ) {
          float dist = radiusParticle - length(surrParticle_nextPosition);
          out_color_particle += surrParticle_localColor.rgb;
          // adds high frequency random speed to make them leave the initial particle
          out_speed_particle += dist * (randomCA.xy - vec2(0.5,0.5));

          // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
      }
    }
  }

  // looks in the 4 neighbors (SE,NW,NE,SW) whether one of them is 
  // going to reach the current cell
  for( int ind = 4 ; ind < 8 ; ind++ ) {
    usedNeighborOffset = neighborOffsets[ind];
    vec2 newDecalCoord = decalCoords + usedNeighborOffset;
    if( newDecalCoord.x >= 0 && newDecalCoord.y >= 0 
	     && newDecalCoord.x < width && newDecalCoord.y < height ) {

      vec4 surrParticle_localColor;
      surrParticle_localColor = texture( fs_decal,
					 newDecalCoord );

      // if the particle is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the location of the
      // particle in the coordinates system of the current pixel
      // expected to be in [-radiusParticle,+radiusParticle]x[-radiusParticle,+radiusParticle]
      if( graylevel(surrParticle_localColor) >  0 ) {
        vec2 surrParticle_speed;
        vec2 surrParticle_position;
        vec2 surrParticle_nextPosition;

        vec4 surrParticle_speed_position
      	  = texture( fs_lookupTable1,
      		     newDecalCoord );
              surrParticle_speed = surrParticle_speed_position.xy;
              surrParticle_position = surrParticle_speed_position.zw;

        vec2 double_accelerations;
        vec2 partTexCoordLoc = particleTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        // FLAT
        if(noiseType == 0 )  {
          double_accelerations = vec2(snoise( partTexCoordLoc , noiseScale * 100 ),
                                  snoise( partTexCoordLoc + vec2(2.0937,9.4872) , noiseScale * 100 ));
        }
        // SUN RAYS
        else if(noiseType == 1 ) {
          vec2 pos = vec2( atan((noiseCenter_0-partTexCoordLoc.x)/(noiseCenter_1-partTexCoordLoc.y)) * (noiseAngleScale * 10),
                           length(vec2(noiseCenter_0,noiseCenter_1) - partTexCoordLoc) / (noiseLineScale) );
          double_accelerations = vec2(snoise( pos , noiseScale * 10 ) ,
                                  snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
        }
        // CAMERA
        else if(noiseType == 2 ) {
          double_accelerations = texture(fs_lookupTable7, decalCoords + usedNeighborOffset ).rg;
        }
        // MOVIE
        else {
          double_accelerations 
          = texture(fs_lookupTable8, movieCoord + usedNeighborOffset/ movieWH ).rg;
        }


      	vec2 acceleration;
      	acceleration = double_accelerations - partAccCenter;
      	if( acceleration_factor > 0 ) {
      	  // acceleration
      	  surrParticle_speed += acceleration_factor * acceleration;
      	}
      	else {
      	  // damping
      	  surrParticle_speed += acceleration_factor * surrParticle_speed;
      	}
      	surrParticle_nextPosition 
      			     = usedNeighborOffset + surrParticle_position + surrParticle_speed; 
      	// the current step is added to the position

      	if( abs(surrParticle_nextPosition.x) <= (radiusParticle - randomCA.z)
                  && abs(surrParticle_nextPosition.y) <= (radiusParticle - randomCA.w) ) {
      	  out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += surrParticle_speed;
      	  // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
      }
    }
  }

  out_position_particle 
    = normalize( out_position_particle + vec2( 0.5, 0.5 ) ) 
    - vec2( 0.5, 0.5 );

  if( graylevel(out_color_particle) > 0 ) {
    out_color_particle /= nb_cumultated_particles;
    out_color_particle = clamp( out_color_particle , 0.0 , 1.0 );
  }
  else {
    out_color_particle = vec3(0,0,0);
  }
}


float stroke_out( float current_Brush_RadiusX , float current_Brush_RadiusY , int current_brushID ) {

  // drawing mode: writes under the cursor
  // tries to avoid artefacts at the junction between two segments
  // vector from the pen center to the current position
  vec2 vector_to_mousePosition = vec2(signed_rx,signed_ry);
  vec2 normalized_vector_to_mousePosition 
    = vector_to_mousePosition/vec2(current_Brush_RadiusX,current_Brush_RadiusY);

  // it is blended with preceding pen color at this position
  // gray level 
  // horizontal texture coordinate shift according to pen number
  float penOutGrayLevel = 0.0;
  if( current_brushID >= 0 ) {
    penOutGrayLevel
      = texture(fs_lookupTable6,
		vec3( normalized_vector_to_mousePosition.x/2.0+0.5,
		      0.5 - normalized_vector_to_mousePosition.y/2.0,
          (current_brushID / 3) * 0.25 + 0.125))[current_brushID % 3];
  }

  // pen gray level
  return penOutGrayLevel;
}

float out_gray_drawing( float current_mouse_x , float current_mouse_y , 
			float prev_mouse_x , float prev_mouse_y ,
			float current_Brush_RadiusX , float current_Brush_RadiusY , 
			int current_brushID ,
			int p_track_drawingStroke ) {

  float interp_mouse_x = current_mouse_x;
  float interp_mouse_y = current_mouse_y;

  // gets the previous mouse position prev_mouse_x prev_mouse_y 
  // and the current one current_mouse_x icurrent_mouse_y 

  /////////////////////////////////////////////////
  // line based rendering
  if( p_track_drawingStroke == DRAWING_LINE ) {
    if( prev_mouse_x > 0 && prev_mouse_y > 0 ) {
      // angle of line wrt Ox is between -45deg and +45deg: drawing
      // is controlled by x and interpolated y is a function of x
      if( abs(current_mouse_x - prev_mouse_x) > 
          abs(current_mouse_y - prev_mouse_y) ) {
        if( ( prev_mouse_x < current_mouse_x 
	      && decalCoords.x > prev_mouse_x 
	      && decalCoords.x <= current_mouse_x ) 
	    || ( prev_mouse_x > current_mouse_x 
		 && decalCoords.x >= current_mouse_x 
		 && decalCoords.x < prev_mouse_x ) ) {
          interp_mouse_x = decalCoords.x;
          interp_mouse_y 
            = Line( prev_mouse_x , prev_mouse_y , 
		    current_mouse_x , current_mouse_y , 
		    interp_mouse_x );
        } 
      } 
      // angle of line wrt Ox is under -45deg or above +45deg: drawing
      // is controlled by y and interpolated x is a function of y
      else {
        if( ( prev_mouse_y < current_mouse_y 
	      && (height - decalCoords.y) > prev_mouse_y 
	      && (height - decalCoords.y) <= current_mouse_y ) 
	    || ( prev_mouse_y > current_mouse_y  
		 && (height - decalCoords.y) >= current_mouse_y 
		 && (height - decalCoords.y) < prev_mouse_y ) ) {
          interp_mouse_y = (height - decalCoords.y);
          interp_mouse_x
            = Line( prev_mouse_y , prev_mouse_x , 
		    current_mouse_y , current_mouse_x , 
		    interp_mouse_y );
        } 
      }
    }
  }

  /////////////////////////////////////////////////
  // point or line stroke
  float return_out_gray_drawing = 0.0;
  float unsigned_rx = 10000.0;
  float unsigned_ry = 10000.0;
  if( p_track_drawingStroke == DRAWING_POINT 
      || p_track_drawingStroke == DRAWING_LINE ) {

    //////////////////////////////////////
    // GRAY LEVEL OUTPUT
    //////////////////////////////////////

    // in case of point drawing only points are drawn when uniform_Drawing_fs_3fv_flashTrack0Coefs is not null
    // otherwise the pen is considered out of the screen
    if( p_track_drawingStroke == DRAWING_POINT 
	      && !(currentDrawingTrack > 0 
             && uniform_Drawing_fs_3fv_flashTrack0Coefs[currentDrawingTrack - 1] > 0) ) {
      interp_mouse_x = 10000;
      interp_mouse_y = 10000;
    }

    /////////////////////////////////////////////////
    // WRITING POSITION
    // distance from pen center
    signed_rx = decalCoords.x - interp_mouse_x;
    signed_ry = height - decalCoords.y - interp_mouse_y;
    unsigned_rx = abs( signed_rx );
    unsigned_ry = abs( signed_ry );

    if( current_mouse_x >= 0
	&& current_mouse_y >= 0
	&& (prev_mouse_x != current_mouse_x
	    || prev_mouse_y != current_mouse_y)
    	// doesnt redraw on previously drawn place
	&& unsigned_rx < current_Brush_RadiusX 
	&& unsigned_ry < current_Brush_RadiusY ) {
      // out_rendered_fluidDynamics_FBO.a = 1;
      return_out_gray_drawing 
        = stroke_out( current_Brush_RadiusX , current_Brush_RadiusY , current_brushID );
      newly_drawn = true;
    }
  }
  return return_out_gray_drawing;
}

// Main shader.

void main() {
#include_initializations

// CA Paramaters
// 0.598 0.362 0.535 0.835 0.512 0.488 0.441 0.724 0.425 0.528 0.48  0.693 0.504 0.488 0.339 0.638 0.276 0.874 0.606 0.228 0.276 0.874 0.606 0.228 0.339 0.709 0.276 0.874 0.606 0.228 0.291 0.283 0.842 0.276 0.874 0.606 0.228 0.165 0.347 0.394


  /////////////////////////////////////
  // PARTICLE  FBOOUTPUT
  vec4 out_speed_position_FBO;      // 

#ifdef PG_WITH_CA
  /////////////////////////////////////
  // CA FBO OUTPUT
  vec4 out_CA_FBO;
#endif

  /////////////////////////////////////
  // TRACK COLOR FBO OUTPUT
   vec4 out_track_FBO[PG_NB_TRACKS];

  //////////////////////////
  // variables 

  vec3 pulse = uniform_Drawing_fs_4fv_pulse.rgb;
  float average_pulse = uniform_Drawing_fs_4fv_pulse.a;

  // frame number
  frameNo = int(round(uniform_Drawing_fs_3fv_CAdecay_frameno_Cursor.y));

  // currentDrawingTrack: track number: 0 ==  bg and 1-2 == 2 playing tracks
  // track on which tablet drawing takes place

#ifdef PG_WITH_CA
  // decay of drawing and CA layers
  CAdecay = uniform_Drawing_fs_3fv_CAdecay_frameno_Cursor.x  * (1.0 - CAdecay_pulse * average_pulse);
  
  // cellular automata
  CA_on_off = (CASubType > 0);
#endif

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);
  // drawingStroke = DRAWING_LINE;

  // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Drawing_fs_3fv_CAdecay_frameno_Cursor.z;

  // particles
  acceleration_factor = uniform_Drawing_fs_4fv_partAcc_clearAllLayers_clearCA_radiusPart.x;
  partAccCenter = vec2(partAccCenter_0,partAccCenter_1);
  radiusParticle = uniform_Drawing_fs_4fv_partAcc_clearAllLayers_clearCA_radiusPart.w;

  // working variables for screen dimension
  width = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.x;
  height = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.y;

  // movie size
  movieWH = uniform_Drawing_fs_4fv_movieWH_flashCameCoef_cpTrack.xy;

  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // particle texture + z offset according to the chosen texture
  vec2 position = vec2( 1.0 + sin(frameNo/50000.0),
                        1.0 + cos(frameNo/37000.0));
  vec2 noisePositionOffset = vec2(snoise( position , noiseScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  particleTextureCoordinatesXY = decalCoordsPOT + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( fs_lookupTable9 , vec3( vec2(1,1) - particleTextureCoordinatesXY , 0.0 ) );
  randomCA2 = texture( fs_lookupTable9 , vec3( vec2(1,1) - particleTextureCoordinatesXY , 0.5 ) ).rgb;

/////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
#ifdef PG_WITH_CA
  out_CA_FBO = vec4( 0, 0, 0, 0 );
#endif
  out_track_FBO[0] = vec4(0);
  for(int ind = 1 ; ind < PG_NB_TRACKS ; ind++){
    out_track_FBO[1] = vec4( 0, 0, 0, 1 );
  }

  if(frameNo <= 10 || uniform_Drawing_fs_4fv_partAcc_clearAllLayers_clearCA_radiusPart.y > 0) {
    outColor0 = vec4(0,0,0,1);
    outColor1 = vec4(0,0,0,1);
#ifdef PG_WITH_CA
    outColor2 = vec4(0,0,0,0);
#if PG_NB_TRACKS >= 2
    outColor3 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 3
    outColor4 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 4
    outColor5 = vec4(0,0,0,1);
#endif
#else
#if PG_NB_TRACKS >= 2
    outColor2 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 3
    outColor3 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 4
    outColor4 = vec4(0,0,0,1);
#endif
#endif
    return;
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: COLOR OUTPUT AND GRAY LEVEL CALCULATION
  //////////////////////////////////////
  //////////////////////////////////////

  //////////////////////////////////////
  // COPY OF FBO VALUES AT THE PRECEDING FRAME
  // starts by copying previous output

  // possible horizontal or vertical translation of background texture
  vec2 decalCoordsPrevStep = decalCoords;
    // + vec2( uniform_Drawing_fs_3iv_mouseTracks_x_transl[0] ,
	   //  uniform_Drawing_fs_3iv_mouseTracks_y_transl[0] );
  // copy of previous step output
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[0] = texture( fs_decal , decalCoordsPrevStep );
  }
  else {
    out_track_FBO[0] = vec4( 0, 0, 0, 0 );
  }
  out_speed_position_FBO
    = texture( fs_lookupTable1 , decalCoords );

#ifdef PG_WITH_CA
  out_CA_FBO
    = texture( fs_lookupTable2 , decalCoords );
#endif

  // possible horizontal or vertical translation of track1 texture
  decalCoordsPrevStep = decalCoords;
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
#if PG_NB_TRACKS >= 2
    out_track_FBO[1] = texture( fs_lookupTable3 , decalCoordsPrevStep );
#endif
#if PG_NB_TRACKS >= 3
    out_track_FBO[2] = texture( fs_lookupTable4 , decalCoordsPrevStep );
#endif
#if PG_NB_TRACKS >= 4
    out_track_FBO[3] = texture( fs_lookupTable5 , decalCoordsPrevStep );
#endif
  }
  else {
#if PG_NB_TRACKS >= 2
    out_track_FBO[1] = vec4( 0, 0, 0, 1 );
#endif
#if PG_NB_TRACKS >= 3
    out_track_FBO[2] = vec4( 0, 0, 0, 1 );
#endif
#if PG_NB_TRACKS >= 4
    out_track_FBO[3] = vec4( 0, 0, 0, 1 );
#endif
  }

  if( freeze ) {
        // FBO OUTPUTS
    outColor0 = out_track_FBO[0];
    // outColor0 = 0.01 * outColor0 + 0.99 * vec4(videocolor,1);
    outColor1 = out_speed_position_FBO;
#ifdef PG_WITH_CA
    outColor2 = out_CA_FBO;
#if PG_NB_TRACKS >= 2
    outColor3 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
    outColor4 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
    outColor5 = out_track_FBO[3];
#endif
#else
#if PG_NB_TRACKS >= 2
    outColor2 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
    outColor3 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
    outColor4 = out_track_FBO[3];
#endif
#endif
      return;
  }

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PARTICLE: increase radius for particle capture
  //////////////////////////////////////
  //////////////////////////////////////
  vec3 trackColorCopy = vec3(0.0);
#ifdef PG_WITH_CA
  vec4 trackCACopy = vec4(0.0);
#endif
 
  /////////////////////////////////////////////////
  // void drawing, copies the previous pixel, just manages decay
  // at the end of this code
  // nothing needs to be done here
  // if( drawingStroke == DRAWING_VOID ) {
  // }

  //////////////////////////////////////
  //////////////////////////////////////
  // VIDEO PROCESSING
  //////////////////////////////////////
  //////////////////////////////////////
  // manages the gray level of each track
  vec4 track_grayLevel = vec4(0);
  // each track possibly covers the previous color

  vec3 photocolor = vec3( 0.0 );
  if(uniform_Drawing_fs_2fv_photo01_weights.x > 0) {
    photocolor += uniform_Drawing_fs_2fv_photo01_weights.x * texture(fs_lookupTable10, 
      vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Drawing_fs_4fv_photo01_wh.xy ).rgb;
  }
  if(uniform_Drawing_fs_2fv_photo01_weights.y > 0) {
    photocolor += uniform_Drawing_fs_2fv_photo01_weights.y * texture(fs_lookupTable11,  
      vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Drawing_fs_4fv_photo01_wh.zw ).rgb;
  }

#ifdef PG_VIDEO_ACTIVE
  vec3 videocolor = vec3( 0.0 );

  float flashCameraCoef = uniform_Drawing_fs_4fv_movieWH_flashCameCoef_cpTrack.z;
  // no flash Camera for the moment
  // flashCameraCoef = .0f;
  
  // VIDEO FRAME CAPTURE AND RENDERING
  // video texture used for drawing
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  cameraImage = texture(fs_lookupTable7, decalCoords ).rgb;
  movieImage = texture(fs_lookupTable8, movieCoord ).rgb;

  // Sobel on camera
  if( cameraSobel > 0 ) {
      vec3 sampler;
      // sobel
      vec3 sobelX = vec3(0.0);
      vec3 sobelY = vec3(0.0);

      // samples the center pixel and its Moore neighborhood
      for( int i = 0 ; i < 4 ; i++ ) {
          sampler = texture(fs_lookupTable7 , decalCoords + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * sampler;
          sobelY += sobelMatrixY[i] * sampler;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          sampler = texture(fs_lookupTable7 , decalCoords + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * sampler;
          sobelY += sobelMatrixY[i] * sampler;
      }

      sampler = cameraImage;
      sobelX = mix( sampler , sobelX , cameraSobel );
      sobelY = mix( sampler , sobelY , cameraSobel );

      cameraImage = clamp( sqrt( sobelX * sobelX + sobelY * sobelY ) , 0.0 , 1.0 );
  }
  // Sobel on movie
  if( movieSobel > 0 ) {
      vec3 sampler;
      // sobel
      vec3 sobelX = vec3(0.0);
      vec3 sobelY = vec3(0.0);

      // samples the center pixel and its Moore neighborhood
      for( int i = 0 ; i < 4 ; i++ ) {
          sampler = texture(fs_lookupTable8 , movieCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * sampler;
          sobelY += sobelMatrixY[i] * sampler;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          sampler = texture(fs_lookupTable8 , movieCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * sampler;
          sobelY += sobelMatrixY[i] * sampler;
      }

      sampler = movieImage;
      sobelX = mix( sampler , sobelX , movieSobel );
      sobelY = mix( sampler , sobelY , movieSobel );

      movieImage = clamp( sqrt( sobelX * sobelX + sobelY * sobelY ) , 0.0 , 1.0 );
  }
  // color inversion
  if( invertMovie ) {
      movieImage = vec3(1) - movieImage;
  }

  // video image = mix of movie and camera
  videocolor = cameraWeight * cameraImage 
              + movieWeight * movieImage;

  // image_satur image_satur_pulse
  vec3 pulsed_satur = vec3(1);
  if(video_satur_pulse > 0) {
    pulsed_satur = video_satur_pulse * pulse;
  }
  //  public-domain function by Darel Rex Finley
  float  powerColor = sqrt( (videocolor.r)*(videocolor.r) * .299 +
                             (videocolor.g)*(videocolor.g) * .587 +
                             (videocolor.b)*(videocolor.b) * .114 ) ;
  videocolor.r = clamp( powerColor 
    + (videocolor.r - powerColor) * pulsed_satur.r , 0 , 1 );
  videocolor.g = clamp( powerColor 
    + (videocolor.g - powerColor) * pulsed_satur.g , 0 , 1 );
  videocolor.b = clamp( powerColor 
    + (videocolor.b - powerColor) * pulsed_satur.b , 0 , 1 );
#endif

  // FLASH VIDEO: NON VIDEO TRACKS ARE DARKER WHEN THERE
  // IS A FLASH VIDEO (EXCEPT CA TRACK)

  //////////////////////////////////////
  //////////////////////////////////////
  // TRACK COLOR PROCESSING
  //////////////////////////////////////
  //////////////////////////////////////

  // TRACK indTrack between 1 and PG_NB_TRACKS - 1
  bvec4 path_replay = bvec4(path_replay_0,path_replay_1,path_replay_2,path_replay_3);
  for( int indTrack = 1 ; indTrack <= PG_NB_TRACKS ; indTrack++ ) {
    // track 0 is processed last
    int indCurTrack = indTrack % PG_NB_TRACKS;

    /////////////////
    // TRACK color
    int track_drawingStroke = DRAWING_VOID;
    if( path_replay[indCurTrack] ) {
      track_drawingStroke = DRAWING_LINE;
    }
    else if( currentDrawingTrack == indCurTrack ) {
      track_drawingStroke = drawingStroke;
    }
    if( track_drawingStroke > 0 ) {
      track_grayLevel[indCurTrack] = out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[indCurTrack] , 
  					uniform_Drawing_fs_4fv_mouseTracks_y[indCurTrack] , 
  					uniform_Drawing_fs_4fv_mouseTracks_x_prev[indCurTrack] , 
  					uniform_Drawing_fs_4fv_mouseTracks_y_prev[indCurTrack] ,
  					uniform_Drawing_fs_4fv_mouseTracks_RadiusX[indCurTrack] ,
  					uniform_Drawing_fs_4fv_mouseTracks_RadiusY[indCurTrack] ,
  					int(uniform_Drawing_fs_4fv_mouseTracks_BrushID[indCurTrack]) ,
  					track_drawingStroke ) 
            * uniform_Drawing_fs_4fv_mouseTracks_a[indCurTrack]; // brush opacity is combined with color opacity

      // drawing occurs on track indCurTrack
      if( track_grayLevel[indCurTrack] > 0 ) {
        // normal stylus
        if( Cursor  > 0 ) {  // -1 for rubber and +1 for Stylus
          out_track_FBO[indCurTrack].rgb 
            = out_track_FBO[indCurTrack].rgb * clamp((1 - track_grayLevel[indCurTrack]),0,1)
            	+ track_grayLevel[indCurTrack]
              	* vec3( uniform_Drawing_fs_4fv_mouseTracks_r[indCurTrack] , 
              		      uniform_Drawing_fs_4fv_mouseTracks_g[indCurTrack] , 
              		      uniform_Drawing_fs_4fv_mouseTracks_b[indCurTrack] );
        }
        // rubber stylus
        else {
          out_track_FBO[indCurTrack].rgb *= (1 - track_grayLevel[indCurTrack]);
        }
        if(indCurTrack != 0) {
          out_track_FBO[indCurTrack].a = 1.0;//track_grayLevel[indCurTrack];
        }

        out_track_FBO[indCurTrack] = clamp( out_track_FBO[indCurTrack] , 0.0 , 1.0 );
        // out_track_FBO[1].rgb = 0.001 * out_track_FBO[1].rgb + .999 * vec3(1,0,0);
      }
    }

    /////////////////
    // TRACK photo
    if(currentPhotoTrack == indCurTrack 
      && uniform_Drawing_fs_2fv_photo01_weights.x + uniform_Drawing_fs_2fv_photo01_weights.y > 0 ) {
       out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
    }

    /////////////////
    // TRACK video
#ifdef PG_VIDEO_ACTIVE
    if( currentVideoTrack == indCurTrack ) {
      if( cameraCumul == 1 ) { // ADD
        out_track_FBO[indCurTrack] 
          = vec4( clamp( max(videocolor,out_track_FBO[indCurTrack].rgb) , 0.0 , 1.0 ) ,  1.0 );
      }
      else if( cameraCumul == 2) {
        if(cameraWeight + movieWeight > 0) {
          float grvid = graylevel(videocolor);
          // float grbg = graylevel(out_track_FBO[indCurTrack]);
          // if( grvid > grbg) { // STAMP
          out_track_FBO[indCurTrack] 
            = clamp(vec4( mix( videocolor , out_track_FBO[indCurTrack].rgb * .99 , 1-grvid),  1.0 ),0,1);
          // }
          // else { // STAMP
          //   out_track_FBO[indCurTrack] = vec4( mix(out_track_FBO[indCurTrack].rgb,videocolor,grvid) ,  1.0 );
          // }
        }
      }
      else if( cameraCumul == 3 ) { // XOR
        float gvid = graylevel(videocolor);
        float gtrack = graylevel(out_track_FBO[1].rgb);
        if( gvid > 0 && gtrack == 0)  {
          out_track_FBO[indCurTrack] = vec4( videocolor ,  1.0 );
        }
        else if( gvid > 0 && gtrack > 0)  {
          out_track_FBO[indCurTrack] = vec4( vec3(0) ,  1.0 );
        }
      }
      else { // NORMAL
        out_track_FBO[indCurTrack] 
          = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
      }
      if( flashCameraCoef > 0 ) { // flash video
          // video image copy when there is a flash video
        out_track_FBO[indCurTrack].rgb = flashCameraCoef * cameraImage;
      }
    }
    else {
      // flashCameraCoef = 0.0 <=> no flash video
      out_track_FBO[indCurTrack].rgb *= (1.0 - flashCameraCoef);
    }
#endif

    // flash on track 0 (only concerns tracks >= 1)
    if( indCurTrack != 0 ) {
      trackColorCopy += uniform_Drawing_fs_3fv_flashTrack0Coefs[indCurTrack - 1] 
                        * out_track_FBO[indCurTrack].rgb;
    }

#ifdef PG_WITH_CA
    // flash on CA track
    if( graylevel(out_track_FBO[indCurTrack].rgb) > .2 
        && graylevel(out_track_FBO[indCurTrack].rgb) < 2.1
        && flashCameraCoef == 0 ) { // no flash video
      trackCACopy += uniform_Drawing_fs_4fv_flashCACoefs[indCurTrack] 
                  * vec4(out_track_FBO[indCurTrack].rgb,graylevel(out_track_FBO[indCurTrack].rgb));
    }
#endif
  }


  //////////////////////////////////////
  //////////////////////////////////////
  // CA LAYER: CA "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
#ifdef PG_WITH_CA  
  if( CA_on_off && (frameNo % CAstep == 0) ) {
    out4_CA = vec4(0.0);
    vec4 currentCA = out_CA_FBO; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // will disappear through the updating rule of the automaton)
    if( trackCACopy.a > 0 ) { // gray level of copy color (in case of flash)
      currentCA.a = -1;
      // currentCA.rgb = clamp( currentCA.rgb + out_CA_FBO.rgb , 0.0 , 1.0 );
      currentCA.rgb = clamp( trackCACopy.rgb , 0.0 , 1.0 );
      // currentCA.rgb = out_track_FBO[1].rgb;
      // currentCA.rgb = vec3(1,0,0);
    }

    if( CAType == CA_COLOR ) {
      vec4 composedColor = out_track_FBO[0]
#if PG_NB_TRACKS >= 2
        + out_track_FBO[1]
#endif
#if PG_NB_TRACKS >= 3
        + out_track_FBO[2]
#endif
#if PG_NB_TRACKS >= 4
        + out_track_FBO[3]
#endif
        ;
      composedColor = clamp( composedColor , 0.0 , 1.0 );
      float hue = atan(sqrt(3)*(composedColor.g - composedColor.b),
                        2 * composedColor.r - composedColor.g - composedColor.b) / (2 * PI);
      hue = (hue > 0 ? hue : hue + 1);
      CAType = int(floor(4 * hue));
    }

     // calculates the new state of the automaton
    CA_out( decalCoords , currentCA );


    // CA "REPOPULATION"
    if( repopulatingCA > 0 ) {
      // if the particle noise is equal to frame % 8500 the cell is repopulated with a particle
      vec4 randomValue = texture( fs_lookupTable9 , vec3( decalCoordsPOT , 0.0 ) );
      if( int(frameNo * randomValue.w ) % int(765 *(1-repopulatingCA)) == int((randomValue.x * 255.0
          + randomValue.y * 255.0 + randomValue.z * 255.0 ) *(1-repopulatingCA)) ) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Drawing_fs_4fv_flashBackCoef_repop_Color.yzw;
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_CA_FBO = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Drawing_fs_4fv_flashBackCoef_repop_Color.x == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Drawing_fs_4fv_flashBackCoef_repop_Color.x * out_CA_FBO.rgb , 
            0 , 1 );
#endif

  //////////////////////////////////////
  //////////////////////////////////////
  // BACKGROUND LAYER: PARTICLE "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( particleMode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH PARTICLE: increase radius for particle capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Drawing_fs_4fv_isClearLayer_flashPart.y > 0 ) {
      radiusParticle = 1;
    }

    particle_out( decalCoords );

    // arrival of a new particle
    if( graylevel(out_color_particle) > 0 ) {
      // outputs the color from particle move at drawing layer
      out_track_FBO[0].rgb = out_color_particle;
      out_speed_position_FBO 
	     = vec4( out_speed_particle , out_position_particle );
    }
    // update of the current particle
    else {
      //  modifies speed according to acceleration
      vec2 double_accelerations;
      // FLAT
      if(noiseType == 0 )  {
        double_accelerations = vec2(snoise( particleTextureCoordinatesXY , noiseScale * 100 ),
                                snoise( particleTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));
      }
      // SUN RAYS
      else if(noiseType == 1 ) {
        vec2 pos = vec2( atan((noiseCenter_0-particleTextureCoordinatesXY.x)/(noiseCenter_1-particleTextureCoordinatesXY.y)) * (noiseAngleScale * 10),
                         length(vec2(noiseCenter_0,noiseCenter_1) - particleTextureCoordinatesXY) / (noiseLineScale) );
        double_accelerations = vec2(snoise( pos , noiseScale * 10 ) ,
                                snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
      }
      // CAMERA
      else if(noiseType == 2 ) {
        double_accelerations = cameraImage.rg;
      }
      // MOVIE
      else {
        double_accelerations = movieImage.rg;
      }

      vec2 acceleration;
      acceleration = double_accelerations - partAccCenter;
      if( acceleration_factor > 0 ) {
      	// acceleration
      	out_speed_position_FBO.xy += acceleration_factor * acceleration;
      }
      else {
      	// damping
      	out_speed_position_FBO.xy += acceleration_factor * out_speed_position_FBO.xy;
      }
      // updates the position of the current particle
      out_speed_position_FBO.zw += out_speed_position_FBO.xy; 

      // if the particle leaves the cell, it is erased
      if( abs( out_speed_position_FBO.z ) > 0.5
      	  || abs( out_speed_position_FBO.w ) > 0.5 ) {
      	if( particleMode == PARTICLE_UNIQUE ) {
      	  out_track_FBO[0].rgb = vec3(0);
      	}
      }
    }
  }

  // PARTICLE "ADDITION"
  if( repopulatingBG > 0 ) {
    // if the particle noise is equal to frame % 8500 the cell is repopulated with a particle
    vec4 randomValue = texture( fs_lookupTable9 , vec3( decalCoordsPOT , 0.5 ) );
    if( int(frameNo * randomValue.w ) % int(765 *(1-repopulatingBG)) == int((randomValue.x * 255.0
        + randomValue.y * 255.0 + randomValue.z * 255.0 ) *(1-repopulatingBG)) ) {
        out_track_FBO[0].rgb = uniform_Drawing_fs_4fv_flashBackCoef_repop_Color.yzw;
    }
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRWING AND CA LAYERS: GRAY LEVEL DECAY
  //////////////////////////////////////
  //////////////////////////////////////

  //////////////////////////////////////////////
  // track 0 flashing of tracks >= 1
  out_track_FBO[0].rgb = clamp(out_track_FBO[0].rgb + trackColorCopy, 0 , 1);

  //////////////////////////////////////////////
  // track decay and clear layer
  for(int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++) {
      if( graylevel(out_track_FBO[indTrack].rgb) > 0 ) {
        out_track_FBO[indTrack].rgb 
             = out_track_FBO[indTrack].rgb - vec3(uniform_Drawing_fs_4fv_trackdecay[indTrack]);
      }
      out_track_FBO[indTrack].rgb 
        = clamp( out_track_FBO[indTrack].rgb , 0.0 , 1.0 );
      // clear layer
      if( currentDrawingTrack == indTrack 
          && uniform_Drawing_fs_4fv_isClearLayer_flashPart.x > 0 ) {
        out_track_FBO[indTrack].rgb = vec3(0);
      }
  }

  //////////////////////////////////////////////
  // resets particle speed and position if trackO returns to black
  if( graylevel(out_track_FBO[0].rgb) <= 0 ) {
    out_track_FBO[0].rgb = vec3(0);
    out_speed_position_FBO = vec4(0);
  }

  //////////////////////////////////////////////
  // copy tracks
  if( uniform_Drawing_fs_4fv_movieWH_flashCameCoef_cpTrack.w != 0 ) {
    int targetTrack;
    if( uniform_Drawing_fs_4fv_movieWH_flashCameCoef_cpTrack.w > 0) {
      targetTrack = (currentDrawingTrack + 1) % PG_NB_TRACKS;
    }
    else {
      targetTrack = (currentDrawingTrack + PG_NB_TRACKS - 1) % PG_NB_TRACKS;
    }

    out_track_FBO[targetTrack].rgb += out_track_FBO[currentDrawingTrack].rgb;
  }

#ifdef PG_WITH_CA
  //////////////////////////////////////////////
  // CA LAYER CLEAR
  if( uniform_Drawing_fs_4fv_partAcc_clearAllLayers_clearCA_radiusPart.z > 0 ) {
      out_CA_FBO = vec4(0);
  }
#endif

  //////////////////////////////////////////////
  // FBO OUTPUTS 
  outColor0 = out_track_FBO[0];
  // outColor0 = vec4(out_track_FBO[0].rgb* 0.01 + noiseParticles * 0.99,0,1);
  outColor1 = out_speed_position_FBO;
#ifdef PG_WITH_CA
  outColor2 = out_CA_FBO;
#if PG_NB_TRACKS >= 2
  outColor3 = out_track_FBO[1];// + vec4(0,0.1,0,0);
#endif
#if PG_NB_TRACKS >= 3
  outColor4 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  outColor5 = out_track_FBO[3];
#endif
#else
#if PG_NB_TRACKS >= 2
  outColor2 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
  outColor3 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  outColor4 = out_track_FBO[3];
#endif
#endif
}
