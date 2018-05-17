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
float     trackdecay_sign_0;
float     trackdecay_0;
uniform vec4 uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0;
bool      is_videoOn;
float     videoThreshold;
float     videoGamma;
int       BGSubtr;
uniform vec4 uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr;
int       CAType;
int       CASubType;
float     CAParams1;
float     CAParams2;
uniform vec4 uniform_Drawing_fs_4fv_CAType_CASubType_CAParams1_CAParams2;
float     CAParams3;
float     CAParams4;
float     CAParams5;
float     images_weights_scale;
uniform vec4 uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_images_weights_scale;
float     image_value;
float     image_value_pulse;
float     image_satur;
float     image_satur_pulse;
uniform vec4 uniform_Drawing_fs_4fv_image_value_image_value_pulse_image_satur_image_satur_pulse;
float     mask_scale;
float     image_scale;
float     mask_contrast;
float     image_contrast;
uniform vec4 uniform_Drawing_fs_4fv_mask_scale_image_scale_mask_contrast_image_contrast;
bool      freeze;
uniform float uniform_Drawing_fs_1fv_freeze;

#define PG_VIDEO_ACTIVE
#define PG_NB_TRACKS 2
#define PG_WITH_CA

const float PI = 3.1415926535897932384626433832795;

float dist_to_mousePos = 0;
float signed_rx;
float signed_ry;

int CAstep = 1;

// center of the translation of acceleration values
// used for shifting particle motion in a direction
vec2 partAccCenter = vec2(0.5,0.5);

vec2 flashTrack0Coefs;

// out_particle returned values
vec3 out_color_particle;
vec2 out_position_particle = vec2(0,0);
vec2 out_speed_particle = vec2(0,0);

#ifdef PG_WITH_CA
vec4 out4_CA;
float CAdecay = 0.0;
bool CA_on_off;

// CA types
#define CA_PREY                    0
#define CA_DISLOCATION             1
#define CA_PROTOCELLS              2
#define CA_SOCIAL_PD               3
#define CA_PATHS                   4
// #define CA_LETSGO                  5

#endif

vec4 noiseCA;

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or particles
bool newly_drawn = false;

// cursor type (+1 for stylus , -1 for rubber )
float Cursor = 1;

vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesHV[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
                                    vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// Particle variables
float acceleration_factor;

float height;
float width;

vec3 particleTextureCoordinates; // the POT coordinates of the
// particle texture + z offset according to the chosen texture

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


#ifdef PG_WITH_CA
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
#endif

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

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy;
uniform vec4 uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
uniform vec4 uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor;
uniform vec4 uniform_Drawing_fs_4fv_pulse;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec4 uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;  
uniform vec3 uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold;
#ifdef PG_WITH_CA
uniform vec4 uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs;
#endif

// layer weights combined with mask value to assess the visibility of a layer
uniform vec4 uniform_Drawing_fs_4fv_weights03;
uniform vec2 uniform_Drawing_fs_2fv_weights45;
uniform vec4 uniform_Drawing_fs_4fv_image_noisesxy[3];
uniform vec4 uniform_Drawing_fs_4fv_mask_noisesxy[3];

// interpolation between image buffer and swap buffer
uniform vec3 uniform_Drawing_fs_3fv_alphaSwap02;
uniform vec3 uniform_Drawing_fs_3fv_alphaSwap35;


/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect fs_decal;         // 2-cycle ping-pong Drawing pass BG track step n (FBO attachment 1)
layout (binding = 1) uniform samplerRect fs_lookupTable1;  // 2-cycle ping-pong Drawing pass speed/position of particles step n (FBO attachment 2)
#ifdef PG_WITH_CA
layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
#endif
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
layout (binding = 5) uniform sampler3D   fs_lookupTable5;  // pen patterns
layout (binding = 6) uniform sampler3D   fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
layout (binding = 7) uniform samplerRect fs_lookupTable7;  // video texture
layout (binding = 8) uniform samplerRect fs_lookupTable8;  // current background video texture
layout (binding = 9) uniform samplerRect fs_lookupTable9;  // initial background video texture
#endif
layout (binding = 10) uniform samplerRect fs_lookupTable10;  // buffer image #0
layout (binding = 11) uniform samplerRect fs_lookupTable11;  // buffer image #1 
layout (binding = 12) uniform samplerRect fs_lookupTable12;  // buffer image #2 
layout (binding = 13) uniform samplerRect fs_lookupTable13;  // buffer image #3 
layout (binding = 14) uniform samplerRect fs_lookupTable14;  // buffer image #4 
layout (binding = 15) uniform samplerRect fs_lookupTable15;  // buffer image #5 
layout (binding = 16) uniform samplerRect fs_lookupTable16;  // buffer image mask #0
layout (binding = 17) uniform samplerRect fs_lookupTable17;  // buffer image mask #1
// layout (binding = 18) uniform samplerRect fs_lookupTable18;  // buffer image mask #2
// layout (binding = 19) uniform samplerRect fs_lookupTable19;  // buffer image mask #3
layout (binding = 20) uniform samplerRect fs_lookupTable20;  // buffer swap image #0
layout (binding = 21) uniform samplerRect fs_lookupTable21;  // buffer swap image #1 
layout (binding = 22) uniform samplerRect fs_lookupTable22;  // buffer swap image #2 
layout (binding = 23) uniform samplerRect fs_lookupTable23;  // buffer swap image #3 
layout (binding = 24) uniform samplerRect fs_lookupTable24;  // buffer swap image #4 
layout (binding = 25) uniform samplerRect fs_lookupTable25;  // buffer swap image #5 

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

#ifdef PG_WITH_CA
/////////////////////////////////////
// CA & TRACK COLOR OUTPUT
layout (location = 2) out vec4 outColor2;      // 
layout (location = 3) out vec4 outColor3;      // 
#else
/////////////////////////////////////
// TRACK COLOR OUTPUT
layout (location = 2) out vec4 outColor3;      // 
#endif

float Line( float x1 , float y1 , 
	    float mouse_x , float mouse_y , float interp_mouse_x ) {
  return mouse_y + (y1 - mouse_y)/(x1 - mouse_x) * (interp_mouse_x - mouse_x);
}

#ifdef PG_WITH_CA
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

  vec4 noiseCA2 = texture( fs_lookupTable6, 
                          vec3( vec2(1,1) - particleTextureCoordinates.xy , 
                                7.5 / 8.0 ) );
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

/*  
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
*/

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

      // if( state ==  1 ) {
      //   // survives
      //   newState = state; 
      //   // probability of degradation
      //   // or migration if there is a free neighboring cells
      //   if( noiseCA2.x < CAParams4
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
      //   if( noiseCA2.y < CAParams5
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
      //   if( noiseCA2.z < CAParams6
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

/*
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
*/

 //////////////////////////////////////////////////////
  // LET'GO: DYNAMIC INITIAL CA
  /*
  else if( CAType == CA_LETSGO ) {
     uint newState = 0;
      // 100 states corresponding to 100 saturated colors
      // 1: LINES: each beat restart a column of new states on the leftmost column
      // otherwise copies the state of the pixel at 1-100 pixels on the
      // right depending on the frame number
      if( CASubType == 1 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*100.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 2: LANDSCAPE: each beat restart a column of new states on the leftmost column
      // otherwise copies the state of the pixel at 1-100 pixels on the
      // right depending on the frame number and 0-100 pixels above
      // depending on frame number
      else if( CASubType == 2 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
          float deltaY = ((frameNo %1000)/1000.)*100.;
          vec2 newCoord = decalCoords + vec2(0,deltaY * noiseCA2.x-deltaY/2);
          if(newCoord.y>height) {
            newCoord.y -= height;
          }
          if(newCoord.y<0) {
            newCoord.y += height;
          }
          newState = int(texture(fs_lookupTable2, 
             newCoord ).a);
        }
        else {
          float deltaX = (cos( PI * time / 8) + 1 ) * 400 * TVW_Letsgo_maxspeed_X;
          newState = int(texture(fs_lookupTable2, decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 3: HORIZONTAL STRIPES: each beat restart a column of new states on the leftmost column
      // otherwise applies some rule of wolfram representation of von neumann Cellular Automata
      // otherwise copies the state of the pixel at 1-100 pixels on the
      // right depending on the frame number 
      else if( CASubType == 3 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
         //const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
           //                      {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
          int wolfram_code = ( neighborValues[3].a > 50?1:0) * 4 +
          ( neighborValues[4].a > 50?1:0) * 2 +
          ( neighborValues[5].a > 50?1:0) * 1;
          // int wolframrule[8]={0,   1,   0,   1,   1,   0,   1,   0};
          // int wolframrule[8]={0,   1,   1,   1,   1,   0,   0,   0};
          int wolframrule[8]={0,   0,   0,   1,   1,   1,   0,   1};
          if(wolframrule[wolfram_code] > 0) {
            newState = uint(50 + noiseCA2.y * 50);
          }
          else {
            newState = uint(noiseCA2.z * 50);
          }
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*100.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 4: 2 HALF LANDSCAPE: the same as 3 but without reset - generally ends up in two halves
      else if( CASubType == 4 ) {
        if( decalCoords.x >= width - 1 ) {
         //const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
           //                      {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
          int wolfram_code = ( neighborValues[3].a > 50?1:0) * 4 +
          ( neighborValues[4].a > 50?1:0) * 2 +
          ( neighborValues[5].a > 50?1:0) * 1;
          // int wolframrule[8]={0,   1,   0,   1,   1,   0,   1,   0};
          // int wolframrule[8]={0,   1,   1,   1,   1,   0,   0,   0};
          int wolframrule[8]={0,   0,   0,   1,   1,   1,   0,   1};
          if(wolframrule[wolfram_code] > 0) {
            newState = uint(50 + noiseCA2.y * 50);
          }
          else {
            newState = uint(noiseCA2.z * 50);
          }
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*100.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 5: TREES: each beat restart a column of new states on the leftmost column
      // otherwise takes the average of surrounding pixels in the leftmost column
      // otherwise copies the state of the pixel at 1-30 pixels on the
      // right depending on the frame number 
      else if( CASubType == 5 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
          newState = int(0.33 * ( neighborValues[3].a +
           neighborValues[4].a +
           neighborValues[5].a));
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*30.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 6: VERTICAL STRIPES: each beat restart a column of new states on the leftmost column
      // otherwise takes the max of surrounding pixels in the leftmost column
      // otherwise copies the state of the pixel at 1-10 pixels on the
      // right depending on the frame number 
      else if( CASubType == 6 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
          newState = uint(max(max ( neighborValues[3].a,
           neighborValues[4].a ),
           neighborValues[5].a)*0.95);
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*30.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 7: CLIMBING STAIRCASE: each beat restart a column of new states on the leftmost column
      // otherwise takes the summ of the pixels at different scales vertically 
      // otherwise copies the state of the pixel at 1-10 pixels on the
      // right depending on the frame number 
      else if( CASubType == 7 ) {
        float deltaX = ((frameNo %1000)/1000.)*30.;
        int deltaY = int((((frameNo+555) %777)/777.)*200.);
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
         //const vec2 neighborOffsets[8] = {{1,0},{1,1},{0,1},{-1,1},      // E NE N NW
           //                      {-1,0},{-1,-1},{0,-1},{1,-1},};  // W SW S SE
          newState = uint(0.99 * max(max(texture(fs_lookupTable2, vec2(deltaX*0.3,int(decalCoords.y)%deltaY) ).a,
           texture(fs_lookupTable2, vec2(deltaX*0.6,int(decalCoords.y)%deltaY) ).a),
           texture(fs_lookupTable2, vec2(deltaX*0.9,int(decalCoords.y)%deltaY) ).a));
        }
        else {
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }
      // 8: MOUNTAINS: like 2 + vertical blend in last column
      else if( CASubType == 8 ) {
        if( uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.z > 0 
            && decalCoords.x >= width - 1 ) {
          newState = int(noiseCA2.x * ((decalCoords.y)/height) * 100.0) % 100;
        }
        else if( decalCoords.x >= width - 1 ) {
          float deltaY = ((frameNo %1000)/1000.)*100.;
          vec2 newCoord = decalCoords + vec2(0,deltaY * noiseCA2.y-deltaY/2);
          if(newCoord.y>height) {
            newCoord.y -= height;
          }
          if(newCoord.y<0) {
            newCoord.y += height;
          }
          newState = int(texture(fs_lookupTable2, newCoord ).a );
        }
        else {
          float deltaX = ((frameNo %1000)/1000.)*100.;
          newState = int(texture(fs_lookupTable2, 
            decalCoords + vec2(deltaX,0) ).a);
        }
      }

      // subliminal image each 333 frames
      if(cos( PI * time / 8) < -0.99 && sin( PI * time / 8) > 0) {
        int remainder = int(floor(time * 6 / 16))%6;
        vec2 coordsIm = decalCoordsPOT * vec2(1024,768);
        if(remainder == 0) {
          newState = int(graylevel(texture( fs_lookupTable10 , 
                                            coordsIm ) ) * 100);
        }
        else if(remainder == 1) {
          newState = int(graylevel(texture( fs_lookupTable11 , 
                                            coordsIm ) ) * 100);
        }
        else if(remainder == 2) {
          newState = int(graylevel(texture( fs_lookupTable12 , 
                                            coordsIm ) ) * 100);
        }
        else if(remainder == 3) {
          newState = int(graylevel(texture( fs_lookupTable13 , 
                                            coordsIm ) ) * 100);
        }
        else if(remainder == 4) {
          newState = int(graylevel(texture( fs_lookupTable14 , 
                                            coordsIm ) ) * 100);
        }
        else if(remainder == 5) {
          newState = int(graylevel(texture( fs_lookupTable15 , 
                                            coordsIm ) ) * 100);
        }
      }
 
      out4_CA.a = float(newState);
      if(newState <= 33) {
        out4_CA.rgb = vec3(1.0-float(newState)/33.0*TVW_Letsgo_satur,
                            float(newState)/33.0*TVW_Letsgo_satur,0.0);
        // pastel colors for 8
        if( CASubType == 2 ) {
            out4_CA.b += TVW_Letsgo_satur;
            out4_CA.rgb *= image_value;
        }
      }
      else if(newState <= 66) {
        newState -= 33;
        out4_CA.rgb = vec3(0.0,1.0-float(newState)/33.0*TVW_Letsgo_satur,
                            float(newState)/33.0*TVW_Letsgo_satur);
        // pastel colors for 8
        if( CASubType == 2 ) {
            out4_CA.r += TVW_Letsgo_satur;
            out4_CA.rgb *= image_value;
        }
      }
      else {
        out4_CA.rgb = vec3(float(newState)/33.0*TVW_Letsgo_satur,0.0,
                            1.0-float(newState)/33.0*TVW_Letsgo_satur);
        // pastel colors for 8
        if( CASubType == 2 ) {
            out4_CA.g += TVW_Letsgo_satur;
            out4_CA.rgb *= image_value;
        }
      }
  }
  */

/*
  //////////////////////////////////////////////////////
  // DISLOCATION
  // parameter values: 0.314  0.606 0.354 0.409
  else if( CAType == CA_PATHS ) {

    for( int ind = 0 ; ind < 8 ; ind++ ) {
      neighborValuesHV[ind] = texture(fs_lookupTable2, 
                  decalCoords + neighborOffsetsPaths[ind] );
    }
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      neighborValuesDiag[ind] = texture(fs_lookupTable2, 
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
      if( noiseCA.x > 0.5 ) {
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
*/
  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = currentCA; // ready
  }

}
#endif


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


