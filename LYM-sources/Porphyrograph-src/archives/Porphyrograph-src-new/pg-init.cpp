/*! \file pg-init.cpp
*
*
*     File pg-init.cpp
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

float damping = 0.f;


/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
GLfloat pg_orthoWindowProjMatrix[16];
GLfloat doubleProjMatrix[16];
GLfloat pg_identityViewMatrix[16];
GLfloat pg_identityModelMatrix[16];
GLfloat pg_homographyForTexture[9];

//////////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
// geometry: quads
GLuint pg_vaoID[pg_VAOCount];
GLuint pg_vboID[pg_VBOCount];

// geometrical data
float quadDraw_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float quadDraw_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int quadDraw_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

// geometrical data
float quadMaster_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float quadMaster_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int quadMaster_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

unsigned int quadSensor_vao = 0;
// quad for sensors
float quadSensor_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f
};
float quadSensor_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
	0.0f,         0.0f,
	0.0f,         0.0f
};


// CURVE PARTICLES
GLfloat *pg_Particle_control_points;

GLfloat *pg_Particle_radius;
GLfloat *pg_Particle_colors;
GLfloat *pg_Particle_vertices;
unsigned int *pg_Particle_indices;

//////////////////////////////////////////////////////////////////////
// TEXTURES
//////////////////////////////////////////////////////////////////////
//GLuint pg_Particle_Pos_Texture_texID = 0;
//GLfloat *pg_Particle_Pos_Texture = NULL;

// CURVE PARTICLES TEXTIRE
GLuint curve_particle_2D_texID[PG_MAX_CONFIGURATIONS] = { NULL_ID };
// blurred disk texture
std::vector<GLuint>  blurredDisk_texture_2D_texID[PG_MAX_CONFIGURATIONS];

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
GLuint enumDrawBuffersEntries[16] = {
	GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15
};

/////////////////////////////////////////////
// FBO 
GLuint pg_FBO_Update = 0; // PG_FBO_UPDATE_NBATTACHTS
GLuint pg_FBO_ParticleRendering = 0; // particle rendering
GLuint pg_FBO_ClipArtRendering = 0; // ClipArt rendering
GLuint pg_FBO_ParticleAnimation = 0; // PG_FBO_PARTICLEANIMATION_NBATTACHTS
GLuint pg_FBO_Mixing_capturedFB_prec = 0; //  drawing memory on odd and even frames for echo
// Augmented Reality or mesh rendering bypassing: FBO capture of Master to be displayed on a mesh
GLuint pg_FBO_Master_capturedFB_prec = 0; // master output memory for mapping on mesh

// FBO texture
GLuint pg_FBO_ParticleAnimation_texID[2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS] = { 0 }; // particle animation
GLuint pg_FBO_Particle_render_texID = 0; // particle rendering + stencil
GLuint FBO_ParticleRendering_depthAndStencilBuffer = 0;
GLuint pg_FBO_ClipArt_render_texID = 0; // particle rendering + stencil
GLuint FBO_ClipArt_depthAndStencilBuffer = 0;
GLuint pg_FBO_Update_texID[2 * PG_FBO_UPDATE_NBATTACHTS] = { 0 }; // Update
GLuint pg_FBO_Mixing_capturedFB_prec_texID[2] = { 0 }; // drawing memory on odd and even frames for echo 
// GLuint FBO_CameraFrame_texID = 0; // video preprocessing outcome 
// augmented reality or mesh rendering bypassing
GLuint pg_FBO_Master_capturedFB_prec_texID = 0; // master output memory for mapping on mesh  


//////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

void InterfaceInitializations(void) {
	// PEN COLOR PRESET INTERFACE VARIABLE INITIALIZATION
	int indPreset = 0;
	for(ColorPreset &preset: pg_ColorPresets[pg_current_configuration_rank]) {
		sprintf(AuxString, "/pen_colorPreset_name%d %s",
			indPreset, preset.pen_colorPresets_names.c_str()); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
		indPreset++;
	}
	// interpolation duration initial value (0.f)
	sprintf(AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
	pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");

	// ClipArt GPU INTERFACE VARIABLE INITIALIZATION
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_activeClipArts]
		&& has_NV_path_rendering) {
		for (unsigned int indClipArt = 0; indClipArt < pg_ClipArts[pg_current_configuration_rank].size(); indClipArt++) {
			sprintf(AuxString, "/ClipArt_%d_onOff %d", indClipArt, ((activeClipArts == -1) || (activeClipArts & (1 << (indClipArt - 1))))); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
		}
	}

	// MESH INTERFACE VARIABLE INITIALIZATION
	for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
		sprintf(AuxString, "/Mesh_%d_onOff %d", indMesh + 1, (activeMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
	for (unsigned int indMesh = 0; indMesh < pg_Meshes[pg_current_configuration_rank].size(); indMesh++) {
		sprintf(AuxString, "/Mesh_mobile_%d_onOff %d", indMesh + 1, (mobileMeshes & (1 << (indMesh)))); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}

	if (pg_nb_light_groups[pg_current_configuration_rank] > 0) {
		pg_lightGUI_initialization();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// PERLIN NOISE
//////////////////////////////////////////////////////////////////////////////////////////////
// https://rosettacode.org/wiki/Perlin_noise
double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b) { return a + t * (b - a); }
double grad(int hash, double x, double y, double z) {
	int h = hash & 15;
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
int p[512] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 
	194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 
	23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 
	117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 
	171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 
	231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 
	245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 
	132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 
	164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 
	202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 
	58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 
	154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 
	19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 
	97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 
	81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 
	106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 
	205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

// Perlin noise
double PerlinNoise(double x, double y, double z) {
	int X = (int)floor(x) & 255,
		Y = (int)floor(y) & 255,
		Z = (int)floor(z) & 255;
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	double u = fade(x),
		v = fade(y),
		w = fade(z);
	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
		B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
		grad(p[BA], x - 1, y, z)),
		lerp(u, grad(p[AB], x, y - 1, z),
			grad(p[BB], x - 1, y - 1, z))),
		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
			grad(p[BA + 1], x - 1, y, z - 1)),
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
				grad(p[BB + 1], x - 1, y - 1, z - 1))));
}


//////////////////////////////////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
int printOglError(int no) {

	int    retCode = 0;

	GLenum err;
	if (((err = glGetError()) != GL_NO_ERROR)) {
		printf("*** glError at %d -> %d: %s\n", no,
			err, gluErrorString(err));
		retCode = 1;
	}
	return retCode;
}

void ReportError(char *errorString) {
	//if (pg_csv_log_file) {
	//	fprintf(pg_csv_log_file, "%s\n", errorString);
	//}

	fprintf(stderr, "%s\n", errorString);
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTION FOR TESTING THE CURRENT WORKING DIRECTORY
std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	std::replace(current_working_dir.begin(), current_working_dir.end(), '\\', '/'); // replace all 'x' to 'y'
	return current_working_dir;
}



/////////////////////////////////////////////////////////////////
// TIME
/////////////////////////////////////////////////////////////////

#if defined(_WIN32)
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* pt) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
#endif

double pg_TimeAtApplicationLaunch = -1.;
double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, NULL);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	if (pg_TimeAtApplicationLaunch < 0) {
		pg_TimeAtApplicationLaunch = realtime;
	}
	return ((realtime - pg_TimeAtApplicationLaunch) - InitialRealTime) * time_scale;
}

void MemoryUsage(void) {
#ifndef _WIN32
	// struct rusage {
	//   struct timeval ru_utime; /* user time used */
	//   struct timeval ru_stime; /* system time used */
	//   long   ru_maxrss;        /* maximum resident set size */
	//   long   ru_ixrss;         /* integral shared memory size */
	//   long   ru_idrss;         /* integral unshared data size */
	//   long   ru_isrss;         /* integral unshared stack size */
	//   long   ru_minflt;        /* page reclaims */
	//   long   ru_majflt;        /* page faults */
	//   long   ru_nblend;         /* blends */
	//   long   ru_inblock;       /* block input operations */
	//   long   ru_oublock;       /* block output operations */
	//   long   ru_msgsnd;        /* messages sent */
	//   long   ru_msgrcv;        /* messages received */
	//   long   ru_nsignals;      /* signals received */
	//   long   ru_nvcsw;         /* voluntary context switches */
	//   long   ru_nivcsw;        /* involuntary context switches */
	// };

	struct rusage usage;

	getrusage(RUSAGE_SELF, &usage);

	printf("Memory usage %ld %ld %ld %ld\n", usage.ru_maxrss, usage.ru_ixrss, usage.ru_idrss, usage.ru_isrss);
