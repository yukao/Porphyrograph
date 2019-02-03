/* SVG_GPU.cpp - draw paths */

//#include <string.h>
//#include <math.h>
//
//#include <GL/glew.h>
//
//#include <cstdio>
//#include <cstring>
//#include <fstream>
//#include <iostream>
//#include <string>
//#include <iomanip>
//#include <algorithm>    // std::min

#include "pg-all_include.h"

int has_NV_path_rendering = 0;

GLuint SVG_path_baseID = 0;
GLint *SVG_path_fill_color = NULL;

bool SVG_stroke_onOff = false;
bool SVG_fill_onOff = true;
float SVG_fill_color_shift = 0.f;
float SVG_fill_color_brightness = 1.f;
float SVG_stroke_width = 20.f;
int SVG_fill_color_swap = 0;
int SVG_stroke_color_swap = 0;

/*
#ifndef __APPLE__
#ifndef __GLEW_H__
PFNGLGENPATHSNVPROC FUNC(glGenPathsNV) = NULL;
PFNGLDELETEPATHSNVPROC FUNC(glDeletePathsNV) = NULL;
PFNGLISPATHNVPROC FUNC(glIsPathNV) = NULL;
PFNGLPATHCOMMANDSNVPROC FUNC(glPathCommandsNV) = NULL;
PFNGLPATHCOORDSNVPROC FUNC(glPathCoordsNV) = NULL;
PFNGLPATHSUBCOMMANDSNVPROC FUNC(glPathSubCommandsNV) = NULL;
PFNGLPATHSUBCOORDSNVPROC FUNC(glPathSubCoordsNV) = NULL;
PFNGLPATHSTRINGNVPROC FUNC(glPathStringNV) = NULL;
PFNGLPATHGLYPHSNVPROC FUNC(glPathGlyphsNV) = NULL;
PFNGLPATHGLYPHRANGENVPROC FUNC(glPathGlyphRangeNV) = NULL;
PFNGLWEIGHTPATHSNVPROC FUNC(glWeightPathsNV) = NULL;
PFNGLCOPYPATHNVPROC FUNC(glCopyPathNV) = NULL;
PFNGLINTERPOLATEPATHSNVPROC FUNC(glInterpolatePathsNV) = NULL;
PFNGLTRANSFORMPATHNVPROC FUNC(glTransformPathNV) = NULL;
PFNGLPATHPARAMETERIVNVPROC FUNC(glPathParameterivNV) = NULL;
PFNGLPATHPARAMETERINVPROC FUNC(glPathParameteriNV) = NULL;
PFNGLPATHPARAMETERFVNVPROC FUNC(glPathParameterfvNV) = NULL;
PFNGLPATHPARAMETERFNVPROC FUNC(glPathParameterfNV) = NULL;
PFNGLPATHDASHARRAYNVPROC FUNC(glPathDashArrayNV) = NULL;
PFNGLSTENCILFILLPATHNVPROC FUNC(glStencilFillPathNV) = NULL;
PFNGLPATHSTENCILDEPTHOFFSETNVPROC FUNC(glPathStencilDepthOffsetNV) = NULL;
PFNGLSTENCILSTROKEPATHNVPROC FUNC(glStencilStrokePathNV) = NULL;
PFNGLSTENCILFILLPATHINSTANCEDNVPROC FUNC(glStencilFillPathInstancedNV) = NULL;
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC FUNC(glStencilStrokePathInstancedNV) = NULL;
PFNGLPATHCOVERDEPTHFUNCNVPROC FUNC(glPathCoverDepthFuncNV) = NULL;
PFNGLPATHCOLORGENNVPROC FUNC(glPathColorGenNV) = NULL;
PFNGLPATHTEXGENNVPROC FUNC(glPathTexGenNV) = NULL;
PFNGLPATHFOGGENNVPROC FUNC(glPathFogGenNV) = NULL;
PFNGLCOVERFILLPATHNVPROC FUNC(glCoverFillPathNV) = NULL;
PFNGLCOVERSTROKEPATHNVPROC FUNC(glCoverStrokePathNV) = NULL;
PFNGLCOVERFILLPATHINSTANCEDNVPROC FUNC(glCoverFillPathInstancedNV) = NULL;
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC FUNC(glCoverStrokePathInstancedNV) = NULL;
PFNGLGETPATHPARAMETERIVNVPROC FUNC(glGetPathParameterivNV) = NULL;
PFNGLGETPATHPARAMETERFVNVPROC FUNC(glGetPathParameterfvNV) = NULL;
PFNGLGETPATHCOMMANDSNVPROC FUNC(glGetPathCommandsNV) = NULL;
PFNGLGETPATHCOORDSNVPROC FUNC(glGetPathCoordsNV) = NULL;
PFNGLGETPATHDASHARRAYNVPROC FUNC(glGetPathDashArrayNV) = NULL;
PFNGLGETPATHMETRICSNVPROC FUNC(glGetPathMetricsNV) = NULL;
PFNGLGETPATHMETRICRANGENVPROC FUNC(glGetPathMetricRangeNV) = NULL;
PFNGLGETPATHSPACINGNVPROC FUNC(glGetPathSpacingNV) = NULL;
PFNGLGETPATHCOLORGENIVNVPROC FUNC(glGetPathColorGenivNV) = NULL;
PFNGLGETPATHCOLORGENFVNVPROC FUNC(glGetPathColorGenfvNV) = NULL;
PFNGLGETPATHTEXGENIVNVPROC FUNC(glGetPathTexGenivNV) = NULL;
PFNGLGETPATHTEXGENFVNVPROC FUNC(glGetPathTexGenfvNV) = NULL;
PFNGLISPOINTINFILLPATHNVPROC FUNC(glIsPointInFillPathNV) = NULL;
PFNGLISPOINTINSTROKEPATHNVPROC FUNC(glIsPointInStrokePathNV) = NULL;
PFNGLGETPATHLENGTHNVPROC FUNC(glGetPathLengthNV) = NULL;
PFNGLPOINTALONGPATHNVPROC FUNC(glPointAlongPathNV) = NULL;
PFNGLPATHSTENCILFUNCNVPROC FUNC(glPathStencilFuncNV) = NULL;

#endif // __GLEW_H__
#endif // __APPLE__

#if defined(_WIN32)
# define GET_PROC_ADDRESS(name)  wglGetProcAddress(#name)
#elif defined(__APPLE__)
# define GET_PROC_ADDRESS(name)  //nothing
#elif defined(vxworks)
# define GET_PROC_ADDRESS(name)  rglGetProcAddress(#name)
#else
// Assume using GLX 
# define GET_PROC_ADDRESS(name)  glXGetProcAddressARB((const GLubyte *) #name)
#endif

#ifdef __APPLE__
#define LOAD_PROC(type, name)  //nothing
#else
#define LOAD_PROC(type, name) \
  FUNC(name) = (type) GET_PROC_ADDRESS(name); \
  if (!FUNC(name)) { \
    fprintf(stderr, "%s: failed to GetProcAddress for %s\n", programName, #name); \
    exit(1); \
  }
#endif
*/

