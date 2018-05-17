/*! \file \a dm-texture.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-texture.h
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

#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H

#define UINT unsigned int
#define LPSTR char *

typedef  char *     Pchar;

/// IMAGE DATA
/// This stores jpeg data
struct tImageJPG
{
	int rowSpan;
	int sizeX;
	int sizeY;
	unsigned char *data;
};

/// This stores png data
struct tImagePNG
{
	int bytesPerPixel;
	int sizeX;
	int sizeY;
	int sizeZ;
	unsigned char *data;
};

/// This stores raw data
struct tImageRAW
{
	int            bytesPerPixel;
	int            sizeX;
	int            sizeY;
	int            sizeZ;
	float         *dataFloat;
	unsigned char *dataByte;
};

// From http://stackoverflow.com/questions/1821806/how-to-encode-png-to-buffer-using-libpng
// How to encode PNG to buffer using libpng?
// stackoverflow website

/* Pixels in this bitmap structure are stored as BGR. */
typedef struct _RGBPixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} RGBPixel;

/* Structure for containing decompressed bitmaps. */
typedef struct _RGBBitmap {
    RGBPixel *pixels;
    size_t width;
    size_t height;
    size_t bytewidth;
    unsigned char bytes_per_pixel;
} RGBBitmap;

/// LOAD IMAGES
/* void loadJpegImage(char *fichier ,  */
/* 		   unsigned char image[] , */
/* 		   int textureWidth , int textureHeight ); */

/*!
 * \brief This loads a jpeg file and returns its data
 * \param cinfo							to be added
 * \param pImageData			to be added
 *
 * 	This decodes the jpeg and fills in the tImageJPG structure
 */
// void DecodeJPG(jpeg_decompress_struct* cinfo, tImageJPG *pImageData);

/*!
 * \brief loads a texture into OpenGL from a file (IE, "image.jpg")
 * \param filename			the image file
 */
// tImageJPG *LoadJPG(const char *filename);

/*!
 * \brief This loads the PNG file and returns its data in a tImagePNG struct
 * \param strFileName					to be added
 * \param sizeX								to be added
 * \param sizeY								to be added
 * \param theTextureEncoding			to be added
 *
 * this function reads a texture in order to return its size
 * pointer to texture data is freed before returning
 */
/* void readSizeTexturePNG(char * strFileName, int * sizeX, int * sizeY, */
/* 			TextureEncoding theTextureEncoding); */

/*!
 * \param png_ptr				to be added
 * \param data					to be added
 * \param length					to be added
 *
 * This function replaces the C standard file I/O function fread(), which 
 * libPNG would use by default. But we want to use C++ I/O here.
 * This stems from the PortaLib source code - see above.
 */
/* void pngReadData(png_structp png_ptr, png_bytep data, png_size_t length); */

/*!
 * \brief This is the interesting function here. It sets up a PNG image as a texture.
 * \param fname								to be added
 * \param theTextureEncoding			to be added
 */
/* tImagePNG *loadPngTex(Pchar fname , TextureEncoding theTextureEncoding , bool invert ); */

/*!
 * \brief Choose the right image loading function switch the image type
 * \param strFileName					to be added
 * \param theTextureEncoding			to be added
 * \param sizeX								to be added
 * \param sizeY								to be added
 * \param bytesPerPixel				to be added
 * \param bitmap							to be added
 */
/* void LoadImageData(Pchar strFileName, TextureEncoding theTextureEncoding, int * sizeX, int * sizeY, int * bytesPerPixel, unsigned char ** bitmap, bool invert); */

/*!
 * \brief This creates a texture in OpenGL that we can use as a texture map
 * \param width							to be added
 * \param height							to be added
 * \param data							to be added
 * \param bytesPerPixel			to be added
 */

/// SAVE IMAGE
int writepng(char *filename, char *description,
	     int x, int y, int width, int height);

int writejpg(char *filename, char *description,
	     int x, int y, int width, int height, int compression);