#endif
}

/////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TWO QUADS USED FOR DISPLAY: DRAWING AND COMPOSITION QUADS
void pg_initGeometry_quads(void) {
	for (int ind = 0; ind < pg_VAOCount; ind++) {
		pg_vaoID[ind] = 0;
	}
	for (int ind = 0; ind < pg_VBOCount; ind++) {
		pg_vboID[ind] = 0;
	}
	glGenVertexArrays(pg_VAOCount, pg_vaoID);
	glGenBuffers(pg_VBOCount, pg_vboID);

	/////////////////////////////////////////////////////////////////////
	// QUAD FOR DRAWING AND COMPOSITION
	// point positions and texture coordinates
	quadDraw_points[1] = (float)window_height;
	quadDraw_points[3] = (float)workingWindow_width;
	quadDraw_points[9] = (float)workingWindow_width;
	quadDraw_points[10] = (float)window_height;

	quadDraw_texCoords[1] = (float)window_height;
	quadDraw_texCoords[2] = (float)workingWindow_width;
	quadDraw_texCoords[6] = (float)workingWindow_width;
	quadDraw_texCoords[7] = (float)window_height;

	quadMaster_points[1] = (float)window_height;
	quadMaster_points[3] = (float)window_width;
	quadMaster_points[9] = (float)window_width;
	quadMaster_points[10] = (float)window_height;
	//printf("quad master rectangle dimensions %d %d\n", int(quadMaster_points[3]), int(quadMaster_points[1]));

	quadMaster_texCoords[1] = (float)window_height;
	quadMaster_texCoords[2] = (float)window_width;
	quadMaster_texCoords[6] = (float)window_width;
	quadMaster_texCoords[7] = (float)window_height;

	printOglError(21);

	/////////////////////////////////////////////////////////////////////
	// single quad for update

	// vertex buffer objects and vertex array
	unsigned int quadDraw_points_vbo = 0;
	glGenBuffers(1, &quadDraw_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 4 * sizeof(float),
		quadDraw_points,
		GL_STATIC_DRAW);
	unsigned int quadDraw_texCoord_vbo = 0;
	glGenBuffers(1, &quadDraw_texCoord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 4 * sizeof(float),
		quadDraw_texCoords,
		GL_STATIC_DRAW);

	// vertex array object
	glBindVertexArray(pg_vaoID[pg_VAOQuad]);
	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuad]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PG_SIZE_QUAD_ARRAY * sizeof(unsigned int),
		quadDraw_indices, GL_STATIC_DRAW);

	printOglError(22);

	/////////////////////////////////////////////////////////////////////
	// double quad for composition

	// vertex buffer objects and vertex array
	unsigned int quadMaster_points_vbo = 0;
	glGenBuffers(1, &quadMaster_points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_points_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 4 * sizeof(float),
		quadMaster_points,
		GL_STATIC_DRAW);
	unsigned int quadMaster_texCoord_vbo = 0;
	glGenBuffers(1, &quadMaster_texCoord_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_texCoord_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		2 * 4 * sizeof(float),
		quadMaster_texCoords,
		GL_STATIC_DRAW);

	// vertex array object
	glBindVertexArray(pg_vaoID[pg_VAOQuadMaster]);
	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	// texture coordinates are location 1
	glBindBuffer(GL_ARRAY_BUFFER, quadMaster_texCoord_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	// vertex indices for indexed rendering 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EABQuadMaster]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, PG_SIZE_QUAD_ARRAY * sizeof(unsigned int),
		quadMaster_indices, GL_STATIC_DRAW);
	printOglError(23);

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_sensor_layout]) {
		/////////////////////////////////////////////////////////////////////
	// QUADS FOR SENSORS
	// point positions and texture coordinates
		quadSensor_points[0] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[1] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[3] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[4] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[6] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[7] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[9] = (float)-PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[10] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[12] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[13] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[15] = (float)PG_SENSOR_GEOMETRY_WIDTH;
		quadSensor_points[16] = (float)-PG_SENSOR_GEOMETRY_WIDTH;

		quadSensor_texCoords[1] = (float)PG_SENSOR_TEXTURE_WIDTH;
		quadSensor_texCoords[2] = (float)PG_SENSOR_TEXTURE_WIDTH;
		quadSensor_texCoords[7] = (float)PG_SENSOR_TEXTURE_WIDTH;
		quadSensor_texCoords[8] = (float)PG_SENSOR_TEXTURE_WIDTH;
		quadSensor_texCoords[9] = (float)PG_SENSOR_TEXTURE_WIDTH;
		quadSensor_texCoords[10] = (float)PG_SENSOR_TEXTURE_WIDTH;

		///////////////////////////////////////////////////////////////////////////////////////
		// vertex buffer objects and vertex array
		///////////////////////////////////////////////////////////////////////////////////////
		unsigned int quadSensor_points_vbo = 0;
		glGenBuffers(1, &quadSensor_points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quadSensor_points_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * 2 * sizeof(float),
			quadSensor_points,
			GL_STATIC_DRAW);
		unsigned int quadSensor_texCoord_vbo = 0;
		glGenBuffers(1, &quadSensor_texCoord_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			2 * 3 * 2 * sizeof(float),
			quadSensor_texCoords,
			GL_STATIC_DRAW);

		quadSensor_vao = 0;
		glGenVertexArrays(1, &quadSensor_vao);
		glBindVertexArray(quadSensor_vao);
		// vertex positions are location 0
		glBindBuffer(GL_ARRAY_BUFFER, quadSensor_points_vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(0);

		// texture coordinates are location 1
		glBindBuffer(GL_ARRAY_BUFFER, quadSensor_texCoord_vbo);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(1);

		// printf("Sensor size  %d\n" , PG_SENSOR_GEOMETRY_WIDTH );

		printOglError(23);

		///////////////////////////////////////////////////////////////////////////////////////
		// sensor layouts
		///////////////////////////////////////////////////////////////////////////////////////
		int indLayout;

#ifndef pg_Project_CAaudio
		int indSens;

		// square grid
		indLayout = 0;
		for (indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			int heightInt = indSens / 4;
			int widthInt = indSens % 4;
			int heightStep = window_height / (4 + 1);
			int widthStep = workingWindow_width / (4 + 1);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = (float)widthStep + widthInt * (float)widthStep;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = (float)heightStep + heightInt * (float)heightStep;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		}

		// circular grid
		indLayout = 1;
		// central sensor
		indSens = 0;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		for (indSens = 1; indSens < 5; indSens++) {
			float radius = window_height / 5.0f;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f + radius * (float)cos(indSens * 2.0 * M_PI / 4.0f);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(indSens * 2.0 * M_PI / 4.0f);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		}
		for (indSens = 5; indSens < PG_NB_SENSORS; indSens++) {
			float radius = window_height / 3.0f;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f + radius * (float)cos(indSens * 2.0 * M_PI / 11.0f);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(indSens * 2.0 * M_PI / 11.0f);
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		}

		// radial grid
		indLayout = 2;
		// central sensor
		indSens = 0;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		for (int indRay = 0; indRay < 5; indRay++) {
			float angle = indRay * 2.0f * (float)M_PI / 5.0f;
			for (indSens = 1 + indRay * 3; indSens < 1 + indRay * 3 + 3; indSens++) {
				float radius = ((indSens - 1) % 3 + 1) * window_height / 10.0f;
				sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f + radius * (float)cos(angle);
				sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f + radius * (float)sin(angle);
				sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
			}
		}

		// conflated in the center
		// will be transformed into a random layout each time it is invoked
		indLayout = 3;
		for (indSens = 0; indSens < PG_NB_SENSORS; indSens++) {
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens] = workingWindow_width / 2.0f;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 1] = window_height / 2.0f;
			sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 3 * indSens + 2] = 0.1f;
		}
