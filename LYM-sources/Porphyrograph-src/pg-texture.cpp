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
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
PhotoDataStruct pg_Photo_buffer_data[PG_PHOTO_NB_TEXTURES]
= { PhotoDataStruct() , PhotoDataStruct() };
PhotoDataStruct pg_CameraFrame_buffer_data = PhotoDataStruct();
PhotoDataStruct pg_MovieFrame_buffer_data = PhotoDataStruct();

float pg_Photo_weight[PG_PHOTO_NB_TEXTURES] = { 0.0f , 0.0f };

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

std::string DiaporamaDir = "Data/" + project_name + "-data/photos/";
std::string MaskDir = "Data/TVW-data/masks/TVW-wandering";
std::string ImageDir = "Data/TVW-data/images/TVW-wandering";

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
			unsigned int fileNameLength = strlen(subdirp->d_name);
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
#if defined (TVW) || defined (CRITON)
	pg_displayMsg1Bitmap =
		(GLubyte *)pg_generateTexture(&pg_Display_Message1_Bitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg1Bitmap) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	glGenTextures(1, &(pg_Display_Message1_Bitmap_texID));

	pg_displayMsg2Bitmap =
		(GLubyte *)pg_generateTexture(&pg_Display_Message2_Bitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg2Bitmap) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	glGenTextures(1, &(pg_Display_Message2_Bitmap_texID));

	pg_loadTexture(display_font_file_name, &Display_Font_image,
		&Display_Font_texture_Rectangle_texID, true, false, GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		256, 86);
	printOglError(6);
