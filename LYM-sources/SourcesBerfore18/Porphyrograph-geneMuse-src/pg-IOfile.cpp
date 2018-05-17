/*! \file pg-IOfile.cpp
 *  \brief the implementation of pg-IOfile.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-IOfile.cpp
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

#include "pg-all_include.h"

////////////////////////////////////////////////////////////
// dynamically parse time
////////////////////////////////////////////////////////////
float dynamicTimeParsing( char *timeString ) {
  int indAux = 1;
  int ch = *timeString;
  return (float)ReadExpression( NULL , &ch , true , timeString , 
				 &indAux , false , NULL );
}

////////////////////////////////////////////////////////////
// REPLACEMENT OF PARAMETERS OR EXPRESSIONS BY THEIR VALUES
////////////////////////////////////////////////////////////

bool containsExpression( char *initialString ) {
  char * substring;
  substring = strchr( initialString , '(' );
  // non backslashed '('
  if( substring
      && (substring == initialString
	  || *(substring - 1) != '\\' ) ) {
    return true;
  }
  return false;
}

char *StrCpyAndSubst( char **targetString , char *initialString ) {
  return StrCpyAndSubst( targetString , NULL , 
			 initialString , -1 , 5 );
}
char *StrCpyAndSubst( char **targetString , char *initialString , 
		      int precision ) {
  return StrCpyAndSubst( targetString , NULL , 
			 initialString , -1 , precision );
}

char *StrCpyAndSubst( char *targetString , char *initialString , 
		      int lengthTargetString ) {
  return StrCpyAndSubst( NULL , targetString , initialString , 
			 lengthTargetString , 5 );
}
char *StrCpyAndSubst( char *targetString , char *initialString , 
		      int lengthTargetString , int precision ) {
  return StrCpyAndSubst( NULL , targetString , initialString , 
			 lengthTargetString , precision );
}

char *StrCpyAndSubst( char **variableTargetString , char *fixedTargetString , 
		      char *initialString , int lengthFixedTargetString ) {
  return StrCpyAndSubst( variableTargetString , fixedTargetString , 
			 initialString , lengthFixedTargetString , 5 );
}
char *StrCpyAndSubst( char **variableTargetString , char *fixedTargetString , 
		      char *initialString , int lengthFixedTargetString , 
		      int precision ) {
  // current location in the initial string
  char * currentPointerInit = NULL;
  // current location in the returned string
  char * currentPointerTarget = NULL;

  // substring that contains an expression
  char * substringInit = NULL;

  // stores the trailing part of a string that 
  // follows an evaluated expression
  char tailString[XMLAttrValLength];

  // stores the string that contains a balanced expression
  char expressionString[XMLAttrValLength];

  // string in which to store the string value of an embedded expression
  // if this expression has a string value (e.g. a node state)
  char outputString[XMLAttrValLength];

  // string in which to store the target string befre actual copy
  char temporaryTargetString[XMLAttrValLength];

  // expressions are not evaluated if the environment has
  // not been parsed
  if( !PG_EnvironmentNode ) {
    return initialString;
  }

  // printf( "string before [%s]\n" , initialString );

  // a copy of the original string is made for future
  // substitutions
  int currentLegnthTargetString = strlen( initialString );
  strcpy( temporaryTargetString , initialString );

  // current pointer in the initialString
  currentPointerInit = initialString;
  currentPointerTarget = temporaryTargetString;

  // stops on the first opening parenthesis
  while( currentPointerInit 
	 && (substringInit = strchr( currentPointerInit , '(' ) ) ) {

    // string in which is stored the output value
    // if it is a string
    *outputString = 0;

    // backslashed '('
    if( substringInit 
	&& substringInit != initialString // not the first char
	&& *(substringInit - 1) == '\\'   // backslashed 
	) {
      // jumps over the '\\' & '(' characters in the source string
      currentPointerInit = substringInit + 1;
      // deletes the '\\' character
      // jumps over the '(' character in the target string

      if( strchr( currentPointerTarget , '(' ) ) {
	currentPointerTarget = strchr( currentPointerTarget , '(' );
	// shifts the target string 1 char left to ignore \\ char
	char *auxPointerTarget = currentPointerTarget;
	while( *(auxPointerTarget) ) {
	  *(auxPointerTarget - 1) = *(auxPointerTarget);
	  auxPointerTarget++;
	}
	// final null char
	*(auxPointerTarget - 1) = *(auxPointerTarget);
      }
    }

    // expression to evaluate
    else {
      int car;
      int indChar = 1;
      car = *substringInit;

      // updates the current pointer in the working string
      if( strchr( currentPointerTarget , '(' ) ) {
	currentPointerTarget = strchr( currentPointerTarget , '(' ); 
      }

      // stores the expression in a temporary string
      *expressionString = 0;
      if( !ReadBalancedStringWithFrontiers( NULL , &car , 
					    expressionString ,
					    XMLAttrValLength , '(' , ')' , 
					    false , true , 
					    substringInit,&indChar ) ) {
	sprintf( ErrorStr , "Error: unbalanced expression!" ); 
	ReportError( ErrorStr ); 
	return initialString;
      }
      // printf( "expression [%s] %d\n" , expressionString , indChar );
	
      // returns the numerical or string value of the expression
      //printf( "parsed string [%s]\n" , expressionString );
      int indch = 1;
      int carch = '(';
      *outputString = 0;
      float val = ReadExpression( NULL , &carch , false , 
				  expressionString , &indch , true ,
				  outputString );
      //printf( "float val [%.2f]\n" , val );
      //printf( "string val [%s]\n" , outputString );
	
      // localizes the beginning of the substringInit tail:
      // the remaining part of the initial string that
      // follows the processed expression
      int ind = _Min(indChar - 1 , (int)strlen(substringInit));
      if( car == EOF ) {
	ind = strlen(substringInit);
      }
      strcpy( tailString , substringInit + ind );
      // printf( "tail [%s]\n" , tailString );
	
      // places the current pointer on the
      // first character that follows the processed string
      if( ind < (int)strlen(substringInit) ) {
	currentPointerInit = substringInit + ind;
      }
      else {
	currentPointerInit = NULL;
      }
      //printf( "tailString [%s]\n" , tailString );
	
      // appends the interperted expression to the working string
      if( *outputString ) {
	// outputString is appended
      }
      else if( fabs( val - (int)(val) ) < 0.00001 ) {
	sprintf( outputString , "%d" , (int)val );
      }
      else {
	switch( precision ) {
	case 0 :
	  sprintf( outputString , "%.0f" , val );
	  break;
	case 1 :
	  sprintf( outputString , "%.1f" , val );
	  break;
	case 2 :
	  sprintf( outputString , "%.2f" , val );
	  break;
	case 3 :
	  sprintf( outputString , "%.3f" , val );
	  break;
	case 4 :
	  sprintf( outputString , "%.4f" , val );
	  break;
	default :
	  sprintf( outputString , "%.5f" , val );
	  break;
	}
      }
	
      if( strlen( outputString ) + currentLegnthTargetString 
	  > XMLAttrValLength ) {
	sprintf( ErrorStr , "Error: insufficient parameter XMLAttValLength %d!" , XMLAttrValLength ); ReportError( ErrorStr ); 
	outputString[ XMLAttrValLength - currentLegnthTargetString - 1 ] = '\0';
      }
      strcpy( currentPointerTarget , outputString );
      currentLegnthTargetString += strlen( outputString );
      currentPointerTarget += strlen( outputString );
	
      // appends the remaining string to the working string
      if( strlen( tailString ) + currentLegnthTargetString > XMLAttrValLength ) {
	sprintf( ErrorStr , "Error: insufficient parameter XMLAttValLength %d!" , XMLAttrValLength ); ReportError( ErrorStr ); 
	tailString[ XMLAttrValLength - currentLegnthTargetString - 1 ] = '\0';
      }
      strcat( currentPointerTarget , tailString );
      // currentLegnthTargetString += strlen( tailString );
      // printf( "string after [%s]\n             [%s]\n             [%s]\n             [%s]\n" , 
      // 	      targetString , initialString ,
      // 	      currentPointerTarget , currentPointerInit );
    }
  }
  if( fixedTargetString ) {
    if( (int)strlen( temporaryTargetString ) > lengthFixedTargetString ) {
      sprintf( ErrorStr , "Error: insufficient parameter XMLAttrValLength %d!" , XMLAttrValLength ); ReportError( ErrorStr ); 
    }
    temporaryTargetString[ lengthFixedTargetString - 1 ] = '\0';
    strcpy( fixedTargetString , temporaryTargetString );
  }
  else {
    StrCpy( variableTargetString , temporaryTargetString );
  }
  return initialString;
}

void ExpectClosingTagXML( int *fileDepth , int *p_c ,  
			  const char expectedTag[XMLAttrChLength] , 
			  char *chaine , int *ind ) {
  pg_XMLTag *XMLtag = new pg_XMLTag();

  // expecting point closing tag
  XMLtag->ParseTagXML( fileDepth , p_c , true , chaine , ind );
  
  if( strcmp( XMLtag->tag , expectedTag ) != 0 || (XMLtag->tagType != ClosingTag) ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: closing tag [%s] expected!" , expectedTag ); ReportError( ErrorStr ); throw 8;
  }
  delete XMLtag;
  XMLtag = NULL;
}

pg_Environment* ParseComponent( int *fileDepth , int *curChar ,  
			      pg_XMLTag *XMLtag ,
			      bool isConfigurationFile , 
			      pg_Environment *returnNode ) {
  
  if( isConfigurationFile 
      && strcmp( XMLtag->tag , "environment" ) != 0 ) {
    sprintf( ErrorStr , "Error: only environment node allowed in configuration file!" ); ReportError( ErrorStr ); throw 63;
  }

  if( strcmp( XMLtag->tag , "environment" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    returnNode->parse( fileDepth , curChar );

    //theEnvironment->print( stdout , true , 1 );

    // current node
    return returnNode;

  }

  /////////////////////////////////
  // any other unexpected component
  else {
    sprintf( ErrorStr , "Error: unknown node component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 42;
  }
}


pg_Environment* ParseNode( int *fileDepth , int *curChar ,
			 bool isConfigurationFile ,
			 pg_XMLTag *XMLtag ) {

				 
  pg_Environment     *returnNode;
  returnNode = NULL;
  returnNode = new pg_Environment();
  returnNode->setParameters( XMLtag , false , assign , assignI );

  // printf( "Parse node\n" );

  // new tag XML : transformation component or closing tag
  XMLtag->deallocateXMLTag();
  XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
  
  // printf( "Tag %s\n" , XMLtag->tag );
  
  // configuration file: only environment
  if( strcmp( XMLtag->tag , "node" ) == 0 ) { 
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
  }
  
  if( strcmp( XMLtag->tag , "environment" ) == 0 ) { 
    // reading the characteristics of a node
    ParseComponent( fileDepth , curChar , XMLtag , 
		    isConfigurationFile , returnNode );
    
    // node closing tag
    // ExpectClosingTagXML( fileDepth , curChar ,  "environment" , NULL , NULL );
    
    // node closing tag
    ExpectClosingTagXML( fileDepth , curChar ,  "node" , NULL , NULL );
  }
  
  else {
    sprintf( ErrorStr , "Error: unknown component [%s] !" , XMLtag->tag ); ReportError( ErrorStr ); throw 66;
  }
  return returnNode;
}

void ParseNodeSet( int *fileDepth ,  int *p_c ,
		   pg_Environment **currentNode , int *nbParsedNodes ,
		   pg_XMLTag *XMLtag ) {
  // single node
  if( strcmp( XMLtag->tag , "node" ) == 0 ) {
    //printf( "parse node (%s)\n" , IDnode );
    *currentNode = ParseNode( fileDepth , p_c , false , XMLtag );

    //fprintf( stdout , "Node %s, %s\n" , NodeTypeString[(*currentNode)->type] , (*currentNode)->id);

    *nbParsedNodes = 1;
    //(*currentNode)->print( stdout , 1 , 1 );
  }

  else {
    sprintf( ErrorStr , "Error: unknown node tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
  }
}


