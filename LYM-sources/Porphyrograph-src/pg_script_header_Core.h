// CONFIGURATION CONSTANTS
// RANK,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26
// VERBATIM,double window,wide screen,window width,window height,window x,window y,minimal interframe latency,time scale,initial time,message pixel length,trace output frame number,trace time,first frame number,last frame number,max mouse recording frames (for track drawing reording),max network message length,nb of particles,nb types of CA,nb of tracks,nb of paths,projet name,nb of configs,nb CA parameters,nb Master Masks,nb ClipArt layers,nb lights
constexpr auto double_window                  = false     ;
constexpr auto wide_screen                    = false     ;
constexpr auto window_width                   = 1920      ;
constexpr auto window_height                  = 1080      ;
constexpr auto window_x                       = 0         ;
constexpr auto window_y                       = 0         ;
constexpr auto minimal_interframe_latency     = 0.01      ;
constexpr auto time_scale                     = 1         ;
constexpr auto initial_time                   = 0         ;
constexpr auto message_pixel_length           = 128       ;
constexpr auto trace_output_frame_number      = false     ;
constexpr auto trace_time                     = false     ;
constexpr auto first_frame_number             = 0         ;
constexpr auto last_frame_number              = 1000000   ;
constexpr auto max_mouse_recording_frames     = 16384     ;
constexpr auto max_network_message_length     = 65535     ;
constexpr auto nb_particles                   = 200000    ;
constexpr auto nb_CATypes                     = 7         ;
#define PG_NB_TRACKS                   4         
#define PG_NB_PATHS                    12        
constexpr auto project_name                   = "Song";
#define PG_MAX_CONFIGURATIONS          10        
#define PG_NB_CA_PARAMS                8         
#define PG_NB_MASTER_MASKS             6         
#define PG_NB_CLIPART_LAYERS           8         
#define PG_NB_LIGHTS                   8         
#define var_double_window
#define var_wide_screen
#define var_window_width
#define var_window_height
#define var_window_x
#define var_window_y
#define var_minimal_interframe_latency
#define var_time_scale
#define var_initial_time
#define var_message_pixel_length
#define var_trace_output_frame_number
#define var_trace_time
#define var_first_frame_number
#define var_last_frame_number
#define var_max_mouse_recording_frames
#define var_max_network_message_length
#define var_nb_particles
#define var_nb_CATypes
#define var_PG_NB_TRACKS
#define var_PG_NB_PATHS
#define var_project_name
#define var_PG_MAX_CONFIGURATIONS
#define var_PG_NB_CA_PARAMS
#define var_PG_NB_MASTER_MASKS
#define var_PG_NB_CLIPART_LAYERS
#define var_PG_NB_LIGHTS
enum ConfigurationVarIDs {
  _double_window = 0,
  _wide_screen,
  _window_width,
  _window_height,
  _window_x,
  _window_y,
  _minimal_interframe_latency,
  _time_scale,
  _initial_time,
  _message_pixel_length,
  _trace_output_frame_number,
  _trace_time,
  _first_frame_number,
  _last_frame_number,
  _max_mouse_recording_frames,
  _max_network_message_length,
  _nb_particles,
  _nb_CATypes,
  _PG_NB_TRACKS,
  _PG_NB_PATHS,
  _project_name,
  _PG_MAX_CONFIGURATIONS,
  _PG_NB_CA_PARAMS,
  _PG_NB_MASTER_MASKS,
  _PG_NB_CLIPART_LAYERS,
  _PG_NB_LIGHTS,
  _MaxConfigurationVarIDs};
