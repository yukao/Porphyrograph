/*! \file dm-strings.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-strings.h
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
#ifndef DM_STRINGS_H
#define DM_STRINGS_H

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

enum NodeType{ AQuad = 0 , AMesh , APoints , APoint, ALine , AStrip , ATransformation , AViewpoint , ATextureStorage , APixelReader , AFBOStorage , AFBDisplay , AEnvironment , AUser , ANodeLink , ANode , EmptyNode };
extern const char *NodeTypeString[EmptyNode + 1];

enum DataType{ DataTypeString = 0, DataTypeInt , DataTypeBool , DataTypeDouble , DataTypeTexture , EmptyDataType };
/// first letter of OSC data types correspond to
/// first letter of VC data types
extern const char *DataTypeStrings[EmptyDataType + 1];

enum numericalMode{ DM_DECIMAL , DM_RANDOM };


/*! \name Group time_management
 *  \brief time management
 */
// @{
double RealTime( void );
#ifdef _WIN32
//#ifndef _TIME_H_
int gettimeofday(struct timeval* tp, void* tzp);
//#endif
#endif
// @}

/*!
 * \brief rounding function
 * \param x the double type vaule needed to be rounded
 * \return the rounded result of the input \a x
 */
double my_round(const double x);

/*! \name Group operations
 *  \brief OPERATORS
 */
// @{
enum Operator{ Assign = 0, Add , Sub , Mul , Div , Modulo , EmptyOperator };
enum BooleanOperator{ Equal = 0, Different , LessOrEqual , GreaterOrEqual , Less , Greater , EmptyBooleanOperator };

typedef void (*FloatBinOp) ( float & , float );
typedef void (*IntBinOp) ( int & , int );
extern FloatBinOp FloatOperators[EmptyOperator];
extern IntBinOp IntOperators[EmptyOperator];
extern const char *OperatorString[EmptyOperator + 1];

extern const char *BooleanOperatorString[EmptyBooleanOperator + 1];
// @}

enum OpenGLMaterialType{ COLOR = 0, TEXTURE , MATERIAL , EmptyMaterial };

/*! \name Group enum_access
 *  \brief enum type access
 */
// @{
NodeType GetNodeType( char *tag );
Operator GetOperator( char *tag );
BooleanOperator GetBooleanOperator( char *tag );
bool booleanNumericalExpression( float operand1 , BooleanOperator oper , 
				 float operand2 );
bool booleanStringExpression( char *operand1 , BooleanOperator oper , 
			      char * operand2 );
// @}

/*! \name Group wildcarding
 *  \brief wildcarding
 */
/*!
  * \brief charset is a RE char set description that terminates with a 
  * \param aChar		to be added
  *	 \param charSet		to be added
  */
bool inCharSet( char aChar , char *charSet );

/*! 
  * \brief string equality
  * \param searchdedId			to be added
  * \param refld							to be added
  */
bool stringEqual( char *searchedId , char *refId );


/*!
  * \brief error report
  *	 \param errorString		the output error string
  */
void ReportError( char *errorString );

/*! \name Group arithmetic
 *  \brief my arithmetic classes so that I can use pointers to basic operators
 */
// @{
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

/*! \name formula_interpertation
 *  \brief arithmetics: formula interpertation
 * 
 *
 * code from The Code Project:
 * Iluska:
 * http://www.codeproject.com/script/profile/whos_who.asp?vt=arts&id=42339
 */
