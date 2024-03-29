/*! \file dm-geom-objects.h
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-geom-objects.h
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
#ifndef DM_GEOM_OBJECTS_H
#define DM_GEOM_OBJECTS_H

typedef dm_SceneNode ** PPSceneNode;

extern dm_point3 *Origin;
extern dm_vector3 *NullVector;

enum StorageType{ DM_NO_FBO_STORAGE = 0 , DM_RENDERBUFFER_FBO_STORAGE , DM_TEXTURE_FBO_STORAGE , EmptyStorageType};
extern const char *StorageTypeString[EmptyStorageType + 1];

/// a subdivision surface
class dm_Texturecoord {
public:
  float      u, v;


  void print( FILE *file , int depth , int index );
  void print( FILE *file , int depth );
  void operator=(dm_Texturecoord& v);
  void operator*=(double d);
  void operator+=(dm_Texturecoord& ve);
  void operator*(double f);

  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				FloatBinOp oper ,
		      IntBinOp operI );
};

/////////////////////////////////////////////////////////
// SCENE COMPONENTS (GEOMETRICAL OR NOT)
// ANCESTOR CLASS OF ANY SCENE COMPONENT
/////////////////////////////////////////////////////////
class dm_Shader;
class dm_MaterialProperties;
class dm_Schedule;
class dm_SceneObject;

/// struct for remebering the scene object in the scene
struct strAllSceneObject {
  dm_SceneObject* curObj;
  strAllSceneObject* nextObj;
};

/// struct for remebering the texture defined in the scene
struct strAllTexture{
  dm_Texture* curTex;
  strAllTexture* nextTex;
};

//AllSceneObject theSceneObjects;

/*! 
 * \brief scene components (geometrical or not)
 * ancestor class or any scene component
 */ 
class dm_SceneObject {
 protected:
  char     *id;

 public:

  dm_SceneObject( void );
  virtual ~dm_SceneObject( void );

  /*! \name Group id_access
   *  \brief id access
   */
  // @{
  char *GetId( void );
  void SetId( const char *newId );
  // @}

  /*! \name Group vertex arrays
   *  \brief vertex arrays
   */
  // @{
  virtual void NewvertexArrayID( void );
  virtual void NewelementArrayID( void );
  virtual void NewvertexBufferObjectID( void );
  virtual GLuint GetvertexArrayID( void );
  virtual GLuint GetelementArrayID( void );
  virtual GLuint GetvertexBufferObjectID( void );
  virtual void SetvertexArrayID( GLuint alistID );
  virtual void SetelementArrayID( GLuint alistID );
  virtual void SetvertexBufferObjectID( GLuint alistID );
 // @}

  /*! \name Group display_list_only
   *  \brief display lists only
   */
  // @{
  virtual void DeletevertexArrayID( void );
  virtual void DeleteelementArrayID( void );
  virtual void DeletevertexBufferObjectID( void );
  // @}

  ///  display  
  virtual void display( DrawingMode mode , bool *invalidDL );
  virtual void bindVertexData( void );
  virtual bool IsGeometricalObject( void );

  virtual dm_Shader *GetMaterialShader( void );
  virtual dm_MaterialProperties *GetMaterial( void );
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
  void operator=(dm_SceneObject& h);
};

/*!
 * \brief class of display geometrical objects
 */
class dm_GeometricalObject: public dm_SceneObject {
 protected:
  /// contains a pointer to the shader geometrical data, (former Display List ID or VBOs)
  // associated with each window
  GLuint   vertexArrayID;
  GLuint   elementArrayID;
  GLuint   vertexBufferObjectID;

 public:
  dm_MaterialProperties *material;

  dm_GeometricalObject( void );
  virtual ~dm_GeometricalObject( void );

  /*! \name Group vertex array
   *  \brief vertex array
   */
  // @{
  void NewvertexArrayID( void );
  void NewelementArrayID( void );
  void NewvertexBufferObjectID( void );
  GLuint GetvertexArrayID( void );
  GLuint GetelementArrayID( void );
  GLuint GetvertexBufferObjectID( void );
  void SetvertexArrayID( GLuint alistID );
  void SetelementArrayID( GLuint alistID );
  void SetvertexBufferObjectID( GLuint alistID );
  // @}

  /*! \name Group display_lists_only
   *  \brief display lists only
   */
  // @ {
  void DeletevertexArrayID( void );
  void DeleteelementArrayID( void );
  void DeletevertexBufferObjectID( void );
  // @}

  dm_Shader * GetMaterialShader( void );
  dm_MaterialProperties *GetMaterial( void );

  /*! \name Group display
   *  \brief display
   */
  // @{
  /*!
   * \brief builds the display list and draws
   * \param mode						to be added
   *
   * It used for any type of node that needs to be
   * redrawn - even though for some type of nodes,
   * display lists are not built
   */
  virtual void bindVertexData( void );
  virtual void display( DrawingMode mode , bool *invalidDL );

