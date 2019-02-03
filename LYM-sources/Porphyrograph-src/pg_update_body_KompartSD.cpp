
 glUseProgram(shader_programme[pg_shader_Update]);
 glUniform4f( uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_noiseLineScale ,
	      (GLfloat)pixel_mode,
	      (GLfloat)pixel_acc_factor,
	      (GLfloat)noiseScale,
	      (GLfloat)noiseLineScale );
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_center_0_pixel_acc_center_1_repop_BG_currentDrawingTrack ,
	      (GLfloat)pixel_acc_center_0,
	      (GLfloat)pixel_acc_center_1,
	      (GLfloat)repop_BG,
	      (GLfloat)currentDrawingTrack );
 glUniform4f( uniform_Update_fs_4fv_currentVideoTrack_currentPhotoTrack_activeClipArts_path_replay_trackNo_1 ,
	      (GLfloat)currentVideoTrack,
	      (GLfloat)currentPhotoTrack,
	      (GLfloat)activeClipArts,
	      (GLfloat)path_replay_trackNo_1 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4_path_replay_trackNo_5 ,
	      (GLfloat)path_replay_trackNo_2,
	      (GLfloat)path_replay_trackNo_3,
	      (GLfloat)path_replay_trackNo_4,
	      (GLfloat)path_replay_trackNo_5 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_6_path_replay_trackNo_7_CAdecay_pulse_invertMovie ,
	      (GLfloat)path_replay_trackNo_6,
	      (GLfloat)path_replay_trackNo_7,
	      (GLfloat)CAdecay_pulse,
	      (GLfloat)invertMovie );
 glUniform4f( uniform_Update_fs_4fv_cameraCumul_cameraThreshold_cameraGamma_video_satur ,
	      (GLfloat)cameraCumul,
	      (GLfloat)cameraThreshold,
	      (GLfloat)cameraGamma,
	      (GLfloat)video_satur );
 glUniform4f( uniform_Update_fs_4fv_video_satur_pulse_cameraWeight_movieWeight_cameraSobel ,
	      (GLfloat)video_satur_pulse,
	      (GLfloat)cameraWeight,
	      (GLfloat)movieWeight,
	      (GLfloat)cameraSobel );
 glUniform4f( uniform_Update_fs_4fv_movieSobel_BGSubtr_CAstep_CAcolorSpread ,
	      (GLfloat)movieSobel,
	      (GLfloat)BGSubtr,
	      (GLfloat)CAstep,
	      (GLfloat)CAcolorSpread );
 glUniform1f( uniform_Update_fs_1fv_freeze ,
	      (GLfloat)freeze );

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
