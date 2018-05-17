/*! \file dm-material.h
 *  \brief the header for classes ( \a dm_MaterialProperties , \a dm_ColorStrip )
 * and structures ( \a EnvMode, \a Finish, \a TextureEncoding, \a Interior, \a TextureGenerationMode,
 *  \a Texturing )
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-material.h
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

#ifndef DM_MATERIAL_H
#define DM_MATERIAL_H

#define     NULL_ID   0

enum Finish{ AMBIENT_RT = 0, BRILLIANCE_RT , DIFFUSE_RT , REFLECTION_RT , SPECULAR_RT , ROUGHNESS_RT , PHONG_RT , EmptyFinish };
enum Interior{ IOR_RT = 0, CAUSTICS_RT , FADE_DISTANCE_RT , FADE_POWER_RT , EmptyInterior };

extern GLfloat white_transparent[4];
extern GLfloat black_transparent[4];
extern GLfloat white_opaque[4];
extern GLfloat black_opaque[4];

enum EnvMode{ DECAL , MODULATE , REPLACE , BLEND , BLEND_FRAGMENT , BLEND_CUMULATIVE , EmptyEnvMode };
extern const char *EnvModeString[EmptyEnvMode + 1];

enum Texturing{ DM_DIRECT_TEXTURING , DM_SHADERSOURCE_TEXTURING , EmptyTexturing };
extern const char *TexturingString[EmptyTexturing + 1];

enum TextureEncoding{ JPG = 0 , PNG , PNGA , PNG_GRAY , PNGA_GRAY , RGB , RAW , EmptyTextureEncoding };
extern const char *TextureEncodingString[EmptyTextureEncoding + 1];

enum TextureFormat{ byte_tex_format = 0 , float_tex_format , depth_tex_format ,  EmptyTextureFormat };
extern const char *TextureFormatString[EmptyTextureFormat + 1];

enum TextureFilter{ linear_filter = 0 , nearest_filter , EmptyTextureFilter };
extern const char *TextureFilterString[EmptyTextureFilter + 1];

enum TextureGenerationMode{ DM_BITMAP_TEXTURE = 0 , DM_FRAME_BUFFER_CLEARED , DM_FRAME_BUFFER_UNCLEARED , DM_VARIABLE_TEXTURE , EmptyTextureGenerationMode };
extern const char *TextureGenerationModeString[EmptyTextureGenerationMode + 1];

class dm_Shader;
class dm_VertexShader;

class dm_SceneNode;
#ifdef DM_HAVE_FFMPEG
class dm_Video;
#endif

class dm_ColorStripe {
 public:
  GLfloat  color[4];
  float    length;
  dm_ColorStripe( void );  
  ~dm_ColorStripe(void);

  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  void print( FILE *file , int depth );
  void operator=(dm_ColorStripe& p);
};

class dm_Subdivision;

/*!
 * \brief material properties
 */
class dm_MaterialProperties {
 public:
  bool    hasTexture;
  bool    hasVideo;
  bool    hasColor;
  bool    hasMaterial;
  bool    hasMaterialRayTrace;
  bool    hasShader;

  /*! \name Group color
   *  \brief color
   */
  // \{
  GLfloat    color[4];
  TextureEncoding  color_encoding;
  // \}

  /*! \name material
   *  \brief material
   */
  // \{
  GLfloat Ambient[4];
  GLfloat Diffuse[4];
  GLfloat Specular[4];
  GLfloat Emission[4];
  GLfloat Shininess;
  // \}

  dm_Texture  * materialTexture;
#ifdef DM_HAVE_FFMPEG
  dm_Video    * materialVideo;
#endif

  /// shader
  dm_Shader *shader;

  dm_MaterialProperties( void );
  ~dm_MaterialProperties(void);

/*   GLuint dm_MaterialProperties::GetTextureNo( void ); */
/*   void dm_MaterialProperties::SetTextureNo( GLuint aTextureNo ); */

  /*! 
   * \brief parsing material properties
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  /*!
   * \brief reading a dm_MaterialProperties
   * \param attribute							to be added
   * \param dataType						to be added
   * \param valDouble						to be added
   * \param valInt									to be added
   * \param valBool							to be added
   * \param valString							to be added
   * \param tableOrMatrixSize		to be added
   * \param valScalars						to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );

  /*!
   * \brief parse
   * \param fileScene					to be added
   * \param p_c							to be added
   * \param ID								to be added
   * \param XMLtag					to be added
   * \param subdivision				to be added
   *
   * material properties are allocated when a solid is created
   * the same material properties can be parsed several
   * times as for instance a texture + a bump texture
   */
  void parse( int *fileDepth , int *p_c , 
	      char *ID , dm_XMLTag *XMLtag ,
	      dm_Subdivision * subdivision );

  /*!
   * \brief printing material properties
   * \param file				to be added
   * \param depth		to be added
   */
  void print( FILE *file , int depth );

 /*!
  * \brief initializes the material properties of an object
  * \param mode					to be added
  * \param invalidDL			to be added
  *
  * initializes the material properties of an object
  * if the texture is loaded, the display list must
  * be invalidated and reloaded at the next frame
  */
  /* void textureModeCall( DrawingMode mode , bool *invalidDL ); */
  /* void close( DrawingMode mode ); */

  /*!
   * \brief display video
   * \param mode			to be added
   *
   * performed once for every display in the dm_SceneNode::display function
   * is made during the pre display phase
   * post display only calls the display list
   */
  void displayVideo( DrawingMode mode );

  /// updates film-based textures 
  void update( void );

  void operator=(dm_MaterialProperties& p);
};

#endif
