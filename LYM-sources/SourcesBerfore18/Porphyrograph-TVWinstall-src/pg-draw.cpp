////////////////////////////////////////////////////////////////////
//
//
//
//     File pg-draw.cpp
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; TrackDisplay even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

/////////////////////////////////////////////////////////////////
// config variables
float mouse_x;
float mouse_y;
float mouse_x_prev;
float mouse_y_prev;
float tracks_x[PG_NB_TRACKS];
float tracks_y[PG_NB_TRACKS];
float tracks_x_prev[PG_NB_TRACKS];
float tracks_y_prev[PG_NB_TRACKS];
float tracks_Color_r[PG_NB_TRACKS];
float tracks_Color_g[PG_NB_TRACKS];
float tracks_Color_b[PG_NB_TRACKS];
float tracks_Color_a[PG_NB_TRACKS];
int tracks_BrushID[PG_NB_TRACKS];
float tracks_RadiusX[PG_NB_TRACKS];
float tracks_RadiusY[PG_NB_TRACKS];

bool track_replay_0 = false;
bool track_replay_1 = false;
bool track_replay_2 = false;

bool source_track_record_0 = false;
bool source_track_record_1 = false;
bool source_track_record_2 = false;

// on/off automatic image output in case of live performance
bool is_automatic_snapshots = true;

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shader
GLfloat projMatrix[16];
GLfloat doubleProjMatrix[16];
GLfloat viewMatrix[16];
GLfloat modelMatrix[16];
#ifdef PG_TRACK_DISPLAY
GLfloat projMatrixDisplayTracks[16];
GLfloat modelMatrixDisplayTracks[16];
#endif

////////////////////////////////////////
// hashtags
std::string* HashtagList;
int* HashtagColorList;
int NbHashtags = 0;
int NbCharHashtagPrec = -1;
int NbCharHashtagCur = -1;

string Hashtag;
int IndHashtag = -1;
double HashtagSwapInitialTime = 0;

////////////////////////////////////////
// large messages displayed on the bottom of the screen
string DisplayText1;
float DisplayText1Alpha = 0.0f;
float LengthDisplayText1 = 0.0f;
int IndDisplayText1 = -1;

string DisplayText2;
float DisplayText2Alpha = 0.0f;
float LengthDisplayText2 = 0.0f;
int IndDisplayText2 = -1;

bool DisplayText1Front = true;
double DisplayTextSwapInitialTime = 0;

// large messages
std::string* DisplayTextList;
int* DisplayTextFirstInChapter;
int NbDisplayTexts = 0;

// message chapters
int TextChapter = -1;
int NbTextChapters = 0;

/////////////////////////////////////////////////////////////////
// Image distribution
float centralImage = 0;

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
GLuint pg_screen_msg_offset_texID = NULL_ID; // PG_NB_ATTACHMENTS=1
GLubyte *pg_screen_msg_offset_texture = NULL;

GLuint pg_displayMsg1_offset_texID = NULL_ID;
GLubyte *pg_displayMsg1_offset_texture = NULL;
GLuint pg_displayMsg2_offset_texID = NULL_ID;
GLubyte *pg_displayMsg2_offset_texture = NULL;

GLuint pg_hashtag_offset_texID = NULL_ID;
GLubyte *pg_hashtag_offset_texture = NULL;
int hashtag_pixel_length = 1920;

GLuint pg_tracks_Pos_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_Pos_Texture[PG_NB_TRACKS];
GLuint pg_tracks_Col_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_Col_Texture[PG_NB_TRACKS];
GLuint pg_tracks_RadBrushRendmode_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_RadBrushRendmode_Texture[PG_NB_TRACKS];

GLuint pg_tracks_Pos_target_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_Pos_target_Texture[PG_NB_TRACKS];
GLuint pg_tracks_Col_target_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_Col_target_Texture[PG_NB_TRACKS];
GLuint pg_tracks_RadBrushRendmode_target_Texture_texID[PG_NB_TRACKS];
GLfloat *pg_tracks_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

GLuint Screen_Font_texture_texID = NULL_ID;
GLuint Display_Font_texture_texID = NULL_ID;
GLuint Hashtag_Font_texture_texID = NULL_ID;

GLuint Pen_texture_3D_texID = NULL_ID;
GLuint Particle_acceleration_texture_3D = NULL_ID;

#ifdef PG_TRACK_DISPLAY
GLuint trackBrush_texture_2D_texID = NULL_ID;
#endif

#ifdef PG_VIDEO_ACTIVE
// video texture ID, image and camera
GLuint pg_video_texture_texID = NULL_ID;
GLuint pg_video_currentBG_texture_texID = NULL_ID;
GLuint pg_video_initialBG_texture_texID = NULL_ID;
VideoCapture pg_input_camera;
Mat pg_video_frame;
#endif

const char *TextureEncodingString[EmptyTextureEncoding + 1] = { "jpeg", "png", "pnga", "png_gray", "pnga_gray", "rgb", "raw", "emptyimagetype" };

////////////////////////////////////////
// geometry: quads
int nbFaces = 2; // 6 squares made of 2 triangles
unsigned int quadDraw_vao = 0;
unsigned int quadComposition_vao = 0;

// geometrical data
float quadDraw_points[] = {
  0.0f,         0.0f,  0.0f, //1st triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f, //2nd triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f
};
float quadDraw_texCoords[] = {
  0.0f,         0.0f, //1st triangle
  0.0f,         0.0f,
  0.0f,         0.0f,
  0.0f,         0.0f, //2nd triangle
  0.0f,         0.0f,
  0.0f,         0.0f
};

// geometrical data
float quadComposition_points[] = {
  0.0f,         0.0f,  0.0f, //1st triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f, //2nd triangle
  0.0f,         0.0f,  0.0f,
  0.0f,         0.0f,  0.0f
};
float quadComposition_texCoords[] = {
  0.0f,         0.0f, //1st triangle
  0.0f,         0.0f,
  0.0f,         0.0f,
  0.0f,         0.0f, //2nd triangle
  0.0f,         0.0f,
  0.0f,         0.0f
};

////////////////////////////////////////
// geometry: track lines
unsigned int line_tracks_vao[PG_NB_TRACKS];
unsigned int line_tracks_target_vao[PG_NB_TRACKS];

float *line_tracks_points[PG_NB_TRACKS];
float *line_tracks_target_points[PG_NB_TRACKS];


//////////////////////////////////////////////////////////////////////
// TEXT
//////////////////////////////////////////////////////////////////////

GLbyte stb__arial_15_usascii_x[95]={ 0,1,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,-1,0,0,1,1,1,0,1,1,0,0,
0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,
-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0, };
GLubyte stb__arial_15_usascii_y[95]={ 12,2,2,2,1,2,2,2,2,2,2,4,10,7,
10,2,2,2,2,2,2,2,2,2,2,2,5,5,4,5,4,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,13,2,4,2,4,2,4,2,4,2,2,
2,2,2,4,4,4,4,4,4,4,2,5,5,5,5,5,5,2,2,2,6, };
GLubyte stb__arial_15_usascii_w[95]={ 0,2,5,8,7,12,9,2,4,4,5,8,2,5,
2,4,7,5,7,7,7,7,7,7,7,7,2,2,8,8,8,7,14,10,9,10,8,8,7,10,8,2,6,9,
7,11,8,10,8,10,9,9,8,8,9,13,9,9,8,4,4,3,6,9,4,7,7,7,7,7,5,7,7,3,
4,7,3,11,7,7,7,7,5,7,4,7,7,10,7,7,7,5,2,5,8, };
GLubyte stb__arial_15_usascii_h[95]={ 0,10,4,11,13,11,11,4,13,13,5,7,4,3,
2,11,11,10,10,11,10,11,11,10,11,11,7,9,7,5,7,10,13,10,10,11,10,10,10,11,10,10,11,10,
10,10,10,11,10,11,10,11,10,11,10,10,10,10,10,13,11,13,6,2,3,9,11,9,11,9,10,11,10,10,
13,10,10,8,8,9,11,11,8,9,11,8,7,7,7,10,7,13,13,13,3, };
GLubyte stb__arial_15_usascii_s[95]={ 127,68,60,79,1,104,117,66,58,29,54,
1,69,72,102,1,25,12,105,67,97,83,59,43,109,33,100,123,21,45,103,
18,34,1,113,14,88,79,71,10,59,1,102,33,25,13,4,68,117,91,103,
41,86,1,72,58,48,38,29,49,117,54,38,92,78,42,6,34,75,58,122,
51,21,82,24,95,113,66,92,50,96,88,78,26,63,84,30,10,112,51,120,
18,15,9,83, };
GLubyte stb__arial_15_usascii_t[95]={ 1,39,61,1,1,1,1,61,1,1,61,
61,61,61,61,15,15,50,39,15,39,15,15,39,15,15,50,39,61,61,50,
50,1,50,39,15,39,39,39,27,39,39,15,39,39,39,39,1,27,15,27,
15,27,27,27,27,27,27,27,1,15,1,61,61,61,50,15,50,15,50,15,
15,27,27,1,27,27,50,50,50,1,1,50,50,1,50,61,61,50,39,50,
1,1,1,61, };
GLubyte stb__arial_15_usascii_a[95]={ 60,60,76,119,119,191,143,41,
72,72,84,125,60,72,60,60,119,119,119,119,119,119,119,119,
119,119,60,60,125,125,125,119,218,143,143,155,155,143,131,167,
155,60,107,143,119,179,155,167,143,167,155,143,131,155,143,203,
143,143,131,60,60,60,101,119,72,119,119,107,119,119,60,119,
119,48,48,107,48,179,119,119,119,119,72,107,60,119,107,155,
107,107,107,72,56,72,125, };

GLbyte stb__arial_25_usascii_x[95]={ 0,1,1,0,0,1,0,0,1,1,0,1,1,0,
2,0,0,2,0,0,0,0,0,1,0,0,2,1,1,1,1,0,1,-1,1,1,1,1,1,1,1,2,0,1,
1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,1,0,0,0,-1,0,0,1,0,0,0,0,0,1,1,
-2,1,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,0,2,0,0, };
GLbyte stb__arial_25_usascii_y[95]={ 20,3,3,3,2,3,3,3,3,3,3,6,17,13,
17,3,3,3,3,3,3,4,3,4,3,3,8,8,6,8,6,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,23,3,8,3,8,3,8,3,8,3,3,
3,3,3,8,8,8,8,8,8,8,4,8,8,8,8,8,8,3,3,3,10, };
// modified from original data by assigning a width of 8 to space char
// first value in this array
GLubyte stb__arial_25_usascii_w[95]={ 8,4,6,13,12,18,15,4,6,6,8,11,4,7,
3,7,12,7,12,12,12,12,12,11,12,12,3,4,11,11,11,12,21,16,13,15,14,13,12,16,14,3,10,14,
11,16,14,16,13,17,15,13,14,14,15,21,15,15,14,5,7,5,10,14,6,12,11,11,11,12,7,11,10,3,
6,11,3,17,10,12,11,11,7,11,7,10,11,16,12,11,11,7,2,7,13, };
GLubyte stb__arial_25_usascii_h[95]={ 0,17,7,18,21,18,18,7,22,22,8,12,7,3,
3,18,18,17,17,18,17,17,18,16,18,18,12,16,12,8,12,17,22,17,17,18,17,17,17,18,17,17,18,17,
17,17,17,18,17,19,17,18,17,18,17,17,17,17,17,22,18,22,10,2,4,13,18,13,18,13,17,17,17,17,
22,17,17,12,12,13,17,17,12,13,17,13,12,12,12,17,12,22,22,22,4, };
GLubyte stb__arial_25_usascii_s[95]={ 255,58,157,123,75,162,181,152,27,1,143,
169,251,185,251,197,55,134,155,137,210,1,205,34,231,218,205,29,193,131,181,
142,47,237,223,80,195,181,168,23,14,252,244,107,95,78,63,106,44,88,28,
1,13,40,232,210,194,178,163,69,15,41,120,193,164,82,150,108,68,69,96,
116,136,147,34,1,248,209,150,95,104,151,161,57,128,46,239,120,137,122,227,
19,16,8,171, };
GLubyte stb__arial_25_usascii_t[95]={ 1,43,74,1,1,1,1,74,1,1,74,
61,61,74,69,1,24,43,43,1,43,61,1,61,1,1,61,61,61,74,61,
43,1,43,43,24,43,43,43,24,61,24,1,43,43,43,43,1,43,1,43,
24,43,24,24,24,24,24,24,1,24,1,74,74,74,61,1,61,24,61,24,
24,24,24,1,43,24,61,61,61,24,24,61,61,24,61,61,61,61,43,61,
1,1,1,74, };
int stb__arial_25_usascii_a[95]={ 99,99,127,199,199,318,239,68,
119,119,139,209,99,119,99,99,199,199,199,199,199,199,199,199,
199,199,99,99,209,209,209,199,363,239,239,259,259,239,219,278,
259,99,179,239,199,298,259,278,239,278,259,239,219,259,239,338,
239,239,219,99,99,99,168,199,119,199,199,179,199,199,99,199,
199,80,80,179,80,298,199,199,199,199,119,179,99,199,179,259,
179,179,179,120,93,120,209, };

