/*! \file pg-clipArt.cpp
 *
 *
 *     File pg-clipArt.cpp
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

bool pg_has_NV_path_rendering = false;

// ClipArt GPU
// number of files
vector<ClipArt> pg_ClipArts[PG_MAX_SCENARIOS];

// last activated clipArt
ClipArt* pg_last_activated_ClipArt = NULL;

// total number of paths
int pg_nb_tot_SvgGpu_paths[PG_MAX_SCENARIOS];
// base ID of the GPU paths
GLuint pg_ClipArt_path_baseID[PG_MAX_SCENARIOS];
// fill color table
unsigned int** pg_ClipArt_path_fill_color[PG_MAX_SCENARIOS];


void pg_initializeNVPathRender(void) {
	pg_has_NV_path_rendering = (glutExtensionSupported("GL_NV_path_rendering") != 0);
	if (!pg_has_NV_path_rendering) {
		sprintf(pg_errorStr, "Error: required NV_path_rendering OpenGL extension is not present\n"); pg_ReportError(pg_errorStr);
	}
}


void pg_LoadClipArtPathsToGPU(string fileName, int nb_gpu_paths, int indClipArtFile, int indScenario) {
	string splitFileName = fileName;
	splitFileName.replace(fileName.length() - 4, 0, "_split");
	sprintf(pg_errorStr, "python  C:\\home\\LYM-sources\\vv\\vv_python\\utils\\vv_one_SVG_tag_per_line.py -i %s -o %s",
		fileName.c_str(), splitFileName.c_str());
	system(pg_errorStr);

	//std::cout << "Loading ClipArt GPU path " << splitFileName << "\n";
	std::ifstream pathFin(splitFileName);
	if (!pathFin) {
		std::cout << "Error: clip art file [" << splitFileName << "] not found!\n";
		exit(0);
	}
	std::string line;
	int ind_gpu_path = 0;
	// counts the number of non empty lines -> the number of paths
	while (std::getline(pathFin, line))
	{
		if (!line.empty() && (line.find("<path") != std::string::npos) && (line.find(" d=\"") != std::string::npos)
			&& (line.find(" d=\"\"") == std::string::npos))
			ind_gpu_path++;
	}
	if (ind_gpu_path != nb_gpu_paths) {
		std::cout << "Error: " << nb_gpu_paths << " layers expected in file [" << fileName << "] (" << ind_gpu_path << ")!\n";
		exit(0);
	}

	// rewind the file
	pathFin.clear();
	pathFin.seekg(0);

	// loads the paths
	ind_gpu_path = 0;
	while (std::getline(pathFin, line)) {
		if (!line.empty() && ind_gpu_path < nb_gpu_paths) {
			switch (pg_ClipArts[indScenario][indClipArtFile].pg_ClipArt_Colors[ind_gpu_path]) {
			case pg_enum_ClipArt_nat:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0x888888"), nullptr, 16));
				break;
			case pg_enum_ClipArt_white:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0xFFFFFF"), nullptr, 16));
				break;
			case pg_enum_ClipArt_red:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0xFF0000"), nullptr, 16));
				break;
			case pg_enum_ClipArt_green:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0x00FF00"), nullptr, 16));
				break;
			case pg_enum_ClipArt_blue:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0x0000FF"), nullptr, 16));
				break;
			case pg_enum_ClipArt_yellow:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0xFFFF00"), nullptr, 16));
				break;
			case pg_enum_ClipArt_cyan:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0x00FFFF"), nullptr, 16));
				break;
			case pg_enum_ClipArt_magenta:
				pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path]
					= (unsigned int)(std::stoi(std::string("0xFF00FF"), nullptr, 16));
				break;
			}
			std::size_t found = std::string::npos;
			found = line.find("<path");
			if (found != std::string::npos) {
				// looks for a fill (inside a css style attribute)
				found = line.find("fill:#");
				if (found != std::string::npos) {
					// copies the the fill
					std::string lineAux = line.substr(found + strlen("fill:#"));
					// loooks for string endingx
					found = lineAux.find(";");
					if (found == std::string::npos) {
						found = lineAux.find("\"");
					}
					if (found != std::string::npos) {
						lineAux = lineAux.substr(0, found);
						lineAux = std::string("0x") + lineAux;
						//printf("File %d Fill color of path #%d: %s\n", indClipArtFile, ind_gpu_path, lineAux.c_str());
						pg_ClipArt_path_fill_color[indScenario][indClipArtFile][ind_gpu_path] = (unsigned int)(std::stoi(lineAux, nullptr, 16));
					}
				}
				// looks for a path
				found = line.find(" d=\"");
				if (found != std::string::npos) {
					// copies the beginning of the path
					line = line.substr(found + strlen(" d=\""));
					// loooks for string ending
					found = line.find("\"");
					// " is found in the same line
					if (found != std::string::npos) {
						line = line.substr(0, found);
					}
					else {
						// concatenates string line by line
						while (found == std::string::npos) {
							std::string lineAux;
							std::getline(pathFin >> std::ws, lineAux);
							found = lineAux.find("\"");
							if (found != std::string::npos) {
								lineAux = lineAux.substr(0, found);
							}
							line = line + lineAux;
						}
					}

					//if (pg_ClipArt_nb_loaded_paths == 0) {
					//	 std::cout << "path: [" << line << "]\n";
					//}

					if (line.length() > 0) {
						ind_gpu_path++;

						// const char *svg_str = "M-122.304 84.285C-122.304 84.285 -122.203 86.179 -123.027 86.16C-123.851 86.141 -140.305 38.066 -160.833 40.309C-160.833 40.309 -143.05 32.956 -122.304 84.285z";
						size_t svg_len = line.size();
						const char* svg_str = line.c_str();
						glPathStringNV(pg_ClipArts[indScenario][indClipArtFile].ClipArt_file_baseID + ind_gpu_path - 1, GL_PATH_FORMAT_SVG_NV,
							(GLsizei)svg_len, svg_str);
					}
				}
			}
		}
		else if (!line.empty() && ind_gpu_path >= nb_gpu_paths) {
			std::size_t found = std::string::npos;
			found = line.find("<path");
			if (found != std::string::npos) {
				// looks for a non-empty path
				found = line.find(" d=\"\"");
				if (found == std::string::npos) {
					std::cout << "Error: " << nb_gpu_paths << " layers expected in file [" << fileName << "] more are given !\n";
					exit(0);
				}
			}
		}
	}
	if (ind_gpu_path != nb_gpu_paths) {
		std::cout << "Error: " << nb_gpu_paths << " layers expected in file [" << fileName << "] !\n";
		exit(0);
	}
	pathFin.close();
	pg_printOglError(4);
}

static void pg_OpenGLBasicColors(int color) {
	switch (color) {
	case 0:
		glColor3ub(0, 0, 0);
		break;
	case 1:
		glColor3ub(255, 0, 0);
		break;
	case 2:
		glColor3ub(0, 255, 0);
		break;
	case 3:
		glColor3ub(0, 0, 255);
		break;
	case 4:
		glColor3ub(255, 255, 0);
		break;
	case 5:
		glColor3ub(255, 0, 255);
		break;
	case 6:
		glColor3ub(0, 255, 255);
		break;
	case 7:
		glColor3ub(255, 255, 255);
		break;
	}
}
static void pg_OpenGLIntColors(unsigned int color) {
	unsigned int red, green, blue;
	blue = color % 256;
	green = ((color - blue) / 256) % 256;
	red = (color - blue - 256 * green) / (256 * 256);
	//printf("color % d, red % d green % d blue % d\n", color, red, green, blue);
	glColor3ub((unsigned char)(red), (unsigned char)(green), (unsigned char)(blue));
}
static void pg_OpenGLPaletteColors(Color& col, float palette_pulse) {
	float pulsed_color[3] = { 0.f };
	pg_compute_pulsed_palette_color(col.color, palette_pulse, col.grey, palette_pulse, pulsed_color, pg_enum_CLIP_ART_COLOR);
	unsigned int red, green, blue;
	red = (unsigned int)(pulsed_color[0] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	green = (unsigned int)(pulsed_color[1] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	blue = (unsigned int)(pulsed_color[2] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	//printf("color % d, red % d green % d blue % d\n", color, red, green, blue);
	glColor3ub((unsigned char)(red), (unsigned char)(green), (unsigned char)(blue));
}

static void pg_Display_One_ClipArt(int indClipArt, int indLayer) {
	int low_palette = -1;
	float alpha = 0;
	float palette_pulse = 0.f;

	if (!pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		return;
	}

	if (indLayer >= 0 && indLayer < PG_NB_CLIPART_LAYERS) {
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_ClipArt_layer_color_preset]) {
			if (ClipArt_layer_color_preset[indLayer + 1] >= 0) {
				low_palette = int(floor(ClipArt_layer_color_preset[indLayer + 1]));
				alpha = ClipArt_layer_color_preset[indLayer + 1] - low_palette;
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_ClipArt_layer_color_preset_pulse]) {
					palette_pulse = ClipArt_layer_color_preset_pulse[indLayer + 1];
				}
			}
		}
	}

	// scenario-based clip art color for surface filling
	if (low_palette >= 0) {
		low_palette = low_palette % pg_ColorPresets[pg_ind_scenario].size();
		int high_palette = (low_palette + 1) % pg_ColorPresets[pg_ind_scenario].size();
		Color color_interp = (pg_ColorPresets[pg_ind_scenario][low_palette].pg_colorPreset_values * (1.f - alpha))
			+ (pg_ColorPresets[pg_ind_scenario][high_palette].pg_colorPreset_values * alpha);
		//if (indLayer == 0) {
		//	color_interp.print();
		//}
		pg_OpenGLPaletteColors(color_interp, palette_pulse);
	}
	else {
		//printf("config %d active clipart display %d layer %d color %d\n", pg_ind_scenario, indClipArt, indLayer, 
		//	pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_path_fill_color[indLayer]);
		// original clip art color for surface filling
		pg_OpenGLIntColors(pg_ClipArt_path_fill_color[pg_ind_scenario][indClipArt][indLayer]);
	}
	glStencilFillPathNV(pg_ClipArts[pg_ind_scenario][indClipArt].ClipArt_file_baseID + indLayer, GL_COUNT_UP_NV, 0x1F);
	glCoverFillPathNV(pg_ClipArts[pg_ind_scenario][indClipArt].ClipArt_file_baseID + indLayer, GL_BOUNDING_BOX_NV);

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_ClipArt_width]
		&& ClipArt_width > 0.f) {
		// clip art surface contouring
		low_palette = -1;
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_ClipArt_stroke_color_preset]) {
			if (ClipArt_stroke_color_preset >= 0) {
				low_palette = int(floor(ClipArt_stroke_color_preset));
				alpha = ClipArt_stroke_color_preset - low_palette;
				palette_pulse = ClipArt_stroke_color_preset_pulse;
			}
		}

		// scenario-based clip art color for stroke
		if (low_palette >= 0) {
			low_palette = low_palette % pg_ColorPresets[pg_ind_scenario].size();
			int high_palette = (low_palette + 1) % pg_ColorPresets[pg_ind_scenario].size();
			Color color_interp = (pg_ColorPresets[pg_ind_scenario][low_palette].pg_colorPreset_values * (1.f - alpha))
				+ (pg_ColorPresets[pg_ind_scenario][high_palette].pg_colorPreset_values * alpha);
			//if (indLayer == 0) {
			//	color_interp.print();
			//}
			pg_OpenGLPaletteColors(color_interp, palette_pulse);
		}
		else {
			// black
			glColor3ub((unsigned char)0, (unsigned char)0, (unsigned char)0);
		}
		glPathParameterfNV(pg_ClipArts[pg_ind_scenario][indClipArt].ClipArt_file_baseID + indLayer, GL_PATH_STROKE_WIDTH_NV, ClipArt_width);
		glStencilStrokePathNV(pg_ClipArts[pg_ind_scenario][indClipArt].ClipArt_file_baseID + indLayer, GL_COUNT_UP_NV, 0x1F);
		glCoverStrokePathNV(pg_ClipArts[pg_ind_scenario][indClipArt].ClipArt_file_baseID + indLayer, GL_BOUNDING_BOX_NV);
	}
}

//////////////////////////////////////////////////
// RENDERING GPU ClipArt IF SOME LAYERS ARE ACTIVE
void pg_Display_All_ClipArt(int activeFiles) {
	//printf("active cliparts: %d\n", activeFiles);
	if (pg_has_NV_path_rendering
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (activeFiles != 0 && pg_shader_programme[pg_ind_scenario][pg_enum_shader_ClipArt]) {
			glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ClipArt]);
			//glDisable(GL_DEPTH_TEST);

			//glClearStencil(0);
			// glClearColor(1, 1, 1, 1);

			// glEnable(GL_BLEND);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
			glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

			glClear(GL_STENCIL_BUFFER_BIT);
			glMatrixPushEXT(GL_PROJECTION); {
				glMatrixLoadIdentityEXT(GL_PROJECTION);
				glMatrixOrthoEXT(GL_PROJECTION, 0, PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT, 0, -1, 1);

				int maxNbDisplayedClipArt = std::min(int(pg_ClipArts[pg_ind_scenario].size()), 32);
				if (activeFiles == -1) { // if activeClipArt is equal to -1: all clip arts are visible
					maxNbDisplayedClipArt = pg_ClipArts[pg_ind_scenario].size();
				}
				for (int indClipArt = 0; indClipArt < maxNbDisplayedClipArt; indClipArt++) {
					if ((activeFiles == -1) || (activeFiles & (1 << indClipArt))) {
						glMatrixPushEXT(GL_MODELVIEW); {
							glMatrixLoadIdentityEXT(GL_MODELVIEW);
							glTranslatef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Translation_X, pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Translation_Y, 0);
							glRotatef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Rotation, 0, 0, 1);
							glScalef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Scale, pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Scale, 1);
							//printf("config %d active clipart display %d layer color: ", pg_ind_scenario, indClipArt);
							//for (int indLayer = 0;
							//	indLayer < pg_ClipArts[pg_ind_scenario][indClipArt].pg_nb_paths_in_ClipArt;
							//	indLayer++) {
							//	printf("%.2f ", ClipArt_layer_color_preset[indLayer + 1]);
							//}
							//printf("\n");
							for (int indLayer = 0;
								indLayer < pg_ClipArts[pg_ind_scenario][indClipArt].pg_nb_paths_in_ClipArt;
								indLayer++) {
								if (indLayer < pg_ClipArts[pg_ind_scenario][indClipArt].pg_nb_paths_in_ClipArt
									&& pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_SubPath[indLayer] == true) {
									//printf("config %d active clipart display %d layer %d\n", pg_ind_scenario, indClipArt, indLayer);
									pg_Display_One_ClipArt(indClipArt, indLayer);
								}
							}
						}
						glMatrixPopEXT(GL_MODELVIEW);
					}
				}
			} glMatrixPopEXT(GL_PROJECTION);
			glDisable(GL_STENCIL_TEST);
		}
		pg_printOglError(5257);
	}
}

void pg_listAllClipArts(void) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		printf("Listing ClipArts:\n");
		for (int indScenario = 0; indScenario < pg_NbConfigurations; indScenario++) {
			std::cout << "    " << indScenario << ": ";
			for (ClipArt& aClipArt : pg_ClipArts[indScenario]) {
				std::cout << aClipArt.pg_ClipArt_fileNames << " (" << aClipArt.pg_nb_paths_in_ClipArt << " paths), ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

//////////////////////////////////////////////////
// RENDERING OF MESSAGE THROUGH ClipArt GPU CHARACTERS
float pg_Translate_ClipArt_Text(int indDisplayText) {
	return float(log(indDisplayText / 3.f) * 210 + 160);
	// return ((indDisplayText % 370) / 10) * 30.f;
}
void pg_convertTextStringToClipartIndices(std::vector<int>* indClipArts, string displayed_text) {
	// decomposition of the string into characters and svg index lookup
	for (unsigned int indChar = 0; indChar < displayed_text.size(); indChar++) {
		// indChar character code
		char curChar = displayed_text.at(indChar);
		int indClipArt = -1;
		// corresponding index of clipart (the cliparts are declared in the scenario file)
		if (curChar >= 'A' && curChar <= 'Z') {
			indClipArt = curChar - 'A';
		}
		else {
			switch (curChar) {
			case ' ':  indClipArt = 26; break;
			case '\'':  indClipArt = 27; break;
			case '#':  indClipArt = 28; break;
			}
		}
		(*indClipArts)[indChar] = indClipArt;
	}
}
void pg_Display_ClipArt_Text(int* ind_Current_DisplayText, int mobile) {
	for (int indLine = 0; indLine < 60; indLine++) {
		if ((*ind_Current_DisplayText) >= 0 && (*ind_Current_DisplayText) < pg_NbDisplayTexts
			&& pg_shader_programme[pg_ind_scenario][pg_enum_shader_ClipArt]) {
			glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_ClipArt]);
			//glDisable(GL_DEPTH_TEST);

			//glClearStencil(0);
			// glClearColor(1, 1, 1, 1);

			// glEnable(GL_BLEND);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_NOTEQUAL, 0, 0x1F);
			glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);

			glClear(GL_STENCIL_BUFFER_BIT);
			glMatrixPushEXT(GL_PROJECTION);
			glMatrixLoadIdentityEXT(GL_PROJECTION);
			glMatrixOrthoEXT(GL_PROJECTION, 0, PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT, 0, -1, 1);
			glMatrixPushEXT(GL_MODELVIEW);
			std::string displayed_text("");

			// the string to display is loaded from a text file
			int ind_current_displayed_line = max(0, (*ind_Current_DisplayText) - 10 * indLine);
			displayed_text = pg_displayTextList[ind_current_displayed_line];

			vector<int> indClipArts(displayed_text.size(), 26);
			pg_convertTextStringToClipartIndices(&indClipArts, displayed_text);

			// decomposition of the string into characters and svg index lookup
			for (unsigned int indChar = 0; indChar < displayed_text.size(); indChar++) {
				// displays the character
				unsigned int indClipArt = indClipArts[indChar];
				if (indClipArt >= 0 && indClipArt < pg_ClipArts[pg_ind_scenario].size()) {
					//printf("active clipart display %d\n", indClipArt);
					glMatrixLoadIdentityEXT(GL_MODELVIEW);
					float y_transl = 0.f;
					// value given by the variable moving_messages in the scenario
					// the message is moving vertically (log based falling function)
					if (mobile) {
						y_transl = pg_Translate_ClipArt_Text(ind_current_displayed_line) - indLine * 32;
					}
					// the translation is given in the scenario file together with the clipart declaration at the bottom of the scenario
					else {
						y_transl = pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Translation_Y;
					}

					if (mobile == 2 && indChar < (unsigned int)(pg_displayText_maxLen)) {
						pg_displayText_rand_translX[indChar] += (rand_0_1 - 0.5f) * (1.f + float(pg_current_scene_percent) * 10.f);
						pg_displayText_rand_translY[indChar] += (rand_0_1 - 0.5f) * (1.f + float(pg_current_scene_percent) * 10.f) + rand_0_1 * 0.2f * (1.f + float(pg_current_scene_percent) * 10.f);
					}
					else {
						pg_displayText_rand_translX[indChar] = 0.f;
						pg_displayText_rand_translY[indChar] = 0.f;
					}

					// XxY translation of the character
					// Y translation is given by motion or by scenario
					// X translation is the scenario translation + the horizontal shift due to position in the string
					glTranslatef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Translation_X * 1.5f
						+ pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Translation_X * indChar + pg_displayText_rand_translX[indChar],
						y_transl + pg_displayText_rand_translY[indChar], 0);

					//glRotatef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Rotation, 0, 0, 1);
					//glScalef(pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Scale, pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Scale, 1);
					// the clipart can be made of several sub-paths, only display the ones that are not been set to off
					for (int indLayer = 0;
						indLayer < pg_ClipArts[pg_ind_scenario][indClipArt].pg_nb_paths_in_ClipArt;
						indLayer++) {
						//if (pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_SubPath[indLayer] == true) {
							//std::cout << "pg_Display_One_ClipArt COLOR " << pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Colors[indLayer] 
							//	<< " ind PATH " << indLayer << " ind CLIPART " << indClipArt << std::endl;
						float scale = pg_ClipArts[pg_ind_scenario][indClipArt].pg_ClipArt_Scale;
						glScalef(scale, scale, scale);
						pg_Display_One_ClipArt(indClipArt, indLayer);
						//}
						//else {
						//	printf("subpath not visible\n");
						//}
					}
				}
			}
			glMatrixPopEXT(GL_MODELVIEW);
			glMatrixPopEXT(GL_PROJECTION);
			glDisable(GL_STENCIL_TEST);
		}
	}
	if (pg_FrameNo % 3 == 0) {
		(*ind_Current_DisplayText) = int(min(float((*ind_Current_DisplayText) + 1), float(pg_NbDisplayTexts - 1)));
	}

	pg_printOglError(5257);
}

//////////////////////////////////////////////////
// LOAD ALL CLIPARTS
void pg_loadAllClipArts(void) {
	//std::cout << "Loading ClipArts\n";
	for (int indScenario = 0; indScenario < pg_NbConfigurations; indScenario++) {
		//std::cout << "    " << indScenario << ": ";
		if (pg_FullScenarioActiveVars[indScenario][_activeClipArts]) {
			const char* ClipArt_programName = "nvpr_ClipArt";
			//std::cout << "Loading " << pg_ClipArts[indScenario].size() << " ClipArt\n";
			pg_initializeNVPathRender();
			if (pg_has_NV_path_rendering) {
				pg_ClipArt_path_baseID[indScenario] = glGenPathsNV(pg_nb_tot_SvgGpu_paths[indScenario]);
				pg_ClipArt_path_fill_color[indScenario] = new unsigned int* [pg_ClipArts[indScenario].size()];
				int nb_tot_paths = 0;
				for (unsigned int indClipArtFile = 0; indClipArtFile < pg_ClipArts[indScenario].size(); indClipArtFile++) {
					//std::cout << pg_ClipArts[indScenario][indClipArtFile].pg_ClipArt_fileNames << " (" << pg_ClipArts[indScenario][indClipArtFile].pg_nb_paths_in_ClipArt << " paths), ";
					pg_ClipArt_path_fill_color[indScenario][indClipArtFile] = new unsigned int[pg_nb_tot_SvgGpu_paths[indScenario]];
					pg_ClipArts[indScenario][indClipArtFile].ClipArt_file_baseID = pg_ClipArt_path_baseID[indScenario] + nb_tot_paths;
					pg_LoadClipArtPathsToGPU(pg_ClipArts[indScenario][indClipArtFile].pg_ClipArt_fileNames,
						pg_ClipArts[indScenario][indClipArtFile].pg_nb_paths_in_ClipArt, indClipArtFile, indScenario);
					nb_tot_paths += pg_ClipArts[indScenario][indClipArtFile].pg_nb_paths_in_ClipArt;
				}
			}
			else {
				sprintf(pg_errorStr, "Error: required NV_path_rendering OpenGL extension is not present\n"); pg_ReportError(pg_errorStr); throw 23771;
			}
		}
	}
}

void pg_ClipArt_OnOff(int indCLipArt) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (indCLipArt <= int(pg_ClipArts[pg_ind_scenario].size()) && activeClipArts != -1) { // activeClipArts == -1 <=> all ClipArt always visible
			bool isClipArtOn = activeClipArts & (1 << (indCLipArt - 1));
			if (isClipArtOn) {
				activeClipArts = activeClipArts & ~(1 << (indCLipArt - 1));
			}
			else {
				activeClipArts |= (1 << (indCLipArt - 1));
				pg_last_activated_ClipArt = &(pg_ClipArts[pg_ind_scenario][indCLipArt - 1]);
			}
			pg_BrokenInterpolationVar[_activeClipArts] = true;
			*((int*)pg_FullScenarioVarPointers[_activeClipArts]) = activeClipArts;
			sprintf(pg_AuxString, "/ClipArt_%d_onOff %d", indCLipArt, (!isClipArtOn)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void pg_ClipArt_Off(int indCLipArt) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (indCLipArt <= int(pg_ClipArts[pg_ind_scenario].size()) && activeClipArts != -1) { // activeClipArts == -1 <=> all ClipArt always visible
			bool isClipArtOn = activeClipArts & (1 << (indCLipArt - 1));
			if (isClipArtOn) {
				activeClipArts = activeClipArts & ~(1 << (indCLipArt - 1));
				pg_BrokenInterpolationVar[_activeClipArts] = true;
				*((int*)pg_FullScenarioVarPointers[_activeClipArts]) = activeClipArts;
				sprintf(pg_AuxString, "/ClipArt_%d_onOff %d", indCLipArt, (!isClipArtOn)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
}

void pg_ClipArt_On(int indCLipArt) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeClipArts]) {
		if (indCLipArt <= int(pg_ClipArts[pg_ind_scenario].size() && activeClipArts != -1)) { // activeClipArts == -1 <=> all ClipArt always visible
			bool isClipArtOn = activeClipArts & (1 << (indCLipArt - 1));
			if (!isClipArtOn) {
				activeClipArts |= (1 << (indCLipArt - 1));
				pg_last_activated_ClipArt = &(pg_ClipArts[pg_ind_scenario][indCLipArt - 1]);
				pg_BrokenInterpolationVar[_activeClipArts] = true;
				*((int*)pg_FullScenarioVarPointers[_activeClipArts]) = activeClipArts;
				sprintf(pg_AuxString, "/ClipArt_%d_onOff %d", indCLipArt, (!isClipArtOn)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
			}
		}
	}
}

void pg_ClipArt_SubPathOnOff(int indPath) {
	if (indPath - 1 < pg_last_activated_ClipArt->pg_nb_paths_in_ClipArt) {
		pg_last_activated_ClipArt->pg_ClipArt_SubPath[indPath - 1]
			= !pg_last_activated_ClipArt->pg_ClipArt_SubPath[indPath - 1];
	}
}
