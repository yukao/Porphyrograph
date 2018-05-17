/***********************************************************************
File: effe/shaders/LYM_Drawing_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

int       particleMode;
float     particle_texture_ID;
float     partAccCenter_0;
float     partAccCenter_1;
uniform vec4 uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1;
float     radiusParticle;
float     repopulatingBG;
float     repopulatingCA;
int       CAType;
uniform vec4 uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType;
int       CASubType;
int       currentDrawingTrack;
int       currentVideoTrack;
bool      track_replay_0;
uniform vec4 uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0;
bool      track_replay_1;
bool      track_replay_2;
float     CAParams1;
float     CAParams2;
uniform vec4 uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2;
float     CAParams3;
float     CAParams4;
float     CAParams5;
float     CAParams6;
uniform vec4 uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6;
float     CAParams7;
float     CAParams8;
bool      is_videoOn;
int       BGSubtr;
uniform vec4 uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr;
int       CAstep;
bool      CAcolorSpread;
uniform vec2 uniform_Drawing_fs_2fv_CAstep_CAcolorSpread;

// #define PG_VIDEO_ACTIVE
#define PG_NB_TRACKS 3
// #define GENERATIVE_MOVIE

float dist_to_mousePos = 0;
float unsigned_rx;
float unsigned_ry;
float signed_rx;
float signed_ry;
vec3 out_drawn_filled;

// center of the translation of acceleration values
// used for shifting particle motion in a direction
vec2 partAccCenter = vec2(0.5,0.5);

vec2 flashTrack0Coefs;

// out_particle returned values
vec3 out_color_particle;
vec2 out_position_particle = vec2(0,0);
vec2 out_speed_particle = vec2(0,0);

vec4 out4_CA;
vec4 noiseCA;
float CAdecay = 0.0;

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or particles
bool newly_drawn = false;

// cursor type (+1 for stylus , -1 for rubber )
float Cursor = 1;

/*
  vec3 out_color_gol;
*/

bool CA_on_off;
vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// CA types
#define CA_PREY                    0
#define CA_DISLOCATION             1
#define CA_PROTOCELLS              2
#define CA_SOCIAL_PD               3

// Particle variables
float acceleration_factor;

float height;
float width;

vec3 particleTextureCoordinates; // the POT coordinates of the
// particle texture + z offset according to the chosen texture

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
                                 {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
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
const float shadeSocial[4] = {0,               // empty
                                 0.5, 1.0};    // Coorperator / Defector
const float inverseShadeSocial[4] = {0,                 // empty
                                        2.0 , 1.0};     // Coorperator / Defector
// const vec3 colorSocialPD[3] = {{0,0,0},      // empty
//                              {0.5,0.5,0.5},{1,1,1}};     // Coorperator / Defector

// const vec3 colorSocialChange[3] = {{0,0,0},                    // empty
//                                    {0.5,0.5,0.5},{1,1,1}};     // Coorperator / Defector

// const vec3 colorPrey[5] = {{0,0,0},{0,0,1},   // dead predator/prey
//                            {0,1,0},           // prey
//                            {1,0,0},{1,1,0}};  // hungry/full predator
// const vec3 colorDisloc[5] = {{0.3,0.3,0.3},     // empty
//                              {1,0,0},{0,0,1},   // new/old border
//                              {1,0,0},{1,1,0}};  // new/old nucleus

// const vec3 colorProtocell[4] = {{0.3,0.3,0.3},      // empty
//                              {0,1,0},{1,0,0},       // X Y
//                              {0,0,1}};              // Z
// const vec3 colorSocialPD[3] = {{0.3,0.3,0.3},      // empty
//                              {0,1,0},{1,0,0}};     // Coorperator / Defector
// const vec3 colorSocialChange[3] = {{0,0,0},                    // empty
//                                    {1.0,0.0,0.0},{0.0,1.0,0.0}};     // For / Against


// frame number
int frameNo;

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

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_x;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y_videoH;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_r;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_g;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_b;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_a;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_RadiusX;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_RadiusY;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec3 uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack;
uniform vec4 uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;  
uniform vec4 uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs;  
uniform vec4 uniform_Drawing_fs_4fv_trackdecay_clearLayer;
uniform vec3 uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal;         // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass speed/position of particles step n (FBO attachment 2)
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
#if PG_NB_TRACKS >= 3
layout (binding = 4) uniform samplerRect fs_lookupTable4;  // 2-cycle ping-pong Drawing pass track 2 step n (FBO attachment 5)
#endif
layout (binding = 5) uniform sampler3D   fs_lookupTable5;  // pen patterns
layout (binding = 6) uniform sampler3D   fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
layout (binding = 7) uniform samplerRect fs_lookupTable7;  // video texture
layout (binding = 8) uniform samplerRect fs_lookupTable8;  // current background video texture
layout (binding = 9) uniform samplerRect fs_lookupTable9;  // initial background video texture
#endif

/////////////////////////////////////
// BACKGROUND COLOR OUTPUT
layout (location = 0) out vec4 outColor0;
// the local color copied from the ping pong color FBO
// the out_localColor_FBO and out_CA_FBO are copied from the preceding buffer
// value unless we are in the CA mode where it is averaged
// between neighbors
// the ALPHA canal contains the CA gray level 
// the RGB channels contain the local color

/////////////////////////////////////
// PARTICLE OUTPUT
layout (location = 1) out vec4 outColor1;      // 
// the RG channels contain the particle speed
// the BA channels contain the particle position

/////////////////////////////////////
// CA & TRACK COLOR OUTPUT
layout (location = 2) out vec4 outColor2;      // 
layout (location = 3) out vec4 outColor3;      // 
layout (location = 4) out vec4 outColor4;      // 
// the RGB channels contains the track Color

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

void CA_out( vec2 decalCoords , vec4 currentCA ) {
  // the ALPHA canal of fs_lookupTable2 contains > 0 if it is a live cell

  // gets Moore neighbor values
  neighborValues[0] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[0] );
  neighborValues[1] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[1] );
  neighborValues[2] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[2] );
  neighborValues[3] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[3] );
  neighborValues[4] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[4] );
  neighborValues[5] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[5] );
  neighborValues[6] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[6] );
  neighborValues[7] = texture(fs_lookupTable2, 
			      decalCoords + neighborOffsets[7] );

  vec4 noiseCA2 = texture( fs_lookupTable6, 
                          vec3( vec2(1,1) - particleTextureCoordinates.xy , 
                                6.5 / 8.0 ) );