GLbyte stb__arial_50_usascii_x[95] = { 0,3,2,0,1,2,1,1,2,2,1,2,3,1,
4,0,1,4,1,1,0,1,1,2,1,1,4,3,2,2,2,1,2,-1,3,2,3,3,3,2,3,4,1,3,
3,3,3,2,3,1,3,2,1,3,0,0,0,0,0,3,0,0,1,-1,1,1,2,1,1,1,0,1,2,2,
-3,2,2,2,2,1,2,1,2,1,0,2,0,0,0,0,0,1,4,1,1,};
GLbyte stb__arial_50_usascii_y[95] = { 40,7,7,7,5,7,7,7,7,7,7,13,35,26,
35,7,7,7,7,7,7,8,7,8,7,7,16,16,13,17,13,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,46,7,16,7,16,7,16,7,16,7,7,
7,7,7,16,16,16,16,16,16,16,8,16,16,16,16,16,16,7,7,7,20,};
// modified from original data by assigning a width of 16 to space char
// first value in this array
GLubyte stb__arial_50_usascii_w[95] = { 16,6,12,25,22,36,28,6,12,12,15,22,6,13,
5,13,22,13,22,22,23,23,22,21,22,22,5,6,22,22,22,22,42,31,25,29,27,25,23,31,26,5,18,27,
21,31,26,31,25,33,29,26,26,26,30,42,30,30,27,9,13,10,19,27,10,22,22,21,21,23,14,21,20,5,
10,21,5,33,20,23,22,21,14,20,13,20,22,32,23,22,22,13,4,13,24,};
GLubyte stb__arial_50_usascii_h[95] = { 0,33,13,34,40,35,34,13,43,43,15,22,12,5,
5,34,34,33,33,34,33,33,34,32,34,34,24,31,23,14,23,33,43,33,33,34,33,33,33,34,33,33,34,33,
33,33,33,34,33,36,33,34,33,34,33,33,33,33,33,42,34,42,18,3,7,25,34,25,34,25,33,34,33,33,
43,33,33,24,24,25,33,33,24,25,33,25,24,24,24,34,24,43,43,43,8,};
GLubyte stb__arial_50_usascii_s[95] = { 252,57,169,56,135,192,82,162,1,101,123,
80,182,229,246,111,178,225,1,69,183,110,114,207,125,201,246,239,34,139,57,
160,58,78,134,148,50,24,173,37,228,197,164,145,123,91,64,1,31,158,1,
224,201,137,148,105,74,43,15,125,23,114,103,158,214,1,33,24,92,46,210,
1,183,204,47,179,248,115,172,70,225,203,240,229,1,94,149,1,216,229,193,
33,28,14,189,};
GLubyte stb__arial_50_usascii_t[95] = { 1,149,243,45,1,1,45,243,1,1,243,
243,243,209,174,45,45,149,183,80,183,183,80,183,45,45,149,149,243,243,243,
183,1,183,183,45,183,183,149,80,115,149,80,149,149,149,149,45,149,1,149,
45,115,80,115,115,115,115,115,1,80,1,243,38,243,217,45,217,80,217,80,
80,80,80,1,115,80,217,217,217,80,149,217,183,115,217,217,243,217,1,217,
1,1,1,243,};
int stb__arial_50_usascii_a[95] = { 199,199,254,398,398,637,478,137,
238,238,279,418,199,238,199,199,398,398,398,398,398,398,398,398,
398,398,199,199,418,418,418,398,727,478,478,517,517,478,437,557,
517,199,358,478,398,597,517,557,478,557,517,478,437,517,478,676,
478,478,437,199,199,199,336,398,238,398,398,358,398,398,199,398,
398,159,159,358,159,597,398,398,398,398,238,358,199,398,358,517,
358,358,358,239,186,239,418,};

int *yHashtagFont = NULL;
int *wHashtagFont = NULL;
int *hHashtagFont = NULL;
int *sHashtagFont = NULL;
int *tHashtagFont = NULL;

GLuint drawBuffers[16] = {
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
/////////////////////////////////////////////
GLuint FBO_Drawing_BGParticlesCATracks[PG_NB_MEMORIZED_PASS]; // PG_NB_ATTACHMENTS=5
GLuint FBO_Composition_capturedFB_prec[2] = { 0, 0 }; //  drawing memory on odd and even frames for echo and sensors
GLuint FBO_TrackDisplay = 0;
// GLuint FBO_Composition_capturedFB = 0; //  frame memory for frame subtraction or old frame display
// FBO texture
GLuint FBO_Drawing_BGParticlesCATracks_texID[PG_NB_MEMORIZED_PASS*PG_NB_ATTACHMENTS]; // PG_NB_ATTACHMENTS=5
GLuint FBO_Composition_capturedFB_prec_texID[2] = { 0, 0 }; // drawing memory on odd and even frames for echo
#ifdef PG_TRACK_DISPLAY
GLuint FBO_TrackDisplay_texID = 0;
#endif
// GLuint FBO_Composition_capturedFB_texID = 0; //  frame memory for frame subtraction or old frame display


/////////////////////////////////////////////////////////////////
// ERRORS
/////////////////////////////////////////////////////////////////
GLenum printOglError( int no ) {
    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR) {
        printf("glError at %d -> %d: %s\n", no ,
			     glErr, gluErrorString(glErr));
    }
    return glErr;
}

void ReportError( char *errorString ) {
  if( fileLog ) {
    fprintf( fileLog , "**** %s ****\n" , errorString );
  }

  fprintf( stderr , "**** %s ****\n" , errorString );
}


/////////////////////////////////////////////////////////////////
// TIME
/////////////////////////////////////////////////////////////////

#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
  SYSTEMTIME system_time;
  FILETIME file_time;
  ULARGE_INTEGER ularge;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time, &file_time);
  ularge.LowPart = file_time.dwLowDateTime;
  ularge.HighPart = file_time.dwHighDateTime;

  tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
  tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
  /* 0 indicates that the call succeeded. */
  return 0;
}
#endif

double RealTime( void ) {
  struct timeval time;
  gettimeofday(&time, 0);
  double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
  return (realtime - InitialRealTime) * time_scale;
}

float MemoryUsage( void ) {
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS info;
	GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
	return (float)(info.WorkingSetSize) / (1024.f*1024.f);
#else
  // struct rusage {
  //   struct timeval ru_utime; /* user time used */
  //   struct timeval ru_stime; /* system time used */
  //   long   ru_maxrss;        /* maximum resident set size */
  //   long   ru_ixrss;         /* integral shared memory size */
  //   long   ru_idrss;         /* integral unshared data size */
  //   long   ru_isrss;         /* integral unshared stack size */
  //   long   ru_minflt;        /* page reclaims */
  //   long   ru_majflt;        /* page faults */
  //   long   ru_nswap;         /* swaps */
  //   long   ru_inblock;       /* block input operations */
  //   long   ru_oublock;       /* block output operations */
  //   long   ru_msgsnd;        /* messages sent */
  //   long   ru_msgrcv;        /* messages received */
  //   long   ru_nsignals;      /* signals received */
  //   long   ru_nvcsw;         /* voluntary context switches */
  //   long   ru_nivcsw;        /* involuntary context switches */
  // };

  struct rusage usage;

  getrusage( RUSAGE_SELF , &usage );

  return float(usage.ru_maxrss/1024.f);
#endif
}