/// IMAGE UTILITIES

void CheckImageDimensions( int sizeX , int sizeY , LPSTR strFileName);

/// the class that contains and manages texture data
/// the texture pointers are managed by the class dm_Composition
/// that manages the global data for a composition

class dm_Texture {
  int        textureNo;
  // pointer to the texture no of a variable in case of texture
  // defined by a variable
  int       *ptTextureNo;
 public:
  /*! \name handle_texture
   *  \brief 
   * an environment map shares the same (OpenGL and) VC ID
   * environmentMapTextureNo between all its textures and videos
   * for this reason we need to store this shared ID so that we can update
   * the texture if it is changed some time
   */
  int        environmentMapTextureNo;
  char       variableNodeName[XMLAttrValLength];
  char       variableName[XMLAttrValLength];

  TextureEncoding  encoding;
  float           textureTile_s;
  float           textureTile_t;
  float           textureTile_u;
  float           textureOffset_s;
  float           textureOffset_t;
  float           textureOffset_u;
  bool            textureRepeat_s;
  bool            textureRepeat_t;
  bool            textureRepeat_u;
  EnvMode         textureEnvMode;
  bool            mipmapped;
  bool            rectangle;
  int             nb_attachments;  // values set in the case of texture variable
  int             attachment;      // value set in case of multiple attachment
                                   // texture variable
  TextureFormat   texture_format;
  TextureFilter   texture_min_filter;
  TextureFilter   texture_max_filter;
  GLenum          textureTarget;

  /*! \name Group texture
   *  \brief texture
   */
  // \{
  TextureGenerationMode textureGenerationMode;
  // \}

  dm_Texture( void );
  void init( void );
  ~dm_Texture(void);

/*   GLuint dm_Texture::GetTextureNo( void ); */
/*   void dm_Texture::SetTextureNo( GLuint aTextureNo ); */

  /*! 
   * \brief parsing material properties
   * \param XMLtag			to be added
   * \param atRunTime			to be added
   * \param videoTexture		to be added
   * \param oper			to be added
   * \param operI			to be added
   * \return returns true if geometry is modified (texture coordinate modification)
   */

  bool setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      bool videoTexture ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  
  /*!
   * \brief reading a dm_Texture
   * \param attribute			to be added
   * \param dataType			to be added
   * \param valDouble			to be added
   * \param valInt			to be added
   * \param valBool			to be added
   * \param valString			to be added
   * \param tableOrMatrixSize		to be added
   * \param valScalars			to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );

  int GetTextureNo( void );
  void SetTextureNo( int texNo );

  void ResolveReferences( dm_SceneNode *rootNode );

  void LoadOrGenerateTexture( char *textureName ,
			      int *fileDepth , int *p_c ,
			      char *ID , dm_XMLTag *XMLtag ,
			      bool invert );

  /// PRINTING MATERIAL PROPERTIES
  int textureDimension( GLenum textureTarget );

  /*!
   * \brief printing material properties
   * \param file				to be added
   * \param depth		to be added
   */
  void print( FILE *file , int depth );
  
  /*!
   * \brief binds the texture with its ID
   * \param invalidDL				to be added
   */
  void bindFrameBufferTexture( bool *invalidDL , bool customSize , int x0 , int y0 , int width , int height );

  /*!
    * \brief initializes the material properties of an object
    * \param mode					to be added
    * \param invalidDL			to be added
	*
	* initializes the material properties of an object
    * if the texture is loaded, the display list must
    * be invalidated and reloaded at the next frame
    */
  void textureMode( bool *invalidDL );
  void close( void );


  void operator=(dm_Texture& p);
};

extern GLbyte stb__arial_10_usascii_x[95];
extern GLubyte stb__arial_10_usascii_y[95];
extern GLubyte stb__arial_10_usascii_w[95];
extern GLubyte stb__arial_10_usascii_h[95];
extern GLubyte stb__arial_10_usascii_s[95];
extern GLubyte stb__arial_10_usascii_t[95];
extern GLubyte stb__arial_10_usascii_a[95];

