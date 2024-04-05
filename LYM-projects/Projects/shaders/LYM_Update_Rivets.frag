/***********************************************************************
File: song/shaders/LYM_Update_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#include_declarations

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;

#define SPLAT_PARTICLES
#define PG_BEZIER_PATHS

////////////////////////////////////////////////////////////////////
// TRACK CONST
#define PG_NB_TRACKS 4
#define PG_NB_PATHS 11

// path data array structure
#define PG_PATH_P_X              0
#define PG_PATH_P_Y              1
#define PG_PATH_BOX              2
#define PG_PATH_COLOR            3
#define PG_PATH_RADIUS_BEGINEND  4
#define PG_MAX_PATH_DATA         5

///////////////////////////////////////////////////////////////////
const uint pg_FBO_fs_CA_attacht = 0;
const uint pg_FBO_fs_Pixels_attacht = 1;

////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA
// CA types
#define CA_TOTALISTIC                    0
#define CA_TOTALISTIC_NB_SUBTYPES        4
#define CA_TOTALISTIC_TABLE_OFFSET       0
#define CA_GENERATION                    1
#define CA_GENERATION_NB_SUBTYPES        4
#define CA_GENERATION_TABLE_OFFSET       (CA_TOTALISTIC_TABLE_OFFSET + CA_TOTALISTIC_NB_SUBTYPES)
#define CA_GAL_BIN_MOORE                 2
#define CA_GAL_BIN_MOORE_NB_SUBTYPES     9
#define CA_GAL_BIN_MOORE_TABLE_OFFSET    (CA_GENERATION_TABLE_OFFSET + CA_GENERATION_NB_SUBTYPES)
#define CA_GAL_BIN_NEUMANN               3
#define CA_GAL_BIN_NEUMANN_NB_SUBTYPES   4
#define CA_GAL_BIN_NEUMANN_TABLE_OFFSET  (CA_GAL_BIN_MOORE_TABLE_OFFSET + CA_GAL_BIN_MOORE_NB_SUBTYPES)
#define CA_NEUMANN_BIN                   4
#define CA_NEUMANN_BIN_NB_SUBTYPES       10
#define CA_NEUMANN_BIN_TABLE_OFFSET      (CA_GAL_BIN_NEUMANN_TABLE_OFFSET + CA_GAL_BIN_NEUMANN_NB_SUBTYPES)

// CA OFFSETS
const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
                                 {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW
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

const int nbStatesTotalistic[CA_TOTALISTIC_NB_SUBTYPES] ={1, 16, 16, 16};
const int nbStatesGeneration[CA_GENERATION_NB_SUBTYPES] ={1,  4,  4, 25};

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
vec4 BezierControlX;
vec4 BezierControlY;
vec4 BezierBox;
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
vec3 cameraOriginal = vec3( 0.0 );
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
vec4 noiseCA = vec4(0);

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiamond[4]=vec4[4](vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),
                                    vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec2 pixelTextureCoordinatesPOT_XY; // the POT coordinates of the
// pixel texture + z offset according to the chosen texture
vec2 noisepixels;

vec4 randomCA;
vec4 randomCA2;
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
// pen Bezier curve control points
uniform vec4 uniform_Update_path_data[PG_MAX_PATH_DATA * (PG_NB_PATHS + 1)];

uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  
uniform vec4 uniform_Update_fs_4fv_flashTrkCAWghts;  

uniform vec4 uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow;
uniform vec3 uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_W_H_time_currentScene;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght;
uniform vec3 uniform_Update_fs_3fv_repop_ColorCA;
uniform vec3 uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
uniform vec4 uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_photo01Wghts_randomValues;
uniform vec3 uniform_Update_fs_3fv_photo_rgb; 
uniform vec2 uniform_Update_fs_2fv_clip01Wghts;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;
uniform vec4 uniform_Update_fs_4fv_CAseed_type_size_loc;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Update_texture_fs_CA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect   uniform_Update_texture_fs_PreviousCA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 2) uniform samplerRect   uniform_Update_texture_fs_Pixels;        // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
layout (binding = 3) uniform sampler3D     uniform_Update_texture_fs_Brushes;       // pen patterns
layout (binding = 4) uniform samplerRect   uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 5) uniform samplerRect   uniform_Update_texture_fs_Camera_BG;     // camera BG texture
layout (binding = 6) uniform samplerRect   uniform_Update_texture_fs_Movie_frame;   // movie textures
layout (binding = 7) uniform sampler3D     uniform_Update_texture_fs_Noise;         // noise texture
layout (binding = 8)  uniform samplerRect  uniform_Update_texture_fs_RepopDensity;  // repop density texture
layout (binding = 9)  uniform sampler2D    uniform_Update_texture_fs_Photo0;        // photo_0 texture or first clip left
layout (binding = 10)  uniform sampler2D   uniform_Update_texture_fs_Photo1;        // photo_1 texture or first clip right
layout (binding = 11)  uniform sampler2D   uniform_Update_texture_fs_Clip0;         // second clip left texture
layout (binding = 12)  uniform sampler2D   uniform_Update_texture_fs_Clip1;         // second clip right texture
layout (binding = 13)  uniform samplerRect uniform_Update_texture_fs_Part_render;   // FBO capture of particle rendering
layout (binding = 14) uniform samplerRect  uniform_Update_texture_fs_Trk0;          // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 15) uniform samplerRect  uniform_Update_texture_fs_Trk1;          // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 16) uniform samplerRect  uniform_Update_texture_fs_Trk2;          // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 17) uniform samplerRect  uniform_Update_texture_fs_Trk3;          // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
layout (binding = 18) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA
layout (binding = 19) uniform samplerRect  uniform_Update_texture_fs_Camera_BGIni; // initial background camera texture
layout (binding = 20) uniform samplerRect  uniform_Update_texture_fs_pixel_acc;     // image for pixel acceleration

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
float snoise(vec2 v , float noiseUpdateScale) {
  v *= noiseUpdateScale;

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
return vec2(snoise( texCoordLoc , noiseUpdateScale * 100 ),
                        snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));
}

vec2 multiTypeGenerativeNoise(vec2 texCoordLoc, vec2 usedNeighborOffset) {
#if defined(var_noiseType) && defined(var_noiseCenterX) && defined(var_noiseCenterY) && defined(var_noiseLineScale) && defined(var_noiseAngleScale)
  // FLAT
  if(noiseType == 0 )  {
    return vec2(snoise( texCoordLoc , noiseUpdateScale * 100 ),
                            snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));
  }
  // SUN RAYS
  else if(noiseType == 1 ) {
    vec2 pos = vec2( atan((noiseCenterX-texCoordLoc.x)/(noiseCenterY-texCoordLoc.y)) * (noiseAngleScale * 10),
                     length(vec2(noiseCenterX,noiseCenterY) - texCoordLoc) / (noiseLineScale) );
    return vec2(snoise( pos , noiseUpdateScale * 10 ) ,
                            snoise( pos + vec2(2.0937,9.4872) , noiseUpdateScale * 10 ));
  }
  // CAMERA
  else if(noiseType == 2 ) {
    return texture(uniform_Update_texture_fs_Camera_frame, decalCoords + usedNeighborOffset ).rg;
  }
  // MOVIE
  else {
    return texture(uniform_Update_texture_fs_Movie_frame, movieCoord + usedNeighborOffset/ movieWH ).rg;
  }
#else
  return vec2(snoise( texCoordLoc , noiseUpdateScale * 100 ),
                          snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));
#endif
}

// random noise
// https://www.ronja-tutorials.com/2018/09/02/white-noise.html
// after (c) Ronja Böhringer
//get a scalar random value from a 3d value
// 2-step computation due to lack of precision
int rand3D(vec3 value, float threshold){
    if(threshold <= 0) {
      return 0;
    }
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
  // the ALPHA canal of uniform_Update_texture_fs_CA contains > 0 if it is a live cell

  noiseCA = texture( uniform_Update_texture_fs_Noise, 
                          vec3( pixelTextureCoordinatesXY , 
                                6.5 / 7.0 ) );

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
  // GAME OF LIFE - TOTALISTIC OR GENERATION
  if(CAType == CA_TOTALISTIC || CAType == CA_GENERATION ) {
    int nbSurroundingLives =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);
    
    vec4 gatherSurroundingLives =
      (neighborValues[0]) +
      (neighborValues[1]) +
      (neighborValues[2]) +
      (neighborValues[3]) +
      (neighborValues[4]) +
      (neighborValues[5]) +
      (neighborValues[6]) +
      (neighborValues[7]);
    
    vec4 averageSurrounding;
    if( nbSurroundingLives > 0 ) {
      averageSurrounding = gatherSurroundingLives / nbSurroundingLives;
    }
    else {
      averageSurrounding = vec4(0.0);
    }
    
    // The first CA alpha value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed 
    // then alpha represents the state of the automaton
    if( currentCA.a < 0 ) {
      /* out4_CA.a = noiseCA.r * (nbStates+1); // nbStates states randomly */
      // out4_CA.rgb = averageSurrounding.rgb;
      
      // special treatment for generation, otherwise
      // it does not start from flashCA
      if( CAType == CA_TOTALISTIC ) { 
        int nbStates = nbStatesTotalistic[CASubType];
        float newState = clamp(noiseCA.r * (nbStates+1),1,nbStates); // nbStates states randomly
        out4_CA.a = newState;
        out4_CA.rgb = currentCA.rgb;
        out4_CA.rgb = averageSurrounding.rgb;
      }
      else if( CAType == CA_GENERATION ) { 
        int nbStates = nbStatesGeneration[CASubType];
        float newState = clamp(noiseCA.r * (nbStates+1),1,nbStates); // nbStates states randomly
        newState = 1;
        out4_CA.a = newState;
        out4_CA.rgb = currentCA.rgb;
        out4_CA.rgb = averageSurrounding.rgb;
      }
    }
    else {
      if( CAType == CA_TOTALISTIC ) {
        int nbStates = nbStatesTotalistic[CASubType];
        float state = clamp(currentCA.a,0,nbStates);
        float newState = 0;
        newState = texelFetch(uniform_Update_texture_fs_CATable, 
                         ivec2( int(state) * 10 + nbSurroundingLives + 1 , 
                                CA_TOTALISTIC_TABLE_OFFSET + CASubType ) ).r;
        out4_CA.a = float(newState);
        if( newState > 0 ) {
          out4_CA.rgb = currentCA.rgb;
          out4_CA.rgb = averageSurrounding.rgb;
        }
        else {
          out4_CA.rgb = vec3(0,0,0);
        }
      }
      else if( CAType == CA_GENERATION ) {
        int nbStates = nbStatesGeneration[CASubType];
        float state = clamp(currentCA.a,0,nbStates);
        float newState = 0;
        newState = texelFetch(uniform_Update_texture_fs_CATable, 
                         ivec2( int(state) * 10 + nbSurroundingLives + 1 , 
                                CA_GENERATION_TABLE_OFFSET + CASubType ) ).r;
        out4_CA.a = float(newState);
        if( newState > 0 ) {
          out4_CA.rgb = currentCA.rgb;
          out4_CA.rgb = averageSurrounding.rgb;
        }
        else {
          out4_CA.rgb = vec3(0,0,0);
        }
      }
    }
  }

  ////////////////////////////////////////////////////// 
  // GAL BIN MOORE NEIGHBORHOOD
  else if( CAType == CA_GAL_BIN_MOORE ) {
    // Fallski
    // C48,NM,Sb255a,Babb189ab63a
    // 48 states 0-47
    // Moore neihborhood Order N,NE,E,SE,S,SW,W,NW
    // states are encoded: N + 2 * NE + 4 * E + 8 * SE + 16 * S + 32 * SW + 64 * W + 128 * NW
    // 00000000 0 neighbor
    // 10000000 N neighbor
    // 01000000 NE neighbor
    // 192 = 00000011 W and NW neighbors
    // Survive b255a survival on no alive neighbors: 
    //                             1 x one    255 x zeros
    // Birth   abb189ab63a birth on a single N or  NE neighbor, or on W and NW neighbors:
    //                             0 1 1   189 x zeros   1   63 x zeros
    // Encoding of Survival and Birth
    // 256 0/1 digits encode 

    // const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
    //                                  {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW

    uint nbSurroundingLives =
      (neighborValues[0].a > 0?   4:0) +  // E
      (neighborValues[1].a > 0?  64:0) +  // W
      (neighborValues[2].a > 0?   1:0) +  // N
      (neighborValues[3].a > 0?  16:0) +  // S
      (neighborValues[4].a > 0?   2:0) +  // NE
      (neighborValues[5].a > 0?  32:0) +  // SW
      (neighborValues[6].a > 0?   8:0) +  // SE
      (neighborValues[7].a > 0? 128:0);   // NW

    // uint CArank = 38;
    #define nbStatesGeneralBinary 21
    /*
    uint nbStates = texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( 0 , 38 + CASubType ) ).r;
    */

    // previous automata collected from the web
    if(CASubType <= 5) {
      // The first CA value is negative so that it is not 
      // displayed, here we change alpha value to positive
      // because it is the second time it is displayed if 
      float state = clamp(currentCA.a,0,nbStatesGeneralBinary);
      float newState = 0;
      if( currentCA.a < 0 ) {
        newState = floor(noiseCA.r * (nbStatesGeneralBinary+1)); // nbStates states randomly
       //  out4_CA.a = nbStatesGeneralBinary;
        out4_CA.a = newState;
      }
      else {
        // survival
        if( state != 0 ) {
           newState = texelFetch(uniform_Update_texture_fs_CATable, 
                                 ivec2( nbSurroundingLives + 1 , 
                                        CA_GAL_BIN_MOORE_TABLE_OFFSET + CASubType ) ).r;
           // survival
           if( newState != 0 && out4_CA.a >= 0.0 ) {
              // out4_CA.a -= 1.0;
           }
           else {
              out4_CA.a = 0;
           }
        }
        // birth
        else {
          newState = texelFetch(uniform_Update_texture_fs_CATable, 
                         ivec2( 256 + nbSurroundingLives + 1 , 
                                CA_GAL_BIN_MOORE_TABLE_OFFSET + CASubType ) ).r;
          // birth
          if( newState != 0 ) {
            out4_CA.a = nbStatesGeneralBinary;
          }
        }
      }
    }
    // personal automata
    else {
      // The first CA value is negative so that it is not 
      // displayed, here we change alpha value to positive
      // because it is the second time it is displayed if 
      float state = clamp(currentCA.a,0,1);
      float newState = 0;
      if( currentCA.a < 0 ) {
        out4_CA.a = 1;
      }
      else {
        // survival
        if( state > 0 ) {
           newState = texelFetch(uniform_Update_texture_fs_CATable, 
                                 ivec2( nbSurroundingLives + 1 , 
                                        CA_GAL_BIN_MOORE_TABLE_OFFSET + CASubType ) ).r * 255;
           // survival
           if( newState != 0) {
              out4_CA.a = currentCA.a;
           }
           // ageing
           else {
              out4_CA.a = max(0,round(currentCA.a - 1));
           }
        }
        // birth
        else {
          newState = texelFetch(uniform_Update_texture_fs_CATable, 
                         ivec2( 256 + nbSurroundingLives + 1 , 
                                CA_GAL_BIN_MOORE_TABLE_OFFSET + CASubType ) ).r * 255;
          // birth
          if( newState != 0 ) {
            out4_CA.a = round(newState);
          }
          // nothing (stays dead)
          else {
              out4_CA.a = 0;
           }
        }
      }
    }
    if( out4_CA.a != 0 ) {
      out4_CA.rgb = currentCA.rgb;
    }
    else {
      out4_CA.rgb = vec3(0);
    }
  }

  ////////////////////////////////////////////////////// 
  // GAL BIN VON NEUMANN NEIGHBORHOOD
  else if( CAType == CA_GAL_BIN_NEUMANN ) {
    // Banks,
    // C0,NN,S3babbabbabba3b,B7ab3aba3b
    // 2 states 0-1
    // von Neumann neihborhood Order N,E,S,W
    // states are encoded: N + 2 * E + 4 * S + 8 * W
    // 0000 0 neighbor
    // 1000 N neighbor
    // 0100 E neighbor
    // 3 = 1100 N and E neighbors
    // Survive 3babbabbabba3b survival on 
    //         1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,
    // Birth   7ab3aba3b birth on 
    //         0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,
    // Encoding of Survival and Birth
    // 16 0/1 digits encode 

    // const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
    //                                  {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW

    int nbSurroundingLives =
      (neighborValues[0].a > 0?   2:0) +  // E
      (neighborValues[1].a > 0?   8:0) +  // W
      (neighborValues[2].a > 0?   1:0) +  // N
      (neighborValues[3].a > 0?   4:0) ;  // S

    // uint CArank = 38;
    uint nbStates = uint(texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( 0 , 
                                      CA_GAL_BIN_NEUMANN_TABLE_OFFSET + CASubType ) ).r * 255);

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    uint state = int(clamp(currentCA.a,0,nbStates));
    float newState = 0;
    if( currentCA.a < 0 ) {
      // newState = int(noiseCA.r * (nbStates+1)); // nbStates states randomly
      out4_CA.a = float(nbStates);
    }
    else {
      // survival
      if( state > 0 ) {
         newState = texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( nbSurroundingLives + 1 , 
                                      CA_GAL_BIN_NEUMANN_TABLE_OFFSET + CASubType ) ).r;
         // survival
         if( newState > 0 && out4_CA.a >= 0.0 ) {
            out4_CA.a -= 1.0;
         }
      }
      // birth
      else {
        newState = texelFetch(uniform_Update_texture_fs_CATable, 
                       ivec2( 16 + nbSurroundingLives + 1 , 
                              CA_GAL_BIN_NEUMANN_TABLE_OFFSET + CASubType ) ).r;
        // birth
        if( newState > 0 ) {
          out4_CA.a = float(nbStates);
        }
      }
    }

    if( out4_CA.a > 0 ) {
      out4_CA.rgb = vec3(out4_CA.a/float(nbStates-1));
    }
    else {
      out4_CA.rgb = vec3(0,0,0);
    }
  }

  ////////////////////////////////////////////////////// 
  // NEUMANN BIN VON NEUMANN NEIGHBORHOOD + CENTER
  else if( CAType == CA_NEUMANN_BIN ) {
    // Fredkin2 rule has the following definition: 2,01101001100101101001011001101001
    // The first digit, '2', tells the rule has 2 states (it's a 1 bit rule).
    // The second digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=0 will get the state 0.
    // The third digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=1 will get the state 1.
    // The fourth digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=0 will get the state 1.
    // The fifth digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=1 will get the state 0.
    // . . .

    // const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S

    int nbSurroundingLives =
      (int(currentCA.a)) +  // CENTER
      (int(neighborValues[0].a) * 9) +  // E
      (int(neighborValues[1].a) * 81) +  // W
      (int(neighborValues[2].a) * 3) +  // N
      (int(neighborValues[3].a) * 27) ;  // S

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    float state = clamp(currentCA.a,0,3);
    float newState = 0;
    if( currentCA.a < 0 ) {
      newState = clamp(noiseCA.r * (3+1),0,3); // nbStates states randomly
      // newState = 2;
      out4_CA.a = float(newState);
    }
    else {
      newState = texelFetch(uniform_Update_texture_fs_CATable, 
                    ivec2( nbSurroundingLives + 1 , 
                            CA_NEUMANN_BIN_TABLE_OFFSET + CASubType ) ).r * 255.0;
      // newState = texelFetch(uniform_Update_texture_fs_CATable, 
      //                       ivec2( nbSurroundingLives + 1 , 58 ) ).r;
      out4_CA.a = float(newState);
    }

    if( out4_CA.a > 0 ) {
      vec3 averageSurrounding = vec3(out4_CA.a/float(3-1)); //gatherSurroundingLives* 3 / nbSurroundingLives;
      out4_CA.rgb = currentCA.rgb;
    }
    else {
      out4_CA.rgb = vec3(0);
    }
    // out4_CA = vec4(1.0,0.0,0.0,1.0);
  }

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = currentCA; // ready
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
      // the positions are calculated in the coordinates centered at the current pixel
      if( graylevel(surrpixel_localColor) >  0 ) {
        vec2 surrpixel_speed;
        vec2 surrpixel_position;
        vec2 surrpixel_nextPosition;

        vec4 surrpixel_speed_position = texture( uniform_Update_texture_fs_Pixels, newDecalCoord );
        surrpixel_speed = surrpixel_speed_position.xy;
        surrpixel_position = surrpixel_speed_position.zw;
        vec2 pixelTexCoordLocPOT = pixelTextureCoordinatesPOT_XY
                       + usedNeighborOffset / vec2(width,height);
        vec2 acceleration;
#ifdef var_pixel_image_acceleration
        // texture based acceleration shift
        if(pixel_image_acceleration >= 0 && decalCoords.x < width - 2 && decalCoords.y < height - 2) {
          float grey_center = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords ).rgb);
          float greyX = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(1,0)).rgb);
          float greyY = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(0,1)).rgb);
          float gradX = greyX - grey_center;
          float gradY = greyY - grey_center;
          float norm = length(vec2(gradX, gradY));
          if(norm != 0) {
            float cosa = gradX / norm;
            float sina = gradY / norm;
            acceleration = vec2(dmat2(cosa, -sina, sina, cosa) * vec2(1,1));
          }
          else {          
            acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT, usedNeighborOffset) - pixel_acc_center;
          }
        }
        else 
