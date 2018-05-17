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

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).

class PhotoSwapDataStruct {
public:
	int indSwappedPhoto;
	int indOldPhoto;
	bool swapping;
	float swapStart;
	float blendStart;        // start of blending the current photo (<0 if not displayed)
	float intervalDuration;  // duration between end of plateau and start of the other photo
	PhotoSwapDataStruct(void) {
		int indSwappedPhoto = -1;
		int indOldPhoto = -1;
		swapping = false;
		swapStart = 0.0f;
		blendStart = -1.0f; // convention blendStart < 0 == no blending
		intervalDuration = 0.01f;
	}
};
class PhotoDataStruct {
	public:
		char PhotoName[256];
		int w;
		int h;
		GLenum format;
		bool invert;
		bool IDallocated;
		GLuint texBuffID;
		cv::Mat *imgPhotoOpenCV;
		unsigned char *compressedPhotoBitmap;
		unsigned int compressedFormat;
		PhotoDataStruct(void) {
			w = -1;
			h = -1;
			format = 0;
			*PhotoName = 0;
			IDallocated = false;
			imgPhotoOpenCV = NULL;
			compressedPhotoBitmap = NULL;
			compressedFormat = 0;
		}
		void release(void) {
			w = -1;
			h = -1;
			format = 0;
			*PhotoName = 0;
			if (imgPhotoOpenCV) {
				imgPhotoOpenCV->release();
				imgPhotoOpenCV = NULL;
			}
			if (compressedPhotoBitmap) {
				free(compressedPhotoBitmap);
				compressedPhotoBitmap = NULL;
			}
			compressedFormat = 0;
		}
		~PhotoDataStruct(void) {
			if (imgPhotoOpenCV) {
				imgPhotoOpenCV->release();
				imgPhotoOpenCV = NULL;
			}
			if (compressedPhotoBitmap) {
				free(compressedPhotoBitmap);
				compressedPhotoBitmap = NULL;
			}
		}
		bool pg_loadPhoto(
			bool invert, int width, int height, bool verbose);
		bool pg_toGPUPhoto(bool is_rectangle,
			GLint components, 
			GLenum datatype, GLenum texturefilter);
	};


// additional contexts for threading transfer to GPU
#define PG_MAX_NUMBER_OF_OPENGL_CONTEXTS  (4)
	
////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
enum TextureEncoding { JPG = 0, PNG, PNGA, PNG_GRAY, PNGA_GRAY, RGB, RAW, EmptyTextureEncoding };
extern const char *TextureEncodingString[EmptyTextureEncoding + 1];

enum pg_TextureFormat { pg_byte_tex_format = 0, pg_float_tex_format, Emptypg_TextureFormat };


////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
#define PG_PHOTO_NB_TEXTURES                 (2)
#if defined (TVW) || defined (CRITON)
#define PG_PHOTO_NB_TEXTURES_TVW                 (6)
#endif

extern PhotoDataStruct pg_Photo_buffer_data[PG_PHOTO_NB_TEXTURES];
// cv::Mat imgPhotoBuffer[PG_PHOTO_NB_TEXTURES];
extern PhotoDataStruct pg_CameraFrame_buffer_data;
extern PhotoDataStruct pg_MovieFrame_buffer_data;

extern float pg_Photo_weight[PG_PHOTO_NB_TEXTURES];

#if defined (TVW) || defined (CRITON)
extern PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES_TVW];
extern PhotoDataStruct **pg_Photo_buffer_dataTVW;
#else
extern PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES];
#endif

#if defined (TVW) || defined (CRITON)
/////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern int pg_CurrentDiaporamaDir;
extern int pg_CurrentDiaporamaFile;
extern int pg_CurrentDiaporamaDir;
extern int pg_CurrentDiaporamaFile;

extern std::string ImageDir;
extern std::string MaskDir;

///////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
extern int pg_nbCompressedImages;
extern int pg_nbCompressedImageDirs;
extern int *pg_nbCompressedImagesPerFolder;
extern int *pg_firstCompressedFileInFolder;
extern PhotoDataStruct pg_Photo_mask_buffer_data[PG_PHOTO_NB_TEXTURES_TVW / 3];
// interpolation weight between image buffer swap buffer in each layer
extern GLfloat pg_Photo_alphaSwap02[PG_PHOTO_NB_TEXTURES_TVW / 2];
extern GLfloat pg_Photo_alphaSwap35[PG_PHOTO_NB_TEXTURES_TVW / 2];
// image buffer layer weights
extern GLfloat pg_Photo_weightTVW[PG_PHOTO_NB_TEXTURES_TVW];

extern GLfloat pg_Photo_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];
extern GLfloat pg_Photo_mask_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];
// GLfloat mask_inverse_buffer_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];

// the index from which an image available for swapping is looked for
extern int pg_IndInitialSwapPhoto;
#endif


////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
extern int pg_CurrentDiaporamaFile;
extern int pg_CurrentDiaporamaDir;
extern bool ascendingDiaporama;

extern std::string DiaporamaDir;

////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
// int GetFreeOpenGLContext(void);
// int GetFreeOpenGLContextTVW(void);

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath);

// ONLY FOR TERRAINS VAGUES
#if defined (TVW) || defined (CRITON)
std::string *nextFileIndexDiskLoop(std::string *dirpath, int *currentDirIndex, int *currentFileIndex);
string * nextFileIndexDiskNoLoop(string *dirpath, int *currentDirIndex, int *currentFileIndex, 
	int maxFilesPerFolder);
#endif
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)
bool is_substring_index(std::string str, int ind);
bool is_substring_index(char * charstr, int ind);
bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir);
bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile);
string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile);
int nextFileIndexMemoryLoop(int currentDirIndex, int *currentFileIndex, bool ascending);

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

/// NON THREADED LOAD CAMERA FRAME
#ifdef PG_WITH_CAMERA_CAPTURE
void loadCameraFrame(bool initial_capture);
bool pg_initCameraFrameTexture(Mat *video_frame);
int pg_initVideoCameraCapture(void);
#endif

// particle initialization from photography
bool generateParticleInitialPosColorRadiusfromImage(string fileName,
	GLuint *textureParticleInitializationID); // 2 texture IDs one for pos/speed, the other one for color/radius

// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
bool pg_initMovieFrameTexture(Mat *video_frame);
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

// TEXTURE BUFFER MANAGEMENT
void pg_launch_diaporama(void);
bool pg_update_diaporama(void);

#if defined (GN) || defined (MALAUSSENA)
void pg_CATable_values(GLuint textureID, GLubyte * data_table, int width, int height);
#endif

#if defined (TVW) || defined (CRITON)
// TEXTURE BUFFER MANAGEMENT
bool update_image_buffer_swapping(void);
int available_swap_image_buffer(int indInitialImage);
int available_random_swap_image_buffer(int indOpposedCenter);
bool pg_swap_image(int indcomprImage);
bool  pg_ReadInitalImageTextures(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder);
#endif

#endif
