/*! \file dm-lib.cpp
 *  \brief the implementation of dm-lib.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-lib.cpp
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

#include "dm-all_include.h"

class dm_matrix44;

dm_matrix44 *identity_matrix44 = new dm_matrix44();
dm_matrix44 *null_matrix44 = new dm_matrix44( 0.f );

float DegtoRad = (float)M_PI / 180.0F;
float RadtoDeg = 180.0F / (float)M_PI;
float M_PI2 = (float)M_PI * 2.0F;

const char *TextureModeString[DM_NB_TEXTURING_MODES + 1] = { "normal" , "vertical" , "radial" , "" };

void indent( FILE *file , int depth ) {
  for( int ind = 0 ; ind < depth ; ind++ ) {
    fprintf( file , "  " );
  }
}

void generateRandomWeights( float *tabWeights ,
			    int numberOfComponents ) {
  float weightSum = 0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    weightSum += 
      (tabWeights[ind] = (float)((double)(rand() - RAND_MAX/2)
				 /(double)RAND_MAX));
  }
  // normalization
  if( weightSum != 0 ) {
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      tabWeights[ind] /= weightSum;
    }
  }
}  

float generateRandomFloat( float inf , float sup ) {
  if( inf > sup ) {
    return 0.0F;
  }
  if( inf == sup ) {
    return inf;
  }
  float valret = 
    inf + (sup - inf) * (float)((double)rand()/(double)RAND_MAX);
//    printf( "[%.5f,%.5f] -> {%.4f]\n" ,
//  	  inf , sup , valret );
  return valret;
}

int generateRandomInt( int min , int sup ) {
  if( min > sup ) {
    return 0;
  }
  if( min == sup ) {
    return min;
  }
  int resul = (int)generateRandomFloat( (float)min , (float)(sup + 1) );
  if( resul < min ) {
    return min;
  }
  if( resul > sup ) {
    return sup;
  }
  return resul;
}


void alignToGrid( float *f ) {
  *f = (float)( floor(( 10.0F * (*f) ) + 0.5F) ) / 10.0F;
  // *f = (float)( rint( 10.0 * (*f) ) ) / 10.0;
}

/////////////////////////////////////////////////////////
// Math functions


GLfloat powExt(GLfloat x , GLfloat exp) {
  if( x >= 0.0 ) {
    return  pow(x,exp);
  }
  else {
    return  -pow(-x,exp);
  }
}

void tangentContourLine( float alpha , dm_vector3 &tangNiv ) {
  tangNiv.x = - sin( alpha );
  tangNiv.y = 0;
  tangNiv.z = cos( alpha );
}

void normaliseAngleLongitude( float *angle ) {
  if( *angle == MAXFLOAT || *angle == -MAXFLOAT ) {
    return;
  }

  if( *angle < -10000 || *angle > 10000 ) {
    sprintf( ErrorStr , "Error: excessve angle value, cannot be normalized [%.5f]!" , *angle ); ReportError( ErrorStr ); throw 63;
  }
    
  while( *angle < -180 ) {
    *angle += 360;
    // printf( "> %.5f\n" , *angle);
  }
  while( *angle > 180 ) {
    *angle -= 360;
    // printf( "< %.5f\n" , *angle);
  }
}

void normaliseAngleLatitude( float *angle ) {
  if( *angle == MAXFLOAT || *angle == -MAXFLOAT ) {
    return;
  }

  normaliseAngleLongitude( angle );
    
  if( *angle > 90 ) {
    *angle = 180 - *angle;
    // printf( "> %.5f\n" , *angle);
  }
  else if( *angle < -90 ) {
    *angle = -180 - *angle;
    // printf( "< %.5f\n" , *angle);
  }
}

int smallestPOT( int x ) {
  int out = 1;
  bool isPOT = true;
  while( x >>= 1 ) {
    out <<= 1;
    if( x & 1 && x != 1 ) {
      isPOT = false;
    }
  }
  // if x is NPOT, out must be x-ied by 2
  if( !isPOT ) {
    out <<= 1;
  }
  return out;
}

dm_point2::dm_point2( void ) {
  x = 0;
  y = 0;
  estSelectionne = false;
}
dm_point2::dm_point2( GLfloat X , GLfloat Y ) {
  x = X;
  y = Y;
  estSelectionne = false;
}
void dm_point2::operator=(dm_point2& p) {
  x = p.x;
  y = p.y;
  estSelectionne = p.estSelectionne;
}
void dm_point2::operator+=(dm_vector2 &v) {
  x += v.x;
  y += v.y;
}
void dm_point2::operator+(dm_point2 &p) {
  x += p.x;
  y += p.y;
}
void dm_point2::middle( dm_point2 &p1 , dm_point2 &p2 )
{
  x = .5F * (p1.x + p2.x);
  y = .5F * (p1.y + p2.y);
}

dm_point3::dm_point3( void ) {
  x = 0;
  y = 0;
  z = 0;
  estSelectionne = false;
}
dm_point3::dm_point3( GLfloat X , GLfloat Y , GLfloat Z ) {
  x = X;
  y = Y;
  z = Z;
  estSelectionne = false;
}
void dm_point3::operator=(dm_point3& p) {
  x = p.x;
  y = p.y;
  z = p.z;
  estSelectionne = p.estSelectionne;
}
void dm_point3::operator+=(dm_vector3 &v) {
  x += v.x;
  y += v.y;
  z += v.z;
}
void dm_point3::operator+=(dm_point3 &p) {
  x += p.x;
  y += p.y;
  z += p.z;
}
void dm_point3::operator*=(float k) {
  x *= k;
  y *= k;
  z *= k;
}
void dm_point3::operator+(dm_point3 &p) {
  x += p.x;
  y += p.y;
  z += p.z;
}
void dm_point3::operator*(float f) {
  x = f * x;
  y = f * y;
  z = f * z;
}
void dm_point3::update( GLfloat X , GLfloat Y , GLfloat Z ) {
  x = X;
  y = Y;
  z = Z;
}
void dm_point3::middle( dm_point3 &p1 , dm_point3 &p2 )
{
  x = .5F * (p1.x + p2.x);
  y = .5F * (p1.y + p2.y);
  z = .5F * (p1.z + p2.z);
}
GLfloat dm_point3::distance( dm_point3 &p1 ) {
  dm_vector3 v;
  v.x = p1.x - x;
  v.y = p1.y - y;
  v.z = p1.z - z;
  return v.norme();
}
GLfloat dm_point3::distancexz( dm_point3 &p1 ) {
  dm_vector3 v;
  v.x = p1.x - x;
  v.y = 0;
  v.z = p1.z - z;
  return v.norme();
}
void dm_point3::canonizeAnglexz( void ) {
  normaliseAngleLatitude( &x );
  normaliseAngleLongitude( &z );
}

void dm_point3::rotationv( dm_point3 &source , float alpha ) {
  x = source.x * cos( alpha );
  y = source.y;
  z = source.x * sin( alpha );
}
void dm_point3::linearCombination( dm_point3 *points , 
				   float w1 , float w2 ) {
  x = w1 * points[0].x + w2 * points[1].x;
  y = w1 * points[0].y + w2 * points[1].y;
  z = w1 * points[0].z + w2 * points[1].z;
} 
void dm_point3::linearCombination( dm_point3 *points , 
				   float w1 , float w2 , float w3 ) {
  x = w1 * points[0].x + w2 * points[1].x + w3 * points[2].x;
  y = w1 * points[0].y + w2 * points[1].y + w3 * points[2].y;
  z = w1 * points[0].z + w2 * points[1].z + w3 * points[2].z;
} 
void dm_point3::linearCombination( dm_point3 *points0 , 
				   dm_point3 *points1 , 
				   dm_point3 *points2 , 
				   int ind ,
				   float w1 , float w2 , float w3 ) {
  x = w1 * points0[ind].x + w2 * points1[ind].x + w3 * points2[ind].x;
  y = w1 * points0[ind].y + w2 * points1[ind].y + w3 * points2[ind].y;
  z = w1 * points0[ind].z + w2 * points1[ind].z + w3 * points2[ind].z;
} 
void dm_point3::linearCombination( dm_point3 *points , 
				   float w1 , float w2 , float w3 , float w4 ) {
  x = w1 * points[0].x + w2 * points[1].x + w3 * points[2].x
    + w4 * points[3].x;
  y = w1 * points[0].y + w2 * points[1].y + w3 * points[2].y
    + w4 * points[3].y;
  z = w1 * points[0].z + w2 * points[1].z + w3 * points[2].z
    + w4 * points[3].z;
} 
void dm_point3::linearCombination( dm_point3 *points0 , 
				   dm_point3 *points1 , 
				   dm_point3 *points2 , 
				   dm_point3 *points3 , 
				   int ind ,
				   float w1 , float w2 , float w3 , float w4 ) {
  x = w1 * points0[ind].x + w2 * points1[ind].x + w3 * points2[ind].x
    + w4 * points3[ind].x;
  y = w1 * points0[ind].y + w2 * points1[ind].y + w3 * points2[ind].y
    + w4 * points3[ind].y;
  z = w1 * points0[ind].z + w2 * points1[ind].z + w3 * points2[ind].z
    + w4 * points3[ind].z;
} 

void dm_point3::interpolate( dm_point3 **tabPoints , 
			  float *weights1 , 
			  float *weights2 , 
			  int numberOfComponents ,
			  float coef ) {
  float coef2 = 1.0F - coef;
  float x1 = 0.0;
  float x2 = 0.0;
  float y1 = 0.0;
  float y2 = 0.0;
  float z1 = 0.0;
  float z2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    x1 += weights1[ind] * tabPoints[ind]->x;
    x2 += weights2[ind] * tabPoints[ind]->x;
    y1 += weights1[ind] * tabPoints[ind]->y;
    y2 += weights2[ind] * tabPoints[ind]->y;
    z1 += weights1[ind] * tabPoints[ind]->z;
    z2 += weights2[ind] * tabPoints[ind]->z;
  }
  x = x1 * coef + x2 * coef2;
  y = y1 * coef + y2 * coef2;
  z = z1 * coef + z2 * coef2;
}

void dm_point3::interpolate( dm_point3 *p1 , dm_point3 *p2 ) {
  x += generateRandomFloat( -p2->x , p2->x );
  y += generateRandomFloat( -p2->y , p2->y );
  z += generateRandomFloat( -p2->z , p2->z );
}

void dm_point3::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<point index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , index + 1  , x , y , z );
  }
  else {
    fprintf( file , "<point x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , x , y , z );
  }
}
void dm_point3::print( FILE *file , int depth ) {
  print( file , depth , 0 );
}

void dm_point3::product( dm_point3 &p , dm_matrix44 &m ) {
  product( p , m.GetValue());
}

void dm_point3::product( dm_point3 &p , float * matrixValue ) {
  x = p.x * matrixValue[0] + p.y * matrixValue[4] 
    + p.z * matrixValue[8] + matrixValue[12];
  y = p.x * matrixValue[1] + p.y * matrixValue[5]
    + p.z * matrixValue[9] + matrixValue[13];
  z = p.x * matrixValue[2] + p.y * matrixValue[6] 
    + p.z * matrixValue[10] + matrixValue[14];
}

void dm_point3::product2D( dm_point3 &p , float * matrixValue ) {
  x = p.x * matrixValue[0] + p.y * matrixValue[3] 
    + p.z * matrixValue[6];
  y = p.x * matrixValue[1] + p.y * matrixValue[4]
    + p.z * matrixValue[7];
  z = p.x * matrixValue[2] + p.y * matrixValue[5] 
    + p.z * matrixValue[8];
}

void dm_Vertex::init( void ) {
  // actual coordinates (after possible transformation(s) by keypoint(s))
  x = 0.0;
  y = 0.0;
  z = 0.0;
  // initial coordinates (before transformation(s) by keypoint(s))
  x_ini = 0.0;
  y_ini = 0.0;
  z_ini = 0.0;
  vertexType = EmptyVertexType;
  indReplacedVertex = -1;
  indSegmentInitialVertex = -1;
  indSegmentFinalVertex = -1;
  indNewNormal = -1;
//   indNewColor = -1;
  indNewTextureCoord = -1;
  modified = false;
}

void dm_Vertex::update( GLfloat X , GLfloat Y , GLfloat Z ) {
  x = X;
  y = Y;
  z = Z;
}

void dm_Vertex::updateXZ( GLfloat X , GLfloat Z ) {
  x = X;
  z = Z;
}

void dm_Vertex::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<vertex index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , index + 1 , x , y , z );
  }
  else {
    fprintf( file , "<vertex x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , x , y , z );
  }
}
void dm_Vertex::print( FILE *file , int depth ) {
  print( file , depth , 0 );
}
void dm_Vertex::middlePoint( dm_Vertex &p1 , dm_Vertex &p2 ) {
  x = .5F * (p1.x + p2.x);
  y = .5F * (p1.y + p2.y);
  z = .5F * (p1.z + p2.z);
}
void dm_Vertex::operator=(dm_Vertex& ve) {
  x = ve.x;
  y = ve.y;
  z = ve.z;
}
void dm_Vertex::operator*=(double d) {
  x *= (float)d;
  y *= (float)d;
  z *= (float)d;
}
void dm_Vertex::operator+=(dm_Vertex& ve) {
  x += ve.x;
  y += ve.y;
  z += ve.z;
}
void dm_Vertex::operator+=(dm_vector3& v) {
  x += v.x;
  y += v.y;
  z += v.z;
}
void dm_Vertex::operator*(double f) {
  x *= (float)f;
  y *= (float)f;
  z *= (float)f;
}
int dm_Vertex::operator==(dm_Vertex& v) {
  return((x == v.x) && (y == v.y) && (z == v.z));
}

void dm_Vertex::interpolate( dm_Vertex **tabVertex , 
			  float *weights1 , 
			  float *weights2 , 
			  int numberOfComponents ,
			  float coef ) {
  float coef2 = 1.0F - coef;
  float x1 = 0.0;
  float x2 = 0.0;
  float y1 = 0.0;
  float y2 = 0.0;
  float z1 = 0.0;
  float z2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    x1 += weights1[ind] * tabVertex[ind]->x;
    x2 += weights2[ind] * tabVertex[ind]->x;
    y1 += weights1[ind] * tabVertex[ind]->y;
    y2 += weights2[ind] * tabVertex[ind]->y;
    z1 += weights1[ind] * tabVertex[ind]->z;
    z2 += weights2[ind] * tabVertex[ind]->z;
  }
  x = x1 * coef + x2 * coef2;
  y = y1 * coef + y2 * coef2;
  z = z1 * coef + z2 * coef2;
}

void dm_Vertex::interpolate( dm_Vertex *p1 , dm_Vertex *p2 ) {
  x += generateRandomFloat( -p2->x , p2->x );
  y += generateRandomFloat( -p2->y , p2->y );
  z += generateRandomFloat( -p2->z , p2->z );
}

bool dm_Vertex::getParameters( char *attribute , 
			       DataType * dataType ,
			       double * valDouble ,
			       int * valInt ,
			       bool * valBool ,
			       char ** valString ,
			       int * tableOrMatrixSize ,
			       dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "x" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)x;
    return true;
  }
  else if( strcmp( attribute , "y" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)y;
    return true;
  }
  else if( strcmp( attribute , "z" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)z;
    return true;
  }
  return false;
}

void dm_Vertex::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				FloatBinOp oper ,
				IntBinOp operI ) {
  if( !atRunTime ) {
    x = 0.0;
  }
  if( XMLtag->hasAttribute( "x"  ) ) {
    oper( x 
      , (float)XMLtag->getAttributeDoubleValue( "x" , 
					atRunTime ));
    // printf("x %f\n",x);
  }
  if( !atRunTime ) {
    y = 0.0;
  }
  if( XMLtag->hasAttribute( "y"  ) ) {
    oper( y 
      , (float)XMLtag->getAttributeDoubleValue( "y" , 
					atRunTime ));
  }
  if( !atRunTime ) {
    z = 0.0;
  }
  if( XMLtag->hasAttribute( "z"  ) ) {
    oper( z
      , (float)XMLtag->getAttributeDoubleValue( "z" , 
					atRunTime ));
  }
}

void dm_Texturecoord::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<textureCoord index=\"%d\" u=\"%.5f\" v=\"%.5f\" />\n" , index + 1 , u , v );
  }
  else {
    fprintf( file , "<textureCoord u=\"%.5f\" v=\"%.5f\" />\n" , u , v );
  }
}
void dm_Texturecoord::print( FILE *file , int depth ) {
  print( file , depth , 0 );
}
void dm_Texturecoord::operator=(dm_Texturecoord& ve) {
  u = ve.u;
  v = ve.v;
}
void dm_Texturecoord::operator*=(double d) {
  u *= (float)d;
  v *= (float)d;
}
void dm_Texturecoord::operator+=(dm_Texturecoord& te) {
  u += te.u;
  v += te.v;
}
void dm_Texturecoord::operator*(double f) {
  u *= (float)f;
  v *= (float)f;
}

void dm_Texturecoord::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				FloatBinOp oper ,
				IntBinOp operI ) {
  if( !atRunTime ) {
    u = 0.0;
  }
  if( XMLtag->hasAttribute( "u"  ) ) {
    oper( u 
      , (float)XMLtag->getAttributeDoubleValue( "u" , 
					atRunTime ));
  }
  if( !atRunTime ) {
    v = 0.0;
  }
  if( XMLtag->hasAttribute( "v"  ) ) {
    oper( v 
      , (float)XMLtag->getAttributeDoubleValue( "v" , 
					atRunTime ));
  }
}

dm_vector2::dm_vector2( void ) {
  x = 0;
  y = 0;
}
dm_vector2::dm_vector2( GLfloat X , GLfloat Y ) {
  x = X;
  y = Y;
}
dm_vector2::dm_vector2( dm_point2 &p1 , dm_point2 &p2 ) {
  x = p2.x - p1.x;
  y = p2.y - p1.y;
}
dm_vector2::dm_vector2( GLfloat v[2] ) {
  x = v[0];
  y = v[1];
}
void dm_vector2::operator=(dm_vector2& v) {
  x = v.x;
  y = v.y;
}
void dm_vector2::operator+=(dm_vector2& v) {
  x += v.x;
  y += v.y;
}
void dm_vector2::operator*=(double d) {
  x *= (float)d;
  y *= (float)d;
}
void dm_vector2::operator/=(double d) {
  x /= (float)d;
  y /= (float)d;
}
void dm_vector2::operator*(float f) {
  x = f * x;
  y = f * y;
}
int dm_vector2::operator==(dm_vector2& v) {
  return((x == v.x) && (y == v.y));
}

GLfloat dm_vector2::norme( void ) {
  return sqrt( x * x + y * y );
}

void dm_vector2::normalize( void )
{
  GLfloat norme = this->norme();

  if( norme != 0 ) {
    x /= norme;
    y /= norme;
  }
}

GLfloat dm_vector2::cosinus( dm_vector2 &v1 , dm_vector2 &v2 ) {
  GLfloat norm = v1.norme() * v2.norme();
  if( norm != 0 ) {
    return ( v1.x * v2.x +  v1.y * v2.y )/norm;
  }
  else {
    return 0;
  }
}

GLfloat dm_vector2::sinus( dm_vector2 &v1 , dm_vector2 &v2 ) {
  GLfloat norm = v1.norme() * v2.norme();
  if( norm != 0 ) {
    return ( v1.x * v2.y -  v1.y * v2.x )/norm;
  }
  else {
    return 0;
  }
}

void dm_vector2::resolves( dm_vector2 &v1 , dm_vector2 &v2 , dm_vector2 &v3 ) {
  float det = v1.x * v2.y - v1.y * v2.x;
  if( det ) {
    x = (v3.x * v2.y - v3.y * v2.x)/det;
    y = (v1.x * v3.y - v1.y * v3.x)/det;
  }
  else {
    x = (y = 0);
  }
}

dm_vector3::dm_vector3( void ) {
  x = 0;
  y = 0;
  z = 0;
}
dm_vector3::dm_vector3( GLfloat X , GLfloat Y, GLfloat Z ) {
  x = X;
  y = Y;
  z = Z;
}
dm_vector3::dm_vector3( GLfloat v[3] ) {
  x = v[0];
  y = v[1];
  z = v[2];
}
dm_vector3::dm_vector3( dm_point3 &p1 , dm_point3 &p2 ) {
  x = p2.x - p1.x;
  y = p2.y - p1.y;
  z = p2.z - p1.z;
}
dm_vector3::dm_vector3( GLfloat v1[3] , GLfloat v2[3] ) {
  x = v2[0] - v1[0];
  y = v2[1] - v1[1];
  z = v2[2] - v1[2];
}
void dm_vector3::operator=(dm_vector3& v) {
  x = v.x;
  y = v.y;
  z = v.z;
}
void dm_vector3::operator+=(dm_vector3& v) {
  x += v.x;
  y += v.y;
  z += v.z;
}
void dm_vector3::operator*=(double d) {
  x *= (float)d;
  y *= (float)d;
  z *= (float)d;
}
void dm_vector3::operator/=(double d) {
  x /= (float)d;
  y /= (float)d;
  z /= (float)d;
}
void dm_vector3::operator*(float f) {
  x = f * x;
  y = f * y;
  z = f * z;
}
int dm_vector3::operator==(dm_vector3& v) {
  return((x == v.x) && (y == v.y) && (z == v.z));
}
void dm_vector3::update( GLfloat X , GLfloat Y , GLfloat Z ) {
  x = X;
  y = Y;
  z = Z;
}
void dm_vector3::update( dm_point3 &p1 , dm_point3 &p2 ) {
  x = p2.x - p1.x;
  y = p2.y - p1.y;
  z = p2.z - p1.z;
}
GLfloat dm_vector3::norme( void ) {
  return sqrt( x * x + y * y + z * z );
}
GLfloat dm_vector3::norme2( void ) {
  return ( x * x + y * y + z * z );
}

void dm_vector3::normalize()
{
  GLfloat norme = this->norme();

  if( norme != 0 ) {
    x /= norme;
    y /= norme;
    z /= norme;
  }
}

void dm_vector3::averageVectors( dm_vector3 *vectors , int nbVectors ) {
  x = 0; y = 0; z = 0;
  if( nbVectors <= 0 ) {
    return;
  }
  for( int ind = 0 ; ind < nbVectors ; ind++ ) {
    x += vectors[ ind ].x;
    y += vectors[ ind ].y;
    z += vectors[ ind ].z;
  }
  float inv = 1.0F / (float)nbVectors;
  x *= inv;
  y *= inv;
  z *= inv;
}

GLfloat dm_vector3::cosinus( dm_vector3 &v1 , dm_vector3 &v2 ) {
  GLfloat norm = v1.norme() * v2.norme();
  if( norm != 0 ) {
    return ( v1.x * v2.x +  v1.y * v2.y +  v1.z * v2.z )/norm;
  }
  else {
    return 0;
  }
}

void dm_vector3::averageVector( dm_vector3 &v1 , dm_vector3 &v2 ) {
  x = v1.x * .5F + v2.x * .5F;
  y = v1.y * .5F + v2.y * .5F;
  z = v1.z * .5F + v2.z * .5F;
}

void dm_vector3::averageNormalizedVector( dm_vector3 &v1 , dm_vector3 &v2 ) {
  x = v1.x * .5F + v2.x * .5F;
  y = v1.y * .5F + v2.y * .5F;
  z = v1.z * .5F + v2.z * .5F;
  normalize();
}

void dm_vector3::prodVect( dm_vector3 &v1 , dm_vector3 &v2 )
{
  x = v1.y * v2.z - v2.y * v1.z;
  y = v2.x * v1.z - v1.x * v2.z;
  z = v2.y * v1.x - v1.y * v2.x;
}

void dm_vector3::prodVect( dm_point3 &p1 , dm_point3 &p2 , 
			   dm_point3 &p3 )
{
  dm_vector3 v1( p2 , p1 );
  dm_vector3 v2( p2 , p3 );

  prodVect( v1 , v2 );
  //  pv->x = (p1.y - p2.y) * (p3.z - p2.z) - (p3.y - p2.y) * (p1.z - p2.z);
  //  pv->y = (p3.x - p2.x) * (p1.z - p2.z) - (p1.x - p2.x) * (p3.z - p2.z);
  //  pv->z = (p3.y - p2.y) * (p1.x - p2.x) - (p1.y - p2.y) * (p3.x - p2.x);
}

void dm_vector3::prodVect( dm_point3 &p1 , dm_point3 &p2 , dm_vector3 &v ) {
  dm_vector3 v1( p1 , p2 );
  prodVect( v1 , v );
}

GLfloat dm_vector3::prodScal( dm_vector3 &v1 ) {
  return x * v1.x + y * v1.y + z * v1.z;
}

GLfloat dm_vector3::prodScal( dm_point3 &p1 , dm_point3 &p2 ) {
  dm_vector3 v1( p1 , p2 );
  return prodScal( v1 );
}

GLfloat dm_vector3::prodScalxz( dm_vector3 &v1 ) {
  return x * v1.x + z * v1.z;
}

GLfloat dm_vector3::prodScalxz( dm_point3 &p1 , dm_point3 &p2 ) {
  dm_vector3 v1( p1 , p2 );
  return prodScalxz( v1 );
}

void dm_vector3::product( dm_vector3 &v , dm_matrix44 &m ) {
  float * value = m.GetValue();

  x = v.x * value[0] + v.y * value[4] + v.z * value[8];
  y = v.x * value[1] + v.y * value[5] + v.z * value[9];
  z = v.x * value[2] + v.y * value[6] + v.z * value[10];
}

void dm_vector3::product( dm_matrix44 &m , dm_vector3 &v ) {
  float * value = m.GetValue();

  x = v.x * value[0] + v.y * value[1] + v.z * value[2];
  y = v.x * value[4] + v.y * value[5] + v.z * value[6];
  z = v.x * value[8] + v.y * value[9] + v.z * value[10];
}

void dm_vector3::rotationv( dm_vector3 &source , float alpha ) {
  x = source.x * cos( alpha );
  y = source.y;
  z = source.x * sin( alpha );
}

void dm_vector3::interpolate( dm_vector3 **tabVectors , 
			  float *weights1 , 
			  float *weights2 , 
			  int numberOfComponents ,
			  float coef ) {
  float coef2 = 1.0F - coef;
  float x1 = 0.0;
  float x2 = 0.0;
  float y1 = 0.0;
  float y2 = 0.0;
  float z1 = 0.0;
  float z2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    x1 += weights1[ind] * tabVectors[ind]->x;
    x2 += weights2[ind] * tabVectors[ind]->x;
    y1 += weights1[ind] * tabVectors[ind]->y;
    y2 += weights2[ind] * tabVectors[ind]->y;
    z1 += weights1[ind] * tabVectors[ind]->z;
    z2 += weights2[ind] * tabVectors[ind]->z;
  }
  x = x1 * coef + x2 * coef2;
  y = y1 * coef + y2 * coef2;
  z = z1 * coef + z2 * coef2;
}

void dm_vector3::interpolate( dm_vector3 *p1 , dm_vector3 *p2 ) {
  x += generateRandomFloat( -p2->x , p2->x );
  y += generateRandomFloat( -p2->y , p2->y );
  z += generateRandomFloat( -p2->z , p2->z );
}

void dm_vector3::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				FloatBinOp oper ,
				IntBinOp operI ) {
  if( !atRunTime ) {
    x = 0.0;
  }
  if( XMLtag->hasAttribute( "x"  ) ) {
    oper( x 
      , (float)XMLtag->getAttributeDoubleValue( "x" , 
					atRunTime ));
  }
  if( !atRunTime ) {
    y = 0.0;
  }
  if( XMLtag->hasAttribute( "y"  ) ) {
    oper( y 
      , (float)XMLtag->getAttributeDoubleValue( "y" , 
					atRunTime ));
  }
  if( !atRunTime ) {
    z = 0.0;
  }
  if( XMLtag->hasAttribute( "z"  ) ) {
    oper( z
      , (float)XMLtag->getAttributeDoubleValue( "z" , 
					atRunTime ));
  }
}

void dm_vector3::print( FILE *file , int depth , int index ) {
  indent( file , depth );
  if( index >= 0 ) {
    fprintf( file , "<vector index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , index + 1 , x , y , z );
  }
  else {
    fprintf( file , "<vector x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , x , y , z );
  }
}

dm_matrix44::dm_matrix44( void ) {
  LoadIdentity();
}
dm_matrix44::dm_matrix44( GLfloat source_value ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = source_value;
  }
}
dm_matrix44::dm_matrix44( GLfloat *source_values ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = source_values[ind];
  }
}

dm_matrix44::~dm_matrix44(void)
{
}

void dm_matrix44::update ( GLfloat *source_values ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = source_values[ind];
  }
}

void dm_matrix44::update ( double *source_values ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = (float)source_values[ind];
  }
}

void dm_matrix44::rotationMatrix(dm_vector3 axis, float angle) {
	float cost = cos(angle*DegtoRad);
	float sint = sin(angle*DegtoRad);
	float mcost = 1 - cost;
	
	values[0] = cost + mcost*axis.x*axis.x;
	values[1] = mcost*axis.x*axis.y + sint*axis.z;
	values[2] = mcost*axis.x*axis.z - sint*axis.y;
	values[3] = 0;
	values[4] = mcost*axis.y*axis.x - sint*axis.z;
	values[5] = cost + mcost*axis.y*axis.y;
	values[6] = mcost*axis.y*axis.z + sint*axis.x;
	values[7] = 0;
	values[8] = mcost*axis.x*axis.z + sint*axis.y;
	values[9] = mcost*axis.z*axis.y - sint*axis.x;
	values[10] = cost + mcost*axis.z*axis.z;
	values[11] = 0;
	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;
}
// TER -- PHYSIQUE

void dm_matrix44::rotationMatrixNew(dm_vector3 axis, float angleRad) {
//   fprintf( stdout , "setAxisAngle Angle radian %.6f axis %.6f %.6f %.6f \n" , angleRad , axis.x , axis.y , axis.z );

  double cost,sint;
  double dNx,dNy,dNz;
  
  cost=cos(0.5*(double)angleRad);
  sint=sin(0.5*(double)angleRad);
  
  dNx = (double)axis.x * sint;
  dNy = (double)axis.y * sint;
  dNz = (double)axis.z * sint;
  
  values[0] = (float)(1.0F-2.0F*dNy*dNy-2.0F*dNz*dNz);
  values[1] = (float)(2.0F*dNx*dNy-2.0F*(float)cost*dNz);
  values[2] = (float)(2.0F*dNx*dNz+2.0F*(float)cost*dNy);
  values[3] = 0;
  values[4] = (float)(2.0F*dNx*dNy+2.0F*(float)cost*dNz);
  values[5] = (float)(1.0F-2.0F*dNx*dNx-2.0F*dNz*dNz);
  values[6] = (float)(2.0F*dNy*dNz-2.0F*(float)cost*dNx);
  values[7] = 0;
  values[8] = (float)(2.0F*dNx*dNz-2.0F*(float)cost*dNy);
  values[9] = (float)(2.0F*dNy*dNz+2.0F*(float)cost*dNx);
  values[10] = (float)(1.0F-2.0F*dNx*dNx-2.0F*dNy*dNy);
  values[11] = 0;
  values[12] = 0;
  values[13] = 0;
  values[14] = 0;
  values[15] = 1;
//   fprintf( stdout , "setAxisAngle %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n" , 
// 	  values[0] , values[1] , values[2] , values[3] , values[4] , values[5] , values[6] , values[7] , values[8] , values[9] , values[10] , values[11] , values[12] , values[13] , values[14] , values[15] );
}

void dm_matrix44::LoadIdentity( void ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = 0.0;
  }
  values[0] = 1.0; values[5] = 1.0; values[10] = 1.0;
  values[15] = 1.0;
}
void LoadIdentityf( float *values ) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = 0.0;
  }
  values[0] = 1.0; values[5] = 1.0; values[10] = 1.0;
  values[15] = 1.0;
}
void dm_matrix44::LoadRotationIdentity( void ) {
  for( int ind = 0 ; ind < 12 ; ind++ ) {
    values[ind] = 0.0;
  }
  values[0] = 1.0; values[5] = 1.0; values[10] = 1.0;
}
void LoadRotationIdentityf( float *values ) {
  for( int ind = 0 ; ind < 12 ; ind++ ) {
    values[ind] = 0.0;
  }
  values[0] = 1.0; values[5] = 1.0; values[10] = 1.0;
}
GLfloat *dm_matrix44::GetValue( void ) {
  return values;
}
void dm_matrix44::SetValue( float val , int index ) {
  if( index >= 0 && index < 16 ) 
    values[ index ] = val;
  else {
    sprintf( ErrorStr , "Error: incorrect matrix index [%.5d]!" , index ); ReportError( ErrorStr );
  }
}
GLfloat dm_matrix44::GetValue( int index ) {
  if( index >= 0 && index < 16 ) 
    return values[ index ];
  else {
    sprintf( ErrorStr , "Error: incorrect matrix index [%.5d]!" , index ); ReportError( ErrorStr );
    return 0;
  }
}
void dm_matrix44::product(dm_matrix44& R , dm_matrix44& D) {
  GLfloat val[16];

  val[0]  = values[0]*D.values[0] + values[4]*D.values[1] + values[8]*D.values[2] + values[12]*D.values[3];
  val[1]  = values[1]*D.values[0] + values[5]*D.values[1] + values[9]*D.values[2] + values[13]*D.values[3];
  val[2]  = values[2]*D.values[0] + values[6]*D.values[1] + values[10]*D.values[2] + values[14]*D.values[3];
  val[3]  = values[3]*D.values[0] + values[7]*D.values[1] + values[11]*D.values[2] + values[15]*D.values[3];
  
  val[4]  = values[0]*D.values[4] + values[4]*D.values[5] + values[8]*D.values[6] + values[12]*D.values[7];
  val[5]  = values[1]*D.values[4] + values[5]*D.values[5] + values[9]*D.values[6] + values[13]*D.values[7];
  val[6]  = values[2]*D.values[4] + values[6]*D.values[5] + values[10]*D.values[6] + values[14]*D.values[7];
  val[7]  = values[3]*D.values[4] + values[7]*D.values[5] + values[11]*D.values[6] + values[15]*D.values[7];

  val[8]  = values[0]*D.values[8] + values[4]*D.values[9] + values[8]*D.values[10] + values[12]*D.values[11];
  val[9]  = values[1]*D.values[8] + values[5]*D.values[9] + values[9]*D.values[10] + values[13]*D.values[11];
  val[10] = values[2]*D.values[8] + values[6]*D.values[9] + values[10]*D.values[10] + values[14]*D.values[11];
  val[11] = values[3]*D.values[8] + values[7]*D.values[9] + values[11]*D.values[10] + values[15]*D.values[11];
  
  val[12] = values[0]*D.values[12] + values[4]*D.values[13] + values[8]*D.values[14] + values[12]*D.values[15];
  val[13] = values[1]*D.values[12] + values[5]*D.values[13] + values[9]*D.values[14] + values[13]*D.values[15];
  val[14] = values[2]*D.values[12] + values[6]*D.values[13] + values[10]*D.values[14] + values[14]*D.values[15];
  val[15] = values[3]*D.values[12] + values[7]*D.values[13] + values[11]*D.values[14] + values[15]*D.values[15];

  memcpy( (char *)R.values , val , 16 * sizeof( float ) );
}

void dm_matrix44::productRot(dm_matrix44& R , dm_matrix44& D) {
  GLfloat val[9];

  val[0]  = values[0]*D.values[0] + values[4]*D.values[1] + values[8]*D.values[2];
  val[1]  = values[1]*D.values[0] + values[5]*D.values[1] + values[9]*D.values[2];
  val[2]  = values[2]*D.values[0] + values[6]*D.values[1] + values[10]*D.values[2];
  
  val[3]  = values[0]*D.values[4] + values[4]*D.values[5] + values[8]*D.values[6];
  val[4]  = values[1]*D.values[4] + values[5]*D.values[5] + values[9]*D.values[6];
  val[5]  = values[2]*D.values[4] + values[6]*D.values[5] + values[10]*D.values[6];
  
  val[6]  = values[0]*D.values[8] + values[4]*D.values[9] + values[8]*D.values[10];
  val[7]  = values[1]*D.values[8] + values[5]*D.values[9] + values[9]*D.values[10];
  val[8]  = values[2]*D.values[8] + values[6]*D.values[9] + values[10]*D.values[10];

  memcpy( (char *)(R.values) , (char *)(val) , 3 * sizeof( float ) );
  memcpy( (char *)(R.values + 4) , (char *)(val + 3) , 3 * sizeof( float ) );
  memcpy( (char *)(R.values + 8) , (char *)(val + 6) , 3 * sizeof( float ) );
  //fprintf( fileLog , "<mat %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f />\n" , 
		//val[0] , val[1] , val[2] , 
		//val[3] , val[4] , val[5] , 
		//val[6] , val[7] , val[8] );
  }

void dm_matrix44::product(float *R , float *D) {
  GLfloat val[16];

  val[0]  = values[0]*D[0] + values[4]*D[1] + values[8]*D[2] + values[12]*D[3];
  val[1]  = values[1]*D[0] + values[5]*D[1] + values[9]*D[2] + values[13]*D[3];
  val[2]  = values[2]*D[0] + values[6]*D[1] + values[10]*D[2] + values[14]*D[3];
  val[3]  = values[3]*D[0] + values[7]*D[1] + values[11]*D[2] + values[15]*D[3];
  
  val[4]  = values[0]*D[4] + values[4]*D[5] + values[8]*D[6] + values[12]*D[7];
  val[5]  = values[1]*D[4] + values[5]*D[5] + values[9]*D[6] + values[13]*D[7];
  val[6]  = values[2]*D[4] + values[6]*D[5] + values[10]*D[6] + values[14]*D[7];
  val[7]  = values[3]*D[4] + values[7]*D[5] + values[11]*D[6] + values[15]*D[7];
  
  val[8]  = values[0]*D[8] + values[4]*D[9] + values[8]*D[10] + values[12]*D[11];
  val[9]  = values[1]*D[8] + values[5]*D[9] + values[9]*D[10] + values[13]*D[11];
  val[10] = values[2]*D[8] + values[6]*D[9] + values[10]*D[10] + values[14]*D[11];
  val[11] = values[3]*D[8] + values[7]*D[9] + values[11]*D[10] + values[15]*D[11];
  
  val[12] = values[0]*D[12] + values[4]*D[13] + values[8]*D[14] + values[12]*D[15];
  val[13] = values[1]*D[12] + values[5]*D[13] + values[9]*D[14] + values[13]*D[15];
  val[14] = values[2]*D[12] + values[6]*D[13] + values[10]*D[14] + values[14]*D[15];
  val[15] = values[3]*D[12] + values[7]*D[13] + values[11]*D[14] + values[15]*D[15];

  memcpy( (char *)R , val , 16 * sizeof( float ) );
}

void dm_matrix44::EulertoMatrix( float X , float Y , float Z , 
				 float roll , float yaw , float pitch ) {
//   glRotatef(aCurve->currentRoll , 1.0, 0.0, 0.0);   
//   glRotatef(aCurve->getcurrentPitch() , 0.0, 0.0, 1.0);
//   glRotatef(aCurve->currentYaw , 0.0, 1.0, 0.0);       

  // printf( "Euler to mat XYZ RYP %.2f %.2f %.2f %.2f %.2f %.2f\n" , X , Y , Z , roll , yaw , pitch );

  float rollRd = roll * DegtoRad;
  float yawRd = yaw * DegtoRad;
  float pitchRd = pitch * DegtoRad;

  float cx = cos(rollRd); float cy = cos(yawRd); float cz = cos(pitchRd);
  float sx = sin(rollRd); float sy = sin(yawRd); float sz = sin(pitchRd);

  // Product = MatRoll/X * MatPitch/Z * MatYaw/Y
  // http://en.wikipedia.org/wiki/Euler_angles

  values[0] = cy * cz;
  values[1] = sx * sy + cx * cy * sz;                    
  values[2] = -cx * sy + cy * sx * sz;              
  values[4] = -sz;
  values[5] = cx * cz;
  values[6] = cz * sx;
  values[8] = cz * sy;
  values[9] = -cy * sx + cx * sy * sz;              
  values[10] = cx * cy + sx * sy * sz;

  // Product = MatRoll/X * MatPitch/Z * MatYaw/Y
  // http://en.wikipedia.org/wiki/Euler_angles
  // values[0] = cx * cz - sx * sy * sz;
  // values[1] = cz * sx + cx * sy * sz;                    
  // values[2] = -cy * sz;              
  // values[4] = -cy * sx;
  // values[5] = cx * cy;
  // values[6] = sy;
  // values[8] = cx * sz + cz * sx * sy;
  // values[9] = sx * sz - cx * cz * sy;              
  // values[10] = cy * cz;

  values[3] = 0.0;
  values[7] = 0.0;
  values[11] = 0.0;
  values[12] = X;
  values[13] = Y;
  values[14] = Z;
  values[15] = 1.0;
  //printf( "Euler to mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , values[0] , values[1] , values[2] , values[3] , values[4] , values[5] , values[6] , values[7] , values[8] , values[9] , values[10] , values[11] , values[12] , values[13] , values[14] , values[15] );
}

void dm_matrix44::ScaletoMatrix( float X , float Y , float Z ) {
  values[0] = X;
  values[1] = 0.0;                    
  values[2] = 0.0;              
  values[4] = 0.0;
  values[5] = Y;
  values[6] = 0.0;
  values[8] = 0.0;
  values[9] = 0.0;              
  values[10] = Z;

  values[3] = 0.0;
  values[7] = 0.0;
  values[11] = 0.0;
  values[12] = 0.0;
  values[13] = 0.0;
  values[14] = 0.0;
  values[15] = 1.0;
}

void dm_matrix44::TranslationtoMatrix( float X , float Y , float Z ) {
  values[0] = 1.0;
  values[1] = 0.0;                    
  values[2] = 0.0;              
  values[4] = 0.0;
  values[5] = 1.0;
  values[6] = 0.0;
  values[8] = 0.0;
  values[9] = 0.0;              
  values[10] = 1.0;

  values[3] = 0.0;
  values[7] = 0.0;
  values[11] = 0.0;
  values[12] = X;
  values[13] = Y;
  values[14] = Z;
  values[15] = 1.0;
}

void dm_matrix44::QuaterniontoMatrix( float W , float X , float Y , float Z ) {
  float xx      = X * X;
  float xy      = X * Y;
  float xz      = X * Z;
  float xw      = X * W;
  
  float yy      = Y * Y;
  float yz      = Y * Z;
  float yw      = Y * W;
  
  float zz      = Z * Z;
  float zw      = Z * W;
  
  values[0]  = 1 - 2 * ( yy + zz );
  values[1]  =     2 * ( xy - zw );
  values[2] =     2 * ( xz + yw );
  
  values[4]  =     2 * ( xy + zw );
  values[5]  = 1 - 2 * ( xx + zz );
  values[6]  =     2 * ( yz - xw );
  
  values[8]  =     2 * ( xz - yw );
  values[9]  =     2 * ( yz + xw );
  values[10] = 1 - 2 * ( xx + yy );
  
  values[3] = 0.0;
  values[7] = 0.0;
  values[11] = 0.0;
  values[12] = 0.0;
  values[13] = 0.0;
  values[14] = 0.0;
  values[15] = 1.0;
  //printf( "Euler to mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , values[0] , values[1] , values[2] , values[3] , values[4] , values[5] , values[6] , values[7] , values[8] , values[9] , values[10] , values[11] , values[12] , values[13] , values[14] , values[15] );
}

void dm_matrix44::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<mat %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f />\n" , values[0] , values[1] , values[2] , values[3] , values[4] , values[5] , values[6] , values[7] , values[8] , values[9] , values[10] , values[11] , values[12] , values[13] , values[14] , values[15] );
}

void dm_matrix44::MatrixtoEuler( float * X , float * Y , float * Z , 
				 float * roll , float * yaw , float * pitch ) {
//   glRotatef(aCurve->currentRoll , 1.0, 0.0, 0.0);   
//   glRotatef(aCurve->getcurrentPitch() , 0.0, 0.0, 1.0);
//   glRotatef(aCurve->currentYaw , 0.0, 1.0, 0.0);       

//   float rollRd = roll * DegtoRad;
//   float yawRd = yaw * DegtoRad;
//   float pitchRd = pitch * DegtoRad;

//   cx = cos(rollRd); cy = cos(yawRd); cz = cos(pitchRd);
//   sx = sin(rollRd); sy = sin(yawRd); sz = sin(pitchRd);

  // Product = MatRoll/X * MatPitch/Z * MatYaw/y

  // from: values[4] = -sz;
  *pitch = asin( -values[4] ) * RadtoDeg;

  if( *pitch  < 90 ) {
    // from: values[6] = cz * sx;
    // from: values[5] = cx * cz;
    // => tx = values[6] / values[5]
    // from: values[8] = cz * sy;
    // from: values[0] = cy * cz;
    // => ty = values[8] / values[0]
    if( *pitch  > -90 ) {
      *roll = atan2( values[6] , values[5] ) * RadtoDeg;
      *yaw = atan2( values[8] , values[0] ) * RadtoDeg;
    }
    else {
      // not a unique solution for *pitch == z == -90 => cz = 0/sz = -1
      // from: values[2] = -cx * sy - cy * sx = -s(x+y);              
      // from: values[10] = cx * cy - sx * sy = c(x+y);
      // => t(x+y) = -values[2] / values[10]
      // we choose y = 0
      *roll = atan2( -values[2] , values[10] ) * RadtoDeg;
      *yaw = 0;
    }
  }
  else {
    // not a unique solution for *pitch == z == 90 => cz = 0/sz = 1
    // from: values[2] = -cx * sy + cy * sx = -s(x-y);              
    // from: values[10] = cx * cy + sx * sy = c(x-y);
    // => t(x-y) = -values[2] / values[10]
    // we choose y = 0
    *roll = atan2( -values[2] , values[10] ) * RadtoDeg;
    *yaw = 0;
  }

  *X = values[12];
  *Y = values[13];
  *Z = values[14];
//   printf( "col 4: %.2f , %.2f , %.2f , %.2f\n" , 
// 	  values[12] , values[13] , values[14] , values[15] );
}

dm_matrix44 dm_matrix44::operator * (dm_matrix44 &Droite) {
  dm_matrix44 r;
  product(r, Droite);
  return (dm_matrix44(r)); 
}
void dm_matrix44::operator=(dm_matrix44& m) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] = m.values[ind];
  }
}
bool dm_matrix44::operator!=(dm_matrix44& m) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    if(values[ind] != m.values[ind] )
      return true;
  }
  return false;
}
void dm_matrix44::operator+=(dm_matrix44& m) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    values[ind] += m.values[ind];
  }
}
void dm_matrix44::operator+=(dm_vector3& v) {
  values[12] += v.x;
  values[13] += v.y;
  values[14] += v.z;
}
int dm_matrix44::operator==(dm_matrix44& m) {
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    if( values[ind] != m.values[ind] ) {
      return false;
    }
  }
  return true;
}

GLboolean invert_matrix(const GLfloat * m, GLfloat * out) {
/* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { GLfloat *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

   GLfloat wtmp[4][8];
   GLfloat m0, m1, m2, m3, s;
   GLfloat *r0, *r1, *r2, *r3;

   r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

   r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
      r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
      r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
      r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
      r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
      r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
      r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
      r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
      r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
      r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
      r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
      r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

   /* choose pivot - or die */
   if (fabs(r3[0]) > fabs(r2[0]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[0]) > fabs(r1[0]))
      SWAP_ROWS(r2, r1);
   if (fabs(r1[0]) > fabs(r0[0]))
      SWAP_ROWS(r1, r0);
   if (0.0 == r0[0])
      return GL_FALSE;

   /* eliminate first variable     */
   m1 = r1[0] / r0[0];
   m2 = r2[0] / r0[0];
   m3 = r3[0] / r0[0];
   s = r0[1];
   r1[1] -= m1 * s;
   r2[1] -= m2 * s;
   r3[1] -= m3 * s;
   s = r0[2];
   r1[2] -= m1 * s;
   r2[2] -= m2 * s;
   r3[2] -= m3 * s;
   s = r0[3];
   r1[3] -= m1 * s;
   r2[3] -= m2 * s;
   r3[3] -= m3 * s;
   s = r0[4];
   if (s != 0.0) {
      r1[4] -= m1 * s;
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r0[5];
   if (s != 0.0) {
      r1[5] -= m1 * s;
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r0[6];
   if (s != 0.0) {
      r1[6] -= m1 * s;
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r0[7];
   if (s != 0.0) {
      r1[7] -= m1 * s;
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[1]) > fabs(r2[1]))
      SWAP_ROWS(r3, r2);
   if (fabs(r2[1]) > fabs(r1[1]))
      SWAP_ROWS(r2, r1);
   if (0.0 == r1[1])
      return GL_FALSE;

   /* eliminate second variable */
   m2 = r2[1] / r1[1];
   m3 = r3[1] / r1[1];
   r2[2] -= m2 * r1[2];
   r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3];
   r3[3] -= m3 * r1[3];
   s = r1[4];
   if (0.0 != s) {
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
   }
   s = r1[5];
   if (0.0 != s) {
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
   }
   s = r1[6];
   if (0.0 != s) {
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
   }
   s = r1[7];
   if (0.0 != s) {
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
   }

   /* choose pivot - or die */
   if (fabs(r3[2]) > fabs(r2[2]))
      SWAP_ROWS(r3, r2);
   if (0.0 == r2[2])
      return GL_FALSE;

   /* eliminate third variable */
   m3 = r3[2] / r2[2];
   r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
      r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

   /* last check */
   if (0.0 == r3[3])
      return GL_FALSE;

   s = 1.0F / r3[3];		/* now back substitute row 3 */
   r3[4] *= s;
   r3[5] *= s;
   r3[6] *= s;
   r3[7] *= s;

   m2 = r2[3];			/* now back substitute row 2 */
   s = 1.0F / r2[2];
   r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
      r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
   m1 = r1[3];
   r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
      r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
   m0 = r0[3];
   r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
      r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

   m1 = r1[2];			/* now back substitute row 1 */
   s = 1.0F / r1[1];
   r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
      r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
   m0 = r0[2];
   r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
      r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

   m0 = r0[1];			/* now back substitute row 0 */
   s = 1.0F / r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
      r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

   MAT(out, 0, 0) = r0[4];
   MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6];
   MAT(out, 0, 3) = r0[7], MAT(out, 1, 0) = r1[4];
   MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6];
   MAT(out, 1, 3) = r1[7], MAT(out, 2, 0) = r2[4];
   MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6];
   MAT(out, 2, 3) = r2[7], MAT(out, 3, 0) = r3[4];
   MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6];
   MAT(out, 3, 3) = r3[7];

   return GL_TRUE;

