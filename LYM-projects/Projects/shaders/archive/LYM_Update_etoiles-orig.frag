/***********************************************************************
File: LYM_Update_etoiles.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajim
a
*************************************************************************/

#version 430

#include_declarations

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;
/*
// Gaussian blur
const float weights_GaussianBlur[8][10] = {
  { 0.361710386853153, 0.117430169561019, 0.00401823943539796, 0, 0, 0, 0, 0, 0, 0, },
  { 0.160943513523684, 0.0976171754339892, 0.0217813359878234, 0.00178792093458716, 0, 0, 0, 0, 0, 0, },
  { 0.0906706626819157, 0.0684418069308968, 0.0294364543567006, 0.00721371337812935, 0.00100726007785354, 0, 0, 0, 0, 0, },
  { 0.0578976290770481, 0.0483601648793973, 0.0281818015679868, 0.0114578654743735, 0.00325006967689874, 0.000643184560989364, 0, 0, 0, 0, },
  { 0.0402440592208142, 0.0355152576097996, 0.0244092557887147, 0.0130653331225529, 0.0054464411532399, 0.00176820055872139, 0.000447071114568843, 0, 0, 0, },
  { 0.0296130974533233, 0.0270146688793152, 0.0205091228908708, 0.0129576472196559, 0.00681297573282909, 0.00298111793949366, 0.0010855581688134, 0.000328971797095601, 0, 0, },
  { 0.022659246945633, 0.0211207405530146, 0.0171040969458123, 0.0120342352127056, 0.00735638042938192, 0.00390694415457561, 0.00180275855491957, 0.000722711999073055, 0.000251721495878215, 0, },
  { 0.0178892106047989, 0.0169224681628769, 0.0143245600399185, 0.0108503547098669, 0.00735447434710435, 0.00446071417758884, 0.00242104138407988, 0.00117583149116467, 0.000511014259566371, 0.000198731178680599, },
};
*/

#define SPLAT_PARTICLES

#define PG_BEZIER_PATHS

////////////////////////////////////////////////////////////////////
// TRACK CONST
#define PG_NB_TRACKS 3
#define PG_NB_PATHS 7

///////////////////////////////////////////////////////////////////
const uint pg_FBO_fs_CA_attacht = 0;
const uint pg_FBO_fs_Pixels_attacht = 1;

////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA
// CA types
const uint CA_DISLOCATION = 0;
const uint CA_CYCLIC_1 = 1;
const uint CA_PROTOCELLS = 2;
const uint GOL_1 = 3;
const uint CA_CYCLIC = 4;
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

const float shadeDisloc[5] = {0,0.4,              // empty new/old border
                                 0.6, 0.8,  1.0};       // new/old nucleus
const float inverseShadeDisloc[5] = {0.0,2.5,              // empty new/old border
                                 1.67,1.25,  1.0};       // new/old nucleus

////////////////////////////////////////////////////////////////////
// pixel mode
const uint PIXEL_NONE = 0;
const uint PIXEL_UNIQUE = 1;
const uint PIXEL_FIREWORK = 2;

////////////////////////////////////////////////////////////////////
// drawing modes
const uint DRAWING_VOID = 0;
const uint DRAWING_BEZIER = 1;

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

// CA Type
int CAType;
int CASubType;

/////////////////////////////////////
// 3 FBO OUTPUT: CA, PIXELS AND PARTICLES
// const uint pg_FBO_fs_CA_attacht = 0;
// const uint pg_FBO_fs_Pixels_attacht = 1;
vec4 out_attachment_FBO[pg_FBO_fs_Pixels_attacht + 1];


/////////////////////////////////////
// PG_NB_TRACKS TRACK COLOR FBO OUTPUT
 vec4 out_track_FBO[PG_NB_TRACKS];

////////////////////////////////////
// PARTICLE UPDATE

// PARTICLE PASS OUTPUT
vec4 out_particlesRendering;

////////////////////////////////////
// PEN STROKE CALCULATION
float signed_rx;
float signed_ry;

// Bezier control points 
vec2 BezierControl[4];
bool isBegin;
bool isEnd;

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
vec3 cameraImage = vec3( 0.0 );
vec2 cameraCoord = vec2(0.0);
vec2 cameraWH;
vec3 movieImage = vec3( 0.0 );
vec2 movieCoord = vec2(0.0);
vec2 movieWH;

// SOBEL OFFSETS
vec2 offsetsVideo[9] = {vec2( -1, 1 ), vec2( 0, 1 ), vec2( 1, 1 ),
                        vec2( 0, -1 ), vec2( 0, 0 ), vec2( 0, 1 ),
                        vec2( -1, -1 ),vec2( 0, -1 ),vec2( 1, -1 )};
float sobelMatrixX[9] = {-1,0,1,-2,0,2,-1,0,1};
float sobelMatrixY[9] = {1,2,1,0,0,0,-1,-2,-1};

////////////////////////////////////
// CA UPDATE
vec4 out4_CA;

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiamond[4]=vec4[4](vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),
                                    vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec2 pixelTextureCoordinatesPOT_XY; // the POT coordinates of the
// pixel texture + z offset according to the chosen texture
vec2 noisepixels;

vec4 randomCA;

int currentScene;

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
// pen Bezier curve tangents
uniform vec4 uniform_Update_fs_4fv_paths03_xL;
uniform vec4 uniform_Update_fs_4fv_paths03_yL;
uniform vec4 uniform_Update_fs_4fv_paths03_xR;
uniform vec4 uniform_Update_fs_4fv_paths03_yR;
uniform ivec4 uniform_Update_fs_4iv_path0_next_in_hull;
uniform ivec4 uniform_Update_fs_4iv_path03_beginOrEnd;
uniform vec4 uniform_Update_fs_4fv_paths03_r;
uniform vec4 uniform_Update_fs_4fv_paths03_g;
uniform vec4 uniform_Update_fs_4fv_paths03_b;
uniform vec4 uniform_Update_fs_4fv_paths03_a;
#ifndef PG_BEZIER_PATHS
uniform vec4 uniform_Update_fs_4fv_paths03_BrushID;
#endif
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusX;