// FULL SCENARIO VARIABLES
#define var_CA1_CA2_weight
#define var_CA1SubType
#define var_CA1Type
#define var_CA2SubType
#define var_CA2Type
#define var_CAcolorSpread
#define var_CATable
#define var_CAParams
#define var_CAParams_pulse
#define var_CAstep
#define var_repop_CA
#define var_flashCABG_freq
#define var_flashCameraTrkBeat
#define var_flashCameraTrkBright
#define var_flashCameraTrkLength
#define var_flashCAPart_freq
#define var_flashchange_invertAllLayers_freq
#define var_flashchange_diaporama_freq
#define var_flashchange_BGcolor_freq
#define var_flashPartBG_freq
#define var_flashPartCA_freq
#define var_flashParticleInit_freq
#define var_flashPhotoChangeBeat
#define var_flashPhotoTrkBeat
#define var_flashPhotoTrkBright
#define var_flashPhotoTrkLength
#define var_flashPixel_duration
#define var_flashPixel_freq
#define var_flashTrkBG_freq
#define var_flashTrkCA_freq
#define var_flashTrkPart_freq
#define var_master_crop_width
#define var_master_crop_x
#define var_master_crop_y
#define var_master_mask
#define var_master_mask_offsetX
#define var_master_mask_offsetY
#define var_master_mask_opacity
#define var_master_mask_scale
#define var_master_mask_scale_ratio
#define var_master_offsetX
#define var_master_offsetY
#define var_master_scale
#define var_master_scale_pulse
#define var_master_scale_ratio
#define var_audioInput_weight
#define var_beat_delay
#define var_beat_threshold
#define var_playing_soundtrackNo
#define var_sound_env_max
#define var_sound_env_min
#define var_sound_min
#define var_sound_volume
#define var_soundtrack_PA_weight
#define var_soundtrack_PD_weight
#define var_camera_gamma
#define var_camera_gamma_pulse
#define var_cameraBrightness
#define var_cameraCaptFreq
#define var_cameraContrast
#define var_cameraCumul
#define var_cameraExposure
#define var_cameraGain
#define var_cameraGamma
#define var_cameraNo
#define var_cameraSaturation
#define var_cameraSobel
#define var_cameraSobel_pulse
#define var_cameraThreshold
#define var_cameraThreshold_pulse
#define var_cameraWB_B
#define var_cameraWB_R
#define var_cameraWeight
#define var_cameraWeight_pulse
#define var_invertCamera
#define var_clip_mix
#define var_clip_nudge_factor
#define var_clip_scratch_factor
#define var_clipCaptFreq
#define var_playing_clipNameLeft
#define var_playing_clipNameRight
#define var_playing_secondClipNameLeft
#define var_playing_secondClipNameRight
#define var_activeClipArts
#define var_moving_messages
#define var_ClipArt_width
#define var_ClipArt_width_pulse
#define var_ClipArt_stroke_color_preset
#define var_ClipArt_stroke_color_preset_pulse
#define var_invertMovie
#define var_movie_gamma
#define var_movie_gamma_pulse
#define var_movie_loop
#define var_movie_threshold
#define var_movie_threshold_pulse
#define var_movieCaptFreq
#define var_movieSobel
#define var_movieSobel_pulse
#define var_movieWeight
#define var_movieWeight_pulse
#define var_playing_movieNo
#define var_invertPhoto
#define var_maskJitterAmpl
#define var_photo_contrast
#define var_photo_diaporama
#define var_photo_diaporama_fade
#define var_photo_diaporama_plateau
#define var_photo_gamma
#define var_photo_gamma_pulse
#define var_photo_hue
#define var_photo_hue_pulse
#define var_photo_homography
#define var_photo_offsetX
#define var_photo_offsetY
#define var_photo_rot
#define var_photo_satur
#define var_photo_satur_pulse
#define var_photo_scaleX
#define var_photo_scaleY
#define var_photo_threshold
#define var_photo_threshold_pulse
#define var_photo_transl_x
#define var_photo_transl_y
#define var_photo_value
#define var_photo_value_pulse
#define var_photoJitterAmpl
#define var_photoSobel
#define var_photoSobel_pulse
#define var_photoWeight
#define var_photoWeight_pulse
#define var_video_contrast
#define var_video_contrast_pulse
#define var_video_gamma
#define var_video_gamma_pulse
#define var_video_hue
#define var_video_hue_pulse
#define var_video_satur
#define var_video_satur_pulse
#define var_video_threshold
#define var_video_threshold_pulse
#define var_video_value
#define var_video_value_pulse
#define var_video_white
#define var_video_white_pulse
#define var_noiseType
#define var_noiseLineScale
#define var_noiseAngleScale
#define var_noiseCenterX
#define var_noiseCenterY
#define var_noiseParticleScale
#define var_noiseParticleScale_pulse
#define var_part_acc
#define var_part_acc_pulse
#define var_part_damp
#define var_part_damp_pulse
#define var_part_damp_targtRad
#define var_part_field_weight
#define var_part_field_weight_pulse
#define var_part_gravity
#define var_part_gravity_pulse
#define var_part_image_acceleration
#define var_part_initialization
#define var_part_size
#define var_part_size_pulse
#define var_part_timeToTargt
#define var_partColor_mode
#define var_partExit_mode
#define var_particle_type
#define var_partMove_rand
#define var_partMove_target
#define var_partSplat_texture
#define var_partStroke_mode
#define var_pulsed_part_Hshift
#define var_pulsed_part_Vshift
#define var_tracksSync
#define var_part_path_follow
#define var_part_path_repop
#define var_part_path_repulse
#define var_path_group
#define var_path_record
#define var_path_replay_loop
#define var_path_replay_speed
#define var_path_replay_trackNo
#define var_path_scaleX
#define var_path_scaleY
#define var_path_translX
#define var_path_translY
#define var_color_spectrum_coef
#define var_fingers
#define var_tactile_tablet
#define var_pen_angle_pulse
#define var_pen_brush
#define var_pen_brush_replay
#define var_pen_color
#define var_pen_color_pulse
#define var_pen_color_a
#define var_pen_color_a_pulse
#define var_pen_hue
#define var_pen_hue_pulse
#define var_pen_sat
#define var_pen_sat_pulse
#define var_pen_value
#define var_pen_value_pulse
#define var_pen_hsv
#define var_pen_color_replay
#define var_pen_color_replay_pulse
#define var_pen_grey
#define var_pen_grey_pulse
#define var_pen_grey_replay
#define var_pen_grey_replay_pulse
#define var_pen_hue_replay
#define var_pen_hue_replay_pulse
#define var_pen_position_dash
#define var_pen_position_pulse
#define var_pen_radius
#define var_pen_radius_angleHor_coef
#define var_pen_radius_angleVer_coef
#define var_pen_radius_pressure_coef
#define var_pen_radius_pulse
#define var_pen_radius_replay
#define var_pen_radius_replay_pulse
#define var_pen_saturation_replay
#define var_pen_saturation_replay_pulse
#define var_pen_value_replay
#define var_pen_value_replay_pulse
#define var_penStrokeAtBeat
#define var_Pixelstep
#define var_noiseUpdateScale
#define var_noiseUpdateScale_pulse
#define var_pixel_acc
#define var_pixel_acc_pulse
#define var_pixel_acc_shiftX
#define var_pixel_acc_shiftX_pulse
#define var_pixel_acc_shiftY
#define var_pixel_acc_shiftY_pulse
#define var_pixel_image_acceleration
#define var_pixel_mode
#define var_pixel_radius
#define var_pixel_radius_pulse
#define var_BG_CA_repop_color_mode
#define var_BG_CA_repop_density
#define var_Part_repop_color_mode
#define var_Part_repop_density
#define var_repop_BG
#define var_repop_BG_pulse
#define var_repop_CA_pulse
#define var_repop_colorBG
#define var_repop_colorBG_pulse
#define var_repop_colorCA
#define var_repop_colorCA_pulse
#define var_repop_colorPart
#define var_repop_colorPart_pulse
#define var_repop_greyBG
#define var_repop_greyBG_pulse
#define var_repop_greyCA
#define var_repop_greyCA_pulse
#define var_repop_greyPart
#define var_repop_greyPart_pulse
#define var_repop_huePart
#define var_repop_huePart_pulse
#define var_repop_satPart
#define var_repop_satPart_pulse
#define var_repop_valuePart
#define var_repop_valuePart_pulse
#define var_repop_hsvPart
#define var_repop_part
#define var_repop_part_pulse
#define var_repop_path
#define var_repop_path_pulse
#define var_script_1
#define var_pg_metawear
#define var_sensor_activation
#define var_sensor_layout
#define var_sensor_sample_setUp
#define var_sensor_vol
#define var_auto_beat
#define var_auto_beat_duration
#define var_auto_pulse
#define var_camera_BG_subtr
#define var_clearAllLayers
#define var_clearCA
#define var_clearLayer
#define var_cursorSize
#define var_freeze
#define var_invertAllLayers
#define var_master
#define var_mute_second_screen
#define var_reset_camera
#define var_take_snapshots
#define var_CAdecay
#define var_CAdecay_pulse
#define var_echo
#define var_echo_pulse
#define var_echoNeg
#define var_echoNeg_pulse
#define var_partDecay
#define var_partDecay_pulse
#define var_trkDecay
#define var_trkDecay_pulse
#define var_CAMasterWeight
#define var_CAMasterWeight_pulse
#define var_CAMixingWeight
#define var_CAMixingWeight_pulse
#define var_ClipArtMixingWeight
#define var_ClipArtMixingWeight_pulse
#define var_ClipArtMasterWeight
#define var_ClipArtMasterWeight_pulse
#define var_PartMasterWeight
#define var_PartMasterWeight_pulse
#define var_PartMixingWeight
#define var_PartMixingWeight_pulse
#define var_SecondMasterMixingWeight
#define var_trackMasterWeight
#define var_trackMasterWeight_pulse
#define var_trackMixingWeight
#define var_trackMixingWeight_pulse
#define var_blurRadius_1
#define var_blurRadius_2
#define var_currentDrawingTrack
#define var_currentMaskTrack
#define var_currentPhotoTrack
#define var_currentVideoTrack
#define var_slow_track_translation
#define var_track_x_transl
#define var_track_x_transl_pulse
#define var_track_y_transl
#define var_track_y_transl_pulse
#define var_ClipArt_layer_color_preset
#define var_ClipArt_layer_color_preset_pulse
#define var_activeMeshes
#define var_mobileMeshes
#define var_mesh_homography
#define var_isDisplayLookAt
#define var_with_mesh
#define var_with_blue
#define var_with_whiteText
#define var_VP1LocX
#define var_VP1LocY
#define var_VP1LocZ
#define var_VP1UpY
#define var_VP1Reversed
#define var_VP1LookAtX
#define var_VP1LookAtY
#define var_VP1LookAtZ
#define var_VP1WidthTopAt1m
#define var_VP1WidthBottomAt1m
#define var_VP1TopAt1m
#define var_VP1BottomAt1m
#define var_VP1KeystoneXTopLeft
#define var_VP1KeystoneXBottomLeft
#define var_VP1KeystoneYTopLeft
#define var_VP1KeystoneYBottomLeft
#define var_VP1KeystoneXTopRight
#define var_VP1KeystoneXBottomRight
#define var_VP1KeystoneYTopRight
#define var_VP1KeystoneYBottomRight
#define var_textureFrontier_wmin
#define var_textureFrontier_wmax
#define var_textureFrontier_hmin
#define var_textureFrontier_hmax
#define var_textureFrontier_wmin_width
#define var_textureFrontier_wmax_width
#define var_textureFrontier_hmin_width
#define var_textureFrontier_hmax_width
#define var_textureScale_w
#define var_textureScale_h
#define var_textureTranslate_w
#define var_textureTranslate_h
#define var_meshFrontier_X
#define var_meshFrontier_width
#define var_nearPlane
#define var_farPlane
#define var_augmentedReality
#define var_meshRenderBypass
#define var_MIDIwithBeat
#define var_MIDIwithColor
#define var_MIDIwithBrush
#define var_MIDIwithCameraFlash
#define var_MIDIwithPhotoFlash
#define var_MIDIwithStroke
#define var_MIDIwithDiaporama
#define var_light_color
#define var_light_color_pulse
#define var_light_grey
#define var_light_grey_pulse
#define var_light_dimmer
#define var_light_dimmer_pulse
#define var_light_strobe
#define var_light_strobe_pulse
enum InterpVarIDs{ 
  _CA1_CA2_weight = 0,
  _CA1SubType,
  _CA1Type,
  _CA2SubType,
  _CA2Type,
  _CAcolorSpread,
  _CATable,
  _CAParams,
  _CAParams_pulse,
  _CAstep,
  _repop_CA,
  _flashCABG_freq,
  _flashCameraTrkBeat,
  _flashCameraTrkBright,
  _flashCameraTrkLength,
  _flashCAPart_freq,
  _flashchange_invertAllLayers_freq,
  _flashchange_diaporama_freq,
  _flashchange_BGcolor_freq,
  _flashPartBG_freq,
  _flashPartCA_freq,
  _flashParticleInit_freq,
  _flashPhotoChangeBeat,
  _flashPhotoTrkBeat,
  _flashPhotoTrkBright,
  _flashPhotoTrkLength,
  _flashPixel_duration,
  _flashPixel_freq,
  _flashTrkBG_freq,
  _flashTrkCA_freq,
  _flashTrkPart_freq,
  _master_crop_width,
  _master_crop_x,
  _master_crop_y,
  _master_mask,
  _master_mask_offsetX,
  _master_mask_offsetY,
  _master_mask_opacity,
  _master_mask_scale,
  _master_mask_scale_ratio,
  _master_offsetX,
  _master_offsetY,
  _master_scale,
  _master_scale_pulse,
  _master_scale_ratio,
  _audioInput_weight,
  _beat_delay,
  _beat_threshold,
  _playing_soundtrackNo,
  _sound_env_max,
  _sound_env_min,
  _sound_min,
  _sound_volume,
  _soundtrack_PA_weight,
  _soundtrack_PD_weight,
  _camera_gamma,
  _camera_gamma_pulse,
  _cameraBrightness,
  _cameraCaptFreq,
  _cameraContrast,
  _cameraCumul,
  _cameraExposure,
  _cameraGain,
  _cameraGamma,
  _cameraNo,
  _cameraSaturation,
  _cameraSobel,
  _cameraSobel_pulse,
  _cameraThreshold,
  _cameraThreshold_pulse,
  _cameraWB_B,
  _cameraWB_R,
  _cameraWeight,
  _cameraWeight_pulse,
  _invertCamera,
  _clip_mix,
  _clip_nudge_factor,
  _clip_scratch_factor,
  _clipCaptFreq,
  _playing_clipNameLeft,
  _playing_clipNameRight,
  _playing_secondClipNameLeft,
  _playing_secondClipNameRight,
  _activeClipArts,
  _moving_messages,
  _ClipArt_width,
  _ClipArt_width_pulse,
  _ClipArt_stroke_color_preset,
  _ClipArt_stroke_color_preset_pulse,
  _invertMovie,
  _movie_gamma,
  _movie_gamma_pulse,
  _movie_loop,
  _movie_threshold,
  _movie_threshold_pulse,
  _movieCaptFreq,
  _movieSobel,
  _movieSobel_pulse,
  _movieWeight,
  _movieWeight_pulse,
  _playing_movieNo,
  _invertPhoto,
  _maskJitterAmpl,
  _photo_contrast,
  _photo_diaporama,
  _photo_diaporama_fade,
  _photo_diaporama_plateau,
  _photo_gamma,
  _photo_gamma_pulse,
  _photo_hue,
  _photo_hue_pulse,
  _photo_homography,
  _photo_offsetX,
  _photo_offsetY,
  _photo_rot,
  _photo_satur,
  _photo_satur_pulse,
  _photo_scaleX,
  _photo_scaleY,
  _photo_threshold,
  _photo_threshold_pulse,
  _photo_transl_x,
  _photo_transl_y,
  _photo_value,
  _photo_value_pulse,
  _photoJitterAmpl,
  _photoSobel,
  _photoSobel_pulse,
  _photoWeight,
  _photoWeight_pulse,
  _video_contrast,
  _video_contrast_pulse,
  _video_gamma,
  _video_gamma_pulse,
  _video_hue,
  _video_hue_pulse,
  _video_satur,
  _video_satur_pulse,
  _video_threshold,
  _video_threshold_pulse,
  _video_value,
  _video_value_pulse,
  _video_white,
  _video_white_pulse,
  _noiseType,
  _noiseLineScale,
  _noiseAngleScale,
  _noiseCenterX,
  _noiseCenterY,
  _noiseParticleScale,
  _noiseParticleScale_pulse,
  _part_acc,
  _part_acc_pulse,
  _part_damp,
  _part_damp_pulse,
  _part_damp_targtRad,
  _part_field_weight,
  _part_field_weight_pulse,
  _part_gravity,
  _part_gravity_pulse,
  _part_image_acceleration,
  _part_initialization,
  _part_size,
  _part_size_pulse,
  _part_timeToTargt,
  _partColor_mode,
  _partExit_mode,
  _particle_type,
  _partMove_rand,
  _partMove_target,
  _partSplat_texture,
  _partStroke_mode,
  _pulsed_part_Hshift,
  _pulsed_part_Vshift,
  _tracksSync,
  _part_path_follow,
  _part_path_repop,
  _part_path_repulse,
  _path_group,
  _path_record,
  _path_replay_loop,
  _path_replay_speed,
  _path_replay_trackNo,
  _path_scaleX,
  _path_scaleY,
  _path_translX,
  _path_translY,
  _color_spectrum_coef,
  _fingers,
  _tactile_tablet,
  _pen_angle_pulse,
  _pen_brush,
  _pen_brush_replay,
  _pen_color,
  _pen_color_pulse,
  _pen_color_a,
  _pen_color_a_pulse,
  _pen_hue,
  _pen_hue_pulse,
  _pen_sat,
  _pen_sat_pulse,
  _pen_value,
  _pen_value_pulse,
  _pen_hsv,
  _pen_color_replay,
  _pen_color_replay_pulse,
  _pen_grey,
  _pen_grey_pulse,
  _pen_grey_replay,
  _pen_grey_replay_pulse,
  _pen_hue_replay,
  _pen_hue_replay_pulse,
  _pen_position_dash,
  _pen_position_pulse,
  _pen_radius,
  _pen_radius_angleHor_coef,
  _pen_radius_angleVer_coef,
  _pen_radius_pressure_coef,
  _pen_radius_pulse,
  _pen_radius_replay,
  _pen_radius_replay_pulse,
  _pen_saturation_replay,
  _pen_saturation_replay_pulse,
  _pen_value_replay,
  _pen_value_replay_pulse,
  _penStrokeAtBeat,
  _Pixelstep,
  _noiseUpdateScale,
  _noiseUpdateScale_pulse,
  _pixel_acc,
  _pixel_acc_pulse,
  _pixel_acc_shiftX,
  _pixel_acc_shiftX_pulse,
  _pixel_acc_shiftY,
  _pixel_acc_shiftY_pulse,
  _pixel_image_acceleration,
  _pixel_mode,
  _pixel_radius,
  _pixel_radius_pulse,
  _BG_CA_repop_color_mode,
  _BG_CA_repop_density,
  _Part_repop_color_mode,
  _Part_repop_density,
  _repop_BG,
  _repop_BG_pulse,
  _repop_CA_pulse,
  _repop_colorBG,
  _repop_colorBG_pulse,
  _repop_colorCA,
  _repop_colorCA_pulse,
  _repop_colorPart,
  _repop_colorPart_pulse,
  _repop_greyBG,
  _repop_greyBG_pulse,
  _repop_greyCA,
  _repop_greyCA_pulse,
  _repop_greyPart,
  _repop_greyPart_pulse,
  _repop_huePart,
  _repop_huePart_pulse,
  _repop_satPart,
  _repop_satPart_pulse,
  _repop_valuePart,
  _repop_valuePart_pulse,
  _repop_hsvPart,
  _repop_part,
  _repop_part_pulse,
  _repop_path,
  _repop_path_pulse,
  _script_1,
  _pg_metawear,
  _sensor_activation,
  _sensor_layout,
  _sensor_sample_setUp,
  _sensor_vol,
  _auto_beat,
  _auto_beat_duration,
  _auto_pulse,
  _camera_BG_subtr,
  _clearAllLayers,
  _clearCA,
  _clearLayer,
  _cursorSize,
  _freeze,
  _invertAllLayers,
  _master,
  _mute_second_screen,
  _reset_camera,
  _take_snapshots,
  _CAdecay,
  _CAdecay_pulse,
  _echo,
  _echo_pulse,
  _echoNeg,
  _echoNeg_pulse,
  _partDecay,
  _partDecay_pulse,
  _trkDecay,
  _trkDecay_pulse,
  _CAMasterWeight,
  _CAMasterWeight_pulse,
  _CAMixingWeight,
  _CAMixingWeight_pulse,
  _ClipArtMixingWeight,
  _ClipArtMixingWeight_pulse,
  _ClipArtMasterWeight,
  _ClipArtMasterWeight_pulse,
  _PartMasterWeight,
  _PartMasterWeight_pulse,
  _PartMixingWeight,
  _PartMixingWeight_pulse,
  _SecondMasterMixingWeight,
  _trackMasterWeight,
  _trackMasterWeight_pulse,
  _trackMixingWeight,
  _trackMixingWeight_pulse,
  _blurRadius_1,
  _blurRadius_2,
  _currentDrawingTrack,
  _currentMaskTrack,
  _currentPhotoTrack,
  _currentVideoTrack,
  _slow_track_translation,
  _track_x_transl,
  _track_x_transl_pulse,
  _track_y_transl,
  _track_y_transl_pulse,
  _ClipArt_layer_color_preset,
  _ClipArt_layer_color_preset_pulse,
  _activeMeshes,
  _mobileMeshes,
  _mesh_homography,
  _isDisplayLookAt,
  _with_mesh,
  _with_blue,
  _with_whiteText,
  _VP1LocX,
  _VP1LocY,
  _VP1LocZ,
  _VP1UpY,
  _VP1Reversed,
  _VP1LookAtX,
  _VP1LookAtY,
  _VP1LookAtZ,
  _VP1WidthTopAt1m,
  _VP1WidthBottomAt1m,
  _VP1TopAt1m,
  _VP1BottomAt1m,
  _VP1KeystoneXTopLeft,
  _VP1KeystoneXBottomLeft,
  _VP1KeystoneYTopLeft,
  _VP1KeystoneYBottomLeft,
  _VP1KeystoneXTopRight,
  _VP1KeystoneXBottomRight,
  _VP1KeystoneYTopRight,
  _VP1KeystoneYBottomRight,
  _textureFrontier_wmin,
  _textureFrontier_wmax,
  _textureFrontier_hmin,
  _textureFrontier_hmax,
  _textureFrontier_wmin_width,
  _textureFrontier_wmax_width,
  _textureFrontier_hmin_width,
  _textureFrontier_hmax_width,
  _textureScale_w,
  _textureScale_h,
  _textureTranslate_w,
  _textureTranslate_h,
  _meshFrontier_X,
  _meshFrontier_width,
  _nearPlane,
  _farPlane,
  _augmentedReality,
  _meshRenderBypass,
  _MIDIwithBeat,
  _MIDIwithColor,
  _MIDIwithBrush,
  _MIDIwithCameraFlash,
  _MIDIwithPhotoFlash,
  _MIDIwithStroke,
  _MIDIwithDiaporama,
  _light_color,
  _light_color_pulse,
  _light_grey,
  _light_grey_pulse,
  _light_dimmer,
  _light_dimmer_pulse,
  _light_strobe,
  _light_strobe_pulse,
  _MaxInterpVarIDs};
