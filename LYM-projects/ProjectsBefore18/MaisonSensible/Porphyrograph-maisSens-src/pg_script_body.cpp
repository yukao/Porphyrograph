bool  double_window       ;
bool  flat_view           ;
bool  wide_screen         ;
int   window_width        ;
int   window_height       ;
int   window_x            ;
int   window_y            ;
float minimal_interframe_latency;
float time_scale          ;
float initial_time        ;
float input_volume        ;
int   camID               ;
float videoThreshold      ;
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
    _pg_bool,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_float,
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
   (void *)&flat_view,
   (void *)&wide_screen,
   (void *)&window_width,
   (void *)&window_height,
   (void *)&window_x,
   (void *)&window_y,
   (void *)&minimal_interframe_latency,
   (void *)&time_scale,
   (void *)&initial_time,
   (void *)&input_volume,
   (void *)&camID,
   (void *)&videoThreshold,
   (void *)&message_pixel_length,
   (void *)&trace_output_frame_number,
   (void *)&trace_time,
   (void *)&first_frame_number,
   (void *)&last_frame_number,
   (void *)&max_mouse_recording_frames,
   (void *)&max_network_message_length,
};
int   particleMode        ;
float particle_texture_ID ;
float particle_texture_noise;
float partAccCoef         ;
float partAccCenterX      ;
float partAccCenterY      ;
float radiusParticle      ;
float breathing           ;
float repopulating        ;
float repopulatingCA      ;
float hidingCoef          ;
float hidingX             ;
float hidingY             ;
int   currentDrawingStroke;
int   brushID             ;
float brushRadius         ;
int   impact_period       ;
float impact_random       ;
float impactX             ;
float impactY             ;
float RGBspectrum         ;
float local_color_r       ;
float local_color_g       ;
float local_color_b       ;
float local_color_a       ;
int   CAType              ;
int   CASubType           ;
float clearAllLayers      ;
float invertAllLayers     ;
int   cursorSize          ;
float echo                ;
float echoNeg             ;
float color_spectrum_coef ;
float part_acc_attack_coef;
float brush_radius_vol_coef;
float CAdecay_sign        ;
float trackdecay_sign_0   ;
float CAdecay             ;
float trackdecay_0        ;
float CAweight            ;
float trackweight_0       ;
float repopulatingImage   ;
float isDisplayLookAt     ;
float with_mesh           ;
float with_blue           ;
float with_whiteText      ;
float blendTransp         ;
float projLocX            ;
float projLocY            ;
float projLocZ            ;
float projUpY             ;
float projReversed        ;
float projLookAtX         ;
float projLookAtY         ;
float projLookAtZ         ;
float projWidthTopAt1m    ;
float projWidthBottomAt1m ;
float projTopAt1m         ;
float projBottomAt1m      ;
float projKeystoneXTopLeft;
float projKeystoneXBottomLeft;
float projKeystoneYTopLeft;
float projKeystoneYBottomLeft;
float projKeystoneXTopRight;
float projKeystoneXBottomRight;
float projKeystoneYTopRight;
float projKeystoneYBottomRight;
float proj2LocX           ;
float proj2LocY           ;
float proj2LocZ           ;
float proj2UpY            ;
float proj2Reversed       ;
float proj2LookAtX        ;
float proj2LookAtY        ;
float proj2LookAtZ        ;
float proj2WidthTopAt1m   ;
float proj2WidthBottomAt1m;
float proj2TopAt1m        ;
float proj2BottomAt1m     ;
float proj2KeystoneXTopLeft;
float proj2KeystoneXBottomLeft;
float proj2KeystoneYTopLeft;
float proj2KeystoneYBottomLeft;
float proj2KeystoneXTopRight;
float proj2KeystoneXBottomRight;
float proj2KeystoneYTopRight;
float proj2KeystoneYBottomRight;
float textureFrontier_wmin;
float textureFrontier_wmax;
float textureFrontier_hmin;
float textureFrontier_hmax;
float textureFrontier_wmin_width;
float textureFrontier_wmax_width;
float textureFrontier_hmin_width;
float textureFrontier_hmax_width;
float meshFrontier_X      ;
float meshFrontier_width  ;
float nearPlane           ;
float farPlane            ;
VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { 
    _pg_int,
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
    _pg_int ,
    _pg_int ,
    _pg_float,
    _pg_int ,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_int,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_float,
    _pg_sign,
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
};
void * ScenarioVarPointers[_MaxInterpVarIDs] = { 
   (void *)&particleMode,
   (void *)&particle_texture_ID,
   (void *)&particle_texture_noise,
   (void *)&partAccCoef,
   (void *)&partAccCenterX,
   (void *)&partAccCenterY,
   (void *)&radiusParticle,
   (void *)&breathing,
   (void *)&repopulating,
   (void *)&repopulatingCA,
   (void *)&hidingCoef,
   (void *)&hidingX,
   (void *)&hidingY,
   (void *)&currentDrawingStroke,
   (void *)&brushID,
   (void *)&brushRadius,
   (void *)&impact_period,
   (void *)&impact_random,
   (void *)&impactX,
   (void *)&impactY,
   (void *)&RGBspectrum,
   (void *)&local_color_r,
   (void *)&local_color_g,
   (void *)&local_color_b,
   (void *)&local_color_a,
   (void *)&CAType,
   (void *)&CASubType,
   (void *)&clearAllLayers,
   (void *)&invertAllLayers,
   (void *)&cursorSize,
   (void *)&echo,
   (void *)&echoNeg,
   (void *)&color_spectrum_coef,
   (void *)&part_acc_attack_coef,
   (void *)&brush_radius_vol_coef,
   (void *)&CAdecay_sign,
   (void *)&trackdecay_sign_0,
   (void *)&CAdecay,
   (void *)&trackdecay_0,
   (void *)&CAweight,
   (void *)&trackweight_0,
   (void *)&repopulatingImage,
   (void *)&isDisplayLookAt,
   (void *)&with_mesh,
   (void *)&with_blue,
   (void *)&with_whiteText,
   (void *)&blendTransp,
   (void *)&projLocX,
   (void *)&projLocY,
   (void *)&projLocZ,
   (void *)&projUpY,
   (void *)&projReversed,
   (void *)&projLookAtX,
   (void *)&projLookAtY,
   (void *)&projLookAtZ,
   (void *)&projWidthTopAt1m,
   (void *)&projWidthBottomAt1m,
   (void *)&projTopAt1m,
   (void *)&projBottomAt1m,
   (void *)&projKeystoneXTopLeft,
   (void *)&projKeystoneXBottomLeft,
   (void *)&projKeystoneYTopLeft,
   (void *)&projKeystoneYBottomLeft,
   (void *)&projKeystoneXTopRight,
   (void *)&projKeystoneXBottomRight,
   (void *)&projKeystoneYTopRight,
   (void *)&projKeystoneYBottomRight,
   (void *)&proj2LocX,
   (void *)&proj2LocY,
   (void *)&proj2LocZ,
   (void *)&proj2UpY,
   (void *)&proj2Reversed,
   (void *)&proj2LookAtX,
   (void *)&proj2LookAtY,
   (void *)&proj2LookAtZ,
   (void *)&proj2WidthTopAt1m,
   (void *)&proj2WidthBottomAt1m,
   (void *)&proj2TopAt1m,
   (void *)&proj2BottomAt1m,
   (void *)&proj2KeystoneXTopLeft,
   (void *)&proj2KeystoneXBottomLeft,
   (void *)&proj2KeystoneYTopLeft,
   (void *)&proj2KeystoneYBottomLeft,
   (void *)&proj2KeystoneXTopRight,
   (void *)&proj2KeystoneXBottomRight,
   (void *)&proj2KeystoneYTopRight,
   (void *)&proj2KeystoneYBottomRight,
   (void *)&textureFrontier_wmin,
   (void *)&textureFrontier_wmax,
   (void *)&textureFrontier_hmin,
   (void *)&textureFrontier_hmax,
   (void *)&textureFrontier_wmin_width,
   (void *)&textureFrontier_wmax_width,
   (void *)&textureFrontier_hmin_width,
   (void *)&textureFrontier_hmax_width,
   (void *)&meshFrontier_X,
   (void *)&meshFrontier_width,
   (void *)&nearPlane,
   (void *)&farPlane,
};
void particle_texture_ID_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void currentDrawingStroke_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void impactX_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void impactY_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void projReversed_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void proj2Reversed_callBack(pg_Parameter_Input_Type param_input_type , float scenario_or_gui_command_value);
void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type,float) = { 
   NULL,
   &particle_texture_ID_callBack,
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
   &currentDrawingStroke_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
   &impactX_callBack,
   &impactY_callBack,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
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
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   &projReversed_callBack,
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
   &proj2Reversed_callBack,
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
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
};
char *ScenarioVarMessages[_MaxInterpVarIDs] = { 
  (char *)"particleMode_can",
  (char *)"particle_texture_ID_can",
  (char *)"particle_texture_noise_can",
  (char *)"partAccCoef_can",
  (char *)"partAccCenterX_can",
  (char *)"partAccCenterY_can",
  (char *)"radiusParticle_can",
  (char *)"breathing_can",
  (char *)"repopulating_can",
  (char *)"repopulatingCA_can",
  (char *)"hidingCoef_can",
  (char *)"hidingX_can",
  (char *)"hidingY_can",
  (char *)"",
  (char *)"brushID_can",
  (char *)"brushRadius_can",
  (char *)"impact_period_can",
  (char *)"impact_random_can",
  (char *)"impactX_can",
  (char *)"impactY_can",
  (char *)"RGBspectrum_can",
  (char *)"local_color_r_can",
  (char *)"",
  (char *)"",
  (char *)"local_color_a_can",
  (char *)"CAType_can",
  (char *)"CASubType_can",
  (char *)"",
  (char *)"invertAllLayers_can",
  (char *)"",
  (char *)"echo_can",
  (char *)"echoNeg_can",
  (char *)"color_spectrum_coef_can",
  (char *)"part_acc_attack_coef_can",
  (char *)"brush_radius_vol_coef_can",
  (char *)"CAdecay_sign_can",
  (char *)"trackdecay_sign_0_can",
  (char *)"CAdecay_can",
  (char *)"trackdecay_0_can",
  (char *)"CAweight_can",
  (char *)"trackweight_0_can",
  (char *)"repopulatingImage_can",
  (char *)"isDisplayLookAt_can",
  (char *)"with_mesh_can",
  (char *)"with_blue_can",
  (char *)"with_whiteText_can",
  (char *)"blendTransp_can",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
  (char *)"",
};
char *CmdString[_MaxInterpVarIDs] = { 
  (char *)"particleMode",
  (char *)"particle_texture_ID",
  (char *)"particle_texture_noise",
  (char *)"partAccCoef",
  (char *)"partAccCenterX",
  (char *)"partAccCenterY",
  (char *)"radiusParticle",
  (char *)"breathing",
  (char *)"repopulating",
  (char *)"repopulatingCA",
  (char *)"hidingCoef",
  (char *)"hidingX",
  (char *)"hidingY",
  (char *)"currentDrawingStroke",
  (char *)"brushID",
  (char *)"brushRadius",
  (char *)"impact_period",
  (char *)"impact_random",
  (char *)"impactX",
  (char *)"impactY",
  (char *)"RGBspectrum",
  (char *)"local_color_r",
  (char *)"local_color_g",
  (char *)"local_color_b",
  (char *)"local_color_a",
  (char *)"CAType",
  (char *)"CASubType",
  (char *)"clearAllLayers",
  (char *)"invertAllLayers",
  (char *)"cursorSize",
  (char *)"echo",
  (char *)"echoNeg",
  (char *)"color_spectrum_coef",
  (char *)"part_acc_attack_coef",
  (char *)"brush_radius_vol_coef",
  (char *)"CAdecay_sign",
  (char *)"trackdecay_sign_0",
  (char *)"CAdecay",
  (char *)"trackdecay_0",
  (char *)"CAweight",
  (char *)"trackweight_0",
  (char *)"repopulatingImage",
  (char *)"isDisplayLookAt",
  (char *)"with_mesh",
  (char *)"with_blue",
  (char *)"with_whiteText",
  (char *)"blendTransp",
  (char *)"projLocX",
  (char *)"projLocY",
  (char *)"projLocZ",
  (char *)"projUpY",
  (char *)"projReversed",
  (char *)"projLookAtX",
  (char *)"projLookAtY",
  (char *)"projLookAtZ",
  (char *)"projWidthTopAt1m",
  (char *)"projWidthBottomAt1m",
  (char *)"projTopAt1m",
  (char *)"projBottomAt1m",
  (char *)"projKeystoneXTopLeft",
  (char *)"projKeystoneXBottomLeft",
  (char *)"projKeystoneYTopLeft",
  (char *)"projKeystoneYBottomLeft",
  (char *)"projKeystoneXTopRight",
  (char *)"projKeystoneXBottomRight",
  (char *)"projKeystoneYTopRight",
  (char *)"projKeystoneYBottomRight",
  (char *)"proj2LocX",
  (char *)"proj2LocY",
  (char *)"proj2LocZ",
  (char *)"proj2UpY",
  (char *)"proj2Reversed",
  (char *)"proj2LookAtX",
  (char *)"proj2LookAtY",
  (char *)"proj2LookAtZ",
  (char *)"proj2WidthTopAt1m",
  (char *)"proj2WidthBottomAt1m",
  (char *)"proj2TopAt1m",
  (char *)"proj2BottomAt1m",
  (char *)"proj2KeystoneXTopLeft",
  (char *)"proj2KeystoneXBottomLeft",
  (char *)"proj2KeystoneYTopLeft",
  (char *)"proj2KeystoneYBottomLeft",
  (char *)"proj2KeystoneXTopRight",
  (char *)"proj2KeystoneXBottomRight",
  (char *)"proj2KeystoneYTopRight",
  (char *)"proj2KeystoneYBottomRight",
  (char *)"textureFrontier_wmin",
  (char *)"textureFrontier_wmax",
  (char *)"textureFrontier_hmin",
  (char *)"textureFrontier_hmax",
  (char *)"textureFrontier_wmin_width",
  (char *)"textureFrontier_wmax_width",
  (char *)"textureFrontier_hmin_width",
  (char *)"textureFrontier_hmax_width",
  (char *)"meshFrontier_X",
  (char *)"meshFrontier_width",
  (char *)"nearPlane",
  (char *)"farPlane",
};
char CmdCharMinus[_MaxInterpVarIDs+1] = "***************************************************************************************************";
char CmdCharPlus[_MaxInterpVarIDs+1] = "***************************************************************************************************";
float StepMinus[_MaxInterpVarIDs] = { 
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  -10.000000F,
  -10.000000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  0.000000F,
  0.000000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.001000F,
  -0.001000F,
  -0.100000F,
  -0.100000F,
  -1.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  -1.000000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
  -0.100000F,
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
  -1.000000F,
  -1.000000F,
  -1.000000F,
};
float StepPlus[_MaxInterpVarIDs] = { 
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.000000F,
  10.000000F,
  10.000000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.001000F,
  0.001000F,
  0.100000F,
  0.100000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  9.000000F,
  3.500000F,
  0.100000F,
  0.100000F,
  0.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
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
  1.000000F,
  1.000000F,
  1.000000F,
};
float MinValues[_MaxInterpVarIDs] = { 
  0.000000F,
  0.000000F,
  0.000000F,
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
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  500.000000F,
  0.000000F,
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
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  -1.000000F,
  -1.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  11.000000F,
  5.500000F,
  5.500000F,
  -1.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  11.000000F,
  5.500000F,
  5.500000F,
  -1.000000F,
  0.000000F,
  -9999.000000F,
  -9999.000000F,
  -9999.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.000000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
  0.100000F,
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
  0.000000F,
  0.000000F,
};
float MaxValues[_MaxInterpVarIDs] = { 
  3.000000F,
  7.000000F,
  100.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  10.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  2.000000F,
  7.000000F,
  9999.000000F,
  7.000000F,
  1.000000F,
  1500.000000F,
  256.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  9.000000F,
  42.000000F,
  1.000000F,
  1.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  1.000000F,
  1.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  1.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  1.000000F,
  1.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  10.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
  9999.000000F,
};