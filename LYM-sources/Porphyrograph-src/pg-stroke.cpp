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

float **pg_Path_Pos_x = NULL;
float **pg_Path_Pos_y = NULL;
float **pg_Path_Pos_xL = NULL;
float **pg_Path_Pos_yL = NULL;
float **pg_Path_Pos_xR = NULL;
float **pg_Path_Pos_yR = NULL;
float **pg_Path_Time = NULL;
struct pg_Path_Status *pg_Path_Status = NULL;

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
  pg_Path_Pos_x = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_y = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_xL = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_yL = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_xR = new float*[PG_NB_PATHS + 1];
  pg_Path_Pos_yR = new float*[PG_NB_PATHS + 1];
  pg_Path_Time = new float*[PG_NB_PATHS + 1];
  pg_Path_Status = new struct pg_Path_Status[PG_NB_PATHS + 1];
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
    pg_Path_Pos_x[ ind ] 
      = new float[ max_mouse_recording_frames ];
    pg_Path_Pos_y[ ind ] 
      = new float[ max_mouse_recording_frames ];
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
    pg_Path_Status[ ind ].initialTimeReading = 0.0f;
    pg_Path_Status[ ind ].readSpeedScale = 1.0F;
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
	float *translation, float pathRadius, float path_r_color, float path_g_color, float path_b_color) {
	float          interFrameDuration = .02F;
	int            indSpline = 0;
	int            curChar = ' ';
	int            indChar = 0;
	float          currentPoint[2] = { 0.0 , 0.0 };
	float          curveInitialPoint[2] = { 0.0 , 0.0 };
	float          polyCurveInitialPoint[2] = { 0.0 , 0.0 };

	// printf("[%s]\n", pathString);

	if (indPath < 1 || indPath > PG_NB_PATHS) {
		return;
	}

	pg_Path_Status[indPath].isFirstFrame = false;
	pg_Path_Status[indPath].isActiveRecording = false;
	pg_Path_Status[indPath].isNormalized = false;
	pg_Path_Status[indPath].nbRecordedFrames = 0;
	pg_Path_Status[indPath].indReading = -1;
	pg_Path_Status[indPath].initialTimeRecording = 0.0f;
	pg_Path_Status[indPath].initialTimeReading = 0.0f;
	pg_Path_Status[indPath].readSpeedScale = 1.0F;

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
				if (relative  && indSpline != 0)
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
				if (relative  && indSpline != 0)
					currentPoint[1] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}
				nbMovePoints++;
				if (indSpline < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][indSpline] = path_r_color;
					pg_Path_Color_g[indPath][indSpline] = path_g_color;
					pg_Path_Color_b[indPath][indSpline] = path_b_color;
					pg_Path_Color_a[indPath][indSpline] = 1.0;
					pg_Path_BrushID[indPath][indSpline] = 0;
					pg_Path_RadiusX[indPath][indSpline] = pathRadius;
					pg_Path_RadiusY[indPath][indSpline] = pathRadius;
					pg_Path_Pos_x[indPath][indSpline] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][indSpline] = currentPoint[1] + translation[1];
					pg_Path_Time[indPath][indSpline] = indSpline * interFrameDuration;
					// printf("src move to %d %f %f\n" , indSpline ,
					// 	     pg_Path_Pos_x[ indPath ][ indSpline ] , 
					// 	     pg_Path_Pos_y[ indPath ][ indSpline ]);
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
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			curveInitialPoint[0] = currentPoint[0];
			curveInitialPoint[1] = currentPoint[1];

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (relative)
					currentPoint[0] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar) + curveInitialPoint[0];
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
						pathString, &indChar) + curveInitialPoint[1];
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}
				nbCurvePoints++;

				if ((nbCurvePoints) % 3 == 0 && indSpline < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][indSpline] = path_r_color;
					pg_Path_Color_g[indPath][indSpline] = path_g_color;
					pg_Path_Color_b[indPath][indSpline] = path_b_color;
					pg_Path_Color_a[indPath][indSpline] = 1.0;
					pg_Path_BrushID[indPath][indSpline] = 0;
					pg_Path_RadiusX[indPath][indSpline] = pathRadius;
					pg_Path_RadiusY[indPath][indSpline] = pathRadius;
					pg_Path_Pos_x[indPath][indSpline] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][indSpline] = currentPoint[1] + translation[1];
					pg_Path_Time[indPath][indSpline] = indSpline * interFrameDuration;
					// if( indSpline < 5 ) {
					// 	printf("src curve to %d %f %f\n" , indSpline ,
					// 	       pg_Path_Pos_x[ indPath ][ indSpline ] , 
					// 	       pg_Path_Pos_y[ indPath ][ indSpline ]);
					// }
					curveInitialPoint[0] = currentPoint[0];
					curveInitialPoint[1] = currentPoint[1];
					indSpline++;
				}
				// right tangent of the first (and current) point
				else if ((nbCurvePoints) % 3 == 1 && indSpline > 0 && indSpline < max_mouse_recording_frames) {
					pg_Path_Pos_xR[indPath][indSpline - 1] = currentPoint[0] + translation[0];
					pg_Path_Pos_yR[indPath][indSpline - 1] = currentPoint[1] + translation[1];
				}
				// left tangent of the second (and next) point
				else if ((nbCurvePoints) % 3 == 2 && indSpline < max_mouse_recording_frames) {
					pg_Path_Pos_xL[indPath][indSpline] = currentPoint[0] + translation[0];
					pg_Path_Pos_yL[indPath][indSpline] = currentPoint[1] + translation[1];
				}
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

			if (indSpline < max_mouse_recording_frames) {
				pg_Path_Color_r[indPath][indSpline] = path_r_color;
				pg_Path_Color_g[indPath][indSpline] = path_g_color;
				pg_Path_Color_b[indPath][indSpline] = path_b_color;
				pg_Path_Color_a[indPath][indSpline] = 1.0;
				pg_Path_BrushID[indPath][indSpline] = 0;
				pg_Path_RadiusX[indPath][indSpline] = pathRadius;
				pg_Path_RadiusY[indPath][indSpline] = pathRadius;
				pg_Path_Pos_x[indPath][indSpline] = currentPoint[0] + translation[0];
				pg_Path_Pos_y[indPath][indSpline] = currentPoint[1] + translation[1];
				pg_Path_Time[indPath][indSpline] = indSpline * interFrameDuration;
				// printf("line to %d %f %f\n" , indSpline ,
				// 	 pg_Path_Pos_x[ indPath ][ indSpline ] , 
				// 	 pg_Path_Pos_y[ indPath ][ indSpline ]);

				indSpline++;
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

				if (indSpline < max_mouse_recording_frames) {
					pg_Path_Color_r[indPath][indSpline] = path_r_color;
					pg_Path_Color_g[indPath][indSpline] = path_g_color;
					pg_Path_Color_b[indPath][indSpline] = path_b_color;
					pg_Path_Color_a[indPath][indSpline] = 1.0;
					pg_Path_BrushID[indPath][indSpline] = 0;
					pg_Path_RadiusX[indPath][indSpline] = pathRadius;
					pg_Path_RadiusY[indPath][indSpline] = pathRadius;
					pg_Path_Pos_x[indPath][indSpline] = currentPoint[0] + translation[0];
					pg_Path_Pos_y[indPath][indSpline] = currentPoint[1] + translation[1];
					pg_Path_Time[indPath][indSpline] = indSpline * interFrameDuration;

					// left tangent of current point is preceding point
					// and right tangent of preceding point is the current point
					pg_Path_Pos_xL[indPath][indSpline]
						= pg_Path_Pos_x[indPath][indSpline - 1];
					pg_Path_Pos_yL[indPath][indSpline]
						= pg_Path_Pos_y[indPath][indSpline - 1];
					pg_Path_Pos_xR[indPath][indSpline - 1]
						= pg_Path_Pos_x[indPath][indSpline];
					pg_Path_Pos_yR[indPath][indSpline - 1]
						= pg_Path_Pos_y[indPath][indSpline];

					// printf("new line to %d %f %f\n" , indSpline ,
					// 	   pg_Path_Pos_x[ indPath ][ indSpline ] , 
					// 	   pg_Path_Pos_y[ indPath ][ indSpline ]);
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
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}
			// printf("cut\n");
			if (indSpline < max_mouse_recording_frames) {
				pg_Path_Color_r[indPath][indSpline] = path_r_color;
				pg_Path_Color_g[indPath][indSpline] = path_g_color;
				pg_Path_Color_b[indPath][indSpline] = path_b_color;
				pg_Path_Color_a[indPath][indSpline] = 1.0;
				pg_Path_BrushID[indPath][indSpline] = 0;
				pg_Path_RadiusX[indPath][indSpline] = pathRadius;
				pg_Path_RadiusY[indPath][indSpline] = pathRadius;
				pg_Path_Pos_x[indPath][indSpline] = polyCurveInitialPoint[0] + translation[0];
				pg_Path_Pos_y[indPath][indSpline] = polyCurveInitialPoint[1] + translation[1];
				pg_Path_Time[indPath][indSpline] = indSpline * interFrameDuration;

				// printf("close curve %d %f %f\n" , indSpline ,
				// 	 pg_Path_Pos_x[ indPath ][ indSpline ] , 
				// 	 pg_Path_Pos_y[ indPath ][ indSpline ]);
				indSpline++;
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
	pg_Path_Status[indPath].nbRecordedFrames = indSpline;
}

//////////////////////////////////////////////////////////////////
// MEMORY STORAGE OF THE CURRENT STROKE  
//////////////////////////////////////////////////////////////////
// update of the tables that contain thecurrrentTrack stroke parameters
void updateMouseEnvironmentVariablesAndTables(float theTime) {
	// memory of current and preceding values
	mouse_x = (float)CurrentMousePos_x;
	mouse_y = (float)CurrentMousePos_y;

	// change colors according to music pulse
	update_pulsed_colors();

	///////////////////////////////////////////////////////////////////////////
	// PATH RECORDING
	// no update if the mouse has not been moving
	// no update if the mode is not a drawing mode
	if ((mouse_x_prev != mouse_x)
		|| (mouse_y_prev != mouse_y)) {
		// printf("mouse pos %dx%d\n",CurrentMousePos_x,CurrentMousePos_y);

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

				pg_Path_BrushID[indRecordingPath][indFrameRec]
					= pen_brush;

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

				pg_Path_Pos_x[indRecordingPath][indFrameRec] = (float)CurrentMousePos_x;
				pg_Path_Pos_y[indRecordingPath][indFrameRec] = (float)CurrentMousePos_y;

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
				// printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , indRecordingPath + 1 , indFrameRec - 1 , (int)pg_Path_Pos_xL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_xR[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yR[ indRecordingPath ][ indFrameRec - 1 ] );

				pg_Path_Status[indRecordingPath].nbRecordedFrames++;
			}
			// RECORDING SOURCE TRACK
		}
	}

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
			}

			float theRecodingElapsedTime;
			float theReadingElapsedTime;
			bool isEndOfLoop = false;
			bool isCurveBreak = false;

			do {
				theRecodingElapsedTime =
					pg_Path_Time[indPath][pg_Path_Status[indPath].indReading]
					- pg_Path_Status[indPath].initialTimeRecording;
				theReadingElapsedTime =
					(theTime - pg_Path_Status[indPath].initialTimeReading)
					* pg_Path_Status[indPath].readSpeedScale;
				// printf( "Ind %d rec time %.2f read time %.2f\n" , pg_Path_Status[ indPath ].indReading ,theRecodingElapsedTime , theReadingElapsedTime );
				// the negtive values correspond to a curve break. If they are jumped over the first 
				// following point should be negative
				if (pg_Path_Pos_x[indPath][pg_Path_Status[indPath].indReading] < 0
					|| pg_Path_Pos_y[indPath][pg_Path_Status[indPath].indReading] < 0) {
					isCurveBreak = true;
				}
				if (theRecodingElapsedTime <= theReadingElapsedTime) {
					pg_Path_Status[indPath].indReading++;
				}
				else {
					break;
				}

				// loops at the end
				if (pg_Path_Status[indPath].indReading
					>= pg_Path_Status[indPath].nbRecordedFrames) {
					pg_Path_Status[indPath].indReading = 0;
					isEndOfLoop = true;
					break;
				}
			} while (true);

			// management of previous mouse position with
			// consideration for looping
			int indFrameReading = pg_Path_Status[indPath].indReading;
			// printf("alpha %.6f ind %d\n" , pg_Path_Status[ indPath ].coefInterpolation , pg_Path_Status[ indPath ].indReading );

			if (isEndOfLoop) {
				paths_x_prev[indPath] = (float)pg_Path_Pos_x[indPath][indFrameReading];
				paths_y_prev[indPath] = (float)pg_Path_Pos_y[indPath][indFrameReading];
			}
			else {
				paths_x_prev[indPath] = paths_x[indPath];
				paths_y_prev[indPath] = paths_y[indPath];
			}

			// management of current mouse position with
			// consideration for looping      

			paths_x[indPath] = (float)pg_Path_Pos_x[indPath][indFrameReading];
			paths_y[indPath] = (float)pg_Path_Pos_y[indPath][indFrameReading];

			// negative values in case of curve break
			if (isCurveBreak) {
				paths_x[indPath] = -10000;
				paths_y[indPath] = -10000;
			}

			// management of color (w/wo possible interpolation)

			paths_Color_r[indPath] = (float)pg_Path_Color_r[indPath][indFrameReading];
			paths_Color_g[indPath] = (float)pg_Path_Color_g[indPath][indFrameReading];
			paths_Color_b[indPath] = (float)pg_Path_Color_b[indPath][indFrameReading];
			paths_Color_a[indPath] = (float)pg_Path_Color_a[indPath][indFrameReading];


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

			// if( pg_Path_RadiusX[ indPath ][ indFrameReading ] == 0.0 ) 
			// 	printf( "replay %.d %.d brush %d radius %f %f (F%d)\n" , 
			// 		pg_Path_Pos_x[ indPath ][ indFrameReading ] , pg_Path_Pos_y[ indPath ][ indFrameReading ] , pg_Path_BrushID[ indPath ][ indFrameReading ] , pg_Path_RadiusX[ indPath ][ indFrameReading ] , pg_Path_RadiusY[ indPath ][ indFrameReading ] , indFrameReading );

		}
	}

	///////////////////////////////////////////////////////////////////////
	// PARTICLE COLOR UPDATING INDEPENDENTLY OF TRACK READING OR WRITING
	///////////////////////////////////////////////////////////////////////
	/// uses pulsed color to repop
	repop_Color_r
		= repop_grey
		+ pulsed_repop_color[0];
	repop_Color_g
		= repop_grey
		+ pulsed_repop_color[1];
	repop_Color_b
		= repop_grey
		+ pulsed_repop_color[2];
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
	
	
	///////////////////////////////////////////////////////////////////////
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	// +++ pen path (#0) update   +++++++ 
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
	if (mouse_x_prev != mouse_x
		|| mouse_y_prev != mouse_y) {
		paths_x[0] = mouse_x;
		paths_y[0] = mouse_y;

		paths_x_prev[0] = mouse_x_prev;
		paths_y_prev[0] = mouse_y_prev;

		// printf("PEN pos %dx%d\n",CurrentMousePos_x,CurrentMousePos_y);
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
	}

	///////////////////////////////////////////////////////////////////////
	// saves mouse values 
	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
}


