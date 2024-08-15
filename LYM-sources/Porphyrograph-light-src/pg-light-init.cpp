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

#include "pg-light-all_include.h"

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shaders
//////////////////////////////////////////////////////////////////////
GLfloat pg_orthoWindowProjMatrix[16];
GLfloat pg_doubleProjMatrix[16];
GLfloat pg_identityViewMatrix[16];
GLfloat pg_identityModelMatrix[16];
GLfloat pg_homographyForTexture[9];

// resend all values (fx after interface crash)
bool pg_resend_all_variables = true;

// resend all light values (fx after interface crash)
bool pg_resend_all_light_variables = true;


//////////////////////////////////////////////////////////////////////
// GEOMETRY
//////////////////////////////////////////////////////////////////////
// geometry: quads
GLuint pg_vaoID[pg_enum_VAOCount];
GLuint pg_vboID[pg_enum_VBOCount];

// geometrical data
float pg_quadDraw_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float pg_quadDraw_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int pg_quadDraw_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

// geometrical data
float pg_quadMaster_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
};
float pg_quadMaster_texCoords[] = {
	0.0f,         0.0f, //1st triangle
	0.0f,         0.0f,
	0.0f,         0.0f,
	0.0f,         0.0f, //2nd triangle
};
unsigned int pg_quadMaster_indices[PG_SIZE_QUAD_ARRAY] = { 2, 1, 0, 3 };

unsigned int pg_quadSensor_vao = 0;
// quad for sensors
float pg_quadSensor_points[] = {
	0.0f,         0.0f,  0.0f, //1st triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f, //2nd triangle
	0.0f,         0.0f,  0.0f,
	0.0f,         0.0f,  0.0f
};
float pg_quadSensor_texCoords[] = {
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
GLuint pg_curve_particle_2D_texID[PG_MAX_SCENARIOS] = { NULL_ID };
// blurred disk texture
std::vector<GLuint>  pg_blurredDisk_texture_2D_texID[PG_MAX_SCENARIOS];

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
GLuint pg_FBO_Update = 0; // pg_enum_FBO_Update_nbAttachts
GLuint pg_FBO_ParticleRendering = 0; // particle rendering
GLuint pg_FBO_ClipArtRendering = 0; // ClipArt rendering
GLuint pg_FBO_ParticleAnimation = 0; // pg_enum_FBO_ParticleAnimation_nbAttachts
GLuint pg_FBO_Mixing_capturedFB_prec = 0; //  drawing memory on odd and even frames for echo
// Augmented Reality or mesh rendering bypassing: FBO capture of Master to be displayed on a mesh
GLuint pg_FBO_Master_capturedFB_prec = 0; // master output memory for mapping on mesh

// FBO texture
GLuint pg_FBO_ParticleAnimation_texID[2 * pg_enum_FBO_ParticleAnimation_nbAttachts] = { 0 }; // particle animation
GLuint pg_FBO_Particle_render_texID = 0; // particle rendering + stencil
GLuint pg_FBO_ParticleRendering_depthAndStencilBuffer = 0;
GLuint pg_FBO_ClipArt_render_texID = 0; // particle rendering + stencil
GLuint FBO_ClipArt_depthAndStencilBuffer = 0;
GLuint pg_FBO_Update_texID[2 * pg_enum_FBO_Update_nbAttachts] = { 0 }; // Update
GLuint pg_FBO_Mixing_capturedFB_prec_texID[2] = { 0 }; // drawing memory on odd and even frames for echo 
// GLuint FBO_CameraFrame_texID = 0; // video preprocessing outcome 
// augmented reality or mesh rendering bypassing
GLuint pg_FBO_Master_capturedFB_prec_texID = 0; // master output memory for mapping on mesh  


//////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

void pg_InterfaceInitializations(void) {
	// PEN COLOR PRESET INTERFACE VARIABLE INITIALIZATION
	int indPreset = 0;
	for(ColorPreset &preset: pg_ColorPresets[pg_ind_scenario]) {
		sprintf(pg_AuxString, "/pen_colorPreset_name%d %s",
			indPreset, preset.pen_colorPresets_names.c_str()); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
		indPreset++;
	}
	// interpolation duration initial value (0.f)
	sprintf(pg_AuxString, "/interpolation_duration %.2f", pg_SceneInterpolationDuration);
	pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
		if (pg_nb_light_groups[pg_ind_scenario] > 0) {
			pg_lightGUI_initialization();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// PERLIN NOISE
//////////////////////////////////////////////////////////////////////////////////////////////
// https://rosettacode.org/wiki/Perlin_noise
double pg_fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
double pg_lerp(double t, double a, double b) { return a + t * (b - a); }
double pg_grad(int hash, double x, double y, double z) {
	int h = hash & 15;
	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

// Perlin noise
double pg_PerlinNoise(double x, double y, double z) {
	int X = (int)floor(x) & 255,
		Y = (int)floor(y) & 255,
		Z = (int)floor(z) & 255;
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
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	double u = pg_fade(x),
		v = pg_fade(y),
		w = pg_fade(z);
	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
		B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return pg_lerp(w, pg_lerp(v, pg_lerp(u, pg_grad(p[AA], x, y, z),
		pg_grad(p[BA], x - 1, y, z)),
		pg_lerp(u, pg_grad(p[AB], x, y - 1, z),
			pg_grad(p[BB], x - 1, y - 1, z))),
		pg_lerp(v, pg_lerp(u, pg_grad(p[AA + 1], x, y, z - 1),
			pg_grad(p[BA + 1], x - 1, y, z - 1)),
			pg_lerp(u, pg_grad(p[AB + 1], x, y - 1, z - 1),
				pg_grad(p[BB + 1], x - 1, y - 1, z - 1))));
}


//////////////////////////////////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
//////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
int pg_printOglError(int no) {

	int    retCode = 0;

	GLenum err;
	if (((err = glGetError()) != GL_NO_ERROR)) {
		printf("*** glError at %d -> %d: %s\n", no,
			err, gluErrorString(err));
		retCode = 1;
	}
	return retCode;
}

void pg_ReportError(char *errorString) {
	//if (pg_csv_log_file) {
	//	fprintf(pg_csv_log_file, "%s\n", errorString);
	//}

	fprintf(stderr, "%s\n", errorString);
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTION FOR TESTING THE CURRENT WORKING DIRECTORY
std::string pg_GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	std::replace(current_working_dir.begin(), current_working_dir.end(), '\\', '/'); // replace all 'x' to 'y'
	return current_working_dir;
}



/////////////////////////////////////////////////////////////////
// TIME
/////////////////////////////////////////////////////////////////

#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp) {
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
double pg_RealTime(void) {
	struct timeval time;
#ifdef _WIN32
	gettimeofday(&time);
#else
	gettimeofday(&time, NULL);
#endif
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	if (pg_TimeAtApplicationLaunch < 0) {
		pg_TimeAtApplicationLaunch = realtime;
	}
	return ((realtime - pg_TimeAtApplicationLaunch) - pg_InitialRealTime) * time_scale;
}

void pg_MemoryUsage(void) {
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


