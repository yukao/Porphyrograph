
 glUseProgram(shader_Drawing_programme);
 glUniform4f( uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1 ,
	      (GLfloat)particleMode,
	      (GLfloat)particle_texture_ID,
	      (GLfloat)partAccCenter_0,
	      (GLfloat)partAccCenter_1 );
 glUniform4f( uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_trackdecay_sign_0_trackdecay_0 ,
	      (GLfloat)radiusParticle,
	      (GLfloat)repopulatingBG,
	      (GLfloat)trackdecay_sign_0,
	      (GLfloat)trackdecay_0 );
 glUniform4f( uniform_Drawing_fs_4fv_is_videoOn_videoThreshold_videoGamma_BGSubtr ,
	      (GLfloat)is_videoOn,
	      (GLfloat)videoThreshold,
	      (GLfloat)videoGamma,
	      (GLfloat)BGSubtr );
 glUniform4f( uniform_Drawing_fs_4fv_CAType_CASubType_images_weights_scale_image_value ,
	      (GLfloat)CAType,
	      (GLfloat)CASubType,
	      (GLfloat)images_weights_scale,
	      (GLfloat)image_value );
 glUniform4f( uniform_Drawing_fs_4fv_image_value_pulse_image_satur_image_satur_pulse_mask_contrast ,
	      (GLfloat)image_value_pulse,
	      (GLfloat)image_satur,
	      (GLfloat)image_satur_pulse,
	      (GLfloat)mask_contrast );
 glUniform3f( uniform_Drawing_fs_3fv_mask_threshold_image_contrast_freeze ,
	      (GLfloat)mask_threshold,
	      (GLfloat)image_contrast,
	      (GLfloat)freeze );

 glUseProgram(shader_Composition_programme);
 glUniform4f( uniform_Composition_fs_4fv_echo_echoNeg_trackCompositionWeight_0_trackCompositionWeight_1 ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)trackCompositionWeight_0,
	      (GLfloat)trackCompositionWeight_1 );
 glUniform1f( uniform_Composition_fs_1fv_CACompositionWeight ,
	      (GLfloat)CACompositionWeight );

 glUseProgram(shader_Final_programme);
 glUniform4f( uniform_Final_fs_4fv_blendTransp_trackFinalWeight_0_trackFinalWeight_1_CAFinalWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)trackFinalWeight_0,
	      (GLfloat)trackFinalWeight_1,
	      (GLfloat)CAFinalWeight );
