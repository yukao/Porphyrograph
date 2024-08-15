////////////////////////////////////////////////////////////////////
// _
// 
// 
//     File pg-update.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-light-all_include.h"

#if defined(_CRTDBG_MAP_ALLOC)
// MEMORY LEAK CONTROL
_CrtMemState s1{};
_CrtMemState s2{};
_CrtMemState s3{};
_CrtMemState s4{};
_CrtMemState s5{};
_CrtMemState s6{};
_CrtMemState s7{};
_CrtMemState s8{};
_CrtMemState s9{};
_CrtMemState s10{};
_CrtMemState s11{};
_CrtMemState s12{};
_CrtMemState s13{};
_CrtMemState sDiff{};
#endif
      

float pg_repop_ColorBGcolorRed;
float pg_repop_ColorBG_g;
float pg_repop_ColorBG_b;
float pg_repop_ColorCA_r;
float pg_repop_ColorCA_g;
float pg_repop_ColorCA_b;
float pg_repop_ColorPart_r;
float pg_repop_ColorPart_g;
float pg_repop_ColorPart_b;

// interpolated CA
int CAInterpolatedType;
int CAInterpolatedSubType;
int CAInterpolatedType_prev = -1;
int CAInterpolatedSubType_prev = -1;

// svg gpu translation
float				svg_translate_x = 0.f;
float				svg_translate_y = 0.f;
float				svg_translate_z = 0.f;
// svg gpu scaling
float				svg_scale = 1.f;

// mobile cursor for cursor blinking in Master shader
bool mobile_cursor = true;

int pg_tablet_prec_x = 0;
int pg_tablet_prec_y = 0;

// MVP matrices
glm::mat4 pg_NbTextChapters;
glm::mat4 pg_VP1viewMatrix;
glm::mat4 pg_MeshPosModelMatrix;
glm::mat4 ObjectPosModelMatrix;
glm::mat4 pg_VP1homographyMatrix;
#if defined(PG_SECOND_MESH_CAMERA)
glm::mat4 VP2perspMatrix;
glm::mat4 VP2viewMatrix;
glm::mat4 VP2homographyMatrix;
#endif

// homographyw matrix
float pg_matValues[16] = { 0 };

cv::Mat homography;

float pen_x, pen_y, vec_x, vec_y;

/////////////////////////////////////////////////////////////////
// VIDEO PARAMETERS

std::string* pg_displayTextList = NULL;
int* pg_displayTextFirstInChapter = NULL;
int pg_NbDisplayTexts = 0;
int pg_displayText_maxLen = 0;
int pg_Ind_Current_DisplayText = 0;
float * pg_displayText_rand_translX = NULL;
float * pg_displayText_rand_translY = NULL;

////////////////////////////////////////////////////////////////////
// DIAPORAMA IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
double pg_CurrentDiaporamaEnd = -1;

GLuint Sensor_texture_rectangle[PG_MAX_SCENARIOS] = { NULL_ID };

// pen palettes presets
vector<ColorPreset> pg_ColorPresets[PG_MAX_SCENARIOS];

// lights presets
int pg_nb_light_groups[PG_MAX_SCENARIOS] = {0};
vector<Light> pg_Lights[PG_MAX_SCENARIOS];
vector<LightGroup> pg_light_groups[PG_MAX_SCENARIOS];
// interface current light group
int pg_interface_light_group = 0;

// Create an unordered_map of three strings (that map to strings)
// corresponds to all the parameters associated with DMX light channels
// key constants are from enum pg_light_command_hashMap_IDs
std::unordered_map<int, std::string> pg_light_param_hashMap = {
	{ pg_enum_light_dimmer, "dimmer" },
	{ pg_enum_light_strobe, "strobe" },
	{ pg_enum_light_zoom, "zoom" },
	{ pg_enum_light_pan, "pan" },
	{ pg_enum_light_tilt, "tilt" },
	{ pg_enum_light_hue, "hue" },
	{ pg_enum_light_red, "red" },
	{ pg_enum_light_green, "green" },
	{ pg_enum_light_blue, "blue" },
	{ pg_enum_light_grey, "grey" },
	{ pg_enum_light_palette_color, "palette_color" }
};
// same map but from strings to enum values
std::unordered_map<std::string, int> pg_inverse_light_param_hashMap = {
};
// submap of the parameters which can be looped
std::unordered_map<int, std::string> pg_light_loop_param_hashMap = {
	{ pg_enum_light_zoom, "zoom" },
	{ pg_enum_light_pan, "pan" },
	{ pg_enum_light_tilt, "tilt" }
};