uniform vec4 uniform_Update_fs_4fv_paths47_x;
uniform vec4 uniform_Update_fs_4fv_paths47_y;
uniform vec4 uniform_Update_fs_4fv_paths47_x_prev;
uniform vec4 uniform_Update_fs_4fv_paths47_y_prev;
// pen Bezier curve tangents
uniform vec4 uniform_Update_fs_4fv_paths47_xL;
uniform vec4 uniform_Update_fs_4fv_paths47_yL;
uniform vec4 uniform_Update_fs_4fv_paths47_xR;
uniform vec4 uniform_Update_fs_4fv_paths47_yR;
uniform ivec4 uniform_Update_fs_4iv_path47_beginOrEnd;
uniform vec4 uniform_Update_fs_4fv_paths47_r;
uniform vec4 uniform_Update_fs_4fv_paths47_g;
uniform vec4 uniform_Update_fs_4fv_paths47_b;
uniform vec4 uniform_Update_fs_4fv_paths47_a;
#ifndef PG_BEZIER_PATHS
uniform vec4 uniform_Update_fs_4fv_paths47_BrushID;
#endif
uniform vec4 uniform_Update_fs_4fv_paths47_RadiusX;

uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  
uniform vec4 uniform_Update_fs_4fv_flashTrkCAWghts;  

uniform vec3 uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght;
uniform vec3 uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
uniform vec4 uniform_Update_fs_4fv_pulse;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_W_H_time_currentScene;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_repop_Color_flashCABGWght;
uniform vec3 uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_photo01Wghts_randomValues;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Update_texture_fs_CA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Update_texture_fs_Pixels;   // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
#ifdef PG_BEZIER_PATHS
layout (binding = 2) uniform samplerRect uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 3) uniform samplerRect uniform_Update_texture_fs_Camera_BG;     // camera BG texture
layout (binding = 4) uniform samplerRect uniform_Update_texture_fs_Movie_frame;   // movie textures
layout (binding = 5) uniform sampler3D   uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 6) uniform sampler2D   uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 7) uniform sampler2D   uniform_Update_texture_fs_Photo1;  // photo_1 texture
layout (binding = 8) uniform samplerRect uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering
layout (binding = 9) uniform samplerRect uniform_Update_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 10) uniform samplerRect uniform_Update_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 11) uniform samplerRect uniform_Update_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 12) uniform samplerRect uniform_Update_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
#else
layout (binding = 2) uniform sampler3D uniform_Update_texture_fs_Brushes; // pen patterns
layout (binding = 3) uniform samplerRect uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 4) uniform samplerRect uniform_Update_texture_fs_Camera_BG;     // camera BG texture
layout (binding = 5) uniform samplerRect uniform_Update_texture_fs_Movie_frame;   // movie textures
layout (binding = 6) uniform sampler3D   uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 7) uniform sampler2D   uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 8) uniform sampler2D   uniform_Update_texture_fs_Photo1;  // photo_1 texture
layout (binding = 9) uniform samplerRect uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering
layout (binding = 10) uniform samplerRect uniform_Update_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 11) uniform samplerRect uniform_Update_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 12) uniform samplerRect uniform_Update_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 13) uniform samplerRect uniform_Update_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
#endif

/////////////////////////////////////
// CA OUTPUT COLOR + STATE
layout (location = 0) out vec4 out_Update_FBO_fs_CA;

/////////////////////////////////////
// PIXEL OUTPUT
layout (location = 1) out vec4 out_Update_FBO_fs_Pixels;

/////////////////////////////////////
// TRACK COLOR OUTPUT
// the RGB channels contain the track Color
layout (location = 2) out vec4 out_Update_FBO_fs_Trk0;
#if PG_NB_TRACKS >= 2
layout (location = 3) out vec4 out_Update_FBO_fs_Trk1;
#endif
#if PG_NB_TRACKS >= 3
layout (location = 4) out vec4 out_Update_FBO_fs_Trk2; 
#endif
#if PG_NB_TRACKS >= 4
layout (location = 5) out vec4 out_Update_FBO_fs_Trk3;
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

