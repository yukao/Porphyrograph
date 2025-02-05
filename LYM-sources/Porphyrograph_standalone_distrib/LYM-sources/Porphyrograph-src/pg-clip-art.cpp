/*! \file pg-svg-clip-art.cpp
 *
 *
 *     File pg-svg-clip-art.cpp
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

 /* based on ClipArt.cpp - draw paths */

#include "pg-all_include.h"

bool has_NV_path_rendering = false;

GLuint ClipArt_path_baseID[_NbConfigurations] = { 0 };
GLuint *ClipArt_file_baseID[_NbConfigurations] = { NULL };
unsigned int** ClipArt_path_fill_color[_NbConfigurations] = { NULL };
void initializeNVPathRender(const char *programName) {
	has_NV_path_rendering = (glutExtensionSupported("GL_NV_path_rendering") != 0);
	if (!has_NV_path_rendering) {
		sprintf(ErrorStr, "Error: required NV_path_rendering OpenGL extension is not present\n"); ReportError(ErrorStr);
	}
}


void LoadClipArtPathsToGPU(string fileName, int nb_gpu_paths, int indClipArtFile, int indConfiguration) {
	string splitFileName = fileName;
	splitFileName.replace(fileName.length() - 4, 0, "_split");
	sprintf(ErrorStr,"python  C:\\sync.com\\Sync\\LYM-sources\\vv\\vv_python\\utils\\vv_one_SVG_tag_per_line.py -i ./%s -o ./%s",
		fileName.c_str(), splitFileName.c_str());
	system(ErrorStr);

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
			switch (pg_ClipArt_Colors[indConfiguration][indClipArtFile][ind_gpu_path]) {
			case ClipArt_nat:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0x888888"), nullptr, 16));
				break;
			case ClipArt_white:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path] 
					= unsigned int(std::stoul(std::string("0xFFFFFF"), nullptr, 16));
				break;
			case ClipArt_red:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0xFF0000"), nullptr, 16));
				break;
			case ClipArt_green:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0x00FF00"), nullptr, 16));
				break;
			case ClipArt_blue:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0x0000FF"), nullptr, 16));
				break;
			case ClipArt_yellow:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0xFFFF00"), nullptr, 16));
				break;
			case ClipArt_cyan:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0x00FFFF"), nullptr, 16));
				break;
			case ClipArt_magenta:
				ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path]
					= unsigned int(std::stoul(std::string("0xFF00FF"), nullptr, 16));
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
						ClipArt_path_fill_color[indConfiguration][indClipArtFile][ind_gpu_path] = unsigned int(std::stoul(lineAux, nullptr, 16));
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
						glPathStringNV(ClipArt_file_baseID[indConfiguration][indClipArtFile] + ind_gpu_path - 1, GL_PATH_FORMAT_SVG_NV,
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
	printOglError(4);
}

static void OpenGLBasicColors(int color) {
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
static void OpenGLIntColors(unsigned int color) {
	unsigned int red, green, blue;
	blue = color % 256;
	green = ((color - blue) / 256) % 256;
	red = (color - blue - 256 * green) / (256 * 256);
	//printf("color % d, red % d green % d blue % d\n", color, red, green, blue);
	glColor3ub(unsigned char(red), unsigned char(green), unsigned char(blue));
}
static void OpenGLPaletteColors(Color &col, float palette_pulse) {
	float pulsed_color[3] = { 0.f };
	compute_pulsed_palette_color(col.color, palette_pulse, col.grey, palette_pulse, pulsed_color, false);
	unsigned int red, green, blue;
	red = unsigned int(pulsed_color[0] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	green = unsigned int(pulsed_color[1] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	blue = unsigned int(pulsed_color[2] * (col.alpha + pulse_average * palette_pulse) * 255) % 256;
	//printf("color % d, red % d green % d blue % d\n", color, red, green, blue);
	glColor3ub(unsigned char(red), unsigned char(green), unsigned char(blue));
}

static void pg_Display_One_ClipArt(pg_ClipArt_Colors_Types color, int indClipArt, int indLayer) {
	int low_palette = -1;
	float alpha = 0;
	float palette_pulse = 0.f;
#if defined(var_ClipArt_layer_1_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_1_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 0) {
			if (ClipArt_layer_1_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_1_color_preset));
				alpha = ClipArt_layer_1_color_preset - low_palette;
#if defined(var_ClipArt_layer_1_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_1_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_1_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_2_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_2_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 1) {
			if (ClipArt_layer_2_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_2_color_preset));
				alpha = ClipArt_layer_2_color_preset - low_palette;
#if defined(var_ClipArt_layer_2_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_2_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_2_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_3_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_3_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 2) {
			if (ClipArt_layer_3_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_3_color_preset));
				alpha = ClipArt_layer_3_color_preset - low_palette;
#if defined(var_ClipArt_layer_3_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_3_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_3_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_4_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_4_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 3) {
			if (ClipArt_layer_4_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_4_color_preset));
				alpha = ClipArt_layer_4_color_preset - low_palette;
