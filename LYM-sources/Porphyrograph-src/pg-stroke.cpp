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
//float **pg_Path_Color_r = NULL;
//float **pg_Path_Color_g = NULL;
//float **pg_Path_Color_b = NULL;
//float **pg_Path_Color_a = NULL;
//
//int **pg_Path_BrushID = NULL;
//
//float **pg_Path_RadiusX = NULL;
//float **pg_Path_RadiusY = NULL;
//
//float **pg_Path_Pos_x_prev = NULL;
//float **pg_Path_Pos_y_prev = NULL;
//float **pg_Path_Pos_x = NULL;
//float **pg_Path_Pos_y = NULL;
//float **pg_Path_Pos_xL = NULL;
//float **pg_Path_Pos_yL = NULL;
//float **pg_Path_Pos_xR = NULL;
//float **pg_Path_Pos_yR = NULL;
// pg_Path_TimeStamp is only used for replaying a path that has been previously
// recorded in the same session 
// for paths loaded from a svg file, either initial and final time are given
// and an average duration is calculated and used to timestamp the points
// or an average 1/60 s duration is chosen. The readSpeedScale value can
// be adjusted in the scenario to replay at an appropriate speed
//float **pg_Path_TimeStamp = NULL;
struct pg_Path_Status_Struct* pg_Path_Status = NULL;
struct pg_Path_Data_Struct** pg_Path_Data = NULL;
int *pg_indPreviousFrameReading = NULL;

#ifdef PG_BEZIER_PATHS
// convex hull shipped to the GPU
glm::vec2 pg_BezierControl[(PG_NB_PATHS + 1) * 4];
glm::vec2 pg_BezierHull[(PG_NB_PATHS + 1) * 4];
glm::vec4 pg_BezierBox[(PG_NB_PATHS + 1)];
#endif

