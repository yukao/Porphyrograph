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
GLfloat **pointBuffer = NULL;
GLfloat **texCoordBuffer = NULL;
GLfloat **normalBuffer = NULL;
GLuint  **indexBuffer = NULL;

// shader variable pointers
GLint *uniform_mesh_model = NULL;
GLint *uniform_mesh_view = NULL;
GLint *uniform_mesh_proj = NULL;
GLint *uniform_mesh_light = NULL;

//////////////////////////////////////////////////////////////////
// MESH AND KEYPOINT FILE PARSING
//////////////////////////////////////////////////////////////////

// the linearization for OpenGL replaces indices associated with each face in obj format
// by coordinates copied from these temporary buffers (through copyMeshData)
void copyMeshData(  int indMeshInFile , GLfloat *pointBufferIni , GLfloat *texCoordBufferIni , GLfloat *normalBufferIni ,
					GLuint  *indexPointBufferIni, GLuint  *indexTexCoordBufferIni, GLuint  *indexNormalBufferIni,
					int nbFacesInThisMesh) {
  	pointBuffer[indMeshInFile] = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 3 * sizeof *pointBuffer);
	texCoordBuffer[indMeshInFile] = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 2 * sizeof *texCoordBuffer);
	normalBuffer[indMeshInFile] = (GLfloat *)malloc(nbFacesInThisMesh * 3 * 3 * sizeof *normalBuffer);
	indexBuffer[indMeshInFile] = (GLuint *)malloc( nbFacesInThisMesh * 3 * sizeof *indexBuffer);
	for( int indFace = 0 ; indFace < nbFacesInThisMesh ; indFace++ ) {
	  // copies the indices in the index table
	  indexBuffer[indMeshInFile][ indFace * 3 ] = indFace * 3;
	  indexBuffer[indMeshInFile][ indFace * 3 + 1 ] = indFace * 3 + 1;
	  indexBuffer[indMeshInFile][ indFace * 3 + 2 ] = indFace * 3 + 2;

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indVertex1 = indexPointBufferIni[ indFace * 3 ];
	  int indVertex2 = indexPointBufferIni[ indFace * 3 + 1 ];
	  int indVertex3 = indexPointBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indPoint %d %d %d %d\n" , indFace ,indVertex1,indVertex2,indVertex3);
	  pointBuffer[indMeshInFile][ indFace * 3 * 3 ]     = pointBufferIni[ indVertex1 * 3 ];
	  pointBuffer[indMeshInFile][ indFace * 3 * 3 + 1 ] = pointBufferIni[ indVertex1 * 3 + 1 ];
	  pointBuffer[indMeshInFile][ indFace * 3 * 3 + 2 ] = pointBufferIni[ indVertex1 * 3 + 2 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 ] = pointBufferIni[ indVertex2 * 3 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 1 ] = pointBufferIni[ indVertex2 * 3 + 1 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 2 ] = pointBufferIni[ indVertex2 * 3 + 2 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 ] = pointBufferIni[ indVertex3 * 3 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 1 ] = pointBufferIni[ indVertex3 * 3 + 1 ];
	  pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 2 ] = pointBufferIni[ indVertex3 * 3 + 2 ];
	 // printf("Face point %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		//pointBuffer[indMeshInFile][ indFace * 3 * 3 ] , pointBuffer[indMeshInFile][ indFace * 3 * 3 + 1 ] , pointBuffer[indMeshInFile][ indFace * 3 * 3 + 2 ] ,
		//pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 ] , pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 1 ] , pointBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 2 ] ,
		//pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 ] , pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 1 ] , pointBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 2 ] );

	  // copies the texture coordinates from the initial buffer to the final one
	  int indexTexCoord1 = indexTexCoordBufferIni[ indFace * 3 ];
	  int indexTexCoord2 = indexTexCoordBufferIni[ indFace * 3 + 1 ];
	  int indexTexCoord3 = indexTexCoordBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indTexC %d %d %d %d\n" , indFace ,indexTexCoord1,indexTexCoord2,indexTexCoord3);
	  texCoordBuffer[indMeshInFile][ indFace * 3 * 2 ]     = texCoordBufferIni[ indexTexCoord1 * 2 ];
	  texCoordBuffer[indMeshInFile][ indFace * 3 * 2 + 1 ] = texCoordBufferIni[ indexTexCoord1 * 2 + 1 ];
	  texCoordBuffer[indMeshInFile][ (indFace * 3 + 1) * 2 ] = texCoordBufferIni[ indexTexCoord2 * 2 ];
	  texCoordBuffer[indMeshInFile][ (indFace * 3 + 1) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord2 * 2 + 1 ];
	  texCoordBuffer[indMeshInFile][ (indFace * 3 + 2) * 2 ] = texCoordBufferIni[ indexTexCoord3 * 2 ];
	  texCoordBuffer[indMeshInFile][ (indFace * 3 + 2) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord3 * 2 + 1 ];

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indNormal1 = indexNormalBufferIni[indFace * 3];
	  int indNormal2 = indexNormalBufferIni[indFace * 3 + 1];
	  int indNormal3 = indexNormalBufferIni[indFace * 3 + 2];
	  // printf("Face indNormal %d %d %d %d\n" , indFace ,indNormal1,indNormal2,indNormal3);
	  normalBuffer[indMeshInFile][indFace * 3 * 3] = normalBufferIni[indNormal1 * 3];
	  normalBuffer[indMeshInFile][indFace * 3 * 3 + 1] = normalBufferIni[indNormal1 * 3 + 1];
	  normalBuffer[indMeshInFile][indFace * 3 * 3 + 2] = normalBufferIni[indNormal1 * 3 + 2];
	  normalBuffer[indMeshInFile][(indFace * 3 + 1) * 3] = normalBufferIni[indNormal2 * 3];
	  normalBuffer[indMeshInFile][(indFace * 3 + 1) * 3 + 1] = normalBufferIni[indNormal2 * 3 + 1];
	  normalBuffer[indMeshInFile][(indFace * 3 + 1) * 3 + 2] = normalBufferIni[indNormal2 * 3 + 2];
	  normalBuffer[indMeshInFile][(indFace * 3 + 2) * 3] = normalBufferIni[indNormal3 * 3];
	  normalBuffer[indMeshInFile][(indFace * 3 + 2) * 3 + 1] = normalBufferIni[indNormal3 * 3 + 1];
	  normalBuffer[indMeshInFile][(indFace * 3 + 2) * 3 + 2] = normalBufferIni[indNormal3 * 3 + 2];
	  // printf("Face normal %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		 //normalBuffer[indMeshInFile][ indFace * 3 * 3 ] , normalBuffer[indMeshInFile][ indFace * 3 * 3 + 1 ] , normalBuffer[indMeshInFile][ indFace * 3 * 3 + 2 ] ,
		 //normalBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 ] , normalBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 1 ] , normalBuffer[indMeshInFile][ (indFace * 3 + 1) * 3 + 2 ] ,
		 //normalBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 ] , normalBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 1 ] , normalBuffer[indMeshInFile][ (indFace * 3 + 2) * 3 + 2 ] );
	  // printf("Face texC %d %.2f/%.2f %.2f/%.2f %.2f/%.2f  \n" , indFace ,
		//texCoordBuffer[indMeshInFile][ indFace * 3 * 2  ] , texCoordBuffer[indMeshInFile][ indFace * 3 * 2 + 1 ] ,
		//texCoordBuffer[indMeshInFile][ (indFace * 3 + 1) * 2 ] , texCoordBuffer[indMeshInFile][ (indFace * 3 + 1) * 2 + 1 ] ,
		//texCoordBuffer[indMeshInFile][ (indFace * 3 + 2) * 2 ] , texCoordBuffer[indMeshInFile][ (indFace * 3 + 2) * 2 + 1 ]  );
	}
}

