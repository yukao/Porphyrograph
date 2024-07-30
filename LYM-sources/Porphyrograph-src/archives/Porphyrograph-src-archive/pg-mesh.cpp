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

////////////////////////////////////////
// geometrical data of mesh

// mesh data
GLfloat *vertexBuffer = NULL;
GLfloat *texCoordBuffer = NULL;
GLfloat *normalBuffer = NULL;
GLuint  *indexBuffer = NULL;
GLint* boneIndexBuffer = NULL;
GLfloat* boneWeightBuffer = NULL;

// shader variable pointers
GLint* uniform_mesh_model[PG_MAX_CONFIGURATIONS] = { NULL };
GLint* uniform_mesh_view[PG_MAX_CONFIGURATIONS] = { NULL };
GLint* uniform_mesh_proj[PG_MAX_CONFIGURATIONS] = { NULL };
GLint* uniform_mesh_light[PG_MAX_CONFIGURATIONS] = { NULL };

// mesh lighting
GLfloat mesh_light_x = 0.56f;
GLfloat mesh_light_y = 0.35f;
GLfloat mesh_light_z = 3.f;

// mesh anim data
double *mesh_startAnime[PG_MAX_CONFIGURATIONS] = { NULL };
double *mesh_anime_precTime[PG_MAX_CONFIGURATIONS] = { NULL };
int *mesh_precedingAnime[PG_MAX_CONFIGURATIONS] = { NULL };
double* mesh_startMotion[PG_MAX_CONFIGURATIONS] = { NULL };
double* mesh_motion_precTime[PG_MAX_CONFIGURATIONS] = { NULL };
int* mesh_precedingMotion[PG_MAX_CONFIGURATIONS] = { NULL };
bool* mesh_positiveChange[PG_MAX_CONFIGURATIONS] = { NULL };
bool* mesh_negativeChange[PG_MAX_CONFIGURATIONS] = { NULL };
int chosen_mesh_LibraryPose1 = 0;
int chosen_mesh_LibraryPose2 = 0;
int chosen_mesh_LibraryPose3 = 0;


//////////////////////////////////////////////////////////////////
// MESH FILE PARSING
//////////////////////////////////////////////////////////////////

// the linearization for OpenGL replaces indices associated with each face in obj format
// by coordinates copied from these temporary buffers (through copyMeshData)
void copyMeshData(  int indObjectInMesh , GLfloat *vertexBufferIni , GLfloat *texCoordBufferIni , GLfloat *normalBufferIni ,
					GLint* boneIndexBufferIni, GLfloat* boneWeightBufferIni,
					GLuint  *indexPointBufferIni, GLuint  *indexTexCoordBufferIni, GLuint  *indexNormalBufferIni,
					int nbFacesInThisMesh) {
  	vertexBuffer = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 3 * sizeof(GLfloat));
	texCoordBuffer = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 2 * sizeof(GLfloat));
	normalBuffer = (GLfloat*)malloc(nbFacesInThisMesh * 3 * 3 * sizeof(GLfloat));
	boneIndexBuffer = (GLint*)malloc(nbFacesInThisMesh * 3 * 4 * sizeof(GLint));
	boneWeightBuffer = (GLfloat*)malloc(nbFacesInThisMesh * 3 * 4 * sizeof(GLfloat));
	indexBuffer = (GLuint *)malloc( nbFacesInThisMesh * 3 * sizeof(GLuint));
	for( int indFace = 0 ; indFace < nbFacesInThisMesh ; indFace++ ) {
	  // copies the indices in the index table
	  indexBuffer[ indFace * 3 ] = indFace * 3;
	  indexBuffer[ indFace * 3 + 1 ] = indFace * 3 + 1;
	  indexBuffer[ indFace * 3 + 2 ] = indFace * 3 + 2;

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indVertex1 = indexPointBufferIni[ indFace * 3 ];
	  int indVertex2 = indexPointBufferIni[ indFace * 3 + 1 ];
	  int indVertex3 = indexPointBufferIni[ indFace * 3 + 2 ];
	  //if (indFace == 10) {
		 // printf("Face ind vertices %d %d %d %d\n", indFace, indVertex1, indVertex2, indVertex3);
	  //}
	  vertexBuffer[ indFace * 3 * 3 ]     = vertexBufferIni[ indVertex1 * 3 ];
	  vertexBuffer[ indFace * 3 * 3 + 1 ] = vertexBufferIni[ indVertex1 * 3 + 1 ];
	  vertexBuffer[ indFace * 3 * 3 + 2 ] = vertexBufferIni[ indVertex1 * 3 + 2 ];
	  vertexBuffer[ (indFace * 3 + 1) * 3 ] = vertexBufferIni[ indVertex2 * 3 ];
	  vertexBuffer[ (indFace * 3 + 1) * 3 + 1 ] = vertexBufferIni[ indVertex2 * 3 + 1 ];
	  vertexBuffer[ (indFace * 3 + 1) * 3 + 2 ] = vertexBufferIni[ indVertex2 * 3 + 2 ];
	  vertexBuffer[ (indFace * 3 + 2) * 3 ] = vertexBufferIni[ indVertex3 * 3 ];
	  vertexBuffer[ (indFace * 3 + 2) * 3 + 1 ] = vertexBufferIni[ indVertex3 * 3 + 1 ];
	  vertexBuffer[ (indFace * 3 + 2) * 3 + 2 ] = vertexBufferIni[ indVertex3 * 3 + 2 ];
	 // printf("Face point %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		//vertexBuffer[ indFace * 3 * 3 ] , vertexBuffer[ indFace * 3 * 3 + 1 ] , vertexBuffer[ indFace * 3 * 3 + 2 ] ,
		//vertexBuffer[ (indFace * 3 + 1) * 3 ] , vertexBuffer[ (indFace * 3 + 1) * 3 + 1 ] , vertexBuffer[ (indFace * 3 + 1) * 3 + 2 ] ,
		//vertexBuffer[ (indFace * 3 + 2) * 3 ] , vertexBuffer[ (indFace * 3 + 2) * 3 + 1 ] , vertexBuffer[ (indFace * 3 + 2) * 3 + 2 ] );

	  // copies the texture coordinates from the initial buffer to the final one
	  int indexTexCoord1 = indexTexCoordBufferIni[ indFace * 3 ];
	  int indexTexCoord2 = indexTexCoordBufferIni[ indFace * 3 + 1 ];
	  int indexTexCoord3 = indexTexCoordBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indTexC %d %d %d %d\n" , indFace ,indexTexCoord1,indexTexCoord2,indexTexCoord3);
	  texCoordBuffer[ indFace * 3 * 2 ]     = texCoordBufferIni[ indexTexCoord1 * 2 ];
	  texCoordBuffer[ indFace * 3 * 2 + 1 ] = texCoordBufferIni[ indexTexCoord1 * 2 + 1 ];
	  texCoordBuffer[ (indFace * 3 + 1) * 2 ] = texCoordBufferIni[ indexTexCoord2 * 2 ];
	  texCoordBuffer[ (indFace * 3 + 1) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord2 * 2 + 1 ];
	  texCoordBuffer[ (indFace * 3 + 2) * 2 ] = texCoordBufferIni[ indexTexCoord3 * 2 ];
	  texCoordBuffer[ (indFace * 3 + 2) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord3 * 2 + 1 ];

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indNormal1 = indexNormalBufferIni[indFace * 3];
	  int indNormal2 = indexNormalBufferIni[indFace * 3 + 1];
	  int indNormal3 = indexNormalBufferIni[indFace * 3 + 2];
	  // printf("Face indNormal %d %d %d %d\n" , indFace ,indNormal1,indNormal2,indNormal3);
	  normalBuffer[indFace * 3 * 3] = normalBufferIni[indNormal1 * 3];
	  normalBuffer[indFace * 3 * 3 + 1] = normalBufferIni[indNormal1 * 3 + 1];
	  normalBuffer[indFace * 3 * 3 + 2] = normalBufferIni[indNormal1 * 3 + 2];
	  normalBuffer[(indFace * 3 + 1) * 3] = normalBufferIni[indNormal2 * 3];
	  normalBuffer[(indFace * 3 + 1) * 3 + 1] = normalBufferIni[indNormal2 * 3 + 1];
	  normalBuffer[(indFace * 3 + 1) * 3 + 2] = normalBufferIni[indNormal2 * 3 + 2];
	  normalBuffer[(indFace * 3 + 2) * 3] = normalBufferIni[indNormal3 * 3];
	  normalBuffer[(indFace * 3 + 2) * 3 + 1] = normalBufferIni[indNormal3 * 3 + 1];
	  normalBuffer[(indFace * 3 + 2) * 3 + 2] = normalBufferIni[indNormal3 * 3 + 2];
	 //  printf("Face normal %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		// normalBuffer[ indFace * 3 * 3 ] , normalBuffer[ indFace * 3 * 3 + 1 ] , normalBuffer[ indFace * 3 * 3 + 2 ] ,
		// normalBuffer[ (indFace * 3 + 1) * 3 ] , normalBuffer[ (indFace * 3 + 1) * 3 + 1 ] , normalBuffer[ (indFace * 3 + 1) * 3 + 2 ] ,
		// normalBuffer[ (indFace * 3 + 2) * 3 ] , normalBuffer[ (indFace * 3 + 2) * 3 + 1 ] , normalBuffer[ (indFace * 3 + 2) * 3 + 2 ] );
	 //  printf("Face texC %d %.2f/%.2f %.2f/%.2f %.2f/%.2f  \n" , indFace ,
		//texCoordBuffer[ indFace * 3 * 2  ] , texCoordBuffer[ indFace * 3 * 2 + 1 ] ,
		//texCoordBuffer[ (indFace * 3 + 1) * 2 ] , texCoordBuffer[ (indFace * 3 + 1) * 2 + 1 ] ,
		//texCoordBuffer[ (indFace * 3 + 2) * 2 ] , texCoordBuffer[ (indFace * 3 + 2) * 2 + 1 ]  );

	  // copies the bone indices and weights from the initial buffer to the final one
	  //if (indFace == 10) {
		 // printf("Face %d vertex %d ind object %d faces %d bone index & weights second vertex %d %d %d %d %f %f %f %f\n\n", indFace, indVertex2, indObjectInMesh, nbFacesInThisMesh,
			//  boneIndexBufferIni[indVertex2 * 4], boneIndexBufferIni[indVertex2 * 4 + 1], boneIndexBufferIni[indVertex2 * 4 + 2], boneIndexBufferIni[indVertex2 * 4 + 3],
			//  boneWeightBufferIni[indVertex2 * 4], boneWeightBufferIni[indVertex2 * 4 + 1], boneWeightBufferIni[indVertex2 * 4 + 2], boneWeightBufferIni[indVertex2 * 4 + 3]);
	  //}
	  boneIndexBuffer[indFace * 3 * 4] = boneIndexBufferIni[indVertex1 * 4];
	  boneIndexBuffer[indFace * 3 * 4 + 1] = boneIndexBufferIni[indVertex1 * 4 + 1];
	  boneIndexBuffer[indFace * 3 * 4 + 2] = boneIndexBufferIni[indVertex1 * 4 + 2];
	  boneIndexBuffer[indFace * 3 * 4 + 3] = boneIndexBufferIni[indVertex1 * 4 + 3];
	  boneIndexBuffer[(indFace * 3 + 1) * 4] = boneIndexBufferIni[indVertex2 * 4];
	  boneIndexBuffer[(indFace * 3 + 1) * 4 + 1] = boneIndexBufferIni[indVertex2 * 4 + 1];
	  boneIndexBuffer[(indFace * 3 + 1) * 4 + 2] = boneIndexBufferIni[indVertex2 * 4 + 2];
	  boneIndexBuffer[(indFace * 3 + 1) * 4 + 3] = boneIndexBufferIni[indVertex2 * 4 + 3];
	  boneIndexBuffer[(indFace * 3 + 2) * 4] = boneIndexBufferIni[indVertex3 * 4];
	  boneIndexBuffer[(indFace * 3 + 2) * 4 + 1] = boneIndexBufferIni[indVertex3 * 4 + 1];
	  boneIndexBuffer[(indFace * 3 + 2) * 4 + 2] = boneIndexBufferIni[indVertex3 * 4 + 2];
	  boneIndexBuffer[(indFace * 3 + 2) * 4 + 3] = boneIndexBufferIni[indVertex3 * 4 + 3];

	  boneWeightBuffer[indFace * 3 * 4] = boneWeightBufferIni[indVertex1 * 4];
	  boneWeightBuffer[indFace * 3 * 4 + 1] = boneWeightBufferIni[indVertex1 * 4 + 1];
	  boneWeightBuffer[indFace * 3 * 4 + 2] = boneWeightBufferIni[indVertex1 * 4 + 2];
	  boneWeightBuffer[indFace * 3 * 4 + 3] = boneWeightBufferIni[indVertex1 * 4 + 3];
	  boneWeightBuffer[(indFace * 3 + 1) * 4] = boneWeightBufferIni[indVertex2 * 4];
	  boneWeightBuffer[(indFace * 3 + 1) * 4 + 1] = boneWeightBufferIni[indVertex2 * 4 + 1];
	  boneWeightBuffer[(indFace * 3 + 1) * 4 + 2] = boneWeightBufferIni[indVertex2 * 4 + 2];
	  boneWeightBuffer[(indFace * 3 + 1) * 4 + 3] = boneWeightBufferIni[indVertex2 * 4 + 3];
	  boneWeightBuffer[(indFace * 3 + 2) * 4] = boneWeightBufferIni[indVertex3 * 4];
	  boneWeightBuffer[(indFace * 3 + 2) * 4 + 1] = boneWeightBufferIni[indVertex3 * 4 + 1];
	  boneWeightBuffer[(indFace * 3 + 2) * 4 + 2] = boneWeightBufferIni[indVertex3 * 4 + 2];
	  boneWeightBuffer[(indFace * 3 + 2) * 4 + 3] = boneWeightBufferIni[indVertex3 * 4 + 3];
	}
	//printf("\n");
}

