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

#define     NULL_ID   0

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).

extern unsigned int imgPhotoCompressedFormat[PG_MAX_CONFIGURATIONS];
extern unsigned char* imgPhotoCompressedBitmap[PG_MAX_CONFIGURATIONS];
extern cv::Mat imgPhotoBGRInit[PG_MAX_CONFIGURATIONS];
extern cv::Mat imgPhotoRGB[PG_MAX_CONFIGURATIONS];

class PhotoSwapData {
public:
	int indSwappedPhoto;
	int indOldPhoto;
	bool swapping;
	float swapStart;
	float blendStart;        // start of blending the current photo (<0 if not displayed)
	PhotoSwapData(void) {
		indSwappedPhoto = 0;
		indOldPhoto = 0;
		swapping = false;
		swapStart = 0.0f;
		blendStart = -1.0f; // convention blendStart < 0 == no blending
	}
};
class PhotoData {
public:
	string PhotoName;
	int w;
	int h;
	GLenum photoFormat;
	bool invert;
	bool IDallocated;
	GLuint texBuffID;
	PhotoData(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		PhotoName = "";
		IDallocated = false;
		invert = false;
		texBuffID = NULL_ID;
	}
	void release(void) {
		w = -1;
		h = -1;
		photoFormat = 0;
		PhotoName = "";
	}
	~PhotoData(void) {
	}
	// calls to pg_loadPhoto should be immediately followed by pg_toGPUPhoto
	// due to global storage variables imgPhotoCompressedFormat[indConfiguration], imgPhotoCompressedBitmap[indConfiguration], imgPhotoBGRInit[indConfiguration], imgPhotoRGB[indConfiguration];
	bool pg_loadPhoto(bool invert, int width, int height, bool verbose, int indConfiguration);
	bool pg_toGPUPhoto(bool is_rectangle,
		GLint components,
		GLenum datatype, GLenum texturefilter, int iindConfiguration);
};

class ClipFramesData {
public:
	GLuint texBuffID;
	GLenum clipImgFormat;
	ClipFramesData(void) {
		clipImgFormat = 0;
		texBuffID = NULL_ID;
	}
	~ClipFramesData(void) {
		clipImgFormat = 0;
		texBuffID = NULL_ID;
	}
	// calls to pg_loadClipFrames should be immediately followed by pg_toGPUClipFrames
	// due to global storage variables imgPhotoCompressedFormat, imgPhotoCompressedBitmap, imgPhotoBGRInit, imgPhotoRGB;
	bool pg_loadClipFrames(string *fileName, int width, int height, int indConfiguration);
	bool pg_toGPUClipFrames(int w, int h, GLenum texturefilter, int indConfiguration);
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
enum pg_Texture_Usages {
	Texture_master_mask = 0, Texture_mesh, Texture_sensor, Texture_logo,
	Texture_brush, Texture_noise, Texture_curve_particle, Texture_splat_particle, Texture_part_init,
	Texture_part_acc, Texture_pixel_acc, Texture_repop_density, Texture_multilayer_master_mask
};

class pg_TextureData {
	public:
	// file names
	string texture_fileName;
	string texture_fileNameSuffix;// usages
	// usages
	pg_Texture_Usages texture_usage;
	// rank (if used several times for the same usage)
	unsigned int texture_Rank;
	// 2D or 3D
	int texture_Dimension;
	// number of piled 2D textures for a 3D texture
	unsigned int texture_Nb_Layers;
	// dimensions
	int texture_Size_X;
	int texture_Size_Y;
	// color depth
	int texture_Nb_Bytes_per_Pixel;
	// rectangle or PoT
	bool texture_Is_Rectangle;
	// texture inversion
	bool texture_Invert;
	// texture ID
	GLuint texture_texID;
	pg_TextureData(void) {
		texture_fileName = "";
		texture_fileNameSuffix = "";// usages
		// usages
		texture_usage = Texture_brush;
		// rank (if used several times for the same usage)
		texture_Rank = 0;
		// 2D or 3D
		texture_Dimension = 2;
		// number of piled 2D textures for a 3D texture
		texture_Nb_Layers = 1;
		// dimensions
		texture_Size_X = 0;
		texture_Size_Y = 0;
		// color depth
		texture_Nb_Bytes_per_Pixel = 4;
		// rectangle or PoT
		texture_Is_Rectangle = true;
		// texture inversion
		texture_Invert = false;
		// texture ID
		texture_texID = 0;
	}
	~pg_TextureData(void) {
	}
};

extern 	pg_TextureData texDataScreenFont;

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
#define PG_PHOTO_NB_TEXTURES                 (2)

extern PhotoData pg_CameraFrame_buffer_data;
extern PhotoData pg_MovieFrame_buffer_data;

extern float pg_Photo_weight[PG_PHOTO_NB_TEXTURES];
extern PhotoSwapData pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES];

