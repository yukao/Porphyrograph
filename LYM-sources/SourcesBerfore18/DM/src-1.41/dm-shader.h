/*! \file dm-shader.h
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-shader.h
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

#ifndef DM_SHADER_H
#define DM_SHADER_H

extern int    NbbumpMappingTextures;

#ifdef DM_HAVE_FFMPEG
class dm_Video;
#endif
 
unsigned long getFileLength(ifstream& file);
int loadshader(string fileName , GLuint aShader);
void printLog(GLuint obj);

class dm_VertexShader {
 public:
  GLuint   vertexProgram;
  char *vertex_program_name;

  dm_VertexShader( void );

  /*!
   * \brief loading shaders
   * \param type							to be added
   * \param nomFic						to be added
   * \param shaderSrc				to be added
   */
  void InitializeShader( char *nomFic  , dm_ShaderSourceObject *shaderSrc );
  void cleanExit(void);
};


class dm_GeometryShader {
 public:
  GLuint   geometryProgram;
  char *geometry_program_name;
  char *geometry_program_ouput_type;

  char* options[2] ;

  // \}
  
  dm_GeometryShader( void );

    /*!
   * \brief loading shaders
   * \param type							to be added
   * \param nomFic						to be added
   * \param shaderSource		to be added
   */
  void InitializeShader( char *nomFic ,  char *outType , int outMax,
			 dm_ShaderSourceObject   *shaderSource );
  void cleanExit(void);
};



class dm_FragmentShader {
 public:
  GLuint   fragmentProgram;
  char *fragment_program_name;

  dm_FragmentShader( void );

    /*!
   * \brief loading shaders
   * \param type							to be added
   * \param nomFic						to be added
   * \param shaderSource		to be added
   */
  void InitializeShader( char *nomFic , 
			 dm_ShaderSourceObject   *shaderSource );
  void cleanExit(void);
};

/// a shader 
class dm_Shader {
 public:
  char                        *vertex_program_name;
  char                        *geometry_program_name;
  char                        *fragment_program_name;
  char                        *geometry_program_output_type;
  int	                       geometry_max_vertex;

  dm_VertexShader             *vertexShader;
  dm_GeometryShader           *geometryShader;
  dm_FragmentShader           *fragmentShader;

  GLuint                       shaderProgram;

  GLuint                       sampler_nearest;
  GLuint                       sampler_linear;

  /*! \name Group stand_vertext_param
   *  \brief standard vertex parameters
   */
  // \{
  GLint                       projMatrixLoc;
  GLint                       viewMatrixLoc;
  GLint                       timeParam;
 
  GLint                       tangentParam;
  GLint                       diffuseParam;
  GLint                       specularParam;
  // \}


  /// specific vertex parameters
  GLint                       *uniformVertexParameters;

  /// specific vertex parameters
  GLint                       *uniformGeometryParameters;

  /// specific vertex parameters
  GLint                       *uniformFragmentParameters;
  GLint                       uniformMessageTextureIDParameter;
  GLint                       uniformFontTextureIDParameter;
  GLint                       messageTransparencyLoc;

  // texture parameters
  GLint                       fragmentDecalParamLoc;

  /*! \Group tex_vertex_param
   *  \brief texture vertex parameters
   */
  // \{
  int                          nbVertexTextureParam;
  GLint                      *vertexLookupTableParam;
  // \}

  /*! \Group tex_vertex_param
   *  \brief texture vertex parameters
   */
  // \{

  int                          nbGeometryTextureParam;
  GLint                      *geometryLookupTableParam;

  /*! \name Group texture_frag_param
   *  \brief texture fragment parameters
   */
  // \{
  int                          nbFragmentTextureParam;
  GLint                      *fragmentLookupTableParam;
  // \}

  // a pointer to the texture of the material properties
  dm_Texture                  **baseMaterialTexture;
    // int                          baseTextureNo;
    // int                         *basePtTextureNo;
    //  TextureEncoding              baseEncoding;
    // bool                         baseMipmapping;
    // int                          baseDimension;
    // TextureGenerationMode        baseGenerationMode;
#ifdef DM_HAVE_FFMPEG
  // a pointer to the video of the material properties
  dm_Video                    **baseMaterialVideo;
#endif
  dm_ShaderSourceObject        *shaderSource;

  char *id;

  dm_Shader( dm_Texture **materialTexture 
#ifdef DM_HAVE_FFMPEG
	     , dm_Video **materialVideo 
#endif
	     );
  void cleanExit( void );

  void SetId( char *newId );

 /*!
   * \brief parsing a shader
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

 /*!
   * \brief reading a dm_Shader
   * \param attribute								to be added
   * \param dataType							to be added
   * \param valDouble							to be added
   * \param valInt										to be added
   * \param valBool								to be added
   * \param valString								to be added
   * \param tableOrMatrixSize			to be added
   * \param valScalars							to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );

  void parse( int *fileDepth , int *p_c , 
	      dm_Subdivision * subdivision );

  void InitializeShader( void );

  void update( void );

  /*!
   * \brief enabling or disabling a shader
   * \param mat			to be added
   */
  void shader_enables( dm_MaterialProperties *mat );
  void shader_disables( void );
  void print( FILE *file , int depth );
  void operator=(dm_Shader& p);
};

#endif
