/***********************************************************************
File: alKemi/shaders/LYM_Drawing_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 460

#define var_camera_BG_subtr
bool	  camera_BG_subtr;
#define var_CAdecay
float	 CAdecay;
#define var_trkDecay_0
float	 trkDecay_0;
#define var_trkDecay_1
float	 trkDecay_1;
#define var_trkDecay_2
float	 trkDecay_2;
#define var_trkDecay_3
float	 trkDecay_3;
#define var_currentDrawingTrack
int		currentDrawingTrack;
#define var_currentVideoTrack
int		currentVideoTrack;
#define var_currentPhotoTrack
int		currentPhotoTrack;
#define var_path_replay_trackNo_1
int		path_replay_trackNo_1;
#define var_path_replay_trackNo_2
int		path_replay_trackNo_2;
#define var_path_replay_trackNo_3
int		path_replay_trackNo_3;
#define var_path_replay_trackNo_4
int		path_replay_trackNo_4;
#define var_path_replay_trackNo_5
int		path_replay_trackNo_5;
#define var_path_replay_trackNo_6
int		path_replay_trackNo_6;
#define var_path_replay_trackNo_7
int		path_replay_trackNo_7;
#define var_path_replay_trackNo_8
int		path_replay_trackNo_8;
#define var_path_replay_trackNo_9
int		path_replay_trackNo_9;
#define var_path_replay_trackNo_10
int		path_replay_trackNo_10;
#define var_path_replay_trackNo_11
int		path_replay_trackNo_11;
#define var_noiseUpdateScale
float	 noiseUpdateScale;
#define var_pixel_acc
float	 pixel_acc;
#define var_pixel_acc_shiftX
float	 pixel_acc_shiftX;
#define var_pixel_acc_shiftY
float	 pixel_acc_shiftY;
#define var_pixel_radius
float	 pixel_radius;
#define var_pixel_mode
int		pixel_mode;
#define var_repop_CA
float	 repop_CA;
#define var_repop_BG
float	 repop_BG;
#define var_BG_CA_repop_density
int		BG_CA_repop_density;
#define var_BG_CA_repop_color_mode
int		BG_CA_repop_color_mode;
#define var_CAParams1
float	 CAParams1;
#define var_CAParams2
float	 CAParams2;
#define var_CAParams3
float	 CAParams3;
#define var_CAParams4
float	 CAParams4;
#define var_CAParams5
float	 CAParams5;
#define var_CAParams6
float	 CAParams6;
#define var_CAParams7
float	 CAParams7;
#define var_CAParams8
float	 CAParams8;
#define var_CAstep
int		CAstep;
#define var_CAcolorSpread
bool	  CAcolorSpread;
#define var_freeze
bool	  freeze;
#define var_video_white
float	 video_white;
#define var_Song_CA_color_mode
bool	  Song_CA_color_mode;
#define var_movieWeight
float	 movieWeight;
#define var_movieSobel
float	 movieSobel;
#define var_invertMovie
bool	  invertMovie;
#define var_invertCamera
bool	  invertCamera;
#define var_video_satur
float	 video_satur;
#define var_video_value
float	 video_value;
#define var_photo_satur
float	 photo_satur;
#define var_photo_value
float	 photo_value;
#define var_photo_gamma
float	 photo_gamma;
#define var_photo_threshold
float	 photo_threshold;
#define var_photoWeight
float	 photoWeight;
#define var_photo_scale
float	 photo_scale;
#define var_photo_rot
float	 photo_rot;
#define var_photo_transl_x
float	 photo_transl_x;
#define var_photo_transl_y
float	 photo_transl_y;
#define var_mask_scale
float	 mask_scale;
#define var_cameraCumul
int		cameraCumul;
#define var_cameraGamma
float	 cameraGamma;
#define var_camera_gamma
float	 camera_gamma;
#define var_camera_contrast
float	 camera_contrast;
#define var_cameraThreshold
float	 cameraThreshold;
#define var_cameraWeight
float	 cameraWeight;
#define var_cameraSobel
float	 cameraSobel;
#define var_photo_contrast
float	 photo_contrast;
#define var_mask_contrast
float	 mask_contrast;
#define var_pixel_image_acceleration
int		pixel_image_acceleration;
#define var_invertPhoto
bool	  invertPhoto;
#define var_movie_gamma
float	 movie_gamma;
#define var_movie_threshold
float	 movie_threshold;
#define var_video_gamma
float	 video_gamma;
#define var_video_threshold
float	 video_threshold;
#define var_video_contrast
float	 video_contrast;
#define var_photoSobel
float	 photoSobel;
#define var_photo_scaleX
float	 photo_scaleX;
#define var_photo_scaleY
float	 photo_scaleY;
#define var_photo_equalization
float	 photo_equalization;
#define var_Pixelstep
int		Pixelstep;
#define var_video_hue
float	 video_hue;
#define var_photo_hue
float	 photo_hue;
uniform float uniform_Update_scenario_var_data[82];

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;

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
#define CA_GOL                    0
#define CA_TOTALISTIC             1
#define CA_GENERATION             2
#define CA_GENERAL_BINARY_MOORE   3
#define CA_GENERAL_BINARY_NEUMANN 4
#define CA_NEUMANN_BINARY         5
#define CA_TUMOR_CELL             6
#define CA_WORM                   7
/* #define CA_MIXED          3 */

