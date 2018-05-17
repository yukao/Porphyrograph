/*! \file dm-strings.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-strings.cpp
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

void assign( float & oper , float arg ) {
  oper = arg;
}
void assignI( int & oper , int arg ) {
  oper = arg;
}
void add( float & oper , float arg ) {
  oper += arg;
}
void addI( int & oper , int arg ) {
  oper += arg;
}
void sub( float & oper , float arg ) {
  oper -= arg;
}
void subI( int & oper , int arg ) {
  oper -= arg;
}
void mult( float & oper , float arg ) {
  oper *= arg;
}
void multI( int & oper , int arg ) {
  oper *= arg;
}
void div( float & oper , float arg ) {
  oper /= arg;
}
void divI( int & oper , int arg ) {
  oper /= arg;
}
void mod( float & oper , float arg ) {
  oper = (float)((int)oper % (int)arg);
}
void modI( int & oper , int arg ) {
  oper %= arg;
}

/*! \name Group pointer_dynamic_operator_choise
 *  \brief arrays of pointers to functions for dynamic operator choice in scripts
 */
// \{
FloatBinOp FloatOperators[EmptyOperator] = { assign , add , sub , mult , div , mod };
IntBinOp IntOperators[EmptyOperator] = { assignI , addI , subI , multI , divI , modI };
const char *OperatorString[EmptyOperator + 1] = { "=" , "+=" , "-=" , "*=" , "/=" , "%=" , "emptyoperator" };

const char *BooleanOperatorString[EmptyBooleanOperator + 1] = { "==" , "!=" , "<=" , ">=" , "<"  , ">" , "emptybooleanoperator" };
const char *BooleanOperatorSGMLString[EmptyBooleanOperator + 1] = { "==" , "!=" , "&lt;=" , "&gt;=" , "&lt;"  , "&gt;" , "emptybooleanoperator" };
// \}

/// first letter of OSC data types correspond to
/// first letter of VC data types
const char *DataTypeStrings[EmptyDataType + 1] = { "string" , "int" , "bool" , "float" , "texture" , "emptydatatype" };


double my_round(const double x) {
  return (x>=0 ? floor(x + 0.5) : ceil(x - 0.5));
}

////////////////////////////////////////////////////////////
// CLASS FOR PARSING ARITHMETIC EXPRESSIONS

/*! \name Group string_priority_number_operands
 *  \brief string / priority / number of operands
 */
// @{
SOperator MaxOperator("max",3,2,Max);
SOperator MinOperator("min",3,2,Min);
SOperator PowerOperator("pow",3,2,Power);
SOperator LogarithmOperator("log",3,2,Logarithm);
SOperator RandomOperator("rand",3,2,Random);
SOperator CosineOperator("cos",3,1,Cosine);
SOperator SinusOperator("sin",3,1,Sinus);
SOperator TangentOperator("tan",3,1,Tangent);
SOperator AcosineOperator("acos",3,1,Acosine);
SOperator AsinusOperator("asin",3,1,Asinus);
SOperator AtangentOperator("atan",3,1,Atangent);
SOperator RoundOperator("round",3,1,Round);
SOperator RoundOldOperator("Rint",3,1,Round);
SOperator SqrtOperator("sqrt",3,1,Sqrt);
SOperator SqrtOldOperator("Sqrt",3,1,Sqrt);
SOperator FloorOperator("floor",3,1,Floor);
SOperator AbsOperator("abs",3,1,Abs);
SOperator SignOperator("sign",3,1,Sign);
SOperator AdditionOperator("+",1,2,Addition);
SOperator SubstractionOperator("-",1,2,Substraction);
SOperator MultiplicationOperator("*",2,2,Multiplication);
SOperator DivisionOperator("/",2,2,Division);
SOperator MatMatMultiplicationOperator(";",2,32,NULL);
SOperator MatVecMultiplicationOperator(":",2,19,NULL);
SOperator VecMatMultiplicationOperator("?",2,19,NULL);
SOperator HomogMatVecMultiplicationOperator("::",2,20,NULL);
SOperator HomogVecMatMultiplicationOperator("??",2,20,NULL);
SOperator VecNormOperator("norm",3,1,NULL);
SOperator MatInverseOperator("inv",3,1,NULL);
SOperator PointProjectOperator("proj",3,1,NULL);
SOperator PointUnProjectOperator("unproj",3,1,NULL);
SOperator HomographyXOperator("homographyX",3,1,NULL);
SOperator HomographyYOperator("homographyY",3,1,NULL);
SOperator HomographyZOperator("homographyZ",3,1,NULL);
SOperator ToEulerOperator("toEuler",3,1,NULL);
SOperator FromEulerOperator("fromEuler",3,1,NULL);
SOperator CameraOperator("camera",3,1,NULL);
SOperator ModOperator("%",2,2,Mod);
SOperator NowOperator("now",3,0,Now);
SOperator FalseOperator("false",3,0,IsFalse);
SOperator TrueOperator("true",3,0,IsTrue);
SOperator CurrentFrameOperator("currentFrame",3,0,CurrentFrame);
SOperator NOP(",",0,0,NULL);
SOperator LeftBraceOperator("(",0,0,NULL);
SOperator RightBraceOperator(")",0,0,NULL);


enum predefinedOperatorsID{
  DM_predefMaxOperator = 0,
  DM_predefMinOperator,
  DM_predefPowerOperator,
  DM_predefLogarithmOperator,
  DM_predefRandomOperator,
  DM_predefCosineOperator,
  DM_predefSinusOperator,
  DM_predefTangentOperator,
  DM_predefAcosineOperator,
  DM_predefAsinusOperator,
  DM_predefAtangentOperator,
  DM_predefRoundOperator,
  DM_predefRoundOldOperator,
  DM_predefSqrtOperator,
  DM_predefSqrtOldOperator,
  DM_predefFloorOperator,
  DM_predefAbsOperator,
  DM_predefSignOperator,
  DM_predefAdditionOperator,
  DM_predefSubstractionOperator,
  DM_predefMultiplicationOperator,
  DM_predefDivisionOperator,
  DM_predefMatMatMultiplicationOperator,
  DM_predefMatVecMultiplicationOperator,
  DM_predefVecMatMultiplicationOperator,
  DM_predefHomogMatVecMultiplicationOperator,
  DM_predefHomogVecMatMultiplicationOperator,
  DM_predefVecNormOperator,
  DM_predefMatInverseOperator,
  DM_predefPointProjectOperator,
  DM_predefPointUnProjectOperator,
  DM_predefHomographyXOperator,
  DM_predefHomographyYOperator,
  DM_predefHomographyZOperator,
  DM_predefToEulerOperator,
  DM_predefFromEulerOperator,
  DM_predefCameraOperator,
  DM_predefModOperator,
  DM_predefNowOperator,
  DM_predefFalseOperator,
  DM_predefTrueOperator,
  DM_predefCurrentFrameOperator,
  DM_predefNOP,
  DM_predefLeftBraceOperator,
  DM_predefRightBraceOperator
};
SOperator* predefinedOperators[g_constNumberOfOperators] = {
  &MaxOperator,
  &MinOperator,
  &PowerOperator,
  &LogarithmOperator,
  &RandomOperator,
  &CosineOperator,
  &SinusOperator,
  &TangentOperator,
  &AcosineOperator,
  &AsinusOperator,
  &AtangentOperator,
  &RoundOperator,
  &RoundOldOperator,
  &SqrtOperator,
  &SqrtOldOperator,
  &FloorOperator,
  &AbsOperator,
  &SignOperator,
  &AdditionOperator,
  &SubstractionOperator,
  &MultiplicationOperator,
  &DivisionOperator,
  &MatMatMultiplicationOperator,
  &MatVecMultiplicationOperator,
  &VecMatMultiplicationOperator,
  &HomogMatVecMultiplicationOperator,
  &HomogVecMatMultiplicationOperator,
  &VecNormOperator,
  &MatInverseOperator,
  &PointProjectOperator,
  &PointUnProjectOperator,
  &HomographyXOperator,
  &HomographyYOperator,
  &HomographyZOperator,
  &ToEulerOperator,
  &FromEulerOperator,
  &CameraOperator,
  &ModOperator,
  &NowOperator,
  &FalseOperator,
  &TrueOperator,
  &CurrentFrameOperator,
  &NOP,
  &LeftBraceOperator,
  &RightBraceOperator
};
// @}


NodeType GetNodeType( char *tag ) {
  NodeType thetype = EmptyNode;
  for( int ind = 0 ; ind < EmptyNode ; ind++ ) {
    if( NodeTypeString[ind] && strcmp( NodeTypeString[ind] , tag ) == 0 ) {
      thetype = (NodeType)ind;
      break;
    }
  }
  return thetype;
}

Operator GetOperator( char *tag ) {
  Operator thetype = EmptyOperator;
  for( int ind = 0 ; ind < EmptyOperator ; ind++ ) {
    if( strcmp( OperatorString[ind] , tag ) == 0 ) {
      thetype = (Operator)ind;
      break;
    }
  }
  if( thetype == EmptyOperator ) {
    sprintf( ErrorStr , "Error: unknown arithmetic operator [%s]!" , tag ); ReportError( ErrorStr );
  }
  return thetype;
}

BooleanOperator GetBooleanOperator( char *tag ) {
  BooleanOperator thetype = EmptyBooleanOperator;
  for( int ind = 0 ; ind < EmptyBooleanOperator ; ind++ ) {
    if( strcmp( BooleanOperatorString[ind] , tag ) == 0 
	|| strcmp( BooleanOperatorSGMLString[ind] , tag ) == 0 ) {
      thetype = (BooleanOperator)ind;
      break;
    }
  }
  if( thetype == EmptyBooleanOperator ) {
    sprintf( ErrorStr , "Error: unknown boolean operator [%s]!" , tag ); ReportError( ErrorStr );
  }
  return thetype;
}

bool booleanNumericalExpression( float operand1 , BooleanOperator oper , 
				 float operand2 ) {
  return ( ( oper == Equal && ( operand1 == operand2 ) ) 
	   || ( oper == Different && ( operand1 != operand2 ) ) 
	   || ( oper == LessOrEqual && ( operand1 <= operand2 ) ) 
	   || ( oper == Less && ( operand1 < operand2 ) ) 
	   || ( oper == GreaterOrEqual && ( operand1 >= operand2 ) ) 
	   || ( oper == Greater && ( operand1 > operand2 ) ) );
}

bool booleanStringExpression( char *operand1 , BooleanOperator oper , 
			      char * operand2 ) {
  return ( (oper == Equal && stringEqual( operand1 , operand2 ) ) 
	   || (oper == Different && strcmp( operand1 , operand2 ) != 0 ) 
	   || (oper == LessOrEqual && strcmp( operand1 , operand2 ) <= 0 ) 
	   || (oper == Less && strcmp( operand1 , operand2 ) < 0 ) 
	   || (oper == GreaterOrEqual && strcmp( operand1 , operand2 ) >= 0 ) 
	   || (oper == Greater && strcmp( operand1 , operand2 ) > 0 ) );
}

bool inCharSet( char aChar , char *charSet ) {
  if( charSet[1] != '-' && aChar == *charSet ) {
    return true;
  }
  else if( charSet[1] == '-' && charSet[2]
	   && aChar >= *charSet && aChar <= charSet[2] ) {
    return true;
  }
  return false;
}

bool stringEqual( char *searchedId , char *refId ) {
  // printf( "(%s) = (%s)\n" , searchedId , refId );

  // strict equality
  if( strcmp( searchedId , refId ) == 0 ) {
    // printf( "true 0\n" );
    return true;
  }

  char *posStar = NULL;
  char *posQuest = NULL;
  char *posBegCharSet = NULL;
  char *posEndCharSet = NULL;

  char *tmp = searchedId;
  while( *tmp ) {
    if( *tmp == '*' ) {
      posStar = tmp;
      break;
    }
    else if( *tmp == '?' ) {
      posQuest = tmp;
      break;
    }
    else if( *tmp == '[' ) {
      posBegCharSet = tmp;
      break;
    }
    tmp++;
  }

  // wild card
  if( posStar ) {
//     printf( "search [%s] ref [%s] common length %d\n" , searchedId , refId ,
// 	    strlen( searchedId ) - strlen( posStar ) );
    // prefix equality
    if( strncmp( searchedId , refId , 
		 strlen( searchedId ) - strlen( posStar ) ) != 0 ) {
     // printf( "false\n" );
     return false;
    }

    // skips the star char
    posStar++;

    // postfix equality
    if( posStar ) {
      // skips prefix
      refId += strlen( searchedId ) - strlen( posStar ) - 1;
      // and tries a match at any place following the prefix
      while( *refId ) {
	if( stringEqual( posStar , refId ) ) {
	  // printf( "true 1\n" );
	  return true;
	}
	refId++;
      }
      // empty string: match ok if both strings are empty
      if( *posStar ) {
	return false;
      }
      else {
	// printf( "true 2\n" );
	return true;
      }
    }
    // no postfix
    else {
      // printf( "true 3\n" );
      return true;
    }
  }
  // character set
  else if( posBegCharSet ) {
    if( !(posEndCharSet = strchr( posBegCharSet , ']' ) ) ) {
      sprintf( ErrorStr , "Error: unbalanced char set in regular expression (%s)!" , searchedId ); ReportError( ErrorStr ); throw 16;
    }
//     printff( "search [%s] ref [%s] common length %d\n" , searchedId , refId ,
// 	    strlen( searchedId ) - strlen( posQuest ) );
    // prefix equality
    if( strncmp( searchedId , refId , 
		 strlen( searchedId ) - strlen( posBegCharSet ) ) != 0 ) {
      // printf( "false\n" );
      return false;
    }

    // stores the charset pointer and 
    // skips the char set and its possible modifier
    char modifier = '1';
    if( posEndCharSet[1] == '+' || posEndCharSet[1] == '*' ) {
      modifier = posEndCharSet[1];
      posEndCharSet++;
    }
    posEndCharSet++;
      
    // skips the char set opening char ([)
    posBegCharSet++;

    // postfix equality
    // skips prefix
    refId += strlen( searchedId ) - strlen( posBegCharSet ) - 1;
    // and tries a match at any place following the prefix
    // with char matching inbetween
    if( modifier == '*' ) {
      while(  *refId && inCharSet( *refId , posBegCharSet ) ) {
	if( stringEqual( posEndCharSet , refId + 1 ) ) {
	  // printf( "true 4\n" );
	  return true;
	}
	refId++;
      }
      // end of the searched pattern
      // printf( "%d\n" , stringEqual( posEndCharSet , refId ) );
      return stringEqual( posEndCharSet , refId );
    }
    // and tries a match at any place following the prefix
    // with char matching inbetween (at least 1 char)
    else if( modifier == '+' ) {
      // jumps first first char
      if( !inCharSet( *refId , posBegCharSet ) ) {
	// printf( "false\n" );
	return false;
      }
      refId++;
      while(  *refId && inCharSet( *refId , posBegCharSet ) ) {
	if( stringEqual( posEndCharSet , refId + 1 ) ) {
	  // printf( "true 5\n" );
	  return true;
	}
	refId++;
      }
      // end of the searched pattern
      // printf( "%d\n" , stringEqual( posEndCharSet , refId ) );
      return stringEqual( posEndCharSet , refId );
    }
    // and tries a match at any place following the prefix
    // with exactly one char matching inbetween
    else if( modifier == '1' ) {
      if( !inCharSet( *refId , posBegCharSet ) ) {
	// printf( "false\n" );
	return false;
      }
      // end of the searched pattern
      // printf( "%d\n" , stringEqual( posEndCharSet , refId ) );
      return stringEqual( posEndCharSet , refId );
    }
    // all the characters in refId have been covered by the char set
    // printf( "true 6\n" );
    return true;
  }
  // question mark
  else if( posQuest ) {
//     printf( "search [%s] ref [%s] common length %d\n" , searchedId , refId ,
// 	    strlen( searchedId ) - strlen( posQuest ) );
    // prefix equality
    if( strncmp( searchedId , refId , 
		 strlen( searchedId ) - strlen( posQuest ) ) != 0 ) {
      // printf( "false\n" );
      return false;
    }

    // skips the question mark char
    posQuest++;

    // postfix equality
    if( *posQuest ) {
      // searched string prefix longer than the reference string
      if( strlen( searchedId ) - strlen( posQuest )
	  > strlen( refId ) ) {
	// printf( "false\n" );
	return false;
      }
      // skips prefix
      refId += strlen( searchedId ) - strlen( posQuest );
      // and tries a match on the string following the question mark
      return stringEqual( posQuest , refId );
    }
    // no postfix
    // final question mark: prefix equality
    else {
      // printf( "%d\n" , (strlen( searchedId ) == strlen( refId )));
      return (strlen( searchedId ) == strlen( refId ));
    }
  }
  // printf( "false\n" );
  return false;
}


void ReportError( char *errorString ) {
  char *DecycledInputBuffer = NULL;

  // finds the next line beginning in the input buffer
  int ind = InputBufferCurrentBegin;
  while( ind != InputBufferCurrentEnd 
	 && InputBuffer[ ind ] != '\n') {
    ind = (ind + 1) % InputBufferSize;
  }

  // stores the cyclic input buffer into a linear buffer
  DecycledInputBuffer = new char[InputBufferSize];
  int indAux = 0;
  while( ind != InputBufferCurrentEnd ) {
    DecycledInputBuffer[ indAux ] = InputBuffer[ ind ];
    ind = (ind + 1) % InputBufferSize;
    indAux++;
  }
  // copies the final 0
  DecycledInputBuffer[ indAux ] = InputBuffer[ InputBufferCurrentEnd ];
  
  if( fileLog ) {
    fprintf( fileLog , "%s\n" , DecycledInputBuffer );
    fprintf( fileLog , "%s\n" , errorString );
  }
  
  fprintf( stderr , "%s\n" , DecycledInputBuffer );
  fprintf( stderr , "%s\n" , errorString );
  delete [] DecycledInputBuffer;
  DecycledInputBuffer = NULL;
}

int     ReadChar( int *fileDepth , char *charstring , int *ind ) {
  int    c;

  if( !fileDepth && !charstring ) {
    sprintf( ErrorStr , "Error: Empty file!" ); ReportError( ErrorStr ); throw 16;
  }

  if( fileDepth ) {
    c = getc( fileXML[ *fileDepth ] );
    if( DM_EnvironmentNode && DM_EnvironmentNode->trace_input_file ) {
      putchar( c );
    }
    InputBuffer[ InputBufferCurrentEnd ] = c;
    InputBufferCurrentEnd = (InputBufferCurrentEnd + 1 ) 
      % InputBufferSize;
    InputBuffer[ InputBufferCurrentEnd ] = 0;
    if( InputBufferCurrentEnd == InputBufferCurrentBegin ) {
      InputBufferCurrentBegin = (InputBufferCurrentBegin + 1 ) 
	% InputBufferSize;
    }
  }
  else {
    c = charstring[*ind];
    if( c == 0 ) {
      c = EOF;
      (*ind)--;
    }
    else {
      (*ind)++;
    }
  }

  return( c );
}