// OBJ file parsing (Alias Wavefront ASCII format)
#define MAX_MESHES 1200
void count_faces_mesh_obj(FILE *file, int *meshNo,
	int **nbVerticesInEachMesh, int **nbTextCoordsInEachMesh, int **nbNormalsInEachMesh,
	int **nbFacesInEachMesh) {
	char    tag[256];
	char    line[256];
	int     nbVerticesInEachMeshTmp[MAX_MESHES];
	int     nbTextCoordsInEachMeshTmp[MAX_MESHES];
	int     nbNormalsInEachMeshTmp[MAX_MESHES];
	int     nbFacesInEachMeshTmp[MAX_MESHES];

	// jumps until the first object
	if (!fgets(line, 256, file)) { return; }
	sscanf(line, "%s", tag);
	while (strcmp(tag, "o") != 0) {
		if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s", tag);
	}

	*meshNo = 0;
	while (strcmp(tag, "o") == 0) {
		if (*meshNo >= MAX_MESHES) {
			printf("Error: Excessive number of Meshes, max %d\n", MAX_MESHES);
			throw 0;
		}

		nbVerticesInEachMeshTmp[*meshNo] = 0;
		nbTextCoordsInEachMeshTmp[*meshNo] = 0;
		nbNormalsInEachMeshTmp[*meshNo] = 0;
		nbFacesInEachMeshTmp[*meshNo] = 0;

		// mesh ID
		sscanf(line, "%s", tag);
		//printf("VH mesh ID %s\n", line);

		// next tag
		if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s", tag);

		// Scan for Verts in this mesh
		while (strcmp(tag, "v") == 0) {
			sscanf(line, "%s", tag);
			nbVerticesInEachMeshTmp[*meshNo]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Skips Verts Weights in this mesh
		while (strcmp(tag, "vw") == 0) {
			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for text coords in this mesh
		while (strcmp(tag, "vt") == 0) {
			sscanf(line, "%s", tag);
			nbTextCoordsInEachMeshTmp[*meshNo]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for normals in this mesh
		while (strcmp(tag, "vn") == 0) {
			sscanf(line, "%s", tag);
			nbNormalsInEachMeshTmp[*meshNo]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for Faces in this mesh
		while (strcmp(tag, "f") == 0
			|| strcmp(tag, "usemtl") == 0
			|| strcmp(tag, "s") == 0) {

			// Scan for a Face in this mesh
			if (strcmp(tag, "f") == 0) {
				nbFacesInEachMeshTmp[*meshNo]++;
			}

			// last line of the obj file, last mesh in this file
			// copies the numbers
			if (!fgets(line, 256, file)) {
				(*meshNo)++;

				*nbVerticesInEachMesh = new int[*meshNo];
				*nbTextCoordsInEachMesh = new int[*meshNo];
				*nbNormalsInEachMesh = new int[*meshNo];
				*nbFacesInEachMesh = new int[*meshNo];
				for (int indMesh = 0; indMesh < *meshNo; indMesh++) {
					(*nbVerticesInEachMesh)[indMesh] = nbVerticesInEachMeshTmp[indMesh];
					(*nbTextCoordsInEachMesh)[indMesh] = nbTextCoordsInEachMeshTmp[indMesh];
					(*nbNormalsInEachMesh)[indMesh] = nbNormalsInEachMeshTmp[indMesh];
					(*nbFacesInEachMesh)[indMesh] = nbFacesInEachMeshTmp[indMesh];
					//printf("Counted mesh %d Vertices %d texCoords %d normals %d faces %d\n",
					//	indMesh, (*nbVerticesInEachMesh)[indMesh], (*nbTextCoordsInEachMesh)[indMesh], (*nbNormalsInEachMesh)[indMesh], (*nbFacesInEachMesh)[indMesh]);
				}
				return;
			}

			// printf("%s\n", line);
			sscanf(line, "%s", tag);

			// last mesh in the obj file
			if (!tag) {
				(*meshNo)++;

				*nbVerticesInEachMesh = new int[*meshNo];
				*nbTextCoordsInEachMesh = new int[*meshNo];
				*nbNormalsInEachMesh = new int[*meshNo];
				*nbFacesInEachMesh = new int[*meshNo];
				for (int indMesh = 0; indMesh < *meshNo; indMesh++) {
					(*nbVerticesInEachMesh)[indMesh] = nbVerticesInEachMeshTmp[indMesh];
					(*nbTextCoordsInEachMesh)[indMesh] = nbTextCoordsInEachMeshTmp[indMesh];
					(*nbNormalsInEachMesh)[indMesh] = nbNormalsInEachMeshTmp[indMesh];
					(*nbFacesInEachMesh)[indMesh] = nbFacesInEachMeshTmp[indMesh];
					printf("Counted mesh %d Vertices %d texCoords %d normals %d faces %d\n",
						indMesh, (*nbVerticesInEachMesh)[indMesh], (*nbTextCoordsInEachMesh)[indMesh], (*nbNormalsInEachMesh)[indMesh], (*nbFacesInEachMesh)[indMesh]);
				}
				return;
			}
		}

		// another mesh follows in the same obj file
		(*meshNo)++;
	}
}

void transferMeshDataToGPU(int indMeshFile, int indObjectInMesh, int indConfiguration) {
	///////////////////////////////////////////////////////////
	// vertex buffer objects and vertex array for the mesh
	unsigned int mesh_points_vbo;
	unsigned int mesh_texCoords_vbo;
	unsigned int mesh_normals_vbo;
	unsigned int mesh_boneIndex_vbo;
	unsigned int mesh_boneWeight_vbo;

	// 3 VBOs
	//printf("GPU transfer %d mesh %d faces %d\n", indMeshFile, indObjectInMesh, pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh]);
	glGenBuffers(1, &mesh_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		vertexBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_texCoords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_texCoords_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		texCoordBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		normalBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_boneIndex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneIndex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLint),
		boneIndexBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_boneWeight_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneWeight_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		boneWeightBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &(pg_Meshes[indConfiguration][indMeshFile].mesh_index_vbo[indObjectInMesh]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_Meshes[indConfiguration][indMeshFile].mesh_index_vbo[indObjectInMesh]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLuint),
		indexBuffer,
		GL_STATIC_DRAW);

	//printf("Index Buffer mesh %d: ", indMeshFile);
	//for (int ind = 0; ind < 3 * pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[indObjectInMesh]; ind++) {
	//	printf("%d ", indexBuffer[ind]);
	//}
	//printf("\n");

	// VAO
	pg_Meshes[indConfiguration][indMeshFile].mesh_vao[indObjectInMesh] = 0;
	glGenVertexArrays(1, &(pg_Meshes[indConfiguration][indMeshFile].mesh_vao[indObjectInMesh]));
	glBindVertexArray(pg_Meshes[indConfiguration][indMeshFile].mesh_vao[indObjectInMesh]);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_points_vbo);
	// vertex positions are at location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_texCoords_vbo);
	// texCoord positions are at location 1
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);
	// normals are at location 2
	glEnableVertexAttribArray(2); 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneIndex_vbo);
	// bone indices are at location 3
	glEnableVertexAttribArray(3); 
	// use glVertexAttribIPointer instead glVertexAttribPointer if the variable is an ivec and not a vec
	glVertexAttribIPointer(3, 4, GL_INT, 0, (GLubyte*)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneWeight_vbo);
	// bone indices are at location 3
	glEnableVertexAttribArray(4); 
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_Meshes[pg_current_configuration_rank][indMeshFile].mesh_index_vbo[indObjectInMesh]);

	//printf("Transferred Mesh %d/%d vao ID %d vbo ID %d nbfaces %d\n" , indMeshFile, indObjectInMesh , pg_Meshes[indConfiguration][indMeshFile].mesh_vao[indObjectInMesh],
	// pg_Meshes[indConfiguration][indMeshFile].mesh_index_vbo[indObjectInMesh], pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile);
	glBindVertexArray(0); // Disable our Vertex Buffer Object

	printOglError(23);

	free(vertexBuffer);
	free(texCoordBuffer);
	free(normalBuffer);
	free(boneIndexBuffer);
	free(boneWeightBuffer);
	free(indexBuffer);
}

