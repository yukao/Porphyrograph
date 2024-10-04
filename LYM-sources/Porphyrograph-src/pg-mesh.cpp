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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
// geometrical data of mesh

// mesh data
GLfloat *pg_mesh_vertexBuffer = NULL;
GLfloat *pg_mesh_texCoordBuffer = NULL;
GLfloat *pg_mesh_normalBuffer = NULL;
GLuint  *pg_mesh_indexBuffer = NULL;
GLint* pg_mesh_boneIndexBuffer = NULL;
GLfloat* pg_mesh_boneWeightBuffer = NULL;

// mesh lighting
GLfloat pg_mesh_light_x = 0.56f;
GLfloat pg_mesh_light_y = 0.35f;
GLfloat pg_mesh_light_z = 3.f;

int pg_chosen_mesh_LibraryPose1 = 0;
int pg_chosen_mesh_LibraryPose2 = 0;

// MESHES
vector<MeshData> pg_Meshes[PG_MAX_SCENARIOS];
vector<MeshAnimationData> pg_Mesh_Animations[PG_MAX_SCENARIOS];

glm::mat4 pg_MeshPoseModelMatrix;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// ARMATURE PARSING (BONES)
//////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshData::pg_drawOneMesh(int indMeshFile) {
	// visibility
	bool visible = false;
#if defined(pg_Project_Etoiles)
	visible = Etoiles_mesh_guided_by_strokes(indMeshFile);
#elif defined(var_Caverne_Mesh_Profusion)
	visible = (indMeshFile < 7 && (activeMeshes & (1 << indMeshFile))) || (pg_CaverneActveMesh
		&& (pg_CurrentClockTime - pg_CaverneMeshBirthTime > pg_CaverneMeshWakeupTime)
		&& (pg_CurrentClockTime < pg_CaverneMeshDeathTime));
#else
	visible = (activeMeshes & (1 << indMeshFile));
#endif

	// visible mesh
	if (visible) {
		// mesh animation
#if defined(pg_Project_Etoiles)
		Etoiles_ray_animation();
#elif defined(var_Caverne_Mesh_Profusion)
		Caverne_profusion_automatic_rotation(indMeshFile);
#endif

		// copies the bone animation matrices that are used by the vertex shader together with bone weights to animate the mesh
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_anime]) {
			if (pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones > PG_MAX_BONES) {
				sprintf(pg_errorStr, "Error: numer of bones is currently limited at %d! For higher values change constant PG_MAX_BONES in header file and in mesh vertex shader", PG_MAX_BONES); pg_ReportError(pg_errorStr); throw 745;
			}

			glm::f32* valMats = new glm::f32[16 * pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones];
			for (int indBone = 0; indBone < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones; indBone++) {
				// 16 = size of a 4x4 animation matrix attached to a bone
				memcpy(&valMats[16 * indBone],
					glm::value_ptr(pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones[indBone].pointAnimationMatrix),
					16 * sizeof(glm::f32));
			}
			glUniformMatrix4fv(uniform_Mesh_bones_matrices[pg_ind_scenario], PG_MAX_BONES, GL_FALSE, valMats);
			delete[] valMats;
		}

		// Model matrix 
		// transformed mesh according to scenario file
		// 1. a varying translation matrix
		// mesh translation animation
		pg_MeshPoseModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(pg_Mesh_Translation_X, pg_Mesh_Translation_Y, pg_Mesh_Translation_Z));
		// translation of the root bone (from posing)
		if (pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones > 0) {
			pg_MeshPoseModelMatrix = pg_MeshPoseModelMatrix *
				pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones[0].boneAnimationTranslationMatrix;
		}

		// 2. a varying rotation matrix 
		// mesh rotation animation
		glm::vec3 myRotationAxis(pg_Mesh_Rotation_X, pg_Mesh_Rotation_Y, pg_Mesh_Rotation_Z);
		pg_MeshPoseModelMatrix = glm::rotate(pg_MeshPoseModelMatrix, pg_Mesh_Rotation_angle, myRotationAxis);

		// 3. a varying scaling matrix 
#if defined(var_Caverne_Mesh_Profusion)
		if (indMeshFile < 7) {
			pg_MeshPoseModelMatrix = glm::scale(pg_MeshPoseModelMatrix, glm::vec3(pg_Mesh_Scale));
		}
		else {
			pg_MeshPoseModelMatrix = glm::scale(pg_MeshPoseModelMatrix, glm::vec3(pg_Mesh_Scale * min(2.0f, (pg_CurrentClockTime - pg_CaverneMeshBirthTime) / 20.f)));
		}
#else
		pg_MeshPoseModelMatrix = glm::scale(pg_MeshPoseModelMatrix, glm::vec3(pg_Mesh_Scale));
#endif

		// model matrix transfered to GPU (if it is not object by object made)
		glUniformMatrix4fv(uniform_Mesh_vp_model[pg_ind_scenario], 1, GL_FALSE,
			glm::value_ptr(pg_MeshPoseModelMatrix));

#if defined(var_MmeShanghai_brokenGlass)
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_MmeShanghai_brokenGlass]) {
			if (indMeshFile == 0 /* wall (broken glass on the right panel) */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_ind_scenario], 0, 0, 0, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_ind_scenario], 0, 0, 1.f, MS_BLACK);
			}
			else if (indMeshFile == 2 /* WHITE_RIGHT */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_ind_scenario], MS_WHITE_RIGHT_r, MS_WHITE_RIGHT_g, MS_WHITE_RIGHT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_ind_scenario], MS_WHITE_RIGHT_color_weight, MS_WHITE_RIGHT_master_weight, MS_WHITE_RIGHT_photo_weight, MS_BLACK);
			}
			else if (indMeshFile == 1 /* WHITE_LEFT */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_ind_scenario], MS_WHITE_LEFT_r, MS_WHITE_LEFT_g, MS_WHITE_LEFT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_ind_scenario], MS_WHITE_LEFT_color_weight, MS_WHITE_LEFT_master_weight, MS_WHITE_LEFT_photo_weight, MS_BLACK);
			}
			else if (indMeshFile == 3 /* BLACK */) {
				glUniform4f(uniform_Mesh_fs_4fv_color[pg_ind_scenario], MS_WHITE_LEFT_r, MS_WHITE_LEFT_g, MS_WHITE_LEFT_b, 1.0f);
				glUniform4f(uniform_Mesh_fs_4fv_color_master_photo_weight_bg[pg_ind_scenario], 0.f, 0.f, 0.f, MS_BLACK);
			}
		}
#endif

		for (int indObjectInMesh = 0; indObjectInMesh < pg_nbObjectsPerMeshFile; indObjectInMesh++) {
			bool visibleObject = true;
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_MmeShanghai_brokenGlass]) {
				if (indMeshFile == 0) {
					if (MmeShanghai_brokenGlass > 0 && MmeShanghai_brokenGlass <= pg_MmeShanghai_NbMeshSubParts) {
						visibleObject = pg_MmeShanghai_MeshSubParts[MmeShanghai_brokenGlass - 1][indObjectInMesh];
						// new visible object, defines the wake up time for falling
						if (visibleObject && pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] == 0.f
							// animation starts when all the meshes are displayed
							&& MmeShanghai_brokenGlass >= pg_MmeShanghai_NbMeshSubParts) {
							pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] = pg_CurrentClockTime + rand_0_1 * 10.0;
							pg_MmeShanghai_Object_Rotation_Ini_angle[indObjectInMesh] = float(rand_0_1 * M_PI * 0.004);
							pg_MmeShanghai_Object_Rotation_X[indObjectInMesh] = rand_0_1;
							pg_MmeShanghai_Object_Rotation_Y[indObjectInMesh] = rand_0_1;
							pg_MmeShanghai_Object_Rotation_Z[indObjectInMesh] = rand_0_1;
						}
					}
					else if (MmeShanghai_brokenGlass > pg_MmeShanghai_NbMeshSubParts) {
						visibleObject = pg_MmeShanghai_MeshSubParts[pg_MmeShanghai_NbMeshSubParts - 1][indObjectInMesh];
					}
					else {
						visibleObject = false;
					}
					if (visibleObject
						&& pg_MmeShanghaiMeshObjectWakeupTime[indObjectInMesh] < pg_CurrentClockTime
						// animation starts when all the meshes are displayed
						&& MmeShanghai_brokenGlass >= pg_MmeShanghai_NbMeshSubParts) {
						MmeShanghai_automatic_brokenGlass_animation(indMeshFile, indObjectInMesh);
						// Model matrix 
						// transformed mesh according to scenario file
						// 1. a varying translation matrix
						pg_MeshPoseModelMatrix = glm::translate(pg_MeshPoseModelMatrix,
							glm::vec3(pg_MmeShanghai_Object_Translation_X[indObjectInMesh],
								pg_MmeShanghai_Object_Translation_Y[indObjectInMesh],
								pg_MmeShanghai_Object_Translation_Z[indObjectInMesh]));
						// 2. a varying rotation matrix (with bakc and forth translation to barycenter so that the object rotates on itself
						pg_MeshPoseModelMatrix = glm::translate(pg_MeshPoseModelMatrix,
							glm::vec3(mesh_barycenter[3 * indObjectInMesh + 0],
								mesh_barycenter[3 * indObjectInMesh + 1],
								mesh_barycenter[3 * indObjectInMesh + 2]));
						glm::vec3 myObjectRotationAxis(pg_MmeShanghai_Object_Rotation_X[indObjectInMesh],
							pg_MmeShanghai_Object_Rotation_Y[indObjectInMesh],
							pg_MmeShanghai_Object_Rotation_Z[indObjectInMesh]);
						pg_MeshPoseModelMatrix = glm::rotate(pg_MeshPoseModelMatrix,
							pg_MmeShanghai_Object_Rotation_angle[indObjectInMesh],
							myObjectRotationAxis);
						pg_MeshPoseModelMatrix = glm::translate(pg_MeshPoseModelMatrix,
							glm::vec3(-mesh_barycenter[3 * indObjectInMesh + 0],
								-mesh_barycenter[3 * indObjectInMesh + 1],
								-mesh_barycenter[3 * indObjectInMesh + 2]));

						// model matrix transfered to GPU (if it is not object by object made)
						glUniformMatrix4fv(uniform_Mesh_vp_model[pg_ind_scenario], 1, GL_FALSE,
							glm::value_ptr(pg_MeshPoseModelMatrix));
					}
					else {
						glUniformMatrix4fv(uniform_Mesh_vp_model[pg_ind_scenario], 1, GL_FALSE,
							glm::value_ptr(glm::mat4(1.0f)));
					}
				}
			}
#endif
			if (visibleObject
				&& pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]) {
				//printf("mesh pass mesh %d obj %d\n", indMeshFile, indObjectInMesh);

				// binds VAO
				glBindVertexArray(mesh_vao[indObjectInMesh]);

				// activate shaders and sets uniform variable values    
				glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]);

				glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// texture unit location
				glUniform1i(uniform_Mesh_texture_fs_decal[pg_ind_scenario], 0);
				glActiveTexture(GL_TEXTURE0 + 0);

				// specific texture
				if (pg_Mesh_TextureRank != -1) {
					glBindTexture(GL_TEXTURE_RECTANGLE, Mesh_texture_rectangle);
				}
				// Augmented Reality: FBO capture of Master to be displayed on a mesh
				else if (augmentedReality) {
					glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);
				}
				else {
					glBindTexture(GL_TEXTURE_RECTANGLE, 0);
				}

