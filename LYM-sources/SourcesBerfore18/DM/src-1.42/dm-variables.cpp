/*! \file dm-variables.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-variables.cpp
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

const char *VariableTypeString[EmptyVariableType + 1] = { "scalar" , "table" , "matrix" , "emptyvariabletype" };
const char *SmoothingFunctionString[EmptySmoothingFunction + 1] = { "spline" , "polynom" , "emptysmoothingfunction" };


dm_Variable::dm_Variable( void ) {
  id = NULL;
  InitString( &id );
  motherVariable = NULL;
  interpolatorVariable = true;
  nextVariable = NULL;
}

dm_Variable::~dm_Variable( void ) {
  if (id) {
    delete [] id;
    id = NULL;
  }
  /*
    if (motherVariable) {
    delete motherVariable;
    motherVariable = NULL;
    }
  */
  if (nextVariable) {
    delete nextVariable;
    nextVariable = NULL;
  }
}

void dm_Variable::print( FILE *file , int depth ) {
  sprintf( ErrorStr , "Error: print not implemented for dm_Variable!" ); ReportError( ErrorStr ); throw 53;
}

bool dm_Variable::getParameters( char *attribute , 
				 DataType * dataType ,
				 double * valDouble ,
				 int * valInt ,
				 bool * valBool ,
				 char ** valString ,
				 int * tableOrMatrixSize ,
				 dm_ValScalar ** valScalars ) {
  sprintf( ErrorStr , "Error: getParameters not implemented for dm_Variable!" ); ReportError( ErrorStr ); throw 53;
}

void dm_Variable::interpolate( dm_Variable **tabVariables , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef ) {
  sprintf( ErrorStr , "Error: interpolate not implemented for dm_Variable!" ); ReportError( ErrorStr ); throw 53;
}

void dm_Variable::interpolate( dm_Variable *n1 , dm_Variable *n2 ) {
  sprintf( ErrorStr , "Error: interpolate not implemented for dm_Variable!" ); ReportError( ErrorStr ); throw 53;
}

void dm_Variable::operator=(dm_Variable& s)
{
  if (s.id) {
    if (id)
      {
	delete [] id;
	id = NULL;
      }
    StrCpy(&id, s.id);
  }
  
  interpolatorVariable = s.interpolatorVariable;
  nextVariable = s.nextVariable;
  motherVariable = s.motherVariable;

  /*
    if (nextVariable) {
    delete nextVariable;
    nextVariable = NULL;
    }
    
    if (s.nextVariable) {
    nextVariable = new dm_Variable;
    *nextVariable = *s.nextVariable;
    nextVariable->nextVariable = NULL;
    nextVariable->motherVariable = this;
    }
  */
}

dm_ValScalar::dm_ValScalar( void ) {
  clean();
}

dm_ValScalar::~dm_ValScalar( void ) {
  if (valString) {
    delete [] valString;
    valString = NULL;
  }
  
  if (valHRef) {
    delete [] valHRef;
    valHRef = NULL;
  }

  if (smoothingValues) {
    delete [] smoothingValues;
    smoothingValues = NULL;
  }

  if (smoothingTimes) {
    delete [] smoothingTimes;
    smoothingTimes = NULL;
  }
}

void dm_ValScalar::clean( void ) {
  type = DataTypeDouble;
  valFloat = 0.f;
  valInt = 0;
  materialTexture = NULL;
  smoothingDepth = 0;
  valString = NULL;
  valHRef = NULL;
  smoothingValues = NULL;
  smoothingTimes = NULL;
}
DataType dm_ValScalar::GetType( void ) {
  return type;
}
void dm_ValScalar::SetType( DataType aType ) {
  type = aType;
}
float  dm_ValScalar::GetNumericalValue( void ) {
  if( type == DataTypeDouble ) {
    return valFloat;
  }
  else if( type == DataTypeInt ) {
    return (float)valInt;
  }
  else if( type == DataTypeTexture ) {
    return (float)materialTexture->GetTextureNo();
  }
  else if( valString ) {
    return (float)atof( valString );
  }
  return 0.f;
}
void   dm_ValScalar::SetValue( float val ) {
  // smoothing (only for float values)
  if( smoothingValues ) {
  }
  else {
    if( type == DataTypeDouble ) {
      valFloat = val;
    }
    else if( type == DataTypeInt ) {
      valInt = (int)val;
    }
    if( !valString ) {
      InitString( &valString , NumberMaximalLength );
    }
#ifdef _WIN32
    _snprintf( valString , NumberMaximalLength , "%.5f" , val );
#else
    snprintf( valString , NumberMaximalLength , "%.5f" , val );
#endif
  }
}
char *dm_ValScalar::GetStringValue( void ) {
  if( !valString ) {
    InitString( &valString , NumberMaximalLength );
  }
  if( type == DataTypeDouble ) {
#ifdef _WIN32
    _snprintf( valString , NumberMaximalLength , "%.5f" , valFloat );
#else
    snprintf( valString , NumberMaximalLength , "%.5f" , valFloat );
#endif
  }
  else if( type == DataTypeInt ) {
#ifdef _WIN32
    _snprintf( valString , NumberMaximalLength , "%.d" , valInt );
#else
    snprintf( valString , NumberMaximalLength , "%.d" , valInt );
#endif
  }
  return valString;
}

void dm_ValScalar::print( FILE *file ) {
  if( type == DataTypeDouble ) {
    fprintf( file , "type=\"%s\" value=\"%.5f\" " , 
	     DataTypeStrings[type] , valFloat );
  }
  else if( type == DataTypeInt ) {
    fprintf( file , "type=\"%s\" value=\"%d\" " , 
	     DataTypeStrings[type] , valInt );
  }
  else if( type == DataTypeTexture ) {
    fprintf( file , "type=\"%s\" " , 
	     DataTypeStrings[type] );
  }
  else if( type == DataTypeString && valString ) {
    if( valHRef ) {
      fprintf( file , "type=\"%s\" xlink:href=\"%s\" value=\"%s\" " , 
	       DataTypeStrings[type] , valHRef , valString );
    }
    else {
      fprintf( file , "type=\"%s\" value=\"%s\" " , 
	       DataTypeStrings[type] , valString );
    }
  }
}

