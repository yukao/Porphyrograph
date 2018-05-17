/*! \file dm-material.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-material.cpp
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

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg" , "png" , "pnga" , "png_gray" , "pnga_gray" , "rgb", "raw" , "emptyimagetype" };

const char *TexturingString[EmptyTexturing + 1] = { "direct_texturing" , "shadersource_texturing" , "emptytexturing" };

const char *TextureGenerationModeString[EmptyTextureGenerationMode + 1] = { "bitmap_texture" , "frame_buffer_cleared" , "frame_buffer_uncleared" , "variable_texture" , "emptytexturegenerationmode" };

const char *TextureFormatString[EmptyTextureFormat + 1] = { "byte_tex_format" , "float_tex_format" , "depth_tex_format" , "emptytextureformat" };

const char *TextureFilterString[EmptyTextureFilter + 1] = { "linear_filter" , "nearest_filter" , "emptytexturefilter" };

/*! \name default_material
 *  \brief default material: white,
 */
// \{
GLfloat white_transparent[4] = {1.0,1.0,1.0,0.0};
GLfloat gray_transparent[4] = {0.5,0.5,0.5,0.0};
GLfloat black_transparent[4] = {0.0,0.0,0.0,0.0};
GLfloat white_opaque[4] = {1.0,1.0,1.0,1.0};
GLfloat gray_opaque[4] = {0.5,0.5,0.5,1.0};
GLfloat black_opaque[4] = {0.0,0.0,0.0,1.0};
// \}

dm_ColorStripe::dm_ColorStripe( void ) {
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    color[ind] = 0.0;
  }
  length = 0.0;
}

dm_ColorStripe::~dm_ColorStripe(void) {
}

