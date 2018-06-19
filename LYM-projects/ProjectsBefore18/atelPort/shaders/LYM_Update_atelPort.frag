/***********************************************************************
File: song/shaders/LYM_Update_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#include_declarations

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;

#define PG_VIDEO_ACTIVE
// #define CURVE_PARTICLES
#define SPLAT_PARTICLES

// CODE SPECIFIC FOR ATELIERS PORTATIFS
#define ATELIERS_PORTATIFS

// #define GENERATIVE_MOVIE

////////////////////////////////////////////////////////////////////
// TRACK CONST
#define PG_NB_TRACKS 3
#define PG_NB_PATHS 7

///////////////////////////////////////////////////////////////////
// brush side divider for ateliers portatifs
int brushSizeDivider = 1;

///////////////////////////////////////////////////////////////////
// PARTICLE IMAGE INITIALIZATION
#define PG_NB_PARTICLE_INITIAL_IMAGES 6

const uint pg_CA_FBO_attcht = 0;
const uint pg_Pixel_FBO_attcht = 1;
const uint pg_ParticlePosSpeed_FBO_attcht = 2;
const uint pg_ParticleColRadius_FBO_attcht = 3;
const uint pg_ParticleTargetPosColRadius_FBO_attcht = 4;

////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA
// CA types
const uint CA_CYCLIC = 0;
const uint CA_CYCLIC_1 = 1;
const uint GOL_1 = 2;
const uint CA_PROTOCELLS = 3;
// const uint CA_PATHS = 4;
// const uint CA_COLOR = 5;
// const uint CA_NOTYPE = 6;

// CA OFFSETS
const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},              // E NE N NW
                                 {1,1},{-1,1},{-1,-1},{1,-1}};           // W SW S SE
const vec2 neighborOffsetsDiamond[4] = {{1,0},{-1,0},{0,1},{0,-1}};  // W WW S SS
const float neighborDistances[8]={1.0,0.707,1.0,0.707,1.0,0.707,1.0,0.707};


const vec3 shadeCyclic[6] = {{208/255.f,57/255.f,98/255.f},{62/255.f,125/255.f,218/255.f},{252/255.f,249/255.f,50/255.f},
                            {250/255.f,135/255.f,50/255.f},{129/255.f,166/255.f,61/255.f},{100/255.f,100/255.f,100/255.f}};

const float shadeProtocell[4] = {0,               // empty
                                 0.4,0.8,         // X Y
                                 1.0};            // Z


////////////////////////////////////////////////////////////////////
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
#define minCol(col) (min(col.r,min(col.g,col.b)))
#define delta(gray) (gray>0.5?1:0)

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
// const uint pg_CA_FBO_attcht = 0;
// const uint pg_Pixel_FBO_attcht = 1;
// const uint pg_ParticlePosSpeed_FBO_attcht = 2;
// const uint pg_ParticleColRadius_FBO_attcht = 3;
// const uint pg_ParticleTargetPosColRadius_FBO_attcht = 4;
vec4 out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht + 1];


/////////////////////////////////////
// PG_NB_TRACKS TRACK COLOR FBO OUTPUT
 vec4 out_track_FBO[PG_NB_TRACKS];

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

// part acc & damp
float part_acc_factor;
float part_damp_factor;


////////////////////////////////////
// PEN STROKE CALCULATION
float signed_rx;
float signed_ry;

// cursor type (+1 for stylus , -1 for rubber )
float Cursor = 1;

////////////////////////////////////
// PIXEL UPDATE
// center of the translation of acceleration values
// used for shifting pixel motion in a direction
vec2 pixel_acc_center = vec2(0.5,0.5);

// out_pixel returned values
vec3 out_color_pixel;
vec2 out_position_pixel = vec2(0,0);
vec2 out_speed_pixel = vec2(0,0);

////////////////////////////////////
// VIDEO UPDATE
#ifdef PG_VIDEO_ACTIVE
  vec3 cameraImage = vec3( 0.0 );
  vec2 cameraCoord = vec2(0.0);
  vec2 cameraWH;
  vec3 movieImage = vec3( 0.0 );
  vec2 movieCoord = vec2(0.0);
  vec2 movieWH;
#endif

// SOBEL OFFSETS
vec2 offsetsVideo[9] = {vec2( -1, 1 ),vec2( 0, 1 ),vec2( 1, 1 ),
                      vec2( 0, -1 ),vec2( 0, 0 ),vec2( 0, 1 ),
                      vec2( -1, -1 ),vec2( 0, -1 ),vec2( 1, -1 )};
float sobelMatrixX[9] = {-1,0,1,-2,0,2,-1,0,1};
float sobelMatrixY[9] = {1,2,1,0,0,0,-1,-2,-1};

////////////////////////////////////
// CA UPDATE
vec4 out4_CA;
float CAdecay = 0.0;
bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiamond[4]=vec4[4](vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),
                                    vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec2 pixelTextureCoordinatesXY; // the POT coordinates of the
// pixel texture + z offset according to the chosen texture
vec2 noisepixels;
float radiuspixel;
vec4 randomCA;
vec4 randomCA2;

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
uniform vec4 uniform_Update_fs_4fv_paths03_x;
uniform vec4 uniform_Update_fs_4fv_paths03_y;
uniform vec4 uniform_Update_fs_4fv_paths03_x_prev;
uniform vec4 uniform_Update_fs_4fv_paths03_y_prev;
uniform vec4 uniform_Update_fs_4fv_paths03_r;
uniform vec4 uniform_Update_fs_4fv_paths03_g;
uniform vec4 uniform_Update_fs_4fv_paths03_b;
uniform vec4 uniform_Update_fs_4fv_paths03_a;
uniform vec4 uniform_Update_fs_4fv_paths03_BrushID;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusX;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusY;
uniform vec4 uniform_Update_fs_4fv_paths47_x;
uniform vec4 uniform_Update_fs_4fv_paths47_y;
uniform vec4 uniform_Update_fs_4fv_paths47_x_prev;
uniform vec4 uniform_Update_fs_4fv_paths47_y_prev;
uniform vec4 uniform_Update_fs_4fv_paths47_r;
uniform vec4 uniform_Update_fs_4fv_paths47_g;
uniform vec4 uniform_Update_fs_4fv_paths47_b;
uniform vec4 uniform_Update_fs_4fv_paths47_a;
uniform vec4 uniform_Update_fs_4fv_paths47_BrushID;
uniform vec4 uniform_Update_fs_4fv_paths47_RadiusX;
uniform vec4 uniform_Update_fs_4fv_paths47_RadiusY;
uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  
uniform vec4 uniform_Update_fs_4fv_flashTrkCAWghts;  
uniform vec4 uniform_Update_fs_4fv_flashTrkPartWghts;  
uniform vec4 uniform_Update_fs_4fv_trkDecay_partDecay;
uniform vec4 uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
uniform vec4 uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
uniform vec4 uniform_Update_fs_4fv_pulse;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_repop_Color_flashCABGWght;
uniform vec4 uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_photo01Wghts_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_repop_part_path_acc_damp_factor;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal;         // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Update pass position/speed of Particles step n (FBO attachment 2)
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Update pass color/radius of Particles step n (FBO attachment 3)
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // 2-cycle ping-pong Update pass target position/color/radius of Particles step n (FBO attachment 4)
layout (binding = 5) uniform samplerRect fs_lookupTable5;  // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 6) uniform samplerRect fs_lookupTable6;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 7) uniform samplerRect fs_lookupTable7;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 8) uniform samplerRect fs_lookupTable8;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
layout (binding = 8) uniform sampler3D   fs_lookupTable8;  // pen patterns
#ifdef PG_VIDEO_ACTIVE
layout (binding = 9) uniform samplerRect fs_lookupTable9;  // camera texture
layout (binding = 10) uniform samplerRect fs_lookupTable10;  // camera BG texture
layout (binding = 11) uniform samplerRect fs_lookupTable11;  // movie texture
#endif
layout (binding = 12) uniform sampler3D   fs_lookupTable12;  // noise texture
layout (binding = 13) uniform samplerRect fs_lookupTable13;  // photo_0 texture
layout (binding = 14) uniform samplerRect fs_lookupTable14;  // photo_1 texture
layout (binding = 15) uniform samplerRect fs_lookupTable15;  // FBO capture of particle rendering
layout (binding = 16) uniform samplerRect fs_lookupTable16;  // couples of textures for particle initialization through photo or video: position/speed
layout (binding = 17) uniform samplerRect fs_lookupTable17;  // couples of textures for particle initialization through photo: color/radius
                      // particle initialization from images

/////////////////////////////////////
// CA OUTPUT COLOR + STATE
layout (location = 0) out vec4 outColor0;

/////////////////////////////////////
// PIXEL OUTPUT
layout (location = 1) out vec4 outColor1;

/////////////////////////////////////
// PARTICLE OUTPUT
// POSITION / SPEED
layout (location = 2) out vec4 outColor2;
// COLOR / RADIUS
layout (location = 3) out vec4 outColor3;
// TARGET POSITION / COLOR / RADIUS
layout (location = 4) out vec4 outColor4;

/////////////////////////////////////
// TRACK COLOR OUTPUT
// the RGB channels contain the track Color
layout (location = 5) out vec4 outColor5;
#if PG_NB_TRACKS >= 2
layout (location = 6) out vec4 outColor6;
#endif
#if PG_NB_TRACKS >= 3
layout (location = 7) out vec4 outColor7; 
#endif
#if PG_NB_TRACKS >= 4
layout (location = 8) out vec4 outColor8;
#endif

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

vec2 generativeNoise(vec2 texCoordLoc) {
  // FLAT
    return vec2(snoise( texCoordLoc , noiseScale * 100 ),
                            snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseScale * 100 ));
  }


float Line( float x1 , float y1 , 
	    float mouse_x , float mouse_y , float interp_mouse_x ) {
  return mouse_y + (y1 - mouse_y)/(x1 - mouse_x) * (interp_mouse_x - mouse_x);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void CA_out( vec4 currentCA ) {
  // the ALPHA canal of fs_decal contains > 0 if it is a live cell

  // gets Moore neighbor values
  neighborValues[0] = texture(fs_decal, 
            decalCoords + neighborOffsets[0] );
  neighborValues[1] = texture(fs_decal, 
            decalCoords + neighborOffsets[1] );
  neighborValues[2] = texture(fs_decal, 
            decalCoords + neighborOffsets[2] );
  neighborValues[3] = texture(fs_decal, 
            decalCoords + neighborOffsets[3] );
  neighborValues[4] = texture(fs_decal, 
            decalCoords + neighborOffsets[4] );
  neighborValues[5] = texture(fs_decal, 
            decalCoords + neighborOffsets[5] );
  neighborValues[6] = texture(fs_decal, 
            decalCoords + neighborOffsets[6] );
  neighborValues[7] = texture(fs_decal, 
            decalCoords + neighborOffsets[7] );

  //////////////////////////////////////////////////////
  // CCA MOORE
  if( CAType == CA_CYCLIC  || CAType == CA_CYCLIC_1 ) {
    const int nbStates = 5;
    uint nbNeighbors[nbStates];

    nbNeighbors[0] =
      (round(neighborValues[0].a) == 0? 1:0) +
      (round(neighborValues[1].a) == 0? 1:0) +
      (round(neighborValues[2].a) == 0? 1:0) +
      (round(neighborValues[3].a) == 0? 1:0) +
      (round(neighborValues[4].a) == 0? 1:0) +
      (round(neighborValues[5].a) == 0? 1:0) +
      (round(neighborValues[6].a) == 0? 1:0) +
      (round(neighborValues[7].a) == 0? 1:0);
    
    nbNeighbors[1] =
      (round(neighborValues[0].a) == 1? 1:0) +
      (round(neighborValues[1].a) == 1? 1:0) +
      (round(neighborValues[2].a) == 1? 1:0) +
      (round(neighborValues[3].a) == 1? 1:0) +
      (round(neighborValues[4].a) == 1? 1:0) +
      (round(neighborValues[5].a) == 1? 1:0) +
      (round(neighborValues[6].a) == 1? 1:0) +
      (round(neighborValues[7].a) == 1? 1:0);
    
    nbNeighbors[2] =
      (round(neighborValues[0].a) == 2? 1:0) +
      (round(neighborValues[1].a) == 2? 1:0) +
      (round(neighborValues[2].a) == 2? 1:0) +
      (round(neighborValues[3].a) == 2? 1:0) +
      (round(neighborValues[4].a) == 2? 1:0) +
      (round(neighborValues[5].a) == 2? 1:0) +
      (round(neighborValues[6].a) == 2? 1:0) +
      (round(neighborValues[7].a) == 2? 1:0);
    
    nbNeighbors[3] =
      (round(neighborValues[0].a) == 3? 1:0) +
      (round(neighborValues[1].a) == 3? 1:0) +
      (round(neighborValues[2].a) == 3? 1:0) +
      (round(neighborValues[3].a) == 3? 1:0) +
      (round(neighborValues[4].a) == 3? 1:0) +
      (round(neighborValues[5].a) == 3? 1:0) +
      (round(neighborValues[6].a) == 3? 1:0) +
      (round(neighborValues[7].a) == 3? 1:0);
    
    nbNeighbors[4] =
      (round(neighborValues[0].a) == 4? 1:0) +
      (round(neighborValues[1].a) == 4? 1:0) +
      (round(neighborValues[2].a) == 4? 1:0) +
      (round(neighborValues[3].a) == 4? 1:0) +
      (round(neighborValues[4].a) == 4? 1:0) +
      (round(neighborValues[5].a) == 4? 1:0) +
      (round(neighborValues[6].a) == 4? 1:0) +
      (round(neighborValues[7].a) == 4? 1:0);
    
    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    uint newState = 0;
    if( currentCA.a < 0 ) {
      out4_CA.a = floor(randomCA.x * (nbStates+1)); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      out4_CA.rgb = shadeCyclic[newState];
    }
    else {
      // CCA with atuotmatic cycling from one step to the next
      if(CAType == CA_CYCLIC) {
        uint state = int(clamp(currentCA.a,0,nbStates));
        if( state < nbStates ) {
          uint nextState = (state + 1) % nbStates;
          vec3 nextColor = currentCA.rgb;
          newState = state;
          if(nbNeighbors[nextState] > 0) {
            newState = nextState;
            nextColor = 
              ((round(neighborValues[0].a)) == nextState? neighborValues[0].rgb:vec3(0)) +
              ((round(neighborValues[1].a)) == nextState? neighborValues[1].rgb:vec3(0)) +
              ((round(neighborValues[2].a)) == nextState? neighborValues[2].rgb:vec3(0)) +
              ((round(neighborValues[3].a)) == nextState? neighborValues[3].rgb:vec3(0)) +
              ((round(neighborValues[4].a)) == nextState? neighborValues[4].rgb:vec3(0)) +
              ((round(neighborValues[5].a)) == nextState? neighborValues[5].rgb:vec3(0)) +
              ((round(neighborValues[6].a)) == nextState? neighborValues[6].rgb:vec3(0)) +
              ((round(neighborValues[7].a)) == nextState? neighborValues[7].rgb:vec3(0));
            nextColor /= nbNeighbors[nextState];
          }

          nextColor -= vec3(CAdecay);
          if(nextColor.r > 0 &&  nextColor.g > 0 &&  nextColor.b > 0) {
            out4_CA.a = float(newState);
            out4_CA.rgb = nextColor;
          }
          else {
            out4_CA.a = float(nbStates);
            out4_CA.rgb = vec3(0);
          }

        }
        else {
            out4_CA.rgb = vec3(0);
        }
      }
      // b color used to count steps in a state
      else if(CAType == CA_CYCLIC_1) {
        uint state = int(clamp(currentCA.a,0,nbStates));
        const uint durations[nbStates] = {1,2,3,4,5};
        if( state < nbStates ) {
          uint nextState = (state + 1) % nbStates;
          vec3 nextColor = currentCA.rgb;
          newState = state;
          if(nbNeighbors[nextState] > 0 && nextColor.g <= 0) {
            newState = nextState;
            nextColor = 
              ((round(neighborValues[0].a)) == nextState? neighborValues[0].rgb:vec3(0)) +
              ((round(neighborValues[1].a)) == nextState? neighborValues[1].rgb:vec3(0)) +
              ((round(neighborValues[2].a)) == nextState? neighborValues[2].rgb:vec3(0)) +
              ((round(neighborValues[3].a)) == nextState? neighborValues[3].rgb:vec3(0)) +
              ((round(neighborValues[4].a)) == nextState? neighborValues[4].rgb:vec3(0)) +
              ((round(neighborValues[5].a)) == nextState? neighborValues[5].rgb:vec3(0)) +
              ((round(neighborValues[6].a)) == nextState? neighborValues[6].rgb:vec3(0)) +
              ((round(neighborValues[7].a)) == nextState? neighborValues[7].rgb:vec3(0));
            nextColor /= nbNeighbors[nextState];
            nextColor.g = durations[nextState];
          }

          nextColor.rb -= vec2(CAdecay);
          nextColor.g -= 1;
          if(nextColor.r > 0 &&  nextColor.b > 0) {
            out4_CA.a = float(newState);
            out4_CA.rb = nextColor.rb;
          }
          else {
            out4_CA.a = float(nbStates);
            out4_CA.rgb = vec3(0);
          }

        }
        else {
            out4_CA.rgb = vec3(0);
        }
      }
    }
  }
  //////////////////////////////////////////////////////
  // GOL MOORE
  else if( CAType == GOL_1 ) {
    const int nbStates = 2;
    uint nbNeighbors[nbStates];

    nbNeighbors[0] =
      (round(neighborValues[0].a) == 0? 1:0) +
      (round(neighborValues[1].a) == 0? 1:0) +
      (round(neighborValues[2].a) == 0? 1:0) +
      (round(neighborValues[3].a) == 0? 1:0) +
      (round(neighborValues[4].a) == 0? 1:0) +
      (round(neighborValues[5].a) == 0? 1:0) +
      (round(neighborValues[6].a) == 0? 1:0) +
      (round(neighborValues[7].a) == 0? 1:0);

    nbNeighbors[1] =
      (round(neighborValues[0].a) == 1? 1:0) +
      (round(neighborValues[1].a) == 1? 1:0) +
      (round(neighborValues[2].a) == 1? 1:0) +
      (round(neighborValues[3].a) == 1? 1:0) +
      (round(neighborValues[4].a) == 1? 1:0) +
      (round(neighborValues[5].a) == 1? 1:0) +
      (round(neighborValues[6].a) == 1? 1:0) +
      (round(neighborValues[7].a) == 1? 1:0);
    
    
    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    uint newState = 0;
    vec3 nextColor = currentCA.rgb;
    if( currentCA.a < 0 ) {
      out4_CA.a = floor(randomCA.x * (nbStates+1)); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      out4_CA.rgb = uniform_Update_fs_4fv_repop_Color_flashCABGWght.rgb;
    }
    else {
      // CCA with atuotmatic cycling from one step to the next
      if(CAType == GOL_1) {
        uint state = int(clamp(currentCA.a,0,nbStates));
        if( state < nbStates) {
          newState = 0;
          // if( (state == 0 && (nbNeighbors[1] == 3 || nbNeighbors[1] == 5 || nbNeighbors[1] == 7 || nbNeighbors[1] == 8) )
          //    || (state == 1 && (nbNeighbors[1] == 4 || nbNeighbors[1] == 6 || nbNeighbors[1] == 7 || nbNeighbors[1] == 8) ) ){
          //   newState = 1;
          // }
          if( (state == 1) 
             || (state == 0 && (nbNeighbors[1] == 3) ) ){
            newState = 1;
          }

          if(newState == 1 && nbNeighbors[newState] > 0) {
            nextColor = 
              ((round(neighborValues[0].a)) == newState? neighborValues[0].rgb:vec3(0)) +
              ((round(neighborValues[1].a)) == newState? neighborValues[1].rgb:vec3(0)) +
              ((round(neighborValues[2].a)) == newState? neighborValues[2].rgb:vec3(0)) +
              ((round(neighborValues[3].a)) == newState? neighborValues[3].rgb:vec3(0)) +
              ((round(neighborValues[4].a)) == newState? neighborValues[4].rgb:vec3(0)) +
              ((round(neighborValues[5].a)) == newState? neighborValues[5].rgb:vec3(0)) +
              ((round(neighborValues[6].a)) == newState? neighborValues[6].rgb:vec3(0)) +
              ((round(neighborValues[7].a)) == newState? neighborValues[7].rgb:vec3(0));
            nextColor /= nbNeighbors[newState];
          }

          nextColor = clamp(nextColor-vec3(CAdecay),0,1);
          if(newState == 0){
            nextColor = vec3(0);
          }
          if(nextColor.r > 0 ||  nextColor.g > 0 || nextColor.b > 0) {
            out4_CA.a = float(newState);
            out4_CA.rgb = nextColor;
          }
          else {
            out4_CA.a = float(0);
            out4_CA.rgb = vec3(0);
          }
        }
        else {
            out4_CA.rgb = vec3(0);
        }
      }
    }
  }

  else if( CAType == CA_PROTOCELLS ) {
    int nbNeighbors =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);

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
      averageSurrounding = (gatherColorFactors + currentCA.rgb)/8;
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

    // CAParams optimal values from Composites/soundinitiative²
    float CAParams1 = 0.480315;
    float CAParams2 = 0.511811;
    float CAParams3 = 0.637795;
    float CAParams4 = 0.472441;
    float CAParams5 = 0.244094;

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

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = clamp(vec4(currentCA.rgb-vec3(CAdecay),currentCA.a),0,1); // ready
  }

}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PIXELS UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void pixel_out( void ) {
  // fills by cumulating the neighboring pixels that
  // can reach the pixel at this frame (come into a circle of radius radiuspixel)
  // with the pixel seen as length unit.
  out_color_pixel = vec3(0,0,0);
  int nb_cumultated_pixels = 0;

  vec2 usedNeighborOffset;

  // looks in the 4 neighbors (S,N,E,W) whether one of them is 
  // going to reach the current cell
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    usedNeighborOffset = neighborOffsets[ind];
    vec2 newDecalCoord = decalCoords + usedNeighborOffset;
    if( newDecalCoord.x >= 0 && newDecalCoord.y >= 0 
        && newDecalCoord.x < width && newDecalCoord.y < height ) {
      vec4 surrpixel_localColor;
      surrpixel_localColor = texture( fs_lookupTable5, newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the loclation of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-radiuspixel,+radiuspixel]x[-radiuspixel,+radiuspixel]
      if( graylevel(surrpixel_localColor) >  0 ) {
        vec2 surrpixel_speed;
        vec2 surrpixel_position;
        vec2 surrpixel_nextPosition;

        vec4 surrpixel_speed_position = texture( fs_lookupTable1,
                newDecalCoord );
        surrpixel_speed = surrpixel_speed_position.xy;
        surrpixel_position = surrpixel_speed_position.zw;
        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = generativeNoise(pixelTexCoordLoc);

        vec2 acceleration;
        acceleration = pixel_acceleration - pixel_acc_center;
        if( pixel_acc_factor > 0 ) {
          // acceleration
          surrpixel_speed 
            += pixel_acc_factor * acceleration;
        }
        else {
          // damping
          surrpixel_speed 
            += pixel_acc_factor * surrpixel_speed;
        }
        surrpixel_nextPosition 
                 = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
        // the current step is added to the position

        if( abs(surrpixel_nextPosition.x) <= 0.5 
                  && abs(surrpixel_nextPosition.y) <= 0.5 ) {
          out_color_pixel += surrpixel_localColor.rgb;
          out_speed_pixel += surrpixel_speed;
          // computes the position of the pixel
          out_position_pixel += surrpixel_nextPosition;
          nb_cumultated_pixels++;
        }
        // radius pixel extension for (S,N,E,W) neighbors
        else if( abs(surrpixel_nextPosition.x) <= (radiuspixel - randomCA.z)
                  && abs(surrpixel_nextPosition.y) <= (radiuspixel - randomCA.w) ) {
          float dist = radiuspixel - length(surrpixel_nextPosition);
          out_color_pixel += surrpixel_localColor.rgb;
          // adds high frequency random speed to make them leave the initial pixel
          out_speed_pixel += dist * (randomCA.xy - vec2(0.5,0.5));

          // computes the position of the pixel
          out_position_pixel += surrpixel_nextPosition;
          nb_cumultated_pixels++;
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

      vec4 surrpixel_localColor;
      surrpixel_localColor = texture( fs_lookupTable5,
           newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the location of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-radiuspixel,+radiuspixel]x[-radiuspixel,+radiuspixel]
      if( graylevel(surrpixel_localColor) >  0 ) {
        vec2 surrpixel_speed;
        vec2 surrpixel_position;
        vec2 surrpixel_nextPosition;

        vec4 surrpixel_speed_position
          = texture( fs_lookupTable1,
               newDecalCoord );
              surrpixel_speed = surrpixel_speed_position.xy;
              surrpixel_position = surrpixel_speed_position.zw;

        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = generativeNoise(pixelTexCoordLoc);

        vec2 acceleration;
        acceleration = pixel_acceleration - pixel_acc_center;
        if( pixel_acc_factor > 0 ) {
          // acceleration
          surrpixel_speed 
            += pixel_acc_factor * acceleration;
        }
        else {
          // damping
          surrpixel_speed 
            += pixel_acc_factor * surrpixel_speed;
        }
        surrpixel_nextPosition 
                 = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
        // the current step is added to the position

        if( abs(surrpixel_nextPosition.x) <= (radiuspixel - randomCA.z)
                  && abs(surrpixel_nextPosition.y) <= (radiuspixel - randomCA.w) ) {
          out_color_pixel += surrpixel_localColor.rgb;
          out_speed_pixel += surrpixel_speed;
          // computes the position of the pixel
          out_position_pixel += surrpixel_nextPosition;
          nb_cumultated_pixels++;
        }
      }
    }
  }

  out_position_pixel 
    = normalize( out_position_pixel + vec2( 0.5, 0.5 ) ) 
    - vec2( 0.5, 0.5 );

  if( graylevel(out_color_pixel) > 0 ) {
    out_color_pixel /= nb_cumultated_pixels;
    out_color_pixel = clamp( out_color_pixel , 0.0 , 1.0 );
  }
  else {
    out_color_pixel = vec3(0,0,0);
  }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PARTICLES UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


void particle_out( void ) {
#ifdef CURVE_PARTICLES
  // index of the control point: 0 for the head and 1, 2, or 3 for the tail
  int index = int(decalCoords.x) % 4;
#endif

#ifdef CURVE_PARTICLES
  ////////////////////////////////////////////////////////////////////
  // FIRST VERTEX OF THE PRIMITIVE
  // for the head of the curve, takes acceleration from noise strength field
  if( index == 0 ) {
#endif

    //////////////////////////////////////////////////////////////////
    // COLOR AND RADIUS UPDATE VARIABLE
    // copies color and radius at preceding frame
    out_color_radius_particle = out_attachment_FBO[pg_ParticleColRadius_FBO_attcht];
    out_target_position_color_radius_particle = out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht];

    //////////////////////////////////////////////////////////////////
    // SPPED AND POSITION UPDATE VARIABLE
    // copies position and speed at preceding frame
    out_position_speed_particle = out_attachment_FBO[pg_ParticlePosSpeed_FBO_attcht];

    ////////////////////////////////////////////////////////////////////
    // INITIALIZATION
    // random initialization of the particle positions (initial speed is null currently)
    if(part_initialization >= 0) {
      /////////////////////////////////////////////////////////////////////
      // RANDOM INITIALIZATION
      // head and tail are initialized with the same values
      // reaches for regular grid position
      // fs_lookupTable16
      // float rank = (decalCoords.x + decalCoords.y * width) / float(nbParticles);
      // int rankGrid = int(rank * width * height);

      if(part_initialization < PG_NB_PARTICLE_INITIAL_IMAGES) {
          vec4 target_color_radius = texture( fs_lookupTable17 , decalCoords );
          if(partMove_target) { // reaches a target in several steps
              out_target_position_color_radius_particle.xy
                    = texture( fs_lookupTable16 , decalCoords ).xy;
              out_target_position_color_radius_particle.z
                = target_color_radius.r * 255. + target_color_radius.g * 65025. + target_color_radius.b * 16581375.;
              out_target_position_color_radius_particle.w = partRepopRadius;
          }
          else { // instant positioning on the target
              out_position_speed_particle
                    = texture( fs_lookupTable16 , decalCoords );
              out_color_radius_particle = vec4(target_color_radius.rgb, partRepopRadius);
          }
      }
      // camera
      else if(part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES ) {
          if(partMove_target) { // reaches a target in several steps
              out_target_position_color_radius_particle.xy
                    = texture( fs_lookupTable16 , decalCoords ).xy;
              vec2 cameraCoord = out_position_speed_particle.xy / vec2(width, height) * cameraWH;
              vec4 target_color = texture( fs_lookupTable9 , vec2(cameraCoord.x , cameraWH.y - cameraCoord.y) );
              out_target_position_color_radius_particle.z
                = target_color.r * 255. + target_color.g * 65025. + target_color.b * 16581375.;
              out_target_position_color_radius_particle.w = partRepopRadius;
          }
          else { // instant positioning on the target
              out_position_speed_particle
                    = texture( fs_lookupTable16 , decalCoords );
              out_position_speed_particle.zw = vec2(0,0);
              vec2 cameraCoord = out_position_speed_particle.xy / vec2(width, height) * cameraWH;
              vec4 target_color = texture( fs_lookupTable9 , vec2(cameraCoord.x , cameraWH.y - cameraCoord.y) );
              out_color_radius_particle = vec4(target_color.rgb, partRepopRadius);
          }
      }
      // movie
      else if(part_initialization == PG_NB_PARTICLE_INITIAL_IMAGES + 1 ) {
          if(partMove_target) { // reaches a target in several steps
              out_target_position_color_radius_particle.xy
                    = texture( fs_lookupTable16 , decalCoords ).xy;
              vec2 movieCoord = out_position_speed_particle.xy / vec2(width, height) * movieWH;
              vec4 target_color = texture( fs_lookupTable11 , vec2(movieCoord.x, movieWH.y - movieCoord.y)  );
              out_target_position_color_radius_particle.z
                = target_color.r * 255. + target_color.g * 65025. + target_color.b * 16581375.;
              out_target_position_color_radius_particle.w = partRepopRadius;
          }
          else { // instant positioning on the target
              out_position_speed_particle
                    = texture( fs_lookupTable16 , decalCoords );
              out_position_speed_particle.zw = vec2(0,0);
              vec2 movieCoord = out_position_speed_particle.xy / vec2(width, height) * movieWH;
              vec4 target_color = texture( fs_lookupTable11 , vec2(movieCoord.x, movieWH.y - movieCoord.y)  );
              out_color_radius_particle = vec4(target_color.rgb, partRepopRadius);
          }
      }
      return;
    }
    // position update according to speed, acceleration and damping

    //////////////////////////////////////////////////////////////////////
    // REPOP ALONG PATHS (A RANDOM PATH AMONG THE ACTIVE ONES IS CHOSEN AT EACH FRAME)
    // SINCE REPOPULATION IS AN ADDITIVE PROCESS, SEVERAL SOURCES OF PARTICLES CAN
    // EXIST SIMULTANEOUSLY
    int indPath = int(uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo.z);
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
    float repop_path = uniform_Update_fs_4fv_repop_part_path_acc_damp_factor.y;
    vec4 randomValue = texture( fs_lookupTable12 , vec3( decalCoordsPOT , 0.75 ) );
    if( repop_path > 0
        && int((frameNo+5000) * randomValue.w ) % int(15000 *(1-repop_path)) == int((randomValue.x * 5000.0
            + randomValue.z * 5000.0 + randomValue.y * 5000.0 ) *(1-repop_path)) ) {
        if( indPath >= 0 && indPath < 4 
          && uniform_Update_fs_4fv_paths03_x[indPath] > 0
          && uniform_Update_fs_4fv_paths03_y[indPath] > 0 ) {
         /////////////////////////////////////////////////////////////////////
          vec2 pen_prev = vec2( uniform_Update_fs_4fv_paths03_x_prev[indPath],
                    height - uniform_Update_fs_4fv_paths03_y_prev[indPath]);
          vec2 pen_curr = vec2( uniform_Update_fs_4fv_paths03_x[indPath],
                    height - uniform_Update_fs_4fv_paths03_y[indPath]);
          vec2 orth = pen_prev - pen_curr;
          if(length(orth) != 0) {
            orth.xy = orth.yx;
            orth.y *= -1;
            orth = normalize(orth);
          }
          out_position_speed_particle.xy 
            = mix(pen_curr, pen_prev, randomCA.z ); // linear position
          out_position_speed_particle.xy += orth * 2 * (randomCA.w - 0.5) 
                  * (uniform_Update_fs_4fv_paths03_RadiusX[indPath] / brushSizeDivider); // thickness
          out_position_speed_particle.zw = (randomValue.xy - vec2(0.5)) * vec2(0.1); // speed
          out_color_radius_particle 
              = vec4( uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz , 
                      partRepopRadius);
        }
#endif
#if PG_NB_PATHS == 7
        else if( indPath >= 4 
                  && uniform_Update_fs_4fv_paths47_x[indPath - 4] > 0
                  && uniform_Update_fs_4fv_paths47_y[indPath - 4] > 0 ) {
         /////////////////////////////////////////////////////////////////////
          vec2 pen_prev = vec2( uniform_Update_fs_4fv_paths47_x_prev[indPath - 4],
                    height - uniform_Update_fs_4fv_paths47_y_prev[indPath - 4]);
          vec2 pen_curr = vec2( uniform_Update_fs_4fv_paths47_x[indPath - 4],
                    height - uniform_Update_fs_4fv_paths47_y[indPath - 4]);
          vec2 orth = pen_prev - pen_curr;
          if(length(orth) != 0) {
            orth.xy = orth.yx;
            orth.y *= -1;
            orth = normalize(orth);
          }
          out_position_speed_particle.xy 
            = mix(pen_curr, pen_prev, randomCA.z ); // linear position
          out_position_speed_particle.xy += orth * 2 * (randomCA.w - 0.5) 
                  * (uniform_Update_fs_4fv_paths47_RadiusX[indPath - 4] / brushSizeDivider); // thickness
          out_position_speed_particle.zw = (randomValue.xy - vec2(0.5)) * vec2(0.1); // speed
          out_color_radius_particle 
              = vec4( uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz , 
                      partRepopRadius);
        }
#endif
      }

    //////////////////////////////////////////////////////////////////
    // // REPOP
    // Uniform repopulation
    if( out_position_speed_particle.x == -10000) {
          // if the pixel noise is equal to frame % 8500 the cell is repopulated with a pixel
          float repop_part = uniform_Update_fs_4fv_repop_part_path_acc_damp_factor.x;
          vec4 randomValue = texture( fs_lookupTable12 , vec3( decalCoordsPOT , 0.25 ) );
          if( repop_part > 0
              && int(frameNo * randomValue.w ) % int(15000 *(1-repop_part)) == int((randomValue.y * 5000.0
                + randomValue.z * 5000.0 + randomValue.x * 5000.0 ) *(1-repop_part)) ) {
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
                = vec4(uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz,partRepopRadius);
          }
    }

    //////////////////////////////////////////////////////////////////
    // FLASH CALCULATION AT PARTICLE PRECEDING POSITION
    // track 0 flash on particles
    flashToPartCumul = uniform_Update_fs_4fv_flashTrkPartWghts[0] *
                         texture( fs_lookupTable5 , out_position_speed_particle.xy ).rgb;
#if PG_NB_TRACKS >= 2
    // track 1 flash on particles
    flashToPartCumul += uniform_Update_fs_4fv_flashTrkPartWghts[1] *
                         texture( fs_lookupTable6 , out_position_speed_particle.xy ).rgb;
#endif
#if PG_NB_TRACKS >= 3
    // track 2 flash on particles
    flashToPartCumul += uniform_Update_fs_4fv_flashTrkPartWghts[2] *
                         texture( fs_lookupTable7 , out_position_speed_particle.xy ).rgb;
#endif
#if PG_NB_TRACKS >= 4
    // track 3 flash on particles
    flashToPartCumul += uniform_Update_fs_4fv_flashTrkPartWghts[3] *
                         texture( fs_lookupTable8 , out_position_speed_particle.xy ).rgb;
#endif
    // CA flash on particles
    flashToPartCumul += uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles.z
                      * texture( fs_decal , out_position_speed_particle.xy ).rgb;

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

    vec2 part_acceleration = vec2(0);
    float dist_to_target = 1000;

    ///////////////////////////////////////////////////////////////////
    // builds a path_follow or repulse vector so that it can be used in the for loop
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
    bvec4 path_follow03 = bvec4(part_path_follow_0,part_path_follow_1,part_path_follow_2,part_path_follow_3);
    bvec4 path_repulse03 = bvec4(part_path_repulse_0,part_path_repulse_1,part_path_repulse_2,part_path_repulse_3);
#endif
#if PG_NB_PATHS == 7
    bvec4 path_follow47 = bvec4(part_path_follow_4,part_path_follow_5,part_path_follow_6,part_path_follow_7);
    bvec4 path_repulse47 = bvec4(part_path_repulse_4,part_path_repulse_5,part_path_repulse_6,part_path_repulse_7);
#endif

    ///////////////////////////////////////////////////////////////////
    // PARTICLE: PATH FOLLOW OR REPULSE
    for( int indPath = 0 ; indPath <= PG_NB_PATHS ; indPath++ ) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
      if( indPath < 4 && path_follow03[indPath] ) {
        // reaches for pen position
        part_acceleration 
          = (vec2( uniform_Update_fs_4fv_paths03_x[indPath] , 
                            height - uniform_Update_fs_4fv_paths03_y[indPath] )
                      - out_position_speed_particle.xy);
        dist_to_target = length(part_acceleration);
        // adds some field disturbance
        part_acceleration += (generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
      }
#endif
#if PG_NB_PATHS == 7
      else if( indPath >= 4 && path_follow47[indPath - 4] ) {
        // reaches for pen position
        part_acceleration 
          = (vec2( uniform_Update_fs_4fv_paths47_x[indPath - 4] , 
                            height - uniform_Update_fs_4fv_paths47_y[indPath - 4] )
                      - out_position_speed_particle.xy);
        dist_to_target = length(part_acceleration);
        // adds some field disturbance
        part_acceleration += (generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
     }
#endif
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
      if( indPath < 4 && path_repulse03[indPath] ) {
        // escapes from pen position
        part_acceleration 
          = (out_position_speed_particle.xy 
               - vec2( (uniform_Update_fs_4fv_paths03_x[indPath]) , 
                       height - uniform_Update_fs_4fv_paths03_y[indPath] ));
      }
#endif
#if PG_NB_PATHS == 7
      else if( indPath >= 4 && path_repulse47[indPath - 4] ) {
        // reaches for pen position
        // escapes from pen position
        part_acceleration 
          = (out_position_speed_particle.xy 
               - vec2( (uniform_Update_fs_4fv_paths47_x[indPath - 4]) , 
                       height - uniform_Update_fs_4fv_paths47_y[indPath - 4] ));
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
        = (out_target_position_color_radius_particle.xy
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
      part_acceleration = vec2(0);
    }

    ///////////////////////////////////////////////////////////////////
    // PARTICLE: RANDOM MOTION
    if(partMove_rand) { // random motion
      // random motion
      part_acceleration = (randomCA.zw - vec2(0.5));
    }

    if(length(part_acceleration) > 0) {
      part_acceleration = normalize(part_acceleration);
    }

    // acceleration
    out_position_speed_particle.zw 
      += part_acc_factor * part_acceleration;
    // damping
    out_position_speed_particle.zw 
      -= part_damp_factor * out_position_speed_particle.zw;


    //////////////////////////////////////////////////////////////////
    // SPEED UPDATE FROM ACCELERATION AND DAMPING
    // reading the noise value for acceleration 
    float speed = length(out_position_speed_particle.zw);
    out_position_speed_particle.zw  
      = normalize(out_position_speed_particle.zw + part_field_weight * (generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center)) * speed;
    // part_acceleration 
    //   += part_field_weight * (generativeNoise(pixelTextureCoordinatesXY) - pixel_acc_center);
    // }


    // speed damping to reach a target
    if( dist_to_target < part_damp_targtRad ) {
        out_position_speed_particle.zw 
        -= 0.01 * ((part_damp_targtRad - dist_to_target) / part_damp_targtRad) * out_position_speed_particle.zw;
    }

    // position update from new speed value
    // with speed limit
    float l =length(out_position_speed_particle.zw);
    if(l > 50) {
      out_position_speed_particle.zw *= (50/l);
    }
    if(l < 0.0001) {
      out_position_speed_particle.zw = generativeNoise(pixelTextureCoordinatesXY);
    }
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
#ifdef CURVE_PARTICLES
  }
  // for the rest of the curve, it follows the head
  else {
    vec4 headout_position_speed_particle 
          = texture(fs_lookupTable2, decalCoords-vec2(index,0));

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
    //   out_position_speed_particle.zw += (snoise( decalCoords + vec2(9.4872,2.0937) , noiseScale * 100 ) - 0.5)
    //     * vec2(lookatHead.y,-lookatHead.x);
    // }

    // position update
    out_position_speed_particle.xy += out_position_speed_particle.zw;
  }
#endif
}


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// PEN STROKE UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


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
      = texture(fs_lookupTable8,
            		vec3( normalized_vector_to_mousePosition.x/2.0+0.5,
            		      0.5 - normalized_vector_to_mousePosition.y/2.0,
                      (current_brushID / 3) * 0.5 + 0.250))[current_brushID % 3];
  }

  // pen gray level
  return penOutGrayLevel;
}

float out_gray_drawing( float current_mouse_x , float current_mouse_y , 
			float prev_mouse_x , float prev_mouse_y ,
			float current_Brush_RadiusX , float current_Brush_RadiusY , 
			int current_brushID ,
			uint p_track_drawingStroke ) {

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
  // line stroke
  float return_out_gray_drawing = 0.0;
  float unsigned_rx = 10000.0;
  float unsigned_ry = 10000.0;
  if( p_track_drawingStroke == DRAWING_LINE ) {

    //////////////////////////////////////
    // GRAY LEVEL OUTPUT
    //////////////////////////////////////

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
      // reads the gray level of the brush at this position
      return_out_gray_drawing 
        = stroke_out( current_Brush_RadiusX , current_Brush_RadiusY , current_brushID );
    }
  }
  return return_out_gray_drawing;
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
  // sound pulse
  vec3 pulse = uniform_Update_fs_4fv_pulse.rgb;
  float average_pulse = uniform_Update_fs_4fv_pulse.a;

  // frame number
  frameNo = int(round(uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.y));

  // target Frame number
  targetFrameNo = int(round(uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo.w));

  // decay of drawing and CA layers
  CAdecay = uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.x  * (1.0 - CAdecay_pulse * average_pulse);
  
  // cellular automata
  CA_on_off = (CASubType > 0);

  // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.z;

  // pixels position speed update parameters
  pixel_acc_center = vec2(pixel_acc_center_0,pixel_acc_center_1);
  radiuspixel = uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.z;

  // working variables for screen dimension
  width = uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo.x;
  height = uniform_Update_fs_4fv_W_H_repopChannel_targetFrameNo.y;

  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // pixel texture + z offset according to the chosen texture
  vec2 position = vec2( 1.0 + sin(frameNo/50000.0),
                        1.0 + cos(frameNo/37000.0));
  vec2 noisePositionOffset = vec2(snoise( position , noiseScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  pixelTextureCoordinatesXY = decalCoordsPOT + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( fs_lookupTable12 , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.0 ) );
  randomCA2 = texture( fs_lookupTable12 , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.5 ) );

  nbParticles = int(uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles.w);

  // part acc and dâmp
  part_acc_factor = uniform_Update_fs_4fv_repop_part_path_acc_damp_factor.z;
  part_damp_factor = uniform_Update_fs_4fv_repop_part_path_acc_damp_factor.w;

#ifdef ATELIERS_PORTATIFS
  // non circle brush pen is smaller on the left screen part
  if(decalCoords.x < width / 2 && uniform_Update_fs_4fv_paths03_BrushID[0] != 0) {
      brushSizeDivider = 10;
  }
#endif            

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING 
  // AND COPY OF PRECEDING FRAME FBO VALUES
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////

  if(frameNo <= 10 || uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.x > 0) {
    outColor0 = vec4(0);  // CA
    outColor1 = vec4(0);  // pixel speed / position
    outColor2 = vec4(-10000,-10000,0,0);  // particle position / speed
    outColor3 = vec4(1,1,1,1);  // particle color / radius
    outColor4 = vec4(-10000,-10000,16646655,1);  // particle target position / color / radius
    outColor5 = vec4(0,0,0,1);  // tracks 0-(PG_NB_TRACKS-1)
#if PG_NB_TRACKS >= 2
    outColor6 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 3
    outColor7 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 4
    outColor8 = vec4(0,0,0,1);
#endif
    return;
  }

  ///////////////////////////////////////////////////
  // COPY OF PRECEDING FRAME FBO VALUES

  // CA, pixels and particle parameters FBO copy
  out_attachment_FBO[pg_CA_FBO_attcht]
    = texture( fs_decal , decalCoords );
  out_attachment_FBO[pg_Pixel_FBO_attcht]
    = texture( fs_lookupTable1 , decalCoords );
  out_attachment_FBO[pg_ParticlePosSpeed_FBO_attcht]
    = texture( fs_lookupTable2 , decalCoords );
  out_attachment_FBO[pg_ParticleColRadius_FBO_attcht]
    = texture( fs_lookupTable3 , decalCoords );
  out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht]
    = texture( fs_lookupTable4 , decalCoords );


  // track colors FBO copy
  // track 0 (BG track)
  // possible horizontal or vertical translation of background texture
  vec2 decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.xy;
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[0] = texture( fs_lookupTable5 , decalCoordsPrevStep );
  }
  else {
    out_track_FBO[0] = vec4( 0, 0, 0, 0 );;
  }

#if PG_NB_TRACKS >= 2
  // track 1
  // possible horizontal or vertical translation of background texture
#ifdef ATELIERS_PORTATIFS
  // central band drawing the sound track
  if(decalCoords.x >= width/2 - 70 && decalCoords.x <= width/2 + 70
    && path_replay_trackNo_1 >= 0 ) {
      if(decalCoords.y > height/2 ) {
          decalCoordsPrevStep = decalCoords 
                + uniform_Update_fs_4fv_xy_transl_tracks_0_1.zw;
      }
      else {
          decalCoordsPrevStep = decalCoords 
                - uniform_Update_fs_4fv_xy_transl_tracks_0_1.zw;
      }
  }
  else {
    // translation and symmetry of houses & boat in scenes 6-7-8
    if(particle_type == 0 /*moteur*/ && decalCoords.x >= width/2 + 70) {
      if(decalCoords.y > height/2 ) {
        if(frameNo % 16 == 0) {
          decalCoordsPrevStep = decalCoords + vec2(0,-1);
        }
        else {
          decalCoordsPrevStep = decalCoords;
        }
      }
      else {
          decalCoordsPrevStep = vec2(decalCoords.x, height - decalCoords.y);
      }
    }
    // normal drawing
    else {
      decalCoordsPrevStep = decalCoords;
    }
  }
