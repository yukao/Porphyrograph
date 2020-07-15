/*! \file pg-stroke.h
 *
 * 
 *     File pg-stroke.h
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

#ifndef PG_STROKE_H
#define PG_STROKE_H

/// mouse recording tracks management
extern float **pg_Path_Pos_x_prev;
extern float **pg_Path_Pos_y_prev;
extern float **pg_Path_Pos_x;
extern float **pg_Path_Pos_y;
extern float **pg_Path_Pos_xL;
extern float **pg_Path_Pos_yL;
extern float **pg_Path_Pos_xR;
extern float **pg_Path_Pos_yR;
extern float **pg_Path_Time;
struct pg_Path_Status_Struct {
  bool isFirstFrame;
  bool isActiveRecording;
  bool isNormalized;
  int nbRecordedFrames;
  int indReading;
  float initialTimeRecording;
  float finalTimeRecording;
  float initialTimeReading;
  float readSpeedScale;
};
extern struct pg_Path_Status_Struct *pg_Path_Status;
extern int *pg_indPreviousFrameReading;

#ifdef PG_BEZIER_PATHS
// convex hull shipped to the GPU
extern glm::vec2 pg_BezierControl[(PG_NB_PATHS + 1) * 4];
extern glm::vec2 pg_BezierHull[(PG_NB_PATHS + 1) * 4];
extern glm::vec4 pg_BezierBox[(PG_NB_PATHS + 1)];
#endif

// pen_radius multiplicative factor for large pen_brush
extern float pen_radiusMultiplier;

struct vec2
{
  float x;
  float y;
};


void ScaleVec2(vec2* pOut_result, vec2* p0, float c0);
void AddScaledVec2_Self(vec2* pOut_result, vec2* p0, float c0);


/// SAVE IMAGE
#ifdef WIN32
DWORD WINAPI writesvg(LPVOID lpParam);
#else
void* writesvg(void * lpParam);
#endif

//The base Catmull-Rom evaluation function which requires 4 points
// 
void CatmullRom_Evaluate(vec2* pOut_result, vec2* p0, vec2* p1, vec2* p2, vec2* p3, float t);

// initialization of the tables that contain the stroke parameters
void pg_initStrokes( void );

// loads a track from a path string
void LoadPathFromXML( char *pathString , int indPath , float *translation , float pathRadius, float path_r_color, float path_g_color, float path_b_color, 
					  int * indCurve, float precedingCurrentPoint[2], float  currentPoint[2]);

// loads a track from a svg file
void load_svg_path( char *fileName , int indPath , int indTrack , float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale);
void readsvg( int *fileDepth , int indPath , char *fileName , float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale);

// CONVEX HULL 
#ifdef PG_BEZIER_PATHS
bool pointEquals(glm::vec2 p, glm::vec2 q);
float distance(glm::vec2 p, glm::vec2 q);
bool left_oriented(glm::vec2 p1, glm::vec2 p2, glm::vec2 candidate);
void convex_hull(glm::vec2 control_points[4], int *hull);
void hull_expanded_by_radius(glm::vec2 control_points[4], int *hull, float radius, glm::vec2 hull_points[4]);
void boundingBox_expanded_by_radius(glm::vec2 control_points[4], float radius, glm::vec4 *boundingBox);
void build_bounding_box(int indPath);
void build_expanded_hull(int indPath);
void test_hull(void);
#endif

// REPLAY PATHS  
void pg_replay_paths(float theTime);
// update of the tables that contain the stroke parameters
void pg_update_pulsed_colors_and_replay_paths( float theTime );


#endif