const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
                                 {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW
const vec2 oppositeNeighborOffsets[8] = {{-1,0},{1,0},{0,-1},{0,1},      // E W N S
                                         {-1,-1},{1,1},{-1,1},{1,-1},};  // NE SW SE NW

////////////////////////////////////////////////////////////////////
// pixel mode
const uint PIXEL_NONE = 0;
const uint PIXEL_UNIQUE = 1;
const uint PIXEL_FUR = 2;
const uint PIXEL_MULTIPLY = 3;

////////////////////////////////////////////////////////////////////
// drawing modes
const uint DRAWING_VOID = 0;
const uint DRAWING_LINE = 1;
const uint DRAWING_POINT = 2;
const uint DRAWING_BEZIER = 3;

////////////////////////////////////////////////////////////////////
// MACROS
#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

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
vec3 cameraImage = vec3(0.0);
vec3 videocolor = vec3(0.0);
vec3 photocolor = vec3(0.0);
vec2 cameraCoord = vec2(0.0);
vec3 movieImage = vec3(0.0);
vec2 movieCoord = vec2(0.0);
vec2 movieWH;

////////////////////////////////////
// CA UPDATE
vec4 out4_CA; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

vec4 noiseCA = vec4(0);

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),
                               vec4(0),vec4(0),vec4(0),vec4(0));


// pixel variables
float radius_pixel = 0.5;


vec2 pixelTextureCoordinatesXY; // the POT coordinates of the
// pixel texture 

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

uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  // *
uniform vec4 uniform_Update_fs_4fv_flashTrkCAWghts;  

uniform vec3 uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght;
uniform vec3 uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift;
uniform vec4 uniform_Update_fs_4fv_xy_transl_tracks_0_1;
uniform vec4 uniform_Update_fs_4fv_W_H_time_currentScene;
uniform vec4 uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack;
uniform vec4 uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght;
uniform vec3 uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres;
uniform vec4 uniform_Update_fs_4fv_photo01_wh;
uniform vec4 uniform_Update_fs_4fv_photo01Wghts_randomValues;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;
uniform vec2 uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect   uniform_Update_texture_fs_CA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect   uniform_Update_texture_fs_PreviousCA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 2) uniform samplerRect   uniform_Update_texture_fs_Pixels;   // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
layout (binding = 3) uniform sampler3D     uniform_Update_texture_fs_Brushes;  // pen patterns
layout (binding = 4) uniform samplerRect   uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 5) uniform samplerRect   uniform_Update_texture_fs_Camera_BG;     // camera BG texture
layout (binding = 6) uniform samplerRect   uniform_Update_texture_fs_Movie_frame;  // movie textures
layout (binding = 7) uniform sampler3D     uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 8)  uniform samplerRect  uniform_Update_texture_fs_RepopDensity;  // repop density texture
layout (binding = 9) uniform sampler2D     uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 10) uniform sampler2D    uniform_Update_texture_fs_Photo1;  // photo_1 texture
layout (binding = 11)  uniform sampler2D   uniform_Update_texture_fs_Clip0;  // second clip left texture
layout (binding = 12)  uniform sampler2D   uniform_Update_texture_fs_Clip1;  // second clip right texture
layout (binding = 13)  uniform samplerRect uniform_Update_texture_fs_Part_render;   // FBO capture of particle rendering
layout (binding = 14) uniform samplerRect  uniform_Update_texture_fs_Trk0;         // 3-cycle ping-pong localColor step n (FBO attachment 1)
#if PG_NB_TRACKS >= 2
layout (binding = 15) uniform samplerRect uniform_Update_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 16) uniform samplerRect uniform_Update_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 17) uniform samplerRect uniform_Update_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
layout (binding = 18) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA
layout (binding = 19) uniform samplerRect  uniform_Update_texture_fs_Camera_BGIni; // initial background camera texture
layout (binding = 20) uniform samplerRect  uniform_Update_texture_fs_pixel_acc;     // image for pixel acceleration

/////////////////////////////////////
// CA & TRACK COLOR OUTPUT
layout (location = 0) out vec4 out_Update_FBO_fs_CA;      // 
// the RGB channels contains the track Color

/////////////////////////////////////
// PIXEL OUTPUT
layout (location = 1) out vec4 out_Update_FBO_fs_Pixels;      // 
// the RG channels contain the pixel speed
// the BA channels contain the pixel position

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
    return vec2(snoise( texCoordLoc , noiseUpdateScale * 100 ),
                            snoise( texCoordLoc + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));
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

