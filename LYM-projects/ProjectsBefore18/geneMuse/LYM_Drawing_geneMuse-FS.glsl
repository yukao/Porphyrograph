/***********************************************************************
File: alKemi/shaders/LYM_Drawing_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 420

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

vec4 out4_CA; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)
float CAdecay = 0.0;

// indicates whether this area is impacted
// by pen drawing to generate new CA and/or particles
bool newly_drawn = false;

/*
  vec3 out_color_gol;
*/

bool CA_on_off;
vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// CA types
#define CA_GOL                    0
#define CA_TOTALISTIC             1
#define CA_GENERATION             2
#define CA_GENERAL_BINARY_MOORE   3
#define CA_GENERAL_BINARY_NEUMANN 4
#define CA_NEUMANN_BINARY         5
#define CA_TIME_TUNNEL            6
#define CA_WORM                   7
/* #define CA_MIXED          3 */

int CAType;
int CASubType;

// Particle variables
float radius_particle = 0.5;
float acceleration_factor;

float height;
float width;

float particle_texture;
vec3 particleTextureCoordinates; // the POT coordinates of the
// particle texture + z offset according to the chosen texture

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

const vec2 neighborOffsets[8] = {{1,0},{-1,0},{0,1},{0,-1},      // E W N S
                                 {1,1},{-1,-1},{1,-1},{-1,1},};  // NE SW SE NW
// const int generation_state_nb[21]= {
//                                  8,1,4,6,6, 8,18,18,4,3,
//                                  4,4,5,6,6, 6,6,9,25,21,21};
// frame number
int frameNo;

// track number: 0 ==  bg and 1-4 == 4 playing tracks
int currentTrack;

// PARTICLE mode
#define PARTICLE_NONE   0
#define PARTICLE_UNIQUE 1
#define PARTICLE_FUR    2
#define PARTICLE_MULTIPLY    3

// drawing modes
#define DRAWING_VOID   0
#define DRAWING_LINE   1
#define DRAWING_POINT  2
#define DRAWING_BEZIER 3
int drawingStroke;

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

/////////////////////////////////////
// UNIFORMS
// passed by the C program
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_replay;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_x;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y;
uniform ivec3 uniform_Drawing_fs_3iv_mouseTracks_x_transl;
uniform ivec3 uniform_Drawing_fs_3iv_mouseTracks_y_transl;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_x_prev;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_y_prev;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_r;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_g;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_b;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_Color_a;
uniform vec3 uniform_Drawing_fs_3fv_mouseTracks_BrushID;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusX;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusY;
uniform vec4 uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec4 uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack;
uniform vec3 uniform_Drawing_fs_3fv_flashPart_flashTrack0Coefs;  
uniform vec4 uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs;  
uniform vec3 uniform_Drawing_fs_3fv_trackdecay;
uniform vec4 uniform_Drawing_fs_4fv_partMode_partAcc_clr_void;

/////////////////////////////////////
// INPUT
layout (binding = 0) uniform samplerRect  fs_decal;         // 3-cycle ping-pong localColor step n (FBO attachment 1)
layout (binding = 1) uniform samplerRect  fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles step n (FBO attachment 2)
layout (binding = 2) uniform samplerRect  fs_lookupTable2;  // 3-cycle ping-pong CA step n+2 (or n-1) (FBO attachment 3)
layout (binding = 3) uniform samplerRect  fs_lookupTable3;  // 3-cycle ping-pong CA step n (FBO attachment 3)
layout (binding = 4) uniform samplerRect  fs_lookupTable4;  // 3-cycle ping-pong track 1 step n (FBO attachment 4)
layout (binding = 5) uniform samplerRect  fs_lookupTable5;  // 3-cycle ping-pong track 2 step n (FBO attachment 5)
layout (binding = 6) uniform sampler2D    fs_lookupTable6;  // pen patterns
layout (binding = 7) uniform sampler3D    fs_lookupTable7;  // particle acceleration texture
layout (binding = 8) uniform usampler2DRect  fs_lookupTable8;  // data tables for the CA

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


