/*

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
/*
	based on Francesco Caruso GLSLParametricCurves
	https://github.com/fcaruso/GLSLParametricCurve
*/

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <random>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <netdb.h>
#include <unistd.h>
#else // !_WIN32
//#define socklen_t int
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <wspiapi.h>
#endif // _WIN32

#ifdef _WIN32
#include <time.h>
#include <Winbase.h>
#include <Windows.h>

// transparent windows
#include <windowsx.h>
// transparent windows
#include <dwmapi.h>

#ifndef _VISUAL_STUDIO
#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
#define _TIMEVAL_DEFINED
#endif /* !_TIMEVAL_DEFINED */
#endif /* !_VISUAL_STUDIO */
#else // _WIN32
#include <sys/time.h>
#include <pthread.h>
#endif // _WIN32
// \}

#define OPENCV_3

#ifndef OPENCV_3
// opencv include files
#include <opencv/cv.h>
#include <opencv/highgui.h>
#else
// opencv include files
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#endif
using namespace cv;

using std::string;

#ifndef _TIMEVAL_DEFINED /* also in winsock[2].h */
#define _TIMEVAL_DEFINED
struct timeval {
	unsigned long int tv_usec;
	unsigned long int tv_sec;
};
#endif /* !_TIMEVAL_DEFINED */


// #define GLEW_STATIC
#include <list>
#include <vector>
#include <map>
using std::list;
using std::vector;
using std::map;
using std::string;
using std::ios;
using std::cerr;
using std::ifstream;
#include <fstream>
#include <GL/glew.h>
#include <GL/wglew.h>

#include "../share/freeglut-wacom/include/GL/freeglut.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

#include "BezierCurveMain.h"
#include "LoadShaders.h"
#include "pg-texture.h"


mat4	matMV;
mat4	matP;

int pg_FrameNo = 0;
int LastFrame = 0;
double InitialRealTime;
double CurrentClockTime;
double LastFrameTime = 0;
int FramePerSecond = 0;
int Step = 0;

enum eVertexArrayObject
{
	pg_VAOCurve,
	pg_VAOQuad,
	pg_VAOCount
};

enum eVertexBufferObject
{
	pg_VBOQuad,
	pg_VBOQuadTexCoords,
	pg_EABQuad,
	pg_VBOCurve,
	//pg_VBOCurveTexCoords,
	pg_EABCurve,
	pg_VBOCount
};

enum eShader
{
	pg_BezierCurveUpdateShader,
	pg_BezierCurveShader,
	pg_BezierCompositionShader,
	pg_BezierFinalShader,
	pg_ShaderCount
};

GLuint pg_vaoID[pg_VAOCount];
GLuint pg_vboID[pg_VBOCount];
GLuint pg_shaderID[pg_ShaderCount];

GLfloat *pg_ClipArt_points;
//GLfloat *pg_ClipArt_texCoords;
unsigned int *pg_ClipArt_indices;

// geometrical data
#define PG_NBFACES_QUAD 2
float quadDraw_points[3 * 4] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float quadDraw_texCoords[2 * 4] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int quadDraw_indices[3 * PG_NBFACES_QUAD] = { 0, 1, 2, 1, 0, 3 };

int leftWindowWidth = PG_WIDTH;
int window_height = PG_HEIGHT;

// shader uniform variables
float echo = 0.959f;
float echoNeg = 0.969f;
float noiseScale = 0.5f;
float damping = 0.f;

double RealTime(void);

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
GLuint drawBuffers[16] = {
	GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
};

GLuint FBO_BezierUpdate[PG_NB_MEMORIZED_PASS] = { 0,0 };
GLuint FBO_BezierUpdate_texID[PG_NB_MEMORIZED_PASS] = { 0,0 };
GLuint FBO_BezierCurves = 0;
GLuint FBO_BezierCurves_texID = 0;
GLuint FBO_BezierEcho[PG_NB_MEMORIZED_PASS] = { 0,0 };
GLuint FBO_BezierEcho_texID[PG_NB_MEMORIZED_PASS] = { 0,0 };

