
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
 glUniform4f( uniform_Update_fs_4fv_noiseScale_noiseType_noiseLineScale_noiseAngleScale ,
	      (GLfloat)noiseScale * (1.f + pulse_average * noiseScale_pulse),
	      (GLfloat)noiseType,
	      (GLfloat)noiseLineScale,
	      (GLfloat)noiseAngleScale );
 glUniform4f( uniform_Update_fs_4fv_noiseCenterX_noiseCenterY_pixel_acc_pixel_acc_shiftX ,
	      (GLfloat)noiseCenterX,
	      (GLfloat)noiseCenterY,
	      (GLfloat)pixel_acc * (1.f + pulse_average * pixel_acc_pulse),
	      (GLfloat)pixel_acc_shiftX * (pulse_average - pulse_average_prec) * pixel_acc_shiftX_pulse );
 glUniform4f( uniform_Update_fs_4fv_pixel_acc_shiftY_pixel_radius_pixel_mode_repop_CA ,
	      (GLfloat)pixel_acc_shiftY * (pulse_average - pulse_average_prec) * pixel_acc_shiftY_pulse,
	      (GLfloat)pixel_radius * (1.f + pulse_average * pixel_radius_pulse),
	      (GLfloat)pixel_mode,
	      (GLfloat)repop_CA * (1.f + pulse_average * repop_CA_pulse) );
 glUniform4f( uniform_Update_fs_4fv_repop_BG_cameraGamma_activeClipArts_cameraThreshold ,
	      (GLfloat)repop_BG * (1.f + pulse_average * repop_BG_pulse),
	      (GLfloat)cameraGamma,
	      (GLfloat)activeClipArts,
	      (GLfloat)cameraThreshold * (1.f + pulse_average * cameraThreshold_pulse) );
 glUniform4f( uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel ,
	      (GLfloat)cameraWeight * (1.f + pulse_average * cameraWeight_pulse),
	      (GLfloat)cameraSobel * (1.f + pulse_average * cameraSobel_pulse),
	      (GLfloat)movieWeight * (1.f + pulse_average * movieWeight_pulse),
	      (GLfloat)movieSobel * (1.f + pulse_average * movieSobel_pulse) );
 glUniform4f( uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse ,
	      (GLfloat)invertMovie,
	      (GLfloat)video_hue * (1.f + pulse_average * video_hue_pulse),
	      (GLfloat)video_satur * (1.f + pulse_average * video_satur_pulse),
	      (GLfloat)video_satur_pulse );
 glUniform4f( uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_value ,
	      (GLfloat)video_value * (1.f + pulse_average * video_value_pulse),
	      (GLfloat)photoWeight * (1.f + pulse_average * photoWeight_pulse),
	      (GLfloat)photo_hue * (1.f + pulse_average * photo_hue_pulse),
	      (GLfloat)photo_value * (1.f + pulse_average * photo_value_pulse) );
 glUniform4f( uniform_Update_fs_4fv_photo_value_pulse_photo_scale_CAParams1_CAParams2 ,
	      (GLfloat)photo_value_pulse,
	      (GLfloat)photo_scale,
	      (GLfloat)CAParams1 * (1.f + pulse_average * CAParams1_pulse),
	      (GLfloat)CAParams2 * (1.f + pulse_average * CAParams2_pulse) );
 glUniform4f( uniform_Update_fs_4fv_CAParams3_cameraCumul_CAstep_CAcolorSpread ,
	      (GLfloat)CAParams3 * (1.f + pulse_average * CAParams3_pulse),
	      (GLfloat)cameraCumul,
	      (GLfloat)CAstep,
	      (GLfloat)CAcolorSpread );
 glUniform1f( uniform_Update_fs_1fv_freeze ,
	      (GLfloat)freeze );

 glUseProgram(shader_programme[pg_shader_Mixing]);
 glUniform4f( uniform_Mixing_fs_4fv_CAMixingWeight_trackMixingWeight_0_echo_echoNeg ,
	      (GLfloat)CAMixingWeight * (1.f + pulse_average * CAMixingWeight_pulse),
	      (GLfloat)trackMixingWeight_0 * (1.f + pulse_average * trackMixingWeight_0_pulse),
	      (GLfloat)echo * (1.f + pulse_average * echo_pulse),
	      (GLfloat)echoNeg * (1.f + pulse_average * echoNeg_pulse) );

 glUseProgram(shader_programme[pg_shader_Master]);
 glUniform4f( uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master ,
	      (GLfloat)mute_second_screen,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)master );
 glUniform3f( uniform_Master_fs_3fv_CAMasterWeight_trackMasterWeight_0_interfaceOnScreen ,
	      (GLfloat)CAMasterWeight * (1.f + pulse_average * CAMasterWeight_pulse),
	      (GLfloat)trackMasterWeight_0 * (1.f + pulse_average * trackMasterWeight_0_pulse),
	      (GLfloat)interfaceOnScreen );