vec2 multiTypeGenerativeNoise(vec2 texCoordLocPOT) {
  // FLAT
  if(noiseType == 0 )  {
    return vec2(snoise( texCoordLocPOT , noiseScale * 100 ),
                            snoise( texCoordLocPOT + vec2(2.0937,9.4872) , noiseScale * 100 ));
  }
  // SUN RAYS
  else if(noiseType == 1 ) {
    vec2 pos = vec2( atan((noiseCenterX-texCoordLocPOT.x)/(noiseCenterY-texCoordLocPOT.y)) * (noiseAngleScale * 10),
                     length(vec2(noiseCenterX,noiseCenterY) - texCoordLocPOT) / (noiseLineScale) );
    return vec2(snoise( pos , noiseScale * 10 ) ,
                            snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
  }
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
// CELLULAR AUTOMATA UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void CA_out( vec4 currentCA ) {
  // the ALPHA canal of uniform_Update_texture_fs_CA contains > 0 if it is a live cell

  // gets Moore neighbor values
  neighborValues[0] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[0] );
  neighborValues[1] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[1] );
  neighborValues[2] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[2] );
  neighborValues[3] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[3] );
  neighborValues[4] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[4] );
  neighborValues[5] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[5] );
  neighborValues[6] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[6] );
  neighborValues[7] = texture(uniform_Update_texture_fs_CA, 
            decalCoords + neighborOffsets[7] );

  //////////////////////////////////////////////////////
  // CCA MOORE
  /*
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
  */
  //////////////////////////////////////////////////////
  // GOL MOORE
  if( CAType == GOL_1 ) {
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
  /*
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
    // float CAParams1 = 0.480315;
    // float CAParams2 = 0.511811;
    // float CAParams3 = 0.637795;
    // float CAParams4 = 0.472441;
    // float CAParams5 = 0.244094;
 
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
          int firstindex = clamp(int(randomCA.x * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && randomCA.y < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(randomCA.z * 8),0,8);
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
      // randomCA.x choice of a first neighbor
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
  */

  //////////////////////////////////////////////////////
  // DISLOCATION
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_DISLOCATION ) {
    // CAParams optimal values from effe
/*     float CAParams1 = 0.393673;
    float CAParams2 = 0.102449;
    float CAParams3 = 0.53551;
    float CAParams4 = 0.165306;
    float CAParams5 = 0.196939;
    float CAParams6 = 0.322857;
 */
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
  // can reach the pixel at this frame (come into a circle of radius pixel_radius)
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
      surrpixel_localColor = texture( uniform_Update_texture_fs_Trk0, newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the loclation of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-pixel_radius,+pixel_radius]x[-pixel_radius,+pixel_radius]
      if( graylevel(surrpixel_localColor) >  0 ) {
        vec2 surrpixel_speed;
        vec2 surrpixel_position;
        vec2 surrpixel_nextPosition;

        vec4 surrpixel_speed_position = texture( uniform_Update_texture_fs_Pixels,
                newDecalCoord );
        surrpixel_speed = surrpixel_speed_position.xy;
        surrpixel_position = surrpixel_speed_position.zw;
        vec2 pixel_acceleration;
        vec2 pixelTexCoordLocPOT = pixelTextureCoordinatesPOT_XY
                       + usedNeighborOffset / vec2(width,height);

        // for the heaven's swirl, a specific texture for swirling the particles
        if(currentScene == 2) {
          vec2 acc = texture( uniform_Update_texture_fs_Trk1 , newDecalCoord ).gb 
                             - texture( uniform_Update_texture_fs_Trk1 , newDecalCoord + vec2(10)).gb;
          if(acc.x*acc.y != 0) {
            pixel_acceleration = acc / 2.0 + vec2(0.5);
          }
          else {
            pixel_acceleration = 1.5 * vec2(snoise( pixelTextureCoordinatesPOT_XY , noiseScale * 100 ),
                              snoise( pixelTextureCoordinatesPOT_XY + vec2(2.0937,9.4872) , noiseScale * 100 ));
          }
        }
/*        else if(currentScene >= 7) {
          vec2 acc = texture( uniform_Update_texture_fs_Trk2 , newDecalCoord ).gb 
                             - texture( uniform_Update_texture_fs_Trk2 , newDecalCoord + vec2(5,-5)).rg;
          if(acc.x*acc.y != 0) {
            pixel_acceleration = acc / 2.0 + vec2(0.5);
          }
          else {
            pixel_acceleration = randomCA.rg;
          }
        }
*/        // generative noise
        else {
          pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT);
        }

        vec2 acceleration;
        acceleration = pixel_acceleration - pixel_acc_center;
        if( pixel_acc > 0 ) {
          // acceleration
          surrpixel_speed 
            += pixel_acc * acceleration;
        }
        else {
          // damping
          surrpixel_speed 
            += pixel_acc * surrpixel_speed;
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
        else if( abs(surrpixel_nextPosition.x) <= (pixel_radius - randomCA.z)
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius - randomCA.w) ) {
          float dist = pixel_radius - length(surrpixel_nextPosition);
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
      surrpixel_localColor = texture( uniform_Update_texture_fs_Trk0,
           newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the location of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-pixel_radius,+pixel_radius]x[-pixel_radius,+pixel_radius]
      if( graylevel(surrpixel_localColor) >  0 ) {
        vec2 surrpixel_speed;
        vec2 surrpixel_position;
        vec2 surrpixel_nextPosition;

        vec4 surrpixel_speed_position
          = texture( uniform_Update_texture_fs_Pixels,
               newDecalCoord );
              surrpixel_speed = surrpixel_speed_position.xy;
              surrpixel_position = surrpixel_speed_position.zw;

        vec2 pixel_acceleration;
        vec2 pixelTexCoordLocPOT = pixelTextureCoordinatesPOT_XY
                       + usedNeighborOffset / vec2(width,height);

        // for the heaven's swirl, a specific texture for swirling the particles
        if(currentScene == 2) {
          vec2 acc = texture( uniform_Update_texture_fs_Trk1 , newDecalCoord ).gb 
                             - texture( uniform_Update_texture_fs_Trk1 , newDecalCoord + vec2(10)).gb;
          if(acc.x*acc.y != 0) {
            pixel_acceleration = acc / 2.0 + vec2(0.5);
          }
          else {
            pixel_acceleration = 1.5 * vec2(snoise( pixelTextureCoordinatesPOT_XY , noiseScale * 100 ),
                              snoise( pixelTextureCoordinatesPOT_XY + vec2(2.0937,9.4872) , noiseScale * 100 ));
          }
        }
/*        else if(currentScene >= 7) {
          vec2 acc = texture( uniform_Update_texture_fs_Trk2 , newDecalCoord ).gb 
                             - texture( uniform_Update_texture_fs_Trk2 , newDecalCoord + vec2(5,-5)).rg;
          if(acc.x*acc.y != 0) {
            pixel_acceleration = acc / 2.0 + vec2(0.5);
          }
          else {
            pixel_acceleration = randomCA.rg;
          }
        }
*/        // generative noise
        else {
          pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT);
        }

        vec2 acceleration;
        acceleration = pixel_acceleration - pixel_acc_center;
        if( pixel_acc > 0 ) {
          // acceleration
          surrpixel_speed 
            += pixel_acc * acceleration;
        }
        else {
          // damping
          surrpixel_speed 
            += pixel_acc * surrpixel_speed;
        }
        surrpixel_nextPosition 
                 = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
        // the current step is added to the position

        if( abs(surrpixel_nextPosition.x) <= (pixel_radius - randomCA.z)
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius - randomCA.w) ) {
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
// PEN STROKE UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

float OneMinusT;
float OneMinusT_sqare;
float T_sqare;

// Found in nvidia sdk
vec2 BezierVal(float T )
{
    OneMinusT = 1.0 - T;
    OneMinusT_sqare = OneMinusT*OneMinusT;
    T_sqare = T*T;
    float b0 = OneMinusT*OneMinusT_sqare;
    float b1 = 3.0*T*OneMinusT_sqare;
    float b2 = 3.0*T_sqare*OneMinusT;
    float b3 = T*T_sqare;
    return b0*BezierControl[0] + b1*BezierControl[1] + b2*BezierControl[2] + b3*BezierControl[3];
}
// should always be called after BezierVal because of variable initialization
vec2 TangentVal(float T )
{
    float d0 = 3.0*OneMinusT_sqare;
    float d1 = 6.0*T*OneMinusT;
    float d2 = 3.0*T_sqare;
    return d0*(BezierControl[1] - BezierControl[0]) + d1*(BezierControl[2] - BezierControl[1]) + d2*(BezierControl[3] - BezierControl[2]);
}

// BezierControl the coordinates of the four control points
// uniform_Update_fs_4iv_path0_next_in_hull the next index for the convex hull
// grey color return value (a mask)
float out_gray_drawing( float current_Brush_Radius ) {

  /////////////////////////////////////////////////
  // point in screen coordinates
  vec2 PixelLocation = vec2(decalCoords.x , height - decalCoords.y);

  /////////////////////////////////////////////////
  // The cubic Bezier curve hasfour control points BezierControl
  // from the previous mouse position BezierControl[0].x BezierControl[0].y 
  // to the current one BezierControl[3].x BezierControl[3].y 

  /////////////////////////////////////////////////
  // a segment made of 2 distinct real points 
  if( BezierControl[0].x >= 0 && BezierControl[0].y >= 0 
      && BezierControl[3].x >= 0 && BezierControl[3].y >= 0
      && (BezierControl[0].x != BezierControl[3].x
          || BezierControl[0].y != BezierControl[3].y) ) {

    // Bezier curve based rendering
    // checks whether the point is inside the convex hull of the control points
    // the convex hull is given by the next vertex of each vertex on the  hull
    // the test is made by counting how many times a horizontal line from the point
    // cuts the hull, odd: outside, even: inside 

    // checks whether current point is inside the hull of the bezier curve 
    // and takes into consideration the radius
    int countIntersections = 0;
    for(int i = 1; i < 5; i++) {
      // case of a convex polygon: only triangle the fourth point is inside the triangle
      int indHull = i % 4;
      if(uniform_Update_fs_4iv_path0_next_in_hull[indHull] != -1) {
        float yA = BezierControl[uniform_Update_fs_4iv_path0_next_in_hull[i-1]].y;
        float yB = BezierControl[uniform_Update_fs_4iv_path0_next_in_hull[indHull]].y;
        float xA = BezierControl[uniform_Update_fs_4iv_path0_next_in_hull[i-1]].x;
        float xB = BezierControl[uniform_Update_fs_4iv_path0_next_in_hull[indHull]].x;

        if(yA != yB) {
            float x_intersection = ((xB - xA) * PixelLocation.y + yB * xA - xB * yA)/(yB - yA);
            if(PixelLocation.x <= x_intersection - current_Brush_Radius
              || (PixelLocation.x <= x_intersection + current_Brush_Radius
                && PixelLocation.x >= x_intersection - current_Brush_Radius)) { 
              countIntersections++;
            }
        }
      }
    }
    // odd number of intersections -> outside the hull
    if(countIntersections % 2 == 0) {
      return  0.f; 
    }

    // marching along the Bezier curve
    // 3-pixel steps
    int nb_steps = int( length(BezierControl[1]-BezierControl[0])
                      + length(BezierControl[2]-BezierControl[1])
                      + length(BezierControl[3]-BezierControl[2]) ) / 3 + 1;
    // not more than 10 steps
    nb_steps = max(10, nb_steps);

    // marching along the curve and stopping when the point is between the frontiers of the 
    // line trapeze (slab) delimited by lines orthogonal to tangents
    float step = 1. / nb_steps;

    // first point in the subdivision (preceding point)
    float param_prec = 0.;
    vec2 P_prec = BezierVal(param_prec); // is BezierControl[0] but has to be calculated to initialize the values for TangentVal
    float T_prec = dot(vec2(PixelLocation - P_prec),normalize(TangentVal(param_prec)));
    vec2 P_cur;
    float T_cur;

    /////////////////////////////////////////////////
    // a segment beginning
    if( isBegin && T_prec < 0) {
      /////////////////////////////////////////////////
      // WRITING POSITION
      // distance from pen center
      signed_rx = PixelLocation.x - BezierControl[3].x;
      signed_ry = PixelLocation.y - BezierControl[3].y;
      float distanceToCurve = length( vec2(signed_rx , signed_ry) );

      if( // doesnt redraw on previously drawn place
          distanceToCurve < current_Brush_Radius ) {
        // reads the gray level of the brush at this position
        return 1.0f - distanceToCurve / current_Brush_Radius;
        // to reactivate later
        // return stroke_out( current_Brush_Radius , current_brushID );
      }
    }    

    // loop on all the ponts in the subdivision
    for(int ind_step = 1 ; ind_step <= nb_steps ; ind_step++) {
      // current point in the subdivision
      float param_cur = ind_step * step;
      P_cur = BezierVal(param_cur);
      T_cur = dot(vec2(PixelLocation - P_cur) , normalize(TangentVal(param_cur)));
      // inside the slab delimited by two points and their normals
      if(T_prec >= 0 && T_cur <= 0) {
        float interpolatedParam = (-T_cur * param_prec + T_prec * param_cur)/(T_prec - T_cur);

        /////////////////////////////////////////////////
        // WRITING POSITION
        // distance from pen center
        vec2 PointOnCurve = BezierVal(interpolatedParam);
        signed_rx = PixelLocation.x - PointOnCurve.x;
        signed_ry = PixelLocation.y - PointOnCurve.y;
        float distanceToCurve = length( vec2(signed_rx , signed_ry) );

        if( // doesnt redraw on previously drawn place
            distanceToCurve < current_Brush_Radius ) {
          // reads the gray level of the brush at this position
          return 1.0f - distanceToCurve / current_Brush_Radius;
          // to reactivate later
          // return stroke_out( current_Brush_Radius , current_brushID );
        }
        break;
      }

      // memorization of the current point
      param_prec = param_cur;
      T_prec = T_cur;
      P_prec = P_cur;
    }

    /////////////////////////////////////////////////
    // a segment ending
    if( isEnd && T_cur < 0) {
      /////////////////////////////////////////////////
      // WRITING POSITION
      // distance from pen center
      signed_rx = PixelLocation.x - BezierControl[0].x;
      signed_ry = PixelLocation.y - BezierControl[0].y;
      float distanceToCurve = length( vec2(signed_rx , signed_ry) );

      if( // doesnt redraw on previously drawn place
          distanceToCurve < current_Brush_Radius ) {
        // reads the gray level of the brush at this position
        return 1.0f - distanceToCurve / current_Brush_Radius;
        // to reactivate later
        // return stroke_out( current_Brush_Radius , current_brushID );
      }
    }    
  }

  return 0.f;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN OF UPDATE PASS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void main() {
#include_initializations

  //////////////////////////
  // TRACK DECAY
  vec4 trkDecay = vec4(trkDecay_0,trkDecay_1,trkDecay_2,trkDecay_3);

  //////////////////////////
  // variables 
  // sound pulse
  vec3 pulse = uniform_Update_fs_4fv_pulse.rgb;
  

  // frame number
  frameNo = int(round(uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.x));

 // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.y;

  // pixels position speed update parameters
  pixel_acc_center = vec2(pixel_acc_shiftX,pixel_acc_shiftY);
  

  // working variables for screen dimension
  width = uniform_Update_fs_4fv_W_H_time_currentScene.x;
  height = uniform_Update_fs_4fv_W_H_time_currentScene.y;

  // CAType
  CAType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.x);
  CASubType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.y);

  // cellular automata
  CA_on_off = (CASubType > 0);

  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // pixel texture + z offset according to the chosen texture
  vec2 position = vec2( 1.0 + sin(frameNo/50000.0),
                        1.0 + cos(frameNo/37000.0));
  vec2 noisePositionOffset = vec2(snoise( position , noiseScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  pixelTextureCoordinatesPOT_XY = decalCoordsPOT 
                  + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesPOT_XY , 0.0 ) );

  // current scene
  currentScene = int(uniform_Update_fs_4fv_W_H_time_currentScene.w);

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING 
  // AND COPY OF PRECEDING FRAME FBO VALUES
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////

  if(frameNo <= 10 || uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift.x > 0) {
    out_Update_FBO_fs_CA = vec4(0);  // CA
    out_Update_FBO_fs_Pixels = vec4(0);  // pixel speed / position
    out_Update_FBO_fs_Trk0 = vec4(0,0,0,1);  // tracks 0-(PG_NB_TRACKS-1)
#if PG_NB_TRACKS >= 2
    out_Update_FBO_fs_Trk1 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 3
    out_Update_FBO_fs_Trk2 = vec4(0,0,0,1);
#endif
#if PG_NB_TRACKS >= 4
    out_Update_FBO_fs_Trk3 = vec4(0,0,0,1);
#endif
    return;
  }

  ///////////////////////////////////////////////////
  // COPY OF PRECEDING FRAME FBO VALUES

  // CA, pixels and particle parameters FBO copy
  out_attachment_FBO[pg_FBO_fs_CA_attacht]
    = texture( uniform_Update_texture_fs_CA , decalCoords );
  out_attachment_FBO[pg_FBO_fs_Pixels_attacht]
    = texture( uniform_Update_texture_fs_Pixels , decalCoords );

  // track colors FBO copy
  // track 0 (BG track)
  // possible horizontal or vertical translation of background texture
  vec2 decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.xy;
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[0] = texture( uniform_Update_texture_fs_Trk0 , decalCoordsPrevStep );
    // BLUR
  }
  else {
    out_track_FBO[0] = vec4( 0, 0, 0, 0 );
  }

#if PG_NB_TRACKS >= 2
  // track 1
  // possible horizontal or vertical translation of background texture
  decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.zw;

  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[1] = texture( uniform_Update_texture_fs_Trk1 , decalCoordsPrevStep );
  /*
    // BLUR
    if(uniform_Update_fs_4fv_CAType_SubType_blurRadius.z >= 2) {
      int blurRad = min(int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.z),10);
      vec3 valPixel = vec3(0);
      float totWeight = 0;
      for( int i = -blurRad ; i <= blurRad ; i++ ) {
        for( int j = -blurRad ; j <= blurRad ; j++ ) {
          int dist = min(int(length(vec2(i,j))),10);
          float locWeight = weights_GaussianBlur[blurRad - 2][dist];
          valPixel += texture( uniform_Update_texture_fs_Trk1 , decalCoordsPrevStep + vec2(i,j)).rgb 
                      * locWeight;
          totWeight += locWeight;
        }
      }
      out_track_FBO[1] = vec4(valPixel/totWeight, out_track_FBO[1].a);
    }
    */
  }
