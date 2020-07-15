////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg-texture.cpp
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

////////////////////////////////////////////////////////////////////
// FIRST FRAME IN A MOVIE
////////////////////////////////////////////////////////////////////
Mat pg_FirstMovieFrame; // First frame in a video (opening or looping) 
bool pg_FirstMovieFrameIsAvailable = false; // Available frist frame so as to avoid second reading  


////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
PhotoDataStruct pg_CameraFrame_buffer_data = PhotoDataStruct();
PhotoDataStruct pg_MovieFrame_buffer_data = PhotoDataStruct();

float pg_Photo_weight[PG_PHOTO_NB_TEXTURES] = { 0.0f , 0.0f };

////////////////////////////////////////////////////////////////////
// BLENDED MULTILAYER IMAGES FOR TVW
////////////////////////////////////////////////////////////////////
#if defined (TVW)
PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES_TVW] = { PhotoSwapDataStruct() };
#else
PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES] = { PhotoSwapDataStruct() };
#endif

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

////////////////////////////////////////////////////////////////////
// DIAPORAMA IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
int pg_CurrentDiaporamaFile = 0;
int pg_CurrentDiaporamaDir = -1;
bool ascendingDiaporama = true;

std::string pg_ImageDirectory;
#if defined (TVW)
std::string pg_MaskDirectory;
#endif
#if defined (TVW) || defined(ETOILES)
std::string pg_MessageDirectory;
#endif

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "remove_files_in_dir Error(" << errno << ") opening " << *dirpath << std::endl;
		return;
	}
	// std::cout << std::endl << "dir to get files of: " + *dirpath << std::endl;
	// scans all the subdirectories until finding one that has the current 
	// index + 1 as substring, typically 001_XXXX
	struct dirent *subdirp;
	while ((subdirp = readdir(dp))) {
		string filepath = *dirpath + "/" + subdirp->d_name;
		struct stat filestat;
		if (stat(filepath.c_str(), &filestat)) continue; // colleccts file status and returns 0 on success
		if (S_ISREG(filestat.st_mode)) { // the file is a normal file 
			unsigned int fileNameLength = int(strlen(subdirp->d_name));
			if (fileNameLength > 4
				&& (strcmp(subdirp->d_name + fileNameLength - 4, ".jpg") == 0
					|| strncmp(subdirp->d_name + fileNameLength - 4, ".png", 4) == 0)) {
				remove(filepath.c_str());
				std::cout << "deleted (" << filepath << ")" << std::endl;
			}
		}
	}
	// ends of the dir without finding the file, the file index will be reset
	closedir(dp);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE INITIALIZATION
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_initTextures(void) {

	//////////////////////////////////////////////////////////////
	// loaded textures
	// loads all the textures from the configuration file
	// unless the image itself is used to generate other textures
	for (int indTextureFile = 0; indTextureFile < pg_nb_Texture_files; indTextureFile++) {
		if (pg_Texture_usages[indTextureFile] != Texture_part_init) {
			if (pg_Texture_Dimension[indTextureFile] == 2) {
				pg_loadTexture((char *)(pg_Texture_fileNames[indTextureFile] + pg_Texture_fileNamesSuffix[indTextureFile]).c_str(),
					&pg_Texture_texID[indTextureFile],
					pg_Texture_Is_Rectangle[indTextureFile],
					pg_Texture_Invert[indTextureFile],
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR,
					pg_Texture_Size_X[indTextureFile],
					pg_Texture_Size_Y[indTextureFile]);
				printOglError(8);
			}
			else if (pg_Texture_Dimension[indTextureFile] == 3) {
				pg_loadTexture3D(pg_Texture_fileNames[indTextureFile],
					pg_Texture_fileNamesSuffix[indTextureFile],
					pg_Texture_Nb_3D_Nextures[indTextureFile],
					pg_Texture_Nb_Bytes_per_Pixel[indTextureFile],
					pg_Texture_Invert[indTextureFile],
					&pg_Texture_texID[indTextureFile],
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR,
					pg_Texture_Size_X[indTextureFile],
					pg_Texture_Size_Y[indTextureFile]);
				printOglError(8);
			}
		}
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
		else if (pg_Texture_usages[indTextureFile] == Texture_part_init
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			// printf("Loading particles initial images %s\n", fileName.c_str()); 
			if (generateParticleInitialPosColorRadiusfromImage(pg_Texture_fileNames[indTextureFile] + pg_Texture_fileNamesSuffix[indTextureFile])) {
				if (pg_Texture_Rank[indTextureFile] != pg_particle_initial_pos_speed_texID.size()
					|| pg_Texture_Rank[indTextureFile] != pg_particle_initial_color_radius_texID.size()) {
					sprintf(ErrorStr, "Error: particle initialization texture #%d rank incorrect (%d rank expected)!\n", pg_particle_initial_pos_speed_texID.size(), pg_Texture_Rank[indTextureFile]); ReportError(ErrorStr); throw 336;
				}
			}
		}
#endif
	}

	// assigns the textures to their destination according to their usage field
	for (int indTextureFile = 0; indTextureFile < pg_nb_Texture_files; indTextureFile++) {
#ifdef PG_SENSORS
		if (pg_Texture_usages[indTextureFile] == Texture_sensor
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			Sensor_texture_rectangle = pg_Texture_texID[indTextureFile];
		}
#endif
#if defined (GN)
		if (pg_Texture_usages[indTextureFile] == Texture_logo
			&& pg_Texture_Dimension[indTextureFile] == 2) { // 1024, 768
			pg_LYMlogo_texID = pg_Texture_texID[indTextureFile];
		}
#endif
#ifdef PG_WITH_MASTER_MASK
		if (pg_Texture_usages[indTextureFile] == Texture_master_mask
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			Master_Mask_texID = pg_Texture_texID[indTextureFile];
	}
#endif
#ifndef PG_BEZIER_PATHS
		if (pg_Texture_usages[indTextureFile] == Texture_brush
			&& pg_Texture_Dimension[indTextureFile] == 3) {
			Pen_texture_3D_texID = pg_Texture_texID[indTextureFile];
		}
#endif
		if (pg_Texture_usages[indTextureFile] == Texture_noise
			&& pg_Texture_Dimension[indTextureFile] == 3) {
			Noise_texture_3D = pg_Texture_texID[indTextureFile];
		}
#ifdef PG_WITH_REPOP_DENSITY
		if (pg_Texture_usages[indTextureFile] == Texture_repop_density
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			pg_RepopDensity_texture_texID.push_back(pg_Texture_texID[indTextureFile]);
			if (pg_Texture_Rank[indTextureFile] != pg_RepopDensity_texture_texID.size()) {
				sprintf(ErrorStr, "Error: repopulation texture density #%d rank incorrect (%d rank expected)!\n", pg_RepopDensity_texture_texID.size(), pg_Texture_Rank[indTextureFile]); ReportError(ErrorStr); throw 336;
			}
		}
#endif
#if defined (TEXTURED_QUAD_PARTICLES) || defined (LINE_SPLAT_PARTICLES) || defined (CURVE_PARTICLES) 
		// printf("Loading particles initial images %s\n", fileName.c_str()); 
		if (!pg_particle_initial_pos_speed_texID.size()
				|| !pg_particle_initial_color_radius_texID.size()) {
			sprintf(ErrorStr, "Error: particle initialization texture not provided, check texture list with part_init usage!\n"); ReportError(ErrorStr); throw 336;
		}
#endif
#ifdef PG_MESHES
		if (pg_Texture_usages[indTextureFile] == Texture_mesh
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			// assigns this textures to the meshes which have the same texture rank
			for (int indMeshFile = 0; indMeshFile < pg_nb_Mesh_files; indMeshFile++) {
				if (pg_Mesh_TextureRank[indMeshFile] == pg_Texture_Rank[indTextureFile]) {
					Mesh_texture_rectangle[indMeshFile] = pg_Texture_texID[indTextureFile];
				}
			}
		}
#endif
#ifdef CURVE_PARTICLES
		if (pg_Texture_usages[indTextureFile] == Texture_curve_particle
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			comet_texture_2D_texID = pg_Texture_texID[indTextureFile];
		}
#endif
#if defined (TEXTURED_QUAD_PARTICLES)
		if (pg_Texture_usages[indTextureFile] == Texture_splat_particle
			&& pg_Texture_Dimension[indTextureFile] == 2) {
			blurredDisk_texture_2D_texID = pg_Texture_texID[indTextureFile];
		}
#endif
	}
	// and checks that the textures were provided
#ifdef PG_SENSORS
	if (Sensor_texture_rectangle == NULL_ID) {
		sprintf(ErrorStr, "Error: sensor texture not provided, check texture list with sensor usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
#if defined (GN)
	if (pg_LYMlogo_texID == NULL_ID) {
		sprintf(ErrorStr, "Error: logo texture not provided, check texture list with sensor usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
#ifdef PG_WITH_MASTER_MASK
	if (Master_Mask_texID == NULL_ID) {
		sprintf(ErrorStr, "Error: master mask texture not provided, check texture list with sensor usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
#ifndef PG_BEZIER_PATHS
	if (Pen_texture_3D_texID == NULL_ID) {
		sprintf(ErrorStr, "Error: brush texture not provided, check texture list with sensor usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
	if (Noise_texture_3D == NULL_ID) {
		sprintf(ErrorStr, "Error: noise texture not provided, check texture list with sensor usage!\n"); ReportError(ErrorStr); throw 336;
	}
#ifdef PG_WITH_REPOP_DENSITY
	if (pg_RepopDensity_texture_texID.size() == 0) {
		sprintf(ErrorStr, "Error: repopulation texture density not provided, check texture list with repop_density usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
#ifdef PG_MESHES
	// assigns this textures to the meshes which have the same texture rank
	for (int indMeshFile = 0; indMeshFile < pg_nb_Mesh_files; indMeshFile++) {
		if (pg_Mesh_TextureRank[indMeshFile] != -1 // frame buffer texture
			&& Mesh_texture_rectangle[indMeshFile] == NULL_ID) {
			sprintf(ErrorStr, "Error: mesh texture not provided (mesh #%d and texture rank %d, check texture list with mesh usage!\n", indMeshFile, pg_Mesh_TextureRank[indMeshFile]); ReportError(ErrorStr); throw 336;
		}
	}
#endif
#ifdef CURVE_PARTICLES
	if (comet_texture_2D_texID == NULL_ID) {
		sprintf(ErrorStr, "Error: curve particles texture not provided, check texture list with curve_particle usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif
#if defined (TEXTURED_QUAD_PARTICLES)
	if (blurredDisk_texture_2D_texID == NULL_ID) {
		sprintf(ErrorStr, "Error: splat particles texture not provided, check texture list with splat_particle usage!\n"); ReportError(ErrorStr); throw 336;
	}
#endif

#if defined (TVW)
	pg_loadTexture(display_font_file_name,
		&Display_Font_texture_Rectangle_texID, true, false, GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		256, 86);
	printOglError(6);
#endif

	pg_loadTexture(screen_font_file_name,
		&Screen_Font_texture_Rectangle_texID, true, false, GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		128, 70);
	printOglError(6);

	//////////////////////////////////////////////////////////////
	// generated textures
#if defined (TVW)
	pg_displayMsg1Bitmap =
		(GLubyte *)pg_generateTexture(&pg_Display_Message1_Bitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg1Bitmap) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!\n"); ReportError(ErrorStr); throw 336;
	}
	glGenTextures(1, &(pg_Display_Message1_Bitmap_texID));

	pg_displayMsg2Bitmap =
		(GLubyte *)pg_generateTexture(&pg_Display_Message2_Bitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg2Bitmap) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!\n"); ReportError(ErrorStr); throw 336;
	}
	glGenTextures(1, &(pg_Display_Message2_Bitmap_texID));
#endif

	pg_screenMessageBitmap =
		(GLubyte *)pg_generateTexture(&pg_screenMessageBitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_screenMessageBitmap) {
		sprintf(ErrorStr, "Error: screen message bitmap not allocated (%s)!\n", ScreenMessage); ReportError(ErrorStr); throw 336;
	}
	// glGenTextures( 1, &(pg_screenMessageBitmap_texID) );

	// paths initialization
	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		pg_paths_Pos_Texture_texID[indPath] = 0;
		pg_paths_Pos_Texture[indPath] = NULL;
		pg_paths_Col_Texture_texID[indPath] = 0;
		pg_paths_Col_Texture[indPath] = NULL;
		pg_paths_RadBrushRendmode_Texture_texID[indPath] = 0;
		pg_paths_RadBrushRendmode_Texture[indPath] = NULL;
	}
	// paths textures generation
	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		pg_paths_Pos_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_Pos_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_Pos_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_Pos_Texture not allocated (%s)!\n", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
		pg_paths_Col_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_Col_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_Col_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_Col_Texture not allocated (%s)!\n", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
		pg_paths_RadBrushRendmode_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_RadBrushRendmode_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_RadBrushRendmode_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_RadBrushRendmode_Texture not allocated (%s)!\n", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
	}

	// CA Tables
#if defined (GN) || defined (CAAUDIO)
#define width_data_table 600
#define height_data_table 200
	pg_CATable =
		(GLubyte *)pg_generateTexture(&pg_CATable_ID, pg_byte_tex_format,
			width_data_table, height_data_table);
	pg_CATable_values(pg_CATable_ID, pg_CATable,
		width_data_table, height_data_table);
	if (!pg_CATable_ID) {
		sprintf(ErrorStr, "Error: data tables for the CA bitmap not allocated (%s)!\n", ScreenMessage); ReportError(ErrorStr); throw 336;
	}

	//printf( "\n" );
	//for( int i = 0 ; i <= 243 * 4 ; i+=4 ) {
	//  printf("%d " , (int)pg_CATable[ 4 * 51 * width_data_table + i ] );
	//}
	//printf( "\n" );
#endif

	printOglError(6);

	return true;
}



/////////////////////////////////////////////////////////////////
// TEXTURE GENERATION
/////////////////////////////////////////////////////////////////
// general texture allocation
void *pg_generateTexture(GLuint *textureID, pg_TextureFormat texture_format,
	int sizeX, int sizeY) {
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	// rgb POT raw image allocation (without alpha channel)
	// printf( "Texture %dx%d nb_attachments %d\n" ,  
	// 	  sizeX , sizeY , nb_attachments );
	void *returnvalue = NULL;

	// Return from the function if no file name was passed in
	// Load the image and store the data
	if (texture_format == pg_byte_tex_format) {
		GLubyte *ptr = new GLubyte[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			strcpy(ErrorStr, "Texture allocation error!\n"); ReportError(ErrorStr); throw 425;
		}
		int indTex = 0;
		for (int i = 0; i < sizeX * sizeY; i++) {
			ptr[indTex] = 0;
			ptr[indTex + 1] = 0;
			ptr[indTex + 2] = 0;
			ptr[indTex + 3] = 0;
			indTex += 4;
		}
		returnvalue = (void *)ptr;
	}
	else if (texture_format == pg_float_tex_format) {
		float *ptr = new float[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			strcpy(ErrorStr, "Texture allocation error!\n"); ReportError(ErrorStr); throw 425;
		}
		int indTex = 0;
		for (int i = 0; i < sizeX * sizeY; i++) {
			ptr[indTex] = 0.0f;
			ptr[indTex + 1] = 0.0f;
			ptr[indTex + 2] = 0.0f;
			ptr[indTex + 3] = 0.0f;
			indTex += 4;
		}
		returnvalue = (void *)ptr;
	}

	return returnvalue;
}

//////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TRANSITION TABLES FOR THE CELLULAR AUTOMATA
//////////////////////////////////////////////////////////////////////////////////////////

#if defined (GN)
#include "pg_CATable_GN.cpp"
#endif

#if defined (CAAUDIO)
#include "pg_CATable_CAaudio.cpp"
#endif


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// IMAGE FILE PROCESSING
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// IMAGE FILE WRITING (JPG OR PNG) (THREADED) 
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// SAVE IMAGE
//////////////////////////////////////////////////////////////////////

/* Attempts to save PNG to file; returns 0 on success, non-zero on error. */
#ifdef WIN32
DWORD WINAPI writepng(LPVOID lpParam) {
#else
void* writepng(void * lpParam) {
#endif
	threadData * pDataArray = (threadData *)lpParam;

	cv::Mat result;
	cv::flip(*(pDataArray->imgThreadData), result, 0); // vertical flip
#ifndef OPENCV_3
	cv::cvtColor(result, result, CV_RGB2BGR);

	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
#else
	cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

	std::vector<int> params;
	params.push_back(cv::IMWRITE_PNG_COMPRESSION);
#endif
	params.push_back(9);
	params.push_back(0);
	cv::imwrite(pDataArray->fname, result);

	delete pDataArray->imgThreadData;
	delete pDataArray->fname;
	delete pDataArray;

	return 0;
}


/* Attempts to save JPG to file; returns 0 on success, non-zero on error. */
#ifdef WIN32
DWORD WINAPI writejpg(LPVOID lpParam) {
#else
void* writejpg(void * lpParam) {
#endif
	threadData * pDataArray = (threadData *)lpParam;

	cv::Mat result;
	cv::flip(*(pDataArray->imgThreadData), result, 0); // vertical flip
#ifndef OPENCV_3
	cv::cvtColor(result, result, CV_RGB2BGR);

	std::vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
#else
	cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

	std::vector<int> params;
	params.push_back(cv::IMWRITE_JPEG_QUALITY);
#endif
	params.push_back(70);   // that's percent, so 100 == no compression, 1 == full 
							// printf("write jpg (%s)\n", pDataArray->fname);
	cv::imwrite(pDataArray->fname, result);

	delete pDataArray->imgThreadData;
	delete pDataArray->fname;
	delete pDataArray;

	return 0;
}


/////////////////////////////////////////////////////////////////
// NON THREADED TEXTURE LOADING
/////////////////////////////////////////////////////////////////

// 3D texture loading + transfer to GPU
bool pg_loadTexture3D(string filePrefixName, string fileSuffixName,
	int nbTextures, int bytesperpixel,
	bool invert,
	GLuint *textureID,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height) {
	// data type is assumed to be GL_UNSIGNED_BYTE

	char filename[1024];

	long size = width * height * bytesperpixel;
	GLubyte * bitmap = new unsigned char[size * nbTextures];
	long ind = 0;

	glEnable(GL_TEXTURE_3D);
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	for (int indTex = 0; indTex < nbTextures; indTex++) {
		sprintf(filename, "%s_%03d%s", filePrefixName.c_str(),
			indTex + 1, fileSuffixName.c_str());
		printf("Loading %s\n", filename);

		// texture load through OpenCV
#ifndef OPENCV_3
		cv::Mat imgOpenCV = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
		// printf("nb channels %d\n", imgOpenCV.channels());
		int colorTransform = (imgOpenCV.channels() == 4) ? CV_BGRA2RGBA :
			(imgOpenCV.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
#else
		cv::Mat imgOpenCV = cv::imread(filename, cv::IMREAD_UNCHANGED);   // Read the file
		int colorTransform = (imgOpenCV.channels() == 4) ? cv::COLOR_BGRA2RGBA :
			(imgOpenCV.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
		// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
#endif
		cv::Mat result;
		if (invert)
			cv::flip(imgOpenCV, result, 0); // vertical flip
		else
			result = imgOpenCV;

		if (!result.data) {                              // Check for invalid input
			sprintf(ErrorStr, "Could not open or find the image %s!\n", filename); ReportError(ErrorStr); throw 425;
			return false;
		}
		if (result.cols != width
			|| result.rows != height) {   // Check for invalid input
			sprintf(ErrorStr, "Unexpected 3D image size %s %d/%d %d/%d!\n", filename, result.cols, width, result.rows, height); ReportError(ErrorStr); throw 425;
			return false;
		}

		GLubyte * ptr = result.data;
		for (long int i = 0; i < size; i++)
			bitmap[ind++] = ptr[i];
	}
	// printf("Master index %ld / %ld\n" , ind , size * nbTextures );
	// glActiveTexture (GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_3D, *textureID);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D,     // Type of texture
		0,                  // Pyramid level (for mip-mapping) - 0 is the top level
		components,         // Components: Internal colour format to convert to
		width,              // Image width
		height,             // Image heigh
		nbTextures,         // Image depth
		0,                  // Border width in pixels (can either be 1 or 0)
		format,             // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		datatype,           // Image data type
		bitmap);            // The actual image data itself

	printOglError(0);

	// memory release
	delete[] bitmap;

	glEnable(GL_TEXTURE_RECTANGLE);

	// glGenerateMipmap(GL_TEXTURE_2D);
	// printf("3D textures loaded\n" );
	return true;
}

// 2D texture loading + transfer to GPU

bool pg_loadTexture(string fileName,
	GLuint *textureID, bool is_rectangle,
	bool invert,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height) {

	printf("Loading %s\n", fileName.c_str());

	glEnable(GL_TEXTURE_2D);
	if (!(*textureID)) {
		glGenTextures(1, textureID);
	}

	// texture load through OpenCV
#ifndef OPENCV_3
	cv::Mat imgOpenCV = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	// printf("File %s nb channels %d\n", fileName.c_str(), imgOpenCV.channels());
	int colorTransform = (imgOpenCV.channels() == 4) ? CV_BGRA2RGBA :
		(imgOpenCV.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
	if (imgOpenCV.channels() >= 3) {
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
	}
#else
	cv::Mat imgOpenCV = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
	int colorTransform = (imgOpenCV.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(imgOpenCV.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
	if (imgOpenCV.channels() >= 3) {
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
	}
#endif

	cv::Mat result;
	if (invert)
		cv::flip(imgOpenCV, result, 0); // vertical flip
	else
		result = imgOpenCV;

	if (!result.data) {                              // Check for invalid input
		sprintf(ErrorStr, "Could not open or find the image %s!\n", fileName.c_str()); ReportError(ErrorStr); throw 425;
		return false;
	}
	if (result.cols != width || result.rows != height) {                              // Check for invalid input
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!\n", fileName.c_str(), result.cols, width, result.rows, height); ReportError(ErrorStr); throw 425;
		return false;
	}

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, *textureID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			result.cols,          // Image width
			result.rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			result.ptr());        // The actual image data itself
		printOglError(40);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			result.cols,          // Image width
			result.rows,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			result.ptr());        // The actual image data itself
		printOglError(50);
		glEnable(GL_TEXTURE_RECTANGLE);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	return true;
}


//////////////////////////////////////////////////////////////////
// INITIALIZES PARTICLES FROM PHOTO
/////////////////////////////////////////////////////////////////

bool generateParticleInitialPosColorRadiusfromImage(string fileName) { // 2 texture IDs one for pos/speed, the other one for color/radius
	bool invert = false;

	printOglError(705);

	cv::Mat image;
#ifndef OPENCV_3
	cv::Mat imgOpenCV = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	cv::Mat imgOpenCV = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
																// Check for invalid input
	if (!imgOpenCV.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // ReportError(ErrorStr); throw 425;
		return false;
	}
	int nbChannels = imgOpenCV.channels();
	// printf("File %s nb channels %d\n", fileName.c_str(), nbChannels);
	int colorTransform = (nbChannels == 4) ? CV_BGRA2RGBA :
		(nbChannels == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (nbChannels == 4) ? GL_RGBA : GL_RGB;
	if (nbChannels >= 3) {
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
	}
	if (invert)
		cv::flip(imgOpenCV, image, 0); // vertical flip
	else
		image = imgOpenCV;

	if (nb_particles > leftWindowWidth * window_height) {
		sprintf(ErrorStr, "Particle size should be lower than %d!\n", leftWindowWidth * window_height); ReportError(ErrorStr); throw 425;
		return false;
	}
	//else {
	//	 printf("File %s nb channels %d for %d particles in window (%dx%d)\n", fileName.c_str(), nbChannels, nb_particles, leftWindowWidth, window_height);
	//}

	float *pos_speed = new float[leftWindowWidth * window_height * 4];
	float *color_radius = new float[leftWindowWidth * window_height * 4];
	bool *allocated = new bool[leftWindowWidth * window_height];
	for (int i = 0; i < leftWindowWidth * window_height; i++) {
		allocated[i] = false;
		color_radius[i * 4 + 0] = 1.f;
		color_radius[i * 4 + 1] = 1.f;
		color_radius[i * 4 + 2] = 1.f;
		color_radius[i * 4 + 3] = 1.f;
		pos_speed[i * 4] = 512.f;
		pos_speed[i * 4 + 1] = 512.f;
		pos_speed[i * 4 + 2] = 0.f;
		pos_speed[i * 4 + 3] = 0.f;
	}

	// for a 4:3 image- should be modified for another ratio
	float image_ratio = float(leftWindowWidth) / float(window_height);
#ifndef TEMPETE
	int partic_height = int(floor(sqrt(nb_particles / image_ratio)));
	int partic_width = int(floor(partic_height * image_ratio));
#else
	int partic_height = int(floor(sqrt(nb_particles / image_ratio)));
	int partic_width = int(floor(partic_height * image_ratio));
#endif
	float ratioImgToPart = float(image.cols) / partic_width;
	float partic_radius = 2.f * ratioImgToPart;
	for (int indRow = 0; indRow < partic_height; indRow++) {
		for (int indCol = 0; indCol < partic_width; indCol++) {
			int indPixelCol = int(round(indCol * ratioImgToPart));
			int indPixelRow = int(round(indRow * ratioImgToPart));
			int intPartic_radius = int(floor(partic_radius));
			int nbPixels = 0;
			float r = 0.f, g = 0.f, b = 0.f;
			for (int indLocCol = -intPartic_radius; indLocCol <= intPartic_radius; indLocCol++) {
				for (int indLocRow = -intPartic_radius; indLocRow <= intPartic_radius; indLocRow++) {
					if ((indPixelRow + indLocRow) >= 0 && (indPixelRow + indLocRow) < image.rows
						&& (indPixelCol + indLocCol) >= 0 && (indPixelCol + indLocCol) < image.cols) {
						Point3_<uchar>* p = image.ptr<Point3_<uchar> >(image.rows - (indPixelRow + indLocRow) - 1, (indPixelCol + indLocCol));
						r += p->x / 255.f;
						g += p->y / 255.f;
						b += p->z / 255.f;
						nbPixels++;
					}
				}
			}
			// average color
			r /= nbPixels;
			g /= nbPixels;
			b /= nbPixels;

			// random particle (for some mixture)
			int indParticle = int(rand_0_1 * nb_particles) % nb_particles;
			while (allocated[indParticle]) {
				indParticle = (indParticle + 1) % nb_particles;
			}
			allocated[indParticle] = true;

			color_radius[indParticle * 4 + 0] = r;
			color_radius[indParticle * 4 + 1] = g;
			color_radius[indParticle * 4 + 2] = b;
			color_radius[indParticle * 4 + 3] = partic_radius;
			pos_speed[indParticle * 4] = float(indPixelCol);
			pos_speed[indParticle * 4 + 1] = float(indPixelRow);
			pos_speed[indParticle * 4 + 2] = rand_0_1 - 0.5f;
			pos_speed[indParticle * 4 + 3] = rand_0_1 - 0.5f;
			//if (abs(indRow - partic_height / 2) < 5 && abs(indCol - partic_width / 2) < 5) {
			//	printf("col/row %dx%d ind part %d color %.1f %.1f %.1f rad %.1f  pos %.1f %.1f  speed %.1f %.1f \n",
			//		indCol, indRow, indParticle,
			//		color_radius[indParticle * 4 + 0],
			//		color_radius[indParticle * 4 + 1],
			//		color_radius[indParticle * 4 + 2],
			//		color_radius[indParticle * 4 + 3],
			//		pos_speed[indParticle * 4],
			//		pos_speed[indParticle * 4 + 1],
			//		pos_speed[indParticle * 4 + 2],
			//		pos_speed[indParticle * 4 + 3]);
			//}
		}
	}

	GLuint textureParticleInitialization_pos_speed_ID;
	GLuint textureParticleInitialization_color_radius_ID;
	glGenTextures(1, &textureParticleInitialization_pos_speed_ID);
	glGenTextures(1, &textureParticleInitialization_color_radius_ID);
	glEnable(GL_TEXTURE_RECTANGLE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_pos_speed_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, leftWindowWidth, window_height, 0,
		GL_RGBA, GL_FLOAT, pos_speed);
	pg_particle_initial_pos_speed_texID.push_back(textureParticleInitialization_pos_speed_ID);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_color_radius_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, leftWindowWidth, window_height, 0,
		GL_RGBA, GL_FLOAT, color_radius);
	pg_particle_initial_color_radius_texID.push_back(textureParticleInitialization_color_radius_ID);


	//printf("Init image loaded %s Real size %dx%d\nParticle size %d=%dx%d (of %d particles)\nRadius %.2f\n",
	//	fileName.c_str(),
	//	image.cols, image.rows,
	//	partic_width * partic_height, partic_width, partic_height, nb_particles,
	//	partic_radius);
	printf("Loading particle initial texture %s\n", fileName.c_str());

	free(color_radius);
	free(pos_speed);
	free(allocated);

	printOglError(706);

	return true;
}

//////////////////////////////////////////////
/// NON THREADED LOAD CAMERA FRAME
#ifdef PG_WITH_CAMERA_CAPTURE
void loadCameraFrame(bool initial_capture) {
#ifdef KOMPARTSD
	return;
#endif
	//Grabs and returns a frame from camera
	Mat pg_camera_frame;
	pg_camera_capture >> pg_camera_frame;
	// cv::Mat pg_camera_frame;
	if (initial_capture) {
		//Grabs and returns a frame from camera
		if (!pg_camera_frame.data) {
			printf("**************** Camera frame not grabbed! ****************\n");
			exit( 0 );
		}
		if (!pg_initCameraFrameTexture(&pg_camera_frame)) {
			printf("**************** Camera frame not initialized! **************** \n");
			return;
		}

		// tries to preserve ratio
		float window_ratio = float(leftWindowWidth) / window_height;
		if ( pg_camera_frame.rows * window_ratio < pg_camera_frame.cols) {
			// printf("camera size height leading\n");
			pg_camera_frame_width = int(pg_camera_frame.rows * window_ratio);
			pg_camera_frame_height = pg_camera_frame.rows;
			pg_camera_x_offset = (pg_camera_frame.rows - pg_camera_frame_width) / 2;
			pg_camera_y_offset = 0;
		}
		else {
			// printf("camera size width leading\n");
			pg_camera_frame_width = pg_camera_frame.cols;
			pg_camera_frame_height = pg_camera_frame.rows;
			if (pg_camera_frame.cols / window_ratio < pg_camera_frame.rows) {
				// printf("horizontal offset\n");
				pg_camera_x_offset = (int(pg_camera_frame.rows * window_ratio) - pg_camera_frame.cols) / 2;
				pg_camera_y_offset = 0;
			}
			else {
				// printf("camera height respecting\n");
				pg_camera_frame_height = pg_camera_frame.rows;
				pg_camera_x_offset = 0;
				pg_camera_y_offset = 0;
			}
		}
		// DASEIN float ratioSubImage = 0.5f;
#ifdef PG_WIDE_ANGLE_CAMERA
		float ratioSubImage = 0.5f;
#else
		float ratioSubImage = 1.0f;
#endif
		pg_camera_x_offset = pg_camera_x_offset + int(pg_camera_frame_width * (1.f - ratioSubImage) * 0.5);
#ifdef PG_WIDE_ANGLE_CAMERA
		pg_camera_x_offset = 350;
#endif
		pg_camera_frame_width = int(pg_camera_frame_width * ratioSubImage);
		pg_camera_y_offset = pg_camera_y_offset + int(pg_camera_frame_height * (1.f - ratioSubImage) * 0.5);
		pg_camera_frame_height = int(pg_camera_frame_height * ratioSubImage);
		// old cam
		/*
		pg_camera_frame_width = pg_camera_frame.cols;
		pg_camera_frame_height = pg_camera_frame.rows;
		pg_camera_x_offset = 0;
		pg_camera_y_offset = 0;
		*/
		printf("Camera frame %dx%d (before ratio %dx%d) offset (%dx%d) ch %d size %d\n",
			pg_camera_frame_width, pg_camera_frame_height,
			pg_camera_frame.cols, pg_camera_frame.rows,
			pg_camera_x_offset, pg_camera_y_offset, 
			pg_camera_frame.channels(), int(pg_camera_frame.total() * pg_camera_frame.elemSize()));
	}
	

	if (pg_camera_frame.data) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,            // Internal colour format to convert to
			pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
			pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
			0,                 // Border width in pixels (can either be 1 or 0)
			GL_BGR,			   // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,  // Image data type
			pg_camera_frame.data);        // The actual image data itself
												// /////////////////////////
												// reads BG camera frame for image subtraction
		if (reset_camera || secondCurrentBGCapture) { // currentVideoTrack <=> video off
			printf("*** non threaded current BG video capture frame %d\n", pg_FrameNo);
			reset_camera = false;

			// makes a second video capture 
			secondCurrentBGCapture = !secondCurrentBGCapture;

			// current background memory
			// printf("Threaded Transfer of camera BG to GPU (%s)\n", texData->photo_buffer_data->fname);
			glEnable(GL_TEXTURE_RECTANGLE);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
			glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
				pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
				0,                 // Border width in pixels (can either be 1 or 0)
				GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,  // Image data type
				pg_camera_frame.data);        // The actual image data itself
		}

#ifdef GN
		if (initialBGCapture || secondInitialBGCapture) { // currentVideoTrack <=> video off
			printf("*** non threaded initial BG video capture frame %d\n", pg_FrameNo);
			initialBGCapture = false;

			// makes a second video capture 
			secondInitialBGCapture = !secondInitialBGCapture;

			// current background memory
			// printf("Threaded Transfer of camera BG to GPU (%s)\n", texData->photo_buffer_data->fname);
			glEnable(GL_TEXTURE_RECTANGLE);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
			glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				GL_RGB,            // Internal colour format to convert to
				pg_camera_frame.cols,          // Image width  i.e. 640 for Kinect in standard mode
				pg_camera_frame.rows,          // Image height i.e. 480 for Kinect in standard mode
				0,                 // Border width in pixels (can either be 1 or 0)
				GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				GL_UNSIGNED_BYTE,  // Image data type
				pg_camera_frame.data);        // The actual image data itself
		}
#endif
	}
}


/////////////////////////////////////////////////////////////////
// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
/////////////////////////////////////////////////////////////////

bool pg_initCameraFrameTexture(Mat *video_frame) {
	glEnable(GL_TEXTURE_RECTANGLE);

	// camera image
	if (!pg_camera_texture_texID) {
		glGenTextures(1, &pg_camera_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
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
	if (!pg_camera_BG_texture_texID) {
		glGenTextures(1, &pg_camera_BG_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
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

#ifdef GN
	// initial background
	if (!pg_camera_BGIni_texture_texID) {
		glGenTextures(1, &pg_camera_BGIni_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
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
#endif
	return true;
}

int pg_initVideoCameraCapture(void) {
	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use.
	pg_camera_capture.open(camID);
	sprintf(pg_camera_capture_name, "Camera_%d", camID);

	// to be checked in real time
	// SHARPNESS
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// BRIGHTNESS
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// CONTRAST
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// SATURATION
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 128

	// GAIN
	// minimum                 : 0
	// maximum                 : 255
	// default_value           : 0

	// WHITE BALANCE
	// minimum                 : 2000
	// maximum                 : 6500
	// default_value           : 4000

	/* cvSetCaptureProperty comment for see3cam */
	float window_ratio = float(leftWindowWidth) / window_height;
	const float ratio_16_9 = 16.0f / 9.0f;
	if (window_ratio >= ratio_16_9 || ratio_16_9 - window_ratio < window_ratio - 4.0 / 3.0) {
		// pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
		pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
		// pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
		pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	}
	else {
		pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
		pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	}
	// higher values could be tried for width, fx 1280 x 720 (16:9) 960x720 (4:3)
	//pg_camera_capture.set(CV_CAP_PROP_FPS, 60);

	// initial camera frame reading
	loadCameraFrame(true);

	CameraCurrent_exposure = (float)pg_camera_capture.get(CV_CAP_PROP_EXPOSURE);
	CameraCurrent_gain = (float)pg_camera_capture.get(CV_CAP_PROP_GAIN);
	CameraCurrent_brightness = (float)pg_camera_capture.get(CV_CAP_PROP_BRIGHTNESS);
	CameraCurrent_saturation = (float)pg_camera_capture.get(CV_CAP_PROP_SATURATION);
	CameraCurrent_contrast = (float)pg_camera_capture.get(CV_CAP_PROP_CONTRAST);
	CameraCurrent_FPS = (float)pg_camera_capture.get(CV_CAP_PROP_FPS);
	CameraCurrent_focus = (float)pg_camera_capture.get(CV_CAP_PROP_FOCUS);
	CameraCurrent_gamma = (float)pg_camera_capture.get(CV_CAP_PROP_GAMMA);
	CameraCurrent_WB_B = (float)pg_camera_capture.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U);
	CameraCurrent_WB_R = (float)pg_camera_capture.get(CV_CAP_PROP_WHITE_BALANCE_RED_V);

	/*
	printf("Current Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Current Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Current Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Current Cam saturation %.2f\n", CameraCurrent_saturation);
	printf("Current Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Current Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Current Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Current Cam gamma      %.2f\n", CameraCurrent_gamma);
	printf("Current Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);
	*/

	if (*((float *)ScenarioVarPointers[_cameraExposure]) != CameraCurrent_exposure) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, (*((float *)ScenarioVarPointers[_cameraExposure])));
		CameraCurrent_exposure = *((float *)ScenarioVarPointers[_cameraExposure]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraGain]) != CameraCurrent_gain) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_GAIN, *((float *)ScenarioVarPointers[_cameraGain]));
		CameraCurrent_gain = *((float *)ScenarioVarPointers[_cameraGain]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraBrightness]) != CameraCurrent_brightness) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, *((float *)ScenarioVarPointers[_cameraBrightness]));
		CameraCurrent_brightness = *((float *)ScenarioVarPointers[_cameraBrightness]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraSaturation]) != CameraCurrent_saturation) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_SATURATION, *((float *)ScenarioVarPointers[_cameraSaturation]));
		CameraCurrent_saturation = *((float *)ScenarioVarPointers[_cameraSaturation]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraContrast]) != CameraCurrent_contrast) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_CONTRAST, *((float *)ScenarioVarPointers[_cameraContrast]));
		CameraCurrent_contrast = *((float *)ScenarioVarPointers[_cameraContrast]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraWB_R]) != CameraCurrent_WB_R
		&& *((float *)ScenarioVarPointers[_cameraWB_R]) >= 0) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_RED_V, *((float *)ScenarioVarPointers[_cameraWB_R]));
		CameraCurrent_WB_R = *((float *)ScenarioVarPointers[_cameraWB_R]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraWB_B]) != CameraCurrent_WB_B) {
		// cvSetCaptureProperty comment for see3cam
		pg_camera_capture.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U, *((float *)ScenarioVarPointers[_cameraWB_B]));
		CameraCurrent_WB_B = *((float *)ScenarioVarPointers[_cameraWB_B]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}

	/*
	printf("Initial Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Initial Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Initial Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Initial Cam saturation %.2f\n", CameraCurrent_saturation);
	printf("Initial Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Initial Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Initial Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Initial Cam gamma      %.2f\n", CameraCurrent_gamma);
	printf("Initial Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);
	*/

	return 1;
}
#endif