#if defined(var_ClipArt_layer_4_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_4_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_4_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_5_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_5_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 4) {
			if (ClipArt_layer_5_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_5_color_preset));
				alpha = ClipArt_layer_5_color_preset - low_palette;
#if defined(var_ClipArt_layer_5_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_5_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_5_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_6_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_6_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 5) {
			if (ClipArt_layer_6_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_6_color_preset));
				alpha = ClipArt_layer_6_color_preset - low_palette;
#if defined(var_ClipArt_layer_6_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_6_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_6_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_7_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_7_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 6) {
			if (ClipArt_layer_7_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_7_color_preset));
				alpha = ClipArt_layer_7_color_preset - low_palette;
#if defined(var_ClipArt_layer_7_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_7_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_7_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
#if defined(var_ClipArt_layer_8_color_preset)
	if (ScenarioVarConfigurations[_ClipArt_layer_8_color_preset][pg_current_configuration_rank]) {
		if (indLayer == 7) {
			if (ClipArt_layer_8_color_preset >= 0) {
				low_palette = int(floor(ClipArt_layer_8_color_preset));
				alpha = ClipArt_layer_8_color_preset - low_palette;
#if defined(var_ClipArt_layer_8_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_layer_8_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_layer_8_color_preset_pulse;
				}
#endif
			}
		}
	}
#endif
	// scenario-based clip art color for surface filling
	if (low_palette >= 0) {
		low_palette = low_palette % nb_pen_colorPresets[pg_current_configuration_rank];
		int high_palette = (low_palette + 1) % nb_pen_colorPresets[pg_current_configuration_rank];
		Color color_interp = (pg_colorPreset_values[pg_current_configuration_rank][low_palette] * (1.f - alpha)) + (pg_colorPreset_values[pg_current_configuration_rank][high_palette] * alpha);
		//if (indLayer == 0) {
		//	color_interp.print();
		//}
		OpenGLPaletteColors(color_interp, palette_pulse);
	}
	else {
		//printf("config %d active clipart display %d layer %d color %d\n", pg_current_configuration_rank, indClipArt, indLayer, 
		//	ClipArt_path_fill_color[pg_current_configuration_rank][indClipArt][indLayer]);
		// original clip art color forsurface filling
		OpenGLIntColors(ClipArt_path_fill_color[pg_current_configuration_rank][indClipArt][indLayer]);
	}
	glStencilFillPathNV(ClipArt_file_baseID[pg_current_configuration_rank][indClipArt] + indLayer, GL_COUNT_UP_NV, 0x1F);
	glCoverFillPathNV(ClipArt_file_baseID[pg_current_configuration_rank][indClipArt] + indLayer, GL_BOUNDING_BOX_NV);

	float width = 0.f;
#if defined(var_ClipArt_width)
	if (ScenarioVarConfigurations[_ClipArt_width][pg_current_configuration_rank]) {
		width = ClipArt_width;
	}
	else
