// SCENARIO VARIABLES
bool  auto_beat           ;
bool  auto_pulse          ;
bool  clearAllLayers      ;
bool  clearCA             ;
bool  clearLayer          ;
bool  reset_camera        ;
bool  camera_BG_subtr     ;
bool  mute_second_screen  ;
bool  invertAllLayers     ;
bool  take_snapshots      ;
int   cursorSize          ;
float auto_beat_duration  ;
float master              ;
float CAMixingWeight      ;
float CAMixingWeight_pulse;
float ClipArtMixingWeight ;
float ClipArtMixingWeight_pulse;
float PartMixingWeight    ;
float PartMixingWeight_pulse;
float trackMixingWeight_0 ;
float trackMixingWeight_0_pulse;
float trackMixingWeight_1 ;
float trackMixingWeight_1_pulse;
float trackMixingWeight_2 ;
float trackMixingWeight_2_pulse;
float trackMixingWeight_3 ;
float trackMixingWeight_3_pulse;
float CAMasterWeight      ;
float CAMasterWeight_pulse;
float ClipArtMasterWeight ;
float ClipArtMasterWeight_pulse;
float PartMasterWeight    ;
float PartMasterWeight_pulse;
float trackMasterWeight_0 ;
float trackMasterWeight_0_pulse;
float trackMasterWeight_1 ;
float trackMasterWeight_1_pulse;
float trackMasterWeight_2 ;
float trackMasterWeight_2_pulse;
float trackMasterWeight_3 ;
float trackMasterWeight_3_pulse;
float SecondMasterMixingWeight;
float echo                ;
float echo_pulse          ;
float echoNeg             ;
float echoNeg_pulse       ;
float CAdecay             ;
float CAdecay_pulse       ;
float partDecay           ;
float partDecay_pulse     ;
float trkDecay_0          ;
float trkDecay_0_pulse    ;
float trkDecay_1          ;
float trkDecay_1_pulse    ;
float trkDecay_2          ;
float trkDecay_2_pulse    ;
float trkDecay_3          ;
float trkDecay_3_pulse    ;
float pen_radius          ;
float pen_radius_pulse    ;
float pen_position_pulse  ;
float pen_angle_pulse     ;
int   pen_position_dash   ;
float pen_color           ;
float pen_color_pulse     ;
float color_spectrum_coef ;
float pen_grey            ;
float pen_grey_pulse      ;
float pen_color_a         ;
float pen_color_a_pulse   ;
int   pen_brush           ;
float pen_radius_replay   ;
float pen_radius_replay_pulse;
float pen_color_replay    ;
float pen_color_replay_pulse;
float pen_hue_replay      ;
float pen_hue_replay_pulse;
float pen_grey_replay     ;
float pen_grey_replay_pulse;
float pen_saturation_replay;
float pen_saturation_replay_pulse;
float pen_value_replay    ;
float pen_value_replay_pulse;
int   pen_brush_replay    ;
int   currentDrawingTrack ;
int   currentVideoTrack   ;
int   currentPhotoTrack   ;
int   currentMaskTrack    ;
bool  slow_track_translation;
float track_x_transl_0    ;
float track_x_transl_0_pulse;
float track_y_transl_0    ;
float track_y_transl_0_pulse;
float track_x_transl_1    ;
float track_x_transl_1_pulse;
float track_y_transl_1    ;
float track_y_transl_1_pulse;
int   part_initialization ;
int   part_image_acceleration;
int   pixel_image_acceleration;
bool  part_path_repop_0   ;
bool  part_path_repop_1   ;
bool  part_path_repop_2   ;
bool  part_path_repop_3   ;
bool  part_path_repop_4   ;
bool  part_path_repop_5   ;
bool  part_path_repop_6   ;
bool  part_path_repop_7   ;
bool  part_path_repop_8   ;
bool  part_path_repop_9   ;
bool  part_path_repop_10  ;
bool  part_path_repop_11  ;
bool  part_path_follow_0  ;
bool  part_path_follow_1  ;
bool  part_path_follow_2  ;
bool  part_path_follow_3  ;
bool  part_path_follow_4  ;
bool  part_path_follow_5  ;
bool  part_path_follow_6  ;
bool  part_path_follow_7  ;
bool  part_path_follow_8  ;
bool  part_path_follow_9  ;
bool  part_path_follow_10 ;
bool  part_path_follow_11 ;
bool  part_path_repulse_0 ;
bool  part_path_repulse_1 ;
bool  part_path_repulse_2 ;
bool  part_path_repulse_3 ;
bool  part_path_repulse_4 ;
bool  part_path_repulse_5 ;
bool  part_path_repulse_6 ;
bool  part_path_repulse_7 ;
bool  part_path_repulse_8 ;
bool  part_path_repulse_9 ;
bool  part_path_repulse_10;
bool  part_path_repulse_11;
bool  path_record_1       ;
bool  path_record_2       ;
bool  path_record_3       ;
bool  path_record_4       ;
bool  path_record_5       ;
bool  path_record_6       ;
bool  path_record_7       ;
bool  path_record_8       ;
bool  path_record_9       ;
bool  path_record_10      ;
bool  path_record_11      ;
int   path_replay_trackNo_1;
int   path_replay_trackNo_2;
int   path_replay_trackNo_3;
int   path_replay_trackNo_4;
int   path_replay_trackNo_5;
int   path_replay_trackNo_6;
int   path_replay_trackNo_7;
int   path_replay_trackNo_8;
int   path_replay_trackNo_9;
int   path_replay_trackNo_10;
int   path_replay_trackNo_11;
float part_size           ;
float part_size_pulse     ;
int   partSplat_texture   ;
float part_acc            ;
float part_acc_pulse      ;
float part_damp           ;
float part_damp_pulse     ;
float part_gravity        ;
float part_gravity_pulse  ;
float pulsed_part_Hshift  ;
float pulsed_part_Vshift  ;
float noiseUpdateScale    ;
float noiseUpdateScale_pulse;
float noiseParticleScale  ;
float noiseParticleScale_pulse;
int   noiseType           ;
float noiseLineScale      ;
float noiseAngleScale     ;
float noiseCenterX        ;
float noiseCenterY        ;
float part_field_weight   ;
float part_field_weight_pulse;
float part_damp_targtRad  ;
float part_timeToTargt    ;
int   particle_type       ;
bool  partMove_target     ;
bool  partMove_rand       ;
int   partExit_mode       ;
int   partStroke_mode     ;
int   partColor_mode      ;
float pixel_acc           ;
float pixel_acc_pulse     ;
float pixel_acc_shiftX    ;
float pixel_acc_shiftX_pulse;
float pixel_acc_shiftY    ;
float pixel_acc_shiftY_pulse;
float pixel_radius        ;
float pixel_radius_pulse  ;
int   pixel_mode          ;
float repop_CA            ;
float repop_CA_pulse      ;
float repop_part          ;
float repop_part_pulse    ;
float repop_BG            ;
float repop_BG_pulse      ;
float repop_path          ;
float repop_path_pulse    ;
int   BG_CA_repop_density ;
int   Part_repop_density  ;
float repop_colorBG       ;
int   BG_CA_repop_color_mode;
int   Part_repop_color_mode;
float repop_colorBG_pulse ;
float repop_greyBG        ;
float repop_greyBG_pulse  ;
float repop_colorPart     ;
float repop_colorPart_pulse;
float repop_greyPart      ;
float repop_greyPart_pulse;
float repop_colorCA       ;
float repop_colorCA_pulse ;
float repop_greyCA        ;
float repop_greyCA_pulse  ;
float cameraWB_R          ;
float cameraWB_B          ;
float cameraExposure      ;
float cameraGain          ;
float cameraBrightness    ;
float cameraSaturation    ;
float cameraContrast      ;
float cameraGamma         ;
float cameraCaptFreq      ;
float camera_gamma        ;
float camera_gamma_pulse  ;
int   cameraNo            ;
int   playing_movieNo     ;
float movieCaptFreq       ;
string playing_clipNameLeft;
string playing_clipNameRight;
string playing_secondClipNameLeft;
string playing_secondClipNameRight;
float clip_mix            ;
float clipCaptFreq        ;
float clip_scratch_factor ;
float clip_nudge_factor   ;
int   photo_diaporama     ;
float photo_diaporama_fade;
float photo_diaporama_plateau;
int   activeClipArts      ;
int   playing_soundtrackNo;
float cameraThreshold     ;
float cameraThreshold_pulse;
float cameraWeight        ;
float cameraWeight_pulse  ;
float cameraSobel         ;
float cameraSobel_pulse   ;
float movieWeight         ;
float movieWeight_pulse   ;
float movieSobel          ;
float movieSobel_pulse    ;
bool  invertMovie         ;
bool  invertCamera        ;
bool  invertPhoto         ;
float video_satur         ;
float video_satur_pulse   ;
float video_value         ;
float video_value_pulse   ;
float movie_gamma         ;
float movie_gamma_pulse   ;
float movie_threshold     ;
float movie_threshold_pulse;
float video_gamma         ;
float video_gamma_pulse   ;
float video_threshold     ;
float video_threshold_pulse;
float video_contrast      ;
float video_contrast_pulse;
float photoWeight         ;
float photoWeight_pulse   ;
float photo_satur         ;
float photo_satur_pulse   ;
float photo_value         ;
float photo_value_pulse   ;
float photo_gamma         ;
float photo_gamma_pulse   ;
float photo_threshold     ;
float photo_threshold_pulse;
float photoSobel          ;
float photoSobel_pulse    ;
float photoJitterAmpl     ;
float maskJitterAmpl      ;
float photo_scaleX        ;
float photo_scaleY        ;
float photo_rot           ;
float photo_transl_x      ;
float photo_transl_y      ;
float photo_offsetX       ;
float photo_offsetY       ;
float mask_scale          ;
float photo_contrast      ;
float mask_contrast       ;
float CAParams1           ;
float CAParams1_pulse     ;
float CAParams2           ;
float CAParams2_pulse     ;
float CAParams3           ;
float CAParams3_pulse     ;
float CAParams4           ;
float CAParams4_pulse     ;
float CAParams5           ;
float CAParams5_pulse     ;
float CAParams6           ;
float CAParams6_pulse     ;
float CAParams7           ;
float CAParams7_pulse     ;
float CAParams8           ;
float CAParams8_pulse     ;
int   CA1Type             ;
int   CA1SubType          ;
int   CA2Type             ;
int   CA2SubType          ;
float CA1_CA2_weight      ;
int   flashPartCA_freq    ;
int   flashTrkCA_freq_0   ;
int   flashTrkCA_freq_1   ;
int   flashTrkCA_freq_2   ;
int   flashTrkCA_freq_3   ;
int   flashCABG_freq      ;
int   flashPartBG_freq    ;
int   flashTrkBG_freq_1   ;
int   flashTrkBG_freq_2   ;
int   flashTrkBG_freq_3   ;
int   flashCAPart_freq    ;
int   flashTrkPart_freq_0 ;
int   flashTrkPart_freq_1 ;
int   flashTrkPart_freq_2 ;
int   flashTrkPart_freq_3 ;
int   flashParticleInit_freq;
int   flashPixel_freq     ;
int   flashPixel_duration ;
float flashCameraTrkLength;
float flashCameraTrkBright;
int   flashCameraTrkBeat  ;
float flashPhotoTrkLength ;
float flashPhotoTrkBright ;
int   flashPhotoTrkBeat   ;
int   flashPhotoChangeBeat;
float master_scale        ;
float master_scale_pulse  ;
float master_scale_ratio  ;
float master_offsetX      ;
float master_offsetY      ;
float master_mask_opacity_1;
float master_mask_opacity_2;
float master_mask_opacity_3;
float master_mask_opacity_4;
float master_mask_opacity_5;
float master_mask_opacity_6;
float beat_threshold      ;
float beat_delay          ;
float sound_volume        ;
float sound_min           ;
float pen_radius_pressure_coef;
float pen_radius_angleHor_coef;
float pen_radius_angleVer_coef;
bool  tracksSync          ;
int   cameraCumul         ;
int   CAstep              ;
int   Pixelstep           ;
bool  CAcolorSpread       ;
bool  freeze              ;
float sound_env_min       ;
float sound_env_max       ;
float audioInput_weight   ;
float soundtrack_PD_weight;
float soundtrack_PA_weight;
float sensor_sample_setUp ;
int   sensor_layout       ;
int   sensor_activation   ;
float sensor_vol          ;
bool  movie_loop          ;
bool  path_replay_loop    ;
float path_replay_speed   ;
float path_scaleX         ;
float path_scaleY         ;
float path_translX        ;
float path_translY        ;
float master_crop_x       ;
float master_crop_y       ;
float master_crop_width   ;
int   blurRadius_1        ;
int   blurRadius_2        ;
float video_white         ;
float video_white_pulse   ;
int   screen_drawing_no   ;
float master_mask         ;
float master_mask_scale   ;
float master_mask_scale_ratio;
float master_mask_offsetX ;
float master_mask_offsetY ;
float video_hue           ;
float video_hue_pulse     ;
float photo_hue           ;
float photo_hue_pulse     ;
bool  penStrokeAtBeat     ;
int   path_group          ;
string script_1            ;
float moving_messages     ;
bool  ScenarioVarConfigurations[_MaxInterpVarIDs][1] = {
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
  {true, },
};
VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { 
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_path,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_string,
	_pg_string,
	_pg_string,
	_pg_string,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_int,
	_pg_string,
	_pg_float,
};
void * ScenarioVarPointers[_MaxInterpVarIDs] = { 
	(void *)&auto_beat,
	(void *)&auto_pulse,
	(void *)&clearAllLayers,
	(void *)&clearCA,
	(void *)&clearLayer,
	(void *)&reset_camera,
	(void *)&camera_BG_subtr,
	(void *)&mute_second_screen,
	(void *)&invertAllLayers,
	(void *)&take_snapshots,
	(void *)&cursorSize,
	(void *)&auto_beat_duration,
	(void *)&master,
	(void *)&CAMixingWeight,
	(void *)&CAMixingWeight_pulse,
	(void *)&ClipArtMixingWeight,
	(void *)&ClipArtMixingWeight_pulse,
	(void *)&PartMixingWeight,
	(void *)&PartMixingWeight_pulse,
	(void *)&trackMixingWeight_0,
	(void *)&trackMixingWeight_0_pulse,
	(void *)&trackMixingWeight_1,
	(void *)&trackMixingWeight_1_pulse,
	(void *)&trackMixingWeight_2,
	(void *)&trackMixingWeight_2_pulse,
	(void *)&trackMixingWeight_3,
	(void *)&trackMixingWeight_3_pulse,
	(void *)&CAMasterWeight,
	(void *)&CAMasterWeight_pulse,
	(void *)&ClipArtMasterWeight,
	(void *)&ClipArtMasterWeight_pulse,
	(void *)&PartMasterWeight,
	(void *)&PartMasterWeight_pulse,
	(void *)&trackMasterWeight_0,
	(void *)&trackMasterWeight_0_pulse,
	(void *)&trackMasterWeight_1,
	(void *)&trackMasterWeight_1_pulse,
	(void *)&trackMasterWeight_2,
	(void *)&trackMasterWeight_2_pulse,
	(void *)&trackMasterWeight_3,
	(void *)&trackMasterWeight_3_pulse,
	(void *)&SecondMasterMixingWeight,
	(void *)&echo,
	(void *)&echo_pulse,
	(void *)&echoNeg,
	(void *)&echoNeg_pulse,
	(void *)&CAdecay,
	(void *)&CAdecay_pulse,
	(void *)&partDecay,
	(void *)&partDecay_pulse,
	(void *)&trkDecay_0,
	(void *)&trkDecay_0_pulse,
	(void *)&trkDecay_1,
	(void *)&trkDecay_1_pulse,
	(void *)&trkDecay_2,
	(void *)&trkDecay_2_pulse,
	(void *)&trkDecay_3,
	(void *)&trkDecay_3_pulse,
	(void *)&pen_radius,
	(void *)&pen_radius_pulse,
	(void *)&pen_position_pulse,
	(void *)&pen_angle_pulse,
	(void *)&pen_position_dash,
	(void *)&pen_color,
	(void *)&pen_color_pulse,
	(void *)&color_spectrum_coef,
	(void *)&pen_grey,
	(void *)&pen_grey_pulse,
	(void *)&pen_color_a,
	(void *)&pen_color_a_pulse,
	(void *)&pen_brush,
	(void *)&pen_radius_replay,
	(void *)&pen_radius_replay_pulse,
	(void *)&pen_color_replay,
	(void *)&pen_color_replay_pulse,
	(void *)&pen_hue_replay,
	(void *)&pen_hue_replay_pulse,
	(void *)&pen_grey_replay,
	(void *)&pen_grey_replay_pulse,
	(void *)&pen_saturation_replay,
	(void *)&pen_saturation_replay_pulse,
	(void *)&pen_value_replay,
	(void *)&pen_value_replay_pulse,
	(void *)&pen_brush_replay,
	(void *)&currentDrawingTrack,
	(void *)&currentVideoTrack,
	(void *)&currentPhotoTrack,
	(void *)&currentMaskTrack,
	(void *)&slow_track_translation,
	(void *)&track_x_transl_0,
	(void *)&track_x_transl_0_pulse,
	(void *)&track_y_transl_0,
	(void *)&track_y_transl_0_pulse,
	(void *)&track_x_transl_1,
	(void *)&track_x_transl_1_pulse,
	(void *)&track_y_transl_1,
	(void *)&track_y_transl_1_pulse,
	(void *)&part_initialization,
	(void *)&part_image_acceleration,
	(void *)&pixel_image_acceleration,
	(void *)&part_path_repop_0,
	(void *)&part_path_repop_1,
	(void *)&part_path_repop_2,
	(void *)&part_path_repop_3,
	(void *)&part_path_repop_4,
	(void *)&part_path_repop_5,
	(void *)&part_path_repop_6,
	(void *)&part_path_repop_7,
	(void *)&part_path_repop_8,
	(void *)&part_path_repop_9,
	(void *)&part_path_repop_10,
	(void *)&part_path_repop_11,
	(void *)&part_path_follow_0,
	(void *)&part_path_follow_1,
	(void *)&part_path_follow_2,
	(void *)&part_path_follow_3,
	(void *)&part_path_follow_4,
	(void *)&part_path_follow_5,
	(void *)&part_path_follow_6,
	(void *)&part_path_follow_7,
	(void *)&part_path_follow_8,
	(void *)&part_path_follow_9,
	(void *)&part_path_follow_10,
	(void *)&part_path_follow_11,
	(void *)&part_path_repulse_0,
	(void *)&part_path_repulse_1,
	(void *)&part_path_repulse_2,
	(void *)&part_path_repulse_3,
	(void *)&part_path_repulse_4,
	(void *)&part_path_repulse_5,
	(void *)&part_path_repulse_6,
	(void *)&part_path_repulse_7,
	(void *)&part_path_repulse_8,
	(void *)&part_path_repulse_9,
	(void *)&part_path_repulse_10,
	(void *)&part_path_repulse_11,
	(void *)&path_record_1,
	(void *)&path_record_2,
	(void *)&path_record_3,
	(void *)&path_record_4,
	(void *)&path_record_5,
	(void *)&path_record_6,
	(void *)&path_record_7,
	(void *)&path_record_8,
	(void *)&path_record_9,
	(void *)&path_record_10,
	(void *)&path_record_11,
	(void *)&path_replay_trackNo_1,
	(void *)&path_replay_trackNo_2,
	(void *)&path_replay_trackNo_3,
	(void *)&path_replay_trackNo_4,
	(void *)&path_replay_trackNo_5,
	(void *)&path_replay_trackNo_6,
	(void *)&path_replay_trackNo_7,
	(void *)&path_replay_trackNo_8,
	(void *)&path_replay_trackNo_9,
	(void *)&path_replay_trackNo_10,
	(void *)&path_replay_trackNo_11,
	(void *)&part_size,
	(void *)&part_size_pulse,
	(void *)&partSplat_texture,
	(void *)&part_acc,
	(void *)&part_acc_pulse,
	(void *)&part_damp,
	(void *)&part_damp_pulse,
	(void *)&part_gravity,
	(void *)&part_gravity_pulse,
	(void *)&pulsed_part_Hshift,
	(void *)&pulsed_part_Vshift,
	(void *)&noiseUpdateScale,
	(void *)&noiseUpdateScale_pulse,
	(void *)&noiseParticleScale,
	(void *)&noiseParticleScale_pulse,
	(void *)&noiseType,
	(void *)&noiseLineScale,
	(void *)&noiseAngleScale,
	(void *)&noiseCenterX,
	(void *)&noiseCenterY,
	(void *)&part_field_weight,
	(void *)&part_field_weight_pulse,
	(void *)&part_damp_targtRad,
	(void *)&part_timeToTargt,
	(void *)&particle_type,
	(void *)&partMove_target,
	(void *)&partMove_rand,
	(void *)&partExit_mode,
	(void *)&partStroke_mode,
	(void *)&partColor_mode,
	(void *)&pixel_acc,
	(void *)&pixel_acc_pulse,
	(void *)&pixel_acc_shiftX,
	(void *)&pixel_acc_shiftX_pulse,
	(void *)&pixel_acc_shiftY,
	(void *)&pixel_acc_shiftY_pulse,
	(void *)&pixel_radius,
	(void *)&pixel_radius_pulse,
	(void *)&pixel_mode,
	(void *)&repop_CA,
	(void *)&repop_CA_pulse,
	(void *)&repop_part,
	(void *)&repop_part_pulse,
	(void *)&repop_BG,
	(void *)&repop_BG_pulse,
	(void *)&repop_path,
	(void *)&repop_path_pulse,
	(void *)&BG_CA_repop_density,
	(void *)&Part_repop_density,
	(void *)&repop_colorBG,
	(void *)&BG_CA_repop_color_mode,
	(void *)&Part_repop_color_mode,
	(void *)&repop_colorBG_pulse,
	(void *)&repop_greyBG,
	(void *)&repop_greyBG_pulse,
	(void *)&repop_colorPart,
	(void *)&repop_colorPart_pulse,
	(void *)&repop_greyPart,
	(void *)&repop_greyPart_pulse,
	(void *)&repop_colorCA,
	(void *)&repop_colorCA_pulse,
	(void *)&repop_greyCA,
	(void *)&repop_greyCA_pulse,
	(void *)&cameraWB_R,
	(void *)&cameraWB_B,
	(void *)&cameraExposure,
	(void *)&cameraGain,
	(void *)&cameraBrightness,
	(void *)&cameraSaturation,
	(void *)&cameraContrast,
	(void *)&cameraGamma,
	(void *)&cameraCaptFreq,
	(void *)&camera_gamma,
	(void *)&camera_gamma_pulse,
	(void *)&cameraNo,
	(void *)&playing_movieNo,
	(void *)&movieCaptFreq,
	(void *)&playing_clipNameLeft,
	(void *)&playing_clipNameRight,
	(void *)&playing_secondClipNameLeft,
	(void *)&playing_secondClipNameRight,
	(void *)&clip_mix,
	(void *)&clipCaptFreq,
	(void *)&clip_scratch_factor,
	(void *)&clip_nudge_factor,
	(void *)&photo_diaporama,
	(void *)&photo_diaporama_fade,
	(void *)&photo_diaporama_plateau,
	(void *)&activeClipArts,
	(void *)&playing_soundtrackNo,
	(void *)&cameraThreshold,
	(void *)&cameraThreshold_pulse,
	(void *)&cameraWeight,
	(void *)&cameraWeight_pulse,
	(void *)&cameraSobel,
	(void *)&cameraSobel_pulse,
	(void *)&movieWeight,
	(void *)&movieWeight_pulse,
	(void *)&movieSobel,
	(void *)&movieSobel_pulse,
	(void *)&invertMovie,
	(void *)&invertCamera,
	(void *)&invertPhoto,
	(void *)&video_satur,
	(void *)&video_satur_pulse,
	(void *)&video_value,
	(void *)&video_value_pulse,
	(void *)&movie_gamma,
	(void *)&movie_gamma_pulse,
	(void *)&movie_threshold,
	(void *)&movie_threshold_pulse,
	(void *)&video_gamma,
	(void *)&video_gamma_pulse,
	(void *)&video_threshold,
	(void *)&video_threshold_pulse,
	(void *)&video_contrast,
	(void *)&video_contrast_pulse,
	(void *)&photoWeight,
	(void *)&photoWeight_pulse,
	(void *)&photo_satur,
	(void *)&photo_satur_pulse,
	(void *)&photo_value,
	(void *)&photo_value_pulse,
	(void *)&photo_gamma,
	(void *)&photo_gamma_pulse,
	(void *)&photo_threshold,
	(void *)&photo_threshold_pulse,
	(void *)&photoSobel,
	(void *)&photoSobel_pulse,
	(void *)&photoJitterAmpl,
	(void *)&maskJitterAmpl,
	(void *)&photo_scaleX,
	(void *)&photo_scaleY,
	(void *)&photo_rot,
	(void *)&photo_transl_x,
	(void *)&photo_transl_y,
	(void *)&photo_offsetX,
	(void *)&photo_offsetY,
	(void *)&mask_scale,
	(void *)&photo_contrast,
	(void *)&mask_contrast,
	(void *)&CAParams1,
	(void *)&CAParams1_pulse,
	(void *)&CAParams2,
	(void *)&CAParams2_pulse,
	(void *)&CAParams3,
	(void *)&CAParams3_pulse,
	(void *)&CAParams4,
	(void *)&CAParams4_pulse,
	(void *)&CAParams5,
	(void *)&CAParams5_pulse,
	(void *)&CAParams6,
	(void *)&CAParams6_pulse,
	(void *)&CAParams7,
	(void *)&CAParams7_pulse,
	(void *)&CAParams8,
	(void *)&CAParams8_pulse,
	(void *)&CA1Type,
	(void *)&CA1SubType,
	(void *)&CA2Type,
	(void *)&CA2SubType,
	(void *)&CA1_CA2_weight,
	(void *)&flashPartCA_freq,
	(void *)&flashTrkCA_freq_0,
	(void *)&flashTrkCA_freq_1,
	(void *)&flashTrkCA_freq_2,
	(void *)&flashTrkCA_freq_3,
	(void *)&flashCABG_freq,
	(void *)&flashPartBG_freq,
	(void *)&flashTrkBG_freq_1,
	(void *)&flashTrkBG_freq_2,
	(void *)&flashTrkBG_freq_3,
	(void *)&flashCAPart_freq,
	(void *)&flashTrkPart_freq_0,
	(void *)&flashTrkPart_freq_1,
	(void *)&flashTrkPart_freq_2,
	(void *)&flashTrkPart_freq_3,
	(void *)&flashParticleInit_freq,
	(void *)&flashPixel_freq,
	(void *)&flashPixel_duration,
	(void *)&flashCameraTrkLength,
	(void *)&flashCameraTrkBright,
	(void *)&flashCameraTrkBeat,
	(void *)&flashPhotoTrkLength,
	(void *)&flashPhotoTrkBright,
	(void *)&flashPhotoTrkBeat,
	(void *)&flashPhotoChangeBeat,
	(void *)&master_scale,
	(void *)&master_scale_pulse,
	(void *)&master_scale_ratio,
	(void *)&master_offsetX,
	(void *)&master_offsetY,
	(void *)&master_mask_opacity_1,
	(void *)&master_mask_opacity_2,
	(void *)&master_mask_opacity_3,
	(void *)&master_mask_opacity_4,
	(void *)&master_mask_opacity_5,
	(void *)&master_mask_opacity_6,
	(void *)&beat_threshold,
	(void *)&beat_delay,
	(void *)&sound_volume,
	(void *)&sound_min,
	(void *)&pen_radius_pressure_coef,
	(void *)&pen_radius_angleHor_coef,
	(void *)&pen_radius_angleVer_coef,
	(void *)&tracksSync,
	(void *)&cameraCumul,
	(void *)&CAstep,
	(void *)&Pixelstep,
	(void *)&CAcolorSpread,
	(void *)&freeze,
	(void *)&sound_env_min,
	(void *)&sound_env_max,
	(void *)&audioInput_weight,
	(void *)&soundtrack_PD_weight,
	(void *)&soundtrack_PA_weight,
	(void *)&sensor_sample_setUp,
	(void *)&sensor_layout,
	(void *)&sensor_activation,
	(void *)&sensor_vol,
	(void *)&movie_loop,
	(void *)&path_replay_loop,
	(void *)&path_replay_speed,
	(void *)&path_scaleX,
	(void *)&path_scaleY,
	(void *)&path_translX,
	(void *)&path_translY,
	(void *)&master_crop_x,
	(void *)&master_crop_y,
	(void *)&master_crop_width,
	(void *)&blurRadius_1,
	(void *)&blurRadius_2,
	(void *)&video_white,
	(void *)&video_white_pulse,
	(void *)&screen_drawing_no,
	(void *)&master_mask,
	(void *)&master_mask_scale,
	(void *)&master_mask_scale_ratio,
	(void *)&master_mask_offsetX,
	(void *)&master_mask_offsetY,
	(void *)&video_hue,
	(void *)&video_hue_pulse,
	(void *)&photo_hue,
	(void *)&photo_hue_pulse,
	(void *)&penStrokeAtBeat,
	(void *)&path_group,
	(void *)&script_1,
	(void *)&moving_messages,
};
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void auto_beat_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	auto_beat_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void auto_pulse_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void auto_pulse_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	auto_pulse_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearAllLayers_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearAllLayers_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearCA_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearCA_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearLayer_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearLayer_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearLayer_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void pen_color_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	pen_color_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void pen_brush_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	pen_brush_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void part_initialization_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_initialization_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void part_path_follow_0_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_0_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_0_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_1_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_1_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_2_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_2_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_2_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_3_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_3_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_3_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_4_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_4_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_4_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_5_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_5_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_5_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_6_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_6_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_6_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_7_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_7_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_7_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_8_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_8_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_8_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_9_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_9_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_9_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_10_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_10_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_10_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_11_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_11_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_follow_11_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_0_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_0_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_0_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_1_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_1_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_2_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_2_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_2_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_3_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_3_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_3_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_4_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_4_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_4_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_5_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_5_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_5_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_6_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_6_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_6_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_7_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_7_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_7_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_8_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_8_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_8_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_9_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_9_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_9_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_10_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_10_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_10_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_repulse_11_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_11_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_path_repulse_11_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void path_record_1_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_1_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_2_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_2_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_2_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_3_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_3_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_3_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_4_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_4_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_4_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_5_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_5_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_5_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_6_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_6_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_6_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_7_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_7_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_7_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_8_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_8_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_8_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_9_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_9_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_9_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_10_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_10_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_10_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_record_11_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_11_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_record_11_callBack(param_input_type, double_to_path(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_1_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_1_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_2_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_2_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_2_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_3_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_3_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_3_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_4_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_4_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_4_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_5_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_5_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_5_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_6_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_6_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_6_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_7_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_7_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_7_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_8_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_8_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_8_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_9_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_9_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_9_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_10_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_10_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_10_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_replay_trackNo_11_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_11_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_replay_trackNo_11_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void partMove_target_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void partMove_target_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	partMove_target_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void partMove_rand_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	partMove_rand_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraWB_R_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraWB_R_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraWB_B_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraWB_B_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraExposure_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraExposure_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraGain_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraGain_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraBrightness_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraBrightness_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraSaturation_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraSaturation_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraContrast_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraContrast_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraGamma_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraGamma_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void cameraNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_movieNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_movieNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void movieCaptFreq_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void movieCaptFreq_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	movieCaptFreq_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void playing_clipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_clipNameLeft_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_clipNameLeft_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_clipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_clipNameRight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_clipNameRight_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_secondClipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_secondClipNameLeft_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_secondClipNameLeft_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_secondClipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_secondClipNameRight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_secondClipNameRight_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void photo_diaporama_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	photo_diaporama_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_soundtrackNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_soundtrackNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void flashCameraTrkLength_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	flashCameraTrkLength_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void flashPhotoTrkLength_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	flashPhotoTrkLength_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void beat_threshold_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	beat_threshold_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void beat_delay_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	beat_delay_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sound_env_min_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sound_env_min_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sound_env_max_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sound_env_max_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void audioInput_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	audioInput_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void soundtrack_PD_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void soundtrack_PD_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	soundtrack_PD_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void soundtrack_PA_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void soundtrack_PA_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	soundtrack_PA_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sensor_sample_setUp_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_sample_setUp_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void sensor_layout_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_layout_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void sensor_activation_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_activation_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_group_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_group_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_group_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void script_1_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void script_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	script_1_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue) = { 
	&auto_beat_callBack_generic,
	&auto_pulse_callBack_generic,
	&clearAllLayers_callBack_generic,
	&clearCA_callBack_generic,
	&clearLayer_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&pen_color_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&pen_brush_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&part_initialization_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&part_path_follow_0_callBack_generic,
	&part_path_follow_1_callBack_generic,
	&part_path_follow_2_callBack_generic,
	&part_path_follow_3_callBack_generic,
	&part_path_follow_4_callBack_generic,
	&part_path_follow_5_callBack_generic,
	&part_path_follow_6_callBack_generic,
	&part_path_follow_7_callBack_generic,
	&part_path_follow_8_callBack_generic,
	&part_path_follow_9_callBack_generic,
	&part_path_follow_10_callBack_generic,
	&part_path_follow_11_callBack_generic,
	&part_path_repulse_0_callBack_generic,
	&part_path_repulse_1_callBack_generic,
	&part_path_repulse_2_callBack_generic,
	&part_path_repulse_3_callBack_generic,
	&part_path_repulse_4_callBack_generic,
	&part_path_repulse_5_callBack_generic,
	&part_path_repulse_6_callBack_generic,
	&part_path_repulse_7_callBack_generic,
	&part_path_repulse_8_callBack_generic,
	&part_path_repulse_9_callBack_generic,
	&part_path_repulse_10_callBack_generic,
	&part_path_repulse_11_callBack_generic,
	&path_record_1_callBack_generic,
	&path_record_2_callBack_generic,
	&path_record_3_callBack_generic,
	&path_record_4_callBack_generic,
	&path_record_5_callBack_generic,
	&path_record_6_callBack_generic,
	&path_record_7_callBack_generic,
	&path_record_8_callBack_generic,
	&path_record_9_callBack_generic,
	&path_record_10_callBack_generic,
	&path_record_11_callBack_generic,
	&path_replay_trackNo_1_callBack_generic,
	&path_replay_trackNo_2_callBack_generic,
	&path_replay_trackNo_3_callBack_generic,
	&path_replay_trackNo_4_callBack_generic,
	&path_replay_trackNo_5_callBack_generic,
	&path_replay_trackNo_6_callBack_generic,
	&path_replay_trackNo_7_callBack_generic,
	&path_replay_trackNo_8_callBack_generic,
	&path_replay_trackNo_9_callBack_generic,
	&path_replay_trackNo_10_callBack_generic,
	&path_replay_trackNo_11_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&partMove_target_callBack_generic,
	&partMove_rand_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&cameraWB_R_callBack_generic,
	&cameraWB_B_callBack_generic,
	&cameraExposure_callBack_generic,
	&cameraGain_callBack_generic,
	&cameraBrightness_callBack_generic,
	&cameraSaturation_callBack_generic,
	&cameraContrast_callBack_generic,
	&cameraGamma_callBack_generic,
	NULL,
	NULL,
	NULL,
	&cameraNo_callBack_generic,
	&playing_movieNo_callBack_generic,
	&movieCaptFreq_callBack_generic,
	&playing_clipNameLeft_callBack_generic,
	&playing_clipNameRight_callBack_generic,
	&playing_secondClipNameLeft_callBack_generic,
	&playing_secondClipNameRight_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	&photo_diaporama_callBack_generic,
	NULL,
	NULL,
	NULL,
	&playing_soundtrackNo_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&flashCameraTrkLength_callBack_generic,
	NULL,
	NULL,
	&flashPhotoTrkLength_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&beat_threshold_callBack_generic,
	&beat_delay_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&sound_env_min_callBack_generic,
	&sound_env_max_callBack_generic,
	&audioInput_weight_callBack_generic,
	&soundtrack_PD_weight_callBack_generic,
	&soundtrack_PA_weight_callBack_generic,
	&sensor_sample_setUp_callBack_generic,
	&sensor_layout_callBack_generic,
	&sensor_activation_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&path_group_callBack_generic,
	&script_1_callBack_generic,
	NULL,
};
char *ScenarioVarMessages[_MaxInterpVarIDs] = { 
  (char *)"auto_beat",
  (char *)"auto_pulse",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"reset_camera",
  (char *)"camera_BG_subtr",
  (char *)"mute_second_screen",
  (char *)"invertAllLayers",
  (char *)"take_snapshots",
  (char *)"cursorSize",
  (char *)"auto_beat_duration",
  (char *)"master",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"ClipArtMixingWeight",
  (char *)"ClipArtMixingWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"trackMixingWeight_0",
  (char *)"trackMixingWeight_0_pulse",
  (char *)"trackMixingWeight_1",
  (char *)"trackMixingWeight_1_pulse",
  (char *)"trackMixingWeight_2",
  (char *)"trackMixingWeight_2_pulse",
  (char *)"trackMixingWeight_3",
  (char *)"trackMixingWeight_3_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"ClipArtMasterWeight",
  (char *)"ClipArtMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"trackMasterWeight_0",
  (char *)"trackMasterWeight_0_pulse",
  (char *)"trackMasterWeight_1",
  (char *)"trackMasterWeight_1_pulse",
  (char *)"trackMasterWeight_2",
  (char *)"trackMasterWeight_2_pulse",
  (char *)"trackMasterWeight_3",
  (char *)"trackMasterWeight_3_pulse",
  (char *)"SecondMasterMixingWeight",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
  (char *)"partDecay",
  (char *)"partDecay_pulse",
  (char *)"trkDecay_0",
  (char *)"trkDecay_0_pulse",
  (char *)"trkDecay_1",
  (char *)"trkDecay_1_pulse",
  (char *)"trkDecay_2",
  (char *)"trkDecay_2_pulse",
  (char *)"trkDecay_3",
  (char *)"trkDecay_3_pulse",
  (char *)"pen_radius",
  (char *)"pen_radius_pulse",
  (char *)"pen_position_pulse",
  (char *)"pen_angle_pulse",
  (char *)"pen_position_dash",
  (char *)"pen_color",
  (char *)"pen_color_pulse",
  (char *)"color_spectrum_coef",
  (char *)"pen_grey",
  (char *)"pen_grey_pulse",
  (char *)"pen_color_a",
  (char *)"pen_color_a_pulse",
  (char *)"pen_brush",
  (char *)"pen_radius_replay",
  (char *)"pen_radius_replay_pulse",
  (char *)"pen_color_replay",
  (char *)"pen_color_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_grey_replay",
  (char *)"pen_grey_replay_pulse",
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"pen_brush_replay",
  (char *)"currentDrawingTrack",
  (char *)"currentVideoTrack",
  (char *)"currentPhotoTrack",
  (char *)"currentMaskTrack",
  (char *)"slow_track_translation",
  (char *)"track_x_transl_0",
  (char *)"track_x_transl_0_pulse",
  (char *)"track_y_transl_0",
  (char *)"track_y_transl_0_pulse",
  (char *)"track_x_transl_1",
  (char *)"track_x_transl_1_pulse",
  (char *)"track_y_transl_1",
  (char *)"track_y_transl_1_pulse",
  (char *)"part_initialization",
  (char *)"part_image_acceleration",
  (char *)"pixel_image_acceleration",
  (char *)"part_path_repop_0",
  (char *)"part_path_repop_1",
  (char *)"part_path_repop_2",
  (char *)"part_path_repop_3",
  (char *)"part_path_repop_4",
  (char *)"part_path_repop_5",
  (char *)"part_path_repop_6",
  (char *)"part_path_repop_7",
  (char *)"part_path_repop_8",
  (char *)"part_path_repop_9",
  (char *)"part_path_repop_10",
  (char *)"part_path_repop_11",
  (char *)"part_path_follow_0",
  (char *)"part_path_follow_1",
  (char *)"part_path_follow_2",
  (char *)"part_path_follow_3",
  (char *)"part_path_follow_4",
  (char *)"part_path_follow_5",
  (char *)"part_path_follow_6",
  (char *)"part_path_follow_7",
  (char *)"part_path_follow_8",
  (char *)"part_path_follow_9",
  (char *)"part_path_follow_10",
  (char *)"part_path_follow_11",
  (char *)"part_path_repulse_0",
  (char *)"part_path_repulse_1",
  (char *)"part_path_repulse_2",
  (char *)"part_path_repulse_3",
  (char *)"part_path_repulse_4",
  (char *)"part_path_repulse_5",
  (char *)"part_path_repulse_6",
  (char *)"part_path_repulse_7",
  (char *)"part_path_repulse_8",
  (char *)"part_path_repulse_9",
  (char *)"part_path_repulse_10",
  (char *)"part_path_repulse_11",
  (char *)"path_record_1",
  (char *)"path_record_2",
  (char *)"path_record_3",
  (char *)"path_record_4",
  (char *)"path_record_5",
  (char *)"path_record_6",
  (char *)"path_record_7",
  (char *)"path_record_8",
  (char *)"path_record_9",
  (char *)"path_record_10",
  (char *)"path_record_11",
  (char *)"path_replay_trackNo_1",
  (char *)"path_replay_trackNo_2",
  (char *)"path_replay_trackNo_3",
  (char *)"path_replay_trackNo_4",
  (char *)"path_replay_trackNo_5",
  (char *)"path_replay_trackNo_6",
  (char *)"path_replay_trackNo_7",
  (char *)"path_replay_trackNo_8",
  (char *)"path_replay_trackNo_9",
  (char *)"path_replay_trackNo_10",
  (char *)"path_replay_trackNo_11",
  (char *)"part_size",
  (char *)"part_size_pulse",
  (char *)"partSplat_texture",
  (char *)"part_acc",
  (char *)"part_acc_pulse",
  (char *)"part_damp",
  (char *)"part_damp_pulse",
  (char *)"part_gravity",
  (char *)"part_gravity_pulse",
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"noiseUpdateScale",
  (char *)"noiseUpdateScale_pulse",
  (char *)"noiseParticleScale",
  (char *)"noiseParticleScale_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
  (char *)"part_field_weight",
  (char *)"part_field_weight_pulse",
  (char *)"part_damp_targtRad",
  (char *)"part_timeToTargt",
  (char *)"particle_type",
  (char *)"partMove_target",
  (char *)"partMove_rand",
  (char *)"partExit_mode",
  (char *)"partStroke_mode",
  (char *)"partColor_mode",
  (char *)"pixel_acc",
  (char *)"pixel_acc_pulse",
  (char *)"pixel_acc_shiftX",
  (char *)"pixel_acc_shiftX_pulse",
  (char *)"pixel_acc_shiftY",
  (char *)"pixel_acc_shiftY_pulse",
  (char *)"pixel_radius",
  (char *)"pixel_radius_pulse",
  (char *)"pixel_mode",
  (char *)"repop_CA",
  (char *)"repop_CA_pulse",
  (char *)"repop_part",
  (char *)"repop_part_pulse",
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"BG_CA_repop_density",
  (char *)"Part_repop_density",
  (char *)"repop_colorBG",
  (char *)"BG_CA_repop_color_mode",
  (char *)"Part_repop_color_mode",
  (char *)"repop_colorBG_pulse",
  (char *)"repop_greyBG",
  (char *)"repop_greyBG_pulse",
  (char *)"repop_colorPart",
  (char *)"repop_colorPart_pulse",
  (char *)"repop_greyPart",
  (char *)"repop_greyPart_pulse",
  (char *)"repop_colorCA",
  (char *)"repop_colorCA_pulse",
  (char *)"repop_greyCA",
  (char *)"repop_greyCA_pulse",
  (char *)"cameraWB_R",
  (char *)"cameraWB_B",
  (char *)"cameraExposure",
  (char *)"cameraGain",
  (char *)"cameraBrightness",
  (char *)"cameraSaturation",
  (char *)"cameraContrast",
  (char *)"cameraGamma",
  (char *)"cameraCaptFreq",
  (char *)"camera_gamma",
  (char *)"camera_gamma_pulse",
  (char *)"cameraNo",
  (char *)"playing_movieNo",
  (char *)"movieCaptFreq",
  (char *)"playing_clipNameLeft",
  (char *)"playing_clipNameRight",
  (char *)"playing_secondClipNameLeft",
  (char *)"playing_secondClipNameRight",
  (char *)"clip_mix",
  (char *)"clipCaptFreq",
  (char *)"clip_scratch_factor",
  (char *)"clip_nudge_factor",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"activeClipArts",
  (char *)"playing_soundtrackNo",
  (char *)"cameraThreshold",
  (char *)"cameraThreshold_pulse",
  (char *)"cameraWeight",
  (char *)"cameraWeight_pulse",
  (char *)"cameraSobel",
  (char *)"cameraSobel_pulse",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"invertMovie",
  (char *)"invertCamera",
  (char *)"invertPhoto",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"movie_gamma",
  (char *)"movie_gamma_pulse",
  (char *)"movie_threshold",
  (char *)"movie_threshold_pulse",
  (char *)"video_gamma",
  (char *)"video_gamma_pulse",
  (char *)"video_threshold",
  (char *)"video_threshold_pulse",
  (char *)"video_contrast",
  (char *)"video_contrast_pulse",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photoSobel",
  (char *)"photoSobel_pulse",
  (char *)"photoJitterAmpl",
  (char *)"maskJitterAmpl",
  (char *)"photo_scaleX",
  (char *)"photo_scaleY",
  (char *)"photo_rot",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"photo_offsetX",
  (char *)"photo_offsetY",
  (char *)"mask_scale",
  (char *)"photo_contrast",
  (char *)"mask_contrast",
  (char *)"CAParams1",
  (char *)"CAParams1_pulse",
  (char *)"CAParams2",
  (char *)"CAParams2_pulse",
  (char *)"CAParams3",
  (char *)"CAParams3_pulse",
  (char *)"CAParams4",
  (char *)"CAParams4_pulse",
  (char *)"CAParams5",
  (char *)"CAParams5_pulse",
  (char *)"CAParams6",
  (char *)"CAParams6_pulse",
  (char *)"CAParams7",
  (char *)"CAParams7_pulse",
  (char *)"CAParams8",
  (char *)"CAParams8_pulse",
  (char *)"CA1Type",
  (char *)"CA1SubType",
  (char *)"CA2Type",
  (char *)"CA2SubType",
  (char *)"CA1_CA2_weight",
  (char *)"flashPartCA_freq",
  (char *)"flashTrkCA_freq_0",
  (char *)"flashTrkCA_freq_1",
  (char *)"flashTrkCA_freq_2",
  (char *)"flashTrkCA_freq_3",
  (char *)"flashCABG_freq",
  (char *)"flashPartBG_freq",
  (char *)"flashTrkBG_freq_1",
  (char *)"flashTrkBG_freq_2",
  (char *)"flashTrkBG_freq_3",
  (char *)"flashCAPart_freq",
  (char *)"flashTrkPart_freq_0",
  (char *)"flashTrkPart_freq_1",
  (char *)"flashTrkPart_freq_2",
  (char *)"flashTrkPart_freq_3",
  (char *)"flashParticleInit_freq",
  (char *)"flashPixel_freq",
  (char *)"flashPixel_duration",
  (char *)"flashCameraTrkLength",
  (char *)"flashCameraTrkBright",
  (char *)"flashCameraTrkBeat",
  (char *)"flashPhotoTrkLength",
  (char *)"flashPhotoTrkBright",
  (char *)"flashPhotoTrkBeat",
  (char *)"flashPhotoChangeBeat",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"master_mask_opacity_1",
  (char *)"master_mask_opacity_2",
  (char *)"master_mask_opacity_3",
  (char *)"master_mask_opacity_4",
  (char *)"master_mask_opacity_5",
  (char *)"master_mask_opacity_6",
  (char *)"beat_threshold",
  (char *)"beat_delay",
  (char *)"sound_volume",
  (char *)"sound_min",
  (char *)"pen_radius_pressure_coef",
  (char *)"pen_radius_angleHor_coef",
  (char *)"pen_radius_angleVer_coef",
  (char *)"tracksSync",
  (char *)"cameraCumul",
  (char *)"CAstep",
  (char *)"Pixelstep",
  (char *)"CAcolorSpread",
  (char *)"freeze",
  (char *)"sound_env_min",
  (char *)"sound_env_max",
  (char *)"audioInput_weight",
  (char *)"soundtrack_PD_weight",
  (char *)"soundtrack_PA_weight",
  (char *)"sensor_sample_setUp",
  (char *)"sensor_layout",
  (char *)"sensor_activation",
  (char *)"sensor_vol",
  (char *)"movie_loop",
  (char *)"path_replay_loop",
  (char *)"path_replay_speed",
  (char *)"path_scaleX",
  (char *)"path_scaleY",
  (char *)"path_translX",
  (char *)"path_translY",
  (char *)"master_crop_x",
  (char *)"master_crop_y",
  (char *)"master_crop_width",
  (char *)"blurRadius_1",
  (char *)"blurRadius_2",
  (char *)"video_white",
  (char *)"video_white_pulse",
  (char *)"screen_drawing_no",
  (char *)"master_mask",
  (char *)"master_mask_scale",
  (char *)"master_mask_scale_ratio",
  (char *)"master_mask_offsetX",
  (char *)"master_mask_offsetY",
  (char *)"video_hue",
  (char *)"video_hue_pulse",
  (char *)"photo_hue",
  (char *)"photo_hue_pulse",
  (char *)"penStrokeAtBeat",
  (char *)"path_group",
  (char *)"script_1",
  (char *)"moving_messages",
};
PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = {   _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
};
char *ScenarioVarStrings[_MaxInterpVarIDs] = { 
  (char *)"auto_beat",
  (char *)"auto_pulse",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"reset_camera",
  (char *)"camera_BG_subtr",
  (char *)"mute_second_screen",
  (char *)"invertAllLayers",
  (char *)"take_snapshots",
  (char *)"cursorSize",
  (char *)"auto_beat_duration",
  (char *)"master",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"ClipArtMixingWeight",
  (char *)"ClipArtMixingWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"trackMixingWeight_0",
  (char *)"trackMixingWeight_0_pulse",
  (char *)"trackMixingWeight_1",
  (char *)"trackMixingWeight_1_pulse",
  (char *)"trackMixingWeight_2",
  (char *)"trackMixingWeight_2_pulse",
  (char *)"trackMixingWeight_3",
  (char *)"trackMixingWeight_3_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"ClipArtMasterWeight",
  (char *)"ClipArtMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"trackMasterWeight_0",
  (char *)"trackMasterWeight_0_pulse",
  (char *)"trackMasterWeight_1",
  (char *)"trackMasterWeight_1_pulse",
  (char *)"trackMasterWeight_2",
  (char *)"trackMasterWeight_2_pulse",
  (char *)"trackMasterWeight_3",
  (char *)"trackMasterWeight_3_pulse",
  (char *)"SecondMasterMixingWeight",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
  (char *)"partDecay",
  (char *)"partDecay_pulse",
  (char *)"trkDecay_0",
  (char *)"trkDecay_0_pulse",
  (char *)"trkDecay_1",
  (char *)"trkDecay_1_pulse",
  (char *)"trkDecay_2",
  (char *)"trkDecay_2_pulse",
  (char *)"trkDecay_3",
  (char *)"trkDecay_3_pulse",
  (char *)"pen_radius",
  (char *)"pen_radius_pulse",
  (char *)"pen_position_pulse",
  (char *)"pen_angle_pulse",
  (char *)"pen_position_dash",
  (char *)"pen_color",
  (char *)"pen_color_pulse",
  (char *)"color_spectrum_coef",
  (char *)"pen_grey",
  (char *)"pen_grey_pulse",
  (char *)"pen_color_a",
  (char *)"pen_color_a_pulse",
  (char *)"pen_brush",
  (char *)"pen_radius_replay",
  (char *)"pen_radius_replay_pulse",
  (char *)"pen_color_replay",
  (char *)"pen_color_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_grey_replay",
  (char *)"pen_grey_replay_pulse",
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"pen_brush_replay",
  (char *)"currentDrawingTrack",
  (char *)"currentVideoTrack",
  (char *)"currentPhotoTrack",
  (char *)"currentMaskTrack",
  (char *)"slow_track_translation",
  (char *)"track_x_transl_0",
  (char *)"track_x_transl_0_pulse",
  (char *)"track_y_transl_0",
  (char *)"track_y_transl_0_pulse",
  (char *)"track_x_transl_1",
  (char *)"track_x_transl_1_pulse",
  (char *)"track_y_transl_1",
  (char *)"track_y_transl_1_pulse",
  (char *)"part_initialization",
  (char *)"part_image_acceleration",
  (char *)"pixel_image_acceleration",
  (char *)"part_path_repop_0",
  (char *)"part_path_repop_1",
  (char *)"part_path_repop_2",
  (char *)"part_path_repop_3",
  (char *)"part_path_repop_4",
  (char *)"part_path_repop_5",
  (char *)"part_path_repop_6",
  (char *)"part_path_repop_7",
  (char *)"part_path_repop_8",
  (char *)"part_path_repop_9",
  (char *)"part_path_repop_10",
  (char *)"part_path_repop_11",
  (char *)"part_path_follow_0",
  (char *)"part_path_follow_1",
  (char *)"part_path_follow_2",
  (char *)"part_path_follow_3",
  (char *)"part_path_follow_4",
  (char *)"part_path_follow_5",
  (char *)"part_path_follow_6",
  (char *)"part_path_follow_7",
  (char *)"part_path_follow_8",
  (char *)"part_path_follow_9",
  (char *)"part_path_follow_10",
  (char *)"part_path_follow_11",
  (char *)"part_path_repulse_0",
  (char *)"part_path_repulse_1",
  (char *)"part_path_repulse_2",
  (char *)"part_path_repulse_3",
  (char *)"part_path_repulse_4",
  (char *)"part_path_repulse_5",
  (char *)"part_path_repulse_6",
  (char *)"part_path_repulse_7",
  (char *)"part_path_repulse_8",
  (char *)"part_path_repulse_9",
  (char *)"part_path_repulse_10",
  (char *)"part_path_repulse_11",
  (char *)"path_record_1",
  (char *)"path_record_2",
  (char *)"path_record_3",
  (char *)"path_record_4",
  (char *)"path_record_5",
  (char *)"path_record_6",
  (char *)"path_record_7",
  (char *)"path_record_8",
  (char *)"path_record_9",
  (char *)"path_record_10",
  (char *)"path_record_11",
  (char *)"path_replay_trackNo_1",
  (char *)"path_replay_trackNo_2",
  (char *)"path_replay_trackNo_3",
  (char *)"path_replay_trackNo_4",
  (char *)"path_replay_trackNo_5",
  (char *)"path_replay_trackNo_6",
  (char *)"path_replay_trackNo_7",
  (char *)"path_replay_trackNo_8",
  (char *)"path_replay_trackNo_9",
  (char *)"path_replay_trackNo_10",
  (char *)"path_replay_trackNo_11",
  (char *)"part_size",
  (char *)"part_size_pulse",
  (char *)"partSplat_texture",
  (char *)"part_acc",
  (char *)"part_acc_pulse",
  (char *)"part_damp",
  (char *)"part_damp_pulse",
  (char *)"part_gravity",
  (char *)"part_gravity_pulse",
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"noiseUpdateScale",
  (char *)"noiseUpdateScale_pulse",
  (char *)"noiseParticleScale",
  (char *)"noiseParticleScale_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
  (char *)"part_field_weight",
  (char *)"part_field_weight_pulse",
  (char *)"part_damp_targtRad",
  (char *)"part_timeToTargt",
  (char *)"particle_type",
  (char *)"partMove_target",
  (char *)"partMove_rand",
  (char *)"partExit_mode",
  (char *)"partStroke_mode",
  (char *)"partColor_mode",
  (char *)"pixel_acc",
  (char *)"pixel_acc_pulse",
  (char *)"pixel_acc_shiftX",
  (char *)"pixel_acc_shiftX_pulse",
  (char *)"pixel_acc_shiftY",
  (char *)"pixel_acc_shiftY_pulse",
  (char *)"pixel_radius",
  (char *)"pixel_radius_pulse",
  (char *)"pixel_mode",
  (char *)"repop_CA",
  (char *)"repop_CA_pulse",
  (char *)"repop_part",
  (char *)"repop_part_pulse",
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"BG_CA_repop_density",
  (char *)"Part_repop_density",
  (char *)"repop_colorBG",
  (char *)"BG_CA_repop_color_mode",
  (char *)"Part_repop_color_mode",
  (char *)"repop_colorBG_pulse",
  (char *)"repop_greyBG",
  (char *)"repop_greyBG_pulse",
  (char *)"repop_colorPart",
  (char *)"repop_colorPart_pulse",
  (char *)"repop_greyPart",
  (char *)"repop_greyPart_pulse",
  (char *)"repop_colorCA",
  (char *)"repop_colorCA_pulse",
  (char *)"repop_greyCA",
  (char *)"repop_greyCA_pulse",
  (char *)"cameraWB_R",
  (char *)"cameraWB_B",
  (char *)"cameraExposure",
  (char *)"cameraGain",
  (char *)"cameraBrightness",
  (char *)"cameraSaturation",
  (char *)"cameraContrast",
  (char *)"cameraGamma",
  (char *)"cameraCaptFreq",
  (char *)"camera_gamma",
  (char *)"camera_gamma_pulse",
  (char *)"cameraNo",
  (char *)"playing_movieNo",
  (char *)"movieCaptFreq",
  (char *)"playing_clipNameLeft",
  (char *)"playing_clipNameRight",
  (char *)"playing_secondClipNameLeft",
  (char *)"playing_secondClipNameRight",
  (char *)"clip_mix",
  (char *)"clipCaptFreq",
  (char *)"clip_scratch_factor",
  (char *)"clip_nudge_factor",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"activeClipArts",
  (char *)"playing_soundtrackNo",
  (char *)"cameraThreshold",
  (char *)"cameraThreshold_pulse",
  (char *)"cameraWeight",
  (char *)"cameraWeight_pulse",
  (char *)"cameraSobel",
  (char *)"cameraSobel_pulse",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"invertMovie",
  (char *)"invertCamera",
  (char *)"invertPhoto",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"movie_gamma",
  (char *)"movie_gamma_pulse",
  (char *)"movie_threshold",
  (char *)"movie_threshold_pulse",
  (char *)"video_gamma",
  (char *)"video_gamma_pulse",
  (char *)"video_threshold",
  (char *)"video_threshold_pulse",
  (char *)"video_contrast",
  (char *)"video_contrast_pulse",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photoSobel",
  (char *)"photoSobel_pulse",
  (char *)"photoJitterAmpl",
  (char *)"maskJitterAmpl",
  (char *)"photo_scaleX",
  (char *)"photo_scaleY",
  (char *)"photo_rot",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"photo_offsetX",
  (char *)"photo_offsetY",
  (char *)"mask_scale",
  (char *)"photo_contrast",
  (char *)"mask_contrast",
  (char *)"CAParams1",
  (char *)"CAParams1_pulse",
  (char *)"CAParams2",
  (char *)"CAParams2_pulse",
  (char *)"CAParams3",
  (char *)"CAParams3_pulse",
  (char *)"CAParams4",
  (char *)"CAParams4_pulse",
  (char *)"CAParams5",
  (char *)"CAParams5_pulse",
  (char *)"CAParams6",
  (char *)"CAParams6_pulse",
  (char *)"CAParams7",
  (char *)"CAParams7_pulse",
  (char *)"CAParams8",
  (char *)"CAParams8_pulse",
  (char *)"CA1Type",
  (char *)"CA1SubType",
  (char *)"CA2Type",
  (char *)"CA2SubType",
  (char *)"CA1_CA2_weight",
  (char *)"flashPartCA_freq",
  (char *)"flashTrkCA_freq_0",
  (char *)"flashTrkCA_freq_1",
  (char *)"flashTrkCA_freq_2",
  (char *)"flashTrkCA_freq_3",
  (char *)"flashCABG_freq",
  (char *)"flashPartBG_freq",
  (char *)"flashTrkBG_freq_1",
  (char *)"flashTrkBG_freq_2",
  (char *)"flashTrkBG_freq_3",
  (char *)"flashCAPart_freq",
  (char *)"flashTrkPart_freq_0",
  (char *)"flashTrkPart_freq_1",
  (char *)"flashTrkPart_freq_2",
  (char *)"flashTrkPart_freq_3",
  (char *)"flashParticleInit_freq",
  (char *)"flashPixel_freq",
  (char *)"flashPixel_duration",
  (char *)"flashCameraTrkLength",
  (char *)"flashCameraTrkBright",
  (char *)"flashCameraTrkBeat",
  (char *)"flashPhotoTrkLength",
  (char *)"flashPhotoTrkBright",
  (char *)"flashPhotoTrkBeat",
  (char *)"flashPhotoChangeBeat",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"master_mask_opacity_1",
  (char *)"master_mask_opacity_2",
  (char *)"master_mask_opacity_3",
  (char *)"master_mask_opacity_4",
  (char *)"master_mask_opacity_5",
  (char *)"master_mask_opacity_6",
  (char *)"beat_threshold",
  (char *)"beat_delay",
  (char *)"sound_volume",
  (char *)"sound_min",
  (char *)"pen_radius_pressure_coef",
  (char *)"pen_radius_angleHor_coef",
  (char *)"pen_radius_angleVer_coef",
  (char *)"tracksSync",
  (char *)"cameraCumul",
  (char *)"CAstep",
  (char *)"Pixelstep",
  (char *)"CAcolorSpread",
  (char *)"freeze",
  (char *)"sound_env_min",
  (char *)"sound_env_max",
  (char *)"audioInput_weight",
  (char *)"soundtrack_PD_weight",
  (char *)"soundtrack_PA_weight",
  (char *)"sensor_sample_setUp",
  (char *)"sensor_layout",
  (char *)"sensor_activation",
  (char *)"sensor_vol",
  (char *)"movie_loop",
  (char *)"path_replay_loop",
  (char *)"path_replay_speed",
  (char *)"path_scaleX",
  (char *)"path_scaleY",
  (char *)"path_translX",
  (char *)"path_translY",
  (char *)"master_crop_x",
  (char *)"master_crop_y",
  (char *)"master_crop_width",
  (char *)"blurRadius_1",
  (char *)"blurRadius_2",
  (char *)"video_white",
  (char *)"video_white_pulse",
  (char *)"screen_drawing_no",
  (char *)"master_mask",
  (char *)"master_mask_scale",
  (char *)"master_mask_scale_ratio",
  (char *)"master_mask_offsetX",
  (char *)"master_mask_offsetY",
  (char *)"video_hue",
  (char *)"video_hue_pulse",
  (char *)"photo_hue",
  (char *)"photo_hue_pulse",
  (char *)"penStrokeAtBeat",
  (char *)"path_group",
  (char *)"script_1",
  (char *)"moving_messages",
};
