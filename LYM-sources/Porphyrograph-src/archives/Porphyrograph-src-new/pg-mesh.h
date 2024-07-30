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

class Bone {
public:
    // bone ID
    string id;
    // bone length (along y axis)
    float length;
    // parent bone if there's one
    Bone* daughterBone;
    Bone* parentBone;
    Bone* sisterBone;

    ////////////////////////////////////////
    // geometrical data
    float points[2 * 3 * 3];
    unsigned int vbo;
    unsigned int vao;

    // initial translation matrix computed from translation vector
    glm::mat4      boneAnimationTranslationMatrix;
    glm::mat4      boneInitialTranslationMatrix;
    glm::vec3*     boneLibraryPoseTranslationVector;
    glm::vec3*     boneAnimationPoseTranslationVector;

    // initial and animation rotation computed from axis and angle
    glm::mat4      boneAnimationRotationMatrix;
    glm::mat4      boneInitialRotationMatrix;
    glm::quat      *boneLibraryPoseRotationQuat;
    glm::quat      *boneAnimationPoseRotationQuat;

    // joint Transformation Matrices (initial and current)
    glm::mat4      initialJointTransformation;
    glm::mat4      currentJointTransformation;

    // current point Transformation Matrices (initial and current)
    glm::mat4      pointAnimationMatrix;

    Bone(void) {
        id = "";
        length = 0;
        daughterBone = NULL;
        parentBone = NULL;
        sisterBone = NULL;

        boneInitialTranslationMatrix = glm::mat4(1.0f);
        boneAnimationTranslationMatrix = glm::mat4(1.0f);
        boneLibraryPoseTranslationVector = NULL;

        boneInitialRotationMatrix = glm::mat4(1.0f);
        boneAnimationRotationMatrix = glm::mat4(1.0f);
        boneLibraryPoseRotationQuat = NULL;

        initialJointTransformation = glm::mat4(1.0f);
        currentJointTransformation = glm::mat4(1.0f);

        pointAnimationMatrix = glm::mat4(1.0f);

        // 1st triangle
        points[0] = 0.05f;
        points[1] = 0.0f;
        points[2] = 0.05f;
        points[3] = -0.05f;
        points[4] = 0.0f;
        points[5] = -0.05f;
        points[6] = 0.0f;
        points[7] = 1.0f;
        points[8] = 0.0f;
        // 2nd triangle
        points[9] = -0.05f;
        points[10] = 0.0f;
        points[11] = 0.05f;
        points[12] = 0.05f;
        points[13] = 0.0f;
        points[14] = -0.05f;
        points[15] = 0.0f;
        points[16] = 1.0f;
        points[17] = 0.0f;
    }
    ~Bone(void) {
    }
};

