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


#if defined (GN) || defined (MALAUSSENA)
//////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION OF THE TRANSITION TABLES FOR THE AUTOMATA
//////////////////////////////////////////////////////////////////////////////////////////

void pg_CATable_values(GLuint textureID, GLubyte * data_table, int width, int height) {
	GLubyte *ptr = data_table;

	//vec4 GOL_params[9]
	//=vec4[9](vec4(0,0,0,0),vec4(3,3,2,3),
	//  vec4(3,6,2,3),vec4(1,1,1,2),
	//  vec4(1,2,3,4),vec4(1,2,4,6),
	//  vec4(2,10,4,6),vec4(2,6,5,6),
	//  vec4(2,7,5,7));

	////////////////////////////////////////////
	// GAME OF LIFE FAMILY: 1 neutral + 8 variants
	// #0 line: nbStates/ encoding in sequence of output states the neutral automaton for 
	// each of the number of neighbor values (10 form 0 to 9)
	// #1-8 line: encoding in sequence of output states the 8 variants for 
	// each of the number of neighbor values (10 form 0 to 9)
	GLubyte transition_tableGOL[9 * 2 * 10] = {
		0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,

		0,0,0,1,0,0,0,0,0,0, 0,0,1,1,0,0,0,0,0,0,
		0,0,0,1,1,1,1,0,0,0, 0,0,1,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,0,0, 0,1,1,0,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,0,0, 0,0,0,1,1,0,0,0,0,0,

		0,1,1,0,0,0,0,0,0,0, 0,0,0,0,1,1,1,0,0,0,
		0,0,1,1,1,1,1,1,1,1, 0,0,0,0,1,1,1,0,0,0,
		0,0,1,1,1,1,1,0,0,0, 0,0,0,0,0,1,1,0,0,0,
		0,0,1,1,1,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0,

	};
	for (int indGOL = 0; indGOL < 9; indGOL++) {
		ptr[0] = 2;
		for (int ind = 1; ind < std::min(width, 2 * 10 + 1); ind++) {
			ptr[ind * 4] = transition_tableGOL[indGOL * 2 * 10 + ind - 1];
		}
		ptr += 4 * width;

	}

	////////////////////////////////////////////
	// TOTALISTIC FAMILY: 1 neutral + 8 variants
	// #9 line: nbStates/ encoding in sequence of output states the neutral automaton for 
	// each of the nb_states * nb_neighors transitions
	// #10-17 line: encoding in sequence of output states the 8 variants for 
	// each of the nb_states * nb_neighors (16 * 10) transitions

	// SubType 0: neutral
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = 0;
	}
	ptr += 4 * width;

	// SubType 1: CARS
	GLubyte transition_tableCARS[16 * 10] = {
		0,2,15,6,8,2,4,6,8,0, 0,0,0,0,0,0,0,0,0,0, 4,4,4,4,4,4,4,4,4,0, 0,0,0,0,0,0,0,0,0,0,
		0,6,6,6,6,6,6,6,6,0, 0,0,0,0,0,0,0,0,0,0, 8,8,8,8,8,8,8,8,8,0, 0,0,0,0,0,0,0,0,0,0,
		10,10,10,10,10,10,10,10,10,0, 0,0,0,0,0,0,0,0,0,0, 12,12,12,12,12,12,12,12,12,0, 0,0,0,0,0,0,0,0,0,0,
		14,14,14,14,14,14,14,14,14,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableCARS[ind - 1];
	}
	ptr += 4 * width;

	// SubType 2: EcoLiBra
	GLubyte transition_tableEcoLiBra[16 * 10] = {
		0,0,7,0,0,0,15,15,0,0, 0,0,0,0,0,0,0,0,0,0, 15,15,15,15,15,2,2,15,15,0,
		0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0, 12,12,12,12,12,12,12,12,12,0, 0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
		15,0,15,15,15,2,15,15,15,0
	};
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableEcoLiBra[ind - 1];
	}
	ptr += 4 * width;

	// SubType 3: Ladders
	GLubyte transition_tableLadders[16 * 10] = {
		0,6,5,0,0,2,15,5,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,8,7,15,0,15,0,0,
		0,0,6,0,0,0,0,0,3,0, 0,0,0,0,0,0,0,0,0,0, 8,0,0,0,0,0,0,0,0,0,
		8,4,2,5,6,0,0,0,0,0, 4,0,11,0,0,0,0,0,0,0, 0,0,0,0,0,0,15,4,0,0,
		0,8,0,15,5,0,0,0,0,0, 4,10,0,0,4,5,0,0,4,0, 0,8,8,0,0,12,4,6,0,0,
		0,0,0,10,2,10,6,6,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,9,0,11,3,0,0, 9,0,0,0,14,0,0,6
	};
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableLadders[ind - 1];
	}
	ptr += 4 * width;

	// SubType 4: Wire World
	GLubyte transition_tableWire[4 * 10] = {
		0,0,0,0,0,0,0,0,0,0, 2,2,2,2,2,2,2,2,2,0, 3,3,3,3,3,3,3,3,3,0, 3,1,1,3,3,3,3,3,3,3,
	};
	ptr[0] = 4;
	for (int ind = 1; ind < std::min(width, 4 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableWire[ind - 1];
	}
	ptr += 4 * width;

	// SubType 5: Busy Brain
	GLubyte transition_tableBusyBrain[3 * 10] = {
		0,0,1,2,0,2,2,2,2,0, 2,2,2,1,0,2,2,2,2, 0,0,0,0,0,1,2,2,1,2,
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 3 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableBusyBrain[ind - 1];
	}
	ptr += 4 * width;

	// SubType 6: Balloons
	GLubyte transition_tableBalloons[16 * 10] = {
		0,0,15,0,0,0,5,0,0,0,  0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0, 4,4,8,4,4,4,4,4,4,0,  5,5,5,5,5,7,7,9,11,
		0,2,2,2,2,2,2,2,2,2,  0,5,5,5,5,5,13,13,9,11, 0,8,8,10,8,8,8,8,8,8,0,
		2,2,2,2,2,9,13,9,11,0, 10,10,0,10,10,10,10,10,10,0,  4,14,14,14,14,14,14,14,11,0, 2,12,4,12,12,12,12,12,12,0,
		6,6,6,6,13,13,13,9,11,0, 14,14,14,12,14,14,14,14,14,0,  2,2,2,2,2,2,2,2,2
	};
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableBalloons[ind - 1];
	}
	ptr += 4 * width;

	// SubType 7: Ran Brain
	GLubyte transition_tableRanBrain[16 * 10] = {
		0,0,5,10,0,0,5,10,0,0, 0,0,5,10,0,0,0,0,15,0,
		0,0,0,0,0,15,15,0,0,0, 0,0,14,0,0,0,0,0,0,0, 0,0,4,0,0,0,0,0,0,0,
		2,6,2,6,2,6,2,6,2,0, 2,6,2,6,2,6,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
		0,0,12,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,12,0,0,
		0,2,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,14,7
	};
	ptr[0] = 16;
	for (int ind = 1; ind < std::min(width, 16 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableRanBrain[ind - 1];
	}
	ptr += 4 * width;

	// SubType 8: Brian's Brain
	GLubyte transition_tableBriansBrain[3 * 10] = {
		0,0,1,0,0,0,0,0,0,0, 2,2,2,2,2,2,2,2,2,2, 0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 3 * 10 + 1); ind++) {
		ptr[ind * 4] = transition_tableBriansBrain[ind - 1];
	}
	ptr += 4 * width;

	////////////////////////////////////////////
	// GENERATION FAMILY: 1 neutral + 19 variants -> 1 neutral + 9 variants
	// #18 line: nbStates/ encoding in sequence of output states the neutral automaton for 
	// each of the nb_states * nb_neighors transitions
	// #19-37 line: nbStates/ encoding in sequence of output states the 19 variants for 
	// each of the nb_states * nb_neighors transitions

	// SubType 0: neutral