  /*!
   * \brief draws: each geometrical object is in charge of implementing it
   * \param mode									to be added
   * \param invalidDL								to be added
   */
  virtual bool IsGeometricalObject( void );
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
  void operator=(dm_GeometricalObject&);

};

/*!
 * \brief mesh
 */
struct MeshFullVertex
{
  float X, Y, Z;        //Vertex
  float NX, NY, NZ;     //Normal
  float S0, T0, U0;     //Texcoord0
  float R, G, B, A;     //Color
};
struct MeshVertex
{
  float x, y, z;     //Normal
};

struct MeshNormal
{
  float nx, ny, nz;     //Normal
};

struct MeshTexturecoord
{
  float s0, t0, u0;     //Texcoord
};

struct MeshFace
{
  GLuint ind;             // Face
};

enum MeshLeadingComponent{ VertexMeshLeadingComponent = 0 , TextureCoordMeshLeadingComponent , NormalMeshLeadingComponent , EmptyMeshLeadingComponent };

class  dm_Mesh: public dm_GeometricalObject {
 public:

  int nb_normals;
  int nb_vertices;
  int nb_fullvertices;
  int nb_faces;
  int nb_texturecoords;
  
  dm_vector3  averageNormal;
  dm_point3   center;

  MeshFullVertex *pFullvertex;
  MeshNormal *pnormal;
  MeshTexturecoord *ptexturecoord;
  MeshFace *pface_vertex;
  MeshFace *pface_normal;
  MeshFace *pface_texturecoord;
  MeshVertex *pvertex;
  MeshLeadingComponent leadingComponent;

  dm_Mesh( void );
  virtual ~dm_Mesh( void );
  
  /*!
   * \brief parsing a mesh
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI );
  
  /*!
   * \brief reading a dm_Mesh
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
  void parse( int *fileDepth , int *p_c );
  void display( DrawingMode mode , bool *invalidDL );
  void bindVertexData( void );
  void print( FILE *file , int isLong , int depth );

  void update( dm_point3 *tabPoint );
  void update( void );

  /*!
   * \brief normal vector of a mesh
   * \param normal				to be added
   */
  void averageNormals( dm_vector3 *normal );

  /*!
   * \brief random location in a mesh
   * \param aPoint				to be added
   */
  float height( void );
  void operator=(dm_Mesh&);
};

/*!
 * \brief points
 */
struct PointsFullVertex
{
  float X, Y, Z;        //Vertex
  float S0, T0, U0;     //Texcoord0
};
struct PointsIndex
{
  GLuint i;
};
enum PointsLeadingComponent{ VertexPointsLeadingComponent = 0 , TextureCoordPointsLeadingComponent , EmptyPointsLeadingComponent };

class  dm_Points: public dm_GeometricalObject {
 public:

  int nb_vertices;
  int width;
  int height;
  
  PointsFullVertex *pFullvertex;
  PointsIndex *pIndices;
  MeshTexturecoord *ptexturecoord;
  MeshVertex *pvertex;
  PointsLeadingComponent leadingComponent;

  dm_Points( void );
  virtual ~dm_Points( void );
  
  /*!
   * \brief parsing a points
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI );
  
  /*!
   * \brief reading a dm_Points
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
  void parse( int *fileDepth , int *p_c );
  void display( DrawingMode mode , bool *invalidDL );
  void bindVertexData( void );
  void print( FILE *file , int isLong , int depth );

  void update( dm_point3 *tabPoint );
  void update( void );

  /*!
   * \brief normal vector of a points
   * \param normal				to be added
   */
  void averageNormals( dm_vector3 *normal );

  /*!
   * \brief random location in a points
   * \param aPoint				to be added
   */
  void operator=(dm_Points&);
};

/*!
 * \brief quad
 */
class  dm_Quad: public dm_GeometricalObject {
 public:
  dm_vector3  averageNormal;
  dm_point3   center;

  MeshFullVertex pFullvertex[4];

  dm_Quad( void );
  virtual ~dm_Quad( void );
  
  /*!
   * \brief parsing a quad
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI );
  
  /*!
   * \brief reading a dm_Quad
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
  void parse( int *fileDepth , int *p_c );
  void display( DrawingMode mode , bool *invalidDL );
  void bindVertexData( void );
  void print( FILE *file , int isLong , int depth );

  void update( dm_point3 *tabPoint );
  void update( void );

  /*!
   * \brief normal vector of a quad
   * \param normal				to be added
   */
  void averageNormals( dm_vector3 *normal );

  /*!
   * \brief random location in a quad
   * \param aPoint				to be added
   */
  float height( void );
  void operator=(dm_Quad&);
};

/*! 
 * \brief point
 */
class  dm_Point: public dm_GeometricalObject {
 public:
  dm_point3        center;
  GLfloat          color[ 4 ];
  float            width;
  /// matrix used to store the returned getParameter
  /// value for parameter "matrix"
  dm_ValScalar    *valScalar_screen_location;

