/*! \file pg-light.cpp
 *
 *
 *     File pg-light.cpp
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
 /* based on ENTEC source code */

#include "pg-all_include.h"

// movie playing
int currentlyPlaying_movieNo = -1;
float current_extern_movieNo = -1;

// movie soundtrack passes over an onset or a peak before next frame
bool pg_movie_sound_onset = false;
bool pg_movie_sound_peak = false;

// track loading 
int currentClipArtTrack = 0;

bool ascendingDiaporama = true;

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// // DIAPORAMAS
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
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
	vector<PhotoData>dir_photoData;
	// scans image files inside a diaporama
	int nbImagesInDir = 0;
	//printf("Dir photo %s\n", dir_entry.path().string().c_str());
	for (auto& subdir_entry : subDirIter) {
		if (subdir_entry.is_regular_file()) {
			PhotoData photoData;
			++pg_nbCompressedImages[indConfiguration];
			dir_photoData.push_back(photoData);

			// allocates a texture ID for the image
			glGenTextures(1, &(dir_photoData[nbImagesInDir].texBuffID));
			dir_photoData[nbImagesInDir].IDallocated = true;

			// stores the photo file name
			dir_photoData[nbImagesInDir].PhotoName = subdir_entry.path().string();
			//printf("Subdir photo %s ID %d\n", subdir_entry.path().string().c_str(), dir_photoData[nbImagesInDir].texBuffID);

			// loads the images with a size that corresponds to the screen size
			dir_photoData[nbImagesInDir].pg_loadPhoto(true, workingWindow_width_powerOf2, window_height_powerOf2, false, indConfiguration);

			// loads the compressed image into GPU
			dir_photoData[nbImagesInDir].pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indConfiguration);

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
				PhotoData curPhotoData;

				// allocates a texture ID for the image
				glGenTextures(1, &(curPhotoData.texBuffID));
				curPhotoData.IDallocated = true;

				// stores the photo file name
				curPhotoData.PhotoName = subdir_entry.path().string();

				// loads the images with a size that corresponds to the screen size
				curPhotoData.pg_loadPhoto(true, workingWindow_width_powerOf2, window_height_powerOf2, false, indConfiguration);

				// loads the compressed image into GPU
				curPhotoData.pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indConfiguration);
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
int rankInVector(string* element, vector<string>* vec) {
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
		pg_CurrentDiaporamaFile = slideNo % pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
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
	pg_CurrentDiaporamaFile = slideNo % pg_nbCompressedImagesPerFolder[pg_current_configuration_rank][photo_diaporama];
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
		float* photoWeightPtr = pg_Photo_weight + indPhoto;

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

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// // CAMERA
///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_initCameraFrameTexture(Mat* camera_frame) {
	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		return;
	}

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
}