void SpaceCommentsInclude ( int *fileDepth , int *p_c , char *charstring , int *ind ) {
  static bool CommentErrorReported = false;
  long curCharPos = 0;

  while( true ) {
    if( _SpaceChar(*p_c) ) {
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    // XML comment or node
    else if( *p_c == '<' ) {
      if( fileDepth ) {
	curCharPos = ftell( fileXML[ *fileDepth ] );
      }
      else {
	curCharPos = (long)*ind;
      }
      // next char
      *p_c = ReadChar(fileDepth,charstring,ind);

      // XML comment
      if( *p_c == '!' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	// genuine comment 
	if( *p_c == '-' ) {
	  int comment_finished = false;
	  while( !comment_finished && *p_c != EOF ) {
	    *p_c = ReadChar(fileDepth,charstring,ind);
	    int nb_dash = 0;
	    while( *p_c == '-' ) {
	      nb_dash++;
	      *p_c = ReadChar(fileDepth,charstring,ind);
	      if( nb_dash >= 2 && *p_c == '>' ) {
		*p_c = ReadChar(fileDepth,charstring,ind);
		comment_finished = true;
		break;
	      }
	      else if( nb_dash >= 2 && !CommentErrorReported ) {
		sprintf( ErrorStr , "Error: -- inside comments is not permitted!" ); ReportError( ErrorStr ); 
		CommentErrorReported = true;
	      }
	    }
	  }
	}
	// <!XXX tag 
	else {
	  // special XML tags
	  int level = 1;
	  // looks for balanced ">"
	  while( level && *p_c != EOF ) {
	    if( *p_c == '<' ) {
	      level++;
	    }
	    else if( *p_c == '>' ) {
	      level--;
	    }
	    *p_c = ReadChar(fileDepth,charstring,ind);
	  }
	}
      }
      // XML tag if it is an inclusion tag, 
      // we load the corresponding file, 
      // for any other tag, we return the first 
      // character of the tag: <
      else {
	// we are on the first character after a <
	
	// inclusion tags are only used when reading from a file
	// not when reading from a string
	if( fileDepth ) {
	  // reads the tag in a temporary string and
	  // allocates tag and copies 
	  char  *tempID = NULL;
	  ReadIDWithCarSpec( fileDepth , p_c , &tempID , 
			     "?:_-" , 
			     false , charstring,ind );
	  
	  // return to the beginning of the tag
	  if( fileDepth ) {
	    fseek( fileXML[ *fileDepth ], curCharPos, SEEK_SET);
	  }
	  else {
	    *ind = (int)curCharPos;
	  }
	  *p_c = '<';
	  
	  // erases the console for the console output
	  if( DM_EnvironmentNode && DM_EnvironmentNode->trace_input_file ) {
	    for( int i = 0 ; i < (int)strlen( tempID ) ; i++ ) {
	      putchar( '\b' );
	    }
	  }
	  
	  // it is an inclusion tag, we load the file
	  if( strcmp( tempID , "include" ) == 0 ) {
	    dm_XMLTag *XMLtag = new dm_XMLTag();
	    
	    // expecting point closing tag
	    XMLtag->ParseTagXML( fileDepth , p_c , true , charstring , ind );
	    
	    if( XMLtag->tagType != EmptyTag ) {
	      delete XMLtag;
	      XMLtag = NULL;
	      sprintf( ErrorStr , "Error: empty tag expected for <include/>!" ); ReportError( ErrorStr ); throw 8;
	    }

	    // copies the file name and deallocate the tag that
	    // is no more useful

	    // deallocating memory
	    if( tempID ) {
	      delete [] tempID;
	      tempID = NULL;
	    }
	    
	    // allocating string to contain the attribute
	    StrCpy( &tempID , 
		    XMLtag->getAttributeStringValue( "xlink:href" ) );
	    delete XMLtag;
	    XMLtag = NULL;
	    
	    // opens the file if the depth has not
	    // reached the maximal value
	    if( *fileDepth < DM_NB_MAX_EMBEDDED_XML_FILES - 1 ) {
	      // storing the last read character of the current file
	      lastChar[ (*fileDepth) ] = *p_c;

	      // opening the embedded file
	      printf( "Including %s\n" , tempID );
	      (*fileDepth)++;
	      fileXML[ (*fileDepth) ] = fopen( tempID , "rb" );
	      
	      if( !fileXML[ (*fileDepth) ] ) {
		sprintf( ErrorStr , "Error: scene file [%s] not found!" , tempID ); ReportError( ErrorStr ); throw 11;
	      }
	      
	      // deallocating memory
	      if( tempID ) {
		delete [] tempID;
		tempID = NULL;
	      }
	    
	      // reading the XML file
	      *p_c = ReadChar( fileDepth , NULL , NULL);
	      SpaceCommentsInclude( fileDepth , p_c , NULL , NULL );
	    }
	    else {
		sprintf( ErrorStr , "Error: excessive number of embedded file inclusion, limit is %d!" , DM_NB_MAX_EMBEDDED_XML_FILES ); ReportError( ErrorStr ); throw 11;
	    }
	  }
	  // it is not an inclusion tag, return
	  else {
	    if( *p_c == EOF && fileDepth && (*fileDepth) > 0 ) {
	      // end of an inclusion
	      (*fileDepth)--;
	      *p_c = lastChar[ (*fileDepth) ];
	    } 
	    return;
	  }
	}
	// it is not an inclusion tag and we are reading a string, return
	else {
	  *p_c = charstring[ curCharPos - 1 ];
	  (*ind)--;
	  return;
	}
      }
    }
    // non < char and non space char
    else {
      if( *p_c == EOF && fileDepth && (*fileDepth) > 0 ) {
	// end of an inclusion
	(*fileDepth)--;
	*p_c = lastChar[ (*fileDepth) ];
      } 
      return;
    }
  }
}

int     ReadStringWithTermination( int *fileDepth , int *p_c , 
				   char **p_targetstring ,
				   int p_frontierRight ,
				   int p_withSpaceCommentsInclude , 
				   char *charstring , int *ind ) {
  int             i = 0;
  int             str_size = 0;

  // deallocates possibly preallocated memory
  if( *p_targetstring ) {
    delete [] *p_targetstring;
    *p_targetstring = NULL;
  }

  // gets the size of the string to allocate the necessary space
  if( fileDepth ) {
    int charIni = *p_c;
    fpos_t  pos;
    if( fgetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not retrieved!" ); ReportError( ErrorStr ); return false;
    }
    i = 0;
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	i++;
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    if( fsetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not set!" ); ReportError( ErrorStr ); return false;
    }
  }
  else {
    int charIni = *p_c;
    int  pos = *ind;
    i = 0;
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	i++;
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    *ind = pos;
  }

  // reads the size from the file or from the string
  i = 0;
  while( *p_c != p_frontierRight && i < str_size - 1  && *p_c != EOF ) {
    if( *p_c == '\\' ) {
      *p_c = ReadChar(fileDepth,charstring,ind);
      // backshlashed opening parenthesis are left
      // escaped so that they are not interpreted as
      // interpretable expression in posterior string processing
      if( *p_c == '(' ) {
	(*p_targetstring)[i++] = '\\';
      }
    }
    (*p_targetstring)[i++] = *p_c;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  if( i == str_size - 1) {
    sprintf( ErrorStr , "Error: max string size exceeded ReadStringWithTermination (max %d chars)!" , str_size ); ReportError( ErrorStr ); throw(100);
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
  }
  (*p_targetstring)[i] = '\0';
    
  if( p_withSpaceCommentsInclude ) {
    SpaceCommentsInclude( fileDepth ,p_c,charstring,ind );
  }
  
  return true;
}

int     ReadStringWithFrontiers( int *fileDepth , int *p_c , 
				 char **p_targetstring ,
				 int p_frontierLeft , 
				 int p_frontierRight ,
				 int p_withSpaceCommentsInclude , 
				 char *charstring , int *ind ) {
  int             i = 0;
  int             str_size = 0;

  if( !ExpectsChar( fileDepth , p_frontierLeft , p_c , false ,
		    charstring , ind ) ) {
    return false;
  }

  // deallocates possibly preallocated memory
  if( *p_targetstring ) {
    delete [] *p_targetstring;
    *p_targetstring = NULL;
  }

  // gets the size of the string to allocate the necessary space
  if( fileDepth ) {
    int charIni = *p_c;
    fpos_t  pos;
    if( fgetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not retrieved!" ); ReportError( ErrorStr ); return false;
    }
    i = 0;
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	// backshlashed opening parenthesis are left
	// escaped so that they are not interpreted as
	// interpretable expression in posterior string processing
	if( *p_c == '(' ) {
	  i++;
	}
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    if( fsetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not set!" ); ReportError( ErrorStr ); return false;
    }
  }
  else {
    int charIni = *p_c;
    int  pos = *ind;
    i = 0;
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	// backshlashed opening parenthesis are left
	// escaped so that they are not interpreted as
	// interpretable expression in posterior string processing
	if( *p_c == '(' ) {
	  i++;
	}
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    *ind = pos;
  }

  // reads the size from the file or from the string
  i = 0;
  while( *p_c != p_frontierRight && i < str_size - 1  && *p_c != EOF ) {
    if( *p_c == '\\' ) {
      *p_c = ReadChar(fileDepth,charstring,ind);
      // backshlashed opening parenthesis are left
      // escaped so that they are not interpreted as
      // interpretable expression in posterior string processing
      if( *p_c == '(' ) {
	(*p_targetstring)[i++] = '\\';
      }
    }
    (*p_targetstring)[i++] = *p_c;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  if( i == str_size - 1) {
    sprintf( ErrorStr , "Error: max string size exceeded ReadStringWithFrontiers (max %d chars)!" , str_size ); ReportError( ErrorStr ); throw(100);
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
  }
  (*p_targetstring)[i] = '\0';
    
  if( !ExpectsChar( fileDepth , p_frontierRight , 
		    p_c , p_withSpaceCommentsInclude , charstring , ind ) ) {
    return false;
  }
    
  return true;
}

int     ReadBalancedStringWithFrontiers( int *fileDepth , 
                                 int *p_c , char *p_targetstring ,
				 int p_size , int p_frontierLeft , 
				 int p_frontierRight ,
				 bool p_withSpaceCommentsInclude , 
				 bool p_keepFrontiers , 
				 char *charstring , int *ind ) {
  int             i = 0;

  if( !ExpectsChar( fileDepth , p_frontierLeft , p_c , false ,
		    charstring , ind ) ) {
    return false;
  }
  if( p_keepFrontiers ) {
    p_targetstring[i++] = p_frontierLeft;
  }

  int level = 0;
  while( !(*p_c == p_frontierRight && level == 0)
	 && i < p_size - 1  && *p_c != EOF ) {

    if( *p_c == '(' || *p_c == '{' ) {
      level++;
    }
    if( *p_c == '}' || *p_c == ')' ) {
      level--;
    }
    if( *p_c == '\\' ) {
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    p_targetstring[i++] = *p_c;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }

  // reads the end of the string if the container is too small
  if( i == p_size - 1) {
    while( *p_c != p_frontierRight && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
  }

  // expects the end of the string
  if( !ExpectsChar( fileDepth , p_frontierRight , 
		    p_c , p_withSpaceCommentsInclude , charstring , ind ) ) {
    return false;
  }
  if( p_keepFrontiers ) {
    p_targetstring[i++] = p_frontierRight;
  }
    
  // terminates the container
  p_targetstring[i] = '\0';
    
  return true;
}

int     ReadTableStringWithFrontiers( int *fileDepth , 
				      int *p_c , char ***p_tablecharstring ,
				      int size ,
				      int p_size , int p_frontierLeft , 
				      int p_frontierRight ,
				      int p_withSpaceCommentsInclude , 
				      char *charstring , int *ind ) {
  char  *chAux = NULL;

  // if( charstring )  printf( "input table string [%s]\n" , charstring ); 

  if( !ExpectsChar( fileDepth , '(' , p_c , true ,
		    charstring , ind ) ) {
    return false;
  }
  for( int indString = 0 ; indString < size - 1 ; indString ++ ) {
    if( !ReadStringWithFrontiers( fileDepth , p_c , 
				  &chAux ,
				  p_frontierLeft , 
				  p_frontierRight ,
				  p_withSpaceCommentsInclude , 
				  charstring , ind ) ) {
      if( chAux ) {
	delete [] chAux;
	chAux = NULL;
      }
      return false;
    }
    StrCpy( (p_tablecharstring[ indString ]) , chAux );

    if( !ExpectsChar( fileDepth , ',' , p_c , true ,
		      charstring , ind ) ) {
      if( chAux ) {
	delete [] chAux;
	chAux = NULL;
      }
      return false;
    }
  }
  if( size > 0 ) {
    if( !ReadStringWithFrontiers( fileDepth , p_c , 
				  &chAux ,
				  p_frontierLeft , 
				  p_frontierRight ,
				  p_withSpaceCommentsInclude , 
				  charstring , ind ) ) {
      if( chAux ) {
	delete [] chAux;
	chAux = NULL;
      }
      return false;
    }
    StrCpy( (p_tablecharstring[ size - 1 ]) , chAux );
  }
  
  if( chAux ) {
    delete [] chAux;
    chAux = NULL;
  }
  
  if( !ExpectsChar( fileDepth , ')' , p_c , true ,
		    charstring , ind ) ) {
    return false;
  }
  
  return true;
}

void InitString( char **target ) {
  if (*target) {
    delete [] *target;
    *target = NULL;
  }

  *target = new char[ 1 ];
  **target = 0;
}  

void InitString( char **target , int str_l ) {
  if (*target) {
    delete [] *target;
    *target = NULL;
  }

  *target = new char[ str_l ];
  memset( *target , 0 , str_l * sizeof( char ) );
}  

void StrCpy( char **target , const char *source ) {
  if( !source ) {
    return;
  }
  if( *target && (strlen( *target ) < strlen( source ) ) ) {
    delete [] *target;
    *target = NULL;
  }
  int str_l = strlen( source ) + 1;
  if( !(*target) ) {
    *target = new char[ str_l ];
  }
  memcpy( *target , source , str_l * sizeof( char ) );
}  

void StrCpyTest( char *target , const char *source ) {
  if( !source ) {
    return;
  }
  if( target && (strlen( target ) < strlen( source ) ) ) {
    delete [] target;
    target = NULL;
  }
  int str_l = strlen( source ) + 1;
  if( !(target) ) {
    target = new char[ str_l ];
  }
  memcpy( target , source , str_l * sizeof( char ) );
}  

void StrNCpy( char **target , const char *source , unsigned int length ) {
  if( !source ) {
    return;
  }
  if( *target && (strlen( *target ) < length ) ) {
    delete [] *target;
    *target = NULL;
  }
  int str_l = length + 1;
  if( !(*target) ) {
    *target = new char[ str_l ];
  }
  memcpy( *target , source , str_l * sizeof( char ) );
}  

void StrCat( char **source , const char *addition , unsigned int * sourceLength ) {
  if( !addition ) {
    return;
  }
  if( *source && (strlen( *source ) + strlen( addition ) >= *sourceLength ) ) {
    // case of length 0
    if( *sourceLength == 0 ) {
      *sourceLength = 1;
    }
    // increase string length as much as necessary
    while( *sourceLength <= strlen( *source ) + strlen( addition ) ) {
      *sourceLength *= 2;
    }
    // copies source + addition
    char *aux_source = new char[ *sourceLength ];
    strcpy( aux_source , *source );
    strcat( aux_source , addition );
    // frees the old string
    delete [] *source;
    *source = aux_source;
  }
  else if( *source ) {
    strcat( *source , addition );
  }
  else { // !(*source) 
    // case of length 0
    if( *sourceLength <= 0 ) {
      *sourceLength = 1;
    }
    // increase string length as much as necessary
    while( *sourceLength <= strlen( addition ) ) {
      *sourceLength *= 2;
    }
    *source = new char[ *sourceLength ];
    strcpy( *source , addition );
  }
}  

int readAttributeValue( int *fileDepth , int *p_c , char **valueString, 
			char *charstring , int *ind ) {
  if( *p_c == '"' ) {
    if( !ReadStringWithFrontiers( fileDepth , p_c , valueString ,
				  '"' , '"' , true , 
				  charstring,ind ) ) {
      return false;
    }
  }
  else {
    // dot is added in order to read float values
    ReadIDWithCarSpec( fileDepth , p_c ,valueString ,
		       ":_-." , true , 
		       charstring,ind );
  }
  return true;
}

dm_XMLTag::dm_XMLTag() {
  initXMLTag();
}

void dm_XMLTag::initXMLTag( void ) {
  tag = NULL; 
  attributes = NULL;
  values = NULL; 
  nbAttributes = 0;
  tagType = EmptyTag;
}

void dm_XMLTag::deallocateXMLTag() {
  if (tag) {
    delete [] tag; 
    tag = NULL;
  }

  for( int i = 0 ; i < nbAttributes ; i++ ) {
    if( attributes[ i ] ) {
      delete [] attributes[ i ];
      attributes[i] = NULL;
    }
    
    if( values[ i ] ) {
      delete [] values[ i ];
      values[i] = NULL;
    }
  }
  
  delete [] attributes;
  attributes = NULL;

  delete [] values; 
  values = NULL;

  initXMLTag();
}

dm_XMLTag::~dm_XMLTag() {
  deallocateXMLTag(); 
}


bool  dm_XMLTag::ParseTagXML( int *fileDepth , int *p_c ,
			      int p_withSpaceCommentsInclude  , 
			      char *charstring , int *ind ) {
  /*! \name Group XML_tag_buffer_attr_value
   *  \brief XML tag buffer: temporary storage of attr and value
   */
  // \{
  char *temporaryAttribute = NULL;
  char *temporaryValue = NULL;
  char **temporaryAttributeArray = NULL;
  char **temporaryValueArray = NULL;
  // \}
  
  // XML tag buffer: temporary storage of attr and value
  temporaryAttributeArray = new char*[XMLNbMaxAttributes];
  memset( temporaryAttributeArray , 0 , XMLNbMaxAttributes * sizeof (char*) );
  temporaryValueArray = new char*[XMLNbMaxAttributes];
  memset( temporaryValueArray , 0 , XMLNbMaxAttributes * sizeof (char*) );
  
  // default type: opening tag
  tagType = OpeningTag;
  
  // initialisation of attribute strings and values
  if( nbAttributes != 0 ) {
    deallocateXMLTag();
  }
  
  if( !ExpectsChar( fileDepth , '<' , p_c , true , charstring , ind ) ) {
    sprintf( ErrorStr , "Error: Expecting XML tag (check tag balance)!" ); ReportError( ErrorStr ); throw 68;
  }

  // closing tag
  if( *p_c == '/' ) {
    ExpectsChar( fileDepth , '/' , p_c , true , charstring,ind );

    // reads the tag in a temporary string and
    // allocates tag and copies 
    ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
		       "?:_-" , true , charstring,ind );
    StrCpy( &tag , temporaryAttribute );

    tagType = ClosingTag;
    bool valreturn = true;
    if( !ExpectsChar( fileDepth , '>' , 
		      p_c , p_withSpaceCommentsInclude , charstring , ind ) ) {
      valreturn = false;
    }

    // temporary memory deallocation
    if (temporaryAttribute) {
      delete [] temporaryAttribute;
      temporaryAttribute = NULL;
    }
    if (temporaryValue) {
      delete [] temporaryValue;
      temporaryValue = NULL;
    }
    if (temporaryAttributeArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryAttributeArray[ind]) {
	  delete [] temporaryAttributeArray[ind];
	}
	temporaryAttributeArray[ind] = NULL;
      }
      delete [] temporaryAttributeArray;
      temporaryAttributeArray = NULL;
    }
    if (temporaryValueArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryValueArray[ind]) {
	  delete [] temporaryValueArray[ind];
	}
	temporaryValueArray[ind] = NULL;
      }
      delete [] temporaryValueArray;
      temporaryValueArray = NULL;
    }
    return valreturn;
  }

  // opening or empty tag
  ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
		     "!?:_-" , true , charstring,ind );
  StrCpy( &tag , temporaryAttribute );
  
  // very long tags: lists of numerical values
  // that are not processed as regular tags
  // for optimisation purposes
  // processing is made locally by the calling function
  if( strcmp( tag , "vertexCoordinate" ) == 0 
      || strcmp( tag , "normalCoordinate" ) == 0 
      || strcmp( tag , "textureCoordinate" ) == 0 
      || strcmp( tag , "vertexIndex" ) == 0 
      || strcmp( tag , "normalIndex" ) == 0 
      || strcmp( tag , "textureCoordinateIndex" ) == 0 ) {
    // temporary memory deallocation
    if (temporaryAttribute) {
      delete [] temporaryAttribute;
      temporaryAttribute = NULL;
    }
    if (temporaryValue) {
      delete [] temporaryValue;
      temporaryValue = NULL;
    }
    if (temporaryAttributeArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryAttributeArray[ind]) {
	  delete [] temporaryAttributeArray[ind];
	}
	temporaryAttributeArray[ind] = NULL;
      }
      delete [] temporaryAttributeArray;
      temporaryAttributeArray = NULL;
    }
    if (temporaryValueArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryValueArray[ind]) {
	  delete [] temporaryValueArray[ind];
	}
	temporaryValueArray[ind] = NULL;
      }
      delete [] temporaryValueArray;
      temporaryValueArray = NULL;
    }
    // printf( "exceptional tag %s\n" , tag );
    return true;
  }

  // special XML tags
  if( *tag == '?' ) {
    bool   EndOfTag = false;
    // looks for "?>"
    while( !EndOfTag && *p_c != EOF ) {
      if( *p_c == '?' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
	if( *p_c == '>' ) {
	  EndOfTag = true;
	}
      }
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    if( p_withSpaceCommentsInclude ) {
      SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
    }

    // temporary memory deallocation
    if (temporaryAttribute) {
      delete [] temporaryAttribute;
      temporaryAttribute = NULL;
    }
    if (temporaryValue) {
      delete [] temporaryValue;
      temporaryValue = NULL;
    }
    if (temporaryAttributeArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryAttributeArray[ind]) {
	  delete [] temporaryAttributeArray[ind];
	}
	temporaryAttributeArray[ind] = NULL;
      }
      delete [] temporaryAttributeArray;
      temporaryAttributeArray = NULL;
    }
    if (temporaryValueArray) {
      for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	if (temporaryValueArray[ind]) {
	  delete [] temporaryValueArray[ind];
	}
	temporaryValueArray[ind] = NULL;
      }
      delete [] temporaryValueArray;
      temporaryValueArray = NULL;
    }
    return true;
  } 
  
  // regular VirChor XML tags
  while( *p_c != '>' && *p_c != '/' && nbAttributes < XMLNbMaxAttributes  
	 && *p_c != EOF ) {
    // reads the attribute in a temporary string and
    // allocates final string and copies its value in temporary array
    ReadIDWithCarSpec( fileDepth , p_c , &temporaryAttribute , 
		       ":_-" , true , charstring,ind );
    StrCpy( &(temporaryAttributeArray[nbAttributes]) , temporaryAttribute );

    // reads the value in a temporary string and
    // allocates final string and copies its value in temporary array
    if( !ExpectsChar( fileDepth , '=' , p_c , true , charstring,ind ) 
	|| !readAttributeValue( fileDepth , p_c , &temporaryValue , 
				charstring,ind ) ) {
      // temporary memory deallocation
      if (temporaryAttribute) {
	delete [] temporaryAttribute;
	temporaryAttribute = NULL;
      }
      if (temporaryValue) {
	delete [] temporaryValue;
	temporaryValue = NULL;
      }
      if (temporaryAttributeArray) {
	for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	  if (temporaryAttributeArray[ind]) {
	    delete [] temporaryAttributeArray[ind];
	  }
	  temporaryAttributeArray[ind] = NULL;
	}
	delete [] temporaryAttributeArray;
	temporaryAttributeArray = NULL;
      }
      if (temporaryValueArray) {
	for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
	  if (temporaryValueArray[ind]) {
	    delete [] temporaryValueArray[ind];
	  }
	  temporaryValueArray[ind] = NULL;
	}
	delete [] temporaryValueArray;
	temporaryValueArray = NULL;
      }

      return false;
    }
    StrCpy( &(temporaryValueArray[nbAttributes]) , temporaryValue );

    (nbAttributes)++;
  }

  // empty tag
  if( *p_c == '/' ) {
    ExpectsChar( fileDepth , '/' , 
		 p_c , p_withSpaceCommentsInclude , charstring,ind );
    tagType = EmptyTag;
  }
  
  // tag closing
  if( *p_c != '>' ) {
    sprintf( ErrorStr , "Error: incorrect XML element syntax or excessive number of attribute/value pairs!" ); ReportError( ErrorStr ) ; throw 298;
  }

  ExpectsChar( fileDepth , '>' , 
	       p_c , p_withSpaceCommentsInclude , charstring,ind );

  // copies temporary pointer arrays in the final tag
  attributes = new char*[nbAttributes];
  values = new char*[nbAttributes];
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    attributes[ind] = NULL;
    values[ind] = NULL;
    StrCpy( &(attributes[ind]) , temporaryAttributeArray[ind]);
    StrCpy( &(values[ind]) , temporaryValueArray[ind]);
  }

  // temporary memory deallocation
  if (temporaryAttribute) {
    delete [] temporaryAttribute;
    temporaryAttribute = NULL;
  }
  if (temporaryValue) {
    delete [] temporaryValue;
    temporaryValue = NULL;
  }
  if (temporaryAttributeArray) {
    for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
      if (temporaryAttributeArray[ind]) {
	delete [] temporaryAttributeArray[ind];
      }
      temporaryAttributeArray[ind] = NULL;
    }
    delete [] temporaryAttributeArray;
    temporaryAttributeArray = NULL;
  }
  if (temporaryValueArray) {
    for (int ind = 0; ind < XMLNbMaxAttributes; ind ++) {
      if (temporaryValueArray[ind]) {
	delete [] temporaryValueArray[ind];
      }
      temporaryValueArray[ind] = NULL;
    }
    delete [] temporaryValueArray;
    temporaryValueArray = NULL;
  }

  return true;
}

