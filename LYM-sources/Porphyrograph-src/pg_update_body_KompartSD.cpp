
 glUseProgram(shader_programme[pg_shader_Update]);
 glUniform4f( uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale ,
	      (GLfloat)pixel_mode,
	      (GLfloat)pixel_acc_factor,
	      (GLfloat)noiseScale,
	      (GLfloat)noiseLineScale );
 glUniform4f( uniform_Update_fs_4fv_noiseType_noiseAngleScale_noiseCenter_0_noiseCenter_1 ,
	      (GLfloat)noiseType,
	      (GLfloat)noiseAngleScale,
	      (GLfloat)noiseCenter_0,
	      (GLfloat)noiseCenter_1 );
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_repop_CA ,
	      (GLfloat)pixel_acc_center_0,
	      (GLfloat)pixel_acc_center_1,
	      (GLfloat)repop_BG,
	      (GLfloat)repop_CA );
 glUniform4f( uniform_Update_fs_4fv_CA1Type_CA1SubType_CAParams1_CAParams2 ,
	      (GLfloat)CA1Type,
	      (GLfloat)CA1SubType,
	      (GLfloat)CAParams1,
	      (GLfloat)CAParams2 );
 glUniform4f( uniform_Update_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6 ,
	      (GLfloat)CAParams3,
	      (GLfloat)CAParams4,
	      (GLfloat)CAParams5,
	      (GLfloat)CAParams6 );
 glUniform4f( uniform_Update_fs_4fv_CAParams7_CAParams8_currentDrawingTrack_currentVideoTrack ,
	      (GLfloat)CAParams7,
	      (GLfloat)CAParams8,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack );
 glUniform4f( uniform_Update_fs_4fv_currentPhotoTrack_path_replay_trackNo_1_path_replay_trackNo_2_path_replay_trackNo_3 ,
	      (GLfloat)currentPhotoTrack,
	      (GLfloat)path_replay_trackNo_1,
	      (GLfloat)path_replay_trackNo_2,
	      (GLfloat)path_replay_trackNo_3 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_4_path_replay_trackNo_5_path_replay_trackNo_6_path_replay_trackNo_7 ,
	      (GLfloat)path_replay_trackNo_4,
	      (GLfloat)path_replay_trackNo_5,
	      (GLfloat)path_replay_trackNo_6,
	      (GLfloat)path_replay_trackNo_7 );
 glUniform4f( uniform_Update_fs_4fv_CAdecay_pulse_invertMovie_cameraCumul_cameraThreshold ,
	      (GLfloat)CAdecay_pulse,
	      (GLfloat)invertMovie,
	      (GLfloat)cameraCumul,
	      (GLfloat)cameraThreshold );
 glUniform4f( uniform_Update_fs_4fv_cameraGamma_video_satur_video_satur_pulse_cameraWeight ,
	      (GLfloat)cameraGamma,
	      (GLfloat)video_satur,
	      (GLfloat)video_satur_pulse,
	      (GLfloat)cameraWeight );
 glUniform4f( uniform_Update_fs_4fv_movieWeight_cameraSobel_movieSobel_BGSubtr ,
	      (GLfloat)movieWeight,
	      (GLfloat)cameraSobel,
	      (GLfloat)movieSobel,
	      (GLfloat)BGSubtr );
 glUniform4f( uniform_Update_fs_4fv_CAstep_CAcolorSpread_freeze_photo_value ,
	      (GLfloat)CAstep,
	      (GLfloat)CAcolorSpread,
	      (GLfloat)freeze,
	      (GLfloat)photo_value );
 glUniform4f( uniform_Update_fs_4fv_photo_value_pulse_photo_satur_photo_satur_pulse_mask_scale ,
	      (GLfloat)photo_value_pulse,
	      (GLfloat)photo_satur,
	      (GLfloat)photo_satur_pulse,
	      (GLfloat)mask_scale );
 glUniform3f( uniform_Update_fs_3fv_photo_scale_mask_contrast_photo_contrast ,
	      (GLfloat)photo_scale,
	      (GLfloat)mask_contrast,
	      (GLfloat)photo_contrast );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_echo_echoNeg_CAMixingWeight_trackMixingWeight_0 ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)CAMixingWeight,
	      (GLfloat)trackMixingWeight_0 );
 glUniform1f( uniform_Mixing_fs_1fv_trackMixingWeight_1 ,
	      (GLfloat)trackMixingWeight_1 );

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)CAMasterWeight );
 glUniform4f( uniform_Master_fs_4fv_trackMasterWeight_0_trackMasterWeight_1_interfaceOnScreen_hide ,
	      (GLfloat)trackMasterWeight_0,
	      (GLfloat)trackMasterWeight_1,
	      (GLfloat)interfaceOnScreen,
	      (GLfloat)hide );
 glUniform1f( uniform_Master_fs_1fv_mute_screen ,
	      (GLfloat)mute_screen );