// @{
double Addition(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Substraction(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Multiplication(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Division(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
// @}

/*!
 * \brief matrix/scaler or scalar/matrix operation: + - * /
 * \param valStack				to be added
 * \param sizStack					to be added
 * \param oper						to be added
 */
void   ScalarMatOperation( std::vector <double> * valStack ,  
			   std::vector <int> * sizStack ,
			   int oper ) ; // predefinedOperatorsID

/*!
 * \brief matrix/matrix operation: + -
 * \param valStack				to be added
 * \param sizStack					to be added
 * \param oper						to be added
 */
void   MatMatOperation( std::vector <double> * valStack ,  
			std::vector <int> * sizStack ,
			int oper ) ; // predefinedOperatorsID
void   MatMatMultiplication( std::vector <double> * valStack ,
			     std::vector <int> * sizStack );
void   MatVecMultiplication( std::vector <double> * valStack ,
			     std::vector <int> * sizStack );
void   VecMatMultiplication( std::vector <double> * valStack ,
			     std::vector <int> * sizStack );
void   HomogMatVecMultiplication( std::vector <double> * valStack ,
				  std::vector <int> * sizStack );
void   HomogVecMatMultiplication( std::vector <double> * valStack ,
				  std::vector <int> * sizStack );
void   MatInverse( std::vector <double> * valStack ,
		   std::vector <int> * sizStack );
void   PointProject( std::vector <double> * valStack ,  
		     std::vector <int> * sizStack );
void   PointGluProject( double source[3] ,
			double target[3] );
void   PointUnProject( std::vector <double> * valStack ,  
		       std::vector <int> * sizStack );
void   Homography( std::vector <double> * valStack ,  
		   std::vector <int> * sizStack , int fixedParam ); 
void ToEuler( std::vector <double> * valStack ,  
	      std::vector <int> * sizStack );
void FromEuler( std::vector <double> * valStack ,  
	      std::vector <int> * sizStack );
void   CameraOperation( std::vector <double> * valStack ,  
		       std::vector <int> * sizStack );
double Mod(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Sinus(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Cosine(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Tangent(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Asinus(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Acosine(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Atangent(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Logarithm(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Max(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Min(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Power(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Random(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Round(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Floor(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Sqrt(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Abs(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Sign(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double Now(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double IsFalse(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double IsTrue(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
double CurrentFrame(double * in_pdArg1,double* in_pdArg2,double* in_pdDummy);
void VecNorm(std::vector <double> * valStack ,  
	     std::vector <int> * sizStack); 

typedef double (*TThreeArgFnPtr) (double * in_pdArg1,double* in_pdArg2,
				  double* in_pdArg3);

////////////////////////////////////////////////////////////
// CLASS FOR PARSING ARITHMETIC EXPRESSIONS
////////////////////////////////////////////////////////////
const int g_constNumberOfOperators = 45;

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

class dm_Table;
class dm_CCalculator {
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
  dm_CCalculator( char * ch );

  ~dm_CCalculator( void );

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

extern dm_CCalculator m_Calculator;

////////////////////////////////////////////////////////////
// STRING ALLOCATION AND COPY
////////////////////////////////////////////////////////////

/*!
 * \brief string allocation and copy
 * \param target			to be added
 */
void InitString( char **target );
void InitString( char **target , int str_l );
void StrCpy( char **target , const char *source );
void StrCpyTest( char *target , const char *source );
void StrNCpy( char **target , const char *source , unsigned int length );
void StrCat( char **source , const char *addition , unsigned int * sourceLength );

/*!
 *	\brief class for parsing XML tags
 */
class dm_XMLTag {
 public:
  char *tag; 
  char **attributes;
  char **values; 
  int  nbAttributes;
  int  tagType;
  
  dm_XMLTag();
  ~dm_XMLTag();

  void initXMLTag( void );
  void substituteParametersXML( void );

  void copyXMLTag( dm_XMLTag *source );
  void copyAndSubstitueExpressions( dm_XMLTag *source );
  void copyAndSubstitueExpressionsbutNotTable( dm_XMLTag *source ,
					       char **argList , int argCount );

  void print( FILE *file , int depth );

  /*!
   * \brief reads an XML tag and allocates memory
   * \param p_ptFic												to be added
   * \param p_c													to be added
   * \param p_withSpaceCommentsInclude			to be added
   * \param charstring										to be added
   * \param ind														to be added
   */
  bool  ParseTagXML( int *fileDepth , int *p_c ,
		     int p_withSpaceCommentsInclude  , 
		     char *charstring , int *ind );

  /*!
   * \brief read scalar values from XML elements 
   * \param attributeName				to be added
   */
  char *getAttributeStringValue( const char *attributeName );

   /*! 
    * \brief transforms the string associated with an attribute into an int
    * \param attributeName						to be added
	* \param isScriptCommand				to be added
	*
	* transforms the string associated with an attribute into an int
	* atRunTime is only true for strings that are dynamically
    * evaluated: xlink:value are given by a dynamic node attribute value
	*/
  int getAttributeIntValue( const char *attributeName , bool isScriptCommand );

  /*!
   * \brief transforms the string associated with an attribute into a double
   * \param attributeName						to be added
   * \param isScriptCommand				to be added
   *
   * transforms the string associated with an attribute into a double
   * atRunTime is only true for strings that are dynamically
   * evaluated: xlink:value are given by a dynamic node attribute value
   */
  double getAttributeDoubleValue( const char *attributeName , bool isScriptCommand );

  /*!
   * \brief read table values from XML elements  
   * \param scalars							to be added
   * \param size								to be added
   * \param attributeName			to be added
   * \param atRunTime					to be added
   */
  void getAttributeTableStringValue( char  ***scalars , int size , 
				     const char *attributeName , bool atRunTime );
  void getAttributeTableIntValue( int  *scalars , int size , 
				  const char *attributeName , bool isScriptCommand );
  void getAttributeTableBoolValue( bool  *scalars , int size ,
				   const char *attributeName , 
				   bool atRunTime );
  void getAttributeTableDoubleValue( double *scalars , int size , 
				     const char *attributeName , 
				     bool isScriptCommand );

  /*! 
   * \brief checks whether an attribute is in an element
   * \param attributeName			to be added
   */
  int hasAttribute( const char *attributeName );
  numericalMode getModeValue( void );
  void BooleanFieldValue ( const char *attributeName , bool *returnValue );
  void deallocateXMLTag( void );
};

/*! \name Group string_management
 *  \brief string management
 */
// @{
/*! 
 * \brief next input char
 * \param p_ptFic				to be added
 *	\param charstring			to be added
 *	\param ind						to be added
 */
int     ReadChar( int *fileDepth  , char *charstring , int *ind );

/*!
 * \brief next character not white entree comments also jumps between "" 
 * \param p_ptFic				to be added
 *	\param p_c						to be added
 *	\param charstring			to be added
 *	\param ind						to be added
 *
 * PROCHAIN CARACTERE NON BLANC EN ENTREE
 * SAUTE EGALEMENT LES COMMENTAIRES ENTRE ""
 */
void    SpaceCommentsInclude( int *fileDepth , int *p_c  , char *charstring , int *ind );

/*! 
 * \brief reading a string with a final character or end of string
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_targetstring									to be added
 * \param p_size												to be added
 * \param p_frontierRight									to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 * reading a string with a final character or end of string. 
 * The final character will not be jumped over
 */
int     ReadStringWithTermination( int *fileDepth , int *p_c , 
				   char **p_targetstring ,
				   int p_frontierRight ,
				   int p_withSpaceCommentsInclude , 
				   char *charstring , int *ind );


/*! 
 * \brief reading a table of  string between frontiers, e.g.,('a','bcd','efg')
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_targetstring									to be added
 * \param p_size												to be added
 * \param p_frontierLeft									to be added
 * \param p_frontierRight									to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 * reading a string with a final character or end of string. 
 * The final character will not be jumped over
 */
int     ReadTableStringWithFrontiers( int *fileDepth , 
				 int *p_c , char ***p_tablecharstring ,
                                 int size ,
				 int p_size , int p_frontierLeft , 
				 int p_frontierRight ,
				 int p_withSpaceCommentsInclude , 
				      char *charstring , int *ind );

/*! 
 * \brief reading a string between 2 frontier characters
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_targetstring									to be added
 * \param p_size												to be added
 * \param p_frontierRight									to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 */
int     ReadStringWithFrontiers( int *fileDepth , int *p_c , char **p_targetstring ,
				 int p_frontierLeft , 
				 int p_frontierRight ,
				 int p_withSpaceCommentsInclude  , 
				 char *charstring , int *ind );

/*! 
 * \brief reading a string between 2 frontier characters 
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_targetstring									to be added
 * \param p_size												to be added
 * \param p_frontierLeft									to be added
 * \param p_frontierRight									to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param p_keepFrontiers								to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 */
int     ReadBalancedStringWithFrontiers( int *fileDepth , 
					 int *p_c , char *p_targetstring ,
					 int p_size , int p_frontierLeft , 
					 int p_frontierRight ,
					 bool p_withSpaceCommentsInclude , 
					 bool p_keepFrontiers  , 
					 char *charstring , int *ind );

int readAttributeValue( int *fileDepth , int *p_c , char *valueString, 
			char *charstring , int *ind );

/*!
 * \brief reading of the charstring characteristics of an identifier.
 * It may not include some features alphanum
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_targetstring									to be added
 * \param p_size												to be added
 * \param p_lisCarSpec									to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 * 
 * LECTURE DE LA CHARSTRING DE CARACTERES D'UN IDENTIFICATEUR
 * ELLE PEUT COMPORTER CERTAINS CARACT NON ALPHANUM
 */
void     ReadIDWithCarSpec( int *fileDepth , int *p_c , 
			    char **p_targetstring ,
			    const char *p_lisCarSpec ,
			    int p_withSpaceCommentsInclude  , 
			    char *charstring , int *ind );

/*!
 * \brief play of the nature
 * \param p_ptFic												to be added
 * \param p_modele											to be added
 * \param p_c														to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 * LECTURE D'UN CARACTERE ATTENDU
 */
int     ExpectsChar( int *fileDepth , int p_modele , int  *p_c ,
                               int p_withSpaceCommentsInclude  , 
			       char *charstring , int *ind );

/*!
 * \brief transforms a string into a double
 * \param aString				to be added
 * \param atRunTime		to be added
 *
 * transforms a string into a double
 * atRunTime is only true for strings that are dynamically
 * evaluated: xlink:value are given by a dynamic node attribute value
 */
double   stringToDoubleValue( char *aString , bool atRunTime );
void stringToTableDoubleValue( double *scalars , int size ,
			       char *aString , bool atRunTime );

/*!
 * \brief transforms a string into a integer
 * \param aString				to be added
 * \param atRunTime		to be added
 *
 * transforms a string into a double
 * atRunTime is only true for strings that are dynamically
 * evaluated: xlink:value are given by a dynamic node attribute value
 */
int      stringToIntValue( char *aString , bool atRunTime );
void stringToTableIntValue( int *scalars , int size ,
			    char *aString  , bool atRunTime );
void stringToTableBoolValue( bool *scalars , int size ,
			     char *aString  , bool atRunTime );


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

/*!
 * \brief play an entire
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 * LECTURE D'UN ENTIER
 */
long     ReadInteger( int *fileDepth , int *p_c , int p_withSpaceCommentsInclude  , 
		     char *charstring , int *ind );

/*!
 * \brief conversion of a node/attribute reference into a number
 * \param nodeIdAttributeString		to be added
 * \param atRunTime							to be added
 * \param outputString							to be added
 */
float    ReferenceNodeAttributeValue( const char * nodeIdAttributeString ,
				      bool atRunTime ,
				      char * outputString );
class dm_ValScalar;

/*!
 * \brief conversion of a node/attribute reference into a table pointer
 * \param attributeOrVariableString			to be added
 * \param atRunTime									to be added
 * \param tableOrMatrixSize						to be added
 * \param tableOrMatrixType					to be added
 */
dm_ValScalar * ReferenceNodeAttributeTable( const char * attributeOrVariableString ,
				      bool atRunTime ,
				      int * tableOrMatrixSize ,
				      DataType * tableOrMatrixType );

/*!
 * \brief reads a numerical value
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 * \param atRunTime										to be added
 */
float    ReadFloat( int *fileDepth , int *p_c , int p_withSpaceCommentsInclude  , 
		   char *charstring , int *ind , bool atRunTime );

/*!
 * \brief play of a real
 * \param p_ptFic												to be added
 * \param p_c														to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param inf															to be added
 * \param sup														to be added
 * \param atRunTime										to be added
 *
 * LECTURE D'UN REEL
 */
void     ReadInterval( int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			float *inf , float *sup , bool atRunTime );

/*!
 * \brief play an expected charstring
 * \param p_ptFic												to be added
 * \param p_modele											to be added
 * \param p_c														to be added
 * \param p_withSpaceCommentsInclude			to be added
 * \param charstring											to be added
 * \param ind														to be added
 *
 * LECTURE D'UNE CHARSTRING ATTENDUE
 */
int      ExpectsChar( int *fileDepth , 
		      char *p_modele , int *p_c  , 
		      int p_withSpaceCommentsInclude , 
		      char *charstring , int *ind );

void MemoryUsage( void );

#endif
// @}