#if defined(var_MmeShanghai_brokenGlass)
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_MmeShanghai_brokenGlass]) {
					// second texture is the BG rendering
					glUniform1i(uniform_Mesh_texture_fs_BG[pg_ind_scenario], 1);
					glActiveTexture(GL_TEXTURE0 + 1);
					glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);  // master output memory for mapping on mesh
				}
#endif
				//printf("Draw filled triangles\n");

				// standard filled mesh drawing
				// draw triangles from the currently bound VAO with current in-use shader
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indObjectInMesh]);

				// updates this variable according whether triangles or lines are shown
				glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_ind_scenario],
					isDisplayLookAt, -1, with_blue, (GLfloat)pg_CurrentSceneIndex);
				glUniform3f(uniform_Mesh_fs_3fv_light[pg_ind_scenario], pg_mesh_light_x, pg_mesh_light_y, pg_mesh_light_z);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glDrawElements(GL_TRIANGLES, pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
				pg_printOglError(27);

				// draws the polygon contours
				// updates this variable according whether triangles or lines are shown
				// optional additional drawing of the polygon contours for checking calibration in augmented reality
				if (with_mesh > 0
#if defined(var_MmeShanghai_brokenGlass)
					&& indMeshFile == 0
#endif
					) {
					// no z-Buffer to display the full mesh as if it was transparent
					glDisable(GL_DEPTH_TEST);
					glLineWidth(1);
					//printf("Draw triangle lines\n");
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
					// goes back into triangle filling mode
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					// no z-Buffer
					glEnable(GL_DEPTH_TEST);
				}
				// armature rendering
				if (pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones != NULL && with_bones > 0) {
					glDisable(GL_DEPTH_TEST);
					//printf("render armature mesh #%d\n", indMeshFile);
					// updates this variable according whether triangles or lines are shown
					glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_ind_scenario],
						isDisplayLookAt, 1, float(indMeshFile), (GLfloat)pg_CurrentSceneIndex);
					glUniform3f(uniform_Mesh_fs_3fv_light[pg_ind_scenario], 0, 0, 0);
					pg_render_bones(pg_MeshPoseModelMatrix, indMeshFile);
					glEnable(GL_DEPTH_TEST);
				}
				//printf("Display mesh VP1 %d/%d size (nb faces) %d\n\n", indMeshFile + 1, pg_Meshes->pg_nbObjectsPerMeshFile,
				//	pg_nbFacesPerMeshFile[indObjectInMesh]);
			} // visible submesh object
		} // submeshes


	} // visible mesh
}

void MeshData::pg_drawOneMesh2(int indMeshFile) {
	// visibility
	bool visible = false;
	visible = (activeMeshes & (1 << indMeshFile));

	// visible mesh
	if (visible
		&& pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]) {

		// activate shaders and sets uniform variable values    
		glUseProgram(pg_shader_programme[pg_ind_scenario][pg_enum_shader_Mesh]);

		// Model matrix 
		// transformed mesh according to scenario file
		// 1. a varying translation matrix
		pg_MeshPoseModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(pg_Mesh_Translation_X, pg_Mesh_Translation_Y, pg_Mesh_Translation_Z));
		// 2. a varying rotation matrix 
		glm::vec3 myRotationAxis(pg_Mesh_Rotation_X,
			pg_Mesh_Rotation_Y, pg_Mesh_Rotation_Z);
		pg_MeshPoseModelMatrix = glm::rotate(pg_MeshPoseModelMatrix, pg_Mesh_Rotation_angle, myRotationAxis);
		// 3. a varying scaling matrix 
		pg_MeshPoseModelMatrix = glm::scale(pg_MeshPoseModelMatrix, glm::vec3(pg_Mesh_Scale));

		// model matrix transfered to GPU
		glUniformMatrix4fv(uniform_Mesh_vp_model[pg_ind_scenario], 1, GL_FALSE,
			glm::value_ptr(pg_MeshPoseModelMatrix));

		for (int indObjectInMesh = 0; indObjectInMesh < pg_nbObjectsPerMeshFile; indObjectInMesh++) {
			// binds VAO
			glBindVertexArray(mesh_vao[indObjectInMesh]);

			glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// texture unit location
			glUniform1i(uniform_Mesh_texture_fs_decal[pg_ind_scenario], 0);
			glActiveTexture(GL_TEXTURE0 + 0);
			// specific mesh texture
			if (pg_Mesh_TextureRank != -1) {
				glBindTexture(GL_TEXTURE_RECTANGLE, Mesh_texture_rectangle);
			}
			// Augmented Reality: FBO capture of Master to be displayed on a mesh
			else if (augmentedReality) {
				glBindTexture(GL_TEXTURE_RECTANGLE, pg_FBO_Master_capturedFB_prec_texID);
			}
			else {
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);  // no texture
			}

			// standard filled mesh drawing
			// draw triangles from the currently bound VAO with current in-use shader
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indObjectInMesh]);

			// updates this variable according whether triangles or lines are shown
			glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_ind_scenario], isDisplayLookAt, 0, with_blue, (GLfloat)pg_CurrentSceneIndex);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);

			// Augmented Reality: FBO capture of Master to be displayed on a mesh
					// optional additional drawing of the polygon contours for checking calibration in augmented reality
			if (with_mesh) {
				// no z-Buffer
				glDisable(GL_DEPTH_TEST);
				glLineWidth(3);
				glUniform4f(uniform_Mesh_fs_4fv_isDisplayLookAt_with_mesh_with_blue_currentScene[pg_ind_scenario], isDisplayLookAt, 1, with_blue, (GLfloat)pg_CurrentSceneIndex);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, pg_nbFacesPerMeshFile[indObjectInMesh] * 3, GL_UNSIGNED_INT, (GLvoid*)0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				// no z-Buffer
				glEnable(GL_DEPTH_TEST);
			}
			//printf("Display mesh VP2 %d/%d size (nb faces) %d\n", indMeshFile, indObjectInMesh,
			//	pg_nbFacesPerMeshFile[indObjectInMesh]);


		} // submeshes

	} // visible mesh
}