#else
		// right
		indLayout = 0;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(7 * workingWindow_width / 8);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(window_height / 2);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

		// center
		indLayout = 1;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(workingWindow_width / 2);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(window_height / 2);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

		// top
		indLayout = 2;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(workingWindow_width / 2);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(7 * window_height / 8);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

		// corner
		indLayout = 3;
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS] = (float)(7 * workingWindow_width / 8);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 1] = (float)(7 * window_height / 8);
		sensorLayouts[indLayout * 3 * PG_NB_SENSORS + 2] = 0.1f;

#endif


		///////////////////////////////////////////////////////////////////////////////////////
		// sensor activations
		///////////////////////////////////////////////////////////////////////////////////////
		int indActivation;

#ifndef pg_Project_CAaudio
		// no activation
		indActivation = 0;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}

		// corners activation
		indActivation = 1;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}
		sensorActivations[indActivation * PG_NB_SENSORS + 0] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 3] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 12] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 15] = true;

		// central square activation
		indActivation = 2;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}
		sensorActivations[indActivation * PG_NB_SENSORS + 5] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 6] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 9] = true;
		sensorActivations[indActivation * PG_NB_SENSORS + 10] = true;

		// every second activation
		indActivation = 3;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			if (indSensor % 2 == 0) {
				sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
			}
			else {
				sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
			}
		}

		// full activation
		indActivation = 4;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
		}

		// no activation and activation of an additional sensor each 10 seconds
		indActivation = 5;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}
