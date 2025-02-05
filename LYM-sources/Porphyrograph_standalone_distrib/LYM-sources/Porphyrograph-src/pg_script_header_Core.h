// CONFIGURATION CONSTANTS
// RANK,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18
// VERBATIM,double window,wide screen,window width,window height,window x,window y,minimal interframe latency,time scale,initial time,message pixel length,trace output frame number,trace time,first frame number,last frame number,max mouse recording frames (for track drawing reording),max network message length,nb of particles,nb types of CA
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
  _MaxConfigurationVarIDs};
// SCENARIO VARIABLES #0
// 0,1,2,3,4,5,6,7,8,9,2,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,196,197,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391
// auto_beat,auto_pulse,clear all Layers,clear CA,clear layer,current background capture,BG Subtraction on/off,mute screen,invert,snapshot onOff,cursor size,beat duration,master level,CA mix weight,ID comment,Part mix weight,ID comment,track0 mix weight,ID comment,track1 mix weight,ID comment,track2 mix weight,ID comment,track3 mix weight,ID comment,CA Master weight,ID comment,Part Master weight,ID comment,track0 Master weight,ID comment,track1 Master weight,ID comment,track2 Master weight,ID comment,track3 Master weight,ID comment,second Master mixing weight,echo,ID comment,echo neg,ID comment,CA decay,CA decay pulse,part decay,ID comment,track0 decay,ID comment,track1 decay,ID comment,track2 decay,ID comment,track3 decay,ID comment,pen radius,pen radius pulse,pen position pulse,pen angle pulse,pen position dashed,palette for pen,palette for pen pulse,color music,Grey level for pen,pen grey pulse,pen color alpha,pen color alpha pulse,pen brush,pen radius replay,pen radius replay pulse,pen color replay,ID comment,pen color replay,ID comment,ID comment,ID comment,pen saturation replay,ID comment,ID comment,ID comment,ID comment,current drawing track,current video track,current photo track,current mask track,ID comment,ID comment,ID comment,ID comment,ID comment,ID comment,ID comment,ID comment,part reinitialize,pixel image based acceleration,part image based acceleration,path0 repop on/off,path1 repop on/off,path2 repop on/off,path3 repop on/off,path4 repop on/off,path5 repop on/off,path6 repop on/off,path7 repop on/off,path8 repop on/off,path9 repop on/off,path10 repop on/off,path11 repop on/off,path0 follow on/off,path1 follow on/off,path2 follow on/off,path3 follow on/off,path4 follow on/off,path5 follow on/off,path6 follow on/off,path7 follow on/off,path8 follow on/off,path9 follow on/off,path10 follow on/off,path11 follow on/off,path0 repulse on/off,path1 repulse on/off,path2 repulse on/off,path3 repulse on/off,path4 repulse on/off,path5 repulse on/off,path6 repulse on/off,path7 repulse on/off,path8 repulse on/off,path9 repulse on/off,path10 repulse on/off,path11 repulse on/off,path1 record on/off,path2 record on/off,path3 record on/off,path4 record on/off,path5 record on/off,path3 record on/off,path7 record on/off,path8 record on/off,path9 record on/off,path10 record on/off,path11 record on/off,path1 replay track,path2 replay track,path3 replay track,path4 replay track,path5 replay track,path6 replay track,path7 replay track,path8 replay track,path9 replay track,path10 replay track,path11 replay track,part size,particle radius pulse,partSplat texture rank,part acc,part acc pulse,part damp factor,ID comment,part gravity,part gravity_pulse factor,ID comment,particle motion shift pulse,noise scale,ID comment,noise scale,ID comment,part field weight,ID comment,part damp target radius,part time to target,particle type,part move grid,part move rand,part exit mode,part stroke mode,part color mode,pixel acc,pixel acc pulse,pixel acc center-x,ID comment,pixel acc center-y,ID comment,pixel radius ,pixel radius pulse,pixel mode,repopuplating CA,ID comment,repopuplating Part,particle repop pulse,repopuplating BG (grid),ID comment,repopuplating Path,ID comment,prepop density for pixels,prepop density for particles,prepop color mode for pixels,prepop color mode for particles,repop_colorBG,repop_colorBG_pulse,repop_greyBG,repop_greyBG_pulse,repop_colorPart,repop_colorPart_pulse,repop_greyPart,repop_greyPart_pulse,repop_colorCA,repop_colorCA_pulse,repop_greyCA,repop_greyCA_pulse,camera white balance red,camera white balance blue,camera exposure,camera gain,cameraBrightness,camera saturation,camera contrast,camera gamma,camera capture frequency,camera gamma (shader),ID comment,camera no,movie no,movie capture frequency,clip name,clip name,clip name,clip name,clip left right mix,clip capture frequency,clip jog scratch factor,clip jog nudge factor,photo diaporama index,photo diaporama fade dur,photo diaporama plateau dur,current active clip arts,soundtrack no,camera threshold,ID comment,camera Weight,ID comment,Sobel camera,ID comment,movie weight,ID comment,Sobel movie,ID comment,invert movie,ID comment,invertPhoto,video saturation ,video saturation pulse,video_value,video_value_pulse,movie_gamma,movie_gamma_pulse,movie_threshold,movie_threshold_pulse,video_gamma,video_gamma_pulse,video_threshold,video_threshold_pulse,video_contrast,video_contrast_pulse,photo weight,ID comment,photo satur,photo satur pulse,photo value,photo value pulse,photo value,photo value pulse,ID comment,ID comment,Sobel photo,ID comment,photo jitter amplitude,mask jitter amplitude,photo scaleX,photo scaleY,photo rot,photo x_transl,photo y_transl,photo offSetX,photo offsetY,mask scale,photo contrast,mask contrast,CAParams1,ID comment,CAParams2,ID comment,CAParams3,ID comment,CAParams4,ID comment,CAParams5,ID comment,CAParams6,ID comment,CAParams7,ID comment,CAParams8,ID comment,CA 1 type,CA 1 subtype,CA 2 type,CA 2 subtype,CA1/CA2 weight,Flash Part->CA freq, Flash track0->CA freq, Flash track1->CA freq, Flash track2->CA freq, Flash track3->CA freq,Flash CA->BG freq,Flash Part->BG freq, Flash track1->BG freq, Flash track2->BG freq, Flash track3->BG freq,Flash CA->Part freq,Flash Trk0->Part freq,Flash Trk1->Part freq,Flash Trk2->Part freq,Flash Trk3->Part freq,Flash particle init freq,Flash pixel freq,Flash pixel duration,camera flash length (in frames),camera flash bright (in frames),camera flash beat no,photo flash length (in frames),photo flash bright (in frames),photo flash beat no,photo change flash beat no,master scale,master scale pulse,master scale ratio,master offsetX,master offsetY,Master mask opacity No1,Master mask opacity No2,Master mask opacity No3,Master mask opacity No4,Master mask opacity No5,Master mask opacity No6,beat threshold,beat min duration (ms),input volume,Min input volume,pen pressure coef,pen angleH coef,pen angleV coef,tracks sync on/off,cumul video,CA update step,Pixel update step,fast vs slow CA color spreading,freeze on/off,sound enveloppe min,sound envelope max,adc weight (eg microphone),soundtrack weight pure data,soundtrack weight portaudio,sample setUp,sensor spatial pattern,sensor activation,sensor volume,movie loop,path_replay loop,path_replay speed,path scaleX,path scaleY,master mask crop x,master mask crop y,master mask crop width,blur radius trk1,blur radius trk2,ID comment,ID comment,screen_drawing_no,Blend Transparency,master mask scale,master mask scale ratio,master mask offsetX,master mask offsetY,ID comment,ID comment,ID comment,ID comment,penStrokeAtBeat,path group,Argenteuil Flash change invert,Flash change invert,Bugs,Bugs pulse
#define var_auto_beat
#define var_auto_pulse
#define var_clearAllLayers
#define var_clearCA
#define var_clearLayer
#define var_reset_camera
#define var_camera_BG_subtr
#define var_mute_second_screen
#define var_invertAllLayers
#define var_take_snapshots
#define var_cursorSize
#define var_auto_beat_duration
#define var_master
#define var_CAMixingWeight
#define var_CAMixingWeight_pulse
#define var_PartMixingWeight
#define var_PartMixingWeight_pulse
#define var_trackMixingWeight_0
#define var_trackMixingWeight_0_pulse
#define var_trackMixingWeight_1
#define var_trackMixingWeight_1_pulse
#define var_trackMixingWeight_2
#define var_trackMixingWeight_2_pulse
#define var_trackMixingWeight_3
#define var_trackMixingWeight_3_pulse
#define var_CAMasterWeight
#define var_CAMasterWeight_pulse
#define var_PartMasterWeight
#define var_PartMasterWeight_pulse
#define var_trackMasterWeight_0
#define var_trackMasterWeight_0_pulse
#define var_trackMasterWeight_1
#define var_trackMasterWeight_1_pulse
#define var_trackMasterWeight_2
#define var_trackMasterWeight_2_pulse
#define var_trackMasterWeight_3
#define var_trackMasterWeight_3_pulse
#define var_SecondMasterMixingWeight
#define var_echo
#define var_echo_pulse
#define var_echoNeg
#define var_echoNeg_pulse
#define var_CAdecay
#define var_CAdecay_pulse
#define var_partDecay
#define var_partDecay_pulse
#define var_trkDecay_0
#define var_trkDecay_0_pulse
#define var_trkDecay_1
#define var_trkDecay_1_pulse
#define var_trkDecay_2
#define var_trkDecay_2_pulse
#define var_trkDecay_3
#define var_trkDecay_3_pulse
#define var_pen_radius
#define var_pen_radius_pulse
#define var_pen_position_pulse
#define var_pen_angle_pulse
#define var_pen_position_dash
#define var_pen_color
#define var_pen_color_pulse
#define var_color_spectrum_coef
#define var_pen_grey
#define var_pen_grey_pulse
#define var_pen_color_a
#define var_pen_color_a_pulse
#define var_pen_brush
#define var_pen_radius_replay
#define var_pen_radius_replay_pulse
#define var_pen_color_replay
#define var_pen_color_replay_pulse
#define var_pen_hue_replay
#define var_pen_hue_replay_pulse
#define var_pen_grey_replay
#define var_pen_grey_replay_pulse
#define var_pen_saturation_replay
#define var_pen_saturation_replay_pulse
#define var_pen_value_replay
#define var_pen_value_replay_pulse
#define var_pen_brush_replay
#define var_currentDrawingTrack
#define var_currentVideoTrack
#define var_currentPhotoTrack
#define var_currentMaskTrack
#define var_track_x_transl_0
#define var_track_x_transl_0_pulse
#define var_track_y_transl_0
#define var_track_y_transl_0_pulse
#define var_track_x_transl_1
#define var_track_x_transl_1_pulse
#define var_track_y_transl_1
#define var_track_y_transl_1_pulse
#define var_part_initialization
#define var_pixel_image_acceleration
#define var_part_image_acceleration
#define var_part_path_repop_0
#define var_part_path_repop_1
#define var_part_path_repop_2
#define var_part_path_repop_3
#define var_part_path_repop_4
#define var_part_path_repop_5
#define var_part_path_repop_6
#define var_part_path_repop_7
#define var_part_path_repop_8
#define var_part_path_repop_9
#define var_part_path_repop_10
#define var_part_path_repop_11
#define var_part_path_follow_0
#define var_part_path_follow_1
#define var_part_path_follow_2
#define var_part_path_follow_3
#define var_part_path_follow_4
#define var_part_path_follow_5
#define var_part_path_follow_6
#define var_part_path_follow_7
#define var_part_path_follow_8
#define var_part_path_follow_9
#define var_part_path_follow_10
#define var_part_path_follow_11
#define var_part_path_repulse_0
#define var_part_path_repulse_1
#define var_part_path_repulse_2
#define var_part_path_repulse_3
#define var_part_path_repulse_4
#define var_part_path_repulse_5
#define var_part_path_repulse_6
#define var_part_path_repulse_7
#define var_part_path_repulse_8
#define var_part_path_repulse_9
#define var_part_path_repulse_10
#define var_part_path_repulse_11
#define var_path_record_1
#define var_path_record_2
#define var_path_record_3
#define var_path_record_4
#define var_path_record_5
#define var_path_record_6
#define var_path_record_7
#define var_path_record_8
#define var_path_record_9
#define var_path_record_10
#define var_path_record_11
#define var_path_replay_trackNo_1
#define var_path_replay_trackNo_2
#define var_path_replay_trackNo_3
#define var_path_replay_trackNo_4
#define var_path_replay_trackNo_5
#define var_path_replay_trackNo_6
#define var_path_replay_trackNo_7
#define var_path_replay_trackNo_8
#define var_path_replay_trackNo_9
#define var_path_replay_trackNo_10
#define var_path_replay_trackNo_11
#define var_part_size
#define var_part_size_pulse
#define var_partSplat_texture
#define var_part_acc
#define var_part_acc_pulse
#define var_part_damp
#define var_part_damp_pulse
#define var_part_gravity
#define var_part_gravity_pulse
#define var_pulsed_part_Hshift
#define var_pulsed_part_Vshift
#define var_noiseUpdateScale
#define var_noiseUpdateScale_pulse
#define var_noiseParticleScale
#define var_noiseParticleScale_pulse
#define var_part_field_weight
#define var_part_field_weight_pulse
#define var_part_damp_targtRad
#define var_part_timeToTargt
#define var_particle_type
#define var_partMove_target
#define var_partMove_rand
#define var_partExit_mode
#define var_partStroke_mode
#define var_partColor_mode
#define var_pixel_acc
#define var_pixel_acc_pulse
#define var_pixel_acc_shiftX
#define var_pixel_acc_shiftX_pulse
#define var_pixel_acc_shiftY
#define var_pixel_acc_shiftY_pulse
#define var_pixel_radius
#define var_pixel_radius_pulse
#define var_pixel_mode
#define var_repop_CA
#define var_repop_CA_pulse
#define var_repop_part
#define var_repop_part_pulse
#define var_repop_BG
#define var_repop_BG_pulse
#define var_repop_path
#define var_repop_path_pulse
#define var_BG_CA_repop_density
#define var_Part_repop_density
#define var_BG_CA_repop_color_mode
#define var_Part_repop_color_mode
#define var_repop_colorBG
#define var_repop_colorBG_pulse
#define var_repop_greyBG
#define var_repop_greyBG_pulse
#define var_repop_colorPart
#define var_repop_colorPart_pulse
#define var_repop_greyPart
#define var_repop_greyPart_pulse
#define var_repop_colorCA
#define var_repop_colorCA_pulse
#define var_repop_greyCA
#define var_repop_greyCA_pulse
#define var_cameraWB_R
#define var_cameraWB_B
#define var_cameraExposure
#define var_cameraGain
#define var_cameraBrightness
#define var_cameraSaturation
#define var_cameraContrast
#define var_cameraGamma
#define var_cameraCaptFreq
#define var_camera_gamma
#define var_camera_gamma_pulse
#define var_cameraNo
#define var_playing_movieNo
#define var_movieCaptFreq
#define var_playing_clipNameLeft
#define var_playing_clipNameRight
#define var_playing_secondClipNameLeft
#define var_playing_secondClipNameRight
#define var_clip_mix
#define var_clipCaptFreq
#define var_clip_scratch_factor
#define var_clip_nudge_factor
#define var_photo_diaporama
#define var_photo_diaporama_fade
#define var_photo_diaporama_plateau
#define var_activeClipArts
#define var_playing_soundtrackNo
#define var_cameraThreshold
#define var_cameraThreshold_pulse
#define var_cameraWeight
#define var_cameraWeight_pulse
#define var_cameraSobel
#define var_cameraSobel_pulse
#define var_movieWeight
#define var_movieWeight_pulse
#define var_movieSobel
#define var_movieSobel_pulse
#define var_invertMovie
#define var_invertCamera
#define var_invertPhoto
#define var_video_satur
#define var_video_satur_pulse
#define var_video_value
#define var_video_value_pulse
#define var_movie_gamma
#define var_movie_gamma_pulse
#define var_movie_threshold
#define var_movie_threshold_pulse
#define var_video_gamma
#define var_video_gamma_pulse
#define var_video_threshold
#define var_video_threshold_pulse
#define var_video_contrast
#define var_video_contrast_pulse
#define var_photoWeight
#define var_photoWeight_pulse
#define var_photo_satur
#define var_photo_satur_pulse
#define var_photo_value
#define var_photo_value_pulse
#define var_photo_gamma
#define var_photo_gamma_pulse
#define var_photo_threshold
#define var_photo_threshold_pulse
#define var_photoSobel
#define var_photoSobel_pulse
#define var_photoJitterAmpl
#define var_maskJitterAmpl
#define var_photo_scaleX
#define var_photo_scaleY
#define var_photo_rot
#define var_photo_transl_x
#define var_photo_transl_y
#define var_photo_offsetX
#define var_photo_offsetY
#define var_mask_scale
#define var_photo_contrast
#define var_mask_contrast
#define var_CAParams1
#define var_CAParams1_pulse
#define var_CAParams2
#define var_CAParams2_pulse
#define var_CAParams3
#define var_CAParams3_pulse
#define var_CAParams4
#define var_CAParams4_pulse
#define var_CAParams5
#define var_CAParams5_pulse
#define var_CAParams6
#define var_CAParams6_pulse
#define var_CAParams7
#define var_CAParams7_pulse
#define var_CAParams8
#define var_CAParams8_pulse
#define var_CA1Type
#define var_CA1SubType
#define var_CA2Type
#define var_CA2SubType
#define var_CA1_CA2_weight
#define var_flashPartCA_freq
#define var_flashTrkCA_freq_0
#define var_flashTrkCA_freq_1
#define var_flashTrkCA_freq_2
#define var_flashTrkCA_freq_3
#define var_flashCABG_freq
#define var_flashPartBG_freq
#define var_flashTrkBG_freq_1
#define var_flashTrkBG_freq_2
#define var_flashTrkBG_freq_3
#define var_flashCAPart_freq
#define var_flashTrkPart_freq_0
#define var_flashTrkPart_freq_1
#define var_flashTrkPart_freq_2
#define var_flashTrkPart_freq_3
#define var_flashParticleInit_freq
#define var_flashPixel_freq
#define var_flashPixel_duration
#define var_flashCameraTrkLength
#define var_flashCameraTrkBright
#define var_flashCameraTrkBeat
#define var_flashPhotoTrkLength
#define var_flashPhotoTrkBright
#define var_flashPhotoTrkBeat
#define var_flashPhotoChangeBeat
#define var_master_scale
#define var_master_scale_pulse
#define var_master_scale_ratio
#define var_master_offsetX
#define var_master_offsetY
#define var_master_mask_opacity_1
#define var_master_mask_opacity_2
#define var_master_mask_opacity_3
#define var_master_mask_opacity_4
#define var_master_mask_opacity_5
#define var_master_mask_opacity_6
#define var_beat_threshold
#define var_beat_delay
#define var_sound_volume
#define var_sound_min
#define var_pen_radius_pressure_coef
#define var_pen_radius_angleHor_coef
#define var_pen_radius_angleVer_coef
#define var_tracksSync
#define var_cameraCumul
#define var_CAstep
#define var_Pixelstep
#define var_CAcolorSpread
#define var_freeze
#define var_sound_env_min
#define var_sound_env_max
#define var_audioInput_weight
#define var_soundtrack_PD_weight
#define var_soundtrack_PA_weight
#define var_sensor_sample_setUp
#define var_sensor_layout
#define var_sensor_activation
#define var_sensor_vol
#define var_movie_loop
#define var_path_replay_loop
#define var_path_replay_speed
#define var_path_scaleX
#define var_path_scaleY
#define var_master_crop_x
#define var_master_crop_y
#define var_master_crop_width
#define var_blurRadius_1
#define var_blurRadius_2
#define var_video_white
#define var_video_white_pulse
#define var_screen_drawing_no
#define var_master_mask
#define var_master_mask_scale
#define var_master_mask_scale_ratio
#define var_master_mask_offsetX
#define var_master_mask_offsetY
#define var_video_hue
#define var_video_hue_pulse
#define var_photo_hue
#define var_photo_hue_pulse
#define var_penStrokeAtBeat
#define var_path_group
#define var_Argenteuil_flash_move_track1_freq
#define var_Argenteuil_flashchange_diaporama_freq
#define var_Argenteuil_bugs
#define var_Argenteuil_bugs_pulse
enum InterpVarIDs{ 
  _auto_beat = 0,
  _auto_pulse,
  _clearAllLayers,
  _clearCA,
  _clearLayer,
  _reset_camera,
  _camera_BG_subtr,
  _mute_second_screen,
  _invertAllLayers,
  _take_snapshots,
  _cursorSize,
  _auto_beat_duration,
  _master,
  _CAMixingWeight,
  _CAMixingWeight_pulse,
  _PartMixingWeight,
  _PartMixingWeight_pulse,
  _trackMixingWeight_0,
  _trackMixingWeight_0_pulse,
  _trackMixingWeight_1,
  _trackMixingWeight_1_pulse,
  _trackMixingWeight_2,
  _trackMixingWeight_2_pulse,
  _trackMixingWeight_3,
  _trackMixingWeight_3_pulse,
  _CAMasterWeight,
  _CAMasterWeight_pulse,
  _PartMasterWeight,
  _PartMasterWeight_pulse,
  _trackMasterWeight_0,
  _trackMasterWeight_0_pulse,
  _trackMasterWeight_1,
  _trackMasterWeight_1_pulse,
  _trackMasterWeight_2,
  _trackMasterWeight_2_pulse,
  _trackMasterWeight_3,
  _trackMasterWeight_3_pulse,
  _SecondMasterMixingWeight,
  _echo,
  _echo_pulse,
  _echoNeg,
  _echoNeg_pulse,
  _CAdecay,
  _CAdecay_pulse,
  _partDecay,
  _partDecay_pulse,
  _trkDecay_0,
  _trkDecay_0_pulse,
  _trkDecay_1,
  _trkDecay_1_pulse,
  _trkDecay_2,
  _trkDecay_2_pulse,
  _trkDecay_3,
  _trkDecay_3_pulse,
  _pen_radius,
  _pen_radius_pulse,
  _pen_position_pulse,
  _pen_angle_pulse,
  _pen_position_dash,
  _pen_color,
  _pen_color_pulse,
  _color_spectrum_coef,
  _pen_grey,
  _pen_grey_pulse,
  _pen_color_a,
  _pen_color_a_pulse,
  _pen_brush,
  _pen_radius_replay,
  _pen_radius_replay_pulse,
  _pen_color_replay,
  _pen_color_replay_pulse,
  _pen_hue_replay,
  _pen_hue_replay_pulse,
  _pen_grey_replay,
  _pen_grey_replay_pulse,
  _pen_saturation_replay,
  _pen_saturation_replay_pulse,
  _pen_value_replay,
  _pen_value_replay_pulse,
  _pen_brush_replay,
  _currentDrawingTrack,
  _currentVideoTrack,
  _currentPhotoTrack,
  _currentMaskTrack,
  _track_x_transl_0,
  _track_x_transl_0_pulse,
  _track_y_transl_0,
  _track_y_transl_0_pulse,
  _track_x_transl_1,
  _track_x_transl_1_pulse,
  _track_y_transl_1,
  _track_y_transl_1_pulse,
  _part_initialization,
  _pixel_image_acceleration,
  _part_image_acceleration,
  _part_path_repop_0,
  _part_path_repop_1,
  _part_path_repop_2,
  _part_path_repop_3,
  _part_path_repop_4,
  _part_path_repop_5,
  _part_path_repop_6,
  _part_path_repop_7,
  _part_path_repop_8,
  _part_path_repop_9,
  _part_path_repop_10,
  _part_path_repop_11,
  _part_path_follow_0,
  _part_path_follow_1,
  _part_path_follow_2,
  _part_path_follow_3,
  _part_path_follow_4,
  _part_path_follow_5,
  _part_path_follow_6,
  _part_path_follow_7,
  _part_path_follow_8,
  _part_path_follow_9,
  _part_path_follow_10,
  _part_path_follow_11,
  _part_path_repulse_0,
  _part_path_repulse_1,
  _part_path_repulse_2,
  _part_path_repulse_3,
  _part_path_repulse_4,
  _part_path_repulse_5,
  _part_path_repulse_6,
  _part_path_repulse_7,
  _part_path_repulse_8,
  _part_path_repulse_9,
  _part_path_repulse_10,
  _part_path_repulse_11,
  _path_record_1,
  _path_record_2,
  _path_record_3,
  _path_record_4,
  _path_record_5,
  _path_record_6,
  _path_record_7,
  _path_record_8,
  _path_record_9,
  _path_record_10,
  _path_record_11,
  _path_replay_trackNo_1,
  _path_replay_trackNo_2,
  _path_replay_trackNo_3,
  _path_replay_trackNo_4,
  _path_replay_trackNo_5,
  _path_replay_trackNo_6,
  _path_replay_trackNo_7,
  _path_replay_trackNo_8,
  _path_replay_trackNo_9,
  _path_replay_trackNo_10,
  _path_replay_trackNo_11,
  _part_size,
  _part_size_pulse,
  _partSplat_texture,
  _part_acc,
  _part_acc_pulse,
  _part_damp,
  _part_damp_pulse,
  _part_gravity,
  _part_gravity_pulse,
  _pulsed_part_Hshift,
  _pulsed_part_Vshift,
  _noiseUpdateScale,
  _noiseUpdateScale_pulse,
  _noiseParticleScale,
  _noiseParticleScale_pulse,
  _part_field_weight,
  _part_field_weight_pulse,
  _part_damp_targtRad,
  _part_timeToTargt,
  _particle_type,
  _partMove_target,
  _partMove_rand,
  _partExit_mode,
  _partStroke_mode,
  _partColor_mode,
  _pixel_acc,
  _pixel_acc_pulse,
  _pixel_acc_shiftX,
  _pixel_acc_shiftX_pulse,
  _pixel_acc_shiftY,
  _pixel_acc_shiftY_pulse,
  _pixel_radius,
  _pixel_radius_pulse,
  _pixel_mode,
  _repop_CA,
  _repop_CA_pulse,
  _repop_part,
  _repop_part_pulse,
  _repop_BG,
  _repop_BG_pulse,
  _repop_path,
  _repop_path_pulse,
  _BG_CA_repop_density,
  _Part_repop_density,
  _BG_CA_repop_color_mode,
  _Part_repop_color_mode,
  _repop_colorBG,
  _repop_colorBG_pulse,
  _repop_greyBG,
  _repop_greyBG_pulse,
  _repop_colorPart,
  _repop_colorPart_pulse,
  _repop_greyPart,
  _repop_greyPart_pulse,
  _repop_colorCA,
  _repop_colorCA_pulse,
  _repop_greyCA,
  _repop_greyCA_pulse,
  _cameraWB_R,
  _cameraWB_B,
  _cameraExposure,
  _cameraGain,
  _cameraBrightness,
  _cameraSaturation,
  _cameraContrast,
  _cameraGamma,
  _cameraCaptFreq,
  _camera_gamma,
  _camera_gamma_pulse,
  _cameraNo,
  _playing_movieNo,
  _movieCaptFreq,
  _playing_clipNameLeft,
  _playing_clipNameRight,
  _playing_secondClipNameLeft,
  _playing_secondClipNameRight,
  _clip_mix,
  _clipCaptFreq,
  _clip_scratch_factor,
  _clip_nudge_factor,
  _photo_diaporama,
  _photo_diaporama_fade,
  _photo_diaporama_plateau,
  _activeClipArts,
  _playing_soundtrackNo,
  _cameraThreshold,
  _cameraThreshold_pulse,
  _cameraWeight,
  _cameraWeight_pulse,
  _cameraSobel,
  _cameraSobel_pulse,
  _movieWeight,
  _movieWeight_pulse,
  _movieSobel,
  _movieSobel_pulse,
  _invertMovie,
  _invertCamera,
  _invertPhoto,
  _video_satur,
  _video_satur_pulse,
  _video_value,
  _video_value_pulse,
  _movie_gamma,
  _movie_gamma_pulse,
  _movie_threshold,
  _movie_threshold_pulse,
  _video_gamma,
  _video_gamma_pulse,
  _video_threshold,
  _video_threshold_pulse,
  _video_contrast,
  _video_contrast_pulse,
  _photoWeight,
  _photoWeight_pulse,
  _photo_satur,
  _photo_satur_pulse,
  _photo_value,
  _photo_value_pulse,
  _photo_gamma,
  _photo_gamma_pulse,
  _photo_threshold,
  _photo_threshold_pulse,
  _photoSobel,
  _photoSobel_pulse,
  _photoJitterAmpl,
  _maskJitterAmpl,
  _photo_scaleX,
  _photo_scaleY,
  _photo_rot,
  _photo_transl_x,
  _photo_transl_y,
  _photo_offsetX,
  _photo_offsetY,
  _mask_scale,
  _photo_contrast,
  _mask_contrast,
  _CAParams1,
  _CAParams1_pulse,
  _CAParams2,
  _CAParams2_pulse,
  _CAParams3,
  _CAParams3_pulse,
  _CAParams4,
  _CAParams4_pulse,
  _CAParams5,
  _CAParams5_pulse,
  _CAParams6,
  _CAParams6_pulse,
  _CAParams7,
  _CAParams7_pulse,
  _CAParams8,
  _CAParams8_pulse,
  _CA1Type,
  _CA1SubType,
  _CA2Type,
  _CA2SubType,
  _CA1_CA2_weight,
  _flashPartCA_freq,
  _flashTrkCA_freq_0,
  _flashTrkCA_freq_1,
  _flashTrkCA_freq_2,
  _flashTrkCA_freq_3,
  _flashCABG_freq,
  _flashPartBG_freq,
  _flashTrkBG_freq_1,
  _flashTrkBG_freq_2,
  _flashTrkBG_freq_3,
  _flashCAPart_freq,
  _flashTrkPart_freq_0,
  _flashTrkPart_freq_1,
  _flashTrkPart_freq_2,
  _flashTrkPart_freq_3,
  _flashParticleInit_freq,
  _flashPixel_freq,
  _flashPixel_duration,
  _flashCameraTrkLength,
  _flashCameraTrkBright,
  _flashCameraTrkBeat,
  _flashPhotoTrkLength,
  _flashPhotoTrkBright,
  _flashPhotoTrkBeat,
  _flashPhotoChangeBeat,
  _master_scale,
  _master_scale_pulse,
  _master_scale_ratio,
  _master_offsetX,
  _master_offsetY,
  _master_mask_opacity_1,
  _master_mask_opacity_2,
  _master_mask_opacity_3,
  _master_mask_opacity_4,
  _master_mask_opacity_5,
  _master_mask_opacity_6,
  _beat_threshold,
  _beat_delay,
  _sound_volume,
  _sound_min,
  _pen_radius_pressure_coef,
  _pen_radius_angleHor_coef,
  _pen_radius_angleVer_coef,
  _tracksSync,
  _cameraCumul,
  _CAstep,
  _Pixelstep,
  _CAcolorSpread,
  _freeze,
  _sound_env_min,
  _sound_env_max,
  _audioInput_weight,
  _soundtrack_PD_weight,
  _soundtrack_PA_weight,
  _sensor_sample_setUp,
  _sensor_layout,
  _sensor_activation,
  _sensor_vol,
  _movie_loop,
  _path_replay_loop,
  _path_replay_speed,
  _path_scaleX,
  _path_scaleY,
  _master_crop_x,
  _master_crop_y,
  _master_crop_width,
  _blurRadius_1,
  _blurRadius_2,
  _video_white,
  _video_white_pulse,
  _screen_drawing_no,
  _master_mask,
  _master_mask_scale,
  _master_mask_scale_ratio,
  _master_mask_offsetX,
  _master_mask_offsetY,
  _video_hue,
  _video_hue_pulse,
  _photo_hue,
  _photo_hue_pulse,
  _penStrokeAtBeat,
  _path_group,
  _Argenteuil_flash_move_track1_freq,
  _Argenteuil_flashchange_diaporama_freq,
  _Argenteuil_bugs,
  _Argenteuil_bugs_pulse,
  _MaxInterpVarIDs};
