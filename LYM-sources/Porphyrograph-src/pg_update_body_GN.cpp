
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
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_pixel_radius_repop_BG ,
	      (GLfloat)pixel_acc_center_0,
	      (GLfloat)pixel_acc_center_1,
	      (GLfloat)pixel_radius,
	      (GLfloat)repop_BG );
 glUniform4f( uniform_Update_fs_4fv_repop_CA_CAParams1_CAParams2_CAParams3 ,
	      (GLfloat)repop_CA,
	      (GLfloat)CAParams1,
	      (GLfloat)CAParams2,
	      (GLfloat)CAParams3 );
 glUniform4f( uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_currentSvgGpuImages ,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack,
	      (GLfloat)currentPhotoTrack,
	      (GLfloat)currentSvgGpuImages );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_1_path_replay_trackNo_2_path_replay_trackNo_3_CAdecay_pulse ,
	      (GLfloat)path_replay_trackNo_1,
	      (GLfloat)path_replay_trackNo_2,
	      (GLfloat)path_replay_trackNo_3,
	      (GLfloat)CAdecay_pulse );
 glUniform4f( uniform_Update_fs_4fv_invertMovie_cameraCumul_cameraThreshold_cameraGamma ,
	      (GLfloat)invertMovie,
	      (GLfloat)cameraCumul,
	      (GLfloat)cameraThreshold,
	      (GLfloat)cameraGamma );
 glUniform4f( uniform_Update_fs_4fv_video_satur_video_satur_pulse_cameraWeight_movieWeight ,
	      (GLfloat)video_satur,
	      (GLfloat)video_satur_pulse,
	      (GLfloat)cameraWeight,
	      (GLfloat)movieWeight );
 glUniform4f( uniform_Update_fs_4fv_cameraSobel_movieSobel_BGSubtr_CAstep ,
	      (GLfloat)cameraSobel,
	      (GLfloat)movieSobel,
	      (GLfloat)BGSubtr,
	      (GLfloat)CAstep );
 glUniform4f( uniform_Update_fs_4fv_CAcolorSpread_freeze_photo_value_photo_value_pulse ,
	      (GLfloat)CAcolorSpread,
	      (GLfloat)freeze,
	      (GLfloat)photo_value,
	      (GLfloat)photo_value_pulse );
 glUniform1f( uniform_Update_fs_1fv_photo_scale ,
	      (GLfloat)photo_scale );

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