// pen_radius multiplicative factor for large pen_brush 
float pen_radiusMultiplier = 1.0f;

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
void pg_initStrokes( void ) {
  // looks for specific variables (brush radius, local color, brush

  // mouse pointer tracks recording initialization
  pg_Path_Data = new pg_Path_Data_Struct * [PG_NB_PATHS + 1];
  pg_Path_Status = new struct pg_Path_Status_Struct[PG_NB_PATHS + 1];
  pg_indPreviousFrameReading = new int[PG_NB_PATHS + 1];
  // printf("Path initialization size %d\n", max_mouse_recording_frames);
  for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
	  pg_Path_Data[ind]
		  = new pg_Path_Data_Struct[max_mouse_recording_frames];
	  pg_Path_Status[ind].isFirstFrame = false;
	  pg_Path_Status[ind].isActiveRecording = false;
	  pg_Path_Status[ind].isNormalized = false;
	  pg_Path_Status[ind].nbRecordedFrames = 0;
	  pg_Path_Status[ind].indReading = -1;
	  pg_Path_Status[ind].initialTimeRecording = 0.0f;
	  pg_Path_Status[ind].finalTimeRecording = 0.0f;
	  pg_Path_Status[ind].initialTimeReading = 0.0f;
	  pg_Path_Status[ind].readSpeedScale = 1.0F;
	  pg_indPreviousFrameReading[ind] = 0;
  }

  for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
	  synchr_start_recording_path[ind] = false;
	  synchr_start_path_replay_trackNo[ind] = -1;
	  recorded_path[ind] = false;
	  is_path_replay[ind] = false;
  }

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
// LOADS A TRACK FROM A PATH STRING
//////////////////////////////////////////////////////////////////
void LoadPathPointsFromXML(char *pathString, int indPath,
	glm::mat4 *p_M_transf, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
	float precedingCurrentPoint[2], float  currentPoint[2], bool withRecordingOfStrokeParameters, bool with_color__brush_radius_from_scenario) {
	int            curChar = ' ';
	int            indChar = 0;
	int            indFrame = 0;
	// printf("[%s]\n", pathString);
	glm::vec4 vec0(0, 0, 0, 1);

	//std::cout << glm::to_string(*p_M_transf) << std::endl;

	if (indPath < 1 || indPath > PG_NB_PATHS) {
		return;
	}

	while (_SpaceChar(curChar)) {
		curChar = pathString[indChar++];
	}
	while (indChar <= (int)strlen(pathString) && curChar != EOF  && curChar != '"') {
		if (indFrame >= max_mouse_recording_frames) {
			sprintf(ErrorStr, "Error: configuration file parameter value %d is too low for recording the full path", max_mouse_recording_frames); ReportError(ErrorStr);
		}
		switch (curChar) {
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
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (relative  && (indFrame) != 0)
					currentPoint[0] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[0] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				if (curChar == ',') {
					curChar = pathString[indChar++];
					while (_SpaceChar(curChar)) {
						curChar = pathString[indChar++];
					}
				}
				else {
					sprintf(ErrorStr, "Expected char [%c] not found!", ','); ReportError(ErrorStr); throw 17;
				}
				if (relative  && indFrame != 0)
					currentPoint[1] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}
				nbMovePoints++;
				// if the move is to a point different from the preceding current point, an out of frame
				// point is added so that the rendering in porphyrograph jumps to the new current point
				if ((precedingCurrentPoint[0] != currentPoint[0]
				     || precedingCurrentPoint[1] != currentPoint[1])
					&& indFrame < max_mouse_recording_frames) {
					// copies current fixed parameters
					if (!withRecordingOfStrokeParameters) {
						pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
						pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
						pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
						pg_Path_Data[indPath][indFrame].Color_a = 1.0;
						pg_Path_Data[indPath][indFrame].BrushID = 0;
						pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
						pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
						//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
					}
					// shifts parameters one value ahead due to additional point
					else {
						for (int indFrameAux = pg_Path_Status[indPath].nbRecordedFrames - 1; indFrameAux >= indFrame; indFrameAux--) {
							if (with_color__brush_radius_from_scenario) {
								pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
								pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
								pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
								pg_Path_Data[indPath][indFrame].Color_a = 1.0;
								pg_Path_Data[indPath][indFrame].BrushID = 0;
								pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
								pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
							}
							else {
								pg_Path_Data[indPath][indFrameAux + 1].Color_r = pg_Path_Data[indPath][indFrameAux].Color_r;
								pg_Path_Data[indPath][indFrameAux + 1].Color_g = pg_Path_Data[indPath][indFrameAux].Color_g;
								pg_Path_Data[indPath][indFrameAux + 1].Color_b = pg_Path_Data[indPath][indFrameAux].Color_b;
								pg_Path_Data[indPath][indFrameAux + 1].Color_a = pg_Path_Data[indPath][indFrameAux].Color_a;
								pg_Path_Data[indPath][indFrameAux + 1].BrushID = pg_Path_Data[indPath][indFrameAux].BrushID;
								pg_Path_Data[indPath][indFrameAux + 1].RadiusX = pg_Path_Data[indPath][indFrameAux].RadiusX;
								pg_Path_Data[indPath][indFrameAux + 1].RadiusY = pg_Path_Data[indPath][indFrameAux].RadiusY;
							}
							pg_Path_Data[indPath][indFrameAux + 1].TimeStamp = pg_Path_Data[indPath][indFrameAux].TimeStamp;
						}
						pg_Path_Status[indPath].nbRecordedFrames += 1;
					}
					vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
					pg_Path_Data[indPath][indFrame].Pos_x_prev = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y_prev = vec0.y;
					pg_Path_Data[indPath][indFrame].Pos_x = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Data[indPath][indFrame].Pos_y = PG_OUT_OF_SCREEN_CURSOR;
					// printf("src move to %d %f %f\n" , indFrame ,
					// 	     pg_Path_Pos_x[ indPath ][ indFrame ] , 
					// 	     pg_Path_Pos_y[ indPath ][ indFrame ]);
					indFrame++;
				}
				// move from out of screen point to the current point
				if (indFrame < max_mouse_recording_frames) {
					// copies current fixed parameters
					if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
						pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
						pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
						pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
						pg_Path_Data[indPath][indFrame].Color_a = 1.0;
						pg_Path_Data[indPath][indFrame].BrushID = 0;
						pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
						pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
						//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
					}
					pg_Path_Data[indPath][indFrame].Pos_x_prev = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Data[indPath][indFrame].Pos_y_prev = PG_OUT_OF_SCREEN_CURSOR;
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					pg_Path_Data[indPath][indFrame].Pos_x = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y = vec0.y;
					// printf("src move to %d %f %f\n" , indFrame ,
					// 	     pg_Path_Pos_x[ indPath ][ indFrame ] , 
					// 	     pg_Path_Pos_y[ indPath ][ indFrame ]);

					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					indFrame++;
				}
			}
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
			float tanL[2] = { 0.f, 0.f };
			float tanR[2] = { 0.f, 0.f };

			bool relative = (curChar == 'c');
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (relative)
					currentPoint[0] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar) + precedingCurrentPoint[0];
				else
					currentPoint[0] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				if (curChar == ',') {
					curChar = pathString[indChar++];
					while (_SpaceChar(curChar)) {
						curChar = pathString[indChar++];
					}
				}
				else {
					sprintf(ErrorStr, "Expected char [%c] not found!", ','); ReportError(ErrorStr); throw 17;
				}
				if (relative)
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar) + precedingCurrentPoint[1];
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}

				if ((nbCurvePoints) % 3 == 2 && indFrame < max_mouse_recording_frames) {
					// copies current fixed parameters
					if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
						pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
						pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
						pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
						pg_Path_Data[indPath][indFrame].Color_a = 1.0;
						pg_Path_Data[indPath][indFrame].BrushID = 0;
						pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
						pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
						//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
					}
					vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
					//std::cout << glm::to_string(glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1)) << std::endl;
					//std::cout << glm::to_string(vec0) << std::endl << std::endl;
					pg_Path_Data[indPath][indFrame].Pos_x_prev = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y_prev = vec0.y;
					pg_Path_Data[indPath][indFrame].Pos_xL = tanL[0];
					pg_Path_Data[indPath][indFrame].Pos_yL = tanL[1];
					pg_Path_Data[indPath][indFrame].Pos_xR = tanR[0];
					pg_Path_Data[indPath][indFrame].Pos_yR = tanR[1];
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					pg_Path_Data[indPath][indFrame].Pos_x = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y = vec0.y;
					// if( indFrame < 5 ) {
					// 	printf("src curve to %d %f %f\n" , indFrame ,
					// 	       pg_Path_Pos_x[ indPath ][ indFrame ] , 
					// 	       pg_Path_Pos_y[ indPath ][ indFrame ]);
					// }

					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					indFrame++;
					//printf("src curve to %d %f %f\n" , indFrame - 1 ,
					// 	    pg_Path_Pos_x[ indPath ][ indFrame - 1 ] , 
					// 	    pg_Path_Pos_y[ indPath ][ indFrame - 1 ]);
				}
				// right tangent of the first (and current) point
				else if ((nbCurvePoints) % 3 == 0 && indFrame > 0 && indFrame < max_mouse_recording_frames) {
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					tanL[0] = vec0.x;
					tanL[1] = vec0.y;
				}
				// left tangent of the second (and next) point
				else if ((nbCurvePoints) % 3 == 1 && indFrame < max_mouse_recording_frames) {
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					tanR[0] = vec0.x;
					tanR[1] = vec0.y;
				}
				nbCurvePoints++;
			}
			if (nbCurvePoints % 3 != 0) {
				printf("Bezier curve points are expected to be given by groups of 3 %d next %c %f %f!\n", nbCurvePoints, curChar, currentPoint[0], currentPoint[1]);
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
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			if (indFrame < max_mouse_recording_frames) {
				// copies current fixed parameters
				if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
					pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
					pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
					pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
					pg_Path_Data[indPath][indFrame].Color_a = 1.0;
					pg_Path_Data[indPath][indFrame].BrushID = 0;
					pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
					pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
				}
				vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
				pg_Path_Data[indPath][indFrame].Pos_x_prev = vec0.x;
				pg_Path_Data[indPath][indFrame].Pos_y_prev = vec0.y;
				vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
				pg_Path_Data[indPath][indFrame].Pos_x = vec0.x;
				pg_Path_Data[indPath][indFrame].Pos_y = vec0.y;
				// printf("line to %d %f %f\n" , indFrame ,
				// 	 pg_Path_Pos_x[ indPath ][ indFrame ] , 
				// 	 pg_Path_Pos_y[ indPath ][ indFrame ]);

				precedingCurrentPoint[0] = currentPoint[0];
				precedingCurrentPoint[1] = currentPoint[1];
				indFrame++;
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (relative)
					currentPoint[0] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[0] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				if (curChar == ',') {
					curChar = pathString[indChar++];
					while (_SpaceChar(curChar)) {
						curChar = pathString[indChar++];
					}
				}
				else {
					sprintf(ErrorStr, "Expected char [%c] not found!", ','); ReportError(ErrorStr); throw 17;
				}
				if (relative)
					currentPoint[1] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}

				if (indFrame < max_mouse_recording_frames) {
					// copies current fixed parameters
					if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
						pg_Path_Data[indPath][indFrame].Color_r = path_r_color;
						pg_Path_Data[indPath][indFrame].Color_g = path_g_color;
						pg_Path_Data[indPath][indFrame].Color_b = path_b_color;
						pg_Path_Data[indPath][indFrame].Color_a = 1.0;
						pg_Path_Data[indPath][indFrame].BrushID = 0;
						pg_Path_Data[indPath][indFrame].RadiusX = pathRadius;
						pg_Path_Data[indPath][indFrame].RadiusY = pathRadius;
					}
					vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
					pg_Path_Data[indPath][indFrame].Pos_x_prev = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y_prev = vec0.y;
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					pg_Path_Data[indPath][indFrame].Pos_x = vec0.x;
					pg_Path_Data[indPath][indFrame].Pos_y = vec0.y;

					// left tangent of current point is preceding point
					// and right tangent of preceding point is the current point
					pg_Path_Data[indPath][indFrame].Pos_xL
						= pg_Path_Data[indPath][indFrame].Pos_x_prev;
					pg_Path_Data[indPath][indFrame].Pos_yL
						= pg_Path_Data[indPath][indFrame].Pos_y_prev;
					pg_Path_Data[indPath][indFrame].Pos_xR
						= pg_Path_Data[indPath][indFrame].Pos_x;
					pg_Path_Data[indPath][indFrame].Pos_yR
						= pg_Path_Data[indPath][indFrame].Pos_y;

					// printf("new line to %d %f %f\n" , indFrame ,
					// 	   pg_Path_Pos_x[ indPath ][ indFrame ] , 
					// 	   pg_Path_Pos_y[ indPath ][ indFrame ]);
					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					indFrame++;
				}
			}
		}
		break;
		case 'z':
		case 'Z':
		{
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}
			// printf("cut\n");
		}
		break;
		default:
		{
			sprintf(ErrorStr, "Unknown SVG path tag [%c] %d!", curChar, curChar); ReportError(ErrorStr); throw(100);
		}
		break;
		}
	}
	if (!withRecordingOfStrokeParameters) {
		pg_Path_Status[indPath].nbRecordedFrames = indFrame;
	}
}
void LoadPathColorsFromXML(string pathString, int indPath, int * nbRecordedFrames) {
	std::stringstream  sstream;
	sstream.clear();
	std::replace(pathString.begin(), pathString.end(), ',', ' ');
	sstream.str(pathString);
	unsigned int       indFrame = 0;
	while (sstream >> pg_Path_Data[indPath][indFrame].Color_r && sstream >> pg_Path_Data[indPath][indFrame].Color_g 
		&& sstream >> pg_Path_Data[indPath][indFrame].Color_b && sstream >> pg_Path_Data[indPath][indFrame].Color_a) {
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
}

void LoadPathBrushesFromXML(string pathString, int indPath, int * nbRecordedFrames) {
	std::stringstream  sstream;
	sstream.clear();
	std::replace(pathString.begin(), pathString.end(), ',', ' ');
	sstream.str(pathString);
	unsigned int       indFrame = 0;
	while (sstream >> pg_Path_Data[indPath][indFrame].BrushID && sstream >> pg_Path_Data[indPath][indFrame].RadiusX
		&& sstream >> pg_Path_Data[indPath][indFrame].RadiusY) {
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
}
void LoadPathTimeStampsFromXML(string pathString, int indPath, int * nbRecordedFrames) {
	std::stringstream  sstream;
	sstream.clear();
	sstream.str(pathString);
	//printf("TS string %.s\n", pathString.c_str());
	unsigned int       indFrame = 0;
	while (sstream >> pg_Path_Data[indPath][indFrame].TimeStamp) { 
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
	//for (int indFrame = 0; indFrame < nbRecordedFrames; indFrame++) {
	//	sstream >> pg_Path_Data[indPath][indFrame].TimeStamp;
	//	//printf("TS %.2f\n", pg_Path_Data[indPath][indFrame].TimeStamp);
	//}
}

//////////////////////////////////////////////////////////////////
// CONVEX HULL 
//////////////////////////////////////////////////////////////////
#ifdef PG_BEZIER_PATHS
bool pointEquals(glm::vec2 p, glm::vec2 q) {
	return p.x == q.x && p.y == q.y;
};
float distance(glm::vec2 p, glm::vec2 q) {
	float dx = (p.x - q.x);
	float dy = (p.y - q.y);
	return sqrt(dx*dx + dy*dy);
};
bool left_oriented(glm::vec2 p1, glm::vec2 p2, glm::vec2 candidate) {
	float det = (p2.x - p1.x) * (candidate.y - p1.y)
		- (candidate.x - p1.x) * (p2.y - p1.y);
	if (det > 0) return true;  // left-oriented 
	if (det < 0) return false; // right oriented
	// select the farthest point in case of colinearity
	return distance(p1, candidate) > distance(p1, p2);
}
void convex_hull(glm::vec2 control_points[4], int *hull) {
	// get leftmost point
	int min = 0;
	hull[0] = -1;
	for (int i = 1; i < 4; i++) {
		if (control_points[i].y < control_points[min].y) {
			min = i;
		}
		hull[i] = -1;
	}

	int hull_point = min;
	int end_point;
	int indHull = 0;
	// walk the hull
	do
	{
		hull[indHull++] = hull_point;

		end_point = 0;
		for (int i = 1; i < 4; i++) {
			if (pointEquals(control_points[hull_point], control_points[end_point])
				|| left_oriented(control_points[hull_point], control_points[end_point], control_points[i])) {
				end_point = i;
			}
		}
		hull_point = end_point;
	}
	/*
	 * must compare coordinates values (and not simply objects)
	 * for the case of 4 co-incident points
	 */
	while (!pointEquals(control_points[end_point], control_points[hull[0]]));
}

void hull_expanded_by_radius(glm::vec2 control_points[4], int *hull,
	float radius, glm::vec2 hull_points[4]) {
	// copies the coordinates of the hull points
	glm::vec2 inner_hull_points[4];
	int nb_hull_points = 0;
	for (int i = 0; i < 4; i++) {
		if (hull[i] >= 0 && hull[i] < 4) {
			inner_hull_points[i] = control_points[hull[i]];
			nb_hull_points++;
		}
		else {
			inner_hull_points[i] = glm::vec2(PG_OUT_OF_SCREEN_CURSOR, PG_OUT_OF_SCREEN_CURSOR);
		}
		hull_points[i].x = PG_OUT_OF_SCREEN_CURSOR;
		hull_points[i].y = PG_OUT_OF_SCREEN_CURSOR;
	}
	// expands each segment by its normal and looks for the intersection of the expanded segments, as new hull points
	// (1) quad or triangle
	if (nb_hull_points >= 3) {
		for (int i = 0; i < 4; i++) {
			if (inner_hull_points[i].x != PG_OUT_OF_SCREEN_CURSOR
				&& inner_hull_points[i].y != PG_OUT_OF_SCREEN_CURSOR) {
				int next_i = (i + 1) % 4;
				int prev_i = (i - 1 + 4) % 4;
				while ((inner_hull_points[next_i].x == PG_OUT_OF_SCREEN_CURSOR
					|| inner_hull_points[next_i].y == PG_OUT_OF_SCREEN_CURSOR)
					&& next_i != i) {
					next_i = (next_i + 1) % 4;
				}
				while ((inner_hull_points[prev_i].x == PG_OUT_OF_SCREEN_CURSOR
					|| inner_hull_points[prev_i].y == PG_OUT_OF_SCREEN_CURSOR)
					&& prev_i != i) {
					prev_i = (prev_i - 1 + 4) % 4;
				}
				if (next_i != i && next_i != prev_i && prev_i != i) {
					//printf("\nPrev - loc - next %d - %d - %d\n", prev_i, i, next_i);
					glm::vec2 next_normal = inner_hull_points[next_i] - inner_hull_points[i];
					next_normal = glm::vec2(-next_normal.y, next_normal.x);
					float n = float(glm::length(next_normal));
					if (n != 0) {
						next_normal /= n;
					}
					glm::vec2 prev_normal = inner_hull_points[i] - inner_hull_points[prev_i];
					prev_normal = glm::vec2(-prev_normal.y, prev_normal.x);
					n = float(glm::length(prev_normal));
					if (n != 0) {
						prev_normal /= n;
					}
					glm::vec2 expanded_prev_points[2];
					expanded_prev_points[0] = inner_hull_points[prev_i] + radius * prev_normal;
					expanded_prev_points[1] = inner_hull_points[i] + radius * prev_normal;
					glm::vec2 expanded_next_points[2];
					expanded_next_points[0] = inner_hull_points[i] + radius * next_normal;
					expanded_next_points[1] = inner_hull_points[next_i] + radius * next_normal;
					//printf("Expanded points %.2f,%.2f   %.2f,%.2f   %.2f,%.2f   %.2f,%.2f \n",
					//	expanded_prev_points[0].x, expanded_prev_points[0].y,
					//	expanded_prev_points[1].x, expanded_prev_points[1].y,
					//	expanded_next_points[0].x, expanded_next_points[0].y,
					//	expanded_next_points[1].x, expanded_next_points[1].y);
					// D_next next_normal.x * x + next_normal.y * y 
					//        = next_normal.x * expanded_next_points[0].x + next_normal.y * expanded_next_points[0].y
					float det_normals = prev_normal.x * next_normal.y - next_normal.x * prev_normal.y;
					float c_prev = prev_normal.x * expanded_prev_points[0].x + prev_normal.y * expanded_prev_points[0].y;
					float c_next = next_normal.x * expanded_next_points[0].x + next_normal.y * expanded_next_points[0].y;
					if (det_normals != 0) {
						hull_points[i].x = (c_prev * next_normal.y - c_next * prev_normal.y) / det_normals;
						hull_points[i].y = (prev_normal.x * c_next - next_normal.x * c_prev) / det_normals;
						//printf("hull_point %d %.2f,%.2f \n", i,
						//	hull_points[i].x, hull_points[i].y);
					}
				}
			}
		}
	}
	// (2) segment
	else if (nb_hull_points == 2) {
		// builds a square around the point
		glm::vec2 segment_points[2];
		int ind_segment_points = 0;
		for (int i = 0; i < 4; i++) {
			if (inner_hull_points[i].x != PG_OUT_OF_SCREEN_CURSOR && inner_hull_points[i].y != PG_OUT_OF_SCREEN_CURSOR) {
				segment_points[ind_segment_points++] = inner_hull_points[i];
				if (ind_segment_points == 2) {
					break;
				}
			}
		}
		glm::vec2 seg_vec = segment_points[1] - segment_points[0];
		float n = float(glm::length(seg_vec));
		if (n != 0) {
			seg_vec /= n;
		}
		else { // point
			hull_points[0] = segment_points[0] + glm::vec2(radius, radius);
			hull_points[1] = segment_points[0] + glm::vec2(radius, -radius);
			hull_points[2] = segment_points[0] + glm::vec2(-radius, -radius);
			hull_points[3] = segment_points[0] + glm::vec2(-radius, radius);
			return;
		}
		glm::vec2 normal = glm::vec2(-seg_vec.y, seg_vec.x);
		hull_points[0] = segment_points[0] - radius * 1.5f * seg_vec - radius * 1.5f * normal;
		hull_points[1] = segment_points[0] - radius * 1.5f * seg_vec + radius * 1.5f * normal;
		hull_points[2] = segment_points[1] + radius * 1.5f * seg_vec + radius * 1.5f * normal;
		hull_points[3] = segment_points[1] + radius * 1.5f * seg_vec - radius * 1.5f * normal;
	}
	// (1) point
	else if (nb_hull_points == 1) {
		// builds a square around the point
		for (int i = 0; i < 4; i++) {
			if (inner_hull_points[i].x != PG_OUT_OF_SCREEN_CURSOR && inner_hull_points[i].y != PG_OUT_OF_SCREEN_CURSOR) {
				hull_points[0] = inner_hull_points[i] + glm::vec2(radius * 1.5f, radius * 1.5f);
				hull_points[1] = inner_hull_points[i] + glm::vec2(radius * 1.5f, -radius * 1.5f);
				hull_points[2] = inner_hull_points[i] + glm::vec2(-radius * 1.5f, -radius * 1.5f);
				hull_points[3] = inner_hull_points[i] + glm::vec2(-radius * 1.5f, radius * 1.5f);
			}
		}
	}
}

void boundingBox_expanded_by_radius(glm::vec2 control_points[4], 
	float radius, glm::vec4 *boundingBox) {
	// coords min/max
	float min_x = control_points[0].x;
	float max_x = control_points[0].x;
	float min_y = control_points[0].y;
	float max_y = control_points[0].y;
	for (int i = 1; i < 4; i++) {
		if (control_points[i].x != PG_OUT_OF_SCREEN_CURSOR
			&& control_points[i].y != PG_OUT_OF_SCREEN_CURSOR) {
			if (control_points[i].x < min_x) {
				min_x = control_points[i].x;
			}
			else if (control_points[i].x > max_x) {
				max_x = control_points[i].x;
			}
			if (control_points[i].y < min_y) {
				min_y = control_points[i].y;
			}
			else if (control_points[i].y > max_y) {
				max_y = control_points[i].y;
			}
		}
	}
	// expands the bounding box by 1.5 the radius
	(*boundingBox).x = min_x - 1.5f * radius;
	(*boundingBox).y = max_x + 1.5f * radius;
	(*boundingBox).z = min_y - 1.5f * radius;
	(*boundingBox).w = max_y + 1.5f * radius;
}


void build_bounding_box(int indPath) {
	if (indPath > PG_NB_PATHS) {
		return;
	}

	// convex hull shipped to the GPU
	pg_BezierControl[indPath * 4 + 0] = glm::vec2(paths_x_prev[indPath], paths_y_prev[indPath]);
	pg_BezierControl[indPath * 4 + 1] = glm::vec2(paths_xL[indPath], paths_yL[indPath]);
	pg_BezierControl[indPath * 4 + 2] = glm::vec2(paths_xR[indPath], paths_yR[indPath]);
	if (indPath < PG_NB_CURSORS_MAX) {
		pg_BezierControl[indPath * 4 + 3] = glm::vec2(paths_x_forGPU[indPath], paths_y_forGPU[indPath]);
	}
	else {
		pg_BezierControl[indPath * 4 + 3] = glm::vec2(paths_x[indPath], paths_y[indPath]);
	}

	// BOUNDING SOLUTION
	// alternative possibility with a simple bounding box around the stroke
	boundingBox_expanded_by_radius(&(pg_BezierControl[indPath * 4]),
		paths_RadiusX[indPath] * 1.1f,
		&(pg_BezierBox[indPath]));
}

void build_expanded_hull(int indPath) {
	if (indPath > PG_NB_PATHS) {
		return;
	}

	// convex hull shipped to the GPU
	pg_BezierControl[indPath * 4 + 0] = glm::vec2(paths_x_prev[indPath], paths_y_prev[indPath]);
	pg_BezierControl[indPath * 4 + 1] = glm::vec2(paths_xL[indPath], paths_yL[indPath]);
	pg_BezierControl[indPath * 4 + 2] = glm::vec2(paths_xR[indPath], paths_yR[indPath]);
	if (indPath < PG_NB_CURSORS_MAX) {
		pg_BezierControl[indPath * 4 + 3] = glm::vec2(paths_x_forGPU[indPath], paths_y_forGPU[indPath]);
	}
	else {
		pg_BezierControl[indPath * 4 + 3] = glm::vec2(paths_x[indPath], paths_y[indPath]);
	}

	// CONVEX HULL SOLUTION
	// checks whether the point is inside the convex hull of the control points
	// the test is made by counting how many times a horizontal 1/2 line from the point
	// cuts the hull, odd: outside, even: inside 
	// checks whether current point is inside the hull of the bezier curve 
	// and takes into consideration the radius

	// does not work well because the points are often almost aligned resulting in a 
	int path_next_in_hull[4];
	for (int indPt = 0; indPt < 4; indPt++) {
		path_next_in_hull[indPt] = -1;
	}
	convex_hull(&(pg_BezierControl[indPath * 4]), path_next_in_hull);
	hull_expanded_by_radius(&(pg_BezierControl[indPath * 4]),
		path_next_in_hull,
		paths_RadiusX[indPath] * 1.5f,
		&(pg_BezierHull[indPath * 4])); 

	// trace in the log file

	if (indPath == 0) {
		//fprintf(pg_csv_file, "Points %.2f,%.2f   %.2f,%.2f   %.2f,%.2f   %.2f,%.2f (radius %.2f)\n",
		//	pg_BezierControl[indPath * 4 + 0].x, pg_BezierControl[indPath * 4 + 0].y,
		//	pg_BezierControl[indPath * 4 + 1].x, pg_BezierControl[indPath * 4 + 1].y,
		//	pg_BezierControl[indPath * 4 + 2].x, pg_BezierControl[indPath * 4 + 2].y,
		//	pg_BezierControl[indPath * 4 + 3].x, pg_BezierControl[indPath * 4 + 3].y,
		//	paths_RadiusX[indPath]);
		//fprintf(pg_csv_file, "Hull %d %d %d %d\n", path_next_in_hull[0], path_next_in_hull[1], path_next_in_hull[2], path_next_in_hull[3]);
		//fprintf(pg_csv_file, "Hull points %.2f,%.2f   %.2f,%.2f   %.2f,%.2f   %.2f,%.2f \n\n",
		//	pg_BezierHull[indPath * 4 + 0].x, pg_BezierHull[indPath * 4 + 0].y,
		//	pg_BezierHull[indPath * 4 + 1].x, pg_BezierHull[indPath * 4 + 1].y,
		//	pg_BezierHull[indPath * 4 + 2].x, pg_BezierHull[indPath * 4 + 2].y,
		//	pg_BezierHull[indPath * 4 + 3].x, pg_BezierHull[indPath * 4 + 3].y);
	}

}

void test_hull(void) {
	glm::vec2 control_points[4];
	glm::vec2 hull_points[4];
	int path_next_in_hull[4] = { -1,-1,-1,-1 };
	float rad = 1.0;
	hull_points[0].x = 0.0f; hull_points[0].y = 0.0f;
	hull_points[1].x = 0.0f; hull_points[1].y = 0.0f;
	hull_points[2].x = 0.0f; hull_points[2].y = 0.0f;
	hull_points[3].x = 0.0f; hull_points[3].y = 0.0f;

	control_points[0].x = 5.0f; control_points[0].y = 5.0f;
	control_points[1].x = 3.0f; control_points[1].y = 7.0f;
	control_points[2].x = 9.0f; control_points[2].y = 6.0f;
	control_points[3].x = 4.0f; control_points[3].y = 10.0f;
	convex_hull(control_points, path_next_in_hull);
	hull_expanded_by_radius(control_points,
		path_next_in_hull,
		rad,
		hull_points);
	printf("Points %.2f,%.2f   %.2f,%.2f   %.2f,%.2f   %.2f,%.2f \n",
		control_points[0].x, control_points[0].y,
		control_points[1].x, control_points[1].y,
		control_points[2].x, control_points[2].y,
		control_points[3].x, control_points[3].y);
	printf("Hull %d %d %d %d\n", path_next_in_hull[0], path_next_in_hull[1], path_next_in_hull[2], path_next_in_hull[3]);
	printf("Hull points %.2f,%.2f   %.2f,%.2f   %.2f,%.2f   %.2f,%.2f \n",
		hull_points[0].x, hull_points[0].y,
		hull_points[1].x, hull_points[1].y,
		hull_points[2].x, hull_points[2].y,
		hull_points[3].x, hull_points[3].y);

	int countIntersections = 0;
	glm::vec2 PixelLocation = glm::vec2(2.0f, 9.0f);
	for (int i = 0; i < 4; i++) {
		// quad or triangle if one of the points is inside the triangle defined by the 3 others
		// the non out of screen points in the hull are given first
		int indHull = (i + 1) % 4;
		float xA = hull_points[i].x;
		float yA = hull_points[i].y;
		float xB = hull_points[indHull].x;
		float yB = hull_points[indHull].y;
		while (indHull != 0 && xB == PG_OUT_OF_SCREEN_CURSOR && yB == PG_OUT_OF_SCREEN_CURSOR) {
			indHull = (indHull + 1) % 4;
			xB = hull_points[indHull].x;
			yB = hull_points[indHull].y;
		}

		if (yA != yB) {
			float alpha_intersection = (PixelLocation.y  - yA) / (yB - yA);
			if (alpha_intersection >= 0 && alpha_intersection <= 1) {
				float x_intersection = (1 - alpha_intersection) * xA + alpha_intersection * xB;
				if (PixelLocation.x <= x_intersection ) {
					printf("Ind segment %d - %d x intersection %.2f alpha %.2f\n", i, indHull, x_intersection, alpha_intersection);
					countIntersections++;
				}
				else {
					printf("Ind segment %d - %d x intersection %.2f alpha %.2f\n", i, indHull, x_intersection, alpha_intersection);
				}
			}
			else {
				printf("Ind segment %d - %d alpha %.2f\n", i, indHull, alpha_intersection);
			}
		}
		else {
			if (PixelLocation.y == yA
				&& (PixelLocation.x < xA) != (PixelLocation.x < xB)) {
				countIntersections++;
			}
		}
		if (indHull == 0) {
			break;
		}
	}
	// odd number of intersections -> outside the hull
	if (countIntersections % 2 == 0) {
		printf("Out\n");
	}
	else {
		printf("in\n");
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////////
// scene update
//////////////////////////////////////////////////////////////////
// REPLAY PATHS  
//////////////////////////////////////////////////////////////////

// replays a path with the same duration
// however since the time stamps are not saved in the SVG file, the 
// is uniform and does not match the exact initial speed 
// to make it better synchronized, it would be necessary
// to store the time stamp of each curve inside the SVG file
#define PG_SYNC_REPLAY 

// calculation of tangents from successive locations of the pen
void stroke_geometry_calculation(int indPath, int curr_position_x, int curr_position_y) {
	paths_time_prev_prev[indPath] = paths_time_prev[indPath];
	paths_x_prev_prev[indPath] = paths_x_prev[indPath];
	paths_y_prev_prev[indPath] = paths_y_prev[indPath];

	paths_time_prev[indPath] = paths_time[indPath];
	paths_x_prev[indPath] = paths_x[indPath];
	paths_y_prev[indPath] = paths_y[indPath];

	paths_time[indPath] = float(CurrentClockTime);
	paths_x[indPath] = paths_x_next[indPath];
	paths_y[indPath] = paths_y_next[indPath];
	if (indPath < PG_NB_CURSORS_MAX) {
		paths_x_forGPU[indPath] = paths_x[indPath];
		paths_y_forGPU[indPath] = paths_y[indPath];
	}

	paths_x_next[indPath] = float(curr_position_x);
	paths_y_next[indPath] = float(curr_position_y);
	// printf("/abs_pen_xy %.0f %.0f\n", float(curr_position_x), float(curr_position_y));
	//printf("stroke_geometry_calculation for path %d (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f)\n", indPath, paths_x_prev_prev[indPath], paths_y_prev_prev[indPath], paths_x_prev[indPath], paths_y_prev[indPath], paths_x[indPath], paths_y[indPath], paths_x_next[indPath], paths_y_next[indPath]);

#ifdef KOMPARTSD
		// sends the position of the cursor to the recorder for later replay
	pg_IPClient * client;
	sprintf(AuxString, "/abs_pen_xy %.0f %.0f", float(CurrentMousePos_x[0]), float(CurrentMousePos_y[0]));
	if ((client = pg_UDP_client((char *)"udp_Record_send"))) {
		pg_send_message_udp((char *)"ff", (char *)AuxString, client);
	}
#endif

	// define the tangents
	// in the future, keep the previous tangent memory so that it is not recomputed twice

	// segments between two positions
	float u_x = (paths_x_prev[indPath] - paths_x_prev_prev[indPath]);
	float u_y = (paths_y_prev[indPath] - paths_y_prev_prev[indPath]);
	float v_x = (paths_x[indPath] - paths_x_prev[indPath]);
	float v_y = (paths_y[indPath] - paths_y_prev[indPath]);
	float w_x = (paths_x_next[indPath] - paths_x[indPath]);
	float w_y = (paths_y_next[indPath] - paths_y[indPath]);

	// normalized segments
	float norm_u = sqrt(u_x * u_x + u_y * u_y);
	float norm_v = sqrt(v_x * v_x + v_y * v_y);
	float norm_w = sqrt(w_x * w_x + w_y * w_y);
	if (norm_u != 0) {
		u_x /= norm_u;
		u_y /= norm_u;
	}
	if (norm_v != 0) {
		v_x /= norm_v;
		v_y /= norm_v;
	}
	if (norm_w != 0) {
		w_x /= norm_w;
		w_y /= norm_w;
	}

	// tangents for current and preceding positions as othogonal vector to bissectrix
	// their length is a third of the current segment
	float tang_x_prev = (u_x + v_x);
	float tang_y_prev = (u_y + v_y);
	float tang_x = (v_x + w_x);
	float tang_y = (v_y + w_y);
	float norm_tang_prev = sqrt(tang_x_prev * tang_x_prev + tang_y_prev * tang_y_prev);
	float norm_tang = sqrt(tang_x * tang_x + tang_y * tang_y);
	norm_v /= 3.f; // tangents are the third of the length of the segment
	if (norm_tang_prev != 0 && norm_v != 0) {
		tang_x_prev = tang_x_prev * norm_v / norm_tang_prev;
		tang_y_prev = tang_y_prev * norm_v / norm_tang_prev;
	}
	if (norm_tang != 0 && norm_v != 0) {
		tang_x = tang_x * norm_v / norm_tang;
		tang_y = tang_y * norm_v / norm_tang;
	}

	// control points from positions and tangents for current and preceding positions
#if defined(PG_BEZIER_PATHS)
	paths_xL[indPath] = paths_x_prev[indPath] + tang_x_prev;
	paths_yL[indPath] = paths_y_prev[indPath] + tang_y_prev;
	paths_xR[indPath] = paths_x[indPath] - tang_x;
	paths_yR[indPath] = paths_y[indPath] - tang_y;

	// possible resize to avoid crossing
	// non-crossing <=> both points prev and left on the same size of the line (right, current)
	// if N is the normal to (right, current), (vec(curr prev).N) * (vec(curr L).N) >= 0
	int n = 4;
	while ((tang_y * (paths_x_prev[indPath] - paths_x[indPath]) - tang_x * (paths_y_prev[indPath] - paths_y[indPath])) // vec(curr prev).N
		* (tang_y * (paths_xL[indPath] - paths_x[indPath]) - tang_x * (paths_yL[indPath] - paths_y[indPath])) // vec(curr L).N
		< 0 && n > 0) {
		// tangents are made shorter
		tang_x_prev /= 2.f;
		tang_y_prev /= 2.f;
		tang_x /= 2.f;
		tang_y /= 2.f;

		// control points from positions and tangents for current and preceding positions
		paths_xL[indPath] = paths_x_prev[indPath] + tang_x_prev;
		paths_yL[indPath] = paths_y_prev[indPath] + tang_y_prev;
		paths_xR[indPath] = paths_x[indPath] - tang_x;
		paths_yR[indPath] = paths_y[indPath] - tang_y;

		n--;
	}
#ifdef PIERRES
	// random angle for cristal effect
	if (pen_angle_pulse > 0) {
		paths_x_prev[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_y_prev[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_x_next[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_y_next[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_x[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_y[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_xL[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_yL[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_xR[indPath] += rand_0_1 * pen_angle_pulse * 10;
		paths_yR[indPath] += rand_0_1 * pen_angle_pulse * 10;
	}
#endif
#endif

#ifndef PG_TACTILE_TABLET
	// line begin or line end between inscreen and offscreen positions
	// off screen positions also correspond to mouseover flights
	// begin
	if (paths_x_prev_prev[indPath] == PG_OUT_OF_SCREEN_CURSOR && paths_y_prev_prev[indPath] == PG_OUT_OF_SCREEN_CURSOR
		&& paths_x_prev[indPath] >= 0 && paths_y_prev[indPath] >= 0 && paths_x[indPath] >= 0 && paths_y[indPath] >= 0) {
		isBegin[indPath] = true;
	}
	else {
		isBegin[indPath] = false;
	}
	// end
	if (paths_x_prev[indPath] >= 0 && paths_y_prev[indPath] >= 0 && paths_x[indPath] >= 0 && paths_y[indPath] >= 0
		&& paths_x_next[indPath] == PG_OUT_OF_SCREEN_CURSOR && paths_y_next[indPath] == PG_OUT_OF_SCREEN_CURSOR) {
		isEnd[indPath] = true;
	}
	else {
		isEnd[indPath] = false;
	}
#else
	// line begin or line end between two distant points or for a shorter distance but with a long waiting time
	glm::vec2 prev = glm::vec2(paths_x_prev[indPath], paths_y_prev[indPath]);
	glm::vec2 cur = glm::vec2(paths_x[indPath], paths_y[indPath]);
	glm::vec2 next = glm::vec2(paths_x_next[indPath], paths_y_next[indPath]);
	//printf("dist prev %.2f cur %.2f\n", distance(prev, cur), distance(cur, next));

	// begin
	//printf("distance %.2f %.2f temps %.2f\n", distance(prev, cur), distance(next, cur), CurrentClockTime - LastCursorPositionUpdate[indPath]);
	if (distance(prev, cur) > 200.0f || (distance(prev, cur) > 10.0f && (CurrentClockTime - LastCursorPositionUpdate[indPath] > 0.3))) {
		isBegin[indPath] = true;
		paths_x_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR; paths_y_prev_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		//printf("********** BEGIN *************\n");
	}
	else {
		isBegin[indPath] = false;
	}
	// end
	if (distance(cur, next) > 200.0f || (distance(cur, next) > 10.0f && (CurrentClockTime - LastCursorPositionUpdate[indPath] > 0.3))) {
		isEnd[indPath] = true;
		paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR; paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		//printf("********** END *************\n");
	}
	else {
		isEnd[indPath] = false;
	}
	LastCursorPositionUpdate[indPath] = float(CurrentClockTime);
#endif
}

// replays a path that has been loaded or previously recorded
void pg_replay_one_path(int pathNo, double theTime) {
	//printf("read track %d\n" , pathNo );
// int indFrameReading = pg_Path_Status[ pathNo ].indReading;

	// does not replay a path if the path replay is active for a multitouch interaction
	if (pg_Path_Status[pathNo].nbRecordedFrames <= 0) {
		return;
	}

	// records the initial time and uses the elapsed reading
	// time and the elapsed recording time to play in synch
	if (pg_Path_Status[pathNo].isFirstFrame) {
		pg_Path_Status[pathNo].initialTimeReading = theTime; // -pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].TimeStamp + pg_Path_Status[pathNo].initialTimeRecording;
		pg_Path_Status[pathNo].isFirstFrame = false;
		pg_indPreviousFrameReading[pathNo] = 0;
		//printf("Initial time reading %.2f time %.2f path first time stamp %.2f path initial time recording %.2f\n", pg_Path_Status[pathNo].initialTimeReading, 
		//	theTime, pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].TimeStamp, pg_Path_Status[pathNo].initialTimeRecording);
	}

	bool isCurveBreakBegin = false;
	bool isCurveBreakEnd = false;
#ifdef PG_SYNC_REPLAY
	double theRecordingElapsedTime;
	double theReadingElapsedTime;

	do {
		theRecordingElapsedTime =
			pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].TimeStamp
			- pg_Path_Status[pathNo].initialTimeRecording;
		theReadingElapsedTime =
			(theTime - pg_Path_Status[pathNo].initialTimeReading)
			* pg_Path_Status[pathNo].readSpeedScale;
		// printf( "Ind %d rec time %.2f read time %.2f\n" , pg_Path_Status[ pathNo ].indReading ,theRecordingElapsedTime , theReadingElapsedTime );
		// the negtive values correspond to a curve break. If they are jumped over the first 
		// following point should be negative
		if (pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].Pos_x < 0
			|| pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].Pos_y < 0) {
			isCurveBreakEnd = true;
		}
		if (pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].Pos_x_prev < 0
			|| pg_Path_Data[pathNo][pg_Path_Status[pathNo].indReading].Pos_y_prev < 0) {
			isCurveBreakBegin = true;
		}
		if (theRecordingElapsedTime < theReadingElapsedTime) {
			pg_Path_Status[pathNo].indReading++;
		}
		else {
			break;
		}

		if (path_replay_loop == true) {
			// loops at the end
			if (pg_Path_Status[pathNo].indReading >= pg_Path_Status[pathNo].nbRecordedFrames) {
				pg_Path_Status[pathNo].indReading = 0;
				pg_Path_Status[pathNo].isFirstFrame = true;
				isCurveBreakEnd = true;
				break;
			}
		}
		else {
			// stops at the end
			if (pg_Path_Status[pathNo].indReading
				>= pg_Path_Status[pathNo].nbRecordedFrames) {
				pg_Path_Status[pathNo].indReading = pg_Path_Status[pathNo].nbRecordedFrames - 1;
				isCurveBreakEnd = false;
				break;
			}
		}
	} while (true);
	//printf("theRecordingElapsedTime %.2f theReadingElapsedTime %.2f ind reading %d\n", theRecordingElapsedTime, theReadingElapsedTime, pg_Path_Status[pathNo].indReading);
#else
	// the negtive values correspond to a curve break. If they are jumped over the first 
	// following point should be negative
	if (pg_Path_Data[pathNo].Pos_x[pg_Path_Status[pathNo].indReading] < 0
		|| pg_Path_Data[pathNo].Pos_y[pg_Path_Status[pathNo].indReading] < 0) {
		isCurveBreakEnd = true;
	}
	if (pg_Path_Data[pathNo].Pos_x_prev[pg_Path_Status[pathNo].indReading] < 0
		|| pg_Path_Data[pathNo].Pos_y_prev[pg_Path_Status[pathNo].indReading] < 0) {
		isCurveBreakBegin = true;
	}

	if (path_replay_loop == true) {
		// loops at the end
		if (pg_Path_Status[pathNo].indReading
			>= pg_Path_Status[pathNo].nbRecordedFrames) {
			pg_Path_Status[pathNo].indReading = 0;
			isCurveBreakEnd = true;
			break;
		}
	}
	else {	// stops at the end
		if (pg_Path_Status[pathNo].indReading
			>= pg_Path_Status[pathNo].nbRecordedFrames) {
			pg_Path_Status[pathNo].indReading = pg_Path_Status[pathNo].nbRecordedFrames - 1;
			isCurveBreakEnd = false;
			break;
		}
	}
	pg_Path_Status[pathNo].indReading++;
#endif

	///////////////////////////////////////////////////////////
	// previous frame for tangent and position
	// management of previous mouse position with
	// consideration for looping
	int indFrameReading = pg_Path_Status[pathNo].indReading;
	if (isCurveBreakBegin) {
		//pg_indPreviousFrameReading[pathNo] = 0;
		//indFrameReading = pg_indPreviousFrameReading[pathNo] + 1;
		paths_x_prev[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
#ifdef PG_BEZIER_PATHS
		paths_xL[pathNo] = 0.f;
		paths_yL[pathNo] = 0.f;
#endif
	}
	else {
		paths_x_prev[pathNo] = (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_x;
		paths_y_prev[pathNo] = (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_y;
#ifdef PG_BEZIER_PATHS
		paths_xL[pathNo] = 2 * (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_x - (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_xR;
		paths_yL[pathNo] = 2 * (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_y - (float)pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo]].Pos_yR;
#endif
	}
	//printf("prev frame %d curr frame %d\n", pg_indPreviousFrameReading[pathNo], indFrameReading);

	///////////////////////////////////////////////////////////
	// next frame for tangent and position
	// negative values in case of curve break
	if (isCurveBreakEnd) {
#ifdef PG_BEZIER_PATHS
		paths_xR[pathNo] = 0.f;
		paths_yR[pathNo] = 0.f;
#endif
		paths_x[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
	}
	else {
#ifdef PG_BEZIER_PATHS
		paths_xR[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Pos_xR;
		paths_yR[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Pos_yR;
#endif
		paths_x[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Pos_x;
		paths_y[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Pos_y;
	}

	///////////////////////////////////////////////////////////
	// line begin or line end
	// begin
	if ((pg_indPreviousFrameReading[pathNo] > 0
		&& pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo] - 1].Pos_x < 0
		&& pg_Path_Data[pathNo][pg_indPreviousFrameReading[pathNo] - 1].Pos_y < 0)
		&& paths_x_prev[pathNo] >= 0 && paths_y_prev[pathNo] >= 0 && paths_x[pathNo] >= 0 && paths_y[pathNo] >= 0) {
		isBegin[pathNo] = true;
	}
	else {
		isBegin[pathNo] = false;
	}

	// end
	if (paths_x_prev[pathNo] >= 0 && paths_y_prev[pathNo] >= 0 && paths_x[pathNo] >= 0 && paths_y[pathNo] >= 0
		&& pg_Path_Data[pathNo][(indFrameReading + 1) % pg_Path_Status[pathNo].nbRecordedFrames].Pos_x < 0
		&& pg_Path_Data[pathNo][(indFrameReading + 1) % pg_Path_Status[pathNo].nbRecordedFrames].Pos_y < 0) {
		isEnd[pathNo] = true;
	}
	else {
		isEnd[pathNo] = false;
	}

	//printf("B0 %.2f %.2f  B1 %.2f %.2f  B2 %.2f %.2f B3 %.2f %.2f \n\n", paths_x_prev[pathNo], paths_y_prev[pathNo], paths_xL[pathNo], paths_yL[pathNo], 
	//	paths_xR[pathNo], paths_yR[pathNo], paths_x[pathNo], paths_y[pathNo]);

	// management of color (w/wo possible interpolation)
	if (pen_saturation_replay == 0 && pen_saturation_replay_pulse == 0
		&& pen_hue_replay == 0 && pen_hue_replay_pulse == 0
		&& pen_value_replay == 0 && pen_value_replay_pulse == 0) {
		paths_Color_r[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Color_r;
		paths_Color_g[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Color_g;
		paths_Color_b[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Color_b;
		//printf("RGB: %.2f %.2f %.2f \n", paths_Color_r[pathNo], paths_Color_g[pathNo], paths_Color_b[pathNo]);
	}
	else {
		float r = (float)pg_Path_Data[pathNo][indFrameReading].Color_r;
		float g = (float)pg_Path_Data[pathNo][indFrameReading].Color_g;
		float b = (float)pg_Path_Data[pathNo][indFrameReading].Color_b;

		float h, s, v;
		RGBtoHSV(r, g, b, &h, &s, &v);

		s += pen_saturation_replay
			* (1.f + pulse_average * pen_saturation_replay_pulse);
		v += pen_value_replay
			* (1.f + pulse_average * pen_value_replay_pulse);
		h += pen_hue_replay
			* (1.f + pulse_average * pen_hue_replay_pulse);
		s = max(min(s, 1.f), 0.f);
		v = max(min(v, 1.f), 0.f);
		h = max(min(h, 1.f), 0.f);
#if !defined (LIGHT)
		sprintf(AuxString, "/pen_hue_replay %.5f", h); pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("%s\n", AuxString);
		sprintf(AuxString, "/pen_value_replay %.5f", v); pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("%s\n", AuxString);
		sprintf(AuxString, "/pen_saturation_replay %.5f", s); pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_TouchOSC_send");
		//printf("%s\n", AuxString);
#endif
		HSVtoRGB(h, s, v, &paths_Color_r[pathNo], &paths_Color_g[pathNo], &paths_Color_b[pathNo]);

		//printf("RGB: In %.2f %.2f %.2f OUT SV %.2f %.2f OUT %.2f %.2f %.2f \n", r, g, b, s, v, paths_Color_r[pathNo], paths_Color_g[pathNo], paths_Color_b[pathNo]);
	}
	paths_Color_a[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].Color_a;

	//if (pg_indPreviousFrameReading[pathNo] < indFrameReading - 1) {
	//	paths_Color_r[pathNo] = 1.f;
	//	paths_Color_g[pathNo] = 0.f;
	//	paths_Color_b[pathNo] = 0.f;
	//}

	// management of brush ID (w/wo possible interpolation)
	int brushNo = pg_Path_Data[pathNo][indFrameReading].BrushID + pen_brush_replay;
	while (brushNo < 0) {
		brushNo += (nb_pen_brushes * 3);
	}
	paths_BrushID[pathNo] = brushNo % (nb_pen_brushes * 3);
	// management of brush radius (w/wo possible interpolation)
	paths_RadiusX[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].RadiusX * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);
	paths_RadiusY[pathNo] = (float)pg_Path_Data[pathNo][indFrameReading].RadiusY * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);

	//if (is_path_replay[pathNo]) {
	//	printf("replay #%d %.2f %.2f rad %.2f pen_radius_replay %.2f\n", pathNo,
	//		paths_x[pathNo],
	//		paths_y[pathNo], paths_RadiusX[pathNo], pen_radius_replay);
	//}

	pg_indPreviousFrameReading[pathNo] = indFrameReading;
}

// outputs the trace of a metawear sensor
#ifdef PG_METAWEAR
void pg_play_metawear_trajectory(int pathNo, int sensorNo) {
	if (!is_path_replay[pathNo]) {
		is_path_replay[pathNo] = true;
		// path 1 (senosr 1 sensorNo 0) and 2 (sensor 2 sensorNo 1) replays on currentDrawingTrack + 1
		pg_path_replay_trackNo_start(pathNo, currentDrawingTrack + 1);
		*((int*)ScenarioVarPointers[_path_replay_trackNo_1 + (pathNo - 1)]) = currentDrawingTrack + 1;
		printf("start replay path %d (sensor %d) on track %d\n", pathNo, pathNo, *((int*)ScenarioVarPointers[_path_replay_trackNo_1 + (pathNo - 1)]));
	}
	stroke_geometry_calculation(pathNo, int(pg_mw_sensors[sensorNo].mw_mss_pos[0]), int(pg_mw_sensors[sensorNo].mw_mss_pos[1]));
	//printf("new sensor position %d path %d: %.1f %.1f replay %d\n", sensorNo, pathNo, 
	//	(pg_mw_sensors[sensorNo].mw_mss_pos[0]), (pg_mw_sensors[sensorNo].mw_mss_pos[1]), int(is_path_replay[pathNo]));
	pg_mw_sensors[sensorNo].mw_mss_pos_update = false;

	// management of brush radius (w/wo possible interpolation)
	paths_RadiusX[pathNo] = pen_radius;
	paths_RadiusY[pathNo] = pen_radius;
	// management of brush radius (w/wo possible interpolation)
	paths_RadiusX[pathNo] = pen_radius * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);
	paths_RadiusY[pathNo] = pen_radius * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);}
#endif

// PATHS REPLAY
void pg_replay_paths(double theTime) {
	for (int indPath = 1; indPath <= PG_NB_PATHS ; indPath++) {
#ifdef PG_METAWEAR
		int ind_sensor = indPath - 1;
		if (ind_sensor < PG_MW_NB_MAX_SENSORS) {
			if (pg_mw_sensors[ind_sensor].mw_mss_pos_update == true) {
				pg_play_metawear_trajectory(indPath, ind_sensor);
			}
			continue;
		}
#endif

		// active reading
		if (is_path_replay[indPath]) {
			pg_replay_one_path(indPath, theTime);
		}
	}
}

//////////////////////////////////////////////////////////////////
// PEN STROKE AND PATH REPLAY  
//////////////////////////////////////////////////////////////////
void pg_update_pulsed_colors_and_replay_paths(double theTime) {
	// change colors according to music pulse
	pg_update_pulsed_colors();

	// replays recoded paths
	pg_replay_paths(theTime);

	///////////////////////////////////////////////////////////////////////
	// PARTICLE COLOR UPDATING INDEPENDENTLY OF TRACK READING OR WRITING
	///////////////////////////////////////////////////////////////////////
	/// uses pulsed color to repop
	repop_ColorBG_r
		= min(1.f, pulsed_repop_colorBG[0]);
	repop_ColorBG_g
		= min(1.f, pulsed_repop_colorBG[1]);
	repop_ColorBG_b
		= min(1.f, pulsed_repop_colorBG[2]);
	repop_ColorCA_r
		= min(1.f, pulsed_repop_colorCA[0]);
	repop_ColorCA_g
		= min(1.f, pulsed_repop_colorCA[1]);
	repop_ColorCA_b
		= min(1.f, pulsed_repop_colorCA[2]);
	repop_ColorPart_r
		= min(1.f, pulsed_repop_colorPart[0]);
	repop_ColorPart_g
		= min(1.f, pulsed_repop_colorPart[1]);
	repop_ColorPart_b
		= min(1.f, pulsed_repop_colorPart[2]);
	// printf("repop color %.2f %.2f %.2f \n", repop_ColorPart_r, repop_ColorPart_g, repop_ColorPart_b);
	if (invertAllLayers) {
	}

	///////////////////////////////////////////////////////////////////////
	// PEN PARAMETERS IN PATH 0-(PG_NB_CURSORS_MAX - 1)
	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// draws from mouse position 


	for (int indPath = 0; indPath < PG_NB_CURSORS_MAX; indPath++) {
		/// uses pulsed color to draw
		paths_Color_r[indPath] = min(1.f, pulsed_pen_color[0]);
		paths_Color_g[indPath] = min(1.f, pulsed_pen_color[1]);
		paths_Color_b[indPath] = min(1.f, pulsed_pen_color[2]);
		paths_Color_a[indPath] = min(1.f, pen_color_a);
		//printf("PEN color a %.2f %.2f %.2f %.2f\n", paths_Color_r[indPath] ,
		//	   paths_Color_g[1] , paths_Color_b[2] , paths_Color_a[3] );

		paths_BrushID[indPath] = pen_brush;
	#ifdef PG_WACOM_TABLET
		paths_RadiusX[indPath]
			= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
			+ tabletPressureRadius * pen_radius_pressure_coef
			+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * pen_radius_angleHor_coef;
		paths_RadiusY[indPath]
			= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
			+ tabletPressureRadius * pen_radius_pressure_coef
			+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * pen_radius_angleVer_coef;
		if (indPath == 0) {
			sprintf(AuxString, "/pen_radius %.0f", float(int(paths_RadiusX[indPath]))); pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
		//if (indPath == 0) {
		//	printf("message %s\n", AuxString);
		//	printf("PEN pen_radius %f pulse_average pen_radius_pulse %.2f %.2f muliplied radius %.2f en radius calculated [%.2f %.2f]\n",
		//		pen_radius, pulse_average, pen_radius_pulse, pen_radius * pen_radiusMultiplier, paths_RadiusX[indPath], paths_RadiusY[indPath]);
		//}
#else
		paths_RadiusX[indPath] = pen_radius * pen_radiusMultiplier
			+ pulse_average * pen_radius_pulse;
		paths_RadiusY[indPath] = pen_radius * pen_radiusMultiplier
			+ pulse_average * pen_radius_pulse;
	#endif
		// printf("PEN brush ID radius %d %.2f\n" , pen_brush, pen_radius );


		///////////////////////////////////////////////////////////////////////
		// PEN PATH UPDATE AND TANGENT CALCULATION
		///////////////////////////////////////////////////////////////////////
		// AUTOMATIC DRAWING FROM MIDI EVENT
		if (FourFrameStrokeNb > 0 && indPath == 0) {
			// intermediary point
			if (FourFrameStrokeNb > 1) {
				CurrentMousePos_x[indPath] = int(FourFrameStroke_x + pen_radius * 2 * (rand_0_1 - 0.5));
				CurrentMousePos_y[indPath] = int(FourFrameStroke_y + pen_radius * 2 * (rand_0_1 - 0.5));
			}
			// last point
			else {
				CurrentMousePos_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
				CurrentMousePos_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
			}
			//printf("MIDI stroke %d %d\n", CurrentMousePos_x[indPath], CurrentMousePos_y[indPath]);
			FourFrameStrokeNb--;
		}

		Pulsed_CurrentMousePos_x[indPath] = CurrentMousePos_x[indPath];
		Pulsed_CurrentMousePos_y[indPath] = CurrentMousePos_y[indPath];
		//printf("current position %d,%d \n", Pulsed_CurrentMousePos_x[indPath], Pulsed_CurrentMousePos_y[indPath]);

#if defined(PIERRES)
		if (indPath = 0) {
			// pen position update from noise
			float random_angle = rand_0_1 * 2.f * float(PI);
			Pulsed_CurrentMousePos_x[0] += int(pulse_average * 200 * pen_position_pulse * cos(random_angle));
			Pulsed_CurrentMousePos_y[0] += int(pulse_average * 200 * pen_position_pulse * sin(random_angle));
			//printf("current position after motion %d,%d \n", Pulsed_CurrentMousePos_x[0], Pulsed_CurrentMousePos_y[0]);

			// writing interruption in case pen-position_dash is not null
			if (pen_position_dash != 0 && pg_FrameNo % (2 + pen_position_dash) == 0) {
				Pulsed_CurrentMousePos_x[0] = PG_OUT_OF_SCREEN_CURSOR;
				Pulsed_CurrentMousePos_y[0] = PG_OUT_OF_SCREEN_CURSOR;
			}
		}
#endif

		// a new drawing step has to be made from a new pen positions: 
		// a the next pen position (different from the preceding next one)
		// The next pen position is the current mouse position.
		// From the previous step:
		// previous previous pen position: paths_x_prev[indPath]
		// previous pen position: paths_x[indPath]
		// current pen position: paths_x_next[indPath]

		// calculates the distance wrt the preceding position and 
		// only updates if it is greater than a minimal value
		glm::vec2 Pulsed_CurrentMousePos = glm::vec2(Pulsed_CurrentMousePos_x[indPath], Pulsed_CurrentMousePos_y[indPath]);
		glm::vec2 next = glm::vec2(paths_x_next[indPath], paths_y_next[indPath]);

		// begin
		float distanceFromPrecedingPoint = distance(Pulsed_CurrentMousePos, next);
		//if (indPath == 0) {
		//	printf("dist from preceding %.2f\n", distanceFromPrecedingPoint);
		//}

		// does not update GPU and considers the position as fixed 
		// if the pen moves of less than 2 pixels or the fifth of the radius
#ifndef ALKEMI
		if (distanceFromPrecedingPoint < std::max(2.f, (paths_RadiusX[indPath] / 5.f))
#else
		if (distanceFromPrecedingPoint < 2.f
#endif
			&& Pulsed_CurrentMousePos_x[indPath] != PG_OUT_OF_SCREEN_CURSOR
			&& Pulsed_CurrentMousePos_y[indPath] != PG_OUT_OF_SCREEN_CURSOR) {
			paths_x_forGPU[indPath] = PG_IDLE_CURSOR;
			paths_y_forGPU[indPath] = PG_IDLE_CURSOR;
		}
		// pen position update for the GPU rendering
		else {
			stroke_geometry_calculation(indPath, Pulsed_CurrentMousePos_x[indPath], Pulsed_CurrentMousePos_y[indPath]);
#ifdef ARAKNIT
			// uses the pen's motion on the tablet to compensate for the metawear sensors, in case they do not work
			// sends the acceleration of the cursor to the sensor's web model for animation
			if (indPath == 0) {
				pg_IPClient* client;
				float delta_t_prev = float(paths_time_prev_prev[0] - paths_time_prev[0]);
				float delta_t = float(paths_time_prev[0] - paths_time[0]);
				float delta_t2 = delta_t_prev + delta_t;
				float linAcc_x = 0.f;
				float linAcc_y = 0.f;
				if (delta_t_prev != 0. && delta_t != 0. && delta_t2 != 0.
					&& paths_x_prev[0] != PG_OUT_OF_SCREEN_CURSOR && paths_y_prev[0] != PG_OUT_OF_SCREEN_CURSOR
					&& paths_x[0] != PG_OUT_OF_SCREEN_CURSOR && paths_y[0] != PG_OUT_OF_SCREEN_CURSOR) {
					linAcc_x = ((float(paths_x_prev_prev[0] - paths_x_prev[0]) / (delta_t_prev * 1000.f)) - (float(paths_x_prev[0] - paths_x[0]) / (delta_t * 1000.f))) / (delta_t2 * 1000.f);
					linAcc_y = ((float(paths_y_prev_prev[0] - paths_y_prev[0]) / (delta_t_prev * 1000.f)) - (float(paths_y_prev[0] - paths_y[0]) / (delta_t * 1000.f))) / (delta_t2 * 1000.f);
				}
				else {
					linAcc_x = 0.f;
					linAcc_y = 0.f;
				}
				if ((client = pg_UDP_client((char*)"udp_Live_send"))) {
					//sprintf(AuxString, "/mss_mass_sensor_acc mw1 %.4f %.4f", linAcc_x, linAcc_y);
					if (pg_FrameNo % 3 == 0) {
						sprintf(AuxString, "/penPosX %.4f", fabs(float(Pulsed_CurrentMousePos_x[0] / 1920.f)));
						pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_Live_send");
					}
					else if (pg_FrameNo % 3 == 1) {
						sprintf(AuxString, "/penPosY %.4f", fabs(float(Pulsed_CurrentMousePos_y[0] / 2160.f)));
						pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_Live_send");
					}
					else {
						sprintf(AuxString, "/penAcc %.4f", min(1.f, float(sqrt(fabs(linAcc_x * linAcc_x + linAcc_y * linAcc_y)) * 10.f + 0.1)));
						pg_send_message_udp((char*)"f", (char*)AuxString, (char*)"udp_Live_send");
					}
					//printf("/mss_mass_sensor_acc mw1 %.4f %.4f\n", linAcc_x, linAcc_y);
				}
			}
#endif

			// POINT STROKE MANAGEMENT
			if ((paths_x_prev[indPath] == PG_OUT_OF_SCREEN_CURSOR || paths_y_prev[indPath] == PG_OUT_OF_SCREEN_CURSOR)
				&& (paths_x_forGPU[indPath] >= 0 && paths_y_forGPU[indPath] >= 0)) {
				//printf("Current position BEGIN %.1f %.1f\n", paths_x_forGPU[indPath], paths_y_forGPU[indPath]);
				paths_x_begin_position[indPath] = paths_x_forGPU[indPath];
				paths_y_begin_position[indPath] = paths_y_forGPU[indPath];
			}
			if ((paths_x_forGPU[indPath] == PG_OUT_OF_SCREEN_CURSOR || paths_y[indPath] == PG_OUT_OF_SCREEN_CURSOR)
				&& (paths_x_prev[indPath] >= 0 && paths_y_prev[indPath] >= 0)) {
				//printf("Current position END %.1f %.1f\n", paths_x_prev[indPath], paths_y_prev[indPath]);
				if (paths_x_prev[indPath] == paths_x_begin_position[indPath] && paths_y_prev[indPath] == paths_y_begin_position[indPath]) {
					//printf("Point at %.1f %.1f\n", paths_x_prev[indPath], paths_y_prev[indPath]);
					//paths_x_forGPU[indPath] = paths_x_prev[indPath];
					//paths_y_forGPU[indPath] = paths_y_prev[indPath];
					//paths_xL[indPath] = paths_x_prev[indPath];
					//paths_yL[indPath] = paths_y_prev[indPath];
					//paths_xR[indPath] = paths_x_prev[indPath];
					//paths_yR[indPath] = paths_y_prev[indPath];
					//paths_x[indPath] = paths_x_prev[indPath];
					//paths_y[indPath] = paths_y_prev[indPath];
					//isBegin[indPath] = true;
					//isEnd[indPath] = true;
				}
			}

			// tactile drawing above first track
			if (indPath > 0) {
				// tells the shader that it should be drawn on the current drawing track
				if (*((int*)ScenarioVarPointers[_path_replay_trackNo_1 + (indPath - 1)]) < 0) {
					*((int*)ScenarioVarPointers[_path_replay_trackNo_1 + (indPath - 1)]) = currentDrawingTrack;
				}

				//stroke_geometry_calculation(pathNo, CurrentMousePos_x[cursorNo], CurrentMousePos_y[cursorNo]);
				//printf("new cursor position %d path %d: %d %d replay %d\n", cursorNo, pathNo, CurrentMousePos_x[cursorNo], CurrentMousePos_y[cursorNo], int(is_path_replay[pathNo]));

				// management of brush radius (w/wo possible interpolation)
				paths_RadiusX[indPath]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
				paths_RadiusY[indPath]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
				paths_Color_r[indPath] = min(1.f, pulsed_pen_color[0]);
				paths_Color_g[indPath] = min(1.f, pulsed_pen_color[1]);
				paths_Color_b[indPath] = min(1.f, pulsed_pen_color[2]);
				paths_Color_a[indPath] = min(1.f, pen_color_a);
			}
		}


		///////////////////////////////////////////////////////////////////////
		// RECORDING PATH
		///////////////////////////////////////////////////////////////////////
		for (int indRecordingPath = 1; indRecordingPath <= PG_NB_PATHS; indRecordingPath++) {
			if (pg_Path_Status[indRecordingPath].isActiveRecording
				&& pg_Path_Status[indRecordingPath].nbRecordedFrames < max_mouse_recording_frames) {
				int indFrameRec = pg_Path_Status[indRecordingPath].nbRecordedFrames;
				// printf( "rec track %d frame %d\n",indRecordingPath,indFrameRec );

				// records the initial time and the current time to play in sync
				if (indFrameRec == 0) {
					pg_Path_Status[indRecordingPath].initialTimeRecording = theTime;
				}
				pg_Path_Status[indRecordingPath].finalTimeRecording = theTime;
				pg_Path_Data[indRecordingPath][indFrameRec].TimeStamp = theTime;

				pg_Path_Data[indRecordingPath][indFrameRec].Color_r = min(1.f, pulsed_pen_color[0]);
				pg_Path_Data[indRecordingPath][indFrameRec].Color_g = min(1.f, pulsed_pen_color[1]);
				pg_Path_Data[indRecordingPath][indFrameRec].Color_b = min(1.f, pulsed_pen_color[2]);
				pg_Path_Data[indRecordingPath][indFrameRec].Color_a = min(1.f, pen_color_a);

				//if (indFrameRec < 10) {
				//	printf("capture %.2f %.2f %.2f %.2f\n",
				//		pg_Path_Data[indRecordingPath][indFrameRec].Color_r, pg_Path_Data[indRecordingPath][indFrameRec].Color_g, 
				//		pg_Path_Data[indRecordingPath][indFrameRec].Color_b, pg_Path_Data[indRecordingPath][indFrameRec].Color_a);
				//}

				pg_Path_Data[indRecordingPath][indFrameRec].BrushID = pen_brush;

#ifdef PG_WACOM_TABLET
				pg_Path_Data[indRecordingPath][indFrameRec].RadiusX
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
					+ tabletPressureRadius * pen_radius_pressure_coef
					+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * pen_radius_angleHor_coef;
				pg_Path_Data[indRecordingPath][indFrameRec].RadiusY
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
					+ tabletPressureRadius * pen_radius_pressure_coef
					+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * pen_radius_angleVer_coef;
#else
				pg_Path_Data[indRecordingPath][indFrameRec].RadiusX
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
				pg_Path_Data[indRecordingPath][indFrameRec].RadiusY
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
#endif

				// printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , indRecordingPath + 1 , indFrameRec - 1 , (int)pg_Path_Pos_xL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_xR[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yR[ indRecordingPath ][ indFrameRec - 1 ] );

				// first control point
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_x_prev = paths_x_prev[0];
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_y_prev = paths_y_prev[0];

#ifdef PG_BEZIER_PATHS
				// second control point
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_xL = paths_xL[0];
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_yL = paths_yL[0];

				// third control point
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_xR = paths_xR[0];
				pg_Path_Data[indRecordingPath][indFrameRec].Pos_yR = paths_yR[0];
#endif

				// fourth control points (next curve first control point)
				if (indFrameRec >= 1) {
					pg_Path_Data[indRecordingPath][indFrameRec].Pos_x = paths_x[0];
					pg_Path_Data[indRecordingPath][indFrameRec].Pos_y = paths_y[0];
				}
				else {
					// move first step: current point with negative coordinates and null second control points
					pg_Path_Data[indRecordingPath][indFrameRec].Pos_x = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Data[indRecordingPath][indFrameRec].Pos_y = PG_OUT_OF_SCREEN_CURSOR;
				}

				// moving forward in the recording
				(pg_Path_Status[indRecordingPath].nbRecordedFrames)++;
			}
			// RECORDING SOURCE TRACK
		}
	}
}



//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A SVG FILE
//////////////////////////////////////////////////////////////////
void load_svg_path(char *fileName, int indPath, int indTrack,
	float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale, 
	string path_ID, bool p_with_color__brush_radius_from_scenario) {
	if (indPath >= 1 && indPath <= PG_NB_PATHS) {
		is_path_replay[indPath] = false;
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		case 1:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_1]) = -1;
			*((bool *)ScenarioVarPointers[_path_record_1]) = false;
			break;
		case 2:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_2]) = -1;
			*((bool *)ScenarioVarPointers[_path_record_2]) = false;
			break;
		case 3:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_3]) = -1;
			*((bool *)ScenarioVarPointers[_path_record_3]) = false;
			break;
#endif
#if PG_NB_PATHS == 7 || PG_NB_PATHS == 11
		case 4:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_4]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_4]) = false;
			break;
		case 5:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_5]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_5]) = false;
			break;
		case 6:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_6]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_6]) = false;
			break;
		case 7:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_7]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_7]) = false;
			break;
#endif
#if PG_NB_PATHS == 11
		case 8:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_8]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_8]) = false;
			break;
		case 9:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_9]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_9]) = false;
			break;
		case 10:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_10]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_10]) = false;
			break;
		case 11:
			*((int*)ScenarioVarPointers[_path_replay_trackNo_11]) = -1;
			*((bool*)ScenarioVarPointers[_path_record_11]) = false;
			break;
