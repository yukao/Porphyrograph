/*! \file dm-variables.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-variables.h
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

#ifndef DM_VARIABLES_H
#define DM_VARIABLES_H

enum VariableType{ dm_ScalarType = 0, dm_TableType , dm_MatrixType , EmptyVariableType };
extern const char *VariableTypeString[EmptyVariableType + 1];
enum SmoothingFunction{ dm_Spline = 0, dm_Polynom , EmptySmoothingFunction };
extern const char *SmoothingFunctionString[EmptySmoothingFunction + 1];

/// abstract superclass for interpolation with a single element type
class dm_Variable {
 public:
  char            *id; 
  dm_Variable     *nextVariable;
  dm_Variable     *motherVariable;
  bool            interpolatorVariable;

  virtual bool getParameters( char *attribute , 
			      DataType * dataType ,
			      double * valDouble ,
			      int * valInt ,
			      bool * valBool ,
			      char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars );
  virtual void   print( FILE *file , int depth );

  // ------------- interpolation ------------------- //
  dm_Variable( void );
  virtual ~dm_Variable( void );

  virtual void interpolate( dm_Variable **tabVariables , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef );
  virtual void interpolate( dm_Variable *n1 , dm_Variable *n2 );

  void operator=(dm_Variable& s);
};

/// scalar node
class dm_Texture;
class dm_ValScalar {
 public:
  DataType     type;
  float        valFloat;
  int          valInt;
  dm_Texture  *materialTexture;
  int	       smoothingDepth;
  char        *valString;
  char        *valHRef;
  float       *smoothingValues;
  float       *smoothingTimes;

  dm_ValScalar( void );
  ~dm_ValScalar( void );
  void clean( void );
  DataType GetType( void );
  void   SetType( DataType aType );
  float  GetNumericalValue( void );
  void   SetValue( float val );
  char  *GetStringValue( void );
  void   print( FILE *file );
  void   SetValue( char *val );
  void   operator=(dm_ValScalar& s);
};

class dm_VariableInterpolator;

class dm_Scalar: public dm_Variable {
 public:
  dm_ValScalar              *value;
  dm_VariableInterpolator   *interp;
  bool                      isInterpolator;

  /*! \name Group smooth
   *  \brief smoothing
   */
  // \{
  /// size of value memory for next value computation
  int                       smoothingDepth;  
   /// function used for computing smoothed valules
  SmoothingFunction         smoothingFunction; 
  // \}

  /// the pointer to the next scalar created. It is used for deletion all the scalar created.
  dm_Scalar*  chineScalar;

  dm_Scalar( void );
  dm_Scalar( dm_VariableInterpolator   *interp );
  virtual ~dm_Scalar( void );

  dm_ValScalar *GetValue( void );
  DataType GetType( void );
  void   SetType( DataType aType );
  float  GetNumericalValue( void );
  void   SetValue( float val );
  char  *GetStringValue( void );
  void   SetValue( char *val );
  char  *GetId( void );
  void   SetId( const char *newId );
  void   print( FILE *file , int depth );
  void   setScalar( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper , IntBinOp operI );
  void   setScalarRefString( dm_XMLTag *XMLtag );
  void   setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			FloatBinOp oper ,
			IntBinOp operI );

  // ------------- interpolation ------------------- //
  virtual void interpolate( dm_Scalar **tabScalar , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef );
  virtual void interpolate( dm_Scalar *n1 , dm_Scalar *n2 );
	  
//  void operator=(dm_Scalar& n);
};

/// table node
class dm_Table: public dm_Variable {
  int                       size;
  dm_ValScalar             *scalars;
 public:
  dm_VariableInterpolator  *interp;
  bool                      isInterpolator;
  dm_Table*			chineTable;	 /// the pointer to the next table for final release
  /*! \name Group smooth
   *  \brief smoothing
   */
  // \{
  /// size of value memory for next value computation
  int                       smoothingDepth;  
  /// function used for computing smoothed valules
  SmoothingFunction         smoothingFunction;  
  // \}

  dm_Table( int nbVals );
  dm_Table( dm_VariableInterpolator   *interp );
  dm_Table( void );
  virtual ~dm_Table( void );

  void  InitScalars( int nbVals );
  float  GetNumericalValue( int index );
  DataType GetType( void );
  dm_ValScalar *GetValue( int index );
  void   SetValue( double *val );
  void   SetValue( float val , int index );
  char  *GetStringValue( int index );
  void   SetValue( char *val , int index );
  char  *GetId( void );
  int    GetSize( void );
  dm_ValScalar   *GetScalars( void );
  void   SetId( const char *newId );
  void printScalars( FILE *file );
  void   print( FILE *file , int depth );
  void   setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			FloatBinOp oper , IntBinOp operI );

  // ------------- interpolation ------------------- //
  virtual void interpolate( dm_Table **tabTable , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef );
  virtual void interpolate( dm_Table *n1 , dm_Table *n2 );
};

/// matrix node
class dm_Matrix: public dm_Variable {
  int               sizeCol;
  int              sizeLine;
  dm_ValScalar    *scalars;
 public:
  dm_VariableInterpolator  *interp;
  bool                      isInterpolator;

  /// the pointer to the next matrix for final release
  dm_Matrix*			chineMatrix;		

  /*! \name Group smooth
   *  \brief smoothing
   */
  // \{
  /// size of value memory for next value computation
  int                       smoothingDepth;  
  /// function used for computing smoothed valules
  SmoothingFunction         smoothingFunction;  
  // \}

  dm_Matrix( int nbVals );
  dm_Matrix( dm_VariableInterpolator   *interp );
  virtual ~dm_Matrix( void );
  float  GetNumericalValue( int index );
  float  GetNumericalValue( int indexLine , int indexCol );
  DataType GetType( void );
  dm_ValScalar *GetValue( int index );
  dm_ValScalar *GetValue( int indexLine , int indexCol );
  void   SetValue( double *val );
  void   SetValue( float val , int index );
  void   SetValue( float val , int indexLine , int indexCol );
  char  *GetStringValue( int index );
  char  *GetStringValue( int indexLine , int indexCol );
  void   SetValue( char *val , int index );
  void   SetValue( char *val , int indexLine , int indexCol );
  char  *GetId( void );
  int    GetSize( void );
  int    GetSizeLine( void );
  int    GetSizeCol( void );
  dm_ValScalar   *GetScalars( void );
  void   SetId( const char *newId );
  void   print( FILE *file , int depth );
  void   setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			FloatBinOp oper , IntBinOp operI );

  // ------------- interpolation ------------------- //
  virtual void interpolate( dm_Matrix **tabMatrix , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef );
  virtual void interpolate( dm_Matrix *n1 , dm_Matrix *n2 );
};


#endif