GLuint pg_bezierCurve_Pos_Texture_texID = 0;
GLfloat *pg_bezierCurve_Pos_Texture = NULL;

cv::Mat comet_image;
GLuint comet_texture_2D_texID = 0;


//////////////////////////////////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
// BEZIER CURVES INITIALIZATION

void initVectorClipArtData()
{
	// the control point position contain column and row of the control point coordinates
	// inside the texture of initial positions so that the coordinates contained in this
	// texture can be retrieved in the vertex shader
	pg_ClipArt_points = new float[PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 3];
	//pg_ClipArt_texCoords = new float[PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 2];
	pg_ClipArt_indices = new unsigned int[PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1)];

	// the used width is a multiple of (PG_CURVE_DEGREE + 1) so that each set of control
	// point coordinates is on a single line
	int width_used = PG_WIDTH - PG_WIDTH % (PG_CURVE_DEGREE + 1);

	for (int indCurve = 0; indCurve < PG_NB_VECTOR_CLIPART; indCurve++) {
		int ind_index = indCurve * (PG_CURVE_DEGREE + 1);
		int ind_point = ind_index * 3;
		// col 
		pg_ClipArt_points[ind_point + 0] = float((indCurve * (PG_CURVE_DEGREE + 1) + 0) % width_used);
		// row
		pg_ClipArt_points[ind_point + 1] = float((indCurve * (PG_CURVE_DEGREE + 1) + 0) / width_used);
		pg_ClipArt_points[ind_point + 2] = 0.f;

		//// u 
		//pg_ClipArt_texCoords[ind_point + 0] = 0.f;
		//// v
		//pg_ClipArt_texCoords[ind_point + 1] = 0.f;

		pg_ClipArt_indices[ind_index] = ind_index;

		ind_point += 3;
		ind_index++;
		for (int indControlPoint = 0; indControlPoint < PG_CURVE_DEGREE; indControlPoint++) {
			// col 
			pg_ClipArt_points[ind_point + 0] = float((indCurve * (PG_CURVE_DEGREE + 1) + indControlPoint + 1) % width_used);
			// row
			pg_ClipArt_points[ind_point + 1] = float((indCurve * (PG_CURVE_DEGREE + 1) + indControlPoint + 1) / width_used);
			pg_ClipArt_points[ind_point + 2] = 0.f;

			//// u 
			//if (indControlPoint == 0) {
			//	pg_ClipArt_texCoords[ind_point + 0] = 0.f;
			//}
			//else {
			//	pg_ClipArt_texCoords[ind_point + 0] = 1.f;
			//}
			//// v
			//pg_ClipArt_texCoords[ind_point + 1] = 0.f;

			pg_ClipArt_indices[ind_index] = ind_index;

			ind_point += 3;
			ind_index++;
		}
	}
}

/////////////////////////////////////////////////
// QUAD INITIALIZATION