float stroke_out( float current_Brush_RadiusX , float current_Brush_RadiusY ) {

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
    penOutGrayLevel
      = texture(fs_lookupTable5,
		vec3( normalized_vector_to_mousePosition.x/2.0+0.5,
		      0.5 - normalized_vector_to_mousePosition.y/2.0,
          0.125))[0];
 
  // pen gray level
  return penOutGrayLevel;
}

float out_gray_drawing( float current_mouse_x , float current_mouse_y , 
			float prev_mouse_x , float prev_mouse_y ,
			float current_Brush_RadiusX , float current_Brush_RadiusY , 
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
        = stroke_out( current_Brush_RadiusX , current_Brush_RadiusY );
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
  radiusParticle = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0[0];
  repopulatingBG = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0[1];
  trackdecay_sign_0 = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0[2];
  trackdecay_0 = uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0[3];
  is_videoOn = (uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr[0] > 0 ? true : false);
  videoThreshold = uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr[1];
  videoGamma = uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr[2];
  BGSubtr = int(uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr[3]);
  CAType = int(uniform_Drawing_fs_4fv_CAType_CASubType_CAParams1_CAParams2[0]);
  CASubType = int(uniform_Drawing_fs_4fv_CAType_CASubType_CAParams1_CAParams2[1]);
  CAParams1 = uniform_Drawing_fs_4fv_CAType_CASubType_CAParams1_CAParams2[2];
  CAParams2 = uniform_Drawing_fs_4fv_CAType_CASubType_CAParams1_CAParams2[3];
  CAParams3 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_images_weights_scale[0];
  CAParams4 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_images_weights_scale[1];
  CAParams5 = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_images_weights_scale[2];
  images_weights_scale = uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_images_weights_scale[3];
  image_value = uniform_Drawing_fs_4fv_image_value_image_value_pulse_image_satur_image_satur_pulse[0];
  image_value_pulse = uniform_Drawing_fs_4fv_image_value_image_value_pulse_image_satur_image_satur_pulse[1];
  image_satur = uniform_Drawing_fs_4fv_image_value_image_value_pulse_image_satur_image_satur_pulse[2];
  image_satur_pulse = uniform_Drawing_fs_4fv_image_value_image_value_pulse_image_satur_image_satur_pulse[3];
  mask_scale = uniform_Drawing_fs_4fv_mask_scale_image_scale_mask_contrast_image_contrast[0];
  image_scale = uniform_Drawing_fs_4fv_mask_scale_image_scale_mask_contrast_image_contrast[1];
  mask_contrast = uniform_Drawing_fs_4fv_mask_scale_image_scale_mask_contrast_image_contrast[2];
  image_contrast = uniform_Drawing_fs_4fv_mask_scale_image_scale_mask_contrast_image_contrast[3];
  freeze = (uniform_Drawing_fs_1fv_freeze > 0 ? true : false);

// CA Paramaters
// 0.598 0.362 0.535 0.835 0.512 0.488 0.441 0.724 0.425 0.528 0.48  0.693 0.504 0.488 0.339 0.638 0.276 0.874 0.606 0.228 0.276 0.874 0.606 0.228 0.339 0.709 0.276 0.874 0.606 0.228 0.291 0.283 0.842 0.276 0.874 0.606 0.228 0.165 0.347 0.394


  /////////////////////////////////////
  // PARTICLE  FBOOUTPUT
  vec4 out_speed_position_FBO;      // 
  /////////////////////////////////////
  // COLOR FBO OUTPUT
  vec4 out_localColor_FBO;

#ifdef PG_WITH_CA
  /////////////////////////////////////
  // CA FBO OUTPUT
  vec4 out_CA_FBO;
#endif

  /////////////////////////////////////
  // TRACK FBO OUTPUT
#if PG_NB_TRACKS >= 2
  vec4 out_track1_FBO; // color of track 1
#endif

  //////////////////////////
  // variables 

  vec3 pulse = uniform_Drawing_fs_4fv_pulse.rgb;
  float average_pulse = uniform_Drawing_fs_4fv_pulse.a;

  // frame number
  frameNo = int(round(uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time.z));

  // time
  time = uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time.w;
  
  // track number: 0 ==  bg and 1-2 == 2 playing tracks
  // in includes
  // currentDrawingTrack track on which tablet drawing takes place
  // currentVideoTrack track on which video is displayed

#ifdef PG_WITH_CA
  // decay of drawing and CA layers
  CAdecay = uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.x;
  
  // cellular automata
  CA_on_off = (CASubType > 0);
#endif

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);
  // drawingStroke = DRAWING_LINE;

  // cursor type (+1 for stylus and -1 for rubber)
  Cursor = uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.w;

  // particles
  acceleration_factor = uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time.x;
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
                                7.5 / 8.0 ) );


  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
  out_localColor_FBO = vec4( 0, 0, 0, 0 );
