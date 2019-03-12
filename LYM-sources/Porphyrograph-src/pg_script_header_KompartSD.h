// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
// VERBATIM	double window	wide screen	window width	window height	window x	window y	minimal interframe latency	time scale	initial time	camera ID	message pixel length	trace output frame number	trace time	first frame number	last frame number	max mouse recording frames (for track drawing reording)	max network message length	nb of particles
enum ConfigurationVarIDs{ 
  _double_window = 0,
  _wide_screen,
  _window_width,
  _window_height,
  _window_x,
  _window_y,
  _minimal_interframe_latency,
  _time_scale,
  _initial_time,
  _camID,
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
extern int   camID               ;
extern int   message_pixel_length;
extern bool  trace_output_frame_number;
extern bool  trace_time          ;
extern int   first_frame_number  ;
extern int   last_frame_number   ;
extern int   max_mouse_recording_frames;
extern int   max_network_message_length;
extern int   nb_particles        ;
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path };
enum PulseTypes { _pg_pulsed_absolute = 0 , _pg_pulsed_differential , _pg_pulsed_special , _pg_pulsed_none };
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63	64	65	66	67	68	69	70	71	72	73	74	75	76	77	78	79	80	81	82	83	84	85	86	87	88	89	90	91	92	93	94	95	96	97	98	99	100	101	102	103	104	105	106	107	108	109	110	111	112	113	114	115	116	117	118	119	120	121	122	123	124	125	126	127	128	129	130	131	132	133	134	135	136	137	138	139	140	141	142	143	144	145	146	147	148	149	150	151	152	153	154
// VERBATIM	auto_beat	clear all Layers	clear layer	current background capture	BG Subtraction on/off	mute screen	invert	snapshot onOff	cursor size	beat duration	Blend Transparency	CA mix weight	ID comment	track0 mix weight	ID comment	track0 mix weight	ID comment	CA Master weight	ID comment	track0 Master weight	ID comment	track0 Master weight	ID comment	echo	ID comment	echo neg	ID comment	CA decay	CA decay pulse	track0 decay	ID comment	track1 decay	ID comment	pen radius	pen radius pulse	palette for pen	palette for pen	Grey level for pen	ID comment	A color	ID comment	pen brush	pen radius replay	pen radius replay pulse	ID comment	ID comment	current drawing track	current video track	current photo track	ID comment	ID comment	ID comment	ID comment	ID comment	ID comment	ID comment	ID comment	path1 record on/off	path2 record on/off	path3 record on/off	path4 record on/off	path5 record on/off	path3 record on/off	path7 record on/off	path1 replay track	path2 replay track	path3 replay track	path4 replay track	path5 replay track	path6 replay track	path7 replay track	noise scale	ID comment	pixel acc	pixel acc pulse	pixel acc center-x	ID comment	pixel acc center-y	ID comment	pixel radius 	pixel radius pulse	pixel mode	repopuplating BG (grid)	ID comment	repopuplating Path	ID comment	palette for repop	repop color pulse	Grey level for repop	ID comment	camera white balance red	camera white balance blue	camera exposure	camera gain	cameraBrightness	camera saturation	camera contrast	camera gamma	camera capture frequency	movie no	movie capture frequency	photo diaporama index	photo diaporama fade dur	photo diaporama plateau dur	current SvgGpu images	soundtrack no	camera threshold	ID comment	camera Weight	ID comment	Sobel camera	ID comment	movie weight	ID comment	Sobel movie	ID comment	invert movie	ID comment	ID comment	ID comment	video saturation 	video saturation pulse	ID comment	ID comment	photo weight	ID comment	ID comment	ID comment	 Flash track1->BG freq	Flash pixel freq	Flash pixel duration	camera flash length (in frames)	camera flash bright (in frames)	camera flash beat no	beat threshold	beat min duration (ms)	input volume	Min input volume	pen pressure coef	pen angleH coef	pen angleV coef	tracks sync on/off	cumul video	CA update step	color interface display	fast vs slow CA color spreading	freeze on/off	sound enveloppe min	sound envelope max	adc weight (eg microphone)	soundtrack weight	sample setUp	sensor spatial pattern	sensor activation	sensor volume
enum InterpVarIDs{ 
  _auto_beat = 0,
  _clearAllLayers,
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
  _trackMixingWeight_0,
  _trackMixingWeight_0_pulse,
  _trackMixingWeight_1,
  _trackMixingWeight_1_pulse,
  _CAMasterWeight,
  _CAMasterWeight_pulse,
  _trackMasterWeight_0,
  _trackMasterWeight_0_pulse,
  _trackMasterWeight_1,
  _trackMasterWeight_1_pulse,
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
  _pen_radius,
  _pen_radius_pulse,
  _pen_color,
  _pen_color_pulse,
  _pen_grey,
  _pen_grey_pulse,
  _pen_color_a,
  _pen_color_a_pulse,
  _pen_brush,
  _pen_radius_replay,
  _pen_radius_replay_pulse,
  _pen_color_replay,
  _pen_color_replay_pulse,
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
  _path_record_1,
  _path_record_2,
  _path_record_3,
  _path_record_4,
  _path_record_5,
  _path_record_6,
  _path_record_7,
  _path_replay_trackNo_1,
  _path_replay_trackNo_2,
  _path_replay_trackNo_3,
  _path_replay_trackNo_4,
  _path_replay_trackNo_5,
  _path_replay_trackNo_6,
  _path_replay_trackNo_7,
  _noiseScale,
  _noiseScale_pulse,
  _pixel_acc,
  _pixel_acc_pulse,
  _pixel_acc_shiftX,
  _pixel_acc_shiftX_pulse,
  _pixel_acc_shiftY,
  _pixel_acc_shiftY_pulse,
  _pixel_radius,
  _pixel_radius_pulse,
  _pixel_mode,
  _repop_BG,
  _repop_BG_pulse,
  _repop_path,
  _repop_path_pulse,
  _repop_color,
  _repop_color_pulse,
  _repop_grey,
  _repop_grey_pulse,
  _cameraWB_R,
  _cameraWB_B,
  _cameraExposure,
  _cameraGain,
  _cameraBrightness,
  _cameraSaturation,
  _cameraContrast,
  _cameraGamma,
  _cameraCaptFreq,
  _playing_movieNo,
  _movieCaptFreq,
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
  _video_hue,
  _video_hue_pulse,
  _video_satur,
  _video_satur_pulse,
  _video_value,
  _video_value_pulse,
  _photoWeight,
  _photoWeight_pulse,
  _photo_hue,
  _photo_hue_pulse,
  _flashTrkBG_freq_1,
  _flashPixel_freq,
  _flashPixel_duration,
  _flashCameraTrkLength,
  _flashCameraTrkBright,
  _flashCameraTrkBeat,
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
  _freeze,
  _sound_env_min,
  _sound_env_max,
  _audioInput_weight,
  _soundtrack_weight,
  _sensor_sample_setUp,
  _sensor_layout,
  _sensor_activation,
  _sensor_vol,
  _MaxInterpVarIDs};