void initQuadCurveData()
{
	// point positions and texture coordinates
	quadDraw_points[1] = (float)window_height;
	quadDraw_points[3] = (float)leftWindowWidth;
	quadDraw_points[9] = (float)leftWindowWidth;
	quadDraw_points[10] = (float)window_height;

	quadDraw_texCoords[1] = (float)window_height;
	quadDraw_texCoords[2] = (float)leftWindowWidth;
	quadDraw_texCoords[6] = (float)leftWindowWidth;
	quadDraw_texCoords[7] = (float)window_height;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// SHADER COMPILING
//////////////////////////////////////////////////////////////////////////////////////////////

void initShaders()
{
	// shaders for rendering the points and the control polyline

	ShaderInfo curveUpdate_shader_info[] =
	{
		{ GL_VERTEX_SHADER, "./ProjectsBefore18/curves/shaders/bezier_quad.vert" },
		{ GL_FRAGMENT_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve_update.frag" },
		{ GL_NONE, NULL }
	};

	pg_shaderID[pg_BezierCurveUpdateShader] = LoadShaders(curveUpdate_shader_info);
	printOglError(30);

	// shaders for tessellating and rendering the Bezier curve

	ShaderInfo curves_shader_info[] =
	{
		{ GL_VERTEX_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve.vert" },
		{ GL_TESS_CONTROL_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve.tess" },
		{ GL_TESS_EVALUATION_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve.eval" },
		{ GL_GEOMETRY_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve.geom" },
		{ GL_FRAGMENT_SHADER, "./ProjectsBefore18/curves/shaders/bezier_curve.frag" },
		{ GL_NONE, NULL }
	};

	pg_shaderID[pg_BezierCurveShader] = LoadShaders(curves_shader_info);
	printOglError(31);

	// shaders for compositing and managing echo

	ShaderInfo composition_shader_info[] =
	{
		{ GL_VERTEX_SHADER, "./ProjectsBefore18/curves/shaders/bezier_quad.vert" },
		{ GL_FRAGMENT_SHADER, "./ProjectsBefore18/curves/shaders/bezier_composition.frag" },
		{ GL_NONE, NULL }
	};

	pg_shaderID[pg_BezierCompositionShader] = LoadShaders(composition_shader_info);
	printOglError(30);

	// shaders for rendering the composited FBO

	ShaderInfo final_shader_info[] =
	{
		{ GL_VERTEX_SHADER, "./ProjectsBefore18/curves/shaders/bezier_quad.vert" },
		{ GL_FRAGMENT_SHADER, "./ProjectsBefore18/curves/shaders/bezier_final.frag" },
		{ GL_NONE, NULL }
	};

	pg_shaderID[pg_BezierFinalShader] = LoadShaders(final_shader_info);
	printOglError(30);

}

//////////////////////////////////////////////////////////////////////////////////////////////
// GEOMTRICAL DATA GPU TRANSFER
//////////////////////////////////////////////////////////////////////////////////////////////

void initVAOs()
{
	/////////////////////////////////////////////////////////////////////
	// BEZIER CURVES TO BE TESSELATED
	// vertex buffer objects and vertex array
	glBindVertexArray(pg_vaoID[pg_VAOCurve]);
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOCurve]);
	glBufferData(GL_ARRAY_BUFFER, PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 3 * sizeof(float), pg_ClipArt_points, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOCurveTexCoords]);
	//glBufferData(GL_ARRAY_BUFFER, PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 2 * sizeof(float), pg_ClipArt_texCoords, GL_STATIC_DRAW);

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOCurve]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	//glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOCurveTexCoords]);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	//glEnableVertexAttribArray(1); // don't forget this!

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABCurve]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * sizeof(unsigned int),
		pg_ClipArt_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printOglError(32);

	/////////////////////////////////////////////////////////////////////
	// QUAD FOR DRAWING AND COMPOSITION
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);

	// vertex buffer objects and vertex array
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOQuad]);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 4 * sizeof(float),
		quadDraw_points,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOQuadTexCoords]);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 4 * sizeof(float),
		quadDraw_texCoords,
		GL_STATIC_DRAW);

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOQuad]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOQuadTexCoords]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * PG_NBFACES_QUAD * sizeof(unsigned int), 
				quadDraw_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printOglError(22);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// TIME MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////

