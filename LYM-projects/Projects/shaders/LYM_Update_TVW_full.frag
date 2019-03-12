/***********************************************************************
/***********************************************************************
File: effe/shaders/LYM_Drawing_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/
#version 420

bool      camera_BG_subtr;
float     CAdecay;
float     trkDecay_0;
float     trkDecay_1;
uniform vec4 uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1;
float     trkDecay_2;
float     trkDecay_3;
int       currentDrawingTrack;
int       currentVideoTrack;
uniform vec4 uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack;
int       currentPhotoTrack;
float     noiseScale;
int       noiseType;
float     noiseLineScale;
uniform vec4 uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale;
float     noiseAngleScale;
float     noiseCenterX;
float     noiseCenterY;
float     pixel_acc;
uniform vec4 uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc;
float     pixel_acc_shiftX;
float     pixel_acc_shiftY;
float     pixel_radius;
int       pixel_mode;
uniform vec4 uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode;
float     repop_CA;
float     repop_BG;
float     cameraGamma;
float     cameraThreshold;
uniform vec4 uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold;
float     cameraWeight;
float     cameraSobel;
float     movieWeight;
float     movieSobel;
uniform vec4 uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel;
bool      invertMovie;
float     video_hue;
float     video_satur;
float     video_satur_pulse;
uniform vec4 uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse;
float     video_value;
float     photoWeight;
float     photo_hue;
float     photo_satur;
uniform vec4 uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur;
float     photo_satur_pulse;
float     photo_value;
float     photo_value_pulse;
float     photo_scale;
uniform vec4 uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale;
float     mask_scale;
float     photo_contrast;
float     mask_contrast;
float     CAParams1;
uniform vec4 uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1;
float     CAParams2;
float     CAParams3;
float     CAParams4;
float     CAParams5;
uniform vec4 uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5;
float     CAParams6;
float     CAParams7;
float     CAParams8;
int       cameraCumul;
uniform vec4 uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul;
int       CAstep;
bool      CAcolorSpread;
bool      freeze;
uniform vec3 uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze;

#define PG_NB_TRACKS 2

//////////////////////////
// TRACK DECAY
vec4 trkDecay = vec4(trkDecay_0,trkDecay_1,trkDecay_2,trkDecay_3);

const float PI = 3.1415926535897932384626433832795;

float dist_to_mousePos = 0;
float signed_rx;
float signed_ry;

// center of the translation of acceleration values
// used for shifting Pixel motion in a direction
vec2 pixel_acc_center = vec2(0.5,0.5);

// out_Pixel returned values
vec3 out_color_pixel;
vec2 out_position_pixel = vec2(0,0);
vec2 out_speed_pixel = vec2(0,0);

vec4 out4_CA;

bool CA_on_off;

int currentScene;



// CA types
#define CA_PREY                    0
// #define CA_DISLOCATION             1
#define CA_DRIPPING                1
#define CA_PROTOCELLS              2
#define CA_SOCIAL_PD               3
#define CA_PATHS                   4

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or pixels
bool newly_drawn = false;

// cursor type (+1 for stylus , -1 for rubber )
float Cursor = 1;

vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesHV[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
                                    vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

/////////////////////////////////////
// screen & FBO size
float height;
float width;

// CA Type
int CAType;
int CASubType;

vec2 pixelTextureCoordinatesXY; // the POT coordinates of the
// pixel texture 

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))
#define delta(gray) (gray>0.5?1:0)

const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
                                 {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
const vec2 neighborOffsetsPaths[8] = {{1,0},{2,0},{0,1},{0,2},      // EE EE N NN
                                      {-1,0},{-2,0},{0,-1},{0,-2}};  // W WW S SS
const vec2 neighborOffsetsNonPaths[16] = {{1,1},{2,1},{1,2},{2,2},
                                          {-1,-1},{-2,-1},{-1,-2},{-2,-2},
                                          {1,-1},{2,-1},{1,-2},{2,-2},
                                          {-1,1},{-2,1},{-1,2},{-2,2}};  // W WW S SS
const float neighborDistances[8]={1.0,0.707,1.0,0.707,1.0,0.707,1.0,0.707};

// VIDEO
float videoGray = 0.0;
vec3 cameraImage = vec3(0.0);
vec3 videocolor = vec3(0.0);
vec2 cameraCoord = vec2(0.0);
vec2 movieCoord = vec2(0.0);
vec2 movieWH;

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

// frame number
int frameNo;

// time
float time;

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

vec4 randomCA;
vec4 randomCA2;

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Update_fs_4fv_paths03_x;
uniform vec4 uniform_Update_fs_4fv_paths03_y;
uniform vec4 uniform_Update_fs_4fv_paths03_x_prev;
uniform vec4 uniform_Update_fs_4fv_paths03_y_prev;
uniform vec4 uniform_Update_fs_4fv_paths03_BrushID;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusX;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusY;
uniform vec4 uniform_Update_fs_4fv_flashTrkCAWghts;  
uniform vec4 uniform_Update_fs_4fv_repop_Color_flashCABGWght;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_paths03_r;
uniform vec4 uniform_Update_fs_4fv_paths03_g;
uniform vec4 uniform_Update_fs_4fv_paths03_b;
uniform vec4 uniform_Update_fs_4fv_paths03_a;
uniform vec4 uniform_Update_fs_4fv_W_H_time_currentScene;
uniform vec3 uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
uniform vec3 uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght;

uniform vec4 uniform_Update_fs_4fv_pulse;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  
uniform vec3 uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;

// layer weights combined with mask value to assess the visibility of a layer
uniform vec4 uniform_Update_fs_4fv_weights03;
uniform vec2 uniform_Update_fs_2fv_weights45;
uniform vec4 uniform_Update_fs_4fv_image_noisesxy[3];
uniform vec4 uniform_Update_fs_4fv_mask_noisesxy[3];

// interpolation between image buffer and swap buffer
uniform vec3 uniform_Update_fs_3fv_alphaSwap02;
uniform vec3 uniform_Update_fs_3fv_alphaSwap35;

// uniform vec4 uniform_Update_fs_4fv_flashBackCoef_videoWH_videoThreshold;
// uniform vec4 uniform_Update_fs_4fv_mouseTracks_Color_rgba;
// uniform vec4 uniform_Update_fs_4fv_partAcc_clearAllLayers_frameNo_time;
// uniform vec4 uniform_Update_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor;
// uniform vec4 uniform_Update_fs_4fv_W_H_drawingStart_drawingStroke;
// uniform vec4 uniform_Update_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;  
// uniform vec3 uniform_Update_fs_3fv_clearLayer_flashCameraTrkWght_flashCameraTrkThreshold;



/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Update_texture_fs_CA;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
layout (binding = 1) uniform samplerRect uniform_Update_texture_fs_Pixels;  // 2-cycle ping-pong Drawing pass speed/position of pixels step n (FBO attachment 2)
layout (binding = 2) uniform sampler3D   uniform_Update_texture_fs_Brushes;  // pen patterns
layout (binding = 3) uniform samplerRect uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 4) uniform samplerRect uniform_Update_texture_fs_Camera_BG;  // camera BG texture
layout (binding = 5) uniform samplerRect uniform_Update_texture_fs_Movie_frame;  // movie textures
layout (binding = 6) uniform sampler3D   uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 7) uniform samplerRect uniform_Update_texture_fs_Part_render;  // FBO capture of particle rendering
                      // particle initialization from images
layout (binding = 8) uniform sampler2D uniform_Update_texture_fs_TVWPixels0;  // buffer image #0
layout (binding = 9) uniform sampler2D uniform_Update_texture_fs_TVWPixels1;  // buffer image #1 
layout (binding = 10) uniform sampler2D uniform_Update_texture_fs_TVWPixels2;  // buffer image #2 
layout (binding = 11) uniform sampler2D uniform_Update_texture_fs_TVWPixels3;  // buffer image #3 
layout (binding = 12) uniform sampler2D uniform_Update_texture_fs_TVWPixels4;  // buffer image #4 
layout (binding = 13) uniform sampler2D uniform_Update_texture_fs_TVWPixels5;  // buffer image #5 
layout (binding = 14) uniform samplerRect uniform_Update_texture_fs_TVWMask02;  // buffer image mask #0
layout (binding = 15) uniform samplerRect uniform_Update_texture_fs_TVWMask35;  // buffer image mask #1
layout (binding = 16) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap0;  // buffer swap image #0
layout (binding = 17) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap1;  // buffer swap image #1 
layout (binding = 18) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap2;  // buffer swap image #2 
layout (binding = 19) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap3;  // buffer swap image #3 
layout (binding = 20) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap4;  // buffer swap image #4 
layout (binding = 21) uniform sampler2D uniform_Update_texture_fs_TVWPixelsSwap5;  // buffer swap image #5 
layout (binding = 22) uniform samplerRect uniform_Update_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)
#if PG_NB_TRACKS >= 2
layout (binding = 23) uniform samplerRect uniform_Update_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 6)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 24) uniform samplerRect uniform_Update_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 25) uniform samplerRect uniform_Update_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
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
    vec2 pos = vec2( atan((noiseCenterX-texCoordLoc.x)/(noiseCenterY-texCoordLoc.y)) * (noiseAngleScale * 10),
                     length(vec2(noiseCenterX,noiseCenterY) - texCoordLoc) / (noiseLineScale) );
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
      (neighborValues[0].rgb * inverseShadePrey[ int(max(0,neighborValues[0].a)) ]) +
      (neighborValues[1].rgb * inverseShadePrey[ int(max(0,neighborValues[1].a)) ]) +
      (neighborValues[2].rgb * inverseShadePrey[ int(max(0,neighborValues[2].a)) ]) +
      (neighborValues[3].rgb * inverseShadePrey[ int(max(0,neighborValues[3].a)) ]) +
      (neighborValues[4].rgb * inverseShadePrey[ int(max(0,neighborValues[4].a)) ]) +
      (neighborValues[5].rgb * inverseShadePrey[ int(max(0,neighborValues[5].a)) ]) +
      (neighborValues[6].rgb * inverseShadePrey[ int(max(0,neighborValues[6].a)) ]) +
      (neighborValues[7].rgb * inverseShadePrey[ int(max(0,neighborValues[7].a)) ]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadePrey[ int(max(0,currentCA.a)) ]) +
      (neighborValues[0].rgb * inverseShadePrey[ int(max(0,neighborValues[0].a)) ]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadePrey[ int(max(0,neighborValues[ind].a)) ];
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
      out4_CA.rgb = shadePrey[ newState ].r * currentCA.rgb;
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
      out4_CA.rgb = shadePrey[ newState ] * averageSurrounding;
      // out4_CA.rgb = vec3(shadePrey[ newState ]);
    }
  }
/*
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
      (neighborValues[0].rgb * inverseShadeDisloc[ int(max(0,neighborValues[0].a)) ]) +
      (neighborValues[1].rgb * inverseShadeDisloc[ int(max(0,neighborValues[1].a)) ]) +
      (neighborValues[2].rgb * inverseShadeDisloc[ int(max(0,neighborValues[2].a)) ]) +
      (neighborValues[3].rgb * inverseShadeDisloc[ int(max(0,neighborValues[3].a)) ]) +
      (neighborValues[4].rgb * inverseShadeDisloc[ int(max(0,neighborValues[4].a)) ]) +
      (neighborValues[5].rgb * inverseShadeDisloc[ int(max(0,neighborValues[5].a)) ]) +
      (neighborValues[6].rgb * inverseShadeDisloc[ int(max(0,neighborValues[6].a)) ]) +
      (neighborValues[7].rgb * inverseShadeDisloc[ int(max(0,neighborValues[7].a)) ]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeDisloc[ int(max(0,currentCA.a)) ]) +
      (neighborValues[0].rgb * inverseShadeDisloc[ int(max(0,neighborValues[0].a)) ]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeDisloc[ int(max(0,neighborValues[ind].a)) ];
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
      out4_CA.rgb = shadeDisloc[ newState ].r * currentCA.rgb;
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
      out4_CA.rgb = shadeDisloc[ newState ] * averageSurrounding;
      // out4_CA.rgb = colorDisloc[ newState ];
    }
    // const vec3 colorDisloc[5] = {{0.3,0.3,0.3},     // emptry
    //                              {0,1,0},{0,0,1},   // new/old border
    //                              {1,0,0},{1,1,0}};  // new/old nucleus
  }
*/
  //////////////////////////////////////////////////////
  // PROTOCELLS
  else if( CAType == CA_DRIPPING ) {
// const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
//                                  {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
// south bias
const float neighborSouthBias[8] = {1.,-0.5,-1.,-0.5,      // E NE N NW
                                    1.,1.3,2.,1.3,};    // W SW S SE

    float nbNeighbors =
      (neighborValues[0].a > 0? neighborSouthBias[0]:0) +
      (neighborValues[1].a > 0? neighborSouthBias[1]:0) +
      (neighborValues[2].a > 0? neighborSouthBias[2]:0) +
      (neighborValues[3].a > 0? neighborSouthBias[3]:0) +
      (neighborValues[4].a > 0? neighborSouthBias[4]:0) +
      (neighborValues[5].a > 0? neighborSouthBias[5]:0) +
      (neighborValues[6].a > 0? neighborSouthBias[6]:0) +
      (neighborValues[7].a > 0? neighborSouthBias[7]:0);
/*
    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeProtocell[ int(max(0,neighborValues[0].a)) ]) +
      (neighborValues[1].rgb * inverseShadeProtocell[ int(max(0,neighborValues[1].a)) ]) +
      (neighborValues[2].rgb * inverseShadeProtocell[ int(max(0,neighborValues[2].a)) ]) +
      (neighborValues[3].rgb * inverseShadeProtocell[ int(max(0,neighborValues[3].a)) ]) +
      (neighborValues[4].rgb * inverseShadeProtocell[ int(max(0,neighborValues[4].a)) ]) +
      (neighborValues[5].rgb * inverseShadeProtocell[ int(max(0,neighborValues[5].a)) ]) +
      (neighborValues[6].rgb * inverseShadeProtocell[ int(max(0,neighborValues[6].a)) ]) +
      (neighborValues[7].rgb * inverseShadeProtocell[ int(max(0,neighborValues[7].a)) ]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeProtocell[ int(max(0,currentCA.a)) ]) +
      (neighborValues[0].rgb * inverseShadeProtocell[ int(max(0,neighborValues[0].a)) ]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeProtocell[ int(max(0,neighborValues[ind].a)) ];
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
      (neighborValues[0].rgb * neighborSouthBias[0]) +
      (neighborValues[1].rgb * neighborSouthBias[1]) +
      (neighborValues[2].rgb * neighborSouthBias[2]) +
      (neighborValues[3].rgb * neighborSouthBias[3]) +
      (neighborValues[4].rgb * neighborSouthBias[4]) +
      (neighborValues[5].rgb * neighborSouthBias[5]) +
      (neighborValues[6].rgb * neighborSouthBias[6]) +
      (neighborValues[7].rgb * neighborSouthBias[7]);

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
      out4_CA.rgb = shadeProtocell[ newState ].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams1: px - X duplication from X+Y (randomCA.y)
      // CAParams2: py - Y duplication from Y+Z (randomCA.z)
      // CAParams3: pz - Z duplication from Z+X (randomCA.w)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(randomCA2.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && randomCA.x < CAParams1) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(randomCA2.y * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && randomCA.y < CAParams2) {
              newState = 2; // new Y
            }
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Z and a neighboring X slot
          int firstindex = clamp(int(randomCA2.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && randomCA.z < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(randomCA.x * 8),0,8);
          newState = 0;
          // looks for an old border around
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a != 0 ) {
              newState = int(neighborValues[ind % 8].a);
            }
          }
        }
      }
      // CAParams5: px - X/Y/Z degradation (randomCA2.x)
      // randomCA2.y choice of a first neighbor
      // degradation or migration of X
      else if( randomCA2.x < CAParams4 
        || nbNeighbors < 8 * CAParams5 ) { 
        newState = 0; 
      }

      // if( state ==  1 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.x < CAParams4
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
      //   if( randomCA2.y < CAParams5
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
      //   if( randomCA2.z < CAParams6
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      // }


    out4_CA.a = float(newState);
    // state change: takes the color of the current stage
    if( currentCA.a != out4_CA.a ) {
      out4_CA.rgb = shadeProtocell[ newState ].r * currentCA.rgb;
    }
    else {
      //averageSurrounding  maxSurrounding
      maxSurrounding = clamp( maxSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeProtocell[ newState ] * maxSurrounding;
      // out4_CA.rgb = vec3(shadeProtocell[ newState ]);
    }
  }
}

  //////////////////////////////////////////////////////
  // PROTOCELLS
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
/*
    vec3 gatherColorFactors =
      (neighborValues[0].rgb * inverseShadeProtocell[ int(max(0,neighborValues[0].a)) ]) +
      (neighborValues[1].rgb * inverseShadeProtocell[ int(max(0,neighborValues[1].a)) ]) +
      (neighborValues[2].rgb * inverseShadeProtocell[ int(max(0,neighborValues[2].a)) ]) +
      (neighborValues[3].rgb * inverseShadeProtocell[ int(max(0,neighborValues[3].a)) ]) +
      (neighborValues[4].rgb * inverseShadeProtocell[ int(max(0,neighborValues[4].a)) ]) +
      (neighborValues[5].rgb * inverseShadeProtocell[ int(max(0,neighborValues[5].a)) ]) +
      (neighborValues[6].rgb * inverseShadeProtocell[ int(max(0,neighborValues[6].a)) ]) +
      (neighborValues[7].rgb * inverseShadeProtocell[ int(max(0,neighborValues[7].a)) ]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeProtocell[ int(max(0,currentCA.a)) ]) +
      (neighborValues[0].rgb * inverseShadeProtocell[ int(max(0,neighborValues[0].a)) ]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeProtocell[ int(max(0,neighborValues[ind].a)) ];
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
      out4_CA.rgb = shadeProtocell[ newState ].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams1: px - X duplication from X+Y (randomCA.y)
      // CAParams2: py - Y duplication from Y+Z (randomCA.z)
      // CAParams3: pz - Z duplication from Z+X (randomCA.w)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(randomCA2.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && randomCA.x < CAParams1) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(randomCA2.y * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && randomCA.y < CAParams2) {
              newState = 2; // new Y
            }
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Z and a neighboring X slot
          int firstindex = clamp(int(randomCA2.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && randomCA.z < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(randomCA.x * 8),0,8);
          newState = 0;
          // looks for an old border around
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a != 0 ) {
              newState = int(neighborValues[ind % 8].a);
            }
          }
        }
      }
      // CAParams5: px - X/Y/Z degradation (randomCA2.x)
      // randomCA2.y choice of a first neighbor
      // degradation or migration of X
      else if( randomCA2.x < CAParams4 
        || nbNeighbors < 8 * CAParams5 ) { 
        newState = 0; 
      }

      // if( state ==  1 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( randomCA2.x < CAParams4
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
      //   if( randomCA2.y < CAParams5
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
      //   if( randomCA2.z < CAParams6
      //       || nbNeighbors < 8 ) {
      //     newState = 0; //dies or migrates
      //   }
      // }


    out4_CA.a = float(newState);
    // state change: takes the color of the current stage
    if( currentCA.a != out4_CA.a ) {
      out4_CA.rgb = shadeProtocell[ newState ].r * currentCA.rgb;
    }
    else {
      //averageSurrounding  maxSurrounding
      maxSurrounding = clamp( maxSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeProtocell[ newState ] * maxSurrounding;
      // out4_CA.rgb = vec3(shadeProtocell[ newState ]);
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
      (neighborValues[0].rgb * inverseShadeSocial[ int(max(0,neighborValues[0].a)) ]) +
      (neighborValues[1].rgb * inverseShadeSocial[ int(max(0,neighborValues[1].a)) ]) +
      (neighborValues[2].rgb * inverseShadeSocial[ int(max(0,neighborValues[2].a)) ]) +
      (neighborValues[3].rgb * inverseShadeSocial[ int(max(0,neighborValues[3].a)) ]) +
      (neighborValues[4].rgb * inverseShadeSocial[ int(max(0,neighborValues[4].a)) ]) +
      (neighborValues[5].rgb * inverseShadeSocial[ int(max(0,neighborValues[5].a)) ]) +
      (neighborValues[6].rgb * inverseShadeSocial[ int(max(0,neighborValues[6].a)) ]) +
      (neighborValues[7].rgb * inverseShadeSocial[ int(max(0,neighborValues[7].a)) ]) ;

    vec3 maxColorFactors =
      //(currentCA.rgb * inverseShadeSocial[ int(max(0,currentCA.a)) ]) +
      (neighborValues[0].rgb * inverseShadeSocial[ int(max(0,neighborValues[0].a)) ]);
    for( int ind = 1; ind < 8 ; ind++ ) {
      vec3 newColorFactor = neighborValues[ind].rgb 
                          * inverseShadeSocial[ int(max(0,neighborValues[ind].a)) ];
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
      out4_CA.rgb = shadeSocial[ newState ].r * currentCA.rgb;
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
      out4_CA.rgb = shadeSocial[ newState ] * averageSurrounding;
    }
  }

  //////////////////////////////////////////////////////
  // DISLOCATION
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
    out4_CA = currentCA; // ready
  }

}

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
      vec4 surrPixel_localColor;
      surrPixel_localColor = texture( uniform_Update_texture_fs_Trk0, newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the loclation of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-pixel_radius,+pixel_radius]x[-pixel_radius,+pixel_radius]
      if( graylevel(surrPixel_localColor) >  0 ) {
        vec2 surrPixel_speed;
        vec2 surrPixel_position;
        vec2 surrPixel_nextPosition;

        vec4 surrPixel_speed_position = texture( uniform_Update_texture_fs_Pixels,
						    newDecalCoord );
        surrPixel_speed = surrPixel_speed_position.xy;
        surrPixel_position = surrPixel_speed_position.zw;
        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

      	vec2 acceleration;
      	acceleration = pixel_acceleration - pixel_acc_center;
      	if( pixel_acc > 0 ) {
      	  // acceleration
      	  surrPixel_speed += pixel_acc * acceleration;
      	}
      	else {
      	  // damping
      	  surrPixel_speed += pixel_acc * surrPixel_speed;
      	}
      	surrPixel_nextPosition 
      			     = usedNeighborOffset + surrPixel_position + surrPixel_speed; 
      	// the current step is added to the position

        if( abs(surrPixel_nextPosition.x) <= 0.5 
                  && abs(surrPixel_nextPosition.y) <= 0.5 ) {
          out_color_pixel += surrPixel_localColor.rgb;
          out_speed_pixel += surrPixel_speed;
          // computes the position of the pixel
          out_position_pixel += surrPixel_nextPosition;
          nb_cumultated_pixels++;
        }
        else if( abs(surrPixel_nextPosition.x) <= pixel_radius
                  && abs(surrPixel_nextPosition.y) <= pixel_radius ) {
          float dist = pixel_radius - length(surrPixel_nextPosition);
          out_color_pixel += surrPixel_localColor.rgb;
          out_speed_pixel += dist * (randomCA.zy - vec2(0.5));
          // computes the position of the pixel
          out_position_pixel += surrPixel_nextPosition;
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

      vec4 surrPixel_localColor;
      surrPixel_localColor = texture( uniform_Update_texture_fs_Trk0,
					 newDecalCoord );

      // if the pixel is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the location of the
      // pixel in the coordinates system of the current pixel
      // expected to be in [-pixel_radius,+pixel_radius]x[-pixel_radius,+pixel_radius]
      if( graylevel(surrPixel_localColor) >  0 ) {
        vec2 surrPixel_speed;
        vec2 surrPixel_position;
        vec2 surrPixel_nextPosition;

        vec4 surrPixel_speed_position
	         = texture( uniform_Update_texture_fs_Pixels,
		     newDecalCoord );
        surrPixel_speed = surrPixel_speed_position.xy;
        surrPixel_position = surrPixel_speed_position.zw;

        vec2 pixel_acceleration;
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

      	vec2 acceleration;
      	acceleration = pixel_acceleration - pixel_acc_center;
      	if( pixel_acc > 0 ) {
      	  // acceleration
      	  surrPixel_speed += pixel_acc * acceleration;
      	}
      	else {
      	  // damping
      	  surrPixel_speed += pixel_acc * surrPixel_speed;
      	}
      	surrPixel_nextPosition 
      			     = usedNeighborOffset + surrPixel_position + surrPixel_speed; 
      	// the current step is added to the position

      	if( abs(surrPixel_nextPosition.x) <= pixel_radius 
                  && abs(surrPixel_nextPosition.y) <= pixel_radius ) {
      	  out_color_pixel += surrPixel_localColor.rgb;
                out_speed_pixel += surrPixel_speed;
      	  // computes the position of the pixel
          out_position_pixel += surrPixel_nextPosition;
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
                      (current_brushID / 3) * 0.5 + 0.250))[current_brushID % 3];
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
  camera_BG_subtr = (uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1[0] > 0 ? true : false);
  CAdecay = uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1[1];
  trkDecay_0 = uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1[2];
  trkDecay_1 = uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1[3];
  trkDecay_2 = uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack[0];
  trkDecay_3 = uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack[1];
  currentDrawingTrack = int(uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack[2]);
  currentVideoTrack = int(uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack[3]);
  currentPhotoTrack = int(uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale[0]);
  noiseScale = uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale[1];
  noiseType = int(uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale[2]);
  noiseLineScale = uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale[3];
  noiseAngleScale = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[0];
  noiseCenterX = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[1];
  noiseCenterY = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[2];
  pixel_acc = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[3];
  pixel_acc_shiftX = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[0];
  pixel_acc_shiftY = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[1];
  pixel_radius = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[2];
  pixel_mode = int(uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[3]);
  repop_CA = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold[0];
  repop_BG = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold[1];
  cameraGamma = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold[2];
  cameraThreshold = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold[3];
  cameraWeight = uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel[0];
  cameraSobel = uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel[1];
  movieWeight = uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel[2];
  movieSobel = uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel[3];
  invertMovie = (uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse[0] > 0 ? true : false);
  video_hue = uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse[1];
  video_satur = uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse[2];
  video_satur_pulse = uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse[3];
  video_value = uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur[0];
  photoWeight = uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur[1];
  photo_hue = uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur[2];
  photo_satur = uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur[3];
  photo_satur_pulse = uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale[0];
  photo_value = uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale[1];
  photo_value_pulse = uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale[2];
  photo_scale = uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale[3];
  mask_scale = uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1[0];
  photo_contrast = uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1[1];
  mask_contrast = uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1[2];
  CAParams1 = uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1[3];
  CAParams2 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[0];
  CAParams3 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[1];
  CAParams4 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[2];
  CAParams5 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[3];
  CAParams6 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[0];
  CAParams7 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[1];
  CAParams8 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[2];
  cameraCumul = int(uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[3]);
  CAstep = int(uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze[0]);
  CAcolorSpread = (uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze[1] > 0 ? true : false);
  freeze = (uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze[2] > 0 ? true : false);

// CA Paramaters
// 0.598 0.362 0.535 0.835 0.512 0.488 0.441 0.724 0.425 0.528 0.48  0.693 0.504 0.488 0.339 0.638 0.276 0.874 0.606 0.228 0.276 0.874 0.606 0.228 0.339 0.709 0.276 0.874 0.606 0.228 0.291 0.283 0.842 0.276 0.874 0.606 0.228 0.165 0.347 0.394


  /////////////////////////////////////
  // PARTICLE  FBOOUTPUT
  vec4 out_speed_position_FBO;      // 
  /////////////////////////////////////
  // COLOR FBO OUTPUT
  vec4 out_localColor_FBO;

  /////////////////////////////////////
  // CA FBO OUTPUT
  vec4 out_CA_FBO;

  /////////////////////////////////////
  // TRACK FBO OUTPUT
#if PG_NB_TRACKS >= 2
  vec4 out_track1_FBO; // color of track 1
#endif

  //////////////////////////
  // variables 

  vec3 pulse = uniform_Update_fs_4fv_pulse.rgb;
  

  // frame number
  frameNo = int(round(uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.x));

  // current scene
  currentScene = int(uniform_Update_fs_4fv_W_H_time_currentScene.w);

  // time
  time = uniform_Update_fs_4fv_W_H_time_currentScene.z;
  
  // track number: 0 ==  bg and 1-2 == 2 playing tracks
  // in includes
  // currentDrawingTrack track on which tablet drawing takes place
  // currentVideoTrack track on which video is displayed

  // current drawing mode
  // drawingStroke = int(uniform_Update_fs_4fv_W_H_drawingStart_drawingStroke.w);
  drawingStroke = DRAWING_LINE;

  // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.y;

  // pixels
  pixel_acc_center = vec2(pixel_acc_shiftX,pixel_acc_shiftY);
  

  // working variables for screen dimension
  width = uniform_Update_fs_4fv_W_H_time_currentScene.x;
  height = uniform_Update_fs_4fv_W_H_time_currentScene.y;

  // CAType
  CAType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.x);
  CASubType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.y);

  /////////////////////////////////////////////////////////////////////////////
  // CAdecay pulsation for the scene of gbagbo (like flames on the drawing)
  if(currentScene == 7 || currentScene == 8) {
    CAdecay *= 0.3 * (0.1 * (cos(time*11) + cos(time *3) + cos(time *5) + cos(time *7)) + 1);
  }
  
  // cellular automata
  CA_on_off = (CASubType > 0);


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


  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
  out_localColor_FBO = vec4( 0, 0, 0, 0 );
  out_CA_FBO = vec4( 0, 0, 0, 0 );

#if PG_NB_TRACKS >= 2
  out_track1_FBO = vec4( 0, 0, 0, 0 );
#endif
  if(frameNo <= 10 || uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift.x > 0) {
    out_Update_FBO_fs_Trk0 = vec4(0,0,0,1);
    out_Update_FBO_fs_Pixels = vec4(0,0,0,1);
    out_Update_FBO_fs_CA = vec4(0,0,0,0);
    out_Update_FBO_fs_Trk1 = vec4(0,0,0,1);
    // outColor4 = vec4(0,0,0,1);
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

  // copy of previous step output
  // possible horizontal or vertical translation of background texture
  vec2 decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.xy;
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_localColor_FBO = texture( uniform_Update_texture_fs_Trk0 , decalCoordsPrevStep );
  }
  else {
    out_localColor_FBO = vec4( 0, 0, 0, 0 );
  }
  out_speed_position_FBO = texture( uniform_Update_texture_fs_Pixels , decalCoords );

  out_CA_FBO
    = texture( uniform_Update_texture_fs_CA , decalCoords );

#if PG_NB_TRACKS >= 2
  decalCoordsPrevStep = decalCoords 
        + uniform_Update_fs_4fv_xy_transl_tracks_0_1.zw;

  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track1_FBO = texture( uniform_Update_texture_fs_Trk1 , decalCoordsPrevStep );
  }
  else {
    out_track1_FBO = vec4( 0, 0, 0, 0 );
  }
#endif

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_localColor_FBO.rgb;

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PARTICLE: increase radius for pixel capture
  //////////////////////////////////////
  //////////////////////////////////////
  vec3 trackColorCopy = vec3(0.0);
  vec4 trackCACopy = vec4(0.0);
 
  /////////////////////////////////////////////////
  // void drawing, copies the previous pixel, just manages decay
  // at the end of this code
  // nothing needs to be done here
  // if( drawingStroke == DRAWING_VOID ) {
  // }

  //////////////////////////////////////////////
  // STROKE INTERPOLATION FOR LINE
  // int newtrack = -1;
  float background_drawing_opacity = 0.0;
  // each track possibly covers the previous color

  float flashCameraTrkWght = uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.z;
  float flashCameraTrkThreshold = uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.z;

  // VIDEO FRAME CAPTURE AND RENDERING
  if( cameraWeight > 0 || flashCameraTrkWght > 0 )  
  {
    // video texture used for drawing
    cameraCoord = vec2(1.0-decalCoordsPOT.x , decalCoordsPOT.y )
                 * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw
                 + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
    cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
    // gamma correction
    cameraImage = vec3( pow(cameraImage.r,cameraGamma) );

    if( camera_BG_subtr ) {
      videocolor = abs(cameraImage - texture(uniform_Update_texture_fs_Camera_BG, cameraCoord ).rgb); // initial background subtraction
    }

    videoGray = graylevel(videocolor);
    if( videoGray <= cameraThreshold ) {
      videocolor = vec3(0.0);
    }
    else {
      videocolor = vec3(1.0);
    }
  }

#if PG_NB_TRACKS >= 2
  ////////////////////////////////////////////////////////////////////////////////////
  // MOIRE / MASKS ADDITION TO LAYER #1
  ////////////////////////////////////////////////////////////////////////////////////
  // TRACK 1
  // moire
  vec2 coordsMaskScaled = decalCoords / mask_scale + vec2(0.5) * vec2(width,height) * (mask_scale - 1) / mask_scale;
  float mask1 = texture( uniform_Update_texture_fs_TVWMask02 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[0].xy ) .r;
  float mask2 = texture( uniform_Update_texture_fs_TVWMask02 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[0].zw ) .g;
  float mask3 = texture( uniform_Update_texture_fs_TVWMask02 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[1].xy ) .b;
  float mask4 = texture( uniform_Update_texture_fs_TVWMask35 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[1].zw ) .r;
  float mask5 = texture( uniform_Update_texture_fs_TVWMask35 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[2].xy ) .g;
  float mask6 = texture( uniform_Update_texture_fs_TVWMask35 , coordsMaskScaled + uniform_Update_fs_4fv_mask_noisesxy[2].zw ) .b;
  
  // mask contrast
  //  from http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/
   float  correctionFactorMask =  1.016 * (mask_contrast + 1.0) 
                                  / (1.016 - mask_contrast);
  mask1 = clamp( correctionFactorMask * (mask1 - 0.5) + 0.5 , 0 , 1 );
  mask2 = clamp( correctionFactorMask * (mask2 - 0.5) + 0.5 , 0 , 1 );
  mask3 = clamp( correctionFactorMask * (mask3 - 0.5) + 0.5 , 0 , 1 );
  mask4 = clamp( correctionFactorMask * (mask4 - 0.5) + 0.5 , 0 , 1 );
  mask5 = clamp( correctionFactorMask * (mask5 - 0.5) + 0.5 , 0 , 1 );
  mask6 = clamp( correctionFactorMask * (mask6 - 0.5) + 0.5 , 0 , 1 );

  mask3 = clamp( 0.7 * mask3 , 0 , 1 );
  mask4 = clamp( 0.8 * mask4 , 0 , 1 );

  const float mask_threshold = 0.35;
  mask1 = (mask1 > mask_threshold + uniform_Update_fs_4fv_weights03[0] ? 1.0 : 0.0 );
  mask2 = (mask2 > mask_threshold + uniform_Update_fs_4fv_weights03[1] ? 1.0 : 0.0 );
  mask3 = (mask3 > mask_threshold + uniform_Update_fs_4fv_weights03[2] ? 1.0 : 0.0 );
  mask4 = (mask4 > mask_threshold + uniform_Update_fs_4fv_weights03[3] ? 1.0 : 0.0 );
  mask5 = (mask5 > mask_threshold + uniform_Update_fs_2fv_weights45[0] ? 1.0 : 0.0 );
  mask6 = (mask6 > mask_threshold + uniform_Update_fs_2fv_weights45[1] ? 1.0 : 0.0 );

  // keeps the first non nul mask the other ones are null
  float maskCumul = mask1;
  mask2 = (mask2 <= maskCumul ? 0. : mask2);
  maskCumul += mask2;
  mask3 = (mask3 <= maskCumul ? 0. : mask3);
  maskCumul += mask3;
  mask4 = (mask4 <= maskCumul ? 0. : mask4);
  maskCumul += mask4;
  mask5 = (mask5 <= maskCumul ? 0. : mask5);
  maskCumul += mask5;
  mask6 = (mask6 <= maskCumul ? 0. : mask6);

  // vec4 weights03 = uniform_Update_fs_4fv_weights03;
  vec4 weights03 = uniform_Update_fs_4fv_weights03 * 0.01 + vec4(1);
  // vec2 weights45 = uniform_Update_fs_2fv_weights45;
  vec2 weights45 = uniform_Update_fs_2fv_weights45 * 0.01 + vec2(1);


  vec3 imageBufferColor = vec3(0);
  vec2 coordsImage = vec2(decalCoordsPOT.x,(1-decalCoordsPOT.y) * 0.75) ;
  vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * (photo_scale - 1) / photo_scale;

  // masked pixels according to random masks
  if(currentScene != 7 && currentScene != 8) {
    vec2 coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[0].xy  / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels0 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.x)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap0 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.x) * (mask1) * weights03[0];

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[0].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels1 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.y)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap1 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.y) * (mask2) * (weights03[1]); 

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[1].xy / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels2 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.z)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap2 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.z) * (mask3) * (weights03[2]); 

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[1].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels3 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.x)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap3 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.x) * (mask4) * (weights03[3]); 

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[2].xy / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels4 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.y)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap4 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.y) * (mask5) * (weights45[0]); 

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[2].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    // vec2 coord = coordsImageScaled;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels5 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.z)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap5 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.z) * (mask6) * (weights45[1]); 
  }
  // darkest pixel for gbagbo
  else {
    vec2 coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[0].xy  / uniform_Update_fs_4fv_photo01_wh.xy;
    imageBufferColor += (texture( uniform_Update_texture_fs_TVWPixels0 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.x)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap0 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.x);
    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[0].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    vec3 pixel = (texture( uniform_Update_texture_fs_TVWPixels1 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.y)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap1 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.y); 
    if(graylevel(pixel) < graylevel(imageBufferColor))
       imageBufferColor = pixel;


    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[1].xy / uniform_Update_fs_4fv_photo01_wh.xy;
    pixel =  (texture( uniform_Update_texture_fs_TVWPixels2 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap02.z)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap2 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap02.z); 
    if(graylevel(pixel) < graylevel(imageBufferColor))
       imageBufferColor = pixel;

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[1].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    pixel =  (texture( uniform_Update_texture_fs_TVWPixels3 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.x)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap3 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.x); 
    if(graylevel(pixel) < graylevel(imageBufferColor))
       imageBufferColor = pixel;

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[2].xy / uniform_Update_fs_4fv_photo01_wh.xy;
    pixel =  (texture( uniform_Update_texture_fs_TVWPixels4 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.y)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap4 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.y) ; 
    if(graylevel(pixel) < graylevel(imageBufferColor))
       imageBufferColor = pixel;

    coord = coordsImageScaled + uniform_Update_fs_4fv_image_noisesxy[2].zw / uniform_Update_fs_4fv_photo01_wh.xy;
    // vec2 coord = coordsImageScaled;
    pixel =  (texture( uniform_Update_texture_fs_TVWPixels5 , coord ).rgb
                         * (1.0 - uniform_Update_fs_3fv_alphaSwap35.z)
                       + texture( uniform_Update_texture_fs_TVWPixelsSwap5 , coord ).rgb
                         * uniform_Update_fs_3fv_alphaSwap35.z) ; 
    if(graylevel(pixel) < graylevel(imageBufferColor))
       imageBufferColor = pixel;
  }

  // image value
  float flashValueCoef = ((flashCameraTrkWght <= flashCameraTrkThreshold) ?
                  (1.0 - flashCameraTrkWght/flashCameraTrkThreshold): 0.0);
  if( flashCameraTrkWght <= 0 ) {
    flashValueCoef = 1.0f;
  }
  if( currentPhotoTrack == 1 ) {
    out_track1_FBO.rgb 
      = clamp(imageBufferColor * (vec3(photo_value) + photo_value_pulse * pulse * flashValueCoef) , 0 , 1 );
    // out_track1_FBO.rgb = imageBufferColor;

    // photo_satur photo_satur_pulse
    vec3 pulsed_satur = vec3(photo_satur) + photo_satur_pulse * pulse;
    //  public-domain function by Darel Rex Finley
    float  powerColor = sqrt( (out_track1_FBO.r)*(out_track1_FBO.r) * .299 +
                               (out_track1_FBO.g)*(out_track1_FBO.g) * .587 +
                               (out_track1_FBO.b)*(out_track1_FBO.b) * .114 ) ;
    out_track1_FBO.r = clamp( powerColor 
      + (out_track1_FBO.r - powerColor) * pulsed_satur.r , 0 , 1 );
    out_track1_FBO.g = clamp( powerColor 
      + (out_track1_FBO.g - powerColor) * pulsed_satur.g , 0 , 1 );
    out_track1_FBO.b = clamp( powerColor 
      + (out_track1_FBO.b - powerColor) * pulsed_satur.b , 0 , 1 );

    // image contrast
    //  from http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/
    float correctionFactor =  1.016 * (photo_contrast + 1.0) 
                                / (1.016 - photo_contrast);
    out_track1_FBO.rgb = clamp( vec3(correctionFactor)
      * (out_track1_FBO.rgb - vec3(0.5)) + vec3(0.5) , 0 , 1 );

    // for camera view of drawing
    if( cameraWeight > 0 ) {
      // the video black parts stamp the background moiré
      out_track1_FBO *= vec4( clamp( vec3(1)-videocolor , 0.0 , 1.0 ) ,  1.0 );

      // checking the camera before launching
      // all the other scenes are with currentVideoTrack == 1
      if(currentVideoTrack == 0) {
        out_track1_FBO += vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
      }
    }
  }

  trackColorCopy = uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght[0] * out_track1_FBO.rgb;
  // flash track 1 on CA
  if( graylevel(out_track1_FBO.rgb) > .2 && graylevel(out_track1_FBO.rgb) < 2.1
    && flashCameraTrkWght == 0 ) { // no flash video
    trackCACopy = uniform_Update_fs_4fv_flashTrkCAWghts[1] * vec4(imageBufferColor.rgb,graylevel(out_track1_FBO.rgb));
  }

  ///////////////////////////////////////////////////
  // PARTICLE FLASHES ON CA & TRACKS
  // copies particle rendering at preceding step (for particle flashes)
  vec4 out_particlesRendering 
    = texture( uniform_Update_texture_fs_Part_render , decalCoords );

  // particle flash on CA
  trackCACopy += uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.z
                  * vec4(out_particlesRendering.rgb,graylevel(out_particlesRendering.rgb));
  // particle flash on BG track
  trackColorCopy += uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght.w 
                      * out_particlesRendering.rgb;
#endif

  ////////////////////////////////////////////////////////////////////////////////////
  // TRACK 0: drawing
  ////////////////////////////////////////////////////////////////////////////////////
  // TRACK 0
  // background drawing
  if( drawingStroke > 0 && currentDrawingTrack == 0 ) {
    background_drawing_opacity 
      += out_gray_drawing( uniform_Update_fs_4fv_paths03_x[ 0 ] , 
			  uniform_Update_fs_4fv_paths03_y[ 0 ] , 
			  uniform_Update_fs_4fv_paths03_x_prev[ 0 ] , 
			  uniform_Update_fs_4fv_paths03_y_prev[ 0 ] ,
			  uniform_Update_fs_4fv_paths03_RadiusX[ 0 ] ,
			  uniform_Update_fs_4fv_paths03_RadiusY[ 0 ] ,
        int(uniform_Update_fs_4fv_paths03_BrushID[0]),
			  drawingStroke )  
         * uniform_Update_fs_4fv_paths03_a[0]  // brush opacity is combined with color opacity
         * Cursor;   // -1 for rubber and +1 for Stylus

    // drawing occurs on background
    if( background_drawing_opacity > 0 ) {
      // newtrack = 0;
      out_localColor_FBO.rgb = 
      	out_localColor_FBO.rgb * clamp((1 - background_drawing_opacity),0,1)
      	+ background_drawing_opacity
      	* vec3(uniform_Update_fs_4fv_paths03_r[0], 
          uniform_Update_fs_4fv_paths03_g[0], 
          uniform_Update_fs_4fv_paths03_b[0]);

      out_localColor_FBO = clamp( out_localColor_FBO , 0.0 , 1.0 );
    }
  }
  

  ////////////////////////////////////////////////////////////////////////////////////
  // CA LAYER
  ////////////////////////////////////////////////////////////////////////////////////

  // flash background track on CA
  if( flashCameraTrkWght == 0 ) { // no flash video 
    trackCACopy += uniform_Update_fs_4fv_flashTrkCAWghts[0] * vec4(out_localColor_FBO.rgb,graylevel(out_localColor_FBO.rgb));
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // CA LAYER: CA "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
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
      // currentCA.rgb = out_track1_FBO.rgb;
      // currentCA.rgb = vec3(1,0,0);
    }

    if(flashCameraTrkWght > 0 && flashCameraTrkWght >= flashCameraTrkThreshold) {
      currentCA.a = -1;
      currentCA.rgb = videocolor;
    }

     // calculates the new state of the automaton
    CA_out( currentCA );

    // outputs the color from CA (average surrounding color if birth)
    out_CA_FBO = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_Color_flashCABGWght.w == 0
  out_localColor_FBO.rgb = clamp( out_localColor_FBO.rgb 
            + uniform_Update_fs_4fv_repop_Color_flashCABGWght.w * out_CA_FBO.rgb , 0 , 1 );

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: PARTICLE "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( pixel_mode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH PIXEL: increase radius for pixel capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.y > 0 ) {
      pixel_radius = 1;
    }

    pixel_out();

    // arrival of a new pixel
    if( graylevel(out_color_pixel) > 0 ) {
      // outputs the color from pixel move at drawing layer
      out_localColor_FBO.rgb = out_color_pixel;
      out_speed_position_FBO 
	     = vec4( out_speed_pixel , out_position_pixel );
    }
    // update of the current pixel
    else {
      //  modifies speed according to acceleration
        vec2 pixel_acceleration;
      // FLAT
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseScale * 100 ),
                                  snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc > 0 ) {
      	// acceleration
      	out_speed_position_FBO.xy += pixel_acc * acceleration;
      }
      else {
      	// damping
      	out_speed_position_FBO.xy += pixel_acc * out_speed_position_FBO.xy;
      }
      // updates the position of the current pixel
      out_speed_position_FBO.zw += out_speed_position_FBO.xy; 

      // if the pixel leaves the cell, it is erased
      if( abs( out_speed_position_FBO.z ) > 0.5
      	  || abs( out_speed_position_FBO.w ) > 0.5 ) {
      	if( pixel_mode == PARTICLE_UNIQUE ) {
      	  out_localColor_FBO.rgb = vec3(0,0,0);
      	}
      }
    }
  }

  // PARTICLE "ADDITION"
  if( repop_BG > 0 ) {
    // if the pixel noise is equal to frame % 10000 the cell is repopulated with a pixel
    if( int(frameNo*repop_BG) % 10000 == int(randomCA.x * 21.5
        + randomCA.y * 462.0
        + randomCA.z * 8000.0 ) ) {
        out_localColor_FBO.rgb 
        = uniform_Update_fs_4fv_repop_Color_flashCABGWght.rgb;
    }
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRWING AND CA LAYERS: GRAY LEVEL DECAY
  //////////////////////////////////////
  //////////////////////////////////////

  // track flashing on the background level
  out_localColor_FBO.rgb = clamp(out_localColor_FBO.rgb + trackColorCopy, 0 , 1);

  // BACKGROUND TRACK 0 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_localColor_FBO.rgb) > 0 ) {
    out_localColor_FBO.rgb = out_localColor_FBO.rgb 
      - vec3(trkDecay[0]);
  }
  out_localColor_FBO.rgb 
    = clamp( out_localColor_FBO.rgb , 0.0 , 1.0 );

  //////////////////////////////////////////////
  // rebuilds output for the gray/drawing buffer after decay
  // has been taken into consideration
  if( graylevel(out_localColor_FBO.rgb) == 0 ) {
    // in case color has faded out or pen has never written
    // returns black
    out_speed_position_FBO = vec4(0,0,0,0);
    out_localColor_FBO.rgb= vec3(0,0,0);
  }
  // clear layer
  if( uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.x > 0 ) {
    out_speed_position_FBO = vec4(0,0,0,0);
    out_localColor_FBO.rgb= vec3(0,0,0);
  }

#if PG_NB_TRACKS >= 2
  // TRACK 1 DECAY
  // incay or decay does not make sense on a black color
  // if( graylevel(out_track1_FBO.rgb) > 0 ) {
  //   out_track1_FBO.rgb = out_track1_FBO.rgb - vec3(uniform_Update_fs_4fv_trackdecay_clearLayer[ 1 ],
		// 				   uniform_Update_fs_4fv_trackdecay_clearLayer[ 1 ],uniform_Update_fs_4fv_trackdecay_clearLayer[ 1 ]);
  // }
  out_track1_FBO.rgb 
    = clamp( out_track1_FBO.rgb , 0.0 , 1.0 );
  // clear layer
#endif

  // CA LAYER CLEAR
  if( uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift.y > 0 ) {
      out_CA_FBO = vec4(0);
  }

  if( flashCameraTrkWght > 0 ) { // flash video
    if(flashCameraTrkWght >= flashCameraTrkThreshold) {
      // video image copy when there is a flash video
      out_track1_FBO.rgb = videocolor;
    }
    else {
      out_track1_FBO.rgb = out_track1_FBO.rgb;
    }
    out_localColor_FBO.rgb = vec3(0);
  }


  // FBO OUTPUTS
  if( !freeze ) {
    out_Update_FBO_fs_Trk0 = out_localColor_FBO;
    out_Update_FBO_fs_Pixels = out_speed_position_FBO;
  }
  else {
    out_Update_FBO_fs_Trk0 = out_localColor_FBO;
    // out_Update_FBO_fs_Trk0 = texture( uniform_Update_texture_fs_Trk0 , decalCoords );
    out_Update_FBO_fs_Pixels = out_speed_position_FBO;
    // out_Update_FBO_fs_Pixels = texture( uniform_Update_texture_fs_Pixels , decalCoords );
  }

  out_Update_FBO_fs_CA = out_CA_FBO;

#if PG_NB_TRACKS >= 2
  if( !freeze ) {
    out_Update_FBO_fs_Trk1 = out_track1_FBO;
  }
  else {
    out_Update_FBO_fs_Trk1 = texture( uniform_Update_texture_fs_Trk1 , decalCoords );
  }
  // out_Update_FBO_fs_Trk1.rgb = out_Update_FBO_fs_Trk1.rgb * 0.1 + cameraImage * 0.9;
#endif

/*
  vec2 rectCoord = decalCoordsPOT * vec2(width,height);
  vec2 coord = coordsImageScaled;
  
  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels0, coord).rgb, 1);
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels1, coord).rgb, 1);
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels2, coord).rgb, 1);
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels3, coord).rgb, 1);
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels4, coord).rgb, 1);
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels5, coord).rgb, 1);
  }
  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap0, coord).rgb, 1);
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap1, coord).rgb, 1);
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap2, coord).rgb, 1);
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap3, coord).rgb, 1);
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap4, coord).rgb, 1);
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y >= 100 && rectCoord.y < 200) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap5, coord).rgb, 1);
  }
  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask1), 1);
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask2), 1);
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask3), 1);
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask4), 1);
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask5), 1);
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y >= 200 && rectCoord.y < 300) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask6), 1);
  }

  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask1), 1);
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask2), 1);
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask3), 1);
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask4), 1);
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask5), 1);
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y >= 0 && rectCoord.y < 100) {
    out_Update_FBO_fs_Trk1 = vec4( vec3(mask6), 1);
  }

  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels0, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap02.x);
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels1, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap02.y);
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels2, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap02.z);
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels3, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap35.x);
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels4, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap35.y);
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y >= 300 && rectCoord.y < 400) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixels5, coord).rgb, 1) * (1.0 - uniform_Update_fs_3fv_alphaSwap35.z);
  }
  if(rectCoord.x >=0 && rectCoord.x < 100 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap0, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap02.x;
  }
  if(rectCoord.x >=100 && rectCoord.x < 200 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap1, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap02.y;
  }
  if(rectCoord.x >=200 && rectCoord.x < 300 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap2, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap02.z;
  }
  if(rectCoord.x >=300 && rectCoord.x < 400 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap3, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap35.x;
  }
  if(rectCoord.x >=400 && rectCoord.x < 500 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap4, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap35.y;
  }
  if(rectCoord.x >=500 && rectCoord.x < 600 && rectCoord.y >= 400 && rectCoord.y < 500) {
    out_Update_FBO_fs_Trk1 = vec4( texture( uniform_Update_texture_fs_TVWPixelsSwap5, coord).rgb, 1) * uniform_Update_fs_3fv_alphaSwap35.z;
  }
  */
}