void   dm_ValScalar::SetValue( char *val ) {
  StrCpy( &valString , val );
  if( type == DataTypeDouble ) {
     valFloat = (float)atof( valString );
  }
  else if( type == DataTypeInt ) {
     valInt = atoi( valString );
  }
}
void dm_ValScalar::operator=(dm_ValScalar& s) {
  StrCpy( &valString , s.valString );
  valFloat = s.valFloat;
  valInt = s.valInt;
  materialTexture = s.materialTexture;
  type = s.type;
  smoothingDepth = s.smoothingDepth;

  if (smoothingValues) {
    delete [] smoothingValues;
    smoothingValues = NULL;
  }
  
  if (s.smoothingValues) {
    smoothingValues = new float[s.smoothingDepth];
    for (int ind =0; ind < s.smoothingDepth; ind++) {
      smoothingValues[ind] = s.smoothingValues[ind];
    }
  }

  if (smoothingTimes) {
    delete [] smoothingTimes;
    smoothingTimes = NULL;
  }

  if (s.smoothingTimes) {
    smoothingTimes = new float[s.smoothingDepth];
    for (int ind =0; ind < s.smoothingDepth; ind++) {
      smoothingTimes[ind] = s.smoothingTimes[ind];
    }
  }
}

dm_Scalar::dm_Scalar( void ): dm_Variable() {
  // save this scalar to the deletion list
  this->chineScalar = deletedScalarList;
  deletedScalarList = this;
  
  value = new dm_ValScalar();
  nextVariable = NULL;
  interp = NULL;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Scalar::dm_Scalar( dm_VariableInterpolator *interpolator ): dm_Variable() {
  // save this scalar to the deletion list
  this->chineScalar = deletedScalarList;
  deletedScalarList = this;

  value = new dm_ValScalar();
  *value = *(((dm_Scalar *)(interpolator->curVariable))->GetValue() );
  nextVariable = NULL;
   
  interp = interpolator;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Scalar::~dm_Scalar( void ) {
  if (value) {
    delete value;
    value = NULL;
  }
  
  // this pointer refers to another scene object in the scene
  // should not be deallocated otherwise the corresponding
  // object will be deallocated twice
  // the interpolator will be released by releaseSceneObjects
  // (an interpolator is a non geometrical scene object)

  //   if (interp) {
  //     delete interp;
  //   }

  if (nextVariable) {
    delete (dm_Scalar*) nextVariable;
    nextVariable = NULL;
    motherVariable = NULL;
  }
}

// void dm_Scalar::operator=(dm_Scalar& n) {
//   if (n.id) {
//     if (id)
//       {
// 	delete [] id;
// 	id = NULL;
//       }
//     StrCpy(&id, n.id);
//   }
  
//   interpolatorVariable = n.interpolatorVariable;
  
//   this->dm_Variable::operator=(n);
  
//   isInterpolator = n.isInterpolator;
//   smoothingDepth = n.smoothingDepth;
//   smoothingFunction = n.smoothingFunction;
  
//   if (interp) {
//     delete interp;
//     interp = NULL;
//   }
  
//   if (n.interp) {
//     interp = new dm_VariableInterpolator(n.interp->nbComponents);
//     *interp = *n.interp;
//   }
  
//   if (value) {
//     delete value;
//     value = NULL;
//   }
  
//   if (n.value) {
//     value = new dm_ValScalar;
//     *value = *n.value;
//   }

//   nextVariable = n.nextVariable;
// }


dm_ValScalar *dm_Scalar::GetValue( void ) {
  if( !interp )
    return value;
  else 
    return ((dm_Scalar *)(interp->curVariable))->value;
}
DataType dm_Scalar::GetType( void ) {
  return GetValue()->GetType();
}
void dm_Scalar::SetType( DataType aType ) {
  if( !interp )
    value->SetType( aType );
  else {
    sprintf( ErrorStr , "Error: interpolated variable (scalar), type modification violation!" ); ReportError( ErrorStr ); throw 11;
  }
}
float  dm_Scalar::GetNumericalValue( void ) {
  return GetValue()->GetNumericalValue();
}
void   dm_Scalar::SetValue( float val ) {
  GetValue()->SetValue( val );
}
char *dm_Scalar::GetStringValue( void ) {
  return GetValue()->GetStringValue();
}
void   dm_Scalar::SetValue( char *val ) {
  GetValue()->SetValue( val );
}
char *dm_Scalar::GetId( void ) {
  if( !interp )
    return id;
  else 
    return ((dm_Scalar *)(interp->curVariable))->id;
}
void dm_Scalar::SetId( const char *newId ) {
  if( !interp )
    StrCpy( &id , newId );
  else {
    sprintf( ErrorStr , "Error: interpolated variable (scalar), ID modification violation!" ); ReportError( ErrorStr ); throw 11;
  }
}

void dm_Scalar::print( FILE *file , int depth ) {
  if( !interp ) {
    indent( file , depth );
    fprintf( file , "<scalar id=\"%s\" " , id );
    value->print( file );
    fprintf( file , "smoothingFunction=\"%s\" smoothingDepth=\"%d\" />\n" , 
	     SmoothingFunctionString[smoothingFunction] , smoothingDepth );
  }
  else {
    interp->print( file , true , depth );
  }
}

void dm_Scalar::setScalar( dm_XMLTag *XMLtag , bool atRunTime ,
			   FloatBinOp oper , IntBinOp operI ) {
  dm_ValScalar  *val = GetValue();
  if( val->type == DataTypeInt ) {
    operI(  val->valInt
	    , XMLtag->getAttributeIntValue( "value" , atRunTime ) );
  }
  else if( val->type == DataTypeDouble ) {
    oper(  val->valFloat
	   , (float)XMLtag->getAttributeDoubleValue( "value" , atRunTime ) );
  }
  else if( val->type == DataTypeString ) {
    StrCpyAndSubst( &(val->valString) 
		    , XMLtag->getAttributeStringValue( "value" ) );
    //printf( "set value (%s)\n" , val->valString );
  }
}

void dm_Scalar::setScalarRefString( dm_XMLTag *XMLtag ) {
  dm_ValScalar  *val = GetValue();
  if( val->type == DataTypeString ) {
    char  chAux[XMLAttrValLength];
    FILE *file = fopen( XMLtag->getAttributeStringValue( "xlink:href" ) , "rb" );
    if( !file ) {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: variable value file [%s] not found!" , XMLtag->getAttributeStringValue( "xlink:href" ) ); ReportError( ErrorStr ); throw 11;
    }
    
    int c;
    int indChar = 0;
    while( (c = getc( file ) ) != EOF && indChar < XMLAttrValLength - 1 ) {
      chAux[indChar ] = c;
      indChar++;
    }
    chAux[indChar ] = 0;
    fclose( file );
    
    StrCpy( &(val->valString) , chAux );
    //printf( "set value (%s)\n" , val->valString );
  }
}

void dm_Scalar::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			       FloatBinOp oper ,
			       IntBinOp operI ) {
  dm_ValScalar  *val = GetValue();
  if( !atRunTime 
      || XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "type" ) ) {
    char *thetype = XMLtag->getAttributeStringValue( "type" );
    // default type is "float"
    val->type = EmptyDataType;
    if( strcmp( thetype , "" ) == 0 ) {
      val->type = DataTypeDouble; 
    }
    else {
      for( int ind = 0 ; ind < EmptyDataType ; ind++ ) {
	if( strcmp( thetype , DataTypeStrings[ind] ) == 0 ) {
	  val->type = (DataType)ind;
	  break;
	}
      }
      if( val->type == EmptyDataType ) {
	sprintf( ErrorStr , "Error: unknown scalar type [%s]!" , thetype ); ReportError( ErrorStr ); throw 249;
      }
    }
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingFunction" ) ) {
    char *thesmoothingFunction = XMLtag->getAttributeStringValue( "smoothingFunction" );
    // default smoothingFunction is "spline"
    smoothingFunction = EmptySmoothingFunction;
    if( strcmp( thesmoothingFunction , "" ) == 0 ) {
      smoothingFunction = dm_Spline; 
    }
    else {
      for( int ind = 0 ; ind < EmptySmoothingFunction ; ind++ ) {
	if( strcmp( thesmoothingFunction , SmoothingFunctionString[ind] ) == 0 ) {
	  smoothingFunction = (SmoothingFunction)ind;
	  break;
	}
      }
      if( smoothingFunction == EmptySmoothingFunction ) {
	sprintf( ErrorStr , "Error: unknown scalar smoothingFunction [%s]!" , thesmoothingFunction ); ReportError( ErrorStr ); throw 249;
      }
    }
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingDepth" ) ) {
    // default smoothingDepth is 0 (no smoothing)
    smoothingDepth = (int)XMLtag->getAttributeDoubleValue( "smoothingDepth" , false );
    if( smoothingFunction == dm_Spline && smoothingDepth != 0 && smoothingDepth != 4 ) {
      sprintf( ErrorStr , "Error: only 4 point spline smoothing [%d]!" , smoothingDepth ); ReportError( ErrorStr ); throw 249;
    }
    if( smoothingDepth > 0 ) {
      if( val->type != DataTypeDouble ) {
	sprintf( ErrorStr , "Error: only float scalars can be smoothed!" ); ReportError( ErrorStr ); throw 249;
      }
      else {
	val->smoothingValues = new float[ smoothingDepth ];
	val->smoothingTimes = new float[ smoothingDepth ];
	val->smoothingDepth = smoothingDepth;
      }
    }
  }

  if( !atRunTime ) {
    val->valInt = 0;
    val->valFloat = 0.f;
    if( val->valString ) {
      *val->valString = 0;
    }
  }
  if( XMLtag->hasAttribute( "value" ) ) {
    setScalar( XMLtag , atRunTime , oper , operI );
  }
  if( XMLtag->hasAttribute( "xlink:href" ) ) {
    setScalarRefString( XMLtag );
  }
  if( val->type == DataTypeTexture ) {
    // allocates the texture
    if( !val->materialTexture ) {
      val->materialTexture = new dm_Texture();
      val->materialTexture->encoding = RAW;
      val->materialTexture->textureTarget = GL_TEXTURE_2D;
      val->materialTexture->nb_attachments = 1;

      if( !atRunTime ) {
	val->materialTexture->rectangle = false;
      }
      if( XMLtag->hasAttribute( "rectangle" ) ) {
	XMLtag->BooleanFieldValue( "rectangle" ,
				   &(val->materialTexture->rectangle) );
      }

      if( !atRunTime ) {
	val->materialTexture->mipmapped = true;
      }
      if( XMLtag->hasAttribute( "mipmapped" ) ) {
	XMLtag->BooleanFieldValue( "mipmapped" ,
				   &(val->materialTexture->mipmapped) );
      }

      // nb of texture attachments (for FBO textures)
      if( !atRunTime ) {
	val->materialTexture->nb_attachments = 1;
      }
      if( XMLtag->hasAttribute( "nb_attachments" ) ) {
	operI( val->materialTexture->nb_attachments , 
	       XMLtag->getAttributeIntValue( "nb_attachments" , 
					     atRunTime ) );
      }
      GLint maxbuffers;
      glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxbuffers);

      // if( val->materialTexture->nb_attachments > maxbuffers ) {
      // 	sprintf( ErrorStr , "Error: texture scalar [%s] has excessive number of attachments [%d] current hardware only allows [%d]!" , id , val->materialTexture->nb_attachments , maxbuffers ); ReportError( ErrorStr ); throw 127;
      // }

       // texture format
      if( !atRunTime ) {
	val->materialTexture->texture_format = byte_tex_format;
      }
      if( XMLtag->hasAttribute( "texture_format" ) ) {
	char *texture_format_str = XMLtag->getAttributeStringValue( "texture_format" );
	val->materialTexture->texture_format = EmptyTextureFormat;
	for( int ind = 0 ; ind < EmptyTextureFormat ; ind++ ) {
	  if( strcmp( texture_format_str , TextureFormatString[ind] ) == 0 ) {
	    val->materialTexture->texture_format = (TextureFormat)ind;
	    break;
	  }
	}
	if( val->materialTexture->texture_format == EmptyTextureFormat ) {
	  sprintf( ErrorStr , "Error: unknown texture format [%s]!" , texture_format_str ); ReportError( ErrorStr ); throw 127;
	}
      }

      // texture format
      if( !atRunTime ) {
	val->materialTexture->texture_min_filter = linear_filter;
	val->materialTexture->texture_max_filter = linear_filter;
      }
      if( XMLtag->hasAttribute( "texture_min_filter" ) ) {
	char *texture_filter_str = XMLtag->getAttributeStringValue( "texture_min_filter" );
	val->materialTexture->texture_min_filter = EmptyTextureFilter;
	for( int ind = 0 ; ind < EmptyTextureFilter ; ind++ ) {
	  if( strcmp( texture_filter_str , TextureFilterString[ind] ) == 0 ) {
	    val->materialTexture->texture_min_filter = (TextureFilter)ind;
	    break;
	  }
	}
	if( val->materialTexture->texture_min_filter == EmptyTextureFilter ) {
	  sprintf( ErrorStr , "Error: unknown texture filter [%s]!" , texture_filter_str ); ReportError( ErrorStr ); throw 127;
	}
      }
      if( XMLtag->hasAttribute( "texture_max_filter" ) ) {
	char *texture_filter_str = XMLtag->getAttributeStringValue( "texture_max_filter" );
	val->materialTexture->texture_max_filter = EmptyTextureFilter;
	for( int ind = 0 ; ind < EmptyTextureFilter ; ind++ ) {
	  if( strcmp( texture_filter_str , TextureFilterString[ind] ) == 0 ) {
	    val->materialTexture->texture_max_filter = (TextureFilter)ind;
	    break;
	  }
	}
	if( val->materialTexture->texture_max_filter == EmptyTextureFilter ) {
	  sprintf( ErrorStr , "Error: unknown texture filter [%s]!" , texture_filter_str ); ReportError( ErrorStr ); throw 127;
	}
      }
      
      // texture dimension
      if( !atRunTime ) {
	val->materialTexture->textureTarget = GL_TEXTURE_2D;
	if( XMLtag->hasAttribute( "dimension" ) ) {
	  int dimension = XMLtag->getAttributeIntValue( "dimension" , false );
	  if( dimension == 1 ) {
	    val->materialTexture->textureTarget = GL_TEXTURE_1D;
	  }
	  else if( dimension == 2 ) {
	    val->materialTexture->textureTarget = GL_TEXTURE_2D;
	  }
	  else if( dimension == 3 ) {
	    val->materialTexture->textureTarget = GL_TEXTURE_3D;
	  }
	}
      }

      val->materialTexture
	->SetTextureNo(DM_CompositionNode
		       ->GenerateTexture(val->materialTexture->rectangle,
					 val->materialTexture->texture_format,
					 val->materialTexture->nb_attachments));
      strcpy( DM_CompositionNode->texture_names[
			val->materialTexture->GetTextureNo() ] , id );
      // printf( "Allocate FRAME_BUFFER variable No [%d] ID [%d] rectangle [%d]  nb_attachments %d\n" , val->materialTexture->GetTextureNo() , DM_CompositionNode->GetTextureID( val->materialTexture->GetTextureNo() ),val->materialTexture->rectangle , val->materialTexture->nb_attachments);
    }
  }
}