#endif
        {          
          acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT, usedNeighborOffset) - pixel_acc_center;
        }

        if( pixel_acc > 0 ) {
          // acceleration
          surrpixel_speed += pixel_acc * acceleration;
        }
        else {
          // damping
          surrpixel_speed += pixel_acc * surrpixel_speed;
        }

        // the current step is added to the position
        surrpixel_nextPosition 
                 = usedNeighborOffset + surrpixel_position + surrpixel_speed; 

        // if( abs(surrpixel_nextPosition.x) <= 0.5 
        //           && abs(surrpixel_nextPosition.y) <= 0.5 ) {
        if( length(surrpixel_nextPosition) <= 0.5 ) {
          out_color_pixel += surrpixel_localColor.rgb;
          out_speed_pixel += surrpixel_speed;
          // computes the position of the pixel
          out_position_pixel += surrpixel_nextPosition;
          nb_cumultated_pixels++;
        }
        // radius pixel extension for (S,N,E,W) neighbors
        else if( abs(surrpixel_nextPosition.x) <= (pixel_radius - (randomCA.z - 0.5))
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius - (randomCA.w - 0.5)) ) {
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
          = texture( uniform_Update_texture_fs_Pixels, newDecalCoord );
              surrpixel_speed = surrpixel_speed_position.xy;
              surrpixel_position = surrpixel_speed_position.zw;

        vec2 pixelTexCoordLocPOT = pixelTextureCoordinatesPOT_XY
                       + usedNeighborOffset / vec2(width,height);
        vec2 acceleration;

#ifdef var_pixel_image_acceleration
        // texture based acceleration shift
        if(pixel_image_acceleration >= 0 && decalCoords.x < width - 2 && decalCoords.y < height - 2) {
          float grey_center = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords ).rgb);
          float greyX = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(1,0)).rgb);
          float greyY = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(0,1)).rgb);
          float gradX = greyX - grey_center;
          float gradY = greyY - grey_center;
          float norm = length(vec2(gradX, gradY));
          if(norm != 0) {
            float cosa = gradX / norm;
            float sina = gradY / norm;
            acceleration = vec2(dmat2(cosa, -sina, sina, cosa) * vec2(1,1));
          }
          else {          
            acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT, usedNeighborOffset) - pixel_acc_center;
          }
        }
        else
