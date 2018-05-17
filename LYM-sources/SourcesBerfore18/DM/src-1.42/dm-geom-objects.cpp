/*! \file dm-geom-objects.cpp
 *
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
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


const char *StorageTypeString[EmptyStorageType + 1] = { "none" , "texture" , "renderbuffer" , "" };

const char *StripTypeString[EmptyStripType + 1] = { "triangle" , "line" , "quad" };

dm_point3 *Origin = new dm_point3( 0 , 0 , 0 );
dm_vector3 *NullVector = new dm_vector3( 0 , 0 , 0 );


dm_SceneObject::dm_SceneObject( void ) {
  // save the new scene object into the header part of AllSceneObject for later removal
  strAllSceneObject* newObject = new strAllSceneObject;
  newObject->curObj = this;
  newObject->nextObj = AllSceneObject->nextObj;
  AllSceneObject->nextObj = newObject;
  
  // common initialization for all scene objects
  id = NULL;
  InitString( &id );
}

dm_SceneObject::~dm_SceneObject( void ) {
  // printf("delete scene object\n" );
  if (id)  {
    // printf("delete ID (%s)\n" , id );
    delete []id;
    id = NULL;
  }
}

char *dm_SceneObject::GetId( void ) {
  return id;
}

void dm_SceneObject::SetId( const char *newId ) {
  StrCpy( &id , newId );
}

void dm_SceneObject::bindVertexData( void ) {
  sprintf( ErrorStr , "Error: bindVertexData not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_SceneObject::display( DrawingMode mode , bool *invalidDL ) {
  sprintf( ErrorStr , "Error: display not implemented for dm_SceneObject %s!" , id ); ReportError( ErrorStr ); throw 53;
}

bool dm_SceneObject::IsGeometricalObject( void ) {
  return false;
}

void dm_SceneObject::NewvertexArrayID( void ){
  sprintf( ErrorStr , "Error: NewvertexArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::NewelementArrayID( void ){
  sprintf( ErrorStr , "Error: NewelementArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::NewvertexBufferObjectID( void ){
  sprintf( ErrorStr , "Error: NewvertexBufferObjectID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
GLuint dm_SceneObject::GetvertexArrayID( void ){
  sprintf( ErrorStr , "Error: GetvertexArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
GLuint dm_SceneObject::GetelementArrayID( void ){
  sprintf( ErrorStr , "Error: GetelementArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
GLuint dm_SceneObject::GetvertexBufferObjectID( void ){
  sprintf( ErrorStr , "Error: GetvertexBufferObjectID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::SetvertexArrayID( GLuint alistID ){
  sprintf( ErrorStr , "Error: SetvertexArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::SetelementArrayID( GLuint alistID ){
  sprintf( ErrorStr , "Error: SetelementArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::SetvertexBufferObjectID( GLuint alistID ){
  sprintf( ErrorStr , "Error: SetvertexBufferObjectID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_SceneObject::DeletevertexArrayID( void ){
  sprintf( ErrorStr , "Error: DeletevertexArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::DeleteelementArrayID( void ){
  sprintf( ErrorStr , "Error: DeleteelementArrayID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}
void dm_SceneObject::DeletevertexBufferObjectID( void ){
  sprintf( ErrorStr , "Error: DeletevertexBufferObjectID not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

dm_Shader * dm_SceneObject::GetMaterialShader( void ) {
  sprintf( ErrorStr , "Error: GetMaterialShader not implemented for dm_SceneObject [%s]!" , id ); ReportError( ErrorStr ); throw 53;
}
dm_MaterialProperties *dm_SceneObject::GetMaterial( void ) {
  sprintf( ErrorStr , "Error: GetMaterial not implemented for dm_SceneObject [%s]!" , id ); ReportError( ErrorStr ); throw 53;
}

void dm_SceneObject::print( FILE *file , int isLong , int depth ) {
  sprintf( ErrorStr , "Error: print not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_SceneObject::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				    FloatBinOp oper ,
				    IntBinOp operI ) {
  sprintf( ErrorStr , "Error: setParameters not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

bool dm_SceneObject::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  sprintf( ErrorStr , "Error: getParameters not implemented for dm_SceneObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_SceneObject::operator=(dm_SceneObject& h) {
  StrCpy( &id , h.id );
}

///////////////////////////////////////////////////////////
// DISPLAYABLE GEOMETRICAL OBJECTS
// ANCESTOR CLASS OF GEOMETRIC OBJECTS
///////////////////////////////////////////////////////////

dm_GeometricalObject::dm_GeometricalObject( void ): dm_SceneObject() {
  material = new dm_MaterialProperties();

  vertexArrayID = NULL_ID;
  elementArrayID = NULL_ID;
  vertexBufferObjectID = NULL_ID;
}

dm_GeometricalObject::~dm_GeometricalObject( void ) {
  if (material) {
    delete material;
    material = NULL;
  }
}


// ---------- vertex array ----------------- //

void dm_GeometricalObject::NewvertexArrayID( void ) {
  if( vertexArrayID == NULL_ID ) {
    glGenVertexArrays( 1 , &vertexArrayID );
    // printf( "Affect ID %d Object %s\n" , vertexArrayID , id  );
  }
}

void dm_GeometricalObject::NewelementArrayID( void ) {
  if( elementArrayID == NULL_ID ) {
    glGenBuffers( 1 , &elementArrayID );
    // printf( "Affect ID %d Object %s\n" , elementArrayID , id  );
  }
}

void dm_GeometricalObject::NewvertexBufferObjectID( void ) {
  if( vertexBufferObjectID == NULL_ID ) {
    glGenBuffers( 1 , &vertexBufferObjectID );
    // printf( "Affect ID %d Object %s\n" , vertexBufferObjectID , id  );
  }
}

GLuint dm_GeometricalObject::GetvertexArrayID( void ) {
  // printf( "Get ID %d Object %s\n" , vertexArrayID , id  );
  return vertexArrayID;
}

GLuint dm_GeometricalObject::GetelementArrayID( void ) {
  // printf( "Get ID %d Object %s\n" ,elementArrayID , id  );
  return elementArrayID;
}

GLuint dm_GeometricalObject::GetvertexBufferObjectID( void ) {
  // printf( "Get ID %d Object %s\n" ,vertexBufferObjectID , id  );
  return vertexBufferObjectID;
}

void dm_GeometricalObject::SetvertexArrayID( GLuint avertexArrayID ) {
  vertexArrayID = avertexArrayID;
}
void dm_GeometricalObject::SetelementArrayID( GLuint aelementArrayID ) {
  elementArrayID = aelementArrayID;
}
void dm_GeometricalObject::SetvertexBufferObjectID( GLuint avertexBufferObjectID ) {
  vertexBufferObjectID = avertexBufferObjectID;
}

// ---------- display lists only ----------------- //

void dm_GeometricalObject::DeletevertexArrayID( void ) {
  // printf( "Delete ID %d Object %s\n" , vertexArrayID , id  );
  if( vertexArrayID != NULL_ID )
    glDeleteVertexArrays( 1 , &vertexArrayID );
  vertexArrayID = NULL_ID;
  // printf( "Delete ID %d Object %s\n" , vertexArrayID , id  );
}

void dm_GeometricalObject::DeleteelementArrayID( void ) {
  // printf( "Delete ID %d Object %s\n" , elementArrayID , id  );
  if( elementArrayID != NULL_ID )
    glDeleteBuffers( 1 , &elementArrayID );
  elementArrayID = NULL_ID;
}

void dm_GeometricalObject::DeletevertexBufferObjectID( void ) {
  // printf( "Delete ID %d Object %s\n" ,vertexBufferObjectID , id  );
  if( vertexBufferObjectID != NULL_ID )
    glDeleteBuffers( 1 , &vertexBufferObjectID );
  vertexBufferObjectID = NULL_ID;
}

dm_Shader * dm_GeometricalObject::GetMaterialShader( void ) {
  return material->shader;
}

dm_MaterialProperties *dm_GeometricalObject::GetMaterial( void ) {
  return material;
}


void dm_GeometricalObject::display( DrawingMode mode , bool *invalidDL ) {
  sprintf( ErrorStr , "Error: display not implemented for dm_GeometricalObject %s!" , id ); ReportError( ErrorStr ); throw 53;
}


void dm_GeometricalObject::bindVertexData( void ) {
  sprintf( ErrorStr , "Error: bindVertexData not implemented for dm_GeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}
 
bool dm_GeometricalObject::IsGeometricalObject( void ) {
  return true;
}

void dm_GeometricalObject::print( FILE *file , int isLong , int depth ) {
  sprintf( ErrorStr , "Error: print not implemented for dm_GeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_GeometricalObject::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
					  FloatBinOp oper ,
					  IntBinOp operI ) {
  sprintf( ErrorStr , "Error: setParameters not implemented for dm_GeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

bool dm_GeometricalObject::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  sprintf( ErrorStr , "Error: getParameters not implemented for dm_GeometricalObject!" ); ReportError( ErrorStr ); throw 53;
}

void dm_GeometricalObject::operator=(dm_GeometricalObject& h) {
  this->dm_SceneObject::operator=(h);

  if (h.material) {
    if (!material) {
      material = new dm_MaterialProperties;
    }
    *material = *(h.material);
  }

  // because it corresponds to a pointer in a table
  // otherwise all the copied objects must have the
  // same dipslay list ID
  vertexArrayID = h.vertexArrayID;
  elementArrayID = h.elementArrayID;
  vertexBufferObjectID = h.vertexBufferObjectID;
}


////////////////////////////////////////////////////////////////////
// MESH
////////////////////////////////////////////////////////////////////

dm_Mesh::dm_Mesh( void ):dm_GeometricalObject() {
  nb_normals = 0;
  nb_vertices = 0;
  nb_fullvertices = 0;
  nb_faces = 0;
  nb_texturecoords = 0;
  
  pvertex = NULL;
  pFullvertex = NULL;
  pnormal = NULL;
  ptexturecoord = NULL;
  pface_vertex = NULL;
  pface_normal = NULL;
  pface_texturecoord = NULL;

  leadingComponent = EmptyMeshLeadingComponent;

  center = *Origin;
  averageNormal = *NullVector;
}

dm_Mesh::~dm_Mesh( void ) {
  if (pvertex) {
    delete [] pvertex;
    pvertex = NULL;
  }
  if (pFullvertex) {
    delete [] pFullvertex;
    pFullvertex = NULL;
  }
  if (pnormal) {
    delete [] pnormal;
    pnormal = NULL;
  }
  if (ptexturecoord) {
    delete [] ptexturecoord;
    ptexturecoord = NULL;
  }
  if (pface_vertex) {
    delete [] pface_vertex;
    pface_vertex = NULL;
  }
  if (pface_normal) {
    delete [] pface_normal;
    pface_normal = NULL;
  }
  if (pface_texturecoord) {
    delete [] pface_texturecoord;
    pface_texturecoord = NULL;
  }
}

void dm_Mesh::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    nb_normals = 0;
  }
  if( XMLtag->hasAttribute( "nb_normals" ) ) {
    operI( nb_normals , XMLtag->getAttributeIntValue( "nb_normals" ,
						      atRunTime ));
  }
  if( !atRunTime ) {
    nb_vertices = 0;
  }
  if( XMLtag->hasAttribute( "nb_vertices" ) ) {
    operI( nb_vertices , XMLtag->getAttributeIntValue( "nb_vertices" ,
						       atRunTime ));
  }
  if( !atRunTime ) {
    nb_faces = 0;
  }
  if( XMLtag->hasAttribute( "nb_faces" ) ) {
    operI( nb_faces , XMLtag->getAttributeIntValue( "nb_faces" ,
						    atRunTime ));
  }
  if( !atRunTime ) {
    nb_texturecoords = 0;
  }
  if( XMLtag->hasAttribute( "nb_texturecoords" ) ) {
    operI( nb_texturecoords , XMLtag->getAttributeIntValue( "nb_texturecoords" ,
							    atRunTime ));
  }
  if( !atRunTime ) {

    if( pvertex ) {
      sprintf( ErrorStr , "Error: normal table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pvertex = new MeshVertex[ nb_vertices ];
    if( !pvertex ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      pvertex[ indVertex ].x = 0.0;
      pvertex[ indVertex ].y = 0.0;
      pvertex[ indVertex ].z = 0.0;
    }

    if( pnormal ) {
      sprintf( ErrorStr , "Error: normal table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pnormal = new MeshNormal[ nb_normals ];
    if( !pnormal ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indNormal = 0 ; indNormal < nb_normals ; indNormal++ ) {
      pnormal[ indNormal ].nx = 0.0;
      pnormal[ indNormal ].ny = 0.0;
      pnormal[ indNormal ].nz = 0.0;
    }

    if( ptexturecoord ) {
      sprintf( ErrorStr , "Error: texturecoord table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    ptexturecoord = new MeshTexturecoord[ nb_texturecoords ];
    if( !ptexturecoord ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indTexturecoord = 0 ; indTexturecoord < nb_texturecoords ; indTexturecoord++ ) {
      ptexturecoord[ indTexturecoord ].s0 = 0.0;
      ptexturecoord[ indTexturecoord ].t0 = 0.0;
      ptexturecoord[ indTexturecoord ].u0 = 0.0;
    }

    if( pface_vertex ) {
      sprintf( ErrorStr , "Error: face_vertex table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pface_vertex = new MeshFace[ nb_faces * 3 ];
    if( !pface_vertex ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    if( pface_normal ) {
      sprintf( ErrorStr , "Error: face_normal table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pface_normal = new MeshFace[ nb_faces * 3 ];
    if( !pface_normal ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    if( pface_texturecoord ) {
      sprintf( ErrorStr , "Error: face table_texturecoord already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pface_texturecoord = new MeshFace[ nb_faces * 3 ];
    if( !pface_texturecoord ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indFace = 0 ; indFace < nb_faces * 3 ; indFace++ ) {
      pface_vertex[ indFace ].ind = -1;
      pface_normal[ indFace ].ind = -1;
      pface_texturecoord[ indFace ].ind = -1;
    }

    // nb_fullvertices = _Max( nb_vertices , _Max( nb_normals , nb_texturecoords ) );
    // if( nb_fullvertices == nb_vertices ) {
    //   leadingComponent = VertexMeshLeadingComponent;
    // }
    // else if( nb_fullvertices == nb_texturecoords ) {
    //   leadingComponent = TextureCoordMeshLeadingComponent;
    // }
    // else if( nb_fullvertices == nb_normals ) {
    //   leadingComponent = NormalMeshLeadingComponent;
    // }
    nb_fullvertices = nb_faces * 3;
    if( pFullvertex ) {
      sprintf( ErrorStr , "Error: vertex table already allocated in mesh ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pFullvertex = new MeshFullVertex[ nb_fullvertices ];
    if( !pFullvertex ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nb_fullvertices ; indVertex++ ) {
      pFullvertex[ indVertex ].X = 0.0;
      pFullvertex[ indVertex ].Y = 0.0;
      pFullvertex[ indVertex ].Z = 0.0;
      pFullvertex[ indVertex ].NX = 0.0;
      pFullvertex[ indVertex ].NY = 0.0;
      pFullvertex[ indVertex ].NZ = 0.0;
      pFullvertex[ indVertex ].S0 = 0.0;
      pFullvertex[ indVertex ].T0 = 0.0;
      pFullvertex[ indVertex ].U0 = 0.0;
      pFullvertex[ indVertex ].R = 1.0;
      pFullvertex[ indVertex ].G = 1.0;
      pFullvertex[ indVertex ].B = 1.0;
      pFullvertex[ indVertex ].A = 1.0;
    }
  }

  update();
}

bool dm_Mesh::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "nb_normals" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_normals;
    return true;
  }
  else if( strcmp( attribute , "nb_vertices" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_vertices;
    return true;
  }
  else if( strcmp( attribute , "nb_faces" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_faces;
    return true;
  }
  else if( strcmp( attribute , "nb_texturecoords" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_texturecoords;
    return true;
  }
  return false;
}

void dm_Mesh::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  while( true ) {
    // new tag XML : mesh component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // mesh closing tag
    if( strcmp( XMLtag->tag , "mesh" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
      XMLtag = NULL;
      
      // copies the normals and texturecoords data inside the vertex
      // according to the pointers
      for( int indFullVertices = 0 ;  indFullVertices < nb_fullvertices ; indFullVertices++ ) { 
	pFullvertex[ indFullVertices ].X 
	  = pvertex[ pface_vertex[ indFullVertices ].ind ].x;
	pFullvertex[ indFullVertices ].Y 
	  = pvertex[ pface_vertex[ indFullVertices ].ind ].y;
	pFullvertex[ indFullVertices ].Z
	  = pvertex[ pface_vertex[ indFullVertices ].ind ].z;
	
	pFullvertex[ indFullVertices ].NX 
	  = pnormal[ pface_normal[ indFullVertices ].ind ].nx;
	pFullvertex[ indFullVertices ].NY 
	  = pnormal[ pface_normal[ indFullVertices ].ind ].ny;
	pFullvertex[ indFullVertices ].NZ
	  = pnormal[ pface_normal[ indFullVertices ].ind ].nz;
	
	pFullvertex[ indFullVertices ].S0 
	  = ptexturecoord[ pface_texturecoord[ indFullVertices ].ind ].s0;
	pFullvertex[ indFullVertices ].T0 
	  = ptexturecoord[ pface_texturecoord[ indFullVertices ].ind ].t0;
	pFullvertex[ indFullVertices ].U0
	  = ptexturecoord[ pface_texturecoord[ indFullVertices ].ind ].u0;
      }
      for( int indFace = 0 ;  indFace < nb_faces * 3 ; indFace++ ) { 
	pface_vertex[ indFace ].ind = indFace;
	pface_normal[ indFace ].ind = indFace;
	pface_texturecoord[ indFace ].ind = indFace;
      }

      return;
    }

    // color
    else if( ( strcmp( XMLtag->tag , "color" ) == 0  
	       || strcmp( XMLtag->tag , "material" ) == 0  
	       || strcmp( XMLtag->tag , "materialRaytrace" ) == 0  
	       || strcmp( XMLtag->tag , "shader" ) == 0  
	       || strcmp( XMLtag->tag , "texture" ) == 0   
	       || strcmp( XMLtag->tag , "video" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
      material->parse( fileDepth , p_c , id ,
		       XMLtag , NULL );
    }

    // subdivision initial vertices
    else if( strcmp( XMLtag->tag , "tabvertex" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabvertex\n");

      // parsing the coordinates of the initial vertices
      ParseTabVertex( fileDepth , p_c , pvertex ,  nb_vertices );
    }

    // subdivision initial texture coordinates
    else if( strcmp( XMLtag->tag , "tabtextureCoord" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabtextureCoord\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabTextureCoord( fileDepth , p_c , ptexturecoord , nb_texturecoords );
    }

    // subdivision initial normals
    else if( strcmp( XMLtag->tag , "tabnormal" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabnormal\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabNormal( fileDepth , p_c , pnormal , nb_normals , true );
    }

    // subdivision initial faces
    else if( strcmp( XMLtag->tag , "tabface" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabface\n");

      // parsing the indices of the vertices in the faces
      ParseTabFace( fileDepth , p_c , 
		    pface_vertex , pface_normal , pface_texturecoord , 
		    nb_faces , nb_vertices , nb_texturecoords ,
		    nb_normals ,
		    0 
// 		    , numberOfInitialColors 
		    );
    }

    else {
      sprintf( ErrorStr , "Error: unknown mesh component tag [%s] in mesh ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Mesh::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<mesh id=\"%s\" nb_normals=\"%d\" nb_vertex=\"%d\" nb_faces=\"%d\" nb_texturecoords=\"%d\"  >\n" , id , nb_normals, nb_fullvertices, nb_faces, nb_texturecoords  );

  if( isLong ) {
    material->print( file , depth + 1 );


    indent( file , depth + 1 );
    fprintf( file , "<tabvertex size=\"%d\">\n" ,  nb_fullvertices );
    indent( file , depth + 2 );
    fprintf( file , "<vertexCoordinate point=\"" );
    for( int indVertex = 0 ; indVertex < nb_fullvertices ; indVertex++ ) {
      fprintf( file , "%.6f %.6f %.6f " , pFullvertex[indVertex].X , pFullvertex[indVertex].Y , 
	       pFullvertex[indVertex].Z );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabvertex>\n\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabtextureCoord size=\"%d\">\n" ,  nb_fullvertices );
    indent( file , depth + 2 );
    fprintf( file , "<textureCoordinate point=\"" );
    for( int indVertex = 0 ; indVertex < nb_fullvertices ; indVertex++ ) {
      fprintf( file , "%.6f %.6f " , pFullvertex[indVertex].S0 , pFullvertex[indVertex].T0 );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabtextureCoord>\n\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabnormal size=\"%d\">\n" ,  nb_fullvertices );
    indent( file , depth + 2 );
    fprintf( file , "<normalCoordinate vector=\"" );
    for( int indVertex = 0 ; indVertex < nb_fullvertices ; indVertex++ ) {
      fprintf( file , "%.6f %.6f %.6f " , pFullvertex[indVertex].NX , pFullvertex[indVertex].NY , 
	       pFullvertex[indVertex].NZ );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabnormal>\n\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabface size=\"%d\">\n" ,  nb_faces );
    indent( file , depth + 2 );
    fprintf( file , "<normalIndex index=\"" );
    for( int indFaces = 0 ; indFaces < nb_faces * 3 ; indFaces++ ) {
      fprintf( file , "%d " , pface_vertex[indFaces].ind + 1 );
    }
    fprintf( file , "\" />\n" );

    indent( file , depth + 2 );
    fprintf( file , "<vertexIndex index=\"" );
    for( int indFaces = 0 ; indFaces < nb_faces * 3 ; indFaces++ ) {
      fprintf( file , "%d " , pface_vertex[indFaces].ind + 1 );
    }
    fprintf( file , "\" />\n" );

    indent( file , depth + 2 );
    fprintf( file , "<textureCoordinateIndex index=\"" );
    for( int indFaces = 0 ; indFaces < nb_faces * 3 ; indFaces++ ) {
      fprintf( file , "%d " , pface_vertex[indFaces].ind + 1 );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabface>\n\n" );

  }
  indent( file , depth );
  fprintf( file , "</mesh>\n" );
}

void dm_Mesh::bindVertexData( void ) {
  // no shader => no rendering
  if( !material || !material->shader ) {
    // sprintf( ErrorStr , "Warning: missing material or shader on compiled mesh ID [%s]!"  , id ); ReportError( ErrorStr ); 
    return;
  }

  // primitive drawing
  if( GetvertexArrayID() != NULL_ID ) {
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    // printf( "Geometrical object already has a Display List!\n" );
    //       sprintf( ErrorStr , "Error: Geometrical object already has a Display List!" ); ReportError( ErrorStr ); throw 139;
  }
  
  // Initializes vertex shader data (former display lists or VBOs)
  NewvertexArrayID();
  // printf( "New NewvertexArrayID %s ID %d\n" , id , GetvertexArrayID() );
  NewelementArrayID();
  // printf( "New NewelementArrayID %s ID %d\n" , id , GetelementArrayID() );
  NewvertexBufferObjectID();
  // printf( "New NewvertexBufferObjectID %s ID %d\n" , id , GetvertexBufferObjectID() );
  
  ////////////////////////
  // Ondemand shader initialization
  // initializes the shader if it is the first rendering
  if( material->shader->shaderProgram == NULL_ID ) {
    // printf("Initialize shader\n");
    material->shader->InitializeShader();
  }

  //  glUseProgram( material->shader->shaderProgram );

  // printf("points %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f \n", pFullvertex[0].x , pFullvertex[0].y , pFullvertex[0].z , pFullvertex[1].x , pFullvertex[1].y , pFullvertex[1].z , pFullvertex[2].x , pFullvertex[2].y , pFullvertex[2].z , pFullvertex[3].x , pFullvertex[3].y , pFullvertex[3].z );
  
  ////////////////////////
  // Generate a vertex array object VAO for 2 triangles
  glBindVertexArray( GetvertexArrayID() );

  // Generate an element array buffer EAO and copy data into it
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshFace) * (nb_faces * 3), pface_vertex, GL_STATIC_DRAW);
  // printf("size of faces %d\n", sizeof(pface_vertex));

  // Generate vertex buffer object VBO and copy data into the buffer
  glBindBuffer(GL_ARRAY_BUFFER, GetvertexBufferObjectID() );
  glBufferData(GL_ARRAY_BUFFER, sizeof(MeshFullVertex) * nb_fullvertices, pFullvertex, GL_STATIC_DRAW);
  // printf("size of vertices %d\n", sizeof(pFullvertex));

  // Specify the layout of the vertex data
  GLuint shaderProgram = material->shader->shaderProgram;
  // positions
  GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );  
  glEnableVertexAttribArray(posAttrib);    //We like submitting vertices on stream 0 for no special reason
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(0));   //The starting point of the vertex array, for the vertices
  // printf("size of one vertex %d\n", sizeof(MeshFullVertex));

  // normals
  GLint normAttrib = glGetAttribLocation( shaderProgram, "normal" );
  glEnableVertexAttribArray( normAttrib );
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(3*sizeof(float)));     //The starting point of normals, 12 bytes away

  // textures
  GLint textAttrib = glGetAttribLocation( shaderProgram, "texcoord" );
  glEnableVertexAttribArray( textAttrib );
  glVertexAttribPointer(textAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(6*sizeof(float)));   //The starting point of texcoords, 24 bytes away

  // colors
  GLint colAttrib = glGetAttribLocation( shaderProgram, "color" );
  glEnableVertexAttribArray( colAttrib );
  glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(9*sizeof(float)));   //The starting point of texcoords, 24 bytes away

  // printf("end of bind vertex data %s\n", id );
}

void dm_Mesh::display( DrawingMode mode , bool *invalidDL ) {
  // if the texture of the object has been loaded in GPU during drawing
  // the display list cannot be used otherwise the texture would be
  // reloaded at each DL ID call
  if( GetvertexArrayID() == NULL_ID || *invalidDL ) {
    // printf( "Invalid DL bind vertex data %d\n" , *invalidDL );
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    bindVertexData();
  }
  // printf( "Display %s GetvertexArrayID %d GetelementArrayID %d shaderProgram %d \n" , id,  GetvertexArrayID(), GetelementArrayID() , material->getshaderProgram()  );

  // no shader => no rendering
  if( !material || !material->shader ) {
    return;
  }

  // printf( "Enable material\n" );
  material->shader->shader_enables( material );
  // glActiveTexture(GL_TEXTURE0 + 0);

  // printf( "Draw primitive\n" );
  glBindVertexArray( GetvertexArrayID() );
  // printf("Mesh GetvertexArrayID %d\n", GetvertexArrayID() );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glDrawElements( GL_TRIANGLES, nb_faces * 3 , GL_UNSIGNED_INT, 0 );
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  

  // printf( "End of drawing\n" );
  // material->shader->shader_disables();

  // glActiveTexture(GL_TEXTURE0 + 0);
}

void dm_Mesh::averageNormals( dm_vector3 *normal ) {
  dm_vector3 normals[4];

  dm_point3 ptaux1 , ptaux2;
  ptaux1.x =pFullvertex[0].X;
  ptaux1.y =pFullvertex[0].Y;
  ptaux1.z =pFullvertex[0].Z;
  ptaux2.x =pFullvertex[1].X;
  ptaux2.y =pFullvertex[1].Y;
  ptaux2.z =pFullvertex[1].Z;
  normals[0].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[1].X;
  ptaux1.y =pFullvertex[1].Y;
  ptaux1.z =pFullvertex[1].Z;
  ptaux2.x =pFullvertex[2].X;
  ptaux2.y =pFullvertex[2].Y;
  ptaux2.z =pFullvertex[2].Z;
  normals[1].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[2].X;
  ptaux1.y =pFullvertex[2].Y;
  ptaux1.z =pFullvertex[2].Z;
  ptaux2.x =pFullvertex[3].X;
  ptaux2.y =pFullvertex[3].Y;
  ptaux2.z =pFullvertex[3].Z;
  normals[2].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[3].X;
  ptaux1.y =pFullvertex[3].Y;
  ptaux1.z =pFullvertex[3].Z;
  ptaux2.x =pFullvertex[0].X;
  ptaux2.y =pFullvertex[0].Y;
  ptaux2.z =pFullvertex[0].Z;
  normals[3].prodVect( center , ptaux1 , ptaux2 );

  averageNormal.averageVectors( normals , 4 );
  averageNormal *= -1;
  averageNormal.normalize();
}

void dm_Mesh::update( dm_point3 *tabPoint ) {
  // for( int ind  = 0 ; ind < 4 ; ind++ ) {
  //   points[ind] = tabPoint[ind];
  // }
  update();
}

void dm_Mesh::update( void ) {
  center.x = 0.25F * (pFullvertex[0].X + pFullvertex[1].X + pFullvertex[2].X + pFullvertex[3].X);
  center.y = 0.25F * (pFullvertex[0].Y + pFullvertex[1].Y + pFullvertex[2].Y + pFullvertex[3].Y);
  center.z = 0.25F * (pFullvertex[0].Z + pFullvertex[1].Z + pFullvertex[2].Z + pFullvertex[3].Z);
  averageNormals( &averageNormal );
}

void dm_Mesh::operator=(dm_Mesh& p) {
  this->dm_GeometricalObject::operator=(p);

  nb_fullvertices = p.nb_fullvertices;
  if( pFullvertex ) {
    delete [] pFullvertex;
    pFullvertex = NULL;
  }
  if( p.pFullvertex ) {
    pFullvertex = new MeshFullVertex[ nb_fullvertices ];
    for( int ind  = 0 ; ind < nb_fullvertices ; ind++ ) {
      pFullvertex[ind].X = p.pFullvertex[ind].X;
      pFullvertex[ind].Y = p.pFullvertex[ind].Y;
      pFullvertex[ind].Z = p.pFullvertex[ind].Z;
      pFullvertex[ind].NX = p.pFullvertex[ind].NX;
      pFullvertex[ind].NY = p.pFullvertex[ind].NY;
      pFullvertex[ind].NZ = p.pFullvertex[ind].NZ;
      pFullvertex[ind].S0 = p.pFullvertex[ind].S0;
      pFullvertex[ind].T0 = p.pFullvertex[ind].T0;
      pFullvertex[ind].U0 = p.pFullvertex[ind].U0;
      pFullvertex[ind].R = p.pFullvertex[ind].R;
      pFullvertex[ind].G = p.pFullvertex[ind].G;
      pFullvertex[ind].B = p.pFullvertex[ind].B;
      pFullvertex[ind].A = p.pFullvertex[ind].A;
    }
  }

  nb_faces = p.nb_faces;
  nb_normals = p.nb_normals;
  nb_texturecoords = p.nb_texturecoords;
  nb_vertices = p.nb_vertices;
  if( pface_vertex ) {
    delete [] pface_vertex;
    pface_vertex = NULL;
  }
  if( pface_normal ) {
    delete [] pface_normal;
    pface_normal = NULL;
  }
  if( pface_texturecoord ) {
    delete [] pface_texturecoord;
    pface_texturecoord = NULL;
  }
  if( p.pface_vertex ) {
    pface_vertex = new MeshFace[ nb_faces * 3 ];
    pface_normal = new MeshFace[ nb_faces * 3 ];
    pface_texturecoord = new MeshFace[ nb_faces * 3 ];
    for( int ind  = 0 ; ind < nb_faces * 3 ; ind++ ) {
      pface_vertex[ind].ind = p.pface_vertex[ind].ind;
      pface_normal[ind].ind = p.pface_normal[ind].ind;
      pface_texturecoord[ind].ind = p.pface_texturecoord[ind].ind;
    }
  }
  if( pnormal ) {
    delete [] pnormal;
    pnormal = NULL;
  }
  if( p.pnormal ) {
    pnormal = new MeshNormal[ nb_normals ];
    for( int ind  = 0 ; ind < nb_normals ; ind++ ) {
      pnormal[ind].nx = p.pnormal[ind].nx;
      pnormal[ind].ny = p.pnormal[ind].ny;
      pnormal[ind].nz = p.pnormal[ind].nz;
    }
  }
  if( ptexturecoord ) {
    delete [] ptexturecoord;
    ptexturecoord = NULL;
  }
  if( p.ptexturecoord ) {
    ptexturecoord = new MeshTexturecoord[ nb_texturecoords ];
    for( int ind  = 0 ; ind < nb_texturecoords ; ind++ ) {
      ptexturecoord[ind].s0 = p.ptexturecoord[ind].s0;
      ptexturecoord[ind].t0 = p.ptexturecoord[ind].t0;
      ptexturecoord[ind].u0 = p.ptexturecoord[ind].u0;
    }
  }
  if( pvertex ) {
    delete [] pvertex;
    pvertex = NULL;
  }
  if( p.pvertex ) {
    pvertex = new MeshVertex[ nb_vertices ];
    for( int ind  = 0 ; ind < nb_vertices ; ind++ ) {
      pvertex[ind].x = p.pvertex[ind].x;
      pvertex[ind].y = p.pvertex[ind].y;
      pvertex[ind].z = p.pvertex[ind].z;
    }
  }

  center = p.center;
  averageNormal = p.averageNormal;
}


////////////////////////////////////////////////////////////////////
// POINTS
////////////////////////////////////////////////////////////////////

dm_Points::dm_Points( void ):dm_GeometricalObject() {
  nb_vertices = 0;

  width = 0;
  height = 0;

  pvertex = NULL;
  pFullvertex = NULL;
  ptexturecoord = NULL;
  pIndices = NULL;

  leadingComponent = EmptyPointsLeadingComponent;
}

dm_Points::~dm_Points( void ) {
  if (pvertex) {
    delete [] pvertex;
    pvertex = NULL;
  }
  if (pFullvertex) {
    delete [] pFullvertex;
    pFullvertex = NULL;
  }
  if (ptexturecoord) {
    delete [] ptexturecoord;
    ptexturecoord = NULL;
  }
}

void dm_Points::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    nb_vertices = 0;
  }
  if( XMLtag->hasAttribute( "nb_vertices" ) ) {
    operI( nb_vertices , XMLtag->getAttributeIntValue( "nb_vertices" ,
						       atRunTime ));
  }
  if( !atRunTime ) {
    width = 0;
  }
  if( XMLtag->hasAttribute( "width" ) ) {
    operI( width , XMLtag->getAttributeIntValue( "width" ,
						 atRunTime ));
  }
  if( !atRunTime ) {
    height = 0;
  }
  if( XMLtag->hasAttribute( "height" ) ) {
    operI( height , XMLtag->getAttributeIntValue( "height" ,
						  atRunTime ));
  }
  if( !atRunTime ) {

    if( pvertex ) {
      sprintf( ErrorStr , "Error: normal table already allocated in points ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pvertex = new MeshVertex[ nb_vertices ];
    if( !pvertex ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      pvertex[ indVertex ].x = (float)(indVertex * 30 % width);
      pvertex[ indVertex ].y = (float)(indVertex * 30 / width);
      pvertex[ indVertex ].z = 0.0;
    }

    if( ptexturecoord ) {
      sprintf( ErrorStr , "Error: texturecoord table already allocated in points ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    ptexturecoord = new MeshTexturecoord[ nb_vertices ];
    if( !ptexturecoord ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indTexturecoord = 0 ; indTexturecoord < nb_vertices ; indTexturecoord++ ) {
      ptexturecoord[ indTexturecoord ].s0 = (float)(indTexturecoord * 30 % width);
      ptexturecoord[ indTexturecoord ].t0 = (float)(indTexturecoord * 30 / width);
      ptexturecoord[ indTexturecoord ].u0 = 0.0;
    }

    if( pFullvertex ) {
      sprintf( ErrorStr , "Error: vertex table already allocated in points ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pFullvertex = new PointsFullVertex[ nb_vertices ];
    if( !pFullvertex ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      pFullvertex[ indVertex ].X = 0.0;
      pFullvertex[ indVertex ].Y = 0.0;
      pFullvertex[ indVertex ].Z = 0.0;
      pFullvertex[ indVertex ].S0 = 0.0;
      pFullvertex[ indVertex ].T0 = 0.0;
      pFullvertex[ indVertex ].U0 = 0.0;
    }
    // printf("width %d height %d\n",width,height);

    if( pIndices ) {
      sprintf( ErrorStr , "Error: index table already allocated in points ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    pIndices = new PointsIndex[ nb_vertices ];
    if( !pIndices ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      pIndices[ indVertex ].i = indVertex;
    }
  }

  update();
}

bool dm_Points::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "nb_vertices" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nb_vertices;
    return true;
  }
  else if( strcmp( attribute , "height" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = height;
    return true;
  }
  else if( strcmp( attribute , "width" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = width;
    return true;
  }
  return false;
}

void dm_Points::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  while( true ) {
    // new tag XML : points component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // points closing tag
    if( strcmp( XMLtag->tag , "points" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
      XMLtag = NULL;
      
      // copies the positions and texturecoords data inside the vertex
      // according to the pointers
      for( int indFullVertices = 0 ;  indFullVertices < nb_vertices ; indFullVertices++ ) { 
	pFullvertex[ indFullVertices ].X 
	  = pvertex[ indFullVertices ].x;
	pFullvertex[ indFullVertices ].Y 
	  = pvertex[ indFullVertices ].y;
	pFullvertex[ indFullVertices ].Z
	  = pvertex[ indFullVertices ].z;
	
	pFullvertex[ indFullVertices ].S0 
	  = ptexturecoord[ indFullVertices ].s0;
	pFullvertex[ indFullVertices ].T0 
	  = ptexturecoord[ indFullVertices ].t0;
	pFullvertex[ indFullVertices ].U0
	  = ptexturecoord[ indFullVertices ].u0;
      }
      return;
    }

    // color
    else if( ( strcmp( XMLtag->tag , "color" ) == 0  
	       || strcmp( XMLtag->tag , "material" ) == 0  
	       || strcmp( XMLtag->tag , "materialRaytrace" ) == 0  
	       || strcmp( XMLtag->tag , "shader" ) == 0  
	       || strcmp( XMLtag->tag , "texture" ) == 0   
	       || strcmp( XMLtag->tag , "video" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
      material->parse( fileDepth , p_c , id ,
		       XMLtag , NULL );
    }

    // subdivision initial vertices
    else if( strcmp( XMLtag->tag , "tabvertex" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabvertex\n");

      // parsing the coordinates of the initial vertices
      ParseTabVertex( fileDepth , p_c , pvertex ,  nb_vertices );
    }

    // subdivision initial texture coordinates
    else if( strcmp( XMLtag->tag , "tabtextureCoord" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabtextureCoord\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabTextureCoord( fileDepth , p_c , ptexturecoord , nb_vertices );
    }

    else {
      sprintf( ErrorStr , "Error: unknown points component tag [%s] in points ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Points::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<points id=\"%s\" nb_vertex=\"%d\" >\n" , id, nb_vertices  );

  if( isLong ) {
    material->print( file , depth + 1 );


    indent( file , depth + 1 );
    fprintf( file , "<tabvertex size=\"%d\">\n" ,  nb_vertices );
    indent( file , depth + 2 );
    fprintf( file , "<vertexCoordinate point=\"" );
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      fprintf( file , "%.6f %.6f %.6f " , pFullvertex[indVertex].X , pFullvertex[indVertex].Y , 
	       pFullvertex[indVertex].Z );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabvertex>\n\n" );

    indent( file , depth + 1 );
    fprintf( file , "<tabtextureCoord size=\"%d\">\n" ,  nb_vertices );
    indent( file , depth + 2 );
    fprintf( file , "<textureCoordinate point=\"" );
    for( int indVertex = 0 ; indVertex < nb_vertices ; indVertex++ ) {
      fprintf( file , "%.6f %.6f " , pFullvertex[indVertex].S0 , pFullvertex[indVertex].T0 );
    }
    fprintf( file , "\" />\n" );
    indent( file , depth + 1 );
    fprintf( file , "</tabtextureCoord>\n\n" );
  }
  indent( file , depth );
  fprintf( file , "</points>\n" );
}

void dm_Points::bindVertexData( void ) {
  // no shader => no rendering
  if( !material || !material->shader ) {
    // sprintf( ErrorStr , "Warning: missing material or shader on compiled points ID [%s]!"  , id ); ReportError( ErrorStr ); 
    return;
  }

  // primitive drawing
  if( GetvertexArrayID() != NULL_ID ) {
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    // printf( "Geometrical object already has a Display List!\n" );
    //       sprintf( ErrorStr , "Error: Geometrical object already has a Display List!" ); ReportError( ErrorStr ); throw 139;
  }
  
  // Initializes vertex shader data (former display lists or VBOs)
  NewvertexArrayID();
  // printf( "New NewvertexArrayID %s ID %d\n" , id , GetvertexArrayID() );
  NewelementArrayID();
  // printf( "New NewelementArrayID %s ID %d\n" , id , GetelementArrayID() );
  NewvertexBufferObjectID();
  // printf( "New NewvertexBufferObjectID %s ID %d\n" , id , GetvertexBufferObjectID() );
  
  ////////////////////////
  // Ondemand shader initialization
  // initializes the shader if it is the first rendering
  if( material->shader->shaderProgram == NULL_ID ) {
    // printf("Initialize shader\n");
    material->shader->InitializeShader();
  }

  //  glUseProgram( material->shader->shaderProgram );

  // printf("points %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f \n", pFullvertex[0].x , pFullvertex[0].y , pFullvertex[0].z , pFullvertex[1].x , pFullvertex[1].y , pFullvertex[1].z , pFullvertex[2].x , pFullvertex[2].y , pFullvertex[2].z , pFullvertex[3].x , pFullvertex[3].y , pFullvertex[3].z );
  
  ////////////////////////
  // Generate a vertex array object VAO for the points
  glBindVertexArray( GetvertexArrayID() );

  // Generate an element array buffer EAO and copy data into it
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PointsIndex) * (nb_vertices), pIndices , GL_STATIC_DRAW);
  // printf("size of faces %d\n", sizeof(PointsIndex));

  // Generate vertex buffer object VBO and copy data into the buffer
  glBindBuffer(GL_ARRAY_BUFFER, GetvertexBufferObjectID() );
  glBufferData(GL_ARRAY_BUFFER, sizeof(PointsFullVertex) * nb_vertices, pFullvertex, GL_STATIC_DRAW);
  // printf("size of vertices %d\n", sizeof(PointsFullVertex));

  // Specify the layout of the vertex data
  GLuint shaderProgram = material->shader->shaderProgram;

  // positions
  GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );  
  glEnableVertexAttribArray(posAttrib);    //We like submitting vertices on stream 0 for no special reason
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(PointsFullVertex), BUFFER_OFFSET(0));   //The starting point of the vertex array, for the vertices
  // printf("size of one vertex %d\n", sizeof(MeshFullVertex));

  // textures
  GLint textAttrib = glGetAttribLocation( shaderProgram, "texcoord" );
  glEnableVertexAttribArray( textAttrib );
  glVertexAttribPointer(textAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(PointsFullVertex), BUFFER_OFFSET(3*sizeof(float)));   //The starting point of texcoords, 12 bytes away

  // printf("end of bind vertex data %s\n", id );
}

void dm_Points::display( DrawingMode mode , bool *invalidDL ) {
  // if the texture of the object has been loaded in GPU during drawing
  // the display list cannot be used otherwise the texture would be
  // reloaded at each DL ID call
  if( GetvertexArrayID() == NULL_ID || *invalidDL ) {
    // printf( "Invalid DL bind vertex data %d\n" , *invalidDL );
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    bindVertexData();
  }
  // printf( "Display %s GetvertexArrayID %d GetelementArrayID %d shaderProgram %d \n" , id,  GetvertexArrayID(), GetelementArrayID() , material->getshaderProgram()  );

  glPointSize( 1 );

  // no shader => no rendering
  if( !material || !material->shader ) {
    return;
  }

  // printf( "Enable material\n" );
  material->shader->shader_enables( material );
  // glActiveTexture(GL_TEXTURE0 + 0);

  // printf( "Draw primitive\n" );
  glBindVertexArray( GetvertexArrayID() );
  // printf("Points GetvertexArrayID %d\n", GetvertexArrayID() );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glDrawElements( GL_POINTS, nb_vertices , GL_UNSIGNED_INT, 0 );
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  

  // printf( "End of drawing\n" );
  // material->shader->shader_disables();

  // glActiveTexture(GL_TEXTURE0 + 0);
}

void dm_Points::update( dm_point3 *tabPoint ) {
  // for( int ind  = 0 ; ind < 4 ; ind++ ) {
  //   points[ind] = tabPoint[ind];
  // }
  update();
}

void dm_Points::update( void ) {
}

void dm_Points::operator=(dm_Points& p) {
  this->dm_GeometricalObject::operator=(p);

  nb_vertices = p.nb_vertices;
  if( pFullvertex ) {
    delete [] pFullvertex;
    pFullvertex = NULL;
  }
  if( p.pFullvertex ) {
    pFullvertex = new PointsFullVertex[ nb_vertices ];
    for( int ind  = 0 ; ind < nb_vertices ; ind++ ) {
      pFullvertex[ind].X = p.pFullvertex[ind].X;
      pFullvertex[ind].Y = p.pFullvertex[ind].Y;
      pFullvertex[ind].Z = p.pFullvertex[ind].Z;
      pFullvertex[ind].S0 = p.pFullvertex[ind].S0;
      pFullvertex[ind].T0 = p.pFullvertex[ind].T0;
      pFullvertex[ind].U0 = p.pFullvertex[ind].U0;
    }
  }

  nb_vertices = p.nb_vertices;
  if( ptexturecoord ) {
    delete [] ptexturecoord;
    ptexturecoord = NULL;
  }
  if( p.ptexturecoord ) {
    ptexturecoord = new MeshTexturecoord[ nb_vertices ];
    for( int ind  = 0 ; ind < nb_vertices ; ind++ ) {
      ptexturecoord[ind].s0 = p.ptexturecoord[ind].s0;
      ptexturecoord[ind].t0 = p.ptexturecoord[ind].t0;
      ptexturecoord[ind].u0 = p.ptexturecoord[ind].u0;
    }
  }
  if( pvertex ) {
    delete [] pvertex;
    pvertex = NULL;
  }
  if( p.pvertex ) {
    pvertex = new MeshVertex[ nb_vertices ];
    for( int ind  = 0 ; ind < nb_vertices ; ind++ ) {
      pvertex[ind].x = p.pvertex[ind].x;
      pvertex[ind].y = p.pvertex[ind].y;
      pvertex[ind].z = p.pvertex[ind].z;
    }
  }
}


///////////////////////////////////////////////////////////////////
// QUAD
///////////////////////////////////////////////////////////////////

dm_Quad::dm_Quad( void ):dm_GeometricalObject() {
  for( int ind  = 0 ; ind < 4 ; ind++ ) {
    pFullvertex[ind].X = 0;
    pFullvertex[ind].Y = 0;
    pFullvertex[ind].Z = 0;
    pFullvertex[ind].NX = 0;
    pFullvertex[ind].NY = 0;
    pFullvertex[ind].NZ = 0;
    pFullvertex[ind].S0 = 0;
    pFullvertex[ind].T0 = 0;
    pFullvertex[ind].U0 = 0;
  }
  center = *Origin;
  averageNormal = *NullVector;
}

dm_Quad::~dm_Quad( void ) {
  // TODO
}

void dm_Quad::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
		    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  // POINTS
  if( XMLtag->hasAttribute( "x1" ) ) {
    oper( pFullvertex[0].X , (float)XMLtag->getAttributeDoubleValue( "x1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y1" ) ) {
    oper( pFullvertex[0].Y , (float)XMLtag->getAttributeDoubleValue( "y1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z1" ) ) {
    oper( pFullvertex[0].Z , (float)XMLtag->getAttributeDoubleValue( "z1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "x2" ) ) {
    oper( pFullvertex[1].X , (float)XMLtag->getAttributeDoubleValue( "x2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y2" ) ) {
    oper( pFullvertex[1].Y , (float)XMLtag->getAttributeDoubleValue( "y2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z2" ) ) {
    oper( pFullvertex[1].Z , (float)XMLtag->getAttributeDoubleValue( "z2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "x3" ) ) {
    oper( pFullvertex[2].X , (float)XMLtag->getAttributeDoubleValue( "x3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y3" ) ) {
    oper( pFullvertex[2].Y , (float)XMLtag->getAttributeDoubleValue( "y3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z3" ) ) {
    oper( pFullvertex[2].Z , (float)XMLtag->getAttributeDoubleValue( "z3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "x4" ) ) {
    oper( pFullvertex[3].X , (float)XMLtag->getAttributeDoubleValue( "x4" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y4" ) ) {
    oper( pFullvertex[3].Y , (float)XMLtag->getAttributeDoubleValue( "y4" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z4" ) ) {
    oper( pFullvertex[3].Z , (float)XMLtag->getAttributeDoubleValue( "z4" , 
						       atRunTime ));
  }

  // COLORS
  if( XMLtag->hasAttribute( "r1" ) ) {
    oper( pFullvertex[0].R , (float)XMLtag->getAttributeDoubleValue( "r1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g1" ) ) {
    oper( pFullvertex[0].G , (float)XMLtag->getAttributeDoubleValue( "g1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b1" ) ) {
    oper( pFullvertex[0].B , (float)XMLtag->getAttributeDoubleValue( "b1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a1" ) ) {
    oper( pFullvertex[0].A , (float)XMLtag->getAttributeDoubleValue( "a1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "r2" ) ) {
    oper( pFullvertex[1].R , (float)XMLtag->getAttributeDoubleValue( "r2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g2" ) ) {
    oper( pFullvertex[1].G , (float)XMLtag->getAttributeDoubleValue( "g2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b2" ) ) {
    oper( pFullvertex[1].B , (float)XMLtag->getAttributeDoubleValue( "b2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a2" ) ) {
    oper( pFullvertex[1].A , (float)XMLtag->getAttributeDoubleValue( "a2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "r3" ) ) {
    oper( pFullvertex[2].R , (float)XMLtag->getAttributeDoubleValue( "r3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g3" ) ) {
    oper( pFullvertex[2].G , (float)XMLtag->getAttributeDoubleValue( "g3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b3" ) ) {
    oper( pFullvertex[2].B , (float)XMLtag->getAttributeDoubleValue( "b3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a3" ) ) {
    oper( pFullvertex[2].A , (float)XMLtag->getAttributeDoubleValue( "a3" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "r4" ) ) {
    oper( pFullvertex[3].R , (float)XMLtag->getAttributeDoubleValue( "r4" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g4" ) ) {
    oper( pFullvertex[3].G , (float)XMLtag->getAttributeDoubleValue( "g4" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b4" ) ) {
    oper( pFullvertex[3].B , (float)XMLtag->getAttributeDoubleValue( "b4" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a4" ) ) {
    oper( pFullvertex[3].A , (float)XMLtag->getAttributeDoubleValue( "a4" , 
						       atRunTime ));
  }
  update();
}

bool dm_Quad::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "x1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].X;
    return true;
  }
  else if( strcmp( attribute , "y1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].Y;
    return true;
  }
  else if( strcmp( attribute , "z1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].Z;
    return true;
  }
  else if( strcmp( attribute , "x2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].X;
    return true;
  }
  else if( strcmp( attribute , "y2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].Y;
    return true;
  }
  else if( strcmp( attribute , "z2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].Z;
    return true;
  }
  else if( strcmp( attribute , "x3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].X;
    return true;
  }
  else if( strcmp( attribute , "y3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].Y;
    return true;
  }
  else if( strcmp( attribute , "z3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].Z;
    return true;
  }
  else if( strcmp( attribute , "x4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].X;
    return true;
  }
  else if( strcmp( attribute , "y4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].Y;
    return true;
  }
  else if( strcmp( attribute , "z4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].Z;
    return true;
  }
  else if( strcmp( attribute , "r1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].R;
    return true;
  }
  else if( strcmp( attribute , "g1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].G;
    return true;
  }
  else if( strcmp( attribute , "b1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].B;
    return true;
  }
  else if( strcmp( attribute , "a1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[0].A;
    return true;
  }
  else if( strcmp( attribute , "r2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].R;
    return true;
  }
  else if( strcmp( attribute , "g2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].G;
    return true;
  }
  else if( strcmp( attribute , "b2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].B;
    return true;
  }
  else if( strcmp( attribute , "a2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[1].A;
    return true;
  }
  else if( strcmp( attribute , "r3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].R;
    return true;
  }
  else if( strcmp( attribute , "g3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].G;
    return true;
  }
  else if( strcmp( attribute , "b3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].B;
    return true;
  }
  else if( strcmp( attribute , "a3" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[2].A;
    return true;
  }
  else if( strcmp( attribute , "r4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].R;
    return true;
  }
  else if( strcmp( attribute , "g4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].G;
    return true;
  }
  else if( strcmp( attribute , "b4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].B;
    return true;
  }
  else if( strcmp( attribute , "a4" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)pFullvertex[3].A;
    return true;
  }
  return false;
}

void dm_Quad::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  while( true ) {
    // new tag XML : quad component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // quad closing tag
    if( strcmp( XMLtag->tag , "quad" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // color
    else if( ( strcmp( XMLtag->tag , "color" ) == 0  
	       || strcmp( XMLtag->tag , "material" ) == 0  
	       || strcmp( XMLtag->tag , "materialRaytrace" ) == 0  
	       || strcmp( XMLtag->tag , "shader" ) == 0  
	       || strcmp( XMLtag->tag , "texture" ) == 0   
	       || strcmp( XMLtag->tag , "video" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
      material->parse( fileDepth , p_c , id ,
		       XMLtag , NULL );
    }

    else {
      sprintf( ErrorStr , "Error: unknown quad component tag [%s] in quad ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Quad::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<quad id=\"%s\" x1=\"%.5f\" y1=\"%.5f\" z1=\"%.5f\" x2=\"%.5f\" y2=\"%.5f\" z2=\"%.5f\" x3=\"%.5f\" y3=\"%.5f\" z3=\"%.5f\" x4=\"%.5f\" y4=\"%.5f\" z4=\"%.5f\" r1=\"%.5f\" g1=\"%.5f\" b1=\"%.5f\" a1=\"%.5f\" r2=\"%.5f\" g2=\"%.5f\" b2=\"%.5f\" a2=\"%.5f\" r3=\"%.5f\" g3=\"%.5f\" b3=\"%.5f\" a3=\"%.5f\" r4=\"%.5f\" g4=\"%.5f\" b4=\"%.5f\" a4=\"%.5f\" s1=\"%.5f\" t1=\"%.5f\" u1=\"%.5f\" s2=\"%.5f\" t2=\"%.5f\" u2=\"%.5f\" s3=\"%.5f\" t3=\"%.5f\" u3=\"%.5f\" s4=\"%.5f\" t4=\"%.5f\" u4=\"%.5f\" nx1=\"%.5f\" ny1=\"%.5f\" nz1=\"%.5f\" nx2=\"%.5f\" ny2=\"%.5f\" nz2=\"%.5f\" nx3=\"%.5f\" ny3=\"%.5f\" nz3=\"%.5f\" nx4=\"%.5f\" ny4=\"%.5f\" nz4=\"%.5f\" >\n" , id , pFullvertex[0].X , pFullvertex[0].Y , pFullvertex[0].Z , pFullvertex[1].X , pFullvertex[1].Y , pFullvertex[1].Z , pFullvertex[2].X , pFullvertex[2].Y , pFullvertex[2].Z , pFullvertex[3].X , pFullvertex[3].Y , pFullvertex[3].Z , pFullvertex[0].R , pFullvertex[0].G , pFullvertex[0].B , pFullvertex[0].A , pFullvertex[1].R , pFullvertex[1].G , pFullvertex[1].B , pFullvertex[1].A , pFullvertex[2].R , pFullvertex[2].G , pFullvertex[2].B , pFullvertex[2].A , pFullvertex[3].R , pFullvertex[3].G , pFullvertex[3].B , pFullvertex[3].A , pFullvertex[0].S0 , pFullvertex[0].T0 , pFullvertex[0].U0 , pFullvertex[1].S0 , pFullvertex[1].T0 , pFullvertex[1].U0 , pFullvertex[2].S0 , pFullvertex[2].T0 , pFullvertex[2].U0 , pFullvertex[3].S0 , pFullvertex[3].T0 , pFullvertex[3].U0  , pFullvertex[0].NX , pFullvertex[0].NY , pFullvertex[0].NZ , pFullvertex[1].NX , pFullvertex[1].NY , pFullvertex[1].NZ , pFullvertex[2].NX , pFullvertex[2].NY , pFullvertex[2].NZ , pFullvertex[3].NX , pFullvertex[3].NY , pFullvertex[3].NZ );

  if( isLong ) {
    material->print( file , depth + 1 );


//     indent( file , depth + 1 );
//     fprintf( file , "<tabpoint size=4>\n" );
//     for( int indPoint = 0 ; indPoint < 4 ; indPoint++ ) {
//       points[indPoint].print( file , depth + 2 , indPoint );
//     }
//     indent( file , depth + 1 );
//     fprintf( file , "</tabpoint>\n" );
  }
  indent( file , depth );
  fprintf( file , "</quad>\n" );
}

void dm_Quad::bindVertexData( void ) {
  // no shader => no rendering
  if( !material || !material->shader ) {
    // sprintf( ErrorStr , "Warning: missing material or shader on compiled quad ID [%s]!"  , id ); ReportError( ErrorStr ); 
    return;
  }

  // primitive drawing
  if( GetvertexArrayID() != NULL_ID ) {
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    // printf( "Geometrical object already has a Display List!\n" );
    //       sprintf( ErrorStr , "Error: Geometrical object already has a Display List!" ); ReportError( ErrorStr ); throw 139;
  }
  
  // Initializes vertex shader data (former display lists or VBOs)
  NewvertexArrayID();
  // printf( "New NewvertexArrayID %s ID %d\n" , id , GetvertexArrayID() );
  NewelementArrayID();
  // printf( "New NewelementArrayID %s ID %d\n" , id , GetelementArrayID() );
  NewvertexBufferObjectID();
  // printf( "New NewvertexBufferObjectID %s ID %d\n" , id , GetvertexBufferObjectID() );
   
  //printf( "Draw quad geometry\n" );

  ///////////////////////////////////////////////////////////////
  // GEOMETRY COMPILING

  ///////////////////////////
  // vertex #0
  if( material->hasTexture ) {
    if( material->materialTexture->textureTarget == GL_TEXTURE_2D ) {
      pFullvertex[0].S0 = material->materialTexture->textureOffset_s;
      pFullvertex[0].T0 = material->materialTexture->textureOffset_t;
      pFullvertex[0].U0 = 0.0;
    }
    else if( material->materialTexture->textureTarget == GL_TEXTURE_3D ) {
      pFullvertex[0].S0 = material->materialTexture->textureOffset_s;
      pFullvertex[0].T0 = material->materialTexture->textureOffset_t;
      pFullvertex[0].U0 = material->materialTexture->textureOffset_u + material->materialTexture->textureTile_u;
    }
  }
  
  // same normal for all the vertices (flat quad expected)
  // printf( "averageNormal %s %.2f %.2f %.2f\n", id , averageNormal.x , averageNormal.y , averageNormal.z );
  //       printf( "center %s %.2f %.2f %.2f\n", id , center.x , center.y , center.z );
  pFullvertex[0].NX = averageNormal.x;
  pFullvertex[0].NY = averageNormal.y;
  pFullvertex[0].NZ = averageNormal.z;
  
  ///////////////////////////
  // vertex #1
  if( material->hasTexture ) {
    if( material->materialTexture->textureTarget == GL_TEXTURE_2D ) {
      pFullvertex[1].S0 = material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s;
      pFullvertex[1].T0 = material->materialTexture->textureOffset_t;
      pFullvertex[1].U0 = 0.0;
    }
    else if( material->materialTexture->textureTarget == GL_TEXTURE_3D ) {
      pFullvertex[1].S0 = material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s;
      pFullvertex[1].T0 = material->materialTexture->textureOffset_t;
      pFullvertex[1].U0 = material->materialTexture->textureOffset_u + material->materialTexture->textureTile_u;
    }
  }
  pFullvertex[1].NX = averageNormal.x;
  pFullvertex[1].NY = averageNormal.y;
  pFullvertex[1].NZ = averageNormal.z;
  
  ///////////////////////////
  // vertex #2
  if( material->hasTexture ) {
    if( material->materialTexture->textureTarget == GL_TEXTURE_2D ) {
      pFullvertex[2].S0 = material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s;
      pFullvertex[2].T0 = material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t;
      pFullvertex[2].U0 = 0.0;
    }
    else if( material->materialTexture->textureTarget == GL_TEXTURE_3D ) {
      pFullvertex[2].S0 = material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s;
      pFullvertex[2].T0 = material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t;
      pFullvertex[2].U0 = material->materialTexture->textureOffset_u + material->materialTexture->textureTile_u;
    }
  }
  pFullvertex[2].NX = averageNormal.x;
  pFullvertex[2].NY = averageNormal.y;
  pFullvertex[2].NZ = averageNormal.z;
  
  ///////////////////////////
  // vertex #3
  if( material->hasTexture ) {
    if( material->materialTexture->textureTarget == GL_TEXTURE_2D ) {
      pFullvertex[3].S0 = material->materialTexture->textureOffset_s;
      pFullvertex[3].T0 = material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t;
      pFullvertex[3].U0 = 0.0;
    }
    else if( material->materialTexture->textureTarget == GL_TEXTURE_3D ) {
      pFullvertex[3].S0 = material->materialTexture->textureOffset_s;
      pFullvertex[3].T0 = material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t;
      pFullvertex[3].U0 = material->materialTexture->textureOffset_u + material->materialTexture->textureTile_u;
    }
  }
  pFullvertex[3].NX = averageNormal.x;
  pFullvertex[3].NY = averageNormal.y;
  pFullvertex[3].NZ = averageNormal.z;

  GLuint pindices[6];
  pindices[0] = 0;
  pindices[1] = 1;
  pindices[2] = 3;
  pindices[3] = 1;
  pindices[4] = 2;
  pindices[5] = 3;

  ////////////////////////
  // Ondemand shader initialization
  // initializes the shader if it is the first rendering
  if( material->shader->shaderProgram == NULL_ID ) {
    // printf("Initialize shader\n");
    material->shader->InitializeShader();
  }

  //  glUseProgram( material->shader->shaderProgram );

   //printf("points %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f   %.2f %.2f %.2f \n", pFullvertex[0].X , pFullvertex[0].Y , pFullvertex[0].Z ,
   //  pFullvertex[1].X , pFullvertex[1].Y, pFullvertex[1].Z , pFullvertex[2].X , pFullvertex[2].Y , pFullvertex[2].Z , pFullvertex[3].X , pFullvertex[3].Y , pFullvertex[3].Z );
  
  ////////////////////////
  // Generate a vertex array object VAO for 2 triangles
  glBindVertexArray( GetvertexArrayID() );

  // Generate an element array buffer EAO and copy data into it
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pindices), pindices, GL_STATIC_DRAW);
  // printf("size of pindices %d\n", sizeof(pindices));

  // Specify the layout of the vertex data
  // Generate vertex buffer object VBO and copy data into the buffer
  glBindBuffer(GL_ARRAY_BUFFER, GetvertexBufferObjectID() );
  glBufferData(GL_ARRAY_BUFFER, sizeof(pFullvertex), pFullvertex, GL_STATIC_DRAW);
  // printf("size of vertices %d\n", sizeof(pFullvertex));

  // Specify the layout of the vertex data
  GLuint shaderProgram = material->shader->shaderProgram;
  // positions
  GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );  
  glEnableVertexAttribArray(posAttrib);    //We like submitting vertices on stream 0 for no special reason
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(0));   //The starting point of the vertex array, for the vertices
  // printf("size of one vertex %d\n", sizeof(MeshFullVertex));

  // normals
  GLint normAttrib = glGetAttribLocation( shaderProgram, "normal" );
  glEnableVertexAttribArray( normAttrib );
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(3*sizeof(float)));     //The starting point of normals, 12 bytes away

  // textures
  GLint textAttrib = glGetAttribLocation( shaderProgram, "texcoord" );
  glEnableVertexAttribArray( textAttrib );
  glVertexAttribPointer(textAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(6*sizeof(float)));   //The starting point of texcoords, 24 bytes away

  // colors
  GLint colAttrib = glGetAttribLocation( shaderProgram, "color" );
  glEnableVertexAttribArray( colAttrib );
  glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(MeshFullVertex), BUFFER_OFFSET(9*sizeof(float)));   //The starting point of texcoords, 24 bytes away

  // printf("end of bind vertex data %s\n", id );
}

void dm_Quad::display( DrawingMode mode , bool *invalidDL ) {
  // if the texture of the object has been loaded in GPU during drawing
  // the display list cannot be used otherwise the texture would be
  // reloaded at each DL ID call
  // printf( "bindVertexData %s GetvertexArrayID %d GetelementArrayID %d shaderProgram %d \n" , id,  GetvertexArrayID(), GetelementArrayID() , GetvertexArrayID() );
  if( GetvertexArrayID() == NULL_ID || *invalidDL ) {
    DeletevertexArrayID();
    DeleteelementArrayID();
    DeletevertexBufferObjectID();
    bindVertexData();
  }

  // no shader => no rendering
  if( !material || !material->shader ) {
    return;
  }

  // printf( "Enable material\n" );
  material->shader->shader_enables( material );
  // glActiveTexture(GL_TEXTURE0 + 0);

  // printf( "Draw primitive\n" );
  glBindVertexArray( GetvertexArrayID() );
  // printf("Quad GetvertexArrayID %d\n", GetvertexArrayID() );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetelementArrayID() );
  glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  

  // printf( "End of drawing\n" );
  // material->shader->shader_disables();

  // glActiveTexture(GL_TEXTURE0 + 0);
}

void dm_Quad::averageNormals( dm_vector3 *normal ) {
  dm_vector3 normals[4];

  dm_point3 ptaux1 , ptaux2;
  ptaux1.x =pFullvertex[0].X;
  ptaux1.y =pFullvertex[0].Y;
  ptaux1.z =pFullvertex[0].Z;
  ptaux2.x =pFullvertex[1].X;
  ptaux2.y =pFullvertex[1].Y;
  ptaux2.z =pFullvertex[1].Z;
  normals[0].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[1].X;
  ptaux1.y =pFullvertex[1].Y;
  ptaux1.z =pFullvertex[1].Z;
  ptaux2.x =pFullvertex[2].X;
  ptaux2.y =pFullvertex[2].Y;
  ptaux2.z =pFullvertex[2].Z;
  normals[1].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[2].X;
  ptaux1.y =pFullvertex[2].Y;
  ptaux1.z =pFullvertex[2].Z;
  ptaux2.x =pFullvertex[3].X;
  ptaux2.y =pFullvertex[3].Y;
  ptaux2.z =pFullvertex[3].Z;
  normals[2].prodVect( center , ptaux1 , ptaux2 );
  ptaux1.x =pFullvertex[3].X;
  ptaux1.y =pFullvertex[3].Y;
  ptaux1.z =pFullvertex[3].Z;
  ptaux2.x =pFullvertex[0].X;
  ptaux2.y =pFullvertex[0].Y;
  ptaux2.z =pFullvertex[0].Z;
  normals[3].prodVect( center , ptaux1 , ptaux2 );

  averageNormal.averageVectors( normals , 4 );
  averageNormal *= -1;
  averageNormal.normalize();
}

void dm_Quad::update( dm_point3 *tabPoint ) {
  // for( int ind  = 0 ; ind < 4 ; ind++ ) {
  //   points[ind] = tabPoint[ind];
  // }
  update();
}

void dm_Quad::update( void ) {
  center.x = 0.25F * (pFullvertex[0].X + pFullvertex[1].X + pFullvertex[2].X + pFullvertex[3].X);
  center.y = 0.25F * (pFullvertex[0].Y + pFullvertex[1].Y + pFullvertex[2].Y + pFullvertex[3].Y);
  center.z = 0.25F * (pFullvertex[0].Z + pFullvertex[1].Z + pFullvertex[2].Z + pFullvertex[3].Z);
  averageNormals( &averageNormal );
}

float dm_Quad::height( void ) {
  return pFullvertex[0].Y;
}

void dm_Quad::operator=(dm_Quad& p) {
  this->dm_GeometricalObject::operator=(p);

  for( int ind  = 0 ; ind < 4 ; ind++ ) {
    pFullvertex[ind].X = p.pFullvertex[ind].X;
    pFullvertex[ind].Y = p.pFullvertex[ind].Y;
    pFullvertex[ind].X = p.pFullvertex[ind].Z;
    pFullvertex[ind].NX = p.pFullvertex[ind].NX;
    pFullvertex[ind].NY = p.pFullvertex[ind].NY;
    pFullvertex[ind].NZ = p.pFullvertex[ind].NZ;
    pFullvertex[ind].S0 = p.pFullvertex[ind].S0;
    pFullvertex[ind].T0 = p.pFullvertex[ind].T0;
    pFullvertex[ind].U0 = p.pFullvertex[ind].U0;
    pFullvertex[ind].R = p.pFullvertex[ind].R;
    pFullvertex[ind].G = p.pFullvertex[ind].G;
    pFullvertex[ind].B = p.pFullvertex[ind].B;
    pFullvertex[ind].A = p.pFullvertex[ind].A;
  }
  center = p.center;
  averageNormal = p.averageNormal;
}


///////////////////////////////////////////////////////////
// POINT
////////////////////////////////////////////////////////// 


dm_Point::dm_Point( void ):dm_GeometricalObject() {
  center = *Origin;
  for( int indCol  = 0 ; indCol < 4 ; indCol++ ) {
      color[indCol] = 1.0;
  }
  width = 1.0;
  valScalar_screen_location = NULL;
}

dm_Point::~dm_Point( void ) {
  // TODO
  if( valScalar_screen_location ) {
    delete [] valScalar_screen_location;
  }
}

void dm_Point::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  // POINT
  if( XMLtag->hasAttribute( "x" ) ) {
    oper( center.x , (float)XMLtag->getAttributeDoubleValue( "x" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y" ) ) {
    oper( center.y , (float)XMLtag->getAttributeDoubleValue( "y" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z" ) ) {
    oper( center.z , (float)XMLtag->getAttributeDoubleValue( "z" , 
						       atRunTime ));
  }

  // COLOR
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

  // SIZE
  if( XMLtag->hasAttribute( "width" ) ) {
    oper( width , (float)XMLtag->getAttributeDoubleValue( "width" , 
						       atRunTime ));
  }
}

bool dm_Point::getParameters( char *attribute , 
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
    *valDouble = (double)center.x;
    return true;
  }
  else if( strcmp( attribute , "y" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)center.y;
    return true;
  }
  else if( strcmp( attribute , "z" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)center.z;
    return true;
  }
  else if( strcmp( attribute , "r" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color[0];
    return true;
  }
  else if( strcmp( attribute , "g" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color[1];
    return true;
  }
  else if( strcmp( attribute , "b" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color[2];
    return true;
  }
  else if( strcmp( attribute , "a" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color[3];
    return true;
  }
  else if( strcmp( attribute , "width" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)width;
    return true;
  }
  else if( strcmp( attribute , "screen_location" ) == 0 ) {
    // returned table size
    *tableOrMatrixSize = 3;
    // returned table parameter
    // uses the local storage 
    if( !valScalar_screen_location ) {
      valScalar_screen_location = new dm_ValScalar[ 3 ];
    }
    *valScalars = valScalar_screen_location;

    for( int ind = 0 ; ind < 3 ; ind++ ) {
      valScalar_screen_location[ind].SetType( DataTypeDouble );
      valScalar_screen_location[ ind ].SetValue( 0.0 );
    } 
    // else the screen coordinates are stored by the display
    // function of the sceneNode

    *dataType = DataTypeDouble;
    return true;
  }
  return false;
}

void dm_Point::parse( int *fileDepth , int *p_c ,
		      bool isOpeningTag ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  if( isOpeningTag ) {
    while( true ) {
      // new tag XML : point component or closing tag
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
      // point closing tag
      if( strcmp( XMLtag->tag , "point" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
	delete XMLtag;
	XMLtag = NULL;

	return;
      }
      
      // color
      else if( ( strcmp( XMLtag->tag , "color" ) == 0  
		 || strcmp( XMLtag->tag , "material" ) == 0  
		 || strcmp( XMLtag->tag , "materialRaytrace" ) == 0  
		 || strcmp( XMLtag->tag , "shader" ) == 0  
		 || strcmp( XMLtag->tag , "texture" ) == 0   
		 || strcmp( XMLtag->tag , "video" ) == 0  )
	       && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
	material->parse( fileDepth , p_c , id ,
			 XMLtag , NULL );
      }
      
      else {
	sprintf( ErrorStr , "Error: unknown point component tag [%s] in point ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
      }
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Point::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<point id=\"%s\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" r=\"%.5f\" g=\"%.5f\" b=\"%.5f\" a=\"%.5f\" width=\"%.5f\">\n" , id , center.x , center.y , center.z , color[0] , color[1] , color[2] , color[3] , width );

  if( isLong ) {
    material->print( file , depth + 1 );


//     indent( file , depth + 1 );
//     fprintf( file , "<tabpoint size=4>\n" );
//     for( int indPoint = 0 ; indPoint < 4 ; indPoint++ ) {
//       points[indPoint].print( file , depth + 2 , indPoint );
//     }
//     indent( file , depth + 1 );
//     fprintf( file , "</tabpoint>\n" );
  }
  indent( file , depth );
  fprintf( file , "</point>\n" );
}

void dm_Point::display( DrawingMode mode , bool *invalidDL ) {
  if( width <= 0.0 ) {
    return;
  }

  //printf( "Draw point material\n" );
  // material->display( mode , invalidDL );
  //printf( "Draw point geometry\n" );
  
  glPointSize( width );
  glLineWidth( width );
  ///////////////////////////
  // center
  // printf( "dm_Point hasColor %d\n" , material->hasColor );
  if( !material->hasMaterial ) {
    glColor4fv( color );
  }
  glBegin( GL_POINTS );    
  glVertex3f( center.x , center.y , center.z );
  glEnd();

  // material->close( mode );
}

float dm_Point::height( void ) {
  return center.y;
}

void dm_Point::operator=(dm_Point& p) {
  this->dm_GeometricalObject::operator=(p);

  center = p.center;
  for( int ind  = 0 ; ind < 4 ; ind++ ) {
      color[ind] = p.color[ind];
  }
  width = p.width;
  valScalar_screen_location = p.valScalar_screen_location;
  if( valScalar_screen_location ) {
    delete [] valScalar_screen_location;
  }
  if( p.valScalar_screen_location ) {
    valScalar_screen_location = new dm_ValScalar[ 3 ];
    for( int ind = 0 ; ind < 3 ; ind++ ) {
      valScalar_screen_location[ind] = p.valScalar_screen_location[ind];
    }
  }
}

///////////////////////////////////////////////////////////
// LINE
////////////////////////////////////////////////////////// 


dm_Line::dm_Line( void ):dm_GeometricalObject() {
  p1 = *Origin;
  p2 = *Origin;
  for( int indCol  = 0 ; indCol < 4 ; indCol++ ) {
      color_p1[indCol] = 1.0;
	  color_p2[indCol] = 1.0;
  }
  width = 1.0;
}

dm_Line::~dm_Line( void ) {
  // TODO
}

void dm_Line::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  // LINE
  if( XMLtag->hasAttribute( "x1" ) ) {
    oper( p1.x , (float)XMLtag->getAttributeDoubleValue( "x1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y1" ) ) {
    oper( p1.y , (float)XMLtag->getAttributeDoubleValue( "y1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z1" ) ) {
    oper( p1.z , (float)XMLtag->getAttributeDoubleValue( "z1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "x2" ) ) {
    oper( p2.x , (float)XMLtag->getAttributeDoubleValue( "x2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "y2" ) ) {
    oper( p2.y , (float)XMLtag->getAttributeDoubleValue( "y2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "z2" ) ) {
    oper( p2.z , (float)XMLtag->getAttributeDoubleValue( "z2" , 
						       atRunTime ));
  }

  // COLOR
  if( XMLtag->hasAttribute( "r1" ) ) {
    oper( color_p1[0] , (float)XMLtag->getAttributeDoubleValue( "r1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g1" ) ) {
    oper( color_p1[1] , (float)XMLtag->getAttributeDoubleValue( "g1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b1" ) ) {
    oper( color_p1[2] , (float)XMLtag->getAttributeDoubleValue( "b1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a1" ) ) {
    oper( color_p1[3] , (float)XMLtag->getAttributeDoubleValue( "a1" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "r2" ) ) {
    oper( color_p2[0] , (float)XMLtag->getAttributeDoubleValue( "r2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "g2" ) ) {
    oper( color_p2[1] , (float)XMLtag->getAttributeDoubleValue( "g2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "b2" ) ) {
    oper( color_p2[2] , (float)XMLtag->getAttributeDoubleValue( "b2" , 
						       atRunTime ));
  }
  if( XMLtag->hasAttribute( "a2" ) ) {
    oper( color_p2[3] , (float)XMLtag->getAttributeDoubleValue( "a2" , 
						       atRunTime ));
  }

  // SIZE
  if( XMLtag->hasAttribute( "width" ) ) {
    oper( width , (float)XMLtag->getAttributeDoubleValue( "width" , 
						       atRunTime ));
  }
}

bool dm_Line::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "x1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p1.x;
    return true;
  }
  else if( strcmp( attribute , "y1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p1.y;
    return true;
  }
  else if( strcmp( attribute , "z1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p1.z;
    return true;
  }
  else if( strcmp( attribute , "x2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p2.x;
    return true;
  }
  else if( strcmp( attribute , "y2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p2.y;
    return true;
  }
  else if( strcmp( attribute , "z2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)p2.z;
    return true;
  }
  else if( strcmp( attribute , "r1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p1[0];
    return true;
  }
  else if( strcmp( attribute , "g1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p1[1];
    return true;
  }
  else if( strcmp( attribute , "b1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p1[2];
    return true;
  }
  else if( strcmp( attribute , "a1" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p1[3];
    return true;
  }
  else if( strcmp( attribute , "r2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p2[0];
    return true;
  }
  else if( strcmp( attribute , "g2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p2[1];
    return true;
  }
  else if( strcmp( attribute , "b2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p2[2];
    return true;
  }
  else if( strcmp( attribute , "a2" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_p2[3];
    return true;
  }
  else if( strcmp( attribute , "width" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)width;
    return true;
  }
  return false;
}

void dm_Line::parse( int *fileDepth , int *p_c ,
		      bool isOpeningTag ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  if( isOpeningTag ) {
    while( true ) {
      // new tag XML : line component or closing tag
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
      // line closing tag
      if( strcmp( XMLtag->tag , "line" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
	delete XMLtag;
	XMLtag = NULL;

	return;
      }
      
      // color
      else if( ( strcmp( XMLtag->tag , "color" ) == 0  
		 || strcmp( XMLtag->tag , "material" ) == 0  
		 || strcmp( XMLtag->tag , "materialRaytrace" ) == 0  
		 || strcmp( XMLtag->tag , "shader" ) == 0  
		 || strcmp( XMLtag->tag , "texture" ) == 0   
		 || strcmp( XMLtag->tag , "video" ) == 0  )
	       && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
	material->parse( fileDepth , p_c , id ,
			 XMLtag , NULL );
      }
      
      else {
	sprintf( ErrorStr , "Error: unknown line component tag [%s] in point ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
      }
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Line::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<line id=\"%s\" x1=\"%.5f\" y1=\"%.5f\" z1=\"%.5f\" x2=\"%.5f\" y2=\"%.5f\" z2=\"%.5f\" r1=\"%.5f\" g1=\"%.5f\" b1=\"%.5f\" a1=\"%.5f\" r2=\"%.5f\" g2=\"%.5f\" b2=\"%.5f\" a2=\"%.5f\" width=\"%.5f\">\n" , id , p1.x , p1.y , p1.z , p2.x , p2.y , p2.z , color_p1[0] , color_p1[1] , color_p1[2] , color_p1[3] , color_p2[0] , color_p2[1] , color_p2[2] , color_p2[3] , width );

  if( isLong ) {
    material->print( file , depth + 1 );
  }
  indent( file , depth );
  fprintf( file , "</line>\n" );
}

void dm_Line::display( DrawingMode mode , bool *invalidDL ) {
  if( width <= 0.0 ) {
    return;
  }

  //printf( "Draw line material\n" );
  // material->display( mode , invalidDL );
  //printf( "Draw line geometry\n" );


  float old_width = 10.0;
  glGetFloatv( GL_LINE_WIDTH, &old_width );
  
  glLineWidth( width );
  glBegin( GL_LINES );    
  ///////////////////////////
  // p1
  // printf( "dm_Line point1 hasColor %d\n" , material->hasColor );
  if( !material->hasMaterial ) {
    glColor4fv( color_p1 );
  }
  glVertex3f( p1.x , p1.y , p1.z );
  ///////////////////////////
  // p2
  // printf( "dm_Line point2 hasColor %d\n" , material->hasColor );
  if( !material->hasMaterial ) {
    glColor4fv( color_p2 );
  }
  glVertex3f( p2.x , p2.y , p2.z );
  glEnd();
  glLineWidth( old_width );

  // material->close( mode );
}

float dm_Line::height( void ) {
  return (_Max(p1.y, p2.y) - _Min(p1.y, p2.y)) / 2.0f;
}

void dm_Line::operator=(dm_Line& l) {
  this->dm_GeometricalObject::operator=(l);

  p1 = l.p1;
  p2 = l.p2;
  for( int ind  = 0 ; ind < 4 ; ind++ ) {
      color_p1[ind] = l.color_p1[ind];
	  color_p2[ind] = l.color_p2[ind];
  }
  width = l.width;
}

///////////////////////////////////////////////////////////
// STRIP
////////////////////////////////////////////////////////// 

dm_Strip::dm_Strip( void ):dm_GeometricalObject() {
  // default values
  tabFullVertices = NULL;
  tabVertices = NULL;
  tabTexturecoords = NULL;
  tabNormals = NULL;
  nbVertices = 0;
  nbTexCoords = 0;
  nbNormals = 0;
  type = EmptyStripType;
}

dm_Strip::~dm_Strip( void ) {
  // delete the buffers
  if (tabFullVertices) {
    delete [] tabFullVertices;
    tabFullVertices = NULL;
  }
  
  if (tabVertices) {
    delete [] tabVertices;
    tabVertices = NULL;
  }
  
  if (tabTexturecoords) {
    delete [] tabTexturecoords;
    tabTexturecoords = NULL;
  }
  
  if (tabNormals) {
    delete [] tabNormals;
    tabNormals = NULL;
  }
}

void dm_Strip::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    nbVertices = 0;
  }
  if( XMLtag->hasAttribute( "nbVertices" ) ) {
    operI( nbVertices , XMLtag->getAttributeIntValue( "nbVertices" , 
						      atRunTime ));
  }
  if( !atRunTime ) {
    nbTexCoords = 0;
  }
  if( XMLtag->hasAttribute( "nbTexCoords" ) ) {
    operI( nbTexCoords , XMLtag->getAttributeIntValue( "nbTexCoords" , 
						       atRunTime ));
  }
  if( !atRunTime ) {
    nbNormals = 0;
  }
  if( XMLtag->hasAttribute( "nbNormals" ) ) {
    operI( nbNormals , XMLtag->getAttributeIntValue( "nbNormals" , 
						      atRunTime ));
  }
  if( XMLtag->hasAttribute( "type" ) ) {
    char *render_str = XMLtag->getAttributeStringValue( "type" );
    type = EmptyStripType;
    for( int ind = 0 ; ind < EmptyStripType ; ind++ ) {
      if( strcmp( render_str , StripTypeString[ind] ) == 0 ) {
	type = (StripType)ind;
	break;
      }
    }
    if( type == EmptyStripType ) {
      sprintf( ErrorStr , "Error: unknown strip type [%s] in component ID [%s]!" , render_str , id ); ReportError( ErrorStr ); throw 203;
    }
  }

  if( !atRunTime ) {
    if( tabFullVertices ) {
      sprintf( ErrorStr , "Error: vertex table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabFullVertices = new MeshFullVertex[ nbVertices ];
    if( !tabFullVertices ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
      tabFullVertices[ indVertex ].X = 0.0;
      tabFullVertices[ indVertex ].Y = 0.0;
      tabFullVertices[ indVertex ].Z = 0.0;
    }

    if( tabVertices ) {
      sprintf( ErrorStr , "Error: vertex table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabVertices = new MeshVertex[ nbVertices ];
    if( !tabVertices ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
      tabVertices[ indVertex ].x = 0.0;
      tabVertices[ indVertex ].y = 0.0;
      tabVertices[ indVertex ].z = 0.0;
    }

    if( tabTexturecoords ) {
      sprintf( ErrorStr , "Error: texture coord table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabTexturecoords = new MeshTexturecoord[ nbTexCoords ];
    if( !tabTexturecoords ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indTexturecoord = 0 ; indTexturecoord < nbTexCoords ; indTexturecoord++ ) {
      tabTexturecoords[ indTexturecoord ].s0 = 0.0;
      tabTexturecoords[ indTexturecoord ].t0 = 0.0;
      tabTexturecoords[ indTexturecoord ].u0 = 0.0;
    }

    if( tabNormals ) {
      sprintf( ErrorStr , "Error: normal table already allocated in subdivision ID [%s]!" , id ); ReportError( ErrorStr ); throw 5;
    }
    tabNormals = new MeshNormal[ nbNormals ];
    if( !tabNormals ) {
      sprintf( ErrorStr , "Error: insufficient memory!" ); ReportError( ErrorStr ); throw 5;
    }
    for( int indNormal = 0 ; indNormal < nbNormals ; indNormal++ ) {
      tabNormals[ indNormal ].nx = 0.0;
      tabNormals[ indNormal ].ny = 0.0;
      tabNormals[ indNormal ].nz = 0.0;
    }
  }
}

bool dm_Strip::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "nbVertices" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbVertices;
    return true;
  }
  else if( strcmp( attribute , "nbTexCoords" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbVertices;
    return true;
  }
  else if( strcmp( attribute , "nbNormals" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbVertices;
    return true;
  }
  else if( strcmp( attribute , "strip_type" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)StripTypeString[type];
    return true;
  }
  return false;
}

void dm_Strip::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  while( true ) {
    // new tag XML : strip component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // strip closing tag
    if( strcmp( XMLtag->tag , "strip" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // subdivision initial vertices
    else if( strcmp( XMLtag->tag , "tabvertex" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabvertex\n");

      // parsing the coordinates of the initial vertices
      ParseTabVertex( fileDepth , p_c , tabVertices ,
		      nbVertices );
    }

    // subdivision initial texture coordinates
    else if( strcmp( XMLtag->tag , "tabtexturecoord" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabtexturecoord\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabTextureCoord( fileDepth , p_c , tabTexturecoords ,
			    nbTexCoords );
    }

    // subdivision initial normals
    else if( strcmp( XMLtag->tag , "tabnormal" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // printf("subdivision tabnormal\n");

      // parsing the coordinates of the initial texture coordinates
      ParseTabNormal( fileDepth , p_c , tabNormals ,
		      nbNormals , true );
    }

    // color
    else if( ( strcmp( XMLtag->tag , "color" ) == 0  
	       || strcmp( XMLtag->tag , "material" ) == 0  
	       || strcmp( XMLtag->tag , "shader" ) == 0  
	       || strcmp( XMLtag->tag , "texture" ) == 0   
	       || strcmp( XMLtag->tag , "video" ) == 0  )
	     && ((XMLtag->tagType == OpeningTag) || (XMLtag->tagType == EmptyTag)) ) {
      material->parse( fileDepth , p_c , id ,
		     XMLtag , NULL );
    }

    else {
      sprintf( ErrorStr , "Error: unknown strip component tag [%s] in strip ID [%s]!" , XMLtag->tag , id ); delete XMLtag; ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Strip::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<strip id=\"%s\" nbVertices=\"%d\" nbTexCoords=\"%d\" nbNormals=\"%d\" type=\"%s\">\n" , id , nbVertices , nbTexCoords , nbNormals , (char *)StripTypeString[type] );

  if( isLong ) {
    material->print( file , depth + 1 );

    // indent( file , depth + 1 );
    // fprintf( file , "<tabvertex size=\"%d\">\n" ,  nbVertices );
    // for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
    //   tabFullVertices[indVertex].print( file , depth + 2 , indVertex );
    // }
    // indent( file , depth + 1 );
    // fprintf( file , "</tabvertex>\n" );

    // indent( file , depth + 1 );
    // fprintf( file , "<tabtexturecoord size=\"%d\">\n" ,  nbVertices );
    // for( int indVertex = 0 ; indVertex < nbTexCoords ; indVertex++ ) {
    //   tabTexturecoords[indVertex].print( file , depth + 2 , indVertex );
    // }
    // indent( file , depth + 1 );
    // fprintf( file , "</tabtexturecoord>\n" );

    // indent( file , depth + 1 );
    // fprintf( file , "<tabnormal size=\"%d\">\n" ,  nbNormals );
    // for( int indNormal = 0 ; indNormal < nbNormals ; indNormal++ ) {
    //   indent( file , depth + 2 );
    //   fprintf( file , "<normal index=\"%d\" x=\"%.5f\" y=\"%.5f\" z=\"%.5f\" />\n" , indNormal + 1 , tabNormals[indNormal].x , tabNormals[indNormal].y , tabNormals[indNormal].z );
    // }
    // indent( file , depth + 1 );
    // fprintf( file , "</tabnormal>\n" );
  }
  indent( file , depth );
  fprintf( file , "</strip>\n" );
}

void dm_Strip::display( DrawingMode mode , bool *invalidDL ) {

  // material->display( mode , invalidDL );

  glPushMatrix();
    {
      //glLoadIdentity();
      // line strip
      // triangle strip
      // quad strip
      glDisable(GL_BLEND);
      glDisable(GL_COLOR_MATERIAL);
      glColor4f( 1 , 0 , 0 , 1 );
      glLineWidth( 1.0 );
      switch( type ) {
      case DM_LINE_STRIP :
	// printf("display line strip\n" );
	glBegin( GL_LINE_STRIP );
	for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
	  glVertex3f( tabFullVertices[ indVertex ].X , 
		      tabFullVertices[ indVertex ].Y , 
		      tabFullVertices[ indVertex ].Z);
	  // printf("point %.2f %.2f %.2f \n" ,
	  // 	 tabFullVertices[ indVertex ].x , 
	  // 	 tabFullVertices[ indVertex ].y , 
	  // 	 tabFullVertices[ indVertex ].z );
	}
	glEnd();
	break;
      case DM_TRIANGLE_STRIP :
	glBegin( GL_TRIANGLE_STRIP );
	for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
	  if( !material->hasTexture ) {
	    if( indVertex % 2 == 0 ) {
	      glTexCoord2f( material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s ,
			    material->materialTexture->textureOffset_t );
	    }
	    else {
	      glTexCoord2f( material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s ,
			    material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t );
	    }
	  }
	  glVertex3f( tabFullVertices[ indVertex ].X , 
		      tabFullVertices[ indVertex ].Y , 
		      tabFullVertices[ indVertex ].Z );
	}
	glEnd();
	break;
      case DM_QUAD_STRIP :
	glBegin( GL_QUAD_STRIP );
	for( int indVertex = 0 ; indVertex < nbVertices ; indVertex++ ) {
	  if( !material->hasTexture ) {
	    if( indVertex % 2 == 0 ) {
	      glTexCoord2f( material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s ,
			    material->materialTexture->textureOffset_t );
	    }
	    else {
	      glTexCoord2f( material->materialTexture->textureOffset_s + material->materialTexture->textureTile_s ,
			    material->materialTexture->textureOffset_t + material->materialTexture->textureTile_t );
	    }
	  }
	  glVertex3f( tabFullVertices[ indVertex ].X , 
		      tabFullVertices[ indVertex ].Y , 
		      tabFullVertices[ indVertex ].Z );
	}
	glEnd();
	break;
      default :
	break;
      }
    }
  glPopMatrix();

  // material->close( mode );
}

void dm_Strip::operator=(dm_Strip& c) {
  this->dm_GeometricalObject::operator=(c);

  nbVertices = c.nbVertices;
  nbTexCoords = c.nbTexCoords;
  nbNormals = c.nbNormals;
  type = c.type;
}

dm_TextureStorage::dm_TextureStorage( void ): dm_GeometricalObject() {
  TextureStorageScalarNode = NULL;
  TextureStorageScalar = NULL;
  textureGenerationMode = DM_BITMAP_TEXTURE;
  x0 = 0;
  y0 = 0;
  width = -1;
  height = -1;
}

dm_TextureStorage::~dm_TextureStorage( void ) {
  // TextureStorageScalarNode is not deallocated
  // will be deleted through releaseSceneObjects
  // TextureStorageScalar is not deallocated
  // will be deleted through releaseSceneObjects
}

///////////////////////////////////////////////////////////
// FBO
////////////////////////////////////////////////////////// 

void dm_TextureStorage::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime && XMLtag->hasAttribute( "xlink:href" ) ) {
    TextureStorageScalarNode = new dm_SceneNode(ANodeLink);
    StrCpy( &(TextureStorageScalarNode->id) , 
	    XMLtag->getAttributeStringValue( "xlink:href" ) );
  }

  if( XMLtag->hasAttribute( "x0" ) ) {
    operI( x0 , XMLtag->getAttributeIntValue( "x0" , 
						atRunTime ));
  }
  if( XMLtag->hasAttribute( "y0" ) ) {
    operI( y0 , XMLtag->getAttributeIntValue( "y0" , 
						atRunTime ));
  }
  if( XMLtag->hasAttribute( "width" ) ) {
    operI( width , XMLtag->getAttributeIntValue( "width" , 
						atRunTime ));
  }
  if( XMLtag->hasAttribute( "height" ) ) {
    operI( height , XMLtag->getAttributeIntValue( "height" , 
						atRunTime ));
  }

  if( !atRunTime ) {
    textureGenerationMode = DM_BITMAP_TEXTURE;
  } 
  if( XMLtag->hasAttribute( "type" ) ) {
    char *type_str = XMLtag->getAttributeStringValue( "type" );
    textureGenerationMode = EmptyTextureGenerationMode;
    for( int ind = 0 ; ind < EmptyTextureGenerationMode ; ind++ ) {
      //printf( "mode [%s]\n" , EnvModeString[ind] );
      if( strcmp( type_str , TextureGenerationModeString[ind] ) == 0 ) {
	textureGenerationMode = (TextureGenerationMode)ind;
	break;
      }
    }
    if( textureGenerationMode == EmptyTextureGenerationMode ) {
      sprintf( ErrorStr , "Error: unknown texture type [%s]!" , type_str ); ReportError( ErrorStr ); throw 127;
    }
  }
}

bool dm_TextureStorage::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "xlink:href" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = TextureStorageScalarNode->id;
    return true;
  }
  else if( strcmp( attribute , "x0" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = x0;
    return true;
  }
  else if( strcmp( attribute , "y0" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = y0;
    return true;
  }
  else if( strcmp( attribute , "width" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = width;
    return true;
  }
  else if( strcmp( attribute , "height" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = height;
    return true;
  }
  return false;
}

void dm_TextureStorage::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<texture_storage id=\"%s\" " , id );
  if( TextureStorageScalarNode ) {
    fprintf( file , 
	     "xlink:href=\"%s\" " , TextureStorageScalarNode->id );
  }
  fprintf( file , "type=\"%s\" />\n" , TextureGenerationModeString[ textureGenerationMode ] );
}

void dm_TextureStorage::display( DrawingMode mode , bool *invalidDL ) {
  // printf("TextureStorage: display\n");
  if( TextureStorageScalar ) {
    // builds a temporary texture with the attributes
    // given by the dm_TextureStorage scene object
    // and calls bindFrameBufferTexture to capture the frame buffer
    // printf("TextureStorage: display\n");
    // printf( "mode [%s]\n" , TextureGenerationModeString[textureGenerationMode] );
    dm_Texture *mat 
      = TextureStorageScalar->GetValue()->materialTexture;
    mat->textureGenerationMode = textureGenerationMode;
    if( width >= 0 && height >= 0 ) {
      mat->bindFrameBufferTexture( invalidDL , true , x0 , y0 , width , height );
    }
    else {
      mat->bindFrameBufferTexture( invalidDL , false , 0 , 0 , 0 , 0 );
    }
  }
}

void dm_TextureStorage::operator=(dm_TextureStorage& c) {
  this->dm_GeometricalObject::operator=(c);

  TextureStorageScalarNode = c.TextureStorageScalarNode;
  TextureStorageScalar = c.TextureStorageScalar;
  textureGenerationMode = c.textureGenerationMode;
}

dm_FBOStorage::dm_FBOStorage( void ): dm_GeometricalObject() {
  FBOStorageScalarNode = NULL;
  textureGenerationMode = DM_BITMAP_TEXTURE;
  depth_FBOStorageScalar = NULL;
  stencil_FBOStorageScalar = NULL;
  color_FBOStorageScalar = NULL;
  color_color[0] = 0;
  color_color[1] = 0;
  color_color[2] = 0;
  color_color[3] = 1;
  depth_color[0] = 0;
  depth_color[1] = 0;
  depth_color[2] = 0;
  depth_color[3] = 1;
  stencil_color[0] = 0;
  stencil_color[1] = 0;
  stencil_color[2] = 0;
  stencil_color[3] = 1;
  color_type = DM_NO_FBO_STORAGE;
  depth_type = DM_NO_FBO_STORAGE;
  stencil_type = DM_NO_FBO_STORAGE;
  color_id = NULL;
  depth_id = NULL;
  stencil_id = NULL;
  InitString( &color_id );
  InitString( &depth_id );
  InitString( &stencil_id );
  frameBufferId = 0;
}

dm_FBOStorage::~dm_FBOStorage( void ) {
  // FBOStorageScalarNode is not deallocated
  // will be deleted through releaseSceneObjects
  // FBOStorageScalar is not deallocated
  // will be deleted through releaseSceneObjects
  // printf("delete scene object\n" );
  // printf("delete scene object\n" );
  if (color_id)  {
    // printf("delete ID (%s)\n" , id );
    delete []color_id;
    color_id = NULL;
  }
  if (depth_id)  {
    // printf("delete ID (%s)\n" , id );
    delete []depth_id;
    depth_id = NULL;
  }
  // printf("delete scene object\n" );
  if (stencil_id)  {
    // printf("delete ID (%s)\n" , id );
    delete []stencil_id;
    stencil_id = NULL;
  }
}

void dm_FBOStorage::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime && XMLtag->hasAttribute( "xlink:href" ) ) {
    FBOStorageScalarNode = new dm_SceneNode(ANodeLink);
    StrCpy( &(FBOStorageScalarNode->id) , 
	    XMLtag->getAttributeStringValue( "xlink:href" ) );
    }

    if( !atRunTime ) {
      textureGenerationMode = DM_BITMAP_TEXTURE;
    } 
}

void dm_FBOStorage::setApplicationParameters( dm_XMLTag *XMLtag , bool atRunTime ,
 	            FloatBinOp oper ,
		    IntBinOp operI ) {
  /////////////////////////////////////////////////////////////////
  // depth component
  if( strcmp( XMLtag->tag , "depth" ) == 0 ) {
    if (XMLtag->hasAttribute("depth_r")){
      depth_color[0] = (float)XMLtag->getAttributeDoubleValue("depth_r", atRunTime);
    }
    if (XMLtag->hasAttribute("depth_g")) {
      depth_color[1] = (float)XMLtag->getAttributeDoubleValue("depth_g", atRunTime);
    }    
    if (XMLtag->hasAttribute("depth_b")) {
      depth_color[2] = (float)XMLtag->getAttributeDoubleValue("depth_b", atRunTime);
    }
    if (XMLtag->hasAttribute("depth_a")) {
      depth_color[3] = (float)XMLtag->getAttributeDoubleValue("depth_a", atRunTime);
    }
    if (XMLtag->hasAttribute("depth_cleared")) {
      XMLtag->BooleanFieldValue("depth_cleared", &depth_cleared);
    }
    if( !atRunTime ) {
      if( XMLtag->hasAttribute( "depth_storage" ) ) {
	char *type_str = XMLtag->getAttributeStringValue( "depth_storage" );
	depth_type = EmptyStorageType;
	for( int ind = 0 ; ind < EmptyStorageType ; ind++ ) {
	  if( strcmp( type_str , StorageTypeString[ind] ) == 0 ) {
	    depth_type = (StorageType)ind;
	    break;
	  }
	}
	if( depth_type == EmptyStorageType ) {
	  sprintf( ErrorStr , "Error: unknown depth_storage type [%s] !" , type_str ); ReportError( ErrorStr ); 
	  if( !atRunTime ) {
	  throw 203;
	  }
	  else {
	    depth_type = DM_NO_FBO_STORAGE;
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: missing depth_storage type on depth element of FBO_storage node [%s] !" , id ); ReportError( ErrorStr ); throw 0;
      }
    }
    if( XMLtag->hasAttribute( "depth_id"  ) ) {
      StrCpy( &depth_id , XMLtag->getAttributeStringValue( "depth_id" ) );
    }
  }

  /////////////////////////////////////////////////////////////////
  // color component
  else if( strcmp( XMLtag->tag , "color" ) == 0 ) {
    if (XMLtag->hasAttribute("color_r")){
      color_color[0] = (float)XMLtag->getAttributeDoubleValue("color_r", atRunTime);
    }
    if (XMLtag->hasAttribute("color_g")) {
      color_color[1] = (float)XMLtag->getAttributeDoubleValue("color_g", atRunTime);
    }    
    if (XMLtag->hasAttribute("color_b")) {
      color_color[2] = (float)XMLtag->getAttributeDoubleValue("color_b", atRunTime);
    }
    if (XMLtag->hasAttribute("color_a")) {
      color_color[3] = (float)XMLtag->getAttributeDoubleValue("color_a", atRunTime);
    }
    if (XMLtag->hasAttribute("color_cleared")) {
      XMLtag->BooleanFieldValue("color_cleared", &color_cleared);
    }
    if( !atRunTime ) {
      if( XMLtag->hasAttribute( "color_storage" ) ) {
	char *type_str = XMLtag->getAttributeStringValue( "color_storage" );
	color_type = EmptyStorageType;
	for( int ind = 0 ; ind < EmptyStorageType ; ind++ ) {
	  if( strcmp( type_str , StorageTypeString[ind] ) == 0 ) {
	    color_type = (StorageType)ind;
	    break;
	  }
	}
	if( color_type == EmptyStorageType ) {
	  sprintf( ErrorStr , "Error: unknown color_storage type [%s] !" , type_str ); ReportError( ErrorStr ); 
	  if( !atRunTime ) {
	  throw 203;
	  }
	  else {
	    color_type = DM_NO_FBO_STORAGE;
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: missing color_storage type on color element of FBO_storage node [%s] !" , id ); ReportError( ErrorStr ); throw 0;
      }
    }
    if( XMLtag->hasAttribute( "color_id"  ) ) {
      StrCpy( &color_id , XMLtag->getAttributeStringValue( "color_id" ) );
    }
  }

  /////////////////////////////////////////////////////////////////
  // stencil component
  else if( strcmp( XMLtag->tag , "stencil" ) == 0 ) {
    if (XMLtag->hasAttribute("stencil_r")){
      stencil_color[0] = (float)XMLtag->getAttributeDoubleValue("stencil_r", atRunTime);
    }
    if (XMLtag->hasAttribute("stencil_g")) {
      stencil_color[1] = (float)XMLtag->getAttributeDoubleValue("stencil_g", atRunTime);
    }    
    if (XMLtag->hasAttribute("stencil_b")) {
      stencil_color[2] = (float)XMLtag->getAttributeDoubleValue("stencil_b", atRunTime);
    }
    if (XMLtag->hasAttribute("stencil_a")) {
      stencil_color[3] = (float)XMLtag->getAttributeDoubleValue("stencil_a", atRunTime);
    }
    if (XMLtag->hasAttribute("stencil_cleared")) {
      XMLtag->BooleanFieldValue("stencil_cleared", &stencil_cleared);
    }
    if( !atRunTime ) {
      if( XMLtag->hasAttribute( "stencil_storage" ) ) {
	char *type_str = XMLtag->getAttributeStringValue( "stencil_storage" );
	stencil_type = EmptyStorageType;
	for( int ind = 0 ; ind < EmptyStorageType ; ind++ ) {
	  if( strcmp( type_str , StorageTypeString[ind] ) == 0 ) {
	    stencil_type = (StorageType)ind;
	    break;
	  }
	}
	if( stencil_type == EmptyStorageType ) {
	  sprintf( ErrorStr , "Error: unknown stencil_storage type [%s] !" , type_str ); ReportError( ErrorStr ); 
	  if( !atRunTime ) {
	  throw 203;
	  }
	  else {
	    stencil_type = DM_NO_FBO_STORAGE;
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: missing stencil_storage type on stencil element of FBO_storage node [%s] !" , id ); ReportError( ErrorStr ); throw 0;
      }
    }
    if( XMLtag->hasAttribute( "stencil_id"  ) ) {
      StrCpy( &stencil_id , XMLtag->getAttributeStringValue( "stencil_id" ) );
    }
  }

  ///////////// Unknown tag
  else {
    sprintf( ErrorStr , "Error: unknown FBOStorage component tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); throw 73;
    return;
  }
}

void dm_FBOStorage::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag   *XMLtag = new dm_XMLTag();

  // printf( "Parse FBO_storage \n" );
  while( *p_c != EOF ) {
    // new tag XML : user component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // user closing tag
    if( strcmp( XMLtag->tag , "FBO_storage" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      // printf( "End of user parsing \n" );
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    setApplicationParameters( XMLtag , false ,
			      assign , assignI );

  }
  delete XMLtag;
  XMLtag = NULL;
}

bool dm_FBOStorage::getParameters( char *attribute , 
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
  else if( strcmp( attribute , "xlink:href" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = FBOStorageScalarNode->id;
    return true;
  }
  else if( strcmp( attribute , "color_r" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_color[0];
    return true;
  }
  else if( strcmp( attribute , "color_g" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_color[1];
    return true;
  }
  else if( strcmp( attribute , "color_b" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_color[2];
    return true;
  }
  else if( strcmp( attribute , "color_a" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)color_color[3];
    return true;
  }
  else if( strcmp( attribute , "depth_r" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)depth_color[0];
    return true;
  }
  else if( strcmp( attribute , "depth_g" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)depth_color[1];
    return true;
  }
  else if( strcmp( attribute , "depth_b" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)depth_color[2];
    return true;
  }
  else if( strcmp( attribute , "depth_a" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)depth_color[3];
    return true;
  }
  else if( strcmp( attribute , "stencil_r" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)stencil_color[0];
    return true;
  }
  else if( strcmp( attribute , "stencil_g" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)stencil_color[1];
    return true;
  }
  else if( strcmp( attribute , "stencil_b" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)stencil_color[2];
    return true;
  }
  else if( strcmp( attribute , "stencil_a" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)stencil_color[3];
    return true;
  }
  return false;
}

void dm_FBOStorage::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<FBO_storage id=\"%s\" >" , id );
  indent( file , depth + 1 );
  fprintf( file , "<depth depth_storage=\"%s\" depth_cleared=\"%s\" depth_r=\"%.3f\" depth_g=\"%.3f\" depth_b=\"%.3f\" depth_a=\"%.3f\" \n" , 
	   StorageTypeString[depth_type] , BooleanString[BooleanToInt(depth_cleared)] , 
	   (float)depth_color[0] , (float)depth_color[1] , (float)depth_color[2] , (float)depth_color[3] );
  if( FBOStorageScalarNode ) {
    fprintf( file , 
	     "depth_id=\"%s\" xlink:href=\"%s\" " , depth_id , FBOStorageScalarNode->id );
  }
  fprintf( file , " />" );
  indent( file , depth + 1 );
  fprintf( file , "<color color_storage=\"%s\" color_cleared=\"%s\" color_r=\"%.3f\" color_g=\"%.3f\" color_b=\"%.3f\" color_a=\"%.3f\" \n" , 
	   StorageTypeString[color_type] , BooleanString[BooleanToInt(color_cleared)] , 
	   color_color[0] , color_color[1] , color_color[2] , color_color[3] );
  if( FBOStorageScalarNode ) {
    fprintf( file , 
	     "color_id=\"%s\" xlink:href=\"%s\" " , color_id , FBOStorageScalarNode->id );
  }
  fprintf( file , " />" );
  indent( file , depth + 1 );
  fprintf( file , "<stencil stencil_storage=\"%s\" stencil_cleared=\"%s\" stencil_r=\"%.3f\" stencil_g=\"%.3f\" stencil_b=\"%.3f\" stencil_a=\"%.3f\" \n" , 
	   StorageTypeString[stencil_type] , BooleanString[BooleanToInt(stencil_cleared)] , 
	   (float)stencil_color[0] , (float)stencil_color[1] , (float)stencil_color[2] , (float)stencil_color[3] );
  if( FBOStorageScalarNode ) {
    fprintf( file , 
	     "stencil_id=\"%s\" xlink:href=\"%s\" " , stencil_id , FBOStorageScalarNode->id );
  }
  fprintf( file , " />" );
  indent( file , depth );
  fprintf( file , "</FBO_storage>" );
}

void dm_FBOStorage::display( DrawingMode mode , bool *invalidDL ) {
  if( !(color_FBOStorageScalar || depth_FBOStorageScalar || stencil_FBOStorageScalar) ) {
    return;
  }

  if( !frameBufferId ) {
    // printf( "Get FBO ID %d \n" , frameBufferId );
    glGenFramebuffersEXT( 1, &frameBufferId );
    // printf( "Got FBO ID \n" );
    glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );

  
    if( color_FBOStorageScalar ) {

      // builds a temporary texture with the attributes
      // given by the dm_FBOStorage scene object
      // fprintf( fileLog , "FBOStorage: display\n" );
      // printf( "mode [%s]\n" , TextureGenerationModeString[textureGenerationMode] );
      dm_Texture *mat 
	= color_FBOStorageScalar->GetValue()->materialTexture;
      mat->textureGenerationMode = textureGenerationMode;
      mat->textureMode( invalidDL );
      // mat->close();

      // FBO texture binding
      // printf( "Bind color FBO ID %d w texture ID %d %s nb attachts %d\n" , frameBufferId  , DM_CompositionNode->GetTextureID( mat->GetTextureNo() ),DM_CompositionNode->texture_names[mat->GetTextureNo()], mat->nb_attachments);
      // glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );
      if( mat->rectangle ) {
	for( int ind = 0 ; ind < mat->nb_attachments ; ind++ ) {
	  glFramebufferTexture2D( GL_FRAMEBUFFER, 
				  GL_COLOR_ATTACHMENT0 + ind ,
				  GL_TEXTURE_RECTANGLE,
				  DM_CompositionNode->GetTextureID( 
								   mat->GetTextureNo() + ind ) , 
				  0 );
	  if(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	    printf("Error while Binding RECT FBO color %d texture texture No %d ID %d (error %d)\n" , frameBufferId , mat->GetTextureNo() + ind , DM_CompositionNode->GetTextureID( mat->GetTextureNo() + ind ) , glCheckFramebufferStatus(GL_FRAMEBUFFER));
	  // else
	  // printf("RECT FBO color binding succesfull\n");
	}
      }
      else {
	for( int ind = 0 ; ind < mat->nb_attachments ; ind++ ) {
	  glFramebufferTexture2D( GL_FRAMEBUFFER, 
				  GL_COLOR_ATTACHMENT0 + ind ,
				  GL_TEXTURE_2D,
				  DM_CompositionNode->GetTextureID( 
								   mat->GetTextureNo() + ind ) , 
				  0 );
	  if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	    printf("Error while Binding 2D FBO color %d texture texture No %d ID %d (error %d)\n" , frameBufferId , mat->GetTextureNo() + ind , DM_CompositionNode->GetTextureID( mat->GetTextureNo() + ind ) , glCheckFramebufferStatus(GL_FRAMEBUFFER));
	  // else
	  // printf("2D FBO color binding succesfull\n");
	}
      }

      // Setup draw buffers
      if( mat->nb_attachments > 1 ) {
	GLuint drawBuffers[16] = {
	  GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
	  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
	  GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
	  GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
	  GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
	  GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
	  GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
	  GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
	};
	glDrawBuffers(mat->nb_attachments, drawBuffers);
      }
      // glEnable(GL_BLEND);
      // glBlendFunc(GL_SRC_COLOR, GL_ZERO); 
      // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND); 
      // glClearColor(0,0,0,1);
    }
    if( depth_FBOStorageScalar ) {

      glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );

      // builds a temporary texture with the attributes
      // given by the dm_FBOStorage scene object
      // printf("FBOStorage: display\n");
      // printf( "mode [%s]\n" , TextureGenerationModeString[textureGenerationMode] );
      dm_Texture *mat 
	= depth_FBOStorageScalar->GetValue()->materialTexture;
      mat->textureGenerationMode = textureGenerationMode;
      mat->textureMode( invalidDL );
      // mat->close();

      // FBO texture binding
      // printf( "Bind depth FBO %d %s\n" , DM_CompositionNode->GetTextureID( mat->GetTextureNo() ),DM_CompositionNode->texture_names[mat->GetTextureNo()]);
      // printf( "Bind depth FBO %d\n" , mat->rectangle);
      // glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );
      if( mat->rectangle ) {
	glFramebufferTexture2D( GL_FRAMEBUFFER, 
				GL_DEPTH_ATTACHMENT ,
				GL_TEXTURE_RECTANGLE,
				DM_CompositionNode->GetTextureID( 
								 mat->GetTextureNo() ) , 
				0 );
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
	  printf("Error while Binding FBO depth texture\n");
	// else
	// printf("FBO depth  binding succesfull\n");
      }
      else {
	glFramebufferTexture2D( GL_FRAMEBUFFER, 
				GL_DEPTH_ATTACHMENT ,
				GL_TEXTURE_2D,
				DM_CompositionNode->GetTextureID( 
								 mat->GetTextureNo() ) , 
				0 );
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	  printf("Error while Binding FBO depth texture\n");
	// else
	// printf("FBO depth binding succesfull\n");
      }
    }
    if( stencil_FBOStorageScalar ) {
      // builds a temporary texture with the attributes
      // given by the dm_FBOStorage scene object
      // printf("FBOStorage: display\n");
      // printf( "mode [%s]\n" , TextureGenerationModeString[textureGenerationMode] );
      dm_Texture *mat 
	= stencil_FBOStorageScalar->GetValue()->materialTexture;
      mat->textureGenerationMode = textureGenerationMode;
      mat->textureMode( invalidDL );
      // mat->close();

      // FBO texture binding
      // printf( "Bind stencil FBO %d %s\n" , DM_CompositionNode->GetTextureID( mat->GetTextureNo() ),DM_CompositionNode->texture_names[mat->GetTextureNo()]);
      glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );
      if( mat->rectangle ) {
	glFramebufferTexture2D( GL_FRAMEBUFFER, 
				GL_STENCIL_ATTACHMENT ,
				GL_TEXTURE_RECTANGLE,
				DM_CompositionNode->GetTextureID( 
								 mat->GetTextureNo() ) , 
				0 );
      }
      else {
	glFramebufferTexture2D( GL_FRAMEBUFFER, 
				GL_STENCIL_ATTACHMENT ,
				GL_TEXTURE_2D,
				DM_CompositionNode->GetTextureID( 
								 mat->GetTextureNo() ) , 
				0 );
      }

      if( stencil_cleared ) {
	glClearColor(stencil_color[0],stencil_color[1],stencil_color[2],stencil_color[3]);
	glClear(GL_STENCIL_BUFFER_BIT);
      }
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
      printf(">> FBO binding error -> NOT using FBO !\n");
  }
  else {
    // printf( "Got FBO ID \n" );
    glBindFramebuffer( GL_FRAMEBUFFER, frameBufferId );
  }

  if( color_FBOStorageScalar ) 
    if( color_cleared ) {
      glClearColor(color_color[0],color_color[1],color_color[2],color_color[3]);
      glClear(GL_COLOR_BUFFER_BIT);
    }
  if( depth_FBOStorageScalar ) 
    if( depth_cleared ) {
      glClearColor(depth_color[0],depth_color[1],depth_color[2],depth_color[3]);
      glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void dm_FBOStorage::operator=(dm_FBOStorage& c) {
  this->dm_GeometricalObject::operator=(c);

  FBOStorageScalarNode = c.FBOStorageScalarNode;
  textureGenerationMode = c.textureGenerationMode;
  color_FBOStorageScalar = c.color_FBOStorageScalar;
  depth_FBOStorageScalar = c.depth_FBOStorageScalar;
  stencil_FBOStorageScalar = c.stencil_FBOStorageScalar;
  color_color[0] = c.color_color[0];
  color_color[1] = c.color_color[1];
  color_color[2] = c.color_color[2];
  color_color[3] = c.color_color[3];
  depth_color[0] = c.depth_color[0];
  depth_color[1] = c.depth_color[1];
  depth_color[2] = c.depth_color[2];
  depth_color[3] = c.depth_color[3];
  stencil_color[0] = c.stencil_color[0];
  stencil_color[1] = c.stencil_color[1];
  stencil_color[2] = c.stencil_color[2];
  stencil_color[3] = c.stencil_color[3];
  color_type = c.color_type;
  depth_type = c.depth_type;
  stencil_type = c.stencil_type;
  color_id = c.color_id;
  depth_id = c.depth_id;
  stencil_id = c.stencil_id;
  frameBufferId = c.frameBufferId;
}

dm_FBDisplay::dm_FBDisplay( void ): dm_GeometricalObject() {
}

dm_FBDisplay::~dm_FBDisplay( void ) {
}

void dm_FBDisplay::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }
}

bool dm_FBDisplay::getParameters( char *attribute , 
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
  return false;
}

void dm_FBDisplay::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<FB_display id=\"%s\" " , id );
  fprintf( file , " />\n" );
}

void dm_FBDisplay::display( DrawingMode mode , bool *invalidDL ) {
  //
  // Unbind the frame-buffer objects.
  //
  
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  // fprintf( fileLog , "Unbind FBO\n" );
}

void dm_FBDisplay::operator=(dm_FBDisplay& c) {
  this->dm_GeometricalObject::operator=(c);
}


dm_PixelReader::dm_PixelReader( void ): dm_GeometricalObject() {
  x = 0;
  y = 0;
  size_x = 1;
  size_y = 1;
  offset_x = 0;
  offset_y = 0;
}

dm_PixelReader::~dm_PixelReader( void ) {
}

void dm_PixelReader::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper ,
				  IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id"  ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  if( XMLtag->hasAttribute( "x" ) ) {
    operI( x , XMLtag->getAttributeIntValue( "x" , 
					     atRunTime ));
  }
  if( XMLtag->hasAttribute( "y" ) ) {
    operI( y , XMLtag->getAttributeIntValue( "y" , 
					     atRunTime ));
  }

  if( XMLtag->hasAttribute( "size_x" ) ) {
    operI( size_x , XMLtag->getAttributeIntValue( "size_x" , 
					     atRunTime ));
  }
  if( XMLtag->hasAttribute( "size_y" ) ) {
    operI( size_y , XMLtag->getAttributeIntValue( "size_y" , 
					     atRunTime ));
  }

  if( XMLtag->hasAttribute( "offset_x" ) ) {
    operI( offset_x , XMLtag->getAttributeIntValue( "offset_x" , 
					     atRunTime ));
  }
  if( XMLtag->hasAttribute( "offset_y" ) ) {
    operI( offset_y , XMLtag->getAttributeIntValue( "offset_y" , 
					     atRunTime ));
  }

  if( size_x * size_y != DM_EnvironmentNode->frame_color_r_table->GetSize() )  {
    DM_EnvironmentNode->frame_color_r_table->nextVariable = NULL;
    delete DM_EnvironmentNode->frame_color_r_table;
    DM_EnvironmentNode->frame_color_r_table = new dm_Table(size_x * size_y);
    DM_EnvironmentNode->frame_color_r_table->SetId( "frame_color_r" );
    DM_EnvironmentNode->pixel_color_b_scalar->nextVariable 
      = DM_EnvironmentNode->frame_color_r_table;

    DM_EnvironmentNode->frame_color_g_table->nextVariable = NULL;
    delete DM_EnvironmentNode->frame_color_g_table;
    DM_EnvironmentNode->frame_color_g_table = new dm_Table(size_x * size_y);
    DM_EnvironmentNode->frame_color_g_table->SetId( "frame_color_g" );
    DM_EnvironmentNode->frame_color_r_table->nextVariable 
      = DM_EnvironmentNode->frame_color_g_table;

    DM_EnvironmentNode->frame_color_b_table->nextVariable = NULL;
    delete DM_EnvironmentNode->frame_color_b_table;
    DM_EnvironmentNode->frame_color_b_table = new dm_Table(size_x * size_y);
    DM_EnvironmentNode->frame_color_b_table->SetId( "frame_color_b" );
    DM_EnvironmentNode->frame_color_g_table->nextVariable 
      = DM_EnvironmentNode->frame_color_b_table;
    DM_EnvironmentNode->frame_color_b_table->nextVariable 
      =  DM_EnvironmentNode->window_width_scalar;
  }
    
}

bool dm_PixelReader::getParameters( char *attribute , 
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
    *dataType = DataTypeInt;
    *valInt = x;
    return true;
  }
  else if( strcmp( attribute , "y" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = y;
    return true;
  }
  else if( strcmp( attribute , "size_x" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = size_x;
    return true;
  }
  else if( strcmp( attribute , "size_y" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = size_y;
    return true;
  }
  else if( strcmp( attribute , "offset_x" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = offset_x;
    return true;
  }
  else if( strcmp( attribute , "offset_y" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = offset_y;
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////
// MOUSE READING OF PIXEL
////////////////////////////////////////////////////////// 

void dm_PixelReader::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , 
	   "<pixel_reader id=\"%s\" " , id );
  fprintf( file , "x=\"%d\" y=\"%d size_x=\"%d\" size_y=\"%d offset_x=\"%d\" offset_y=\"%d \" />\n" , x , y , size_x , size_y , offset_x , offset_y );
}

void dm_PixelReader::display( DrawingMode mode , bool *invalidDL ) {
  if( size_x * size_y > 0 ) {
    GLubyte * pixelColor 
      = (GLubyte *) malloc( sizeof(GLubyte) * 3 * size_x * size_y );
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if( !pixelColor ) {
      sprintf( ErrorStr , "Error: pixel table allocation error (size %d)!" , 3 * size_x * size_y ); ReportError( ErrorStr ); throw 16;
    }

    glReadPixels( x + offset_x, 
    		  CurrentWindow->getWindowHeight() - y + offset_y, 
    		  size_x, size_y, 
    		  GL_RGB, GL_UNSIGNED_BYTE, pixelColor );

    DM_EnvironmentNode->pixel_color_r_scalar
      ->SetValue( (float)(pixelColor[ 0 ])/255.0F );
    DM_EnvironmentNode->pixel_color_g_scalar
      ->SetValue( (float)(pixelColor[ 1 ])/255.0F );
    DM_EnvironmentNode->pixel_color_b_scalar
      ->SetValue( (float)(pixelColor[ 2 ])/255.0F );
    for( int ind = 0 ; ind < size_x * size_y * 3 ; ind += 3 ) {
      DM_EnvironmentNode->frame_color_r_table
    	->SetValue( (float)(pixelColor[ ind  ])/255.0F , ind / 3 );
      DM_EnvironmentNode->frame_color_g_table
    	->SetValue( (float)(pixelColor[ ind  + 1 ])/255.0F , ind / 3 );
      DM_EnvironmentNode->frame_color_b_table
    	->SetValue( (float)(pixelColor[ ind  + 2 ])/255.0F , ind / 3 );
      // printf( "pixel=%d  %.3f %.3f %.3f\n" , 
      // 	      ind/3, 
      // 	      (float)(pixelColor[ ind  ])/255.0F ,
      // 	      (float)(pixelColor[ ind +1 ])/255.0F , 
      // 	      (float)(pixelColor[ ind  + 2])/255.0F );
    }
    // printf( "color at X=%d Y=%d size %dx%d offset %dx%d %.3f %.3f %.3f\n" , 
    // 	    x  + offset_x, y + offset_y,
    // 	    size_x , size_y ,
    // 	    offset_x , offset_y ,
    // 	    (float)(pixelColor[ 0 ])/255.0 , 
    // 	    (float)(pixelColor[ 1 ])/255.0 , 
    // 	    (float)(pixelColor[ 2 ])/255.0 );
   
    delete [] pixelColor;
  }
}

void dm_PixelReader::operator=(dm_PixelReader& c) {
  this->dm_GeometricalObject::operator=(c);

  x = c.x;
  y = c.y;
  size_x = c.size_x;
  size_y = c.size_y;
  offset_x = c.offset_x;
  offset_y = c.offset_y;
}