#else
		// full activation
		indActivation = 0;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = true;
		}

		// no activation 
		indActivation = 1;
		for (int indSensor = 0; indSensor < PG_NB_SENSORS; indSensor++) {
			sensorActivations[indActivation * PG_NB_SENSORS + indSensor] = false;
		}
#endif

		///////////////////////////////////////////////////////////////////////////////////////
		// sample setup
		///////////////////////////////////////////////////////////////////////////////////////
		// sample choice
		sensor_sample_setUp_interpolation();
	}


	/////////////////////////////////////////////////////////////////////
	// PARTICLES TO BE TESSELATED
	// initializes the arrays that contains the positions and the indices of the particles
	pg_initParticlePosition_Texture();

	if (PG_PARTICLE_TYPE == 2) {
		// vertex buffer objects and vertex array
		glBindVertexArray(pg_vaoID[pg_VAOParticle]);
		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 3 * sizeof(float), pg_Particle_control_points, GL_STATIC_DRAW);
		// radius
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * 1 * sizeof(float), pg_Particle_radius, GL_STATIC_DRAW);
		// color
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_colors, GL_STATIC_DRAW);
	}
	else {
		// vertex buffer objects and vertex array
		glBindVertexArray(pg_vaoID[pg_VAOParticle]);
		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_vertices, GL_STATIC_DRAW);
		// radius
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * 1 * sizeof(float), pg_Particle_radius, GL_STATIC_DRAW);
		// color
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
		glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(float), pg_Particle_colors, GL_STATIC_DRAW);
	}

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticle]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);

	if (PG_PARTICLE_TYPE == 2) {
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(1);
		// color is location 2
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(2);

		// vertex indices for indexed rendering 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * sizeof(unsigned int),
			pg_Particle_indices, GL_STATIC_DRAW);
	}
	else {
		// radius is location 1
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOpartRadius]);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(1);
		// color is location 2
		glBindBuffer(GL_ARRAY_BUFFER, pg_vboID[pg_VBOParticleColors]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
		glEnableVertexAttribArray(2);

		// vertex indices for indexed rendering 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pg_vboID[pg_EAOParticle]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_particles * sizeof(unsigned int),
			pg_Particle_indices, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printOglError(32);

	printf("Geometry initialized. Working size: %dx%d & full size: %dx%d\n", workingWindow_width, window_height,
		window_width, window_height);
}




