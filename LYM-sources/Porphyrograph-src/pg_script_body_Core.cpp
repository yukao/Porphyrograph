// SCENARIO VARIABLES
float CA1_CA2_weight       = float(1);
int   CA1SubType           = 1;
int   CA1Type              = 1;
int   CA2SubType           = 0;
int   CA2Type              = 0;
bool  CAcolorSpread        = 1;
float CAParams            [(PG_NB_CA_PARAMS+1)];
float CAParams_pulse      [(PG_NB_CA_PARAMS+1)];
int   CAstep               = 1;
float repop_CA             = float(0);
int   flashCABG_freq       = 0;
int   flashCameraTrkBeat   = 0;
float flashCameraTrkBright = float(1);
float flashCameraTrkLength = float(10);
int   flashCAPart_freq     = 0;
int   flashPartBG_freq     = 0;
int   flashPartCA_freq     = 0;
int   flashParticleInit_freq = 0;
int   flashPhotoChangeBeat = 0;
int   flashPhotoTrkBeat    = 0;
float flashPhotoTrkBright  = float(1);
float flashPhotoTrkLength  = float(0);
int   flashPixel_duration  = 1;
int   flashPixel_freq      = 0;
int   flashTrkBG_freq     [PG_NB_TRACKS];
int   flashTrkCA_freq     [PG_NB_TRACKS];
int   flashTrkPart_freq   [PG_NB_TRACKS];
float master_crop_width    = float(0);
float master_crop_x        = float(0);
float master_crop_y        = float(1920);
float master_mask          = float(0);
float master_mask_offsetX  = float(0);
float master_mask_offsetY  = float(0);
float master_mask_opacity [(PG_NB_MASTER_MASKS+1)];
float master_mask_scale    = float(1);
float master_mask_scale_ratio = float(1);
float master_offsetX       = float(0);
float master_offsetY       = float(0);
float master_scale         = float(1);
float master_scale_pulse   = float(0);
float master_scale_ratio   = float(1);
float audioInput_weight    = float(1);
float beat_delay           = float(1000);
float beat_threshold       = float(0.13);
int   playing_soundtrackNo = -1;
float sound_env_max        = float(85);
float sound_env_min        = float(40);
float sound_min            = float(0);
float sound_volume         = float(2);
float soundtrack_PA_weight = float(0);
float soundtrack_PD_weight = float(0);
float camera_gamma         = float(1);
float camera_gamma_pulse   = float(0);
float cameraBrightness     = float(-3);
float cameraCaptFreq       = float(14);
float cameraContrast       = float(30);
int   cameraCumul          = 0;
float cameraExposure       = float(-6);
float cameraGain           = float(0);
float cameraGamma          = float(0);
int   cameraNo             = -1;
float cameraSaturation     = float(60);
float cameraSobel          = float(0);
float cameraSobel_pulse    = float(0);
float cameraThreshold      = float(0);
float cameraThreshold_pulse = float(0);
float cameraWB_B           = float(2800);
float cameraWB_R           = float(-1);
float cameraWeight         = float(0);
float cameraWeight_pulse   = float(0);
bool  invertCamera         = 0;
float clip_mix             = float(0);
float clip_nudge_factor    = float(0);
float clip_scratch_factor  = float(1.8);
float clipCaptFreq         = float(24);
string playing_clipNameLeft = "NULL";
string playing_clipNameRight = "NULL";
string playing_secondClipNameLeft = "NULL";
string playing_secondClipNameRight = "NULL";
int   activeClipArts       = 0;
float moving_messages      = float(0);
bool  invertMovie          = 0;
float movie_gamma          = float(1);
float movie_gamma_pulse    = float(0);
bool  movie_loop           = 1;
float movie_threshold      = float(0);
float movie_threshold_pulse = float(0);
float movieCaptFreq        = float(30);
float movieSobel           = float(0);
float movieSobel_pulse     = float(0);
float movieWeight          = float(0);
float movieWeight_pulse    = float(0);
int   playing_movieNo      = -1;
bool  invertPhoto          = 0;
float maskJitterAmpl       = float(0);
float photo_contrast       = float(0);
int   photo_diaporama      = -1;
float photo_diaporama_fade = float(0.2);
float photo_diaporama_plateau = float(100000);
float photo_gamma          = float(1);
float photo_gamma_pulse    = float(0);
float photo_hue            = float(0);
float photo_hue_pulse      = float(0);
float photo_offsetX        = float(0);
float photo_offsetY        = float(0);
float photo_rot            = float(0);
float photo_satur          = float(0);
float photo_satur_pulse    = float(0);
float photo_scaleX         = float(1);
float photo_scaleY         = float(1);
float photo_threshold      = float(0);
float photo_threshold_pulse = float(0);
float photo_transl_x       = float(0);
float photo_transl_y       = float(0);
float photo_value          = float(1);
float photo_value_pulse    = float(0);
float photoJitterAmpl      = float(0);
float photoSobel           = float(0);
float photoSobel_pulse     = float(0);
float photoWeight          = float(0);
float photoWeight_pulse    = float(0);
float video_contrast       = float(0);
float video_contrast_pulse = float(0);
float video_gamma          = float(1);
float video_gamma_pulse    = float(0);
float video_hue            = float(0);
float video_hue_pulse      = float(0);
float video_satur          = float(1);
float video_satur_pulse    = float(0);
float video_threshold      = float(0);
float video_threshold_pulse = float(0);
float video_value          = float(1);
float video_value_pulse    = float(0);
float video_white          = float(0);
float video_white_pulse    = float(0);
int   noiseType            = 0;
float noiseLineScale       = float(0.5);
float noiseAngleScale      = float(0.5);
float noiseCenterX         = float(1.7);
float noiseCenterY         = float(1.7);
float noiseParticleScale   = float(0.04);
float noiseParticleScale_pulse = float(0);
float part_acc             = float(0);
float part_acc_pulse       = float(0);
float part_damp            = float(0);
float part_damp_pulse      = float(0);
float part_damp_targtRad   = float(0);
float part_field_weight    = float(0);
float part_field_weight_pulse = float(0);
float part_gravity         = float(0);
float part_gravity_pulse   = float(0);
int   part_image_acceleration = -1;
int   part_initialization  = -1;
float part_size            = float(0.35114);
float part_size_pulse      = float(0);
float part_timeToTargt     = float(0);
int   partColor_mode       = 1;
int   partExit_mode        = 0;
int   particle_type        = 0;
bool  partMove_rand        = 0;
bool  partMove_target      = 0;
int   partSplat_texture    = 1;
int   partStroke_mode      = 0;
float pulsed_part_Hshift   = float(0);
float pulsed_part_Vshift   = float(0);
bool  tracksSync           = 0;
bool  part_path_follow    [PG_NB_PATHS];
bool  part_path_repop     [PG_NB_PATHS];
bool  part_path_repulse   [PG_NB_PATHS];
int   path_group           = 1;
bool  path_record         [PG_NB_PATHS];
bool  path_replay_loop     = 1;
float path_replay_speed    = float(1);
int   path_replay_trackNo [PG_NB_PATHS];
float path_scaleX          = float(1);
float path_scaleY          = float(1);
float path_translX         = float(0);
float path_translY         = float(0);
float color_spectrum_coef  = float(1);
float pen_angle_pulse      = float(0);
int   pen_brush            = 0;
int   pen_brush_replay     = 0;
float pen_color            = float(0.5);
float pen_color_a          = float(1);
float pen_color_a_pulse    = float(0);
float pen_color_pulse      = float(0);
float pen_color_replay     = float(0);
float pen_color_replay_pulse = float(0);
float pen_grey             = float(0);
float pen_grey_pulse       = float(0);
float pen_grey_replay      = float(1);
float pen_grey_replay_pulse = float(0);
float pen_hue_replay       = float(0);
float pen_hue_replay_pulse = float(0);
int   pen_position_dash    = 0;
float pen_position_pulse   = float(0);
float pen_radius           = float(4);
float pen_radius_angleHor_coef = float(0);
float pen_radius_angleVer_coef = float(0);
float pen_radius_pressure_coef = float(0.15892);
float pen_radius_pulse     = float(0);
float pen_radius_replay    = float(1);
float pen_radius_replay_pulse = float(0);
float pen_saturation_replay = float(0);
float pen_saturation_replay_pulse = float(0);
float pen_value_replay     = float(0);
float pen_value_replay_pulse = float(0);
bool  penStrokeAtBeat      = 0;
int   Pixelstep            = 1;
float noiseUpdateScale     = float(0.04);
float noiseUpdateScale_pulse = float(0);
float pixel_acc            = float(0.0001);
float pixel_acc_pulse      = float(0);
float pixel_acc_shiftX     = float(0.5);
float pixel_acc_shiftX_pulse = float(0);
float pixel_acc_shiftY     = float(0.5);
float pixel_acc_shiftY_pulse = float(0);
int   pixel_image_acceleration = -1;
int   pixel_mode           = 1;
float pixel_radius         = float(0.000142);
float pixel_radius_pulse   = float(0.999858);
int   BG_CA_repop_color_mode = 1;
int   BG_CA_repop_density  = 0;
int   Part_repop_color_mode = 1;
int   Part_repop_density   = -1;
float repop_BG             = float(0);
float repop_BG_pulse       = float(0);
float repop_CA_pulse       = float(0);
float repop_colorBG        = float(0.4286);
float repop_colorBG_pulse  = float(-0.2);
float repop_colorCA        = float(0.4286);
float repop_colorCA_pulse  = float(-0.1);
float repop_colorPart      = float(0.4286);
float repop_colorPart_pulse = float(0.1);
float repop_greyBG         = float(1);
float repop_greyBG_pulse   = float(0);
float repop_greyCA         = float(1);
float repop_greyCA_pulse   = float(0);
float repop_greyPart       = float(1);
float repop_greyPart_pulse = float(0);
float repop_part           = float(0);
float repop_part_pulse     = float(0);
float repop_path           = float(0);
float repop_path_pulse     = float(0);
string script_1             = "NULL";
int   sensor_activation    = 0;
int   sensor_layout        = 0;
float sensor_sample_setUp  = float(0);
float sensor_vol           = float(1);
bool  auto_beat            = 0;
float auto_beat_duration   = float(1);
bool  auto_pulse           = 0;
bool  camera_BG_subtr      = 0;
bool  clearAllLayers       = 0;
bool  clearCA              = 0;
bool  clearLayer           = 0;
int   cursorSize           = 2;
bool  freeze               = 0;
bool  invertAllLayers      = 0;
float master               = float(1);
bool  mute_second_screen   = 0;
bool  reset_camera         = 0;
bool  take_snapshots       = 1;
float CAdecay              = float(0.05401);
float CAdecay_pulse        = float(0.80344);
float echo                 = float(0.9996);
float echo_pulse           = float(0);
float echoNeg              = float(0.9783);
float echoNeg_pulse        = float(0);
float partDecay            = float(0);
float partDecay_pulse      = float(0);
float trkDecay            [PG_NB_TRACKS];
float trkDecay_pulse      [PG_NB_TRACKS];
float CAMasterWeight       = float(0);
float CAMasterWeight_pulse = float(0);
float CAMixingWeight       = float(0);
float CAMixingWeight_pulse = float(0);
float ClipArtMixingWeight  = float(0);
float ClipArtMixingWeight_pulse = float(0);
float ClipArtMasterWeight  = float(0);
float ClipArtMasterWeight_pulse = float(0);
float PartMasterWeight     = float(0);
float PartMasterWeight_pulse = float(0);
float PartMixingWeight     = float(0);
float PartMixingWeight_pulse = float(0);
float SecondMasterMixingWeight = float(0);
float trackMasterWeight   [PG_NB_TRACKS];
float trackMasterWeight_pulse[PG_NB_TRACKS];
float trackMixingWeight   [PG_NB_TRACKS];
float trackMixingWeight_pulse[PG_NB_TRACKS];
int   blurRadius_1         = 10;
int   blurRadius_2         = 3;
int   currentDrawingTrack  = 1;
int   currentMaskTrack     = -1;
int   currentPhotoTrack    = 2;
int   currentVideoTrack    = 2/0;
bool  slow_track_translation = 0/0;
float track_x_transl      [PG_NB_TRACKS];
float track_x_transl_pulse[PG_NB_TRACKS];
float track_y_transl      [PG_NB_TRACKS];
float track_y_transl_pulse[PG_NB_TRACKS];
float ClipArt_layer_color_preset[(PG_NB_CLIPART_LAYERS+1)];
float ClipArt_layer_color_preset_pulse[(PG_NB_CLIPART_LAYERS+1)];
int   activeMeshes         = 0;
int   mobileMeshes         = 0;
float isDisplayLookAt      = float(0);
float with_mesh            = float(0);
float with_blue            = float(0);
float with_whiteText       = float(0);
float VP1LocX              = float(-0.07449);
float VP1LocY              = float(-14.15998);
float VP1LocZ              = float(2.9375);
float VP1UpY               = float(0);
float VP1Reversed          = float(0);
float VP1LookAtX           = float(0);
float VP1LookAtY           = float(3.41096);
float VP1LookAtZ           = float(-1.07505);
float VP1WidthTopAt1m      = float(0.13925);
float VP1WidthBottomAt1m   = float(0.48402);
float VP1TopAt1m           = float(0.04954);
float VP1BottomAt1m        = float(-0.054);
float VP1KeystoneXTopLeft  = float(1);
float VP1KeystoneXBottomLeft = float(1);
float VP1KeystoneYTopLeft  = float(1);
float VP1KeystoneYBottomLeft = float(1);
float VP1KeystoneXTopRight = float(1);
float VP1KeystoneXBottomRight = float(1);
float VP1KeystoneYTopRight = float(1);
float VP1KeystoneYBottomRight = float(1);
float textureFrontier_wmin = float(0);
float textureFrontier_wmax = float(1920);
float textureFrontier_hmin = float(0);
float textureFrontier_hmax = float(1080);
float textureFrontier_wmin_width = float(0);
float textureFrontier_wmax_width = float(0);
float textureFrontier_hmin_width = float(0);
float textureFrontier_hmax_width = float(0);
float textureScale_w       = float(0.98686);
float textureScale_h       = float(1);
float textureTranslate_w   = float(0.00139);
float textureTranslate_h   = float(0);
float meshFrontier_X       = float(0);
float meshFrontier_width   = float(0);
float nearPlane            = float(1);
float farPlane             = float(50);
bool  augmentedReality     = 1;
bool  meshRenderBypass     = 0;
VarTypes ScenarioVarTypes[_MaxInterpVarIDs] = { 
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
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
	_pg_float,
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
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
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
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_string,
	_pg_string,
	_pg_string,
	_pg_string,
	_pg_int,
	_pg_float,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_bool,
	_pg_float,
	_pg_float,
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
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_int,
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
	_pg_bool,
	_pg_int,
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
	_pg_int,
	_pg_int,
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
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_float,
	_pg_string,
	_pg_int,
	_pg_int,
	_pg_float,
	_pg_float,
	_pg_bool,
	_pg_float,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_bool,
	_pg_int,
	_pg_bool,
	_pg_bool,
	_pg_float,
	_pg_bool,
	_pg_bool,
	_pg_bool,
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
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_int,
	_pg_bool,
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
	_pg_bool,
	_pg_bool,
};
int ScenarioVarIndiceRanges[_MaxInterpVarIDs][2] = { 
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{1, (PG_NB_CA_PARAMS+1)},
	{1, (PG_NB_CA_PARAMS+1)},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{1, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{1, (PG_NB_MASTER_MASKS+1)},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{0, PG_NB_PATHS},
	{0, PG_NB_PATHS},
	{0, PG_NB_PATHS},
	{-1, -1},
	{1, PG_NB_PATHS},
	{-1, -1},
	{-1, -1},
	{1, PG_NB_PATHS},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{0, PG_NB_TRACKS},
	{1, (PG_NB_CLIPART_LAYERS+1)},
	{1, (PG_NB_CLIPART_LAYERS+1)},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
	{-1, -1},
};
void * ScenarioVarPointers[_MaxInterpVarIDs] = { 
	(void *)&CA1_CA2_weight,
	(void *)&CA1SubType,
	(void *)&CA1Type,
	(void *)&CA2SubType,
	(void *)&CA2Type,
	(void *)&CAcolorSpread,
	(void *)&CAParams,
	(void *)&CAParams_pulse,
	(void *)&CAstep,
	(void *)&repop_CA,
	(void *)&flashCABG_freq,
	(void *)&flashCameraTrkBeat,
	(void *)&flashCameraTrkBright,
	(void *)&flashCameraTrkLength,
	(void *)&flashCAPart_freq,
	(void *)&flashPartBG_freq,
	(void *)&flashPartCA_freq,
	(void *)&flashParticleInit_freq,
	(void *)&flashPhotoChangeBeat,
	(void *)&flashPhotoTrkBeat,
	(void *)&flashPhotoTrkBright,
	(void *)&flashPhotoTrkLength,
	(void *)&flashPixel_duration,
	(void *)&flashPixel_freq,
	(void *)&flashTrkBG_freq,
	(void *)&flashTrkCA_freq,
	(void *)&flashTrkPart_freq,
	(void *)&master_crop_width,
	(void *)&master_crop_x,
	(void *)&master_crop_y,
	(void *)&master_mask,
	(void *)&master_mask_offsetX,
	(void *)&master_mask_offsetY,
	(void *)&master_mask_opacity,
	(void *)&master_mask_scale,
	(void *)&master_mask_scale_ratio,
	(void *)&master_offsetX,
	(void *)&master_offsetY,
	(void *)&master_scale,
	(void *)&master_scale_pulse,
	(void *)&master_scale_ratio,
	(void *)&audioInput_weight,
	(void *)&beat_delay,
	(void *)&beat_threshold,
	(void *)&playing_soundtrackNo,
	(void *)&sound_env_max,
	(void *)&sound_env_min,
	(void *)&sound_min,
	(void *)&sound_volume,
	(void *)&soundtrack_PA_weight,
	(void *)&soundtrack_PD_weight,
	(void *)&camera_gamma,
	(void *)&camera_gamma_pulse,
	(void *)&cameraBrightness,
	(void *)&cameraCaptFreq,
	(void *)&cameraContrast,
	(void *)&cameraCumul,
	(void *)&cameraExposure,
	(void *)&cameraGain,
	(void *)&cameraGamma,
	(void *)&cameraNo,
	(void *)&cameraSaturation,
	(void *)&cameraSobel,
	(void *)&cameraSobel_pulse,
	(void *)&cameraThreshold,
	(void *)&cameraThreshold_pulse,
	(void *)&cameraWB_B,
	(void *)&cameraWB_R,
	(void *)&cameraWeight,
	(void *)&cameraWeight_pulse,
	(void *)&invertCamera,
	(void *)&clip_mix,
	(void *)&clip_nudge_factor,
	(void *)&clip_scratch_factor,
	(void *)&clipCaptFreq,
	(void *)&playing_clipNameLeft,
	(void *)&playing_clipNameRight,
	(void *)&playing_secondClipNameLeft,
	(void *)&playing_secondClipNameRight,
	(void *)&activeClipArts,
	(void *)&moving_messages,
	(void *)&invertMovie,
	(void *)&movie_gamma,
	(void *)&movie_gamma_pulse,
	(void *)&movie_loop,
	(void *)&movie_threshold,
	(void *)&movie_threshold_pulse,
	(void *)&movieCaptFreq,
	(void *)&movieSobel,
	(void *)&movieSobel_pulse,
	(void *)&movieWeight,
	(void *)&movieWeight_pulse,
	(void *)&playing_movieNo,
	(void *)&invertPhoto,
	(void *)&maskJitterAmpl,
	(void *)&photo_contrast,
	(void *)&photo_diaporama,
	(void *)&photo_diaporama_fade,
	(void *)&photo_diaporama_plateau,
	(void *)&photo_gamma,
	(void *)&photo_gamma_pulse,
	(void *)&photo_hue,
	(void *)&photo_hue_pulse,
	(void *)&photo_offsetX,
	(void *)&photo_offsetY,
	(void *)&photo_rot,
	(void *)&photo_satur,
	(void *)&photo_satur_pulse,
	(void *)&photo_scaleX,
	(void *)&photo_scaleY,
	(void *)&photo_threshold,
	(void *)&photo_threshold_pulse,
	(void *)&photo_transl_x,
	(void *)&photo_transl_y,
	(void *)&photo_value,
	(void *)&photo_value_pulse,
	(void *)&photoJitterAmpl,
	(void *)&photoSobel,
	(void *)&photoSobel_pulse,
	(void *)&photoWeight,
	(void *)&photoWeight_pulse,
	(void *)&video_contrast,
	(void *)&video_contrast_pulse,
	(void *)&video_gamma,
	(void *)&video_gamma_pulse,
	(void *)&video_hue,
	(void *)&video_hue_pulse,
	(void *)&video_satur,
	(void *)&video_satur_pulse,
	(void *)&video_threshold,
	(void *)&video_threshold_pulse,
	(void *)&video_value,
	(void *)&video_value_pulse,
	(void *)&video_white,
	(void *)&video_white_pulse,
	(void *)&noiseType,
	(void *)&noiseLineScale,
	(void *)&noiseAngleScale,
	(void *)&noiseCenterX,
	(void *)&noiseCenterY,
	(void *)&noiseParticleScale,
	(void *)&noiseParticleScale_pulse,
	(void *)&part_acc,
	(void *)&part_acc_pulse,
	(void *)&part_damp,
	(void *)&part_damp_pulse,
	(void *)&part_damp_targtRad,
	(void *)&part_field_weight,
	(void *)&part_field_weight_pulse,
	(void *)&part_gravity,
	(void *)&part_gravity_pulse,
	(void *)&part_image_acceleration,
	(void *)&part_initialization,
	(void *)&part_size,
	(void *)&part_size_pulse,
	(void *)&part_timeToTargt,
	(void *)&partColor_mode,
	(void *)&partExit_mode,
	(void *)&particle_type,
	(void *)&partMove_rand,
	(void *)&partMove_target,
	(void *)&partSplat_texture,
	(void *)&partStroke_mode,
	(void *)&pulsed_part_Hshift,
	(void *)&pulsed_part_Vshift,
	(void *)&tracksSync,
	(void *)&part_path_follow,
	(void *)&part_path_repop,
	(void *)&part_path_repulse,
	(void *)&path_group,
	(void *)&path_record,
	(void *)&path_replay_loop,
	(void *)&path_replay_speed,
	(void *)&path_replay_trackNo,
	(void *)&path_scaleX,
	(void *)&path_scaleY,
	(void *)&path_translX,
	(void *)&path_translY,
	(void *)&color_spectrum_coef,
	(void *)&pen_angle_pulse,
	(void *)&pen_brush,
	(void *)&pen_brush_replay,
	(void *)&pen_color,
	(void *)&pen_color_a,
	(void *)&pen_color_a_pulse,
	(void *)&pen_color_pulse,
	(void *)&pen_color_replay,
	(void *)&pen_color_replay_pulse,
	(void *)&pen_grey,
	(void *)&pen_grey_pulse,
	(void *)&pen_grey_replay,
	(void *)&pen_grey_replay_pulse,
	(void *)&pen_hue_replay,
	(void *)&pen_hue_replay_pulse,
	(void *)&pen_position_dash,
	(void *)&pen_position_pulse,
	(void *)&pen_radius,
	(void *)&pen_radius_angleHor_coef,
	(void *)&pen_radius_angleVer_coef,
	(void *)&pen_radius_pressure_coef,
	(void *)&pen_radius_pulse,
	(void *)&pen_radius_replay,
	(void *)&pen_radius_replay_pulse,
	(void *)&pen_saturation_replay,
	(void *)&pen_saturation_replay_pulse,
	(void *)&pen_value_replay,
	(void *)&pen_value_replay_pulse,
	(void *)&penStrokeAtBeat,
	(void *)&Pixelstep,
	(void *)&noiseUpdateScale,
	(void *)&noiseUpdateScale_pulse,
	(void *)&pixel_acc,
	(void *)&pixel_acc_pulse,
	(void *)&pixel_acc_shiftX,
	(void *)&pixel_acc_shiftX_pulse,
	(void *)&pixel_acc_shiftY,
	(void *)&pixel_acc_shiftY_pulse,
	(void *)&pixel_image_acceleration,
	(void *)&pixel_mode,
	(void *)&pixel_radius,
	(void *)&pixel_radius_pulse,
	(void *)&BG_CA_repop_color_mode,
	(void *)&BG_CA_repop_density,
	(void *)&Part_repop_color_mode,
	(void *)&Part_repop_density,
	(void *)&repop_BG,
	(void *)&repop_BG_pulse,
	(void *)&repop_CA_pulse,
	(void *)&repop_colorBG,
	(void *)&repop_colorBG_pulse,
	(void *)&repop_colorCA,
	(void *)&repop_colorCA_pulse,
	(void *)&repop_colorPart,
	(void *)&repop_colorPart_pulse,
	(void *)&repop_greyBG,
	(void *)&repop_greyBG_pulse,
	(void *)&repop_greyCA,
	(void *)&repop_greyCA_pulse,
	(void *)&repop_greyPart,
	(void *)&repop_greyPart_pulse,
	(void *)&repop_part,
	(void *)&repop_part_pulse,
	(void *)&repop_path,
	(void *)&repop_path_pulse,
	(void *)&script_1,
	(void *)&sensor_activation,
	(void *)&sensor_layout,
	(void *)&sensor_sample_setUp,
	(void *)&sensor_vol,
	(void *)&auto_beat,
	(void *)&auto_beat_duration,
	(void *)&auto_pulse,
	(void *)&camera_BG_subtr,
	(void *)&clearAllLayers,
	(void *)&clearCA,
	(void *)&clearLayer,
	(void *)&cursorSize,
	(void *)&freeze,
	(void *)&invertAllLayers,
	(void *)&master,
	(void *)&mute_second_screen,
	(void *)&reset_camera,
	(void *)&take_snapshots,
	(void *)&CAdecay,
	(void *)&CAdecay_pulse,
	(void *)&echo,
	(void *)&echo_pulse,
	(void *)&echoNeg,
	(void *)&echoNeg_pulse,
	(void *)&partDecay,
	(void *)&partDecay_pulse,
	(void *)&trkDecay,
	(void *)&trkDecay_pulse,
	(void *)&CAMasterWeight,
	(void *)&CAMasterWeight_pulse,
	(void *)&CAMixingWeight,
	(void *)&CAMixingWeight_pulse,
	(void *)&ClipArtMixingWeight,
	(void *)&ClipArtMixingWeight_pulse,
	(void *)&ClipArtMasterWeight,
	(void *)&ClipArtMasterWeight_pulse,
	(void *)&PartMasterWeight,
	(void *)&PartMasterWeight_pulse,
	(void *)&PartMixingWeight,
	(void *)&PartMixingWeight_pulse,
	(void *)&SecondMasterMixingWeight,
	(void *)&trackMasterWeight,
	(void *)&trackMasterWeight_pulse,
	(void *)&trackMixingWeight,
	(void *)&trackMixingWeight_pulse,
	(void *)&blurRadius_1,
	(void *)&blurRadius_2,
	(void *)&currentDrawingTrack,
	(void *)&currentMaskTrack,
	(void *)&currentPhotoTrack,
	(void *)&currentVideoTrack,
	(void *)&slow_track_translation,
	(void *)&track_x_transl,
	(void *)&track_x_transl_pulse,
	(void *)&track_y_transl,
	(void *)&track_y_transl_pulse,
	(void *)&ClipArt_layer_color_preset,
	(void *)&ClipArt_layer_color_preset_pulse,
	(void *)&activeMeshes,
	(void *)&mobileMeshes,
	(void *)&isDisplayLookAt,
	(void *)&with_mesh,
	(void *)&with_blue,
	(void *)&with_whiteText,
	(void *)&VP1LocX,
	(void *)&VP1LocY,
	(void *)&VP1LocZ,
	(void *)&VP1UpY,
	(void *)&VP1Reversed,
	(void *)&VP1LookAtX,
	(void *)&VP1LookAtY,
	(void *)&VP1LookAtZ,
	(void *)&VP1WidthTopAt1m,
	(void *)&VP1WidthBottomAt1m,
	(void *)&VP1TopAt1m,
	(void *)&VP1BottomAt1m,
	(void *)&VP1KeystoneXTopLeft,
	(void *)&VP1KeystoneXBottomLeft,
	(void *)&VP1KeystoneYTopLeft,
	(void *)&VP1KeystoneYBottomLeft,
	(void *)&VP1KeystoneXTopRight,
	(void *)&VP1KeystoneXBottomRight,
	(void *)&VP1KeystoneYTopRight,
	(void *)&VP1KeystoneYBottomRight,
	(void *)&textureFrontier_wmin,
	(void *)&textureFrontier_wmax,
	(void *)&textureFrontier_hmin,
	(void *)&textureFrontier_hmax,
	(void *)&textureFrontier_wmin_width,
	(void *)&textureFrontier_wmax_width,
	(void *)&textureFrontier_hmin_width,
	(void *)&textureFrontier_hmax_width,
	(void *)&textureScale_w,
	(void *)&textureScale_h,
	(void *)&textureTranslate_w,
	(void *)&textureTranslate_h,
	(void *)&meshFrontier_X,
	(void *)&meshFrontier_width,
	(void *)&nearPlane,
	(void *)&farPlane,
	(void *)&augmentedReality,
	(void *)&meshRenderBypass,
};
void flashCameraTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void flashCameraTrkLength_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	flashCameraTrkLength_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void flashPhotoTrkLength_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void flashPhotoTrkLength_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	flashPhotoTrkLength_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void audioInput_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void audioInput_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	audioInput_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void beat_delay_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void beat_delay_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	beat_delay_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void beat_threshold_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void beat_threshold_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	beat_threshold_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void playing_soundtrackNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_soundtrackNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_soundtrackNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void sound_env_max_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sound_env_max_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sound_env_max_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void sound_env_min_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sound_env_min_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sound_env_min_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void soundtrack_PA_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void soundtrack_PA_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	soundtrack_PA_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void soundtrack_PD_weight_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void soundtrack_PD_weight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	soundtrack_PD_weight_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraBrightness_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraBrightness_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraBrightness_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraContrast_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraContrast_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraContrast_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraExposure_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraExposure_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraExposure_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraGain_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraGain_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraGain_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraGamma_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraGamma_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraGamma_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void cameraNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void cameraSaturation_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraSaturation_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraSaturation_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraWB_B_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraWB_B_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraWB_B_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void cameraWB_R_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void cameraWB_R_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	cameraWB_R_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void playing_clipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_clipNameLeft_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_clipNameLeft_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_clipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_clipNameRight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_clipNameRight_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_secondClipNameLeft_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_secondClipNameLeft_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_secondClipNameLeft_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void playing_secondClipNameRight_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void playing_secondClipNameRight_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_secondClipNameRight_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void movieCaptFreq_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void movieCaptFreq_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	movieCaptFreq_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void playing_movieNo_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void playing_movieNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	playing_movieNo_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void photo_diaporama_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void photo_diaporama_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	photo_diaporama_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void part_initialization_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void part_initialization_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	part_initialization_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void partMove_rand_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void partMove_rand_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	partMove_rand_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void partMove_target_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void partMove_target_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	partMove_target_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void part_path_follow_callBack(int ind_path, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_follow_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	for (int index = 0; index < PG_NB_PATHS; index++) {;
		part_path_follow_callBack(index, param_input_type, bool(scenario_or_gui_command_value.val_array[index]));
	}
}
void part_path_repulse_callBack(int ind_path, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void part_path_repulse_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	for (int index = 0; index < PG_NB_PATHS; index++) {;
		part_path_repulse_callBack(index, param_input_type, bool(scenario_or_gui_command_value.val_array[index]));
	}
}
void path_group_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_group_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	path_group_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void path_record_callBack(int ind_path, pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void path_record_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	for (int index = 1; index < PG_NB_PATHS; index++) {;
		path_record_callBack(index, param_input_type, bool(scenario_or_gui_command_value.val_array[index]));
	}
}
void path_replay_trackNo_callBack(int ind_path, pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void path_replay_trackNo_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	for (int index = 1; index < PG_NB_PATHS; index++) {;
		path_replay_trackNo_callBack(index, param_input_type, int(scenario_or_gui_command_value.val_array[index]));
	}
}
void pen_brush_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void pen_brush_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	pen_brush_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void pen_color_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void pen_color_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	pen_color_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void script_1_callBack(pg_Parameter_Input_Type param_input_type, string scenario_or_gui_command_value);
void script_1_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	script_1_callBack(param_input_type, scenario_or_gui_command_value.val_string);
}
void sensor_activation_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void sensor_activation_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_activation_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void sensor_layout_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void sensor_layout_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_layout_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void sensor_sample_setUp_callBack(pg_Parameter_Input_Type param_input_type, float scenario_or_gui_command_value);
void sensor_sample_setUp_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	sensor_sample_setUp_callBack(param_input_type, float(scenario_or_gui_command_value.val_num));
}
void auto_beat_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void auto_beat_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	auto_beat_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void auto_pulse_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void auto_pulse_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	auto_pulse_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearAllLayers_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearAllLayers_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearAllLayers_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearCA_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearCA_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearCA_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void clearLayer_callBack(pg_Parameter_Input_Type param_input_type, bool scenario_or_gui_command_value);
void clearLayer_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	clearLayer_callBack(param_input_type, double_to_bool(scenario_or_gui_command_value.val_num));
}
void activeMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void activeMeshes_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	activeMeshes_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void mobileMeshes_callBack(pg_Parameter_Input_Type param_input_type, int scenario_or_gui_command_value);
void mobileMeshes_callBack_generic(pg_Parameter_Input_Type param_input_type, ScenarioValue scenario_or_gui_command_value) {
	mobileMeshes_callBack(param_input_type, int(scenario_or_gui_command_value.val_num));
}
void (*ScenarioVarCallbacks[_MaxInterpVarIDs])(pg_Parameter_Input_Type, ScenarioValue) = { 
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
	&flashCameraTrkLength_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&flashPhotoTrkLength_callBack_generic,
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
	&audioInput_weight_callBack_generic,
	&beat_delay_callBack_generic,
	&beat_threshold_callBack_generic,
	&playing_soundtrackNo_callBack_generic,
	&sound_env_max_callBack_generic,
	&sound_env_min_callBack_generic,
	NULL,
	NULL,
	&soundtrack_PA_weight_callBack_generic,
	&soundtrack_PD_weight_callBack_generic,
	NULL,
	NULL,
	&cameraBrightness_callBack_generic,
	NULL,
	&cameraContrast_callBack_generic,
	NULL,
	&cameraExposure_callBack_generic,
	&cameraGain_callBack_generic,
	&cameraGamma_callBack_generic,
	&cameraNo_callBack_generic,
	&cameraSaturation_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	&cameraWB_B_callBack_generic,
	&cameraWB_R_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&playing_clipNameLeft_callBack_generic,
	&playing_clipNameRight_callBack_generic,
	&playing_secondClipNameLeft_callBack_generic,
	&playing_secondClipNameRight_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&movieCaptFreq_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	&playing_movieNo_callBack_generic,
	NULL,
	NULL,
	NULL,
	&photo_diaporama_callBack_generic,
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
	NULL,
	&part_initialization_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&partMove_rand_callBack_generic,
	&partMove_target_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&part_path_follow_callBack_generic,
	NULL,
	&part_path_repulse_callBack_generic,
	&path_group_callBack_generic,
	&path_record_callBack_generic,
	NULL,
	NULL,
	&path_replay_trackNo_callBack_generic,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&pen_brush_callBack_generic,
	NULL,
	&pen_color_callBack_generic,
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
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&script_1_callBack_generic,
	&sensor_activation_callBack_generic,
	&sensor_layout_callBack_generic,
	&sensor_sample_setUp_callBack_generic,
	NULL,
	&auto_beat_callBack_generic,
	NULL,
	&auto_pulse_callBack_generic,
	NULL,
	&clearAllLayers_callBack_generic,
	&clearCA_callBack_generic,
	&clearLayer_callBack_generic,
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
	&activeMeshes_callBack_generic,
	&mobileMeshes_callBack_generic,
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
  (char *)"CA1_CA2_weight",
  (char *)"CA1SubType",
  (char *)"CA1Type",
  (char *)"CA2SubType",
  (char *)"CA2Type",
  (char *)"CAcolorSpread",
  (char *)"CAParams",
  (char *)"CAParams_pulse",
  (char *)"CAstep",
  (char *)"repop_CA",
  (char *)"flashCABG_freq",
  (char *)"flashCameraTrkBeat",
  (char *)"flashCameraTrkBright",
  (char *)"flashCameraTrkLength",
  (char *)"flashCAPart_freq",
  (char *)"flashPartBG_freq",
  (char *)"flashPartCA_freq",
  (char *)"flashParticleInit_freq",
  (char *)"flashPhotoChangeBeat",
  (char *)"flashPhotoTrkBeat",
  (char *)"flashPhotoTrkBright",
  (char *)"flashPhotoTrkLength",
  (char *)"flashPixel_duration",
  (char *)"flashPixel_freq",
  (char *)"flashTrkBG_freq",
  (char *)"flashTrkCA_freq",
  (char *)"flashTrkPart_freq",
  (char *)"master_crop_width",
  (char *)"master_crop_x",
  (char *)"master_crop_y",
  (char *)"master_mask",
  (char *)"master_mask_offsetX",
  (char *)"master_mask_offsetY",
  (char *)"master_mask_opacity",
  (char *)"master_mask_scale",
  (char *)"master_mask_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"audioInput_weight",
  (char *)"beat_delay",
  (char *)"beat_threshold",
  (char *)"playing_soundtrackNo",
  (char *)"sound_env_max",
  (char *)"sound_env_min",
  (char *)"sound_min",
  (char *)"sound_volume",
  (char *)"soundtrack_PA_weight",
  (char *)"soundtrack_PD_weight",
  (char *)"camera_gamma",
  (char *)"camera_gamma_pulse",
  (char *)"cameraBrightness",
  (char *)"cameraCaptFreq",
  (char *)"cameraContrast",
  (char *)"cameraCumul",
  (char *)"cameraExposure",
  (char *)"cameraGain",
  (char *)"cameraGamma",
  (char *)"cameraNo",
  (char *)"cameraSaturation",
  (char *)"cameraSobel",
  (char *)"cameraSobel_pulse",
  (char *)"cameraThreshold",
  (char *)"cameraThreshold_pulse",
  (char *)"cameraWB_B",
  (char *)"cameraWB_R",
  (char *)"cameraWeight",
  (char *)"cameraWeight_pulse",
  (char *)"invertCamera",
  (char *)"clip_mix",
  (char *)"clip_nudge_factor",
  (char *)"clip_scratch_factor",
  (char *)"clipCaptFreq",
  (char *)"playing_clipNameLeft",
  (char *)"playing_clipNameRight",
  (char *)"playing_secondClipNameLeft",
  (char *)"playing_secondClipNameRight",
  (char *)"activeClipArts",
  (char *)"moving_messages",
  (char *)"invertMovie",
  (char *)"movie_gamma",
  (char *)"movie_gamma_pulse",
  (char *)"movie_loop",
  (char *)"movie_threshold",
  (char *)"movie_threshold_pulse",
  (char *)"movieCaptFreq",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"playing_movieNo",
  (char *)"invertPhoto",
  (char *)"maskJitterAmpl",
  (char *)"photo_contrast",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_hue",
  (char *)"photo_hue_pulse",
  (char *)"photo_offsetX",
  (char *)"photo_offsetY",
  (char *)"photo_rot",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_scaleX",
  (char *)"photo_scaleY",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photoJitterAmpl",
  (char *)"photoSobel",
  (char *)"photoSobel_pulse",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"video_contrast",
  (char *)"video_contrast_pulse",
  (char *)"video_gamma",
  (char *)"video_gamma_pulse",
  (char *)"video_hue",
  (char *)"video_hue_pulse",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_threshold",
  (char *)"video_threshold_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"video_white",
  (char *)"video_white_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
  (char *)"noiseParticleScale",
  (char *)"noiseParticleScale_pulse",
  (char *)"part_acc",
  (char *)"part_acc_pulse",
  (char *)"part_damp",
  (char *)"part_damp_pulse",
  (char *)"part_damp_targtRad",
  (char *)"part_field_weight",
  (char *)"part_field_weight_pulse",
  (char *)"part_gravity",
  (char *)"part_gravity_pulse",
  (char *)"part_image_acceleration",
  (char *)"part_initialization",
  (char *)"part_size",
  (char *)"part_size_pulse",
  (char *)"part_timeToTargt",
  (char *)"partColor_mode",
  (char *)"partExit_mode",
  (char *)"particle_type",
  (char *)"partMove_rand",
  (char *)"partMove_target",
  (char *)"partSplat_texture",
  (char *)"partStroke_mode",
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"tracksSync",
  (char *)"part_path_follow",
  (char *)"part_path_repop",
  (char *)"part_path_repulse",
  (char *)"path_group",
  (char *)"path_record",
  (char *)"path_replay_loop",
  (char *)"path_replay_speed",
  (char *)"path_replay_trackNo",
  (char *)"path_scaleX",
  (char *)"path_scaleY",
  (char *)"path_translX",
  (char *)"path_translY",
  (char *)"color_spectrum_coef",
  (char *)"pen_angle_pulse",
  (char *)"pen_brush",
  (char *)"pen_brush_replay",
  (char *)"pen_color",
  (char *)"pen_color_a",
  (char *)"pen_color_a_pulse",
  (char *)"pen_color_pulse",
  (char *)"pen_color_replay",
  (char *)"pen_color_replay_pulse",
  (char *)"pen_grey",
  (char *)"pen_grey_pulse",
  (char *)"pen_grey_replay",
  (char *)"pen_grey_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_position_dash",
  (char *)"pen_position_pulse",
  (char *)"pen_radius",
  (char *)"pen_radius_angleHor_coef",
  (char *)"pen_radius_angleVer_coef",
  (char *)"pen_radius_pressure_coef",
  (char *)"pen_radius_pulse",
  (char *)"pen_radius_replay",
  (char *)"pen_radius_replay_pulse",
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"penStrokeAtBeat",
  (char *)"Pixelstep",
  (char *)"noiseUpdateScale",
  (char *)"noiseUpdateScale_pulse",
  (char *)"pixel_acc",
  (char *)"pixel_acc_pulse",
  (char *)"pixel_acc_shiftX",
  (char *)"pixel_acc_shiftX_pulse",
  (char *)"pixel_acc_shiftY",
  (char *)"pixel_acc_shiftY_pulse",
  (char *)"pixel_image_acceleration",
  (char *)"pixel_mode",
  (char *)"pixel_radius",
  (char *)"pixel_radius_pulse",
  (char *)"BG_CA_repop_color_mode",
  (char *)"BG_CA_repop_density",
  (char *)"Part_repop_color_mode",
  (char *)"Part_repop_density",
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_CA_pulse",
  (char *)"repop_colorBG",
  (char *)"repop_colorBG_pulse",
  (char *)"repop_colorCA",
  (char *)"repop_colorCA_pulse",
  (char *)"repop_colorPart",
  (char *)"repop_colorPart_pulse",
  (char *)"repop_greyBG",
  (char *)"repop_greyBG_pulse",
  (char *)"repop_greyCA",
  (char *)"repop_greyCA_pulse",
  (char *)"repop_greyPart",
  (char *)"repop_greyPart_pulse",
  (char *)"repop_part",
  (char *)"repop_part_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"script_1",
  (char *)"sensor_activation",
  (char *)"sensor_layout",
  (char *)"sensor_sample_setUp",
  (char *)"sensor_vol",
  (char *)"auto_beat",
  (char *)"auto_beat_duration",
  (char *)"auto_pulse",
  (char *)"camera_BG_subtr",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"cursorSize",
  (char *)"freeze",
  (char *)"invertAllLayers",
  (char *)"master",
  (char *)"mute_second_screen",
  (char *)"reset_camera",
  (char *)"take_snapshots",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"partDecay",
  (char *)"partDecay_pulse",
  (char *)"trkDecay",
  (char *)"trkDecay_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"ClipArtMixingWeight",
  (char *)"ClipArtMixingWeight_pulse",
  (char *)"ClipArtMasterWeight",
  (char *)"ClipArtMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"SecondMasterMixingWeight",
  (char *)"trackMasterWeight",
  (char *)"trackMasterWeight_pulse",
  (char *)"trackMixingWeight",
  (char *)"trackMixingWeight_pulse",
  (char *)"blurRadius_1",
  (char *)"blurRadius_2",
  (char *)"currentDrawingTrack",
  (char *)"currentMaskTrack",
  (char *)"currentPhotoTrack",
  (char *)"currentVideoTrack",
  (char *)"slow_track_translation",
  (char *)"track_x_transl",
  (char *)"track_x_transl_pulse",
  (char *)"track_y_transl",
  (char *)"track_y_transl_pulse",
  (char *)"ClipArt_layer_color_preset",
  (char *)"ClipArt_layer_color_preset_pulse",
  (char *)"activeMeshes",
  (char *)"mobileMeshes",
  (char *)"isDisplayLookAt",
  (char *)"with_mesh",
  (char *)"with_blue",
  (char *)"with_whiteText",
  (char *)"VP1LocX",
  (char *)"VP1LocY",
  (char *)"VP1LocZ",
  (char *)"VP1UpY",
  (char *)"VP1Reversed",
  (char *)"VP1LookAtX",
  (char *)"VP1LookAtY",
  (char *)"VP1LookAtZ",
  (char *)"VP1WidthTopAt1m",
  (char *)"VP1WidthBottomAt1m",
  (char *)"VP1TopAt1m",
  (char *)"VP1BottomAt1m",
  (char *)"VP1KeystoneXTopLeft",
  (char *)"VP1KeystoneXBottomLeft",
  (char *)"VP1KeystoneYTopLeft",
  (char *)"VP1KeystoneYBottomLeft",
  (char *)"VP1KeystoneXTopRight",
  (char *)"VP1KeystoneXBottomRight",
  (char *)"VP1KeystoneYTopRight",
  (char *)"VP1KeystoneYBottomRight",
  (char *)"textureFrontier_wmin",
  (char *)"textureFrontier_wmax",
  (char *)"textureFrontier_hmin",
  (char *)"textureFrontier_hmax",
  (char *)"textureFrontier_wmin_width",
  (char *)"textureFrontier_wmax_width",
  (char *)"textureFrontier_hmin_width",
  (char *)"textureFrontier_hmax_width",
  (char *)"textureScale_w",
  (char *)"textureScale_h",
  (char *)"textureTranslate_w",
  (char *)"textureTranslate_h",
  (char *)"meshFrontier_X",
  (char *)"meshFrontier_width",
  (char *)"nearPlane",
  (char *)"farPlane",
  (char *)"augmentedReality",
  (char *)"meshRenderBypass",
};
PulseTypes ScenarioVarPulse[_MaxInterpVarIDs] = { 
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_special,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_uniform,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_differential,
  _pg_pulsed_none,
  _pg_pulsed_absolute,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
  _pg_pulsed_none,
};
char *ScenarioVarStrings[_MaxInterpVarIDs] = { 
  (char *)"CA1_CA2_weight",
  (char *)"CA1SubType",
  (char *)"CA1Type",
  (char *)"CA2SubType",
  (char *)"CA2Type",
  (char *)"CAcolorSpread",
  (char *)"CAParams",
  (char *)"CAParams_pulse",
  (char *)"CAstep",
  (char *)"repop_CA",
  (char *)"flashCABG_freq",
  (char *)"flashCameraTrkBeat",
  (char *)"flashCameraTrkBright",
  (char *)"flashCameraTrkLength",
  (char *)"flashCAPart_freq",
  (char *)"flashPartBG_freq",
  (char *)"flashPartCA_freq",
  (char *)"flashParticleInit_freq",
  (char *)"flashPhotoChangeBeat",
  (char *)"flashPhotoTrkBeat",
  (char *)"flashPhotoTrkBright",
  (char *)"flashPhotoTrkLength",
  (char *)"flashPixel_duration",
  (char *)"flashPixel_freq",
  (char *)"flashTrkBG_freq",
  (char *)"flashTrkCA_freq",
  (char *)"flashTrkPart_freq",
  (char *)"master_crop_width",
  (char *)"master_crop_x",
  (char *)"master_crop_y",
  (char *)"master_mask",
  (char *)"master_mask_offsetX",
  (char *)"master_mask_offsetY",
  (char *)"master_mask_opacity",
  (char *)"master_mask_scale",
  (char *)"master_mask_scale_ratio",
  (char *)"master_offsetX",
  (char *)"master_offsetY",
  (char *)"master_scale",
  (char *)"master_scale_pulse",
  (char *)"master_scale_ratio",
  (char *)"audioInput_weight",
  (char *)"beat_delay",
  (char *)"beat_threshold",
  (char *)"playing_soundtrackNo",
  (char *)"sound_env_max",
  (char *)"sound_env_min",
  (char *)"sound_min",
  (char *)"sound_volume",
  (char *)"soundtrack_PA_weight",
  (char *)"soundtrack_PD_weight",
  (char *)"camera_gamma",
  (char *)"camera_gamma_pulse",
  (char *)"cameraBrightness",
  (char *)"cameraCaptFreq",
  (char *)"cameraContrast",
  (char *)"cameraCumul",
  (char *)"cameraExposure",
  (char *)"cameraGain",
  (char *)"cameraGamma",
  (char *)"cameraNo",
  (char *)"cameraSaturation",
  (char *)"cameraSobel",
  (char *)"cameraSobel_pulse",
  (char *)"cameraThreshold",
  (char *)"cameraThreshold_pulse",
  (char *)"cameraWB_B",
  (char *)"cameraWB_R",
  (char *)"cameraWeight",
  (char *)"cameraWeight_pulse",
  (char *)"invertCamera",
  (char *)"clip_mix",
  (char *)"clip_nudge_factor",
  (char *)"clip_scratch_factor",
  (char *)"clipCaptFreq",
  (char *)"playing_clipNameLeft",
  (char *)"playing_clipNameRight",
  (char *)"playing_secondClipNameLeft",
  (char *)"playing_secondClipNameRight",
  (char *)"activeClipArts",
  (char *)"moving_messages",
  (char *)"invertMovie",
  (char *)"movie_gamma",
  (char *)"movie_gamma_pulse",
  (char *)"movie_loop",
  (char *)"movie_threshold",
  (char *)"movie_threshold_pulse",
  (char *)"movieCaptFreq",
  (char *)"movieSobel",
  (char *)"movieSobel_pulse",
  (char *)"movieWeight",
  (char *)"movieWeight_pulse",
  (char *)"playing_movieNo",
  (char *)"invertPhoto",
  (char *)"maskJitterAmpl",
  (char *)"photo_contrast",
  (char *)"photo_diaporama",
  (char *)"photo_diaporama_fade",
  (char *)"photo_diaporama_plateau",
  (char *)"photo_gamma",
  (char *)"photo_gamma_pulse",
  (char *)"photo_hue",
  (char *)"photo_hue_pulse",
  (char *)"photo_offsetX",
  (char *)"photo_offsetY",
  (char *)"photo_rot",
  (char *)"photo_satur",
  (char *)"photo_satur_pulse",
  (char *)"photo_scaleX",
  (char *)"photo_scaleY",
  (char *)"photo_threshold",
  (char *)"photo_threshold_pulse",
  (char *)"photo_transl_x",
  (char *)"photo_transl_y",
  (char *)"photo_value",
  (char *)"photo_value_pulse",
  (char *)"photoJitterAmpl",
  (char *)"photoSobel",
  (char *)"photoSobel_pulse",
  (char *)"photoWeight",
  (char *)"photoWeight_pulse",
  (char *)"video_contrast",
  (char *)"video_contrast_pulse",
  (char *)"video_gamma",
  (char *)"video_gamma_pulse",
  (char *)"video_hue",
  (char *)"video_hue_pulse",
  (char *)"video_satur",
  (char *)"video_satur_pulse",
  (char *)"video_threshold",
  (char *)"video_threshold_pulse",
  (char *)"video_value",
  (char *)"video_value_pulse",
  (char *)"video_white",
  (char *)"video_white_pulse",
  (char *)"noiseType",
  (char *)"noiseLineScale",
  (char *)"noiseAngleScale",
  (char *)"noiseCenterX",
  (char *)"noiseCenterY",
  (char *)"noiseParticleScale",
  (char *)"noiseParticleScale_pulse",
  (char *)"part_acc",
  (char *)"part_acc_pulse",
  (char *)"part_damp",
  (char *)"part_damp_pulse",
  (char *)"part_damp_targtRad",
  (char *)"part_field_weight",
  (char *)"part_field_weight_pulse",
  (char *)"part_gravity",
  (char *)"part_gravity_pulse",
  (char *)"part_image_acceleration",
  (char *)"part_initialization",
  (char *)"part_size",
  (char *)"part_size_pulse",
  (char *)"part_timeToTargt",
  (char *)"partColor_mode",
  (char *)"partExit_mode",
  (char *)"particle_type",
  (char *)"partMove_rand",
  (char *)"partMove_target",
  (char *)"partSplat_texture",
  (char *)"partStroke_mode",
  (char *)"pulsed_part_Hshift",
  (char *)"pulsed_part_Vshift",
  (char *)"tracksSync",
  (char *)"part_path_follow",
  (char *)"part_path_repop",
  (char *)"part_path_repulse",
  (char *)"path_group",
  (char *)"path_record",
  (char *)"path_replay_loop",
  (char *)"path_replay_speed",
  (char *)"path_replay_trackNo",
  (char *)"path_scaleX",
  (char *)"path_scaleY",
  (char *)"path_translX",
  (char *)"path_translY",
  (char *)"color_spectrum_coef",
  (char *)"pen_angle_pulse",
  (char *)"pen_brush",
  (char *)"pen_brush_replay",
  (char *)"pen_color",
  (char *)"pen_color_a",
  (char *)"pen_color_a_pulse",
  (char *)"pen_color_pulse",
  (char *)"pen_color_replay",
  (char *)"pen_color_replay_pulse",
  (char *)"pen_grey",
  (char *)"pen_grey_pulse",
  (char *)"pen_grey_replay",
  (char *)"pen_grey_replay_pulse",
  (char *)"pen_hue_replay",
  (char *)"pen_hue_replay_pulse",
  (char *)"pen_position_dash",
  (char *)"pen_position_pulse",
  (char *)"pen_radius",
  (char *)"pen_radius_angleHor_coef",
  (char *)"pen_radius_angleVer_coef",
  (char *)"pen_radius_pressure_coef",
  (char *)"pen_radius_pulse",
  (char *)"pen_radius_replay",
  (char *)"pen_radius_replay_pulse",
  (char *)"pen_saturation_replay",
  (char *)"pen_saturation_replay_pulse",
  (char *)"pen_value_replay",
  (char *)"pen_value_replay_pulse",
  (char *)"penStrokeAtBeat",
  (char *)"Pixelstep",
  (char *)"noiseUpdateScale",
  (char *)"noiseUpdateScale_pulse",
  (char *)"pixel_acc",
  (char *)"pixel_acc_pulse",
  (char *)"pixel_acc_shiftX",
  (char *)"pixel_acc_shiftX_pulse",
  (char *)"pixel_acc_shiftY",
  (char *)"pixel_acc_shiftY_pulse",
  (char *)"pixel_image_acceleration",
  (char *)"pixel_mode",
  (char *)"pixel_radius",
  (char *)"pixel_radius_pulse",
  (char *)"BG_CA_repop_color_mode",
  (char *)"BG_CA_repop_density",
  (char *)"Part_repop_color_mode",
  (char *)"Part_repop_density",
  (char *)"repop_BG",
  (char *)"repop_BG_pulse",
  (char *)"repop_CA_pulse",
  (char *)"repop_colorBG",
  (char *)"repop_colorBG_pulse",
  (char *)"repop_colorCA",
  (char *)"repop_colorCA_pulse",
  (char *)"repop_colorPart",
  (char *)"repop_colorPart_pulse",
  (char *)"repop_greyBG",
  (char *)"repop_greyBG_pulse",
  (char *)"repop_greyCA",
  (char *)"repop_greyCA_pulse",
  (char *)"repop_greyPart",
  (char *)"repop_greyPart_pulse",
  (char *)"repop_part",
  (char *)"repop_part_pulse",
  (char *)"repop_path",
  (char *)"repop_path_pulse",
  (char *)"script_1",
  (char *)"sensor_activation",
  (char *)"sensor_layout",
  (char *)"sensor_sample_setUp",
  (char *)"sensor_vol",
  (char *)"auto_beat",
  (char *)"auto_beat_duration",
  (char *)"auto_pulse",
  (char *)"camera_BG_subtr",
  (char *)"clearAllLayers",
  (char *)"clearCA",
  (char *)"clearLayer",
  (char *)"cursorSize",
  (char *)"freeze",
  (char *)"invertAllLayers",
  (char *)"master",
  (char *)"mute_second_screen",
  (char *)"reset_camera",
  (char *)"take_snapshots",
  (char *)"CAdecay",
  (char *)"CAdecay_pulse",
  (char *)"echo",
  (char *)"echo_pulse",
  (char *)"echoNeg",
  (char *)"echoNeg_pulse",
  (char *)"partDecay",
  (char *)"partDecay_pulse",
  (char *)"trkDecay",
  (char *)"trkDecay_pulse",
  (char *)"CAMasterWeight",
  (char *)"CAMasterWeight_pulse",
  (char *)"CAMixingWeight",
  (char *)"CAMixingWeight_pulse",
  (char *)"ClipArtMixingWeight",
  (char *)"ClipArtMixingWeight_pulse",
  (char *)"ClipArtMasterWeight",
  (char *)"ClipArtMasterWeight_pulse",
  (char *)"PartMasterWeight",
  (char *)"PartMasterWeight_pulse",
  (char *)"PartMixingWeight",
  (char *)"PartMixingWeight_pulse",
  (char *)"SecondMasterMixingWeight",
  (char *)"trackMasterWeight",
  (char *)"trackMasterWeight_pulse",
  (char *)"trackMixingWeight",
  (char *)"trackMixingWeight_pulse",
  (char *)"blurRadius_1",
  (char *)"blurRadius_2",
  (char *)"currentDrawingTrack",
  (char *)"currentMaskTrack",
  (char *)"currentPhotoTrack",
  (char *)"currentVideoTrack",
  (char *)"slow_track_translation",
  (char *)"track_x_transl",
  (char *)"track_x_transl_pulse",
  (char *)"track_y_transl",
  (char *)"track_y_transl_pulse",
  (char *)"ClipArt_layer_color_preset",
  (char *)"ClipArt_layer_color_preset_pulse",
  (char *)"activeMeshes",
  (char *)"mobileMeshes",
  (char *)"isDisplayLookAt",
  (char *)"with_mesh",
  (char *)"with_blue",
  (char *)"with_whiteText",
  (char *)"VP1LocX",
  (char *)"VP1LocY",
  (char *)"VP1LocZ",
  (char *)"VP1UpY",
  (char *)"VP1Reversed",
  (char *)"VP1LookAtX",
  (char *)"VP1LookAtY",
  (char *)"VP1LookAtZ",
  (char *)"VP1WidthTopAt1m",
  (char *)"VP1WidthBottomAt1m",
  (char *)"VP1TopAt1m",
  (char *)"VP1BottomAt1m",
  (char *)"VP1KeystoneXTopLeft",
  (char *)"VP1KeystoneXBottomLeft",
  (char *)"VP1KeystoneYTopLeft",
  (char *)"VP1KeystoneYBottomLeft",
  (char *)"VP1KeystoneXTopRight",
  (char *)"VP1KeystoneXBottomRight",
  (char *)"VP1KeystoneYTopRight",
  (char *)"VP1KeystoneYBottomRight",
  (char *)"textureFrontier_wmin",
  (char *)"textureFrontier_wmax",
  (char *)"textureFrontier_hmin",
  (char *)"textureFrontier_hmax",
  (char *)"textureFrontier_wmin_width",
  (char *)"textureFrontier_wmax_width",
  (char *)"textureFrontier_hmin_width",
  (char *)"textureFrontier_hmax_width",
  (char *)"textureScale_w",
  (char *)"textureScale_h",
  (char *)"textureTranslate_w",
  (char *)"textureTranslate_h",
  (char *)"meshFrontier_X",
  (char *)"meshFrontier_width",
  (char *)"nearPlane",
  (char *)"farPlane",
  (char *)"augmentedReality",
  (char *)"meshRenderBypass",
};
void FullScenarioArrayVarInit() {
	CAParams[1] = float(0.393673);
	CAParams[2] = float(0.102449);
	CAParams[3] = float(0.53551);
	CAParams[4] = float(0.165306);
	CAParams[5] = float(0.196939);
	CAParams[6] = float(0.322857);
	CAParams[7] = float(0.307143);
	CAParams[8] = float(0);
	CAParams_pulse[1] = float(0);
	CAParams_pulse[2] = float(0);
	CAParams_pulse[3] = float(0);
	CAParams_pulse[4] = float(0);
	CAParams_pulse[5] = float(0);
	CAParams_pulse[6] = float(0);
	CAParams_pulse[7] = float(0);
	CAParams_pulse[8] = float(0);
	flashTrkBG_freq[1] = 0;
	flashTrkBG_freq[2] = 0;
	flashTrkBG_freq[3] = 0;
	flashTrkCA_freq[0] = 0;
	flashTrkCA_freq[1] = 0;
	flashTrkCA_freq[2] = 0;
	flashTrkCA_freq[3] = 0;
	flashTrkPart_freq[0] = 0;
	flashTrkPart_freq[1] = 0;
	flashTrkPart_freq[2] = 0;
	flashTrkPart_freq[3] = 0;
	master_mask_opacity[1] = float(1);
	master_mask_opacity[2] = float(1);
	master_mask_opacity[3] = float(1);
	master_mask_opacity[4] = float(1);
	master_mask_opacity[5] = float(1);
	master_mask_opacity[6] = float(1);
	part_path_follow[0] = 0;
	part_path_follow[1] = 0;
	part_path_follow[2] = 0;
	part_path_follow[3] = 0;
	part_path_follow[4] = 0;
	part_path_follow[5] = 0;
	part_path_follow[6] = 0;
	part_path_follow[7] = 0;
	part_path_follow[8] = 0;
	part_path_follow[9] = 0;
	part_path_follow[10] = 0;
	part_path_follow[11] = 0;
	part_path_repop[0] = 0;
	part_path_repop[1] = 0;
	part_path_repop[2] = 0;
	part_path_repop[3] = 0;
	part_path_repop[4] = 0;
	part_path_repop[5] = 0;
	part_path_repop[6] = 0;
	part_path_repop[7] = 0;
	part_path_repop[8] = 0;
	part_path_repop[9] = 0;
	part_path_repop[10] = 0;
	part_path_repop[11] = 0;
	part_path_repulse[0] = 0;
	part_path_repulse[1] = 0;
	part_path_repulse[2] = 0;
	part_path_repulse[3] = 0;
	part_path_repulse[4] = 0;
	part_path_repulse[5] = 0;
	part_path_repulse[6] = 0;
	part_path_repulse[7] = 0;
	part_path_repulse[8] = 0;
	part_path_repulse[9] = 0;
	part_path_repulse[10] = 0;
	part_path_repulse[11] = 0;
	path_record[1] = 0;
	path_record[2] = 0;
	path_record[3] = 0;
	path_record[4] = 0;
	path_record[5] = 0;
	path_record[6] = 0;
	path_record[7] = 0;
	path_record[8] = 0;
	path_record[9] = 0;
	path_record[10] = 0;
	path_record[11] = 0;
	path_replay_trackNo[1] = -1;
	path_replay_trackNo[2] = -1;
	path_replay_trackNo[3] = -1;
	path_replay_trackNo[4] = -1;
	path_replay_trackNo[5] = -1;
	path_replay_trackNo[6] = -1;
	path_replay_trackNo[7] = -1;
	path_replay_trackNo[8] = -1;
	path_replay_trackNo[9] = -1;
	path_replay_trackNo[10] = -1;
	path_replay_trackNo[11] = -1;
	trkDecay[0] = float(0.00273);
	trkDecay[1] = float(0.001);
	trkDecay[2] = float(0);
	trkDecay[3] = float(0);
	trkDecay_pulse[0] = float(0);
	trkDecay_pulse[1] = float(0);
	trkDecay_pulse[2] = float(0);
	trkDecay_pulse[3] = float(0);
	trackMasterWeight[0] = float(1);
	trackMasterWeight[1] = float(1);
	trackMasterWeight[2] = float(1);
	trackMasterWeight[3] = float(1);
	trackMasterWeight_pulse[0] = float(0);
	trackMasterWeight_pulse[1] = float(0);
	trackMasterWeight_pulse[2] = float(0);
	trackMasterWeight_pulse[3] = float(0);
	trackMixingWeight[0] = float(0);
	trackMixingWeight[1] = float(0);
	trackMixingWeight[2] = float(0);
	trackMixingWeight[3] = float(0);
	trackMixingWeight_pulse[0] = float(0);
	trackMixingWeight_pulse[1] = float(0);
	trackMixingWeight_pulse[2] = float(0);
	trackMixingWeight_pulse[3] = float(0);
	track_x_transl[0] = float(0);
	track_x_transl[1] = float(0);
	track_x_transl[2] = float(0);
	track_x_transl[3] = float(0);
	track_x_transl_pulse[0] = float(0);
	track_x_transl_pulse[1] = float(0);
	track_x_transl_pulse[2] = float(0);
	track_x_transl_pulse[3] = float(0);
	track_y_transl[0] = float(0);
	track_y_transl[1] = float(0);
	track_y_transl[2] = float(0);
	track_y_transl[3] = float(0);
	track_y_transl_pulse[0] = float(0);
	track_y_transl_pulse[1] = float(0);
	track_y_transl_pulse[2] = float(0);
	track_y_transl_pulse[3] = float(0);
	ClipArt_layer_color_preset[1] = float(-1);
	ClipArt_layer_color_preset[2] = float(-1);
	ClipArt_layer_color_preset[3] = float(-1);
	ClipArt_layer_color_preset[4] = float(-1);
	ClipArt_layer_color_preset[5] = float(-1);
	ClipArt_layer_color_preset[6] = float(-1);
	ClipArt_layer_color_preset[7] = float(-1);
	ClipArt_layer_color_preset[8] = float(-1);
	ClipArt_layer_color_preset_pulse[1] = float(0);
	ClipArt_layer_color_preset_pulse[2] = float(0);
	ClipArt_layer_color_preset_pulse[3] = float(0);
	ClipArt_layer_color_preset_pulse[4] = float(0);
	ClipArt_layer_color_preset_pulse[5] = float(0);
	ClipArt_layer_color_preset_pulse[6] = float(0);
	ClipArt_layer_color_preset_pulse[7] = float(0);
	ClipArt_layer_color_preset_pulse[8] = float(0);
}