void MeshAnimationData::pg_parseOneBoneObj(FILE* file, int level, char* line, char* tag, char* id, int* nbBonesLoc) {
	//printf("Bone object level %d tag %s nb bones %d line %s\n", level, tag, *nbBonesLoc, line);
	//       transl Femur_L_transl
	while (strcmp(tag, "transl") == 0) {
		float w, x, y, z;

		// Scan for Bones in this mesh
		if ((*nbBonesLoc) >= pg_nb_bones) {
			printf("Error: Excessive number of Bones\n");
			throw 0;
		}

		// has read the transl/ID line

		// stores the translation values
		// 0 0.7475 0
		fgets(line, 512, file);
		sscanf(line, "%f %f %f", &x, &y, &z);
		pg_tabBones[(*nbBonesLoc)].boneInitialTranslationMatrix
			= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		pg_tabBones[(*nbBonesLoc)].boneAnimationTranslationMatrix
			= glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

		// initialRotation tag
		// rot_quat Tibia_L_rot
		fgets(line, 512, file);

		// stores the initialRotation values
		// 0.9997 - 0.0111 - 0.0047 - 0.0221
		fgets(line, 512, file);
		sscanf(line, "%f %f %f %f", &w, &x, &y, &z);
		glm::quat initialRotation
			= glm::quat(w, x, y, z);
		pg_tabBones[(*nbBonesLoc)].boneInitialRotationMatrix
			= glm::mat4_cast(initialRotation);
		pg_tabBones[(*nbBonesLoc)].boneAnimationRotationMatrix
			= glm::mat4_cast(initialRotation);

		// bone
		// bone Tibia_L
		fgets(line, 512, file);
		char boneID[256];
		sscanf(line, "%s %256s", tag, boneID);
		if (pg_tabBones[(*nbBonesLoc)].id != string(boneID)) {
			printf("Error: Incorrect Bone ID\n");
			throw 0;
		}

		// length
		// 0.9169
		fgets(line, 512, file);
		sscanf(line, "%f", &(pg_tabBones[(*nbBonesLoc)].length));
		pg_tabBones[(*nbBonesLoc)].points[2 * 3 + 1] = pg_tabBones[(*nbBonesLoc)].length;
		pg_tabBones[(*nbBonesLoc)].points[5 * 3 + 1] = pg_tabBones[(*nbBonesLoc)].length;

		///////////////////////////////////////////////////////////
		// vertex buffer objects and vertex array for the bones
		pg_tabBones[(*nbBonesLoc)].vbo = 0;
		glGenBuffers(1, &(pg_tabBones[(*nbBonesLoc)].vbo));

		pg_tabBones[(*nbBonesLoc)].vao = 0;
		glGenVertexArrays(1, &(pg_tabBones[(*nbBonesLoc)].vao));

		// vertex buffer objects and vertex array
		glBindBuffer(GL_ARRAY_BUFFER, pg_tabBones[(*nbBonesLoc)].vbo);
		glBufferData(GL_ARRAY_BUFFER,
			2 * 3 * 3 * sizeof(float),
			pg_tabBones[(*nbBonesLoc)].points,
			GL_STATIC_DRAW);

		glBindVertexArray(pg_tabBones[(*nbBonesLoc)].vao);
		glBindBuffer(GL_ARRAY_BUFFER, pg_tabBones[(*nbBonesLoc)].vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(0);

		// parent
		// parent_bone Femur_L
		fgets(line, 512, file);
		sscanf(line, "%s %256s", tag, boneID);
		//printf("Bone %s (parent: %s) (prof: %d)\n",
		//	pg_tabBones[(*nbBonesLoc)].id.c_str(), boneID, level);

		// associates the parent bone with the current bone
		// if it is not a root bone
		if (strcmp(boneID, "NULL") != 0) {
			bool parentfound = false;
			for (int ind = 0; ind < (*nbBonesLoc); ind++) {
				if (pg_tabBones[ind].id == string(boneID)) {
					pg_tabBones[(*nbBonesLoc)].parentBone = pg_tabBones + ind;
					if (!pg_tabBones[ind].daughterBone) {
						pg_tabBones[ind].daughterBone = pg_tabBones + (*nbBonesLoc);
					}
					else {
						Bone* currentBone = pg_tabBones[ind].daughterBone;
						while (currentBone->sisterBone) {
							currentBone = currentBone->sisterBone;
						}
						currentBone->sisterBone = pg_tabBones + (*nbBonesLoc);
					}
					parentfound = true;
					break;
				}
			}
			if (!parentfound) {
				printf("Parent of bone %s (%s) not found!\n",
					pg_tabBones[(*nbBonesLoc)].id.c_str(), id);
			}
		}
		// no parent chains with the root node if it is not the first node wo parents
		else {
			// it is not the root node
			if ((*nbBonesLoc) > 0) {
				Bone* currentBone = pg_tabBones;
				while (currentBone->sisterBone) {
					currentBone = currentBone->sisterBone;
				}
				currentBone->sisterBone = pg_tabBones + (*nbBonesLoc);
			}
		}

		// next tag
		fgets(line, 512, file);
		sscanf(line, "%s %s", tag, id);

		(*nbBonesLoc)++;

		// daughter bone
		// transl Femur_L_transl
		if (strcmp(tag, "transl") == 0) {
			pg_parseOneBoneObj(file, level + 1, line, tag, id, nbBonesLoc);
			if (strcmp(tag, "end") == 0) { strcpy(tag, "end"); return; }

			// if empty line: end of file
			if (!fgets(line, 512, file)) { strcpy(tag, "end"); return; }
			// non empty line: reads further (possible sister node)
			sscanf(line, "%s %s", tag, id);
		}

		// end_bone tag
		// bone_end Tibia_L
		else if (strcmp(tag, "bone_end") == 0) {
			// if empty line: end of file
			if (!fgets(line, 512, file)) { strcpy(tag, "end"); return; }
			// non empty line: reads further (possible sister node)
			sscanf(line, "%s %s", tag, id);
		}
	}
}

void MeshAnimationData::pg_parseArmatureObj(FILE* file, char* line, char* tag, char* id) {
	int nbBonesLoc = 0;

	// next tag
	sscanf(line, "%s %s", tag, id);

	//printf("Skeleton Armature level %d tag %s nb bones %d line %s\n", 1, tag, nbBonesLoc, line);
	while (strcmp(tag, "transl") == 0) {
		pg_parseOneBoneObj(file, 1, line, tag, id, &nbBonesLoc);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH PARSING
//////////////////////////////////////////////////////////////////////////////////////////////////////


// the linearization for OpenGL replaces indices associated with each face in obj format
// by coordinates copied from these temporary buffers (through pg_copyMeshData)
void pg_copyMeshData(  int indObjectInMesh , GLfloat *vertexBufferIni , GLfloat *texCoordBufferIni , GLfloat *normalBufferIni ,
					GLint* boneIndexBufferIni, GLfloat* boneWeightBufferIni,
					GLuint  *indexPointBufferIni, GLuint  *indexTexCoordBufferIni, GLuint  *indexNormalBufferIni,
					int nbFacesInThisMesh) {
  	pg_mesh_vertexBuffer = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 3 * sizeof(GLfloat));
	pg_mesh_texCoordBuffer = (GLfloat *)malloc( nbFacesInThisMesh * 3 * 2 * sizeof(GLfloat));
	pg_mesh_normalBuffer = (GLfloat*)malloc(nbFacesInThisMesh * 3 * 3 * sizeof(GLfloat));
	pg_mesh_boneIndexBuffer = (GLint*)malloc(nbFacesInThisMesh * 3 * 4 * sizeof(GLint));
	pg_mesh_boneWeightBuffer = (GLfloat*)malloc(nbFacesInThisMesh * 3 * 4 * sizeof(GLfloat));
	pg_mesh_indexBuffer = (GLuint *)malloc( nbFacesInThisMesh * 3 * sizeof(GLuint));
	for( int indFace = 0 ; indFace < nbFacesInThisMesh ; indFace++ ) {
	  // copies the indices in the index table
	  pg_mesh_indexBuffer[ indFace * 3 ] = indFace * 3;
	  pg_mesh_indexBuffer[ indFace * 3 + 1 ] = indFace * 3 + 1;
	  pg_mesh_indexBuffer[ indFace * 3 + 2 ] = indFace * 3 + 2;

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indVertex1 = indexPointBufferIni[ indFace * 3 ];
	  int indVertex2 = indexPointBufferIni[ indFace * 3 + 1 ];
	  int indVertex3 = indexPointBufferIni[ indFace * 3 + 2 ];
	  //if (indFace == 10) {
		  //printf("Face ind vertices %d %d %d %d\n", indFace, indVertex1, indVertex2, indVertex3);
	  //}
	  pg_mesh_vertexBuffer[ indFace * 3 * 3 ]     = vertexBufferIni[ indVertex1 * 3 ];
	  pg_mesh_vertexBuffer[ indFace * 3 * 3 + 1 ] = vertexBufferIni[ indVertex1 * 3 + 1 ];
	  pg_mesh_vertexBuffer[ indFace * 3 * 3 + 2 ] = vertexBufferIni[ indVertex1 * 3 + 2 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 ] = vertexBufferIni[ indVertex2 * 3 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 + 1 ] = vertexBufferIni[ indVertex2 * 3 + 1 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 + 2 ] = vertexBufferIni[ indVertex2 * 3 + 2 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 ] = vertexBufferIni[ indVertex3 * 3 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 + 1 ] = vertexBufferIni[ indVertex3 * 3 + 1 ];
	  pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 + 2 ] = vertexBufferIni[ indVertex3 * 3 + 2 ];
	 // printf("Face point %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		//pg_mesh_vertexBuffer[ indFace * 3 * 3 ] , pg_mesh_vertexBuffer[ indFace * 3 * 3 + 1 ] , pg_mesh_vertexBuffer[ indFace * 3 * 3 + 2 ] ,
		//pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 ] , pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 + 1 ] , pg_mesh_vertexBuffer[ (indFace * 3 + 1) * 3 + 2 ] ,
		//pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 ] , pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 + 1 ] , pg_mesh_vertexBuffer[ (indFace * 3 + 2) * 3 + 2 ] );

	  // copies the texture coordinates from the initial buffer to the final one
	  int indexTexCoord1 = indexTexCoordBufferIni[ indFace * 3 ];
	  int indexTexCoord2 = indexTexCoordBufferIni[ indFace * 3 + 1 ];
	  int indexTexCoord3 = indexTexCoordBufferIni[ indFace * 3 + 2 ];
	  // printf("Face indTexC %d %d %d %d\n" , indFace ,indexTexCoord1,indexTexCoord2,indexTexCoord3);
	  pg_mesh_texCoordBuffer[ indFace * 3 * 2 ]     = texCoordBufferIni[ indexTexCoord1 * 2 ];
	  pg_mesh_texCoordBuffer[ indFace * 3 * 2 + 1 ] = texCoordBufferIni[ indexTexCoord1 * 2 + 1 ];
	  pg_mesh_texCoordBuffer[ (indFace * 3 + 1) * 2 ] = texCoordBufferIni[ indexTexCoord2 * 2 ];
	  pg_mesh_texCoordBuffer[ (indFace * 3 + 1) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord2 * 2 + 1 ];
	  pg_mesh_texCoordBuffer[ (indFace * 3 + 2) * 2 ] = texCoordBufferIni[ indexTexCoord3 * 2 ];
	  pg_mesh_texCoordBuffer[ (indFace * 3 + 2) * 2 + 1 ] = texCoordBufferIni[ indexTexCoord3 * 2 + 1 ];

	  // copies the vertices coordinates from the initial buffer to the final one
	  int indNormal1 = indexNormalBufferIni[indFace * 3];
	  int indNormal2 = indexNormalBufferIni[indFace * 3 + 1];
	  int indNormal3 = indexNormalBufferIni[indFace * 3 + 2];
	  // printf("Face indNormal %d %d %d %d\n" , indFace ,indNormal1,indNormal2,indNormal3);
	  pg_mesh_normalBuffer[indFace * 3 * 3] = normalBufferIni[indNormal1 * 3];
	  pg_mesh_normalBuffer[indFace * 3 * 3 + 1] = normalBufferIni[indNormal1 * 3 + 1];
	  pg_mesh_normalBuffer[indFace * 3 * 3 + 2] = normalBufferIni[indNormal1 * 3 + 2];
	  pg_mesh_normalBuffer[(indFace * 3 + 1) * 3] = normalBufferIni[indNormal2 * 3];
	  pg_mesh_normalBuffer[(indFace * 3 + 1) * 3 + 1] = normalBufferIni[indNormal2 * 3 + 1];
	  pg_mesh_normalBuffer[(indFace * 3 + 1) * 3 + 2] = normalBufferIni[indNormal2 * 3 + 2];
	  pg_mesh_normalBuffer[(indFace * 3 + 2) * 3] = normalBufferIni[indNormal3 * 3];
	  pg_mesh_normalBuffer[(indFace * 3 + 2) * 3 + 1] = normalBufferIni[indNormal3 * 3 + 1];
	  pg_mesh_normalBuffer[(indFace * 3 + 2) * 3 + 2] = normalBufferIni[indNormal3 * 3 + 2];
	 //  printf("Face normal %d %.2f/%.2f/%.2f %.2f/%.2f/%.2f %.2f/%.2f/%.2f \n" , indFace ,
		// pg_mesh_normalBuffer[ indFace * 3 * 3 ] , pg_mesh_normalBuffer[ indFace * 3 * 3 + 1 ] , pg_mesh_normalBuffer[ indFace * 3 * 3 + 2 ] ,
		// pg_mesh_normalBuffer[ (indFace * 3 + 1) * 3 ] , pg_mesh_normalBuffer[ (indFace * 3 + 1) * 3 + 1 ] , pg_mesh_normalBuffer[ (indFace * 3 + 1) * 3 + 2 ] ,
		// pg_mesh_normalBuffer[ (indFace * 3 + 2) * 3 ] , pg_mesh_normalBuffer[ (indFace * 3 + 2) * 3 + 1 ] , pg_mesh_normalBuffer[ (indFace * 3 + 2) * 3 + 2 ] );
	 //  printf("Face texC %d %.2f/%.2f %.2f/%.2f %.2f/%.2f  \n" , indFace ,
		//pg_mesh_texCoordBuffer[ indFace * 3 * 2  ] , pg_mesh_texCoordBuffer[ indFace * 3 * 2 + 1 ] ,
		//pg_mesh_texCoordBuffer[ (indFace * 3 + 1) * 2 ] , pg_mesh_texCoordBuffer[ (indFace * 3 + 1) * 2 + 1 ] ,
		//pg_mesh_texCoordBuffer[ (indFace * 3 + 2) * 2 ] , pg_mesh_texCoordBuffer[ (indFace * 3 + 2) * 2 + 1 ]  );

	  // copies the bone indices and weights from the initial buffer to the final one
	  //if (indFace == 10) {
		 // printf("Face %d vertex %d ind object %d faces %d bone index & weights second vertex %d %d %d %d %f %f %f %f\n\n", indFace, indVertex2, indObjectInMesh, nbFacesInThisMesh,
			//  boneIndexBufferIni[indVertex2 * 4], boneIndexBufferIni[indVertex2 * 4 + 1], boneIndexBufferIni[indVertex2 * 4 + 2], boneIndexBufferIni[indVertex2 * 4 + 3],
			//  boneWeightBufferIni[indVertex2 * 4], boneWeightBufferIni[indVertex2 * 4 + 1], boneWeightBufferIni[indVertex2 * 4 + 2], boneWeightBufferIni[indVertex2 * 4 + 3]);
	  //}
	  pg_mesh_boneIndexBuffer[indFace * 3 * 4] = boneIndexBufferIni[indVertex1 * 4];
	  pg_mesh_boneIndexBuffer[indFace * 3 * 4 + 1] = boneIndexBufferIni[indVertex1 * 4 + 1];
	  pg_mesh_boneIndexBuffer[indFace * 3 * 4 + 2] = boneIndexBufferIni[indVertex1 * 4 + 2];
	  pg_mesh_boneIndexBuffer[indFace * 3 * 4 + 3] = boneIndexBufferIni[indVertex1 * 4 + 3];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 1) * 4] = boneIndexBufferIni[indVertex2 * 4];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 1) * 4 + 1] = boneIndexBufferIni[indVertex2 * 4 + 1];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 1) * 4 + 2] = boneIndexBufferIni[indVertex2 * 4 + 2];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 1) * 4 + 3] = boneIndexBufferIni[indVertex2 * 4 + 3];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 2) * 4] = boneIndexBufferIni[indVertex3 * 4];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 2) * 4 + 1] = boneIndexBufferIni[indVertex3 * 4 + 1];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 2) * 4 + 2] = boneIndexBufferIni[indVertex3 * 4 + 2];
	  pg_mesh_boneIndexBuffer[(indFace * 3 + 2) * 4 + 3] = boneIndexBufferIni[indVertex3 * 4 + 3];

	  pg_mesh_boneWeightBuffer[indFace * 3 * 4] = boneWeightBufferIni[indVertex1 * 4];
	  pg_mesh_boneWeightBuffer[indFace * 3 * 4 + 1] = boneWeightBufferIni[indVertex1 * 4 + 1];
	  pg_mesh_boneWeightBuffer[indFace * 3 * 4 + 2] = boneWeightBufferIni[indVertex1 * 4 + 2];
	  pg_mesh_boneWeightBuffer[indFace * 3 * 4 + 3] = boneWeightBufferIni[indVertex1 * 4 + 3];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 1) * 4] = boneWeightBufferIni[indVertex2 * 4];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 1) * 4 + 1] = boneWeightBufferIni[indVertex2 * 4 + 1];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 1) * 4 + 2] = boneWeightBufferIni[indVertex2 * 4 + 2];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 1) * 4 + 3] = boneWeightBufferIni[indVertex2 * 4 + 3];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 2) * 4] = boneWeightBufferIni[indVertex3 * 4];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 2) * 4 + 1] = boneWeightBufferIni[indVertex3 * 4 + 1];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 2) * 4 + 2] = boneWeightBufferIni[indVertex3 * 4 + 2];
	  pg_mesh_boneWeightBuffer[(indFace * 3 + 2) * 4 + 3] = boneWeightBufferIni[indVertex3 * 4 + 3];
	}
	//printf("\n");
}

