/***********************************************************************
File: song/shaders/LYM_Update_song.frag

LYM song & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 430

bool      camera_BG_subtr;
float     CAdecay;
float     trkDecay_0;
int       currentDrawingTrack;
uniform vec4 uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_currentDrawingTrack;
int       currentVideoTrack;
int       currentPhotoTrack;
int       path_replay_trackNo_1;
int       path_replay_trackNo_2;
uniform vec4 uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2;
int       path_replay_trackNo_3;
float     noiseScale;
int       noiseType;
float     noiseLineScale;
uniform vec4 uniform_Update_fs_4fv_path_replay_trackNo_3_noiseScale_noiseType_noiseLineScale;
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
int       activeClipArts;
uniform vec4 uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_activeClipArts;
float     cameraThreshold;
float     cameraWeight;
float     cameraSobel;
float     movieWeight;
uniform vec4 uniform_Update_fs_4fv_cameraThreshold_cameraWeight_cameraSobel_movieWeight;
float     movieSobel;
bool      invertMovie;
float     video_hue;
float     video_satur;
uniform vec4 uniform_Update_fs_4fv_movieSobel_invertMovie_video_hue_video_satur;
float     video_satur_pulse;
float     video_value;
float     photoWeight;
float     photo_hue;
uniform vec4 uniform_Update_fs_4fv_video_satur_pulse_video_value_photoWeight_photo_hue;
float     photo_satur;
float     photo_satur_pulse;
float     photo_value;
float     photo_value_pulse;
uniform vec4 uniform_Update_fs_4fv_photo_satur_photo_satur_pulse_photo_value_photo_value_pulse;
float     photo_scale;
float     photo_contrast;
float     mask_contrast;
float     CAParams1;
uniform vec4 uniform_Update_fs_4fv_photo_scale_photo_contrast_mask_contrast_CAParams1;
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
float     fft_scale;
uniform vec4 uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_fft_scale;

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// CONST
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

const float PI = 3.1415926535897932384626433832795;

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

#define SPLAT_PARTICLES

////////////////////////////////////////////////////////////////////
// TRACK CONST
#define PG_NB_TRACKS 1
#define PG_NB_PATHS 3

///////////////////////////////////////////////////////////////////
const uint pg_FBO_fs_CA_attacht = 0;
const uint pg_FBO_fs_Pixels_attacht = 1;

////////////////////////////////////////////////////////////////////
// CELLULAR AUTOMATA
// CA types
const uint CA_PROTOCELLS = 0;
// const uint CA_DISLOCATION = 2;
// const uint CA_CYCLIC_1 = 1;
// const uint GOL_1 = 3;
// const uint CA_CYCLIC = 4;
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
int pg_CurrentScene;

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

bool CA_on_off;

vec4 neighborValues[8]=vec4[8](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiamond[4]=vec4[4](vec4(0),vec4(0),vec4(0),vec4(0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),
                                    vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0),vec4(0));
vec2 pixelTextureCoordinatesXY; // the POT coordinates of the
// pixel texture + z offset according to the chosen texture
vec2 noisepixels;

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
uniform vec2 uniform_Update_fs_2fv_photo01Wghts;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;
uniform vec4 uniform_Update_fs_4fv_fftLevels03;
uniform vec4 uniform_Update_fs_4fv_fftFrequencies03;
uniform vec4 uniform_Update_fs_4fv_fftPhases03;
uniform vec4 uniform_Update_fs_4fv_fftLevels47;
uniform vec4 uniform_Update_fs_4fv_fftFrequencies47;
uniform vec4 uniform_Update_fs_4fv_fftPhases47;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect uniform_Update_texture_fs_CA;       // 2-cycle ping-pong Update pass CA step n (FBO attachment 0)
layout (binding = 1) uniform samplerRect uniform_Update_texture_fs_Pixels;   // 2-cycle ping-pong Update pass speed/position of Pixels step n (FBO attachment 1)
layout (binding = 2) uniform sampler3D   uniform_Update_texture_fs_Brushes;  // pen patterns
layout (binding = 3) uniform samplerRect uniform_Update_texture_fs_Camera_frame;  // camera texture
layout (binding = 4) uniform samplerRect uniform_Update_texture_fs_Camera_BG;     // camera BG texture
layout (binding = 5) uniform samplerRect uniform_Update_texture_fs_Movie_frame;   // movie textures
layout (binding = 6) uniform sampler3D   uniform_Update_texture_fs_Noise;  // noise texture
layout (binding = 7) uniform sampler2D   uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 8) uniform sampler2D   uniform_Update_texture_fs_Photo1;  // photo_1 texture
layout (binding = 9) uniform samplerRect uniform_Update_texture_fs_Trk0;  // 2-cycle ping-pong Update pass track 0 step n (FBO attachment 5)

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
  //  41 pts uniformly over a line, mapped onto a diamo
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  //  The ring size 17*17 = 289 is close to a multiple 
  //      of 41 (41*7 = 287)

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
/*     float CAParams1 = 0.480315;
    float CAParams2 = 0.511811;
    float CAParams3 = 0.637795;
    float CAParams4 = 0.472441;
    float CAParams5 = 0.244094;
 */
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
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

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
        vec2 pixelTexCoordLoc = pixelTextureCoordinatesXY
                       + usedNeighborOffset / vec2(width,height);

        pixel_acceleration = multiTypeGenerativeNoise(pixelTexCoordLoc, usedNeighborOffset);

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
                      (current_brushID / 3) * 0.33333 + 0.16667))[current_brushID % 3];
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
  camera_BG_subtr = (uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_currentDrawingTrack[0] > 0 ? true : false);
  CAdecay = uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_currentDrawingTrack[1];
  trkDecay_0 = uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_currentDrawingTrack[2];
  currentDrawingTrack = int(uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_currentDrawingTrack[3]);
  currentVideoTrack = int(uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2[0]);
  currentPhotoTrack = int(uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2[1]);
  path_replay_trackNo_1 = int(uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2[2]);
  path_replay_trackNo_2 = int(uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2[3]);
  path_replay_trackNo_3 = int(uniform_Update_fs_4fv_path_replay_trackNo_3_noiseScale_noiseType_noiseLineScale[0]);
  noiseScale = uniform_Update_fs_4fv_path_replay_trackNo_3_noiseScale_noiseType_noiseLineScale[1];
  noiseType = int(uniform_Update_fs_4fv_path_replay_trackNo_3_noiseScale_noiseType_noiseLineScale[2]);
  noiseLineScale = uniform_Update_fs_4fv_path_replay_trackNo_3_noiseScale_noiseType_noiseLineScale[3];
  noiseAngleScale = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[0];
  noiseCenterX = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[1];
  noiseCenterY = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[2];
  pixel_acc = uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc[3];
  pixel_acc_shiftX = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[0];
  pixel_acc_shiftY = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[1];
  pixel_radius = uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[2];
  pixel_mode = int(uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode[3]);
  repop_CA = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_activeClipArts[0];
  repop_BG = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_activeClipArts[1];
  cameraGamma = uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_activeClipArts[2];
  activeClipArts = int(uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_activeClipArts[3]);
  cameraThreshold = uniform_Update_fs_4fv_cameraThreshold_cameraWeight_cameraSobel_movieWeight[0];
  cameraWeight = uniform_Update_fs_4fv_cameraThreshold_cameraWeight_cameraSobel_movieWeight[1];
  cameraSobel = uniform_Update_fs_4fv_cameraThreshold_cameraWeight_cameraSobel_movieWeight[2];
  movieWeight = uniform_Update_fs_4fv_cameraThreshold_cameraWeight_cameraSobel_movieWeight[3];
  movieSobel = uniform_Update_fs_4fv_movieSobel_invertMovie_video_hue_video_satur[0];
  invertMovie = (uniform_Update_fs_4fv_movieSobel_invertMovie_video_hue_video_satur[1] > 0 ? true : false);
  video_hue = uniform_Update_fs_4fv_movieSobel_invertMovie_video_hue_video_satur[2];
  video_satur = uniform_Update_fs_4fv_movieSobel_invertMovie_video_hue_video_satur[3];
  video_satur_pulse = uniform_Update_fs_4fv_video_satur_pulse_video_value_photoWeight_photo_hue[0];
  video_value = uniform_Update_fs_4fv_video_satur_pulse_video_value_photoWeight_photo_hue[1];
  photoWeight = uniform_Update_fs_4fv_video_satur_pulse_video_value_photoWeight_photo_hue[2];
  photo_hue = uniform_Update_fs_4fv_video_satur_pulse_video_value_photoWeight_photo_hue[3];
  photo_satur = uniform_Update_fs_4fv_photo_satur_photo_satur_pulse_photo_value_photo_value_pulse[0];
  photo_satur_pulse = uniform_Update_fs_4fv_photo_satur_photo_satur_pulse_photo_value_photo_value_pulse[1];
  photo_value = uniform_Update_fs_4fv_photo_satur_photo_satur_pulse_photo_value_photo_value_pulse[2];
  photo_value_pulse = uniform_Update_fs_4fv_photo_satur_photo_satur_pulse_photo_value_photo_value_pulse[3];
  photo_scale = uniform_Update_fs_4fv_photo_scale_photo_contrast_mask_contrast_CAParams1[0];
  photo_contrast = uniform_Update_fs_4fv_photo_scale_photo_contrast_mask_contrast_CAParams1[1];
  mask_contrast = uniform_Update_fs_4fv_photo_scale_photo_contrast_mask_contrast_CAParams1[2];
  CAParams1 = uniform_Update_fs_4fv_photo_scale_photo_contrast_mask_contrast_CAParams1[3];
  CAParams2 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[0];
  CAParams3 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[1];
  CAParams4 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[2];
  CAParams5 = uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5[3];
  CAParams6 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[0];
  CAParams7 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[1];
  CAParams8 = uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[2];
  cameraCumul = int(uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul[3]);
  CAstep = int(uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_fft_scale[0]);
  CAcolorSpread = (uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_fft_scale[1] > 0 ? true : false);
  freeze = (uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_fft_scale[2] > 0 ? true : false);
  fft_scale = uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_fft_scale[3];

  //////////////////////////
  // TRACK DECAY
  vec4 trkDecay = vec4(trkDecay_0,0,0,0);

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
  pg_CurrentScene = int(uniform_Update_fs_4fv_W_H_time_currentScene.w);

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
  pixelTextureCoordinatesXY = decalCoordsPOT 
                  + 0.1 * noisePositionOffset; //+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // noise for CA: random value
  randomCA = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.0 ) );
  randomCA2 = texture( uniform_Update_texture_fs_Noise , vec3( vec2(1,1) - pixelTextureCoordinatesXY , 0.5 ) );

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
        - uniform_Update_fs_4fv_xy_transl_tracks_0_1.xy;
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track_FBO[0] = texture( uniform_Update_texture_fs_Trk0 , decalCoordsPrevStep );
    // BLUR
  }
  else {
    out_track_FBO[0] = vec4( 0, 0, 0, 0 );
  }


  // if freeze, just keep values as they are
  if( freeze ) {
    // FBO OUTPUTS
    out_Update_FBO_fs_CA = out_attachment_FBO[pg_FBO_fs_CA_attacht];
    out_Update_FBO_fs_Pixels = out_attachment_FBO[pg_FBO_fs_Pixels_attacht];
    out_Update_FBO_fs_Trk0 = out_track_FBO[0];
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
  if(photoWeight * uniform_Update_fs_2fv_photo01Wghts.x > 0) {
    coordsImage = vec2(1.0 - decalCoordsPOT.x , decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.xy;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
    photocolor += photoWeight * uniform_Update_fs_2fv_photo01Wghts.x * texture(uniform_Update_texture_fs_Photo0, 
        coordsImageScaled ).rgb;
  }
  if(photoWeight * uniform_Update_fs_2fv_photo01Wghts.y > 0) {
    coordsImage = vec2(1.0 - decalCoordsPOT.x , decalCoordsPOT.y) * uniform_Update_fs_4fv_photo01_wh.zw;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
    photocolor += photoWeight * uniform_Update_fs_2fv_photo01Wghts.y * texture(uniform_Update_texture_fs_Photo1,  
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

  // CAMERA IMAGE
  cameraCoord = vec2(decalCoordsPOT.x, (1 - decalCoordsPOT.y) )
              // added for wide angle lens that covers more than the drawing surface
               * (cameraWH + (2 * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy)) - uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;

  // image reading
  if(cameraCoord.x >= 0 && cameraCoord.x  < cameraWH.x && cameraCoord.y >= 0 && cameraCoord.y  < cameraWH.y) {
    cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;

    // gamma correction
    // cameraImage = vec3( pow(cameraImage.r,cameraGamma) , pow(cameraImage.g,cameraGamma) , pow(cameraImage.b,cameraGamma) );
    if( camera_BG_subtr ) {
      cameraImage = abs(cameraImage - texture(uniform_Update_texture_fs_Camera_BG, cameraCoord ).rgb); // initial background subtraction
    }
    if( graylevel(cameraImage) < cameraThreshold ) {
      cameraImage = vec3(0.0);
    }

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
  }
  // cameraImage = vec3(1) - cameraImage;


  // MOVIE IMAGE
  movieCoord = vec2(1 - decalCoordsPOT.x , decalCoordsPOT.y )
               * movieWH;

  movieImage = texture(uniform_Update_texture_fs_Movie_frame, movieCoord ).rgb;

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
            if(indPath == 0 && // rubber stylus
                               Cursor < 0) {
                curTrack_grayLevel =  out_gray_drawing( uniform_Update_fs_4fv_paths03_x[indPath] , 
                    uniform_Update_fs_4fv_paths03_y[indPath] , 
                    uniform_Update_fs_4fv_paths03_x_prev[indPath] , 
                    uniform_Update_fs_4fv_paths03_y_prev[indPath] ,
                    3 * uniform_Update_fs_4fv_paths03_RadiusX[indPath] ,
                    3 * uniform_Update_fs_4fv_paths03_RadiusY[indPath] ,
                    int(uniform_Update_fs_4fv_paths03_BrushID[indPath]),
                    pathStroke );
                out_track_FBO[indCurTrack].rgb *= (1 - curTrack_grayLevel);
                curTrack_color.rgb = vec3(0);
            }
            else { // normal stylus
                curTrack_grayLevel =  out_gray_drawing( uniform_Update_fs_4fv_paths03_x[indPath] , 
                    uniform_Update_fs_4fv_paths03_y[indPath] , 
                    uniform_Update_fs_4fv_paths03_x_prev[indPath] , 
                    uniform_Update_fs_4fv_paths03_y_prev[indPath] ,
                    uniform_Update_fs_4fv_paths03_RadiusX[indPath] ,
                    uniform_Update_fs_4fv_paths03_RadiusY[indPath] ,
                    int(uniform_Update_fs_4fv_paths03_BrushID[indPath]),
                    pathStroke );
                curTrack_color.rgb
                  += curTrack_grayLevel
                    * uniform_Update_fs_4fv_paths03_a[indPath]
                    * vec3( uniform_Update_fs_4fv_paths03_r[indPath] , 
                                uniform_Update_fs_4fv_paths03_g[indPath] , 
                                uniform_Update_fs_4fv_paths03_b[indPath] );  // brush opacity is combined with color opacity
            }
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
        float gtrack = graylevel(out_track_FBO[indCurTrack].rgb);
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
      && photoWeight * uniform_Update_fs_2fv_photo01Wghts.x + photoWeight * uniform_Update_fs_2fv_photo01Wghts.y > 0 ) {
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

  ////////////////
  // TRACK FFT
  // photo color is defined as a rendering based on FFT analysis 
  // instantaneous circular rendering
  float dist = 0;
  float waveGrey = 0;
  // circulaire ou lineaire fixe
  if(pg_CurrentScene == 0 || pg_CurrentScene == 9) {
    if(pg_CurrentScene == 0) {
      dist = length(vec2(decalCoords.x-(width/2),decalCoords.y-(height/2)));
    }
    else {
      dist = decalCoords.x;
    }
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale + uniform_Update_fs_4fv_fftPhases03.x) + 1)
        + uniform_Update_fs_4fv_fftLevels03.y * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale + uniform_Update_fs_4fv_fftPhases03.y) + 1)
        + uniform_Update_fs_4fv_fftLevels03.z * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale + uniform_Update_fs_4fv_fftPhases03.z) + 1)
        + uniform_Update_fs_4fv_fftLevels03.w * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale + uniform_Update_fs_4fv_fftPhases03.w) + 1)
        + uniform_Update_fs_4fv_fftLevels47.x * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale + uniform_Update_fs_4fv_fftPhases47.x) + 1)
        + uniform_Update_fs_4fv_fftLevels47.y * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale + uniform_Update_fs_4fv_fftPhases47.y) + 1)
        + uniform_Update_fs_4fv_fftLevels47.z * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale + uniform_Update_fs_4fv_fftPhases47.z) + 1)
        + uniform_Update_fs_4fv_fftLevels47.w * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale + uniform_Update_fs_4fv_fftPhases47.w) + 1);
    if(pg_CurrentScene == 0) {
      if(dist > height/2) {
        waveGrey = 0;
      }
      if(dist > (height/2 - 20)) {
        waveGrey *= 1 - (dist - (height/2 - 20)) / 20.;
      }
    }
    out_track_FBO[0].rgb = vec3( waveGrey );
  }
  // lineaire temporel avec ou sans effacement
  else if((pg_CurrentScene == 1 || pg_CurrentScene == 2) && decalCoords.x < 1) {
    dist = decalCoords.y;
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale + uniform_Update_fs_4fv_fftPhases03.x) + 1)
        + uniform_Update_fs_4fv_fftLevels03.y * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale + uniform_Update_fs_4fv_fftPhases03.y) + 1)
        + uniform_Update_fs_4fv_fftLevels03.z * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale + uniform_Update_fs_4fv_fftPhases03.z) + 1)
        + uniform_Update_fs_4fv_fftLevels03.w * (cos(dist * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale + uniform_Update_fs_4fv_fftPhases03.w) + 1)
        + uniform_Update_fs_4fv_fftLevels47.x * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale + uniform_Update_fs_4fv_fftPhases47.x) + 1)
        + uniform_Update_fs_4fv_fftLevels47.y * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale + uniform_Update_fs_4fv_fftPhases47.y) + 1)
        + uniform_Update_fs_4fv_fftLevels47.z * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale + uniform_Update_fs_4fv_fftPhases47.z) + 1)
        + uniform_Update_fs_4fv_fftLevels47.w * (cos(dist * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale + uniform_Update_fs_4fv_fftPhases47.w) + 1);
    out_track_FBO[0].rgb = vec3( waveGrey );
  }
  // lignes verticales avec déphasage
  else if(pg_CurrentScene == 3) {
    dist = decalCoords.x;
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale + uniform_Update_fs_4fv_fftPhases03.x)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.y * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale + uniform_Update_fs_4fv_fftPhases03.y)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.z * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale + uniform_Update_fs_4fv_fftPhases03.z)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.w * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale + uniform_Update_fs_4fv_fftPhases03.w)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.x * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale + uniform_Update_fs_4fv_fftPhases47.x)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.y * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale + uniform_Update_fs_4fv_fftPhases47.y)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.z * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale + uniform_Update_fs_4fv_fftPhases47.z)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.w * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale + uniform_Update_fs_4fv_fftPhases47.w)+ 0.01);
    out_track_FBO[0].rgb = clamp(vec3( 8 * waveGrey ), 0, 1);
  }
  // lignes verticales sans déphasage
  else if(pg_CurrentScene == 4) {
    dist = decalCoords.x;
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.y * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.z * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels03.w * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.x * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.y * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.z * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale)+ 0.01)
             + uniform_Update_fs_4fv_fftLevels47.w * floor(cos(dist * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale)+ 0.01);
    out_track_FBO[0].rgb = clamp(vec3( 8 * waveGrey ), 0, 1);
  }
  // moiré vertical et dephasage utilisé pour incliner les bandes (en mode rendu lineaire)
  else if(pg_CurrentScene == 5 || pg_CurrentScene == 6) {
    dist = decalCoords.x;
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.x/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale))
             + uniform_Update_fs_4fv_fftLevels03.y * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.y/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale))
             + uniform_Update_fs_4fv_fftLevels03.z * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.z/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale))
             + uniform_Update_fs_4fv_fftLevels03.w * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.w/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale))
             + uniform_Update_fs_4fv_fftLevels47.x * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.x/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale))
             + uniform_Update_fs_4fv_fftLevels47.y * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.y/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale))
             + uniform_Update_fs_4fv_fftLevels47.z * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.z/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale))
             + uniform_Update_fs_4fv_fftLevels47.w * (cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.w/20) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale));
    out_track_FBO[0].rgb = clamp(vec3( 8 * waveGrey ), 0, 1);
  }
  // moiré horizontal et dephasage utilisé pour incliner les bandes (en mode rendu lineaire)
  else if(pg_CurrentScene == 7 || pg_CurrentScene == 8) {
    dist = decalCoords.x;
    waveGrey = uniform_Update_fs_4fv_fftLevels03.x * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.x/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.x / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels03.y * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.y/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.y / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels03.z * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.z/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.z / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels03.w * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases03.w/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies03.w / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels47.x * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.x/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.x / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels47.y * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.y/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.y / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels47.z * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.z/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.z / fft_scale)),300)
             + uniform_Update_fs_4fv_fftLevels47.w * pow(abs(cos((dist + tan(uniform_Update_fs_4fv_fftPhases47.w/100) * decalCoords.y) * uniform_Update_fs_4fv_fftFrequencies47.w / fft_scale)),300);
    out_track_FBO[0].rgb = clamp(vec3( 8 * waveGrey ), 0, 1);
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
        pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseScale * 100 ),
                                snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));
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
}