#endif
		}
		recorded_path[indPath] = true;

		paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_x_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_y_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		isBegin[indPath] = false;
		isEnd[indPath] = false;
#ifdef PG_BEZIER_PATHS
		paths_xL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yL[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_xR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
		paths_yR[indPath] = PG_OUT_OF_SCREEN_CURSOR;
#endif
		paths_Color_r[indPath] = path_r_color;
		paths_Color_g[indPath] = path_g_color;
		paths_Color_b[indPath] = path_b_color;
		paths_Color_a[indPath] = 1.0f;
		paths_RadiusX[indPath] = pathRadius;
		paths_RadiusY[indPath] = pathRadius;
		// printf( "-> stop_read_path\n" );

		// loads track
		int indDepth = 0;
		readsvg(&indDepth, indPath, fileName, pathRadius, path_r_color, path_g_color, path_b_color, readSpeedScale, path_ID, p_with_color__brush_radius_from_scenario);
	}
}

void readsvg(int *fileDepth, int indPath, char *fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale, string path_ID_in_scenario, bool p_with_color__brush_radius_from_scenario) {
	string         val;
	float          SVG_translation[2] = { 0.0 , 0.0 };
	float          SVG_rotation[3] = { 0.0, 0.0 , 0.0 };
	glm::mat4	   M_transf = glm::mat4(1.0f); // construct identity matrix
	string		   line;
	float          precedingCurrentPoint[2] = { 0.0 , 0.0 };
	float          currentPoint[2] = { 0.0 , 0.0 };

	std::ifstream fin(fileName);
	if (!fin) {
		sprintf(ErrorStr, "Error: file [%s] not found!", fileName); ReportError(ErrorStr); throw 11;
	}
	printf("Loading %s (#%d)\n", fileName, indPath);

	// scans the svg file
	bool timeStamps_loaded = false;
	while (true) {
		std::getline(fin, line);
		//printf("cur line: %s\n", line.substr(0, 40).c_str());
		if (fin.eof()) {
			break;
		}

		////////////////////////////
		////// group transformation tag
		std::size_t found_t = line.find("translate");
		std::size_t found_r = line.find("rotate");
		if (found_t != std::string::npos || found_r != std::string::npos) {
			if (found_t != std::string::npos) {
				found_t = line.find("(", found_t + 1);
				if (found_t != std::string::npos) {
					std::stringstream  sstream(line);
					sstream.seekg(int(found_t) + 1);
					if (found_t != std::string::npos) {
						sstream >> SVG_translation[0];
						sstream.seekg(1, std::ios::cur);
						sstream >> SVG_translation[1];
					}
				}
			}
			if (found_r != std::string::npos) {
				found_r = line.find("(", found_r + 1);
				if (found_r != std::string::npos) {
					std::stringstream  sstream(line);
					sstream.seekg(int(found_r) + 1);
					if (found_r != std::string::npos) {
						sstream >> SVG_rotation[0];
						sstream.seekg(1, std::ios::cur);
						sstream >> SVG_rotation[1];
						sstream.seekg(1, std::ios::cur);
						sstream >> SVG_rotation[2];
					}
				}
			}
			glm::mat4 identity = glm::mat4(1.0f); // construct identity matrix
			glm::mat4 M_rot = glm::mat4(1.0f); // construct identity matrix
			glm::mat4 M_transl = glm::mat4(1.0f); // construct identity matrix
			if (found_r != std::string::npos) {
				//get the matrix transformation to translate
				glm::mat4 transl = glm::translate(identity, glm::vec3(SVG_rotation[1], SVG_rotation[2], 0.f));
				//get the matrix transformation to rotate
				glm::mat4 rot = glm::rotate(identity, glm::radians(SVG_rotation[0]), glm::vec3(0.0, 0.0, 1.0));  // rotate with car ROT -
				//get the matrix transformation to translate
				glm::mat4 translBack = glm::translate(identity, glm::vec3(-SVG_rotation[1], -SVG_rotation[2], 0.f));
				M_rot = transl * rot * translBack;
			}
			if (found_t != std::string::npos) {
				//get the matrix transformation to translate
				//printf("SVG path translation %.2f %.2f\n", SVG_translation[0], SVG_translation[1]);
				M_transl = glm::translate(identity, glm::vec3(SVG_translation[0], SVG_translation[1], 0.f));
			}
			if (found_r != std::string::npos && found_t != std::string::npos) {
				if (found_r < found_t) {
					//printf("SVG path translation (R * T) %.2f %.2f\n", SVG_translation[0], SVG_translation[1]);
					M_transf = M_rot * M_transl;
				}
				else {
					//printf("SVG path translation (T * R) %.2f %.2f\n", SVG_translation[0], SVG_translation[1]);
					M_transf = M_transl * M_rot;
				}
			}
			else if (found_r != std::string::npos) {
				//printf("SVG path rotation (no translation) %.2f %.2f %.2f\n", SVG_rotation[0], SVG_rotation[1], SVG_rotation[2]);
				M_transf = M_rot;
			}
			else if (found_t != std::string::npos) {
				//printf("SVG path translation (no rotation) %.2f %.2f\n", SVG_translation[0], SVG_translation[1]);
				M_transf = M_transl;
			}
		}

		////////////////////////////
		////// path tag
		else {
			std::stringstream  sstream;
			float init_time = 0.f;
			float fin_time = 0.f;
			std::size_t found_end_of_path = std::string::npos;
			string path_ID_in_SVG_file = "";

			std::size_t found = line.find("<path");
			if (found != std::string::npos) {
				string string_path_points;
				string string_path_colors;
				string string_path_brushes;
				string string_path_timeStamps;

				pg_Path_Status[indPath].isFirstFrame = false;
				pg_Path_Status[indPath].isActiveRecording = false;
				pg_Path_Status[indPath].isNormalized = false;
				pg_Path_Status[indPath].nbRecordedFrames = 0;
				pg_Path_Status[indPath].indReading = -1;
				pg_Path_Status[indPath].initialTimeRecording = init_time;
				pg_Path_Status[indPath].finalTimeRecording = fin_time;
				pg_Path_Status[indPath].initialTimeReading = 0.0f;
				pg_Path_Status[indPath].readSpeedScale = readSpeedScale;

				// reads until the end of the file to capture the path and its initial and final time
				do {
					found = line.find(" initial_time=\"", 0);
					if (found != std::string::npos) {
						sstream.str(line.substr(found + strlen(" initial_time=\"")));
						sstream >> pg_Path_Status[indPath].initialTimeRecording;
					}

					found = line.find(" final_time=\"", 0);
					if (found != std::string::npos) {
						sstream.str(line.substr(found + strlen(" final_time=\"")));
						sstream >> pg_Path_Status[indPath].finalTimeRecording;
					}

					found = line.find(" id=\"", 0);
					if (found != std::string::npos) {
						sstream.str(line.substr(found + strlen(" id=\"")));
						sstream >> path_ID_in_SVG_file;
						path_ID_in_SVG_file = path_ID_in_SVG_file.substr(0, path_ID_in_SVG_file.find("\""));
					}

					found = line.find(" d=\"", 0);
					if (found != std::string::npos) {
						 //printf("read path points\n");
						// copies the beginning of the path
						string_path_points = line.substr(found + 4);
						found = line.find("z\"", 0);
						std::size_t found2 = line.find("Z\"", 0);
						while (found == std::string::npos && found2 == std::string::npos) {
							std::getline(fin, line);
							string_path_points += line;
							found = line.find("z\"", 0);
							std::size_t found2 = line.find("Z\"", 0);
						}
					}

					found = line.find(" colors=\"", 0);
					if (found != std::string::npos) {
						//printf("read path colors\n");
						// copies the beginning of the path
						string_path_colors = line.substr(found + strlen(" colors=\""));
						found = line.find("\"", 0);
						while (found == std::string::npos) {
							std::getline(fin, line);
							string_path_colors += line;
							found = line.find("\"", 0);
						}
					}

					found = line.find(" brushes=\"", 0);
					if (found != std::string::npos) {
						//printf("read path brushes\n");
						// copies the beginning of the path
						string_path_brushes = line.substr(found + strlen(" brushes=\""));
						found = line.find("\"", 0);
						while (found == std::string::npos) {
							std::getline(fin, line);
							string_path_brushes += line;
							found = line.find("\"", 0);
						}
					}

					found = line.find(" timeStamps=\"", 0);
					if (found != std::string::npos) {
						//printf("read path time stamps\n");
						// copies the beginning of the path
						string_path_timeStamps = line.substr(found + strlen(" timeStamps=\""));
						found = line.find("\"", 0);
						while (found == std::string::npos) {
							std::getline(fin, line);
							string_path_timeStamps += line;
							found = line.find("\"", 0);
						}
						timeStamps_loaded = true;
					}

					// last path line
					found_end_of_path = line.find("/>", 0);
					// not end of path, reads next line
					if (found_end_of_path == std::string::npos && !fin.eof()) {
						std::getline(fin, line);
					}
				} while (found_end_of_path == std::string::npos && !fin.eof()); // stops reading at the end of path or end of file

				if (string_path_points != "") {
					//printf("load path points\n");
					currentPoint[0] = 0.0;
					currentPoint[1] = 0.0;
					precedingCurrentPoint[0] = 0.0;
					precedingCurrentPoint[1] = 0.0;
					if (string_path_colors != "" && string_path_brushes != "" &&  string_path_timeStamps != "") {
						//printf("load path time stamps\n");
						int nbRecordedFrames = 0;
						LoadPathTimeStampsFromXML(string_path_timeStamps, indPath, &nbRecordedFrames);
						//printf("nbRecordedFrames %d\n", nbRecordedFrames);
						pg_Path_Status[indPath].nbRecordedFrames = nbRecordedFrames;
						//printf("load path colors\n");
						int nbFramesAux = 0;
						LoadPathColorsFromXML(string_path_colors, indPath, &nbFramesAux);
						if (nbFramesAux != nbRecordedFrames) {
							sprintf(ErrorStr, "XML path loading error: incorrect number of color frames %d vs %d!", nbFramesAux, nbRecordedFrames); ReportError(ErrorStr); throw 152;
						}
						//printf("load path brushes\n");
						nbFramesAux = 0;
						LoadPathBrushesFromXML(string_path_brushes, indPath, &nbFramesAux);
						if (nbFramesAux != nbRecordedFrames) {
							sprintf(ErrorStr, "XML path loading error: incorrect number of color frames %d vs %d!", nbFramesAux, nbRecordedFrames); ReportError(ErrorStr); throw 152;
						}
						LoadPathPointsFromXML((char*)string_path_points.c_str(), indPath, &M_transf, pathRadius, path_r_color, path_g_color, path_b_color,
							precedingCurrentPoint, currentPoint, true, p_with_color__brush_radius_from_scenario);
						//printf("nbRecordedFrames after point loading %d\n", pg_Path_Status[indPath].nbRecordedFrames);
					}
					else {
						LoadPathPointsFromXML((char*)string_path_points.c_str(), indPath, &M_transf, pathRadius, path_r_color, path_g_color, path_b_color,
							precedingCurrentPoint, currentPoint, true, true);
						//printf("direct point loading %d\n", pg_Path_Status[indPath].nbRecordedFrames);
					}
					break;
				}
			}
		}
	}
	val.clear();
	fin.close();


	// uniform reading speed
	double interFrameDuration = 1.f / 60.f;
	if (!timeStamps_loaded) {
		if (pg_Path_Status[indPath].nbRecordedFrames > 0 &&
			pg_Path_Status[indPath].finalTimeRecording > pg_Path_Status[indPath].initialTimeRecording) {
			interFrameDuration = (pg_Path_Status[indPath].finalTimeRecording - pg_Path_Status[indPath].initialTimeRecording)
				/ pg_Path_Status[indPath].nbRecordedFrames;
		}
		for (int indPoint = 0; indPoint < pg_Path_Status[indPath].nbRecordedFrames; indPoint++) {
			pg_Path_Data[indPath][indPoint].TimeStamp = indPoint * interFrameDuration + pg_Path_Status[indPath].initialTimeRecording;
		}
	}
	//printf("ind Path %d Nb Frames %d int time %.3f fin time %.3f frameDuration %.3f\n", indPath, pg_Path_Status[indPath].nbRecordedFrames,
	//	pg_Path_Status[indPath].initialTimeRecording, pg_Path_Status[indPath].finalTimeRecording, interFrameDuration);
}


