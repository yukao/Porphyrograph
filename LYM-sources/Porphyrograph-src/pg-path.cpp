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

 // SVG paths from scenario
vector<SVG_scenarioPathCurve>	 pg_SVG_scenarioPathCurves[PG_MAX_SCENARIOS];
int						 pg_nb_SVG_path_groups[PG_MAX_SCENARIOS] = { 0 };
int						 pg_current_SVG_path_group = 1;

Path_Status* pg_Path_Status = NULL;

// Bezier path convex hull shipped to the GPU
glm::vec2 pg_BezierControl[(PG_NB_PATHS + 1) * 4];
glm::vec2 pg_BezierHull[(PG_NB_PATHS + 1) * 4];
glm::vec4 pg_BezierBox[(PG_NB_PATHS + 1)];

/////////////////////////////////////////////////////////////////
// config variables
// large array for passing the path data to the fragment shader
float pg_path_data_Update[(PG_NB_PATHS + 1) * PG_MAX_PATH_DATA * 4];
float pg_path_data_ParticleAnimation[(PG_NB_PATHS + 1) * PG_MAX_PATH_ANIM_DATA * 4];

// path variables
pg_paths_currentDynPoint_data pg_paths_currentDynPoint[PG_NB_PATHS + 1];

// auxiliary 
bool pg_recorded_path[PG_NB_PATHS + 1];
bool pg_is_path_replay[PG_NB_PATHS + 1];

// +++++++++++++++++++++ SETUP +++++++++++++++++++++++++++
double pg_LastTrackRecordingChangeClockTime = 0;

////////////////////////////////////////////////////////////////
// SPLINES
////////////////////////////////////////////////////////////////
void pg_ScaleVec2(vec2* pOut_result, vec2* p0, float c0) {
	pOut_result->x = c0 * p0->x;
	pOut_result->y = c0 * p0->y;
}

void pg_AddScaledVec2_Self(vec2* pOut_result, vec2* p0, float c0) {
	pOut_result->x += c0 * p0->x;
	pOut_result->y += c0 * p0->y;
}

//////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TABLES THAT CONTAIN THE STROKE PARAMETERS
//////////////////////////////////////////////////////////////////
// called ones for all the paths
void pg_initPaths(void) {
	pg_Path_Status = new Path_Status[PG_NB_PATHS + 1];
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		pg_Path_Status[ind].Path_Status_init(ind);
	}
}

//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A PATH STRING
//////////////////////////////////////////////////////////////////
long pg_ScanIntegerString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind) {
	long            i = 0L;
	short           neg = 0;

	if (*p_c == '-') {
		neg = 1;
		*p_c = charstring[*ind];
		(*ind)++;
	}
	else if (*p_c == '+') {
		*p_c = charstring[*ind];
		(*ind)++;
	}
	if (_Num(*p_c)) {
		while (_Num(*p_c)) {
			i = i * 10L + (long)(*p_c - '0');
			*p_c = charstring[*ind];
			(*ind)++;
		}

		if (withTrailingSpaceChars) {
			while (_SpaceChar(*p_c)) {
				*p_c = charstring[*ind];
				(*ind)++;
			}
		}
		if (neg == 1)
			i = -i;
		return i;
	}
	else {
		return 0L;
	}
}

float    pg_ScanFloatString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind) {
	float           resul, dec = 0.1F;
	short           neg = 0;

	// arithmetic expression
	if (*p_c == '-') {
		neg = 1;
		*p_c = charstring[*ind];
		(*ind)++;
	}
	else if (*p_c == '+') {
		*p_c = charstring[*ind];
		(*ind)++;
	}
	resul = (float)pg_ScanIntegerString(p_c, false, charstring, ind);
	if (*p_c == '.') {
		*p_c = charstring[*ind];
		(*ind)++;

		while (_Num(*p_c)) {
			resul += dec * (*p_c - '0');
			dec *= 0.1F;
			*p_c = charstring[*ind];
			(*ind)++;
		}
	}
	// exponent
	if (*p_c == 'e') {
		*p_c = charstring[*ind];
		(*ind)++;
		float exponent = (float)pg_ScanIntegerString(p_c, false, charstring, ind);
		resul *= powf(10.0, exponent);
	}
	if (withTrailingSpaceChars) {
		while (_SpaceChar(*p_c)) {
			*p_c = charstring[*ind];
			(*ind)++;
		}
	}
	if (neg == 1)
		resul = -resul;
	return resul;
}

void pg_BezierSubdivideAboveLength(glm::vec2 controlPoints[4], int pathNo, int* indFrame, float path_r_color, float path_g_color, float path_b_color,
	float path_radius, bool with_color__brush_radius_from_scenario, float* path_length, double p_secondsforwidth, int indConfiguration) {
	float curveLength = pg_Bezier_length(controlPoints, 20);
	if (curveLength < 1) {
		// copies current fixed parameters
		if (with_color__brush_radius_from_scenario) {
			pg_Path_Status[pathNo].setFrameValues(indConfiguration, *indFrame, path_r_color, path_g_color, path_b_color, 1.f, 0, path_radius, path_radius);
			//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
		}
		*path_length += curveLength;
		pg_Path_Status[pathNo].setFramePositionsTimeStamp(indConfiguration, *indFrame, controlPoints[0].x, controlPoints[0].y, controlPoints[1].x, controlPoints[1].y,
			controlPoints[2].x, controlPoints[2].y, controlPoints[3].x, controlPoints[3].y, *path_length / pg_workingWindow_width * p_secondsforwidth);
		pg_Path_Status[pathNo].pushFrame(indConfiguration);
		(*indFrame)++;
	}
	else {
		// curve subdivision from decasteljau recursive barycenter (middle)
		// https://web.mit.edu/hyperbook/Patrikalakis-Maekawa-Cho/node13.html
		glm::vec2 controlPointsCurve1[4];
		glm::vec2 controlPointsCurve2[4];
		glm::vec2 middleP1P2;
		controlPointsCurve1[0] = controlPoints[0];
		controlPointsCurve2[3] = controlPoints[3];

		controlPointsCurve1[1] = (controlPoints[0] + controlPoints[1]) / 2.f;
		middleP1P2 = (controlPoints[1] + controlPoints[2]) / 2.f;
		controlPointsCurve2[2] = (controlPoints[2] + controlPoints[3]) / 2.f;

		controlPointsCurve1[2] = (controlPointsCurve1[1] + middleP1P2) / 2.f;
		controlPointsCurve2[1] = (middleP1P2 + controlPointsCurve2[2]) / 2.f;

		controlPointsCurve1[3] = (controlPointsCurve1[2] + controlPointsCurve2[1]) / 2.f;
		controlPointsCurve2[0] = controlPointsCurve1[3];

		pg_BezierSubdivideAboveLength(controlPointsCurve1, pathNo, indFrame, path_r_color, path_g_color, path_b_color,
			path_radius, with_color__brush_radius_from_scenario, path_length, p_secondsforwidth, indConfiguration);
		pg_BezierSubdivideAboveLength(controlPointsCurve2, pathNo, indFrame, path_r_color, path_g_color, path_b_color,
			path_radius, with_color__brush_radius_from_scenario, path_length, p_secondsforwidth, indConfiguration);
	}
}