static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	return (realtime - InitialRealTime);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// OPENGL INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////
void initGL()
{	
	glClearColor(0.f, 0.f, 0.f, 1.f);

	glPointSize(3.f);
	glEnable (GL_LINE_SMOOTH);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glDisable(GL_BLEND);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GLUT CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////
void window_resize(int w, int h)
{
	printf("Reshape %dx%d\n", w, h);
	float ratio = (float)h / w;
	glViewport(0,0, w,h);
	matP = glm::ortho(0.f ,(float)PG_WIDTH, 0.f, (float)PG_WIDTH * ratio, -1.f, 1.f );
	matMV = glm::mat4(1.f);
}
void window_key(unsigned char c, int w, int h )
{
	switch (c)
	{
	case 'e':
		if (echo < 0.9f)
			echo -= 0.1f;
		else if (echo < 0.99f)
			echo -= 0.01f;
		else
			echo -= 0.001f;
		printf("echo %.3f\n", echo);
		break;
	case 'E':
		if (echo < 0.9f)
			echo += 0.1f;
		else if (echo < 0.99f)
			echo += 0.01f;
		else
			echo += 0.001f;
		printf("echo %.3f\n", echo);
		break;
	case 'n':
		if (echoNeg < 0.9f)
			echoNeg -= 0.1f;
		else if (echoNeg < 0.99f)
			echoNeg -= 0.01f;
		else
			echoNeg -= 0.001f;
		printf("echoNeg %.3f\n", echoNeg);
		break;
	case 'N':
		if (echoNeg < 0.9f)
			echoNeg += 0.1f;
		else if (echoNeg < 0.99f)
			echoNeg += 0.01f;
		else
			echoNeg += 0.001f;
		printf("echoNeg %.3f\n", echoNeg);
		break;
	case 's':
		noiseScale -= 0.1f;
		printf("noiseScale %.2f\n", noiseScale);
		break;
	case 'S':
		noiseScale += 0.1f;
		printf("noiseScale %.2f\n", noiseScale);
		break;
	case 'd':
		damping -= .001f;
		printf("damping %.4f\n", damping);
		break;
	case 'D':
		damping += .001f;
		printf("damping %.4f\n", damping);
		break;
	case 27:case'q':
		glutExit();
		break;
	}
}
void window_idle_browse(int step) {
	// -------------------- idle function recall ------------------------- //
	glutTimerFunc((int)1,
		&window_idle_browse, Step);

	Step = step;

	pg_FrameNo++;
	CurrentClockTime = RealTime();

	// FPS computation
	if (pg_FrameNo % 100 == 0 && CurrentClockTime != LastFrameTime) {
		FramePerSecond = (int)((pg_FrameNo - LastFrame) / (CurrentClockTime - LastFrameTime));
		LastFrame = pg_FrameNo;
		LastFrameTime = CurrentClockTime;
		printf("%d FPS\n", FramePerSecond);
	}

	// -------------------- window redisplay ------------------------- //
	glutPostRedisplay();

	Step++;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// MULTIPASS RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////
// PASS #1: UPDATE BEZIER CURVES

void updateBezierCurves()
{
	// ping pong output and input FBO bindings
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_BezierUpdate[((pg_FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);
	// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shaderID[pg_BezierCurveUpdateShader]);

	GLuint locMatP = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "vp_projMatrix");
	GLuint locMatMV = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "vp_modelViewMatrix");
	glUniformMatrix4fv(locMatP, 1, GL_FALSE, glm::value_ptr(matP));
	glUniformMatrix4fv(locMatMV, 1, GL_FALSE, glm::value_ptr(matMV));

	// flashes the initial positions when not null
	GLuint uniform_CurveUpdate_flashPositionTexture = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "flashPositionTexture");
	glUniform1i(uniform_CurveUpdate_flashPositionTexture, int(pg_FrameNo < 10));
	// texture noise scale control
	GLuint uniform_noiseScale = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "noiseScale");
	glUniform1f(uniform_noiseScale, noiseScale);
	// curve damping
	GLuint uniform_damping = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "damping");
	glUniform1f(uniform_damping, damping);

	// texture unit location
	// position at previous pass
	GLuint uniform_CurveUpdate_texture_fs_decal = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "fs_posSpeedAtPrecedingFrame");
	glUniform1i(uniform_CurveUpdate_texture_fs_decal, 0);
	// initial positions stored in texture
	GLuint uniform_CurveUpdate_texture_fs_controlPoint_positions = glGetUniformLocation(pg_shaderID[pg_BezierCurveUpdateShader], "fs_controlPoint_positions");
	glUniform1i(uniform_CurveUpdate_texture_fs_controlPoint_positions, 1);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong BG track step n (FBO attachment 1)
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_BezierUpdate_texID[(pg_FrameNo % PG_NB_MEMORIZED_PASS)]);

	// texture of initial curve control point positions
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_bezierCurve_Pos_Texture_texID);

	////////////////////////////////////////
	// binds geometry and displays it    
	// quad vao
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	// draw triangles from the currently bound VAO with current in-use shader
	// glDrawArrays(GL_TRIANGLES, 0, 3 * PG_NBFACES_QUAD);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_NBFACES_QUAD,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	// unbinds VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	printOglError(524);
}