void CA_out( vec2 decalCoords , vec4 currentCAValue ) {
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
  if(/* CAType == CA_GOL || CAType == CA_TOTALISTIC || */ CAType == CA_GENERATION ) {
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
    
    #define nbStatesGeneration 25
    #define CArankGeneration 18

    /*
    uint nbStates = 16;
    uint CArank = 9;

    if( CAType == CA_TOTALISTIC && CASubType == 4 ) { // Wire number of states
      nbStates = 4;
    }
    if( CAType == CA_TOTALISTIC && (CASubType == 5 || CASubType == 8) ) { // Busy Brain and Brian's Brain number of states
      nbStates = 3;
    }

    if( CAType == CA_GOL ) { // GOL number of states + rank (height in data texture)
      nbStates = 2;
      CArank = 0;
    }

    if( CAType == CA_GENERATION ) { // CA generation number of states + rank (height in data texture)
      CArank = 18;
      nbStates = texelFetch(uniform_Update_texture_fs_CATable, 
                            ivec2( 0 , CArank + CASubType ) ).r;
    }
    */

    // The first CA alpha value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed 
    // then alpha represents the state of the automaton
    if( currentCAValue.a < 0 ) {
      /* out4_CA.a = noiseCA.r * (nbStates+1); // nbStates states randomly
      // out4_CA.rgb = averageSurrounding.rgb;
      
      // special treatment for generation, otherwise
      // it does not start from flashCA
      if( CAType == CA_GENERATION ) { */
        out4_CA.a = 1;
        out4_CA.rgb = vec3(1);//averageSurrounding.rgb;
      // }
    }
    else {
      float state = clamp(currentCAValue.a,0,nbStatesGeneration);
      float newState = 0;
      newState = texelFetch(uniform_Update_texture_fs_CATable, 
                       ivec2( int(state) * 10 + nbSurroundingLives + 1 , 
                              CArankGeneration + CASubType ) ).r;
      out4_CA.a = float(newState);
      if( newState > 0 ) {
        out4_CA.rgb = averageSurrounding.rgb;
        // out4_CA.rgb = vec3(1);
      }
      else {
        out4_CA.rgb = vec3(0,0,0);
      }
    }
  }

  ////////////////////////////////////////////////////// 
  // GENERAL BINARY MOORE NEIGHBORHOOD
  else if( CAType == CA_GENERAL_BINARY_MOORE ) {
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

    int nbSurroundingLives =
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

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    float state = clamp(currentCAValue.a,0,nbStatesGeneralBinary);
    float newState = 0;
    if( currentCAValue.a < 0 ) {
      newState = floor(noiseCA.r * (nbStatesGeneralBinary+1)); // nbStates states randomly
      out4_CA.a = nbStatesGeneralBinary;
    }
    else {
      // survival
      if( state > 0 ) {
         newState = texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( nbSurroundingLives + 1 , 38 + CASubType ) ).r;
         // survival
         if( newState > 0 && out4_CA.a >= 0.0 ) {
            out4_CA.a -= 1.0;
         }
      }
      // birth
      else {
        newState = texelFetch(uniform_Update_texture_fs_CATable, 
                       ivec2( 256 + nbSurroundingLives + 1 , 38 + CASubType ) ).r;
        // birth
        if( newState > 0 ) {
          out4_CA.a = nbStatesGeneralBinary;
        }
      }
    }
    if( out4_CA.a > 0 ) {
      out4_CA.rgb = vec3(1.0,1.0,1.0);
    }
    else {
      out4_CA.rgb = vec3(0,0,0);
    }
  }

  ////////////////////////////////////////////////////// 
  // NEUMANN BINARY VON NEUMANN NEIGHBORHOOD + CENTER
  else if( CAType == CA_NEUMANN_BINARY ) {
    // Fredkin2 rule has the following definition: 2,01101001100101101001011001101001
    // The first digit, '2', tells the rule has 2 states (it's a 1 bit rule).
    // The second digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=0 will get the state 0.
    // The third digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=1 will get the state 1.
    // The fourth digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=0 will get the state 1.
    // The fifth digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=1 will get the state 0.
    // . . .

    // const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S

    int nbSurroundingLives =
      (int(currentCAValue.a)) +  // CENTER
      (int(neighborValues[0].a) * 9) +  // E
      (int(neighborValues[1].a) * 81) +  // W
      (int(neighborValues[2].a) * 3) +  // N
      (int(neighborValues[3].a) * 27) ;  // S

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    float state = clamp(currentCAValue.a,0,3);
    float newState = 0;
    if( currentCAValue.a < 0 ) {
      newState = clamp(noiseCA.r * (3+1),0,3); // nbStates states randomly
      // newState = 2;
      out4_CA.a = float(newState);
    }
    else {
      newState = texelFetch(uniform_Update_texture_fs_CATable, 
                    ivec2( nbSurroundingLives + 1 , 50 + CASubType ) ).r * 255.0;
      // newState = texelFetch(uniform_Update_texture_fs_CATable, 
      //                       ivec2( nbSurroundingLives + 1 , 58 ) ).r;
      out4_CA.a = float(newState);
    }

    if( out4_CA.a > 0 ) {
      vec3 averageSurrounding = vec3(out4_CA.a/float(3-1)); //gatherSurroundingLives* 3 / nbSurroundingLives;
      out4_CA.rgb = vec3(1);
    }
    else {
      out4_CA.rgb = vec3(0);
    }
    // out4_CA = vec4(1.0,0.0,0.0,1.0);
  }

  //////////////////////////////////////////////////////
  // TUMOR CELL
  else if( CAType == CA_TUMOR_CELL ) { // CAType == 6
    // const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
    //                                  {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW

    // vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
    //                                vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

    // STATES:
    // 0: free
    // 1: normal cell, 2-9: normal cell candidate for subdivision 
    //                      in one of the 8 Moore neighborhoods
    // 11: cancer cell, 12-19: cancer cell candidate for subdivision 
    //                         in one of the 8 Moore neighborhoods
    // states 10 and 20 had been made for quiescent cell but they
    // are not necessary to deal with such cells (surrounded by living cells)

    #define nbStatesTumorCell 21
    uint state = int(clamp(currentCAValue.a,0,nbStatesTumorCell));
    out4_CA = currentCAValue;
    if( currentCAValue.a < 0 ) {
      // free cell generated by pen
      if( noiseCA.r < 0.6 ) {
        out4_CA = vec4(0.0,0.0,0.0,0.0);
      }
      // normal cell generated by pen
      else if( noiseCA.r < 0.6 + CAParams3 * 0.4 ) {
        out4_CA = vec4(1.0,1.0,1.0,1.0);
      }
      // cancer cell generated by pen
      else {
        out4_CA = vec4(1.0,1.0,1.0,11.0);
      }
    }
    else {
      // stores and count free neighbor cells
      int nbFreeNeighborCells = 0;
      int indFreeNeighborCells[8];
      int nbUsedNeighborCells = 0;
      int indUsedNeighborCells[8];
      for(int ind = 0 ; ind < 8 ; ind++ ) {
        if( neighborValues[ind].a == 0 ) {
          indFreeNeighborCells[nbFreeNeighborCells] = ind;
          nbFreeNeighborCells++;
        }
        else {
          indUsedNeighborCells[nbUsedNeighborCells] = ind;
          nbUsedNeighborCells++;
        }
      }

      // quiescent state does not have to be dealt with separately
      // (a state with 8 alive neighbors)

      // free slot: checks for neighboring cell subdivision
      if( state == 0 ) {
        // offsets and states are ranked accordingly
        // so as to guess easily the expected state from the
        // position of the offset 
        for(int ind = 0 ; ind < nbUsedNeighborCells ; ind++ ) {
          int indUsedNeighborCell = indUsedNeighborCells[ind];
          // if the used neighbor cell is in the state of subdivision
          // and if the subdivision location corresponds to the
          // current free cell (diametrically opposite)
          // normal cell candidate for subdivision at preceding step produces an offspring
          // at the current location
          if( neighborValues[indUsedNeighborCell].a == 2 + (7 - indUsedNeighborCell) ) {
            out4_CA = vec4(1.0,1.0,1.0,1.0);
          }
          // cancer cell candidate for subdivision at preceding step produces an offspring
          // at the current location
          if( neighborValues[indUsedNeighborCell].a == 12 + (7 - indUsedNeighborCell) ) {
            out4_CA = vec4(1.0,1.0,1.0,11.0);
          }
        }
      }
      // subdivision of a normal or cancer cell
      else if( nbFreeNeighborCells > 0 ) {
        // normal cell subdivision
        if( state == 1 ) {
           // subdivision
           if( noiseCA.r < CAParams1 ) { // 0.85 
              int indMigration = int(clamp(floor(noiseCA.b * nbFreeNeighborCells),
                                                  0.0,float(nbFreeNeighborCells)));
              out4_CA = vec4(1.0,1.0,1.0,2 + indFreeNeighborCells[indMigration]);
           }
           // ageing is made after update in main function
           // out4_CA.rgb = clamp( out4_CA.rgb - 10 * vec3(CAdecay) , 0.0 , 1.0 );
        }
        // cancer cell subdivision
        else if( state == 11 ) {
           // subdivision
           if( noiseCA.r < CAParams2 ) { // 0.9 
              int indMigration = int(clamp(floor(noiseCA.b * nbFreeNeighborCells),
                                                  0.0,float(nbFreeNeighborCells)));
              out4_CA = vec4(1.0,1.0,1.0,12 + indFreeNeighborCells[indMigration]);
           }
        }
        // the normal cell candidate for subdivision
        // returns to normal state
        else if( state >= 2 && state <= 9 ) {
          out4_CA.a = 1.0;
          // out4_CA.rgb = clamp( out4_CA.rgb - 10 * vec3(CAdecay) , 0.0 , 1.0 );
        }
        // the cancer cell candidate for subdivision
        // returns to normal state
        else if( state >= 12 && state <= 19 ) {
          out4_CA.a = 11.0;
        }
      }
    }

    if( out4_CA.a <= 0 ) {
      out4_CA.rgb = vec3(0.0,0.0,0.0);
    }
    // if( out4_CA.a > 0 ) {
    //   out4_CA = vec4(1.0,0.0,0.0,11.0);
    // }
  }

  /*
  ////////////////////////////////////////////////////// 
  // GENERAL BINARY VON NEUMANN NEIGHBORHOOD
  else if( CAType == CA_GENERAL_BINARY_NEUMANN ) {
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
    uint nbStates = texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( 0 , 46 + CASubType ) ).r;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    uint state = int(clamp(currentCAValue.a,0,nbStates));
    uint newState = 0;
    if( currentCAValue.a < 0 ) {
      newState = int(noiseCA.r * (nbStates+1)); // nbStates states randomly
      out4_CA.a = float(nbStates);
    }
    else {
      // survival
      if( state > 0 ) {
         newState = texelFetch(uniform_Update_texture_fs_CATable, 
                               ivec2( nbSurroundingLives + 1 , 46 + CASubType ) ).r;
         // survival
         if( newState > 0 && out4_CA.a >= 0.0 ) {
            out4_CA.a -= 1.0;
         }
      }
      // birth
      else {
        newState = texelFetch(uniform_Update_texture_fs_CATable, 
                       ivec2( 16 + nbSurroundingLives + 1 , 46 + CASubType ) ).r;
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
  */

  /*
  //////////////////////////////////////////////////////
  // TIME TUNNEL
  else if( CAType == CA_TIME_TUNNEL ) { // CAType == 6
    float previousGray =  texture(uniform_Update_texture_fs_CATable, decalCoords ).a;

    int nbSurroundingFiring =
      (currentCAValue.a > 0? 1:0) +
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0);
    bool fired = false;
    if( CASubType == 1 ) {
      fired = (nbSurroundingFiring > 0 && nbSurroundingFiring < 5);
    }
    else if( CASubType == 2 ) {
      fired = (nbSurroundingFiring > (0 + int(noiseCA.x * 2.0))
	       && nbSurroundingFiring < (5 - int(noiseCA.y * 2.0)) );
    }
    else if( CASubType == 3 ) {
      fired = (nbSurroundingFiring > (0 + int(noiseCA.x * 2.0))
	       && nbSurroundingFiring < (4 - int(noiseCA.y * 2.0)) );
    }
    if( fired ) {
      if( previousGray == 0.0 ) {
        vec4 gatherSurroundingLives =
      	  currentCAValue +
	       (neighborValues[0]) +
	       (neighborValues[1]) +
	       (neighborValues[2]) +
	       (neighborValues[3]);
        out4_CA = gatherSurroundingLives/nbSurroundingFiring; // ready
      }
      else {
        out4_CA = vec4(0.0); // ready
      }
    }
    else {
      out4_CA = vec4(0.0); // ready
    }
    return;
  }
*/

  /*
  //////////////////////////////////////////////////////
  // WORM
  else if( CAType == CA_WORM ) {
    // > 0.5 firing
    // > 0 && <= 0.5 refractory
    // 0 ready
    // ready -> firing  (w 2 neighbors firing)
    // firing -> rafractory
    // refractory -> ready
    float previousGray =  texture(uniform_Update_texture_fs_CATable, decalCoords ).a;
    int nbSurroundingFiring =
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0) +
      (neighborValues[4].a > 0? 1:0) +
      (neighborValues[5].a > 0? 1:0) +
      (neighborValues[6].a > 0? 1:0) +
      (neighborValues[7].a > 0? 1:0);

    /////////////
    // COLOR WORM
    if( CASubType == 1 ) {
      // alarm is off && many neighbors
      if( (nbSurroundingFiring == 2 + int( noiseCA.x * 2) 
	         || nbSurroundingFiring == 3 + int( noiseCA.y * 2) 
	         || nbSurroundingFiring == 5 + int( noiseCA.z * 2) )
	           && currentCAValue.a <= 0 ) {
        vec4 gatherSurroundingLives =
          (neighborValues[0]) +
          (neighborValues[1]) +
          (neighborValues[2]) +
          (neighborValues[3]) +
          (neighborValues[4]) +
          (neighborValues[5]) +
          (neighborValues[6]) +
          (neighborValues[7]);
	      out4_CA = clamp(gatherSurroundingLives/nbSurroundingFiring,0.0,1.0); // resets alarm
      }
      else {
        out4_CA = currentCAValue - 100.0 * vec4(CAdecay);
        if( out4_CA.a < 0.0 ) {
          out4_CA = vec4(0.0);
        }
      }
    }

    /////////////
    // B/W WORM
    else if( CASubType == 2 ) {
      // alarm is off && many neighbors
      if( (nbSurroundingFiring == 2 + int( noise.x * 2) 
	     || nbSurroundingFiring == 3 + int( noise.y * 2) 
	     || nbSurroundingFiring == 5 + int( noise.z * 2) )
	     && currentCAValue.a <= 0 ) {
	       out4_CA = vec4(1.0); // resets alarm
      }
      else {
        out4_CA = currentCAValue - 100.0 * vec4(CAdecay);
        if( out4_CA.a < 0.0 ) {
          out4_CA = vec4(0.0);
        }
      }
    }
  }
*/
  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = currentCAValue; // ready
  }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// ANIMATED PIXELS UPDATE
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void pixel_out( vec2 decalCoords ) {
  // fills by cumulating the neighboring pixels that
  // can reach the pixel at this frame (come into a circle of radius radius_pixel)
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
      // expected to be in [-radius_pixel,+radius_pixel]x[-radius_pixel,+radius_pixel]
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
      	if( pixel_acc > 0 ) {
      	  // acceleration
      	  surrpixel_speed += pixel_acc * acceleration;
      	}
      	else {
      	  // damping
      	  surrpixel_speed += pixel_acc * surrpixel_speed;
      	}
      	surrpixel_nextPosition 
      			     = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
      	// the current step is added to the position

      	if( abs(surrpixel_nextPosition.x) <= radius_pixel 
                  && abs(surrpixel_nextPosition.y) <= radius_pixel ) {
      	  out_color_pixel += surrpixel_localColor.rgb;
                out_speed_pixel += surrpixel_speed;
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
      // expected to be in [-radius_pixel,+radius_pixel]x[-radius_pixel,+radius_pixel]
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
      	if( pixel_acc > 0 ) {
      	  // acceleration
      	  surrpixel_speed += pixel_acc * acceleration;
      	}
      	else {
      	  // damping
      	  surrpixel_speed += pixel_acc * surrpixel_speed;
      	}
      	surrpixel_nextPosition 
      			     = usedNeighborOffset + surrpixel_position + surrpixel_speed; 
      	// the current step is added to the position

      	if( abs(surrpixel_nextPosition.x) <= radius_pixel 
                  && abs(surrpixel_nextPosition.y) <= radius_pixel ) {
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
  camera_BG_subtr = (uniform_Update_scenario_var_data[0] > 0 ? true : false);
  CAdecay = uniform_Update_scenario_var_data[1];
  trkDecay_0 = uniform_Update_scenario_var_data[2];
  trkDecay_1 = uniform_Update_scenario_var_data[3];
  trkDecay_2 = uniform_Update_scenario_var_data[4];
  trkDecay_3 = uniform_Update_scenario_var_data[5];
  currentDrawingTrack = int(uniform_Update_scenario_var_data[6]);
  currentVideoTrack = int(uniform_Update_scenario_var_data[7]);
  currentPhotoTrack = int(uniform_Update_scenario_var_data[8]);
  path_replay_trackNo_1 = int(uniform_Update_scenario_var_data[9]);
  path_replay_trackNo_2 = int(uniform_Update_scenario_var_data[10]);
  path_replay_trackNo_3 = int(uniform_Update_scenario_var_data[11]);
  path_replay_trackNo_4 = int(uniform_Update_scenario_var_data[12]);
  path_replay_trackNo_5 = int(uniform_Update_scenario_var_data[13]);
  path_replay_trackNo_6 = int(uniform_Update_scenario_var_data[14]);
  path_replay_trackNo_7 = int(uniform_Update_scenario_var_data[15]);
  path_replay_trackNo_8 = int(uniform_Update_scenario_var_data[16]);
  path_replay_trackNo_9 = int(uniform_Update_scenario_var_data[17]);
  path_replay_trackNo_10 = int(uniform_Update_scenario_var_data[18]);
  path_replay_trackNo_11 = int(uniform_Update_scenario_var_data[19]);
  noiseUpdateScale = uniform_Update_scenario_var_data[20];
  pixel_acc = uniform_Update_scenario_var_data[21];
  pixel_acc_shiftX = uniform_Update_scenario_var_data[22];
  pixel_acc_shiftY = uniform_Update_scenario_var_data[23];
  pixel_radius = uniform_Update_scenario_var_data[24];
  pixel_mode = int(uniform_Update_scenario_var_data[25]);
  repop_CA = uniform_Update_scenario_var_data[26];
  repop_BG = uniform_Update_scenario_var_data[27];
  BG_CA_repop_density = int(uniform_Update_scenario_var_data[28]);
  BG_CA_repop_color_mode = int(uniform_Update_scenario_var_data[29]);
  CAParams1 = uniform_Update_scenario_var_data[30];
  CAParams2 = uniform_Update_scenario_var_data[31];
  CAParams3 = uniform_Update_scenario_var_data[32];
  CAParams4 = uniform_Update_scenario_var_data[33];
  CAParams5 = uniform_Update_scenario_var_data[34];
  CAParams6 = uniform_Update_scenario_var_data[35];
  CAParams7 = uniform_Update_scenario_var_data[36];
  CAParams8 = uniform_Update_scenario_var_data[37];
  CAstep = int(uniform_Update_scenario_var_data[38]);
  CAcolorSpread = (uniform_Update_scenario_var_data[39] > 0 ? true : false);
  freeze = (uniform_Update_scenario_var_data[40] > 0 ? true : false);
  video_white = uniform_Update_scenario_var_data[41];
  Song_CA_color_mode = (uniform_Update_scenario_var_data[42] > 0 ? true : false);
  movieWeight = uniform_Update_scenario_var_data[43];
  movieSobel = uniform_Update_scenario_var_data[44];
  invertMovie = (uniform_Update_scenario_var_data[45] > 0 ? true : false);
  invertCamera = (uniform_Update_scenario_var_data[46] > 0 ? true : false);
  video_satur = uniform_Update_scenario_var_data[47];
  video_value = uniform_Update_scenario_var_data[48];
  photo_satur = uniform_Update_scenario_var_data[49];
  photo_value = uniform_Update_scenario_var_data[50];
  photo_gamma = uniform_Update_scenario_var_data[51];
  photo_threshold = uniform_Update_scenario_var_data[52];
  photoWeight = uniform_Update_scenario_var_data[53];
  photo_scale = uniform_Update_scenario_var_data[54];
  photo_rot = uniform_Update_scenario_var_data[55];
  photo_transl_x = uniform_Update_scenario_var_data[56];
  photo_transl_y = uniform_Update_scenario_var_data[57];
  mask_scale = uniform_Update_scenario_var_data[58];
  cameraCumul = int(uniform_Update_scenario_var_data[59]);
  cameraGamma = uniform_Update_scenario_var_data[60];
  camera_gamma = uniform_Update_scenario_var_data[61];
  camera_contrast = uniform_Update_scenario_var_data[62];
  cameraThreshold = uniform_Update_scenario_var_data[63];
  cameraWeight = uniform_Update_scenario_var_data[64];
  cameraSobel = uniform_Update_scenario_var_data[65];
  photo_contrast = uniform_Update_scenario_var_data[66];
  mask_contrast = uniform_Update_scenario_var_data[67];
  pixel_image_acceleration = int(uniform_Update_scenario_var_data[68]);
  invertPhoto = (uniform_Update_scenario_var_data[69] > 0 ? true : false);
  movie_gamma = uniform_Update_scenario_var_data[70];
  movie_threshold = uniform_Update_scenario_var_data[71];
  video_gamma = uniform_Update_scenario_var_data[72];
  video_threshold = uniform_Update_scenario_var_data[73];
  video_contrast = uniform_Update_scenario_var_data[74];
  photoSobel = uniform_Update_scenario_var_data[75];
  photo_scaleX = uniform_Update_scenario_var_data[76];
  photo_scaleY = uniform_Update_scenario_var_data[77];
  photo_equalization = uniform_Update_scenario_var_data[78];
  Pixelstep = int(uniform_Update_scenario_var_data[79]);
  video_hue = uniform_Update_scenario_var_data[80];
  photo_hue = uniform_Update_scenario_var_data[81];

  //////////////////////////
  // TRACK DECAY
  vec4 trkDecay = vec4(trkDecay_0,trkDecay_1,trkDecay_2,trkDecay_3);

  //////////////////////////
  // variables 
  // frame number
  frameNo = int(round(uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.x));
  Cursor = uniform_Update_fs_3fv_frameno_Cursor_flashPartCAWght.y;
  
  // CAType
  CAType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.x);
  CASubType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.y);
  CA_on_off = (CASubType > 0);
  
  // pixels
  pixel_acc_center = vec2(pixel_acc_shiftX,pixel_acc_shiftY);

  // working variables for screen dimension
  width = uniform_Update_fs_4fv_W_H_time_currentScene.x;
  height = uniform_Update_fs_4fv_W_H_time_currentScene.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // pixel texture + z offset according to the chosen texture
  vec2 noisePositionOffset;
  noisePositionOffset = texture( uniform_Update_texture_fs_Noise, 
		   vec3( 1.0 + sin(frameNo/50000.0),
			 1.0 + cos(frameNo/50000.0),
			 3.5  / 7.0 ) ).rg; // 
  pixelTextureCoordinatesXY = decalCoordsPOT + noisePositionOffset;//+ 5.0 + sin(frameNo/10000.0) * 5) 

  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

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
    // stops there unless CA has to be initialized (at launch time)
    if(uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.x <= 0) {
      return;
    }
  }

  /////////////////////////////////////
  /////////////////////////////////////
  // COORDINATE CALCULATION FOR STROKE RENDERING
  /////////////////////////////////////
  /////////////////////////////////////

  /////////////////////////////////////////////////
  // STROKE INTERPOLATION FOR LINE OR BEZIER

  // distance to closest point in case of curve drawing
  // is in curve drawing mode
  bool drawnBezier = false;

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: COLOR OUTPUT AND GRAY LEVEL CALCULATION
  //////////////////////////////////////
  //////////////////////////////////////

  //////////////////////////////////////
  // COPY OF FBO VALUES AT THE PRECEDING FRAME
  // starts by copying previous output

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
  out_attachment_FBO[pg_FBO_fs_Pixels_attacht]
    = texture( uniform_Update_texture_fs_Pixels , decalCoords );
  out_attachment_FBO[pg_FBO_fs_CA_attacht]
    = texture( uniform_Update_texture_fs_CA , decalCoords );

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PIXEL: increase radius for pixel capture
  //////////////////////////////////////
  //////////////////////////////////////
  float flashCACoef = uniform_Update_fs_4fv_flashTrkCAWghts.x;
  vec3 flashToBGCumul = vec3(0.0);
  vec4 flashToCACumul = vec4(0.0);  // RGB + GRAY
  // vec3 flashToCACumul = vec3(0.0);

  //////////////////////////////////////////////
  // STROKE INTERPOLATION FOR LINE
  // int newtrack = -1;
  float track_0_opacity = 0.0;
  vec4 pathColor = vec4(1,1,1,1);

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
  photocolor *= vec3(photo_value);

  // video texture used for drawing
  cameraCoord = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y )
               * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw
               + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
  cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;
  vec4 cameraImageIni = vec4(cameraImage, 1);
