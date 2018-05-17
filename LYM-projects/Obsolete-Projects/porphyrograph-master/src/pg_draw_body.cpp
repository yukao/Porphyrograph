
 glUseProgram(shader_Drawing_programme);
 glUniform4f( uniform_Drawing_fs_4fv_particleMode_particle_texture_ID_partAccCenter_0_partAccCenter_1 ,
	      (GLfloat)particleMode,
	      (GLfloat)particle_texture_ID,
	      (GLfloat)partAccCenter_0,
	      (GLfloat)partAccCenter_1 );
 glUniform4f( uniform_Drawing_fs_4fv_radiusParticle_repopulatingBG_repopulatingCA_CAType ,
	      (GLfloat)radiusParticle,
	      (GLfloat)repopulatingBG,
	      (GLfloat)repopulatingCA,
	      (GLfloat)CAType );
 glUniform4f( uniform_Drawing_fs_4fv_CASubType_currentDrawingTrack_currentVideoTrack_track_replay_0 ,
	      (GLfloat)CASubType,
	      (GLfloat)currentDrawingTrack,
	      (GLfloat)currentVideoTrack,
	      (GLfloat)track_replay_0 );
 glUniform4f( uniform_Drawing_fs_4fv_track_replay_1_track_replay_2_CAParams1_CAParams2 ,
	      (GLfloat)track_replay_1,
	      (GLfloat)track_replay_2,
	      (GLfloat)CAParams1,
	      (GLfloat)CAParams2 );
 glUniform4f( uniform_Drawing_fs_4fv_CAParams3_CAParams4_CAParams5_CAParams6 ,
	      (GLfloat)CAParams3,
	      (GLfloat)CAParams4,
	      (GLfloat)CAParams5,
	      (GLfloat)CAParams6 );
 glUniform4f( uniform_Drawing_fs_4fv_CAParams7_CAParams8_is_videoOn_BGSubtr ,
	      (GLfloat)CAParams7,
	      (GLfloat)CAParams8,
	      (GLfloat)is_videoOn,
	      (GLfloat)BGSubtr );
 glUniform2f( uniform_Drawing_fs_2fv_CAstep_CAcolorSpread ,
	      (GLfloat)CAstep,
	      (GLfloat)CAcolorSpread );

 glUseProgram(shader_Composition_programme);
 glUniform4f( uniform_Composition_fs_4fv_echo_echoNeg_CACompositionWeight_trackCompositionWeight_0 ,
	      (GLfloat)echo,
	      (GLfloat)echoNeg,
	      (GLfloat)CACompositionWeight,
	      (GLfloat)trackCompositionWeight_0 );
 glUniform3f( uniform_Composition_fs_3fv_trackCompositionWeight_1_trackCompositionWeight_2_FBCaptureWeight ,
	      (GLfloat)trackCompositionWeight_1,
	      (GLfloat)trackCompositionWeight_2,
	      (GLfloat)FBCaptureWeight );

 glUseProgram(shader_Final_programme);
 glUniform4f( uniform_Final_fs_4fv_blendTransp_invertAllLayers_cursorSize_CAFinalWeight ,
	      (GLfloat)blendTransp,
	      (GLfloat)invertAllLayers,
	      (GLfloat)cursorSize,
	      (GLfloat)CAFinalWeight );
 glUniform3f( uniform_Final_fs_3fv_trackFinalWeight_0_trackFinalWeight_1_trackFinalWeight_2 ,
	      (GLfloat)trackFinalWeight_0,
	      (GLfloat)trackFinalWeight_1,
	      (GLfloat)trackFinalWeight_2 );