#endif
        {          
          acceleration = multiTypeGenerativeNoise(pixelTexCoordLocPOT, usedNeighborOffset) - pixel_acc_center;
        }

        if( pixel_acc > 0 ) {
          // acceleration
          surrpixel_speed += pixel_acc * acceleration;
        }
        else if( pixel_acc < 0 && pixel_acc > -1) {
          // damping
          surrpixel_speed *= (1 + pixel_acc);
        }
        else { // if( pixel_acc == 0 || pixel_acc <= -1)
          // stops
          surrpixel_speed = vec2(0);
        }
        // the current step is added to the position
        surrpixel_nextPosition 
                 = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
        if( abs(surrpixel_nextPosition.x) <= (pixel_radius - (randomCA.z - 0.5))
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius - (randomCA.w - 0.5)) ) {
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
    float out_color_pixel_max_col = maxCol(out_color_pixel);
    if(out_color_pixel_max_col > 1) {
      out_color_pixel -= vec3((out_color_pixel_max_col) - 1);
    out_color_pixel = clamp( out_color_pixel , 0.0 , 1.0 );
  }
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

float stroke_out( float current_Brush_Radius , int current_brushID ) {

  // drawing mode: writes under the cursor
  // tries to avoid artefacts at the junction between two segments
  // vector from the pen center to the current position
  vec2 vector_to_mousePosition = vec2(signed_rx,signed_ry);
  vec2 normalized_vector_to_mousePosition 
    = vector_to_mousePosition/vec2(current_Brush_Radius);

  // it is blended with preceding pen color at this position
  // gray level 
  // horizontal texture coordinate shift according to pen number
  float penOutGrayLevel = 0.0;
  if( current_brushID > 0 ) {
    penOutGrayLevel
      = texture(uniform_Update_texture_fs_Brushes,
                vec3( normalized_vector_to_mousePosition.x/2.0+0.5,
                      0.5 - normalized_vector_to_mousePosition.y/2.0,
                      (current_brushID / 3) * 0.33333 + 0.16667))[current_brushID % 3];
  }

  // pen gray level
  return penOutGrayLevel;
}

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
float out_gray_drawing( float current_Brush_Radius, int current_brushID ) {

  /////////////////////////////////////////////////
  // point in screen coordinates
  vec2 PixelLocation = vec2(decalCoords.x , height - decalCoords.y);

  /////////////////////////////////////////////////
  // just a point
  if(isBegin && isEnd) {
      /////////////////////////////////////////////////
      // WRITING POSITION
      // distance from pen center
      signed_rx = PixelLocation.x - BezierControl[3].x;
      signed_ry = PixelLocation.y - BezierControl[3].y;
      float distanceToCurve = length( vec2(signed_rx , signed_ry) );

      if( // doesnt redraw on previously drawn place
          distanceToCurve < current_Brush_Radius ) {
        if(current_brushID > 0) {
            // reads the pen texture at this position
            return stroke_out( current_Brush_Radius , current_brushID );
        }
        else {
        // reads the gray level of the brush at this position
        return 1.0f - distanceToCurve / current_Brush_Radius;
        }
      }
  }

  /////////////////////////////////////////////////
  // The cubic Bezier curve hasfour control points BezierControl
  // from the previous mouse position BezierControl[0].x BezierControl[0].y 
  // to the current one BezierControl[3].x BezierControl[3].y 

  /////////////////////////////////////////////////
  // a segment made of 2 distinct real points 
  if( BezierControlX[0] >= 0 && BezierControlY[0] >= 0 
      && BezierControlX[3] >= 0 && BezierControlY[3] >= 0
      && (BezierControlX[0] != BezierControlX[3]
          || BezierControlY[0] != BezierControlY[3]) ) {

    // Bezier curve based rendering

    // BOUNDING SOLUTION
    // simple bounding box around the stroke
/*    if( (PixelLocation.x <= BezierBox.x) || (PixelLocation.x >= BezierBox.y)
       || (PixelLocation.y <= BezierBox.z) || (PixelLocation.y >= BezierBox.w) ) {
      return  0.f; 
    }
*/
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
      signed_rx = PixelLocation.x - BezierControlX[3];
      signed_ry = PixelLocation.y - BezierControlY[3];
      float distanceToCurve = length( vec2(signed_rx , signed_ry) );

      if( // doesnt redraw on previously drawn place
          distanceToCurve < current_Brush_Radius ) {
        if(current_brushID > 0) {
            // reads the pen texture at this position
            return stroke_out( current_Brush_Radius , current_brushID );
        }
        else {
        // reads the gray level of the brush at this position
        return 1.0f - distanceToCurve / current_Brush_Radius;
        }
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
          if(current_brushID > 0) {
              // reaads the pen texture at this position
              return stroke_out( current_Brush_Radius , current_brushID );
          }
          else {
          // reads the gray level of the brush at this position
          return 1.0f - distanceToCurve / current_Brush_Radius;
          }
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
      signed_rx = PixelLocation.x - BezierControlX[0];
      signed_ry = PixelLocation.y - BezierControlY[0];
      float distanceToCurve = length( vec2(signed_rx , signed_ry) );

      if( // doesnt redraw on previously drawn place
          distanceToCurve < current_Brush_Radius ) {
        if(current_brushID > 0) {
            // reads the pen texture at this position
            return stroke_out( current_Brush_Radius , current_brushID );
        }
        else {
        // reads the gray level of the brush at this position
        return 1.0f - distanceToCurve / current_Brush_Radius;
        }
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
  // frame number
  frameNo = int(round(uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow.x));

 // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow.y;

  // pixels position speed update parameters
  pixel_acc_center = vec2(pixel_acc_shiftX,pixel_acc_shiftY);
  

  // working variables for screen dimension
  width = uniform_Update_fs_4fv_W_H_time_currentScene.x;
  height = uniform_Update_fs_4fv_W_H_time_currentScene.y;
  int currentScene = int(uniform_Update_fs_4fv_W_H_time_currentScene.w);

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
  vec2 noisePositionOffset = vec2(snoise( position , noiseUpdateScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  pixelTextureCoordinatesPOT_XY = decalCoordsPOT 
                  + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesPOT_XY , 0.0 ) );
  randomCA2 = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesPOT_XY , 0.5 ) );
  // CA or BG "REPOPULATION"
  if( BG_CA_repop_density >= 0 && (repop_CA > 0 || repop_BG > 0)) {
    textureDensityValue = texture(uniform_Update_texture_fs_RepopDensity,decalCoords).rgb;
    repop_density_weight = graylevel(textureDensityValue);
  }
  else {
    repop_density_weight = 1;
  }

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
  // pixel speed and local position inside the current pixel
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
  }
#endif

  // track colors FBO copy
#if PG_NB_TRACKS >= 3
  out_track_FBO[2] 
    = texture( uniform_Update_texture_fs_Trk2 , decalCoords );
#endif
#if PG_NB_TRACKS >= 4
  out_track_FBO[3] 
    = texture( uniform_Update_texture_fs_Trk3 , decalCoords );
#endif

  // if freeze, just keep values as they are
  if( freeze ) {
    // FBO OUTPUTS
    out_Update_FBO_fs_CA = out_attachment_FBO[pg_FBO_fs_CA_attacht];
    // pixel speed and local position inside the current pixel
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
  vec3 photoOriginal = vec3( 0.0 );
  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x > 0) {
    vec2 coordsImageScaled = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) / photo_scale 
                             + vec2(0.5) * (photo_scale - 1) / photo_scale;
    coordsImageScaled += vec2(photo_offsetX, photo_offsetY);
    coordsImageScaled = clamp(coordsImageScaled, 0.001, 0.999);
    photoOriginal = texture(uniform_Update_texture_fs_Photo0, coordsImageScaled  * uniform_Update_fs_4fv_photo01_wh.xy).rgb;
    photocolor += uniform_Update_fs_4fv_photo01Wghts_randomValues.x * photoOriginal;
  }
  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y > 0) {
    vec2 coordsImageScaled = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) / photo_scale 
                             + vec2(0.5) * (photo_scale - 1) / photo_scale;
    coordsImageScaled += vec2(photo_offsetX, photo_offsetY);
    coordsImageScaled = clamp(coordsImageScaled, 0.001, 0.999);
    photoOriginal = texture(uniform_Update_texture_fs_Photo1, coordsImageScaled  * uniform_Update_fs_4fv_photo01_wh.zw).rgb;
    photocolor += uniform_Update_fs_4fv_photo01Wghts_randomValues.y * photoOriginal;
  }
  photocolor *= photoWeight;

  vec3 videocolor = vec3( 0.0 );

  float flashCameraTrkWght = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.z;
  float flashCameraTrkThreshold = uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.z;
  float flashPhotoTrkWght = uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.x;
  float flashPhotoTrkThreshold = uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.y;
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
     cameraCoord = vec2(decalCoordsPOT.x, (1 - decalCoordsPOT.y) )
               * cameraWH;
 */
  cameraCoord = vec2((decalCoordsPOT.x), (1 - decalCoordsPOT.y) )
              // added for wide angle lens that covers more than the drawing surface
               * cameraWH; + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
