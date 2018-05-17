/*! \file dm-shader.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-shader.cpp
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

dm_Shader  *CurrentShader = NULL;

unsigned long getFileLength(ifstream& file)
{
    if(!file.good()) return 0;
    
    file.seekg(0,ios::end);
    unsigned long len = file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

int loadshader(string filename, GLuint shader)
{
  GLchar* shaderSource;
  unsigned long len;

  ifstream file;
  file.open(filename.c_str(), ios::in); // opens as ASCII!
  if(!file) {
    sprintf( ErrorStr , "Error: shader file not found %s!" , filename.c_str() ); ReportError( ErrorStr ); throw 53;
    return -1;
  }
  
  len = getFileLength(file);
  if (len==0) {
    sprintf( ErrorStr , "Error: empty shader file %s!" , filename.c_str() ); ReportError( ErrorStr ); throw 53;
  }
  
  shaderSource = new char[len+1];
  if (shaderSource == 0)  {
    sprintf( ErrorStr , "Error: shader string allocation error %s!" , filename.c_str() ); ReportError( ErrorStr ); throw 53;
  }
  
  printf( "Loading %s\n" , filename.c_str() );

  // len isn't always strlen cause some characters are stripped in ascii read...
  // it is important to 0-terminate the real length later, len is just max possible value... 
  shaderSource[len] = 0; 
  
  unsigned int i=0;
  while (file.good()) {
    shaderSource[i] = file.get();       // get character from file.
    if (!file.eof())
      i++;
  }
  
  shaderSource[i] = 0;  // 0-terminate it at the correct position
  
  file.close();
  
  glShaderSource( shader, 1, (const char **)&shaderSource, NULL );
  
  delete[] shaderSource;
  
  return 0; // No Error
}


void printLog(GLuint obj)
{
	int infologLength = 0;
	int maxLength;
 
	if(glIsShader(obj))
		glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	else
		glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
 
	char *infoLog = new char[maxLength];
 
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
 
	if (infologLength > 0)
		printf("%s\n",infoLog);

       delete infoLog;
}

dm_VertexShader *dm_Composition::FindVertexShader( 
			     char *fileName ,
			     dm_ShaderSourceObject *shaderSource ) {
  // define search path for Cg shaders file
  char nomFicShader[512];
  strcpy( nomFicShader ,  DM_EnvironmentNode->shader_programs );
  strcat( nomFicShader ,  fileName );

  //printf( "beginning of dm_VertexShader Loading %s\n" , nomFicShader );

  for( int ind = 0 ; ind < NbVertexShaders ; ind++ ) {
    if( vertexShader_names[ind] 
	&& strcmp( vertexShader_names[ind] , nomFicShader ) == 0 ) {
      return tabVertexShaders[ind];
    }
  }
  if( NbVertexShaders >= DM_EnvironmentNode->Nb_max_shaders ) {
    sprintf( ErrorStr , "Error: unable to load vertexShader [%s] exceeds max_shaders parameter!" , nomFicShader ); ReportError( ErrorStr ); throw 83;
  }

  ////////// loading the vertex program
  // loading the code
  dm_VertexShader *vertexShader = new dm_VertexShader();
  vertexShader->InitializeShader( nomFicShader ,
				  shaderSource );

  vertexShader_names[NbVertexShaders] = new char[XMLAttrValLength];
  strcpy( vertexShader_names[NbVertexShaders] , nomFicShader );
  tabVertexShaders[NbVertexShaders] = vertexShader;
  NbVertexShaders++;

  //printf( "end of dm_VertexShader Loading %s\n" , nomFicShader );
  return vertexShader;
}


dm_GeometryShader *dm_Composition::FindGeometryShader( 
			     char *fileName , char * geometry_program_output_type, int geometry_max_vertex,
			     dm_ShaderSourceObject *shaderSource ) {
  // define search path for Cg shaders file
  // only concatenates with prefix subdir if the
  // file name is not empty
  char nomFicShader[512];
  if( *fileName ) {
    strcpy( nomFicShader  ,  DM_EnvironmentNode->shader_programs );
    strcat( nomFicShader ,  fileName );
  }
  else
    *nomFicShader = 0;

  //printf( "beginning of dm_GeoemtryShader Loading %s\n" , nomFicShader );

  for( int ind = 0 ; ind < NbGeometryShaders ; ind++ ) {
    if( geometryShader_names[ind] 
	&& strcmp( geometryShader_names[ind] , nomFicShader ) == 0 ) {
      return tabGeometryShaders[ind];
    }
  }
  if( NbGeometryShaders >= DM_EnvironmentNode->Nb_max_shaders ) {
    sprintf( ErrorStr , "Error: unable to load geometry Shader [%s] exceeds max_shaders parameter!" , nomFicShader ); ReportError( ErrorStr ); throw 83;
  }

  ////////// loading the geometry program
  // loading the code
  dm_GeometryShader *geometryShader = new dm_GeometryShader();
  geometryShader->InitializeShader( nomFicShader , geometry_program_output_type, geometry_max_vertex,
				  shaderSource );

  geometryShader_names[NbGeometryShaders] = new char[XMLAttrValLength];
  strcpy( geometryShader_names[NbGeometryShaders] , nomFicShader );
  tabGeometryShaders[NbGeometryShaders] = geometryShader;
  NbGeometryShaders++;


  //printf( "end of dm_GeoemtryShader Loading %s\n" , nomFicShader );
  return geometryShader;
}


dm_FragmentShader *dm_Composition::FindFragmentShader(
				 char *fileName  , 
				 dm_ShaderSourceObject   *shaderSource ) {
  // define search path for Cg shaders file
  char nomFicShader[512];
  strcpy( nomFicShader ,  DM_EnvironmentNode->shader_programs );
  strcat( nomFicShader ,  fileName );

  //printf( "beginning of dm_FragmentShader Loading %s\n" , nomFicShader );

  for( int ind = 0 ; ind < NbFragmentShaders ; ind++ ) {
    if( fragmentShader_names[ind] 
	&& strcmp( fragmentShader_names[ind] , nomFicShader ) == 0 ) {
      return tabFragmentShaders[ind];
    }
  }
  if( NbFragmentShaders >= DM_EnvironmentNode->Nb_max_shaders ) {
    sprintf( ErrorStr , "Error: unable to load fragmentShader [%s] exceeds max_shaders parameter!" , nomFicShader ); ReportError( ErrorStr ); throw 83;
  }

  ////////// loading the fragment program
  // loading the code
  dm_FragmentShader *fragmentShader = new dm_FragmentShader();
  fragmentShader->InitializeShader( nomFicShader , 
				    shaderSource );

  fragmentShader_names[NbFragmentShaders] = new char[XMLAttrValLength];
  strcpy( fragmentShader_names[NbFragmentShaders] , nomFicShader );
  tabFragmentShaders[NbFragmentShaders] = fragmentShader;
  NbFragmentShaders++;

  //printf( "end of dm_FragmentShader Loading %s\n" , nomFicShader );
  return fragmentShader;
}

dm_VertexShader::dm_VertexShader( void ) {
  vertex_program_name = NULL;
  InitString( &vertex_program_name );

  vertexProgram = NULL_ID;
}

void dm_VertexShader::InitializeShader( char *nomFic , 
					dm_ShaderSourceObject *shaderSrc ) {
  StrCpy( &vertex_program_name , nomFic );

  // Create and compile the vertex shader
  vertexProgram = glCreateShader( GL_VERTEX_SHADER );
  string fileName(vertex_program_name);
  loadshader( fileName, vertexProgram);
  glCompileShader( vertexProgram );

  // + check
  printLog(vertexProgram);
}

void dm_VertexShader::cleanExit( void ) {
  if (vertexProgram)
    glDeleteShader( vertexProgram );
  exit(0);
}


dm_GeometryShader::dm_GeometryShader( void ) {
  geometry_program_name = NULL;
  InitString( &geometry_program_name );
  geometry_program_ouput_type = NULL;
  InitString( &geometry_program_ouput_type );

  geometryProgram = NULL_ID;

  options[0] =  new char[16];
  options[1] =  new char[16];
}

void dm_GeometryShader::InitializeShader(  char *nomFic ,  char *outType , int outMax,
					   dm_ShaderSourceObject   *shaderSrc ) {
  StrCpy( &geometry_program_name , nomFic );
  StrCpy( &geometry_program_ouput_type , outType );

  if (strlen(geometry_program_name)>0){
    // Create and compile the geometry shader
    geometryProgram = glCreateShader( GL_GEOMETRY_SHADER );
    string fileName(geometry_program_name);
    loadshader( fileName, geometryProgram);
    glCompileShader( geometryProgram );

    // printf( "Geometry program output type: %s \n" , geometry_program_ouput_type);

    if (strlen(geometry_program_ouput_type)>0){
	  
      // printf( "\tLength positive.\n");

      if (strcmp(geometry_program_ouput_type,"point")==0){
	strcpy( options[0] , (char *)"-po");
	strcpy( options[1] , (char *)"POINT_OUT");
      }
      else {
	    
	if (strcmp(geometry_program_ouput_type,"line")==0){
	  strcpy( options[0] , (char *)"-po");
	  strcpy( options[1] , (char *)"LINE_OUT");
	}
	else {
	  
	  if (strcmp(geometry_program_ouput_type,"triangle")==0){
	    
	    // printf( "\tIs Triangle.\n");
	    strcpy( options[0] , (char *)"-po");
	    strcpy( options[1] , (char *)"TRIANGLE_OUT");
	  }
	  else {
	      
	    if (strcmp(geometry_program_ouput_type,"input_type")!=0){
	      sprintf( ErrorStr , "Incorrect geometry program %s output type  (%s)\n" , geometry_program_name , geometry_program_ouput_type ); ReportError( ErrorStr ); throw 83;
	    }
	  }
	}
      }
    }

    // + check
    printLog(geometryProgram);    
  }
  geometryProgram = NULL_ID;
}

void dm_GeometryShader::cleanExit(void) {
  if (geometryProgram)
    glDeleteShader( geometryProgram );
  exit(0);
}

dm_FragmentShader::dm_FragmentShader( void ) {
  fragment_program_name = NULL;
  InitString( &fragment_program_name );

  fragmentProgram = NULL_ID;
}

void dm_FragmentShader::InitializeShader( char *nomFic  , 
					  dm_ShaderSourceObject *shaderSrc ) {
  StrCpy( &fragment_program_name , nomFic );

  // Create and compile the vertex shader
  fragmentProgram = glCreateShader( GL_FRAGMENT_SHADER );
  string fileName(fragment_program_name);
  loadshader( fileName, fragmentProgram);
  glCompileShader( fragmentProgram );

  // + check
  printLog(fragmentProgram);

}

void dm_FragmentShader::cleanExit(void) {
  if (fragmentProgram)
    glDeleteShader( fragmentProgram );
  exit(0);
}

// a pointer to the texture & the video of the material properties
dm_Shader::dm_Shader( dm_Texture  ** materialTexture 
#ifdef DM_HAVE_FFMPEG
		      , dm_Video    ** materialVideo 
#endif
		      ) {
  fragment_program_name = NULL;
  vertex_program_name = NULL;
  geometry_program_name = NULL;
  geometry_program_output_type = NULL;
  geometry_max_vertex = 0;
  InitString( &fragment_program_name );
  InitString( &vertex_program_name );
  InitString( &geometry_program_name );
  InitString( &geometry_program_output_type );
  
  vertexShader = NULL;
  fragmentShader = NULL;
  geometryShader = NULL;

  shaderProgram = NULL_ID;

  id = NULL;
  InitString( &id );

  shaderSource = NULL;
  baseMaterialTexture = materialTexture;
#ifdef DM_HAVE_FFMPEG
  baseMaterialVideo = materialVideo;
#endif

  tangentParam = NULL_ID;
  diffuseParam = NULL_ID;
  specularParam = NULL_ID;

  vertexLookupTableParam = NULL;
  nbVertexTextureParam = 0;

  geometryLookupTableParam = NULL;
  nbGeometryTextureParam = 0;

  fragmentDecalParamLoc = NULL_ID;
  fragmentLookupTableParam = NULL;
  nbFragmentTextureParam = 0;
}

void dm_Shader::SetId( char *newId ) {
  StrCpy( &id , newId );
}

void dm_Shader::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }
}

bool dm_Shader::getParameters( char *attribute , 
			       DataType * dataType ,
			       double * valDouble ,
			       int * valInt ,
			       bool * valBool ,
			       char ** valString ,
			       int * tableOrMatrixSize ,
			       dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = id;
    return true;
  }
  else {
    if( shaderSource ) {
      if( shaderSource->getParameters( attribute , 
				   dataType ,
				   valDouble ,
				   valInt ,
				   valBool ,
				   valString ,
				   tableOrMatrixSize ,
				       valScalars ) ) {
	return true;
      }
    }
  }
  return false;
}

void dm_Shader::parse( int *fileDepth , int *p_c , 
		       dm_Subdivision * subdivision ) {
  dm_XMLTag  *XMLtag = new dm_XMLTag();

  while( true ) {
    if( *p_c == EOF ) {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unbalanced node tag in node [%s]!" , id ); ReportError( ErrorStr ); throw 68;
    }
    
    // new tag XML : transformation component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    
    // node closing tag
    if( strcmp( XMLtag->tag , "shader" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      //shader->print( stdout , 0 );
      if( !*(fragment_program_name) ||
	  !*(vertex_program_name) ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: missing vertex or fragment shader in shader [%s]!" , id ); ReportError( ErrorStr ); throw 263;
      }	
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    ////////////////////////////////////////////////////
    // vertex shader
    else if( strcmp( XMLtag->tag , "vertexShader" ) == 0 && (XMLtag->tagType == EmptyTag) ) {
      StrCpy(  &vertex_program_name , XMLtag->getAttributeStringValue( "xlink:href" ) );
      // printf("Registering [%s]...\n" , vertex_program_name);
    }
    // geometry shader
    else if( strcmp( XMLtag->tag , "geometryShader" ) == 0 && (XMLtag->tagType == EmptyTag) ) {
      StrCpy( &geometry_program_name , XMLtag->getAttributeStringValue( "xlink:href" ) );   

	  if (XMLtag->hasAttribute( "output_type" ))
		  StrCpy( &geometry_program_output_type , XMLtag->getAttributeStringValue( "output_type" ) );
	  else
		  StrCpy( &geometry_program_output_type , "" );

	  if (XMLtag->hasAttribute( "max_output_vertex" ))
		  geometry_max_vertex = XMLtag->getAttributeIntValue( "max_output_vertex", false  );
	  else
		  geometry_max_vertex = 0;
  }
    // fragment shader
    else if( strcmp( XMLtag->tag , "fragmentShader" ) == 0 && (XMLtag->tagType == EmptyTag) ) {
      StrCpy( &fragment_program_name , XMLtag->getAttributeStringValue( "xlink:href" ) );
    }

    ////////////////////////////////////////////////////
    // shader source
    else if( strcmp( XMLtag->tag , "shader_source" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
       // printf( "Tag %s\n" , XMLtag->tag );
      if( shaderSource ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: shader_source already allocated in sharder [%s]!" , id ); ReportError( ErrorStr ); throw 285;
      }

      shaderSource = new dm_ShaderSourceObject();
      shaderSource->setParameters( XMLtag , false , 
				   assign , assignI );
      shaderSource->parse( fileDepth , p_c );
      // printf( "End of Tag %s\n" , XMLtag->tag );
      // shaderSource->print( stdout , true , 1 );
    } // shaderSource

    else {
      sprintf( ErrorStr , "Error: unknown shader component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); delete XMLtag; throw 281;
    }

  }  // parsing embedded tags
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Shader::cleanExit( void ) {
  if (vertexShader->vertexProgram)
    glDeleteShader( vertexShader->vertexProgram );
  if (geometryShader->geometryProgram)
    glDeleteShader( geometryShader->geometryProgram );
  if (fragmentShader->fragmentProgram)
    glDeleteShader( fragmentShader->fragmentProgram );
  if (shaderProgram)
    glDeleteShader( shaderProgram );

  exit(0);
}

void dm_Shader::InitializeShader( void ) {
  CurrentShader = this;

  //////////////////////////////
  /// TEXTURE SAMPLER
  /////////////////////////////////
  sampler_nearest = 0;
  glGenSamplers(1, &sampler_nearest);
  glSamplerParameteri(sampler_nearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glSamplerParameteri(sampler_nearest, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glSamplerParameteri(sampler_nearest, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glSamplerParameteri(sampler_nearest, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glSamplerParameterf(sampler_nearest, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);    // glBindTexture(GL_TEXTURE

  //////////////////////////////
  /// TEXTURE SAMPLER
  /////////////////////////////////
  sampler_linear = 0;
  glGenSamplers(1, &sampler_linear);
  glSamplerParameteri(sampler_linear, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glSamplerParameteri(sampler_linear, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glSamplerParameteri(sampler_linear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glSamplerParameteri(sampler_linear, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameterf(sampler_linear, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);    // glBindTexture(GL_TEXTURE

  // printf( "shader initialization %s %s\n" , vertex_program_name , fragment_program_name );

  vertexShader 
    = DM_CompositionNode->FindVertexShader( vertex_program_name , shaderSource );

  geometryShader
    = DM_CompositionNode->FindGeometryShader( 
		geometry_program_name, geometry_program_output_type , geometry_max_vertex, shaderSource );

  fragmentShader
    = DM_CompositionNode->FindFragmentShader( fragment_program_name , shaderSource );

  // printf( "end of shader loading %s %s\n" , vertex_program_name , fragment_program_name );

  // Link the vertex and fragment (and possible geometry) shader into a shader program
  shaderProgram = glCreateProgram();
  glAttachShader( shaderProgram, vertexShader->vertexProgram );
  glAttachShader( shaderProgram, fragmentShader->fragmentProgram );
  glBindFragDataLocation( shaderProgram, 0, "outColor0" );
  for( int attachment = 1 ; attachment < shaderSource->nb_attachments ; attachment++ ) {
    char attribName[12];
    sprintf( attribName , "outColor%d" , attachment );
    glBindFragDataLocation( shaderProgram, attachment, attribName );
  }
  glLinkProgram( shaderProgram );

  // printf( "end of shader linking %s %s\n" , vertex_program_name , fragment_program_name );

  // + check
  printLog(shaderProgram);

  if( !shaderProgram ) {
    printf("Unable to link shader program, exiting...\n");
    cleanExit();
  }

  //////////////////////////////
  /// SHADER PARAMETER BINDINGS
  /////////////////////////////////

  // binding the VERTEX PROGRAM parameters
  // binding the vertex program matrices
  projMatrixLoc = glGetUniformLocation(shaderProgram, "vp_projMatrix");
  if( projMatrixLoc == -1 ) {
    printf("Unable to access projection matrix 'vp_projMatrix' in vertex program, exiting...\n");
    cleanExit();
  }
  viewMatrixLoc = glGetUniformLocation(shaderProgram, "vp_viewMatrix");
  if( viewMatrixLoc == -1 ) {
    printf("Unable to access projection matrix 'vp_viewMatrix' in vertex program, exiting...\n");
    cleanExit();
  }
  messageTransparencyLoc = -1;
  if( shaderSource->message_display ) {
    messageTransparencyLoc = glGetUniformLocation(shaderProgram, "glsl_fs_message_transparency");
    if( messageTransparencyLoc == -1 ) {
      printf("Unable to access message transparency 'glsl_fs_message_transparency' in fragment program, exiting...\n");
      cleanExit();
    }
  }
  timeParam = glGetUniformLocation(shaderProgram, "time");
  printLog(shaderProgram);

  // binding the vertex program geometrical parameters
  // tangentParam = glGetUniformLocation(shaderProgram, "tangent"); 
  // diffuseParam = glGetUniformLocation(shaderProgram, "diffuse");
  // specularParam = glGetUniformLocation(shaderProgram, "specular");
  // if (tangentParam == -1) {
  //   printf("Unable to retrieve vertex program parameters 'tangent'...\n");
  // }
  // if (diffuseParam == -1) {
  //   printf("Unable to retrieve vertex program parameters 'diffuse'...\n");
  // }
  // if (specularParam == -1) {
  //   printf("Unable to retrieve vertex program parameters 'specular'...\n");
  // }
  
  if( !shaderSource ) {
    printf("Missing shader source in shader %s, exiting...\n",id);
    cleanExit();
  }

  uniformVertexParameters
    = new GLint[shaderSource->nb_vertexUniformVars];
  for( int indVertexVar = 0 ; 
       indVertexVar < shaderSource->nb_vertexUniformVars ; 
       indVertexVar++ ) {
    uniformVertexParameters[indVertexVar] 
      = glGetUniformLocation(shaderProgram, 
			    shaderSource->vertexUniformVarName[ indVertexVar ]);
    if (uniformVertexParameters[indVertexVar] == -1) {
      printf("Unable to retrieve vertex program parameter #%d %s in shader %s, exiting...\n",indVertexVar,shaderSource->vertexUniformVarName[ indVertexVar ] , id );
      cleanExit();
    }
  }
  
  // association of texture parameters with lookuptablexx uniform vars
  nbVertexTextureParam = 0;
  for( int ind = 0 ; ind < shaderSource->nb_shaderTextures ; ind++ ) {
    if( shaderSource->textureTarget[ ind ] == DM_VERTEX_TEXTURE ) {
      nbVertexTextureParam++;
    }
  }
  
  if( nbVertexTextureParam > 0 ) {
    vertexLookupTableParam = new GLint[ nbVertexTextureParam ];
    
    nbVertexTextureParam = 0;
    for( int ind = 0 ; ind < shaderSource->nb_shaderTextures ; ind++ ) {
      char varString[64];
      if( shaderSource->textureTarget[ ind ] == DM_VERTEX_TEXTURE ) {
	nbVertexTextureParam++;
	sprintf( varString , "vp_lookupTable%d" , shaderSource->textureIndex[ ind ] + 1 );
	vertexLookupTableParam[ shaderSource->textureIndex[ ind ] ] 
	    = glGetUniformLocation(shaderProgram , varString );
	// printf("vertexProgram parameter %s loaded index %d \n"  , varString , shaderSource->textureIndex[ ind ] );
	if (vertexLookupTableParam[ shaderSource->textureIndex[ ind ] ] == -1 ) {
	  printf("Unable to retrieve vertex program parameter %s, exiting...\n"  , varString );
	  cleanExit();
	}
      }
    }
  }
  else  {
    vertexLookupTableParam = NULL;
  }

  // printf( "end of vertex program bindings %s %s\n" , vertex_program_name , fragment_program_name );

  // binding the geometry shader parameters
  uniformGeometryParameters = new GLint[shaderSource->nb_geometryUniformVars];
  for( int indGeometryVar = 0 ; 
       indGeometryVar < shaderSource->nb_geometryUniformVars ; 
       indGeometryVar++ ) {
    
    uniformGeometryParameters[indGeometryVar] 
      = glGetUniformLocation(shaderProgram, 
			    shaderSource->geometryUniformVarName[ indGeometryVar ]);
    if (uniformGeometryParameters[indGeometryVar] == -1) {
      printf("Unable to retrieve geometry program parameter #%d %s, exiting...\n",indGeometryVar,shaderSource->geometryUniformVarName[ indGeometryVar ]);
      cleanExit();
    }
  }
  
  // association of texture parameters with lookuptablexx uniform vars
  nbGeometryTextureParam = 0;
  for( int ind = 0 ; ind < shaderSource->nb_shaderTextures ; ind++ ) {
    if( shaderSource->textureTarget[ ind ] == DM_GEOMETRY_TEXTURE ) {
      nbGeometryTextureParam++;
    }
  }
  
  if( nbGeometryTextureParam > 0 ) {
    geometryLookupTableParam = new GLint[ nbGeometryTextureParam ];
    
    nbGeometryTextureParam = 0;
    for( int ind = 0 ; ind < shaderSource->nb_shaderTextures ; ind++ ) {
      
      // printf("Index de texture %i : %i \n",ind,shaderSource->textureIndex[ ind ]);
      
      char varString[64];
      
      if( shaderSource->textureTarget[ ind ] == DM_GEOMETRY_TEXTURE ) {
	nbGeometryTextureParam++;
	sprintf( varString , "gs_lookupTable%d" , shaderSource->textureIndex[ ind ] + 1 );
	geometryLookupTableParam[ shaderSource->textureIndex[ ind ] ] 
	  = glGetUniformLocation(shaderProgram , varString );
	// printf("geometryProgram parameter %s loaded index %d param %ld\n"  , varString , shaderSource->textureIndex[ ind ] , geometryLookupTableParam[ shaderSource->textureIndex[ ind ] ]);
	if ( geometryLookupTableParam[ shaderSource->textureIndex[ ind ] ] == -1 ) {
	  printf("Unable to retrieve geometry program parameter %s, exiting...\n"  , varString );
	    cleanExit();
	}
      }
      else {
	// printf("geometryProgram parameter %s NOT loaded for GS\n"  , varString  );
      }
    }
  }
  else  {
    geometryLookupTableParam = NULL;
  }
  
  // printf( "end of geometry program bindings %s %s\n" , vertex_program_name , fragment_program_name );

  // binding the fragment program parameters
  // generalized fragment shader
  fragmentDecalParamLoc = glGetUniformLocation(shaderProgram, "fs_decal");
  // printf( "param fragment shader texture %d\n" , fragmentDecalParamLoc );
  if ( fragmentDecalParamLoc == -1 ) {
    printf("Unable to retrieve 'fs_decal' texture parameter in shader %s, exiting...\n" , id );
    cleanExit();
  }

  // association of texture parameters with lookuptablexx uniform vars
  nbFragmentTextureParam = 0;
  for( int ind = 0 ; ind < shaderSource->nb_shaderTextures ; ind++ ) {
    if( shaderSource->textureTarget[ ind ] == DM_FRAGMENT_TEXTURE ) {
      nbFragmentTextureParam++;
    }
  }
  
  if( nbFragmentTextureParam > 0 ) {
    fragmentLookupTableParam = new GLint[ nbFragmentTextureParam ];
    
    nbFragmentTextureParam = 0;
    for( int textureRank = 0 ; textureRank < shaderSource->nb_shaderTextures ; textureRank++ ) {
      char varString[64];
      if( shaderSource->textureTarget[ textureRank ] == DM_FRAGMENT_TEXTURE ) {
	nbFragmentTextureParam++;
	sprintf( varString , "fs_lookupTable%d" , shaderSource->textureIndex[ textureRank ] + 1 );

	// printf( "param fragment shader texture fs_lookupTable%d\n" , shaderSource->textureIndex[ textureRank ] + 1 );
	fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ] ] 
	  = glGetUniformLocation( shaderProgram , varString );
	// printf( "param fragment shader texture fs_lookupTable%d: %d\n" , shaderSource->textureIndex[ textureRank ] + 1 , fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ] ] );
	if (fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ] ] == -1 ) {
	  printf("Unable to retrieve fragment program parameter %s, exiting...\n"  , varString );
	  cleanExit();
	}
      }
    }
  }
  else  {
    fragmentLookupTableParam = NULL;
  }
  
  uniformFragmentParameters = new GLint[shaderSource->nb_fragmentUniformVars];
  for( int indFragmentVar = 0 ; 
       indFragmentVar < shaderSource->nb_fragmentUniformVars ; 
       indFragmentVar++ ) {
    uniformFragmentParameters[indFragmentVar] 
      = glGetUniformLocation( shaderProgram, 
			      shaderSource->fragmentUniformVarName[ indFragmentVar ]);
    if (uniformFragmentParameters[indFragmentVar] == -1) {
      printf("Unable to retrieve fragment program parameter #%d %s, exiting...\n",indFragmentVar,shaderSource->fragmentUniformVarName[ indFragmentVar ]);
      cleanExit();
    }
  }

  uniformFontTextureIDParameter = -1;
  uniformMessageTextureIDParameter = -1;
  if( shaderSource->message_display && DM_EnvironmentNode->font_tex_no >= 0 
      && DM_EnvironmentNode->message_tex_no ) {
    uniformFontTextureIDParameter 
      = glGetUniformLocation( shaderProgram, "fs_lookupTable_font" );
    if (uniformFontTextureIDParameter == -1) {
      printf("Unable to retrieve font fragment program  fs_lookupTable_font in shader %s, exiting...\n" , id );
      cleanExit();
    }
    uniformMessageTextureIDParameter 
      = glGetUniformLocation( shaderProgram, "fs_lookupTable_message" );
    if (uniformMessageTextureIDParameter == -1) {
      printf("Unable to retrieve font fragment program  fs_lookupTable_message in shader %s, exiting...\n" , id );
      cleanExit();
    }
  }

  // printf( "end of fragment program bindings %s %s\n" , vertex_program_name , fragment_program_name );

  // no current shader, will be activated when necessary
  CurrentShader = NULL;
}

void dm_Shader::shader_enables( dm_MaterialProperties * mat ) {
  // dm_VertexShader                  *curVertShader = vertexShader;
  dm_GeometryShader                *curGeomShader = geometryShader;
  // dm_FragmentShader                *curFragShader = fragmentShader;

  CurrentShader = this;

  //////////////////////////////////////////////////
  // SHADER ACTIVATION
  //////////////////////////////////////////////////
  // printf( "\nUse shader program %d\n" , shaderProgram );
  glUseProgram( shaderProgram );

  //////////////////////////////////////////////////
  // GENERALIZED_SHADER SHADER PARAMETERS
  //////////////////////////////////////////////////
  // SETTING THE VERTEX PROGRAM MODEL VIEW & PROJ MATRIX PARAMETER
  glUniformMatrix4fv(projMatrixLoc,  1, false, matProj);
  glUniformMatrix4fv(viewMatrixLoc,  1, false, matView);
  if( timeParam != -1 ) {
    glUniform1f( timeParam , (float)FrameNo );
  }

  // if( shaderProgram == 3 ) {
  //   glUniform1i( glGetUniformLocation(shaderProgram, "fs_time"), FrameNo);
  // }
  // else {
  //   glUniform1i( glGetUniformLocation(shaderProgram, "fs_time"), 0 );
  // }


  // // cgGLSetStateMatrixParameter(cgGetNamedParameter(
  // // 						  curVertShader->vertexProgram, "ModelView"),
  // // 			      CG_GL_MODELVIEW_MATRIX,
  // // 			      CG_GL_MATRIX_IDENTITY);
  
  // GLint paramModelViewInverse = NULL_ID;
  // if( ( paramModelViewInverse 
  // 	= glGetUniformLocation(shaderProgram, "ModelViewInverse") ) != -1 ) {
  //   sprintf( ErrorStr , "Error: paramModelViewInverse not implemented [%s]!" , id ); ReportError( ErrorStr ); throw 281;
  //   // printf( "paramModelViewInverse storage\n" );
  //   // cgGLSetStateMatrixParameter( paramModelViewInverse ,
  //   // 				 CG_GL_MODELVIEW_MATRIX,
  //   // 				 CG_GL_MATRIX_INVERSE);
  // }
  
  // // setting the fragment program input parameters
  // // diffuse and specular light components taken
  // // from the material properties
  // if( diffuseParam != -1 && specularParam != -1 ) {
  //   if( mat ) {
  //     // printf( "diffuse %.6f %.6f %.6f %.6f\n" , mat->Diffuse[0], mat->Diffuse[1], mat->Diffuse[2], mat->Diffuse[3]);
  //     glUniform4fv( diffuseParam, 1, mat->Diffuse );
  //     glUniform4fv( specularParam, 1, mat->Specular );
  //   }
  //   else {
  //     // diffuse and specular coefficient
  //     float diffuse_coefficient[ 4 ] = {.7f, .7f, .7f, 1.0f};
  //     float specular_coefficient[ 4 ] = {1.0f , 1.0f , 1.0f, 1.0f };
      
  //     glUniform4fv(diffuseParam, 1, diffuse_coefficient );
  //     glUniform4fv(specularParam, 1, specular_coefficient );
  //   }
  // }
  
  if( shaderSource ) {
    for( int indVertexVar = 0 ; 
	 indVertexVar < shaderSource->nb_vertexUniformVars ; 
	 indVertexVar++ ) {
      // printf( "Currentparam %d %.3f %s\n" , indVertexVar ,
      // 	shaderSource->vertexUniformVarValue[ indVertexVar ],
      // 	shaderSource->vertexUniformVarName[ indVertexVar ]);
      GLint vp_param 
	= uniformVertexParameters[indVertexVar];
      dm_Table * tab
	= shaderSource->vertexUniformVarValue[indVertexVar];
      int size
	= shaderSource->vertexUniformVarSize[indVertexVar];
      if( size == 1 ) {
	glUniform1f( vp_param , 
		     tab->GetNumericalValue( 0 ) );
      }
      else if( size == 2 ) {
	glUniform2f( vp_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) );
      }
      else if( size == 3 ) {
	glUniform3f( vp_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) , 
		     tab->GetNumericalValue( 2 ) );
      }
      else if( size == 4 ) {
	glUniform4f( vp_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) , 
		     tab->GetNumericalValue( 2 ) , 
		     tab->GetNumericalValue( 3 ) );
      }
      else if( size == 9 ) {
	float tf[9] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8)};
	glUniformMatrix3fv( vp_param, 1, false, tf );
      }
      else if( size == 16 ) {
	// printf( "FS Arguments %s %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n\n", shaderSource->vertexUniformVarName[indVertexVar] , tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15) );
	float tf[16] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15)};
	glUniformMatrix4fv( vp_param,  1, false, tf );
      }
    }
    
    if (curGeomShader->geometryProgram != NULL_ID )
      for( int indGeometryVar = 0 ; 
	   indGeometryVar < shaderSource->nb_geometryUniformVars ; 
	   indGeometryVar++ ) {
	
	GLint gs_param 
	  = uniformGeometryParameters[indGeometryVar];
	dm_Table * tab
	  = shaderSource->geometryUniformVarValue[indGeometryVar];
	int size
	  = shaderSource->geometryUniformVarSize[indGeometryVar];
	if( size == 1 ) {
	  glUniform1f( gs_param , 
		       tab->GetNumericalValue( 0 ) );
	}
	else if( size == 2 ) {
	  glUniform2f( gs_param , 
		       tab->GetNumericalValue( 0 ) , 
		       tab->GetNumericalValue( 1 ) );
	}
	else if( size == 3 ) {
	  glUniform3f( gs_param , 
		       tab->GetNumericalValue( 0 ) , 
		       tab->GetNumericalValue( 1 ) , 
		       tab->GetNumericalValue( 2 ) );
	}
	else if( size == 4 ) {
	  glUniform4f( gs_param , 
		       tab->GetNumericalValue( 0 ) , 
		       tab->GetNumericalValue( 1 ) , 
		       tab->GetNumericalValue( 2 ) , 
		       tab->GetNumericalValue( 3 ) );
	}
	else if( size == 9 ) {
	  float tf[9] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8)};
	  glUniformMatrix3fv( gs_param, 1, false, tf );
	}
	else if( size == 16 ) {
	  // printf( "FS Arguments %s %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n\n", shaderSource->vertexUniformVarName[indVertexVar] , tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15) );
	  float tf[16] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15)};
	  glUniformMatrix4fv( gs_param, 1, false, tf );
	}
      }
    
    
    
    
    
    for( int indFragmentVar = 0 ; 
	 indFragmentVar < shaderSource->nb_fragmentUniformVars ; 
	 indFragmentVar++ ) {
      GLint fs_param 
	= uniformFragmentParameters[indFragmentVar];
      dm_Table * tab
	= shaderSource->fragmentUniformVarValue[indFragmentVar];
      int size
	= shaderSource->fragmentUniformVarSize[indFragmentVar];
      if( size == 1 ) {
	glUniform1f( fs_param , 
		     tab->GetNumericalValue( 0 ) );
      }
      else if( size == 2 ) {
	glUniform2f( fs_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) );
      }
      else if( size == 3 ) {
	// printf( "FS Arguments %s %.6f %.6f %.6f\n", shaderSource->fragmentUniformVarName[indFragmentVar] , tab->GetNumericalValue( 0 ) , tab->GetNumericalValue( 1 ) , tab->GetNumericalValue( 2 ) );
	glUniform3f( fs_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) , 
		     tab->GetNumericalValue( 2 ) );
      }
      else if( size == 4 ) {
	glUniform4f( fs_param , 
		     tab->GetNumericalValue( 0 ) , 
		     tab->GetNumericalValue( 1 ) , 
		     tab->GetNumericalValue( 2 ) , 
		     tab->GetNumericalValue( 3 ) );
      }
      else if( size == 9 ) {
	float tf[9] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8)};
	glUniformMatrix3fv( fs_param, 1, false, tf );
      }
      else if( size == 16 ) {
	// printf( "FS Arguments %s %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n", shaderSource->fragmentUniformVarName[indFragmentVar] , tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15) );
	float tf[16] = {tab->GetNumericalValue(0),tab->GetNumericalValue(1),tab->GetNumericalValue(2),tab->GetNumericalValue(3),tab->GetNumericalValue(4),tab->GetNumericalValue(5),tab->GetNumericalValue(6),tab->GetNumericalValue(7),tab->GetNumericalValue(8),tab->GetNumericalValue(9),tab->GetNumericalValue(10),tab->GetNumericalValue(11),tab->GetNumericalValue(12),tab->GetNumericalValue(13),tab->GetNumericalValue(14),tab->GetNumericalValue(15)};
	glUniformMatrix4fv( fs_param, 1, false, tf );
      }
    }
  }
  // GENERALIZED_SHADER SHADER PARAMETERS
  //////////////////////////////////////////////////

  bool invalidDL;
  invalidDL = false;

  // printf( "base shader texture mode\n" );

  //////////////////////////////////////////////////
  // pixel blending
  if( (*baseMaterialTexture) ) {
    (*baseMaterialTexture)->textureMode( &invalidDL );
  }

  // printf( "end of base shader texture mode\n" );
  //////////////////////////////////////////////////
  ////////// LOADING THE TEXTURES
  // 1: the base texture is loaded by textureMode that also defines blending

  // // 1: loading the base texture
  // // printf( "=> display base shader texture\n" );
  // // (*baseMaterialTexture)->print(1,1);
  // if( (*baseMaterialTexture)->GetTextureNo() >= 0 ) {
  //   if( DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ) == NULL_ID ) {
  //     // printf( "=> load base shader texture (%s) attachment %d nb attachment %d No [%d] ID [%d]\n" , DM_CompositionNode->texture_names[(*baseMaterialTexture)->GetTextureNo()] , (*baseMaterialTexture)->attachment + 1 , (*baseMaterialTexture)->nb_attachments , (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment) );
      
  //     // Interpolation defintition / Transfer to GPU + possible mipmaps
  //     DM_CompositionNode
  // 	->LoadTextureInGPUMemory( (*baseMaterialTexture)->GetTextureNo() , 
  // 				  (*baseMaterialTexture)->attachment , 
  // 				  (*baseMaterialTexture)->nb_attachments , 
  // 				  (*baseMaterialTexture)->encoding ,
  // 				  DM_BITMAP_TEXTURE ,
  // 				  (*baseMaterialTexture)->mipmapped , 
  // 				  (*baseMaterialTexture)->rectangle , 
  // 				  (*baseMaterialTexture)->texture_format , 
  // 				  (*baseMaterialTexture)->texture_min_filter , 
  // 				  (*baseMaterialTexture)->texture_max_filter , 
  // 				  (*baseMaterialTexture)->textureTarget );
  //     printf( "=> GPU load base shader texture (%s) No [%d] ID [%d]\n" , DM_CompositionNode->texture_names[(*baseMaterialTexture)->GetTextureNo()] , (*baseMaterialTexture)->GetTextureNo() , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ) );
  //   }
  //   else {
  //     // printf( "=> display base shader texture (%s) attachment %d nb attachment %d No [%d] ID [%d]\n" , DM_CompositionNode->texture_names[(*baseMaterialTexture)->GetTextureNo()] , (*baseMaterialTexture)->attachment + 1 , (*baseMaterialTexture)->nb_attachments , (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment, DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment) );
  //   }

  if( (*baseMaterialTexture)->GetTextureNo() >= 0 ) {
    // video texture 
#ifdef DM_HAVE_FFMPEG
    if( baseMaterialVideo && (*baseMaterialVideo) ) {
      if( (*baseMaterialVideo)->filmAudioVideo->newFrame ) {
        // printf( "Video Shader %s ID %d \n" , id , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ));

	// loads frame
	(*baseMaterialVideo)
	  ->LoadVideoFrameInGPUMemory( (*baseMaterialTexture)->GetTextureNo() , 
				       DM_FRAGMENT_TEXTURE ,
				       (*baseMaterialTexture)->rectangle );
      }
    }
#endif

    // (*baseMaterialTexture)->bindTexture( &invalidDL , false , 0 , 0 , 0 , 0 );
  }

  // 2: loading the shader active texture
  // Interpolation defintition / Transfer to GPU + possible mipmaps
  // multi-texture binding
  int textureRank = -1;
  for( textureRank = 0 ; textureRank < shaderSource->nb_shaderTextures ; 
       textureRank++ ) {
    dm_Texture *localTex = shaderSource->materialTextures[textureRank];

    if( DM_CompositionNode->GetTextureID( localTex->GetTextureNo() ) == NULL_ID ) {
      // printf( "=> load shader texture rank %d (%s) attachment %d nb attachment %d No [%d] ID [%d]\n" , textureRank + 1 , DM_CompositionNode->texture_names[localTex->GetTextureNo()] , localTex->attachment + 1 , localTex->nb_attachments ,localTex->GetTextureNo() + localTex->attachment , DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment) );
	
      // printf( "\nload base texture in memory \n" );
      // Interpolation defintition / Transfer to GPU + possible mipmaps
      DM_CompositionNode
	->LoadTextureInGPUMemory( localTex->GetTextureNo() , 
				  localTex->attachment , 
				  localTex->nb_attachments , 
				  localTex->encoding ,
				  DM_BITMAP_TEXTURE ,
				  localTex->mipmapped , 
				  localTex->rectangle , 
				  localTex->texture_format , 
				  localTex->texture_min_filter , 
				  localTex->texture_max_filter , 
				  localTex->textureTarget );
      // printf( "=> GPU load shader texture #%d (%s) No [%d] ID [%d]\n" , textureRank , DM_CompositionNode->texture_names[localTex->GetTextureNo()] , localTex->GetTextureNo() , DM_CompositionNode->GetTextureID( localTex->GetTextureNo() ) );
    }
    else {
      // printf( "=> display shader texture rank %d (%s) attachment %d nb attachment %d No [%d] ID [%d]\n" , textureRank + 1 , DM_CompositionNode->texture_names[localTex->GetTextureNo()] , localTex->attachment + 1 , localTex->nb_attachments ,localTex->GetTextureNo() + localTex->attachment, DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment) );
    }
      
    // video texture 
#ifdef DM_HAVE_FFMPEG
    if( shaderSource && shaderSource->materialVideo[ textureRank ]) {
      if( shaderSource->materialVideo[ textureRank ]
	  ->filmAudioVideo->newFrame ) {
	// printf( "Video Shader %s ID %d \n" , id , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ));
	  
	// loads frame
	shaderSource->materialVideo[ textureRank ]->
	  LoadVideoFrameInGPUMemory( localTex->GetTextureNo() , 
				     DM_FRAGMENT_TEXTURE ,
				     localTex->rectangle );
      }
    }
#endif
      
    // localTex->bindTexture( &invalidDL , false , 0 , 0 , 0 , 0 );
  }

  // printf( "end of load non base texture\n" );

  //////////////////////////////////////////////////

  //////////////////////////////////////////////////
  ////////// BINDING OF TEXTURE PARAMETERS
  // 1: enables the base texture parameter in the shader
  if( (*baseMaterialTexture)->GetTextureNo() >= 0 ) {
    // printf( "bind fragment shader base texture parameter ID [%d] rank 0, %s\n" , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ) , DM_CompositionNode->texture_names[(*baseMaterialTexture)->GetTextureNo()]);
    // printf( "bind fragment shader texture %d\n" , fragmentDecalParamLoc );
    glUniform1i( fragmentDecalParamLoc , 0); //Texture unit 0 is for base images.

    // texture parameter binding
    glActiveTexture(GL_TEXTURE0 + 0);
    if( (*baseMaterialTexture)->rectangle ) {
      glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment ) );
    }
    else {
      if( (*baseMaterialTexture)->textureTarget == GL_TEXTURE_2D ) {
	glBindTexture(GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment ) );
      }
      else if( (*baseMaterialTexture)->textureTarget == GL_TEXTURE_3D ) {
	glBindTexture(GL_TEXTURE_3D, DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() + (*baseMaterialTexture)->attachment ) );
      }
    }
    // glBindTexture(GL_TEXTURE_RECTANGLE, 2 );
    // if( shaderProgram == 3 ) {
    //   glBindTexture( GL_TEXTURE_RECTANGLE, 1);
    // }
    // else {
    //   glBindTexture( GL_TEXTURE_RECTANGLE, 2 );
    // }
  }
  else {
    printf( "no base shader texture parameter No %d ID [%d:%s]\n" , (*baseMaterialTexture)->GetTextureNo() , DM_CompositionNode->GetTextureID( (*baseMaterialTexture)->GetTextureNo() ) , DM_CompositionNode->texture_names[(*baseMaterialTexture)->GetTextureNo()]);
  }

  // 2: enables the active texture parameter in the shader
  // printf( "=> shader texture enables\n" );
  for( textureRank = 0 ; textureRank < shaderSource->nb_shaderTextures ; 
       textureRank++ ) {
    dm_Texture *localTex = shaderSource->materialTextures[textureRank];
    // texture parameter binding
    // printf( "bind shader texture parameter ind %d No %d ID [%d:%s] rank %d\n" , shaderSource->textureIndex[textureRank] , localTex->GetTextureNo() , DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment ) , DM_CompositionNode->texture_names[localTex->GetTextureNo() + localTex->attachment] , textureRank + 1);
    if( shaderSource->textureTarget[textureRank] == DM_VERTEX_TEXTURE ) {
      // printf( "bind vertex shader rank %d texture %d (%s)\n" , textureRank , fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ]] , vertex_program_name);
      glUniform1i( vertexLookupTableParam[ shaderSource->textureIndex[ textureRank ] ], 
		   textureRank + 1); //Texture unit 0 is for base images.
    }
    else if( shaderSource->textureTarget[textureRank] == DM_GEOMETRY_TEXTURE ) {
      glUniform1i( geometryLookupTableParam[ shaderSource->textureIndex[ textureRank ] ], 
		   textureRank + 1); //Texture unit 0 is for base images.
    }
    else if( shaderSource->textureTarget[textureRank] == DM_FRAGMENT_TEXTURE ) {
      // printf( "bind fragment shader rank %d texture %d (%s)\n" , textureRank , fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ]] , fragment_program_name);
      glUniform1i( fragmentLookupTableParam[ shaderSource->textureIndex[ textureRank ] ], 
		   textureRank + 1); //Texture unit 0 is for base images.
    }
    glActiveTexture(GL_TEXTURE0 + textureRank + 1 ); // decal has 0 for rank
    if( localTex->rectangle ) {
      glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment) );
      // printf( "bind texture rect ID %d attacht %d\n" , DM_CompositionNode->GetTextureID( localTex->GetTextureNo()) , localTex->attachment);
    }
    else {
      if( localTex->textureTarget == GL_TEXTURE_2D ) {
	glBindTexture(GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment) );
	// printf( "bind texture 2D ID %d attacht %d\n" , DM_CompositionNode->GetTextureID( localTex->GetTextureNo()) , localTex->attachment);
      }
      else if( localTex->textureTarget == GL_TEXTURE_3D ) {
	glBindTexture(GL_TEXTURE_3D, DM_CompositionNode->GetTextureID( localTex->GetTextureNo() + localTex->attachment) );
      }
       
    }
    // glBindTexture(GL_TEXTURE_RECTANGLE, textureRank + 1 );
  }

  // 3: enables font fragment shader parameters
  // passed only once
  if( !ScreenMessageInitialized && uniformFontTextureIDParameter != -1 && 
      uniformFontTextureIDParameter != -1 ) {
    // printf( "screen message initialization\n" );
    glUniform1i( uniformFontTextureIDParameter, 
		 shaderSource->nb_shaderTextures + 1); //Texture unit 0 is for base images.
    glActiveTexture(GL_TEXTURE0 + shaderSource->nb_shaderTextures + 1 ); // decal has 0 for rank
    glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( DM_EnvironmentNode->font_tex_no ) );

    // printf( "=> load shader texture rank %d (%s)  No [%d] ID [%d]\n" , shaderSource->nb_shaderTextures + 2 , DM_CompositionNode->texture_names[DM_EnvironmentNode->message_tex_no] ,DM_EnvironmentNode->message_tex_no , DM_CompositionNode->GetTextureID( DM_EnvironmentNode->message_tex_no) );
	
    glUniform1i( uniformMessageTextureIDParameter, 
		 shaderSource->nb_shaderTextures + 2); //Texture unit 0 is for base images.
    // glBindSampler(shaderSource->nb_shaderTextures + 2, sampler_nearest);
    glActiveTexture(GL_TEXTURE0 + shaderSource->nb_shaderTextures + 2 ); // decal has 0 for rank
    glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( DM_EnvironmentNode->message_tex_no ) );
    // ScreenMessageInitialized = true;

    // screen message decay
    if( messageTransparencyLoc != -1 ) {
      glUniform1f( messageTransparencyLoc , ScreenMessageColor );
    }
  }

}

void dm_Shader::update( void ) {
#ifdef DM_HAVE_FFMPEG
  if( shaderSource ) {
    for( int textureRank = 0 ; textureRank < shaderSource->nb_shaderTextures ; textureRank++ ) {
      if( shaderSource->materialVideo[ textureRank ] ) {
	// printf("Update shader video #%d (%s,%s) %s\n" , textureRank ,
	//  vertex_program_name , fragment_program_name ,
	//  shaderSource->materialVideo[ textureRank ]->fileName );
	shaderSource->materialVideo[ textureRank ]->update();
      }
    }
  }
#endif
}

void dm_Shader::shader_disables( void ) {
  // glDisable(GL_TEXTURE_RECTANGLE);
  glUseProgram( NULL_ID );
  CurrentShader = NULL;
}

void dm_Shader::print( FILE *file , int depth ) {
  indent( file , depth );
  fprintf( file , "<shader id=\"%s\">\n" , id );
  indent( file , depth + 1 );
  fprintf( file , "<vertexShader xlink:href=\"%s\"/>\n" , 
	   vertex_program_name );
  indent( file , depth + 1 );
  fprintf( file , "<fragmentShader xlink:href=\"%s\"/>\n" , 
	   fragment_program_name );

  if( shaderSource ) {
    shaderSource->print( file , true , depth + 1 );
  }

  indent( file , depth );
  fprintf( file , "</shader>\n" );
}

void dm_Shader::operator=(dm_Shader& p) {
  vertex_program_name = p.vertex_program_name;
  geometry_program_name = p.geometry_program_name;
  geometry_program_output_type = p.geometry_program_output_type;
  geometry_max_vertex  = p.geometry_max_vertex;
  fragment_program_name = p.fragment_program_name;
  *vertexShader = *(p.vertexShader);
  *fragmentShader = *(p.fragmentShader);
  *geometryShader = *(p.geometryShader);
  baseMaterialTexture = p.baseMaterialTexture;
#ifdef DM_HAVE_FFMPEG
  baseMaterialVideo = p.baseMaterialVideo;
#endif
  
  if( p.shaderSource ) {
    *shaderSource = *(p.shaderSource);
  }
}