// OBJ file parsing (Alias Wavefront ASCII format)
#define MAX_MESHES 100
void count_faces_mesh_obj(FILE *file, int *meshNo,
	int **nbVerticesInEachMesh, int **nbTextCoordsInEachMesh, int **nbNormalsInEachMesh,
	int **nbFacesInEachMesh) {
	char    tag[256];
	char    line[256];
	int     nbVerticesInEachMeshTmp[MAX_MESHES];
	int     nbTextCoordsInEachMeshTmp[MAX_MESHES];
	int     nbNormalsInEachMeshTmp[MAX_MESHES];
	int     nbFacesInEachMeshTmp[MAX_MESHES];

	// Two comment lines
	// # Blender3D v244 OBJ File: Anime_Girl.blend
	// # www.blender3d.org
	if (!fgets(line, 256, file)) { return; }
	if (!fgets(line, 256, file)) { return; }

	if (!fgets(line, 256, file)) { return; }
	// material name
	if (strcmp(tag, "mtllib") == 0) {
		sscanf(line, "%s", tag);
		// reads next line: object
		if (!fgets(line, 256, file)) { return; }
	}
	else {
		// we are on object line
	}

	// mesh ID
	sscanf(line, "%s", tag);

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
					//printf("Mesh Count %d Vertices %d texCoords %d normals %d faces %d\n",
					//	*meshNo, (*nbVerticesInEachMesh)[indMesh], (*nbTextCoordsInEachMesh)[indMesh], (*nbNormalsInEachMesh)[indMesh], (*nbFacesInEachMesh)[indMesh]);
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
					printf("Mesh %d Vertices %d texCoords %d normals %d faces %d\n",
						*meshNo, (*nbVerticesInEachMesh)[indMesh], (*nbTextCoordsInEachMesh)[indMesh], (*nbNormalsInEachMesh)[indMesh], (*nbFacesInEachMesh)[indMesh]);
				}
				return;
			}
		}

		// another mesh follows in the same obj file
		(*meshNo)++;
	}
}

