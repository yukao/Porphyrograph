
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
 glUniform4f( uniform_Update_fs_4fv_currentPhotoTrack_currentSvgGpuImages_path_replay_trackNo_1_path_replay_trackNo_2 ,
	      (GLfloat)currentPhotoTrack,
	      (GLfloat)currentSvgGpuImages,
	      (GLfloat)path_replay_trackNo_1,
	      (GLfloat)path_replay_trackNo_2 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_3_CAdecay_pulse_invertMovie_cameraCumul ,
	      (GLfloat)path_replay_trackNo_3,
	      (GLfloat)CAdecay_pulse,
	      (GLfloat)invertMovie,
	      (GLfloat)cameraCumul );
 glUniform4f( uniform_Update_fs_4fv_cameraThreshold_cameraGamma_video_satur_video_satur_pulse ,
	      (GLfloat)cameraThreshold,
	      (GLfloat)cameraGamma,
	      (GLfloat)video_satur,
	      (GLfloat)video_satur_pulse );
 glUniform4f( uniform_Update_fs_4fv_cameraWeight_movieWeight_cameraSobel_movieSobel ,
	      (GLfloat)cameraWeight,
	      (GLfloat)movieWeight,
	      (GLfloat)cameraSobel,
	      (GLfloat)movieSobel );
 glUniform4f( uniform_Update_fs_4fv_BGSubtr_CAstep_CAcolorSpread_freeze ,
	      (GLfloat)BGSubtr,
	      (GLfloat)CAstep,
	      (GLfloat)CAcolorSpread,
	      (GLfloat)freeze );
 glUniform4f( uniform_Update_fs_4fv_photo_value_photo_value_pulse_photo_satur_photo_satur_pulse ,
	      (GLfloat)photo_value,
	      (GLfloat)photo_value_pulse,
	      (GLfloat)photo_satur,
	      (GLfloat)photo_satur_pulse );
 glUniform4f( uniform_Update_fs_4fv_photo_scale_mask_contrast_photo_contrast_fft_scale ,
	      (GLfloat)photo_scale,
	      (GLfloat)mask_contrast,
	      (GLfloat)photo_contrast,
	      (GLfloat)fft_scale );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_echo_echoNeg_CAMixingWeight_trackMixingWeight_0 ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)CAMixingWeight,
	      (GLfloat)trackMixingWeight_0 );

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)CAMasterWeight );
 glUniform4f( uniform_Master_fs_4fv_trackMasterWeight_0_interfaceOnScreen_hide_mute_screen ,
	      (GLfloat)trackMasterWeight_0,
	      (GLfloat)interfaceOnScreen,
	      (GLfloat)hide,
	      (GLfloat)mute_screen );