//////////////////////////////////////////////////////////////////
// LOADS A TRACK FROM A ClipArt FILE
//////////////////////////////////////////////////////////////////
void Path_Status::load_svg_path(char* fileName,
	float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale,
	string path_ID, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indConfiguration) {
	if (pathNo >= 1 && pathNo <= PG_NB_PATHS) {
		pg_is_path_replay[pathNo] = false;
		((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[pathNo] = -1;
		((bool*)pg_FullScenarioVarPointers[_path_record])[pathNo] = false;
		pg_recorded_path[pathNo] = true;
		pg_paths_currentDynPoint[pathNo].pg_paths_x = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_y = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_x_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_y_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = false;
		pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = false;

		pg_paths_currentDynPoint[pathNo].pg_paths_xL = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_yL = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_xR = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_yR = PG_OUT_OF_SCREEN_CURSOR;

		pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = path_r_color;
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = path_g_color;
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = path_b_color;
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_a = 1.0f;
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = pathRadius;
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = pathRadius;
		// printf( "-> stop_read_path\n" );

		// loads track
		readsvg(fileName, pathRadius, path_r_color, path_g_color, path_b_color,
			readSpeedScale, path_ID, p_with_color__brush_radius_from_scenario, secondsforwidth, indConfiguration);
	}
}

void Path_Status::readsvg(char* fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
	float readSpeedScale, string path_ID_in_scenario, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indConfiguration) {
	string         val;
	float          ClipArt_translation[2] = { 0.0 , 0.0 };
	float          ClipArt_rotation[3] = { 0.0, 0.0 , 0.0 };
	glm::mat4	   M_transf = glm::mat4(1.0f); // construct identity matrix
	string		   line;
	float          precedingCurrentPoint[2] = { 0.0 , 0.0 };
	float          currentPoint[2] = { 0.0 , 0.0 };
	float          path_length = 0.f;

	//printf("svg path file name %s\n", fileName);
	std::ifstream fin(fileName);
	if (!fin) {
		sprintf(pg_errorStr, "Error: file [%s] not found!", fileName); pg_ReportError(pg_errorStr); throw 11;
	}
	//printf("Loading %s (#%d)\n", fileName, pathNo);

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
						sstream >> ClipArt_translation[0];
						sstream.seekg(1, std::ios::cur);
						sstream >> ClipArt_translation[1];
					}
				}
			}
			if (found_r != std::string::npos) {
				found_r = line.find("(", found_r + 1);
				if (found_r != std::string::npos) {
					std::stringstream  sstream(line);
					sstream.seekg(int(found_r) + 1);
					if (found_r != std::string::npos) {
						sstream >> ClipArt_rotation[0];
						sstream.seekg(1, std::ios::cur);
						sstream >> ClipArt_rotation[1];
						sstream.seekg(1, std::ios::cur);
						sstream >> ClipArt_rotation[2];
					}
				}
			}
			glm::mat4 identity = glm::mat4(1.0f); // construct identity matrix
			glm::mat4 M_rot = glm::mat4(1.0f); // construct identity matrix
			glm::mat4 M_transl = glm::mat4(1.0f); // construct identity matrix
			if (found_r != std::string::npos) {
				//get the matrix transformation to translate
				glm::mat4 transl = glm::translate(identity, glm::vec3(ClipArt_rotation[1], ClipArt_rotation[2], 0.f));
				//get the matrix transformation to rotate
				glm::mat4 rot = glm::rotate(identity, glm::radians(ClipArt_rotation[0]), glm::vec3(0.0, 0.0, 1.0));  // rotate with car ROT -
				//get the matrix transformation to translate
				glm::mat4 translBack = glm::translate(identity, glm::vec3(-ClipArt_rotation[1], -ClipArt_rotation[2], 0.f));
				M_rot = transl * rot * translBack;
			}
			if (found_t != std::string::npos) {
				//get the matrix transformation to translate
				//printf("ClipArt path translation %.2f %.2f\n", ClipArt_translation[0], ClipArt_translation[1]);
				M_transl = glm::translate(identity, glm::vec3(ClipArt_translation[0], ClipArt_translation[1], 0.f));
			}
			if (found_r != std::string::npos && found_t != std::string::npos) {
				if (found_r < found_t) {
					//printf("ClipArt path translation (R * T) %.2f %.2f\n", ClipArt_translation[0], ClipArt_translation[1]);
					M_transf = M_rot * M_transl;
				}
				else {
					//printf("ClipArt path translation (T * R) %.2f %.2f\n", ClipArt_translation[0], ClipArt_translation[1]);
					M_transf = M_transl * M_rot;
				}
			}
			else if (found_r != std::string::npos) {
				//printf("ClipArt path rotation (no translation) %.2f %.2f %.2f\n", ClipArt_rotation[0], ClipArt_rotation[1], ClipArt_rotation[2]);
				M_transf = M_rot;
			}
			else if (found_t != std::string::npos) {
				//printf("ClipArt path translation (no rotation) %.2f %.2f\n", ClipArt_translation[0], ClipArt_translation[1]);
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
			string path_ID_in_ClipArt_file = "";

			std::size_t found = line.find("<path");
			if (found != std::string::npos) {
				string string_path_points;
				string string_path_colors;
				string string_path_brushes;
				string string_path_timeStamps;

				path_isFirstFrame = false;
				path_isActiveRecording = false;
				path_indReading = -1;
				path_initialTimeReading = 0.0f;
				setCurveValues(indConfiguration, readSpeedScale, init_time, fin_time);
				emptyFrame(indConfiguration);

				// reads until the end of the file to capture the path and its initial and final time
				do {
					found = line.find(" initial_time=\"", 0);
					if (found != std::string::npos) {
						double it;
						sstream.str(line.substr(found + strlen(" initial_time=\"")));
						sstream >> it;
						set_path_curve_initialTimeRecording(indConfiguration, it);
					}

					found = line.find(" final_time=\"", 0);
					if (found != std::string::npos) {
						double ft;
						sstream.str(line.substr(found + strlen(" final_time=\"")));
						sstream >> ft;
						set_path_curve_finalTimeRecording(indConfiguration, ft);
					}

					found = line.find(" id=\"", 0);
					if (found != std::string::npos) {
						sstream.str(line.substr(found + strlen(" id=\"")));
						sstream >> path_ID_in_ClipArt_file;
						path_ID_in_ClipArt_file = path_ID_in_ClipArt_file.substr(0, path_ID_in_ClipArt_file.find("\""));
					}

					found = line.find(" d=\"", 0);
					if (found != std::string::npos) {
						// printf("read path points\n");
						// copies the beginning of the path
						string_path_points = line.substr(found + 4);
						found = line.find("z\"", 0);
						std::size_t found2 = line.find("Z\"", 0);
						std::size_t found3 = line.find("\"", 0);
						while (found == std::string::npos && found2 == std::string::npos && found3 == std::string::npos) {
							std::getline(fin, line);
							string_path_points += line;
							found = line.find("z\"", 0);
							found2 = line.find("Z\"", 0);
							found3 = line.find("\"", 0);
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
					int nbRecordedTimeStamps = 0;

					// printf("load path points\n");
					currentPoint[0] = 0.0;
					currentPoint[1] = 0.0;
					precedingCurrentPoint[0] = 0.0;
					precedingCurrentPoint[1] = 0.0;
					if (string_path_colors != "" && string_path_brushes != "" && string_path_timeStamps != "") {
						//printf("load path time stamps\n");
						path_TmpTimeStamps_init();
						LoadPathTimeStampsFromXML(string_path_timeStamps, &nbRecordedTimeStamps);
						// the number of frames is given by the number of time stamps, the other data should be coherent
						// however edition through inkscape could make these data incoherent if the number of points on the curve changes
						//printf("path %d recorded timestamps %d\n", pathNo, nbRecordedTimeStamps);
						//printf("time stamps %.2f %.2f %.2f %.2f %.2f\n", 
						//	path_TmpTimeStamps[0], path_TmpTimeStamps[1], path_TmpTimeStamps[2], path_TmpTimeStamps[3], path_TmpTimeStamps[4]);

						//printf("load path colors\n");
						int nbRecordedColors = 0;
						LoadPathColorsFromXML(string_path_colors, &nbRecordedColors, indConfiguration);
						//printf("recorded colors %d / %d\n", nbRecordedColors, nbRecordedTimeStamps);
						if (nbRecordedColors != nbRecordedTimeStamps) {
							sprintf(pg_errorStr, "XML path loading error: incorrect number of color frames %d vs time stamps %d!", nbRecordedColors, nbRecordedTimeStamps); pg_ReportError(pg_errorStr); throw 152;
						}

						//printf("load path brushes\n");
						int nbRecordedBrushes = 0;
						LoadPathBrushesFromXML(string_path_brushes, &nbRecordedBrushes, indConfiguration);
						//printf("recorded brushes %d / %d\n", nbRecordedBrushes, nbRecordedTimeStamps);
						if (nbRecordedBrushes != nbRecordedTimeStamps) {
							sprintf(pg_errorStr, "XML path loading error: incorrect number of brush frames %d vs time stamps %d!", nbRecordedBrushes, nbRecordedTimeStamps); pg_ReportError(pg_errorStr); throw 152;
						}

						// printf("load path points\n");
						// possible edition through Inkscape might increase the number of recorded frames
						int nbPointsInPath = pg_LoadPathPointsFromXML((char*)string_path_points.c_str(),
							&M_transf, pathRadius, path_r_color, path_g_color, path_b_color,
							precedingCurrentPoint, currentPoint, true, p_with_color__brush_radius_from_scenario, &path_length, secondsforwidth,
							&nbRecordedTimeStamps, indConfiguration);
						// printf("path points loaded\n");
						if (nbPointsInPath < 1) {
							sprintf(pg_errorStr, "XML path loading error: empty XML path %d!", nbPointsInPath); pg_ReportError(pg_errorStr); throw 152;
						}
						if (nbPointsInPath != nbRecordedTimeStamps) {
							sprintf(pg_errorStr, "XML path loading correction: number of points in path %d does not match number of time stamps %d!", nbPointsInPath, nbRecordedTimeStamps); pg_ReportError(pg_errorStr);
							if (nbPointsInPath < nbRecordedTimeStamps) {
								// the timestamps are retimed to match the actual number of points in the path
								double current_finalTimeRecording = getFrameTimeStamp(pg_ind_scenario, nbPointsInPath - 1);
								if (current_finalTimeRecording > 0) {
									double time_ratio = get_path_curve_finalTimeRecording(indConfiguration) / current_finalTimeRecording;
									for (int indFrame = 0; indFrame < nbPointsInPath; indFrame++) {
										setFrameTimeStamp(pg_ind_scenario, indFrame, getTmpTimeStamp(indFrame) * time_ratio);
									}
								}
							}
							else if (nbPointsInPath > nbRecordedTimeStamps) {
								// fake additional timestamps are added and the initial timestamps are retimed to match the actual number of points in the path
								double time_ratio = float(nbRecordedTimeStamps) / float(nbPointsInPath);
								for (int indFrame = 0; indFrame < nbRecordedTimeStamps; indFrame++) {
									setFrameTimeStamp(pg_ind_scenario, indFrame, getTmpTimeStamp(indFrame) * time_ratio);
								}
								for (int indFrame = nbRecordedTimeStamps; indFrame < nbPointsInPath; indFrame++) {
									setFrameTimeStamp(pg_ind_scenario, indFrame, get_path_curve_finalTimeRecording(pg_ind_scenario) * float(indFrame) / float(nbPointsInPath));
								}
								// number of points is greater than the number of brushes, the brushes are completed with the last one
								// number of points is greater than the number of colors, the colors are completed with the last one
								for (int indFrame = nbRecordedTimeStamps; indFrame < nbPointsInPath; indFrame++) {
									setFrameTimeStamp(pg_ind_scenario, indFrame, getTmpTimeStamp(nbRecordedTimeStamps - 1));
									if (p_with_color__brush_radius_from_scenario) {
										setFrameValues(pg_ind_scenario, indFrame, path_r_color, path_g_color, path_b_color, 1.0, 0, pathRadius, pathRadius);
									}
									else {
										copyFrameValues(indConfiguration, nbRecordedTimeStamps - 1, indFrame);
									}
								}
							}
						}
						else {
							for (int indFrame = 0; indFrame < nbRecordedTimeStamps; indFrame++) {
								setFrameTimeStamp(pg_ind_scenario, indFrame, getTmpTimeStamp(indFrame));
							}
						}
						if (PathStatus_nbFrames(indConfiguration) != nbPointsInPath) {
							sprintf(pg_errorStr, "XML path loading error Nb 1: unexpected frame number of frames %d after reading XML path of %d points!", PathStatus_nbFrames(indConfiguration), nbPointsInPath); pg_ReportError(pg_errorStr); throw 152;
						}
						//set_path_curve_nbRecordedFrames(indConfiguration, nbPointsInPath);
					}
					else {
						int nbPointsInPath = 0;
						nbPointsInPath = pg_LoadPathPointsFromXML((char*)string_path_points.c_str(), &M_transf, pathRadius, path_r_color, path_g_color, path_b_color,
							precedingCurrentPoint, currentPoint, false, true, &path_length, secondsforwidth, &nbRecordedTimeStamps, indConfiguration);
						if (PathStatus_nbFrames(indConfiguration) != nbPointsInPath) {
							sprintf(pg_errorStr, "XML path loading error Nb2: unexpected frame number of frames %d after reading XML path of %d points!", PathStatus_nbFrames(indConfiguration), nbPointsInPath); pg_ReportError(pg_errorStr); throw 152;
						}
						//printf("End of point loading (without path data (color, radius,, time stamps...)) nbRecordedFrames: %d, lentgh: %.2f\n",
						//PathStatus_nbFrames(indConfiguration), path_length);
						// calculation of time stamps according to the length and the speed (uniform speed)
						set_path_curve_initialTimeRecording(indConfiguration, 0.);
						set_path_curve_finalTimeRecording(indConfiguration, path_length / pg_workingWindow_width * secondsforwidth);
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
		//printf("Load uniformspeed time stamps\n");
		if (PathStatus_nbFrames(indConfiguration) > 0 &&
			get_path_curve_finalTimeRecording(indConfiguration) > get_path_curve_initialTimeRecording(indConfiguration)) {
			interFrameDuration = (get_path_curve_finalTimeRecording(indConfiguration) - get_path_curve_initialTimeRecording(indConfiguration))
				/ PathStatus_nbFrames(indConfiguration);
		}
		for (int indPoint = 0; indPoint < PathStatus_nbFrames(indConfiguration); indPoint++) {
			setFrameTimeStamp(indConfiguration, indPoint, indPoint * interFrameDuration + get_path_curve_initialTimeRecording(indConfiguration));
		}
	}
	//printf("ind Path %d Nb Frames %d int time %.3f fin time %.3f frameDuration %.3f\n", pathNo, PathStatus_nbFrames(indConfiguration),
	//	get_path_curve_initialTimeRecording(indConfiguration), get_path_curve_finalTimeRecording(indConfiguration), interFrameDuration);
}


int Path_Status::pg_LoadPathPointsFromXML(char* pathString,
	glm::mat4* p_M_transf, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
	float precedingCurrentPoint[2], float  currentPoint[2],
	bool withRecordingOfStrokeParameters, bool with_color__brush_radius_from_scenario,
	float* path_length, double p_secondsforwidth, int* p_nbRecordedTimeStamps, int indConfiguration) {
	// current char in string
	int            curChar = ' ';
	int            indChar = 0;
	// frame number that is already known if the time stamps are given
	// if not, the frame stack has to be increased as the number of frames increases
	int            indFrame = 0;
	// printf("[%s]\n", pathString);
	// used to calculate transformed coordinates of a point when applying a global transformation to the curve
	glm::vec4 vec0(0, 0, 0, 1);

	// length of the full curve
	*path_length = 0.f;
	if (!withRecordingOfStrokeParameters) {
		setFrameTimeStamp(indConfiguration, indFrame, *path_length / pg_workingWindow_width * p_secondsforwidth);
	}

	//std::cout << glm::to_string(*p_M_transf) << std::endl;

	while (_SpaceChar(curChar)) {
		curChar = pathString[indChar++];
	}
	while (indChar <= (int)strlen(pathString) && curChar != EOF && curChar != '"') {
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
			bool relative = (curChar == 'm');
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (relative && (indFrame) != 0)
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
					sprintf(pg_errorStr, "Expected char [%c] not found!", ','); pg_ReportError(pg_errorStr); throw 17;
				}
				if (relative && indFrame != 0)
					currentPoint[1] += pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				else
					currentPoint[1] = pg_ScanFloatString(&curChar, true,
						pathString, &indChar);
				while (_SpaceChar(curChar)) {
					curChar = pathString[indChar++];
				}

				// if the move is to a point different from the preceding current point, an out of frame
				// point is added so that the rendering in porphyrograph jumps to the new current point
				if ((precedingCurrentPoint[0] != currentPoint[0]
					|| precedingCurrentPoint[1] != currentPoint[1])
					&& indFrame > 0) {
					// copies current fixed parameters
					if (!withRecordingOfStrokeParameters) {
						setFrameValues(indConfiguration, indFrame, path_r_color, path_g_color, path_b_color,
							1.f, 0, pathRadius, pathRadius);
						//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
					}
					// shifts parameters one value ahead due to additional point
					else {
						//printf("move is to a point different from the preceding current point at frame #%d\n", indFrame);
						for (int indFrameAux = PathStatus_nbFrames(indConfiguration) - 1; indFrameAux >= indFrame; indFrameAux--) {
							if (with_color__brush_radius_from_scenario) {
								setFrameValues(indConfiguration, indFrameAux, path_r_color, path_g_color, path_b_color,
									1.f, 0, pathRadius, pathRadius);
							}
							else {
								copyFrameValues(indConfiguration, indFrameAux, indFrameAux + 1);
								copyFrameTimeStamp(indConfiguration, indFrameAux, indFrameAux + 1);
							}
						}
						*p_nbRecordedTimeStamps += 1;
					}
					vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
					setFramePositionsCurrentPrevious(indConfiguration, indFrame, vec0.x, vec0.y,
						PG_OUT_OF_SCREEN_CURSOR, PG_OUT_OF_SCREEN_CURSOR);
					// printf("src move to %d %f %f\n" , indFrame ,
					// 	     pg_Path_Pos_x[ pathNo ][ indFrame ] , 
					// 	     pg_Path_Pos_y[ pathNo ][ indFrame ]);

					if (!withRecordingOfStrokeParameters) {
						setFrameTimeStamp(indConfiguration, indFrame, *path_length / pg_workingWindow_width * p_secondsforwidth);
					}
					// otherwise keep the same timeStamp

					// adds a new frame 
					pushFrame(indConfiguration);
					indFrame++;
					//printf("End of point and path data loading: nbRecordedFrames %d, lentgh: %.2f\n", nbRecordedFrames, path_length);
				}

				// move from out of screen point to the current point
				if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
					setFrameValues(indConfiguration, indFrame, path_r_color, path_g_color, path_b_color, 1.f, 0, pathRadius, pathRadius);
					setFrameTimeStamp(indConfiguration, indFrame, *path_length / pg_workingWindow_width * p_secondsforwidth);
					//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
				}
				vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
				setFramePositionsCurrentPrevious(indConfiguration, indFrame, PG_OUT_OF_SCREEN_CURSOR, PG_OUT_OF_SCREEN_CURSOR, vec0.x, vec0.y);
				// printf("src move to %d %f %f\n" , indFrame ,
				// 	     pg_Path_Pos_x[ pathNo ][ indFrame ] , 
				// 	     pg_Path_Pos_y[ pathNo ][ indFrame ]);

				if (!withRecordingOfStrokeParameters) {
					setFrameTimeStamp(indConfiguration, indFrame, *path_length / pg_workingWindow_width * p_secondsforwidth);
				}
				else {
					setFrameTimeStamp(indConfiguration, indFrame, getTmpTimeStamp((unsigned int)indFrame));
				}

				precedingCurrentPoint[0] = currentPoint[0];
				precedingCurrentPoint[1] = currentPoint[1];

				// adds a new frame
				pushFrame(indConfiguration);
				indFrame++;
				//}
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
			glm::vec2 controlPoints[4];

			bool relative = (curChar == 'c');
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				//////////////////////////////
				// reads a Bezier control point
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
					sprintf(pg_errorStr, "Expected char [%c] not found but [%c] at index %d!", ',', curChar, indChar); pg_ReportError(pg_errorStr); throw 17;
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

				vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
				controlPoints[0] = glm::vec2(vec0.x, vec0.y);

				//////////////////////////////
				// last Bezier control point
				if ((nbCurvePoints) % 3 == 2) {
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					controlPoints[3] = glm::vec2(vec0.x, vec0.y);

					precedingCurrentPoint[0] = currentPoint[0];
					precedingCurrentPoint[1] = currentPoint[1];

					// no work on time stamp calculation: recorded curve
					if (withRecordingOfStrokeParameters) {
						if (with_color__brush_radius_from_scenario) {
							setFrameValues(indConfiguration, indFrame, path_r_color, path_g_color, path_b_color, 1.f, 0, pathRadius, pathRadius);
							//printf("color %.2f %.2f %.2f\n", path_r_color, path_g_color, path_b_color);
						}
						setFramePositions(indConfiguration, indFrame, controlPoints[0].x, controlPoints[0].y, controlPoints[1].x, controlPoints[1].y,
							controlPoints[2].x, controlPoints[2].y, controlPoints[3].x, controlPoints[3].y);

						// curve length addtion
						float curveLength = pg_Bezier_length(controlPoints, 20);
						*path_length += curveLength;

						// adds the frames because they were not added while reading the time stamps
						pushFrame(indConfiguration);
						indFrame++;
					}
					// Inkscape based path edition: the time stamps should be calculated from lenght and the 
					// curve is subdivided if it is too long
					else {
						//std::cout << glm::to_string(glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1)) << std::endl;
						// if( indFrame < 5 ) {
							//printf("src curve to %d %f %f\n" , indFrame ,
								//pg_PathCurveFrame_Data[indConfiguration][pathNo][indFrame].path_Pos_x, pg_PathCurveFrame_Data[indConfiguration][pathNo][indFrame].path_Pos_y);
						// }

						pg_BezierSubdivideAboveLength(controlPoints, pathNo, &indFrame, path_r_color, path_g_color, path_b_color,
							pathRadius, with_color__brush_radius_from_scenario, path_length, p_secondsforwidth, indConfiguration);
					}
				}

				//////////////////////////////
				// right tangent of the first (and current) Bezier control point
				else if ((nbCurvePoints) % 3 == 0 && indFrame > 0) {
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					controlPoints[1] = glm::vec2(vec0.x, vec0.y);
				}

				//////////////////////////////
				// left tangent of the second (and next) Bezier control point
				else if ((nbCurvePoints) % 3 == 1) {
					vec0 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
					controlPoints[2] = glm::vec2(vec0.x, vec0.y);
				}
				nbCurvePoints++;
			}
			if (nbCurvePoints % 3 != 0) {
				printf("Bezier curve points are expected to be given by groups of 3 %d next %c %f %f!\n", nbCurvePoints, curChar, currentPoint[0], currentPoint[1]);
			}
		}
		break;
		// oblique vertical or horizontal line
		case 'l':
		case 'L':
		case 'v':
		case 'V':
		case 'h':
		case 'H':
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
			bool relative = (curChar == 'l') || (curChar == 'h') || (curChar == 'v');
			bool horizontal = (curChar == 'h') || (curChar == 'H');
			bool vertical = (curChar == 'v') || (curChar == 'V');
			bool oblique = (curChar == 'l') || (curChar == 'L');
			curChar = pathString[indChar++];
			while (_SpaceChar(curChar)) {
				curChar = pathString[indChar++];
			}

			while (_Num(curChar) || curChar == '-' || curChar == '+') {
				if (oblique || horizontal) {
					if (relative)
						currentPoint[0] += pg_ScanFloatString(&curChar, true,
							pathString, &indChar);
					else
						currentPoint[0] = pg_ScanFloatString(&curChar, true,
							pathString, &indChar);
					if (oblique) {
						if (curChar == ',') {
							curChar = pathString[indChar++];
						}
						else {
							sprintf(pg_errorStr, "Expected char [%c] not found!", ','); pg_ReportError(pg_errorStr); throw 17;
						}
					}
					while (_SpaceChar(curChar)) {
						curChar = pathString[indChar++];
					}
				}
				if (vertical) {
					// nothing to do: currentPoint[0] == precedingCurrentPoint[0];
					currentPoint[0] = precedingCurrentPoint[0];
				}

				if (oblique || vertical) {
					if (relative)
						currentPoint[1] += pg_ScanFloatString(&curChar, true,
							pathString, &indChar);
					else
						currentPoint[1] = pg_ScanFloatString(&curChar, true,
							pathString, &indChar);
					while (_SpaceChar(curChar)) {
						curChar = pathString[indChar++];
					}
				}
				if (horizontal) {
					// nothing to do: currentPoint[1] == precedingCurrentPoint[1];
					currentPoint[1] = precedingCurrentPoint[1];
				}

				// copies current fixed parameters
				if (!withRecordingOfStrokeParameters || with_color__brush_radius_from_scenario) {
					setFrameValues(indConfiguration, indFrame, path_r_color, path_g_color, path_b_color, 1.f, 0, pathRadius, pathRadius);
				}
				vec0 = (*p_M_transf) * glm::vec4(precedingCurrentPoint[0], precedingCurrentPoint[1], 0, 1);
				glm::vec4 vec1 = (*p_M_transf) * glm::vec4(currentPoint[0], currentPoint[1], 0, 1);
				setFramePositionsCurrentPrevious(indConfiguration, indFrame, vec0.x, vec0.y, vec1.x, vec1.y);

				// left tangent of current point is preceding point
				// and right tangent of preceding point is the current point
				setFramePositionsLeftRightCopy(indConfiguration, indFrame);

				// curve length addtion
				glm::vec2 precPoint(getFramePositionX_prev(indConfiguration, indFrame), getFramePositionY_prev(indConfiguration, indFrame));
				glm::vec2 curPoint(getFramePositionX(indConfiguration, indFrame), getFramePositionY(indConfiguration, indFrame));
				*path_length += glm::distance(precPoint, curPoint);

				if (!withRecordingOfStrokeParameters) {
					setFrameTimeStamp(indConfiguration, indFrame, *path_length / pg_workingWindow_width * p_secondsforwidth);
				}
				else {
					setFrameTimeStamp(indConfiguration, indFrame, getTmpTimeStamp((unsigned int)indFrame));
				}

				//printf("new line to %d %.2f %.2f - %.2f %.2f\n" , indFrame, vec0.x, vec0.y, vec1.x, vec1.y);
				//printf("new line to %d %.2f %.2f - %.2f %.2f\n", indFrame,
				//	getFramePositionX(indConfiguration, indFrame), getFramePositionY(indConfiguration, indFrame),
				//	getFramePositionX_prev(indConfiguration, indFrame), getFramePositionY_prev(indConfiguration, indFrame));

				precedingCurrentPoint[0] = currentPoint[0];
				precedingCurrentPoint[1] = currentPoint[1];

				// adds the frames because they were not added while reading the time stamps
				pushFrame(indConfiguration);
				indFrame++;
				//}
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
			sprintf(pg_errorStr, "Unknown ClipArt path tag [%c] %d!", curChar, curChar); pg_ReportError(pg_errorStr); throw(100);
		}
		break;
		}
	}
	//printf("Number of frames counted from XML Path %d\n", indFrame);
	return indFrame;
}

