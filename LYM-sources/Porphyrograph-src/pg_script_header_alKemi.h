// RANK,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17
// VERBATIM,double window,wide screen,window width,window height,window x,window y,minimal interframe latency,time scale,initial time,message pixel length,trace output frame number,trace time,first frame number,last frame number,max mouse recording frames (for track drawing reording),max network message length,nb of particles
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
  _MaxConfigurationVarIDs};
extern bool  double_window       ;
extern bool  wide_screen         ;
extern int   window_width        ;
extern int   window_height       ;
extern int   window_x            ;
extern int   window_y            ;
extern float minimal_interframe_latency;
extern float time_scale          ;
extern float initial_time        ;
extern int   message_pixel_length;
extern bool  trace_output_frame_number;
extern bool  trace_time          ;
extern int   first_frame_number  ;
extern int   last_frame_number   ;
extern int   max_mouse_recording_frames;
extern int   max_network_message_length;
extern int   nb_particles        ;
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path , _pg_string };
enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_uniform , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
// RANK,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239
// VERBATIM,auto_beat,auto_pulse,clear,clear,clear,mute screen,invert,snapshot onOff,cursor size,beat duration,Blend Transparency,CA mixing weight,ID comment,CA Mixing weight,ID comment,track0 mixing weight,ID comment,track1 mixing weight,ID comment,track2 mixing weight,ID comment,CA Master weight,ID comment,CA Master weight,ID comment,track0 Master weight,ID comment,track1 Master weight,ID comment,track2 Master weight,ID comment,echo,echo_pulse,echo neg,echoNeg_pulse,CA decay,CA decay pulse,track0 decay,ID comment,track1 decay,ID comment,track2 decay,ID comment,track2 decay,ID comment,pen_radius,pen radius pulse,palette for pen,palette for pen,color music,Grey level for pen,Grey level for pen,A color,A color,pen brush,pen radius replay,pen radius replay pulse,pen_color_replay,pen_color_replay_pulse,pen saturation replay,pen saturation replay pulse,pen value replay,pen value replay pulse,pen hue replay,pen hue replay pulse,pen brush replay,current drawing track,current video track,current photo track,track_x_transl_0,track_x_transl_0_pulse,track_y_transl_0,track_y_transl_0_pulse,track_x_transl_1,track_x_transl_1_pulse,track_y_transl_1,track_y_transl_1_pulse,path0 repop on/off,path1 repop on/off,path2 repop on/off,path3 repop on/off,path4 repop on/off,path5 repop on/off,path6 repop on/off,path7 repop on/off,path8 repop on/off,path9 repop on/off,path10 repop on/off,path11 repop on/off,path1 record on/off,path2 record on/off,path3 record on/off,path4 record on/off,path5 record on/off,path3 record on/off,path7 record on/off,path8 record on/off,path9 record on/off,path10 record on/off,path11 record on/off,path1 replay track,path2 replay track,path3 replay track,path4 replay track,path5 replay track,path6 replay track,path7 replay track,path8 replay track,path9 replay track,path10 replay track,path11 replay track,ID comment,particle motion shift pulse,noise scale,ID comment,part noise type,part noise line scale,part noise angle scale,pixel noise center-x,pixel noise center-y,pixel acc,pixel acc pulse,pixel acc center-x,ID comment,pixel acc center-y,ID comment,radius pixel,pixel radius pulse,pixel_mode,repopuplating CA,ID comment,repopuplating track0,ID comment,repopuplating Path,ID comment,repop_colorBG,repop_colorBG_pulse,repop_greyBG,repop_greyBG_pulse,repop_colorPart,repop_colorPart_pulse,repop_greyPart,repop_greyPart_pulse,repop_colorCA,repop_colorCA_pulse,repop_greyCA,repop_greyCA_pulse,movie no,movie capture frequency,photo diaporama index,photo diaporama fade dur,photo diaporama plateau dur,current SvgGpu images,soundtrack on/off,movie weight,ID comment,Sobel movie,ID comment,invert movie,ID comment,video saturation ,video saturation pulse,ID comment,ID comment,photo satur,photo satur pulse,photo value,photo value pulse,photo gamma,photo gamma pulse,photo_threshold,photo_threshold_pulse,photo jitter amplitude,mask jitter amplitude,photo weight,ID comment,photo scale,photo rot,photo x_transl,photo y_transl,mask scale,mask contrast,CA cell proliferation,ID comment,CA cancer proliferation,ID comment,CA cancer cell ratio,ID comment,CAParams4,ID comment,CAParams5,ID comment,CAParams6,ID comment,CAParams7,ID comment,CAParams8,ID comment,CA1 type,CA1 subtype,CA2 type,CA2 subtype,CA1/CA2 weight, Flash track0->CA freq, Flash track1->CA freq, Flash track2->CA freq,Flash CA->BG freq, Flash track1->BG freq, Flash track2->BG freq,Flash pixel freq,Flash pixel duration,beat threshold,beat min duration (ms),input volume,Min input volume,pen pressure coef,pen angleH coef,pen angleV coef,tracks sync on/off,cumul video,CA step,color interface display,fast vs slow CA color spreading,master scale,master scale pulse,master scale ratio,master offsetX,master offsetY,freeze on/off,sound enveloppe min,sound envelope max,adc weight (eg microphone),soundtrack weight,MIDIwithBeat,MIDIwithColor,MIDIwithBrush,MIDIwithPhotoFlash,path_replay loop,movie loop
 #define var_auto_beat
 #define var_auto_pulse
 #define var_clearAllLayers
 #define var_clearCA
 #define var_clearLayer
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
 #define var_echo
 #define var_echo_pulse
 #define var_echoNeg
 #define var_echoNeg_pulse
 #define var_CAdecay
 #define var_CAdecay_pulse
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
 #define var_pen_saturation_replay
 #define var_pen_saturation_replay_pulse
 #define var_pen_value_replay
 #define var_pen_value_replay_pulse
 #define var_pen_hue_replay
 #define var_pen_hue_replay_pulse
 #define var_pen_brush_replay
 #define var_currentDrawingTrack
 #define var_currentVideoTrack
 #define var_currentPhotoTrack
 #define var_track_x_transl_0
 #define var_track_x_transl_0_pulse
 #define var_track_y_transl_0
 #define var_track_y_transl_0_pulse
 #define var_track_x_transl_1
 #define var_track_x_transl_1_pulse
 #define var_track_y_transl_1
 #define var_track_y_transl_1_pulse
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
 #define var_pulsed_part_Hshift
 #define var_pulsed_part_Vshift
 #define var_noiseScale
 #define var_noiseScale_pulse
 #define var_noiseType
 #define var_noiseLineScale
 #define var_noiseAngleScale
 #define var_noiseCenterX
 #define var_noiseCenterY
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
 #define var_repop_BG
 #define var_repop_BG_pulse
 #define var_repop_path
 #define var_repop_path_pulse
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
 #define var_playing_movieNo
 #define var_movieCaptFreq
 #define var_photo_diaporama
 #define var_photo_diaporama_fade
 #define var_photo_diaporama_plateau
 #define var_activeClipArts
 #define var_playing_soundtrackNo
 #define var_movieWeight
 #define var_movieWeight_pulse
 #define var_movieSobel
 #define var_movieSobel_pulse
 #define var_invertMovie
 #define var_invertCamera
 #define var_video_satur
 #define var_video_satur_pulse
 #define var_video_value
 #define var_video_value_pulse
 #define var_photo_satur
 #define var_photo_satur_pulse
 #define var_photo_value
 #define var_photo_value_pulse
 #define var_photo_gamma
 #define var_photo_gamma_pulse
 #define var_photo_threshold
 #define var_photo_threshold_pulse
 #define var_photoJitterAmpl
 #define var_maskJitterAmpl
 #define var_photoWeight
 #define var_photoWeight_pulse
 #define var_photo_scale
 #define var_photo_rot
 #define var_photo_transl_x
 #define var_photo_transl_y
 #define var_mask_scale
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
 #define var_flashTrkCA_freq_0
 #define var_flashTrkCA_freq_1
 #define var_flashTrkCA_freq_2
 #define var_flashCABG_freq
 #define var_flashTrkBG_freq_1
 #define var_flashTrkBG_freq_2
 #define var_flashPixel_freq
 #define var_flashPixel_duration
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
 #define var_interfaceOnScreen
 #define var_CAcolorSpread
 #define var_master_scale
 #define var_master_scale_pulse
 #define var_master_scale_ratio
 #define var_master_offsetX
 #define var_master_offsetY
 #define var_freeze
 #define var_sound_env_min
 #define var_sound_env_max
 #define var_audioInput_weight
 #define var_soundtrack_weight
 #define var_MIDIwithBeat
 #define var_MIDIwithColor
 #define var_MIDIwithBrush
 #define var_MIDIwithPhotoFlash
 #define var_path_replay_loop
 #define var_movie_loop