dm_Scalar * dm_SceneNode::getScalarFromName( const char * scalarName ) {
  // searches among the variables
  // scalar variables
  dm_Scalar *scalar = lisScalars;
//   if( !scalar ) {
//     printf( "empty scalar list on node [%s]\n" , id );
//   }
  while( scalar ) {
    // printf( "scalar value [%s] [%s]\n" , scalar->GetId() , scalarName );
    if( strcmp( scalar->GetId() , scalarName ) == 0 ) {
      return scalar;
    }
    if( scalar->interp ) {
      for( int ind = 0 ; ind < scalar->interp->nbComponents ; ind++ ) {
	dm_Scalar *scalarxAux = ((dm_Scalar *)(scalar->interp->variables[ind]));
	if( strcmp( scalarxAux->GetId() , scalarName ) == 0 ) {
	  // printf( "interpolated scalar rank [%d]\n" , ind + 1 );
	  return scalarxAux;
	}
      }
    }
    scalar = (dm_Scalar *)scalar->nextVariable;
  }
  return NULL;
}

void dm_Scalar::interpolate( dm_Scalar **tabScalars , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef ) {
  float coef2 = 1.0F - coef;
  float values1 = 0.0;
  float values2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    values1 += weights1[ind] * tabScalars[ind]->GetNumericalValue();
    values2 += weights2[ind] * tabScalars[ind]->GetNumericalValue();
  }
  SetValue(values1 * coef + values2 * coef2);
  //printf( "value %.3f\n" , values1 * coef + values2 * coef2 );

}

