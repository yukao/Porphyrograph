/*! \file pg-texture.h
 * 
 * 
 *     File pg-texture.h
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

#ifndef PG_TEXTURE_H
#define PG_TEXTURE_H
// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).


// additional contexts for threading transfer to GPU
#define PG_MAX_NUMBER_OF_OPENGL_CONTEXTS  (4)
	
////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
#define PG_PHOTO_NB_TEXTURES                 (2)

enum TextureEncoding { JPG = 0, PNG, PNGA, PNG_GRAY, PNGA_GRAY, RGB, RAW, EmptyTextureEncoding };
extern const char *TextureEncodingString[EmptyTextureEncoding + 1];

enum pg_TextureFormat { pg_byte_tex_format = 0, pg_float_tex_format, Emptypg_TextureFormat };

////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////
// ERROR MANAGEMENT
int printOglError(int no);

// OpenGL context for thread management
int GetFreeOpenGLContext(void);

// texture loading
bool pg_loadTexture3D(string filePrefixName, string fileSuffixName,
	int nbTextures, int bytesperpixel,
	bool invert,
	GLuint *textureID,
	GLint components, GLenum format,
	GLenum texturefilter,
	int width, int height, int depth);
bool pg_loadTexture(string fileName, cv::Mat *image,
	GLuint *textureID, bool is_rectangle,
	bool invert,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height);

// Bezier curve control point position FBO initialization
bool pg_initFBOTextures(GLuint *textureID, int nb_attachments);
bool pg_initFBO(void);

// Bezier curve control point initial position texture initialization
bool pg_initTextures(void);

// texture generation
void *pg_generateTexture(GLuint *textureID, pg_TextureFormat texture_format,
	int sizeX, int sizeY);

// Bezier curve control point initial position writing and GPU transfer of the texture
void pg_writeBezierCurveTextures(void);

#endif