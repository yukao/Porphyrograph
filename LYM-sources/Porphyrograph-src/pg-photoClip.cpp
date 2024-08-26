/*! \file pg-media.cpp
 *
 *
 *     File pg-media.cpp
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

// clip track
std::vector<pg_clip_track> pg_clip_tracks[PG_MAX_SCENARIOS];
// clip status (left and right)
pg_clip_status pg_all_clip_status[pg_enum_clipLeftRight] = { pg_clip_status(pg_enum_clipLeft), pg_clip_status(pg_enum_clipRight) };

// echo modulator for clips
float pg_fx_dry_wet = 0.f;

// DIAPORAMAS
// diaporama directories
vector<string> pg_compressedImageDirsNames[PG_MAX_SCENARIOS];
// files inside diaporama directories
vector <vector<PhotoData>> pg_compressedImageData[PG_MAX_SCENARIOS];
vector<vector<string>> pg_compressedImageFilesNames[PG_MAX_SCENARIOS];

float pg_Photo_weight[PG_PHOTO_NB_TEXTURES] = { 0.0f , 0.0f };

bool pg_ascendingDiaporama = true;

// photo albums
std::string pg_ImageDirectory[PG_MAX_SCENARIOS] = { "" };

// short video clip albums
extern string pg_ClipDirectory[PG_MAX_SCENARIOS];
extern int pg_NbClipAlbums[PG_MAX_SCENARIOS];

// IMAGE TEXTURES
// Images used for displaying or for swapping
// PhotoData describing a buffer of images stored in the GPU
// these images are used to make piled rendering 
// they are doubled by swap images used to smoothly change between images
vector<int> pg_nbCompressedImagesPerFolder[PG_MAX_SCENARIOS];
int pg_nbCompressedImageDirs[PG_MAX_SCENARIOS] = { 0 };
int pg_nbCompressedImages[PG_MAX_SCENARIOS] = { 0 };

//////////////////////////////////////////////////////////////////////////////////////////////////////
// DIAPORAMA MANAGEMENT: INITIALIZATION, MODIFICATION
//////////////////////////////////////////////////////////////////////////////////////////////////////

// ADDS A DIRECTORY (A NEW IMAGE)
void pg_AddDirectoryToDiaporamas(int indScenario, fs::directory_entry dir_entry) {
	auto subDirIter = fs::directory_iterator(dir_entry);
	++pg_nbCompressedImageDirs[indScenario];
	vector<string>dir_fileNames;
	vector<PhotoData>dir_photoData;
	// scans image files inside a diaporama
	int nbImagesInDir = 0;
	//printf("Dir photo %s\n", dir_entry.path().string().c_str());
	for (auto& subdir_entry : subDirIter) {
		if (subdir_entry.is_regular_file()) {
			PhotoData photoData;
			++pg_nbCompressedImages[indScenario];
			dir_photoData.push_back(photoData);

			// allocates a texture ID for the image
			glGenTextures(1, &(dir_photoData[nbImagesInDir].texBuffID));
			dir_photoData[nbImagesInDir].IDallocated = true;

			// stores the photo file name
			dir_photoData[nbImagesInDir].PhotoName = subdir_entry.path().string();
			//printf("Subdir photo %s ID %d\n", subdir_entry.path().string().c_str(), dir_photoData[nbImagesInDir].texBuffID);

			// loads the images with a size that corresponds to the screen size
			dir_photoData[nbImagesInDir].pg_loadPhoto(true, pg_workingWindow_width_powerOf2, pg_window_height_powerOf2, false, indScenario);

			// loads the compressed image into GPU
			dir_photoData[nbImagesInDir].pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indScenario);

			pg_printOglError(8);

			dir_fileNames.push_back(subdir_entry.path().string());
			++nbImagesInDir;
		}
	}
	pg_compressedImageFilesNames[indScenario].push_back(dir_fileNames);
	pg_compressedImageData[indScenario].push_back(dir_photoData);
	pg_compressedImageDirsNames[indScenario].push_back(dir_entry.path().string());
	pg_nbCompressedImagesPerFolder[indScenario].push_back(nbImagesInDir);
}

// EMPTY DIAPORAMA INITIALIZATION
void  pg_initDiaporamas(void) {
	///////////////////////////////////////////////
	// NULL INITIALIZATIONS
	for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
		pg_nbCompressedImagesPerFolder[indScenario].clear();
		pg_compressedImageData[indScenario].clear();
		pg_nbCompressedImages[indScenario] = 0;
		pg_nbCompressedImageDirs[indScenario] = 0;
		pg_compressedImageDirsNames[indScenario].clear();
		pg_compressedImageFilesNames[indScenario].clear();
	}
}

// LOADS ALL DIAPORAMAS
bool  pg_loadAllDiaporamas(void) {
	bool valret = true;

	printf("Load Diaporamas:\n");
	for (int indScenario = 0; indScenario < pg_NbScenarios; indScenario++) {
		std::cout << "    " << indScenario << ": ";

		////////////////////////////////////////////
		// LOADS IMAGES FROM FOLDERS
		std::cout << pg_ImageDirectory[indScenario] << " (";
		// scans a diaporama
		if (fs::is_directory(pg_ImageDirectory[indScenario])) {
			auto dirIter = fs::directory_iterator(pg_ImageDirectory[indScenario]);
			for (auto& dir_entry : dirIter)
			{
				if (dir_entry.is_directory()) {
					pg_AddDirectoryToDiaporamas(indScenario, dir_entry);
				}
			}
		}
		if (pg_nbCompressedImageDirs[indScenario] <= 0) {
			std::cout << "0 images) ";
			std::cout << std::endl;
		}
		else {
			std::cout << pg_nbCompressedImages[indScenario] << " images from " << pg_nbCompressedImageDirs[indScenario] << " folders), ";
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

// ADDS NEW PHOTOS TO DIAPORAMA
void pg_AddFilesToDiaporamas(int indScenario, fs::directory_entry dir_entry) {
	auto it = find(pg_compressedImageDirsNames[indScenario].begin(), pg_compressedImageDirsNames[indScenario].end(),
		dir_entry.path().string());
	// If element was found 
	int dir_rank = -1;
	if (it != pg_compressedImageDirsNames[indScenario].end())
	{
		// calculating the index 
		dir_rank = it - pg_compressedImageDirsNames[indScenario].begin();
	}

	auto subDirIter = fs::directory_iterator(dir_entry);
	vector<string>dir_fileNames = pg_compressedImageFilesNames[indScenario][dir_rank];
	// scans image files inside a diaporama
	int nbImagesInDir = pg_nbCompressedImagesPerFolder[indScenario][dir_rank];
	for (auto& subdir_entry : subDirIter) {
		if (subdir_entry.is_regular_file()) {
			// file is not found
			if (std::find(dir_fileNames.begin(), dir_fileNames.end(), subdir_entry.path().string())
				== dir_fileNames.end()) {
				++pg_nbCompressedImages[indScenario];
				PhotoData curPhotoData;

				// allocates a texture ID for the image
				glGenTextures(1, &(curPhotoData.texBuffID));
				curPhotoData.IDallocated = true;

				// stores the photo file name
				curPhotoData.PhotoName = subdir_entry.path().string();

				// loads the images with a size that corresponds to the screen size
				curPhotoData.pg_loadPhoto(true, pg_workingWindow_width_powerOf2, pg_window_height_powerOf2, false, indScenario);

				// loads the compressed image into GPU
				curPhotoData.pg_toGPUPhoto(false, GL_RGB8, GL_UNSIGNED_BYTE, GL_LINEAR, indScenario);
				// printf("texture ID indCompressedImage %d\n", dir_photoData[indCompressedImage]->texBuffID);

				pg_compressedImageData[indScenario][dir_rank].push_back(curPhotoData);

				pg_printOglError(8);

				pg_compressedImageFilesNames[indScenario][dir_rank].push_back(subdir_entry.path().string());
				++nbImagesInDir;
			}
		}
	}
	pg_nbCompressedImagesPerFolder[indScenario][dir_rank] = nbImagesInDir;
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

// CREATES A NEW DIAPORAMA
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

// RELOADS ALL DIAPORAMAS
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
// DIAPORAMA MANAGEMENT: DISPLAY
//////////////////////////////////////////////////////////////////////////////////////////////////////

// RANDOM DIAPORAMA SELECTION
void pg_diaporama_random() {
	if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0) {
		// goes to the first photo diaporama if it is not already selected and if there is one 
		if (photo_diaporama < 0 && pg_ImageDirectory[pg_ind_scenario] != "") {
			photo_diaporama = 0;
		}
		photo_diaporama = int(rand_0_1 * pg_nbCompressedImageDirs[pg_ind_scenario]) % pg_nbCompressedImageDirs[pg_ind_scenario];
		//printf("photo_diaporama %d\n", photo_diaporama);
		sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
		pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		pg_launch_diaporama(0);
	}
}

// DISPLAYS A GIVEN PHOTO INSIDE A DIAPORAMA
void pg_diaporama_slide(int slideNo) {
	if (slideNo >= 0 && photo_diaporama >= 0 && photo_diaporama < pg_nbCompressedImageDirs[pg_ind_scenario]) {
		pg_CurrentDiaporamaFile = slideNo % pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
		pg_launch_diaporama(pg_CurrentDiaporamaFile);
	}
}

// LAUNCHES A DIAPORAMA:
// initializes the blending of each image inside a folder
void pg_launch_diaporama(int slideNo) {
	//std::cout << "launch diaporama " << std::endl;
	// initialization
	for (int indPhoto = 0; indPhoto < PG_PHOTO_NB_TEXTURES; indPhoto++) {
		pg_Photo_swap_buffer_data[indPhoto].blendStart = -1.0f;
		pg_Photo_swap_buffer_data[indPhoto].indSwappedPhoto = slideNo;
	}

	// loads the first images if it is not the case
	pg_CurrentDiaporamaFile = slideNo % pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
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
	pg_printOglError(465);
}

// AFTER LOADING IN MEMORY, FOR DISPLAY: TAKES IMAGES IN A SINGLE DIR ONE AFTER ANOTHER
// AND POSSIBLY LOOPS INSIDE THE DIRECTORY
void pg_nextFileIndexMemoryLoop(bool ascending) {
	if (photo_diaporama < pg_nbCompressedImageDirs[pg_ind_scenario]
		&& pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama] > 0) {
		if (ascending) {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile + 1) % pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
		}
		else {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile - 1 + pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama])
				% pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
		}
	}
	else {
		while (pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama] == 0) {
			photo_diaporama++;
			pg_CurrentDiaporamaFile = 0;
		}
		if (ascending) {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile + 1) % pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
		}
		else {
			pg_CurrentDiaporamaFile
				= (pg_CurrentDiaporamaFile - 1 + pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama])
				% pg_nbCompressedImagesPerFolder[pg_ind_scenario][photo_diaporama];
		}
	}
}

// DIAPORAMA UPDATE (TIME-BASED PHOTO SWAP°
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
			pg_nextFileIndexMemoryLoop(pg_ascendingDiaporama);
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


//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP TRACKS MANAGEMENT
//////////////////////////////////////////////////////////////////////////////////////////////////////
// // CLIP TRACKS
pg_clip_track::pg_clip_track(int ind_clip, int nbFrames, string name) {
	clipNo = ind_clip;
	initialNbFrames = nbFrames;
	for (int ind = 0; ind < _NbMaxCues; ind++) {
		clip_cue[ind] = -1;
	}
	clip_name = name;
}
// catches a cue
int pg_clip_track::set_cue_onOff(int indCue, int frameNo) {
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
void pg_clip_track::set_cue_off(int indCue) {
	if (indCue < _NbMaxCues) {
		//printf("Reset indCue %d cue value %d\n", indCue, -1);
		clip_cue[indCue] = -1;
	}
}
// catches a cue
void pg_clip_track::set_cue(int indCue, int indFrame) {
	if (indCue < _NbMaxCues && indFrame < initialNbFrames) {
		clip_cue[indCue] = indFrame;
	}
}
// returns a cue
int pg_clip_track::get_cue(int indCue) {
	if (indCue < _NbMaxCues) {
		return clip_cue[indCue];
	}
	else {
		return -1;
	}
}
// initial total number of frames in the clip
int pg_clip_track::get_initialNbFrames() {
	return initialNbFrames;
}
// clip name
string pg_clip_track::get_name() {
	return clip_name;
}

///////////////////////////////////////////////////////
// CLIP STATUS MANAGEMENT WHILE STREAMING
pg_clip_status::pg_clip_status(int clipSide) {
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
int pg_clip_status::get_lastFrame(int indClipRank) {
	return lastFrame[indClipRank];
}
// last frame exact time
double pg_clip_status::get_lastPlayedFrameTime(int indClipRank) {
	return lastPlayedFrameTime[indClipRank];
}
void pg_clip_status::set_lastPlayedFrameTime(int indClipRank, double a_time) {
	lastPlayedFrameTime[indClipRank] = a_time;
}

// next frame index and sets it as last frame together 
// with memory of exact time of current frame (including the delay to the current time)
int pg_clip_status::get_nextFrameAndUpdate(int indClipRank) {
	// if minimally one frame has elapsed
	double timeLapseSinceLastFrame = (pg_CurrentClockTime - lastPlayedFrameTime[indClipRank]);
	// clip is not updated if the speed is null or if the scratching plate is pressed 
	if (currentFPS != 0 && !is_scratch_pressed) {
		if (fabs(timeLapseSinceLastFrame) >= fabs(1. / currentFPS)) {
			int lastPlayedFrame = lastFrame[indClipRank];
			int nb_elapsedFrames = int(floor(fabs(timeLapseSinceLastFrame) * currentFPS));
			lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames);
			int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
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
					if (sideLR == pg_enum_clipLeft) {
						sprintf(pg_AuxString, "/clip_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(pg_AuxString, "/clip_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
					}
				}
				else if (indClipRank == 1 && PG_NB_PARALLEL_CLIPS >= 2) {
					if (sideLR == pg_enum_clipLeft) {
						sprintf(pg_AuxString, "/clip2_percent_left %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
					}
					else {
						sprintf(pg_AuxString, "/clip2_percent_right %d", int(lastFrame[indClipRank] / float(initialNbFrames) * 100.f)); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
					}
				}
			}
		}
	}
	//if (sideLR == pg_enum_clipLeft) {
	//	sprintf(pg_AuxString, "/clip_frame_left %d", lastFrame[indClipRank]); pg_send_message_udp((char*)"i", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	//}
	return lastFrame[indClipRank];
}
// current speed according to nudging
double pg_clip_status::get_currentFPS() {
	return currentFPS;
}
// position of the wheel in last interaction
float pg_clip_status::get_last_scratch_angle(double* a_last_time) {
	last_scratch_angle_change = *a_last_time;
	return last_scratch_angle;
}
// position of the wheel in last interaction
float pg_clip_status::get_last_nudge_angle(double* a_last_time) {
	last_nudge_angle_change = *a_last_time;
	return last_nudge_angle;
}
// scratch wheel is pressed: do not update clip automatically
bool pg_clip_status::get_scratch_pressed() {
	return is_scratch_pressed;
}
// current speed according to nudging
void pg_clip_status::set_currentFPS(double a_FPS) {
	currentFPS = a_FPS;
	if (sideLR == pg_enum_clipRight) {
		sprintf(pg_AuxString, "/clip_FPS_right %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
	else {
		sprintf(pg_AuxString, "/clip_FPS_left %d", int(a_FPS)); pg_send_message_udp((char*)"s", (char*)pg_AuxString, (char*)"udp_TouchOSC_send");
	}
}
// current playing clip No
int pg_clip_status::getCurrentlyPlaying_clipNo(int indClipRank) {
	return currentlyPlaying_clipNo[indClipRank];
}
// autoplay
bool pg_clip_status::get_clip_autoplay(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// autoplay
bool pg_clip_status::get_clip_play(int indClipRank) {
	return clip_autoplay[indClipRank];
}
// current playing clip No
void pg_clip_status::setCurrentlyPlaying_clipNo(int indClipRank, int indClip) {
	//printf("setCurrentlyPlaying_clipNo %d %d\n", indClipRank, indClip);
	currentlyPlaying_clipNo[indClipRank] = indClip;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
// position of the wheel in last interaction
void pg_clip_status::set_last_scratch_angle(float a_angle, double a_time) {
	last_scratch_angle = a_angle;
	last_scratch_angle_change = a_time;
}
// scratch wheel is pressed: do not update clip automatically
void pg_clip_status::set_scratch_pressed(bool a_value) {
	is_scratch_pressed = a_value;
	cumulated_scratch_offset = 0.;
}
// position of the wheel in last interaction
void pg_clip_status::set_last_nudge_angle(float a_angle, double a_time) {
	last_nudge_angle = a_angle;
	last_nudge_angle_change = a_time;
}
// sets the clip at a position from beginning
void pg_clip_status::set_position(int indClipRank, int nb_frames_from_beginning) {
	if (nb_frames_from_beginning <= 0 && lastFrame[indClipRank] != 0) {
		reset_clip(indClipRank);
		//printf("set position %d  initialTime %.5f\n", 0, pg_CurrentClockTime);
		return;
	}
	int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = nb_frames_from_beginning % initialNbFrames;
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	//printf("set position %d  initialTime %.5f\n", lastFrame[indClipRank], lastPlayedFrameTime[indClipRank]);
}
// offsets the movie position by a positive or negative time delta (play mode)
// used once for a big jump
void pg_clip_status::play_offset_position(int indClipRank, double elapsedTime) {
	int nb_elapsedFrames = int(floor(elapsedTime * currentFPS));
	int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
	lastFrame[indClipRank] = max(min((lastFrame[indClipRank] + nb_elapsedFrames), initialNbFrames - 1), 0);
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
}
// offsets the movie position by a positive or negative time delta (scratch mode)
// used several times for a small offset
void pg_clip_status::scratch_offset_position(double timeLapse) {
	cumulated_scratch_offset += timeLapse;
	//printf("cumulated_scratch_offset %.3f\n", cumulated_scratch_offset);
	// positive number of frames, move forward
	if (cumulated_scratch_offset >= 0) {
		int nb_elapsedFrames = int(floor(cumulated_scratch_offset * currentFPS));
		//printf("positive lapse %d %.2f\n", nb_elapsedFrames, cumulated_scratch_offset * currentFPS);
		if (nb_elapsedFrames > 0) {
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (currentlyPlaying_clipNo[indClipRank] >= 0) {
					int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
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
					int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
					lastFrame[indClipRank] = (lastFrame[indClipRank] + nb_elapsedFrames + initialNbFrames) % initialNbFrames;
					lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
				}
			}
			cumulated_scratch_offset = cumulated_scratch_offset - nb_elapsedFrames / currentFPS;
		}
	}
}
// (re)starts the clip from begginning with its total number of frames
void pg_clip_status::reset_clip(int indClipRank) {
	// printf("clip restarted\n");
	if (currentlyPlaying_clipNo[indClipRank] >= 0 && currentlyPlaying_clipNo[indClipRank] < pg_nbClips[pg_ind_scenario]) {
		int initialNbFrames = pg_clip_tracks[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]].get_initialNbFrames();
		initialNbFrames = pg_nbCompressedClipFramesPerFolder[pg_ind_scenario][currentlyPlaying_clipNo[indClipRank]];
	}
	lastPlayedFrameTime[indClipRank] = pg_CurrentClockTime;
	lastFrame[indClipRank] = 0;
}
pg_clip_status::~pg_clip_status() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP STREAMING
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_play_clip_no(int indClipRank, int clipSide, int clipNo) {
	//printf("play clip No %d side %d rank %d previous clip no %d\n", clipNo, clipSide, indClipRank, pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank));
	if ((clipSide == pg_enum_clipLeft || clipSide == pg_enum_clipRight)
		&& clipNo >= 0 && clipNo < pg_nbClips[pg_ind_scenario]
		&& clipNo != pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {

		pg_all_clip_status[clipSide].setCurrentlyPlaying_clipNo(indClipRank, clipNo);
		pg_all_clip_status[clipSide].clip_play[indClipRank] = pg_all_clip_status[clipSide].clip_autoplay;
		if (clipSide == pg_enum_clipLeft) {
			if (indClipRank == 0) {
				pg_playing_clipNoLeft = clipNo;
				sprintf(pg_AuxString, "/clip_shortName_0 %s", pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoLeft].get_name().c_str());
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				sprintf(pg_AuxString, "/clip_play_0 %d", pg_playing_clipNoLeft);
				pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
				(*((string*)pg_FullScenarioVarPointers[_playing_clipNameLeft])) = pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoLeft].get_name();
				playing_clipNameLeft = pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoLeft].get_name();
				//printf("Play left clip %d at %d fps, is playing %d nb frames %d\n", pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_all_clip_status[clipSide].clip_play[indClipRank]), pg_clip_tracks[pg_ind_scenario][pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)].get_initialNbFrames());
			}
			else {
				if (PG_NB_PARALLEL_CLIPS >= 2) {
					pg_playing_secondClipNoLeft = clipNo;
					sprintf(pg_AuxString, "/clip2_shortName_0 %s", pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoLeft].get_name().c_str());
					pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip2_play_0 %d", pg_playing_secondClipNoLeft);
					pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
					//printf("Play second left clip %d at %d fps, is playing %d\n", pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_all_clip_status[clipSide].clip_play[indClipRank]));
					(*((string*)pg_FullScenarioVarPointers[_playing_secondClipNameLeft])) = pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoLeft].get_name();
					playing_secondClipNameLeft = pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoLeft].get_name();
				}
			}
		}
		else {
			if (indClipRank == 0) {
				pg_playing_clipNoRight = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
				sprintf(pg_AuxString, "/clip_shortName_1 %s", pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoRight].get_name().c_str());
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				sprintf(pg_AuxString, "/clip_play_1 %d", pg_playing_clipNoRight);
				pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
				//printf("Play right clip %d at %d fps, is playing %d\n", pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_all_clip_status[clipSide].clip_play[indClipRank]));
				(*((string*)pg_FullScenarioVarPointers[_playing_clipNameRight])) = pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoRight].get_name();
				playing_clipNameRight = pg_clip_tracks[pg_ind_scenario][pg_playing_clipNoRight].get_name();
			}
			else {
				if (PG_NB_PARALLEL_CLIPS >= 2) {
					pg_playing_secondClipNoRight = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
					sprintf(pg_AuxString, "/clip2_shortName_1 %s", pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoRight].get_name().c_str());
					pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip2_play_1 %d", pg_playing_secondClipNoRight);
					pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");
					//printf("Play second right clip %d at %d fps, is playing %d\n", pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank), int(clipCaptFreq), int(pg_all_clip_status[clipSide].clip_play[indClipRank]));
					(*((string*)pg_FullScenarioVarPointers[_playing_secondClipNameRight])) = pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoRight].get_name();
					playing_secondClipNameRight = pg_clip_tracks[pg_ind_scenario][pg_playing_secondClipNoRight].get_name();
				}
			}
		}

		pg_all_clip_status[clipSide].reset_clip(indClipRank);
		pg_all_clip_status[clipSide].set_currentFPS(double(clipCaptFreq));
	}

}

//SELECTS NEW CLIP FOR STREAMING
void pg_clip_new(int indClipRank, int clipSide, int nb_arguments, float* float_arguments) {
	if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_nbClips[pg_ind_scenario] > 0) {
		int i = 0;
		int clipNo = -1;
		if (nb_arguments == 1) {
			clipNo = int(floor(rand_0_1 * pg_nbClips[pg_ind_scenario])) % pg_nbClips[pg_ind_scenario];
			while (i++ < 10 && clipNo == pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {
				clipNo = int(floor(rand_0_1 * pg_nbClips[pg_ind_scenario])) % pg_nbClips[pg_ind_scenario];
			}
		}
		else if (nb_arguments == 3 || nb_arguments == 2) {
			if ((nb_arguments == 3 && (float_arguments[1] > float_arguments[2])) || float_arguments[1] < 0) {
				if (nb_arguments == 2)
					sprintf(pg_errorStr, "Incorrect new clip arguments (%.2f)!", float_arguments[1]); pg_ReportError(pg_errorStr);
				if (nb_arguments == 3)
					sprintf(pg_errorStr, "Incorrect new clip arguments (%.2f, %.2f)!", float_arguments[1], float_arguments[2]); pg_ReportError(pg_errorStr);
			}
			int clipMin = int(float_arguments[1]);
			int clipMax = pg_nbClips[pg_ind_scenario];
			if (nb_arguments == 3) {
				clipMax = int(float_arguments[2]);
			}
			clipNo = int(floor(rand_0_1 * (clipMax - clipMin + 1))) % (clipMax - clipMin + 1) + clipMin;
			while (i++ < 10 && clipNo == pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank)) {
				clipNo = int(floor(rand_0_1 * (clipMax - clipMin + 1))) % (clipMax - clipMin + 1) + clipMin;
			}
		}
		if (clipNo >= 0 && clipNo < pg_nbClips[pg_ind_scenario]) {
			pg_play_clip_no(indClipRank, clipSide, clipNo);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIPS AND PHOTOS SCENARIO
//////////////////////////////////////////////////////////////////////////////////////////////////////
void pg_parseScenario_ClipsAndPhotos(std::ifstream& scenarioFin, int indScenario) {
	std::stringstream  sstream;
	string line;
	string ID;
	string temp;

	////////////////////////////
	////// SHORT VIDEO CLIPS ALBUMS
	//pg_NbClipAlbums[indScenario] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("clips") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"clips\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}
	//sstream >> pg_NbClipAlbums[indScenario];

	//printf("Clip album Number %d\n", pg_NbClipAlbums);

	int nbClipAlbums = 0;
	pg_ClipDirectory[indScenario] = "";
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/clips") == 0) {
			break;
		}
		else if (ID.compare("album") != 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"album\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}

		if (nbClipAlbums > 1) {
			sprintf(pg_errorStr, "Error: incorrect configuration file: Only one clip album with subdirectories is expected!"); pg_ReportError(pg_errorStr); throw 100;
		}

		sstream >> pg_ClipDirectory[indScenario];
		if (!pg_isFullPath(pg_ClipDirectory[indScenario])) {
			pg_ClipDirectory[indScenario] = pg_clips_directory + pg_ClipDirectory[indScenario];
		}
		std::cout << "Clip album directory: " << pg_ClipDirectory << "\n";
		sstream >> clip_image_width[indScenario];
		sstream >> clip_image_height[indScenario];
		sstream >> clip_crop_width[indScenario];
		sstream >> clip_crop_height[indScenario];
		sstream >> clip_max_length[indScenario];
		if (clip_image_width[indScenario] == 0 || clip_image_height[indScenario] == 0 || clip_crop_width[indScenario] == 0 || clip_crop_height[indScenario] == 0 || clip_max_length[indScenario] == 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file: missing dimension data for clip image size %dx%d cropped size %dx%d max length %d\n", clip_image_width[indScenario], clip_image_height[indScenario], clip_crop_width[indScenario], clip_crop_height[indScenario], clip_max_length[indScenario]); pg_ReportError(pg_errorStr); throw 100;
		}

		nbClipAlbums;
	}
	pg_NbClipAlbums[indScenario] = nbClipAlbums;

	////////////////////////////
	////// PHOTO ALBUMS
	//pg_NbPhotoAlbums[indScenario] = 0;
	// Number of photo albums
	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string videos
	if (ID.compare("photos") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"photos\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	int nbPhotoAlbums = 0;
	string photoAlbumDir = "";
	while (true) {
		// new line
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /svg_paths or svg_path
		if (ID.compare("/photos") == 0) {
			break;
		}
		else if (ID.compare("album") != 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"album\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}

		if (nbPhotoAlbums > 1) {
			sprintf(pg_errorStr, "Error: incorrect configuration file: Only one photo album with subdirectories is expected"); pg_ReportError(pg_errorStr); throw 100;
		}

		sstream >> photoAlbumDir;
		//std::cout << "photo album : " << photoAlbumDir << "\n";

		nbPhotoAlbums++;
	}

	if (photoAlbumDir != "") {
		if (!pg_isFullPath(photoAlbumDir)) {
			pg_ImageDirectory[indScenario] = pg_diaporamas_directory + photoAlbumDir;
		}
		else {
			pg_ImageDirectory[indScenario] = photoAlbumDir;
		}
		//std::cout << "Photo album directory: " << pg_ImageDirectory[indScenario] << std::endl;
	}
	else {
		pg_ImageDirectory[indScenario] = "";
	}

	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string messages
	if (ID.compare("messages") != 0) {
		sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"messages\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
	}

	std::getline(scenarioFin, line);
	pg_stringstreamStoreLine(&sstream, &line);
	sstream >> ID; // string lines
	if (ID.compare("/messages") != 0) {
		sstream >> pg_MessageFile[indScenario];
		if (!pg_isFullPath(pg_MessageFile[indScenario])) {
			pg_MessageFile[indScenario] = pg_messages_directory + pg_MessageFile[indScenario];
		}
		std::cout << "Loading messages from " << pg_MessageFile[indScenario] << std::endl;

		// /messages
		std::getline(scenarioFin, line);
		pg_stringstreamStoreLine(&sstream, &line);
		sstream >> ID; // string /messages
		if (ID.compare("/messages") != 0) {
			sprintf(pg_errorStr, "Error: incorrect configuration file expected string \"/messages\" not found! (instead \"%s\")", ID.c_str()); pg_ReportError(pg_errorStr); throw 100;
		}
	}
	else {
		pg_MessageFile[indScenario] = "";
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PHOTO OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////
 
void pg_aliasScript_Photo(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
	case _currentPhotoTrack_plus:
		currentPhotoTrack = (currentPhotoTrack + 1) % PG_NB_TRACKS;
		break;
	case _flashPhoto:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBeat]) {
			if (flashPhotoTrkBeat == 0) {
				if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBright]) {
					pg_Make_flashPhoto();
				}
			}
			else {
				// will be launched when the beat corresponds
				pg_is_flashPhotoTrk = true;
				pg_flashPhotoTrk_weight = 0.0f;
			}
		}
		// printf("start flash\n");
		break;
	case _diaporama_add_dirs:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			pg_addNewDiaporamas(pg_ind_scenario, true);
		}
		break;
	case _reload_all_diaporamas:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			pg_reloadAllDiaporamas(pg_ind_scenario, true);
		}
		break;
	case _diaporama_random:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			pg_diaporama_random();
		}
		break;
	case _diaporama_slide:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			pg_diaporama_slide(int(float_arguments[0]));
		}
		break;
	case _diaporama_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0) {
				// goes to the first photo diaporama if it is not already selected and if there is one 
				if (photo_diaporama < 0 && pg_ImageDirectory[pg_ind_scenario] != "") {
					photo_diaporama = 0;
				}
				photo_diaporama = (photo_diaporama + 1) % pg_nbCompressedImageDirs[pg_ind_scenario];
				printf("photo_diaporama %d\n", photo_diaporama);
				sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				pg_launch_diaporama(0);
			}
		}
		break;
	case _diaporama_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0) {
				photo_diaporama = (photo_diaporama - 1 + pg_nbCompressedImageDirs[pg_ind_scenario]) % pg_nbCompressedImageDirs[pg_ind_scenario];
				printf("photo_diaporama %d\n", photo_diaporama);
				sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				pg_launch_diaporama(0);
			}
		}
		break;
	case _flash_photo_diaporama:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_photo_diaporama]) {
			if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0 && float_arguments[0] >= 0) {
				photo_diaporama = int(float_arguments[0]) % pg_nbCompressedImageDirs[pg_ind_scenario];
				pg_CurrentDiaporamaEnd = float_arguments[1] + pg_CurrentClockTime;
				photoWeight = 1.f;
				printf("flash_photo_diaporama %d %.2f %d\n", int(float_arguments[0]), float_arguments[1], pg_FrameNo);
				sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama); pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				pg_launch_diaporama(0);
			}
		}
		break;
	default:
		sprintf(pg_errorStr, "Photo command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP OSC COMMANDS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_aliasScript_Clip(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar) {
	// special command not in the scenario file
	switch (indVar) {
		// +++++++++++++++++ CLIPS NO++++++++++++++++++++++++++++++ 
	case _clip_plus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_nbClips[pg_ind_scenario] > 0) {
				for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
					pg_play_clip_no(indClipRank, clipSide, (pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) + 1) % pg_nbClips[pg_ind_scenario]);
				}
			}
		}
		break;
	case _clip_minus:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_nbClips[pg_ind_scenario] > 0) {
				for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
					pg_play_clip_no(indClipRank, clipSide, (pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) - 1 + pg_nbClips[pg_ind_scenario]) % pg_nbClips[pg_ind_scenario]);
				}
			}
		}
		break;
		// +++++++++++++++++ CLIP FWD/BWD ++++++++++++++++++++++++++++++ 
	case _clip_forward:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0) {
					pg_all_clip_status[clipSide].play_offset_position(indClipRank, +0.1);
				}
			}
		}
		break;
	case _clip_backward:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0) {
					pg_all_clip_status[clipSide].play_offset_position(indClipRank, -0.1);
				}
			}
		}
		break;
		// +++++++++++++++++ CLIP NUDGE AND SCRATCH ++++++++++++++++++ 
		// jog wheel controls with two arguments
		// x: position of the wheel on the circle (0 at 0 hour and 1 at 12 hours)
		// y: 1 for clockwise rotation, 0 for anticlockwise rotation

		// SCRATCHING
	case _clip_scratch:
#if defined(var_clip_scratch_factor3)
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clip_scratch_factor3]) {
			float angle_pos = 0.f;
			float prec_angle_pos = 0.f;
			double prec_angle_time = 0.;
			bool is_forward = true;
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				if (nb_arguments >= 1) {
					angle_pos = float_arguments[1];
				}
				if (nb_arguments >= 2) {
					is_forward = (float_arguments[2] > 0.);
				}
				prec_angle_pos = pg_all_clip_status[clipSide].get_last_scratch_angle(&prec_angle_time);
				// it is not the first interaction after a release
				if (prec_angle_pos >= 0) {
					float delta_angle = angle_pos - prec_angle_pos;
					// if we go over noon, the angle difference should be increased by 1
					// for clockwise: e.g. angle_pos = 0.1 prec_angle_pos = 0.9 delta_angle = (0.1 -  0.9) + 1. = 0.2
					// otherwise delta_angle = 0.9 - 0.7 = 0.2
					if (is_forward && delta_angle < -0.5f) {
						delta_angle = delta_angle + 1.f;
					}
					// for anti-clockwise: e.g. angle_pos = 0.9 prec_angle_pos = 0.1 delta_angle = (0.9 -  0.1) - 1. = -0.2
					// otherwise delta_angle = 0.7 - 0.9 = -0.2
					else if (!is_forward && delta_angle > 0.5f) {
						delta_angle = delta_angle - 1.f;
					}
					pg_all_clip_status[clipSide].scratch_offset_position(delta_angle * clip_scratch_factor);
				}
				else {
					// pas d'incidence sur le jeu
				}
				pg_all_clip_status[clipSide].set_last_scratch_angle(angle_pos, pg_CurrentClockTime);
			}
		}
#endif
		break;
	case _clip_scratch_touch_press:
#if defined(var_clip_scratch_factor3)
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clip_scratch_factor3]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				pg_all_clip_status[clipSide].set_last_scratch_angle(-1., pg_CurrentClockTime);
				pg_all_clip_status[clipSide].set_scratch_pressed(true);
			}
		}
#endif
		break;
	case _clip_scratch_touch_release:
#if defined(var_clip_scratch_factor3)
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clip_scratch_factor3]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				pg_all_clip_status[clipSide].set_last_scratch_angle(-1., pg_CurrentClockTime);
				pg_all_clip_status[clipSide].set_scratch_pressed(false);
			}
		}
#endif
		break;
		// CLIP TEMPO BENDING
	case _clip_nudge:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_clip_nudge_factor]) {
			int clipSide = int(float_arguments[0]);
			for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank) >= 0) {
					float fps = clipCaptFreq;
					float mult_factor = 1.f;
					if (float_arguments[1] > 0.5f) {
						mult_factor = (1.f + (float_arguments[1] - 0.5f) * clip_nudge_factor);
					}
					else if (float_arguments[1] >= 0.f) {
						float div_factor = (1.f + (0.5f - float_arguments[1]) * clip_nudge_factor);
						if (div_factor != 0) {
							mult_factor = 1.f / div_factor;
						}
					}
					pg_all_clip_status[clipSide].set_currentFPS(fps * mult_factor);
				}
			}
		}
		break;
	case _clip_nudge_touch_press:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_clip_nudge_factor]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				pg_all_clip_status[clipSide].set_last_nudge_angle(-1., pg_CurrentClockTime);
			}
		}
		break;
	case _clip_nudge_touch_release:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]
			&& pg_FullScenarioActiveVars[pg_ind_scenario][_clip_nudge_factor]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				pg_all_clip_status[clipSide].set_last_nudge_angle(-1., pg_CurrentClockTime);
			}
		}
		break;
		// CLIP CUE MANAGEMENT
	case _clip_cue_onOff:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
					int indClip = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
					int indCue = int(float_arguments[1]);
					if (indClip >= 0 && indClip < pg_nbClips[pg_ind_scenario]) {
						int cueFrame = pg_clip_tracks[pg_ind_scenario][indClip].set_cue_onOff(indCue, pg_all_clip_status[clipSide].get_lastFrame(indClipRank));
						// cue on
						if (cueFrame >= 0) {
							sprintf(pg_AuxString, "/clip_cue_onOff_%d_%d_color 1. 0. 0.", clipSide, indCue);
							pg_send_message_udp((char*)"fff", pg_AuxString, (char*)"udp_TouchOSC_send");
							sprintf(pg_AuxString, "/clip_cue_onOff_%d_%d 1.", clipSide, indCue);
							pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						}
						// cue off
						else {
							sprintf(pg_AuxString, "/clip_cue_onOff_%d_%d_color 0. 1. 1.", clipSide, indCue);
							pg_send_message_udp((char*)"fff", pg_AuxString, (char*)"udp_TouchOSC_send");
							sprintf(pg_AuxString, "/clip_cue_onOff_%d_%d 0.", clipSide, indCue);
							pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						}
					}
				}
			}
		}
		break;

		// CLIP GOTO CUE: makes a difference whether or not the clip is playing (jump vs call)
	case _clip_cue_jump:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				int indClip = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(0);
				//printf("cue jump indclip %d id=nd cue %d\n", indClip, int(float_arguments[1]));
				if (indClip >= 0 && indClip < pg_nbClips[pg_ind_scenario]) {
					int cue_frame = pg_clip_tracks[pg_ind_scenario][indClip].get_cue(int(float_arguments[1]));
					//printf("cue jump frame %d/%d\n", cue_frame, pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames());
					if (cue_frame >= 0 && cue_frame < pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames()) {
						pg_all_clip_status[clipSide].set_position(0, cue_frame);
					}
				}
			}
		}
		break;
#if PG_NB_PARALLEL_CLIPS >= 2
	case _clip2_cue_jump:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				int indClip = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(1);
				//printf("cue jump indclip %d id=nd cue %d\n", indClip, int(float_arguments[1]));
				if (indClip >= 0 && indClip < pg_nbClips[pg_ind_scenario]) {
					int cue_frame = pg_clip_tracks[pg_ind_scenario][indClip].get_cue(int(float_arguments[1]));
					//printf("cue jump frame %d/%d\n", cue_frame, pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames());
					if (cue_frame >= 0 && cue_frame < pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames()) {
						pg_all_clip_status[clipSide].set_position(1, cue_frame);
					}
				}
			}
		}
		break;
#endif
		// CLIP GOTO CUE: makes a difference whether or not the clip is playing (jump vs call)
	case _clip_cue_call:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			int clipCueNo = int(float_arguments[1]);
			bool onOff = (float_arguments[2] == 1.f);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				for (int indClipRank = 0; indClipRank < PG_NB_PARALLEL_CLIPS; indClipRank++) {
					int indClip = pg_all_clip_status[clipSide].getCurrentlyPlaying_clipNo(indClipRank);
					//printf("cue jump indclip %d id=nd cue %d\n", indClip, int(float_arguments[1]));
					if (indClip >= 0 && indClip < pg_nbClips[pg_ind_scenario]) {
						int cue_frame = pg_clip_tracks[pg_ind_scenario][indClip].get_cue(int(float_arguments[1]));
						//printf("cue jump frame %d/%d\n", cue_frame, pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames());
						if (cue_frame >= 0 && cue_frame < pg_clip_tracks[pg_ind_scenario][indClip].get_initialNbFrames()) {
							//if (onOff) {
							pg_all_clip_status[clipSide].set_position(indClipRank, cue_frame);
							//	pg_all_clip_status[clipSide].clip_play[indClipRank] = true;
							//}
							//else {
							//	pg_all_clip_status[clipSide].clip_play[indClipRank] = false;
							//}
						}
					}
				}
			}
		}
		break;
		// CLIP: FX
	case _clip_fx:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			int fxNo = int(float_arguments[1]);
			if (clipSide == pg_enum_clipLeft) {
				switch (fxNo) {
				case 0:
					pg_fx_dry_wet = float_arguments[2];
					// dry_wet
					break;
				case 1: {
					float val = float_arguments[2];
					if (val < 1.f / 3.f) {
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[1] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[1] = 1.f;
					}
					else if (val < 2.f / 3.f) {
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[1] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[1] = 1.f;
					}
					else {
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[0] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[0] = 1.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_b_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_b_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_g_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipRight].clip_g_channel_level[1] = 0.f;
						pg_all_clip_status[pg_enum_clipLeft].clip_r_channel_level[1] = 1.f;
						pg_all_clip_status[pg_enum_clipRight].clip_r_channel_level[1] = 1.f;
					}
				}
					  break;
				case 2:
					echo = float_arguments[2];
					*((float*)pg_FullScenarioVarPointers[_echo]) = echo * pg_fx_dry_wet;
					break;
				case 3:
					photo_gamma = float_arguments[2];
					*((float*)pg_FullScenarioVarPointers[_photo_gamma]) = photo_gamma * pg_fx_dry_wet;
					break;
				}
			}
			else {
				switch (fxNo) {
				case 0:
					pg_fx_dry_wet = float_arguments[2];
					// dry_wet
					break;
				case 1:
					flashTrkBG_freq[2] = max(0, min(9, int(floor(float_arguments[2] * 10 * pg_fx_dry_wet))));
					((int*)pg_FullScenarioVarPointers[_flashTrkBG_freq])[2] = flashTrkBG_freq[2];
					break;
				case 2:
					photo_satur = float_arguments[2];
					*((float*)pg_FullScenarioVarPointers[_photo_satur]) = photo_satur * pg_fx_dry_wet;
					break;
				case 3:
					pg_all_clip_status[pg_enum_clipLeft].jump_frame_freq = int(float_arguments[2] * 10 * pg_fx_dry_wet);
					pg_all_clip_status[pg_enum_clipRight].jump_frame_freq = int(float_arguments[2] * 10 * pg_fx_dry_wet);
					break;
				}
			}
		}
		break;
		// CLIP FX
	case _clip_fx_std:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int fxNo = int(float_arguments[0]);
			switch (fxNo) {
			case 0:
				repop_part = 1.f - repop_part;
				*((float*)pg_FullScenarioVarPointers[_repop_part]) = repop_part;
				break;
			case 1:
				pixel_radius = 1.f - pixel_radius;
				*((float*)pg_FullScenarioVarPointers[_pixel_radius]) = pixel_radius;
				break;
			case 2:
				pixel_acc = 0.0001f;
				*((float*)pg_FullScenarioVarPointers[_pixel_acc]) = pixel_acc;
				break;
			case 3:
				CAdecay = 0.f;
				*((float*)pg_FullScenarioVarPointers[_CAdecay]) = CAdecay;
				break;
			case 4:
				part_size_pulse = 0.5f;
				*((float*)pg_FullScenarioVarPointers[_part_size_pulse]) = part_size_pulse;
				break;
			}
		}
		break;
		// CLIP EQ
	case _clip_equalizer:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (nb_arguments == 3) {
				int eqNo = int(float_arguments[1]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
					switch (eqNo) {
					case 0:
						pg_all_clip_status[clipSide].clip_r_channel_level[0] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_red %.2f", pg_all_clip_status[clipSide].clip_r_channel_level[0]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					case 1:
						pg_all_clip_status[clipSide].clip_g_channel_level[0] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_green %.2f", pg_all_clip_status[clipSide].clip_g_channel_level[0]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					case 2:
						pg_all_clip_status[clipSide].clip_b_channel_level[0] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_blue %.2f", pg_all_clip_status[clipSide].clip_b_channel_level[0]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					}
				}
			}
			else if (nb_arguments == 4) {
				int clipSide = int(float_arguments[0]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
					pg_all_clip_status[clipSide].clip_r_channel_level[0] = float_arguments[1];
					pg_all_clip_status[clipSide].clip_g_channel_level[0] = float_arguments[2];
					pg_all_clip_status[clipSide].clip_b_channel_level[0] = float_arguments[3];
					sprintf(pg_AuxString, "/clip_eq_red %.2f", pg_all_clip_status[clipSide].clip_r_channel_level[0]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip_eq_green %.2f", pg_all_clip_status[clipSide].clip_g_channel_level[0]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip_eq_blue %.2f", pg_all_clip_status[clipSide].clip_b_channel_level[0]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
#if PG_NB_PARALLEL_CLIPS >= 2
		// CLIP EQ
	case _clip2_equalizer:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (nb_arguments == 3) {
				int eqNo = int(float_arguments[1]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
					switch (eqNo) {
					case 0:
						pg_all_clip_status[clipSide].clip_r_channel_level[1] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_red %.2f", pg_all_clip_status[clipSide].clip_r_channel_level[1]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					case 1:
						pg_all_clip_status[clipSide].clip_g_channel_level[1] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_green %.2f", pg_all_clip_status[clipSide].clip_g_channel_level[1]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					case 2:
						pg_all_clip_status[clipSide].clip_b_channel_level[1] = float_arguments[2];
						sprintf(pg_AuxString, "/clip_eq_blue %.2f", pg_all_clip_status[clipSide].clip_b_channel_level[1]);
						pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
						break;
					}
				}
			}
			else if (nb_arguments == 4) {
				int clipSide = int(float_arguments[0]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
					pg_all_clip_status[clipSide].clip_r_channel_level[1] = float_arguments[1];
					pg_all_clip_status[clipSide].clip_g_channel_level[1] = float_arguments[2];
					pg_all_clip_status[clipSide].clip_b_channel_level[1] = float_arguments[3];
					sprintf(pg_AuxString, "/clip_eq_red %.2f", pg_all_clip_status[clipSide].clip_r_channel_level[1]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip_eq_green %.2f", pg_all_clip_status[clipSide].clip_g_channel_level[1]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
					sprintf(pg_AuxString, "/clip_eq_blue %.2f", pg_all_clip_status[clipSide].clip_b_channel_level[1]);
					pg_send_message_udp((char*)"f", pg_AuxString, (char*)"udp_TouchOSC_send");
				}
			}
		}
		break;
#endif
		// CLIP: NEW RANDOM CLIP
	case _clip_new:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			pg_clip_new(0, clipSide, nb_arguments, float_arguments);
		}
		break;
#if PG_NB_PARALLEL_CLIPS >= 2
	case _clip2_new:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			pg_clip_new(1, clipSide, nb_arguments, float_arguments);
		}
		break;
#endif
		// CLIP: NEW SELECTED CLIP
	case _clip_sample:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			int clip_no = int(float_arguments[1]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(0, clipSide, clip_no);

			}
		}
		break;
		// CLIP: CLIP IS SELECTED IN A RANGE (BEGIN AND END OF RANGE ARE INCLUDED)
	case _clip_sample_range:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			int clipRank = int(float_arguments[1]);
			int clip_no_low = int(float_arguments[2]);
			int clip_no_high = int(float_arguments[3]);
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		break;
		// CLIP: NEW SELECTED CLIP IN CASE OF TWO DECKS ON EACH SIDE
	case _clip2_sample:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				int clipSide = int(float_arguments[0]);
				int clip_no = int(float_arguments[1]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
					clip_no = clip_no % pg_nbClips[pg_ind_scenario];
					pg_play_clip_no(1, clipSide, clip_no);

				}
			}
		}
		break;
		// CLIP: LEFT OR RIGHT CLIP FADE IN/OUT
	case _clip_fader:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			int clipSide = int(float_arguments[0]);
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
				// pg_all_clip_status[clipSide].clip_level[0] = float_arguments[1];
			}
		}
		break;
	case _clip2_fader:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				int clipSide = int(float_arguments[0]);
				if (clipSide < pg_enum_clipLeftRight && clipSide >= 0) {
					// pg_all_clip_status[clipSide].clip_level[1] = float_arguments[1];
				}
			}
		}
		break;
		// CLIP PLAY STOP
	case _clip_autoplay_left:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			pg_all_clip_status[pg_enum_clipLeft].clip_autoplay[0] = (float_arguments[0] == 1.f);
		}
		break;
	case _clip_autoplay_right:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			pg_all_clip_status[pg_enum_clipRight].clip_autoplay[0] = (float_arguments[0] == 1.f);
		}
		break;
	case _clip_play_left:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			pg_all_clip_status[pg_enum_clipLeft].clip_play[0] = (float_arguments[0] == 1.f);
			if (pg_all_clip_status[pg_enum_clipLeft].clip_play[0]) {
				pg_all_clip_status[pg_enum_clipLeft].set_lastPlayedFrameTime(0, pg_CurrentClockTime);
			}
		}
		break;
	case _clip_play_right:
		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
			pg_all_clip_status[pg_enum_clipRight].clip_play[0] = (float_arguments[0] == 1.f);
			if (pg_all_clip_status[pg_enum_clipRight].clip_play[0]) {
				pg_all_clip_status[pg_enum_clipRight].set_lastPlayedFrameTime(0, pg_CurrentClockTime);
			}
		}
		break;
	case _clip2_autoplay_left:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				pg_all_clip_status[pg_enum_clipLeft].clip_autoplay[1] = (float_arguments[0] == 1.f);
			}
		}
		break;
	case _clip2_autoplay_right:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				pg_all_clip_status[pg_enum_clipRight].clip_autoplay[1] = (float_arguments[0] == 1.f);
			}
		}
		break;
	case _clip2_play_left:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				pg_all_clip_status[pg_enum_clipLeft].clip_play[1] = (float_arguments[0] == 1.f);
				if (pg_all_clip_status[pg_enum_clipLeft].clip_play[1]) {
					pg_all_clip_status[pg_enum_clipLeft].set_lastPlayedFrameTime(1, pg_CurrentClockTime);
				}
			}
		}
		break;
	case _clip2_play_right:
		if (PG_NB_PARALLEL_CLIPS >= 2) {
			if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
				pg_all_clip_status[pg_enum_clipRight].clip_play[1] = (float_arguments[0] == 1.f);
				if (pg_all_clip_status[pg_enum_clipRight].clip_play[1]) {
					pg_all_clip_status[pg_enum_clipRight].set_lastPlayedFrameTime(1, pg_CurrentClockTime);
				}
			}
		}
		break;
	default:
		sprintf(pg_errorStr, "Clip command not found (%s)!", address_string.c_str()); pg_ReportError(pg_errorStr);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PHOTO CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		//printf("photo_diaporama_callBack %d\n", scenario_or_gui_command_value);
		if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0 && scenario_or_gui_command_value >= 0) {
			photo_diaporama = int(scenario_or_gui_command_value) % pg_nbCompressedImageDirs[pg_ind_scenario];
			//printf("new photo_diaporama %d\n", photo_diaporama);
			sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		else if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0 && scenario_or_gui_command_value < 0) {
			photo_diaporama = -1;
			//printf("photo_diaporama %d\n", photo_diaporama);
			sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
		// do nothing / no images to display
		else {
		}

		// photo diaporama initialization
		//printf("current photo diaporama %d\n", scenario_or_gui_command_value);
		ExclusiveButtonsAndLabelsOnOff<int>(pg_FondButtonsPaths, pg_FondButtonLabelsPaths, pg_FondButtonValues, true, scenario_or_gui_command_value);
	}
}
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value) {
	if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_KEYSTROKE || param_input_type == pg_enum_PG_SCENARIO) {
		if (scenario_or_gui_command_value > 0.) {
			pg_flashPhotoTrk_decay = 1.f / scenario_or_gui_command_value;
		}
		else {
			pg_flashPhotoTrk_decay = 0.001f;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIP CALLBACKS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void playing_clipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					//printf("clip ID (%s)\n", pg_clip_tracks[pg_ind_scenario][ind_clip].get_name().c_str());
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_clipNoLeft = ind_clip;
						printf("playing left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, pg_enum_clipLeft, pg_playing_clipNoLeft);
						return;
					}
				}
				printf("Left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing left cLip [NULL]\n");
			}
			pg_playing_clipNoLeft = -1;
			pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoLeft);
			sprintf(pg_AuxString, "/clip_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void playing_clipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_clipNoRight = ind_clip;
						printf("playing right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(0, pg_enum_clipRight, pg_playing_clipNoRight);
						return;
					}
				}
				printf("Right cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing right cLip [NULL]\n");
			}
			pg_playing_clipNoRight = -1;
			pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(0, pg_playing_clipNoRight);
			sprintf(pg_AuxString, "/clip_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}
void playing_secondClipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq] && PG_NB_PARALLEL_CLIPS >= 2) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_secondClipNoLeft = ind_clip;
						printf("playing second left clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, pg_enum_clipLeft, pg_playing_secondClipNoLeft);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			pg_playing_secondClipNoLeft = -1;
			pg_all_clip_status[pg_enum_clipLeft].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoLeft);
			sprintf(pg_AuxString, "/clip2_shortName_0 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void playing_secondClipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq] && PG_NB_PARALLEL_CLIPS >= 2) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			if (scenario_or_gui_command_value != "NULL" && scenario_or_gui_command_value != "") {
				for (int ind_clip = 0; ind_clip < pg_nbClips[pg_ind_scenario]; ind_clip++) {
					if (pg_clip_tracks[pg_ind_scenario][ind_clip].get_name() == scenario_or_gui_command_value) {
						// starts playing another clip
						//printf("Start playing clip \n");
						pg_playing_secondClipNoRight = ind_clip;
						printf("playing second right clip name %s\n", scenario_or_gui_command_value.c_str());
						pg_play_clip_no(1, pg_enum_clipRight, pg_playing_secondClipNoRight);
						return;
					}
				}
				printf("Second left cLip not found [%s]\n", scenario_or_gui_command_value.c_str());
			}
			else {
				//printf("Stop playing second left cLip [NULL]\n");
			}
			pg_playing_secondClipNoRight = -1;
			pg_all_clip_status[pg_enum_clipRight].setCurrentlyPlaying_clipNo(1, pg_playing_secondClipNoRight);
			sprintf(pg_AuxString, "/clip2_shortName_1 %s", "---");
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
		}
	}
}

void Contact_clip_in_range_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value) {
#if defined(var_Contact_clip_in_range)
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_clipCaptFreq]) {
		if (param_input_type == pg_enum_PG_GUI_COMMAND || param_input_type == pg_enum_PG_SCENARIO) {
			ContAct_clip_ranges_min.clear();
			ContAct_clip_ranges_max.clear();
			if (scenario_or_gui_command_value != "NULL") {
				std::vector<std::string> ContAct_clip_ranges;
				ContAct_clip_ranges = pg_split_string(scenario_or_gui_command_value, '/');
				for (string& range : ContAct_clip_ranges) {
					std::vector<std::string> clip_min_max;
					clip_min_max = pg_split_string(range, '-');
					ContAct_clip_ranges_min.push_back(stoi(clip_min_max[0]));
					ContAct_clip_ranges_max.push_back(stoi(clip_min_max[1]));
				}
			}
		}
	}
#endif
}
