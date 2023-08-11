bool  double_window       ;
bool  wide_screen         ;
int   window_width        ;
int   window_height       ;
int   window_x            ;
int   window_y            ;
float minimal_interframe_latency;
float time_scale          ;
float initial_time        ;
int   camID               ;
int   message_pixel_length;
bool  trace_output_frame_number;
bool  trace_time          ;
int   first_frame_number  ;
int   last_frame_number   ;
int   max_mouse_recording_frames;
int   max_network_message_length;
int   nb_particles        ;
VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs] = { 
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
};
void * ConfigurationVarPointers[_MaxConfigurationVarIDs] = { 
	(void *)&double_window,
	(void *)&wide_screen,
	(void *)&window_width,
	(void *)&window_height,
	(void *)&window_x,
	(void *)&window_y,
	(void *)&minimal_interframe_latency,
	(void *)&time_scale,
	(void *)&initial_time,
	(void *)&camID,
	(void *)&message_pixel_length,
	(void *)&trace_output_frame_number,
	(void *)&trace_time,
	(void *)&first_frame_number,
	(void *)&last_frame_number,
	(void *)&max_mouse_recording_frames,
	(void *)&max_network_message_length,
	(void *)&nb_particles,
};
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
float echo                ;
float echo_pulse          ;
float echoNeg             ;
float echoNeg_pulse       ;
int   blurRadius_1        ;
int   blurRadius_2        ;
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
float pen_color           ;
float pen_color_pulse     ;
float pen_grey            ;
float pen_grey_pulse      ;
float pen_color_a         ;
float pen_color_a_pulse   ;
int   pen_brush           ;
float pen_radius_replay   ;
float pen_radius_replay_pulse;
float pen_color_replay    ;
float pen_color_replay_pulse;
float pen_grey_replay     ;
float pen_grey_replay_pulse;
float pen_saturation_replay;
float pen_saturation_replay_pulse;
float pen_value_replay    ;
float pen_value_replay_pulse;
int   currentDrawingTrack ;
int   currentVideoTrack   ;
int   currentPhotoTrack   ;
float track_x_transl_0    ;
float track_x_transl_0_pulse;
float track_y_transl_0    ;
float track_y_transl_0_pulse;
float track_x_transl_1    ;
float track_x_transl_1_pulse;
float track_y_transl_1    ;
float track_y_transl_1_pulse;
int   part_initialization ;
bool  part_path_repop_0   ;
bool  part_path_repop_1   ;
bool  part_path_repop_2   ;
bool  part_path_repop_3   ;
bool  part_path_repop_4   ;
bool  part_path_repop_5   ;
bool  part_path_repop_6   ;
bool  part_path_repop_7   ;
bool  part_path_follow_0  ;
bool  part_path_follow_1  ;
bool  part_path_follow_2  ;
bool  part_path_follow_3  ;
bool  part_path_follow_4  ;
bool  part_path_follow_5  ;
bool  part_path_follow_6  ;
bool  part_path_follow_7  ;
bool  part_path_repulse_0 ;
bool  part_path_repulse_1 ;
bool  part_path_repulse_2 ;
bool  part_path_repulse_3 ;
bool  part_path_repulse_4 ;
bool  part_path_repulse_5 ;
bool  part_path_repulse_6 ;
bool  part_path_repulse_7 ;
bool  path_record_1       ;
bool  path_record_2       ;
bool  path_record_3       ;
bool  path_record_4       ;
bool  path_record_5       ;
bool  path_record_6       ;
bool  path_record_7       ;
int   path_replay_trackNo_1;
int   path_replay_trackNo_2;
int   path_replay_trackNo_3;
int   path_replay_trackNo_4;
int   path_replay_trackNo_5;
int   path_replay_trackNo_6;
int   path_replay_trackNo_7;
float part_size           ;
float part_size_pulse     ;
float part_acc            ;
float part_acc_pulse      ;
int   part_image_acceleration;
float part_damp           ;
float part_damp_pulse     ;
float pulsed_part_Hshift  ;
float pulsed_part_Vshift  ;
float noiseScale          ;
float noiseScale_pulse    ;
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
float repop_colorBG       ;
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
int   playing_movieNo     ;
float movieCaptFreq       ;
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
float video_satur         ;
float video_satur_pulse   ;
float video_value         ;
float video_value_pulse   ;
float video_white         ;
float video_white_pulse   ;
float photoWeight         ;
float photoWeight_pulse   ;
float photo_satur         ;
float photo_satur_pulse   ;
float photo_value         ;
float photo_value_pulse   ;
float photoJitterAmpl     ;
float maskJitterAmpl      ;
float photo_scale         ;
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
int   flashPhotoChangeBeat;
float flashCameraTrkLength;
float flashCameraTrkBright;
int   flashCameraTrkBeat  ;
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
bool  interfaceOnScreen   ;
bool  CAcolorSpread       ;
bool  freeze              ;
float sound_env_min       ;
float sound_env_max       ;
float audioInput_weight   ;
float soundtrack_weight   ;
float sensor_sample_setUp ;
int   sensor_layout       ;
int   sensor_activation   ;
float sensor_vol          ;
bool  movie_loop          ;
bool  path_replay_loop    ;
int   screen_drawing_no   ;
float light1_color        ;
float light1_color_pulse  ;
float light1_grey         ;
float light1_grey_pulse   ;
float light1_dimmer_pulse ;
float light1_dimmer       ;
float light1_strobe       ;
float light2_dimmer       ;
float light3_dimmer       ;
float light4_dimmer       ;
float light5_dimmer       ;
float light1_strobe_pulse ;
float light2_color        ;
float light2_color_pulse  ;
float light2_grey         ;
float light2_grey_pulse   ;
float light2_dimmer_pulse ;
float light2_strobe       ;
float light2_strobe_pulse ;
float light3_color        ;
float light3_color_pulse  ;
float light3_grey         ;
float light3_grey_pulse   ;
float light3_dimmer_pulse ;
float light3_strobe       ;
float light3_strobe_pulse ;
float light4_color        ;
float light4_color_pulse  ;
float light4_grey         ;
float light4_grey_pulse   ;
float light4_dimmer_pulse ;
float light4_strobe       ;
float light4_strobe_pulse ;
float light5_color        ;
float light5_color_pulse  ;
float light5_grey         ;
float light5_grey_pulse   ;
float light5_dimmer_pulse ;
float light5_strobe       ;
float light5_strobe_pulse ;
float master_mask         ;
bool  MIDIwithBeat        ;
bool  MIDIwithColor       ;
bool  MIDIwithBrush       ;
bool  MIDIwithCameraFlash ;
bool  MIDIwithPhotoFlash  ;
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
	_pg_int ,
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
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
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
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int ,
	_pg_int ,
	_pg_float,
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
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
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
	(void *)&echo,
	(void *)&echo_pulse,
	(void *)&echoNeg,
	(void *)&echoNeg_pulse,
	(void *)&blurRadius_1,
	(void *)&blurRadius_2,
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
	(void *)&pen_color,
	(void *)&pen_color_pulse,
	(void *)&pen_grey,
	(void *)&pen_grey_pulse,
	(void *)&pen_color_a,
	(void *)&pen_color_a_pulse,
	(void *)&pen_brush,
	(void *)&pen_radius_replay,
	(void *)&pen_radius_replay_pulse,
	(void *)&pen_color_replay,
	(void *)&pen_color_replay_pulse,
	(void *)&pen_grey_replay,
	(void *)&pen_grey_replay_pulse,
	(void *)&pen_saturation_replay,
	(void *)&pen_saturation_replay_pulse,
	(void *)&pen_value_replay,
	(void *)&pen_value_replay_pulse,
	(void *)&currentDrawingTrack,
	(void *)&currentVideoTrack,
	(void *)&currentPhotoTrack,
	(void *)&track_x_transl_0,
	(void *)&track_x_transl_0_pulse,
	(void *)&track_y_transl_0,
	(void *)&track_y_transl_0_pulse,
	(void *)&track_x_transl_1,
	(void *)&track_x_transl_1_pulse,
	(void *)&track_y_transl_1,
	(void *)&track_y_transl_1_pulse,
	(void *)&part_initialization,
	(void *)&part_path_repop_0,
	(void *)&part_path_repop_1,
	(void *)&part_path_repop_2,
	(void *)&part_path_repop_3,
	(void *)&part_path_repop_4,
	(void *)&part_path_repop_5,
	(void *)&part_path_repop_6,
	(void *)&part_path_repop_7,
	(void *)&part_path_follow_0,
	(void *)&part_path_follow_1,
	(void *)&part_path_follow_2,
	(void *)&part_path_follow_3,
	(void *)&part_path_follow_4,
	(void *)&part_path_follow_5,
	(void *)&part_path_follow_6,
	(void *)&part_path_follow_7,
	(void *)&part_path_repulse_0,
	(void *)&part_path_repulse_1,
	(void *)&part_path_repulse_2,
	(void *)&part_path_repulse_3,
	(void *)&part_path_repulse_4,
	(void *)&part_path_repulse_5,
	(void *)&part_path_repulse_6,
	(void *)&part_path_repulse_7,
	(void *)&path_record_1,
	(void *)&path_record_2,
	(void *)&path_record_3,
	(void *)&path_record_4,
	(void *)&path_record_5,
	(void *)&path_record_6,
	(void *)&path_record_7,
	(void *)&path_replay_trackNo_1,
	(void *)&path_replay_trackNo_2,
	(void *)&path_replay_trackNo_3,
	(void *)&path_replay_trackNo_4,
	(void *)&path_replay_trackNo_5,
	(void *)&path_replay_trackNo_6,
	(void *)&path_replay_trackNo_7,
	(void *)&part_size,
	(void *)&part_size_pulse,
	(void *)&part_acc,
	(void *)&part_acc_pulse,
	(void *)&part_image_acceleration,
	(void *)&part_damp,
	(void *)&part_damp_pulse,
	(void *)&pulsed_part_Hshift,
	(void *)&pulsed_part_Vshift,
	(void *)&noiseScale,
	(void *)&noiseScale_pulse,
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
	(void *)&repop_colorBG,
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
	(void *)&playing_movieNo,
	(void *)&movieCaptFreq,
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
	(void *)&video_satur,
	(void *)&video_satur_pulse,
	(void *)&video_value,
	(void *)&video_value_pulse,
	(void *)&video_white,
	(void *)&video_white_pulse,
	(void *)&photoWeight,
	(void *)&photoWeight_pulse,
	(void *)&photo_satur,
	(void *)&photo_satur_pulse,
	(void *)&photo_value,
	(void *)&photo_value_pulse,
	(void *)&photoJitterAmpl,
	(void *)&maskJitterAmpl,
	(void *)&photo_scale,
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
	(void *)&flashPhotoChangeBeat,
	(void *)&flashCameraTrkLength,
	(void *)&flashCameraTrkBright,
	(void *)&flashCameraTrkBeat,
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
	(void *)&interfaceOnScreen,
	(void *)&CAcolorSpread,
	(void *)&freeze,
	(void *)&sound_env_min,
	(void *)&sound_env_max,
	(void *)&audioInput_weight,
	(void *)&soundtrack_weight,
	(void *)&sensor_sample_setUp,
	(void *)&sensor_layout,
	(void *)&sensor_activation,
	(void *)&sensor_vol,
	(void *)&movie_loop,
	(void *)&path_replay_loop,
	(void *)&screen_drawing_no,
	(void *)&light1_color,
	(void *)&light1_color_pulse,
	(void *)&light1_grey,
	(void *)&light1_grey_pulse,
	(void *)&light1_dimmer_pulse,
	(void *)&light1_dimmer,
	(void *)&light1_strobe,
	(void *)&light2_dimmer,
	(void *)&light3_dimmer,
	(void *)&light4_dimmer,
	(void *)&light5_dimmer,
	(void *)&light1_strobe_pulse,
	(void *)&light2_color,
	(void *)&light2_color_pulse,
	(void *)&light2_grey,
	(void *)&light2_grey_pulse,
	(void *)&light2_dimmer_pulse,
	(void *)&light2_strobe,
	(void *)&light2_strobe_pulse,
	(void *)&light3_color,
	(void *)&light3_color_pulse,
	(void *)&light3_grey,
	(void *)&light3_grey_pulse,
	(void *)&light3_dimmer_pulse,
	(void *)&light3_strobe,
	(void *)&light3_strobe_pulse,
	(void *)&light4_color,
	(void *)&light4_color_pulse,
	(void *)&light4_grey,
	(void *)&light4_grey_pulse,
	(void *)&light4_dimmer_pulse,
	(void *)&light4_strobe,
	(void *)&light4_strobe_pulse,
	(void *)&light5_color,
	(void *)&light5_color_pulse,
	(void *)&light5_grey,
	(void *)&light5_grey_pulse,
	(void *)&light5_dimmer_pulse,
	(void *)&light5_strobe,
	(void *)&light5_strobe_pulse,
	(void *)&master_mask,
	(void *)&MIDIwithBeat,
	(void *)&MIDIwithColor,
	(void *)&MIDIwithBrush,
	(void *)&MIDIwithCameraFlash,
	(void *)&MIDIwithPhotoFlash,
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
