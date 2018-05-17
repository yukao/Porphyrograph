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
// PhotoBufferDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
PhotoBufferDataStruct pg_Photo_buffer_data[PG_PHOTO_NB_TEXTURES]
= { PhotoBufferDataStruct(0) , PhotoBufferDataStruct(1) };
// cv::Mat imgPhotoBuffer[PG_PHOTO_NB_TEXTURES];
PhotoBufferDataStruct pg_CameraFrame_buffer_data = PhotoBufferDataStruct(0);
PhotoBufferDataStruct pg_MovieFrame_buffer_data = PhotoBufferDataStruct(0);

float pg_Photo_weight[PG_PHOTO_NB_TEXTURES] = { 0.0f , 0.0f };


////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
////////////////////////////////////////////////////////////////////
int GetFreeOpenGLContext( void ) {
  for( int indContext = 0 ; indContext < PG_MAX_NUMBER_OF_OPENGL_CONTEXTS ; indContext++ ) {
	if( ThreadOpenGLTextureData[indContext].is_free ) {
		ThreadOpenGLTextureData[indContext].is_free = false;
	  return indContext;
	}
  }
  return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

////////////////////////////////////////////////////////////////////
// DIAPORAMA IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
int IndPlayedDiaporamaFile = 0;
int CurrentDiaporamaDir = -1;
bool ascendingDiaporama = true;

std::string DiaporamaDir = "Data/" + project_name + "-data/photos/";

// FUNCTION FOR CLEARING A DIRECTORY
void remove_files_in_dir(std::string *dirpath) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << *dirpath << std::endl;
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

// FUNCTION FOR CHECKING THAT A PATH IS AN OPENABLE FILE IN A DIRECTORY
bool is_subfile(struct dirent *dirp, std::string *dirpath) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory (or is in some way invalid) we'll skip it 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (!S_ISREG(filestat.st_mode)) { // the file is not a regular file
		return false;
	}

	ifstream fin;
	fin.open(filepath.c_str());
	if (fin) {
		fin.close();
		return true;
	}

	return false;
}

// FINDS THE NTH FILE IN A DIR
bool is_image_dir_file(string *dirpath, int *indfile, string *fileName, bool ascending) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << *dirpath << std::endl;
		return false;
	}
	// std::cout << std::endl << "dir to get files of: " + *dirpath << std::endl;
	// scans all the subdirectories until finding one that has the current 
	// index + 1 as substring, typically 001_XXXX
	struct dirent *subdirp;
	int indFileInDir = -2; // because, it first encounters the pseudo subfiles ./ and ../
	while ((subdirp = readdir(dp))) {
		if (indFileInDir == *indfile) {
			// checks whether it is an image file
			unsigned int fileNameLength = strlen(subdirp->d_name);
			if (fileNameLength < 4
				|| (strcmp(subdirp->d_name + fileNameLength - 4, ".jpg") != 0
					&& strncmp(subdirp->d_name + fileNameLength - 4, ".png", 4) != 0)) {
				// printf("Non image file(%s)\n", subdirp->d_name);
				if (ascending) {
					(*indfile)++;
				}
				else {
					(*indfile)--;
					closedir(dp);
					indFileInDir = -2;
					dp = opendir(dirpath->c_str());
				}
			}

			// checks that it is an openable file
			else if (is_subfile(subdirp, dirpath)) {
				*fileName = *dirpath + "/" + subdirp->d_name;
				// std::cout << "found: (" << indfile << "): " << *fileName << std::endl;
				closedir(dp);
				if (ascending) {
					(*indfile)++;
				}
				else {
					(*indfile)--;
				}
				return true;
			}
		}
		indFileInDir++;
	}
	// ends of the dir without finding the file, the file index will be reset
	closedir(dp);
	return false;
}

// COUNTS THE FILES IN A DIR
int count_dir_file(string *dirpath) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << *dirpath << std::endl;
		return false;
	}
	// std::cout << std::endl << "dir to get files of: " + *dirpath << std::endl;
	// scans all the subdirectories until finding one that has the current 
	// index + 1 as substring, typically 001_XXXX
	struct dirent *subdirp;
	int indFileInDir = -2; // because, it first encounters the pseudo subfiles ./ and ../
	while ((subdirp = readdir(dp))) {
		indFileInDir++;
	}
	// ends of the dir without finding the file, the file index will be reset
	closedir(dp);
	return indFileInDir;
}

