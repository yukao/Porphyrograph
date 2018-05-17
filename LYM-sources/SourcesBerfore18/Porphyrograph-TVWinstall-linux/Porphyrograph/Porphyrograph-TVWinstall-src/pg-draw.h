/*! \file pg-draw.h
 *
 *
 *     File pg-draw.h
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

#ifndef PG_DRAW_H
#define PG_DRAW_H

#ifdef PG_WITH_CA
#define PG_NB_ATTACHMENTS (2 + PG_NB_TRACKS)
#else
#define PG_NB_ATTACHMENTS (1 + PG_NB_TRACKS)
#endif


#define   PG_NB_HASHTAGS 47
#define   PG_NB_DISPLAY_MESSAGES (4789-PG_NB_HASHTAGS)
#define   PG_NB_HASHTAG_CHARS 54

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
extern GLuint pg_screen_msg_offset_texID; // PG_NB_ATTACHMENTS=1
extern GLubyte *pg_screen_msg_offset_texture;

extern GLuint pg_displayMsg1_offset_texID;
extern GLubyte *pg_displayMsg1_offset_texture;
extern GLuint pg_displayMsg2_offset_texID;
extern GLubyte *pg_displayMsg2_offset_texture;

extern GLuint pg_hashtag_offset_texID;
extern GLubyte *pg_hashtag_offset_texture;
extern int hashtag_pixel_length;

extern GLuint pg_tracks_Pos_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_Col_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Col_target_Texture[PG_NB_TRACKS];
extern GLuint pg_tracks_RadBrushRendmode_target_Texture_texID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

extern GLuint pg_tracks_Pos_Col_RadBrushRendmode_target_Texture_ID[PG_NB_TRACKS];
extern GLfloat *pg_tracks_Pos_Col_RadBrushRendmode_target_Texture[PG_NB_TRACKS];

extern GLuint Screen_Font_texture_texID;
extern GLuint Display_Font_texture_texID;
extern GLuint Hashtag_Font_texture_texID;

extern GLuint Pen_texture_3D_texID;
extern GLuint Particle_acceleration_texture_3D;

////////////////////////////////////////
// large messages displayed on the bottom of the screen
extern string DisplayText1;
extern float DisplayText1Alpha;
extern float LengthDisplayText1;
extern int IndDisplayText1;

extern string DisplayText2;
extern float DisplayText2Alpha;
extern float LengthDisplayText2;
extern int IndDisplayText2;

extern bool DisplayText1Front;
extern double DisplayTextSwapInitialTime;

extern std::string* DisplayTextList;
extern int* DisplayTextFirstInChapter;
extern int NbDisplayTexts;

// message chapters
extern int TextChapter;
extern int NbTextChapters;


////////////////////////////////////////
// geometry: track lines
extern unsigned int line_tracks_vao[PG_NB_TRACKS];
extern unsigned int line_tracks_target_vao[PG_NB_TRACKS];

enum TextureEncoding{ JPG = 0 , PNG , PNGA , PNG_GRAY , PNGA_GRAY , RGB , RAW , EmptyTextureEncoding };
extern const char *TextureEncodingString[EmptyTextureEncoding + 1];

enum pg_TextureFormat{ pg_byte_tex_format = 0 , pg_float_tex_format , Emptypg_TextureFormat };

/*! \name Group texture_ mode
 */
// \{
/// _Render : rendering,
/// _Jpg : framebuffer output to jpg file
/// _Png : framebuffer output to png file
/// _Svg : framebuffer output to svg file
enum DrawingMode{ _Render=0 , _Svg , _Png , _Jpg , _Video };

// on/off automatic image output in case of live performance
extern bool is_automatic_snapshots;

// projection and view matrices transmitted to the shader
extern GLfloat projMatrix[16];
extern GLfloat viewMatrix[16];
extern GLfloat modelMatrix[16];

extern GLuint drawBuffers[16];

/////////////////////////////////////////////////////////////////
// config variables
// current mouse location (also used for displaying the cursor)
extern float mouse_x;
extern float mouse_y;
extern float mouse_x_prev;
extern float mouse_y_prev;

extern float tracks_x[PG_NB_TRACKS];
extern float tracks_y[PG_NB_TRACKS];
extern float tracks_x_prev[PG_NB_TRACKS];
extern float tracks_y_prev[PG_NB_TRACKS];
extern float tracks_Color_r[PG_NB_TRACKS];
extern float tracks_Color_g[PG_NB_TRACKS];
extern float tracks_Color_b[PG_NB_TRACKS];
extern float tracks_Color_a[PG_NB_TRACKS];
extern int tracks_BrushID[PG_NB_TRACKS];
extern float tracks_RadiusX[PG_NB_TRACKS];
extern float tracks_RadiusY[PG_NB_TRACKS];

extern bool track_replay_0;
extern bool track_replay_1;
extern bool track_replay_2;

extern bool source_track_record_0;
extern bool source_track_record_1;
extern bool source_track_record_2;

extern float *line_tracks_points[PG_NB_TRACKS];
extern float *line_tracks_target_points[PG_NB_TRACKS];

extern int *yHashtagFont;
extern int *wHashtagFont;
extern int *hHashtagFont;
extern int *sHashtagFont;
extern int *tHashtagFont;

#ifdef PG_VIDEO_ACTIVE
// video texture ID, image and camera
extern GLuint pg_video_texture_texID;
extern VideoCapture pg_input_camera;
extern Mat pg_video_frame;
#endif

// FUNCTIONS
// opengl error
GLenum printOglError( int no );
// error report
void ReportError( char *errorString );

// time management
double RealTime( void );
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif
float MemoryUsage( void );

void pg_initGeometry_quads(void);
// intialization of the tack lines used to display a track as a strip
#ifdef _WIN32
bool pg_initFrameTexture(Mat *video_frame);
#else
bool pg_initFrameTexture(IplImage *video_frame);
#endif
#ifdef PG_VIDEO_ACTIVE
int pg_initVideoCapture(void);
#endif

bool pg_initFBOTextures(GLuint textureID, int nb_attachments);
bool pg_initFBO(void);
void pg_initRenderingMatrices(void);

bool pg_initHashtagFontOffets(string dir, string basefilename);
bool pg_ReadAllDisplayMessages(string dir, string basefilename);
bool pg_ReadAllHashtags(string dir, string basefilename, string colorfilename);

int pg_screenMessage_upload(void);
int pg_displayMessage_upload(int indMesg);
int pg_hashtag_upload(void);

void window_display(void);
void pg_update_scene(void);
void pg_draw_scene(DrawingMode mode);


#endif