/////////////////////////////////////////////////////////////////
// FBO TEXTURES INITIALIZATION
/////////////////////////////////////////////////////////////////
bool pg_initFBOTextures(GLuint *textureID, int nb_attachments, bool with_stencil_andOr_depth, GLuint *depthAndStencilBuffer) {
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int indAtt = 0; indAtt < nb_attachments; indAtt++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, textureID[indAtt]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F, window_width, window_height);
		glTexStorage2D(GL_TEXTURE_RECTANGLE, 1, GL_RGBA32F, workingWindow_width, window_height);
	}
	if (with_stencil_andOr_depth) {
		glBindRenderbuffer(GL_RENDERBUFFER, *depthAndStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, workingWindow_width, window_height);
	}

	return true;
}

bool pg_bindFBOTextures(GLuint currentFBO, GLuint *currentTexutreID, int nb_attachments, bool with_stencil_andOr_depth, GLuint currentStencilOrDepthBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
	for (int indAtt = 0; indAtt < nb_attachments; indAtt++) {
		// printf("FBO size %d %d \n" , workingGLuint currentFBO, intWindow_width , window_height );
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + indAtt,
			GL_TEXTURE_RECTANGLE, currentTexutreID[indAtt], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			sprintf(ErrorStr, "Error: Binding RECT FBO texture No %d ID %d (error %d)!", indAtt, currentTexutreID[indAtt], glCheckFramebufferStatus(GL_FRAMEBUFFER)); ReportError(ErrorStr); throw 336;
		}
		glDrawBuffers(nb_attachments, enumDrawBuffersEntries);
	}
	if (with_stencil_andOr_depth) {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, currentStencilOrDepthBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, currentStencilOrDepthBuffer);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		sprintf(ErrorStr, "Error: FBO status not complete (error %d)!", glCheckFramebufferStatus(GL_FRAMEBUFFER)); ReportError(ErrorStr); throw 336;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
