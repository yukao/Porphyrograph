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

 // ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: Trk->CA -> values passed to the shader
// as a function of on/off values and weights
float flashTrkCA_weights[PG_NB_TRACKS];
int flashTrkCA_weights_duration[PG_NB_TRACKS];
// flash: Trk->Part -> values passed to the shader
// as a function of on/off values and weights
float flashTrkPart_weights[PG_NB_TRACKS];
int flashTrkPart_weights_duration[PG_NB_TRACKS];
// flash: Trk->BG   -> values passed to the shader
// as a function of on/off values and weights
// only tracks 1 and 2 are concerned since it is a projection on track0
float flashTrkBG_weights[PG_NB_TRACKS];
int flashTrkBG_weights_duration[PG_NB_TRACKS];
// flash: CA->BG
float flashCABG_weight;
int flashCABG_weight_duration;
// flash: CA->Part
float flashCAPart_weight;
int flashCAPart_weight_duration;
// flash: Part->BG
float flashPartBG_weight;
int flashPartBG_weight_duration;
// flash: Part->CA
float flashPartCA_weight;
int flashPartCA_weight_duration;
// flash: extension of pixel reproduction  
int flashPixel;
// one-frame master flashing  
int flashMaster;

int flashTrkCA_freq_saved[PG_NB_TRACKS] = { 0 };
int flashTrkPart_freq_saved[PG_NB_TRACKS] = { 0 };
int flashTrkBG_freq_saved[PG_NB_TRACKS] = { 0 };

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
bool is_flashCameraTrk = false;
float flashCameraTrk_weight = 0.0f;
float flashCameraTrk_decay = 1.0f;
float flashCameraTrk_threshold = 0.5f;
bool is_flashPhotoTrk = false;
float flashPhotoTrk_weight = 0.0f;
float flashPhotoTrk_decay = 1.0f;
float flashPhotoTrk_threshold = 0.5f;
int flashPhotoTrk_nbFrames = 0;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
// +++++++++++++++++ FLASHES +++++++++++++++++++++++++++++++ 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
// ======================================== 
// flash triggering according to beat & frequency, weight, and on/off values 
// ======================================== 
bool flash_beat_generation(int flash_frequency) {
	// flashes are synchronized on beats according to their frequency
	if (flash_frequency > 0
		&& flash_frequency <= PG_LOOP_SIZE
		&& (pg_BeatNo % (PG_LOOP_SIZE / flash_frequency)) == 0) {
		return true;
	}
	return false;
}
bool flash_continuous_generation(int flash_frequency) {
	// flashes are made every frame if frequency is PG_LOOP_SIZE + 1
	//printf("peak %d\n", int(pg_movie_sound_peak));
	if (flash_frequency == (PG_LOOP_SIZE + 1)) {
		return true;
	}
	// flashes are made every video soundtrack peak (sound volume 1) if frequency is PG_LOOP_SIZE + 2
	else if (flash_frequency == (PG_LOOP_SIZE + 2) && (pg_movie_sound_peak || pg_track_sound_peak)) {
		//printf("peak %.5f left %d\n", pg_CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	// flashes are made every video soundtrack onset detected by aubio library if frequency is PG_LOOP_SIZE + 3
	else if (flash_frequency == (PG_LOOP_SIZE + 3) && (pg_movie_sound_onset || pg_track_sound_onset)) {
		//printf("onset %.5f left %d\n", pg_CurrentClockTime - pg_movie_status.get_initialTime(), pg_movie_status.get_nbFramesLeft());
		return true;
	}
	return false;
}

void pg_flash_control(bool (*control_function)(int)) {
	if ((*control_function)(flashPixel_freq)) {
		flashPixel = flashPixel_duration;
	}

	if ((*control_function)(flashPixel_freq)) {
		flashPixel = flashPixel_duration;
	}

	if ((*control_function)(flashParticleInit_freq)) {
		part_initialization = (unsigned int)(floor(rand_0_1 * pg_particle_initial_pos_speed_texID[pg_current_configuration_rank].size())) % pg_particle_initial_pos_speed_texID[pg_current_configuration_rank].size();
		pg_ParticleTargetFrameNo = pg_FrameNo + int(part_timeToTargt);
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashTrkCA_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkCA_freq[indTrack])) {
				flashTrkCA_weights[indTrack] = 1.0;
				flashTrkCA_weights_duration[indTrack] = 1;
				// printf( "flashTrkCA %d (%.2f)\n" , indTrack, flashTrkCA_weights[indTrack] );
			}
		}
	}
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashTrkPart_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkPart_freq[indTrack])) {
				flashTrkPart_weights[indTrack] = 1.0;
				flashTrkPart_weights_duration[indTrack] = 1;
				// printf( "flashTrkPart %d (%.2f)\n" , indTrack, flashTrkPart_weights[indTrack] );
			}
		}
	}
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashTrkBG_freq]) {
		for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
			if ((*control_function)(flashTrkBG_freq[indTrack])) {
				// should only be flashed every second frame because of the way the pixels are spreaded
				// otherwise the pixels are not emitted
				if (control_function != flash_continuous_generation || pg_FrameNo % 2 == 0) {
					flashTrkBG_weights[indTrack] = 1.0;
					flashTrkBG_weights_duration[indTrack] = 1;
					//printf("flashTrkBG_freq_%d\n", indTrack);
				}
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashCABG_freq]) {
		if ((*control_function)(flashCABG_freq)) {
			flashCABG_weight = 1.0;
			flashCABG_weight_duration = 1;
			// printf( "flashCABG_freq (%d)\n" , flashCABG_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashCAPart_freq]) {
		if ((*control_function)(flashCAPart_freq)) {
			flashCAPart_weight = 1.0;
			flashCAPart_weight_duration = 1;
			// printf( "flashCAPart_freq (%d)\n" , flashCAPart_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPartBG_freq]) {
		if ((*control_function)(flashPartBG_freq)) {
			flashPartBG_weight = 1.0;
			flashPartBG_weight_duration = 1;
			// printf( "flashPartBG_freq (%d)\n" , flashPartBG_freq );
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPartCA_freq]) {
		if ((*control_function)(flashPartCA_freq)) {
			flashPartCA_weight = 1.0;
			flashPartCA_weight_duration = 1;
			// printf( "flashPartCA_freq (%d)\n" , flashPartCA_freq );
		}
	}

#if defined(var_Contact_flashchangeClipLeft_freq) && defined(var_Contact_flashchangeClip2Left_freq) \
			&& defined(var_Contact_flashchangeClipRight_freq) && defined(var_Contact_flashchangeClip2Right_freq) \
			&& defined(var_Contact_flashchangeScreenLayout_freq) && defined(var_Contact_flashchange_mesh_palette_freq) && defined(var_Contact_flashchange_mesh_anime_freq) \
			&& defined(var_Contact_flashchangeRightHandPose_freq) && defined(var_Contact_flashchangeLeftHandPose_freq)
	//std::vector<int> ContAct_clip_ranges_min;
	//std::vector<int> ContAct_clip_ranges_max;
	int clip_no_low = 0;
	int clip_no_high = pg_nbClips[pg_current_configuration_rank] - 1;

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_flashchangeClipLeft_freq]) {

#if defined(var_Contact_clip_in_range)
		if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_Contact_clip_in_range]) {
			if (ContAct_clip_ranges_min.size() > 0 && ContAct_clip_ranges_min.size() == ContAct_clip_ranges_max.size()) {
				// CLIP IS SELECTED IN A RANGE (BEGIN AND END OF RANGE ARE INCLUDED)
				int range_selected = int(rand_0_1 * ContAct_clip_ranges_min.size()) % ContAct_clip_ranges_min.size();
				int clip_no_low = ContAct_clip_ranges_min[range_selected];
				int clip_no_high = ContAct_clip_ranges_max[range_selected];
				int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
				clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_current_configuration_rank];
			}
		}
