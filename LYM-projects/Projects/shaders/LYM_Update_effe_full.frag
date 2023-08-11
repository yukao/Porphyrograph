/***********************************************************************
File: song/shaders/LYM_Update_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

int       pixel_mode;
float     pixel_acc_factor;
float     noiseScale;
float     noiseLineScale;
uniform vec4 uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale;
int       noiseType;
float     noiseAngleScale;
float     noiseCenter_0;
float     noiseCenter_1;
uniform vec4 uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1;
float     pixel_acc_center_0;
float     pixel_acc_center_1;
float     repop_BG;
float     repop_CA;
uniform vec4 uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA;
float     CAParams1;
float     CAParams2;
float     CAParams3;
float     CAParams4;
uniform vec4 uniform_Update_fs_4fv_CAParams1_CAParams2_CAParams3_CAParams4;
float     CAParams5;
float     CAParams6;
float     CAParams7;
float     CAParams8;
uniform vec4 uniform_Update_fs_4fv_CAParams5_CAParams6_CAParams7_CAParams8;
int       currentDrawingTrack;
int       currentVideoTrack;
int       currentPhotoTrack;
int       path_replay_trackNo_1;
uniform vec4 uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1;
int       path_replay_trackNo_2;
int       path_replay_trackNo_3;
int       path_replay_trackNo_4;
int       path_replay_trackNo_5;
uniform vec4 uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5;
int       path_replay_trackNo_6;
int       path_replay_trackNo_7;
float     CAdecay_pulse;
bool      invertMovie;
uniform vec4 uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie;
int       cameraCumul;
float     cameraThreshold;
float     cameraGamma;
float     video_satur;
uniform vec4 uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur;
float     video_satur_pulse;
float     cameraWeight;
float     movieWeight;
float     cameraSobel;
uniform vec4 uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel;
float     movieSobel;
bool      BGSubtr;
int       CAstep;
bool      CAcolorSpread;
uniform vec4 uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread;
bool      freeze;
float     photo_value;
float     photo_value_pulse;
float     photo_satur;
uniform vec4 uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_satur;
float     photo_satur_pulse;
float     mask_scale;
float     photo_scale;
float     mask_contrast;
uniform vec4 uniform_Update_fs_4fv_photo_satur_pulse_mask_scale_photo_scale_mask_contrast;
float     photo_contrast;
uniform float uniform_Update_fs_1fv_photo_contrast;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;

// #define CURVE_PARTICLES
#define SPLAT_PARTICLES

// #define ATELIERS_PORTATIFS

// #define GENERATIVE_MOVIE

////////////////////////////////////////////////////////////////////
// TRACK CONST
#define PG_NB_TRACKS 4
#define PG_NB_PATHS 7

///////////////////////////////////////////////////////////////////
// brush side divider for ateliers portatifs
int brushSizeDivider = 1;

///////////////////////////////////////////////////////////////////
// PARTICLE IMAGE INITIALIZATION
const uint pg_FBO_fs_CA_attacht = 0;
const uint pg_FBO_fs_Pixels_attacht = 1;

////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA
// CA types
/* const uint CA_DISLOCATION = 0;
const uint CA_CYCLIC_1 = 1;
const uint GOL_1 = 2;
const uint CA_PROTOCELLS = 3;
const uint CA_CYCLIC = 4;
 */
// CA types
const uint CA_PREY                    = 0;
const uint CA_DISLOCATION             = 1;
const uint CA_PROTOCELLS              = 2;
const uint CA_SOCIAL_PD               = 3;
const uint CA_PATHS                   = 4;
const uint CA_COLOR                   = 5;
const uint CA_NOTYPE                  = 6;
// const uint CA_PATHS = 4;
// const uint CA_COLOR = 5;
// const uint CA_NOTYPE = 6;

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
vec4 neighborValuesHV[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
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
uniform vec4 uniform_Update_fs_4fv_trkDecay;
uniform vec4 uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght;
uniform vec4 uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift;
uniform vec4 uniform_Update_fs_4fv_pulse;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_W_H_time_currentScene;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_repop_Color_flashCABGWght;
uniform vec3 uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_photo01Wghts_Camera_W_H;
uniform vec2 uniform_Update_fs_2fv_CAType_SubType;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Update_texture_fs_CA;         // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Update_texture_fs_Pixels;  // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
layout (binding = 2) uniform sampler3D   uniform_Update_texture_fs_Brushes;  // pen patterns
layout (binding = 3) uniform samplerRect uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 4) uniform samplerRect uniform_Update_texture_fs_Camera_BG;  // camera BG texture
layout (binding = 5) uniform samplerRect uniform_Update_texture_fs_Movie_frame;  // movie texture
layout (binding = 6) uniform sampler3D   uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 7) uniform samplerRect uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 8) uniform samplerRect uniform_Update_texture_fs_Photo1;  // photo_1 texture
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
                      // particle initialization from images

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

vec2 generativeNoise(vec2 texCoordLoc) {
  // FLAT
    return vec2(snoise( texCoordLoc , noiseScale * 100 ),
                            snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseScale * 100 ));
  }