#endif
	{
		width = 0.1f;
	}

	if (width > 0.f) {
		// clip art surface contouring
		low_palette = -1;
#if defined(var_ClipArt_stroke_color_preset)
		if (ScenarioVarConfigurations[_ClipArt_stroke_color_preset][pg_current_configuration_rank]) {
			if (ClipArt_stroke_color_preset >= 0) {
				low_palette = int(floor(ClipArt_stroke_color_preset));
				alpha = ClipArt_stroke_color_preset - low_palette;
#if defined(var_ClipArt_stroke_color_preset_pulse)
				if (ScenarioVarConfigurations[_ClipArt_stroke_color_preset_pulse][pg_current_configuration_rank]) {
					palette_pulse = ClipArt_stroke_color_preset_pulse;
				}
#endif
			}
		}
#endif
		// scenario-based clip art color for stroke
		if (low_palette >= 0) {
			low_palette = low_palette % nb_pen_colorPresets[pg_current_configuration_rank];
			int high_palette = (low_palette + 1) % nb_pen_colorPresets[pg_current_configuration_rank];
			Color color_interp = (pg_colorPreset_values[pg_current_configuration_rank][low_palette] * (1.f - alpha)) + (pg_colorPreset_values[pg_current_configuration_rank][high_palette] * alpha);
			//if (indLayer == 0) {
			//	color_interp.print();
			//}
			OpenGLPaletteColors(color_interp, palette_pulse);
		}
		else {
			// black
			glColor3ub((unsigned char)0, (unsigned char)0, (unsigned char)0);
		}
		glPathParameterfNV(ClipArt_file_baseID[pg_current_configuration_rank][indClipArt] + indLayer, GL_PATH_STROKE_WIDTH_NV, width);
		glStencilStrokePathNV(ClipArt_file_baseID[pg_current_configuration_rank][indClipArt] + indLayer, GL_COUNT_UP_NV, 0x1F);
		glCoverStrokePathNV(ClipArt_file_baseID[pg_current_configuration_rank][indClipArt] + indLayer, GL_BOUNDING_BOX_NV);
	}
}

//////////////////////////////////////////////////
// RENDERING GPU ClipArt IF SOME LAYERS ARE ACTIVE
#if defined(var_activeClipArts)
void pg_Display_All_ClipArt(int activeFiles) {
	//printf("active cliparts: %d\n", activeFiles);
	if (has_NV_path_rendering) {
		if (activeFiles != 0 && pg_shader_programme[pg_current_configuration_rank][_pg_shader_ClipArt]) {
			glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ClipArt]);
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
				glMatrixOrthoEXT(GL_PROJECTION, 0, window_width, window_height, 0, -1, 1);

				int maxNbDisplayedClipArt = std::min(pg_nb_ClipArt[pg_current_configuration_rank], 32);
				if (activeFiles == -1) { // if activeClipArt is equal to -1: all clip arts are visible
					maxNbDisplayedClipArt = pg_nb_ClipArt[pg_current_configuration_rank];
				}
				for (int indClipArt = 0; indClipArt < maxNbDisplayedClipArt; indClipArt++) {
					if ((activeFiles == -1) || (activeFiles & (1 << indClipArt))) {
						glMatrixPushEXT(GL_MODELVIEW); {
							glMatrixLoadIdentityEXT(GL_MODELVIEW);
							glTranslatef(pg_ClipArt_Translation_X[pg_current_configuration_rank][indClipArt], pg_ClipArt_Translation_Y[pg_current_configuration_rank][indClipArt], 0);
							glRotatef(pg_ClipArt_Rotation[pg_current_configuration_rank][indClipArt], 0, 0, 1);
							glScalef(pg_ClipArt_Scale[pg_current_configuration_rank][indClipArt], pg_ClipArt_Scale[pg_current_configuration_rank][indClipArt], 1);
							for (int indPath = 0;
								indPath < pg_nb_paths_in_ClipArt[pg_current_configuration_rank][indClipArt];
								indPath++) {
								if (indPath < pg_nb_paths_in_ClipArt[pg_current_configuration_rank][indClipArt]
									&& pg_ClipArt_SubPath[pg_current_configuration_rank][indClipArt][indPath] == true) {
									//printf("config %d active clipart display %d layer %d\n", pg_current_configuration_rank, indClipArt, indPath);
									pg_Display_One_ClipArt(pg_ClipArt_Colors[pg_current_configuration_rank][indClipArt][indPath], indClipArt, indPath);
								}
							}
						}
						glMatrixPopEXT(GL_MODELVIEW);
					}
				}
			} glMatrixPopEXT(GL_PROJECTION);
			glDisable(GL_STENCIL_TEST);
		}
		printOglError(5257);
	}
}