// pen palettes colors
vector<Palette> pg_Palettes[PG_MAX_SCENARIOS];
// photo albums
std::string pg_ImageDirectory[PG_MAX_SCENARIOS] = { "" };
// short video clip albums
string pg_ClipDirectory[PG_MAX_SCENARIOS] = { "" };
int pg_NbClipAlbums[PG_MAX_SCENARIOS] = {0};
int clip_image_width[PG_MAX_SCENARIOS] = {0};
int clip_image_height[PG_MAX_SCENARIOS] = {0};
int clip_crop_width[PG_MAX_SCENARIOS] = {0};
int clip_crop_height[PG_MAX_SCENARIOS] = {0};
int clip_max_length[PG_MAX_SCENARIOS] = {0};
// pen brushes
string pen_brushes_fileName[PG_MAX_SCENARIOS] = { "" };
int nb_pen_brushes[PG_MAX_SCENARIOS] = {0};
// textures with multiple layers
int nb_layers_master_mask[PG_MAX_SCENARIOS] = {0};

// indices of the currently played clips
int pg_playing_clipNoLeft = -1;
int pg_playing_secondClipNoLeft = -1;
int pg_playing_clipNoRight = -1;
int pg_playing_secondClipNoRight = -1;

/*
threadData pDataWriteJpg;
threadData pDataWriteSvg;
threadData pDataWritePng;
*/


///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)

// the glut callback
// requires predrawing (drawing from the user to the root node)

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void pg_window_display(void) {

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("\nstat 1\n"));
	_CrtMemCheckpoint(&s1);
#endif

	//printf("Maser %.2f\n", master);

	pg_windowDisplayed = true;
	// pg_printOglError(508);

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// SCENE UPDATE AND SHADER UNIFORM PARAMETERS UPDATE
	//////////////////////////////////////////////////
	// scenario update 
	pg_update_scenario();
	// ships uniform variables  pg_printOglError(51);
	pg_printOglError(50);
	pg_printOglError(51);

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 7\n"));
	_CrtMemCheckpoint(&s7);
	if (_CrtMemDifference(&sDiff, &s9, &s7))
		_CrtMemDumpStatistics(&sDiff);
#endif

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// SCENE DISPLAY AND SHADER UNIFORM PARAMETERS UPDATE
	//////////////////////////////////////////////////
#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 4\n"));
	_CrtMemCheckpoint(&s4);
	if (_CrtMemDifference(&sDiff, &s7, &s4))
		_CrtMemDumpStatistics(&sDiff);
#endif

	// proper scene redrawing
	pg_draw_scene( pg_enum_render_Display );

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 2\n"));
	_CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&sDiff, &s4, &s2))
		_CrtMemDumpStatistics(&sDiff);
#endif

	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// ONE-FRAME PARAMETERS RESET OR UPDATE (AUTOMATIC, NON SCENARIO-BASED UPDATES)
	//////////////////////////////////////////////////
	// resets to 0 the variables that are only true for one frame such as flashes, resets...
	pg_automatic_var_reset_or_update();

	// flushes OpenGL commands
	glFlush();

	// displays new frame
	glutSwapBuffers();

#if defined(PG_DEBUG)
	OutputDebugStringW(_T("stat 3\n"));
	_CrtMemCheckpoint(&s3);
	if (_CrtMemDifference(&sDiff, &s2, &s3))
		_CrtMemDumpStatistics(&sDiff);