#ifndef GENERATIVE_MOVIE
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
      out4_CA.a = round(noiseCA.x * (nbStates+1)); // nbStates states randomly
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
            && pow( noiseCA.x , nbSurroundingPreys ) 
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
            && pow( noiseCA.x , nbSurroundingFullPredators ) 
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
            && pow( noiseCA.x , nbSurroundingHungryPredators ) 
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
            && pow( noiseCA.x , nbSurroundingPreys ) 
               < CAParams3 ) {
          newState = 4; // survives in state full
        }
        else { // dies with a certain probability
          if( noiseCA.x < CAParams4 ) {
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
        if( noiseCA.x < CAParams5 ) {
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
      out4_CA.a = noiseCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      // newState = 3;
      out4_CA.rgb = shadeDisloc[ newState ].r * currentCA.rgb;
    }
    else {
      int indexOld = -1;
      int firstindex = int(noiseCA.x * 8);
      // looks for an old border around
      for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
        if( neighborValues[ind % 8].a == 2 ) {
          indexOld = ind;
        }
      }
      int indexNew = -1;
      firstindex = int(noiseCA.y * 8);
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
                && noiseCA.y < CAParams2 ) {
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
          if( noiseCA.y < CAParams3 ) {
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

  //////////////////////////////////////////////////////
  // PROTOCELLS
  else 
#endif

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
      out4_CA.a = noiseCA.x * (nbStates+1); // nbStates states randomly
      newState = int( clamp(out4_CA.a,0,nbStates) );
      newState = 3;
      out4_CA.rgb = shadeProtocell[ newState ].r * currentCA.rgb;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      newState = state;
      // parameters CA thresholds
      // CAParams1: px - X duplication from X+Y (noiseCA.y)
      // CAParams2: py - Y duplication from Y+Z (noiseCA.z)
      // CAParams3: pz - Z duplication from Z+X (noiseCA.w)

      // empty cell: replication or migration from neighboring cells
      if( state ==  0 ) { // empty
        newState = 0; // stays empty
        int firstindex = clamp(int(noiseCA2.x * 8),0,8);
        // has a neighboring X and a neighboring Y slot
        for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
          if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 2 
              && noiseCA.x < CAParams1) {
            newState = 1; // new X
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Y and a neighboring Z slot
          int firstindex = clamp(int(noiseCA2.y * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 6 
                && noiseCA.y < CAParams2) {
              newState = 2; // new Y
            }
          }
        }

        // did not replicate at the preceding loop
        if( newState == 0 ) {
          // has a neighboring Z and a neighboring X slot
          int firstindex = clamp(int(noiseCA2.z * 8),0,8);
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a * neighborValues[(ind+1)%8].a == 3 
                && noiseCA.z < CAParams3) {
              newState = 3; // new Z
            }
          }
        }

        // migration
        if( newState == 0 && nbNeighbors < 4 ) {
          // random selection of a neighbor
          int firstindex = clamp(int(noiseCA.x * 8),0,8);
          newState = 0;
          // looks for an old border around
          for( int ind = firstindex ; ind < firstindex + 8 ; ind++ ) {
            if( neighborValues[ind % 8].a != 0 ) {
              newState = int(neighborValues[ind % 8].a);
            }
          }
        }
      }
      // CAParams5: px - X/Y/Z degradation (noiseCA2.x)
      // noiseCA2.y choice of a first neighbor
      // degradation or migration of X
      else if( noiseCA2.x < CAParams4 
        || nbNeighbors < 8 * CAParams5 ) { 
        newState = 0; 
      }

      /* if( state ==  1 ) {
        // survives
        newState = state; 
        // probability of degradation
        // or migration if there is a free neighboring cells
        if( noiseCA2.x < CAParams4
            || nbNeighbors < 8 ) {
          newState = 0; //dies or migrates
        }
         out4_CA.a = float(newState);
      }
      // degradation or migration of Y
      else if( state ==  2 ) {
        // survives
        newState = state; 
        // probability of degradation
        // or migration if there is a free neighboring cells
        if( noiseCA2.y < CAParams5
            || nbNeighbors < 8 ) {
          newState = 0; //dies or migrates
        }
         out4_CA.a = float(newState);
      }
      // degradation or migration of Z
      else if( state ==  3 ) {
        // survives
        newState = state; 
        // probability of degradation
        // or migration if there is a free neighboring cells
        if( noiseCA2.z < CAParams6
            || nbNeighbors < 8 ) {
          newState = 0; //dies or migrates
        }
      } */
      out4_CA.a = float(newState);
      averageSurrounding = clamp( averageSurrounding - vec3(CAdecay) , 0.0 , 1.0 );
      out4_CA.rgb = shadeProtocell[ newState ] * averageSurrounding;
      // out4_CA.rgb = vec3(shadeProtocell[ newState ]);
    }
  }