void initializeNVPathRender(const char *programName) {
	has_NV_path_rendering = glutExtensionSupported("GL_NV_path_rendering");
	if (!has_NV_path_rendering) {
		sprintf(ErrorStr, "Error: required NV_path_rendering OpenGL extension is not present\n"); ReportError(ErrorStr); throw 100;
	}
	/*
	if (has_NV_path_rendering) {
		LOAD_PROC(PFNGLGENPATHSNVPROC, glGenPathsNV);
		LOAD_PROC(PFNGLDELETEPATHSNVPROC, glDeletePathsNV);
		LOAD_PROC(PFNGLISPATHNVPROC, glIsPathNV);
		LOAD_PROC(PFNGLPATHCOMMANDSNVPROC, glPathCommandsNV);
		LOAD_PROC(PFNGLPATHCOORDSNVPROC, glPathCoordsNV);
		LOAD_PROC(PFNGLPATHSUBCOMMANDSNVPROC, glPathSubCommandsNV);
		LOAD_PROC(PFNGLPATHSUBCOORDSNVPROC, glPathSubCoordsNV);
		LOAD_PROC(PFNGLPATHSTRINGNVPROC, glPathStringNV);
		LOAD_PROC(PFNGLPATHGLYPHSNVPROC, glPathGlyphsNV);
		LOAD_PROC(PFNGLPATHGLYPHRANGENVPROC, glPathGlyphRangeNV);
		LOAD_PROC(PFNGLWEIGHTPATHSNVPROC, glWeightPathsNV);
		LOAD_PROC(PFNGLCOPYPATHNVPROC, glCopyPathNV);
		LOAD_PROC(PFNGLINTERPOLATEPATHSNVPROC, glInterpolatePathsNV);
		LOAD_PROC(PFNGLTRANSFORMPATHNVPROC, glTransformPathNV);
		LOAD_PROC(PFNGLPATHPARAMETERIVNVPROC, glPathParameterivNV);
		LOAD_PROC(PFNGLPATHPARAMETERINVPROC, glPathParameteriNV);
		LOAD_PROC(PFNGLPATHPARAMETERFVNVPROC, glPathParameterfvNV);
		LOAD_PROC(PFNGLPATHPARAMETERFNVPROC, glPathParameterfNV);
		LOAD_PROC(PFNGLPATHDASHARRAYNVPROC, glPathDashArrayNV);
		LOAD_PROC(PFNGLSTENCILFILLPATHNVPROC, glStencilFillPathNV);
		LOAD_PROC(PFNGLSTENCILSTROKEPATHNVPROC, glStencilStrokePathNV);
		LOAD_PROC(PFNGLSTENCILFILLPATHINSTANCEDNVPROC, glStencilFillPathInstancedNV);
		LOAD_PROC(PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC, glStencilStrokePathInstancedNV);
		LOAD_PROC(PFNGLPATHCOLORGENNVPROC, glPathColorGenNV);
		LOAD_PROC(PFNGLPATHTEXGENNVPROC, glPathTexGenNV);
		LOAD_PROC(PFNGLPATHFOGGENNVPROC, glPathFogGenNV);
		LOAD_PROC(PFNGLCOVERFILLPATHNVPROC, glCoverFillPathNV);
		LOAD_PROC(PFNGLCOVERSTROKEPATHNVPROC, glCoverStrokePathNV);
		LOAD_PROC(PFNGLCOVERFILLPATHINSTANCEDNVPROC, glCoverFillPathInstancedNV);
		LOAD_PROC(PFNGLCOVERSTROKEPATHINSTANCEDNVPROC, glCoverStrokePathInstancedNV);
		LOAD_PROC(PFNGLGETPATHPARAMETERIVNVPROC, glGetPathParameterivNV);
		LOAD_PROC(PFNGLGETPATHPARAMETERFVNVPROC, glGetPathParameterfvNV);
		LOAD_PROC(PFNGLGETPATHCOMMANDSNVPROC, glGetPathCommandsNV);
		LOAD_PROC(PFNGLGETPATHCOORDSNVPROC, glGetPathCoordsNV);
		LOAD_PROC(PFNGLGETPATHDASHARRAYNVPROC, glGetPathDashArrayNV);
		LOAD_PROC(PFNGLGETPATHMETRICSNVPROC, glGetPathMetricsNV);
		LOAD_PROC(PFNGLGETPATHMETRICRANGENVPROC, glGetPathMetricRangeNV);
		LOAD_PROC(PFNGLGETPATHSPACINGNVPROC, glGetPathSpacingNV);
		LOAD_PROC(PFNGLGETPATHCOLORGENIVNVPROC, glGetPathColorGenivNV);
		LOAD_PROC(PFNGLGETPATHCOLORGENFVNVPROC, glGetPathColorGenfvNV);
		LOAD_PROC(PFNGLGETPATHTEXGENIVNVPROC, glGetPathTexGenivNV);
		LOAD_PROC(PFNGLGETPATHTEXGENFVNVPROC, glGetPathTexGenfvNV);
		LOAD_PROC(PFNGLISPOINTINFILLPATHNVPROC, glIsPointInFillPathNV);
		LOAD_PROC(PFNGLISPOINTINSTROKEPATHNVPROC, glIsPointInStrokePathNV);
		LOAD_PROC(PFNGLGETPATHLENGTHNVPROC, glGetPathLengthNV);
		LOAD_PROC(PFNGLPOINTALONGPATHNVPROC, glPointAlongPathNV);
		LOAD_PROC(PFNGLPATHSTENCILFUNCNVPROC, glPathStencilFuncNV);
		LOAD_PROC(PFNGLPATHSTENCILDEPTHOFFSETNVPROC, glPathStencilDepthOffsetNV);
		LOAD_PROC(PFNGLPATHCOVERDEPTHFUNCNVPROC, glPathCoverDepthFuncNV);
	}
	*/
}