void pg_openCameraCaptureAndLoadFrame(void) {
	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		return;
	}

	// video loading openCV
	//Allocates and initializes cvCapture structure
	// for reading a video stream from the camera.
	//Index of camera is -1 since only one camera
	// connected to the computer or it does not
	// matter what camera to use
	printf("camera No %d nbWebcam %d\n", cameraNo, pg_webCams.size());
	if (cameraNo < 0 && -cameraNo - 1 < int(pg_webCams.size())) {
		// cv::CAP_FFMPEG or cv::CAP_IMAGES or cv::CAP_DSHOW.
		printf("Open webcam #%d: %s\n", -cameraNo - 1, pg_webCams[-cameraNo - 1].cameraString.c_str());
		pg_webCam_capture.open(pg_webCams[-cameraNo - 1].cameraID, cv::CAP_DSHOW);
		if (!pg_webCam_capture.isOpened()) {
			sprintf(ErrorStr, "Error: webcam ID #%d not opened!\n", pg_webCams[-cameraNo - 1].cameraID);
			ReportError(ErrorStr);
			pg_current_active_cameraNo = INT_MIN;
			pg_cameraCaptureIsOn = false;
		}
		else {
			pg_webCam_capture.set(CAP_PROP_FRAME_WIDTH, pg_webCams[-cameraNo - 1].cameraWidth);
			pg_webCam_capture.set(CAP_PROP_FRAME_HEIGHT, pg_webCams[-cameraNo - 1].cameraHeight);

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
	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		return;
	}

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
	if (!pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		return;
	}

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

	if (*((float*)pg_FullScenarioVarPointers[_cameraExposure]) != CameraCurrent_exposure) {
		printf("cv VideoCapture set exposure new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraExposure]), CameraCurrent_exposure);
		//pg_webCam_capture.set(CAP_PROP_EXPOSURE, (*((float *)pg_FullScenarioVarPointers[_cameraExposure])));
		CameraCurrent_exposure = *((float*)pg_FullScenarioVarPointers[_cameraExposure]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraGain]) != CameraCurrent_gain) {
		printf("cv VideoCapture set gain new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraGain]), CameraCurrent_gain);
		pg_webCam_capture.set(CAP_PROP_GAIN, *((float*)pg_FullScenarioVarPointers[_cameraGain]));
		CameraCurrent_gain = *((float*)pg_FullScenarioVarPointers[_cameraGain]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraBrightness]) != CameraCurrent_brightness) {
		printf("cv VideoCapture set brightness new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraBrightness]), CameraCurrent_brightness);
		pg_webCam_capture.set(CAP_PROP_BRIGHTNESS, *((float*)pg_FullScenarioVarPointers[_cameraBrightness]));
		CameraCurrent_brightness = *((float*)pg_FullScenarioVarPointers[_cameraBrightness]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraSaturation]) != CameraCurrent_saturation) {
		printf("cv VideoCapture set saturation new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraSaturation]), CameraCurrent_saturation);
		pg_webCam_capture.set(CAP_PROP_SATURATION, *((float*)pg_FullScenarioVarPointers[_cameraSaturation]));
		CameraCurrent_saturation = *((float*)pg_FullScenarioVarPointers[_cameraSaturation]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraContrast]) != CameraCurrent_contrast) {
		printf("cv VideoCapture set contrast new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraContrast]), CameraCurrent_contrast);
		pg_webCam_capture.set(CAP_PROP_CONTRAST, *((float*)pg_FullScenarioVarPointers[_cameraContrast]));
		CameraCurrent_contrast = *((float*)pg_FullScenarioVarPointers[_cameraContrast]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraGamma]) != CameraCurrent_gamma) {
		printf("cv VideoCapture set gamma new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraGamma]), CameraCurrent_gamma);
		pg_webCam_capture.set(CAP_PROP_GAMMA, *((float*)pg_FullScenarioVarPointers[_cameraGamma]));
		CameraCurrent_contrast = *((float*)pg_FullScenarioVarPointers[_cameraGamma]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraWB_R]) != CameraCurrent_WB_R
		&& *((float*)pg_FullScenarioVarPointers[_cameraWB_R]) >= 0) {
		printf("cv VideoCapture set wbR new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraWB_R]), CameraCurrent_WB_R);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_RED_V, *((float*)pg_FullScenarioVarPointers[_cameraWB_R]));
		CameraCurrent_WB_R = *((float*)pg_FullScenarioVarPointers[_cameraWB_R]);
		pg_LastCameraParameterChange_Frame = pg_FrameNo;
	}
	if (*((float*)pg_FullScenarioVarPointers[_cameraWB_B]) != CameraCurrent_WB_B) {
		printf("cv VideoCapture set wbB new/current  %.2f / %.2f\n", *((float*)pg_FullScenarioVarPointers[_cameraWB_B]), CameraCurrent_WB_B);
		pg_webCam_capture.set(CAP_PROP_WHITE_BALANCE_BLUE_U, *((float*)pg_FullScenarioVarPointers[_cameraWB_B]));
		CameraCurrent_WB_B = *((float*)pg_FullScenarioVarPointers[_cameraWB_B]);
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

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// // MOVIES
///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVIE STATUS MANAGEMENT WHILE STREAMING
media_status::media_status() {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		nbFramesLeft = 0;
		initialTime = 0.;
		initialMediaNbFrames = 0;

		pg_movie_sound_onset = false;
		pg_movie_sound_peak = false;
		currentMovieSoundPeakIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
// number frames left until the end from current frame
const int media_status::get_nbFramesLeft() {
	return nbFramesLeft;
}
// number of frames read from the beginning of the movie
const int media_status::get_nbFramesRead() {
	return initialMediaNbFrames - nbFramesLeft;
}
// current time when movie started
const double media_status::get_initialTime() {
	return initialTime;
}
// initial total number of frames in the movie
const int media_status::get_initialNbFrames() {
	return initialMediaNbFrames;
}
// sets the movie at a position from beginning
const void media_status::set_position(int nb_frames_from_beginning) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (nb_frames_from_beginning <= 0) {
			reset_movie(initialMediaNbFrames);
			return;
		}
		if (nb_frames_from_beginning >= initialMediaNbFrames) {
			nb_frames_from_beginning = initialMediaNbFrames - 1;
		}
		nbFramesLeft = initialMediaNbFrames - nb_frames_from_beginning;
		initialTime = pg_CurrentClockTime - (nb_frames_from_beginning / movieCaptFreq);
		//printf("set position frames from start %d frames left %d initialTime %.5f\n", nb_frames_from_beginning, nbFramesLeft, initialTime);

		currentMovieSoundPeakIndex = 0;
		currentMovieSoundOnsetIndex = 0;
		// sets the position for the onsets and peaks as well
		updateMoviePeakOrOnset();
	}
}
// changes the movie initial time if capture frequency changes
const void media_status::reset_initialTime(float new_captFreq) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (movieCaptFreq > 0) {
			int nb_frames_from_beginning = initialMediaNbFrames - nbFramesLeft;
			double initialTimeBeforeChange = initialTime;
			initialTime = pg_CurrentClockTime - (nb_frames_from_beginning / new_captFreq);
			//printf("Initial time before %.2f after %.2f\n", initialTimeBeforeChange, initialTime);
		}
	}
}