void dm_Scalar::interpolate( dm_Scalar *n1 , dm_Scalar *n2 ) {
  SetValue( GetNumericalValue() 
	    + generateRandomFloat( -n2->GetNumericalValue() , 
				   n2->GetNumericalValue() ) );
}

/*
void dm_Scalar::operator=(dm_Scalar& n) {
if (n.interp) {
		this.interp = new dm_VariableInterpolator;
		this.interp = n.interp;
	}
}
*/

dm_Table::dm_Table( int nbVals ): dm_Variable() {
  // save the newly created matrix to the deletion list
  this->chineTable = deletedTableList;
  deletedTableList = this;
	
  // other initialization
  size = nbVals;
  scalars = new dm_ValScalar[ size ];
  nextVariable = NULL;
  interp = NULL;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Table::dm_Table( dm_VariableInterpolator   *interpolator ): dm_Variable() {
  // save the newly created matrix to the deletion list
  this->chineTable = deletedTableList;
  deletedTableList = this;
	
  // other initialization
  size = ((dm_Table *)(interpolator->curVariable))->GetSize();
  scalars = new dm_ValScalar[ size ];
  for( int ind = 0 ; ind < size ; ind++ ) {
    scalars[ ind ] = *(((dm_Table *)(interpolator->curVariable))->GetValue(ind) );
  }
  nextVariable = NULL;
  interp = interpolator;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Table::dm_Table( void ): dm_Variable() {
  // save the newly created matrix to the deletion list
  this->chineTable = deletedTableList;
  deletedTableList = this;
	
  // other initialization
  size = 0;
  scalars = NULL;
  nextVariable = NULL;
  interp = NULL;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Table::~dm_Table( void ) {
  if (scalars) {
    delete [] scalars;
    scalars = NULL;
  }

  if (nextVariable) {
    delete nextVariable;
    nextVariable = NULL;
  }
}

void  dm_Table::InitScalars( int nbVals ) {
  if( scalars ) {
    sprintf( ErrorStr , "Error: table already initialized %s!" , id ); ReportError( ErrorStr ); throw 11;
  }
  size = nbVals;
  scalars = new dm_ValScalar[ size ];
}

float  dm_Table::GetNumericalValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < size ) {
    return scals[ index ].GetNumericalValue();
  }
  return 0.f;
}
dm_ValScalar *dm_Table::GetValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < size ) {
    // printf(  "String table value [%s] (%d)!\n" , scalars[ index ].GetStringValue() , index ); 
    return &(scals[ index ]);
  }
  return scals;
}
void   dm_Table::SetValue( double *val ) {
  dm_ValScalar  *scals = GetScalars();
  for( int index = 0 ; index < size ; index++ ) {
    scals[ index ].SetValue( (float)val[ index ] );
  }
}
void   dm_Table::SetValue( float val , int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < size ) {
    scals[ index ].SetValue( val );
  }
}
char  *dm_Table::GetStringValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < size ) {
    // printf(  "String table value [%s] (%d)!\n" , scalars[ index ].GetStringValue() , index ); 
    return scals[ index ].GetStringValue();
  }
  else {
    sprintf( ErrorStr , "Error: out of bound value in table [%s] (%d/0-%d)!" , id , index , size ); ReportError( ErrorStr );
    return EmptyString;
  }
}
void   dm_Table::SetValue( char *val , int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < size ) {
    scals[ index ].SetValue( val );
  }
}
char *dm_Table::GetId( void ) {
  if( !interp )
    return id;
  else 
    return ((dm_Table *)(interp->curVariable))->id;
}
int    dm_Table::GetSize( void ) {
  if( !interp )
    return size;
  else 
    return ((dm_Table *)(interp->curVariable))->size;
}
DataType dm_Table::GetType( void ) {
  dm_ValScalar  *scals = GetScalars();
  if( size > 0 ) {
    return scals[ 0 ].GetType();
  }
  return EmptyDataType;
}
dm_ValScalar * dm_Table::GetScalars( void ) {
  if( !interp )
    return scalars;
  else 
    return ((dm_Table *)(interp->curVariable))->scalars;
}
void dm_Table::SetId( const char *newId ) {
  if( !interp )
    StrCpy( &id , newId );
  else {
    sprintf( ErrorStr , "Error: interpolated variable (table), ID modification violation!" ); ReportError( ErrorStr ); throw 11;
  }
}
void dm_Table::printScalars( FILE *file ) {
    fprintf( file , "(" );
    for( int ind = 0 ; ind < size - 1 ; ind++ ) {
      if( scalars[ ind ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ ind ].valFloat );
      }
      else if( scalars[ ind ].type == DataTypeInt ) {
      fprintf( file , "%d" ,  scalars[ ind ].valInt );
      }
      else if( scalars[ ind ].type == DataTypeString 
	       && scalars[ ind ].valString ) {
	fprintf( file , "%s" ,  scalars[ ind ].valString );
      }
      fprintf( file , "," );
    }
    if( size > 0 ) {
      if( scalars[ size - 1 ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ size - 1 ].valFloat );
      }
      else if( scalars[ size - 1 ].type == DataTypeInt ) {
      fprintf( file , "%d" ,  scalars[ size - 1 ].valInt );
      }
      else if( scalars[ size - 1 ].type == DataTypeString 
	       && scalars[ size - 1 ].valString ) {
	fprintf( file , "%s" ,  scalars[ size - 1 ].valString );
      }
    }
    fprintf( file , ")" );
}
void dm_Table::print( FILE *file , int depth ) {
  if( !interp ) {
    indent( file , depth );
    DataType     thetype = EmptyDataType;
    if( size > 0 ) {
      thetype = scalars[ 0 ].type;
    }
    fprintf( file , "<table id=\"%s\" size=\"%d\" type=\"%s\" smoothingFunction=\"%s\" smoothingDepth=\"%d\" value=\"(" ,
	     id , size , DataTypeStrings[thetype] , SmoothingFunctionString[smoothingFunction] , smoothingDepth );
    for( int ind = 0 ; ind < size - 1 ; ind++ ) {
      if( scalars[ ind ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ ind ].valFloat );
      }
      else if( scalars[ ind ].type == DataTypeInt ) {
      fprintf( file , "%d" ,  scalars[ ind ].valInt );
      }
      else if( scalars[ ind ].type == DataTypeString 
	       && scalars[ ind ].valString ) {
	fprintf( file , "%s" ,  scalars[ ind ].valString );
      }
      fprintf( file , "," );
    }
    if( size > 0 ) {
      if( scalars[ size - 1 ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ size - 1 ].valFloat );
      }
      else if( scalars[ size - 1 ].type == DataTypeInt ) {
      fprintf( file , "%d" ,  scalars[ size - 1 ].valInt );
      }
      else if( scalars[ size - 1 ].type == DataTypeString 
	       && scalars[ size - 1 ].valString ) {
	fprintf( file , "%s" ,  scalars[ size - 1 ].valString );
      }
    }
    fprintf( file , ")\" /> \n" );
  }
  else {
    interp->print( file , true , depth );
  }
}

