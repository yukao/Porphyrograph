/*! \file dm-non-geom-objects.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-non-geom-objects.cpp
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

const char *ShaderTextureTargetString[EmptyShaderTextureTarget + 1] = { "vertex_texture",  "geometry_texture", "fragment_texture", "emptyshadertexturetarget" };

const char *TransformationTypeString[EmptyTransformation + 1] = { "translation" , "rotation" , "quaternion" , "scale" , "matrix" , "emptytransformation" };

const char *ViewvolumeTypeString[EmptyViewvolumeType + 1] = { "frustum", "ortho", "emptyviewvolumetype" };

const char *ViewPointTypeString[EmptyViewPointType + 1] = { "orientable", "emptyviewpointtype" };

dm_NonGeometricalObject::dm_NonGeometricalObject( void ): dm_SceneObject() {
}

dm_NonGeometricalObject::~dm_NonGeometricalObject( void ) {
  // TODO
}

void dm_NonGeometricalObject::display( DrawingMode mode , bool *invalidDL ) {
  sprintf( ErrorStr , "Error: display not implemented for dm_NonGeometricalObject %s!" , id ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::bindVertexData( void ) {
  sprintf( ErrorStr , "Error: bindVertexData not implemented for dm_NonGeometricalObject %s!" , id ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::print( FILE *file , int isLong , int depth ) {
  sprintf( ErrorStr , "Error: print not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::NewvertexArrayID( void ){
  sprintf( ErrorStr , "Error: NewvertexArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_NonGeometricalObject::NewelementArrayID( void ){
  sprintf( ErrorStr , "Error: NewelementArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
GLuint dm_NonGeometricalObject::GetvertexArrayID( void ){
  sprintf( ErrorStr , "Error: GetvertexArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
GLuint dm_NonGeometricalObject::GetelementArrayID( void ){
  sprintf( ErrorStr , "Error: GetelementArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_NonGeometricalObject::SetvertexArrayID( GLuint alistID ){
  sprintf( ErrorStr , "Error: SetvertexArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_NonGeometricalObject::SetelementArrayID( GLuint alistID ){
  sprintf( ErrorStr , "Error: SetelementArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_NonGeometricalObject::DeletevertexArrayID( void ){
  sprintf( ErrorStr , "Error: DeletevertexArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_NonGeometricalObject::DeleteelementArrayID( void ){
  sprintf( ErrorStr , "Error: DeleteelementArrayID not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI ) {
  sprintf( ErrorStr , "Error: setParameters not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

bool dm_NonGeometricalObject::getParameters( char *attribute , 
					     DataType * dataType ,
					     double * valDouble ,
					     int * valInt ,
					     bool * valBool ,
					     char ** valString ,
					     int * tableOrMatrixSize ,
					     dm_ValScalar ** valScalars ) {
  sprintf( ErrorStr , "Error: getParameters not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::setCommonParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		    IntBinOp operI ) {
  sprintf( ErrorStr , "Error: setCommonParameters not implemented for dm_NonGeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_NonGeometricalObject::operator=(dm_NonGeometricalObject& h) {
  this->dm_SceneObject::operator=(h);
}

dm_ShaderSourceObject::dm_ShaderSourceObject( void ): dm_NonGeometricalObject() {
  on = false;

  nb_attachments = 1;

  materialTextures = NULL;
  textureIndex = NULL;
  textureTarget = NULL;
#ifdef DM_HAVE_FFMPEG
  materialVideo = NULL;
#endif
  fileName = NULL;
  encoding = NULL;
  nb_shaderTextures = 0;
  active_texture = 0;
  
  message_display = false;
  
  vertexUniformVarName = NULL;
  vertexUniformVarSize = NULL;
  vertexUniformVarValue = NULL;
  nb_vertexUniformVars = 0;
  
  geometryUniformVarName = NULL;
  geometryUniformVarSize = NULL;
  geometryUniformVarValue = NULL;
  nb_geometryUniformVars = 0;

  fragmentUniformVarName = NULL;
  fragmentUniformVarSize = NULL;
  fragmentUniformVarValue = NULL;
  nb_fragmentUniformVars = 0;
}

dm_ShaderSourceObject::~dm_ShaderSourceObject( void ) {
  if (materialTextures) {
    // textures are released by releaseAllTextures
//     for( int ind = 0 ; ind < nb_shaderTextures ; ind++ ) {
//       delete materialTextures[ind];
//     }
    delete [] materialTextures;
    materialTextures = NULL;
  }

  if (textureIndex) {
    delete [] textureIndex;
    textureIndex = NULL;
  }

  if (textureTarget) {
    delete [] textureTarget;
    textureTarget = NULL;
  }
  
#ifdef DM_HAVE_FFMPEG
  if (materialVideo) {
    for (int ind = 0; ind < nb_shaderTextures; ind++)
      {
	delete materialVideo[ind];
	materialVideo[ind] = NULL;
      }

    delete [] materialVideo;
    materialVideo = NULL;
  }
#endif
  if (fileName) {
    for (int ind = 0; ind < nb_shaderTextures; ind++)
      {
	delete fileName[ind];
	fileName[ind] = NULL;
      }

    delete [] fileName;
    fileName = NULL;
  }

  if (encoding) {
    for (int ind = 0; ind < nb_shaderTextures; ind++)
      {
	delete encoding[ind];
	encoding[ind] = NULL;
      }

    delete [] encoding;
    encoding = NULL;
  }


  if (vertexUniformVarName) {
    // tables don't have to be deleted, they are chained
    // and deleted through the deletedTableList
     for (int ind = 0; ind < nb_vertexUniformVars; ind++)
       {
 	delete vertexUniformVarName[ind];
 	vertexUniformVarName[ind] = NULL;
       }

    delete [] vertexUniformVarName;
    vertexUniformVarName = NULL;
  }

  if (vertexUniformVarSize) {
    delete [] vertexUniformVarSize;
    vertexUniformVarSize = NULL;
  }

  if (vertexUniformVarValue) {
    for (int ind = 0; ind < nb_vertexUniformVars; ind++)
      {
	    // delete vertexUniformVarValue[ind];
	     vertexUniformVarValue[ind] = NULL;
      
	}
    delete [] vertexUniformVarValue;
    vertexUniformVarValue = NULL;
  }

  if (geometryUniformVarName) {
    // tables don't have to be deleted, they are chained
    // and deleted through the deletedTableList
     for (int ind = 0; ind < nb_geometryUniformVars; ind++)
       {
 	delete geometryUniformVarName[ind];
 	geometryUniformVarName[ind] = NULL;
       }

    delete [] geometryUniformVarName;
    geometryUniformVarName = NULL;
  }

  if (geometryUniformVarSize) {
    delete [] geometryUniformVarSize;
    geometryUniformVarSize = NULL;
  }

  if (geometryUniformVarValue) {
    for (int ind = 0; ind < nb_geometryUniformVars; ind++)
      {
	    // delete geometryUniformVarValue[ind];
	     geometryUniformVarValue[ind] = NULL;
      
	}
    delete [] geometryUniformVarValue;
    geometryUniformVarValue = NULL;
  }

  if (fragmentUniformVarValue) {
    // tables don't have to be deleted, they are chained
    // and deleted through the deletedTableList
//     for (int ind = 0; ind < nb_fragmentUniformVars; ind++)
//       {
// 	delete fragmentUniformVarValue[ind];
// 	fragmentUniformVarValue[ind] = NULL;
//       }

    delete [] fragmentUniformVarValue;
    fragmentUniformVarValue = NULL;
  }

  if (fragmentUniformVarName) {
    for (int ind = 0; ind < nb_fragmentUniformVars; ind++)
      {
	delete fragmentUniformVarName[ind];
	fragmentUniformVarName[ind] = NULL;
      }

    delete [] fragmentUniformVarName;
    fragmentUniformVarName = NULL;
  }

  if (fragmentUniformVarSize) {
    delete [] fragmentUniformVarSize;
    fragmentUniformVarSize = NULL;
  }

}

void dm_ShaderSourceObject::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }
  if( XMLtag->hasAttribute( "nb_attachments" ) ) {
    nb_attachments
      = (int) XMLtag->getAttributeIntValue( "nb_attachments" , 
					    atRunTime );
  }
  if( !atRunTime 
      || XMLtag->hasAttribute( "nb_textures" ) ) {
    nb_shaderTextures 
      = (int) XMLtag->getAttributeIntValue( "nb_textures" , 
					    atRunTime );
  }
  if( XMLtag->hasAttribute( "active_texture"  ) ) {
    active_texture 
      = XMLtag->getAttributeIntValue( "active_texture" , 
				      atRunTime ) - 1;
    
    if( active_texture < 0 || active_texture >= nb_shaderTextures  ) {
      sprintf( ErrorStr , "Error: shaderSource active texture out of bounds [%d: 1-%d] in shaderSource ID [%s]!" , active_texture + 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); throw 291;
    }
  }
  if( !atRunTime 
      || XMLtag->hasAttribute( "message_display" ) ) {
    XMLtag->BooleanFieldValue( "message_display" ,
			       &(message_display) );
  }
  
  ///////////////////////////////////////////////////////////////////:
  // vertex program or fragment shader parameters
  // non predefined parameters
  // correspond to vertex program or fragment shader parameters
  // IDs must begin with specific prefix
  // attributes and values are stored in tables that are used
  // for parameter passing with the shaders 
  // At compile time: stores parameters and values
  if( !atRunTime ) {
    nb_fragmentUniformVars = 0;
    nb_geometryUniformVars = 0;
    nb_vertexUniformVars = 0;
    for( int ind = 0 ; ind < XMLtag->nbAttributes ; ind++ ) {
      if( strncmp( XMLtag->attributes[ ind ] , "glsl_fs" , 7 ) == 0 ) {
	nb_fragmentUniformVars++;
      }
      if( strncmp( XMLtag->attributes[ ind ] , "glsl_gs" , 7 ) == 0 ) {
	nb_geometryUniformVars++;
      }
      if( strncmp( XMLtag->attributes[ ind ] , "glsl_vp" , 7 ) == 0 ) {
	nb_vertexUniformVars++;
      }
    }
    vertexUniformVarName = new char*[nb_vertexUniformVars];
    vertexUniformVarSize = new int[nb_vertexUniformVars];
    vertexUniformVarValue = new dm_Table*[nb_vertexUniformVars];

    geometryUniformVarName = new char*[nb_geometryUniformVars];
    geometryUniformVarSize = new int[nb_geometryUniformVars];
    geometryUniformVarValue = new dm_Table*[nb_geometryUniformVars];

    fragmentUniformVarName = new char*[nb_fragmentUniformVars];
    fragmentUniformVarSize = new int[nb_fragmentUniformVars];
    fragmentUniformVarValue = new dm_Table*[nb_fragmentUniformVars];

    int ind_fragmentUniformVar = 0;
    int ind_geometryUniformVar = 0;
    int ind_vertexUniformVar = 0;
    for( int ind = 0 ; ind < XMLtag->nbAttributes ; ind++ ) {
      if( strncmp( XMLtag->attributes[ ind ] , "glsl_fs" , 7 ) == 0 ) {
	fragmentUniformVarName[ind_fragmentUniformVar] = NULL;
	StrCpy( &fragmentUniformVarName[ ind_fragmentUniformVar ] , 
		XMLtag->attributes[ ind ] );
	fragmentUniformVarSize[ ind_fragmentUniformVar ] = 1;
	if( strncmp( XMLtag->attributes[ ind ] + 7 , "_2fv" , 4 ) == 0 ) {
	  fragmentUniformVarSize[ ind_fragmentUniformVar ] = 2;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3fv" , 4 ) == 0 ) {
	  fragmentUniformVarSize[ ind_fragmentUniformVar ] = 3;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4fv" , 4 ) == 0 ) {
	  fragmentUniformVarSize[ ind_fragmentUniformVar ] = 4;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4x4fv" , 6 ) == 0 ) {
	  fragmentUniformVarSize[ ind_fragmentUniformVar ] = 16;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3x3fv" , 6 ) == 0 ) {
	  fragmentUniformVarSize[ ind_fragmentUniformVar ] = 9;
	}
	fragmentUniformVarValue[ ind_fragmentUniformVar ] 
	  = new dm_Table( fragmentUniformVarSize[ ind_fragmentUniformVar ] );
	int size = fragmentUniformVarSize[ ind_fragmentUniformVar ];
	if( size == 1 ) {
	  fragmentUniformVarValue[ ind_fragmentUniformVar ]->SetValue(   
	    (float)XMLtag->getAttributeDoubleValue( 
                           XMLtag->attributes[ ind ] , 
			   atRunTime ) , 0 );
	  // printf("Var : %s %.5f\n" ,fragmentUniformVarName[ ind_fragmentUniformVar ] , fragmentUniformVarValue[ ind_fragmentUniformVar ]);
	}
	else if( size > 1 ) {
	  double   *tabValues = new double[ size ];
	  XMLtag->getAttributeTableDoubleValue( tabValues , 
						size ,
						XMLtag->attributes[ ind ] , 
						atRunTime );
	  fragmentUniformVarValue[ ind_fragmentUniformVar ]->SetValue( tabValues ); 
	  delete [] tabValues;
	}
	ind_fragmentUniformVar++;
      }

      if( strncmp( XMLtag->attributes[ ind ] , "glsl_gs" , 7 ) == 0 ) {
	geometryUniformVarName[ind_geometryUniformVar] = NULL;
	StrCpy( &geometryUniformVarName[ ind_geometryUniformVar ] , 
		XMLtag->attributes[ ind ] );
	geometryUniformVarSize[ ind_geometryUniformVar ] = 1;
	if( strncmp( XMLtag->attributes[ ind ] + 7 , "_2fv" , 4 ) == 0 ) {
	  geometryUniformVarSize[ ind_geometryUniformVar ] = 2;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3fv" , 4 ) == 0 ) {
	  geometryUniformVarSize[ ind_geometryUniformVar ] = 3;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4fv" , 4 ) == 0 ) {
	  geometryUniformVarSize[ ind_geometryUniformVar ] = 4;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4x4fv" , 6 ) == 0 ) {
	  geometryUniformVarSize[ ind_geometryUniformVar ] = 16;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3x3fv" , 6 ) == 0 ) {
	  geometryUniformVarSize[ ind_geometryUniformVar ] = 9;
	}
	geometryUniformVarValue[ ind_geometryUniformVar ] 
	  = new dm_Table( geometryUniformVarSize[ ind_geometryUniformVar ] );
	int size = geometryUniformVarSize[ ind_geometryUniformVar ];
	if( size == 1 ) {
	  geometryUniformVarValue[ ind_geometryUniformVar ]->SetValue(   
	    (float)XMLtag->getAttributeDoubleValue( 
                           XMLtag->attributes[ ind ] , 
			   atRunTime ) , 0 );
	  // printf("Var : %s %.5f\n" ,fragmentUniformVarName[ ind_fragmentUniformVar ] , fragmentUniformVarValue[ ind_fragmentUniformVar ]);
	}
	else if( size > 1 ) {
	  double   *tabValues = new double[ size ];
	  XMLtag->getAttributeTableDoubleValue( tabValues , 
						size ,
						XMLtag->attributes[ ind ] , 
						atRunTime );
	  geometryUniformVarValue[ ind_geometryUniformVar ]->SetValue( tabValues ); 
	  delete [] tabValues;
	}
	ind_geometryUniformVar++;
      }


      if( strncmp( XMLtag->attributes[ ind ] , "glsl_vp" , 7 ) == 0 ) {
	vertexUniformVarName[ind_vertexUniformVar] = NULL;
	StrCpy( &vertexUniformVarName[ ind_vertexUniformVar ] , 
		XMLtag->attributes[ ind ] );
	vertexUniformVarSize[ ind_vertexUniformVar ] = 1;
	if( strncmp( XMLtag->attributes[ ind ] + 7 , "_2fv" , 4 ) == 0 ) {
	  vertexUniformVarSize[ ind_vertexUniformVar ] = 2;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3fv" , 4 ) == 0 ) {
	  vertexUniformVarSize[ ind_vertexUniformVar ] = 3;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4fv" , 4 ) == 0 ) {
	  vertexUniformVarSize[ ind_vertexUniformVar ] = 4;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_4x4fv" , 6 ) == 0 ) {
	  vertexUniformVarSize[ ind_vertexUniformVar ] = 16;
	}
	else if( strncmp( XMLtag->attributes[ ind ] + 7 , "_3x3fv" , 6 ) == 0 ) {
	  vertexUniformVarSize[ ind_vertexUniformVar ] = 9;
	}
	vertexUniformVarValue[ ind_vertexUniformVar ] 
	  = new dm_Table( vertexUniformVarSize[ ind_vertexUniformVar ] );
	int size = vertexUniformVarSize[ ind_vertexUniformVar ];
	if( size == 1 ) {
	  vertexUniformVarValue[ ind_vertexUniformVar ]->SetValue(   
	    (float)XMLtag->getAttributeDoubleValue( 
			  XMLtag->attributes[ ind ] , 
			  atRunTime ) , 0 );
	}
	else if( size > 1 ) {
	  double   *tabValues = new double[ size ];
	  XMLtag->getAttributeTableDoubleValue( tabValues , 
						size ,
						XMLtag->attributes[ ind ] , 
						atRunTime );
	  vertexUniformVarValue[ ind_vertexUniformVar ]->SetValue( tabValues ); 
	  delete [] tabValues;
	}
	ind_vertexUniformVar++;
      }
    }
  }
  // At run time: retrieves predeclared attribute name
  else {
    for( int indCurrentAttr = 0 ; indCurrentAttr < XMLtag->nbAttributes ; 
	 indCurrentAttr++ ) {
      for( int indFragmentName = 0 ; 
	   indFragmentName < nb_fragmentUniformVars ; 
	   indFragmentName++ ) {
	if( strcmp( XMLtag->attributes[ indCurrentAttr ] , 
		    fragmentUniformVarName[ indFragmentName ] ) == 0 ) {
	  // printf( "run time Dynamic FS Arguments %s\n", fragmentUniformVarName[indFragmentName]);
	  int size = fragmentUniformVarSize[ indFragmentName ];
	  dm_ValScalar * scals 
	    = fragmentUniformVarValue[ indFragmentName ]->GetScalars();
	  if( size == 1 ) {
	    oper( scals[ 0 ].valFloat ,  
		  (float)XMLtag->getAttributeDoubleValue( 
				 XMLtag->attributes[ indCurrentAttr ] , 
				 atRunTime ) );
	  }
	  else if( size > 1 ) {
	    double   *tabValues = new double[ size ];
	    // printf( "Load Dynamic FS Arguments %d %s\n", size , XMLtag->attributes[ indCurrentAttr ] ); 
	    XMLtag->getAttributeTableDoubleValue( tabValues , 
			   size ,
			   XMLtag->attributes[ indCurrentAttr ] , 
			   atRunTime );
	    // printf( "Dynamic FS Arguments %s %.2f %.2f %.2f\n", fragmentUniformVarName[indFragmentName] , tabValues[ 0 ] , tabValues[ 1 ] , abValues[ 2 ] );
	    for( int ind = 0 ; ind < size ; ind++ ) {
	      oper( scals[ ind ].valFloat , (float)tabValues[ ind ] );
	    }
	    delete [] tabValues;
	  }
	}
      }


      for( int indGeometryName = 0 ; 
	   indGeometryName < nb_geometryUniformVars ; 
	   indGeometryName++ ) {
	if( strcmp( XMLtag->attributes[ indCurrentAttr ] , 
		    geometryUniformVarName[ indGeometryName ] ) == 0 ) {
	  // printf( "run time Dynamic GS Arguments %s\n", geometryUniformVarName[indGeometryName]);
	  int size = geometryUniformVarSize[ indGeometryName ];
	  dm_ValScalar * scals 
	    = geometryUniformVarValue[ indGeometryName ]->GetScalars();
	  if( size == 1 ) {
	    oper( scals[ 0 ].valFloat ,  
		  (float)XMLtag->getAttributeDoubleValue( 
				 XMLtag->attributes[ indCurrentAttr ] , 
				 atRunTime ) );
	  }
	  else if( size > 1 ) {
	    double   *tabValues = new double[ size ];
	    // printf( "Load Dynamic GS Arguments %d %s\n", size , XMLtag->attributes[ indCurrentAttr ] ); 
	    XMLtag->getAttributeTableDoubleValue( tabValues , 
			   size ,
			   XMLtag->attributes[ indCurrentAttr ] , 
			   atRunTime );
	    // printf( "Dynamic GS Arguments %s %.2f %.2f %.2f\n", geometryUniformVarName[indFragmentName] , tabValues[ 0 ] , tabValues[ 1 ] , abValues[ 2 ] );
	    for( int ind = 0 ; ind < size ; ind++ ) {
	      oper( scals[ ind ].valFloat , (float)tabValues[ ind ] );
	    }
	    delete [] tabValues;
	  }
	}
      }



      for( int indVertexName = 0 ; 
	   indVertexName < nb_vertexUniformVars ; 
	   indVertexName++ ) {
	if( strcmp( XMLtag->attributes[ indCurrentAttr ] , 
		    vertexUniformVarName[ indVertexName ] ) == 0 ) {
	  int size = vertexUniformVarSize[ indVertexName ];
	  // printf( "run time Dynamic VP Arguments %s size %d\n", vertexUniformVarName[indVertexName],size);
	  dm_ValScalar * scals 
	    = vertexUniformVarValue[ indVertexName ]->GetScalars();
	  if( size == 1 ) {
	    oper( scals[ 0 ].valFloat ,  
		  (float)XMLtag->getAttributeDoubleValue( 
				 XMLtag->attributes[ indCurrentAttr ] , 
				 atRunTime ) );
	  }
	  else if( size > 1 ) {
	    double   *tabValues = new double[ size ];
	    XMLtag->getAttributeTableDoubleValue( tabValues , 
			   size ,
			   XMLtag->attributes[ indCurrentAttr ] , 
			   atRunTime );
	    // printf( "Dynamic VP Arguments %s %.2f %.2f %.2f\n", vertexUniformVarName[indVertexName] , tabValues[ 0 ] , tabValues[ 1 ] , tabValues[ 2 ] );
	    for( int ind = 0 ; ind < size ; ind++ ) {
	      oper( scals[ ind ].valFloat ,  
		    (float)tabValues[ ind ] );
	    }
	    delete [] tabValues;
	  }
	}
      }
    }
  }
}

bool dm_ShaderSourceObject::getParameters( char *attribute , 
					   DataType * dataType ,
					   double * valDouble ,
					   int * valInt ,
					   bool * valBool ,
					   char ** valString ,
					   int * tableOrMatrixSize ,
					   dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "nb_textures" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_shaderTextures;
    return true;
  }
  else if( strcmp( attribute , "nb_attachments" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_attachments;
    return true;
  }
  else if( strcmp( attribute , "active_texture" ) == 0 ) {
    *dataType = DataTypeInt;
    *valDouble = active_texture;
    return true;
  }
  else if( strcmp( attribute , "message_display" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = message_display;
    return true;
  }
  else {
    // printf("shader attribute (%s)\n" , attribute );
    // predefined fragment shader or vertex program name
    for( int indFragmentName = 0 ; 
	 indFragmentName < nb_fragmentUniformVars ; 
	 indFragmentName++ ) {
      if( strcmp( attribute , 
		  fragmentUniformVarName[ indFragmentName ] ) == 0 ) {
	*dataType = DataTypeDouble;
	*tableOrMatrixSize = fragmentUniformVarSize[ indFragmentName ];
	*valScalars = fragmentUniformVarValue[ indFragmentName ]->GetScalars();
	return true;
      }
    }
	for( int indGeometryName = 0 ; 
	 indGeometryName < nb_geometryUniformVars ; 
	 indGeometryName++ ) {
      if( strcmp( attribute , 
		  geometryUniformVarName[ indGeometryName ] ) == 0 ) {
	*dataType = DataTypeDouble;
	*tableOrMatrixSize = geometryUniformVarSize[ indGeometryName ];
	*valScalars = geometryUniformVarValue[ indGeometryName ]->GetScalars();
	return true;
      }
    }

    for( int indVertexName = 0 ; 
	 indVertexName < nb_vertexUniformVars ; 
	 indVertexName++ ) {
      if( strcmp( attribute , 
		  vertexUniformVarName[ indVertexName ] ) == 0 ) {
	*dataType = DataTypeDouble;
	*tableOrMatrixSize = vertexUniformVarSize[ indVertexName ];
	*valScalars = vertexUniformVarValue[ indVertexName ]->GetScalars();
	// printf("shader tableOrMatrixSize (%d)\n" , *tableOrMatrixSize );
	return true;
      }
    }
  }
  return false;
}

void dm_ShaderSourceObject::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag   *XMLtag = new dm_XMLTag();
  int          nbVertexTexture = 0;
  int          nbGeometryTexture = 0;
  int          nbFragmentTexture = 0;
  int          textureRank = 0;

  fileName = new char*[nb_shaderTextures];
  encoding = new char*[nb_shaderTextures];
  materialTextures = new dm_Texture*[nb_shaderTextures];
  textureIndex = new int[nb_shaderTextures];
  textureTarget = new ShaderTextureTarget[nb_shaderTextures];
#ifdef DM_HAVE_FFMPEG
  materialVideo = new dm_Video*[nb_shaderTextures];
#endif

  for( int ind = 0 ; ind < nb_shaderTextures ; ind++ ) {
    fileName[ind] = NULL;
    encoding[ind] = NULL;
    
    InitString( &fileName[ind] );
    InitString( &encoding[ind] );
    materialTextures[ind] = new dm_Texture();
    materialTextures[ind]->mipmapped = true;
    materialTextures[ind]->rectangle = false;
    materialTextures[ind]->textureTarget = GL_TEXTURE_2D;
    textureIndex[ind] = _Nil;
    textureTarget[ind] = DM_FRAGMENT_TEXTURE;
#ifdef DM_HAVE_FFMPEG
    materialVideo[ind] = NULL;
#endif
  }

  // printf( "Parse shaderSource \n" );
  while( true ) {
    // new tag XML : shaderSource component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // shaderSource closing tag
    if( strcmp( XMLtag->tag , "shader_source" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {

      for( int ind = 0 ; ind < nb_shaderTextures ; ind++ ) {
	if( textureIndex[ind] == _Nil ) {
	  sprintf( ErrorStr , "Error: shaderSource texture #[%d] non allocated in shaderSource ID [%s]!" , ind + 1 , id ); ReportError( ErrorStr ); delete XMLtag; throw 291;
	}
      }
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    ///////////////////////////////////////////////////////////////
    // TEXTURE
    else if( strcmp( XMLtag->tag , "shader_source_texture" ) == 0 
	     && (XMLtag->tagType == EmptyTag) ) {
      int index = (int)XMLtag->getAttributeIntValue( "index" , false ) - 1;
      if( index >= 0 && index < nb_shaderTextures ) {
	dm_Texture *materialTexture = new dm_Texture();
	
	materialTexture->setParameters( XMLtag , 
					false ,            // compile time
					false ,            // not video texture
					assign , assignI );

	// texture target: vertex or fragment program (default = fragment)
	ShaderTextureTarget shaderTexture_Target = DM_FRAGMENT_TEXTURE;
	if( XMLtag->hasAttribute( "target" ) ) {
		//printf("Has target : ");
	  char * shaderTextureTarget_str 
	    = XMLtag->getAttributeStringValue( "target" );

	//	printf("%s \n ",shaderTextureTarget_str);
	  // texture target: vertex or fragment textures
	  shaderTexture_Target = EmptyShaderTextureTarget;
	  for( int ind = 0 ; ind < EmptyShaderTextureTarget ; ind++ ) {
	    //printf( "target [%s]\n" , ShaderTextureTargetString[ind] );
	    if( strcmp( shaderTextureTarget_str , 
		        ShaderTextureTargetString[ind] ) == 0 ) {
	      shaderTexture_Target = (ShaderTextureTarget)ind;
	      break;
	    }
	  }
	  if( shaderTexture_Target == EmptyShaderTextureTarget ) {
	    sprintf( ErrorStr , "Error: unknown shader texture target [%s]!" , shaderTextureTarget_str ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	  }
	}
	else{		
	//	printf("Does not declare target : %s \n",XMLtag->getAttributeStringValue( "xlink:href" ));
	}

	///////////////////////////////////////////////////////////////
	// TEXTURE RANK

	// texture rank: first vertex textures, then geometry textures, 
	// then fragment textures

	// TEXTURE indices are given BY CATEGORY: index 1->nbVertexTextures for vertex textures, 
	// 1->nbGeometryTextures for geometry textures... etc.

	// printf( "target [%s]\n" , ShaderTextureTargetString[shaderTexture_Target] );

	///////////////////////////////////////////////////////////////
	// fragment texture
	if( shaderTexture_Target == DM_FRAGMENT_TEXTURE ) {
	  if( nbFragmentTexture < index + 1 ) {
	    if( index + 1 + nbGeometryTexture + nbVertexTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures Total texture number (%d) outside expected range  [%d-%d] in component ID [%s] (fragment texture)!" , index + 1 + nbGeometryTexture + nbVertexTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    nbFragmentTexture = index + 1;
	  }
	  textureRank = nbVertexTexture + nbGeometryTexture + index;
	}

	///////////////////////////////////////////////////////////////
	// geometry texture
	else if( shaderTexture_Target == DM_GEOMETRY_TEXTURE ) {
 
	  // printf("\nTexture for Geometry Shader \n");

	  if( nbGeometryTexture < index + 1 ) {
	    if( index + 1 + nbFragmentTexture + nbVertexTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures (%d) outside expected range  [%d-%d] in component ID [%s] (geometry texture)!" , index + 1 + nbFragmentTexture + nbVertexTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    // fragment textures are moved above geometry textures
	    for( int ind = index + nbFragmentTexture + nbVertexTexture ; 
		 ind > index + nbVertexTexture ; ind-- ) {
#ifdef DM_HAVE_FFMPEG
	      materialVideo[ ind ] = materialVideo[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];
#endif
	      *(materialTextures[ ind ]) 
		= *(materialTextures[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ]);
	      StrCpy( &(fileName[ ind ]) , 
		      fileName[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ] );
	      StrCpy( &(encoding[ ind ]) , 
		      encoding[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ] );
	      textureTarget[ ind ] 
		= textureTarget[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];
	      textureIndex[ ind ] 
		= textureIndex[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];

	      InitString( &fileName[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] );
	      InitString( &encoding[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] );
	      materialTextures[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)]->init();
	      textureIndex[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = _Nil;
	      textureTarget[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = DM_GEOMETRY_TEXTURE;
#ifdef DM_HAVE_FFMPEG
	      materialVideo[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = NULL;
#endif
	    }
	    nbGeometryTexture = index + 1;
	  }
	  textureRank = nbVertexTexture + index;
	}

	///////////////////////////////////////////////////////////////
	// vertex texture
	else if( shaderTexture_Target == DM_VERTEX_TEXTURE ) {
	  if( nbVertexTexture < index + 1 ) {
	    if( index + 1 + nbFragmentTexture + nbGeometryTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures (%d) outside expected range  [%d-%d] in component ID [%s] (vertex texture)!" , index + 1 + nbFragmentTexture + nbGeometryTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    // fragment and geometry textures are moved above vertex textures
	    // (index - nbVertexTexture) shift
	    for( int ind = index + nbFragmentTexture + nbGeometryTexture ; 
		 ind > index ; ind-- ) {
#ifdef DM_HAVE_FFMPEG
	      materialVideo[ ind ] = materialVideo[ ind - (index + 1 - nbVertexTexture) ];
#endif
	      *(materialTextures[ ind ]) 
		= *(materialTextures[ ind - (index + 1 - nbVertexTexture) ]);
	      StrCpy( &(fileName[ ind ]) , 
		      fileName[ ind - (index + 1 - nbVertexTexture) ] );
	      StrCpy( &(encoding[ ind ]) , 
		      encoding[ ind - (index + 1 - nbVertexTexture) ] );
	      textureTarget[ ind ] 
		= textureTarget[ ind - (index + 1 - nbVertexTexture) ];
	      textureIndex[ ind ] 
		= textureIndex[ ind - (index + 1 - nbVertexTexture) ];

	      InitString( &fileName[ind - (index + 1 - nbVertexTexture)] );
	      InitString( &encoding[ind - (index + 1 - nbVertexTexture)] );
	      materialTextures[ind - (index + 1 - nbVertexTexture)]->init();
	      textureIndex[ind - (index + 1 - nbVertexTexture)] = _Nil;
	      textureTarget[ind - (index + 1 - nbVertexTexture)] = DM_VERTEX_TEXTURE;
#ifdef DM_HAVE_FFMPEG
	      materialVideo[ind - (index + 1 - nbVertexTexture)] = NULL;
#endif
	    }
	    nbVertexTexture = index + 1;
	  }
	  textureRank = index;
	}

	///////////////////////////////////////////////////////////////
	// verification of texture rank
	if( textureRank < 0 || textureRank >= nb_shaderTextures ) {
	  sprintf( ErrorStr , "Error: shaderSource texture rank (%d) outside expected range  [%d-%d] in component ID [%s] nb text vert/geom %d/%d target %d!" , textureRank + 1 , 1 , nb_shaderTextures , id , nbVertexTexture , nbGeometryTexture , shaderTexture_Target ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	}  
	
	*(materialTextures[ textureRank ]) = *materialTexture;

	// texture bound to a variable
	// references will be solved in a second pass
	if( materialTextures[ textureRank ]->textureGenerationMode 
	    == DM_VARIABLE_TEXTURE ) {

	  // raw encoding
	  materialTextures[ textureRank ]->encoding = RAW;

	  // materialTextures[ textureRank ]->rectangle = false;
	  // materialTextures[ textureRank ]->mipmapped = false;

	  // texture name  
	  if( XMLtag->hasAttribute( "xlink:href" ) ) {
	
	    char *fileName = XMLtag->getAttributeStringValue( "xlink:href" );
	    // strcpy( textureName , fileName );
	    StrCpyAndSubst( materialTextures[ textureRank ]->variableNodeName , fileName , XMLAttrValLength );
	  }
	  else {
	    sprintf( ErrorStr , "Error: missing xlink:href tag on variable texture [%s] in component ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); throw 206;
	  }
    
	  // texture variable name  
	  if( *(materialTextures[ textureRank ]->variableNodeName) && XMLtag->hasAttribute( "value" ) ) {
	    char *fileName = XMLtag->getAttributeStringValue( "value" );
	    // strcpy( textureName , fileName );
	    StrCpyAndSubst( materialTextures[ textureRank ]->variableName , fileName , XMLAttrValLength );
	  }
	  else {
	    sprintf( ErrorStr , "Error: unknown variable texture [%s] on node [%s] in component ID [%s]!" , materialTextures[ textureRank ]->variableName , XMLtag->tag , id ); ReportError( ErrorStr ); throw 206;
	  }
	  // printf( "parse variable shader texture %s %s\n" , materialTextures[ textureRank ]->variableNodeName , materialTextures[ textureRank ]->variableName );
	}
  
	// regular texture
	else {
	  // texture name  
	  char textureName[XMLAttrValLength];
	  *textureName = '\0';
	  if( XMLtag->hasAttribute( "xlink:href" ) ) {
	
	    char *fileName = XMLtag->getAttributeStringValue( "xlink:href" );
	    // strcpy( textureName , fileName );
	    StrCpyAndSubst( textureName , fileName , XMLAttrValLength );
	  }
	  materialTextures[ textureRank ]
	    ->LoadOrGenerateTexture( textureName ,
				     fileDepth ,
				     p_c , id , XMLtag , true );
	}

	//  texture target: fragment, geometry or vertex shader
	textureTarget[ textureRank ] = shaderTexture_Target;
	textureIndex[ textureRank ] = index;
      }
      else {
	sprintf( ErrorStr , "Error: shaderSource texture index (%d) outside expected range  [%d-%d] in component ID [%s]!" , index + 1 , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
      }  
    }
      
#ifdef DM_HAVE_FFMPEG
    // video
    else if( strcmp( XMLtag->tag , "shader_source_video" ) == 0 
	     && (XMLtag->tagType == EmptyTag) ) {
      int index = (int)XMLtag->getAttributeIntValue( "index" , false ) - 1;
      if( index >= 0 && index < nb_shaderTextures ) {

	char shaderSourcefileName[XMLAttrValLength];
	char *videoSource = NULL;
	int id_live_video = 0;
	
	// filme name  
	*shaderSourcefileName = '\0';
	if( XMLtag->hasAttribute( "xlink:href" ) ) {
	  char *fileName = XMLtag->getAttributeStringValue( "xlink:href" );
	  strcpy( shaderSourcefileName , fileName );
	}
	
	// texture name  
	if( XMLtag->hasAttribute( "video_source" ) ) {
	  videoSource = XMLtag->getAttributeStringValue( "video_source" );
	  if( strcmp( videoSource , "videolive" ) == 0 ) {
	    if( XMLtag->hasAttribute( "id_live_video" ) ) {
	      id_live_video =
		XMLtag->getAttributeIntValue( "id_live_video" , 
					      false );
	    }
	    sprintf( shaderSourcefileName , "/dev/video%d" , id_live_video );
	  }
	}

	// texture target: vertex or fragment program (default = fragment)
	ShaderTextureTarget shaderTexture_Target = DM_FRAGMENT_TEXTURE;
	if( XMLtag->hasAttribute( "target" ) ) {
	  char * shaderTextureTarget_str 
	    = XMLtag->getAttributeStringValue( "target" );
	  shaderTexture_Target = EmptyShaderTextureTarget;
	  for( int ind = 0 ; ind < EmptyShaderTextureTarget ; ind++ ) {
	    //printf( "encoding [%s]\n" , TextureEncodingString[ind] );
	    if( strcmp( shaderTextureTarget_str , 
		        ShaderTextureTargetString[ind] ) == 0 ) {
	      shaderTexture_Target = (ShaderTextureTarget)ind;
	      break;
	    }
	  }
	  if( shaderTexture_Target == EmptyShaderTextureTarget ) {
	    sprintf( ErrorStr , "Error: unknown shader texture target [%s]!" , shaderTextureTarget_str ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	  }
	}

	///////////////////////////////////////////////////////////////
	// fragment texture
	// texture rank: first vertex textures, then geometry textures, then fragment textures
	if( shaderTexture_Target == DM_FRAGMENT_TEXTURE ) {
	  if( nbFragmentTexture < index + 1 ) {
	    if( index + 1 + nbFragmentTexture + nbVertexTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures Total texture number (%d) outside expected range  [%d-%d] in component ID [%s] (fragment texture)!" , index + 1 + nbFragmentTexture + nbVertexTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    nbFragmentTexture = index + 1;
	  }
	  textureRank = nbVertexTexture + nbGeometryTexture + index;
	}

	///////////////////////////////////////////////////////////////
	// geometry texture
	else if( shaderTexture_Target == DM_GEOMETRY_TEXTURE ) {

	  if( nbGeometryTexture < index + 1 ) {
	    if( index + 1 + nbFragmentTexture + nbVertexTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures (%d) outside expected range  [%d-%d] in component ID [%s] (geometry texture)!" , index + 1 + nbFragmentTexture + nbVertexTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    // fragment textures are moved above geometry textures
	    for( int ind = index + nbFragmentTexture + nbGeometryTexture ; 
		 ind > index + nbVertexTexture ; ind-- ) {
	      materialVideo[ ind ] = materialVideo[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];
	      *(materialTextures[ ind ]) 
		= *(materialTextures[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ]);
	      StrCpy( &(fileName[ ind ]) , 
		      fileName[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ] );
	      StrCpy( &(encoding[ ind ]) , 
		      encoding[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ] );
	      textureTarget[ ind ] 
		= textureTarget[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];
	      textureIndex[ ind ] 
		= textureIndex[ ind - (index + 1 - nbVertexTexture - nbGeometryTexture) ];

	      InitString( &fileName[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] );
	      InitString( &encoding[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] );
	      materialTextures[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)]->init();
	      textureIndex[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = _Nil;
	      textureTarget[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = DM_GEOMETRY_TEXTURE;
#ifdef DM_HAVE_FFMPEG
	      materialVideo[ind - (index + 1 - nbVertexTexture - nbGeometryTexture)] = NULL;
#endif
	    }
	    nbGeometryTexture = nbVertexTexture + index ;
	  }
	  textureRank = nbVertexTexture + index;
	}

	///////////////////////////////////////////////////////////////
	// vertex texture
	else if( shaderTexture_Target == DM_VERTEX_TEXTURE ) {
	  if( nbVertexTexture < index + 1 ) {
	    if( index + 1 + nbFragmentTexture + nbGeometryTexture > nb_shaderTextures ) {
	      sprintf( ErrorStr , "Error: excessive number of shaderSource textures (%d) outside expected range  [%d-%d] in component ID [%s] (vertex texture)!" , index + 1 + nbFragmentTexture + nbGeometryTexture , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	    }  

	    // fragment textures are moved above vertex textures
	    // (index - nbVertexTexture) shift
	    for( int ind = index + nbFragmentTexture + nbGeometryTexture ; 
		 ind > index ; ind-- ) {
	      materialVideo[ ind ] = materialVideo[ ind - (index + 1 - nbVertexTexture) ];
	      *(materialTextures[ ind ]) = *(materialTextures[ ind - (index + 1 - nbVertexTexture) ]);
	      StrCpy( &(fileName[ ind ]) , fileName[ ind - (index + 1 - nbVertexTexture) ] );
	      StrCpy( &(encoding[ ind ]) , encoding[ ind - (index + 1 - nbVertexTexture) ] );
	      textureTarget[ ind ] = textureTarget[ ind - (index + 1 - nbVertexTexture) ];
	      textureIndex[ ind ] = textureIndex[ ind - (index + 1 - nbVertexTexture) ];

	      InitString( &fileName[ind - (index + 1 - nbVertexTexture)] );
	      InitString( &encoding[ind - (index + 1 - nbVertexTexture)] );
	      materialTextures[ind - (index + 1 - nbVertexTexture)]->init();
	      textureIndex[ind - (index + 1 - nbVertexTexture)] = _Nil;
	      textureTarget[ind - (index + 1 - nbVertexTexture)] = DM_VERTEX_TEXTURE;
	      materialVideo[ind - (index + 1 - nbVertexTexture)] = NULL;
	    }
	    nbVertexTexture = index + 1;
	  }
	  textureRank = index;
	}

	///////////////////////////////////////////////////////////////
	// verification of texture rank
	if( textureRank < 0 || textureRank >= nb_shaderTextures ) {
	  sprintf( ErrorStr , "Error: shaderSource video texture rank (%d) outside expected range  [%d-%d] in component ID [%s]!" , textureRank + 1 , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
	}  

	// texture no is associated with film name
	if( *(shaderSourcefileName) ) {
	  //printf( "textureNo %d %s\n" , index , shaderSourcefileName );
	  bool newText;

	  // texture for a film: just a name and an ID
	  // sets a texture ID that will be used to refer to the
	  // video frame
	  TextureEncoding t_enc = RAW;
	  materialTextures[ textureRank ]->SetTextureNo( 
	    DM_CompositionNode->FindTexture( shaderSourcefileName , 
					     &t_enc , 2 , false ,
					     DM_DIRECT_TEXTURING , 
					     &newText ,
					     true ) );
	  //printf( "textureNo %d %s\n" , textureNo[ index ] , shaderSourcefileName );
	  
	  // printf( "new texture textureNo %d %s\n" , textureNo[ index ] , shaderSourcefileName );
	  // ID texture is generated from the beginning 
	  // because texture will be bound to several images
	  // but ID will not change
	  materialVideo[ textureRank ] = new dm_Video();
	  materialVideo[ textureRank ]->setParameters( XMLtag , 
						       false ,            // compile time
						       newText ,
						       assign , assignI );

	  if( newText ) {
	    // video net yet allocated: pointer recorded	
	    DM_CompositionNode->video_records[ 
		     materialTextures[ textureRank ]->GetTextureNo() ] 
					       = materialVideo[ textureRank ];

	    materialVideo[ textureRank ]->filmAudioVideo->
	      loadFilm( shaderSourcefileName ,
			materialVideo[ textureRank ]->id_live_video ,
			materialVideo[ textureRank ]->live_video_w ,
			materialVideo[ textureRank ]->live_video_h ,
			materialVideo[ textureRank ]->rectangle ,
			materialVideo[ textureRank ]->external );
	    //materialVideo[ textureRank ]->print( stdout , 1 , 1 );
	  }
	  else {
	    // video already allocated
	    dm_Video *previousReference =  DM_CompositionNode->FindVideo( shaderSourcefileName );
	    
	    if( !previousReference ) {
	      sprintf( ErrorStr , "Error: missing previous video reference [%s]!" ,shaderSourcefileName ); ReportError( ErrorStr ); throw 422;
	    }

	    // printf( "known texture textureNo %d %s\n" , textureNo[ index ] , shaderSourcefileName );
	    materialVideo[ textureRank ]->filmAudioVideo = previousReference->filmAudioVideo;
	  }
	}

	bool is_mipmapped = true;
	if( XMLtag->hasAttribute( "mipmapped" ) ) {
	  XMLtag->BooleanFieldValue( "mipmapped" ,
				     &(is_mipmapped) );
	}

	bool is_rectangle = false;
	if( XMLtag->hasAttribute( "rectangle" ) ) {
	  XMLtag->BooleanFieldValue( "rectangle" ,
				     &(is_rectangle) );
	}

	// the texture is transformed from a height
	// map to a shaderSource mapping texture
	materialTextures[ textureRank ]->encoding = RAW;

	materialTextures[ textureRank ]->textureGenerationMode = DM_BITMAP_TEXTURE;

	materialTextures[ textureRank ]->mipmapped = is_mipmapped;
	materialTextures[ textureRank ]->rectangle = is_rectangle;

	materialTextures[ textureRank ]->textureTarget = GL_TEXTURE_2D;
	
	StrCpy( &(fileName[ textureRank ]) , shaderSourcefileName );
	StrCpy( &(encoding[ textureRank ]) , "raw" );
	
	//  texture target: fragment, geometry or vertex shader
	
	textureTarget[ textureRank ] = shaderTexture_Target;
	textureIndex[ textureRank ] = index;

      }
      else {
	sprintf( ErrorStr , "Error: shaderSource texture index (%d) outside expected range  [%d-%d] in component ID [%s]!" , index + 1 , 1 , nb_shaderTextures , id ); ReportError( ErrorStr ); delete XMLtag; throw 10;
      }  
    }
#endif
      
    // unknown tag
    else {
      sprintf( ErrorStr , "Error: unknown shaderSource component tag [%s] in shaderSource ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); delete XMLtag; throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_ShaderSourceObject::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<shader_source id=\"%s\" nb_textures=\"%d\" nb_attachments=\"%d\" message_display=\"%s\"" , 
	   id , nb_shaderTextures , nb_attachments , BooleanString[BooleanToInt(message_display)] );
  for( int indFragmentName = 0 ; 
       indFragmentName < nb_fragmentUniformVars ; 
       indFragmentName++ ) {
    fprintf( file , " %s=" , 
	     fragmentUniformVarName[indFragmentName] );
    int size = fragmentUniformVarSize[ indFragmentName ];
    if( size == 1 ) {
      fprintf( file , "\"%.5f\"" , 
	       fragmentUniformVarValue[indFragmentName]->GetNumericalValue( 0 ) );
    }
    else {
      fprintf( file , "\"(" );
      for( int ind = 0 ; ind < size ; ind++ ) {
	fprintf( file , " %.5f" , 
		 fragmentUniformVarValue[indFragmentName]->GetNumericalValue( ind ) );
	if( ind < size - 1 ) {
	  fprintf( file , "," );
	}
      }
      fprintf( file , ")\"" );
    }
  }
    for( int indGeometryName = 0 ; 
       indGeometryName < nb_geometryUniformVars ; 
       indGeometryName++ ) {
    fprintf( file , " %s=" , 
	     geometryUniformVarName[indGeometryName] );
    int size = geometryUniformVarSize[ indGeometryName ];
    if( size == 1 ) {
      fprintf( file , "\"%.5f\"" , 
	       geometryUniformVarValue[indGeometryName]->GetNumericalValue( 0 ) );
    }
    else {
      fprintf( file , "\"(" );
      for( int ind = 0 ; ind < size ; ind++ ) {
	fprintf( file , " %.5f" , 
		 geometryUniformVarValue[indGeometryName]->GetNumericalValue( ind ) );
	if( ind < size - 1 ) {
	  fprintf( file , "," );
	}
      }
      fprintf( file , ")\"" );
    }
  }
  for( int indVertexName = 0 ; 
       indVertexName < nb_vertexUniformVars ; 
       indVertexName++ ) {
    fprintf( file , " %s=" , 
	     vertexUniformVarName[indVertexName] );
    int size = vertexUniformVarSize[ indVertexName ];
    if( size == 1 ) {
      fprintf( file , "\"%.5f\"" , 
	       vertexUniformVarValue[indVertexName]->GetNumericalValue( 0 ) );
    }
    else {
      fprintf( file , "\"(" );
      for( int ind = 0 ; ind < size ; ind++ ) {
	fprintf( file , " %.5f" , 
		 vertexUniformVarValue[indVertexName]->GetNumericalValue( ind ) );
	if( ind < size - 1 ) {
	  fprintf( file , "," );
	}
      }
      fprintf( file , ")\"" );
    }
  }
  fprintf( file , ">\n" ); 

  if( isLong ) {
    for( int ind = 0 ; ind < nb_shaderTextures ; ind++ ) {
      indent( file , depth + 1 );
#ifdef DM_HAVE_FFMPEG
      if( materialVideo[ ind ] ) {
	fprintf( file , "<shader_source_video index=\"%d\" id=\"%s\" xlink:href=\"%s\" type=\"%s\" video_source=\"%s\" target=\"%s\" begin=\"%.5f\" end=\"%.5f\" duration=\"%.5f\" period=\"%.5f\"/>\n" , 
	   ind + 1 , id , materialVideo[ ind ]->fileName , VideoTypeString[materialVideo[ ind ]->type] , VideoSourceString[materialVideo[ ind ]->video_source]  , ShaderTextureTargetString[textureTarget[ ind ]]  , materialVideo[ ind ]->begin , materialVideo[ ind ]->end , materialVideo[ ind ]->duration , materialVideo[ ind ]->period );
      }
      else 
#endif
      {
	fprintf( file , "<shader_source_texture index=\"%d\" id=\"%s\" encoding=\"%s\" target=\"%s\" xlink:href=\"%s\"/>\n" , textureIndex[ ind ] + 1 , id , encoding[ind] , ShaderTextureTargetString[textureTarget[ ind ]]  , DM_CompositionNode->texture_names[materialTextures[ind]->GetTextureNo()] );
      }
    }
  }
  indent( file , depth );
  fprintf( file , "</shader_source>\n" );
}

void dm_ShaderSourceObject::operator=(dm_ShaderSourceObject& c) {
  this->dm_NonGeometricalObject::operator=(c);

  if (vertexUniformVarName) {
    for (int ind = 0; ind < nb_vertexUniformVars; ind++)
      {
	delete vertexUniformVarName[ind];
	vertexUniformVarName[ind] = NULL;
      }

    delete [] vertexUniformVarName;
    vertexUniformVarName = NULL;
  }

  if (vertexUniformVarSize) {
    delete [] vertexUniformVarSize;
    vertexUniformVarSize = NULL;
  }

  if (vertexUniformVarValue) {
    for (int ind = 0; ind < nb_vertexUniformVars; ind++)
      {
	delete vertexUniformVarValue[ind];
	vertexUniformVarValue[ind] = NULL;
      }

    delete [] vertexUniformVarValue;
    vertexUniformVarValue = NULL;
  }





  if (geometryUniformVarName) {
    for (int ind = 0; ind < nb_geometryUniformVars; ind++)
      {
	delete geometryUniformVarName[ind];
	geometryUniformVarName[ind] = NULL;
      }

    delete [] geometryUniformVarName;
    geometryUniformVarName = NULL;
  }

  if (geometryUniformVarSize) {
    delete [] geometryUniformVarSize;
    geometryUniformVarSize = NULL;
  }

  if (geometryUniformVarValue) {
    for (int ind = 0; ind < nb_geometryUniformVars; ind++)
      {
	delete geometryUniformVarValue[ind];
	geometryUniformVarValue[ind] = NULL;
      }

    delete [] geometryUniformVarValue;
    geometryUniformVarValue = NULL;
  }





  if (fragmentUniformVarName) {
    for (int ind = 0; ind < nb_fragmentUniformVars; ind++)
      {
	delete fragmentUniformVarName[ind];
	fragmentUniformVarName[ind] = NULL;
      }

    delete [] fragmentUniformVarName;
    fragmentUniformVarName = NULL;
  }

  if (fragmentUniformVarSize) {
     delete [] fragmentUniformVarSize;
    fragmentUniformVarSize = NULL;
  }

  if (fragmentUniformVarValue) {
    for (int ind = 0; ind < nb_fragmentUniformVars; ind++)
      {
	delete fragmentUniformVarValue[ind];
	fragmentUniformVarValue[ind] = NULL;
      }

    delete [] fragmentUniformVarValue;
    fragmentUniformVarValue = NULL;
  }

  fileName = c.fileName;
  encoding = c.encoding;
  materialTextures = c.materialTextures;
  textureIndex = c.textureIndex;
  textureTarget = c.textureTarget;
#ifdef DM_HAVE_FFMPEG
  materialVideo = c.materialVideo;
#endif
  nb_shaderTextures = c.nb_shaderTextures;
  message_display = c.message_display;
  nb_attachments = c.nb_attachments;
  active_texture = c.active_texture;
  // predefined fragment shader or vertex program name
  nb_fragmentUniformVars = c.nb_fragmentUniformVars;
  nb_geometryUniformVars = c.nb_geometryUniformVars;
  nb_vertexUniformVars = c.nb_vertexUniformVars;

  

  if( !fragmentUniformVarName || !fragmentUniformVarValue || !vertexUniformVarName || !vertexUniformVarValue || !geometryUniformVarName || !geometryUniformVarValue ) {
    fragmentUniformVarName = new char*[nb_fragmentUniformVars];
    fragmentUniformVarSize = new int[nb_fragmentUniformVars];
    fragmentUniformVarValue = new dm_Table*[nb_fragmentUniformVars];
    vertexUniformVarName = new char*[nb_vertexUniformVars];
    vertexUniformVarSize = new int[nb_vertexUniformVars];
    vertexUniformVarValue = new dm_Table*[nb_vertexUniformVars];
    geometryUniformVarName = new char*[nb_geometryUniformVars];
    geometryUniformVarSize = new int[nb_geometryUniformVars];
    geometryUniformVarValue = new dm_Table*[nb_geometryUniformVars];

    for( int indFragmentName = 0 ; 
	 indFragmentName < nb_fragmentUniformVars ; 
	 indFragmentName++ ) {
      fragmentUniformVarName[indFragmentName] = NULL;
      fragmentUniformVarSize[indFragmentName] = 1;
      fragmentUniformVarValue[indFragmentName] 
	= new dm_Table( fragmentUniformVarSize[indFragmentName] );
    }
    for( int indGeometryName = 0 ; 
	 indGeometryName < nb_geometryUniformVars ; 
	 indGeometryName++ ) {
      geometryUniformVarName[indGeometryName] = NULL;
      geometryUniformVarSize[indGeometryName] = 1;
      geometryUniformVarValue[indGeometryName] 
	= new dm_Table( geometryUniformVarSize[indGeometryName] );
    }
    for( int indVertexName = 0 ; 
	 indVertexName < nb_vertexUniformVars ; 
	 indVertexName++ ) {
      vertexUniformVarName[indVertexName] = NULL;
      vertexUniformVarSize[indVertexName] = 1;
      vertexUniformVarValue[indVertexName] 
	= new dm_Table( vertexUniformVarSize[indVertexName] );
    }
  }
  for( int indFragmentName = 0 ; 
       indFragmentName < nb_fragmentUniformVars ; 
       indFragmentName++ ) {
    StrCpy( &fragmentUniformVarName[ indFragmentName ] , 
	    c.fragmentUniformVarName[ indFragmentName ] );
    fragmentUniformVarSize[ indFragmentName ]  
      = c.fragmentUniformVarSize[ indFragmentName ];
    for( int ind = 0 ; ind < fragmentUniformVarSize[indFragmentName] ; ind++ ) {
      fragmentUniformVarValue[ indFragmentName ]->SetValue(   
       c.fragmentUniformVarValue[ indFragmentName ]->GetNumericalValue( ind ) , 
       ind );
    }
  }
  for( int indGeometryName = 0 ; 
       indGeometryName < nb_geometryUniformVars ; 
       indGeometryName++ ) {
    StrCpy( &geometryUniformVarName[ indGeometryName ] , 
	    c.geometryUniformVarName[ indGeometryName ] );
    geometryUniformVarSize[ indGeometryName ]  
      = c.geometryUniformVarSize[ indGeometryName ];
    for( int ind = 0 ; ind < geometryUniformVarSize[indGeometryName] ; ind++ ) {
      geometryUniformVarValue[ indGeometryName ]->SetValue(   
       c.geometryUniformVarValue[ indGeometryName ]->GetNumericalValue( ind ) , 
       ind );
    }
  }
  for( int indVertexName = 0 ; 
       indVertexName < nb_vertexUniformVars ; 
       indVertexName++ ) {
    StrCpy( &vertexUniformVarName[ indVertexName ] , 
	    c.vertexUniformVarName[ indVertexName ] );
    vertexUniformVarSize[ indVertexName ]  
      = c.vertexUniformVarSize[ indVertexName ];
    for( int ind = 0 ; ind < vertexUniformVarSize[indVertexName] ; ind++ ) {
      vertexUniformVarValue[ indVertexName ]->SetValue(   
       c.vertexUniformVarValue[ indVertexName ]->GetNumericalValue( ind ) , 
       ind );
    }
  }
}

dm_Viewpoint::dm_Viewpoint( void ): dm_NonGeometricalObject()  {
  pitch_head = 0;
  yaw_head = 0;
  type = DM_ORIENTABLE_CAMERA;
  
  currentPointAbsolute = NULL;

  vertical.x = 0.0; 
  vertical.y = 1.0; 
  vertical.z = 0.0;
}

dm_Viewpoint::~dm_Viewpoint( void ) {
  // this pointer refers to another scene object in the scene
  // should not be deallocated otherwise the corresponding
  // object will be deallocated twice

  if (currentPointAbsolute) {
    delete currentPointAbsolute;
    currentPointAbsolute = NULL;
  }
}

void dm_Viewpoint::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    type = DM_ORIENTABLE_CAMERA;
  }
  if( XMLtag->hasAttribute( "type"  ) ) {
    char *type_str = XMLtag->getAttributeStringValue( "type" );
    type = EmptyViewPointType;
    for( int ind = 0 ; ind < EmptyViewPointType ; ind++ ) {
      if( strcmp( type_str , ViewPointTypeString[ind] ) == 0 ) {
	type = (ViewPointType)ind;
	break;
      }
    }
    if( type == EmptyViewPointType ) {
      sprintf( ErrorStr , "Error: unknown viewpoint type [%s] !" , type_str ); ReportError( ErrorStr ); throw 203;
    }
  }

  if( !atRunTime ) {
    vertical.x = 0.0;
    vertical.y = 1.0;
    vertical.z = 0.0;
  }
  if( XMLtag->hasAttribute( "x" ) ) {
    oper( vertical.x , (float)XMLtag->getAttributeDoubleValue( "x" , 
							       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y" ) ) {
    oper( vertical.x , (float)XMLtag->getAttributeDoubleValue( "y" , 
							       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z" ) ) {
    oper( vertical.x , (float)XMLtag->getAttributeDoubleValue( "z" , 
							       atRunTime ));
  }
  vertical.normalize();
}

bool dm_Viewpoint::getParameters( char *attribute , 
				  DataType * dataType ,
				  double * valDouble ,
				  int * valInt ,
				  bool * valBool ,
				  char ** valString ,
				  int * tableOrMatrixSize ,
				  dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "x" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)vertical.x;
    return true;
  }
  else if( strcmp( attribute , "y" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)vertical.x;
    return true;
  }
  else if( strcmp( attribute , "z" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)vertical.z;
    return true;
  }
  return false;
}

void dm_Viewpoint::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<viewpoint id=\"%s\" type=\"%s\" />\n" , 
	   id , ViewPointTypeString[type] );
}

void dm_Viewpoint::display( DrawingMode mode , bool *invalidDL ) {
}

void dm_Viewpoint::operator=(dm_Viewpoint& c) {
  this->dm_NonGeometricalObject::operator=(c);

  pitch_head = c.pitch_head;
  yaw_head = c.yaw_head;
  type = c.type;
  vertical = c.vertical;
}

dm_Hearpoint::dm_Hearpoint( void ): dm_NonGeometricalObject()  {
  active = false;
}

dm_Hearpoint::~dm_Hearpoint( void ) {
  // TODO
}

void dm_Hearpoint::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    active = false;
  }
  if( XMLtag->hasAttribute( "active"  ) ) {
    XMLtag->BooleanFieldValue( "active" ,
			       &(active) );
  }
}

bool dm_Hearpoint::getParameters( char *attribute , 
				  DataType * dataType ,
				  double * valDouble ,
				  int * valInt ,
				  bool * valBool ,
				  char ** valString ,
				  int * tableOrMatrixSize ,
				  dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "active" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = active;
    return true;
  }
  return false;
}

void dm_Hearpoint::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<hearpoint id=\"%s\" active=\"%s\"/>\n" , 
	   id , BooleanString[BooleanToInt(active)] );
}

void dm_Hearpoint::display( DrawingMode mode , bool *invalidDL ) {
}

void dm_Hearpoint::operator=(dm_Hearpoint& c) {
  this->dm_NonGeometricalObject::operator=(c);
  active = c.active;
}

dm_Transformation::dm_Transformation( void ): dm_NonGeometricalObject() {
  valScalarMatrix = NULL;
  type = EmptyTransformation;
  aVector.update( 1, 0, 0 );
  aQuatIm.update( 0, 0, 0 );
  aQuat_w = 1;
  aAngle = 0;
  rotationUpdated = false;
  id[0] = 0;
  // initial transformation is Identity
  memset( transformationMatrix , 0 , 16 * sizeof( float ) );
  transformationMatrix[0] = 1;
  transformationMatrix[5] = 1;
  transformationMatrix[10] = 1;
  transformationMatrix[15] = 1;
}

dm_Transformation::~dm_Transformation( void ) {
  if (valScalarMatrix) {
    delete [] valScalarMatrix;
    valScalarMatrix = NULL;
  }
}

void dm_Transformation::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				       FloatBinOp oper , IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    char *typeString = XMLtag->getAttributeStringValue( "geometry" );
    type = EmptyTransformation;
    int ind;
    for( ind = 0 ; ind < EmptyTransformation + 1 ; ind ++ ){
      if( strcmp( typeString , TransformationTypeString[ind] ) == 0 ) {
	type = (TransformationType)ind;
	break;
      }
    }
    // empty transformations correspond to empty nodes
    // used to group nodes
    if( ind == EmptyTransformation + 1 ) {
      sprintf( ErrorStr , "Error: unknown transformation geometry [%s] in node [%s]!" , typeString , id ); ReportError( ErrorStr ); throw 220;
    }
  }

  // all the classical transformations: translations, scales, rotations 
  // and path or curve transformations
  if( type != AMatrix ) {

    ////////////////////////////////////////////////////////////
    // new syntax: rotation and translation components
    // are given in the XML tag in order to allow for
    // easier scripting
    ////////////////////////////////////////////////////////////
  
    // vector
    if( !atRunTime ) {
      aQuatIm.update( 0, 0, 0 );
      aVector.update( 0, 0, 0 );
      aQuat_w = 0.0;
      aAngle = 0.0;
    }

    float 
      x_loc = 0, 
      y_loc = 0, 
      z_loc = 0, 
      w_loc = 0, 
      angle_loc = 0;
    bool 
      x_updated = false, 
      y_updated = false, 
      z_updated = false, 
      w_updated = false, 
      angle_updated = false;

    if( XMLtag->hasAttribute( "x"  ) ) {
      x_loc = (float)XMLtag->getAttributeDoubleValue( "x" , atRunTime );
      x_updated = true;
    }
    if( XMLtag->hasAttribute( "y"  ) ) {
      y_loc = (float)XMLtag->getAttributeDoubleValue( "y" , atRunTime );
      y_updated = true;
    }
    if( XMLtag->hasAttribute( "z"  ) ) {
      z_loc = (float)XMLtag->getAttributeDoubleValue( "z" , atRunTime );
      z_updated = true;
    }
    if( XMLtag->hasAttribute( "w"  ) ) {
      w_loc = (float)XMLtag->getAttributeDoubleValue( "w" , atRunTime );
      w_updated = true;
      if( type != AQuaternion ) {
	sprintf( ErrorStr , "Error: only quaternions use w attribute [%s]!" , id ); ReportError( ErrorStr );
      }
    }
    // angle
    if( XMLtag->hasAttribute( "angle"  ) ) {
      angle_loc = (float)XMLtag->getAttributeDoubleValue( "angle" , atRunTime );
      angle_updated = true;
      if( type != ARotation ) {
	sprintf( ErrorStr , "Error: only rotations use angle attribute [%s]!" , id ); ReportError( ErrorStr );
      }
    }
  
    // angle => rotation input
    if( angle_updated && !w_updated ) {
      if( x_updated ) {
	oper( aVector.x , x_loc );
      }
      if( y_updated ) {
	oper( aVector.y , y_loc );
      }
      if( z_updated ) {
	oper( aVector.z , z_loc );
      }
      oper( aAngle , angle_loc );

      if( type == AQuaternion ) {
	normalize();
	AngleAxisToQuaternion();      
      }
    }
    // w => quaternion input
    else if( !angle_updated && w_updated ) {
      if( x_updated ) {
	oper( aQuatIm.x , x_loc );
      }
      if( y_updated ) {
	oper( aQuatIm.y , y_loc );
      }
      if( z_updated ) {
	oper( aQuatIm.z , z_loc );
      }
      oper( aQuat_w , w_loc );

      normalize();
      QuaternionToAngleAxis();      
    }
    // not w not angle => quaternion input if quaternion, angle/axis
    // input otherwise
    else if( !angle_updated && !w_updated ) {
      if( type == AQuaternion ) {
	if( x_updated ) {
	  oper( aQuatIm.x , x_loc );
	}
	if( y_updated ) {
	  oper( aQuatIm.y , y_loc );
	}
	if( z_updated ) {
	  oper( aQuatIm.z , z_loc );
	}

	normalize();
	QuaternionToAngleAxis();      
      }
      else {
	if( x_updated ) {
	  oper( aVector.x , x_loc );
	}
	if( y_updated ) {
	  oper( aVector.y , y_loc );
	}
	if( z_updated ) {
	  oper( aVector.z , z_loc );
	}

	if( type == AQuaternion ) {
	  normalize();
	  AngleAxisToQuaternion();      
	}
      }
    }
    // not w not angle => quaternion input if quaternion, angle/axis
    // input otherwise
    else { // if( angle_updated && w_updated )
      // no update
      sprintf( ErrorStr , "Error: ambiguous quaternion and angle axis rotation input transformation [%s]!" , id ); ReportError( ErrorStr );
    }

  }
  // matrix transformation
  else {
    if( XMLtag->hasAttribute( "m_1_1" ) ) {
      oper( transformationMatrix[0] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_1_1" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_2_1" ) ) {
      oper( transformationMatrix[1] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_2_1" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_3_1" ) ) {
      oper( transformationMatrix[2] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_3_1" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_4_1" ) ) {
      oper( transformationMatrix[3] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_4_1" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_1_2" ) ) {
      oper( transformationMatrix[4] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_1_2" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_2_2" ) ) {
      oper( transformationMatrix[5] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_2_2" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_3_2" ) ) {
      oper( transformationMatrix[6] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_3_2" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_4_2" ) ) {
      oper( transformationMatrix[7] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_4_2" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_1_3" ) ) {
      oper( transformationMatrix[8] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_1_3" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_2_3" ) ) {
      oper( transformationMatrix[9] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_2_3" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_3_3" ) ) {
      oper( transformationMatrix[10] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_3_3" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_4_3" ) ) {
      oper( transformationMatrix[11] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_4_3" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_1_4" ) ) {
      oper( transformationMatrix[12] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_1_4" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_2_4" ) ) {
      oper( transformationMatrix[13] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_2_4" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_3_4" ) ) {
      oper( transformationMatrix[14] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_3_4" , 
						    atRunTime ));
    }
    if( XMLtag->hasAttribute( "m_4_4" ) ) {
      oper( transformationMatrix[15] , 
	    (float)XMLtag->getAttributeDoubleValue( "m_4_4" , 
						    atRunTime ));
    }
    // COLUMNS
    // a 4-float tables are expected
    if( XMLtag->hasAttribute( "m__1" ) ) {
      double   newTabValues[ 4 ];
      
      XMLtag->getAttributeTableDoubleValue( newTabValues , 4 , 
					    "m__1" , atRunTime );
      for( int ind = 0 ; ind < 4 ; ind++ ) {
	oper(  transformationMatrix[ ind ] , (float)newTabValues [ ind ] );
      }
    }
    if( XMLtag->hasAttribute( "m__2" ) ) {
      double   newTabValues[ 4 ];
      
      XMLtag->getAttributeTableDoubleValue( newTabValues , 4 , 
					    "m__2" , atRunTime );
      for( int ind = 4 ; ind < 8 ; ind++ ) {
	oper(  transformationMatrix[ ind ] , (float)newTabValues [ ind - 4 ] );
      }
    }
    if( XMLtag->hasAttribute( "m__3" ) ) {
      double   newTabValues[ 4 ];
      
      XMLtag->getAttributeTableDoubleValue( newTabValues , 4 , 
					    "m__3" , atRunTime );
      for( int ind = 8 ; ind < 12 ; ind++ ) {
	oper(  transformationMatrix[ ind ] , (float)newTabValues [ ind - 8 ] );
      }
    }
    if( XMLtag->hasAttribute( "m__4" ) ) {
      double   newTabValues[ 4 ];
      
      XMLtag->getAttributeTableDoubleValue( newTabValues , 4 , 
					    "m__4" , atRunTime );
      for( int ind = 12 ; ind < 16 ; ind++ ) {
	oper(  transformationMatrix[ ind ] , (float)newTabValues [ ind - 12 ] );
      }
    }
    // a 16-float table is expected
    if( XMLtag->hasAttribute( "matrix" ) ) {
      double   newTabValues[ 16 ];
      XMLtag->getAttributeTableDoubleValue( newTabValues , 16 , 
					    "matrix" , atRunTime );

      for( int ind = 0 ; ind < 16 ; ind++ ) {
	oper(  transformationMatrix[ ind ] , (float)newTabValues [ ind ] );
      }
    }
  }
}

void dm_Transformation::parse( int *fileDepth , int *p_c , 
			       dm_SceneNode *sourceNode ) {
  dm_XMLTag   *XMLtag = new dm_XMLTag();

  while( true ) {
    // new tag XML : transformation component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // transformation closing tag
    if( strcmp( XMLtag->tag , "transformation" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {

      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    ////////////////////////////////////////////////////////////
    // new syntax: rotation and translation components
    // are given in the XML tag in order to allow for
    // easier scripting
    ////////////////////////////////////////////////////////////

    // DEPRECATED: vector
    else if( strcmp( XMLtag->tag , "vector" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      aVector.x 
	=  ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aVector.y 
	=  ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      aVector.z 
	=  ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );

      // expecting vector closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  "vector" , NULL , NULL );
    }

    else {
	sprintf( ErrorStr , "Error: unknown transformation component tag [%s] (vector or angle expected) in transformation ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); delete XMLtag; throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

bool dm_Transformation::getParameters( char *attribute , 
				       DataType * dataType ,
				       double * valDouble ,
				       int * valInt ,
				       bool * valBool ,
				       char ** valString ,
				       int * tableOrMatrixSize ,
				       dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "x" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( type == ATranslation || type == ARotation || type == AScale ) {
      *valDouble = (double)aVector.x;
      return true;
    }
    else if( type == AQuaternion ) {
      *valDouble = (double)aQuatIm.x;
      return true;
    }
    else {
      *valDouble = 0.0;
      return true;
    }
  }
  else if( strcmp( attribute , "y" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( type == ATranslation || type == ARotation || type == AScale ) {
      *valDouble = (double)aVector.y;
      return true;
    }
    else if( type == AQuaternion ) {
      *valDouble = (double)aQuatIm.y;
      return true;
    }
    else {
      *valDouble = 0.0;
      return true;
    }
  }
  else if( strcmp( attribute , "z" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( type == ATranslation || type == ARotation || type == AScale ) {
      *valDouble = (double)aVector.z;
      return true;
    }
    else if( type == AQuaternion ) {
      *valDouble = (double)aQuatIm.z;
      return true;
    }
    else {
      *valDouble = 0.0;
      return true;
    }
  }
  else if( strcmp( attribute , "w" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( type == AQuaternion ) {
      *valDouble = (double)aQuat_w;
      return true;
    }
    else {
      *valDouble = 0.0;
      return true;
    }
  }
  else if( strcmp( attribute , "yaw" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = 0.0;
    return true;
  }
  else if( strcmp( attribute , "pitch" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = 0.0;
    return true;
  }
  else if( strcmp( attribute , "roll" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = 0.0;
    return true;
  }
  else if( strcmp( attribute , "angle" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)aAngle;
    return true;
  }
  else if( strcmp( attribute , "matrix" ) == 0 
	   || (type == AMatrix && strncmp( attribute , "m_" , 2 ) == 0) ) {
    //     bool dm_Transformation::getParameters( char *attribute , 
    // 				       DataType * dataType ,
    // 				       double * valDouble ,
    // 				       int * valInt ,
    // 				       bool * valBool ,
    // 				       char ** valString ,
    // 				       int * tableOrMatrixSize ,
    // 				       dm_ValScalar ** valScalars ) {

    // returned data type
    *dataType = DataTypeDouble;

    // builds the matrix for non matrix transformation
    // stores it in the temporary matrix returnedValue
    dm_matrix44 returnedValue;
    if( type == ATranslation ) {
      returnedValue.EulertoMatrix( aVector.x , aVector.y , aVector.z , 
				   0.0 , 0.0 , 0.0 );
    }
    else if( type == ARotation ) {
      normalize();
      AngleAxisToQuaternion();      
      returnedValue.QuaterniontoMatrix( aQuat_w , 
					aQuatIm.x , aQuatIm.y , aQuatIm.z );
    }
    else if( type == AScale ) {
      returnedValue.ScaletoMatrix( aVector.x , aVector.y , aVector.z );
    }
    else if( type == AQuaternion ) {
      returnedValue.QuaterniontoMatrix( aQuat_w , 
					aQuatIm.x , aQuatIm.y , aQuatIm.z );
    }

    // full matrix
    if( strcmp( attribute , "matrix" ) == 0 ) {
      // returned table size
      *tableOrMatrixSize = 16;
      // returned table parameter
      // uses the local storage 
      if( !valScalarMatrix ) {
	valScalarMatrix = new dm_ValScalar[ 16 ];
      }
      *valScalars = valScalarMatrix;

      // transfers the temporary storage of the matrix
      // into the transformation variable valScalarMatrix
      float * matValues = returnedValue.GetValue();
      for( int ind = 0 ; ind < 16 ; ind++ ) {
	valScalarMatrix[ind].SetType( DataTypeDouble );
	// for non matrix type: computed previously into returnedValue
	if( type != AMatrix ) {
	  valScalarMatrix[ ind ].SetValue( matValues[ ind ] );
	}
	// for matrix type: stored into transformationMatrix
	else {
	  valScalarMatrix[ ind ].SetValue( transformationMatrix[ ind ] );
	}
      }
    }
    // matrix column or element
    // this is only allowed for matrix type. 
    // if it is ever used for other type, it should be checked that
    // the matrix is only built once after changes in the values
    // the preceding values should be stored to rebuild the matrix
    // only if they have changed
    // MATRIX COLUMN
    else if( type == AMatrix && strncmp( attribute , "m__" , 3 ) == 0 ) {
      int column_rank = atoi( attribute + 3 ) - 1;

      // returned table size
      *tableOrMatrixSize = 4;
      // returned table parameter
      // uses the local storage 
      if( !valScalarMatrix ) {
	valScalarMatrix = new dm_ValScalar[ 16 ];
      }
      *valScalars = valScalarMatrix + column_rank * 4;

      for( int ind = column_rank * 4 ; ind < column_rank * 4 + 4 ; ind++ ) {
	valScalarMatrix[ind].SetType( DataTypeDouble );
	// for matrix type: stored into transformationMatrix
	valScalarMatrix[ ind ].SetValue( transformationMatrix[ ind ] );
      }
    }
    // ELEMENT
    else if( type == AMatrix && strncmp( attribute , "m_" , 2 ) == 0 ) {
      int line_rank = atoi( attribute + 2 ) - 1;
      char * striped_attribute = strchr( attribute + 2 , '_' );
      int column_rank = atoi( striped_attribute + 1 ) - 1;
      // printf("Line %d col %d\n" , line_rank , column_rank );

      *valDouble = transformationMatrix[ column_rank * 4 + line_rank ];
    }
    return true;
  }
  return false;
}

void dm_Transformation::print( FILE *file , int isLong , int depth ) {
  if( type != EmptyTransformation ) {
    indent( file , depth );
    if( type == AMatrix ) {
      fprintf( file , "<transformation id=\"%s\" geometry=\"%s\" matrix=\"(%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f)\"" , 
	       id , TransformationTypeString[type] ,
	       transformationMatrix[0] , transformationMatrix[1] ,  
	       transformationMatrix[2] , transformationMatrix[3] ,  
	       transformationMatrix[4] , transformationMatrix[5] ,  
	       transformationMatrix[6] , transformationMatrix[7] ,  
	       transformationMatrix[8] , transformationMatrix[9] ,  
	       transformationMatrix[10] , transformationMatrix[11] ,  
	       transformationMatrix[12] , transformationMatrix[13] ,  
	       transformationMatrix[14] , transformationMatrix[15] );
    }      
    if( isLong ) {
      fprintf( file , ">\n" );
      indent( file , depth );
      fprintf( file , "</transformation>\n" );
    }
    else {
      fprintf( file , "/>\n" );
    }
  }
}

void dm_Transformation::drawInverseTransformation( DrawingMode mode ) {
  switch( type ) {
  case EmptyTransformation:
    break;
  case ATranslation:
    // nothing to do, keep coordinates as they are
    if( aVector.x || aVector.y || aVector.z ) {
      GLfloat mat1[] = { 1.0 , 0.0 , 0.0 , 0.0 , 
			 0.0 , 1.0 , 0.0 , 0.0 , 
			 0.0 , 0.0 , 1.0 , 0.0 , 
			 -aVector.x , -aVector.y , -aVector.z , 1.0 };
      glMultMatrixf(mat1); 
    }
    // glTranslatef( -aVector.x , -aVector.y , -aVector.z );
    break;
  case AQuaternion:
    // quaternion to angle/axis transformation
    if( !rotationUpdated ) {
      QuaternionToAngleAxis();      
    }
  case ARotation:
    if( aAngle ) {
      float c = cos(-aAngle);
      float C = 1.0F - c;
      float s = sin(-aAngle);
      float norm = aVector.norme();
      if( norm == 0 ) {
	break;
      }
      float x = aVector.x/norm;
      float y = aVector.y/norm;
      float z = aVector.z/norm;
      GLfloat mat2[] = { c + x*x*C , z*s + y*x*C , -y*s + z*x*C , 0.0 ,
			 -z*s + x*y*C , c + y*y*C , x*s + z*y*C , 0.0 ,
			 y*s + x*z*C , -x*s + y*z*C , c + z*z*C , 0.0 ,
			 0.0 , 0.0 , 0.0 , 1.0 };

      glMultMatrixf(mat2); 
    }
     // printf( "rotate\n" );
    // glRotatef( -aAngle , aVector.x , aVector.y , aVector.z );
    break;
  case AScale:
    if( (aVector.x != 1.0 || aVector.y != 1.0 || aVector.z != 1.0)
	&& aVector.x && aVector.y && aVector.z ) {
      GLfloat mat3[] = { 1.F/aVector.x , 0.0 , 0.0 , 0.0 , 
			 0.0 , 1.F/aVector.y , 0.0 , 0.0 , 
			 0.0 , 0.0 , 1.F/aVector.z , 0.0 , 
			 0.0 , 0.0 , 0.0 , 1.0 };
      glMultMatrixf(mat3); 
      // glScalef( 1.F/aVector.x , 1.F/aVector.y , 1.F/aVector.z );
    }
    break;
  case AMatrix:
    float matInv[ 16 ];
    if( invert_matrix( transformationMatrix , matInv ) ) 
      glMultMatrixf( matInv );
    break;
  }
}

void dm_Transformation::preDisplay( DrawingMode mode ) {
  switch( type ) {
  case EmptyTransformation:
    break;
  case ATranslation:
//          printf( "transl\n" );
    // nothing to do, keep coordinates as they are
    if( aVector.x || aVector.y || aVector.z ) {
      GLfloat mat1[] = { 1.0 , 0.0 , 0.0 , 0.0 , 
			 0.0 , 1.0 , 0.0 , 0.0 , 
			 0.0 , 0.0 , 1.0 , 0.0 , 
			 aVector.x , aVector.y , aVector.z , 1.0 };
      glMultMatrixf(mat1); 
    }
    // glTranslatef( aVector.x , aVector.y , aVector.z );
    break;
  case AQuaternion:
    // quaternion to angle/axis transformation
    if( !rotationUpdated ) {
      QuaternionToAngleAxis();   
//       printf( "Quaternion x=%.3f y=%.3f z=%.3f w=%.3f\n", aQuatIm.x, aQuatIm.y, aQuatIm.z, aQuat_w);
//       printf( "Rotation x=%.3f y=%.3f z=%.3f w=%.3f\n", aVector.x, aVector.y, aVector.z, aAngle);
    }
  case ARotation:
    if( aAngle ) {
      float c = cos(aAngle);
      float C = 1.0F - c;
      float s = sin(aAngle);
      float norm = aVector.norme();
      if( norm == 0 ) {
	break;
      }
      float x = aVector.x/norm;
      float y = aVector.y/norm;
      float z = aVector.z/norm;
      GLfloat mat2[] = { c + x*x*C , z*s + y*x*C , -y*s + z*x*C , 0.0 ,
			 -z*s + x*y*C , c + y*y*C , x*s + z*y*C , 0.0 ,
			 y*s + x*z*C , -x*s + y*z*C , c + z*z*C , 0.0 ,
			 0.0 , 0.0 , 0.0 , 1.0 };

      glMultMatrixf(mat2); 
    }
    // glRotatef( aAngle , aVector.x , aVector.y , aVector.z );
    break;
  case AScale:
    if( aVector.x != 1.0 || aVector.y != 1.0 || aVector.z != 1.0 ) {
      GLfloat mat3[] = { aVector.x , 0.0 , 0.0 , 0.0 , 
			 0.0 , aVector.y , 0.0 , 0.0 , 
			 0.0 , 0.0 , aVector.z , 0.0 , 
			 0.0 , 0.0 , 0.0 , 1.0 };
      glMultMatrixf(mat3); 
      // glScalef( aVector.x , aVector.y , aVector.z );
    }
    break;
  case AMatrix:
    glMultMatrixf( transformationMatrix );
    break;
  }
}

void dm_Transformation::display( DrawingMode mode , bool *invalidDL ) {
  switch( type ) {
  case EmptyTransformation:
  case ATranslation:
  case AQuaternion:
  case ARotation:
  case AScale:
  case AMatrix:
    break;
  }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void dm_Transformation::update( dm_SceneNode *carrierNode ) {
}

bool dm_Transformation::hasDrawablePrimitive( void ) {
  return false;
}

void dm_Transformation::AngleAxisToQuaternion( void ) {
  // axis is normalized
  //     q_w   =   cos(theta/2)
  //     q_x   =   x * sin(theta/2)
  //     q_y   =   y * sin(theta/2)
  //     q_z   =   z * sin(theta/2)
    
  float halfThetaRadian = (float)( aAngle * M_PI / 360.0 );
  float sinHalfThetaRadian = (float)sin( halfThetaRadian );
  aVector.normalize();
  aQuat_w = cos( halfThetaRadian );
  aQuatIm = aVector;
  aQuatIm *= sinHalfThetaRadian;
}

////////////////////////////////////////////////////////////
// quaternion to oepnGL rotation transformation
////////////////////////////////////////////////////////////
void dm_Transformation::QuaternionToAngleAxis( void ) {
  //            angle= 2 * acos(q_w / scale)
  //            x= q_x / scale
  //            y= q_y / scale
  //            z= q_z / scale
  // where scale = sqrt (w2 + x2 + y2 + z2)
  float length = sqrt( aQuat_w * aQuat_w + aQuatIm.x * aQuatIm.x 
		       + aQuatIm.y * aQuatIm.y + aQuatIm.z * aQuatIm.z );
  if( length > 0.0000001 ) {
    float scale = 1.0F / length;
    aQuat_w  *= scale;
    if( aQuat_w > 1.0 ) {
      aQuat_w = 1.0;
    }
    else if( aQuat_w < -1.0 ) {
      aQuat_w = -1.0;
    }
    aAngle = (float)acos( aQuat_w ) * 360.0F / (float)M_PI;
    aVector = aQuatIm;
    aVector *= scale;
  }
  else {
    // null angle axis undetermined (here Ox)
    aAngle = 0;
    aVector.x = 1.0;
    aVector.y = 0.0;
    aVector.z = 0.0;
  }
  rotationUpdated = true;
}

void dm_Transformation::normalize( void ) {
  switch( type ) {
  case ARotation :
    aVector.normalize();    
  break;
  case AQuaternion :
    {
      float length = aQuat_w * aQuat_w + aQuatIm.norme2();
      if( length > 0.0 ) {
	aQuat_w /= length;
	aQuatIm /= length;
      }    
    }
    break;
  default:
    break;
  }
}

void dm_Transformation::operator=(dm_Transformation& t) {
  this->dm_NonGeometricalObject::operator=(t);
  type = t.type;
  aAngle = t.aAngle;
  valScalarMatrix = t.valScalarMatrix;
  if( valScalarMatrix ) {
    delete [] valScalarMatrix;
  }
  if( t.valScalarMatrix ) {
    valScalarMatrix = new dm_ValScalar[ 16 ];
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      valScalarMatrix[ind] = t.valScalarMatrix[ind];
    }
  }
  for( int ind = 0 ; ind < 16 ; ind++ ) {
    transformationMatrix[ind] = t.transformationMatrix[ind];
  }
  if( type == ARotation || type == AScale || type == ATranslation ) {
    aVector = t.aVector;
  }
  if( type == AQuaternion ) {
    aQuatIm = t.aQuatIm;
    aQuat_w = t.aQuat_w;
  }

  switch( type ) {
  case ATranslation :
  case AQuaternion:
  case ARotation :
  case AScale :
  case AMatrix :
  case EmptyTransformation :
    break;
  }
  StrCpy( &id , t.id );
}

dm_Frustum::dm_Frustum() {
  xMin = -.10F;
  xMax = -xMin;
  yMin = -.10F * HEIGHT / WIDTH;
  yMax = -yMin;
  nearFrustum = 0.1F;
  farFrustum = 100000.0F;
}

dm_Frustum::~dm_Frustum( void ) {
  // TODO
}

void dm_Frustum::update( GLfloat height , GLfloat width ) {
  //yMin = -.10 * height / width;
  //yMax = -yMin;
}

void dm_Frustum::operator=(dm_Frustum& f) {
  xMin = f.xMin;
  xMax = f.xMax;
  yMin = f.yMin;
  yMax = f.yMax;
  nearFrustum = f.nearFrustum;
  farFrustum = f.farFrustum;
}

dm_Ortho::dm_Ortho() {
  xMin = -3.0;
  xMax = -xMin;
  yMin = -3.0 * HEIGHT / WIDTH;
  yMax = -yMin;
  nearOrtho = -100000.0;
  farOrtho  =  100000.0;
}

dm_Ortho::~dm_Ortho( void ) {
  // TODO
}

void dm_Ortho::update( GLfloat height , GLfloat width ) {
//   yMin = -3.00 * height / width;
//   yMax = -yMin;
}

void dm_Ortho::operator=(dm_Ortho& o) {
  xMin = o.xMin;
  xMax = o.xMax;
  yMin = o.yMin;
  yMax = o.yMax;
  nearOrtho = o.nearOrtho;
  farOrtho = o.farOrtho;
}

dm_User::dm_User (): dm_NonGeometricalObject() {
  viewvolume_type = DM_FRUSTUM;

  // ------ stereo user with two eyes ----- //
  alternate_stereo_viewer = false;

  // --------- view volume ---------- //
  ortho = new dm_Ortho();
  frustum = new dm_Frustum();

  // -------- color ----------------- //
  bg_color[0] = 0;
  bg_color[1] = 0;
  bg_color[2] = 0;
  bg_color[3] = 1;

  // --------- viewpoints ----------- //
  default_viewpoint = -1;
  default_viewpoint_stereo_left = -1;
  default_viewpoint_node = NULL;
  default_viewpoint_stereo_left_node = NULL;

  nbViewpointShots = 0;
  indCurrentViewpoint = 0;
  shotIni = NULL;
  shotEnd = NULL;
  shotIndViewpoint = NULL;
  shotIndViewpointStereoLeft = NULL;
  manualViewpointControl = false;

  // --------- navigation ----------- //
  start_y = 0.0;
  start_x = 0.0;
  start_z = 0.0;
  start_yaw = 0.0;
  start_pitch = 0.0;
  // ------ whether or not mouse motion modifies view direction ----- //
  rotation_amplitude_yaw = 1.0;
  rotation_amplitude_pitch = 1.0;

  // manual moving steps
//   manualMoveLinearStep = 0.0;
//   manualMoveHeightStep = 0.0;
}

dm_User::~dm_User( void ) {
  if (ortho) {
    delete ortho;
    ortho = NULL;
  }
 
  if (frustum) {
    delete frustum;
    frustum = NULL;
  }

  // these pointers refer to other nodes in the scene
  // should not be deallocated otherwise the corresponding
  // nodes will be deallocated twice

//   if (default_viewpoint_node) {
//     delete default_viewpoint_node;
//     default_viewpoint_node = NULL;
//   }

//   if (default_viewpoint_stereo_left_node) {
//     delete default_viewpoint_stereo_left_node;
//     default_viewpoint_stereo_left_node = NULL;
//   }

  if (shotIni) {
    delete [] shotIni;
    shotIni = NULL;
  }

  if (shotEnd) {
    delete [] shotEnd;
    shotEnd = NULL;
  }
	
  if (shotIndViewpoint) {
    delete [] shotIndViewpoint;
    shotIndViewpoint = NULL;
  }

  if (shotIndViewpointStereoLeft) {
    delete [] shotIndViewpointStereoLeft;
    shotIndViewpointStereoLeft = NULL;
  }	
}

void dm_User::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<user id=\"%s\" alternate_stereo_viewer=\"%s\" r=\"%.5f\" g=\"%.5f\" b=\"%.5f\" a=\"%.5f\" >\n" , id , BooleanString[BooleanToInt(alternate_stereo_viewer)] , bg_color[0] ,  bg_color[1] , bg_color[2] , bg_color[3] );
  indent( file , depth + 1 );
  fprintf( file , "<viewpoints default_viewpoint=\"%d\" default_viewpoint_stereo_left=\"%d\" shots=\"%d\">\n" , default_viewpoint , default_viewpoint_stereo_left , nbViewpointShots );
  for( int ind = 0 ; ind < nbViewpointShots ; ind++ ) {
    indent( file , depth + 1 );
    fprintf( file , "%d " , shotIndViewpoint[ind] );
    if( alternate_stereo_viewer ) {
      fprintf( file , "%d " , shotIndViewpointStereoLeft[ind] );
    }
    fprintf( file , "%.5f %.5f\n" , shotIni[ind] , shotEnd[ind] );
  }
  indent( file , depth + 1 );
  fprintf( file , "</viewpoints>\n" );
  indent( file , depth + 1 );
  fprintf( file , "<navigation start_y=\"%.5f\" start_x=\"%.5f\" start_z=\"%.5f\" start_yaw=\"%.5f\" start_pitch=\"%.5f\" rotation_amplitude_yaw=\"%.5f\" rotation_amplitude_pitch=\"%.5f\"/>\n" ,  start_y , start_x , start_z , start_yaw , start_pitch , rotation_amplitude_yaw , rotation_amplitude_pitch );
  if( viewvolume_type == DM_ORTHO ) {
    indent( file , depth + 1 );
    fprintf( file , "<view_volume type=\"ortho\" x_min=\"%.5f\" y_min=\"%.5f\" x_max=\"%.5f\" y_max=\"%.5f\" near=\"%.5f\" far=\"%.5f\"/>\n" , ortho->xMin , ortho->yMin , ortho->xMax , ortho->yMax , ortho->nearOrtho , ortho->farOrtho );
  }
  if( viewvolume_type == DM_FRUSTUM ) {
    indent( file , depth + 1 );
    fprintf( file , "<view_volume type=\"frustum\" x_min=\"%.5f\" y_min=\"%.5f\" x_max=\"%.5f\" y_max=\"%.5f\" near=\"%.5f\" far=\"%.5f\"/>\n" , frustum->xMin , frustum->yMin , frustum->xMax , frustum->yMax , frustum->nearFrustum , frustum->farFrustum );
  }

  indent( file , depth );
  fprintf( file , "</user>\n" );
}

void dm_User::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
 	            FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    alternate_stereo_viewer = false;
  }
  if( XMLtag->hasAttribute( "alternate_stereo_viewer"  ) ) {
    XMLtag->BooleanFieldValue( "alternate_stereo_viewer" ,
			       &(alternate_stereo_viewer) );
  }
  if( !atRunTime ) {
    bg_color[0] = 0;
    bg_color[1] = 0;
    bg_color[2] = 0;
    bg_color[3] = 1;
  }
  if( XMLtag->hasAttribute( "r"  ) ) {
    oper( bg_color[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
								atRunTime ));
  }
  if( XMLtag->hasAttribute( "g"  ) ) {
    oper( bg_color[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
								atRunTime ));
  }
  if( XMLtag->hasAttribute( "b"  ) ) {
    oper( bg_color[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
								atRunTime ));
  }
  if( XMLtag->hasAttribute( "a"  ) ) {
    oper( bg_color[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
								atRunTime ));
  }
}
  
void dm_User::setApplicationParameters( dm_XMLTag *XMLtag , bool atRunTime ,
 	            FloatBinOp oper ,
		    IntBinOp operI ) {
  
  /////////////////////////////////////////////////////////////////
  // view_volume component
  if( strcmp( XMLtag->tag , "view_volume" ) == 0 ) {
    if( XMLtag->hasAttribute( "type" ) ) {
      char *type_str = XMLtag->getAttributeStringValue( "type" );
      viewvolume_type = EmptyViewvolumeType;
      for( int ind = 0 ; ind < EmptyViewvolumeType ; ind++ ) {
	if( strcmp( type_str , ViewvolumeTypeString[ind] ) == 0 ) {
	  viewvolume_type = (ViewvolumeType)ind;
	  break;
	}
      }
      if( viewvolume_type == EmptyViewvolumeType ) {
	sprintf( ErrorStr , "Error: unknown view_volume type [%s] !" , type_str ); ReportError( ErrorStr ); 
	if( !atRunTime ) {
	  throw 203;
	}
	else {
	  viewvolume_type = DM_FRUSTUM;
	}
      }
    }

    if( viewvolume_type == DM_FRUSTUM ) {
      if( XMLtag->hasAttribute( "x_min" ) ) {
	oper( frustum->xMin , (float)XMLtag->getAttributeDoubleValue( "x_min" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "x_max" ) ) {
	oper( frustum->xMax , (float)XMLtag->getAttributeDoubleValue( "x_max" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "y_min" ) ) {
	oper( frustum->yMin , (float)XMLtag->getAttributeDoubleValue( "y_min" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "y_max" ) ) {
	oper( frustum->yMax , (float)XMLtag->getAttributeDoubleValue( "y_max" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "near" ) ) {
	oper( frustum->nearFrustum , (float)XMLtag->getAttributeDoubleValue( "near" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "far" ) ) {
	oper( frustum->farFrustum , (float)XMLtag->getAttributeDoubleValue( "far" , 
							     atRunTime ));
      }
    }
    else if( viewvolume_type == DM_ORTHO ) {
      if( XMLtag->hasAttribute( "x_min" ) ) {
	oper( ortho->xMin , (float)XMLtag->getAttributeDoubleValue( "x_min" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "x_max" ) ) {
	oper( ortho->xMax , (float)XMLtag->getAttributeDoubleValue( "x_max" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "y_min" ) ) {
	oper( ortho->yMin , (float)XMLtag->getAttributeDoubleValue( "y_min" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "y_max" ) ) {
	oper( ortho->yMax , (float)XMLtag->getAttributeDoubleValue( "y_max" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "near" ) ) {
	oper( ortho->nearOrtho , (float)XMLtag->getAttributeDoubleValue( "near" , 
							      atRunTime ));
      }
      if( XMLtag->hasAttribute( "far" ) ) {
	oper( ortho->farOrtho , (float)XMLtag->getAttributeDoubleValue( "far" , 
							     atRunTime ));
      }
    }
  }

  /////////////////////////////////////////////////////////////////
  // navigaiton component
  else if( strcmp( XMLtag->tag , "navigation" ) == 0 ) {
    if( XMLtag->hasAttribute( "start_y" ) ) {
      start_y 
	= (float)XMLtag->getAttributeDoubleValue( "start_y" , 
				   atRunTime );
    }
    if( XMLtag->hasAttribute( "start_rotate" ) ) {
      fprintf( stderr , "Deprecated: \"start_rotate\" attribute is deprecated use \"start_yaw\"!\n" );
      start_yaw 
	= (float)XMLtag->getAttributeDoubleValue( "start_rotate" , 
				   atRunTime );
    }
    if( XMLtag->hasAttribute( "start_yaw" ) ) {
      start_yaw 
	= (float)XMLtag->getAttributeDoubleValue( "start_yaw" , 
				   atRunTime );
    }

    if( XMLtag->hasAttribute( "start_spin" ) ) {
      fprintf( stderr , "Deprecated: \"start_spin\" attribute is deprecated use \"start_pitch\"!\n" );
      start_pitch
	= (float)XMLtag->getAttributeDoubleValue( "start_spin" , 
				   atRunTime );
    }
    if( XMLtag->hasAttribute( "start_pitch" ) ) {
      start_pitch
	= (float)XMLtag->getAttributeDoubleValue( "start_pitch" , 
				   atRunTime );
    }
    if( XMLtag->hasAttribute( "start_x" ) ) {
      start_x 
	= (float)XMLtag->getAttributeDoubleValue( "start_x" , 
				   atRunTime );
    }

    if( XMLtag->hasAttribute( "start_z" ) ) {
      start_z 
	= (float)XMLtag->getAttributeDoubleValue( "start_z" , 
				   atRunTime );
    }

    if( !atRunTime ) {
      rotation_amplitude_yaw = 1.0;
    }
    if( XMLtag->hasAttribute( "rotation_amplitude_yaw"  ) ) {
      rotation_amplitude_yaw 
	= (float)XMLtag->getAttributeDoubleValue( "rotation_amplitude_yaw" , 
				   atRunTime );
    }

    if( !atRunTime ) {
      rotation_amplitude_pitch = 1.0;
    }
    if( XMLtag->hasAttribute( "rotation_amplitude_pitch"  ) ) {
      rotation_amplitude_pitch 
	= (float)XMLtag->getAttributeDoubleValue( "rotation_amplitude_pitch" , 
				   atRunTime );
    }

    if( XMLtag->hasAttribute( "rotation_amplitude"  ) ) {
      fprintf( stderr , "Deprecated: \"rotation_amplitude\" attribute is deprecated use \"rotation_amplitude_yaw\" and \"rotation_amplitude_pitch\"!\n" );
      rotation_amplitude_yaw 
	= (float)XMLtag->getAttributeDoubleValue( "rotation_amplitude" , 
				   atRunTime );
      rotation_amplitude_pitch = rotation_amplitude_yaw;
    }

  }

  /////////////////////////////////////////////////////////////////
  // viewpoints component
  else if( strcmp( XMLtag->tag , "viewpoints" ) == 0 ) {
    if( XMLtag->hasAttribute( "default_viewpoint" ) ) {
      default_viewpoint = XMLtag->getAttributeIntValue( "default_viewpoint" , 
							atRunTime );
    }
    else if( XMLtag->hasAttribute( "xlink:href" ) ) {
      default_viewpoint_node = new dm_SceneNode(ANodeLink);
      StrCpy( &(default_viewpoint_node->id) , 
	      XMLtag->getAttributeStringValue( "xlink:href" ) );
      // printf( "ViewPoint Name (%s)\n" , default_viewpoint_node->id );
    }


    if( XMLtag->hasAttribute( "default_viewpoint_stereo_left" ) ) {
      default_viewpoint_stereo_left = XMLtag->getAttributeIntValue( "default_viewpoint_stereo_left" , 
						atRunTime );
    }
    else if( XMLtag->hasAttribute( "xlink:href_stereo_left" ) ) {
      default_viewpoint_stereo_left_node = new dm_SceneNode(ANodeLink);
      StrCpy( &(default_viewpoint_stereo_left_node->id) , 
	      XMLtag->getAttributeStringValue( "xlink:href_stereo_left" ) );
    }

    if( XMLtag->hasAttribute( "default_camera" ) ) {
      fprintf( stderr , "Deprecated: \"default_camera\" attribute is deprecated use \"default_viewpoint\"!\n" );
      default_viewpoint = XMLtag->getAttributeIntValue( "default_camera" , 
						atRunTime );
    }

    if( XMLtag->hasAttribute( "shots" ) ) {
      nbViewpointShots
	= XMLtag->getAttributeIntValue( "shots" , 
				atRunTime );
    }
    else 
      nbViewpointShots = 0;
  }
  ///////////////////////////////
  // set user node property
  else if( strcmp( XMLtag->tag , "set_user_attribute_value" ) == 0 ) {
	  if (XMLtag->hasAttribute("r")){
		  bg_color[0] = (float)XMLtag->getAttributeDoubleValue("r", atRunTime);
	  }
	  if (XMLtag->hasAttribute("g")) {
		  bg_color[1] = (float)XMLtag->getAttributeDoubleValue("g", atRunTime);
	  }
	  
	  if (XMLtag->hasAttribute("b")) {
		  bg_color[2] = (float)XMLtag->getAttributeDoubleValue("b", atRunTime);
	  }
	  
	  if (XMLtag->hasAttribute("alternate_stereo_viewer")) {
		  XMLtag->BooleanFieldValue("alternate_stereo_viewer", &alternate_stereo_viewer);
	  }
  }

  ///////////// Unknown tag
  else {
    sprintf( ErrorStr , "Error: unknown user component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 73;
    return;
  }
}

bool dm_User::getParameters( char *attribute , 
			   DataType * dataType ,
			   double * valDouble ,
			   int * valInt ,
			   bool * valBool ,
			   char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "r" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)bg_color[0];
    return true;
  }
  else if( strcmp( attribute , "g" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)bg_color[1];
    return true;
  }
  else if( strcmp( attribute , "b" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)bg_color[2];
    return true;
  }
  else if( strcmp( attribute , "a" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)bg_color[3];
    return true;
  }
  else if( strcmp( attribute , "x_min" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->xMin;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->xMin;
      return true;
    }
    return true;
  }
  else if( strcmp( attribute , "x_max" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->xMax;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->xMax;
      return true;
    }
  }
  else if( strcmp( attribute , "y_min" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->yMin;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->yMin;
      return true;
    }
  }
  else if( strcmp( attribute , "y_max" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->yMax;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->yMax;
      return true;
    }
  }
  else if( strcmp( attribute , "near" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->nearOrtho;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->nearFrustum;
      return true;
    }
  }
  else if( strcmp( attribute , "far" ) == 0 ) {
    *dataType = DataTypeDouble;
    if( viewvolume_type == DM_ORTHO ) {
      *valDouble = (double)ortho->farOrtho;
      return true;
    }
    if( viewvolume_type == DM_FRUSTUM ) {
      *valDouble = (double)frustum->farFrustum;
      return true;
    }
  }
  return false;
}

void dm_User::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag   *XMLtag = new dm_XMLTag();

  // printf( "Parse user \n" );
  while( *p_c != EOF ) {
    // new tag XML : user component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // user closing tag
    if( strcmp( XMLtag->tag , "user" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      // printf( "End of user parsing \n" );
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    setApplicationParameters( XMLtag , false ,
			      assign , assignI );

    ////////////////////////////
    ////// viewpoints: parsing the scenario, a set of shots
    if( strcmp( XMLtag->tag , "viewpoints" ) == 0 && XMLtag->tagType == OpeningTag ) {
      shotIndViewpoint = new int[nbViewpointShots];
      if( alternate_stereo_viewer ) {
	shotIndViewpointStereoLeft = new int[nbViewpointShots];
      }
      shotIni = new float[nbViewpointShots];
      shotEnd = new float[nbViewpointShots];
      
      // parsing the shot intervals and associated viewpoints
      for( int ind = 0; ind < nbViewpointShots ; ind++ ) {
	shotIndViewpoint[ind] = (int)ReadExpression( fileDepth , p_c , 
						     true , NULL , NULL , 
						     false , NULL );
	if( alternate_stereo_viewer ) {
	  shotIndViewpointStereoLeft[ind] 
	    = (int)ReadExpression( fileDepth , p_c , 
				   true , NULL , NULL , 
				   false , NULL );
	}
	shotIni[ind] = (float)ReadExpression( fileDepth , p_c , 
					      true , NULL , NULL , false , NULL );
	shotEnd[ind] = (float)ReadExpression( fileDepth , p_c , 
					      true , NULL , NULL , false , NULL );
      }

      // expecting viewpoints closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  XMLtag->tag , NULL , NULL );
    }

    ////////////////////////////
    ////// unknown tag
    else if( XMLtag->tagType == OpeningTag ) {
      sprintf( ErrorStr , "Error: unknown environment component tag [%s] in environment ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); delete XMLtag; throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

// display: nothing to display
void dm_User::display( DrawingMode mode , bool *invalidDL ) {
}

void dm_User::operator=(dm_User& h) {
  this->dm_NonGeometricalObject::operator=(h);

  alternate_stereo_viewer = h.alternate_stereo_viewer;

  for( int indColor = 0 ; indColor < 4 ; indColor++ ) {
    bg_color[indColor] = h.bg_color[indColor];    
  }

  viewvolume_type = h.viewvolume_type;

  // --------- view volume ----------- //
  if( !ortho ) {
    ortho = new dm_Ortho();
  }
  *ortho = *(h.ortho);
  if( !frustum ) {
    frustum = new dm_Frustum();
  }
  *frustum = *(h.frustum);

  // --------- navigation ----------- //
  start_y = h.start_y;
  start_x = h.start_x;
  start_z = h.start_z;
  start_yaw = h.start_yaw;
  start_pitch = h.start_pitch;
//   manualMoveLinearStep = h.manualMoveLinearStep;
//   manualMoveHeightStep = h.manualMoveHeightStep;

  // --------- viewpoints ----------- //
  default_viewpoint = h.default_viewpoint;
  default_viewpoint_stereo_left = h.default_viewpoint_stereo_left;
  default_viewpoint_node = h.default_viewpoint_node;
  default_viewpoint_stereo_left_node = h.default_viewpoint_stereo_left_node;
  indCurrentViewpoint = h.indCurrentViewpoint;
  manualViewpointControl = h.manualViewpointControl;

  nbViewpointShots = h.nbViewpointShots;

  if (shotIndViewpoint) {
    delete [] shotIndViewpoint;
    shotIndViewpoint = NULL;
  }
	
  if (shotIndViewpointStereoLeft) {
    delete [] shotIndViewpointStereoLeft;
    shotIndViewpointStereoLeft = NULL;
  }

  if (shotIni) {
    delete [] shotIni;
    shotIni = NULL;
  }

  if (shotEnd) {
    delete [] shotEnd;
    shotEnd = NULL;
  }

  if( !shotIndViewpoint ) {
    shotIndViewpoint = new int[nbViewpointShots];
  }
  if( alternate_stereo_viewer ) {
    if( !shotIndViewpointStereoLeft ) {
      shotIndViewpointStereoLeft = new int[nbViewpointShots];
    }
  }
  if( !shotIni ) {
    shotIni = new float[nbViewpointShots];
  }
  if( !shotEnd ) {
    shotEnd = new float[nbViewpointShots];
  }
      
  // parsing the shot intervals and associated viewpoints
  for( int ind = 0; ind < nbViewpointShots ; ind++ ) {
    shotIndViewpoint[ind] = h.shotIndViewpoint[ind];
    if( alternate_stereo_viewer ) {
      shotIndViewpointStereoLeft[ind] = h.shotIndViewpointStereoLeft[ind];
    }
    shotIni[ind] = h.shotIni[ind];
    shotEnd[ind] = h.shotEnd[ind];
  }
}