#ifndef GENERATIVE_MOVIE
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
      out4_CA.a = noiseCA.x * (nbStates+1); // nbStates states randomly
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
      int firstIndex = int(noiseCA.y * 9)%8;
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
          if( noiseCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
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
              if( noiseCA.z > CAParams6 ) { // ****** payoffCalc uses parameters 1 to 4
                newState = 3 - int(neighborValues[indMax].a);
              }
              // or stays in its current state
          }
          //dead
          else {
              if( noiseCA.z > CAParams7 ) {
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
          if( noiseCA.z > CAParams6 ) {
             newState = int(neighborValues[indMax].a);
          }
          else {
                newState = 3 - int(neighborValues[indMax].a);
          }
        }
        else {
          if( noiseCA.z > 1 - CAParams7 ) {
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
#endif

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = currentCA; // ready
  }

}

void particle_out( vec2 decalCoords ) {
  // fills by cumulating the neighboring particles that
  // can reach the pixel at this frame (come into a circle of radius radiusParticle)
  // with the pixel seen as length unit.
  out_color_particle = vec3(0,0,0);
  int nb_cumultated_particles = 0;

  vec2 usedNeighborPos;

  // looks in the 4 neighbors (S,N,E,W) whether one of them is 
  // going to reach the current cell
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    usedNeighborPos = neighborOffsets[ind];
    vec2 newDecalCoord = decalCoords + usedNeighborPos;
    if( newDecalCoord.x >= 0 && newDecalCoord.y >= 0 
      	&& newDecalCoord.x < width && newDecalCoord.y < height ) {
      vec4 surrParticle_localColor;
      surrParticle_localColor = texture( fs_decal, newDecalCoord );

      // if the particle is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the loclation of the
      // particle in the coordinates system of the current pixel
      // expected to be in [-radiusParticle,+radiusParticle]x[-radiusParticle,+radiusParticle]
      if( graylevel(surrParticle_localColor) >  0 ) {
        vec2 surrParticle_speed;
        vec2 surrParticle_position;
        vec2 surrParticle_nextPosition;

        vec4 surrParticle_speed_position = texture( fs_lookupTable1,
						    newDecalCoord );
        surrParticle_speed = surrParticle_speed_position.xy;
        surrParticle_position = surrParticle_speed_position.zw;
        vec4 double_accelerations;
        double_accelerations
            = texture( fs_lookupTable6, // decalCoords  / (w,h)
		       particleTextureCoordinates
		       + vec3( usedNeighborPos / vec2(width,height) , 0.0 ) );
      	vec2 acceleration;
      	acceleration = double_accelerations.rg - partAccCenter;
      	if( acceleration_factor > 0 ) {
      	  // acceleration
      	  surrParticle_speed += acceleration_factor * acceleration;
      	}
      	else {
      	  // damping
      	  surrParticle_speed += acceleration_factor * surrParticle_speed;
      	}
      	surrParticle_nextPosition 
      			     = usedNeighborPos + surrParticle_position + surrParticle_speed; 
      	// the current step is added to the position

        if( abs(surrParticle_nextPosition.x) <= 0.5 
                  && abs(surrParticle_nextPosition.y) <= 0.5 ) {
          out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += surrParticle_speed;
          // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
        else if( abs(surrParticle_nextPosition.x) <= radiusParticle
                  && abs(surrParticle_nextPosition.y) <= radiusParticle ) {
          float dist = radiusParticle - length(surrParticle_nextPosition);
          out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += dist * (noiseCA.zy - vec2(0.5));
          // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
      }
    }
  }

  // looks in the 4 neighbors (SE,NW,NE,SW) whether one of them is 
  // going to reach the current cell
  for( int ind = 4 ; ind < 8 ; ind++ ) {
    usedNeighborPos = neighborOffsets[ind];
    vec2 newDecalCoord = decalCoords + usedNeighborPos;
    if( newDecalCoord.x >= 0 && newDecalCoord.y >= 0 
	&& newDecalCoord.x < width && newDecalCoord.y < height ) {

      vec4 surrParticle_localColor;
      surrParticle_localColor = texture( fs_decal,
					 newDecalCoord );

      // if the particle is graphically active, its local 
      // displacement is computed by using its speed, and
      // the acceleration found in the continuous texture
      // and the position corresponding to the location of the
      // particle in the coordinates system of the current pixel
      // expected to be in [-radiusParticle,+radiusParticle]x[-radiusParticle,+radiusParticle]
      if( graylevel(surrParticle_localColor) >  0 ) {
        vec2 surrParticle_speed;
        vec2 surrParticle_position;
        vec2 surrParticle_nextPosition;

        vec4 surrParticle_speed_position
	  = texture( fs_lookupTable1,
		     newDecalCoord );
        surrParticle_speed = surrParticle_speed_position.xy;
        surrParticle_position = surrParticle_speed_position.zw;

        vec4 double_accelerations;
        double_accelerations
	    = texture( fs_lookupTable6, // decalCoords  / (w,h)
		       particleTextureCoordinates
		       + vec3( usedNeighborPos / vec2(width,height) , 0.0 ) );
	vec2 acceleration;
	acceleration = double_accelerations.rg - partAccCenter;
	if( acceleration_factor > 0 ) {
	  // acceleration
	  surrParticle_speed += acceleration_factor * acceleration;
	}
	else {
	  // damping
	  surrParticle_speed += acceleration_factor * surrParticle_speed;
	}
	surrParticle_nextPosition 
			     = usedNeighborPos + surrParticle_position + surrParticle_speed; 
	// the current step is added to the position

	if( abs(surrParticle_nextPosition.x) <= radiusParticle 
            && abs(surrParticle_nextPosition.y) <= radiusParticle ) {
	  out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += surrParticle_speed;
	  // computes the position of the particle
          out_position_particle += surrParticle_nextPosition;
          nb_cumultated_particles++;
        }
      }
    }
  }

  out_position_particle 
    = normalize( out_position_particle + vec2( 0.5, 0.5 ) ) 
    - vec2( 0.5, 0.5 );

  if( graylevel(out_color_particle) > 0 ) {
    out_color_particle /= nb_cumultated_particles;
    out_color_particle = clamp( out_color_particle , 0.0 , 1.0 );
  }
  else {
    out_color_particle = vec3(0,0,0);
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
      = texture(fs_lookupTable5,
		vec3( normalized_vector_to_mousePosition.x/2.0+0.5,
		      0.5 - normalized_vector_to_mousePosition.y/2.0,
          (current_brushID / 4) * 0.25 + 0.125))[current_brushID % 4];
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

    // in case of point drawing only points are drawn when flashTrack0Coefs is not null
    // otherwise the pen is considered out of the screen
    if( p_track_drawingStroke == DRAWING_POINT 
	      && !(currentDrawingTrack > 0 && flashTrack0Coefs[currentDrawingTrack - 1] > 0) ) {
      interp_mouse_x = 10000;
      interp_mouse_y = 10000;
    }

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
  particleMode = int(uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1[0]);
  particle_texture_ID = uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1[1];
  partAccCenter_0 = uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1[2];
  partAccCenter_1 = uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1[3];
  radiusParticle = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType[0];
  repopulatingBG = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType[1];
  repopulatingCA = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType[2];
  CAType = int(uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType[3]);
  CASubType = int(uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0[0]);
  currentDrawingTrack = int(uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0[1]);
  currentVideoTrack = int(uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0[2]);
  track_replay_0 = (uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0[3] > 0 ? true : false);
  track_replay_1 = (uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2[0] > 0 ? true : false);
  track_replay_2 = (uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2[1] > 0 ? true : false);
  CAParams1 = uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2[2];
  CAParams2 = uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2[3];
  CAParams3 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6[0];
  CAParams4 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6[1];
  CAParams5 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6[2];
  CAParams6 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6[3];
  CAParams7 = uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr[0];
  CAParams8 = uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr[1];
  is_videoOn = (uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr[2] > 0 ? true : false);
  BGSubtr = int(uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr[3]);
  CAstep = int(uniform_Drawing_fs_2fv_CAstep_CAcolorSpread[0]);
  CAcolorSpread = (uniform_Drawing_fs_2fv_CAstep_CAcolorSpread[1] > 0 ? true : false);

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
#if PG_NB_TRACKS >= 3
  vec4 out_track2_FBO; // color of track 2
#endif

  //////////////////////////
  // variables 

  // frame number
  frameNo = int(round(uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack.y));
  
  // track number: 0 ==  bg and 1-2 == 2 playing tracks
  // track on which tablet drawing takes place
  currentDrawingTrack = int(uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack.z);
  // track on which video is displayed

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);
  // drawingStroke = DRAWING_LINE;

  // decay of drawing and CA layers
  CAdecay = uniform_Drawing_fs_3fv_CAdecay_frameno_currentDrawingTrack.x;
  
  // cellular automata
  CA_on_off = (CASubType > 0);

  // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor.w;

  // particles
  acceleration_factor = uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA.x;
  partAccCenter = vec2(partAccCenter_0,partAccCenter_1);

  // working variables for screen dimension
  width = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.x;
  height = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // particle texture + z offset according to the chosen texture
  vec2 noise;
  noise = texture( fs_lookupTable6, 
		   vec3( 1.0 + sin(frameNo/50000.0),
			 1.0 + cos(frameNo/50000.0),
			 3.5  / 8.0 ) ).rg; // 
  particleTextureCoordinates =
    vec3( decalCoordsPOT + noise , 
	  particle_texture_ID / 7.0 );//+ 5.0 + sin(frameNo/10000.0) * 5) 
  
  // another noise for particles and CA
  noiseCA = texture( fs_lookupTable6, 
                          vec3( particleTextureCoordinates.xy , 
                                6.5 / 8.0 ) );

  /* particleTextureCoordinates *=  */
  /*   vec3(  texture( fs_lookupTable6, vec3( 1.0 + cos(frameNo/1000.0),1.0 + 1.0 + sin(frameNo/1000.0),particle_texture_ID * 0.2 + 0.1 ) ).rg , 1.0); */

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
#if PG_NB_TRACKS >= 3
  out_track2_FBO = vec4( 0, 0, 0, 0 );
#endif
  if(frameNo <= 10 || uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA.y > 0) {
    outColor0 = vec4(0,0,0,1);
    outColor1 = vec4(0,0,0,1);
    outColor2 = vec4(0,0,0,0);
    outColor3 = vec4(0,0,0,1);
    outColor4 = vec4(0,0,0,1);
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

  // possible horizontal or vertical translation of background texture
  vec2 decalCoordsPrevStep = decalCoords;
    // + vec2( uniform_Drawing_fs_3iv_mouseTracks_x_transl[0] ,
	   //  uniform_Drawing_fs_3iv_mouseTracks_y_transl[0] );
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_localColor_FBO = texture( fs_decal , decalCoordsPrevStep );
  }
  else {
    out_localColor_FBO = vec4( 0, 0, 0, 0 );;
  }
  out_speed_position_FBO
    = texture( fs_lookupTable1 , decalCoords );
  out_CA_FBO
    = texture( fs_lookupTable2 , decalCoords );

  // possible horizontal or vertical translation of track1 texture
  decalCoordsPrevStep = decalCoords;
#if PG_NB_TRACKS >= 2
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track1_FBO = texture( fs_lookupTable3 , decalCoordsPrevStep );
  }
  else {
    out_track1_FBO = vec4( 0, 0, 0, 0 );;
  }
#endif

  // possible horizontal or vertical translation of track2 texture
  decalCoordsPrevStep = decalCoords;
#if PG_NB_TRACKS >= 3
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track2_FBO = texture( fs_lookupTable4 , decalCoordsPrevStep );
  }
  else {
    out_track2_FBO = vec4( 0, 0, 0, 0 );;
  }
#endif

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_localColor_FBO.rgb;

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PARTICLE: increase radius for particle capture
  //////////////////////////////////////
  //////////////////////////////////////
  flashTrack0Coefs = uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12.yz;
  vec3 flashCACoefs = uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs.yzw;
  vec3 trackColorCopy = vec3(0.0);
  vec4 trackCACopy = vec4(0.0);
  // vec3 trackCACopy = vec3(0.0);

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
  float track_1_opacity = 0.0;
  float track_2_opacity = 0.0;
  // each track possibly covers the previous color

#ifdef PG_VIDEO_ACTIVE
  float videoGray = 0.0;
  vec3 videocolor = vec3( 0.0 );
  vec2 videoCoord = vec2(0.0);
  // VIDEO FRAME CAPTURE AND RENDERING
  if( is_videoOn )  
  {
    // video texture used for drawing
    videoCoord = vec2(1.0-decalCoordsPOT.x , decalCoordsPOT.y )
                 * vec2( uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW.w,
                         uniform_Drawing_fs_4fv_mouseTracks_y_videoH.w );

    if( BGSubtr == 0 ) {
      videocolor = vec3(1) - texture(fs_lookupTable7, videoCoord ).rgb; // no subtraction
    }
    else if( BGSubtr == 1 ) {
      videocolor = abs(texture(fs_lookupTable7, videoCoord ).rgb 
                            - texture(fs_lookupTable9, videoCoord ).rgb); // initial background subtraction
    }
    else if( BGSubtr == 2 ) {
      videocolor = abs(texture(fs_lookupTable7, videoCoord ).rgb 
                            - texture(fs_lookupTable8, videoCoord ).rgb); // current background subtraction
    }
    videoGray = graylevel(videocolor);
    if( videoGray < uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold.w ) {
      videocolor = vec3(0.0);
    }
  }
#endif

#if PG_NB_TRACKS >= 2
  // TRACK 1
  int track_drawingStroke = DRAWING_VOID;
  if( track_replay_1 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentDrawingTrack == 1 ) {
    track_drawingStroke = drawingStroke;
  }
  if( track_drawingStroke > 0 || track_1_opacity > 0 ) {
    track_1_opacity += out_gray_drawing( uniform_Drawing_fs_3fv_mouseTracks_x[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_videoH[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold[ 1 ] ,
					uniform_Drawing_fs_3fv_mouseTracks_RadiusX[ 1 ] ,
					uniform_Drawing_fs_3fv_mouseTracks_RadiusY[ 1 ] ,
					int(uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor[ 1 ]) ,
					track_drawingStroke ) 
          * uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 1 ]  // brush opacity is combined with color opacity
          * Cursor;   // -1 for rubber and +1 for Stylus

    // drawing occurs on track 1
    out_track1_FBO.rgb = out_track1_FBO.rgb * clamp((1 - track_1_opacity),0,1)
    	+ track_1_opacity
    	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 1 ] , 
    		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 1 ] , 
    		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 1 ] );
    out_track1_FBO.a = 1.0;//track_1_opacity;

    out_track1_FBO = clamp( out_track1_FBO , 0.0 , 1.0 );
  }