void dm_Table::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  dm_ValScalar  *scals = GetScalars();
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "type" ) ) {
    char *thetypeStr = XMLtag->getAttributeStringValue( "type" );
    // default type is "float"
    DataType    the_type = EmptyDataType;
    if( strcmp( thetypeStr , "" ) == 0 ) {
      the_type = DataTypeDouble; 
    }
    else {
      for( int ind = 0 ; ind < EmptyDataType ; ind++ ) {
	if( strcmp( thetypeStr , DataTypeStrings[ind] ) == 0 ) {
	  the_type = (DataType)ind;
	  break;
	}
      }
      if( the_type == EmptyDataType ) {
	sprintf( ErrorStr , "Error: unknown scalar type [%s]!" , thetypeStr ); ReportError( ErrorStr ); throw 249;
      }
    }
    for( int ind = 0 ; ind < size ; ind++ ) {
      scals[ ind ].SetType( the_type );
    }
  }
  DataType   type = scals[ 0 ].GetType();

  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingFunction" ) ) {
    char *thesmoothingFunction = XMLtag->getAttributeStringValue( "smoothingFunction" );
    // default smoothingFunction is "spline"
    smoothingFunction = EmptySmoothingFunction;
    if( strcmp( thesmoothingFunction , "" ) == 0 ) {
      smoothingFunction = dm_Spline; 
    }
    else {
      for( int ind = 0 ; ind < EmptySmoothingFunction ; ind++ ) {
	if( strcmp( thesmoothingFunction , SmoothingFunctionString[ind] ) == 0 ) {
	  smoothingFunction = (SmoothingFunction)ind;
	  break;
	}
      }
      if( smoothingFunction == EmptySmoothingFunction ) {
	sprintf( ErrorStr , "Error: unknown scalar smoothingFunction [%s]!" , thesmoothingFunction ); ReportError( ErrorStr ); throw 249;
      }
    }
  }


  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingDepth" ) ) {
    // default smoothingDepth is 0 (no smoothing)
    smoothingDepth = (int)XMLtag->getAttributeDoubleValue( "smoothingDepth" , false );
    if( smoothingFunction == dm_Spline && smoothingDepth != 0 && smoothingDepth != 4 ) {
      sprintf( ErrorStr , "Error: only 4 point spline smoothing [%d]!" , smoothingDepth ); ReportError( ErrorStr ); throw 249;
    }
    if( smoothingDepth > 0 ) {
      if( type != DataTypeDouble ) {
	sprintf( ErrorStr , "Error: only float tables can be smoothed!" ); ReportError( ErrorStr ); throw 249;
      }
      else {
	for( int ind = 0 ; ind < size ; ind++ ) {
	  scals[ ind ].smoothingValues = new float[ smoothingDepth ];
	  scals[ ind ].smoothingTimes = new float[ smoothingDepth ];
	  scals[ind].smoothingDepth = smoothingDepth;
	}
      }
    }
  }

  if( size > 0 
      && XMLtag->hasAttribute( "value" ) ) {
    int        targetIntValue = -1;

    if( XMLtag->hasAttribute( "index" ) ) {
      // the parameter atRunTime is set to false
      // atRunTime is only true for strings that are dynamically
      // evaluated. the index value is evaluated at compile time
      targetIntValue = XMLtag->getAttributeIntValue( "index" , atRunTime ) - 1;
    }

    // printf( "type %s\n" ,DataTypeStrings[type]);
    if( type == DataTypeInt ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < size ) {
	  operI( scals[ targetIntValue ].valInt ,  
		 XMLtag->getAttributeIntValue( "value" , atRunTime ) );
	}
      }
      else {
	int    *tabValues = new int[ size ];
	XMLtag->getAttributeTableIntValue( tabValues , size , "value" , 
					   atRunTime );
	for( int ind = 0 ; ind < size ; ind++ ) {
	  operI( scals[ ind ].valInt ,  tabValues [ ind ] );
	}
	delete [] tabValues;
	tabValues = NULL;
      }
    }
    else if( type == DataTypeDouble ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < size ) {
	  oper( scals[ targetIntValue ].valFloat ,  
		(float)XMLtag->getAttributeDoubleValue( "value" , atRunTime ) );
	}
      }
      else {
	double   *tabValues = new double[ size ];
	
	XMLtag->getAttributeTableDoubleValue( tabValues , size , "value" , 
					      atRunTime );
	for( int ind = 0 ; ind < size ; ind++ ) {
	  // printf( "Id %s scalars[ %d ] %.3f\n" , id , ind , tabValues [ ind ] );
	  oper(  scals[ ind ].valFloat , (float)tabValues [ ind ] );
	}
	delete [] tabValues;
	tabValues = NULL;
      }
    }
    else if( type == DataTypeString ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < size ) {
      StrCpyAndSubst( &(scals[ targetIntValue ].valString) 
		    , XMLtag->getAttributeStringValue( "value" ) );
	}
      }
      else {
	char    ***tabValues = new char** [ size ];
	memset( tabValues , 0 , size * sizeof( char * ) );

	// uses a temporary table to store the pointers to the scalars
	for( int ind = 0 ; ind < size ; ind++ ) {
	  if( !scals[ ind ].valString ) {
	    InitString( &(scals[ ind ].valString) );
	  }
	  tabValues [ ind ] = &(scals[ ind ].valString);
	}
	
	XMLtag->getAttributeTableStringValue( tabValues , size , 
					      "value" , atRunTime );
	delete [] tabValues;
	tabValues = NULL;
      }
    }
  }
}

