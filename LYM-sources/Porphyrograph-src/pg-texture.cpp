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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// TEXTURES
// number of Texture files
vector<pg_TextureData> pg_Textures[PG_MAX_SCENARIOS];

//////////////////////////////////////////////////////////////////////
// PARTICLES TEXTURES
// CURVE PARTICLES TEXTURE
GLuint pg_curve_particle_2D_texID[PG_MAX_SCENARIOS] = { NULL_ID };
// blurred disk texture
std::vector<GLuint>  pg_blurredDisk_texture_2D_texID[PG_MAX_SCENARIOS];

////////////////////////////////////////////////////////////////////
// BLENDED MULTILAYER IMAGES FOR TVW
PhotoSwapData pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES] = { PhotoSwapData() };

//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image blend frequency and duration
// and according to directory size
int pg_CurrentDiaporamaFile = 0;

////////////////////////////////////////////////////////////////////
// OPENCV MATRICES TO STORE IMAGES
cv::Mat pngImgMatRGBInitial;
cv::Mat pngImgMatRGBFlipped;
cv::Mat pngImgMatBGR;
cv::Mat jpgImgMatRGBInitial;
cv::Mat jpgImgMatRGBFlipped;
cv::Mat jpgImgMatBGR;
cv::Mat tiffLayerMatRGBInit;
cv::Mat tiffLayerMatBGR;
cv::Mat tiffLayerMatBGRFlipped;
cv::Mat image3DLayerBGRInit;
cv::Mat image3DLayerRGB;
cv::Mat image3DLayerRGBFlipped;
cv::Mat textureImgMatBGRInit;
cv::Mat textureImgMatRGB;
cv::Mat textureImgMatRGBFlipped;
cv::Mat particleImgMatBGRInit;
cv::Mat particleImgMatRGB;
cv::Mat particleImgMatRGBFlipped;
cv::Mat particleAccImgMatBGRInit;
cv::Mat particleAccImgMatRGB;
cv::Mat particleAccImgMatRGBFlipped;

unsigned int pg_imgPhotoCompressedFormat[PG_MAX_SCENARIOS];
unsigned char* pg_imgPhotoCompressedBitmap[PG_MAX_SCENARIOS] = { NULL };
cv::Mat pg_imgPhotoBGRInit[PG_MAX_SCENARIOS];
cv::Mat pg_imgPhotoRGB[PG_MAX_SCENARIOS];


////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
ClipFramesData** pg_ClipFrames_buffer_data[PG_MAX_SCENARIOS] = { NULL };

////////////////////////////////////////////////////////////////////
// CLIP TEXTURES
int pg_nbClips[PG_MAX_SCENARIOS] = { 0 };
int pg_nbCompressedClipFrames[PG_MAX_SCENARIOS] = { 0 };
int* pg_nbCompressedClipFramesPerFolder[PG_MAX_SCENARIOS] = { NULL };
int* pg_firstCompressedClipFramesInFolder[PG_MAX_SCENARIOS] = { NULL };
//int pg_CurrentClipFramesFile = 0;
//int pg_CurrentClipFramesDir = -1;

/////////////////////////////////////////////////////////////////
// textures bitmaps and associated IDs
std::vector<GLuint> pg_particle_initial_pos_speed_texID[PG_MAX_SCENARIOS];
std::vector<GLuint> pg_particle_initial_color_radius_texID[PG_MAX_SCENARIOS];
std::vector<GLuint> pg_particle_acc_texID[PG_MAX_SCENARIOS];
std::vector<GLuint> pg_pixel_acc_texID[PG_MAX_SCENARIOS];

GLuint pg_CATable_ID = NULL_ID;
GLubyte* pg_CATable = NULL;


GLuint pg_Pen_texture_3D_texID[PG_MAX_SCENARIOS] = { NULL_ID };
GLuint pg_Noise_texture_3D[PG_MAX_SCENARIOS] = { NULL_ID };
std::vector<GLuint>  pg_RepopDensity_texture_texID[PG_MAX_SCENARIOS];

GLuint pg_Master_Mask_texID[PG_MAX_SCENARIOS] = { NULL_ID };
GLuint pg_Master_Multilayer_Mask_texID[PG_MAX_SCENARIOS] = { NULL_ID };

///////////////////////////////////////////////////////////////////////////////////////////////////////
// UTILS: DIRECTORY CLEANING
//////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNCTION FOR CLEARING A DIRECTORY
void pg_remove_files_in_dir(std::string *dirpath) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "pg_remove_files_in_dir Error(" << errno << ") opening " << *dirpath << std::endl;
		return;
	}
	// std::cout << std::endl << "dir to get files of: " + *dirpath << std::endl;
	// scans all the subdirectories until finding one that has the current 
	// index + 1 as substring, typically 001_XXXX
	struct dirent* subdirp = readdir(dp);
	while (subdirp != NULL) {
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
		subdirp = readdir(dp);
	}
	// ends of the dir without finding the file, the file index will be reset
	closedir(dp);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZES PARTICLES FROM PHOTO
//////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_generateParticleInitialPosColorRadiusfromImage(string fileName, int indScenario) { // 2 texture IDs one for pos/speed, the other one for color/radius
	bool invert = false;

	pg_printOglError(83);

	std::cout << fileName + " (2D particle init), ";

