ParticleAnimation_scenario_var_data[0] = (GLfloat)noiseParticleScale * (1.f + pulse_average * noiseParticleScale_pulse);
ParticleAnimation_scenario_var_data[1] = (GLfloat)part_acc * (1.f + pulse_average * part_acc_pulse);
ParticleAnimation_scenario_var_data[2] = (GLfloat)part_damp * (1.f + pulse_average * part_damp_pulse);
ParticleAnimation_scenario_var_data[3] = (GLfloat)part_damp_targtRad;
ParticleAnimation_scenario_var_data[4] = (GLfloat)part_field_weight * (1.f + pulse_average * part_field_weight_pulse);
ParticleAnimation_scenario_var_data[5] = (GLfloat)part_gravity * (1.f + pulse_average * part_gravity_pulse);
ParticleAnimation_scenario_var_data[6] = (GLfloat)part_image_acceleration;
ParticleAnimation_scenario_var_data[7] = (GLfloat)part_initialization;
ParticleAnimation_scenario_var_data[8] = (GLfloat)part_size * (1.f + pulse_average * part_size_pulse);
ParticleAnimation_scenario_var_data[9] = (GLfloat)part_timeToTargt;
ParticleAnimation_scenario_var_data[10] = (GLfloat)partColor_mode;
ParticleAnimation_scenario_var_data[11] = (GLfloat)partExit_mode;
ParticleAnimation_scenario_var_data[12] = (GLfloat)partMove_rand;
ParticleAnimation_scenario_var_data[13] = (GLfloat)partMove_target;
ParticleAnimation_scenario_var_data[14] = (GLfloat)partStroke_mode;
ParticleAnimation_scenario_var_data[15] = (GLfloat)part_path_follow[0];
ParticleAnimation_scenario_var_data[16] = (GLfloat)part_path_follow[1];
ParticleAnimation_scenario_var_data[17] = (GLfloat)part_path_follow[2];
ParticleAnimation_scenario_var_data[18] = (GLfloat)part_path_follow[3];
ParticleAnimation_scenario_var_data[19] = (GLfloat)part_path_follow[4];
ParticleAnimation_scenario_var_data[20] = (GLfloat)part_path_follow[5];
ParticleAnimation_scenario_var_data[21] = (GLfloat)part_path_follow[6];
ParticleAnimation_scenario_var_data[22] = (GLfloat)part_path_follow[7];
ParticleAnimation_scenario_var_data[23] = (GLfloat)part_path_follow[8];
ParticleAnimation_scenario_var_data[24] = (GLfloat)part_path_follow[9];
ParticleAnimation_scenario_var_data[25] = (GLfloat)part_path_follow[10];
ParticleAnimation_scenario_var_data[26] = (GLfloat)part_path_follow[11];
ParticleAnimation_scenario_var_data[27] = (GLfloat)part_path_repulse[0];
ParticleAnimation_scenario_var_data[28] = (GLfloat)part_path_repulse[1];
ParticleAnimation_scenario_var_data[29] = (GLfloat)part_path_repulse[2];
ParticleAnimation_scenario_var_data[30] = (GLfloat)part_path_repulse[3];
ParticleAnimation_scenario_var_data[31] = (GLfloat)part_path_repulse[4];
ParticleAnimation_scenario_var_data[32] = (GLfloat)part_path_repulse[5];
ParticleAnimation_scenario_var_data[33] = (GLfloat)part_path_repulse[6];
ParticleAnimation_scenario_var_data[34] = (GLfloat)part_path_repulse[7];
ParticleAnimation_scenario_var_data[35] = (GLfloat)part_path_repulse[8];
ParticleAnimation_scenario_var_data[36] = (GLfloat)part_path_repulse[9];
ParticleAnimation_scenario_var_data[37] = (GLfloat)part_path_repulse[10];
ParticleAnimation_scenario_var_data[38] = (GLfloat)part_path_repulse[11];
ParticleAnimation_scenario_var_data[39] = (GLfloat)pixel_acc_shiftX * (1.f + pulse_average * pixel_acc_shiftX_pulse);
ParticleAnimation_scenario_var_data[40] = (GLfloat)pixel_acc_shiftY * (1.f + pulse_average * pixel_acc_shiftY_pulse);
ParticleAnimation_scenario_var_data[41] = (GLfloat)Part_repop_color_mode;
ParticleAnimation_scenario_var_data[42] = (GLfloat)Part_repop_density;
ParticleAnimation_scenario_var_data[43] = (GLfloat)repop_part * (1.f + pulse_average * repop_part_pulse);
ParticleAnimation_scenario_var_data[44] = (GLfloat)repop_path * (1.f + pulse_average * repop_path_pulse);
ParticleAnimation_scenario_var_data[45] = (GLfloat)freeze;
ParticleAnimation_scenario_var_data[46] = (GLfloat)partDecay * (1.f + pulse_average * partDecay_pulse);

