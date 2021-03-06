
 glUseProgram(shader_programme[pg_shader_ParticleAnimation]);
 glUniform4f( uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1 ,
	      (GLfloat)partDecay * (1.f + pulse_average * partDecay_pulse),
	      (GLfloat)part_initialization,
	      (GLfloat)part_path_follow_0,
	      (GLfloat)part_path_follow_1 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_follow_4_part_path_follow_5 ,
	      (GLfloat)part_path_follow_2,
	      (GLfloat)part_path_follow_3,
	      (GLfloat)part_path_follow_4,
	      (GLfloat)part_path_follow_5 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_follow_6_part_path_follow_7_part_path_repulse_0_part_path_repulse_1 ,
	      (GLfloat)part_path_follow_6,
	      (GLfloat)part_path_follow_7,
	      (GLfloat)part_path_repulse_0,
	      (GLfloat)part_path_repulse_1 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_path_repulse_4_part_path_repulse_5 ,
	      (GLfloat)part_path_repulse_2,
	      (GLfloat)part_path_repulse_3,
	      (GLfloat)part_path_repulse_4,
	      (GLfloat)part_path_repulse_5 );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_path_repulse_6_part_path_repulse_7_part_size_part_acc ,
	      (GLfloat)part_path_repulse_6,
	      (GLfloat)part_path_repulse_7,
	      (GLfloat)part_size * (1.f + pulse_average * part_size_pulse),
	      (GLfloat)part_acc * (1.f + pulse_average * part_acc_pulse) );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad ,
	      (GLfloat)part_damp * (1.f + pulse_average * part_damp_pulse),
	      (GLfloat)noiseScale * (1.f + pulse_average * noiseScale_pulse),
	      (GLfloat)part_field_weight * (1.f + pulse_average * part_field_weight_pulse),
	      (GLfloat)part_damp_targtRad );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode ,
	      (GLfloat)part_timeToTargt,
	      (GLfloat)partMove_target,
	      (GLfloat)partMove_rand,
	      (GLfloat)partExit_mode );
 glUniform4f( uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY ,
	      (GLfloat)partStroke_mode,
	      (GLfloat)partColor_mode,
	      (GLfloat)pixel_acc_shiftX * (pulse_average - pulse_average_prec) * pixel_acc_shiftX_pulse,
	      (GLfloat)pixel_acc_shiftY * (pulse_average - pulse_average_prec) * pixel_acc_shiftY_pulse );
 glUniform3f( uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze ,
	      (GLfloat)repop_part * (1.f + pulse_average * repop_part_pulse),
	      (GLfloat)repop_path * (1.f + pulse_average * repop_path_pulse),
	      (GLfloat)freeze );

 glUseProgram(shader_programme[pg_shader_Update]);
 glUniform4f( uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1 ,
	      (GLfloat)camera_BG_subtr,
	      (GLfloat)CAdecay * (1.f + pulse_average * CAdecay_pulse),
	      (GLfloat)trkDecay_0 * (1.f + pulse_average * trkDecay_0_pulse),
	      (GLfloat)trkDecay_1 * (1.f + pulse_average * trkDecay_1_pulse) );
 glUniform4f( uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack ,
	      (GLfloat)trkDecay_2 * (1.f + pulse_average * trkDecay_2_pulse),
	      (GLfloat)trkDecay_3 * (1.f + pulse_average * trkDecay_3_pulse),
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
 glUniform4f( uniform_Update_fs_4fv_noiseScale_noiseLineScale_pixel_acc_pixel_acc_shiftX ,
	      (GLfloat)noiseScale * (1.f + pulse_average * noiseScale_pulse),
	      (GLfloat)noiseLineScale,
	      (GLfloat)pixel_acc * (1.f + pulse_average * pixel_acc_pulse),
	      (GLfloat)pixel_acc_shiftX * (pulse_average - pulse_average_prec) * pixel_acc_shiftX_pulse );
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_shiftY_pixel_radius_pixel_mode_repop_CA ,
	      (GLfloat)pixel_acc_shiftY * (pulse_average - pulse_average_prec) * pixel_acc_shiftY_pulse,
	      (GLfloat)pixel_radius * (1.f + pulse_average * pixel_radius_pulse),
	      (GLfloat)pixel_mode,
	      (GLfloat)repop_CA * (1.f + pulse_average * repop_CA_pulse) );
 glUniform4f( uniform_Update_fs_4fv_repop_BG_cameraGamma_cameraThreshold_cameraWeight ,
	      (GLfloat)repop_BG * (1.f + pulse_average * repop_BG_pulse),
	      (GLfloat)cameraGamma,
	      (GLfloat)cameraThreshold * (1.f + pulse_average * cameraThreshold_pulse),
	      (GLfloat)cameraWeight * (1.f + pulse_average * cameraWeight_pulse) );
 glUniform4f( uniform_Update_fs_4fv_cameraSobel_movieWeight_movieSobel_invertMovie ,
	      (GLfloat)cameraSobel * (1.f + pulse_average * cameraSobel_pulse),
	      (GLfloat)movieWeight * (1.f + pulse_average * movieWeight_pulse),
	      (GLfloat)movieSobel * (1.f + pulse_average * movieSobel_pulse),
	      (GLfloat)invertMovie );
 glUniform4f( uniform_Update_fs_4fv_video_hue_video_satur_video_satur_pulse_video_value ,
	      (GLfloat)video_hue * (1.f + pulse_average * video_hue_pulse),
	      (GLfloat)video_satur * (1.f + pulse_average * video_satur_pulse),
	      (GLfloat)video_satur_pulse,
	      (GLfloat)video_value * (1.f + pulse_average * video_value_pulse) );
 glUniform4f( uniform_Update_fs_4fv_photoWeight_photo_hue_photo_satur_photo_satur_pulse ,
	      (GLfloat)photoWeight * (1.f + pulse_average * photoWeight_pulse),
	      (GLfloat)photo_hue * (1.f + pulse_average * photo_hue_pulse),
	      (GLfloat)photo_satur * (1.f + pulse_average * photo_satur_pulse),
	      (GLfloat)photo_satur_pulse );
 glUniform4f( uniform_Update_fs_4fv_photo_value_photo_value_pulse_photo_scale_mask_scale ,
	      (GLfloat)photo_value * (1.f + pulse_average * photo_value_pulse),
	      (GLfloat)photo_value_pulse,
	      (GLfloat)photo_scale,
	      (GLfloat)mask_scale );
 glUniform4f( uniform_Update_fs_4fv_photo_contrast_mask_contrast_cameraCumul_CAstep ,
	      (GLfloat)photo_contrast,
	      (GLfloat)mask_contrast,
	      (GLfloat)cameraCumul,
	      (GLfloat)CAstep );
 glUniform2f( uniform_Update_fs_2fv_CAcolorSpread_freeze ,
	      (GLfloat)CAcolorSpread,
	      (GLfloat)freeze );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1 ,
	      (GLfloat)CAMixingWeight * (1.f + pulse_average * CAMixingWeight_pulse),
	      (GLfloat)PartMixingWeight * (1.f + pulse_average * PartMixingWeight_pulse),
	      (GLfloat)trackMixingWeight_0 * (1.f + pulse_average * trackMixingWeight_0_pulse),
	      (GLfloat)trackMixingWeight_1 * (1.f + pulse_average * trackMixingWeight_1_pulse) );
 glUniform4f( uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg ,
	      (GLfloat)trackMixingWeight_2 * (1.f + pulse_average * trackMixingWeight_2_pulse),
	      (GLfloat)trackMixingWeight_3 * (1.f + pulse_average * trackMixingWeight_3_pulse),
	      (GLfloat)echo * (1.f + pulse_average * echo_pulse),
	      (GLfloat)echoNeg * (1.f + pulse_average * echoNeg_pulse) );

 glUseProgram(shader_programme[pg_shader_ParticleRender]);

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master ,
	      (GLfloat)mute_second_screen,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)master );
 glUniform4f( uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1 ,
	      (GLfloat)CAMasterWeight * (1.f + pulse_average * CAMasterWeight_pulse),
	      (GLfloat)PartMasterWeight * (1.f + pulse_average * PartMasterWeight_pulse),
	      (GLfloat)trackMasterWeight_0 * (1.f + pulse_average * trackMasterWeight_0_pulse),
	      (GLfloat)trackMasterWeight_1 * (1.f + pulse_average * trackMasterWeight_1_pulse) );
 glUniform3f( uniform_Master_fs_3fv_trackMasterWeight_2_trackMasterWeight_3_interfaceOnScreen ,
	      (GLfloat)trackMasterWeight_2 * (1.f + pulse_average * trackMasterWeight_2_pulse),
	      (GLfloat)trackMasterWeight_3 * (1.f + pulse_average * trackMasterWeight_3_pulse),
	      (GLfloat)interfaceOnScreen );