/////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
extern double pg_CurrentDiaporamaEnd;
extern int pg_CurrentDiaporamaFile;


///////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
extern int pg_nbCompressedImages[PG_MAX_CONFIGURATIONS];
extern int pg_nbCompressedImageDirs[PG_MAX_CONFIGURATIONS];
extern vector<int>pg_nbCompressedImagesPerFolder[PG_MAX_CONFIGURATIONS];
// diaporama directories
extern vector<string> pg_compressedImageDirsNames[PG_MAX_CONFIGURATIONS];
// files inside diaporama directories
extern vector <vector<PhotoData>> pg_compressedImageData[PG_MAX_CONFIGURATIONS];
extern vector<vector<string>> pg_compressedImageFilesNames[PG_MAX_CONFIGURATIONS];

// the index from which an image available for swapping is looked for
extern int pg_IndInitialSwapPhoto;
extern std::string pg_MessageFile[PG_MAX_CONFIGURATIONS];

////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
////////////////////////////////////////////////////////////////////
// clips are made of sequences of images loaded in GPU
extern ClipFramesData** pg_ClipFrames_buffer_data[PG_MAX_CONFIGURATIONS];
extern int pg_nbClips[PG_MAX_CONFIGURATIONS];
extern int* pg_nbCompressedClipFramesPerFolder[PG_MAX_CONFIGURATIONS];
extern int* pg_firstCompressedClipFramesInFolder[PG_MAX_CONFIGURATIONS];
extern int pg_nbCompressedClipFrames[PG_MAX_CONFIGURATIONS];


////////////////////////////////////////////////////////////////////
// PROTOTYPES
////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
// int GetFreeOpenGLContext(void);
// int GetFreeOpenGLContextTVW(void);

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath);

// ONLY FOR TERRAINS VAGUES
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)
//bool is_substring_index(char * charstr, int ind);
//bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir);
//bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile);
//string * is_subdir_subfile_index(std::string *dirpath, int *inddir, int *indfile);
void nextFileIndexMemoryLoop(bool ascending);

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
#if defined(WIN32)
DWORD WINAPI writepng(LPVOID lpParam);
#else
void* writepng(void * lpParam);
#endif
#if defined(WIN32)
DWORD WINAPI writejpg(LPVOID lpParam);
#else
void* writejpg(void * lpParam);
#endif
*/

void writejpg(cv::String imageFileName);
void writepng(cv::String imageFileName);

// loading a dds format under its compressed form (contrary to other formats (png, jpg) which are decompressed)
bool pg_load_compressed_photo(string *fileName, int width, int height, int indConfiguration);

// TEXTURE LOADING
bool pg_loadTexture3D(pg_TextureData* texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter);
bool pg_loadTexture2D(pg_TextureData* texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter);

/// NON THREADED LOAD CAMERA FRAME
void loadCameraFrame(bool initial_capture, int IPCam_no);
void pg_initCameraFrameTexture(Mat *video_frame);
void pg_openCameraCaptureAndLoadFrame(void);
void pg_releaseCameraCapture(void);
void pg_initWebcamParameters(void);

// particle initialization from photography
bool generateParticleInitialPosColorRadiusfromImage(string fileName, int indConfiguration); // 2 texture IDs one for pos/speed, the other one for color/radius

// particle acceleration shift from photography
bool storeParticleAccelerationfromImage(string fileName); 

// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
void pg_initMovieFrameTexture(Mat *video_frame);

#if defined(WIN32)
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam);
#else
void* pg_initVideoMoviePlayback(void * lpParam);
#endif
void* pg_initVideoMoviePlayback_nonThreaded(string * fileName);

#if defined(WIN32)
DWORD WINAPI pg_movieLoop(LPVOID lpParam);
#else
void* pg_movieLoop(void * lpParam);
#endif

// TEXTURE BUFFER MANAGEMENT
void diaporama_random(void);
void diaporama_slide(int slideNo);
void pg_launch_diaporama(int slideNo);
bool pg_update_diaporama(void);

void pg_CATable_values(GLuint textureID, GLubyte * data_table, int width, int height);

void pg_AddDirectoryToDiaporamas(int indConfiguration, fs::directory_entry dir_entry);
void pg_AddFilesToDiaporamas(int indConfiguration, fs::directory_entry dir_entry);
void pg_initDiaporamas(void);
bool pg_addNewDiaporamas(int current_config, bool with_trace);
bool pg_reloadAllDiaporamas(int currentConfig, bool with_trace);
bool pg_loadAllDiaporamas(void);
bool pg_ReadInitalClipFramesTextures(void);
#endif