// OBJ file parsing (Alias Wavefront ASCII format)
#define MAX_MESHES 1200
void pg_count_faces_mesh_obj(FILE *file, int *meshNo,
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

void MeshData::pg_transferMeshDataToGPU(int indObjectInMesh) {
	///////////////////////////////////////////////////////////
	// vertex buffer objects and vertex array for the mesh
	unsigned int mesh_points_vbo;
	unsigned int mesh_texCoords_vbo;
	unsigned int mesh_normals_vbo;
	unsigned int mesh_boneIndex_vbo;
	unsigned int mesh_boneWeight_vbo;

	// 3 VBOs
	//printf("GPU transfer mesh %d faces %d\n", indObjectInMesh, pg_nbFacesPerMeshFile[indObjectInMesh]);
	glGenBuffers(1, &mesh_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		pg_mesh_vertexBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_texCoords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_texCoords_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		pg_mesh_texCoordBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_normals_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_normals_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		pg_mesh_normalBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_boneIndex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneIndex_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLint),
		pg_mesh_boneIndexBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &mesh_boneWeight_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh_boneWeight_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLfloat),
		pg_mesh_boneWeightBuffer,
		GL_STATIC_DRAW);

	glGenBuffers(1, &(mesh_index_vbo[indObjectInMesh]));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh_index_vbo[indObjectInMesh]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * pg_nbFacesPerMeshFile[indObjectInMesh] * sizeof(GLuint),
		pg_mesh_indexBuffer,
		GL_STATIC_DRAW);

	//printf("Index Buffer mesh");
	//for (int ind = 0; ind < 3 *pg_nbFacesPerMeshFile[indObjectInMesh]; ind++) {
	//	printf("%d ", pg_mesh_indexBuffer[ind]);
	//}
	//printf("\n");

	// VAO
	mesh_vao[indObjectInMesh] = 0;
	glGenVertexArrays(1, &(mesh_vao[indObjectInMesh]));
	glBindVertexArray(mesh_vao[indObjectInMesh]);

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
	// bone weights are at location 4
	glEnableVertexAttribArray(4); 
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_index_vbo[indObjectInMesh]);

	//printf("Transferred Mesh %d vao ID %d vbo ID %d nbfaces %d\n" , indObjectInMesh ,mesh_vao[indObjectInMesh],
	//mesh_index_vbo[indObjectInMesh],pg_nbFacesPerMeshFile);
	glBindVertexArray(0); // Disable our Vertex Buffer Object

	pg_printOglError(26);

	free(pg_mesh_vertexBuffer);
	free(pg_mesh_texCoordBuffer);
	free(pg_mesh_normalBuffer);
	free(pg_mesh_boneIndexBuffer);
	free(pg_mesh_boneWeightBuffer);
	free(pg_mesh_indexBuffer);

	pg_mesh_vertexBuffer = NULL;
	pg_mesh_texCoordBuffer = NULL;
	pg_mesh_normalBuffer = NULL;
	pg_mesh_boneIndexBuffer = NULL;
	pg_mesh_boneWeightBuffer = NULL;
	pg_mesh_indexBuffer = NULL;
}



// MESH AND ARMATURE SENT TO GPU

void MeshData::pg_copy_mesh_data_and_ship_to_GPU(int indObjectInMesh, GLfloat* vertexBufferIni, 
	GLfloat* texCoordBufferIni, GLfloat* normalBufferIni,
	GLint* boneIndexBufferIni, GLfloat* boneWeightBufferIni,
	GLuint* indexPointBufferIni, GLuint* indexTexCoordBufferIni, GLuint* indexNormalBufferIni, int nbFacesInThisMesh, 
	int * nbVertexTot, int * nbCoordTexTot, int * nbNormalTot, int nbVertices, int nbTexCoords, int nbNormals, vector <string> mesh_IDs_current_mesh) {
	pg_copyMeshData(indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
		boneIndexBufferIni, boneWeightBufferIni,
		indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
		nbFacesInThisMesh);

	//printf("Parsed mesh %d %s Vertices %d texCoords %d normals %d faces %d\n",
	//	indObjectInMesh, mesh_IDs_current_mesh[indObjectInMesh].c_str(), nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh], nbFacesInEachMesh[indObjectInMesh]);
	pg_transferMeshDataToGPU(indObjectInMesh);

	(*nbVertexTot) += nbVertices;
	(*nbCoordTexTot) += nbTexCoords;
	(*nbNormalTot) += nbNormals;

	(indObjectInMesh)++;
}