#define nbStatesNeutral 8
	ptr[0] = nbStatesNeutral;
	for (int ind = 1; ind < std::min(width, nbStatesNeutral * 10 + 1); ind++) {
		ptr[ind * 4] = 0;
	}
	ptr += 4 * width;

	/*
	// SubType 1: TwoStates
#define nbStatesTwoStates 2
	GLubyte TwoStates[nbStatesTwoStates * 10] = {
		0,0,1,0,1,0,0,1,1,0, // TwoStates
		0,0,0,0,0,0,0,0,0,0, // TwoStates
	}; // TwoStates
	ptr[0] = nbStatesTwoStates;
	for (int ind = 1; ind < std::min(width, nbStatesTwoStates * 10 + 1); ind++) {
		ptr[ind * 4] = TwoStates[ind - 1];
	}
	ptr += 4 * width;
	*/

	// SubType 2 -> 1: Caterpillars
#define nbStatesCaterpillars 4
	GLubyte Caterpillars[nbStatesCaterpillars * 10] = {
		0,0,0,1,0,0,0,1,1,0, // Caterpillars
		2,1,1,2,1,1,1,1,2,2, // Caterpillars
		3,1,1,3,1,1,1,1,3,3, // Caterpillars
		0,1,1,0,1,1,1,1,0,0,
	}; // Caterpillars
	ptr[0] = nbStatesCaterpillars;
	for (int ind = 1; ind < std::min(width, nbStatesCaterpillars * 10 + 1); ind++) {
		ptr[ind * 4] = Caterpillars[ind - 1];
	}
	ptr += 4 * width;

	// SubType 3 -> 2: SoftFreeze
#define nbStatesSoftFreeze 6
	GLubyte SoftFreeze[nbStatesSoftFreeze * 10] = {
		0,0,0,1,0,0,0,0,1,0, // SoftFreeze
		2,1,2,1,1,1,2,2,1,2, // SoftFreeze
		3,1,3,1,1,1,3,3,1,3, // SoftFreeze
		4,1,4,1,1,1,4,4,1,4, // SoftFreeze
		5,1,5,1,1,1,5,5,1,5, // SoftFreeze
		0,1,0,1,1,1,0,0,1,0,
	}; // SoftFreeze
	ptr[0] = nbStatesSoftFreeze;
	for (int ind = 1; ind < std::min(width, nbStatesSoftFreeze * 10 + 1); ind++) {
		ptr[ind * 4] = SoftFreeze[ind - 1];
	}
	ptr += 4 * width;

	// SubType 4 -> 3: LOTE
