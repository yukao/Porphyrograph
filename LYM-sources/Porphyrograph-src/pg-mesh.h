/*! \file pg-mesh.h
 *  \brief the header files for classes ( \a pg_Window, \a pg_Environment ) 
 *  and structures (\a cursorShape, and \a WindowType)
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File pg-mesh.h
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

#ifndef PG_MESH_H
#define PG_MESH_H

// mesh data
extern GLfloat *vertexBuffer;
extern GLfloat *texCoordBuffer;
extern GLfloat *normalBuffer;
extern GLuint  *indexBuffer;

// shader variable pointers
extern GLint *uniform_mesh_model;
extern GLint *uniform_mesh_view;
extern GLint *uniform_mesh_proj;
extern GLint *uniform_mesh_light;

#ifndef TEMPETE
// mesh lighting
extern GLfloat mesh_light_x;
extern GLfloat mesh_light_y;
extern GLfloat mesh_light_z;
#endif

void copyMeshData(int indMeshInFile, GLfloat *vertexBufferIni, GLfloat *texCoordBufferIni, GLfloat *normalBufferIni,
	GLuint  *indexPointBufferIni, GLuint  *indexTexCoordBufferIni, GLuint  *indexNormalBufferIni,
	int nbFacesInThisMesh);
void count_faces_mesh_obj(FILE *file, int *meshNo,
	int **nbVerticesInEachMesh, int **nbTextCoordsInEachMesh, int **nbNormalsInEachMesh,
	int **nbFacesInEachMesh);
void parse_mesh_obj(FILE *file, int indMeshFile, int nbMeshes,
	int *nbVerticesInEachMesh, int *nbTextCoordsInEachMesh, int *nbNormalsInEachMesh,
	int *nbFacesInEachMesh);
void transferMeshDataToGPU(int indMeshFile, int indMeshInFile);
void load_mesh_obj(string obj_file_name, int indMeshFile);

#endif