Mixing_scenario_var_data[0] = (GLfloat)echo * (1.f + pulse_average * echo_pulse);
Mixing_scenario_var_data[1] = (GLfloat)echoNeg * (1.f + pulse_average * echoNeg_pulse);
Mixing_scenario_var_data[2] = (GLfloat)CAMixingWeight + pulse_average * CAMixingWeight_pulse;
Mixing_scenario_var_data[3] = (GLfloat)ClipArtMixingWeight + pulse_average * ClipArtMixingWeight_pulse;
Mixing_scenario_var_data[4] = (GLfloat)PartMixingWeight + pulse_average * PartMixingWeight_pulse;
Mixing_scenario_var_data[5] = (GLfloat)trackMixingWeight[0] + pulse_average * trackMixingWeight_pulse[0];
Mixing_scenario_var_data[6] = (GLfloat)trackMixingWeight[1] + pulse_average * trackMixingWeight_pulse[1];
Mixing_scenario_var_data[7] = (GLfloat)trackMixingWeight[2] + pulse_average * trackMixingWeight_pulse[2];
Mixing_scenario_var_data[8] = (GLfloat)trackMixingWeight[3] + pulse_average * trackMixingWeight_pulse[3];

Update_scenario_var_data[0] = (GLfloat)CAcolorSpread;
Update_scenario_var_data[1] = (GLfloat)CAParams[1] * (1.f + pulse_average * CAParams_pulse[1]);
Update_scenario_var_data[2] = (GLfloat)CAParams[2] * (1.f + pulse_average * CAParams_pulse[2]);
Update_scenario_var_data[3] = (GLfloat)CAParams[3] * (1.f + pulse_average * CAParams_pulse[3]);
Update_scenario_var_data[4] = (GLfloat)CAParams[4] * (1.f + pulse_average * CAParams_pulse[4]);
Update_scenario_var_data[5] = (GLfloat)CAParams[5] * (1.f + pulse_average * CAParams_pulse[5]);
Update_scenario_var_data[6] = (GLfloat)CAParams[6] * (1.f + pulse_average * CAParams_pulse[6]);
Update_scenario_var_data[7] = (GLfloat)CAParams[7] * (1.f + pulse_average * CAParams_pulse[7]);
Update_scenario_var_data[8] = (GLfloat)CAParams[8] * (1.f + pulse_average * CAParams_pulse[8]);
Update_scenario_var_data[9] = (GLfloat)CAstep;
Update_scenario_var_data[10] = (GLfloat)camera_gamma * (1.f + pulse_average * camera_gamma_pulse);
Update_scenario_var_data[11] = (GLfloat)cameraCumul;
Update_scenario_var_data[12] = (GLfloat)cameraGamma;
Update_scenario_var_data[13] = (GLfloat)cameraSobel * (1.f + pulse_average * cameraSobel_pulse);
Update_scenario_var_data[14] = (GLfloat)cameraThreshold * (1.f + pulse_average * cameraThreshold_pulse);
Update_scenario_var_data[15] = (GLfloat)cameraWeight * (1.f + pulse_average * cameraWeight_pulse);
Update_scenario_var_data[16] = (GLfloat)invertCamera;
Update_scenario_var_data[17] = (GLfloat)invertMovie;
Update_scenario_var_data[18] = (GLfloat)movie_gamma * (1.f + pulse_average * movie_gamma_pulse);
Update_scenario_var_data[19] = (GLfloat)movie_threshold * (1.f + pulse_average * movie_threshold_pulse);
Update_scenario_var_data[20] = (GLfloat)movieSobel * (1.f + pulse_average * movieSobel_pulse);
Update_scenario_var_data[21] = (GLfloat)movieWeight * (1.f + pulse_average * movieWeight_pulse);
Update_scenario_var_data[22] = (GLfloat)invertPhoto;
Update_scenario_var_data[23] = (GLfloat)photo_contrast;
Update_scenario_var_data[24] = (GLfloat)photo_gamma * (1.f + pulse_average * photo_gamma_pulse);
Update_scenario_var_data[25] = (GLfloat)photo_hue * (1.f + pulse_average * photo_hue_pulse);
Update_scenario_var_data[26] = (GLfloat)photo_rot;
Update_scenario_var_data[27] = (GLfloat)photo_satur * (1.f + pulse_average * photo_satur_pulse);
Update_scenario_var_data[28] = (GLfloat)photo_scaleX;
Update_scenario_var_data[29] = (GLfloat)photo_scaleY;
Update_scenario_var_data[30] = (GLfloat)photo_threshold * (1.f + pulse_average * photo_threshold_pulse);
Update_scenario_var_data[31] = (GLfloat)photo_transl_x;
Update_scenario_var_data[32] = (GLfloat)photo_transl_y;
Update_scenario_var_data[33] = (GLfloat)photo_value * (1.f + pulse_average * photo_value_pulse);
Update_scenario_var_data[34] = (GLfloat)photoSobel * (1.f + pulse_average * photoSobel_pulse);
Update_scenario_var_data[35] = (GLfloat)photoWeight * (1.f + pulse_average * photoWeight_pulse);
Update_scenario_var_data[36] = (GLfloat)video_contrast * (1.f + pulse_average * video_contrast_pulse);
Update_scenario_var_data[37] = (GLfloat)video_gamma * (1.f + pulse_average * video_gamma_pulse);
Update_scenario_var_data[38] = (GLfloat)video_hue * (1.f + pulse_average * video_hue_pulse);
Update_scenario_var_data[39] = (GLfloat)video_satur * (1.f + pulse_average * video_satur_pulse);
Update_scenario_var_data[40] = (GLfloat)video_threshold * (1.f + pulse_average * video_threshold_pulse);
Update_scenario_var_data[41] = (GLfloat)video_value * (1.f + pulse_average * video_value_pulse);
Update_scenario_var_data[42] = (GLfloat)video_white * (1.f + pulse_average * video_white_pulse);
Update_scenario_var_data[43] = (GLfloat)noiseType;
Update_scenario_var_data[44] = (GLfloat)noiseLineScale;
Update_scenario_var_data[45] = (GLfloat)noiseAngleScale;
Update_scenario_var_data[46] = (GLfloat)noiseCenterX;
Update_scenario_var_data[47] = (GLfloat)noiseCenterY;
Update_scenario_var_data[48] = (GLfloat)path_replay_trackNo[1];
Update_scenario_var_data[49] = (GLfloat)path_replay_trackNo[2];
Update_scenario_var_data[50] = (GLfloat)path_replay_trackNo[3];
Update_scenario_var_data[51] = (GLfloat)path_replay_trackNo[4];
Update_scenario_var_data[52] = (GLfloat)path_replay_trackNo[5];
Update_scenario_var_data[53] = (GLfloat)path_replay_trackNo[6];
Update_scenario_var_data[54] = (GLfloat)path_replay_trackNo[7];
Update_scenario_var_data[55] = (GLfloat)path_replay_trackNo[8];
Update_scenario_var_data[56] = (GLfloat)path_replay_trackNo[9];
Update_scenario_var_data[57] = (GLfloat)path_replay_trackNo[10];
Update_scenario_var_data[58] = (GLfloat)path_replay_trackNo[11];
Update_scenario_var_data[59] = (GLfloat)Pixelstep;
Update_scenario_var_data[60] = (GLfloat)noiseUpdateScale * (1.f + pulse_average * noiseUpdateScale_pulse);
Update_scenario_var_data[61] = (GLfloat)pixel_acc * (1.f + pulse_average * pixel_acc_pulse);
Update_scenario_var_data[62] = (GLfloat)pixel_acc_shiftX * (1.f + pulse_average * pixel_acc_shiftX_pulse);
Update_scenario_var_data[63] = (GLfloat)pixel_acc_shiftY * (1.f + pulse_average * pixel_acc_shiftY_pulse);
Update_scenario_var_data[64] = (GLfloat)pixel_image_acceleration;
Update_scenario_var_data[65] = (GLfloat)pixel_mode;
Update_scenario_var_data[66] = (GLfloat)pixel_radius * (1.f + pulse_average * pixel_radius_pulse);
Update_scenario_var_data[67] = (GLfloat)BG_CA_repop_color_mode;
Update_scenario_var_data[68] = (GLfloat)BG_CA_repop_density;
Update_scenario_var_data[69] = (GLfloat)repop_BG * (1.f + pulse_average * repop_BG_pulse);
Update_scenario_var_data[70] = (GLfloat)repop_CA * (1.f + pulse_average * repop_CA_pulse);
Update_scenario_var_data[71] = (GLfloat)camera_BG_subtr;
Update_scenario_var_data[72] = (GLfloat)freeze;
Update_scenario_var_data[73] = (GLfloat)CAdecay + pulse_average * CAdecay_pulse;
Update_scenario_var_data[74] = (GLfloat)trkDecay[0] * (1.f + pulse_average * trkDecay_pulse[0]);
Update_scenario_var_data[75] = (GLfloat)trkDecay[1] * (1.f + pulse_average * trkDecay_pulse[1]);
Update_scenario_var_data[76] = (GLfloat)trkDecay[2] * (1.f + pulse_average * trkDecay_pulse[2]);
Update_scenario_var_data[77] = (GLfloat)trkDecay[3] * (1.f + pulse_average * trkDecay_pulse[3]);
Update_scenario_var_data[78] = (GLfloat)currentDrawingTrack;
Update_scenario_var_data[79] = (GLfloat)currentPhotoTrack;
Update_scenario_var_data[80] = (GLfloat)currentVideoTrack;