#define nbStatesLOTE 6
	GLubyte LOTE[nbStatesLOTE * 10] = {
		0,0,0,1,0,0,0,0,0,0, // LOTE
		2,2,2,1,1,1,2,2,2,2, // LOTE
		3,3,3,1,1,1,3,3,3,3, // LOTE
		4,4,4,1,1,1,4,4,4,4, // LOTE
		5,5,5,1,1,1,5,5,5,5, // LOTE
		0,0,0,1,1,1,0,0,0,0,
	}; // LOTE
	ptr[0] = nbStatesLOTE;
	for (int ind = 1; ind < std::min(width, nbStatesLOTE * 10 + 1); ind++) {
		ptr[ind * 4] = LOTE[ind - 1];
	}
	ptr += 4 * width;


	// SubType 5 -> 4: MeterGuns
#define nbStatesMeterGuns 8
	GLubyte MeterGuns[nbStatesMeterGuns * 10] = {
		0,0,0,1,0,0,0,0,0,0, // MeterGuns
		1,1,1,2,1,1,1,1,1,2, // MeterGuns
		1,1,1,3,1,1,1,1,1,3, // MeterGuns
		1,1,1,4,1,1,1,1,1,4, // MeterGuns
		1,1,1,5,1,1,1,1,1,5, // MeterGuns
		1,1,1,6,1,1,1,1,1,6, // MeterGuns
		1,1,1,7,1,1,1,1,1,7, // MeterGuns
		1,1,1,0,1,1,1,1,1,0,
	}; // MeterGuns
	ptr[0] = nbStatesMeterGuns;
	for (int ind = 1; ind < std::min(width, nbStatesMeterGuns * 10 + 1); ind++) {
		ptr[ind * 4] = MeterGuns[ind - 1];
	}
	ptr += 4 * width;


	// SubType 6 -> 5: EbbFlow
#define nbStatesEbbFlow 18
	GLubyte EbbFlow[nbStatesEbbFlow * 10] = {
		0,0,0,1,0,0,1,0,0,0, // EbbFlow
		1,1,1,2,1,2,2,1,1,2, // EbbFlow
		1,1,1,3,1,3,3,1,1,3, // EbbFlow
		1,1,1,4,1,4,4,1,1,4, // EbbFlow
		1,1,1,5,1,5,5,1,1,5, // EbbFlow
		1,1,1,6,1,6,6,1,1,6, // EbbFlow
		1,1,1,7,1,7,7,1,1,7, // EbbFlow
		1,1,1,8,1,8,8,1,1,8, // EbbFlow
		1,1,1,9,1,9,9,1,1,9, // EbbFlow
		1,1,1,10,1,10,10,1,1,10, // EbbFlow
		1,1,1,11,1,11,11,1,1,11, // EbbFlow
		1,1,1,12,1,12,12,1,1,12, // EbbFlow
		1,1,1,13,1,13,13,1,1,13, // EbbFlow
		1,1,1,14,1,14,14,1,1,14, // EbbFlow
		1,1,1,15,1,15,15,1,1,15, // EbbFlow
		1,1,1,16,1,16,16,1,1,16, // EbbFlow
		1,1,1,17,1,17,17,1,1,17, // EbbFlow
		1,1,1,0,1,0,0,1,1,0,
	}; // EbbFlow
	ptr[0] = nbStatesEbbFlow;
	for (int ind = 1; ind < std::min(width, nbStatesEbbFlow * 10 + 1); ind++) {
		ptr[ind * 4] = EbbFlow[ind - 1];
	}
	ptr += 4 * width;


	// SubType 7 -> 6: EbbFlow2
#define nbStatesEbbFlow2 18
	GLubyte EbbFlow2[nbStatesEbbFlow2 * 10] = {
		0,0,0,1,0,0,0,1,0,0, // EbbFlow2
		1,1,1,2,1,2,1,2,1,2, // EbbFlow2
		1,1,1,3,1,3,1,3,1,3, // EbbFlow2
		1,1,1,4,1,4,1,4,1,4, // EbbFlow2
		1,1,1,5,1,5,1,5,1,5, // EbbFlow2
		1,1,1,6,1,6,1,6,1,6, // EbbFlow2
		1,1,1,7,1,7,1,7,1,7, // EbbFlow2
		1,1,1,8,1,8,1,8,1,8, // EbbFlow2
		1,1,1,9,1,9,1,9,1,9, // EbbFlow2
		1,1,1,10,1,10,1,10,1,10, // EbbFlow2
		1,1,1,11,1,11,1,11,1,11, // EbbFlow2
		1,1,1,12,1,12,1,12,1,12, // EbbFlow2
		1,1,1,13,1,13,1,13,1,13, // EbbFlow2
		1,1,1,14,1,14,1,14,1,14, // EbbFlow2
		1,1,1,15,1,15,1,15,1,15, // EbbFlow2
		1,1,1,16,1,16,1,16,1,16, // EbbFlow2
		1,1,1,17,1,17,1,17,1,17, // EbbFlow2
		1,1,1,0,1,0,1,0,1,0, // EbbFlow2
	}; // EbbFlow2
	ptr[0] = nbStatesEbbFlow2;
	for (int ind = 1; ind < std::min(width, nbStatesEbbFlow2 * 10 + 1); ind++) {
		ptr[ind * 4] = EbbFlow2[ind - 1];
	}
	ptr += 4 * width;


	// SubType 8 -> 7: SediMental