void Path_Status::LoadPathColorsFromXML(string pathString, int* nbRecordedFrames, int indConfiguration) {
	std::stringstream  sstream;
	sstream.clear();
	std::replace(pathString.begin(), pathString.end(), ',', ' ');
	sstream.str(pathString);
	unsigned int indFrame = 0;
	float r, g, b, a;
	while (sstream >> r && sstream >> g && sstream >> b && sstream >> a) {
		setFrameColor(indConfiguration, indFrame, r, g, b, a);
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
}

void Path_Status::LoadPathBrushesFromXML(string pathString, int* nbRecordedFrames, int indConfiguration) {
	std::stringstream  sstream;
	sstream.clear();
	std::replace(pathString.begin(), pathString.end(), ',', ' ');
	sstream.str(pathString);
	unsigned int       indFrame = 0;
	int br;
	float rx, ry;
	while (sstream >> br && sstream >> rx && sstream >> ry) {
		setFrameBrushRadius(indConfiguration, indFrame, br, rx, ry);
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
}

void Path_Status::LoadPathTimeStampsFromXML(string pathString, int* nbRecordedFrames) {
	std::stringstream  sstream;
	sstream.clear();
	sstream.str(pathString);
	//printf("TS string %.s\n", pathString.c_str());
	int indFrame = 0;
	double ts;
	while (sstream >> ts) {
		pushTmpTimeStamps(ts);
		// adds a new frame for each timestamp (and they will not be added when reading the points
		++indFrame;
	}
	*nbRecordedFrames = indFrame;
	//for (int indFrame2 = 0; indFrame2 < *nbRecordedFrames; indFrame2++) {
	//	printf("%.2f ", path_TmpTimeStamps[indFrame2]);
	//}
	//printf("\n");
}

//////////////////////////////////////////////////////////////////
// BEZIER PATH CONVEX HULL 
//////////////////////////////////////////////////////////////////
bool pg_pointEquals(glm::vec2* p, glm::vec2* q) {
	return p->x == q->x && p->y == q->y;
};
bool pg_left_oriented(glm::vec2* p1, glm::vec2* p2, glm::vec2* candidate) {
	float det = (p2->x - p1->x) * (candidate->y - p1->y)
		- (candidate->x - p1->x) * (p2->y - p1->y);
	if (det > 0) return true;  // left-oriented 
	if (det < 0) return false; // right oriented
	// select the farthest point in case of colinearity
	return glm::distance(*p1, *candidate) > glm::distance(*p1, *p2);
}

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and draw bezier curve
void pg_cubicBezier(glm::vec2 control_points[4], glm::vec2* curve_point, float alphaBezier) {
	(*curve_point).x = float(pow(1 - alphaBezier, 3) * control_points[0].x + 3 * alphaBezier * pow(1 - alphaBezier, 2) * control_points[1].x
		+ 3 * pow(alphaBezier, 2) * (1 - alphaBezier) * control_points[2].x + pow(alphaBezier, 3) * control_points[3].x);
	(*curve_point).y = float(pow(1 - alphaBezier, 3) * control_points[0].y + 3 * alphaBezier * pow(1 - alphaBezier, 2) * control_points[1].y
		+ 3 * pow(alphaBezier, 2) * (1 - alphaBezier) * control_points[2].y + pow(alphaBezier, 3) * control_points[3].y);
}
void pg_cubicBezier(glm::vec3 control_points[4], glm::vec3* curve_point, float alphaBezier) {
	(*curve_point).x = float(pow(1 - alphaBezier, 3) * control_points[0].x + 3 * alphaBezier * pow(1 - alphaBezier, 2) * control_points[1].x
		+ 3 * pow(alphaBezier, 2) * (1 - alphaBezier) * control_points[2].x + pow(alphaBezier, 3) * control_points[3].x);
	(*curve_point).y = float(pow(1 - alphaBezier, 3) * control_points[0].y + 3 * alphaBezier * pow(1 - alphaBezier, 2) * control_points[1].y
		+ 3 * pow(alphaBezier, 2) * (1 - alphaBezier) * control_points[2].y + pow(alphaBezier, 3) * control_points[3].y);
	(*curve_point).z = float(pow(1 - alphaBezier, 3) * control_points[0].z + 3 * alphaBezier * pow(1 - alphaBezier, 2) * control_points[1].z
		+ 3 * pow(alphaBezier, 2) * (1 - alphaBezier) * control_points[2].z + pow(alphaBezier, 3) * control_points[3].z);
}

#if defined(var_Novak_flight_on)
///////////////////////////////////////////////////////////
// BIRD FLIGHTS FOR NOVAK
///////////////////////////////////////////////////////////
glm::vec3     prev_Novak_flight_control_points[PG_NB_FLIGHTS][4];
glm::vec3     cur_Novak_flight_control_points[PG_NB_FLIGHTS][4];
glm::vec3     cur_Novak_flight_points[PG_NB_FLIGHTS];
glm::vec2     cur_Novak_flight_2D_points[PG_NB_FLIGHTS];
float         prev_Novak_flightTime[PG_NB_FLIGHTS] = { 0.f };
double        prev_Novak_flightCurrentCLockTime[PG_NB_FLIGHTS] = { 0.f };
float         cur_Novak_flightTime[PG_NB_FLIGHTS] = { 0.f };
int           cur_Novak_flightIndex[PG_NB_FLIGHTS] = { 1 };
glm::vec3     Novak_flight_deviation[PG_NB_FLIGHTS] = { glm::vec3(0,0,0) };
float         cur_Novak_flightPerlinNoise[PG_NB_FLIGHTS][3][2];

void Novak_flight_next_control_points(int indFlight) {
	// C0 continuity
	cur_Novak_flight_control_points[indFlight][0] = prev_Novak_flight_control_points[indFlight][3];
	// C1 continuity
	cur_Novak_flight_control_points[indFlight][1] = prev_Novak_flight_control_points[indFlight][3] + prev_Novak_flight_control_points[indFlight][3]
		- prev_Novak_flight_control_points[indFlight][2];
	cur_Novak_flight_control_points[indFlight][2].x = (float(cur_Novak_flightIndex[indFlight]) + 0.333333333f * 2) * Novak_flight_cuve_length;
	cur_Novak_flight_control_points[indFlight][2].y = rand_x(Novak_flight_cuve_x_spread);
	cur_Novak_flight_control_points[indFlight][2].z = rand_x(Novak_flight_cuve_y_spread);
	cur_Novak_flight_control_points[indFlight][3].x = (float(cur_Novak_flightIndex[indFlight]) + 0.333333333f * 3) * Novak_flight_cuve_length;
	cur_Novak_flight_control_points[indFlight][3].y = rand_x(Novak_flight_cuve_x_spread);
	cur_Novak_flight_control_points[indFlight][3].z = rand_x(Novak_flight_cuve_y_spread);
	//printf("CP flight %d index %d   (%.2f,%.2f,%.2f) \n",
	//	indFlight, cur_Novak_flightIndex[indFlight],
	//	prev_Novak_flight_control_points[indFlight][0].x, prev_Novak_flight_control_points[indFlight][0].y, prev_Novak_flight_control_points[indFlight][0].z);

}

/////////////////////////////////////////////////
// deviation from main trajectory
void Novak_yz_flight_deviation_from_trajectory(int indFlight) {
	double noise[3] = {
		pg_PerlinNoise(cur_Novak_flightPerlinNoise[indFlight][0][0], cur_Novak_flightPerlinNoise[indFlight][0][1], cur_Novak_flightTime[indFlight] * Novak_flight_deviation_speed),
		pg_PerlinNoise(cur_Novak_flightPerlinNoise[indFlight][1][0], cur_Novak_flightPerlinNoise[indFlight][1][1], cur_Novak_flightTime[indFlight] * Novak_flight_deviation_speed),
		pg_PerlinNoise(cur_Novak_flightPerlinNoise[indFlight][2][0], cur_Novak_flightPerlinNoise[indFlight][2][1], cur_Novak_flightTime[indFlight] * Novak_flight_deviation_speed)
	};
	Novak_flight_deviation[indFlight] = glm::vec3(Novak_flight_deviation_amplitude * noise[0],
		Novak_flight_deviation_amplitude * noise[1], Novak_flight_deviation_amplitude * noise[2]);
	//printf("flight deviation #0 %.2f %.2f %.2f\n", Novak_flight_deviation[0].x, Novak_flight_deviation[0].y, Novak_flight_deviation[0].z);
	//printf("flight deviation #1 %.2f %.2f %.2f\n", Novak_flight_deviation[1].x, Novak_flight_deviation[1].y, Novak_flight_deviation[1].z);
	//printf("flight deviation #2 %.2f %.2f %.2f\n", Novak_flight_deviation[2].x, Novak_flight_deviation[2].y, Novak_flight_deviation[2].z);
}

void Novak_flight_init_control_points(void) {
	for (int indFlight = 1; indFlight < PG_NB_FLIGHTS; indFlight++) {
		prev_Novak_flightCurrentCLockTime[indFlight] = pg_CurrentClockTime;
		prev_Novak_flightTime[indFlight] = float(pg_CurrentClockTime) * Novak_flight_speed;
		for (int indPt = 0; indPt < 4; indPt++) {
			prev_Novak_flight_control_points[indFlight][indPt].x = (float(cur_Novak_flightIndex[indFlight]) + 0.25f * indPt) * Novak_flight_cuve_length;
			prev_Novak_flight_control_points[indFlight][indPt].y = rand_x(Novak_flight_cuve_x_spread);
			prev_Novak_flight_control_points[indFlight][indPt].z = rand_x(Novak_flight_cuve_x_spread);
			Novak_flight_next_control_points(indFlight);
		}
	}
	for (int indFlight = 1; indFlight < PG_NB_FLIGHTS; indFlight++) {
		Novak_flight_deviation[indFlight] = glm::vec3(0, 0, 0);
		for (int indDim = 0; indDim < 3; indDim++) {
			for (int indSeed = 0; indSeed < 2; indSeed++) {
				cur_Novak_flightPerlinNoise[indFlight][indDim][indSeed] = rand_0_1 * 255;
			}
		}
	}
}

void Novak_cur_Novak_flight_2D_coordinates(int indFlight) {
	//float max_y = cur_Novak_flight_points[0].y;
	//float max_z = cur_Novak_flight_points[0].z;
	//float min_y = cur_Novak_flight_points[0].y;
	//float min_z = cur_Novak_flight_points[0].z;
	//for (int indFlight = 1; indFlight < PG_NB_FLIGHTS; indFlight++) {
	//	max_y = max(max_y, cur_Novak_flight_points[indFlight].y);
	//	max_z = max(max_z, cur_Novak_flight_points[indFlight].z);
	//	min_y = min(min_y, cur_Novak_flight_points[indFlight].y);
	//	min_z = min(min_z, cur_Novak_flight_points[indFlight].z);
	//}
	// the first flight is the leading one and is not displayed 
	// it is used to center the view on the other flights
	//glm::vec2 center = glm::vec2(cur_Novak_flight_points[0].y, cur_Novak_flight_points[0].z);
	//float maxcoord = max(max_y - min_y, max_z - min_z);
	float min_window_size = float(min(pg_workingWindow_width, PG_WINDOW_HEIGHT));
	// normalizing and centering on the first point of the other points
	// coordinates wrt to local origin
	//cur_Novak_flight_2D_points[indFlight].x = cur_Novak_flight_points[indFlight].y - min_y;
	//cur_Novak_flight_2D_points[indFlight].y = cur_Novak_flight_points[indFlight].z - min_z;
	cur_Novak_flight_2D_points[indFlight].x = cur_Novak_flight_points[indFlight].y;
	cur_Novak_flight_2D_points[indFlight].y = cur_Novak_flight_points[indFlight].z;
	// normalized coordinates so that they all fit inside the window
	//if (maxcoord > 0) {
	//	cur_Novak_flight_2D_points[indFlight].x /= maxcoord;
	//	cur_Novak_flight_2D_points[indFlight].y /= maxcoord;
	//}
	cur_Novak_flight_2D_points[indFlight].x *= min_window_size;
	cur_Novak_flight_2D_points[indFlight].y *= min_window_size;

	// points are centered around the first point
	// whose coordinates are (pg_workingWindow_width/2, PG_WINDOW_HEIGHT/2)
	// calculation of the translation for these coordinates
	// glm::vec2 translationToCenter = glm::vec2(pg_workingWindow_width / 2, PG_WINDOW_HEIGHT / 2) - cur_Novak_flight_2D_points[1];
	// coordinates wrt to local origin
	//cur_Novak_flight_2D_points[indFlight] = cur_Novak_flight_2D_points[indFlight] + translationToCenter;
	// coordinates wrt to screen center
	cur_Novak_flight_2D_points[indFlight] = cur_Novak_flight_2D_points[indFlight] + glm::vec2(pg_workingWindow_width / 2, PG_WINDOW_HEIGHT / 2);
}

void Novak_flight_update_coordinates(int indFlight) {
	for (int indPt = 0; indPt < 4; indPt++) {
		prev_Novak_flight_control_points[indFlight][indPt] = cur_Novak_flight_control_points[indFlight][indPt];
	}

	int flightIndex = int(floor(cur_Novak_flightTime[indFlight]));
	float alphaBezier = min(1.f, max(0.f, cur_Novak_flightTime[indFlight] - float(flightIndex)));
	if (flightIndex != cur_Novak_flightIndex[indFlight]) {
		//printf("Flight time %.2f (%.5f %.5f) index new %d cur %d alpha %.2f\n", cur_Novak_flightTime[indFlight], pg_CurrentClockTime, InitialFlightClockTime[indFlight], flightIndex, cur_Novak_flightIndex[indFlight], alphaBezier);
		cur_Novak_flightIndex[indFlight] = flightIndex;
		Novak_flight_next_control_points(indFlight);
	}
	Novak_yz_flight_deviation_from_trajectory(indFlight);

	pg_cubicBezier(cur_Novak_flight_control_points[indFlight], &cur_Novak_flight_points[indFlight], alphaBezier);
	//if (indFlight == 1) {
	//	printf("before flight deviation #1 %.2f %.2f %.2f\n", cur_Novak_flight_points[1].x, cur_Novak_flight_points[1].y, cur_Novak_flight_points[1].z);
	//}
	// no deviation is useful on the first flight which is the center of the display for the other flights
	//if (indFlight > 1) {
	cur_Novak_flight_points[indFlight] = cur_Novak_flight_points[indFlight] + Novak_flight_deviation[indFlight];
	//}
	//if (indFlight == 1) {
	//	printf("after flight deviation #1 %.2f %.2f %.2f\n\n", cur_Novak_flight_points[1].x, cur_Novak_flight_points[1].y, cur_Novak_flight_points[1].z);
	//}
	Novak_cur_Novak_flight_2D_coordinates(indFlight);
}
#endif

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and calculates the indices of the control points
// which build the Bezier curve convex hull 
void pg_Bezier_convex_hull(glm::vec2 control_points[4], int* hull) {
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
			if (pg_pointEquals(&control_points[hull_point], &control_points[end_point])
				|| pg_left_oriented(&control_points[hull_point], &control_points[end_point], &control_points[i])) {
				end_point = i;
			}
		}
		hull_point = end_point;
	}
	/*
	 * must compare coordinates values (and not simply objects)
	 * for the case of 4 co-incident points
	 */
	while (!pg_pointEquals(&control_points[end_point], &control_points[hull[0]]));
}

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and calculates the coordinates of the points
// of the Bezier curve convex hull expanded by a the radius of the pen
void pg_Bezier_hull_expanded_by_radius(glm::vec2 control_points[4], int* hull,
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

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and calculates the coordinates of the points
// of the Bezier curve bouding box expanded by a the radius of the pen
void pg_Bezier_boundingBox_expanded_by_radius(glm::vec2 control_points[4],
	float radius, glm::vec4* boundingBox) {
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

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and calculates the length
// of the Bezier curve
float pg_Bezier_length(glm::vec2 control_points[4], int nb_steps) {
	if (nb_steps <= 0) {
		sprintf(pg_errorStr, "Bezier length calculation must be made with positive steps %d!", nb_steps); pg_ReportError(pg_errorStr);
	}
	float returned_length = 0.f;
	glm::vec2 precPoint(0, 0);
	glm::vec2 curPoint(0, 0);
	pg_cubicBezier(control_points, &precPoint, 0.f);
	// polygon-based length calculation
	for (int ind = 1; ind <= nb_steps; ind++) {
		float alpha = float(ind) / float(nb_steps);
		pg_cubicBezier(control_points, &curPoint, alpha);
		returned_length += glm::distance(precPoint, curPoint);
		precPoint = curPoint;
	}
	return returned_length;
}

/////////////////////////////////////////////////////////////////////////////////
// Function that take input as Control Point x_coordinates and
// Control Point y_coordinates and calculates the xy coordinates
// of the Bezier curve bounding box
void pg_build_Bezier_bounding_box(int pathNo) {
	if (pathNo > PG_NB_PATHS) {
		return;
	}

	// bounding box shipped to the GPU
	pg_BezierControl[pathNo * 4 + 0] = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev);
	pg_BezierControl[pathNo * 4 + 1] = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_xL, pg_paths_currentDynPoint[pathNo].pg_paths_yL);
	pg_BezierControl[pathNo * 4 + 2] = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR);
	if (pathNo < fingers) {
		pg_BezierControl[pathNo * 4 + 3] = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU, pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU);
	}
	else {
		pg_BezierControl[pathNo * 4 + 3] = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
	}
	//if (pathNo == 1) {
	//	printf("Bezier in data 4 points %.1fx%.1f %.1fx%.1f %.1fx%.1f %.1fx%.1f \n",
	//		pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev,
	//		pg_paths_currentDynPoint[pathNo].pg_paths_xL, pg_paths_currentDynPoint[pathNo].pg_paths_yL,
	//		pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR,
	//		pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
	//}

	// BOUNDING SOLUTION
	// alternative possibility with a simple bounding box around the stroke
	pg_Bezier_boundingBox_expanded_by_radius(&(pg_BezierControl[pathNo * 4]),
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX * 1.1f, &(pg_BezierBox[pathNo]));
}