vec2 multiTypeGenerativeNoise(vec2 texCoordLoc, vec2 usedNeighborOffset) {
  // FLAT
  if(noiseType == 0 )  {
    return vec2(snoise( texCoordLoc , noiseScale * 100 ),
                            snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseScale * 100 ));
  }
  // SUN RAYS
  else if(noiseType == 1 ) {
    vec2 pos = vec2( atan((noiseCenter_0-texCoordLoc.x)/(noiseCenter_1-texCoordLoc.y)) * (noiseAngleScale * 10),
                     length(vec2(noiseCenter_0,noiseCenter_1) - texCoordLoc) / (noiseLineScale) );
    return vec2(snoise( pos , noiseScale * 10 ) ,
                            snoise( pos + vec2(2.0937,9.4872) , noiseScale * 10 ));
  }
  // CAMERA
  else if(noiseType == 2 ) {
    return texture(uniform_Update_texture_fs_Camera_frame, decalCoords + usedNeighborOffset ).rg;
  }
  // MOVIE
  else {
    return texture(uniform_Update_texture_fs_Movie_frame, movieCoord + usedNeighborOffset/ movieWH ).rg;
  }
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

void CA_out( vec4 currentCA ) {
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

  //////////////////////////////////////////////////////
  // PATHS
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_PATHS ) {

    for( int ind = 0 ; ind < 8 ; ind++ ) {
      neighborValuesHV[ind] = texture(uniform_Update_texture_fs_CA, 
                  decalCoords + neighborOffsetsPaths[ind] );
    }
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      neighborValuesDiag[ind] = texture(uniform_Update_texture_fs_CA, 
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
      surrpixel_localColor = texture( uniform_Update_texture_fs_Trk0, newDecalCoord );

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

        vec4 surrpixel_speed_position = texture( uniform_Update_texture_fs_Pixels,
                newDecalCoord );
        surrpixel_speed = surrpixel_speed_position.xy;
        surrpixel_position = surrpixel_speed_position.zw;
        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

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
      surrpixel_localColor = texture( uniform_Update_texture_fs_Trk0,
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
          = texture( uniform_Update_texture_fs_Pixels,
               newDecalCoord );
              surrpixel_speed = surrpixel_speed_position.xy;
              surrpixel_position = surrpixel_speed_position.zw;

        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

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
      = texture(uniform_Update_texture_fs_Brushes,
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
  pixel_mode = int(uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale[0]);
  pixel_acc_factor = uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale[1];
  noiseScale = uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale[2];
  noiseLineScale = uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale[3];
  noiseType = int(uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1[0]);
  noiseAngleScale = uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1[1];
  noiseCenter_0 = uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1[2];
  noiseCenter_1 = uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1[3];
  pixel_acc_center_0 = uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA[0];
  pixel_acc_center_1 = uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA[1];
  repop_BG = uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA[2];
  repop_CA = uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA[3];
  CAParams1 = uniform_Update_fs_4fv_CAParams1_CAParams2_CAParams3_CAParams4[0];
  CAParams2 = uniform_Update_fs_4fv_CAParams1_CAParams2_CAParams3_CAParams4[1];
  CAParams3 = uniform_Update_fs_4fv_CAParams1_CAParams2_CAParams3_CAParams4[2];
  CAParams4 = uniform_Update_fs_4fv_CAParams1_CAParams2_CAParams3_CAParams4[3];
  CAParams5 = uniform_Update_fs_4fv_CAParams5_CAParams6_CAParams7_CAParams8[0];
  CAParams6 = uniform_Update_fs_4fv_CAParams5_CAParams6_CAParams7_CAParams8[1];
  CAParams7 = uniform_Update_fs_4fv_CAParams5_CAParams6_CAParams7_CAParams8[2];
  CAParams8 = uniform_Update_fs_4fv_CAParams5_CAParams6_CAParams7_CAParams8[3];
  currentDrawingTrack = int(uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1[0]);
  currentVideoTrack = int(uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1[1]);
  currentPhotoTrack = int(uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1[2]);
  path_replay_trackNo_1 = int(uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1[3]);
  path_replay_trackNo_2 = int(uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5[0]);
  path_replay_trackNo_3 = int(uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5[1]);
  path_replay_trackNo_4 = int(uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5[2]);
  path_replay_trackNo_5 = int(uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5[3]);
  path_replay_trackNo_6 = int(uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie[0]);
  path_replay_trackNo_7 = int(uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie[1]);
  CAdecay_pulse = uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie[2];
  invertMovie = (uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie[3] > 0 ? true : false);
  cameraCumul = int(uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur[0]);
  cameraThreshold = uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur[1];
  cameraGamma = uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur[2];
  video_satur = uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur[3];
  video_satur_pulse = uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel[0];
  cameraWeight = uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel[1];
  movieWeight = uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel[2];
  cameraSobel = uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel[3];
  movieSobel = uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread[0];
  BGSubtr = (uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread[1] > 0 ? true : false);
  CAstep = int(uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread[2]);
  CAcolorSpread = (uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread[3] > 0 ? true : false);
  freeze = (uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_satur[0] > 0 ? true : false);
  photo_value = uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_satur[1];
  photo_value_pulse = uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_satur[2];
  photo_satur = uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_satur[3];
  photo_satur_pulse = uniform_Update_fs_4fv_photo_satur_pulse_mask_scale_photo_scale_mask_contrast[0];
  mask_scale = uniform_Update_fs_4fv_photo_satur_pulse_mask_scale_photo_scale_mask_contrast[1];
  photo_scale = uniform_Update_fs_4fv_photo_satur_pulse_mask_scale_photo_scale_mask_contrast[2];
  mask_contrast = uniform_Update_fs_4fv_photo_satur_pulse_mask_scale_photo_scale_mask_contrast[3];
  photo_contrast = uniform_Update_fs_1fv_photo_contrast;

  //////////////////////////
  // variables 
  // sound pulse
  vec3 pulse = uniform_Update_fs_4fv_pulse.rgb;
  float average_pulse = uniform_Update_fs_4fv_pulse.a;

  // frame number
  frameNo = int(round(uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.y));

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
  width = uniform_Update_fs_4fv_W_H_time_currentScene.x;
  height = uniform_Update_fs_4fv_W_H_time_currentScene.y;

  // CAType
  CAType = int(uniform_Update_fs_2fv_CAType_SubType.x);
  CASubType = int(uniform_Update_fs_2fv_CAType_SubType.y);


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // pixel texture + z offset according to the chosen texture
  vec2 position = vec2( 1.0 + sin(frameNo/50000.0),
                        1.0 + cos(frameNo/37000.0));
  vec2 noisePositionOffset = vec2(snoise( position , noiseScale * 100 ) ,
                                  snoise( position + vec2(2.0937,9.4872) , 100 )); // 
  pixelTextureCoordinatesXY = decalCoordsPOT + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.0 ) );
  randomCA2 = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.5 ) );

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
    out_track_FBO[1] = texture( uniform_Update_texture_fs_Trk1 , decalCoordsPrevStep );
  }
  else {
    out_track_FBO[1] = vec4( 0, 0, 0, 0 );
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
  if(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.x > 0) {
    vec2 coordsImage = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.xy;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
    photocolor += uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.x * texture(uniform_Update_texture_fs_Photo0, 
        coordsImageScaled ).rgb;
  }
  if(uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.y > 0) {
    vec2 coordsImage = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.zw;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
    photocolor += uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.y * texture(uniform_Update_texture_fs_Photo1,  
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
  cameraWH = uniform_Update_fs_4fv_photo01Wghts_Camera_W_H.zw;

  // video texture used for drawing
/*   cameraCoord = vec2(0.4 * (decalCoordsPOT.x + 0.55), 0.4 * (1. - decalCoordsPOT.y) )
               * cameraWH;
 */  cameraCoord = vec2(1 - decalCoordsPOT.x, (decalCoordsPOT.y) )
               * cameraWH;
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  // image reading
  cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
  // gamma correction
  cameraImage = vec3( pow(cameraImage.r,cameraGamma) , pow(cameraImage.g,cameraGamma) , pow(cameraImage.b,cameraGamma) );
  if( BGSubtr ) {
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
  float  powerColor = sqrt( (videocolor.r)*(videocolor.r) * .299 +
                             (videocolor.g)*(videocolor.g) * .587 +
                             (videocolor.b)*(videocolor.b) * .114 ) ;
  videocolor.r = clamp( powerColor 
    + (videocolor.r - powerColor) * pulsed_satur.r , 0 , 1 );
  videocolor.g = clamp( powerColor 
    + (videocolor.g - powerColor) * pulsed_satur.g , 0 , 1 );
  videocolor.b = clamp( powerColor 
    + (videocolor.b - powerColor) * pulsed_satur.b , 0 , 1 );

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
      if( int(frameNo * randomCA2.w ) % int(15000 *(1-repop_CA)) == int((randomCA2.z * 5000.0
        + randomCA2.x * 5000.0 + randomCA2.y * 5000.0 ) *(1-repop_CA)) ) {
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
				    + uniform_Update_fs_4fv_repop_Color_flashCABGWght.w * out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 
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
      radiuspixel = 1;
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
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseScale * 100 ),
                                snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));
      // }

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc_factor > 0 ) {
      	// acceleration
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
          += pixel_acc_factor * acceleration;
      }
      else {
      	// damping
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy 
          += pixel_acc_factor * out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy;
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
    if( int(frameNo * randomCA2.z ) % int(15000 *(1-repop_BG)) == int((randomCA2.x * 5000.0
        + randomCA2.y * 5000.0 + randomCA2.w * 5000.0 ) *(1-repop_BG)) ) {
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
#ifdef ATELIERS_PORTATIFS
        // no decay in the central zone of track 1
        if(indTrack != 1 || decalCoords.x < width/2 - 70 || decalCoords.x > width/2 + 70) {
#endif
          out_track_FBO[indTrack].rgb 
               = out_track_FBO[indTrack].rgb - vec3(uniform_Update_fs_4fv_trkDecay[indTrack]);
#ifdef ATELIERS_PORTATIFS
        }
#endif
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
  if( uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.y > 0 ) {
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