// OBJ file parsing (Alias Wavefront ASCII format)
void parse_mesh_obj(FILE *file, int indMeshFile, int nbMeshes,
	int *nbVerticesInEachMesh, int *nbTextCoordsInEachMesh, int *nbNormalsInEachMesh,
	int *nbFacesInEachMesh) {
	char    tag[256];
	char    meshString[256];
	char    line[256];

	// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
	// the buffers of vertices, tex coords, and normals, and the associated counts are temporary
	// they are then used for linearization for OpenGL
	// the linearization for OpenGL replaces indices associated with each face in obj format
	// by coordinates copied from these temporary buffers (through copyMeshData)
	GLfloat *pointBufferIni = NULL;
	GLfloat *texCoordBufferIni = NULL;
	GLfloat *normalBufferIni = NULL;
	GLuint  *indexPointBufferIni = NULL;
	GLuint  *indexTexCoordBufferIni = NULL;
	GLuint  *indexNormalBufferIni = NULL;

	// local size of mesh
	// initial count of mesh elements, before linearization by copyMeshData for OpenGL
	// after linearization there are 3 vertices, texCoords, and normals per face
	int *nbPointsMeshIni = new int[nbMeshes];
	int *nbTexCoordsMeshIni = new int[nbMeshes];
	int *nbNormalsMeshIni = new int[nbMeshes];

	int indMesh = 0;

	// Two comment lines
	// # Blender3D v244 OBJ File: Anime_Girl.blend
	// # www.blender3d.org
	if (!fgets(line, 256, file)) { return; }
	if (!fgets(line, 256, file)) { return; }

	if (!fgets(line, 256, file)) { return; }
	// material name
	if (strcmp(tag, "mtllib") == 0) {
		sscanf(line, "%s", tag);
		// reads next line: object
		if (!fgets(line, 256, file)) { return; }
	}
	else {
		// we are on object line
	}

	// mesh ID
	sscanf(line, "%s", tag);

	int nbVertexTot = 0;
	int nbCoordTexTot = 0;
	int nbNormalTot = 0;
	while (strcmp(tag, "o") == 0) {
		if (indMesh >= nbMeshes) {
			printf("Error: Excessive number of Meshes, max %d\n", nbMeshes);
			throw 0;
		}

		// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
		pointBufferIni = (GLfloat *)malloc(nbVerticesInEachMesh[indMesh] * 3 * sizeof *pointBufferIni);
		texCoordBufferIni = (GLfloat *)malloc(nbTextCoordsInEachMesh[indMesh] * 2 * sizeof *texCoordBufferIni);
		normalBufferIni = (GLfloat *)malloc(nbNormalsInEachMesh[indMesh] * 3 * sizeof *normalBufferIni);
		indexTexCoordBufferIni = (GLuint *)malloc(nbFacesInEachMesh[indMesh] * 3 * sizeof *indexTexCoordBufferIni);
		indexPointBufferIni = (GLuint *)malloc(nbFacesInEachMesh[indMesh] * 3 * sizeof *indexPointBufferIni);
		indexNormalBufferIni = (GLuint *)malloc(nbFacesInEachMesh[indMesh] * 3 * sizeof *indexNormalBufferIni);

		// initial count of mesh elements, before linearization by copyMeshData for OpenGL, local value
		nbPointsMeshIni[indMesh] = 0;
		nbTexCoordsMeshIni[indMesh] = 0;
		nbNormalsMeshIni[indMesh] = 0;

		// number faces per mesh, non local value
		nbFacesInEachMesh[indMesh] = 0;

		// mesh ID
		sscanf(line, "%s %s", tag, meshString);
		mesh_IDs[indMeshFile][indMesh] = string(meshString);

		// next tag
		if (!fgets(line, 256, file)) { return; }
		sscanf(line, "%s", tag);

		// Scan for Verts in this mesh
		while (strcmp(tag, "v") == 0) {
			if (nbPointsMeshIni[indMesh] >= nbVerticesInEachMesh[indMesh]) {
				printf("Error: Excessive number of vertices\n");
				throw 0;
			}
			sscanf(line, "%s %f %f %f",
				tag, &pointBufferIni[nbPointsMeshIni[indMesh] * 3],
				&pointBufferIni[nbPointsMeshIni[indMesh] * 3 + 1],
				&pointBufferIni[nbPointsMeshIni[indMesh] * 3 + 2]);
			nbPointsMeshIni[indMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for text coords in this mesh
		while (strcmp(tag, "vt") == 0) {
			if (nbTexCoordsMeshIni[indMesh] > nbTextCoordsInEachMesh[indMesh]) {
				printf("Error: Excessive number of texCoords\n");
				throw 0;
			}
			sscanf(line, "%s %f %f", tag, &texCoordBufferIni[nbTexCoordsMeshIni[indMesh] * 2],
				&texCoordBufferIni[nbTexCoordsMeshIni[indMesh] * 2 + 1]);
			//printf("vt %d %.2f %.2f\n",nbTexCoordsMeshIni[indMesh],texCoordBufferIni[nbTexCoordsMeshIni[indMesh] * 2],
				  //	texCoordBufferIni[nbTexCoordsMeshIni[indMesh] * 2+1]);
			nbTexCoordsMeshIni[indMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for normals in this mesh
		while (strcmp(tag, "vn") == 0) {
			if (nbNormalsMeshIni[indMesh] > nbNormalsInEachMesh[indMesh]) {
				printf("Error: Excessive number of texCoords\n");
				throw 0;
			}
			sscanf(line, "%s %f %f %f", tag, &normalBufferIni[nbNormalsMeshIni[indMesh] * 3],
				&normalBufferIni[nbNormalsMeshIni[indMesh] * 3 + 1], &normalBufferIni[nbNormalsMeshIni[indMesh] * 3 + 2]);
			//printf("vt %d %.2f %.2f\n",nbNormalsMeshIni[indMesh],normalBufferIni[nbNormalsMeshIni[indMesh] * 3],
				  //	normalBufferIni[nbNormalsMeshIni[indMesh] * 3+1] , normalBufferIni[nbNormalsMeshIni[indMesh] * 3+2]);
			nbNormalsMeshIni[indMesh]++;

			if (!fgets(line, 256, file)) { return; }
			sscanf(line, "%s", tag);
		}

		// Scan for Faces in this mesh
		while (strcmp(tag, "f") == 0
			|| strcmp(tag, "usemtl") == 0
			|| strcmp(tag, "s") == 0) {
			if (strcmp(tag, "f") == 0) {
				if (nbFacesInEachMesh[indMesh] > nbFacesInEachMesh[indMesh]) {
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
				// pointBufferIniPtr should be used because pointBufferIni has been
				// incremented during texCoords scanning
				indexPointBufferIni[nbFacesInEachMesh[indMesh] * 3] = indVertex1 - nbVertexTot;
				indexPointBufferIni[nbFacesInEachMesh[indMesh] * 3 + 1] = indVertex2 - nbVertexTot;
				indexPointBufferIni[nbFacesInEachMesh[indMesh] * 3 + 2] = indVertex3 - nbVertexTot;

				indexTexCoordBufferIni[nbFacesInEachMesh[indMesh] * 3] = indexTexCoord1 - nbCoordTexTot;
				indexTexCoordBufferIni[nbFacesInEachMesh[indMesh] * 3 + 1] = indexTexCoord2 - nbCoordTexTot;
				indexTexCoordBufferIni[nbFacesInEachMesh[indMesh] * 3 + 2] = indexTexCoord3 - nbCoordTexTot;

				indexNormalBufferIni[nbFacesInEachMesh[indMesh] * 3] = indexNormal1 - nbNormalTot;
				indexNormalBufferIni[nbFacesInEachMesh[indMesh] * 3 + 1] = indexNormal2 - nbNormalTot;
				indexNormalBufferIni[nbFacesInEachMesh[indMesh] * 3 + 2] = indexNormal3 - nbNormalTot;

				nbFacesInEachMesh[indMesh]++;
			}

			// last line of the obj file, last mesh in this file
			// copies the data (and possibly duplicate them)
			if (!fgets(line, 256, file)) {
				copyMeshData(indMesh, pointBufferIni, texCoordBufferIni, normalBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesInEachMesh[indMesh]);
				nbVertexTot += nbPointsMeshIni[indMesh];
				nbCoordTexTot += nbTexCoordsMeshIni[indMesh];
				nbNormalTot += nbNormalsMeshIni[indMesh];
				/*
				if (indMeshFile == 0) {
					printf("Final Mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
						indMesh, mesh_IDs[indMeshFile][indMesh].c_str(), nbPointsMeshIni[indMesh], nbTexCoordsMeshIni[indMesh], nbNormalsMeshIni[indMesh], nbFacesInEachMesh[indMesh]);
					for (int indF = 0; indF < nbFacesInEachMesh[indMesh]; indF++) {
						printf("indices %d %d %d\n", indexBuffer[indMesh][3 * indF], indexBuffer[indMesh][3 * indF + 1], indexBuffer[indMesh][3 * indF + 2]);
						printf("vertices %.2f %.2f %.2f        %.2f %.2f %.2f        %.2f %.2f %.2f\n",
							pointBuffer[indMesh][(3 * indF) * 3], pointBuffer[indMesh][(3 * indF) * 3 + 1], pointBuffer[indMesh][(3 * indF) * 3 + 2],
							pointBuffer[indMesh][(3 * indF + 1) * 3], pointBuffer[indMesh][(3 * indF + 1) * 3 + 1], pointBuffer[indMesh][(3 * indF + 1) * 3 + 2],
							pointBuffer[indMesh][(3 * indF + 2) * 3], pointBuffer[indMesh][(3 * indF + 2) * 3 + 1], pointBuffer[indMesh][(3 * indF + 2) * 3 + 2]);
						printf("texCoords %.2f %.2f         %.2f %.2f        %.2f %.2f\n",
							texCoordBuffer[indMesh][(3 * indF) * 2], texCoordBuffer[indMesh][(3 * indF) * 2 + 1],
							texCoordBuffer[indMesh][(3 * indF + 1) * 2], texCoordBuffer[indMesh][(3 * indF + 1) * 2 + 1],
							texCoordBuffer[indMesh][(3 * indF + 2) * 2], texCoordBuffer[indMesh][(3 * indF + 2) * 2 + 1]);
						printf("normals %.2f %.2f %.2f         %.2f %.2f %.2f        %.2f %.2f %.2f\n\n",
							normalBuffer[indMesh][(3 * indF) * 3], normalBuffer[indMesh][(3 * indF) * 3 + 1], normalBuffer[indMesh][(3 * indF) * 3 + 2],
							normalBuffer[indMesh][(3 * indF + 1) * 3], normalBuffer[indMesh][(3 * indF + 1) * 3 + 1], normalBuffer[indMesh][(3 * indF + 1) * 3 + 2],
							normalBuffer[indMesh][(3 * indF + 2) * 3], normalBuffer[indMesh][(3 * indF + 2) * 3 + 1], normalBuffer[indMesh][(3 * indF + 2) * 3 + 2]);
					}
				}
				*/
				(indMesh)++;

				// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
				free(pointBufferIni);
				free(texCoordBufferIni);
				free(normalBufferIni);
				free(indexPointBufferIni);
				free(indexTexCoordBufferIni);
				free(indexNormalBufferIni);

				// initial count of mesh elements, before linearization by copyMeshData for OpenGL
				// after linearization there are 3 vertices, texCoords, and normals per face
				free(nbPointsMeshIni);
				free(nbTexCoordsMeshIni);
				free(nbNormalsMeshIni);

				return;
			}

			// printf("%s\n", line);
			sscanf(line, "%s", tag);
			// last mesh in the obj file
			if (!tag) {
				copyMeshData(indMesh, pointBufferIni, texCoordBufferIni, normalBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesInEachMesh[indMesh]);
				nbVertexTot += nbPointsMeshIni[indMesh];
				nbCoordTexTot += nbTexCoordsMeshIni[indMesh];
				nbNormalTot += nbNormalsMeshIni[indMesh];
				if (indMeshFile == 0) {
					printf("Final Mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
						indMesh, mesh_IDs[indMeshFile][indMesh].c_str(), nbPointsMeshIni[indMesh], nbTexCoordsMeshIni[indMesh], nbNormalsMeshIni[indMesh], nbFacesInEachMesh[indMesh]);
					for (int indF = 0; indF < nbFacesInEachMesh[indMesh]; indF++) {
						printf("indices %d %d %d\n", indexBuffer[indMesh][3 * indF], indexBuffer[indMesh][3 * indF + 1], indexBuffer[indMesh][3 * indF + 2]);
						printf("vertices %.2f %.2f %.2f        %.2f %.2f %.2f        %.2f %.2f %.2f\n",
							pointBuffer[indMesh][(3 * indF) * 3], pointBuffer[indMesh][(3 * indF) * 3 + 1], pointBuffer[indMesh][(3 * indF) * 3 + 2],
							pointBuffer[indMesh][(3 * indF + 1) * 3], pointBuffer[indMesh][(3 * indF + 1) * 3 + 1], pointBuffer[indMesh][(3 * indF + 1) * 3 + 2],
							pointBuffer[indMesh][(3 * indF + 2) * 3], pointBuffer[indMesh][(3 * indF + 2) * 3 + 1], pointBuffer[indMesh][(3 * indF + 2) * 3 + 2]);
						printf("texCoords %.2f %.2f         %.2f %.2f        %.2f %.2f\n",
							texCoordBuffer[indMesh][(3 * indF) * 2], texCoordBuffer[indMesh][(3 * indF) * 2 + 1],
							texCoordBuffer[indMesh][(3 * indF + 1) * 2], texCoordBuffer[indMesh][(3 * indF + 1) * 2 + 1],
							texCoordBuffer[indMesh][(3 * indF + 2) * 2], texCoordBuffer[indMesh][(3 * indF + 2) * 2 + 1]);
						printf("normals %.2f %.2f %.2f         %.2f %.2f %.2f        %.2f %.2f %.2f\n\n",
							normalBuffer[indMesh][(3 * indF) * 3], normalBuffer[indMesh][(3 * indF) * 3 + 1], normalBuffer[indMesh][(3 * indF) * 3 + 2],
							normalBuffer[indMesh][(3 * indF + 1) * 3], normalBuffer[indMesh][(3 * indF + 1) * 3 + 1], normalBuffer[indMesh][(3 * indF + 1) * 3 + 2],
							normalBuffer[indMesh][(3 * indF + 2) * 3], normalBuffer[indMesh][(3 * indF + 2) * 3 + 1], normalBuffer[indMesh][(3 * indF + 2) * 3 + 2]);
					}
				}
				(indMesh)++;

				// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
				free(pointBufferIni);
				free(texCoordBufferIni);
				free(normalBufferIni);
				free(indexPointBufferIni);
				free(indexTexCoordBufferIni);
				free(indexNormalBufferIni);

				// initial count of mesh elements, before linearization by copyMeshData for OpenGL
				// after linearization there are 3 vertices, texCoords, and normals per face
				free(nbPointsMeshIni);
				free(nbTexCoordsMeshIni);
				free(nbNormalsMeshIni);

				return;
			}
		}

		// another mesh follows in the same obj file
		copyMeshData(indMesh, pointBufferIni, texCoordBufferIni, normalBufferIni,
			indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
			nbFacesInEachMesh[indMesh]);
		nbVertexTot += nbPointsMeshIni[indMesh];
		nbCoordTexTot += nbTexCoordsMeshIni[indMesh];
		nbNormalTot += nbNormalsMeshIni[indMesh];

		// printf ( "%s", line );
		printf("Mesh %d Vertices %d texCoords %d normals %d faces %d\n",
			indMesh, nbPointsMeshIni[indMesh], nbTexCoordsMeshIni[indMesh], nbNormalsMeshIni[indMesh], nbFacesInEachMesh[indMesh]);
		(indMesh)++;

		// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
		free(pointBufferIni);
		free(texCoordBufferIni);
		free(normalBufferIni);
		free(indexPointBufferIni);
		free(indexTexCoordBufferIni);
		free(indexNormalBufferIni);
	}
}

// OBJ file parsing (Alias Wavefront ASCII format)
// nbMeshesInFile: number of meshes inside the obj file
void load_mesh_obj(string obj_file_name, int indMeshFile) {
	int *nbVerticesPerMesh = NULL;
	int *nbTexCoordsPerMesh = NULL;
	int *nbNormalsPerMesh = NULL;

	///////////////////////////////////////////////////////////
	// opens the obj file
	FILE * fileMesh = fopen(obj_file_name.c_str(), "r");
	if (!fileMesh) {
		printf("Object file %s not found\n", obj_file_name.c_str());
		exit(0);
	}

	///////////////////////////////////////////////////////////
	// parses the mesh (obj format) 
	// and loads the data of the meshes in the obj file
	printf("Loading %s\n", obj_file_name.c_str());
	int nbMeshesInFile = 0;
	count_faces_mesh_obj(fileMesh, &nbMeshesInFile, &nbVerticesPerMesh,
		&nbTexCoordsPerMesh, &nbNormalsPerMesh, &(nbFacesPerMesh[indMeshFile]));
	if (nbMeshesInFile > 0) {
		//printf("File mesh counts Nb meshes %d Nb faces %d vertices %d tex coord %d normals %d\n", nbMeshesInFile,
		//	nbFacesPerMesh[indMeshFile][0], nbVerticesPerMesh[0],
		//	nbTexCoordsPerMesh[0], nbNormalsPerMesh[0]);
	}
	else {
		printf("File mesh withouth meshes\n");
		exit(0);
	}

	mesh_IDs[indMeshFile] = new string[nbMeshesInFile];
	mesh_vao[indMeshFile] = new unsigned int[nbMeshesInFile];
	mesh_index_vbo[indMeshFile] = new unsigned int[nbMeshesInFile];
	nbMeshesPerMeshFile[indMeshFile] = nbMeshesInFile;
	rewind(fileMesh);
	parse_mesh_obj(fileMesh, indMeshFile, nbMeshesInFile, nbVerticesPerMesh,
		nbTexCoordsPerMesh, nbNormalsPerMesh, nbFacesPerMesh[indMeshFile]);
	fclose(fileMesh);

	///////////////////////////////////////////////////////////
	// vertex buffer objects and vertex array for the mesh
	unsigned int mesh_points_vbo;
	unsigned int mesh_texCoords_vbo;
	unsigned int mesh_normals_vbo;
	for (int indMeshInFile = 0; indMeshInFile < nbMeshesInFile; indMeshInFile++) {
		// 3 VBOs
		glGenBuffers(1, &mesh_points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_points_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * nbFacesPerMesh[indMeshFile][indMeshInFile] * sizeof(GLfloat),
			pointBuffer[indMeshInFile],
			GL_STATIC_DRAW);

		glGenBuffers(1, &mesh_texCoords_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_texCoords_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			2 * 3 * nbFacesPerMesh[indMeshFile][indMeshInFile] * sizeof(GLfloat),
			texCoordBuffer[indMeshInFile],
			GL_STATIC_DRAW);

		glGenBuffers(1, &mesh_normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * nbFacesPerMesh[indMeshFile][indMeshInFile] * sizeof(GLfloat),
			normalBuffer[indMeshInFile],
			GL_STATIC_DRAW);

		glGenBuffers(1, &(mesh_index_vbo[indMeshFile][indMeshInFile]));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMeshFile][indMeshInFile]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			3 * nbFacesPerMesh[indMeshFile][indMeshInFile] * sizeof(GLuint),
			indexBuffer[indMeshInFile],
			GL_STATIC_DRAW);

		//printf("indices mesh %d: ", indMeshFile);
		//for (int ind = 0; ind < 3 * nbFacesPerMesh[indMeshFile][indMeshInFile]; ind++) {
		//	printf("%d ", indexBuffer[indMeshInFile][ind]);
		//}
		//printf("\n");

		// VAO
		mesh_vao[indMeshFile][indMeshInFile] = 0;
		glGenVertexArrays(1, &(mesh_vao[indMeshFile][indMeshInFile]));
		glBindVertexArray(mesh_vao[indMeshFile][indMeshInFile]);

		glBindBuffer(GL_ARRAY_BUFFER, mesh_points_vbo);
		// vertex positions are at location 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		glBindBuffer(GL_ARRAY_BUFFER, mesh_texCoords_vbo);
		// texCoord positions are at location 1
		glEnableVertexAttribArray(1); // don't forget this!
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);
		// texCoord positions are at location 1
		glEnableVertexAttribArray(2); // don't forget this!
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indMeshFile][indMeshInFile]);

		// printf("Load Mesh %d vao ID %d vbo ID %d nbfaces %d\n" ,indMeshInFile , mesh_vao[indMeshInFile], mesh_index_vbo[indMeshInFile], nbFacesInEachMesh[indMeshInFile]);
		glBindVertexArray(0); // Disable our Vertex Buffer Object

		printOglError(23);
	}

	free(nbVerticesPerMesh);
	free(nbTexCoordsPerMesh);
	free(nbNormalsPerMesh);
}