#endif

	// ------------------------------------------------------------ //
	// --------------- FRAME/SUBFRAME GRABBING -------------------- //

	// ---------------- frame by frame output --------------------- //
	// Svg screen shots
	// printf("Draw Svg\n" );
	if (take_snapshots && pg_Svg_Capture_param.outputSvg) {
		// frame count based output
		if (pg_Svg_Capture_param.stepSvgInFrames > 0) {
			if (pg_FrameNo % pg_Svg_Capture_param.stepSvgInFrames == 0
				&& pg_FrameNo / pg_Svg_Capture_param.stepSvgInFrames >= pg_Svg_Capture_param.beginSvg &&
				pg_FrameNo / pg_Svg_Capture_param.stepSvgInFrames <= pg_Svg_Capture_param.endSvg) {
				pg_draw_scene( pg_enum_render_Svg );
			}
		}
		else if (pg_Svg_Capture_param.stepSvgInSeconds > 0) {
			if (pg_Svg_Capture_param.nextSvgCapture < 0) {
				pg_Svg_Capture_param.nextSvgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Svg_Capture_param.nextSvgCapture
				&& pg_CurrentClockTime >= pg_Svg_Capture_param.beginSvg &&
				pg_CurrentClockTime <= pg_Svg_Capture_param.endSvg) {
				pg_Svg_Capture_param.nextSvgCapture = max(pg_Svg_Capture_param.nextSvgCapture + pg_Svg_Capture_param.stepSvgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Svg );
			}
		}
	}

	// ---------------- frame by frame output --------------------- //
	// Png screen shots
	// printf("Draw Png\n" );
	if (take_snapshots && pg_Png_Capture_param.outputPng ) {
		// frame count based output
		if (pg_Png_Capture_param.stepPngInFrames > 0) {
			if (pg_FrameNo % pg_Png_Capture_param.stepPngInFrames == 0
				&& pg_FrameNo / pg_Png_Capture_param.stepPngInFrames >= pg_Png_Capture_param.beginPng &&
				pg_FrameNo / pg_Png_Capture_param.stepPngInFrames <= pg_Png_Capture_param.endPng) {
				pg_draw_scene( pg_enum_render_Png );
			}
		}
		else if (pg_Png_Capture_param.stepPngInSeconds > 0) {
			if (pg_Png_Capture_param.nextPngCapture < 0) {
				pg_Png_Capture_param.nextPngCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Png_Capture_param.nextPngCapture
				&& pg_CurrentClockTime >= pg_Png_Capture_param.beginPng &&
				pg_CurrentClockTime <= pg_Png_Capture_param.endPng) {
				pg_Png_Capture_param.nextPngCapture = max(pg_Png_Capture_param.nextPngCapture + pg_Png_Capture_param.stepPngInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Png );
			}
		}
	}

	// ---------------- frame by frame output --------------------- //
	// Jpg screen shots
	// printf("Draw Jpg\n"  );
	if (take_snapshots && pg_Jpg_Capture_param.outputJpg) {
		// frame count based output
		if (pg_Jpg_Capture_param.stepJpgInFrames > 0) {
			if (pg_FrameNo % pg_Jpg_Capture_param.stepJpgInFrames == 0
				&& pg_FrameNo / pg_Jpg_Capture_param.stepJpgInFrames >= pg_Jpg_Capture_param.beginJpg &&
				pg_FrameNo / pg_Jpg_Capture_param.stepJpgInFrames <= pg_Jpg_Capture_param.endJpg) {
				pg_draw_scene( pg_enum_render_Jpg );
			}
		}
		else if (pg_Jpg_Capture_param.stepJpgInSeconds > 0) {
			if (pg_Jpg_Capture_param.nextJpgCapture < 0) {
				pg_Jpg_Capture_param.nextJpgCapture = pg_CurrentClockTime;
			}
			if (pg_CurrentClockTime >= pg_Jpg_Capture_param.nextJpgCapture
				&& pg_CurrentClockTime >= pg_Jpg_Capture_param.beginJpg &&
				pg_CurrentClockTime <= pg_Jpg_Capture_param.endJpg) {
				pg_Jpg_Capture_param.nextJpgCapture = max(pg_Jpg_Capture_param.nextJpgCapture + pg_Jpg_Capture_param.stepJpgInSeconds, pg_CurrentClockTime);
				pg_draw_scene( pg_enum_render_Jpg );
				//printf("nextJpgCapture %.2f pg_CurrentClockTime %.2f\n", nextJpgCapture, pg_CurrentClockTime);
			}
		}
	}
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// SCENE VARIABLE UPDATES
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// restores variables to 0 or false so that
// they do not stay on more than one frame (flashes, initialization...)
void pg_automatic_var_reset_or_update(void) {
	
	// clear 3 frames
	// clear CA reset 
	// clear layer reset
	// clear all layers reset
	// clear echo reset
	(pg_isClearCA > 0.f ? pg_isClearCA -= 0.35f : pg_isClearCA = 0.f);
	(pg_isClearLayer > 0.f ? pg_isClearLayer -= 0.35f : pg_isClearLayer = 0.f);
	(pg_isClearAllLayers > 0.f ? pg_isClearAllLayers -= 0.35f : pg_isClearAllLayers = 0.f);
	(pg_isClearEcho > 0.f ? pg_isClearEcho -= 0.35f : pg_isClearEcho = 0.f);

	// layer copy reset	
	// copy to layer above (+1) or to layer below (-1)
	pg_copyToNextTrack = 0;

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_cameraCaptFreq]) {
		// DELAYED CAMERA WEIGHT COUNTDOWN
		if (pg_delayedCameraWeight > 1) {
			pg_delayedCameraWeight--;
		}
		else if (pg_delayedCameraWeight == 1) {
			*((float*)pg_FullScenarioVarPointers[_cameraWeight]) = 1;
			pg_BrokenInterpolationVar[_cameraWeight] = true;
			cameraWeight = 1;
			pg_delayedCameraWeight = 0;
		}
	}