/////////////////////////////////////
// PASS #2: DRAW BEZIER CURVES

void drawBezierCurves()
{
	// draws the Bezier curve
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_BezierCurves);

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shaderID[pg_BezierCurveShader]);

	GLuint locMatP = glGetUniformLocation(pg_shaderID[pg_BezierCurveShader], "matProjection");
	GLuint locMatMV = glGetUniformLocation(pg_shaderID[pg_BezierCurveShader], "matModelView");
	glUniformMatrix4fv(locMatP, 1, GL_FALSE, glm::value_ptr(matP));
	glUniformMatrix4fv(locMatMV, 1, GL_FALSE, glm::value_ptr(matMV));

	// texture unit location
	// bezier curve update
	GLuint uniform_Curve_texture_vp_decal = glGetUniformLocation(pg_shaderID[pg_BezierCurveShader], "vp_decal");
	glUniform1i(uniform_Curve_texture_vp_decal, 0);
	// comet texture
	GLuint uniform_Comet_texture_fs_decal = glGetUniformLocation(pg_shaderID[pg_BezierCurveShader], "fs_comet");
	glUniform1i(uniform_Comet_texture_fs_decal, 1);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// 2-cycle ping-pong bezier curve update
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_BezierUpdate_texID[((pg_FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);
	
	// comet texture
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, comet_texture_2D_texID);

	////////////////////////////////////////
	// binds geometry and displays it    
	// vertex buffer for a patch, made of 4 points: 4 x 3 floats
	glBindVertexArray(pg_vaoID[pg_VAOCurve]);

	glPatchParameteri(GL_PATCH_VERTICES, (PG_CURVE_DEGREE + 1));  // number of vertices in each patch
	// glDrawArrays(GL_PATCHES, 0, PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1));
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABCurve]);
	// Draw the triangles !
	glDrawElements(
		GL_PATCHES,      // mode
		PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	printOglError(5256);

	// unbinds VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

/////////////////////////////////////
// PASS #3: COMPOSITION: ECHO MANAGEMENT

void pg_CompositionRendering()
{
	// ping pong output and input FBO bindings
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_BezierEcho[((pg_FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);
	// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shaderID[pg_BezierCompositionShader]);

	GLuint locMatP = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "vp_projMatrix");
	GLuint locMatMV = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "vp_modelViewMatrix");
	glUniformMatrix4fv(locMatP, 1, GL_FALSE, glm::value_ptr(matP));
	glUniformMatrix4fv(locMatMV, 1, GL_FALSE, glm::value_ptr(matMV));

	// echo and echoNeg control
	GLuint uniform_echo = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "echo");
	GLuint uniform_echoNeg = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "echoNeg");
	glUniform1f(uniform_echo, echo);
	glUniform1f(uniform_echoNeg, echoNeg);

	// texture unit location
	// rendering at previous pass
	GLuint uniform_CurveUpdate_texture_fs_decal = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "fs_previousFrame");
	glUniform1i(uniform_CurveUpdate_texture_fs_decal, 0);

	// rendering at previous pass
	GLuint uniform_Echo_texture_fs_decal = glGetUniformLocation(pg_shaderID[pg_BezierCompositionShader], "fs_currentFrame");
	glUniform1i(uniform_Echo_texture_fs_decal, 1);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// echo FBO previous pass
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_BezierEcho_texID[(pg_FrameNo % PG_NB_MEMORIZED_PASS)]);

	// rendering at preceding pass
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_BezierCurves_texID);
	// glBindTexture(GL_TEXTURE_RECTANGLE, pg_bezierCurve_Pos_Texture_texID);

	////////////////////////////////////////
	// binds geometry and displays it    
	// quad vao
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	// draw triangles from the currently bound VAO with current in-use shader
	// glDrawArrays(GL_TRIANGLES, 0, 3 * PG_NBFACES_QUAD);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_NBFACES_QUAD,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	// unbind output FBO 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// unbinds VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	printOglError(524);
}

