/*! \file pg-flash.cpp
 *
 *
 *     File pg-flash.cpp
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

#include "pg-all_include.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARS
//////////////////////////////////////////////////////////////////////////////////////////////////////

 // ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: Trk->CA -> values passed to the shader
// as a function of on/off values and weights
float pg_flashTrkCA_weights[PG_NB_TRACKS];
int pg_flashTrkCA_weights_duration[PG_NB_TRACKS];
// flash: Trk->Part -> values passed to the shader
// as a function of on/off values and weights
float pg_flashTrkPart_weights[PG_NB_TRACKS];
int pg_flashTrkPart_weights_duration[PG_NB_TRACKS];
// flash: Trk->BG   -> values passed to the shader
// as a function of on/off values and weights
// only tracks 1 and 2 are concerned since it is a projection on track0
float pg_flashTrkBG_weights[PG_NB_TRACKS];
int pg_flashTrkBG_weights_duration[PG_NB_TRACKS];
// flash: CA->BG
float pg_flashCABG_weight;
int pg_flashCABG_weight_duration;
// flash: CA->Part
float pg_flashCAPart_weight;
int pg_flashCAPart_weight_duration;
// flash: Part->BG
float pg_flashPartBG_weight;
int pg_flashPartBG_weight_duration;
// flash: Part->CA
float pg_flashPartCA_weight;
int pg_flashPartCA_weight_duration;
// flash: extension of pixel reproduction  
int pg_flashPixel;
// one-frame master flashing  
int pg_flashMaster;

int pg_flashTrkCA_freq_saved[PG_NB_TRACKS] = { 0 };
int pg_flashTrkPart_freq_saved[PG_NB_TRACKS] = { 0 };
int pg_flashTrkBG_freq_saved[PG_NB_TRACKS] = { 0 };

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
bool pg_is_flashCameraTrk = false;
float pg_flashCameraTrk_weight = 0.0f;
float pg_flashCameraTrk_decay = 1.0f;
float pg_flashCameraTrk_threshold = 0.5f;
bool pg_is_flashPhotoTrk = false;
float pg_flashPhotoTrk_weight = 0.0f;
float pg_flashPhotoTrk_decay = 1.0f;
float pg_flashPhotoTrk_threshold = 0.5f;
int pg_flashPhotoTrk_nbFrames = 0;

// +++++++++++++++++++++ BEAT SYNCHRONIZED RECORD/REPLAY +++++++++++++++++++++
bool pg_synchr_start_recording_path[PG_NB_PATHS + 1];
int pg_synchr_start_path_replay_trackNo[PG_NB_PATHS + 1];

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FLASH TRINGGERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

// flash triggering according to beat & frequency, weight, and on/off values 
// ======================================== 
bool pg_flash_beat_generation(int flash_frequency) {
	// flashes are synchronized on beats according to their frequency
	if (flash_frequency > 0
		&& flash_frequency <= PG_LOOP_SIZE
		&& (pg_BeatNo % (PG_LOOP_SIZE / flash_frequency)) == 0) {
		return true;
	}
	return false;
}
bool pg_flash_continuous_generation(int flash_frequency) {
	// flashes are made every frame if frequency is PG_LOOP_SIZE + 1
	//printf("peak %d\n", int(pg_movie_sound_peak));
	if (flash_frequency == (PG_LOOP_SIZE + 1)) {
		return true;
	}
	// flashes are made every video soundtrack peak (sound volume 1) if frequency is PG_LOOP_SIZE + 2
	else if (flash_frequency == (PG_LOOP_SIZE + 2) && (pg_movie_sound_peak || pg_soundTrack_peak)) {
		//printf("peak %.5f left %d\n", pg_CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	// flashes are made every video soundtrack onset detected by aubio library if frequency is PG_LOOP_SIZE + 3
	else if (flash_frequency == (PG_LOOP_SIZE + 3) && (pg_movie_sound_onset || pg_soundTrack_onset)) {
		//printf("onset %.5f left %d\n", pg_CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	return false;
}

void pg_flash_control(bool (*control_function)(int)) {
	if ((*control_function)(flashPixel_freq)) {
		pg_flashPixel = flashPixel_duration;
	}

	if ((*control_function)(flashPixel_freq)) {
		pg_flashPixel = flashPixel_duration;
	}

	if ((*control_function)(flashParticleInit_freq)) {
		part_initialization = (unsigned int)(floor(rand_0_1 * pg_particle_initial_pos_speed_texID[pg_ind_scenario].size())) % pg_particle_initial_pos_speed_texID[pg_ind_scenario].size();
		pg_ParticleTargetFrameNo = pg_FrameNo + int(part_timeToTargt);
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashTrkCA_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkCA_freq[indTrack])) {
				pg_flashTrkCA_weights[indTrack] = 1.0;
				pg_flashTrkCA_weights_duration[indTrack] = 1;
				// printf( "flashTrkCA %d (%.2f)\n" , indTrack, pg_flashTrkCA_weights[indTrack] );
			}
		}
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashTrkPart_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkPart_freq[indTrack])) {
				pg_flashTrkPart_weights[indTrack] = 1.0;
				pg_flashTrkPart_weights_duration[indTrack] = 1;
				// printf( "flashTrkPart %d (%.2f)\n" , indTrack, pg_flashTrkPart_weights[indTrack] );
			}
		}
	}
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashTrkBG_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkBG_freq[indTrack])) {
				// should only be flashed every second frame because of the way the pixels are spreaded
				// otherwise the pixels are not emitted
				if (control_function != pg_flash_continuous_generation || pg_FrameNo % 2 == 0) {
					pg_flashTrkBG_weights[indTrack] = 1.0;
					pg_flashTrkBG_weights_duration[indTrack] = 1;
					//printf("flashTrkBG_freq_%d\n", indTrack);
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashCABG_freq]) {
		if ((*control_function)(flashCABG_freq)) {
			pg_flashCABG_weight = 1.0;
			pg_flashCABG_weight_duration = 1;
			// printf( "flashCABG_freq (%d)\n" , flashCABG_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashCAPart_freq]) {
		if ((*control_function)(flashCAPart_freq)) {
			pg_flashCAPart_weight = 1.0;
			pg_flashCAPart_weight_duration = 1;
			// printf( "flashCAPart_freq (%d)\n" , flashCAPart_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPartBG_freq]) {
		if ((*control_function)(flashPartBG_freq)) {
			pg_flashPartBG_weight = 1.0;
			pg_flashPartBG_weight_duration = 1;
			// printf( "flashPartBG_freq (%d)\n" , flashPartBG_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPartCA_freq]) {
		if ((*control_function)(flashPartCA_freq)) {
			pg_flashPartCA_weight = 1.0;
			pg_flashPartCA_weight_duration = 1;
			// printf( "flashPartCA_freq (%d)\n" , flashPartCA_freq );
		}
	}

	//std::vector<int> pg_clip_ranges_min;
	//std::vector<int> pg_clip_ranges_max;
	int clip_no_low = 0;
	int clip_no_high = pg_nbClips[pg_ind_scenario] - 1;

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashchangeClipLeft_freq]) {

		if (pg_FullScenarioActiveVars[pg_ind_scenario][_clip_in_range]) {
			if (pg_clip_ranges_min.size() > 0 && pg_clip_ranges_min.size() == pg_clip_ranges_max.size()) {
				// CLIP IS SELECTED IN A RANGE (BEGIN AND END OF RANGE ARE INCLUDED)
				int range_selected = int(rand_0_1 * pg_clip_ranges_min.size()) % pg_clip_ranges_min.size();
				int clip_no_low = pg_clip_ranges_min[range_selected];
				int clip_no_high = pg_clip_ranges_max[range_selected];
				int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
				clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			}
		}

		if ((*control_function)(flashchangeClipLeft_freq)) {
			int clipSide = pg_enum_clipLeft;
			int clipRank = 0;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(flashchangeClip2Left_freq)) {
			int clipSide = pg_enum_clipLeft;
			int clipRank = 1;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(flashchangeClipRight_freq)) {
			int clipSide = pg_enum_clipRight;
			int clipRank = 0;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(flashchangeClip2Right_freq)) {
			int clipSide = pg_enum_clipRight;
			int clipRank = 1;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_ind_scenario];
			if (clipSide < pg_enum_clipLeftRight && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_ind_scenario] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_ind_scenario];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(flashchangeScreenLayout_freq)) {
			// screen layout is between O and 5
			// O: Left / 1: RIght
			// 2: L+R full size / 3: L+R half size side by side
			// 4: 4 screens / 5: L+R half size on top of each other
			int layout = int(rand_0_1 * 6) % 6;
			(*((int*)pg_FullScenarioVarPointers[_screenLayout])) = layout;
			screenLayout = layout;
		}
		if ((*control_function)(flashchange_mesh_palette_freq)) {
			int level = int(rand_0_1 * 9) % 9;
			(*((int*)pg_FullScenarioVarPointers[_mesh_palette])) = level;
			mesh_palette = level;
		}
		if ((*control_function)(flashchange_mesh_anime_freq)) {
			int anime = int(rand_0_1 * (pg_Mesh_Animations[pg_ind_scenario][0].pg_nb_MaxAnimationPoses + 1)) % (pg_Mesh_Animations[pg_ind_scenario][0].pg_nb_MaxAnimationPoses + 1);
			(*((int*)pg_FullScenarioVarPointers[_mesh_anime])) = anime;
			mesh_anime = anime;
		}
		if ((*control_function)(flashchange_mesh_motion_freq)) {
			int motion = int(rand_0_1 * (pg_Mesh_Animations[pg_ind_scenario][0].pg_nb_MaxAnimationPoses + 1)) % (pg_Mesh_Animations[pg_ind_scenario][0].pg_nb_MaxAnimationPoses + 1);
			(*((int*)pg_FullScenarioVarPointers[_mesh_motion])) = motion;
			mesh_motion = motion;
		}
		bool fixedLeftPose = false;
		bool fixedRightPose = false;
		if ((*control_function)(flashchangeRightHandPose_freq)) {
			fixedRightPose = true;
		}
		if ((*control_function)(flashchangeLeftHandPose_freq)) {
			fixedLeftPose = true;
		}
		if (fixedRightPose || fixedLeftPose) {
			int chosen_mesh_LibraryPose = -1;
			// bianry animation between two indentical poses
			(*((int*)pg_FullScenarioVarPointers[_mesh_anime])) = 1;
			mesh_anime = 1;
			// two poses are memorized: twice the randomly selected pose;
			for (int indMeshFile = 0; indMeshFile < std::min(2, int(pg_Meshes[pg_ind_scenario].size())); indMeshFile++) {
				if ((indMeshFile == 0 && fixedLeftPose) || (indMeshFile == 1 && fixedRightPose)) {
					if (chosen_mesh_LibraryPose == -1) {
						chosen_mesh_LibraryPose = int(rand_0_1 * pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses) % pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_LibraryPoses;
					}
					pg_copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose, 0);
					pg_copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose, 1);
					//printf("ind Mesh %d chosen library pose %d \n", indMeshFile, chosen_mesh_LibraryPose);
					// all the weights are set to zero except the first pose weight
					for (int indPose = 0; indPose < pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_MaxAnimationPoses; indPose++) {
						pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_interpolation_weight_AnimationPose[indPose] = 0.f;
					}
					pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_interpolation_weight_AnimationPose[0] = 1.f;
					pg_Mesh_Animations[pg_ind_scenario][indMeshFile].pg_nb_CurAnimationPoses = 2;
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashchange_invertAllLayers_freq]) {
		if ((*control_function)(flashchange_invertAllLayers_freq)) {
			invertAllLayers = !invertAllLayers;
			*((bool*)pg_FullScenarioVarPointers[_invertAllLayers]) = invertAllLayers;
		}
	}


	// random selection of an image in the list of available ones
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashchange_diaporama_freq]) {
		if ((*control_function)(flashchange_diaporama_freq)) {
			if (pg_nbCompressedImageDirs[pg_ind_scenario] > 0) {
				// goes to the first photo diaporama if it is not already selected and if there is one 
				if (photo_diaporama < 0 && pg_ImageDirectory[pg_ind_scenario] != "") {
					photo_diaporama = 0;
				}
				int cnt = 0;
				int new_dir = -1;
				do {
					new_dir = int(rand_0_1 * pg_nbCompressedImageDirs[pg_ind_scenario]) % pg_nbCompressedImageDirs[pg_ind_scenario];
				} while (cnt++ < 10 && (new_dir == photo_diaporama || new_dir == 0)); // 0 is Fatima hand
				photo_diaporama = new_dir;
				//printf("photo_diaporama %d\n", photo_diaporama);
				sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
				pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
				pg_launch_diaporama(0);
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashchange_BGcolor_freq]) {
		if ((*control_function)(flashchange_BGcolor_freq)) {
			float BG_hue = rand_0_1;
			pg_HSVtoRGB(BG_hue, 1.f, 1.f, &BGcolorRed, &BGcolorGreen, &BGcolorBlue);
			//printf("RGB %.2f -> %.2f %.2f %.2f \n", BG_hue, BGcolorRed, BGcolorGreen, BGcolorBlue);
		}
		else if (flashchange_BGcolor_freq == 0) {
			BGcolorRed = 0.f;
			BGcolorGreen = 0.f;
			BGcolorBlue = 0.f;
		}
	}

#if defined(var_flashchange_freeze_freq)
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashchange_freeze_freq]) {
		if ((*control_function)(flashchange_freeze_freq)) {
			freeze = !freeze;
			*((bool*)pg_FullScenarioVarPointers[_freeze]) = freeze;
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// BEAT RECEPTION AND ASSOCIATED FUNCTIONS TRIGGERING
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// beat reception and corresponding events triggering
void pg_receiveBeat(void) {
	sprintf(pg_AuxString, "/loop_beat %d", pg_BeatNo % PG_LOOP_SIZE); pg_send_message_udp((char*)"i", pg_AuxString, (char*)"udp_TouchOSC_send");

	// loop start 
	if ((pg_BeatNo % PG_LOOP_SIZE) == 0) {
		for (int ind = 1; ind <= PG_NB_PATHS; ind++) {
			// source track recording is on -> source track start 
			if (pg_synchr_start_recording_path[ind]) {
				pg_path_recording_onOff(ind);
				// sets to 0 the recording message since recording has started 
				pg_synchr_start_recording_path[ind] = false;
			}


			// track reading is on -> track read start 
			if (pg_synchr_start_path_replay_trackNo[ind] >= 0) {
				pg_path_replay_trackNo_onOff(ind, pg_synchr_start_path_replay_trackNo[ind]);
				pg_synchr_start_path_replay_trackNo[ind] = -1;
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashCameraTrkBeat]) {
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// camera flash
		if (pg_is_flashCameraTrk
			&& ((pg_BeatNo % PG_LOOP_SIZE) == flashCameraTrkBeat || flashCameraTrkBeat == 9)
			) {
			pg_is_flashCameraTrk = false;
			pg_flashCameraTrk_weight = 1.0f;
			pg_flashCameraTrk_threshold = std::min(std::max(0.0f, 1.0f - pg_flashCameraTrk_decay * flashCameraTrkBright), 1.0f);
			//printf("pg_flashCameraTrk_weight %.3f decay %.2f threshold %.3f\n", 
			//	pg_flashCameraTrk_weight , pg_flashCameraTrk_decay, pg_flashPhotoTrk_threshold);
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBeat]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBright]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkLength]
		&& pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoChangeBeat]) {
		// photo flash
		// weight is the current brightness of the lighting at each frame  until it reaches the threshold value and is then stopped 
		if (pg_is_flashPhotoTrk
			&& ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoTrkBeat || flashPhotoTrkBeat == 0)) {
			pg_is_flashPhotoTrk = false;
			pg_Make_flashPhoto();
		}
	}

	pg_flash_control(pg_flash_beat_generation);

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoChangeBeat]) {
		// photo change flash
		if (flashPhotoChangeBeat > 0 && ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoChangeBeat)) {
			// goes to the first photo diaporama if it is not already selected and if there is one 
			if (photo_diaporama < 0 && pg_ImageDirectory[pg_ind_scenario] != "") {
				photo_diaporama = 0;
			}
			// random selection and loopiing
			photo_diaporama = (photo_diaporama + int(rand_0_1 * pg_nbCompressedImageDirs[pg_ind_scenario])) % pg_nbCompressedImageDirs[pg_ind_scenario];
			//printf("photo_diaporama %d\n", photo_diaporama);

			sprintf(pg_AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", pg_AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_BGcolor]) {
		if (BGcolor) {
			BGcolorRed = rand_0_1;
			BGcolorGreen = rand_0_1;
			BGcolorBlue = rand_0_1;
			// color saturation
			float max_col = max(BGcolorRed, max(BGcolorGreen, BGcolorBlue));
			BGcolorRed += 1.f - max_col;
			BGcolorGreen += 1.f - max_col;
			BGcolorBlue += 1.f - max_col;
			// color debrillance
			float brillance = min(BGcolorRed, min(BGcolorGreen, BGcolorBlue));
			if (BGcolorRed == brillance) {
				BGcolorRed = 0.f;
			}
			else if (BGcolorRed != 1.f) {
				BGcolorRed -= brillance / 2.f;
			}
			if (BGcolorGreen == brillance) {
				BGcolorGreen = 0.f;
			}
			else if (BGcolorGreen != 1.f) {
				BGcolorGreen -= brillance / 2.f;
			}
			if (BGcolorBlue == brillance) {
				BGcolorBlue = 0.f;
			}
			else if (BGcolorBlue != 1.f) {
				BGcolorBlue -= brillance / 2.f;
			}
		}
		else {
			BGcolorRed = 0.f;
			BGcolorGreen = 0.f;
			BGcolorBlue = 0.f;
		}
	}

	// ONLY if DMX
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_light_color]) {
		for (int ind = 0; ind < pg_nb_light_groups[pg_ind_scenario]; ind++) {
			if (pg_light_groups[pg_ind_scenario][ind].get_group_hasBeatCommand()) {
				pg_light_groups[pg_ind_scenario][ind].update_beatCommand();
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_ind_scenario][_penStrokeAtBeat]) {
		if (penStrokeAtBeat) {
			pg_FourFrameStrokeNb = 5;
			pg_FourFrameStroke_x = int(640 + 2 * rand_0_1);
			pg_FourFrameStroke_y = int(180 + 2 * rand_0_1);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PHOTO AND CAMERA FLASH
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// photo flash launching
void pg_Make_flashPhoto(void) {
	if (pg_FullScenarioActiveVars[pg_ind_scenario][_flashPhotoTrkBright]) {
		pg_flashPhotoTrk_weight = 1.0f;
		pg_flashPhotoTrk_nbFrames = 0;
		pg_flashPhotoTrk_threshold = std::min(std::max(0.0f, 1.0f - pg_flashPhotoTrk_decay * flashPhotoTrkBright), 1.0f);
		// invertAllLayers = !invertAllLayers;
		//photo_scale = rand_0_1 * 0.5f + 0.5f;
		//photo_offsetX = rand_0_1 * 1.5f - 0.5f;
		//photo_offsetY = rand_0_1 * 1.5f - 0.5f;
		// photo_diaporama = int(floor(9 + rand_0_1 * 14.9));
		photo_diaporama = int(floor(rand_0_1 * pg_nbCompressedImageDirs[pg_ind_scenario])) % pg_nbCompressedImageDirs[pg_ind_scenario];
		pg_launch_diaporama(0);
		//printf("pg_flashPhotoTrk_weight %.3f decay %.2f threshold %.3f\n",
		//	pg_flashPhotoTrk_weight, pg_flashPhotoTrk_decay, pg_flashPhotoTrk_threshold);
	}
}

void pg_Make_flashCamera(void) {
	pg_is_flashCameraTrk = true;
	pg_flashCameraTrk_weight = 0.0f;
	// printf("start flash\n");
	//    break;
}