class MeshData {
public:
	string pg_Mesh_fileNames;
	// geometrical transformations
	float pg_Mesh_Scale;
	float pg_Mesh_Rotation_angle;
	float pg_Mesh_Rotation_X;
	float pg_Mesh_Rotation_Y;
	float pg_Mesh_Rotation_Z;
	float pg_Mesh_Translation_X;
	float pg_Mesh_Translation_Y;
	float pg_Mesh_Translation_Z;
	float pg_Mesh_Rotation_Ini_X;
	float pg_Mesh_Rotation_Ini_Y;
	float pg_Mesh_Rotation_Ini_Z;
	float pg_Mesh_Translation_Ini_X;
	float pg_Mesh_Translation_Ini_Y;
	float pg_Mesh_Translation_Ini_Z;
	float pg_Mesh_Motion_X;
	float pg_Mesh_Motion_Y;
	float pg_Mesh_Motion_Z;
	int pg_Mesh_TextureRank;
	int pg_nbObjectsPerMeshFile;
	int* pg_nbFacesPerMeshFile;
#if defined(var_MmeShanghai_brokenGlass)
	bool* pg_MmeShanghaiActveMeshObjects;
	double* pg_MmeShanghaiMeshObjectWakeupTime;
	bool** pg_MmeShanghai_MeshSubParts;
	string* pg_MmeShanghai_MeshSubPart_FileNames;
	int pg_MmeShanghai_NbMeshSubParts;
	float* pg_MmeShanghai_Object_Rotation_angle;
	float* pg_MmeShanghai_Object_Rotation_X;
	float* pg_MmeShanghai_Object_Rotation_Y;
	float* pg_MmeShanghai_Object_Rotation_Z;
	float* pg_MmeShanghai_Object_Translation_X;
	float* pg_MmeShanghai_Object_Translation_Y;
	float* pg_MmeShanghai_Object_Translation_Z;
	float* pg_MmeShanghai_Object_Rotation_Ini_angle;
#endif
#if defined(var_Caverne_Mesh_Profusion)
	bool pg_CaverneActveMesh;
	float pg_CaverneMeshWakeupTime;
	float pg_CaverneMeshBirthTime;
	float pg_CaverneMeshDeathTime;
#endif
	// color
	array<float, 4> pg_Mesh_Colors;
	// textures
	GLuint Mesh_texture_rectangle;
	vector<unsigned int> mesh_vao;
	vector<array<float, 3>> mesh_barycenter;
	vector<unsigned int> mesh_index_vbo;
	MeshData(int indConfiguration) {
		pg_Mesh_fileNames = "";
		// geometrical transformations
		pg_Mesh_Scale = 1.f;
		pg_Mesh_Rotation_angle = 0.f;
		pg_Mesh_Rotation_X = 0.f;
		pg_Mesh_Rotation_Y = 0.f;
		pg_Mesh_Rotation_Z = 0.f;
		pg_Mesh_Translation_X = 0.f;
		pg_Mesh_Translation_Y = 0.f;
		pg_Mesh_Translation_Z = 0.f;
		pg_Mesh_Rotation_Ini_X = 0.f;
		pg_Mesh_Rotation_Ini_Y = 0.f;
		pg_Mesh_Rotation_Ini_Z = 0.f;
		pg_Mesh_Translation_Ini_X = 0.f;
		pg_Mesh_Translation_Ini_Y = 0.f;
		pg_Mesh_Translation_Ini_Z = 0.f;
		pg_Mesh_Motion_X = 0.f;
		pg_Mesh_Motion_Y = 0.f;
		pg_Mesh_Motion_Z = 0.f;
		pg_Mesh_TextureRank = 0;
		pg_Mesh_Colors[0] = 1.f;
		pg_Mesh_Colors[1] = 1.f;
		pg_Mesh_Colors[2] = 1.f;
		pg_Mesh_Colors[3] = 1.f;
		pg_nbObjectsPerMeshFile = 0;
		pg_nbFacesPerMeshFile = NULL;
		Mesh_texture_rectangle = NULL_ID;
		mesh_vao = {};
		mesh_barycenter = {};
		mesh_index_vbo = {};
#if defined(var_Caverne_Mesh_Profusion)
		if (pg_FullScenarioActiveVars[indConfiguration][_Caverne_Mesh_Profusion]) {
			pg_CaverneActveMesh = false;
			if (indFile < 7) {
				pg_CaverneMeshWakeupTime = float(rand_0_1 * 10.);
			}
			else {
				pg_CaverneMeshWakeupTime = float(rand_0_1 * 30.);
			}
			pg_CaverneMeshBirthTime = 0.f;
			pg_CaverneMeshDeathTime = 0.f;
		}
#endif
#if defined(var_MmeShanghai_brokenGlass)
		if (pg_FullScenarioActiveVars[indConfiguration][_MmeShanghai_brokenGlass]) {
			pg_MmeShanghai_MeshSubParts = NULL;
			pg_MmeShanghai_NbMeshSubParts = 0;
			pg_MmeShanghai_MeshSubPart_FileNames = NULL;
		}
		pg_MmeShanghaiActveMeshObjects = NULL;
		pg_MmeShanghaiMeshObjectWakeupTime = NULL;
		pg_MmeShanghai_MeshSubParts = NULL;
		pg_MmeShanghai_MeshSubPart_FileNames = NULL;
		pg_MmeShanghai_NbMeshSubParts = NULL;
		pg_MmeShanghai_Object_Rotation_angle = NULL;
		pg_MmeShanghai_Object_Rotation_X = NULL;
		pg_MmeShanghai_Object_Rotation_Y = NULL;
		pg_MmeShanghai_Object_Rotation_Z = NULL;
		pg_MmeShanghai_Object_Translation_X = NULL;
		pg_MmeShanghai_Object_Translation_Y = NULL;
		pg_MmeShanghai_Object_Translation_Z = NULL;
		pg_MmeShanghai_Object_Rotation_Ini_angle = NULL;
#endif
#if defined(var_Caverne_Mesh_Profusion)
		pg_CaverneActveMesh = false;
		pg_CaverneMeshWakeupTime = 0.f;
		pg_CaverneMeshBirthTime = 0.f;
		pg_CaverneMeshDeathTime = 0.f;
#endif
	}
	~MeshData(void) {
	}
};