/////////////////////////////////////////////////////////////////
// GEOMETRY INITIALIZATION
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TWO QUADS USED FOR DISPLAY: DRAWING AND COMPOSITION QUADS
void pg_initGeometry_quads( void ) {
  /////////////////////////////////////////////////////////////////////
  // QUAD FOR DRAWING AND COMPOSITION
  // point positions and texture coordinates
  quadDraw_points[1] = (float)window_height;
  quadDraw_points[3] = (float)leftWindowWidth;
  quadDraw_points[10] = (float)window_height;
  quadDraw_points[12] = (float)leftWindowWidth;
  quadDraw_points[13] = (float)window_height;
  quadDraw_points[15] = (float)leftWindowWidth;

  quadDraw_texCoords[1] = (float)window_height;
  quadDraw_texCoords[2] = (float)leftWindowWidth;
  quadDraw_texCoords[7] = (float)window_height;
  quadDraw_texCoords[8] = (float)leftWindowWidth;
  quadDraw_texCoords[9] = (float)window_height;
  quadDraw_texCoords[10] = (float)leftWindowWidth;

  quadComposition_points[1] = (float)window_height;
  quadComposition_points[3] = (float)doubleWindowWidth;
  quadComposition_points[10] = (float)window_height;
  quadComposition_points[12] = (float)doubleWindowWidth;
  quadComposition_points[13] = (float)window_height;
  quadComposition_points[15] = (float)doubleWindowWidth;

  quadComposition_texCoords[1] = (float)window_height;
  quadComposition_texCoords[2] = (float)doubleWindowWidth;
  quadComposition_texCoords[7] = (float)window_height;
  quadComposition_texCoords[8] = (float)doubleWindowWidth;
  quadComposition_texCoords[9] = (float)window_height;
  quadComposition_texCoords[10] = (float)doubleWindowWidth;

  printOglError( 21 );

  // vertex buffer objects and vertex array
  unsigned int quadDraw_points_vbo = 0;
  glGenBuffers (1, &quadDraw_points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_points_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		3 * 3 * nbFaces * sizeof (float),
		quadDraw_points,
		GL_STATIC_DRAW);
  unsigned int quadDraw_texCoord_vbo = 0;
  glGenBuffers (1, &quadDraw_texCoord_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		2 * 3 * nbFaces * sizeof (float),
		quadDraw_texCoords,
		GL_STATIC_DRAW);

  quadDraw_vao = 0;
  glGenVertexArrays (1, &quadDraw_vao);
  glBindVertexArray (quadDraw_vao);
  // vertex positions are location 0
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (0);

  // texture coordinates are location 1
  glBindBuffer (GL_ARRAY_BUFFER, quadDraw_texCoord_vbo);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (1); // don't forget this!

  printOglError( 22 );

  // vertex buffer objects and vertex array
  unsigned int quadComposition_points_vbo = 0;
  glGenBuffers (1, &quadComposition_points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadComposition_points_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		3 * 3 * nbFaces * sizeof (float),
		quadComposition_points,
		GL_STATIC_DRAW);
  unsigned int quadComposition_texCoord_vbo = 0;
  glGenBuffers (1, &quadComposition_texCoord_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, quadComposition_texCoord_vbo);
  glBufferData (GL_ARRAY_BUFFER,
		2 * 3 * nbFaces * sizeof (float),
		quadComposition_texCoords,
		GL_STATIC_DRAW);

  quadComposition_vao = 0;
  glGenVertexArrays (1, &quadComposition_vao);
  glBindVertexArray (quadComposition_vao);
  // vertex positions are location 0
  glBindBuffer (GL_ARRAY_BUFFER, quadComposition_points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (0);

  // texture coordinates are location 1
  glBindBuffer (GL_ARRAY_BUFFER, quadComposition_texCoord_vbo);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
  glEnableVertexAttribArray (1); // don't forget this!

  printf("Geometry initialized %dx%d & %dx%d\n" , leftWindowWidth , window_height ,
	  doubleWindowWidth , window_height );

  printOglError( 23 );
}

/////////////////////////////////////////////////////////////////
// INTIALIZATION OF THE TACK LINES USED TO DISPLAY A TRACK AS A STRIP
void pg_initGeometry_track_line( int indTrack , bool is_target ) {
  // source track
  if( !is_target && TrackStatus_source[ indTrack ].nbRecordedFrames > 0 ) {
    if( line_tracks_points[indTrack] ) {
      // memory release
      delete [] line_tracks_points[indTrack];
    }
    line_tracks_points[indTrack] = new float[TrackStatus_source[ indTrack ].nbRecordedFrames * 3];
    if(line_tracks_points[indTrack]  == NULL) {
      strcpy( ErrorStr , "Track line point allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    // assigns the index to the coordinates to be able to recover the coordinates from the texture through the point index
    for(int indFrame = 0 ; indFrame < TrackStatus_source[ indTrack ].nbRecordedFrames ; indFrame++ ) {
	line_tracks_points[indTrack][ indFrame * 3 ] = (float)indFrame;
    }

    printOglError( 41 );

    // vertex buffer objects
    unsigned int line_tracks_points_vbo = 0;
    glGenBuffers (1, &line_tracks_points_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_points_vbo);
    glBufferData (GL_ARRAY_BUFFER,
		3 * TrackStatus_source[ indTrack ].nbRecordedFrames * sizeof (float),
		line_tracks_points[indTrack],
		GL_STATIC_DRAW);

    line_tracks_vao[indTrack] = 0;
    glGenVertexArrays (1, &(line_tracks_vao[indTrack]));
    glBindVertexArray (line_tracks_vao[indTrack]);
    // vertex positions are location 0
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_points_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (0);

    printf("Source track geometry initialized track %d: size %d\n" , indTrack , TrackStatus_source[ indTrack ].nbRecordedFrames );
    printOglError( 42 );
  }

  // target track
  if( is_target && TrackStatus_target[ indTrack ].nbRecordedFrames > 0 ) {
    if( line_tracks_target_points[indTrack] ) {
      // memory release
      delete [] line_tracks_target_points[indTrack];
    }
    line_tracks_target_points[indTrack] = new float[TrackStatus_target[ indTrack ].nbRecordedFrames * 3];
    if(line_tracks_target_points[indTrack]  == NULL) {
      strcpy( ErrorStr , "Target track line point allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    // assigns the index to the coordinates to be able to recover the coordinates from the texture through the point index
    for(int indFrame = 0 ; indFrame < TrackStatus_target[ indTrack ].nbRecordedFrames ; indFrame++  ) {
	line_tracks_target_points[indTrack][ indFrame * 3 ] = (float)indFrame;
    }

    printOglError( 43 );

    // vertex buffer objects
    unsigned int line_tracks_target_points_vbo = 0;
    glGenBuffers (1, &line_tracks_target_points_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_target_points_vbo);
    glBufferData (GL_ARRAY_BUFFER,
		3 * TrackStatus_target[ indTrack ].nbRecordedFrames * sizeof (float),
		line_tracks_target_points[indTrack],
		GL_STATIC_DRAW);

    line_tracks_target_vao[indTrack] = 0;
    glGenVertexArrays (1, &(line_tracks_target_vao[indTrack]));
    glBindVertexArray (line_tracks_target_vao[indTrack]);
    // vertex positions are location 0
    glBindBuffer (GL_ARRAY_BUFFER, line_tracks_target_points_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
    glEnableVertexAttribArray (0);

    printf("Target track geometry initialized track %d: size %d\n" , indTrack , TrackStatus_target[ indTrack ].nbRecordedFrames );

    printOglError( 44 );
  }
}

/////////////////////////////////////////////////////////////////
// VIDEO FRAME INITIALIZATION
/////////////////////////////////////////////////////////////////

#ifdef PG_VIDEO_ACTIVE
bool pg_initFrameTexture(Mat *video_frame) {
		glEnable(GL_TEXTURE_2D);

	// image capture
	glGenTextures(1, &pg_video_texture_texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		video_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		video_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		video_frame->data);        // The actual image data itself
						   // current background
	glGenTextures(1, &pg_video_currentBG_texture_texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_currentBG_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		video_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		video_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		video_frame->data);        // The actual image data itself
										// initial background
	glGenTextures(1, &pg_video_initialBG_texture_texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_initialBG_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		video_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		video_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		video_frame->data);        // The actual image data itself
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}

int pg_initVideoCapture(void) {
	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use.
    printf("Video frame grabbing from device ID #%d!\n",camID);
	pg_input_camera.open(camID);

	// pg_input_camera.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	// pg_input_camera.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	// printf("exposure   %.2f\n", (float)pg_input_camera.get(CV_CAP_PROP_EXPOSURE));
	// printf("gain       %.2f\n", (float)pg_input_camera.get(CV_CAP_PROP_GAIN));
	// printf("brightness %.2f\n", (float)pg_input_camera.get(CV_CAP_PROP_BRIGHTNESS));
	// printf("saturation %.2f\n", (float)pg_input_camera.get(CV_CAP_PROP_SATURATION));
	// printf("contrast   %.2f\n", (float)pg_input_camera.get(CV_CAP_PROP_CONTRAST));
	// printf("focus      %.2f\n", pg_input_camera.get(CV_CAP_PROP_FOCUS));
	// pg_input_camera.set(CV_CAP_PROP_EXPOSURE, -6.0);
	// pg_input_camera.set(CV_CAP_PROP_GAIN, 0.0);
	// pg_input_camera.set(CV_CAP_PROP_BRIGHTNESS, 128.0);
	// pg_input_camera.set(CV_CAP_PROP_CONTRAST, 128.0);
	// pg_input_camera.set(CV_CAP_PROP_SATURATION, 50.0);
	// pg_input_camera.set(CV_CAP_PROP_FPS, 60);
	// pg_input_camera.set(CV_CAP_PROP_FOCUS, 25);

	//Grabs and returns a frame from camera
	pg_input_camera >> pg_video_frame;
	// texture loading
	if (!pg_video_frame.data) {
		ReportError((char *)"Error: Video frame not grabbed!");
		return -1;
	}
	if (!pg_initFrameTexture(&pg_video_frame)) {
		printf("Video frame not initialized!\n");
		return -1;
	}
	printf("Video initialized %dx%d\n", pg_video_frame.cols, pg_video_frame.rows);

	return 1;
}
#endif

/////////////////////////////////////////////////////////////////
// FBO INITIALIZATION
/////////////////////////////////////////////////////////////////
bool pg_initFBOTextures( GLuint *textureID , int nb_attachments ) {
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  for( int indAtt = 0 ; indAtt < nb_attachments ; indAtt++ ) {
    glBindTexture(GL_TEXTURE_RECTANGLE,
		  textureID[ indAtt ]);
    glTexParameteri(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_RECTANGLE,1,GL_RGBA32F,
		   leftWindowWidth , window_height );
    // printf("FBO size %d %d \n" , leftWindowWidth , window_height );
    glFramebufferTexture2D( GL_FRAMEBUFFER,
			    GL_COLOR_ATTACHMENT0 + indAtt ,
			    GL_TEXTURE_RECTANGLE,
			    textureID[ indAtt ] ,
			    0 );
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)
       ==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
      sprintf( ErrorStr , "Error: Binding RECT FBO texture No %d ID %d (error %d)!" , indAtt , textureID[ indAtt ] , glCheckFramebufferStatus(GL_FRAMEBUFFER) ); ReportError( ErrorStr ); throw 336;
    }
  }

  return true;
}

bool pg_initFBO( void ) {
  int maxbuffers;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);

  if( maxbuffers < PG_NB_ATTACHMENTS ) {
    sprintf( ErrorStr , "Error: Maximal attachment (%d) -> %d required!" , maxbuffers , PG_NB_ATTACHMENTS ); ReportError( ErrorStr ); throw 336;
  }

  // initializations to NULL
  for( int indFB = 0 ; indFB < PG_NB_MEMORIZED_PASS ; indFB++ ) {
    FBO_Drawing_BGParticlesCATracks[indFB] = 0;
  }
  for( int indFB_att = 0 ; indFB_att < PG_NB_MEMORIZED_PASS * PG_NB_ATTACHMENTS ; indFB_att++ ) {
    FBO_Drawing_BGParticlesCATracks_texID[indFB_att] = 0;
  }

  // FBO initializations
  glGenFramebuffers( PG_NB_MEMORIZED_PASS, FBO_Drawing_BGParticlesCATracks );
  glGenTextures( PG_NB_MEMORIZED_PASS * PG_NB_ATTACHMENTS, FBO_Drawing_BGParticlesCATracks_texID);
  for( int indFB = 0 ; indFB < PG_NB_MEMORIZED_PASS ; indFB++ ) {
    glBindFramebuffer( GL_FRAMEBUFFER, FBO_Drawing_BGParticlesCATracks[indFB] );
    pg_initFBOTextures( FBO_Drawing_BGParticlesCATracks_texID + indFB * PG_NB_ATTACHMENTS , PG_NB_ATTACHMENTS );
    glDrawBuffers( PG_NB_ATTACHMENTS , drawBuffers);
  }

  glGenFramebuffers( 2, FBO_Composition_capturedFB_prec );  // drawing memory on odd and even frames for echo
  glGenTextures(2, FBO_Composition_capturedFB_prec_texID);
  for( int indFB = 0 ; indFB < 2 ; indFB++ ) {
    glBindFramebuffer( GL_FRAMEBUFFER, FBO_Composition_capturedFB_prec[indFB] );
    pg_initFBOTextures(  FBO_Composition_capturedFB_prec_texID + indFB , 1 );
    glDrawBuffers( 1 , drawBuffers);
  }

#ifdef PG_TRACK_DISPLAY
  glGenFramebuffers( 1, &FBO_TrackDisplay );
  glGenTextures(1, &FBO_TrackDisplay_texID );
  glBindFramebuffer( GL_FRAMEBUFFER, FBO_TrackDisplay );
  pg_initFBOTextures( &FBO_TrackDisplay_texID , 1 );
  glDrawBuffers( 1 , drawBuffers);
#endif

  //glGenFramebuffers(1, &FBO_Composition_capturedFB); //  frame memory for frame subtraction or old frame display
  //glGenTextures(1, &FBO_Composition_capturedFB_texID); //  frame memory for frame subtraction or old frame display
  //glBindFramebuffer(GL_FRAMEBUFFER, FBO_Composition_capturedFB);
  //pg_initFBOTextures(&FBO_Composition_capturedFB_texID, 1);
  //glDrawBuffers(1, drawBuffers);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

/////////////////////////////////////////////////////////////////
// MATRIX INITIALIZATION
/////////////////////////////////////////////////////////////////
void pg_initRenderingMatrices( void ) {
  memset( (char *)viewMatrix , 0 , 16 * sizeof( float ) );
  memset( (char *)modelMatrix , 0 , 16 * sizeof( float ) );
#ifdef PG_TRACK_DISPLAY
GLfloat projMatrixDisplayTracks[16];
  memset( (char *)modelMatrixDisplayTracks , 0 , 16 * sizeof( float ) );
  modelMatrixDisplayTracks[0] = 1.0f;
  modelMatrixDisplayTracks[5] = 1.0f;
  modelMatrixDisplayTracks[10] = 1.0f;
  modelMatrixDisplayTracks[15] = 1.0f;
#endif
  viewMatrix[0] = 1.0f;
  viewMatrix[5] = 1.0f;
  viewMatrix[10] = 1.0f;
  viewMatrix[15] = 1.0f;
  modelMatrix[0] = 1.0f;
  modelMatrix[5] = 1.0f;
  modelMatrix[10] = 1.0f;
  modelMatrix[15] = 1.0f;

  // ORTHO
  float l = 0.0f;
  float r = (float)leftWindowWidth;
  float b = 0.0f;
  float t = (float)window_height;
  float n = -1.0f;
  float f = 1.0f;
  GLfloat mat[] = {
    (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0,
    0.0, (GLfloat)(2.0/(t-b)), 0.0, 0.0,
    0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0,
    (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
  memcpy( (char *)projMatrix , mat , 16 * sizeof( float ) );
  // printf("Orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
#ifdef PG_TRACK_DISPLAY
    GLfloat matDisplayTracks[] = {
      (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0,
      0.0, (GLfloat)(2.0/(b-t)), 0.0, 0.0,
      0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0,
      (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(b+t)/(b-t)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
    memcpy( (char *)projMatrixDisplayTracks , matDisplayTracks , 16 * sizeof( float ) );
    // printf("Orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
#endif
  r = (float)doubleWindowWidth;
  GLfloat mat2[] = {
    (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0,
    0.0, (GLfloat)(2.0/(t-b)), 0.0, 0.0,
    0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0,
    (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
  memcpy( (char *)doubleProjMatrix , mat2 , 16 * sizeof( float ) );
  // printf("Double width orthographic projection l %.2f r %.2f b %.2f t %.2f n %.2f f %.2f\n" , l,r,b,t,n,f);
}

///////////////////////////////////////////////////////
// On-Screen Message Display
///////////////////////////////////////////////////////

bool pg_initHashtagFontOffets(string dir, string basefilename) {
	bool valRet = true;
	sHashtagFont = new int[PG_NB_HASHTAG_CHARS];
	tHashtagFont = new int[PG_NB_HASHTAG_CHARS];
	wHashtagFont = new int[PG_NB_HASHTAG_CHARS];
	hHashtagFont = new int[PG_NB_HASHTAG_CHARS];
	yHashtagFont = new int[PG_NB_HASHTAG_CHARS];

	ifstream myReadFile;
	myReadFile.open(dir + basefilename);
	int indTag = 0;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof() && indTag < PG_NB_HASHTAG_CHARS) {
			string line;
			std::getline(myReadFile, line);
			std::stringstream  ss(line);
			ss >> sHashtagFont[indTag];
			ss >> tHashtagFont[indTag];
			ss >> wHashtagFont[indTag];
			ss >> hHashtagFont[indTag];
			yHashtagFont[indTag] = 0;
			// printf("READ hf ind %d %d %d %d %d %d\n", indTag , sHashtagFont[indTag], tHashtagFont[indTag], wHashtagFont[indTag], hHashtagFont[indTag], yHashtagFont[indTag]);
			indTag++;
		}
	}
	else {
		strcpy(ErrorStr, ("File " + dir + basefilename + "not opened!").c_str()); ReportError(ErrorStr); throw 152;
	}
	myReadFile.close();
	return valRet;
}

///////////////////////////////////////////////////////
// Message Uploads

bool pg_ReadAllDisplayMessages(string dir, string basefilename) {
	bool valRet = true;
	DisplayTextList = new string[PG_NB_DISPLAY_MESSAGES];
	DisplayTextFirstInChapter = new int[PG_NB_DISPLAY_MESSAGES];
	for (int ind = 0; ind < PG_NB_DISPLAY_MESSAGES; ind++) {
		DisplayTextList[ind] = "";
		DisplayTextFirstInChapter[ind] = 0;
	}
	ifstream myReadFile;
	myReadFile.open(dir + basefilename);
	int indChapter = 0;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof() && NbDisplayTexts < PG_NB_DISPLAY_MESSAGES) {
			std::getline(myReadFile, DisplayTextList[NbDisplayTexts]); // Saves the line in STRING.
																	   // std::cout << DisplayTextList[NbDisplayTexts]+"\n"; // Prints our STRING.
			while (!myReadFile.eof() && (DisplayTextList[NbDisplayTexts].find("request =>", 0, strlen("request =>")) == 0)) {
				DisplayTextFirstInChapter[indChapter++] = NbDisplayTexts;
				// std::cout << "Message dir #" << indChapter - 1 << " index #" << DisplayTextFirstInChapter[indChapter-1] << "\n"; // Prints our STRING.
				std::getline(myReadFile, DisplayTextList[NbDisplayTexts]);
			}
			if (!(DisplayTextList[NbDisplayTexts].empty())) {
				NbDisplayTexts++;
			}
		}
	}
	else {
		strcpy(ErrorStr, ("File " + dir + basefilename + "not opened!").c_str()); ReportError(ErrorStr); throw 152;
	}
	myReadFile.close();

	if (NbDisplayTexts >= PG_NB_DISPLAY_MESSAGES) {
		strcpy(ErrorStr, ("Only part of the messages read from " + dir + basefilename + "!").c_str()); ReportError(ErrorStr);
	}
	TextChapter = -1;
	DisplayTextFirstInChapter[indChapter] = NbDisplayTexts;
	NbTextChapters = indChapter;
	std::cout << "Loaded texts #" << NbDisplayTexts << " in chapters #" << NbTextChapters << "\n";
	return valRet;
}

///////////////////////////////////////////////////////
// Hashtags Uploads

bool pg_ReadAllHashtags(string dir, string basefilename, string colorfilename) {
	bool valRet = true;
	HashtagList = new string[PG_NB_HASHTAGS];
	HashtagColorList = new int[PG_NB_HASHTAGS * 3];
	for (int ind = 0; ind < PG_NB_HASHTAGS; ind++) {
		HashtagList[ind] = "";
		HashtagColorList[ind * 3 + 0] = 0;
		HashtagColorList[ind * 3 + 1] = 0;
		HashtagColorList[ind * 3 + 2] = 0;
		//HashtagColorList[ind * 3] = (GLubyte)(255 * (double)rand() / (double)RAND_MAX);
		//HashtagColorList[ind * 3 + 1] = (GLubyte)(255 * (double)rand() / (double)RAND_MAX);
		//HashtagColorList[ind * 3 + 2] = (GLubyte)(255 * (double)rand() / (double)RAND_MAX);
		//printf("%d %d %d\n", HashtagColorList[ind * 3], HashtagColorList[ind * 3 + 1],
		//	HashtagColorList[ind * 3 + 2]);
	}
	ifstream myReadFile;
	myReadFile.open(dir + basefilename);
	NbHashtags = 0;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof() && NbHashtags < PG_NB_HASHTAGS) {
			std::getline(myReadFile, HashtagList[NbHashtags]); // Saves the line in STRING.
			if (!(HashtagList[NbHashtags].empty())) {
				NbHashtags++;
			}
		}
	}
	else {
		strcpy(ErrorStr, ("File " + dir + basefilename + "not opened!").c_str()); ReportError(ErrorStr); throw 152;
	}
	myReadFile.close();

	if (NbHashtags > PG_NB_HASHTAGS) {
		strcpy(ErrorStr, ("Only part of the hashtags read from " + dir + basefilename + "!").c_str()); ReportError(ErrorStr);
	}
	std::cout << "Loaded hashtags #" << NbHashtags << "\n";

	myReadFile.open(dir + colorfilename);
	int indColor = 0;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof() && indColor < PG_NB_HASHTAGS) {
			string line;
			std::getline(myReadFile, line);
			std::stringstream  ss(line);
			// std::cout << line << "\nVALUES: ";
			for (int indP = 0; indP < 3; indP++) {
				ss >> HashtagColorList[indColor * 3 + indP];
				// std::cout << HashtagColorList[indColor * 3 + indP] << " ";
			}
			indColor++;
		}
	}
	else {
		strcpy(ErrorStr, ("File " + dir + colorfilename + "not opened!").c_str()); ReportError(ErrorStr); throw 152;
	}
	myReadFile.close();

	return valRet;
}

///////////////////////////////////////////////////////
// small warning messages
int  pg_screenMessage_upload(void) {
	GLubyte *yfont = stb__arial_15_usascii_y;
	GLubyte *wfont = stb__arial_15_usascii_w;
	GLubyte *hfont = stb__arial_15_usascii_h;
	GLubyte *sfont = stb__arial_15_usascii_s;
	GLubyte *tfont = stb__arial_15_usascii_t;

	int pixelRank = 0;
	memset(pg_screen_msg_offset_texture, (GLubyte)0, message_pixel_length * 4);

	// strcpy( ScreenMessage , "abcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuv");
	// strcpy( ScreenMessage , "abcdefghijkl");

	for (char *c = ScreenMessage; *c != '\0' && pixelRank < message_pixel_length; c++)  {
		char cur_car = *c;

		// usable ascii starts at blank space
		int cur_car_rank = (int)cur_car - 32;
		cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank);
		cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank);

		// defines offset according to table
		for (int indPixel = 0; indPixel < wfont[cur_car_rank] && pixelRank + indPixel < message_pixel_length; indPixel++) {
			int indPixelColor = (pixelRank + indPixel) * 4;
			pg_screen_msg_offset_texture[indPixelColor] = sfont[cur_car_rank] + indPixel;
			pg_screen_msg_offset_texture[indPixelColor + 1] = tfont[cur_car_rank];
			pg_screen_msg_offset_texture[indPixelColor + 2] = hfont[cur_car_rank];
			pg_screen_msg_offset_texture[indPixelColor + 3] = yfont[cur_car_rank];
			// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
		}
		pixelRank += wfont[cur_car_rank] + 1;
	}
	ScreenMessageUpdated = false;

	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screen_msg_offset_texID);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA8,           // Internal colour format to convert to
		message_pixel_length,         // Image width
		1,                 // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		pg_screen_msg_offset_texture);        // The actual image data itself
	printOglError(6);
	return pixelRank + 1;
}

///////////////////////////////////////////////////////
// large display messages
int pg_displayMessage_upload(int indMesg) {
	GLbyte *yfont = stb__arial_25_usascii_y;
	GLubyte *wfont = stb__arial_25_usascii_w;
	GLubyte *hfont = stb__arial_25_usascii_h;
	GLubyte *sfont = stb__arial_25_usascii_s;
	GLubyte *tfont = stb__arial_25_usascii_t;

	int pixelRank = 0;
	GLubyte *offSetText = (indMesg == 1 ? pg_displayMsg1_offset_texture : pg_displayMsg2_offset_texture);
	memset(offSetText, (GLubyte)0, message_pixel_length * 4);

	if (indMesg == 1) {
		if (IndDisplayText1 == -1) {
			return 0;
		}
		DisplayText1 = DisplayTextList[IndDisplayText1 % NbDisplayTexts];
		// DisplayText1 = "abcdefghijkl";
	}
	else {
		if (IndDisplayText2 == -1) {
			return 0;
		}
		DisplayText2 = DisplayTextList[IndDisplayText2 % NbDisplayTexts];
		// DisplayText1 = "abcdefghijkl";
	}

	for (const char *c = (indMesg == 1 ? DisplayText1.c_str() : DisplayText2.c_str());
		*c != '\0' && pixelRank < message_pixel_length; c++) {
		char cur_car = *c;
		if (cur_car == 'à' ||
			cur_car == 'â') {
			cur_car = 'a';
		}
		else if (cur_car == 'é' ||
			cur_car == 'è' ||
			cur_car == 'ê' ||
			cur_car == 'ë') {
			cur_car = 'e';
		}
		else if (cur_car == 'î' ||
			cur_car == 'ï') {
			cur_car = 'i';
		}
		else if (cur_car == 'ô') {
			cur_car = 'o';
		}
		else if (cur_car == 'û' ||
			cur_car == 'ù') {
			cur_car = 'u';
		}
		else if (cur_car == 'À' ||
			cur_car == 'Â') {
			cur_car = 'A';
		}
		else if (cur_car == 'É' ||
			cur_car == 'È' ||
			cur_car == 'Ê' ||
			cur_car == 'Ë') {
			cur_car = 'E';
		}
		else if (cur_car == 'Î' ||
			cur_car == 'Ï') {
			cur_car = 'I';
		}
		else if (cur_car == 'Ô') {
			cur_car = 'o';
		}
		else if (cur_car == 'Û' ||
			cur_car == 'Ù') {
			cur_car = 'U';
		}

		// usable ascii starts at blank space
		int cur_car_rank = (int)cur_car - 32;
		cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank);
		cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank);

		// defines offset according to table
		// defines the offsets for all the horizontal pixels covering
		// the texture of the current character
		for (int indPixel = 0;
			indPixel < wfont[cur_car_rank] && (pixelRank + indPixel) < message_pixel_length;
			indPixel++) {
			// the position of the four bytes corresponding to the current horizontal pixel
			// pixelRank is the rank of the first horizontal pixel of the current character
			// indPixel is the rank of the current pixel inside the horizontal texture of the current character
			int indPixelColor = (pixelRank + indPixel) * 4;
			// X encodes the s texture coordinate of the current character (it is offseted by
			// indPixel because we are on the indPixel_th horizontal pixel
			offSetText[indPixelColor] = sfont[cur_car_rank] + indPixel;
			// Y encodes the t texture coordinate of the current character
			offSetText[indPixelColor + 1] = tfont[cur_car_rank];
			// Z and W encode height H and vertical offsets Y
			// the character texture coordinates are bound by Y and Y + H
			offSetText[indPixelColor + 2] = hfont[cur_car_rank];
			offSetText[indPixelColor + 3] = yfont[cur_car_rank];
			// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
		}
		// jumps to the next character over the width of the current character + 1 black vertical 1 pixel stripe
		pixelRank += wfont[cur_car_rank] + 1;
	}

	glEnable(GL_TEXTURE_RECTANGLE);
	if (indMesg == 1) {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_displayMsg1_offset_texID);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_displayMsg2_offset_texID);
	}
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA8,           // Internal colour format to convert to
		message_pixel_length,         // Image width
		1,                 // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		offSetText);        // The actual image data itself
	printOglError(6);
	return pixelRank + 1;
}

