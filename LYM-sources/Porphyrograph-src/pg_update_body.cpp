
 glUseProgram(shader_programme[pg_shader_ParticleAnimation]);
 glUniform4f( uniform_ParticleAnimation_fs_4fv_noiseScale_pixel_acc_center_0_pixel_acc_center_1_part_path_follow_0 ,
	      (GLfloat)noiseScale,
	      (GLfloat)pixel_acc_center_0,
	      (GLfloat)pixel_acc_center_1,
	      (GLfloat)part_path_follow_0 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_follow_1_part_path_follow_2_part_path_follow_3_part_path_repulse_0 ,
	      (GLfloat)part_path_follow_1,
	      (GLfloat)part_path_follow_2,
	      (GLfloat)part_path_follow_3,
	      (GLfloat)part_path_repulse_0 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_repulse_1_part_path_repulse_2_part_path_repulse_3_freeze ,
	      (GLfloat)part_path_repulse_1,
	      (GLfloat)part_path_repulse_2,
	      (GLfloat)part_path_repulse_3,
	      (GLfloat)freeze );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_initialization_partMove_target_partMove_rand_partExit_mode ,
	      (GLfloat)part_initialization,
	      (GLfloat)partMove_target,
	      (GLfloat)partMove_rand,
	      (GLfloat)partExit_mode );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_part_damp_targtRad_part_timeToTargt ,
	      (GLfloat)partStroke_mode,
	      (GLfloat)partColor_mode,
	      (GLfloat)part_damp_targtRad,
	      (GLfloat)part_timeToTargt );
 glUniform2f( uniform_ParticleAnimation_fs_2fv_part_field_weight_partRepopRadius ,
	      (GLfloat)part_field_weight,
	      (GLfloat)partRepopRadius );

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
 glUniform4f( uniform_Update_fs_4fv_currentDrawingTrack_currentVideoTrack_currentPhotoTrack_path_replay_trackNo_1 ,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack,
	      (GLfloat)currentPhotoTrack,
	      (GLfloat)path_replay_trackNo_1 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_2_path_replay_trackNo_3_CAdecay_pulse_invertMovie ,
	      (GLfloat)path_replay_trackNo_2,
	      (GLfloat)path_replay_trackNo_3,
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
 glUniform4f( uniform_Update_fs_4fv_freeze_photo_value_photo_value_pulse_photo_scale ,
	      (GLfloat)freeze,
	      (GLfloat)photo_value,
	      (GLfloat)photo_value_pulse,
	      (GLfloat)photo_scale );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_echo_echoNeg_CAMixingWeight_PartMixingWeight ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)CAMixingWeight,
	      (GLfloat)PartMixingWeight );
 glUniform1f( uniform_Mixing_fs_1fv_trackMixingWeight_0 ,
	      (GLfloat)trackMixingWeight_0 );

 glUseProgram(shader_programme[pg_shader_ParticleSplat]);

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)CAMasterWeight );
 glUniform3f( uniform_Master_fs_3fv_PartMasterWeight_trackMasterWeight_0_hide ,
	      (GLfloat)PartMasterWeight,
	      (GLfloat)trackMasterWeight_0,
	      (GLfloat)hide );
