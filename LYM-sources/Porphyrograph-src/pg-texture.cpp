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
bool pg_FirstMovieFrameIsAvailable = false; // Available first frame so as to avoid second reading  

////////////////////////////////////////////////////////////////////
// CAMERA FRAME
////////////////////////////////////////////////////////////////////
Mat pg_camera_frame;


////////////////////////////////////////////////////////////////////
// ClipFrames TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for blending
// PhotoBufferDataStructOld describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by blend images used to smoothly change between images
PhotoData pg_CameraFrame_buffer_data = PhotoData();
PhotoData pg_MovieFrame_buffer_data = PhotoData();

float pg_Photo_weight[PG_PHOTO_NB_TEXTURES] = { 0.0f , 0.0f };

////////////////////////////////////////////////////////////////////
// BLENDED MULTILAYER IMAGES FOR TVW
////////////////////////////////////////////////////////////////////
PhotoSwapData pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES] = { PhotoSwapData() };

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
double pg_CurrentDiaporamaEnd = -1;
bool ascendingDiaporama = true;

std::string pg_MessageFile[PG_MAX_CONFIGURATIONS];

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoData describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
vector<int> pg_nbCompressedImagesPerFolder[PG_MAX_CONFIGURATIONS];
int pg_nbCompressedImageDirs[PG_MAX_CONFIGURATIONS] = { 0 };
int pg_nbCompressedImages[PG_MAX_CONFIGURATIONS] = { 0 };
// diaporama directories
vector<string> pg_compressedImageDirsNames[PG_MAX_CONFIGURATIONS];
// files inside diaporama directories
vector <vector<PhotoData*>> pg_compressedImageData[PG_MAX_CONFIGURATIONS];
vector<vector<string>> pg_compressedImageFilesNames[PG_MAX_CONFIGURATIONS];
// the index from which an image available for swapping is looked for
int pg_IndInitialSwapPhoto = 0;

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

unsigned int imgPhotoCompressedFormat[PG_MAX_CONFIGURATIONS];
unsigned char* imgPhotoCompressedBitmap[PG_MAX_CONFIGURATIONS] = { NULL };
cv::Mat imgPhotoBGRInit[PG_MAX_CONFIGURATIONS];
cv::Mat imgPhotoRGB[PG_MAX_CONFIGURATIONS];


////////////////////////////////////////////////////////////////////
// SHORT CLIP IMAGE FILES
////////////////////////////////////////////////////////////////////
ClipFramesData** pg_ClipFrames_buffer_data[PG_MAX_CONFIGURATIONS] = { NULL };


////////////////////////////////////////////////////////////////////
// CLIP TEXTURES
////////////////////////////////////////////////////////////////////
int pg_nbClips[PG_MAX_CONFIGURATIONS] = { 0 };
int pg_nbCompressedClipFrames[PG_MAX_CONFIGURATIONS] = { 0 };
int* pg_nbCompressedClipFramesPerFolder[PG_MAX_CONFIGURATIONS] = { NULL };
int* pg_firstCompressedClipFramesInFolder[PG_MAX_CONFIGURATIONS] = { NULL };
//int pg_CurrentClipFramesFile = 0;
//int pg_CurrentClipFramesDir = -1;

////////////////////////////////////////////////////////////////////
// BG COLOR
////////////////////////////////////////////////////////////////////
#if defined(var_flashchange_BGcolor_freq)
float BG_r = 0.f;
float BG_g = 0.f;
float BG_b = 0.f;
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


