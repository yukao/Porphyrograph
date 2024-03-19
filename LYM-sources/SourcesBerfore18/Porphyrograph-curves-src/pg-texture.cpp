////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg-texture.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; TrackDisplay even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <stdarg.h>     // Header File For Variable Argument Routines
#include <string>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <GL/glew.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <random>
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

#include "pg-texture.h"
#include "BezierCurveMain.h"


/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
int printOglError(int no) {

	int    retCode = 0;

	GLenum err;
	if (((err = glGetError()) != GL_NO_ERROR)) {
		printf("glError at %d -> %d: %s\n", no,
			err, gluErrorString(err));
		retCode = 1;
	}
	return retCode;
}

/////////////////////////////////////////////////////////////////
// TEXTURE LOADING
/////////////////////////////////////////////////////////////////

// 3D texture loading + transfer to GPU
bool pg_loadTexture3D(string filePrefixName, string fileSuffixName,
	int nbTextures, int bytesperpixel,
	bool invert,
	GLuint *textureID,
	GLint components, GLenum format,
	GLenum texturefilter,
	int width, int height, int depth) {
	// data type is assumed to be GL_UNSIGNED_BYTE

	char filename[1024];

	long size = width * height * bytesperpixel;
	GLubyte * bitmap = new unsigned char[size * nbTextures];
	long ind = 0;

	glEnable(GL_TEXTURE_3D);
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	for (int indTex = 0; indTex < nbTextures; indTex++) {
		sprintf(filename, "%s_%03d%s", filePrefixName.c_str(),
			indTex + 1, fileSuffixName.c_str());
		printf("Loading %s\n", filename);

		// texture load through OpenCV
#ifndef OPENCV_3
		cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
		// printf("nb channels %d\n", img.channels());
		int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
			(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(img, img, colorTransform);
		cv::Mat result;
		if (invert)
			cv::flip(img, result, 0); // vertical flip
		else
			result = img;
#else
		cv::Mat img = cv::imread(filename, cv::IMREAD_UNCHANGED);   // Read the file
		int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
			(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
		// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(img, img, colorTransform);
		cv::Mat result;
		if (invert)
			cv::flip(img, result, 0); // vertical flip
		else
			result = img;
#endif

		if (!result.data) {                              // Check for invalid input
			printf( "Could not open or find the image %s!", filename); throw 425;
			return false;
		}
		if (result.cols != width
			|| result.rows != height) {   // Check for invalid input
			printf( "Unexpected image size %s %d/%d %d/%d!", filename, result.cols, width, result.rows, height); throw 425;
			return false;
		}

		GLubyte * ptr = result.data;
		for (long int i = 0; i < size; i++)
			bitmap[ind++] = ptr[i];
	}
	// printf("Final index %ld / %ld\n" , ind , size * nbTextures );
	// glActiveTexture (GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_3D, *textureID);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		components,            // Components: Internal colour format to convert to
		width,          // Image width
		height,          // Image heigh
		depth,          // Image depth
		0,                 // Border width in pixels (can either be 1 or 0)
		format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		bitmap);        // The actual image data itself

	printOglError(0);

	// memory release
	delete[] bitmap;

	// glGenerateMipmap(GL_TEXTURE_2D);
	// printf("3D textures loaded\n" );
	return true;
}

// 2D texture loading + transfer to GPU

bool pg_loadTexture(string fileName, cv::Mat *image,
	GLuint *textureID, bool is_rectangle,
	bool invert,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height) {

	// printf("Loading %s\n", fileName.c_str());

	glEnable(GL_TEXTURE_2D);
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	// texture load through OpenCV
#ifndef OPENCV_3
	cv::Mat img = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
		(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, *image, 0); // vertical flip
	else
		*image = img;
#else
	cv::Mat img = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
	int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, *image, 0); // vertical flip
	else
		*image = img;
#endif

	if (!image->data) {                              // Check for invalid input
		printf( "Could not open or find the image %s!", fileName.c_str()); throw 425;
		return false;
	}
	if (image->cols != width || image->rows != height) {                              // Check for invalid input
		printf( "Unexpected image size %s %d/%d %d/%d!", fileName.c_str(), image->cols, width, image->rows, height); throw 425;
		return false;
	}

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, *textureID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			image->cols,          // Image width
			image->rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			image->ptr());        // The actual image data itself
		printOglError(4);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			image->cols,          // Image width
			image->rows,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			image->ptr());        // The actual image data itself
		printOglError(5);
	}
	// glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}