///////////////////////////////////////////////////////
// hashtags
int pg_hashtag_upload(void) {
	GLubyte *offSetText = pg_hashtag_offset_texture;
	memset(offSetText, 0, hashtag_pixel_length * 2 * 4 * sizeof(GLubyte));
	int charColor[40];

	int precHTind = CurrentHashtag - 1;
	int curHTind = CurrentHashtag;
	// first hashtag non blended
	if (precHTind < 0) {
		Hashtag = "#" + HashtagList[0];
		for (int ind = 1; ind < int(HashtagList[0].length()) + 1 && ind < 40; ind++) {
			charColor[ind] = 0;
		}
	}
	// last hashtag non blended
	else if (curHTind == PG_NB_HASHTAGS)  {
		Hashtag = "#" + HashtagList[PG_NB_HASHTAGS - 1];
		for (int ind = 1; ind < int(HashtagList[PG_NB_HASHTAGS - 1].length()) + 1 && ind < 40; ind++) {
			charColor[ind] = PG_NB_HASHTAGS - 1;
		}
	}
	// hashtag blending
	else {
        while (precHTind >= PG_NB_HASHTAGS) {
            precHTind -= PG_NB_HASHTAGS;
        }
        while (curHTind < 0) {
            curHTind += PG_NB_HASHTAGS;
        }
        while (curHTind >= PG_NB_HASHTAGS) {
            curHTind -= PG_NB_HASHTAGS;
        }

        float elapsed_time_from_start = (float)(CurrentClockTime - InitialScenarioTime);
        float percent = float(int(elapsed_time_from_start) % subscene_duration) / subscene_duration * 0.8f +  0.1f;
        int nbCharsPrec = 0;
        int nbCharsCur = 0;
        if (percent < 0.5) {
            nbCharsPrec = int(HashtagList[precHTind].length() * (1.0 - 2.0 * percent));
        }
        else {
            nbCharsCur = int(HashtagList[curHTind].length() * (2.0 * percent - 1.0));
        }
        // at least one of both hashtag has to begin
        if (nbCharsPrec == 0 && nbCharsCur == 0) {
            nbCharsCur = 1;
        }

        // if the ratio has not changed, the hybrid hashtag is not computed
        if (nbCharsPrec == NbCharHashtagPrec && nbCharsCur == NbCharHashtagCur) {
            return 0;
        }
        NbCharHashtagPrec = nbCharsPrec;
        NbCharHashtagCur = nbCharsCur;

        ///////////////////////////////////////////////
        // Hashtag blending
        // printf("Prec %d cur %d\n", nbCharsPrec, nbCharsCur);
        int indCharPrec = nbCharsPrec;
        int indCharCur = nbCharsCur;
        if (nbCharsPrec > 0) {
            Hashtag = "#" + HashtagList[precHTind].substr(0, nbCharsPrec);
            charColor[0] = (precHTind - 2 + PG_NB_HASHTAGS) % PG_NB_HASHTAGS;
            for (int ind = 1; ind < nbCharsPrec + 1 && ind < 40; ind++) {
                charColor[ind] = (precHTind) % PG_NB_HASHTAGS;
            }
        }
        else if (nbCharsCur > 0) {
            Hashtag = "#" + HashtagList[curHTind].substr(0, nbCharsCur);
            charColor[0] = (precHTind - 2 + PG_NB_HASHTAGS) % PG_NB_HASHTAGS;
            for (int ind = 1; ind < nbCharsCur + 1 && ind < 40; ind++) {
                charColor[ind] = (curHTind) % PG_NB_HASHTAGS;
            }
        }

        //	"#" + HashtagList[curHTind].substr(0, nbCharsCur);
        int nbRemainingCharPrec = HashtagList[precHTind].length() - nbCharsPrec;
        int nbRemainingCharCur = int(HashtagList[curHTind].length()) - nbCharsCur;
        // first letter swapped
        // advances the current hastag if the preceding is the beginning
        if (nbCharsPrec > 0 && nbRemainingCharPrec > 0 && nbRemainingCharCur > 0) {
            Hashtag += HashtagList[curHTind].substr(indCharCur, 1);
            indCharCur++;
            nbRemainingCharCur--;
            charColor[nbCharsPrec + nbCharsCur + 1] = curHTind % PG_NB_HASHTAGS;
        }
        // else advances the preceding hastag if the current is the beginning
        else if (nbCharsCur > 0 && nbRemainingCharPrec > 0 && nbRemainingCharCur > 0) {
            Hashtag += HashtagList[precHTind].substr(indCharPrec, 1);
            indCharPrec++;
            nbRemainingCharPrec--;
            charColor[nbCharsPrec + nbCharsCur + 1] = precHTind % PG_NB_HASHTAGS;
        }
        // copies the remainder
        for (int ind = indCharPrec + indCharCur + 1;
            ind < int(HashtagList[precHTind].length()) + 1 + int(HashtagList[curHTind].length())
                && ind < 40;
            ind++) {
            // advances the current hastag
            if (nbRemainingCharPrec == 0 && nbRemainingCharCur > 0) {
                Hashtag += HashtagList[curHTind].substr(indCharCur, 1);
                indCharCur++;
                nbRemainingCharCur--;
                charColor[ind] = curHTind % PG_NB_HASHTAGS;
            }
            // advances the preceding hastag
            else if (nbRemainingCharCur == 0 && nbRemainingCharPrec > 0) {
                Hashtag += HashtagList[precHTind].substr(indCharPrec, 1);
                indCharPrec++;
                nbRemainingCharPrec--;
                charColor[ind] = precHTind % PG_NB_HASHTAGS;
            }
            // advances the current hastag
            else if (float((double)rand() / (double)RAND_MAX - 0.5) > 0 && nbRemainingCharCur > 0) {
                Hashtag += HashtagList[curHTind].substr(indCharCur, 1);
                indCharCur++;
                nbRemainingCharCur--;
                charColor[ind] = curHTind % PG_NB_HASHTAGS;
            }
            // advances the preceding hastag
            else if (nbRemainingCharPrec > 0) {
                Hashtag += HashtagList[precHTind].substr(indCharPrec, 1);
                indCharPrec++;
                nbRemainingCharPrec--;
                charColor[ind] = precHTind % PG_NB_HASHTAGS;
            }
        }
	}
	// printf("Hashtag [%s]\n", Hashtag.c_str());

	///////////////////////////////////////////////
	// Hashtag pixel length
	int hashtagStringLength = 0;
	for (const char *c = Hashtag.c_str();
		*c != '\0'; c++) {
		char cur_car = *c;

		int cur_car_rank = 0;
		if (cur_car >= 'A' && cur_car <= 'Z') {
			cur_car_rank = (int)(cur_car - 'A');
		}
		else if (cur_car >= 'a' && cur_car <= 'z') {
			cur_car_rank = (int)(cur_car - 'a') + 26;
		}
		else if (cur_car == '#') {
			cur_car_rank = 52;
		}
		else if (cur_car == '_') {
			cur_car_rank = 53;
		}
		hashtagStringLength += wHashtagFont[cur_car_rank];
	}

	// hashtag in the middle of the screen
	int marginPixelLength = 0;
	if (hashtag_pixel_length / 2 - hashtagStringLength > 0) {
		marginPixelLength = (hashtag_pixel_length / 2 - hashtagStringLength) / 2;
	}

	///////////////////////////////////////////////
	// Hashtag shipping to GPU
	// Hashtag = "#STATEONFEMERGENCY";
	// fills with white (1,1) point in font texture
	for (int indPixel = 0;
		indPixel < hashtag_pixel_length / 2 + marginPixelLength;
		indPixel++) {
		offSetText[indPixel * 4] = 1;
		offSetText[indPixel * 4 + 1] = 0;
		// Z-W encodes the t texture coordinate of the current character
		offSetText[indPixel * 4 + 2] = 1;
		offSetText[indPixel * 4 + 3] = 0;
	}
	int pixelRank = hashtag_pixel_length / 2 + marginPixelLength;
	int indChar = 0;
	for (const char *c = Hashtag.c_str();
		*c != '\0' && pixelRank < hashtag_pixel_length;
		c++) {
		char cur_car = *c;

		// usable ascii starts at blank space
		//(theString1 "ABCDEFGHIJKLM"); BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
		//(theString2 "NOPQRSTUVWXYZ"); BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
		//(theString3 "abcdefghijklm"); BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
		//(theString4 "nopqrstuvwxyz#_"); BCDEFGHIJKLMNOPQRSTUVWXYZabcdef#_
		int cur_car_rank = 0;
		if (cur_car >= 'A' && cur_car <= 'Z') {
			cur_car_rank = (int)(cur_car - 'A');
		}
		else if (cur_car >= 'a' && cur_car <= 'z') {
			cur_car_rank = (int)(cur_car - 'a') + 26;
		}
		else if (cur_car == '#') {
			cur_car_rank = 52;
		}
		else if (cur_car == '_') {
			cur_car_rank = 53;
		}
		// printf("%c (%d,w %d,s %d,t %d,h %d) ", cur_car, cur_car_rank,
		// 	wHashtagFont[cur_car_rank], sHashtagFont[cur_car_rank], tHashtagFont[cur_car_rank], hHashtagFont[cur_car_rank]);

		// defines offset according to table
		// defines the offsets for all the horizontal pixels covering
		// the texture of the current character
		for (int indPixel = 0;
			indPixel < wHashtagFont[cur_car_rank] && (pixelRank + indPixel) < hashtag_pixel_length;
			indPixel++) {
			// the position of the four bytes corresponding to the current horizontal pixel
			// pixelRank is the rank of the first horizontal pixel of the current character
			// indPixel is the rank of the current pixel inside the horizontal texture of the current character
			int indPixelOffest = (pixelRank + indPixel) * 4;
			// X-Y encodes the s texture coordinate of the current character (it is offseted by
			// indPixel because we are on the indPixel_th horizontal pixel
			offSetText[indPixelOffest] = (sHashtagFont[cur_car_rank] + indPixel) % 256;
			offSetText[indPixelOffest + 1] = (sHashtagFont[cur_car_rank] + indPixel) / 256;
			// Z-W encodes the t texture coordinate of the current character
			offSetText[indPixelOffest + 2] = tHashtagFont[cur_car_rank] % 256;
			offSetText[indPixelOffest + 3] = tHashtagFont[cur_car_rank] / 256;
			// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );

			int indPixelColor = (hashtag_pixel_length + pixelRank + indPixel) * 4;
			// # is white
			if (indChar == 0) {
				offSetText[indPixelColor] = 255;
				offSetText[indPixelColor + 1] = 255;
				offSetText[indPixelColor + 2] = 255;
				offSetText[indPixelColor + 3] = 255;
			}
			else if (indChar < 40) {
				int indColor = charColor[indChar] % PG_NB_HASHTAGS;
				offSetText[indPixelColor] = HashtagColorList[indColor * 3];
				offSetText[indPixelColor + 1] = HashtagColorList[indColor * 3 + 1];
				offSetText[indPixelColor + 2] = HashtagColorList[indColor * 3 + 2];
				offSetText[indPixelColor + 3] = 255;
			}
		}
		// jumps to the next character over the width of the current character + 1 black vertical 1 pixel stripe
		pixelRank += wHashtagFont[cur_car_rank];
		indChar++;
	}
	// printf("pixelRank % d\n", pixelRank);
	// fills with white (1,1) point in font texture
	for (int indPixel = pixelRank;
		indPixel < hashtag_pixel_length;
		indPixel++) {
		offSetText[indPixel * 4] = 1;
		offSetText[indPixel * 4 + 1] = 0;
		// Z-W encodes the t texture coordinate of the current character
		offSetText[indPixel * 4 + 2] = 1;
		offSetText[indPixel * 4 + 3] = 0;
	}

	glBindTexture(GL_TEXTURE_RECTANGLE, pg_hashtag_offset_texID);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA8,           // Internal colour format to convert to
		hashtag_pixel_length,         // Image width
		2,         // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		(GLvoid *)offSetText);        // The actual image data itself
	return hashtag_pixel_length;
}