#endif

  // track colors FBO copy
#if PG_NB_TRACKS >= 3
  out_track_FBO[2] 
    = texture( uniform_Update_texture_fs_Trk2 , decalCoords );
  // BLUR
    /*
  if(uniform_Update_fs_4fv_CAType_SubType_blurRadius.w >= 2) {
    int blurRad = min(int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.w),10);
    vec3 valPixel = vec3(0);
    float totWeight = 0;
    for( int i = -blurRad ; i <= blurRad ; i++ ) {
      for( int j = -blurRad ; j <= blurRad ; j++ ) {
        int dist = min(int(length(vec2(i,j))),10);
        float locWeight = weights_GaussianBlur[blurRad - 2][dist];
        valPixel += texture( uniform_Update_texture_fs_Trk2 , decalCoordsPrevStep + vec2(i,j)).rgb 
                    * locWeight;
        totWeight += locWeight;
      }
    }
    out_track_FBO[2] = vec4(valPixel/totWeight, out_track_FBO[1].a);
  }
  */
#endif
#if PG_NB_TRACKS >= 4
  out_track_FBO[3] 
    = texture( uniform_Update_texture_fs_Trk3 , decalCoords );
  // BLUR
#endif

  // if freeze, just keep values as they are
  if( freeze ) {
    // FBO OUTPUTS
    out_Update_FBO_fs_CA = out_attachment_FBO[pg_FBO_fs_CA_attacht];
    out_Update_FBO_fs_Pixels = out_attachment_FBO[pg_FBO_fs_Pixels_attacht];
    out_Update_FBO_fs_Trk0 = out_track_FBO[0];
#if PG_NB_TRACKS >= 2
    out_Update_FBO_fs_Trk1 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
    out_Update_FBO_fs_Trk2 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
    out_Update_FBO_fs_Trk3 = out_track_FBO[3];
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
  vec2 coordsImage = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y);
  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x > 0) {
    coordsImage *= uniform_Update_fs_4fv_photo01_wh.xy;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
    photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x * texture(uniform_Update_texture_fs_Photo0, 
        coordsImageScaled ).rgb;
  }
  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y > 0) {
    coordsImage *= uniform_Update_fs_4fv_photo01_wh.zw;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
    photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y * texture(uniform_Update_texture_fs_Photo1,  
        coordsImageScaled ).rgb;
  }
  photocolor *= (vec3(photo_value) + photo_value * photo_value_pulse * pulse);

  vec3 videocolor = vec3( 0.0 );

  float flashCameraTrkWght = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.z;
  // no flash Camera for the moment
  // flashCameraTrkWght = .0f;
  
  // VIDEO FRAME CAPTURE AND RENDERING
  // movie size
  movieWH = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.xy;
  // camera size
  cameraWH = uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw;

  // video texture used for drawing
