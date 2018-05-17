/*! \file dm-IOfile.h
 *  \brief the header for structure ( \a __unnamed_00d2_1 )
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-IOfile.h
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

#ifndef DM_IOFILE_H
#define DM_IOFILE_H

class dm_Shader;

enum ShaderType{ GENERALIZED_SHADER = 0 , EmptyShaderType };
extern const char *ShaderTypeString[EmptyShaderType + 1];

typedef enum
{
  /*! non instantiated value */
  _Nil = -1,
  
} SpecialValues;

extern int NbParameters;
extern char *ParameterValues;
extern char *ParameterStrings;

float dynamicTimeParsing( char *timeString );
char * ParameterValue( char * chaineLue  );

dm_SceneNode* ParseTest( int *fileDepth , int *curChar , dm_XMLTag *XMLtag , 
			 int *nbReturnedNodes );

dm_SceneNode* ParseSet( int *fileDepth , int *curChar , dm_XMLTag *XMLtag , 
		     int *nbReturnedNodes );

/*!
 * \brief parameter substitution: static version
 * \param string			to be added
 * 
 * the initial string is assumed to be long enough for containing the substituted values
 */
char *substituteParameters( char *string );

/*! 
 * \brief parameter substitution: dynamic version
 * \param initialString			to be added
 *
 * the initial string is replaced by a longer one if necessary.
 * It is a better version with dynamic strings
 */
char *SubstituteParameters( char **initialString );

/// copies and subsitute expressions
/// fixed length
char *StrCpyAndSubst( char *targetString , char *initialString , 
		      int lengthTargetString );
char *StrCpyAndSubst( char *targetString , char *initialString , 
		      int lengthTargetString , int precision );
/*!
 * \brief dynamic string
 * \param targetString			to be added
 * \param initialString			to be added
 *
 * copies the initialString in the targetString and
 * replaces all the expressions (signaled by () )
 * by their value in targetString
 */
char *StrCpyAndSubst( char **targetString , char *initialString );
char *StrCpyAndSubst( char **targetString , char *initialString , 
		      int precision );

/// the generic version
char *StrCpyAndSubst( char **variableTargetString , char *fixedTargetString ,
		      char *initialString , int lengthFixedTargetString );
char *StrCpyAndSubst( char **variableTargetString , char *fixedTargetString ,
		      char *initialString , int lengthFixedTargetString ,
		      int precition );

bool containsExpression( char *initialString );

/*!
 * \brief tag expectation
 * \param	fileScene														to be added
 * \param	p_c																	to be added
 * \param	expectedTag[XMLAttrChLength]			to be added
 * \param	chaine															to be added
 * \param	ind																	to be added
 */
void ExpectClosingTagXML( int *fileDepth , int *p_c ,  
			  const char expectedTag[XMLAttrChLength] , 
			  char *chaine , int *ind );
dm_Shader *ParseShader( int *fileDepth , int *p_c , char *ID ,
		     ShaderType shaderType );

/*!
 * \brief parsing a command pattern
 * \param currentCommand			to be added
 * \param aliasPattern						to be added
 * \param isOSC									to be added
 */
void PatternProcessing( dm_Command *currentCommand , char *aliasPattern ,
			bool isOSC );

/*!
 * \brief parsing a node
 * \param p_ptFic								to be added
 * \param p_c										to be added
 * \param isConfigurationFile			to be added
 * \param XMLtag								to be added
 */
dm_SceneNode* ParseNode( int *fileDepth , int *p_c ,
		      bool isConfigurationFile ,
		      dm_XMLTag *XMLtag );

/*!
 * \brief parsing a node (actual node, link or repeat)
 * \param fileScene							to be added
 * \param p_c										to be added
 * \param currentNode						to be added
 * \param nbParsedNodes			to be added
 * \param XMLtag								to be added
 */
void ParseNodeSet( int *fileDepth ,  int *p_c ,
		   dm_SceneNode **currentNode , int *nbParsedNodes ,
		   dm_XMLTag *XMLtag );

/*!
 * \brief parsing a node component
 * \param p_ptFic								to be added
 * \param p_c										to be added
 * \param XMLtag								to be added
 * \param isConfigurationFile			to be added
 * \param returnNode						to be added
 */
dm_SceneNode* ParseComponent( 
         int *fileDepth , int *p_c , dm_XMLTag *XMLtag ,
	 bool isConfigurationFile , dm_SceneNode *returnNode );

/*! 
 * \brief parsing a color
 * \param fileScene			to be added
 * \param p_c						to be added
 * \param aColor				to be added
 * \param XMLtag				to be added
 */
void ParseColor( int *fileDepth , int *p_c , GLfloat *aColor ,
		 dm_XMLTag *XMLtag );

void ParsePoint( int *fileDepth , int *p_c , dm_point3 *aPoint ,
		 dm_XMLTag *XMLtag );
void ParseTexturecoord( int *fileDepth , int *p_c , MeshTexturecoord *aTexturecoord ,
		 dm_XMLTag *XMLtag );
void ParseTabTextureCoord( int *fileDepth , int *p_c , 
			   MeshTexturecoord *tabTextureCoord , int nbTextureCoords );
void ParseTabNormal( int *fileDepth , int *p_c , 
		     dm_vector3 *tabNormal , int nbNormals , 
		     bool normal_normalization );

/*!
 * \brief parsing a vector
 * \param fileScene			to be added
 * \param p_c						to be added
 * \param aVector				to be added
 * \param XMLtag				to be added
 */
void ParseVector( int *fileDepth , int *p_c , dm_vector3 *aVector ,
		 dm_XMLTag *XMLtag );

/*!
 * \brief mesh keypoints
 * \param fileScene							to be added
 * \param p_c										to be added
 * \param nbVertices						to be added
 * \param keypointTransfs				to be added
 * \param keypointSize					to be added
 */
void ParseTabVertex( int *fileDepth , int *p_c , 
		     MeshVertex *tabVertex , int nbVertices );
void ParseTabPoint( int *fileDepth , int *p_c , 
		    dm_point3 *tabPoint , int nbPoints );
void ParseTabNormal( int *fileDepth , int *p_c , 
		     MeshNormal *tabNormal , int nbNormals , 
		     bool normal_normalization );
void ParseTabFace( int *fileDepth , int *p_c , 
		   MeshFace *tabFace_vertex , 
		   MeshFace *tabFace_normal , 
		   MeshFace *tabFace_texturecoord , 
		   int nbFaces ,
		   int nbVertices ,
		   int nbTextureCoords ,
		   int nbNormals ,
		   int nbTangents );

/*!
 * \brief parsing a table of weights
 * \param fileScene						to be added
 * \param p_c									to be added
 * \param tabWeights					to be added
 * \param nbWeights					to be added
 * \param nbComponents			to be added
 * \param atRunTime					to be added
 * \param chaine							to be added
 * \param ind									to be added
 */
void ParseTabWeights( int *fileDepth , int *p_c , 
		      float **tabWeights , 
		      int nbWeights ,
		      int *nbComponents , bool atRunTime ,
		      char *chaine , int *ind );
#endif