bool pg_initMovieFrameTexture(Mat *movie_frame) {
	glEnable(GL_TEXTURE_RECTANGLE);

	// movie image
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_movie_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		pg_movie_frame_width,          // Image width  i.e. 640 for Kinect in standard mode
		pg_movie_frame_height,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		movie_frame->data);        // The actual image data itself
										// current background
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return true;
}


#ifdef WIN32
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam) {
#else
void* pg_initVideoMoviePlayback(void * lpParam) {
#endif
	string * fileName = (string *)lpParam;
	// printf("VideoPb Init movie playback and capture first frame \n");

	// film loading openCV
#ifndef TEMPETE
	pg_movie_capture.open(("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
	pg_movie_capture.open((movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
	if (!pg_movie_capture.isOpened()) {
#ifndef TEMPETE
		printf("Movie file %s not loaded!\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
		printf("Movie file %s not loaded!\n", (movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
		return NULL;
	}
#ifndef TEMPETE
	strcpy(pg_MovieFrame_buffer_data.PhotoName, ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
	strcpy(pg_MovieFrame_buffer_data.PhotoName, (movieFileName[currentlyPlaying_movieNo]).c_str());
#endif

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	if (!pg_FirstMovieFrame.data) {
#ifndef TEMPETE
		printf("Movie frame %s not grabbed!\n", ("Data/" + project_name + "-data/videos/" + *fileName).c_str());
#else
		printf("Movie frame %s not grabbed!\n", (*fileName).c_str());
#endif
		return NULL;
	}
	else {
		pg_FirstMovieFrameIsAvailable = true;
	}

	//  transfer to GPU
	pg_initMovieFrameTexture(&pg_FirstMovieFrame);

	pg_movie_frame_width = pg_FirstMovieFrame.cols;
	pg_movie_frame_height = pg_FirstMovieFrame.rows;

	//printf("Movie frame %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_nbFrames = int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT));
	is_movieLoading = false;
	return NULL;
}


void* pg_initVideoMoviePlayback_nonThreaded(string * fileName) {
	// printf("VideoPb Init movie playback and capture first frame non threaded\n");

	// film loading openCV
#ifndef TEMPETE
	pg_movie_capture.open(("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
	pg_movie_capture.open((movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
	// printf("%s Opened\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
#ifndef TEMPETE
		printf("Movie file %s not loaded!\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
		printf("Movie file %s not loaded!\n", (movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
		return NULL;
	}
#ifndef TEMPETE
	strcpy(pg_MovieFrame_buffer_data.PhotoName, ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
	strcpy(pg_MovieFrame_buffer_data.PhotoName, (movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
	// printf("Name %s Opened\n", pg_MovieFrame_buffer_data.PhotoName);

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	// texture loading
	if (!pg_FirstMovieFrame.data) {
#ifndef TEMPETE
		printf("Movie frame %s not grabbed!\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
#else
		printf("Movie frame %s not grabbed!\n", (movieFileName[currentlyPlaying_movieNo]).c_str());
#endif
		return NULL;
	}
	if (!pg_initMovieFrameTexture(&pg_FirstMovieFrame)) {
		printf("Movie frame not initialized!\n");
		return NULL;
	}
	else {
		pg_FirstMovieFrameIsAvailable = true;
	}

	pg_movie_frame_width = pg_FirstMovieFrame.cols;
	pg_movie_frame_height = pg_FirstMovieFrame.rows;

	// printf("Movie frame %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_nbFrames = int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT));
	is_movieLoading = false;

	// printf("Movie nb frames %d\n", pg_movie_nbFrames);
	return NULL;
}


/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// PROCESSES PHOTO DIAPORAMA

// initializes the blending of each image inside a folder
void pg_launch_diaporama(void) {
	// printf("pg_launch_performance diaporama\n");

	// printOglError(467);

	// loads the first images if it is not the case
	string fileName;
	pg_CurrentDiaporamaFile = 0;
	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		//printf("load diaporama initial files #%d diapor %d nb albums %d\n", indPhoto, pg_CurrentDiaporamaDir, nb_photo_albums);
		pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart = -1.0f;
		if (pg_CurrentDiaporamaDir >= 0 && pg_CurrentDiaporamaDir < pg_nbCompressedImageDirs) {
			if (photoAlbumDirName.compare("captures") == 0) {
				printf("The scanning of captures should be reimplemented to take into consideration the preloading of images (vs dynamic as in the previous versions)\n");
				ascendingDiaporama = false;
				int nextCompressedImage
					= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
						&pg_CurrentDiaporamaFile, ascendingDiaporama);
				pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
					= nextCompressedImage;
			}
			else {
				ascendingDiaporama = true;
				int nextCompressedImage
					= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
						&pg_CurrentDiaporamaFile, ascendingDiaporama);
				pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
					= nextCompressedImage;
				//printf("curr diap %d curr file %d asc %d next img %d\n", pg_CurrentDiaporamaDir,
				//	pg_CurrentDiaporamaFile, ascendingDiaporama, nextCompressedImage);
				// launches blending of the first image
				pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart = CurrentClockTime;
			}
		}
		// printf("ama initial files #%d\n", indPhoto);
	}

	printOglError(469);

	// launches blending of the first image
	pg_Photo_swap_buffer_data[0].blendStart = CurrentClockTime;
	for (int indPhoto = 1; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		pg_Photo_swap_buffer_data[1].blendStart = -1.0f;
	}
	printOglError(465);
}

bool pg_update_diaporama(void) {
		
	bool valRet = false;

	// avoids blocking the diaporama with two photos to be started
	// and not being triggered (because the ongoing blends have been
	// achieved while loading the new photos
	if (pg_Photo_swap_buffer_data[0].blendStart < 0.0f
		&& pg_Photo_swap_buffer_data[1].blendStart < 0.0f) {
		pg_Photo_swap_buffer_data[0].blendStart = CurrentClockTime;
	}

	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		float * photoWeightPtr;
		if (indPhoto == 0) {
			photoWeightPtr = pg_Photo_weight + 0;
		}
		else {
			photoWeightPtr = pg_Photo_weight + 1;
		}

		// if blendStart is negative, there is no ongoing blending for the current photo
		// it will be started when the preceding one has reached
		// blendStart + photo_diaporama_fade + photo_diaporama_plateau + intervalDuration
		if (pg_Photo_swap_buffer_data[indPhoto].blendStart < 0.0f) {
			// not blending
			//printf("Photo %d blend start %.3f\n",
			//	indPhoto, pg_Photo_swap_buffer_data[indPhoto].blendStart);
			*photoWeightPtr = 0.0f;
			continue;
		}

		// ongoing blending
		float playingTime = CurrentClockTime - pg_Photo_swap_buffer_data[indPhoto].blendStart;
		*photoWeightPtr = 0.0f;
		// incay and decay are 0 if is_capture_diaporama
		float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float interval = (is_capture_diaporama ? 0 : pg_Photo_swap_buffer_data[indPhoto].intervalDuration);
		// printf("Photo %d playing time %.3f\n", indPhoto, playingTime);
		if (playingTime < (incay + photo_diaporama_plateau + decay)) {
			if (playingTime < incay) {
				if (incay > 0.0f) {
					*photoWeightPtr = playingTime / incay;
				}
				else {
					*photoWeightPtr = 1.0f;
				}
			}
			else if (playingTime < incay + photo_diaporama_plateau) {
				*photoWeightPtr = 1.0f;
			}
			else {
				if (decay > 0.0f) {
					*photoWeightPtr = 1.0f - (playingTime - incay - photo_diaporama_plateau)
						/ decay;
				}
				else {
					*photoWeightPtr = 1.0f;
				}
			}
			//if( indPhoto == 1)
			//printf("            "); 
			//printf("blend %d weight %.2f playing time %.1f\n",
			//	indPhoto, photoWeight, playingTime );
		}

		// it is time to start the other photo because we reach
		// the end of the playing duration (incay+plateau+interval) of the current one
		// printf("Playing time %.2f layer %d\n", playingTime, indPhoto);
		if (playingTime >= (incay + photo_diaporama_plateau + interval)
			&& pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart < 0) {
			pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart =
				pg_Photo_swap_buffer_data[indPhoto].blendStart
				+ incay + photo_diaporama_plateau + interval;
			pg_Photo_weight[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES] = 0.0f;
			valRet = true;
			// that displays the full blendped image
			//std::cout << "start new photo for layer " << (indPhoto + 1) % PG_PHOTO_NB_TEXTURES << std::endl;

			// releases the blend image
			// imgPhotoBuffer[indPhoto].release();
		}

		// it is time to stop the current photo because we reach
		// the end of the blending duration (incay+plateau+decay) of the current one
		if (playingTime >= (incay + photo_diaporama_plateau + decay)) {
			// stops blending
			pg_Photo_swap_buffer_data[indPhoto].blendStart = -1.0f;
			*photoWeightPtr = 0.0f;
			// releases the image
		    //std::cout << "load next file for layer " << indPhoto << std::endl;
			//imgPhotoBuffer[indPhoto].release();
			// loads the next image to be prepared for next blend
			string fileName = "";
			int nextCompressedImage
				= nextFileIndexMemoryLoop(pg_CurrentDiaporamaDir,
					&pg_CurrentDiaporamaFile, ascendingDiaporama);
			pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
				= nextCompressedImage;

			valRet = true;
			// that displays the full blendped image

			// releases the blend image
			// imgPhotoBuffer[indPhoto].release();
		}
	}

	return valRet;
}

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
PhotoDataStruct **pg_Photo_buffer_data = NULL;
int pg_nbCompressedImageDirs = 0;
int *pg_nbCompressedImagesPerFolder = NULL;
int *pg_firstCompressedFileInFolder = NULL;
int pg_nbCompressedImages = 0;
#if defined (TVW)
PhotoDataStruct pg_Photo_mask_buffer_data[PG_PHOTO_NB_TEXTURES_TVW / 3] = { PhotoDataStruct() };
// interpolation weight between image buffer swap buffer in each layer
GLfloat pg_Photo_alphaSwap02[PG_PHOTO_NB_TEXTURES_TVW / 2] = { 0.0f, 0.0f, 0.0f };
GLfloat pg_Photo_alphaSwap35[PG_PHOTO_NB_TEXTURES_TVW / 2] = { 0.0f, 0.0f, 0.0f };
// image buffer layer weights
GLfloat pg_Photo_weightTVW[PG_PHOTO_NB_TEXTURES_TVW] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat pg_Photo_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2] = { 0.0f };
GLfloat pg_Photo_mask_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2] = { 0.0f };
// GLfloat mask_inverse_buffer_position_noises[PG_PHOTO_NB_TEXTURES_TVW * 2] = { 0.0f };
#endif
// the index from which an image available for swapping is looked for
int pg_IndInitialSwapPhoto = 0;

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

bool is_substring_index(char * char_string_subdir, int ind) {
	std::string string_subdir(char_string_subdir);
	char string_number[16];
	sprintf(string_number, "_%03d", ind);
	std::string string_number_s(string_number);

	// std::cout << "subdir index: " + string_number_s << std::endl;

	if (string_subdir.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + std::string(string_subdir) << " index " << ind << std::endl;
		return true;
	}
	return false;
}

bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir) {
	string filepath = *dirpath + dirp->d_name;
	struct stat filestat;
	// std::cout << "subdir path: " + filepath << std::endl;

	// If the file is a directory and the name contains the integer 
	if (stat(filepath.c_str(), &filestat)) {
		std::cout << "subdir error: " + filepath << std::endl;
		return false; // colleccts file status and returns 0 on success
	}
	if (S_ISDIR(filestat.st_mode) 
		&& is_substring_index(dirp->d_name, inddir)) { // the file is a directory 
													   // and contains the integer substring
		// std::cout << "subdir found: " + filepath << std::endl;
		return true;
	}
	// std::cout << "subdir not found: " + filepath << std::endl;
	return false;
}

bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;
	// std::cout << "file path looking for: " + filepath << std::endl;

	// If the file is a directory (or is in some way invalid) we'll skip it 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode)) { // the file is a directory 
									 // and contains the integer substring
		return false;
	}
	ifstream fin;
	fin.open(filepath.c_str());
	if (fin) {
		if (is_substring_index(dirp->d_name, indfile)) {
			fin.close();
			return true;
		}
		fin.close();
	}
	return false;
}

string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile) {
	DIR *dp = opendir(dirpath->c_str());
	// std::cout << "opening dir : (" << *dirpath << ")" << std::endl;
	if (dp == NULL)
	{
		std::cout << "is_subdir_subfile_index 1 Error(" << errno << ") opening " << *dirpath << std::endl;
		return NULL;
	}
	// std::cout << std::endl << "dir to get files of: " + *dirpath << std::endl;
	struct dirent *dirp;
	while ((dirp = readdir(dp))) {
		if (is_subdir_index(dirp, dirpath, inddir)) {
			string subdirpath(*dirpath + dirp->d_name);
			// std::cout << std::endl << "reading files number " << indfile << " from subdir: " + subdirpath << std::endl;
			DIR *subdp = opendir(subdirpath.c_str());
			if (subdp == NULL)
			{
				// std::cout << "is_subdir_subfile_index 2 Error(" << errno << ") opening " << subdirpath << std::endl;
				return NULL;
			}
			struct dirent *subdirp;
			while ((subdirp = readdir(subdp))) {
				if (is_subfile_index(subdirp, &subdirpath, indfile)) {
					string * subdirfilepath = new string(subdirpath + "/" + subdirp->d_name);
					// std::cout << "found: (" << inddir << "," << indfile << ") in " << *subdirfilepath << std::endl;
					closedir(subdp);
					closedir(dp);
					return subdirfilepath;
				}
			}
			closedir(subdp);
		}
	}
	closedir(dp);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// AFTER LOADING IN MEMORY, FOR DISPLAY: TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY LOOPS INSIDE THE DIRECTORY
int nextFileIndexMemoryLoop(int currentDirIndex, int *currentFileIndex, bool ascending) {
	if (pg_nbCompressedImagesPerFolder[currentDirIndex] > 0
		&& pg_firstCompressedFileInFolder[currentDirIndex] >= 0
		&& pg_firstCompressedFileInFolder[currentDirIndex] < pg_nbCompressedImages) {
		int indcomprImage
			= pg_firstCompressedFileInFolder[currentDirIndex] + *currentFileIndex;
		if (ascending) {
			*currentFileIndex
				= (*currentFileIndex + 1) % pg_nbCompressedImagesPerFolder[currentDirIndex];
		}
		else {
			*currentFileIndex
				= (*currentFileIndex - 1 + pg_nbCompressedImagesPerFolder[currentDirIndex])
				% pg_nbCompressedImagesPerFolder[currentDirIndex];
		}
		return indcomprImage % pg_nbCompressedImages;
	}
	else {
		while (pg_nbCompressedImagesPerFolder[currentDirIndex] == 0
			|| pg_firstCompressedFileInFolder[currentDirIndex] < 0
			|| pg_firstCompressedFileInFolder[currentDirIndex] >= pg_nbCompressedImages) {
			pg_CurrentDiaporamaDir++;
			currentDirIndex = pg_CurrentDiaporamaDir;
			*currentFileIndex = 0;
		}
		int indcomprImage
			= pg_firstCompressedFileInFolder[currentDirIndex] + *currentFileIndex;
		if (ascending) {
			*currentFileIndex
				= (*currentFileIndex + 1) % pg_nbCompressedImagesPerFolder[currentDirIndex];
		}
		else {
			*currentFileIndex
				= (*currentFileIndex - 1 + pg_nbCompressedImagesPerFolder[currentDirIndex])
				% pg_nbCompressedImagesPerFolder[currentDirIndex];
		}
		return indcomprImage % pg_nbCompressedImages;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// ON THE DISK TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY JUMPS TO NEXT DIRECTORY IF ALL IMAGES ARE USED
// AND POSSIBLY JUMPS TO THE FIRST DIRECTORY IF ALL IMAGES ARE USED
std::string *nextFileIndexDiskLoop(std::string *dirpath, int *currentDirIndex,
	int *currentFileIndex) {
	// printf("dir path %s cur dir index %d cur file index %d\n", dirpath->c_str(), *currentDirIndex, *currentFileIndex);
	string * returnedString;
	// next file in the same dir
	if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
		(*currentFileIndex)++;
		return returnedString;
	}
	else {
		// first file in the same dir
		(*currentFileIndex) = 0;
		if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
			(*currentFileIndex)++;
			return returnedString;
		}
		else {
			// first file in the next dir
			(*currentDirIndex)++;
			(*currentFileIndex) = 0;
			if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
				(*currentFileIndex)++;
				return returnedString;
			}
			else {
				// first file in the first dir
				(*currentDirIndex) = 0;
				(*currentFileIndex) = 0;
				if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
					(*currentFileIndex)++;
					return returnedString;
				}
				else {
					std::cout << "nextFileIndexDiskLoop Error(" << errno << ") opening " << dirpath << std::endl;
					return NULL;
				}
			}

		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// ON THE DISK TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY JUMPS TO NEXT DIRECTORY IF ALL IMAGES ARE USED
// USED FOR LOADING ALL IMAGES
// STOPS AT THE LAST DIRECTORY
// SUBDIRECTORY AND FILE NAMES SHOULD END WITH 000, 001, 002...
string * nextFileIndexDiskNoLoop(string *dirpath, int *currentDirIndex, int *currentFileIndex,
	int maxFilesPerFolder) {
	// printf("dir path %s cur dir index %d cur file index %d maxFilesPerFolder %d\n", dirpath->c_str(), *currentDirIndex, *currentFileIndex, maxFilesPerFolder);
	string * returnedString;
	// next file in the same dir
	if ((*currentFileIndex) < maxFilesPerFolder
		&& (returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
		(*currentFileIndex)++;
		return returnedString;
	}
	else {
		// first file in the next dir
		(*currentFileIndex) = 0;
		(*currentDirIndex)++;
		if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
			(*currentFileIndex)++;
			return returnedString;
		}
		else {
			// finishes
			return NULL;
		}
	}
}


/////////////////////////////////////////////
// 2D image loading
bool PhotoDataStruct::pg_loadPhoto(bool toBeInverted, int width,
	int height, bool verbose) {
	int flieNameLenght = int(strlen(PhotoName));
	char * extension;

	h = height;
	w = width;
	invert = toBeInverted;

	const char * ptr = strrchr(PhotoName, '/');

	if (verbose) {
		printf("Loading [%s]\n", (ptr ? ptr + 1 : PhotoName));
	}

	if (flieNameLenght < 4) {
		printf("Incorrect photo file name [%s]\n", PhotoName);
		return false;
	}
	extension = PhotoName + flieNameLenght - 4;

	// DXT1 FILE
	if (strcmp(extension, "dxt1") == 0 || strcmp(extension, "DXT1") == 0 || strcmp(extension, ".DDS") == 0 || strcmp(extension, ".dds") == 0) {
		unsigned char header[124];

		FILE *fp;

		/* try to open the file */
		fp = fopen(PhotoName, "rb");

		// Check for invalid input
		if (fp == NULL) {
			sprintf(ErrorStr, "Could not open or find the 2D image %s!", PhotoName); ReportError(ErrorStr);
			*(PhotoName) = 0;
			return false;
		}

		/* verify the type of file */
		char filecode[5];
		fread(filecode, 1, 4, fp);
		filecode[4] = 0;
		if (strncmp(filecode, "DDS ", 4) != 0) {
			sprintf(ErrorStr, "Incorrect compressed file type %s!", filecode); ReportError(ErrorStr);
			*(PhotoName) = 0;
			fclose(fp);
			return false;
		}

		/* get the surface desc */
		fread(&header, 124, 1, fp);

		unsigned int height_from_header = *(unsigned int*)&(header[8]);
		unsigned int width_from_header = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);
		unsigned int ratio_h = 1;
		unsigned int ratio_v = 1;

		// Check for image size
		if ((w > 0 && width_from_header != u_int(w))
			|| (h > 0 && height_from_header != u_int(h))) {
			sprintf(ErrorStr, "Unexpected dds diaporama image size %s %d/%d %d/%d!",
				PhotoName, width_from_header, w, height_from_header,
				h); ReportError(ErrorStr);
			//ratio_h = u_int(w) / width_from_header;
			//ratio_v = u_int(h) / height_from_header;
			//*(PhotoName) = 0;
			//return false;
		}
		h = height_from_header;
		w = width_from_header;
		format = GL_RGB;

		// Check for image size
		if (mipMapCount > 1) {
			sprintf(ErrorStr, "Unexpected number of mipmaps %s %d!",
				PhotoName, mipMapCount); ReportError(ErrorStr);
			*(PhotoName) = 0;
			return false;
		}

		// allocates memory
		compressedPhotoBitmap
			= (unsigned char*)malloc(linearSize * sizeof(unsigned char));
		fread(compressedPhotoBitmap, 1, linearSize, fp);
		/* close the file pointer */
		fclose(fp);

		switch (fourCC)
		{
		case FOURCC_DXT1:
			compressedFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			// printf("compressed format #1\n");
			break;
		case FOURCC_DXT3:
			compressedFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			// printf("compressed format #2\n");
			break;
		case FOURCC_DXT5:
			compressedFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			// printf("compressed format #3\n");
			break;
		default:
			sprintf(ErrorStr, "Unexpected compression format %s %d!",
				PhotoName, fourCC); ReportError(ErrorStr);
			*(PhotoName) = 0;
			free(compressedPhotoBitmap);
			compressedPhotoBitmap = NULL;
			return false;
		}
	}
	// JPEG FILE
	else if (strcmp(extension, ".jpg") == 0) {
		// texture load through OpenCV
#ifndef OPENCV_3
		// threaded = false;
		imgPhotoOpenCV = new cv::Mat();
		*(imgPhotoOpenCV) = cv::imread(PhotoName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
																			  // Check for invalid input
		if (!imgPhotoOpenCV.data) {
			sprintf(ErrorStr, "Could not open or find the 2D image %s!", PhotoName); ReportError(ErrorStr);
			loadedInRAM = false;
			imgPhotoOpenCV->release();
			*(PhotoName) = 0;
			return false;
		}
		// Check for image size
		if ((w > 0 && imgPhotoOpenCV.cols != w)
			|| (h > 0 && imgPhotoOpenCV.rows != h)) {
			sprintf(ErrorStr, "Unexpected jpg diaporama image size %s %d/%d %d/%d!",
				PhotoName, imgPhotoOpenCV.cols, w, imgPhotoOpenCV.rows,
				h); ReportError(ErrorStr);
			loadedInRAM = false;
			imgPhotoOpenCV->release();
			*(PhotoName) = 0;
			return false;
		}
		w = imgPhotoOpenCV.cols;
		h = imgPhotoOpenCV.rows;
		format
			= ((imgPhotoOpenCV->channels() == 4) ? GL_RGBA :
			((imgPhotoOpenCV->channels() == 3) ? GL_RGB : GL_LUMINANCE));

		int colorTransform = (channelsPhoto == 4) ? CV_BGRA2RGBA :
			(imgPhotoOpenCV->channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (imgPhotoOpenCV->channels() == 4) ? GL_RGBA : GL_RGB;
		if (imgPhotoOpenCV->channels() >= 3) {
			cv::cvtColor(imgPhotoOpenCV, imgPhotoOpenCV, colorTransform);
		}
#else
		imgPhotoOpenCV = new cv::Mat();
		*(imgPhotoOpenCV) = cv::imread(PhotoName, cv::IMREAD_UNCHANGED);   // Read the file
																		   // Check for invalid input
		if (!imgPhotoOpenCV->data) {
			sprintf(ErrorStr, "Could not open or find the 2D image %s!", PhotoName); ReportError(ErrorStr);
			imgPhotoOpenCV->release();
			*(PhotoName) = 0;
			return false;
		}
		// Check for image size
		if ((w > 0 && imgPhotoOpenCV->cols != w)
			|| (h > 0 && imgPhotoOpenCV->rows != h)) {
			sprintf(ErrorStr, "Unexpected jpg image size %s %d/%d %d/%d!",
				PhotoName, imgPhotoOpenCV->cols, w, imgPhotoOpenCV->rows,
				h); ReportError(ErrorStr);
			imgPhotoOpenCV->release();
			*(PhotoName) = 0;
			return false;
		}
		w = imgPhotoOpenCV->cols;
		h = imgPhotoOpenCV->rows;
		format
			= ((imgPhotoOpenCV->channels() == 4) ? GL_RGBA :
			((imgPhotoOpenCV->channels() == 3) ? GL_RGB : GL_LUMINANCE));


		int colorTransform
			= ((imgPhotoOpenCV->channels() == 4) ? cv::COLOR_BGRA2RGBA :
			((imgPhotoOpenCV->channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB));

		// int glColorType = (imgPhotoOpenCV->channels() == 4) ? GL_RGBA : GL_RGB;
		if (imgPhotoOpenCV->channels() >= 3) {
			cv::cvtColor(*(imgPhotoOpenCV), *(imgPhotoOpenCV), colorTransform);
		}
#endif
	}
	// UNKNOWN
	else {
		printf("Incorrect photo file name extension [%s]\n", PhotoName);
		return false;
	}

	//	char * ptr = strrchr( PhotoName , '/' );
	//	printf( "-> RAM image [%s] size %dx%d\n" , (ptr? ptr + 1: PhotoName) ,
	//		w  , imgpDataArray->h );
	//      printf("Loaded in RAM %d\n",loadedInRAM);

	return true;
}

//////////////////////////////////////////////////////////////////
// TRANSFERS BUFFER TO GPU

bool PhotoDataStruct::pg_toGPUPhoto(bool is_rectangle,
	GLint components,
	GLenum datatype, GLenum texturefilter) {
	/* // components GL_RGB8,
	// datatype GL_UNSIGNED_BYTE
	// format GL_RGB
	printf("format GL_RGB comp GL_RGB8 datatype GL_UNSIGNED_BYTE %d %d %d\n",
	int(components == GL_RGB8),
	int(datatype == GL_UNSIGNED_BYTE),
	int(format == GL_RGB)); */

	bool valret = true;

	printOglError(6);

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle && !compressedFormat) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, texBuffID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			w,          // Image width
			h,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			imgPhotoOpenCV->ptr());        // The actual image data itself
		printOglError(4);
		// printf("filename %s\n", PhotoName);
		valret = false;
	}
	else {
		if (!compressedFormat) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texBuffID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				components,            // Components: Internal colour format to convert to
				w,          // Image width
				h,          // Image height
				0,                 // Border width in pixels (can either be 1 or 0)
				format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				datatype,  // Image data type
				imgPhotoOpenCV->ptr());        // The actual image data itself
			printOglError(5);
			valret = false;
		}
		else {
			unsigned int blockSize
				= (compressedFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
			unsigned int size = ((w + 3) / 4)*((h + 3) / 4)*blockSize;
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texBuffID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glCompressedTexImage2D(GL_TEXTURE_2D,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				compressedFormat,  // compression format
				w,                 // Image width
				h,                 // Image height
				0,                 // Border width in pixels (can either be 1 or 0)
				size,              // Image data type
				compressedPhotoBitmap);        // The actual image data itself

			free(compressedPhotoBitmap);
			compressedPhotoBitmap = NULL;
			printOglError(6);
			valret = false;
		}
	}

	// glGenerateMipmap(GL_TEXTURE_2D);
	return valret;
}

/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

#if defined (TVW)
//////////////////////////////////////////////////////////////////
// PROCESSES IMAGE SWAPPING

// updates the swapping weights from 0 to 1 until the
// swapped image is visible
// if it reaches 1, the image contents are swapped and 
// the swap image is freed
bool update_image_buffer_swapping(void) {
	bool valRet = false;

	for (int indImage = 0; indImage < PG_PHOTO_NB_TEXTURES_TVW;
		indImage++) {
		bool swapImages = false;
		// ongoing swapping
		// checks that the swapped image is loaded to begin swapping
		// the base image is a copy of a former swapped image
		if (pg_Photo_swap_buffer_data[indImage].swapping == true) {
			if (indImage < 3) {
				if (pg_Photo_alphaSwap02[indImage] < 1.0) {
					pg_Photo_alphaSwap02[indImage]
						= std::min(1.0f, (float)((CurrentClockTime
							- pg_Photo_swap_buffer_data[indImage].swapStart) /
							photo_diaporama_fade));
					//printf("Image %d time %.2f alpha %.2f (start %.2f dur %.2f)\n" , 
					//  indImage , CurrentClockTime , pg_Photo_alphaSwap02[indImage] ,
					//  pg_Photo_swap_buffer_data[indImage].swapStart,
					//  photo_diaporama_fade );
					valRet = true;
				}
				else {
					swapImages = true;
				}
			}
			else {
				if (pg_Photo_alphaSwap35[indImage - 3] < 1.0) {
					pg_Photo_alphaSwap35[indImage - 3]
						= std::min(1.0f, (float)((CurrentClockTime
							- pg_Photo_swap_buffer_data[indImage].swapStart) /
							photo_diaporama_fade));
					//printf("Image %d time %.2f alpha %.2f (start %.2f dur %.2f)\n" , 
					//  indImage , CurrentClockTime , pg_Photo_alphaSwap35[indImage - 3] ,
					//  pg_Photo_swap_buffer_data[indImage].swapStart,
					//  photo_diaporama_fade  );
					valRet = true;
				}
				else {
					swapImages = true;
				}
			}
		}
		// swapping is finished: the swapping alpha coeficient has reached 1.O
		// the swap image is exchanged with buffer image and the swp image (ex buffer image)
		// is released
		if (swapImages == true) {
			valRet = true;
			pg_Photo_swap_buffer_data[indImage].swapping = false; // makes it so that there is one frame 
																  // that displays the full swapped image

																  // exchanges texture IDs between buffer image and swap image
			int indTmp = pg_Photo_swap_buffer_data[indImage].indSwappedPhoto;
			pg_Photo_swap_buffer_data[indImage].indOldPhoto = pg_Photo_swap_buffer_data[indImage].indSwappedPhoto;
			pg_Photo_swap_buffer_data[indImage].indSwappedPhoto = indTmp;

			// resests swap weight
			if (indImage < 3) {
				pg_Photo_alphaSwap02[indImage] = 0.0f;
			}
			else {
				pg_Photo_alphaSwap35[indImage - 3] = 0.0f;
			}
		}
	}


	/*
	printf("swap %.1f %.1f %.1f %.1f %.1f %.1f \n", pg_Photo_alphaSwap02[0], pg_Photo_alphaSwap02[1], pg_Photo_alphaSwap02[2], pg_Photo_alphaSwap35[0], pg_Photo_alphaSwap35[1], pg_Photo_alphaSwap35[2]);

	char * ptr[6];
	for (int indImage = 0; indImage < PG_PHOTO_NB_TEXTURES_TVW;
	indImage++) {
	ptr[indImage] = strrchr(pg_Photo_buffer_data[indImage]->PhotoName, '/');
	ptr[indImage] = (ptr[indImage] ? ptr[indImage] + 1 : pg_Photo_buffer_data[indImage]->PhotoName);
	}
	printf("img %s %s %s %s %s %s \n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
	for (int indImage = 0; indImage < PG_PHOTO_NB_TEXTURES_TVW;
	indImage++) {
	ptr[indImage] = strrchr(pg_Photo_swap_buffer_data[indImage].PhotoName, '/');
	ptr[indImage] = (ptr[indImage] ? ptr[indImage] + 1 : pg_Photo_swap_buffer_data[indImage].PhotoName);
	}
	printf("swap [%s] [%s] [%s] [%s] [%s] [%s] \n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
	*/

	return valRet;
}

//////////////////////////////////////////////////////////////////
// LOOKS FOR AVAILABLE SWAP IMAGE

// tries to find a buffer that has been invalidated 
// by expanding the search around the image that is 
// opposite to random_image_buffer_index
// returns available index if not returns negative
int available_random_swap_image_buffer(int indOpposedCenter) {
	for (int indWindow = 0;
		indWindow < PG_PHOTO_NB_TEXTURES_TVW / 2 + 1;
		indWindow++) {
		int curInd = (indOpposedCenter + indWindow) % PG_PHOTO_NB_TEXTURES_TVW;
		if (pg_Photo_swap_buffer_data[curInd].swapping == false) {
			return curInd;
		}
		curInd = (indOpposedCenter - indWindow) % PG_PHOTO_NB_TEXTURES_TVW;
		if (pg_Photo_swap_buffer_data[curInd].swapping == false) {
			return curInd;
		}
	}
	return -1;
}
int available_swap_image_buffer(int indInitialImage) {
	for (int indWindow = indInitialImage;
		indWindow < indInitialImage + PG_PHOTO_NB_TEXTURES_TVW;
		indWindow++) {
		int curInd = indWindow % PG_PHOTO_NB_TEXTURES_TVW;
		if (pg_Photo_swap_buffer_data[curInd].swapping == false) {
			return curInd;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////
// LOADS BUFFER IMAGE FROM FILE

// threaded upload of the free swap image from a randomly chosen available layer
bool pg_swap_image(int indcomprImage) {
	if (pg_Photo_buffer_data[indcomprImage]->texBuffID == NULL_ID) {
		return false;
	}

	// we take in turn a new start index for available swap images so that it changes regularly
	int indAvailableSwapImage;
	if ((indAvailableSwapImage
		= available_random_swap_image_buffer(pg_IndInitialSwapPhoto)) >= 0) {
		// sets the start index for available image layer to the next layer
		pg_IndInitialSwapPhoto = (pg_IndInitialSwapPhoto + 1) % PG_PHOTO_NB_TEXTURES_TVW;

		// memorizes the index of the new image
		pg_Photo_swap_buffer_data[indAvailableSwapImage].indOldPhoto
			= pg_Photo_swap_buffer_data[indAvailableSwapImage].indSwappedPhoto;
		pg_Photo_swap_buffer_data[indAvailableSwapImage].indSwappedPhoto = indcomprImage;

		// trace
		int indOld = pg_Photo_swap_buffer_data[indAvailableSwapImage].indOldPhoto;
		char * ptr1 = strrchr(pg_Photo_buffer_data[indcomprImage]->PhotoName, '/');
		char * ptr2 = strrchr(pg_Photo_buffer_data[indOld]->PhotoName, '/');
		printf("Swap image (%s replaces %s layer %d (dur %.2f))\n",
			(ptr1 ? ptr1 + 1 : pg_Photo_buffer_data[indcomprImage]->PhotoName),
			(ptr2 ? ptr2 + 1 : pg_Photo_buffer_data[indOld]->PhotoName),
			indAvailableSwapImage, photo_diaporama_fade);

		// launches swapping
		pg_Photo_swap_buffer_data[indAvailableSwapImage].swapStart = CurrentClockTime;
		pg_Photo_swap_buffer_data[indAvailableSwapImage].swapping = true;
		if (indAvailableSwapImage < 3) {
			pg_Photo_alphaSwap02[indAvailableSwapImage] = 0.0f;
		}
		else {
			pg_Photo_alphaSwap35[indAvailableSwapImage - 3] = 0.0f;
		}
		LastImageSwap = CurrentClockTime;
		return true;
	}
	else {
		printf("Image swapping not possible (all layers currently swap)\n");
		return false;
	}
}


/////////////////////////////////////////////////////////
// INITIAL IMAGE UPLOADING
/////////////////////////////////////////////////////////
bool  pg_ReadInitalImageTexturesTVW(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder) {
	// printf("Image dir %d # %d dur %.2f\n" , dir_index, file_index, duration);
	std::string * fileName;
	bool valret = true;

	///////////////////////////////////////////////
	// NULL INITIALIZATIONS
	pg_nbCompressedImagesPerFolder = NULL;
	pg_firstCompressedFileInFolder = NULL;
	pg_Photo_buffer_data = NULL;
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;

	// assigns standard weights to each image buffer
	for (int ind = 0; ind < PG_PHOTO_NB_TEXTURES_TVW; ind++) {
		pg_Photo_weightTVW[ind] = 1.0f / PG_PHOTO_NB_TEXTURES_TVW;
	}

	////////////////////////////////////////////
	// CAPTURE DIAPORAMA
	if (pg_ImageDirectory.compare("captures") == 0) {
		std::cout << "Multilayer Diaporama loading completed 0 files." << std::endl;
		return false;
	}

	// loads all the associated masks
	for (int indBuffer = 0; indBuffer < PG_PHOTO_NB_TEXTURES_TVW / 3; indBuffer++) {
		if (*(pg_Photo_mask_buffer_data[indBuffer].PhotoName) == 0) {
			if (!pg_Photo_mask_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(pg_Photo_mask_buffer_data[indBuffer].texBuffID));
				pg_Photo_mask_buffer_data[indBuffer].IDallocated = true;

				int indDir = 0;
				int indFile = indBuffer;
				if ((fileName = nextFileIndexDiskLoop(&pg_MaskDirectory, &indDir, &indFile))) {
					strcpy(pg_Photo_mask_buffer_data[indBuffer].PhotoName,
						fileName->c_str());
					valret &= pg_Photo_mask_buffer_data[indBuffer].pg_loadPhoto(
						false, leftWindowWidth, window_height, true);
					delete fileName;
					fileName = NULL;

					pg_Photo_mask_buffer_data[indBuffer].pg_toGPUPhoto(true,
						GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR);

					printOglError(8);
				}
			}
		}
	}

	////////////////////////////////////////////
	// COUNTS IMAGES AND FOLDERS
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;
	if (nbImages <= 0 || nbFolders <= 0) {
		pg_nbCompressedImages = 0;
		pg_nbCompressedImageDirs = 0;
		std::cout << "Counting Diaporama Images " << std::endl;
		while ((fileName
			= nextFileIndexDiskNoLoop(&pg_ImageDirectory,
				&pg_CurrentDiaporamaDir, &pg_CurrentDiaporamaFile, maxFilesPerFolder))) {
			pg_nbCompressedImages++;
		}
		pg_nbCompressedImageDirs = pg_CurrentDiaporamaDir;
	}
	else {
		pg_nbCompressedImages = nbImages;
		pg_nbCompressedImageDirs = nbFolders;
	}

	////////////////////////////////////////////
	// LOADS IMAGES FROM FOLDERS
	if (pg_nbCompressedImageDirs <= 0) {
		std::cout << "Multilayer Diaporama loading completed 0 files." << std::endl;
		return false;
	}
	pg_nbCompressedImagesPerFolder = new int[pg_nbCompressedImageDirs];
	pg_firstCompressedFileInFolder = new int[pg_nbCompressedImageDirs];
	pg_Photo_buffer_data = new PhotoDataStruct *[pg_nbCompressedImages];
	for (int ind = 0; ind < pg_nbCompressedImages; ind++) {
		pg_Photo_buffer_data[ind] = new PhotoDataStruct();
	}
	for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
		pg_nbCompressedImagesPerFolder[ind] = 0;
		pg_firstCompressedFileInFolder[ind] = -1;
	}
	int indCompressedImage = 0;
	std::cout << "Loading Multilayer Diaporama " << pg_nbCompressedImages << " images from " << pg_nbCompressedImageDirs << " folders" << std::endl;
	if (pg_nbCompressedImageDirs != NbTextChapters) {
		printf("Incoherent text and image database!");
		exit(0);
	}
	while ((fileName
		= nextFileIndexDiskNoLoop(&pg_ImageDirectory,
			&pg_CurrentDiaporamaDir, &pg_CurrentDiaporamaFile, maxFilesPerFolder))
		&& indCompressedImage < pg_nbCompressedImages
		&& pg_CurrentDiaporamaDir < pg_nbCompressedImageDirs) {
		// std::cout << "file " << *fileName << std::endl;
		// counts files in dir
		pg_nbCompressedImagesPerFolder[pg_CurrentDiaporamaDir] = pg_CurrentDiaporamaFile;
		// if first file, stores the pointer to the file index, so that ID can be retrived 
		// later from index in folder
		if (pg_CurrentDiaporamaFile == 1) {
			pg_firstCompressedFileInFolder[pg_CurrentDiaporamaDir] = indCompressedImage;
		}
		if (*(pg_Photo_buffer_data[indCompressedImage]->PhotoName) == 0) {
			if (!pg_Photo_buffer_data[indCompressedImage]->IDallocated) {
				glGenTextures(1, &(pg_Photo_buffer_data[indCompressedImage]->texBuffID));
				pg_Photo_buffer_data[indCompressedImage]->IDallocated = true;

				strcpy(pg_Photo_buffer_data[indCompressedImage]->PhotoName,
					fileName->c_str());
				valret &= pg_Photo_buffer_data[indCompressedImage]->pg_loadPhoto(
					true, leftWindowWidth, leftWindowWidth, false);
				delete fileName;
				fileName = NULL;

				pg_Photo_buffer_data[indCompressedImage]->pg_toGPUPhoto(false,
					GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR);

				printOglError(8);
			}
		}
		indCompressedImage++;
	}
	std::cout << "Multilayer Diaporama loading completed " << pg_nbCompressedImages << " files." << std::endl;
	//std::cout << "Folders index/nbFiles/1stFileIndex";
	//for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
	//	std::cout << " " << ind << "/" << pg_nbCompressedImagesPerFolder[ind] << "/" << pg_firstCompressedFileInFolder[ind];
	//}
	//std::cout << std::endl;

	// INITIALIZES SWAP
	for (int indBuffer = 0; indBuffer < PG_PHOTO_NB_TEXTURES_TVW;
		indBuffer++) {
		pg_Photo_swap_buffer_data[indBuffer].swapping = false;
		pg_Photo_swap_buffer_data[indBuffer].indOldPhoto = indBuffer;
		pg_Photo_swap_buffer_data[indBuffer].indSwappedPhoto = indBuffer;
	}
	return valret;
}
#endif


bool  pg_ReadInitalImageTextures(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder) {
	std::string * fileName;
	bool valret = true;

	///////////////////////////////////////////////
	// NULL INITIALIZATIONS
	pg_nbCompressedImagesPerFolder = NULL;
	pg_firstCompressedFileInFolder = NULL;
	pg_Photo_buffer_data = NULL;
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;

	////////////////////////////////////////////
	// CAPTURE DIAPORAMA
	std::cout << "Directory name " << pg_ImageDirectory << std::endl;
	if (pg_ImageDirectory.compare("captures") == 0) {
		std::cout << "Multilayer Diaporama loading completed 0 files." << std::endl;
		return false;
	}

	////////////////////////////////////////////
	// COUNTS IMAGES AND FOLDERS
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;
	if (nbImages <= 0 || nbFolders <= 0) {
		pg_nbCompressedImages = 0;
		pg_nbCompressedImageDirs = 0;
		std::cout << "Counting Diaporama Images " << std::endl;
		while ((fileName
			= nextFileIndexDiskNoLoop(&pg_ImageDirectory,
				&pg_CurrentDiaporamaDir, &pg_CurrentDiaporamaFile, maxFilesPerFolder))) {
			// std::cout << "Image " << *fileName << " dir " << pg_CurrentDiaporamaDir << " file " << pg_CurrentDiaporamaFile << std::endl;
			pg_nbCompressedImages++;
		}
		pg_nbCompressedImageDirs = pg_CurrentDiaporamaDir;
	}
	else {
		pg_nbCompressedImages = nbImages;
		pg_nbCompressedImageDirs = nbFolders;
	}

	////////////////////////////////////////////
	// LOADS IMAGES FROM FOLDERS
	if (pg_nbCompressedImageDirs <= 0) {
		std::cout << "Multilayer Diaporama loading completed 0 files." << std::endl;
		return false;
	}
	pg_nbCompressedImagesPerFolder = new int[pg_nbCompressedImageDirs];
	pg_firstCompressedFileInFolder = new int[pg_nbCompressedImageDirs];
	pg_Photo_buffer_data = new PhotoDataStruct *[pg_nbCompressedImages];
	for (int ind = 0; ind < pg_nbCompressedImages; ind++) {
		pg_Photo_buffer_data[ind] = new PhotoDataStruct();
	}
	for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
		pg_nbCompressedImagesPerFolder[ind] = 0;
		pg_firstCompressedFileInFolder[ind] = -1;
	}
	int indCompressedImage = 0;
	std::cout << "Loading Multilayer Diaporama " << pg_nbCompressedImages << " images from " << pg_nbCompressedImageDirs << " folders" << std::endl;
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;
	while ((fileName
		= nextFileIndexDiskNoLoop(&pg_ImageDirectory,
			&pg_CurrentDiaporamaDir, &pg_CurrentDiaporamaFile, maxFilesPerFolder))
		&& indCompressedImage < pg_nbCompressedImages
		&& pg_CurrentDiaporamaDir < pg_nbCompressedImageDirs) {
		// std::cout << "file " << *fileName << std::endl;
		// counts files in dir
		pg_nbCompressedImagesPerFolder[pg_CurrentDiaporamaDir] = pg_CurrentDiaporamaFile;
		// if first file, stores the pointer to the file index, so that ID can be retrived 
		// later from index in folder
		if (pg_CurrentDiaporamaFile == 1) {
			pg_firstCompressedFileInFolder[pg_CurrentDiaporamaDir] = indCompressedImage;
		}
		if (*(pg_Photo_buffer_data[indCompressedImage]->PhotoName) == 0) {
			if (!pg_Photo_buffer_data[indCompressedImage]->IDallocated) {
				glGenTextures(1, &(pg_Photo_buffer_data[indCompressedImage]->texBuffID));
				pg_Photo_buffer_data[indCompressedImage]->IDallocated = true;

				// stores the photo file name
				strcpy(pg_Photo_buffer_data[indCompressedImage]->PhotoName,
					fileName->c_str());

				// loads the images with a size that corresponds to the screen size
				valret &= pg_Photo_buffer_data[indCompressedImage]->pg_loadPhoto(
					true, leftWindowWidth_powerOf2, window_height_powerOf2, false);

				// frees file name string memory
				delete fileName;
				fileName = NULL;

				pg_Photo_buffer_data[indCompressedImage]->pg_toGPUPhoto(false,
					GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR);
				// printf("texture ID indCompressedImage %d\n", pg_Photo_buffer_data[indCompressedImage]->texBuffID);

				printOglError(8);
			}
		}
		indCompressedImage++;
	}
	std::cout << "Multilayer Diaporama loading completed " << pg_nbCompressedImages << " files." << std::endl;
	//std::cout << "Folders index/nbFiles/1stFileIndex";
	//for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
	//	std::cout << " " << ind << "/" << pg_nbCompressedImagesPerFolder[ind] << "/" << pg_firstCompressedFileInFolder[ind];
	//}
	//std::cout << std::endl;

	// INITIALIZES SWAP
	for (int indBuffer = 0; indBuffer < PG_PHOTO_NB_TEXTURES;
		indBuffer++) {
		pg_Photo_swap_buffer_data[indBuffer].swapping = false;
		pg_Photo_swap_buffer_data[indBuffer].indOldPhoto = indBuffer;
		pg_Photo_swap_buffer_data[indBuffer].indSwappedPhoto = indBuffer;
	}
	return valret;
}