bool pg_initFBOTextureImagesAndRendering(void) {
	int maxbuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	int maxDrawBuf;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuf);

	if (maxbuffers < PG_FBO_UPDATE_NBATTACHTS) {
		sprintf(ErrorStr, "Error: Maximal attachment (%d) -> %d required!", maxbuffers, PG_FBO_UPDATE_NBATTACHTS); ReportError(ErrorStr); // throw 336;
	}
	if (maxDrawBuf < PG_FBO_UPDATE_NBATTACHTS) {
		sprintf(ErrorStr, "Error: Maximal draw buffers (%d) -> %d required!", maxbuffers, PG_FBO_UPDATE_NBATTACHTS); ReportError(ErrorStr); // throw 336;
	}

	/*
	// FBO: camera frame after initial processing
	glGenFramebuffers(1, &FBO_CameraFrame);  // drawing memory on odd and even frames for echo
	if (!FBO_CameraFrame_texID) {
		glGenTextures(1, &FBO_CameraFrame_texID);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_CameraFrame);
	pg_initFBOTextures(&FBO_CameraFrame_texID, 1, false);
	glDrawBuffers(1, enumDrawBuffersEntries);
	printOglError(343);
	*/

	// FBO: multi-attachment for update 
	// initializations to NULL
		// 2 = FBO ping-pong size for Update shader FBOs
	for (int indAttachedFB = 0; indAttachedFB < 2 * PG_FBO_UPDATE_NBATTACHTS; indAttachedFB++) {
		pg_FBO_Update_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * PG_FBO_UPDATE_NBATTACHTS, pg_FBO_Update_texID);
	printf("FBO Update size %d %d attachments %d (configs %d)\n", workingWindow_width, window_height, PG_FBO_UPDATE_NBATTACHTS, pg_NbConfigurations);
	pg_initFBOTextures(pg_FBO_Update_texID, 2 * PG_FBO_UPDATE_NBATTACHTS, false, NULL);

	glGenFramebuffers(1, &pg_FBO_Update);
	// ping-pong FBO texture binding, changes each frame

	printOglError(341);

	// FBO: multi-attachment for particle animation 
	// 2 = FBO ping-pong size for ParticleAnimation shader FBOs
	// initializations to NULL
	for (int indAttachedFB = 0; indAttachedFB < 2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS; indAttachedFB++) {
		pg_FBO_ParticleAnimation_texID[indAttachedFB] = 0;
	}
	glGenTextures(2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS, pg_FBO_ParticleAnimation_texID);
	printf("FBO Particle animation size %d %d attachments %d\n", workingWindow_width, window_height, PG_FBO_PARTICLEANIMATION_NBATTACHTS);
	pg_initFBOTextures(pg_FBO_ParticleAnimation_texID, 2 * PG_FBO_PARTICLEANIMATION_NBATTACHTS, false, NULL);

	glGenFramebuffers(1, &pg_FBO_ParticleAnimation);
	// ping-pong FBO texture binding, changes each frame

	printOglError(341);

	// FBO: ClipArt GPU drawing output 
	glGenTextures(1, &pg_FBO_ClipArt_render_texID);
	glGenRenderbuffers(1, &FBO_ClipArt_depthAndStencilBuffer);
	printf("FBO ClipArt GPU rendering size %d %d\n", workingWindow_width, window_height);
	pg_initFBOTextures(&pg_FBO_ClipArt_render_texID, 1, true, &FBO_ClipArt_depthAndStencilBuffer);

	glGenFramebuffers(1, &pg_FBO_ClipArtRendering);  // drawing memory on odd and even frames for echo 
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_ClipArtRendering, &pg_FBO_ClipArt_render_texID, 1, true, FBO_ClipArt_depthAndStencilBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	printOglError(344);


	// FBO: particle drawing output 
	glGenTextures(1, &pg_FBO_Particle_render_texID);
	glGenRenderbuffers(1, &FBO_ParticleRendering_depthAndStencilBuffer);
	printf("FBO Particle rendering size %d %d\n", workingWindow_width, window_height);
	pg_initFBOTextures(&pg_FBO_Particle_render_texID, 1, true, &FBO_ParticleRendering_depthAndStencilBuffer);

	glGenFramebuffers(1, &pg_FBO_ParticleRendering);  // drawing memory on odd and even frames for echo 
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_ParticleRendering, &pg_FBO_Particle_render_texID, 1, true, FBO_ParticleRendering_depthAndStencilBuffer);

	printOglError(344);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// FBO: composition output for echo
	// 2 = FBO ping-pong size for echo FBOs
	// initializations to NULL
	for (int indFB = 0; indFB < 2; indFB++) {
		pg_FBO_Mixing_capturedFB_prec_texID[indFB] = 0;
	}
	glGenTextures(2, pg_FBO_Mixing_capturedFB_prec_texID);
	printf("FBO Mixing animation size %d %d attachments %d\n", workingWindow_width, window_height, 2);
	pg_initFBOTextures(pg_FBO_Mixing_capturedFB_prec_texID, 2, false, 0);

	glGenFramebuffers(1, &pg_FBO_Mixing_capturedFB_prec);  // drawing memory on odd and even frames for echo 
	// ping-pong FBO texture binding, changes each frame

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Augmented Reality or bypassing mesh rendering: FBO capture of Master to be displayed on a mesh
	pg_FBO_Master_capturedFB_prec_texID = 0;
	glGenTextures(1, &pg_FBO_Master_capturedFB_prec_texID);
	printf("FBO Master size %d %d attachments %d\n", workingWindow_width, window_height, 1);
	pg_initFBOTextures(&pg_FBO_Master_capturedFB_prec_texID, 1, false, 0);

	glGenFramebuffers(1, &pg_FBO_Master_capturedFB_prec);  // master output memory for mapping on mesh of bypassing mesh rendering
	// texture binding is constant and made once for all
	pg_bindFBOTextures(pg_FBO_Master_capturedFB_prec, &pg_FBO_Master_capturedFB_prec_texID, 1, false, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	printOglError(342);
	return true;
}