void parseArmatureObj(FILE* file, char * line, char * tag, char * id, int indMeshFile, int indConfiguration) {
	int nbBonesLoc = 0;

	// next tag
	sscanf(line, "%s %s", tag, id);

	//printf("VH Armature level %d tag %s nb bones %d line %s\n", 1, tag, nbBonesLoc, line);
	while (strcmp(tag, "transl") == 0) {
		parseOneBoneObj(file, 1, line, tag, id, &nbBonesLoc, indMeshFile, indConfiguration);
	}
}

void parseOneBoneObj(FILE* file, int level, char* line, char* tag, char* id, int * nbBonesLoc, int indMeshFile, int indConfiguration) {
	//printf("VH Bone object level %d tag %s nb bones %d line %s\n", level, tag, *nbBonesLoc, line);
	while (strcmp(tag, "transl") == 0) {
		float w, x, y, z;

		// Scan for Bones in this mesh
		if ((*nbBonesLoc) >= pg_nb_bones[indConfiguration][indMeshFile]) {
			printf("Error: Excessive number of Bones\n");
			throw 0;
		}

		// has read the transl/ID line

		// stores the translation values
		fgets(line, 512, file);
		sscanf(line, "%f %f %f", &x, &y, &z);
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].boneInitialTranslationMatrix
			= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].boneAnimationTranslationMatrix
			= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

		// initialRotation tag
		fgets(line, 512, file);
		// stores the initialRotation values
		fgets(line, 512, file);

		sscanf(line, "%f %f %f %f", &w, &x, &y, &z);
		glm::quat initialRotation
			= glm::quat(w, x, y, z);
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].boneInitialRotationMatrix
			= glm::mat4_cast(initialRotation);
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].boneAnimationRotationMatrix
			= glm::mat4_cast(initialRotation);

		// bone
		fgets(line, 512, file);
		char boneID[256];
		sscanf(line, "%s %256s",
			tag, boneID);
		if (TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].id != string(boneID)) {
			printf("Error: Incorrect Bone ID\n");
			throw 0;
		}

		// length
		fgets(line, 512, file);
		sscanf(line, "%f",
			&(TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].length));
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].points[2 * 3 + 1] = TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].length;
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].points[5 * 3 + 1] = TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].length;

		///////////////////////////////////////////////////////////
		// vertex buffer objects and vertex array for the bones
		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vbo = 0;
		glGenBuffers(1, &(TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vbo));

		TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vao = 0;
		glGenVertexArrays(1, &(TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vao));

		// vertex buffer objects and vertex array
		glBindBuffer(GL_ARRAY_BUFFER, TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vbo);
		glBufferData(GL_ARRAY_BUFFER,
			2 * 3 * 3 * sizeof(float),
			TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].points,
			GL_STATIC_DRAW);

		glBindVertexArray(TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vao);
		glBindBuffer(GL_ARRAY_BUFFER, TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(0);

		// parent
		fgets(line, 512, file);
		sscanf(line, "%s %256s", tag, boneID);
		//printf("Bone %s (parent: %s) (prof: %d)\n",
		//	TabBones[indMeshFile][(*nbBonesLoc)].id.c_str(), boneID, level);

		// associates the parent bone with the current bone
		if (strcmp(boneID, "NULL") != 0) {
			bool parentfound = false;
			for (int ind = 0; ind < (*nbBonesLoc); ind++) {
				if (TabBones[indConfiguration][indMeshFile][ind].id == string(boneID)) {
					TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].parentBone = TabBones[indConfiguration][indMeshFile] + ind;
					if (!TabBones[indConfiguration][indMeshFile][ind].daughterBone) {
						TabBones[indConfiguration][indMeshFile][ind].daughterBone = TabBones[indConfiguration][indMeshFile] + (*nbBonesLoc);
					}
					else {
						Bone* currentBone = TabBones[indConfiguration][indMeshFile][ind].daughterBone;
						while (currentBone->sisterBone) {
							currentBone = currentBone->sisterBone;
						}
						currentBone->sisterBone = TabBones[indConfiguration][indMeshFile] + (*nbBonesLoc);
					}
					parentfound = true;
					break;
				}
			}
			if (!parentfound) {
				printf("Parent of bone %s (%s) not found!\n",
					TabBones[indConfiguration][indMeshFile][(*nbBonesLoc)].id.c_str(), id);
			}
		}
		// no parent chains with the root node
		else {
			// it is not the root node
			if ((*nbBonesLoc) > 0) {
				Bone* currentBone = TabBones[indConfiguration][indMeshFile];
				while (currentBone->sisterBone) {
					currentBone = currentBone->sisterBone;
				}
				currentBone->sisterBone = TabBones[indConfiguration][indMeshFile] + (*nbBonesLoc);
			}
		}

		// next tag
		fgets(line, 512, file);
		sscanf(line, "%s %s", tag, id);

		(*nbBonesLoc)++;

		// daughter bone
		if (strcmp(tag, "transl") == 0) {
			parseOneBoneObj(file, level + 1, line, tag, id, nbBonesLoc, indMeshFile, indConfiguration);
			if (strcmp(tag, "end") == 0) { strcpy(tag, "end"); return; }

			// if empty line: end of file
			if (!fgets(line, 512, file)) { strcpy(tag, "end"); return; }
			// non empty line: reads further (possible sister node)
			sscanf(line, "%s %s", tag, id);
		}

		// end_bone tag
		else if (strcmp(tag, "bone_end") == 0) {
			// if empty line: end of file
			if (!fgets(line, 512, file)) { strcpy(tag, "end"); return; }
			// non empty line: reads further (possible sister node)
			sscanf(line, "%s %s", tag, id);
		}
	}
}

void copy_mesh_data_and_ship_to_GPU(int indMeshFile, int indObjectInMesh, GLfloat* vertexBufferIni, 
	GLfloat* texCoordBufferIni, GLfloat* normalBufferIni,
	GLint* boneIndexBufferIni, GLfloat* boneWeightBufferIni,
	GLuint* indexPointBufferIni, GLuint* indexTexCoordBufferIni, GLuint* indexNormalBufferIni, int nbFacesInThisMesh, 
	int * nbVertexTot, int * nbCoordTexTot, int * nbNormalTot, int nbVertices, int nbTexCoords, int nbNormals, vector <string> mesh_IDs_current_mesh,
	int indConfiguration) {
	copyMeshData(indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
		boneIndexBufferIni, boneWeightBufferIni,
		indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
		nbFacesInThisMesh);

	//printf("Parsed mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
	//	indObjectInMesh, mesh_IDs_current_mesh[indObjectInMesh].c_str(), nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh], nbFacesInEachMesh[indObjectInMesh]);
	transferMeshDataToGPU(indMeshFile, indObjectInMesh, indConfiguration);

	(*nbVertexTot) += nbVertices;
	(*nbCoordTexTot) += nbTexCoords;
	(*nbNormalTot) += nbNormals;

	(indObjectInMesh)++;
}

