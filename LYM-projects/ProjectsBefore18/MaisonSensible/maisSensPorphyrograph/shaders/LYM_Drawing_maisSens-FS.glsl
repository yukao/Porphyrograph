/***********************************************************************
File: alKemi/shaders/LYM_Drawing_alK-FS.glsl

LYM alK & Drawing Machine (c) Yukao Nagemi & Lola Ajima

*************************************************************************/

#version 330

float dist_to_mousePos = 0;
float unsigned_rx;
float unsigned_ry;
float signed_rx;
float signed_ry;
vec3 out_drawn_filled;


// out_particle returned values
vec3 out_color_particle;
vec2 out_position_particle = vec2(0,0);
vec2 out_speed_particle = vec2(0,0);

vec4 out4_CA;
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

int CAType;
int CASubType;

// Particle variables
float radiusParticle;

// center of the translation of acceleration values
// used for shifting particle motion in a direction
vec2 partAccCenter = vec2(0.5,0.5);
float partAccCoef;

// hiding center
// used for shifting particle motion towards a center
vec2 hidingCenter = vec2(0.5,0.5);
float hidingCoef = 0.0;

float height;
float width;

float particle_texture;
vec3 particleTextureCoordinates; // the POT coordinates of the
// particle texture + z offset according to the chosen texture
// acceleration of particles that want to hide
vec2 hidingAcc;

#define graylevel(col) ((col.r+col.g+col.b)/3.0)
#define maxCol(col) (max(col.r,max(col.g,col.b)))
#define minCol(col) (min(col.r,min(col.g,col.b)))

const vec2 neighborOffsets[8] = vec2[8](vec2(1,0),vec2(-1,0),vec2(0,1),vec2(0,-1),vec2(1,1),vec2(-1,-1),vec2(1,-1),vec2(-1,1));
  // E W N S
  // NE SW SE NW
// const int generation_state_nb[21]= {
//                                  8,1,4,6,6, 8,18,18,4,3,
//                                  4,4,5,6,6, 6,6,9,25,21,21};
// frame number
int frameNo;

// track number: 0 ==  bg and 1-4 == 4 playing tracks
// int currentTrack;

// PARTICLE mode
#define PARTICLE_NONE       0
#define PARTICLE_UNIQUE     1
#define PARTICLE_FIREWORK   2

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
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_Color_rgba;
uniform vec4 uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay;
uniform vec4 uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle;
uniform vec4 uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke;
uniform vec4 uniform_Drawing_fs_4fv_CAType_frameno_void_void;
uniform vec4 uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA;  
uniform vec4 uniform_Drawing_fs_4fv_void_void_void_clr;  
uniform vec4 uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef;
uniform vec4 uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture;

/////////////////////////////////////
// INPUT
uniform samplerRect  fs_decal;         // 3-cycle ping-pong localColor step n (FBO attachment 1)
uniform samplerRect  fs_lookupTable1;  // 3-cycle ping-pong speed/position of particles step n (FBO attachment 2)
uniform samplerRect  fs_lookupTable2;  // 3-cycle ping-pong CA step n (FBO attachment 3)
uniform sampler2D    fs_lookupTable5;  // pen patterns
uniform sampler3D    fs_lookupTable6;  // particle acceleration texture
uniform usampler2DRect  fs_lookupTable7;  // data tables for the CA
uniform sampler2D    fs_lookupTable8;  // 2D float noise texture
uniform sampler2D    fs_lookupTable9;  // 2D vessels texture

/////////////////////////////////////
// BACKGROUND COLOR OUTPUT
out vec4 outColor0;
// the local color copied from the ping pong color FBO
// the out_localColor_FBO and out_CA_FBO are copied from the preceding buffer
// value unless we are in the CA mode where it is averaged
// between neighbors
// the ALPHA canal contains the CA gray level 
// the RGB channels contain the local color

/////////////////////////////////////
// PARTICLE OUTPUT
out vec4 outColor1;      // 
// the RG channels contain the particle speed
// the BA channels contain the particle position

/////////////////////////////////////
// CA & TRACK COLOR OUTPUT
out vec4 outColor2;      // 
// the RGB channels contains the track Color