#endif

  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  // image reading
  cameraOriginal = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
  cameraImage = cameraOriginal;
  // cameraOriginal = vec3(1) - cameraOriginal;
  // gamma correction
#ifdef var_camera_gamma
  if(camera_gamma != 1) {
    float gamma = max(camera_gamma, 0.01);
    cameraImage = vec3( pow(cameraImage.r,gamma) , pow(cameraImage.g,gamma) , pow(cameraImage.b,gamma) );
  }
#endif
#ifdef var_camera_BG_subtr
  if( camera_BG_subtr ) {
    cameraImage = abs(cameraImage - texture(uniform_Update_texture_fs_Camera_BG, cameraCoord ).rgb); // initial background subtraction
  }
#endif
#ifdef var_cameraThreshold
  if( graylevel(cameraImage) < cameraThreshold ) {
    cameraImage = vec3(0.0);
  }
#endif

  // Sobel on camera
#ifdef var_cameraSobel
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
#endif
  // color inversion
#ifdef var_invertCamera
  if( invertCamera ) {
      cameraImage = vec3(1) - cameraImage;
  }
#endif

  movieImage = texture(uniform_Update_texture_fs_Movie_frame, movieCoord ).rgb;
  
  // gamma correction
#if defined(var_photo_contrast)
  if(movie_gamma != 1) {
    float gamma = max(movie_gamma, 0.01);
    movieImage = vec3( pow(movieImage.r,gamma) , pow(movieImage.g,gamma) , pow(movieImage.b,gamma) );
  }
