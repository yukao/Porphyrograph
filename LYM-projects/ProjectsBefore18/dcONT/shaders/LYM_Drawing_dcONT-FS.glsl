/***********************************************************************
File: dcONT/shaders/LYM_Drawing_dcONT-FS.cg

LYM dcONT & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 150

float normalized_dist_to_mousePos;
float dist_to_mousePos = 0;
float unsigned_rx;
float unsigned_ry;
float signed_rx;
float signed_ry;
vec3 out_drawn_filled;

float out_gray_drawing;

// out_particle returned values
vec3 out_color_particle;
vec2 out_position_particle = vec2(0,0);
vec2 out_speed_particle = vec2(0,0);

float out_gray_gol;
/*
vec3 out_color_gol;
*/

float dcONT_on_off;
vec4 surrLive01[4]=vec4[4](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));
vec4 surrLive23[4]=vec4[4](vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0),vec4(0,0,0,0));

int radius_dcONT;
int dcONTFreq;
int nbExtNeighbors_dcONT;
float neighbors_dcONT_min;
float neighbors_dcONT_max;
float parents_dcONT_min;
float parents_dcONT_max;

float acceleration_factor;

float brushRadius;
float brushID;
float length_brushRadius;
float height;
float width;
float local_transparency;

int particle_texture;
vec3     particleTextureCoordinates; // the POT coordinates of the
// particle texture + z offset according to the chosen texture

#define closeNeighborPos0 vec4(1, -1, 0, 0)
#define closeNeighborPos1 vec4(0, 0, 1, -1)
#define closeNeighborPos2 vec4(1, -1, 1, -1)
#define closeNeighborPos3 vec4(1, -1, -1, 1)
#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

// frame number
int frameNo;

// dcONT ON/OFF
#define dcONT_NONE 0
#define dcONT_ON 1

// PARTICLE ON/OFF
#define PARTICLE_NONE   0
#define PARTICLE_UNIQUE 1
#define PARTICLE_FUR    2

// drawing modes
#define DRAWING_VOID   0
#define DRAWING_LINE   1
#define DRAWING_BEZIER 2
#define DRAWING_POINT  3

#define DRAWING_MARGINE  4

// obtained from Vertex Program
in vec2 decalCoords;     // texture coordinates
in vec2 decalCoordsPOT;  // normalized texture coordinates

// passed by the C program
uniform vec4 glsl_fs_4fv_drawContmode_imgCpy1_imgCpy2_nbExtNeigh;
uniform vec4 glsl_fs_4fv_minmax_dcONTparents_dcONTneighbors;
uniform vec4 glsl_fs_4fv_W_H_drawingStart_drawingMode;
uniform vec4 glsl_fs_4fv_xy_frameno_freqDots;
uniform vec4 glsl_fs_4fv_mouse_xy_prev;         // previous 2 frame
// mouse position
uniform vec4 glsl_fs_4fv_GOLdecay_drawingdecay_flash_backFlash;  
uniform vec4 glsl_fs_4fv_local_color;
uniform vec4 glsl_fs_4fv_partMode_acc_flashContmode_partTexture;
uniform vec4 glsl_fs_4fv_brushRadius_dcONTFreq_brushID_dcONTradius;
uniform samplerRect  fs_decal;                     // ping-pong speed/position of particles (FBO)
// pos FBO
uniform samplerRect  fs_lookupTable1;  // ping-pong localColor/grayGOL (FBO)
// FBO
uniform sampler2D    fs_lookupTable2;  // pen patterns
uniform samplerRect  fs_lookupTable3;  // external image #1
uniform samplerRect  fs_lookupTable4;  // external image #2
uniform samplerRect  fs_lookupTable5;  // 1920x1080 binary noise texture
uniform sampler3D    fs_lookupTable6;  // continuous noise

/////////////////////////////////////
// PARTICLE OUTPUT
out vec4 outColor0;      // 

/////////////////////////////////////
// COLOR OUTPUT
out vec4 outColor1;
// the local color copied from the ping pong color FBO
// the out_localColor_grayGOL_FBO is copied from the preceding buffer
//  value unless we are in the dcONT mode where it is averaged
// between neighbors
// the ALPHA canal contains the dcONT gray level 

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