/////////////////////////////////////
// PASS #4: FINAL RENDERING

void pg_FinalRendering() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	////////////////////////////////////////
	// activate shaders and sets uniform variable values    
	glUseProgram(pg_shaderID[pg_BezierFinalShader]);

	GLuint locMatP = glGetUniformLocation(pg_shaderID[pg_BezierFinalShader], "vp_projMatrix");
	GLuint locMatMV = glGetUniformLocation(pg_shaderID[pg_BezierFinalShader], "vp_modelViewMatrix");
	glUniformMatrix4fv(locMatP, 1, GL_FALSE, glm::value_ptr(matP));
	glUniformMatrix4fv(locMatMV, 1, GL_FALSE, glm::value_ptr(matMV));

	// texture unit location
	// rendering at previous pass
	GLuint uniform_CurveUpdate_texture_fs_decal = glGetUniformLocation(pg_shaderID[pg_BezierFinalShader], "fs_decal");
	glUniform1i(uniform_CurveUpdate_texture_fs_decal, 0);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// echo FBO previous pass
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_BezierEcho_texID[((pg_FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);

	////////////////////////////////////////
	// binds geometry and displays it    
	// quad vao
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	// draw triangles from the currently bound VAO with current in-use shader
	// glDrawArrays(GL_TRIANGLES, 0, 3 * PG_NBFACES_QUAD);
	// Index buffer for indexed rendering
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLE_STRIP,      // mode
		3 * PG_NBFACES_QUAD,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	// unbind output FBO 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// unbinds VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	printOglError(524);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// DRAW FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////////
void window_draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glDisable (GL_BLEND);
	// PASS #1
	updateBezierCurves();
	
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	// PASS #2
	drawBezierCurves();

	glDisable(GL_BLEND);
	// PASS #3
	pg_CompositionRendering();

	glDisable(GL_BLEND);
	// PASS #4
	pg_FinalRendering();
	
	glutSwapBuffers(); 
}

//////////////////////////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM
//////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	srand(uint(RealTime()));

	// initialization of glut and its callbacks

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize(PG_WIDTH, PG_HEIGHT);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	// window opening
	glutCreateWindow("Bezier Curve");
	// window reside callback
	glutReshapeFunc(window_resize);
	// keystroke callback
	glutKeyboardFunc(window_key);
	// idle update
	glutTimerFunc((int)1,&window_idle_browse, Step);
	// draw callback
	glutDisplayFunc(window_draw);

	// glew initialization
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "GLEW version %s\n", glewGetString(GLEW_VERSION));

	// initialization of OpenGL and glew
	initGL();

	// Bezier curve & quad data initialization
	initVectorClipArtData();
	initQuadCurveData();

	// Bezier curve textures initialization
	// are used to initialize the 
	pg_initTextures();
	pg_writeBezierCurveTextures();

	// curve and curve update VBO/VAO initialization
	// curve update: a quad
	// curve: a list of 4-uple points
	glGenVertexArrays(pg_VAOCount, &pg_vaoID[0]);
	glGenBuffers(pg_VBOCount, &pg_vboID[0]);
	initShaders();
	initVAOs();

	// Bezier curve current position FBO initialization	
	pg_initFBO();

	// initial time for FPS
	InitialRealTime = RealTime();
	
	// drawing loop
	glutMainLoop();
	return 0;
}
