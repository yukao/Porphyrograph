/*! \file dm-stroke.h
 *  \brief the header file of dm-main.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-main.h
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

#ifndef DM_STROKE_H
#define DM_STROKE_H

/// mouse recording tracks management
extern float **TrackPos_x;
extern float **TrackPos_y;
extern float **TrackPos_xL;
extern float **TrackPos_yL;
extern float **TrackPos_xR;
extern float **TrackPos_yR;
extern double **TrackTime;
extern float **TrackPos_x_target;
extern float **TrackPos_y_target;
extern float **TrackPos_xL_target;
extern float **TrackPos_yL_target;
extern float **TrackPos_xR_target;
extern float **TrackPos_yR_target;
extern double **TrackTime_target;
extern float **TrackPos_x_temp_target;
extern float **TrackPos_y_temp_target;
extern double **TrackTime_temp_target;
struct TrackStatus {
  bool isFirstFrame;
  bool isActiveInterpolation;
  bool isActiveRecording;
  bool isNormalized;
  int nbRecordedFrames;
  int indReading;
  float coefInterpolation;
  float sppedInterpolation;
  double initialTimeRecording;
  double initialTimeReading;
  float interpolationStep;
  float readSpeedScale;
  bool isStepwiseInterpolation;
};
extern struct TrackStatus *TrackStatus;
extern struct TrackStatus *TrackStatus_target;

/*
struct DistanceTableEntry
{
  float t;
  float distance;
};
*/     
struct vec2
{
  float x;
  float y;
};

void ScaleVec2(vec2* pOut_result, vec2* p0, float c0);
void AddScaledVec2_Self(vec2* pOut_result, vec2* p0, float c0);


/// SAVE IMAGE
int writesvg(char *filename, char *description, int width, int height);

//The base Catmull-Rom evaluation function which requires 4 points
// 
void CatmullRom_Evaluate(vec2* pOut_result, vec2* p0, vec2* p1, vec2* p2, vec2* p3, float t);

// initialization of the tables that contain the stroke parameters
void InitStrokes( void );

// loads a track from a path string
void LoadTrackFromPath( char *pathString , int indTrack , float *translation , bool isSource );

// loads a track from a svg file
void readsvg( int *fileDepth , int indTrack , char *fileName , bool isSource );

// adjusts the duration of the target to the source
// replaces the current control points of the target by control points 
// that are synchronous with the ones of the source
void NormalizeTargetTrack( int indTrack );

// update of the tables that contain the stroke parameters
void updateMouseEnvironmentVariablesAndTables( double theTime );

#endif