///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)
///////////////////////////////////////////////////////

// the glut callback
// requires predrawing (drawing from the user to the root node)

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void window_display( void ) {
  windowDisplayed = true;

  // OpenGL initializations before redisplay
  OpenGLInit();

  // proper scene redrawing
  pg_draw_scene( _Render );

  // flushes OpenGL commands
  glFlush();

  glutSwapBuffers();
  //

  // ------------------------------------------------------------ //
  // --------------- FRAME/SUBFRAME GRABBING -------------------- //

  // ---------------- frame by frame output --------------------- //
  // Svg screen shots
  // printf("Draw Svg\n" );
  if( outputSvg && is_automatic_snapshots
	&& FrameNo % stepSvg == 0
	&& FrameNo / stepSvg >= beginSvg &&
	FrameNo / stepSvg <= endSvg ) {
    // printf("Draw Svg %d\n" , FrameNo  );
    pg_draw_scene( _Svg );
  }

  // ---------------- frame by frame output --------------------- //
  // Png screen shots
  // printf("Draw Png\n" );
  if( outputPng && is_automatic_snapshots
	&& FrameNo % stepPng == 0
	&& FrameNo / stepPng >= beginPng &&
	FrameNo / stepPng <= endPng ) {
    pg_draw_scene( _Png );
  }

  // ---------------- frame by frame output --------------------- //
  // Jpg screen shots
  //printf("Draw Jpg %d %d %d %d %d\n"  ,
	 // (int)outputJpg , (int)is_automatic_snapshots
	 // , stepJpg , beginJpg , endJpg);
  if( outputJpg && is_automatic_snapshots
	&& FrameNo % stepJpg == 0
	&& FrameNo / stepJpg >= beginJpg &&
	FrameNo / stepJpg <= endJpg ) {
    pg_draw_scene( _Jpg );
  }
}


//////////////////////////////////////////////////////
// SCENE UPDATING
//////////////////////////////////////////////////////

// generic interactive draw function (from the root)

// scene redisplay (controlled by a drawing mode: file, interactive
// or edition)