// OBJ file parsing (Alias Wavefront ASCII format)
void parseMeshObj(FILE *file, int indMeshFile, int nbMeshObjects,
	int *nbVerticesInEachMesh, int *nbTextCoordsInEachMesh, int *nbNormalsInEachMesh,
	int *nbFacesInEachMesh, int indConfiguration) {
	char    tag[256];
	char    tag2[256];
	char    meshString[1024];
	char    line[256];
	char    id[256];

	// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
	// the buffers of vertices, tex coords, and normals, and the associated counts are temporary
	// they are then used for linearization for OpenGL
	// the linearization for OpenGL replaces indices associated with each face in obj format
	// by coordinates copied from these temporary buffers (through copyMeshData)
	GLfloat *vertexBufferIni = NULL;
	GLfloat *texCoordBufferIni = NULL;
	GLfloat* normalBufferIni = NULL;
	GLint* boneIndexBufferIni = NULL;
	GLfloat* boneWeightBufferIni = NULL;
	GLuint  *indexPointBufferIni = NULL;
	GLuint  *indexTexCoordBufferIni = NULL;
	GLuint* indexNormalBufferIni = NULL;

	vector <string> mesh_IDs_current_mesh;

	// local size of mesh
	// initial count of mesh elements, before linearization by copyMeshData for OpenGL
	// after linearization there are 3 vertices, texCoords, and normals per face
	int *nbVerticesMeshIni = new int[nbMeshObjects];
	int *nbTexCoordsMeshIni = new int[nbMeshObjects];
	int* nbNormalsMeshIni = new int[nbMeshObjects];
	int *nbFacesMeshIni = new int[nbMeshObjects];

	// Two comment lines
	// # Blender3D v244 OBJ File: Anime_Girl.blend
	// # www.blender3d.org
	if (!fgets(line, 256, file)) { return; }
	if (!fgets(line, 256, file)) { return; }

	// optional material name
	if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s", tag);
	if (strcmp(tag, "mtllib") == 0) {
		sscanf(line, "%s", tag);
		// reads next line: object
		if (!fgets(line, 256, file)) { return; }
		// mesh ID
		sscanf(line, "%s", tag);
	}
	else {
		// we are on object or armature line
	}

	// optional armature
	// Bone list
	string str(line);
	if (str.find(string("Bones")) != string::npos) {
		// line with bone number
		if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s %d", tag, &(pg_nb_bones[indConfiguration][indMeshFile]));
		TabBones[indConfiguration][indMeshFile] = new Bone[pg_nb_bones[indConfiguration][indMeshFile]];
		//printf("VH %d bones to parse\n", pg_nb_bones[indMeshFile]);

		if (pg_nb_bones[indConfiguration][indMeshFile] > 0) {
			// bone ID line
			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
			int nbBonesLoc = 0;
			while (strcmp(tag, "bone") == 0) {
				if (nbBonesLoc >= pg_nb_bones[indConfiguration][indMeshFile]) {
					printf("Error: Excessive number of bones in object\n");
					throw 0;
				}
				char boneID[256];
				sscanf(line, "%s %256s", tag, boneID);
				TabBones[indConfiguration][indMeshFile][nbBonesLoc].id = string(boneID);
				nbBonesLoc++;

				if (!fgets(line, 512, file)) { return; }
				sscanf(line, "%s", tag);
			}
		}

		parseArmatureObj(file, line, tag, id, indMeshFile, indConfiguration);
		printf("Armature ind Mesh %d Nb bones %d\n", indMeshFile, pg_nb_bones[indConfiguration][indMeshFile]);

		pg_nb_LibraryPoses[indConfiguration][indMeshFile] = 0;
		// possible list of poses
		if (strcmp(tag, "POSES") == 0) {
			sscanf(line, "%s %d", tag, &(pg_nb_LibraryPoses[indConfiguration][indMeshFile]));
			printf("Nb poses %d\n", pg_nb_LibraryPoses[indConfiguration][indMeshFile]);

			// allocates the pose keyframe transformations (quaternion for rotation, and 3D vector for translation)
			for (int indBone = 0; indBone < pg_nb_bones[indConfiguration][indMeshFile]; indBone++) {
				TabBones[indConfiguration][indMeshFile][indBone].boneLibraryPoseRotationQuat = new glm::quat[pg_nb_LibraryPoses[indConfiguration][indMeshFile]];
				TabBones[indConfiguration][indMeshFile][indBone].boneLibraryPoseTranslationVector = new glm::vec3[pg_nb_LibraryPoses[indConfiguration][indMeshFile]];
				TabBones[indConfiguration][indMeshFile][indBone].boneAnimationPoseRotationQuat = new glm::quat[PG_MAX_ANIMATION_POSES];
				TabBones[indConfiguration][indMeshFile][indBone].boneAnimationPoseTranslationVector = new glm::vec3[PG_MAX_ANIMATION_POSES];
			}

			// reads the keyframes for the bones, 
			// no assumption about the order in which the transformations are given
			if (!fgets(line, 512, file)) { return; }
			sscanf(line, "%s", tag);
			int nbPoses = 0;
			while (strcmp(tag, "POSE") == 0) {
				int indPose = -1;
				int indFrame = -1;
				sscanf(line, "%s %d", tag, &indPose);
				if (indPose < pg_nb_LibraryPoses[indConfiguration][indMeshFile] && indPose == nbPoses) {
					//printf("scanning pose #%d\n", indPose);
					// first transformation (or next pose or next object if no transformation given)
					if (!fgets(line, 256, file)) { return; }
					sscanf(line, "%s", tag);
					// reads each pose
					while (strcmp(tag, "POSE") != 0 && strcmp(tag, "o") != 0) {
						float val1 = 0.f, val2 = 0.f, val3 = 0.f, val4 = 1.f;
						sscanf(line, "%s %s %f %f %f %f", tag, tag2, &val1, &val2, &val3, &val4);
						bool found = false;
						for (int indBone = 0; indBone < pg_nb_bones[indConfiguration][indMeshFile]; indBone++) {
							if (TabBones[indConfiguration][indMeshFile][indBone].id == string(tag)) {
								if (strcmp(tag2, "transl") == 0) {
									TabBones[indConfiguration][indMeshFile][indBone].boneLibraryPoseTranslationVector[indPose]
										= glm::vec3(val1, val2, val3);
									//printf("transl mesh #%d, bone #%d,%s %.4f %.4f %.4f\n", indMeshFile, indBone, TabBones[indMeshFile][indBone].id.c_str(), val1, val2, val3);
								}
								else if (strcmp(tag2, "rot") == 0) {
									// CAUTION: W is the first coordinate in the quat constructor
									TabBones[indConfiguration][indMeshFile][indBone].boneLibraryPoseRotationQuat[indPose]
										= glm::quat(val4, val1, val2, val3);
									//printf("rot mesh #%d, bone #%d,%s %.4f %.4f %.4f %.4f\n", indMeshFile, indBone, TabBones[indMeshFile][indBone].id.c_str(), val1, val2, val3, val4);
								}
								else {
									printf("Error: incorrect transformation tag for pose %d: %s\n", indPose, tag);
									throw 0;
								}
								found = true;
								break;
							}
						}
						if (!found) {
							printf("Error: unknown bone ID %s for pose %d\n", tag2, indPose);
							throw 0;
						}

						// next transformation or next pose or first object
						if (!fgets(line, 512, file)) { return; }
						sscanf(line, "%s", tag);
					}
				}
				else {
					printf("Error: incorrect pose number %d maximum for this mesh %d expected rank %d\n", indPose, pg_nb_LibraryPoses[indConfiguration][indMeshFile], nbPoses - 1);
					throw 0;
				}
				nbPoses++;
			}
			// next pose
		}
		// else no pose
	}
	// else no armature

	int nbVertexTot = 0;
	int nbCoordTexTot = 0;
	int nbNormalTot = 0;
	int indObjectInMesh = 0;
	//printf("VH tag before parsing object %s line %s\n", tag, line);
	while (strcmp(tag, "o") == 0) {
		if (indObjectInMesh >= nbMeshObjects) {
			printf("Error: Excessive number of Meshes, max %d\n", nbMeshObjects);
			throw 0;
		}

		// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
		vertexBufferIni = (GLfloat *)malloc(nbVerticesInEachMesh[indObjectInMesh] * 3 * sizeof *vertexBufferIni);
		texCoordBufferIni = (GLfloat *)malloc(nbTextCoordsInEachMesh[indObjectInMesh] * 2 * sizeof *texCoordBufferIni);
		normalBufferIni = (GLfloat*)malloc(nbNormalsInEachMesh[indObjectInMesh] * 3 * sizeof * normalBufferIni);
		boneIndexBufferIni = (GLint*)malloc(nbVerticesInEachMesh[indObjectInMesh] * 4 * sizeof * boneIndexBufferIni);
		boneWeightBufferIni = (GLfloat*)malloc(nbVerticesInEachMesh[indObjectInMesh] * 4 * sizeof * boneWeightBufferIni);
		indexTexCoordBufferIni = (GLuint *)malloc(nbFacesInEachMesh[indObjectInMesh] * 3 * sizeof *indexTexCoordBufferIni);
		indexPointBufferIni = (GLuint *)malloc(nbFacesInEachMesh[indObjectInMesh] * 3 * sizeof *indexPointBufferIni);
		indexNormalBufferIni = (GLuint*)malloc(nbFacesInEachMesh[indObjectInMesh] * 3 * sizeof * indexNormalBufferIni);

		// barycenter
		array<float, 3> barycenter = { 0.f, 0.f, 0.f};

		// initial count of mesh elements, before linearization by copyMeshData for OpenGL, local value
		nbVerticesMeshIni[indObjectInMesh] = 0;
		nbTexCoordsMeshIni[indObjectInMesh] = 0;
		nbNormalsMeshIni[indObjectInMesh] = 0;
		nbFacesMeshIni[indObjectInMesh] = 0;

		// mesh ID
		sscanf(line, "%s %s", tag, meshString);
		mesh_IDs_current_mesh.push_back(meshString);

		// next tag
		if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s", tag);

		// Scan for Verts in this mesh
		while (strcmp(tag, "v") == 0) {
			if (nbVerticesMeshIni[indObjectInMesh] >= nbVerticesInEachMesh[indObjectInMesh]) {
				printf("Error: Excessive number of vertices\n");
				throw 0;
			}
			sscanf(line, "%s %f %f %f",
				tag, &vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3],
				&vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3 + 1],
				&vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3 + 2]);
			barycenter[0] += vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3 + 0];
			barycenter[1] += vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3 + 1];
			barycenter[2] += vertexBufferIni[nbVerticesMeshIni[indObjectInMesh] * 3 + 2];
			nbVerticesMeshIni[indObjectInMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}
		if (nbVerticesMeshIni[indObjectInMesh] != 0) {
			barycenter[0] /= nbVerticesMeshIni[indObjectInMesh];
			barycenter[1] /= nbVerticesMeshIni[indObjectInMesh];
			barycenter[2] /= nbVerticesMeshIni[indObjectInMesh];
		}
		pg_Meshes[indConfiguration][indMeshFile].mesh_barycenter[indObjectInMesh] = barycenter;
		//printf("Object %d/%d barycenter %.2f %.2f %.2f\n", indMeshFile, indObjectInMesh, barycenter[0], barycenter[1], barycenter[2]);

		if (nbVerticesMeshIni[indObjectInMesh] != nbVerticesInEachMesh[indObjectInMesh]) {
			printf("Error: Vertices counted number and Vertices parsed number discrepancy (%d vs %d) in Mesh %d / Object %d\n",
				nbVerticesMeshIni[indObjectInMesh], nbVerticesInEachMesh[indObjectInMesh], indMeshFile, indObjectInMesh);
			throw 0;
		}

		// Scan for vertex weights in this mesh
		// first fills the weights with null values since all the vertices might not be weighted
		for (int indVertex = 0; indVertex < nbVerticesMeshIni[indObjectInMesh]; indVertex++) {
			boneIndexBufferIni[indVertex * 4 + 0] = -1;
			boneIndexBufferIni[indVertex * 4 + 1] = -1;
			boneIndexBufferIni[indVertex * 4 + 2] = -1;
			boneIndexBufferIni[indVertex * 4 + 3] = -1;

			boneWeightBufferIni[indVertex * 4 + 0] = 0.f;
			boneWeightBufferIni[indVertex * 4 + 1] = 0.f;
			boneWeightBufferIni[indVertex * 4 + 2] = 0.f;
			boneWeightBufferIni[indVertex * 4 + 3] = 0.f;
		}
		while (strcmp(tag, "vw") == 0) {
			int indVertex = 0;
			int indBone1 = -1, indBone2 = -1, indBone3 = -1, indBone4 = -1;
			float boneW1 = -1, boneW2 = -1, boneW3 = -1, boneW4 = -1;
			// output is normalized with four weight (possibly padded with null weights and negative indices)
			sscanf(line, "%s %d %d %f %d %f %d %f %d %f",
				tag, &indVertex, &indBone1, &boneW1, &indBone2, &boneW2, &indBone3, &boneW3, &indBone4, &boneW4);
			indVertex--;
			indBone1--; indBone2--; indBone3--; indBone4--;
			if (indVertex < 0 || indVertex >= nbVerticesMeshIni[indObjectInMesh]) {
				printf("Error: Incorrect vertex index in bone weights %d (not int %d-%d)\n", indVertex, 0, nbVerticesMeshIni[indObjectInMesh] - 1);
				throw 0;
			}
			if (indBone1 < -1 || indBone1 >= pg_nb_bones[indConfiguration][indMeshFile]|| indBone2 < -1 || indBone2 >= pg_nb_bones[indConfiguration][indMeshFile]
				|| indBone3 < -1 || indBone3 >= pg_nb_bones[indConfiguration][indMeshFile] || indBone4 < -1 || indBone4 >= pg_nb_bones[indConfiguration][indMeshFile]) {
				printf("Error: Incorrect vertex index in bone weights %d/%d/%d/%d (not int %d-%d)\n", indBone1, indBone2, indBone3, indBone4, -1, pg_nb_bones[indConfiguration][indMeshFile] - 1);
				throw 0;
			}
			//printf("vw %s %d %d %f %d %f %d %f %d % f\n",
			//	tag, indVertex, indBone1, float(boneW1), indBone2, float(boneW2), indBone3, float(boneW3), indBone4, float(boneW4));

			boneIndexBufferIni[indVertex * 4 + 0] = indBone1;
			boneIndexBufferIni[indVertex * 4 + 1] = indBone2;
			boneIndexBufferIni[indVertex * 4 + 2] = indBone3;
			boneIndexBufferIni[indVertex * 4 + 3] = indBone4;

			boneWeightBufferIni[indVertex * 4 + 0] = boneW1;
			boneWeightBufferIni[indVertex * 4 + 1] = boneW2;
			boneWeightBufferIni[indVertex * 4 + 2] = boneW3;
			boneWeightBufferIni[indVertex * 4 + 3] = boneW4;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for text coords in this mesh
		while (strcmp(tag, "vt") == 0) {
			if (nbTexCoordsMeshIni[indObjectInMesh] > nbTextCoordsInEachMesh[indObjectInMesh]) {
				printf("Error: Excessive number of texCoords\n");
				throw 0;
			}
			sscanf(line, "%s %f %f", tag, &texCoordBufferIni[nbTexCoordsMeshIni[indObjectInMesh] * 2],
				&texCoordBufferIni[nbTexCoordsMeshIni[indObjectInMesh] * 2 + 1]);
			//printf("vt %d %.2f %.2f\n",nbTexCoordsMeshIni[indObjectInMesh],texCoordBufferIni[nbTexCoordsMeshIni[indObjectInMesh] * 2],
				  //	texCoordBufferIni[nbTexCoordsMeshIni[indObjectInMesh] * 2+1]);
			nbTexCoordsMeshIni[indObjectInMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for normals in this mesh
		while (strcmp(tag, "vn") == 0) {
			if (nbNormalsMeshIni[indObjectInMesh] > nbNormalsInEachMesh[indObjectInMesh]) {
				printf("Error: Excessive number of texCoords\n");
				throw 0;
			}
			sscanf(line, "%s %f %f %f", tag, &normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3],
				&normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3 + 1], &normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3 + 2]);
			//printf("vt %d %.2f %.2f\n",nbNormalsMeshIni[indObjectInMesh],normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3],
				  //	normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3+1] , normalBufferIni[nbNormalsMeshIni[indObjectInMesh] * 3+2]);
			nbNormalsMeshIni[indObjectInMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for Faces in this mesh and what possibly follows before a new object or the end of the file
		while (strcmp(tag, "f") == 0
			|| strcmp(tag, "usemtl") == 0
			|| strcmp(tag, "s") == 0
			|| strcmp(tag, "o") == 0) {
			//printf("VH tag after reading vn %s mesh %d object %d line %s\n", tag, indMeshFile, indObjectInMesh, line);

			// scans faces and stops at file end or new object
			if (strcmp(tag, "f") == 0) {
				if (nbFacesMeshIni[indObjectInMesh] >= nbFacesInEachMesh[indObjectInMesh]) {
					printf("Error: Excessive number of faces\n");
					throw 0;
				}

				int indVertex1, indVertex2, indVertex3;
				int indexTexCoord1, indexTexCoord2, indexTexCoord3;
				int indexNormal1, indexNormal2, indexNormal3;
				sscanf(line, "%s %d/%d/%d %d/%d/%d %d/%d/%d",
					tag, &indVertex1, &indexTexCoord1, &indexNormal1,
					&indVertex2, &indexTexCoord2, &indexNormal2,
					&indVertex3, &indexTexCoord3, &indexNormal3);
				// indices start from 1 in OBJ format
				// we make start from 0 for C++
				indVertex1--;
				indVertex2--;
				indVertex3--;
				indexTexCoord1--;
				indexTexCoord2--;
				indexTexCoord3--;
				indexNormal1--;
				indexNormal2--;
				indexNormal3--;

				//printf( "%s indexPointBufferIni %d/%d %d/%d %d/%d\n", 
				//	line, indVertex1 , indexTexCoord1 , 
				//	indVertex2 , indexTexCoord2 , indVertex3 , indexTexCoord3 );

				// copies the texCoords from the initial buffer to the final one
				// pointBufferIniPtr should be used because vertexBufferIni has been
				// incremented during texCoords scanning
				indexPointBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3] = indVertex1 - nbVertexTot;
				indexPointBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3 + 1] = indVertex2 - nbVertexTot;
				indexPointBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3 + 2] = indVertex3 - nbVertexTot;

				indexTexCoordBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3] = indexTexCoord1 - nbCoordTexTot;
				indexTexCoordBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3 + 1] = indexTexCoord2 - nbCoordTexTot;
				indexTexCoordBufferIni[nbFacesMeshIni[indObjectInMesh]  * 3 + 2] = indexTexCoord3 - nbCoordTexTot;

				indexNormalBufferIni[nbFacesMeshIni[indObjectInMesh] * 3] = indexNormal1 - nbNormalTot;
				indexNormalBufferIni[nbFacesMeshIni[indObjectInMesh] * 3 + 1] = indexNormal2 - nbNormalTot;
				indexNormalBufferIni[nbFacesMeshIni[indObjectInMesh] * 3 + 2] = indexNormal3 - nbNormalTot;

				nbFacesMeshIni[indObjectInMesh]++;

			}

			// last line of the obj file, last mesh in this file
			// copies the data (and possibly duplicate them)
			if (!fgets(line, 256, file)) {
				//printf("Last parsed mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
				//	indObjectInMesh, mesh_IDs_current_mesh[indObjectInMesh].c_str(), nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh], nbFacesMeshIni[indObjectInMesh]);

				copy_mesh_data_and_ship_to_GPU(indMeshFile, indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
					boneIndexBufferIni, boneWeightBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
					nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
					mesh_IDs_current_mesh, indConfiguration);

				if (indObjectInMesh != nbMeshObjects - 1) {
					printf("Error: Mesh counted number and mesh parsed number discrepancy %d/%d\n", indObjectInMesh, nbMeshObjects - 1);
					throw 0;
				}
				if (nbFacesMeshIni[indObjectInMesh] != nbFacesInEachMesh[indObjectInMesh]) {
					printf("Error: Face counted number and face parsed number discrepancy 1\n");
					throw 0;
				}

				// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
				free(vertexBufferIni);
				free(texCoordBufferIni);
				free(normalBufferIni);
				free(indexPointBufferIni);
				free(indexTexCoordBufferIni);
				free(indexNormalBufferIni);

				// initial count of mesh elements, before linearization by copyMeshData for OpenGL
				// after linearization there are 3 vertices, texCoords, and normals per face
				free(nbVerticesMeshIni);
				free(nbTexCoordsMeshIni);
				free(nbNormalsMeshIni);

				return;
			}

			// printf("%s\n", line);
			sscanf(line, "%s", tag);
			// last mesh in the obj file
			if (!tag) {
				//printf("Last parsed mesh -2- %d %s Vertices %d texCoords %d normals %d faces %d\n",
				//	indObjectInMesh, mesh_IDs_current_mesh[indObjectInMesh].c_str(), nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh], nbFacesMeshIni[indObjectInMesh]);

				copy_mesh_data_and_ship_to_GPU(indMeshFile, indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
					boneIndexBufferIni, boneWeightBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
					nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
					mesh_IDs_current_mesh, indConfiguration);

				if (indObjectInMesh != nbMeshObjects - 1) {
					printf("Error: Mesh counted number and mesh parsed number discrepancy %d/%d\n", indObjectInMesh, nbMeshObjects - 1);
					throw 0;
				}
				if (nbFacesMeshIni[indObjectInMesh] != nbFacesInEachMesh[indObjectInMesh]) {
					printf("Error: Face counted number and face parsed number discrepancy 2\n");
					throw 0;
				}

				(indObjectInMesh)++;

				// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
				free(vertexBufferIni);
				free(texCoordBufferIni);
				free(normalBufferIni);
				free(indexPointBufferIni);
				free(indexTexCoordBufferIni);
				free(indexNormalBufferIni);

				// initial count of mesh elements, before linearization by copyMeshData for OpenGL
				// after linearization there are 3 vertices, texCoords, and normals per face
				free(nbVerticesMeshIni);
				free(nbTexCoordsMeshIni);
				free(nbNormalsMeshIni);

				return;
			}

			if (strcmp(tag, "o") == 0) {
				break;
			}
		}

		// about to read a new object should store the current object and continue reading objects
		if (nbFacesMeshIni[indObjectInMesh] != nbFacesInEachMesh[indObjectInMesh]) {
			printf("Error: Face counted number and face parsed number discrepancy (%d vs %d) in Mesh %d / Object %d\n", 
				nbFacesMeshIni[indObjectInMesh], nbFacesInEachMesh[indObjectInMesh], indMeshFile, indObjectInMesh);
			throw 0;
		}

		//printf("Parsed non last mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
		//	indObjectInMesh, mesh_IDs_current_mesh[indObjectInMesh].c_str(), nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh], nbFacesMeshIni[indObjectInMesh]);

		// another mesh follows in the same obj file
		copy_mesh_data_and_ship_to_GPU(indMeshFile, indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
			boneIndexBufferIni, boneWeightBufferIni,
			indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
			nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
			nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
			mesh_IDs_current_mesh, indConfiguration);
		(indObjectInMesh)++;

		// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
		free(vertexBufferIni);
		free(texCoordBufferIni);
		free(normalBufferIni);
		free(indexPointBufferIni);
		free(indexTexCoordBufferIni);
		free(indexNormalBufferIni);
	}
}

