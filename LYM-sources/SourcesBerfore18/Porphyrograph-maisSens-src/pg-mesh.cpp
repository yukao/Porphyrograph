/*! \file pg-mesh.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 *
 *     File pg-mesh.cpp
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
// geometry of mesh
string meshId[PG_NB_MAX_MESH];
int nbpointsMesh[PG_NB_MAX_MESH];
int nbtexCoordsMesh[PG_NB_MAX_MESH];
int NbFacesMesh[PG_NB_MAX_MESH]; // nb faces of mesh
int NbMesh = 0;

////////////////////////////////////////
// geometrical data of mesh
#define MAX_FACES 20000

GLfloat *pointBuffer[PG_NB_MAX_MESH] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
GLfloat *texCoordBuffer[PG_NB_MAX_MESH] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
GLuint  *indexBuffer[PG_NB_MAX_MESH] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
// shader variable pointers
GLint uniform_mesh_model[PG_NB_MAX_MESH];
GLint uniform_mesh_view[PG_NB_MAX_MESH];
GLint uniform_mesh_proj[PG_NB_MAX_MESH];
GLint uniform_mesh_light[PG_NB_MAX_MESH];

// mesh vertex array object for rendering
unsigned int mesh_vao[PG_NB_MAX_MESH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// mesh vertex indices for rendering
unsigned int mesh_index_vbo[PG_NB_MAX_MESH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//////////////////////////////////////////////////////////////////
// MESH AND KEYPOINT FILE PARSING
//////////////////////////////////////////////////////////////////

void copyMeshData(  int NbMesh , GLfloat *pointBufferIni , GLfloat *texCoordBufferIni ,  
					GLuint  *indexpointBufferIni , GLuint  *indextexCoordBufferIni ) {
  	pointBuffer[NbMesh] = (GLfloat *)malloc( NbFacesMesh[NbMesh] * 3 * 3 * sizeof *pointBuffer);
	texCoordBuffer[NbMesh] = (GLfloat *)malloc( NbFacesMesh[NbMesh] * 3 * 2 * sizeof *texCoordBuffer);
	indexBuffer[NbMesh] = (GLuint *)malloc( NbFacesMesh[NbMesh] * 3 * sizeof *indexBuffer);
	for( int indFace = 0 ; indFace < NbFacesMesh[NbMesh] ; indFace++ ) {
	  // copies the indices in the index table
	  indexBuffer[NbMesh][ indFace * 3 ] = indFace * 3;
	  indexBuffer[NbMesh][ indFace * 3 + 1 ] = indFace * 3 + 1;
	  indexBuffer[NbMesh][ indFace * 3 + 2 ] = indFace * 3 + 2;

	  // copies the texCoords from the initial buffer to the final one
	  // pointBufferIniPtr should be used because pointBufferIni has been
	  // incremented during texCoords scanning
	  int indVertex1 = indexpointBufferIni[ indFace * 3 ];
	  int indVertex2 = indexpointBufferIni[ indFace * 3 + 1 ];
	  int indVertex3 = indexpointBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indPoint %d %d %d %d\n" , indFace ,indVertex1,indVertex2,indVertex3);
	  pointBuffer[NbMesh][ indFace * 3 * 3 ]     = pointBufferIni[ indVertex1 * 3 ];
	  pointBuffer[NbMesh][ indFace * 3 * 3 + 1 ] = pointBufferIni[ indVertex1 * 3 + 1 ];
	  pointBuffer[NbMesh][ indFace * 3 * 3 + 2 ] = pointBufferIni[ indVertex1 * 3 + 2 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 ] = pointBufferIni[ indVertex2 * 3 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 + 1 ] = pointBufferIni[ indVertex2 * 3 + 1 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 + 2 ] = pointBufferIni[ indVertex2 * 3 + 2 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 ] = pointBufferIni[ indVertex3 * 3 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 + 1 ] = pointBufferIni[ indVertex3 * 3 + 1 ];
	  pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 + 2 ] = pointBufferIni[ indVertex3 * 3 + 2 ];
	 // printf("Face point %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		//pointBuffer[NbMesh][ indFace * 3 * 3 ] , pointBuffer[NbMesh][ indFace * 3 * 3 + 1 ] , pointBuffer[NbMesh][ indFace * 3 * 3 + 2 ] ,
		//pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 ] , pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 + 1 ] , pointBuffer[NbMesh][ (indFace * 3 + 1) * 3 + 2 ] ,
		//pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 ] , pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 + 1 ] , pointBuffer[NbMesh][ (indFace * 3 + 2) * 3 + 2 ] );

	  int indtexCoord1 = indextexCoordBufferIni[ indFace * 3 ];
	  int indtexCoord2 = indextexCoordBufferIni[ indFace * 3 + 1 ];
	  int indtexCoord3 = indextexCoordBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indTexC %d %d %d %d\n" , indFace ,indtexCoord1,indtexCoord2,indtexCoord3);
	  texCoordBuffer[NbMesh][ indFace * 3 * 2 ]     = texCoordBufferIni[ indtexCoord1 * 2 ];
	  texCoordBuffer[NbMesh][ indFace * 3 * 2 + 1 ] = texCoordBufferIni[ indtexCoord1 * 2 + 1 ];
	  texCoordBuffer[NbMesh][ (indFace * 3 + 1) * 2 ] = texCoordBufferIni[ indtexCoord2 * 2 ];
	  texCoordBuffer[NbMesh][ (indFace * 3 + 1) * 2 + 1 ] = texCoordBufferIni[ indtexCoord2 * 2 + 1 ];
	  texCoordBuffer[NbMesh][ (indFace * 3 + 2) * 2 ] = texCoordBufferIni[ indtexCoord3 * 2 ];
	  texCoordBuffer[NbMesh][ (indFace * 3 + 2) * 2 + 1 ] = texCoordBufferIni[ indtexCoord3 * 2 + 1 ];
	 // printf("Face texC %d %.2f/%.2f %.2f/%.2f %.2f/%.2f  \n" , indFace ,
		//texCoordBuffer[NbMesh][ indFace * 3 * 2  ] , texCoordBuffer[NbMesh][ indFace * 3 * 2 + 1 ] ,
		//texCoordBuffer[NbMesh][ (indFace * 3 + 1) * 2 ] , texCoordBuffer[NbMesh][ (indFace * 3 + 1) * 2 + 1 ] ,
		//texCoordBuffer[NbMesh][ (indFace * 3 + 2) * 2 ] , texCoordBuffer[NbMesh][ (indFace * 3 + 2) * 2 + 1 ]  );
	}
}

// OBJ file parsing (Alias Wavefront ASCII format)
void parse_mesh_obj( FILE *file )
{
  char    tag[256];
  char    meshString[256];
  char    matId[256];
  char    line[256];

  GLfloat *pointBufferIni = NULL;
  GLfloat *texCoordBufferIni = NULL;
  GLuint  *indexpointBufferIni = NULL;
  GLuint  *indextexCoordBufferIni = NULL;
  
  // vertex positions and texCoords
  pointBufferIni = (GLfloat *)malloc( MAX_FACES * 3 * 3 * sizeof *pointBufferIni);
  texCoordBufferIni = (GLfloat *)malloc( MAX_FACES * 3 * 2 * sizeof *texCoordBufferIni);
  indextexCoordBufferIni = (GLuint *)malloc( MAX_FACES * 3 * sizeof *indextexCoordBufferIni);
  indexpointBufferIni = (GLuint *)malloc( MAX_FACES * 3 * sizeof *indexpointBufferIni);

  // Two comment lines
  // # Blender3D v244 OBJ File: Anime_Girl.blend
  // # www.blender3d.org
  if( !fgets  ( line, 256, file ) ) { return; }
  if( !fgets  ( line, 256, file ) ) { return; }
  
  // material name
  if( !fgets  ( line, 256, file ) ) { return; }
  sscanf ( line, "%s %s", 
	   tag, matId );
  
  // mesh ID
  if( !fgets  ( line, 256, file ) ) { return; }
  sscanf ( line, "%s", tag );
  
  NbMesh = 0;
  int nbVertexTot = 0;
  int nbCoordTexTot = 0;
  while( strcmp( tag , "o" ) == 0 ) {
    if( NbMesh >= PG_NB_MAX_MESH ) {
      printf( "Error: Excessive number of Meshes, max %d\n" , PG_NB_MAX_MESH );
      throw 0;
    }

	nbpointsMesh[NbMesh] = 0;
	nbtexCoordsMesh[NbMesh] = 0;
	NbFacesMesh[NbMesh] = 0;

    // mesh ID
    sscanf ( line, "%s %s", tag , meshString );
	meshId[NbMesh] = string( meshString );
    
    // next tag
    if( !fgets  ( line, 256, file ) ) { return; }
    sscanf ( line, "%s", tag );
  
    // Scan for Verts in this mesh
    while( strcmp( tag , "v" ) == 0 ) {
      if( nbpointsMesh[NbMesh] > MAX_FACES * 3 ) {
		  printf( "Error: Excessive number of vertices\n" );
		  throw 0;
      }
      sscanf ( line, "%s %f %f %f", 
	       tag, &pointBufferIni[nbpointsMesh[NbMesh] * 3] ,
		   &pointBufferIni[nbpointsMesh[NbMesh] * 3 + 1] , 
		   &pointBufferIni[nbpointsMesh[NbMesh] * 3 + 2] );
      nbpointsMesh[NbMesh]++;

      if( !fgets  ( line, 256, file ) ) { return; }
      sscanf ( line, "%s", tag );
    }
    
    // Scan for text coords in this mesh
    while( strcmp( tag , "vt" ) == 0 ) {
      if( nbtexCoordsMesh[NbMesh] > MAX_FACES * 3 ) {
		  printf( "Error: Excessive number of texCoords\n" );
		  throw 0;
      }
      sscanf ( line, "%s %f %f", 
	       tag , &texCoordBufferIni[nbtexCoordsMesh[NbMesh] * 2] ,
		   &texCoordBufferIni[nbtexCoordsMesh[NbMesh] * 2 + 1] );
	  //printf("vt %d %.2f %.2f\n",nbtexCoordsMesh[NbMesh],texCoordBufferIni[nbtexCoordsMesh[NbMesh] * 2],
			//	texCoordBufferIni[nbtexCoordsMesh[NbMesh] * 2+1]);
      nbtexCoordsMesh[NbMesh]++;

      if( !fgets  ( line, 256, file ) ) { return; }
      sscanf ( line, "%s", tag );
	}
    
    // Scan for Mat in this mesh
    if( strcmp( tag , "usemtl" ) == 0 ) {
      sscanf ( line, "%s %s", 
	       tag , matId );
      if( !fgets  ( line, 256, file ) ) { return; }
      sscanf ( line, "%s", tag );
    }
    
    // Scan for Faces in this mesh
    while( strcmp( tag , "f" ) == 0 
	   || strcmp( tag , "usemtl" ) == 0
	   || strcmp( tag , "s" ) == 0 ) {
      if( NbFacesMesh[NbMesh] > MAX_FACES ) {
		printf( "Error: Excessive number of faces\n" );
		throw 0;
      }

      // Scan for Mat in this mesh
      // currently only one mat per mesh
      if( strcmp( tag , "usemtl" ) == 0 ) {
		sscanf ( line, "%s", matId );
      }
      // Scan for Smooth boolean in this mesh
      else if( strcmp( tag , "s" ) == 0 ) {
		sscanf ( line, "%s", tag );
      }
      // Scan for a Face in this mesh
      else {
		int indVertex1 , indVertex2 , indVertex3;
		int indtexCoord1 , indtexCoord2 , indtexCoord3;
		sscanf( line, "%s %d/%d %d/%d %d/%d", 
			tag, &indVertex1 , &indtexCoord1 , 
			&indVertex2 , &indtexCoord2 , &indVertex3 , &indtexCoord3 );
		// indices start from 1 in OBJ format
		// we make start from 0 for C++
		indVertex1--;
		indVertex2--;
		indVertex3--;
		indtexCoord1--;
		indtexCoord2--;
		indtexCoord3--;

		//printf( "%s indexpointBufferIni %d/%d %d/%d %d/%d\n", 
		//	line, indVertex1 , indtexCoord1 , 
		//	indVertex2 , indtexCoord2 , indVertex3 , indtexCoord3 );

		// copies the texCoords from the initial buffer to the final one
		// pointBufferIniPtr should be used because pointBufferIni has been
		// incremented during texCoords scanning
		indexpointBufferIni[ NbFacesMesh[NbMesh] * 3 ]     = indVertex1 - nbVertexTot;
		indexpointBufferIni[ NbFacesMesh[NbMesh] * 3 + 1 ] = indVertex2 - nbVertexTot;
		indexpointBufferIni[ NbFacesMesh[NbMesh] * 3 + 2 ] = indVertex3 - nbVertexTot;

		indextexCoordBufferIni[ NbFacesMesh[NbMesh] * 3 ]     = indtexCoord1 - nbCoordTexTot;
		indextexCoordBufferIni[ NbFacesMesh[NbMesh] * 3 + 1 ] = indtexCoord2 - nbCoordTexTot;
		indextexCoordBufferIni[ NbFacesMesh[NbMesh] * 3 + 2 ] = indtexCoord3 - nbCoordTexTot;

		NbFacesMesh[NbMesh]++;
	  }

	  if( !fgets  ( line, 256, file ) ) {
			copyMeshData( NbMesh , pointBufferIni , texCoordBufferIni ,  
					indexpointBufferIni , indextexCoordBufferIni);
			nbVertexTot += nbpointsMesh[NbMesh];
			nbCoordTexTot += nbtexCoordsMesh[NbMesh];
			nbpointsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
			nbtexCoordsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
			printf( "Final Mesh %d Vertices %d texCoords %d Faces %d\n" , 
				NbMesh , nbpointsMesh[NbMesh] , nbtexCoordsMesh[NbMesh] , NbFacesMesh[NbMesh] );
			//for( int indF = 0 ; indF < NbFacesMesh[NbMesh] * 3 ;  indF++ ) {
			//    printf("indices %d %d %d\n" , indexBuffer[3*indF] , indexBuffer[3*indF+1] , indexBuffer[3*indF+2] );
			//    printf("vertices %.2f %.2f %.2f        %.2f %.2f %.2f        %.2f %.2f %.2f\n" , 
			//	pointBuffer[ indexBuffer[3*indF] * 3 ] , pointBuffer[ indexBuffer[3*indF] * 3 + 1] , pointBuffer[ indexBuffer[3*indF] * 3 + 2],
			//	pointBuffer[ indexBuffer[3*indF+1] * 3 ] , pointBuffer[ indexBuffer[3*indF+1] * 3 + 1] , pointBuffer[ indexBuffer[3*indF+1] * 3 + 2],
			//	pointBuffer[ indexBuffer[3*indF+2] * 3 ] , pointBuffer[ indexBuffer[3*indF+2] * 3 + 1] , pointBuffer[ indexBuffer[3*indF+2] * 3 + 2]);
			//    printf("texCoords %.2f %.2f         %.2f %.2f        %.2f %.2f\n\n" , 
			//	texCoordBuffer[ indexBuffer[3*indF] * 2 ] , texCoordBuffer[ indexBuffer[3*indF] * 2 + 1],
			//	texCoordBuffer[ indexBuffer[3*indF+1] * 2 ] , texCoordBuffer[ indexBuffer[3*indF+1] * 2 + 1],
			//	texCoordBuffer[ indexBuffer[3*indF+2] * 2 ] , texCoordBuffer[ indexBuffer[3*indF+2] * 2 + 1]);
			//}
			NbMesh++;

			free(pointBufferIni);
			free(texCoordBufferIni);
			free(indexpointBufferIni);
			free(indextexCoordBufferIni);

			return;
	  }

	  // printf("%s\n", line);
	  sscanf ( line, "%s", tag );
	  if( !tag ) {
			copyMeshData(NbMesh , pointBufferIni , texCoordBufferIni ,  
					indexpointBufferIni , indextexCoordBufferIni);
			nbVertexTot += nbpointsMesh[NbMesh];
			nbCoordTexTot += nbtexCoordsMesh[NbMesh];
			nbpointsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
			nbtexCoordsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
			// printf ( "%s", tag );
			printf( "Final Mesh %d Vertices %d texCoords %d Faces %d\n" , 
				NbMesh , nbpointsMesh[NbMesh] , nbtexCoordsMesh[NbMesh] , NbFacesMesh[NbMesh] );
			//for( int indF = 0 ; indF < NbFacesMesh[NbMesh] * 3 ;  indF++ ) {
			//    printf("indices %d %d %d\n" , indexBuffer[3*indF] , indexBuffer[3*indF+1] , indexBuffer[3*indF+2] );
			//    printf("vertices %.2f %.2f %.2f        %.2f %.2f %.2f        %.2f %.2f %.2f\n" , 
			//	pointBuffer[ indexBuffer[3*indF] * 3 ] , pointBuffer[ indexBuffer[3*indF] * 3 + 1] , pointBuffer[ indexBuffer[3*indF] * 3 + 2],
			//	pointBuffer[ indexBuffer[3*indF+1] * 3 ] , pointBuffer[ indexBuffer[3*indF+1] * 3 + 1] , pointBuffer[ indexBuffer[3*indF+1] * 3 + 2],
			//	pointBuffer[ indexBuffer[3*indF+2] * 3 ] , pointBuffer[ indexBuffer[3*indF+2] * 3 + 1] , pointBuffer[ indexBuffer[3*indF+2] * 3 + 2]);
			//    printf("texCoords %.2f %.2f         %.2f %.2f        %.2f %.2f\n\n" , 
			//	texCoordBuffer[ indexBuffer[3*indF] * 2 ] , texCoordBuffer[ indexBuffer[3*indF] * 2 + 1],
			//	texCoordBuffer[ indexBuffer[3*indF+1] * 2 ] , texCoordBuffer[ indexBuffer[3*indF+1] * 2 + 1],
			//	texCoordBuffer[ indexBuffer[3*indF+2] * 2 ] , texCoordBuffer[ indexBuffer[3*indF+2] * 2 + 1]);
			//}
			NbMesh++;

			free(pointBufferIni);
			free(texCoordBufferIni);
			free(indexpointBufferIni);
			free(indextexCoordBufferIni);

			return;
	  }
	}

	copyMeshData(NbMesh , pointBufferIni , texCoordBufferIni ,  
					indexpointBufferIni , indextexCoordBufferIni);
	nbVertexTot += nbpointsMesh[NbMesh];
	nbCoordTexTot += nbtexCoordsMesh[NbMesh];
	nbpointsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
	nbtexCoordsMesh[NbMesh] = NbFacesMesh[NbMesh] * 3;
	// printf ( "%s", line );
	printf( "Parsed mesh %d Vertices %d texCoords %d Faces %d\n" , 
		NbMesh , NbFacesMesh[NbMesh] * 3 , NbFacesMesh[NbMesh] * 3 , NbFacesMesh[NbMesh] );
	NbMesh++;
  }
}

// OBJ file parsing (Alias Wavefront ASCII format)
void load_mesh_obj( void ) {
  NbMesh = 0;

  // parses the mesh (obj format)
  FILE * fileMesh = fopen(obj_file_name.c_str(), "r");
  if( !fileMesh ) {
	  printf("Object file %s not found\n", obj_file_name.c_str());
    exit(0);
  }
  printf("Loading %s\n", obj_file_name.c_str());
  parse_mesh_obj( fileMesh );
  fclose( fileMesh );

  ///////////////////////////////////////////////////////////
  // vertex buffer objects and vertex array for the mesh
  unsigned int mesh_points_vbo[PG_NB_MAX_MESH] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  unsigned int mesh_texCoords_vbo[PG_NB_MAX_MESH] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  for( int indMesh = 0; indMesh < NbMesh ; indMesh++ ) {
	// 3 VBOs
	glGenBuffers (1, &mesh_points_vbo[indMesh]);
	glBindBuffer (GL_ARRAY_BUFFER, mesh_points_vbo[indMesh]);
	glBufferData (GL_ARRAY_BUFFER, 
		  3 * 3 * NbFacesMesh[indMesh] * sizeof (GLfloat), 
		  pointBuffer[indMesh], 
		  GL_STATIC_DRAW);

	glGenBuffers (1, &mesh_texCoords_vbo[indMesh]);
	glBindBuffer (GL_ARRAY_BUFFER, mesh_texCoords_vbo[indMesh]);
	glBufferData (GL_ARRAY_BUFFER,
		  2 * 3 * NbFacesMesh[indMesh] * sizeof (GLfloat),
		  texCoordBuffer[indMesh],
		  GL_STATIC_DRAW);

	mesh_index_vbo[indMesh] = 0;
	glGenBuffers (1, &mesh_index_vbo[indMesh]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMesh]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER,
		  3 * NbFacesMesh[indMesh] * sizeof (GLuint),
		  indexBuffer[indMesh],
		  GL_STATIC_DRAW);

	// VAO
	mesh_vao[indMesh] = 0;
	glGenVertexArrays (1, &mesh_vao[indMesh]);
	glBindVertexArray (mesh_vao[indMesh]);

	glBindBuffer (GL_ARRAY_BUFFER, mesh_points_vbo[indMesh]);
	// vertex positions are at location 0
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer (GL_ARRAY_BUFFER, mesh_texCoords_vbo[indMesh]);
	// texCoord positions are at location 1
	glEnableVertexAttribArray (1); // don't forget this!
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	// glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMesh]);

	// printf("Load Mesh %d vao ID %d vbo ID %d nbfaces %d\n" ,indMesh , mesh_vao[indMesh], mesh_index_vbo[indMesh], NbFacesMesh[indMesh]);
	glBindVertexArray(0); // Disable our Vertex Buffer Object
  }
}