enum InterpVarIDs{ 
  _auto_beat = 0,
  _auto_pulse,
  _clearAllLayers,
  _clearCA,
  _clearLayer,
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
  _echo,
  _echo_pulse,
  _echoNeg,
  _echoNeg_pulse,
  _CAdecay,
  _CAdecay_pulse,
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
  _pen_saturation_replay,
  _pen_saturation_replay_pulse,
  _pen_value_replay,
  _pen_value_replay_pulse,
  _pen_hue_replay,
  _pen_hue_replay_pulse,
  _pen_brush_replay,
  _currentDrawingTrack,
  _currentVideoTrack,
  _currentPhotoTrack,
  _track_x_transl_0,
  _track_x_transl_0_pulse,
  _track_y_transl_0,
  _track_y_transl_0_pulse,
  _track_x_transl_1,
  _track_x_transl_1_pulse,
  _track_y_transl_1,
  _track_y_transl_1_pulse,
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
  _pulsed_part_Hshift,
  _pulsed_part_Vshift,
  _noiseScale,
  _noiseScale_pulse,
  _noiseType,
  _noiseLineScale,
  _noiseAngleScale,
  _noiseCenterX,
  _noiseCenterY,
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
  _repop_BG,
  _repop_BG_pulse,
  _repop_path,
  _repop_path_pulse,
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
  _playing_movieNo,
  _movieCaptFreq,
  _photo_diaporama,
  _photo_diaporama_fade,
  _photo_diaporama_plateau,
  _activeClipArts,
  _playing_soundtrackNo,
  _movieWeight,
  _movieWeight_pulse,
  _movieSobel,
  _movieSobel_pulse,
  _invertMovie,
  _invertCamera,
  _video_satur,
  _video_satur_pulse,
  _video_value,
  _video_value_pulse,
  _photo_satur,
  _photo_satur_pulse,
  _photo_value,
  _photo_value_pulse,
  _photo_gamma,
  _photo_gamma_pulse,
  _photo_threshold,
  _photo_threshold_pulse,
  _photoJitterAmpl,
  _maskJitterAmpl,
  _photoWeight,
  _photoWeight_pulse,
  _photo_scale,
  _photo_rot,
  _photo_transl_x,
  _photo_transl_y,
  _mask_scale,
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
  _flashTrkCA_freq_0,
  _flashTrkCA_freq_1,
  _flashTrkCA_freq_2,
  _flashCABG_freq,
  _flashTrkBG_freq_1,
  _flashTrkBG_freq_2,
  _flashPixel_freq,
  _flashPixel_duration,
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
  _interfaceOnScreen,
  _CAcolorSpread,
  _master_scale,
  _master_scale_pulse,
  _master_scale_ratio,
  _master_offsetX,
  _master_offsetY,
  _freeze,
  _sound_env_min,
  _sound_env_max,
  _audioInput_weight,
  _soundtrack_weight,
  _MIDIwithBeat,
  _MIDIwithColor,
  _MIDIwithBrush,
  _MIDIwithPhotoFlash,
  _path_replay_loop,
  _movie_loop,
  _MaxInterpVarIDs};