#ifdef PG_WITH_CA
  out_CA_FBO = vec4( 0, 0, 0, 0 );
#endif
#if PG_NB_TRACKS >= 2
  out_track1_FBO = vec4( 0, 0, 0, 0 );
#endif
  if(frameNo <= 10 || uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time.y > 0) {
    outColor0 = vec4(0,0,0,1);
    outColor1 = vec4(0,0,0,1);
#ifdef PG_WITH_CA
    outColor2 = vec4(0,0,0,0);
#endif
    outColor3 = vec4(0,0,0,1);
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
  out_localColor_FBO = texture( fs_decal , decalCoords );
  out_speed_position_FBO = texture( fs_lookupTable1 , decalCoords );
#ifdef PG_WITH_CA
  out_CA_FBO
    = texture( fs_lookupTable2 , decalCoords );
#endif
#if PG_NB_TRACKS >= 2
  out_track1_FBO = texture( fs_lookupTable3 , decalCoords );
#endif

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_localColor_FBO.rgb;

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PARTICLE: increase radius for particle capture
  //////////////////////////////////////
  //////////////////////////////////////
  flashTrack0Coefs = uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12.yz;
  vec3 trackColorCopy = vec3(0.0);
#ifdef PG_WITH_CA
  vec2 flashCACoefs = uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.zw;
  vec4 trackCACopy = vec4(0.0);
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
  float background_drawing_opacity = 0.0;
  // each track possibly covers the previous color

  // coordinates for images & masks
  vec2 coordsImage = decalCoordsPOT * vec2(1024,768);

#ifdef PG_VIDEO_ACTIVE
  float videoGray = 0.0;
  vec3 videoImage = vec3( 0.0 );
  vec3 videocolor = vec3( 0.0 );
  vec2 videoCoord = vec2(0.0);
  float mask12 = texture( fs_lookupTable17 , mod( coordsImage + uniform_Drawing_fs_4fv_mask_noisesxy[2].zw*100.0 , vec2(1024,768)) ).r;
  float flashVideoCoef = uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold.y;
  float flashVideoThreshold = uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold.z;

  // VIDEO FRAME CAPTURE AND RENDERING
  if( is_videoOn || flashVideoCoef > 0 )  
  {
    // video texture used for drawing
    videoCoord = vec2(1.0-decalCoordsPOT.x , decalCoordsPOT.y )
                 * vec2( uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.y,
                         uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.z );
    videoImage = texture(fs_lookupTable7, videoCoord ).rgb;
    // gamma correction
    videoImage = vec3( pow(videoImage.r,videoGamma) );

    if( BGSubtr == 0 ) {
      videocolor = vec3(1) - videoImage; // no subtraction
    }
    else if( BGSubtr == 1 ) {
      videocolor = abs(videoImage - texture(fs_lookupTable9, videoCoord ).rgb); // initial background subtraction
    }
    else if( BGSubtr == 2 ) {
      videocolor = abs(videoImage - texture(fs_lookupTable8, videoCoord ).rgb); // current background subtraction
    }
    videoGray = graylevel(videocolor);
    if( videoGray < uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.w ) {
      videocolor = vec3(0.0);
    }
    else {
      videocolor = vec3(1.0);
    }
  }
#endif

#if PG_NB_TRACKS >= 2
  ////////////////////////////////////////////////////////////////////////////////////
  // MOIRE / MASKS ADDITION TO LAYER #1
  ////////////////////////////////////////////////////////////////////////////////////
  // image imageBufferColor
  vec2 coordsImageScaled = coordsImage / image_scale + vec2(0.5) * vec2(1024,768) * (image_scale - 1) / image_scale;
  vec2 coordsMaskScaled = coordsImage / mask_scale + vec2(0.5) * vec2(1024,768) * (mask_scale - 1) / mask_scale;
  float mask1 = texture( fs_lookupTable16 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[0].xy ) .r;
  float mask2 = texture( fs_lookupTable16 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[0].zw ) .g;
  float mask3 = texture( fs_lookupTable16 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[1].xy ) .b;
  float mask4 = texture( fs_lookupTable17 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[1].zw ) .r;
  float mask5 = texture( fs_lookupTable17 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[2].xy ) .g;
  float mask6 = texture( fs_lookupTable17 , coordsMaskScaled + uniform_Drawing_fs_4fv_mask_noisesxy[2].zw ) .b;
  
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

  vec3 imageBufferColor = vec3(0);
  vec2 coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[0].xy , vec2(1024,768));
  // vec2 coord = coordsImageScaled;
  imageBufferColor += (texture( fs_lookupTable10 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.x)
                     + texture( fs_lookupTable20 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.x) * (mask1) * uniform_Drawing_fs_4fv_weights03[0];
  coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[0].zw , vec2(1024,768));
  // vec2 coord = coordsImageScaled;

  imageBufferColor += (texture( fs_lookupTable11 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.y)
                     + texture( fs_lookupTable21 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.y) * (mask2) * (uniform_Drawing_fs_4fv_weights03[1]); 
  coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[1].xy , vec2(1024,768));
  // vec2 coord = coordsImageScaled;
  imageBufferColor += (texture( fs_lookupTable12 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.z)
                     + texture( fs_lookupTable22 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.z) * (mask3) * (uniform_Drawing_fs_4fv_weights03[2]); 
  coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[1].zw , vec2(1024,768));
  // vec2 coord = coordsImageScaled;
  imageBufferColor += (texture( fs_lookupTable13 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.x)
                     + texture( fs_lookupTable23 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.x) * (mask4) * (uniform_Drawing_fs_4fv_weights03[3]); 
  coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[2].xy , vec2(1024,768));
  // vec2 coord = coordsImageScaled;
  imageBufferColor += (texture( fs_lookupTable14 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.y)
                     + texture( fs_lookupTable24 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.y) * (mask5) * (uniform_Drawing_fs_2fv_weights45[0]); 
  coord = mod( coordsImageScaled + uniform_Drawing_fs_4fv_image_noisesxy[2].zw , vec2(1024,768));
  // vec2 coord = coordsImageScaled;
  imageBufferColor += (texture( fs_lookupTable15 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.z)
                     + texture( fs_lookupTable25 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.z) * (mask6) * (uniform_Drawing_fs_2fv_weights45[1]); 
  // image value
  float flashValueCoef = ((flashVideoCoef <= flashVideoThreshold) ?
                  (1.0 - flashVideoCoef/flashVideoThreshold): 0.0);
  if( flashVideoCoef <= 0 ) {
    flashValueCoef = 1.0f;
  }
  image_value *= flashValueCoef;
  image_value_pulse *= flashValueCoef;
  out_track1_FBO.rgb 
    = clamp(imageBufferColor * (vec3(image_value) + image_value * image_value_pulse * pulse) , 0 , 1 );
  // out_track1_FBO.rgb = imageBufferColor;

  // image_satur image_satur_pulse
  vec3 pulsed_satur = vec3(image_satur) + image_satur * image_satur_pulse * pulse;
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
  float correctionFactor =  1.016 * (image_contrast + 1.0) 
                              / (1.016 - image_contrast);
  out_track1_FBO.rgb = clamp( vec3(correctionFactor)
    * (out_track1_FBO.rgb - vec3(0.5)) + vec3(0.5) , 0 , 1 );

#ifdef PG_VIDEO_ACTIVE
  if( is_videoOn ) 
  {
      out_track1_FBO *= vec4( clamp( vec3(1)-videocolor , 0.0 , 1.0 ) ,  1.0 );
  }
#endif

  trackColorCopy = flashTrack0Coefs[0] * out_track1_FBO.rgb;
#ifdef PG_WITH_CA
  // flash CA on track 1
  if( graylevel(out_track1_FBO.rgb) > .2 && graylevel(out_track1_FBO.rgb) < 2.1
    && flashVideoCoef == 0 ) { // no flash video
    trackCACopy = flashCACoefs[1] * vec4(imageBufferColor.rgb,graylevel(out_track1_FBO.rgb));
  }
#endif
#endif

  // TRACK 0
  // background drawing
  if( drawingStroke > 0 ) {
    background_drawing_opacity 
      += out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy[ 1 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy[ 2 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy[ 3 ] ,
			  uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor[ 0 ] ,
			  uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor[ 1 ] ,
			  drawingStroke )  
         * uniform_Drawing_fs_4fv_mouseTracks_Color_rgba.a  // brush opacity is combined with color opacity
         * Cursor;   // -1 for rubber and +1 for Stylus

    // drawing occurs on background
    if( background_drawing_opacity > 0 ) {
      // newtrack = 0;
      out_localColor_FBO.rgb = 
      	out_localColor_FBO.rgb * clamp((1 - background_drawing_opacity),0,1)
      	+ background_drawing_opacity
      	* uniform_Drawing_fs_4fv_mouseTracks_Color_rgba.rgb;

      out_localColor_FBO = clamp( out_localColor_FBO , 0.0 , 1.0 );
    }
  }
// #ifdef PG_VIDEO_ACTIVE
//   if( is_videoOn && currentVideoTrack == 0 ) {
//     videocolor.rgb *= round(mask12);
//     out_localColor_FBO = vec4( clamp( videocolor , 0.0 , 1.0 ) ,  1.0 );
//   }
// #endif
  

  ////////////////////////////////////////////////////////////////////////////////////
  // CA LAYER
  ////////////////////////////////////////////////////////////////////////////////////

#ifdef PG_WITH_CA
  // flash CA on background track
  if( flashVideoCoef == 0 ) { // no flash video 
    trackCACopy += flashCACoefs[0] * vec4(out_localColor_FBO.rgb,graylevel(out_localColor_FBO.rgb));
  }
#endif

#ifdef PG_WITH_CA
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

#ifdef PG_VIDEO_ACTIVE
    if(flashVideoCoef > 0 && flashVideoCoef >= flashVideoThreshold) {
      currentCA.a = -1;
      currentCA.rgb = videocolor;
    }
#endif

     // calculates the new state of the automaton
    CA_out( decalCoords , currentCA );

    // outputs the color from CA (average surrounding color if birth)
    out_CA_FBO = out4_CA;
  }

  //////////////////////////////////////
  // FLASH BACK FROM CA LAYER TO BG LAYER
  // if no flashback uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.x == 0
  out_localColor_FBO.rgb = clamp( out_localColor_FBO.rgb 
            + uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.x * out_CA_FBO.rgb , 0 , 1 );
#endif

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
        out_localColor_FBO.rgb = uniform_Drawing_fs_4fv_mouseTracks_Color_rgba.rgb;
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
      - vec3(trackdecay_0*trackdecay_sign_0);
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
  if( uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold.x > 0 ) {
    out_speed_position_FBO = vec4(0,0,0,0);
    out_localColor_FBO.rgb= vec3(0,0,0);
  }

#if PG_NB_TRACKS >= 2
  // TRACK 1 DECAY
  // incay or decay does not make sense on a black color
  // if( graylevel(out_track1_FBO.rgb) > 0 ) {
  //   out_track1_FBO.rgb = out_track1_FBO.rgb - vec3(uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ],
		// 				   uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ],uniform_Drawing_fs_4fv_trackdecay_clearLayer[ 1 ]);
  // }
  out_track1_FBO.rgb 
    = clamp( out_track1_FBO.rgb , 0.0 , 1.0 );
  // clear layer
#endif

#ifdef PG_WITH_CA
  // CA LAYER CLEAR
  if( uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.y > 0 ) {
      out_CA_FBO = vec4(0);
  }
#endif

#ifdef PG_VIDEO_ACTIVE
  if( flashVideoCoef > 0 ) { // flash video
    if(flashVideoCoef >= flashVideoThreshold) {
      // video image copy when there is a flash video
      out_track1_FBO.rgb = videocolor;
    }
    else {
      out_track1_FBO.rgb = out_track1_FBO.rgb;
    }
    out_localColor_FBO.rgb = vec3(0);
  }
#endif

  // FBO OUTPUTS
  if( !freeze ) {
    outColor0 = out_localColor_FBO;
    outColor1 = out_speed_position_FBO;
  }
  else {
    outColor0 = out_localColor_FBO;
    // outColor0 = texture( fs_decal , decalCoords );
    outColor1 = out_speed_position_FBO;
    // outColor1 = texture( fs_lookupTable1 , decalCoords );
  }
#ifdef PG_WITH_CA
  outColor2 = out_CA_FBO;
#endif
#if PG_NB_TRACKS >= 2
  if( !freeze ) {
    outColor3 = out_track1_FBO;
  }
  else {
    outColor3 = texture( fs_lookupTable3 , decalCoords );
  }
  // outColor3.rgb = outColor3.rgb * 0.1 + videoImage * 0.9;
#endif
}