//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A SVG FILE
//////////////////////////////////////////////////////////////////
void load_svg_path(char *fileName, int indPath, int indTrack,
	float pathRadius, float path_r_color, float path_g_color, float path_b_color) {
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

		paths_x[indPath] = -1.0F;
		paths_y[indPath] = -1.0F;
		paths_x_prev[indPath] = -1.0F;
		paths_y_prev[indPath] = -1.0F;
		paths_Color_r[indPath] = path_r_color;
		paths_Color_g[indPath] = path_g_color;
		paths_Color_b[indPath] = path_b_color;
		paths_Color_a[indPath] = 1.0F;
		paths_RadiusX[indPath] = pathRadius;
		paths_RadiusY[indPath] = pathRadius;
		// printf( "-> stop_read_path\n" );

		// loads track
		int indDepth = 0;
		readsvg(&indDepth, indPath, fileName, pathRadius, path_r_color, path_g_color, path_b_color);
	}
}

void readsvg(int *fileDepth, int indPath, char *fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color) {
	string         val;
	float          translation[2] = { 0.0 , 0.0 };
	string line;

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
			found = line.find("<path");
			if (found != std::string::npos) {
				found = line.find(" d=\"", found + 1);
				while (found == std::string::npos && !fin.eof()) {
					std::getline(fin, line);
					found = line.find(" d=\"", found + 1);
				}
				if (found != std::string::npos) {
					// printf("found path: %s (offset %d)\n", line.c_str(), found);
					// copies the beginning of the path
					val = line.substr(found + 4);

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
					LoadPathFromXML((char *)val.c_str(), indPath, translation, pathRadius, path_r_color, path_g_color, path_b_color);
					for (int i = 0; i < 100; i++) {
						// printf("track %d %f %f\n" , i ,
						// 	 pg_Path_Pos_x[ 0 ][ i ] , 
						// 	 pg_Path_Pos_y[ 0 ][ i ]);
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
DWORD WINAPI writesvg(LPVOID lpParam) {
#else
void* writesvg(void * lpParam) {
#endif
	FILE          *fileSVG;
	threadData * pDataArray = (threadData *)lpParam;

	fileSVG = fopen(pDataArray->fname, "wb");
	if (fileSVG == NULL) {
		sprintf(ErrorStr, "File %s not opened!", pDataArray->fname); ReportError(ErrorStr); throw 152;
	}

	fprintf(fileSVG, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\n<svg\n   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n   xmlns:cc=\"http://creativecommons.org/ns#\"\n   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n   xmlns:svg=\"http://www.w3.org/2000/svg\"\n   xmlns=\"http://www.w3.org/2000/svg\"\n   version=\"1.1\"\n   width=\"%d\"\n   height=\"%d\"\n   id=\"svg2\">\n  <defs\n     id=\"defs4\" />\n  <metadata\n     id=\"metadata7\">\n    <rdf:RDF>\n      <cc:Work\n         rdf:about=\"\">\n        <dc:format>image/svg+xml</dc:format>\n        <dc:type\n           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n        <dc:title></dc:title>\n      </cc:Work>\n    </rdf:RDF>\n  </metadata>\n  <g\n     transform=\"translate(0.0,0.0)\"\n     id=\"layer1\">\n", pDataArray->w, pDataArray->h);

	// for all paths
	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		// main track
		if (pg_Path_Status[indPath].nbRecordedFrames > 0) {
			fprintf(fileSVG, "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n       d=\"M %f,%f ",
				pg_Path_Status[indPath].initialTimeRecording,
				pg_Path_Status[indPath].initialTimeReading,
				pg_Path_Pos_x[indPath][0],
				pg_Path_Pos_y[indPath][0]);
			for (int indFrame = 1;
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
				if (pg_Path_Pos_xR[indPath][indFrame - 1] == 0.0
					&& pg_Path_Pos_yR[indPath][indFrame - 1] == 0.0
					&& pg_Path_Pos_xL[indPath][indFrame] == 0.0
					&& pg_Path_Pos_yL[indPath][indFrame] == 0.0) {
					fprintf(fileSVG, "M %f,%f ",
						pg_Path_Pos_x[indPath][indFrame],
						pg_Path_Pos_y[indPath][indFrame]);
				}
				// curve point
				else {
					fprintf(fileSVG, "C %f,%f ",
						pg_Path_Pos_xR[indPath][indFrame - 1],
						pg_Path_Pos_yR[indPath][indFrame - 1]);
					// the right tangent of the last point on curve should be computed
					// as the half way between right control of preceding point and current point
					if (indFrame == pg_Path_Status[indPath].nbRecordedFrames - 1) {
						pg_Path_Pos_xR[indPath][indFrame] = pg_Path_Pos_x[indPath][indFrame]
							+ 0.5F * (pg_Path_Pos_xR[indPath][indFrame - 1] - pg_Path_Pos_x[indPath][indFrame]);
						pg_Path_Pos_yR[indPath][indFrame] = pg_Path_Pos_y[indPath][indFrame]
							+ 0.5F * (pg_Path_Pos_yR[indPath][indFrame - 1] - pg_Path_Pos_y[indPath][indFrame]);
					}
					// the last point has a null left tangent
					if (indFrame == pg_Path_Status[indPath].nbRecordedFrames - 1) {
						fprintf(fileSVG, "%f,%f ",
							pg_Path_Pos_x[indPath][indFrame],
							pg_Path_Pos_y[indPath][indFrame]);
					}
					else {
						fprintf(fileSVG, "%f,%f ",
							pg_Path_Pos_xL[indPath][indFrame],
							pg_Path_Pos_yL[indPath][indFrame]);
					}
					fprintf(fileSVG, "%f,%f ",
						pg_Path_Pos_x[indPath][indFrame],
						pg_Path_Pos_y[indPath][indFrame]);
				}
			}
			fprintf(fileSVG, "\"\n       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1\" />\n", indPath);
		}
	}
	fprintf(fileSVG, "  </g>\n</svg>\n");

	fclose(fileSVG);

	delete pDataArray->fname;
	delete pDataArray;

	return 0;
}


//////////////////////////////////////////////////////////////////
// WRITES TRACKS TO TEXTURES FOR DRAWING A LINE OR A STRIP
//////////////////////////////////////////////////////////////////
void pg_writeTrackTextures(int indPath) {
	printOglError(11);

	// source track
	if (pg_Path_Status[indPath].nbRecordedFrames > 0) {
		int indTex = 0;
		for (int indFrame = 0;
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
			pg_paths_Pos_Texture[indPath][indTex] = pg_Path_Pos_x[indPath][indFrame];
			pg_paths_Pos_Texture[indPath][indTex + 1] = pg_Path_Pos_y[indPath][indFrame];
			pg_paths_Pos_Texture[indPath][indTex + 2] = 0.1f + indPath * 0.1f;
			if (pg_Path_Pos_xR[indPath][indFrame - 1] == 0.0
				&& pg_Path_Pos_yR[indPath][indFrame - 1] == 0.0
				&& pg_Path_Pos_xL[indPath][indFrame] == 0.0
				&& pg_Path_Pos_yL[indPath][indFrame] == 0.0) {
				pg_paths_Pos_Texture[indPath][indTex + 3] = -1.0f;
			}
			// curve point
			else
			{
				pg_paths_Pos_Texture[indPath][indTex + 3] = 1.0f;
			}
			// if( indTex/4 < 5 ) {
			// 	printf("src curve to %d %f %f (%f)\n" , indFrame ,
			// 	       pg_paths_Pos_Texture[ indPath ][ indTex ] , 
			// 	       pg_paths_Pos_Texture[ indPath ][ indTex + 1 ], 
			// 	       pg_paths_Pos_Texture[ indPath ][ indTex + 3 ]);
			// }
			indTex += 4;
		}
		indTex = 0;
		for (int indFrame = 0;
			indFrame < pg_Path_Status[indPath].nbRecordedFrames;
			indFrame++) {
			pg_paths_Col_Texture[indPath][indTex] = 1.0;//pg_Path_Color_r[ indPath ][ indFrame ];
			pg_paths_Col_Texture[indPath][indTex + 1] = 1.0;//pg_Path_Color_g[ indPath ][ indFrame ];
			pg_paths_Col_Texture[indPath][indTex + 2] = 1.0;//pg_Path_Color_b[ indPath ][ indFrame ];
			pg_paths_Col_Texture[indPath][indTex + 3] = 1.0;//pg_Path_Color_a[ indPath ][ indFrame ];
			indTex += 4;
		}
		indTex = 0;
		for (int indFrame = 0;
			indFrame < pg_Path_Status[indPath].nbRecordedFrames;
			indFrame++) {
			pg_paths_RadBrushRendmode_Texture[indPath][indTex] = pg_Path_RadiusX[indPath][indFrame];
			pg_paths_RadBrushRendmode_Texture[indPath][indTex + 1] = (float)pg_Path_BrushID[indPath][indFrame];
			pg_paths_RadBrushRendmode_Texture[indPath][indTex + 2] = 1.0; // line
			pg_paths_RadBrushRendmode_Texture[indPath][indTex + 3] = 0.0;
			indTex += 4;
		}

		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, pg_paths_Pos_Texture_texID[indPath]);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexImage1D(GL_TEXTURE_1D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGBA32F,            // Components: Internal colour format to convert to
			max_mouse_recording_frames,          // Image width
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_FLOAT,  // Image data type
			(GLvoid *)pg_paths_Pos_Texture[indPath]);        // The actual image data itself

		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, pg_paths_Col_Texture_texID[indPath]);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexImage1D(GL_TEXTURE_1D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGBA32F,            // Components: Internal colour format to convert to
			max_mouse_recording_frames,          // Image width
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_FLOAT,  // Image data type
			(GLvoid *)pg_paths_Col_Texture[indPath]);        // The actual image data itself

		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, pg_paths_RadBrushRendmode_Texture_texID[indPath]);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexImage1D(GL_TEXTURE_1D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGBA32F,            // Components: Internal colour format to convert to
			max_mouse_recording_frames,          // Image width
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_FLOAT,  // Image data type
			(GLvoid *)pg_paths_RadBrushRendmode_Texture[indPath]);        // The actual image data itself

		printf("Position texture values initialized, track %d: size %d, text size %dx%d\n", indPath, pg_Path_Status[indPath].nbRecordedFrames,
			max_mouse_recording_frames, 1);
	}
	glEnable(GL_TEXTURE_RECTANGLE);

	printOglError(12);
}