// OBJ file parsing (Alias Wavefront ASCII format)
// nbMeshObjectsInFile: number of mesh objects inside the obj file
void load_mesh_objects(string mesh_file_name, int indMeshFile, int indConfiguration) {
	int *nbVerticesPerMesh = NULL;
	int *nbTexCoordsPerMesh = NULL;
	int *nbNormalsPerMesh = NULL;

	///////////////////////////////////////////////////////////
	// opens the obj file
	FILE * fileMesh = fopen(mesh_file_name.c_str(), "r");
	if (!fileMesh) {
		printf("Mesh file %s not found\n", mesh_file_name.c_str());
		exit(0);
	}

	///////////////////////////////////////////////////////////
	// parses the mesh (obj format) 
	// and loads the data of the meshes in the obj file
	//printf("Loading %s\n", mesh_file_name.c_str());
	int nbMeshObjectsInFile = 0;
	count_faces_mesh_obj(fileMesh, &nbMeshObjectsInFile, &nbVerticesPerMesh,
		&nbTexCoordsPerMesh, &nbNormalsPerMesh, &(pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile));
	//printf("File mesh %s has %d objects\n", mesh_file_name.c_str(), nbMeshObjectsInFile);
	if (nbMeshObjectsInFile > 0) {
		for (int ind = 0; ind < nbMeshObjectsInFile; ind++) {
			//printf("   Object %d Nb faces %d vertices %d tex coord %d normals %d\n", ind,
			//	pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile[ind], nbVerticesPerMesh[ind],
			//	nbTexCoordsPerMesh[ind], nbNormalsPerMesh[ind]);
		}
	}
	else {
		//printf("File mesh withouth objects\n");
		pg_Meshes[indConfiguration][indMeshFile].pg_nbObjectsPerMeshFile = 0;
		exit(0);
	}

	///////////////////////////////////////////////////////////
	// vertex buffer objects and vertex array for the mesh
	array<float, 3> a = { 0.f };
	for (int ind = 0; ind < nbMeshObjectsInFile; ind++) {
		pg_Meshes[indConfiguration][indMeshFile].mesh_vao.push_back(NULL_ID);
		pg_Meshes[indConfiguration][indMeshFile].mesh_index_vbo.push_back(NULL_ID);
		pg_Meshes[indConfiguration][indMeshFile].mesh_barycenter.push_back(a);
	}

	pg_Meshes[indConfiguration][indMeshFile].pg_nbObjectsPerMeshFile = nbMeshObjectsInFile;
	rewind(fileMesh);
	parseMeshObj(fileMesh, indMeshFile, nbMeshObjectsInFile, nbVerticesPerMesh,
		nbTexCoordsPerMesh, nbNormalsPerMesh, pg_Meshes[indConfiguration][indMeshFile].pg_nbFacesPerMeshFile, indConfiguration);
	fclose(fileMesh);

	free(nbVerticesPerMesh);
	free(nbTexCoordsPerMesh);
	free(nbNormalsPerMesh);
}