void dm_ColorStripe::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				    FloatBinOp oper ,
				    IntBinOp operI ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( color[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( color[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( color[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( color[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "length" ) ) {
      oper( length , (float)XMLtag->getAttributeDoubleValue( "length" , 
							  atRunTime ));
    }
}

void dm_ColorStripe::print( FILE *file , int depth ) {
  indent( file , depth );
  fprintf( file , "<color_stripe r=\"%.2f\" g=\"%.2f\" g=\"%.2f\" b=\"%.2f\" length=\"%.2f\" />\n" , color[0] , color[1] , color[2] , color[3] , length );
}

void dm_ColorStripe::operator=(dm_ColorStripe& p) {
  for( int ind = 0 ; ind < 4 ; ind++ ) {
    color[ind] = p.color[ind];
  }
  length = p.length;
}

//////////////////////////////////////////////////////////////////

dm_MaterialProperties::dm_MaterialProperties( void ) {
  int ind;

  hasTexture = false;
  materialTexture = NULL;

  hasVideo = false;
#ifdef DM_HAVE_FFMPEG
  materialVideo = NULL;
#endif

  hasColor = false;
  hasMaterial = false;
  hasShader = false;

  for( ind = 0 ; ind < 3 ; ind++ ) {
    color[ind] = 0.0;
    Ambient[ind] = 0.0; 
    Diffuse[ind] = 0.0; 
    Specular[ind] = 0.0; 
    Emission[ind] = 0.0; 
  }
  color[3] = 1.0;
  color_encoding = EmptyTextureEncoding;
  Ambient[3] = 1.0; Diffuse[3] = 1.0; Specular[3] = 1.0; Emission[3] = 1.0;
  Shininess = 0.0;

  shader = NULL;
}

dm_MaterialProperties::~dm_MaterialProperties(void) {
//	if (materialTexture)
//	{
//		delete materialTexture;
//		materialTexture = NULL;
//	}

#ifdef DM_HAVE_FFMPEG
	if (materialVideo)
	{
		delete materialVideo;
		materialVideo = NULL;
	}
#endif
}

void dm_MaterialProperties::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
					   FloatBinOp oper ,
					   IntBinOp operI ) {
  
  /////////////////////////////////////////////////////////////////
  // ambient component
  // given by attributes
  if( strcmp( XMLtag->tag , "ambient" ) == 0 ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( Ambient[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( Ambient[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( Ambient[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( Ambient[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
  }
  /////////////////////////////////////////////////////////////////
  // diffuse component
  // given by attributes
  else if( strcmp( XMLtag->tag , "diffuse" ) == 0 ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( Diffuse[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( Diffuse[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( Diffuse[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( Diffuse[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
  }
  /////////////////////////////////////////////////////////////////
  // specular component
  // given by attributes
  else if( strcmp( XMLtag->tag , "specular" ) == 0 ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( Specular[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( Specular[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( Specular[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( Specular[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
  }
  /////////////////////////////////////////////////////////////////
  // emission component
  // given by attributes
  else if( strcmp( XMLtag->tag , "emission" ) == 0 ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( Emission[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( Emission[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( Emission[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( Emission[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
  }
  /////////////////////////////////////////////////////////////////
  // shininess component
  // given by attributes
  else if( strcmp( XMLtag->tag , "shininess" ) == 0 ) {
    if( XMLtag->hasAttribute( "s" ) ) {
      oper( Shininess , (float)XMLtag->getAttributeDoubleValue( "s" , 
							  atRunTime ));
    }
  }
  /////////////////////////////////////////////////////////////////
  // color component
  // given by attributes
  else if( strcmp( XMLtag->tag , "color" ) == 0 ) {
    if( XMLtag->hasAttribute( "r" ) ) {
      oper( color[0] , (float)XMLtag->getAttributeDoubleValue( "r" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "g" ) ) {
      oper( color[1] , (float)XMLtag->getAttributeDoubleValue( "g" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "b" ) ) {
      oper( color[2] , (float)XMLtag->getAttributeDoubleValue( "b" , 
							  atRunTime ));
    }
    if( XMLtag->hasAttribute( "a" ) ) {
      oper( color[3] , (float)XMLtag->getAttributeDoubleValue( "a" , 
							  atRunTime ));
    }
  }
  else {
    sprintf( ErrorStr , "Error: unknown material component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 73;
    return;
  }
}


bool dm_MaterialProperties::getParameters( char * attribute , 
					   DataType * dataType ,
					   double * valDouble ,
					   int * valInt ,
					   bool * valBool ,
					   char ** valString ,
					   int * tableOrMatrixSize ,
					   dm_ValScalar ** valScalars ) {
  char tagAux[XMLAttrChLength];
  char * attributeAux;

  strcpy( tagAux , attribute );
  if( ( attributeAux = strchr( tagAux , ':' ) ) ) {
    *attributeAux = 0;
    attributeAux++;
  }
  else {
    return false;
  }

  if( strcmp( tagAux , "ambient" ) == 0 ) {
    if( strcmp( attributeAux , "r" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Ambient[0];
      return true;
    }
    else if( strcmp( attributeAux , "g" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Ambient[1];
      return true;
    }
    else if( strcmp( attributeAux , "b" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Ambient[2];
      return true;
    }
    else if( strcmp( attributeAux , "a" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Ambient[3];
      return true;
    }
  }
  else if( strcmp( tagAux , "diffuse" ) == 0 ) {
    if( strcmp( attributeAux , "r" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Diffuse[0];
      return true;
    }
    else if( strcmp( attributeAux , "g" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Diffuse[1];
      return true;
    }
    else if( strcmp( attributeAux , "b" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Diffuse[2];
      return true;
    }
    else if( strcmp( attributeAux , "a" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Diffuse[3];
      return true;
    }
  }
  else if( strcmp( tagAux , "specular" ) == 0 ) {
    if( strcmp( attributeAux , "r" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Specular[0];
      return true;
    }
    else if( strcmp( attributeAux , "g" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Specular[1];
      return true;
    }
    else if( strcmp( attributeAux , "b" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Specular[2];
      return true;
    }
    else if( strcmp( attributeAux , "a" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Specular[3];
      return true;
    }
  }
  else if( strcmp( tagAux , "emission" ) == 0 ) {
    if( strcmp( attributeAux , "r" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Emission[0];
      return true;
    }
    else if( strcmp( attributeAux , "g" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Emission[1];
      return true;
    }
    else if( strcmp( attributeAux , "b" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Emission[2];
      return true;
    }
    else if( strcmp( attributeAux , "a" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Emission[3];
      return true;
    }
  }
  else if( strcmp( tagAux , "color" ) == 0 ) {
    if( strcmp( attributeAux , "r" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)color[0];
      return true;
    }
    else if( strcmp( attributeAux , "g" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)color[1];
      return true;
    }
    else if( strcmp( attributeAux , "b" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)color[2];
      return true;
    }
    else if( strcmp( attributeAux , "a" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)color[3];
      return true;
    }
  }
  else if( strcmp( tagAux , "shininess" ) == 0 ) {
    if( strcmp( attributeAux , "s" ) == 0 ) {
      *dataType = DataTypeDouble;
      *valDouble = (double)Shininess;
      return true;
    }
  }
  return false;
}

void dm_MaterialProperties::parse( int *fileDepth , int *p_c ,
				   char *ID , dm_XMLTag *XMLtag ,
				   dm_Subdivision * subdivision ) {
  ///////////////////////////////////////////////
  // color
  if( ( strcmp( XMLtag->tag , "color" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
    hasColor = true;
    color_encoding = RGB;
//     ParseColor( fileDepth , p_c , color , XMLtag );
    setParameters( XMLtag , 
		   false ,            // compile time
		   assign , assignI );
    
    if( XMLtag->tagType == OpeningTag ) {
      color[0] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      color[1] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      color[2] 
	= ReadExpression( fileDepth , p_c , true , NULL , NULL , false , NULL );
      // expecting color closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  "color" , NULL , NULL );
    }

//     printf(  "<color r=%.5f g=%.5f b=%.5f a=%.5f/>\n" , 
// 	     color[0], color[1], color[2], color[3] );
  }

  ///////////////////////////////////////////////
  // material properties
  else if( strcmp( XMLtag->tag , "material" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
    hasMaterial = true;
    //printf( "material comp [%d]\n" , type );
	
    while( true ) {
      if( *p_c == EOF ) {
	sprintf( ErrorStr , "Error: unbalanced node tag in node [%s]!" , ID ); ReportError( ErrorStr ); throw 68;
	return;
      }

      // new tag XML : transformation component or closing tag
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
      
      /////////////////////////////////////////////////////////////////
      // node closing tag
      if( strcmp( XMLtag->tag , "material" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
	return;
      }
      /////////////////////////////////////////////////////////////////
      // material preperties given in a compact form (the only valid form 
      // for commands)
      else if( XMLtag->tagType == EmptyTag ) {
	// tag: ambient, diffuse, specular, shininess, color
	setParameters( XMLtag , 
		       true ,            // compile time
		       assign , assignI );
      }
      /////////////////////////////////////////////////////////////////
      // DEPRECATED: material preperties given in an extended form
      else if( XMLtag->tagType == OpeningTag ) {
	// ambient component
	// given by values
	if( strcmp( XMLtag->tag , "ambient" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
	  int nbVal 
	    = XMLtag->getAttributeIntValue( "size" , false );
	  if( nbVal > 4 ) {
	    sprintf( ErrorStr , "Error: material properties have more than 4 components in node ID [%s]!" , ID ); ReportError( ErrorStr ); throw 74;
	    return;
	  }

	  //printf( "ambient comp [%d]\n" , nbVal );
	  numericalMode mode = XMLtag->getModeValue();
	  for(int ind = 0 ; ind < nbVal ; ind++ ) {
	    if( mode == DM_DECIMAL ) {
	      Ambient[ind] = ReadExpression( fileDepth , p_c , true , 
					     NULL , NULL , false , NULL );
	    }
	    else if( mode == DM_RANDOM ) {
	      float inf, sup;
	      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
	      Ambient[ind] = generateRandomFloat( inf , sup );
	    }
	  }
	  // expecting ambient closing tag
	  ExpectClosingTagXML( fileDepth , p_c ,  "ambient" , NULL , NULL );
	}
	// diffuse component
	// given by values
	else if( strcmp( XMLtag->tag , "diffuse" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
	  int nbVal 
	    = XMLtag->getAttributeIntValue( "size" , false );
	  if( nbVal > 4 ) {
	    sprintf( ErrorStr , "Error: material properties have more than 4 components!" ); ReportError( ErrorStr ); throw 74;
	    return;
	  }
	  numericalMode mode = XMLtag->getModeValue();
	  for(int ind = 0 ; ind < nbVal ; ind++ ) {
	    if( mode == DM_DECIMAL ) {
	      Diffuse[ind] = ReadExpression( fileDepth , p_c , true , 
					     NULL , NULL , false , NULL );
	    }
	    else if( mode == DM_RANDOM ) {
	      float inf, sup;
	      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
	      Diffuse[ind] = generateRandomFloat( inf , sup );
	    }
	  }
	  // expecting diffuse closing tag
	  ExpectClosingTagXML( fileDepth , p_c ,  "diffuse" , NULL , NULL );
	}
	// specular component
	// given by values
	else if( strcmp( XMLtag->tag , "specular" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
	  int nbVal 
	    = XMLtag->getAttributeIntValue( "size" , false );
	  if( nbVal > 4 ) {
	    sprintf( ErrorStr , "Error: material properties have more than 4 components in node ID [%s]!" , ID ); ReportError( ErrorStr ); throw 74;
	    return;
	  }
	  numericalMode mode = XMLtag->getModeValue();
	  for(int ind = 0 ; ind < nbVal ; ind++ ) {
	    if( mode == DM_DECIMAL ) {
	      Specular[ind] = ReadExpression( fileDepth , p_c , true , 
					      NULL , NULL , false , NULL );
	    }
	    else if( mode == DM_RANDOM ) {
	      float inf, sup;
	      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
	      Specular[ind] = generateRandomFloat( inf , sup );
	    }
	  }
	  // expecting specular closing tag
	  ExpectClosingTagXML( fileDepth , p_c ,  "specular" , NULL , NULL );
	}
	// emission component
	// given by values
	else if( strcmp( XMLtag->tag , "emission" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
	  int nbVal 
	    = XMLtag->getAttributeIntValue( "size" , false );
	  if( nbVal > 4 ) {
	    sprintf( ErrorStr , "Error: material properties have more than 4 components in node ID [%s]!" , ID ); ReportError( ErrorStr ); throw 74;
	    return;
	  }
	  numericalMode mode = XMLtag->getModeValue();
	  for(int ind = 0 ; ind < nbVal ; ind++ ) {
	    if( mode == DM_DECIMAL ) {
	      Emission[ind] = ReadExpression( fileDepth , p_c , true , 
					      NULL , NULL , false , NULL );
	    }
	    else if( mode == DM_RANDOM ) {
	      float inf, sup;
	      ReadInterval( fileDepth , p_c , true , &inf , &sup , false );
	      Emission[ind] = generateRandomFloat( inf , sup );
	    }
	  }
	  // expecting emission closing tag
	  ExpectClosingTagXML( fileDepth , p_c ,  "emission" , NULL , NULL );
	}
	// shininess component
	// given by values
	else if( strcmp( XMLtag->tag , "shininess" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
	  Shininess = ReadExpression( fileDepth , p_c , true , 
				      NULL , NULL , false , NULL );
	  // expecting shininess closing tag
	  ExpectClosingTagXML( fileDepth , p_c ,  "shininess" , NULL , NULL );
	}
	else {
	  sprintf( ErrorStr , "Error: unknown material component tag [%s] in component ID [%s]!" , XMLtag->tag , ID ); ReportError( ErrorStr ); throw 73;
	  return;
	}
      }
    }
  }

  /////////////////////////////////////////////////////////////////
  // texture material properties
  else if( strcmp( XMLtag->tag , "texture" ) == 0
	   && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag))  ) {
    hasTexture = true;
    materialTexture = new dm_Texture();
  
    materialTexture->setParameters( XMLtag , 
				    false ,            // compile time
				    false ,            // not video texture
				    assign , assignI );

    // texture bound to a variable
    // references will be solved in a second pass
    if( materialTexture->textureGenerationMode == DM_VARIABLE_TEXTURE ) {
      // raw encoding
      materialTexture->encoding = RAW;

      // texture name  
      if( XMLtag->hasAttribute( "xlink:href" ) ) {
	
	char *fileName = XMLtag->getAttributeStringValue( "xlink:href" );
	// strcpy( textureName , fileName );
	StrCpyAndSubst( materialTexture->variableNodeName , fileName , XMLAttrValLength );
      }
      else {
	sprintf( ErrorStr , "Error: missing xlink:href tag on variable texture [%s] in component ID [%s]!" , XMLtag->tag , ID ); ReportError( ErrorStr ); throw 206;
      }
    
      // texture variable name  
      if( *(materialTexture->variableNodeName) && XMLtag->hasAttribute( "value" ) ) {
	char *fileName = XMLtag->getAttributeStringValue( "value" );
	// strcpy( textureName , fileName );
	StrCpyAndSubst( materialTexture->variableName , fileName , XMLAttrValLength );
      }
      else {
	sprintf( ErrorStr , "Error: unknown variable texture [%s] on node [%s] in component ID [%s]!" , materialTexture->variableName , XMLtag->tag , ID ); ReportError( ErrorStr ); throw 206;
      }
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
      materialTexture->LoadOrGenerateTexture( textureName ,
					      fileDepth ,
					      p_c , ID , XMLtag , true );
    }
  
  }  // texture

  /////////////////////////////////////////////////////////////////
  // film material properties
  else if( strcmp( XMLtag->tag , "video" ) == 0
	   && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag))  ) {
#ifdef DM_HAVE_FFMPEG
    hasVideo = true;

    char filmName[XMLAttrValLength];
    char *videoSource = NULL;
    int id_live_video = 0;

    // filme name  
    *filmName = '\0';
    if( XMLtag->hasAttribute( "xlink:href" ) ) {
	char *fileName = XMLtag->getAttributeStringValue( "xlink:href" );
	strcpy( filmName , fileName );
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
	sprintf( filmName , "/dev/video%d" , id_live_video );
      }
    }

    // texture no is associated with film name
    if( *(filmName) ) {
      bool newText;

      // texture for a film: just a name and an ID
      // sets a texture ID that will be used to refer to the
      // video fram
      hasTexture = true;
      materialTexture = new dm_Texture();
      materialTexture->rectangle = false;
      materialTexture->mipmapped = false; 
      materialTexture->setParameters( XMLtag , 
				      false ,            // compile time
				      true ,             // video texture
				      assign , assignI );
      materialTexture->encoding = RAW; 
      materialTexture->SetTextureNo(  
	DM_CompositionNode->FindTexture( filmName , 
					 &materialTexture->encoding ,
					 materialTexture->textureDimension( materialTexture->textureTarget ) , 
					 materialTexture->rectangle ,
					 DM_DIRECT_TEXTURING , 
					 &newText ,
					 true ) );

      //materialTexture->textureNo = DM_CompositionNode->GenerateTexture( filmName );

      // printf( "textureNo %d video texture %s\n" , materialTexture->textureNo , filmName );
      
      // ID texture is generated from the beginning 
      // because texture will be bound to several images
      // but ID will not change
      materialVideo = new dm_Video();
      materialVideo->setParameters( XMLtag , 
				    false ,            // compile time
				    newText , 
				    assign , assignI );
      if( newText ) {
	// video net yet allocated: pointer recorded	
	DM_CompositionNode->video_records[ materialTexture->GetTextureNo() ] 
	  = materialVideo;
	if(materialVideo->video_source == DM_VIDEOFILE || materialVideo->video_source == DM_VIDEOLIVE)
	{
	  materialVideo->filmAudioVideo->loadFilm( filmName ,
						   materialVideo->id_live_video ,
						   materialVideo->live_video_w ,
						   materialVideo->live_video_h ,
						   materialVideo->rectangle ,
						   materialVideo->external );
	}
	else if(materialVideo->video_source == DM_VIDEOSTREAM)	{
	  materialVideo->filmAudioVideo->loadServer( filmName,
						     materialTexture->rectangle  );
	}

      }
      else {
	// video already allocated
	dm_Video *previousReference =  DM_CompositionNode->FindVideo( filmName );

	if( !previousReference ) {
	  sprintf( ErrorStr , "Error: missing previous video reference [%s]!" ,filmName ); ReportError( ErrorStr ); throw 422;
	}
	materialVideo->filmAudioVideo = previousReference->filmAudioVideo;
      }
    }
    else {
     sprintf( ErrorStr , "Error: missing film name in component ID [%s]!" , ID ); ReportError( ErrorStr ); throw 422;
    }
#endif
  }  // texture

  /////////////////////////////////////////////////////////////////
  // shader properties
  else if( strcmp( XMLtag->tag , "shader" ) == 0  && (XMLtag->tagType == OpeningTag) ) {
    hasShader = true;
    
    shader = new dm_Shader( &materialTexture 
#ifdef DM_HAVE_FFMPEG
			    , &materialVideo 
#endif
			    );
    shader->setParameters( XMLtag , false , 
			   assign , assignI );
    // the base texture of the shader is a copy
    // of the object's material texture
    if( materialTexture ) {
      shader->parse( fileDepth , p_c , subdivision );
    }
#ifdef DM_HAVE_FFMPEG
    else if( materialVideo ) {
      materialTexture->encoding = RAW;
      materialTexture->textureTarget = GL_TEXTURE_2D;
      materialTexture->textureGenerationMode = DM_BITMAP_TEXTURE;
      
      shader->parse( fileDepth , p_c , subdivision );
    }
#endif
    else {
      sprintf( ErrorStr , "Error: missing shader texture in node [%s]!" , ID ); ReportError( ErrorStr ); throw 68;
    }
  }

  ///////////////////////////////
  // unknown tag
  else {
    sprintf( ErrorStr , "Error: unknown material tag [%s] in component ID [%s]!" , XMLtag->tag , ID); ReportError( ErrorStr ); throw 79;
  }
}

void dm_MaterialProperties::print( FILE *file , int depth ) {
  if( hasColor ) {
    indent( file , depth );
    fprintf( file , "<color encoding=\"%s\" r=\"%.5f\" g=\"%.5f\" b=\"%.5f\" a=\"%.5f\" />\n" , 
	     TextureEncodingString[color_encoding] , color[0] , color[1] , color[2] , color[3] );
  }

  if( hasTexture && materialTexture 
      && materialTexture->GetTextureNo() >= 0
      && !(hasVideo)) {
    materialTexture->print( file , depth );
  }

#ifdef DM_HAVE_FFMPEG
  // fprintf( file , "hasTexture %d hasVideo %d materialVideo %d\n" , hasTexture , hasVideo , materialVideo);
  // video first time
  if( hasTexture && hasVideo && materialVideo ) {
    materialVideo->print( file , true , depth );
  }
  // video already printed
  if( hasTexture && hasVideo && !materialVideo && materialTexture ) {
    indent( file , depth );
    fprintf( file , "<video xlink:href=\"%s\"/>\n" , 
	     DM_CompositionNode
	     ->texture_names[ materialTexture->GetTextureNo() ] );
  }
#endif

  if( hasMaterial ) {
    indent( file , depth );
    fprintf( file , "<material>\n" );
    indent( file , depth + 1 );
    fprintf( file , "<ambient size=\"4\">\n" );
    indent( file , depth + 2 );
    fprintf( file , "%.5f %.5f %.5f %.5f\n" , 
	     Ambient[0] , Ambient[1] , Ambient[2] , Ambient[3] );
    indent( file , depth + 1 );
    fprintf( file , "</ambient>\n" );
    indent( file , depth + 1 );
    fprintf( file , "<diffuse size=\"4\">\n" );
    indent( file , depth + 2 );
    fprintf( file , "%.5f %.5f %.5f %.5f\n" , 
	     Diffuse[0] , Diffuse[1] , Diffuse[2] , Diffuse[3] );
    indent( file , depth + 1 );
    fprintf( file , "</diffuse>\n" );
    indent( file , depth + 1 );
    fprintf( file , "<specular size=\"4\">\n" );
    indent( file , depth + 2 );
    fprintf( file , "%.5f %.5f %.5f %.5f\n" , 
	     Specular[0] , Specular[1] , Specular[2] , Specular[3] );
    indent( file , depth + 1 );
    fprintf( file , "</specular>\n" );
    indent( file , depth + 1 );
    fprintf( file , "<emission size=\"4\">\n" );
    indent( file , depth + 2 );
    fprintf( file , "%.5f %.5f %.5f %.5f\n" , 
	     Emission[0] , Emission[1] , Emission[2] , Emission[3] );
    indent( file , depth + 1 );
    fprintf( file , "</emission>\n" );
    indent( file , depth + 1 );
    fprintf( file , "<shininess size=\"1\">\n" );
    indent( file , depth + 2 );
    fprintf( file , "%.5f\n" , 
	     Shininess );
    indent( file , depth + 1 );
    fprintf( file , "</shininess>\n" );
    indent( file , depth );
    fprintf( file , "</material>\n" );
  }

  if( hasShader ) {
    shader->print( file , depth );
  }
}

// void dm_MaterialProperties::textureModeCall( DrawingMode mode , bool *invalidDL ) {
//   //////////////////////////////////////////////////////////
//   // REAL-TIME RENDERING
//   // printf( "display mat prop hasVideo[%d] materialVideo[%d]\n" , hasVideo , materialVideo );

//   // //////////////////////////////////////////////////////////
//   // // COLOR
//   // if( hasColor ) {
//   //   glDisable( GL_TEXTURE_1D );
//   //   glDisable( GL_TEXTURE_2D );
//   //   glDisable( GL_TEXTURE_RECTANGLE );
//   //   glDisable( GL_TEXTURE_3D );
    
//   //   glEnable( GL_COLOR_MATERIAL );
//   //   glColor4fv( color );
//   // }

//   // //////////////////////////////////////////////////////////
//   // // MATERIAL
//   // if( hasMaterial ) {
    
//   //   glDisable( GL_TEXTURE_1D );
//   //   glDisable( GL_TEXTURE_2D );
//   //   glDisable( GL_TEXTURE_RECTANGLE );
//   //   glDisable( GL_TEXTURE_3D );
    
//     //printf( "material\n" );
    
//     // glEnable(GL_BLEND);
//     // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
//   //   if( hasTexture ) {
//   //     // printf( "mat diffuse %.2f %.2f %.2f %.2f\n" , Diffuse[0], Diffuse[1], Diffuse[2], Diffuse[3]);
//   //     glMaterialfv(GL_BACK, GL_DIFFUSE, Diffuse);
//   //     glMaterialfv(GL_BACK, GL_SPECULAR, Specular);
//   //     glMaterialfv(GL_BACK, GL_AMBIENT, Ambient);
//   //     glMaterialfv(GL_BACK, GL_EMISSION, Emission);
//   //     glMaterialf(GL_BACK, GL_SHININESS, Shininess);
//   //     glMaterialfv(GL_FRONT, GL_DIFFUSE, Diffuse);
//   //     glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
//   //     glMaterialfv(GL_FRONT, GL_AMBIENT, Ambient);
//   //     glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
//   //     glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
//   //   }
//   //   else {
//   //     glMaterialfv(GL_BACK, GL_DIFFUSE, gray_opaque);
//   //     glMaterialfv(GL_BACK, GL_SPECULAR, gray_opaque);
//   //     glMaterialfv(GL_BACK, GL_AMBIENT, gray_opaque);
//   //     glMaterialfv(GL_BACK, GL_EMISSION, gray_opaque);
//   //     glMaterialfv(GL_FRONT, GL_DIFFUSE, gray_opaque);
//   //     glMaterialfv(GL_FRONT, GL_SPECULAR, gray_opaque);
//   //     glMaterialfv(GL_FRONT, GL_AMBIENT, gray_opaque);
//   //     glMaterialfv(GL_FRONT, GL_EMISSION, gray_opaque);
//   //   }
//   // }
//   // else {
//   //   glMaterialfv(GL_BACK, GL_AMBIENT, black_opaque);
//   //   glMaterialfv(GL_BACK, GL_DIFFUSE, white_opaque);
//   //   glMaterialfv(GL_BACK, GL_SPECULAR, black_opaque);
//   //   glMaterialfv(GL_BACK, GL_EMISSION, black_opaque);
//   //   glMaterialfv(GL_FRONT, GL_AMBIENT, black_opaque);
//   //   glMaterialfv(GL_FRONT, GL_DIFFUSE, white_opaque);
//   //   glMaterialfv(GL_FRONT, GL_SPECULAR, black_opaque);
//   //   glMaterialfv(GL_FRONT, GL_EMISSION, black_opaque);
//   // }
  
//   //////////////////////////////////////////////////////////
//   // TEXTURE
//   if( hasTexture 
//       && (materialTexture && materialTexture->GetTextureNo() >= 0 ) ){
//     if( DM_EnvironmentNode->texture_displayed ) {
//       if( !hasShader) {
// 	// 	  if( DM_CompositionNode->GetTextureID(materialTexture->GetTextureNo() ) == 0 ) {
// 	// 	    sprintf( ErrorStr , "Error: unknown texture ID [%d]!" , materialTexture->GetTextureNo() ); ReportError( ErrorStr ); throw 10;
// 	// 	  }
// 	// printf( "materialTexture display [%d] [%d]\n" , materialTexture->GetTextureNo() , DM_CompositionNode->GetTextureID(materialTexture->GetTextureNo() ));
// 	materialTexture->textureMode( invalidDL );
	
// 	// video frame binding is not performed here because
// 	// this part of rendering is stored into the display list
	
//       }
//       else {
// 	// 	  printf( "DECAL shader texture[%d]\n" , 
// 	// 		  DM_CompositionNode->GetTextureID( materialTexture->GetTextureNo() ) );
// 	materialTexture->textureMode( invalidDL );
// 	// glEnable(GL_BLEND);
// 	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 	// glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white_transparent );
// 	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); 

// 	// 	  glDisable(GL_BLEND);
// 	// 	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
// 	// printf( "glBindTexture texture[%d/%d] \n" , materialTexture->GetTextureNo() ,DM_CompositionNode->GetTextureID( materialTexture->GetTextureNo() ));
// 	// texture binding is made by the shader itself
// 	// through loadShaderTexture
// 	// materialTexture->bindTexture( invalidDL );
//       }
      
//     }
//     // environment configuration: texture not displayed
//     // textures are replaced by simple colors
//     else {
//       float h = (float)materialTexture->GetTextureNo() 
// 	/ (float)(DM_CompositionNode->NbTextures);
//       float rvb[4] = {0.0, 0.0, 0.0, 1.0};
//       if( h < 1.0/6.0 ) {
// 	rvb[0] = 1.0;
// 	rvb[1] = h / (1.0F/6.0F);
//       }
//       else if( h < 2.0/6.0 ) {
// 	rvb[1] = 1.0;
// 	rvb[0] = ((2.0F/6.0F) - h) / (1.0F/6.0F);
//       }
//       else if( h < 3.0/6.0 ) {
// 	rvb[1] = 1.0;
// 	rvb[2] = (h - (2.0F/6.0F) ) / (1.0F/6.0F);
//       }
//       else if( h < 4.0/6.0 ) {
// 	rvb[2] = 1.0;
// 	rvb[1] = ((4.0F/6.0F) - h) / (1.0F/6.0F);
//       }
//       else if( h < 5.0/6.0 ) {
// 	rvb[2] = 1.0;
// 	rvb[0] = (h - (4.0F/6.0F)) / (1.0F/6.0F);
//       }
//       else if( h < 5.0/6.0 ) {
// 	rvb[0] = 1.0;
// 	rvb[2] = ((6.0F/6.0F) - h) / (1.0F/6.0F);
//       }
      
//       for( int ind = 0 ; ind < 3 ; ind++ ) {
// 	rvb[ind] *= .5;
//       }
//       glMaterialfv(GL_BACK, GL_DIFFUSE,  rvb );
//       glMaterialfv(GL_FRONT, GL_DIFFUSE,  rvb );
//       for( int ind = 0 ; ind < 3 ; ind++ ) {
// 	rvb[ind] *= .5;
//       }
//       glMaterialfv(GL_BACK, GL_AMBIENT,  rvb );
//       glMaterialfv(GL_FRONT, GL_AMBIENT, rvb );
//       glMaterialfv(GL_BACK, GL_SPECULAR, white_opaque );
//       glMaterialfv(GL_FRONT, GL_SPECULAR, white_opaque );
//       glMaterialfv(GL_BACK, GL_EMISSION, white_opaque );
//       glMaterialfv(GL_FRONT, GL_EMISSION, white_opaque );
//       glMaterialf(GL_BACK, GL_SHININESS, 0.0 );
//       glMaterialf(GL_FRONT, GL_SHININESS, 0.0 );
//     }
//   }
  
//   // if( !hasColor && !hasMaterial
//   //     && !(hasTexture && materialTexture 
//   // 	   && materialTexture->GetTextureNo() >= 0 ) ) {
//   //   //printf( "Unknown material\n" );
//   //   glDisable( GL_TEXTURE_1D );
//   //   glDisable( GL_TEXTURE_2D );
//   //   glDisable( GL_TEXTURE_RECTANGLE );
//   //   glDisable( GL_TEXTURE_3D );
//     //       glColor4f( 1.0 , 1.0 , 1.0 , 1.0 );
//     //        glEnable( GL_COLOR_MATERIAL );
    
//     //       glMaterialfv(GL_BACK, GL_DIFFUSE, black_opaque );
//     //       glMaterialfv(GL_FRONT, GL_DIFFUSE, black_opaque );
//     //       glMaterialfv(GL_BACK, GL_AMBIENT, black_opaque );
//     //       glMaterialfv(GL_FRONT, GL_AMBIENT, black_opaque );
//       //       glMaterialfv(GL_BACK, GL_SPECULAR, black_opaque );
//       //       glMaterialfv(GL_FRONT, GL_SPECULAR, black_opaque );
//       //       glMaterialfv(GL_BACK, GL_EMISSION, black_opaque );
//       //       glMaterialfv(GL_FRONT, GL_EMISSION, black_opaque );
//       //       glMaterialf(GL_BACK, GL_SHININESS, 0.0 );
//       //       glMaterialf(GL_FRONT, GL_SHININESS, 0.0 );
//   // }
// }

// void dm_MaterialProperties::close( DrawingMode mode ) {
//   glDisable( GL_TEXTURE_1D );
//   glDisable( GL_TEXTURE_2D );
//   glDisable( GL_TEXTURE_RECTANGLE );
//   glDisable( GL_TEXTURE_3D );
  
//   glDisable( GL_COLOR_MATERIAL );
  
//   glDisable(GL_BLEND);
  
//   glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, 
// 	      black_transparent );
  
//   glColor4fv( white_opaque );
//   //           glMaterialfv(GL_BACK, GL_DIFFUSE, black_opaque );
//   //           glMaterialfv(GL_FRONT, GL_DIFFUSE, black_opaque );
//   //           glMaterialfv(GL_BACK, GL_AMBIENT, black_opaque );
//   //           glMaterialfv(GL_FRONT, GL_AMBIENT, black_opaque );
//   //           glMaterialfv(GL_BACK, GL_SPECULAR, black_opaque );
//   //           glMaterialfv(GL_FRONT, GL_SPECULAR, black_opaque );
//   //           glMaterialfv(GL_BACK, GL_EMISSION, black_opaque );
//   //           glMaterialfv(GL_FRONT, GL_EMISSION, black_opaque );
//   //           glMaterialf(GL_BACK, GL_SHININESS, 0.0 );
//   //           glMaterialf(GL_FRONT, GL_SHININESS, 0.0 );
  
//   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); 
// }

void dm_MaterialProperties::displayVideo( DrawingMode mode ) {
#ifdef DM_HAVE_FFMPEG
  if( hasVideo && materialVideo ) {
    materialVideo->displayVideo( mode , 
				 materialTexture->GetTextureNo() , 
				 materialVideo->environmentMapTextureNo ,
				 materialTexture->rectangle );
  }
#endif
}

void dm_MaterialProperties::update( void ) {
#ifdef DM_HAVE_FFMPEG
  if( hasVideo && materialVideo ) {
      //printf("update video %s\n" , materialVideo->fileName);
      materialVideo->update();
  }
#endif
}

void dm_MaterialProperties::operator=(dm_MaterialProperties& p) {
  int ind;

  hasColor = p.hasColor;
  hasMaterial = p.hasMaterial;
  hasShader = p.hasShader;

  for( ind = 0 ; ind < 4 ; ind++ ) {
    color[ind] = p.color[ind]; 
    Ambient[ind] = p.Ambient[ind]; 
    Diffuse[ind] = p.Diffuse[ind]; 
    Specular[ind] = p.Specular[ind]; 
    Emission[ind] = p.Emission[ind]; 
  }
  Shininess = p.Shininess;

  hasTexture = p.hasTexture;
  hasVideo = p.hasVideo;
  
  if (materialTexture) {
    delete materialTexture;
    materialTexture = NULL;
  }
  if (p.materialTexture) {
    materialTexture = new dm_Texture;
    *materialTexture = *(p.materialTexture);
  }

#ifdef DM_HAVE_FFMPEG
  if (materialVideo) {
    delete materialVideo;
    materialVideo = NULL;
  }
  if (p.materialVideo) {
    materialVideo = new dm_Video();
    *materialVideo = *(p.materialVideo);
  }
#endif

  shader = p.shader;
  if (shader) {
    shader->baseMaterialTexture = &materialTexture;
#ifdef DM_HAVE_FFMPEG
    shader->baseMaterialVideo = &materialVideo;
#endif
  }

//   if (shader) {
//     delete shader;
//     shader = NULL;
//   }
//   if (p.shader) {
//     shader = new dm_Shader( &materialTexture 
// #ifdef DM_HAVE_FFMPEG
// 			    , &materialVideo 
// #endif
// 			   );
//     *shader = *(p.shader);
//     shader->baseMaterialTexture = &materialTexture;
// #ifdef DM_HAVE_FFMPEG
//     shader->baseMaterialVideo = &materialVideo;
// #endif
//   }

}