#else
  decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.zw;
#endif

#ifdef ATELIERS_PORTATIFS
  // because of horizontal symmetry the central line must be black on track 1
  if(int(decalCoords.y) == height/2) {
    out_track_FBO[1] = vec4( 0, 0, 0, 0 );
  }
  else 
#endif

  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[1] = texture( fs_lookupTable6 , decalCoordsPrevStep );
  }
  else {
    out_track_FBO[1] = vec4( 0, 0, 0, 0 );
  }
#endif

  // track colors FBO copy
#if PG_NB_TRACKS >= 3
  out_track_FBO[2] 
    = texture( fs_lookupTable7 , decalCoords );
#endif
#if PG_NB_TRACKS >= 4
  out_track_FBO[3] 
    = texture( fs_lookupTable8 , decalCoords );
#endif

  // if freeze, just keep values as they are
  if( freeze ) {
    // FBO OUTPUTS
    outColor0 = out_attachment_FBO[pg_CA_FBO_attcht];
    outColor1 = out_attachment_FBO[pg_Pixel_FBO_attcht];
    outColor2 = out_attachment_FBO[pg_ParticlePosSpeed_FBO_attcht];
    outColor3 = out_attachment_FBO[pg_ParticleColRadius_FBO_attcht];
    outColor4 = out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht];
    outColor5 = out_track_FBO[0];