///////////////////////////////////////////////////////////////////////////////////////
// scene update
//////////////////////////////////////////////////////////////////
// REPLAY PATHS  
//////////////////////////////////////////////////////////////////

// replays a path with the same duration
// however since the time stamps are not saved in the ClipArt file, the 
// is uniform and does not match the exact initial speed 
// to make it better synchronized, it would be necessary
// to store the time stamp of each curve inside the ClipArt file
#define PG_SYNC_REPLAY 

// calculation of tangents from successive locations of the pen
void pg_stroke_Bezier_geometry_calculation(int pathNo, int curr_position_x, int curr_position_y) {
	pg_paths_currentDynPoint[pathNo].pg_paths_time_prev_prev = pg_paths_currentDynPoint[pathNo].pg_paths_time_prev;
	pg_paths_currentDynPoint[pathNo].pg_paths_x_prev_prev = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev;
	pg_paths_currentDynPoint[pathNo].pg_paths_y_prev_prev = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev;

	pg_paths_currentDynPoint[pathNo].pg_paths_time_prev = pg_paths_currentDynPoint[pathNo].pg_paths_time;
	pg_paths_currentDynPoint[pathNo].pg_paths_x_prev = pg_paths_currentDynPoint[pathNo].pg_paths_x;
	pg_paths_currentDynPoint[pathNo].pg_paths_y_prev = pg_paths_currentDynPoint[pathNo].pg_paths_y;

	pg_paths_currentDynPoint[pathNo].pg_paths_time = float(pg_CurrentClockTime);
	pg_paths_currentDynPoint[pathNo].pg_paths_x = pg_paths_currentDynPoint[pathNo].pg_paths_x_next;
	pg_paths_currentDynPoint[pathNo].pg_paths_y = pg_paths_currentDynPoint[pathNo].pg_paths_y_next;
	if (pathNo < fingers) {
		pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU = pg_paths_currentDynPoint[pathNo].pg_paths_x;
		pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU = pg_paths_currentDynPoint[pathNo].pg_paths_y;
	}

	pg_paths_currentDynPoint[pathNo].pg_paths_x_next = float(curr_position_x);
	pg_paths_currentDynPoint[pathNo].pg_paths_y_next = float(curr_position_y);
	// printf("/abs_pen_xy %.0f %.0f\n", float(curr_position_x), float(curr_position_y));
	//printf("pg_stroke_Bezier_geometry_calculation for path %d (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f)\n", pathNo, pg_paths_currentDynPoint[pathNo].pg_paths_x_prev_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev_prev, pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y, pg_paths_currentDynPoint[pathNo].pg_paths_x_next, pg_paths_currentDynPoint[pathNo].pg_paths_y_next);

#if defined(pg_Project_SilentDrawing)
	// sends the position of the cursor to the recorder for later replay
	sprintf(pg_AuxString, "/abs_pen_xy %.0f %.0f", float(pg_CurrentCursorPos_x[0]), float(pg_CurrentCursorPos_y[0]));
	pg_send_message_udp((char*)"ff", (char*)pg_AuxString, (char*)"udp_Record_send");
#endif

	// define the tangents
	// in the future, keep the previous tangent memory so that it is not recomputed twice

	// segments between two positions
	float u_x = (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev - pg_paths_currentDynPoint[pathNo].pg_paths_x_prev_prev);
	float u_y = (pg_paths_currentDynPoint[pathNo].pg_paths_y_prev - pg_paths_currentDynPoint[pathNo].pg_paths_y_prev_prev);
	float v_x = (pg_paths_currentDynPoint[pathNo].pg_paths_x - pg_paths_currentDynPoint[pathNo].pg_paths_x_prev);
	float v_y = (pg_paths_currentDynPoint[pathNo].pg_paths_y - pg_paths_currentDynPoint[pathNo].pg_paths_y_prev);
	float w_x = (pg_paths_currentDynPoint[pathNo].pg_paths_x_next - pg_paths_currentDynPoint[pathNo].pg_paths_x);
	float w_y = (pg_paths_currentDynPoint[pathNo].pg_paths_y_next - pg_paths_currentDynPoint[pathNo].pg_paths_y);

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
	pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev = (u_x + v_x);
	pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev = (u_y + v_y);
	pg_paths_currentDynPoint[pathNo].pg_paths_tang_x = (v_x + w_x);
	pg_paths_currentDynPoint[pathNo].pg_paths_tang_y = (v_y + w_y);
	float norm_tang_prev = sqrt(pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev * pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev + pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev * pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev);
	float norm_tang = sqrt(pg_paths_currentDynPoint[pathNo].pg_paths_tang_x * pg_paths_currentDynPoint[pathNo].pg_paths_tang_x + pg_paths_currentDynPoint[pathNo].pg_paths_tang_y * pg_paths_currentDynPoint[pathNo].pg_paths_tang_y);
	norm_v /= 3.f; // tangents are the third of the length of the segment
	if (norm_tang_prev != 0 && norm_v != 0) {
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev = pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev * norm_v / norm_tang_prev;
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev = pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev * norm_v / norm_tang_prev;
	}
	if (norm_tang != 0 && norm_v != 0) {
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_x = pg_paths_currentDynPoint[pathNo].pg_paths_tang_x * norm_v / norm_tang;
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_y = pg_paths_currentDynPoint[pathNo].pg_paths_tang_y * norm_v / norm_tang;
	}

	// control points from positions and tangents for current and preceding positions
	pg_paths_currentDynPoint[pathNo].pg_paths_xL = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev + pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev;
	pg_paths_currentDynPoint[pathNo].pg_paths_yL = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev + pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev;
	pg_paths_currentDynPoint[pathNo].pg_paths_xR = pg_paths_currentDynPoint[pathNo].pg_paths_x - pg_paths_currentDynPoint[pathNo].pg_paths_tang_x;
	pg_paths_currentDynPoint[pathNo].pg_paths_yR = pg_paths_currentDynPoint[pathNo].pg_paths_y - pg_paths_currentDynPoint[pathNo].pg_paths_tang_y;

	// possible resize to avoid crossing
	// non-crossing <=> both points prev and left on the same size of the line (right, current)
	// if N is the normal to (right, current), (vec(curr prev).N) * (vec(curr L).N) >= 0
	int n = 4;
	while ((pg_paths_currentDynPoint[pathNo].pg_paths_tang_y * (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev - pg_paths_currentDynPoint[pathNo].pg_paths_x) - pg_paths_currentDynPoint[pathNo].pg_paths_tang_x * (pg_paths_currentDynPoint[pathNo].pg_paths_y_prev - pg_paths_currentDynPoint[pathNo].pg_paths_y)) // vec(curr prev).N
		* (pg_paths_currentDynPoint[pathNo].pg_paths_tang_y * (pg_paths_currentDynPoint[pathNo].pg_paths_xL - pg_paths_currentDynPoint[pathNo].pg_paths_x) - pg_paths_currentDynPoint[pathNo].pg_paths_tang_x * (pg_paths_currentDynPoint[pathNo].pg_paths_yL - pg_paths_currentDynPoint[pathNo].pg_paths_y)) // vec(curr L).N
		< 0 && n > 0) {
		// tangents are made shorter
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev /= 2.f;
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev /= 2.f;
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_x /= 2.f;
		pg_paths_currentDynPoint[pathNo].pg_paths_tang_y /= 2.f;

		// control points from positions and tangents for current and preceding positions
		pg_paths_currentDynPoint[pathNo].pg_paths_xL = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev + pg_paths_currentDynPoint[pathNo].pg_paths_tang_x_prev;
		pg_paths_currentDynPoint[pathNo].pg_paths_yL = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev + pg_paths_currentDynPoint[pathNo].pg_paths_tang_y_prev;
		pg_paths_currentDynPoint[pathNo].pg_paths_xR = pg_paths_currentDynPoint[pathNo].pg_paths_x - pg_paths_currentDynPoint[pathNo].pg_paths_tang_x;
		pg_paths_currentDynPoint[pathNo].pg_paths_yR = pg_paths_currentDynPoint[pathNo].pg_paths_y - pg_paths_currentDynPoint[pathNo].pg_paths_tang_y;

		n--;
	}

	// random angle for cristal effect used in pg_Project_Pierres
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_angle_pulse]) {
		if (pen_angle_pulse > 0) {
			pg_paths_currentDynPoint[pathNo].pg_paths_x_prev += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_y_prev += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_x_next += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_y_next += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_x += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_y += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_xL += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_yL += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_xR += rand_0_1 * pen_angle_pulse * 10;
			pg_paths_currentDynPoint[pathNo].pg_paths_yR += rand_0_1 * pen_angle_pulse * 10;
		}
	}

	if (!tactile_tablet) { // not a tactile tablet
		// line begin or line end between inscreen and offscreen positions
		// off screen positions also correspond to mouseover flights
		// begin
		if (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev_prev == PG_OUT_OF_SCREEN_CURSOR && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev_prev == PG_OUT_OF_SCREEN_CURSOR
			&& pg_paths_currentDynPoint[pathNo].pg_paths_x_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_x >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y >= 0) {
			pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = true;
		}
		else {
			pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = false;
		}
		// end
		if (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_x >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y >= 0
			&& pg_paths_currentDynPoint[pathNo].pg_paths_x_next == PG_OUT_OF_SCREEN_CURSOR && pg_paths_currentDynPoint[pathNo].pg_paths_y_next == PG_OUT_OF_SCREEN_CURSOR) {
			pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = true;
		}
		else {
			pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = false;
		}
	}
	// for a tactile entry, there is no mouseover and the path has to be broken if the distance between two consecutive points is too big
	else { // tactile tablet
		// line begin or line end between two distant points or for a shorter distance but with a long waiting time
		glm::vec2 prev = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev);
		glm::vec2 cur = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
		glm::vec2 next = glm::vec2(pg_paths_currentDynPoint[pathNo].pg_paths_x_next, pg_paths_currentDynPoint[pathNo].pg_paths_y_next);
		//printf("dist prev %.2f cur %.2f\n", distance(prev, cur), distance(cur, next));

		// begin
		//printf("distance %.2f %.2f temps %.2f\n", distance(prev, cur), distance(next, cur), pg_CurrentClockTime - pg_LastCursorPositionUpdate[pathNo]);
		if (glm::distance(prev, cur) > 200.0f || (glm::distance(prev, cur) > 10.0f && (pg_CurrentClockTime - pg_LastCursorPositionUpdate[pathNo] > 0.3))) {
			pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = true;
			pg_paths_currentDynPoint[pathNo].pg_paths_x_prev_prev = PG_OUT_OF_SCREEN_CURSOR; pg_paths_currentDynPoint[pathNo].pg_paths_y_prev_prev = PG_OUT_OF_SCREEN_CURSOR;
			//printf("********** BEGIN *************\n");
		}
		else {
			pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = false;
		}
		// end
		if (glm::distance(cur, next) > 200.0f || (glm::distance(cur, next) > 10.0f && (pg_CurrentClockTime - pg_LastCursorPositionUpdate[pathNo] > 0.3))) {
			pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = true;
			pg_paths_currentDynPoint[pathNo].pg_paths_x = PG_OUT_OF_SCREEN_CURSOR; pg_paths_currentDynPoint[pathNo].pg_paths_y = PG_OUT_OF_SCREEN_CURSOR;
			//printf("********** END *************\n");
		}
		else {
			pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = false;
		}
		pg_LastCursorPositionUpdate[pathNo] = float(pg_CurrentClockTime);
	}
}