/////////////////////////////////////////////////////////////////
// FBO INTIALIZATION
/////////////////////////////////////////////////////////////////

bool pg_initFBOTextures(GLuint *textureID, int nb_attachments) {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glEnable(GL_TEXTURE_RECTANGLE);
	for (int indAtt = 0; indAtt < nb_attachments; indAtt++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, textureID[indAtt]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F,
						leftWindowWidth, window_height);
		printf("FBO size %d %d \n" , leftWindowWidth , window_height );
		glFramebufferTexture2D(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + indAtt,
			GL_TEXTURE_RECTANGLE,
			textureID[indAtt],
			0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
			== GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			printf( "Error: Binding RECT FBO texture No %d ID %d (error %d)!", indAtt, 
					textureID[indAtt], glCheckFramebufferStatus(GL_FRAMEBUFFER)); 
			throw 336;
		}
	}

	return true;
}

bool pg_initFBO(void) {
	printOglError(3420);
	glGenFramebuffers(PG_NB_MEMORIZED_PASS, FBO_BezierUpdate);  // Bezier control point position update FBO 
	glGenFramebuffers(PG_NB_MEMORIZED_PASS, FBO_BezierEcho);  // echoed rendering of preceding frame
	glGenFramebuffers(1, &FBO_BezierCurves);  // Bezier curves rendering

	for (int indFB = 0; indFB < PG_NB_MEMORIZED_PASS; indFB++) {
		FBO_BezierUpdate_texID[indFB] = 0;
		FBO_BezierEcho_texID[indFB] = 0;
	}
	FBO_BezierCurves_texID = 0;

	glGenTextures(PG_NB_MEMORIZED_PASS, FBO_BezierUpdate_texID);
	glGenTextures(PG_NB_MEMORIZED_PASS, FBO_BezierEcho_texID);
	glGenTextures(1, &FBO_BezierCurves_texID);

	for (int indFB = 0; indFB < PG_NB_MEMORIZED_PASS; indFB++) {
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_BezierUpdate[indFB]);
		pg_initFBOTextures(FBO_BezierUpdate_texID + indFB, 1);
		glDrawBuffers(1, drawBuffers);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_BezierEcho[indFB]);
		pg_initFBOTextures(FBO_BezierEcho_texID + indFB, 1);
		glDrawBuffers(1, drawBuffers);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_BezierCurves);
	pg_initFBOTextures(&FBO_BezierCurves_texID, 1);
	glDrawBuffers(1, drawBuffers);
	printOglError(342);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

/////////////////////////////////////////////////////////////////
// TEXTURE GENERATION AND TRANSFER TO GPU
/////////////////////////////////////////////////////////////////

// TEXTURE GENERATION

// general texture allocation
void *pg_generateTexture(GLuint *textureID, pg_TextureFormat texture_format,
						 int sizeX, int sizeY) {
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	// rgb POT raw image allocation (without alpha channel)
	// printf( "Texture %dx%d nb_attachments %d\n" ,  
	//    sizeX , sizeY , nb_attachments );
	void *returnvalue = NULL;

	// Return from the function if no file name was passed in
	// Load the image and store the data
	if (texture_format == pg_byte_tex_format) {
		GLubyte *ptr = new GLubyte[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			printf( "Texture allocation error!"); throw 425;
		}
		int indTex = 0;
		for (int i = 0; i < sizeX * sizeY; i++) {
			ptr[indTex] = 0;
			ptr[indTex + 1] = 0;
			ptr[indTex + 2] = 0;
			ptr[indTex + 3] = 0;
			indTex += 4;
		}
		returnvalue = (void *)ptr;
	}
	else if (texture_format == pg_float_tex_format) {
		float *ptr = new float[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			printf("Texture allocation error!"); throw 425;
		}
		int indTex = 0;
		for (int i = 0; i < sizeX * sizeY; i++) {
			ptr[indTex] = 0.0f;
			ptr[indTex + 1] = 0.0f;
			ptr[indTex + 2] = 0.0f;
			ptr[indTex + 3] = 0.0f;
			indTex += 4;
		}
		returnvalue = (void *)ptr;
	}

	return returnvalue;
}