float Line( float x1 , float y1 , 
	    float mouse_x , float mouse_y , float interp_mouse_x ) {
  return mouse_y + (y1 - mouse_y)/(x1 - mouse_x) * (interp_mouse_x - mouse_x);
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

  //////////////////////////////////////////////////////
  // GAME OF LIFE - TOTALISTIC OR GENERATION
  if( CAType == CA_GOL ) {
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
    
    int nbStates = 16;
    int CArank = 9;

    if( CAType == CA_GOL ) { // GOL number of states + rank (height in data texture)
      nbStates = 2;
      CArank = 0;
    }

    // The first CA value is negative so that it is not 
    // displayed, here we change alpha value to positive
    // because it is the second time it is displayed if 
    if( currentCA.a < 0 ) {
      out4_CA.a = texture( fs_lookupTable6, 
         vec3( particleTextureCoordinates.xy , 
         6.5 / 7.0 ) ).r * (nbStates+1); // nbStates states randomly
      out4_CA.rgb = averageSurrounding.rgb;
      // check: out4_CA.a = 1.0;
    }
    else {
      int state = int(clamp(currentCA.a,0,nbStates));
      int newState = 0;
      newState = int(texelFetch(fs_lookupTable7, 
                       ivec2( state * 10 + nbSurroundingLives + 1 , CArank + CASubType ) ).r);
      out4_CA.a = float(newState);
      if( newState > 0 ) {
        out4_CA.rgb = max(vec3(0.1),averageSurrounding.rgb);
      }
      else {
        out4_CA.rgb = vec3(0,0,0);
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
      	acceleration = partAccCoef * (double_accelerations.xy - partAccCenter) + hidingAcc;

      	// if( partAccCoef > 0 ) {
      	  // acceleration
      	  surrParticle_speed += acceleration;
      	// }
        /* DAMPING TO BE REMADE
      	else {
      	  // damping
      	  surrParticle_speed += partAccCoef * surrParticle_speed;
      	}
        */

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
      	acceleration = partAccCoef * (double_accelerations.rg - partAccCenter) + hidingAcc;

      	// if( partAccCoef > 0 ) {
      	  // acceleration
      	  surrParticle_speed += acceleration;
      	// }
        /* DAMPING TO BE REMADE
      	else {
      	  // damping
      	  surrParticle_speed += partAccCoef * surrParticle_speed;
      	}
        */
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

  //////////////////////////
  // variables 

  // frame number
  frameNo = int(round(uniform_Drawing_fs_4fv_CAType_frameno_void_void.y));
  
  // track number: 0 ==  bg and 1-4 == 4 playing tracks
  // currentTrack = int(uniform_Drawing_fs_4fv_CAType_frameno_void_void.w);

  // current drawing mode
  drawingStroke = int(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.w);

  // decay of drawing and CA layers
  CAdecay = uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA.x;

  // radius of capture of the particles by the next cell
  radiusParticle = uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle.w;
  
  // cellular automata
  CAType = int(uniform_Drawing_fs_4fv_CAType_frameno_void_void.x);
  CASubType = int(uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle.x);
  CA_on_off = (CASubType > 0);
  
  // particles
  int particle_mode;
  particle_mode = int(uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture.x);
  particle_texture = uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture.w;

  // uniform acceleration
  partAccCoef = uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture.y;
  partAccCenter = uniform_Drawing_fs_4fv_CASubType_partAccCenter_radiusParticle.yz;

  // acceleration towards a center
  hidingCoef = uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef.w;
  hidingCenter = uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef.yz;

  // working variables for screen dimension
  width = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.x;
  height = uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke.y;


  // 2 noise texture: a binary one and a continous one
  // the POT coordinates of the
  // particle texture + z offset according to the chosen texture
  // *************************************************************************
  /////////////////////// TO CHECK: IS THIS REALLY USED AND USEFUL??????????
  //////////// THE COEFFICIENT 50000 IS CERTAINLY TOO HIGHT
  // *************************************************************************
  vec2 vec2Noise;
  vec2Noise = texture( fs_lookupTable6, 
		   vec3( 1.0 + sin(frameNo/50000.0),
			 1.0 + cos(frameNo/50000.0),
			 3.5  / 7.0 ) ).rg; // */
  float partTextNoise = uniform_Drawing_fs_4fv_partMode_partAcc_partTextNoise_partTexture.z;
  particleTextureCoordinates =
    vec3( (decalCoordsPOT + vec2Noise * vec2(partTextNoise,partTextNoise)) * vec2(2,0.5), 
	  particle_texture / 7.0 );//+ 5.0 + sin(frameNo/10000.0) * 5) 

  /* particleTextureCoordinates *=  */
  /*   vec3(  texture( fs_lookupTable6, vec3( 1.0 + cos(frameNo/1000.0),1.0 + 1.0 + 
       sin(frameNo/1000.0),particle_texture * 0.2 + 0.1 ) ).rg , 1.0); */

  /////////////////////////////////////
  /////////////////////////////////////
  // RESETS CHANNELS AT BEGINNING
  /////////////////////////////////////
  /////////////////////////////////////

  // at first frames resets all channels
  out_speed_position_FBO = vec4( 0, 0, 0, 0 );
  out_localColor_FBO = vec4( 0, 0, 0, 0 );
  out_CA_FBO = vec4( 0, 0, 0, 0 );
  if(frameNo <= 10 || uniform_Drawing_fs_4fv_void_void_void_clr.w > 0) {
    outColor0 = vec4(0,0,0,1);
    outColor1 = vec4(0,0,0,1);
    outColor2 = vec4(0,0,0,0);
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

  out_localColor_FBO = texture( fs_decal , decalCoords );
  out_speed_position_FBO
    = texture( fs_lookupTable1 , decalCoords );
  out_CA_FBO
    = texture( fs_lookupTable2 , decalCoords );

  // PREVIOUS GRAY LEVEL
  vec3 previous_out_drawing = out_localColor_FBO.rgb;

  //////////////////////////////////////////////
  // STROKE INTERPOLATION FOR LINE
  // int newtrack = -1;
  float tracks_opacity = 0.0;
  // each track possibly covers the previous color
  // for( int ind = 0 ; ind < 4 ; ind++ ) {

   
  // background drawing
  float background_drawing_opacity = 0.0;
  int track_drawingStroke = DRAWING_VOID;
  if( uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay.w > 0.0 ) {
    track_drawingStroke = DRAWING_LINE;
  }
  else {
    track_drawingStroke = drawingStroke;
  }
  // track_drawingStroke = DRAWING_LINE;
  if( track_drawingStroke > 0 ) {
    background_drawing_opacity 
      = out_gray_drawing( uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev.x , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev.y , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev.z , 
			  uniform_Drawing_fs_4fv_mouseTracks_xy_xyprev.w ,
			  uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay.y ,
			  uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay.z ,
			  int(uniform_Drawing_fs_4fv_mouseTracks_BrushIDRadius_replay.x) ,
			  track_drawingStroke );
    tracks_opacity += background_drawing_opacity;

    // drawing occurs on background
    if( background_drawing_opacity > 0 ) {
      // newtrack = 0;
      // brush opacity is combined with color opacity
      background_drawing_opacity *= uniform_Drawing_fs_4fv_mouseTracks_Color_rgba.w;
      out_localColor_FBO.rgb = 
	out_localColor_FBO.rgb * (1 - background_drawing_opacity)
	+ background_drawing_opacity
	* uniform_Drawing_fs_4fv_mouseTracks_Color_rgba.xyz;
    }
  }
  
  //////////////////////////////////////
  //////////////////////////////////////
  // CA LAYER: CA "SPREADING"
  //////////////////////////////////////
  //////////////////////////////////////
  if( CA_on_off ) {
    out4_CA = vec4(0.0);
    vec4 currentCA = out_CA_FBO;
    // if( trackCACopy.a > 0 ) {
    //   currentCA.rgb = clamp( currentCA.rgb 
    //        + uniform_Drawing_fs_4fv_void_void_void_clr.x * trackCACopy.rgb , 
		// 	 0.0 , 1.0 );
    // }
    CA_out( decalCoords , currentCA );

    ///////////////////////////////////////////
    // GRAY VALUE (LIFE)
    // in case of new drawing (if the pen has modified
    // the background layer), CA cells are produced
    // seen as a kind of contamination
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // are destroyed because they are in an overcrowded zone
    if( tracks_opacity > 0 ) {
      if( out4_CA.a < tracks_opacity ) {
	        out4_CA.a = -1.0;
	        out4_CA.rgb = out_localColor_FBO.rgb;
      }
    }

    // CA "ADDITION"
    float repopulatingCA 
      = uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA.w;
    if( repopulatingCA > 0 ) {
      vec3 random_noise = texture( fs_lookupTable8, vec2( decalCoordsPOT ) ).rgb; // 

      // if the particle noise is equal to frame % 10000 the cell is repopulated with a particle
      if( int(frameNo*repopulatingCA) % 5457 == int(random_noise.x * 9.5
          + random_noise.y * 231.0
          + random_noise.z * 3957.0 ) ) {
          out4_CA.a = -1.0;
          out4_CA.rgb = vec3(1.0);
      }
    }

    // outputs the color from CA (average surrounding color if birth)
    out_CA_FBO = out4_CA;

    // COPY of one of the tracks on CA layer
    // The first CA value is negative so that it is not 
    // displayed (from the second step a lot of these pixels
    // if( trackCACopy.a > 0.1 ) {
    //   out_CA_FBO.a = -1.0;
    // }

  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRAWING LAYER: PARTICLE MANAGEMENT
  //////////////////////////////////////
  //////////////////////////////////////
  // PARTICLE "SPREADING"
  if( particle_mode == PARTICLE_UNIQUE || particle_mode == PARTICLE_FIREWORK ) {
    // hiding acceleration is considered as the same for one
    // particle and all the surrounding ones
    hidingAcc = vec2(0,0);
    if( hidingCoef > 0 ) {
      vec2 hidingDirection = particleTextureCoordinates.xy - hidingCenter;
      float hidingLength = length(hidingDirection);
      hidingDirection = normalize( hidingDirection );
      hidingAcc = -hidingCoef * hidingLength  * 0.01 * hidingDirection;
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
      acceleration = partAccCoef * (double_accelerations.rg - partAccCenter) + hidingAcc;;
      // if( partAccCoef > 0 ) {
          // acceleration
        out_speed_position_FBO.xy += acceleration;
      // }
      /* DAMPING SHOULD BE REIMPLEMENTED
      else {
        // damping
        out_speed_position_FBO.xy += partAccCoef * out_speed_position_FBO.xy;
      }
      */
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

  // PARTICLE "ADDITION"
  float repopulating 
    = uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA.z;
  if( repopulating > 0 ) {
    vec3 random_noise = texture( fs_lookupTable8, 
         vec2( decalCoordsPOT ) ).rgb; // 

    // if the particle noise is equal to frame % 10000 the cell is repopulated with a particle
    if( int(frameNo*repopulating) % 10000 == int(random_noise.x * 21.5
        + random_noise.y * 462.0
        + random_noise.z * 8000.0 ) ) {
        out_localColor_FBO.rgb = vec3(1.0);
    }
  }

  // IMAGE-BASED PARTICLE "ADDITION"
  float repopulatingImage
    = uniform_Drawing_fs_4fv_CAdecay_repopulatingImage_repopulating_repopulatingCA.y;
  if( repopulatingImage > 0 ) {
     vec3 random_noise = texture( fs_lookupTable8, 
         vec2( decalCoordsPOT ) ).rgb; // 
   float image_particle = texture( fs_lookupTable9, vec2( decalCoordsPOT ) ).r; // 

    // if the particle noise is equal to frame % 10000 the cell is repopulated with a particle
    if( int(frameNo*image_particle) % 100 == int(random_noise.x * .5
        + random_noise.y * 4.0
        + random_noise.z * 80.0 ) ) {
        out_localColor_FBO.rgb = vec3(image_particle);
    }
  }

  //////////////////////////////////////
  //////////////////////////////////////
  // DRWING AND CA LAYERS: GRAY LEVEL DECAY
  //////////////////////////////////////
  //////////////////////////////////////

  // color clamping
  out_localColor_FBO.rgb = clamp(out_localColor_FBO.rgb , 0 , 1);

  // BACKGROUND TRACK 0 DECAY
  // incay or decay does not make sense on a black color
  if( graylevel(out_localColor_FBO.rgb) > 0 ) {
    out_localColor_FBO.rgb = out_localColor_FBO.rgb 
      - vec3(uniform_Drawing_fs_4fv_trackdecay_hidingXYcoef.x);
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


  // CA LAYER DECAY
  vec4 newCA_w_decay = vec4(0);
  // the first frame for CA is negative so that it is not drawn
  // in this case its value is preserved as it is
  // and incay or decay does not make sense on a black color
  if( out_CA_FBO.a > 0 ) {
    newCA_w_decay 
      = clamp( out_CA_FBO - vec4(CAdecay) , 0.0 , 1.0 );
    //////////////////////////////////////////////
    // rebuilds output for the gray/drawing buffer after decay
    // has been taken into consideration
    if( newCA_w_decay.a > 0 ) {
      out_CA_FBO = newCA_w_decay;
    }
    else {
      // in case color has faded out the cell is considered as dead
      out_CA_FBO = vec4(0);
    }
  }

  // outColor0 = out_localColor_FBO * vec4(0.00001,0.00001,0.00001,1);
  // outColor0 = out_localColor_FBO + vec4(1,1,0,1);
  outColor0 = out_localColor_FBO;
  // outColor0 = outColor0 * 0.001 +
  //   texture( fs_lookupTable5, vec2(decalCoords.x/1024.0,decalCoords.y/768.0));
  // outColor0 = outColor0 * 0.001 +
  //   texture( fs_lookupTable6, vec3(decalCoords.x/1024.0,decalCoords.y/768.0,particle_texture / 7.0));
  // outColor0 = vec4(1,0,0,1);
  outColor1 = out_speed_position_FBO;
  outColor2 = out_CA_FBO;
}