#if defined(PG_WITH_BLUR)
	// blur reset
	if (nb_blur_frames_1 > 0) {
		nb_blur_frames_1--;
		if (nb_blur_frames_1 <= 0) {
			is_blur_1 = false;
		}
	}
	if (nb_blur_frames_2 > 0) {
		nb_blur_frames_2--;
		if (nb_blur_frames_2 <= 0) {
			is_blur_2 = false;
		}
	}
#endif

#if defined(pg_Project_CAaudio)
	// CA seed
	pg_CAseed_trigger = false;
#endif

	// /////////////////////////
	// particle initialization reset
	part_initialization = -1;

	// automatic master incay/decay
	if (pg_master_incay_duration > 0) {
		if (pg_master_incay_start_time + pg_master_incay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_incay_start_time) / pg_master_incay_duration)));
			master = pg_master_incay_start_value + alpha * (1.f - pg_master_incay_start_value);
		}
		else {
			pg_master_incay_duration = 0.;
			master = 1.f;
		}
		pg_BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
	if (pg_master_decay_duration > 0) {
		if (pg_master_decay_start_time + pg_master_decay_duration >= pg_CurrentClockTime) {
			float alpha = max(0.f, min(1.f, float((pg_CurrentClockTime - pg_master_decay_start_time) / pg_master_decay_duration)));
			master = (1.f - alpha) * pg_master_decay_start_value;
		}
		else {
			pg_master_decay_duration = 0.;
			master = 0.f;
		}
		pg_BrokenInterpolationVar[_master] = true;
		*((float*)pg_FullScenarioVarPointers[_master]) = master;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// DRAWING A SCENE ON VARIOUS MODALITIES (CURVE, IMAGE, FRAMEBUFFER...)
//////////////////////////////////////////////////////////////////////////////////////////////
void pg_draw_scene(DrawingMode mode) {
	// sets viewport to single window
	//printf("Draw scene %dx%d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT);
	glViewport(0, 0, pg_workingWindow_width, PG_WINDOW_HEIGHT);
	// buffer reset
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// no z-Buffer
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0.f, 0.f, 0.2f, 1.f);
	pg_printOglError(686);
}

