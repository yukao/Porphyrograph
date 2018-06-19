/***********************************************************************
File: effe/shaders/LYM_Drawing_effe-FS.glsl

LYM effe & Porphyrograph (c) Yukao Nagemi & Lola Ajima

*************************************************************************/
#version 420

#include_declarations

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
// float CAdecay = 0.0;
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
// float Cursor = 1;

vec4 neighborValues[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesHV[8]=vec4[8](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 neighborValuesDiag[16]=vec4[16](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),
                                    vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

// Particle variables
float acceleration_factor;

int height;
int width;

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
// uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy;
uniform vec4 uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
uniform vec4 uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time;
// uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor;
uniform vec4 uniform_Drawing_fs_4fv_pulse;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec4 uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12;  
uniform vec3 uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold;
#ifdef PG_WITH_CA
// uniform vec4 uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs;
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
// layout (binding = 2) uniform samplerRect fs_lookupTable2;  // 2-cycle ping-pong Drawing pass CA step n (FBO attachment 3)
#endif
#if PG_NB_TRACKS >= 2
layout (binding = 3) uniform samplerRect fs_lookupTable3;  // 2-cycle ping-pong Drawing pass track 1 step n (FBO attachment 4)
#endif
// layout (binding = 5) uniform sampler3D   fs_lookupTable5;  // pen patterns
layout (binding = 6) uniform sampler3D   fs_lookupTable6;  // particle acceleration texture
#ifdef PG_VIDEO_ACTIVE
layout (binding = 7) uniform samplerRect fs_lookupTable7;  // video texture
// layout (binding = 8) uniform samplerRect fs_lookupTable8;  // current background video texture
// layout (binding = 9) uniform samplerRect fs_lookupTable9;  // initial background video texture
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
/*
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
*/

void CA_out( vec2 decalCoords , vec4 currentCA ) {
    out4_CA = currentCA; // ready
  // }

}
#endif


void particle_out( vec2 decalCoords ) {
  // fills by cumulating the neighboring particles that
  // can reach the pixel at this frame (come into a circle of radius radiusParticle)
  // with the pixel seen as length unit.
  out_color_particle = vec3(0,0,0);
}

// Main shader.

void main() {
#include_initializations

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
  // CAdecay = uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.x;
  
  // cellular automata
  CA_on_off = (CASubType > 0);
#endif

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);
  // drawingStroke = DRAWING_LINE;

  // cursor type (+1 for stylus and -1 for rubber)
  // Cursor = uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor.w;

  // particles
  acceleration_factor = uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time.x;
  partAccCenter = vec2(partAccCenter_0,partAccCenter_1);

  // working variables for screen dimension
  width = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.x);
  height = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.y);


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
  out_CA_FBO = vec4(0);
    // = texture( fs_lookupTable2 , decalCoords );
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
  // vec2 flashCACoefs = uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.zw;
  // vec4 trackCACopy = vec4(0.0);
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
  // vec2 coordsMask = decalCoordsPOT * vec2(width,height);

#ifdef PG_VIDEO_ACTIVE
  float videoGray = 0.0;
  vec3 videoImage = vec3( 0.0 );
  vec2 videoCoord = vec2( 0.0 );
  //float mask12 = texture( fs_lookupTable17 , mod( coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[2].zw*100.0 , vec2(width/2,height)) ).r;
  float flashVideoCoef = uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold.y;
  float flashVideoThreshold = uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold.z;

  // VIDEO FRAME CAPTURE AND RENDERING
  if( is_videoOn || flashVideoCoef > 0 )  
  {
    // video texture used for drawing
    float widthVideo = uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.z / height * width;
    videoCoord = vec2(0.5-decalCoordsPOT.x, 1.0 - decalCoordsPOT.y )
                 * vec2( widthVideo,
                         uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.z );
    videoImage = texture(fs_lookupTable7, videoCoord ).rgb;
    // gamma correction
    // videoImage = vec3( pow(videoImage.r,videoGamma) );

    // if( BGSubtr == 0 ) {
    //   videoImage = vec3(1) - videoImage; // no subtraction
    // }
    // else if( BGSubtr == 1 ) {
    //   videoImage = abs(videoImage - texture(fs_lookupTable9, videoCoord ).rgb); // initial background subtraction
    // }
    // else if( BGSubtr == 2 ) {
    //   videoImage = abs(videoImage - texture(fs_lookupTable8, videoCoord ).rgb); // current background subtraction
    // }
    // videoGray = graylevel(videoImage);
    // if( videoGray < uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold.w ) {
    //   videoImage = vec3(0.0);
    // }
    // else {
    //   videoImage = vec3(1.0);
    // }
  }