#ifdef PG_VIDEO_ACTIVE
  else if( videoGray > 0 && currentVideoTrack == 1 ) {
    out_track1_FBO = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
  }
#endif

  trackColorCopy = flashTrack0Coefs[0] * out_track1_FBO.rgb;
  // trackCACopy = out_track1_FBO.rgb;
  trackCACopy = flashCACoefs[1] * vec4(out_track1_FBO.rgb,graylevel(out_track1_FBO.rgb));
#endif

#if PG_NB_TRACKS >= 3
  // TRACK 2
  track_drawingStroke = DRAWING_VOID;
  if( track_replay_2 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentDrawingTrack == 2 ) {
    track_drawingStroke = drawingStroke;
  }
  if( track_drawingStroke > 0 || track_2_opacity > 0  ) {
    track_2_opacity += out_gray_drawing( uniform_Drawing_fs_3fv_mouseTracks_x[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_videoH[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold[ 2 ] ,
					uniform_Drawing_fs_3fv_mouseTracks_RadiusX[ 2 ] ,
					uniform_Drawing_fs_3fv_mouseTracks_RadiusY[ 2 ] ,
					int(uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor[ 2 ]) ,
					track_drawingStroke ) 
         * uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 2 ]  // brush opacity is combined with color opacity
         * Cursor;   // -1 for rubber and +1 for Stylus

    // drawing occurs on track 2
    if( track_2_opacity > 0 ) {
      out_track2_FBO.rgb = out_track2_FBO.rgb * clamp((1 - track_2_opacity),0,1)
      	+ track_2_opacity
      	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 2 ] , 
      		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 2 ] , 
      		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 2 ] );
      out_track2_FBO.a = 1.0;//track_2_opacity;

      out_track2_FBO = clamp( out_track2_FBO , 0.0 , 1.0 );
    }  
  }