/////////////////////////////////////////////////////////////////
// MATRIX INITIALIZATION
/////////////////////////////////////////////////////////////////
void pg_initRenderingMatrices(void) {
	memset((char *)pg_identityViewMatrix, 0, 16 * sizeof(float));
	memset((char *)pg_identityModelMatrix, 0, 16 * sizeof(float));
	memset((char *)modelMatrixSensor, 0, 16 * sizeof(float));
	pg_identityViewMatrix[0] = 1.0f;
	pg_identityViewMatrix[5] = 1.0f;
	pg_identityViewMatrix[10] = 1.0f;
	pg_identityViewMatrix[15] = 1.0f;
	pg_identityModelMatrix[0] = 1.0f;
	pg_identityModelMatrix[5] = 1.0f;
	pg_identityModelMatrix[10] = 1.0f;
	pg_identityModelMatrix[15] = 1.0f;
	modelMatrixSensor[0] = 1.0f;
	modelMatrixSensor[5] = 1.0f;
	modelMatrixSensor[10] = 1.0f;
	modelMatrixSensor[15] = 1.0f;

	// ORTHO
	float l = 0.0f;
	float r = (float)workingWindow_width;
	float b = 0.0f;
	float t = (float)window_height;
	float n = -1.0f;
	float f = 1.0f;
	GLfloat mat[] = {
		(GLfloat)(2.0f / (r - l)), 0.0, 0.0, 0.0,
		0.0, (GLfloat)(2.0f / (t - b)), 0.0, 0.0,
		0.0, 0.0, (GLfloat)(2.0f / (f - n)), 0.0,
		(GLfloat)(-(r + l) / (r - l)), (GLfloat)(-(t + b) / (t - b)), (GLfloat)(-(f + n) / (f - n)), 1.0f };
	memcpy((char *)pg_orthoWindowProjMatrix, mat, 16 * sizeof(float));
	// printf("Orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
	memcpy((char*)pg_homographyForTexture, mat, 9 * sizeof(float));

	r = (float)window_width;
	GLfloat mat2[] = {
		(GLfloat)(2.0f / (r - l)), 0.0, 0.0, 0.0,
		0.0, (GLfloat)(2.0f / (t - b)), 0.0, 0.0,
		0.0, 0.0, (GLfloat)(2.0f / (f - n)), 0.0,
		(GLfloat)(-(r + l) / (r - l)), (GLfloat)(-(t + b) / (t - b)), (GLfloat)(-(f + n) / (f - n)), 1.0f };
	memcpy((char *)doubleProjMatrix, mat2, 16 * sizeof(float));
	// printf("Double width orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
}