#define nbStatesSediMental 4
	GLubyte SediMental[nbStatesSediMental * 10] = {
		0,0,1,0,0,1,1,1,1,0, // SediMental
		2,2,2,2,1,1,1,1,1,2, // SediMental
		3,3,3,3,1,1,1,1,1,3, // SediMental
		0,0,0,0,1,1,1,1,1,0,
	}; // SediMental
	ptr[0] = nbStatesSediMental;
	for (int ind = 1; ind < std::min(width, nbStatesSediMental * 10 + 1); ind++) {
		ptr[ind * 4] = SediMental[ind - 1];
	}
	ptr += 4 * width;

/*
	// SubType 9: Brain6
#define nbStatesBrain6 3
	GLubyte Brain6[nbStatesBrain6 * 10] = {
		0,0,1,0,1,0,1,0,0,0, // Brain6
		2,2,2,2,2,2,1,2,2,2, // Brain6
		0,0,0,0,0,0,1,0,0,0, // Brain6
	}; // Brain64
	ptr[0] = nbStatesBrain6;
	for (int ind = 1; ind < std::min(width, nbStatesBrain6 * 10 + 1); ind++) {
		ptr[ind * 4] = Brain6[ind - 1];
	}
	ptr += 4 * width;


	// SubType 10: OrthoGo
#define nbStatesOrthoGo 4
	GLubyte OrthoGo[nbStatesOrthoGo * 10] = {
		0,0,1,0,0,0,0,0,0,0, // OrthoGo
		2,2,2,1,2,2,2,2,2,2, // OrthoGo
		3,3,3,1,3,3,3,3,3,3, // OrthoGo
	}; // OrthoGo
	ptr[0] = nbStatesOrthoGo;
	for (int ind = 1; ind < std::min(width, nbStatesOrthoGo * 10 + 1); ind++) {
		ptr[ind * 4] = OrthoGo[ind - 1];
	}
	ptr += 4 * width;


	// SubType 11: StarWars
#define nbStatesStarWars 4
	GLubyte StarWars[nbStatesStarWars * 10] = {
		0,0,1,0,0,0,0,0,0,0, // StarWars
		2,2,2,1,1,1,2,2,2,2, // StarWars
		3,3,3,1,1,1,3,3,3,3, // StarWars
		0,0,0,1,1,1,0,0,0,0,
	}; // StarWars
	ptr[0] = nbStatesStarWars;
	for (int ind = 1; ind < std::min(width, nbStatesStarWars * 10 + 1); ind++) {
		ptr[ind * 4] = StarWars[ind - 1];
	}
	ptr += 4 * width;


	// SubType 12: Transers
#define nbStatesTransers 5
	GLubyte Transers[nbStatesTransers * 10] = {
		0,0,1,0,0,0,1,0,0,0, // Transers
		2,2,2,1,1,1,2,2,2,2, // Transers
		3,3,3,1,1,1,3,3,3,3, // Transers
		4,4,4,1,1,1,4,4,4,4, // Transers
		0,0,0,1,1,1,0,0,0,0,
	}; // Transers
	ptr[0] = nbStatesTransers;
	for (int ind = 1; ind < std::min(width, nbStatesTransers * 10 + 1); ind++) {
		ptr[ind * 4] = Transers[ind - 1];
	}
	ptr += 4 * width;


	// SubType 13: Snake
#define nbStatesSnake 6
	GLubyte Snake[nbStatesSnake * 10] = {
		0,0,1,0,0,1,0,0,0,0, // Snake
		1,2,2,1,1,2,1,1,2,2, // Snake
		1,3,3,1,1,3,1,1,3,3, // Snake
		1,4,4,1,1,4,1,1,4,4, // Snake
		1,5,5,1,1,5,1,1,5,5, // Snake
		1,0,0,1,1,0,1,1,0,0,
	}; // Snake
	ptr[0] = nbStatesSnake;
	for (int ind = 1; ind < std::min(width, nbStatesSnake * 10 + 1); ind++) {
		ptr[ind * 4] = Snake[ind - 1];
	}
	ptr += 4 * width;


	// SubType 14: Sticks
#define nbStatesSticks 6
	GLubyte Sticks[nbStatesSticks * 10] = {
		0,0,1,0,0,0,0,0,0,0, // Sticks
		2,2,2,1,1,1,1,2,2,2, // Sticks
		3,3,3,1,1,1,1,3,3,3, // Sticks
		4,4,4,1,1,1,1,4,4,4, // Sticks
		5,5,5,1,1,1,1,5,5,5, // Sticks
		0,0,0,1,1,1,1,0,0,0,
	}; // Sticks
	ptr[0] = nbStatesSticks;
	for (int ind = 1; ind < std::min(width, nbStatesSticks * 10 + 1); ind++) {
		ptr[ind * 4] = Sticks[ind - 1];
	}
	ptr += 4 * width;


	// SubType 15: Transers2
#define nbStatesTransers2 6
	GLubyte Transers2[nbStatesTransers2 * 10] = {
		0,0,1,0,0,0,1,0,0,0, // Transers2
		1,2,2,1,1,1,2,2,2,2, // Transers2
		1,3,3,1,1,1,3,3,3,3, // Transers2
		1,4,4,1,1,1,4,4,4,4, // Transers2
		1,5,5,1,1,1,5,5,5,5, // Transers2
		1,0,0,1,1,1,0,0,0,0, // Transers2
	}; // Transers2
	ptr[0] = nbStatesTransers2;
	for (int ind = 1; ind < std::min(width, nbStatesTransers2 * 10 + 1); ind++) {
		ptr[ind * 4] = Transers2[ind - 1];
	}
	ptr += 4 * width;


	// SubType 16: Worms
#define nbStatesWorms 6
	GLubyte Worms[nbStatesWorms * 10] = {
		0,0,1,0,0,1,0,0,0,0, // Worms
		2,2,2,1,1,2,1,1,2,2, // Worms
		3,3,3,1,1,3,1,1,3,3, // Worms
		4,4,4,1,1,4,1,1,4,4, // Worms
		5,5,5,1,1,5,1,1,5,5, // Worms
		0,0,0,1,1,0,1,1,0,0,
	}; // Worms
	ptr[0] = nbStatesWorms;
	for (int ind = 1; ind < std::min(width, nbStatesWorms * 10 + 1); ind++) {
		ptr[ind * 4] = Worms[ind - 1];
	}
	ptr += 4 * width;


	// SubType 17: Cooties
#define nbStatesCooties 9
	GLubyte Cooties[nbStatesCooties * 10] = {
		0,0,1,0,0,0,0,0,0,0, // Cooties
		2,2,1,1,2,2,2,2,2,2, // Cooties
		3,3,1,1,3,3,3,3,3,3, // Cooties
		4,4,1,1,4,4,4,4,4,4, // Cooties
		5,5,1,1,5,5,5,5,5,5, // Cooties
		6,6,1,1,6,6,6,6,6,6, // Cooties
		7,7,1,1,7,7,7,7,7,7, // Cooties
		8,8,1,1,8,8,8,8,8,8, // Cooties
		0,0,1,1,0,0,0,0,0,0,
	}; // Cooties
	ptr[0] = nbStatesCooties;
	for (int ind = 1; ind < std::min(width, nbStatesCooties * 10 + 1); ind++) {
		ptr[ind * 4] = Cooties[ind - 1];
	}
	ptr += 4 * width;
*/
	// ***************** GENERATIVE NIGHTS
	// SubType 18 -> 8: Faders