void pg_update_scene( void ) {
  ///////////////////////////////////////////////////////////////////////
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // +++ mouse pointer adjustment according to music   +++++++
  // +++ notice that there is one frame delay   ++++++++++++++
  // +++ for taking these values in consideration   ++++++++++
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  bool is_trackreplay = track_replay_0;
  if( !is_trackreplay ) {
    if( activeDrawingStroke > 0 ) {
      tracks_x[ 0 ] = mouse_x;
      tracks_y[ 0 ] = mouse_y;
      tracks_x_prev[ 0 ] = mouse_x_prev;
      tracks_y_prev[ 0 ] = mouse_y_prev;
    }
    else {
      tracks_x[ 0 ] = -1;
      tracks_y[ 0 ] = -1;
      tracks_x_prev[ 0 ] = -1;
      tracks_y_prev[ 0 ] = -1;
    }
  }
  // printf("Track %d %.1fx%.1f prev:  %.1fx%.1f\n",0,tracks_x[ 0 ],tracks_y[ 0 ],tracks_x_prev[ 0 ],tracks_y_prev[ 0 ]);
  //  <write_console value="Position: 0 activeDrawingStroke ({$config:track_replay[1]}) ({$config:tracks_BrushID[1]}) ({$config:tracks_RadiusX[1]}) ({$config:tracks_RadiusY[1]}) ({$config:tracks_x[1]}) ({$config:tracks_y[1]})"/>

  /////////////////////////////////////////////////////////////////////////
  // TRANSFER OF LOADED BUFFER IMAGES TO GPU
  // NEW: non threaded transfer to GPU
  GPUtransfer_image_buffer_data();

  /////////////////////////////////////////////////////////////////////////
  // AUDIO STREAMING
  // pg_audio_load_new_samples();
  /////////////////////////////////////////////////////////////////////////
  // AUDIO STREAMING
  // synthesis and streaming automatically called by pg_audio_paStreamCallback
  // pg_audio_streaming();

  /////////////////////////////////////////////////////////////////////////
  // IMAGE UPDATE
  // pg_load_new_images()
  printOglError(509);

  /////////////////////////////////////////////////////////////////////////
  // SHADER UNIFORM VARIABLES UPDATE
#include "pg_draw_body.cpp"

  printOglError(510);

  /////////////////////////////////////////////////////////////////////////
  // DRAWING SHADER UNIFORM VARIABLES
  glUseProgram(shader_Drawing_programme);

  // drawing off and drawing point or line
  glUniform4f(uniform_Drawing_fs_4fv_W_H_drawingStart_drawingStroke ,
	      (GLfloat)leftWindowWidth ,
	      (GLfloat)window_height ,
	      (GLfloat)drawing_start_frame , (GLfloat)activeDrawingStroke );
  // printf("W x H %d x %d\n", leftWindowWidth,window_height);

  // image buffer layer weights
  centralImage += imageJitterAmpl * fabs(float((double)rand()/(double)RAND_MAX-0.5));
  while (centralImage < 0) centralImage += PG_IMAGE_NB_TEXTURES;
  while (centralImage >= PG_IMAGE_NB_TEXTURES) centralImage -= PG_IMAGE_NB_TEXTURES;
  // float images_weights_variance = 10.0;
  float dist[PG_IMAGE_NB_TEXTURES];
  for (int ind = 0; ind < PG_IMAGE_NB_TEXTURES; ind++) {
	  //// the new value obtained by a gaussian around the central standard value
	  dist[ind] = Min(Min(fabs(ind - centralImage), fabs(ind + PG_IMAGE_NB_TEXTURES - centralImage)),fabs(ind - PG_IMAGE_NB_TEXTURES - centralImage));
	  image_buffer_weights[ind] = max(0.f,0.2f * (exp(-(dist[ind]*dist[ind])/mask_variance) - 0.5f));
	  // std value variance 36.f
	  image_buffer_weights[ind] = .3f;

//	  float center = ((float)(ind + 1) / PG_IMAGE_NB_TEXTURES) * images_weights_scale;
//	  float newValue = center + images_weights_rand * (float)((double)rand() / (RAND_MAX)-0.5);
//	  // the new value obtained randomly around the central standard value
//	  // should not be lower than the preceding one (they are given by increasing order)
//	  newValue = std::max(newValue, (ind > 0 ? image_buffer_weights[ind - 1] : 0.0f));
//	  image_buffer_weights[ind]
//		  = std::min(newValue, (ind < PG_IMAGE_NB_TEXTURES - 1 ? image_buffer_weights[ind + 1] : 1.0f));
  }
//  printf("CI %.2f W %.2f %.2f %.2f %.2f %.2f %.2f\n",centralImage,image_buffer_weights[0],image_buffer_weights[1],image_buffer_weights[2],image_buffer_weights[3],image_buffer_weights[4],image_buffer_weights[5]);
//  printf("CI %.2f D %.2f %.2f %.2f %.2f %.2f %.2f\n\n",centralImage,dist[0],dist[1],dist[2],dist[3],dist[4],dist[5]);

  glUniform4fv(uniform_Drawing_fs_4fv_weights03, 1, image_buffer_weights);
  glUniform2fv(uniform_Drawing_fs_2fv_weights45, 1, image_buffer_weights + 4);

  // interpolation weight between image buffer swap buffer in each la
  glUniform3fv(uniform_Drawing_fs_3fv_alphaSwap02, 1, image_buffer_alphaSwap02);
  glUniform3fv(uniform_Drawing_fs_3fv_alphaSwap35, 1, image_buffer_alphaSwap35);
    //  printf("swap1 %.1f %.1f %.1f\n", image_buffer_alphaSwap02[0], image_buffer_alphaSwap02[1],
    //	  image_buffer_alphaSwap02[2]);
    //  printf("swap2 %.1f %.1f %.1f\n", image_buffer_alphaSwap35[0], image_buffer_alphaSwap35[1],
    //	  image_buffer_alphaSwap35[2]);

  //for (int ind = 0; ind < 3; ind++) {
	 // image_buffer_alphaSwap02[ind] += 0.001f;
	 // image_buffer_alphaSwap35[ind] += 0.001f;
  //}
  // printf("Jitter %.2f %.2f\n" ,maskJitterAmpl ,maskJitterPhase );
  // image buffer layer and masks coordinate offsets
  for(int ind = 0 ; ind < PG_IMAGE_NB_TEXTURES*2 ; ind++ ) {
	  float imageCoordOffest = imageJitterAmpl * (float)((double)rand() / (double)RAND_MAX - 0.5);
	  float maskCoordOffest = maskJitterAmpl * float((double)rand() / (double)RAND_MAX - 0.5);
	  if (image_buffer_position_noises[ind] + imageCoordOffest < 2.f
		  && image_buffer_position_noises[ind] + imageCoordOffest > -2.f) {
		  image_buffer_position_noises[ind] += imageCoordOffest;
	  }
	  else {
		  image_buffer_position_noises[ind] -= imageCoordOffest;
	  }
	  if (mask_buffer_position_noises[ind] + maskCoordOffest < 2.f
		  && mask_buffer_position_noises[ind] + maskCoordOffest > -2.f) {
		  mask_buffer_position_noises[ind] += maskCoordOffest;
	  }
	  else {
		  mask_buffer_position_noises[ind] -= maskCoordOffest;
	  }
  }
  //printf("noises %.1f %.1f %.1f %.1f\n", mask_buffer_position_noises[0], mask_buffer_position_noises[1],
	 // mask_buffer_position_noises[2], mask_buffer_position_noises[3]);
  //printf("noises %.1f %.1f %.1f %.1f\n", mask_buffer_position_noises[4], mask_buffer_position_noises[5],
	 // mask_buffer_position_noises[6], mask_buffer_position_noises[7]);
  //printf("noises %.1f %.1f %.1f %.1f\n\n", mask_buffer_position_noises[8], mask_buffer_position_noises[9],
	 // mask_buffer_position_noises[10], mask_buffer_position_noises[11]);
  glUniform4fv( uniform_Drawing_fs_4fv_image_noisesxy , 3 , image_buffer_position_noises );
  glUniform4fv( uniform_Drawing_fs_4fv_mask_noisesxy, 3, mask_buffer_position_noises );

#ifdef PG_WITH_CA
  // CA Parameters
//  glUniform4f(uniform_Drawing_fs_4fv_CAdecay_clearCA_flashCACoefs,
//	  CAdecay * CAdecay_sign,
//	  (GLfloat)isClearCA, flashCA_weights[0], flashCA_weights[1]);
#endif

// track decay
  glUniform3f(uniform_Drawing_fs_3fv_clearLayer_flashVideoCoef_flashVideoThreshold , (GLfloat)isClearLayer ,
	  flashVideoCoef , flashVideoThreshold);
  //if (flashVideoCoef > 0.0f) {
	 // printf("************ flashVideoCoef %.2f %.2f\n\n", flashVideoCoef , flashVideoLength);
  //}

  // decay, flash
#if PG_NB_TRACKS >= 3
  glUniform4f(uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 ,
	      (GLfloat)flashPart, flashTrack0_weights[1] , flashTrack0_weights[2] , (GLfloat)isCopyTrack12 );
#else
  glUniform4f(uniform_Drawing_fs_4fv_flashPart_flashTrack0Coefs_copyTrack12 ,
	      (GLfloat)flashPart, flashTrack0_weights[1] , 0.0f , (GLfloat)isCopyTrack12 );
#endif

//  glUniform4f(uniform_Drawing_fs_4fv_mouseTracks_xy_prevxy, tracks_x[0], tracks_y[0], tracks_x_prev[0], tracks_y_prev[0]);

#ifdef PG_VIDEO_ACTIVE
  if (pg_video_frame.data) {
	  glUniform4f(uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold,
		  flashBack_weight, (GLfloat)pg_video_frame.cols, (GLfloat)pg_video_frame.rows, videoThreshold);
  }
  else {
	  glUniform4f(uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold,
		  flashBack_weight, 640, 480, 1.0f);
 }
#else
  glUniform4f(uniform_Drawing_fs_4fv_flashBackCoef_videoWH_videoThreshold,
	  flashBack_weight, 640 , 480 , 1.0f );
#endif
  // pen color
  glUniform4f(uniform_Drawing_fs_4fv_mouseTracks_Color_rgba,
	  tracks_Color_r[0], tracks_Color_g[0], tracks_Color_b[0], tracks_Color_a[0]);
  glUniform4f(uniform_Drawing_fs_4fv_partAcc_clearAllLayers_frameNo_time ,
	  part_acc_factor, (GLfloat)isClearAllLayers, (float)FrameNo, (float)CurrentClockTime );
  // printf("Repopulating CA/BG %.2f %.2f\n",repopulatingCA, repopulatingBG);

  // pen brush & size & pulse
//  glUniform4f(uniform_Drawing_fs_4fv_mouseTracks_RadiusXY_isNewBeat_Cursor, (GLfloat)tracks_RadiusX[0],
//				(GLfloat)tracks_RadiusY[0], (float)isNewBeat, (GLfloat)CurrentCursorStylusvsRubber);
  glUniform4f(uniform_Drawing_fs_4fv_pulse, pulse[0], pulse[1], pulse[2] , (pulse[0]+ pulse[1]+ pulse[2]) / 3.0f);

  //if (FrameNo % 30 == 0) {
	 // printf("Pulse %.2f %.2f %.2f\n", pulse[0] , pulse[1] , pulse[2]);
  //}
  // printf("Track radius x %.2f %.2f %.2f\n", tracks_RadiusX[0], tracks_RadiusX[1], tracks_RadiusX[2]);

  printOglError(511);

  /////////////////////////////////////////////////////////////////////////
  // COMPOSITION SHADER UNIFORM VARIABLES

  glUseProgram(shader_Composition_programme);

   glUniform4f(uniform_Composition_fs_4fv_height_screenMsgTransp_Text1_2_Alpha ,
	  (GLfloat)window_height, ScreenMessageTransparency,
	  (GLfloat)DisplayText1Alpha, (GLfloat)DisplayText2Alpha);
 // printf("DisplayText1Alpha %.2f DisplayText2Alpha %.2f %d %d\n", DisplayText1Alpha, DisplayText2Alpha,CurrentScene,NbTotScenes);

   glUniform3f(uniform_Composition_fs_3fv_flashVideoCoef_flashVideoThreshold_width,
	   flashVideoCoef, flashVideoThreshold, (GLfloat)leftWindowWidth);

  printOglError(512);

  /////////////////////////////////////////////////////////////////////////
  // FINAL SHADER UNIFORM VARIABLES
  glUseProgram(shader_Final_programme);
  // hoover cursor
  glUniform3f(uniform_Final_fs_3fv_xy_frameno ,
	      (GLfloat)CurrentCursorHooverPos_x, (GLfloat)CurrentCursorHooverPos_y,
		  (GLfloat)FrameNo );
  // screen size
  glUniform2f(uniform_Final_fs_2fv_width_height ,
	      (GLfloat)leftWindowWidth, (GLfloat)window_height);
 // printf("hoover %d %d\n",CurrentCursorHooverPos_x, CurrentCursorHooverPos_y);


  ///////////////////////////////////////////////////////////////////////
  // saves mouse values
  mouse_x_prev = mouse_x;
  mouse_y_prev = mouse_y;

  ///////////////////////////////////////////////////////////////////////
  // flash reset: restores flash to 0 so that
  // it does not stay on more than one frame
  for( int indtrack = 0 ; indtrack < PG_NB_TRACKS ; indtrack++ ) {
#ifdef PG_WITH_CA
	  flashCA_weights[indtrack] = 0;
#endif
    flashTrack0_weights[indtrack] = 0;
  }

  if( flashPart > 0 ) {
    flashPart -= 1;
  }
#ifdef PG_WITH_CA
  flashBack_weight = 0;
#endif

  ////////////////////////////
  // flash video reset
  if (flashVideoCoef > 0.0f) {
	  if (flashVideoCoef - flashVideoDecay > 0) {
		  flashVideoCoef -= flashVideoDecay;
	  }
	  else {
		  flashVideoCoef = 0.0f;
	  }
	  // printf("coef %.3f\n", flashVideoCoef);
  }

  // /////////////////////////
  // clear layer reset
  isClearAllLayers = 0;
#ifdef PG_WITH_CA
  // clear CA reset
  isClearCA = 0;
#endif
  // clear layer reset
  isClearLayer = 0;
  // layer copy reset
  isCopyTrack12 = 0;
  // new beat
  isNewBeat = 0;

  printOglError(512);

}

//////////////////////////////////////////////////////
// SCENE RENDERING
//////////////////////////////////////////////////////

void pg_draw_scene( DrawingMode mode ) {
  // ******************** Svg output ********************
  if( mode == _Svg ) {
    threadWriteImageData *pData = new threadWriteImageData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;

    sprintf( pData->fname , "%s%s-%s-%04d.svg" ,
		snapshots_dir_path_name.c_str(),
		Svg_file_name.c_str() ,
	     date_stringStream.str().c_str() ,
	     FrameNo / stepSvg );
    fprintf( fileLog , "Snapshot svg step %d (%s)\n" ,
	     FrameNo / stepSvg ,
	     pData->fname );
#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            writesvg,		    // thread function name
            (void *)pData,		    // argument to thread function
            0,                      // use default creation flags
            &rc);   // returns the thread identifier
   if (hThread == NULL){
         std::cout << "Error:unable to create thread writesvg" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL,
                          writesvg, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread writesvg" << rc << std::endl;
		 pthread_exit(NULL);
   }
#endif
  }

  // ******************** Png output ********************
  else if( mode == _Png ) {
    threadWriteImageData *pData = new threadWriteImageData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->img = new cv::Mat( pData->h, pData->w, CV_8UC3 );

    sprintf( pData->fname , "%s%s-%s-%04d.png" ,
		snapshots_dir_path_name.c_str(),
		Png_file_name.c_str() ,
	     date_stringStream.str().c_str() ,
	     FrameNo / stepPng );
	struct stat buffer;
	int count = 0;
	while (stat(pData->fname, &buffer) == 0) {
		sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
			snapshots_dir_path_name.c_str(),
			Png_file_name.c_str(),
			date_stringStream.str().c_str(),
			FrameNo / stepPng, count);
		count++;
	}
	pg_logCurrentLineSceneVariables(pData->fname);
	printf("Snapshot png step %d (%s)\n",
		FrameNo / stepPng,
		pData->fname);

	glReadBuffer(GL_FRONT);

    // OpenGL's default 4 byte pack alignment would leave extra bytes at the
    //   end of each image row so that each full row contained a number of bytes
    //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
    //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
    //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
    //   the rows are packed as tight as possible (no row padding), set the pack
    //   alignment to 1.
	// this part cannot be threaded because it has to be made with
	// the content of a single frame
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->img->data);

#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            threaded_write_png,		    // thread function name
            (void *)pData,		    // argument to thread function
            0,                      // use default creation flags
            &rc);   // returns the thread identifier
   if (hThread == NULL){
         std::cout << "Error:unable to create thread threaded_write_png" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL,
                          threaded_write_png, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread threaded_write_png" << rc << std::endl;
		 pthread_exit(NULL);
   }