#endif


	pg_screenMessageBitmap =
		(GLubyte *)pg_generateTexture(&pg_screenMessageBitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_screenMessageBitmap) {
		sprintf(ErrorStr, "Error: screen message bitmap not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
	}
	// glGenTextures( 1, &(pg_screenMessageBitmap_texID) );

	pg_loadTexture(screen_font_file_name, &Screen_Font_image,
		&Screen_Font_texture_Rectangle_texID, true, false, GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		128, 70);
	printOglError(6);


	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		pg_paths_Pos_Texture_texID[indPath] = 0;
		pg_paths_Pos_Texture[indPath] = NULL;
		pg_paths_Col_Texture_texID[indPath] = 0;
		pg_paths_Col_Texture[indPath] = NULL;
		pg_paths_RadBrushRendmode_Texture_texID[indPath] = 0;
		pg_paths_RadBrushRendmode_Texture[indPath] = NULL;
	}

	for (int indPath = 0; indPath < PG_NB_PATHS; indPath++) {
		pg_paths_Pos_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_Pos_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_Pos_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_Pos_Texture not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
		pg_paths_Col_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_Col_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_Col_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_Col_Texture not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
		pg_paths_RadBrushRendmode_Texture[indPath] =
			(GLfloat *)pg_generateTexture(&(pg_paths_RadBrushRendmode_Texture_texID[indPath]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_paths_RadBrushRendmode_Texture[indPath]) {
			sprintf(ErrorStr, "Error: pg_paths_RadBrushRendmode_Texture not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
		}
	}

	pg_loadTexture3D((char *)("Data/" + project_name + "-data/brushes/" + pen_brushes_fileName).c_str(),
		".png", nb_pen_brushes, 4, true,
		&Pen_texture_3D_texID, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 2048, nb_pen_brushes);
	printOglError(7);

#ifdef GN
	pg_loadTexture((char *)("Data/" + project_name + "-data/textures/LYMlogo.png").c_str(), &pg_LYMlogo_image,
		&pg_LYMlogo_texID, true, false, GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		1024, 768);
	printOglError(8);
#endif

	pg_loadTexture3D((char *)("Data/" + project_name + "-data/textures/Noise_" + project_name + "_3D").c_str(),
		".png", 2, 4, true,
		&Noise_texture_3D, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 1024, 2);
	printOglError(9);

#ifdef PG_SENSORS
	pg_loadTexture((char *)("Data/" + project_name + "-data/textures/sensor.v2.png").c_str(), &Sensor_image,
		&Sensor_texture_rectangle, true, false, GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		100, 100);
#endif

	for (int ind = 0; ind < PG_NB_PARTICLE_INITIAL_IMAGES; ind++) {
		string fileName = "Data/" + project_name + "-data/textures/PartInit_" + std::to_string(ind) + ".png";
		// printf("Loading particles initial images %s\n", fileName.c_str()); 
		generateParticleInitialPosColorRadiusfromImage(fileName,
			pg_particle_initial_images_texID[ind]); // 2 texture IDs one for pos/speed, the other one for color/radius
	}

#ifdef GN
#define width_data_table 600
#define height_data_table 200
	pg_CATable =
		(GLubyte *)pg_generateTexture(&pg_CATable_ID, pg_byte_tex_format,
			width_data_table, height_data_table);
	pg_CATable_values(pg_CATable_ID, pg_CATable,
		width_data_table, height_data_table);
	if (!pg_CATable_ID) {
		sprintf(ErrorStr, "Error: data tables for the CA bitmap not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
	}

	//printf( "\n" );
	//for( int i = 0 ; i <= 243 * 4 ; i+=4 ) {
	//  printf("%d " , (int)pg_CATable[ 4 * 51 * width_data_table + i ] );
	//}
	//printf( "\n" );
#endif

	// random positions of control points
	/* now made directly inside shader
	pg_Particle_Pos_Texture_texID = 0;
	pg_Particle_Pos_Texture =
		(GLfloat *)pg_generateTexture(&(pg_Particle_Pos_Texture_texID), pg_float_tex_format,
			leftWindowWidth, (nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 4) / leftWindowWidth + 1);
	// 4 floats per point because of RGBA format - BA can be used for other purposes
	if (!pg_Particle_Pos_Texture) {
		printf("Error: pg_Particle_Pos_Texture not allocated!"); throw 336;
	}
	pg_writeAndTreansferParticlePosition_Texture();
	*/

#ifdef CURVE_PARTICLES
// loads the curve comet texture
	string fileNameCurveText = "Data/" + project_name + "-data/textures/comet.png";
	printf("Loading %s\n", fileNameCurveText.c_str());
	pg_loadTexture(fileNameCurveText, &comet_image,
		&comet_texture_2D_texID, false, false, GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		512, 256);
#endif
#ifdef BLURRED_SPLAT_PARTICLES
	// loads the splat blurred disk texture
	string fileNameSplatText = "Data/" + project_name + "-data/textures/blurredDisk.png";
	printf("Loading %s\n", fileNameSplatText.c_str());
	pg_loadTexture(fileNameSplatText, &blurredDisk_image,
		&blurredDisk_texture_2D_texID, false, false, GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		256, 256);
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
			strcpy(ErrorStr, "Texture allocation error!"); ReportError(ErrorStr); throw 425;
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
			strcpy(ErrorStr, "Texture allocation error!"); ReportError(ErrorStr); throw 425;
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
	GLenum texturefilter,
	int width, int height, int depth) {
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
		cv::Mat result;
		if (invert)
			cv::flip(imgOpenCV, result, 0); // vertical flip
		else
			result = imgOpenCV;
#else
		cv::Mat imgOpenCV = cv::imread(filename, cv::IMREAD_UNCHANGED);   // Read the file
		int colorTransform = (imgOpenCV.channels() == 4) ? cv::COLOR_BGRA2RGBA :
			(imgOpenCV.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
		// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
		cv::Mat result;
		if (invert)
			cv::flip(imgOpenCV, result, 0); // vertical flip
		else
			result = imgOpenCV;
#endif

		if (!result.data) {                              // Check for invalid input
			sprintf(ErrorStr, "Could not open or find the image %s!", filename); ReportError(ErrorStr); throw 425;
			return false;
		}
		if (result.cols != width
			|| result.rows != height) {   // Check for invalid input
			sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!", filename, result.cols, width, result.rows, height); ReportError(ErrorStr); throw 425;
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
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		components,            // Components: Internal colour format to convert to
		width,          // Image width
		height,          // Image heigh
		depth,          // Image depth
		0,                 // Border width in pixels (can either be 1 or 0)
		format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		bitmap);        // The actual image data itself

	printOglError(0);

	// memory release
	delete[] bitmap;

	glEnable(GL_TEXTURE_RECTANGLE);

	// glGenerateMipmap(GL_TEXTURE_2D);
	// printf("3D textures loaded\n" );
	return true;
}

// 2D texture loading + transfer to GPU

bool pg_loadTexture(string fileName, cv::Mat *image,
	GLuint *textureID, bool is_rectangle,
	bool invert,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter,
	int width, int height) {

	// printf("Loading %s\n", fileName.c_str());

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
	if (invert)
		cv::flip(imgOpenCV, *image, 0); // vertical flip
	else
		*image = imgOpenCV;
#else
	cv::Mat imgOpenCV = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
	int colorTransform = (imgOpenCV.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(imgOpenCV.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
	if (imgOpenCV.channels() >= 3) {
		cv::cvtColor(imgOpenCV, imgOpenCV, colorTransform);
	}
	if (invert)
		cv::flip(imgOpenCV, *image, 0); // vertical flip
	else
		*image = imgOpenCV;
#endif

	if (!image->data) {                              // Check for invalid input
		sprintf(ErrorStr, "Could not open or find the image %s!", fileName.c_str()); ReportError(ErrorStr); throw 425;
		return false;
	}
	if (image->cols != width || image->rows != height) {                              // Check for invalid input
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!", fileName.c_str(), image->cols, width, image->rows, height); ReportError(ErrorStr); throw 425;
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
			image->cols,          // Image width
			image->rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			image->ptr());        // The actual image data itself
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
			image->cols,          // Image width
			image->rows,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			image->ptr());        // The actual image data itself
		printOglError(50);
		glEnable(GL_TEXTURE_RECTANGLE);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	return true;
}


//////////////////////////////////////////////////////////////////
// INITIALIZES PARTICLES FROM PHOTO
/////////////////////////////////////////////////////////////////

bool generateParticleInitialPosColorRadiusfromImage(string fileName,
	GLuint *textureParticleInitializationID) { // 2 texture IDs one for pos/speed, the other one for color/radius
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
		sprintf(ErrorStr, "Particle size should be lower than %d!", leftWindowWidth * window_height); ReportError(ErrorStr); throw 425;
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
	int partic_height = int(floor(sqrt(nb_particles * 3. / 4.)));
	int partic_width = int(floor(partic_height / 3. * 4.));
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
						Point3_<uchar>* p = image.ptr<Point3_<uchar> >(image.rows - indPixelRow - 1, indPixelCol);
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
			int indParticle = int(randomValue * nb_particles) % nb_particles;
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
			pos_speed[indParticle * 4 + 2] = randomValue - 0.5f;
			pos_speed[indParticle * 4 + 3] = randomValue - 0.5f;
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

	glGenTextures(2, textureParticleInitializationID);
	for (int indText = 0; indText < 2; indText++) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE,
			textureParticleInitializationID[indText]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		if (indText == 0) {
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, leftWindowWidth, window_height, 0,
				GL_RGBA, GL_FLOAT, pos_speed);
		}
		else {
			glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, leftWindowWidth, window_height, 0,
				GL_RGBA, GL_FLOAT, color_radius);
		}
	}

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
			pg_camera_frame_width = int(pg_camera_frame.rows * window_ratio);
			pg_camera_frame_height = pg_camera_frame.rows;
		}
		else {
			pg_camera_frame_width = pg_camera_frame.cols;
			if (pg_camera_frame.cols / window_ratio < pg_camera_frame.rows) {
				pg_camera_frame_height = int(pg_camera_frame.cols / window_ratio);
			}
			else {
				pg_camera_frame_height = pg_camera_frame.rows;
			}
		}
		printf("Camera frame %dx%d (before ratio %dx%d) ch %d size %d\n", pg_camera_frame_width, pg_camera_frame_height,
			pg_camera_frame.cols, pg_camera_frame.rows,
			pg_camera_frame.channels(), pg_camera_frame.total() * pg_camera_frame.elemSize());
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
		if (currentBGCapture || secondCurrentBGCapture) { // currentVideoTrack <=> video off
			printf("*** non threaded current BG video capture frame %d\n", pg_FrameNo);
			currentBGCapture = false;

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

	// pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	// pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	// higher values could be tried for width, fx 1280 x 720 

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
	//pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
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

	printf("Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Cam saturation %.2f\n", CameraCurrent_saturation);
	printf("Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Cam gamma      %.2f\n", CameraCurrent_gamma);
	printf("Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);

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

	printf("Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Cam saturation %.2f\n", CameraCurrent_saturation);
	printf("Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Cam gamma      %.2f\n", CameraCurrent_gamma);
	printf("Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);

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
	//cv::VideoCapture cap(("D:/cloud/LYM/" + "Data/" + project_name + "-data/videos/birds.avi" ).c_str());
	//if (!cap.isOpened()) {
	//	printf("cap movie file %s not opened!\n", ("D:/cloud/LYM/" + "Data/" + project_name + "-data/videos/birds.avi").c_str());
	//	return -1;
	//}

	string * fileName = (string *)lpParam;

	// film loading openCV
	pg_movie_capture.open(("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file %s not loaded!\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
		return NULL;
	}
	strcpy(pg_MovieFrame_buffer_data.PhotoName, ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());

	//Grabs and returns a frame from camera
	Mat new_movie_frame; //Preparing frame pointer 
	pg_movie_capture >> new_movie_frame;
	// texture loading
	if (!new_movie_frame.data) {
		printf("Movie frame %s not grabbed!\n", ("Data/" + project_name + "-data/videos/" + *fileName).c_str());
		return NULL;
	}
	if (!pg_initMovieFrameTexture(&new_movie_frame)) {
		printf("Movie frame not initialized!\n");
		return NULL;
	}
	pg_movie_frame_width = new_movie_frame.cols;
	pg_movie_frame_height = new_movie_frame.rows;

	//printf("Movie frame %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_nbFrames = int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT));
	is_movieLoading = false;
	return NULL;
}

#ifdef WIN32
DWORD WINAPI pg_movieLoop(LPVOID lpParam) {
#else
void* pg_movieLoop(void * lpParam) {
#endif
	pg_movie_capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	Mat new_movie_frame;
	pg_movie_capture >> new_movie_frame;

	pg_movie_nbFrames = int(pg_movie_capture.get(CV_CAP_PROP_FRAME_COUNT));
	printf("movie restarted\n");
	is_movieLooping = false;
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
		// printf("load diaporama initial files #%d\n", indPhoto);
		if (photoAlbumDirName[photo_diaporama].compare("captures") == 0) {
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
		}
	}

	// printOglError(469);

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
		float * photoWeight;
		if (indPhoto == 0) {
			photoWeight = pg_Photo_weight + 0;
		}
		else {
			photoWeight = pg_Photo_weight + 1;
		}

		// if blendStart is negative, there is no ongoing blending for the current photo
		// it will be started when the preceding one has reached
		// blendStart + photo_diaporama_fade + photo_diaporama_plateau + intervalDuration
		if (pg_Photo_swap_buffer_data[indPhoto].blendStart < 0.0f) {
			// not blending
			//printf("Photo %d blend start %.3f\n",
			//	indPhoto, pg_Photo_buffer_data[indPhoto].blendStart);
			*photoWeight = 0.0f;
			continue;
		}

		// ongoing blending
		float playingTime = CurrentClockTime - pg_Photo_swap_buffer_data[indPhoto].blendStart;
		*photoWeight = 0.0f;
		// incay and decay are 0 if is_capture_diaporama
		float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float interval = (is_capture_diaporama ? 0 : pg_Photo_swap_buffer_data[indPhoto].intervalDuration);
		//printf("Photo %d playing time %.3f\n",
		//	indPhoto, playingTime);
		if (playingTime < (incay + photo_diaporama_plateau + decay)) {
			if (playingTime < incay) {
				if (incay > 0.0f) {
					*photoWeight = playingTime / incay;
				}
				else {
					*photoWeight = 1.0f;
				}
			}
			else if (playingTime < incay + photo_diaporama_plateau) {
				*photoWeight = 1.0f;
			}
			else {
				if (decay > 0.0f) {
					*photoWeight = 1.0f - (playingTime - incay - photo_diaporama_plateau)
						/ decay;
				}
				else {
					*photoWeight = 1.0f;
				}
			}
			//if( indPhoto == 1)
				//printf("            "); 
			//char * ptr = strrchr(pg_Photo_buffer_data[indPhoto].fname, '/');
			//printf("blend %d weight %.2f playing time %.1f [%s]\n",
			//	indPhoto, *photoWeight, playingTime, ptr );
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

			// releases the blend image
			// imgPhotoBuffer[indPhoto].release();
		}

		// it is time to stop the current photo because we reach
		// the end of the blending duration (incay+plateau+decay) of the current one
		if (playingTime >= (incay + photo_diaporama_plateau + decay) ) {
			// stops blending
			pg_Photo_swap_buffer_data[indPhoto].blendStart = -1.0f;
			*photoWeight = 0.0f;
			// releases the image
			//  std::cout << "load next file for layer " << indPhoto << std::endl;
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