// path scaling
void pg_path_scale_wrtScreenCenter(float* scaled_Xcoord, float* scaled_Ycoord, float orig_Xcoord, float orig_Ycoord, float scaleX, float scaleY) {
	if (scaleX != 0) {
		*scaled_Xcoord = orig_Xcoord * scaleX + 0.5f * pg_workingWindow_width * (1.f / scaleX - 1.f) * scaleX;
		//*scaled_Xcoord = orig_Xcoord * scaleX;
	}
	if (scaleY != 0) {
		*scaled_Ycoord = orig_Ycoord * scaleY + 0.5f * PG_WINDOW_HEIGHT * (1.f / scaleY - 1.f) * scaleY;
		//*scaled_Ycoord = orig_Ycoord * scaleY;
	}
}
// path tanslation
void pg_path_transl(float* translated_Xcoord, float* translated_Ycoord, float orig_Xcoord, float orig_Ycoord, float translX, float translY) {
	*translated_Xcoord = orig_Xcoord + translX;
	*translated_Ycoord = orig_Ycoord + translY;
}

// replays a path that has been loaded or previously recorded
void pg_replay_one_path(int pathNo, double theTime) {
	//printf("replay path %d ind reading %d nb Frames %d\n" , pathNo, 
		//pg_Path_Status[pathNo].path_indReading, pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario));

	// does not replay a path if the path replay is active for a multitouch interaction
	if (pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario) <= 0) {
		return;
	}

	// does not replay a path if the path replay is active for a multitouch interaction
	if (freeze) {
		pg_Path_Status[pathNo].path_initialTimeReading += pg_CurrentClockTime - pg_PrecedingClockTime;
		pg_Path_Status[pathNo].path_lastPlayedFrameTime += pg_CurrentClockTime - pg_PrecedingClockTime;
		return;
	}

	// records the initial time and uses the elapsed reading
	// time and the elapsed recording time to play in synch
	if (pg_Path_Status[pathNo].path_isFirstFrame) {
		pg_Path_Status[pathNo].path_initialTimeReading = theTime;
		pg_Path_Status[pathNo].path_lastPlayedFrameTime = theTime;
		pg_Path_Status[pathNo].path_isFirstFrame = false;
		pg_Path_Status[pathNo].path_indPreviousReading = 0;
	}

	// does not advance the path if the speed is null or negative
	double readingSpeed = pg_Path_Status[pathNo].get_path_curve_readSpeedScale(pg_ind_scenario);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_replay_speed]) {
		readingSpeed *= path_replay_speed;
		// printf("reading speed of path %d is replay speed %.2f readingSpeed %.2f\n", pathNo, path_replay_speed, readingSpeed);
	}
	if (readingSpeed <= 0) {
		//printf("reading speed of path %d is <=0 replay speed %.2f\n", pathNo, path_replay_speed);
		return;
	}
	//printf("reading speed %.2f\n", readingSpeed);

	bool isCurveBreakBegin = false;
	bool isCurveBreakEnd = false;
	pg_Path_Status[pathNo].path_lastPlayedindReading = pg_Path_Status[pathNo].path_indReading;
#if defined(PG_SYNC_REPLAY)
	double playingTimeSinceLastPlayedFrame = (theTime - pg_Path_Status[pathNo].path_lastPlayedFrameTime);
	double lastPlayedFrameRecordingTime = pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, pg_Path_Status[pathNo].path_lastPlayedindReading);
	bool newFrame = false;
	// if minimally one frame has elapsed
	do {
		//printf("reading speed %.2f\n", readingSpeed);
		double recordingTimeSinceLastPlayedFrame;
		if (pg_Path_Status[pathNo].path_indReading > 0) {
			recordingTimeSinceLastPlayedFrame = pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading)
				- lastPlayedFrameRecordingTime;
			//printf("* current recordingTimeSinceLastPlayedFrame %.2f cur frame %.2f prece frame %.2f ind reading cur %d prec %d\n",
			//	recordingTimeSinceLastPlayedFrame, pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, 
			//		pg_Path_Status[pathNo].path_indReading), pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, 
			//			pg_Path_Status[pathNo].path_lastPlayedindReading), pg_Path_Status[pathNo].path_indReading, pg_Path_Status[pathNo].path_lastPlayedindReading);
		}
		else if (pg_Path_Status[pathNo].path_indReading == 0) {
			recordingTimeSinceLastPlayedFrame = pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading)
				- pg_Path_Status[pathNo].get_path_curve_initialTimeRecording(pg_ind_scenario);
			//printf("Initial recording time since last frame %.2f playingTimeSinceLastPlayedFrame %.2f\n", recordingTimeSinceLastPlayedFrame, playingTimeSinceLastPlayedFrame);
		}
		else { // if (pg_PathCurve_Params[pg_ind_scenario][pathNo].path_indReading < 0)
			//printf("empty path\n");
			return;
		}

		//if (pathNo == 1) {
		//	printf("Ind %d frame no %d rec time %.4f read time %.4f speed %.2f\n", pathNo, pg_Path_Status[pathNo].path_indReading,
		//		recordingTimeSinceLastPlayedFrame, playingTimeSinceLastPlayedFrame, readingSpeed);
		//}
		// the negtive values correspond to a curve break. If they are jumped over the first 
		// following point should be negative
		if (pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading) < 0
			|| pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading) < 0) {
			isCurveBreakEnd = true;
		}
		if (pg_Path_Status[pathNo].getFramePositionX_prev(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading) < 0
			|| pg_Path_Status[pathNo].getFramePositionY_prev(pg_ind_scenario, pg_Path_Status[pathNo].path_indReading) < 0) {
			isCurveBreakBegin = true;
		}

		recordingTimeSinceLastPlayedFrame /= readingSpeed;

		if (recordingTimeSinceLastPlayedFrame < playingTimeSinceLastPlayedFrame) {
			pg_Path_Status[pathNo].path_indReading++;
			newFrame = true;
			//if (pathNo == 1) {
			//	printf("increment frame %d\n", pg_Path_Status[pathNo].path_indReading);
			//}
		}
		else { // recordingTimeSinceLastPlayedFrame >= playingTimeSinceLastPlayedFrame
			// the next frame is played slightly ahead because the recording time since last played frame is  lightly longer than the effective playing time
			if (newFrame) {
				pg_Path_Status[pathNo].path_lastPlayedFrameTime = theTime + (recordingTimeSinceLastPlayedFrame - playingTimeSinceLastPlayedFrame) / readingSpeed;
			}
			pg_Path_Status[pathNo].path_lastPlayedindReading = pg_Path_Status[pathNo].path_indReading;
			//if (pathNo == 1) {
			//	printf("exit frame %d\n", pg_Path_Status[pathNo].path_indReading);
			//}
			break;
		}

		// loopiing or stopping in the end when last frame is reached
		if (path_replay_loop == true) {
			// loops at the end
			if (pg_Path_Status[pathNo].path_indReading >= pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) {
				pg_Path_Status[pathNo].path_indReading = 0;
				pg_Path_Status[pathNo].path_isFirstFrame = true;
				isCurveBreakEnd = true;
				break;
			}
		}
		else {
			// stops at the end
			if (pg_Path_Status[pathNo].path_indReading >= pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) {
				pg_path_replay_trackNo_onOff(pathNo, -1);
				pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = true;
				pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = false;
				printf("path %d off at %.3f\n", pathNo, pg_CurrentClockTime);
				return;
			}
		}
		//printf("recordingTimeSinceLastPlayedFrame %.2f playingTimeSinceLastPlayedFrame %.2f ind reading %d\n",
		//	recordingTimeSinceLastPlayedFrame, playingTimeSinceLastPlayedFrame, pg_Path_Status[pathNo].path_indReading);
	} while (true);
#else
	// the negtive values correspond to a curve break. If they are jumped over the first 
	// following point should be negative
	if (pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, .path_Pos_x[pg_Path_Status[pathNo].path_indReading) < 0
		|| pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, .path_Pos_y[pg_Path_Status[pathNo].path_indReading) < 0) {
		isCurveBreakEnd = true;
	}
	if (pg_Path_Status[pathNo].getFramePositionX_prev(pg_ind_scenario, .Pos_x_prev[pg_Path_Status[pathNo].path_indReading) < 0
		|| pg_Path_Status[pathNo].getFramePositionY_prev(pg_ind_scenario, .Pos_y_prev[pg_Path_Status[pathNo].path_indReading) < 0) {
		isCurveBreakBegin = true;
	}

	if (path_replay_loop == true) {
		// loops at the end
		if (pg_Path_Status[pathNo].path_indReading
			>= pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario) {
			pg_Path_Status[pathNo].path_indReading = 0;
			isCurveBreakEnd = true;
			break;
		}
	}
	else {	// stops at the end
		if (pg_Path_Status[pathNo].path_indReading
			>= pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) {
			pg_Path_Status[pathNo].path_indReading = pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario) - 1;
			isCurveBreakEnd = false;
			break;
		}
	}
	pg_Path_Status[pathNo].path_indReading++;
#endif

	///////////////////////////////////////////////////////////
	// previous frame for tangent and position
	// management of previous mouse position with
	// consideration for looping
	int indFrameReading = pg_Path_Status[pathNo].path_indReading;
	if (isCurveBreakBegin) {
		//pg_Path_Status[pathNo].path_indPreviousReading = 0;
		//indFrameReading = pg_Path_Status[pathNo].path_indPreviousReading + 1;
		pg_paths_currentDynPoint[pathNo].pg_paths_x_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_y_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_xL = 0.f;
		pg_paths_currentDynPoint[pathNo].pg_paths_yL = 0.f;
	}
	else {
		pg_paths_currentDynPoint[pathNo].pg_paths_x_prev = pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_y_prev = pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_translX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_translY]) {
			pg_path_transl(&pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, &pg_paths_currentDynPoint[pathNo].pg_paths_y_prev, pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev,
				path_translX, path_translY);
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleY]) {
			pg_path_scale_wrtScreenCenter(&pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, &pg_paths_currentDynPoint[pathNo].pg_paths_y_prev, pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev,
				path_scaleX, path_scaleY);
		}
		pg_paths_currentDynPoint[pathNo].pg_paths_xL = 2 * (float)pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading) - (float)pg_Path_Status[pathNo].getFramePositionXR(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_yL = 2 * (float)pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading) - (float)pg_Path_Status[pathNo].getFramePositionYR(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_translX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_translY]) {
			pg_path_transl(&pg_paths_currentDynPoint[pathNo].pg_paths_xL, &pg_paths_currentDynPoint[pathNo].pg_paths_yL, pg_paths_currentDynPoint[pathNo].pg_paths_xL, pg_paths_currentDynPoint[pathNo].pg_paths_yL,
				path_translX, path_translY);
		}
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleY]) {
			pg_path_scale_wrtScreenCenter(&pg_paths_currentDynPoint[pathNo].pg_paths_xL, &pg_paths_currentDynPoint[pathNo].pg_paths_yL, pg_paths_currentDynPoint[pathNo].pg_paths_xL, pg_paths_currentDynPoint[pathNo].pg_paths_yL,
				path_scaleX, path_scaleY);
		}
	}
	//printf("prev frame %d curr frame %d\n", pg_Path_Status[pathNo].path_indPreviousReading, indFrameReading);

	///////////////////////////////////////////////////////////
	// next frame for tangent and position
	// negative values in case of curve break
	if (isCurveBreakEnd) {
		pg_paths_currentDynPoint[pathNo].pg_paths_xR = 0.f;
		pg_paths_currentDynPoint[pathNo].pg_paths_yR = 0.f;
		//printf("curve end\n");
		pg_paths_currentDynPoint[pathNo].pg_paths_x = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[pathNo].pg_paths_y = PG_OUT_OF_SCREEN_CURSOR;
	}
	else {
		pg_paths_currentDynPoint[pathNo].pg_paths_xR = pg_Path_Status[pathNo].getFramePositionXR(pg_ind_scenario, indFrameReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_yR = pg_Path_Status[pathNo].getFramePositionYR(pg_ind_scenario, indFrameReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_x = pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrameReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_y = pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrameReading);
		//printf("Path %d Point ini %.2f %.2f %.2f %.2f\n",
		//	pathNo, pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_translX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_translY]) {
			pg_path_transl(&pg_paths_currentDynPoint[pathNo].pg_paths_xR, &pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR,
				path_translX, path_translY);
			pg_path_transl(&pg_paths_currentDynPoint[pathNo].pg_paths_x, &pg_paths_currentDynPoint[pathNo].pg_paths_y, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y,
				path_translX, path_translY);
		}
		//printf("Path %d Point transl %.2f %.2f %.2f %.2f\n",
		//	pathNo, pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleX]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_path_scaleY]) {
			pg_path_scale_wrtScreenCenter(&pg_paths_currentDynPoint[pathNo].pg_paths_xR, &pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR,
				path_scaleX, path_scaleY);
			pg_path_scale_wrtScreenCenter(&pg_paths_currentDynPoint[pathNo].pg_paths_x, &pg_paths_currentDynPoint[pathNo].pg_paths_y, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y,
				path_scaleX, path_scaleY);
		}
		//printf("Path %d Point scale %.2f %.2f %.2f %.2f\n",
		//	pathNo, pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);
	}

	///////////////////////////////////////////////////////////
	// line begin or line end
	// begin
	if ((pg_Path_Status[pathNo].path_indPreviousReading > 0
		&& pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading - 1) < 0
		&& pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, pg_Path_Status[pathNo].path_indPreviousReading - 1) < 0)
		&& pg_paths_currentDynPoint[pathNo].pg_paths_x_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_x >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y >= 0) {
		pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = true;
	}
	else {
		pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = false;
	}

	// end
	if (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_x >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y >= 0
		&& pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, (indFrameReading + 1) % pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) < 0
		&& pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, (indFrameReading + 1) % pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) < 0) {
		pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = true;
	}
	else {
		pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = false;
	}

	//printf("Path %d Point %.2f %.2f %.2f %.2f frame %d %d tot frames %d (curve %d) begin/end %d/%d %d/%d \n", 
	//	pathNo, 
	//	pg_paths_currentDynPoint[pathNo].pg_paths_xR,
	//	pg_paths_currentDynPoint[pathNo].pg_paths_yR,
	//	pg_paths_currentDynPoint[pathNo].pg_paths_x,
	//	pg_paths_currentDynPoint[pathNo].pg_paths_y,
	//	pg_Path_Status[pathNo].path_indPreviousReading,
	//	indFrameReading,
	//	pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario),
	//	pg_Path_Status[pathNo].path_currentPathCurve[pg_ind_scenario],
	//	pg_paths_currentDynPoint[pathNo].pg_paths_isBegin, pg_paths_currentDynPoint[pathNo].pg_paths_isEnd,
	//	isCurveBreakBegin,
	//	isCurveBreakEnd);

	//printf("B0 %.2f %.2f  B1 %.2f %.2f  B2 %.2f %.2f B3 %.2f %.2f \n\n", pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev, pg_paths_currentDynPoint[pathNo].pg_paths_xL, pg_paths_currentDynPoint[pathNo].pg_paths_yL, 
	//	pg_paths_currentDynPoint[pathNo].pg_paths_xR, pg_paths_currentDynPoint[pathNo].pg_paths_yR, pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y);

	// management of color (w/wo possible interpolation)
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_hue_replay]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_saturation_replay]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_pen_value_replay]
		&& pen_hsv) {
		if (pen_saturation_replay == 0 && pen_saturation_replay_pulse == 0
			&& pen_hue_replay == 0 && pen_hue_replay_pulse == 0
			&& pen_value_replay == 0 && pen_value_replay_pulse == 0) {
			pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = (float)pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrameReading);
			pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = (float)pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrameReading);
			pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = (float)pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrameReading);
			//printf("RGB: %.2f %.2f %.2f \n", pg_paths_currentDynPoint[pathNo].pg_paths_Color_r, pg_paths_currentDynPoint[pathNo].pg_paths_Color_g, pg_paths_currentDynPoint[pathNo].pg_paths_Color_b);
		}
		else {
			float r = (float)pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrameReading);
			float g = (float)pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrameReading);
			float b = (float)pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrameReading);

			float h, s, v;
			pg_RGBtoHSV(r, g, b, &h, &s, &v);

			float h_pulsed, s_pulsed, v_pulsed;
			h_pulsed = pen_hue_replay
				* (1.f + pulse_average * pen_hue_replay_pulse);
			s_pulsed = pen_saturation_replay
				* (1.f + pulse_average * pen_saturation_replay_pulse);
			v_pulsed = pen_value_replay
				* (1.f + pulse_average * pen_value_replay_pulse);
			s += s_pulsed;
			v += v_pulsed;
			h += h_pulsed;
			s = max(min(s, 1.f), 0.f);
			v = max(min(v, 1.f), 0.f);
			h = max(min(h, 1.f), 0.f);

			sprintf(pg_AuxString, "/pen_hue_replay %.5f", h_pulsed); pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("%s\n", pg_AuxString);
			sprintf(pg_AuxString, "/pen_value_replay %.5f", v_pulsed); pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("%s\n", pg_AuxString);
			sprintf(pg_AuxString, "/pen_saturation_replay %.5f", s_pulsed); pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			//printf("%s\n", pg_AuxString);

			pg_HSVtoRGB(h, s, v, &pg_paths_currentDynPoint[pathNo].pg_paths_Color_r, &pg_paths_currentDynPoint[pathNo].pg_paths_Color_g, &pg_paths_currentDynPoint[pathNo].pg_paths_Color_b);

			//printf("RGB: In %.2f %.2f %.2f OUT SV %.2f %.2f OUT %.2f %.2f %.2f \n", r, g, b, s, v, pg_paths_currentDynPoint[pathNo].pg_paths_Color_r, pg_paths_currentDynPoint[pathNo].pg_paths_Color_g, pg_paths_currentDynPoint[pathNo].pg_paths_Color_b);
		}
	}
	else {
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = (float)pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrameReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = (float)pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrameReading);
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = (float)pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrameReading);
	}

	pg_paths_currentDynPoint[pathNo].pg_paths_Color_a = (float)pg_Path_Status[pathNo].getFrameColor_a(pg_ind_scenario, indFrameReading);

	//if (pg_Path_Status[pathNo].path_indPreviousReading < indFrameReading - 1) {
	//	pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = 1.f;
	//	pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = 0.f;
	//	pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = 0.f;
	//}

	// management of brush ID (w/wo possible interpolation)
	int brushNo = pg_Path_Status[pathNo].getFrameBrush(pg_ind_scenario, indFrameReading);
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_brush_replay]) {
		brushNo += pen_brush_replay;
	}
	while (brushNo < 0) {
		brushNo += (nb_pen_brushes[pg_ind_scenario] * 3);
	}
	pg_paths_currentDynPoint[pathNo].pg_paths_BrushID = brushNo % (nb_pen_brushes[pg_ind_scenario] * 3);
	// management of brush radius (w/wo possible interpolation)
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = (float)pg_Path_Status[pathNo].getFrameRadiusX(pg_ind_scenario, indFrameReading) * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = (float)pg_Path_Status[pathNo].getFrameRadiusY(pg_ind_scenario, indFrameReading) * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);

	//if (pg_is_path_replay[pathNo]) {
	//	printf("replay #%d %.2f %.2f rad %.2f pen_radius_replay %.2f color %.2f %.2f %.2f %.2f brush %d\n", pathNo,
	//		pg_paths_currentDynPoint[pathNo].pg_paths_x, pg_paths_currentDynPoint[pathNo].pg_paths_y, pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX, pen_radius_replay, pg_paths_currentDynPoint[pathNo].pg_paths_Color_r, pg_paths_currentDynPoint[pathNo].pg_paths_Color_g, pg_paths_currentDynPoint[pathNo].pg_paths_Color_b, pg_paths_currentDynPoint[pathNo].pg_paths_Color_a, pg_paths_currentDynPoint[pathNo].pg_paths_BrushID);
	//}

	pg_Path_Status[pathNo].path_indPreviousReading = indFrameReading;
}