extern bool  auto_beat           ;
extern bool  clearAllLayers      ;
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
extern float trackMixingWeight_0 ;
extern float trackMixingWeight_0_pulse;
extern float trackMixingWeight_1 ;
extern float trackMixingWeight_1_pulse;
extern float CAMasterWeight      ;
extern float CAMasterWeight_pulse;
extern float trackMasterWeight_0 ;
extern float trackMasterWeight_0_pulse;
extern float trackMasterWeight_1 ;
extern float trackMasterWeight_1_pulse;
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
extern float pen_radius          ;
extern float pen_radius_pulse    ;
extern float pen_color           ;
extern float pen_color_pulse     ;
extern float pen_grey            ;
extern float pen_grey_pulse      ;
extern float pen_color_a         ;
extern float pen_color_a_pulse   ;
extern int   pen_brush           ;
extern float pen_radius_replay   ;
extern float pen_radius_replay_pulse;
extern float pen_color_replay    ;
extern float pen_color_replay_pulse;
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
extern bool  path_record_1       ;
extern bool  path_record_2       ;
extern bool  path_record_3       ;
extern bool  path_record_4       ;
extern bool  path_record_5       ;
extern bool  path_record_6       ;
extern bool  path_record_7       ;
extern int   path_replay_trackNo_1;
extern int   path_replay_trackNo_2;
extern int   path_replay_trackNo_3;
extern int   path_replay_trackNo_4;
extern int   path_replay_trackNo_5;
extern int   path_replay_trackNo_6;
extern int   path_replay_trackNo_7;
extern float noiseScale          ;
extern float noiseScale_pulse    ;
extern float pixel_acc           ;
extern float pixel_acc_pulse     ;
extern float pixel_acc_shiftX    ;
extern float pixel_acc_shiftX_pulse;
extern float pixel_acc_shiftY    ;
extern float pixel_acc_shiftY_pulse;
extern float pixel_radius        ;
extern float pixel_radius_pulse  ;
extern int   pixel_mode          ;
extern float repop_BG            ;
extern float repop_BG_pulse      ;
extern float repop_path          ;
extern float repop_path_pulse    ;
extern float repop_color         ;
extern float repop_color_pulse   ;
extern float repop_grey          ;
extern float repop_grey_pulse    ;
extern float cameraWB_R          ;
extern float cameraWB_B          ;
extern float cameraExposure      ;
extern float cameraGain          ;
extern float cameraBrightness    ;
extern float cameraSaturation    ;
extern float cameraContrast      ;
extern float cameraGamma         ;
extern float cameraCaptFreq      ;
extern int   playing_movieNo     ;
extern float movieCaptFreq       ;
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
extern float video_hue           ;
extern float video_hue_pulse     ;
extern float video_satur         ;
extern float video_satur_pulse   ;
extern float video_value         ;
extern float video_value_pulse   ;
extern float photoWeight         ;
extern float photoWeight_pulse   ;
extern float photo_hue           ;
extern float photo_hue_pulse     ;
extern int   flashTrkBG_freq_1   ;
extern int   flashPixel_freq     ;
extern int   flashPixel_duration ;
extern float flashCameraTrkLength;
extern float flashCameraTrkBright;
extern int   flashCameraTrkBeat  ;
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
extern bool  freeze              ;
extern float sound_env_min       ;
extern float sound_env_max       ;
extern float audioInput_weight   ;
extern float soundtrack_weight   ;
extern float sensor_sample_setUp ;
extern int   sensor_layout       ;
extern int   sensor_activation   ;
extern float sensor_vol          ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