// FINDS THE NEXT FILE IN A DIR
bool nextFile( int *currentFileIndex , string *fileName , bool ascending ) {
	if (photo_diaporama < 0 || photo_diaporama >= nb_photo_albums) {
		return false;
	}

	string currrentPhotoDir = "";
	if (photoAlbumDirName[photo_diaporama].compare("captures") == 0) {
		currrentPhotoDir = snapshots_dir_path_name;
		is_capture_diaporama = true;
	}
	else {
		currrentPhotoDir = DiaporamaDir + photoAlbumDirName[photo_diaporama];
		is_capture_diaporama = false;
	}

	// next file in the same dir
	if ((is_image_dir_file( &currrentPhotoDir , currentFileIndex , fileName, ascending))) {
		return true;
	}
	else {
		if (ascending) {
			// first file in the same dir
			(*currentFileIndex) = 0;
		}
		else {
			// last file in the same dir
			(*currentFileIndex) = count_dir_file(&currrentPhotoDir) - 1;
		}
		if ((is_image_dir_file( &currrentPhotoDir , currentFileIndex, fileName, ascending))) {
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE INITIALIZATION
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_initTextures(void) {
	pg_screenMessageBitmap =
		(GLubyte *)pg_generateTexture(&pg_screenMessageBitmap_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_screenMessageBitmap) {
		sprintf(ErrorStr, "Error: screen message bitmap not allocated (%s)!", ScreenMessage); ReportError(ErrorStr); throw 336;
	}
	// glGenTextures( 1, &(pg_screenMessageBitmap_texID) );

	for (int indPath = 0; indPath < PG_NB_TRACKS; indPath++) {
		pg_paths_Pos_Texture_texID[indPath] = 0;
		pg_paths_Pos_Texture[indPath] = NULL;
		pg_paths_Col_Texture_texID[indPath] = 0;
		pg_paths_Col_Texture[indPath] = NULL;
		pg_paths_RadBrushRendmode_Texture_texID[indPath] = 0;
		pg_paths_RadBrushRendmode_Texture[indPath] = NULL;
	}

	for (int indPath = 0; indPath < PG_NB_TRACKS; indPath++) {
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

	pg_loadTexture(font_file_name, &Font_image,
		&Font_texture_Rectangle_texID, true, false, GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		128, 70);
	printOglError(6);

	pg_loadTexture3D((char *)("Data/" + project_name + "-data/brushes/" + pen_brushes_fileName).c_str(),
		".png", nb_pen_brushes, 4, true,
		&Pen_texture_3D_texID, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 2048, nb_pen_brushes);
	printOglError(7);

	pg_loadTexture3D((char *)("Data/" + project_name + "-data/textures/Noise_" + project_name + "_3D").c_str(),
		".png", 2, 4, true,
		&Noise_texture_3D, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 1024, 2);
	printOglError(9);

	for (int ind = 0; ind < PG_NB_PARTICLE_INITIAL_IMAGES; ind++) {
		generateParticleInitialPosColorRadiusfromImage("Data/" + project_name + "-data/textures/PartInit_" + std::to_string(ind) + ".png",
			pg_particle_initial_images_texID[ind]); // 2 texture IDs one for pos/speed, the other one for color/radius
	}


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
	string fName = "./song-data/textures/comet.png";
	printf("Loading %s\n", fName.c_str());
	pg_loadTexture(fName, &comet_image,
		&comet_texture_2D_texID, false, false, GL_RGAB8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		512, 256);
#endif
#ifdef BLURRED_SPLAT_PARTICLES
	// loads the splat blurred disk texture
	string fName = "Data/" + project_name + "-data/textures/blurredDisk.png";
	printf("Loading %s\n", fName.c_str());
	pg_loadTexture(fName, &blurredDisk_image,
		&blurredDisk_texture_2D_texID, false, false, GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR,
		256, 256);
#endif
	printOglError(6);

	return true;
}


// WRITES INITIAL CURVES TO TEXTURES FOR DRAWING AN INITIAL SET OF CURVES
/* NOW MADE DIRECTLY INSIDE THE SHADER
void pg_writeAndTreansferParticlePosition_Texture(void) {
	// the used width is a multiple of (PG_PARTICLE_CURVE_DEGREE + 1) so that each set of control
	// point coordinates is on a single line
	int width_used = leftWindowWidth - leftWindowWidth % (PG_PARTICLE_CURVE_DEGREE + 1);

	// sets the values of the texture
	int index = 0;
	for (int indCurve = 0; indCurve < nb_particles; indCurve++) {
		pg_Particle_Pos_Texture[index + 0] = randomValue * leftWindowWidth;
		pg_Particle_Pos_Texture[index + 1] = randomValue * leftWindowWidth;
		pg_Particle_Pos_Texture[index + 2] = 0.f;
		pg_Particle_Pos_Texture[index + 3] = 0.f;
		index += 4;  // RGBA
		for (int indControlPoint = 0; indControlPoint < PG_PARTICLE_CURVE_DEGREE; indControlPoint++) {
			pg_Particle_Pos_Texture[index + 0] = pg_Particle_Pos_Texture[index - 4 + 0] + randomValue * 20.f - 10.f;
			pg_Particle_Pos_Texture[index + 1] = pg_Particle_Pos_Texture[index - 4 + 1] + randomValue * 20.f - 10.f;
			pg_Particle_Pos_Texture[index + 2] = 0.f;
			pg_Particle_Pos_Texture[index + 3] = 0.f;
			index += 4; // RGBA
		}
		// jumps over the end of the line in case the texture width is
		// not a multiple of(PG_PARTICLE_CURVE_DEGREE + 1) so that each set 
		// of control point coordinates is on a single line
		while (index % leftWindowWidth >= width_used) {
			index++;
		}
	}

	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_Particle_Pos_Texture_texID);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA32F,            // Components: Internal colour format to convert to
		leftWindowWidth,          // Image width
		(nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 4) / leftWindowWidth + 1,          // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_FLOAT,  // Image data type
		(GLvoid *)pg_Particle_Pos_Texture);        // The actual image data itself

	printf("Bezier Curve control point position texture values text size %dx%d\n",
		leftWindowWidth, (nb_particles * (PG_PARTICLE_CURVE_DEGREE + 1) * 4) / leftWindowWidth + 1);

	printOglError(12);
}
*/


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
	cv::flip(*(pDataArray->img), result, 0); // vertical flip
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

	delete pDataArray->img;
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
	cv::flip(*(pDataArray->img), result, 0); // vertical flip
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

	delete pDataArray->img;
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
		cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
		// printf("nb channels %d\n", img.channels());
		int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
			(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(img, img, colorTransform);
		cv::Mat result;
		if (invert)
			cv::flip(img, result, 0); // vertical flip
		else
			result = img;
#else
		cv::Mat img = cv::imread(filename, cv::IMREAD_UNCHANGED);   // Read the file
		int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
			(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
		// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
		cv::cvtColor(img, img, colorTransform);
		cv::Mat result;
		if (invert)
			cv::flip(img, result, 0); // vertical flip
		else
			result = img;
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
	cv::Mat img = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	// printf("File %s nb channels %d\n", fileName.c_str(), img.channels());
	int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
		(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, *image, 0); // vertical flip
	else
		*image = img;
#else
	cv::Mat img = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
	int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, *image, 0); // vertical flip
	else
		*image = img;
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
		printOglError(4);
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
		printOglError(5);
	}
	// glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}

//////////////////////////////////////////////////////////////////
// INITIALIZES PARTICLES FROM PHOTO
/////////////////////////////////////////////////////////////////

bool generateParticleInitialPosColorRadiusfromImage(string fileName,
	GLuint *textureParticleInitializationID) { // 2 texture IDs one for pos/speed, the other one for color/radius
	bool invert = false;
	cv::Mat image;
#ifndef OPENCV_3
	cv::Mat img = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	cv::Mat img = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
																// Check for invalid input
	if (!img.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // ReportError(ErrorStr); throw 425;
		return false;
	}
	int nbChannels = img.channels();
	// printf("File %s nb channels %d\n", fileName.c_str(), nbChannels);
	int colorTransform = (nbChannels == 4) ? CV_BGRA2RGBA :
		(nbChannels == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (nbChannels == 4) ? GL_RGBA : GL_RGB;
	if (nbChannels >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (invert)
		cv::flip(img, image, 0); // vertical flip
	else
		image = img;

	if (nb_particles > leftWindowWidth* window_height) {
		sprintf(ErrorStr, "Particle size should be lower than %d!", leftWindowWidth* window_height); ReportError(ErrorStr); throw 425;
		return false;
	}

	float *pos_speed = new float[leftWindowWidth* window_height * 4];
	float *color_radius = new float[leftWindowWidth* window_height * 4];
	bool *allocated = new bool[leftWindowWidth* window_height];
	for (int i = 0; i < leftWindowWidth* window_height; i++) {
		allocated[i] = false;
	}

	// for a 4:3 image- should be modified for another ratio
	int partic_height = int(floor(sqrt(nb_particles * 3. / 4.)));
	int partic_width = int(floor(partic_height / 3.*4.));
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
	printf("Loading %s\n", fileName.c_str() );

	free(color_radius);
	free(pos_speed);
	free(allocated);

	return true;
}

//////////////////////////////////////////////////////////////////
// LOADS PHOTO FROM FILE
/////////////////////////////////////////////////////////////////

// threaded upload of the free blend image from a randomly chosen available layer
bool pg_load_photo_layer(std::string * fileName, int indPhoto, bool threaded) {
	const char * ptr1 = strrchr(fileName->c_str(), '/');
	printf("Loading [%s] layer #%d\n", (ptr1 ? ptr1 + 1 : fileName->c_str()), indPhoto);

	strcpy(pg_Photo_buffer_data[indPhoto].fname, fileName->c_str());
	if (!pg_Photo_buffer_data[indPhoto].pg_loadPhoto(threaded, false)) {
		return false;
	}
	// printOglError(419);

	return true;
}

/////////////////////////////////////////////////////////////////
// THREADED 2D TEXTURE LOADING
/////////////////////////////////////////////////////////////////

// 2D texture loading

bool PhotoBufferDataStruct::pg_loadPhoto(bool threaded, bool verbose) {
	bool valret = true;
	const char * ptr = strrchr(fname, '/');

	if (!texBuffID) {
		glGenTextures(1, &(texBuffID));
	}

	// if (verbose) 
	{
		printf("Loading [%s] threaded %d\n", (ptr ? ptr + 1 : fname),int(threaded));
	}

	// printOglError(431);
		
	// thread for the transfer to GPU
	if (threaded) {
		// thread for the transfer to GPU
		int indFreeOpenGLContext = GetFreeOpenGLContext();
		// printf("opengl context #%d\n", indFreeOpenGLContext);
		
		if (indFreeOpenGLContext >= 0) {
			transfering = true;
			transferred = false;
			ThreadOpenGLTextureData[indFreeOpenGLContext].fname = fname;
			ThreadOpenGLTextureData[indFreeOpenGLContext].photo_buffer_data = this;
			ThreadOpenGLTextureData[indFreeOpenGLContext].textureID = texBuffID;
			ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
			ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB;
			ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
			ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;
			// printf("Load img text ID %d\n", texBuffID);

			// Assuming that this is the main thread
			ThreadOpenGLTextureData[indFreeOpenGLContext].hdc = wglGetCurrentDC(); // GetDC(hwnd);
			HGLRC mainContext = wglGetCurrentContext();
			if (!ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext) {
				ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext
					= wglCreateContext(ThreadOpenGLTextureData[indFreeOpenGLContext].hdc);
			}
			wglShareLists(mainContext, ThreadOpenGLTextureData[indFreeOpenGLContext].loaderContext); // Order matters


#ifdef WIN32
			DWORD rc;
			HANDLE  hThread = CreateThread(
				NULL,                   // default security attributes
				0,                      // use default stack size  
				pg_threaded_loadPhotoData,		    // thread function name
				(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function 
				0,                      // use default creation flags 
				&rc);   // returns the thread identifier 
			if (hThread == NULL) {
				std::cout << "Error:unable to create thread pg_threaded_loadPhotoData" << std::endl;
				*fname = 0;
				return NULL;
			}
			CloseHandle(hThread);
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			// should only be closed when the thread is finished + assigned NULL value
			// make code so that only one reading (or writing thread) is called at a time
			// because of concurrent call to glTexImage2D that might be a source of problem
			// a texture has been loaded in RAM
			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#else
			pthread_t reading_thread;
			int rc;
			rc = pthread_create(&reading_thread, NULL,
				pg_threaded_loadPhotoData, (void *)(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
			if (rc) {
				std::cout << "Error:unable to create thread pg_threaded_loadPhotoData" << rc << std::endl;
				pthread_exit(NULL);
			}
#endif
		}
		// falls back to non threaded loading if no opengl context is available
		else {
			cv::Mat imgData = cv::imread(fname, CV_LOAD_IMAGE_COLOR);   // Read the file;
   		    // Check for invalid input
			if (!imgData.data) {
				printf("Could not open or find the 2D image (%s)!\n", fname); // ReportError(ErrorStr); throw 425;
				transferred = false;
				transfering = false;
				imgData.release();
				*(fname) = 0;
			}
			// Store image size
			w = imgData.cols;
			h = imgData.rows;
			// printf("Loaded [%s]\n", (ptr ? ptr + 1 : fname));

			GLenum format = (imgData.channels() == 4) ? GL_BGRA :
				(imgData.channels() == 3) ? GL_BGR : GL_LUMINANCE;

			pg_sendGPUPhotoData(texBuffID, true, GL_RGB, format,
				GL_UNSIGNED_BYTE, GL_LINEAR, 
				imgData.cols , imgData.rows, 
				(char *)(imgData.ptr()));

			// printf("Transfered GPU [%s]\n", (ptr ? ptr + 1 : fname));

			// releases openCV image
			imgData.release();
		}
	}
	else {
		cv::Mat imgData = cv::imread(fname, CV_LOAD_IMAGE_COLOR);   // Read the file;
																	// Check for invalid input
		// printf("Loaded [%s]\n", (ptr ? ptr + 1 : fname));

		if (!imgData.data || imgData.empty()) {
			printf("Could not open or find the 2D image (%s)!\n", fname); // ReportError(ErrorStr); throw 425;
			transferred = false;
			transfering = false;
			imgData.release();
			*(fname) = 0;
		}
		// Store image size
		w = imgData.cols;
		h = imgData.rows;

		GLenum format = (imgData.channels() == 4) ? GL_BGRA :
			(imgData.channels() == 3) ? GL_BGR : GL_LUMINANCE;

		// printf("Transfer GPU [%s] %dx%d\n", (ptr ? ptr + 1 : fname) , w , h);

		pg_sendGPUPhotoData(texBuffID, true, GL_RGB, format,
			GL_UNSIGNED_BYTE, GL_LINEAR, 
			imgData.cols , imgData.rows, 
			(char *)(imgData.ptr()));

		// printf("Transfered GPU [%s]\n", (ptr ? ptr + 1 : fname));

		// releases openCV image
		imgData.release();

		// printf("Released [%s]\n", (ptr ? ptr + 1 : fname));
	}
	return valret;
}

//////////////////////////////////////////////
// 2D opencv texture loading + GPU transfer

#ifdef WIN32
DWORD WINAPI pg_threaded_loadPhotoData(LPVOID lpParam) {
#else
void* pg_threaded_loadPhotoData(void * lpParam) {
#endif
	struct ThreadTextureData *texData = (struct ThreadTextureData *)lpParam;

	///////////////////////////////////////////
	// sets the current OpenGL context
	wglMakeCurrent(texData->hdc, texData->loaderContext);
	// printOglError(499);

	cv::Mat imgData = cv::imread(texData->fname, CV_LOAD_IMAGE_COLOR);   // Read the file;
																		 // Check for invalid input
																		 // printf("Loaded thread [%s]\n", (ptr ? ptr + 1 : texData->photo_buffer_data->fname));
	if (imgData.empty()) {
		printf("Could not open or find the 2D image (%s)!\n", texData->fname); // ReportError(ErrorStr); throw 425;
		texData->photo_buffer_data->transfering = false;
		texData->photo_buffer_data->transferred = false;
		imgData.release();
		*(texData->fname) = 0;
	}
	// Store image size
	texData->photo_buffer_data->w = imgData.cols;
	texData->photo_buffer_data->h = imgData.rows;

	// printOglError(456);

	GLenum format = (imgData.channels() == 4) ? GL_BGRA :
		(imgData.channels() == 3) ? GL_BGR : GL_LUMINANCE;

	//char * ptr = strrchr(fname, '/');
	// printf("Threaded Transfer of image to GPU (%s)\n", texData->photo_buffer_data->fname);
	texData->photo_buffer_data->pg_sendGPUPhotoData(texData->textureID, texData->is_rectangle,
		texData->components, format,
		texData->datatype, texData->texturefilter, 
		imgData.cols , imgData.rows , 
		(char *)(imgData.ptr()) );

	// releases openCV image
	imgData.release();

	// frees the opengl context
	texData->is_free = true;

	// indicates that transfer is finished
	texData->photo_buffer_data->transferred = true;
	texData->photo_buffer_data->transfering = false;

	// releases OpenGL context
	//wglDeleteContext(texData->loaderContext);
	//texData->loaderContext = NULL;
	wglMakeCurrent(NULL, NULL);

	// delete pDataArray;
#ifndef WIN32
	pthread_exit(NULL);
#endif
	return 0;
}

//////////////////////////////////////////////
/// NON THREADED LOAD CAMERA FRAME

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
		pg_camera_frame_width = pg_camera_frame.cols;
		pg_camera_frame_height = pg_camera_frame.rows;
		printf("Camera frame %dx%d ch %d size %d\n", pg_camera_frame.cols, pg_camera_frame.rows, 
			pg_camera_frame.channels(), pg_camera_frame.total() * pg_camera_frame.elemSize());
	}
	//else {
	//	printf("Camera frame %dx%d ch %d size %d\n", pg_camera_frame.cols, pg_camera_frame.rows,
	//		pg_camera_frame.channels(), pg_camera_frame.total() * pg_camera_frame.elemSize());
	//}
	//pg_camera_capture.set(CV_CAP_PROP_FRAME_WIDTH, pg_camera_frame.cols);
	//pg_camera_capture.set(CV_CAP_PROP_FRAME_HEIGHT, pg_camera_frame.rows);

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
		if (currentBGCapture || secondCBGCapture) { // currentVideoTrack <=> video off
			printf("*** non threaded current BG video capture frame %d\n" , pg_FrameNo);
			currentBGCapture = false;

			// makes a second video capture 
			secondCBGCapture = !secondCBGCapture;

			// current background memory
			// printf("Threaded Transfer of camera BG to GPU (%s)\n", texData->photo_buffer_data->fname);
			glEnable(GL_TEXTURE_RECTANGLE);
			glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_currentBG_texture_texID);
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
	}
}

//////////////////////////////////////////////
/// THREAD LOAD CAMERA FRAME

#ifdef WIN32
DWORD WINAPI pg_threaded_loadCameraFrame(LPVOID lpParam) {
#else
void* pg_threaded_loadCameraFrame(void * lpParam) {
#endif
	struct ThreadTextureData *texData = (struct ThreadTextureData *)lpParam;

	///////////////////////////////////////////
	// sets the current OpenGL context
	wglMakeCurrent(texData->hdc, texData->loaderContext);
	// printOglError(499);

	Mat camera_frame;
	pg_camera_capture >> camera_frame;

	if (camera_frame.data) {
		// Store image size
		texData->photo_buffer_data->w = pg_camera_frame_width;
		texData->photo_buffer_data->h = pg_camera_frame_height;

		// printOglError(456);

		//char * ptr = strrchr(fname, '/');
		// printf("Threaded Transfer of camera frame to GPU (%s)\n", texData->photo_buffer_data->fname);
		texData->photo_buffer_data->pg_sendGPUPhotoData(texData->textureID,
			texData->is_rectangle,
			texData->components, GL_RGB,
			texData->datatype, texData->texturefilter,
			pg_camera_frame_width, pg_camera_frame_height,
			(char *)camera_frame.data);

		// /////////////////////////
		// reads BG camera frame for image subtraction
		if (currentBGCapture || secondCBGCapture) { // currentVideoTrack <=> video off
			// printf("**** threaded current BG video capture %d\n", pg_FrameNo);
			currentBGCapture = false;

			// makes a second video capture 
			secondCBGCapture = !secondCBGCapture;

			// current background memory
			// printf("Threaded Transfer of camera BG to GPU (%s)\n", texData->photo_buffer_data->fname);
			texData->photo_buffer_data->pg_sendGPUPhotoData(pg_camera_currentBG_texture_texID,
				texData->is_rectangle,
				texData->components, GL_RGB,
				texData->datatype, texData->texturefilter,
				pg_camera_frame_width, pg_camera_frame_height,
				(char *)camera_frame.data);
		}
	}

	// frees the opengl context
	texData->is_free = true;

	// indicates that transfer is finished
	texData->photo_buffer_data->transferred = true;
	texData->photo_buffer_data->transfering = false;

	// releases OpenGL context
	//wglDeleteContext(texData->loaderContext);
	//texData->loaderContext = NULL;
	wglMakeCurrent(NULL, NULL);

	// delete pDataArray;
#ifndef WIN32
	pthread_exit(NULL);
#endif
	return 0;
}

//////////////////////////////////////////////
/// THREAD LOAD MOVIE FRAME

#ifdef WIN32
DWORD WINAPI pg_threaded_loadMovieFrame(LPVOID lpParam) {
#else
void* pg_threaded_loadMovieFrame(void * lpParam) {
#endif
	struct ThreadTextureData *texData = (struct ThreadTextureData *)lpParam;

	///////////////////////////////////////////
	// sets the current OpenGL context
	wglMakeCurrent(texData->hdc, texData->loaderContext);
	// printOglError(499);

	Mat movie_frame;
	pg_movie_capture >> movie_frame;
	pg_movie_nbFrames--;
	// printf("Capture of movie frame %d\n",pg_movie_nbFrames);

	if (movie_frame.data) {
		// Store image size
		texData->photo_buffer_data->w = pg_movie_frame_width;
		texData->photo_buffer_data->h = pg_movie_frame_height;

		// printOglError(456);

		//char * ptr = strrchr(fname, '/');
		// printf("Threaded Transfer of movie frame %d %dx%d to GPU (%s)\n", 
		// 	pg_movie_nbFrames , pg_movie_frame_width, pg_movie_frame_height,texData->photo_buffer_data->fname);
		texData->photo_buffer_data->pg_sendGPUPhotoData(texData->textureID,
			texData->is_rectangle,
			texData->components, GL_BGR,
			texData->datatype, texData->texturefilter,
			pg_movie_frame_width, pg_movie_frame_height,
			(char *)movie_frame.data);
	}

	// frees the opengl context
	texData->is_free = true;

	// indicates that transfer is finished
	texData->photo_buffer_data->transferred = true;
	texData->photo_buffer_data->transfering = false;

	// releases OpenGL context
	//wglDeleteContext(texData->loaderContext);
	//texData->loaderContext = NULL;
	wglMakeCurrent(NULL, NULL);

	// delete pDataArray;
#ifndef WIN32
	pthread_exit(NULL);
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////
// TRANSFERS BUFFER TO GPU

bool PhotoBufferDataStruct::pg_sendGPUPhotoData(GLuint textureID, bool is_rectangle,
		GLint components, GLenum format,
		GLenum datatype, GLenum texturefilter,
		int thewidth , int theheight ,
		char* imgdata) {
	bool valret = true;

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			thewidth,          // Image width
			theheight,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			imgdata);        // The actual image data itself
							 // printf("-> end of image to GPU rect ID %d %dx%d  %s\n", textureID, w, h, fname);
		printOglError( 4 );
		valret = false;
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			thewidth,          // Image width
			theheight,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			imgdata);        // The actual image data itself
		printOglError( 5 );
		valret = false;
	}


	// when this boolean is set to true, the content of the image has been transfered to GPU
	transferred = true;
	transfering = false;

	// glGenerateMipmap(GL_TEXTURE_2D);
	return valret;
}

/////////////////////////////////////////////////////////////////
// VIDEO FRAME AND CAPTURE INITIALIZATION (CAMERA AND MOVIE)
/////////////////////////////////////////////////////////////////

#ifdef PG_VIDEO_ACTIVE
bool pg_initCameraFrameTexture(Mat *video_frame) {
	glEnable(GL_TEXTURE_2D);

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
	if (!pg_camera_currentBG_texture_texID) {
		glGenTextures(1, &pg_camera_currentBG_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_currentBG_texture_texID);
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

bool pg_initMovieFrameTexture(Mat *movie_frame) {
	glEnable(GL_TEXTURE_2D);

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
	CameraCurrent_WB_B = (float)pg_camera_capture.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U);
	CameraCurrent_WB_R = (float)pg_camera_capture.get(CV_CAP_PROP_WHITE_BALANCE_RED_V);

	printf("Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Cam saturation %.2f\n", CameraCurrent_saturation);
	printf("Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);

	// pg_camera_capture.set(CV_CAP_PROP_AUTO_EXPOSURE, 1);
	//pg_camera_capture.set(CV_CAP_PROP_EXPOSURE, -5.0);
	//pg_camera_capture.set(CV_CAP_PROP_GAIN, 0.0);
	//pg_camera_capture.set(CV_CAP_PROP_BRIGHTNESS, 0.0);
	//pg_camera_capture.set(CV_CAP_PROP_SATURATION, 16.0);
	//pg_camera_capture.set(CV_CAP_PROP_CONTRAST, 10.0);
	//pg_camera_capture.set(CV_CAP_PROP_FPS, 60);
	//pg_camera_capture.set(CV_CAP_PROP_FOCUS, 28);
	//pg_camera_capture.set(CameraCurrent_WB_B, 0);
	//pg_camera_capture.set(CameraCurrent_WB_R, 0);

	return 1;
}

#ifdef WIN32
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam) {
#else
void* pg_initVideoMoviePlayback(void * lpParam) {
#endif
	//cv::VideoCapture cap(("Data/" + project_name + "-data/videos/birds.avi" ).c_str());
	//if (!cap.isOpened()) {
	//	printf("cap movie file %s not opened!\n", ("Data/" + project_name + "-data/videos/birds.avi").c_str());
	//	return -1;
	//}

	string * fileName = (string *)lpParam;

	// film loading openCV
	pg_movie_capture.open(("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file %s not loaded!\n", ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());
		return NULL;
	}
	strcpy(pg_MovieFrame_buffer_data.fname, ("Data/" + project_name + "-data/videos/" + movieFileName[currentlyPlaying_movieNo]).c_str());

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

#endif



/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// PROCESSES PHOTO DIAPORAMA

// initializes the blending of each image inside a folder
void pg_launch_diaporama(void) {
	// printf("launch diaporama\n");

	// printOglError(467);

	// loads the first images if it is not the case
	string fileName;
	IndPlayedDiaporamaFile = 0;
	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		if (!pg_Photo_buffer_data[indPhoto].texBuffID) {
			glGenTextures(1, &(pg_Photo_buffer_data[indPhoto].texBuffID));
		}

		// printOglError(468);

		// printf("load diaporama initial files #%d\n", indPhoto);
		if (photoAlbumDirName[photo_diaporama].compare("captures") == 0) {
			ascendingDiaporama = false;
			// if ((nextFile(&IndPlayedDiaporamaFile, &fileName, ascendingDiaporama))) {
				if ((nextFile(&IndPlayedDiaporamaFile, &fileName, ascendingDiaporama))) {
					// printf("load diaporama initial files name %s\n", fileName->c_str());
					if (pg_load_photo_layer(&fileName, indPhoto, true)) {
					}
				}
			// }
		}
		else {
			ascendingDiaporama = true;
			if ((nextFile(&IndPlayedDiaporamaFile, &fileName, ascendingDiaporama))) {
				// printf("load diaporama initial files name %s\n", fileName->c_str());
				if (pg_load_photo_layer(&fileName, indPhoto, true)) {
				}
			}
		}
	}

	// printOglError(469);

	// launches blending of the first image
	pg_Photo_buffer_data[0].blendStart = CurrentClockTime;
	for (int indPhoto = 1; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		pg_Photo_buffer_data[1].blendStart = -1.0;
	}
	printOglError(465);
}

bool pg_update_diaporama(void) {
		
	bool valRet = false;

	// avoids blocking the diaporama with two photos to be started
	// and not being triggered (because the ongoing blends have been
	// achieved while loading the new photos
	if (pg_Photo_buffer_data[0].blendStart < 0.0f
		&& pg_Photo_buffer_data[1].blendStart < 0.0f
		&& pg_Photo_buffer_data[0].transferred == true
		&& pg_Photo_buffer_data[1].transferred == true) {
		pg_Photo_buffer_data[0].blendStart = CurrentClockTime;
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
		// and it is not transferred
		// it will be started when the preceding one has reached
		// blendStart + photo_diaporama_fade + photo_diaporama_plateau + intervalDuration
		if (pg_Photo_buffer_data[indPhoto].blendStart < 0.0f) {
			// not blending
			//printf("Photo %d blend start %.3f\n",
			//	indPhoto, pg_Photo_buffer_data[indPhoto].blendStart);
			*photoWeight = 0.0f;
			continue;
		}

		// ongoing blending
		float playingTime = float(CurrentClockTime - pg_Photo_buffer_data[indPhoto].blendStart);
		*photoWeight = 0.0f;
		// incay and decay are 0 if is_capture_diaporama
		float incay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float decay = (is_capture_diaporama ? 0 : photo_diaporama_fade);
		float interval = (is_capture_diaporama ? 0 : pg_Photo_buffer_data[indPhoto].intervalDuration);
		//printf("Photo %d transferred %d playing time %.3f\n",
		//	indPhoto, int(pg_Photo_buffer_data[indPhoto].transferred), playingTime);
		if (pg_Photo_buffer_data[indPhoto].transferred == true
			&& playingTime < (incay	+ photo_diaporama_plateau + decay)) {
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
			&& pg_Photo_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].transferred == true
			&& pg_Photo_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart < 0) {
			pg_Photo_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart =
				pg_Photo_buffer_data[indPhoto].blendStart 
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
			pg_Photo_buffer_data[indPhoto].blendStart = -1.0f;
			*photoWeight = 0.0f;
			// releases the image
			//  std::cout << "load next file for layer " << indPhoto << std::endl;
			//imgPhotoBuffer[indPhoto].release();
			// loads the next image to be prepared for next blend
			string fileName = "";
			if ((nextFile(&IndPlayedDiaporamaFile,&fileName, ascendingDiaporama))) {
				if (pg_load_photo_layer(&fileName, indPhoto, true)) {
				}
			}

			valRet = true;
			// that displays the full blendped image

			// releases the blend image
			// imgPhotoBuffer[indPhoto].release();
		}
	}

	return valRet;
}



