/*! \file pg-stroke.cpp
 * 
 *
 *     File pg-stroke.cpp
 *
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "pg-all_include.h"

// mouse recording tracks management
float **TrackColor_r_source = NULL;
float **TrackColor_g_source = NULL;
float **TrackColor_b_source = NULL;
float **TrackColor_a_source = NULL;
float **TrackColor_r_target = NULL;
float **TrackColor_g_target = NULL;
float **TrackColor_b_target = NULL;
float **TrackColor_a_target = NULL;
float **TrackColor_r_temp_target = NULL;
float **TrackColor_g_temp_target = NULL;
float **TrackColor_b_temp_target = NULL;
float **TrackColor_a_temp_target = NULL;

bool isDrawingOn = false;
bool isAMove = false;

int **TrackBrushID_source = NULL;
int **TrackBrushID_target = NULL;
int **TrackBrushID_temp_target = NULL;

float **TrackRadiusX_source = NULL;
float **TrackRadiusX_target = NULL;
float **TrackRadiusX_temp_target = NULL;
float **TrackRadiusY_source = NULL;
float **TrackRadiusY_target = NULL;
float **TrackRadiusY_temp_target = NULL;

float **TrackPos_x_source = NULL;
float **TrackPos_y_source = NULL;
float **TrackPos_xL_source = NULL;
float **TrackPos_yL_source = NULL;
float **TrackPos_xR_source = NULL;
float **TrackPos_yR_source = NULL;
double **TrackTime_source = NULL;
float **TrackPos_x_target = NULL;
float **TrackPos_y_target = NULL;
float **TrackPos_xL_target = NULL;
float **TrackPos_yL_target = NULL;
float **TrackPos_xR_target = NULL;
float **TrackPos_yR_target = NULL;
double **TrackTime_target = NULL;
float **TrackPos_x_temp_target = NULL;
float **TrackPos_y_temp_target = NULL;
double **TrackTime_temp_target = NULL;
struct TrackStatus_source *TrackStatus_source = NULL;
struct TrackStatus_source *TrackStatus_target = NULL;

// tension of catmul-rom spline: currently, the standard value 1/6
// could be a parameter in the future
float Tension = 1.0F/6.0F;

////////////////////////////////////////////////////////////////
// SPLINES
////////////////////////////////////////////////////////////////
void ScaleVec2(vec2* pOut_result, vec2* p0, float c0) {
  pOut_result->x = c0 * p0->x;
  pOut_result->y = c0 * p0->y;
}

void AddScaledVec2_Self(vec2* pOut_result, vec2* p0, float c0) {
  pOut_result->x += c0 * p0->x;
  pOut_result->y += c0 * p0->y;
}

//////////////////////////////////////////////////////////////////
// Catmull-Rom evaluation between two points: 
// a spline that requires 4 points the two points AB between which
// evaluation is made + 1 a before A and 1 b after B that are
// used to define the tangents at A // aB and at B // Ab
//////////////////////////////////////////////////////////////////
// 
void CatmullRom_Evaluate(vec2* pOut_result, vec2* p0, vec2* p1, vec2* p2, vec2* p3, float t)
{
  float c0 = ((-t + 2.0f) * t - 1.0f) * t * 0.5f;
  float c1 = (((3.0f * t - 5.0f) * t) * t + 2.0f) * 0.5f;
  float c2 = ((-3.0f * t + 4.0f) * t + 1.0f) * t * 0.5f;
  float c3 = ((t - 1.0f) * t * t) * 0.5f;
           
  ScaleVec2(pOut_result, p0, c0);
  AddScaledVec2_Self(pOut_result, p1, c1);
  AddScaledVec2_Self(pOut_result, p2, c2);
  AddScaledVec2_Self(pOut_result, p3, c3);
}         

//////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TABLES THAT CONTAIN THE STROKE PARAMETERS
//////////////////////////////////////////////////////////////////
void pg_initStrokes( void ) {
  // looks for specific variables (brush radius, local color, brush

  // mouse pointer tracks recording initialization
  TrackColor_r_source = new float*[ PG_NB_TRACKS ];
  TrackColor_g_source = new float*[ PG_NB_TRACKS ];
  TrackColor_b_source = new float*[ PG_NB_TRACKS ];
  TrackColor_a_source = new float*[ PG_NB_TRACKS ];
  TrackBrushID_source = new int*[ PG_NB_TRACKS ];
  TrackRadiusX_source = new float*[ PG_NB_TRACKS ];
  TrackRadiusY_source = new float*[ PG_NB_TRACKS ];
  TrackPos_x_source = new float*[ PG_NB_TRACKS ];
  TrackPos_y_source = new float*[ PG_NB_TRACKS ];
  TrackPos_xL_source = new float*[ PG_NB_TRACKS ];
  TrackPos_yL_source = new float*[ PG_NB_TRACKS ];
  TrackPos_xR_source = new float*[ PG_NB_TRACKS ];
  TrackPos_yR_source = new float*[ PG_NB_TRACKS ];
  TrackTime_source = new double*[ PG_NB_TRACKS ];
  TrackStatus_source = new struct TrackStatus_source[ PG_NB_TRACKS ];
  TrackColor_r_target = new float*[ PG_NB_TRACKS ];
  TrackColor_g_target = new float*[ PG_NB_TRACKS ];
  TrackColor_b_target = new float*[ PG_NB_TRACKS ];
  TrackColor_a_target = new float*[ PG_NB_TRACKS ];
  TrackBrushID_target = new int*[ PG_NB_TRACKS ];
  TrackRadiusX_target = new float*[ PG_NB_TRACKS ];
  TrackRadiusY_target = new float*[ PG_NB_TRACKS ];
  TrackPos_x_target = new float*[ PG_NB_TRACKS ];
  TrackPos_y_target = new float*[ PG_NB_TRACKS ];
  TrackPos_xL_target = new float*[ PG_NB_TRACKS ];
  TrackPos_yL_target = new float*[ PG_NB_TRACKS ];
  TrackPos_xR_target = new float*[ PG_NB_TRACKS ];
  TrackPos_yR_target = new float*[ PG_NB_TRACKS ];
  TrackTime_target = new double*[ PG_NB_TRACKS ];
  TrackColor_r_temp_target = new float*[ PG_NB_TRACKS ];
  TrackColor_g_temp_target = new float*[ PG_NB_TRACKS ];
  TrackColor_b_temp_target = new float*[ PG_NB_TRACKS ];
  TrackColor_a_temp_target = new float*[ PG_NB_TRACKS ];
  TrackBrushID_temp_target = new int*[ PG_NB_TRACKS ];
  TrackRadiusX_temp_target = new float*[ PG_NB_TRACKS ];
  TrackRadiusY_temp_target = new float*[ PG_NB_TRACKS ];
  TrackPos_x_temp_target = new float*[ PG_NB_TRACKS ];
  TrackPos_y_temp_target = new float*[ PG_NB_TRACKS ];
  TrackTime_temp_target = new double*[ PG_NB_TRACKS ];
  TrackStatus_target = new struct TrackStatus_source[ PG_NB_TRACKS ];
  for( int ind = 0 ; ind < PG_NB_TRACKS ; ind++ ) {
    TrackColor_r_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_g_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_b_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_a_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackBrushID_source[ ind ] 
      = new int[ max_mouse_recording_frames ];
    TrackRadiusX_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackRadiusY_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_x_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_y_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_xL_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_yL_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_xR_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_yR_source[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackTime_source[ ind ] 
      = new double[ max_mouse_recording_frames ];
    TrackStatus_source[ ind ].isFirstFrame = false;
    TrackStatus_source[ ind ].isActiveInterpolation = false;
    TrackStatus_source[ ind ].isActiveRecording = false;
    TrackStatus_source[ ind ].isNormalized = false;
    TrackStatus_source[ ind ].nbRecordedFrames = 0;
    TrackStatus_source[ ind ].indReading = 0;
    TrackStatus_source[ ind ].coefInterpolation = 0;
    TrackStatus_source[ ind ].initialTimeRecording = 0.0;
    TrackStatus_source[ ind ].initialTimeReading = 0.0;
    TrackStatus_source[ ind ].isStepwiseInterpolation = false;
    TrackStatus_source[ ind ].interpolationStep = 0.00001F;
    TrackStatus_source[ ind ].readSpeedScale = 1.0F;
    TrackColor_r_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_g_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_b_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_a_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackBrushID_target[ ind ] 
      = new int[ max_mouse_recording_frames ];
    TrackRadiusX_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackRadiusY_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_x_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_y_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_xL_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_yL_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_xR_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_yR_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackTime_target[ ind ] 
      = new double[ max_mouse_recording_frames ];
    TrackColor_r_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_g_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_b_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackColor_a_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackBrushID_temp_target[ ind ] 
      = new int[ max_mouse_recording_frames ];
    TrackRadiusX_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackRadiusY_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_x_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackPos_y_temp_target[ ind ] 
      = new float[ max_mouse_recording_frames ];
    TrackTime_temp_target[ ind ] 
      = new double[ max_mouse_recording_frames ];
    TrackStatus_target[ ind ].isFirstFrame = false;
    TrackStatus_target[ ind ].isActiveInterpolation = false;
    TrackStatus_target[ ind ].isActiveRecording = false;
    TrackStatus_target[ ind ].isNormalized = false;
    TrackStatus_target[ ind ].nbRecordedFrames = 0;
    TrackStatus_target[ ind ].indReading = 0;
    TrackStatus_target[ ind ].initialTimeRecording = 0.0;
    TrackStatus_target[ ind ].initialTimeReading = 0.0;
    TrackStatus_target[ ind ].isStepwiseInterpolation = false;
    TrackStatus_target[ ind ].interpolationStep = 0.00001F;
    TrackStatus_target[ ind ].readSpeedScale = 1.0F;
  }

  for( int ind = 0 ; ind < PG_NB_TRACKS ; ind++ ) {
    line_tracks_vao[ind] = 0;
    line_tracks_target_vao[ind] = 0;

    line_tracks_points[ind] = NULL;
    line_tracks_target_points[ind] = NULL;

	synchr_start_recording_source_track[ind] = false;
	synchr_start_recording_target_track[ind] = false;
	synchr_start_replay_track[ind] = false;
  }

  // int indDepth = 0;
  // char fileName[128];

  // // loads 4 source tracks
  // strcpy( fileName ,  "alK/textures/SVGs/fall_src_01_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 0 , fileName , true );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_src_02_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 1 , fileName , true );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_src_03_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 2 , fileName , true );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_src_04_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 3 , fileName , true );

  // // loads 4 target tracks
  // strcpy( fileName ,  "alK/textures/SVGs/fall_tgt_01_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 0 , fileName , false );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_tgt_02_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 1 , fileName , false );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_tgt_03_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 2 , fileName , false );
  // strcpy( fileName ,  "alK/textures/SVGs/fall_tgt_04_01.svg" );
  // indDepth = 0;
  // readsvg( &indDepth , 3 , fileName , false );

  ///////////////////////////////////////////////////
  // NVIDIA PATH RENDERING
  ///////////////////////////////////////////////////
  // glPathCommandsNV = NULL;
  // glPathStringNV = NULL;
  // glPathParameteriNV = NULL;
  // glPathParameterfNV = NULL;
  // glCoverFillPathNV = NULL;
  // glCoverStrokePathNV = NULL;
  // glStencilFillPathNV = NULL;
  // glStencilStrokePathNV = NULL;
  
  // glMatrixLoadIdentityEXT = NULL;
  // glMatrixOrthoEXT = NULL;

  // NV_PR_init();
}

void pg_trackVariablesInit( void ) {
  for( int ind = 0 ; ind < PG_NB_TRACKS ; ind++ ) {
    tracks_x[ind] = 0.0f;
    tracks_y[ind] = 0.0f;
    tracks_x_prev[ind] = 0.0f;
    tracks_y_prev[ind] = 0.0f;
    tracks_Color_r[ind] = 1.0f;
    tracks_Color_g[ind] = 1.0f;
    tracks_Color_b[ind] = 1.0f;
    tracks_Color_a[ind] = 1.0f;
    tracks_BrushID[ind] = 1;
    tracks_RadiusX[ind] = 3.0f;
    tracks_RadiusY[ind] = 3.0f;
  }
}

//////////////////////////////////////////////////////////////////
// TARGET TRACK NORMALIZATION PRIOR TO INTERPOLATION
//////////////////////////////////////////////////////////////////
// adjusts the duration of the target to the source
// replaces the current control points of the target by control points 
// that are synchronous with the ones of the source
void NormalizeTargetTrack( int indTrack ) {
  if( TrackStatus_target[ indTrack ].isNormalized ) {
    return;
  }
  if( TrackStatus_source[ indTrack ].nbRecordedFrames <= 4 
      || TrackStatus_target[ indTrack ].nbRecordedFrames <= 4 ) {
    TrackStatus_source[ indTrack ].isActiveInterpolation = false;
    return;
  }

  for( int indFrameRec = 0 ; 
       indFrameRec < TrackStatus_target[ indTrack ].nbRecordedFrames ;
       indFrameRec++ ) {
    // copies all the target frame values
    TrackColor_r_temp_target[ indTrack ][ indFrameRec ] = TrackColor_r_target[ indTrack ][ indFrameRec ]; 
    TrackColor_g_temp_target[ indTrack ][ indFrameRec ] = TrackColor_g_target[ indTrack ][ indFrameRec ]; 
    TrackColor_b_temp_target[ indTrack ][ indFrameRec ] = TrackColor_b_target[ indTrack ][ indFrameRec ]; 
    TrackColor_a_temp_target[ indTrack ][ indFrameRec ] = TrackColor_a_target[ indTrack ][ indFrameRec ]; 
    TrackBrushID_temp_target[ indTrack ][ indFrameRec ] = TrackBrushID_target[ indTrack ][ indFrameRec ]; 
    TrackRadiusX_temp_target[ indTrack ][ indFrameRec ] = TrackRadiusX_target[ indTrack ][ indFrameRec ]; 
    TrackRadiusY_temp_target[ indTrack ][ indFrameRec ] = TrackRadiusY_target[ indTrack ][ indFrameRec ]; 
    TrackPos_x_temp_target[ indTrack ][ indFrameRec ] = TrackPos_x_target[ indTrack ][ indFrameRec ]; 
    TrackPos_y_temp_target[ indTrack ][ indFrameRec ] = TrackPos_y_target[ indTrack ][ indFrameRec ]; 
    TrackTime_temp_target[ indTrack ][ indFrameRec ] = TrackTime_target[ indTrack ][ indFrameRec ];
  }

  double sourceDuration = TrackTime_source[ indTrack ][ TrackStatus_source[ indTrack ].nbRecordedFrames - 1 ]
    - TrackStatus_source[ indTrack ].initialTimeRecording;
  double targetDuration = TrackTime_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ]
    - TrackStatus_target[ indTrack ].initialTimeRecording;
  double timeRatio = sourceDuration/targetDuration;

  // makes a copy of the initially recorded target track so that it can be normalized
  // and overwritten in the target track values

  // copies first frame of both
  // copies the time of the source frame
  // copies the current time in the current frame
  TrackTime_target[ indTrack ][ 0 ] = TrackTime_source[ indTrack ][ 0 ];

  // copies the data of the temporary stored target frame
  TrackColor_r_target[ indTrack ][ 0 ] = TrackColor_r_temp_target[ indTrack ][ 0 ]; 
  TrackColor_g_target[ indTrack ][ 0 ] = TrackColor_g_temp_target[ indTrack ][ 0 ]; 
  TrackColor_b_target[ indTrack ][ 0 ] = TrackColor_b_temp_target[ indTrack ][ 0 ]; 
  TrackColor_a_target[ indTrack ][ 0 ] = TrackColor_a_temp_target[ indTrack ][ 0 ]; 
  TrackBrushID_target[ indTrack ][ 0 ] = TrackBrushID_temp_target[ indTrack ][ 0 ]; 
  TrackRadiusX_target[ indTrack ][ 0 ] = TrackRadiusX_temp_target[ indTrack ][ 0 ]; 
  TrackRadiusY_target[ indTrack ][ 0 ] = TrackRadiusY_temp_target[ indTrack ][ 0 ]; 
  TrackPos_x_target[ indTrack ][ 0 ] = TrackPos_x_temp_target[ indTrack ][ 0 ]; 
  TrackPos_y_target[ indTrack ][ 0 ] = TrackPos_y_temp_target[ indTrack ][ 0 ]; 

  // retiming and point interpolation to define synchronous target curve 
  int indTargetFrame = 0;
  double previousTargetElapsedTime = 0;
  double nextTargetElapsedTime 
    = timeRatio * (TrackTime_temp_target[ indTrack ][ 1 ] 
		   - TrackStatus_target[ indTrack ].initialTimeRecording);  
  double sourceElapsedTime = 0;
    
  // scans all the source frames except first and last ones
  for(int indSourceFrame = 1 ; 
      indSourceFrame < TrackStatus_source[ indTrack ].nbRecordedFrames - 1; 
      indSourceFrame++ ) {
    // source time of the current source frame
    sourceElapsedTime = TrackTime_source[ indTrack ][ indSourceFrame ] 
      - TrackStatus_source[ indTrack ].initialTimeRecording;

    // looking at the index in the target frame that corresponds to
    // the first target frame later than the current source one
    while( nextTargetElapsedTime < sourceElapsedTime 
	   && indTargetFrame + 2 < TrackStatus_target[ indTrack ].nbRecordedFrames ) {
      indTargetFrame++;
      previousTargetElapsedTime = nextTargetElapsedTime;
      nextTargetElapsedTime = timeRatio * (TrackTime_temp_target[ indTrack ][ indTargetFrame + 1 ] 
				       - TrackStatus_target[ indTrack ].initialTimeRecording);
    }

    if( !((previousTargetElapsedTime <= sourceElapsedTime)
	  &&(sourceElapsedTime <= nextTargetElapsedTime)) ) {
      printf("Error: the current source time is not contained between two target times\nsource time %f target times %f %f index source %d index target %d size source %d size target %d\n" ,
	     sourceElapsedTime , previousTargetElapsedTime , nextTargetElapsedTime ,
	     indSourceFrame , indTargetFrame ,TrackStatus_source[ indTrack ].nbRecordedFrames , TrackStatus_target[ indTrack ].nbRecordedFrames);
      printf("Duration source %.2f target %.2f time ratio %.2f\n" ,(float)sourceDuration, (float)targetDuration, (float)timeRatio);
    }

    // interpolation coefficient used for both Lerp and Spline interpolation
    float interpolationStepf = 0;
    if( previousTargetElapsedTime != nextTargetElapsedTime ) {
      interpolationStepf = (float)((sourceElapsedTime - previousTargetElapsedTime)
								/ (nextTargetElapsedTime - previousTargetElapsedTime));
    }

    // linear interpolation of the float parameters other than position
    TrackColor_r_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackColor_r_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackColor_r_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );
    TrackColor_g_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackColor_g_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackColor_g_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );
    TrackColor_b_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackColor_b_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackColor_b_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );
    TrackColor_a_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackColor_a_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackColor_a_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );
    TrackRadiusX_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackRadiusX_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackRadiusX_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );
    TrackRadiusY_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackRadiusY_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackRadiusY_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );

    // upper value of brush ID
    TrackBrushID_target[ indTrack ][ indSourceFrame ]
      = TrackBrushID_temp_target[ indTrack ][ indTargetFrame + 1 ];

    // spline intperpolation for position
    vec2 p0,p1,p2,p3,p_interp;
    // point before previous one
    if( indTargetFrame > 0 ) {
      p0.x = (float)TrackPos_x_temp_target[ indTrack ][ indTargetFrame - 1 ];
      p0.y = (float)TrackPos_y_temp_target[ indTrack ][ indTargetFrame - 1 ];
    }
    else {
      p0.x = (float)TrackPos_x_temp_target[ indTrack ][ 0 ];
      p0.y = (float)TrackPos_y_temp_target[ indTrack ][ 0 ];
    }
    // previous point (before current source frame time)
    p1.x = (float)TrackPos_x_temp_target[ indTrack ][ indTargetFrame ];
    p1.y = (float)TrackPos_y_temp_target[ indTrack ][ indTargetFrame ]; 
    // next point (after current source frame time)
    if( indTargetFrame + 1 < TrackStatus_target[ indTrack ].nbRecordedFrames ) {
      p2.x = (float)TrackPos_x_temp_target[ indTrack ][ indTargetFrame + 1 ];
      p2.y = (float)TrackPos_y_temp_target[ indTrack ][ indTargetFrame + 1 ];
    }
    else {
      p2.x = (float)TrackPos_x_temp_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ];
      p2.y = (float)TrackPos_y_temp_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ];
    }
    // point after next
    if( indTargetFrame + 2 < TrackStatus_target[ indTrack ].nbRecordedFrames ) {
      p3.x = (float)TrackPos_x_temp_target[ indTrack ][ indTargetFrame + 2 ];
      p3.y = (float)TrackPos_y_temp_target[ indTrack ][ indTargetFrame + 2 ];
    }
    else {
      p3.x = (float)TrackPos_x_temp_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ];
      p3.y = (float)TrackPos_y_temp_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ];
    }
    // spline interpolator to smooth the curve
    CatmullRom_Evaluate( &p_interp, &p0, &p1, &p2, &p3, interpolationStepf);
    TrackPos_x_target[ indTrack ][ indSourceFrame ] = (float)round(p_interp.x);
    TrackPos_y_target[ indTrack ][ indSourceFrame ] = (float)round(p_interp.y);

    // Catmull-Rom spline: computes right and left tangent of the preceding point from 
    // position of current point and current point - 2
    if( indSourceFrame > 1 ) {
      float tan_x = Tension * ( TrackPos_x_target[ indTrack ][ indSourceFrame ] - TrackPos_x_target[ indTrack ][ indSourceFrame - 2 ]);
      float tan_y = Tension * ( TrackPos_y_target[ indTrack ][ indSourceFrame ] - TrackPos_y_target[ indTrack ][ indSourceFrame - 2 ]); 
      TrackPos_xL_target[ indTrack ][ indSourceFrame - 1 ] 
	= TrackPos_x_target[ indTrack ][ indSourceFrame - 1 ] - tan_x;
      TrackPos_yL_target[ indTrack ][ indSourceFrame - 1 ]  
	= TrackPos_y_target[ indTrack ][ indSourceFrame - 1 ] - tan_y;
      TrackPos_xR_target[ indTrack ][ indSourceFrame - 1 ] 
	= TrackPos_x_target[ indTrack ][ indSourceFrame - 1 ] + tan_x;
      TrackPos_yR_target[ indTrack ][ indSourceFrame - 1 ]  
	= TrackPos_y_target[ indTrack ][ indSourceFrame - 1 ] + tan_y;
      // special case of first curve point
      // right tangent is half way between first point and left tangent extremity of second point
      if( indSourceFrame == 2 ) {
	TrackPos_xR_target[ indTrack ][ 0 ] = TrackPos_x_target[ indTrack ][ 0 ] 
	  + 0.5F * ( TrackPos_xL_target[ indTrack ][ 1 ] - TrackPos_x_target[ indTrack ][ 0 ]); 
	TrackPos_yR_target[ indTrack ][ 0 ] = TrackPos_y_target[ indTrack ][ 0 ] 
	  + 0.5F * ( TrackPos_yL_target[ indTrack ][ 1 ] - TrackPos_y_target[ indTrack ][ 0 ]); 
      }
    }

    // copies the current time in the current frame
    TrackTime_target[ indTrack ][ indSourceFrame ]
      = TrackTime_source[ indTrack ][ indSourceFrame ];
  }

  // copies last frame of both
  int indSourceFrame = TrackStatus_source[ indTrack ].nbRecordedFrames - 1;
  indTargetFrame = TrackStatus_target[ indTrack ].nbRecordedFrames - 1;
  // copies the final time and number of frames of the source track
  TrackStatus_target[ indTrack ].initialTimeRecording
    = TrackStatus_source[ indTrack ].initialTimeRecording;
  TrackStatus_target[ indTrack ].nbRecordedFrames
    = TrackStatus_source[ indTrack ].nbRecordedFrames;

  // copies the current time in the current frame
  TrackTime_target[ indTrack ][ indSourceFrame ]
    = TrackTime_source[ indTrack ][ indSourceFrame ];

  // copies the data of the temporary stored target frame
  TrackColor_r_target[ indTrack ][ indSourceFrame ] = TrackColor_r_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_g_target[ indTrack ][ indSourceFrame ] = TrackColor_g_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_b_target[ indTrack ][ indSourceFrame ] = TrackColor_b_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_a_target[ indTrack ][ indSourceFrame ] = TrackColor_a_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackBrushID_target[ indTrack ][ indSourceFrame ] = TrackBrushID_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackRadiusX_target[ indTrack ][ indSourceFrame ] = TrackRadiusX_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackRadiusY_target[ indTrack ][ indSourceFrame ] = TrackRadiusY_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackPos_x_target[ indTrack ][ indSourceFrame ] = TrackPos_x_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackPos_y_target[ indTrack ][ indSourceFrame ] = TrackPos_y_temp_target[ indTrack ][ indTargetFrame ]; 

  // marks the track as normalized
  TrackStatus_target[ indTrack ].isNormalized = true;
}

//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A PATH STRING
//////////////////////////////////////////////////////////////////
void LoadTrackFromPath( char *pathString , int indTrack , 
			float *translation , bool isSource ) {
  float          interFrameDuration = .02F;
  int            indSpline = 0;
  int            curChar = ' ';
  int            indChar = 0;
  float          currentPoint[2] = { 0.0 , 0.0 };  
  float          curveInitialPoint[2] = { 0.0 , 0.0 };
  float          polyCurveInitialPoint[2] = { 0.0 , 0.0 };

  if( isSource ) {
    TrackStatus_source[ indTrack ].isFirstFrame = false;
    TrackStatus_source[ indTrack ].isActiveInterpolation = false;
    TrackStatus_source[ indTrack ].isActiveRecording = false;
    TrackStatus_source[ indTrack ].isNormalized = false;
    TrackStatus_source[ indTrack ].nbRecordedFrames = 0;
    TrackStatus_source[ indTrack ].indReading = 0;
    TrackStatus_source[ indTrack ].coefInterpolation = 0;
    TrackStatus_source[ indTrack ].initialTimeRecording = 0.0;
    TrackStatus_source[ indTrack ].initialTimeReading = 0.0;
    TrackStatus_source[ indTrack ].isStepwiseInterpolation = false;
    TrackStatus_source[ indTrack ].interpolationStep = 0.00001F;
    TrackStatus_source[ indTrack ].readSpeedScale = 1.0F;
  }
  else {
    TrackStatus_target[ indTrack ].isFirstFrame = false;
    TrackStatus_target[ indTrack ].isActiveInterpolation = false;
    TrackStatus_target[ indTrack ].isActiveRecording = false;
    TrackStatus_target[ indTrack ].isNormalized = false;
    TrackStatus_target[ indTrack ].nbRecordedFrames = 0;
    TrackStatus_target[ indTrack ].indReading = 0;
    TrackStatus_target[ indTrack ].coefInterpolation = 0;
    TrackStatus_target[ indTrack ].initialTimeRecording = 0.0;
    TrackStatus_target[ indTrack ].initialTimeReading = 0.0;
    TrackStatus_target[ indTrack ].isStepwiseInterpolation = false;
    TrackStatus_target[ indTrack ].interpolationStep = 0.00001F;
    TrackStatus_target[ indTrack ].readSpeedScale = 1.0F;
  }

  while( _SpaceChar(curChar) ) {
    curChar = pathString[ indChar++ ];
  }
  while( indChar <= (int)strlen( pathString ) && curChar != EOF  && curChar != '"' ) {
    switch( curChar ) {
    case 'm':
    case 'M':
      {
	// RULES
	// Start a new sub-path at the given (x,y) coordinate. M
	// (uppercase) indicates that absolute coordinates will
	// follow; m (lowercase) indicates that relative coordinates
	// will follow. If a moveto is followed by multiple pairs of
	// coordinates, the subsequent pairs are treated as implicit
	// lineto commands. Hence, implicit lineto commands will be
	// relative if the moveto is relative, and absolute if the
	// moveto is absolute. If a relative moveto (m) appears as the
	// first element of the path, then it is treated as a pair of
	// absolute coordinates. In this case, subsequent pairs of
	// coordinates are treated as relative even though the initial
	// moveto is interpreted as an absolute moveto.
	int nbMovePoints = 0;
	bool relative = (curChar == 'm');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative  && indSpline != 0 )
	    currentPoint[0] += pg_ScanFloatString( &curChar , true  , 
					 pathString , &indChar );
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }
	  if( relative  && indSpline != 0 )
	    currentPoint[1] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }
	  nbMovePoints++;
	  if( indSpline < max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackColor_r_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_source[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_source[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_source[ indTrack ][ indSpline ] = 0.0;
	      TrackRadiusY_source[ indTrack ][ indSpline ] = 0.0;
	      TrackPos_x_source[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_source[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_source[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("src move to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x_source[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y_source[ indTrack ][ indSpline ]);
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_target[ indTrack ][ indSpline ] = 0.0;
	      TrackRadiusY_target[ indTrack ][ indSpline ] = 0.0;
	      TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("tgt move to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x_source[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y_source[ indTrack ][ indSpline ]);
	    }
	    indSpline++;
	  }
	}
	polyCurveInitialPoint[0] = currentPoint[0];
	polyCurveInitialPoint[1] = currentPoint[1];
      }
      break;
    case 'c':
    case 'C':
      {
	// RULES 
	// Draws a cubic Bézier curve from the current point to
	// (x,y) using (x1,y1) as the control point at the beginning
	// of the curve and (x2,y2) as the control point at the end of
	// the curve. C (uppercase) indicates that absolute
	// coordinates will follow; c (lowercase) indicates that
	// relative coordinates will follow. Multiple sets of
	// coordinates may be specified to draw a polybézier. At the
	// end of the command, the new current point becomes the final
	// (x,y) coordinate pair used in the polybézier.
	int nbCurvePoints = 0;
	bool relative = (curChar == 'c');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}

	curveInitialPoint[0] = currentPoint[0];
	curveInitialPoint[1] = currentPoint[1];
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative )
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar ) + curveInitialPoint[0];
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }
	  if( relative )
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar ) + curveInitialPoint[1];
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }
	  nbCurvePoints++;

	  if( (nbCurvePoints) % 3 == 0  && indSpline < max_mouse_recording_frames ) {	    
	    if( isSource ) {
	      TrackColor_r_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_source[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_source[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_source[ indTrack ][ indSpline ] = 3.0;
	      TrackRadiusY_source[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x_source[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_source[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_source[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // if( indSpline < 5 ) {
	      // 	printf("src curve to %d %f %f\n" , indSpline ,
	      // 	       TrackPos_x_source[ indTrack ][ indSpline ] , 
	      // 	       TrackPos_y_source[ indTrack ][ indSpline ]);
	      // }
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_target[ indTrack ][ indSpline ] = 3.0;
	      TrackRadiusY_target[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("tgt curve to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x_target[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y_target[ indTrack ][ indSpline ]);
	    }

	    curveInitialPoint[0] = currentPoint[0];
	    curveInitialPoint[1] = currentPoint[1];
	    indSpline++;
	  }
	  // right tangent of the first (and current) point
	  else if( (nbCurvePoints) % 3 == 1  && indSpline > 0 && indSpline < max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackPos_xR_source[ indTrack ][ indSpline - 1 ] = currentPoint[0] + translation[0];
	      TrackPos_yR_source[ indTrack ][ indSpline - 1 ] = currentPoint[1] + translation[1];
	    }
	    else {
	    }
	      TrackPos_xR_target[ indTrack ][ indSpline - 1 ] = currentPoint[0] + translation[0];
	      TrackPos_yR_target[ indTrack ][ indSpline - 1 ] = currentPoint[1] + translation[1];
	  }
	  // left tangent of the second (and next) point
	  else if( (nbCurvePoints) % 3 == 2  && indSpline < max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackPos_xL_source[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_yL_source[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    }
	    else {
	      TrackPos_xL_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_yL_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    }
	  }
	}
	if( nbCurvePoints % 3 != 0 ) {
	  printf( "Bezier curve points are expected to be given by groups of 3 %d next %c %f %f!\n" , nbCurvePoints , curChar , currentPoint[0] , currentPoint[1]);
	}	  
      }
      break;
    case 'l':
    case 'L':
      {
	// RULES 
	// Draw a line from the current point to the given (x,y)
	// coordinate which becomes the new current point. L
	// (uppercase) indicates that absolute coordinates will
	// follow; l (lowercase) indicates that relative coordinates
	// will follow. A number of coordinates pairs may be specified
	// to draw a polyline. At the end of the command, the new
	// current point is set to the final set of coordinates
	// provided.
	bool relative = (curChar == 'l');
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}

	if( indSpline < max_mouse_recording_frames ) {
	  if( isSource ) {
	    TrackColor_r_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_source[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_source[ indTrack ][ indSpline ] = 0;
	    TrackRadiusX_source[ indTrack ][ indSpline ] = 3.0;
	    TrackRadiusY_source[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_source[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	    TrackPos_y_source[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    TrackTime_source[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  else {
	    TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	    TrackRadiusX_target[ indTrack ][ indSpline ] = 3.0;
	    TrackRadiusY_target[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	    TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  // printf("line to %d %f %f\n" , indSpline ,
	  // 	 TrackPos_x_source[ indTrack ][ indSpline ] , 
	  // 	 TrackPos_y_source[ indTrack ][ indSpline ]);

	  indSpline++;
	}
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 

	  if( relative )
	    currentPoint[0] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[0] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  if( curChar == ',' ) {
	    curChar = pathString[ indChar++ ];
	    while( _SpaceChar(curChar) ) {
	      curChar = pathString[ indChar++ ];
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Expected char [%c] not found!" , ',' ); ReportError( ErrorStr ); throw 17;
	  }
	  if( relative )
	    currentPoint[1] += pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  else
	    currentPoint[1] = pg_ScanFloatString( &curChar , true  , 
					  pathString , &indChar );
	  while( _SpaceChar(curChar) ) {
	    curChar = pathString[ indChar++ ];
	  }

	  if( indSpline < max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackColor_r_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_source[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_source[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_source[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_source[ indTrack ][ indSpline ] = 3.0;
	      TrackRadiusY_source[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x_source[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_source[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_source[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      
	      // left tangent of current point is preceding point
	      // and right tangent of preceding point is the current point
	      TrackPos_xL_source[ indTrack ][ indSpline ] 
		= TrackPos_x_source[ indTrack ][ indSpline - 1 ];
	      TrackPos_yL_source[ indTrack ][ indSpline ]
		= TrackPos_y_source[ indTrack ][ indSpline -1 ];
	      TrackPos_xR_source[ indTrack ][ indSpline - 1 ] 
		= TrackPos_x_source[ indTrack ][ indSpline ];
	      TrackPos_yR_source[ indTrack ][ indSpline - 1 ]
		= TrackPos_y_source[ indTrack ][ indSpline ];
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadiusX_target[ indTrack ][ indSpline ] = 3.0;
	      TrackRadiusY_target[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      
	      // left tangent of current point is preceding point
	      // and right tangent of preceding point is the current point
	      TrackPos_xL_target[ indTrack ][ indSpline ] 
		= TrackPos_x_target[ indTrack ][ indSpline - 1 ];
	      TrackPos_yL_target[ indTrack ][ indSpline ]
		= TrackPos_y_target[ indTrack ][ indSpline -1 ];
	      TrackPos_xR_target[ indTrack ][ indSpline - 1 ] 
		= TrackPos_x_target[ indTrack ][ indSpline ];
	      TrackPos_yR_target[ indTrack ][ indSpline - 1 ]
		= TrackPos_y_target[ indTrack ][ indSpline ];
	    }

	    // printf("new line to %d %f %f\n" , indSpline ,
	    // 	   TrackPos_x_source[ indTrack ][ indSpline ] , 
	    // 	   TrackPos_y_source[ indTrack ][ indSpline ]);
	    indSpline++;
	  }
	}
      }
      break;
    case 'z':
    case 'Z':
      {
	// RULES 
	// Close the current subpath by drawing a straight line
	// from the current point to current subpath's initial
	// point. Since the Z and z commands take no parameters, they
	// have an identical effect.
	curChar = pathString[ indChar++ ];
	while( _SpaceChar(curChar) ) {
	  curChar = pathString[ indChar++ ];
	}
	// printf("cut\n");
	if( indSpline < max_mouse_recording_frames ) {
	  if( isSource ) {
	    TrackColor_r_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_source[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_source[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_source[ indTrack ][ indSpline ] = 0;
	    TrackRadiusX_source[ indTrack ][ indSpline ] = 3.0;
	    TrackRadiusY_source[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_source[ indTrack ][ indSpline ] = polyCurveInitialPoint[0] + translation[0];
	    TrackPos_y_source[ indTrack ][ indSpline ] = polyCurveInitialPoint[1] + translation[1];
	    TrackTime_source[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  else {
	    TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	    TrackRadiusX_target[ indTrack ][ indSpline ] = 3.0;
	    TrackRadiusY_target[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_target[ indTrack ][ indSpline ] = polyCurveInitialPoint[0] + translation[0];
	    TrackPos_y_target[ indTrack ][ indSpline ] = polyCurveInitialPoint[1] + translation[1];
	    TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }

	  // printf("close curve %d %f %f\n" , indSpline ,
	  // 	 TrackPos_x_source[ indTrack ][ indSpline ] , 
	  // 	 TrackPos_y_source[ indTrack ][ indSpline ]);
	  indSpline++;
	}
      }
      break;
    default:
      {
	sprintf( ErrorStr , "Unknown SVG path tag [%c] %d!" , curChar , curChar ); ReportError( ErrorStr ); throw(100);
      }
      break;
    }  
  }
  if( isSource ) {
    TrackStatus_source[ indTrack ].nbRecordedFrames =indSpline;
  }
  else {
    TrackStatus_target[ indTrack ].nbRecordedFrames =indSpline;
  }
}

//////////////////////////////////////////////////////////////////
// MEMORY STORAGE OF THE CURRENT STROKE  
//////////////////////////////////////////////////////////////////
// update of the tables that contain thecurrrentTrack stroke parameters
void updateMouseEnvironmentVariablesAndTables(double theTime) {
	// the storage in the preceding mouse position variable is
	// made in the script of the scene XML
	// -1 is background layer

	// memory of current and preceding values
	mouse_x = (float)CurrentMousePos_x;
	mouse_y = (float)CurrentMousePos_y;

	// memory about drawing at preceding step
	bool wasDrawingOn = isDrawingOn;
	isDrawingOn = (activeDrawingStroke > 0);

	// move second step: the left tangent is null
	bool wasAMove = isAMove;
	// move first step: the right tangent is null
	isAMove = (isDrawingOn && !wasDrawingOn);

	///////////////////////////////////////////////////////////////////////////
	// SOURCE AND TARGET TRACK RECORDING
	// no update if the mouse has not been moving
	// no update if the mode is not a drawing mode
	if (((mouse_x_prev != CurrentMousePos_x)
		|| (mouse_y_prev != CurrentMousePos_y)
		|| isAMove || wasAMove)
		&& isDrawingOn) {
		// printf("mouse pos %dx%d\n",CurrentMousePos_x,CurrentMousePos_y);

		///////////////////////////////////////////////////////////////////////
		// RECORDING SOURCE TRACK
		///////////////////////////////////////////////////////////////////////
		if (TrackStatus_source[0].isActiveRecording
			&& TrackStatus_source[0].nbRecordedFrames
			< max_mouse_recording_frames) {
			int indFrameRec = TrackStatus_source[0].nbRecordedFrames;
			// printf( "rec track %d frame %d\n",0,indFrameRec );

			// records the initial time and the current time to play in synch
			if (indFrameRec == 0) {
				TrackStatus_source[0].initialTimeRecording = theTime;
			}
			TrackTime_source[0][indFrameRec] = theTime;

			TrackColor_r_source[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[0] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_g_source[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[1] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_b_source[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[2] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_a_source[0][indFrameRec]
				= 1.0;
			// printf( "capture %.2f %.2f %.2f %.2f\n" , 
			// 	      TrackColor_r_source[ 0 ][ indFrameRec ] ,
			// 	      TrackColor_g_source[ 0 ][ indFrameRec ] ,
			// 	      TrackColor_b_source[ 0 ][ indFrameRec ] ,
			// 	      TrackColor_a_source[ 0 ][ indFrameRec ] );

			TrackBrushID_source[0][indFrameRec]
				= 0;

#ifdef PG_WACOM_TABLET
			TrackRadiusX_source[0][indFrameRec]
				= brushRadius
				+ tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * brush_radius_angleHor_coef;
			TrackRadiusY_source[0][indFrameRec]
				= brushRadius
				+ tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * brush_radius_angleVer_coef;
#else
			TrackRadiusX_source[0][indFrameRec]
				= brushRadius;
			TrackRadiusY_source[0][indFrameRec]
				= brushRadius;
#endif

			TrackPos_x_source[0][indFrameRec] = (float)CurrentMousePos_x;
			TrackPos_y_source[0][indFrameRec] = (float)CurrentMousePos_y;

			// Catmull-Rom spline: computes right and left tangent of the preceding point from 
			// position of current point and current point - 2
			if (indFrameRec > 1) {
				// sets tangent extremities to 0 in case of move for not redrawing the
				// jump later & for inserting a move in the saved svg
				float tan_x = Tension * (TrackPos_x_source[0][indFrameRec] - TrackPos_x_source[0][indFrameRec - 2]);
				float tan_y = Tension * (TrackPos_y_source[0][indFrameRec] - TrackPos_y_source[0][indFrameRec - 2]);
				// move second step: null left tangent of current point
				if (!wasAMove) {
					TrackPos_xL_source[0][indFrameRec - 1]
						= TrackPos_x_source[0][indFrameRec - 1] - tan_x;
					TrackPos_yL_source[0][indFrameRec - 1]
						= TrackPos_y_source[0][indFrameRec - 1] - tan_y;
				}
				else {
					TrackPos_xL_source[0][indFrameRec - 1] = 0.0;
					TrackPos_yL_source[0][indFrameRec - 1] = 0.0;
					// a null left tangent was givent at next frame
				}
				// move first step: null right tangent extremity of preceding point
				// so that it is saved as a move
				if (!isAMove) {
					if (!wasAMove) {
						TrackPos_xR_source[0][indFrameRec - 1]
							= TrackPos_x_source[0][indFrameRec - 1] + tan_x;
						TrackPos_yR_source[0][indFrameRec - 1]
							= TrackPos_y_source[0][indFrameRec - 1] + tan_y;
					}
					else {
						// first point in a new curve
						TrackPos_xR_source[0][indFrameRec - 1]
							= TrackPos_x_source[0][indFrameRec - 1];
						TrackPos_yR_source[0][indFrameRec - 1]
							= TrackPos_y_source[0][indFrameRec - 1];
					}
				}
				else {
					// assigns null values to right tangent extremity so that it is saved
					// as a move
					TrackPos_xR_source[0][indFrameRec - 1] = 0.0;
					TrackPos_yR_source[0][indFrameRec - 1] = 0.0;
					// a null left tangent extremity will be givent at next frame

					// null left tangent at closing point before the move
					TrackPos_xL_source[0][indFrameRec - 1] = TrackPos_x_source[0][indFrameRec - 1];
					TrackPos_yL_source[0][indFrameRec - 1] = TrackPos_y_source[0][indFrameRec - 1];
				}

				// right tangent is half way between first point and left tangent extremity of second point
				if (indFrameRec == 2) {
					TrackPos_xR_source[0][0] = TrackPos_x_source[0][0]
						+ 0.5F * (TrackPos_xL_source[0][1] - TrackPos_x_source[0][0]);
					TrackPos_yR_source[0][0] = TrackPos_y_source[0][0]
						+ 0.5F * (TrackPos_yL_source[0][1] - TrackPos_y_source[0][0]);
				}
			}
			// printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , 0 + 1 , indFrameRec - 1 , (int)TrackPos_xL_source[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_yL_source[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_xR_source[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_yR_source[ 0 ][ indFrameRec - 1 ] );

			TrackStatus_source[0].nbRecordedFrames++;
		}
		// RECORDING SOURCE TRACK

		///////////////////////////////////////////////////////////////////////
		// RECORDING INTERPOLATION TARGET TRACK
		///////////////////////////////////////////////////////////////////////
		if (TrackStatus_target[0].isActiveRecording
			&& TrackStatus_target[0].nbRecordedFrames
			< max_mouse_recording_frames) {
			int indFrameRec = TrackStatus_target[0].nbRecordedFrames;

			// records the initial time and the current time to play in synch
			if (indFrameRec == 0) {
				TrackStatus_target[0].initialTimeRecording = theTime;
			}
			TrackTime_target[0][indFrameRec] = theTime;

			TrackColor_r_target[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[0] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_g_target[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[1] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_b_target[0][indFrameRec]
				= local_color_gray
				+ RGBpalette_received[2] * (RGBpalette_val_coef + RGBpalette_val_const);
			TrackColor_a_target[0][indFrameRec]
				= 1.0;
			// printf( "capture %.2f %.2f %.2f\n" , 
			// 	      TrackColor_r_target[ 0 ][ indFrameRec ] ,
			// 	      TrackColor_g_target[ 0 ][ indFrameRec ] ,
			// 	      TrackColor_b_target[ 0 ][ indFrameRec ] );

			TrackBrushID_target[0][indFrameRec]
				= 0;

#ifdef PG_WACOM_TABLET
			TrackRadiusX_target[0][indFrameRec]
				= brushRadius
				+ tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * brush_radius_angleHor_coef;
			TrackRadiusY_target[0][indFrameRec]
				= brushRadius
				+ tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * brush_radius_angleVer_coef;
#else
			TrackRadiusX_target[0][indFrameRec]
				= brushRadius;
			TrackRadiusY_target[0][indFrameRec]
				= brushRadius;
#endif

			TrackPos_x_target[0][indFrameRec] = (float)CurrentMousePos_x;
			TrackPos_y_target[0][indFrameRec] = (float)CurrentMousePos_y;

			// Catmull-Rom spline: computes right and left tangent of the preceding point from 
			// position of current point and current point - 2
			if (indFrameRec > 1) {
				// sets tangent extremities to 0 in case of move for not redrawing the
				// jump later & for inserting a move in the saved svg
				float tan_x = Tension * (TrackPos_x_target[0][indFrameRec] - TrackPos_x_target[0][indFrameRec - 2]);
				float tan_y = Tension * (TrackPos_y_target[0][indFrameRec] - TrackPos_y_target[0][indFrameRec - 2]);
				// move second step: null left tangent of current point
				if (!wasAMove) {
					TrackPos_xL_target[0][indFrameRec - 1]
						= TrackPos_x_target[0][indFrameRec - 1] - tan_x;
					TrackPos_yL_target[0][indFrameRec - 1]
						= TrackPos_y_target[0][indFrameRec - 1] - tan_y;
				}
				else {
					TrackPos_xL_target[0][indFrameRec - 1] = 0.0;
					TrackPos_yL_target[0][indFrameRec - 1] = 0.0;
					// a null left tangent was givent at next frame
				}
				// move first step: null right tangent extremity of preceding point
				// so that it is saved as a move
				if (!isAMove) {
					if (!wasAMove) {
						TrackPos_xR_target[0][indFrameRec - 1]
							= TrackPos_x_target[0][indFrameRec - 1] + tan_x;
						TrackPos_yR_target[0][indFrameRec - 1]
							= TrackPos_y_target[0][indFrameRec - 1] + tan_y;
					}
					else {
						// first point in a new curve
						TrackPos_xR_target[0][indFrameRec - 1]
							= TrackPos_x_target[0][indFrameRec - 1];
						TrackPos_yR_target[0][indFrameRec - 1]
							= TrackPos_y_target[0][indFrameRec - 1];
					}
				}
				else {
					// assigns null values to right tangent extremity so that it is saved
					// as a move
					TrackPos_xR_target[0][indFrameRec - 1] = 0.0;
					TrackPos_yR_target[0][indFrameRec - 1] = 0.0;
					// a null left tangent extremity will be givent at next frame

					// null left tangent at closing point before the move
					TrackPos_xL_target[0][indFrameRec - 1] = TrackPos_x_target[0][indFrameRec - 1];
					TrackPos_yL_target[0][indFrameRec - 1] = TrackPos_y_target[0][indFrameRec - 1];
				}
				// printf( "Track %d Ind %d tan %d %d / prev pos %d %d\n" , ind + 1 , indFrameRec - 1 , (int)TrackPos_xL_target[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_yL_target[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_xR_target[ 0 ][ indFrameRec - 1 ] , (int)TrackPos_yR_target[ 0 ][ indFrameRec - 1 ] );
				// right tangent is half way between first point and left tangent extremity of second point
				if (indFrameRec == 2) {
					TrackPos_xR_target[0][0] = TrackPos_x_target[0][0]
						+ 0.5F * (TrackPos_xL_target[0][1] - TrackPos_x_target[0][0]);
					TrackPos_yR_target[0][0] = TrackPos_y_target[0][0]
						+ 0.5F * (TrackPos_yL_target[0][1] - TrackPos_y_target[0][0]);
				}
			}

			TrackStatus_target[0].nbRecordedFrames++;
		}
		// RECORDING INTERPOLATION TARGET TRACK

	}

	///////////////////////////////////////////////////////////////////////
	// READING TRACK WITH OR WITHOUT INTERPOLATION WITH THE TARGET TRACK
	///////////////////////////////////////////////////////////////////////
	for (int indTrack = 0;
		indTrack < PG_NB_TRACKS; indTrack++) {
		// active reading
		bool is_trackreplay = false;
		switch (indTrack) {
		case 0:
			is_trackreplay = track_replay_0;
			break;
#if PG_NB_TRACKS >= 2
		case 1:
			is_trackreplay = track_replay_1;
			break;
#endif
#if PG_NB_TRACKS >= 3
		case 2:
			is_trackreplay = track_replay_2;
			break;
#endif
		}
		if (is_trackreplay) {
			// printf("read track %d\n" , indTrack );
			// int indFrameReading = TrackStatus_source[ indTrack ].indReading;

			// records the initial time and uses the elapsed reading
			// time and the elapsed recording time to play in synch
			if (TrackStatus_source[indTrack].indReading == 0) {
				TrackStatus_source[indTrack].initialTimeReading = theTime;
			}

			double theRecodingElapsedTime;
			double theReadingElapsedTime;
			bool isEndOfLoop = false;
			bool isCurveBreak = false;

			do {
				theRecodingElapsedTime =
					TrackTime_source[indTrack][TrackStatus_source[indTrack].indReading]
					- TrackStatus_source[indTrack].initialTimeRecording;
				theReadingElapsedTime =
					(theTime - TrackStatus_source[indTrack].initialTimeReading)
					* TrackStatus_source[indTrack].readSpeedScale;
				// printf( "Ind %d rec time %.2f read time %.2f\n" , TrackStatus_target[ indTrack ].indReading ,theRecodingElapsedTime , theReadingElapsedTime );
				// the negtive values correspond to a curve break. If they are jumped over the first 
				// following point should be negative
				if (TrackPos_x_source[indTrack][TrackStatus_source[indTrack].indReading] < 0
					|| TrackPos_y_source[indTrack][TrackStatus_source[indTrack].indReading] < 0) {
					isCurveBreak = true;
				}
				if (theRecodingElapsedTime <= theReadingElapsedTime) {
					TrackStatus_source[indTrack].indReading++;
				}
				else {
					break;
				}

				// loops at the end
				if (TrackStatus_source[indTrack].indReading
					>= TrackStatus_source[indTrack].nbRecordedFrames) {
					TrackStatus_source[indTrack].indReading = 0;
					isEndOfLoop = true;
					break;
				}
			} while (true);

			// loop also occurs at the end of an interpolation and
			// requires to have the current mouse position as preceding one also
			if ((TrackStatus_source[indTrack].isActiveInterpolation
				&& TrackStatus_source[indTrack].coefInterpolation == 0.0)
				|| TrackStatus_source[indTrack].isFirstFrame == true) {
				isEndOfLoop = true;
				TrackStatus_source[indTrack].isFirstFrame = false;
			}

			// management of previous mouse position with
			// consideration for looping

			float alpha = TrackStatus_source[indTrack].coefInterpolation;
			int indFrameReading = TrackStatus_source[indTrack].indReading;
			// printf("alpha %.6f ind %d\n" , TrackStatus_source[ indTrack ].coefInterpolation , TrackStatus_source[ indTrack ].indReading );

			if (isEndOfLoop) {
				if (alpha > 0) {
					tracks_x_prev[indTrack] = (float)Lerp((float)TrackPos_x_source[indTrack][indFrameReading],
						(float)TrackPos_x_target[indTrack][indFrameReading], alpha);
					tracks_y_prev[indTrack] = (float)Lerp((float)TrackPos_y_source[indTrack][indFrameReading],
						(float)TrackPos_y_target[indTrack][indFrameReading], alpha);
				}
				else {
					tracks_x_prev[indTrack] = (float)TrackPos_x_source[indTrack][indFrameReading];
					tracks_y_prev[indTrack] = (float)TrackPos_y_source[indTrack][indFrameReading];
				}
			}
			else {
				tracks_x_prev[indTrack] = tracks_x[indTrack];
				tracks_y_prev[indTrack] = tracks_y[indTrack];
			}

			// management of current mouse position with
			// consideration for looping      
			if (alpha > 0) {
				tracks_x[indTrack] = (float)Lerp((float)TrackPos_x_source[indTrack][indFrameReading],
					(float)TrackPos_x_target[indTrack][indFrameReading], alpha);
				tracks_y[indTrack] = (float)Lerp((float)TrackPos_y_source[indTrack][indFrameReading],
					(float)TrackPos_y_target[indTrack][indFrameReading], alpha);
			}
			else {
				tracks_x[indTrack] = (float)TrackPos_x_source[indTrack][indFrameReading];
				tracks_y[indTrack] = (float)TrackPos_y_source[indTrack][indFrameReading];
			}

			// negative values in case of curve break
			if (isCurveBreak) {
				tracks_x[indTrack] = -10000;
				tracks_y[indTrack] = -10000;
			}

			// management of color (w/wo possible interpolation)
			if (alpha > 0) {
				tracks_Color_r[indTrack] = (float)Lerp((float)TrackColor_r_source[indTrack][indFrameReading],
					(float)TrackColor_r_target[indTrack][indFrameReading], alpha);
				tracks_Color_g[indTrack] = (float)Lerp((float)TrackColor_g_source[indTrack][indFrameReading],
					(float)TrackColor_g_target[indTrack][indFrameReading], alpha);
				tracks_Color_b[indTrack] = (float)Lerp((float)TrackColor_b_source[indTrack][indFrameReading],
					(float)TrackColor_b_target[indTrack][indFrameReading], alpha);
				tracks_Color_a[indTrack] = (float)Lerp((float)TrackColor_a_source[indTrack][indFrameReading],
					(float)TrackColor_a_target[indTrack][indFrameReading], alpha);
			}
			else {
				tracks_Color_r[indTrack] = (float)TrackColor_r_source[indTrack][indFrameReading];
				tracks_Color_g[indTrack] = (float)TrackColor_g_source[indTrack][indFrameReading];
				tracks_Color_b[indTrack] = (float)TrackColor_b_source[indTrack][indFrameReading];
				tracks_Color_a[indTrack] = (float)TrackColor_a_source[indTrack][indFrameReading];
			}

			// printf( "replay %d %.2f %.2f %.2f\n" , indTrack ,
			// 	      track_replay[ indTrack ] ,
			// 	      tracks_x[ indTrack -1 ] ,
			// 	      tracks_y[ indTrack ] );

			// management of brush ID (w/wo possible interpolation)
			if (alpha < 0.5) {
				tracks_BrushID[indTrack] = TrackBrushID_source[indTrack][indFrameReading];
			}
			else {
				tracks_BrushID[indTrack] = TrackBrushID_target[indTrack][indFrameReading];
			}

			// management of brush radius (w/wo possible interpolation)
			if (alpha > 0) {
				tracks_RadiusX[indTrack] = (float)Lerp((float)TrackRadiusX_source[indTrack][indFrameReading],
					(float)TrackRadiusX_target[indTrack][indFrameReading], alpha);
				tracks_RadiusY[indTrack] = (float)Lerp((float)TrackRadiusY_source[indTrack][indFrameReading],
					(float)TrackRadiusY_target[indTrack][indFrameReading], alpha);
			}
			else {
				tracks_RadiusX[indTrack] = (float)TrackRadiusX_source[indTrack][indFrameReading];
				tracks_RadiusY[indTrack] = (float)TrackRadiusY_source[indTrack][indFrameReading];
			}

			// if( TrackRadiusX_source[ indTrack ][ indFrameReading ] == 0.0 ) 
			// 	printf( "replay %.d %.d brush %d radius %f %f (F%d)\n" , 
			// 		TrackPos_x_source[ indTrack ][ indFrameReading ] , TrackPos_y_source[ indTrack ][ indFrameReading ] , TrackBrushID_source[ indTrack ][ indFrameReading ] , TrackRadiusX_source[ indTrack ][ indFrameReading ] , TrackRadiusY_source[ indTrack ][ indFrameReading ] , indFrameReading );

			// interpolation time function management
			// interpolation is frozen if no drawing takes place
			if (TrackStatus_source[indTrack].isActiveInterpolation) {
				// continuous interpolation
				if (!TrackStatus_source[indTrack].isStepwiseInterpolation) {
					TrackStatus_source[indTrack].coefInterpolation +=
						TrackStatus_source[indTrack].interpolationStep;
				}
				// interpolation after each loop
				else if (isEndOfLoop) {
					TrackStatus_source[indTrack].coefInterpolation
						+= TrackStatus_source[indTrack].interpolationStep
						* TrackStatus_source[indTrack].nbRecordedFrames
						/ TrackStatus_source[indTrack].readSpeedScale;
					alpha = TrackStatus_source[indTrack].coefInterpolation;
					tracks_x_prev[indTrack] = (float)Lerp((float)TrackPos_x_source[indTrack][0],
						(float)TrackPos_x_target[indTrack][0], alpha);
					tracks_y_prev[indTrack] = (float)Lerp((float)TrackPos_y_source[indTrack][0],
						(float)TrackPos_y_target[indTrack][0], alpha);
					tracks_x[indTrack] = (float)Lerp((float)TrackPos_x_source[indTrack][0],
						(float)TrackPos_x_target[indTrack][0], alpha);
					tracks_y[indTrack] = (float)Lerp((float)TrackPos_y_source[indTrack][0],
						(float)TrackPos_y_target[indTrack][0], alpha);
				}
			}
			if (TrackStatus_source[indTrack].coefInterpolation >= 1.0) {
				TrackStatus_source[indTrack].coefInterpolation = 0.0;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	// JUST RENDERING WITHOUT READING A TRACK (IF NOT ACTIVE TRACK READING)
	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// draws from mouse position if the current track is not replayed
	bool is_trackreplay = false;
	switch (0) {
	case 0:
		is_trackreplay = track_replay_0;
		break;
#if PG_NB_TRACKS >= 2
	case 1:
		is_trackreplay = track_replay_1;
		break;
#endif
#if PG_NB_TRACKS >= 3
	case 2:
		is_trackreplay = track_replay_2;
		break;
#endif
	}
	if (!is_trackreplay) {
		tracks_Color_r[0] = local_color_gray
			+ RGBpalette_received[0] * (RGBpalette_val_coef + RGBpalette_val_const);
		tracks_Color_g[0] = local_color_gray
			+ RGBpalette_received[1] * (RGBpalette_val_coef + RGBpalette_val_const);
		tracks_Color_b[0] = local_color_gray
			+ RGBpalette_received[2] * (RGBpalette_val_coef + RGBpalette_val_const);
		tracks_Color_a[0] = 1.0f + RGBpalette_received[3];
		//printf("Track 0 color %.2f %.2f %.2f\n",
		//	tracks_Color_r[0], tracks_Color_g[0], tracks_Color_b[0]);
		if ((mouse_x_prev != CurrentMousePos_x
			|| mouse_y_prev != CurrentMousePos_y
			// || isAMove || wasAMove 
			)
			&& isDrawingOn) {
			// printf("mouse pos %dx%d %d\n",CurrentMousePos_x,CurrentMousePos_y,0);
			tracks_BrushID[0] = 0;
#ifdef PG_WACOM_TABLET
			tracks_RadiusX[0]
				= brushRadius +
				+tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * brush_radius_angleHor_coef;
			tracks_RadiusY[0]
				= brushRadius
				+ tabletPressureRadius * brush_radius_pressure_coef
				+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * brush_radius_angleVer_coef;
			//printf("track %d brushRadius %.2f\n" ,
			//  0 ,brushRadius );

#else
			tracks_RadiusX[0] = brushRadius;
			tracks_RadiusY[0] = brushRadius;
#endif
			// printf("Track %d brush ID radius %d %.2f\n" , 0, 0, brushRadius );
		}
	}

}


//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A SVG FILE
//////////////////////////////////////////////////////////////////
void load_svg_track( char *fileName , int indTrack , bool is_source_track ) {
  if( indTrack >= 0 
      && indTrack < PG_NB_TRACKS ) {
    track_replay_0 = false;
    source_track_record_0 = false;

	tracks_x[ indTrack ] = -1.0F;
    tracks_y[ indTrack ] = -1.0F;
    tracks_x_prev[ indTrack ] = -1.0F;
    tracks_y_prev[ indTrack ] = -1.0F;
    tracks_Color_r[ indTrack ] = 0.0F;
    tracks_Color_g[ indTrack ] = 0.0F;
    tracks_Color_b[ indTrack ] = 0.0F;
    tracks_Color_a[ indTrack ] = 1.0F;
    tracks_RadiusX[ indTrack ] = 0.0F;
    tracks_RadiusY[ indTrack ] = 0.0F;
    // printf( "-> stop_read_source_track\n" );
    
    // loads track
    int indDepth = 0;
    readsvg( &indDepth , indTrack , fileName , is_source_track );
  }
}

void readsvg(int *fileDepth, int indTrack, char *fileName,
	bool isSource) {
	string         val;
	float          translation[2] = { 0.0 , 0.0 };
	string line;

	std::ifstream fin(fileName);
	if (!fin) {
		sprintf(ErrorStr, "Error: file [%s] not found!", fileName); ReportError(ErrorStr); throw 11;
	}
	printf("Loading %s\n", fileName);

	// scans the svg file
	while (true) {
		std::getline(fin, line);
		if (fin.eof()) {
			break;
		}

		////////////////////////////
		////// group transformation tag
		std::size_t found = line.find("translate");
		if (found != std::string::npos) {
			found = line.find("(", found + 1);
			if (found != std::string::npos) {
				std::stringstream  sstream(line);
				sstream.seekg(found + 1);
				if (found != std::string::npos) {
					sstream >> translation[0];
					sstream.seekg(1, std::ios::cur);
					sstream >> translation[1];
				}
			}
		}
		////////////////////////////
		////// path tag
		else {
			found = line.find("<path");
			if (found != std::string::npos) {
				found = line.find("d=\"", found + 1);
				while (found == std::string::npos && !fin.eof()) {
					std::getline(fin, line);
					found = line.find("d=\"", found + 1);
				}
				if (found != std::string::npos) {
					// copies the beginning of the path
					val = line.substr(found + 3);

					std::getline(fin, line);
					found = line.find("z\"", found + 1);
					std::size_t found2 = line.find("Z\"", found + 1);
					while (found == std::string::npos && found2 == std::string::npos && !fin.eof()) {
						val += line;

						std::getline(fin, line);
						found = line.find("z\"", found + 1);
						// std::size_t found2 = line.find("Z\"",found+1);
					}

					// printf( "path values [%s]\n", (char *)val.c_str() );
					LoadTrackFromPath((char *)val.c_str(), indTrack, translation, isSource);
					for (int i = 0; i < 100; i++) {
						// printf("track %d %f %f\n" , i ,
						// 	 TrackPos_x_source[ 0 ][ i ] , 
						// 	 TrackPos_y_source[ 0 ][ i ]);
					}
				}
			}
		}
	}
	val.clear();
	fin.close();
}


//////////////////////////////////////////////////////////////////
// WRITES TRACKS TO A SVG FILE
//////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD WINAPI writesvg( LPVOID lpParam ) {
#else
void* writesvg(void * lpParam) {
#endif
  FILE          *fileSVG;
  threadWriteImageData * pDataArray = (threadWriteImageData *)lpParam;
  
  fileSVG = fopen( pDataArray->fname , "wb" );
  if ( fileSVG == NULL ) {
    sprintf( ErrorStr , "File %s not opened!" , pDataArray->fname ); ReportError( ErrorStr ); throw 152;
  }

  fprintf( fileSVG , "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\n<svg\n   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n   xmlns:cc=\"http://creativecommons.org/ns#\"\n   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n   xmlns:svg=\"http://www.w3.org/2000/svg\"\n   xmlns=\"http://www.w3.org/2000/svg\"\n   version=\"1.1\"\n   width=\"%d\"\n   height=\"%d\"\n   id=\"svg2\">\n  <defs\n     id=\"defs4\" />\n  <metadata\n     id=\"metadata7\">\n    <rdf:RDF>\n      <cc:Work\n         rdf:about=\"\">\n        <dc:format>image/svg+xml</dc:format>\n        <dc:type\n           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n        <dc:title></dc:title>\n      </cc:Work>\n    </rdf:RDF>\n  </metadata>\n  <g\n     transform=\"translate(0.0,0.0)\"\n     id=\"layer1\">\n",pDataArray->w, pDataArray->h );

  // for all tracks
  for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
    // main track
    if( TrackStatus_source[ indTrack ].nbRecordedFrames > 0 ) {
      fprintf( fileSVG , "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n       d=\"M %f,%f " , 
	       TrackStatus_source[ indTrack ].initialTimeRecording ,
	       TrackStatus_source[ indTrack ].initialTimeReading ,
	       TrackPos_x_source[ indTrack ][ 0 ] , 
	       TrackPos_y_source[ indTrack ][ 0 ] );
      for( int indFrame = 1 ; 
	   indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; 
	   indFrame++ ) {
	// skips identical frame
	// if( TrackPos_x_source[ indTrack ][ indFrame ] 
	//     == TrackPos_x_source[ indTrack ][ indFrame - 1 ] 
	//     && TrackPos_x_source[ indTrack ][ indFrame ] 
	//     == TrackPos_x_source[ indTrack ][ indFrame - 1 ] ) {
	//   continue;
	// }
	// move point with a new curve
	if( TrackPos_xR_source[ indTrack ][ indFrame - 1 ] == 0.0 
	    && TrackPos_yR_source[ indTrack ][ indFrame - 1 ] == 0.0 
	    && TrackPos_xL_source[ indTrack ][ indFrame ] == 0.0 
	    && TrackPos_yL_source[ indTrack ][ indFrame ] == 0.0 ) {
	  fprintf( fileSVG , "M %f,%f " , 
		   TrackPos_x_source[ indTrack ][ indFrame ] , 
		   TrackPos_y_source[ indTrack ][ indFrame ]);
	}
	// curve point
	else {
	  fprintf( fileSVG , "C %f,%f " , 
		   TrackPos_xR_source[ indTrack ][ indFrame - 1] , 
		   TrackPos_yR_source[ indTrack ][ indFrame - 1]);
	  // the right tangent of the last point on curve should be computed
	  // as the half way between right control of preceding point and current point
	  if( indFrame == TrackStatus_source[ indTrack ].nbRecordedFrames - 1 ) {
	    TrackPos_xR_source[ indTrack ][ indFrame ] = TrackPos_x_source[ indTrack ][ indFrame ] 
	      + 0.5F * ( TrackPos_xR_source[ indTrack ][ indFrame - 1 ] - TrackPos_x_source[ indTrack ][ indFrame ]); 
	    TrackPos_yR_source[ indTrack ][ indFrame ] = TrackPos_y_source[ indTrack ][ indFrame ] 
	      + 0.5F * ( TrackPos_yR_source[ indTrack ][ indFrame - 1 ] - TrackPos_y_source[ indTrack ][ indFrame ]); 
	  }
	  // the last point has a null left tangent
	  if( indFrame == TrackStatus_source[ indTrack ].nbRecordedFrames - 1 ) {
	    fprintf( fileSVG , "%f,%f " , 
		     TrackPos_x_source[ indTrack ][ indFrame ] , 
		     TrackPos_y_source[ indTrack ][ indFrame ]);
	  }
	  else {
	    fprintf( fileSVG , "%f,%f " , 
		     TrackPos_xL_source[ indTrack ][ indFrame ] , 
		     TrackPos_yL_source[ indTrack ][ indFrame ]);
	  }
	  fprintf( fileSVG , "%f,%f " , 
		   TrackPos_x_source[ indTrack ][ indFrame ] , 
		   TrackPos_y_source[ indTrack ][ indFrame ]);
	}
      }
      fprintf( fileSVG , "\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1\" />\n" , indTrack );
    }
    if( TrackStatus_target[ indTrack ].nbRecordedFrames > 0 ) {
      // target track
      fprintf( fileSVG , "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n       d=\"M %f,%f C " , 
    	       TrackStatus_target[ indTrack ].initialTimeRecording ,
    	       TrackStatus_target[ indTrack ].initialTimeReading ,
    	       TrackPos_x_target[ indTrack ][ 0 ] , 
    	       TrackPos_y_target[ indTrack ][ 0 ]);
      for( int indFrame = 1 ; 
    	   indFrame < TrackStatus_target[ indTrack ].nbRecordedFrames ; 
    	   indFrame++ ) {
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_xR_target[ indTrack ][ indFrame - 1 ] , 
    		 TrackPos_yR_target[ indTrack ][ indFrame - 1 ]);
    	// the right tangent of the last point on curve should be computed
    	// as the half way between right control of preceding point and current point
    	if( indFrame == TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ) {
    	  TrackPos_xR_target[ indTrack ][ indFrame ] = TrackPos_x_target[ indTrack ][ indFrame ] 
    	    + 0.5F * ( TrackPos_xR_target[ indTrack ][ indFrame - 1 ] - TrackPos_x_target[ indTrack ][ indFrame ]); 
    	  TrackPos_yR_target[ indTrack ][ indFrame ] = TrackPos_y_target[ indTrack ][ indFrame ] 
    	    + 0.5F * ( TrackPos_yR_target[ indTrack ][ indFrame - 1 ] - TrackPos_y_target[ indTrack ][ indFrame ]); 
    	}
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_xL_target[ indTrack ][ indFrame ] , 
    		 TrackPos_yL_target[ indTrack ][ indFrame ]);
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_x_target[ indTrack ][ indFrame ] , 
    		 TrackPos_y_target[ indTrack ][ indFrame ]);
      }
      fprintf( fileSVG , "z\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1\" />\n", indTrack + PG_NB_TRACKS );
    }
  }
  fprintf( fileSVG , "  </g>\n</svg>\n" );

  fclose( fileSVG );

  delete pDataArray->fname;
  delete pDataArray;

  return 0;
}


//////////////////////////////////////////////////////////////////
// WRITES TRACKS TO TEXTURES FOR DRAWING A LINE OR A STRIP
//////////////////////////////////////////////////////////////////
void pg_writeTrackTextures( int indTrack , bool is_target ) {
  printOglError( 11 );

  // source track
  if( !is_target && TrackStatus_source[ indTrack ].nbRecordedFrames > 0 ) {
    int indTex = 0;
    for( int indFrame = 0 ; 
	 indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; 
	 indFrame++ ) {
      // skips identical frame
      // if( TrackPos_x_source[ indTrack ][ indFrame ] 
      //     == TrackPos_x_source[ indTrack ][ indFrame - 1 ] 
      //     && TrackPos_x_source[ indTrack ][ indFrame ] 
      //     == TrackPos_x_source[ indTrack ][ indFrame - 1 ] ) {
      //   continue;
      // }
      // move point with a new curve
      pg_tracks_Pos_Texture[indTrack][ indTex ] = TrackPos_x_source[ indTrack ][ indFrame ];
      pg_tracks_Pos_Texture[indTrack][ indTex + 1 ] = TrackPos_y_source[ indTrack ][ indFrame ];
      pg_tracks_Pos_Texture[indTrack][ indTex + 2 ] = 0.1f + indTrack * 0.1f;
      if( TrackPos_xR_source[ indTrack ][ indFrame - 1 ] == 0.0 
	  && TrackPos_yR_source[ indTrack ][ indFrame - 1 ] == 0.0
	  && TrackPos_xL_source[ indTrack ][ indFrame ] == 0.0 
	  && TrackPos_yL_source[ indTrack ][ indFrame ] == 0.0 ) {
        pg_tracks_Pos_Texture[indTrack][ indTex + 3 ] = -1.0f;
      }
      // curve point
      else 
	{
        pg_tracks_Pos_Texture[indTrack][ indTex + 3 ] = 1.0f;
      }
      // if( indTex/4 < 5 ) {
      // 	printf("src curve to %d %f %f (%f)\n" , indFrame ,
      // 	       pg_tracks_Pos_Texture[ indTrack ][ indTex ] , 
      // 	       pg_tracks_Pos_Texture[ indTrack ][ indTex + 1 ], 
      // 	       pg_tracks_Pos_Texture[ indTrack ][ indTex + 3 ]);
      // }
      indTex += 4;
    }
    indTex = 0;
    for( int indFrame = 0 ; 
	 indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; 
	 indFrame++ ) {
        pg_tracks_Col_Texture[indTrack][ indTex ] = 1.0;//TrackColor_r_source[ indTrack ][ indFrame ];
        pg_tracks_Col_Texture[indTrack][ indTex + 1 ] = 1.0;//TrackColor_g_source[ indTrack ][ indFrame ];
        pg_tracks_Col_Texture[indTrack][ indTex + 2 ] = 1.0;//TrackColor_b_source[ indTrack ][ indFrame ];
        pg_tracks_Col_Texture[indTrack][ indTex + 3 ] = 1.0;//TrackColor_a_source[ indTrack ][ indFrame ];
	indTex += 4;
    }
    indTex = 0;
    for( int indFrame = 0 ; 
	 indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; 
	 indFrame++ ) {
        pg_tracks_RadBrushRendmode_Texture[indTrack][ indTex ] = TrackRadiusX_source[ indTrack ][ indFrame ];
        pg_tracks_RadBrushRendmode_Texture[indTrack][ indTex + 1 ] = (float)TrackBrushID_source[ indTrack ][ indFrame ];
        pg_tracks_RadBrushRendmode_Texture[indTrack][ indTex + 2 ] = 1.0; // line
        pg_tracks_RadBrushRendmode_Texture[indTrack][ indTex + 3 ] = 0.0; 
	indTex += 4;
    }
   
    glEnable( GL_TEXTURE_1D );
    glBindTexture( GL_TEXTURE_1D, pg_tracks_Pos_Texture_texID[indTrack] );
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexImage1D(GL_TEXTURE_1D,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA32F,            // Components: Internal colour format to convert to
		 max_mouse_recording_frames ,          // Image width
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_FLOAT,  // Image data type
		 (GLvoid *)pg_tracks_Pos_Texture[indTrack] );        // The actual image data itself

    glEnable( GL_TEXTURE_1D );
    glBindTexture( GL_TEXTURE_1D, pg_tracks_Col_Texture_texID[indTrack] );
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexImage1D(GL_TEXTURE_1D,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA32F,            // Components: Internal colour format to convert to
		 max_mouse_recording_frames ,          // Image width
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_FLOAT,  // Image data type
		 (GLvoid *)pg_tracks_Col_Texture[indTrack] );        // The actual image data itself

    glEnable( GL_TEXTURE_1D );
    glBindTexture( GL_TEXTURE_1D, pg_tracks_RadBrushRendmode_Texture_texID[indTrack] );
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
    glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexImage1D(GL_TEXTURE_1D,     // Type of texture
		 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		 GL_RGBA32F,            // Components: Internal colour format to convert to
		 max_mouse_recording_frames ,          // Image width
		 0,                 // Border width in pixels (can either be 1 or 0)
		 GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		 GL_FLOAT,  // Image data type
		 (GLvoid *)pg_tracks_RadBrushRendmode_Texture[indTrack] );        // The actual image data itself

    printf("Position texture values initialized, track %d: size %d, text size %dx%d\n" , indTrack , TrackStatus_source[ indTrack ].nbRecordedFrames ,
	max_mouse_recording_frames , 1 );
  }
  // target track
  if( is_target && TrackStatus_target[ indTrack ].nbRecordedFrames > 0 ) {
    // TODO
  }

  printOglError( 12 );
}