/*  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
             * uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.xy;
  movieImage = texture(uniform_Update_texture_fs_Movie_frame, movieCoord ).rgb;
*/
  if(camera_gamma != 1) {
    cameraImage = vec3( pow(cameraImage.r,camera_gamma) , pow(cameraImage.g,camera_gamma) , pow(cameraImage.b,camera_gamma) );
  }

  if( camera_BG_subtr ) {
    cameraImage = abs(cameraImage 
                     - texture(uniform_Update_texture_fs_Camera_BG, cameraCoord ).rgb); // current background subtraction
  }
  else {
    cameraImage = abs(cameraImage 
                     - texture(uniform_Update_texture_fs_Camera_BGIni, cameraCoord ).rgb); // initial background subtraction
  }
  // initial frame subtraction - could be reactivated and used in the last 2 scenes
  // videocolor = abs(texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb 
  //                      - texture(fs_lookupTable8, cameraCoord ).rgb); // initial background subtraction

  float cameraGray = graylevel(cameraImage);
  if( cameraGray <= cameraThreshold ) {
    cameraImage = vec3(0);
  }
  else {
    cameraImage = vec3(cameraGray);
  }

  float correctionFactor =  1.016 * (camera_contrast + 1.0) 
                              / (1.016 - camera_contrast);
  cameraImage = clamp( vec3(correctionFactor)
    * (cameraImage - vec3(0.5)) + vec3(0.5) , 0 , 1 );


  // video image = mix of movie and camera
  videocolor = cameraWeight * cameraImage;
  //         + movieWeight * movieImage;


  // each track possibly covers the previous color
  // for( int ind = 0 ; ind < 4 ; ind++ ) {

  // TRACK 0
  // background drawing
  uint pathStroke = DRAWING_VOID;
  // pen drawing on the current track (comes last)
  if( currentDrawingTrack == 0 ) {
    pathStroke = DRAWING_LINE;
  }

  // pathStroke = DRAWING_LINE;
  if( pathStroke > 0 ) {
    int indPath = 0;
    BezierControlX = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_P_X];
    BezierControlY = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_P_Y];
    BezierControl[0] = vec2(BezierControlX[0], BezierControlY[0]);
    BezierControl[1] = vec2(BezierControlX[1], BezierControlY[1]);
    BezierControl[2] = vec2(BezierControlX[2], BezierControlY[2]);
    BezierControl[3] = vec2(BezierControlX[3], BezierControlY[3]);
    BezierBox = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_BOX];
    vec4 radiusX_beginOrEnd_radiusY_brushID 
     = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_RADIUS_BEGINEND];
    pathColor
     = uniform_Update_path_data[indPath * PG_MAX_PATH_DATA + PG_PATH_COLOR];
    float isBeginOrEnd = radiusX_beginOrEnd_radiusY_brushID.y;
    int current_brushID = int(radiusX_beginOrEnd_radiusY_brushID.w);
    isBegin = (isBeginOrEnd > 0);
    isEnd = (isBeginOrEnd < 0);

    if(Cursor < 0) {
      float curTrack_grayLevel
        = out_gray_drawing(
                 3 * radiusX_beginOrEnd_radiusY_brushID.x, current_brushID );
      out_track_FBO[0].rgb *= (1 - curTrack_grayLevel);
   }
    else {
      track_0_opacity 
        += out_gray_drawing(
                  radiusX_beginOrEnd_radiusY_brushID.x, current_brushID );
    }

    // drawing occurs on background
    if( track_0_opacity > 0 ) {
      // newtrack = 0;
      // brush opacity is combined with color opacity
      track_0_opacity *= pathColor.a;
      out_track_FBO[0].rgb = 
      	out_track_FBO[0].rgb * (1 - track_0_opacity)
      	+ track_0_opacity * pathColor.rgb;
    }
  }
  
  // if(cameraWeight + movieWeight > 0) {
  if(cameraWeight > 0) {
     out_track_FBO[0].rgb = clamp( videocolor + photocolor, 0.0 , 1.0 );
  }

  // flash CA opacity
  flashToCACumul = flashCACoef * vec4(out_track_FBO[0].rgb,graylevel(out_track_FBO[0].rgb));

  //////////////////////////////////////
  //////////////////////////////////////
  // CA LAYER: CA "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( CA_on_off && (frameNo % CAstep == 0) ) {
    out4_CA = vec4(0.0);
    vec4 currentCA = out_attachment_FBO[pg_FBO_fs_CA_attacht]; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // will disappear through the updating rule of the automaton)
    if(  flashToCACumul.a > 0 && decalCoords.x > 2 && height - decalCoords.y > 2) { // gray level of copy color (in case of flash)
      // currentCA.a = -1;
      // currentCA.rgb = clamp( currentCA.rgb + out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 0.0 , 1.0 );
      out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(currentCA.rgb,-1);
      // out_track_FBO[0] = vec4(0,0,0,1);
    }
    else {
       // calculates the new state of the automaton
      CA_out( decalCoords , currentCA );
     // outputs the color from CA (average surrounding color if birth)
      out_attachment_FBO[pg_FBO_fs_CA_attacht] = out4_CA;
    }
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.w == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Update_fs_4fv_repop_ColorBG_flashCABGWght.w * out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 0 , 1 );

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: PIXEL "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( pixel_mode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH PIXEL: increase radius for pixel capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.y > 0 ) {
      radius_pixel = 1;
    }

    pixel_out( decalCoords );

    // arrival of a new pixel
    if( graylevel(out_color_pixel) > 0 ) {
      // outputs the color from pixel move at drawing layer
      out_track_FBO[0].rgb = out_color_pixel;
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht] = vec4( out_speed_pixel , out_position_pixel );
    }
    // update of the current pixel
    else {
      //  modifies speed according to acceleration
      vec2 pixel_acceleration;
      // FLAT
      pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseUpdateScale * 100 ),
                                snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseUpdateScale * 100 ));

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc > 0 ) {
      	// acceleration
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy += pixel_acc * acceleration;
      }
      else {
	// damping
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy += pixel_acc * out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy;
      }
      // updates the position of the current pixel
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht].zw += out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy; 

      // if the pixel leaves the cell, it is erased
      if( pixel_mode == PIXEL_UNIQUE ) {
        if( abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].z ) > 0.5
          || abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].w ) > 0.5 ) {
          out_track_FBO[0].rgb = vec3(0,0,0);
        }
      }
    }
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRWING AND CA LAYERS: GRAY LEVEL DECAY
  //////////////////////////////////////
  //////////////////////////////////////

  // track flashing on the background level
  out_track_FBO[0].rgb = clamp(out_track_FBO[0].rgb + flashToBGCumul, 0 , 1);

  // BACKGROUND TRACK 0 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_track_FBO[0].rgb) > 0 ) {
    out_track_FBO[0].rgb = out_track_FBO[0].rgb 
      - vec3(trkDecay.x,
             trkDecay.x,
	           trkDecay.x);
  }
  out_track_FBO[0].rgb 
    = clamp( out_track_FBO[0].rgb , 0.0 , 1.0 );


  //////////////////////////////////////////////
  // rebuilds output for the gray/drawing buffer after decay
  // has been taken into consideration
  if( graylevel(out_track_FBO[0].rgb) == 0 ) {
    // in case color has faded out or pen has never written
    // returns black
    out_attachment_FBO[pg_FBO_fs_Pixels_attacht] = vec4(0,0,0,0);
    out_track_FBO[0].rgb= vec3(0,0,0);
  }
  // clear layer
  if( uniform_Update_fs_3fv_isClearLayer_flashPixel_flashCameraTrkThres.x > 0 ) {
    out_attachment_FBO[pg_FBO_fs_Pixels_attacht] = vec4(0,0,0,0);
    out_track_FBO[0].rgb= vec3(0,0,0);
  }

  // CA LAYER DECAY
  // the first frame for CA is negative so that it is not drawn
  // in this case its value is preserved as it is
  // and incay or decay does not make sense on a black color
  if( out_attachment_FBO[pg_FBO_fs_CA_attacht].a > 0 ) {
    vec3 newCA_w_decay = out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb;
    // decay for generation 
    if( CAType == CA_GENERATION ) {
      newCA_w_decay = clamp( newCA_w_decay - vec3(CAdecay) , 0.0 , 1.0 );
    }
    // normal or normal quiescent cell ageing
    else if( CAType == CA_TUMOR_CELL ) {
      // fast decay for normal cells
      if( out_attachment_FBO[pg_FBO_fs_CA_attacht].a >= 1 && out_attachment_FBO[pg_FBO_fs_CA_attacht].a <= 10 ) { 
        newCA_w_decay = clamp( newCA_w_decay - 10.0 * vec3(CAdecay) , 0.0 , 1.0 );
        newCA_w_decay = clamp( newCA_w_decay - 10.0 * vec3(CAdecay) , 0.0 , 1.0 );
      }
      // slow decay for tumor cells
      if( out_attachment_FBO[pg_FBO_fs_CA_attacht].a >= 11 && out_attachment_FBO[pg_FBO_fs_CA_attacht].a <= 20 ) { 
        newCA_w_decay = clamp( newCA_w_decay - 2.0 * vec3(CAdecay) , 0.0 , 1.0 );
        newCA_w_decay = clamp( newCA_w_decay - 2.0 * vec3(CAdecay) , 0.0 , 1.0 );
      }
    }
    // normal or normal quiescent cell ageing
    else if( CAType == CA_GENERAL_BINARY_MOORE ) {
      // random decay for 
      if( noiseCA.g < CAdecay ) { 
        newCA_w_decay = clamp( newCA_w_decay - vec3(CAdecay) , 0.0 , 1.0 );
      }
    }

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

  // clear CA
  if( uniform_Update_fs_3fv_clearAllLayers_clearCA_pulsedShift.y > 0 ) {
      out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(0);
  }

  // launching losange CA at the beginning
  float initCA = uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.x;
  if(initCA > 0 ) {
     if( length(vec2(decalCoords.x - width/2 , decalCoords.y - height/2)) < initCA ) {
        out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(1,1,1,-1);
        out_track_FBO[0] = vec4(0,0,0,1);
      }
      else {
        out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(0);
        out_track_FBO[0] = vec4(0,0,0,1);
      }
  }

  // launching airstrike 
  if( CAType == CA_NEUMANN_BINARY
     && frameNo < uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.y + 4 * 2600 
     && int(frameNo - uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.y) % 2600 < 10
     && abs(decalCoords.x - width + 10) < 3.2  
     && abs(decalCoords.y - height/2) < 3.2 ) {
    out_attachment_FBO[pg_FBO_fs_CA_attacht] = vec4(1,1,1,1);
  }


  out_Update_FBO_fs_CA = out_attachment_FBO[pg_FBO_fs_CA_attacht];
  out_Update_FBO_fs_Pixels = out_attachment_FBO[pg_FBO_fs_Pixels_attacht];
  // out_Update_FBO_fs_Trk0 = out_track_FBO[0] * 0.1 + texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ) * 0.9;
  out_Update_FBO_fs_Trk0 = out_track_FBO[0];
#if PG_NB_TRACKS >= 2
  out_Update_FBO_fs_Trk1 = out_track_FBO[1];
  out_Update_FBO_fs_Trk1 = cameraImageIni;
#endif
#if PG_NB_TRACKS >= 3
  out_Update_FBO_fs_Trk2 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  out_Update_FBO_fs_Trk3 = out_track_FBO[3];
#endif
}