#define nbStatesFaders 25
	GLubyte Faders[nbStatesFaders * 10] = {
		0,0,1,0,0,0,0,0,0,0, // Faders
		2,2,1,2,2,2,2,2,2,2, // Faders
		3,3,1,3,3,3,3,3,3,3, // Faders
		4,4,1,4,4,4,4,4,4,4, // Faders
		5,5,1,5,5,5,5,5,5,5, // Faders
		6,6,1,6,6,6,6,6,6,6, // Faders
		7,7,1,7,7,7,7,7,7,7, // Faders
		8,8,1,8,8,8,8,8,8,8, // Faders
		9,9,1,9,9,9,9,9,9,9, // Faders
		10,10,1,10,10,10,10,10,10,10, // Faders
		11,11,1,11,11,11,11,11,11,11, // Faders
		12,12,1,12,12,12,12,12,12,12, // Faders
		13,13,1,13,13,13,13,13,13,13, // Faders
		14,14,1,14,14,14,14,14,14,14, // Faders
		15,15,1,15,15,15,15,15,15,15, // Faders
		16,16,1,16,16,16,16,16,16,16, // Faders
		17,17,1,17,17,17,17,17,17,17, // Faders
		18,18,1,18,18,18,18,18,18,18, // Faders
		19,19,1,19,19,19,19,19,19,19, // Faders
		20,20,1,20,20,20,20,20,20,20, // Faders
		21,21,1,21,21,21,21,21,21,21, // Faders
		22,22,1,22,22,22,22,22,22,22, // Faders
		23,23,1,23,23,23,23,23,23,23, // Faders
		24,24,1,24,24,24,24,24,24,24, // Faders
		0,0,1,0,0,0,0,0,0,0,
	}; // Faders
	ptr[0] = nbStatesFaders;
	for (int ind = 1; ind < std::min(width, nbStatesFaders * 10 + 1); ind++) {
		ptr[ind * 4] = Faders[ind - 1];
	}
	ptr += 4 * width;


	// SubType 19 -> 9: Fireworks
