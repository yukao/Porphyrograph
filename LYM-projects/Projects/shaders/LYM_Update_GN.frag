/***********************************************************************
File: alKemi/shaders/LYM_Drawing_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

#include_declarations

#define PG_NB_TRACKS 1
#define PG_NB_PATHS 3

const uint pg_FBO_fs_CA_attacht = 0;
const uint pg_FBO_fs_Pixels_attacht = 1;

////////////////////////////////////
// PEN STROKE CALCULATION
float dist_to_mousePos = 0;
float unsigned_rx;
float unsigned_ry;
float signed_rx;
float signed_ry;
vec3 out_drawn_filled;

// out_pixel returned values
vec3 out_color_pixel;
vec2 out_position_pixel = vec2(0,0);
vec2 out_speed_pixel = vec2(0,0);

/////////////////////////////////////
// 3 FBO OUTPUT: CA, PIXELS AND PARTICLES
// const uint pg_FBO_fs_CA_attacht = 0;
// const uint pg_FBO_fs_Pixels_attacht = 1;
vec4 out_attachment_FBO[pg_FBO_fs_Pixels_attacht + 1];


/////////////////////////////////////
// PG_NB_TRACKS TRACK COLOR FBO OUTPUT
 vec4 out_track_FBO[PG_NB_TRACKS];

vec4 out4_CA; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)
float CAdecay = 0.0;
vec4 noiseCA = vec4(0);

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or pixels
bool newly_drawn = false;

/*
  vec3 out_color_gol;
*/

bool CA_on_off;

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

// CA Type
int CAType;
int CASubType;

// pixel variables
float radius_pixel = 0.5;

float height;
float width;

////////////////////////////////////
// PIXEL UPDATE
// center of the translation of acceleration values
// used for shifting pixel motion in a direction
vec2 pixel_acc_center = vec2(0.5,0.5);