const void media_status::updateMoviePeakOrOnset() {
	double timeFromBeginning = pg_CurrentClockTime - initialTime;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (int(pg_VideoTracks[pg_current_configuration_rank].size()) > currentlyPlaying_movieNo
			&& pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaksFileName != "") {
			updatePeakOrOnset(timeFromBeginning, &pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks, &pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets,
				&pg_movie_sound_peak, &pg_movie_sound_onset, nbMovieSoundPeakIndex, nbMovieSoundOnsetIndex, &currentMovieSoundPeakIndex, &currentMovieSoundOnsetIndex);
		}
	}
}
// reads one frame and updates the remaining number of frames in the movie
const void media_status::read_one_frame() {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		nbFramesLeft--;
		updateMoviePeakOrOnset();
	}
}
// (re)starts the movie from begginning with its total number of frames
const void media_status::reset_movie(int nbTotFramesLeft) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// printf("movie restarted\n");
		nbFramesLeft = nbTotFramesLeft;
		initialMediaNbFrames = nbFramesLeft;
		initialTime = pg_CurrentClockTime;

		currentMovieSoundPeakIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundPeakIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackPeaks.size();
		}
		else {
			nbMovieSoundPeakIndex = 0;
		}
		currentMovieSoundOnsetIndex = 0;
		if (currentlyPlaying_movieNo >= 0) {
			nbMovieSoundOnsetIndex = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoSoundtrackOnsets.size();
		}
		else {
			nbMovieSoundOnsetIndex = 0;
		}
	}
}
media_status::~media_status() {

}

// checks whether a peak or an onset are passed or closer than one frame
void updatePeakOrOnset(double timeFromBeginning, vector<float>* peaks, vector<float>* onsets,
	bool* is_new_peak, bool* is_new_onset, int nbPeaks, int nbOnsets, int* curPeakIndex, int* curOnsetIndex) {
	*is_new_peak = false;
	//printf("curPeakIndex %d\n", *curPeakIndex);
	if (*curPeakIndex < nbPeaks - 1) {
		float nextPeak = (*peaks)[*curPeakIndex + 1];
		while (nextPeak < timeFromBeginning || nextPeak - timeFromBeginning < 1 / 70.) {
			*is_new_peak = true;
			//printf("peak next %.5f time from begin %.5f index %d\n", nextPeak, timeFromBeginning, *curPeakIndex);
			(*curPeakIndex)++;
			if (*curPeakIndex < nbPeaks - 1) {
				nextPeak = (*peaks)[*curPeakIndex + 1];
			}
			else {
				break;
			}
		}
	}
	*is_new_onset = false;
	if (*curOnsetIndex < nbOnsets - 1) {
		float nextOnset = (*onsets)[*curOnsetIndex + 1];
		while (nextOnset < timeFromBeginning || nextOnset - timeFromBeginning < 1 / 70.) {
			*is_new_onset = true;
			//printf("onset next %.5f time from begin %.5f\n", nextOnset, timeFromBeginning);
			(*curOnsetIndex)++;
			if (*curOnsetIndex < nbOnsets - 1) {
				nextOnset = (*onsets)[*curOnsetIndex + 1];
			}
			else {
				break;
			}
		}
	}
}

void pg_play_movie_no(void) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		if (playing_movieNo >= 0 && playing_movieNo < int(pg_VideoTracks[pg_current_configuration_rank].size())
			&& playing_movieNo != currentlyPlaying_movieNo) {
			pg_movie_frame.setTo(Scalar(0, 0, 0));

			// printf("VideoPb Video initialization inside command movie+ (thread) \n");

			BrokenInterpolationVar[_playing_movieNo] = true;
			currentlyPlaying_movieNo = playing_movieNo;
			pg_movie_frame.setTo(Scalar(0, 0, 0));
			sprintf(AuxString, "/movie_shortName %s", pg_VideoTracks[pg_current_configuration_rank][playing_movieNo].videoShortName.c_str());
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");

			// texture ID initialization (should not be inside a thread)
			if (pg_movie_texture_texID == NULL_ID) {
				glGenTextures(1, &pg_movie_texture_texID);
			}

			is_movieLoading = true;
			printf("Loading %s\n", pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());
			// sprintf(AuxString, "/message %s", movieFileName[currentlyPlaying_movieNo].c_str()); pg_send_message_udp((char *)"s", (char *)AuxString, (char *)"udp_TouchOSC_send");
			pg_initVideoMoviePlayback_nonThreaded(&pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName);
		}
	}
}

///////////////////////////////////////
// MOVIE STREAMING JUMPS
void pg_movie_backward(int nb_frames_back) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie - nb_frames_back;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// rewinds the movie
		if (new_pos_in_movie > 0) {
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, new_pos_in_movie);
			pg_movie_status.set_position(new_pos_in_movie);
			printf("rewinds %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
		}
		else {
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, 0);
			pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
			printf("starts from beginning\n");
		}
	}
}

