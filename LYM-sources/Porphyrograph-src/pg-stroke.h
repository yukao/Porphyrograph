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
//extern float **pg_Path_Pos_x_prev;
//extern float **pg_Path_Pos_y_prev;
//extern float **pg_Path_Pos_x;
//extern float **pg_Path_Pos_y;
//extern float **pg_Path_Pos_xL;
//extern float **pg_Path_Pos_yL;
//extern float **pg_Path_Pos_xR;
//extern float **pg_Path_Pos_yR;
//extern float **pg_Path_TimeStamp;
struct pg_Path_Data_Struct {
	float path_Color_r;
	float path_Color_g;
	float path_Color_b;
	float path_Color_a;
	int path_BrushID;
	float path_RadiusX;
	float path_RadiusY;
	float Pos_x_prev;
	float Pos_y_prev;
	float path_Pos_x;
	float path_Pos_y;
	float path_Pos_xL;
	float path_Pos_yL;
	float path_Pos_xR;
	float path_Pos_yR;
	double TimeStamp;
};
struct pg_Path_Status_Struct {
	bool isFirstFrame;
	bool isActiveRecording;
	bool isNormalized;
	int path_nbRecordedFrames;
	int indReading;
	double initialTimeRecording;
	double finalTimeRecording;
	double initialTimeReading;
	double readSpeedScale; // speed from the scenario file
	double lastPlayedFrameTime;
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
//#ifdef WIN32
//DWORD WINAPI writesvg(LPVOID lpParam);
//#else
//void* writesvg(void * lpParam);
//#endif
void writesvg(cv::String imageFileName);

//The base Catmull-Rom evaluation function which requires 4 points
// 
void CatmullRom_Evaluate(vec2* pOut_result, vec2* p0, vec2* p1, vec2* p2, vec2* p3, float t);

// initialization of the tables that contain the stroke parameters
void pg_initStrokes( void );

// char array to integer or float
long     pg_ScanIntegerString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind);
float    pg_ScanFloatString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind);

// loads a track from a path string
int LoadPathPointsFromXML(char* pathString, int indPath,
	glm::mat4* p_M_transf, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
	float precedingCurrentPoint[2], float  currentPoint[2],
	bool withRecordingOfStrokeParameters, bool with_color__brush_radius_from_scenario,
	float* path_length, double p_secondsforwidth, int *nb_timeStamps);
void LoadPathColorsFromXML(string pathString, int indPath, int* nbRecordedFrames);
void LoadPathBrushesFromXML(string pathString, int indPath, int* nbRecordedFrames);
void LoadPathTimeStampsFromXML(string pathString, int indPath, int* nbRecordedFrames);

// loads a track from a svg file
#if defined(var_path_replay_trackNo_1) && defined(var_path_record_1)
void load_svg_path( char *fileName , int indPath , int indTrack , float pathRadius, float path_r_color, float path_g_color, float path_b_color,
	float readSpeedScale, string path_ID, bool p_with_color__brush_radius_from_scenario, double secondsforwidth);
#endif
void readsvg( int *fileDepth , int indPath , char *fileName , float pathRadius, float path_r_color, float path_g_color, float path_b_color, 
	float readSpeedScale, string path_ID, bool p_with_color__brush_radius_from_scenario, double secondsforwidth);

// CONVEX HULL 
#ifdef PG_BEZIER_PATHS
bool pointEquals(glm::vec2 *p, glm::vec2 *q);
bool left_oriented(glm::vec2 *p1, glm::vec2 *p2, glm::vec2 *candidate);
void cubicBezier(glm::vec2 control_points[4], glm::vec2* curve_point, float alphaBezier);
void cubicBezier(glm::vec3 control_points[4], glm::vec3* curve_point, float alphaBezier);
void Bezier_hull_expanded_by_radius(glm::vec2 control_points[4], int *hull, float radius, glm::vec2 hull_points[4]);
void Bezier_boundingBox_expanded_by_radius(glm::vec2 control_points[4], float radius, glm::vec4 *boundingBox);
float Bezier_length(glm::vec2 control_points[4], int nb_steps);
void build_bounding_box(int indPath);
void build_expanded_hull(int indPath);
void test_hull(void);
#endif

#if defined(var_Novak_flight_on)
#define PG_NB_FLIGHTS 11
extern glm::vec3     prev_Novak_flight_control_points[PG_NB_FLIGHTS][4];
extern glm::vec3     cur_Novak_flight_control_points[PG_NB_FLIGHTS][4];
extern glm::vec3     cur_Novak_flight_points[PG_NB_FLIGHTS];
extern glm::vec2     cur_Novak_flight_2D_points[PG_NB_FLIGHTS];
extern float         prev_Novak_flightTime[PG_NB_FLIGHTS];
extern double        prev_Novak_flightCurrentCLockTime[PG_NB_FLIGHTS];
extern float         cur_Novak_flightTime[PG_NB_FLIGHTS];
extern int           cur_Novak_flightIndex[PG_NB_FLIGHTS];
extern glm::vec3     Novak_flight_deviation[PG_NB_FLIGHTS];
extern float         cur_Novak_flightPerlinNoise[PG_NB_FLIGHTS][3][2];
void             Novak_flight_next_control_points(int indFlight);
void             Novak_flight_init_control_points(void);
void             Novak_flight_update_coordinates(int indFlight);
void             cur_Novak_flight_2D_coordinates(void);
#endif

// calculation of tangents from successive locations of the pen
void stroke_geometry_calculation(int indPath, int curr_position_x, int curr_position_y);
// REPLAY PATHS  
void pg_replay_paths(double theTime);
// update of the tables that contain the stroke parameters
void pg_update_pulsed_colors_and_replay_paths( double theTime );


#endif