#ifdef PG_VIDEO_ACTIVE
  else if( videoGray > 0 && currentVideoTrack == 2 ) {
    out_track2_FBO = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
  }
#endif

  trackColorCopy += flashTrack0Coefs[1] * out_track2_FBO.rgb;
  trackCACopy += flashCACoefs[2] * vec4(out_track2_FBO.rgb,graylevel(out_track2_FBO.rgb));
#endif

  // TRACK 0
  // background drawing
  track_drawingStroke = DRAWING_VOID;
  if( track_replay_0 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentDrawingTrack == 0 ) {
    track_drawingStroke = drawingStroke;
  }
  // track_drawingStroke = DRAWING_LINE;
  if( track_drawingStroke > 0  || background_drawing_opacity > 0 ) {
    background_drawing_opacity 
      += out_gray_drawing( uniform_Drawing_fs_3fv_mouseTracks_x[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_y_videoH[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_x_prev_videoW[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_y_prev_videoThreshold[ 0 ] ,
			  uniform_Drawing_fs_3fv_mouseTracks_RadiusX[ 0 ] ,
			  uniform_Drawing_fs_3fv_mouseTracks_RadiusY[ 0 ] ,
			  int(uniform_Drawing_fs_4fv_mouseTracks_BrushID_Cursor[ 0 ]) ,
			  track_drawingStroke )  
         * uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 0 ]  // brush opacity is combined with color opacity
         * Cursor;   // -1 for rubber and +1 for Stylus

    // drawing occurs on background
    if( background_drawing_opacity > 0 ) {
      // newtrack = 0;
      out_localColor_FBO.rgb = 
      	out_localColor_FBO.rgb * clamp((1 - background_drawing_opacity),0,1)
      	+ background_drawing_opacity
      	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 0 ] , 
      		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 0 ] , 
      		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 0 ] );

      out_localColor_FBO = clamp( out_localColor_FBO , 0.0 , 1.0 );
    }
  }