//////////////////////////////////////////////////////////////////
// WRITES TRACKS TO A SVG FILE
//////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD WINAPI writesvg(LPVOID lpParam) {
#else
void* writesvg(void * lpParam) {
#endif
	FILE          *fileSVG;
	threadData    *pDataArray = (threadData *)lpParam;

	fileSVG = fopen(pDataArray->fname, "wb");
	if (fileSVG == NULL) {
		sprintf(ErrorStr, "File %s not opened!", pDataArray->fname); ReportError(ErrorStr); throw 152;
	}
	printf("Snapshot svg (%s)\n",
		pDataArray->fname);

	fprintf(fileSVG, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\n<svg\n   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n   xmlns:cc=\"http://creativecommons.org/ns#\"\n   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n   xmlns:svg=\"http://www.w3.org/2000/svg\"\n   xmlns=\"http://www.w3.org/2000/svg\"\n   version=\"1.1\"\n   width=\"%d\"\n   height=\"%d\"\n   id=\"svg2\">\n  <defs\n     id=\"defs4\" />\n  <metadata\n     id=\"metadata7\">\n    <rdf:RDF>\n      <cc:Work\n         rdf:about=\"\">\n        <dc:format>image/svg+xml</dc:format>\n        <dc:type\n           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n        <dc:title></dc:title>\n      </cc:Work>\n    </rdf:RDF>\n  </metadata>\n  <g\n     transform=\"translate(0.0,0.0)\"\n     id=\"layer1\">\n", pDataArray->w, pDataArray->h);

	// for all paths
	for (int indPath = 0; indPath <= PG_NB_PATHS; indPath++) {
		// main track
		if (pg_Path_Status[indPath].nbRecordedFrames > 0) {
			fprintf(fileSVG, "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n",
				pg_Path_Status[indPath].initialTimeRecording,
				pg_Path_Status[indPath].finalTimeRecording);
			std::ostringstream  path_points;
			std::ostringstream  path_colors;
			std::ostringstream  path_brushes;
			std::ostringstream  path_timeStamps;
			int indFrameIni = 0;
			while (pg_Path_Data[indPath][indFrameIni].Pos_x < 0.0
				&& pg_Path_Data[indPath][indFrameIni].Pos_y < 0.0
				&& indFrameIni < pg_Path_Status[indPath].nbRecordedFrames) {
				indFrameIni++;
			}
			if (indFrameIni < pg_Path_Status[indPath].nbRecordedFrames) {
				path_points << "       d=\"M " << pg_Path_Data[indPath][indFrameIni].Pos_x << ',' << pg_Path_Data[indPath][indFrameIni].Pos_y << ' ';
				path_colors << "       colors=\"" << pg_Path_Data[indPath][indFrameIni].Color_r << ',' << pg_Path_Data[indPath][indFrameIni].Color_g << ',' << pg_Path_Data[indPath][indFrameIni].Color_b << ',' << pg_Path_Data[indPath][indFrameIni].Color_a << ' ';;
				path_brushes << "       brushes=\"" << pg_Path_Data[indPath][indFrameIni].BrushID << ',' << pg_Path_Data[indPath][indFrameIni].RadiusX << ',' << pg_Path_Data[indPath][indFrameIni].RadiusY << ' ';
				path_timeStamps << "       timeStamps=\"" << pg_Path_Data[indPath][indFrameIni].TimeStamp << ' ';
			}
			for (int indFrame = indFrameIni + 1;
				indFrame < pg_Path_Status[indPath].nbRecordedFrames;
				indFrame++) {
				// skips identical frame
				// if( pg_Path_Pos_x[ indPath ][ indFrame ] 
				//     == pg_Path_Pos_x[ indPath ][ indFrame - 1 ] 
				//     && pg_Path_Pos_x[ indPath ][ indFrame ] 
				//     == pg_Path_Pos_x[ indPath ][ indFrame - 1 ] ) {
				//   continue;
				// }
				// move point with a new curve
				if (pg_Path_Data[indPath][indFrame].Pos_x_prev < 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_y_prev < 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_x >= 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_y >= 0.0) {
					path_points << "M " << pg_Path_Data[indPath][indFrame].Pos_x << ',' << pg_Path_Data[indPath][indFrame].Pos_y << ' ';
					path_colors << pg_Path_Data[indPath][indFrame].Color_r << ',' << pg_Path_Data[indPath][indFrame].Color_g << ',' << pg_Path_Data[indPath][indFrame].Color_b << ',' << pg_Path_Data[indPath][indFrame].Color_a << ' ';
					path_brushes << pg_Path_Data[indPath][indFrame].BrushID << ',' << pg_Path_Data[indPath][indFrame].RadiusX << ',' << pg_Path_Data[indPath][indFrame].RadiusY << ' ';
					path_timeStamps << pg_Path_Data[indPath][indFrame].TimeStamp << ' ';
				}
				// curve point
				else if (pg_Path_Data[indPath][indFrame].Pos_x_prev >= 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_y_prev >= 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_x >= 0.0
					&& pg_Path_Data[indPath][indFrame].Pos_y >= 0.0) {
					path_points << "C " << pg_Path_Data[indPath][indFrame].Pos_xL << ',' << pg_Path_Data[indPath][indFrame].Pos_yL << ' ';
					path_points << pg_Path_Data[indPath][indFrame].Pos_xR << ',' << pg_Path_Data[indPath][indFrame].Pos_yR << ' ';
					path_points << pg_Path_Data[indPath][indFrame].Pos_x << ',' << pg_Path_Data[indPath][indFrame].Pos_y << ' ';
					path_colors << pg_Path_Data[indPath][indFrame].Color_r << ',' << pg_Path_Data[indPath][indFrame].Color_g << ',' << pg_Path_Data[indPath][indFrame].Color_b << ',' << pg_Path_Data[indPath][indFrame].Color_a << ' ';
					path_brushes << pg_Path_Data[indPath][indFrame].BrushID << ',' << pg_Path_Data[indPath][indFrame].RadiusX << ',' << pg_Path_Data[indPath][indFrame].RadiusY << ' ';
					path_timeStamps << pg_Path_Data[indPath][indFrame].TimeStamp << ' ';
				}
			}
			// writes the strings that have been stored previously
			fprintf(fileSVG, "%sZ\"\n", path_points.str().c_str());
			fprintf(fileSVG, "%s\"\n", path_colors.str().c_str());
			fprintf(fileSVG, "%s\"\n", path_brushes.str().c_str());
			fprintf(fileSVG, "%s\"\n", path_timeStamps.str().c_str());
			fprintf(fileSVG, "       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1;stroke-width:%dpx\" />\n", indPath, int(pen_radius));
		}
	}
	fprintf(fileSVG, "  </g>\n</svg>\n");

	fclose(fileSVG);

	delete pDataArray->fname;
	delete pDataArray;

	return 0;
}