bool pg_initTextures(void) {
	// random positions of control points
	pg_bezierCurve_Pos_Texture_texID = 0;
	pg_bezierCurve_Pos_Texture =
		(GLfloat *)pg_generateTexture(&(pg_bezierCurve_Pos_Texture_texID), pg_float_tex_format,
			PG_WIDTH , (PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 4) / PG_WIDTH + 1);   
			// 4 floats per point because of RGBA format - BA can be used for other purposes
	if (!pg_bezierCurve_Pos_Texture) {
		printf("Error: pg_bezierCurve_Pos_Texture not allocated!"); throw 336;
	}

	// loads the curve texture
	pg_loadTexture("./ProjectsBefore18/curves/textures/comet.png", &comet_image,
		&comet_texture_2D_texID, false, false, GL_RGB8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		512, 256);
	printOglError(6);

	return true;
}


// WRITES INITIAL CURVES TO TEXTURES FOR DRAWING AN INITIAL SET OF CURVES

void pg_writeBezierCurveTextures(void) {
	// the used width is a multiple of (PG_CURVE_DEGREE + 1) so that each set of control
	// point coordinates is on a single line
	int width_used = PG_WIDTH - PG_WIDTH % (PG_CURVE_DEGREE + 1);

	// sets the values of the texture
	int index = 0;
	for (int indCurve = 0; indCurve < PG_NB_VECTOR_CLIPART; indCurve++) {
		pg_bezierCurve_Pos_Texture[index + 0] = randomValue * PG_WIDTH;
		pg_bezierCurve_Pos_Texture[index + 1] = randomValue * PG_HEIGHT;
		pg_bezierCurve_Pos_Texture[index + 2] = 0.f;
		pg_bezierCurve_Pos_Texture[index + 3] = 0.f;
		index += 4;  // RGBA
		for (int indControlPoint = 0; indControlPoint < PG_CURVE_DEGREE; indControlPoint++) {
			pg_bezierCurve_Pos_Texture[index + 0] = pg_bezierCurve_Pos_Texture[index - 4 + 0] + randomValue * 20.f - 10.f;
			pg_bezierCurve_Pos_Texture[index + 1] = pg_bezierCurve_Pos_Texture[index - 4 + 1] + randomValue * 20.f - 10.f;
			pg_bezierCurve_Pos_Texture[index + 2] = 0.f;
			pg_bezierCurve_Pos_Texture[index + 3] = 0.f;
			index += 4; // RGBA
		}
		// jumps over the end of the line in case the texture width is
		// not a multiple of(PG_CURVE_DEGREE + 1) so that each set 
		// of control point coordinates is on a single line
		while (index % PG_WIDTH >= width_used) {
			index++;
		}
	}

	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_bezierCurve_Pos_Texture_texID);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA32F,            // Components: Internal colour format to convert to
		PG_WIDTH ,          // Image width
		(PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 4) / PG_WIDTH + 1,          // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_FLOAT,  // Image data type
		(GLvoid *)pg_bezierCurve_Pos_Texture);        // The actual image data itself

	printf("Bezier Curve control point position texture values text size %dx%d\n", 
		PG_WIDTH, (PG_NB_VECTOR_CLIPART * (PG_CURVE_DEGREE + 1) * 4) / PG_WIDTH + 1);

	printOglError(12);
}