#endif

#if defined(var_movie_threshold)
  if( graylevel(movieImage) <= movie_threshold ) {
      movieImage = vec3(0.0);
  }
#endif

  // Sobel on movie
#ifdef var_movieSobel
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
#endif

#ifdef var_invertMovie
  // color inversion
  if( invertMovie ) {
      movieImage = vec3(1) - movieImage;
  }
#endif

  // video image = mix of movie and camera
  float videoWeight = 0;
#ifdef var_cameraWeight
  videocolor += cameraWeight * cameraImage;
  videoWeight += cameraWeight;
#endif
#ifdef var_movieWeight
  videocolor += movieWeight * movieImage;
  videoWeight += movieWeight;
#endif

  // videocolor = movieImage;
  // videoWeight = 1.;

  // video_satur
  //  public-domain function by Darel Rex Finley
#ifdef var_video_satur
  if(video_satur >= 0) {
    float  powerColor = sqrt( (videocolor.r)*(videocolor.r) * .299 +
                               (videocolor.g)*(videocolor.g) * .587 +
                               (videocolor.b)*(videocolor.b) * .114 ) ;
    videocolor = clamp( powerColor 
      + (videocolor - vec3(powerColor)) * video_satur , 0 , 1 );
  }
#endif

#ifdef var_video_gamma
  // video_gamma
  // Gamma correction on video (mainly blck and white)
  if(video_gamma != 1) 
  {
    float gamma = max(video_gamma, 0.01);
    videocolor.r = pow( videocolor.r, gamma);
    videocolor.g = pow( videocolor.g, gamma);
    videocolor.b = pow( videocolor.b, gamma);
  }
