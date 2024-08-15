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
const uint CA_DISLOCATION   = 0;
const uint CA_CYCLIC_1      = 1;
const uint CA_PROTOCELLS    = 2;
const uint GOL_1            = 3;
const uint CA_CYCLIC        = 4;
const uint CA_PREY          = 5;
const uint CA_SOCIAL_PD     = 6;

// CA OFFSETS
const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},              // E NE N NW
                                 {1,1},{-1,1},{-1,-1},{1,-1}};           // W SW S SE
const vec2 neighborOffsetsDiamond[4] = {{1,0},{-1,0},{0,1},{0,-1}};  // W WW S SS
const float neighborDistances[8]={1.0,0.707,1.0,0.707,1.0,0.707,1.0,0.707};


const float shadePrey[5] = {0,0.4,              // dead predator/prey
                                 0.6, 0.8,  1.0};       // prey hungry/full predator
const float inverseShadePrey[5] = {0.0,2.5,              // dead predator/prey
                                 1.67,1.25,  1.0};       // prey hungry/full predator
const vec3 shadeCyclic[6] = {{208/255.f,57/255.f,98/255.f},{62/255.f,125/255.f,218/255.f},{252/255.f,249/255.f,50/255.f},
                            {250/255.f,135/255.f,50/255.f},{129/255.f,166/255.f,61/255.f},{100/255.f,100/255.f,100/255.f}};

const float shadeProtocell[4] = {0,               // empty
                                 0.4,0.8,         // X Y
                                 1.0};            // Z

const float shadeDisloc[5] = {0,0.4,              // empty new/old border
                                 0.6, 0.8,  1.0};       // new/old nucleus
const float inverseShadeDisloc[5] = {0.0,2.5,              // empty new/old border
                                 1.67,1.25,  1.0};       // new/old nucleus
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
const uint DRAWING_BEZIER = 1;

////////////////////////////////////////////////////////////////////
// MACROS
#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))
#define delta(gray) (gray>0.5?1:0)

ivec3 orderedRank;
vec3 orderedColor;
void oderedChannels(vec3 col) {
  float tmp;
  orderedRank = ivec3(0,1,2);
  orderedColor = col;
  // if (orderedColor[0] > orderedColor[2]) {
  if (orderedColor.r > orderedColor.b) {
     // swap(orderedColor[0], orderedColor[2]);
    orderedRank.rgb = orderedRank.bgr;
    orderedColor.rgb = orderedColor.bgr;
  }

  // if (orderedColor[0] > orderedColor[1]) {
  if (orderedColor.r > orderedColor.g) {
    // swap(orderedColor[0], orderedColor[1]);
    orderedRank.rgb = orderedRank.grb;
    orderedColor.rgb = orderedColor.grb;
  }

  //Now the smallest element is the 1st one. Just check the 2nd and 3rd

  // if (orderedColor[1] > orderedColor[2]) {
  if (orderedColor.g > orderedColor.b) {
    // swap(orderedColor[1], orderedColor[2]);
    orderedRank.rgb = orderedRank.rbg;
    orderedColor.rgb = orderedColor.rbg;
  }
}

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
vec2 offsetsSobelPOT[9] = {vec2( -1./2048., 1./2048. ), vec2( 0, 1./2048. ), vec2( 1./2048., 1./2048. ),
                        vec2( 0, -1./2048. ), vec2( 0, 1./2048. ),
                        vec2( -1./2048., -1./2048. ),vec2( 0, -1./2048. ),vec2( 1./2048., -1./2048. ), vec2( 0, 0 )};
float sobelMatrixX[9] = {-1,0,1,-2,0,2,-1,0,1};
float sobelMatrixY[9] = {1,2,1,0,0,0,-1,-2,-1};
float sobelMatrixX_0[9] = {-1,0,1,-2,2,-1, 0, 1, 0};
float sobelMatrixY_0[9] = { 1,2,1, 0,0,-1,-2,-1, 0};

////////////////////////////////////
// CA UPDATE
vec4 out4_CA;
vec4 noiseCA = vec4(0);

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),
                               vec4(0),vec4(0),vec4(0),vec4(0));
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

///////////////////////////////////////
// photo or clip images
vec3 photoOriginal = vec3( 0.0 );
vec3 clipOriginal = vec3( 0.0 );


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

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect   uniform_Update_texture_fs_CA;            // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
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
// layout (binding = 18) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA
layout (binding = 18) uniform samplerRect  uniform_Update_texture_fs_Camera_BGIni; // initial background camera texture
layout (binding = 19) uniform samplerRect  uniform_Update_texture_fs_pixel_acc;     // image for pixel acceleration

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

float payoffCalc( float state1 , float state2) {
  if(state1==1.0) {
    if (state2==1.0) {
      return CAParams[1];
    }
    else {
      return CAParams[3];
    }
  }
  else {
    if (state2==1.0) {
      return CAParams[2];
    }
    else {
      return CAParams[4];
    }
  }
}

void CA_out( vec4 currentCA ) {
    out4_CA = clamp(vec4(currentCA.rgb-vec3(CAdecay),currentCA.a),0,1); // ready
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PIXELS UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void pixel_out( void ) {

  out_position_pixel  = vec2(0,0);
    out_color_pixel = vec3(0,0,0);
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
  return 0.f;
}


#if defined(var_photoSobel)
// Sobel on photo or clip
void Sobel(int currentPhotoSource, int currentClipSource, vec2 coordsImageScaled) {
}
#endif

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN OF UPDATE PASS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void main() {
#include_initializations


  //////////////////////////////////////////////
  //////////////////////////////////////////////
  // FBO OUTPUTS 
  //////////////////////////////////////////////
  //////////////////////////////////////////////
  out_Update_FBO_fs_CA = vec4(0);
  out_Update_FBO_fs_Pixels = vec4(0);
  out_Update_FBO_fs_Trk0 = vec4(0);
#if PG_NB_TRACKS >= 2
  out_Update_FBO_fs_Trk1 = vec4(0);
#endif
#if PG_NB_TRACKS >= 3
  out_Update_FBO_fs_Trk2 = vec4(0);
#endif
#if PG_NB_TRACKS >= 4
  out_Update_FBO_fs_Trk3 = vec4(0);
#endif
}