void dm_XMLTag::substituteParametersXML( void ) {
  // parameter values in the action strings
  for( int indAux = 0 ; indAux < nbAttributes ; indAux++ ) {
    // must take into consideration the possible length increase: 
    // substitution can make strings longer
    SubstituteParameters( &(values[indAux]) );
  }
}

void dm_XMLTag::copyXMLTag( dm_XMLTag *source ) {
  if( nbAttributes ) {
    deallocateXMLTag();
  }
  nbAttributes = source->nbAttributes;

  // must allocate before copying
  attributes = new char*[nbAttributes];
  memset( attributes , 0 , nbAttributes * sizeof (char*) );
  values = new char*[nbAttributes];
  memset( values , 0 , nbAttributes * sizeof (char*) );

  StrCpy( &(tag) , source->tag );

  // parameter values in the action strings
  for( int indAux = 0 ; indAux < source->nbAttributes ; indAux++ ) {
	  attributes[indAux] = new char[strlen(source->attributes[indAux]) + 2];
	  values[indAux] = new char[strlen(source->values[indAux]) + 2];
	  strcpy(attributes[indAux], source->attributes[indAux]);
	  strcpy(values[indAux], source->values[indAux]);
	 
//    StrCpy( &(attributes[indAux]) , source->attributes[indAux] );
//    StrCpy( &(values[indAux]) , source->values[indAux] );
  }	
}

void dm_XMLTag::copyAndSubstitueExpressions( dm_XMLTag *source ) {
  if( nbAttributes ) {
    deallocateXMLTag();
  }
  nbAttributes = source->nbAttributes;

  // must allocate before copying
  attributes = new char*[nbAttributes];
  memset( attributes , 0 , nbAttributes * sizeof (char*) );
  values = new char*[nbAttributes];
  memset( values , 0 , nbAttributes * sizeof (char*) );

  StrCpy( &tag , source->tag );

  // parameter values in the action strings
  for( int indAux = 0 ; indAux < source->nbAttributes ; indAux++ ) {
    StrCpy( &(attributes[indAux]) , source->attributes[indAux] );
    StrCpyAndSubst( &(values[indAux])  , source->values[indAux] );
  }	
}

void dm_XMLTag::copyAndSubstitueExpressionsbutNotTable( dm_XMLTag *source  ,
							char **argList , int argCount ) {
  if( nbAttributes ) {
    deallocateXMLTag();
  }
  nbAttributes = source->nbAttributes;

  // must allocate before copying
  attributes = new char*[nbAttributes];
  memset( attributes , 0 , nbAttributes * sizeof (char*) );
  values = new char*[nbAttributes];
  memset( values , 0 , nbAttributes * sizeof (char*) );

  StrCpy( &tag , source->tag );
  // parameter values in the action strings
  for( int indAux = 0 ; indAux < source->nbAttributes ; indAux++ ) {
    StrCpy( &(attributes[indAux]) , source->attributes[indAux] );

    // the value of the "value" or "matrix" or "m__" matrix column
    // attribute should not be pre-substituted
    // because it can be a table value and should not be
    // treated as an expression. Substitution will be made by the
    // setParameters command that follows
    bool oneTableFound = false;
    // printf("Arg count %d\n" , argCount );
    for( int ind = 0 ; ind < argCount ; ind++ ) {
      // non substitued table value
      // printf("Attr %s\n" , argList[ ind ] );
      if( strcmp( attributes[indAux] , argList[ ind ] ) == 0 ) {
	StrCpy( &(values[indAux]) , source->values[indAux] );
	oneTableFound = true;
	break;
      }
    }
    // substituted scalar value
    if( !oneTableFound ) {
	StrCpyAndSubst( &(values[indAux]) , source->values[indAux] );
    }
  }	
}

void dm_XMLTag::print( FILE *file , int depth ) {
  indent( file , depth );
  fprintf( file , "<%s " , tag );
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    fprintf( file , "%s=\"%s\" " , attributes[ind] , values[ind] );
  }
  fprintf( file , " />\n" );
}

char *dm_XMLTag::getAttributeStringValue( const char *attributeName ) {
  char *returnValue = NULL;

  //printf( "nbAttributes %d\n" , nbAttributes );
  //printf( "attribute name %s\n" , attributeName );
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    //printf( "attr %d [%s] [%s]\n" , ind , attributes[ ind ]  , values[ ind ] );
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      returnValue = SubstituteParameters( &(values[ ind ]) );
      break;
    }
  }

  if( returnValue ) {
    return returnValue;
  }

  sprintf( ErrorStr , "Error: string attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 4;
  return NULL;
}
  
int dm_XMLTag::getAttributeIntValue( const char *attributeName , bool atRunTime ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      return stringToIntValue( values[ ind ] , atRunTime );
    }
  }
  sprintf( ErrorStr , "Error: integer attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 4;
  return -1;
}
  

double dm_XMLTag::getAttributeDoubleValue( const char *attributeName ,
					   bool atRunTime ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      return (double)stringToDoubleValue( values[ ind ] , atRunTime );
    }
  }
  sprintf( ErrorStr , "Error: double attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 36;
  return -1;
}
  

void dm_XMLTag::getAttributeTableStringValue( char  ***scalars , int size ,
					      const char *attributeName , bool atRunTime ) {
  //printf( "nbAttributes %d\n" , nbAttributes );
  //printf( "attribute name %s\n" , attributeName );
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    //printf( "attr %d [%s] [%s]\n" , ind , attributes[ ind ]  , values[ ind ] );
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      // printf( "attr [%s]\n" , attributeName );
      int curChar = values[ ind ][0];
      int indChar = 1;
      char  chAux[XMLAttrValLength];
      ReadTableStringWithFrontiers( NULL , 
				    &curChar , scalars ,
				    size ,
				    XMLAttrValLength , '\'' , '\'' ,
				    true , 
				    values[ ind ] , &indChar );

      for( int indAux = 0 ; indAux < size ; indAux++ ) {
	// printf( "scalars [%d] [%s]\n" , indAux , *(scalars[ indAux ]) );
	//int indchAux = 1;
	//int ch = **(scalars[ indAux ]);
	*(chAux) = 0;
	//SubstituteParameters( &(chAux) );
// 	ReadExpression( NULL , &ch , true , *(scalars[ indAux ]) ,  
// 			&indchAux , atRunTime , chAux );
	strcpy( chAux , *(scalars[ indAux ]) );
	// printf( "scalars [%d] [%s]\n" , indAux , *(scalars[ indAux ]) );
	StrCpyAndSubst( scalars[ indAux ] , chAux );
	// printf( "scalars [%d] [%s]\n" , indAux , *(scalars[ indAux ]) );
      }
      return;
    }
  }

  sprintf( ErrorStr , "Error: string attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 4;
}

void dm_XMLTag::getAttributeTableIntValue( int  *scalars , int size ,
					   const char *attributeName , 
					   bool atRunTime ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      stringToTableIntValue( scalars , size , 
			     values[ ind ] , atRunTime );
      return;
    }
  }
  sprintf( ErrorStr , "Error: integer attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 4;
}
void dm_XMLTag::getAttributeTableBoolValue( bool  *scalars , int size ,
					    const char *attributeName , 
					    bool atRunTime ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      stringToTableBoolValue( scalars , size , 
			      values[ ind ] , atRunTime );
      return;
    }
  }
  sprintf( ErrorStr , "Error: integer attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 4;
}
void dm_XMLTag::getAttributeTableDoubleValue( double *scalars , int size ,
					      const char *attributeName , 
					      bool atRunTime ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      // printf( "getAttributeTableDoubleValue Expression %s\n" , values[ ind ] );
      stringToTableDoubleValue( scalars , size , 
				values[ ind ] , atRunTime );
      return;
    }
  }
  sprintf( ErrorStr , "Error: double attribute [%s] of tag [%s] not provided!" , attributeName , tag ); ReportError( ErrorStr ); throw 36;
}

int dm_XMLTag::hasAttribute( const char *attributeName ) {
  for( int ind = 0 ; ind < nbAttributes ; ind++ ) {
    if( strcmp( attributes[ ind ] , attributeName ) == 0 ) {
      return true;
    }
  }
  return false;
}

numericalMode dm_XMLTag::getModeValue( void ) {
  char *mode = NULL;
  if( hasAttribute( "mode" ) ) {
    mode = getAttributeStringValue( "mode" );
  }
  
  if( mode ){
    if( strcmp( mode , "random" ) == 0 ) {
      return DM_RANDOM;
    }
    else{
      if( strcmp( mode , "decimal" ) != 0 ){
	sprintf( ErrorStr , "Error: point index out of bounds!"); ReportError( ErrorStr ); throw 112;
      }
    }
  }
  return DM_DECIMAL;
}

void dm_XMLTag::BooleanFieldValue ( const char *attributeName , bool *returnValue ) {
  if( hasAttribute( attributeName ) ) {
    char *isdisplayed_str = getAttributeStringValue( attributeName );
    if( strcmp( isdisplayed_str , "true" ) == 0 ) {
      *returnValue = true;
      return;
    }
    if( strcmp( isdisplayed_str , "false" ) == 0 ) {
      *returnValue = false;
      return;
    }
    sprintf( ErrorStr , "Error: unknown boolean [%s]!" , isdisplayed_str ); ReportError( ErrorStr ); throw 205;
    // printf(  "boolean [%s %d]\n" , field , *returnValue  );
  }
}

