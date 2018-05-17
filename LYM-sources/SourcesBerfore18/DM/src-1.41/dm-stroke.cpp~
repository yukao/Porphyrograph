/*! \file dm-main.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 *
 *     File dm-main.cpp
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

#include "dm-all_include.h"

// scene root node
dm_SceneNode *rootNode = NULL;

// mouse recording tracks management
dm_Table *table_local_color = NULL;
dm_Table *table_color_spectrum = NULL;
float **TrackColor_r = NULL;
float **TrackColor_g = NULL;
float **TrackColor_b = NULL;
float **TrackColor_a = NULL;
float **TrackColor_r_target = NULL;
float **TrackColor_g_target = NULL;
float **TrackColor_b_target = NULL;
float **TrackColor_a_target = NULL;
float **TrackColor_r_temp_target = NULL;
float **TrackColor_g_temp_target = NULL;
float **TrackColor_b_temp_target = NULL;
float **TrackColor_a_temp_target = NULL;

dm_Scalar *scalar_drawingMode = NULL;
bool isDrawingOn = false;
bool isAMove = false;

dm_Scalar *scalar_brushID = NULL;
int **TrackBrushID = NULL;
int **TrackBrushID_target = NULL;
int **TrackBrushID_temp_target = NULL;

dm_Scalar *scalar_brushRadius = NULL;
dm_Scalar *scalar_brush_radius_vol = NULL;
float **TrackRadius = NULL;
float **TrackRadius_target = NULL;
float **TrackRadius_temp_target = NULL;

dm_Scalar *scalar_currentTrack = NULL;

float **TrackPos_x = NULL;
float **TrackPos_y = NULL;
float **TrackPos_xL = NULL;
float **TrackPos_yL = NULL;
float **TrackPos_xR = NULL;
float **TrackPos_yR = NULL;
double **TrackTime = NULL;
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
struct TrackStatus *TrackStatus = NULL;
struct TrackStatus *TrackStatus_target = NULL;

// tension of catmul-rom spline: currently, the standard value 1/6
// could be a parameter in the future
float Tension = 1.0F/6.0F;

////////////////////////////////////////////////////////////////
// SPLINES
////////////////////////////////////////////////////////////////
#define Max(x,y)  ((x>y)?x:y)
#define Min(x,y)  ((y>x)?x:y)
#define Lerp(x,y,s)  (x*(1-s) + y*s)

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
void InitStrokes( void ) {
  // looks for specific variables (brush radius, local color, brush
  // ID...) that can be used to access values for stroke loop
  // memorization
  int nbRefs = 0;
  dm_SceneNode **auxNode 
    = findReferencesDynamic( "#root" , &nbRefs , true , true );
  
  if( auxNode ) {
    // allocates a list of target nodes that
    // will be instantiated by the actual pointers in
    // the scene graph
    rootNode = auxNode[ 0 ];
    // printf( "ReferenceNodeAttributeValue %s\n" , rootNode->id );
    
    // throws away the nodes that were allocated during
    // the resolution of the reference
    delete [] auxNode;
    auxNode = NULL;
    
    scalar_drawingMode = rootNode->getScalarFromName( "drawingMode" );
    if( scalar_drawingMode == NULL ) {
      sprintf( ErrorStr , "Root node scalar drawingMode not found!" ); ReportError( ErrorStr ); throw 207;
    }

    scalar_currentTrack = rootNode->getScalarFromName( "currentTrack" );

    scalar_brushID = rootNode->getScalarFromName( "brushID" );
    if( scalar_brushID == NULL ) {
      sprintf( ErrorStr , "Root node scalar brushID not found!" ); ReportError( ErrorStr ); throw 207;
    }
    scalar_brushRadius = rootNode->getScalarFromName( "brushRadius" );
    if( scalar_brushRadius == NULL ) {
      sprintf( ErrorStr , "Root node scalar brushRadius not found!" ); ReportError( ErrorStr ); throw 207;
    }
    scalar_brush_radius_vol = rootNode->getScalarFromName( "brush_radius_vol" );
    if( scalar_brush_radius_vol == NULL ) {
      sprintf( ErrorStr , "Root node scalar brush_radius_vol found!" ); ReportError( ErrorStr ); throw 207;
    }
    table_local_color = rootNode->getTableFromName( "local_color" );
    if( table_local_color == NULL ) {
      sprintf( ErrorStr , "Root node table local_color not found!" ); ReportError( ErrorStr ); throw 207;
    }
    table_color_spectrum = rootNode->getTableFromName( "color_spectrum" );
    if(table_color_spectrum == NULL ) {
      sprintf( ErrorStr , "Root node table color_spectrum not found!" ); ReportError( ErrorStr ); throw 207;
    }
  }
  else {
    sprintf( ErrorStr , "Root node not found!" ); ReportError( ErrorStr ); throw 207;
  }

  // mouse pointer tracks recording initialization
  TrackColor_r = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_g = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_b = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_a = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackBrushID = new int*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackRadius = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_x = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_y = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_xL = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_yL = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_xR = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_yR = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackTime = new double*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackStatus = new struct TrackStatus[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_r_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_g_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_b_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_a_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackBrushID_target = new int*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackRadius_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_x_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_y_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_xL_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_yL_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_xR_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_yR_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackTime_target = new double*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_r_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_g_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_b_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackColor_a_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackBrushID_temp_target = new int*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackRadius_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_x_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackPos_y_temp_target = new float*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackTime_temp_target = new double*[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  TrackStatus_target = new struct TrackStatus[ (DM_EnvironmentNode->Nb_max_tracks + 1) ];
  for( int ind = 0 ; ind < (DM_EnvironmentNode->Nb_max_tracks + 1) ; ind++ ) {
    TrackColor_r[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_g[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_b[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_a[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackBrushID[ ind ] 
      = new int[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackRadius[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_x[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_y[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_xL[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_yL[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_xR[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_yR[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackTime[ ind ] 
      = new double[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackStatus[ ind ].isFirstFrame = false;
    TrackStatus[ ind ].isActiveInterpolation = false;
    TrackStatus[ ind ].isActiveRecording = false;
    TrackStatus[ ind ].isNormalized = false;
    TrackStatus[ ind ].nbRecordedFrames = 0;
    TrackStatus[ ind ].indReading = 0;
    TrackStatus[ ind ].coefInterpolation = 0;
    TrackStatus[ ind ].initialTimeRecording = 0.0;
    TrackStatus[ ind ].initialTimeReading = 0.0;
    TrackStatus[ ind ].isStepwiseInterpolation = false;
    TrackStatus[ ind ].interpolationStep = 0.00001F;
    TrackStatus[ ind ].readSpeedScale = 1.0F;
    TrackColor_r_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_g_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_b_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_a_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackBrushID_target[ ind ] 
      = new int[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackRadius_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_x_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_y_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_xL_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_yL_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_xR_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_yR_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackTime_target[ ind ] 
      = new double[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_r_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_g_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_b_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackColor_a_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackBrushID_temp_target[ ind ] 
      = new int[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackRadius_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_x_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackPos_y_temp_target[ ind ] 
      = new float[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
    TrackTime_temp_target[ ind ] 
      = new double[ DM_EnvironmentNode->Nb_max_mouse_recording_frames ];
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
  if( TrackStatus[ indTrack ].nbRecordedFrames <= 4 
      || TrackStatus_target[ indTrack ].nbRecordedFrames <= 4 ) {
    TrackStatus[ indTrack ].isActiveInterpolation = false;
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
    TrackRadius_temp_target[ indTrack ][ indFrameRec ] = TrackRadius_target[ indTrack ][ indFrameRec ]; 
    TrackPos_x_temp_target[ indTrack ][ indFrameRec ] = TrackPos_x_target[ indTrack ][ indFrameRec ]; 
    TrackPos_y_temp_target[ indTrack ][ indFrameRec ] = TrackPos_y_target[ indTrack ][ indFrameRec ]; 
    TrackTime_temp_target[ indTrack ][ indFrameRec ] = TrackTime_target[ indTrack ][ indFrameRec ];
  }

  double sourceDuration = TrackTime[ indTrack ][ TrackStatus[ indTrack ].nbRecordedFrames - 1 ]
    - TrackStatus[ indTrack ].initialTimeRecording;
  double targetDuration = TrackTime_target[ indTrack ][ TrackStatus_target[ indTrack ].nbRecordedFrames - 1 ]
    - TrackStatus_target[ indTrack ].initialTimeRecording;
  double timeRatio = sourceDuration/targetDuration;

  // makes a copy of the initially recorded target track so that it can be normalized
  // and overwritten in the target track values

  // copies first frame of both
  // copies the time of the source frame
  // copies the current time in the current frame
  TrackTime_target[ indTrack ][ 0 ] = TrackTime[ indTrack ][ 0 ];

  // copies the data of the temporary stored target frame
  TrackColor_r_target[ indTrack ][ 0 ] = TrackColor_r_temp_target[ indTrack ][ 0 ]; 
  TrackColor_g_target[ indTrack ][ 0 ] = TrackColor_g_temp_target[ indTrack ][ 0 ]; 
  TrackColor_b_target[ indTrack ][ 0 ] = TrackColor_b_temp_target[ indTrack ][ 0 ]; 
  TrackColor_a_target[ indTrack ][ 0 ] = TrackColor_a_temp_target[ indTrack ][ 0 ]; 
  TrackBrushID_target[ indTrack ][ 0 ] = TrackBrushID_temp_target[ indTrack ][ 0 ]; 
  TrackRadius_target[ indTrack ][ 0 ] = TrackRadius_temp_target[ indTrack ][ 0 ]; 
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
      indSourceFrame < TrackStatus[ indTrack ].nbRecordedFrames - 1; 
      indSourceFrame++ ) {
    // source time of the current source frame
    sourceElapsedTime = TrackTime[ indTrack ][ indSourceFrame ] 
      - TrackStatus[ indTrack ].initialTimeRecording;

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
	     indSourceFrame , indTargetFrame ,TrackStatus[ indTrack ].nbRecordedFrames , TrackStatus_target[ indTrack ].nbRecordedFrames);
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
    TrackRadius_target[ indTrack ][ indSourceFrame ]
      = Lerp( TrackRadius_temp_target[ indTrack ][ indTargetFrame ] , 
	      TrackRadius_temp_target[ indTrack ][ indTargetFrame + 1 ] , interpolationStepf );

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
      = TrackTime[ indTrack ][ indSourceFrame ];
  }

  // copies last frame of both
  int indSourceFrame = TrackStatus[ indTrack ].nbRecordedFrames - 1;
  indTargetFrame = TrackStatus_target[ indTrack ].nbRecordedFrames - 1;
  // copies the final time and number of frames of the source track
  TrackStatus_target[ indTrack ].initialTimeRecording
    = TrackStatus[ indTrack ].initialTimeRecording;
  TrackStatus_target[ indTrack ].nbRecordedFrames
    = TrackStatus[ indTrack ].nbRecordedFrames;

  // copies the current time in the current frame
  TrackTime_target[ indTrack ][ indSourceFrame ]
    = TrackTime[ indTrack ][ indSourceFrame ];

  // copies the data of the temporary stored target frame
  TrackColor_r_target[ indTrack ][ indSourceFrame ] = TrackColor_r_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_g_target[ indTrack ][ indSourceFrame ] = TrackColor_g_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_b_target[ indTrack ][ indSourceFrame ] = TrackColor_b_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackColor_a_target[ indTrack ][ indSourceFrame ] = TrackColor_a_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackBrushID_target[ indTrack ][ indSourceFrame ] = TrackBrushID_temp_target[ indTrack ][ indTargetFrame ]; 
  TrackRadius_target[ indTrack ][ indSourceFrame ] = TrackRadius_temp_target[ indTrack ][ indTargetFrame ]; 
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
    TrackStatus[ indTrack ].isFirstFrame = false;
    TrackStatus[ indTrack ].isActiveInterpolation = false;
    TrackStatus[ indTrack ].isActiveRecording = false;
    TrackStatus[ indTrack ].isNormalized = false;
    TrackStatus[ indTrack ].nbRecordedFrames = 0;
    TrackStatus[ indTrack ].indReading = 0;
    TrackStatus[ indTrack ].coefInterpolation = 0;
    TrackStatus[ indTrack ].initialTimeRecording = 0.0;
    TrackStatus[ indTrack ].initialTimeReading = 0.0;
    TrackStatus[ indTrack ].isStepwiseInterpolation = false;
    TrackStatus[ indTrack ].interpolationStep = 0.00001F;
    TrackStatus[ indTrack ].readSpeedScale = 1.0F;
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

  SpaceCommentsInclude( NULL , &curChar , pathString , &indChar );
  while( indChar <= (int)strlen( pathString ) && curChar != EOF ) {
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
	ExpectsChar( NULL , curChar , &curChar , true , 
		     pathString , &indChar );
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative  && indSpline != 0 )
	    currentPoint[0] += ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false );
	  else
	    currentPoint[0] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  ExpectsChar( NULL , ',' , &curChar , true , 
		       pathString , &indChar );
	  if( relative  && indSpline != 0 )
	    currentPoint[1] += ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false );
	  else
	    currentPoint[1] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  SpaceCommentsInclude( NULL , &curChar , pathString , &indChar );
	  nbMovePoints++;
	  if( indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackColor_r[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID[ indTrack ][ indSpline ] = 0;
	      TrackRadius[ indTrack ][ indSpline ] = 0.0;
	      TrackPos_x[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("src move to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y[ indTrack ][ indSpline ]);
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadius_target[ indTrack ][ indSpline ] = 0.0;
	      TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("tgt move to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y[ indTrack ][ indSpline ]);
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
	ExpectsChar( NULL , curChar , &curChar , true , 
		     pathString , &indChar );

	curveInitialPoint[0] = currentPoint[0];
	curveInitialPoint[1] = currentPoint[1];
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 
	  if( relative )
	    currentPoint[0] = ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false ) + curveInitialPoint[0];
	  else
	    currentPoint[0] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  ExpectsChar( NULL , ',' , &curChar , true , 
		       pathString , &indChar );
	  if( relative )
	    currentPoint[1] = ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false ) + curveInitialPoint[1];
	  else
	    currentPoint[1] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  SpaceCommentsInclude( NULL , &curChar , pathString , &indChar );
	  nbCurvePoints++;

	  if( (nbCurvePoints) % 3 == 0  && indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {	    
	    if( isSource ) {
	      TrackColor_r[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID[ indTrack ][ indSpline ] = 0;
	      TrackRadius[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      // printf("src curve to %d %f %f\n" , indSpline ,
	      // 	     TrackPos_x[ indTrack ][ indSpline ] , 
	      // 	     TrackPos_y[ indTrack ][ indSpline ]);
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadius_target[ indTrack ][ indSpline ] = 3.0;
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
	  else if( (nbCurvePoints) % 3 == 1  && indSpline > 0 && indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackPos_xR[ indTrack ][ indSpline - 1 ] = currentPoint[0] + translation[0];
	      TrackPos_yR[ indTrack ][ indSpline - 1 ] = currentPoint[1] + translation[1];
	    }
	    else {
	    }
	      TrackPos_xR_target[ indTrack ][ indSpline - 1 ] = currentPoint[0] + translation[0];
	      TrackPos_yR_target[ indTrack ][ indSpline - 1 ] = currentPoint[1] + translation[1];
	  }
	  // left tangent of the second (and next) point
	  else if( (nbCurvePoints) % 3 == 2  && indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackPos_xL[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_yL[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
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
	ExpectsChar( NULL , curChar , &curChar , true , 
		     pathString , &indChar );

	if( indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	  if( isSource ) {
	    TrackColor_r[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID[ indTrack ][ indSpline ] = 0;
	    TrackRadius[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	    TrackPos_y[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    TrackTime[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  else {
	    TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	    TrackRadius_target[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_target[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	    TrackPos_y_target[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	    TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  // printf("line to %d %f %f\n" , indSpline ,
	  // 	 TrackPos_x[ indTrack ][ indSpline ] , 
	  // 	 TrackPos_y[ indTrack ][ indSpline ]);

	  indSpline++;
	}
	
	while( _Num(curChar) || curChar == '-' || curChar == '+' ) { 

	  if( relative )
	    currentPoint[0] += ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false );
	  else
	    currentPoint[0] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  ExpectsChar( NULL , ',' , &curChar , true , 
		       pathString , &indChar );
	  if( relative )
	    currentPoint[1] += ReadFloat( NULL , &curChar , true  , 
					 pathString , &indChar , false );
	  else
	    currentPoint[1] = ReadFloat( NULL , &curChar , true  , 
					  pathString , &indChar , false );
	  SpaceCommentsInclude( NULL , &curChar , pathString , &indChar );

	  if( indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	    if( isSource ) {
	      TrackColor_r[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID[ indTrack ][ indSpline ] = 0;
	      TrackRadius[ indTrack ][ indSpline ] = 3.0;
	      TrackPos_x[ indTrack ][ indSpline ] = currentPoint[0] + translation[0];
	      TrackPos_y[ indTrack ][ indSpline ] = currentPoint[1] + translation[1];
	      TrackTime[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	      
	      // left tangent of current point is preceding point
	      // and right tangent of preceding point is the current point
	      TrackPos_xL[ indTrack ][ indSpline ] 
		= TrackPos_x[ indTrack ][ indSpline - 1 ];
	      TrackPos_yL[ indTrack ][ indSpline ]
		= TrackPos_y[ indTrack ][ indSpline -1 ];
	      TrackPos_xR[ indTrack ][ indSpline - 1 ] 
		= TrackPos_x[ indTrack ][ indSpline ];
	      TrackPos_yR[ indTrack ][ indSpline - 1 ]
		= TrackPos_y[ indTrack ][ indSpline ];
	    }
	    else {
	      TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	      TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	      TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	      TrackRadius_target[ indTrack ][ indSpline ] = 3.0;
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
	    // 	   TrackPos_x[ indTrack ][ indSpline ] , 
	    // 	   TrackPos_y[ indTrack ][ indSpline ]);
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
	ExpectsChar( NULL , curChar , &curChar , true , 
		     pathString , &indChar );
	// printf("cut\n");
	if( indSpline < DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
	  if( isSource ) {
	    TrackColor_r[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID[ indTrack ][ indSpline ] = 0;
	    TrackRadius[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x[ indTrack ][ indSpline ] = polyCurveInitialPoint[0] + translation[0];
	    TrackPos_y[ indTrack ][ indSpline ] = polyCurveInitialPoint[1] + translation[1];
	    TrackTime[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }
	  else {
	    TrackColor_r_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_g_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_b_target[ indTrack ][ indSpline ] = 1.0;
	    TrackColor_a_target[ indTrack ][ indSpline ] = 1.0;
	    TrackBrushID_target[ indTrack ][ indSpline ] = 0;
	    TrackRadius_target[ indTrack ][ indSpline ] = 3.0;
	    TrackPos_x_target[ indTrack ][ indSpline ] = polyCurveInitialPoint[0] + translation[0];
	    TrackPos_y_target[ indTrack ][ indSpline ] = polyCurveInitialPoint[1] + translation[1];
	    TrackTime_target[ indTrack ][ indSpline ] = indSpline * interFrameDuration;
	  }

	  // printf("close curve %d %f %f\n" , indSpline ,
	  // 	 TrackPos_x[ indTrack ][ indSpline ] , 
	  // 	 TrackPos_y[ indTrack ][ indSpline ]);
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
    TrackStatus[ indTrack ].nbRecordedFrames =indSpline;
  }
  else {
    TrackStatus_target[ indTrack ].nbRecordedFrames =indSpline;
  }
}

//////////////////////////////////////////////////////////////////
// MEMORY STORAGE OF THE CURRENT STROKE  
//////////////////////////////////////////////////////////////////
// update of the tables that contain thecurrrentTrack stroke parameters
void updateMouseEnvironmentVariablesAndTables( double theTime ) {
  // the storage in the preceding mouse position variable is
  // made in the script of the scene XML
  // -1 is background layer
  int currentTrack = (scalar_currentTrack == NULL? -1 : scalar_currentTrack->GetValue()->valInt);

  // memory of current and preceding values
  DM_EnvironmentNode
    ->mouse_x_scalar->SetValue( (float)CurrentMousePos_x 
				+ (float)DM_EnvironmentNode
				->mouse_x_deviation_scalar->GetValue()->valFloat );
  DM_EnvironmentNode
    ->mouse_y_scalar->SetValue( (float)CurrentMousePos_y 
				+ (float)DM_EnvironmentNode
				->mouse_y_deviation_scalar->GetValue()->valFloat );

  // memory about drawing at preceding step
  bool wasDrawingOn = isDrawingOn;
  isDrawingOn = (scalar_drawingMode->GetValue()->valInt > 0);

  // move second step: the left tangent is null
  bool wasAMove = isAMove;
  // move first step: the right tangent is null
  isAMove = (isDrawingOn && !wasDrawingOn);

  ///////////////////////////////////////////////////////////////////////////
  // SOURCE AND TARGET TRACK RECORDING
  // no update if the mouse has not been moving
  // no update if the mode is not a drawing mode
  // does not concern the background because there is no track
  // recording for this layer
  if( currentTrack >= 0
      && (((float)DM_EnvironmentNode->mouse_x_prev_scalar->GetValue()->valFloat
	   != CurrentMousePos_x)
	  || ((float)DM_EnvironmentNode->mouse_y_prev_scalar->GetValue()->valFloat
	      != CurrentMousePos_y)
	  || isAMove || wasAMove )
      && isDrawingOn ) {

    ///////////////////////////////////////////////////////////////////////
    // RECORDING SOURCE TRACK
    ///////////////////////////////////////////////////////////////////////
    if( TrackStatus[ currentTrack ].isActiveRecording 
	&& TrackStatus[ currentTrack ].nbRecordedFrames 
	< DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
      int indFrameRec = TrackStatus[ currentTrack ].nbRecordedFrames;
      // printf( "rec track %d frame %d\n",currentTrack,indFrameRec );
	
      // records the initial time and the current time to play in synch
      if( indFrameRec == 0 ) {
	TrackStatus[ currentTrack ].initialTimeRecording = theTime;
      }
      TrackTime[ currentTrack ][ indFrameRec ] = theTime;
	
      TrackColor_r[ currentTrack ][ indFrameRec ] 
	= (float)table_local_color->GetNumericalValue( 0 )
	+ (float)table_color_spectrum->GetNumericalValue( 0 );
      TrackColor_g[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 1 )
	+ (float)table_color_spectrum->GetNumericalValue( 1 );
      TrackColor_b[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 2 )
	+ (float)table_color_spectrum->GetNumericalValue( 2 );
      TrackColor_a[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 3 );
      // printf( "capture %.2f %.2f %.2f %.2f\n" , 
      // 	      TrackColor_r[ currentTrack ][ indFrameRec ] ,
      // 	      TrackColor_g[ currentTrack ][ indFrameRec ] ,
      // 	      TrackColor_b[ currentTrack ][ indFrameRec ] ,
      // 	      TrackColor_a[ currentTrack ][ indFrameRec ] );

      TrackBrushID[ currentTrack ][ indFrameRec ] 
	= (int)scalar_brushID->GetValue()->valInt;

      TrackRadius[ currentTrack ][ indFrameRec ] 
	= (float)scalar_brushRadius->GetValue()->valFloat
	+ (float)scalar_brush_radius_vol->GetValue()->valFloat;

      TrackPos_x[ currentTrack ][ indFrameRec ] = (float)CurrentMousePos_x;
      TrackPos_y[ currentTrack ][ indFrameRec ] = (float)CurrentMousePos_y;

      // Catmull-Rom spline: computes right and left tangent of the preceding point from 
      // position of current point and current point - 2
      if( indFrameRec > 1 ) {
	// sets tangent extremities to 0 in case of move for not redrawing the
	// jump later & for inserting a move in the saved svg
	float tan_x = Tension * ( TrackPos_x[ currentTrack ][ indFrameRec ] - TrackPos_x[ currentTrack ][ indFrameRec - 2 ]);
	float tan_y = Tension * ( TrackPos_y[ currentTrack ][ indFrameRec ] - TrackPos_y[ currentTrack ][ indFrameRec - 2 ]); 
	// move second step: null left tangent of current point
	if( !wasAMove ) {
	  TrackPos_xL[ currentTrack ][ indFrameRec - 1 ] 
	    = TrackPos_x[ currentTrack ][ indFrameRec - 1 ] - tan_x;
	  TrackPos_yL[ currentTrack ][ indFrameRec - 1 ]  
	    = TrackPos_y[ currentTrack ][ indFrameRec - 1 ] - tan_y;
	}
	else {
	  TrackPos_xL[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  TrackPos_yL[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  // a null left tangent was givent at next frame
	}
	// move first step: null right tangent extremity of preceding point
	// so that it is saved as a move
	if( !isAMove ) {
	  if( !wasAMove ) {
	    TrackPos_xR[ currentTrack ][ indFrameRec - 1 ] 
	      = TrackPos_x[ currentTrack ][ indFrameRec - 1 ] + tan_x;
	    TrackPos_yR[ currentTrack ][ indFrameRec - 1 ]  
	      = TrackPos_y[ currentTrack ][ indFrameRec - 1 ] + tan_y;
	  }
	  else {
	    // first point in a new curve
	    TrackPos_xR[ currentTrack ][ indFrameRec - 1 ] 
	      = TrackPos_x[ currentTrack ][ indFrameRec - 1 ];
	    TrackPos_yR[ currentTrack ][ indFrameRec - 1 ]  
	      = TrackPos_y[ currentTrack ][ indFrameRec - 1 ];
	  }	      
	}
	else {
	  // assigns null values to right tangent extremity so that it is saved
	  // as a move
	  TrackPos_xR[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  TrackPos_yR[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  // a null left tangent extremity will be givent at next frame

	  // null left tangent at closing point before the move
	  TrackPos_xL[ currentTrack ][ indFrameRec - 1 ] = TrackPos_x[ currentTrack ][ indFrameRec - 1 ];
	  TrackPos_yL[ currentTrack ][ indFrameRec - 1 ] = TrackPos_y[ currentTrack ][ indFrameRec - 1 ];
	}

	// right tangent is half way between first point and left tangent extremity of second point
	if( indFrameRec == 2 ) {
	  TrackPos_xR[ currentTrack ][ 0 ] = TrackPos_x[ currentTrack ][ 0 ] 
	    + 0.5F * ( TrackPos_xL[ currentTrack ][ 1 ] - TrackPos_x[ currentTrack ][ 0 ]); 
	  TrackPos_yR[ currentTrack ][ 0 ] = TrackPos_y[ currentTrack ][ 0 ] 
	    + 0.5F * ( TrackPos_yL[ currentTrack ][ 1 ] - TrackPos_y[ currentTrack ][ 0 ]); 
	}
      }	
      // printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , currentTrack + 1 , indFrameRec - 1 , (int)TrackPos_xL[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_yL[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_xR[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_yR[ currentTrack ][ indFrameRec - 1 ] );

      TrackStatus[ currentTrack ].nbRecordedFrames++;
    }
    // RECORDING SOURCE TRACK

    ///////////////////////////////////////////////////////////////////////
    // RECORDING INTERPOLATION TARGET TRACK
    ///////////////////////////////////////////////////////////////////////
    if( TrackStatus_target[ currentTrack ].isActiveRecording 
	&& TrackStatus_target[ currentTrack ].nbRecordedFrames 
	< DM_EnvironmentNode->Nb_max_mouse_recording_frames ) {
      int indFrameRec = TrackStatus_target[ currentTrack ].nbRecordedFrames;
	
      // records the initial time and the current time to play in synch
      if( indFrameRec == 0 ) {
	TrackStatus_target[ currentTrack ].initialTimeRecording = theTime;
      }
      TrackTime_target[ currentTrack ][ indFrameRec ] = theTime;
      
      TrackColor_r_target[ currentTrack ][ indFrameRec ] 
	= (float)table_local_color->GetNumericalValue( 0 )
	+ (float)table_color_spectrum->GetNumericalValue( 0 );
      TrackColor_g_target[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 1 )
	+ (float)table_color_spectrum->GetNumericalValue( 1 );
      TrackColor_b_target[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 2 )
	+ (float)table_color_spectrum->GetNumericalValue( 2 );
      TrackColor_a_target[ currentTrack ][ indFrameRec ]
	= (float)table_local_color->GetNumericalValue( 3 );
      // printf( "capture %.2f %.2f %.2f\n" , 
      // 	      TrackColor_r_target[ currentTrack ][ indFrameRec ] ,
      // 	      TrackColor_g_target[ currentTrack ][ indFrameRec ] ,
      // 	      TrackColor_b_target[ currentTrack ][ indFrameRec ] );
      
      TrackBrushID_target[ currentTrack ][ indFrameRec ] 
	= (int)scalar_brushID->GetValue()->valInt;
      
      TrackRadius_target[ currentTrack ][ indFrameRec ] 
	= (float)scalar_brushRadius->GetValue()->valFloat
	+ (float)scalar_brush_radius_vol->GetValue()->valFloat;
      
      TrackPos_x_target[ currentTrack ][ indFrameRec ] = (float)CurrentMousePos_x;
      TrackPos_y_target[ currentTrack ][ indFrameRec ] = (float)CurrentMousePos_y;
      
      // Catmull-Rom spline: computes right and left tangent of the preceding point from 
      // position of current point and current point - 2
      if( indFrameRec > 1 ) {
	// sets tangent extremities to 0 in case of move for not redrawing the
	// jump later & for inserting a move in the saved svg
	float tan_x = Tension * ( TrackPos_x_target[ currentTrack ][ indFrameRec ] - TrackPos_x_target[ currentTrack ][ indFrameRec - 2 ]);
	float tan_y = Tension * ( TrackPos_y_target[ currentTrack ][ indFrameRec ] - TrackPos_y_target[ currentTrack ][ indFrameRec - 2 ]); 
	// move second step: null left tangent of current point
	if( !wasAMove ) {
	  TrackPos_xL_target[ currentTrack ][ indFrameRec - 1 ] 
	    = TrackPos_x_target[ currentTrack ][ indFrameRec - 1 ] - tan_x;
	  TrackPos_yL_target[ currentTrack ][ indFrameRec - 1 ]  
	    = TrackPos_y_target[ currentTrack ][ indFrameRec - 1 ] - tan_y;
	}
	else {
	  TrackPos_xL_target[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  TrackPos_yL_target[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  // a null left tangent was givent at next frame
	}
	// move first step: null right tangent extremity of preceding point
	// so that it is saved as a move
	if( !isAMove ) {
	  if( !wasAMove ) {
	    TrackPos_xR_target[ currentTrack ][ indFrameRec - 1 ] 
	      = TrackPos_x_target[ currentTrack ][ indFrameRec - 1 ] + tan_x;
	    TrackPos_yR_target[ currentTrack ][ indFrameRec - 1 ]  
	      = TrackPos_y_target[ currentTrack ][ indFrameRec - 1 ] + tan_y;
	  }
	  else {
	    // first point in a new curve
	    TrackPos_xR_target[ currentTrack ][ indFrameRec - 1 ] 
	      = TrackPos_x_target[ currentTrack ][ indFrameRec - 1 ];
	    TrackPos_yR_target[ currentTrack ][ indFrameRec - 1 ]  
	      = TrackPos_y_target[ currentTrack ][ indFrameRec - 1 ];
	  }	      
	}
	else {
	  // assigns null values to right tangent extremity so that it is saved
	  // as a move
	  TrackPos_xR_target[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  TrackPos_yR_target[ currentTrack ][ indFrameRec - 1 ] = 0.0;
	  // a null left tangent extremity will be givent at next frame
	  
	  // null left tangent at closing point before the move
	  TrackPos_xL_target[ currentTrack ][ indFrameRec - 1 ] = TrackPos_x_target[ currentTrack ][ indFrameRec - 1 ];
	  TrackPos_yL_target[ currentTrack ][ indFrameRec - 1 ] = TrackPos_y_target[ currentTrack ][ indFrameRec - 1 ];
	}
	// printf( "Track %d Ind %d tan %d %d / prev pos %d %d\n" , ind + 1 , indFrameRec - 1 , (int)TrackPos_xL_target[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_yL_target[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_xR_target[ currentTrack ][ indFrameRec - 1 ] , (int)TrackPos_yR_target[ currentTrack ][ indFrameRec - 1 ] );
	// right tangent is half way between first point and left tangent extremity of second point
	if( indFrameRec == 2 ) {
	  TrackPos_xR_target[ currentTrack ][ 0 ] = TrackPos_x_target[ currentTrack ][ 0 ] 
	    + 0.5F * ( TrackPos_xL_target[ currentTrack ][ 1 ] - TrackPos_x_target[ currentTrack ][ 0 ]); 
	  TrackPos_yR_target[ currentTrack ][ 0 ] = TrackPos_y_target[ currentTrack ][ 0 ] 
	    + 0.5F * ( TrackPos_yL_target[ currentTrack ][ 1 ] - TrackPos_y_target[ currentTrack ][ 0 ]); 
	}
      }

      TrackStatus_target[ currentTrack ].nbRecordedFrames++;
    }
    // RECORDING INTERPOLATION TARGET TRACK

  }
  
  ///////////////////////////////////////////////////////////////////////
  // READING TRACK WITH OR WITHOUT INTERPOLATION WITH THE TARGET TRACK
  ///////////////////////////////////////////////////////////////////////
  for( int indTrack = 0 ; 
       indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ; indTrack++ ) {
    // active reading
    if( DM_EnvironmentNode->tracks_replay_table
	->GetValue( indTrack )->valFloat == 1.0F ) {
      // printf("read track %d\n" , indTrack );
      // int indFrameReading = TrackStatus[ indTrack ].indReading;
      
      // records the initial time and uses the elapsed reading
      // time and the elapsed recording time to play in synch
      if( TrackStatus[ indTrack ].indReading == 0 ) {
	TrackStatus[ indTrack ].initialTimeReading = theTime;
      }
      
      double theRecodingElapsedTime;
      double theReadingElapsedTime;
      bool isEndOfLoop = false;
      
      do {
	theRecodingElapsedTime =
	  TrackTime[ indTrack ][ TrackStatus[ indTrack ].indReading ]
	  - TrackStatus[ indTrack ].initialTimeRecording;
	theReadingElapsedTime =
	  (theTime - TrackStatus[ indTrack ].initialTimeReading)
	  * TrackStatus[ indTrack ].readSpeedScale;
	// printf( "Ind %d rec time %.2f read time %.2f\n" , TrackStatus_target[ indTrack ].indReading ,theRecodingElapsedTime , theReadingElapsedTime );
	if( theRecodingElapsedTime <= theReadingElapsedTime ) {
	  TrackStatus[ indTrack ].indReading++;
	}
	else {
	  break;
	}
	
	// loops at the end
	if( TrackStatus[ indTrack ].indReading 
	    >= TrackStatus[ indTrack ].nbRecordedFrames ) {
	  TrackStatus[ indTrack ].indReading = 0;
	  isEndOfLoop = true;
	  break;
	}
      } while( true );
      
      // loop also occurs at the end of an interpolation and
      // requires to have the current mouse position as preceding one also
      if( (TrackStatus[ indTrack ].isActiveInterpolation
	   && TrackStatus[ indTrack ].coefInterpolation == 0.0)
	  || TrackStatus[ indTrack ].isFirstFrame ==  true ) {
	isEndOfLoop = true;
	TrackStatus[ indTrack ].isFirstFrame = false;
      }
      
      // management of previous mouse position with
      // consideration for looping
      
      float alpha = TrackStatus[ indTrack ].coefInterpolation;
      int indFrameReading = TrackStatus[ indTrack ].indReading;
      // printf("alpha %.6f ind %d\n" , TrackStatus[ indTrack ].coefInterpolation , TrackStatus[ indTrack ].indReading );
      
      if( isEndOfLoop ) {
	if( alpha > 0 ) {
	  DM_EnvironmentNode->tracks_x_prev_table->
	    SetValue( (float)Lerp((float)TrackPos_x[ indTrack ][ indFrameReading ] , 
				  (float)TrackPos_x_target[ indTrack ][ indFrameReading ] , alpha ) ,
		      indTrack );
	  DM_EnvironmentNode->tracks_y_prev_table->
	    SetValue( (float)Lerp((float)TrackPos_y[ indTrack ][ indFrameReading ] , 
				  (float)TrackPos_y_target[ indTrack ][ indFrameReading ] , alpha ) ,
		      indTrack );
	}
	else {
	  DM_EnvironmentNode->tracks_x_prev_table->
	    SetValue( (float)TrackPos_x[ indTrack ][ indFrameReading ] , indTrack );
	  DM_EnvironmentNode->tracks_y_prev_table->
	    SetValue( (float)TrackPos_y[ indTrack ][ indFrameReading ] , indTrack );
	}
      }
      else {
	DM_EnvironmentNode->tracks_x_prev_table->
	  SetValue( DM_EnvironmentNode->tracks_x_table->GetNumericalValue( indTrack ) , indTrack );
	DM_EnvironmentNode->tracks_y_prev_table->
	  SetValue( DM_EnvironmentNode->tracks_y_table->GetNumericalValue( indTrack ) , indTrack );
      }
      
      // management of current mouse position with
      // consideration for looping      
      if( alpha > 0 ) {
	DM_EnvironmentNode->tracks_x_table->
	  SetValue( (float)Lerp((float)TrackPos_x[ indTrack ][ indFrameReading ] , 
				(float)TrackPos_x_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
	DM_EnvironmentNode->tracks_y_table->
	  SetValue( (float)Lerp((float)TrackPos_y[ indTrack ][ indFrameReading ] , 
				(float)TrackPos_y_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
      }
      else {
	DM_EnvironmentNode->tracks_x_table->
	  SetValue( (float)TrackPos_x[ indTrack ][ indFrameReading ] , indTrack );
	DM_EnvironmentNode->tracks_y_table->
	  SetValue( (float)TrackPos_y[ indTrack ][ indFrameReading ] , indTrack );
      }
      
      // management of color (w/wo possible interpolation)
      if( alpha > 0 ) {
	DM_EnvironmentNode->tracks_Color_r_table->
	  SetValue( (float)Lerp((float)TrackColor_r[ indTrack ][ indFrameReading ] , 
				(float)TrackColor_r_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
	DM_EnvironmentNode->tracks_Color_g_table->
	  SetValue( (float)Lerp((float)TrackColor_g[ indTrack ][ indFrameReading ] , 
				(float)TrackColor_g_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
	DM_EnvironmentNode->tracks_Color_b_table->
	  SetValue( (float)Lerp((float)TrackColor_b[ indTrack ][ indFrameReading ] , 
				(float)TrackColor_b_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
	DM_EnvironmentNode->tracks_Color_a_table->
	  SetValue( (float)Lerp((float)TrackColor_a[ indTrack ][ indFrameReading ] , 
				(float)TrackColor_a_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
      }
      else {
	DM_EnvironmentNode->tracks_Color_r_table->
	  SetValue( (float)TrackColor_r[ indTrack ][ indFrameReading ] , indTrack );
	DM_EnvironmentNode->tracks_Color_g_table->
	  SetValue( (float)TrackColor_g[ indTrack ][ indFrameReading ] , indTrack );
	DM_EnvironmentNode->tracks_Color_b_table->
	  SetValue( (float)TrackColor_b[ indTrack ][ indFrameReading ] , indTrack );
	DM_EnvironmentNode->tracks_Color_a_table->
	  SetValue( (float)TrackColor_a[ indTrack ][ indFrameReading ] , indTrack );
      }
      
      // printf( "replay %d %.2f %.2f %.2f\n" , indTrack ,
      // 	      DM_EnvironmentNode->tracks_replay_table->GetValue( indTrack )->valFloat ,
      // 	      DM_EnvironmentNode->tracks_x_table->GetValue( indTrack )->valFloat ,
      // 	      DM_EnvironmentNode->tracks_y_table->GetValue( indTrack )->valFloat );
      
      // management of brush ID (w/wo possible interpolation)
      if( alpha < 0.5 ) {
	DM_EnvironmentNode->tracks_BrushID_table->
	  SetValue( (float)TrackBrushID[ indTrack ][ indFrameReading ] , indTrack );
      }
      else {
	DM_EnvironmentNode->tracks_BrushID_table->
	  SetValue( (float)TrackBrushID_target[ indTrack ][ indFrameReading ] , indTrack );
      }
      
      // management of brush radius (w/wo possible interpolation)
      if( alpha > 0 ) {
	DM_EnvironmentNode->tracks_Radius_table->
	  SetValue( (float)Lerp((float)TrackRadius[ indTrack ][ indFrameReading ] , 
				(float)TrackRadius_target[ indTrack ][ indFrameReading ] , alpha ) ,
		    indTrack );
      }
      else {
	DM_EnvironmentNode->tracks_Radius_table->
	  SetValue( (float)TrackRadius[ indTrack ][ indFrameReading ] , indTrack );
      }
      
      // if( TrackRadius[ indTrack ][ indFrameReading ] == 0.0 ) 
      // 	printf( "replay %.d %.d brush %d radius %f (F%d)\n" , 
      // 		TrackPos_x[ indTrack ][ indFrameReading ] , TrackPos_y[ indTrack ][ indFrameReading ] , TrackBrushID[ indTrack ][ indFrameReading ] , TrackRadius[ indTrack ][ indFrameReading ] , indFrameReading );
      
      // interpolation time function management
      // interpolation is frozen if no drawing takes place
      if( TrackStatus[ indTrack ].isActiveInterpolation ) {
	// continuous interpolation
	if( !TrackStatus[ indTrack ].isStepwiseInterpolation ) {
	  TrackStatus[ indTrack ].coefInterpolation += 
	    TrackStatus[ indTrack ].interpolationStep;
	}
	// interpolation after each loop
	else if( isEndOfLoop ) {
	  TrackStatus[ indTrack ].coefInterpolation 
	    += TrackStatus[ indTrack ].interpolationStep
	    * TrackStatus[ indTrack ].nbRecordedFrames
	    / TrackStatus[ indTrack ].readSpeedScale;
	  alpha = TrackStatus[ indTrack ].coefInterpolation;
	  DM_EnvironmentNode->tracks_x_prev_table->
	    SetValue( (float)Lerp((float)TrackPos_x[ indTrack ][ 0 ] , 
				  (float)TrackPos_x_target[ indTrack ][ 0 ] , alpha ) ,
		      indTrack );
	  DM_EnvironmentNode->tracks_y_prev_table->
	    SetValue( (float)Lerp((float)TrackPos_y[ indTrack ][ 0 ] , 
				  (float)TrackPos_y_target[ indTrack ][ 0 ] , alpha ) ,
		      indTrack );
	  DM_EnvironmentNode->tracks_x_table->
	    SetValue( (float)Lerp((float)TrackPos_x[ indTrack ][ 0 ] , 
				  (float)TrackPos_x_target[ indTrack ][ 0 ] , alpha ) ,
		      indTrack );
	  DM_EnvironmentNode->tracks_y_table->
	    SetValue( (float)Lerp((float)TrackPos_y[ indTrack ][ 0 ] , 
				  (float)TrackPos_y_target[ indTrack ][ 0 ] , alpha ) ,
		      indTrack );
	}
      }
      if( TrackStatus[ indTrack ].coefInterpolation >= 1.0 ) {
	TrackStatus[ indTrack ].coefInterpolation = 0.0;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////
  // JUST RENDERING WITHOUT READING A TRACK (IF NOT ACTIVE TRACK READING)
  ///////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////
  // draws from mouse position if the current track is not replayed
  if( DM_EnvironmentNode->tracks_replay_table->GetValue( currentTrack )->valFloat != 1.0F ) {
    if( ((float)DM_EnvironmentNode->mouse_x_prev_scalar->GetValue()->valFloat
	 != CurrentMousePos_x
	 || (float)DM_EnvironmentNode->mouse_y_prev_scalar->GetValue()->valFloat
	 != CurrentMousePos_y
	 // || isAMove || wasAMove 
	 )
	&& isDrawingOn ) {
      DM_EnvironmentNode->tracks_Color_r_table->
	SetValue( (float)table_local_color->GetNumericalValue( 0 )
		  + (float)table_color_spectrum->GetNumericalValue( 0 ) , 
		  currentTrack );
      DM_EnvironmentNode->tracks_Color_g_table->
	SetValue( (float)table_local_color->GetNumericalValue( 1 )
		  + (float)table_color_spectrum->GetNumericalValue( 1 ) , 
		  currentTrack );
      DM_EnvironmentNode->tracks_Color_b_table->
	SetValue( (float)table_local_color->GetNumericalValue( 2 )
		  + (float)table_color_spectrum->GetNumericalValue( 2 ) , 
		  currentTrack );
      DM_EnvironmentNode->tracks_Color_a_table->
	SetValue( (float)table_local_color->GetNumericalValue( 3 )
		  + (float)table_color_spectrum->GetNumericalValue( 3 ) , 
		  currentTrack );
      /* printf("Track %d color a %.2f %.2f %.2f %.2f\n" , currentTrack+1, (float)table_local_color->GetNumericalValue( 0 )
	 + (float)table_color_spectrum->GetNumericalValue( 1 ) , 
	 (float)table_local_color->GetNumericalValue( 1 ) 
	 + (float)table_color_spectrum->GetNumericalValue( 1 ) , 
	 (float)table_local_color->GetNumericalValue( 2 ) 
	 + (float)table_color_spectrum->GetNumericalValue( 2 ) , 
	 (float)table_local_color->GetNumericalValue( 3 )
	 + (float)table_color_spectrum->GetNumericalValue( 3 )  ); */
      DM_EnvironmentNode->tracks_BrushID_table->
	SetValue( (int)scalar_brushID->GetValue()->valInt , 
		  currentTrack );
      DM_EnvironmentNode->tracks_Radius_table->
	SetValue( (float)scalar_brushRadius->GetValue()->valFloat
		  + (float)scalar_brush_radius_vol->GetValue()->valFloat , 
		  currentTrack );
      // printf("Track %d brush ID radius %d %.2f\n" , currentTrack+1, (int)scalar_brushID->GetValue()->valInt, (float)scalar_brushRadius->GetValue()->valFloat + (float)scalar_brush_radius_vol->GetValue()->valFloat );
    }
  }

}


