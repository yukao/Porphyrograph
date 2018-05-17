
 glUseProgram(shader_programme[pg_shader_Update]);
 glUniform4f( uniform_Update_fs_4fv_pixel_mode_pixel_acc_factor_noiseScale_pixel_acc_center_0 ,
	      (GLfloat)pixel_mode,
	      (GLfloat)pixel_acc_factor,
	      (GLfloat)noiseScale,
	      (GLfloat)pixel_acc_center_0 );
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_center_1_repop_BG_repop_CA_CAType ,
	      (GLfloat)pixel_acc_center_1,
	      (GLfloat)repop_BG,
	      (GLfloat)repop_CA,
	      (GLfloat)CAType );
 glUniform4f( uniform_Update_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_currentPhotoTrack ,
	      (GLfloat)CASubType,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack,
	      (GLfloat)currentPhotoTrack );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_1_path_replay_trackNo_2_path_replay_trackNo_3_path_replay_trackNo_4 ,
	      (GLfloat)path_replay_trackNo_1,
	      (GLfloat)path_replay_trackNo_2,
	      (GLfloat)path_replay_trackNo_3,
	      (GLfloat)path_replay_trackNo_4 );
 glUniform4f( uniform_Update_fs_4fv_path_replay_trackNo_5_path_replay_trackNo_6_path_replay_trackNo_7_part_path_follow_0 ,
	      (GLfloat)path_replay_trackNo_5,
	      (GLfloat)path_replay_trackNo_6,
	      (GLfloat)path_replay_trackNo_7,
	      (GLfloat)part_path_follow_0 );
 glUniform4f( uniform_Update_fs_4fv_part_path_follow_1_part_path_follow_2_part_path_follow_3_part_path_follow_4 ,
	      (GLfloat)part_path_follow_1,
	      (GLfloat)part_path_follow_2,
	      (GLfloat)part_path_follow_3,
	      (GLfloat)part_path_follow_4 );
 glUniform4f( uniform_Update_fs_4fv_part_path_follow_5_part_path_follow_6_part_path_follow_7_part_path_repulse_0 ,
	      (GLfloat)part_path_follow_5,
	      (GLfloat)part_path_follow_6,
	      (GLfloat)part_path_follow_7,
	      (GLfloat)part_path_repulse_0 );
 glUniform4f( uniform_Update_fs_4fv_part_path_repulse_1_part_path_repulse_2_part_path_repulse_3_part_path_repulse_4 ,
	      (GLfloat)part_path_repulse_1,
	      (GLfloat)part_path_repulse_2,
	      (GLfloat)part_path_repulse_3,
	      (GLfloat)part_path_repulse_4 );
 glUniform4f( uniform_Update_fs_4fv_part_path_repulse_5_part_path_repulse_6_part_path_repulse_7_CAdecay_pulse ,
	      (GLfloat)part_path_repulse_5,
	      (GLfloat)part_path_repulse_6,
	      (GLfloat)part_path_repulse_7,
	      (GLfloat)CAdecay_pulse );
 glUniform4f( uniform_Update_fs_4fv_invertMovie_cameraCumul_cameraThreshold_cameraGamma ,
	      (GLfloat)invertMovie,
	      (GLfloat)cameraCumul,
	      (GLfloat)cameraThreshold,
	      (GLfloat)cameraGamma );
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
 glUniform4f( uniform_Update_fs_4fv_part_initialization_partMove_target_partMove_rand_partExit_mode ,
	      (GLfloat)part_initialization,
	      (GLfloat)partMove_target,
	      (GLfloat)partMove_rand,
	      (GLfloat)partExit_mode );
 glUniform4f( uniform_Update_fs_4fv_partStroke_mode_partColor_mode_part_damp_targtRad_part_timeToTargt ,
	      (GLfloat)partStroke_mode,
	      (GLfloat)partColor_mode,
	      (GLfloat)part_damp_targtRad,
	      (GLfloat)part_timeToTargt );
 glUniform3f( uniform_Update_fs_3fv_part_field_weight_partRepopRadius_particle_type ,
	      (GLfloat)part_field_weight,
	      (GLfloat)partRepopRadius,
	      (GLfloat)particle_type );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_echo_echoNeg_CAMixingWeight_PartMixingWeight ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)CAMixingWeight,
	      (GLfloat)PartMixingWeight );
 glUniform3f( uniform_Mixing_fs_3fv_trackMixingWeight_0_trackMixingWeight_1_trackMixingWeight_2 ,
	      (GLfloat)trackMixingWeight_0,
	      (GLfloat)trackMixingWeight_1,
	      (GLfloat)trackMixingWeight_2 );

 glUseProgram(shader_programme[pg_shader_Particle]);

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAMasterWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)CAMasterWeight );
 glUniform4f( uniform_Master_fs_4fv_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1_trackMasterWeight_2 ,
	      (GLfloat)PartMasterWeight,
	      (GLfloat)trackMasterWeight_0,
	      (GLfloat)trackMasterWeight_1,
	      (GLfloat)trackMasterWeight_2 );
 glUniform1f( uniform_Master_fs_1fv_hide ,
	      (GLfloat)hide );