  dm_Point( void );
  virtual ~dm_Point( void );

  /*!
   * \brief parsing a point
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI );

  /*!
   * \brief reading a dm_Point
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
  void parse( int *fileDepth , int *p_c , bool isOpeningTag );
  void display( DrawingMode mode , bool *invalidDL );
  void print( FILE *file , int isLong , int depth );

  float height( void );
  void operator=(dm_Point&);
};

/*!
 * \breif line
 */
class  dm_Line: public dm_GeometricalObject {
 public:
  dm_point3   p1;
  dm_point3   p2;
  GLfloat     color_p1[ 4 ];
  GLfloat     color_p2[ 4 ];
  float       width;

  dm_Line( void );
  virtual ~dm_Line( void );

 /*!
   * \brief parsing a line
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI );

  /*!
   * \brief reading a dm_Line
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
  void parse( int *fileDepth , int *p_c , bool isOpeningTag );
  void display( DrawingMode mode , bool *invalidDL );
  void print( FILE *file , int isLong , int depth );

  float height( void );
  void operator=(dm_Line&);
};

/*!
 * \brief strip
 */
enum StripType{ DM_TRIANGLE_STRIP = 0 , DM_LINE_STRIP , DM_QUAD_STRIP , EmptyStripType};
extern const char *StripTypeString[EmptyStripType + 1];
class  dm_Strip: public dm_GeometricalObject {
  MeshFullVertex *tabFullVertices;
  MeshVertex *tabVertices;
  MeshTexturecoord *tabTexturecoords;
  MeshNormal *tabNormals;
 public:
  int    nbVertices;
  int    nbTexCoords;
  int    nbNormals;
  StripType type;

  dm_Strip( void );
  virtual ~dm_Strip( void );

 /*!
   * \brief parsing a strip
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  /*!
   * \brief reading a dm_Strip
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
  void parse( int *fileDepth , int *p_c );
  void display( DrawingMode mode , bool *invalidDL );
  void print( FILE *file , int isLong , int depth );

/*   void update( float thetop_radius , float thebase_radius , float theheight ); */
  void operator=(dm_Strip&);
};

/*!
 * \brief frame buffer texture storage
 */
class dm_TextureStorage: public dm_GeometricalObject {
 public:
  /// variable carrier node           
  dm_SceneNode             *TextureStorageScalarNode;
  /// variable           
  dm_Scalar                *TextureStorageScalar;
  /// generation mode (frame buffer capture or bitmap)
  TextureGenerationMode     textureGenerationMode;
  /// storage of a rectangular subregion
  /// negative width or height correspond to the whole scree
  int x0 , y0 , width , height;

  dm_TextureStorage( void );
  virtual ~dm_TextureStorage( void );
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
  void operator=(dm_TextureStorage&);
};

/*!
 * \brief frame buffer object texture storage
 */
class dm_FBOStorage: public dm_GeometricalObject {
 public:
  /// variable carrier node           
  dm_SceneNode             *FBOStorageScalarNode;

  /// generation mode (frame buffer capture or bitmap)
  TextureGenerationMode     textureGenerationMode;

  /// variable           
  dm_Scalar                 *color_FBOStorageScalar;
  dm_Scalar                 *depth_FBOStorageScalar;
  dm_Scalar                 *stencil_FBOStorageScalar;

  /// colors used to reset the texture attached to a FBO
  GLfloat                    color_color[4];
  GLfloat                    depth_color[4];
  GLfloat                    stencil_color[4];

  /// FBO storage type: none or renderbuffer or texture
  StorageType                color_type;
  StorageType                depth_type;
  StorageType                stencil_type;

  /// FBO texture clearing
  bool                       color_cleared;
  bool                       depth_cleared;
  bool                       stencil_cleared;

  // FBO ID
  char                      *color_id;
  char                      *depth_id;
  char                      *stencil_id;
  GLuint                     frameBufferId;

  dm_FBOStorage( void );
  virtual ~dm_FBOStorage( void );
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  void setApplicationParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				 FloatBinOp oper ,
				 IntBinOp operI );
  void parse( int *fileDepth , int *p_c );
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
  void operator=(dm_FBOStorage&);
};

/*!
 * \brief frame buffer object texture storage
 */
class dm_FBDisplay: public dm_GeometricalObject {
 public:
  dm_FBDisplay( void );
  virtual ~dm_FBDisplay( void );
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
  void operator=(dm_FBDisplay&);
};

/*!
 * \brief frame buffer texture storage
 */
class dm_PixelReader: public dm_GeometricalObject {
 public:
  /// storage of a pixel
  int x , y;
  int size_x, size_y;
  int offset_x, offset_y;

  dm_PixelReader( void );
  virtual ~dm_PixelReader( void );
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
  void operator=(dm_PixelReader&);
};

#endif