void pg_movie_forward(int nb_frames_forth) {
	// calculates hypothetical new position
	int posInMovie = int(pg_movie_capture.get(CAP_PROP_POS_FRAMES));
	int new_pos_in_movie = posInMovie + nb_frames_forth;
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// advances in the movie
		if (new_pos_in_movie < pg_movie_status.get_initialNbFrames()) {
			printf("forward %d frames %d frames left %.1f initial time\n", int(movieCaptFreq * 10), pg_movie_status.get_nbFramesLeft(), pg_movie_status.get_initialTime());
			pg_movie_capture.set(CAP_PROP_POS_FRAMES, new_pos_in_movie);
			pg_movie_status.set_position(new_pos_in_movie);
		}
		// else stays where it 
	}
}

///////////////////////////////////////
// MOVIE AND CAMERA FRAME CAPTURE
void pg_update_clip_camera_and_movie_frame(void) {
	//////////////////////////////////////////////////////////////////////
	// CLIP, CAMERA AND MOVIE FRAME UPDATES BEFORE RENDERING
	// /////////////////////////
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_clipCaptFreq]) {
		// reads next clip frame
		for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
			for (int clipSide = 0; clipSide < _clipLR; clipSide++) {
				//printf("currentPhotoTrack %d clipCaptFreq %.2f clip No %d nb clips %d side %d rank %d\n", currentPhotoTrack, clipCaptFreq, pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), pg_nbClips, clipSide, indClipRank);
				if (currentPhotoTrack >= 0 && clipCaptFreq > 0
					&& pg_clip_status[clipSide].clip_play[indClipRank]
					&& pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0
					&& pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) < pg_nbClips[pg_current_configuration_rank]) {

					// either the first frame has not been captured and the clip should loop at the beginning
					// or whe are coming close to the end of the clip
					//printf("clip update side %d rank %d\n", clipSide, indClipRank);
					pg_clip_status[clipSide].get_nextFrameAndUpdate(indClipRank);
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_movieCaptFreq]) {
		// /////////////////////////
		// reads next movie frame
		if (currentVideoTrack >= 0 && movieCaptFreq > 0
			&& currentlyPlaying_movieNo >= 0
			&& !is_movieLoading
			) {

			// non threaded video update according to fps and time since the beginning of the movie play
			bool new_movie_frame = false;

			// tries to play in sync the whole movie, so counts the time from the 
			// first frame and compares the actual frame no (actualMovieFrameNo)
			// with the frame no expected to play with the correct frame rate (expectedMovieFrameNo)
			int actualMovieFrameNo = (pg_movie_status.get_initialNbFrames() - pg_movie_status.get_nbFramesLeft());
			int expectedMovieFrameNo = int((pg_CurrentClockTime - pg_movie_status.get_initialTime()) * movieCaptFreq);

			is_movieAtEnd = false;

			// either the first frame has not been captured and the movie should loop at the beginning
			// or whe are coming close to the end of the movie
			//printf("%d\n", pg_movie_status.get_nbFramesLeft());
			if (!pg_FirstMovieFrameIsAvailable || (movie_loop && pg_movie_status.get_nbFramesLeft() <= 1)) {
				//printf("Full video loop %d\n", pg_FrameNo);
				// Full video loop
				if (pg_movie_status.get_nbFramesLeft() <= 1) {
					// resets the movie
					pg_movie_capture.set(CAP_PROP_POS_FRAMES, 0);
					pg_movie_status.reset_movie(int(pg_movie_capture.get(CAP_PROP_FRAME_COUNT)));
					printf("Movie restarted\n");
				}
			}

			// if looping is not on and movie is closed to the end, loops on the last 10 frames
			else if (!movie_loop && pg_movie_status.get_nbFramesLeft() <= 1) {
				// does not retrieve more frames and stays on the last one
				is_movieAtEnd = true;
				//// Short end video loop
				//pg_movie_backward(int(movieCaptFreq * 10));
				//printf("Short end video loop at %d, posInMovie %d\n",
				//	pg_FrameNo, pg_movie_status.get_nbFramesLeft());
			}

			// skips frames if rendering is slower than movie fps
			if ((!is_movieAtEnd) && (pg_FirstMovieFrameIsAvailable && expectedMovieFrameNo - actualMovieFrameNo > 2)) {
				while (expectedMovieFrameNo - actualMovieFrameNo > 2) {
					if (pg_movie_capture.grab()) {
						pg_movie_status.read_one_frame();
						actualMovieFrameNo = pg_movie_status.get_nbFramesRead();
						//printf("skip 1 frame\n");
					}
					else {
						break;
					}
				}
			}

			//Grabs and returns a frame from movie if it is time for it
			if (!is_movieAtEnd && (!pg_FirstMovieFrameIsAvailable || expectedMovieFrameNo - actualMovieFrameNo >= 1)) {
				if (pg_movie_capture.grab()) {
					pg_movie_capture.retrieve(pg_movie_frame);
					if (!pg_movie_frame.data) {
						printf("Movie frame not grabbed!\n");
						// has not succeeded to load the movie
						if (!pg_FirstMovieFrameIsAvailable) {
							pg_FirstMovieFrameIsAvailable = false;
						}
						return;
					}
					else {
						pg_movie_status.read_one_frame();
						actualMovieFrameNo = pg_movie_status.get_nbFramesRead();
						if (!pg_FirstMovieFrameIsAvailable) {
							pg_FirstMovieFrameIsAvailable = true;
						}
						new_movie_frame = true;
					}
				}
			}

			//  transfer to GPU
			if (new_movie_frame) {
				pg_initMovieFrameTexture(&pg_movie_frame);
			}
			//elapsedTimeSinceLastCapture = pg_CurrentClockTime - lastFrameCaptureTime;
			// printf("Capture at %.4f last capture %.4f elapsed time %.4f frame duration %.4f\n", pg_CurrentClockTime, lastFrameCaptureTime, elapsedTimeSinceLastCapture, (1.0f / movieCaptFreq));
		}
	}

	// /////////////////////////
	// reads next camera frame
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_cameraCaptFreq]) {
		if ((currentVideoTrack >= 0
			&& pg_cameraCaptureIsOn
			&& (// grabs according to camera capture frequence
				(cameraCaptFreq > 0 && pg_FrameNo % int(60.0 / cameraCaptFreq) == 0)
				// grabs at the beginning, otherwise the program is stuck if no camera frame reading takes place
				|| (pg_FrameNo >= PG_CAMERA_REST_DELAY && pg_FrameNo <= PG_CAMERA_REST_DELAY + 1000 && pg_FrameNo % 3 == 0))
			// a delay has to be let for camera to digest new parameter values
			&& (pg_FrameNo - pg_LastCameraParameterChange_Frame) > PG_CAMERA_REST_DELAY
			// the camera weight is positive
			&& cameraWeight > 0)
			|| (reset_camera || initialSecondBGCapture == 1)) {
			// currentVideoTrack <=> video off
			// frame capture
			// printf("frame capture\n");

			// non threaded
			// printf("*** non threaded camera frame capture %d\n", pg_FrameNo);
			if ((pg_current_active_cameraNo < 0 && -pg_current_active_cameraNo - 1 < int(pg_webCams.size()))
				|| (pg_current_active_cameraNo >= 0 && pg_current_active_cameraNo < int(pg_IPCam_capture.size()))) {
				loadCameraFrame(false, cameraNo);
			}
		}
	}
}