#endif
  }

  // ******************** Jpg output ********************
  else if( mode == _Jpg ) {
    threadWriteImageData *pData = new threadWriteImageData;
    pData->fname = new char[512];
    pData->w = leftWindowWidth;
    pData->h = window_height;
    pData->img = new cv::Mat( pData->h, pData->w, CV_8UC3 );

    sprintf( pData->fname , "%s%s-%s-%04d.jpg" ,
		snapshots_dir_path_name.c_str(),
		Jpg_file_name.c_str() ,
	     date_stringStream.str().c_str() ,
	     FrameNo / stepJpg );
	struct stat buffer;
	int count = 0;
	while (stat(pData->fname, &buffer) == 0) {
		sprintf(pData->fname, "%s%s-%s-%04d-%03d.jpg",
			snapshots_dir_path_name.c_str(),
			Jpg_file_name.c_str(),
			date_stringStream.str().c_str(),
			FrameNo / stepJpg, count);
		count++;
	}
	pg_logCurrentLineSceneVariables(pData->fname);
	printf("Snapshot jpg step %d (%s)\n",
		FrameNo / stepJpg,
		pData->fname);

	glReadBuffer(GL_FRONT);

    // OpenGL's default 4 byte pack alignment would leave extra bytes at the
    //   end of each image row so that each full row contained a number of bytes
    //   divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
    //   be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
    //   just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
    //   the rows are packed as tight as possible (no row padding), set the pack
    //   alignment to 1.
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, pData->w, pData->h, GL_RGB, GL_UNSIGNED_BYTE, pData->img->data);

#ifdef WIN32
   DWORD rc;
   HANDLE  hThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            threaded_write_jpg,		    // thread function name
            (void *)pData,		    // argument to thread function
            0,                      // use default creation flags
            &rc);   // returns the thread identifier
   if (hThread == NULL){
         std::cout << "Error:unable to create thread threaded_write_jpg" << std::endl;
         exit(-1);
   }
   CloseHandle(hThread);
#else
   pthread_t drawing_thread;
   int rc;
   rc = pthread_create(&drawing_thread, NULL,
                          threaded_write_jpg, (void *)pData);
   if (rc){
         std::cout << "Error:unable to create thread threaded_write_jpg" << rc << std::endl;
		 pthread_exit(NULL);
   }
#endif
  }

  //// ******************** scenario output ********************
  //if( mode == _Svg || mode == _Png || mode == _Jpg ) {
  //    /////////////////////////////////////////////////////////////////////////
  //  // SCENARIO-CONTROLLED UNIFORM VARIABLES
  //  for( int indVar = 0 ; indVar < _MaxInterpVarIDs ; indVar++ ) {
  //    if (ScenarioVarTypes[indVar] == _pg_float) {
	 //   fprintf( fileLog , "%s %.4f\n" , CmdString[indVar] , *((float *)ScenarioVarPointers[indVar]) );
  //    }
  //    else if (ScenarioVarTypes[indVar] == _pg_int) {
	 //   fprintf( fileLog , "%s %d\n" , CmdString[indVar] , *((int *)ScenarioVarPointers[indVar]) );
  //    }
  //    else if (ScenarioVarTypes[indVar] == _pg_bool) {
	 //   fprintf( fileLog , "%s %d\n" , CmdString[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) );
  //    }
  //    else if (ScenarioVarTypes[indVar] == _pg_sign) {
	 //   fprintf( fileLog , "%s %.4f\n" , CmdString[indVar] , *((float *)ScenarioVarPointers[indVar]) );
  //    }
  //    else if (ScenarioVarTypes[indVar] == _pg_path) {
	 //   fprintf( fileLog , "%s %d\n" , CmdString[indVar] , (int)(*((bool *)ScenarioVarPointers[indVar])) );
  //    }
  //  }
  //  fprintf( fileLog , "\n");
  //}

  // ******************** Video output ********************
#ifdef PG_HAVE_FFMPEG
  else if( mode == _Video
	   && PG_EnvironmentNode->audioVideoOutData ) {
    glReadBuffer(GL_BACK);
    PG_EnvironmentNode->audioVideoOutData
      ->write_frame();
  }
#endif

  // ******************** interactive output ********************
  // ********************  (OpenGL or evi3d) ********************
  else if( mode == _Render ) {

    //////////////////////////////////////////////////
    // SCENE UPDATE
    pg_update_scene();

    printOglError( 51 );

    //////////////////////////////////////////////////
    // PASS #1: PING PONG DRAWING

    // sets viewport to single window
    glViewport (0, 0, leftWindowWidth , window_height );

    // ping pong output and input FBO bindings
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_Drawing_BGParticlesCATracks[(FrameNo % PG_NB_MEMORIZED_PASS)]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_Drawing_BGParticlesCATracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);

    // output buffer cleanup
    glDisable( GL_DEPTH_TEST );

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    ////////////////////////////////////////
    // activate shaders and sets uniform variable values
    glUseProgram (shader_Drawing_programme);
    glBindVertexArray (quadDraw_vao);
    glUniformMatrix4fv(uniform_Drawing_vp_proj, 1, GL_FALSE, projMatrix);
    glUniformMatrix4fv(uniform_Drawing_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Drawing_vp_model, 1, GL_FALSE, modelMatrix);

    // texture unit location
    glUniform1i(uniform_Drawing_texture_fs_decal, 0);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable1, 1);
    // glUniform1i(uniform_Drawing_texture_fs_lookupTable2, 2);
#if PG_NB_TRACKS >= 2
    glUniform1i(uniform_Drawing_texture_fs_lookupTable3, 3);
#endif
#if PG_NB_TRACKS >= 3
    glUniform1i(uniform_Drawing_texture_fs_lookupTable4, 4);
#endif
//    glUniform1i(uniform_Drawing_texture_fs_lookupTable5, 5);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable6, 6);
#ifdef PG_VIDEO_ACTIVE
    glUniform1i(uniform_Drawing_texture_fs_lookupTable7, 7);
//    glUniform1i(uniform_Drawing_texture_fs_lookupTable8, 8);
//    glUniform1i(uniform_Drawing_texture_fs_lookupTable9, 9);
#endif
    glUniform1i(uniform_Drawing_texture_fs_lookupTable10, 10);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable11, 11);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable12, 12);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable13, 13);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable14, 14);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable15, 15);
    glUniform1i(uniform_Drawing_texture_fs_lookupTable16, 16);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable17, 17);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable17, 17);
	//glUniform1i(uniform_Drawing_texture_fs_lookupTable18, 18);
	//glUniform1i(uniform_Drawing_texture_fs_lookupTable19, 19);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable20, 20);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable21, 21);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable22, 22);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable23, 23);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable24, 24);
	glUniform1i(uniform_Drawing_texture_fs_lookupTable25, 25);

    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );

    // 2-cycle ping-pong BG track step n (FBO attachment 1)
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS]);

    // 2-cycle ping-pong speed/position of particles step n step n (FBO attachment 2)
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 1]);

#ifdef PG_WITH_CA
//	// 2-cycle ping-pong CA step n (FBO attachment 3)
//	glActiveTexture(GL_TEXTURE0 + 2);
//	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 4]);
#endif
#else
#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[(FrameNo % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif
#endif


    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//    // pen patterns
//    glActiveTexture(GL_TEXTURE0 + 5);
//    glBindTexture(GL_TEXTURE_3D, Pen_texture_3D_texID );

    // particle acceleration texture
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindTexture(GL_TEXTURE_3D, Particle_acceleration_texture_3D );

#ifdef PG_VIDEO_ACTIVE
    // video texture
    glActiveTexture(GL_TEXTURE0 + 7);
    glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_texture_texID);
    if (FrameNo % 2 == 0) { // currentVideoTrack <=> video off
      // frame capture
      // printf("frame capture\n");
		pg_input_camera >> pg_video_frame;
		if (pg_video_frame.data) {
		  glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			  0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			  GL_RGB,            // Internal colour format to convert to
			  pg_video_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
			  pg_video_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
			  0,                 // Border width in pixels (can either be 1 or 0)
			  GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			  GL_UNSIGNED_BYTE,  // Image data type
			  pg_video_frame.data);        // The actual image data itself
		}
    }

//    // current background texture
//    glActiveTexture(GL_TEXTURE0 + 8);
//    glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_currentBG_texture_texID);
//    if (FrameNo % 2 == 0 && is_currentBG_capture ) { // currentVideoTrack <=> video off
//      // printf("current BG video capture\n");
//      is_currentBG_capture = false;
//      // current background memory
//      glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
//	      0,                 // Pyramid level (for mip-mapping) - 0 is the top level
//	      GL_RGB,            // Internal colour format to convert to
//	      pg_video_frame->width,          // Image width  i.e. 640 for Kinect in standard mode
//	      pg_video_frame->height,          // Image height i.e. 480 for Kinect in standard mode
//	      0,                 // Border width in pixels (can either be 1 or 0)
//	      GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
//	      GL_UNSIGNED_BYTE,  // Image data type
//	      pg_video_frame->imageData);        // The actual image data itself
//    }
//
//    // initial background texture
//    glActiveTexture(GL_TEXTURE0 + 9);
//    glBindTexture(GL_TEXTURE_RECTANGLE, pg_video_initialBG_texture_texID);
//    if (FrameNo % 2 == 0 && is_initialBG_capture ) { // currentVideoTrack <=> video off
//      // printf("initial BG video capture\n");
//      is_initialBG_capture = false;
//      // initial background memory
//      glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
//	      0,                 // Pyramid level (for mip-mapping) - 0 is the top level
//	      GL_RGB,            // Internal colour format to convert to
//	      pg_video_frame->width,          // Image width  i.e. 640 for Kinect in standard mode
//	      pg_video_frame->height,          // Image height i.e. 480 for Kinect in standard mode
//	      0,                 // Border width in pixels (can either be 1 or 0)
//	      GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
//	      GL_UNSIGNED_BYTE,  // Image data type
//	      pg_video_frame->imageData);        // The actual image data itself
//    }
#endif
    // image buffer texture #0
    glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[0].texBuffID);
    // image buffer texture #1
    glActiveTexture(GL_TEXTURE0 + 11);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[1].texBuffID);
    // image buffer texture #2
    glActiveTexture(GL_TEXTURE0 + 12);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[2].texBuffID);
    // image buffer texture #3
    glActiveTexture(GL_TEXTURE0 + 13);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[3].texBuffID);
    // image buffer texture #4
    glActiveTexture(GL_TEXTURE0 + 14);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[4].texBuffID);
    // image buffer texture #5
    glActiveTexture(GL_TEXTURE0 + 15);
    glBindTexture(GL_TEXTURE_RECTANGLE, image_buffer_data[5].texBuffID);
	// image buffer mask #0
	glActiveTexture(GL_TEXTURE0 + 16);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_mask_buffer_data[0].texBuffID);
	// image buffer mask #1
	glActiveTexture(GL_TEXTURE0 + 17);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_mask_buffer_data[1].texBuffID);
	//// image buffer mask #2
	//glActiveTexture(GL_TEXTURE0 + 18);
	//glBindTexture(GL_TEXTURE_RECTANGLE, image_mask_buffer_data[2].texBuffID);
	//// image buffer mask #3
	//glActiveTexture(GL_TEXTURE0 + 19);
	//glBindTexture(GL_TEXTURE_RECTANGLE, image_mask_buffer_data[3].texBuffID);
	// buffer swap image #0
	glActiveTexture(GL_TEXTURE0 + 20);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[0].texBuffID);
	// buffer swap image #1
	glActiveTexture(GL_TEXTURE0 + 21);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[1].texBuffID);
	// buffer swap image #2
	glActiveTexture(GL_TEXTURE0 + 22);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[2].texBuffID);
	// buffer swap image #3
	glActiveTexture(GL_TEXTURE0 + 23);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[3].texBuffID);
	// buffer swap image #4
	glActiveTexture(GL_TEXTURE0 + 24);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[4].texBuffID);
	// buffer swap image #5
	glActiveTexture(GL_TEXTURE0 + 25);
	glBindTexture(GL_TEXTURE_RECTANGLE, image_swap_buffer_data[5].texBuffID);

    // draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);

    printOglError( 52 );

    //////////////////////////////////////////////////
    // PASS #2: COMPOSITION + PING-PONG ECHO

    // binds input to last output
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO_Drawing_BGParticlesCATracks[((FrameNo + 1) % PG_NB_MEMORIZED_PASS)]);

    /////////////////////////////////////////////////////////
    // draws the main rectangular surface with

    // outputs inside a buffer that can be used for accumulation
    if( FrameNo > 0 ) {
      //if (is_capturedFB) {
	     // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_Composition_capturedFB); //  frame memory for frame subtraction or old frame display
	     // is_capturedFB = false;
      //}
      //else
      //{
	      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_Composition_capturedFB_prec[(FrameNo % 2)]); //  drawing memory on odd and even frames for echo and sensors
      //}
    }

    // output video buffer clean-up
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    /////////////////////////////////////////////////////////
    // draws the main rectangular surface with

    ////////////////////////////////////////
    // activate shaders and sets uniform variable values
    glUseProgram (shader_Composition_programme);
    glBindVertexArray (quadDraw_vao);

    glUniformMatrix4fv(uniform_Composition_vp_proj, 1, GL_FALSE, projMatrix);
    glUniformMatrix4fv(uniform_Composition_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Composition_vp_model, 1, GL_FALSE, modelMatrix);

    // texture unit location
    glUniform1i(uniform_Composition_texture_fs_decal, 0);
//     glUniform1i(uniform_Composition_texture_fs_lookupTable1, 1);
#if PG_NB_TRACKS >= 2
    glUniform1i(uniform_Composition_texture_fs_lookupTable2, 2);
