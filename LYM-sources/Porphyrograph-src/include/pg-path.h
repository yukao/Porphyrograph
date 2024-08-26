/*! \file pg-path.h
 *
 *
 *     File pg-path.h
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

#define PG_OUT_OF_SCREEN_CURSOR -1000

 // stroke shipping to GPU (Update and PartcleAnimation shaders)
#define PG_PATH_P_X              0
#define PG_PATH_P_Y              1

#define PG_PATH_BOX              2
#define PG_PATH_COLOR            3
#define PG_PATH_RADIUS_BEGINEND  4
#define PG_MAX_PATH_DATA         5

#define PG_PATH_ANIM_POS              0
#define PG_PATH_ANIM_RAD              1
#define PG_MAX_PATH_ANIM_DATA         2

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SVG PATH MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// a path curve from scenario with its parameters
class SVG_scenarioPathCurve {
public:
	int path_no;    // the path number associated with the curve between 1 and PG_NB_PATHS
	int rankInPath; // the rank of the curve in the path number
	int path_group; // a group of curves that can be activated or deactivated simultaneously
	int path_track; // the track on which the curve will be drawn
	float pathRadius;
	float path_r_color;
	float path_g_color;
	float path_b_color;
	float path_readSpeedScale;
	string path_ID;
	string path_fileName;
	bool with_color_radius_from_scenario;
	double secondsforwidth;
	SVG_scenarioPathCurve(void) {
		path_no = 1;
		rankInPath = 0;
		path_group = 1;
		path_track = 1;
		pathRadius = 1;
		path_r_color = 1;
		path_g_color = 1;
		path_b_color = 1;
		path_readSpeedScale = 1;
		path_ID = "";
		path_fileName = "";
		with_color_radius_from_scenario = false;
		secondsforwidth = 10.;
	}
	void SVG_scenarioPathCurve_init(int p_indPath, int p_rankInPath, int p_indTrack, float p_pathRadius, float p_path_r_color, float p_path_g_color,
		float p_path_b_color, float p_path_readSpeedScale, string p_path_ID, string p_path_fileName, int p_path_group,
		bool p_with_color__brush_radius_from_scenario, double p_secondsforwidth) {
		path_no = p_indPath;
		rankInPath = p_rankInPath;
		path_track = p_indTrack;
		pathRadius = p_pathRadius;
		path_r_color = p_path_r_color;
		path_g_color = p_path_g_color;
		path_b_color = p_path_b_color;
		path_readSpeedScale = p_path_readSpeedScale;
		path_ID = p_path_ID;
		path_fileName = p_path_fileName;
		// no path group provided in the scenario file
		if (p_path_group <= 0) {
			path_group = 1;
		}
		// path group number is provided inside the scenario
		else {
			path_group = p_path_group;
		}
		with_color_radius_from_scenario = p_with_color__brush_radius_from_scenario;
		secondsforwidth = p_secondsforwidth;
	}
	~SVG_scenarioPathCurve() {
		path_ID = "";
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PATH FRAME (SINGLE POINT) MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

class PathCurveFrame {
public:
	float path_Color_r;
	float path_Color_g;
	float path_Color_b;
	float path_Color_a;
	int path_BrushID;
	float path_RadiusX;
	float path_RadiusY;
	float Pos_x_prev;
	float Pos_y_prev;
	float path_Pos_xL;
	float path_Pos_yL;
	float path_Pos_x;
	float path_Pos_y;
	float path_Pos_xR;
	float path_Pos_yR;
	double TimeStamp;
	PathCurveFrame(void) {
		PathCurveFrame_init();
	}
	void PathCurveFrame_init() {
		path_Color_r = 1.f;
		path_Color_g = 1.f;
		path_Color_b = 1.f;
		path_Color_a = 1.f;
		path_BrushID = 0;
		path_RadiusX = 1.f;
		path_RadiusY = 1.f;
		Pos_x_prev = 0.f;
		Pos_y_prev = 0.f;
		path_Pos_xL = 0.f;
		path_Pos_yL = 0.f;
		path_Pos_xR = 0.f;
		path_Pos_yR = 0.f;
		path_Pos_x = 0.f;
		path_Pos_y = 0.f;
		TimeStamp = 0.;
	}
	void setFrameValues(float path_r_color, float path_g_color, float path_b_color, float path_a_color,
		int brushID, float path_radiusX, float path_radiusY) {
		path_Color_r = path_r_color;
		path_Color_g = path_g_color;
		path_Color_b = path_b_color;
		path_Color_a = path_a_color;
		path_BrushID = brushID;
		path_RadiusX = path_radiusX;
		path_RadiusY = path_radiusY;
	}
	void copyFrameValues(PathCurveFrame* frameSource) {
		path_Color_r = frameSource->path_Color_r;
		path_Color_g = frameSource->path_Color_g;
		path_Color_b = frameSource->path_Color_b;
		path_Color_a = frameSource->path_Color_a;
		path_BrushID = frameSource->path_BrushID;
		path_RadiusX = frameSource->path_RadiusX;
		path_RadiusY = frameSource->path_RadiusY;
	}
	void copyFrameTimeStamp(PathCurveFrame* frameSource) {
		TimeStamp = frameSource->TimeStamp;
	}
	void setFrameBrushRadius(int brushID, float path_radiusX, float path_radiusY) {
		path_BrushID = brushID;
		path_RadiusX = path_radiusX;
		path_RadiusY = path_radiusY;
	}
	void setFrameColor(float path_r_color, float path_g_color, float path_b_color, float path_a_color) {
		path_Color_r = path_r_color;
		path_Color_g = path_g_color;
		path_Color_b = path_b_color;
		path_Color_a = path_a_color;
	}
	void setFramePositionsTimeStamp(float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y, double new_TimeStamp) {
		Pos_x_prev = new_Pos_x_prev;
		Pos_y_prev = new_Pos_y_prev;
		path_Pos_xL = new_path_Pos_xL;
		path_Pos_yL = new_path_Pos_yL;
		path_Pos_xR = new_path_Pos_xR;
		path_Pos_yR = new_path_Pos_yR;
		path_Pos_x = new_path_Pos_x;
		path_Pos_y = new_path_Pos_y;
		TimeStamp = new_TimeStamp;
	}
	void setFramePositions(float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y) {
		Pos_x_prev = new_Pos_x_prev;
		Pos_y_prev = new_Pos_y_prev;
		path_Pos_xL = new_path_Pos_xL;
		path_Pos_yL = new_path_Pos_yL;
		path_Pos_xR = new_path_Pos_xR;
		path_Pos_yR = new_path_Pos_yR;
		path_Pos_x = new_path_Pos_x;
		path_Pos_y = new_path_Pos_y;
	}
	void setFramePositionsCurrentPrevious(float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_x, float new_path_Pos_y) {
		Pos_x_prev = new_Pos_x_prev;
		Pos_y_prev = new_Pos_y_prev;
		path_Pos_x = new_path_Pos_x;
		path_Pos_y = new_path_Pos_y;
	}
	void setFramePositionsLeftRightCopy() {
		path_Pos_xL = Pos_x_prev;
		path_Pos_yL = Pos_y_prev;
		path_Pos_xR = path_Pos_x;
		path_Pos_yR = path_Pos_y;
	}
	void setFrameTimeStamp(double new_TimeStamp) {
		TimeStamp = new_TimeStamp;
	}
	~PathCurveFrame() {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FULL PATH CURVE MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

class PathCurve_Data {
public:
	// int pathCurve_nbRecordedFrames;
	double pathCurve_readSpeedScale; // speed from the scenario file
	double pathCurve_initialTimeRecording;
	double pathCurve_finalTimeRecording;
	vector<PathCurveFrame> path_CurveFrames;
	PathCurve_Data(void) {
		PathCurve_Data_init();
	}
	void PathCurve_Data_init() {
		emptyFrame();

		// pathCurve_nbRecordedFrames = 0;
		pathCurve_readSpeedScale = 1.0F;
		pathCurve_initialTimeRecording = 0.0f;
		pathCurve_finalTimeRecording = 0.0f;
	}
	int PathCurve_nbFrames() {
		return path_CurveFrames.size() - 1;
	}
	void pushFrame() {
		PathCurveFrame frame;
		frame.PathCurveFrame_init();
		path_CurveFrames.push_back(frame);
	}
	void emptyFrame() {
		PathCurveFrame frame;
		frame.PathCurveFrame_init();
		vector<PathCurveFrame> oneframe;
		oneframe.push_back(frame);
		path_CurveFrames = oneframe;
	}
	void setCurveValues(double readSpeedScale, double initialTimeRecording, double finalTimeRecording) {
		pathCurve_readSpeedScale = readSpeedScale;
		pathCurve_initialTimeRecording = initialTimeRecording;
		pathCurve_finalTimeRecording = finalTimeRecording;
	}
	~PathCurve_Data() {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PATH PLAYONG OR RECORDING DYNATMIC STATUS MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

class Path_Status {
public:
	bool path_isFirstFrame;
	bool path_isActiveRecording;
	int path_indReading;
	double path_initialTimeReading;
	// time at which was displayed the last frame
	double path_lastPlayedFrameTime;
	// index of the last displayed frame
	int path_lastPlayedindReading;
	int path_indPreviousReading;
	int pathNo;

	// curves associated with each path
	int path_currentPathCurve[PG_MAX_SCENARIOS];
	// the curves and their data: each path can have several curves
	vector<PathCurve_Data> path_PathCurve_Data[PG_MAX_SCENARIOS];
	// time stamps temporary storage afer reading them from the time stamp list and before inserting them into frames
	vector<double> path_TmpTimeStamps;

	Path_Status(void) {
		path_isFirstFrame = false;
		path_isActiveRecording = false;
		path_indReading = -1;
		path_initialTimeReading = 0.0f;
		path_lastPlayedFrameTime = 1.0F;
		path_lastPlayedindReading = 0;
		path_indPreviousReading = 0;
		pathNo = 0;
		for (int indScenario = 0; indScenario < PG_MAX_SCENARIOS; indScenario++) {
			// path_nb_pathCurves[indScenario] = 0;
			path_currentPathCurve[indScenario] = 0;
		}
	}
	void Path_Status_init(int indPath) {
		pathNo = indPath;
	}
	void readsvg(char* fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
		float readSpeedScale, string path_ID_in_scenario, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indScenario);
	void load_svg_path(char* fileName,
		float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale,
		string path_ID, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indScenario);
	void LoadPathColorsFromXML(string pathString, int* nbRecordedFrames, int indScenario);
	void LoadPathBrushesFromXML(string pathString, int* nbRecordedFrames, int indScenario);
	void LoadPathTimeStampsFromXML(string pathString, int* nbRecordedFrames);
	int pg_LoadPathPointsFromXML(char* pathString,
		glm::mat4* p_M_transf, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
		float precedingCurrentPoint[2], float  currentPoint[2],
		bool withRecordingOfStrokeParameters, bool with_color__brush_radius_from_scenario,
		float* path_length, double p_secondsforwidth, int* p_nbRecordedTimeStamps, int indScenario);
	PathCurveFrame& getFrame(int indConfiguraton, int indCurve, int indFrame) {
		PathCurve_Data& curve = getCurve(indConfiguraton, indCurve);
		return curve.path_CurveFrames[indFrame];
	}
	void pushFrame(int indScenario) {
		PathCurve_Data& curve = getCurrentCurve(indScenario);
		curve.pushFrame();
	}
	void path_TmpTimeStamps_init() {
		path_TmpTimeStamps.clear();
	}
	void pushTmpTimeStamps(double ts) {
		path_TmpTimeStamps.push_back(ts);
	}
	double getTmpTimeStamp(unsigned int indTimeStamp) {
		if (indTimeStamp < path_TmpTimeStamps.size()) {
			return path_TmpTimeStamps[indTimeStamp];
		}
		return 0.;
	}
	void emptyFrame(int indScenario) {
		PathCurve_Data& curve = getCurrentCurve(indScenario);
		curve.emptyFrame();
	}
	PathCurve_Data& getCurve(int indConfiguraton, int indCurve) {
		if (indCurve >= int(path_PathCurve_Data[indConfiguraton].size())) {
			sprintf(pg_errorStr, "Error: Access to non-existing curve %d for path %d max curve %d",
				indCurve, pathNo, (int)(path_PathCurve_Data[indConfiguraton].size() - 1)); pg_ReportError(pg_errorStr); throw 561;
		}
		return path_PathCurve_Data[indConfiguraton][indCurve];
	}
	PathCurve_Data& getCurrentCurve(int indScenario) {
		return path_PathCurve_Data[indScenario][path_currentPathCurve[indScenario]];
	}
	int nbCurves(int indConfiguraton) {
		return path_PathCurve_Data[indConfiguraton].size();
	}
	int PathStatus_nbFrames(int indScenario) {
		if (nbCurves(indScenario) <= 0) {
			return -1;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			return curve.PathCurve_nbFrames();
		}
		else {
			return -1;
		}
	}
	void setFrameValues(int indScenario, int indFrame, float path_r_color, float path_g_color, float path_b_color, float path_a_color,
		int brushID, float path_radiusX, float path_radiusY) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFrameValues(path_r_color, path_g_color, path_b_color, path_a_color,
				brushID, path_radiusX, path_radiusY);
		}
	}
	void copyFrameValues(int indScenario, int indFrameSource, int indFrameTarget) {
		if (indFrameSource < PathStatus_nbFrames(indScenario) && indFrameTarget < PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frameTarget = getFrame(indScenario, path_currentPathCurve[indScenario], indFrameTarget);
			PathCurveFrame& frameSource = getFrame(indScenario, path_currentPathCurve[indScenario], indFrameSource);
			frameTarget.copyFrameValues(&frameSource);
		}
	}
	void copyFrameTimeStamp(int indScenario, int indFrameSource, int indFrameTarget) {
		if (indFrameSource < PathStatus_nbFrames(indScenario) && indFrameTarget < PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frameTarget = getFrame(indScenario, path_currentPathCurve[indScenario], indFrameTarget);
			PathCurveFrame& frameSource = getFrame(indScenario, path_currentPathCurve[indScenario], indFrameSource);
			frameTarget.copyFrameTimeStamp(&frameSource);
		}
	}
	void setFrameBrushRadius(int indScenario, int indFrame,
		int brushID, float path_radiusX, float path_radiusY) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFrameBrushRadius(brushID, path_radiusX, path_radiusY);
		}
	}
	void setFrameColor(int indScenario, int indFrame, float path_r_color, float path_g_color, float path_b_color, float path_a_color) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFrameColor(path_r_color, path_g_color, path_b_color, path_a_color);
		}
	}
	void setFramePositionsTimeStamp(int indScenario, int indFrame, float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y, double new_TimeStamp) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFramePositionsTimeStamp(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_xL, new_path_Pos_yL,
				new_path_Pos_xR, new_path_Pos_yR, new_path_Pos_x, new_path_Pos_y, new_TimeStamp);
		}
	}
	void setFramePositions(int indScenario, int indFrame, float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFramePositions(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_xL, new_path_Pos_yL,
				new_path_Pos_xR, new_path_Pos_yR, new_path_Pos_x, new_path_Pos_y);
		}
	}
	void setFramePositionsLeftRightCopy(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFramePositionsLeftRightCopy();
		}
	}
	float getFramePositionX(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_x;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionY(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_y;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionXL(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_xL;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionYL(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_yL;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionXR(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_xR;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionYR(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Pos_yR;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionX_prev(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.Pos_x_prev;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionY_prev(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.Pos_y_prev;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFrameColor_r(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Color_r;
		}
		return float(0.f);
	}
	float getFrameColor_g(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Color_g;
		}
		return float(0.f);
	}
	float getFrameColor_b(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Color_b;
		}
		return float(0.f);
	}
	float getFrameColor_a(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_Color_a;
		}
		return float(0.f);
	}
	int getFrameBrush(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_BrushID;
		}
		return 0;
	}
	float getFrameRadiusX(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_RadiusX;
		}
		return float(0.f);
	}
	float getFrameRadiusY(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.path_RadiusY;
		}
		return float(0.f);
	}
	void setFramePositionsCurrentPrevious(int indScenario, int indFrame, float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_x, float new_path_Pos_y) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFramePositionsCurrentPrevious(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_x, new_path_Pos_y);
		}
	}
	void setFrameTimeStamp(int indScenario, int indFrame, double new_TimeStamp) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			frame.setFrameTimeStamp(new_TimeStamp);
		}
	}
	double getFrameTimeStamp(int indScenario, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indScenario)) {
			PathCurveFrame& frame = getFrame(indScenario, path_currentPathCurve[indScenario], indFrame);
			return frame.TimeStamp;
		}
		return 0.;
	}
	void setCurveValues(int indScenario, double readSpeedScale, double initialTimeRecording, double finalTimeRecording) {
		if (nbCurves(indScenario) <= 0) {
			return;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			curve.pathCurve_readSpeedScale = readSpeedScale;
			curve.pathCurve_initialTimeRecording = initialTimeRecording;
			curve.pathCurve_finalTimeRecording = finalTimeRecording;
		}
		else {
			return;
		}
	}
	double set_path_curve_readSpeedScale(int indScenario, double readSpeedScale) {
		if (nbCurves(indScenario) <= 0) {
			return -1.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			curve.pathCurve_readSpeedScale = readSpeedScale;
			return readSpeedScale;
		}
		else {
			return -1;
		}
	}
	double get_path_curve_readSpeedScale(int indScenario) {
		if (nbCurves(indScenario) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			return curve.pathCurve_readSpeedScale;
		}
		else {
			return  0.;
		}
	}
	double get_path_curve_initialTimeRecording(int indScenario) {
		if (nbCurves(indScenario) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			return curve.pathCurve_initialTimeRecording;
		}
		else {
			return  0.;
		}
	}
	double set_path_curve_initialTimeRecording(int indScenario, double initialTime) {
		if (nbCurves(indScenario) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			curve.pathCurve_initialTimeRecording = initialTime;
			return initialTime;
		}
		else {
			return  0.;
		}
	}
	double get_path_curve_finalTimeRecording(int indScenario) {
		if (nbCurves(indScenario) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			return curve.pathCurve_finalTimeRecording;
		}
		else {
			return  0.;
		}
	}
	double set_path_curve_finalTimeRecording(int indScenario, double finalTime) {
		if (nbCurves(indScenario) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indScenario] >= 0
			&& path_currentPathCurve[indScenario] < nbCurves(indScenario)) {
			PathCurve_Data& curve = getCurve(indScenario, path_currentPathCurve[indScenario]);
			curve.pathCurve_finalTimeRecording = finalTime;
			return finalTime;
		}
		else {
			return  0.;
		}
	}
	~Path_Status() {
	}
};

struct vec2
{
	float x;
	float y;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PATH CURRENTLY DRAWN POINT MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// current mouse location (also used for displaying the cursor)
class pg_paths_currentDynPoint_data {
public:
	float pg_paths_x;
	float pg_paths_y;
	float pg_paths_x_next;
	float pg_paths_y_next;
	float pg_paths_x_prev_prev;
	float pg_paths_y_prev_prev;
	float pg_paths_x_prev;
	float pg_paths_y_prev;

	float pg_paths_x_memory;
	float pg_paths_y_memory;
	float pg_paths_x_prev_memory;
	float pg_paths_y_prev_memory;

	float pg_paths_x_forGPU;
	float pg_paths_y_forGPU;

	float pg_paths_time;
	float pg_paths_time_prev;
	float pg_paths_time_prev_prev;

	bool pg_paths_isBegin;
	bool pg_paths_isEnd;

	float pg_paths_Color_r;
	float pg_paths_Color_g;
	float pg_paths_Color_b;
	float pg_paths_Color_a;
	float pg_paths_RadiusX;
	float pg_paths_RadiusY;

	float pg_paths_xL;
	float pg_paths_yL;
	float pg_paths_xR;
	float pg_paths_yR;
	float pg_paths_tang_x;
	float pg_paths_tang_y;
	float pg_paths_tang_x_prev;
	float pg_paths_tang_y_prev;

	int pg_paths_BrushID;

	float pg_paths_x_begin_position;
	float pg_paths_y_begin_position;

	pg_paths_currentDynPoint_data(void) {
		pg_paths_x_forGPU = PG_OUT_OF_SCREEN_CURSOR;
		pg_paths_y_forGPU = PG_OUT_OF_SCREEN_CURSOR;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// PATH CURVE AND GROUPS
extern vector<SVG_scenarioPathCurve> pg_SVG_scenarioPathCurves[PG_MAX_SCENARIOS];
extern int pg_current_SVG_path_group;
extern int pg_nb_SVG_path_groups[PG_MAX_SCENARIOS];

/// current mouse location
extern int pg_CurrentCursorPos_x[PG_NB_CURSORS_MAX], pg_CurrentCursorPos_y[PG_NB_CURSORS_MAX];
extern pg_paths_currentDynPoint_data pg_paths_currentDynPoint[PG_NB_PATHS + 1];
/// current mouse type
extern int pg_CurrentStylus_StylusvsRubber;
// current tablet pen pressure and orientation
extern float pg_CurrentStylusPresuse;
extern float pg_CurrentStylusAzimut;
extern float pg_CurrentStylusInclination;
/// current hoover location
extern int pg_CurrentStylusHooverPos_x, pg_CurrentStylusHooverPos_y;

// path data for particle animation (repulse/follow)
extern float pg_path_data_ParticleAnimation[(PG_NB_PATHS + 1) * PG_MAX_PATH_ANIM_DATA * 4];
// communication with shader
extern float pg_path_data_Update[(PG_NB_PATHS + 1) * PG_MAX_PATH_DATA * 4];

// builds the box around a curve for the shader
extern glm::vec4 pg_BezierBox[(PG_NB_PATHS + 1)];

// keeps track of replayed paths
extern bool pg_is_path_replay[PG_NB_PATHS + 1];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// OSC commands
void pg_aliasScript_Path(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);
void pg_parseScenario_SVGPaths(std::ifstream& scenarioFin, int indScenario);

// initialization of the tables that contain the stroke parameters
void pg_initPaths(void);
// list paths after compilling
void pg_listAll_Paths(void);

// command chaining record/replay on paths
void pg_NextRecordReplayPath(void);
// on/oof path recording
void pg_path_recording_onOff(int indPath);
// playing track onoff
void pg_path_replay_trackNo_onOff(int indPath, int trackNo);
// recording on off
void pg_path_recording_stop(int indPath);

// update of the tables that contain the stroke parameters
void pg_update_pulsed_colors_and_replay_paths(double theTime);

// builds the box around a curve for the shader
void pg_build_Bezier_bounding_box(int indPath);

/// SAVE PATH
void pg_writesvg(cv::String imageFileName);

// interface feedback depending on how many fingers are considered on the pad
void NumberOfInteractionFingers(int nb_fingers);

#endif