#define _NbConfigurations 1
extern bool  auto_beat           ;
extern bool  auto_pulse          ;
extern bool  clearAllLayers      ;
extern bool  clearCA             ;
extern bool  clearLayer          ;
extern bool  reset_camera        ;
extern bool  camera_BG_subtr     ;
extern bool  mute_second_screen  ;
extern bool  invertAllLayers     ;
extern bool  take_snapshots      ;
extern int   cursorSize          ;
extern float auto_beat_duration  ;
extern float master              ;
extern float CAMixingWeight      ;
extern float CAMixingWeight_pulse;
extern float PartMixingWeight    ;
extern float PartMixingWeight_pulse;
extern float trackMixingWeight_0 ;
extern float trackMixingWeight_0_pulse;
extern float trackMixingWeight_1 ;
extern float trackMixingWeight_1_pulse;
extern float trackMixingWeight_2 ;
extern float trackMixingWeight_2_pulse;
extern float trackMixingWeight_3 ;
extern float trackMixingWeight_3_pulse;
extern float CAMasterWeight      ;
extern float CAMasterWeight_pulse;
extern float PartMasterWeight    ;
extern float PartMasterWeight_pulse;
extern float trackMasterWeight_0 ;
extern float trackMasterWeight_0_pulse;
extern float trackMasterWeight_1 ;
extern float trackMasterWeight_1_pulse;
extern float trackMasterWeight_2 ;
extern float trackMasterWeight_2_pulse;
extern float trackMasterWeight_3 ;
extern float trackMasterWeight_3_pulse;
extern float SecondMasterMixingWeight;
extern float echo                ;
extern float echo_pulse          ;
extern float echoNeg             ;
extern float echoNeg_pulse       ;
extern float CAdecay             ;
extern float CAdecay_pulse       ;
extern float partDecay           ;
extern float partDecay_pulse     ;
extern float trkDecay_0          ;
extern float trkDecay_0_pulse    ;
extern float trkDecay_1          ;
extern float trkDecay_1_pulse    ;
extern float trkDecay_2          ;
extern float trkDecay_2_pulse    ;
extern float trkDecay_3          ;
extern float trkDecay_3_pulse    ;
extern float pen_radius          ;
extern float pen_radius_pulse    ;
extern float pen_position_pulse  ;
extern float pen_angle_pulse     ;
extern int   pen_position_dash   ;
extern float pen_color           ;
extern float pen_color_pulse     ;
extern float color_spectrum_coef ;
extern float pen_grey            ;
extern float pen_grey_pulse      ;
extern float pen_color_a         ;
extern float pen_color_a_pulse   ;
extern int   pen_brush           ;
extern float pen_radius_replay   ;
extern float pen_radius_replay_pulse;
extern float pen_color_replay    ;
extern float pen_color_replay_pulse;
extern float pen_hue_replay      ;
extern float pen_hue_replay_pulse;
extern float pen_grey_replay     ;
extern float pen_grey_replay_pulse;
extern float pen_saturation_replay;
extern float pen_saturation_replay_pulse;
extern float pen_value_replay    ;
extern float pen_value_replay_pulse;
extern int   pen_brush_replay    ;
extern int   currentDrawingTrack ;
extern int   currentVideoTrack   ;
extern int   currentPhotoTrack   ;
extern int   currentMaskTrack    ;
extern float track_x_transl_0    ;
extern float track_x_transl_0_pulse;
extern float track_y_transl_0    ;
extern float track_y_transl_0_pulse;
extern float track_x_transl_1    ;
extern float track_x_transl_1_pulse;
extern float track_y_transl_1    ;
extern float track_y_transl_1_pulse;
extern int   part_initialization ;
extern int   pixel_image_acceleration;
extern int   part_image_acceleration;
extern bool  part_path_repop_0   ;
extern bool  part_path_repop_1   ;
extern bool  part_path_repop_2   ;
extern bool  part_path_repop_3   ;
extern bool  part_path_repop_4   ;
extern bool  part_path_repop_5   ;
extern bool  part_path_repop_6   ;
extern bool  part_path_repop_7   ;
extern bool  part_path_repop_8   ;
extern bool  part_path_repop_9   ;
extern bool  part_path_repop_10  ;
extern bool  part_path_repop_11  ;
extern bool  part_path_follow_0  ;
extern bool  part_path_follow_1  ;
extern bool  part_path_follow_2  ;
extern bool  part_path_follow_3  ;
extern bool  part_path_follow_4  ;
extern bool  part_path_follow_5  ;
extern bool  part_path_follow_6  ;
extern bool  part_path_follow_7  ;
extern bool  part_path_follow_8  ;
extern bool  part_path_follow_9  ;
extern bool  part_path_follow_10 ;
extern bool  part_path_follow_11 ;
extern bool  part_path_repulse_0 ;
extern bool  part_path_repulse_1 ;
extern bool  part_path_repulse_2 ;
extern bool  part_path_repulse_3 ;
extern bool  part_path_repulse_4 ;
extern bool  part_path_repulse_5 ;
extern bool  part_path_repulse_6 ;
extern bool  part_path_repulse_7 ;
extern bool  part_path_repulse_8 ;
extern bool  part_path_repulse_9 ;
extern bool  part_path_repulse_10;
extern bool  part_path_repulse_11;
extern bool  path_record_1       ;
extern bool  path_record_2       ;
extern bool  path_record_3       ;
extern bool  path_record_4       ;
extern bool  path_record_5       ;
extern bool  path_record_6       ;
extern bool  path_record_7       ;
extern bool  path_record_8       ;
extern bool  path_record_9       ;
extern bool  path_record_10      ;
extern bool  path_record_11      ;
extern int   path_replay_trackNo_1;
extern int   path_replay_trackNo_2;
extern int   path_replay_trackNo_3;
extern int   path_replay_trackNo_4;
extern int   path_replay_trackNo_5;
extern int   path_replay_trackNo_6;
extern int   path_replay_trackNo_7;
extern int   path_replay_trackNo_8;
extern int   path_replay_trackNo_9;
extern int   path_replay_trackNo_10;
extern int   path_replay_trackNo_11;
extern float part_size           ;
extern float part_size_pulse     ;
extern int   partSplat_texture   ;
extern float part_acc            ;
extern float part_acc_pulse      ;
extern float part_damp           ;
extern float part_damp_pulse     ;
extern float part_gravity        ;
extern float part_gravity_pulse  ;
extern float pulsed_part_Hshift  ;
extern float pulsed_part_Vshift  ;
extern float noiseUpdateScale    ;
extern float noiseUpdateScale_pulse;
extern float noiseParticleScale  ;
extern float noiseParticleScale_pulse;
extern float part_field_weight   ;
extern float part_field_weight_pulse;
extern float part_damp_targtRad  ;
extern float part_timeToTargt    ;
extern int   particle_type       ;
extern bool  partMove_target     ;
extern bool  partMove_rand       ;
extern int   partExit_mode       ;
extern int   partStroke_mode     ;
extern int   partColor_mode      ;
extern float pixel_acc           ;
extern float pixel_acc_pulse     ;
extern float pixel_acc_shiftX    ;
extern float pixel_acc_shiftX_pulse;
extern float pixel_acc_shiftY    ;
extern float pixel_acc_shiftY_pulse;
extern float pixel_radius        ;
extern float pixel_radius_pulse  ;
extern int   pixel_mode          ;
extern float repop_CA            ;
extern float repop_CA_pulse      ;
extern float repop_part          ;
extern float repop_part_pulse    ;
extern float repop_BG            ;
extern float repop_BG_pulse      ;
extern float repop_path          ;
extern float repop_path_pulse    ;
extern int   BG_CA_repop_density ;
extern int   Part_repop_density  ;
extern int   BG_CA_repop_color_mode;
extern int   Part_repop_color_mode;
extern float repop_colorBG       ;
extern float repop_colorBG_pulse ;
extern float repop_greyBG        ;
extern float repop_greyBG_pulse  ;
extern float repop_colorPart     ;
extern float repop_colorPart_pulse;
extern float repop_greyPart      ;
extern float repop_greyPart_pulse;
extern float repop_colorCA       ;
extern float repop_colorCA_pulse ;
extern float repop_greyCA        ;
extern float repop_greyCA_pulse  ;
extern float cameraWB_R          ;
extern float cameraWB_B          ;
extern float cameraExposure      ;
extern float cameraGain          ;
extern float cameraBrightness    ;
extern float cameraSaturation    ;
extern float cameraContrast      ;
extern float cameraGamma         ;
extern float cameraCaptFreq      ;
extern float camera_gamma        ;
extern float camera_gamma_pulse  ;
extern int   cameraNo            ;
extern int   playing_movieNo     ;
extern float movieCaptFreq       ;
extern string playing_clipNameLeft;
extern string playing_clipNameRight;
extern string playing_secondClipNameLeft;
extern string playing_secondClipNameRight;
extern float clip_mix            ;
extern float clipCaptFreq        ;
extern float clip_scratch_factor ;
extern float clip_nudge_factor   ;
extern int   photo_diaporama     ;
extern float photo_diaporama_fade;
extern float photo_diaporama_plateau;
extern int   activeClipArts      ;
extern int   playing_soundtrackNo;
extern float cameraThreshold     ;
extern float cameraThreshold_pulse;
extern float cameraWeight        ;
extern float cameraWeight_pulse  ;
extern float cameraSobel         ;
extern float cameraSobel_pulse   ;
extern float movieWeight         ;
extern float movieWeight_pulse   ;
extern float movieSobel          ;
extern float movieSobel_pulse    ;
extern bool  invertMovie         ;
extern bool  invertCamera        ;
extern bool  invertPhoto         ;
extern float video_satur         ;
extern float video_satur_pulse   ;
extern float video_value         ;
extern float video_value_pulse   ;
extern float movie_gamma         ;
extern float movie_gamma_pulse   ;
extern float movie_threshold     ;
extern float movie_threshold_pulse;
extern float video_gamma         ;
extern float video_gamma_pulse   ;
extern float video_threshold     ;
extern float video_threshold_pulse;
extern float video_contrast      ;
extern float video_contrast_pulse;
extern float photoWeight         ;
extern float photoWeight_pulse   ;
extern float photo_satur         ;
extern float photo_satur_pulse   ;
extern float photo_value         ;
extern float photo_value_pulse   ;
extern float photo_gamma         ;
extern float photo_gamma_pulse   ;
extern float photo_threshold     ;
extern float photo_threshold_pulse;
extern float photoSobel          ;
extern float photoSobel_pulse    ;
extern float photoJitterAmpl     ;
extern float maskJitterAmpl      ;
extern float photo_scaleX        ;
extern float photo_scaleY        ;
extern float photo_rot           ;
extern float photo_transl_x      ;
extern float photo_transl_y      ;
extern float photo_offsetX       ;
extern float photo_offsetY       ;
extern float mask_scale          ;
extern float photo_contrast      ;
extern float mask_contrast       ;
extern float CAParams1           ;
extern float CAParams1_pulse     ;
extern float CAParams2           ;
extern float CAParams2_pulse     ;
extern float CAParams3           ;
extern float CAParams3_pulse     ;
extern float CAParams4           ;
extern float CAParams4_pulse     ;
extern float CAParams5           ;
extern float CAParams5_pulse     ;
extern float CAParams6           ;
extern float CAParams6_pulse     ;
extern float CAParams7           ;
extern float CAParams7_pulse     ;
extern float CAParams8           ;
extern float CAParams8_pulse     ;
extern int   CA1Type             ;
extern int   CA1SubType          ;
extern int   CA2Type             ;
extern int   CA2SubType          ;
extern float CA1_CA2_weight      ;
extern int   flashPartCA_freq    ;
extern int   flashTrkCA_freq_0   ;
extern int   flashTrkCA_freq_1   ;
extern int   flashTrkCA_freq_2   ;
extern int   flashTrkCA_freq_3   ;
extern int   flashCABG_freq      ;
extern int   flashPartBG_freq    ;
extern int   flashTrkBG_freq_1   ;
extern int   flashTrkBG_freq_2   ;
extern int   flashTrkBG_freq_3   ;
extern int   flashCAPart_freq    ;
extern int   flashTrkPart_freq_0 ;
extern int   flashTrkPart_freq_1 ;
extern int   flashTrkPart_freq_2 ;
extern int   flashTrkPart_freq_3 ;
extern int   flashParticleInit_freq;
extern int   flashPixel_freq     ;
extern int   flashPixel_duration ;
extern float flashCameraTrkLength;
extern float flashCameraTrkBright;
extern int   flashCameraTrkBeat  ;
extern float flashPhotoTrkLength ;
extern float flashPhotoTrkBright ;
extern int   flashPhotoTrkBeat   ;
extern int   flashPhotoChangeBeat;
extern float master_scale        ;
extern float master_scale_pulse  ;
extern float master_scale_ratio  ;
extern float master_offsetX      ;
extern float master_offsetY      ;
extern float master_mask_opacity_1;
extern float master_mask_opacity_2;
extern float master_mask_opacity_3;
extern float master_mask_opacity_4;
extern float master_mask_opacity_5;
extern float master_mask_opacity_6;
extern float beat_threshold      ;
extern float beat_delay          ;
extern float sound_volume        ;
extern float sound_min           ;
extern float pen_radius_pressure_coef;
extern float pen_radius_angleHor_coef;
extern float pen_radius_angleVer_coef;
extern bool  tracksSync          ;
extern int   cameraCumul         ;
extern int   CAstep              ;
extern int   Pixelstep           ;
extern bool  CAcolorSpread       ;
extern bool  freeze              ;
extern float sound_env_min       ;
extern float sound_env_max       ;
extern float audioInput_weight   ;
extern float soundtrack_PD_weight;
extern float soundtrack_PA_weight;
extern float sensor_sample_setUp ;
extern int   sensor_layout       ;
extern int   sensor_activation   ;
extern float sensor_vol          ;
extern bool  movie_loop          ;
extern bool  path_replay_loop    ;
extern float path_replay_speed   ;
extern float path_scaleX         ;
extern float path_scaleY         ;
extern float master_crop_x       ;
extern float master_crop_y       ;
extern float master_crop_width   ;
extern int   blurRadius_1        ;
extern int   blurRadius_2        ;
extern float video_white         ;
extern float video_white_pulse   ;
extern int   screen_drawing_no   ;
extern float master_mask         ;
extern float master_mask_scale   ;
extern float master_mask_scale_ratio;
extern float master_mask_offsetX ;
extern float master_mask_offsetY ;
extern float video_hue           ;
extern float video_hue_pulse     ;
extern float photo_hue           ;
extern float photo_hue_pulse     ;
extern bool  penStrokeAtBeat     ;
extern int   path_group          ;
extern int   Argenteuil_flash_move_track1_freq;
extern int   Argenteuil_flashchange_diaporama_freq;
extern float Argenteuil_bugs     ;
extern float Argenteuil_bugs_pulse;
extern bool  ScenarioVarConfigurations[_MaxInterpVarIDs][1];
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path , _pg_string };
enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *ScenarioVarStrings[_MaxInterpVarIDs];