#ifdef PG_VIDEO_ACTIVE
  else if( videoGray > 0 && currentVideoTrack == 0 ) {
    out_localColor_FBO = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
  }
#endif

  trackCACopy += flashCACoefs[0] * vec4(out_localColor_FBO.rgb,graylevel(out_localColor_FBO.rgb));
  
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
    }

     // calculates the new state of the automaton
    CA_out( decalCoords , currentCA );


    // CA "REPOPULATION"
    if( repopulatingCA > 0 ) {
      // if the particle noise is equal to frame % 10000 the cell is repopulated with a particle
      if( int(frameNo*repopulatingCA) % 5457 == int(noiseCA.x * 9.5
          + noiseCA.y * 231.0
          + noiseCA.z * 3957.0 ) ) {
          out4_CA.a = -1.0;
          out4_CA.rgb 
            = vec3(uniform_Drawing_fs_3fv_mouseTracks_Color_r[currentDrawingTrack],
              uniform_Drawing_fs_3fv_mouseTracks_Color_g[currentDrawingTrack],
              uniform_Drawing_fs_3fv_mouseTracks_Color_b[currentDrawingTrack]);
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_CA_FBO = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs.x == 0
  out_localColor_FBO.rgb = clamp( out_localColor_FBO.rgb 
				    + uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs.x * out_CA_FBO.rgb , 0 , 1 );

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: PARTICLE "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( particleMode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH PARTICLE: increase radius for particle capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12.x > 0 ) {
      radiusParticle = 1;
    }

    particle_out( decalCoords );

    // arrival of a new particle
    if( graylevel(out_color_particle) > 0 ) {
      // outputs the color from particle move at drawing layer
      out_localColor_FBO.rgb = out_color_particle;
      out_speed_position_FBO 
	     = vec4( out_speed_particle , out_position_particle );
    }
    // update of the current particle
    else {
      //  modifies speed according to acceleration
      vec4 double_accelerations;
	    double_accelerations
	     = texture( fs_lookupTable6, // decalCoords  / (w,h)
		     particleTextureCoordinates );
      vec2 acceleration;
      acceleration = double_accelerations.rg - partAccCenter;
      if( acceleration_factor > 0 ) {
      	// acceleration
      	out_speed_position_FBO.xy += acceleration_factor * acceleration;
      }
      else {
      	// damping
      	out_speed_position_FBO.xy += acceleration_factor * out_speed_position_FBO.xy;
      }
      // updates the position of the current particle
      out_speed_position_FBO.zw += out_speed_position_FBO.xy; 

      // if the particle leaves the cell, it is erased
      if( abs( out_speed_position_FBO.z ) > 0.5
      	  || abs( out_speed_position_FBO.w ) > 0.5 ) {
      	if( particleMode == PARTICLE_UNIQUE ) {
      	  out_localColor_FBO.rgb = vec3(0,0,0);
      	}
      }
    }
  }

  // PARTICLE "ADDITION"
  if( repopulatingBG > 0 ) {
    // if the particle noise is equal to frame % 10000 the cell is repopulated with a particle
    if( int(frameNo*repopulatingBG) % 10000 == int(noiseCA.x * 21.5
        + noiseCA.y * 462.0
        + noiseCA.z * 8000.0 ) ) {
        out_localColor_FBO.rgb = vec3(uniform_Drawing_fs_3fv_mouseTracks_Color_r[currentDrawingTrack],
              uniform_Drawing_fs_3fv_mouseTracks_Color_g[currentDrawingTrack],
              uniform_Drawing_fs_3fv_mouseTracks_Color_b[currentDrawingTrack]);
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
      - vec3(uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 0 ],uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 0 ],
	     uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 0 ]);
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
  if( currentDrawingTrack == 0 && uniform_Drawing_fs_4fv_trackdecay_clearLayer.w > 0 ) {
    out_speed_position_FBO = vec4(0,0,0,0);
    out_localColor_FBO.rgb= vec3(0,0,0);
  }

