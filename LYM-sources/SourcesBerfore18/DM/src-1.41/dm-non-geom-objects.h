/*! \file dm-non-geom-objects.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-non-geom-objects.h
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

#ifndef DM_NON_GEOM_OBJECTS_H
#define DM_NON_GEOM_OBJECTS_H

enum ShaderTextureTarget{ DM_VERTEX_TEXTURE = 0 , DM_GEOMETRY_TEXTURE = 1 , DM_FRAGMENT_TEXTURE = 2 , EmptyShaderTextureTarget = 3};
extern const char *ShaderTextureTargetString[EmptyShaderTextureTarget + 1];

enum TransformationType{ ATranslation = 0, ARotation , AQuaternion , AScale , AMatrix , EmptyTransformation };

extern const char *TransformationTypeString[EmptyTransformation + 1];

enum ViewvolumeType{ DM_FRUSTUM = 0, DM_ORTHO, EmptyViewvolumeType };

enum ViewPointType{ DM_ORIENTABLE_CAMERA = 0, EmptyViewPointType };

extern const char *ViewvolumeTypeString[EmptyViewvolumeType + 1];

/*!
 * \brief non displayable gometrical objects
 *
 * non displayable gometrical objects,
 * ancestor class of transformation, camera
 */
class dm_NonGeometricalObject: public dm_SceneObject {
 protected:
 public:
  dm_NonGeometricalObject( void );
  virtual ~dm_NonGeometricalObject( void );

  /*! \name Group display_list
   *  \brief display lists
   */
  // @{
  virtual void NewvertexArrayID( void );
  virtual GLuint GetvertexArrayID( void );
  virtual void NewelementArrayID( void );
  virtual GLuint GetelementArrayID( void );
  virtual void SetvertexArrayID( GLuint alistID );
  virtual void SetelementArrayID( GLuint alistID );
  virtual void DeletevertexArrayID( void );
  virtual void DeleteelementArrayID( void );
 // @}

  /*! \name Group display
   *  \brief display
   */
  // @{
  virtual void bindVertexData( void );
  virtual void display( DrawingMode mode , bool *invalidDL );
  // @}

  virtual void print( FILE *file , int isLong , int depth );
  virtual void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  virtual bool getParameters( char *attribute , 
			      DataType * dataType ,
			      double * valDouble ,
			      int * valInt ,
			      bool * valBool ,
			      char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars );
  void setCommonParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  void operator=(dm_NonGeometricalObject& h);
};

class  dm_ShaderSourceObject: public dm_NonGeometricalObject {
 public:
  /*! \name Group textures_painting
   *  \brief textures used for painting the shaderSource
   */
  // \{
  char                      **fileName;
  char                      **encoding;
  int                        nb_shaderTextures;

  bool                       message_display;

  int                       *textureIndex;
  ShaderTextureTarget       *textureTarget;
  dm_Texture               **materialTextures;
  // int                       *textureNo;
  // TextureEncoding           *encodings;
  // TextureGenerationMode     *textureGenerationModes;
  // bool                      *mipmappeds;
  // int                       *dimensions;
#ifdef DM_HAVE_FFMPEG
  dm_Video                 **materialVideo;
#endif
  // \}

  GLuint                     shaderProgram;

  /*! \name Group uniform_var
   * \brief uniform variables that are passed to the shaders
   */
  // \{
  char                      **vertexUniformVarName;
  int                       *vertexUniformVarSize;
  dm_Table                  **vertexUniformVarValue;
  int                       nb_vertexUniformVars;
  
  char                      **geometryUniformVarName;
  int                       *geometryUniformVarSize;
  dm_Table                  **geometryUniformVarValue;
  int                       nb_geometryUniformVars;

  char                      **fragmentUniformVarName;
  int                       *fragmentUniformVarSize;
  dm_Table                  **fragmentUniformVarValue;
  int                       nb_fragmentUniformVars;
  // \}

  /*! \name Group working_var
   *  \brief working variables
   */
  // \{
  int                        on;
  int                        active_texture;
  // \}

  // number of attachments in case of multiple rendering targets
  int                        nb_attachments;

  dm_ShaderSourceObject( void );
  virtual ~dm_ShaderSourceObject( void );

  /* \brief parsing a shadersources
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

    /*!
   * \brief reading a dm_ShaderSourceObject
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
  void parse( int *fileDepth , int *p_c );
  void print( FILE *file , int isLong , int depth );

  void operator=(dm_ShaderSourceObject&);
};

/*!
 * \brief camera
 */
class dm_Viewpoint: public dm_NonGeometricalObject {
 public:
  /// rotation around the vertical axis (Y) in degrees
  float                      pitch_head;
  /// rotation around the horizontal axis orthogonal to the motion direction (Z)
  /// in degrees
  float                      yaw_head;
  /// current location
  dm_point3                 *currentPointAbsolute;
  /// type
  ViewPointType              type;
  /// vertical vector for lookat cameras
  dm_vector3                vertical;

  dm_Viewpoint( void );
  virtual ~dm_Viewpoint( void );

  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );
  void print( FILE *file , int isLong , int depth );
  void display( DrawingMode mode , bool *invalidDL );
  void operator=(dm_Viewpoint&);
};

