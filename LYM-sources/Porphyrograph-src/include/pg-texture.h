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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

// data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).

// additional contexts for threading transfer to GPU
#define PG_MAX_NUMBER_OF_OPENGL_CONTEXTS  (4)

enum TextureEncoding { pg_enum_JPG_TEXTURE = 0, pg_enum_PNG_TEXTURE, pg_enum_PNGA_TEXTURE, pg_enum_PNG_GRAY_TEXTURE, pg_enum_PNGA_GRAY_TEXTURE, pg_enum_RGB_TEXTURE, pg_enum_RAW_TEXTURE, pg_enum_EmptyTextureEncoding };
enum pg_TextureFormat { pg_enum_byte_tex_format = 0, pg_enum_float_tex_format, pg_enum_Empty_TextureFormat };
enum pg_Texture_Usages {
	pg_enum_Texture_master_mask = 0, pg_enum_Texture_mesh, pg_enum_Texture_sensor, pg_enum_Texture_logo,
	pg_enum_Texture_brush, pg_enum_Texture_noise, pg_enum_Texture_particle, pg_enum_Texture_splat_particle, pg_enum_Texture_part_init,
	pg_enum_Texture_part_acc, pg_enum_Texture_pixel_acc, pg_enum_Texture_repop_density, pg_enum_Texture_multilayer_master_mask
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE DATA MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
		texture_usage = pg_enum_Texture_brush;
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP FRAME MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////


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
	// due to global storage variables pg_imgPhotoCompressedFormat, pg_imgPhotoCompressedBitmap, pg_imgPhotoBGRInit, pg_imgPhotoRGB;
	bool pg_loadClipFrames(string* fileName, int width, int height, int indScenario);
	bool pg_toGPUClipFrames(int w, int h, GLenum texturefilter, int indScenario);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// OPENCV MATRICES TO STORE IMAGES
extern cv::Mat pngImgMatRGBInitial;
extern cv::Mat jpgImgMatRGBInitial;

////////////////////////////////////////////////////////////////////
// PARTICLES TEXTURE
////////////////////////////////////////////////////////////////////
// CURVE PARTICLES TEXTURE
extern GLuint pg_curve_particle_2D_texID[PG_MAX_SCENARIOS];
// blurred disk texture
extern std::vector<GLuint> pg_blurredDisk_texture_2D_texID[PG_MAX_SCENARIOS];


////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
extern ClipFramesData** pg_ClipFrames_buffer_data[PG_MAX_SCENARIOS];

////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
////////////////////////////////////////////////////////////////////
// clips are made of sequences of images loaded in GPU
extern int pg_nbClips[PG_MAX_SCENARIOS];
extern int* pg_nbCompressedClipFramesPerFolder[PG_MAX_SCENARIOS];
extern int pg_nbCompressedClipFrames[PG_MAX_SCENARIOS];

// CLIP TEXTURES
extern int* pg_firstCompressedClipFramesInFolder[PG_MAX_SCENARIOS];

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_Master_Mask_texID[PG_MAX_SCENARIOS];
extern GLuint pg_Master_Multilayer_Mask_texID[PG_MAX_SCENARIOS];

extern GLuint pg_Pen_texture_3D_texID[PG_MAX_SCENARIOS];

extern GLuint pg_Noise_texture_3D[PG_MAX_SCENARIOS];
extern GLuint pg_Pixel_Noise_texture_3D[PG_MAX_SCENARIOS];
extern std::vector<GLuint>  pg_RepopDensity_texture_texID[PG_MAX_SCENARIOS];

extern std::vector<GLuint> pg_particle_initial_pos_speed_texID[PG_MAX_SCENARIOS];
extern std::vector<GLuint> pg_particle_initial_color_radius_texID[PG_MAX_SCENARIOS];
extern std::vector<GLuint> pg_particle_acc_texID[PG_MAX_SCENARIOS];
extern std::vector<GLuint> pg_pixel_acc_texID[PG_MAX_SCENARIOS];

extern GLuint Sensor_texture_rectangle[PG_MAX_SCENARIOS];

// CA TABLE TEXTURE
extern GLubyte* pg_CATable;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// OPENGL CONTEXTS FOR THREADS MANAGEMENT
// int GetFreeOpenGLContext(void);
// int GetFreeOpenGLContextTVW(void);

// TEXTURE INITIALIZATION
bool pg_loadAllTextures( void );

// TEXTURE GENERATION
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY );

// IMAGE FILE WRITING (pg_enum_JPG_TEXTURE OR PNG) (THREADED) 
void pg_writejpg(cv::String imageFileName);
void pg_writepng(cv::String imageFileName);

// TEXTURE LOADING
bool pg_loadTexture2D(pg_TextureData* texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter);

// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
void pg_initMovieFrameTexture(Mat *video_frame);

// TEXTURE BUFFER MANAGEMENT

void pg_CATable_values(GLuint textureID, GLubyte * data_table, int width, int height);

bool pg_ReadInitalClipFramesTextures(void);
void pg_parseScenario_Textures(std::ifstream& scenarioFin, int indScenario);


#endif