#endif

		if ((*control_function)(Contact_flashchangeClipLeft_freq)) {
			int clipSide = _clipLeft;
			int clipRank = 0;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_current_configuration_rank];
			if (clipSide < _clipLR && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_current_configuration_rank] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_current_configuration_rank];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(Contact_flashchangeClip2Left_freq)) {
			int clipSide = _clipLeft;
			int clipRank = 1;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_current_configuration_rank];
			if (clipSide < _clipLR && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_current_configuration_rank] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_current_configuration_rank];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(Contact_flashchangeClipRight_freq)) {
			int clipSide = _clipRight;
			int clipRank = 0;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_current_configuration_rank];
			if (clipSide < _clipLR && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_current_configuration_rank] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_current_configuration_rank];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(Contact_flashchangeClip2Right_freq)) {
			int clipSide = _clipRight;
			int clipRank = 1;
			int clip_no_low = 0;
			int clip_no_high = 26;
			int clip_no = clip_no_low + int(rand_0_1 * (clip_no_high - clip_no_low + 1)) % (clip_no_high - clip_no_low + 1);
			clip_no = max(min(clip_no, clip_no_high), clip_no_low) % pg_nbClips[pg_current_configuration_rank];
			if (clipSide < _clipLR && clipSide >= 0 && clipRank < PG_NB_PARALLEL_CLIPS && clipRank >= 0 && pg_nbClips[pg_current_configuration_rank] > 0 && clip_no >= 0) {
				clip_no = clip_no % pg_nbClips[pg_current_configuration_rank];
				pg_play_clip_no(clipRank, clipSide, clip_no);
			}
		}
		if ((*control_function)(Contact_flashchangeScreenLayout_freq)) {
			// screen layout is between O and 5
			// O: Left / 1: RIght
			// 2: L+R full size / 3: L+R half size side by side
			// 4: 4 screens / 5: L+R half size on top of each other
			int layout = int(rand_0_1 * 6) % 6;
			(*((int*)pg_FullScenarioVarPointers[_ContAct_screenLayout])) = layout;
			ContAct_screenLayout = layout;
		}
		if ((*control_function)(Contact_flashchange_mesh_palette_freq)) {
			int level = int(rand_0_1 * 9) % 9;
			(*((int*)pg_FullScenarioVarPointers[_Contact_mesh_palette])) = level;
			Contact_mesh_palette = level;
		}
		if ((*control_function)(Contact_flashchange_mesh_anime_freq)) {
			int anime = int(rand_0_1 * (_lastMesh_Anime + 1)) % (_lastMesh_Anime + 1);
			(*((int*)pg_FullScenarioVarPointers[_Contact_mesh_anime])) = anime;
			Contact_mesh_anime = anime;
		}
		if ((*control_function)(Contact_flashchange_mesh_motion_freq)) {
			int motion = int(rand_0_1 * (_lastMesh_Motion + 1)) % (_lastMesh_Motion + 1);
			(*((int*)pg_FullScenarioVarPointers[_Contact_mesh_motion])) = motion;
			Contact_mesh_motion = motion;
		}
		bool fixedLeftPose = false;
		bool fixedRightPose = false;
		if ((*control_function)(Contact_flashchangeRightHandPose_freq)) {
			fixedRightPose = true;
		}
		if ((*control_function)(Contact_flashchangeLeftHandPose_freq)) {
			fixedLeftPose = true;
		}
		if (fixedRightPose || fixedLeftPose) {
			int chosen_mesh_LibraryPose = -1;
			// bianry animation between two indentical poses
			(*((int*)pg_FullScenarioVarPointers[_Contact_mesh_anime])) = 1;
			Contact_mesh_anime = 1;
			// two poses are memorized: twice the randomly selected pose;
			for (int indMeshFile = 0; indMeshFile < min(2, pg_Meshes[pg_current_configuration_rank].size()); indMeshFile++) {
				if ((indMeshFile == 0 && fixedLeftPose) || (indMeshFile == 1 && fixedRightPose)) {
					if (chosen_mesh_LibraryPose == -1) {
						chosen_mesh_LibraryPose = int(rand_0_1 * pg_nb_LibraryPoses[indMeshFile]) % pg_nb_LibraryPoses[indMeshFile];
					}
					pg_nb_AnimationPoses[indMeshFile] = min(2, PG_MAX_ANIMATION_POSES);
					copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose, 0);
					copyLibraryPoseToAnimationPose(indMeshFile, chosen_mesh_LibraryPose, 1);
					//printf("ind Mesh %d chosen library pose %d \n", indMeshFile, chosen_mesh_LibraryPose);
					// all the weights are set to zero except the first pose weight
					for (int indPose = 0; indPose < pg_nb_AnimationPoses[indMeshFile]; indPose++) {
						pg_interpolation_weight_AnimationPose[indMeshFile][indPose] = 0.f;
					}
					pg_interpolation_weight_AnimationPose[indMeshFile][0] = 1.f;
				}
			}
		}
	}