// outputs the trace of a metawear sensor
void pg_play_metawear_trajectory(int pathNo, int sensorNo) {
	if (!pg_is_path_replay[pathNo]) {
		pg_is_path_replay[pathNo] = true;
		// path 1 (senosr 1 sensorNo 0) and 2 (sensor 2 sensorNo 1) replays on currentDrawingTrack + 1
		pg_path_replay_trackNo_start(pathNo, currentDrawingTrack + 1);
		path_replay_trackNo[pathNo] = currentDrawingTrack + 1;
		//printf("start replay path %d (sensor %d) on track %d\n", pathNo, pathNo, path_replay_trackNo[pathNo]);
	}
	pg_stroke_Bezier_geometry_calculation(pathNo, int(pg_mw_sensors[sensorNo].mw_mss_pos[0]), int(pg_mw_sensors[sensorNo].mw_mss_pos[1]));
	//printf("new sensor position %d path %d: %.1f %.1f replay %d\n", sensorNo, pathNo, 
	//	(pg_mw_sensors[sensorNo].mw_mss_pos[0]), (pg_mw_sensors[sensorNo].mw_mss_pos[1]), int(pg_is_path_replay[pathNo]));
	pg_mw_sensors[sensorNo].mw_mss_pos_update = false;

	// management of brush radius (w/wo possible interpolation)
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = pen_radius;
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = pen_radius;
	// management of brush radius (w/wo possible interpolation)
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = pen_radius * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);
	pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = pen_radius * pen_radius_replay
		* (1.f + pulse_average * pen_radius_replay_pulse);
}

// PATHS REPLAY
void pg_replay_paths(double theTime) {
	for (int pathNo = 1; pathNo <= PG_NB_PATHS; pathNo++) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pg_metawear]) {
			if (pg_metawear) {
				int ind_sensor = pathNo - 1;
				if (ind_sensor < PG_MW_NB_MAX_SENSORS) {
					if (pg_mw_sensors[ind_sensor].mw_mss_pos_update == true) {
						pg_play_metawear_trajectory(pathNo, ind_sensor);
					}
					continue;
				}
			}
		}

		// active reading