float Line( float x1 , float y1 , 
	    float mouse_x , float mouse_y , float interp_mouse_x ) {
  return mouse_y + (y1 - mouse_y)/(x1 - mouse_x) * (interp_mouse_x - mouse_x);
}


void CA_out( vec2 decalCoords , vec4 currentCA ) {
  // the ALPHA canal of fs_lookupTable3 contains > 0 if it is a live cell

  // gets Moore neighbor values
  neighborValues[0] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[0] );
  neighborValues[1] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[1] );
  neighborValues[2] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[2] );
  neighborValues[3] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[3] );
  neighborValues[4] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[4] );
  neighborValues[5] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[5] );
  neighborValues[6] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[6] );
  neighborValues[7] = texture(fs_lookupTable3, 
			      decalCoords + neighborOffsets[7] );

  //////////////////////////////////////////////////////
  // GAME OF LIFE - TOTALISTIC OR GENERATION
  if( CAType == CA_GOL || CAType == CA_TOTALISTIC || CAType == CA_GENERATION ) {
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
      nbStates = texelFetch(fs_lookupTable8, 
                            ivec2( 0 , CArank + CASubType ) ).r;
    }

    // The first CA alpha value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed 
    // then alpha represents the state of the automaton
    if( currentCA.a < 0 ) {
      out4_CA.a = texture( fs_lookupTable7, 
         vec3( particleTextureCoordinates.xy , 
         6.5 / 7.0 ) ).r * (nbStates+1); // nbStates states randomly
      // out4_CA.rgb = averageSurrounding.rgb;
      // out4_CA.a = 1.0;
    }
    else {
      uint state = int(clamp(currentCA.a,0,nbStates));
      uint newState = 0;
      newState = texelFetch(fs_lookupTable8, 
                       ivec2( state * 10 + nbSurroundingLives + 1 , CArank + CASubType ) ).r;
      out4_CA.a = float(newState);
      if( newState > 0 ) {
        out4_CA.rgb = averageSurrounding.rgb;
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
    uint nbStates = texelFetch(fs_lookupTable8, 
                               ivec2( 0 , 38 + CASubType ) ).r;


    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    uint state = int(clamp(currentCA.a,0,nbStates));
    uint newState = 0;
    if( currentCA.a < 0 ) {
      newState = int(texture( fs_lookupTable7, 
         vec3( particleTextureCoordinates.xy , 
         6.5 / 7.0 ) ).r * (nbStates+1)); // nbStates states randomly
      out4_CA.a = float(nbStates);
    }
    else {
      // survival
      if( state > 0 ) {
         newState = texelFetch(fs_lookupTable8, 
                               ivec2( nbSurroundingLives + 1 , 38 + CASubType ) ).r;
         // survival
         if( newState > 0 && out4_CA.a >= 0.0 ) {
            out4_CA.a -= 1.0;
         }
      }
      // birth
      else {
        newState = texelFetch(fs_lookupTable8, 
                       ivec2( 256 + nbSurroundingLives + 1 , 38 + CASubType ) ).r;
        // birth
        if( newState > 0 ) {
          out4_CA.a = float(nbStates);
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
    uint nbStates = texelFetch(fs_lookupTable8, 
                               ivec2( 0 , 46 + CASubType ) ).r;

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    uint state = int(clamp(currentCA.a,0,nbStates));
    uint newState = 0;
    if( currentCA.a < 0 ) {
      newState = int(texture( fs_lookupTable7, 
         vec3( particleTextureCoordinates.xy , 
         6.5 / 7.0 ) ).r * (nbStates+1)); // nbStates states randomly
      out4_CA.a = float(nbStates);
    }
    else {
      // survival
      if( state > 0 ) {
         newState = texelFetch(fs_lookupTable8, 
                               ivec2( nbSurroundingLives + 1 , 46 + CASubType ) ).r;
         // survival
         if( newState > 0 && out4_CA.a >= 0.0 ) {
            out4_CA.a -= 1.0;
         }
      }
      // birth
      else {
        newState = texelFetch(fs_lookupTable8, 
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
      (int(currentCA.a)) +  // CENTER
      (int(neighborValues[0].a) * 9) +  // E
      (int(neighborValues[1].a) * 81) +  // W
      (int(neighborValues[2].a) * 3) +  // N
      (int(neighborValues[3].a) * 27) ;  // S

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    uint state = int(clamp(currentCA.a,0,3));
    uint newState = 0;
    if( currentCA.a < 0 ) {
      newState = int(texture( fs_lookupTable7, 
         vec3( particleTextureCoordinates.xy , 
         6.5 / 7.0 ) ).r * (3+1)); // nbStates states randomly
      out4_CA.a = float(newState);
    }
    else {
      newState = texelFetch(fs_lookupTable8, 
                            ivec2( nbSurroundingLives + 1 , 50 + CASubType ) ).r;
      out4_CA.a = float(newState);
    }

    if( out4_CA.a > 0 ) {
/*      vec4 gatherSurroundingLives = currentCA +
        (neighborValues[0]) * 9 +
        (neighborValues[1]) * 81 +
        (neighborValues[2]) * 3 +
        (neighborValues[3]) * 27;
      
      vec4 averageSurrounding;
      if( nbSurroundingLives > 0 ) {
*/      vec3 averageSurrounding = vec3(out4_CA.a/float(3-1)); //gatherSurroundingLives* 3 / nbSurroundingLives;
/*      }
      else {
        averageSurrounding = vec4(0.0);
      }
*/      out4_CA.rgb = averageSurrounding;
    }
    else {
      out4_CA.rgb = vec3(0,0,0);
    }
  }

  //////////////////////////////////////////////////////
  // TIME TUNNEL
  else if( CAType == CA_TIME_TUNNEL ) {
    float previousGray =  texture(fs_lookupTable2, decalCoords ).a;

    int nbSurroundingFiring =
      (currentCA.a > 0? 1:0) +
      (neighborValues[0].a > 0? 1:0) +
      (neighborValues[1].a > 0? 1:0) +
      (neighborValues[2].a > 0? 1:0) +
      (neighborValues[3].a > 0? 1:0);
    bool fired = false;
    if( CASubType == 1 ) {
      fired = (nbSurroundingFiring > 0 && nbSurroundingFiring < 5);
    }
    else if( CASubType == 2 ) {
      vec2 noise = texture( fs_lookupTable7, 
			    vec3( particleTextureCoordinates.xy , 
				  6.5 / 7.0 ) ).rg;
      fired = (nbSurroundingFiring > (0 + int(noise.x * 2.0))
	       && nbSurroundingFiring < (5 - int(noise.y * 2.0)) );
    }
    else if( CASubType == 3 ) {
      vec2 noise = texture( fs_lookupTable7, 
			    vec3( particleTextureCoordinates.xy , 
				  6.5 / 7.0 ) ).rg;
      fired = (nbSurroundingFiring > (0 + int(noise.x * 2.0))
	       && nbSurroundingFiring < (4 - int(noise.y * 2.0)) );
    }
    if( fired ) {
      if( previousGray == 0.0 ) {
        vec4 gatherSurroundingLives =
      	  currentCA +
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

  //////////////////////////////////////////////////////
  // WORM
  else if( CAType == CA_WORM ) {
    // > 0.5 firing
    // > 0 && <= 0.5 refractory
    // 0 ready
    // ready -> firing  (w 2 neighbors firing)
    // firing -> rafractory
    // refractory -> ready
    float previousGray =  texture(fs_lookupTable2, decalCoords ).a;
    vec3 noise = texture( fs_lookupTable7, 
			  vec3( particleTextureCoordinates.xy , 
				6.5 / 7.0 ) ).rgb;
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
      if( (nbSurroundingFiring == 2 + int( noise.x * 2) 
	         || nbSurroundingFiring == 3 + int( noise.y * 2) 
	         || nbSurroundingFiring == 5 + int( noise.z * 2) )
	           && currentCA.a <= 0 ) {
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
        out4_CA = currentCA - 100.0 * vec4(CAdecay);
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
	     && currentCA.a <= 0 ) {
	       out4_CA = vec4(1.0); // resets alarm
      }
      else {
        out4_CA = currentCA - 100.0 * vec4(CAdecay);
        if( out4_CA.a < 0.0 ) {
          out4_CA = vec4(0.0);
        }
      }
    }
  }

  //////////////////////////////////////////////////////
  // NO CA
  else {
    out4_CA = currentCA; // ready
  }
}

void particle_out( vec2 decalCoords ) {
  // fills by cumulating the neighboring particles that
  // can reach the pixel at this frame (come into a circle of radius radius_particle)
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
      // expected to be in [-radius_particle,+radius_particle]x[-radius_particle,+radius_particle]
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
            = texture( fs_lookupTable7, // decalCoords  / (w,h)
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

	if( abs(surrParticle_nextPosition.x) <= radius_particle 
            && abs(surrParticle_nextPosition.y) <= radius_particle ) {
	  out_color_particle += surrParticle_localColor.rgb;
          out_speed_particle += surrParticle_speed;
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
      // expected to be in [-radius_particle,+radius_particle]x[-radius_particle,+radius_particle]
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
	    = texture( fs_lookupTable7, // decalCoords  / (w,h)
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

	if( abs(surrParticle_nextPosition.x) <= radius_particle 
            && abs(surrParticle_nextPosition.y) <= radius_particle ) {
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
      = texture(fs_lookupTable6,
		vec2( (current_brushID
		       +(normalized_vector_to_mousePosition.x/2.0+0.5))/8.0,
		      0.5 - normalized_vector_to_mousePosition.y/2.0 ) ).r;
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

    // in case of point drawing only points are drawn when flashTrack0 is true
    // otherwise the pen is considered out of the screen
    if( p_track_drawingStroke == DRAWING_POINT 
        && !(currentTrack > 0 && flashTrack0Coefs[currentTrack - 1] > 0) ) {
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
  vec4 out_track1_FBO; // color of track 1
  vec4 out_track2_FBO; // color of track 2

  //////////////////////////
  // variables 

  // frame number
  frameNo = int(round(uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack.y));
  
  // track number: 0 ==  bg and 1-4 == 4 playing tracks
  currentTrack = int(uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack.w);

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);

  // decay of drawing and CA layers
  CAdecay = uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack.x;
  
  // cellular automata
  CAType = int(uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter.x);
  CASubType = int(uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter.y);
  CA_on_off = (CASubType > 0);
  
  // particles
  int particle_mode;
  particle_mode = int(uniform_Drawing_fs_4fv_partMode_partAcc_clr_void.x);
  acceleration_factor = uniform_Drawing_fs_4fv_partMode_partAcc_clr_void.y;
  particle_texture = uniform_Drawing_fs_4fv_CAdecay_frameno_partTexture_currentTrack.z;
  partAccCenter = uniform_Drawing_fs_4fv_CAType_CASubType_partAccCenter.zw;

  // working variables for screen dimension
  width = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.x;
  height = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // particle texture + z offset according to the chosen texture
  vec2 noise;
  noise = texture( fs_lookupTable7, 
		   vec3( 1.0 + sin(frameNo/50000.0),
			 1.0 + cos(frameNo/50000.0),
			 3.5  / 7.0 ) ).rg; // 
  particleTextureCoordinates =
    vec3( decalCoordsPOT + noise , 
	  particle_texture / 7.0 );//+ 5.0 + sin(frameNo/10000.0) * 5) 

  /* particleTextureCoordinates *=  */
  /*   vec3(  texture( fs_lookupTable7, vec3( 1.0 + cos(frameNo/1000.0),1.0 + 1.0 + sin(frameNo/1000.0),particle_texture * 0.2 + 0.1 ) ).rg , 1.0); */

  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
  out_localColor_FBO = vec4( 0, 0, 0, 0 );
  out_CA_FBO = vec4( 0, 0, 0, 0 );
  out_track1_FBO = vec4( 0, 0, 0, 0 );
  out_track2_FBO = vec4( 0, 0, 0, 0 );
  if(frameNo <= 10 || uniform_Drawing_fs_4fv_partMode_partAcc_clr_void.z > 0) {
    outColor0 = vec4(0,0,0,1);
    outColor1 = vec4(0,0,0,1);
    outColor2 = vec4(0,0,0,0);
    outColor3 = vec4(0,0,0,1);
    outColor4 = vec4(0,0,0,1);
    return;
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
    + vec2( uniform_Drawing_fs_3iv_mouseTracks_x_transl[0] ,
	    uniform_Drawing_fs_3iv_mouseTracks_y_transl[0] );
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
    = texture( fs_lookupTable3 , decalCoords );

  // possible horizontal or vertical translation of track1 texture
  decalCoordsPrevStep = decalCoords 
    + vec2( uniform_Drawing_fs_3iv_mouseTracks_x_transl[1] ,
	    uniform_Drawing_fs_3iv_mouseTracks_y_transl[1] );
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track1_FBO = texture( fs_lookupTable4 , decalCoordsPrevStep );
  }
  else {
    out_track1_FBO = vec4( 0, 0, 0, 0 );;
  }

  // possible horizontal or vertical translation of track2 texture
  decalCoordsPrevStep = decalCoords 
    + vec2( uniform_Drawing_fs_3iv_mouseTracks_x_transl[2] ,
	    uniform_Drawing_fs_3iv_mouseTracks_y_transl[2] );
  if( decalCoordsPrevStep.x < width && decalCoordsPrevStep.x >= 0 && 
      decalCoordsPrevStep.y < height && decalCoordsPrevStep.y >= 0 ) {
    out_track2_FBO = texture( fs_lookupTable5 , decalCoordsPrevStep );
  }
  else {
    out_track2_FBO = vec4( 0, 0, 0, 0 );;
  }

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_localColor_FBO.rgb;

  //////////////////////////////////////
  //////////////////////////////////////
  //  FLASH PARTICLE: increase radius for particle capture
  //////////////////////////////////////
  //////////////////////////////////////
  flashTrack0Coefs = uniform_Drawing_fs_3fv_flashPart_flashTrack0Coefs.yz;
  vec3 flashCACoefs = uniform_Drawing_fs_4fv_flashBackCoef_flashCACoefs.yzw;
  vec3 trackColorCopy = vec3(0.0);
  vec4 trackCACopy = vec4(0.0);  // RGB + GRAY
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
  float track_1_opacity = 0.0;
  float track_2_opacity = 0.0;
  float track_0_opacity = 0.0;
  float track_tablet1_opacty = 0.0;
  float tracks_opacity = 0.0;

  // tablet drawing
  if( currentTrack == 0 ) {
        track_0_opacity = out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 3 ] ,
          int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 0 ]) ,
          DRAWING_LINE );
  }
  else if( currentTrack == 1 ) {
        track_1_opacity = out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 3 ] ,
          int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 1 ]) ,
          DRAWING_LINE );
  }
  else {
        track_2_opacity = out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 3 ] , 
          uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 3 ] ,
          uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 3 ] ,
          int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 2 ]) ,
          DRAWING_LINE );
  }

  // each track possibly covers the previous color
  // for( int ind = 0 ; ind < 4 ; ind++ ) {

  // TRACK 1
  int track_drawingStroke = DRAWING_VOID;
  if( uniform_Drawing_fs_3fv_mouseTracks_replay[ 1 ] > 0.0 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentTrack == 1 ) {
    track_drawingStroke = drawingStroke; 
  }
  if( track_drawingStroke > 0 ) {
    track_1_opacity += out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 1 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 1 ] ,
					uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 1 ] ,
					uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 1 ] ,
					int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 1 ]) ,
					track_drawingStroke );
    tracks_opacity += track_1_opacity;

    // drawing occurs on track 1
    if( track_1_opacity > 0 ) {
      // bush opacity is combined with color opacity and current track opacity
      // if the track is totally transparent, the color opacity is not 
      // multiplied by the track transparency otherwise coloring will never occur
      track_1_opacity *= uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 1 ];
      /* if( out_track1_FBO.a > 0 ) { */
      /*   track_1_opacity *= out_track1_FBO.a; */
      /* } */
      out_track1_FBO.rgb = out_track1_FBO.rgb * (1 - track_1_opacity)
	+ track_1_opacity * vec3(1,1,1)
	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 1 ] , 
		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 1 ] , 
		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 1 ] );
      out_track1_FBO.a = 1.0;//track_1_opacity;
    } 
  }

  trackColorCopy = flashTrack0Coefs[0] * out_track1_FBO.rgb;
  trackCACopy = flashCACoefs[1] * vec4(out_track1_FBO.rgb,graylevel(out_track1_FBO.rgb));

  // TRACK 2
  track_drawingStroke = DRAWING_VOID;
  if( uniform_Drawing_fs_3fv_mouseTracks_replay[ 2 ] > 0.0 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentTrack == 2 ) {
    track_drawingStroke = drawingStroke;
  }
  if( track_drawingStroke > 0 ) {
    track_2_opacity += out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 2 ] , 
					uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 2 ] ,
					uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 2 ] ,
					uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 2 ] ,
					int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 2 ]) ,
					track_drawingStroke );
    tracks_opacity += track_2_opacity;

    // drawing occurs on track 2
    if( track_2_opacity > 0 ) {
      // bush opacity is combined with color opacity and current track opacity
      // if the track is totally transparent, the color opacity is not 
      // multiplied by the track transparency otherwise coloring will never occur
      track_2_opacity *= uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 2 ];
      /* if( out_track2_FBO.a > 0 ) { */
      /*   track_2_opacity *= out_track2_FBO.a; */
      /* } */
      out_track2_FBO.rgb = out_track2_FBO.rgb * (1 - track_2_opacity)
	+ track_2_opacity
	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 2 ] , 
		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 2 ] , 
		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 2 ] );
      out_track2_FBO.a = 1.0;//track_2_opacity;
    }  
  }

  trackColorCopy += flashTrack0Coefs[1] * out_track2_FBO.rgb;
  trackCACopy += flashCACoefs[2] * vec4(out_track2_FBO.rgb,graylevel(out_track2_FBO.rgb));
  
  // TRACK 0
  // background drawing
  track_drawingStroke = DRAWING_VOID;
  if( uniform_Drawing_fs_3fv_mouseTracks_replay[ 0 ] > 0.0 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else if( currentTrack == 0 ) {
    track_drawingStroke = drawingStroke;
  }
  // track_drawingStroke = DRAWING_LINE;
  if( track_drawingStroke > 0 ) {
    track_0_opacity 
      += out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_x[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_y[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_x_prev[ 0 ] , 
			  uniform_Drawing_fs_4fv_mouseTracks_y_prev[ 0 ] ,
			  uniform_Drawing_fs_4fv_mouseTracks_RadiusX[ 0 ] ,
			  uniform_Drawing_fs_4fv_mouseTracks_RadiusY[ 0 ] ,
			  int(uniform_Drawing_fs_3fv_mouseTracks_BrushID[ 0 ]) ,
			  track_drawingStroke );
    tracks_opacity += track_0_opacity;

    // drawing occurs on background
    if( track_0_opacity > 0 ) {
      // newtrack = 0;
      // brush opacity is combined with color opacity
      track_0_opacity *= uniform_Drawing_fs_3fv_mouseTracks_Color_a[ 0 ];
      out_localColor_FBO.rgb = 
      	out_localColor_FBO.rgb * (1 - track_0_opacity)
      	+ track_0_opacity
      	* vec3( uniform_Drawing_fs_3fv_mouseTracks_Color_r[ 0 ] , 
            		uniform_Drawing_fs_3fv_mouseTracks_Color_g[ 0 ] , 
            		uniform_Drawing_fs_3fv_mouseTracks_Color_b[ 0 ] );
    }
  }
  trackCACopy += flashCACoefs[0] * vec4(out_localColor_FBO.rgb,graylevel(out_localColor_FBO.rgb));
  
  //////////////////////////////////////
  //////////////////////////////////////
  // CA LAYER: CA "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( CA_on_off ) {
    out4_CA = vec4(0.0);
    vec4 currentCA = out_CA_FBO; // RGB: CA color ALPHA: CA state (ALPHA negative 
              // when first time writing on CA for skipping the first frame for CA update)

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // will disappear through the updating rule of the automaton)
    if( trackCACopy.a > 0 ) { // gray level of copy color (in case of flash)
      currentCA.a = -1;
      currentCA.rgb = clamp( currentCA.rgb + out_CA_FBO.rgb , 0.0 , 1.0 );
    }

     // calculates the new state of the automaton
    CA_out( decalCoords , currentCA );

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
  if( particle_mode > 0 ) {
    //////////////////////////////////////
    //////////////////////////////////////
    //  FLASH PARTICLE: increase radius for particle capture
    //////////////////////////////////////
    //////////////////////////////////////
    if( uniform_Drawing_fs_3fv_flashPart_flashTrack0Coefs.x > 0 ) {
      radius_particle = 1;
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
	     = texture( fs_lookupTable7, // decalCoords  / (w,h)
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
	if( particle_mode == PARTICLE_UNIQUE ) {
	  out_localColor_FBO.rgb = vec3(0,0,0);
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
  out_localColor_FBO.rgb = clamp(out_localColor_FBO.rgb + trackColorCopy, 0 , 1);

  // BACKGROUND TRACK 0 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_localColor_FBO.rgb) > 0 ) {
    out_localColor_FBO.rgb = out_localColor_FBO.rgb 
      - vec3(uniform_Drawing_fs_3fv_trackdecay[ 0 ],uniform_Drawing_fs_3fv_trackdecay[ 0 ],
	     uniform_Drawing_fs_3fv_trackdecay[ 0 ]);
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

  // TRACK 1 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_track1_FBO.rgb) > 0 ) {
    out_track1_FBO.rgb = out_track1_FBO.rgb - vec3(uniform_Drawing_fs_3fv_trackdecay[ 1 ],
						   uniform_Drawing_fs_3fv_trackdecay[ 1 ],uniform_Drawing_fs_3fv_trackdecay[ 1 ]);
  }
  out_track1_FBO.rgb 
    = clamp( out_track1_FBO.rgb , 0.0 , 1.0 );

  // TRACK 2 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_track2_FBO.rgb) > 0 ) {
    out_track2_FBO.rgb = out_track2_FBO.rgb - vec3(uniform_Drawing_fs_3fv_trackdecay[ 2 ],
						   uniform_Drawing_fs_3fv_trackdecay[ 2 ],uniform_Drawing_fs_3fv_trackdecay[ 2 ]);
  }
  out_track2_FBO.rgb 
    = clamp( out_track2_FBO.rgb , 0.0 , 1.0 );

  // CA LAYER DECAY
  // the first frame for CA is negative so that it is not drawn
  // in this case its value is preserved as it is
  // and incay or decay does not make sense on a black color
  if( out_CA_FBO.a > 0 ) {
    vec4 newCA_w_decay;
    newCA_w_decay.rgb
      = clamp( out_CA_FBO.rgb - vec3(CAdecay) , 0.0 , 1.0 );
    //////////////////////////////////////////////
    // rebuilds output for the gray/drawing buffer after decay
    // has been taken into consideration
    newCA_w_decay.a = graylevel(newCA_w_decay.rgb);
    if( newCA_w_decay.a > 0 ) {
      out_CA_FBO = newCA_w_decay;
    }
    else {
      // in case color has faded out the cell is considered as dead
      out_CA_FBO = vec4(0);
    }
  }

  outColor0 = out_localColor_FBO;
  // outColor0 = outColor0 * 0.001 +
  //   texture( fs_lookupTable6, vec2(decalCoords.x/1024.0,decalCoords.y/768.0));
  // outColor0 = outColor0 * 0.001 +
  //   texture( fs_lookupTable7, vec3(decalCoords.x/1024.0,decalCoords.y/768.0,particle_texture / 7.0));
  // outColor0 = vec4(1,0,0,1);
  outColor1 = out_speed_position_FBO;
  outColor2 = out_CA_FBO;
  outColor3 = out_track1_FBO;
  outColor4 = out_track2_FBO;
}