#endif
#if PG_NB_TRACKS >= 3
    glUniform1i(uniform_Composition_texture_fs_lookupTable3, 3);
#endif
    glUniform1i(uniform_Composition_texture_fs_lookupTable4, 4);
    //glUniform1i(uniform_Composition_texture_fs_lookupTable5, 5);

	glUniform1i(uniform_Composition_texture_fs_lookupTable_screen_font, 6);
	glUniform1i(uniform_Composition_texture_fs_lookupTable_screen_msg_offset, 7);

	glUniform1i(uniform_Composition_texture_fs_lookupTable_displayMsg_font, 8);
	glUniform1i(uniform_Composition_texture_fs_lookupTable_displayMsg1_offset, 9);
	glUniform1i(uniform_Composition_texture_fs_lookupTable_displayMsg2_offset, 10);

	glUniform1i(uniform_Composition_texture_fs_lookupTable_hashtag_font, 11);
	glUniform1i(uniform_Composition_texture_fs_lookupTable_hashtag_offset, 12);

	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    // 2-cycle ping-pong BG track step n + 1 (FBO attachment 1)
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS]);

#ifdef PG_WITH_CA
    // 2-cycle ping-pong CA step n + 1 (FBO attachment )
     glActiveTexture(GL_TEXTURE0 + 1);
     glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);

#if PG_NB_TRACKS >= 2
    // 2-cycle ping-pong track 1 step n + 1 (FBO attachment 2)
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif

#if PG_NB_TRACKS >= 3
    // 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 4]);
#endif
#else
#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n + 1 (FBO attachment 2)
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n + 1 (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif
#endif

    // preceding snapshot
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Composition_capturedFB_prec_texID[((FrameNo + 1) % 2)] );  // drawing memory on odd and even frames for echo

    // captured frame buffer texture
    //glActiveTexture(GL_TEXTURE0 + 5);
    //glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Composition_capturedFB_texID);

	// small system message font texture
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_RECTANGLE, Screen_Font_texture_texID);

	// small system message texture
	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_screen_msg_offset_texID);

	// large system message font texture
	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_RECTANGLE, Display_Font_texture_texID);

	// large system message texture
	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_displayMsg1_offset_texID);

	// large system message texture
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_displayMsg2_offset_texID);

	// hashtag font texture
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_RECTANGLE, Hashtag_Font_texture_texID);

	// hashtag message texture
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_hashtag_offset_texID);

	// draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);


#ifdef PG_TRACK_DISPLAY
    ////////////////////////////////////////
    // draws the lines of the tracks that are displayed

    // first checks whether one of the tracks is displayed
    bool oneTrackDisplayed = false;
    for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
      // printf("displaying track %d %d\n" , line_tracks_points[indTrack] , source_track_display[indTrack] );
      bool is_source_track_display = false;
      switch( indTrack ) {
	case 0:
	  is_source_track_display = source_track_display_0;
	  break;
	case 1:
	  is_source_track_display = source_track_display_1;
	  break;
	case 2:
	  is_source_track_display = source_track_display_2;
	  break;
      }
      if (line_tracks_points[indTrack] != 0  && is_source_track_display ) {
	oneTrackDisplayed = true;
	break;
      }
      if( line_tracks_target_points[indTrack] != 0  && target_track_display[indTrack] ) {
	oneTrackDisplayed = true;
	break;
      }
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO_TrackDisplay);

    // output video buffer clean-up
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    glEnable( GL_BLEND );
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    if( oneTrackDisplayed ) {
      // printf("one track displayed\n");
      ////////////////////////////////////////
      // activate shaders and sets uniform variable values
      glUseProgram (shader_TrackDisplay_programme);

      for( int indTrack = 0 ; indTrack < PG_NB_TRACKS ; indTrack++ ) {
        bool is_source_track_display = false;
        switch( indTrack ) {
	  case 0:
	    is_source_track_display = source_track_display_0;
	    break;
	  case 1:
	    is_source_track_display = source_track_display_1;
	    break;
	  case 2:
	    is_source_track_display = source_track_display_2;
	    break;
	}
	if( line_tracks_points[indTrack] != 0 && is_source_track_display ) {
	  float semiWidth = leftWindowWidth / 2.0f;
	  float semiHeight = window_height / 2.0f;

          // printf("rendering track %d\n" , indTrack);
          glBindVertexArray (line_tracks_vao[indTrack]);

	  // transformations
	  /*
	  switch( indTrack ) {
	   case 0:
    	      modelMatrixDisplayTracks[0] = track_display_scale_0 * cos( track_display_rot_0 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[1] = track_display_scale_0 * sin( track_display_rot_0 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[4] = -track_display_scale_0 * sin( track_display_rot_0 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[5] = track_display_scale_0 * cos( track_display_rot_0 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[10] = track_display_scale_0;

	      modelMatrixDisplayTracks[12] = track_display_x_transl_0
		- semiWidth * modelMatrixDisplayTracks[0]
		- semiHeight * modelMatrixDisplayTracks[4]
		+ semiWidth;
	      modelMatrixDisplayTracks[13] = track_display_y_transl_0
		- semiWidth * modelMatrixDisplayTracks[1]
		- semiHeight * modelMatrixDisplayTracks[5]
		+ semiHeight;
	      break;
	   case 1:
    	      modelMatrixDisplayTracks[0] = track_display_scale_1 * cos( track_display_rot_1 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[1] = track_display_scale_1 * sin( track_display_rot_1 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[4] = -track_display_scale_1 * sin( track_display_rot_1 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[5] = track_display_scale_1 * cos( track_display_rot_1 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[10] = track_display_scale_1;

	      modelMatrixDisplayTracks[12] = track_display_x_transl_1
		- semiWidth * modelMatrixDisplayTracks[0]
		- semiHeight * modelMatrixDisplayTracks[4]
		+ semiWidth;
	      modelMatrixDisplayTracks[13] = track_display_y_transl_1
		- semiWidth * modelMatrixDisplayTracks[1]
		- semiHeight * modelMatrixDisplayTracks[5]
		+ semiHeight;
	      break;
	   case 2:
    	      modelMatrixDisplayTracks[0] = track_display_scale_2 * cos( track_display_rot_2 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[1] = track_display_scale_2 * sin( track_display_rot_2 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[4] = -track_display_scale_2 * sin( track_display_rot_2 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[5] = track_display_scale_2 * cos( track_display_rot_2 * (float)M_PI / 180.0f );
	      modelMatrixDisplayTracks[10] = track_display_scale_2;

	      modelMatrixDisplayTracks[12] = track_display_x_transl_2
		- semiWidth * modelMatrixDisplayTracks[0]
		- semiHeight * modelMatrixDisplayTracks[4]
		+ semiWidth;
	      modelMatrixDisplayTracks[13] = track_display_y_transl_2
		- semiWidth * modelMatrixDisplayTracks[1]
		- semiHeight * modelMatrixDisplayTracks[5]
		+ semiHeight;
	      break;
	  }
	 //printf("rot %.1f %.1f %.1f %.1f transl %.1f %.1f \n" ,
	 //  modelMatrixDisplayTracks[0],modelMatrixDisplayTracks[1],modelMatrixDisplayTracks[4],modelMatrixDisplayTracks[5],
	 //  modelMatrixDisplayTracks[12],modelMatrixDisplayTracks[13]);
	 */

          glUniformMatrix4fv(uniform_TrackDisplay_gs_proj, 1, GL_FALSE, projMatrixDisplayTracks);
          glUniformMatrix4fv(uniform_TrackDisplay_gs_view, 1, GL_FALSE, viewMatrix);
          glUniformMatrix4fv(uniform_TrackDisplay_gs_model, 1, GL_FALSE, modelMatrixDisplayTracks);

          //glUniformMatrix4fv(uniform_TrackDisplay_gs_proj, 1, GL_FALSE, projMatrix);
          //glUniformMatrix4fv(uniform_TrackDisplay_gs_view, 1, GL_FALSE, viewMatrix);
          //glUniformMatrix4fv(uniform_TrackDisplay_gs_model, 1, GL_FALSE, modelMatrix);

	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
          // texture unit location
          glUniform1i(uniform_TrackDisplay_texture_vp_lookupTable1, 0);
           // position - color - (radius, brush, rendering mode) texture (VP)
          glActiveTexture(GL_TEXTURE0 + 0);
          glBindTexture(GL_TEXTURE_1D, pg_tracks_Pos_Texture_texID[indTrack] );

          // texture unit location
          glUniform1i(uniform_TrackDisplay_texture_vp_lookupTable2, 1);
          // position - color - (radius, brush, rendering mode) texture (VP)
          glActiveTexture(GL_TEXTURE0 + 1);
          glBindTexture(GL_TEXTURE_1D, pg_tracks_Col_Texture_texID[indTrack] );

          // texture unit location
          glUniform1i(uniform_TrackDisplay_texture_vp_lookupTable3, 2);
          // position - color - (radius, brush, rendering mode) texture (VP)
          glActiveTexture(GL_TEXTURE0 + 2);
          glBindTexture(GL_TEXTURE_1D, pg_tracks_RadBrushRendmode_Texture_texID[indTrack] );

          // texture unit location
          // glUniform1i(uniform_TrackDisplay_texture_vp_lookupTable4, 3);
          // noise texture (VP)
          // glActiveTexture(GL_TEXTURE0 + 3);
	  // glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
	  // glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
          // glBindTexture(GL_TEXTURE_RECTANGLE, trackNoise_texture_Rectangle );

	  // VP uniform variables
	  glUniform4f(uniform_TrackDisplay_vp_4fv_width_FrameNo_noiseAmplitude_globalRadius ,
	      (GLfloat)max_mouse_recording_frames ,
	      (GLfloat)FrameNo, 0.0f , 5.0f);

	  // FS uniform variables
	  switch( indTrack ) {
	   case 0:
	      glUniform1f(uniform_TrackDisplay_fs_1fv_transparency, track_display_weight_0 );
	      break;
	   case 1:
	      glUniform1f(uniform_TrackDisplay_fs_1fv_transparency, track_display_weight_1 );
	      break;
	   case 2:
	      glUniform1f(uniform_TrackDisplay_fs_1fv_transparency, track_display_weight_2 );
	      break;
	  }

 	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
         // texture unit location
          glUniform1i(uniform_TrackDisplay_texture_fs_decal, 4);
          // brush texture (FS)
          glActiveTexture(GL_TEXTURE0 + 4);
          glBindTexture(GL_TEXTURE_2D , trackBrush_texture_2D_texID );

	  // draw points from the currently bound VAO with current in-use shader
          glDrawArrays (GL_LINE_STRIP, 0, TrackStatus_source[ indTrack ].nbRecordedFrames );
          //}
        }
        if( line_tracks_target_vao[indTrack] != 0 ) {
	  // TODO
        }
      }
    }
#endif

    //////////////////////////////////////////////////
    // PASS #3: DISPLAY

    // unbind output FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // sets viewport to double window
    glViewport (0, 0, doubleWindowWidth , window_height );

    glDrawBuffer(GL_BACK);

    // output video buffer clean-up
    glClear (GL_COLOR_BUFFER_BIT);
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

    // printf("rendering\n" );

    ////////////////////////////////////////
    // activate shaders and sets uniform variable values
    glUseProgram (shader_Final_programme);
    glBindVertexArray (quadComposition_vao);

    glUniformMatrix4fv(uniform_Final_vp_proj, 1, GL_FALSE, doubleProjMatrix);
    glUniformMatrix4fv(uniform_Final_vp_view, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(uniform_Final_vp_model, 1, GL_FALSE, modelMatrix);

    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S , GL_CLAMP );
    glTexParameterf( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP );
    // texture unit locations
    glUniform1i(uniform_Final_texture_fs_decal, 0);
#ifdef PG_TRACK_DISPLAY
    glUniform1i(uniform_Final_texture_fs_lookupTable1, 1);
#endif
	glUniform1i(uniform_Final_texture_fs_lookupTable2, 2);
	// glUniform1i(uniform_Final_texture_fs_lookupTable3, 3);
#if PG_NB_TRACKS >= 2
	glUniform1i(uniform_Final_texture_fs_lookupTable4, 4);
#endif
#if PG_NB_TRACKS >= 3
	glUniform1i(uniform_Final_texture_fs_lookupTable5, 5);
#endif

	// Composition pass output (echoed composition of tracks)
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Composition_capturedFB_prec_texID[(FrameNo % 2)]);

#ifdef PG_TRACK_DISPLAY
	// display tracks output
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, FBO_TrackDisplay_texID);
#endif
	// 2-cycle ping-pong BG track step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS]);

#ifdef PG_WITH_CA
	// 2-cycle ping-pong CA step n (FBO attachment 3)
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);

#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 4]);
#endif
#else
#if PG_NB_TRACKS >= 2
	// 2-cycle ping-pong track 1 step n (FBO attachment 4)
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 2]);
#endif

#if PG_NB_TRACKS >= 3
	// 2-cycle ping-pong track 2 step n (FBO attachment 5)
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_RECTANGLE, FBO_Drawing_BGParticlesCATracks_texID[((FrameNo + 1) % PG_NB_MEMORIZED_PASS) * PG_NB_ATTACHMENTS + 3]);
#endif
#endif

	// draw points from the currently bound VAO with current in-use shader
    glDrawArrays (GL_TRIANGLES, 0, 3 * nbFaces);
    //}


    printOglError( 595 );
    glDisable( GL_BLEND );

  }


  // // flushes OpenGL commands
  // glFlush();

}