/*!
 * \brief microphone
 */
class dm_Hearpoint: public dm_NonGeometricalObject {
 public:
  /// whether the hearpoint is listener
  bool                      active;

  dm_Hearpoint( void );
  virtual ~dm_Hearpoint( void );
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );
  void print( FILE *file , int isLong , int depth );
  void display( DrawingMode mode , bool *invalidDL );
  void operator=(dm_Hearpoint&);
};

/*! 
 * \brief transformation
 */
class dm_Transformation: public dm_NonGeometricalObject {
 public:
  TransformationType            type;
  
  /// rotation axis or translation vector
  dm_vector3                    aVector;

  /// quaternion
  dm_vector3                    aQuatIm;
  float                         aQuat_w;
  float                         aAngle;
  bool                          rotationUpdated;

  // matrix transformation: stores the OpenGL matrix
  float                         transformationMatrix[16];

  /// matrix used to store the returned getParameter
  /// value for parameter "matrix"
  dm_ValScalar                 *valScalarMatrix;

  dm_Transformation( void );
  virtual ~dm_Transformation( void );

  /*! 
   * \brief parsing a transformation
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  /*!
   * \brief reading a dm_Transformation
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
  void parse( int *fileDepth , int *p_c , 
	      dm_SceneNode *sourceNode );

  /// tells whether it has to be displayed
  bool hasDrawablePrimitive( void );

  /*! 
   * \brief inversee transformation
   *  \param mode			to be added
   *
   * function used for inverse transformation from
   * the viewpoint to the root node: inverse transformation 
   * application whatever the rendering mode
   */
  void drawInverseTransformation( DrawingMode mode );

  /*!
   * \brief transformation position display
   * \param mode			to be added
   */
  void preDisplay( DrawingMode mode );

  /*!
   * \brief transformation geometry display
   * \param mode					to be added
   * \param invalidDL				to be added
   *
   */
  void display( DrawingMode mode , bool *invalidDL );

  /// path transformation updating
  void update( dm_SceneNode *carrierNode );

  /*!
   * \brief printing a transformation
   * \param file				to be added
   * \param depth		to be added
   */
  void print( FILE *file , int isLong , int depth );

  /// oepnGL rotation to quaternion transformation
  void AngleAxisToQuaternion( void );
  void QuaternionToAngleAxis( void );

  /// rotation axis or quaternion normalization
  void normalize( void );
  void operator=(dm_Transformation&);
};

/*! \name Group user_attri_class
 *   \brief user and attribute classes
 */
// @{
/*!
 * \brief perspective projection view volume
 */
class dm_Frustum {
 public:
  GLfloat xMin;
  GLfloat xMax;
  GLfloat yMin;
  GLfloat yMax;
  GLfloat nearFrustum;
  GLfloat farFrustum;
  dm_Frustum();
  virtual ~dm_Frustum( void );
  void update( GLfloat height , GLfloat width );

  void operator=(dm_Frustum& e);
};

/*!
 * \brief orthogonal projection view volume
 */
class dm_Ortho {
 public:
  GLfloat xMin;
  GLfloat xMax;
  GLfloat yMin;
  GLfloat yMax;
  GLfloat nearOrtho;
  GLfloat farOrtho;
  dm_Ortho();
  virtual ~dm_Ortho( void );
  void update( GLfloat height , GLfloat width );

  void operator=(dm_Ortho& e);
};

/*!
 * \brief user
 */
class dm_User: public dm_NonGeometricalObject {
 public:
  /// stereo user with two eyes, non alternative stereo is implemented by two users
  bool                       alternate_stereo_viewer;
  

  /*! \name Group view_vol
   *  \brief view volume
   */
  // \{
  ViewvolumeType             viewvolume_type;
  dm_Ortho                  *ortho;
  dm_Frustum                *frustum;
  GLfloat                    bg_color[4];
  // \}

  float                      start_x;
  float                      start_y;
  float                      start_z;
  float                      start_yaw;
  float                      start_pitch;
  // manual moving steps
  // float                      manualMoveLinearStep;
  // float                      manualMoveHeightStep;
  // ------ amplitude of how mouse motion modifies view direction ----- //
  float                      rotation_amplitude_yaw;
  float                      rotation_amplitude_pitch;
  // \}

  /*! \name Group viewpoint
   *  \brief viewpoints
   */
  // \{
  int                        default_viewpoint;
  int                        default_viewpoint_stereo_left;
  dm_SceneNode              *default_viewpoint_node;
  dm_SceneNode              *default_viewpoint_stereo_left_node;

  int                        nbViewpointShots;
  int                        indCurrentViewpoint;
  float                     *shotIni;
  float                     *shotEnd;
  int                       *shotIndViewpoint;
  int                       *shotIndViewpointStereoLeft;
  int                        manualViewpointControl;
  // \}

  dm_User ();
  virtual ~dm_User( void );
  void setApplicationParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				 FloatBinOp oper ,
				 IntBinOp operI );

  /*! 
   * \brief parsing a user
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  /*!
   * \brief reading a dm_User
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
  void parse( int *fileDepth , int *p_c );
  void display( DrawingMode mode , bool *invalidDL );

  void operator=(dm_User& h);
  void print( FILE *file , int isLong , int depth );
};


#endif
