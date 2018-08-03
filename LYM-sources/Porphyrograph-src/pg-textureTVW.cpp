///////////////////////////////////////////////////////////////////
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
#if defined (TVW) || defined (CRITON)
PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES_TVW] = { PhotoSwapDataStruct() };
#else
PhotoSwapDataStruct pg_Photo_swap_buffer_data[PG_PHOTO_NB_TEXTURES] = { PhotoSwapDataStruct() };
#endif

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// PhotoDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
PhotoDataStruct **pg_Photo_buffer_dataTVW = NULL;
int pg_nbCompressedImageDirs = 0;
int *pg_nbCompressedImagesPerFolder = NULL;
int *pg_firstCompressedFileInFolder = NULL;
int pg_nbCompressedImages = 0;
#if defined (TVW) || defined (CRITON)
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

bool is_substring_index(std::string str, int ind) {
	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_substring_index(char * charstr, int ind) {
	std::string str(charstr);

	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory and the name contains the integer 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode) && is_substring_index(dirp->d_name, inddir)) { // the file is a directory 
																				 // and contains the integer substring
		return true;
	}
	return false;
}

bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

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
	if (dp == NULL)
	{
		std::cout << "is_subdir_subfile_index 1 Error(" << errno << ") opening " << *dirpath << std::endl;
		return NULL;
	}
	// std::cout << std::endl << "dir to get files of: " + dirpath << std::endl;
	struct dirent *dirp;
	while ((dirp = readdir(dp))) {
		if (is_subdir_index(dirp, dirpath, inddir)) {
			string subdirpath(*dirpath + "/" + dirp->d_name);
			DIR *subdp = opendir(subdirpath.c_str());
			if (subdp == NULL)
			{
				std::cout << "is_subdir_subfile_index 2 Error(" << errno << ") opening " << subdirpath << std::endl;
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
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ON THE DISK TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY JUMPS TO NEXT DIRECTORY IF ALL IMAGES ARE USED
// USED FOR LOADING ALL IMAGES
// STOPS AT THE LAST DIRECTORY
string * nextFileIndexDiskNoLoop(string *dirpath, int *currentDirIndex, int *currentFileIndex,
	int maxFilesPerFolder) {
	// printf("dir path %s cur dir index %d cur file index %d\n", dirpath->c_str(), *currentDirIndex, *currentFileIndex);
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
	return NULL;
}


/////////////////////////////////////////////
// 2D image loading
bool PhotoDataStruct::pg_loadPhoto(bool toBeInverted, int width,
	int height, bool verbose) {
	bool valret = true;
	int flieNameLenght = strlen(PhotoName);
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
	if (strcmp(extension, "dxt1") == 0 || strcmp(extension, "DXT1") == 0 || strcmp(extension, ".DDS") == 0 || strcmp(extension, ".dds") == 0 ) {
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

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);

		// Check for image size
		if ((w > 0 && width != w)
			|| (h > 0 && height != h)) {
			sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
				PhotoName, width, w, height,
				h); ReportError(ErrorStr);
			*(PhotoName) = 0;
			return false;
		}
		h = height;
		w = width;
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
			break;
		case FOURCC_DXT3:
			compressedFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			compressedFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
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
				sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
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
			sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
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

bool PhotoDataStruct::pg_toGPUPhoto( bool is_rectangle,
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
		unsigned int offset = 0;
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

#if defined (TVW) || defined (CRITON)
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
		ptr[indImage] = strrchr(pg_Photo_buffer_dataTVW[indImage]->PhotoName, '/');
		ptr[indImage] = (ptr[indImage] ? ptr[indImage] + 1 : pg_Photo_buffer_dataTVW[indImage]->PhotoName);
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
	if (pg_Photo_buffer_dataTVW[indcomprImage]->texBuffID == NULL_ID) {
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
		char * ptr1 = strrchr(pg_Photo_buffer_dataTVW[indcomprImage]->PhotoName, '/');
		char * ptr2 = strrchr(pg_Photo_buffer_dataTVW[indOld]->PhotoName, '/');
		printf("Swap image (%s replaces %s layer %d (dur %.2f))\n",
			(ptr1 ? ptr1 + 1 : pg_Photo_buffer_dataTVW[indcomprImage]->PhotoName),
			(ptr2 ? ptr2 + 1 : pg_Photo_buffer_dataTVW[indOld]->PhotoName),
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
	return false;
}

/////////////////////////////////////////////////////////
// INITIAL IMAGE UPLOADING
/////////////////////////////////////////////////////////
bool  pg_ReadInitalImageTextures(int ind_dir, int nbImages, int nbFolders, int maxFilesPerFolder) {
// printf("Image dir %d # %d dur %.2f\n" , dir_index, file_index, duration);
	std::string * fileName;
	bool valret = true;

	// assigns standard weights to each image buffer
	for (int ind = 0; ind < PG_PHOTO_NB_TEXTURES_TVW; ind++) {
		pg_Photo_weightTVW[ind] = 1.0f / PG_PHOTO_NB_TEXTURES_TVW;
	}

	// loads all the associated masks
	for (int indBuffer = 0; indBuffer < PG_PHOTO_NB_TEXTURES_TVW / 3; indBuffer++) {
		if (*(pg_Photo_mask_buffer_data[indBuffer].PhotoName) == 0) {
			if (!pg_Photo_mask_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(pg_Photo_mask_buffer_data[indBuffer].texBuffID));
				pg_Photo_mask_buffer_data[indBuffer].IDallocated = true;

				int indDir = 0;
				int indFile = indBuffer;
				if ((fileName = nextFileIndexDiskLoop(&MaskDir, &indDir, &indFile))) {
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
	ImageDir = "Data/TVW-data/images/TVW-wandering";
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;
	if (nbImages <= 0 || nbFolders <= 0) {
		pg_nbCompressedImages = 0;
		pg_nbCompressedImageDirs = 0;
		std::cout << "Counting Terrains Vagues Images " << std::endl;
		while ((fileName
			= nextFileIndexDiskNoLoop(&ImageDir,
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
	pg_nbCompressedImagesPerFolder = NULL;
	pg_firstCompressedFileInFolder = NULL;
	pg_nbCompressedImagesPerFolder = new int[pg_nbCompressedImageDirs];
	pg_firstCompressedFileInFolder = new int[pg_nbCompressedImageDirs];
	pg_Photo_buffer_dataTVW = new PhotoDataStruct *[pg_nbCompressedImages];
	for (int ind = 0; ind < pg_nbCompressedImages; ind++) {
		pg_Photo_buffer_dataTVW[ind] = new PhotoDataStruct();
	}
	for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
		pg_nbCompressedImagesPerFolder[ind] = 0;
		pg_firstCompressedFileInFolder[ind] = -1;
	}
	pg_CurrentDiaporamaDir = 0;
	pg_CurrentDiaporamaFile = 0;
	int indCompressedImage = 0;
	std::cout << "Loading Terrains Vagues " << pg_nbCompressedImages << " images from " << pg_nbCompressedImageDirs << " folders" << std::endl;
	if (pg_nbCompressedImageDirs != NbTextChapters) {
		printf("Incoherent text and image database!");
		exit(0);
	}
	while ((fileName
		= nextFileIndexDiskNoLoop(&ImageDir,
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
		if (*(pg_Photo_buffer_dataTVW[indCompressedImage]->PhotoName) == 0) {
			if (!pg_Photo_buffer_dataTVW[indCompressedImage]->IDallocated) {
				glGenTextures(1, &(pg_Photo_buffer_dataTVW[indCompressedImage]->texBuffID));
				pg_Photo_buffer_dataTVW[indCompressedImage]->IDallocated = true;

				strcpy(pg_Photo_buffer_dataTVW[indCompressedImage]->PhotoName,
					fileName->c_str());
				valret &= pg_Photo_buffer_dataTVW[indCompressedImage]->pg_loadPhoto(
					true, leftWindowWidth, leftWindowWidth, false);
				delete fileName;
				fileName = NULL;

				pg_Photo_buffer_dataTVW[indCompressedImage]->pg_toGPUPhoto(false,
					GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR);

				printOglError(8);
			}
		}
		indCompressedImage++;
	}
	std::cout << "Terrains Vagues Images loading completed " << pg_nbCompressedImages << " files." << std::endl;
	std::cout << "folder";
	for (int ind = 0; ind < pg_nbCompressedImageDirs; ind++) {
		std::cout << " " << ind << "/" << pg_nbCompressedImagesPerFolder[ind] << "/" << pg_firstCompressedFileInFolder[ind];
	}
	std::cout << std::endl;

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


//////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TRANSITION TABLES FOR THE AUTOMATA
//////////////////////////////////////////////////////////////////////////////////////////

#if defined (GN)
#include "pg_CATable_GN.cpp"
#endif

#if defined (MALAUSSENA)
#include "pg_CATable_Malaussena.cpp"
#endif