#endif

#ifdef var_video_threshold
  // image_threshold 
  if(video_threshold > 0) 
  {
    videocolor.r = (videocolor.r > video_threshold? videocolor.r : 0);
    videocolor.g = (videocolor.g > video_threshold? videocolor.g : 0);
    videocolor.b = (videocolor.b > video_threshold? videocolor.b : 0);
  }
#endif

#ifdef var_video_contrast
  // video contrast
  //  from http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/
  if(video_contrast > 0) {
    float correctionFactor =  1.016 * (video_contrast + 1.0) 
                                / (1.016 - video_contrast);
    videocolor = clamp( vec3(correctionFactor)
      * (videocolor - vec3(0.5)) + vec3(0.5) , 0 , 1 );
  }
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
  vec4 flashPhotoValue = vec4(0);

  /////////////////////////////////////
  // builds a path_replay_trackNo vector so that it can be used in the for loop
  #if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
  ivec3 path_replay_trackNo03 = ivec3(path_replay_trackNo_1,path_replay_trackNo_2,
                                      path_replay_trackNo_3);
  #endif
  #if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
  ivec4 path_replay_trackNo47 = ivec4(path_replay_trackNo_4,path_replay_trackNo_5,
                                      path_replay_trackNo_6,path_replay_trackNo_7);
  #endif
  #if PG_NB_PATHS == 11
  ivec4 path_replay_trackNo811 = ivec4(path_replay_trackNo_8,path_replay_trackNo_9,
                                      path_replay_trackNo_10,path_replay_trackNo_11);
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
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
      // path drawing on current track
      else if( indPath > 0 && indPath < 4 && path_replay_trackNo03[indPath - 1] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
      // path drawing on current track
      else if( indPath >= 4 && path_replay_trackNo47[indPath - 4] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }
#endif
#if PG_NB_PATHS == 11
      // path drawing on current track
      else if( indPath >= 8 && path_replay_trackNo811[indPath - 8] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }
#endif
      // drawing occurs
      if( pathStroke > 0 ) {
        BezierControlX = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_P_X];
        BezierControlY = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_P_Y];
        BezierControl[0] = vec2(BezierControlX[0], BezierControlY[0]);
        BezierControl[1] = vec2(BezierControlX[1], BezierControlY[1]);
        BezierControl[2] = vec2(BezierControlX[2], BezierControlY[2]);
        BezierControl[3] = vec2(BezierControlX[3], BezierControlY[3]);
        BezierBox = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_BOX];
        vec4 radiusX_beginOrEnd_radiusY_brushID = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_RADIUS_BEGINEND];
        vec4 pathColor = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_COLOR];
        float isBeginOrEnd = radiusX_beginOrEnd_radiusY_brushID.y;
        int current_brushID = int(radiusX_beginOrEnd_radiusY_brushID.w);
        isBegin = (isBeginOrEnd > 0);
        isEnd = (isBeginOrEnd < 0);
        if(indPath == 0 && // rubber stylus
                           Cursor < 0) {
            curTrack_grayLevel =  out_gray_drawing( 3 * radiusX_beginOrEnd_radiusY_brushID.x, 0 ); 
                                 // rubber radius is made 3 times larger than regular pen
            out_track_FBO[indCurTrack].rgb *= (1 - curTrack_grayLevel * pathColor.a);
            curTrack_color.rgb = vec3(0);
        }
        else { // normal stylus
            curTrack_grayLevel =  out_gray_drawing( radiusX_beginOrEnd_radiusY_brushID.x, current_brushID );
            curTrack_color.rgb
              += curTrack_grayLevel
                * pathColor.a
                * pathColor.rgb;  // brush opacity is combined with color opacity
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
    if( currentVideoTrack == indCurTrack) {
      if( videoWeight > 0) {
      videoOn = true;
#ifdef var_cameraCumul
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
#else
        out_track_FBO[indCurTrack] 
          = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
#endif
    }
      if( flashCameraTrkWght > 0 
        && graylevel(cameraOriginal) > flashCameraTrkWght ) { // flash camera
        // video image copy when there is a flash video
        flashToCACumul.rgb = cameraImage;
        flashToBGCumul.rgb = cameraImage;
      flashToCACumul.a = 1;
    }
    }

    /////////////////
    // TRACK photo
    if(currentPhotoTrack == indCurTrack) {
#if defined(var_photoWeight)
      // visible photo
      if(photoWeight * (uniform_Update_fs_4fv_photo01Wghts_randomValues.x 
        + uniform_Update_fs_4fv_photo01Wghts_randomValues.y 
        + uniform_Update_fs_2fv_clip01Wghts.x 
        + uniform_Update_fs_2fv_clip01Wghts.y) > 0 ) {
        // photo + video
        if(currentVideoTrack == indCurTrack) {
          // only photo no video
      if(!videoOn) {
        out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
      }
      // cumul video + photo
      else {
            out_track_FBO[indCurTrack].rgb = clamp( videocolor + photocolor , 0.0 , 1.0 );
          }
        }
        // only photo
        else {
            out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
      }
    }
      // no photo
      // this has been commented because if there is no photo and no video on the
      // same track as drawing, the drawing is erased
      // else {
      //   if(currentVideoTrack == indCurTrack) {
      //     // no photo and no video on the same track
      //     // and no drawing has occurred
      //     // resets to O so that the last photo or video image does not last forever
      //     if(!videoOn) {
      //       out_track_FBO[indCurTrack].rgb = vec3( 0.0 );
      //     }
      //     else {
      //       // nothing to do video or drawing has already been copied
      //     }
      //   }
      // }
#endif
      if( flashPhotoTrkWght > 0 
          && graylevel(photocolor) > flashPhotoTrkWght ) { // flash photo
      // photo image copy when there is a flash photo
        flashToCACumul.rgb = photocolor;
        flashToCACumul.a = 1;
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
        && flashCameraTrkWght == 0 && flashPhotoTrkWght == 0 ) { // no flash video
      flashToCACumul += uniform_Update_fs_4fv_flashTrkCAWghts[indCurTrack] 
                  * vec4(out_track_FBO[indCurTrack].rgb,graylevel(out_track_FBO[indCurTrack].rgb));
    }

    // hides regular tracks when there is a photo or a camera flash
    float flashValueCoef = 1.f;
    if( flashCameraTrkWght > 0 ) {
      flashValueCoef = (1.0 - flashCameraTrkWght);
    }
    if( flashPhotoTrkWght > flashCameraTrkWght ) {
      flashValueCoef =  ((flashPhotoTrkWght <= flashPhotoTrkThreshold) ?
                    (1.0 - flashPhotoTrkWght/flashPhotoTrkThreshold): 0.0);
    }
    out_track_FBO[indCurTrack].rgb *= flashValueCoef;
  }

  ///////////////////////////////////////////////////
  // PARTICLE FLASHES ON CA & TRACKS
  // copies particle rendering at preceding step (for particle flashes)
  out_particlesRendering 
    = texture( uniform_Update_texture_fs_Part_render , decalCoords );

  // particle flash on CA
  flashToCACumul += uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow.z
                  * vec4(out_particlesRendering.rgb,graylevel(out_particlesRendering.rgb));

  // particle flash on BG track
  vec3 cumulatedColor = flashToBGCumul + uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght.w 
                      * out_particlesRendering.rgb;
  float maxCumulatedColor = maxCol( cumulatedColor );
  if( maxCumulatedColor <= 1.0 ) {
    flashToBGCumul = cumulatedColor;
  }

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
    // flash photo
    if( flashPhotoValue.a > 0 ) { // gray level of copy color (in case of flash)
      currentCA.a = -1;
      currentCA.rgb = clamp( flashPhotoValue.rgb , 0.0 , 1.0 );
    }

    // seeding the CA with a predefined shape, size and location
#define  _pg_CAseed_dot_center      0
#define  _pg_CAseed_dot               1
#define  _pg_CAseed_h_line            2
#define  _pg_CAseed_v_line            3
#define  _pg_CAseed_cross             4
#define  _pg_CAseed_X                 5
#define  _pg_CAseed_square            6
    if(uniform_Update_fs_4fv_CAseed_type_size_loc.z > 0) {
      int CAseed_type = int(uniform_Update_fs_4fv_CAseed_type_size_loc.x);
      int CAseed_size = int(uniform_Update_fs_4fv_CAseed_type_size_loc.y);
      vec2 CAseed_location = uniform_Update_fs_4fv_CAseed_type_size_loc.zw;
      switch(CAseed_type) {
        case _pg_CAseed_dot_center:
        if( length(vec2(decalCoords - CAseed_location - vec2(0.5,0.5))) < CAseed_size) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_dot:
        if( length(vec2(decalCoords - CAseed_location)) < CAseed_size) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_h_line:
        if( abs(decalCoords.y - CAseed_location.y) < 1
          && abs(decalCoords.x - CAseed_location.x) < CAseed_size) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_v_line:
        if( abs(decalCoords.x - CAseed_location.x) < 1
          && abs(decalCoords.y - CAseed_location.y) < CAseed_size) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_cross:
        if(( abs(decalCoords.y - CAseed_location.y) < 1
            && abs(decalCoords.x - CAseed_location.x) < CAseed_size)
          || ( abs(decalCoords.x - CAseed_location.x) < 1
              && abs(decalCoords.y - CAseed_location.y) < CAseed_size) ) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_X:
        if(( abs(decalCoords.y - decalCoords.x) <= 1.2
            || abs(decalCoords.y + decalCoords.x) <= 1.2)
          && length(vec2(decalCoords - CAseed_location)) < CAseed_size ) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
        case _pg_CAseed_square:
        if( (abs(abs(decalCoords.y - CAseed_location.y) - CAseed_size) < 1
          && abs(decalCoords.x - CAseed_location.x) < CAseed_size)
          || (abs(abs(decalCoords.x - CAseed_location.x) - CAseed_size) < 1
          && abs(decalCoords.y - CAseed_location.y) < CAseed_size) ) {
            currentCA = vec4(1,1,1,-1);
        }
        break;
      }
    }

     // calculates the new state of the automaton
    CA_out( currentCA );


    // CA "REPOPULATION"
    if( repop_CA > 0 ) {
      if( int(frameNo * randomCA2.w ) % int(15000 *(1-repop_CA)) == int((randomCA2.z * 5000.0
        + randomCA2.x * 5000.0 + randomCA2.y * 5000.0 ) *(1-repop_CA)) ) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Update_fs_3fv_repop_ColorCA.xyz;
#ifdef var_BG_CA_repop_color_mode
        if(BG_CA_repop_color_mode == 1) {
          out4_CA.rgb = textureDensityValue;
        }
#endif
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_attachment_FBO[pg_FBO_fs_CA_attacht] = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.w == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.w 
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

#ifdef var_Pixelstep
    if(frameNo % Pixelstep == 0) {
#endif
    pixel_out();

    // arrival of a new pixel
    if( graylevel(out_color_pixel) > 0 ) {
      // outputs the color from pixel move at drawing layer
      out_track_FBO[0].rgb = out_color_pixel;
        // speed - position
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht] 
	     = vec4( out_speed_pixel , out_position_pixel );
    }
    // update of the current pixel
    else {
      //  modifies speed according to acceleration
        vec2 acceleration;

  #ifdef var_pixel_image_acceleration
        // texture based acceleration shift
        if(pixel_image_acceleration >= 0 && decalCoords.x < width - 2 && decalCoords.y < height - 2) {
          float grey_center = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords ).rgb);
          float greyX = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(1,0)).rgb);
          float greyY = graylevel(texture( uniform_Update_texture_fs_pixel_acc , decalCoords  + vec2(0,1)).rgb);
          float gradX = greyX - grey_center;
          float gradY = greyY - grey_center;
          float norm = length(vec2(gradX, gradY));
          if(norm != 0) {
            float cosa = gradX / norm;
            float sina = gradY / norm;
            acceleration = vec2(dmat2(cosa, -sina, sina, cosa) * vec2(1,1));
          }
          else {
            acceleration = multiTypeGenerativeNoise(pixelTextureCoordinatesPOT_XY, vec2(0)) - pixel_acc_center;
          }
        }
        else
  #endif
        {          
          acceleration = multiTypeGenerativeNoise(pixelTextureCoordinatesPOT_XY, vec2(0)) - pixel_acc_center;
        }

      if( pixel_acc > 0 ) {
      	// acceleration
          // speed
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
          += pixel_acc * acceleration;
      }
        else if( pixel_acc < 0 && pixel_acc > -1) {
      	// damping
          // speed
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
            *= (1 + pixel_acc);
        }
        else { // if( pixel_acc == 0 || pixel_acc <= -1)
          // stops 
          // speed
          out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy = vec2(0);
      }
      // updates the position of the current pixel
        out_attachment_FBO[pg_FBO_fs_Pixels_attacht].zw 
           += out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy; 

        // if the pixel leaves the cell (absolute local position > 0.5), it is erased
      if( abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].z ) > 0.5
      	  || abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].w ) > 0.5 ) {
      	if( pixel_mode == PIXEL_UNIQUE ) {
      	  out_track_FBO[0].rgb = vec3(0);
      	}
          else if( pixel_mode == PIXEL_FIREWORK ) {
            // out_track_FBO[0].rgb += vec3(0);
      }
    }
  }
