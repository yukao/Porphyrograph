/*! \file dm-lib.h
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-lib.h
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

#ifndef DM_LIB_H
#define DM_LIB_H

enum VertexType{ INITIAL_VERTEX = 0 , REPLACEMENT_VERTEX , SPLITTING_VERTEX , EmptyVertexType};

class dm_matrix44;

extern dm_matrix44 *identity_matrix44;
extern dm_matrix44 *null_matrix44;

extern float DegtoRad;
extern float RadtoDeg;
extern float M_PI2;

enum WalkingDirection{ INCREASING = 0, DECREASING };

/*! \name Group texture_ mode
 */
// \{
/// _Render : rendering,
/// _Jpg : framebuffer output to jpg file
/// _Png : framebuffer output to png file
/// _Svg : framebuffer output to svg file
enum DrawingMode{ _Render=0 , _Svg , _Png , _Jpg , _Video };
enum OrientationTexture{ Vide = 0, Verticale, Horizontale, MaxOrientationTexture };

enum TextureMode{ DM_NORMAL_TEXTURING = 0, DM_VERTICAL_TEXTURING , DM_RADIAL_TEXTURING , DM_NB_TEXTURING_MODES };
extern const char *TextureModeString[DM_NB_TEXTURING_MODES + 1];
// \}

class dm_vector2;
class dm_vector3;

/*! \name Group miscellaneous
 *  \biref miscellaneous functions
 */
// \{
void indent( FILE *file , int depth );

/*! \name Group random_value
 *  \brief  random value
 */
// @{
//
void generateRandomWeights( float *tabWeights ,
			    int numberOfComponents );

/*! 
 * \brief integer 
 * \param min			to be added
 * \param sup			to be added
 */
int generateRandomInt( int min , int sup );

/*!
 * \brief float
 * \param inf				to be added
 * \param sup			to be added
 */
float generateRandomFloat( float inf , float sup );
// @}

void alignToGrid( float *f );

/*! \name Group math_functions
 *  \brief Math functions
 */
// @{

/*! 
 * \brief redefinition of power function to accept non integer power 
 *  of negative values
 * \param x		to be added
 * \param y		to be added
 */
GLfloat powExt(GLfloat x , GLfloat exp);

/*!
 * \brief contour line tangent for revolution surfaces
 * \param alpha				to be added
 * \param tangNiv			to be added
 */
void tangentContourLine( float alpha , dm_vector3 &tangNiv );

/*! 
 * \brief transforme an angle into between 90 and -90 degres
 * \param angle		the input angle				
 */
void normaliseAngleLatitude( float *angle );

/*! 
 * \brief transforme an angle into between 180 and -180 degres
 * \param angle		the input angle				
 */
void normaliseAngleLongitude( float *angle );

/// conversion to power of 2 for texture dimensions
int smallestPOT( int x );
// @}

class dm_point3;

void LoadIdentityf( float *values );
void LoadRotationIdentityf( float *values );

/*! \name Group points_vectors
 *  \brief points and vectors
 */
// @{
class dm_point2 {
 public:
  GLfloat   x;
  GLfloat   y;
  int estSelectionne;
  dm_point2( void );
  dm_point2( GLfloat X , GLfloat Y );
  void operator=(dm_point2&);
  void operator+=(dm_vector2&);
  void operator+(dm_point2 &p);
  void middle( dm_point2 &p1 , dm_point2 &p2 );
};

class dm_point3 {
 public:
  GLfloat   x;
  GLfloat   y;
  GLfloat   z;
  int estSelectionne;
  dm_point3( void );
  dm_point3( GLfloat X , GLfloat Y , GLfloat Z );
  void operator=(dm_point3&);
  void operator+=(dm_vector3&);
  void operator+=(dm_point3&);
  void operator*=(float k);
  void operator+(dm_point3 &p);
  void update( GLfloat X , GLfloat Y , GLfloat Z );
  void middle( dm_point3 &p1 , dm_point3 &p2 );
  GLfloat distance( dm_point3 &p1 );
  GLfloat distancexz( dm_point3 &p1 );
  void canonizeAnglexz( void );
  void linearCombination( dm_point3 *points , 
			  float w1 , float w2 );
  void linearCombination( dm_point3 *points , 
			  float w1 , float w2 , float w3 );
  void linearCombination( dm_point3 *points0 , 
			 dm_point3 *points1 , 
			 dm_point3 *points2 , 
			 int ind ,
			 float w1 , float w2 , float w3 );
  void linearCombination( dm_point3 *points0 , 
			  dm_point3 *points1 , 
			  dm_point3 *points2 , 
			  dm_point3 *points3 , 
			  int ind ,
			  float w1 , float w2 , float w3 , float w4 );
  void linearCombination( dm_point3 *points , 
			  float w1 , float w2 , float w3 , float w4 );
  /*!
   * \brief rotation of an angle alpha Oy around a point or a vector
   * \param source			to be added
   * \param alpha			to be added
   */
  void rotationv( dm_point3 &source , float alpha );