#endif

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashchange_invertAllLayers_freq]) {
		if ((*control_function)(flashchange_invertAllLayers_freq)) {
			invertAllLayers = !invertAllLayers;
			*((bool*)pg_FullScenarioVarPointers[_invertAllLayers]) = invertAllLayers;
		}
	}


	// random selection of an image in the list of available ones
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashchange_diaporama_freq]) {
		if ((*control_function)(flashchange_diaporama_freq)) {
			if (pg_nbCompressedImageDirs[pg_current_configuration_rank] > 0) {
				// goes to the first photo diaporama if it is not already selected and if there is one 
				if (photo_diaporama < 0 && pg_ImageDirectory[pg_current_configuration_rank] != "") {
					photo_diaporama = 0;
				}
				int cnt = 0;
				int new_dir = -1;
				do {
					new_dir = int(rand_0_1 * pg_nbCompressedImageDirs[pg_current_configuration_rank]) % pg_nbCompressedImageDirs[pg_current_configuration_rank];
				} while (cnt++ < 10 && (new_dir == photo_diaporama || new_dir == 0)); // 0 is Fatima hand
				photo_diaporama = new_dir;
				//printf("photo_diaporama %d\n", photo_diaporama);
				sprintf(AuxString, "/diaporama_shortName %03d", photo_diaporama);
				pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
				pg_launch_diaporama(0);
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashchange_BGcolor_freq]) {
		if ((*control_function)(flashchange_BGcolor_freq)) {
			float BG_hue = rand_0_1;
			HSVtoRGB(BG_hue, 1.f, 1.f, &BGcolorRed, &BGcolorGreen, &BGcolorBlue);
			//printf("RGB %.2f -> %.2f %.2f %.2f \n", BG_hue, BGcolorRed, BGcolorGreen, BGcolorBlue);
		}
		else if (flashchange_BGcolor_freq == 0) {
			BGcolorRed = 0.f;
			BGcolorGreen = 0.f;
			BGcolorBlue = 0.f;
		}
	}

#if defined(var_flashchange_freeze_freq)
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashchange_freeze_freq]) {
		if ((*control_function)(flashchange_freeze_freq)) {
			freeze = !freeze;
			*((bool*)pg_FullScenarioVarPointers[_freeze]) = freeze;
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////
// beat reception and corresponding events triggering
void ReceiveBeat(void) {
	sprintf(AuxString, "/loop_beat %d", pg_BeatNo % PG_LOOP_SIZE); pg_send_message_udp((char*)"i", AuxString, (char*)"udp_TouchOSC_send");

	// loop start 
	if ((pg_BeatNo % PG_LOOP_SIZE) == 0) {
		for (int ind = 1; ind <= PG_NB_PATHS; ind++) {
			// source track recording is on -> source track start 
			if (synchr_start_recording_path[ind]) {
				pg_path_recording_onOff(ind);
				// sets to 0 the recording message since recording has started 
				synchr_start_recording_path[ind] = false;
			}


			// track reading is on -> track read start 
			if (synchr_start_path_replay_trackNo[ind] >= 0) {
				pg_path_replay_trackNo_onOff(ind, synchr_start_path_replay_trackNo[ind]);
				synchr_start_path_replay_trackNo[ind] = -1;
			}
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashCameraTrkBeat]) {
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
		// camera flash
		if (is_flashCameraTrk
			&& ((pg_BeatNo % PG_LOOP_SIZE) == flashCameraTrkBeat || flashCameraTrkBeat == 9)
			) {
			is_flashCameraTrk = false;
			flashCameraTrk_weight = 1.0f;
			flashCameraTrk_threshold = std::min(std::max(0.0f, 1.0f - flashCameraTrk_decay * flashCameraTrkBright), 1.0f);
			//printf("flashCameraTrk_weight %.3f decay %.2f threshold %.3f\n", 
			//	flashCameraTrk_weight , flashCameraTrk_decay, flashPhotoTrk_threshold);
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBeat]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBright]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkLength]
		&& pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoChangeBeat]) {
		// photo flash
		// weight is the current brightness of the lighting at each frame  until it reaches the threshold value and is then stopped 
		if (is_flashPhotoTrk
			&& ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoTrkBeat || flashPhotoTrkBeat == 0)) {
			is_flashPhotoTrk = false;
			pg_Make_flashPhoto();
		}
	}

	pg_flash_control(flash_beat_generation);

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoChangeBeat]) {
		// photo change flash
		if (flashPhotoChangeBeat > 0 && ((pg_BeatNo % PG_LOOP_SIZE) == flashPhotoChangeBeat)) {
			// goes to the first photo diaporama if it is not already selected and if there is one 
			if (photo_diaporama < 0 && pg_ImageDirectory[pg_current_configuration_rank] != "") {
				photo_diaporama = 0;
			}
			// random selection and loopiing
			photo_diaporama = (photo_diaporama + int(rand_0_1 * pg_nbCompressedImageDirs[pg_current_configuration_rank])) % pg_nbCompressedImageDirs[pg_current_configuration_rank];
			//printf("photo_diaporama %d\n", photo_diaporama);

			sprintf(AuxString, "/diaporama_shortName %03d", photo_diaporama);
			pg_send_message_udp((char*)"s", AuxString, (char*)"udp_TouchOSC_send");
			pg_launch_diaporama(0);
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_BGcolor]) {
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

			// background flash
			BGcolorFlash = true;
			BGcolorFlash_prec = false;
		}
		else {
			BGcolorRed = 0.f;
			BGcolorGreen = 0.f;
			BGcolorBlue = 0.f;
		}
	}

	for (int ind = 0; ind < pg_nb_light_groups[pg_current_configuration_rank]; ind++) {
		if (pg_light_groups[pg_current_configuration_rank][ind].get_group_hasBeatCommand()) {
			pg_light_groups[pg_current_configuration_rank][ind].update_beatCommand();
		}
	}

	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_penStrokeAtBeat]) {
		if (penStrokeAtBeat) {
			FourFrameStrokeNb = 5;
			FourFrameStroke_x = int(640 + 2 * rand_0_1);
			FourFrameStroke_y = int(180 + 2 * rand_0_1);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// photo flash launching
void pg_Make_flashPhoto(void) {
	if (pg_FullScenarioActiveVars[pg_current_configuration_rank][_flashPhotoTrkBright]) {
		flashPhotoTrk_weight = 1.0f;
		flashPhotoTrk_nbFrames = 0;
		flashPhotoTrk_threshold = std::min(std::max(0.0f, 1.0f - flashPhotoTrk_decay * flashPhotoTrkBright), 1.0f);
		// invertAllLayers = !invertAllLayers;
		//photo_scale = rand_0_1 * 0.5f + 0.5f;
		//photo_offsetX = rand_0_1 * 1.5f - 0.5f;
		//photo_offsetY = rand_0_1 * 1.5f - 0.5f;
		// photo_diaporama = int(floor(9 + rand_0_1 * 14.9));
		photo_diaporama = int(floor(rand_0_1 * pg_nbCompressedImageDirs[pg_current_configuration_rank])) % pg_nbCompressedImageDirs[pg_current_configuration_rank];
		pg_launch_diaporama(0);
		//printf("flashPhotoTrk_weight %.3f decay %.2f threshold %.3f\n",
		//	flashPhotoTrk_weight, flashPhotoTrk_decay, flashPhotoTrk_threshold);
	}
}

void pg_Make_flashCamera(void) {
	is_flashCameraTrk = true;
	flashCameraTrk_weight = 0.0f;
	// printf("start flash\n");
	//    break;
}