dm_Table * dm_SceneNode::getTableFromName( const char * tableName ) {
  // searches among the variables
  // table variables
  dm_Table *table = lisTables;
//   if( !table ) {
//     printf( "empty table list on node [%s]\n" , id );
//   }
  while( table ) {
    // printf( "table value [%s] [%s]\n" , table->GetId() , tableName );
    if( strcmp( table->GetId() , tableName ) == 0 ) {
      return table;
    }
    if( table->interp ) {
      for( int ind = 0 ; ind < table->interp->nbComponents ; ind++ ) {
	dm_Table *tablexAux = ((dm_Table *)(table->interp->variables[ind]));
	if( strcmp( tablexAux->GetId() , tableName ) == 0 ) {
	  return tablexAux;
	}
      }
    }
    table = (dm_Table *)table->nextVariable;
  }
  return NULL;
}

void dm_Table::interpolate( dm_Table **tabTables , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef ) {
  float coef2 = 1.0F - coef;
  for( int indVal = 0 ; indVal < size ; indVal++ ) {
    float values1 = 0.0;
    float values2 = 0.0;
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      values1 += weights1[ind] * tabTables[ind]->GetNumericalValue(indVal);
      values2 += weights2[ind] * tabTables[ind]->GetNumericalValue(indVal);
    }
    SetValue( values1 * coef + values2 * coef2 , indVal );
    // printf( "value %s %d %.3f %.3f\n" , GetId() , indVal , values1 * coef + values2 * coef2 ,GetNumericalValue(indVal));
  }
}

void dm_Table::interpolate( dm_Table *n1 , dm_Table *n2 ) {
  for( int indVal = 0 ; indVal < size ; indVal++ ) {
    SetValue( GetNumericalValue(indVal) 
	      + generateRandomFloat( -n2->GetNumericalValue(indVal) , 
				     n2->GetNumericalValue(indVal) ) ,
	      indVal );
  }
}

dm_Matrix::dm_Matrix( int nbVals ): dm_Variable() {
  // save the newly created matrix to the deletion list
  this->chineMatrix = deletedMatrixList;
  deletedMatrixList = this;
  
  // other initialization
  sizeLine = nbVals;
  sizeCol = nbVals;
  scalars = new dm_ValScalar[ sizeLine * sizeCol ];
  interp = NULL;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}
dm_Matrix::dm_Matrix( dm_VariableInterpolator   *interpolator ): dm_Variable() {
  // save the newly created matrix to the deletion list
  this->chineMatrix = deletedMatrixList;
  deletedMatrixList = this;
  
	// other initialization
  sizeLine = ((dm_Matrix *)(interpolator->curVariable))->GetSizeLine();
  sizeCol = ((dm_Matrix *)(interpolator->curVariable))->GetSizeCol();
  scalars = new dm_ValScalar[ sizeLine * sizeCol ];
  for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
    scalars[ ind ] = *(((dm_Matrix *)(interpolator->curVariable))->GetValue(ind) );
  }
  nextVariable = NULL;
  interp = interpolator;

  sizeLine = 0;
  sizeCol = 0;
  //scalars = NULL;
  //nextVariable = NULL;
  interp = interpolator;
  smoothingFunction = dm_Spline;
  smoothingDepth = 0;
}

dm_Matrix::~dm_Matrix( void ) {
  if (scalars) {
    delete [] scalars;
    scalars = NULL;
  }
  
  if (interp) {
    delete interp;
    interp = NULL;
  }
}