extern bool  auto_beat           ;
extern bool  auto_pulse          ;
extern bool  clearAllLayers      ;
extern bool  clearCA             ;
extern bool  clearLayer          ;
extern bool  mute_second_screen  ;
extern bool  invertAllLayers     ;
extern bool  take_snapshots      ;
extern float cursorSize          ;
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
extern float echo                ;
extern float echo_pulse          ;
extern float echoNeg             ;
extern float echoNeg_pulse       ;
extern float CAdecay             ;
extern float CAdecay_pulse       ;
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
extern float pen_saturation_replay;
extern float pen_saturation_replay_pulse;
extern float pen_value_replay    ;
extern float pen_value_replay_pulse;
extern float pen_hue_replay      ;
extern float pen_hue_replay_pulse;
extern int   pen_brush_replay    ;
extern int   currentDrawingTrack ;
extern int   currentVideoTrack   ;
extern int   currentPhotoTrack   ;
extern float track_x_transl_0    ;
extern float track_x_transl_0_pulse;
extern float track_y_transl_0    ;
extern float track_y_transl_0_pulse;
extern float track_x_transl_1    ;
extern float track_x_transl_1_pulse;
extern float track_y_transl_1    ;
extern float track_y_transl_1_pulse;
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
extern float pulsed_part_Hshift  ;
extern float pulsed_part_Vshift  ;
extern float noiseScale          ;
extern float noiseScale_pulse    ;
extern int   noiseType           ;
extern float noiseLineScale      ;
extern float noiseAngleScale     ;
extern float noiseCenterX        ;
extern float noiseCenterY        ;
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
extern float repop_BG            ;
extern float repop_BG_pulse      ;
extern float repop_path          ;
extern float repop_path_pulse    ;
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
extern int   playing_movieNo     ;
extern float movieCaptFreq       ;
extern int   photo_diaporama     ;
extern float photo_diaporama_fade;
extern float photo_diaporama_plateau;
extern int   activeClipArts      ;
extern int   playing_soundtrackNo;
extern float movieWeight         ;
extern float movieWeight_pulse   ;
extern float movieSobel          ;
extern float movieSobel_pulse    ;
extern bool  invertMovie         ;
extern bool  invertCamera        ;
extern float video_satur         ;
extern float video_satur_pulse   ;
extern float video_value         ;
extern float video_value_pulse   ;
extern float photo_satur         ;
extern float photo_satur_pulse   ;
extern float photo_value         ;
extern float photo_value_pulse   ;
extern float photo_gamma         ;
extern float photo_gamma_pulse   ;
extern float photo_threshold     ;
extern float photo_threshold_pulse;
extern float photoJitterAmpl     ;
extern float maskJitterAmpl      ;
extern float photoWeight         ;
extern float photoWeight_pulse   ;
extern float photo_scale         ;
extern float photo_rot           ;
extern float photo_transl_x      ;
extern float photo_transl_y      ;
extern float mask_scale          ;
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
extern int   flashTrkCA_freq_0   ;
extern int   flashTrkCA_freq_1   ;
extern int   flashTrkCA_freq_2   ;
extern int   flashCABG_freq      ;
extern int   flashTrkBG_freq_1   ;
extern int   flashTrkBG_freq_2   ;
extern int   flashPixel_freq     ;
extern int   flashPixel_duration ;
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
extern bool  interfaceOnScreen   ;
extern bool  CAcolorSpread       ;
extern float master_scale        ;
extern float master_scale_pulse  ;
extern float master_scale_ratio  ;
extern float master_offsetX      ;
extern float master_offsetY      ;
extern bool  freeze              ;
extern float sound_env_min       ;
extern float sound_env_max       ;
extern float audioInput_weight   ;
extern float soundtrack_weight   ;
extern bool  MIDIwithBeat        ;
extern bool  MIDIwithColor       ;
extern bool  MIDIwithBrush       ;
extern bool  MIDIwithPhotoFlash  ;
extern bool  path_replay_loop    ;
extern bool  movie_loop          ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *ScenarioVarStrings[_MaxInterpVarIDs];