void gol_out( vec2 decalCoords , float currentGray ) {
  // the ALPHA canal of fs_lookupTable1 contains > 0 if it is a live cell

  // fills according to the Game Of Life rules
  int half_radius = radius_dcONT/2;

  vec4 extendNeighborPos0 = vec4(0,0,0,0);
  vec4 extendNeighborPos1;
  vec4 extendNeighborPos2;
  vec4 extendNeighborPos3;
  vec4 usedNeighborPos0;
  vec4 usedNeighborPos1;
  vec4 usedNeighborPos2;
  vec4 usedNeighborPos3;

  if( nbExtNeighbors_dcONT == 0 ) {
    usedNeighborPos0 = closeNeighborPos0;
    usedNeighborPos1 = closeNeighborPos1;
    usedNeighborPos2 = closeNeighborPos2;
    usedNeighborPos3 = closeNeighborPos3;
  }
  /* 
  else if( nbExtNeighbors_dcONT == 8 ) {
    // vertical and horizontal
    usedNeighborPos0 
      = vec4(
	       texture( fs_lookupTable5, // decalCoords )
			   vec2( (int(decalCoords.x)) % 1920 ,
				   (int(decalCoords.y)) % 1080 ) )
	       * radius_dcONT) 
      - vec4(half_radius,half_radius,half_radius,half_radius) , 0;
    usedNeighborPos1.ba = usedNeighborPos0.ba;
    usedNeighborPos1.rg = vec2(0,0);
    usedNeighborPos0.ba = vec2(0,0);

    // diagonal
    usedNeighborPos2 
      = vec4(
	       texture( fs_lookupTable5, // decalCoords + vec2(110,10))
			   vec2( (int(decalCoords.x + 10)) % 1920 ,
				   (int(decalCoords.y)) % 1080 )  )
	       * radius_dcONT)
      - vec4(half_radius,half_radius,half_radius,half_radius);
    usedNeighborPos3 
      = vec4(
	       texture( fs_lookupTable5, // decalCoords - vec2(110,10) )
			   vec2( (int(decalCoords.x)) % 1920 ,
				   (int(decalCoords.y + 10)) % 1080 )  )
	       * radius_dcONT) 
      - vec4(half_radius,half_radius,half_radius,half_radius);
  }
  */
  else {
    // first assigns the extended positions to the used ones
    usedNeighborPos0 
      = vec4(
	       texture( fs_lookupTable5, // decalCoords )
			   vec2( (int(decalCoords.x) - 10) % 1920 ,
				 (int(decalCoords.y)) % 1080 )  )
	       * radius_dcONT) 
      - vec4(half_radius,half_radius,half_radius,half_radius);
    usedNeighborPos1
      = vec4(
	       texture( fs_lookupTable5, // decalCoords )
			   vec2( (int(decalCoords.x)) % 1920 ,
				 (int(decalCoords.y) - 10) % 1080 )  )
	       * radius_dcONT) 
      - vec4(half_radius,half_radius,half_radius,half_radius);

    // replaces the last ones by the close neighors
    /* if( nbExtNeighbors_dcONT < 4 ) { */
    for( int ind = nbExtNeighbors_dcONT ; ind < 4 ; ind++ ) {
      usedNeighborPos0[ind] = closeNeighborPos0[ind];
      usedNeighborPos1[ind] = closeNeighborPos1[ind];
    }
    usedNeighborPos2 = closeNeighborPos2;
    usedNeighborPos3 = closeNeighborPos3;
    /*
    }
    else {
      usedNeighborPos2 
	= vec4(
		 texture( fs_lookupTable5, // decalCoords + vec2(110,10))
			     vec2( (int(decalCoords.x + 10)) % 1920 ,
			     (int(decalCoords.y)) % 1080 )  )
		 * radius_dcONT)
	- vec4(half_radius,half_radius,half_radius,half_radius);
      usedNeighborPos3 
	= vec4(
		 texture( fs_lookupTable5, // decalCoords - vec2(110,10) )
			     vec42( (int(decalCoords.x)) % 1920 ,
			     (int(decalCoords.y + 10)) % 1080 )  )
		 * radius_dcONT) 
	- vec4(half_radius,half_radius,half_radius,half_radius);
      
      for( int ind = nbExtNeighbors_dcONT ; ind < 8 ; ind++ ) {
	usedNeighborPos2[ind] = closeNeighborPos2[ind];
	usedNeighborPos3[ind] = closeNeighborPos3[ind];
      }
    }
    */
  }

  surrLive01[0] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos0.r,
				    usedNeighborPos1.r) );
  surrLive01[1] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos0.g,
				    usedNeighborPos1.g) );
  surrLive01[2] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos0.b,
				    usedNeighborPos1.b) );
  surrLive01[3] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos0.a,
				    usedNeighborPos1.a) );
  surrLive23[0] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos2.r,
				    usedNeighborPos3.r) );
  surrLive23[1] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos2.g,
				    usedNeighborPos3.g) );
  surrLive23[2] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos2.b,
				    usedNeighborPos3.b) );
  surrLive23[3] = texture(fs_lookupTable1, 
			     decalCoords 
			     + vec2(usedNeighborPos2.a,
				    usedNeighborPos3.a) );

  if( surrLive01[0].a < 0 ) { surrLive01[0].a = -surrLive01[0].a; }
  if( surrLive01[1].a < 0 ) { surrLive01[1].a = -surrLive01[1].a; }
  if( surrLive01[2].a < 0 ) { surrLive01[2].a = -surrLive01[2].a; }
  if( surrLive01[3].a < 0 ) { surrLive01[3].a = -surrLive01[3].a; }
  if( surrLive23[0].a < 0 ) { surrLive23[0].a = -surrLive23[0].a; }
  if( surrLive23[1].a < 0 ) { surrLive23[1].a = -surrLive23[1].a; }
  if( surrLive23[2].a < 0 ) { surrLive23[2].a = -surrLive23[2].a; }
  if( surrLive23[3].a < 0 ) { surrLive23[3].a = -surrLive23[3].a; }

  float nbSurroundingLives =
    (surrLive01[0].a > 0? 1:0) +
    (surrLive01[1].a > 0? 1:0) +
    (surrLive01[2].a > 0? 1:0) +
    (surrLive01[3].a > 0? 1:0) +
    (surrLive23[0].a > 0? 1:0) +
    (surrLive23[1].a > 0? 1:0) +
    (surrLive23[2].a > 0? 1:0) +
    (surrLive23[3].a > 0? 1:0);

  float gatherSurroundingLives =
    (surrLive01[0].a) +
    (surrLive01[1].a) +
    (surrLive01[2].a) +
    (surrLive01[3].a) +
    (surrLive23[0].a) +
    (surrLive23[1].a) +
    (surrLive23[2].a) +
    (surrLive23[3].a);

  float averageSurroundingGray;
  if( nbSurroundingLives > 0 ) {
    averageSurroundingGray = gatherSurroundingLives / nbSurroundingLives;
  }
  else {
    averageSurroundingGray = 0;
  }

  /*
  out_color_gol = textureLod(fs_lookupTable1, 
			     decalCoords , 0.0 ).rgb;
  */

  // a live cell
  if( currentGray > 0 ) {
    // Any live cell with fewer than two live neighbours dies, as if
    // caused by under-population
    // sets filling channed to 0 to indicate a non filled area
    if( nbSurroundingLives < neighbors_dcONT_min ) {
      // dies
      out_gray_gol = 0;
      return;
    }
    // Any live cell with two or three live neighbours lives on to the
    // next generation
    // sets filling channed to 2 to indicate a filled area
    else if( nbSurroundingLives <= neighbors_dcONT_max ) {
      out_gray_gol = averageSurroundingGray;
      return;
    }
    // Any live cell with more than three live neighbours dies, as if
    // by overcrowding
    // sets filling channed to 0 to indicate a non filled area
    else {
      // dies
      out_gray_gol = 0;
      return;
    }
  }
  // a dead cell
  else {
    // Any dead cell with exactly three live neighbours becomes a live
    // cell, as if by reproduction
    if( nbSurroundingLives <= parents_dcONT_max 
	&& nbSurroundingLives >= parents_dcONT_min ) {
      out_gray_gol = averageSurroundingGray;

      /* 
      // computes the average surrounding color
      // obsolete since only the gray color is gathered
      out_color_gol
	= gatherSurroundingLives.rgb / nbSurroundingLives;

      // and saturates it
      float maxcol = maxCol( out_color_gol );
      if( maxcol > 0 ) {
	out_color_gol /= maxcol;
      }
      else {
	out_color_gol = vec3(1, 1, 1);
      }
      */
      return;
    }
    // otherwise remains unchanged
    else {
      out_gray_gol = currentGray;
      return;
    }
  }
}

