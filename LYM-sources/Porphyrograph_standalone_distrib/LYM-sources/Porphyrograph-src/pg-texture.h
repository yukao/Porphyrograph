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

extern unsigned int imgPhotoCompressedFormat[_NbConfigurations];
extern unsigned char* imgPhotoCompressedBitmap[_NbConfigurations];
extern cv::Mat imgPhotoBGRInit[_NbConfigurations];
extern cv::Mat imgPhotoRGB[_NbConfigurations];

class PhotoSwapDataStruct {
public:
	int indSwappedPhoto;
	int indOldPhoto;
	bool swapping;
	float swapStart;
	float blendStart;        // start of blending the current photo (<0 if not displayed)
	float intervalDuration;  // duration between end of plateau and start of the other photo
	PhotoSwapDataStruct(void) {
		indSwappedPhoto = -1;
		indOldPhoto = -1;
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
	GLenum photoFormat;
	bool invert;
	bool IDallocated;
	GLuint texBuffID;
	PhotoDataStruct(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		*PhotoName = 0;
		IDallocated = false;
		invert = false;
		texBuffID = -1;
	}
	void release(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		*PhotoName = 0;
	}
	~PhotoDataStruct(void) {
	}
	// calls to pg_loadPhoto should be immediately followed by pg_toGPUPhoto
	// due to global storage variables imgPhotoCompressedFormat[indConfiguration], imgPhotoCompressedBitmap[indConfiguration], imgPhotoBGRInit[indConfiguration], imgPhotoRGB[indConfiguration];
	bool pg_loadPhoto(
		bool invert, int width, int height, bool verbose, int indConfiguration);
	bool pg_toGPUPhoto(bool is_rectangle,
		GLint components,
		GLenum datatype, GLenum texturefilter, int iindConfiguration);
};

class ClipFramesDataStruct {
public:
	GLuint texBuffID;
	GLenum clipImgFormat;
	ClipFramesDataStruct(void) {
		clipImgFormat = 0;
		texBuffID = -1;
	}
	~ClipFramesDataStruct(void) {
		clipImgFormat = 0;
		texBuffID = -1;
	}
	// calls to pg_loadClipFrames should be immediately followed by pg_toGPUClipFrames
	// due to global storage variables imgPhotoCompressedFormat, imgPhotoCompressedBitmap, imgPhotoBGRInit, imgPhotoRGB;
	bool pg_loadClipFrames(char* fileName, int width, int height, bool verbose, int indConfiguration);
	bool pg_toGPUClipFrames(int w, int h, GLint components, GLenum datatype, GLenum texturefilter, int indConfiguration);
};

// additional contexts for threading transfer to GPU
#define PG_MAX_NUMBER_OF_OPENGL_CONTEXTS  (4)
	
////////////////////////////////////////////////////////////////////
// OPENCV MATRICES TO STORE IMAGES
extern cv::Mat pngImgMatRGBInitial;
extern cv::Mat pngImgMatRGBFlipped;
extern cv::Mat pngImgMatBGR;
extern cv::Mat jpgImgMatRGBInitial;
extern cv::Mat jpgImgMatRGBFlipped;
extern cv::Mat jpgImgMatBGR;
extern cv::Mat tiffLayerMatRGBInit;
extern cv::Mat tiffLayerMatBGR;
extern cv::Mat tiffLayerMatBGRFlipped;
extern cv::Mat image3DLayerBGRInit;
extern cv::Mat image3DLayerRGB;
extern cv::Mat image3DLayerRGBFlipped;
extern cv::Mat textureImgMatBGRInit;
extern cv::Mat textureImgMatRGB;
extern cv::Mat textureImgMatRGBFlipped;
extern cv::Mat particleImgMatBGRInit;
extern cv::Mat particleImgMatRGB;
extern cv::Mat particleImgMatRGBFlipped;
extern cv::Mat particleAccImgMatBGRInit;
extern cv::Mat particleAccImgMatRGB;
extern cv::Mat particleAccImgMatRGBFlipped;

////////////////////////////////////////////////////////////////////
// FIRST FRAME IN A MOVIE
////////////////////////////////////////////////////////////////////
extern Mat pg_FirstMovieFrame; // First frame in a video (opening or looping) 
extern bool pg_FirstMovieFrameIsAvailable; // Available frist frame so as to avoid second reading  

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
#if defined(TVW)
#define PG_PHOTO_NB_TEXTURES_TVW                 (6)
#endif

extern PhotoDataStruct pg_CameraFrame_buffer_data;
extern PhotoDataStruct pg_MovieFrame_buffer_data;

extern float pg_Photo_weight[PG_PHOTO_NB_TEXTURES];

#if defined(TVW)
extern PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES_TVW];
#else
extern PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES];
#endif
extern PhotoDataStruct** pg_Photo_buffer_data[_NbConfigurations];

/////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
#if defined(var_photo_diaporama)
extern int pg_CurrentDiaporamaDir;
extern double pg_CurrentDiaporamaEnd;
extern int pg_CurrentDiaporamaFile;
#endif

///////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
extern int pg_nbCompressedImages[_NbConfigurations];
extern int pg_nbCompressedImageDirs[_NbConfigurations];
extern int* pg_nbCompressedImagesPerFolder[_NbConfigurations];
extern int* pg_firstCompressedFileInFolder[_NbConfigurations];

// the index from which an image available for swapping is looked for
extern int pg_IndInitialSwapPhoto;