float  dm_Matrix::GetNumericalValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < sizeLine * sizeCol ) {
    return scals[ index ].GetNumericalValue();
  }
  return 0.f;
}
float  dm_Matrix::GetNumericalValue( int indexLine , int indexCol ) {
  dm_ValScalar  *scals = GetScalars();
  if( indexLine < sizeLine && indexCol < sizeCol ) {
    return scals[ indexCol * sizeCol + indexLine ].GetNumericalValue();
  }
  return 0.f;
}
dm_ValScalar *dm_Matrix::GetValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < sizeLine * sizeCol ) {
    // printf(  "String table value [%s] (%d)!\n" , scalars[ index ].GetStringValue() , index ); 
    return &(scals[ index ]);
  }
  return scals;
}
dm_ValScalar *dm_Matrix::GetValue( int indexLine , int indexCol ) {
  dm_ValScalar  *scals = GetScalars();
  if( indexLine < sizeLine && indexCol < sizeCol ) {
    // printf(  "String table value [%s] (%d)!\n" , scalars[ index ].GetStringValue() , index ); 
    return &(scals[ indexCol * sizeCol + indexLine ]);
  }
  return scals;
}
void   dm_Matrix::SetValue( double *val ) {
  dm_ValScalar  *scals = GetScalars();
  for( int index = 0 ; index < sizeLine * sizeCol ; index++ ) {
    scals[ index ].SetValue( (float)val[ index ] );
  }
}
void   dm_Matrix::SetValue( float val , int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < sizeLine * sizeCol ) {
    scals[ index ].SetValue( val );
  }
}
void   dm_Matrix::SetValue( float val , int indexLine , int indexCol ) {
  dm_ValScalar  *scals = GetScalars();
  if( indexLine < sizeLine && indexCol < sizeCol ) {
    scals[ indexCol * sizeCol + indexLine ].SetValue( val );
  }
}
void   dm_Matrix::SetValue( char *val , int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < sizeLine * sizeCol ) {
    scals[ index ].SetValue( val );
  }
}
void   dm_Matrix::SetValue( char *val , int indexLine , int indexCol ) {
  dm_ValScalar  *scals = GetScalars();
  if( indexLine < sizeLine && indexCol < sizeCol ) {
    scals[ indexCol * sizeCol + indexLine ].SetValue( val );
  }
}
char  *dm_Matrix::GetStringValue( int index ) {
  dm_ValScalar  *scals = GetScalars();
  if( index >= 0 && index < sizeLine * sizeCol ) {
    return scals[ index ].GetStringValue();
  }
  else {
    sprintf( ErrorStr , "Error: out of bound value in matrix [%s] (%d/0-%d)!" , id , index , sizeLine * sizeCol ); ReportError( ErrorStr ); 
    return EmptyString;
  }
}
char  *dm_Matrix::GetStringValue( int indexLine , int indexCol ) {
  dm_ValScalar  *scals = GetScalars();
  if( indexLine < sizeLine && indexCol < sizeCol ) {
    return scals[ indexCol * sizeCol + indexLine ].GetStringValue();
  }
  else {
    sprintf( ErrorStr , "Error: out of bound value in matrix [%s] (%d/%d) (%d/%d)!" , id , indexLine , sizeLine ,  indexCol ,  sizeCol ); ReportError( ErrorStr ); throw 10;
    return EmptyString;
  }
}
char *dm_Matrix::GetId( void ) {
  if( !interp )
    return id;
  else 
    return ((dm_Matrix *)(interp->curVariable))->id;
}
int    dm_Matrix::GetSizeLine( void ) {
  if( !interp )
    return sizeLine;
  else 
    return ((dm_Matrix *)(interp->curVariable))->sizeLine;
}
int    dm_Matrix::GetSizeCol( void ) {
  if( !interp )
    return sizeCol;
  else 
    return ((dm_Matrix *)(interp->curVariable))->sizeCol;
}
int    dm_Matrix::GetSize( void ) {
  if( !interp )
    return sizeCol * sizeLine;
  else 
    return ((dm_Matrix *)(interp->curVariable))->sizeCol * ((dm_Matrix *)(interp->curVariable))->sizeLine;
}
DataType dm_Matrix::GetType( void ) {
  dm_ValScalar  *scals = GetScalars();
  if( sizeLine * sizeCol > 0 ) {
    return scals[ 0 ].GetType();
  }
  return EmptyDataType;
}
dm_ValScalar   * dm_Matrix::GetScalars( void ) {
  return scalars;
}
void dm_Matrix::SetId( const char *newId ) {
  if( !interp )
    StrCpy( &id , newId );
  else {
    sprintf( ErrorStr , "Error: interpolated variable (matrix), ID modification violation!" ); ReportError( ErrorStr ); throw 11;
  }
}
void dm_Matrix::print( FILE *file , int depth ) {
  if( !interp ) {
    indent( file , depth );
    fprintf( file , "<matrix id=\"%s\" size=\"%d\" smoothingFunction=\"%s\" smoothingDepth=\"%d\" value=\"(" ,
	     id , sizeLine , SmoothingFunctionString[smoothingFunction] , smoothingDepth );
    for( int ind = 0 ; ind < sizeLine * sizeCol - 1 ; ind++ ) {
      if( scalars[ ind ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ ind ].valFloat );
      }
      else if( scalars[ ind ].type == DataTypeInt ) {
	fprintf( file , "%d" ,  scalars[ ind ].valInt );
      }
      else if( scalars[ ind ].type == DataTypeString  
	       && scalars[ ind ].valString ) {
	fprintf( file , "%s" ,  scalars[ ind ].valString );
      }
      fprintf( file , "," );
    }
    if( sizeLine * sizeCol > 0 ) {
      if( scalars[ sizeLine * sizeCol - 1 ].type == DataTypeDouble ) {
	fprintf( file , "%.5f" ,  scalars[ sizeLine * sizeCol - 1 ].valFloat );
      }
      else if( scalars[ sizeLine * sizeCol - 1 ].type == DataTypeInt ) {
	fprintf( file , "%d" ,  scalars[ sizeLine * sizeCol - 1 ].valInt );
      }
      else if( scalars[ sizeLine * sizeCol - 1 ].type == DataTypeString
	       && scalars[ sizeLine * sizeCol - 1 ].valString ) {
	fprintf( file , "%s" ,  scalars[ sizeLine * sizeCol - 1 ].valString );
      }
    }
    fprintf( file , ")\" /> \n" );
  }
  else {
    interp->print( file , true , depth );
  }
}