/////////////////////////////////////////////////
// PARTICLES INITIALIZATION

void pg_initParticlePosition_Texture( void ) {
	if (PG_PARTICLE_TYPE == 2) {
		// the control point position contain column and row of the control point coordinates
	// inside the texture of initial positions so that the coordinates contained in this
	// texture can be retrieved in the vertex shader
		pg_Particle_control_points = new float[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 3];
		pg_Particle_indices = new unsigned int[nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1)];

		pg_Particle_radius = new float[nb_particles * 1];
		pg_Particle_colors = new float[nb_particles * 3];

		// the used width is a multiple of (PG_PARTICLE_CURVE_DEGREE + 1) so that each set of control
		// point coordinates is on a single line
		int width_used = workingWindow_width - workingWindow_width % (PG_PARTICLE_CURVE_DEGREE + 1);

		for (int indParticle = 0; indParticle < nb_particles; indParticle++) {
			int ind_index = indParticle * (PG_PARTICLE_CURVE_DEGREE + 1);
			int ind_radius = indParticle;
			int ind_color = indParticle * 3;
			int ind_point = ind_index * 3;

			// col 
			pg_Particle_control_points[ind_point + 0] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) % width_used);
			// row
			pg_Particle_control_points[ind_point + 1] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + 0) / width_used);
			pg_Particle_control_points[ind_point + 2] = 0.f;

			//// u 
			//pg_Particle_texCoords[ind_point + 0] = 0.f;
			//// v
			//pg_Particle_texCoords[ind_point + 1] = 0.f;

			// radius
			pg_Particle_radius[ind_radius + 0] = 1.f;

			// color r g b a
			pg_Particle_colors[ind_color + 0] = 1.f;
			pg_Particle_colors[ind_color + 1] = 1.f;
			pg_Particle_colors[ind_color + 2] = 1.f;

			pg_Particle_indices[ind_index] = ind_index;

			ind_point += 3;
			ind_index++;
			for (int indControlPoint = 0; indControlPoint < PG_PARTICLE_CURVE_DEGREE; indControlPoint++) {
				// col 
				pg_Particle_control_points[ind_point + 0] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + indControlPoint + 1) % width_used);
				// row
				pg_Particle_control_points[ind_point + 1] = float((indParticle * (PG_PARTICLE_CURVE_DEGREE + 1) + indControlPoint + 1) / width_used);
				pg_Particle_control_points[ind_point + 2] = 0.f;

				//// u 
				//if (indControlPoint == 0) {
				//	pg_Particle_texCoords[ind_point + 0] = 0.f;
				//}
				//else {
				//	pg_Particle_texCoords[ind_point + 0] = 1.f;
				//}
				//// v
				//pg_Particle_texCoords[ind_point + 1] = 0.f;

				pg_Particle_indices[ind_index] = ind_index;

				ind_point += 3;
				ind_index++;
			}
		}
	}
	else {
		// the vertices position contain column and row of the vertices coordinates
		// inside the texture of initial positions so that the coordinates contained in this
		// texture can be retrieved in the vertex shader
		pg_Particle_vertices = new float[nb_particles * 3];
		pg_Particle_radius = new float[nb_particles * 1];
		pg_Particle_colors = new float[nb_particles * 3];
		pg_Particle_indices = new unsigned int[nb_particles];

		for (int indParticle = 0; indParticle < nb_particles; indParticle++) {
			int ind_point = indParticle * 3;
			int ind_radius = indParticle;
			int ind_color = indParticle * 3;
			int ind_index = indParticle;

			// col 
			pg_Particle_vertices[ind_point + 0] = float(indParticle % workingWindow_width);
			// row
			pg_Particle_vertices[ind_point + 1] = float(indParticle / workingWindow_width);
			pg_Particle_vertices[ind_point + 2] = 0.f;

			// radius
			pg_Particle_radius[ind_radius + 0] = 1.f;

			// color r g b a
			pg_Particle_colors[ind_color + 0] = 1.f;
			pg_Particle_colors[ind_color + 1] = 1.f;
			pg_Particle_colors[ind_color + 2] = 1.f;

			// indices
			pg_Particle_indices[ind_index] = ind_index;
		}
	}
}


