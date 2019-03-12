  uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master 
    = glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master");
  if ( uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master == -1 ) {
    fprintf(stderr, "Could not bind uniform Master uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master : %d\n" , uniform_Master_fs_4fv_mute_second_screen_invertAllLayers_cursorSize_master );
  }
  uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1 
    = glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1");
  if ( uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1 == -1 ) {
    fprintf(stderr, "Could not bind uniform Mixing uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1 : %d\n" , uniform_Mixing_fs_4fv_CAMixingWeight_PartMixingWeight_trackMixingWeight_0_trackMixingWeight_1 );
  }
  uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1 
    = glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1");
  if ( uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1 == -1 ) {
    fprintf(stderr, "Could not bind uniform Master uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1 : %d\n" , uniform_Master_fs_4fv_CAMasterWeight_PartMasterWeight_trackMasterWeight_0_trackMasterWeight_1 );
  }
  uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg 
    = glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg");
  if ( uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg == -1 ) {
    fprintf(stderr, "Could not bind uniform Mixing uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg : %d\n" , uniform_Mixing_fs_4fv_trackMixingWeight_2_trackMixingWeight_3_echo_echoNeg );
  }
  uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1");
  if ( uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1 == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1 : %d\n" , uniform_Update_fs_4fv_camera_BG_subtr_CAdecay_trkDecay_0_trkDecay_1 );
  }
  uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack");
  if ( uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack : %d\n" , uniform_Update_fs_4fv_trkDecay_2_trkDecay_3_currentDrawingTrack_currentVideoTrack );
  }
  uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1");
  if ( uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1 == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1 : %d\n" , uniform_ParticleAnimation_fs_4fv_partDecay_part_initialization_part_path_follow_0_part_path_follow_1 );
  }
  uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_repulse_0_part_path_repulse_1 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_repulse_0_part_path_repulse_1");
  if ( uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_repulse_0_part_path_repulse_1 == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_repulse_0_part_path_repulse_1 : %d\n" , uniform_ParticleAnimation_fs_4fv_part_path_follow_2_part_path_follow_3_part_path_repulse_0_part_path_repulse_1 );
  }
  uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_size_part_acc 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_size_part_acc");
  if ( uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_size_part_acc == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_size_part_acc : %d\n" , uniform_ParticleAnimation_fs_4fv_part_path_repulse_2_part_path_repulse_3_part_size_part_acc );
  }
  uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale");
  if ( uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale : %d\n" , uniform_Update_fs_4fv_currentPhotoTrack_noiseScale_noiseType_noiseLineScale );
  }
  uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad");
  if ( uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad : %d\n" , uniform_ParticleAnimation_fs_4fv_part_damp_noiseScale_part_field_weight_part_damp_targtRad );
  }
  uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode");
  if ( uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode : %d\n" , uniform_ParticleAnimation_fs_4fv_part_timeToTargt_partMove_target_partMove_rand_partExit_mode );
  }
  uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc");
  if ( uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc : %d\n" , uniform_Update_fs_4fv_noiseAngleScale_noiseCenterX_noiseCenterY_pixel_acc );
  }
  uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY");
  if ( uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY : %d\n" , uniform_ParticleAnimation_fs_4fv_partStroke_mode_partColor_mode_pixel_acc_shiftX_pixel_acc_shiftY );
  }
  uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode");
  if ( uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode : %d\n" , uniform_Update_fs_4fv_pixel_acc_shiftX_pixel_acc_shiftY_pixel_radius_pixel_mode );
  }
  uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold");
  if ( uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold : %d\n" , uniform_Update_fs_4fv_repop_CA_repop_BG_cameraGamma_cameraThreshold );
  }
  uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel");
  if ( uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel : %d\n" , uniform_Update_fs_4fv_cameraWeight_cameraSobel_movieWeight_movieSobel );
  }
  uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse");
  if ( uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse : %d\n" , uniform_Update_fs_4fv_invertMovie_video_hue_video_satur_video_satur_pulse );
  }
  uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur");
  if ( uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur : %d\n" , uniform_Update_fs_4fv_video_value_photoWeight_photo_hue_photo_satur );
  }
  uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale");
  if ( uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale : %d\n" , uniform_Update_fs_4fv_photo_satur_pulse_photo_value_photo_value_pulse_photo_scale );
  }
  uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1");
  if ( uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1 == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1 : %d\n" , uniform_Update_fs_4fv_mask_scale_photo_contrast_mask_contrast_CAParams1 );
  }
  uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5");
  if ( uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5 == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5 : %d\n" , uniform_Update_fs_4fv_CAParams2_CAParams3_CAParams4_CAParams5 );
  }
  uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul");
  if ( uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul : %d\n" , uniform_Update_fs_4fv_CAParams6_CAParams7_CAParams8_cameraCumul );
  }
  uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze 
    = glGetUniformLocation(shader_programme[pg_shader_ParticleAnimation], "uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze");
  if ( uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze == -1 ) {
    fprintf(stderr, "Could not bind uniform ParticleAnimation uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze : %d\n" , uniform_ParticleAnimation_fs_3fv_repop_part_repop_path_freeze );
  }
  uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze 
    = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze");
  if ( uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze : %d\n" , uniform_Update_fs_3fv_CAstep_CAcolorSpread_freeze );
  }
  uniform_Master_fs_2fv_trackMasterWeight_2_interfaceOnScreen 
    = glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_fs_2fv_trackMasterWeight_2_interfaceOnScreen");
  if ( uniform_Master_fs_2fv_trackMasterWeight_2_interfaceOnScreen == -1 ) {
    fprintf(stderr, "Could not bind uniform Master uniform_Master_fs_2fv_trackMasterWeight_2_interfaceOnScreen : %d\n" , uniform_Master_fs_2fv_trackMasterWeight_2_interfaceOnScreen );
  }
