/***********************************************************************
File: song/shaders/LYM_Update_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define var_CAcolorSpread
bool	  CAcolorSpread;
#define var_CAParams
float	 CAParams[9];
#define var_CAstep
int		CAstep;
#define var_camera_gamma
float	 camera_gamma;
#define var_cameraCumul
int		cameraCumul;
#define var_cameraGamma
float	 cameraGamma;
#define var_cameraSobel
float	 cameraSobel;
#define var_cameraThreshold
float	 cameraThreshold;
#define var_cameraWeight
float	 cameraWeight;
#define var_invertCamera
bool	  invertCamera;
#define var_invertMovie
bool	  invertMovie;
#define var_movie_gamma
float	 movie_gamma;
#define var_movie_threshold
float	 movie_threshold;
#define var_movieSobel
float	 movieSobel;
#define var_movieWeight
float	 movieWeight;
#define var_invertPhoto
bool	  invertPhoto;
#define var_photo_color_balance_r
float	 photo_color_balance_r;
#define var_photo_color_balance_g
float	 photo_color_balance_g;
#define var_photo_color_balance_b
float	 photo_color_balance_b;
#define var_photo_contrast
float	 photo_contrast;
#define var_photo_gamma
float	 photo_gamma;
#define var_photo_hue
float	 photo_hue;
#define var_photo_rot
float	 photo_rot;
#define var_photo_satur
float	 photo_satur;
#define var_photo_scaleX
float	 photo_scaleX;
#define var_photo_scaleY
float	 photo_scaleY;
#define var_photo_threshold
float	 photo_threshold;
#define var_photo_transl_x
float	 photo_transl_x;
#define var_photo_transl_y
float	 photo_transl_y;
#define var_photo_value
float	 photo_value;
#define var_photoSobel
float	 photoSobel;
#define var_photoWeight
float	 photoWeight;
#define var_video_contrast
float	 video_contrast;
#define var_video_gamma
float	 video_gamma;
#define var_video_hue
float	 video_hue;
#define var_video_satur
float	 video_satur;
#define var_video_threshold
float	 video_threshold;
#define var_video_value
float	 video_value;
#define var_video_white
float	 video_white;
#define var_noiseType
int		noiseType;
#define var_noiseLineScale
float	 noiseLineScale;
#define var_noiseAngleScale
float	 noiseAngleScale;
#define var_noiseCenterX
float	 noiseCenterX;
#define var_noiseCenterY
float	 noiseCenterY;
#define var_path_replay_trackNo
int		path_replay_trackNo[12];
#define var_Pixelstep
int		Pixelstep;
#define var_noiseUpdateScale
float	 noiseUpdateScale;
#define var_pixel_acc
float	 pixel_acc;
#define var_pixel_acc_shiftX
float	 pixel_acc_shiftX;
#define var_pixel_acc_shiftY
float	 pixel_acc_shiftY;
#define var_pixel_image_acceleration
int		pixel_image_acceleration;
#define var_pixel_mode
int		pixel_mode;
#define var_pixel_radius
float	 pixel_radius;
#define var_BG_CA_repop_color_mode
int		BG_CA_repop_color_mode;
#define var_BG_CA_repop_density
int		BG_CA_repop_density;
#define var_repop_BG
float	 repop_BG;
#define var_repop_CA
float	 repop_CA;
#define var_camera_BG_subtr
bool	  camera_BG_subtr;
#define var_freeze
bool	  freeze;
#define var_screenLayout
int		screenLayout;
#define var_CAdecay
float	 CAdecay;
#define var_trkDecay
float	 trkDecay[4];
#define var_currentDrawingTrack
int		currentDrawingTrack;
#define var_currentPhotoTrack
int		currentPhotoTrack;
#define var_currentVideoTrack
int		currentVideoTrack;
#define var_fft_scale
float	 fft_scale;
uniform float uniform_Update_scenario_var_data[86];

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
layout (binding = 18) uniform samplerRect  uniform_Update_texture_fs_Camera_BGIni; // initial background camera texture
layout (binding = 19) uniform samplerRect  uniform_Update_texture_fs_pixel_acc;     // image for pixel acceleration
// layout (binding = 20) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA

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
      // CCA with autotmatic cycling from one step to the next
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
      out4_CA.rgb = uniform_Update_fs_3fv_repop_ColorCA.rgb;
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
/*     float CAParams[1] = 0.480315;
    float CAParams[2] = 0.511811;
    float CAParams[3] = 0.637795;
    float CAParams[4] = 0.472441;
    float CAParams[5] = 0.244094;
 */
    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    // we choose a random value according between 0 & nbStates
    if( currentCA.a < 0 ) {
      out4_CA.a = randomCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      newState = 3;
      out4_CA.rgb = shadeProtocell[newState] * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams[1]: px - X duplication from X+Y (randomCA.x)
      // CAParams[2]: py - Y duplication from Y+Z (randomCA.y)
      // CAParams[3]: pz - Z duplication from Z+X (randomCA.z)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(randomCA.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && randomCA.y < CAParams[1]) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(randomCA.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && randomCA.w < CAParams[2]) {
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
                && randomCA2.y < CAParams[3]) {
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
      // CAParams[5]: px - X/Y/Z degradation (randomCA.w)
      // randomCA2.x choice of a first neighbor
      // degradation or migration of X
      else if( randomCA.x < CAParams[4] 
        || nbNeighbors < 8 * CAParams[5] ) { 
        newState = 0; 
      }

      out4_CA.a = float(newState);
      // state change: takes the color of the current stage
      if( currentCA.a != out4_CA.a ) {
        out4_CA.rgb = shadeProtocell[newState] * currentCA.rgb;
#ifdef var_Song_CA_color_mode
        if(Song_CA_color_mode) {
          out4_CA.rgb = shadeProtocell[newState] * vec3(1,0,0);
        }
#endif
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
  // DISLOCATION
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_DISLOCATION ) {
    // CAParams optimal values from effe
/*     float CAParams[1] = 0.393673;
    float CAParams[2] = 0.102449;
    float CAParams[3] = 0.53551;
    float CAParams[4] = 0.165306;
    float CAParams[5] = 0.196939;
    float CAParams[6] = 0.322857;
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
                && randomCA.y < CAParams[2] ) {
          // prolongs the border
          newState = 2; // old border
        }
        else if( nbSurroundingNewBorders >= CAParams[1] * 8.0  ) {
          // creates a nucleus
          newState = 3; // nucleus
        }
      }
      // new border growth: new border pushed by old border
      else if( state ==  1 ) {
        newState = 1; // stays a border
        if( nbSurroundingNewBorders >= CAParams[5] * 10.0 ) {
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
        if( nbSurroundingOldBorders >= CAParams[6] * 10.0 ) {
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
             >= CAParams[4] * 16.0 ) {
          newState = 0; //makes a hole
        }
        else 
          if( randomCA.y < CAParams[3] ) {
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
  // PREY/PREDATOR
  else if( CAType == CA_PREY ) {
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
               < CAParams[1] ) {
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
               < CAParams[2] ) {
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
               < CAParams[3] ) {
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
               < CAParams[3] ) {
          newState = 4; // survives in state full
        }
        else { // dies with a certain probability
          if( randomCA.x < CAParams[4] ) {
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
        if( randomCA.x < CAParams[5] ) {
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
    //     . CAParams[1] Cooperation
    //     . CAParams[2] Defect (with Coop)
    //     . CAParams[3] Coop (with Defect)
    //     . CAParams[4] double defect
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
        if( sumPayoff > 24 * CAParams[5] ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          if( randomCA.z > CAParams[6] ) { // ****** payoffCalc uses parameters 1 to 4
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
        if( maxPayoff > 24 * CAParams[5] ) { // ****** payoffCalc uses parameters 1 to 4
          newState = int(neighborValues[indMax].a);
        }
        else {
          // alive and adopts opposite behavior
          if( neighborValues[indMax].a > 0 ) {
              if( randomCA.z > CAParams[6] ) { // ****** payoffCalc uses parameters 1 to 4
                newState = 3 - int(neighborValues[indMax].a);
              }
              // or stays in its current state
          }
          //dead
          else {
              if( randomCA.z > CAParams[7] ) {
                newState = int(neighborValues[indMax].a);
              }
              // or stays dead
          }
        }
      }
      // VARIANT 4 : adopts behavior of most successful neighbor if it is above a certain minimal value
      // or the opposite behavior with a given probability
      else if( CASubType == 4 ) {
        if( sumPayoff > 24 * CAParams[5] ) {
          if( randomCA.z > CAParams[6] ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
                newState = 3 - int(neighborValues[indMax].a);
          }
        }
        else {
          if( randomCA.z > 1 - CAParams[7] ) {
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
        //           && abs(surrpixel_nextPosition.y) <= 0.5 ) 
        vec2 radiusNoise = 2.0 * vec2(snoise( pixelTexCoordLocPOT , noiseUpdateScale * 100 ) ,
                                  snoise( pixelTexCoordLocPOT + vec2(2.0937,9.4872) , 100 )); // 
        if( length(surrpixel_nextPosition) <= 0.5 ) {
          out_color_pixel += surrpixel_localColor.rgb;
          out_speed_pixel += surrpixel_speed;
          // computes the position of the pixel
          out_position_pixel += surrpixel_nextPosition;
          nb_cumultated_pixels++;
        }
        // radius pixel extension for (S,N,E,W) neighbors
        else if( abs(surrpixel_nextPosition.x) <= (pixel_radius * radiusNoise.x)
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius * radiusNoise.y)) {
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
        vec2 radiusNoise = 2.0 * vec2(snoise( pixelTexCoordLocPOT , noiseUpdateScale * 100 ) ,
                                  snoise( pixelTexCoordLocPOT + vec2(2.0937,9.4872) , 100 )); // 
        if( abs(surrpixel_nextPosition.x) <= (pixel_radius * radiusNoise.x)
                  && abs(surrpixel_nextPosition.y) <= (pixel_radius * radiusNoise.y) ) {
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


#if defined(var_photoSobel)
// Sobel on photo or clip
void Sobel(int currentPhotoSource, int currentClipSource, vec2 coordsImageScaled) {
  if( photoSobel > 0 && ((currentPhotoSource > 0) || (currentClipSource > 0))) {
    vec3 samplerSobel;
    // sobel
    vec3 sobelX = vec3(0.0);
    vec3 sobelY = vec3(0.0);
    vec3 samplerClipSobel;
    // sobel
    vec3 sobelClipX = vec3(0.0);
    vec3 sobelClipY = vec3(0.0);

    // samples the center pixel and its Moore neighborhood
    for( int i = 0 ; i < 9 ; i++ ) {
      switch(currentPhotoSource) {
        case 1:
          samplerSobel = texture(uniform_Update_texture_fs_Photo0, coordsImageScaled + offsetsSobelPOT[i]).rgb;
          break;
        case 2:
          samplerSobel = texture(uniform_Update_texture_fs_Photo1, coordsImageScaled + offsetsSobelPOT[i]).rgb;
          break;
        default:
          break;
      }
      switch(currentClipSource) {
        case 1:
          samplerClipSobel = texture(uniform_Update_texture_fs_Clip0, coordsImageScaled + offsetsSobelPOT[i]).rgb;
          break;
        case 2:
          samplerClipSobel = texture(uniform_Update_texture_fs_Clip1, coordsImageScaled + offsetsSobelPOT[i]).rgb;
          break;
        default:
          break;
      }
      if(i < 8) {
        sobelX += sobelMatrixX_0[i] * samplerSobel;
        sobelY += sobelMatrixY_0[i] * samplerSobel;
        sobelClipX += sobelMatrixX_0[i] * samplerClipSobel;
        sobelClipY += sobelMatrixY_0[i] * samplerClipSobel;
      }
    }

    samplerSobel = photoOriginal;
    sobelX = mix( samplerSobel , sobelX , photoSobel );
    sobelY = mix( samplerSobel , sobelY , photoSobel );
    samplerClipSobel = clipOriginal;
    sobelClipX = mix( samplerClipSobel , sobelClipX , photoSobel );
    sobelClipY = mix( samplerClipSobel , sobelClipY , photoSobel );

    vec3 photo_sobel_result = clamp( sqrt( sobelX * sobelX + sobelY * sobelY ) , 0.0 , 1.0 );
    vec3 clip_sobel_result = clamp( sqrt( sobelClipX * sobelClipX + sobelClipY * sobelClipY ) , 0.0 , 1.0 );
    if(currentPhotoSource > 0) {
        photoOriginal = photo_sobel_result;
    }
    if(currentClipSource > 0) {
        clipOriginal = clip_sobel_result;
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// MAIN OF UPDATE PASS
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void main() {
  CAcolorSpread = (uniform_Update_scenario_var_data[0] > 0 ? true : false);
  CAParams[1] = (uniform_Update_scenario_var_data[1]);
  CAParams[2] = (uniform_Update_scenario_var_data[2]);
  CAParams[3] = (uniform_Update_scenario_var_data[3]);
  CAParams[4] = (uniform_Update_scenario_var_data[4]);
  CAParams[5] = (uniform_Update_scenario_var_data[5]);
  CAParams[6] = (uniform_Update_scenario_var_data[6]);
  CAParams[7] = (uniform_Update_scenario_var_data[7]);
  CAParams[8] = (uniform_Update_scenario_var_data[8]);
  CAstep = int(uniform_Update_scenario_var_data[9]);
  camera_gamma = uniform_Update_scenario_var_data[10];
  cameraCumul = int(uniform_Update_scenario_var_data[11]);
  cameraGamma = uniform_Update_scenario_var_data[12];
  cameraSobel = uniform_Update_scenario_var_data[13];
  cameraThreshold = uniform_Update_scenario_var_data[14];
  cameraWeight = uniform_Update_scenario_var_data[15];
  invertCamera = (uniform_Update_scenario_var_data[16] > 0 ? true : false);
  invertMovie = (uniform_Update_scenario_var_data[17] > 0 ? true : false);
  movie_gamma = uniform_Update_scenario_var_data[18];
  movie_threshold = uniform_Update_scenario_var_data[19];
  movieSobel = uniform_Update_scenario_var_data[20];
  movieWeight = uniform_Update_scenario_var_data[21];
  invertPhoto = (uniform_Update_scenario_var_data[22] > 0 ? true : false);
  photo_color_balance_r = uniform_Update_scenario_var_data[23];
  photo_color_balance_g = uniform_Update_scenario_var_data[24];
  photo_color_balance_b = uniform_Update_scenario_var_data[25];
  photo_contrast = uniform_Update_scenario_var_data[26];
  photo_gamma = uniform_Update_scenario_var_data[27];
  photo_hue = uniform_Update_scenario_var_data[28];
  photo_rot = uniform_Update_scenario_var_data[29];
  photo_satur = uniform_Update_scenario_var_data[30];
  photo_scaleX = uniform_Update_scenario_var_data[31];
  photo_scaleY = uniform_Update_scenario_var_data[32];
  photo_threshold = uniform_Update_scenario_var_data[33];
  photo_transl_x = uniform_Update_scenario_var_data[34];
  photo_transl_y = uniform_Update_scenario_var_data[35];
  photo_value = uniform_Update_scenario_var_data[36];
  photoSobel = uniform_Update_scenario_var_data[37];
  photoWeight = uniform_Update_scenario_var_data[38];
  video_contrast = uniform_Update_scenario_var_data[39];
  video_gamma = uniform_Update_scenario_var_data[40];
  video_hue = uniform_Update_scenario_var_data[41];
  video_satur = uniform_Update_scenario_var_data[42];
  video_threshold = uniform_Update_scenario_var_data[43];
  video_value = uniform_Update_scenario_var_data[44];
  video_white = uniform_Update_scenario_var_data[45];
  noiseType = int(uniform_Update_scenario_var_data[46]);
  noiseLineScale = uniform_Update_scenario_var_data[47];
  noiseAngleScale = uniform_Update_scenario_var_data[48];
  noiseCenterX = uniform_Update_scenario_var_data[49];
  noiseCenterY = uniform_Update_scenario_var_data[50];
  path_replay_trackNo[1] = int(uniform_Update_scenario_var_data[51]);
  path_replay_trackNo[2] = int(uniform_Update_scenario_var_data[52]);
  path_replay_trackNo[3] = int(uniform_Update_scenario_var_data[53]);
  path_replay_trackNo[4] = int(uniform_Update_scenario_var_data[54]);
  path_replay_trackNo[5] = int(uniform_Update_scenario_var_data[55]);
  path_replay_trackNo[6] = int(uniform_Update_scenario_var_data[56]);
  path_replay_trackNo[7] = int(uniform_Update_scenario_var_data[57]);
  path_replay_trackNo[8] = int(uniform_Update_scenario_var_data[58]);
  path_replay_trackNo[9] = int(uniform_Update_scenario_var_data[59]);
  path_replay_trackNo[10] = int(uniform_Update_scenario_var_data[60]);
  path_replay_trackNo[11] = int(uniform_Update_scenario_var_data[61]);
  Pixelstep = int(uniform_Update_scenario_var_data[62]);
  noiseUpdateScale = uniform_Update_scenario_var_data[63];
  pixel_acc = uniform_Update_scenario_var_data[64];
  pixel_acc_shiftX = uniform_Update_scenario_var_data[65];
  pixel_acc_shiftY = uniform_Update_scenario_var_data[66];
  pixel_image_acceleration = int(uniform_Update_scenario_var_data[67]);
  pixel_mode = int(uniform_Update_scenario_var_data[68]);
  pixel_radius = uniform_Update_scenario_var_data[69];
  BG_CA_repop_color_mode = int(uniform_Update_scenario_var_data[70]);
  BG_CA_repop_density = int(uniform_Update_scenario_var_data[71]);
  repop_BG = uniform_Update_scenario_var_data[72];
  repop_CA = uniform_Update_scenario_var_data[73];
  camera_BG_subtr = (uniform_Update_scenario_var_data[74] > 0 ? true : false);
  freeze = (uniform_Update_scenario_var_data[75] > 0 ? true : false);
  screenLayout = int(uniform_Update_scenario_var_data[76]);
  CAdecay = uniform_Update_scenario_var_data[77];
  trkDecay[0] = (uniform_Update_scenario_var_data[78]);
  trkDecay[1] = (uniform_Update_scenario_var_data[79]);
  trkDecay[2] = (uniform_Update_scenario_var_data[80]);
  trkDecay[3] = (uniform_Update_scenario_var_data[81]);
  currentDrawingTrack = int(uniform_Update_scenario_var_data[82]);
  currentPhotoTrack = int(uniform_Update_scenario_var_data[83]);
  currentVideoTrack = int(uniform_Update_scenario_var_data[84]);
  fft_scale = uniform_Update_scenario_var_data[85];

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
  vec2 coordsImage = vec2( 0.0 );
  vec2 coordsImageScaled = vec2( 0.0 );
  int currentPhotoSource = 0;
  int currentClipSource = 0;
  vec2 photo_scale = vec2(1);
  photo_scale = vec2(photo_scaleX, photo_scaleY);
  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x > 0) {
    coordsImage = vec2(decalCoordsPOT.x , 1 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.xy 
      + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
    // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.y - coordsImage.y;
    coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - vec2(1)) / photo_scale;
    // coordsImageScaled = coordsImage;
    photoOriginal = texture(uniform_Update_texture_fs_Photo0, coordsImageScaled ).rgb;
    // color inversion
#if defined(var_photoSobel)
    Sobel(1,-1, coordsImageScaled);
#endif
    if( invertPhoto) {
        photoOriginal = vec3(1) - photoOriginal;
    }
    photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x * photoOriginal;
  }
  else if(photoWeight * uniform_Update_fs_2fv_clip01Wghts.x > 0) {
    coordsImage = vec2(decalCoordsPOT.x , 1 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.xy + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
    // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.y - coordsImage.y;
    coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - vec2(1)) / photo_scale;
    // coordsImageScaled = coordsImage;
    clipOriginal = texture(uniform_Update_texture_fs_Clip0, coordsImageScaled ).rgb;
    // color inversion
#if defined(var_photoSobel)
    Sobel(-1,1, coordsImageScaled);
#endif
   if( invertPhoto) {
        clipOriginal = vec3(1) - clipOriginal;
    }
    photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.x * clipOriginal;
  }

  if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y > 0) {
    coordsImage = vec2(decalCoordsPOT.x , 1 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.zw 
      + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
    // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.w - coordsImage.y;
    coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - vec2(1)) / photo_scale;
    // coordsImageScaled = coordsImage;
    photoOriginal = texture(uniform_Update_texture_fs_Photo1, coordsImageScaled ).rgb;
    // color inversion
#if defined(var_photoSobel)
    Sobel(2, -1, coordsImageScaled);
#endif
    if( invertPhoto) {
        photoOriginal = vec3(1) - photoOriginal;
    }
    photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y * photoOriginal;
  }
  else if(photoWeight * uniform_Update_fs_2fv_clip01Wghts.y > 0) {
    coordsImage = vec2(decalCoordsPOT.x , 1 - decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.zw + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
    // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.w - coordsImage.y;
    coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - vec2(1)) / photo_scale;
    // coordsImageScaled = coordsImage;
    clipOriginal = texture(uniform_Update_texture_fs_Clip1, coordsImageScaled ).rgb;
    // color inversion
#if defined(var_photoSobel)
    Sobel(-1, 2, coordsImageScaled);
#endif
    if( invertPhoto) {
        clipOriginal = vec3(1) - clipOriginal;
    }
    photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.y * clipOriginal;
  }

#if defined(var_photo_value)
  photocolor *= vec3(photo_value);
#endif

  // photo_satur
  //  public-domain function by Darel Rex Finley
#if defined(var_photo_satur)
  if(photo_satur >= 0) {
    float  powerColor = sqrt( (photocolor.r)*(photocolor.r) * .299 +
                               (photocolor.g)*(photocolor.g) * .587 +
                               (photocolor.b)*(photocolor.b) * .114 ) ;
    photocolor = clamp( powerColor 
      + (photocolor - vec3(powerColor)) * photo_satur , 0 , 1 );
  }
#endif

  // photo_threshold
#if defined(var_photo_threshold)
  if( graylevel(photocolor) < photo_threshold ) {
    photocolor = vec3(0.0);
  }
#endif

  // photo_gamma
  // Gamma correction on photo (mainly blck and white)
#if defined(var_photo_gamma)
  if(photo_gamma != 1) 
  {
    float gamma = max(photo_gamma, 0.01);
    photocolor.r = pow( photocolor.r, gamma);
    photocolor.g = pow( photocolor.g, gamma);
    photocolor.b = pow( photocolor.b, gamma);
  }
#endif

  // image_threshold 
#if defined(var_photo_threshold)
  if(photo_threshold > 0) {
    photocolor.r = (photocolor.r > photo_threshold? photocolor.r : 0);
    photocolor.g = (photocolor.g > photo_threshold? photocolor.g : 0);
    photocolor.b = (photocolor.b > photo_threshold? photocolor.b : 0);
  }
#endif

  // image contrast
  //  from http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/
#if defined(var_photo_contrast)
  if(photo_contrast > 0) {
    float correctionFactor =  1.016 * (photo_contrast + 1.0) 
                                / (1.016 - photo_contrast);
    photocolor = clamp( vec3(correctionFactor)
      * (photocolor - vec3(0.5)) + vec3(0.5) , 0 , 1 );
  }
#endif

  // equalization
  photocolor *= vec3(photo_color_balance_r, photo_color_balance_g, photo_color_balance_b);

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
 
/* // FlowingForms
   cameraCoord = vec2(0.7 * (decalCoordsPOT.x + 0.15), 0.7 * (1. - decalCoordsPOT.y + 0.15) ) * cameraWH;
*/

  // cameraCoord = vec2((decalCoordsPOT.x), (1 - decalCoordsPOT.y) )
  cameraCoord = vec2((decalCoordsPOT.x), (1 - decalCoordsPOT.y) );
  // two images in case of double screen
/*  if(uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow.w != 0) {
    cameraCoord.x *= 2.;
    if(cameraCoord.x >= 1) {
      cameraCoord.x -= 1.;
    }
  }
*/
  // Zoom on camera
  #ifdef var_cameraZoom
    cameraCoord = (cameraCoord - vec2(0.5,0.5)) / cameraZoom +vec2(0.5,0.5);
  #endif
  // added for wide angle lens that covers more than the drawing surface
  cameraCoord =  cameraCoord * cameraWH; + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y );
  // two images in case of double screen
/*  if(uniform_Update_fs_4fv_frameno_Cursor_flashPartCAWght_doubleWindow.w != 0) {
    movieCoord.x *= 2.;
    if(movieCoord.x >= 1) {
      movieCoord.x -= 1.;
    }
  }
*/  // from PoT coordinates to movie texture coordina
  movieCoord *= movieWH;

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
#if defined(var_movie_gamma)
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
      // path drawing on current track
      else if( indPath > 0 && path_replay_trackNo[indPath] == indCurTrack ) {
        pathStroke = DRAWING_BEZIER;
      }

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
            //out_track_FBO[indCurTrack].rgb += curTrack_grayLevel * 0.1 * pathColor.rgb;
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
    if( currentVideoTrack == indCurTrack 
      && (videoWeight > 0 || flashCameraTrkWght > 0)) {
      out_track_FBO[indCurTrack] = vec4(vec3(0), 1.);
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
        videoOn = true;
        // video image copy when there is a flash video
        flashToCACumul.rgb = cameraImage;
        flashToBGCumul.rgb = cameraImage;
        flashToCACumul.a = 1;
      }
    }

    /////////////////
    // TRACK photo
    float photoCumulWgth = photoWeight * (uniform_Update_fs_4fv_photo01Wghts_randomValues.x 
        + uniform_Update_fs_4fv_photo01Wghts_randomValues.y 
        + uniform_Update_fs_2fv_clip01Wghts.x 
        + uniform_Update_fs_2fv_clip01Wghts.y);
    if(currentPhotoTrack == indCurTrack 
      && (photoCumulWgth > 0 || flashPhotoTrkWght > 0)) {
      if(!videoOn) {
        out_track_FBO[indCurTrack] = vec4(vec3(0), 1.);
      }
      // visible photo
      if(photoWeight * (uniform_Update_fs_4fv_photo01Wghts_randomValues.x 
        + uniform_Update_fs_4fv_photo01Wghts_randomValues.y 
        + uniform_Update_fs_2fv_clip01Wghts.x 
        + uniform_Update_fs_2fv_clip01Wghts.y) > 0 ) {
        // photo + video
        if(currentVideoTrack == indCurTrack) {
          // only photo no video
          if (!videoOn) {
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
  // if( CA_on_off && (frameNo % CAstep == 0) ) 
  {
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
      if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.z), repop_CA * repop_density_weight) != 0) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Update_fs_3fv_repop_ColorCA.xyz;
        if(BG_CA_repop_color_mode == 1) {
          out4_CA.rgb = textureDensityValue;
        }
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

    if(frameNo % Pixelstep == 0) {
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
    }
    else {
      // keeps previous step value of out_track_FBO[0].rgb
    }
  }

  // pixel "REPOPULATION"
  if( repop_BG > 0 ) {
    if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.w), repop_BG * repop_density_weight) != 0) {
        out_track_FBO[0].rgb = uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.xyz;
        // repopulation with the color of the density texture
        if(BG_CA_repop_color_mode == 1) {
          out_track_FBO[0].rgb = textureDensityValue;
        }
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
        // saturates in case of incay (instead of going to white)
        if(false) {
        // if(trkDecay[indTrack] < 0) {
          vec3 decayedColor = out_track_FBO[indTrack].rgb - vec3(trkDecay[indTrack]);
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
          out_track_FBO[indTrack].rgb = out_track_FBO[indTrack].rgb - vec3(trkDecay[indTrack]);
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