////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE INITIALIZATION
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_loadAllTextures(void) {
	std::cout << "Loading textures: " << std::endl;

	//////////////////////////////////////////////////////////////
	// loaded textures
	// loads all the textures from the configuration file
	// unless the image itself is used to generate other textures
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";
		for (pg_TextureData* texture : pg_Textures[indConfiguration]) {
			if (texture->texture_usage != Texture_part_init
				&& texture->texture_usage != Texture_part_acc
				&& texture->texture_usage != Texture_pixel_acc) {
				if (texture->texture_Dimension == 2) {
					pg_loadTexture2D(texture,
						GL_RGBA8, GL_RGBA,
						GL_UNSIGNED_BYTE, GL_LINEAR);
					printOglError(8);
				}
				else if (texture->texture_Dimension == 3) {
					pg_loadTexture3D(texture,
						GL_RGBA8, GL_RGBA,
						GL_UNSIGNED_BYTE, GL_LINEAR);
					printOglError(8);
				}
			}
#if defined(var_part_initialization) 
			else if (texture->texture_usage == Texture_part_init
				&& texture->texture_Dimension == 2) {
				std::cout << texture->texture_fileName + texture->texture_fileNameSuffix + " (particle init), ";
				if (generateParticleInitialPosColorRadiusfromImage(texture->texture_fileName + texture->texture_fileNameSuffix, 
					indConfiguration)) {
					if (texture->texture_Rank != pg_particle_initial_pos_speed_texID[indConfiguration].size()
						|| texture->texture_Rank != pg_particle_initial_color_radius_texID[indConfiguration].size()) {
						sprintf(ErrorStr, "Error: particle initialization texture #%lu/%lu rank incorrect (%d rank expected)!\n", pg_particle_initial_pos_speed_texID[indConfiguration].size(), pg_particle_initial_color_radius_texID[indConfiguration].size(), texture->texture_Rank); ReportError(ErrorStr); throw 336;
					}
				}
			}
#endif
#if defined(var_part_image_acceleration) 
			else if (texture->texture_usage == Texture_part_acc
				&& texture->texture_Dimension == 2) {
				std::cout << texture->texture_fileName + texture->texture_fileNameSuffix + " (particle acc), ";
				GLuint textureParticle_acc_ID = NULL_ID;
				pg_loadTexture2D(texture,
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR);
				pg_particle_acc_texID[indConfiguration].push_back(textureParticle_acc_ID);
			}
#endif
#if defined(var_pixel_image_acceleration) 
			else if (pg_ScenarioActiveVars[_pixel_image_acceleration][indConfiguration]
				&& texture->texture_usage == Texture_pixel_acc
				&& texture->texture_Dimension == 2) {
				std::cout << texture->texture_fileName + texture->texture_fileNameSuffix + " (pixel acc), ";
				GLuint texturePixel_acc_ID = -1;
				pg_loadTexture2D(texture,
					GL_RGBA8, GL_RGBA,
					GL_UNSIGNED_BYTE, GL_LINEAR);
				pg_pixel_acc_texID[indConfiguration].push_back(texturePixel_acc_ID);
			}
#endif
		}

		// assigns the textures to their destination according to their usage field
		for (pg_TextureData* texture : pg_Textures[indConfiguration]) {
#if defined(var_sensor_layout)
			if (pg_ScenarioActiveVars[_sensor_layout][indConfiguration]) {
				if (texture->texture_usage == Texture_sensor
					&& texture->texture_Dimension == 2) {
					Sensor_texture_rectangle[indConfiguration] = texture->texture_texID;
				}
			}
#endif
#ifdef PG_WITH_MASTER_MASK
			if (texture->texture_usage == Texture_master_mask
				&& texture->texture_Dimension == 2) {
				Master_Mask_texID[indConfiguration] = texture->texture_texID;
			}
			else if (texture->texture_usage == Texture_multilayer_master_mask
				&& texture->texture_Dimension == 3) {
				Master_Multilayer_Mask_texID[indConfiguration] = texture->texture_texID;
			}
#endif
#if !defined (PG_BEZIER_PATHS) || defined(CORE)
			if (texture->texture_usage == Texture_brush
				&& texture->texture_Dimension == 3) {
				Pen_texture_3D_texID[indConfiguration] = texture->texture_texID;
			}
#endif
			if (texture->texture_usage == Texture_noise
				&& texture->texture_Dimension == 3) {
				Noise_texture_3D[indConfiguration] = texture->texture_texID;
			}
#if defined(var_Part_repop_density) || defined(var_BG_CA_repop_density) 
			if (texture->texture_usage == Texture_repop_density
				&& texture->texture_Dimension == 2) {
				pg_RepopDensity_texture_texID[indConfiguration].push_back(texture->texture_texID);
				std::cout << texture->texture_fileName + texture->texture_fileNameSuffix + " (repop density), ";
				if (texture->texture_Rank != pg_RepopDensity_texture_texID[indConfiguration].size()) {
					sprintf(ErrorStr, "Error: repopulation texture density #%lu rank incorrect (%d rank expected)!\n", pg_RepopDensity_texture_texID[indConfiguration].size(), texture->texture_Rank); ReportError(ErrorStr); throw 336;
				}
			}
#endif
			if (pg_ScenarioActiveVars[_part_initialization][indConfiguration]) {
				// printf("Loading particles initial images %s\n", fileName.c_str()); 
				if (!pg_particle_initial_pos_speed_texID[indConfiguration].size()
					|| !pg_particle_initial_color_radius_texID[indConfiguration].size()) {
					sprintf(ErrorStr, "Error: particle initialization texture not provided for configuration %d scenario %s, check texture list with part_init usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
				}
			}
#if defined(var_activeMeshes)
			if (texture->texture_usage == Texture_mesh
				&& texture->texture_Dimension == 2) {
				// assigns this textures to the meshes which have the same texture rank
				for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[indConfiguration].size(); indMeshFile++) {
					if (pg_Meshes[indConfiguration][indMeshFile]->pg_Mesh_TextureRank == texture->texture_Rank) {
						pg_Meshes[indConfiguration][indMeshFile]->Mesh_texture_rectangle = texture->texture_texID;
					}
				}
			}
#endif
#ifdef CURVE_PARTICLES
			if (texture->texture_usage == Texture_curve_particle
				&& texture->texture_Dimension == 2) {
				comet_texture_2D_texID[indConfiguration] = texture->texture_texID;
			}
#endif
#if defined(TEXTURED_QUAD_PARTICLES)
			if (texture->texture_usage == Texture_splat_particle
				&& texture->texture_Dimension == 2) {
				blurredDisk_texture_2D_texID[indConfiguration].push_back(texture->texture_texID);
				std::cout << texture->texture_fileName + texture->texture_fileNameSuffix + " (splat), ";
				if (texture->texture_Rank != blurredDisk_texture_2D_texID[indConfiguration].size()) {
					sprintf(ErrorStr, "Error: texture splat image #%lu rank incorrect (%d rank expected)!\n", blurredDisk_texture_2D_texID[indConfiguration].size(), texture->texture_Rank); ReportError(ErrorStr); throw 336;
				}
			}
#endif
		}
		// and checks that the textures were provided
		if (pg_ScenarioActiveVars[_sensor_layout][indConfiguration]) {
			if (Sensor_texture_rectangle[indConfiguration] == NULL_ID) {
				sprintf(ErrorStr, "Error: sensor texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
			}
		}
#ifdef PG_WITH_MASTER_MASK
		if (Master_Mask_texID[indConfiguration] == NULL_ID || Master_Multilayer_Mask_texID[indConfiguration] == NULL_ID) {
			sprintf(ErrorStr, "Error: master mask texture (usage: master_mask) and multilayer master mask texture (usage: multilayer_master_mask) not provided for configuration %d scenario %s, check texture list with master mask usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
		}
#endif
#if !defined (PG_BEZIER_PATHS) || defined(CORE)
		if (Pen_texture_3D_texID[indConfiguration] == NULL_ID) {
			sprintf(ErrorStr, "Error: brush texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
		}
#endif
		if (Noise_texture_3D[indConfiguration] == NULL_ID) {
			sprintf(ErrorStr, "Error: noise texture not provided for configuration %d scenario %s, check texture list with sensor usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
		}
#if defined(var_Part_repop_density)
		if (pg_ScenarioActiveVars[_Part_repop_density][indConfiguration]) {
			if (pg_RepopDensity_texture_texID[indConfiguration].size() == 0) {
				sprintf(ErrorStr, "Error: particle repopulation texture density not provided for configuration %d scenario %s, check texture list with BG_CA_repop_density usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
			}
		}
#endif
#if defined(var_BG_CA_repop_density) 
		if (pg_ScenarioActiveVars[_BG_CA_repop_density][indConfiguration]) {
			if (pg_RepopDensity_texture_texID[indConfiguration].size() == 0) {
				sprintf(ErrorStr, "Error: BG/CA repopulation texture density not provided for configuration %d scenario %s, check texture list with BG_CA_repop_density usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
			}
		}
#endif
#if defined(var_activeMeshes)
		// assigns this textures to the meshes which have the same texture rank
		for (unsigned int indMeshFile = 0; indMeshFile < pg_Meshes[indConfiguration].size(); indMeshFile++) {
			if (pg_Meshes[indConfiguration][indMeshFile]->pg_Mesh_TextureRank != -1 // frame buffer texture
				&& pg_Meshes[indConfiguration][indMeshFile]->Mesh_texture_rectangle == NULL_ID) {
				sprintf(ErrorStr, "Error: mesh texture not provided  for configuration %d scenario %s (mesh #%d and texture rank %d, check texture list with mesh usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str(), indMeshFile, pg_Meshes[indConfiguration][indMeshFile]->pg_Mesh_TextureRank); ReportError(ErrorStr); throw 336;
			}
		}
#endif
#ifdef CURVE_PARTICLES
		if (comet_texture_2D_texID[indConfiguration] == NULL_ID) {
			sprintf(ErrorStr, "Error: curve particles texture not provided for configuration %d scenario %s, check texture list with curve_particle usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
		}
#endif
#if defined(TEXTURED_QUAD_PARTICLES)
		if (blurredDisk_texture_2D_texID[indConfiguration].size() == 0) {
			sprintf(ErrorStr, "Error: splat particles texture not provided for configuration %d scenario %s, check texture list with splat_particle usage!\n", indConfiguration, pg_ScenarioFileNames[indConfiguration].c_str()); ReportError(ErrorStr); throw 336;
		}
#endif
		pg_loadTexture2D(&texDataScreenFont, GL_RGB8, GL_LUMINANCE,
			GL_UNSIGNED_BYTE, GL_LINEAR);
		printOglError(6);

		//////////////////////////////////////////////////////////////
		// generated textures
		pg_screenMessageBitmap =
			(GLubyte*)pg_generateTexture(&pg_screenMessageBitmap_texID, pg_byte_tex_format,
				message_pixel_length, 1);
		if (!pg_screenMessageBitmap) {
			sprintf(ErrorStr, "Error: screen message bitmap not allocated (%s)!\n", ScreenMessage.c_str()); ReportError(ErrorStr); throw 336;
		}

	// CA Tables
#if defined(var_CATable)
#define width_data_table 600
#define height_data_table 200
		pg_CATable =
			(GLubyte*)pg_generateTexture(&pg_CATable_ID, pg_byte_tex_format,
				width_data_table, height_data_table);
		pg_CATable_values(pg_CATable_ID, pg_CATable,
			width_data_table, height_data_table);
		if (!pg_CATable_ID) {
			sprintf(ErrorStr, "Error: data tables for the CA bitmap not allocated (%dx%d)!\n", width_data_table, height_data_table); ReportError(ErrorStr); throw 336;
		}

		//printf( "\n" );
		//for( int i = 0 ; i <= 243 * 4 ; i+=4 ) {
		//  printf("%d " , (int)pg_CATable[ 4 * 51 * width_data_table + i ] );
		//}
		//printf( "\n" );
#endif
		std::cout << std::endl;

		printOglError(6);
	}
	std::cout << std::endl;
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

#if defined(var_CATable) && !defined(CAAUDIO)
#include "pg_CATable_GN.cpp"
#endif

#if defined(var_CATable) && defined(CAAUDIO)
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

/* Attempts to save PNG to file; returns 0 on success, non-zero on error. 
#ifdef WIN32
DWORD WINAPI writepng(LPVOID lpParam) {
#else
void* writepng(void* lpParam) {
#endif
	threadData* pDataArray = (threadData*)lpParam;

	cv::flip(pDataArray->imgToBeWrittenThreadData, pngImgMatRGBFlipped, 0); // vertical flip
#ifndef OPENCV_3
	cv::cvtColor(*(pDataArray->imgThreadData), pngImgMatBGR, CV_RGB2BGR);

	std::vector<int> params;
	params.push_back(CV_IMWRITE_PNG_COMPRESSION);
#else
	cv::cvtColor(pngImgMatRGBFlipped, pngImgMatBGR, cv::COLOR_RGB2BGR);

	std::vector<int> params;
	params.push_back(cv::IMWRITE_PNG_COMPRESSION);
#endif
	params.push_back(9);
	params.push_back(0);
	cv::imwrite(pDataArray->imageFileName, pngImgMatBGR);

	return 0;
}
*/

void writepng(cv::String imageFileName) {
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


void writejpg(cv::String imageFileName) {
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



/////////////////////////////////////////////////////////////////
// NON THREADED TEXTURE LOADING
/////////////////////////////////////////////////////////////////
// 3D texture loading + transfer to GPU
bool pg_loadTexture3D(pg_TextureData *texData,
	GLint components, GLenum format,
	GLenum datatype, GLenum texturefilter) {
	// data type is assumed to be GL_UNSIGNED_BYTE

	long size = texData->texture_Size_X * texData->texture_Size_Y * texData->texture_Nb_Bytes_per_Pixel;
	GLubyte * bitmap = new unsigned char[size * texData->texture_Nb_Layers];
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
		sprintf(ErrorStr, "Unexpected 3D image extension %s!\n", texData->texture_fileNameSuffix.c_str()); ReportError(ErrorStr); throw 425;
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
			sprintf(ErrorStr, "The number of layers does not match the value in the scenario file %s %lu vs %d!\n", fileName.c_str(), pages.size(), texData->texture_Nb_Layers); ReportError(ErrorStr); throw 425;
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
				sprintf(ErrorStr, "Could not open or find the image %s!\n", fileName.c_str()); ReportError(ErrorStr); throw 425;
				return false;
			}
			if (tiffLayerMatBGRFlipped.cols != texData->texture_Size_X
				|| tiffLayerMatBGRFlipped.rows != texData->texture_Size_Y) {   // Check for invalid input
				sprintf(ErrorStr, "Unexpected multilayer tiff image size %s %d/%d %d/%d!\n", fileName.c_str(), 
					tiffLayerMatBGRFlipped.cols, texData->texture_Size_X, tiffLayerMatBGRFlipped.rows, texData->texture_Size_Y); ReportError(ErrorStr); throw 425;
				return false;
			}
			if (tiffLayerMatBGRFlipped.elemSize() != texData->texture_Nb_Bytes_per_Pixel) {   // Check for invalid input
				sprintf(ErrorStr, "Unexpected multilayer tiff image bytes per pixel %s %lu/%d!\n", fileName.c_str(), tiffLayerMatBGRFlipped.elemSize(), texData->texture_Nb_Bytes_per_Pixel); ReportError(ErrorStr); throw 425;
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
				sprintf(ErrorStr, "Could not open or find the image %s!\n", fileName.c_str()); ReportError(ErrorStr); throw 425;
				return false;
			}
			if (image3DLayerRGBFlipped.cols != texData->texture_Size_X
				|| image3DLayerRGBFlipped.rows != texData->texture_Size_Y) {   // Check for invalid input
				sprintf(ErrorStr, "Unexpected 3D image size %s %d/%d %d/%d!\n", fileName.c_str(), 
					image3DLayerRGBFlipped.cols, texData->texture_Size_X, image3DLayerRGBFlipped.rows, texData->texture_Size_Y); ReportError(ErrorStr); throw 425;
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

	printOglError(0);

	// memory release
	delete[] bitmap;

	glEnable(GL_TEXTURE_RECTANGLE);

	// glGenerateMipmap(GL_TEXTURE_2D);
	// printf("3D textures loaded\n" );
	return true;
}

// 2D texture loading + transfer to GPU

bool pg_loadTexture2D(pg_TextureData *texData,
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
		sprintf(ErrorStr, "RGBA image expected for texture %s not %d-channel!\n", fileName.c_str(), textureImgMatBGRInit.channels()); ReportError(ErrorStr); throw 425;
	}
	else if (format == GL_LUMINANCE && textureImgMatBGRInit.channels() != 1) {
		sprintf(ErrorStr, "Gray level image expected for texture %s not %d-channel!\n", fileName.c_str(), textureImgMatBGRInit.channels()); ReportError(ErrorStr); throw 425;
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
		sprintf(ErrorStr, "Could not open or find the image %s!\n", fileName.c_str()); ReportError(ErrorStr); throw 425;
		return false;
	}
	if (textureImgMatRGBFlipped.cols != texData->texture_Size_X || textureImgMatRGBFlipped.rows != texData->texture_Size_Y) {                              // Check for invalid input
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!\n", fileName.c_str(), textureImgMatRGBFlipped.cols, 
			texData->texture_Size_X, textureImgMatRGBFlipped.rows, texData->texture_Size_Y); ReportError(ErrorStr); throw 425;
		return false;
	}

	// glActiveTexture (GL_TEXTURE0 + index);
	if (texData->texture_Is_Rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, texData->texture_texID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			textureImgMatRGBFlipped.cols,          // Image width
			textureImgMatRGBFlipped.rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			textureImgMatRGBFlipped.ptr());        // The actual image data itself
		printOglError(40);
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
		printOglError(50);
		glEnable(GL_TEXTURE_RECTANGLE);
		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	return true;
}


//////////////////////////////////////////////////////////////////
// INITIALIZES PARTICLES FROM PHOTO
/////////////////////////////////////////////////////////////////

bool generateParticleInitialPosColorRadiusfromImage(string fileName, int indConfiguration) { // 2 texture IDs one for pos/speed, the other one for color/radius
	bool invert = false;

	printOglError(705);

	std::cout << fileName + " (2D particle init), ";

#ifndef OPENCV_3
	particleImgMatBGRInit = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	particleImgMatBGRInit = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
																// Check for invalid input
	if (!particleImgMatBGRInit.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // ReportError(ErrorStr); throw 425;
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

	if (nb_particles > workingWindow_width * window_height) {
		sprintf(ErrorStr, "Particle size should be lower than %d!\n", workingWindow_width * window_height); ReportError(ErrorStr); throw 425;
		return false;
	}
	//else {
	//	 printf("File %s nb channels %d for %d particles in window (%dx%d)\n", fileName.c_str(), nbChannels, nb_particles, workingWindow_width, window_height);
	//}

	float *pos_speed = new float[workingWindow_width * window_height * 4];
	float *color_radius = new float[workingWindow_width * window_height * 4];
	bool *allocated = new bool[workingWindow_width * window_height];
	for (int i = 0; i < workingWindow_width * window_height; i++) {
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
	float image_ratio = float(workingWindow_width) / float(window_height);
	int partic_height = int(floor(sqrt(nb_particles / image_ratio)));
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
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_pos_speed_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, workingWindow_width, window_height, 0,
		GL_RGBA, GL_FLOAT, pos_speed);
	pg_particle_initial_pos_speed_texID[indConfiguration].push_back(textureParticleInitialization_pos_speed_ID);

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_color_radius_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, workingWindow_width, window_height, 0,
		GL_RGBA, GL_FLOAT, color_radius);
	pg_particle_initial_color_radius_texID[indConfiguration].push_back(textureParticleInitialization_color_radius_ID);

	//printf("Init image loaded %s Real size %dx%d\nParticle size %d=%dx%d (of %d particles)\nRadius %.2f\n",
	//	fileName.c_str(),
	//	image.cols, image.rows,
	//	partic_width * partic_height, partic_width, partic_height, nb_particles,
	//	partic_radius);

	free(color_radius);
	free(pos_speed);
	free(allocated);

	printOglError(706);

	return true;
}

//////////////////////////////////////////////
// particle acceleration shift from photography
// stored into pg_particle_acc_texID
/*
bool storeParticleAccelerationfromImage(string fileName)  {
	bool invert = false;

	printOglError(705);

#ifndef OPENCV_3
	particleAccImgMatBGRInit = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
#else
	particleAccImgMatBGRInit = cv::imread(fileName, cv::IMREAD_UNCHANGED);   // Read the file
#endif
																// Check for invalid input
	if (!particleAccImgMatBGRInit.data) {
		printf("Could not open or find the 2D image (%s)!\n", fileName.c_str()); // ReportError(ErrorStr); throw 425;
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

	if (nb_particles > workingWindow_width * window_height) {
		sprintf(ErrorStr, "Particle size should be lower than %d!\n", workingWindow_width * window_height); ReportError(ErrorStr); throw 425;
		return false;
	}
	//else {
	//	 printf("File %s nb channels %d for %d particles in window (%dx%d)\n", fileName.c_str(), nbChannels, nb_particles, workingWindow_width, window_height);
	//}

	float* acceleration_shift = new float[workingWindow_width * window_height * 4];
	for (int i = 0; i < workingWindow_width * window_height; i++) {
		acceleration_shift[i * 4 + 0] = 1.f;
		acceleration_shift[i * 4 + 1] = 1.f;
		acceleration_shift[i * 4 + 2] = 1.f;
		acceleration_shift[i * 4 + 3] = 1.f;
	}

	//printf("image %d %d %d %d\n", image.rows, image.cols, window_height, workingWindow_width);
	if (particleAccImgMatRGBFlipped.rows < window_height) {
		sprintf(ErrorStr, "Image for particle acceleration height (%d) should be higher or equal than window height %d!\n", particleAccImgMatRGBFlipped.rows, window_height); ReportError(ErrorStr); throw 425;
	}
	if (particleAccImgMatRGBFlipped.cols < workingWindow_width) {
		sprintf(ErrorStr, "Image for particle acceleration widht (%d) should be higher or equal than window width %d!\n", particleAccImgMatRGBFlipped.cols, workingWindow_width); ReportError(ErrorStr); throw 425;
	}

	//printf("window dim %dx%d\n", workingWindow_width, window_height);
	for (int indRow = 0; indRow < window_height; indRow++) {
		for (int indCol = 0; indCol < workingWindow_width; indCol++) {
			float r = 0.f, g = 0.f, b = 0.f;
			Point3_<uchar>* p = particleAccImgMatRGBFlipped.ptr<Point3_<uchar> >(particleAccImgMatRGBFlipped.rows - indRow - 1, indCol);
			int indPixel = indRow * workingWindow_width + indCol;
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
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glBindTexture(GL_TEXTURE_RECTANGLE, textureParticleInitialization_acc_ID);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, workingWindow_width, window_height, 0,
		GL_RGBA, GL_FLOAT, acceleration_shift);
	pg_particle_acc_texID.push_back(textureParticleInitialization_acc_ID[indConfiguration]);

	//printf("Init image loaded %s Real size %dx%d\nParticle size %d=%dx%d (of %d particles)\nRadius %.2f\n",
	//	fileName.c_str(),
	//	image.cols, image.rows,
	//	partic_width * partic_height, partic_width, partic_height, nb_particles,
	//	partic_radius);
	//printf("Loading particles acceleration shift %s\n", fileName.c_str());

	free(acceleration_shift);

	printOglError(706);

	return true;
}
*/

//////////////////////////////////////////////
/// NON THREADED LOAD CAMERA FRAME
#if defined(var_cameraCaptFreq)
void loadCameraFrame(bool initial_capture, int IPCam_no) {
#if defined(KOMPARTSD) || defined(LIGHT)
	return;
#endif
	//Grabs and returns a frame from camera
	if (IPCam_no < 0) {
		if (pg_webCam_capture.grab()) {
			pg_webCam_capture.retrieve(pg_camera_frame);
		}
	}
	else if (IPCam_no < int(pg_IPCam_capture.size())) {
		pg_IPCam_capture[IPCam_no] >> pg_camera_frame;
	}
	
	// if it is an initial capture defines image size and allocates frame texture
	if (initial_capture) {
		//Grabs and returns a frame from camera
		if (!pg_camera_frame.data) {
			if (IPCam_no < 0) {
				sprintf(ErrorStr, "WebCam initial frame not grabbed! \n"); ReportError(ErrorStr); throw 425;
			}
			else if (IPCam_no < int(pg_IPCam_capture.size())) {
				sprintf(ErrorStr, "IPCam #%d initial frame not grabbed! \n", IPCam_no); ReportError(ErrorStr); throw 425;
			}
		}
		if (!pg_camera_texture_texID) {
			pg_initCameraFrameTexture(&pg_camera_frame);
		}

		int camera_x_offset = -1;
		int camera_frame_width = -1;
		int camera_y_offset = -1;
		int camera_frame_height = -1;
		if (pg_camera_frame.cols == 1920 && pg_camera_frame.rows == 1080) {
			// camera frame dimensions and offset
			camera_x_offset = 0;
			camera_frame_width = pg_camera_frame.cols;
			camera_y_offset = 0;
			camera_frame_height = pg_camera_frame.rows;
		}
		else {
			// tries to preserve ratio
		float window_ratio = float(workingWindow_width) / window_height;
			if (pg_camera_frame.rows * window_ratio < pg_camera_frame.cols) {
				//printf("camera size height leading\n");
				camera_frame_width = int(pg_camera_frame.rows * window_ratio);
				camera_frame_height = pg_camera_frame.rows;
				camera_x_offset = (pg_camera_frame.rows - camera_frame_width) / 2;
				camera_y_offset = 0;
			}
			else {
				//printf("camera size width leading\n");
				camera_frame_width = pg_camera_frame.cols;
				camera_frame_height = pg_camera_frame.rows;
				if (pg_camera_frame.cols / window_ratio < pg_camera_frame.rows) {
					// printf("horizontal offset\n");
					camera_x_offset = (int(pg_camera_frame.rows * window_ratio) - pg_camera_frame.cols) / 2;
					camera_y_offset = 0;
				}
				else {
					// printf("camera height respecting\n");
					camera_frame_height = pg_camera_frame.rows;
					camera_x_offset = 0;
					camera_y_offset = 0;
				}
			}
			float ratioSubImage = 1.0f;
			camera_x_offset = camera_x_offset + int(camera_frame_width * (1.f - ratioSubImage) * 0.5);
			camera_frame_width = int(camera_frame_width * ratioSubImage);
			camera_y_offset = camera_y_offset + int(camera_frame_height * (1.f - ratioSubImage) * 0.5);
			camera_frame_height = int(camera_frame_height * ratioSubImage);
		}

		// first camera frame allocation
		if (pg_camera_x_offset == -1 && pg_camera_frame_width == -1 && pg_camera_y_offset == -1 && pg_camera_frame_height == -1) {
			pg_camera_x_offset = camera_x_offset;
			pg_camera_frame_width = camera_frame_width;
			pg_camera_y_offset = camera_y_offset;
			pg_camera_frame_height = camera_frame_height;
		}
		else {
			if (pg_camera_x_offset != camera_x_offset || pg_camera_frame_width != camera_frame_width || pg_camera_y_offset != camera_y_offset || pg_camera_frame_height != camera_frame_height) {
				sprintf(ErrorStr, "Camera frames differ in size: wxh %dx%d / %dx%d offset xy %dx%d / %dx%d\n", 
					pg_camera_frame_width, pg_camera_frame_height, camera_frame_width, camera_frame_height, pg_camera_x_offset, pg_camera_y_offset, camera_x_offset, camera_y_offset); ReportError(ErrorStr); throw 425;
			}
		}

		if (IPCam_no < 0) {
			printf("WebCam frame %dx%d (before ratio %dx%d) offset (%dx%d) ch %d size %d\n",
				pg_camera_frame_width, pg_camera_frame_height,
				pg_camera_frame.cols, pg_camera_frame.rows,
				pg_camera_x_offset, pg_camera_y_offset,
				pg_camera_frame.channels(), int(pg_camera_frame.total() * pg_camera_frame.elemSize()));
		}
		else if (IPCam_no < int(pg_IPCam_capture.size())) {
			printf("IPCam #%d frame %dx%d (before ratio %dx%d) offset (%dx%d) ch %d size %d\n", IPCam_no,
				pg_camera_frame_width, pg_camera_frame_height,
				pg_camera_frame.cols, pg_camera_frame.rows,
				pg_camera_x_offset, pg_camera_y_offset,
				pg_camera_frame.channels(), int(pg_camera_frame.total() * pg_camera_frame.elemSize()));
		}
	}
	// end of initial capture

	if (pg_camera_frame.data) {
		// printf("sends camera frame to GPU\n");
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
		if (reset_camera || initialSecondBGCapture == 1) { // currentVideoTrack <=> video off
			// printf("sends camera BG to GPU\n");
			reset_camera = false;

			// makes a second video capture 
			if (initialSecondBGCapture == 0) {
				initialSecondBGCapture = 1;
			}
			else if (initialSecondBGCapture == 1) {
				initialSecondBGCapture = 2;
			}

			// current background memory
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

#if defined(var_camera_BG_subtr)
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

void pg_initCameraFrameTexture(Mat *camera_frame) {
	glEnable(GL_TEXTURE_RECTANGLE);

	// camera image
	if (!pg_camera_texture_texID) {
		glGenTextures(1, &pg_camera_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself

								   // current background
	if (!pg_camera_BG_texture_texID) {
		glGenTextures(1, &pg_camera_BG_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BG_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glGenerateMipmap(GL_TEXTURE_2D);

#if defined(var_camera_BG_subtr)
	// initial background
	if (!pg_camera_BGIni_texture_texID) {
		glGenTextures(1, &pg_camera_BGIni_texture_texID);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE, pg_camera_BGIni_texture_texID);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGB,            // Internal colour format to convert to
		camera_frame->cols,          // Image width  i.e. 640 for Kinect in standard mode
		camera_frame->rows,          // Image height i.e. 480 for Kinect in standard mode
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_BGR, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		camera_frame->data);        // The actual image data itself
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

void pg_openCameraCaptureAndLoadFrame(void) {
	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use
	printf("camera No %d nbWebcam %d\n", cameraNo, pg_webCams.size());
	if (cameraNo < 0 && -cameraNo - 1 < int(pg_webCams.size())) {
		// cv::CAP_FFMPEG or cv::CAP_IMAGES or cv::CAP_DSHOW.
		printf("Open webcam #%d: %s\n", -cameraNo - 1, pg_webCams[-cameraNo - 1]->cameraString.c_str());
		pg_webCam_capture.open(pg_webCams[-cameraNo - 1]->cameraID, cv::CAP_DSHOW);
		if (!pg_webCam_capture.isOpened()) {
			sprintf(ErrorStr, "Error: webcam ID #%d not opened!\n", pg_webCams[-cameraNo - 1]->cameraID); 
			ReportError(ErrorStr);
			pg_current_active_cameraNo = INT_MIN;
			pg_cameraCaptureIsOn = false;
		}
		else {
			pg_webCam_capture.set(CAP_PROP_FRAME_WIDTH, pg_webCams[-cameraNo - 1]->cameraWidth);
			pg_webCam_capture.set(CAP_PROP_FRAME_HEIGHT, pg_webCams[-cameraNo - 1]->cameraHeight);

			// initial webcam frame reading
			loadCameraFrame(true, cameraNo);
			pg_current_active_cameraNo = cameraNo;

			if (!pg_initializedWebcam) {
				pg_initWebcamParameters();
				pg_initializedWebcam = true;
			}
			pg_cameraCaptureIsOn = true;
		}
	}
	else if (cameraNo >= 0 && cameraNo < int(pg_IPCam_capture.size())) {
		printf("Open IPcam #%d: %s\n", cameraNo, pg_IPCam_capture_address[cameraNo].c_str());
		pg_IPCam_capture[cameraNo].open(pg_IPCam_capture_address[cameraNo]);
		if (!pg_IPCam_capture[cameraNo].isOpened()) {
			sprintf(ErrorStr, "Error: IPCam #%d (%s) not opened!\n", cameraNo, pg_IPCam_capture_address[cameraNo].c_str()); 
			ReportError(ErrorStr); 
			pg_current_active_cameraNo = INT_MIN;
			pg_cameraCaptureIsOn = false;
		}
		else {

			pg_IPCam_capture[cameraNo].set(CAP_PROP_FRAME_WIDTH, 1920);
			pg_IPCam_capture[cameraNo].set(CAP_PROP_FRAME_HEIGHT, 1080);

			// initial IPCam #0 frame reading
			loadCameraFrame(true, cameraNo);
			pg_current_active_cameraNo = cameraNo;
			pg_cameraCaptureIsOn = true;
		}
	}
}

void pg_releaseCameraCapture(void) {
	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use.
	if (pg_cameraCaptureIsOn) {
		if (pg_current_active_cameraNo < 0 && -pg_current_active_cameraNo - 1 < int(pg_webCams.size())) {
			pg_webCam_capture.release();
		}
		else if (pg_current_active_cameraNo >= 0 && pg_current_active_cameraNo < int(pg_IPCam_capture.size())) {
			pg_IPCam_capture[pg_current_active_cameraNo].release();
		}
	}
	pg_current_active_cameraNo = INT_MIN;
	pg_cameraCaptureIsOn = false;
}

void pg_initWebcamParameters(void) {
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
	//float window_ratio = float(workingWindow_width) / window_height;
	//const float ratio_16_9 = 16.0f / 9.0f;
	//if (window_ratio >= ratio_16_9 || ratio_16_9 - window_ratio < window_ratio - 4.0 / 3.0) {
	//	 //pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	//	 pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	//	 //pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	//	 pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 540);
	//	//pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	//}
	//else {
	//	pg_webCam_capture.set(CV_CAP_PROP_FRAME_WIDTH, 960);
	//	pg_webCam_capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	//}
	// higher values could be tried for width, fx 1280 x 720 (16:9) 960x720 (4:3)
	//pg_webCam_capture.set(CV_CAP_PROP_FPS, 60);

	CameraCurrent_exposure = (float)pg_webCam_capture.get(CAP_PROP_EXPOSURE);
	CameraCurrent_gain = (float)pg_webCam_capture.get(CAP_PROP_GAIN);
	CameraCurrent_brightness = (float)pg_webCam_capture.get(CAP_PROP_BRIGHTNESS);
	CameraCurrent_saturation = (float)pg_webCam_capture.get(CAP_PROP_SATURATION);
	CameraCurrent_contrast = (float)pg_webCam_capture.get(CAP_PROP_CONTRAST);
	//CameraCurrent_FPS = (float)pg_webCam_capture.get(CAP_PROP_FPS);
	CameraCurrent_focus = (float)pg_webCam_capture.get(CAP_PROP_FOCUS);
	CameraCurrent_gamma = (float)pg_webCam_capture.get(CAP_PROP_GAMMA);
	CameraCurrent_WB_B = (float)pg_webCam_capture.get(CAP_PROP_WHITE_BALANCE_BLUE_U);
	CameraCurrent_WB_R = (float)pg_webCam_capture.get(CAP_PROP_WHITE_BALANCE_RED_V);

	printf("Current Cam exposure   %.2f\n", CameraCurrent_exposure);
	printf("Current Cam gain       %.2f\n", CameraCurrent_gain);
	printf("Current Cam brightness %.2f\n", CameraCurrent_brightness);
	printf("Current Cam contrast   %.2f\n", CameraCurrent_contrast);
	printf("Current Cam FPS        %.2f\n", CameraCurrent_FPS);
	printf("Current Cam focus      %.2f\n", CameraCurrent_focus);
	printf("Current Cam gamma      %.2f\n", CameraCurrent_gamma);
	printf("Current Cam WB_B       %.2f\n", CameraCurrent_WB_B);
	printf("Current Cam WB_R       %.2f\n", CameraCurrent_WB_R);

	printf("Current Cam saturation %.2f\n", CameraCurrent_saturation);
	//printf("Current Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);

	if (*((float *)ScenarioVarPointers[_cameraExposure]) != CameraCurrent_exposure) {
		printf("cv VideoCapture set exposure new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraExposure]), CameraCurrent_exposure);
		//pg_webCam_capture.set(CAP_PROP_EXPOSURE, (*((float *)ScenarioVarPointers[_cameraExposure])));
		CameraCurrent_exposure = *((float *)ScenarioVarPointers[_cameraExposure]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraGain]) != CameraCurrent_gain) {
		printf("cv VideoCapture set gain new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraGain]), CameraCurrent_gain);
		pg_webCam_capture.set(CAP_PROP_GAIN, *((float *)ScenarioVarPointers[_cameraGain]));
		CameraCurrent_gain = *((float *)ScenarioVarPointers[_cameraGain]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraBrightness]) != CameraCurrent_brightness) {
		printf("cv VideoCapture set brightness new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraBrightness]), CameraCurrent_brightness);
		pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, *((float *)ScenarioVarPointers[_cameraBrightness]));
		CameraCurrent_brightness = *((float *)ScenarioVarPointers[_cameraBrightness]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraSaturation]) != CameraCurrent_saturation) {
		printf("cv VideoCapture set saturation new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraSaturation]), CameraCurrent_saturation);
		pg_webCam_capture.set(CAP_PROP_SATURATION, *((float *)ScenarioVarPointers[_cameraSaturation]));
		CameraCurrent_saturation = *((float *)ScenarioVarPointers[_cameraSaturation]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)ScenarioVarPointers[_cameraContrast]) != CameraCurrent_contrast) {
		printf("cv VideoCapture set contrast new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraContrast]), CameraCurrent_contrast);
		pg_webCam_capture.set(CAP_PROP_CONTRAST, *((float*)ScenarioVarPointers[_cameraContrast]));
		CameraCurrent_contrast = *((float*)ScenarioVarPointers[_cameraContrast]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)ScenarioVarPointers[_cameraGamma]) != CameraCurrent_gamma) {
		printf("cv VideoCapture set gamma new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraGamma]), CameraCurrent_gamma);
		pg_webCam_capture.set(CAP_PROP_GAMMA, *((float*)ScenarioVarPointers[_cameraGamma]));
		CameraCurrent_contrast = *((float*)ScenarioVarPointers[_cameraGamma]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraWB_R]) != CameraCurrent_WB_R
		&& *((float *)ScenarioVarPointers[_cameraWB_R]) >= 0) {
		printf("cv VideoCapture set wbR new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraWB_R]), CameraCurrent_WB_R);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, *((float *)ScenarioVarPointers[_cameraWB_R]));
		CameraCurrent_WB_R = *((float *)ScenarioVarPointers[_cameraWB_R]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float *)ScenarioVarPointers[_cameraWB_B]) != CameraCurrent_WB_B) {
		printf("cv VideoCapture set wbB new/current  %.2f / %.2f\n", *((float*)ScenarioVarPointers[_cameraWB_B]), CameraCurrent_WB_B);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, *((float *)ScenarioVarPointers[_cameraWB_B]));
		CameraCurrent_WB_B = *((float *)ScenarioVarPointers[_cameraWB_B]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}

	//printf("Initial Cam exposure   %.2f\n", CameraCurrent_exposure);
	//printf("Initial Cam gain       %.2f\n", CameraCurrent_gain);
	//printf("Initial Cam brightness %.2f\n", CameraCurrent_brightness);
	//printf("Initial Cam saturation %.2f\n", CameraCurrent_saturation);
	//printf("Initial Cam contrast   %.2f\n", CameraCurrent_contrast);
	//printf("Initial Cam FPS        %.2f\n", CameraCurrent_FPS);
	//printf("Initial Cam focus      %.2f\n", CameraCurrent_focus);
	//printf("Initial Cam gamma      %.2f\n", CameraCurrent_gamma);
	//printf("Initial Cam WB         %.2f %.2f\n", CameraCurrent_WB_B, CameraCurrent_WB_R);
}
#endif

void pg_initMovieFrameTexture(Mat *movie_frame) {
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


#ifdef WIN32
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam) {
#else
void* pg_initVideoMoviePlayback(void * lpParam) {
#endif
	string * fileName = (string *)lpParam;

	// film loading openCV
	sprintf(AuxString, "/movie_fileName %s",
		pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName.c_str());
	pg_movie_capture.open((pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file %s is not loaded!\n", (pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName).c_str());
		return NULL;
	}
	pg_MovieFrame_buffer_data.PhotoName = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName;

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	if (!pg_FirstMovieFrame.data) {
		printf("Movie frame %s not grabbed!\n", (*fileName).c_str());
		return NULL;
	}
	else {
		pg_FirstMovieFrameIsAvailable = true;
	}

	//  transfer to GPU
	pg_movie_frame_width = pg_FirstMovieFrame.cols;
	pg_movie_frame_height = pg_FirstMovieFrame.rows;
	pg_initMovieFrameTexture(&pg_FirstMovieFrame);

	printf("Movie frame to GPU %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
	is_movieLoading = false;
	return NULL;
}


void* pg_initVideoMoviePlayback_nonThreaded(string* fileName) {
	// printf("VideoPb Init movie playback and capture first frame non threaded\n");

	//printf("VideoPb Init movie playback and capture first frame %s\n",
	//	pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName.c_str());

	// film loading openCV
	sprintf(AuxString, "/movie_fileName %s",
		pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName.c_str());
	pg_movie_capture.open(pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName.c_str());
	
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] is not loaded!\n", (pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName).c_str());
		return NULL;
	}

	//printf("Opens %s\n", (cwd + videos_directory + movieFileName[currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] not loaded!\n", (cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName).c_str());
		return NULL;
	}
	pg_MovieFrame_buffer_data.PhotoName = cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName;

	// printf("Name %s Opened\n", pg_MovieFrame_buffer_data.PhotoName);

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	// texture loading
	if (!pg_FirstMovieFrame.data) {
		printf("Movie frame %s not grabbed!\n", (cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo]->videoFileName).c_str());
		return NULL;
	}
	pg_movie_frame_width = pg_FirstMovieFrame.cols;
	pg_movie_frame_height = pg_FirstMovieFrame.rows;
	pg_initMovieFrameTexture(&pg_FirstMovieFrame);
	pg_FirstMovieFrameIsAvailable = true;

	//printf("Movie frame initialisation %dx%d\n", pg_movie_frame_width, pg_movie_frame_height);
	pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
	is_movieLoading = false;

	//printf("Movie nb frames %d\n", pg_movie_status.get_nbFramesLeft());
	return NULL;
}

/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// PROCESSES PHOTO DIAPORAMA

void diaporama_random() {
	if (pg_nbCompressedImageDirs[pg_current_configuration_rank] > 0) {
		// goes to the first photo diaporama if it is not already selected and if there is one 
		if (photo_diaporama < 0 && pg_ImageDirectory[pg_current_configuration_rank] != "") {
			photo_diaporama = 0;
		}
		photo_diaporama = int(rand_0_1 * pg_nbCompressedImageDirs[pg_current_configuration_rank]) % pg_nbCompressedImageDirs[pg_current_configuration_rank];
		//printf("photo_diaporama %d\n", photo_diaporama);
		sprintf(AuxString, "/diaporama_shortName %03d", photo_diaporama);
		pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
		pg_launch_diaporama(0);
	}
}

void diaporama_slide(int slideNo) {
	if (slideNo >= 0 && photo_diaporama >= 0 && photo_diaporama < pg_nbCompressedImageDirs[pg_current_configuration_rank]) {
		pg_CurrentDiaporamaFile  = slideNo % pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
		pg_launch_diaporama(pg_CurrentDiaporamaFile);
	}
}

// initializes the blending of each image inside a folder
void pg_launch_diaporama(int slideNo) {
	//std::cout << "launch diaporama " << std::endl;
	// initialization
	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		pg_Photo_swap_buffer_data[indPhoto].blendStart = -1.0f;
		pg_Photo_swap_buffer_data[indPhoto].indSwappedPhoto = slideNo;
	}

	// loads the first images if it is not the case
	pg_CurrentDiaporamaFile = slideNo;
	if (photo_diaporama >= 0) {
		// launches blending of the first image
		pg_Photo_swap_buffer_data[0].blendStart = float(pg_CurrentClockTime);
		pg_Photo_swap_buffer_data[0].indSwappedPhoto = pg_CurrentDiaporamaFile;
		//std::cout << "layer " << 0 << " starts photo no "
		//	<< pg_CurrentDiaporamaFile << " at " << std::setprecision(4) << pg_CurrentClockTime << std::endl;
		for (int indPhoto = 1; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
			pg_Photo_swap_buffer_data[1].blendStart = -1.0f;
		}
	}
	printOglError(465);
}

bool pg_update_diaporama(void) {
		
	bool valRet = false;
	// no ongoing diaporama
	if (photo_diaporama < 0) {
		return valRet;
	}

	// avoids blocking the diaporama with two photos to be started
	// and not being triggered (because the ongoing blends have been
	// achieved while loading the new photos
	if (pg_Photo_swap_buffer_data[0].blendStart < 0.0f
		&& pg_Photo_swap_buffer_data[1].blendStart < 0.0f) {
		pg_Photo_swap_buffer_data[0].blendStart = float(pg_CurrentClockTime);
	}

	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		float * photoWeightPtr = pg_Photo_weight + indPhoto;

		// if blendStart is negative, there is no ongoing blending for the current photo
		// it will be started when the preceding one has reached
		// blendStart + photo_diaporama_fade + photo_diaporama_plateau
		if (pg_Photo_swap_buffer_data[indPhoto].blendStart < 0.0f) {
			// not blending
			// printf("Photo %d not blending\n",indPhoto);
			*photoWeightPtr = 0.0f;
			continue;
		}

		// ongoing blending
		double playingTime = pg_CurrentClockTime - pg_Photo_swap_buffer_data[indPhoto].blendStart;
		*photoWeightPtr = 0.0f;
		// printf("Photo %d playing time %.3f\n", indPhoto, playingTime);
		if (playingTime < (photo_diaporama_fade + photo_diaporama_plateau + photo_diaporama_fade)) {
			if (playingTime < photo_diaporama_fade) {
				if (photo_diaporama_fade > 0.0f) {
					*photoWeightPtr = float(playingTime) / photo_diaporama_fade;
				}
				else {
					*photoWeightPtr = 1.0f;
				}
			}
			else if (playingTime < photo_diaporama_fade + photo_diaporama_plateau) {
				*photoWeightPtr = 1.0f;
			}
			else {
				if (photo_diaporama_fade > 0.0f) {
					*photoWeightPtr = 1.0f - (float(playingTime) - photo_diaporama_fade - photo_diaporama_plateau)
						/ photo_diaporama_fade;
				}
				else {
					*photoWeightPtr = 1.0f;
				}
			}
			// printf("blend %d weight %.2f playing time %.1f\n",	indPhoto, photoWeight, playingTime );
		}

		// it is time to start the other photo because we reach
		// the end of the playing duration (photo_diaporama_fade+plateau+interval) of the current one
		if (playingTime >= (photo_diaporama_fade + photo_diaporama_plateau)
			&& playingTime < (photo_diaporama_fade + photo_diaporama_plateau + photo_diaporama_fade)
			&& pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart < 0) {
			pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].blendStart =
				pg_Photo_swap_buffer_data[indPhoto].blendStart + photo_diaporama_fade + photo_diaporama_plateau;
			pg_Photo_weight[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES] = 0.0f;
			// that displays the full blendped image
			nextFileIndexMemoryLoop(ascendingDiaporama);
			pg_Photo_swap_buffer_data[(indPhoto + 1) % PG_PHOTO_NB_TEXTURES].indSwappedPhoto
				= pg_CurrentDiaporamaFile;
			//std::cout << "layer " << (indPhoto + 1) % PG_PHOTO_NB_TEXTURES << " starts photo no "
			//	<< pg_CurrentDiaporamaFile << " at " << std::setprecision(4) << pg_CurrentClockTime << std::endl;

			valRet = true;
		}

		// it is time to stop the current photo because we reach
		// the end of the blending duration (photo_diaporama_fade+plateau+photo_diaporama_fade) of the current one
		if (playingTime >= (photo_diaporama_fade + photo_diaporama_plateau + photo_diaporama_fade)) {
			// stops blending the photo that has just finished
			pg_Photo_swap_buffer_data[indPhoto].blendStart = -1.0f;
			*photoWeightPtr = 0.0f;
			//std::cout << "layer " << indPhoto << " stops photo no "
			//	<< pg_Photo_swap_buffer_data[indPhoto].indSwappedPhoto 
			//	<< " at " << std::setprecision(4) << pg_CurrentClockTime << std::endl;

			valRet = true;
		}
	}
	return valRet;
}


/////////////////////////////////////////////////////////////////////////////////////////
// AFTER LOADING IN MEMORY, FOR DISPLAY: TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY LOOPS INSIDE THE DIRECTORY
void nextFileIndexMemoryLoop(bool ascending) {
	if (photo_diaporama < pg_nbCompressedImageDirs[pg_current_configuration_rank]
		&& pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama] > 0) {
		if (ascending) {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile + 1) % pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
		}
		else {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile - 1 + pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama])
				% pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
		}
	}
	else {
		while (pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama] == 0) {
			photo_diaporama++;
			pg_CurrentDiaporamaFile = 0;
		}
		if (ascending) {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile + 1) % pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
		}
		else {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile - 1 + pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama])
				% pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
		}
	}
}

/////////////////////////////////////////////
// 2D image loading
bool pg_load_compressed_photo(string *fileName, int width, int height, int indConfiguration) {
	FILE* fp;
	unsigned char header[124];

	/* try to open the file */
	//printf("load image %s\n", fileName);
	fp = fopen(fileName->c_str(), "rb");

	// Check for invalid input
	if (fp == NULL) {
		sprintf(ErrorStr, "Could not open or find the 2D image %s!", fileName->c_str()); ReportError(ErrorStr);
		return false;
	}

	/* verify the type of file */
	char filecode[5];
	fread(filecode, 1, 4, fp);
	filecode[4] = 0;
	if (strncmp(filecode, "DDS ", 4) != 0) {
		sprintf(ErrorStr, "Incorrect compressed file type %s!", filecode); ReportError(ErrorStr);
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
		sprintf(ErrorStr, "Unexpected dds diaporama image width %s %d instead of %d!",
			fileName->c_str(), width_from_header, width); ReportError(ErrorStr);
		//ratio_h = u_int(w) / width_from_header;
		//ratio_v = u_int(h) / height_from_header;
		return false;
	}
	if (height_from_header != u_int(height)) {
		sprintf(ErrorStr, "Unexpected dds diaporama image height %s %d instead of %d!",
			fileName->c_str(), height_from_header, height); ReportError(ErrorStr);
		//ratio_h = u_int(w) / width_from_header;
		//ratio_v = u_int(h) / height_from_header;
		return false;
	}

	// Check for image size
	if (mipMapCount > 1) {
		sprintf(ErrorStr, "Unexpected number of mipmaps %s %d!",
			fileName->c_str(), mipMapCount); ReportError(ErrorStr);
		return false;
	}

	// allocates memory
	if (imgPhotoCompressedBitmap[indConfiguration]) {
		free(imgPhotoCompressedBitmap[indConfiguration]);
		imgPhotoCompressedBitmap[indConfiguration] = NULL;
	}
	imgPhotoCompressedBitmap[indConfiguration]
		= (unsigned char*)malloc(linearSize * sizeof(unsigned char));
	fread(imgPhotoCompressedBitmap[indConfiguration], 1, linearSize, fp);
	//printf("linear size %d\n", linearSize);
	/* close the file pointer */
	fclose(fp);

	switch (fourCC)
	{
	case FOURCC_DXT1:
		imgPhotoCompressedFormat[indConfiguration] = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		//printf("compressed format DTX1\n");
		break;
	case FOURCC_DXT3:
		imgPhotoCompressedFormat[indConfiguration] = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		//printf("compressed format DTX3\n");
		break;
	case FOURCC_DXT5:
		imgPhotoCompressedFormat[indConfiguration] = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		//printf("compressed format DTX5\n");
		break;
	default:
		sprintf(ErrorStr, "Unexpected compression format %s %d!",
			fileName->c_str(), fourCC); ReportError(ErrorStr);
		free(imgPhotoCompressedBitmap[indConfiguration]);
		imgPhotoCompressedBitmap[indConfiguration] = NULL;
		return false;
	}
	//printf("Loaded photo %s compressed format %d\n", fileName, imgPhotoCompressedFormat[indConfiguration]);
	return true;
}
bool PhotoData::pg_loadPhoto(bool toBeInverted, int width, int height, bool verbose, int indConfiguration) {
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
		if (!pg_load_compressed_photo(&PhotoName, w, h, indConfiguration)) {
			PhotoName = "";
		}
		photoFormat = GL_RGB;
	}
	// JPEG FILE
	else if (extension == ".jpg") {
		// texture load through OpenCV
#ifndef OPENCV_3
		// threaded = false;
		imgPhotoBGRInit[indConfiguration] = cv::imread(string(PhotoName), CV_LOAD_IMAGE_UNCHANGED);   // Read the file
																			  // Check for invalid input
		if (!imgPhotoBGRInit[indConfiguration].data) {
			sprintf(ErrorStr, "Could not open or find the 2D image %s!", PhotoName); ReportError(ErrorStr);
			loadedInRAM = false;
			*(PhotoName) = 0;
			return false;
		}
		// Check for image size
		if ((w > 0 && imgPhotoBGRInit[indConfiguration].cols != w)
			|| (h > 0 && imgPhotoBGRInit[indConfiguration].rows != h)) {
			sprintf(ErrorStr, "Unexpected jpg diaporama image size %s %d/%d %d/%d!",
				PhotoName, imgPhotoBGRInit[indConfiguration].cols, w, imgPhotoBGRInit[indConfiguration].rows,
				h); ReportError(ErrorStr);
			loadedInRAM = false;
			*(PhotoName) = 0;
			return false;
		}
		w = imgPhotoBGRInit[indConfiguration].cols;
		h = imgPhotoBGRInit[indConfiguration].rows;
		photoFormat
			= ((imgPhotoBGRInit[indConfiguration].channels() == 4) ? GL_RGBA :
			((imgPhotoBGRInit[indConfiguration].channels() == 3) ? GL_RGB : GL_LUMINANCE));

		int colorTransform = (channelsPhoto == 4) ? CV_BGRA2RGBA :
			(imgPhotoBGRInit[indConfiguration].channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
		// int glColorType = (imgPhotoBGRInit[indConfiguration].channels() == 4) ? GL_RGBA : GL_RGB;
		if (imgPhotoBGRInit[indConfiguration].channels() >= 3) {
			cv::cvtColor(imgPhotoBGRInit[indConfiguration], imgPhotoRGB, colorTransform);
		}
		else {
			imgPhotoRGB = imgPhotoBGRInit[indConfiguration];
		}
#else
		imgPhotoBGRInit[indConfiguration] = cv::imread(PhotoName, cv::IMREAD_UNCHANGED);   // Read the file
																		   // Check for invalid input
		if (!imgPhotoBGRInit[indConfiguration].data) {
			sprintf(ErrorStr, "Could not open or find the 2D image %s!", PhotoName.c_str()); ReportError(ErrorStr);
			PhotoName = "";
			return false;
		}
		// Check for image size
		if ((w > 0 && imgPhotoBGRInit[indConfiguration].cols != w)
			|| (h > 0 && imgPhotoBGRInit[indConfiguration].rows != h)) {
			sprintf(ErrorStr, "Unexpected jpg image size %s %d/%d %d/%d!",
				PhotoName.c_str(), imgPhotoBGRInit[indConfiguration].cols, w, imgPhotoBGRInit[indConfiguration].rows,
				h); ReportError(ErrorStr);
			PhotoName = "";
			return false;
		}
		w = imgPhotoBGRInit[indConfiguration].cols;
		h = imgPhotoBGRInit[indConfiguration].rows;
		photoFormat
			= ((imgPhotoBGRInit[indConfiguration].channels() == 4) ? GL_RGBA :
			((imgPhotoBGRInit[indConfiguration].channels() == 3) ? GL_RGB : GL_LUMINANCE));

		int colorTransform
			= ((imgPhotoBGRInit[indConfiguration].channels() == 4) ? cv::COLOR_BGRA2RGBA :
			((imgPhotoBGRInit[indConfiguration].channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB));

		// int glColorType = (imgPhotoBGRInit[indConfiguration].channels() == 4) ? GL_RGBA : GL_RGB;
		if (imgPhotoBGRInit[indConfiguration].channels() >= 3) {
			cv::cvtColor(imgPhotoBGRInit[indConfiguration], imgPhotoRGB[indConfiguration], colorTransform);
		}
		else {
			imgPhotoRGB[indConfiguration] = imgPhotoBGRInit[indConfiguration];
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
bool ClipFramesData::pg_loadClipFrames(string *fileName, int width, int height, int indConfiguration) {
	int fileNameLength = fileName->length();
	string extension = fileName->substr(fileName->find_last_of("."));
	if (extension == ".dxt1" || extension == ".DXT1" || extension == ".DDS" || extension == ".dds") {
		if (pg_load_compressed_photo(fileName, width, height, indConfiguration)) {
			clipImgFormat = GL_RGB;
			return true;
		}
	}
	*fileName = "";
	return false;
}


//////////////////////////////////////////////////////////////////
// TRANSFERS BUFFER TO GPU

bool pg_toGPUCompressedPhoto(int width, int height, GLuint textureID, GLenum textureFilter, int indConfiguration) {
	unsigned int blockSize
		= (imgPhotoCompressedFormat[indConfiguration] == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)textureFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)textureFilter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glCompressedTexImage2D(GL_TEXTURE_2D,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		imgPhotoCompressedFormat[indConfiguration],  // compression format
		width,                 // Image width
		height,                 // Image height
		0,                 // Border width in pixels (can either be 1 or 0)
		size,              // Image data type
		imgPhotoCompressedBitmap[indConfiguration]);        // The actual image data itself

	free(imgPhotoCompressedBitmap[indConfiguration]);
	imgPhotoCompressedBitmap[indConfiguration] = NULL;
	printOglError(6);
	//printf("To GPU photo ID %d size %dx%d block size %d compressed format %d filter %d\n", textureID, width, height, blockSize, compressedPhotoFormat[indConfiguration], textureFilter);
	return false;
}

bool PhotoData::pg_toGPUPhoto(bool is_rectangle, GLint components, GLenum datatype, GLenum texturefilter, int indConfiguration) {
	/* // components GL_RGB8,
	// datatype GL_UNSIGNED_BYTE
	// format GL_RGB
	printf("format GL_RGB comp GL_RGB8 datatype GL_UNSIGNED_BYTE %d %d %d\n",
	int(components == GL_RGB8),
	int(datatype == GL_UNSIGNED_BYTE),
	int(format == GL_RGB)); */

	bool valret = true;

	printOglError(6);

	//printf("texture rect %d ID indCompressedImage %d compression %d\n", is_rectangle, texBuffID, imgPhotoCompressedFormat[indConfiguration]);

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle && !imgPhotoCompressedFormat[indConfiguration]) {
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
			imgPhotoRGB[indConfiguration].ptr());        // The actual image data itself
		printOglError(4);
		// printf("filename %s\n", PhotoName);
		valret = false;
	}
	else {
		if (!imgPhotoCompressedFormat[indConfiguration]) {
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
				imgPhotoRGB[indConfiguration].ptr());        // The actual image data itself
			printOglError(5);
			valret = false;
		}
		else {
			valret = pg_toGPUCompressedPhoto(w, h, texBuffID, texturefilter, indConfiguration);
		}
	}

	return valret;
}

bool ClipFramesData::pg_toGPUClipFrames(int w, int h, GLenum texturefilter, int indConfiguration) {
	return pg_toGPUCompressedPhoto(w, h, texBuffID, texturefilter, indConfiguration);
}

/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////
void  pg_initDiaporamas(void) {
	///////////////////////////////////////////////
	// NULL INITIALIZATIONS
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		pg_nbCompressedImagesPerFolder[indConfiguration].clear();
		pg_compressedImageData[indConfiguration].clear();
		pg_nbCompressedImages[indConfiguration] = 0;
		pg_nbCompressedImageDirs[indConfiguration] = 0;
		pg_compressedImageDirsNames[indConfiguration].clear();
		pg_compressedImageFilesNames[indConfiguration].clear();
	}
}
bool  pg_loadAllDiaporamas(void) {
	bool valret = true;

	printf("Load Diaporamas:\n");
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		std::cout << "    " << indConfiguration << ": ";

		////////////////////////////////////////////
		// LOADS IMAGES FROM FOLDERS
		std::cout << pg_ImageDirectory[indConfiguration] << " (";
		// scans a diaporama
		if (fs::is_directory(pg_ImageDirectory[indConfiguration])) {
			auto dirIter = fs::directory_iterator(pg_ImageDirectory[indConfiguration]);
			for (auto& dir_entry : dirIter)
			{
				if (dir_entry.is_directory()) {
					pg_AddDirectoryToDiaporamas(indConfiguration, dir_entry);
				}
			}
		}
		if (pg_nbCompressedImageDirs[indConfiguration] <= 0) {
			std::cout << "0 images) ";
			std::cout << std::endl;
		}
		else {
			std::cout << pg_nbCompressedImages[indConfiguration] << " images from " << pg_nbCompressedImageDirs[indConfiguration] << " folders), ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// INITIALIZES SWAP
	for (int indBuffer = 0; indBuffer < PG_PHOTO_NB_TEXTURES; indBuffer++) {
		pg_Photo_swap_buffer_data[indBuffer].swapping = false;
		pg_Photo_swap_buffer_data[indBuffer].indOldPhoto = indBuffer;
		pg_Photo_swap_buffer_data[indBuffer].indSwappedPhoto = indBuffer;
	}
	return valret;
}
void pg_AddDirectoryToDiaporamas(int indConfiguration, fs::directory_entry dir_entry) {
	auto subDirIter = fs::directory_iterator(dir_entry);
	++pg_nbCompressedImageDirs[indConfiguration];
	vector<string>dir_fileNames;
	vector<PhotoData*>dir_photoData;
	// scans image files inside a diaporama
	int nbImagesInDir = 0;
	for (auto& subdir_entry : subDirIter) {
		if (subdir_entry.is_regular_file()) {
			++pg_nbCompressedImages[indConfiguration];
			dir_photoData.push_back(new PhotoData());

			// allocates a texture ID for the image
			glGenTextures(1, &(dir_photoData[nbImagesInDir]->texBuffID));
			dir_photoData[nbImagesInDir]->IDallocated = true;

			// stores the photo file name
			dir_photoData[nbImagesInDir]->PhotoName = subdir_entry.path().string();

			// loads the images with a size that corresponds to the screen size
			dir_photoData[nbImagesInDir]->pg_loadPhoto(true, workingWindow_width_powerOf2, window_height_powerOf2, false, indConfiguration);

			// loads the compressed image into GPU
			dir_photoData[nbImagesInDir]->pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indConfiguration);
			// printf("texture ID indCompressedImage %d\n", dir_photoData[indCompressedImage]->texBuffID);

			printOglError(8);

			dir_fileNames.push_back(subdir_entry.path().string());
			++nbImagesInDir;
		}
	}
	pg_compressedImageFilesNames[indConfiguration].push_back(dir_fileNames);
	pg_compressedImageData[indConfiguration].push_back(dir_photoData);
	pg_compressedImageDirsNames[indConfiguration].push_back(dir_entry.path().string());
	pg_nbCompressedImagesPerFolder[indConfiguration].push_back(nbImagesInDir);
}
void pg_AddFilesToDiaporamas(int indConfiguration, fs::directory_entry dir_entry) {
	auto it = find(pg_compressedImageDirsNames[indConfiguration].begin(), pg_compressedImageDirsNames[indConfiguration].end(), 
		dir_entry.path().string());
	// If element was found 
	int dir_rank = -1;
	if (it != pg_compressedImageDirsNames[indConfiguration].end())
	{
		// calculating the index 
		dir_rank = it - pg_compressedImageDirsNames[indConfiguration].begin();
	}

	auto subDirIter = fs::directory_iterator(dir_entry);
	vector<string>dir_fileNames = pg_compressedImageFilesNames[indConfiguration][dir_rank];
	// scans image files inside a diaporama
	int nbImagesInDir = pg_nbCompressedImagesPerFolder[indConfiguration][dir_rank];
	for (auto& subdir_entry : subDirIter) {
		if (subdir_entry.is_regular_file()) {
			// file is not found
			if (std::find(dir_fileNames.begin(), dir_fileNames.end(), subdir_entry.path().string())
				== dir_fileNames.end()) {
				++pg_nbCompressedImages[indConfiguration];
				PhotoData* curPhotoData = new PhotoData();

				// allocates a texture ID for the image
				glGenTextures(1, &(curPhotoData->texBuffID));
				curPhotoData->IDallocated = true;

				// stores the photo file name
				curPhotoData->PhotoName = subdir_entry.path().string();

				// loads the images with a size that corresponds to the screen size
				curPhotoData->pg_loadPhoto(true, workingWindow_width_powerOf2, window_height_powerOf2, false, indConfiguration);

				// loads the compressed image into GPU
				curPhotoData->pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indConfiguration);
				// printf("texture ID indCompressedImage %d\n", dir_photoData[indCompressedImage]->texBuffID);

				pg_compressedImageData[indConfiguration][dir_rank].push_back(curPhotoData);

				printOglError(8);

				pg_compressedImageFilesNames[indConfiguration][dir_rank].push_back(subdir_entry.path().string());
				++nbImagesInDir;
			}
		}
	}
	pg_nbCompressedImagesPerFolder[indConfiguration][dir_rank] = nbImagesInDir;
}
int rankInVector(string *element, vector<string> *vec) {
	int count = 0;
	for (auto& it : *vec) {
		if (it == *element) {
			return count;
		}
		count++;
	}
	return -1;
}
bool pg_reloadAllDiaporamas(int curent_config, bool with_trace) {
	bool valret = true;

	if (with_trace) {
		printf("Reload all Diaporamas:\n");
		std::cout << "    " << curent_config << ": ";
	}

	////////////////////////////////////////////
	// ADDS NEW FOLDERS
	pg_addNewDiaporamas(curent_config, false);

	////////////////////////////////////////////
	// ADDS NEW FILES INSIDE FOLDERS
	int indCompressedImage = pg_nbCompressedImages[curent_config];
	int indCompressedImageDirs = pg_nbCompressedImageDirs[curent_config];
	if (with_trace) {
		std::cout << pg_ImageDirectory[curent_config] << " (";
	}
	// scans a diaporama
	if (fs::is_directory(pg_ImageDirectory[curent_config])) {
		auto dirIter = fs::directory_iterator(pg_ImageDirectory[curent_config]);
		for (auto& dir_entry : dirIter)
		{
			if (dir_entry.is_directory()) {
				// directory found in the vector of the directory ;
				string dirname = dir_entry.path().string();
				if (rankInVector(&dirname, &(pg_compressedImageDirsNames[curent_config])) != -1) {
					pg_AddFilesToDiaporamas(curent_config, dir_entry);
				}
			}
		}
	}
	if (with_trace) {
		if (pg_nbCompressedImageDirs[curent_config] <= 0) {
			std::cout << "0 images) ";
			std::cout << std::endl;
		}
		else {
			std::cout << pg_nbCompressedImages[curent_config] << " images from " << pg_nbCompressedImageDirs[curent_config] << " folders), ";
		}
		std::cout << std::endl;
	}

	return valret;
}
bool  pg_addNewDiaporamas(int curent_config, bool with_trace) {
	bool valret = true;

	if (with_trace) {
		printf("Add new Diaporamas:\n");
		std::cout << "    " << curent_config << ": ";
	}

	////////////////////////////////////////////
	// ADDS NEW FOLDERS
	int indCompressedImage = pg_nbCompressedImages[curent_config];
	int indCompressedImageDirs = pg_nbCompressedImageDirs[curent_config];
	if (with_trace) {
		std::cout << pg_ImageDirectory[curent_config] << " (";
	}
	// scans a diaporama
	if (fs::is_directory(pg_ImageDirectory[curent_config])) {
		auto dirIter = fs::directory_iterator(pg_ImageDirectory[curent_config]);
		for (auto& dir_entry : dirIter)
		{
			if (dir_entry.is_directory()) {
				// new directory, not found in the vector of the directory ;
				string dirname = dir_entry.path().string();
				if (rankInVector(&dirname, &(pg_compressedImageDirsNames[curent_config])) == -1) {
					pg_AddDirectoryToDiaporamas(curent_config, dir_entry);
				}
			}
		}
	}
	if (with_trace) {
		if (pg_nbCompressedImageDirs[curent_config] <= 0) {
			std::cout << "0 images) ";
			std::cout << std::endl;
		}
		else {
			std::cout << pg_nbCompressedImages[curent_config] << " images from " << pg_nbCompressedImageDirs[curent_config] << " folders), ";
		}
		std::cout << std::endl;
	}

	return valret;
}