  /*!
   * \brief n-ary interpolation
   * \param tabPoints										to be added
   * \param weights1										to be added
   * \param weights2										to be added
   * \param numberOfComponents			to be added
   * \param coef												to be added
   */
  void interpolate( dm_point3 **tabPoints , 
		    float *weights1 , 
		    float *weights2 , 
		    int numberOfComponents ,
		    float coef );

  /*!
   * \brief binary interpolation: random stepwise motion from an orgin
   * \param p1			to be added
   * \param p2			to be added
   */
  void interpolate( dm_point3 *p1 , dm_point3 *p2 );
  void print( FILE *file , int depth , int index );
  void print( FILE *file , int depth );
  void product( dm_point3 &p , dm_matrix44 &m );
  void product( dm_point3 &p , float * matrixValue );
  void product2D( dm_point3 &p , float * matrixValue );
  void operator*(float f);
  void operator *( dm_matrix44 &m );
};

class dm_vector2 {
 public:
  GLfloat   x;
  GLfloat   y;
  dm_vector2( void );
  dm_vector2( GLfloat X , GLfloat Y );
  dm_vector2( GLfloat v[2] );
  dm_vector2( dm_point2 &p1 , dm_point2 &p2 );

  /// calculate the normal
  GLfloat norme( void );

  /// normalize the vectors
  void normalize( void );
  GLfloat cosinus( dm_vector2 &v1 , dm_vector2 &v2 );
  GLfloat sinus( dm_vector2 &v1 , dm_vector2 &v2 );
  void operator=(dm_vector2&);
  void operator+=(dm_vector2&);
  void operator*(float f);
  int operator==(dm_vector2&);
  void operator*=(double d);
  void operator/=(double d);
  void resolves( dm_vector2 &v1 , dm_vector2 &v2 , dm_vector2 &v3 );
};

class dm_Vertex {
public:
  float            x, y, z;
  float            x_ini, y_ini, z_ini;

  /*! \name Group vertex_type
   *  \brief INITIAL_VERTEX = 0 , REPLACEMENT_VERTEX , SPLITTING_VERTEX
   */
  //\{
  VertexType      vertexType;
  int             indReplacedVertex;				/// index of the ancestor vertex in case
																		/// of replacement
  int             indSegmentInitialVertex;		/// index of the initial vertices
  int             indSegmentFinalVertex;     /// in case of splitting
  int             indNewNormal;						/// index of the new normal vector
  int             indNewTangent;						/// index of the new tangent vector
																		/// in case of splitting
  // \}

/*   int             indNewColor;						/// index of the new color vector
																		/// in case of splitting*/
  int             indNewTextureCoord;			/// index of the new texture coordinates
																		/// in case of splitting
  bool             modified;									/// checks whether the algorithm was applied
																		/// on this vert	ex

  /// dm_Vertex: A point with additional data for meshes
  void init( void );
  void update( GLfloat X , GLfloat Y , GLfloat Z );
  void updateXZ( GLfloat X , GLfloat Z );
  void print( FILE *file , int depth , int index );
  void print( FILE *file , int depth );
  void middlePoint( dm_Vertex &p1 , dm_Vertex &p2 );
  void operator=(dm_Vertex& v);
  void operator*=(double d);
  void operator+=(dm_Vertex& v);
  void operator+=(dm_vector3& v);
  void operator*(double f);
  int operator==(dm_Vertex& v);
  float prodScal(dm_vector3& v2);

  /*!
   * \brief n-ary interpolation
   * \param tabVertex									to be added
   * \param weights1										to be added
   * \param weights2										to be added
   * \param numberOfComponents			to be added
   * \param coef												to be added
   */
  void interpolate( dm_Vertex **tabVertex ,
		    float *weights1 ,
		    float *weights2 ,
		    int numberOfComponents ,
		    float coef );