#ifndef OPENCV_3
	particleImgMatBGRInit = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	particleImgMatBGRInit = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
	// Check for invalid input
	if (!particleImgMatBGRInit.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // pg_ReportError(pg_errorStr); throw 425;
		return false;
	}
	int nbChannels = particleImgMatBGRInit.channels();
	//printf("File %s nb channels %d\n", fileName.c_str(), nbChannels);
	int colorTransform = (nbChannels == 4) ? COLOR_BGRA2RGBA :
		(nbChannels == 3) ? COLOR_BGR2RGB : COLOR_GRAY2RGB;
	// int glColorType = (nbChannels == 4) ? GL_RGBA : GL_RGB;
	if (nbChannels >= 3) {
		cv::cvtColor(particleImgMatBGRInit, particleImgMatRGB, colorTransform);
	}
	else {
		particleImgMatRGB = particleImgMatBGRInit;
	}
	if (invert)
		cv::flip(particleImgMatRGB, particleImgMatRGBFlipped, 0); // vertical flip
	else
		particleImgMatRGBFlipped = particleImgMatRGB;

	if (PG_NB_PARTICLES > pg_workingWindow_width * PG_WINDOW_HEIGHT) {
		sprintf(pg_errorStr, "Particle size should be lower than %d!\n", pg_workingWindow_width * PG_WINDOW_HEIGHT); pg_ReportError(pg_errorStr); throw 425;
		return false;
	}
	//else {
	//	 printf("File %s nb channels %d for %d particles in window (%dx%d)\n", fileName.c_str(), nbChannels, PG_NB_PARTICLES, pg_workingWindow_width, PG_WINDOW_HEIGHT);
	//}

	float* pos_speed = new float[pg_workingWindow_width * PG_WINDOW_HEIGHT * 4];
	float* color_radius = new float[pg_workingWindow_width * PG_WINDOW_HEIGHT * 4];
	bool* allocated = new bool[pg_workingWindow_width * PG_WINDOW_HEIGHT];
	for (int i = 0; i < pg_workingWindow_width * PG_WINDOW_HEIGHT; i++) {
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
	float image_ratio = float(pg_workingWindow_width) / float(PG_WINDOW_HEIGHT);
	int partic_height = int(floor(sqrt(PG_NB_PARTICLES / image_ratio)));
	int partic_width = int(floor(partic_height * image_ratio));
	//printf("partic hxw %dx%d\n", partic_height, partic_width);
	float ratioImgToPart = float(particleImgMatRGBFlipped.cols) / partic_width;
	float partic_radius = 2.f * ratioImgToPart;
	int intPartic_radius = int(floor(partic_radius));
	//printf("partic radius %d ratioImgToPart %.2f\n", intPartic_radius, ratioImgToPart);
	for (int indRow = 0; indRow < partic_height; indRow++) {
		for (int indCol = 0; indCol < partic_width; indCol++) {
			int indPixelCol = int(round(indCol * ratioImgToPart));
			int indPixelRow = int(round(indRow * ratioImgToPart));
			int nbPixels = 0;
			float r = 0.f, g = 0.f, b = 0.f;
			for (int indLocCol = -intPartic_radius; indLocCol <= intPartic_radius; indLocCol++) {
				for (int indLocRow = -intPartic_radius; indLocRow <= intPartic_radius; indLocRow++) {
					if ((indPixelRow + indLocRow) >= 0 && (indPixelRow + indLocRow) < particleImgMatRGBFlipped.rows
						&& (indPixelCol + indLocCol) >= 0 && (indPixelCol + indLocCol) < particleImgMatRGBFlipped.cols) {
						Point3_<uchar>* p = particleImgMatRGBFlipped.ptr<Point3_<uchar> >(particleImgMatRGBFlipped.rows - (indPixelRow + indLocRow) - 1, (indPixelCol + indLocCol));
						//Point3_<uchar>* p = particleImgMatRGBFlipped.ptr<Point3_<uchar> >(particleImgMatRGBFlipped.rows - indPixelRow - 1, indPixelCol);
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
			int indParticle = int(rand_0_1 * PG_NB_PARTICLES) % PG_NB_PARTICLES;
			while (allocated[indParticle]) {
				indParticle = (indParticle + 1) % PG_NB_PARTICLES;
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
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_pos_speed_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, pg_workingWindow_width, PG_WINDOW_HEIGHT, 0,
		GL_RGBA, GL_FLOAT, pos_speed);
	pg_particle_initial_pos_speed_texID[indScenario].push_back(textureParticleInitialization_pos_speed_ID);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_color_radius_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, pg_workingWindow_width, PG_WINDOW_HEIGHT, 0,
		GL_RGBA, GL_FLOAT, color_radius);
	pg_particle_initial_color_radius_texID[indScenario].push_back(textureParticleInitialization_color_radius_ID);

	//printf("Init image loaded %s Real size %dx%d\nParticle size %d=%dx%d (of %d particles)\nRadius %.2f\n",
	//	fileName.c_str(),
	//	image.cols, image.rows,
	//	partic_width * partic_height, partic_width, partic_height, PG_NB_PARTICLES,
	//	partic_radius);

	delete(color_radius);
	delete(pos_speed);
	delete(allocated);

	pg_printOglError(85);

	return true;
}

//////////////////////////////////////////////
// particle acceleration shift from photography
// stored into pg_particle_acc_texID
bool pg_storeParticleAccelerationfromImage(string fileName, int indScenario) {
	bool invert = false;

	pg_printOglError(84);

#ifndef OPENCV_3
	particleAccImgMatBGRInit = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	particleAccImgMatBGRInit = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
	// Check for invalid input
	if (!particleAccImgMatBGRInit.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // pg_ReportError(pg_errorStr); throw 425;
		return false;
	}
	int nbChannels = particleAccImgMatBGRInit.channels();
	// printf("File %s nb channels %d\n", fileName.c_str(), nbChannels);
	int colorTransform = (nbChannels == 4) ? COLOR_BGRA2RGBA :
		(nbChannels == 3) ? COLOR_BGR2RGB : COLOR_GRAY2RGB;
	// int glColorType = (nbChannels == 4) ? GL_RGBA : GL_RGB;
	if (nbChannels >= 3) {
		cv::cvtColor(particleAccImgMatBGRInit, particleAccImgMatRGB, colorTransform);
	}
	else {
		particleAccImgMatRGB = particleAccImgMatBGRInit;
	}
	if (invert)
		cv::flip(particleAccImgMatRGB, particleAccImgMatRGBFlipped, 0); // vertical flip
	else
		particleAccImgMatRGBFlipped = particleAccImgMatRGB;

	if (PG_NB_PARTICLES > pg_workingWindow_width * PG_WINDOW_HEIGHT) {
		sprintf(pg_errorStr, "Particle size should be lower than %d!\n", pg_workingWindow_width * PG_WINDOW_HEIGHT); pg_ReportError(pg_errorStr); throw 425;
		return false;
	}
	//else {
	//	 printf("File %s nb channels %d for %d particles in window (%dx%d)\n", fileName.c_str(), nbChannels, PG_NB_PARTICLES, pg_workingWindow_width, PG_WINDOW_HEIGHT);
	//}

	float* acceleration_shift = new float[pg_workingWindow_width * PG_WINDOW_HEIGHT * 4];
	for (int i = 0; i < pg_workingWindow_width * PG_WINDOW_HEIGHT; i++) {
		acceleration_shift[i * 4 + 0] = 1.f;
		acceleration_shift[i * 4 + 1] = 1.f;
		acceleration_shift[i * 4 + 2] = 1.f;
		acceleration_shift[i * 4 + 3] = 1.f;
	}

	//printf("image %d %d %d %d\n", image.rows, image.cols, PG_WINDOW_HEIGHT, pg_workingWindow_width);
	if (particleAccImgMatRGBFlipped.rows < PG_WINDOW_HEIGHT) {
		sprintf(pg_errorStr, "Image for particle acceleration height (%d) should be higher or equal than window height %d!\n", particleAccImgMatRGBFlipped.rows, PG_WINDOW_HEIGHT); pg_ReportError(pg_errorStr); throw 425;
	}
	if (particleAccImgMatRGBFlipped.cols < pg_workingWindow_width) {
		sprintf(pg_errorStr, "Image for particle acceleration widht (%d) should be higher or equal than window width %d!\n", particleAccImgMatRGBFlipped.cols, pg_workingWindow_width); pg_ReportError(pg_errorStr); throw 425;
	}

	//printf("window dim %dx%d\n", pg_workingWindow_width, PG_WINDOW_HEIGHT);
	for (int indRow = 0; indRow < PG_WINDOW_HEIGHT; indRow++) {
		for (int indCol = 0; indCol < pg_workingWindow_width; indCol++) {
			float r = 0.f, g = 0.f, b = 0.f;
			Point3_<uchar>* p = particleAccImgMatRGBFlipped.ptr<Point3_<uchar> >(particleAccImgMatRGBFlipped.rows - indRow - 1, indCol);
			int indPixel = indRow * pg_workingWindow_width + indCol;
			r = p->x / 255.f;
			g = p->y / 255.f;
			b = p->z / 255.f;

			float value = max(max(r, g), b);
			float angle = float(asin(value - 0.5));
			// printf("ind %d value %.2f angle %.2f\n", indPixel, value - 0.5, angle);
			acceleration_shift[indPixel * 4 + 0] = angle;
			acceleration_shift[indPixel * 4 + 1] = 0.f;
			acceleration_shift[indPixel * 4 + 2] = 0.f;
			acceleration_shift[indPixel * 4 + 3] = 1.f;
		}
	}

	GLuint textureParticleInitialization_acc_ID;
	glGenTextures(1, &textureParticleInitialization_acc_ID);
	glEnable(GL_TEXTURE_RECTANGLE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_acc_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, pg_workingWindow_width, PG_WINDOW_HEIGHT, 0,
		GL_RGBA, GL_FLOAT, acceleration_shift);
	pg_particle_acc_texID[indScenario].push_back(textureParticleInitialization_acc_ID);

	//printf("Init image loaded %s Real size %dx%d\nParticle size %d=%dx%d (of %d particles)\nRadius %.2f\n",
	//	fileName.c_str(),
	//	image.cols, image.rows,
	//	partic_width * partic_height, partic_width, partic_height, PG_NB_PARTICLES,
	//	partic_radius);
	//printf("Loading particles acceleration shift %s\n", fileName.c_str());

	free(acceleration_shift);

	pg_printOglError(86);

	return true;
}

void pg_initMovieFrameTexture(Mat* movie_frame) {
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
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// 2D/3D TEXTURE LOADING
//////////////////////////////////////////////////////////////////////////////////////////////////////
// 3D texture loading + transfer to GPU
bool pg_loadTexture3D(pg_TextureData* texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter) {
	// data type is assumed to be GL_UNSIGNED_BYTE

	long size = texData->texture_Size_X * texData->texture_Size_Y * texData->texture_Nb_Bytes_per_Pixel;
	GLubyte* bitmap = new unsigned char[size * texData->texture_Nb_Layers];
	bool is_image_tiff = false;
	string fileName;

	fileName = texData->texture_fileName + texData->texture_fileNameSuffix;

	if (texData->texture_fileNameSuffix.compare(0, 4, ".png") == 0) {
		is_image_tiff = false;
	}
	else if (texData->texture_fileNameSuffix.compare(0, 4, ".tif") == 0) {
		is_image_tiff = true;
	}
	else {
		sprintf(pg_errorStr, "Unexpected 3D image extension %s!\n", texData->texture_fileNameSuffix.c_str()); pg_ReportError(pg_errorStr); throw 425;
		return false;
	}

	glEnable(GL_TEXTURE_3D);
	if (!(texData->texture_texID)) {
		glGenTextures(1, &(texData->texture_texID));
	}

	if (is_image_tiff) {
		vector<Mat> pages;
#ifndef OPENCV_3
		imreadmulti(fileName, pages, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
		imreadmulti(fileName, pages, cv::IMREAD_UNCHANGED);   // Read the file
#endif
		if (pages.size() != texData->texture_Nb_Layers) {
			sprintf(pg_errorStr, "The number of layers does not match the value in the scenario file %s %lu vs %d!\n", fileName.c_str(), pages.size(), texData->texture_Nb_Layers); pg_ReportError(pg_errorStr); throw 425;
			return false;
		}
		std::cout << fileName + cv::format(" (3D tiff %d ch %lu layers), ", pages.at(0).channels(), pages.size());
		long ind = 0;
		for (unsigned int indTex = 0; indTex < texData->texture_Nb_Layers; indTex++) {
			// texture load through OpenCV
			tiffLayerMatRGBInit = pages.at(indTex);
#ifndef OPENCV_3
			// printf("nb channels %d\n", imgOpenCV.channels());
			int colorTransform = (tiffLayerMat.channels() == 4) ? CV_BGRA2RGBA :
				(tiffLayerMat.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
			// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
			cv::cvtColor(tiffLayerMatRGBInit, tiffLayerMatBGR, colorTransform);
#else
			int colorTransform = (tiffLayerMatRGBInit.channels() == 4) ? cv::COLOR_BGRA2RGBA :
				(tiffLayerMatRGBInit.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
			// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
			cv::cvtColor(tiffLayerMatRGBInit, tiffLayerMatBGR, colorTransform);
#endif
			if (texData->texture_Invert)
				cv::flip(tiffLayerMatBGR, tiffLayerMatBGRFlipped, 0); // vertical flip
			else
				tiffLayerMatBGRFlipped = tiffLayerMatBGR;

			if (!tiffLayerMatBGRFlipped.data) {                              // Check for invalid input
				sprintf(pg_errorStr, "Could not open or find the image %s!\n", fileName.c_str()); pg_ReportError(pg_errorStr); throw 425;
				return false;
			}
			if (tiffLayerMatBGRFlipped.cols != texData->texture_Size_X
				|| tiffLayerMatBGRFlipped.rows != texData->texture_Size_Y) {   // Check for invalid input
				sprintf(pg_errorStr, "Unexpected multilayer tiff image size %s %d/%d %d/%d!\n", fileName.c_str(),
					tiffLayerMatBGRFlipped.cols, texData->texture_Size_X, tiffLayerMatBGRFlipped.rows, texData->texture_Size_Y); pg_ReportError(pg_errorStr); throw 425;
				return false;
			}
			if (tiffLayerMatBGRFlipped.elemSize() != texData->texture_Nb_Bytes_per_Pixel) {   // Check for invalid input
				sprintf(pg_errorStr, "Unexpected multilayer tiff image bytes per pixel %s %lu/%d!\n", fileName.c_str(), tiffLayerMatBGRFlipped.elemSize(), texData->texture_Nb_Bytes_per_Pixel); pg_ReportError(pg_errorStr); throw 425;
				return false;
			}
			//printf("texture loading layer %d bytes per pixel %ld\n", indTex, result.elemSize());
			GLubyte* ptr = tiffLayerMatBGRFlipped.data;
			for (long int i = 0; i < size; i++)
				bitmap[ind++] = ptr[i];
		}
	}
	else {
		long ind = 0;
		for (unsigned int indTex = 0; indTex < texData->texture_Nb_Layers; indTex++) {
			cv::String layerFilename = cv::format("%s_%03d%s", texData->texture_fileName.c_str(),
				(indTex + 1), texData->texture_fileNameSuffix.c_str());
			std::cout << layerFilename + " (3D-layer), ";

			// texture load through OpenCV
#ifndef OPENCV_3
			image3DLayerBGRInit = cv::imread(layerFilename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
			// printf("nb channels %d\n", imgOpenCV.channels());
			int colorTransform = (image3DLayerBGRInit.channels() == 4) ? CV_BGRA2RGBA :
				(image3DLayerBGRInit.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
			// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
			cv::cvtColor(image3DLayerBGRInit, image3DLayerRGB, colorTransform);
#else
			image3DLayerBGRInit = cv::imread(layerFilename, cv::IMREAD_UNCHANGED);   // Read the file
			int colorTransform = (image3DLayerBGRInit.channels() == 4) ? cv::COLOR_BGRA2RGBA :
				(image3DLayerBGRInit.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
			// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
			cv::cvtColor(image3DLayerBGRInit, image3DLayerRGB, colorTransform);
#endif
			if (texData->texture_Invert)
				cv::flip(image3DLayerRGB, image3DLayerRGBFlipped, 0); // vertical flip
			else
				image3DLayerRGBFlipped = image3DLayerRGB;

			if (!image3DLayerRGBFlipped.data) {                              // Check for invalid input
				sprintf(pg_errorStr, "Could not open or find the image %s!\n", fileName.c_str()); pg_ReportError(pg_errorStr); throw 425;
				return false;
			}
			if (image3DLayerRGBFlipped.cols != texData->texture_Size_X
				|| image3DLayerRGBFlipped.rows != texData->texture_Size_Y) {   // Check for invalid input
				sprintf(pg_errorStr, "Unexpected 3D image size %s %d/%d %d/%d!\n", fileName.c_str(),
					image3DLayerRGBFlipped.cols, texData->texture_Size_X, image3DLayerRGBFlipped.rows, texData->texture_Size_Y); pg_ReportError(pg_errorStr); throw 425;
				return false;
			}

			GLubyte* ptr = image3DLayerRGBFlipped.data;
			for (long int i = 0; i < size; i++)
				bitmap[ind++] = ptr[i];
		}
	}

	// printf("Master index %ld / %ld\n" , ind , size * nbTextures );
	// glActiveTexture (GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_3D, texData->texture_texID);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D,     // Type of texture
		0,                  // Pyramid level (for mip-mapping) - 0 is the top level
		components,         // Components: Internal colour format to convert to
		texData->texture_Size_X,              // Image width
		texData->texture_Size_Y,             // Image heigh
		texData->texture_Nb_Layers,         // Image depth
		0,                  // Border width in pixels (can either be 1 or 0)
		format,             // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		datatype,           // Image data type
		bitmap);            // The actual image data itself

	pg_printOglError(74);

	// memory release
	delete[] bitmap;

	glEnable(GL_TEXTURE_RECTANGLE);

	// glGenerateMipmap(GL_TEXTURE_2D);
	// printf("3D textures loaded\n" );
	return true;
}

// 2D texture loading + transfer to GPU

bool pg_loadTexture2D(pg_TextureData* texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter) {
	string fileName = texData->texture_fileName + texData->texture_fileNameSuffix;

	std::cout << fileName + " (2D), ";

	glEnable(GL_TEXTURE_2D);
	if (!(texData->texture_texID)) {
		glGenTextures(1, &(texData->texture_texID));
	}

	// texture load through OpenCV
#ifndef OPENCV_3
	textureImgMatBGRInit = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	// printf("File %s nb channels %d\n", fileName.c_str(), imgOpenCV.channels());
	int colorTransform = (textureImgMatBGRInit.channels() == 4) ? CV_BGRA2RGBA :
		(textureImgMatBGRInit.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
	if (imgOpenCV.channels() >= 3) {
		cv::cvtColor(textureImgMatBGRInit, textureImgMatRGB, colorTransform);
	}
	else {
		textureImgMatRGB = textureImgMatBGRInit;
	}
#else
	textureImgMatBGRInit = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
	//printf("texture %s nb channels %d\n", fileName.c_str(), textureImgMatBGRInit.channels());
	if (format == GL_RGBA && textureImgMatBGRInit.channels() != 4) {
		sprintf(pg_errorStr, "RGBA image expected for texture %s not %d-channel!\n", fileName.c_str(), textureImgMatBGRInit.channels()); pg_ReportError(pg_errorStr); throw 425;
	}
	else if (format == GL_LUMINANCE && textureImgMatBGRInit.channels() != 1) {
		sprintf(pg_errorStr, "Gray level image expected for texture %s not %d-channel!\n", fileName.c_str(), textureImgMatBGRInit.channels()); pg_ReportError(pg_errorStr); throw 425;
	}
	int colorTransform = (textureImgMatBGRInit.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(textureImgMatBGRInit.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (imgOpenCV.channels() == 4) ? GL_RGBA : GL_RGB;
	if (textureImgMatBGRInit.channels() >= 3) {
		cv::cvtColor(textureImgMatBGRInit, textureImgMatRGB, colorTransform);
	}
	else {
		textureImgMatRGB = textureImgMatBGRInit;
	}
#endif

	if (texData->texture_Invert)
		cv::flip(textureImgMatRGB, textureImgMatRGBFlipped, 0); // vertical flip
	else
		textureImgMatRGBFlipped = textureImgMatBGRInit;

	if (!textureImgMatRGBFlipped.data) {                              // Check for invalid input
		sprintf(pg_errorStr, "Could not open or find the image %s!\n", fileName.c_str()); pg_ReportError(pg_errorStr); throw 425;
		return false;
	}
	if (textureImgMatRGBFlipped.cols != texData->texture_Size_X || textureImgMatRGBFlipped.rows != texData->texture_Size_Y) {                              // Check for invalid input
		sprintf(pg_errorStr, "Unexpected image size %s %d/%d %d/%d!\n", fileName.c_str(), textureImgMatRGBFlipped.cols,
			texData->texture_Size_X, textureImgMatRGBFlipped.rows, texData->texture_Size_Y); pg_ReportError(pg_errorStr); throw 425;
		return false;
	}

	// glActiveTexture (GL_TEXTURE0 + index);
	if (texData->texture_Is_Rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, texData->texture_texID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			textureImgMatRGBFlipped.cols,          // Image width
			textureImgMatRGBFlipped.rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			textureImgMatRGBFlipped.ptr());        // The actual image data itself
		pg_printOglError(76);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texData->texture_texID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			textureImgMatRGBFlipped.cols,          // Image width
			textureImgMatRGBFlipped.rows,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			textureImgMatRGBFlipped.ptr());        // The actual image data itself
		pg_printOglError(78);
		glEnable(GL_TEXTURE_RECTANGLE);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// ALL SCENARIO TEXTURE LOADING
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_loadAllTextures(void) {
	std::cout << "Loading textures: " << std::endl;

	//////////////////////////////////////////////////////////////
	// loaded textures
	// loads all the textures from the configuration file
	// unless the image itself is used to generate other textures
	for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
		std::cout << "    " << indScenario << ": ";
		for (pg_TextureData &texture : pg_Textures[indScenario]) {
			if (texture.texture_usage != pg_enum_Texture_part_init
				&& texture.texture_usage != pg_enum_Texture_part_acc
				&& texture.texture_usage != pg_enum_Texture_pixel_acc) {
				if (texture.texture_Dimension == 2) {
					pg_loadTexture2D(&texture,
						GL_RGBA8, GL_RGBA,
						GL_UNSIGNED_BYTE, GL_LINEAR);
					pg_printOglError(87);
				}
				else if (texture.texture_Dimension == 3) {
					pg_loadTexture3D(&texture,
						GL_RGBA8, GL_RGBA,
						GL_UNSIGNED_BYTE, GL_LINEAR);
					pg_printOglError(88);
				}
			}
			else if (texture.texture_usage == pg_enum_Texture_part_init
				&& texture.texture_Dimension == 2) {
				std::cout << texture.texture_fileName + texture.texture_fileNameSuffix + " (particle init), ";
				if (pg_generateParticleInitialPosColorRadiusfromImage(texture.texture_fileName + texture.texture_fileNameSuffix, 
					indScenario)) {
					if (texture.texture_Rank != pg_particle_initial_pos_speed_texID[indScenario].size()
						|| texture.texture_Rank != pg_particle_initial_color_radius_texID[indScenario].size()) {
						sprintf(pg_errorStr, "Error: particle initialization texture #%lu/%lu rank incorrect (%d rank expected)!\n", pg_particle_initial_pos_speed_texID[indScenario].size(), pg_particle_initial_color_radius_texID[indScenario].size(), texture.texture_Rank); pg_ReportError(pg_errorStr); throw 336;
					}
				}
			}
			else if (texture.texture_usage == pg_enum_Texture_part_acc
				&& texture.texture_Dimension == 2) {
				std::cout << texture.texture_fileName + texture.texture_fileNameSuffix + " (particle acc), ";
				GLuint textureParticle_acc_ID = NULL_ID;
				pg_loadTexture2D(&texture,
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR);
				pg_particle_acc_texID[indScenario].push_back(textureParticle_acc_ID);
			}
			else if (pg_FullScenarioActiveVars[indScenario][_pixel_image_acceleration]
				&& texture.texture_usage == pg_enum_Texture_pixel_acc
				&& texture.texture_Dimension == 2) {
				std::cout << texture.texture_fileName + texture.texture_fileNameSuffix + " (pixel acc), ";
				GLuint texturePixel_acc_ID = -1;
				pg_loadTexture2D(&texture,
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR);
				pg_pixel_acc_texID[indScenario].push_back(texturePixel_acc_ID);
			}
		}

		//for (pg_TextureData& texture : pg_Textures[0]) {
		//	printf("texture usage %d dim %d rank %d ID %d name %s\n", texture.texture_usage, texture.texture_Dimension,
		//		texture.texture_Rank, texture.texture_texID, (texture.texture_fileName + texture.texture_fileNameSuffix).c_str());
		//}

		// assigns the textures to their destination according to their usage field
		for (pg_TextureData &texture : pg_Textures[indScenario]) {
			if (pg_FullScenarioActiveVars[indScenario][_sensor_layout]) {
				if (texture.texture_usage == pg_enum_Texture_sensor
					&& texture.texture_Dimension == 2) {
					Sensor_texture_rectangle[indScenario] = texture.texture_texID;
				}
			}

			if (texture.texture_usage == pg_enum_Texture_master_mask
				&& texture.texture_Dimension == 2) {
				pg_Master_Mask_texID[indScenario] = texture.texture_texID;
			}
			else if (texture.texture_usage == pg_enum_Texture_multilayer_master_mask
				&& texture.texture_Dimension == 3) {
				pg_Master_Multilayer_Mask_texID[indScenario] = texture.texture_texID;
			}

			if (texture.texture_usage == pg_enum_Texture_brush
				&& texture.texture_Dimension == 3) {
				pg_Pen_texture_3D_texID[indScenario] = texture.texture_texID;
			}

			if (texture.texture_usage == pg_enum_Texture_noise
				&& texture.texture_Dimension == 3) {
				pg_Noise_texture_3D[indScenario] = texture.texture_texID;
			}

			if (texture.texture_usage == pg_enum_Texture_repop_density
				&& texture.texture_Dimension == 2) {
				pg_RepopDensity_texture_texID[indScenario].push_back(texture.texture_texID);
				std::cout << texture.texture_fileName + texture.texture_fileNameSuffix + " (repop density), ";
				if (texture.texture_Rank != pg_RepopDensity_texture_texID[indScenario].size()) {
					sprintf(pg_errorStr, "Error: repopulation texture density #%lu rank incorrect (%d rank expected)!\n", pg_RepopDensity_texture_texID[indScenario].size(), texture.texture_Rank); pg_ReportError(pg_errorStr); throw 336;
				}
			}
			// printf("Loading particles initial images %s\n", fileName.c_str()); 
			if (!pg_particle_initial_pos_speed_texID[indScenario].size()
				|| !pg_particle_initial_color_radius_texID[indScenario].size()) {
				sprintf(pg_errorStr, "Error: particle initialization texture not provided for configuration %d scenario %s, check texture list with part_init usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}

			if (texture.texture_usage == pg_enum_Texture_mesh
				&& texture.texture_Dimension == 2) {
				// assigns this textures to the meshes which have the same texture rank
				for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[indScenario].size(); indMeshFile++) {
					if (pg_Meshes[indScenario][indMeshFile].pg_Mesh_TextureRank == texture.texture_Rank) {
						pg_Meshes[indScenario][indMeshFile].Mesh_texture_rectangle = texture.texture_texID;
					}
				}
			}
			if (texture.texture_usage == pg_enum_Texture_particle
				&& texture.texture_Dimension == 2) {
				pg_curve_particle_2D_texID[indScenario] = texture.texture_texID;
			}
			if (texture.texture_usage == pg_enum_Texture_splat_particle 
				&& texture.texture_Dimension == 2) {
				pg_blurredDisk_texture_2D_texID[indScenario].push_back(texture.texture_texID);
				//printf("splat texture %d\n", texture.texture_texID);
				std::cout << texture.texture_fileName + texture.texture_fileNameSuffix + " (splat), ";
				if (texture.texture_Rank != pg_blurredDisk_texture_2D_texID[indScenario].size()) {
					sprintf(pg_errorStr, "Error: texture splat image #%lu rank incorrect (%d rank expected)!\n", pg_blurredDisk_texture_2D_texID[indScenario].size(), texture.texture_Rank); pg_ReportError(pg_errorStr); throw 336;
				}
			}
		}
		// and checks that the textures were provided
		if (pg_FullScenarioActiveVars[indScenario][_sensor_layout]) {
			if (Sensor_texture_rectangle[indScenario] == NULL_ID) {
				sprintf(pg_errorStr, "Error: sensor texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}
		}
		if (pg_Master_Mask_texID[indScenario] == NULL_ID || pg_Master_Multilayer_Mask_texID[indScenario] == NULL_ID) {
			sprintf(pg_errorStr, "Error: master mask texture (usage: master_mask) and multilayer master mask texture (usage: multilayer_master_mask) not provided for configuration %d scenario %s, check texture list with master mask usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
		}

		if (pg_Pen_texture_3D_texID[indScenario] == NULL_ID) {
			sprintf(pg_errorStr, "Error: brush texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
		}

		if (pg_Noise_texture_3D[indScenario] == NULL_ID) {
			sprintf(pg_errorStr, "Error: noise texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
		}
		if (pg_FullScenarioActiveVars[indScenario][_Part_repop_density]) {
			if (pg_RepopDensity_texture_texID[indScenario].size() == 0) {
				sprintf(pg_errorStr, "Error: particle repopulation texture density not provided for configuration %d scenario %s, check texture list with BG_CA_repop_density usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}
		}
		if (pg_FullScenarioActiveVars[indScenario][_BG_CA_repop_density]) {
			if (pg_RepopDensity_texture_texID[indScenario].size() == 0) {
				sprintf(pg_errorStr, "Error: BG/CA repopulation texture density not provided for configuration %d scenario %s, check texture list with BG_CA_repop_density usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}
		}
		// assigns this textures to the meshes which have the same texture rank
		for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[indScenario].size(); indMeshFile++) {
			if (pg_Meshes[indScenario][indMeshFile].pg_Mesh_TextureRank != -1 // frame buffer texture
				&& pg_Meshes[indScenario][indMeshFile].Mesh_texture_rectangle == NULL_ID) {
				sprintf(pg_errorStr, "Error: mesh texture not provided  for configuration %d scenario %s (mesh #%d and texture rank %d, check texture list with mesh usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str(), indMeshFile, pg_Meshes[indScenario][indMeshFile].pg_Mesh_TextureRank); pg_ReportError(pg_errorStr); throw 336;
			}
		}
		if (particle_geometry == 2) {
			if (pg_curve_particle_2D_texID[indScenario] == NULL_ID) {
				sprintf(pg_errorStr, "Error: curve particles texture not provided for configuration %d scenario %s, check texture list with curve_particle usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}
		}
		else if (particle_geometry == 0) {
			if (pg_blurredDisk_texture_2D_texID[indScenario].size() == 0) {
				sprintf(pg_errorStr, "Error: splat particles texture not provided for configuration %d scenario %s, check texture list with splat_particle usage!\n", indScenario, pg_ScenarioFileNames[indScenario].c_str()); pg_ReportError(pg_errorStr); throw 336;
			}
		}

		pg_loadScreenFontTexture();
		pg_printOglError(82);

		pg_loadScreenMessageTexture();

	// CA Tables
		if (pg_FullScenarioActiveVars[indScenario][_CATable]) {
#define width_data_table 600
#define height_data_table 200
			pg_CATable =
				(GLubyte*)pg_generateTexture(&pg_CATable_ID, pg_enum_byte_tex_format,
					width_data_table, height_data_table);
			pg_CATable_values(pg_CATable_ID, pg_CATable,
				width_data_table, height_data_table);
			if (!pg_CATable_ID) {
				sprintf(pg_errorStr, "Error: data tables for the CA bitmap not allocated (%dx%d)!\n", width_data_table, height_data_table); pg_ReportError(pg_errorStr); throw 336;
			}

			//printf( "\n" );
			//for( int i = 0 ; i <= 243 * 4 ; i+=4 ) {
			//  printf("%d " , (int)pg_CATable[ 4 * 51 * width_data_table + i ] );
			//}
			//printf( "\n" );
		}
		std::cout << std::endl;

		pg_printOglError(79);
	}
	std::cout << std::endl;
	return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE GENERATION FROM DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////

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
	if (texture_format == pg_enum_byte_tex_format) {
		GLubyte *ptr = new GLubyte[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			strcpy(pg_errorStr, "Texture allocation error!\n"); pg_ReportError(pg_errorStr); throw 425;
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
	else if (texture_format == pg_enum_float_tex_format) {
		float *ptr = new float[sizeX * sizeY * 4];
		// If we can't load the file, quit!
		if (ptr == NULL) {
			strcpy(pg_errorStr, "Texture allocation error!\n"); pg_ReportError(pg_errorStr); throw 425;
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


///////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TRANSITION TABLES FOR THE CELLULAR AUTOMATA
//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(var_CATable) && !defined(pg_Project_CAaudio)
#include "pg_CATable_GN.cpp"
#endif

#if defined(var_CATable) && defined(pg_Project_CAaudio)
#include "pg_CATable_CAaudio.cpp"
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////
// JPG/PNG SNAPSHOTS
//////////////////////////////////////////////////////////////////////////////////////////////////////


void pg_writepng(cv::String imageFileName) {
	cv::flip(pngImgMatRGBInitial, pngImgMatRGBFlipped, 0); // vertical flip
#ifndef OPENCV_3
	cv::cvtColor(pngImgMatRGBFlipped, pngImgMatBGR, CV_RGB2BGR);

	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
#else
	cv::cvtColor(pngImgMatRGBFlipped, pngImgMatBGR, cv::COLOR_RGB2BGR);

	std::vector<int> params;
	params.push_back(cv::IMWRITE_PNG_COMPRESSION);
#endif
	params.push_back(9);
	params.push_back(0);
	cv::imwrite(imageFileName, pngImgMatBGR);
}


void pg_writejpg(cv::String imageFileName) {
	cv::flip(jpgImgMatRGBInitial, jpgImgMatRGBFlipped, 0); // vertical flip
#ifndef OPENCV_3
	cv::cvtColor(jpgImgMatRGBFlipped, jpgImgMatBGR, CV_RGB2BGR);

	std::vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
#else
	cv::cvtColor(jpgImgMatRGBFlipped, jpgImgMatBGR, cv::COLOR_RGB2BGR);

	std::vector<int> params;
	params.push_back(cv::IMWRITE_JPEG_QUALITY);
#endif
	params.push_back(70);   // that's percent, so 100 == no compression, 1 == full 

	cv::imwrite(imageFileName, jpgImgMatBGR);
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
// PHOTO BITMAP LOADING AS TEXTURE
//////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// 2D image loading
bool pg_load_compressed_photo(string *fileName, int width, int height, int indScenario) {
	FILE* fp;
	unsigned char header[124];

	/* try to open the file */
	//printf("load image %s\n", fileName);
	fp = fopen(fileName->c_str(), "rb");

	// Check for invalid input
	if (fp == NULL) {
		sprintf(pg_errorStr, "Could not open or find the 2D image %s!", fileName->c_str()); pg_ReportError(pg_errorStr);
		return false;
	}

	/* verify the type of file */
	char filecode[5];
	fread(filecode, 1, 4, fp);
	filecode[4] = 0;
	if (strncmp(filecode, "DDS ", 4) != 0) {
		sprintf(pg_errorStr, "Incorrect compressed file type %s!", filecode); pg_ReportError(pg_errorStr);
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

	// Check for image size
	if (width_from_header != u_int(width)) {
		sprintf(pg_errorStr, "Unexpected dds diaporama image width %s %d instead of %d!",
			fileName->c_str(), width_from_header, width); pg_ReportError(pg_errorStr);
		//ratio_h = u_int(w) / width_from_header;
		//ratio_v = u_int(h) / height_from_header;
		return false;
	}
	if (height_from_header != u_int(height)) {
		sprintf(pg_errorStr, "Unexpected dds diaporama image height %s %d instead of %d!",
			fileName->c_str(), height_from_header, height); pg_ReportError(pg_errorStr);
		//ratio_h = u_int(w) / width_from_header;
		//ratio_v = u_int(h) / height_from_header;
		return false;
	}

	// Check for image size
	if (mipMapCount > 1) {
		sprintf(pg_errorStr, "Unexpected number of mipmaps %s %d!",
			fileName->c_str(), mipMapCount); pg_ReportError(pg_errorStr);
		return false;
	}

	// allocates memory
	if (pg_imgPhotoCompressedBitmap[indScenario]) {
		free(pg_imgPhotoCompressedBitmap[indScenario]);
		pg_imgPhotoCompressedBitmap[indScenario] = NULL;
	}
	pg_imgPhotoCompressedBitmap[indScenario]
		= (unsigned char*)malloc(linearSize * sizeof(unsigned char));
	fread(pg_imgPhotoCompressedBitmap[indScenario], 1, linearSize, fp);
	//printf("linear size %d\n", linearSize);
	/* close the file pointer */
	fclose(fp);

	switch (fourCC)
	{
	case FOURCC_DXT1:
		pg_imgPhotoCompressedFormat[indScenario] = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		//printf("compressed format DTX1\n");
		break;
	case FOURCC_DXT3:
		pg_imgPhotoCompressedFormat[indScenario] = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		//printf("compressed format DTX3\n");
		break;
	case FOURCC_DXT5:
		pg_imgPhotoCompressedFormat[indScenario] = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		//printf("compressed format DTX5\n");
		break;
	default:
		sprintf(pg_errorStr, "Unexpected compression format %s %d!",
			fileName->c_str(), fourCC); pg_ReportError(pg_errorStr);
		free(pg_imgPhotoCompressedBitmap[indScenario]);
		pg_imgPhotoCompressedBitmap[indScenario] = NULL;
		return false;
	}
	//printf("Loaded photo %s compressed format %d\n", fileName, pg_imgPhotoCompressedFormat[indScenario]);
	return true;
}
bool PhotoData::pg_loadPhoto(bool toBeInverted, int width, int height, bool verbose, int indScenario) {
	int fileNameLength = PhotoName.length();
	string extension;

	w = width;
	h = height;
	invert = toBeInverted;

	if (fileNameLength < 4) {
		printf("Incorrect photo file name [%s]\n", PhotoName.c_str());
		return false;
	}
	extension = PhotoName.substr(PhotoName.find_last_of("."));

	if (verbose) {
		printf("Loading [%s] extension %s \n",  PhotoName.c_str(), extension.c_str());
	}

	// DDS FILE
	if (extension == ".dxt1" || extension == ".DXT1" || extension == ".DDS" || extension == ".dds") {
		if (!pg_load_compressed_photo(&PhotoName, w, h, indScenario)) {
			PhotoName = "";
		}
		photoFormat = GL_RGB;
	}
	// JPEG FILE
	else if (extension == ".jpg") {
		// texture load through OpenCV
#ifndef OPENCV_3
		// threaded = false;
		pg_imgPhotoBGRInit[indScenario] = cv::imread(string(PhotoName), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
																			  // Check for invalid input
		if (!pg_imgPhotoBGRInit[indScenario].data) {
			sprintf(pg_errorStr, "Could not open or find the 2D image %s!", PhotoName); pg_ReportError(pg_errorStr);
			loadedInRAM = false;
			*(PhotoName) = 0;
			return false;
		}
		// Check for image size
		if ((w > 0 && pg_imgPhotoBGRInit[indScenario].cols != w)
			|| (h > 0 && pg_imgPhotoBGRInit[indScenario].rows != h)) {
			sprintf(pg_errorStr, "Unexpected jpg diaporama image size %s %d/%d %d/%d!",
				PhotoName, pg_imgPhotoBGRInit[indScenario].cols, w, pg_imgPhotoBGRInit[indScenario].rows,
				h); pg_ReportError(pg_errorStr);
			loadedInRAM = false;
			*(PhotoName) = 0;
			return false;
		}
		w = pg_imgPhotoBGRInit[indScenario].cols;
		h = pg_imgPhotoBGRInit[indScenario].rows;
		photoFormat
			= ((pg_imgPhotoBGRInit[indScenario].channels() == 4) ? GL_RGBA :
			((pg_imgPhotoBGRInit[indScenario].channels() == 3) ? GL_RGB : GL_LUMINANCE));

		int colorTransform = (channelsPhoto == 4) ? CV_BGRA2RGBA :
			(pg_imgPhotoBGRInit[indScenario].channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (pg_imgPhotoBGRInit[indScenario].channels() == 4) ? GL_RGBA : GL_RGB;
		if (pg_imgPhotoBGRInit[indScenario].channels() >= 3) {
			cv::cvtColor(pg_imgPhotoBGRInit[indScenario], pg_imgPhotoRGB, colorTransform);
		}
		else {
			pg_imgPhotoRGB = pg_imgPhotoBGRInit[indScenario];
		}
#else
		pg_imgPhotoBGRInit[indScenario] = cv::imread(PhotoName, cv::IMREAD_UNCHANGED);   // Read the file
																		   // Check for invalid input
		if (!pg_imgPhotoBGRInit[indScenario].data) {
			sprintf(pg_errorStr, "Could not open or find the 2D image %s!", PhotoName.c_str()); pg_ReportError(pg_errorStr);
			PhotoName = "";
			return false;
		}
		// Check for image size
		if ((w > 0 && pg_imgPhotoBGRInit[indScenario].cols != w)
			|| (h > 0 && pg_imgPhotoBGRInit[indScenario].rows != h)) {
			sprintf(pg_errorStr, "Unexpected jpg image size %s %d/%d %d/%d!",
				PhotoName.c_str(), pg_imgPhotoBGRInit[indScenario].cols, w, pg_imgPhotoBGRInit[indScenario].rows,
				h); pg_ReportError(pg_errorStr);
			PhotoName = "";
			return false;
		}
		w = pg_imgPhotoBGRInit[indScenario].cols;
		h = pg_imgPhotoBGRInit[indScenario].rows;
		photoFormat
			= ((pg_imgPhotoBGRInit[indScenario].channels() == 4) ? GL_RGBA :
			((pg_imgPhotoBGRInit[indScenario].channels() == 3) ? GL_RGB : GL_LUMINANCE));

		int colorTransform
			= ((pg_imgPhotoBGRInit[indScenario].channels() == 4) ? cv::COLOR_BGRA2RGBA :
			((pg_imgPhotoBGRInit[indScenario].channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB));

		// int glColorType = (pg_imgPhotoBGRInit[indScenario].channels() == 4) ? GL_RGBA : GL_RGB;
		if (pg_imgPhotoBGRInit[indScenario].channels() >= 3) {
			cv::cvtColor(pg_imgPhotoBGRInit[indScenario], pg_imgPhotoRGB[indScenario], colorTransform);
		}
		else {
			pg_imgPhotoRGB[indScenario] = pg_imgPhotoBGRInit[indScenario];
		}
#endif
	}
	// UNKNOWN
	else {
		printf("Incorrect photo file name extension [%s/%s]\n", PhotoName.c_str(), extension.c_str());
		return false;
	}

	return true;
}

// only loads DDs in HD format width and height are power of two
bool ClipFramesData::pg_loadClipFrames(string *fileName, int width, int height, int indScenario) {
	int fileNameLength = fileName->length();
	string extension = fileName->substr(fileName->find_last_of("."));
	if (extension == ".dxt1" || extension == ".DXT1" || extension == ".DDS" || extension == ".dds") {
		if (pg_load_compressed_photo(fileName, width, height, indScenario)) {
			clipImgFormat = GL_RGB;
			return true;
		}
	}
	*fileName = "";
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE BITMAP TRANSFER TO GPU
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// TRANSFERS BUFFER TO GPU
bool pg_toGPUCompressedPhoto(int width, int height, GLuint textureID, GLenum textureFilter, int indScenario) {
	unsigned int blockSize
		= (pg_imgPhotoCompressedFormat[indScenario] == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)textureFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)textureFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glCompressedTexImage2D(GL_TEXTURE_2D,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		pg_imgPhotoCompressedFormat[indScenario],  // compression format
		width,                 // Image width
		height,                 // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		size,              // Image data type
		pg_imgPhotoCompressedBitmap[indScenario]);        // The actual image data itself

	free(pg_imgPhotoCompressedBitmap[indScenario]);
	pg_imgPhotoCompressedBitmap[indScenario] = NULL;
	pg_printOglError(80);
	//printf("To GPU photo ID %d size %dx%d block size %d compressed format %d filter %d\n", textureID, width, height, blockSize, compressedPhotoFormat[indScenario], textureFilter);
	return false;
}

bool PhotoData::pg_toGPUPhoto(bool is_rectangle, GLint components, GLenum datatype, GLenum texturefilter, int indScenario) {
	/* // components GL_RGB8,
	// datatype GL_UNSIGNED_BYTE
	// format GL_RGB
	printf("format GL_RGB comp GL_RGB8 datatype GL_UNSIGNED_BYTE %d %d %d\n",
	int(components == GL_RGB8),
	int(datatype == GL_UNSIGNED_BYTE),
	int(format == GL_RGB)); */

	bool valret = true;

	pg_printOglError(81);

	//printf("texture rect %d ID indCompressedImage %d compression %d\n", is_rectangle, texBuffID, pg_imgPhotoCompressedFormat[indScenario]);

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle && !pg_imgPhotoCompressedFormat[indScenario]) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, texBuffID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			w,          // Image width
			h,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			photoFormat, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			pg_imgPhotoRGB[indScenario].ptr());        // The actual image data itself
		pg_printOglError(75);
		// printf("filename %s\n", PhotoName);
		valret = false;
	}
	else {
		if (!pg_imgPhotoCompressedFormat[indScenario]) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texBuffID);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexImage2D(GL_TEXTURE_2D,     // Type of texture
				0,                 // Pyramid level (for mip-mapping) - 0 is the top level
				components,            // Components: Internal colour format to convert to
				w,          // Image width
				h,          // Image height
				0,                 // Border width in pixels (can either be 1 or 0)
				photoFormat, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				datatype,  // Image data type
				pg_imgPhotoRGB[indScenario].ptr());        // The actual image data itself
			pg_printOglError(77);
			valret = false;
		}
		else {
			valret = pg_toGPUCompressedPhoto(w, h, texBuffID, texturefilter, indScenario);
		}
	}

	return valret;
}

bool ClipFramesData::pg_toGPUClipFrames(int w, int h, GLenum texturefilter, int indScenario) {
	return pg_toGPUCompressedPhoto(w, h, texBuffID, texturefilter, indScenario);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP FRAME LOADINGx
//////////////////////////////////////////////////////////////////////////////////////////////////////

std::string pg_get_stem(const fs::path& p) { return (p.stem().string()); }
bool  pg_ReadInitalClipFramesTextures(void) {
	bool valret = true;
	printf("Load Clips:\n");
	for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
		std::cout << "    " << indScenario << ": ";
		
		////////////////////////////////////////////
		// LOADS CLIP IMAGES FROM FOLDERS
		std::cout << pg_ClipDirectory[indScenario] << " (";

		///////////////////////////////////////////////
		// NULL INITIALIZATIONS
		pg_nbCompressedClipFramesPerFolder[indScenario] = NULL;
		pg_firstCompressedClipFramesInFolder[indScenario] = NULL;
		pg_ClipFrames_buffer_data[indScenario] = NULL;

		////////////////////////////////////////////
		// COUNTS IMAGES AND FOLDERS
		pg_nbCompressedClipFrames[indScenario] = 0;
		pg_nbClips[indScenario] = 0;

		if (pg_ClipDirectory[indScenario] == "") {
			std::cout << ")" << std::endl << std::endl;
			return false;
		}

		//std::cout << "Counting ClipFrames " << std::endl;
		if (fs::is_directory(pg_ClipDirectory[indScenario])) {
			auto dirIter = fs::directory_iterator(pg_ClipDirectory[indScenario]);
			for (auto& dir_entry : dirIter)
			{
				if (dir_entry.is_directory())
				{
					++pg_nbClips[indScenario];

					int nb_images_per_clip = 0;
					auto subDirIter = fs::directory_iterator(dir_entry);
					for (auto& subdir_entry : subDirIter)
					{
						if (subdir_entry.is_regular_file() && nb_images_per_clip < clip_max_length[indScenario])
						{
							++pg_nbCompressedClipFrames[indScenario];
							++nb_images_per_clip;
						}
					}
				}
			}
		}

		////////////////////////////////////////////
		// LOADS IMAGES FROM FOLDERS
		if (pg_nbClips[indScenario] <= 0) {
			std::cout << ")" << std::endl << std::endl;
			return false;
		}
		//std::cout << "Clip frames counting "<< pg_nbCompressedClipFrames << " completed in " << pg_nbClips << " dirs." << std::endl;

		// allocates first frame indices and number of frames for each clip
		pg_nbCompressedClipFramesPerFolder[indScenario] = new int[pg_nbClips[indScenario]];
		pg_firstCompressedClipFramesInFolder[indScenario] = new int[pg_nbClips[indScenario]];
		for (int ind = 0; ind < pg_nbClips[indScenario]; ind++) {
			pg_nbCompressedClipFramesPerFolder[indScenario][ind] = 0;
			pg_firstCompressedClipFramesInFolder[indScenario][ind] = -1;
		}

		// allocates clip data
		pg_ClipFrames_buffer_data[indScenario] = new ClipFramesData * [pg_nbCompressedClipFrames[indScenario]];
		for (int ind = 0; ind < pg_nbCompressedClipFrames[indScenario]; ind++) {
			pg_ClipFrames_buffer_data[indScenario][ind] = new ClipFramesData();
		}

		int indCompressedClipFrames = 0;
		int indCompressedClipDirs = 0;
		std::cout << pg_nbCompressedClipFrames[indScenario] << " images from " << pg_nbClips[indScenario] << " folders)" << std::endl;
		//pg_CurrentClipFramesDir = 0;
		//pg_CurrentClipFramesFile = 0;
		//std::cout << "Counting ClipFrames " << std::endl;
		if (fs::is_directory(pg_ClipDirectory[indScenario])) {
			auto dirIter = fs::directory_iterator(pg_ClipDirectory[indScenario]);
			for (auto& dir_entry : dirIter)
			{
				string dir_path = dir_entry.path().string();
				size_t last_separator = max(dir_path.find_last_of('/'), dir_path.find_last_of('\\'));
				string dir_name = dir_path;
				if (last_separator != string::npos) {
					dir_name = dir_path.substr(last_separator + 1);
				}
				std::cout << "dir " << "(" << indCompressedClipDirs << ")" << dir_path << ", name: " << dir_name << std::endl;
				pg_firstCompressedClipFramesInFolder[indScenario][indCompressedClipDirs] = indCompressedClipFrames;

				if (indCompressedClipDirs < pg_nbClips[indScenario] && dir_entry.is_directory())
				{
					int initialClipFrameIndex = indCompressedClipFrames;
					int nb_images_per_clip = 0;
					auto subDirIter = fs::directory_iterator(dir_entry);
					int nb_frames_in_clip = 0;
					for (auto& subdir_entry : subDirIter)
					{
						if (subdir_entry.is_regular_file())
						{
							++nb_frames_in_clip;
						}
					}
					subDirIter = fs::directory_iterator(dir_entry);
					for (auto& subdir_entry : subDirIter)
					{
						if (indCompressedClipFrames < pg_nbCompressedClipFrames[indScenario] 
							&& subdir_entry.is_regular_file() 
							&& nb_images_per_clip < clip_max_length[indScenario]) {
							if (pg_ClipFrames_buffer_data[indScenario][indCompressedClipFrames]->texBuffID == NULL_ID) {
								// allocates a texture ID for the image
								glGenTextures(1, &(pg_ClipFrames_buffer_data[indScenario][indCompressedClipFrames]->texBuffID));

								// loads the images with a size that corresponds to the screen size
								//printf("file %s\n", (char *)subdir_entry.path().string().c_str());
								//std::cout << "file " << subdir_entry.path().string() << std::endl;
								string dirName = subdir_entry.path().string();
								valret &= pg_ClipFrames_buffer_data[indScenario][indCompressedClipFrames]
									->pg_loadClipFrames(&dirName,
										clip_image_width[indScenario], clip_image_height[indScenario], indScenario);

								// loads the compressed image into GPU
								pg_ClipFrames_buffer_data[indScenario][indCompressedClipFrames]->pg_toGPUClipFrames(clip_image_width[indScenario], 
									clip_image_height[indScenario], GL_LINEAR, indScenario);
								//printf("texture ID dir %d indCompressedClipFrames %d texID %d\n", indCompressedClipDirs, indCompressedClipFrames, pg_ClipFrames_buffer_data[indCompressedClipFrames]->texBuffID);

								pg_printOglError(89);
								++indCompressedClipFrames;
								++nb_images_per_clip;
							}
						}
						else if (nb_images_per_clip >= clip_max_length[indScenario]) {
							std::cout << "clip: " << dir_name << " partially loaded (max frames per clip: " << clip_max_length[indScenario] << ") of " << nb_frames_in_clip  << " frames" << std::endl;
							break;
						}
					}
					pg_nbCompressedClipFramesPerFolder[indScenario][indCompressedClipDirs] = indCompressedClipFrames - initialClipFrameIndex;
					pg_clip_tracks[indScenario].push_back(pg_clip_track(indCompressedClipDirs, indCompressedClipFrames - initialClipFrameIndex, dir_name));
					++indCompressedClipDirs;
				}
			}
		}
		std::cout << "Clip Frames loading completed " << pg_nbCompressedClipFrames[indScenario] << " frames." << std::endl;
		//for (int ind = 0; ind < pg_nbClips; ind++) {
		//	printf("Clip No %d, first frame %d Nb frames %d\n", ind + 1, pg_firstCompressedClipFramesInFolder[ind], pg_nbCompressedClipFramesPerFolder[ind]);
		//}

		// stores predefined cues if there are some
		fs::path dir(pg_ClipDirectory[indScenario]);
		fs::path file("cues.txt");
		fs::path full_path = dir / file;
		printf("cues file path %ls\n", full_path.c_str());
		if (fs::is_regular_file(full_path)) {
			std::ifstream cuesFile(full_path);
			std::stringstream  sstream;
			string line;
			int clipNo;
			int cue0 = -1, cue1 = -1, cue2 = -1;
			if (!cuesFile) {
				sprintf(pg_errorStr, "Error: cues file [%ls] not opened!", full_path.c_str()); pg_ReportError(pg_errorStr);
			}
			while (std::getline(cuesFile, line)) {
				pg_stringstreamStoreLine(&sstream, &line);
				sstream >> clipNo;
				clipNo--;
				int cues[_NbMaxCues] = { -1 };
				if (_NbMaxCues >= 3 && clipNo < pg_nbClips[indScenario]) {
					sstream >> cues[0];
					sstream >> cues[1];
					sstream >> cues[2];

					for (int ind = 0; ind < 3; ind++) {
						if (cues[ind] >= 0) {
							pg_clip_tracks[indScenario][clipNo].set_cue(ind, cues[ind]);
							//printf("Cue for clip %d #%d at frame %d\n", clipNo, ind, cues[ind]);
						}
					}
				}
			}
		}
		else {
			sprintf(pg_errorStr, "Error: cues file [%ls] not provided - automatic cues!", full_path.c_str()); pg_ReportError(pg_errorStr);
			for (int clipNo = 0; clipNo < pg_nbClips[indScenario]; clipNo++) {
				//printf("Clip No %d (%s), first frame %d Nb frames %d\n", clipNo + 1, pg_firstCompressedClipFramesInFolder[clipNo], pg_nbCompressedClipFramesPerFolder[clipNo]);
				//cues[ind] = rand_0_1 * pg_nbCompressedClipFramesPerFolder[ind]
				int cues[_NbMaxCues] = { -1 };
				cues[0] = 0;
				cues[1] = int(floor(0.33 * pg_nbCompressedClipFramesPerFolder[indScenario][clipNo])) % pg_nbCompressedClipFramesPerFolder[indScenario][clipNo];
				cues[2] = int(floor(0.66 * pg_nbCompressedClipFramesPerFolder[indScenario][clipNo])) % pg_nbCompressedClipFramesPerFolder[indScenario][clipNo];
				pg_clip_tracks[indScenario][clipNo].set_cue(0, 0);
				pg_clip_tracks[indScenario][clipNo].set_cue(1, cues[1]);
				pg_clip_tracks[indScenario][clipNo].set_cue(2, cues[2]);
				//printf("Clip No %d (%s), first frame %d Nb frames %d cues (%d, %d, %d)\n",
				//	clipNo + 1, pg_clip_tracks[indScenario][clipNo].get_name().c_str(), 
				//	pg_firstCompressedClipFramesInFolder[indScenario][clipNo], 
				//	pg_nbCompressedClipFramesPerFolder[indScenario][clipNo],
				//	cues[0], cues[1], cues[2]);
			}
		}
	}
	return valret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE SCENARIO
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_parseScenario_Textures(std::ifstream& scenarioFin, int indScenario) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////	
	////// TEXTURES
	// the textures are loaded inside the GPU and diplayed path by path

	// initial markup
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string textures
	if (ID.compare("textures") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"textures\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	// Number of textures
	while (true) {
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string texture
		if (ID.compare("/textures") == 0) {
			break;
		}
		else if (ID.compare("texture") != 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"texture\" not found! (instead \"%s\")\n", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}

		pg_TextureData cur_texture;

		sstream >> ID; // file name
		if (!pg_isFullPath(ID)) {
			cur_texture.texture_fileName = pg_textures_directory + ID;
		}
		else {
			cur_texture.texture_fileName = ID;
		}
		sstream >> cur_texture.texture_fileNameSuffix; // file suffix

		// usage
		sstream >> ID;
		//printf("Usage %s: (%s)\n", cur_texture.texture_fileName.c_str(), ID.c_str());
		if (ID.compare("master_mask") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_master_mask;
		}
		else if (ID.compare("mesh") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_mesh;
		}
		else if (ID.compare("sensor") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_sensor;
		}
		else if (ID.compare("logo") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_logo;
		}
		else if (ID.compare("brush") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_brush;
		}
		else if (ID.compare("noise") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_noise;
		}
		else if (ID.compare("curve_particle") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_particle;
		}
		else if (ID.compare("splat_particle") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_splat_particle;
		}
		else if (ID.compare("part_init") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_part_init;
		}
		else if (ID.compare("part_acc") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_part_acc;
		}
		else if (ID.compare("pixel_acc") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_pixel_acc;
		}
		else if (ID.compare("repop_density") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_repop_density;
		}
		else if (ID.compare("multilayer_master_mask") == 0) {
			cur_texture.texture_usage = pg_enum_Texture_multilayer_master_mask;
		}
		else {
			sprintf(pg_errorStr, "Error: incorrect configuration file Texture usage \"%s\"\n", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}
		// rank of the texture (used in particular for meshes)
		sstream >> cur_texture.texture_Rank;
		// dimension (2 or 3)
		sstream >> cur_texture.texture_Dimension;
		if (cur_texture.texture_Dimension != 2
			&& cur_texture.texture_Dimension != 3) {
			sprintf(pg_errorStr, "Error: 2D or 3D texture dimension expected, not %d for texture (%s)\n", cur_texture.texture_Dimension, cur_texture.texture_fileName.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}
		// number of piled textures in case of 3D texture or tif format
		sstream >> cur_texture.texture_Nb_Layers;
		if (cur_texture.texture_usage == pg_enum_Texture_brush) {
			nb_pen_brushes[indScenario] = cur_texture.texture_Nb_Layers;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_multilayer_master_mask) {
			nb_layers_master_mask[indScenario] = cur_texture.texture_Nb_Layers;
		}

		// image initial geometry
		sstream >> cur_texture.texture_Size_X;
		sstream >> cur_texture.texture_Size_Y;
		if (cur_texture.texture_usage == pg_enum_Texture_master_mask &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error: master mask texture should be minimlally %dx%d (%dx%d)\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_multilayer_master_mask &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error: multilayer master mask texture should be minimlally %dx%d (%dx%d)\n", PG_WINDOW_WIDTH, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_noise &&
			(cur_texture.texture_Size_X < pg_workingWindow_width_powerOf2 || cur_texture.texture_Size_Y < pg_window_height_powerOf2)) {
			sprintf(pg_errorStr, "Error: noise texture should be minimlally %dx%d (%dx%d)\n", pg_workingWindow_width_powerOf2, pg_window_height_powerOf2,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_part_init &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error: particle initialization texture should be minimlally %dx%d (%dx%d)\n", pg_workingWindow_width, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_part_acc &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error: particle acceleration texture should be minimlally %dx%d (%dx%d)\n", pg_workingWindow_width, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_pixel_acc &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error: pixel acceleration texture should be minimlally %dx%d (%dx%d)\n", pg_workingWindow_width, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}
		if (cur_texture.texture_usage == pg_enum_Texture_repop_density &&
			(cur_texture.texture_Size_X < pg_workingWindow_width || cur_texture.texture_Size_Y < PG_WINDOW_HEIGHT)) {
			sprintf(pg_errorStr, "Error:  repopulation density texture should be minimlally %dx%d (%dx%d)\n", pg_workingWindow_width, PG_WINDOW_HEIGHT,
				cur_texture.texture_Size_X, cur_texture.texture_Size_Y); pg_ReportError(pg_errorStr); throw 100;
		}

		// image color depth
		sstream >> cur_texture.texture_Nb_Bytes_per_Pixel;

		// booleans invert & is rectangle
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			cur_texture.texture_Is_Rectangle = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			cur_texture.texture_Is_Rectangle = false;
		}
		else {
			sprintf(pg_errorStr, "Error: incorrect boolean for Texture rectangle \"%s\" (true or false expected)\n", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}
		sstream >> ID;
		if (ID.compare("true") == 0 || ID.compare("TRUE") == 0) {
			cur_texture.texture_Invert = true;
		}
		else if (ID.compare("false") == 0 || ID.compare("FALSE") == 0) {
			cur_texture.texture_Invert = false;
		}
		else {
			sprintf(pg_errorStr, "Error: incorrect boolean for Texture invert \"%s\" (true or false expected)\n", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}

		pg_Textures[indScenario].push_back(cur_texture);

		//printf("Texture #%d size (%d,%d), rank %d, usage %d\n",
		//	pg_Textures[indScenario].size(), cur_texture.texture_Size_X, cur_texture.texture_Size_Y,
		//	cur_texture.texture_Rank, cur_texture.texture_usage);
	}
	// /textures
}

