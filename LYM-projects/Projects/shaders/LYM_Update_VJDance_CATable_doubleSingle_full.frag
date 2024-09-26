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

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),
                               vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiamond[4]=vec4[4](vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),
                                    vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec2 pixelTextureCoordinatesPOT_XY; // the POT coordinates of the
// pixel texture + z offset according to the chosen texture
vec2 noisepixels;

vec4 randomCA = vec4(0);
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
uniform vec4 uniform_Update_fs_4fv_CAseed_type_size_loc;

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
layout (binding = 20) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA

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
      /* out4_CA.a = randomCA.r * (nbStates+1); // nbStates states randomly */
      // out4_CA.rgb = averageSurrounding.rgb;
      
      // special treatment for generation, otherwise
      // it does not start from flashCA
      if( CAType == CA_TOTALISTIC ) { 
        int nbStates = nbStatesTotalistic[CASubType];
        float newState = clamp(randomCA.r * (nbStates+1),1,nbStates); // nbStates states randomly
        out4_CA.a = newState;
        out4_CA.rgb = currentCA.rgb;
        out4_CA.rgb = averageSurrounding.rgb;
      }
      else if( CAType == CA_GENERATION ) { 
        int nbStates = nbStatesGeneration[CASubType];
        float newState = clamp(randomCA.r * (nbStates+1),1,nbStates); // nbStates states randomly
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
        newState = floor(randomCA.r * (nbStatesGeneralBinary+1)); // nbStates states randomly
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
      // newState = int(randomCA.r * (nbStates+1)); // nbStates states randomly
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
      newState = clamp(randomCA.r * (3+1),0,3); // nbStates states randomly
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
          = texture( uniform_Update_texture_fs_Pixels,
               newDecalCoord );
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
  // CA or BG "REPOPULATION"
  if( BG_CA_repop_density >= 0 && (repop_CA > 0 || repop_BG > 0)) {
        repop_density_weight = texture(uniform_Update_texture_fs_RepopDensity,decalCoords).r;
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
  vec3 clipOriginal = vec3( 0.0 );
  vec2 coordsImage = vec2( 0.0 );
  vec2 coordsImageScaled = vec2( 0.0 );
  int currentPhotoSource = 0;
  int currentClipSource = 0;
  float clip0_x_crop = 0.;
  float clip0_y_shift = 0.;
  float clip0_scale = 1.;
  float clip1_x_crop = 0.;
  float clip1_y_shift = 0.;
  float clip1_scale = 1.;
  float full_clip_bottom_crop = 0.;
  float full_clip_top_crop = 1.;
  float full_clip_left_crop = 0.;
  float full_clip_right_crop = 1.;
  float right_clip_shift = 0.;
  float left_clip_shift = 0.;
  float midScreen = 0.5;
  // 1 full scale right
  if(screenLayout == 1) {
    clip0_x_crop = 0.;
    clip0_y_shift = 0.;
    clip0_scale = 1.;
    clip1_x_crop = 0.;
    clip1_y_shift = 0.;
    clip1_scale = 1.;
    full_clip_bottom_crop = 0.;
    full_clip_top_crop = 1.;
    full_clip_left_crop = 0.;
    full_clip_right_crop = 1.;
    left_clip_shift = 1.;
    right_clip_shift = 0.;
    midScreen = 0.;
  }
  // 0 full scale left
  else if(screenLayout == 0) {
    clip0_x_crop = 0.;
    clip0_y_shift = 0.;
    clip0_scale = 1.;
    clip1_x_crop = 0.;
    clip1_y_shift = 0.;
    clip1_scale = 1.;
    full_clip_bottom_crop = 0.;
    full_clip_top_crop = 1.;
    full_clip_left_crop = 0.;
    full_clip_right_crop = 1.;
    left_clip_shift = 0.;
    right_clip_shift = 1.;
    midScreen = 1.;
  }
  // two full scale (hoizontal with vertical split)
  else if(screenLayout == 2) {
    clip0_x_crop = 0.25;
    clip0_y_shift = 0.;
    clip0_scale = 1.;
    clip1_x_crop = 0.25;
    clip1_y_shift = 0.;
    clip1_scale = 1.;
    full_clip_bottom_crop = 0.;
    full_clip_top_crop = 1.;
    full_clip_left_crop = 0.;
    full_clip_right_crop = 1.;
    left_clip_shift = 0.;
    right_clip_shift = 0.5;
    midScreen = 0.5;
  }
  // 2 half scale
  else if(screenLayout == 3) {
    clip0_x_crop = 0.;
    clip0_y_shift = 0.25;
    clip0_scale = 2.;
    clip1_x_crop = 0.;
    clip1_y_shift = 0.25;
    clip1_scale = 2.;
    full_clip_bottom_crop = 0.25;
    full_clip_top_crop = 0.75;
    full_clip_left_crop = 0.;
    full_clip_right_crop = 1.;
    left_clip_shift = 0.;
    right_clip_shift = 0.5;
    midScreen = 0.5;
  }
  // 4 half scale
  else if(screenLayout == 4) {
    clip0_x_crop = 0.;
    clip0_y_shift = 0.;
    clip0_scale = 2.;
    clip1_x_crop = 0.;
    clip1_y_shift = 0.5;
    clip1_scale = 2.;
    full_clip_bottom_crop = 0.;
    full_clip_top_crop = 1.;
    full_clip_left_crop = 0.;
    full_clip_right_crop = 1.;
    left_clip_shift = 0.;
    right_clip_shift = 0.5;
    midScreen = 0.5;
  }
  // two half scale (vertical with horizontal split)
  else if(screenLayout == 5) {
    clip0_x_crop = 0.;
    clip0_y_shift = 0.;
    clip0_scale = 2.;
    clip1_x_crop = 0.;
    clip1_y_shift = 0.5;
    clip1_scale = 2.;
    full_clip_bottom_crop = 0.;
    full_clip_top_crop = 1.;
    full_clip_left_crop = 0.25;
    full_clip_right_crop = 0.75;
    left_clip_shift = 0.25;
    right_clip_shift = 1.;
    midScreen = 0.75;
  }
  // two full scale (hoizontal with vertical split)
  // black strips top and bottom
  vec2 photo_scale = vec2(1);
  photo_scale = vec2(photo_scaleX, photo_scaleY);
  if(decalCoordsPOT.y >= full_clip_bottom_crop && decalCoordsPOT.y < full_clip_top_crop
     && decalCoordsPOT.x >= full_clip_left_crop && decalCoordsPOT.x < full_clip_right_crop) {
    // left clip top
    if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x > 0 
      && decalCoordsPOT.x < midScreen) {
      if((screenLayout != 4 && screenLayout != 5) || decalCoordsPOT.y > 0.5) {
        coordsImage = vec2(decalCoordsPOT.x - left_clip_shift + clip0_x_crop , (1 - decalCoordsPOT.y) - clip0_y_shift) * clip0_scale * uniform_Update_fs_4fv_photo01_wh.xy + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
        // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.y - coordsImage.y;
        coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
        // coordsImageScaled = coordsImage;
        photoOriginal = texture(uniform_Update_texture_fs_Photo0, coordsImageScaled ).rgb;
        photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x * photoOriginal;
        currentPhotoSource = 1;
      }
    }
    // right clip top
    if(photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y > 0 
      && decalCoordsPOT.x >= midScreen) {
      if((screenLayout != 4 && screenLayout != 5) || decalCoordsPOT.y > 0.5) {
        coordsImage = vec2(decalCoordsPOT.x - right_clip_shift + clip0_x_crop , (1 - decalCoordsPOT.y) - clip0_y_shift) * clip0_scale * uniform_Update_fs_4fv_photo01_wh.zw + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
        // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.w - coordsImage.y;
        coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
        // coordsImageScaled = coordsImage;
        photoOriginal = texture(uniform_Update_texture_fs_Photo1, coordsImageScaled ).rgb;
        photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y * photoOriginal;
        currentPhotoSource = 2;
      }
    }
    // left clip bottom
    if(photoWeight * uniform_Update_fs_2fv_clip01Wghts.x > 0 
      && decalCoordsPOT.x < midScreen) {
      // black strims top and bottom
      if((screenLayout != 4 && screenLayout != 5) || decalCoordsPOT.y < 0.5) {
        coordsImage = vec2(decalCoordsPOT.x - left_clip_shift + clip1_x_crop , (1 - decalCoordsPOT.y) - clip1_y_shift) * clip1_scale * uniform_Update_fs_4fv_photo01_wh.xy + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
        // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.y - coordsImage.y;
        coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
        // coordsImageScaled = coordsImage;
        clipOriginal = texture(uniform_Update_texture_fs_Clip0, coordsImageScaled ).rgb;
        photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.x * clipOriginal;
        currentClipSource = 1;
      }
    }
    // right clip bottom
    if(photoWeight * uniform_Update_fs_2fv_clip01Wghts.y > 0 
      && decalCoordsPOT.x >= midScreen) {
      // black strims top and bottom
      if((screenLayout != 4 && screenLayout != 5) || decalCoordsPOT.y < 0.5) {
        coordsImage = vec2(decalCoordsPOT.x - right_clip_shift + clip1_x_crop , (1 - decalCoordsPOT.y) - clip1_y_shift) * clip1_scale * uniform_Update_fs_4fv_photo01_wh.zw + uniform_Update_fs_4fv_flashPhotoTrkWght_flashPhotoTrkThres_Photo_offSetsXY.zw;
        // coordsImage.y = uniform_Update_fs_4fv_photo01_wh.w - coordsImage.y;
        coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
        // coordsImageScaled = coordsImage;
        clipOriginal = texture(uniform_Update_texture_fs_Clip1, coordsImageScaled ).rgb;
        photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.y * clipOriginal;
        currentClipSource = 2;
      }
    }
  }

  // Sobel on photo
  if( photoSobel > 0 ) {
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
        }
        switch(currentClipSource) {
          case 1:
            samplerClipSobel = texture(uniform_Update_texture_fs_Clip0, coordsImageScaled + offsetsSobelPOT[i]).rgb;
            break;
          case 2:
            samplerClipSobel = texture(uniform_Update_texture_fs_Clip1, coordsImageScaled + offsetsSobelPOT[i]).rgb;
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
      photocolor = vec3(0);
      switch(currentPhotoSource) {
        case 1:
          photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.x * photo_sobel_result;
          break;
        case 2:
          photocolor += photoWeight * uniform_Update_fs_4fv_photo01Wghts_randomValues.y * photo_sobel_result;
          break;
      }
      switch(currentClipSource) {
        case 1:
          photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.x * clip_sobel_result;
          break;
        case 2:
          photocolor += photoWeight * uniform_Update_fs_2fv_clip01Wghts.y * clip_sobel_result;
          break;
      }
  }
  photocolor *= vec3(photo_value);
  // color inversion
  if( invertPhoto) {
      photocolor = vec3(1) - photocolor;
  }
  // video_satur
  //  public-domain function by Darel Rex Finley
  if(photo_satur >= 0) {
    float  powerColor = sqrt( (photocolor.r)*(photocolor.r) * .299 +
                               (photocolor.g)*(photocolor.g) * .587 +
                               (photocolor.b)*(photocolor.b) * .114 ) ;
    photocolor = clamp( powerColor 
      + (photocolor - vec3(powerColor)) * photo_satur , 0 , 1 );
  }
  if( graylevel(photocolor) < photo_threshold ) {
    photocolor = vec3(0.0);
  }

    // photo_gamma
  // Gamma correction on photo (mainly blck and white)
  if(photo_gamma != 1) 
  {
    photocolor.r = pow( photocolor.r, photo_gamma);
    photocolor.g = pow( photocolor.g, photo_gamma);
    photocolor.b = pow( photocolor.b, photo_gamma);
  }

  // image_threshold 
  if(photo_threshold > 0) 
  {
    photocolor.r = (photocolor.r > photo_threshold? photocolor.r : 0);
    photocolor.g = (photocolor.g > photo_threshold? photocolor.g : 0);
    photocolor.b = (photocolor.b > photo_threshold? photocolor.b : 0);
  }

  // image contrast
  //  from http://www.dfstudios.co.uk/articles/programming/image-programming-algorithms/image-processing-algorithms-part-5-contrast-adjustment/
  float correctionFactor =  1.016 * (photo_contrast + 1.0) 
                              / (1.016 - photo_contrast);
  photocolor = clamp( vec3(correctionFactor)
    * (photocolor - vec3(0.5)) + vec3(0.5) , 0 , 1 );

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
  // cameraCoord = vec2((decalCoordsPOT.x), (1 - decalCoordsPOT.y) )
  cameraCoord = vec2((decalCoordsPOT.x), (1 - decalCoordsPOT.y) )
              // added for wide angle lens that covers more than the drawing surface
               * cameraWH; + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
               * movieWH;

  // image reading
  cameraOriginal = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
  cameraImage = cameraOriginal;
  // cameraOriginal = vec3(1) - cameraOriginal;
  // gamma correction
  #ifdef var_camera_gamma
  if(camera_gamma != 1) {
    cameraImage = vec3( pow(cameraImage.r,camera_gamma) , pow(cameraImage.g,camera_gamma) , pow(cameraImage.b,camera_gamma) );
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
  if(movie_gamma != 1) {
    movieImage = vec3( pow(movieImage.r,movie_gamma) , pow(movieImage.g,movie_gamma) , pow(movieImage.b,movie_gamma) );
  }
  if( graylevel(movieImage) <= movie_threshold ) {
      movieImage = vec3(0.0);
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
  float videoWeight = 0;
  #ifdef var_cameraWeight
  videocolor += cameraWeight * cameraImage;
  videoWeight += cameraWeight;
  #endif
  #ifdef var_movieWeight
  videocolor += movieWeight * movieImage;
  videoWeight += movieWeight;
#endif

  // video_satur
  //  public-domain function by Darel Rex Finley
  if(video_satur >= 0) {
    float  powerColor = sqrt( (videocolor.r)*(videocolor.r) * .299 +
                               (videocolor.g)*(videocolor.g) * .587 +
                               (videocolor.b)*(videocolor.b) * .114 ) ;
    videocolor = clamp( powerColor 
      + (videocolor - vec3(powerColor)) * video_satur , 0 , 1 );
  }

  // video_gamma
  // Gamma correction on video (mainly blck and white)
  if(video_gamma != 1) 
  {
    videocolor.r = pow( videocolor.r, video_gamma);
    videocolor.g = pow( videocolor.g, video_gamma);
    videocolor.b = pow( videocolor.b, video_gamma);
  }

  // image_threshold 
  if(video_threshold > 0) 
  {
    videocolor.r = (videocolor.r > video_threshold? videocolor.r : 0);
    videocolor.g = (videocolor.g > video_threshold? videocolor.g : 0);
    videocolor.b = (videocolor.b > video_threshold? videocolor.b : 0);
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
    if( currentVideoTrack == indCurTrack && videoWeight > 0) {
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
    if( currentVideoTrack == indCurTrack 
        && flashCameraTrkWght > 0 
        && graylevel(cameraOriginal) > flashCameraTrkWght ) { // flash camera
        // video image copy when there is a flash video
      flashToCACumul.rgb = cameraImage;
      flashToBGCumul.rgb = cameraImage;
      flashToCACumul.a = 1;
    }

    /////////////////
    // TRACK photo
    if(currentPhotoTrack == indCurTrack 
      && photoWeight * (uniform_Update_fs_4fv_photo01Wghts_randomValues.x 
        + uniform_Update_fs_4fv_photo01Wghts_randomValues.y 
        + uniform_Update_fs_2fv_clip01Wghts.x 
        + uniform_Update_fs_2fv_clip01Wghts.y) > 0 ) {
      // only photo (but not drawing or whatever memory from preceding tracks)
      if(!videoOn) {
        out_track_FBO[indCurTrack].rgb = clamp( photocolor , 0.0 , 1.0 );
      }
      // cumul video + photo
      else {
        out_track_FBO[indCurTrack].rgb
          = clamp( videocolor + photocolor , 0.0 , 1.0 );
      }
    }
    if( currentPhotoTrack == indCurTrack 
        && flashPhotoTrkWght > 0 
        && graylevel(photocolor) > flashPhotoTrkWght ) { // flash photo
      // photo image copy when there is a flash photo
      flashToCACumul.rgb = photocolor;
      flashToCACumul.a = 1;
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

     // calculates the new state of the automaton
    CA_out( currentCA );


    // CA "REPOPULATION"
    if( repop_CA > 0 ) {
      if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.z), repop_CA * repop_density_weight) != 0) {
        out4_CA.a = -1.0;
        out4_CA.rgb  = uniform_Update_fs_3fv_repop_ColorCA.xyz;
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
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesPOT_XY , noiseUpdateScale * 100 ),
                                snoise( pixelTextureCoordinatesPOT_XY + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));
      // }

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
    if( rand3D(vec3(decalCoordsPOT, uniform_Update_fs_4fv_photo01Wghts_randomValues.w), repop_BG * repop_density_weight) != 0) {
        out_track_FBO[0].rgb = uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.xyz;
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
  // CA LAYER DECAY
  // the first frame for CA is negative so that it is not drawn
  // in this case its value is preserved as it is
  // and incay or decay does not make sense on a black color
  if( out_attachment_FBO[pg_FBO_fs_CA_attacht].a > 0 ) {
    vec3 newCA_w_decay = out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb;
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
