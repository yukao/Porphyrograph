bool  double_window       ;
bool  wide_screen         ;
int   window_width        ;
int   window_height       ;
int   window_x            ;
int   window_y            ;
float minimal_interframe_latency;
float time_scale          ;
float initial_time        ;
int   camID               ;
int   message_pixel_length;
bool  trace_output_frame_number;
bool  trace_time          ;
int   first_frame_number  ;
int   last_frame_number   ;
int   max_mouse_recording_frames;
int   max_network_message_length;
VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs] = { 
    _pg_bool,
    _pg_bool,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_int,
    _pg_bool,
    _pg_bool,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_int,
};
void * ConfigurationVarPointers[_MaxConfigurationVarIDs] = { 
   (void *)&double_window,
   (void *)&wide_screen,
   (void *)&window_width,
   (void *)&window_height,
   (void *)&window_x,
   (void *)&window_y,
   (void *)&minimal_interframe_latency,
   (void *)&time_scale,
   (void *)&initial_time,
   (void *)&camID,
   (void *)&message_pixel_length,
   (void *)&trace_output_frame_number,
   (void *)&trace_time,
   (void *)&first_frame_number,
   (void *)&last_frame_number,
   (void *)&max_mouse_recording_frames,
   (void *)&max_network_message_length,
};
float brushRadius         ;
float RGBpalette          ;
float local_color_gray    ;
float blendTransp         ;
int   particleMode        ;
float part_acc_factor     ;
float particle_texture_ID ;
float partAccCenter_0     ;
float partAccCenter_1     ;
float radiusParticle      ;
float repopulatingBG      ;
bool  clearAllLayers      ;
float echo                ;
float echoNeg             ;
float RGBpalette_val_coef ;
float RGBpalette_val_const;
int   flashTrack0_freq    ;
int   flashPart_freq      ;
int   flashPart_duration  ;
float brush_radius_pressure_coef;
float brush_radius_angleHor_coef;
float brush_radius_angleVer_coef;
float trackdecay_sign_0   ;
float trackdecay_0        ;
float trackCompositionWeight_0;
float trackCompositionWeight_1;
float trackFinalWeight_0  ;
float trackFinalWeight_1  ;
bool  auto_beat           ;
bool  is_videoOn          ;
float videoThreshold      ;
float videoGamma          ;
float videoExposure       ;
float flashVideoLength    ;
float flashVideoBright    ;
int   flashVideoBeat      ;
int   BGSubtr             ;
int   CAType              ;
int   CASubType           ;
bool  clearCA             ;
int   flashCA_freq_0      ;
int   flashCA_freq_1      ;
int   flashBack_freq      ;
float CACompositionWeight ;
float CAFinalWeight       ;
float CAdecay_sign        ;
float CAdecay             ;
float images_weights_rand ;
float images_weights_scale;
float maskJitterAmpl      ;
float imageJitterAmpl     ;
float image_value         ;
float image_value_pulse   ;
float image_satur         ;
float image_satur_pulse   ;
float mask_contrast       ;
float mask_threshold      ;
float mask_variance       ;
float image_contrast      ;
bool  freeze              ;
float input_volume        ;
int   subscene_duration   ;
float image_swap_duration ;
int   image_swap_freq     ;
VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { 
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_bool,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_sign,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_bool,
    _pg_bool,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_bool,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_sign,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_bool,
    _pg_float,
    _pg_int,
    _pg_float,
    _pg_int,
};
void * ScenarioVarPointers[_MaxInterpVarIDs] = { 
   (void *)&brushRadius,
   (void *)&RGBpalette,
   (void *)&local_color_gray,
   (void *)&blendTransp,
   (void *)&particleMode,
   (void *)&part_acc_factor,
   (void *)&particle_texture_ID,
   (void *)&partAccCenter_0,
   (void *)&partAccCenter_1,
   (void *)&radiusParticle,
   (void *)&repopulatingBG,
   (void *)&clearAllLayers,
   (void *)&echo,
   (void *)&echoNeg,
   (void *)&RGBpalette_val_coef,
   (void *)&RGBpalette_val_const,
   (void *)&flashTrack0_freq,
   (void *)&flashPart_freq,
   (void *)&flashPart_duration,
   (void *)&brush_radius_pressure_coef,
   (void *)&brush_radius_angleHor_coef,
   (void *)&brush_radius_angleVer_coef,
   (void *)&trackdecay_sign_0,
   (void *)&trackdecay_0,
   (void *)&trackCompositionWeight_0,
   (void *)&trackCompositionWeight_1,
   (void *)&trackFinalWeight_0,
   (void *)&trackFinalWeight_1,
   (void *)&auto_beat,
   (void *)&is_videoOn,
   (void *)&videoThreshold,
   (void *)&videoGamma,
   (void *)&videoExposure,
   (void *)&flashVideoLength,
   (void *)&flashVideoBright,
   (void *)&flashVideoBeat,
   (void *)&BGSubtr,
   (void *)&CAType,
   (void *)&CASubType,
   (void *)&clearCA,
   (void *)&flashCA_freq_0,
   (void *)&flashCA_freq_1,
   (void *)&flashBack_freq,
   (void *)&CACompositionWeight,
   (void *)&CAFinalWeight,
   (void *)&CAdecay_sign,
   (void *)&CAdecay,
   (void *)&images_weights_rand,
   (void *)&images_weights_scale,
   (void *)&maskJitterAmpl,
   (void *)&imageJitterAmpl,
   (void *)&image_value,
   (void *)&image_value_pulse,
   (void *)&image_satur,
   (void *)&image_satur_pulse,
   (void *)&mask_contrast,
   (void *)&mask_threshold,
   (void *)&mask_variance,
   (void *)&image_contrast,
   (void *)&freeze,
   (void *)&input_volume,
   (void *)&subscene_duration,
   (void *)&image_swap_duration,
   (void *)&image_swap_freq,
};
void partAccCenter_0_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void partAccCenter_1_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void is_videoOn_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void videoExposure_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void flashVideoLength_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void clearCA_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void freeze_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type,float) = { 
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   &partAccCenter_0_callBack,
   &partAccCenter_1_callBack,
   NULL,
   NULL,
   &clearAllLayers_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   &is_videoOn_callBack,
   NULL,
   NULL,
   &videoExposure_callBack,
   &flashVideoLength_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   &clearCA_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   &freeze_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
};
char *ScenarioVarMessages[_MaxInterpVarIDs] = { 
  (char *)"brushRadius_can",
  (char *)"RGBpalette_can",
  (char *)"local_color_gray_can",
  (char *)"",
  (char *)"particleMode_can",
  (char *)"part_acc_factor_can",
  (char *)"particle_texture_ID_can",
  (char *)"",
  (char *)"",
  (char *)"radiusParticle_can",
  (char *)"repopulatingBG_can",
  (char *)"",
  (char *)"echo_can",
  (char *)"echoNeg_can",
  (char *)"RGBpalette_val_coef_can",
  (char *)"RGBpalette_val_const_can",
  (char *)"flashTrack0_freq_can",
  (char *)"flashPart_freq_can",
  (char *)"flashPart_duration_can",
  (char *)"brush_radius_pressure_coef_can",
  (char *)"",
  (char *)"",
  (char *)"trackdecay_sign_0_can",
  (char *)"trackdecay_0_can",
  (char *)"trackCompositionWeight_0_can",
  (char *)"trackCompositionWeight_1_can",
  (char *)"trackFinalWeight_0_can",
  (char *)"trackFinalWeight_1_can",
  (char *)"auto_beat_can",
  (char *)"is_videoOn_can",
  (char *)"videoThreshold_can",
  (char *)"videoGamma_can",
  (char *)"videoExposure_can",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"BGSubtr_can",
  (char *)"CAType_can",
  (char *)"CASubType_can",
  (char *)"",
  (char *)"flashCA_freq_0_can",
  (char *)"flashCA_freq_1_can",
  (char *)"flashBack_freq_can",
  (char *)"CACompositionWeight_can",
  (char *)"CAFinalWeight_can",
  (char *)"CAdecay_sign_can",
  (char *)"CAdecay_can",
  (char *)"images_weights_rand_can",
  (char *)"images_weights_scale_can",
  (char *)"",
  (char *)"",
  (char *)"image_value_can",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"input_volume_can",
  (char *)"",
  (char *)"",
  (char *)"",
};
char *CmdString[_MaxInterpVarIDs] = { 
  (char *)"brushRadius",
  (char *)"RGBpalette",
  (char *)"local_color_gray",
  (char *)"blendTransp",
  (char *)"particleMode",
  (char *)"part_acc_factor",
  (char *)"particle_texture_ID",
  (char *)"partAccCenter_0",
  (char *)"partAccCenter_1",
  (char *)"radiusParticle",
  (char *)"repopulatingBG",
  (char *)"clearAllLayers",
  (char *)"echo",
  (char *)"echoNeg",
  (char *)"RGBpalette_val_coef",
  (char *)"RGBpalette_val_const",
  (char *)"flashTrack0_freq",
  (char *)"flashPart_freq",
  (char *)"flashPart_duration",
  (char *)"brush_radius_pressure_coef",
  (char *)"brush_radius_angleHor_coef",
  (char *)"brush_radius_angleVer_coef",
  (char *)"trackdecay_sign_0",
  (char *)"trackdecay_0",
  (char *)"trackCompositionWeight_0",
  (char *)"trackCompositionWeight_1",
  (char *)"trackFinalWeight_0",
  (char *)"trackFinalWeight_1",
  (char *)"auto_beat",
  (char *)"is_videoOn",
  (char *)"videoThreshold",
  (char *)"videoGamma",
  (char *)"videoExposure",
  (char *)"flashVideoLength",
  (char *)"flashVideoBright",
  (char *)"flashVideoBeat",
  (char *)"BGSubtr",
  (char *)"CAType",
  (char *)"CASubType",
  (char *)"clearCA",
  (char *)"flashCA_freq_0",
  (char *)"flashCA_freq_1",
  (char *)"flashBack_freq",
  (char *)"CACompositionWeight",
  (char *)"CAFinalWeight",
  (char *)"CAdecay_sign",
  (char *)"CAdecay",
  (char *)"images_weights_rand",
  (char *)"images_weights_scale",
  (char *)"maskJitterAmpl",
  (char *)"imageJitterAmpl",
  (char *)"image_value",
  (char *)"image_value_pulse",
  (char *)"image_satur",
  (char *)"image_satur_pulse",
  (char *)"mask_contrast",
  (char *)"mask_threshold",
  (char *)"mask_variance",
  (char *)"image_contrast",
  (char *)"freeze",
  (char *)"input_volume",
  (char *)"subscene_duration",
  (char *)"image_swap_duration",
  (char *)"image_swap_freq",
};
char CmdCharMinus[_MaxInterpVarIDs+1] = "r**********c?**********d*****x*******Hh*******i**jy****ktvz*****";
char CmdCharPlus[_MaxInterpVarIDs+1] = "R***P*******!**********D*************Gg*******I**JY****KTVZ*****";
float StepMinus[_MaxInterpVarIDs] = { 
  -1.000000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.100000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  0.000000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.001000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  0.000000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.001000F,
  -1.000000F,
  -1.000000F,
  -0.001000F,
  -0.010000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.010000F,
  -1.000000F,
  -0.100000F,
  0.000000F,
  -0.100000F,
  -1.000000F,
  -0.100000F,
  -1.000000F,
};
float StepPlus[_MaxInterpVarIDs] = { 
  1.000000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  1.000000F,
  0.000000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.001000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.001000F,
  1.000000F,
  1.000000F,
  0.001000F,
  0.010000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.010000F,
  1.000000F,
  0.100000F,
  0.000000F,
  0.100000F,
  1.000000F,
  0.100000F,
  1.000000F,
};
float MinValues[_MaxInterpVarIDs] = { 
  0.000000F,
  -9999.000000F,
  0.000000F,
  -9999.000000F,
  0.000000F,
  -9999.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  -9999.000000F,
  -1.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  -13.000000F,
  -13.000000F,
  -13.000000F,
  -1.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  -1.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  1.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  1.000000F,
  0.000000F,
  0.000000F,
};
float MaxValues[_MaxInterpVarIDs] = { 
  9999.000000F,
  9999.000000F,
  1.000000F,
  9999.000000F,
  2.000000F,
  9999.000000F,
  7.000000F,
  9999.000000F,
  9999.000000F,
  1.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  30.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  1.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  7.000000F,
  1.000000F,
  9.000000F,
  42.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1000.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1000.000000F,
  100.000000F,
  9.000000F,
};