void     ReadIDWithCarSpec( int *fileDepth , int *p_c , 
			    char **p_targetstring ,
			    const char *p_lisCarSpec ,
			    int p_withSpaceCommentsInclude , 
			    char *charstring , int *ind ) {
  int             i = 0;
  int             str_size = 0;

  // deallocates possibly preallocated memory
  if( *p_targetstring ) {
    delete [] *p_targetstring;
    *p_targetstring = NULL;
  }

  // gets the size of the string to allocate the necessary space
  if( fileDepth ) {
    int charIni = *p_c;
    fpos_t  pos;
    if( fgetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not retrieved!" ); ReportError( ErrorStr ); return;
    }
    i = 0;
    while( ( _Alphanum(*p_c)  || strchr( p_lisCarSpec , *p_c )
	     || *p_c == '\\' ) &&  *p_c != '\'' && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    if( fsetpos( fileXML[ *fileDepth ] , &pos ) ) {
      sprintf( ErrorStr , "Error: file position not set!" ); ReportError( ErrorStr ); return;
    }
  }
  else {
    int charIni = *p_c;
    int  pos = *ind;
    i = 0;
    while( ( _Alphanum(*p_c)  || strchr( p_lisCarSpec , *p_c )
	     || *p_c == '\\' ) &&  *p_c != '\'' && *p_c != EOF ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      i++;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
    *p_targetstring = new char[ i + 2 ];
    str_size = i + 2;
    *p_c = charIni;
    *ind = pos;
  }

  // reads the size from the file or from the string
  i = 0;
  if( _Alphanum(*p_c) || strchr( p_lisCarSpec , *p_c )
      || *p_c == '\\' ) {
    while( ( _Alphanum(*p_c)  || strchr( p_lisCarSpec , *p_c )
	     || *p_c == '\\' )  &&  *p_c != '\'' && i < str_size - 1 ) {
      if( *p_c == '\\' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
      (*p_targetstring)[i++] = *p_c;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }

    if( i == str_size - 1) {
      sprintf( ErrorStr , "Error: max string size exceeded ReadIDWithCarSpec (max %d chars)!" , str_size ); ReportError( ErrorStr ); throw(100);
      while( ( _Alphanum(*p_c) || strchr( p_lisCarSpec , *p_c )
	       || *p_c == '\\' )  &&  *p_c != '\'' ) {
	*p_c = ReadChar(fileDepth,charstring,ind);
      }
    }
    (*p_targetstring)[i] = '\0';

    if( p_withSpaceCommentsInclude ) {
      SpaceCommentsInclude( fileDepth , p_c , charstring , ind );
    }
  }
  else {
    (*p_targetstring)[0] = '\0';
  }
}

long     ReadInteger( int *fileDepth , int *p_c  ,
		      int p_withSpaceCommentsInclude , 
		      char *charstring , int *ind ) {
  long            i = 0L;
  short           neg = 0;

  if( *p_c == '-') {
    neg = 1;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  else if( *p_c == '+') {
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  if (_Num(*p_c)) {
    while ( _Num(*p_c) ) {
      i = i*10L + (long)(*p_c - '0');
      *p_c = ReadChar(fileDepth,charstring,ind);
    }

    if( p_withSpaceCommentsInclude ) {
      SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
    }
    if( neg == 1)
      i = -i;
    return i;
  }
  else {
    return 0L;
  }
}


#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
  SYSTEMTIME system_time;
  FILETIME file_time;
  ULARGE_INTEGER ularge;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time, &file_time);
  ularge.LowPart = file_time.dwLowDateTime;
  ularge.HighPart = file_time.dwHighDateTime;

  tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
  tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
  /* 0 indicates that the call succeeded. */
  return 0;
}
#endif

double RealTime( void ) {
  struct timeval time;
  gettimeofday(&time, 0);
  double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
  return realtime - InitialRealTime;
}

double Addition(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return *in_pdArg1 + *in_pdArg2;
}

double Substraction(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return *in_pdArg1 - *in_pdArg2;
}

double Multiplication(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return *in_pdArg1 * *in_pdArg2;
}

double Division(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  if( *in_pdArg2 != 0 ) {
    return *in_pdArg1 / *in_pdArg2;
  }
  else if( *in_pdArg1 > 0 ) {
    return MAXFLOAT;
  }
  else {
    return -MAXFLOAT;
  }
}

double Mod(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return (int)(*in_pdArg1) % (int)(*in_pdArg2);
}

double Cosine(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return cos(*in_pdArg1 * DegtoRad);
}

double Sinus(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return sin(*in_pdArg1 * DegtoRad);
}

double Tangent(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return tan(*in_pdArg1 * DegtoRad);
}

double Acosine(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return RadtoDeg * acos(*in_pdArg1);
}

double Asinus(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return RadtoDeg * asin(*in_pdArg1);
}

double Atangent(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return RadtoDeg * atan(*in_pdArg1);
}

double Logarithm(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return log(*in_pdArg1)/log(*in_pdArg2);
}

double Max(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
//   printf( "Max %.2f %.2f %.2f\n" , *in_pdArg1 , *in_pdArg2 , ((*in_pdArg1<*in_pdArg2)?*in_pdArg2:*in_pdArg1)  );
  return ((*in_pdArg1<*in_pdArg2)?*in_pdArg2:*in_pdArg1);
}

double Min(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
//   printf( "Min %.2f %.2f %.2f\n" , *in_pdArg1 , *in_pdArg2 , ((*in_pdArg2<*in_pdArg1)?*in_pdArg2:*in_pdArg1)  );
  return ((*in_pdArg2<*in_pdArg1)?*in_pdArg2:*in_pdArg1);
}

double Power(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return pow(*in_pdArg1,*in_pdArg2);
}

double Random(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return generateRandomFloat((float)*in_pdArg1,(float)*in_pdArg2);
}

double Round(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
/*  double resul =  floor(*in_pdArg1 + 0.5);
  if( resul == (double)(int)resul ) {
    resul -= 1;
  }
  return resul;*/	
  return my_round(*in_pdArg1);
}

double Floor(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return  floor(*in_pdArg1);
}

double Sqrt(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return sqrt(*in_pdArg1);
}

double Abs(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return fabs(*in_pdArg1);
}

double Sign(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return (*in_pdArg1>0?1.0:(*in_pdArg1<0?-1.0:0.0));
}

double Now(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return CurrentClockTime;
}

double IsFalse(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return 0.0;
}

double IsTrue(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return 1.0;
}

double CurrentFrame(double * in_pdArg1,double* in_pdArg2 = NULL,double* in_pdDummy = NULL) 
{
  return (double)FrameNo;
}


void ScalarMatOperation( std::vector <double> * valStack ,  
			 std::vector <int> * sizStack ,
			 int oper ) // predefinedOperatorsID
{
  float * val1 = NULL;
  float operand;

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  // printf( "Scalar/matrix or matrix/scalar operation (%d*%d)!\n" , size2 , size1 );
  if( size1 != 1 && size2 != 1 ) {
    sprintf( ErrorStr , "Error: incomplete matrix for scalar/matrix or matrix/scalar operation (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  // size1 == 1 :  matrix OPER scalar
  if( size1 == 1 ) {
    val1 = new float[ size2 ];
    if( !val1 ) {
      sprintf( ErrorStr , "Error: float table allocation error (size %d)!" , size2 ); ReportError( ErrorStr ); throw 16;
    }

    operand = (float)(*valStack).back();
    (*valStack).pop_back();
    if( oper == DM_predefAdditionOperator ) {
      for( int ind = size2 - 1 ; ind >= 0 ; ind-- ) {
	val1[ ind ] = (float)(*valStack).back() + operand;
	(*valStack).pop_back();
      }
    }
    else if( oper == DM_predefSubstractionOperator ) {
      for( int ind = size2 - 1 ; ind >= 0 ; ind-- ) {
	val1[ ind ] = (float)(*valStack).back() - operand;
	(*valStack).pop_back();
      }
    }
    else if( oper == DM_predefMultiplicationOperator ) {
      for( int ind = size2 - 1 ; ind >= 0 ; ind-- ) {
	val1[ ind ] = (float)(*valStack).back() * operand;
	(*valStack).pop_back();
      }
    }
    else if( oper == DM_predefDivisionOperator && operand != 0) {
      for( int ind = size2 - 1 ; ind >= 0 ; ind-- ) {
	val1[ ind ] = (float)(*valStack).back() / operand;
	(*valStack).pop_back();
      }
    }

    for( int ind = 0 ; ind < size2 ; ind++ ) {  
      (*valStack).push_back(val1[ ind ]);
    }
    (*sizStack).push_back( size2 );
  }
  // size2 == 1 : scalar OPER matrix
  else {
    val1 = new float[ size1 ];
    if( !val1 ) {
      sprintf( ErrorStr , "Error: float table allocation error (size %d)!" , size1 ); ReportError( ErrorStr ); throw 16;
    }

    for( int ind = size1 - 1 ; ind >= 0 ; ind-- ) {
      val1[ ind ] = (float)(*valStack).back();
      (*valStack).pop_back();
    }
    operand = (float)(*valStack).back();
    (*valStack).pop_back();
    if( oper == DM_predefAdditionOperator ) {
      for( int ind = 0 ; ind < size1 ; ind++ ) {
	val1[ ind ] = operand + val1[ ind ];
      }
    }
    else if( oper == DM_predefSubstractionOperator ) {
      for( int ind = 0 ; ind < size1 ; ind++ ) {
	val1[ ind ] = operand - val1[ ind ];
      }
    }
    else if( oper == DM_predefMultiplicationOperator ) {
      for( int ind = 0 ; ind < size1 ; ind++ ) {
	val1[ ind ] = operand * val1[ ind ];
      }
    }
    else if( oper == DM_predefDivisionOperator && operand != 0) {
      for( int ind = 0 ; ind < size1 ; ind++ ) {
	if( val1[ ind ] != 0 ) {
	  val1[ ind ] = operand / val1[ ind ];
	}
      }
    }

    for( int ind = 0 ; ind < size1 ; ind++ ) {  
      (*valStack).push_back(val1[ ind ]);
    }
    (*sizStack).push_back( size1 );
  }
  delete [] val1;
  val1 = NULL;
}

void MatMatOperation( std::vector <double> * valStack ,  
		      std::vector <int> * sizStack ,
		      int oper ) // predefinedOperatorsID
{
  float * val1 = NULL;

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != size2 ) {
    sprintf( ErrorStr , "Error: matrices must have the same size for matrix/matrix addition or substraction (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  val1 = new float[ size1 ];
  if( !val1 ) {
    sprintf( ErrorStr , "Error: float table allocation error (size %d)!" , size1 ); ReportError( ErrorStr ); throw 16;
  }
  
  // second matrix
  for( int ind = size1 - 1 ; ind >= 0 ; ind-- ) {
    val1[ ind ] = (float)(*valStack).back();
    (*valStack).pop_back();
  }

  // first matrix
  if( oper == DM_predefAdditionOperator ) {
    for( int ind = size1 - 1 ; ind >= 0 ; ind-- ) {
      val1[ ind ] = (float)(*valStack).back() + val1[ ind ];
      (*valStack).pop_back();
    }
  }
  else if( oper == DM_predefSubstractionOperator ) {
    for( int ind = size1 - 1 ; ind >= 0 ; ind-- ) {
      val1[ ind ] = (float)(*valStack).back() - val1[ ind ];
      (*valStack).pop_back();
    }
  }

  for( int ind = 0 ; ind < size1 ; ind++ ) {  
    (*valStack).push_back(val1[ ind ]);
  }
  (*sizStack).push_back( size1 );
  delete [] val1;
  val1 = NULL;
}

void MatMatMultiplication( std::vector <double> * valStack ,  
			   std::vector <int> * sizStack ) 
{
  dm_matrix44 mat1;
  dm_matrix44 mat2;
  dm_matrix44 mat12;

  float * val1 = mat1.GetValue();
  float * val2 = mat2.GetValue();
  float * val12 = mat12.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 16 || size2 != 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix/matrix multiplication (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 32 ) {
	sprintf( ErrorStr , "Error: incomplete matrices for matrix/matrix multiplication (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
	return;    
  }
  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val2[ ind ]	= (float)(*valStack).back();
    (*valStack).pop_back();
  }
  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val1[ ind ]	= (float)(*valStack).back();
    (*valStack).pop_back();
  }
  mat1.product( mat12 , mat2 );
  for( int ind = 0 ; ind < 16 ; ind++ ) {  
    (*valStack).push_back(val12[ ind ]);
  }
  (*sizStack).push_back( 16 );
}

void HomogMatVecMultiplication( std::vector <double> * valStack ,  
				std::vector <int> * sizStack ) 
{
  dm_matrix44 mat;
  float vec[4];
  float matvec[4];
  float * val = mat.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 4 || size2 != 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix/vector multiplication (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 20 ) {
	sprintf( ErrorStr , "Error: incomplete matrices for matrix/vector multiplication (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
	return;    
  }
  // printf( "Size before multiplication %d\n" , (*valStack).size() );
  vec[3] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[2] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[1] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[0] = (float)(*valStack).back();
  (*valStack).pop_back();
  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val[ ind ]= (float)(*valStack).back();
    (*valStack).pop_back();
  }

  // printf( "Mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  // printf( "Vect %.2f %.2f %.2f %.2f\n" , vec[0] , vec[1] , vec[2] , vec[3]  );

  // matvec.product( vec , mat );
  matvec[0] = vec[0] * val[0] + vec[1] * val[4] + vec[2] * val[8] + vec[3] * val[12];
  matvec[1] = vec[0] * val[1] + vec[1] * val[5] + vec[2] * val[9] + vec[3] * val[13];
  matvec[2] = vec[0] * val[2] + vec[1] * val[6] + vec[2] * val[10] + vec[3] * val[14];
  matvec[3] = vec[0] * val[3] + vec[1] * val[7] + vec[2] * val[11] + vec[3] * val[15];

  (*valStack).push_back(matvec[0]);
  (*valStack).push_back(matvec[1]);
  (*valStack).push_back(matvec[2]);
  (*valStack).push_back(matvec[3]);
  // printf( "Size after multiplication %d\n" , (*valStack).size() );

  // printf( "Pro result %.2f %.2f %.2f %.2f\n" , matvec[0] , matvec[1] , matvec[2] , matvec[3] );
  (*sizStack).push_back( 4 );
}

void HomogVecMatMultiplication( std::vector <double> * valStack ,  
				std::vector <int> * sizStack ) 
{
  dm_matrix44 mat;
  float vec[4];
  float vecmat[4];
  float * val = mat.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 16 || size2 != 4 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for vector/matrix multiplication (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 20 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for vector/matrix multiplication (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }

  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val[ ind ]= (float)(*valStack).back();
    (*valStack).pop_back();
  }
  // printf( "Size before multiplication %d\n" , (*valStack).size() );
  vec[3] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[2] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[1] = (float)(*valStack).back();
  (*valStack).pop_back();
  vec[0] = (float)(*valStack).back();
  (*valStack).pop_back();

  // printf( "Mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  // printf( "Vect %.2f %.2f %.2f\n" , vec.x , vec.y , vec.z  );

  // vecmat.product( mat , vec );
  vecmat[0] = vec[0] * val[0] + vec[1] * val[1] + vec[2] * val[2] + vec[3] * val[3];
  vecmat[1] = vec[0] * val[4] + vec[1] * val[5] + vec[2] * val[6] + vec[3] * val[7];
  vecmat[2] = vec[0] * val[8] + vec[1] * val[9] + vec[2] * val[10] + vec[3] * val[11];
  vecmat[3] = vec[0] * val[12] + vec[1] * val[13] + vec[2] * val[14] + vec[3] * val[15];

  (*valStack).push_back(vecmat[0]);
  (*valStack).push_back(vecmat[1]);
  (*valStack).push_back(vecmat[2]);
  (*valStack).push_back(vecmat[3]);
  // printf( "Size after multiplication %d\n" , (*valStack).size() );

  // printf( "Pro result %.2f %.2f %.2f\n" , vecmat.x , vecmat.y , vecmat.z );
  (*sizStack).push_back( 4 );
}

void MatVecMultiplication( std::vector <double> * valStack ,  
			   std::vector <int> * sizStack ) 
{
  dm_matrix44 mat;
  dm_vector3 vec;
  dm_vector3 matvec;
  float * val = mat.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 3 || size2 != 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix/vector multiplication (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 19 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix/vector multiplication (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }
  // printf( "Size before multiplication %d\n" , (*valStack).size() );
  vec.z = (float)(*valStack).back();
  (*valStack).pop_back();
  vec.y = (float)(*valStack).back();
  (*valStack).pop_back();
  vec.x = (float)(*valStack).back();
  (*valStack).pop_back();
  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val[ ind ]= (float)(*valStack).back();
    (*valStack).pop_back();
  }

  //printf( "Mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  //printf( "Vect %.2f %.2f %.2f\n" , vec.x , vec.y , vec.z  );

  matvec.product( vec , mat );
  (*valStack).push_back(matvec.x);
  (*valStack).push_back(matvec.y);
  (*valStack).push_back(matvec.z);
  // printf( "Size after multiplication %d\n" , (*valStack).size() );

  //printf( "Pro result %.2f %.2f %.2f\n" , matvec.x , matvec.y , matvec.z );
  (*sizStack).push_back( 3 );
}

void VecMatMultiplication( std::vector <double> * valStack ,  
			   std::vector <int> * sizStack ) 
{
  dm_matrix44 mat;
  dm_vector3 vec;
  dm_vector3 vecmat;
  float * val = mat.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  int size2 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 16 || size2 != 3 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for vector/matrix multiplication (%d*%d)!" , size2 , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 + size2 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 19 ) {
	sprintf( ErrorStr , "Error: incomplete matrices for vector/matrix multiplication!" ); ReportError( ErrorStr ); 
	return;    
  }
  for( int ind = 15 ; ind >= 0 ; ind-- ) {
    val[ ind ]= (float)(*valStack).back();
    (*valStack).pop_back();
  }
  // printf( "Size before multiplication %d\n" , (*valStack).size() );
  vec.z = (float)(*valStack).back();
  (*valStack).pop_back();
  vec.y = (float)(*valStack).back();
  (*valStack).pop_back();
  vec.x = (float)(*valStack).back();
  (*valStack).pop_back();

  // printf( "Mat %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  // printf( "Vect %.2f %.2f %.2f\n" , vec.x , vec.y , vec.z  );

  vecmat.product( mat , vec );
  (*valStack).push_back(vecmat.x);
  (*valStack).push_back(vecmat.y);
  (*valStack).push_back(vecmat.z);
  // printf( "Size after multiplication %d\n" , (*valStack).size() );

  // printf( "Pro result %.2f %.2f %.2f\n" , vecmat.x , vecmat.y , vecmat.z );
  (*sizStack).push_back( 3 );
}

void VecNorm(std::vector <double> * valStack ,  
		 std::vector <int> * sizStack) 
{
  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 < 3 ) {
    sprintf( ErrorStr , "Error: incomplete vector for vector norm computation (size:%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 3 ) {
    sprintf( ErrorStr , "Error: incomplete vector for vector norm computation (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }

  float norm = 0;
  for( int ind = 3 - 1 ; ind >= 0 ; ind-- ) {
    norm  += (float)pow((*valStack).back(),2.0);
    (*valStack).pop_back();
  }

  (*valStack).push_back(sqrt(norm));
  (*sizStack).push_back( 1 );
  // printf( "VecNorm result %.2f\n" , sqrt(norm) );
}

void MatInverse( std::vector <double> * valStack ,  
		 std::vector <int> * sizStack ) 
{
  float init[16];
  float inv[16];

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix inversion (%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix inversion (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }
  for( int ind = 16 - 1 ; ind >= 0 ; ind-- ) {
    init[ ind ]	= (float)(*valStack).back();
    (*valStack).pop_back();
  }
  invert_matrix(init, inv);
  for( int ind = 0 ; ind < 16 ; ind++ ) {  
    // printf( "matrix pusback [%.5f,%d]\n" , inv[ ind ] , size1 );
    (*valStack).push_back(inv[ ind ]);
  }
  (*sizStack).push_back( 16 );
}

void PointUnProject( std::vector <double> * valStack ,  
		     std::vector <int> * sizStack ) {
  double init[4];

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 3 ) {
    sprintf( ErrorStr , "Error: incomplete point coordinate for projection computation (%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  for( int ind = 3 - 1 ; ind >= 0 ; ind-- ) {
    init[ ind ]	= (*valStack).back();
    (*valStack).pop_back();
  }

  GLdouble* model = new GLdouble[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  GLdouble* proj = new GLdouble[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  GLint* vp = new GLint[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLdouble xt;
  GLdouble yt;
  GLdouble zt;
  
  // get the coordinates in the scene world
  gluUnProject( init[0], glutGet(GLUT_WINDOW_HEIGHT)-init[1], init[2], 
		model, proj, vp, &xt, &yt, &zt);

  // printf( "matrix pusback [%.5f,%d]\n" , inv[ ind ] , size1 );
  (*valStack).push_back(xt);
  (*valStack).push_back(yt);
  (*valStack).push_back(zt);
  (*sizStack).push_back(3);
}

void PointProject( std::vector <double> * valStack ,  
		   std::vector <int> * sizStack ) {
  double source[3];
  double target[3];

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 3 ) {
    sprintf( ErrorStr , "Error: incomplete point coordinate for projection computation (%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  for( int ind = 3 - 1 ; ind >= 0 ; ind-- ) {
    source[ ind ] = (*valStack).back();
    (*valStack).pop_back();
  }

  PointGluProject( source , target );

  // printf( "matrix pusback [%.5f,%d]\n" , inv[ ind ] , size1 );
  (*valStack).push_back(target[0]);
  (*valStack).push_back(target[1]);
  (*valStack).push_back(target[2]);
  (*sizStack).push_back(3);
}

void PointGluProject( double source[3] ,
		      double target[3] ) {

  GLdouble model[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  GLdouble proj[16];
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  // printf( "ModelView %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n" , model[0] , model[1] , model[2] , model[3] , model[4] , model[5] , model[6] , model[7] , model[8] , model[9] , model[10] , model[11] , model[12] , model[13] , model[14] , model[15] );
  
  
  // get the coordinates in the scene world
  gluProject( source[0], source[1], source[2], 
	      model, proj, vp, 
	      &(target[0]), &(target[1]), &(target[2]));

}

// this function has been implemented with the help of Okko Welin
// while he made his Masters Thesis at Helsinki University of Technology

void Homography( std::vector <double> * valStack ,  
		 std::vector <int> * sizStack ,
		 int fixedParam ) {
#ifdef DM_HAVE_OPENCV 
  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 < 16 ) {
    sprintf( ErrorStr , "Error: incomplete point list for homography computation (%d), minimally 4 real and 4 measured 2D points are expected !" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 16 ) {
    sprintf( ErrorStr , "Error: incomplete point list for homography computation (%d), minimally 4 real and 4 measured 2D points are expected !" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }

  if( size1 % 4 != 0 ) {
    sprintf( ErrorStr , "Error: even point list for homography computation required (%d), should contain as many real and measured points !" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }

  CvMat* g_hom;
  CvMat* g_real_corners;
  CvMat* g_meas_corners;

  int nbInputPoints = size1 / 4;

  g_hom = cvCreateMat(3,3,CV_32FC1);
  g_real_corners = cvCreateMat(nbInputPoints,2,CV_32FC1);
  g_meas_corners = cvCreateMat(nbInputPoints,2,CV_32FC1);

  // transformed quad: y then x (reverse order access)
  for( int ind = nbInputPoints - 1 ; ind >= 0  ; ind-- ) {
    cvmSet(g_meas_corners,ind,1,(*valStack).back());
    (*valStack).pop_back();
    cvmSet(g_meas_corners,ind,0,(*valStack).back());
    (*valStack).pop_back();
  }
  //   printf( "Measured: " );
  //   for( int ind = 0 ; ind < nbInputPoints  ; ind++ ) {
  //     printf( "%.2f " , cvmGet(g_meas_corners,ind,0) );
  //     printf( "%.2f " , cvmGet(g_meas_corners,ind,1) );
  //   }
  //   printf( "\n" );
  
  // initial quad: y then x (reverse order access)
  for( int ind = nbInputPoints - 1 ; ind >= 0  ; ind-- ) {
    cvmSet(g_real_corners,ind,1,(*valStack).back());
    (*valStack).pop_back();
    cvmSet(g_real_corners,ind,0,(*valStack).back());
    (*valStack).pop_back();
  }
  //   printf( "Real: " );
  //   for( int ind = 0 ; ind < nbInputPoints ; ind++ ) {
  //     printf( "%.2f " , cvmGet(g_real_corners,ind,0) );
  //     printf( "%.2f " , cvmGet(g_real_corners,ind,1) );
  //   }
  //   printf( "\n" );

  // compute the homography
  // printf( "Homog for %d points\n" , nbInputPoints );
  cvFindHomography( g_real_corners, g_meas_corners, g_hom );

  // transformed quad
  int indCol = 0;
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    if( ind == fixedParam ) {
      for( int ind2 = 0 ; ind2 < 4 ; ind2++ ) {
	if( ind2 == fixedParam ) {
	  (*valStack).push_back(1.0f);
	}
	else {
	  (*valStack).push_back(0.0f);
	}
      }
    }
    else {
      int indLine = 0;
      for( int ind2 = 0 ; ind2 < 4 ; ind2++ ) {
	if( ind2 == fixedParam ) {
	  (*valStack).push_back(0.0f);
	}
	else {
	  (*valStack).push_back(cvmGet(g_hom,indLine,indCol));
	  indLine++;
	}
      }
      indCol++;
   }
  }
  
  cvReleaseMat(&g_real_corners);
  cvReleaseMat(&g_meas_corners);
  cvReleaseMat(&g_hom);

  (*sizStack).push_back( 16 );
#else
    sprintf( ErrorStr , "Error: use --enable-OpenCV option to use homography functions!" ); ReportError( ErrorStr ); 
#endif
}

void ToEuler( std::vector <double> * valStack ,  
	      std::vector <int> * sizStack ) {
  dm_matrix44 mat;
  float * init = mat.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix inversion (%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 16 ) {
    sprintf( ErrorStr , "Error: incomplete matrices for matrix inversion (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }
  for( int ind = 16 - 1 ; ind >= 0 ; ind-- ) {
    init[ ind ]	= (float)(*valStack).back();
    (*valStack).pop_back();
  }

  // the content of the local transformation
  // matrix is composed with the existing values
  // for the translation and Euler angles
  float loc_X; float loc_Y; float loc_Z; 
  float loc_roll; float loc_yaw; float loc_pitch;
  mat.MatrixtoEuler( &loc_X, &loc_Y, &loc_Z , 
		     &loc_roll, &loc_yaw, &loc_pitch );

//   mat.print(stdout,1,1);
//   printf("Euler out angles/transl %.3f %.3f %.3f %.3f %.3f %.3f\n" , loc_roll,loc_yaw,loc_pitch,loc_X,loc_Y,loc_Z);
  // printf( "matrix pusback [%.5f,%d]\n" , inv[ ind ] , size1 );
  (*valStack).push_back(loc_roll);
  (*valStack).push_back(loc_yaw);
  (*valStack).push_back(loc_pitch);
  (*valStack).push_back(loc_X);
  (*valStack).push_back(loc_Y);
  (*valStack).push_back(loc_Z);
  (*sizStack).push_back( 6 );
}

void FromEuler( std::vector <double> * valStack ,  
		std::vector <int> * sizStack ) {
  dm_matrix44 mat_rot_translPre,mat_translPost,mat_resul;
  float * mat_resul_val = mat_resul.GetValue();

  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 != 9 ) {
    sprintf( ErrorStr , "Error: incomplete pre X, Y, Z, roll, yaw, pitch, and post X, Y, Z  for Euler rotation and translation computation (%d)!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 9 ) {
    sprintf( ErrorStr , "Error: incomplete pre X, Y, Z, roll, yaw, pitch, and post X, Y, Z  for Euler rotation and translation computation (%d)!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }

  // the content of the local transformation
  // matrix is composed with the existing values
  // for the translation and Euler angles
  float loc_pre_X; float loc_pre_Y; float loc_pre_Z; 
  float loc_post_X; float loc_post_Y; float loc_post_Z; 
  float loc_roll; float loc_yaw; float loc_pitch;
  loc_pre_Z = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_pre_Y = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_pre_X = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_pitch = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_yaw = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_roll = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_post_Z = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_post_Y = (float)(*valStack).back();
  (*valStack).pop_back();
  loc_post_X = (float)(*valStack).back();
  (*valStack).pop_back();

  mat_rot_translPre.EulertoMatrix( loc_pre_X, loc_pre_Y, loc_pre_Z , 
				   loc_roll, loc_yaw, loc_pitch );

  mat_translPost.TranslationtoMatrix( loc_post_X, loc_post_Y, loc_post_Z );


  mat_translPost.product( mat_resul , mat_rot_translPre );

  // printf("Transl: %.3f %.3f %.3f \n" ,  loc_post_X, loc_post_Y, loc_post_Z);
  // printf("Transl: ");  mat_translPost.print(stdout,1,1); printf("\n");
  // printf("Mat: ");  mat_resul.print(stdout,1,1); printf("\n");
  for( int ind = 0 ; ind < 16 ; ind++ ) {  
    // printf( "matrix pusback [%.5f,%d]\n" , inv[ ind ] , size1 );
    (*valStack).push_back(mat_resul_val[ ind ]);
  }
  (*sizStack).push_back( 16 );
}

// this function has been implemented in collaboration with Peter Eisert
// HHI, Fraunhofer Institute (2008)

// angles are given in degrees
void CameraOperation( std::vector <double> * valStack ,  
		      std::vector <int> * sizStack ) {
  // 10 input parameters
  // 16 output parameters (a 4x4 homogeneous 3d transf matrix)
  int size1 = (*sizStack).back();
  (*sizStack).pop_back();
  if( size1 < 10 ) {
    sprintf( ErrorStr , "Error: incomplete parameter list for camera computation (%d), 10 parameters are expected!" , size1 ); ReportError( ErrorStr ); 
    for( int ind = 0 ; ind < size1 ; ind++ ) {
      (*valStack).pop_back();
    }
    return;    
  }

  if( (*valStack).size() < 10 ) {
    sprintf( ErrorStr , "Error: incomplete parameter list for camera computation (%d), 10 parameters are expected!" , (int)(*valStack).size() ); ReportError( ErrorStr ); 
    return;    
  }
  
  ///////////////////////////////////////////////////////////////////
  // parameter reading
  ///////////////////////////////////////////////////////////////////

  // 10,9,8: Euler angles 8in degrees) for the jewel rotation
  // unused currently
  // float EulerRotationAngleZ=
  (*valStack).back(); // 0
  (*valStack).pop_back();
  float EulerRotationAngleY=(float)(*valStack).back(); // angle
  (*valStack).pop_back();
  // unused currently
  // float EulerRotationAngleX=
  (*valStack).back(); // 0
  (*valStack).pop_back();


  // 7,6,5: camera Rigid Body translation x,y,z vector
  float translation[3];

  translation[2]=(float)(*valStack).back(); // -0.300000f
  (*valStack).pop_back();
  translation[1]=(float)(*valStack).back(); // 0.000000f
  (*valStack).pop_back();
  translation[0]=(float)(*valStack).back(); // 0.000445f
  (*valStack).pop_back();


  // 4,3,2,1: camera Rigid Body rotation x,y,z axis and angle
  // calibration rigid body transformation that brings back to a perfect frustum
  float rotationAxis[3];
  float rotationAngle;

  rotationAngle=(float)(*valStack).back(); // 0.266038f
  (*valStack).pop_back();

  rotationAxis[2]=(float)(*valStack).back(); // -0.100140f
  (*valStack).pop_back();
  rotationAxis[1]=(float)(*valStack).back(); // -0.008135f
  (*valStack).pop_back();
  rotationAxis[0]=(float)(*valStack).back(); // 0.994940f
  (*valStack).pop_back();

  //fprintf(fileLog , "Camera3 first params axis: %.5f %.5f %.5f angle rotation %.5f \n" ,  rotationAxis[0] ,  rotationAxis[1] ,  rotationAxis[2] , rotationAngle );
  //   printf( "Camera translation %.5f %.5f %.5f \n" ,  translation[0] ,  translation[1] ,  translation[2] );
  //   printf( "Euler angles %.5f %.5f %.5f \n" , EulerRotationAngleX ,  EulerRotationAngleY ,  EulerRotationAngleZ );

  // calibration rotation matrix 
  // angle is given in degree
  dm_vector3 rotationAxisVector( rotationAxis );
  dm_matrix44 cRBrotMatrix;
  cRBrotMatrix.rotationMatrixNew(rotationAxisVector, rotationAngle);

//   fprintf(stdout , "RB Init " );
//   cRBrotMatrix.print(stdout,1,1);

  
  ///////////////////////////////////////////////////////////////////
  // dynamic computations (from a rotation vector)
  ///////////////////////////////////////////////////////////////////

  // rotation of the jewel around its axis
  // in this version: no translation is taken into consideration
  dm_matrix44 cRyMatrix;
  rotationAxisVector.update( 0.0f , 1.0f , 0.0f );
  cRyMatrix.rotationMatrixNew(rotationAxisVector, EulerRotationAngleY*DegtoRad);
  //cRyMatrix.EulertoMatrix( 0.0 , 0.0 , 0.0 ,
		//	   EulerRotationAngleX , 
		//	   EulerRotationAngleY , 
		//	   EulerRotationAngleZ );

//   fprintf(stdout , "cRy " );
//   cRyMatrix.print(stdout,1,1);
  
  // calibration multiplied by jewel rotation
  // float4x4 cRB = mul(cRB.getRotMat(), cRy) + cRB.getTranslationVector();
  cRyMatrix.productRot(cRBrotMatrix,cRBrotMatrix);


  cRBrotMatrix.SetValue(translation[0],12);
  cRBrotMatrix.SetValue(translation[1],13);
  cRBrotMatrix.SetValue(translation[2],14);

//   fprintf(stdout , "RB Final " );
//   cRBrotMatrix.print(stdout,1,1);

  // transpose the rotation
  float *mat = cRBrotMatrix.GetValue();
  float tmp[16];
   memcpy( (char *)tmp , mat , 16 * sizeof( float ) );
   for( int c = 0 ; c < 3 ; c++ ) {
     for( int l = 0 ; l < 3 ; l++ ) {
       mat[ 4 * l + c] = tmp[4 * c + l ];
     }
   }

  ///////////////////////////////////////////////////////////////////
  // returned values storage

  // transformed quad
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    (*valStack).push_back(mat[ind]);
  }
  (*sizStack).push_back( 16 );
}


dm_CCalculator::dm_CCalculator( char * ch ) {
  my_predefOperators = predefinedOperators;
  if( ch ) {
    outString = new char[ strlen( ch ) * 2 ];
    outString_length = strlen( ch ) * 2;
  }
  else {
    outString = new char[ 256 ];
    outString_length = 256;
  }
}

dm_CCalculator::~dm_CCalculator( void ) {
  delete [] outString;
  outString = NULL;
  
  outString_length = 0;
  
  if (!symbolsStack.empty()) {
    char* stackChar;
    for (unsigned int i = 0; i < symbolsStack.size(); i++) {
      stackChar = symbolsStack.back();
      symbolsStack.pop_back();
      if( stackChar ) {
	delete stackChar;
      }
    }
  }
}

void dm_CCalculator::ToPostfix( int *fileDepth , int *p_c  ,
				int p_withSpaceCommentsInclude , 
				char *charstring , int *ind , 
				bool atRunTime ) {
  char * chAux = NULL;
  char * tmp_chAux;
  char * tmp_s;
  int   indString;
  // stores the preceding token last char in order to
  // decide whether a signed number can be accepted
  char  precedingToken = '(';
  int level = 0;
  
  // printf( "ToPostfix input string [%s]\n" , charstring ); 

  // char chAux[ 1024 * XMLAttrValLength ];
  chAux = new char[ 1024 * XMLAttrValLength ];
  if( !chAux ) {
    sprintf( ErrorStr , "Error: string allocation error!" ); ReportError( ErrorStr ); throw 16;
  }
    
  indString = 0;
  SpaceCommentsInclude( fileDepth , p_c , charstring,ind);

//   if( charstring && strlen(charstring) > 200 ) {
//     printf( "input string [%s] length %d chaux length %d \n" , charstring , strlen(charstring) , 1024 * XMLAttrValLength ); 
//   }

  while( *p_c && *p_c != EOF ) { 
    // ----------------- A NUMBER ------------------
    if( _Num(*p_c) || *p_c == '.' 
	// signed operand that follows an operator or nothing
	// (begins an expression)
	|| ( (*p_c == '-' || *p_c == '+') 
	     && !_Alphanum( precedingToken ) 
	     && precedingToken != ')' 
	     && precedingToken != '}' ) ) {

      // printf( "Postfix a number [%s,%c]\n" , charstring , *p_c ); 

      // Here we have a number
      // new expression, must return
      if( *outString && symbolsStack.empty()) {
	// printf( "new expression, must return\n" ); 
	break;
      }

      indChar = 0;
      if( indString >= 1024 ) {
	sprintf( ErrorStr , "Error: excessive formula size!" ); ReportError( ErrorStr ); 
	delete [] chAux;
	chAux = NULL;
	return;
      }

      // set the temporary string
      tmp_chAux = &chAux[ indString++ * XMLAttrValLength ];

      // first char
      if(*p_c == '-' || *p_c == '+') {
	tmp_chAux[ indChar++ ] = *p_c;
	*p_c = ReadChar(fileDepth,charstring,ind);
      }

      // next chars
      // a number
      if(_Num(*p_c) || *p_c == '.' ) {
	while( (_Num(*p_c) || *p_c == '.' ) && indChar < XMLAttrValLength ) {
	  tmp_chAux[ indChar++ ] = *p_c;
	  *p_c = ReadChar(fileDepth,charstring,ind);
	}
	
	// exponent (VRML translation)
	if( *p_c == 'e' ) {
	  tmp_chAux[ indChar++ ] = *p_c;
	  *p_c = ReadChar(fileDepth,charstring,ind);
	  if( (*p_c == '-' || *p_c == '+') ) {
	    tmp_chAux[ indChar++ ] = *p_c;
	    *p_c = ReadChar(fileDepth,charstring,ind);
	  }
	  while( (_Num(*p_c) || *p_c == '.' ) && indChar < XMLAttrValLength ) {
	    tmp_chAux[ indChar++ ] = *p_c;
	    *p_c = ReadChar(fileDepth,charstring,ind);
	  }
	}

	tmp_chAux[ indChar ] = 0;
      }
      // not a number: a parameter
      else if( *p_c == '{' ) {
	if( !ReadBalancedStringWithFrontiers( fileDepth , 
					      p_c , tmp_chAux + indChar + 1 ,
					      XMLAttrValLength , '{' , '}' , 
					      true , false , charstring,ind ) ) {
	  sprintf( ErrorStr , "Error: incorrect parameter name!" ); ReportError( ErrorStr ); 
	  delete [] chAux;
	  chAux = NULL;
	  return;
	}
	
	// checks whether the parameter value contains an expression that
	// must be recursively evaluated before 
	// evaluating the parameter
	if( strchr( tmp_chAux + indChar + 1 , '(' ) ) {
	  char substitutedCharstring[XMLAttrValLength];
	  StrCpyAndSubst( substitutedCharstring , 
			  tmp_chAux + indChar + 1 , 
			  XMLAttrValLength );
	  strcpy( tmp_chAux + indChar + 1 , substitutedCharstring );
	}

	tmp_chAux[indChar] = '{';
	int stringLength = strlen(tmp_chAux + indChar);
	tmp_chAux[indChar + stringLength] = '}';
	tmp_chAux[indChar + stringLength + 1] = 0;
      }

      StrCat(&outString,tmp_chAux,&outString_length);
      StrCat(&outString," ",&outString_length);
      precedingToken = '0';
      // printf( "cat -param [%s] [%c]\n" , tmp_chAux , precedingToken ); 
    }

    // ----------------- A PARAMETER ------------------
    else if( *p_c == '{') {
      // new expression, must return
      if( *outString && symbolsStack.empty()) {
	break;
      }

      if( indString >= 1024 ) {
	sprintf( ErrorStr , "Error: excessive formula size!" ); ReportError( ErrorStr ); 
	delete [] chAux;
	chAux = NULL;
	return;
      }
      tmp_chAux = &chAux[ indString++ * XMLAttrValLength ];
      if( !ReadBalancedStringWithFrontiers( fileDepth , p_c , tmp_chAux + 1 ,
					    XMLAttrValLength , '{' , '}' , 
					    true , false , charstring,ind ) ) {
	sprintf( ErrorStr , "Error: incorrect parameter name!" ); ReportError( ErrorStr ); 
	delete [] chAux;
	chAux = NULL;
	return;
      }
      
      // checks whether the parameter value contains an expression that
      // must be recursively evaluated before 
      // evaluating the parameter
      if( strchr( tmp_chAux + 1 , '(' ) ) {
	char substitutedCharstring[XMLAttrValLength];
	StrCpyAndSubst( substitutedCharstring , 
			tmp_chAux + 1 , 
			XMLAttrValLength);
	strcpy( tmp_chAux + 1 , substitutedCharstring );
      }
      
      tmp_chAux[0] = '{';
      int stringLength = strlen(tmp_chAux);
      tmp_chAux[stringLength] = '}';
      tmp_chAux[stringLength + 1] = 0;

      StrCat(&outString,tmp_chAux,&outString_length);
      StrCat(&outString," ",&outString_length);
      precedingToken = '0';
    }

    // ----------------- A TAG ------------------
    else if( *p_c == '<') {
      // new tag XML, must return
      break;
    }

    // ----------------- AN OPERATOR ------------------
    else {
      // new expression at lowest level , must return
      if( *outString 
	  && (*p_c != ')' && symbolsStack.empty() ) ) {
	break;
      }

      if( indString >= 1024 ) {
	sprintf( ErrorStr , "Error: excessive formula size!" ); ReportError( ErrorStr ); 
	delete [] chAux;
	chAux = NULL;
	return;
      }
      tmp_chAux = &chAux[ indString++ * XMLAttrValLength ];

      //We have got an operator or a function
      //Here we have a function name
      if( _Alphanum(*p_c) ) {
	indChar = 0;	tmp_chAux[ indChar++ ] = *p_c;
	*p_c = ReadChar(fileDepth,charstring,ind);
	while( (_Alphanum(*p_c) ) && indChar < XMLAttrValLength ) {
	  tmp_chAux[ indChar++ ] = *p_c;
	  *p_c = ReadChar(fileDepth,charstring,ind);
	}
	tmp_chAux[ indChar ] = 0;
      }
      // an operator
      else {
	indChar = 0;
	tmp_chAux[ indChar++ ] = *p_c;
	tmp_chAux[ indChar ] = 0;
	*p_c = ReadChar(fileDepth,charstring,ind);
	// doubled operator for homogenous coordinate operations
	if( *p_c == tmp_chAux[ 0 ] && (*p_c ==':' || *p_c =='?') ) {
	  tmp_chAux[ indChar++ ] = *p_c;
	  tmp_chAux[ indChar ] = 0;
	  *p_c = ReadChar(fileDepth,charstring,ind);
	}
      }

      int operator_val = -1;
      for (int i = 0; i < g_constNumberOfOperators;i++) {
	if( strcmp( tmp_chAux , 
		    my_predefOperators[i]->m_szSignature ) == 0 ) {
	  operator_val = i;
	  break;
	}
      }

      // end of the expression
      if( operator_val < 0 ) {
	sprintf( ErrorStr , "Error: incorrect arithmetic expression [%s]!" , tmp_chAux ); ReportError( ErrorStr );
	break;
	// delete [] chAux;
	// chAux = NULL;
	// return; 
      }
      precedingToken = *tmp_chAux;

      //Its a comma;
      if ( strcmp(tmp_chAux,",") == 0 ) {
	//printf("comma lelvel %d\n" , level);
	// lowest level comma: value separator, must flush pile
	if( level <= 1 ) {
	  while( !symbolsStack.empty()
	       && symbolsStack.back() 
	       && strcmp(symbolsStack.back(),"(") != 0) {
	    tmp_s = symbolsStack.back();
	    //printf( "flush  symbolsStack.pop [%s]\n" , tmp_s ); 
	    StrCat(&outString,tmp_s,&outString_length);
	    StrCat(&outString," ",&outString_length);

	    symbolsStack.pop_back();
	    valuesStack.pop_back();
	    sizeStack.pop_back();
	  }
	}
      }
      
      //Its a left brace;
      else if (strcmp(tmp_chAux,"(") == 0) {
	// printf("opening parenth\n");
	symbolsStack.push_back(tmp_chAux);
	valuesStack.push_back(0);
	sizeStack.push_back(1);
	level++;
      }

      //Its a right brace;
      else if (strcmp(tmp_chAux,")") == 0) {
	// printf("closing parenth\n");
	while( !symbolsStack.empty() 
	       && symbolsStack.back() 
	       && strcmp(symbolsStack.back(),"(") != 0) {
	  tmp_s = symbolsStack.back();
	  // printf( "operands symbolsStack.pop [%s]\n" , tmp_s ); 
	  StrCat(&outString,tmp_s,&outString_length);
	  StrCat(&outString," ",&outString_length);

	  symbolsStack.pop_back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();
	}

	if( symbolsStack.empty() || strcmp(symbolsStack.back(),"(") != 0 ) {
	  sprintf( ErrorStr , "Error: missing closing parenthesis %s!" , charstring ); ReportError( ErrorStr ); 
	  delete [] chAux;
	  chAux = NULL;
	  return;
	}
	  
	symbolsStack.pop_back();
	valuesStack.pop_back();
	sizeStack.pop_back();
	level--;
     }

      //Finally its a math operator;
      else {
	//printf( "math operator [%s]\n" , tmp_chAux ); 
	int currentOpPriority = my_predefOperators[operator_val]->m_usPriority;
	
	if (!valuesStack.empty()) {
	  while( !valuesStack.empty() && valuesStack.back() 
		 && currentOpPriority <= valuesStack.back()) {
	    tmp_s = symbolsStack.back();
	    //printf( "operator symbolsStack.pop [%s]\n" , tmp_s ); 
	    StrCat(&outString,tmp_s,&outString_length);
	    StrCat(&outString," ",&outString_length);

	    symbolsStack.pop_back();
	    valuesStack.pop_back();
	    sizeStack.pop_back();
	  }
	}
	
	symbolsStack.push_back(tmp_chAux);
	valuesStack.push_back(currentOpPriority);
	sizeStack.push_back(1);
	//printf( "math operator symbolsStack.push -operator [%s]\n" , tmp_chAux ); 
	//printf( "symbolsStack.push -operator [%s]\n" , tmp_chAux ); 
      }
    }
    
    //if( p_withSpaceCommentsInclude ) 
    {
      SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
    }
  }

  // printf( "output at level %d!\n" , level ); 
  if( level != 0 ) {
    sprintf( ErrorStr , "Error: unbalanced closing parenthesis %s!" , charstring ); ReportError( ErrorStr ); 
  //printf( "pile flush\n" ); 
    symbolsStack.clear();
    valuesStack.clear();
    sizeStack.clear();
    delete [] chAux;
    chAux = NULL;
    return;
  }

  //printf( "pile flush\n" ); 
  while (!symbolsStack.empty()) {
    tmp_s = symbolsStack.back();
    //printf( "symbolsStack.pop [%s]\n" , tmp_s ); 
    if( tmp_s ) {
      StrCat(&outString,tmp_s,&outString_length);
      StrCat(&outString," ",&outString_length);
    }
    symbolsStack.pop_back();
  }
    
  valuesStack.clear();
  sizeStack.clear();
  delete [] chAux;
  chAux = NULL;
}

void dm_CCalculator::Process( bool atRunTime  ,
			      char * outputString ) {
  // printf( "Process Postfixed string [%s]\n" ,outString);
  char * tmp_s1;

  valuesStack.clear();
  sizeStack.clear();

  tmp_s1 = outString;

  while( tmp_s1 != NULL && *tmp_s1 != 0 ) {
    // printf( "Single term [%s]\n" ,tmp_s1);

    if (_Num(tmp_s1[0]) || tmp_s1[0] == '.'
	|| (tmp_s1[0] == '+' && (_Num(tmp_s1[1]) || tmp_s1[1] == '.'))
	|| (tmp_s1[0] == '-' && (_Num(tmp_s1[1]) || tmp_s1[1] == '.')) ) {
      char  chAux[XMLAttrValLength];
      strcpy( chAux , tmp_s1 );
      if( strchr( chAux , ' ' ) ) {
	*(strchr( chAux , ' ' ) ) = 0;
      }
      valuesStack.push_back(atof(tmp_s1)); //@#@
      sizeStack.push_back(1); //@#@

      tmp_s1 = strstr( tmp_s1 , " " ) + 1;
    }
    else if( tmp_s1[0] == '{'
	     || ( (tmp_s1[0] == '+' || tmp_s1[0] == '-') 
		  && tmp_s1[1] == '{') ) {
      char  chAux[XMLAttrValLength];
      float signe = 1.0;
      if( tmp_s1[0] == '+' || tmp_s1[0] == '-' ) {
	strcpy( chAux , tmp_s1 + 1 );
	if( tmp_s1[0] == '-' ) {
	  signe = -1.0;
	}
      }
      else {
	strcpy( chAux , tmp_s1 );
      }

      if( strchr( chAux , '}' ) ) {
	*(strchr( chAux , '}' ) + 1) = 0;
      }

	// printf( "parameter string (%s)!\n" , chAux );

      // string corresponds to a parameter
      // the # character is delected and replaced by a {
      // the string is terminated by a }
      if( chAux[1] == '#' ) {
	valuesStack.push_back( signe * atof( ParameterValue( chAux ) ) );
	sizeStack.push_back(1);
      }
      // string corresponds to a scalar variable
      // the string begins by "{$"
      // the string is terminated by a }
      else if( chAux[1] == '$' ) {
	if( !atRunTime ) {
	  sprintf( ErrorStr , "Error: variables and attribute values can only be used at run-time (in scripts)!" ); ReportError( ErrorStr ); exit(0);
	}
	  
	valuesStack
	  .push_back( signe * ReferenceNodeAttributeValue( chAux  , 
							   atRunTime ,
							   outputString ) );
	sizeStack.push_back(1);
      }
      // string corresponds to a table variable
      // the string begins by "{@"
      // the string is terminated by a }
      else if( chAux[1] == '@' ) {
	if( !atRunTime ) {
	  sprintf( ErrorStr , "Error: table or matrix values can only be used at run-time (in scripts)!" ); ReportError( ErrorStr ); exit(0);
	}
	
	// printf( "Process table [%s]\n" , chAux );
	int size = 0; DataType tableOrMatrixType = EmptyDataType;
	dm_ValScalar * resul = ReferenceNodeAttributeTable( chAux  , 
							    atRunTime ,
							    &size ,
							    &tableOrMatrixType );
	// printf( "Table processed size %d\n" , size );
	for( int ind = 0 ; ind < size ; ind++ ) {
	  valuesStack.push_back( resul[ ind ].GetNumericalValue() );
	}
	sizeStack.push_back(size);
      }
      else {
	fprintf( stderr , "Error: incorrect parameter string (%s): {#name} or {$name:attribute} expected !\n" , chAux ); ReportError( ErrorStr ); return;
      }

      tmp_s1 = strstr( tmp_s1 , "}" ) + 2;
    }
    else {
      // printf( "Process operator [%s]\n" ,tmp_s1);
      int operator_val = -1;
      for (int i = 0; i < g_constNumberOfOperators;i++) {
	if( strncmp( tmp_s1 , my_predefOperators[i]->m_szSignature , 
		     strlen(my_predefOperators[i]->m_szSignature) ) == 0 ) {
	  operator_val = i;
	  break;
	}
      }
	
      if( operator_val < 0 ) {
	sprintf( ErrorStr , "Error: Unknown operation [%s]\n!" , tmp_s1 ); ReportError( ErrorStr ); return; 
      }

      int currentNumberOfOps = my_predefOperators[operator_val]->m_usNumberOfOps;

      if( (int)valuesStack.size() < currentNumberOfOps ) {
	sprintf( ErrorStr , "Error: missing operand (%d/%d) for operator [%s] in (%s)!" , (int)valuesStack.size() , currentNumberOfOps ,  my_predefOperators[operator_val]->m_szSignature , tmp_s1 ); ReportError( ErrorStr );
	return;
      }

      // printf( "operation [%s,%d] stack size [%d]!\n" , my_predefOperators[operator_val]->m_szSignature , currentNumberOfOps , (int)valuesStack.size() ); 

      switch(currentNumberOfOps) {
	// constants or dynamic predefined variables such as now
      case 0:
	{
	  double arg1_dummy = 0.0;

	  double arg2_dummy = 0.0;

	  double arg3_dummy = 0.0;

	  valuesStack.push_back((*(my_predefOperators[operator_val]->m_pRegisteredFn))(&arg1_dummy,&arg2_dummy,&arg3_dummy));
	  sizeStack.push_back(1);
	  break;
	}
      case 1:
	{
	  double arg1 = valuesStack.back();
	  int size1 = sizeStack.back();

	  double arg2_dummy = 0.0;
	  // int size2 = 0;

	  double arg3_dummy = 0.0;
	  // int size3 = 0;

	  if( size1 == 1 ) {
	    valuesStack.pop_back();
	    sizeStack.pop_back();
	    valuesStack.push_back((*(my_predefOperators[operator_val]->m_pRegisteredFn))(&arg1,&arg2_dummy,&arg3_dummy));
	    sizeStack.push_back(1);
	  }
	  else if( operator_val == DM_predefVecNormOperator ) {
	    VecNorm( &valuesStack , &sizeStack );
	  }
	  else {
	    // double arg1 = valuesStack.back();
	    // int size1 = sizeStack.back();
	    switch( operator_val )
	      {
	      case DM_predefPointProjectOperator:
		PointProject( &valuesStack , &sizeStack );
		break;
	      case DM_predefPointUnProjectOperator:
		PointUnProject( &valuesStack , &sizeStack );
		break;
	      case DM_predefMatInverseOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		MatInverse( &valuesStack , &sizeStack );
		break;
	      case DM_predefHomographyXOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		Homography( &valuesStack , &sizeStack , 0 );
		break;
	      case DM_predefHomographyYOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		Homography( &valuesStack , &sizeStack , 1 );
		break;
	      case DM_predefHomographyZOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		Homography( &valuesStack , &sizeStack , 2 );
		break;
	      case DM_predefToEulerOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		ToEuler( &valuesStack , &sizeStack );
		break;
	      case DM_predefFromEulerOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		FromEuler( &valuesStack , &sizeStack );
		break;
	      case DM_predefCameraOperator:
		// double arg1 = valuesStack.back();
		// int size1 = sizeStack.back();
		// printf( "matrix Inversion [%.5f,%d]\n" , arg1 , size1 );
		CameraOperation( &valuesStack , &sizeStack );
		break;
	      default:
		printf( "Unknown unary operator [%s]\n" , tmp_s1 );
		valuesStack.pop_back();
		sizeStack.pop_back();
		valuesStack.push_back(0);
		sizeStack.push_back(1);
		break;
	      }
	  }
	  break;
	}
      case 2:
	{
	  // printf( "Operator with 2 operands\n");
	  double arg1 = valuesStack.back();
	  int size1 = sizeStack.back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();

	  double arg2 = valuesStack.back();
	  int size2= sizeStack.back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();

	  double arg3_dummy = 0.0;
	  // int size3 = 0;

	  // scalar OPER scalar
	  if( size1 == 1 && size2 == 1 ) {
	    valuesStack.push_back((*(my_predefOperators[operator_val]->m_pRegisteredFn))(&arg2,&arg1,&arg3_dummy));
	    sizeStack.push_back(1);
	  }

	  // one of the operands is not a scalar
	  else {
	    sizeStack.push_back( size2 );
	    sizeStack.push_back( size1 );
	    valuesStack.push_back( arg2 );
	    valuesStack.push_back( arg1 );

	    // matrix OPER matrix
	    if( size1 == size2 ) {
	      // matrix + matrix or matrix - matrix
	      if( operator_val == DM_predefAdditionOperator
		  || operator_val == DM_predefSubstractionOperator ) {
		MatMatOperation( &valuesStack , &sizeStack ,
				 operator_val );
	      }

	      // matrix * matrix
	      else if( operator_val == DM_predefMultiplicationOperator  ) {
		if( size1 == 16 ) {
		  // printf("mat mat multiplication (%s)\n" , tmp_s1);
		  MatMatMultiplication( &valuesStack , &sizeStack );
		}
		else {
		  sprintf( ErrorStr , "Error: only square 4x4 matrix can be multiplied (%d/%d) for operator [%s] in (%s)!" , size2 , size1 ,  my_predefOperators[operator_val]->m_szSignature , tmp_s1 ); ReportError( ErrorStr );
		  valuesStack.push_back(0);
		  sizeStack.push_back(1);
		}
	      }

	      else {
		sprintf( ErrorStr , "Error: unknown matrix/matrix operation for size (%d/%d) and for operator [%s] in (%s)!" , size2 , size1 ,  my_predefOperators[operator_val]->m_szSignature , tmp_s1 ); ReportError( ErrorStr );
		valuesStack.push_back(0);
		sizeStack.push_back(1);
	      }
	    }

	    // scalar OPER matrix
	    // or matrix OPER scalar
	    else if( size1 == 1 || size2 == 1 ) {
	      if( operator_val == DM_predefAdditionOperator
		  || operator_val == DM_predefSubstractionOperator 
		  || operator_val == DM_predefMultiplicationOperator 
		  || operator_val == DM_predefDivisionOperator ) {
		// printf("scalar/mat or mat/scalar multiplication %d/%d (%s)\n" , size2 , size1 , tmp_s1);
		ScalarMatOperation( &valuesStack , &sizeStack ,
				    operator_val );
	      }

	      else {
		sprintf( ErrorStr , "Error: unknown scalar/matrix or matrix/scalar operation for size (%d/%d) and for operator [%s] in (%s)!" , size2 , size1 ,  my_predefOperators[operator_val]->m_szSignature , tmp_s1 ); ReportError( ErrorStr );
		valuesStack.push_back(0);
		sizeStack.push_back(1);
	      }
	    }

	    // vector OPER matrix
	    // or matrix OPER vector
	    else if( operator_val == DM_predefMultiplicationOperator ) {
		// printf("vector/mat or mat/vector multiplication %d/%d (%s)\n" , size2 , size1 , tmp_s1);
	      if( size2 == 16 && size1 == 4 ) {
		HomogMatVecMultiplication( &valuesStack , &sizeStack );
	      }

	      else if( size2 == 4 && size1 == 16 ) {
		HomogVecMatMultiplication( &valuesStack , &sizeStack );
	      }

	      else if( size2 == 16 && size1 == 3 ) {
		MatVecMultiplication( &valuesStack , &sizeStack );
	      }

	      else if( size2 == 3 && size1 == 16 ) {
		VecMatMultiplication( &valuesStack , &sizeStack );
	      }

	      else {
		sprintf( ErrorStr , "Error: unknown matrix/vector or vector/matrix operation size (%d/%d) for operator [%s] in (%s)!" , size2 , size1 ,  my_predefOperators[operator_val]->m_szSignature , tmp_s1 ); ReportError( ErrorStr );
		valuesStack.push_back(0);
		sizeStack.push_back(1);
	      }
	    }

	    else {
	      valuesStack.push_back(0);
	      sizeStack.push_back(1);
	    }
	  }
	  // one of the operands is not a scalar
	  break;
	}
      case 3:
	{
	  double arg1 = valuesStack.back();
	  int size1 = sizeStack.back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();

	  double arg2 = valuesStack.back();
	  int size2= sizeStack.back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();

	  double arg3 = valuesStack.back();
	  int size3= sizeStack.back();
	  valuesStack.pop_back();
	  sizeStack.pop_back();

	  if( size1 == 1 && size2 == 1 && size3 == 1 ) {
	    valuesStack.push_back((*(my_predefOperators[operator_val]->m_pRegisteredFn))(&arg3,&arg2,&arg1));
	    sizeStack.push_back(1);
	  }
	  else {
	    valuesStack.push_back(0);
	    sizeStack.push_back(1);
	  }
	  break;
	}
	// matrix/matrix or matrix/vector operator
      default:
	{
	  printf( "matrix/matrix or matrix/vector operation [%s]\n" , tmp_s1 );
	  if( strncmp( tmp_s1 , "::" , 2 ) == 0 ) {
	    fprintf( stderr , "Deprecated: \"::\" matrix vector product is deprecated use \"*\" instead!\n" );
	    //printf( "HomogMatVecMultiplication [%s]\n" , tmp_s1 );
	    HomogMatVecMultiplication( &valuesStack , &sizeStack );
	  }
	  else if( strncmp( tmp_s1 , "??" , 2 ) == 0 ) {
	    fprintf( stderr , "Deprecated: \"??\" vector matrix product is deprecated use \"*\" instead!\n" );
	    HomogVecMatMultiplication( &valuesStack , &sizeStack );
	  }
	  else if( strncmp( tmp_s1 , ":" , 1 ) == 0 ) {
	    fprintf( stderr , "Deprecated: \":\" matrix vector product is deprecated use \"*\" instead!\n" );
	    MatVecMultiplication( &valuesStack , &sizeStack );
	  }
	  else if( strncmp( tmp_s1 , "?" , 1 ) == 0 ) {
	    fprintf( stderr , "Deprecated: \"?\" vector matrix product is deprecated use \"*\" instead!\n" );
	    VecMatMultiplication( &valuesStack , &sizeStack );
	  }
	  else if( strncmp( tmp_s1 , ";" , 1 ) == 0 ) {
	    fprintf( stderr , "Deprecated: \";\" matrix matrix product is deprecated use \"*\" instead!\n" );
	    MatMatMultiplication( &valuesStack , &sizeStack );
	  }
	  else {
	    	sprintf( ErrorStr , "Error: Unknown matrix/matrix or matrix/vector operation [%s]\n!" , tmp_s1 ); ReportError( ErrorStr ); return;
	  }
	}
      }

      tmp_s1 = strstr( tmp_s1 , " " ) + 1;
    }
  }
}

double dm_CCalculator::Calc( int *fileDepth , int *p_c  , 
			  int p_withSpaceCommentsInclude , 
			  char *charstring , int *ind , 
			  bool atRunTime ,
			  char * outputString ) {
  double resul = 0;
  char  chAux[XMLAttrValLength];
  
  symbolsStack.clear();
  valuesStack.clear();
  sizeStack.clear();
  
  outString[0] = 0;
  
  if( charstring ) {
//     printf( "Initial string [%s]\n", charstring );
  }

  ToPostfix( fileDepth , p_c  , p_withSpaceCommentsInclude , 
	     charstring , ind , atRunTime );
  
  // printf( "Postfixed scalar string [%s]\n", outString );

  Process( atRunTime , outputString );  
  
  // if the expression is a table, it is transformed into
  // a string, the concatenation of its values
  if( valuesStack.size() != 1 ) {
    while( valuesStack.size() > 0 ) {
      resul = valuesStack.back();
      valuesStack.pop_back();

      if( fabs(resul - (float)my_round((double)resul)) < 0.00000001 ) {
	sprintf( chAux , "%d " , (int)my_round((double)resul) );
      }
      else {
	sprintf( chAux , "%5f " , resul );
      }
      if( outputString 
	  && (strlen( outputString ) + strlen( chAux ) + 2 < XMLAttrValLength ) ) {
	strcat( chAux , outputString );
	strcpy( outputString , chAux );
      }
      else {
	break;
      }
    }
    //sprintf( ErrorStr , "Error: incorrect expression [%s] (depth %d)!" , outString , valuesStack.size() ); ReportError( ErrorStr ); throw 0;  
  }
  else {
    resul = valuesStack.back();
  }

  // not a number
  if( resul != resul ) {
      resul = 1000000000000.0;
  }

  return resul;
}

void dm_CCalculator::CalcTable( double *scalars , int size ,
				int *fileDepth , int *p_c  , 
				int p_withSpaceCommentsInclude , 
				char *charstring , int *ind , 
				bool atRunTime  ,
				char * outputString ) {
  symbolsStack.clear();
  valuesStack.clear();
  sizeStack.clear();
  
  outString[0] = 0;
  
  if( charstring ) {
    // printf( "CalcTable Initial string [%s] before ToPostfix\n", charstring );
  }

  ToPostfix( fileDepth , p_c  , p_withSpaceCommentsInclude , 
	     charstring , ind , atRunTime );
  
  //printf( "Postfixed table string [%s]\n", outString );

  Process( atRunTime , outputString );
  
  if( (int)valuesStack.size() != size ) {
    sprintf( ErrorStr , "Error: incorrect table or matrix size (%d/%d)! %s" , (int)valuesStack.size() , size , outString ); ReportError( ErrorStr ); return;
  }

  for( int indScaler = 0 ; indScaler < size ; indScaler++ ) {
    scalars[ size - indScaler - 1 ] = valuesStack.back();
    valuesStack.pop_back();
    //printf( "dm_Scalar [%.2f]\n", scalars[ size - indScaler - 1 ]  );
  }
}

double stringToDoubleValue( char *aString , bool atRunTime ) {
      int indAux = 1;
      int ch = *(aString);
      float resul = ReadExpression( NULL , &ch , true , aString ,  
				    &indAux , atRunTime , NULL );
      //printf( "Expression %.5f\n" , resul );
      return (double)resul;
}

void stringToTableDoubleValue( double *scalars , int size ,
			       char *aString , bool atRunTime ) {
      int indAux = 1;
      int ch = *(aString);
      // printf( "Expression %s\n" , aString );
      ReadExpression( scalars , size , NULL , &ch , true , aString , 
		      &indAux , atRunTime , NULL );
}

int stringToIntValue( char *aString  , bool atRunTime ) {
    int indAux = 1;
    int ch = *(aString);
    return (int)ReadExpression( NULL , &ch , true , aString , 
				 &indAux , atRunTime , NULL );
}

void stringToTableIntValue( int *scalars , int size ,
			    char *aString  , bool atRunTime ) {
    int indAux = 1;
    int ch = *(aString);
    ReadExpression( scalars , size , NULL , &ch , true , aString , 
		    &indAux , atRunTime , NULL );
}
void stringToTableBoolValue( bool *scalars , int size ,
			     char *aString  , bool atRunTime ) {
    int indAux = 1;
    int ch = *(aString);
    int *intScalars = new int[ size ];
    ReadExpression( intScalars , size , NULL , &ch , true , aString , 
		    &indAux , atRunTime , NULL );
    for( int ind = 0 ; ind < size ; ind++ ) {
      scalars[ ind ] = (bool)(intScalars[ ind ]);
    }
    delete [] intScalars;
	intScalars = NULL;
}

float    ReadExpression( int *fileDepth , int *p_c  ,
			 int p_withSpaceCommentsInclude , 
			 char *charstring , int *ind , bool atRunTime ,
			 char * outputString ) {
  dm_CCalculator * m_Calculator = new dm_CCalculator( charstring );
  float valreturn =  (float)m_Calculator->Calc( fileDepth , p_c  , p_withSpaceCommentsInclude , 
					charstring , ind , atRunTime , outputString  );
  delete m_Calculator;
  m_Calculator = NULL;

  return valreturn;
}

void    ReadExpression( int *scalars , int size ,
			int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			char *charstring , int *ind , bool atRunTime ,
			char * outputString ) {
  double *tempVal = new double[ size ];
  dm_CCalculator * m_Calculator = new dm_CCalculator( charstring );
  m_Calculator->CalcTable( tempVal , size , fileDepth ,
			  p_c  , p_withSpaceCommentsInclude , 
			  charstring , ind , atRunTime , outputString  );
  for( int ind = 0 ; ind < size ; ind++ ) {
    scalars[ ind ] = (int)tempVal[ ind ] ;
  }
  delete [] tempVal;
  tempVal = NULL;

  delete m_Calculator;
  m_Calculator = NULL;
}

void    ReadExpression( double *scalars , int size ,
			int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			char *charstring , int *ind , bool atRunTime ,
			char * outputString ) {
  // printf( "Initial string [%s]\n", charstring );
  dm_CCalculator * m_Calculator = new dm_CCalculator( charstring );
  // printf( "Initial string [%s]\n", charstring );
  m_Calculator->CalcTable( scalars , size , fileDepth , 
			   p_c  , p_withSpaceCommentsInclude , 
			   charstring , ind , atRunTime , outputString );
  delete m_Calculator;
  m_Calculator = NULL;
}


float ReferenceNodeAttributeValue( const char * attributeOrVariableString ,
				   bool atRunTime ,
				   char * outputString ) {
  char  attrOrVar[XMLAttrChLength];
  char  nodeId[XMLAttrValLength];
  dm_SceneNode *referenceNode = NULL;
  char *endOfId;
		    
  // separating file and node names
  // loading the file if necessary
  if( strchr( attributeOrVariableString , ':' ) ) {
    // attribute or variable (scalar or table)
    strcpy( attrOrVar , strchr( attributeOrVariableString , ':' ) + 1 );
    if( ( endOfId = strchr( attrOrVar , '}' ) ) ) {
      *endOfId = 0;
    }

    // node ID
    strcpy( nodeId , attributeOrVariableString + 1 );
    if( ( endOfId = strchr( nodeId , ':' ) ) ) {
      *endOfId = 0;
    }

    // if node ID is ENV: access to enviroment variables
    if( strcmp( nodeId + 1 , "ENV" ) == 0 ) {
      if( outputString ) {
	char * descr = getenv( attrOrVar );
	if (descr) {
	  // printf( "env [%s]\n" , descr );
	  strncat( outputString , descr , 
		   XMLAttrValLength - strlen( outputString ) - 1 );
	  return (float)atof( descr );
	}
      }
      return 0.f;
    }

    nodeId[ 0 ] = '#';

    int nbRefs = 0;
    dm_SceneNode **auxNode 
      = findReferencesDynamic( nodeId , &nbRefs , true , atRunTime );

//     if( auxNode ) {
//       printf("Nb discovered Nodes %d (%s)\n" , nbRefs , auxNode[0]->id );
//     }
//     else {
//       printf("No discovered Nodes (%s)\n" , nodeId );
//     }
    if( auxNode ) {
      // allocates a list of target nodes that
      // will be instantiated by the actual pointers in
      // the scene graph
      referenceNode = auxNode[ 0 ];
      // printf( "ReferenceNodeAttributeValue %s\n" , referenceNode->id );
	  
      // throws away the nodes that were allocated during
      // the resolution of the reference
      delete [] auxNode;
	  auxNode = NULL;
      
      // searches among the variables
      // scalar variables
      dm_Scalar *scalar = referenceNode->getScalarFromName( attrOrVar );
      // printf( "scalar %d\n" , scalar );
      if( scalar ) {
	if( scalar->GetType() == DataTypeDouble ) {
	  // printf( "ReferenceNodeAttributeValue string %s scalar double value %.5f\n" , attributeOrVariableString , scalar->valFloat );
	  return scalar->GetValue()->valFloat;
	}
	else if( scalar->GetType() == DataTypeInt ) {
	  // printf( "ReferenceNodeAttributeValue string %s scalar int value %d\n" , attributeOrVariableString , scalar->valInt );
	  return (float)scalar->GetValue()->valInt;
	}
	else if( scalar->GetType() == DataTypeString ) {
	  //printf( "ReferenceNodeAttributeValue scalar string value %s\n" , scalar->value.valString );
	  if( outputString ) {
	    strncat( outputString , scalar->GetValue()->valString , 
		     XMLAttrValLength - strlen( outputString ) - 1 );
	  }
	  return (float)atof( scalar->GetValue()->valString );
	}
      }
 
      // table variables
      dm_Table *table;
      char * arraySubscript;
      int indSubscript = -1;
      // matrix variables
      dm_Matrix *matrix;
      if( ( arraySubscript = strchr( attrOrVar , '[' ) ) ) {
	// beginning of subscript e.g. [3]
	// transformed into an array index (minus 1 for C indexing)
	indSubscript = atoi( arraySubscript + 1 ) - 1;
	// printf( "beginning of subscript %s (%d)\n" ,arraySubscript ,indSubscript);
	// removes subscript from variable name [3]
	*arraySubscript = 0;

	table = referenceNode->getTableFromName( attrOrVar );
	// printf( "table %s %d\n" ,attrOrVar,table );
	if( table && indSubscript < table->GetSize() ) {
	  DataType type = table->GetType();
	  if( type == DataTypeDouble || type == DataTypeInt ) {
	    // printf( "ReferenceNodeAttributeValue string %s table num value %.5f\n" , attributeOrVariableString , table->GetNumericalValue( indSubscript ) );
	    return table->GetNumericalValue( indSubscript );
	  }
	  else if( type == DataTypeString ) {
	    // printf( "ReferenceNodeAttributeValue string %s table string value %s\n" , attributeOrVariableString , table->GetStringValue( indSubscript ) );
	    if( outputString ) {
	      strncat( outputString , table->GetStringValue( indSubscript ) , 
		       XMLAttrValLength - strlen( outputString ) - 1 );
	    }
	    return (float)atof( table->GetStringValue( indSubscript ) );
	  }
	}

	matrix = referenceNode->getMatrixFromName( attrOrVar );
	// printf( "matrix %s %d\n" ,attrOrVar,matrix );
	if( matrix && indSubscript < matrix->GetSize() ) {
	  DataType type = matrix->GetType();
	  if( type == DataTypeDouble || type == DataTypeInt ) {
	    // printf( "ReferenceNodeAttributeValue string %s matrix num value %.5f\n" , attributeOrVariableString , matrix->GetNumericalValue( indSubscript ) );
	    return matrix->GetNumericalValue( indSubscript );
	  }
	  else if( type == DataTypeString ) {
	    // printf( "ReferenceNodeAttributeValue string %s matrix string value %s\n" , attributeOrVariableString , matrix->GetStringValue( indSubscript ) );
	    if( outputString ) {
	      strncat( outputString , matrix->GetStringValue( indSubscript ) , 
		       XMLAttrValLength - strlen( outputString ) - 1 );
	    }
	    return (float)atof( matrix->GetStringValue( indSubscript ) );
	  }
	}
      }

      // searches among the node attributes
      double            valDouble = 0.0; 
      int               valInt = 0; 
      bool              valBool = 0; 
      char            * valString; 
      dm_ValScalar    * valScalars = NULL; 
      int               size = 0;
      DataType          valDataType = EmptyDataType;

      // printf("attrOrVar (%s)\n" , attrOrVar );
      referenceNode->getParameters( attrOrVar , 
				    &valDataType ,
				    &valDouble ,
				    &valInt ,
				    &valBool ,
				    &valString ,
				    &size ,
				    &valScalars );
      if( valScalars != NULL ) {
	if( indSubscript != -1 ) {
	  // printf( "table size %d -> index %d\n" , size , indSubscript );
	  if( indSubscript < size ) {
	    if( valDataType == DataTypeDouble || valDataType == DataTypeInt ) {
	      return valScalars[ indSubscript ].GetNumericalValue();
	    }
	    else if( valDataType == DataTypeString ) {
	      if( outputString ) {
		strncat( outputString , 
			 valScalars[ indSubscript ].GetStringValue() , 
			 XMLAttrValLength - strlen( outputString ) - 1 );
	      }
	      return (float)atof( valScalars[ indSubscript ].GetStringValue() );
	    }
	  }
	}
	else {
	  sprintf( ErrorStr , "Error: non scalar parameter %s on node ID %s!" , attrOrVar , nodeId ); ReportError( ErrorStr ); return 0;
	}
      }

      if( valDataType == DataTypeDouble ) {
	// printf( "ReferenceNodeAttributeValue string %s attr double value %.5f\n" , attributeOrVariableString , valDouble );
	return (float)valDouble;
      }
      else if( valDataType == DataTypeInt ) {
	// printf( "ReferenceNodeAttributeValue string %s attr int value %d\n" , attributeOrVariableString , valInt );
	return (float)valInt;
      }
      else if( valDataType == DataTypeBool ) {
	// printf( "ReferenceNodeAttributeValue string %s attr bool value %d\n" , attributeOrVariableString , valBool );
	return (float)valBool;
      }
      else if( valDataType == DataTypeString ) {
	// printf( "ReferenceNodeAttributeValue string %s attr string value %s\n" , attributeOrVariableString , valString );
	if( outputString ) {
	  strncat( outputString , valString , 
		  XMLAttrValLength - strlen( outputString ) - 1 );
	}
	return (float)atof( valString );
      }
      // printf( "ReferenceNodeAttributeValue unknown valDataType %d\n" , valDataType );
      // fprintf( stderr , "Node (%s) attr (%s) nbRefs (%d) nbKnownTargets (%d)\n" , nodeId , attrOrVar , nbRefs , nbKnownTargets );

      // searches among the sceneObject attributes
      referenceNode->sceneObject->getParameters( attrOrVar , 
						 &valDataType ,
						 &valDouble ,
						 &valInt ,
						 &valBool ,
						 &valString ,
						 &size ,
						 &valScalars );

      // searches among the sceneObject material attributes
      dm_MaterialProperties *mat;
      if( valDataType == EmptyDataType 
	  && (mat = referenceNode->GetMaterial()) ) {
	mat->getParameters( attrOrVar , 
			    &valDataType ,
			    &valDouble ,
			    &valInt ,
			    &valBool ,
			    &valString ,
			    &size ,
			    &valScalars );
	// printf("Mat prop %s %f\n" , DataTypeStrings[valDataType] , valDouble );
	// printf("Node (%s)\n" , nodeId );

      }

      // searches among the sceneObject shader attributes
      dm_Shader *shad;
      if( valDataType == EmptyDataType 
	  && (shad = referenceNode->GetMaterialShader()) ) {
	shad->getParameters( attrOrVar , 
			     &valDataType ,
			     &valDouble ,
			     &valInt ,
			     &valBool ,
			     &valString ,
			     &size ,
			     &valScalars );
      }

      // scalar table
      if( valScalars != NULL ) {
	// printf( "scalar or table data type %d size %d -> index %d\n" , valDataType , size , indSubscript );
	if( size == 1 || (size > 1 && indSubscript != -1) ) {
	  if( size == 1 ) {
	    indSubscript = 0;
	  }
	  if( indSubscript < size ) {
	    if( valDataType == DataTypeDouble || valDataType == DataTypeInt ) {
	      return valScalars[ indSubscript ].GetNumericalValue();
	    }
	    else if( valDataType == DataTypeString ) {
	      if( outputString ) {
		strncat( outputString , 
			 valScalars[ indSubscript ].GetStringValue() , 
			 XMLAttrValLength - strlen( outputString ) - 1 );
	      }
	      return (float)atof( valScalars[ indSubscript ].GetStringValue() );
	    }
	  }
	  else {
	    sprintf( ErrorStr , "Error: excessive table index %d/%d %s on node ID %s!" , indSubscript , size , attrOrVar , nodeId ); ReportError( ErrorStr ); return 0;
	  }
	}
	else {
	  sprintf( ErrorStr , "Error: incorrect table index %d %s on node ID %s!" , indSubscript , attrOrVar , nodeId ); ReportError( ErrorStr ); return 0;
	}
      }

      if( valDataType == DataTypeDouble ) {
	// printf( "ReferenceNodeAttributeValue string %s attr double value %.5f\n" , attributeOrVariableString , valDouble );
	return (float)valDouble;
      }
      else if( valDataType == DataTypeInt ) {
	// printf( "ReferenceNodeAttributeValue string %s attr int value %d\n" , attributeOrVariableString , valInt );
	return (float)valInt;
      }
      else if( valDataType == DataTypeBool ) {
	// printf( "ReferenceNodeAttributeValue string %s attr bool value %d\n" , attributeOrVariableString , valBool );
	return (float)valBool;
      }
      else if( valDataType == DataTypeString ) {
	// printf( "ReferenceNodeAttributeValue string %s attr string value %s\n" , attributeOrVariableString , valString );
	if( outputString ) {
	  strncat( outputString , valString , 
		   XMLAttrValLength - strlen( outputString ) - 1 );
	}
	return (float)atof( valString );
      }
      // printf( "ReferenceNodeAttributeValue unknown valDataType %d\n" , valDataType );


      return 0;
      // fprintf( stderr , "Node (%s) attr (%s) nbRefs (%d) nbKnownTargets (%d)\n" , nodeId , attrOrVar , nbRefs , nbKnownTargets );
    }
//     else if( strcmp( nodeId , DM_EnvironmentNode->GetId() ) == 0 ) {
//       printf("Get value from environment (%s)\n" , attrOrVar );
//       // scalar environment variables: alt shift control
//       dm_Scalar *scalar = DM_EnvironmentNode->getScalarFromName( attrOrVar );
//       if( scalar ) {
// 	if( scalar->GetType() == DataTypeDouble ) {
// 	  // printf( "ReferenceNodeAttributeValue string %s scalar double value %.5f\n" , attributeOrVariableString , scalar->valFloat );
// 	  return scalar->GetValue()->valFloat;
// 	}
// 	else if( scalar->GetType() == DataTypeInt ) {
// 	  // printf( "ReferenceNodeAttributeValue string %s scalar int value %d\n" , attributeOrVariableString , scalar->valInt );
// 	  return (float)scalar->GetValue()->valInt;
// 	}
// 	else if( scalar->GetType() == DataTypeString ) {
// 	  //printf( "ReferenceNodeAttributeValue scalar string value %s\n" , scalar->value.valString );
// 	  if( outputString ) {
// 	    strncat( outputString , scalar->GetValue()->valString , 
// 		     XMLAttrValLength - strlen( outputString ) - 1 );
// 	  }
// 	  return (float)atof( scalar->GetValue()->valString );
// 	}
//       }

//       return 0;
//     }
    else {
      // printf( "ReferenceNodeAttributeValue unknown attribute or var\n" );
      return 0;
    }
	
  }
  // OSC parameters that are received from companion applications
  else if( isdigit( attributeOrVariableString[2] ) ) {
    int ind = 3;
    while( attributeOrVariableString[ind] != '}' ) {
      if( !isdigit( attributeOrVariableString[ind] ) ) {
	sprintf( ErrorStr , "Error: incorrect OSC parameter string (%s) {$[0-9]+} expected!\n" , attributeOrVariableString ); ReportError( ErrorStr ); return 0;
	return 0;
      }
      ind++;
    }
    // attributeOrVariableString == {$xxx}
    // numbered from 1 to n
    int indOSCParam = atoi( attributeOrVariableString + 2 ) - 1;
    //printf( "OSC param rank %d\n" , indOSCParam + 1 );
    if( DM_CompositionNode->curentNbOSCParams > 0 && 
	indOSCParam < DM_CompositionNode->curentNbOSCParams ) {
      dm_ValScalar *curScalar 
	= DM_CompositionNode->curentOSCParams + indOSCParam;
      if( curScalar->type == DataTypeDouble ) {
	// printf( "ReferenceNodeAttributeValue string %s attr double value %.5f rank %d\n" , attributeOrVariableString , curScalar->valFloat , indOSCParam + 1 );
	return (float)curScalar->valFloat;
      }
      else if( curScalar->type == DataTypeInt ) {
	// printf( "ReferenceNodeAttributeValue string %s attr int value %d rank %d\n" , attributeOrVariableString , curScalar->valInt , indOSCParam + 1 );
	return (float)curScalar->valInt;
      }
      else if( curScalar->type == DataTypeBool ) {
	// printf( "ReferenceNodeAttributeValue string %s attr bool value %d rank %d\n" , attributeOrVariableString , curScalar->valInt , indOSCParam + 1 );
	return (float)curScalar->valInt;
      }
      else if( curScalar->type == DataTypeString ) {
	//printf( "ReferenceNodeAttributeValue string %s attr string value %s rank %d\n" , attributeOrVariableString , curScalar->valString , indOSCParam + 1 );
	if( outputString ) {
	  strncat( outputString , curScalar->valString , 
		   XMLAttrValLength - strlen( outputString ) - 1 );
	}
	return (float)atof( curScalar->valString );
      }
    }
    return 0;
  }
  else {
    sprintf( ErrorStr , "Error: incorrect parameter string (%s) <name>:<attribute> expected!\n" , attributeOrVariableString ); ReportError( ErrorStr ); return 0;
    return 0;
  }
}

dm_ValScalar * ReferenceNodeAttributeTable( const char * attributeOrVariableString ,
					    bool atRunTime ,
					    int * tableOrMatrixSize ,
					    DataType * tableOrMatrixType ) {
  char              attrOrVar[XMLAttrChLength];
  char              nodeId[XMLAttrValLength];
  dm_SceneNode     *referenceNode = NULL;
  char             *endOfId;
		    
  // separating file and node names
  // loading the file if necessary
  if( strchr( attributeOrVariableString , ':' ) ) {
    // attribute or variable (scalar or table)
    strcpy( attrOrVar , strchr( attributeOrVariableString , ':' ) + 1 );
    if( ( endOfId = strchr( attrOrVar , '}' ) ) ) {
      *endOfId = 0;
    }

    // node ID
    strcpy( nodeId , attributeOrVariableString + 1 );
    if( ( endOfId = strchr( nodeId , ':' ) ) ) {
      *endOfId = 0;
    }
    nodeId[ 0 ] = '#';

    int nbRefs = 0;
    dm_SceneNode **auxNode 
      = findReferencesDynamic( nodeId , &nbRefs , true , atRunTime );

//     if( auxNode ) {
//       printf("Nb discovered Nodes %d (%s)\n" , nbRefs , auxNode[0]->id );
//     }
//     else {
//       printf("No discovered Nodes (%s)\n" , nodeId );
//     }
    if( auxNode ) {
      // allocates a list of target nodes that
      // will be instantiated by the actual pointers in
      // the scene graph
      referenceNode = auxNode[ 0 ];
	  
      // throws away the nodes that were allocated during
      // the resolution of the reference
      delete [] auxNode;
	  auxNode = NULL;
      
      // searches among the variables
      // table variables
      dm_Table *table = referenceNode->getTableFromName( attrOrVar );
      if( table ) {
	*tableOrMatrixType = table->GetType();
	*tableOrMatrixSize = table->GetSize();
	return table->GetScalars();
      }
      
      // matrix variables
      dm_Matrix *matrix = referenceNode->getMatrixFromName( attrOrVar );
      if( matrix ) {
	*tableOrMatrixType = matrix->GetType();
	*tableOrMatrixSize = matrix->GetSize();
	return matrix->GetScalars();
      }

      // searches among the node attributes
      double valDouble = 0.0; int valInt = 0; bool valBool = 0; 
      char * valString; dm_ValScalar * valScalars = NULL; 
      *tableOrMatrixType = EmptyDataType;
      // printf("attrOrVar (%s)\n" , attrOrVar );
      referenceNode->getParameters( attrOrVar , 
				    tableOrMatrixType ,
				    &valDouble ,
				    &valInt ,
				    &valBool ,
				    &valString ,
				    tableOrMatrixSize ,
				    &valScalars );

      if( valScalars != NULL ) {
	// printf( "ReferenceNodeAttributeValue string %s attr double value %.5f\n" , attributeOrVariableString , valDouble );
	
	return valScalars;
      }

      // searches among the sceneObject attributes
      // printf("attrOrVar (%s)\n" , attrOrVar );
      referenceNode->sceneObject->getParameters( attrOrVar , 
						 tableOrMatrixType ,
						 &valDouble ,
						 &valInt ,
						 &valBool ,
						 &valString ,
						 tableOrMatrixSize ,
						 &valScalars );

      if( valScalars != NULL ) {
	// printf( "ReferenceNodeAttributeValue string %s attr double value %.5f\n" , attributeOrVariableString , valDouble );
	return valScalars;
      }

      // searches among the sceneObject shader attributes
      dm_Shader *shad;
      if( (shad = referenceNode->GetMaterialShader()) ) {
	shad->getParameters( attrOrVar , 
			     tableOrMatrixType ,
			     &valDouble ,
			     &valInt ,
			     &valBool ,
			     &valString ,
			     tableOrMatrixSize ,
			     &valScalars );
      }

      if( valScalars != NULL ) {
	// printf( "ReferenceNodeAttributeTable %s value %.5f size %d\n" , attributeOrVariableString , valScalars[0].GetNumericalValue() , *tableOrMatrixType );
	
	return valScalars;
      }

    }
    else {
      // printf( "ReferenceNodeAttributeValue unknown attribute or var\n" );
      return NULL;
    }
	
  }
  else {
    fprintf( stderr , "Error: incorrect parameter string (%s) <name>:<attribute> expected!\n" , attributeOrVariableString );
    return NULL;
  }
  return NULL;
}

float    ReadFloat( int *fileDepth , int *p_c  ,
		   int p_withSpaceCommentsInclude , 
		   char *charstring , int *ind ,
		   bool atRunTime ) {
  float           resul, dec = 0.1F;
  short           neg = 0;

  // arithmetic expression
  if( *p_c == '(') {
    return ReadExpression( fileDepth , p_c  , p_withSpaceCommentsInclude , charstring , ind , atRunTime , NULL );
  }

  // parameter value
  if( *p_c == '{') {
    char storedCharstring[XMLAttrValLength];

    if( !ReadBalancedStringWithFrontiers( fileDepth , p_c , storedCharstring + 1 ,
				  XMLAttrValLength , '{' , '}' , 
				  true , false , charstring,ind ) ) {
      sprintf( ErrorStr , "Error: incorrect parameter name!" ); ReportError( ErrorStr ); return 0;
    }

    // checks whether the parameter value contains an expression that
    // must be recursively evaluated before 
    // evaluating the parameter
    if( strchr( storedCharstring + 1 , '(' ) ) {
      char substitutedCharstring[XMLAttrValLength];
      StrCpyAndSubst( substitutedCharstring ,
		      storedCharstring + 1 , 
		      XMLAttrValLength );
      strcpy( storedCharstring + 1 , substitutedCharstring );
    }

    storedCharstring[0] = '{';
    int stringLength = strlen(storedCharstring);
    storedCharstring[stringLength] = '}';
    storedCharstring[stringLength + 1] = 0;
    
    // string corresponding to a parameter
    // the # character is delected and replaced by a {
    // the string is terminated by a }
    if( storedCharstring[1] == '#' ) {
      return (float)atof( ParameterValue( storedCharstring ) );
    }
    // string corresponding to a parameter
    // the string begins by "{$"
    // the string is terminated by a }
    else if( storedCharstring[1] == '$' ) {
      if( !atRunTime ) {
	sprintf( ErrorStr , "Error: incorrect string (%s): variables and attribute values can only be used at run-time (in scripts)!" , storedCharstring ); stringLength = 2; ReportError( ErrorStr );
      }
      
      storedCharstring[stringLength] = 0; 
      return ReferenceNodeAttributeValue( storedCharstring , 
					  atRunTime , NULL );
    }
    else {
      sprintf( ErrorStr , "Error: incorrect string (%s): {#name} or {$name:attribute} expected!" , storedCharstring ); ReportError( ErrorStr );
      return 0;
    }
  }

  if( *p_c == '-' ) {
    neg = 1;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  else if( *p_c == '+' ) {
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  resul = (float) ReadInteger( fileDepth , p_c , false ,charstring,ind );
  if( *p_c == '.') {
    *p_c = ReadChar(fileDepth,charstring,ind);

    while ( _Num(*p_c) ) {
      resul += dec * (*p_c - '0');
      dec *= 0.1F;
      *p_c = ReadChar(fileDepth,charstring,ind);
    }
  }
  // exponent
  if(  *p_c == 'e' ) {
    *p_c = ReadChar(fileDepth,charstring,ind);
    float exponent = (float)ReadInteger( fileDepth , p_c , false ,charstring,ind );
    resul *= powf( 10.0 , exponent );
 }
  if( p_withSpaceCommentsInclude ) {
    SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
  }
  if( neg == 1)
    resul = -resul;
  return resul;
}

void    ReadInterval( int *fileDepth , int *p_c  ,
			int p_withSpaceCommentsInclude , 
			float *inf , float *sup , bool atRunTime ) {
    ExpectsChar( fileDepth , '[' , 
		 p_c , true , NULL , 0 );
    *inf = ReadFloat( fileDepth , 
		      p_c  , true , NULL , 0 , atRunTime );
    ExpectsChar( fileDepth , ',' , 
		 p_c , true , NULL , 0 );
    *sup = ReadFloat( fileDepth ,
		      p_c  , true , NULL , 0 , atRunTime );
    ExpectsChar( fileDepth , ']' , 
		 p_c , p_withSpaceCommentsInclude , NULL , 0 );
    if( inf > sup ) {
      inf = sup;
    }
}

int     ExpectsChar( int *fileDepth , 
		     char *p_modele , int *p_c , 
		     int p_withSpaceCommentsInclude , 
		     char *charstring , int *ind ) {
  int             i = 0;

  while( i < (int)strlen( p_modele ) && p_modele[ i ] == *p_c ) {
    i++;
    *p_c = ReadChar(fileDepth,charstring,ind);
  }

  if( i < (int)strlen( p_modele ) ) {
    sprintf( ErrorStr , "Expected string [%s] not found!" , p_modele ); ReportError( ErrorStr ); throw 17;
  }

  if( p_withSpaceCommentsInclude ) {
    SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
  }
  return true;
}

int     ExpectsChar( int *fileDepth , int p_modele , int  *p_c ,
		     int p_withSpaceCommentsInclude  , 
		     char *charstring , int *ind ) {
  if( p_modele == *p_c ) {
    *p_c = ReadChar(fileDepth,charstring,ind);
  }
  else {
    sprintf( ErrorStr , "Expected character [%c] not found (%c)!" , p_modele , *p_c ); 
    ReportError( ErrorStr ); 
  }
  if( p_withSpaceCommentsInclude ) {
    SpaceCommentsInclude( fileDepth , p_c , charstring,ind);
  }
  return true;
}

void MemoryUsage( void ) {
#ifndef _WIN32
  // struct rusage {
  //   struct timeval ru_utime; /* user time used */
  //   struct timeval ru_stime; /* system time used */
  //   long   ru_maxrss;        /* maximum resident set size */
  //   long   ru_ixrss;         /* integral shared memory size */
  //   long   ru_idrss;         /* integral unshared data size */
  //   long   ru_isrss;         /* integral unshared stack size */
  //   long   ru_minflt;        /* page reclaims */
  //   long   ru_majflt;        /* page faults */
  //   long   ru_nswap;         /* swaps */
  //   long   ru_inblock;       /* block input operations */
  //   long   ru_oublock;       /* block output operations */
  //   long   ru_msgsnd;        /* messages sent */
  //   long   ru_msgrcv;        /* messages received */
  //   long   ru_nsignals;      /* signals received */
  //   long   ru_nvcsw;         /* voluntary context switches */
  //   long   ru_nivcsw;        /* involuntary context switches */
  // };
  
  struct rusage usage;
  
  getrusage( RUSAGE_SELF , &usage );
  
  printf( "Memory usage %ld %ld %ld %ld\n" , usage.ru_maxrss , usage.ru_ixrss , usage.ru_idrss , usage.ru_isrss );
#endif
}
