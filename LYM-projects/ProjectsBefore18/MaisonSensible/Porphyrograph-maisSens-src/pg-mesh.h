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

#define PG_NB_MAX_MESH 40

// geometry of mesh
extern string meshId[PG_NB_MAX_MESH];
extern int nbpointsMesh[PG_NB_MAX_MESH];
extern int nbtexCoordsMesh[PG_NB_MAX_MESH];
extern int NbFacesMesh[PG_NB_MAX_MESH]; // nb faces of mesh
extern int NbMesh;

extern GLfloat *pointBuffer[PG_NB_MAX_MESH];
extern GLfloat *texCoordBuffer[PG_NB_MAX_MESH];
extern GLuint  *indexBuffer[PG_NB_MAX_MESH];

// mesh vertex array object for rendering
extern unsigned int mesh_vao[PG_NB_MAX_MESH];
// mesh vertex indices for rendering
extern unsigned int mesh_index_vbo[PG_NB_MAX_MESH];

void load_mesh_obj( void );

#endif
