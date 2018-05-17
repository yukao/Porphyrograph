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
class threadWriteImageData {
public:
	char *fname;
	int w;
	int h;
	cv::Mat *img;
};
class ImageBufferDataStruct {
public:
	char fname[256];
	int w;
	int h;
	bool invert;
	bool loadedInRAM;
	bool transfering;
	bool transferred;
	bool IDallocated;
	bool swapping;
	double swapStart;
	double swapDuration;
	GLuint texBuffID;
	cv::Mat *img;
	ImageBufferDataStruct(void) {
		*fname = 0;
		loadedInRAM = false;
		transfering = false;
		transferred = false;
		IDallocated = false;
		swapping = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		img = NULL;
	}
	void release(void) {
		*fname = 0;
		loadedInRAM = false;
		transfering = false;
		transferred = false;
		swapping = false;
		swapStart = 0.0;
		swapDuration = 1.0;
		if (img) {
			img->release();
			img = NULL;
		}
	}
	~ImageBufferDataStruct(void) {
		if (img) {
			img->release();
		}
	}
	bool pg_loadImage(
		bool invert, int width, int height, bool threaded, bool verbose);
bool pg_toGPUImage(GLuint textureID, bool is_rectangle,
		GLint components, GLenum format,
		GLenum datatype, GLenum texturefilter);
};
struct ThreadTextureData {
  bool is_free;
  int indOpenGLContext;
  ImageBufferDataStruct *imageStr;
  GLuint textureID;
  bool is_rectangle;
  GLint components;
  GLenum format;
  GLenum datatype;
  GLenum texturefilter;
};
#ifdef WIN32
	DWORD WINAPI pg_toGPUImageThreaded( LPVOID lpParam );
#else
	void* pg_toGPUImageThreaded(void * lpParam);
#endif

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// ImageBufferDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
#define PG_IMAGE_NB_TEXTURES                 (6)
extern ImageBufferDataStruct image_buffer_data[PG_IMAGE_NB_TEXTURES];
extern ImageBufferDataStruct image_mask_buffer_data[PG_IMAGE_NB_TEXTURES/3];
extern ImageBufferDataStruct image_swap_buffer_data[PG_IMAGE_NB_TEXTURES];
// interpolation weight between image buffer swap buffer in each layer
extern GLfloat image_buffer_alphaSwap02[PG_IMAGE_NB_TEXTURES / 2];
extern GLfloat image_buffer_alphaSwap35[PG_IMAGE_NB_TEXTURES / 2];
// image buffer layer weights
extern GLfloat image_buffer_weights[PG_IMAGE_NB_TEXTURES];

extern GLfloat image_buffer_position_noises[PG_IMAGE_NB_TEXTURES*2];
extern GLfloat mask_buffer_position_noises[PG_IMAGE_NB_TEXTURES * 2];
extern GLfloat mask_inverse_buffer_position_noises[PG_IMAGE_NB_TEXTURES * 2];

// the index from which an image available for swapping is looked for
extern int IndInitialSwapImage;

////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern int IndPlayedImageFileDir;
extern int IndPlayedImageFile;

extern std::string ImageDir;
extern std::string MaskDir;

bool pg_initTextures( void );
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );
bool pg_loadTexture3D( string filePrefixName , string fileSuffixName , 
		    int nbTextures , int bytesperpixel , 
		    bool invert ,
		    GLuint *textureID ,
		    GLint components, GLenum format ,
		    GLenum texturefilter , 
		    int width , int height );

/// THREAD SAVE IMAGE
#ifdef WIN32
DWORD WINAPI pg_readImageData(LPVOID lpParam);
#else
void* pg_readImageData(void * lpParam);
#endif

/// THREAD SAVE IMAGE
#ifdef WIN32
DWORD WINAPI threaded_write_png(LPVOID lpParam);
#else
void* threaded_write_png(void * lpParam);
#endif
#ifdef WIN32
DWORD WINAPI threaded_write_jpg(LPVOID lpParam);
#else
void* threaded_write_jpg(void * lpParam);
#endif

// TEXTURE BUFFER MANAGEMENT
bool update_image_buffer_swapping(void);
int available_swap_image_buffer(int indInitialImage);
int available_random_swap_image_buffer(int indOpposedCenter);
bool pg_load_and_swap_image( string * fileName , double duration );
bool GPUtransfer_image_buffer_data(void);
bool  pg_ReadInitalImageTextures( int ind_dir );
#endif