/*   cameraCoord = vec2(0.4 * (decalCoordsPOT.x + 0.55), 0.4 * (1. - decalCoordsPOT.y) )
               * cameraWH;
     cameraCoord = vec2(1 - decalCoordsPOT.x, (decalCoordsPOT.y) )
               * cameraWH;
 */
  cameraCoord = vec2(decalCoordsPOT.x, (1 - decalCoordsPOT.y) )
              // added for wide angle lens that covers more than the drawing surface
               * cameraWH + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  // image reading
  cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
  // gamma correction
  // cameraImage = vec3( pow(cameraImage.r,cameraGamma) , pow(cameraImage.g,cameraGamma) , pow(cameraImage.b,cameraGamma) );
  if( camera_BG_subtr ) {
    cameraImage = abs(cameraImage - texture(uniform_Update_texture_fs_Camera_BG, cameraCoord ).rgb); // initial background subtraction
  }
  if( graylevel(cameraImage) < cameraThreshold ) {
    cameraImage = vec3(0.0);
  }

  // cameraImage = vec3(1) - cameraImage;

  movieImage = texture(uniform_Update_texture_fs_Movie_frame, movieCoord ).rgb;

  // Sobel on camera
  if( cameraSobel > 0 ) {
      vec3 samplerSobel;
      // sobel
      vec3 sobelX = vec3(0.0);
      vec3 sobelY = vec3(0.0);

      // samples the center pixel and its Moore neighborhood
      for( int i = 0 ; i < 4 ; i++ ) {
          samplerSobel = texture(uniform_Update_texture_fs_Camera_frame , cameraCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          samplerSobel = texture(uniform_Update_texture_fs_Camera_frame , cameraCoord + offsetsVideo[i]).rgb;
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
          samplerSobel = texture(uniform_Update_texture_fs_Movie_frame , movieCoord + offsetsVideo[i]).rgb;
          sobelX += sobelMatrixX[i] * samplerSobel;
          sobelY += sobelMatrixY[i] * samplerSobel;
      }
      for( int i = 5 ; i < 9 ; i++ ) {
          samplerSobel = texture(uniform_Update_texture_fs_Movie_frame , movieCoord + offsetsVideo[i]).rgb;
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
  vec3 pulsed_satur = vec3(1);
  if(video_satur_pulse > 0) {
    pulsed_satur = video_satur + video_satur_pulse * pulse;
  }
  //  public-domain function by Darel Rex Finley
  if(video_satur > 0 || video_satur_pulse > 0 ) {
    float  powerColor = sqrt( (videocolor.r)*(videocolor.r) * .299 +
                               (videocolor.g)*(videocolor.g) * .587 +
                               (videocolor.b)*(videocolor.b) * .114 ) ;
    videocolor.r = clamp( powerColor 
      + (videocolor.r - powerColor) * pulsed_satur.r , 0 , 1 );
    videocolor.g = clamp( powerColor 
      + (videocolor.g - powerColor) * pulsed_satur.g , 0 , 1 );
    videocolor.b = clamp( powerColor 
      + (videocolor.b - powerColor) * pulsed_satur.b , 0 , 1 );
  }

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // TRACK COLOR UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////

  //////////////////////////////////////
  //  FLASH BG - FLASH CA - FLASH PART
  vec3 flashToBGCumul = vec3(0.0);
  vec4 flashToCACumul = vec4(0.0);

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
        pathStroke = DRAWING_BEZIER;
      }
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
      // path drawing on current track
      else if( indPath > 0 && indPath < 4 && path_replay_trackNo03[indPath - 1] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }
#endif
#if PG_NB_PATHS == 7
      // path drawing on current track
      else if( indPath >= 4 && path_replay_trackNo47[indPath - 4] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }
#endif
      // drawing occurs
      if( pathStroke > 0 ) {
        if(indPath < 4) {
            BezierControl[0] =
              vec2(uniform_Update_fs_4fv_paths03_x_prev[indPath],uniform_Update_fs_4fv_paths03_y_prev[indPath]);
            BezierControl[1] =
              vec2(uniform_Update_fs_4fv_paths03_xL[indPath],uniform_Update_fs_4fv_paths03_yL[indPath]);
            BezierControl[2] =
              vec2(uniform_Update_fs_4fv_paths03_xR[indPath],uniform_Update_fs_4fv_paths03_yR[indPath]); 
            BezierControl[3] =
              vec2(uniform_Update_fs_4fv_paths03_x[indPath],uniform_Update_fs_4fv_paths03_y[indPath]);
            isBegin = (uniform_Update_fs_4iv_path03_beginOrEnd[indPath] > 0);
            isEnd = (uniform_Update_fs_4iv_path03_beginOrEnd[indPath] < 0);
            if(indPath == 0 && // rubber stylus
                               Cursor < 0) {
                curTrack_grayLevel =  out_gray_drawing( 
                    3 * uniform_Update_fs_4fv_paths03_RadiusX[indPath] ); // rubber radius is made 3 times larger than regular pen
                out_track_FBO[indCurTrack].rgb *= (1 - curTrack_grayLevel);
                curTrack_color.rgb = vec3(0);
            }
            else { // normal stylus
                curTrack_grayLevel =  out_gray_drawing( 
                    uniform_Update_fs_4fv_paths03_RadiusX[indPath] );
                curTrack_color.rgb
                  += curTrack_grayLevel
                    * uniform_Update_fs_4fv_paths03_a[indPath]
                    * vec3( uniform_Update_fs_4fv_paths03_r[indPath] , 
                                uniform_Update_fs_4fv_paths03_g[indPath] , 
                                uniform_Update_fs_4fv_paths03_b[indPath] );  // brush opacity is combined with color opacity
            }
        }
        else {
            int indPathRel = indPath - 4;
            BezierControl[0] =
              vec2(uniform_Update_fs_4fv_paths47_x_prev[indPathRel],uniform_Update_fs_4fv_paths47_y_prev[indPathRel]);
            BezierControl[1] =
              vec2(uniform_Update_fs_4fv_paths47_xL[indPathRel],uniform_Update_fs_4fv_paths47_yL[indPathRel]);
            BezierControl[2] =
              vec2(uniform_Update_fs_4fv_paths47_xR[indPathRel],uniform_Update_fs_4fv_paths47_yR[indPathRel]); 
            BezierControl[3] =
              vec2(uniform_Update_fs_4fv_paths47_x[indPathRel],uniform_Update_fs_4fv_paths47_y[indPathRel]);
            isBegin = (uniform_Update_fs_4iv_path47_beginOrEnd[indPathRel] > 0);
            isEnd = (uniform_Update_fs_4iv_path47_beginOrEnd[indPathRel] < 0);
            curTrack_grayLevel =  out_gray_drawing( 
                uniform_Update_fs_4fv_paths47_RadiusX[indPathRel] );
            curTrack_color.rgb
            += curTrack_grayLevel
                * uniform_Update_fs_4fv_paths47_a[indPathRel]
                * vec3( uniform_Update_fs_4fv_paths47_r[indPathRel] , 
                            uniform_Update_fs_4fv_paths47_g[indPathRel] , 
                            uniform_Update_fs_4fv_paths47_b[indPathRel] );  // brush opacity is combined with color opacity
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
    // TRACK video
    bool videoOn = false;
    if( currentVideoTrack == indCurTrack && cameraWeight + movieWeight > 0) {
      videoOn = true;
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

    /////////////////
    // TRACK photo
    if(currentPhotoTrack == indCurTrack 
      && photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x + photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y > 0 ) {
      // only photo (but not drawing or whatever memory from preceding tracks)
      if(!videoOn) {
       out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
      }
      // cumul video + photo
      else {
       out_track_FBO[indCurTrack].rgb
       = clamp( out_track_FBO[indCurTrack].rgb + photocolor , 0.0 , 1.0 );
      }
    }


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
    = texture( uniform_Update_texture_fs_Part_render , decalCoords );

  // particle flash on CA
  flashToCACumul += uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.z
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
    vec4 currentCA = out_attachment_FBO[pg_FBO_fs_CA_attacht]; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // will disappear through the updating rule of the automaton)
    if( flashToCACumul.a > 0 ) { // gray level of copy color (in case of flash)
      currentCA.a = -1;
      // currentCA.rgb = clamp( currentCA.rgb + out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 0.0 , 1.0 );
      currentCA.rgb = clamp( flashToCACumul.rgb , 0.0 , 1.0 );
      // currentCA.rgb = out_track_FBO[1].rgb;
      // currentCA.rgb = vec3(1,0,0);
    }

     // calculates the new state of the automaton
    CA_out( currentCA );


    // CA "REPOPULATION"
    if( repop_CA > 0 ) {
      if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.z), repop_CA) != 0) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz;
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_attachment_FBO[pg_FBO_fs_CA_attacht] = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_Color_flashCABGWght.w == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Update_fs_4fv_repop_Color_flashCABGWght.w 
            * out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 
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
    if( uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.y > 0 ) {
      pixel_radius = 1;
    }

    pixel_out();

    // arrival of a new pixel
    if( graylevel(out_color_pixel) > 0 ) {
      // outputs the color from pixel move at drawing layer
      out_track_FBO[0].rgb = out_color_pixel;
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht] 
	     = vec4( out_speed_pixel , out_position_pixel );
    }
    // update of the current pixel
    else {
      //  modifies speed according to acceleration
      vec2 pixel_acceleration;
      // FLAT
      // for the heaven's swirl, a specific texture for swirling the particles
      if(currentScene == 2) {
          vec2 acc = texture( uniform_Update_texture_fs_Trk1 , decalCoords ).gb 
                             - texture( uniform_Update_texture_fs_Trk1 , decalCoords + vec2(10)).gb;
          if(acc.x*acc.y != 0) {
            pixel_acceleration = acc / 2.0 + vec2(0.5);
          }
          else {
            pixel_acceleration = 1.5 * vec2(snoise( pixelTextureCoordinatesPOT_XY , noiseScale * 100 ),
                              snoise( pixelTextureCoordinatesPOT_XY + vec2(2.0937,9.4872) , noiseScale * 100 ));
          }
      }
/*      else if(currentScene >= 7) {
        vec2 acc = texture( uniform_Update_texture_fs_Trk2 , decalCoords ).rg 
                           - texture( uniform_Update_texture_fs_Trk2 , decalCoords + vec2(5,-5)).rg;
        if(acc.x*acc.y != 0) {
          pixel_acceleration = acc / 2.0 + vec2(0.5);
        }
        else {
          pixel_acceleration = randomCA.rg;
        }
      }
*/      // generative noise
      else {
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesPOT_XY , noiseScale * 100 ),
                              snoise( pixelTextureCoordinatesPOT_XY + vec2(2.0937,9.4872) , noiseScale * 100 ));
      }

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc > 0 ) {
      	// acceleration
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
          += pixel_acc * acceleration;
      }
      else {
      	// damping
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
          += pixel_acc * out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy;
      }
      // updates the position of the current pixel
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht].zw += out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy; 

      // if the pixel leaves the cell, it is erased
      if( abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].z ) > 0.5
      	  || abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].w ) > 0.5 ) {
      	if( pixel_mode == PIXEL_UNIQUE ) {
      	  out_track_FBO[0].rgb = vec3(0);
      	}
      }
    }
  }

  // pixel "ADDITION"
  if( repop_BG > 0 ) {
    if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.w), repop_BG) != 0) {
        out_track_FBO[0].rgb = uniform_Update_fs_4fv_repop_Color_flashCABGWght.xyz;
    }
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
          out_track_FBO[indTrack].rgb 
               = out_track_FBO[indTrack].rgb - vec3(trkDecay[indTrack]);
      }
      out_track_FBO[indTrack].rgb 
        = clamp( out_track_FBO[indTrack].rgb , 0.0 , 1.0 );
      // clear layer
      if( currentDrawingTrack == indTrack 
          && uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.x > 0 ) {
        out_track_FBO[indTrack].rgb = vec3(0);
      }
  }

  //////////////////////////////////////////////
  // resets pixel speed and position if trackO returns to black
  if( graylevel(out_track_FBO[0].rgb) <= 0 ) {
    out_track_FBO[0].rgb = vec3(0);
    out_attachment_FBO[pg_FBO_fs_Pixels_attacht] = vec4(0);
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
  if( uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift.y > 0 ) {
      out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(0);
  }

  //////////////////////////////////////////////
  //////////////////////////////////////////////
  // FBO OUTPUTS 
  //////////////////////////////////////////////
  //////////////////////////////////////////////
  out_Update_FBO_fs_CA = out_attachment_FBO[pg_FBO_fs_CA_attacht];
  out_Update_FBO_fs_Pixels = out_attachment_FBO[pg_FBO_fs_Pixels_attacht];
  out_Update_FBO_fs_Trk0 = out_track_FBO[0];
#if PG_NB_TRACKS >= 2
  out_Update_FBO_fs_Trk1 = out_track_FBO[1];
#endif
#if PG_NB_TRACKS >= 3
  out_Update_FBO_fs_Trk2 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  out_Update_FBO_fs_Trk3 = out_track_FBO[3];
#endif
}