#ifdef var_Pixelstep
    }
    else {
      // keeps previous step value of out_track_FBO[0].rgb
    }
#endif
  }

  // pixel "ADDITION"
  if( repop_BG > 0 ) {
    if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.w), repop_BG * repop_density_weight) != 0) {
        out_track_FBO[0].rgb = uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.xyz;
#ifdef var_BG_CA_repop_color_mode
        // repopulation with the color of the density texture
        if(BG_CA_repop_color_mode == 1) {
          out_track_FBO[0].rgb = textureDensityValue;
        }
#endif
    }
  }

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // DRAWING AND CA LAYERS: GRAY LEVEL DECAY
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  
  //////////////////////////////////////////////
  // track 0 flashing of tracks >= 1 or particles
  vec3 cumCol = out_track_FBO[0].rgb + flashToBGCumul;
  float max_cumCol = maxCol(cumCol);
  if(max_cumCol > 1) {
     cumCol = cumCol - vec3(1 - max_cumCol);
  }
  out_track_FBO[0].rgb = clamp(cumCol, 0 , 1);

  //////////////////////////////////////////////
  // track decay and clear layer
  for(int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++) {
      if( graylevel(out_track_FBO[indTrack].rgb) > 0 ) {
        if(currentScene != 18) {
          out_track_FBO[indTrack].rgb 
               = out_track_FBO[indTrack].rgb - vec3(trkDecay[indTrack]);
        }
        // saturates in case of incay (instead of going to white)
        else {
          vec3 decayedColor = out_track_FBO[indTrack].rgb - vec3(trkDecay[indTrack]);
          if( trkDecay[indTrack] < 0) {
            // saturated color, nothing to do
            // else proportionally increase each channel
            oderedChannels(decayedColor);
            if(orderedColor[2] <= 1) {
              vec3 decay = vec3(trkDecay[indTrack]);
              decay[orderedRank[0]] = 0;
              decay[orderedRank[1]] *= 0.5;
              out_track_FBO[indTrack].rgb -= decay;
            }
          }
          else {
            out_track_FBO[indTrack].rgb = decayedColor;            
          }
        }
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
  // CA LAYER DECAY
  // the first frame for CA is negative so that it is not drawn
  // in this case its value is preserved as it is
  // and incay or decay does not make sense on a black color
  if( out_attachment_FBO[pg_FBO_fs_CA_attacht].a > 0 ) {
    vec3 newCA_w_decay = out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb;
    // decay for generation 
    newCA_w_decay = clamp( newCA_w_decay - vec3(CAdecay) , 0.0 , 1.0 );

    //////////////////////////////////////////////
    // rebuilds output for the gray/drawing buffer after decay
    // has been taken into consideration
    if( graylevel(newCA_w_decay) > 0 ) {
      out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb = newCA_w_decay;
    }
    else {
      // in case color has faded out the cell is considered as dead
      out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(0);
    }
  }

  //////////////////////////////////////////////
  // FLASH PHOTO, only CA is displayed
  /* if( flashPhotoValue.a > 0 ) { // gray level of copy color (in case of flash)
    out_track_FBO[0] = vec4(vec3(0),1);
    out_track_FBO[0] = vec4(vec3(0),1);
 #if PG_NB_TRACKS >= 2
   out_track_FBO[1] = vec4(vec3(0),1);
#endif
#if PG_NB_TRACKS >= 3
   out_track_FBO[2] = vec4(vec3(0),1);
#endif
#if PG_NB_TRACKS >= 4
   out_track_FBO[3] = vec4(vec3(0),1);
#endif
 } */

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