#define nbStatesFireworks 21
	GLubyte Fireworks[nbStatesFireworks * 10] = {
		0,1,0,1,0,0,0,0,0,0, // Fireworks
		2,2,1,2,2,2,2,2,2,2, // Fireworks
		3,3,1,3,3,3,3,3,3,3, // Fireworks
		4,4,1,4,4,4,4,4,4,4, // Fireworks
		5,5,1,5,5,5,5,5,5,5, // Fireworks
		6,6,1,6,6,6,6,6,6,6, // Fireworks
		7,7,1,7,7,7,7,7,7,7, // Fireworks
		8,8,1,8,8,8,8,8,8,8, // Fireworks
		9,9,1,9,9,9,9,9,9,9, // Fireworks
		10,10,1,10,10,10,10,10,10,10, // Fireworks
		11,11,1,11,11,11,11,11,11,11, // Fireworks
		12,12,1,12,12,12,12,12,12,12, // Fireworks
		13,13,1,13,13,13,13,13,13,13, // Fireworks
		14,14,1,14,14,14,14,14,14,14, // Fireworks
		15,15,1,15,15,15,15,15,15,15, // Fireworks
		16,16,1,16,16,16,16,16,16,16, // Fireworks
		17,17,1,17,17,17,17,17,17,17, // Fireworks
		18,18,1,18,18,18,18,18,18,18, // Fireworks
		19,19,1,19,19,19,19,19,19,19, // Fireworks
		20,20,1,20,20,20,20,20,20,20, // Fireworks
		0,0,1,0,0,0,0,0,0,0,
	}; // Fireworks
	ptr[0] = nbStatesFireworks;
	for (int ind = 1; ind < std::min(width, nbStatesFireworks * 10 + 1); ind++) {
		ptr[ind * 4] = Fireworks[ind - 1];
	}
	ptr += 4 * width;


	////////////////////////////////////////////
	// GENERAL BINARY FAMILY Moore Neighborhood: 1 neutral + 7 variants -> 1 neutral + 5 variants
	// Example: Fallski
	// C48,NM,Sb255a,Babb189ab63a
	// 48 states 0-47
	// Moore neihborhood Order N,NE,E,SE,S,SW,W,NW
	// states are encoded: S_N + 2 * S_NE + 4 * S_E + 8 * S_SE ... + 128 * S_NW
	// 00000000 0 neighbor
	// 10000000 N neighbor
	// 01000000 NE neighbor
	// 192 = 00000011 W and NW neighbors
	// Survive b255a survival on no alive neighbors: 
	//                             1 x one    255 x zeros
	// Birth   abb189ab63a birth on a single N or  NE neighbor, or on W and NW neighbors:
	//                             0 1 1   189 x zeros   1   63 x zeros
	// Encoding of Survival and Birth
	// 256 0/1 digits encode 

	// #38 line: nbStates/ encoding 2 * 256 transitions (encoding of 8 neighbors' directions)
	// #39-45 line: nbStates/ encoding in sequence of output states the 7 variants for 
	// each of the nbStates/ encoding 2 * 256 transitions (encoding of 8 neighbors' directions)

	// SubType 0: neutral
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = 0;
	}
	ptr += 4 * width;

	// Subtype 1: Fallski
	GLubyte transition_tableFallski[256 * 2] = {
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 48;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableFallski[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 2: JustFriends
	GLubyte transition_tableJustFriends[256 * 2] = {
		0,1,1,1,1,1,1,0,1,1,1,0,1,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableJustFriends[ind - 1];
	}
	ptr += 4 * width;
	*/

	// ***************** GENERATIVE NIGHTS
	// Subtype 3 -> 2: LogicRule
	// C0,NM,S256a, B3ababb5abaab4ab3ab23ab16ab14ab15ab32ab62ab63a
	GLubyte transition_tableLogicRule[256 * 2] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		// 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableLogicRule[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 4: Meteorama
	GLubyte transition_tableMeteorama[256 * 2] = {
		0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,1,0,0,0,1,1,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,0,1,0,1,1,0,0,0,1,0,1,0,0,1,1,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0,1,1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,0,0,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0,1,1,1,0,0,0,1,0,0,0,0,1,0,0,1,0,0,1,1,0,1,0,1,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,
		0,0,0,0,0,1,0,1,1,1,1,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,1,1,1,0,0,0,1,1,0,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1,1,1,0,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,
	};
	ptr[0] = 24;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableMeteorama[ind - 1];
	}
	ptr += 4 * width;
	*/

	// Subtype 5 -> 3: Slugfest
	GLubyte transition_tableSlugfest[256 * 2] = {
		1,1,1,1,0,0,0,1,0,0,1,1,0,0,1,1,1,1,0,1,0,1,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1,0,0,1,0,1,1,0,1,0,1,1,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,1,1,0,1,0,1,1,1,0,0,0,0,1,0,0,1,0,1,0,0,1,1,0,1,0,1,0,0,1,1,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,1,1,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,0,1,0,1,1,0,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,0,1,0,
		0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,1,1,1,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,1,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,0,1,1,1,0,1,0,0,1,1,0,0,1,1,1,0,1,1,0,0,0,0,1,1,0,1,1,1,1,0,1,0,1,1,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0,0,1,1,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,
	};
	ptr[0] = 20;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableSlugfest[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 6 -> 4: Snowflakes
	GLubyte transition_tableSnowflakes[256 * 2] = {
		0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,1,0,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,1,0,1,1,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,1,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,1,0,0,0,0,1,1,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,1,0,0,1,1,1,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,1,0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,
	};
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableSnowflakes[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 7 -> 5: Springski
	GLubyte transition_tableSpringski[256 * 2] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 78;
	for (int ind = 1; ind < std::min(width, 256 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableSpringski[ind - 1];
	}
	ptr += 4 * width;

	////////////////////////////////////////////
	// GENERAL BINARY FAMILY von Neumann Neighborhood: 1 neutral + 3 variants

	// #46 line: nbStates/ encoding 2 * 16 transitions (encoding of 4 neighbors' directions)
	// #47-49 line: nbStates/ encoding in sequence of output states the 7 variants for 
	// each of the nbStates/ encoding 2 * 16 transitions (encoding of 4 neighbors' directions)

	// SubType 0: neutral
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 16 * 2 + 1); ind++) {
		ptr[ind * 4] = 0;
	}
	ptr += 4 * width;

	// Subtype 1: Banks
	GLubyte transition_tableBanks[16 * 2] = {
		1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,
		0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,
	};
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 16 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableBanks[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 2: FractalBeads
	GLubyte transition_tableFractalBeads[16 * 2] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
	};
	ptr[0] = 4;
	for (int ind = 1; ind < std::min(width, 16 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableFractalBeads[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 3: Sierpinski
	GLubyte transition_tableSierpinski[16 * 2] = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,
	};
	ptr[0] = 2;
	for (int ind = 1; ind < std::min(width, 16 * 2 + 1); ind++) {
		ptr[ind * 4] = transition_tableSierpinski[ind - 1];
	}
	ptr += 4 * width;


	////////////////////////////////////////////
	// NEUMANNN BINARY FAMILY : 1 neutral + 18 variants -> 1 neutral + 9 variants
	// Fredkin2 rule has the following definition: 2,01101001100101101001011001101001
	// The first digit, '2', tells the rule has 2 states (it's a 1 bit rule).
	// The second digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=0 will get the state 0.
	// The third digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=0,W=1 will get the state 1.
	// The fourth digit, '1', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=0 will get the state 1.
	// The fifth digit, '0', tells a cell in a configuration ME=0,N=0,E=0,S=1,W=1 will get the state 0.
	// . . .
	// binary rules are extended to ternary for a uniform rule system

	// #50 line: nbStates/ encoding 3 * 81 transitions (encoding of 4 neighbors' directions with 3 states)
	// #51-68 line: nbStates/ encoding in sequence of output states the 7 variants for 
	// each of the nbStates/ encoding 3 * 81 transitions (encoding of 4 neighbors' directions with 3 states)

	// printf("table offset %d\n" , (int)(ptr - data_table)/(4 * width) );

	// SubType 0: neutral
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = 0;
	}
	ptr += 4 * width;

	//    // Subtype 1: one state simple
	//    GLubyte transition_tableCrystal2[243] = {
	//1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	//    };
	//    ptr[  0 ] = 3;
	//    for( int ind = 1 ; ind < std::min( width , 243+1 ) ; ind++ ) {
	//     ptr[  ind * 4 ] = transition_tableCrystal2[ind-1];
	//    }
	//    ptr += 4 * width;

	/*
	// Subtype 1: Crystal2
	GLubyte transition_tableCrystal2[243] = {
		0,1,0,1,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableCrystal2[ind - 1];
	}
	ptr += 4 * width;
	*/

	// Subtype 2 -> 1: Fredkin2
	GLubyte transition_tableFredkin2[243] = {
		0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableFredkin2[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 3 -> 2: Aggregation
	GLubyte transition_tableAggregation[243] = {
		0,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,2,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,0,2,1,2,2,2,0,2,0,2,0,1,0,1,0,2,2,2,2,2,1,2,2,2,0,1,2,0,1,2,0,1,0,1,2,2,0,1,1,2,1,1,0,0,0,1,1,1,1,1,1,2,0,2,1,2,2,2,1,2,1,2,1,1,1,1,1,1,1,2,0,2,1,1,1,2,1,1
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableAggregation[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 4 -> 3: Birds
	GLubyte transition_tableBirds[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,2,2,2,2,1,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,2,1,0,2,0,0,0,2,0,0,0,0,0,0,1,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,2,2,0,0,2,0,2,2,0,0,0,2,0,0,0,0,2,0,2,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableBirds[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 5 -> 4: Colony
	GLubyte transition_tableColony[243] = {
		0,1,0,1,0,2,0,2,0,1,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,0,1,0,2,0,1,1,2,1,0,0,1,1,1,0,2,1,2,0,2,1,0,1,2,0,0,0,0,0,2,0,2,1,0,0,0,0,2,1,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,2,1,2,1,1,1,1,1,0,1,0,0,2,1,2,1,0,0,2,0,0,1,1,1,1,1,0,1,0,0,1,1,0,1,0,2,0,2,1,1,0,0,0,2,1,0,1,2,2,1,2,1,0,0,2,0,0,1,0,0,0,2,1,0,1,2,2,0,0,0,1,2,0,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableColony[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 6: Crystal3a
	GLubyte transition_tableCrystal3a[243] = {
		0,1,2,1,0,1,2,2,0,1,0,0,0,1,0,1,0,0,2,1,0,2,0,0,0,0,2,1,0,2,0,1,0,0,0,0,0,1,0,1,2,1,0,1,0,1,0,0,0,1,1,0,0,2,2,1,0,0,0,0,0,0,2,2,0,0,0,1,0,0,1,2,0,0,2,0,0,2,2,2,1,1,1,1,1,0,1,1,1,1,1,0,1,0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,2,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,1,2,2,2,2,2,2,2,2,0,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,0,0,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,0,2,0,0,2,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableCrystal3a[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 7: Crystal3b
	GLubyte transition_tableCrystal3b[243] = {
		0,1,2,1,0,0,2,0,0,1,0,0,0,1,1,0,0,2,2,0,0,0,1,2,0,2,0,1,0,0,0,2,0,0,2,1,0,2,1,2,2,1,0,2,1,0,1,2,0,2,0,1,2,2,2,0,0,0,2,1,0,1,0,0,0,2,2,1,2,1,0,2,0,2,0,1,1,2,0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,1,0,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,2,0,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,2,2,2,0,0,0,2,0,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,2,2,0,0,0,2,0,0,0,2,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableCrystal3b[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 8: Galaxy
	GLubyte transition_tableGalaxy[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,0,0,2,0,2,2,0,0,0,0,1,1,2,1,1,2,2,2,0,1,1,2,1,1,0,2,0,0,2,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,2,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,2,0,2,0,2,2,0,0,0,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,2,2,0,0,0,0,2,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableGalaxy[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 9: Greenberg
	GLubyte transition_tableGreenberg[243] = {
		0,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableGreenberg[ind - 1];
	}
	ptr += 4 * width;
	*/

	// Subtype 10 -> 5: Honeycomb
	GLubyte transition_tableHoneycomb[243] = {
		0,1,0,1,0,2,0,2,0,1,0,2,0,0,2,2,2,2,0,2,0,2,2,2,0,2,0,1,0,2,0,0,2,2,2,2,0,0,2,0,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,1,2,0,2,0,1,1,2,1,0,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,2,2,0,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,0,2,2,2,2,0,2,0,2,2,2,0,2,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableHoneycomb[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 11: Knitting
	GLubyte transition_tableKnitting[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,1,0,2,0,2,1,1,0,1,1,0,0,0,2,2,0,2,0,0,2,2,2,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,2,2,1,0,2,0,2,2,1,0,1,0,1,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,1,2,0,2,0,2,1,2,1,1,0,1,0,1,0,2,0,2,1,1,0,2,0,0,0,2,0,0,0,2,1,2,0,0,1,0,2,2,0,2,0,2,1,2,1,1,0,2,1,2,0,0,1,0,2,2,1,1,1,0,2,1,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableKnitting[ind - 1];
	}
	ptr += 4 * width;
	*/

	// Subtype 12 -> 6: Lake
	GLubyte transition_tableLake[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,0,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,0,2,0,2,1,0,0,0,1,0,0,0,2,2,0,2,0,0,2,2,2,0,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,2,2,0,0,2,0,2,2,0,0,0,0,0,1,2,1,1,0,2,0,2,1,1,0,1,0,0,0,0,0,2,0,2,0,0,0,2,0,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,2,0,2,0,0,0,2,0,0,0,0,0,0,2,2,0,2,0,2,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableLake[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 13: Plankton
	GLubyte transition_tablePlankton[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,0,0,0,2,0,2,0,0,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tablePlankton[ind - 1];
	}
	ptr += 4 * width;
	*/

	// Subtype 14 -> 7: Pond
	GLubyte transition_tablePond[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,1,0,1,1,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tablePond[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 15 -> 8: Strata
	GLubyte transition_tableStrata[243] = {
		0,0,0,0,0,0,2,0,0,1,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableStrata[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 16 -> 9: Tanks
	GLubyte transition_tableTanks[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,2,0,2,0,2,2,2,0,2,0,1,1,2,1,1,2,2,2,2,1,1,2,1,0,2,2,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,0,1,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,0,2,0,1,0,2,0,2,0,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,0,2,0,2,2,2,0,2,0,2,2,2,2,2,0,2,0,0,0,2,0,2,0,0,0,0,0,2,2,2,2,2,0,2,0,0,2,2,0,2,2,2,0,2,0,2,0,0,0,2,0,0,0,0,0,2,0,2,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableTanks[ind - 1];
	}
	ptr += 4 * width;

	/*
	// Subtype 17: Typhoon
	GLubyte transition_tableTyphoon[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,2,2,2,0,0,2,0,2,2,0,0,0,0,1,1,2,1,1,2,2,2,0,1,1,2,1,1,0,2,0,0,2,2,0,2,0,0,0,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,2,0,2,0,2,2,2,2,0,0,2,0,0,2,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableTyphoon[ind - 1];
	}
	ptr += 4 * width;

	// Subtype 18: Wave
	GLubyte transition_tableWave[243] = {
		0,1,0,1,1,2,0,2,0,1,1,2,1,1,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,1,0,2,0,2,1,1,0,1,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,1,1,2,1,0,2,2,2,2,1,0,2,0,0,0,2,0,0,2,2,2,2,0,0,2,0,2,1,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,2,0,2,0,0,0,2,0,0,0,0,2,2,2,2,0,0,2,0,2,2,0,0,0,2,0,0,0,0,2,0,2,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,2,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,2,2,0,2,2,2,0,2,0,2,2,2,2,2,2,2,2,2,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,2,2,2,0,2,0,0,0,0,0,2,0,0,0,0
	};
	ptr[0] = 3;
	for (int ind = 1; ind < std::min(width, 243 + 1); ind++) {
		ptr[ind * 4] = transition_tableWave[ind - 1];
	}
	ptr += 4 * width;
	*/

	glEnable(GL_TEXTURE_RECTANGLE);
	glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
		0,                 // Pyramid level (for mip-mapping) - 0 is the top level
		GL_RGBA8,            // Components: Internal colour format to convert to
		width,          // Image width
		height,          // Image heigh
		0,                 // Border width in pixels (can either be 1 or 0)
		GL_RGBA, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
		GL_UNSIGNED_BYTE,  // Image data type
		data_table);        // The actual image data itself

	printOglError(4);
}

#endif