//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A SVG FILE
//////////////////////////////////////////////////////////////////
void readsvg( int *fileDepth , int indTrack , char *fileName ,
	      bool isSource ) {
  int            curChar;
  dm_XMLTag     *XMLtag = new dm_XMLTag();
  char          *val = NULL;
  float          translation[2] = { 0.0 , 0.0 };;
  char           *tempString = NULL;
  
  (*fileDepth) = 0;
  fileXML[ (*fileDepth) ] = fopen( fileName , "rb" );
  if ( fileXML[ (*fileDepth) ] == NULL ) {
    sprintf( ErrorStr , "File %s not opened!" , fileName ); ReportError( ErrorStr ); // throw 152;
    return;
  }
  printf("Loading %s\n" , fileName );

  // reading the XML file
  curChar = ' ';
  SpaceCommentsInclude( fileDepth , &curChar , NULL , NULL );

  // scans the svg file
  while( curChar != EOF ) {
    // reading the tag of a component
    XMLtag->ParseTagXML( fileDepth , &curChar , true , NULL , NULL );
    // printf( "New tag %s type %d\n" , XMLtag->tag , XMLtag->tagType );

    ////////////////////////////
    ////// unprocessed tag
    if( strcmp( XMLtag->tag , "svg" ) == 0  
	|| strcmp( XMLtag->tag , "defs" ) == 0
	|| strcmp( XMLtag->tag , "metadata" ) == 0
	|| strcmp( XMLtag->tag , "rdf:RDF" ) == 0
	|| strcmp( XMLtag->tag , "cc:Work" ) == 0
	|| strcmp( XMLtag->tag , "dc:type" ) == 0
	|| strcmp( XMLtag->tag , "dc:title" ) == 0
	|| strcmp( XMLtag->tag , "?xml" ) == 0
	|| ( (strcmp( XMLtag->tag , "dc:format" ) == 0)
	     && (XMLtag->tagType == ClosingTag) )
	|| ( (strcmp( XMLtag->tag , "g" ) == 0)
	     && (XMLtag->tagType == ClosingTag) )
	) {
      // printf( "Skipped tag %s %c\n" , XMLtag->tag , curChar );
    }
    ////////////////////////////
    ////// dc:format transformation tag
    else if( strcmp( XMLtag->tag , "dc:format" ) == 0 ) {
      ReadStringWithTermination( fileDepth , &curChar , 
				 &tempString ,
				 '<' ,
				 true , 
				 NULL , NULL );
      // deallocates possibly preallocated memory
      if( tempString ) {
	// printf( "format string %s\n" , tempString );
	delete [] tempString;
	tempString = NULL;
      }
      
      XMLtag->ParseTagXML( fileDepth , &curChar , true , NULL , NULL );
      // printf( "Closing tag %s\n" , XMLtag->tag );
      if( strcmp( XMLtag->tag , "dc:format" ) != 0 ) {
	sprintf( ErrorStr , "Error: dc:format XML closing tag expected!" ); ReportError( ErrorStr ); throw 50;
      }
    }
    ////////////////////////////
    ////// group transformation tag
    else if( strcmp( XMLtag->tag , "g" ) == 0 ) {
      if( XMLtag->hasAttribute( "transform" ) ) {
	StrCpy( &val , XMLtag->getAttributeStringValue( "transform" ) );
	// printf( "transform string %s\n" , val );
	if( strncmp( val , "translate" , 9 ) == 0 ) {
	  int ch = val[10];
	  int ind = 11;
	  translation[0] = ReadFloat( NULL , &ch , true  , 
				      val , &ind , false );
	  ExpectsChar( NULL , ',' , 
		       &ch , true , val , &ind );
	  translation[1] = ReadFloat( NULL , &ch , true  , 
				      val , &ind , false );	
	  ExpectsChar( NULL , ')' , 
		       &ch , true , val , &ind );
	  // printf( "Translation %f %f\n" , translation[0] , translation[1] );
	}
	else {
	  sprintf( ErrorStr , "Error: expected SVG group \"translate\" value of \"transform\" attribute!" ); ReportError( ErrorStr ); throw 50;
	}
      }
      else {
	sprintf( ErrorStr , "Error: expected SVG group transformation \"transform\" attribute!" ); ReportError( ErrorStr ); throw 50;
      }
    }
    ////////////////////////////
    ////// group transformation tag
    else if( strcmp( XMLtag->tag , "path" ) == 0  
	     ) {
      if( XMLtag->hasAttribute( "d" ) ) {
	StrCpy( &val , XMLtag->getAttributeStringValue( "d" ) );
	// printf( "path values [%s]\n", val );
	LoadTrackFromPath( val , indTrack , translation , isSource );
	for (int i = 0 ; i < 100 ; i++ ) {
	  // printf("track %d %f %f\n" , i ,
	  // 	 TrackPos_x[ 0 ][ i ] , 
	  // 	 TrackPos_y[ 0 ][ i ]);
	}
      }
      else {
	sprintf( ErrorStr , "Error: expected SVG group transformation \"translation\" attribute!" ); ReportError( ErrorStr ); throw 50;
      }
    }
    ////////////////////////////
    ////// unknown tag
    else {
      sprintf( ErrorStr , "Error: unknown SVG tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
      delete XMLtag;
      XMLtag = NULL;
      if (val) {
	delete [] val;
	val = NULL;
      }
      fclose( fileXML[ (*fileDepth) ] );
      (*fileDepth)--;
      return;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
  if (val) {
    delete [] val;
    val = NULL;
  }
  fclose( fileXML[ (*fileDepth) ] );
  (*fileDepth)--;
}


//////////////////////////////////////////////////////////////////
// WRITES A TRACK TO A SVG FILE
//////////////////////////////////////////////////////////////////
int writesvg( char *fileName , char * description , 
	      int width, int height) {
  FILE          *fileSVG;
  
  fileSVG = fopen( fileName , "wb" );
  if ( fileSVG == NULL ) {
    sprintf( ErrorStr , "File %s not opened!" , fileName ); ReportError( ErrorStr ); throw 152;
  }

  fprintf( fileSVG , "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\n<svg\n   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n   xmlns:cc=\"http://creativecommons.org/ns#\"\n   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n   xmlns:svg=\"http://www.w3.org/2000/svg\"\n   xmlns=\"http://www.w3.org/2000/svg\"\n   version=\"1.1\"\n   width=\"%d\"\n   height=\"%d\"\n   id=\"svg2\">\n  <defs\n     id=\"defs4\" />\n  <metadata\n     id=\"metadata7\">\n    <rdf:RDF>\n      <cc:Work\n         rdf:about=\"\">\n        <dc:format>image/svg+xml</dc:format>\n        <dc:type\n           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n        <dc:title></dc:title>\n      </cc:Work>\n    </rdf:RDF>\n  </metadata>\n  <g\n     transform=\"translate(0.0,0.0)\"\n     id=\"layer1\">\n", width, height );

  // for all tracks
  for( int ind = 0 ; ind < (DM_EnvironmentNode->Nb_max_tracks + 1) ; ind++ ) {
    // main track
    if( TrackStatus[ ind ].nbRecordedFrames > 0 ) {
      fprintf( fileSVG , "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n       d=\"M %f,%f " , 
	       TrackStatus[ ind ].initialTimeRecording ,
	       TrackStatus[ ind ].initialTimeReading ,
	       TrackPos_x[ ind ][ 0 ] , 
	       TrackPos_y[ ind ][ 0 ] );
      for( int indFrame = 1 ; 
	   indFrame < TrackStatus[ ind ].nbRecordedFrames ; 
	   indFrame++ ) {
	// skips identical frame
	// if( TrackPos_x[ ind ][ indFrame ] 
	//     == TrackPos_x[ ind ][ indFrame - 1 ] 
	//     && TrackPos_x[ ind ][ indFrame ] 
	//     == TrackPos_x[ ind ][ indFrame - 1 ] ) {
	//   continue;
	// }
	// move point with a new curve
	if( TrackPos_xR[ ind ][ indFrame - 1 ] == 0.0 
	    && TrackPos_yR[ ind ][ indFrame - 1 ] == 0.0 
	    && TrackPos_xL[ ind ][ indFrame ] == 0.0 
	    && TrackPos_yL[ ind ][ indFrame ] == 0.0 ) {
	  fprintf( fileSVG , "M %f,%f " , 
		   TrackPos_x[ ind ][ indFrame ] , 
		   TrackPos_y[ ind ][ indFrame ]);
	}
	// curve point
	else {
	  fprintf( fileSVG , "C %f,%f " , 
		   TrackPos_xR[ ind ][ indFrame - 1] , 
		   TrackPos_yR[ ind ][ indFrame - 1]);
	  // the right tangent of the last point on curve should be computed
	  // as the half way between right control of preceding point and current point
	  if( indFrame == TrackStatus[ ind ].nbRecordedFrames - 1 ) {
	    TrackPos_xR[ ind ][ indFrame ] = TrackPos_x[ ind ][ indFrame ] 
	      + 0.5F * ( TrackPos_xR[ ind ][ indFrame - 1 ] - TrackPos_x[ ind ][ indFrame ]); 
++	    TrackPos_yR[ ind ][ indFrame ] = TrackPos_y[ ind ][ indFrame ] 
	      + 0.5F * ( TrackPos_yR[ ind ][ indFrame - 1 ] - TrackPos_y[ ind ][ indFrame ]); 
	  }
	  // the last point has a null left tangent
	  if( indFrame == TrackStatus[ ind ].nbRecordedFrames - 1 ) {
	    fprintf( fileSVG , "%f,%f " , 
		     TrackPos_x[ ind ][ indFrame ] , 
		     TrackPos_y[ ind ][ indFrame ]);
	  }
	  else {
	    fprintf( fileSVG , "%f,%f " , 
		     TrackPos_xL[ ind ][ indFrame ] , 
		     TrackPos_yL[ ind ][ indFrame ]);
	  }
	  fprintf( fileSVG , "%f,%f " , 
		   TrackPos_x[ ind ][ indFrame ] , 
		   TrackPos_y[ ind ][ indFrame ]);
	}
      }
      fprintf( fileSVG , "\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1\" />\n" , ind );
    }
    if( TrackStatus_target[ ind ].nbRecordedFrames > 0 ) {
      // target track
      fprintf( fileSVG , "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n       d=\"M %f,%f C " , 
    	       TrackStatus_target[ ind ].initialTimeRecording ,
    	       TrackStatus_target[ ind ].initialTimeReading ,
    	       TrackPos_x_target[ ind ][ 0 ] , 
    	       TrackPos_y_target[ ind ][ 0 ]);
      for( int indFrame = 1 ; 
    	   indFrame < TrackStatus_target[ ind ].nbRecordedFrames ; 
    	   indFrame++ ) {
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_xR_target[ ind ][ indFrame - 1 ] , 
    		 TrackPos_yR_target[ ind ][ indFrame - 1 ]);
    	// the right tangent of the last point on curve should be computed
    	// as the half way between right control of preceding point and current point
    	if( indFrame == TrackStatus_target[ ind ].nbRecordedFrames - 1 ) {
    	  TrackPos_xR_target[ ind ][ indFrame ] = TrackPos_x_target[ ind ][ indFrame ] 
    	    + 0.5F * ( TrackPos_xR_target[ ind ][ indFrame - 1 ] - TrackPos_x_target[ ind ][ indFrame ]); 
    	  TrackPos_yR_target[ ind ][ indFrame ] = TrackPos_y_target[ ind ][ indFrame ] 
    	    + 0.5F * ( TrackPos_yR_target[ ind ][ indFrame - 1 ] - TrackPos_y_target[ ind ][ indFrame ]); 
    	}
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_xL_target[ ind ][ indFrame ] , 
    		 TrackPos_yL_target[ ind ][ indFrame ]);
    	fprintf( fileSVG , "%f,%f " , 
    		 TrackPos_x_target[ ind ][ indFrame ] , 
    		 TrackPos_y_target[ ind ][ indFrame ]);
      }
      fprintf( fileSVG , "z\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1\" />\n", ind + (DM_EnvironmentNode->Nb_max_tracks + 1) );
    }
  }
  fprintf( fileSVG , "  </g>\n</svg>\n" );

  fclose( fileSVG );
  return 0;
}

