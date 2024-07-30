/*! \file pg-flash.h
 *
 *
 *     File pg-flash.h
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

#ifndef PG_FLASH_H
#define PG_FLASH_H

extern int pg_flashTrkCA_freq_saved[PG_NB_TRACKS];
extern int pg_flashTrkPart_freq_saved[PG_NB_TRACKS];
extern int pg_flashTrkBG_freq_saved[PG_NB_TRACKS];

// +++++++++++++++++++++ BEAT SYNCHRONIZED RECORD/REPLAY +++++++++++++++++++++
extern bool pg_synchr_start_recording_path[PG_NB_PATHS + 1];
extern int pg_synchr_start_path_replay_trackNo[PG_NB_PATHS + 1];

// ++++++++++++++++++++++ FLASHES +++++++++++++++++++++++++
// flash: Trk->CA  
extern float pg_flashTrkCA_weights[PG_NB_TRACKS];
extern int pg_flashTrkCA_weights_duration[PG_NB_TRACKS];
// flash: Trk->Part
extern float pg_flashTrkPart_weights[PG_NB_TRACKS];
extern int pg_flashTrkPart_weights_duration[PG_NB_TRACKS];
// flash: CA->BG
extern float pg_flashCABG_weight;
extern int pg_flashCABG_weight_duration;
// flash: CA->Part
extern float pg_flashCAPart_weight;
extern int pg_flashCAPart_weight_duration;
// flash: Part->BG
extern float pg_flashPartBG_weight;
extern int pg_flashPartBG_weight_duration;
// flash: Part->CA
extern float pg_flashPartCA_weight;
extern int pg_flashPartCA_weight_duration;
// flash: extension of pixel reproduction  
extern int pg_flashPixel;
// one-frame master flashing  
extern int pg_flashMaster;
// flash: Trk->BG  
extern float pg_flashTrkBG_weights[PG_NB_TRACKS];
extern int pg_flashTrkBG_weights_duration[PG_NB_TRACKS];

// +++++++++++++++++++++ VIDEO FLASH +++++++++++++++++++++
extern bool pg_is_flashCameraTrk;
extern float pg_flashCameraTrk_weight;
extern float pg_flashCameraTrk_decay;
extern float pg_flashCameraTrk_threshold;

// +++++++++++++++++++++ PHOTO FLASH +++++++++++++++++++++
extern bool pg_is_flashPhotoTrk;
extern float pg_flashPhotoTrk_weight;
extern float pg_flashPhotoTrk_decay;
extern float pg_flashPhotoTrk_threshold;
extern int pg_flashPhotoTrk_nbFrames;


void pg_Make_flashCamera(void);
void pg_receiveBeat(void);

#endif