extern float CA1_CA2_weight      ;
extern int   CA1SubType          ;
extern int   CA1Type             ;
extern int   CA2SubType          ;
extern int   CA2Type             ;
extern bool  CAcolorSpread       ;
extern bool  CATable             ;
extern float CAParams            [(PG_NB_CA_PARAMS+1)];
extern float CAParams_pulse      [(PG_NB_CA_PARAMS+1)];
extern int   CAstep              ;
extern float repop_CA            ;
extern int   flashCABG_freq      ;
extern int   flashCameraTrkBeat  ;
extern float flashCameraTrkBright;
extern float flashCameraTrkLength;
extern int   flashCAPart_freq    ;
extern int   flashchange_invertAllLayers_freq;
extern int   flashchange_diaporama_freq;
extern int   flashchange_BGcolor_freq;
extern int   flashPartBG_freq    ;
extern int   flashPartCA_freq    ;
extern int   flashParticleInit_freq;
extern int   flashPhotoChangeBeat;
extern int   flashPhotoTrkBeat   ;
extern float flashPhotoTrkBright ;
extern float flashPhotoTrkLength ;
extern int   flashPixel_duration ;
extern int   flashPixel_freq     ;
extern int   flashTrkBG_freq     [PG_NB_TRACKS];
extern int   flashTrkCA_freq     [PG_NB_TRACKS];
extern int   flashTrkPart_freq   [PG_NB_TRACKS];
extern float master_crop_width   ;
extern float master_crop_x       ;
extern float master_crop_y       ;
extern float master_mask         ;
extern float master_mask_offsetX ;
extern float master_mask_offsetY ;
extern float master_mask_opacity [(PG_NB_MASTER_MASKS+1)];
extern float master_mask_scale   ;
extern float master_mask_scale_ratio;
extern float master_offsetX      ;
extern float master_offsetY      ;
extern float master_scale        ;
extern float master_scale_pulse  ;
extern float master_scale_ratio  ;
extern float audioInput_weight   ;
extern float beat_delay          ;
extern float beat_threshold      ;
extern int   playing_soundtrackNo;
extern float sound_env_max       ;
extern float sound_env_min       ;
extern float sound_min           ;
extern float sound_volume        ;
extern float soundtrack_PA_weight;
extern float soundtrack_PD_weight;
extern float camera_gamma        ;
extern float camera_gamma_pulse  ;
extern float cameraBrightness    ;
extern float cameraCaptFreq      ;
extern float cameraContrast      ;
extern int   cameraCumul         ;
extern float cameraExposure      ;
extern float cameraGain          ;
extern float cameraGamma         ;
extern int   cameraNo            ;
extern float cameraSaturation    ;
extern float cameraSobel         ;
extern float cameraSobel_pulse   ;
extern float cameraThreshold     ;
extern float cameraThreshold_pulse;
extern float cameraWB_B          ;
extern float cameraWB_R          ;
extern float cameraWeight        ;
extern float cameraWeight_pulse  ;
extern bool  invertCamera        ;
extern float clip_mix            ;
extern float clip_nudge_factor   ;
extern float clip_scratch_factor ;
extern float clipCaptFreq        ;
extern string playing_clipNameLeft;
extern string playing_clipNameRight;
extern string playing_secondClipNameLeft;
extern string playing_secondClipNameRight;
extern int   activeClipArts      ;
extern float moving_messages     ;
extern float ClipArt_width       ;
extern float ClipArt_width_pulse ;
extern float ClipArt_stroke_color_preset;
extern float ClipArt_stroke_color_preset_pulse;
extern bool  invertMovie         ;
extern float movie_gamma         ;
extern float movie_gamma_pulse   ;
extern bool  movie_loop          ;
extern float movie_threshold     ;
extern float movie_threshold_pulse;
extern float movieCaptFreq       ;
extern float movieSobel          ;
extern float movieSobel_pulse    ;
extern float movieWeight         ;
extern float movieWeight_pulse   ;
extern int   playing_movieNo     ;
extern bool  invertPhoto         ;
extern float maskJitterAmpl      ;
extern float photo_contrast      ;
extern int   photo_diaporama     ;
extern float photo_diaporama_fade;
extern float photo_diaporama_plateau;
extern float photo_gamma         ;
extern float photo_gamma_pulse   ;
extern float photo_hue           ;
extern float photo_hue_pulse     ;
extern bool  photo_homography    ;
extern float photo_offsetX       ;
extern float photo_offsetY       ;
extern float photo_rot           ;
extern float photo_satur         ;
extern float photo_satur_pulse   ;
extern float photo_scaleX        ;
extern float photo_scaleY        ;
extern float photo_threshold     ;
extern float photo_threshold_pulse;
extern float photo_transl_x      ;
extern float photo_transl_y      ;
extern float photo_value         ;
extern float photo_value_pulse   ;
extern float photoJitterAmpl     ;
extern float photoSobel          ;
extern float photoSobel_pulse    ;
extern float photoWeight         ;
extern float photoWeight_pulse   ;
extern float video_contrast      ;
extern float video_contrast_pulse;
extern float video_gamma         ;
extern float video_gamma_pulse   ;
extern float video_hue           ;
extern float video_hue_pulse     ;
extern float video_satur         ;
extern float video_satur_pulse   ;
extern float video_threshold     ;
extern float video_threshold_pulse;
extern float video_value         ;
extern float video_value_pulse   ;
extern float video_white         ;
extern float video_white_pulse   ;
extern int   noiseType           ;
extern float noiseLineScale      ;
extern float noiseAngleScale     ;
extern float noiseCenterX        ;
extern float noiseCenterY        ;
extern float noiseParticleScale  ;
extern float noiseParticleScale_pulse;
extern float part_acc            ;
extern float part_acc_pulse      ;
extern float part_damp           ;
extern float part_damp_pulse     ;
extern float part_damp_targtRad  ;
extern float part_field_weight   ;
extern float part_field_weight_pulse;
extern float part_gravity        ;
extern float part_gravity_pulse  ;
extern int   part_image_acceleration;
extern int   part_initialization ;
extern float part_size           ;
extern float part_size_pulse     ;
extern float part_timeToTargt    ;
extern int   partColor_mode      ;
extern int   partExit_mode       ;
extern int   particle_type       ;
extern bool  partMove_rand       ;
extern bool  partMove_target     ;
extern int   partSplat_texture   ;
extern int   partStroke_mode     ;
extern float pulsed_part_Hshift  ;
extern float pulsed_part_Vshift  ;
extern bool  tracksSync          ;
extern bool  part_path_follow    [PG_NB_PATHS];
extern bool  part_path_repop     [PG_NB_PATHS];
extern bool  part_path_repulse   [PG_NB_PATHS];
extern int   path_group          ;
extern bool  path_record         [PG_NB_PATHS];
extern bool  path_replay_loop    ;
extern float path_replay_speed   ;
extern int   path_replay_trackNo [PG_NB_PATHS];
extern float path_scaleX         ;
extern float path_scaleY         ;
extern float path_translX        ;
extern float path_translY        ;
extern float color_spectrum_coef ;
extern int   fingers             ;
extern bool  tactile_tablet      ;
extern float pen_angle_pulse     ;
extern int   pen_brush           ;
extern int   pen_brush_replay    ;
extern float pen_color           ;
extern float pen_color_pulse     ;
extern float pen_color_a         ;
extern float pen_color_a_pulse   ;
extern float pen_hue             ;
extern float pen_hue_pulse       ;
extern float pen_sat             ;
extern float pen_sat_pulse       ;
extern float pen_value           ;
extern float pen_value_pulse     ;
extern bool  pen_hsv             ;
extern float pen_color_replay    ;
extern float pen_color_replay_pulse;
extern float pen_grey            ;
extern float pen_grey_pulse      ;
extern float pen_grey_replay     ;
extern float pen_grey_replay_pulse;
extern float pen_hue_replay      ;
extern float pen_hue_replay_pulse;
extern int   pen_position_dash   ;
extern float pen_position_pulse  ;
extern float pen_radius          ;
extern float pen_radius_angleHor_coef;
extern float pen_radius_angleVer_coef;
extern float pen_radius_pressure_coef;
extern float pen_radius_pulse    ;
extern float pen_radius_replay   ;
extern float pen_radius_replay_pulse;
extern float pen_saturation_replay;
extern float pen_saturation_replay_pulse;
extern float pen_value_replay    ;
extern float pen_value_replay_pulse;
extern bool  penStrokeAtBeat     ;
extern int   Pixelstep           ;
extern float noiseUpdateScale    ;
extern float noiseUpdateScale_pulse;
extern float pixel_acc           ;
extern float pixel_acc_pulse     ;
extern float pixel_acc_shiftX    ;
extern float pixel_acc_shiftX_pulse;
extern float pixel_acc_shiftY    ;
extern float pixel_acc_shiftY_pulse;
extern int   pixel_image_acceleration;
extern int   pixel_mode          ;
extern float pixel_radius        ;
extern float pixel_radius_pulse  ;
extern int   BG_CA_repop_color_mode;
extern int   BG_CA_repop_density ;
extern int   Part_repop_color_mode;
extern int   Part_repop_density  ;
extern float repop_BG            ;
extern float repop_BG_pulse      ;
extern float repop_CA_pulse      ;
extern float repop_colorBG       ;
extern float repop_colorBG_pulse ;
extern float repop_colorCA       ;
extern float repop_colorCA_pulse ;
extern float repop_colorPart     ;
extern float repop_colorPart_pulse;
extern float repop_greyBG        ;
extern float repop_greyBG_pulse  ;
extern float repop_greyCA        ;
extern float repop_greyCA_pulse  ;
extern float repop_greyPart      ;
extern float repop_greyPart_pulse;
extern float repop_huePart       ;
extern float repop_huePart_pulse ;
extern float repop_satPart       ;
extern float repop_satPart_pulse ;
extern float repop_valuePart     ;
extern float repop_valuePart_pulse;
extern bool  repop_hsvPart       ;
extern float repop_part          ;
extern float repop_part_pulse    ;
extern float repop_path          ;
extern float repop_path_pulse    ;
extern string script_1            ;
extern bool  pg_metawear         ;
extern int   sensor_activation   ;
extern int   sensor_layout       ;
extern float sensor_sample_setUp ;
extern float sensor_vol          ;
extern bool  auto_beat           ;
extern float auto_beat_duration  ;
extern bool  auto_pulse          ;
extern bool  camera_BG_subtr     ;
extern bool  clearAllLayers      ;
extern bool  clearCA             ;
extern bool  clearLayer          ;
extern int   cursorSize          ;
extern bool  freeze              ;
extern bool  invertAllLayers     ;
extern float master              ;
extern bool  mute_second_screen  ;
extern bool  reset_camera        ;
extern bool  take_snapshots      ;
extern float CAdecay             ;
extern float CAdecay_pulse       ;
extern float echo                ;
extern float echo_pulse          ;
extern float echoNeg             ;
extern float echoNeg_pulse       ;
extern float partDecay           ;
extern float partDecay_pulse     ;
extern float trkDecay            [PG_NB_TRACKS];
extern float trkDecay_pulse      [PG_NB_TRACKS];
extern float CAMasterWeight      ;
extern float CAMasterWeight_pulse;
extern float CAMixingWeight      ;
extern float CAMixingWeight_pulse;
extern float ClipArtMixingWeight ;
extern float ClipArtMixingWeight_pulse;
extern float ClipArtMasterWeight ;
extern float ClipArtMasterWeight_pulse;
extern float PartMasterWeight    ;
extern float PartMasterWeight_pulse;
extern float PartMixingWeight    ;
extern float PartMixingWeight_pulse;
extern float SecondMasterMixingWeight;
extern float trackMasterWeight   [PG_NB_TRACKS];
extern float trackMasterWeight_pulse[PG_NB_TRACKS];
extern float trackMixingWeight   [PG_NB_TRACKS];
extern float trackMixingWeight_pulse[PG_NB_TRACKS];
extern float blurRadius_1        ;
extern float blurRadius_2        ;
extern int   currentDrawingTrack ;
extern int   currentMaskTrack    ;
extern int   currentPhotoTrack   ;
extern int   currentVideoTrack   ;
extern bool  slow_track_translation;
extern float track_x_transl      [PG_NB_TRACKS];
extern float track_x_transl_pulse[PG_NB_TRACKS];
extern float track_y_transl      [PG_NB_TRACKS];
extern float track_y_transl_pulse[PG_NB_TRACKS];
extern float ClipArt_layer_color_preset[(PG_NB_CLIPART_LAYERS+1)];
extern float ClipArt_layer_color_preset_pulse[(PG_NB_CLIPART_LAYERS+1)];
extern int   activeMeshes        ;
extern int   mobileMeshes        ;
extern bool  mesh_homography     ;
extern float isDisplayLookAt     ;
extern float with_mesh           ;
extern float with_blue           ;
extern float with_whiteText      ;
extern float VP1LocX             ;
extern float VP1LocY             ;
extern float VP1LocZ             ;
extern float VP1UpY              ;
extern float VP1Reversed         ;
extern float VP1LookAtX          ;
extern float VP1LookAtY          ;
extern float VP1LookAtZ          ;
extern float VP1WidthTopAt1m     ;
extern float VP1WidthBottomAt1m  ;
extern float VP1TopAt1m          ;
extern float VP1BottomAt1m       ;
extern float VP1KeystoneXTopLeft ;
extern float VP1KeystoneXBottomLeft;
extern float VP1KeystoneYTopLeft ;
extern float VP1KeystoneYBottomLeft;
extern float VP1KeystoneXTopRight;
extern float VP1KeystoneXBottomRight;
extern float VP1KeystoneYTopRight;
extern float VP1KeystoneYBottomRight;
extern float textureFrontier_wmin;
extern float textureFrontier_wmax;
extern float textureFrontier_hmin;
extern float textureFrontier_hmax;
extern float textureFrontier_wmin_width;
extern float textureFrontier_wmax_width;
extern float textureFrontier_hmin_width;
extern float textureFrontier_hmax_width;
extern float textureScale_w      ;
extern float textureScale_h      ;
extern float textureTranslate_w  ;
extern float textureTranslate_h  ;
extern float meshFrontier_X      ;
extern float meshFrontier_width  ;
extern float nearPlane           ;
extern float farPlane            ;
extern bool  augmentedReality    ;
extern bool  meshRenderBypass    ;
extern bool  MIDIwithBeat        ;
extern bool  MIDIwithColor       ;
extern bool  MIDIwithBrush       ;
extern bool  MIDIwithCameraFlash ;
extern bool  MIDIwithPhotoFlash  ;
extern bool  MIDIwithStroke      ;
extern bool  MIDIwithDiaporama   ;
extern float light_color         [(PG_NB_LIGHTS+1)];
extern float light_color_pulse   [(PG_NB_LIGHTS+1)];
extern float light_grey          [(PG_NB_LIGHTS+1)];
extern float light_grey_pulse    [(PG_NB_LIGHTS+1)];
extern float light_dimmer        [(PG_NB_LIGHTS+1)];
extern float light_dimmer_pulse  [(PG_NB_LIGHTS+1)];
extern float light_strobe        [(PG_NB_LIGHTS+1)];
extern float light_strobe_pulse  [(PG_NB_LIGHTS+1)];
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path , _pg_string };
enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };
extern VarTypes pg_FullScenarioVarTypes[_MaxInterpVarIDs];
extern int pg_FullScenarioVarIndiceRanges[_MaxInterpVarIDs][2];
extern void * pg_FullScenarioVarPointers[_MaxInterpVarIDs];
extern std::string pg_FullScenarioVarMessages[_MaxInterpVarIDs];
extern std::string pg_FullScenarioVarStrings[_MaxInterpVarIDs];
void pg_FullScenarioArrayVarInit();
