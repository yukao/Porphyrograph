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
float **pg_Path_Color_r = NULL;
float **pg_Path_Color_g = NULL;
float **pg_Path_Color_b = NULL;
float **pg_Path_Color_a = NULL;

int **pg_Path_BrushID = NULL;

float **pg_Path_RadiusX = NULL;
float **pg_Path_RadiusY = NULL;

float **pg_Path_Pos_x_prev = NULL;
float **pg_Path_Pos_y_prev = NULL;
float **pg_Path_Pos_x = NULL;
float **pg_Path_Pos_y = NULL;
float **pg_Path_Pos_xL = NULL;
float **pg_Path_Pos_yL = NULL;
float **pg_Path_Pos_xR = NULL;
float **pg_Path_Pos_yR = NULL;
// pg_Path_Time is only used for replaying a path that has been previously
// recorded in the same session 
// for paths loaded from a svg file, either initial and final time are given
// and an average duration is calculated and used to timestamp the points
// or an average 1/60 s duration is chosen. The readSpeedScale value can
// be adjusted in the scenario to replay at an appropriate speed
float **pg_Path_Time = NULL;
struct pg_Path_Status_Struct *pg_Path_Status = NULL;
int *pg_indPreviousFrameReading = NULL;


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
  pg_Path_Color_r = new float*[PG_NB_PATHS + 1];
  pg_Path_Color_g = new float*[PG_NB_PATHS + 1];
  pg_Path_Color_b = new float*[PG_NB_PATHS + 1];
  pg_Path_Color_a = new float*[PG_NB_PATHS + 1];
  pg_Path_BrushID = new int*[PG_NB_PATHS + 1];
  pg_Path_RadiusX = new float*[PG_NB_PATHS + 1];
  pg_Path_RadiusY = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_x_prev = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_y_prev = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_x = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_y = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_xL = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_yL = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_xR = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_yR = new float*[PG_NB_PATHS + 1];
  pg_Path_Time = new float*[PG_NB_PATHS + 1];
  pg_Path_Status = new struct pg_Path_Status_Struct[PG_NB_PATHS + 1];
  pg_indPreviousFrameReading = new int[PG_NB_PATHS + 1];
  // printf("Path initialization size %d\n", max_mouse_recording_frames);
  for( int ind = 0 ; ind < PG_NB_PATHS + 1; ind++ ) {
    pg_Path_Color_r[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Color_g[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Color_b[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Color_a[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_BrushID[ ind ] 
      = new int[ max_mouse_recording_frames ];
    pg_Path_RadiusX[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_RadiusY[ ind ] 
      = new float[ max_mouse_recording_frames ];
	pg_Path_Pos_x_prev[ind]
		= new float[max_mouse_recording_frames];
	pg_Path_Pos_y_prev[ind]
		= new float[max_mouse_recording_frames];
	pg_Path_Pos_x[ind]
		= new float[max_mouse_recording_frames];
	pg_Path_Pos_y[ind]
		= new float[max_mouse_recording_frames];
	pg_Path_Pos_xL[ ind ]
      = new float[ max_mouse_recording_frames ];
    pg_Path_Pos_yL[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Pos_xR[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Pos_yR[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Time[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Status[ ind ].isFirstFrame = false;
    pg_Path_Status[ ind ].isActiveRecording = false;
    pg_Path_Status[ ind ].isNormalized = false;
    pg_Path_Status[ ind ].nbRecordedFrames = 0;
    pg_Path_Status[ ind ].indReading = -1;
	pg_Path_Status[ ind ].initialTimeRecording = 0.0f;
	pg_Path_Status[ ind ].finalTimeRecording = 0.0f;
	pg_Path_Status[ ind ].initialTimeReading = 0.0f;
    pg_Path_Status[ ind ].readSpeedScale = 1.0F;
	pg_indPreviousFrameReading[ind] = 0;
  }

  for( int ind = 0 ; ind < PG_NB_PATHS + 1; ind++ ) {
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
void LoadPathFromXML(char *pathString, int indPath,
					float *translation, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
					int * indCurve, float precedingCurrentPoint[2], float  currentPoint[2]) {
	int            curChar = ' ';
	int            indChar = 0;
	// printf("[%s]\n", pathString);

	if (indPath < 1 || indPath > PG_NB_PATHS) {
		return;
	}

	while (_SpaceChar(curChar)) {
		curChar = pathString[indChar++];
	}
	while (indChar <= (int)strlen(pathString) && curChar != EOF  && curChar != '"') {
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
				if (relative  && (*indCurve) != 0)
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
				if (relative  && (*indCurve) != 0)
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
					&& (*indCurve) < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][*indCurve] = path_r_color;
					pg_Path_Color_g[indPath][*indCurve] = path_g_color;
					pg_Path_Color_b[indPath][*indCurve] = path_b_color;
					pg_Path_Color_a[indPath][*indCurve] = 1.0;
					pg_Path_BrushID[indPath][*indCurve] = 0;
					pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
					pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
					pg_Path_Pos_x_prev[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
					pg_Path_Pos_y_prev[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];
					pg_Path_Pos_x[indPath][*indCurve] = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Pos_y[indPath][*indCurve] = PG_OUT_OF_SCREEN_CURSOR;
					// printf("src move to %d %f %f\n" , *indCurve ,
					// 	     pg_Path_Pos_x[ indPath ][ *indCurve ] , 
					// 	     pg_Path_Pos_y[ indPath ][ *indCurve ]);
					(*indCurve)++;
				}
				if (*indCurve < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][*indCurve] = path_r_color;
					pg_Path_Color_g[indPath][*indCurve] = path_g_color;
					pg_Path_Color_b[indPath][*indCurve] = path_b_color;
					pg_Path_Color_a[indPath][*indCurve] = 1.0;
					pg_Path_BrushID[indPath][*indCurve] = 0;
					pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
					pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
					pg_Path_Pos_x_prev[indPath][*indCurve] = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Pos_y_prev[indPath][*indCurve] = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Pos_x[indPath][*indCurve] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][*indCurve] = currentPoint[1] + translation[1];
					// printf("src move to %d %f %f\n" , *indCurve ,
					// 	     pg_Path_Pos_x[ indPath ][ *indCurve ] , 
					// 	     pg_Path_Pos_y[ indPath ][ *indCurve ]);

					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					(*indCurve)++;
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

				if ((nbCurvePoints) % 3 == 2 && *indCurve < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][*indCurve] = path_r_color;
					pg_Path_Color_g[indPath][*indCurve] = path_g_color;
					pg_Path_Color_b[indPath][*indCurve] = path_b_color;
					pg_Path_Color_a[indPath][*indCurve] = 1.0;
					pg_Path_BrushID[indPath][*indCurve] = 0;
					pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
					pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
					pg_Path_Pos_x_prev[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
					pg_Path_Pos_y_prev[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];
					pg_Path_Pos_xL[indPath][*indCurve] = tanL[0];
					pg_Path_Pos_yL[indPath][*indCurve] = tanL[1];
					pg_Path_Pos_xR[indPath][*indCurve] = tanR[0];
					pg_Path_Pos_yR[indPath][*indCurve] = tanR[1];
					pg_Path_Pos_x[indPath][*indCurve] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][*indCurve] = currentPoint[1] + translation[1];
					// if( *indCurve < 5 ) {
					// 	printf("src curve to %d %f %f\n" , *indCurve ,
					// 	       pg_Path_Pos_x[ indPath ][ *indCurve ] , 
					// 	       pg_Path_Pos_y[ indPath ][ *indCurve ]);
					// }

					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					(*indCurve)++;
					//printf("src curve to %d %f %f\n" , *indCurve - 1 ,
					// 	    pg_Path_Pos_x[ indPath ][ *indCurve - 1 ] , 
					// 	    pg_Path_Pos_y[ indPath ][ *indCurve - 1 ]);
				}
				// right tangent of the first (and current) point
				else if ((nbCurvePoints) % 3 == 0 && *indCurve > 0 && *indCurve < max_mouse_recording_frames) {
					tanL[0] = currentPoint[0] + translation[0];
					tanL[1] = currentPoint[1] + translation[1];
				}
				// left tangent of the second (and next) point
				else if ((nbCurvePoints) % 3 == 1 && *indCurve < max_mouse_recording_frames) {
					tanR[0] = currentPoint[0] + translation[0];
					tanR[1] = currentPoint[1] + translation[1];
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

			if (*indCurve < max_mouse_recording_frames) {
				pg_Path_Color_r[indPath][*indCurve] = path_r_color;
				pg_Path_Color_g[indPath][*indCurve] = path_g_color;
				pg_Path_Color_b[indPath][*indCurve] = path_b_color;
				pg_Path_Color_a[indPath][*indCurve] = 1.0;
				pg_Path_BrushID[indPath][*indCurve] = 0;
				pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
				pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
				pg_Path_Pos_x_prev[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
				pg_Path_Pos_y_prev[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];
				pg_Path_Pos_x[indPath][*indCurve] = currentPoint[0] + translation[0];
				pg_Path_Pos_y[indPath][*indCurve] = currentPoint[1] + translation[1];
				// printf("line to %d %f %f\n" , *indCurve ,
				// 	 pg_Path_Pos_x[ indPath ][ *indCurve ] , 
				// 	 pg_Path_Pos_y[ indPath ][ *indCurve ]);

				precedingCurrentPoint[0] = currentPoint[0];
				precedingCurrentPoint[1] = currentPoint[1];
				(*indCurve)++;
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

				if (*indCurve < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][*indCurve] = path_r_color;
					pg_Path_Color_g[indPath][*indCurve] = path_g_color;
					pg_Path_Color_b[indPath][*indCurve] = path_b_color;
					pg_Path_Color_a[indPath][*indCurve] = 1.0;
					pg_Path_BrushID[indPath][*indCurve] = 0;
					pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
					pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
					pg_Path_Pos_x_prev[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
					pg_Path_Pos_y_prev[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];
					pg_Path_Pos_x[indPath][*indCurve] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][*indCurve] = currentPoint[1] + translation[1];

					// left tangent of current point is preceding point
					// and right tangent of preceding point is the current point
					pg_Path_Pos_xL[indPath][*indCurve]
						= pg_Path_Pos_x_prev[indPath][*indCurve];
					pg_Path_Pos_yL[indPath][*indCurve]
						= pg_Path_Pos_y_prev[indPath][*indCurve];
					pg_Path_Pos_xR[indPath][*indCurve]
						= pg_Path_Pos_x[indPath][*indCurve];
					pg_Path_Pos_yR[indPath][*indCurve]
						= pg_Path_Pos_y[indPath][*indCurve];

					// printf("new line to %d %f %f\n" , *indCurve ,
					// 	   pg_Path_Pos_x[ indPath ][ *indCurve ] , 
					// 	   pg_Path_Pos_y[ indPath ][ *indCurve ]);
					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];
					(*indCurve)++;
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
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}
			// printf("cut\n");
			if (*indCurve < max_mouse_recording_frames) {
				pg_Path_Color_r[indPath][*indCurve] = path_r_color;
				pg_Path_Color_g[indPath][*indCurve] = path_g_color;
				pg_Path_Color_b[indPath][*indCurve] = path_b_color;
				pg_Path_Color_a[indPath][*indCurve] = 1.0;
				pg_Path_BrushID[indPath][*indCurve] = 0;
				pg_Path_RadiusX[indPath][*indCurve] = pathRadius;
				pg_Path_RadiusY[indPath][*indCurve] = pathRadius;
				pg_Path_Pos_x_prev[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
				pg_Path_Pos_y_prev[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];
				pg_Path_Pos_x[indPath][*indCurve] = precedingCurrentPoint[0] + translation[0];
				pg_Path_Pos_y[indPath][*indCurve] = precedingCurrentPoint[1] + translation[1];

				// printf("close curve %d %f %f\n" , *indCurve ,
				// 	 pg_Path_Pos_x[ indPath ][ *indCurve ] , 
				// 	 pg_Path_Pos_y[ indPath ][ *indCurve ]);
				(*indCurve)++;
			}
		}
		break;
		default:
		{
			sprintf(ErrorStr, "Unknown SVG path tag [%c] %d!", curChar, curChar); ReportError(ErrorStr); throw(100);
		}
		break;
		}
	}
	pg_Path_Status[indPath].nbRecordedFrames = *indCurve;
}

//////////////////////////////////////////////////////////////////
// CONVEX HULL 
//////////////////////////////////////////////////////////////////
// https://www.sanfoundry.com/cpp-program-implement-jarvis-march-find-convex-hull/
// A C++ program to find convex hull of a set of points
// Refer http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
// for explanation of orientation()

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise

int orientation(float p_x, float p_y, float q_x, float q_y, float r_x, float r_y)
{
	float val = (q_y - p_y) * (r_x - q_x) - (q_x - p_x) * (r_y - q_y);

	if (val == 0)
		return 0; // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// Prints convex hull of a set of 4 points.
void convexHull(float points_x[4], float points_y[4], int next[4])
{
	// Result should be initialized when calling the function
	for (int i = 0; i < 4; i++)
		next[i] = -1;

	// Find the leftmost point
	int leftmost_point = 0;
	for (int i = 1; i < 4; i++)
		if (points_x[i] < points_x[leftmost_point])
			leftmost_point = i;

	// Start from leftmost point, keep moving counterclockwise
	// until reach the start point again
	int current_point = leftmost_point;
	int n = 4;
	do
	{
		int next_point;

		// Search for a point 'next_point' such that orientation(current_point, i, next_point) is
		// counterclockwise for all points 'i'
		next_point = (current_point + 1) % 4;
		for (int i = 0; i < 4; i++)
			if (orientation(points_x[current_point], points_y[current_point], points_x[i], points_y[i], points_x[next_point], points_y[next_point]) == 2)
				next_point = i;

		next[current_point] = next_point; // Add next_point to result as a next point of current_point
		current_point = next_point; // Set current_point as next_point for next iteration

		n--;
	}
	while (current_point != leftmost_point && n >= 0);

	// counts the number of next points
	//*nb_next_points = 0;
	//for (int i = 0; i < 4; i++)
	//{
	//	if (next[i] != -1)
	//		(*nb_next_points)++;
	//}


	// Print Result
	//for (int i = 0; i < 4; i++)
	//{
	//	if (next[i] != -1)
	//		std::cout << "(" << points_x[i] << ", " << points_y[i] << ")\n";
	//}
}

//////////////////////////////////////////////////////////////////
// MEMORY STORAGE OF THE CURRENT STROKE  
//////////////////////////////////////////////////////////////////
// update of the tables that contain thecurrrentTrack stroke parameters

// replays a path with the same duration
// however since the time stamps are not saved in the SVG file, the 
// is uniform and does not match the exact initial speed 
// to make it better synchronized, it would be necessary
// to store the time stamp of each curve inside the SVG file
#define PG_SYNC_REPLAY 

// indicates whether replay loops in the end (the default behavior)
#define PG_LOOP_REPLAY

void pg_update_pulsed_colors_and_replay_paths(float theTime) {
	// change colors according to music pulse
	update_pulsed_colors();

	///////////////////////////////////////////////////////////////////////
	// READING PATHS 
	///////////////////////////////////////////////////////////////////////
	for (int indPath = 1; indPath < PG_NB_PATHS + 1; indPath++) {
		// active reading
		if (is_path_replay[indPath] && indPath >= 1) {
			// printf("read track %d\n" , indPath );
			// int indFrameReading = pg_Path_Status[ indPath ].indReading;

			// records the initial time and uses the elapsed reading
			// time and the elapsed recording time to play in synch
			if (pg_Path_Status[indPath].isFirstFrame
				|| pg_Path_Status[indPath].indReading == 0) {
				pg_Path_Status[indPath].initialTimeReading
					= theTime - pg_Path_Time[indPath][pg_Path_Status[indPath].indReading]
					+ pg_Path_Status[indPath].initialTimeRecording;
				pg_Path_Status[indPath].isFirstFrame = false;
				pg_indPreviousFrameReading[indPath] = 0;
			}

			bool isCurveBreakBegin = false;
			bool isCurveBreakEnd = false;
#ifdef PG_SYNC_REPLAY
			float theRecordingElapsedTime;
			float theReadingElapsedTime;

			do {
				theRecordingElapsedTime =
					pg_Path_Time[indPath][pg_Path_Status[indPath].indReading]
					- pg_Path_Status[indPath].initialTimeRecording;
				theReadingElapsedTime =
					(theTime - pg_Path_Status[indPath].initialTimeReading)
					* pg_Path_Status[indPath].readSpeedScale;
				// printf( "Ind %d rec time %.2f read time %.2f\n" , pg_Path_Status[ indPath ].indReading ,theRecordingElapsedTime , theReadingElapsedTime );
				// the negtive values correspond to a curve break. If they are jumped over the first 
				// following point should be negative
				if (pg_Path_Pos_x[indPath][pg_Path_Status[indPath].indReading] < 0
					|| pg_Path_Pos_y[indPath][pg_Path_Status[indPath].indReading] < 0) {
					isCurveBreakEnd = true;
				}
				if (pg_Path_Pos_x_prev[indPath][pg_Path_Status[indPath].indReading] < 0
					|| pg_Path_Pos_y_prev[indPath][pg_Path_Status[indPath].indReading] < 0) {
					isCurveBreakBegin = true;
				}
				if (theRecordingElapsedTime <= theReadingElapsedTime) {
					pg_Path_Status[indPath].indReading++;
				}
				else {
					break;
				}

#ifdef PG_LOOP_REPLAY
				// loops at the end
				if (pg_Path_Status[indPath].indReading
					>= pg_Path_Status[indPath].nbRecordedFrames) {
					pg_Path_Status[indPath].indReading = 0;
					isCurveBreakEnd = true;
					break;
				}
#else
				// stops at the end
				if (pg_Path_Status[indPath].indReading
					>= pg_Path_Status[indPath].nbRecordedFrames) {
					pg_Path_Status[indPath].indReading = pg_Path_Status[indPath].nbRecordedFrames - 1;
					isCurveBreakEnd = false;
					break;
				}
#endif
			} while (true);
#else
			// the negtive values correspond to a curve break. If they are jumped over the first 
			// following point should be negative
			if (pg_Path_Pos_x[indPath][pg_Path_Status[indPath].indReading] < 0
				|| pg_Path_Pos_y[indPath][pg_Path_Status[indPath].indReading] < 0) {
				isCurveBreakEnd = true;
			}
			if (pg_Path_Pos_x_prev[indPath][pg_Path_Status[indPath].indReading] < 0
				|| pg_Path_Pos_y_prev[indPath][pg_Path_Status[indPath].indReading] < 0) {
				isCurveBreakBegin = true;
			}

#ifdef PG_LOOP_REPLAY
			// loops at the end
			if (pg_Path_Status[indPath].indReading
				>= pg_Path_Status[indPath].nbRecordedFrames) {
				pg_Path_Status[indPath].indReading = 0;
				isCurveBreakEnd = true;
				break;
		}
#else
			// stops at the end
			if (pg_Path_Status[indPath].indReading
				>= pg_Path_Status[indPath].nbRecordedFrames) {
				pg_Path_Status[indPath].indReading = pg_Path_Status[indPath].nbRecordedFrames - 1;
				isCurveBreakEnd = false;
				break;
			}
#endif
			}
			pg_Path_Status[indPath].indReading++;
#endif

			///////////////////////////////////////////////////////////
			// previous frame for tangent and position
			// management of previous mouse position with
			// consideration for looping
			int indFrameReading = pg_Path_Status[indPath].indReading;
			if (isCurveBreakBegin) {
				//pg_indPreviousFrameReading[indPath] = 0;
				//indFrameReading = pg_indPreviousFrameReading[indPath] + 1;
				paths_x_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
				paths_y_prev[indPath] = PG_OUT_OF_SCREEN_CURSOR;
				paths_xL[indPath] = 0.f;
				paths_yL[indPath] = 0.f;
			}
			else {
				paths_x_prev[indPath] = (float)pg_Path_Pos_x[indPath][pg_indPreviousFrameReading[indPath]];
				paths_y_prev[indPath] = (float)pg_Path_Pos_y[indPath][pg_indPreviousFrameReading[indPath]];
				paths_xL[indPath] = 2 * (float)pg_Path_Pos_x[indPath][pg_indPreviousFrameReading[indPath]] - (float)pg_Path_Pos_xR[indPath][pg_indPreviousFrameReading[indPath]];
				paths_yL[indPath] = 2 * (float)pg_Path_Pos_y[indPath][pg_indPreviousFrameReading[indPath]] - (float)pg_Path_Pos_yR[indPath][pg_indPreviousFrameReading[indPath]];
			}
			//printf("prev frame %d curr frame %d\n", pg_indPreviousFrameReading[indPath], indFrameReading);

			///////////////////////////////////////////////////////////
			// next frame for tangent and position
			// negative values in case of curve break
			if (isCurveBreakEnd) {
				paths_xR[indPath] = 0.f;
				paths_yR[indPath] = 0.f;
				paths_x[indPath] = PG_OUT_OF_SCREEN_CURSOR;
				paths_y[indPath] = PG_OUT_OF_SCREEN_CURSOR;
			}
			else {
				paths_xR[indPath] = (float)pg_Path_Pos_xR[indPath][indFrameReading];
				paths_yR[indPath] = (float)pg_Path_Pos_yR[indPath][indFrameReading];
				paths_x[indPath] = (float)pg_Path_Pos_x[indPath][indFrameReading];
				paths_y[indPath] = (float)pg_Path_Pos_y[indPath][indFrameReading];
			}

			///////////////////////////////////////////////////////////
			// line begin or line end
			// begin
			if ((pg_indPreviousFrameReading[indPath] > 0
				&& pg_Path_Pos_x[indPath][pg_indPreviousFrameReading[indPath] - 1] < 0 
				&& pg_Path_Pos_y[indPath][pg_indPreviousFrameReading[indPath] - 1] < 0)
				&& paths_x_prev[indPath] >= 0 && paths_y_prev[indPath] >= 0 && paths_x[indPath] >= 0 && paths_y[indPath] >= 0) {
				isBegin[indPath] = true;
			}
			else {
				isBegin[indPath] = false;
			}
			// end
			if (paths_x_prev[indPath] >= 0 && paths_y_prev[indPath] >= 0 && paths_x[indPath] >= 0 && paths_y[indPath] >= 0 
				&& pg_Path_Pos_x[indPath][(indFrameReading + 1) % pg_Path_Status[indPath].nbRecordedFrames] < 0
				&& pg_Path_Pos_y[indPath][(indFrameReading + 1) % pg_Path_Status[indPath].nbRecordedFrames] < 0) {
				isEnd[indPath] = true;
			}
			else {
				isEnd[indPath] = false;
			}

			//printf("B0 %.2f %.2f  B1 %.2f %.2f  B2 %.2f %.2f B3 %.2f %.2f \n\n", paths_x_prev[indPath], paths_y_prev[indPath], paths_xL[indPath], paths_yL[indPath], 
			//	paths_xR[indPath], paths_yR[indPath], paths_x[indPath], paths_y[indPath]);

			// management of color (w/wo possible interpolation)
			paths_Color_r[indPath] = (float)pg_Path_Color_r[indPath][indFrameReading];
			paths_Color_g[indPath] = (float)pg_Path_Color_g[indPath][indFrameReading];
			paths_Color_b[indPath] = (float)pg_Path_Color_b[indPath][indFrameReading];
			paths_Color_a[indPath] = (float)pg_Path_Color_a[indPath][indFrameReading];

			//if (pg_indPreviousFrameReading[indPath] < indFrameReading - 1) {
			//	paths_Color_r[indPath] = 1.f;
			//	paths_Color_g[indPath] = 0.f;
			//	paths_Color_b[indPath] = 0.f;
			//}


			// printf( "replay %d %.2f %.2f %.2f\n" , indPath ,
			// 	      is_path_replay[ indPath ] ,
			// 	      paths_x[ indPath -1 ] ,
			// 	      paths_y[ indPath ] );

			// management of brush ID (w/wo possible interpolation)
			paths_BrushID[indPath] = pg_Path_BrushID[indPath][indFrameReading];

			// management of brush radius (w/wo possible interpolation)
			paths_RadiusX[indPath] = (float)pg_Path_RadiusX[indPath][indFrameReading] * pen_radius_replay
				+ pulse_average * pen_radius_replay_pulse;
			paths_RadiusY[indPath] = (float)pg_Path_RadiusY[indPath][indFrameReading] * pen_radius_replay
				+ pulse_average * pen_radius_replay_pulse;

			pg_indPreviousFrameReading[indPath] = indFrameReading;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// PARTICLE COLOR UPDATING INDEPENDENTLY OF TRACK READING OR WRITING
	///////////////////////////////////////////////////////////////////////
	/// uses pulsed color to repop
	repop_Color_r
		= min(1.f, pulsed_repop_color[0]);
	repop_Color_g
		= min(1.f, pulsed_repop_color[1]);
	repop_Color_b
		= min(1.f, pulsed_repop_color[2]);
	// printf("repop color %.2f %.2f %.2f \n", repop_Color_r, repop_Color_g, repop_Color_b);

	///////////////////////////////////////////////////////////////////////
	// PEN PARAMETERS IN PATH 0
	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// draws from mouse position 

	/// uses pulsed color to draw
	paths_Color_r[0] = min(1.f, pulsed_pen_color[0]);
	paths_Color_g[0] = min(1.f, pulsed_pen_color[1]);
	paths_Color_b[0] = min(1.f, pulsed_pen_color[2]);
	paths_Color_a[0] = min(1.f, pen_color_a);
	//printf("PEN color a %.2f %.2f %.2f %.2f\n", paths_Color_r[0] ,
	//	   paths_Color_g[1] , paths_Color_b[2] , paths_Color_a[3] );

	paths_BrushID[0] = pen_brush;
#ifdef PG_WACOM_TABLET
	paths_RadiusX[0]
		= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
		+ tabletPressureRadius * pen_radius_pressure_coef
		+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * pen_radius_angleHor_coef;
	paths_RadiusY[0]
		= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
		+ tabletPressureRadius * pen_radius_pressure_coef
		+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * pen_radius_angleVer_coef;
	//printf("PEN pen_radius pulse_average pen_radius_pulse %.2f %.2f %.2f\n" ,
	//  pen_radius * pen_radiusMultiplier, pulse_average , pen_radius_pulse );
#else
	paths_RadiusX[0] = pen_radius * pen_radiusMultiplier
		+ pulse_average * pen_radius_pulse;
	paths_RadiusY[0] = pen_radius * pen_radiusMultiplier
		+ pulse_average * pen_radius_pulse;
#endif
	// printf("PEN brush ID radius %d %.2f\n" , pen_brush, pen_radius );


	///////////////////////////////////////////////////////////////////////
	// PEN PATH UPDATE AND TANGENT CALCULATION
	///////////////////////////////////////////////////////////////////////
	//printf("previous/current %d,%d   %d,%d \n", PreviousMousePos_x, PreviousMousePos_y, CurrentMousePos_x, CurrentMousePos_y);
	// a new drawing step has to be made from a new pen positions: 
	// a the next pen position (different from the preceding next one)
	// The next pen position is the current mouse position.
	// From the previous step:
	// previous previous pen positin: paths_x_prev[0]
	// previous pen position: paths_x[0]
	// current pen position: paths_x_next_0
	// 
	// calculates the distance wrt the preceding position and 
	// only updates if it is greater than a minimal value
	// printf("PEN pos %dx%d\n",CurrentMousePos_x,CurrentMousePos_y);
	float motionVector_x = CurrentMousePos_x - paths_x_next_0;
	float motionVector_y = CurrentMousePos_y - paths_y_next_0;
	float distanceFromPrecedingPoint = sqrt(motionVector_x * motionVector_x + motionVector_y * motionVector_y);
	if (distanceFromPrecedingPoint < std::max(2.f,(paths_RadiusX[0]/5.f))
		&& CurrentMousePos_x != PG_OUT_OF_SCREEN_CURSOR
		&& CurrentMousePos_y != PG_OUT_OF_SCREEN_CURSOR) {
		paths_x_0_forGPU = PG_IDLE_CURSOR;
		paths_y_0_forGPU = PG_IDLE_CURSOR;
	}
	else {
		paths_x_prev_prev_0 = paths_x_prev[0];
		paths_y_prev_prev_0 = paths_y_prev[0];

		paths_x_prev[0] = paths_x[0];
		paths_y_prev[0] = paths_y[0];

		paths_x[0] = paths_x_next_0;
		paths_y[0] = paths_y_next_0;
		paths_x_0_forGPU = paths_x[0];
		paths_y_0_forGPU = paths_y[0];

		paths_x_next_0 = (float)CurrentMousePos_x;
		paths_y_next_0 = (float)CurrentMousePos_y;

		// define the tangents
		// in the future, keep the previous tangent memory so that it is not recomputed twice

		// segments between two positions
		float u_x = (paths_x_prev[0] - paths_x_prev_prev_0);
		float u_y = (paths_y_prev[0] - paths_y_prev_prev_0);
		float v_x = (paths_x[0] - paths_x_prev[0]);
		float v_y = (paths_y[0] - paths_y_prev[0]);
		float w_x = (paths_x_next_0 - paths_x[0]);
		float w_y = (paths_y_next_0 - paths_y[0]);
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
		paths_xL[0] = paths_x_prev[0] + tang_x_prev;
		paths_yL[0] = paths_y_prev[0] + tang_y_prev;
		paths_xR[0] = paths_x[0] - tang_x;
		paths_yR[0] = paths_y[0] - tang_y;

		// possible resize to avoid crossing
		// non-crossing <=> both points prev and left on the same size of the line (right, current)
		// if N is the normal to (right, current), (vec(curr prev).N) * (vec(curr L).N) >= 0
		int n = 4;
		while ((tang_y * (paths_x_prev[0] - paths_x[0]) - tang_x * (paths_y_prev[0] - paths_y[0])) // vec(curr prev).N
			* (tang_y * (paths_xL[0] - paths_x[0]) - tang_x * (paths_yL[0] - paths_y[0])) // vec(curr L).N
			< 0 && n > 0) {
			// tangents are made shorter
			tang_x_prev /= 2.f;
			tang_y_prev /= 2.f;
			tang_x /= 2.f;
			tang_y /= 2.f;

			// control points from positions and tangents for current and preceding positions
			paths_xL[0] = paths_x_prev[0] + tang_x_prev;
			paths_yL[0] = paths_y_prev[0] + tang_y_prev;
			paths_xR[0] = paths_x[0] - tang_x;
			paths_yR[0] = paths_y[0] - tang_y;

			n--;
		}

#ifdef PG_BEZIER_PATHS
		/* NOT USED CURRENTLY 
		// convex hull shipped to the GPU
		float points_x[4] = { paths_x_prev[0] , paths_xL[0] , paths_xR[0] , paths_x[0] };
		float points_y[4] = { paths_y_prev[0] , paths_yL[0] , paths_yR[0] , paths_y[0] };
		int nb_next_points = 0;
		convexHull(points_x, points_y, path0_next_in_hull);
		*/
#endif

		// line begin or line end
		// begin
		if (paths_x_prev_prev_0 == PG_OUT_OF_SCREEN_CURSOR && paths_y_prev_prev_0 == PG_OUT_OF_SCREEN_CURSOR 
			&& paths_x_prev[0] >= 0 && paths_y_prev[0] >= 0 && paths_x[0] >= 0 && paths_y[0] >= 0) {
			isBegin[0] = true;
		}
		else {
			isBegin[0] = false;
		}
		// end
		if (paths_x_prev[0] >= 0 && paths_y_prev[0] >= 0 && paths_x[0] >= 0 && paths_y[0] >= 0 
			&& paths_x_next_0 == PG_OUT_OF_SCREEN_CURSOR && paths_y_next_0 == PG_OUT_OF_SCREEN_CURSOR) {
			isEnd[0] = true;
		}
		else {
			isEnd[0] = false;
		}

		// cursor reinitialization
		// CurrentMousePos_x = PG_OUT_OF_SCREEN_CURSOR;
		// CurrentMousePos_y = PG_OUT_OF_SCREEN_CURSOR;

		///////////////////////////////////////////////////////////////////////
		// RECORDING PATH
		///////////////////////////////////////////////////////////////////////
		for (int indRecordingPath = 1; indRecordingPath <= PG_NB_PATHS; indRecordingPath++) {
			if (pg_Path_Status[indRecordingPath].isActiveRecording
				&& pg_Path_Status[indRecordingPath].nbRecordedFrames < max_mouse_recording_frames) {
				int indFrameRec = pg_Path_Status[indRecordingPath].nbRecordedFrames;
				// printf( "rec track %d frame %d\n",indRecordingPath,indFrameRec );

				// records the initial time and the current time to play in synch
				if (indFrameRec == 0) {
					pg_Path_Status[indRecordingPath].initialTimeRecording = theTime;
				}
				pg_Path_Status[indRecordingPath].finalTimeRecording = theTime;
				// pg_Path_Time is only used for replaying a path that has been previously
				// recorded in the same session 
				// for paths loaded from a svg file, either initial and final time are given
				// and an average duration is calculated and used to timestamp the points
				// or an average 1/60 s duration is chosen. The readSpeedScale value can
				// be adjusted in the scenario to replay at an appropriate speed
				pg_Path_Time[indRecordingPath][indFrameRec] = theTime;

				pg_Path_Color_r[indRecordingPath][indFrameRec] = min(1.f, pulsed_pen_color[0]);
				pg_Path_Color_g[indRecordingPath][indFrameRec] = min(1.f, pulsed_pen_color[1]);
				pg_Path_Color_b[indRecordingPath][indFrameRec] = min(1.f, pulsed_pen_color[2]);
				pg_Path_Color_a[indRecordingPath][indFrameRec] = min(1.f, pen_color_a);

				// printf( "capture %.2f %.2f %.2f %.2f\n" , 
				// 	      pg_Path_Color_r[ indRecordingPath ][ indFrameRec ] ,
				// 	      pg_Path_Color_g[ indRecordingPath ][ indFrameRec ] ,
				// 	      pg_Path_Color_b[ indRecordingPath ][ indFrameRec ] ,
				// 	      TrackColor_a[ indRecordingPath ][ indFrameRec ] );

				pg_Path_BrushID[indRecordingPath][indFrameRec] = pen_brush;

#ifdef PG_WACOM_TABLET
				pg_Path_RadiusX[indRecordingPath][indFrameRec]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
					+ tabletPressureRadius * pen_radius_pressure_coef
					+ fabs(sin(tabletAzimutRadius))  * tabletInclinationRadius * pen_radius_angleHor_coef;
				pg_Path_RadiusY[indRecordingPath][indFrameRec]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse
					+ tabletPressureRadius * pen_radius_pressure_coef
					+ fabs(cos(tabletAzimutRadius)) * tabletInclinationRadius * pen_radius_angleVer_coef;
#else
				pg_Path_RadiusX[indRecordingPath][indFrameRec]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
				pg_Path_RadiusY[indRecordingPath][indFrameRec]
					= pen_radius * pen_radiusMultiplier + pulse_average * pen_radius_pulse;
#endif

				/*
				// Catmull-Rom spline: computes right and left tangent of the preceding point from
				// position of current point and current point - 2
				if (indFrameRec > 1) {
					// sets tangent extremities to 0 in case of move for not redrawing the
					// jump later & for inserting a move in the saved svg
					float tan_x = Tension * (pg_Path_Pos_x[indRecordingPath][indFrameRec] - pg_Path_Pos_x[indRecordingPath][indFrameRec - 2]);
					float tan_y = Tension * (pg_Path_Pos_y[indRecordingPath][indFrameRec] - pg_Path_Pos_y[indRecordingPath][indFrameRec - 2]);
					// move second step: null left tangent of current point
					if (indFrameRec >= 2) {
						pg_Path_Pos_xL[indRecordingPath][indFrameRec - 1]
							= pg_Path_Pos_x[indRecordingPath][indFrameRec - 1] - tan_x;
						pg_Path_Pos_yL[indRecordingPath][indFrameRec - 1]
							= pg_Path_Pos_y[indRecordingPath][indFrameRec - 1] - tan_y;
					}
					else {
						pg_Path_Pos_xL[indRecordingPath][indFrameRec - 1] = 0.0;
						pg_Path_Pos_yL[indRecordingPath][indFrameRec - 1] = 0.0;
						// a null left tangent was givent at next frame
					}
					// move first step: null right tangent extremity of preceding point
					// so that it is saved as a move
					if (indFrameRec >= 1) {
						pg_Path_Pos_xR[indRecordingPath][indFrameRec - 1]
							= pg_Path_Pos_x[indRecordingPath][indFrameRec - 1] + tan_x;
						pg_Path_Pos_yR[indRecordingPath][indFrameRec - 1]
							= pg_Path_Pos_y[indRecordingPath][indFrameRec - 1] + tan_y;
					}
					else {
						// assigns null values to right tangent extremity so that it is saved
						// as a move
						pg_Path_Pos_xR[indRecordingPath][indFrameRec - 1] = 0.0;
						pg_Path_Pos_yR[indRecordingPath][indFrameRec - 1] = 0.0;
						// a null left tangent extremity will be givent at next frame

						// null left tangent at closing point before the move
						pg_Path_Pos_xL[indRecordingPath][indFrameRec - 1] = pg_Path_Pos_x[indRecordingPath][indFrameRec - 1];
						pg_Path_Pos_yL[indRecordingPath][indFrameRec - 1] = pg_Path_Pos_y[indRecordingPath][indFrameRec - 1];
					}

					// right tangent is half way between first point and left tangent extremity of second point
					if (indFrameRec == 2) {
						pg_Path_Pos_xR[indRecordingPath][0] = pg_Path_Pos_x[indRecordingPath][0]
							+ 0.5F * (pg_Path_Pos_xL[indRecordingPath][1] - pg_Path_Pos_x[indRecordingPath][0]);
						pg_Path_Pos_yR[indRecordingPath][0] = pg_Path_Pos_y[indRecordingPath][0]
							+ 0.5F * (pg_Path_Pos_yL[indRecordingPath][1] - pg_Path_Pos_y[indRecordingPath][0]);
					}
				}
				*/
				// printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , indRecordingPath + 1 , indFrameRec - 1 , (int)pg_Path_Pos_xL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_xR[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yR[ indRecordingPath ][ indFrameRec - 1 ] );

				// first control point
				pg_Path_Pos_x_prev[indRecordingPath][indFrameRec] = paths_x_prev[0];
				pg_Path_Pos_y_prev[indRecordingPath][indFrameRec] = paths_y_prev[0];

				// second control point
				pg_Path_Pos_xL[indRecordingPath][indFrameRec] = paths_xL[0];
				pg_Path_Pos_yL[indRecordingPath][indFrameRec] = paths_yL[0];

				// third control point
				pg_Path_Pos_xR[indRecordingPath][indFrameRec] = paths_xR[0];
				pg_Path_Pos_yR[indRecordingPath][indFrameRec] = paths_yR[0];

				// fourth control points (next curve first control point)
				if (indFrameRec >= 1) {
					pg_Path_Pos_x[indRecordingPath][indFrameRec] = paths_x[0];
					pg_Path_Pos_y[indRecordingPath][indFrameRec] = paths_y[0];
				}
				else {
					// move first step: current point with negative coordinates and null second control points
					pg_Path_Pos_x[indRecordingPath][indFrameRec] = PG_OUT_OF_SCREEN_CURSOR;
					pg_Path_Pos_y[indRecordingPath][indFrameRec] = PG_OUT_OF_SCREEN_CURSOR;
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
	float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale) {
	if (indPath >= 1
		&& indPath <= PG_NB_PATHS) {
		is_path_replay[indPath] = indTrack;
		switch (indPath) {
#if PG_NB_PATHS == 3 || PG_NB_PATHS == 7
		case 1:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_1]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_1]) = false;
			break;
		case 2:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_2]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_2]) = false;
			break;
		case 3:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_3]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_3]) = false;
			break;
#endif
#if PG_NB_PATHS == 7
		case 4:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_4]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_4]) = false;
			break;
		case 5:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_5]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_5]) = false;
			break;
		case 6:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_6]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_6]) = false;
			break;
		case 7:
			*((int *)ScenarioVarPointers[_path_replay_trackNo_7]) = indTrack;
			*((bool *)ScenarioVarPointers[_path_record_7]) = false;
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
		paths_Color_a[indPath] = 1.0F;
		paths_RadiusX[indPath] = pathRadius;
		paths_RadiusY[indPath] = pathRadius;
		// printf( "-> stop_read_path\n" );

		// loads track
		int indDepth = 0;
		readsvg(&indDepth, indPath, fileName, pathRadius, path_r_color, path_g_color, path_b_color, readSpeedScale);
	}
}