void pg_listAllClipArts(void) {
	printf("Clip Arts:\n");
	for (int indConfiguration = 0; indConfiguration < _NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (int indClipArt = 0; indClipArt < pg_nb_ClipArt[indConfiguration]; indClipArt++) {
			if (ScenarioVarConfigurations[_path_replay_trackNo_1][indConfiguration] && ScenarioVarConfigurations[_path_record_1][indConfiguration]) {
				std::cout << pg_ClipArt_fileNames[indConfiguration][indClipArt] << " (" << pg_nb_paths_in_ClipArt[indConfiguration][indClipArt] << " paths), ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
#endif

//////////////////////////////////////////////////
// RENDERING OF MESSAGE THROUGH ClipArt GPU CHARACTERS
float pg_Translate_ClipArt_Text(int indDisplayText) {
	return float(log(indDisplayText/3.f) * 110);
	// return ((indDisplayText % 370) / 10) * 30.f;
}
void pg_Display_ClipArt_Text(int *ind_Current_DisplayText, int mobile) {
	if ((*ind_Current_DisplayText) >= 0 && (*ind_Current_DisplayText) < NbDisplayTexts
		&& pg_shader_programme[pg_current_configuration_rank][_pg_shader_ClipArt]) {
		glUseProgram(pg_shader_programme[pg_current_configuration_rank][_pg_shader_ClipArt]);
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
			glMatrixOrthoEXT(GL_PROJECTION, 0, window_width, window_height, 0, -1, 1);
			std::string displayed_text("");
#ifdef ETOILES
			int digit_rank = (*ind_Current_DisplayText) / 10;
			// digits of the number
			if ((*ind_Current_DisplayText) < 130) {
				int digit_value = 0;
				for (int curDigitRank = 0; curDigitRank < 12; curDigitRank++) {
					if (curDigitRank >= digit_rank) {
						digit_value = int(rand_0_1 * 10) % 10;
					}
					else {
						digit_value = 0;
					}
					displayed_text.push_back(char('0'+digit_value));
					if (curDigitRank == 0) {
						displayed_text += std::string(".");
					}
				}
				// exponent of the number
				displayed_text += std::string("e-");
				std::string exponent("");
				int exponent_value;
				if (digit_rank < 12) {
					exponent_value = 6;
				}
				else {
					int local_rank = (*ind_Current_DisplayText) % 10;
					exponent_value = int(rand_0_1 * 10 + 10 * local_rank) % 100;
				}
				std::stringstream ss;
				ss << std::setw(2) << std::setfill('0') << exponent_value;
				exponent = ss.str();
				displayed_text += exponent;
			}
			else {
				displayed_text = string("               0.");
			}
			std::cout << "text " << displayed_text << " rank " << (*ind_Current_DisplayText) << std::endl;
#else
			displayed_text = DisplayTextList[(*ind_Current_DisplayText)];
#endif
				
			// decomposition of the string into characters and svg index lookup
			for (unsigned int indChar = 0; indChar < displayed_text.size(); indChar++) {
				char curChar = displayed_text.at(indChar);
				int indClipArt = -1;
#ifdef ETOILES
				/* TEASEER ETOiLES */
				if (curChar >= '0' && curChar <= '9') {
					indClipArt = curChar - '0';
				}
				else {
					switch (curChar) {
					case '.':  indClipArt = 10; break;
					case ',':  indClipArt = 11; break;
					case '+':  indClipArt = 12; break;
					case '-':  indClipArt = 13; break;
					case 'e':  indClipArt = 14; break;
					}
				}
#else
				if (curChar >= 'A' && curChar <= 'Z') {
					indClipArt = curChar - 'A';
				}
				else {
					switch (curChar) {
					case ' ':  indClipArt = 26; break;
					case '\'':  indClipArt = 27; break;
					}
				}
#endif
				if (indClipArt >= 0 && indClipArt < pg_nb_ClipArt[pg_current_configuration_rank]) {
					//printf("active clipart display %d\n", indClipArt);
					glMatrixPushEXT(GL_MODELVIEW); {
						glMatrixLoadIdentityEXT(GL_MODELVIEW);
						float y_transl = 0.f;
						if (mobile) {
							y_transl = pg_Translate_ClipArt_Text((*ind_Current_DisplayText));
						}
						else {
							y_transl = pg_ClipArt_Translation_Y[pg_current_configuration_rank][indClipArt];
						}
#ifdef ETOILES
						glTranslatef(100.f + 100.f * indChar, y_transl, 0);
#else
						if (mobile == 2 && indChar < unsigned int(DisplayText_maxLen)) {
							DisplayText_rand_translX[indChar] += (rand_0_1 - 0.5f) * (1.f + float(current_scene_percent) * 10.f);
							DisplayText_rand_translY[indChar] += (rand_0_1 - 0.5f) * (1.f + float(current_scene_percent) * 10.f) + rand_0_1 * 0.2f * (1.f + float(current_scene_percent) * 10.f);
						}
						glTranslatef(pg_ClipArt_Translation_X[pg_current_configuration_rank][indClipArt] * 1.5f 
							+ pg_ClipArt_Translation_X[pg_current_configuration_rank][indClipArt] * indChar + DisplayText_rand_translX[indChar], 
							y_transl + DisplayText_rand_translY[indChar], 0);
#endif
						//glRotatef(pg_ClipArt_Rotation[pg_current_configuration_rank][indClipArt], 0, 0, 1);
						//glScalef(pg_ClipArt_Scale[pg_current_configuration_rank][indClipArt], pg_ClipArt_Scale[pg_current_configuration_rank][indClipArt], 1);
						for (int indPath = 0;
							indPath < pg_nb_paths_in_ClipArt[pg_current_configuration_rank][indClipArt];
							indPath++) {
							if (indPath < pg_nb_paths_in_ClipArt[pg_current_configuration_rank][indClipArt]
								&& pg_ClipArt_SubPath[pg_current_configuration_rank][indClipArt][indPath] == true) {
								//std::cout << "pg_Display_One_ClipArt COLOR " << pg_ClipArt_Colors[pg_current_configuration_rank][indClipArt][indPath] 
								//	<< " ind PATH " << indPath << " ind CLIPART " << indClipArt << std::endl;
#ifndef ETOILES
								float scale = pg_ClipArt_Scale[pg_current_configuration_rank][indClipArt];
								glScalef(scale, scale, scale);
#endif
								pg_Display_One_ClipArt(pg_ClipArt_Colors[pg_current_configuration_rank][indClipArt][indPath], indClipArt, indPath);
							}
						}
					}
					glMatrixPopEXT(GL_MODELVIEW);
				}
			}
		} glMatrixPopEXT(GL_PROJECTION);
		glDisable(GL_STENCIL_TEST);
	}
#ifdef ETOILES
	if (pg_FrameNo % 3 == 0) {
		(*ind_Current_DisplayText) = int(min(float((*ind_Current_DisplayText) + 1), float(NbDisplayTexts - 1)));
	}
#else
	if (pg_FrameNo % 3 == 0) {
		(*ind_Current_DisplayText) = int(min(float((*ind_Current_DisplayText) + 1), float(NbDisplayTexts - 1)));
	}
#endif

	printOglError(5257);
}

//////////////////////////////////////////////////
// LOAD ALL CLIPARTS
void pg_loadAllClipArts(void) {
	for (int indConfiguration = 0; indConfiguration < _NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		if (ScenarioVarConfigurations[_activeClipArts][indConfiguration]) {
			const char* ClipArt_programName = "nvpr_ClipArt";
			//std::cout << "Loading " << pg_nb_ClipArt[indConfiguration] << " ClipArt\n";
			initializeNVPathRender(ClipArt_programName);
			if (has_NV_path_rendering) {
				ClipArt_path_baseID[indConfiguration] = glGenPathsNV(pg_nb_tot_SvgGpu_paths[indConfiguration]);
				ClipArt_file_baseID[indConfiguration] = new GLuint[pg_nb_ClipArt[indConfiguration]];
				ClipArt_path_fill_color[indConfiguration] = new unsigned int* [pg_nb_ClipArt[indConfiguration]];
				int nb_tot_paths = 0;
				for (int indClipArtFile = 0; indClipArtFile < pg_nb_ClipArt[indConfiguration]; indClipArtFile++) {
					std::cout << "Data/" + project_name + "-data/ClipArts/" + pg_ClipArt_fileNames[indConfiguration][indClipArtFile] << " (" << pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile] << " paths), ";
					ClipArt_path_fill_color[indConfiguration][indClipArtFile] = new unsigned int[pg_nb_tot_SvgGpu_paths[indConfiguration]];
					ClipArt_file_baseID[indConfiguration][indClipArtFile] = ClipArt_path_baseID[indConfiguration] + nb_tot_paths;
					LoadClipArtPathsToGPU("Data/" + project_name + "-data/ClipArts/" + pg_ClipArt_fileNames[indConfiguration][indClipArtFile],
						pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile], indClipArtFile, indConfiguration);
					nb_tot_paths += pg_nb_paths_in_ClipArt[indConfiguration][indClipArtFile];
				}
			}
			else {
				sprintf(ErrorStr, "Error: required NV_path_rendering OpenGL extension is not present\n"); ReportError(ErrorStr); throw 23771;
			}
		}
	}
}