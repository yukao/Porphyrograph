/*! \file pg-strings.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-strings.h
 * 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT NY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef PG_STRINGS_H
#define PG_STRINGS_H

#define   XMLAttrChLength    128
#define   XMLAttrValLength   1024
#define   XMLNbMaxAttributes  30

#define   NumberMaximalLength 64

#define _MinAlphaAcc -127
#define _MaxAlphaAcc -105

/// determine si un caractere est alphabetique
#define _Alpha(c) ((c >= 'a' && c <= 'z') \
                     || (c >= 'A' && c <= 'Z') \
                     || (c >= _MinAlphaAcc && c <= _MaxAlphaAcc))

/// determine si un caractere est numerique
#define _Num(c) (c >= '0' && c <= '9')

/// determine si un caractere est alphanumerique
#define _Alphanum(c) (_Alpha(c) || _Num(c))

#define _Return(c) (c == '\n' || c == 13)
#define _SpaceChar(c) (c == ' ' || _Return(c) || c == '\t')

enum DataType{ DataTypeString = 0, DataTypeInt , DataTypeBool , DataTypeDouble , DataTypeTexture , EmptyDataType };
/// first letter of OSC data types correspond to
/// first letter of VC data types
extern const char *DataTypeStrings[EmptyDataType + 1];

enum numericalMode{ PG_DECIMAL , PG_RANDOM };

// time management
double RealTime( void );
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif

// rounding function
double my_round(const double x);

// operations
enum Operator{ Assign = 0, Add , Sub , Mul , Div , Modulo , EmptyOperator };
enum BooleanOperator{ Equal = 0, Different , LessOrEqual , GreaterOrEqual , Less , Greater , EmptyBooleanOperator };

typedef void (*FloatBinOp) ( float & , float );
typedef void (*IntBinOp) ( int & , int );
extern FloatBinOp FloatOperators[EmptyOperator];
extern IntBinOp IntOperators[EmptyOperator];
extern const char *OperatorString[EmptyOperator + 1];

extern const char *BooleanOperatorString[EmptyBooleanOperator + 1];

// OPerators
Operator GetOperator( char *tag );
BooleanOperator GetBooleanOperator( char *tag );
bool booleanNumericalExpression( float operand1 , BooleanOperator oper , 
				 float operand2 );
bool booleanStringExpression( char *operand1 , BooleanOperator oper , 
			      char * operand2 );
// @}

// wildcarding
bool inCharSet( char aChar , char *charSet );

// string comparison
bool stringEqual( char *searchedId , char *refId );


// error report
void ReportError( char *errorString );

// opengl error
int printOglError( int no );

// smallest POT above or equal to x
int smallestPOT( int x );

// my arithmetic classes so that I can use pointers to basic operators
void assign( float & oper , float arg );
void assignI( int & oper , int arg );
void mul( float & oper , float arg );
void mulI( int & oper , int arg );
void div( float & oper , float arg );
void divI( int & oper , int arg );
void add( float & oper , float arg );
void addI( int & oper , int arg );
void sub( float & oper , float arg );
void subI( int & oper , int arg );
// @}

// formula_interpertation
// code from The Code Project:
// Iluska:
// http://www.codeproject.com/script/profile/whos_who.asp?vt=arts&id=42339
double Addition(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Substraction(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Multiplication(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Division(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);

// oper
double Mod(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Max(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Min(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Round(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Floor(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Abs(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Sign(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Now(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double IsFalse(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double IsTrue(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double CurrentFrame(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);

typedef double (*TThreeArgFnPtr) (double * in_pdArg1,double* in_pdArg2,
				  double* in_pdArg3);

////////////////////////////////////////////////////////////
// CLASS FOR PARSING ARITHMETIC EXPRESSIONS
////////////////////////////////////////////////////////////
const int g_constNumberOfOperators = 19;

struct SOperator {
  char m_szSignature[50];
  unsigned short m_usPriority, m_usNumberOfOps;
  TThreeArgFnPtr m_pRegisteredFn;

  SOperator(const char* in_pszSignature, unsigned short in_usPriority, 
	    unsigned short in_usNumberOfOps, TThreeArgFnPtr in_pRegisteredFn) 
  {
    strncpy(m_szSignature,in_pszSignature,50);
    m_usPriority = in_usPriority;
    m_usNumberOfOps = in_usNumberOfOps;
    m_pRegisteredFn = in_pRegisteredFn;
  }
};

extern SOperator* predefinedOperators[g_constNumberOfOperators];

class pg_CCalculator {
  SOperator** my_predefOperators;

  int indChar;

  char * outString;
  unsigned int outString_length;

  std::vector <char *> symbolsStack;
  std::vector <double> valuesStack;
  std::vector <int>    sizeStack;

  void ToPostfix( int *fileDepth , int *p_c  ,
		  int p_withSpaceCommentsInclude , 
		  char *charstring , int *ind , bool atRunTime );
  void Process( bool atRunTime , char * outputString );
  void ProcessTable( bool atRunTime , char * outputString );
  
 public:
  pg_CCalculator( char * ch );

  ~pg_CCalculator( void );

  double Calc(int *fileDepth , int *p_c  , 
	      int p_withSpaceCommentsInclude , 
	      char *charstring , int *ind , bool atRunTime  ,
	      char * outputString );
  void CalcTable( double *scalars , int size ,
		  int *fileDepth , int *p_c  , 
		  int p_withSpaceCommentsInclude , 
		  char *charstring , int *ind , bool atRunTime  ,
		  char * outputString );
};

extern pg_CCalculator m_Calculator;

////////////////////////////////////////////////////////////
// STRING ALLOCATION AND COPY
////////////////////////////////////////////////////////////

// string allocation and copy
void InitString( char **target );
void InitString( char **target , int str_l );
void StrCpy( char **target , const char *source );
void StrCpyTest( char *target , const char *source );
void StrNCpy( char **target , const char *source , unsigned int length );
void StrCat( char **source , const char *addition , unsigned int * sourceLength );

// class for parsing XML tags
class pg_XMLTag {
 public:
  char *tag; 
  char **attributes;
  char **values; 
  int  nbAttributes;
  int  tagType;
  
  pg_XMLTag();
  ~pg_XMLTag();

  void initXMLTag( void );

  void copyXMLTag( pg_XMLTag *source );
  void copyAndSubstitueExpressions( pg_XMLTag *source );
  void copyAndSubstitueExpressionsbutNotTable( pg_XMLTag *source ,
					       char **argList , int argCount );

  void print( FILE *file , int depth );

  // reads an XML tag and allocates memory
  bool  ParseTagXML( int *fileDepth , int *p_c ,
		     int p_withSpaceCommentsInclude  , 
		     char *charstring , int *ind );

  // read scalar values from XML elements 
  char *getAttributeStringValue( const char *attributeName );

   // transforms the string associated with an attribute into an int
  int getAttributeIntValue( const char *attributeName , bool isScriptCommand );

  // transforms the string associated with an attribute into a double
  double getAttributeDoubleValue( const char *attributeName , bool isScriptCommand );

  // checks whether an attribute is in an element
  int hasAttribute( const char *attributeName );
  numericalMode getModeValue( void );
  void BooleanFieldValue ( const char *attributeName , bool *returnValue );
  void deallocateXMLTag( void );
};

//reads a char
int     ReadChar( int *fileDepth  , char *charstring , int *ind );

// comment reading
void    SpaceCommentsInclude( int *fileDepth , int *p_c  , char *charstring , int *ind );

// reading a string with a final character or end of string
// The final character will not be jumped over
int     ReadStringWithTermination( int *fileDepth , int *p_c , 
				   char **p_targetstring ,
				   int p_frontierRight ,
				   int p_withSpaceCommentsInclude , 
				   char *charstring , int *ind );


//reading a string between 2 frontier characters
int     ReadStringWithFrontiers( int *fileDepth , int *p_c , char **p_targetstring ,
				 int p_frontierLeft , 
				 int p_frontierRight ,
				 int p_withSpaceCommentsInclude  , 
				 char *charstring , int *ind );

// reading a string between 2 frontier characters 
int     ReadBalancedStringWithFrontiers( int *fileDepth , 
					 int *p_c , char *p_targetstring ,
					 int p_size , int p_frontierLeft , 
					 int p_frontierRight ,
					 bool p_withSpaceCommentsInclude , 
					 bool p_keepFrontiers  , 
					 char *charstring , int *ind );

int readAttributeValue( int *fileDepth , int *p_c , char *valueString, 
			char *charstring , int *ind );

/// reading of the charstring characteristics of an identifier.
// It may not include some features alphanum
void     ReadIDWithCarSpec( int *fileDepth , int *p_c , 
			    char **p_targetstring ,
			    const char *p_lisCarSpec ,
			    int p_withSpaceCommentsInclude  , 
			    char *charstring , int *ind );

// gets an expected character
int     ExpectsChar( int *fileDepth , int p_modele , int  *p_c ,
                               int p_withSpaceCommentsInclude  , 
			       char *charstring , int *ind );

//  transforms a string into a double
// atRunTime is only true for strings that are dynamically
// evaluated
double   stringToDoubleValue( char *aString , bool atRunTime );
int      stringToIntValue( char *aString , bool atRunTime );

/// scalar
float    ReadExpression( int *fileDepth , int *p_c  ,
			 int p_withSpaceCommentsInclude  , char *charstring , 
			 int *ind , bool atRunTime ,
			 char * outputString );

/// int table
void    ReadExpression( int *scalars , int size ,
			 int *fileDepth , int *p_c  ,
			 int p_withSpaceCommentsInclude , 
			 char *charstring , int *ind , bool atRunTime ,
			 char * outputString );

/// double table
void    ReadExpression( double *scalars , int size ,
			int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			char *charstring , int *ind , bool atRunTime ,
			 char * outputString );

// reads an integer
long     ReadInteger( int *fileDepth , int *p_c , int p_withSpaceCommentsInclude  , 
		     char *charstring , int *ind );

// conversion of a node/attribute reference into a number
float    ReferenceNodeAttributeValue( const char * nodeIdAttributeString ,
				      bool atRunTime ,
				      char * outputString );
// reads a numerical value
float    ReadFloat( int *fileDepth , int *p_c , int p_withSpaceCommentsInclude  , 
		   char *charstring , int *ind , bool atRunTime );

// reads an interval
void     ReadInterval( int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			float *inf , float *sup , bool atRunTime );

// expect character
int      ExpectsChar( int *fileDepth , 
		      char *p_modele , int *p_c  , 
		      int p_withSpaceCommentsInclude , 
		      char *charstring , int *ind );

void MemoryUsage( void );

#endif