#if defined(WIN32)
DWORD WINAPI pg_initVideoMoviePlayback(LPVOID lpParam) {
#else
void* pg_initVideoMoviePlayback(void* lpParam) {
#endif
	string* fileName = (string*)lpParam;

	// film loading openCV
	sprintf(AuxString, "/movie_fileName %s",
		pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());
	pg_movie_capture.open((pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file %s is not loaded!\n", (pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}
	pg_MovieFrame_buffer_data.PhotoName = pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName;

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


void* pg_initVideoMoviePlayback_nonThreaded(string * fileName) {
	// printf("VideoPb Init movie playback and capture first frame non threaded\n");

	//printf("VideoPb Init movie playback and capture first frame %s\n",
	//	pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());

	// film loading openCV
	sprintf(AuxString, "/movie_fileName %s",
		pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());
	pg_movie_capture.open(pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName.c_str());

	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] is not loaded!\n", (pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}

	//printf("Opens %s\n", (cwd + videos_directory + movieFileName[currentlyPlaying_movieNo]).c_str());
	if (!pg_movie_capture.isOpened()) {
		printf("Movie file [%s] not loaded!\n", (cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName).c_str());
		return NULL;
	}
	pg_MovieFrame_buffer_data.PhotoName = cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName;

	// printf("Name %s Opened\n", pg_MovieFrame_buffer_data.PhotoName);

	//Grabs and returns a frame from movie
	pg_movie_capture >> pg_FirstMovieFrame;
	// texture loading
	if (!pg_FirstMovieFrame.data) {
		printf("Movie frame %s not grabbed!\n", (cwd + videos_directory + pg_VideoTracks[pg_current_configuration_rank][currentlyPlaying_movieNo].videoFileName).c_str());
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


///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// // CLIPS
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// // CLIP TRACKS
clip_track::clip_track(int ind_clip, int nbFrames, string name) {
	clipNo = ind_clip;
	initialNbFrames = nbFrames;
	for (int ind = 0; ind < _NbMaxCues; ind++) {
		clip_cue[ind] = -1;
	}
	clip_name = name;
}
// catches a cue
int clip_track::set_cue_onOff(int indCue, int frameNo) {
	if (indCue < _NbMaxCues) {
		printf("onOff indCue %d cue value %d\n", indCue, clip_cue[indCue]);
		if (clip_cue[indCue] < 0 && frameNo < initialNbFrames) {
			clip_cue[indCue] = frameNo;
		}
		else {
			clip_cue[indCue] = -1;
		}
		return clip_cue[indCue];
	}
	return -1;
}
// catches a cue
void clip_track::set_cue_off(int indCue) {
	if (indCue < _NbMaxCues) {
		//printf("Reset indCue %d cue value %d\n", indCue, -1);
		clip_cue[indCue] = -1;
	}
}
// catches a cue
void clip_track::set_cue(int indCue, int indFrame) {
	if (indCue < _NbMaxCues && indFrame < initialNbFrames) {
		clip_cue[indCue] = indFrame;
	}
}
// returns a cue
int clip_track::get_cue(int indCue) {
	if (indCue < _NbMaxCues) {
		return clip_cue[indCue];
	}
	else {
		return -1;
	}
}
// initial total number of frames in the clip
int clip_track::get_initialNbFrames() {
	return initialNbFrames;
}
// clip name
string clip_track::get_name() {
	return clip_name;
}

///////////////////////////////////////////////////////
// CLIP STATUS MANAGEMENT WHILE STREAMING
clip_status::clip_status(int clipSide) {
	for (int ind = 0; ind < PG_NB_PARALLEL_CLIPS; ind++) {
		currentlyPlaying_clipNo[ind] = -1;
		lastFrame[ind] = 0;
		clip_autoplay[ind] = true;
		clip_play[ind] = true;
		lastPlayedFrameTime[ind] = pg_CurrentClockTime;
		clip_level[ind] = 1.0;
		clip_r_channel_level[ind] = 1.0f;
		clip_g_channel_level[ind] = 1.0f;
		clip_b_channel_level[ind] = 1.0f;
	}
	sideLR = pg_clip_LR(clipSide);
	cumulated_scratch_offset = 0.;
	currentFPS = 0;
	last_scratch_angle = 0.f;
	last_scratch_angle_change = 0.;
	last_nudge_angle = 0.f;
	last_nudge_angle_change = 0.;
	is_scratch_pressed = false;
	jump_frame_freq = 0;
}
// last frame index
int clip_status::get_lastFrame(int indClipRank) {
	return lastFrame[indClipRank];
}
// last frame exact time
double clip_status::get_lastPlayedFrameTime(int indClipRank) {
	return lastPlayedFrameTime[indClipRank];
}
void clip_status::set_lastPlayedFrameTime(int indClipRank, double a_time) {
	lastPlayedFrameTime[indClipRank] = a_time;
}

// next frame index and sets it as last frame together 
// with memory of exact time of current frame (including the delay to the current time)
int clip_status::get_nextFrameAndUpdate(int indClipRank) {
	// if minimally one frame has elapsed
	double timeLapseSinceLastFrame = (pg_CurrentClockTime - lastPlayedFrameTime[indClipRank]);
	// clip is not updated if the speed is null or if the scratching plate is pressed 
	if (currentFPS != 0 && !is_scratch_pressed) {
		if (fabs(timeLapseSinceLastFrame) >= fabs(1. / currentFPS)) {
			int lastPlayedFrame = lastFrame[indClipRank];
			int nb_elapsedFrames = int(floor(fabs(timeLapseSinceLastFrame) * currentFPS));
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames);
			int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
			while (lastFrame[indClipRank] >= initialNbFrames) {
				lastFrame[indClipRank] -= initialNbFrames;
			}
			while (lastFrame[indClipRank] < 0) {
				lastFrame[indClipRank] += initialNbFrames;
			}
			if (lastFrame[indClipRank] != lastPlayedFrame) {
				lastPlayedFrameTime[indClipRank] = lastPlayedFrameTime[indClipRank] + nb_elapsedFrames / currentFPS;
				if (fabs(pg_CurrentClockTime - lastPlayedFrameTime[indClipRank]) > 1. / currentFPS) {
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			if (jump_frame_freq > 0 && pg_FrameNo % max(1, (10 - jump_frame_freq)) == 0) {
				play_offset_position(indClipRank, 0.5);
			}
			if (initialNbFrames != 0) {
				if (indClipRank == 0) {
					if (sideLR == _clipLeft) {
						sprintf(AuxString, "/clip_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(AuxString, "/clip_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
				}
#if PG_NB_PARALLEL_CLIPS >= 2
				else if (indClipRank == 1) {
					if (sideLR == _clipLeft) {
						sprintf(AuxString, "/clip2_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(AuxString, "/clip2_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
					}
				}
#endif
			}
		}
	}
	//if (sideLR == _clipLeft) {
	//	sprintf(AuxString, "/clip_frame_left %d", lastFrame[indClipRank]); pg_send_message_udp((char*)"i", (char*)AuxString, (char*)"udp_TouchOSC_send");
	//}
	return lastFrame[indClipRank];
}
// current speed according to nudging
double clip_status::get_currentFPS() {
	return currentFPS;
}
// position of the wheel in last interaction
float clip_status::get_last_scratch_angle(double* a_last_time) {
	last_scratch_angle_change = *a_last_time;
	return last_scratch_angle;
}
// position of the wheel in last interaction
float clip_status::get_last_nudge_angle(double* a_last_time) {
	last_nudge_angle_change = *a_last_time;
	return last_nudge_angle;
}
// scratch wheel is pressed: do not update clip automatically
bool clip_status::get_scratch_pressed() {
	return is_scratch_pressed;
}
// current speed according to nudging
void clip_status::set_currentFPS(double a_FPS) {
	currentFPS = a_FPS;
	if (sideLR == _clipRight) {
		sprintf(AuxString, "/clip_FPS_right %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
	else {
		sprintf(AuxString, "/clip_FPS_left %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)AuxString, (char*)"udp_TouchOSC_send");
	}
}
// current playing clip No
int clip_status::getCurrentlyPlaying_clipNo(int indClipRank) {
	return currentlyPlaying_clipNo[indClipRank];
}
// autoplay
bool clip_status::get_clip_autoplay(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// autoplay
bool clip_status::get_clip_play(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// current playing clip No
void clip_status::setCurrentlyPlaying_clipNo(int indClipRank, int indClip) {
	//printf("setCurrentlyPlaying_clipNo %d %d\n", indClipRank, indClip);
	currentlyPlaying_clipNo[indClipRank] = indClip;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
// position of the wheel in last interaction
void clip_status::set_last_scratch_angle(float a_angle, double a_time) {
	last_scratch_angle = a_angle;
	last_scratch_angle_change = a_time;
}
// scratch wheel is pressed: do not update clip automatically
void clip_status::set_scratch_pressed(bool a_value) {
	is_scratch_pressed = a_value;
	cumulated_scratch_offset = 0.;
}
// position of the wheel in last interaction
void clip_status::set_last_nudge_angle(float a_angle, double a_time) {
	last_nudge_angle = a_angle;
	last_nudge_angle_change = a_time;
}
// sets the clip at a position from beginning
void clip_status::set_position(int indClipRank, int nb_frames_from_beginning) {
	if (nb_frames_from_beginning <= 0 && lastFrame[indClipRank] != 0) {
		reset_clip(indClipRank);
		//printf("set position %d  initialTime %.5f\n", 0, pg_CurrentClockTime);
		return;
	}
	int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = nb_frames_from_beginning % initialNbFrames;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	//printf("set position %d  initialTime %.5f\n", lastFrame[indClipRank], lastPlayedFrameTime[indClipRank]);
}
// offsets the movie position by a positive or negative time delta (play mode)
// used once for a big jump
void clip_status::play_offset_position(int indClipRank, double elapsedTime) {
	int nb_elapsedFrames = int(floor(elapsedTime * currentFPS));
	int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = max(min((lastFrame[indClipRank] + nb_elapsedFrames), initialNbFrames - 1), 0);
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
}
// offsets the movie position by a positive or negative time delta (scratch mode)
// used several times for a small offset
void clip_status::scratch_offset_position(double timeLapse) {
	cumulated_scratch_offset += timeLapse;
	//printf("cumulated_scratch_offset %.3f\n", cumulated_scratch_offset);
	// positive number of frames, move forward
	if (cumulated_scratch_offset >= 0) {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS));
		//printf("positive lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames > 0) {
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (currentlyPlaying_clipNo[indClipRank] >= 0) {
					int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
					lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames) % initialNbFrames;
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
	// negative number of frames, move backward
	else {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS) + 1);
		//printf("negative lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames < 0) {
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (currentlyPlaying_clipNo[indClipRank] >= 0) {
					int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
					lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames + initialNbFrames) % initialNbFrames;
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
}
// (re)starts the clip from begginning with its total number of frames
void clip_status::reset_clip(int indClipRank) {
	// printf("clip restarted\n");
	if (currentlyPlaying_clipNo[indClipRank] >= 0 && currentlyPlaying_clipNo[indClipRank] < pg_nbClips[pg_current_configuration_rank]) {
		int initialNbFrames = pg_clip_tracks[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
		initialNbFrames = pg_nbCompressedClipFramesPerFolder[pg_current_configuration_rank][currentlyPlaying_clipNo[indClipRank]];
	}
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
clip_status::~clip_status() {

}


void clip_new(int indClipRank, int clipSide, int nb_arguments, float* float_arguments) {
	if (clipSide < _clipLR && clipSide >= 0 && pg_nbClips[pg_current_configuration_rank] > 0) {
		int i = 0;
		int clipNo = -1;
		if (nb_arguments == 1) {
			clipNo = int(floor(rand_0_1 * pg_nbClips[pg_current_configuration_rank])) % pg_nbClips[pg_current_configuration_rank];
			while (i++ < 10 && clipNo == pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {
				clipNo = int(floor(rand_0_1 * pg_nbClips[pg_current_configuration_rank])) % pg_nbClips[pg_current_configuration_rank];
			}
		}
		else if (nb_arguments == 3 || nb_arguments == 2) {
			if ((nb_arguments == 3 && (float_arguments[1] > float_arguments[2])) || float_arguments[1] < 0) {
				if (nb_arguments == 2)
					sprintf(ErrorStr, "Incorrect new clip arguments (%.2f)!", float_arguments[1]); ReportError(ErrorStr);
				if (nb_arguments == 3)
					sprintf(ErrorStr, "Incorrect new clip arguments (%.2f, %.2f)!", float_arguments[1], float_arguments[2]); ReportError(ErrorStr);
			}
			int clipMin = int(float_arguments[1]);
			int clipMax = pg_nbClips[pg_current_configuration_rank];
			if (nb_arguments == 3) {
				clipMax = int(float_arguments[2]);
			}
			clipNo = int(floor(rand_0_1 * (clipMax - clipMin + 1))) % (clipMax - clipMin + 1) + clipMin;
			while (i++ < 10 && clipNo == pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {
				clipNo = int(floor(rand_0_1 * (clipMax - clipMin + 1))) % (clipMax - clipMin + 1) + clipMin;
			}
		}
		if (clipNo >= 0 && clipNo < pg_nbClips[pg_current_configuration_rank]) {
			pg_play_clip_no(indClipRank, clipSide, clipNo);
		}
	}
}

void pg_play_clip_no(int indClipRank, int clipSide, int clipNo) {
	//printf("play clip No %d side %d rank %d previous clip no %d\n", clipNo, clipSide, indClipRank, pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank));
	if ((clipSide == _clipLeft || clipSide == _clipRight)
		&& clipNo >= 0 && clipNo < pg_nbClips[pg_current_configuration_rank]
		&& clipNo != pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {

		pg_clip_status[clipSide].setCurrentlyPlaying_clipNo(indClipRank, clipNo);
		pg_clip_status[clipSide].clip_play[indClipRank] = pg_clip_status[clipSide].clip_autoplay;
		if (clipSide == _clipLeft) {
			if (indClipRank == 0) {
				playing_clipNoLeft = clipNo;
				sprintf(AuxString, "/clip_shortName_0 %s", pg_clip_tracks[pg_current_configuration_rank][playing_clipNoLeft].get_name().c_str());
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
				sprintf(AuxString, "/clip_play_0 %d", playing_clipNoLeft);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				(*((string*)pg_FullScenarioVarPointers[_playing_clipNameLeft])) = pg_clip_tracks[pg_current_configuration_rank][playing_clipNoLeft].get_name();
				playing_clipNameLeft = pg_clip_tracks[pg_current_configuration_rank][playing_clipNoLeft].get_name();
				//printf("Play left clip %d at %d fps, is playing %d nb frames %d\n", pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_clip_status[clipSide].clip_play[indClipRank]), pg_clip_tracks[pg_current_configuration_rank][pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)].get_initialNbFrames());
			}
#if PG_NB_PARALLEL_CLIPS >= 2
			else {
				playing_secondClipNoLeft = clipNo;
				sprintf(AuxString, "/clip2_shortName_0 %s", pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoLeft].get_name().c_str());
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
				sprintf(AuxString, "/clip2_play_0 %d", playing_secondClipNoLeft);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				//printf("Play second left clip %d at %d fps, is playing %d\n", pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_clip_status[clipSide].clip_play[indClipRank]));
				(*((string*)pg_FullScenarioVarPointers[_playing_secondClipNameLeft])) = pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoLeft].get_name();
				playing_secondClipNameLeft = pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoLeft].get_name();
			}
#endif
		}
		else {
			if (indClipRank == 0) {
				playing_clipNoRight = pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
				sprintf(AuxString, "/clip_shortName_1 %s", pg_clip_tracks[pg_current_configuration_rank][playing_clipNoRight].get_name().c_str());
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
				sprintf(AuxString, "/clip_play_1 %d", playing_clipNoRight);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				//printf("Play right clip %d at %d fps, is playing %d\n", pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_clip_status[clipSide].clip_play[indClipRank]));
				(*((string*)pg_FullScenarioVarPointers[_playing_clipNameRight])) = pg_clip_tracks[pg_current_configuration_rank][playing_clipNoRight].get_name();
				playing_clipNameRight = pg_clip_tracks[pg_current_configuration_rank][playing_clipNoRight].get_name();
			}
#if PG_NB_PARALLEL_CLIPS >= 2
			else {
				playing_secondClipNoRight = pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
				sprintf(AuxString, "/clip2_shortName_1 %s", pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoRight].get_name().c_str());
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
				sprintf(AuxString, "/clip2_play_1 %d", playing_secondClipNoRight);
				pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");
				//printf("Play second right clip %d at %d fps, is playing %d\n", pg_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_clip_status[clipSide].clip_play[indClipRank]));
				(*((string*)pg_FullScenarioVarPointers[_playing_secondClipNameRight])) = pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoRight].get_name();
				playing_secondClipNameRight = pg_clip_tracks[pg_current_configuration_rank][playing_secondClipNoRight].get_name();
			}
#endif
		}

		pg_clip_status[clipSide].reset_clip(indClipRank);
		pg_clip_status[clipSide].set_currentFPS(double(clipCaptFreq));
	}

}


///////////////////////////////////////
// CLIP STREAMING JUMPS