void readsvg(int *fileDepth, int indPath, char *fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale) {
	string         val;
	float          translation[2] = { 0.0 , 0.0 };
	string		   line;
	int            indCurve = 0;
	float          precedingCurrentPoint[2] = { 0.0 , 0.0 };
	float          currentPoint[2] = { 0.0 , 0.0 };

	std::ifstream fin(fileName);
	if (!fin) {
		sprintf(ErrorStr, "Error: file [%s] not found!", fileName); ReportError(ErrorStr); throw 11;
	}
	printf("Loading %s (#%d)\n", fileName, indPath);

	// scans the svg file
	while (true) {
		std::getline(fin, line);
		// printf("cur line: %s\n", line.c_str());
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
			std::stringstream  sstrem;
			float init_time = 0.f;
			float fin_time = 0.f;
			std::size_t found2 = std::string::npos;
			std::size_t found3 = std::string::npos;

			found = line.find("<path");
			if (found != std::string::npos) {
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
						sstrem.str(line.substr(found + strlen(" initial_time=\"")));
						sstrem >> pg_Path_Status[indPath].initialTimeRecording;
					}
					found = line.find(" final_time=\"", found + 1);
					if (found != std::string::npos) {
						sstrem.str(line.substr(found + strlen(" final_time=\"")));
						sstrem >> pg_Path_Status[indPath].finalTimeRecording;
					}
					found = line.find(" d=\"", 0);
					if (found != std::string::npos) {
						// printf("found path: %s (offset %d)\n", line.c_str(), found);
						// copies the beginning of the path
						val = line.substr(found + 4);

						// printf( "path values [%s]\n", (char *)val.c_str() );
						currentPoint[0] = 0.0;
						currentPoint[1] = 0.0;
						precedingCurrentPoint[0] = 0.0;
						precedingCurrentPoint[1] = 0.0;
						LoadPathFromXML((char *)val.c_str(), indPath, translation, pathRadius, path_r_color, path_g_color, path_b_color,
							&indCurve, precedingCurrentPoint, currentPoint);
						for (int i = 0; i < 100; i++) {
							// printf("track %d %f %f\n" , i ,
							// 	 pg_Path_Pos_x[ 0 ][ i ] , 
							// 	 pg_Path_Pos_y[ 0 ][ i ]);
						}
					}
					found = line.find("z\"", 0);
					std::size_t found2 = line.find("Z\"", 0);
					std::size_t found3 = line.find("/>", 0);
					std::getline(fin, line);
				} while (found == std::string::npos && found2 == std::string::npos
					&& found3 == std::string::npos && !fin.eof());
			}
		}
	}
	val.clear();
	fin.close();

	// uniform reading speed
	float interFrameDuration = 1.f / 60.f;
	if (pg_Path_Status[indPath].nbRecordedFrames > 0 &&
		pg_Path_Status[indPath].finalTimeRecording > pg_Path_Status[indPath].initialTimeRecording) {
		interFrameDuration = (pg_Path_Status[indPath].finalTimeRecording - pg_Path_Status[indPath].initialTimeRecording)
			/ pg_Path_Status[indPath].nbRecordedFrames;
	}
	for (int indPoint = 0; indPoint < pg_Path_Status[indPath].nbRecordedFrames; indPoint++) {
		pg_Path_Time[indPath][indPoint] = indPoint * interFrameDuration + pg_Path_Status[indPath].initialTimeRecording;
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
	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		// main track
		if (pg_Path_Status[indPath].nbRecordedFrames > 0) {
			fprintf(fileSVG, "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n",
				pg_Path_Status[indPath].initialTimeRecording,
				pg_Path_Status[indPath].finalTimeRecording);
			int indFrameIni = 0;
			while (pg_Path_Pos_x[indPath][indFrameIni] < 0.0
				&& pg_Path_Pos_y[indPath][indFrameIni] < 0.0
				&& indFrameIni < pg_Path_Status[indPath].nbRecordedFrames) {
				indFrameIni++;
			}
			if (indFrameIni < pg_Path_Status[indPath].nbRecordedFrames) {
				fprintf(fileSVG, "       d=\"M %f,%f ",
					pg_Path_Pos_x[indPath][indFrameIni],
					pg_Path_Pos_y[indPath][indFrameIni]);
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
				if (pg_Path_Pos_x_prev[indPath][indFrame] < 0.0
					&& pg_Path_Pos_y_prev[indPath][indFrame] < 0.0
					&& pg_Path_Pos_x[indPath][indFrame] >= 0.0
					&& pg_Path_Pos_y[indPath][indFrame] >= 0.0) {
					fprintf(fileSVG, "M %f,%f ",
						pg_Path_Pos_x[indPath][indFrame],
						pg_Path_Pos_y[indPath][indFrame]);
				}
				// curve point
				else if (pg_Path_Pos_x_prev[indPath][indFrame] >= 0.0
					&& pg_Path_Pos_y_prev[indPath][indFrame] >= 0.0
					&& pg_Path_Pos_x[indPath][indFrame] >= 0.0
					&& pg_Path_Pos_y[indPath][indFrame] >= 0.0) {
					fprintf(fileSVG, "C %f,%f ",
						pg_Path_Pos_xL[indPath][indFrame],
						pg_Path_Pos_yL[indPath][indFrame]);
					fprintf(fileSVG, "%f,%f ",
						pg_Path_Pos_xR[indPath][indFrame],
						pg_Path_Pos_yR[indPath][indFrame]);
					fprintf(fileSVG, "%f,%f ",
						pg_Path_Pos_x[indPath][indFrame],
						pg_Path_Pos_y[indPath][indFrame]);
				}
			}
			fprintf(fileSVG, "\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1;stroke-width:%dpx\" />\n", indPath, int(pen_radius));
		}
	}
	fprintf(fileSVG, "  </g>\n</svg>\n");

	fclose(fileSVG);

	delete pDataArray->fname;
	delete pDataArray;

	return 0;
}