#if PG_NB_TRACKS >= 2
    outColor6 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
    outColor7 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
    outColor8 = out_track_FBO[3];
#endif
      return;
  }

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // VIDEO UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // each track possibly covers the previous color

  vec3 photocolor = vec3( 0.0 );
  if(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.x > 0) {
    photocolor += uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.x * texture(fs_lookupTable13, 
      vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.xy ).rgb;
  }
  if(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.y > 0) {
    photocolor += uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.y * texture(fs_lookupTable14,  
      vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.zw ).rgb;
  }

#ifdef PG_VIDEO_ACTIVE
  vec3 videocolor = vec3( 0.0 );

  float flashCameraTrkWght = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.z;
  // no flash Camera for the moment
  // flashCameraTrkWght = .0f;
  
  // VIDEO FRAME CAPTURE AND RENDERING
  // movie size
  movieWH = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.xy;
  // camera size
  cameraWH = uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.zw;

  // video texture used for drawing
/*   cameraCoord = vec2(0.4 * (decalCoordsPOT.x + 0.55), 0.4 * (1. - decalCoordsPOT.y) )
               * cameraWH;
 */  cameraCoord = vec2(1 - decalCoordsPOT.x, (decalCoordsPOT.y) )
               * cameraWH;
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  // image reading
  cameraImage = texture(fs_lookupTable9, cameraCoord ).rgb;
  // gamma correction
  cameraImage = vec3( pow(cameraImage.r,cameraGamma) , pow(cameraImage.g,cameraGamma) , pow(cameraImage.b,cameraGamma) );
  if( BGSubtr ) {
    cameraImage = abs(cameraImage - texture(fs_lookupTable10, cameraCoord ).rgb); // initial background subtraction
  }
  if( graylevel(cameraImage) < cameraThreshold ) {
    cameraImage = vec3(0.0);
  }

  // cameraImage = vec3(1) - cameraImage;

  movieImage = texture(fs_lookupTable11, movieCoord ).rgb;

  // Sobel on camera
  if( cameraSobel > 0 ) {
      vec3 samplerSobel;
      // sobel
      vec3 sobelX = vec3(0.0);
      vec3 sobelY = vec3(0.0);

      // samples the center pixel and its Moore neighborhood
      for( int i = 0 ; i < 4 ; i++ ) {
          samplerSobel = texture(fs_lookupTable9 , cameraCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          samplerSobel = texture(fs_lookupTable9 , cameraCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }

      samplerSobel = cameraImage;
      sobelX = mix( samplerSobel , sobelX , cameraSobel );
      sobelY = mix( samplerSobel , sobelY , cameraSobel );

      cameraImage = clamp( sqrt( sobelX * sobelX + sobelY * sobelY ) , 0.0 , 1.0 );
  }
  // Sobel on movie
  if( movieSobel > 0 ) {
      vec3 samplerSobel;
      // sobel
      vec3 sobelX = vec3(0.0);
      vec3 sobelY = vec3(0.0);

      // samples the center pixel and its Moore neighborhood
      for( int i = 0 ; i < 4 ; i++ ) {
          samplerSobel = texture(fs_lookupTable11 , movieCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          samplerSobel = texture(fs_lookupTable11 , movieCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }

      samplerSobel = movieImage;
      sobelX = mix( samplerSobel , sobelX , movieSobel );
      sobelY = mix( samplerSobel , sobelY , movieSobel );

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
  /*
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
    */
#endif

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // TRACK COLOR UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////

  //////////////////////////////////////
  //  FLASH BG - FLASH CA - FLASH PART
  vec3 flashToBGCumul = vec3(0.0);
  vec4 flashToCACumul = vec4(0.0);
  flashToPartCumul = vec3(0.0);

  /////////////////////////////////////
  // builds a path_replay_trackNo vector so that it can be used in the for loop
  #if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
  ivec3 path_replay_trackNo03 = ivec3(path_replay_trackNo_1,path_replay_trackNo_2,
                                      path_replay_trackNo_3);
  #endif
  #if PG_NB_PATHS == 7
  ivec4 path_replay_trackNo47 = ivec4(path_replay_trackNo_4,path_replay_trackNo_5,
                                      path_replay_trackNo_6,path_replay_trackNo_7);
  #endif

  /////////////////////////////////////
  // TRACK indTrack between 1 and PG_NB_TRACKS - 1 + track 0 in the end
  for( int indTrack = 1 ; indTrack <= PG_NB_TRACKS ; indTrack++ ) {
    // track 0 is processed last
    int indCurTrack = indTrack % PG_NB_TRACKS;
    // manages the gray level of each track
    vec4 curTrack_color = vec4(0);
    float path0_grayLevel = 0;
    float curTrack_grayLevel = 0;

    /////////////////
    // PEN OR PATH color
    for( int indPath = 0 ; indPath <= PG_NB_PATHS ; indPath++ ) {
      uint pathStroke = DRAWING_VOID;
      // pen drawing on the current track (comes last)
      if( indPath == 0 && currentDrawingTrack == indCurTrack ) {
        pathStroke = DRAWING_LINE;
      }
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
      // path drawing on current track
      else if( indPath > 0 && indPath < 4 && path_replay_trackNo03[indPath - 1] == indCurTrack ) {
        pathStroke = DRAWING_LINE;
      }
#endif
#if PG_NB_PATHS == 7
      // path drawing on current track
      else if( indPath >= 4 && path_replay_trackNo47[indPath - 4] == indCurTrack ) {
        pathStroke = DRAWING_LINE;
      }
#endif
      // drawing occurs
      if( pathStroke > 0 ) {
        if(indPath < 4) {
            curTrack_grayLevel =  out_gray_drawing( uniform_Update_fs_4fv_paths03_x[indPath] , 
                uniform_Update_fs_4fv_paths03_y[indPath] , 
                uniform_Update_fs_4fv_paths03_x_prev[indPath] , 
                uniform_Update_fs_4fv_paths03_y_prev[indPath] ,
                uniform_Update_fs_4fv_paths03_RadiusX[indPath] / brushSizeDivider,
                uniform_Update_fs_4fv_paths03_RadiusY[indPath] / brushSizeDivider,
                int(uniform_Update_fs_4fv_paths03_BrushID[indPath]),
                pathStroke );
            if(indPath == 0 && // rubber stylus
                               Cursor < 0) {
                out_track_FBO[indCurTrack].rgb *= (1 - curTrack_grayLevel);
                curTrack_grayLevel = 0;
            }
            curTrack_color.rgb
            += curTrack_grayLevel
                * uniform_Update_fs_4fv_paths03_a[indPath]
                * vec3( uniform_Update_fs_4fv_paths03_r[indPath] , 
                            uniform_Update_fs_4fv_paths03_g[indPath] , 
                            uniform_Update_fs_4fv_paths03_b[indPath] );  // brush opacity is combined with color opacity
        }
        else {
            curTrack_grayLevel =  out_gray_drawing( uniform_Update_fs_4fv_paths47_x[indPath - 4] , 
                uniform_Update_fs_4fv_paths47_y[indPath - 4] , 
                uniform_Update_fs_4fv_paths47_x_prev[indPath - 4] , 
                uniform_Update_fs_4fv_paths47_y_prev[indPath - 4] ,
                uniform_Update_fs_4fv_paths47_RadiusX[indPath - 4] / brushSizeDivider,
                uniform_Update_fs_4fv_paths47_RadiusY[indPath - 4] / brushSizeDivider,
                int(uniform_Update_fs_4fv_paths47_BrushID[indPath - 4]) ,
                pathStroke );
            curTrack_color.rgb
            += curTrack_grayLevel
                * uniform_Update_fs_4fv_paths47_a[indPath - 4]
                * vec3( uniform_Update_fs_4fv_paths47_r[indPath - 4] , 
                            uniform_Update_fs_4fv_paths47_g[indPath - 4] , 
                            uniform_Update_fs_4fv_paths47_b[indPath - 4] );  // brush opacity is combined with color opacity
        }
      }
    }
    float curTrackCumulatedGrayLevel = graylevel(curTrack_color.rgb);
    // drawing occurs on track indCurTrack
    if( curTrackCumulatedGrayLevel > 0 ) {
      out_track_FBO[indCurTrack].rgb 
          = out_track_FBO[indCurTrack].rgb * clamp((1 - curTrackCumulatedGrayLevel),0,1)
            + curTrack_color.rgb;
      if(indCurTrack != 0) {
        out_track_FBO[indCurTrack].a = 1.0;//track_grayLevel[indCurTrack];
      }
    }
    out_track_FBO[indCurTrack] = clamp( out_track_FBO[indCurTrack] , 0.0 , 1.0 );
    // out_track_FBO[1].rgb = 0.001 * out_track_FBO[1].rgb + .999 * vec3(1,0,0);

    /////////////////
    // TRACK photo
    if(currentPhotoTrack == indCurTrack 
      && uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.x + uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.y > 0 ) {
       out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
    }

    /////////////////
    // TRACK video
#ifdef PG_VIDEO_ACTIVE
    if( currentVideoTrack == indCurTrack && cameraWeight + movieWeight > 0) {
      if( cameraCumul == 1 ) { // ADD
        out_track_FBO[indCurTrack] 
          = vec4( clamp( max(videocolor,out_track_FBO[indCurTrack].rgb) , 0.0 , 1.0 ) ,  1.0 );
      }
      else if( cameraCumul == 2 ) {
        if( graylevel(videocolor) > 0) { // STAMP
          out_track_FBO[indCurTrack] = vec4( videocolor ,  1.0 );
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
      if( flashCameraTrkWght > 0 ) { // flash video
          // video image copy when there is a flash video
        out_track_FBO[indCurTrack].rgb = flashCameraTrkWght * cameraImage;
      }
      else {
        // flashCameraTrkWght = 0.0 <=> no flash video
        out_track_FBO[indCurTrack].rgb *= (1.0 - flashCameraTrkWght);
      }
    }
#endif

    // non BG track flash on BG track (only concerns tracks >= 1)
    if( indCurTrack != 0 ) {
      flashToBGCumul += uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[indCurTrack - 1] 
                        * out_track_FBO[indCurTrack].rgb;
    }

    // track flash on CA
    if( graylevel(out_track_FBO[indCurTrack].rgb) > .2 
        && graylevel(out_track_FBO[indCurTrack].rgb) < 2.1
        && flashCameraTrkWght == 0 ) { // no flash video
      flashToCACumul += uniform_Update_fs_4fv_flashTrkCAWghts[indCurTrack] 
                  * vec4(out_track_FBO[indCurTrack].rgb,graylevel(out_track_FBO[indCurTrack].rgb));
    }
  }

  ///////////////////////////////////////////////////
  // PARTICLE FLASHES ON CA & TRACKS
  // copies particle rendering at preceding step (for particle flashes)
  out_particlesRendering 
    = texture( fs_lookupTable15 , decalCoords );

  // particle flash on CA
  flashToCACumul += uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.w
                  * vec4(out_particlesRendering.rgb,graylevel(out_particlesRendering.rgb));

  // particle flash on BG track
  flashToBGCumul += uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght.w 
                      * out_particlesRendering.rgb;

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // CA LAYER: CA "SPREADING" UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  if( CA_on_off && (frameNo % CAstep == 0) ) {
    out4_CA = vec4(0.0);
    vec4 currentCA = out_attachment_FBO[pg_CA_FBO_attcht]; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // will disappear through the updating rule of the automaton)
    if( flashToCACumul.a > 0 ) { // gray level of copy color (in case of flash)
      currentCA.a = -1;
      // currentCA.rgb = clamp( currentCA.rgb + out_attachment_FBO[pg_CA_FBO_attcht].rgb , 0.0 , 1.0 );
      currentCA.rgb = clamp( flashToCACumul.rgb , 0.0 , 1.0 );
      // currentCA.rgb = out_track_FBO[1].rgb;
      // currentCA.rgb = vec3(1,0,0);
    }

/*
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
*/
     // calculates the new state of the automaton
    CA_out( currentCA );


    // CA "REPOPULATION"
    if( repop_CA > 0 ) {
      if( int(frameNo * randomCA2.w ) % int(15000 *(1-repop_CA)) == int((randomCA2.z * 5000.0
        + randomCA2.x * 5000.0 + randomCA2.y * 5000.0 ) *(1-repop_CA)) ) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz;
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_attachment_FBO[pg_CA_FBO_attcht] = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_Color_flashCABGWght.w == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Update_fs_4fv_repop_Color_flashCABGWght.w * out_attachment_FBO[pg_CA_FBO_attcht].rgb , 
            0 , 1 );

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // BACKGROUND LAYER: PIXEL "SPREADING" UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  if( pixel_mode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH pixel: increase radius for pixel capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles.y > 0 ) {
      radiuspixel = 1;
    }

    pixel_out();

    // arrival of a new pixel
    if( graylevel(out_color_pixel) > 0 ) {
      // outputs the color from pixel move at drawing layer
      out_track_FBO[0].rgb = out_color_pixel;
      out_attachment_FBO[pg_Pixel_FBO_attcht] 
	     = vec4( out_speed_pixel , out_position_pixel );
    }
    // update of the current pixel
    else {
      //  modifies speed according to acceleration
      vec2 pixel_acceleration;
      // FLAT
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseScale * 100 ),
                                snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));
      // }
      // // SUN RAYS
      // else if(noiseType == 1 ) {
      //   vec2 pos = vec2( atan((noiseCenter_0-pixelTextureCoordinatesXY.x)/(noiseCenter_1-pixelTextureCoordinatesXY.y)) * (noiseAngleScale * 10),
      //                    length(vec2(noiseCenter_0,noiseCenter_1) - pixelTextureCoordinatesXY) / (noiseLineScale) );
      //   pixel_acceleration = vec2(snoise( pos , noiseScale * 10 ) ,
      //                           snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
      // }
      // // CAMERA
      // else if(noiseType == 2 ) {
      //   pixel_acceleration = cameraImage.rg;
      // }
      // // MOVIE
      // else {
      //   pixel_acceleration = movieImage.rg;
      // }

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc_factor > 0 ) {
      	// acceleration
      	out_attachment_FBO[pg_Pixel_FBO_attcht].xy 
          += pixel_acc_factor * acceleration;
      }
      else {
      	// damping
      	out_attachment_FBO[pg_Pixel_FBO_attcht].xy 
          += pixel_acc_factor * out_attachment_FBO[pg_Pixel_FBO_attcht].xy;
      }
      // updates the position of the current pixel
      out_attachment_FBO[pg_Pixel_FBO_attcht].zw += out_attachment_FBO[pg_Pixel_FBO_attcht].xy; 

      // if the pixel leaves the cell, it is erased
      if( abs( out_attachment_FBO[pg_Pixel_FBO_attcht].z ) > 0.5
      	  || abs( out_attachment_FBO[pg_Pixel_FBO_attcht].w ) > 0.5 ) {
      	if( pixel_mode == PIXEL_UNIQUE ) {
      	  out_track_FBO[0].rgb = vec3(0);
      	}
      }
    }
  }

  // pixel "ADDITION"
  if( repop_BG > 0 ) {
    if( int(frameNo * randomCA2.z ) % int(15000 *(1-repop_BG)) == int((randomCA2.x * 5000.0
        + randomCA2.y * 5000.0 + randomCA2.w * 5000.0 ) *(1-repop_BG)) ) {
        out_track_FBO[0].rgb = uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz;
    }
  }

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
    // PARTICLE RANDOM MOTION FOR ATELIERS_PORTATIFS
#ifdef ATELIERS_PORTATIFS
    float pulsed_shift = uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.w;
    if(out_position_speed_particle.y < height/2) {
        out_position_speed_particle.xy -= vec2(0,pulsed_shift*randomCA.x); 
    }
    else {
        out_position_speed_particle.xy += vec2(0,pulsed_shift*randomCA.y); 
    }
#endif
    //////////////////////////////////////////////
    // particle decay
    if( graylevel(out_color_radius_particle.rgb) > 0 ) {
      out_color_radius_particle.rgb 
           = out_color_radius_particle.rgb - vec3(uniform_Update_fs_4fv_trkDecay_partDecay.w);
    }
    out_color_radius_particle.rgb 
      = clamp( out_color_radius_particle.rgb , 0.0 , 1.0 );

    //////////////////////////////////////////////
    // resets pixel speed and position if trackO returns to black
    if( graylevel(out_color_radius_particle.rgb) <= 0 ) {
      out_position_speed_particle.xy = vec2(-10000); 
      out_position_speed_particle.zw = vec2(0); 
    }

    out_attachment_FBO[pg_ParticlePosSpeed_FBO_attcht] = out_position_speed_particle;
    out_attachment_FBO[pg_ParticleColRadius_FBO_attcht] = out_color_radius_particle;
    out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht] = out_target_position_color_radius_particle;
  }

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // DRAWING AND CA LAYERS: GRAY LEVEL DECAY
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  
  //////////////////////////////////////////////
  // track 0 flashing of tracks >= 1 or particles
  out_track_FBO[0].rgb = clamp(out_track_FBO[0].rgb + flashToBGCumul, 0 , 1);

  //////////////////////////////////////////////
  // track decay and clear layer
  for(int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++) {
      if( graylevel(out_track_FBO[indTrack].rgb) > 0 ) {
#ifdef ATELIERS_PORTATIFS
        // no decay in the central zone of track 1
        if(indTrack != 1 || decalCoords.x < width/2 - 70 || decalCoords.x > width/2 + 70) {
#endif
          out_track_FBO[indTrack].rgb 
               = out_track_FBO[indTrack].rgb - vec3(uniform_Update_fs_4fv_trkDecay_partDecay[indTrack]);
#ifdef ATELIERS_PORTATIFS
        }
#endif
      }
      out_track_FBO[indTrack].rgb 
        = clamp( out_track_FBO[indTrack].rgb , 0.0 , 1.0 );
      // clear layer
      if( currentDrawingTrack == indTrack 
          && uniform_Update_fs_4fv_isClearLayer_flashPixel_flashCAPartWght_nbParticles.x > 0 ) {
        out_track_FBO[indTrack].rgb = vec3(0);
      }
  }

  //////////////////////////////////////////////
  // resets pixel speed and position if trackO returns to black
  if( graylevel(out_track_FBO[0].rgb) <= 0 ) {
    out_track_FBO[0].rgb = vec3(0);
    out_attachment_FBO[pg_Pixel_FBO_attcht] = vec4(0);
  }

  //////////////////////////////////////////////
  // copy tracks
  if( uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.w != 0 ) {
    int targetTrack;
    if( uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.w > 0) {
      targetTrack = (currentDrawingTrack + 1) % PG_NB_TRACKS;
    }
    else {
      targetTrack = (currentDrawingTrack + PG_NB_TRACKS - 1) % PG_NB_TRACKS;
    }

    out_track_FBO[targetTrack].rgb += out_track_FBO[currentDrawingTrack].rgb;
  }

  //////////////////////////////////////////////
  // CA LAYER CLEAR
  if( uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.y > 0 ) {
      out_attachment_FBO[pg_CA_FBO_attcht] = vec4(0);
  }

  //////////////////////////////////////////////
  //////////////////////////////////////////////
  // FBO OUTPUTS 
  //////////////////////////////////////////////
  //////////////////////////////////////////////
  outColor0 = out_attachment_FBO[pg_CA_FBO_attcht];
  outColor1 = out_attachment_FBO[pg_Pixel_FBO_attcht];
  outColor2 = out_attachment_FBO[pg_ParticlePosSpeed_FBO_attcht];
  outColor3 = out_attachment_FBO[pg_ParticleColRadius_FBO_attcht];
  outColor4 = out_attachment_FBO[pg_ParticleTargetPosColRadius_FBO_attcht];
  outColor5 = out_track_FBO[0];
#if PG_NB_TRACKS >= 2
  outColor6 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
  outColor7 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  outColor8 = out_track_FBO[3];
#endif
}