void LoadSVGPathsToGPU(string fileName, int ind_first_gpu_path, int nb_gpu_paths) {
	int pg_SVG_nb_loaded_paths = 0;

	std::cout << "Loading " << fileName << "\n";
	std::ifstream pathFin(fileName);
	if (!pathFin) {
		std::cout << "Error: clip art file [" << fileName << "] not found!\n";
		exit(0);
	}
	std::string line;
	int ind_gpu_path = 0;
	// coiunts the number of non empty lines -> the number of paths
	while (std::getline(pathFin, line))
	{
		if (!line.empty() && (line.find("<path") != std::string::npos))
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
		if (!line.empty() && ind_gpu_path <= nb_gpu_paths) {
			std::size_t found = std::string::npos;
			found = line.find("<path");
			if (found != std::string::npos) {
				pg_SVG_nb_loaded_paths++;
				ind_gpu_path++;
			}
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
					//printf("Fill color of path #%d: %s\n", ind_first_gpu_path + pg_SVG_nb_loaded_paths - 1, lineAux.c_str());
					SVG_path_fill_color[ind_first_gpu_path + pg_SVG_nb_loaded_paths - 1] = std::stoi(lineAux, nullptr, 16);
				}
			}
			// looks for a path
			found = line.find(" d=\"");
			if (found != std::string::npos) {
				// copies the beginning of the path
				line = line.substr(found + strlen(" d=\""));
				// loooks for string ending
				found = line.find("\"");
				if (found != std::string::npos) {
					line = line.substr(0, found);
				}
				else {
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
				//if (pg_SVG_nb_loaded_paths == 1) {
					 //std::cout << "path: [" << line << "]\n";
				//}

				// const char *svg_str = "M-122.304 84.285C-122.304 84.285 -122.203 86.179 -123.027 86.16C-123.851 86.141 -140.305 38.066 -160.833 40.309C-160.833 40.309 -143.05 32.956 -122.304 84.285z";
				size_t svg_len = line.size();
				const char *svg_str = line.c_str();
				glPathStringNV(SVG_path_baseID + ind_first_gpu_path + pg_SVG_nb_loaded_paths - 1, GL_PATH_FORMAT_SVG_NV,
					(GLsizei)svg_len, svg_str);
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

static void OpenGLAnyColor(GLuint color) {
	GLubyte red = (color >> 16) & 0xFF,
		green = (color >> 8) & 0xFF,
		blue = (color >> 0) & 0xFF;

	float redf = red * SVG_fill_color_brightness;
	float greenf = green * SVG_fill_color_brightness;
	float bluef = blue * SVG_fill_color_brightness;

	redf += SVG_fill_color_shift * 255;

	red = GLubyte(std::max(std::min(redf, 255.f), 0.f));
	green = GLubyte(std::max(std::min(greenf, 255.f), 0.f));
	blue = GLubyte(std::max(std::min(bluef, 255.f), 0.f));
	// printf("RGB %d %d %d\n", int(red), int(green), int(blue));

	glColor3ub(red, green, blue);
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

static void pg_Display_One_SVG_ClipArt(pg_ClipArt_Colors_Types color, int path_layer) {
	// Should this path be stroked?
	if (SVG_fill_onOff) {
		switch (color) {
		case ClipArt_nat:
			OpenGLAnyColor(SVG_path_fill_color[(path_layer + SVG_fill_color_swap) % (pg_nb_tot_SvgGpu_paths)]);
			break;
		case ClipArt_white:
			glColor3ub(255, 255, 255);
			break;
		case ClipArt_red:
			glColor3ub(255, 0, 0);
			break;
		case ClipArt_green:
			glColor3ub(0, 255, 0);
			break;
		}
		glStencilFillPathNV(SVG_path_baseID + path_layer, GL_COUNT_UP_NV, 0x1F);
		glCoverFillPathNV(SVG_path_baseID + path_layer, GL_BOUNDING_BOX_NV);
	}
	/*
	// Should this path be stroked?
	if (SVG_stroke_onOff) {
		const GLint reference = 0x1;
		OpenGLBasicColors(SVG_stroke_color_swap);
		glPathParameterfNV(SVG_path_baseID + path_layer, GL_PATH_STROKE_WIDTH_NV,
			SVG_stroke_width);
		glStencilStrokePathNV(SVG_path_baseID + path_layer, reference, 0x1F);
		glCoverStrokePathNV(SVG_path_baseID + path_layer, GL_BOUNDING_BOX_NV);
	}
	*/
}

//////////////////////////////////////////////////
// RENDERING GPU SVG IF SOME LAYERS ARE ACTIVE
void pg_Display_All_SVG_ClipArt(int activeFiles) {
	if (activeFiles != 0) {
		glUseProgram(shader_programme[pg_SvgGpu]);
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
			for (int indClipArt = 0; indClipArt < std::min(pg_nb_ClipArt, 32); indClipArt++) {
				if (activeFiles & (1 << indClipArt)) {
					glMatrixPushEXT(GL_MODELVIEW); {
						glMatrixLoadIdentityEXT(GL_MODELVIEW);
						glTranslatef(pg_ClipArt_Translation_X[indClipArt], pg_ClipArt_Translation_Y[indClipArt], 0);
						glRotatef(pg_ClipArt_Rotation[indClipArt], 0, 0, 1);
						glScalef(pg_ClipArt_Scale[indClipArt], pg_ClipArt_Scale[indClipArt], 1);
						for (int indPath = pg_ind_first_SvgGpu_path_in_ClipArt[indClipArt];
							indPath < pg_ind_first_SvgGpu_path_in_ClipArt[indClipArt] + pg_nb_paths_in_ClipArt[indClipArt];
							indPath++) {
							int indLocalPath = indPath - pg_ind_first_SvgGpu_path_in_ClipArt[indClipArt];
							if (indLocalPath >= 4
								|| (indLocalPath < 4 && pg_ClipArt_SubPath[indClipArt * 4 + indLocalPath] == true)) {
								pg_Display_One_SVG_ClipArt(pg_ClipArt_Colors[indClipArt], indPath);
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