  /*!
   * \brief binary interpolation: random stepwise motion from an orgin
   * \param p1			to be added
   * \param p2			to be added
   */
  void interpolate( dm_Vertex *p1 , dm_Vertex *p2 );

  /*!
   * \brief reading a dm_Vertex
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
   * \brief setting up a dm_Vertex
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );
};


class dm_vector3 {
 public:
  GLfloat   x;
  GLfloat   y;
  GLfloat   z;
  dm_vector3( void );
  dm_vector3( GLfloat X , GLfloat Y , GLfloat Z );
  dm_vector3( GLfloat v[3] );
  dm_vector3( GLfloat v1[3] , GLfloat v2[3] );
  dm_vector3( dm_point3 &p1 , dm_point3 &p2 );
  dm_vector3( dm_Vertex &p1 , dm_Vertex &p2 );
  GLfloat norme( void );
  GLfloat norme2( void );

  /// normalize the vectors
  void normalize( void );

  /*!
   * \brief average
   * \param vectors					to be added
   * \param nbVectors			to be added
   */
  void averageVectors( dm_vector3 *vectors , int nbVectors );
  GLfloat prodScal( dm_vector3 &v1 );
  GLfloat prodScal( dm_point3 &p1 , dm_point3 &p2 );
  GLfloat prodScalxz( dm_vector3 &v1 );
  GLfloat prodScalxz( dm_point3 &p1 , dm_point3 &p2 );

  /*!
   * \brief calcul du produit vectoriel
   * \param v1			to be added
   * \param v2			to be added
   *
   *     2 vectors vus de l'avant
   *
   *       <---v1---
   *
   *			   |
   *
   *             v2
   *
   *              |
   *
   *              v
   *
   *  2 vectors
   */
  void prodVect( dm_vector3 &v1 , dm_vector3 &v2 );

  /*!
   * \brief vector that is the half sum of 2 vectors
   * \param v1			to be added
   * \param v2			to be added
   */
  void averageVector( dm_vector3 &v1 , dm_vector3 &v2 );

  /*!
   * \brief vector that is the half sum of 2 vectors
   * \param v1			to be added
   * \param v2			to be added
   */
  void averageNormalizedVector( dm_vector3 &v1 , dm_vector3 &v2 );

  /*!
   * 3 points seen from the front
   *
   *       p1     p2
   *
   *              p3
   *
   * 3 points
   */
  void prodVect( dm_point3 &p1 , dm_point3 &p2 , dm_point3 &p3 );
  void prodVect( dm_point3 &p1 , dm_point3 &p2 , dm_vector3 &v );
  void prodVect( dm_Vertex &p1 , dm_Vertex &p2 , dm_Vertex &p3 );
  void prodVect( dm_Vertex &p1 , dm_Vertex &p2 , dm_vector3 &v );

  /*! 
   * \brief rotation d'un angle alpha autour Oy d'un point ou d'un vector
   * \param source		to be added
   * \param alpha		to be added
   */
  void rotationv( dm_vector3 &source , float alpha );

  /*!
   * \brief Matrix x vector product 
   * \param v				to be added
   * \param m			to be added
   */
  void product( dm_vector3 &v , dm_matrix44 &m );

  /*!
   * \brief vector x Matrix  product 
   * \param m			to be added
   * \param v				to be added
   */
  void product( dm_matrix44 &m , dm_vector3 &v );
  GLfloat cosinus( dm_vector3 &v1 , dm_vector3 &v2 );
  void operator=(dm_vector3&);
  void operator+=(dm_vector3&);
  void operator*(float f);
  int operator==(dm_vector3&);
  void operator*=(double d);
  void operator/=(double d);
  void update( GLfloat X , GLfloat Y , GLfloat Z );
  void update( dm_point3 &p1 , dm_point3 &p2 );
  void update( dm_Vertex &p1 , dm_Vertex &p2 );

  /*!
   * \brief n-ary interpolation
   * \param tabVectors										to be added
   * \param weights1										to be added
   * \param weights2										to be added
   * \param numberOfComponents			to be added
   * \param coef												to be added
   */
  void interpolate( dm_vector3 **tabVectors , 
		    float *weights1 , 
		    float *weights2 , 
		    int numberOfComponents ,
		    float coef );

