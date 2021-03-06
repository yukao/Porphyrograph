/*! \file dm-IOfile.cpp
 *  \brief the implementation of dm-IOfile.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-IOfile.cpp
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
// parsing a repeat node
////////////////////////////////////////////////////////////
// returns the float value of a parameter from its string
char *ParameterValue( char * chaineLue  ) {
  //printf( "chaineLue [%s] [%d]\n" , chaineLue , DM_EnvironmentNode->NbParameters );
  //printf( "parameter 0 [%s/%s/%d]\n" , &(DM_EnvironmentNode->ParameterStrings[0 * XMLAttrValLength]), &DM_EnvironmentNode->ParameterValues[0], 0);
  for( int ind = 0 ; ind < DM_EnvironmentNode->NbParameters ; ind++ ) {
    //printf( "param %d %s [%s]\n" , ind , &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] , &(DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength]) );
    if( strcmp( chaineLue , 
		&(DM_EnvironmentNode->ParameterStrings[ind
					   * XMLAttrValLength]) ) == 0 ) {
      //printf( "param value %s\n" , &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] );
      return &(DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength]);
    }
  }

  sprintf( ErrorStr , "Error: unknown parameter [%s]!" , chaineLue ); ReportError( ErrorStr ); throw 93;
}


////////////////////////////////////////////////////////////
// parsing a test node
////////////////////////////////////////////////////////////
dm_SceneNode* ParseTest( int *fileDepth , int *curChar , 
			 dm_XMLTag *XMLtag, 
			 int *nbReturnedNodes ) {
  dm_SceneNode     *returnNode, *auxNode, *currentNode;

  returnNode = NULL;
  *nbReturnedNodes = 0;
  
  // printf( "Parse node\n" );
  char *parameterId = XMLtag->getAttributeStringValue( "id" );
  char paramString[XMLAttrValLength];
  strcpy ( paramString , "{#" );
  strcat ( paramString , parameterId );
  strcat ( paramString , "}" );
  char * comparisonValue = XMLtag->getAttributeStringValue( "value" );
  char *testMode = XMLtag->getAttributeStringValue( "operator" );
  if( strcmp( testMode , "ifequal" ) != 0
      && strcmp( testMode , "ifnotequal" ) != 0
      && strcmp( testMode , "ifinf" ) != 0
      && strcmp( testMode , "ifsup" ) != 0 ) {
    sprintf( ErrorStr , "Error: unknown test operator [%s] (known operators are ifequal, ifnotequal, ifinf, ifsup)!" , paramString ); ReportError( ErrorStr ); throw 162;
  }
  if( DM_EnvironmentNode->NbParameters <= 0 ) {
    sprintf( ErrorStr , "Error: test operator [%s] must be embedded in repeat node!" , paramString ); ReportError( ErrorStr ); throw 164;
  }

  char curParamValue[XMLAttrValLength];
  *curParamValue = 0;	     
  int paramFound = false;
  for( int ind = 0 ; ind < DM_EnvironmentNode->NbParameters ; ind++ ) {
    //printf( "param %d %s [%s]\n" , ind , &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] ,
    //&(DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength]) );
    if( strcmp( paramString ,
		&(DM_EnvironmentNode->ParameterStrings[ind 
						* XMLAttrValLength]) ) == 0 ) {
      strcpy( curParamValue ,
	      &(DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength]) );
      paramFound = true;
    }
  }
  if( !paramFound ) {
    sprintf( ErrorStr , "Error: unknown parameter [%s]!" , paramString ); ReportError( ErrorStr ); throw 161;
  }

  // boolean value true
  int booleanValue =
    (strcmp( testMode , "ifequal" ) == 0 && strcmp( curParamValue , comparisonValue) == 0 )
    || (strcmp( testMode , "ifnotequal" ) == 0 && strcmp( curParamValue , comparisonValue) != 0 )
    || (strcmp( testMode , "ifinf" ) == 0 && atof(curParamValue) <= atof(comparisonValue))
    || (strcmp( testMode , "ifsup" ) == 0 && atof(curParamValue) >= atof(comparisonValue));

  //printf( "boolean value %d param value/test value [%d,%s] (%s)\n" , 
  //  booleanValue , curParamValue , comparisonValue , testMode );

  // new tag XML : transformation component or closing tag
  XMLtag->deallocateXMLTag();
  XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
    
  // daughter node
  if( (strcmp( XMLtag->tag , "node" ) == 0 
       && (XMLtag->tagType == OpeningTag)) 
      || (strcmp( XMLtag->tag , "use" ) == 0 
	  && (XMLtag->tagType == EmptyTag)) ) {
    // printf( "parsing node ID [%s]\n" , ID );
    // node tag
    if( strcmp( XMLtag->tag , "node" ) == 0 ) {
      currentNode = ParseNode( fileDepth , curChar , false ,
			       XMLtag );
    }
    // use tag
    else  {
      bool isdisplayed = true;
      XMLtag->BooleanFieldValue( "displayed" , &isdisplayed );
      // references are solved in a second pass
      currentNode = new dm_SceneNode(ANodeLink);
      currentNode->displayed = isdisplayed;
      StrCpy( &(currentNode->id) , 
	      XMLtag->getAttributeStringValue( "xlink:href" ) );
    }
      
    // node chaining
    (*nbReturnedNodes)++;
    if( !returnNode ) {
      returnNode = currentNode;
    }
    else {
      auxNode = returnNode;
      while( auxNode->sister ) {
	auxNode = auxNode->sister;
      }
      auxNode->sister = currentNode;
    }
  }
  else {
    sprintf( ErrorStr , "Error: unknown component [%s] in test ID [%s] only sub-nodes are allowed!" , XMLtag->tag , paramString ); ReportError( ErrorStr ); throw 164;
  }

  // node closing tag
  ExpectClosingTagXML( fileDepth , curChar ,  "test" , NULL , NULL );

  // default return node is identical transformation
  if( booleanValue ) {
    return returnNode;
  }
  else {
    *nbReturnedNodes = 0;
    return NULL;
  }
}


////////////////////////////////////////////////////////////
// parsing a set node
////////////////////////////////////////////////////////////
dm_SceneNode* ParseSet( int *fileDepth , int *curChar , 
			dm_XMLTag *XMLtag , 
			int *nbReturnedNodes ) {
  dm_SceneNode     *returnNode, *auxNode, *currentNode;

  returnNode = NULL;
  *nbReturnedNodes = 0;
  
  // printf( "Parse node\n" );
  char *parameterId = XMLtag->getAttributeStringValue( "id" );
  char paramString[XMLAttrValLength];
  strcpy ( paramString , "{#" );
  strcat ( paramString , parameterId );
  strcat ( paramString , "}" );
  char setValue[XMLAttrValLength];

  if( XMLtag->hasAttribute( "string_value" ) ) {
    strcpy( setValue , XMLtag->getAttributeStringValue( "string_value" ) );
  }
  else if( XMLtag->hasAttribute( "float_value" ) ) {
    sprintf( setValue , "%.5f" , XMLtag->getAttributeDoubleValue( "float_value" , 
								  false ) );
  }
  else if( XMLtag->hasAttribute( "int_value" ) ) {
    sprintf( setValue , "%d" , XMLtag->getAttributeIntValue( "int_value" ,
							  false ) );
  }
  // deprecated
  else if( XMLtag->hasAttribute( "value" ) ) {
    double val = XMLtag->getAttributeDoubleValue( "value" ,
					  false );
    if( fabs( val - (int)(val) ) < 0.00001 ) {
      sprintf( setValue , "%d" , (int)val );
    }
    else {
      sprintf( setValue , "%.5f" , val );
    }
  }

  //printf( "set value [%s]\n" , setValue ); 
   if( DM_EnvironmentNode->NbParameters >= DM_EnvironmentNode->Nb_max_embedded_parameters ) {
    sprintf( ErrorStr , "Error: unable to load parameter [%s] exceeds max_embedded_parameters!" , paramString ); ReportError( ErrorStr ); throw 90;
  }

  // storing parameter strings
  int currentParameterIndex = DM_EnvironmentNode->NbParameters;
  DM_EnvironmentNode->NbParameters++;
  strcpy ( &(DM_EnvironmentNode->ParameterStrings[currentParameterIndex 
					* XMLAttrValLength]) , paramString );

  // comparing with previous values
  //printf( "Nb parameter [%d]\n" , DM_EnvironmentNode->NbParameters );
  for( int ind = 0 ; ind < DM_EnvironmentNode->NbParameters - 1 ; ind++ ) { 
    //printf( "parameter [%s]\n" , &(DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength]) );
    if( strcmp( &(DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength]) , 
		&(DM_EnvironmentNode->ParameterStrings[currentParameterIndex 
						* XMLAttrValLength]) ) == 0 ) {
      sprintf( ErrorStr , "Error: overridden parameter [%s]!" , paramString ); ReportError( ErrorStr ); throw 189;
    }
  }      
  //printf( "parameter [%s]\n" , &(DM_EnvironmentNode->ParameterStrings[currentParameterIndex * XMLAttrValLength]) );

  // storing value
  strcpy( &(DM_EnvironmentNode->ParameterValues[currentParameterIndex 
					 * XMLAttrValLength]) , setValue );

  // empty tag: remanent parameter addition
  // no sub-node is genenrated
  if( XMLtag->tagType == EmptyTag ) {
    return NULL;
  }

  // new tag XML : transformation component or closing tag
  XMLtag->deallocateXMLTag();
  XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
  
  // embedded set
  if( strcmp( XMLtag->tag , "set" ) == 0 ) {
    int        nbReturnedNodesAux = 0;
    TagType    setTagType = (TagType)XMLtag->tagType;
    
    currentNode = ParseSet( fileDepth , curChar , XMLtag , 
			    &nbReturnedNodesAux );
    
    // opening tag: a subnode is generated
    if( setTagType == OpeningTag) {
      // node chaining
      if( !returnNode ) {
	returnNode = currentNode;
      }
      else {
	auxNode = returnNode;
	while( auxNode->sister ) {
	  auxNode = auxNode->sister;
	}
	auxNode->sister = currentNode;
      }
      *nbReturnedNodes += nbReturnedNodesAux;
    }
    // empty tag: a remanent parameter is added
    // no subnode is generated
  }
  
  // daughter node
  else if( (strcmp( XMLtag->tag , "node" ) == 0 
	    && (XMLtag->tagType == OpeningTag)) 
	   || (strcmp( XMLtag->tag , "use" ) == 0 
	       && (XMLtag->tagType == EmptyTag)) ) {
    // printf( "parsing node ID [%s]\n" , ID );
    // node tag
    char *ID = NULL;
    if( strcmp( XMLtag->tag , "node" ) == 0 ) {
      currentNode = ParseNode( fileDepth , curChar , false ,
			       XMLtag );
    }
    // use tag
    else  {
      ID = XMLtag->getAttributeStringValue( "xlink:href" );
      bool isdisplayed = true;
      XMLtag->BooleanFieldValue( "displayed" ,
			  &isdisplayed );
      // references are solved in a second pass
      currentNode = new dm_SceneNode(ANodeLink);
      currentNode->displayed = isdisplayed;
    }
    if( ID ) {
      StrCpy( &(currentNode->id) , ID );
    }
      
    // node chaining
    (*nbReturnedNodes)++;
    if( !returnNode ) {
      returnNode = currentNode;
    }
    else {
      auxNode = returnNode;
      while( auxNode->sister ) {
	auxNode = auxNode->sister;
      }
      auxNode->sister = currentNode;
    }
  }
  else {
    sprintf( ErrorStr , "Error: unknown component [%s] in set ID [%s] only sub-nodes are allowed!" , XMLtag->tag , paramString ); ReportError( ErrorStr ); throw 194;
  }

  // node closing tag
  ExpectClosingTagXML( fileDepth , curChar ,  "set" , NULL , NULL );

  // default return node is identical transformation
  DM_EnvironmentNode->NbParameters--;
  return returnNode;
}

////////////////////////////////////////////////////////////
// REPLACEMENT OF PARAMETERS OR EXPRESSIONS BY THEIR VALUES
////////////////////////////////////////////////////////////

char *substituteParameters( char *initialString ) {
  char tempVal[XMLAttrValLength];
  if( !DM_EnvironmentNode 
      || !( strstr( initialString , "{#" ) ) ) {
    return initialString;
  }

  // printf( "string before [%s]\n" , initialString );
  for( int ind = 0 ; ind < DM_EnvironmentNode->NbParameters ; ind++ ) {
    char * substring; 
    // printf( "param %d [%s] [%s]\n" , ind , &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] , &DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength] );
    while( (substring = strstr( initialString , 
				&(DM_EnvironmentNode->ParameterStrings[ind 
						  * XMLAttrValLength]) ) ) ) {
      // copies the string after the parameter
      strcpy( tempVal , 
	      substring 
	      + strlen( &(DM_EnvironmentNode->ParameterStrings[ind 
				       		* XMLAttrValLength]) ) );
      // replaces the parameter value and the string after the parameter
      // in the initial string
      sprintf( substring , "%s%s" , 
	       &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] , 
	       tempVal );
    }
  }
  // printf( "string after  [%s]\n" , initialString );
  return initialString;
}

char *SubstituteParameters( char **initialString ) {
  char tempVal[XMLAttrValLength];
  char tempVal2[XMLAttrValLength];

  if( !DM_EnvironmentNode 
      || !( strstr( *initialString , "{#" ) ) ) {
    return *initialString;
  }

  // stores the string in a larger temporary string for param substitution
  strcpy( tempVal2 , *initialString );

  // printf( "string before [%s]\n" , initialString );
  for( int ind = 0 ; ind < DM_EnvironmentNode->NbParameters ; ind++ ) {
    char * substring; 
    // printf( "param %d [%s] [%s]\n" , ind , &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] , &DM_EnvironmentNode->ParameterStrings[ind * XMLAttrValLength] );
    while( (substring = strstr( tempVal2 , 
				&(DM_EnvironmentNode->ParameterStrings[ind 
						  * XMLAttrValLength]) ) ) ) {
      // copies the string after the parameter
      strcpy( tempVal , 
	      substring 
	      + strlen( &(DM_EnvironmentNode->ParameterStrings[ind 
				       		* XMLAttrValLength]) ) );
      // replaces the parameter value and the string after the parameter
      // in tempVal2
      sprintf( substring , "%s%s" , 
	       &DM_EnvironmentNode->ParameterValues[ind * XMLAttrValLength] , 
	       tempVal );
    }
  }

  // printf( "string after  [%s]\n" , initialString );
  if( strlen( tempVal2 ) > strlen( *initialString ) ) {
    StrCpy( initialString , tempVal2 );
    // printf( "targetStringValue %s\n" , *initialString );
  }
  else {
    strcpy( *initialString , tempVal2 );
  }
  return *initialString;
}

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
  if( !DM_EnvironmentNode ) {
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
  dm_XMLTag *XMLtag = new dm_XMLTag();

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

dm_SceneNode* ParseComponent( int *fileDepth , int *curChar ,  
			      dm_XMLTag *XMLtag ,
			      bool isConfigurationFile , 
			      dm_SceneNode *returnNode ) {
  
  if( isConfigurationFile 
      && strcmp( XMLtag->tag , "environment" ) != 0 ) {
    sprintf( ErrorStr , "Error: only environment node allowed in configuration file!" ); ReportError( ErrorStr ); throw 63;
  }

  /////////////////////////////////
  // transformation
  if( strcmp( XMLtag->tag , "transformation" ) == 0 ) {
    dm_Transformation   *theTransformation;

    
    theTransformation = new dm_Transformation();
    theTransformation->setParameters( XMLtag , false , assign , assignI );

    // old fashioned transformations: components are given separately
    if( (XMLtag->tagType == OpeningTag) ) {
      theTransformation->parse( fileDepth , curChar , returnNode );
    }
    // current syntax: components are given in the XML tag
    else {
      // nothing to parse
    }

    //theTransformation->print( stdout , 1, 1 );

    // current node
    returnNode->SetObject( ATransformation , (dm_SceneObject *)theTransformation );
    return returnNode;
  }

  /////////////////////////////////
  // mesh component
  else if( strcmp( XMLtag->tag , "mesh" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    dm_Mesh   *theMesh;

    theMesh = new dm_Mesh();
    theMesh->setParameters( XMLtag , false , assign , assignI );
    theMesh->parse( fileDepth , curChar );
    // theMesh->print( stdout , 1 , 1 );

    // current node
    returnNode->SetObject( AMesh , (dm_SceneObject *)theMesh );
    return returnNode;
  }

  /////////////////////////////////
  // points component
  else if( strcmp( XMLtag->tag , "points" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    dm_Points   *thePoints;

    thePoints = new dm_Points();
    thePoints->setParameters( XMLtag , false , assign , assignI );
    thePoints->parse( fileDepth , curChar );
    // thePoints->print( stdout , 1 , 1 );

    // current node
    returnNode->SetObject( APoints , (dm_SceneObject *)thePoints );
    return returnNode;
  }

  /////////////////////////////////
  // quad component
  else if( strcmp( XMLtag->tag , "quad" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    dm_Quad   *theQuad;

    theQuad = new dm_Quad();
    theQuad->setParameters( XMLtag , false , assign , assignI );
    theQuad->parse( fileDepth , curChar );
    // theQuad->print( stdout , 1 , 1 );

    // current node
    returnNode->SetObject( AQuad , (dm_SceneObject *)theQuad );
    return returnNode;
  }
  // point component
  else if( strcmp( XMLtag->tag , "point" ) == 0 ) {
    dm_Point   *thePoint;

    thePoint = new dm_Point();
    thePoint->setParameters( XMLtag , false , assign , assignI );
    thePoint->parse( fileDepth , curChar , (XMLtag->tagType == OpeningTag) );
    // thePoint->print( stdout , 1 , 1 );

    // current node
    returnNode->SetObject( APoint , (dm_SceneObject *)thePoint );
    return returnNode;
  }
  // line component
  else if( strcmp( XMLtag->tag , "line" ) == 0 ) {
    dm_Line   *theLine;

    theLine = new dm_Line();
    theLine->setParameters( XMLtag , false , assign , assignI );
    theLine->parse( fileDepth , curChar , (XMLtag->tagType == OpeningTag) );
    // theLine->print( stdout , 1 , 1 );

    // current node
    returnNode->SetObject( ALine , (dm_SceneObject *)theLine );
    return returnNode;
  }
  /////////////////////////////////
  // strip component
  else if( strcmp( XMLtag->tag , "strip" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    dm_Strip   *theStrip;
      
    theStrip = new dm_Strip();
    theStrip->setParameters( XMLtag , false , assign , assignI);
    theStrip->parse( fileDepth , curChar );
    // theStrip->print( stdout );

    // current node
    returnNode->SetObject( AStrip , (dm_SceneObject *)theStrip );
    return returnNode;
  }

  /////////////////////////////////
  // viewpoint
  else if( (strcmp( XMLtag->tag , "viewpoint" ) == 0 
	    || strcmp( XMLtag->tag , "camera" ) == 0) 
	   && (XMLtag->tagType == EmptyTag) ) {

    if( strcmp( XMLtag->tag , "camera" ) == 0 ) {
      fprintf( stderr , "Deprecated: \"camera\" node is deprecated use \"viewpoint\"!\n" );
    }

    dm_Viewpoint   *theViewpoint;

    theViewpoint = new dm_Viewpoint();
    theViewpoint->setParameters( XMLtag , false , assign , assignI );

    // theViewpoint->print( stdout );

    // current node
    returnNode->SetObject( AViewpoint , (dm_SceneObject *)theViewpoint );
    return returnNode;
  }


  /////////////////////////////////
  // texture storage (frame buffer)
  else if( (strcmp( XMLtag->tag , "texture_storage" ) == 0) 
	   && (XMLtag->tagType == EmptyTag) ) {

    dm_TextureStorage   *theTextureStorage;

    theTextureStorage = new dm_TextureStorage();
    theTextureStorage->setParameters( XMLtag , false , 
				      assign , assignI );

    // theTextureStorage->print( stdout );

    // current node
    returnNode->SetObject( ATextureStorage , 
			   (dm_SceneObject *)theTextureStorage );
    return returnNode;
  }

  /////////////////////////////////
  // FBO storage (frame buffer object)
  else if( (strcmp( XMLtag->tag , "FBO_storage" ) == 0) 
	   && (XMLtag->tagType == OpeningTag) ) {

    dm_FBOStorage   *theFBOStorage;

    theFBOStorage = new dm_FBOStorage();
    theFBOStorage->setParameters( XMLtag , false , 
				      assign , assignI );
    theFBOStorage->parse( fileDepth , curChar );

    // theFBOStorage->print( stdout );

    // current node
    returnNode->SetObject( AFBOStorage , 
			   (dm_SceneObject *)theFBOStorage );
    return returnNode;
  }

  /////////////////////////////////
  // FB display (frame buffer)
  else if( (strcmp( XMLtag->tag , "FB_display" ) == 0) 
	   && (XMLtag->tagType == EmptyTag) ) {

    dm_FBDisplay   *theFBDisplay;

    theFBDisplay = new dm_FBDisplay();
    theFBDisplay->setParameters( XMLtag , false , 
				      assign , assignI );

    // theFBDisplay->print( stdout );

    // current node
    returnNode->SetObject( AFBDisplay , 
			   (dm_SceneObject *)theFBDisplay );
    return returnNode;
  }

  /////////////////////////////////
  // pixel reader (inside texture)
  else if( (strcmp( XMLtag->tag , "pixel_reader" ) == 0) 
	   && (XMLtag->tagType == EmptyTag) ) {

    dm_PixelReader   *thePixelReader;

    thePixelReader = new dm_PixelReader();
    thePixelReader->setParameters( XMLtag , false , 
				      assign , assignI );

    // thePixelReader->print( stdout );

    // current node
    returnNode->SetObject( APixelReader , 
			   (dm_SceneObject *)thePixelReader );
    return returnNode;
  }

  /////////////////////////////////
  // environment
  else if( strcmp( XMLtag->tag , "environment" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    dm_Environment   *theEnvironment;

    theEnvironment = new dm_Environment();
    theEnvironment->setParameters( XMLtag , false , assign , assignI );
    theEnvironment->parse( fileDepth , curChar );

    //theEnvironment->print( stdout , true , 1 );

    // current node
    returnNode->SetObject( AEnvironment , (dm_SceneObject *)theEnvironment );
    return returnNode;

  }

  /////////////////////////////////
  // users
  else if( strcmp( XMLtag->tag , "user" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    // transformer en analyse d'un objet user list

    dm_User   *theUser;

    theUser = new dm_User();
    theUser->setParameters( XMLtag , false , assign , assignI );
    theUser->parse( fileDepth , curChar );

    // theUser->print( stdout , true , 1 );

    // current node
    returnNode->SetObject( AUser , (dm_SceneObject *)theUser );
 
   return returnNode;
  }

  /////////////////////////////////
  // any other unexpected component
  else {
    sprintf( ErrorStr , "Error: unknown node component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 42;
  }
  return NULL;
}

void PatternProcessing( dm_Command *currentCommand , char *aliasPattern ,
			bool isOSC ) {
  int indAliasPattern = 0;
  int patternChar = aliasPattern[indAliasPattern];
  char paramTypesId[ XMLAttrValLength ];
  char *tempAttr = NULL;

  ////////////////////////
  // pattern processing
  StrCpy( &(currentCommand->aliasPattern) , aliasPattern );
  // printf( "CommandAliasOSCTag (%s)\n" , aliasPattern );
	  
  // OSC address
  if( isOSC ) {
    if( aliasPattern[indAliasPattern] != '/' ) {
      sprintf( ErrorStr , "Error: OSC address expected in command alias pattern!" ); ReportError( ErrorStr ); throw 68;
    }
    // reads the tag in a temporary string and
    // allocates tag and copies 
    patternChar = '/';
    indAliasPattern = 1;
    ReadIDWithCarSpec( NULL , &patternChar , &tempAttr , 
		       "/_" , true , aliasPattern , 
		       &indAliasPattern );
    // leading / is ignored
    StrCpy( &(currentCommand->CommandAliasOSCTag) , 
	    tempAttr + 1 );
  }
  else {
    patternChar = aliasPattern[0];
    indAliasPattern = 1;
    ReadIDWithCarSpec( NULL , &patternChar , &tempAttr , 
		       "/_-" , true , aliasPattern , 
		       &indAliasPattern );
    StrCpy( &(currentCommand->CommandAliasOSCTag) , 
	    tempAttr );
  }
  // deallocates possibly preallocated memory
  if( tempAttr ) {
    delete [] tempAttr;
    tempAttr = NULL;
  }

  
  // printf( "Command (%s) " , currentCommand->CommandAliasOSCTag );

  while(  patternChar != (int)EOF && patternChar 
	  && currentCommand->commandNbOSCParams < XMLAttrValLength ) {
    paramTypesId[ (currentCommand->commandNbOSCParams)++ ] = patternChar;
    // printf( "[%c] " , patternChar );
    patternChar = aliasPattern[indAliasPattern++];
    SpaceCommentsInclude( NULL , &patternChar , aliasPattern , &indAliasPattern );
  }
  // printf( "\n" );
  if( patternChar != (int)EOF && patternChar ) {
    sprintf( ErrorStr , "Error: incorrect syntax in command alias pattern or excessive number of parameters!" ); ReportError( ErrorStr ); throw 68;
  }
	  
  // first letter of OSC data types correspond to
  // first letter of VC data types
  currentCommand->commandOSCParams
    = new dm_ValScalar[ currentCommand->commandNbOSCParams ];
  for( int ind = 0 ; ind < currentCommand->commandNbOSCParams ; ind++ ) {
    currentCommand->commandOSCParams[ ind ].clean();
    for( int indAux = 0 ; indAux < EmptyDataType ; indAux++ ) {
      if( paramTypesId[ ind ] == *(DataTypeStrings[indAux]) ) {
	currentCommand->commandOSCParams[ind].type = (DataType)indAux;
	break;
      }
    }
    if( currentCommand->commandOSCParams[ind].type == EmptyDataType ) {
      sprintf( ErrorStr , "Error: unknown OSC data type [%c]!" , paramTypesId[ ind ] ); ReportError( ErrorStr ); throw 249;
    }
  }
}

dm_SceneNode* ParseNode( int *fileDepth , int *curChar ,
			 bool isConfigurationFile ,
			 dm_XMLTag *XMLtag ) {

				 
  dm_SceneNode     *daughterNode, *returnNode, *auxNode, *currentNode;
  daughterNode = NULL;
  returnNode = NULL;
  auxNode = NULL;
  currentNode = NULL;
  returnNode = new dm_SceneNode();
  returnNode->setParameters( XMLtag , false , assign , assignI );

  // printf( "Parse node\n" );

  while( true ) {
    if( *curChar == EOF ) {
      sprintf( ErrorStr , "Error: unbalanced node tag in node [%s]. Unexpected end of file!" , returnNode->id ); ReportError( ErrorStr ); throw 68;
    }
    
    // new tag XML : transformation component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
    
    // printf( "Tag %s\n" , tag );
    
    // configuration file: only environment
    if( isConfigurationFile 
	&& !(strcmp( XMLtag->tag , "node" ) == 0 
	     && (XMLtag->tagType == ClosingTag))
	&& strcmp( XMLtag->tag , "environment" ) != 0 ) {
      sprintf( ErrorStr , "Error: only environment allowed in configuration file!" ); ReportError( ErrorStr ); throw 63;
    }
    
    // node closing tag
    if( strcmp( XMLtag->tag , "node" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {
      // default return node is identical transformation
      if( !(returnNode->sceneObject) ) {
	// printf( "No return node\n" );
	dm_Transformation *theTransformation = new dm_Transformation();
	
	returnNode->SetObject( ATransformation , (dm_SceneObject *)theTransformation );
      }
	
      // rendering scripts can only be found on geometrical object
      // because they are applied during the fourth drawing phase
      // when postDisplay is called on geometrical objects
      if( returnNode->renderingCommand 
	  && !(returnNode->IsGeometricalObject()) ) {
	sprintf( ErrorStr , "Error: node ID [%s] is not a geometrical object, cannot have rendering script!" , returnNode->id ); ReportError( ErrorStr ); throw 61;
      }
	
      // daughter is either a transformation or an interpolated
      // transformation
      if( daughterNode 
	  && !( (returnNode->type == ATransformation) 
		|| (returnNode->type == AUser) ) ) {
	sprintf( ErrorStr , "Error: node ID [%s] is not a a user or a transformation, cannot have subnodes!" , returnNode->id ); ReportError( ErrorStr ); throw 61;
      }
	
      //         // interpolators cannot have script because they
      //         // are deleted during node interpolation
      //         if( returnNode->script && 
      // 	       returnNode->type == AInterpolator ) {
      // 	            sprintf( ErrorStr , "Error: node ID [%s] is an interpolator, cannot have scripts!" , returnNode->id ); ReportError( ErrorStr ); throw 136;
      //         }
	
      returnNode->daughter = daughterNode;
	
      //returnNode->print( stdout , 1 , 1 );
      return returnNode;
    }
      
    // component
    else if( strcmp( XMLtag->tag , "mesh" ) == 0
	     || strcmp( XMLtag->tag , "points" ) == 0
	     || strcmp( XMLtag->tag , "quad" ) == 0
	     || strcmp( XMLtag->tag , "point" ) == 0
	     || strcmp( XMLtag->tag , "line" ) == 0
	     || strcmp( XMLtag->tag , "text" ) == 0
	     || strcmp( XMLtag->tag , "strip" ) == 0
	     || strcmp( XMLtag->tag , "interpolator" ) == 0
	     || strcmp( XMLtag->tag , "environment" ) == 0
	     || strcmp( XMLtag->tag , "user" ) == 0
	     || (strcmp( XMLtag->tag , "viewpoint" ) == 0 || strcmp( XMLtag->tag , "camera" ) == 0)
	     || strcmp( XMLtag->tag , "texture_storage" ) == 0 
	     || strcmp( XMLtag->tag , "pixel_reader" ) == 0 
	     || strcmp( XMLtag->tag , "FBO_storage" ) == 0 
	     || strcmp( XMLtag->tag , "FB_display" ) == 0 
	     || strcmp( XMLtag->tag , "transformation" ) == 0 ) { 
      if( returnNode->sceneObject ) {
	sprintf( ErrorStr , "Error: multiple components for node ID [%s]!" , returnNode->id ); ReportError( ErrorStr ); throw 63;
      }

      // reading the characteristics of a node
      // printf( "Parse component %s\n" , tag );
      ParseComponent( fileDepth , curChar , XMLtag , 
		      isConfigurationFile , returnNode );
    }
      
    // daughter node
    else if( strcmp( XMLtag->tag , "node" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      //printf( "parse node (%s)\n" , returnNode->id );
      currentNode = ParseNode( fileDepth , curChar , false , XMLtag );
	
	
      // node chaining
      if( !daughterNode ) {
	daughterNode = currentNode;
      }
      else {
	auxNode = daughterNode;
	while( auxNode->sister ) {
	  auxNode = auxNode->sister;
	}
	auxNode->sister = currentNode;
      }
	
      //printf( "end of parse node (%s)\n" , returnNode->id );
    }
      
    // script
    else if( strcmp( XMLtag->tag , "script" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      if( returnNode->script ) {
	sprintf( ErrorStr , "Error: node ID [%s] can only have one script!" , returnNode->id ); ReportError( ErrorStr ); throw 128;
      }
	
      char * scriptID = XMLtag->getAttributeStringValue( "id" );
      {
	// reading a script node
	returnNode->script = ParseScript( fileDepth , curChar , returnNode , scriptID );
      }
      // returnNode->script->print( stdout , 1 , 1 );
    }
      
    // initialization script
    else if( strcmp( XMLtag->tag , "initializationScript" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      if( DM_CompositionNode->initializationCommand ) {
	sprintf( ErrorStr , "Error: a composition can only have one initialization script!" ); ReportError( ErrorStr ); throw 128;
      }
	
      //		DM_CompositionNode->initializationCommand = new dm_Command();
      //		DM_CompositionNode->initializationCommand->clean();
	
      // new tag XML : opening action element
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	
      if( ( strcmp( XMLtag->tag , "command" ) == 0 ) && (XMLtag->tagType == OpeningTag) ) {
	////////////////////////
	// parsing the associated action(s)
	DM_CompositionNode->initializationCommand = 
	  ParseCommand( fileDepth , curChar , returnNode ,  false , NULL , NULL );
      }
      else {
	sprintf( ErrorStr , "Error: incorrect tag [%s] in initialization script of node [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ) ; throw 298;	throw 63;
      }
	
      // expecting script closing tag
      ExpectClosingTagXML( fileDepth , curChar ,  "initializationScript", NULL , NULL  );
    }

    // termination script
    else if( strcmp( XMLtag->tag , "terminationScript" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      if( DM_CompositionNode->terminationCommand ) {
	sprintf( ErrorStr , "Error: a composition can only have one termination script!" ); ReportError( ErrorStr ); throw 128;
      }
      //		DM_CompositionNode->terminationCommand = new dm_Command();
      //		DM_CompositionNode->terminationCommand->clean();
	
      // new tag XML : opening action element
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	
      if( ( strcmp( XMLtag->tag , "command" ) == 0 ) 
	  && (XMLtag->tagType == OpeningTag) ) {
	////////////////////////
	// parsing the associated action(s)
	DM_CompositionNode->terminationCommand 
	  = ParseCommand( fileDepth , curChar , 
			  returnNode , false ,	NULL , NULL );
      }
      else {
	sprintf( ErrorStr , "Error: incorrect tag [%s] in termination script of node [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
	
      // expecting script closing tag
      ExpectClosingTagXML( fileDepth , curChar ,  "terminationScript", NULL , NULL  );
    }

    // alias script
    else if( strcmp( XMLtag->tag , "aliasScript" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      dm_Command    *commandList;
      dm_Command    *headOfCommandList;
      dm_Command    *auxCommand;
      commandList = NULL;
      headOfCommandList = NULL;
      auxCommand = NULL;

      if( DM_CompositionNode->aliasCommand ) {
	sprintf( ErrorStr , "Error: a composition can only have one alias script!" ); ReportError( ErrorStr ); throw 128;
      }
	
      while( true ) {
	// endless script
	// new tag XML : opening action element
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	  
	// node closing tag
	if( strcmp( XMLtag->tag , "aliasScript" ) == 0 
	    && (XMLtag->tagType == ClosingTag) ) {
	  DM_CompositionNode->aliasCommand = headOfCommandList;
	  // returnScript->print( stdout , true , 1 );
	  break;
	}
	else if( ( strcmp( XMLtag->tag , "command" ) == 0 ) 
		 && (XMLtag->tagType == OpeningTag) ) {
	  char *aliasPattern = XMLtag->getAttributeStringValue( "pattern" );
	    
	  ////////////////////////
	  // parsing the associated action(s)
	  //				commandList = new dm_Command();
	  //				commandList->clean();
	  commandList 
	    = ParseCommand( fileDepth , curChar , returnNode , 
			    false , NULL , NULL );				
	  if( !commandList ) {
	    delete XMLtag;
	    XMLtag = NULL;
	      
	    sprintf( ErrorStr , "Error: incorrect command in command alias list!" ); ReportError( ErrorStr ); throw 50;
	  }
	    
	  // chaining commands
	  if( !headOfCommandList ) {
	    headOfCommandList = commandList;
	  }
	  else {
	    auxCommand = headOfCommandList;
	    while( auxCommand->nextCommand ) {
	      auxCommand = auxCommand->nextCommand;
	    }
	    auxCommand->nextCommand = commandList;
	  }
	    
	  ////////////////////////
	  // pattern processing
	  PatternProcessing( commandList , aliasPattern , true );	  
	}
	else {
	  sprintf( ErrorStr , "Error: incorrect tag [%s] in alias script of node [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
	}
      }
	
    }
      
    // script called during the course of rendering
    else if( strcmp( XMLtag->tag , "renderingScript" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      if( returnNode->renderingCommand ) {
	sprintf( ErrorStr , "Error: a node can only have one rendering script!" ); ReportError( ErrorStr ); throw 128;
      }
	
      //		DM_CompositionNode->initializationCommand = new dm_Command();
      //		DM_CompositionNode->initializationCommand->clean();
	
      // new tag XML : opening action element
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	
      if( ( strcmp( XMLtag->tag , "command" ) == 0 ) && (XMLtag->tagType == OpeningTag) ) {
	////////////////////////
	// parsing the associated action(s)
	returnNode->renderingCommand = 
	  ParseCommand( fileDepth , curChar , returnNode ,  false , NULL , NULL );
      }
      else {
	sprintf( ErrorStr , "Error: incorrect tag [%s] in rendering script of node [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ) ; throw 298;	throw 63;
      }
	
      // expecting script closing tag
      ExpectClosingTagXML( fileDepth , curChar ,  "renderingScript", NULL , NULL  );
    }

    // optionnal scalars
    else if( strcmp( XMLtag->tag , "scalar" ) == 0 ) {
      dm_Scalar * currentVar = NULL;
      char * ID = XMLtag->getAttributeStringValue( "id" );
		
      if( ID && (returnNode->getScalarFromName( ID ) 
		 || returnNode->getTableFromName( ID )
		 || returnNode->getMatrixFromName( ID ) ) ) {
	sprintf( ErrorStr , "Error: duplicated scalar, table, or matrix element (%s) in node [%s]!" , ID , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
	
      if( XMLtag->tagType == EmptyTag ) {
	currentVar = new dm_Scalar();
	currentVar->setParameters( XMLtag , false , assign , assignI );
      }
      else {
	dm_Scalar *currentInterpolatedVar = new dm_Scalar();
	currentInterpolatedVar->setParameters( XMLtag , false , assign , assignI );
	  
	// new tag XML : transformation component or closing tag
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	// printf( "Tag %s\n" , tag );
			
	if( strcmp( XMLtag->tag , "interpolator" ) == 0 
	    && (XMLtag->tagType == OpeningTag) ) {
	  dm_VariableInterpolator   *theInterpolator;
	    
	  // allocates the interpolator, sets its parameter and
	  // its base variable that has already been parsed
	  int size = XMLtag->getAttributeIntValue( "size" , false );
	  theInterpolator = new dm_VariableInterpolator( size );
	  theInterpolator->setParameters( XMLtag , false , 
					  assign , assignI );
	  theInterpolator->curVariable = currentInterpolatedVar;
	  if( strcmp( XMLtag->getAttributeStringValue( "type" ) , "scalar" ) != 0 ) {
	    sprintf( ErrorStr , "Error: [scalar] variable interpolator expected in node [%s] (not [%s])!" , returnNode->id , XMLtag->getAttributeStringValue( "type" ) ); ReportError( ErrorStr ) ; throw 298;
	  }
	  theInterpolator->variableType = dm_ScalarType;
	    
	  theInterpolator->parse( fileDepth , curChar , (char *)"scalar" , returnNode );
	  //theInterpolator->print( stdout , true , 0 );
	    
	  currentVar = new dm_Scalar( theInterpolator );
	    
	  // expecting scalar closing tag
	  ExpectClosingTagXML( fileDepth , curChar ,  "scalar", NULL , NULL  );
	}
	else {
	  sprintf( ErrorStr , "Error: unknown component [%s] in node ID [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ); throw 66;
	}
      }
	
      if( !returnNode->lisScalars ) {
	returnNode->lisScalars = currentVar;
      }
      else {
	currentVar->nextVariable = returnNode->lisScalars;
	returnNode->lisScalars = currentVar;
      }
    }
      
    // optionnal tables
    else if( strcmp( XMLtag->tag , "table" ) == 0 ) {
      dm_Table * currentVar = NULL;
      char * ID = XMLtag->getAttributeStringValue( "id" );
      if( ID && (returnNode->getScalarFromName( ID ) 
		 || returnNode->getTableFromName( ID )
		 || returnNode->getMatrixFromName( ID )  ) ) {
	sprintf( ErrorStr , "Error: duplicated scalar, table, or matrix element (%s) in node [%s]!" , ID , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
	
      int size = XMLtag->getAttributeIntValue( "size" , false );
	
      if( XMLtag->tagType == EmptyTag ) {
	currentVar = new dm_Table( size );
	currentVar->setParameters( XMLtag , false , assign , assignI );
      }
      else {
	dm_Table *currentInterpolatedVar = new dm_Table( size );
	currentInterpolatedVar->setParameters( XMLtag , false , assign , assignI );
	  
	// new tag XML : transformation component or closing tag
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
	// printf( "Tag %s\n" , tag );
	  
	if( strcmp( XMLtag->tag , "interpolator" ) == 0 
	    && (XMLtag->tagType == OpeningTag) ) {
	  dm_VariableInterpolator   *theInterpolator;
	    
	  // allocates the interpolator, sets its parameter and
	  // its base variable that has already been parsed
	  int size = XMLtag->getAttributeIntValue( "size" , false );
	  theInterpolator = new dm_VariableInterpolator( size );
	  theInterpolator->setParameters( XMLtag , false , assign , assignI );
	  theInterpolator->curVariable = currentInterpolatedVar;
	  if( strcmp( XMLtag->getAttributeStringValue( "type" ) , "table" ) != 0 ) {
	    sprintf( ErrorStr , "Error: [table] variable interpolator expected in node [%s] (not [%s])!" , returnNode->id , XMLtag->getAttributeStringValue( "type" ) ); ReportError( ErrorStr ) ; throw 298;
	  }				
	  theInterpolator->variableType = dm_TableType;
	    
	  theInterpolator->parse( fileDepth , curChar , (char *)"table" , returnNode );
	  //theInterpolator->print( stdout , true , 0 );
	    
	  currentVar = new dm_Table( theInterpolator );
	    
	  // expecting table closing tag
	  ExpectClosingTagXML( fileDepth , curChar ,  "table", NULL , NULL  );
	}
	  
	else {
	  sprintf( ErrorStr , "Error: unknown component [%s] in node ID [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ); throw 66;
	}
      }
		
      if( !returnNode->lisTables ) {
	returnNode->lisTables = currentVar;
      }
      else {
	currentVar->nextVariable = returnNode->lisTables;
	returnNode->lisTables = currentVar;
      }
    }

    // optionnal matrices
    else if( strcmp( XMLtag->tag , "matrix" ) == 0 ) {
      char * ID = XMLtag->getAttributeStringValue( "id" );
      if( ID && (returnNode->getScalarFromName( ID ) 
		 || returnNode->getMatrixFromName( ID ) ) ) {	
	sprintf( ErrorStr , "Error: duplicated scalar, table, or matrix element (%s) in node [%s]!" , ID , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
	
      // only square matrices for the moment
      int size = XMLtag->getAttributeIntValue( "size" , false );

      dm_Matrix *currentVar = new dm_Matrix( size );
      currentVar->setParameters( XMLtag , false , assign , assignI );
	
      if( !returnNode->lisMatrices ) {
	returnNode->lisMatrices = currentVar;
      }
      else {
	currentVar->nextVariable = returnNode->lisMatrices;
	returnNode->lisMatrices = currentVar;
      }
    }
      
    // daughter node (test on a parameter value)
    else if( strcmp( XMLtag->tag , "test" ) == 0 ) {
      if( XMLtag->tagType != OpeningTag ) {
	sprintf( ErrorStr , "Error: opening tag expected for test element in node [%s]!" , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
		
      // reading a repeat node
      int nbReturnedNodes;
      currentNode = ParseTest( fileDepth , curChar , XMLtag , &nbReturnedNodes );
      // node chaining
      if( !daughterNode ) {
	daughterNode = currentNode;
      } 
      else {
	auxNode = daughterNode;
	while( auxNode->sister ) {
	  auxNode = auxNode->sister;
	}
	auxNode->sister = currentNode;
      }
    }
      
    // daughter node (set on a parameter value)
    else if( strcmp( XMLtag->tag , "set" ) == 0 ) {
      // reading a set node
      int        nbReturnedNodes;
      TagType    setTagType = (TagType)XMLtag->tagType;
      currentNode 
	= ParseSet( fileDepth , curChar , XMLtag , &nbReturnedNodes );
	
      // opening tag: a subnode is generated
      if( setTagType == OpeningTag) {
	// node chaining
	if( !daughterNode ) {
	  daughterNode = currentNode;
	}
	else {
	  auxNode = daughterNode;
	  while( auxNode->sister ) {
	    auxNode = auxNode->sister;
	  }
	  auxNode->sister = currentNode;
	}
      }
	
      // empty tag: a remanent parameter is added
      // no subnode is generated
    }
      
    // daughter node (reference)
    else if( strcmp( XMLtag->tag , "use" ) == 0 ) {
      if( XMLtag->tagType != EmptyTag ) {
	sprintf( ErrorStr , "Error: empty tag expected for test element in node [%s]!" , returnNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
	
      // references are solved in a second pass
      currentNode = new dm_SceneNode( ANodeLink );
      StrCpy( &(currentNode->id) ,  XMLtag->getAttributeStringValue( "xlink:href" ) );
      XMLtag->BooleanFieldValue( "displayed" , &(currentNode->displayed) );
	
      // node chaining
      if( !daughterNode ) {
	daughterNode = currentNode;
      }
      else {
	auxNode = daughterNode;
	while( auxNode->sister ) {
	  auxNode = auxNode->sister;
	}
	auxNode->sister = currentNode;
      }
    }
      
    else {
      sprintf( ErrorStr , "Error: unknown component [%s] in node ID [%s]!" , XMLtag->tag , returnNode->id ); ReportError( ErrorStr ); throw 66;
    }
  }
}

void ParseNodeSet( int *fileDepth ,  int *p_c ,
		   dm_SceneNode **currentNode , int *nbParsedNodes ,
		   dm_XMLTag *XMLtag ) {
  // single node
  if( strcmp( XMLtag->tag , "node" ) == 0 ) {
    //printf( "parse node (%s)\n" , IDnode );
    *currentNode = ParseNode( fileDepth , p_c , false , XMLtag );

    //fprintf( stdout , "Node %s, %s\n" , NodeTypeString[(*currentNode)->type] , (*currentNode)->id);

    *nbParsedNodes = 1;
    //(*currentNode)->print( stdout , 1 , 1 );
  }

  // link (<use>): strcmp( XMLtag->tag , "use" ) == 0
  else if( strcmp( XMLtag->tag , "use" ) == 0 ) {
    // references are solved in a second pass
    (*currentNode) = new dm_SceneNode(ANodeLink);
    XMLtag->BooleanFieldValue( "displayed" , &((*currentNode)->displayed) );
    StrCpy( &((*currentNode)->id) , 
	    XMLtag->getAttributeStringValue( "xlink:href" ) );
    *nbParsedNodes = 1;
  }

  else {
    sprintf( ErrorStr , "Error: unknown node tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 50;
  }
}

void ParseVector( int *fileDepth , int *p_c , dm_vector3 *parsedVector ,
		 dm_XMLTag *XMLtag ) {
  // getting the attributes of the 
  numericalMode mode = XMLtag->getModeValue();
  if( mode == DM_RANDOM ) {
    float inf, sup;
    ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
    parsedVector->x = generateRandomFloat( inf , sup );
    ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
    parsedVector->y = generateRandomFloat( inf , sup );
    ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
    parsedVector->z = generateRandomFloat( inf , sup );
  }
  else if( mode == DM_DECIMAL ) {
    parsedVector->x 
      = ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
    parsedVector->y 
      = ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
    parsedVector->z 
      = ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
  }

  // expecting vector closing tag
  ExpectClosingTagXML( fileDepth , p_c ,  "vector" , NULL , NULL );
}

void ParseColor( int *fileDepth , int *p_c , GLfloat *aColor ,
		 dm_XMLTag *XMLtag ) {
  if( XMLtag->hasAttribute( "r" ) ) {
    aColor[0] = (float)XMLtag->getAttributeDoubleValue( "r" , false );
  }
  if( XMLtag->hasAttribute( "g" ) ) {
    aColor[1] = (float)XMLtag->getAttributeDoubleValue( "g" , false );
  }
  if( XMLtag->hasAttribute( "b" ) ) {
    aColor[2] = (float)XMLtag->getAttributeDoubleValue( "b" , false );
  }
  if( XMLtag->hasAttribute( "a" ) ) {
    aColor[3] = (float)XMLtag->getAttributeDoubleValue( "a" , false );
  }
  
  if( XMLtag->tagType == OpeningTag ) {
    
    // getting the attributes of the 
    numericalMode mode = XMLtag->getModeValue();
    if( mode == DM_RANDOM ) {
      float inf, sup;
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aColor[0] = generateRandomFloat( inf , sup );
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aColor[1] = generateRandomFloat( inf , sup );
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aColor[2] = generateRandomFloat( inf , sup );
      if( *p_c != '/' ) {
	ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
	aColor[3] = generateRandomFloat( inf , sup );
      }
    }
    else if( mode == DM_DECIMAL ) {
      aColor[0] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aColor[1] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aColor[2] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      if( *p_c != '/' ) {
	aColor[3] 
	  = ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );      }
    }
    
    // modifies coordinates in case of spherical coordinates
    //aColor->PositionneColor();
    
    // expecting color closing tag
    ExpectClosingTagXML( fileDepth , p_c ,  "color" , NULL , NULL );
  }
}

void ParsePoint( int *fileDepth , int *p_c , dm_point3 *aPoint ,
		 dm_XMLTag *XMLtag ) {
  if( XMLtag->hasAttribute( "x" ) ) {
    aPoint->x = (float)XMLtag->getAttributeDoubleValue( "x" , false );
  }
  if( XMLtag->hasAttribute( "y" ) ) {
    aPoint->y = (float)XMLtag->getAttributeDoubleValue( "y" , false );
  }
  if( XMLtag->hasAttribute( "z" ) ) {
    aPoint->z = (float)XMLtag->getAttributeDoubleValue( "z" , false );
  }
  
  if( XMLtag->tagType == OpeningTag ) {
    
    // getting the attributes of the 
    numericalMode mode = XMLtag->getModeValue();
    if( mode == DM_RANDOM ) {
      float inf, sup;
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aPoint->x = generateRandomFloat( inf , sup );
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aPoint->y = generateRandomFloat( inf , sup );
      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
      aPoint->z = generateRandomFloat( inf , sup );
    }
    else if( mode == DM_DECIMAL ) {
      aPoint->x 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aPoint->y 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aPoint->z 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
    }
    
    // modifies coordinates in case of spherical coordinates
    //aPoint->PositionnePoint();
    
    // expecting point closing tag
    ExpectClosingTagXML( fileDepth , p_c ,  "point" , NULL , NULL );
  }
}


void ParseTabVertex( int *fileDepth , int *p_c , 
		     MeshVertex *tabVertex , 
		     int nbVertices ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int  nbParsedVertices;
//   int  nbParsedNormals;

  nbParsedVertices = 0;
//   nbParsedNormals = 0;

  while( true ) {
    // new tag XML
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // tabvertex closing tag
    if( strcmp( XMLtag->tag , "tabvertex" ) == 0 
	   && (XMLtag->tagType == ClosingTag) ) {
      if( nbParsedVertices < nbVertices  ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: incomplete vertex table!"); ReportError( ErrorStr ); throw 6;
      }
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // list of vertex coordinates
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "vertexCoordinate" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the point="
      if( strcmp( temporaryAttribute , "point" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: vertexCoordinate tag should have a point attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbVertices ; index++ ) {
	float val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabVertex[index].x = val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabVertex[index].y = val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabVertex[index].z = val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: vertex index out of bounds!"); ReportError( ErrorStr ); throw 7;
      }

      // parsing completed
      nbParsedVertices = nbVertices;

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }

    // unexpected tag
    else {
	sprintf( ErrorStr , "Error: unknown tabvertex component tag [%s]!" , XMLtag->tag ); delete XMLtag; ReportError( ErrorStr ); throw 167;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void ParseTabFace( int *fileDepth , int *p_c , 
		   MeshFace *tabFace_vertex , 
		   MeshFace *tabFace_normal , 
		   MeshFace *tabFace_texturecoord , 
		   int nbFaces ,
		   int nbVertices ,
		   int nbTextureCoords ,
		   int nbNormals ,
		   int nbTangents 
// 		   , int nbColors 
		   ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int  nbParsedFaces;

  nbParsedFaces = 0;
  for( int ind = 0 ; ind < nbFaces * 3 ; ind++ ) {
    tabFace_vertex[ind].ind = -1;
    tabFace_normal[ind].ind = -1;
    tabFace_texturecoord[ind].ind = -1;
  } 


  while( true ) {
    // new tag XML
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // tabface closing tag
    if( strcmp( XMLtag->tag , "tabface" ) == 0 
	   && (XMLtag->tagType == ClosingTag) ) {
      printf( "nbParsedFaces %d nbFaces %d\n" , nbParsedFaces , nbFaces );
      if( nbParsedFaces < nbFaces  ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: incomplete face table!"); ReportError( ErrorStr ); throw 6;
      }
      for( int ind = 0 ; ind < nbParsedFaces * 3 ; ind++ ) {
	if( tabFace_vertex[ind].ind >= (unsigned int)nbVertices ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: incorrect vertex index (%d) in face %d: min=1 max=%d!" , tabFace_vertex[ind].ind + 1 , ind + 1 , nbVertices ); ReportError( ErrorStr ); throw 6;
	}
	if( tabFace_normal[ind].ind >= (unsigned int)nbNormals ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: incorrect normal index (%d) in face %d: min=1 max=%d!" , tabFace_normal[ind].ind + 1 , ind + 1 , nbNormals ); ReportError( ErrorStr ); throw 6;
	}
	if( tabFace_texturecoord[ind].ind >= (unsigned int)nbTextureCoords ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: incorrect textureCoord index (%d) in face %d: min=1 max=%d!" , tabFace_texturecoord[ind].ind + 1 , ind + 1 , nbTextureCoords ); ReportError( ErrorStr ); throw 6;
	}
      }
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }


    // list of vertex indices
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "vertexIndex" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the index="
      if( strcmp( temporaryAttribute , "index" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: vertexIndex tag should have an index attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbFaces * 3 ; index++ ) {
	long val;
	val = ReadInteger( fileDepth , p_c  ,
			   true , NULL , NULL ) - 1;
	tabFace_vertex[index].ind = (int)val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: face vertex index out of bounds!"); ReportError( ErrorStr ); throw 7;
      }

      // parsing completed
      nbParsedFaces = nbFaces;

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }

    // list of normal indices
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "normalIndex" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the index="
      if( strcmp( temporaryAttribute , "index" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: vertexIndex tag should have an index attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbFaces * 3 ; index++ ) {
	long val;
	val = ReadInteger( fileDepth , p_c  ,
			   true , NULL , NULL ) - 1;
	tabFace_normal[index].ind = (int)val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: face normal index out of bounds!"); ReportError( ErrorStr ); throw 7;
      }

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }


    // list of texture coordinate indices
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "textureCoordinateIndex" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the index="
      if( strcmp( temporaryAttribute , "index" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: vertexIndex tag should have an index attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbFaces * 3 ; index++ ) {
	long val;
	val = ReadInteger( fileDepth , p_c  ,
			   true , NULL , NULL ) - 1;
	tabFace_texturecoord[index].ind = (int)val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: face texture coordinate index out of bounds!"); ReportError( ErrorStr ); throw 7;
      }

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }

    // unexpected tag
    else {
	sprintf( ErrorStr , "Error: unknown tabface component tag [%s]!" , XMLtag->tag ); delete XMLtag; ReportError( ErrorStr ); throw 167;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void ParseTabTextureCoord( int *fileDepth , int *p_c , 
			   MeshTexturecoord *tabTextureCoord , 
			   int nbTextureCoords ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int  nbParsedTextureCoords;

  nbParsedTextureCoords = 0;
  for( int ind = 0 ; ind < nbTextureCoords ; ind++ ) {
    tabTextureCoord[ind].s0 = 0;
    tabTextureCoord[ind].t0 = 0;
    tabTextureCoord[ind].u0 = 0;
  }

  while( true ) {
    // new tag XML
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // tabvertex closing tag
    if( strcmp( XMLtag->tag , "tabtextureCoord" ) == 0 
	   && (XMLtag->tagType == ClosingTag) ) {
      if( nbParsedTextureCoords < nbTextureCoords  ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: incomplete texture coordinate table!"); ReportError( ErrorStr ); throw 6;
      }
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // list of texture coordinates
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "textureCoordinate" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the point="
      if( strcmp( temporaryAttribute , "point" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: normalCoordinate tag should have a vector attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbTextureCoords ; index++ ) {
	float val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabTextureCoord[index].s0 = val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabTextureCoord[index].t0 = val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: texture coordinates out of bounds (%d)!" , nbTextureCoords ); ReportError( ErrorStr ); throw 7;
      }	

      // parsing completed
      nbParsedTextureCoords = nbTextureCoords;

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }

    // unexpected tag
    else {
	sprintf( ErrorStr , "Error: unknown tabtextureCoord component tag [%s]!" , XMLtag->tag ); delete XMLtag; ReportError( ErrorStr ); throw 167;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void ParseTabNormal( int *fileDepth , int *p_c , 
		     MeshNormal *tabNormal , 
		     int nbNormals , 
		     bool normal_normalization ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int  nbParsedNormals;

  nbParsedNormals = 0;
  for( int ind = 0 ; ind < nbNormals ; ind++ ) {
    tabNormal[ind].nx = 0;
    tabNormal[ind].ny = 0;
    tabNormal[ind].nz = 0;
  }

  while( true ) {
    // new tag XML
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // tabvertex closing tag
    if( strcmp( XMLtag->tag , "tabnormal" ) == 0 
	   && (XMLtag->tagType == ClosingTag) ) {
      if( nbParsedNormals < nbNormals  ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: incomplete normal table!"); ReportError( ErrorStr ); throw 6;
      }
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // list of normal coordinates
    // the tag is parsed here for optimisation purposes
    else if( strcmp( XMLtag->tag , "normalCoordinate" ) == 0 ) {
      char *temporaryAttribute = NULL;

      // reads the attribute in a temporary string and
      // allocates final string and copies its value in temporary array
      ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
			 ":_-" , true , NULL, NULL );

      // reads the vector="
      if( strcmp( temporaryAttribute , "vector" ) != 0
	  || !ExpectsChar( fileDepth , '=' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: normalCoordinate tag should have a vector attribute!"); ReportError( ErrorStr ); throw 6;
      }	

      for( int index = 0 ; index < nbNormals ; index++ ) {
	float val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabNormal[index].nx = val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabNormal[index].ny = val;
	val = ReadFloat( fileDepth , p_c  ,
			 true , NULL , NULL , false );
	tabNormal[index].nz = val;
      }

      // reads the " />
      if( !ExpectsChar( fileDepth , '"' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '/' , p_c , true , NULL, NULL )
	  || !ExpectsChar( fileDepth , '>' , p_c , true , NULL, NULL ) ) {
	delete XMLtag;
	XMLtag = NULL;
	if (temporaryAttribute) {
	  delete [] temporaryAttribute;
	  temporaryAttribute = NULL;
	}

	sprintf( ErrorStr , "Error: normals out of bounds (%d)!" , nbNormals); ReportError( ErrorStr ); throw 7;
      }	

      // parsing completed
      nbParsedNormals = nbNormals;

      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
    }

    // unexpected tag
    else {
	sprintf( ErrorStr , "Error: unknown tabnormal component tag [%s]!" , XMLtag->tag ); delete XMLtag; ReportError( ErrorStr ); throw 167;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void ParseTabWeights( int *fileDepth , int *p_c , 
		      float **tabWeights , 
		      int nbWeights ,
		      int *nbComponents , bool atRunTime , 
		      char *chaine , int *ind ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int  nbParsedWeights;

  for( int index = 0; index < nbWeights ; index++ ) {
    // tab weight allocation and initialization
    tabWeights[index] = new float[*nbComponents];
    //printf( "|%d|\n" ,  tabWeights[indWeight] );
    for(int indComponent = 0 ; indComponent < *nbComponents ; 
	indComponent++ ) {
      tabWeights[index][indComponent] = 0.0;
    }
  }

  nbParsedWeights = 0;
  while( true ) {
    // new tag XML
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , chaine , ind );

    // tabweight closing tag
    if( strcmp( XMLtag->tag , "tabweights" ) == 0 
	   && (XMLtag->tagType == ClosingTag) ) {
      if( nbParsedWeights < nbWeights  ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: incomplete weight table!"); ReportError( ErrorStr ); throw 6;
      }
      //printf( "End of weight table\n");
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // interpolator weights (1 for each component)
    else if( strcmp( XMLtag->tag , "weights" ) == 0 ) {
      // weight table rank 
      int index;
      if( !atRunTime ) {
	index = XMLtag->getAttributeIntValue( "index" , false ) - 1;
	if( index < 0 || index >= nbWeights ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: weight index out of bounds!"); ReportError( ErrorStr ); throw 104;
	}
      }
      // scripts for interpolator modification only have one table 
      // the target one
      else {
	index = 0;
      }

      // number of components in the interpolator
      int dim = XMLtag->getAttributeIntValue( "dim" , false );
      if( *nbComponents > 0 ) {
	if( dim != *nbComponents ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: weight size must be [%d] not [%d]!" , *nbComponents , dim); ReportError( ErrorStr ); throw 105;
	}
      }
      else {
	*nbComponents = dim;
      }

      // weight parsing
      float weightSum = 0;
      for(int indComponent = 0 ; indComponent < *nbComponents ; 
	  indComponent++ ) {
	tabWeights[index][indComponent] = 
	  ReadExpression( fileDepth , p_c , true , chaine , 
			  ind , false , NULL );
	weightSum += tabWeights[index][indComponent];
      }

      // weight normalization if not a script command
      // because script command can use differential values
      // with operators such as += or -=
      if( weightSum != 0 && !atRunTime ) {
	for(int indComponent = 0 ; indComponent < *nbComponents ; 
	    indComponent++ ) {
	  tabWeights[index][indComponent] /= weightSum;
	}
      }
      nbParsedWeights++;
      //printf( "|%d|\n" ,  tabWeights[index] );

      // expecting weight closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  "weights" , chaine , ind );
      
      // there is only one weight table in scripts
      if( atRunTime ) {
	delete XMLtag;
	XMLtag = NULL;

	return;
      }
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