#if defined(TVW)
extern PhotoDataStruct pg_Photo_mask_buffer_data[_NbConfigurations[PG_PHOTO_NB_TEXTURES_TVW / 3];

// interpolation weight between image buffer swap buffer in each layer
extern GLfloat pg_Photo_alphaSwap02[PG_PHOTO_NB_TEXTURES_TVW / 2];
extern GLfloat pg_Photo_alphaSwap35[PG_PHOTO_NB_TEXTURES_TVW / 2];
// image buffer layer weights
extern GLfloat pg_Photo_weightTVW[PG_PHOTO_NB_TEXTURES_TVW];

extern GLfloat pg_Photo_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];
extern GLfloat pg_Photo_mask_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];
// GLfloat mask_inverse_buffer_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2];

#endif

extern std::string pg_ImageDirectory[_NbConfigurations];
#if defined(TVW)
extern std::string pg_MaskDirectory[_NbConfigurations];
#endif
#if defined(TVW) || defined(var_moving_messages)
extern std::string pg_MessageDirectory[_NbConfigurations];
extern std::string pg_MessageFile[_NbConfigurations];
#endif

////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
#if defined(var_photo_diaporama)
extern int pg_CurrentDiaporamaFile;
extern int pg_CurrentDiaporamaDir;
extern bool ascendingDiaporama;
#endif

////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
////////////////////////////////////////////////////////////////////
// clips are made of sequences of images loaded in GPU
extern std::string pg_ClipDirectory[_NbConfigurations];
extern ClipFramesDataStruct** pg_ClipFrames_buffer_data[_NbConfigurations];
extern int pg_nbClips[_NbConfigurations];
extern int* pg_nbCompressedClipFramesPerFolder[_NbConfigurations];
extern int* pg_firstCompressedClipFramesInFolder[_NbConfigurations];
extern int pg_nbCompressedClipFrames[_NbConfigurations];


////////////////////////////////////////////////////////////////////
// BG COLOR
////////////////////////////////////////////////////////////////////
#if defined(var_flashchange_BGcolor_freq)
extern float BG_r;
extern float BG_g;
extern float BG_b;
#endif

////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
// int GetFreeOpenGLContext(void);
// int GetFreeOpenGLContextTVW(void);

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath);

// ONLY FOR TERRAINS VAGUES
#if defined(TVW)
std::string *nextFileIndexDiskLoop(std::string *dirpath, int *currentDirIndex, int *currentFileIndex);
string * nextFileIndexDiskNoLoop(string *dirpath, int *currentDirIndex, int *currentFileIndex, 
	int maxFilesPerFolder);
#endif
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)
//bool is_substring_index(char * charstr, int ind);
//bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir);
//bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile);
//string * is_subdir_subfile_index(std::string *dirpath, int *inddir, int *indfile);
int nextFileIndexMemoryLoop(int currentDirIndex, int *currentFileIndex, bool ascending);

// TEXTURE INITIALIZATION
bool pg_loadAllTextures( void );
void pg_writeAndTreansferParticlePosition_Texture(void);

// TEXTURE GENERATION
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );

// IMAGE FILE WRITING (JPG OR PNG) (THREADED) 
// THREADED TEXTURE WRITING
/// SAVE IMAGE
/*
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
*/

void writejpg(cv::String imageFileName);
void writepng(cv::String imageFileName);

// loading a dds format under its compressed form (contrary to other formats (png, jpg) which are decompressed)
bool pg_load_compressed_photo(char* fileName, int width, int height, int indConfiguration);

// TEXTURE LOADING
bool pg_loadTexture3D(string filePrefixName, string fileSuffixName,
	int nbTextures, int bytesperpixel,
	bool invert,
	GLuint *textureID,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height);
bool pg_loadTexture2D(string fileName,
	GLuint *textureID, bool is_rectangle,
	bool invert,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height);

/// NON THREADED LOAD CAMERA FRAME
#if defined(var_cameraCaptFreq)
void loadCameraFrame(bool initial_capture, int IPCam_no);
void pg_initCameraFrameTexture(Mat *video_frame);
void pg_openCameraCaptureAndLoadFrame(void);
void pg_releaseCameraCapture(void);
void pg_initWebcamParameters(void);
#endif

// particle initialization from photography
bool generateParticleInitialPosColorRadiusfromImage(string fileName, int indConfiguration); // 2 texture IDs one for pos/speed, the other one for color/radius

// particle acceleration shift from photography
bool storeParticleAccelerationfromImage(string fileName); 

// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
void pg_initMovieFrameTexture(Mat *video_frame);

#ifdef WIN32
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam);
#else
void* pg_initVideoMoviePlayback(void * lpParam);
#endif
void* pg_initVideoMoviePlayback_nonThreaded(string * fileName);

#ifdef WIN32
DWORD WINAPI pg_movieLoop(LPVOID lpParam);
#else
void* pg_movieLoop(void * lpParam);
#endif

// TEXTURE BUFFER MANAGEMENT
#if defined(var_photo_diaporama)
void diaporama_random(void);
void pg_launch_diaporama(void);
bool pg_update_diaporama(void);
#endif

#if defined(var_CATable)
void pg_CATable_values(GLuint textureID, GLubyte * data_table, int width, int height);
#endif

#if defined(TVW)
// TEXTURE BUFFER MANAGEMENT
bool update_image_buffer_swapping(void);
int available_swap_image_buffer(int indInitialImage);
int available_random_swap_image_buffer(int indOpposedCenter);
bool pg_swap_image(int indcomprImage);
#endif

bool  pg_ReadInitalImageTexturesTVW(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder);


#endif

bool  pg_loadAllDiaporamas(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder);
bool  pg_ReadInitalClipFramesTextures(void);