std::string get_stem(const fs::path& p) { return (p.stem().string()); }
bool  pg_ReadInitalClipFramesTextures(void) {
	bool valret = true;
	for (int indConfiguration = 0; indConfiguration < pg_NbConfigurations; indConfiguration++) {
		///////////////////////////////////////////////
		// NULL INITIALIZATIONS
		pg_nbCompressedClipFramesPerFolder[indConfiguration] = NULL;
		pg_firstCompressedClipFramesInFolder[indConfiguration] = NULL;
		pg_ClipFrames_buffer_data[indConfiguration] = NULL;

		////////////////////////////////////////////
		// COUNTS IMAGES AND FOLDERS
		pg_nbCompressedClipFrames[indConfiguration] = 0;
		pg_nbClips[indConfiguration] = 0;

		std::cout << "Clip Directory: " << pg_ClipDirectory[indConfiguration] << std::endl;
		if (pg_ClipDirectory[indConfiguration] == "") {
			return false;
		}

		//std::cout << "Counting ClipFrames " << std::endl;
		if (fs::is_directory(pg_ClipDirectory[indConfiguration])) {
			auto dirIter = fs::directory_iterator(pg_ClipDirectory[indConfiguration]);
			for (auto& dir_entry : dirIter)
			{
				if (dir_entry.is_directory())
				{
					++pg_nbClips[indConfiguration];

					int nb_images_per_clip = 0;
					auto subDirIter = fs::directory_iterator(dir_entry);
					for (auto& subdir_entry : subDirIter)
					{
						if (subdir_entry.is_regular_file() && nb_images_per_clip < clip_max_length[indConfiguration])
						{
							++pg_nbCompressedClipFrames[indConfiguration];
							++nb_images_per_clip;
						}
					}
				}
			}
		}

		////////////////////////////////////////////
		// LOADS IMAGES FROM FOLDERS
		if (pg_nbClips[indConfiguration] <= 0) {
			std::cout << "Clip frames loading completed 0 files." << std::endl;
			return false;
		}
		//std::cout << "Clip frames counting "<< pg_nbCompressedClipFrames << " completed in " << pg_nbClips << " dirs." << std::endl;

		// allocates first frame indices and number of frames for each clip
		pg_nbCompressedClipFramesPerFolder[indConfiguration] = new int[pg_nbClips[indConfiguration]];
		pg_firstCompressedClipFramesInFolder[indConfiguration] = new int[pg_nbClips[indConfiguration]];
		for (int ind = 0; ind < pg_nbClips[indConfiguration]; ind++) {
			pg_nbCompressedClipFramesPerFolder[indConfiguration][ind] = 0;
			pg_firstCompressedClipFramesInFolder[indConfiguration][ind] = -1;
		}

		// allocates clip data
		pg_ClipFrames_buffer_data[indConfiguration] = new ClipFramesData * [pg_nbCompressedClipFrames[indConfiguration]];
		for (int ind = 0; ind < pg_nbCompressedClipFrames[indConfiguration]; ind++) {
			pg_ClipFrames_buffer_data[indConfiguration][ind] = new ClipFramesData();
		}

		int indCompressedClipFrames = 0;
		int indCompressedClipDirs = 0;
		std::cout << "Loading Clip Frames " << pg_nbCompressedClipFrames[indConfiguration] << " images from " << pg_nbClips[indConfiguration] << " folders" << std::endl;
		//pg_CurrentClipFramesDir = 0;
		//pg_CurrentClipFramesFile = 0;
		//std::cout << "Counting ClipFrames " << std::endl;
		if (fs::is_directory(pg_ClipDirectory[indConfiguration])) {
			auto dirIter = fs::directory_iterator(pg_ClipDirectory[indConfiguration]);
			for (auto& dir_entry : dirIter)
			{
				string dir_path = dir_entry.path().string();
				size_t last_separator = max(dir_path.find_last_of('/'), dir_path.find_last_of('\\'));
				string dir_name = dir_path;
				if (last_separator != string::npos) {
					dir_name = dir_path.substr(last_separator + 1);
				}
				std::cout << "dir " << "(" << indCompressedClipDirs << ")" << dir_path << ", name: " << dir_name << std::endl;
				pg_firstCompressedClipFramesInFolder[indConfiguration][indCompressedClipDirs] = indCompressedClipFrames;

				if (indCompressedClipDirs < pg_nbClips[indConfiguration] && dir_entry.is_directory())
				{
					int initialClipFrameIndex = indCompressedClipFrames;
					int nb_images_per_clip = 0;
					auto subDirIter = fs::directory_iterator(dir_entry);
					for (auto& subdir_entry : subDirIter)
					{
						if (indCompressedClipFrames < pg_nbCompressedClipFrames[indConfiguration] 
							&& subdir_entry.is_regular_file() && nb_images_per_clip < clip_max_length[indConfiguration])
						{
							if (pg_ClipFrames_buffer_data[indConfiguration][indCompressedClipFrames]->texBuffID == NULL_ID) {
								// allocates a texture ID for the image
								glGenTextures(1, &(pg_ClipFrames_buffer_data[indConfiguration][indCompressedClipFrames]->texBuffID));

								// loads the images with a size that corresponds to the screen size
								//printf("file %s\n", (char *)subdir_entry.path().string().c_str());
								//std::cout << "file " << subdir_entry.path().string() << std::endl;
								string dirName = subdir_entry.path().string();
								valret &= pg_ClipFrames_buffer_data[indConfiguration][indCompressedClipFrames]
									->pg_loadClipFrames(&dirName,
										clip_image_width[indConfiguration], clip_image_height[indConfiguration], indConfiguration);

								// loads the compressed image into GPU
								pg_ClipFrames_buffer_data[indConfiguration][indCompressedClipFrames]->pg_toGPUClipFrames(clip_image_width[indConfiguration], 
									clip_image_height[indConfiguration], GL_LINEAR, indConfiguration);
								//printf("texture ID dir %d indCompressedClipFrames %d texID %d\n", indCompressedClipDirs, indCompressedClipFrames, pg_ClipFrames_buffer_data[indCompressedClipFrames]->texBuffID);

								printOglError(8);
								++indCompressedClipFrames;
								++nb_images_per_clip;
							}
						}
					}
					pg_nbCompressedClipFramesPerFolder[indConfiguration][indCompressedClipDirs] = indCompressedClipFrames - initialClipFrameIndex;
					pg_clip_tracks[indConfiguration].push_back(clip_track(indCompressedClipDirs, indCompressedClipFrames - initialClipFrameIndex, dir_name));
					++indCompressedClipDirs;
				}
			}
		}
		std::cout << "Clip Frames loading completed " << pg_nbCompressedClipFrames[indConfiguration] << " frames." << std::endl;
		//for (int ind = 0; ind < pg_nbClips; ind++) {
		//	printf("Clip No %d, first frame %d Nb frames %d\n", ind + 1, pg_firstCompressedClipFramesInFolder[ind], pg_nbCompressedClipFramesPerFolder[ind]);
		//}

		// stores predefined cues if there are some
		fs::path dir(pg_ClipDirectory[indConfiguration]);
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
				sprintf(ErrorStr, "Error: cues file [%ls] not opened!", full_path.c_str()); ReportError(ErrorStr);
			}
			while (std::getline(cuesFile, line)) {
				stringstreamStoreLine(&sstream, &line);
				sstream >> clipNo;
				clipNo--;
				int cues[_NbMaxCues] = { -1 };
				if (_NbMaxCues >= 3 && clipNo < pg_nbClips[indConfiguration]) {
					sstream >> cues[0];
					sstream >> cues[1];
					sstream >> cues[2];

					for (int ind = 0; ind < 3; ind++) {
						if (cues[ind] >= 0) {
							pg_clip_tracks[indConfiguration][clipNo].set_cue(ind, cues[ind]);
							//printf("Cue for clip %d #%d at frame %d\n", clipNo, ind, cues[ind]);
						}
					}
				}
			}
		}
		else {
			sprintf(ErrorStr, "Error: cues file [%ls] not provided - automatic cues!", full_path.c_str()); ReportError(ErrorStr);
			for (int clipNo = 0; clipNo < pg_nbClips[indConfiguration]; clipNo++) {
				//printf("Clip No %d (%s), first frame %d Nb frames %d\n", clipNo + 1, pg_firstCompressedClipFramesInFolder[clipNo], pg_nbCompressedClipFramesPerFolder[clipNo]);
				//cues[ind] = rand_0_1 * pg_nbCompressedClipFramesPerFolder[ind]
				int cues[_NbMaxCues] = { -1 };
				cues[0] = 0;
				cues[1] = int(floor(0.33 * pg_nbCompressedClipFramesPerFolder[indConfiguration][clipNo])) % pg_nbCompressedClipFramesPerFolder[indConfiguration][clipNo];
				cues[2] = int(floor(0.66 * pg_nbCompressedClipFramesPerFolder[indConfiguration][clipNo])) % pg_nbCompressedClipFramesPerFolder[indConfiguration][clipNo];
				pg_clip_tracks[indConfiguration][clipNo].set_cue(0, 0);
				pg_clip_tracks[indConfiguration][clipNo].set_cue(1, cues[1]);
				pg_clip_tracks[indConfiguration][clipNo].set_cue(2, cues[2]);
				printf("Clip No %d (%s), first frame %d Nb frames %d cues (%d, %d, %d)\n",
					clipNo + 1, pg_clip_tracks[indConfiguration][clipNo].get_name().c_str(), pg_firstCompressedClipFramesInFolder[indConfiguration][clipNo], pg_nbCompressedClipFramesPerFolder[indConfiguration][clipNo],
					cues[0], cues[1], cues[2]);
			}
		}
	}

	return valret;
}
