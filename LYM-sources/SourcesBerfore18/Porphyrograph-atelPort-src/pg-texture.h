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
class PhotoBufferDataStruct {
public:
	char fname[256];
	int w;
	int h;
	bool transfering;
	bool transferred;
	double blendStart;        // start of blending the current photo (<0 if not displayed)
	float intervalDuration;  // duration between end of plateau and start of the other photo
	GLuint texBuffID;
	int indImgPhotoBuffer;
	PhotoBufferDataStruct(int ind) {
		*fname = 0;
		indImgPhotoBuffer = ind;
		transfering = false;
		transferred = false;
		blendStart = -1.0f; // convention blendStart < 0 == no blending
		intervalDuration = 0.01f;
		texBuffID = 0;
	}
	~PhotoBufferDataStruct(void) {
	}
	// 2D image loading choice threaded/non threaded
	bool pg_loadPhoto(bool threaded, bool verbose);
	// 2D texture tranfer to GPU
	bool pg_sendGPUPhotoData(GLuint textureID, bool is_rectangle,
		GLint components, GLenum format,
		GLenum datatype, GLenum texturefilter, 
		int thewidth , int theheight , char * imgdata);
};

struct ThreadTextureData {
  bool is_free;
  char *fname;
  PhotoBufferDataStruct *photo_buffer_data;
  GLuint textureID;
  bool is_rectangle;
  GLint components;
  GLenum datatype;
  GLenum texturefilter;
  HGLRC loaderContext;
  HDC hdc;
};

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


extern PhotoBufferDataStruct pg_Photo_buffer_data[PG_PHOTO_NB_TEXTURES];
// extern cv::Mat imgPhotoBuffer[PG_PHOTO_NB_TEXTURES];
extern PhotoBufferDataStruct pg_CameraFrame_buffer_data;
extern PhotoBufferDataStruct pg_MovieFrame_buffer_data;
extern float pg_Photo_weight[PG_PHOTO_NB_TEXTURES];

////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
extern int IndPlayedDiaporamaFile;
extern int CurrentDiaporamaDir;
extern bool ascendingDiaporama;

extern std::string DiaporamaDir;

////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
int GetFreeOpenGLContext(void);

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath);

// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)
bool is_subfile(struct dirent *dirp, std::string *dirpath);
int count_dir_file(string *dirpath);
bool is_image_dir_file(string *dirpath, int *indfile, string *fileName, bool ascending);
bool nextFile(int *currentFileIndex, string *fileName, bool ascending);

// TEXTURE INITIALIZATION
bool pg_initTextures( void );
void pg_writeAndTreansferParticlePosition_Texture(void);

// TEXTURE GENERATION
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );

// IMAGE FILE WRITING (JPG OR PNG) (THREADED) 
// THREADED TEXTURE WRITING
/// SAVE IMAGE
#ifdef WIN32
DWORD WINAPI writepng(LPVOID lpParam);
#else
void* writepng(void * lpParam);
#endif
#ifdef WIN32
DWORD WINAPI writejpg(LPVOID lpParam);
#else
void* writejpg(void * lpParam);
#endif

// TEXTURE LOADING
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

/// THREAD LOAD IMAGE
#ifdef WIN32
DWORD WINAPI pg_threaded_loadPhotoData(LPVOID lpParam);
#else
void* pg_threaded_loadPhotoData(void * lpParam);
#endif

/// NON THREADED LOAD CAMERA FRAME
void loadCameraFrame(bool initial_capture);

/// THREAD LOAD CAMERA FRAME
#ifdef WIN32
DWORD WINAPI pg_threaded_loadCameraFrame(LPVOID lpParam);
#else
void* pg_threaded_loadCameraFrame(void * lpParam);
#endif

/// THREAD LOAD MOVIE FRAME
#ifdef WIN32
DWORD WINAPI pg_threaded_loadMovieFrame(LPVOID lpParam);
#else
void* pg_threaded_loadMovieFrame(void * lpParam);
#endif

// particle initialization from photography
bool generateParticleInitialPosColorRadiusfromImage(string fileName,
	GLuint *textureParticleInitializationID); // 2 texture IDs one for pos/speed, the other one for color/radius

// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
#ifdef PG_VIDEO_ACTIVE
bool pg_initCameraFrameTexture(Mat *video_frame);
bool pg_initMovieFrameTexture(Mat *video_frame);
int pg_initVideoCameraCapture(void);
#ifdef WIN32
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam);
#else
void* pg_initVideoMoviePlayback(void * lpParam);
#endif
#ifdef WIN32
DWORD WINAPI pg_movieLoop(LPVOID lpParam);
#else
void* pg_movieLoop(void * lpParam);
#endif
int pg_initVideoCameraCapture(void);
#endif

// TEXTURE BUFFER MANAGEMENT
void pg_launch_diaporama(void);
bool pg_update_diaporama(void);
bool pg_load_photo_layer(std::string * fileName, int indPhoto, bool threaded);
// bool GPUtransfer_photo_buffer_data(void);
#endif