#define PG_MAX_ANIMATION_POSES 7
extern int* pg_nb_bones[PG_MAX_CONFIGURATIONS];
extern Bone** TabBones[PG_MAX_CONFIGURATIONS];
extern int* pg_nb_AnimationPoses[PG_MAX_CONFIGURATIONS];
extern int* pg_nb_LibraryPoses[PG_MAX_CONFIGURATIONS];
extern float** pg_interpolation_weight_AnimationPose[PG_MAX_CONFIGURATIONS];
extern float** pg_interpolation_weight_MotionPose[PG_MAX_CONFIGURATIONS];
extern int* pg_nb_MotionPoses[PG_MAX_CONFIGURATIONS];
class MotionPose {
public:
	float pose_Mesh_Translation_X;
	float pose_Mesh_Translation_Y;
	float pose_Mesh_Rotation_X;
	float pose_Mesh_Rotation_Y;
	float pose_Mesh_Rotation_Z;
	float pose_Mesh_Rotation_angle;
	float pose_Mesh_Scale;
	MotionPose() {
		pose_Mesh_Translation_X = 0.f;
		pose_Mesh_Translation_Y = 0.f;
		pose_Mesh_Rotation_angle = 0.f;
		pose_Mesh_Rotation_X = 0.f;
		pose_Mesh_Rotation_Y = 1.f;
		pose_Mesh_Rotation_Z = 0.f;
		pose_Mesh_Scale = 1.f;
	}
	~MotionPose() {
	}
};
extern MotionPose** pg_motionPoses[PG_MAX_CONFIGURATIONS];


// MESHES
extern vector<MeshData> pg_Meshes[PG_MAX_CONFIGURATIONS];

// mesh anim data
extern double *mesh_startAnime[PG_MAX_CONFIGURATIONS];
extern double *mesh_anime_precTime[PG_MAX_CONFIGURATIONS];
extern int *mesh_precedingAnime[PG_MAX_CONFIGURATIONS];
extern bool* mesh_positiveChange[PG_MAX_CONFIGURATIONS];
extern bool* mesh_negativeChange[PG_MAX_CONFIGURATIONS];
#define _lastMesh_Anime 6
#define _lastMesh_Motion 6

// mesh motion data
extern double* mesh_startMotion[PG_MAX_CONFIGURATIONS];
extern double* mesh_motion_precTime[PG_MAX_CONFIGURATIONS];
extern int* mesh_precedingMotion[PG_MAX_CONFIGURATIONS];

// mesh data
extern GLfloat *vertexBuffer;
extern GLfloat *texCoordBuffer;
extern GLfloat *normalBuffer;
extern GLuint  *indexBuffer;
extern GLint* boneIndexBuffer;
extern GLfloat* boneWeightBuffer;

// shader variable pointers
extern GLint *uniform_mesh_model[PG_MAX_CONFIGURATIONS];
extern GLint *uniform_mesh_view[PG_MAX_CONFIGURATIONS];
extern GLint *uniform_mesh_proj[PG_MAX_CONFIGURATIONS];
extern GLint *uniform_mesh_light[PG_MAX_CONFIGURATIONS];

// mesh lighting
extern GLfloat mesh_light_x;
extern GLfloat mesh_light_y;
extern GLfloat mesh_light_z;

void ParseScenarioMeshes(std::ifstream& scenarioFin, int indConfiguration);
void copyMeshData(int indObjectInMesh, GLfloat* vertexBufferIni, GLfloat* texCoordBufferIni, GLfloat* normalBufferIni,
	GLuint  *indexPointBufferIni, GLuint  *indexTexCoordBufferIni, GLuint  *indexNormalBufferIni,
	int nbFacesInThisMesh);
void copyLibraryPoseToAnimationPose(int indMeshFile, int chosen_mesh_LibraryPose, int mesh_AnimationPose);
void count_faces_mesh_obj(FILE* file, int* meshNo,
	int **nbVerticesInEachMesh, int **nbTextCoordsInEachMesh, int **nbNormalsInEachMesh,
	int **nbFacesInEachMesh);
void parseArmatureObj(FILE* file, char* line, char* tag, char* id, int indMeshFile, int indConfiguration);
void parseOneBoneObj(FILE* file, int level, char* line, char* tag, char* id, int* nbBonesLoc, int indMeshFile, int indConfiguration);
void parseMeshObj(FILE *file, int indMeshFile, int nbMeshObjects,
int *nbVerticesInEachMesh, int *nbTextCoordsInEachMesh, int *nbNormalsInEachMesh,
int *nbFacesInEachMesh, int indConfiguration);
void transferMeshDataToGPU(int indMeshFile, int indObjectInMesh, int indConfiguration);
void load_mesh_objects(string mesh_file_name, int indMeshFile, int indConfiguration);
void pg_loadAllMeshes(void);
#if defined(var_MmeShanghai_brokenGlass)
void loadMeshSubParts(string meshPart_fileName, bool* ObjectsInSubPart, int nbObjectsInMesh);
#endif

void render_one_bone(Bone* bone, glm::mat4 parentModelMatrix);
void render_bones(glm::mat4 modelMatrix, int indMeshFile);

void update_anim(int indMeshFile);
void update_bones(int indMeshFile);
#if defined(var_Contact_mesh_rotation) && defined(var_Contact_mesh_translation_X) && defined(var_Contact_mesh_translation_Y) && defined(var_Contact_mesh_motion)
void Contact_update_motion(int indMeshFile);
#endif
#endif