/////////////////////////////////////////////////////////////////////
// LOADS MESHES FROM BLENDER FILES
// point positions and texture coordinates
void pg_loadAllMeshes(void) {
	std::cout << "Loading meshes: " << std::endl;
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		int nbMeshObjects = 0;
		int indMeshFile = 0;
		for (MeshData &aMesh : pg_Meshes[indConfiguration]) {
			load_mesh_objects(aMesh.pg_Mesh_fileNames, indMeshFile, indConfiguration);
			nbMeshObjects += aMesh.pg_nbObjectsPerMeshFile;
			std::cout << aMesh.pg_Mesh_fileNames << " (" << aMesh.pg_nbObjectsPerMeshFile << " objects), ";
			//printf("Loaded %d mesh objects \n", nbMeshObjects);

			// Mme Changhai: broken glass objects initialization
#if defined(var_MmeShanghai_brokenGlass)
			aMesh.pg_MmeShanghaiActveMeshObjects = new bool[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghaiMeshObjectWakeupTime = new double[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Rotation_angle = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Rotation_X = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Rotation_Y = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Rotation_Z = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Translation_X = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Translation_Y = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Translation_Z = new float[aMesh.pg_nbObjectsPerMeshFile];
			aMesh.pg_MmeShanghai_Object_Rotation_Ini_angle = new float[aMesh.pg_nbObjectsPerMeshFile];
			for (int indMeshObjectInFile = 0; indMeshObjectInFile < aMesh.pg_nbObjectsPerMeshFile; indMeshObjectInFile++) {
				aMesh.pg_MmeShanghaiActveMeshObjects[indMeshObjectInFile] = false;
				aMesh.pg_MmeShanghaiMeshObjectWakeupTime[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Rotation_angle[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Rotation_X[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Rotation_Y[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Rotation_Z[indMeshObjectInFile] = 1.f;
				aMesh.pg_MmeShanghai_Object_Translation_X[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Translation_Y[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Translation_Z[indMeshObjectInFile] = 0.f;
				aMesh.pg_MmeShanghai_Object_Rotation_Ini_angle[indMeshObjectInFile] = 0.f;
			}
			for (int indMeshSubPart = 0; indMeshSubPart < pg_Meshes[pg_current_configuration_rank][indMeshFile].pg_MmeShanghai_NbMeshSubParts; indMeshSubPart++) {
				aMesh.pg_MmeShanghai_MeshSubParts[indMeshSubPart] = new bool[aMesh.pg_nbObjectsPerMeshFile];
				loadMeshSubParts(aMesh.pg_MmeShanghai_MeshSubPart_FileNames[indMeshSubPart],
					aMesh.pg_MmeShanghai_MeshSubParts[indMeshSubPart],
					aMesh.pg_nbObjectsPerMeshFile);
			}
#endif
			std::cout << std::endl;
			indMeshFile++;
		}
	}
	std::cout << std::endl;
}

#if defined(var_MmeShanghai_brokenGlass)
void loadMeshSubParts(string meshPart_fileName, bool* ObjectsInSubPart, int nbObjectsInMesh) {
	///////////////////////////////////////////////////////////
	// opens the text file (one object rank per line)
	FILE* fileMeshPart = fopen(meshPart_fileName.c_str(), "r");
	if (!fileMeshPart) {
		printf("Mesh part file %s not found\n", meshPart_fileName.c_str());
		exit(0);
	}
	char line[256];

	for (int indObj = 0; indObj < nbObjectsInMesh; indObj++) {
		ObjectsInSubPart[indObj] = false;
	}
	while (fgets(line, 256, fileMeshPart)) {
		int indObject = 0;
		sscanf(line, "%d", &indObject);
		if (indObject < nbObjectsInMesh) {
			ObjectsInSubPart[indObject] = true;
		}
		else {
			printf("Object rank in part incorrect %d (nb objetcs in part %d)\n", indObject, nbObjectsInMesh);
		}
	}

	fclose(fileMeshPart);
}
#endif

//////////////////////////////////////////////////////////////////
// MESH ARMATURE RENDERING
//////////////////////////////////////////////////////////////////


void render_one_bone(Bone* bone, glm::mat4 parentModelMatrix) {
	if (!bone)
		return;
	//printf("render bone %s\n", bone->id.c_str());

	// OpenGL transformations for drawing (structure)
	glm::mat4 localModelMatrix
		= parentModelMatrix
		* bone->boneInitialTranslationMatrix
		* bone->boneInitialRotationMatrix;

	// OpenGL transformations for drawing (animation)
	// combination of local model matrix and bone animation matrix
	localModelMatrix = localModelMatrix * bone->boneAnimationRotationMatrix;

	// bone graphical rendering
	glUniformMatrix4fv(uniform_Mesh_vp_model[pg_current_configuration_rank], 1, GL_FALSE,
		glm::value_ptr(localModelMatrix));
	// to be checked
	//glUniformMatrix4fv(uniform_Mesh_vp_inverseModel, 1, GL_FALSE,
	//	glm::value_ptr(glm::inverse(localModelMatrix)));
	//GLfloat boneColor[3] = { 1,1,1 };
	//glUniform3fv(bodyShader->uniform_object_objectColor,
	//	1, boneColor);

	// draw triangle strips from the currently bound VAO
	// with current in-use shader
	glBindVertexArray(bone->vao);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	// recursive call
	render_one_bone(bone->daughterBone, localModelMatrix);

	// recursive call
	render_one_bone(bone->sisterBone, parentModelMatrix);
}

void render_bones(glm::mat4 modelMatrix, int indMeshFile) {
	// calls rendering on the root bone (the first in the table of bones)
	render_one_bone(TabBones[pg_current_configuration_rank][indMeshFile], modelMatrix);
}


//////////////////////////////////////////////////////////////////
// MESH ARMATURE ANIMATION
//////////////////////////////////////////////////////////////////
void copyLibraryPoseToAnimationPose(int indMeshFile, int chosen_mesh_LibraryPose, int mesh_AnimationPose) {
	if (indMeshFile < int(pg_Meshes[pg_current_configuration_rank].size())
		&& chosen_mesh_LibraryPose < pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile] 
		&& mesh_AnimationPose < PG_MAX_ANIMATION_POSES) {
		for (int indBone = 0; indBone < pg_nb_bones[pg_current_configuration_rank][indMeshFile]; indBone++) {
			Bone* curBone = &TabBones[pg_current_configuration_rank][indMeshFile][indBone];
			curBone->boneAnimationPoseRotationQuat[mesh_AnimationPose] = curBone->boneLibraryPoseRotationQuat[chosen_mesh_LibraryPose];
			curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose] = curBone->boneLibraryPoseTranslationVector[chosen_mesh_LibraryPose];
		}
	}
	else {
		sprintf(ErrorStr, "Error: incorrect mesh index %d (max %d) library pose index %d (max %d)  animation pose index %d (max %d) !",
			indMeshFile, pg_Meshes[pg_current_configuration_rank].size(), chosen_mesh_LibraryPose, 
			pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile], 
			mesh_AnimationPose, PG_MAX_ANIMATION_POSES); ReportError(ErrorStr);
	}
}


void update_anim(int indMeshFile) {
#if defined(var_Contact_mesh_anime)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_mesh_anime]) {
		if (Contact_mesh_anime < 0) {
			mesh_anime_precTime[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;
		}
		bool new_anim = Contact_mesh_anime != mesh_precedingAnime[pg_current_configuration_rank][indMeshFile];
		if (new_anim) {
			mesh_startAnime[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;
			mesh_precedingAnime[pg_current_configuration_rank][indMeshFile] = Contact_mesh_anime;
		}
		switch (Contact_mesh_anime) {
			// frozen
		case 0: {
			mesh_startAnime[pg_current_configuration_rank][indMeshFile] += pg_CurrentClockTime - mesh_anime_precTime[pg_current_configuration_rank][indMeshFile];
		}
			  break;
			  // binary 0 - n
		case 1: {
			if (new_anim) {
				chosen_mesh_LibraryPose1 = 0;
				chosen_mesh_LibraryPose2 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0];
			break;
		}
			  // binary n1 / n2
		case 2: {
			if (new_anim) {
				chosen_mesh_LibraryPose1 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				chosen_mesh_LibraryPose2 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			float Pose0 = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0] = Pose0;
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - Pose0;
			break;
		}
			  // ternary n1 / n2 / n3
		case 3: {
			if (new_anim) {
				chosen_mesh_LibraryPose1 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				chosen_mesh_LibraryPose2 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				chosen_mesh_LibraryPose3 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose3, 2);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(3, PG_MAX_ANIMATION_POSES);
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			float pose = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			float Pose1, Pose2, Pose3;
			if (pose < 1.f / 3.f) {
				Pose1 = 3.f * pose;
				Pose2 = 1.f - Pose1;
				Pose3 = 0.f;
			}
			else if (pose < 2.f / 3.f) {
				Pose1 = 3.f * (2.f / 3.f - pose);
				Pose2 = 0.f;
				Pose3 = 1.f - Pose1;
			}
			else {
				Pose1 = 0.f;
				Pose2 = 1.f - 3.f * (1.f - pose);
				Pose3 = 1.f - Pose2;
			}
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0] = Pose1;
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][1] = Pose2;
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][2] = Pose3;
			break;
		}
			  // binary rand(n) / rand(n')
		case 4: {
			if (new_anim) {
				chosen_mesh_LibraryPose1 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				chosen_mesh_LibraryPose2 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = false;
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = false;
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			float Pose0 = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			if (Pose0 > 1.f - 0.01f && mesh_positiveChange[pg_current_configuration_rank][indMeshFile] == false) {
				chosen_mesh_LibraryPose2 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				//printf("pose %.2f new anim1 %d anim2 %d\n", Pose0, chosen_mesh_LibraryPose1, chosen_mesh_LibraryPose2);
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = true;
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = false;
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			if (Pose0 < 0.01f && mesh_negativeChange[pg_current_configuration_rank][indMeshFile] == false) {
				chosen_mesh_LibraryPose1 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				//printf("pose %.2f anim1 %d new anim2 %d\n", Pose0, chosen_mesh_LibraryPose1, chosen_mesh_LibraryPose2);
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = true;
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = false;
				for (int indPose = 0; indPose < PG_MAX_ANIMATION_POSES; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0] = Pose0;
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - Pose0;
			break;
		}
			  // binary n1 / n2 wo interpolation
		case 5: {
			float pose = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			if (new_anim || (pose > 1.f - 0.01f && mesh_positiveChange[pg_current_configuration_rank][indMeshFile] == false)) {
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = true;
				chosen_mesh_LibraryPose1 = int(rand_0_1 * pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]) % pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile];
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(1, PG_MAX_ANIMATION_POSES);
				pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][chosen_mesh_LibraryPose1] = 1.f;
			}
			if (pose < 0.01f && mesh_positiveChange[pg_current_configuration_rank][indMeshFile] == true) {
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = false;
			}
			break;
		}
			  // fingers counting
		case _lastMesh_Anime: {
			if (new_anim) {
				chosen_mesh_LibraryPose1 = min(7, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]);
				chosen_mesh_LibraryPose2 = min(8, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]);
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = false;
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = false;
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			float Pose0 = float((sin((pg_CurrentClockTime - mesh_startAnime[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
			if (Pose0 > 1.f - 0.01f && mesh_positiveChange[pg_current_configuration_rank][indMeshFile] == false) {
				chosen_mesh_LibraryPose2 = (chosen_mesh_LibraryPose2 + 1);
				if (chosen_mesh_LibraryPose2 > min(12, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile])) {
					chosen_mesh_LibraryPose2 = min(7, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]);
				}
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose2, 1);
				//printf("pose %.2f new anim1 %d anim2 %d\n", Pose0, chosen_mesh_LibraryPose1, chosen_mesh_LibraryPose2);
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = true;
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = false;
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			if (Pose0 < 0.01f && mesh_negativeChange[pg_current_configuration_rank][indMeshFile] == false) {
				chosen_mesh_LibraryPose1 = (chosen_mesh_LibraryPose1 + 1);
				if (chosen_mesh_LibraryPose1 > min(12, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile])) {
					chosen_mesh_LibraryPose1 = min(7, pg_nb_LibraryPoses[pg_current_configuration_rank][indMeshFile]);
				}
				copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose1, 0);
				//printf("pose %.2f anim1 %d new anim2 %d\n", Pose0, chosen_mesh_LibraryPose1, chosen_mesh_LibraryPose2);
				mesh_negativeChange[pg_current_configuration_rank][indMeshFile] = true;
				mesh_positiveChange[pg_current_configuration_rank][indMeshFile] = false;
				for (int indPose = 0; indPose < PG_MAX_ANIMATION_POSES; indPose++) {
					pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][0] = Pose0;
			pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - Pose0;
			break;
		}
		}
		mesh_anime_precTime[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;
	}
#endif
}

#if defined(var_Contact_mesh_motion)
void Contact_update_motion(int indMeshFile) {
	//pg_Mesh_Translation_X[indMesh] = (Contact_mesh_translation_X + Contact_mesh_translation_X_pulse * (pulse_average - 0.5f)) * (indMesh % 2 == 0 ? 1 : -1);
	//pg_Mesh_Translation_Y[indMesh] = Contact_mesh_translation_Y + Contact_mesh_translation_Y_pulse * (pulse_average - 0.5f);
	//pg_Mesh_Rotation_angle[indMesh] = (Contact_mesh_rotation + Contact_mesh_rotation_pulse * (pulse_average - 0.5f)) * (indMesh % 2 == 0 ? 1 : -1);
	//pg_Mesh_Scale[indMesh] = Contact_mesh_scale + Contact_mesh_scale_pulse * (pulse_average - 0.5f);

	if (Contact_mesh_motion < 0) {
		mesh_motion_precTime[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;
	}
	bool new_motion = Contact_mesh_motion != mesh_precedingMotion[pg_current_configuration_rank][indMeshFile];
	bool with_motion = false;
	if (new_motion) {
		mesh_startMotion[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;
		mesh_precedingMotion[pg_current_configuration_rank][indMeshFile] = Contact_mesh_motion;
	}
	switch (Contact_mesh_motion) {
		// frozen
	case 0: {
		mesh_startMotion[pg_current_configuration_rank][indMeshFile] += pg_CurrentClockTime - mesh_motion_precTime[pg_current_configuration_rank][indMeshFile];
	}
		  break;
		  // binary n1 / n2 // clapping hands
	case 1: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = 30.f * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = 5.f * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(PI / 4) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = pg_Mesh_Rotation_Ini_X[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = pg_Mesh_Rotation_Ini_X[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = pg_Mesh_Rotation_Ini_Y[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = pg_Mesh_Rotation_Ini_Y[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = pg_Mesh_Rotation_Ini_Z[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = pg_Mesh_Rotation_Ini_Z[pg_current_configuration_rank][indMeshFile];
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	case 2: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 30.f : -5.f);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 5.f : -30.f);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(PI / 4);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = pg_Mesh_Rotation_Ini_X[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = pg_Mesh_Rotation_Ini_X[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = pg_Mesh_Rotation_Ini_Y[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = pg_Mesh_Rotation_Ini_Y[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = pg_Mesh_Rotation_Ini_Z[pg_current_configuration_rank][indMeshFile];
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = pg_Mesh_Rotation_Ini_Z[pg_current_configuration_rank][indMeshFile];
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_anim_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	case 3: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = Contact_mesh_translation_X * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = Contact_mesh_translation_X * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = float(-PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = 1.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = 1.f;
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	case 4: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = float(rand_0_1 * 60 - 30);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = float(rand_0_1 * 60 - 30);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = Contact_mesh_translation_Y;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = float(rand_0_1 * PI / 3 - PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(rand_0_1 * PI / 3 - PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	case 5: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = Contact_mesh_translation_X * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = Contact_mesh_translation_X * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = float(rand_0_1 * 5 - 2.5);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = float(rand_0_1 * 5 - 2.5);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = float(-PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(PI / 6);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = 1.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = 1.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = 0.f;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = 0.f;
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	case _lastMesh_Motion: {
		if (new_motion) {
			pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
			for (int indPose = 0; indPose < pg_nb_MotionPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][indPose] = 0.f;
			}
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X = float(rand_0_1 * 100 - 50);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X = float(rand_0_1 * 100 - 50);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y = float(rand_0_1 * 10 - 5);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y = float(rand_0_1 * 10 - 5);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle = float(rand_0_1 * 2 * PI - PI);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle = float(rand_0_1 * 2 * PI - PI);
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale = Contact_mesh_scale;
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
			pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
		}
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] = float((sin((pg_CurrentClockTime - mesh_startMotion[pg_current_configuration_rank][indMeshFile]) * Contact_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] = 1.f - pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0];
		with_motion = true;
		break;
	}
	}
	mesh_motion_precTime[pg_current_configuration_rank][indMeshFile] = pg_CurrentClockTime;

	// updates the rotation and translations of the mesh from the motion poses and interpolation coeficients
	if (with_motion) {
		pg_Mesh_Translation_X[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_X
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_X;
		pg_Mesh_Translation_Y[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Translation_Y
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Translation_Y;
		pg_Mesh_Rotation_angle[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_angle
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_angle;
		pg_Mesh_Scale[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Scale
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Scale;
		pg_Mesh_Rotation_X[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_X
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_X;
		pg_Mesh_Rotation_Y[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Y
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Y;
		pg_Mesh_Rotation_Z[pg_current_configuration_rank][indMeshFile] = pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][0] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][0].pose_Mesh_Rotation_Z
			+ pg_interpolation_weight_MotionPose[pg_current_configuration_rank][indMeshFile][1] * pg_motionPoses[pg_current_configuration_rank][indMeshFile][1].pose_Mesh_Rotation_Z;
	}
	else {
		pg_Mesh_Translation_X[pg_current_configuration_rank][indMeshFile] = Contact_mesh_translation_X * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_Mesh_Translation_Y[pg_current_configuration_rank][indMeshFile] = Contact_mesh_translation_Y;
		pg_Mesh_Rotation_angle[pg_current_configuration_rank][indMeshFile] = Contact_mesh_rotation * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_Mesh_Scale[pg_current_configuration_rank][indMeshFile] = Contact_mesh_scale;
		pg_Mesh_Rotation_X[pg_current_configuration_rank][indMeshFile] = pg_Mesh_Rotation_Ini_X[pg_current_configuration_rank][indMeshFile];
		pg_Mesh_Rotation_Y[pg_current_configuration_rank][indMeshFile] = pg_Mesh_Rotation_Ini_Y[pg_current_configuration_rank][indMeshFile];
		pg_Mesh_Rotation_Z[pg_current_configuration_rank][indMeshFile] = pg_Mesh_Rotation_Ini_Z[pg_current_configuration_rank][indMeshFile];
	}
	// pulse update
	pg_Mesh_Translation_X[pg_current_configuration_rank][indMeshFile] = (pg_Mesh_Translation_X[pg_current_configuration_rank][indMeshFile] + Contact_mesh_translation_X_pulse * (pulse_average - 0.5f));
	pg_Mesh_Translation_Y[pg_current_configuration_rank][indMeshFile] = pg_Mesh_Translation_Y[pg_current_configuration_rank][indMeshFile] + Contact_mesh_translation_Y_pulse * (pulse_average - 0.5f);
	pg_Mesh_Rotation_angle[pg_current_configuration_rank][indMeshFile] = (pg_Mesh_Rotation_angle[pg_current_configuration_rank][indMeshFile] + Contact_mesh_rotation_pulse * (pulse_average - 0.5f));
	pg_Mesh_Scale[pg_current_configuration_rank][indMeshFile] = pg_Mesh_Scale[pg_current_configuration_rank][indMeshFile] + Contact_mesh_scale_pulse * (pulse_average - 0.5f);

}
#endif

void update_bones(int indMeshFile) {
	// calls rendering on the root bone (the first in the table of bones)
	for (int indBone = 0; indBone < pg_nb_bones[pg_current_configuration_rank][indMeshFile]; indBone++) {
		Bone* curBone = &TabBones[pg_current_configuration_rank][indMeshFile][indBone];

		// rotation interpolation for poses
		if (pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] > 0) {
			glm::quat animQuat(1.0, 0.0, 0.0, 0.0);
			float weight_sum = 0.f;
			for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
				weight_sum += pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose];
				if (indPose == 0) {
					animQuat = curBone->boneAnimationPoseRotationQuat[0];
				}
				else {
					if (pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] > 0 && weight_sum > 0) {
						animQuat = glm::mix(animQuat,
							curBone->boneAnimationPoseRotationQuat[indPose], pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] / weight_sum);
					}
				}
			}
			curBone->boneAnimationRotationMatrix = glm::toMat4(animQuat);
		}
		else {
			curBone->boneAnimationRotationMatrix = curBone->boneInitialRotationMatrix;
		}

		// only root bone(s) have variable translations
		if (curBone->parentBone == NULL) {
			if (pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile] > 0) {
				glm::vec3 animTransl(0);
				float weight_sum = 0.f;
				for (int indPose = 0; indPose < pg_nb_AnimationPoses[pg_current_configuration_rank][indMeshFile]; indPose++) {
					weight_sum += pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose];
					if (indPose == 0) {
						animTransl = curBone->boneAnimationPoseTranslationVector[0];
					}
					else {
						if (pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] > 0 && weight_sum > 0) {
							animTransl = glm::mix(animTransl,
								curBone->boneAnimationPoseTranslationVector[indPose], pg_interpolation_weight_AnimationPose[pg_current_configuration_rank][indMeshFile][indPose] / weight_sum);
						}
					}
				}
				curBone->boneAnimationTranslationMatrix = glm::translate(glm::mat4(1.0f), animTransl);
			}
			else {
				curBone->boneAnimationTranslationMatrix = curBone->boneInitialTranslationMatrix;
			}
		}

		// computes the initial joint transformation matrices from
		// the initial rotation and translation matrices (should be 
		// made once and for all in the future)
		if (curBone->parentBone) {
			// initial joint transformation
			// should be computed only once in an independent function call
			glm::mat4 m1 = curBone->boneInitialTranslationMatrix
				* curBone->boneInitialRotationMatrix;

			curBone->initialJointTransformation =
				curBone->parentBone->initialJointTransformation
				* m1;

			// current joint transformation
			curBone->currentJointTransformation =
				curBone->parentBone->currentJointTransformation
				* m1
				* curBone->boneAnimationRotationMatrix;
		}
		else {
			// initial joint transformation
			// should be computed only once in an independent function call
			curBone->initialJointTransformation =
				curBone->boneInitialTranslationMatrix * curBone->boneInitialRotationMatrix;

			// current joint transformation
			curBone->currentJointTransformation =
				curBone->initialJointTransformation * curBone->boneAnimationRotationMatrix;
		}

		// the point coordinates are in the mesh local coordinate system
		glm::mat4 mj_1;
		mj_1 = glm::inverse(curBone->initialJointTransformation);
		// updates the matrix used to animate points
		curBone->pointAnimationMatrix = curBone->currentJointTransformation * mj_1;
	}
}