void dm_Matrix::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		   FloatBinOp oper ,
		   IntBinOp operI ) {
  dm_ValScalar  *scals = GetScalars();
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "type" ) ) {
    char *thetypeStr = XMLtag->getAttributeStringValue( "type" );
    // default type is "float"
    DataType    the_type = EmptyDataType;
    if( strcmp( thetypeStr , "" ) == 0 ) {
      the_type = DataTypeDouble; 
    }
    else {
      for( int ind = 0 ; ind < EmptyDataType ; ind++ ) {
	if( strcmp( thetypeStr , DataTypeStrings[ind] ) == 0 ) {
	  the_type = (DataType)ind;
	  break;
	}
      }
      if( the_type == EmptyDataType ) {
	sprintf( ErrorStr , "Error: unknown scalar type [%s]!" , thetypeStr ); ReportError( ErrorStr ); throw 249;
      }
    }
    for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
      scals[ ind ].SetType( the_type );
    }
  }

  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingFunction" ) ) {
    char *thesmoothingFunction = XMLtag->getAttributeStringValue( "smoothingFunction" );
    // default smoothingFunction is "spline"
    smoothingFunction = EmptySmoothingFunction;
    if( strcmp( thesmoothingFunction , "" ) == 0 ) {
      smoothingFunction = dm_Spline; 
    }
    else {
      for( int ind = 0 ; ind < EmptySmoothingFunction ; ind++ ) {
	if( strcmp( thesmoothingFunction , SmoothingFunctionString[ind] ) == 0 ) {
	  smoothingFunction = (SmoothingFunction)ind;
	  break;
	}
      }
      if( smoothingFunction == EmptySmoothingFunction ) {
	sprintf( ErrorStr , "Error: unknown scalar smoothingFunction [%s]!" , thesmoothingFunction ); ReportError( ErrorStr ); throw 249;
      }
    }
  }
  DataType   type = scals[ 0 ].GetType();

  if( !atRunTime
      && XMLtag->hasAttribute( "smoothingDepth" ) ) {
    // default smoothingDepth is 0 (no smoothing)
    smoothingDepth = (int)XMLtag->getAttributeDoubleValue( "smoothingDepth" , false );
    if( smoothingFunction == dm_Spline && smoothingDepth != 0 && smoothingDepth != 4 ) {
      sprintf( ErrorStr , "Error: only 4 point spline smoothing [%d]!" , smoothingDepth ); ReportError( ErrorStr ); throw 249;
    }
    if( smoothingDepth > 0 ) {
      if( type != DataTypeDouble ) {
	sprintf( ErrorStr , "Error: only float tables can be smoothed!" ); ReportError( ErrorStr ); throw 249;
      }
      else {
	for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
	  scals[ ind ].smoothingValues = new float[ smoothingDepth ];
	  scals[ ind ].smoothingTimes = new float[ smoothingDepth ];
	  scals[ind].smoothingDepth = smoothingDepth;
	}
      }
    }
  }

  if( sizeLine * sizeCol > 0 
      && XMLtag->hasAttribute( "value" ) ) {
    int        targetIntValue = -1;

    if( XMLtag->hasAttribute( "index" ) ) {
      // the parameter atRunTime is set to false
      // atRunTime is only true for strings that are dynamically
      // evaluated. the index value is evaluated at compile time
      targetIntValue = XMLtag->getAttributeIntValue( "index" , atRunTime ) - 1;
    }

    if( type == DataTypeInt ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < sizeLine * sizeCol ) {
	  operI( scals[ targetIntValue ].valInt ,  
		 XMLtag->getAttributeIntValue( "value" , atRunTime ) );
	}
      }
      else {
	int    *tabValues = new int[ sizeLine * sizeCol ];
	
	XMLtag->getAttributeTableIntValue( tabValues , sizeLine * sizeCol , 
					   "value" , atRunTime );
	for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
	  operI( scals[ ind ].valInt ,  tabValues [ ind ] );
	}
	delete [] tabValues;
	tabValues = NULL;
      }
    }
    else if( type == DataTypeDouble ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < sizeLine * sizeCol ) {
	  oper( scals[ targetIntValue ].valFloat ,  
		(float)XMLtag->getAttributeDoubleValue( "value" , atRunTime ) );
	}
      }
      else {
	double   *tabValues = new double[ sizeLine * sizeCol ];
	
	XMLtag->getAttributeTableDoubleValue( tabValues , sizeLine * sizeCol , 
					      "value" , atRunTime );
	for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
	  oper(  scals[ ind ].valFloat , (float)tabValues [ ind ] );
	}
	delete [] tabValues;
	tabValues = NULL;
      }
    }
    else if( type == DataTypeString ) {
      if( targetIntValue >= 0 ) {
	if( targetIntValue < sizeLine * sizeCol ) {
	  StrCpy( &(scals[ targetIntValue ].valString) 
		  , XMLtag->getAttributeStringValue( "value" ) );
	}
      }
      else {
	char    ***tabValues = new char** [ sizeLine * sizeCol ];
	memset( tabValues , 0 , sizeLine * sizeCol * sizeof( char * ) );

	// uses a temporary table to store the pointers to the scals
	for( int ind = 0 ; ind < sizeLine * sizeCol ; ind++ ) {
	  if( !scals[ ind ].valString ) {
	    InitString( &(scals[ ind ].valString) );
	  }
	  tabValues [ ind ] = &(scals[ ind ].valString);
	}
	
	XMLtag->getAttributeTableStringValue( tabValues , sizeLine * sizeCol , 
					      "value" , atRunTime );
	delete [] tabValues;
	tabValues = NULL;
      }
    }
  }
}

dm_Matrix * dm_SceneNode::getMatrixFromName( const char * matrixName ) {
  // searches among the variables
  // matrix variables
  dm_Matrix *matrix = lisMatrices;
//   if( !matrix ) {
//     printf( "empty matrix list on node [%s]\n" , id );
//   }
  while( matrix ) {
    // printf( "matrix value [%s] [%s]\n" , matrix->GetId() , matrixName );
    if( strcmp( matrix->GetId() , matrixName ) == 0 ) {
      return matrix;
    }
    if( matrix->interp ) {
      for( int ind = 0 ; ind < matrix->interp->nbComponents ; ind++ ) {
	dm_Matrix *matrixAux = ((dm_Matrix *)(matrix->interp->variables[ind]));
	if( strcmp( matrixAux->GetId() , matrixName ) == 0 ) {
	  return matrixAux;
	}
      }
    }
    matrix = (dm_Matrix *)matrix->nextVariable;
  }
  return NULL;
}

void dm_Matrix::interpolate( dm_Matrix **tabMatrix , 
			     float *weights1 , 
			     float *weights2 , 
			     int numberOfComponents ,
			     float coef ) {
  float coef2 = 1.0F - coef;
  for( int indVal = 0 ; indVal < sizeLine * sizeCol ; indVal++ ) {
    float values1 = 0.0;
    float values2 = 0.0;
    for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
      values1 += weights1[ind] * tabMatrix[ind]->GetNumericalValue(indVal);
      values2 += weights2[ind] * tabMatrix[ind]->GetNumericalValue(indVal);
    }
    SetValue( values1 * coef + values2 * coef2 , indVal );
    //printf( "value %.3f\n" , values1 * coef + values2 * coef2 );
  }
}

void dm_Matrix::interpolate( dm_Matrix *n1 , dm_Matrix *n2 ) {
  for( int indVal = 0 ; indVal < sizeLine * sizeCol ; indVal++ ) {
    SetValue( GetNumericalValue(indVal) 
	      + generateRandomFloat( -n2->GetNumericalValue(indVal) , 
				     n2->GetNumericalValue(indVal) ) ,
	      indVal );
  }
}