#undef MAT
#undef SWAP_ROWS
}

bool gaussian_elimination( float * m, float * out , int matSize ) {

/* NB. we assume COLUMN major Matrices. */
#define MAT(r,c) m[(c)*matSize+(r)]

  // prints the initial matrix
  // printf( "Initial matrix (by rows)\n" );
  // for( int i = 0 ; i < matSize ; i++ ) {
  //   printf( "Row %d: ", i );
  //   for( int k = 0 ; k < (matSize + 1) ; k++ ) {
  //     printf( "%.3f ", MAT(i,k) );
  //   }
  //   printf( "\n" );
  // }	   
  // printf( "\n" );

  for( int i = 0 ; i < matSize - 1 ; i++ ) {
    // Find pivot in column j, starting in row i:
    float max_val = MAT(i,i);
    int max_ind = i;
    for( int j = i+1 ; j < matSize ; j++ ) {
      float val = MAT(j,i);
      if( fabs(val) > fabs(max_val) ) {
	max_val = val;
	max_ind = j;
      }
    }
    if( max_val != 0 ) {
      // switches rows i and max_ind; 
      // but the value of max_val remains unchanged
      for( int k = i ; k < (matSize + 1) ; k++ ) {
	float tmp = MAT(max_ind,k);
	MAT(max_ind,k) = MAT(i,k);
	MAT(i,k) = tmp;
      }

      // Now MAT(i,j) contains max_val
      // divide row i by max_val;
      // -> MAT(i,i) will have the value 1
      for( int k = i ; k < (matSize + 1) ; k++ ) {
	MAT(i,k) /= max_val;
      }

      // triangulation of matrix with coefficients
      // MAT(j,*) = MAT(j,*) - MAT(j,i) * MAT(i,*);
      for (int j = i + 1 ; j < matSize ; j++ ) {
	float coef = MAT(j,i);
	for( int k = i ; k < (matSize + 1) ; k++ ) {
	  MAT(j,k) = MAT(j,k) - coef * MAT(i,k);
	  // Now MAT(j,i) (with j > i) will be 0, since 
	  // MAT(j,i) - MAT(j,i) * MAT(i,i) 
	  // = MAT(j,i) - MAT(j,i) * 1 = 0
	}
      }

    }
    // non non zero pivot found
    else {
      return false;
    }
  }

  // prints the triangular matrix
  // printf( "Triangular matrix (by rows)\n" );
  // for( int i = 0 ; i < matSize ; i++ ) {
  //   printf( "Row %d: ", i );
  //   for( int k = 0 ; k < (matSize + 1) ; k++ ) {
  //     printf( "%.3f ", MAT(i,k) );
  //   }
  //   printf( "\n" );
  // }	   
  // printf( "\n" );

  // using back-substitution, each unknown can be solved for.
  for(int i = (matSize - 1) ; i >= 0; i-- ) {
    // constant (right hand side of the system)
    out[i] = -MAT(i,matSize);
    // back-substitution of the computed solutions
    for(int k = (i+1) ; k < matSize ; k++) {
      out[i] -= MAT(i,k) * out[k];
    }
    out[i] /= MAT(i,i);
  }
   
  return true;

#undef MAT
}

void transpose( float * m , int matSize ) {
  for( int i = 0 ; i < matSize ; i++ ) {
    for( int j = 0 ; j < i ; j++ ) {
      float tmp = m[i*matSize+j];
      m[i*matSize+j] = m[j*matSize+i];
      m[j*matSize+i] = tmp;
    }
  }
}   

dm_matrix44 dm_matrix44::operator ~( void ) { 
  float inv[16];

  invert_matrix(values, inv);

  return (dm_matrix44(inv));
}



