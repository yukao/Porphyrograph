
 glUseProgram(shader_Drawing_programme);
 glUniform4f( uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1 ,
	      (GLfloat)particleMode,
	      (GLfloat)particle_texture_ID,
	      (GLfloat)partAccCenter_0,
	      (GLfloat)partAccCenter_1 );
 glUniform4f( uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_currentDrawingTrack_currentVideoTrack ,
	      (GLfloat)radiusParticle,
	      (GLfloat)repopulatingBG,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack );
 glUniform4f( uniform_Drawing_fs_4fv_track_replay_0_track_replay_1_is_videoOn_is_videoInvert ,
	      (GLfloat)track_replay_0,
	      (GLfloat)track_replay_1,
	      (GLfloat)is_videoOn,
	      (GLfloat)is_videoInvert );
 glUniform4f( uniform_Drawing_fs_4fv_videoThreshold_videoGamma_BGSubtr_images_weights_scale ,
	      (GLfloat)videoThreshold,
	      (GLfloat)videoGamma,
	      (GLfloat)BGSubtr,
	      (GLfloat)images_weights_scale );

 glUseProgram(shader_Composition_programme);
 glUniform4f( uniform_Composition_fs_4fv_echo_echoNeg_trackCompositionWeight_0_trackCompositionWeight_1 ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)trackCompositionWeight_0,
	      (GLfloat)trackCompositionWeight_1 );

 glUseProgram(shader_Final_programme);
 glUniform4f( uniform_Final_fs_4fv_blendTransp_invertAllLayers_cursorSize_trackFinalWeight_0 ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)trackFinalWeight_0 );
 glUniform1f( uniform_Final_fs_1fv_trackFinalWeight_1 ,
	      (GLfloat)trackFinalWeight_1 );