extern GLbyte stb__arial_11_usascii_x[95];
extern GLubyte stb__arial_11_usascii_y[95];
extern GLubyte stb__arial_11_usascii_w[95];
extern GLubyte stb__arial_11_usascii_h[95];
extern GLubyte stb__arial_11_usascii_s[95];
extern GLubyte stb__arial_11_usascii_t[95];
extern GLubyte stb__arial_11_usascii_a[95];

extern GLbyte stb__arial_12_usascii_x[95];
extern GLubyte stb__arial_12_usascii_y[95];
extern GLubyte stb__arial_12_usascii_w[95];
extern GLubyte stb__arial_12_usascii_h[95];
extern GLubyte stb__arial_12_usascii_s[95];
extern GLubyte stb__arial_12_usascii_t[95];
extern GLubyte stb__arial_12_usascii_a[95];

extern GLbyte stb__arial_12_usascii_x[95];
extern GLubyte stb__arial_12_usascii_y[95];
extern GLubyte stb__arial_12_usascii_w[95];
extern GLubyte stb__arial_12_usascii_h[95];
extern GLubyte stb__arial_12_usascii_s[95];
extern GLubyte stb__arial_12_usascii_t[95];
extern GLubyte stb__arial_12_usascii_a[95];

extern GLbyte stb__arial_13_usascii_x[95];
extern GLubyte stb__arial_13_usascii_y[95];
extern GLubyte stb__arial_13_usascii_w[95];
extern GLubyte stb__arial_13_usascii_h[95];
extern GLubyte stb__arial_13_usascii_s[95];
extern GLubyte stb__arial_13_usascii_t[95];
extern GLubyte stb__arial_13_usascii_a[95];

extern GLbyte stb__arial_14_usascii_x[95];
extern GLubyte stb__arial_14_usascii_y[95];
extern GLubyte stb__arial_14_usascii_w[95];
extern GLubyte stb__arial_14_usascii_h[95];
extern GLubyte stb__arial_14_usascii_s[95];
extern GLubyte stb__arial_14_usascii_t[95];
extern GLubyte stb__arial_14_usascii_a[95];

extern GLbyte stb__arial_15_usascii_x[95];
extern GLubyte stb__arial_15_usascii_y[95];
extern GLubyte stb__arial_15_usascii_w[95];
extern GLubyte stb__arial_15_usascii_h[95];
extern GLubyte stb__arial_15_usascii_s[95];
extern GLubyte stb__arial_15_usascii_t[95];
extern GLubyte stb__arial_15_usascii_a[95];

extern GLbyte stb__arial_16_usascii_x[95];
extern GLubyte stb__arial_16_usascii_y[95];
extern GLubyte stb__arial_16_usascii_w[95];
extern GLubyte stb__arial_16_usascii_h[95];
extern GLubyte stb__arial_16_usascii_s[95];
extern GLubyte stb__arial_16_usascii_t[95];
extern GLubyte stb__arial_16_usascii_a[95];

extern GLbyte stb__arial_17_usascii_x[95];
extern GLubyte stb__arial_17_usascii_y[95];
extern GLubyte stb__arial_17_usascii_w[95];
extern GLubyte stb__arial_17_usascii_h[95];
extern GLubyte stb__arial_17_usascii_s[95];
extern GLubyte stb__arial_17_usascii_t[95];
extern GLubyte stb__arial_17_usascii_a[95];

extern GLbyte stb__arial_18_usascii_x[95];
extern GLubyte stb__arial_18_usascii_y[95];
extern GLubyte stb__arial_18_usascii_w[95];
extern GLubyte stb__arial_18_usascii_h[95];
extern GLubyte stb__arial_18_usascii_s[95];
extern GLubyte stb__arial_18_usascii_t[95];
extern GLubyte stb__arial_18_usascii_a[95];

#endif