vec2 pixelTextureCoordinatesXY; // the POT coordinates of the
// pixel texture 

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
                                 {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW
const vec2 oppositeNeighborOffsets[8] = {{-1,0},{1,0},{0,-1},{0,1},      // E W N S
                                         {-1,-1},{1,1},{-1,1},{1,-1},};  // NE SW SE NW

vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
                               vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// camera image
vec3 cameraImage = vec3(0.0);
vec3 videocolor = vec3(0.0);
vec3 photocolor = vec3(0.0);
vec2 cameraCoord = vec2(0.0);
vec3 movieImage = vec3(0.0);
vec2 movieCoord = vec2(0.0);
vec2 movieWH;

// frame number
int frameNo;

// PIXEL mode
#define PIXEL_NONE   0
#define PIXEL_UNIQUE 1
#define PIXEL_FUR    2
#define PIXEL_MULTIPLY    3

// drawing modes
#define DRAWING_VOID   0
#define DRAWING_LINE   1
#define DRAWING_POINT  2
#define DRAWING_BEZIER 3

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
uniform vec4 uniform_Update_fs_4fv_paths03_r;
uniform vec4 uniform_Update_fs_4fv_paths03_g;
uniform vec4 uniform_Update_fs_4fv_paths03_b;
uniform vec4 uniform_Update_fs_4fv_paths03_a;
uniform vec4 uniform_Update_fs_4fv_paths03_BrushID;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusX;
uniform vec4 uniform_Update_fs_4fv_paths03_RadiusY;
uniform vec4 uniform_Update_fs_4fv_flashTrkBGWghts_flashPartBGWght;  // *
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
uniform vec2 uniform_Update_fs_2fv_photo01Wghts;
uniform vec4 uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H;
uniform vec4 uniform_Update_fs_4fv_CAType_SubType_blurRadius;
uniform vec2 uniform_Update_fs_2fv_initCA_1stPlaneFrameNo;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect  uniform_Update_texture_fs_CA;  // 3-cycle ping-pong CA step n (FBO attachment 3)
layout (binding = 1) uniform samplerRect  uniform_Update_texture_fs_Pixels;  // 3-cycle ping-pong speed/position of pixels step n (FBO attachment 2)
layout (binding = 2) uniform sampler3D    uniform_Update_texture_fs_Brushes;  // pen patterns
layout (binding = 3) uniform samplerRect  uniform_Update_texture_fs_Camera_frame;  // 
layout (binding = 4) uniform samplerRect  uniform_Update_texture_fs_Camera_BG;     // current background camera texture
layout (binding = 5) uniform samplerRect  uniform_Update_texture_fs_Movie_frame;  // movie textures
layout (binding = 6) uniform sampler3D    uniform_Update_texture_fs_Noise;  // noise texture for pixel acceleration 
layout (binding = 7) uniform sampler2D    uniform_Update_texture_fs_Photo0;  // photo_0 texture
layout (binding = 8) uniform sampler2D    uniform_Update_texture_fs_Photo1;  // photo_1 texture
layout (binding = 9) uniform samplerRect  uniform_Update_texture_fs_Trk0;         // 3-cycle ping-pong localColor step n (FBO attachment 1)
#if PG_NB_TRACKS >= 2
layout (binding = 10) uniform samplerRect uniform_Update_texture_fs_Trk1;  // 2-cycle ping-pong Update pass track 1 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 11) uniform samplerRect uniform_Update_texture_fs_Trk2;  // 2-cycle ping-pong Update pass track 2 step n (FBO attachment 7)
#endif
#if PG_NB_TRACKS >= 4
layout (binding = 12) uniform samplerRect uniform_Update_texture_fs_Trk3;  // 2-cycle ping-pong Update pass track 3 step n (FBO attachment 8)
#endif
// layout (binding = 8) uniform samplerRect  fs_lookupTable8;  // initial background video texture
layout (binding = 10) uniform samplerRect  uniform_Update_texture_fs_CATable;   // data tables for the CA
layout (binding = 11) uniform samplerRect  uniform_Update_texture_fs_Camera_BGIni; // initial background camera texture

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

vec2 Bezier( float x1 , float y1 , float x2 , float y2 , 
	     float mouse_x , float mouse_y , float interp_mouse_x ) {
  float a = (mouse_x - 2 * x1 + x2);
  float deltaprime = (x1 - mouse_x) * (x1 - mouse_x) 
    - a * (mouse_x - interp_mouse_x);
  if( deltaprime < 0 ) {
    return vec2( -10000 , -10000 );
  }
  else {
    float t1 = (mouse_x - x1 + sqrt( deltaprime ))/a;
    float t2 = (mouse_x - x1 - sqrt( deltaprime ))/a;
    float out_y1 = -10000;
    float out_y2 = -10000;
    if( t1 >= 0 && t1 <= 1 ) {
      out_y1 = (1-t1)*(1-t1)*mouse_y + 2*t1*(1-t1)*y1 + t1*t1*y2;
    }
    if( t2 >= 0 && t2 <= 1 ) {
      out_y2 = (1-t2)*(1-t2)*mouse_y + 2*t2*(1-t2)*y1 + t2*t2*y2;
    }
    return vec2( out_y1 , out_y2 );
  }
}


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
      	if( pixel_acc_factor > 0 ) {
      	  // acceleration
      	  surrpixel_speed += pixel_acc_factor * acceleration;
      	}
      	else {
      	  // damping
      	  surrpixel_speed += pixel_acc_factor * surrpixel_speed;
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
      	if( pixel_acc_factor > 0 ) {
      	  // acceleration
      	  surrpixel_speed += pixel_acc_factor * acceleration;
      	}
      	else {
      	  // damping
      	  surrpixel_speed += pixel_acc_factor * surrpixel_speed;
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
  // point or line stroke
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
#include_initializations

  // sound pulse
  vec3 pulse = uniform_Update_fs_4fv_pulse.rgb;
  float average_pulse = uniform_Update_fs_4fv_pulse.a;

  //////////////////////////
  // variables 

  // frame number
  frameNo = int(round(uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.y));
  
  // decay of drawing and CA layers
  CAdecay = uniform_Update_fs_4fv_CAdecay_frameno_Cursor_flashPartCAWght.x;
  
  // CAType
  CAType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.x);
  CASubType = int(uniform_Update_fs_4fv_CAType_SubType_blurRadius.y);
  CA_on_off = (CASubType > 0);
  
  // pixels
  pixel_acc_center = vec2(pixel_acc_center_0,pixel_acc_center_1);

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
    out_track_FBO[0] = vec4( 0, 0, 0, 0 );;
  }
  out_attachment_FBO[pg_FBO_fs_Pixels_attacht]
    = texture( uniform_Update_texture_fs_Pixels , decalCoords );
  out_attachment_FBO[pg_FBO_fs_CA_attacht]
    = texture( uniform_Update_texture_fs_CA , decalCoords );

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_track_FBO[0].rgb;

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PIXEL: increase radius for pixel capture
  //////////////////////////////////////
  //////////////////////////////////////
  float flashCACoef = uniform_Update_fs_4fv_flashTrkCAWghts.x;
  vec3 flashToBGCumul = vec3(0.0);
  vec4 flashToCACumul = vec4(0.0);  // RGB + GRAY
  // vec3 flashToCACumul = vec3(0.0);

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

  /////////////////////////////////////////////////
  // void drawing, copies the previous pixel, just manages decay
  // at the end of this code
  // nothing needs to be done here
  // if( drawingStroke == DRAWING_VOID ) {
  // }

  //////////////////////////////////////////////
  // STROKE INTERPOLATION FOR LINE
  // int newtrack = -1;
  float track_0_opacity = 0.0;
  float track_tablet1_opacty = 0.0;
  float tracks_opacity = 0.0;

  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // VIDEO UPDATE
  ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////
  // each track possibly covers the previous color

  vec3 photocolor = vec3( 0.0 );
  vec2 coordsImage = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y);
  if(uniform_Update_fs_2fv_photo01Wghts.x > 0) {
    coordsImage *= uniform_Update_fs_4fv_photo01_wh.xy;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.xy * (photo_scale - 1) / photo_scale;
    photocolor += uniform_Update_fs_2fv_photo01Wghts.x * texture(uniform_Update_texture_fs_Photo0, 
        coordsImageScaled ).rgb;
  }
  if(uniform_Update_fs_2fv_photo01Wghts.y > 0) {
    coordsImage *= uniform_Update_fs_4fv_photo01_wh.zw;
    vec2 coordsImageScaled = coordsImage / photo_scale + vec2(0.5) * uniform_Update_fs_4fv_photo01_wh.zw * (photo_scale - 1) / photo_scale;
    photocolor += uniform_Update_fs_2fv_photo01Wghts.y * texture(uniform_Update_texture_fs_Photo1,  
        coordsImageScaled ).rgb;
  }
  photocolor *= (vec3(photo_value) + photo_value * photo_value_pulse * pulse);

  // video texture used for drawing
  cameraCoord = vec2(decalCoordsPOT.x , 1.0 - decalCoordsPOT.y )
               * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw
               + uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.xy;
  cameraImage = texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ).rgb;

  movieCoord = vec2(decalCoordsPOT.x , 1.0-decalCoordsPOT.y )
             * uniform_Update_fs_4fv_movieWH_flashCameraTrkWght_cpTrack.xy;
  movieImage = texture(uniform_Update_texture_fs_Movie_frame, movieCoord ).rgb;

  if( BGSubtr ) {
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

  // video image = mix of movie and camera
  videocolor = cameraWeight * cameraImage 
            + movieWeight * movieImage;


  // each track possibly covers the previous color
  // for( int ind = 0 ; ind < 4 ; ind++ ) {

  // TRACK 0
  // background drawing
  uint pathStroke = DRAWING_VOID;
  for( int indPath = 0 ; indPath <= PG_NB_PATHS ; indPath++ ) {
    // pen drawing on the current track (comes last)
    if( indPath == 0 && currentDrawingTrack == 0 ) {
      pathStroke = DRAWING_LINE;
    }
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
    // path drawing on current track
    else if( indPath > 0 && indPath < 4 && path_replay_trackNo03[indPath - 1] == 0 ) {
      pathStroke = DRAWING_LINE;
    }
#endif
#if PG_NB_PATHS == 7
    // path drawing on current track
    else if( indPath >= 4 && path_replay_trackNo47[indPath - 4] == 0 ) {
      pathStroke = DRAWING_LINE;
    }
#endif
  }

  // pathStroke = DRAWING_LINE;
  if( pathStroke > 0 ) {
    track_0_opacity 
      += out_gray_drawing(
        uniform_Update_fs_4fv_paths03_x.x , 
        uniform_Update_fs_4fv_paths03_y.x , 
        uniform_Update_fs_4fv_paths03_x_prev.x , 
        uniform_Update_fs_4fv_paths03_y_prev.x , 
        uniform_Update_fs_4fv_paths03_RadiusX.x ,
        uniform_Update_fs_4fv_paths03_RadiusY.x ,
			  int(uniform_Update_fs_4fv_paths03_BrushID.x) ,
			  pathStroke );
    tracks_opacity += track_0_opacity;

    // drawing occurs on background
    if( track_0_opacity > 0 ) {
      // newtrack = 0;
      // brush opacity is combined with color opacity
      track_0_opacity *= uniform_Update_fs_4fv_paths03_a.x;
      out_track_FBO[0].rgb = 
      	out_track_FBO[0].rgb * (1 - track_0_opacity)
      	+ track_0_opacity
      	* vec3(uniform_Update_fs_4fv_paths03_r.x,
              uniform_Update_fs_4fv_paths03_g.x,
              uniform_Update_fs_4fv_paths03_b.x);
    }
  }
  
  if(cameraWeight + movieWeight > 0) {
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

    // frontier through drawing to restrict CA expansion
    // if( track_0_opacity > 0.5 ) {
    //  out4_CA = vec4(0,0,0,1);
    // }

  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Update_fs_4fv_repop_Color_flashCABGWght.w == 0
  out_track_FBO[0].rgb = clamp( out_track_FBO[0].rgb 
				    + uniform_Update_fs_4fv_repop_Color_flashCABGWght.w * out_attachment_FBO[pg_FBO_fs_CA_attacht].rgb , 0 , 1 );

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
      pixel_acceleration = vec2(snoise( pixelTextureCoordinatesXY , noiseScale * 100 ),
                                snoise( pixelTextureCoordinatesXY + vec2(2.0937,9.4872) , noiseScale * 100 ));

      vec2 acceleration;
      acceleration = pixel_acceleration - pixel_acc_center;
      if( pixel_acc_factor > 0 ) {
      	// acceleration
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy += pixel_acc_factor * acceleration;
      }
      else {
	// damping
      	out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy += pixel_acc_factor * out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy;
      }
      // updates the position of the current pixel
      out_attachment_FBO[pg_FBO_fs_Pixels_attacht].zw += out_attachment_FBO[pg_FBO_fs_Pixels_attacht].xy; 

      // if the pixel leaves the cell, it is erased
      if( abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].z ) > 0.5
    	  || abs( out_attachment_FBO[pg_FBO_fs_Pixels_attacht].w ) > 0.5 ) {
      	if( pixel_mode == PIXEL_UNIQUE ) {
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
      - vec3(uniform_Update_fs_4fv_trkDecay.x,
             uniform_Update_fs_4fv_trkDecay.x,
	           uniform_Update_fs_4fv_trkDecay.x);
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
  if( uniform_Update_fs_4fv_clearAllLayers_clearCA_pixelRadius_pulsedShift.y > 0 ) {
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
     && frameNo < uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.y + 4 * 1800
     && int(frameNo - uniform_Update_fs_2fv_initCA_1stPlaneFrameNo.y) % 1800 < 10
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
#endif
#if PG_NB_TRACKS >= 3
  out_Update_FBO_fs_Trk2 = out_track_FBO[2];
#endif
#if PG_NB_TRACKS >= 4
  out_Update_FBO_fs_Trk3 = out_track_FBO[3];
#endif
  // float state = texelFetch(uniform_Update_texture_fs_CATable, ivec2( 2 , 52 ) ).r;
  // if( state == 1.0/255.0 ) {
  //    out_Update_FBO_fs_Trk0.rgb = 0.1 * out_Update_FBO_fs_Trk0.rgb + 0.9 * vec3(1,0,0);
  //  }
  // else if( state == 2.0/255.0 ) {
  //    out_Update_FBO_fs_Trk0.rgb = 0.1 * out_Update_FBO_fs_Trk0.rgb + 0.9 * vec3(1,1,0);
  //  }
  // else if( state == 3.0/255.0 ) {
  //    out_Update_FBO_fs_Trk0.rgb = 0.1 * out_Update_FBO_fs_Trk0.rgb + 0.9 * vec3(0,1,1);
  //  }
  // else if( state == 0.0 ) {
  //    out_Update_FBO_fs_Trk0.rgb = 0.1 * out_Update_FBO_fs_Trk0.rgb + 0.9 * vec3(0,1,0);
  //  }
  // else {
  //    out_Update_FBO_fs_Trk0.rgb = 0.1 * out_Update_FBO_fs_Trk0.rgb + 0.9 * vec3(0,0,1);
  //  }
  //vec2 cameraCoord = vec2(1.0-decalCoordsPOT.x , decalCoordsPOT.y )
  //                   * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw;
  //out_Update_FBO_fs_Trk0=mix(out_track_FBO[0],texture(uniform_Update_texture_fs_Camera_frame, cameraCoord ),0.9);
  // out_Update_FBO_fs_Trk0 = vec4(1,0,0,1);

  // out_Update_FBO_fs_Trk0 = mix( out_track_FBO[0] , texture(uniform_Update_texture_fs_Camera_BG, 
  //                 decalCoordsPOT * uniform_Update_fs_4fv_Camera_offSetsXY_Camera_W_H.zw ) , 0.9 );
}