#endif

vec2 coordsMask = decalCoordsPOT * vec2(width,height);

#if PG_NB_TRACKS >= 2
  ////////////////////////////////////////////////////////////////////////////////////
  // MOIRE / MASKS ADDITION TO LAYER #1
  ////////////////////////////////////////////////////////////////////////////////////
  // image imageBufferColor

  float mask1 = texture( fs_lookupTable16 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[0].xy ) .r;
  float mask2 = texture( fs_lookupTable16 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[0].zw ) .g;
  float mask3 = texture( fs_lookupTable16 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[1].xy ) .b;
  float mask4 = texture( fs_lookupTable17 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[1].zw ) .r;
  float mask5 = texture( fs_lookupTable17 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[2].xy ) .g;
  float mask6 = texture( fs_lookupTable17 , coordsMask + uniform_Drawing_fs_4fv_mask_noisesxy[2].zw ) .b;
  // float mask1 = texture( fs_lookupTable16 , coordsMask ) .r;
  // float mask2 = texture( fs_lookupTable16 , coordsMask ) .g;
  // float mask3 = texture( fs_lookupTable16 , coordsMask ) .b;
  // float mask4 = texture( fs_lookupTable17 , coordsMask ) .r;
  // float mask5 = texture( fs_lookupTable17 , coordsMask ) .g;
  // float mask6 = texture( fs_lookupTable17 , coordsMask ) .b;
  float mask7 = 0; //texture( fs_lookupTable17 , coordsMask ) .b;
  

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
  // mask7 = clamp( correctionFactorMask * (mask7 - 0.5) + 0.5 , 0 , 1 );

  if(mask_threshold < 1) {
	  mask1 = (mask1 > mask_threshold + uniform_Drawing_fs_4fv_weights03[0] ? 1.0 : 0.0 );
	  mask2 = (mask2 > mask_threshold + uniform_Drawing_fs_4fv_weights03[1] ? 1.0 : 0.0 );
	  mask3 = (mask3 > mask_threshold + uniform_Drawing_fs_4fv_weights03[2] ? 1.0 : 0.0 );
	  mask4 = (mask4 > mask_threshold + uniform_Drawing_fs_4fv_weights03[3] ? 1.0 : 0.0 );
	  mask5 = (mask5 > mask_threshold + uniform_Drawing_fs_2fv_weights45[0] ? 1.0 : 0.0 );
	  mask6 = (mask6 > mask_threshold + uniform_Drawing_fs_2fv_weights45[1] ? 1.0 : 0.0 );
	  // mask7 = (mask7 > mask_threshold ? 1.0 : 0.0 );
  }

  float maskCumul = mask1;
  mask2 = (mask2 <= maskCumul ? 0. : mask2);
  maskCumul += mask2;
  mask3 = (mask3 <= maskCumul ? 0. : mask3);
  maskCumul += mask3;
  mask4 = (mask4 <= maskCumul ? 0. : mask4);
  maskCumul += mask4;
  mask5 = (mask5 <= maskCumul ? 0. : mask5);
  maskCumul += mask5;
  
  /* WITHOUT VIDEO
  mask6 = (maskCumul > 0  ? 0. : 1);
  mask7 = 0;
*/

  /* WITH VIDEO */
  mask6 = (mask6 <= maskCumul ? 0. : mask6);
  maskCumul += mask6;
  // mask7 = (mask7 < maskCumul ? 0. : mask7);
  mask7 = (maskCumul > 0  ? 0. : 1);


  // vec4 weights03 = uniform_Drawing_fs_4fv_weights03;
  vec4 weights03 = uniform_Drawing_fs_4fv_weights03 * 0.01 + vec4(1);
  // vec2 weights45 = uniform_Drawing_fs_2fv_weights45;
  vec2 weights45 = uniform_Drawing_fs_2fv_weights45 * 0.01 + vec2(1);

  vec3 imageBufferColor = vec3(0);
  vec2 coordsImage = decalCoordsPOT * vec2(width,height);
  vec2 coord = mod( ivec2(coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[0].xy) , vec2(width,height));
  // vec2 coord = coordsImage;
  imageBufferColor += (texture( fs_lookupTable10 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.x)
                     + texture( fs_lookupTable20 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.x) * (mask1) * weights03[0];
  coord = mod( coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[0].zw , vec2(width,height));
  // vec2 coord = coordsImage;

  imageBufferColor += (texture( fs_lookupTable11 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.y)
                     + texture( fs_lookupTable21 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.y) * (mask2) * (weights03[1]); 
  coord = mod( coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[1].xy , vec2(width,height));
  // vec2 coord = coordsImage;
  imageBufferColor += (texture( fs_lookupTable12 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap02.z)
                     + texture( fs_lookupTable22 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap02.z) * (mask3) * (weights03[2]); 
  coord = mod( coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[1].zw , vec2(width,height));
  // vec2 coord = coordsImage;
  imageBufferColor += (texture( fs_lookupTable13 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.x)
                     + texture( fs_lookupTable23 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.x) * (mask4) * (weights03[3]); 
  coord = mod( coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[2].xy , vec2(width,height));
  // vec2 coord = coordsImage;
  imageBufferColor += (texture( fs_lookupTable14 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.y)
                     + texture( fs_lookupTable24 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.y) * (mask5) * (weights45[0]); 
  coord = mod( coordsImage + uniform_Drawing_fs_4fv_image_noisesxy[2].zw , vec2(width,height));
  // vec2 coord = coordsImage;
  imageBufferColor += (texture( fs_lookupTable15 , coord ).rgb
                       * (1.0 - uniform_Drawing_fs_3fv_alphaSwap35.z)
                     + texture( fs_lookupTable25 , coord ).rgb
                       * uniform_Drawing_fs_3fv_alphaSwap35.z) * (mask6) * (weights45[1]); 
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
      // out_track1_FBO = 0.1 * out_track1_FBO + 0.9 * vec4( clamp( videoImage , 0.0 , 1.0 ) ,  1.0 ) * mask7 ;
      out_track1_FBO += vec4( clamp( videoImage , 0.0 , 1.0 ) * mask7 ,  0.0 );
  }
#endif

  trackColorCopy = flashTrack0Coefs[0] * out_track1_FBO.rgb;
#ifdef PG_WITH_CA
  // flash CA on track 1
  // if( graylevel(out_track1_FBO.rgb) > .2 && graylevel(out_track1_FBO.rgb) < 2.1
  //   && flashVideoCoef == 0 ) { // no flash video
  //   trackCACopy = flashCACoefs[1] * vec4(imageBufferColor.rgb,graylevel(out_track1_FBO.rgb));
  // }
#endif
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
  // if( uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs.y > 0 ) {
  //     out_CA_FBO = vec4(0);
  // }
#endif

#ifdef PG_VIDEO_ACTIVE
  if( flashVideoCoef > 0 ) { // flash video
    if(flashVideoCoef >= flashVideoThreshold) {
      // video image copy when there is a flash video
      out_track1_FBO.rgb = videoImage;
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

  // outColor0 = texture( fs_lookupTable10 , coordsImage ) /* vec4(1,0,0,1) */ * 0.99 + outColor0 * 0.01;

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