#if PG_NB_TRACKS >= 2
  // TRACK 1 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_track1_FBO.rgb) > 0 ) {
    out_track1_FBO.rgb = out_track1_FBO.rgb - vec3(uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ],
						   uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ],uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ]);
  }
  out_track1_FBO.rgb 
    = clamp( out_track1_FBO.rgb , 0.0 , 1.0 );
  // clear layer
  if( currentDrawingTrack == 1 && uniform_Drawing_fs_4fv_trackdecay_clearLayer.w > 0 ) {
    out_track1_FBO.rgb= vec3(0,0,0);
  }
#endif

#if PG_NB_TRACKS >= 3
  // TRACK 2 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_track2_FBO.rgb) > 0 ) {
    out_track2_FBO.rgb = out_track2_FBO.rgb - vec3(uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 2 ],
						   uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 2 ],uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 2 ]);
  }
  out_track2_FBO.rgb 
    = clamp( out_track2_FBO.rgb , 0.0 , 1.0 );
  if( currentDrawingTrack == 2 && uniform_Drawing_fs_4fv_trackdecay_clearLayer.w > 0 ) {
    out_track2_FBO.rgb= vec3(0,0,0);
  }
#endif

  // copy tracks
  if( uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12.w > 0 ) {
#if PG_NB_TRACKS >= 3
    if( currentDrawingTrack == 1 ) {
      out_track2_FBO = out_track1_FBO;
    }
    if( currentDrawingTrack == 2 ) {
      out_track1_FBO = out_track2_FBO;
    }
#endif
  }

  // CA LAYER CLEAR
  if( uniform_Drawing_fs_3fv_partAcc_clearAllLayers_clearCA.z > 0 ) {
      out_CA_FBO = vec4(0);
  }

  // FBO OUTPUTS
  outColor0 = out_localColor_FBO;
  outColor1 = out_speed_position_FBO;
  outColor2 = out_CA_FBO;
#if PG_NB_TRACKS >= 2
  outColor3 = out_track1_FBO;
#endif
#if PG_NB_TRACKS >= 3
  outColor4 = out_track2_FBO;
#endif
}