#if defined(var_Novak_flight_on)
		//printf("pathNo %d replay %d \n", pathNo, pg_is_path_replay[pathNo]);
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_Novak_flight_on]
			&& pg_is_path_replay[pathNo] && Novak_flight_on) {
			//printf("pathNo %d flight clock %.2f %.2f\n", pathNo, pg_CurrentClockTime, prev_Novak_flightCurrentCLockTime[pathNo]);
			if (pg_CurrentClockTime != prev_Novak_flightCurrentCLockTime[pathNo]) {
				float newFlightTime = float(pg_CurrentClockTime - prev_Novak_flightCurrentCLockTime[pathNo]) * Novak_flight_speed + cur_Novak_flightTime[pathNo];
				//printf("Drawing flight %d cur clock %.2f prev clock %.2f prev flight time %.2f new Flight time %.2f\nw",
				//	pathNo, pg_CurrentClockTime, prev_Novak_flightCurrentCLockTime[pathNo], cur_Novak_flightTime[pathNo], newFlightTime);
				prev_Novak_flightCurrentCLockTime[pathNo] = pg_CurrentClockTime;
				prev_Novak_flightTime[pathNo] = cur_Novak_flightTime[pathNo];
				cur_Novak_flightTime[pathNo] = newFlightTime;
				// calculates the new value for cur_Novak_flight_2D_points
				Novak_flight_update_coordinates(pathNo);
				// assigns the value to the coordinates of the paths 0 to PG_NB_FLIGHTS - 1
				//printf("flight screen coordinates %d x %d\n",
				//	int(cur_Novak_flight_2D_points[pathNo].x), int(cur_Novak_flight_2D_points[pathNo].y));
				//printf("flight screen absolute coordinates %d x %d\n",
				//	int(cur_Novak_flight_points[pathNo].x), int(cur_Novak_flight_points[pathNo].y));
				float pulsed_Novak_flight_color[4];
				pg_compute_pulsed_palette_color(pen_color_replay, pen_color_replay_pulse, pen_grey_replay, pen_grey_replay_pulse, pulsed_Novak_flight_color, pg_enum_PEN_COLOR);

				pg_stroke_Bezier_geometry_calculation(pathNo, int(cur_Novak_flight_2D_points[pathNo].x), int(cur_Novak_flight_2D_points[pathNo].y));
				// tells the shader that it should be drawn on the current drawing track
				if (path_replay_trackNo[pathNo] < 0) {
					path_replay_trackNo[pathNo] = currentDrawingTrack;
				}
				// management of brush radius (w/wo possible interpolation)
				pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX
					= (pen_radius + pulse_average * pen_radius_pulse) * pen_radius_replay;
				pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY
					= (pen_radius + pulse_average * pen_radius_pulse) * pen_radius_replay;
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = min(1.f, pulsed_Novak_flight_color[0]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = min(1.f, pulsed_Novak_flight_color[1]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = min(1.f, pulsed_Novak_flight_color[2]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_a = min(1.f, 1.f);
				//printf("flight radius %.2f pen_radius, pen_radius_replay  %.2f %.2f %.2f\n", pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX, pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY, pen_radius, pen_radiusMultiplier, pen_radius_replay);
				//pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = 2.f;
				//pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = 2.f;
			}
		}
#else
		if (pg_is_path_replay[pathNo]) {
			pg_replay_one_path(pathNo, theTime);
		}
#endif
	}
}


//////////////////////////////////////////////////////////////////
// PEN STROKE AND PATH REPLAY  
//////////////////////////////////////////////////////////////////
void pg_update_pulsed_colors_and_replay_paths(double theTime) {
	// change colors according to music pg_audio_pulse
	pg_update_pulsed_colors();

	// replays recoded paths
	pg_replay_paths(theTime);


	///////////////////////////////////////////////////////////////////////
	// PARTICLE COLOR UPDATING INDEPENDENTLY OF TRACK READING OR WRITING
	///////////////////////////////////////////////////////////////////////
	/// uses pulsed color to repop
	pg_repop_ColorBGcolorRed
		= min(1.f, pg_pulsed_repop_colorBG[0]);
	pg_repop_ColorBG_g
		= min(1.f, pg_pulsed_repop_colorBG[1]);
	pg_repop_ColorBG_b
		= min(1.f, pg_pulsed_repop_colorBG[2]);
	pg_repop_ColorCA_r
		= min(1.f, pg_pulsed_repop_colorCA[0]);
	pg_repop_ColorCA_g
		= min(1.f, pg_pulsed_repop_colorCA[1]);
	pg_repop_ColorCA_b
		= min(1.f, pg_pulsed_repop_colorCA[2]);
	pg_repop_ColorPart_r
		= min(1.f, pg_pulsed_repop_colorPart[0]);
	pg_repop_ColorPart_g
		= min(1.f, pg_pulsed_repop_colorPart[1]);
	pg_repop_ColorPart_b
		= min(1.f, pg_pulsed_repop_colorPart[2]);
	// printf("repop color %.2f %.2f %.2f \n", pg_repop_ColorPart_r, pg_repop_ColorPart_g, pg_repop_ColorPart_b);
	if (invertAllLayers) {
	}

	///////////////////////////////////////////////////////////////////////
	// PEN PARAMETERS IN PATH 0-(fingers - 1)
	///////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// draws from mouse position 
	for (int pathNo = 0; pathNo < fingers; pathNo++) {
		/// uses pulsed color to draw
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = min(1.f, pg_pulsed_pen_color[0]);
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = min(1.f, pg_pulsed_pen_color[1]);
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = min(1.f, pg_pulsed_pen_color[2]);
		pg_paths_currentDynPoint[pathNo].pg_paths_Color_a = min(1.f, pen_color_a);
		//printf("PEN color a %.2f %.2f %.2f %.2f\n", pg_paths_currentDynPoint[pathNo].pg_paths_Color_r ,
		//	   pg_paths_currentDynPoint[1].pg_paths_Color_g , pg_paths_currentDynPoint[2].pg_paths_Color_b , pg_paths_currentDynPoint[3].pg_paths_Color_a );

		pg_paths_currentDynPoint[pathNo].pg_paths_BrushID = pen_brush;
#if defined(PG_WACOM_TABLET)
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX
			= pen_radius + pulse_average * pen_radius_pulse
			+ pg_CurrentStylusPresuse * pen_radius_pressure_coef
			+ fabs(sin(pg_CurrentStylusAzimut)) * pg_CurrentStylusInclination * pen_radius_angleHor_coef;
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY
			= pen_radius + pulse_average * pen_radius_pulse
			+ pg_CurrentStylusPresuse * pen_radius_pressure_coef
			+ fabs(cos(pg_CurrentStylusAzimut)) * pg_CurrentStylusInclination * pen_radius_angleVer_coef;
		//if (pathNo == 0) {
		//	printf("message %s\n", pg_AuxString);
		//	printf("PEN pen_radius %f pulse_average pen_radius_pulse %.2f %.2f muliplied radius %.2f en radius calculated [%.2f %.2f]\n",
		//		pen_radius, pulse_average, pen_radius_pulse, pen_radius, pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX, pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY);
		//}
#else
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX = pen_radius
			+ pulse_average * pen_radius_pulse;
		pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY = pen_radius
			+ pulse_average * pen_radius_pulse;
#endif
		//if (pathNo == 0) {
		//	sprintf(pg_AuxString, "/pen_radius %.0f", float(int(pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX))); pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		//}
		// printf("PEN brush ID radius %d %.2f\n" , pen_brush, pen_radius );


		///////////////////////////////////////////////////////////////////////
		// PEN PATH UPDATE AND TANGENT CALCULATION
		///////////////////////////////////////////////////////////////////////
		// AUTOMATIC DRAWING FROM MIDI EVENT
		if (pg_FourFrameStrokeNb > 0 && pathNo == 0) {
			// intermediary point
			if (pg_FourFrameStrokeNb > 1) {
				pg_CurrentCursorPos_x[pathNo] = int(pg_FourFrameStroke_x + pen_radius * 2 * (rand_0_1 - 0.5));
				pg_CurrentCursorPos_y[pathNo] = int(pg_FourFrameStroke_y + pen_radius * 2 * (rand_0_1 - 0.5));
			}
			// last point
			else {
				pg_CurrentCursorPos_x[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
				pg_CurrentCursorPos_y[pathNo] = PG_OUT_OF_SCREEN_CURSOR;
			}
			//printf("MIDI stroke %d %d\n", pg_CurrentCursorPos_x[pathNo], pg_CurrentCursorPos_y[pathNo]);
			pg_FourFrameStrokeNb--;
		}

		pg_Pulsed_CurrentCursorPos_x[pathNo] = pg_CurrentCursorPos_x[pathNo];
		pg_Pulsed_CurrentCursorPos_y[pathNo] = pg_CurrentCursorPos_y[pathNo];
		//printf("current position %d,%d \n", pg_Pulsed_CurrentCursorPos_x[pathNo], pg_Pulsed_CurrentCursorPos_y[pathNo]);

		//if (double_window) {
		//	pg_Pulsed_CurrentCursorPos_x[pathNo] = pg_Pulsed_CurrentCursorPos_x[pathNo] / 2 % pg_workingWindow_width;
		//}

// pen position variation from pg_audio_pulse used in pg_Project_Pierres
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_pen_position_pulse]) {
			if (pathNo == 0) {
				// pen position update from noise
				float random_angle = rand_0_1 * 2.f * float(PI);
				pg_Pulsed_CurrentCursorPos_x[0] += int(pulse_average * 200 * pen_position_pulse * cos(random_angle));
				pg_Pulsed_CurrentCursorPos_y[0] += int(pulse_average * 200 * pen_position_pulse * sin(random_angle));
				//printf("current position after motion %d,%d \n", pg_Pulsed_CurrentCursorPos_x[0], pg_Pulsed_CurrentCursorPos_y[0]);
			}
		}

		// a new drawing step has to be made from a new pen positions: 
		// a the next pen position (different from the preceding next one)
		// The next pen position is the current mouse position.
		// From the previous step:
		// previous previous pen position: pg_paths_currentDynPoint[pathNo].pg_paths_x_prev
		// previous pen position: pg_paths_currentDynPoint[pathNo].pg_paths_x
		// current pen position: pg_paths_currentDynPoint[pathNo].pg_paths_x_next

		// calculates the distance wrt the preceding position and 
		// only updates if it is greater than a minimal value
#if defined(PG_DEBUG)
		OutputDebugStringW(_T("stat 8\n"));
		_CrtMemCheckpoint(&s8);
		if (_CrtMemDifference(&sDiff, &s5, &s8))
			_CrtMemDumpStatistics(&sDiff);
#endif
		// begin
		float distanceFromPrecedingPoint
			= sqrt((pg_Pulsed_CurrentCursorPos_x[pathNo] - pg_paths_currentDynPoint[pathNo].pg_paths_x_next) * (pg_Pulsed_CurrentCursorPos_x[pathNo] - pg_paths_currentDynPoint[pathNo].pg_paths_x_next) +
				(pg_Pulsed_CurrentCursorPos_y[pathNo] - pg_paths_currentDynPoint[pathNo].pg_paths_y_next) * (pg_Pulsed_CurrentCursorPos_y[pathNo] - pg_paths_currentDynPoint[pathNo].pg_paths_y_next));
		//if (pathNo == 0) {
		//	printf("dist from preceding %.2f\n", distanceFromPrecedingPoint);
		//}

		// does not update GPU and considers the position as fixed 
		// if the pen moves of less than 2 pixels or the fifth of the radius
#ifndef var_alKemi
		if (distanceFromPrecedingPoint < std::max(2.f, (pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX / 5.f))
#else
		if (((pg_FullScenarioActiveVars[pg_ind_scenario][_alKemi] == true && distanceFromPrecedingPoint < 2.f)
			|| (pg_FullScenarioActiveVars[pg_ind_scenario][_alKemi] == false
				&& distanceFromPrecedingPoint < std::max(2.f, (pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX / 5.f))))
#endif
			&& pg_Pulsed_CurrentCursorPos_x[pathNo] != PG_OUT_OF_SCREEN_CURSOR
			&& pg_Pulsed_CurrentCursorPos_y[pathNo] != PG_OUT_OF_SCREEN_CURSOR) {
			pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU = PG_IDLE_CURSOR;
			pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU = PG_IDLE_CURSOR;
		}
		// pen position update for the GPU rendering
		else {
			pg_stroke_Bezier_geometry_calculation(pathNo, pg_Pulsed_CurrentCursorPos_x[pathNo], pg_Pulsed_CurrentCursorPos_y[pathNo]);
#if defined(pg_Project_araKnit)
			// uses the pen's motion on the tablet to compensate for the metawear sensors, in case they do not work
			// sends the acceleration of the cursor to the sensor's web model for animation
			if (pathNo == 0) {
				float delta_t_prev = float(pg_paths_currentDynPoint[0].pg_paths_time_prev_prev - pg_paths_currentDynPoint[0].pg_paths_time_prev);
				float delta_t = float(pg_paths_currentDynPoint[0].pg_paths_time_prev - pg_paths_currentDynPoint[0].pg_paths_time);
				float delta_t2 = delta_t_prev + delta_t;
				float linAcc_x = 0.f;
				float linAcc_y = 0.f;
				if (delta_t_prev != 0. && delta_t != 0. && delta_t2 != 0.
					&& pg_paths_currentDynPoint[0].pg_paths_x_prev != PG_OUT_OF_SCREEN_CURSOR && pg_paths_currentDynPoint[0].pg_paths_y_prev != PG_OUT_OF_SCREEN_CURSOR
					&& pg_paths_currentDynPoint[0].pg_paths_x != PG_OUT_OF_SCREEN_CURSOR && pg_paths_currentDynPoint[0].pg_paths_y != PG_OUT_OF_SCREEN_CURSOR) {
					linAcc_x = ((float(pg_paths_currentDynPoint[0].pg_paths_x_prev_prev - pg_paths_currentDynPoint[0].pg_paths_x_prev) / (delta_t_prev * 1000.f)) - (float(pg_paths_currentDynPoint[0].pg_paths_x_prev - pg_paths_currentDynPoint[0].pg_paths_x) / (delta_t * 1000.f))) / (delta_t2 * 1000.f);
					linAcc_y = ((float(pg_paths_currentDynPoint[0].pg_paths_y_prev_prev - pg_paths_currentDynPoint[0].pg_paths_y_prev) / (delta_t_prev * 1000.f)) - (float(pg_paths_currentDynPoint[0].pg_paths_y_prev - pg_paths_currentDynPoint[0].pg_paths_y) / (delta_t * 1000.f))) / (delta_t2 * 1000.f);
				}
				else {
					linAcc_x = 0.f;
					linAcc_y = 0.f;
				}
				//sprintf(pg_AuxString, "/mss_mass_sensor_acc mw1 %.4f %.4f", linAcc_x, linAcc_y);
				if (pg_FrameNo % 3 == 0) {
					sprintf(pg_AuxString, "/penPosX %.4f", fabs(float(pg_Pulsed_CurrentCursorPos_x[0] / 1920.f)));
					pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_Live_send");
				}
				else if (pg_FrameNo % 3 == 1) {
					sprintf(pg_AuxString, "/penPosY %.4f", fabs(float(pg_Pulsed_CurrentCursorPos_y[0] / 2160.f)));
					pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_Live_send");
				}
				else {
					sprintf(pg_AuxString, "/penAcc %.4f", min(1.f, float(sqrt(fabs(linAcc_x * linAcc_x + linAcc_y * linAcc_y)) * 10.f + 0.1)));
					pg_send_message_udp((char*)"f", (char*)pg_AuxString, (char*)"udp_Live_send");
				}
				//printf("/mss_mass_sensor_acc mw1 %.4f %.4f\n", linAcc_x, linAcc_y);
			}
#endif

#if defined(PG_DEBUG)
			OutputDebugStringW(_T("stat 9\n"));
			_CrtMemCheckpoint(&s9);
			if (_CrtMemDifference(&sDiff, &s8, &s9))
				_CrtMemDumpStatistics(&sDiff);
#endif

			// POINT STROKE MANAGEMENT
			if ((pg_paths_currentDynPoint[pathNo].pg_paths_x_prev == PG_OUT_OF_SCREEN_CURSOR || pg_paths_currentDynPoint[pathNo].pg_paths_y_prev == PG_OUT_OF_SCREEN_CURSOR)
				&& (pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU >= 0)) {
				//printf("Current position BEGIN %.1f %.1f\n", pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU, pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU);
				pg_paths_currentDynPoint[pathNo].pg_paths_x_begin_position = pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU;
				pg_paths_currentDynPoint[pathNo].pg_paths_y_begin_position = pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU;
			}
			if ((pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU == PG_OUT_OF_SCREEN_CURSOR || pg_paths_currentDynPoint[pathNo].pg_paths_y == PG_OUT_OF_SCREEN_CURSOR)
				&& (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev >= 0 && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev >= 0)) {
				//printf("Current position END %.1f %.1f\n", pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev);
				if (pg_paths_currentDynPoint[pathNo].pg_paths_x_prev == pg_paths_currentDynPoint[pathNo].pg_paths_x_begin_position && pg_paths_currentDynPoint[pathNo].pg_paths_y_prev == pg_paths_currentDynPoint[pathNo].pg_paths_y_begin_position) {
					//printf("Point at %.1f %.1f\n", pg_paths_currentDynPoint[pathNo].pg_paths_x_prev, pg_paths_currentDynPoint[pathNo].pg_paths_y_prev);
					//pg_paths_currentDynPoint[pathNo].pg_paths_x_forGPU = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_y_forGPU = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_xL = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_yL = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_xR = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_yR = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_x = pg_paths_currentDynPoint[pathNo].pg_paths_x_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_y = pg_paths_currentDynPoint[pathNo].pg_paths_y_prev;
					//pg_paths_currentDynPoint[pathNo].pg_paths_isBegin = true;
					//pg_paths_currentDynPoint[pathNo].pg_paths_isEnd = true;
				}
			}

			// tactile drawing above first track
			if (pathNo > 0) {
				// tells the shader that it should be drawn on the current drawing track
				if (path_replay_trackNo[pathNo] < 0) {
					path_replay_trackNo[pathNo] = currentDrawingTrack;
				}

				//pg_stroke_Bezier_geometry_calculation(pathNo, pg_CurrentCursorPos_x[cursorNo], pg_CurrentCursorPos_y[cursorNo]);
				//printf("new cursor position %d path %d: %d %d replay %d\n", cursorNo, pathNo, pg_CurrentCursorPos_x[cursorNo], pg_CurrentCursorPos_y[cursorNo], int(pg_is_path_replay[pathNo]));

				// management of brush radius (w/wo possible interpolation)
				pg_paths_currentDynPoint[pathNo].pg_paths_RadiusX
					= pen_radius + pulse_average * pen_radius_pulse;
				pg_paths_currentDynPoint[pathNo].pg_paths_RadiusY
					= pen_radius + pulse_average * pen_radius_pulse;
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_r = min(1.f, pg_pulsed_pen_color[0]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_g = min(1.f, pg_pulsed_pen_color[1]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_b = min(1.f, pg_pulsed_pen_color[2]);
				pg_paths_currentDynPoint[pathNo].pg_paths_Color_a = min(1.f, pen_color_a);
			}
		}


		///////////////////////////////////////////////////////////////////////
		// RECORDING PATH
		///////////////////////////////////////////////////////////////////////
		for (int indRecordingPath = 1; indRecordingPath <= PG_NB_PATHS; indRecordingPath++) {
			if (pg_Path_Status[indRecordingPath].path_isActiveRecording) {
				int indFrameRec = pg_Path_Status[indRecordingPath].PathStatus_nbFrames(pg_ind_scenario);
				// printf( "rec track %d frame %d\n",indRecordingPath,indFrameRec );

				// records the initial time and the current time to play in sync
				if (indFrameRec == 0) {
					pg_Path_Status[indRecordingPath].set_path_curve_initialTimeRecording(pg_ind_scenario, theTime);
				}
				pg_Path_Status[indRecordingPath].set_path_curve_finalTimeRecording(pg_ind_scenario, theTime);
				pg_Path_Status[indRecordingPath].setFrameTimeStamp(pg_ind_scenario, indFrameRec, theTime);
				pg_Path_Status[indRecordingPath].setFrameColor(pg_ind_scenario, indFrameRec, min(1.f, pg_pulsed_pen_color[0]), min(1.f, pg_pulsed_pen_color[1]), min(1.f, pg_pulsed_pen_color[2]), min(1.f, pen_color_a));

				//if (indFrameRec < 10) {
				//	printf("capture %.2f %.2f %.2f %.2f\n",
				//		pg_PathCurveFrame_Data[pg_ind_scenario][indRecordingPath][indFrameRec].path_Color_r, pg_PathCurveFrame_Data[pg_ind_scenario][indRecordingPath][indFrameRec].path_Color_g, 
				//		pg_PathCurveFrame_Data[pg_ind_scenario][indRecordingPath][indFrameRec].path_Color_b, pg_PathCurveFrame_Data[pg_ind_scenario][indRecordingPath][indFrameRec].path_Color_a);
				//}

#if defined(PG_WACOM_TABLET)
				pg_Path_Status[indRecordingPath].setFrameBrushRadius(pg_ind_scenario, indFrameRec, pen_brush,
					pen_radius + pulse_average * pen_radius_pulse
					+ pg_CurrentStylusPresuse * pen_radius_pressure_coef
					+ fabs(sin(pg_CurrentStylusAzimut)) * pg_CurrentStylusInclination * pen_radius_angleHor_coef,
					pen_radius + pulse_average * pen_radius_pulse
					+ pg_CurrentStylusPresuse * pen_radius_pressure_coef
					+ fabs(cos(pg_CurrentStylusAzimut)) * pg_CurrentStylusInclination * pen_radius_angleVer_coef);
#else
				pg_Path_Status[indRecordingPath].setFrameBrushRadius(pg_ind_scenario, indFrameRec, pen_brush,
					pen_radius + pulse_average * pen_radius_pulse,
					pen_radius + pulse_average * pen_radius_pulse);
#endif

				// printf( "Track src rec %d Ind %d tan %d %d / prev pos %d %d\n" , indRecordingPath + 1 , indFrameRec - 1 , (int)pg_Path_Pos_xL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yL[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_xR[ indRecordingPath ][ indFrameRec - 1 ] , (int)pg_Path_Pos_yR[ indRecordingPath ][ indFrameRec - 1 ] );

				// first control point
				float loc_Pos_x_prev = pg_paths_currentDynPoint[0].pg_paths_x_prev;
				float loc_Pos_y_prev = pg_paths_currentDynPoint[0].pg_paths_y_prev;

				// second control point
				float loc_path_Pos_xL = pg_paths_currentDynPoint[0].pg_paths_xL;
				float loc_path_Pos_yL = pg_paths_currentDynPoint[0].pg_paths_yL;

				// third control point
				float loc_path_Pos_xR = pg_paths_currentDynPoint[0].pg_paths_xR;
				float loc_path_Pos_yR = pg_paths_currentDynPoint[0].pg_paths_yR;

				// fourth control points (next curve first control point)
				float loc_path_Pos_x;
				float loc_path_Pos_y;
				if (indFrameRec >= 1) {
					loc_path_Pos_x = pg_paths_currentDynPoint[0].pg_paths_x;
					loc_path_Pos_y = pg_paths_currentDynPoint[0].pg_paths_y;
				}
				else {
					// move first step: current point with negative coordinates and null second control points
					loc_path_Pos_x = PG_OUT_OF_SCREEN_CURSOR;
					loc_path_Pos_y = PG_OUT_OF_SCREEN_CURSOR;
				}
				pg_Path_Status[indRecordingPath].setFramePositions(pg_ind_scenario, indFrameRec,
					loc_Pos_x_prev, loc_Pos_y_prev, loc_path_Pos_xL, loc_path_Pos_yL, loc_path_Pos_xR, loc_path_Pos_yR, loc_path_Pos_x, loc_path_Pos_y);

				// moving forward in the recording
				pg_Path_Status[indRecordingPath].pushFrame(pg_ind_scenario);
			}
			// RECORDING SOURCE TRACK
		}
	}
}


//////////////////////////////////////////////////////////////////
// WRITES TRACKS TO A ClipArt FILE
//////////////////////////////////////////////////////////////////

void pg_writesvg(cv::String imageFileName) {
	FILE* fileClipArt;
	//threadData* pDataArray = (threadData*)lpParam;

	// for all paths
	for (int pathNo = 1; pathNo <= PG_NB_PATHS; pathNo++) {
		// only writes non empty paths
		if (pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario) > 0) {

			string fileName = imageFileName;
			string suffix = format("_%02d.svg", pathNo);
			size_t start_pos = fileName.find(".svg");
			if (start_pos != std::string::npos) {
				fileName.replace(start_pos, string(".svg").length(), suffix);
			}
			printf("Snapshot svg (%s)\n", fileName.c_str());

			fileClipArt = fopen(fileName.c_str(), "wb");
			if (fileClipArt == NULL) {
				sprintf(pg_errorStr, "File %s not opened!", fileName.c_str()); pg_ReportError(pg_errorStr); throw 152;
			}

			fprintf(fileClipArt, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n<!-- Created with Inkscape (http://www.inkscape.org/) -->\n\n<svg\n   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n   xmlns:cc=\"http://creativecommons.org/ns#\"\n   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n   xmlns:svg=\"http://www.w3.org/2000/svg\"\n   xmlns=\"http://www.w3.org/2000/svg\"\n   version=\"1.1\"\n   width=\"%d\"\n   height=\"%d\"\n   id=\"svg2\">\n  <defs\n     id=\"defs4\" />\n  <metadata\n     id=\"metadata7\">\n    <rdf:RDF>\n      <cc:Work\n         rdf:about=\"\">\n        <dc:format>image/svg+xml</dc:format>\n        <dc:type\n           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n        <dc:title></dc:title>\n      </cc:Work>\n    </rdf:RDF>\n  </metadata>\n  <g\n     transform=\"translate(0.0,0.0)\"\n     id=\"layer1\">\n", pg_workingWindow_width, PG_WINDOW_HEIGHT);

			fprintf(fileClipArt, "    <path\n       initial_time=\"%f\"\n       final_time=\"%f\"\n",
				pg_Path_Status[pathNo].get_path_curve_initialTimeRecording(pg_ind_scenario),
				pg_Path_Status[pathNo].get_path_curve_finalTimeRecording(pg_ind_scenario));
			std::ostringstream  path_points;
			std::ostringstream  path_colors;
			std::ostringstream  path_brushes;
			std::ostringstream  path_timeStamps;
			int indFrameIni = 0;
			while (pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrameIni) < 0.0
				&& pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrameIni) < 0.0
				&& indFrameIni < pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) {
				indFrameIni++;
			}
			if (indFrameIni < pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario)) {
				path_points << "       d=\"M " << pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrameIni) << ' ';
				path_colors << "       colors=\"" << pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFrameColor_a(pg_ind_scenario, indFrameIni) << ' ';
				path_brushes << "       brushes=\"" << pg_Path_Status[pathNo].getFrameBrush(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFrameRadiusX(pg_ind_scenario, indFrameIni) << ',' << pg_Path_Status[pathNo].getFrameRadiusY(pg_ind_scenario, indFrameIni) << ' ';
				path_timeStamps << "       timeStamps=\"" << pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, indFrameIni) << ' ';
			}
			for (int indFrame = indFrameIni + 1; indFrame < pg_Path_Status[pathNo].PathStatus_nbFrames(pg_ind_scenario); indFrame++) {
				// skips identical frame
				// if( pg_Path_Pos_x[ pathNo ][ indFrame ] 
				//     == pg_Path_Pos_x[ pathNo ][ indFrame - 1 ] 
				//     && pg_Path_Pos_x[ pathNo ][ indFrame ] 
				//     == pg_Path_Pos_x[ pathNo ][ indFrame - 1 ] ) {
				//   continue;
				// }
				// move point with a new curve
				if (pg_Path_Status[pathNo].getFramePositionX_prev(pg_ind_scenario, indFrame) < 0.0
					&& pg_Path_Status[pathNo].getFramePositionY_prev(pg_ind_scenario, indFrame) < 0.0
					&& pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrame) >= 0.0
					&& pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrame) >= 0.0) {
					path_points << "M " << pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrame) << ' ';
					path_colors << pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_a(pg_ind_scenario, indFrame) << ' ';
					path_brushes << pg_Path_Status[pathNo].getFrameBrush(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameRadiusX(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameRadiusY(pg_ind_scenario, indFrame) << ' ';
					path_timeStamps << pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, indFrame) << ' ';
				}
				// curve point
				else if (pg_Path_Status[pathNo].getFramePositionX_prev(pg_ind_scenario, indFrame) >= 0.0
					&& pg_Path_Status[pathNo].getFramePositionY_prev(pg_ind_scenario, indFrame) >= 0.0
					&& pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrame) >= 0.0
					&& pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrame) >= 0.0) {
					path_points << "C " << pg_Path_Status[pathNo].getFramePositionXL(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFramePositionYL(pg_ind_scenario, indFrame) << ' ';
					path_points << pg_Path_Status[pathNo].getFramePositionXR(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFramePositionYR(pg_ind_scenario, indFrame) << ' ';
					path_points << pg_Path_Status[pathNo].getFramePositionX(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFramePositionY(pg_ind_scenario, indFrame) << ' ';
					path_colors << pg_Path_Status[pathNo].getFrameColor_r(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_g(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_b(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameColor_a(pg_ind_scenario, indFrame) << ' ';
					path_brushes << pg_Path_Status[pathNo].getFrameBrush(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameRadiusX(pg_ind_scenario, indFrame) << ',' << pg_Path_Status[pathNo].getFrameRadiusY(pg_ind_scenario, indFrame) << ' ';
					path_timeStamps << pg_Path_Status[pathNo].getFrameTimeStamp(pg_ind_scenario, indFrame) << ' ';
				}
			}
			// writes the strings that have been stored previously
			fprintf(fileClipArt, "%sZ\"\n", path_points.str().c_str());
			fprintf(fileClipArt, "%s\"\n", path_colors.str().c_str());
			fprintf(fileClipArt, "%s\"\n", path_brushes.str().c_str());
			fprintf(fileClipArt, "%s\"\n", path_timeStamps.str().c_str());
			fprintf(fileClipArt, "       id=\"path000%d\"\n       style=\"fill:none;stroke:#000000;stroke-opacity:1;stroke-width:%dpx\" />\n", pathNo, int(pen_radius));

			fprintf(fileClipArt, "  </g>\n</svg>\n");

			fclose(fileClipArt);
		}
	}
}


void pg_listAllSVG_paths(void) {
	printf("Listing SVG paths:\n");
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (SVG_scenarioPathCurve& curve : pg_SVG_scenarioPathCurves[indConfiguration]) {
			std::cout << curve.path_fileName << " (" << curve.path_no << ", "
				<< curve.path_group << "), ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////
// SCENARIO BASED COMMANDS
void pg_NextRecordReplayPath(void) {
	// only one track recording per second to avoid repetition
	if (pg_CurrentClockTime - pg_LastTrackRecordingChangeClockTime > 1) {
		pg_LastTrackRecordingChangeClockTime = pg_CurrentClockTime;
	}
	else {
		return;
	}
	bool new_record = false;
	for (int pathNo = PG_NB_PATHS - 1; pathNo >= 1; pathNo--) {
		if (path_record[pathNo]) {
			path_replay_trackNo_callBack(pathNo, pg_enum_PG_GUI_COMMAND, -1);
			if (pathNo < PG_NB_PATHS - 1) {
				path_record_callBack(pathNo + 1, pg_enum_PG_GUI_COMMAND, true);
			}
			new_record = true;
			break;
		}
	}
	if (!new_record) {
		for (int pathNo = 1; pathNo < PG_NB_PATHS; pathNo++) {
			if (!pg_is_path_replay[pathNo]) {
				path_record_callBack(pathNo, pg_enum_PG_GUI_COMMAND, true);
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////
// TRACK LIB FUNCTIONS
//////////////////////////////////////////////////////////////

void pg_path_recording_onOff(int indPath) {
	// switches between recording on/off 
	//printf("pg_path_recording_onOff(%d)\n", indPath);
	bool is_path_record = false;
	if (indPath >= 1 && indPath < PG_NB_PATHS) {
		is_path_record = !pg_Path_Status[indPath].path_isActiveRecording;
		((bool*)pg_FullScenarioVarPointers[_path_record])[indPath] = is_path_record;

		// starts recording source 
		if (is_path_record) {
			// is reading -> has to stop reading 
			// printf("Launches recording indPath %d: is replay %d\n", indPath, pg_is_path_replay[indPath]);
			// printf("Replay indPath %d: %d\n", indPath, pg_is_path_replay[indPath]);
			if (pg_is_path_replay[indPath]) {
				//printf("Stops Replay indPath %d: %d\n", indPath, pg_is_path_replay[indPath]);
				pg_path_replay_trackNo_stop(indPath);
			}

			// launches source recording 
			pg_path_recording_start(indPath);
			sprintf(pg_AuxString, "/path_record_%d 1", indPath);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");

			// recording can only be made with one finger
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_fingers]) {
				NumberOfInteractionFingers(1);
			}
		}
		else {
			// stops source recording 
			pg_path_recording_stop(indPath);
			sprintf(pg_AuxString, "/path_record_%d 0", indPath);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void pg_path_replay_trackNo_onOff(int indPath, int trackNo) {
	bool ispath_record = false;
	ispath_record = path_record[indPath];

	// is not currently reading and positive or null track -> starts reading 
	if (!pg_is_path_replay[indPath] && trackNo >= 0) {
		//printf( "start_read_path #%d on track %d\n", indPath, trackNo);

		// is recording source -> has to stop recording source 
		if (ispath_record) {
			//printf("pg_path_recording_stop #%d\n", indPath);
			pg_path_recording_stop(indPath);
		}

		// start reading 
		pg_path_replay_trackNo_start(indPath, trackNo);
	}
	// is currently reading && negative track -> stops reading 
	else if (pg_is_path_replay[indPath] && trackNo < 0) {
		//printf("stop_read_path #%d (track No negative)\n", indPath);

		// stops reading 
		pg_path_replay_trackNo_stop(indPath);

		if (indPath >= 1 && indPath < PG_NB_PATHS) {
			((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[indPath] = -1;
		}
	}
}

void pg_path_recording_start(int indPath) {
	if (indPath >= 1 && indPath < PG_NB_PATHS) {
		((bool*)pg_FullScenarioVarPointers[_path_record])[indPath] = true;
		if (indPath >= 1 && indPath <= PG_NB_PATHS) {
			pg_Path_Status[indPath].path_isActiveRecording = true;
			PathCurve_Data& curve = pg_Path_Status[indPath].getCurrentCurve(pg_ind_scenario);
			curve.PathCurve_Data_init();
			// printf("start recording track %d\n",indPath);
		}
	}
}

void pg_path_recording_stop(int indPath) {
	if (indPath >= 1 && indPath < PG_NB_PATHS) {
		((bool*)pg_FullScenarioVarPointers[_path_record])[indPath] = false;
		if (indPath >= 1 && indPath <= PG_NB_PATHS
			&& pg_Path_Status[indPath].path_isActiveRecording == true) {
			pg_Path_Status[indPath].path_isActiveRecording = false;
			sprintf(pg_AuxString, "/path_record_%d 0", indPath);
			pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void pg_path_replay_trackNo_start(int indPath, int trackNo) {
	//printf("replay path %d starts on track %d replayed %d nb frames %d\n", indPath, trackNo, pg_is_path_replay[indPath], 
		//pg_Path_Status[indPath].PathStatus_nbFrames(pg_ind_scenario));
	if (indPath >= 1 && indPath <= PG_NB_PATHS
		&& !pg_is_path_replay[indPath]
		&& pg_Path_Status[indPath].PathStatus_nbFrames(pg_ind_scenario) > 0) {
		pg_is_path_replay[indPath] = true;
		//printf("replay path No %d on track %d with %d frames (replay %d)\n", indPath, trackNo,
			//pg_Path_Status[indPath].PathStatus_nbFrames(pg_ind_scenario), pg_is_path_replay[indPath]);
		if (indPath >= 1 && indPath < PG_NB_PATHS) {
			((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[indPath] = trackNo;
		}
		// first time reading: starts from beginning
		// otherwise starts from where it is
		//if (pg_Path_Status[indPath].path_indReading < 0) {
		// restarts from beginning, not from where it was
		pg_Path_Status[indPath].path_indReading = 0;
		//}
		pg_Path_Status[indPath].path_isFirstFrame = true;

		sprintf(pg_AuxString, "/path_replay_trackNo_%d %d", indPath, trackNo);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
		//printf("-> start_read_path %d ind reading %d\n", indPath, pg_Path_Status[indPath].path_indReading);
	}
}
void pg_path_replay_trackNo_stop(int indPath) {
	if (indPath >= 1 && indPath <= PG_NB_PATHS
		&& pg_is_path_replay[indPath]) {
		pg_is_path_replay[indPath] = false;
		pg_paths_currentDynPoint[indPath].pg_paths_x = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_y = PG_OUT_OF_SCREEN_CURSOR;

		pg_paths_currentDynPoint[indPath].pg_paths_xL = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_yL = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_xR = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_yR = PG_OUT_OF_SCREEN_CURSOR;

		pg_paths_currentDynPoint[indPath].pg_paths_x_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_y_prev = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_currentDynPoint[indPath].pg_paths_isBegin = false;
		pg_paths_currentDynPoint[indPath].pg_paths_isEnd = false;
		pg_paths_currentDynPoint[indPath].pg_paths_Color_r = 0.0F;
		pg_paths_currentDynPoint[indPath].pg_paths_Color_g = 0.0F;
		pg_paths_currentDynPoint[indPath].pg_paths_Color_b = 0.0F;
		pg_paths_currentDynPoint[indPath].pg_paths_Color_a = 1.0F;
		pg_paths_currentDynPoint[indPath].pg_paths_RadiusX = 0.0F;
		pg_paths_currentDynPoint[indPath].pg_paths_RadiusY = 0.0F;
		// printf( "-> start_read_path\n"  );
		((int*)pg_FullScenarioVarPointers[_path_replay_trackNo])[indPath] = -1;
		sprintf(pg_AuxString, "/path_replay_trackNo_%d -1", indPath);
		pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}

///////////////////////////////////////////////////////////////////////////////////
// beat reception and corresponding events triggering
void NumberOfInteractionFingers(int nb_fingers) {
	if (nb_fingers == 1) {
		sprintf(pg_AuxString, "/multixy/5/visible 0"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/4/visible 0"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/3/visible 0"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/2/visible 0"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		//printf("fingers 2-5 invisible\n");
		pg_CurrentCursorPos_x[4] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[4] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_x[3] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[3] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_x[2] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[2] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_x[1] = PG_OUT_OF_SCREEN_CURSOR;
		pg_CurrentCursorPos_y[1] = PG_OUT_OF_SCREEN_CURSOR;
	}
	else if (nb_fingers == 5) {
		sprintf(pg_AuxString, "/multixy/5/visible 1"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/4/visible 1"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/3/visible 1"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		sprintf(pg_AuxString, "/multixy/2/visible 1"); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_MultiTouch_send");
		//printf("fingers 2-5 visible\n");
	}
}

void pg_clear_path_group(void) {
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		pg_Path_Status[ind].path_isFirstFrame = false;
		pg_Path_Status[ind].path_isActiveRecording = false;
		pg_Path_Status[ind].path_indReading = -1;
		pg_Path_Status[ind].path_initialTimeReading = 0.0f;
		pg_Path_Status[ind].path_lastPlayedFrameTime = 1.0F;
		pg_Path_Status[ind].path_indPreviousReading = 0;
	}
	for (int ind = 0; ind <= PG_NB_PATHS; ind++) {
		for (int indCurve = 0; indCurve < pg_Path_Status[ind].nbCurves(pg_ind_scenario); indCurve++) {
			pg_Path_Status[ind].setCurveValues(pg_ind_scenario, 1., 0., 0.);
		}
	}
}