void particle_out( vec2 decalCoords , vec4 currentcolor_gray ) {
  
  /* #define closeNeighborPos0 vec4(1, -1, 0, 0) */
  /* #define closeNeighborPos1 vec4(0, 0, 1, -1) */
  /* #define closeNeighborPos2 vec4(1, -1, 1, -1) */
  /* #define closeNeighborPos3 vec4(1, -1, -1, 1) */

  // fills by cumulating the neighboring particles that
  // can reach the pixel at this frame (come into a circle of radius 0.5)
  // with the pixel seen as length unit.
  out_color_particle = vec3(0,0,0);
  int nb_cumultated_particles = 0;

  vec2 usedNeighborPos;

  for( int ind = 0 ; ind < 4 ; ind++ ) {
    usedNeighborPos = vec2(closeNeighborPos0[ind],
			     closeNeighborPos1[ind]);

    // for optimization, if both gol and particles are active,
    // this texture lookup is not useful because it has been
    // made by gol_out
    vec4 surrParticle_localColor_graydcONT;
    if( dcONT_on_off == dcONT_ON )  {
      surrParticle_localColor_graydcONT = surrLive01[ind];
    }
    else {
      surrParticle_localColor_graydcONT
	= texture( fs_lookupTable1,
		   decalCoords + usedNeighborPos );
    }
    // if the particle is graphically active, its local 
    // displacement is computed by using its speed, and
    // the acceleration found in the continuous texture
    // and the position corresponding to the loclation of the
    // particle in the coordinates system of the current pixel
    // expected to be in [-0.5,+0.5]x[-0.5,+0.5]
    if( graylevel(surrParticle_localColor_graydcONT) >  0 ) {
      vec2 surrParticle_speed;
      vec2 surrParticle_position;
      vec2 surrParticle_nextPosition;
      
      vec4 surrParticle_speed_position
	= texture( fs_decal,
		   decalCoords + usedNeighborPos );
      surrParticle_speed = surrParticle_speed_position.xy;
      surrParticle_position = surrParticle_speed_position.zw;
      vec4 double_accelerations
	= texture( fs_lookupTable6, // decalCoords  / (w,h)
		   particleTextureCoordinates
		   + vec3( usedNeighborPos / vec2(width,height) , 0.0 ) );
      vec2 acceleration;
      /* if( particle_texture <= 1 ) { */
	acceleration = double_accelerations.rg - vec2( 0.5, 0.5 );
      /* } */
      /* else { */
      /* 	acceleration = double_accelerations.ba - vec2( 0.5, 0.5 ); */
      /* } */
      surrParticle_speed += acceleration * acceleration_factor;
      surrParticle_nextPosition 
	= usedNeighborPos + surrParticle_position + surrParticle_speed; 
      // the current step is added to the position
      
      if( abs(surrParticle_nextPosition.x) <= 0.5 
	  && abs(surrParticle_nextPosition.y) <= 0.5 ) {
	out_color_particle += surrParticle_localColor_graydcONT.rgb;
	out_speed_particle += surrParticle_speed;
	// computes the position of the particle
	out_position_particle += surrParticle_nextPosition;
	nb_cumultated_particles++;
      }
    }
  }

  for( int ind = 0 ; ind < 4 ; ind++ ) {
    usedNeighborPos = vec2(closeNeighborPos2[ind],
			     closeNeighborPos3[ind]);

    // for optimization, if both gol and particles are active,
    // this texture lookup is not useful because it has been
    // made by gol_out
    vec4 surrParticle_localColor_graydcONT;
    if( dcONT_on_off == dcONT_ON )  {
      surrParticle_localColor_graydcONT = surrLive23[ind];
    }
    else {
      surrParticle_localColor_graydcONT
	= texture( fs_lookupTable1,
		   decalCoords + usedNeighborPos );
    }
    
    // if the particle is graphically active, its local 
    // displacement is computed by using its speed, and
    // the acceleration found in the continuous texture
    // and the position corresponding to the location of the
    // particle in the coordinates system of the current pixel
    // expected to be in [-0.5,+0.5]x[-0.5,+0.5]
    if( graylevel(surrParticle_localColor_graydcONT) >  0 ) {
      vec2 surrParticle_speed;
      vec2 surrParticle_position;
      vec2 surrParticle_nextPosition;
      
      vec4 surrParticle_speed_position
	= texture( fs_decal,
		   decalCoords + usedNeighborPos );
      surrParticle_speed = surrParticle_speed_position.xy;
      surrParticle_position = surrParticle_speed_position.zw;
      
      vec4 double_accelerations
	= texture( fs_lookupTable6, // decalCoords  / (w,h)
		   particleTextureCoordinates
		   + vec3( usedNeighborPos / vec2(width,height ) , 0.0 ) );
      vec2 acceleration;
      if( particle_texture <= 1 ) {
	acceleration = double_accelerations.rg - vec2( 0.5, 0.5 );
      }
      else {
	acceleration = double_accelerations.ba - vec2( 0.5, 0.5 );
      }
      surrParticle_speed += acceleration * acceleration_factor;
      surrParticle_nextPosition 
	= usedNeighborPos + surrParticle_position + surrParticle_speed; 
      // the current step is added to the position
      
      if( abs(surrParticle_nextPosition.x) <= 0.5 
	  && abs(surrParticle_nextPosition.y) <= 0.5 ) {
	out_color_particle += surrParticle_localColor_graydcONT.rgb;
	out_speed_particle += surrParticle_speed;
	// computes the position of the particle
	out_position_particle += surrParticle_nextPosition;
	nb_cumultated_particles++;
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

float stroke_out( ) {
  
  // drawing mode: writes under the cursor
  // tries to avoid artefacts at the junction between two segments
  // vector from the pen center to the current position
  vec2 vector_to_mousePosition = vec2(signed_rx,signed_ry);
  vec2 normalized_vector_to_mousePosition 
    = vector_to_mousePosition/brushRadius;
  
  dist_to_mousePos = length( vector_to_mousePosition );
  normalized_dist_to_mousePos = dist_to_mousePos/brushRadius;
  
  // it is blended with preceding pen color at this position
  // gray level 
  // horizontal texture coordinate shift according to pen number
  float penOutGrayLevel 
    = texture(fs_lookupTable2,
	      vec2( (brushID+(normalized_vector_to_mousePosition.x/2.0+0.5))/8.0,
		    0.5 - normalized_vector_to_mousePosition.y/2.0 ) ).r;
  return penOutGrayLevel * local_transparency;
}

// Main shader.

void main() {
  /////////////////////////////////////
  // PARTICLE OUTPUT
  vec4 out_speed_position_FBO;      // 
  /////////////////////////////////////
  // COLOR OUTPUT
  vec4 out_localColor_grayGOL_FBO;
  /////////////////////////////////////
  /////////////////////////////////////
  // RENAMING FOR LEGIBILITY
  /////////////////////////////////////
  /////////////////////////////////////
  
  //////////////////////////
  // variables 
  
  // frame number
  frameNo = int(round(glsl_fs_4fv_xy_frameno_freqDots.z));
  
  // brush
  brushRadius = glsl_fs_4fv_brushRadius_dcONTFreq_brushID_dcONTradius.x;
  dcONTFreq = int(glsl_fs_4fv_brushRadius_dcONTFreq_brushID_dcONTradius.y);
  brushID = glsl_fs_4fv_brushRadius_dcONTFreq_brushID_dcONTradius.z;

  // transparency of the local color
  local_transparency = glsl_fs_4fv_local_color.w;
  int drawing_mode = int(glsl_fs_4fv_W_H_drawingStart_drawingMode.w);
  bool newly_drawn = false; // indicates whether this area is impacted
  // by pen drawing to generate new dcONT and/or particles
  
  // decay of drawing and dcONT layers
  float GOLdecay = glsl_fs_4fv_GOLdecay_drawingdecay_flash_backFlash.x/100.0;
  float drawingdecay = glsl_fs_4fv_GOLdecay_drawingdecay_flash_backFlash.y/100.0;
  nbExtNeighbors_dcONT = int(glsl_fs_4fv_drawContmode_imgCpy1_imgCpy2_nbExtNeigh.w);
  
  // game of life
  radius_dcONT = int(glsl_fs_4fv_brushRadius_dcONTFreq_brushID_dcONTradius.w);
  parents_dcONT_min = glsl_fs_4fv_minmax_dcONTparents_dcONTneighbors.x;
  parents_dcONT_max = glsl_fs_4fv_minmax_dcONTparents_dcONTneighbors.y;
  neighbors_dcONT_min = glsl_fs_4fv_minmax_dcONTparents_dcONTneighbors.z;
  neighbors_dcONT_max = glsl_fs_4fv_minmax_dcONTparents_dcONTneighbors.w;
  if( parents_dcONT_min > 0 && frameNo % dcONTFreq == 0 ) {
    dcONT_on_off = dcONT_ON;
  }
  else {
    dcONT_on_off = dcONT_NONE;
  }
  
  // particles
  int PARTICLE_mode;
  PARTICLE_mode = int(glsl_fs_4fv_partMode_acc_flashContmode_partTexture.x);
  acceleration_factor = glsl_fs_4fv_partMode_acc_flashContmode_partTexture.y;
  particle_texture = int(glsl_fs_4fv_partMode_acc_flashContmode_partTexture.w);
  
  // working variables for mouse position
  float mouse_x = glsl_fs_4fv_xy_frameno_freqDots.x;
  float mouse_y = glsl_fs_4fv_xy_frameno_freqDots.y;

  width = glsl_fs_4fv_W_H_drawingStart_drawingMode.x;
  height = glsl_fs_4fv_W_H_drawingStart_drawingMode.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // particle texture + z offset according to the chosen texture
  particleTextureCoordinates =
    vec3( decalCoordsPOT , particle_texture * 0.2 + 0.1 );

  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
  out_localColor_grayGOL_FBO = vec4( 0, 0, 0, 0 );
  if(frameNo <= 10) {
    outColor0 = vec4( 0, 0, 0, 0 );
    outColor1 = vec4( 0, 0, 0, 0 );
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
  float interp_mouse_x = mouse_x;
  float interp_mouse_y = mouse_y;
  vec2 interp_mouse_x2 = vec2( -10000, -10000);
  vec2 interp_mouse_y2 = vec2( -10000, -10000);

  // drawing occurs at every frame
  // gets the previous mouse position
  float x1 = glsl_fs_4fv_mouse_xy_prev.x;
  float y1 = glsl_fs_4fv_mouse_xy_prev.y;
  float x2 = glsl_fs_4fv_mouse_xy_prev.z;
  float y2 = glsl_fs_4fv_mouse_xy_prev.w;

  bool drawnBezier = false;
  int drawContmode = int(glsl_fs_4fv_drawContmode_imgCpy1_imgCpy2_nbExtNeigh.x);
  int flashContmode = int(glsl_fs_4fv_partMode_acc_flashContmode_partTexture.z);

  // line based rendering
  if( drawing_mode == DRAWING_LINE ) {
    if( x1 > 0 && y1 > 0 ) {
      // angle of line wrt Ox is between -45deg and +45deg: drawing
      // is controlled by x and interpolated y is a function of x
      if( abs(mouse_x - x1) > abs(mouse_y - y1) ) {
	if( ( x1 < mouse_x && decalCoords.x > x1 
	      && decalCoords.x <= mouse_x ) 
	    || ( x1 > mouse_x && decalCoords.x >= mouse_x 
		 && decalCoords.x < x1 ) ) {
	  interp_mouse_x = decalCoords.x;
	  interp_mouse_y 
	    = Line( x1 , y1 , mouse_x , mouse_y , 
		    interp_mouse_x );
	} 
      } 
      // angle of line wrt Ox is under -45deg or above +45deg: drawing
      // is controlled by y and interpolated x is a function of y
      else {
	if( ( y1 < mouse_y && (height - decalCoords.y) > y1 
	      && (height - decalCoords.y) <= mouse_y ) 
	    || ( y1 > mouse_y  && (height - decalCoords.y) >= mouse_y 
		 && (height - decalCoords.y) < y1 ) ) {
	  interp_mouse_y = (height - decalCoords.y);
	  interp_mouse_x
	    = Line( y1 , x1 , mouse_y , mouse_x , 
		    interp_mouse_y );
	} 
      }
    }
  }

  // Bezier drawing
  else if( drawing_mode == DRAWING_BEZIER ) {
    // it is the third frame: a drawed frame
    int nbFramesBezier 
      = (frameNo 
	 - int(round(glsl_fs_4fv_W_H_drawingStart_drawingMode.z)));
    if( nbFramesBezier > 1 && nbFramesBezier % 2 == 0 ) {
      drawnBezier = true;
      // checks whether we are inside the curve bounding box +/- radius
      float minx = min( x1 , min(x2, mouse_x)) - brushRadius;
      float miny = min( y1 , min(y2, mouse_y)) - brushRadius;
      float maxx = max( x1 , max(x2, mouse_x)) + brushRadius;
      float maxy = max( y1 , max(y2, mouse_y)) + brushRadius;
      if( decalCoords.x >= minx && decalCoords.x <= maxx
	  && (height - decalCoords.y) >= miny 
	  && (height - decalCoords.y) <= maxy ) {
	if( maxx - minx > maxy - miny ) {
	  interp_mouse_x = decalCoords.x;
	  interp_mouse_y2
	    = Bezier( x1 , y1 , 
		      x2 , y2 , 
		      mouse_x , mouse_y , interp_mouse_x );
	  // if 2 solutions, chooses the closest
	  if( interp_mouse_y2[0] >= 0 || interp_mouse_y2[1] >= 0 ) {
	    if( abs( interp_mouse_y2[0] - (height - decalCoords.y ))
		< abs( interp_mouse_y2[1] - (height - decalCoords.y )) ) {
	      interp_mouse_y = interp_mouse_y2[0];
	    }
	    else {
	      interp_mouse_y = interp_mouse_y2[1];
	    }
	  }
	  else {
	    interp_mouse_x = -10000;
	    interp_mouse_y = -10000;
	  }	    
	}
	else {
	  interp_mouse_y = (height - decalCoords.y);
	  interp_mouse_x2
	    = Bezier( y1 , x1 , 
		      y2 , x2 , 
		      mouse_y , mouse_x , interp_mouse_y );
	  // if 2 solutions, chooses the closest
	  if( interp_mouse_x2[0] >= 0 || interp_mouse_x2[1] >= 0 ) {
	    if( abs( interp_mouse_x2[0] - decalCoords.x )
		< abs( interp_mouse_x2[1] - decalCoords.x ) ) {
	      interp_mouse_x = interp_mouse_x2[0];
	    }
	    else {
	      interp_mouse_x = interp_mouse_x2[1];
	    }
	  }
	  else {
	    interp_mouse_x = -10000;
	    interp_mouse_y = -10000;
	  }
	}
      }
    }
    else {
      float minx = min( x1 , min(x2, mouse_x)) - brushRadius;
      float miny = min( y1 , min(y2, mouse_y)) - brushRadius;
      float maxx = max( x1 , max(x2, mouse_x)) + brushRadius;
      float maxy = max( y1 , max(y2, mouse_y)) + brushRadius;
    }
  }

  
  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: COLOR OUTPUT AND GRAY LEVEL CALCULATION
  //////////////////////////////////////
  //////////////////////////////////////

  //////////////////////////////////////
  // COPY OF PRECEDING GRAY LEVEL AND COLOR
  // starts by copying previous output
  out_speed_position_FBO
    = texture( fs_decal , decalCoords );
  out_localColor_grayGOL_FBO
    = texture( fs_lookupTable1 , decalCoords );
  // first frame dcONT is negative so that it is not displayed in the
  // composition layer
  if( out_localColor_grayGOL_FBO.a < 0 ) {
    out_localColor_grayGOL_FBO.a = -out_localColor_grayGOL_FBO.a;
  }

  // PREVIOUS GRAY LEVEL
  float previous_out_drawing_gray = graylevel(out_localColor_grayGOL_FBO);

  // out_gray_drawing initialization
  out_gray_drawing = previous_out_drawing_gray;

  /////////////////////////////////////////////////
  // void drawing, copies the previous pixel, just manages decay
  // at the end of this code
  // nothing needs to be done here
  // if( drawing_mode == DRAWING_VOID ) {
  // }

  /////////////////////////////////////////////////
  // DRAWING MODE 
  // point, line or Bezier stroke
  if( drawing_mode == DRAWING_POINT 
      || drawing_mode == DRAWING_LINE 
      || drawing_mode == DRAWING_BEZIER ) {

    //////////////////////////////////////
    // GRAY LEVEL OUTPUT
    //////////////////////////////////////

    // in case of point drawing only points every freqDots are drawn
    // otherwise the pen is considered out of the screen
    if( drawing_mode == DRAWING_POINT 
	&& frameNo % int(glsl_fs_4fv_xy_frameno_freqDots.w) > 0 ) {
      interp_mouse_x = -10000;
      interp_mouse_y = -10000;
    }

    // copies the previous drawing gray level and color
    out_gray_drawing = previous_out_drawing_gray;

    /////////////////////////////////////////////////
    // WRITING POSITION
    // distance from pen center
    signed_rx = decalCoords.x - interp_mouse_x;
    signed_ry = height - decalCoords.y - interp_mouse_y;
    unsigned_rx = abs( signed_rx );
    unsigned_ry = abs( signed_ry );
    
    // default: out color preserved
    normalized_dist_to_mousePos 
      = 10000;//length( vec2(unsigned_rx/brushRadius, unsigned_ry/brushRadius) );

    if( (x1 != mouse_x || y1 != mouse_y) // doesnt redraw on 
	// previously drawn place
	&& unsigned_rx < brushRadius && unsigned_ry < brushRadius ) {
      if( (drawing_mode == DRAWING_BEZIER && drawnBezier)
	  || drawing_mode != DRAWING_BEZIER ) {
	// out_localColor_grayGOL_FBO.a = 1;
	out_gray_drawing = stroke_out();
	float imgCpy1 = glsl_fs_4fv_drawContmode_imgCpy1_imgCpy2_nbExtNeigh.y;
	float imgCpy2 = glsl_fs_4fv_drawContmode_imgCpy1_imgCpy2_nbExtNeigh.z;
	if( imgCpy1 > 0.001 || imgCpy2 > 0.001 ) {
	  out_gray_drawing 
	    = out_gray_drawing 
	    * mix( 
		   mix( 1 ,
			 texture( fs_lookupTable3, 
				  vec2(decalCoords.x , decalCoords.y ) ).r ,
			 imgCpy1 ),
		   texture( fs_lookupTable4, 
			    vec2(decalCoords.x , decalCoords.y ) ).r ,
		   imgCpy2 );
	}
      }
    }

    /////////////////////////////////////
    // COLOR OUTPUT
    /////////////////////////////////////
  
    // color output is the pen color at this position
    // copies the previous color output
    
    // local color displacement according to gray level displacement
    // the previous color from the FBO for color storage is
    // already stored in out_localColor_grayGOL_FBO

    // blending occurs if the distance of the current pixel
    // to the mouse_position is close enough
    // if interpolated drawing (curve or line), only draws if
    // inside a drawing interval, to avoid drawing artefacts
    // at the extremities (draws a cylinder)
    if( normalized_dist_to_mousePos <= 1.0 ) {
      out_localColor_grayGOL_FBO.rgb 
	= mix( out_localColor_grayGOL_FBO.rgb,
	       glsl_fs_4fv_local_color.rgb,
	       out_gray_drawing );
      newly_drawn = true;
    }


    /* // the new pen gray level is brighter than the current pen gray level */
    /* // the new position is memorized with the new gray level */
    /* if(out_gray_drawing > previous_out_drawing_gray) { */
    /*   if( previous_out_drawing_gray > 0 ) { */
    /* 	out_localColor_grayGOL_FBO.rgb */
    /* 	  *= out_gray_drawing/previous_out_drawing_gray; */
    /*   } */
    /*   else { */
    /* 	out_localColor_grayGOL_FBO.rgb *= out_gray_drawing; */
    /*   } */
    /*   newly_drawn = true; */
    /*   // we dont blend the local color, blending is made at drawing time */
    /* } */
    /* // the new pen gray level is darker than the current pen gray level */
    /* // the new position is memorized with the old gray level */
    /* else { */
    /*   // there is nothing to do here */
    /*   /\* out_localColor_grayGOL_FBO.a = previous_out_drawing_gray;  *\/ */
    /*   // we dont blend the local color, blending is made at drawing time */
    /* } */
  }


  // recomputes the gray level that is supposed to be 
  // output
  // out_gray_drawing = graylevel(out_localColor_grayGOL_FBO);

  //////////////////////////////////////
  //////////////////////////////////////
  // GAME OF LIFE LAYER: GAME OF LIFE "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( dcONT_on_off == dcONT_ON ) {
    gol_out( decalCoords , out_localColor_grayGOL_FBO.a );

    ///////////////////////////////////////////
    // GRAY VALUE (LIFE)
    // in case of new drawing (if the pen has modified
    // the drawing layer), dcONT cells are produced
    // seen as a kind of contamination
    // The first dcONT value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // are destroyed because they are in an overcrowded zone
    if( newly_drawn ) {
      float out_pen_color = out_gray_drawing;//graylevel(out_localColor_grayGOL_FBO);
      if( drawContmode == 0 ) {
		if( out_gray_gol < out_pen_color ) {
		  out_gray_gol = -out_pen_color;
		}
      }
      else if( drawContmode == 1 ) {
		if( dist_to_mousePos >= 0.5 * brushRadius - 1
			&& dist_to_mousePos <= 0.5 * brushRadius) {
		  out_gray_gol = -out_pen_color;
		}
      }
      else if( drawContmode == 2 ) {
		out_gray_gol = -min(out_pen_color , out_gray_gol);
      }
      else if( drawContmode == 3 ) {
	if( out_gray_gol > out_pen_color ) {
	  out_gray_gol = -1;
	}
      }
    }
    /* } */
    // outputs the color from dcONT (average surrounding color if birth)
    out_localColor_grayGOL_FBO.a = out_gray_gol;

    ///////////////////////////////////////////
    // COLOR VALUE
    // drawing color
    /* if( out_gray_gol > 0 ) { */
    /*   out_localColor_grayGOL_FBO.rgb = out_color_gol; */
    /*   if( newly_drawn ) { */
    /* 	out_localColor_grayGOL_FBO.rgb *= local_transparency; */
    /*   } */
    /* } */
  }
  /* else { */
  /*   out_localColor_grayGOL_FBO.a = out_gray_gol; */
  /* } */

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: PARTICLE "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( PARTICLE_mode > 0 ) {
    particle_out( decalCoords , out_localColor_grayGOL_FBO );
    // arrival of a new particle
    if( graylevel(out_color_particle) > 0 ) {
      // outputs the color from particle move at drawing layer
      out_localColor_grayGOL_FBO.rgb = out_color_particle;
      out_speed_position_FBO = vec4( out_speed_particle , out_position_particle );
    }
    // update of the current particle
    else {
      //  modifies speed according to acceleration
      vec4 double_accelerations;
      double_accelerations
	= texture( fs_lookupTable6, // decalCoords / (w,h)
		      particleTextureCoordinates );
      vec2 acceleration;
      if( particle_texture <= 1 ) {
	acceleration = double_accelerations.rg - vec2( 0.5, 0.5 );
      }
      else {
	acceleration = double_accelerations.ba - vec2( 0.5, 0.5 );
      }
      out_speed_position_FBO.xy += acceleration * acceleration_factor;
      // updates the position of the current particle
      out_speed_position_FBO.zw += out_speed_position_FBO.xy; 

      // if the particle leaves the cell, it is erased
      if( abs( out_speed_position_FBO.z ) > 0.5
	  || abs( out_speed_position_FBO.w ) > 0.5 ) {
	if( PARTICLE_mode == PARTICLE_UNIQUE ) {
	  out_localColor_grayGOL_FBO.rgb = vec3(0,0,0);
	}
      }
    }
  }

  // recomputes the gray level that is supposed to be 
  // output
  out_gray_drawing = graylevel(out_localColor_grayGOL_FBO);

  //////////////////////////////////////
  //////////////////////////////////////
  //  TRANSFER OF DRAWING LAYER TO/FROM dcONT LAYER
  //////////////////////////////////////
  //////////////////////////////////////
  // FLASH reloads drawing into dcONT
  if( glsl_fs_4fv_GOLdecay_drawingdecay_flash_backFlash.z > 0 
      && out_gray_drawing > 0 ) {
    if( flashContmode == 0 ) {
      out_localColor_grayGOL_FBO.a = out_gray_drawing;
    }
    else if( flashContmode == 1 ) {
      if( out_gray_drawing > 0 ) {
	out_localColor_grayGOL_FBO.a = 1;
      }
    }
    else if( flashContmode == 2 ) {
      out_localColor_grayGOL_FBO.a 
	= min(out_gray_drawing , out_localColor_grayGOL_FBO.a);
    }
    else if( flashContmode == 3 ) {
      out_localColor_grayGOL_FBO.a 
	= max(out_gray_drawing , out_localColor_grayGOL_FBO.a);
    }
  }
  // BACKFLASH reloads dcONT into drawing
  if( glsl_fs_4fv_GOLdecay_drawingdecay_flash_backFlash.w > 0 
      && out_localColor_grayGOL_FBO.a > 0 ) {
    if( out_gray_drawing > 0 ) {
      out_localColor_grayGOL_FBO.rgb *= out_localColor_grayGOL_FBO.a;
    }
    else {
      out_localColor_grayGOL_FBO.rgb 
	= vec3(out_localColor_grayGOL_FBO.a,
		 out_localColor_grayGOL_FBO.a,
		 out_localColor_grayGOL_FBO.a);
    }
    out_gray_drawing = graylevel(out_localColor_grayGOL_FBO);
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRWING AND dcONT LAYERS: GRAY LEVEL DECAY
  //////////////////////////////////////
  //////////////////////////////////////
  // computes the decay (if it is not close to pen center in case
  // of dot painting)
  // preserves the BLUE channel (filling boolean)

  // DRAWING LAYER
  vec3 newDrawingColor_w_decay = vec3(0,0,0);
  if( out_gray_drawing > 0 ) {
    newDrawingColor_w_decay 
      = clamp( out_localColor_grayGOL_FBO.rgb 
	       - vec3(drawingdecay,drawingdecay,drawingdecay)  , 0.0 , 1.0 );
  }
  //////////////////////////////////////////////
  // rebuilds output for the gray/drawing buffer after decay
  // has been taken into consideration
  if( graylevel(newDrawingColor_w_decay) > 0 ) {
    out_localColor_grayGOL_FBO.rgb = newDrawingColor_w_decay;
  }
  else {
    // in case color has faded out or pen has never written
    // returns black
    out_speed_position_FBO = vec4(0,0,0,0);
    out_localColor_grayGOL_FBO.rgb= vec3(0,0,0);
  }


  // dcONT LAYER
  float newGraydcONT_w_decay = 0;
  // the first frame for dcONT is negative so that it is not drawn
  // in this case its value is preserved as it is
  if( out_localColor_grayGOL_FBO.a > 0 ) {
    newGraydcONT_w_decay 
      = clamp( out_localColor_grayGOL_FBO.a - (GOLdecay) , 0.0 , 1.0 );
    //////////////////////////////////////////////
    // rebuilds output for the gray/drawing buffer after decay
    // has been taken into consideration
    if( newGraydcONT_w_decay > 0 ) {
      out_localColor_grayGOL_FBO.a = newGraydcONT_w_decay;
    }
    else {
      // in case color has faded out the cell is considered as dead
      out_localColor_grayGOL_FBO.a = 0;
    }
  }

  outColor0 = out_speed_position_FBO;
  outColor1 = out_localColor_grayGOL_FBO;
  /* outColor1 = 0.1 * outColor1 + 0.9 * texture( fs_lookupTable6, // decalCoords / (w,h) */
  /* 					       particleTextureCoordinates ); */
  /* outColor1.a = 0; */
}
