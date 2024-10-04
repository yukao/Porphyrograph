/*! \file pg-mesh.h
 *  \brief the header files for classes ( \a pg_WindowData, \a pg_Environment ) 
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
// BONE MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

	// transformations provided in the obj file library of poses
	// static poses from the library inside the obj file
	glm::vec3*		boneLibraryPoseTranslationVector;
	glm::quat*		boneLibraryPoseRotationQuat;

	// transformations copied from the library or generated automatically
	// for making animations through pose interpolations
	// dynamic animation poses copied from the library or generated automatically
	glm::vec3*		boneAnimationPoseTranslationVector;
	glm::quat*		boneAnimationPoseRotationQuat;

	// initial transformation matrices computed from translation vector/quaternion
    glm::mat4		boneInitialTranslationMatrix;
	glm::mat4		boneInitialRotationMatrix;

    // transformations matrices computed computed from translation vector/quaternion
	glm::mat4		boneAnimationRotationMatrix;
	glm::mat4		boneAnimationTranslationMatrix;

    // joint Transformation Matrices (initial and current)
    glm::mat4		initialJointTransformation;
    glm::mat4		currentJointTransformation;

    // current point Transformation Matrices (initial and current)
    glm::mat4		pointAnimationMatrix;

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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////
class MeshAnimationData;

class MeshData {
public:
	string pg_Mesh_fileName;
	// geometrical transformations
	float pg_Mesh_Scale;
	float pg_Mesh_Rotation_angle;
	float pg_Mesh_Rotation_X;
	float pg_Mesh_Rotation_Y;
	float pg_Mesh_Rotation_Z;
	float pg_Mesh_Translation_X;
	float pg_Mesh_Translation_Y;
	float pg_Mesh_Translation_Z;
	float pg_Mesh_Rotation_Ini_angle;
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
	unsigned int *mesh_vao;
	float *mesh_barycenter;
	unsigned int *mesh_index_vbo;
	MeshData(int indScenario) {
		pg_Mesh_fileName = "";
		// geometrical transformations
		pg_Mesh_Scale = 1.f;
		pg_Mesh_Rotation_angle = 0.f;
		pg_Mesh_Rotation_X = 0.f;
		pg_Mesh_Rotation_Y = 0.f;
		pg_Mesh_Rotation_Z = 0.f;
		pg_Mesh_Translation_X = 0.f;
		pg_Mesh_Translation_Y = 0.f;
		pg_Mesh_Translation_Z = 0.f;
		pg_Mesh_Rotation_Ini_angle = 0.f;
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
		mesh_vao = NULL;
		mesh_barycenter = NULL;
		mesh_index_vbo = NULL;
#if defined(var_Caverne_Mesh_Profusion)
		if (pg_FullScenarioActiveVars[indScenario][_Caverne_Mesh_Profusion]) {
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
		if (pg_FullScenarioActiveVars[indScenario][_MmeShanghai_brokenGlass]) {
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
	void pg_transferMeshDataToGPU(int indMeshFile);
	void pg_load_mesh_objects(string mesh_file_name, int indMeshFile, int indScenario);
	void pg_copy_mesh_data_and_ship_to_GPU(int indObjectInMesh, GLfloat* vertexBufferIni,
		GLfloat* texCoordBufferIni, GLfloat* normalBufferIni,
		GLint* boneIndexBufferIni, GLfloat* boneWeightBufferIni,
		GLuint* indexPointBufferIni, GLuint* indexTexCoordBufferIni, GLuint* indexNormalBufferIni, int nbFacesInThisMesh,
		int* nbVertexTot, int* nbCoordTexTot, int* nbNormalTot, int nbVertices, int nbTexCoords, int nbNormals, vector <string> mesh_IDs_current_mesh);
	void pg_compute_MeshMotion(int indMeshFile, bool with_motion, MeshAnimationData* animData);
	void pg_drawOneMesh(int indMeshFile);
	void pg_drawOneMesh2(int indMeshFile);
	~MeshData(void) {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ARMATURE POSE AND ANIMATION MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

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

class MeshAnimationData {
public:
	// animation
	double pg_mesh_startAnime;
	int pg_mesh_precedingAnime;
	bool pg_mesh_positiveChange;
	bool pg_mesh_negativeChange;

	// motion
	double pg_mesh_startMotion;
	double pg_mesh_motion_precTime;
	int pg_mesh_precedingMotion;

	// bones & animation
	int pg_nb_bones;
	Bone* pg_tabBones;

	// poses provided in the obj file
	// used as a library for animation which interpolates between some of these poses
	int pg_nb_LibraryPoses;

	// maximal number of interpolated animation poses
	int pg_nb_MaxAnimationPoses;
	// current number of interpolated animation poses given by mesh_anime value
	int pg_nb_CurAnimationPoses;
	// interpolation weight between the pg_nb_CurAnimationPoses
	float *pg_interpolation_weight_AnimationPose;

	// maximal number of interpolated motion poses
	int pg_nb_MaxMotionPoses;
	// current number of interpolated motion poses given by mesh_motion value
	int pg_nb_CurMotionPoses;
	// library of motion poses
	MotionPose *pg_motionPoses;
	// interpolation weight between the pg_nb_CurMotionPoses
	float *pg_interpolation_weight_MotionPose;

	int pg_chosen_mesh_LibraryPose1;
	int pg_chosen_mesh_LibraryPose2;
	int pg_chosen_mesh_LibraryPose3;

	MeshAnimationData(void) {
		// animation
		pg_mesh_startAnime = -1;
		pg_mesh_precedingAnime = -1;
		pg_mesh_positiveChange = false;
		pg_mesh_negativeChange = false;

		// motion 
		pg_mesh_startMotion = -1;
		pg_mesh_motion_precTime = -1;
		pg_mesh_precedingMotion = -1;

		// bones
		pg_nb_bones = 0;
		pg_tabBones = NULL;

		// poses provided in the obj file
		// used as a library for animation which interpolates between some of these poses
		pg_nb_LibraryPoses = 0;

		pg_nb_MaxAnimationPoses = 0;
		pg_nb_CurAnimationPoses = 0;
		pg_interpolation_weight_AnimationPose = NULL;

		pg_nb_MaxMotionPoses = 0;
		pg_nb_CurMotionPoses = 0;
		pg_interpolation_weight_MotionPose = NULL;
		pg_motionPoses = NULL;

		pg_chosen_mesh_LibraryPose1 = -1;
		pg_chosen_mesh_LibraryPose2 = -1;
		pg_chosen_mesh_LibraryPose3 = -1;
	}
	// bone animaton & motion
	void pg_update_BoneAnimation(int indMeshFile);
	void pg_update_MeshMotion(int indMeshFile);
	void pg_compute_BoneTransformations(int indMeshFile);
	void pg_parseOneBoneObj(FILE* file, int level, char* line, char* tag, char* id, int* nbBonesLoc);
	void pg_parseArmatureObj(FILE* file, char* line, char* tag, char* id);
	bool pg_ParseMeshAnimation(FILE* file, int indMeshFile, char* line, char* tag);
	~MeshAnimationData(void) {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// MESHES AND ANIMS
extern vector<MeshData> pg_Meshes[PG_MAX_SCENARIOS];
extern vector<MeshAnimationData> pg_Mesh_Animations[PG_MAX_SCENARIOS];

// mesh lighting
extern GLfloat pg_mesh_light_x;
extern GLfloat pg_mesh_light_y;
extern GLfloat pg_mesh_light_z;

#if defined(var_Caverne_Mesh_Profusion)
void Caverne_Mesh_Profusion_On(int indImage);
void Caverne_Mesh_Profusion_Off(int indImage);
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////


void pg_render_bones(glm::mat4 modelMatrix, int indMeshFile);

void pg_copyLibraryPoseToAnimationPose(int indMeshFile, int chosen_mesh_LibraryPose, int mesh_AnimationPose);

void pg_parseScenario_Meshes(std::ifstream& scenarioFin, int indScenario);
// loads meshes in GPU
void pg_loadAllMeshes(void);

void pg_aliasScript_Mesh(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);

#endif