Master_scenario_var_data[0] = (GLfloat)master_crop_width;
Master_scenario_var_data[1] = (GLfloat)master_crop_x;
Master_scenario_var_data[2] = (GLfloat)master_crop_y;
Master_scenario_var_data[3] = (GLfloat)master_mask;
Master_scenario_var_data[4] = (GLfloat)master_mask_offsetX;
Master_scenario_var_data[5] = (GLfloat)master_mask_offsetY;
Master_scenario_var_data[6] = (GLfloat)master_mask_opacity[1];
Master_scenario_var_data[7] = (GLfloat)master_mask_opacity[2];
Master_scenario_var_data[8] = (GLfloat)master_mask_opacity[3];
Master_scenario_var_data[9] = (GLfloat)master_mask_opacity[4];
Master_scenario_var_data[10] = (GLfloat)master_mask_opacity[5];
Master_scenario_var_data[11] = (GLfloat)master_mask_opacity[6];
Master_scenario_var_data[12] = (GLfloat)master_mask_scale;
Master_scenario_var_data[13] = (GLfloat)master_mask_scale_ratio;
Master_scenario_var_data[14] = (GLfloat)master_offsetX;
Master_scenario_var_data[15] = (GLfloat)master_offsetY;
Master_scenario_var_data[16] = (GLfloat)master_scale * (1.f + pulse_average * master_scale_pulse);
Master_scenario_var_data[17] = (GLfloat)master_scale_pulse;
Master_scenario_var_data[18] = (GLfloat)master_scale_ratio;
Master_scenario_var_data[19] = (GLfloat)cursorSize;
Master_scenario_var_data[20] = (GLfloat)invertAllLayers;
Master_scenario_var_data[21] = (GLfloat)master;
Master_scenario_var_data[22] = (GLfloat)mute_second_screen;
Master_scenario_var_data[23] = (GLfloat)CAMasterWeight + pulse_average * CAMasterWeight_pulse;
Master_scenario_var_data[24] = (GLfloat)ClipArtMasterWeight + pulse_average * ClipArtMasterWeight_pulse;
Master_scenario_var_data[25] = (GLfloat)PartMasterWeight + pulse_average * PartMasterWeight_pulse;
Master_scenario_var_data[26] = (GLfloat)SecondMasterMixingWeight;
Master_scenario_var_data[27] = (GLfloat)trackMasterWeight[0] + pulse_average * trackMasterWeight_pulse[0];
Master_scenario_var_data[28] = (GLfloat)trackMasterWeight[1] + pulse_average * trackMasterWeight_pulse[1];
Master_scenario_var_data[29] = (GLfloat)trackMasterWeight[2] + pulse_average * trackMasterWeight_pulse[2];
Master_scenario_var_data[30] = (GLfloat)trackMasterWeight[3] + pulse_average * trackMasterWeight_pulse[3];
Master_scenario_var_data[31] = (GLfloat)currentMaskTrack;

