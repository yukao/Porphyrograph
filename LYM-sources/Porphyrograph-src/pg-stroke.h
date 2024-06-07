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

#define PG_OUT_OF_SCREEN_CURSOR -1000

 // ClipArt PATHs from scenario
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
extern vector<SVG_scenarioPathCurve*> SVG_scenarioPathCurves[PG_MAX_CONFIGURATIONS];
extern int pg_current_SVG_path_group;
extern int pg_nb_SVG_path_groups[PG_MAX_CONFIGURATIONS];

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
	int path_currentPathCurve[PG_MAX_CONFIGURATIONS];
	// the curves and their data: each path can have several curves
	vector<PathCurve_Data> path_PathCurve_Data[PG_MAX_CONFIGURATIONS];
	// time stamps temporary storage afer reading them from the time stamp list and before inserting them into frames
	vector<double> path_TmpTimeStamps;

	Path_Status(void) {
		Path_Status_init(0);
	}
	void Path_Status_init(int indPath) {
		path_isFirstFrame = false;
		path_isActiveRecording = false;
		path_indReading = -1;
		path_initialTimeReading = 0.0f;
		path_lastPlayedFrameTime = 1.0F;
		path_lastPlayedindReading = 0;
		path_indPreviousReading = 0;
		pathNo = indPath;

		for (int indConfiguration = 0; indConfiguration < PG_MAX_CONFIGURATIONS; indConfiguration++) {
			// path_nb_pathCurves[indConfiguration] = 0;
			path_currentPathCurve[indConfiguration] = 0;
		}

	}
	void readsvg(char* fileName, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
		float readSpeedScale, string path_ID_in_scenario, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indConfiguration);
	void load_svg_path(char* fileName,
		float pathRadius, float path_r_color, float path_g_color, float path_b_color, float readSpeedScale,
		string path_ID, bool p_with_color__brush_radius_from_scenario, double secondsforwidth, int indConfiguration); 
	void LoadPathColorsFromXML(string pathString, int* nbRecordedFrames, int indConfiguration);
	void LoadPathBrushesFromXML(string pathString, int* nbRecordedFrames, int indConfiguration);
	void LoadPathTimeStampsFromXML(string pathString, int* nbRecordedFrames);
	int LoadPathPointsFromXML(char* pathString,
		glm::mat4* p_M_transf, float pathRadius, float path_r_color, float path_g_color, float path_b_color,
		float precedingCurrentPoint[2], float  currentPoint[2],
		bool withRecordingOfStrokeParameters, bool with_color__brush_radius_from_scenario,
		float* path_length, double p_secondsforwidth, int* p_nbRecordedTimeStamps, int indConfiguration);
	PathCurveFrame& getFrame(int indConfiguraton, int indCurve, int indFrame) {
		PathCurve_Data& curve = getCurve(indConfiguraton, indCurve);
		return curve.path_CurveFrames[indFrame];
	}
	void pushFrame(int indConfiguration) {
		PathCurve_Data& curve = getCurrentCurve(indConfiguration);
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
	void emptyFrame(int indConfiguration) {
		PathCurve_Data& curve = getCurrentCurve(indConfiguration);
		curve.emptyFrame();
	}
	PathCurve_Data& getCurve(int indConfiguraton, int indCurve) {
		if (indCurve >= int(path_PathCurve_Data[indConfiguraton].size())) {
			sprintf(ErrorStr, "Error: Access to non-existing curve %d for path %d max curve %d",
				indCurve, pathNo, path_PathCurve_Data[indConfiguraton].size() - 1); ReportError(ErrorStr); throw 561;
		}
		return path_PathCurve_Data[indConfiguraton][indCurve];
	}
	PathCurve_Data& getCurrentCurve(int indConfiguration) {
		return path_PathCurve_Data[indConfiguration][path_currentPathCurve[indConfiguration]];
	}
	int nbCurves(int indConfiguraton) {
		return path_PathCurve_Data[indConfiguraton].size();
	}
	int PathStatus_nbFrames(int indConfiguration) {
		if (nbCurves(indConfiguration) <= 0) {
			return -1;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			return curve.PathCurve_nbFrames();
		}
		else {
			return -1;
		}
	}
	void setFrameValues(int indConfiguration, int indFrame, float path_r_color, float path_g_color, float path_b_color, float path_a_color,
		int brushID, float path_radiusX, float path_radiusY) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFrameValues(path_r_color, path_g_color, path_b_color, path_a_color,
				brushID, path_radiusX, path_radiusY);
		}
	}
	void copyFrameValues(int indConfiguration, int indFrameSource, int indFrameTarget) {
		if (indFrameSource < PathStatus_nbFrames(indConfiguration) && indFrameTarget < PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frameTarget = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrameTarget);
			PathCurveFrame& frameSource = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrameSource);
			frameTarget.copyFrameValues(&frameSource);
		}
	}
	void copyFrameTimeStamp(int indConfiguration, int indFrameSource, int indFrameTarget) {
		if (indFrameSource < PathStatus_nbFrames(indConfiguration) && indFrameTarget < PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frameTarget = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrameTarget);
			PathCurveFrame& frameSource = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrameSource);
			frameTarget.copyFrameTimeStamp(&frameSource);
		}
	}
	void setFrameBrushRadius(int indConfiguration, int indFrame,
		int brushID, float path_radiusX, float path_radiusY) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFrameBrushRadius(brushID, path_radiusX, path_radiusY);
		}
	}
	void setFrameColor(int indConfiguration, int indFrame, float path_r_color, float path_g_color, float path_b_color, float path_a_color) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFrameColor(path_r_color, path_g_color, path_b_color, path_a_color);
		}
	}
	void setFramePositionsTimeStamp(int indConfiguration, int indFrame, float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y, double new_TimeStamp) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFramePositionsTimeStamp(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_xL, new_path_Pos_yL, 
				new_path_Pos_xR, new_path_Pos_yR, new_path_Pos_x, new_path_Pos_y, new_TimeStamp);
		}
	}
	void setFramePositions(int indConfiguration, int indFrame, float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_xL, float new_path_Pos_yL,
		float new_path_Pos_xR, float new_path_Pos_yR, float new_path_Pos_x, float new_path_Pos_y) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFramePositions(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_xL, new_path_Pos_yL, 
				new_path_Pos_xR, new_path_Pos_yR, new_path_Pos_x, new_path_Pos_y);
		}
	}
	void setFramePositionsLeftRightCopy(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFramePositionsLeftRightCopy();
		}
	}
	float getFramePositionX(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_x;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionY(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_y;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionXL(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_xL;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionYL(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_yL;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionXR(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_xR;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionYR(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Pos_yR;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionX_prev(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.Pos_x_prev;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFramePositionY_prev(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.Pos_y_prev;
		}
		return float(PG_OUT_OF_SCREEN_CURSOR);
	}
	float getFrameColor_r(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Color_r;
		}
		return float(0.f);
	}
	float getFrameColor_g(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Color_g;
		}
		return float(0.f);
	}
	float getFrameColor_b(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Color_b;
		}
		return float(0.f);
	}
	float getFrameColor_a(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_Color_a;
		}
		return float(0.f);
	}
	int getFrameBrush(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_BrushID;
		}
		return 0;
	}
	float getFrameRadiusX(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_RadiusX;
		}
		return float(0.f);
	}
	float getFrameRadiusY(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.path_RadiusY;
		}
		return float(0.f);
	}
	void setFramePositionsCurrentPrevious(int indConfiguration, int indFrame,  float new_Pos_x_prev, float new_Pos_y_prev, float new_path_Pos_x, float new_path_Pos_y) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFramePositionsCurrentPrevious(new_Pos_x_prev, new_Pos_y_prev, new_path_Pos_x, new_path_Pos_y);
		}
	}
	void setFrameTimeStamp(int indConfiguration, int indFrame, double new_TimeStamp) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			frame.setFrameTimeStamp(new_TimeStamp);
		}
	}
	double getFrameTimeStamp(int indConfiguration, int indFrame) {
		if (indFrame <= PathStatus_nbFrames(indConfiguration)) {
			PathCurveFrame& frame = getFrame(indConfiguration, path_currentPathCurve[indConfiguration], indFrame);
			return frame.TimeStamp;
		}
		return 0.;
	}
	void setCurveValues(int indConfiguration, double readSpeedScale, double initialTimeRecording, double finalTimeRecording) {
		if (nbCurves(indConfiguration) <= 0) {
			return;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			curve.pathCurve_readSpeedScale = readSpeedScale;
			curve.pathCurve_initialTimeRecording = initialTimeRecording;
			curve.pathCurve_finalTimeRecording = finalTimeRecording;
		}
		else {
			return;
		}
	}
	//int get_path_curve_nbRecordedFrames(int indConfiguration) {
	//	if (nbCurves(indConfiguration) <= 0) {
	//		return -1;
	//	}
	//	else if (path_currentPathCurve[indConfiguration] >= 0
	//		&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
	//		PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
	//		return curve.pathCurve_nbRecordedFrames;
	//	}
	//	else {
	//		return -1;
	//	}
	//}
	//int set_path_curve_nbRecordedFrames(int indConfiguration, int length) {
	//	if (nbCurves(indConfiguration) <= 0) {
	//		return -1;
	//	}
	//	else if (path_currentPathCurve[indConfiguration] >= 0
	//		&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)
	//		&& length < max_mouse_recording_frames (obsolete with vectors)) {
	//		PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
	//		curve.pathCurve_nbRecordedFrames = length;
	//		return length;
	//	}
	//	else {
	//		return -1;
	//	}
	//}
	double set_path_curve_readSpeedScale(int indConfiguration, double readSpeedScale) {
		if (nbCurves(indConfiguration) <= 0) {
			return -1.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			curve.pathCurve_readSpeedScale = readSpeedScale;
			return readSpeedScale;
		}
		else {
			return -1;
		}
	}
	double get_path_curve_readSpeedScale(int indConfiguration) {
		if (nbCurves(indConfiguration) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			return curve.pathCurve_readSpeedScale;
		}
		else {
			return  0.;
		}
	}
	double get_path_curve_initialTimeRecording(int indConfiguration) {
		if (nbCurves(indConfiguration) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			return curve.pathCurve_initialTimeRecording;
		}
		else {
			return  0.;
		}
	}
	double set_path_curve_initialTimeRecording(int indConfiguration, double initialTime) {
		if (nbCurves(indConfiguration) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			curve.pathCurve_initialTimeRecording = initialTime;
			return initialTime;
		}
		else {
			return  0.;
		}
	}
	double get_path_curve_finalTimeRecording(int indConfiguration) {
		if (nbCurves(indConfiguration) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
			return curve.pathCurve_finalTimeRecording;
		}
		else {
			return  0.;
		}
	}
	double set_path_curve_finalTimeRecording(int indConfiguration, double finalTime) {
		if (nbCurves(indConfiguration) <= 0) {
			return 0.;
		}
		else if (path_currentPathCurve[indConfiguration] >= 0
			&& path_currentPathCurve[indConfiguration] < nbCurves(indConfiguration)) {
			PathCurve_Data& curve = getCurve(indConfiguration, path_currentPathCurve[indConfiguration]);
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
// extern struct PathCurve_Data* pg_PathCurve_Params[PG_MAX_CONFIGURATIONS];
// extern struct PathCurveFrame** pg_PathCurveFrame_Data[PG_MAX_CONFIGURATIONS];
extern Path_Status* pg_Path_Status;

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
void pg_initPaths(void);

// char array to integer or float
long     pg_ScanIntegerString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind);
float    pg_ScanFloatString(int* p_c,
	int withTrailingSpaceChars,
	char* charstring, int* ind);

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

#if defined(var_path_replay_trackNo_1)
// REPLAY PATHS  
void pg_replay_paths(double theTime);
#endif

// update of the tables that contain the stroke parameters
void pg_update_pulsed_colors_and_replay_paths( double theTime );


#endif