  /*!
   * \brief binary interpolation: random stepwise motion from an orgin
   * \param p1			to be added
   * \param p2			to be added
   */
  void interpolate( dm_vector3 *p1 , dm_vector3 *p2 );

  /*!
   * \brief reading a dm_Vertex
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added
   */
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				FloatBinOp oper ,
		      IntBinOp operI );
  void print( FILE *file , int depth , int index );
};

// @}

/*!
 * \brief computes the inverse of a matrix
 * taken from glu library (uses gaussian elimination)
 * \param m			to be called
 * \param out		to be called
 */
GLboolean invert_matrix(const GLfloat * m, GLfloat * out);

/*!
 * \brief computes the solution of a system of linear equations
 * \param m						to be called
 * \param out					to be called
 * \param nbRows			to be called
 */
bool gaussian_elimination( float * m, float * out , int nbRows );

/*!
 * \brief computes the solution of a system of linear equations
 * \param m						to be called
 * \param nbRows			to be called
 */
void transpose( float * m , int nbRows );

class dm_matrix44 {
 private:
  GLfloat   values[16];
  int DoPivot(dm_matrix44 &A, dm_matrix44 &I, int diag);
 public:
  dm_matrix44( void );
  dm_matrix44( GLfloat source_value );
  dm_matrix44( GLfloat *source_values );
  ~dm_matrix44(void);

  /*! \name Group TER_PHYSIQUE
   *  \brief TER -- PHYSIQUE
   * 
   * Mets a day values of the matrix 
   * <<A priori is unnecessary now that there GetModelViewMatrix44 ()>>
   */
  // @{
  void update( GLfloat *source_values );
  void update( double *source_values );

  /*!
   * \brief Built a matrix false next rotation axis and the axis angle angle!!!
   * \param axis			to be added
   * \param angle		to be added
   */
  void rotationMatrix(dm_vector3 axis, float angle);
 // TER -- PHYSIQUE
  void rotationMatrixNew(dm_vector3 axis, float angle);
  void LoadRotationIdentity( void );
  void LoadIdentity( void );
  GLfloat *GetValue( void );
  GLfloat GetValue( int index );
  void SetValue( float val , int index );
  void product(dm_matrix44& R , dm_matrix44& D);
  void productRot(dm_matrix44& R , dm_matrix44& D);
  void product(float *R , float *D);

  /*!
   * \brief Generates the rotation part of the matrix from Euler angles
   * from 3D Game Engine Design, David Eberly
   * \param X				to be added
   * \param Y			to be added
   * \param Z				to be added
   * \param roll			to be added
   * \param yaw		to be added
   * \param pitch		to be added
   */
  void EulertoMatrix( float X , float Y , float Z , 
		      float roll , float yaw , float pitch );

  /*!
   * \brief Generates the scale part of the matrix from scale coefs
   * \param X				to be added
   * \param Y			to be added
   * \param Z				to be added
   */
  void ScaletoMatrix( float X , float Y , float Z );


  /*!
   * \brief Generates the translation part of the matrix from translation coefs
   * \param X				to be added
   * \param Y			to be added
   * \param Z				to be added
   */
  void TranslationtoMatrix( float X , float Y , float Z );

  /*!
   * \brief Generates the Euler angles from the rotation part of the matrix
   * from 3D Game Engine Design, David Eberly
   * \param X				to be added
   * \param Y			to be added
   * \param Z				to be added
   * \param roll			to be added
   * \param yaw		to be added
   * \param pitch		to be added
   */
  void MatrixtoEuler( float * X , float * Y , float * Z , 
		      float * roll , float * yaw , float * pitch );

  /*! 
   * Generates the rotation part of the matrix from Quaternions
   * from http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
   * \param W			to be added
   * \param X				to be added
   * \param Y			to be added
   * \param Z				to be added
   */
  void QuaterniontoMatrix( float W , float X , float Y , float Z );
  void print( FILE *file , int isLong , int depth );
  dm_matrix44 operator * (dm_matrix44 &Droite);
  void operator=(dm_matrix44&);
  void operator+=(dm_matrix44&);
  void operator+=(dm_vector3&);
  int operator==(dm_matrix44&);
  bool operator !=( dm_matrix44 &m );
/*   inline dm_matrix44 operator !() const { return ~(*this) ; } ; */
  dm_matrix44 operator ~( void );
  // @}
};


#endif