if(pg_ind_scenario == 0) {
    if (pg_shader_programme[0][pg_enum_shader_ParticleAnimation]) {
      glUseProgram(pg_shader_programme[0][pg_enum_shader_ParticleAnimation]);
      glUniform1fv(uniform_ParticleAnimation_scenario_var_data[0], 47, ParticleAnimation_scenario_var_data);
    }

    if (pg_shader_programme[0][pg_enum_shader_Update]) {
      glUseProgram(pg_shader_programme[0][pg_enum_shader_Update]);
      glUniform1fv(uniform_Update_scenario_var_data[0], 81, Update_scenario_var_data);
    }

    if (pg_shader_programme[0][pg_enum_shader_Mixing]) {
      glUseProgram(pg_shader_programme[0][pg_enum_shader_Mixing]);
      glUniform1fv(uniform_Mixing_scenario_var_data[0], 9, Mixing_scenario_var_data);
    }

    if (pg_shader_programme[0][pg_enum_shader_ParticleRender]) {
      glUseProgram(pg_shader_programme[0][pg_enum_shader_ParticleRender]);
    }

    if (pg_shader_programme[0][pg_enum_shader_Master]) {
      glUseProgram(pg_shader_programme[0][pg_enum_shader_Master]);
      glUniform1fv(uniform_Master_scenario_var_data[0], 32, Master_scenario_var_data);
    }
}