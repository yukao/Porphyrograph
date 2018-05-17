/*! \file pg-IOfile.h
 *  \brief the header for structure ( \a __unnamed_00d2_1 )
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-IOfile.h
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

#ifndef PG_IOFILE_H
#define PG_IOFILE_H

class pg_Shader;
class pg_Environment;

int BooleanToInt( bool bool_val );
extern const char *BooleanString[3];

enum ShaderType{ GENERALIZED_SHADER = 0 , EmptyShaderType };
extern const char *ShaderTypeString[EmptyShaderType + 1];

typedef enum
{
  /*! non instantiated value */
  _Nil = -1,
  
} SpecialValues;

float dynamicTimeParsing( char *timeString );

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
pg_Shader *ParseShader( int *fileDepth , int *p_c , char *ID ,
		     ShaderType shaderType );

/*!
 * \brief parsing a command pattern
 * \param currentCommand			to be added
 * \param aliasPattern						to be added
 * \param isOSC									to be added
 */
/* void PatternProcessing( pg_Command *currentCommand , char *aliasPattern , */
/* 			bool isOSC ); */

/*!
 * \brief parsing a node
 * \param p_ptFic								to be added
 * \param p_c										to be added
 * \param isConfigurationFile			to be added
 * \param XMLtag								to be added
 */
pg_Environment* ParseNode( int *fileDepth , int *p_c ,
		      bool isConfigurationFile ,
		      pg_XMLTag *XMLtag );

/*!
 * \brief parsing a node (actual node, link or repeat)
 * \param fileScene							to be added
 * \param p_c										to be added
 * \param currentNode						to be added
 * \param nbParsedNodes			to be added
 * \param XMLtag								to be added
 */
void ParseNodeSet( int *fileDepth ,  int *p_c ,
		   pg_Environment **currentNode , int *nbParsedNodes ,
		   pg_XMLTag *XMLtag );

/*!
 * \brief parsing a node component
 * \param p_ptFic								to be added
 * \param p_c										to be added
 * \param XMLtag								to be added
 * \param isConfigurationFile			to be added
 * \param returnNode						to be added
 */
pg_Environment* ParseComponent( 
         int *fileDepth , int *p_c , pg_XMLTag *XMLtag ,
	 bool isConfigurationFile , pg_Environment *returnNode );

#endif
