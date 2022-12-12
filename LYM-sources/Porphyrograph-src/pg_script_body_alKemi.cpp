bool  double_window       ;
bool  wide_screen         ;
int   window_width        ;
int   window_height       ;
int   window_x            ;
int   window_y            ;
float minimal_interframe_latency;
float time_scale          ;
float initial_time        ;
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
bool  mute_second_screen  ;
bool  invertAllLayers     ;
bool  take_snapshots      ;
float cursorSize          ;
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
float echo                ;
float echo_pulse          ;
float echoNeg             ;
float echoNeg_pulse       ;
float CAdecay             ;
float CAdecay_pulse       ;
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
float pen_saturation_replay;
float pen_saturation_replay_pulse;
float pen_value_replay    ;
float pen_value_replay_pulse;
float pen_hue_replay      ;
float pen_hue_replay_pulse;
int   pen_brush_replay    ;
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
float pulsed_part_Hshift  ;
float pulsed_part_Vshift  ;
float noiseScale          ;
float noiseScale_pulse    ;
int   noiseType           ;
float noiseLineScale      ;
float noiseAngleScale     ;
float noiseCenterX        ;
float noiseCenterY        ;
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
int   playing_movieNo     ;
float movieCaptFreq       ;
int   photo_diaporama     ;
float photo_diaporama_fade;
float photo_diaporama_plateau;
int   activeClipArts      ;
int   playing_soundtrackNo;
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
float photo_satur         ;
float photo_satur_pulse   ;
float photo_value         ;
float photo_value_pulse   ;
float photo_gamma         ;
float photo_gamma_pulse   ;
float photo_threshold     ;
float photo_threshold_pulse;
float photoJitterAmpl     ;
float maskJitterAmpl      ;
float photoWeight         ;
float photoWeight_pulse   ;
float photo_scale         ;
float photo_rot           ;
float photo_transl_x      ;
float photo_transl_y      ;
float mask_scale          ;
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
int   flashTrkCA_freq_0   ;
int   flashTrkCA_freq_1   ;
int   flashTrkCA_freq_2   ;
int   flashCABG_freq      ;
int   flashTrkBG_freq_1   ;
int   flashTrkBG_freq_2   ;
int   flashPixel_freq     ;
int   flashPixel_duration ;
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
float master_scale        ;
float master_scale_pulse  ;
float master_scale_ratio  ;
float master_offsetX      ;
float master_offsetY      ;
bool  freeze              ;
float sound_env_min       ;
float sound_env_max       ;
float audioInput_weight   ;
float soundtrack_weight   ;
bool  MIDIwithBeat        ;
bool  MIDIwithColor       ;
bool  MIDIwithBrush       ;
bool  MIDIwithPhotoFlash  ;
bool  path_replay_loop    ;
bool  movie_loop          ;
VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { 
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
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
	_pg_int,
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
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
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
	(void *)&echo,
	(void *)&echo_pulse,
	(void *)&echoNeg,
	(void *)&echoNeg_pulse,
	(void *)&CAdecay,
	(void *)&CAdecay_pulse,
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
	(void *)&pen_saturation_replay,
	(void *)&pen_saturation_replay_pulse,
	(void *)&pen_value_replay,
	(void *)&pen_value_replay_pulse,
	(void *)&pen_hue_replay,
	(void *)&pen_hue_replay_pulse,
	(void *)&pen_brush_replay,
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
	(void *)&pulsed_part_Hshift,
	(void *)&pulsed_part_Vshift,
	(void *)&noiseScale,
	(void *)&noiseScale_pulse,
	(void *)&noiseType,
	(void *)&noiseLineScale,
	(void *)&noiseAngleScale,
	(void *)&noiseCenterX,
	(void *)&noiseCenterY,
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
	(void *)&playing_movieNo,
	(void *)&movieCaptFreq,
	(void *)&photo_diaporama,
	(void *)&photo_diaporama_fade,
	(void *)&photo_diaporama_plateau,
	(void *)&activeClipArts,
	(void *)&playing_soundtrackNo,
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
	(void *)&photo_satur,
	(void *)&photo_satur_pulse,
	(void *)&photo_value,
	(void *)&photo_value_pulse,
	(void *)&photo_gamma,
	(void *)&photo_gamma_pulse,
	(void *)&photo_threshold,
	(void *)&photo_threshold_pulse,
	(void *)&photoJitterAmpl,
	(void *)&maskJitterAmpl,
	(void *)&photoWeight,
	(void *)&photoWeight_pulse,
	(void *)&photo_scale,
	(void *)&photo_rot,
	(void *)&photo_transl_x,
	(void *)&photo_transl_y,
	(void *)&mask_scale,
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
	(void *)&flashTrkCA_freq_0,
	(void *)&flashTrkCA_freq_1,
	(void *)&flashTrkCA_freq_2,
	(void *)&flashCABG_freq,
	(void *)&flashTrkBG_freq_1,
	(void *)&flashTrkBG_freq_2,
	(void *)&flashPixel_freq,
	(void *)&flashPixel_duration,
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
	(void *)&master_scale,
	(void *)&master_scale_pulse,
	(void *)&master_scale_ratio,
	(void *)&master_offsetX,
	(void *)&master_offsetY,
	(void *)&freeze,
	(void *)&sound_env_min,
	(void *)&sound_env_max,
	(void *)&audioInput_weight,
	(void *)&soundtrack_weight,
	(void *)&MIDIwithBeat,
	(void *)&MIDIwithColor,
	(void *)&MIDIwithBrush,
	(void *)&MIDIwithPhotoFlash,
	(void *)&path_replay_loop,
	(void *)&movie_loop,
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
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void pen_brush_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	pen_brush_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
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
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_movieNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_movieNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void photo_diaporama_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	photo_diaporama_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_soundtrackNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_soundtrackNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
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
void soundtrack_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void soundtrack_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	soundtrack_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void MIDIwithBeat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void MIDIwithBeat_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	MIDIwithBeat_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void MIDIwithColor_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void MIDIwithColor_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	MIDIwithColor_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void MIDIwithBrush_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void MIDIwithBrush_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	MIDIwithBrush_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void MIDIwithPhotoFlash_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void MIDIwithPhotoFlash_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	MIDIwithPhotoFlash_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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
	&playing_movieNo_callBack_generic,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&sound_env_min_callBack_generic,
	&sound_env_max_callBack_generic,
	&audioInput_weight_callBack_generic,
	&soundtrack_weight_callBack_generic,
	&MIDIwithBeat_callBack_generic,
	&MIDIwithColor_callBack_generic,
	&MIDIwithBrush_callBack_generic,
	&MIDIwithPhotoFlash_callBack_generic,
	NULL,
	NULL,
};
char *ScenarioVarMessages[_MaxInterpVarIDs] = { 
  (char *)"auto_beat",
  (char *)"auto_pulse",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"mute_second_screen",
  (char *)"invertAllLayers",
  (char *)"take_snapshots",
  (char *)"cursorSize",
  (char *)"auto_beat_duration",
  (char *)"master",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"trackMixingWeight_0",
  (char *)"trackMixingWeight_0_pulse",
  (char *)"trackMixingWeight_1",
  (char *)"trackMixingWeight_1_pulse",
  (char *)"trackMixingWeight_2",
  (char *)"trackMixingWeight_2_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"trackMasterWeight_0",
  (char *)"trackMasterWeight_0_pulse",
  (char *)"trackMasterWeight_1",
  (char *)"trackMasterWeight_1_pulse",
  (char *)"trackMasterWeight_2",
  (char *)"trackMasterWeight_2_pulse",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
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
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_brush_replay",
  (char *)"currentDrawingTrack",
  (char *)"currentVideoTrack",
  (char *)"currentPhotoTrack",
  (char *)"track_x_transl_0",
  (char *)"track_x_transl_0_pulse",
  (char *)"track_y_transl_0",
  (char *)"track_y_transl_0_pulse",
  (char *)"track_x_transl_1",
  (char *)"track_x_transl_1_pulse",
  (char *)"track_y_transl_1",
  (char *)"track_y_transl_1_pulse",
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
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"noiseScale",
  (char *)"noiseScale_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
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
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"repop_colorBG",
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
  (char *)"playing_movieNo",
  (char *)"movieCaptFreq",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"activeClipArts",
  (char *)"playing_soundtrackNo",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"invertMovie",
  (char *)"invertCamera",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photoJitterAmpl",
  (char *)"maskJitterAmpl",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"photo_scale",
  (char *)"photo_rot",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"mask_scale",
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
  (char *)"flashTrkCA_freq_0",
  (char *)"flashTrkCA_freq_1",
  (char *)"flashTrkCA_freq_2",
  (char *)"flashCABG_freq",
  (char *)"flashTrkBG_freq_1",
  (char *)"flashTrkBG_freq_2",
  (char *)"flashPixel_freq",
  (char *)"flashPixel_duration",
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
  (char *)"interfaceOnScreen",
  (char *)"CAcolorSpread",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"freeze",
  (char *)"sound_env_min",
  (char *)"sound_env_max",
  (char *)"audioInput_weight",
  (char *)"soundtrack_weight",
  (char *)"MIDIwithBeat",
  (char *)"MIDIwithColor",
  (char *)"MIDIwithBrush",
  (char *)"MIDIwithPhotoFlash",
  (char *)"path_replay_loop",
  (char *)"movie_loop",
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
  _pg_pulsed_absolute,
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
};
char *ScenarioVarStrings[_MaxInterpVarIDs] = { 
  (char *)"auto_beat",
  (char *)"auto_pulse",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"mute_second_screen",
  (char *)"invertAllLayers",
  (char *)"take_snapshots",
  (char *)"cursorSize",
  (char *)"auto_beat_duration",
  (char *)"master",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"trackMixingWeight_0",
  (char *)"trackMixingWeight_0_pulse",
  (char *)"trackMixingWeight_1",
  (char *)"trackMixingWeight_1_pulse",
  (char *)"trackMixingWeight_2",
  (char *)"trackMixingWeight_2_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"trackMasterWeight_0",
  (char *)"trackMasterWeight_0_pulse",
  (char *)"trackMasterWeight_1",
  (char *)"trackMasterWeight_1_pulse",
  (char *)"trackMasterWeight_2",
  (char *)"trackMasterWeight_2_pulse",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
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
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_brush_replay",
  (char *)"currentDrawingTrack",
  (char *)"currentVideoTrack",
  (char *)"currentPhotoTrack",
  (char *)"track_x_transl_0",
  (char *)"track_x_transl_0_pulse",
  (char *)"track_y_transl_0",
  (char *)"track_y_transl_0_pulse",
  (char *)"track_x_transl_1",
  (char *)"track_x_transl_1_pulse",
  (char *)"track_y_transl_1",
  (char *)"track_y_transl_1_pulse",
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
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"noiseScale",
  (char *)"noiseScale_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
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
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"repop_colorBG",
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
  (char *)"playing_movieNo",
  (char *)"movieCaptFreq",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"activeClipArts",
  (char *)"playing_soundtrackNo",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"invertMovie",
  (char *)"invertCamera",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photoJitterAmpl",
  (char *)"maskJitterAmpl",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"photo_scale",
  (char *)"photo_rot",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"mask_scale",
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
  (char *)"flashTrkCA_freq_0",
  (char *)"flashTrkCA_freq_1",
  (char *)"flashTrkCA_freq_2",
  (char *)"flashCABG_freq",
  (char *)"flashTrkBG_freq_1",
  (char *)"flashTrkBG_freq_2",
  (char *)"flashPixel_freq",
  (char *)"flashPixel_duration",
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
  (char *)"interfaceOnScreen",
  (char *)"CAcolorSpread",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"freeze",
  (char *)"sound_env_min",
  (char *)"sound_env_max",
  (char *)"audioInput_weight",
  (char *)"soundtrack_weight",
  (char *)"MIDIwithBeat",
  (char *)"MIDIwithColor",
  (char *)"MIDIwithBrush",
  (char *)"MIDIwithPhotoFlash",
  (char *)"path_replay_loop",
  (char *)"movie_loop",
};