// ARMATURE AND POSES
bool MeshAnimationData::pg_ParseMeshAnimation(FILE* file, int indMeshFile, char *line, char *tag) {
	char    id[256];
	char    tag2[256];

	// line with bone number
	if (!fgets(line, 256, file)) { return false; }
	sscanf(line, "%s %d", tag, &(pg_nb_bones));
	pg_tabBones = new Bone[pg_nb_bones];
	//printf("Skeleton %d bones to parse\n", pg_nb_bones);

	if (pg_nb_bones > 0) {
		// bone ID line
		if (!fgets(line, 256, file)) { return false; }
		sscanf(line, "%s", tag);
		int nbBonesLoc = 0;
		while (strcmp(tag, "bone") == 0) {
			if (nbBonesLoc >= pg_nb_bones) {
				printf("Error: Excessive number of bones in object\n");
				throw 0;
			}
			char boneID[256];
			sscanf(line, "%s %256s", tag, boneID);
			pg_tabBones[nbBonesLoc].id = string(boneID);
			nbBonesLoc++;

			if (!fgets(line, 512, file)) { return false; }
			sscanf(line, "%s", tag);
		}
	}

	pg_parseArmatureObj(file, line, tag, id);
	std::cout << " (" << pg_nb_bones << " bones),";

	pg_nb_LibraryPoses = 0;
	// possible list of poses
	if (strcmp(tag, "POSES") == 0) {
		sscanf(line, "%s %d", tag, &(pg_nb_LibraryPoses));
		std::cout << " (" << pg_nb_LibraryPoses << " poses),";
		// currently the maximum size of a bone-based animation is the same size as the initial library of poses found in the obj file
		pg_nb_MaxAnimationPoses = pg_nb_LibraryPoses;
		// currently the maximum size of a mesh motion is the same size as the initial library of poses found in the obj file
		pg_nb_MaxMotionPoses = pg_nb_LibraryPoses;

		// allocates the pose keyframe transformations (quaternion for rotation, and 3D vector for translation)
		for (int indBone = 0; indBone < pg_nb_bones; indBone++) {
			// static poses from the library inside the obj file
			pg_tabBones[indBone].boneLibraryPoseTranslationVector = new glm::vec3[pg_nb_LibraryPoses];
			pg_tabBones[indBone].boneLibraryPoseRotationQuat = new glm::quat[pg_nb_LibraryPoses];
			// dynamic animation poses copied from the library or generated automatically
			pg_tabBones[indBone].boneAnimationPoseTranslationVector = new glm::vec3[pg_nb_MaxAnimationPoses];
			pg_tabBones[indBone].boneAnimationPoseRotationQuat = new glm::quat[pg_nb_MaxAnimationPoses];
		}
		// weight for the skeleton animation between bone poses
		pg_interpolation_weight_AnimationPose = new float[pg_nb_MaxAnimationPoses];
		// weight for the mesh motion
		pg_interpolation_weight_MotionPose = new float[pg_nb_MaxMotionPoses];
		pg_motionPoses = new MotionPose[pg_nb_MaxMotionPoses];


		// reads the keyframes for the bones, 
		// no assumption about the order in which the transformations are given
		if (!fgets(line, 512, file)) { return false; }
		sscanf(line, "%s", tag);
		int nbPoses = 0;
		while (strcmp(tag, "POSE") == 0) {
			int indPose = -1;
			int indFrame = -1;
			sscanf(line, "%s %d", tag, &indPose);
			if (indPose < pg_nb_LibraryPoses && indPose == nbPoses) {
				//printf("scanning pose #%d\n", indPose);
				// first transformation (or next pose or next object if no transformation given)
				if (!fgets(line, 256, file)) { return false; }
				sscanf(line, "%s", tag);
				// reads each pose
				while (strcmp(tag, "POSE") != 0 && strcmp(tag, "o") != 0) {
					float val1 = 0.f, val2 = 0.f, val3 = 0.f, val4 = 1.f;
					sscanf(line, "%s %s %f %f %f %f", tag, tag2, &val1, &val2, &val3, &val4);
					bool found = false;
					for (int indBone = 0; indBone < pg_nb_bones; indBone++) {
						if (pg_tabBones[indBone].id == string(tag)) {
							if (strcmp(tag2, "transl") == 0) {
								pg_tabBones[indBone].boneLibraryPoseTranslationVector[indPose]
									= glm::vec3(val1, val2, val3);
								//printf("transl mesh #%d, bone #%d,%s %.4f %.4f %.4f\n", indMeshFile, indBone, pg_tabBones[indMeshFile][indBone].id.c_str(), val1, val2, val3);
							}
							else if (strcmp(tag2, "rot") == 0) {
								// CAUTION: W is the first coordinate in the quat constructor
								pg_tabBones[indBone].boneLibraryPoseRotationQuat[indPose]
									= glm::quat(val4, val1, val2, val3);
								//printf("rot mesh #%d, bone #%d,%s %.4f %.4f %.4f %.4f\n", indMeshFile, indBone, pg_tabBones[indMeshFile][indBone].id.c_str(), val1, val2, val3, val4);
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
					if (!fgets(line, 512, file)) { return false; }
					sscanf(line, "%s", tag);
				}
			}
			else {
				printf("Error: incorrect pose number %d maximum for this mesh %d expected rank %d\n", indPose, pg_nb_LibraryPoses, nbPoses - 1);
				throw 0;
			}
			nbPoses++;
		}
		// next pose
	}
	return true;
}


// OBJ file parsing (Alias Wavefront ASCII format)
void pg_parseMeshObj(FILE *file, int indMeshFile, int nbMeshObjects,
	int *nbVerticesInEachMesh, int *nbTextCoordsInEachMesh, int *nbNormalsInEachMesh,
	int *nbFacesInEachMesh, int indScenario) {
	char    tag[256];
	char    meshString[1024];
	char    line[256];

	// temporary storage of values of vertex positions, texCoords, and normals, and indices of faces
	// the buffers of vertices, tex coords, and normals, and the associated counts are temporary
	// they are then used for linearization for OpenGL
	// the linearization for OpenGL replaces indices associated with each face in obj format
	// by coordinates copied from these temporary buffers (through pg_copyMeshData)
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
	// initial count of mesh elements, before linearization by pg_copyMeshData for OpenGL
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
		if (!pg_Mesh_Animations[indScenario][indMeshFile].pg_ParseMeshAnimation(file, indMeshFile, line, tag)) {
			return;
		}
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

		// initial count of mesh elements, before linearization by pg_copyMeshData for OpenGL, local value
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
		pg_Meshes[indScenario][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 0] = barycenter[0];
		pg_Meshes[indScenario][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 1] = barycenter[1];
		pg_Meshes[indScenario][indMeshFile].mesh_barycenter[3 * indObjectInMesh + 2] = barycenter[2];
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
			if (indBone1 < -1 || indBone1 >= pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones|| indBone2 < -1 || indBone2 >= pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones
				|| indBone3 < -1 || indBone3 >= pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones || indBone4 < -1 || indBone4 >= pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones) {
				printf("Error: Incorrect vertex index in bone weights %d/%d/%d/%d (not int %d-%d)\n", indBone1, indBone2, indBone3, indBone4, -1, pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones - 1);
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

				pg_Meshes[indScenario][indMeshFile].pg_copy_mesh_data_and_ship_to_GPU(indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
					boneIndexBufferIni, boneWeightBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
					nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
					mesh_IDs_current_mesh);

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

				// initial count of mesh elements, before linearization by pg_copyMeshData for OpenGL
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

				pg_Meshes[indScenario][indMeshFile].pg_copy_mesh_data_and_ship_to_GPU(indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
					boneIndexBufferIni, boneWeightBufferIni,
					indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
					nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
					nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
					mesh_IDs_current_mesh);

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

				// initial count of mesh elements, before linearization by pg_copyMeshData for OpenGL
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
		pg_Meshes[indScenario][indMeshFile].pg_copy_mesh_data_and_ship_to_GPU(indObjectInMesh, vertexBufferIni, texCoordBufferIni, normalBufferIni,
			boneIndexBufferIni, boneWeightBufferIni,
			indexPointBufferIni, indexTexCoordBufferIni, indexNormalBufferIni,
			nbFacesMeshIni[indObjectInMesh], &nbVertexTot, &nbCoordTexTot, &nbNormalTot,
			nbVerticesMeshIni[indObjectInMesh], nbTexCoordsMeshIni[indObjectInMesh], nbNormalsMeshIni[indObjectInMesh],
			mesh_IDs_current_mesh);
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH LOADING FROM OBJ WAVEFRONT FORMAT
//////////////////////////////////////////////////////////////////////////////////////////////////////

// OBJ file parsing (Alias Wavefront ASCII format)
// nbMeshObjectsInFile: number of mesh objects inside the obj file
void MeshData::pg_load_mesh_objects(string mesh_file_name, int indMeshFile, int indScenario) {
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
	pg_count_faces_mesh_obj(fileMesh, &nbMeshObjectsInFile, &nbVerticesPerMesh,
		&nbTexCoordsPerMesh, &nbNormalsPerMesh, &(pg_nbFacesPerMeshFile));
	//printf("File mesh %s has %d objects\n", mesh_file_name.c_str(), nbMeshObjectsInFile);
	if (nbMeshObjectsInFile > 0) {
		for (int ind = 0; ind < nbMeshObjectsInFile; ind++) {
			//printf("   Object %d Nb faces %d vertices %d tex coord %d normals %d\n", ind,
			//	pg_nbFacesPerMeshFile[ind], nbVerticesPerMesh[ind],
			//	nbTexCoordsPerMesh[ind], nbNormalsPerMesh[ind]);
		}
	}
	else {
		//printf("File mesh withouth objects\n");
		pg_nbObjectsPerMeshFile = 0;
		exit(0);
	}

	///////////////////////////////////////////////////////////
	// vertex buffer objects and vertex array for the mesh
	mesh_vao = new unsigned int[nbMeshObjectsInFile];
	mesh_index_vbo = new unsigned int[nbMeshObjectsInFile];
	mesh_barycenter = new float[nbMeshObjectsInFile * 3];
	for (int ind = 0; ind < nbMeshObjectsInFile; ind++) {
		mesh_vao[ind] = NULL_ID;
		mesh_index_vbo[ind] = NULL_ID;
		mesh_barycenter[3 * ind + 0] = 0.f;
		mesh_barycenter[3 * ind + 1] = 0.f;
		mesh_barycenter[3 * ind + 2] = 0.f;
	}

	pg_nbObjectsPerMeshFile = nbMeshObjectsInFile;
	rewind(fileMesh);
	pg_parseMeshObj(fileMesh, indMeshFile, nbMeshObjectsInFile, nbVerticesPerMesh,
		nbTexCoordsPerMesh, nbNormalsPerMesh, pg_nbFacesPerMeshFile, indScenario);
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
	for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
		std::cout << "    " << indScenario << ": ";
		int nbMeshObjects = 0;
		int indMeshFile = 0;
		for (MeshData &aMesh : pg_Meshes[indScenario]) {
			std::cout << aMesh.pg_Mesh_fileName << ",";
			pg_Meshes[indScenario][indMeshFile].pg_load_mesh_objects(aMesh.pg_Mesh_fileName, indMeshFile, indScenario);
			nbMeshObjects += aMesh.pg_nbObjectsPerMeshFile;
			std::cout << " (" << aMesh.pg_nbObjectsPerMeshFile << " objects).";
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
			for (int indMeshSubPart = 0; indMeshSubPart < pg_Meshes[pg_ind_scenario][indMeshFile].pg_MmeShanghai_NbMeshSubParts; indMeshSubPart++) {
				aMesh.pg_MmeShanghai_MeshSubParts[indMeshSubPart] = new bool[aMesh.pg_nbObjectsPerMeshFile];
				pg_loadMeshSubParts(aMesh.pg_MmeShanghai_MeshSubPart_FileNames[indMeshSubPart],
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
void pg_loadMeshSubParts(string meshPart_fileName, bool* ObjectsInSubPart, int nbObjectsInMesh) {
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH ARMATURE RENDERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_render_one_bone(Bone* bone, glm::mat4 parentModelMatrix) {
	if (!bone)
		return;
	//if (!bone->parentBone) {
	//	printf("render bone %s\n", bone->id.c_str());
	//	std::cout << glm::to_string(parentModelMatrix) << std::endl;
	//	std::cout << glm::to_string(bone->boneInitialTranslationMatrix) << std::endl;
	//	std::cout << glm::to_string(bone->boneInitialRotationMatrix) << std::endl;
	//}

	// OpenGL transformations for drawing (structure)
	glm::mat4 localModelMatrix
		= parentModelMatrix
		* bone->boneInitialTranslationMatrix
		* bone->boneInitialRotationMatrix;
	//if (!bone->parentBone)
	//	localModelMatrix = parentModelMatrix;

	// OpenGL transformations for drawing (animation)
	// combination of local model matrix and bone animation matrix
	localModelMatrix = localModelMatrix * bone->boneAnimationRotationMatrix;

	// bone graphical rendering
	glUniformMatrix4fv(uniform_Mesh_vp_model[pg_ind_scenario], 1, GL_FALSE,
		glm::value_ptr(localModelMatrix));
	// to be checked
	//glUniformMatrix4fv(uniform_Mesh_vp_inverseModel, 1, GL_FALSE,
	//	glm::value_ptr(glm::inverse(localModelMatrix)));
	//GLfloat boneColor[3] = { 1,1,1 };
	//glUniform3fv(bodyShader->uniform_object_objectColor,
	//	1, boneColor);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// draw triangle strips from the currently bound VAO
	// with current in-use shader
	glBindVertexArray(bone->vao);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

	// recursive call
	pg_render_one_bone(bone->daughterBone, localModelMatrix);

	// recursive call
	pg_render_one_bone(bone->sisterBone, parentModelMatrix);
}

void pg_render_bones(glm::mat4 modelMatrix, int indMeshFile) {
	// calls rendering on the root bone (the first in the table of bones)
	pg_render_one_bone(pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones, modelMatrix);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH ARMATURE ANIMATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_randomAnimationPose(int indMeshFile, int mesh_AnimationPose) {
	if (indMeshFile < int(pg_Meshes[pg_ind_scenario].size())
		&& pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses > 0
		&& mesh_AnimationPose < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_MaxAnimationPoses) {
		for (int indBone = 0; indBone < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones; indBone++) {
			Bone* curBone = &pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones[indBone];
			curBone->boneAnimationPoseRotationQuat[mesh_AnimationPose] 
				= glm::normalize(glm::quat(rand_0_1, rand_0_1, rand_0_1, rand_0_1));
			curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose] 
				= curBone->boneLibraryPoseTranslationVector[0];
		}
	}
	else {
		sprintf(pg_errorStr, "Error: incorrect mesh index %d (max %d) animation pose index %d (max %d) !",
			indMeshFile, pg_Meshes[pg_ind_scenario].size(),
			mesh_AnimationPose, pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_MaxAnimationPoses); pg_ReportError(pg_errorStr);
	}
}

void pg_copyLibraryPoseToAnimationPose(int indMeshFile, int chosen_mesh_LibraryPose, int mesh_AnimationPose) {
	if (indMeshFile < int(pg_Meshes[pg_ind_scenario].size())
		&& chosen_mesh_LibraryPose < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses 
		&& mesh_AnimationPose < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_MaxAnimationPoses) {
		for (int indBone = 0; indBone < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_bones; indBone++) {
			Bone* curBone = &pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_tabBones[indBone];
			curBone->boneAnimationPoseRotationQuat[mesh_AnimationPose] = curBone->boneLibraryPoseRotationQuat[chosen_mesh_LibraryPose];
			curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose] = curBone->boneLibraryPoseTranslationVector[chosen_mesh_LibraryPose];
			//if (indBone == 0) {
			//	printf("Translation bone %s pose %d %.2f %.2f %.2f \n", curBone->id.c_str(), 
			//		mesh_AnimationPose,
			//		curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose].x,
			//		curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose].y,
			//		curBone->boneAnimationPoseTranslationVector[mesh_AnimationPose].z);
			//}
		}
	}
	else {
		sprintf(pg_errorStr, "Error: incorrect mesh index %d (max %d) library pose index %d (max %d)  animation pose index %d (max %d) !",
			indMeshFile, pg_Meshes[pg_ind_scenario].size(), 
			chosen_mesh_LibraryPose, pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses, 
			mesh_AnimationPose, pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_MaxAnimationPoses); pg_ReportError(pg_errorStr);
	}
}


void MeshAnimationData::pg_update_BoneAnimation(int indMeshFile) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_mesh_anime]) {
		bool new_anim = mesh_anime != pg_mesh_precedingAnime && pg_mesh_precedingAnime != 0;
		if (new_anim) {
			pg_mesh_startAnime = pg_CurrentClockTime;
			pg_mesh_precedingAnime = mesh_anime;
		}
		switch (mesh_anime) {
		// frozen
		case 0: {
			if (new_anim && pg_chosen_mesh_LibraryPose1 == -1) {
				if (mesh_anime_min_range < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses) {
					pg_chosen_mesh_LibraryPose1 = mesh_anime_min_range;
					pg_chosen_mesh_LibraryPose2 = mesh_anime_min_range;
				}
				else if (pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses > 0) {
					pg_chosen_mesh_LibraryPose1 = 0;
					pg_chosen_mesh_LibraryPose2 = 0;
				}
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose1, 0);
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose1, 1);
				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
				pg_interpolation_weight_AnimationPose[0] = 1.f;
				pg_nb_CurAnimationPoses = 1;
			}
			else {
				// frozen: nothing is done, time is frozen
				pg_mesh_startAnime += pg_CurrentClockTime - pg_PrecedingClockTime;
			}
		}
		break;
		// n-ary between mesh_anime_min_range and mesh_anime_max_range
		case 1: {
			if (new_anim) {
				if (mesh_anime_max_range < mesh_anime_min_range) {
					int tmp = mesh_anime_min_range;
					mesh_anime_min_range = mesh_anime_max_range;
					mesh_anime_max_range = tmp;
				}
				mesh_anime_min_range = max(0, min(pg_nb_LibraryPoses - 1, mesh_anime_min_range));
				mesh_anime_max_range = max(0, min(pg_nb_LibraryPoses - 1, mesh_anime_max_range));
				int range_width = mesh_anime_max_range - mesh_anime_min_range + 1;
				pg_chosen_mesh_LibraryPose1 = int(floor(rand_0_1 * (range_width))) + mesh_anime_min_range;
				pg_chosen_mesh_LibraryPose2 = int(floor(rand_0_1 * (range_width))) + mesh_anime_min_range;
				pg_chosen_mesh_LibraryPose1 = max(0, min(pg_nb_LibraryPoses - 1, pg_chosen_mesh_LibraryPose1));
				pg_chosen_mesh_LibraryPose2 = max(0, min(pg_nb_LibraryPoses - 1, pg_chosen_mesh_LibraryPose2));
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose1, 0);
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose2, 1);

				pg_mesh_positiveChange = false;
				pg_mesh_negativeChange = false;

				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			pg_interpolation_weight_AnimationPose[1] = 1.f - pg_interpolation_weight_AnimationPose[0];
			pg_nb_CurAnimationPoses = 2;

			float Pose0 = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			if (Pose0 > 1.f - 0.01f && pg_mesh_positiveChange == false) {
				int range_width = mesh_anime_max_range - mesh_anime_min_range + 1;
				pg_chosen_mesh_LibraryPose2 = int(floor(rand_0_1 * (range_width))) + mesh_anime_min_range;
				pg_chosen_mesh_LibraryPose2 = max(0, min(pg_nb_LibraryPoses - 1, pg_chosen_mesh_LibraryPose2));
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose2, 1);
				//printf("pose %.2f new anim1 %d anim2 %d\n", Pose0, pg_chosen_mesh_LibraryPose1, pg_chosen_mesh_LibraryPose2);
				pg_mesh_positiveChange = true;
				pg_mesh_negativeChange = false;
				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			if (Pose0 < 0.01f && pg_mesh_negativeChange == false) {
				int range_width = mesh_anime_max_range - mesh_anime_min_range + 1;
				pg_chosen_mesh_LibraryPose1 = int(floor(rand_0_1 * (range_width))) + mesh_anime_min_range;
				pg_chosen_mesh_LibraryPose1 = max(0, min(pg_nb_LibraryPoses - 1, pg_chosen_mesh_LibraryPose1));
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose1, 0);
				//printf("pose %.2f anim1 %d new anim2 %d\n", Pose0, pg_chosen_mesh_LibraryPose1, pg_chosen_mesh_LibraryPose2);
				pg_mesh_negativeChange = true;
				pg_mesh_positiveChange = false;
				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[0] = Pose0;
			pg_interpolation_weight_AnimationPose[1] = 1.f - Pose0;
			pg_nb_CurAnimationPoses = 2;
			break;
		}
			  // n-ary between mesh_anime_min_range and mesh_anime_max_range wo interpolation
		case 2: {
			float pose = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			if (new_anim || (pose > 1.f - 0.01f && pg_mesh_positiveChange == false)) {
				if (new_anim) {
					if (mesh_anime_max_range < mesh_anime_min_range) {
						int tmp = mesh_anime_min_range;
						mesh_anime_min_range = mesh_anime_max_range;
						mesh_anime_max_range = tmp;
					}
				}
				pg_mesh_positiveChange = true;
				int range_width = mesh_anime_max_range - mesh_anime_min_range + 1;
				pg_chosen_mesh_LibraryPose1 = int(floor(rand_0_1 * (range_width))) + mesh_anime_min_range;
				pg_chosen_mesh_LibraryPose1 = max(0, min(pg_nb_LibraryPoses - 1, pg_chosen_mesh_LibraryPose1));
				pg_copyLibraryPoseToAnimationPose(indMeshFile, pg_chosen_mesh_LibraryPose1, 0);
				pg_interpolation_weight_AnimationPose[pg_chosen_mesh_LibraryPose1] = 1.f;
			}
			if (pose < 0.01f && pg_mesh_positiveChange == true) {
				pg_mesh_positiveChange = false;
			}
			pg_nb_CurAnimationPoses = 1;
			break;
		}
			  // n-ary of random poses
		case 3: {
			if (new_anim) {
				pg_randomAnimationPose(indMeshFile, 0);
				pg_randomAnimationPose(indMeshFile, 1);

				pg_mesh_positiveChange = false;
				pg_mesh_negativeChange = false;

				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			pg_interpolation_weight_AnimationPose[1] = 1.f - pg_interpolation_weight_AnimationPose[0];
			pg_nb_CurAnimationPoses = 2;

			float Pose0 = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			if (Pose0 > 1.f - 0.01f && pg_mesh_positiveChange == false) {
				pg_randomAnimationPose(indMeshFile, 1);
				pg_mesh_positiveChange = true;
				pg_mesh_negativeChange = false;
				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			if (Pose0 < 0.01f && pg_mesh_negativeChange == false) {
				pg_randomAnimationPose(indMeshFile, 0);
				pg_mesh_negativeChange = true;
				pg_mesh_positiveChange = false;
				for (int indPose = 0; indPose < pg_nb_MaxAnimationPoses; indPose++) {
					pg_interpolation_weight_AnimationPose[indPose] = 0.f;
				}
			}
			pg_interpolation_weight_AnimationPose[0] = Pose0;
			pg_interpolation_weight_AnimationPose[1] = 1.f - Pose0;
			pg_nb_CurAnimationPoses = 2;
			break;
		}
			  // n-ary between mesh_anime_min_range and mesh_anime_max_range wo interpolation
		case 4: {
			float pose = float((sin((pg_CurrentClockTime - pg_mesh_startAnime) * mesh_anime_speed) + 1.f) / 2.f);
			if (new_anim || (pose > 1.f - 0.01f && pg_mesh_positiveChange == false)) {
				pg_mesh_positiveChange = true;
				pg_randomAnimationPose(indMeshFile, 0);
				pg_interpolation_weight_AnimationPose[pg_chosen_mesh_LibraryPose1] = 1.f;
			}
			if (pose < 0.01f && pg_mesh_positiveChange == true) {
				pg_mesh_positiveChange = false;
			}
			pg_nb_CurAnimationPoses = 1;
			break;
		}
		}
	}
}

void MeshAnimationData::pg_compute_BoneTransformations(int indMeshFile) {
	// calls rendering on the root bone (the first in the table of bones)
	for (int indBone = 0; indBone < pg_nb_bones; indBone++) {
		Bone* curBone = &pg_tabBones[indBone];

		// rotation interpolation for poses
		if (pg_nb_CurAnimationPoses > 0) {
			glm::quat animQuat(1.0, 0.0, 0.0, 0.0);
			float weight_sum = 0.f;
			for (int indPose = 0; indPose < pg_nb_CurAnimationPoses; indPose++) {
				weight_sum += pg_interpolation_weight_AnimationPose[indPose];
				if (indPose == 0) {
					animQuat = curBone->boneAnimationPoseRotationQuat[0];
				}
				else {
					if (pg_interpolation_weight_AnimationPose[indPose] > 0 && weight_sum > 0) {
						animQuat = glm::mix(animQuat,
							curBone->boneAnimationPoseRotationQuat[indPose], pg_interpolation_weight_AnimationPose[indPose] / weight_sum);
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
			if (pg_nb_CurAnimationPoses > 0) {
				glm::vec3 animTransl(0);
				float weight_sum = 0.f;
				for (int indPose = 0; indPose < pg_nb_CurAnimationPoses; indPose++) {
					weight_sum += pg_interpolation_weight_AnimationPose[indPose];
					if (indPose == 0) {
						animTransl = curBone->boneAnimationPoseTranslationVector[0];
					}
					else {
						if (pg_interpolation_weight_AnimationPose[indPose] > 0 && weight_sum > 0) {
							animTransl = glm::mix(animTransl,
								curBone->boneAnimationPoseTranslationVector[indPose], pg_interpolation_weight_AnimationPose[indPose] / weight_sum);
						}
					}
				}
				curBone->boneAnimationTranslationMatrix = glm::translate(glm::mat4(1.0f), animTransl);
				//if (pg_FrameNo % 20 == 0) {
				//	printf("Current root translation %.3f %.3f %.3f\n", animTransl.x, animTransl.y, animTransl.z);
				//}
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

void MeshData::pg_compute_MeshMotion(int indMeshFile, bool with_motion, MeshAnimationData *animData) {
	if (with_motion) {
		pg_Mesh_Translation_X
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Translation_X;
		pg_Mesh_Translation_Y
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Translation_Y;
		pg_Mesh_Rotation_angle
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Rotation_angle;
		pg_Mesh_Scale
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Scale;
		pg_Mesh_Rotation_X
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Rotation_X;
		pg_Mesh_Rotation_Y
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Rotation_Y;
		pg_Mesh_Rotation_Z
			= animData->pg_interpolation_weight_MotionPose[0] * animData->pg_motionPoses[0].pose_Mesh_Rotation_Z;
		for (int indMotion = 1; indMotion < animData->pg_nb_CurMotionPoses; indMotion++) {
			pg_Mesh_Translation_X
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Translation_X;
			pg_Mesh_Translation_Y
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Translation_Y;
			pg_Mesh_Rotation_angle
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Rotation_angle;
			pg_Mesh_Scale
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Scale;
			pg_Mesh_Rotation_X
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Rotation_X;
			pg_Mesh_Rotation_Y
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Rotation_Y;
			pg_Mesh_Rotation_Z
				+= animData->pg_interpolation_weight_MotionPose[indMotion] * animData->pg_motionPoses[indMotion].pose_Mesh_Rotation_Z;
		}
	}
	else {
		pg_Mesh_Translation_X = pg_Mesh_Translation_Ini_X;
		pg_Mesh_Translation_Y = pg_Mesh_Translation_Ini_Y;
		pg_Mesh_Rotation_angle = mesh_rotation;
		pg_Mesh_Rotation_X = pg_Mesh_Rotation_Ini_X;
		pg_Mesh_Rotation_Y = pg_Mesh_Rotation_Ini_Y;
		pg_Mesh_Rotation_Z = pg_Mesh_Rotation_Ini_Z;
	}

	pg_Mesh_Translation_X += mesh_translation_X;
	pg_Mesh_Translation_Y += mesh_translation_Y;
	pg_Mesh_Rotation_angle += mesh_rotation;
	pg_Mesh_Scale = mesh_scale;

	// pg_audio_pulse update
	pg_Mesh_Translation_X = (pg_Mesh_Translation_X + mesh_translation_X_pulse * (pulse_average - 0.5f));
	pg_Mesh_Translation_Y = pg_Mesh_Translation_Y + mesh_translation_Y_pulse * (pulse_average - 0.5f);
	pg_Mesh_Rotation_angle = (pg_Mesh_Rotation_angle + mesh_rotation_pulse * (pulse_average - 0.5f));
	pg_Mesh_Scale = pg_Mesh_Scale + mesh_scale_pulse * (pulse_average - 0.5f);
}

void MeshAnimationData::pg_update_MeshMotion(int indMeshFile) {
	//pg_Mesh_Translation_X[indMesh] = (mesh_translation_X + mesh_translation_X_pulse * (pulse_average - 0.5f)) * (indMesh % 2 == 0 ? 1 : -1);
	//pg_Mesh_Translation_Y[indMesh] = mesh_translation_Y + mesh_translation_Y_pulse * (pulse_average - 0.5f);
	//pg_Mesh_Rotation_angle[indMesh] = (mesh_rotation + mesh_rotation_pulse * (pulse_average - 0.5f)) * (indMesh % 2 == 0 ? 1 : -1);
	//pg_Mesh_Scale[indMesh] = mesh_scale + mesh_scale_pulse * (pulse_average - 0.5f);

	bool new_motion = mesh_motion != pg_mesh_precedingMotion;
	bool with_motion = false;

	if (pg_nb_MaxMotionPoses < 3) {
		sprintf(pg_errorStr, "Error: there should be minimally 3 poses for each object not %d", pg_nb_MaxMotionPoses); pg_ReportError(pg_errorStr);
		return;
	}

	if (mesh_motion < 0) {
		pg_mesh_motion_precTime = pg_CurrentClockTime;
	}

	if (new_motion) {
		pg_mesh_startMotion = pg_CurrentClockTime;
		pg_mesh_precedingMotion = mesh_motion;
	}
	switch (mesh_motion) {
	// frozen
	case 0: {
		pg_mesh_startMotion += pg_CurrentClockTime - pg_mesh_motion_precTime;
		pg_motionPoses[0].pose_Mesh_Translation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Ini_X;
		pg_motionPoses[0].pose_Mesh_Translation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Translation_Ini_Y;
		pg_motionPoses[0].pose_Mesh_Rotation_angle = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_angle;
		pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
		pg_motionPoses[0].pose_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_X;
		pg_motionPoses[0].pose_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Y;
		pg_motionPoses[0].pose_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Z;
		pg_interpolation_weight_MotionPose[0] = 1.f;
		pg_nb_CurMotionPoses = 1;
		with_motion = true;
	}
	break;
	// getting closer and facing / getting distant and same direction
	case 1: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = 1.f * (indMeshFile % 2 == 0 ? 10.f : -10.f);
			pg_motionPoses[1].pose_Mesh_Translation_X = 1.f * (indMeshFile % 2 == 0 ? 2.f : -2.f);
			pg_motionPoses[0].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[1].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_angle = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(PI / 2) * (indMeshFile % 2 == 0 ? -1 : 1);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_X;
			pg_motionPoses[1].pose_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_X;
			pg_motionPoses[0].pose_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Y;
			pg_motionPoses[1].pose_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Y;
			pg_motionPoses[0].pose_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Z;
			pg_motionPoses[1].pose_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Z;
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		with_motion = true;
		break;
	}
		  // horizontal parallel translation
	case 2: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 15.f : -3.f);
			pg_motionPoses[1].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 3.f : -15.f);
			pg_motionPoses[0].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[1].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_angle = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(PI / 4);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_X;
			pg_motionPoses[1].pose_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_X;
			pg_motionPoses[0].pose_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Y;
			pg_motionPoses[1].pose_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Y;
			pg_motionPoses[0].pose_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Z;
			pg_motionPoses[1].pose_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMeshFile].pg_Mesh_Rotation_Ini_Z;
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		with_motion = true;
		break;
	}
		  // lateral opposite translation plus rotation around horizontal axis
	case 3: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 5.f : -5.f);
			pg_motionPoses[1].pose_Mesh_Translation_X = (indMeshFile % 2 == 0 ? 10.f : -10.f);
			pg_motionPoses[0].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[1].pose_Mesh_Translation_Y = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_angle = float(PI / 6) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(-PI / 6) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_Y = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_Y = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_Z = 1.f;
			pg_motionPoses[1].pose_Mesh_Rotation_Z = 1.f;
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		with_motion = true;
		break;
	}
		  // motions at different distances and heights
	case 4: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = float(rand_0_1 * 20.f - 10.f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Translation_X = float(rand_0_1 * 20.f - 10.f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Translation_Y = float(rand_0_1 * 10.f - 5.f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Translation_Y = float(rand_0_1 * 10.f - 5.f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Rotation_angle = float(rand_0_1 * PI / 3 - PI / 6);
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(rand_0_1 * PI / 3 - PI / 6);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[1].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1));
			pg_motionPoses[0].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[1].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1));
			pg_motionPoses[0].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
			pg_motionPoses[1].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1));
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		with_motion = true;
		break;
	}
		  // close poses almost overlapping
	case 5: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = rand_0_1 * 7.f * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Translation_X = rand_0_1 * 7.f * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Translation_Y = float(-rand_0_1 * 5.f + 2.5f);
			pg_motionPoses[1].pose_Mesh_Translation_Y = float(rand_0_1 * 5.f - 2.5f);
			pg_motionPoses[0].pose_Mesh_Rotation_angle = float(-PI / 6);
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(PI / 6);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_X = 0.f;
			pg_motionPoses[0].pose_Mesh_Rotation_Y = 1.f;
			pg_motionPoses[1].pose_Mesh_Rotation_Y = 1.f;
			pg_motionPoses[0].pose_Mesh_Rotation_Z = 0.f;
			pg_motionPoses[1].pose_Mesh_Rotation_Z = 0.f;
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		with_motion = true;
		break;
	}
		  // random poses changing
	case 6: {
		if (new_motion) {
			for (int indPose = 0; indPose < pg_nb_MaxMotionPoses; indPose++) {
				pg_interpolation_weight_MotionPose[indPose] = 0.f;
			}
			pg_motionPoses[0].pose_Mesh_Translation_X = float(rand_0_1 * 15.f - 7.5f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Translation_X = float(rand_0_1 * 15.f - 7.5f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Translation_Y = float(rand_0_1 * 5.f - 2.5f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Translation_Y = float(rand_0_1 * 5.f - 2.5f) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Rotation_angle = float(rand_0_1 * 2.f * PI - PI);
			pg_motionPoses[1].pose_Mesh_Rotation_angle = float(rand_0_1 * 2.f * PI - PI);
			pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
			pg_motionPoses[0].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Rotation_X = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Rotation_Y = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[0].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
			pg_motionPoses[1].pose_Mesh_Rotation_Z = max(0.1f, float(rand_0_1)) * (indMeshFile % 2 == 0 ? 1 : -1);
		}
		pg_interpolation_weight_MotionPose[0] = float((sin((pg_CurrentClockTime - pg_mesh_startMotion) * mesh_motion_speed) + 1.f) / 2.f);
		pg_interpolation_weight_MotionPose[1] = 1.f - pg_interpolation_weight_MotionPose[0];
		pg_nb_CurMotionPoses = 2;
		pg_motionPoses[0].pose_Mesh_Translation_X += float(rand_0_1 * 0.02f - 0.01f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[1].pose_Mesh_Translation_X += float(rand_0_1 * 0.02f - 0.01f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[0].pose_Mesh_Translation_Y += float(rand_0_1 * 0.02f - 0.01f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[1].pose_Mesh_Translation_Y += float(rand_0_1 * 0.02f - 0.01f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[0].pose_Mesh_Rotation_angle += float(rand_0_1 * 0.01f - 0.005f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[1].pose_Mesh_Rotation_angle += float(rand_0_1 * 0.01f - 0.005f) * (indMeshFile % 2 == 0 ? 1 : -1);
		pg_motionPoses[0].pose_Mesh_Scale = mesh_scale;
		pg_motionPoses[1].pose_Mesh_Scale = mesh_scale;
		with_motion = true;
		break;
	}
	}
	pg_mesh_motion_precTime = pg_CurrentClockTime;

	// updates the rotation and translations of the mesh from the motion poses and interpolation coeficients
	pg_Meshes[pg_ind_scenario][indMeshFile].pg_compute_MeshMotion(indMeshFile, with_motion, this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH DISPLAY/ANIMATION TOGGLE
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_meshOn(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = activeMeshes & (1 << (indMesh - 1));
		if (!isMeshOn) {
			activeMeshes |= (1 << (indMesh - 1));
		}
		pg_BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_activeMeshes]) = activeMeshes;
		sprintf(pg_AuxString, "/Mesh_%d_onOff %d", indMesh, 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_meshOff(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = activeMeshes & (1 << (indMesh - 1));
		if (isMeshOn) {
			activeMeshes = activeMeshes & ~(1 << (indMesh - 1));
		}
		pg_BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_activeMeshes]) = mobileMeshes;
		sprintf(pg_AuxString, "/Mesh_%d_onOff %d", indMesh, 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}

#if defined(var_Caverne_Mesh_Profusion)
void Caverne_Mesh_Profusion_On(int indMesh) {
	pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneActveMesh = !pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneActveMesh;
	pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneMeshBirthTime = pg_CurrentClockTime;
	pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneMeshDeathTime = pg_CurrentClockTime + 100000.f;
}
void Caverne_Mesh_Profusion_Off(int indMesh) {
	pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneMeshDeathTime = pg_CurrentClockTime + pg_Meshes[pg_ind_scenario][indMesh].pg_CaverneMeshWakeupTime / 3.f;
}
#endif
void pg_meshMobileOnOff(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = mobileMeshes & (1 << (indMesh - 1));
		printf("Mesh on %d\n", isMeshOn);
		if (isMeshOn) {
			mobileMeshes = mobileMeshes & ~(1 << (indMesh - 1));
		}
		else {
			mobileMeshes |= (1 << (indMesh - 1));
		}
		pg_BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(pg_AuxString, "/Mesh_mobile_%d_onOff %d", indMesh, (!isMeshOn)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_meshMobileOff(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = mobileMeshes & (1 << (indMesh - 1));
		if (isMeshOn) {
			mobileMeshes = mobileMeshes & ~(1 << (indMesh - 1));
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_X = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_Ini_X;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_Y = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_Ini_Y;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_Z = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Translation_Ini_Z;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_angle = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Ini_angle;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_X = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Ini_X;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Y = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Ini_Y;
			pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Z = pg_Meshes[pg_ind_scenario][indMesh - 1].pg_Mesh_Rotation_Ini_Z;
		}
		pg_BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(pg_AuxString, "/Mesh_mobile_%d_onOff %d", indMesh, 0); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_meshMobileOn(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = mobileMeshes & (1 << (indMesh - 1));
		if (!isMeshOn) {
			mobileMeshes |= (1 << (indMesh - 1));
		}
		pg_BrokenInterpolationVar[_mobileMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_mobileMeshes]) = mobileMeshes;
		sprintf(pg_AuxString, "/Mesh_mobile_%d_onOff %d", indMesh, 1); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
void pg_meshOnOff(int indMesh) {
	if (indMesh <= int(pg_Meshes[pg_ind_scenario].size())) {
		bool isMeshOn = activeMeshes & (1 << (indMesh - 1));
		if (isMeshOn) {
			activeMeshes = activeMeshes & ~(1 << (indMesh - 1));
		}
		else {
			activeMeshes |= (1 << (indMesh - 1));
		}
		pg_BrokenInterpolationVar[_activeMeshes] = true;
		*((int*)pg_FullScenarioVarPointers[_activeMeshes]) = activeMeshes;
		sprintf(pg_AuxString, "/Mesh_%d_onOff %d", indMesh, (!isMeshOn)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
 
//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH SCENARIO
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_parseScenario_Meshes(std::ifstream& scenarioFin, int indScenario) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// MESHES
	// the meshes are loaded inside the GPU and diplayed depending on their activity
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string meshes
	if (ID.compare("meshes") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"meshes\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/meshes") == 0) {
			break;
		}

		MeshData *aMesh;
		MeshAnimationData *aMeshAnimation;
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_activeMeshes]) {
			// adds a new mesh
			aMesh = new MeshData(indScenario);
			aMeshAnimation = new MeshAnimationData();

			if (ID.compare("mesh") != 0) {
				sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"mesh\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
			
			aMesh->pg_Mesh_Scale = 1.0f;
			sstream >> aMesh->pg_Mesh_fileName; // file name
			// full path is not given, look in default local path
			if (!pg_isFullPath(aMesh->pg_Mesh_fileName)) {
				aMesh->pg_Mesh_fileName
					= pg_meshes_directory + aMesh->pg_Mesh_fileName;
			}

			// image initial geometry
			sstream >> aMesh->pg_Mesh_Scale;
			sstream >> aMesh->pg_Mesh_Translation_X;
			sstream >> aMesh->pg_Mesh_Translation_Y;
			sstream >> aMesh->pg_Mesh_Translation_Z;
			sstream >> aMesh->pg_Mesh_Rotation_angle;
			sstream >> aMesh->pg_Mesh_Rotation_X;
			sstream >> aMesh->pg_Mesh_Rotation_Y;
			sstream >> aMesh->pg_Mesh_Rotation_Z;
			if (aMesh->pg_Mesh_Rotation_X == 0 && aMesh->pg_Mesh_Rotation_Y == 0 && aMesh->pg_Mesh_Rotation_Z == 0) {
				sprintf(pg_errorStr, "Error: incorrect mesh %s configuration: rotation with Null axix ", aMesh->pg_Mesh_fileName.c_str()); pg_ReportError(pg_errorStr); throw 100;
			}
			sstream >> aMesh->pg_Mesh_Motion_X;
			sstream >> aMesh->pg_Mesh_Motion_Y;
			sstream >> aMesh->pg_Mesh_Motion_Z;
			aMesh->pg_Mesh_Translation_Ini_X = aMesh->pg_Mesh_Translation_X;
			aMesh->pg_Mesh_Translation_Ini_Y = aMesh->pg_Mesh_Translation_Y;
			aMesh->pg_Mesh_Translation_Ini_Z = aMesh->pg_Mesh_Translation_Z;
			aMesh->pg_Mesh_Rotation_Ini_angle = aMesh->pg_Mesh_Rotation_angle;
			aMesh->pg_Mesh_Rotation_Ini_X = aMesh->pg_Mesh_Rotation_X;
			aMesh->pg_Mesh_Rotation_Ini_Y = aMesh->pg_Mesh_Rotation_Y;
			aMesh->pg_Mesh_Rotation_Ini_Z = aMesh->pg_Mesh_Rotation_Z;
			sstream >> ID;
			std::regex pat("[0-9A-F]{8}");
			if (ID.length() != 9 || ID[0] != '#' || std::regex_match(ID.substr(1), pat) == false) {
				sprintf(pg_errorStr, "Error: incorrect configuration file Mesh color \"%s\" (hexadecimal color + alpha channel prefixed with # expected)", ID.c_str()); pg_ReportError(pg_errorStr);
				aMesh->pg_Mesh_Colors[0] = 1.f;
				aMesh->pg_Mesh_Colors[1] = 1.f;
				aMesh->pg_Mesh_Colors[2] = 1.f;
				aMesh->pg_Mesh_Colors[3] = 1.f;
			}
			else {
				unsigned int color
					= (unsigned int)(std::stoi(std::string("0x") + ID.substr(1,2), nullptr, 16));
				aMesh->pg_Mesh_Colors[0] = float(color)/255.f;
				color = (unsigned int)(std::stoi(std::string("0x") + ID.substr(3, 2), nullptr, 16));
				aMesh->pg_Mesh_Colors[1] = float(color) / 255.f;
				color = (unsigned int)(std::stoi(std::string("0x") + ID.substr(5, 2), nullptr, 16));
				aMesh->pg_Mesh_Colors[2] = float(color) / 255.f;
				color = (unsigned int)(std::stoi(std::string("0x") + ID.substr(7, 2), nullptr, 16));
				aMesh->pg_Mesh_Colors[3] = float(color) / 255.f;
			}
			sstream >> aMesh->pg_Mesh_TextureRank;
			//printf("Mesh #%d scale %.2f translation (%.2f,%.2f,%.2f), rotation %.2f\n",
			//	pg_Meshes[indScenario].size() + 1, aMesh->pg_Mesh_Scale, aMesh->pg_Mesh_Translation_X,
			//	aMesh->pg_Mesh_Translation_Y, aMesh->pg_Mesh_Translation_Z,
			//	aMesh->pg_Mesh_Rotation_angle);
			// the rank of the mesh textures applied to this mesh
#if defined(var_MmeShanghai_brokenGlass)
			if (pg_FullScenarioActiveVars[indScenario][_MmeShanghai_brokenGlass]) {
				sstream >> aMesh->pg_MmeShanghai_NbMeshSubParts;
				aMesh->pg_MmeShanghai_MeshSubPart_FileNames = new string[aMesh->pg_MmeShanghai_NbMeshSubParts];
				aMesh->pg_MmeShanghai_MeshSubParts = new bool* [aMesh->pg_MmeShanghai_NbMeshSubParts];
				for (int indPart = 0; indPart < aMesh->pg_MmeShanghai_NbMeshSubParts; indPart++) {
					sstream >> aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart];
					aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart]
						= pg_meshes_directory + aMesh->pg_MmeShanghai_MeshSubPart_FileNames[indPart];
					aMesh->pg_MmeShanghai_MeshSubParts[indPart] = NULL;
				}
			}
#endif
			pg_Meshes[indScenario].push_back(*aMesh);
			pg_Mesh_Animations[indScenario].push_back(*aMeshAnimation);
		}
		else {
			printf("incorrect mesh definition \n");
			sprintf(pg_errorStr, "Error: incorrect configuration file unexpected mesh definition at \"%s\"", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}
	}
	// Augmented Reality: FBO capture of Master to be displayed on a mesh
	if (pg_FullScenarioActiveVars[indScenario][_textureFrontier_wmin]) {
		if (pg_Meshes[indScenario].empty()) {
			sprintf(pg_errorStr, "Error: Augemented reality requires that at least one mesh file is declared in the scenario file"); pg_ReportError(pg_errorStr); throw 100;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////
 
void pg_aliasScript_Mesh(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
	case _Mesh_onOff:
		pg_meshOnOff(int(float_arguments[0]) + 1);
		break;

	case _Mesh_mobile_onOff:
		pg_meshMobileOnOff(int(float_arguments[0]) + 1);
		break;

#if defined(var_Caverne_Mesh_Profusion)
	case _Caverne_Mesh_Profusion_on: for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) { Caverne_Mesh_Profusion_On(indMesh); }; break;
	case _Caverne_Mesh_Profusion_off: for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) { Caverne_Mesh_Profusion_Off(indMesh); }; break;
	case _Caverne_Mesh_7Solids_on: for (int indMesh = 0; indMesh < 7; indMesh++) { pg_meshOn(indMesh + 1); pg_meshMobileOff(indMesh + 1); }; break;
	case _Caverne_Mesh_7Solids_off: for (int indMesh = 0; indMesh < 7; indMesh++) { pg_meshOff(indMesh + 1); pg_meshMobileOff(indMesh + 1); }; break;
	case _BGcolor_onOff: BGcolor = !BGcolor; break;
#endif
	case _Mesh_light_x: pg_mesh_light_x = float_arguments[0]; printf("MESH light x %.2f\n", float_arguments[0]);  break;
	case _Mesh_light_y: pg_mesh_light_y = float_arguments[0]; printf("MESH light y %.2f\n", float_arguments[0]);  break;
	case _Mesh_light_z: pg_mesh_light_z = float_arguments[0]; printf("MESH light z %.2f\n", float_arguments[0]);  break;
	default:
		sprintf(pg_errorStr, "Mesh command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void mesh_anime_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_ind_scenario].size()); indMesh++) {
			pg_Mesh_Animations[pg_ind_scenario][indMesh].pg_mesh_startAnime = pg_CurrentClockTime;
		}
	}
}
void mesh_motion_speed_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		for (int indMesh = 0; indMesh < int(pg_Meshes[pg_ind_scenario].size()); indMesh++) {
			pg_Mesh_Animations[pg_ind_scenario][indMesh].pg_mesh_startMotion = pg_CurrentClockTime;
		}
	}
}
void Caverne_Mesh_Profusion_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
#if defined(var_Caverne_Mesh_Profusion)
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
		//printf("/Caverne_Mesh_Profusion %.1f\n", scenario_or_gui_command_value);
		if (scenario_or_gui_command_value == 1.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
				Caverne_Mesh_Profusion_On(indMesh);
			}
		}
		else if (scenario_or_gui_command_value == 0.) {
			for (int indMesh = 7; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
				Caverne_Mesh_Profusion_Off(indMesh);
			}
		}
	}
#endif
}
void activeMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
			sprintf(pg_AuxString, "/Mesh_%d_onOff %d", indMesh + 1, (activeMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void mobileMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// MESH INTERFACE VARIABLE INITIALIZATION
		for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_ind_scenario].size(); indMesh++) {
			sprintf(pg_AuxString, "/Mesh_mobile_%d_onOff %d", indMesh + 1, (mobileMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void MmeShanghai_brokenGlass_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
#if defined(var_MmeShanghai_brokenGlass)
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		// activation of a mesh subgroup for which the edges will be drawn
	}
#endif
}

