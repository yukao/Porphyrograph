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
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63	64	65	66	67	68	69	70	71	72	73	74	75	76	77	78	78	79	80	81	82	83	84	85	86	87	88	89	90	91	92	93	94	95	96	97	98	99	100	101	102	103	104	105	106	107	108	109	110	111	112	113	114	115	116	117	118	119	120	121	122	123	124	125	126	127	128	129	130	131	132	133	134	135	136	137	138	139	140	141	142	143	144	145	146	147	148	149	150	151	152	153	154	155	156	157	158	159	160	161	162	163	164	165	166	167	168	169	170	171	172	173	174	175	176	177	178	179	180	181	182	183	184	185	186	187	188	189	190	191	192	193	194	195	196	196	197	198	199	200	201	202	203	204	205	206	207	208	209	210	211	212	213	214	215	216	217	218	219	220	221	222	223	224
// VERBATIM	pen brush	pen radius	pen radius pulse	palette for pen	palette for pen	palette for repop	repop color pulse	Grey level for pen	Grey level for repop	A color	Blend Transparency	pixel mode	pixel acc	pixel acc pulse	noise scale	part noise line scale	part noise type	part noise angle scale	part noise center-x	part noise center-y	pixel acc center-x	pixel acc center-y	pixel radius 	pixel radius pulse	repopuplating BG (grid)	repopuplating CA	repopuplating Part	particle repop pulse	CA 1 type	CA 1 subtype	CA 2 type	CA 2 subtype	CA1/CA2 weight	CAParams1	CAParams2	CAParams3	CAParams4	CAParams5	CAParams6	CAParams7	CAParams8	clear CA	clear layer	clear all Layers	invert	snapshot onOff	cursor size	echo	echo neg	blur radius trk1	blur radius trk2	Flash Part->CA freq	 Flash track0->CA freq	 Flash track1->CA freq	 Flash track2->CA freq	 Flash track3->CA freq	Flash CA->Part freq	Flash Trk0->Part freq	Flash Trk1->Part freq	Flash Trk2->Part freq	Flash Trk3->Part freq	Flash CA->BG freq	Flash Part->BG freq	 Flash track1->BG freq	 Flash track2->BG freq	 Flash track3->BG freq	Flash pixel freq	Flash pixel duration	camera flash length (in frames)	camera flash bright (in frames)	camera flash beat no	pen pressure coef	pen angleH coef	pen angleV coef	tracks sync on/off	current drawing track	current video track	current photo track	current active clip arts	track0 decay sign	track 1 decay sign	track 2 decay sign	track 3 decay sign	track0 decay	track1 decay	track2 decay	track3 decay	CA mix weight	Part mix weight	track0 mix weight	track1 mix weight	track2 mix weight	track3 mix weight	CA Master weight	Part Master weight	track0 Master weight	track1 Master weight	track2 Master weight	track3 Master weight	path1 replay track	path2 replay track	path3 replay track	path4 replay track	path5 replay track	path6 replay track	path7 replay track	path1 record on/off	path2 record on/off	path3 record on/off	path4 record on/off	path5 record on/off	path3 record on/off	path7 record on/off	path0 repop on/off	path1 repop on/off	path2 repop on/off	path3 repop on/off	path4 repop on/off	path5 repop on/off	path6 repop on/off	path7 repop on/off	path0 follow on/off	path1 follow on/off	path2 follow on/off	path3 follow on/off	path4 follow on/off	path5 follow on/off	path6 follow on/off	path7 follow on/off	path0 repulse on/off	path1 repulse on/off	path2 repulse on/off	path3 repulse on/off	path4 repulse on/off	path5 repulse on/off	path6 repulse on/off	path7 repulse on/off	auto_beat	beat duration	CA decay sign	CA decay	CA decay pulse	invert movie	cumul video	camera threshold	camera gamma	camera exposure	camera gain	cameraBrightness	camera saturation	camera contrast	camera white balance red	camera white balance blue	video saturation 	video saturation pulse	camera capture frequency	movie capture frequency	camera Weight	movie weight	Sobel camera	Sobel movie	movie no	BG Subtraction on/off	CA update step	color interface display	fast vs slow CA color spreading	input volume	Min input volume	beat threshold	beat min duration (ms)	freeze on/off	photo weight	photo diaporama index	photo diaporama fade dur	photo diaporama plateau dur	sound enveloppe min	sound envelope max	current background capture	part decay sign	part decay	part reinitialize	part radius	particle radius pulse	part move grid	part move rand	part exit mode	part stroke mode	part color mode	part damp target radius	part time to target	part field weight	part repop radius	part acc	part damp factor	part acc pulse	hide	soundtrack no	adc weight (eg microphone)	soundtrack weight	track0 x_transl	track1 x_transl	track0 y_transl	track1 y_transl	repopuplating Path	particle type	sample setUp	sensor spatial pattern	sensor activation	sensor volume	pen radius replay	pen radius replay pulse	pen color  replay pulse	echo layer shift pulse	particle motion shift pulse	tracks layers pulse	flash weights pulse	mask jitter amplitude	photo jitter amplitude	photo value	photo value pulse	photo satur	photo satur pulse	mask scale	photo scale	mask contrast	photo contrast	mute screen
enum InterpVarIDs{ 
  _pen_brush = 0,
  _pen_radius,
  _pen_radius_pulse,
  _pen_color,
  _pen_color_pulse,
  _repop_color,
  _repop_color_pulse,
  _pen_grey,
  _repop_grey,
  _pen_color_a,
  _blendTransp,
  _pixel_mode,
  _pixel_acc_factor,
  _pixel_acc_pulse,
  _noiseScale,
  _noiseLineScale,
  _noiseType,
  _noiseAngleScale,
  _noiseCenter_0,
  _noiseCenter_1,
  _pixel_acc_center_0,
  _pixel_acc_center_1,
  _pixel_radius,
  _pixel_radius_pulse,
  _repop_BG,
  _repop_CA,
  _repop_part,
  _repop_part_pulse,
  _CA1Type,
  _CA1SubType,
  _CA2Type,
  _CA2SubType,
  _CA1_CA2_weight,
  _CAParams1,
  _CAParams2,
  _CAParams3,
  _CAParams4,
  _CAParams5,
  _CAParams6,
  _CAParams7,
  _CAParams8,
  _clearCA,
  _clearLayer,
  _clearAllLayers,
  _invertAllLayers,
  _take_snapshots,
  _cursorSize,
  _echo,
  _echoNeg,
  _blurRadius_1,
  _blurRadius_2,
  _flashPartCA_freq,
  _flashTrkCA_freq_0,
  _flashTrkCA_freq_1,
  _flashTrkCA_freq_2,
  _flashTrkCA_freq_3,
  _flashCAPart_freq,
  _flashTrkPart_freq_0,
  _flashTrkPart_freq_1,
  _flashTrkPart_freq_2,
  _flashTrkPart_freq_3,
  _flashCABG_freq,
  _flashPartBG_freq,
  _flashTrkBG_freq_1,
  _flashTrkBG_freq_2,
  _flashTrkBG_freq_3,
  _flashPixel_freq,
  _flashPixel_duration,
  _flashCameraTrkLength,
  _flashCameraTrkBright,
  _flashCameraTrkBeat,
  _pen_radius_pressure_coef,
  _pen_radius_angleHor_coef,
  _pen_radius_angleVer_coef,
  _tracksSync,
  _currentDrawingTrack,
  _currentVideoTrack,
  _currentPhotoTrack,
  _activeClipArts,
  _trkDecay_sign_0,
  _trkDecay_sign_1,
  _trkDecay_sign_2,
  _trkDecay_sign_3,
  _trkDecay_0,
  _trkDecay_1,
  _trkDecay_2,
  _trkDecay_3,
  _CAMixingWeight,
  _PartMixingWeight,
  _trackMixingWeight_0,
  _trackMixingWeight_1,
  _trackMixingWeight_2,
  _trackMixingWeight_3,
  _CAMasterWeight,
  _PartMasterWeight,
  _trackMasterWeight_0,
  _trackMasterWeight_1,
  _trackMasterWeight_2,
  _trackMasterWeight_3,
  _path_replay_trackNo_1,
  _path_replay_trackNo_2,
  _path_replay_trackNo_3,
  _path_replay_trackNo_4,
  _path_replay_trackNo_5,
  _path_replay_trackNo_6,
  _path_replay_trackNo_7,
  _path_record_1,
  _path_record_2,
  _path_record_3,
  _path_record_4,
  _path_record_5,
  _path_record_6,
  _path_record_7,
  _path_repop_0,
  _path_repop_1,
  _path_repop_2,
  _path_repop_3,
  _path_repop_4,
  _path_repop_5,
  _path_repop_6,
  _path_repop_7,
  _part_path_follow_0,
  _part_path_follow_1,
  _part_path_follow_2,
  _part_path_follow_3,
  _part_path_follow_4,
  _part_path_follow_5,
  _part_path_follow_6,
  _part_path_follow_7,
  _part_path_repulse_0,
  _part_path_repulse_1,
  _part_path_repulse_2,
  _part_path_repulse_3,
  _part_path_repulse_4,
  _part_path_repulse_5,
  _part_path_repulse_6,
  _part_path_repulse_7,
  _auto_beat,
  _auto_beat_duration,
  _CAdecay_sign,
  _CAdecay,
  _CAdecay_pulse,
  _invertMovie,
  _cameraCumul,
  _cameraThreshold,
  _cameraGamma,
  _cameraExposure,
  _cameraGain,
  _cameraBrightness,
  _cameraSaturation,
  _cameraContrast,
  _cameraWB_R,
  _cameraWB_B,
  _video_satur,
  _video_satur_pulse,
  _cameraCaptFreq,
  _movieCaptFreq,
  _cameraWeight,
  _movieWeight,
  _cameraSobel,
  _movieSobel,
  _playing_movieNo,
  _BGSubtr,
  _CAstep,
  _interfaceOnScreen,
  _CAcolorSpread,
  _sound_volume,
  _sound_min,
  _beat_threshold,
  _beat_delay,
  _freeze,
  _photoWeight,
  _photo_diaporama,
  _photo_diaporama_fade,
  _photo_diaporama_plateau,
  _sound_env_min,
  _sound_env_max,
  _currentBGCapture,
  _partDecay_sign,
  _partDecay,
  _part_initialization,
  _part_radius,
  _part_radius_pulse,
  _partMove_target,
  _partMove_rand,
  _partExit_mode,
  _partStroke_mode,
  _partColor_mode,
  _part_damp_targtRad,
  _part_timeToTargt,
  _part_field_weight,
  _partRepopRadius,
  _part_acc,
  _part_damp_factor,
  _part_acc_pulse,
  _hide,
  _playing_soundtrackNo,
  _audioInput_weight,
  _soundtrack_weight,
  _track_x_transl_0,
  _track_x_transl_1,
  _track_y_transl_0,
  _track_y_transl_1,
  _repop_path,
  _particle_type,
  _sample_setUp,
  _sensor_layout,
  _sensor_activation,
  _sensor_vol,
  _pen_radius_replay,
  _pen_radius_replay_pulse,
  _pen_color_replay_pulse,
  _echo_Hshift_pulse,
  _part_Vshift_pulse,
  _tracks_Hshift_pulse,
  _flashWeight_pulse,
  _maskJitterAmpl,
  _photoJitterAmpl,
  _photo_value,
  _photo_value_pulse,
  _photo_satur,
  _photo_satur_pulse,
  _mask_scale,
  _photo_scale,
  _mask_contrast,
  _photo_contrast,
  _mute_screen,
  _MaxInterpVarIDs};
extern int   pen_brush           ;
extern float pen_radius          ;
extern float pen_radius_pulse    ;
extern float pen_color           ;
extern float pen_color_pulse     ;
extern float repop_color         ;
extern float repop_color_pulse   ;
extern float pen_grey            ;
extern float repop_grey          ;
extern float pen_color_a         ;
extern float blendTransp         ;
extern int   pixel_mode          ;
extern float pixel_acc_factor    ;
extern float pixel_acc_pulse     ;
extern float noiseScale          ;
extern float noiseLineScale      ;
extern int   noiseType           ;
extern float noiseAngleScale     ;
extern float noiseCenter_0       ;
extern float noiseCenter_1       ;
extern float pixel_acc_center_0  ;
extern float pixel_acc_center_1  ;
extern float pixel_radius        ;
extern float pixel_radius_pulse  ;
extern float repop_BG            ;
extern float repop_CA            ;
extern float repop_part          ;
extern float repop_part_pulse    ;
extern int   CA1Type             ;
extern int   CA1SubType          ;
extern int   CA2Type             ;
extern int   CA2SubType          ;
extern float CA1_CA2_weight      ;
extern float CAParams1           ;
extern float CAParams2           ;
extern float CAParams3           ;
extern float CAParams4           ;
extern float CAParams5           ;
extern float CAParams6           ;
extern float CAParams7           ;
extern float CAParams8           ;
extern bool  clearCA             ;
extern bool  clearLayer          ;
extern bool  clearAllLayers      ;
extern bool  invertAllLayers     ;
extern bool  take_snapshots      ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern int   blurRadius_1        ;
extern int   blurRadius_2        ;
extern int   flashPartCA_freq    ;
extern int   flashTrkCA_freq_0   ;
extern int   flashTrkCA_freq_1   ;
extern int   flashTrkCA_freq_2   ;
extern int   flashTrkCA_freq_3   ;
extern int   flashCAPart_freq    ;
extern int   flashTrkPart_freq_0 ;
extern int   flashTrkPart_freq_1 ;
extern int   flashTrkPart_freq_2 ;
extern int   flashTrkPart_freq_3 ;
extern int   flashCABG_freq      ;
extern int   flashPartBG_freq    ;
extern int   flashTrkBG_freq_1   ;
extern int   flashTrkBG_freq_2   ;
extern int   flashTrkBG_freq_3   ;
extern int   flashPixel_freq     ;
extern int   flashPixel_duration ;
extern float flashCameraTrkLength;
extern float flashCameraTrkBright;
extern int   flashCameraTrkBeat  ;
extern float pen_radius_pressure_coef;
extern float pen_radius_angleHor_coef;
extern float pen_radius_angleVer_coef;
extern bool  tracksSync          ;
extern int   currentDrawingTrack ;
extern int   currentVideoTrack   ;
extern int   currentPhotoTrack   ;
extern int   activeClipArts      ;
extern float trkDecay_sign_0     ;
extern float trkDecay_sign_1     ;
extern float trkDecay_sign_2     ;
extern float trkDecay_sign_3     ;
extern float trkDecay_0          ;
extern float trkDecay_1          ;
extern float trkDecay_2          ;
extern float trkDecay_3          ;
extern float CAMixingWeight      ;
extern float PartMixingWeight    ;
extern float trackMixingWeight_0 ;
extern float trackMixingWeight_1 ;
extern float trackMixingWeight_2 ;
extern float trackMixingWeight_3 ;
extern float CAMasterWeight      ;
extern float PartMasterWeight    ;
extern float trackMasterWeight_0 ;
extern float trackMasterWeight_1 ;
extern float trackMasterWeight_2 ;
extern float trackMasterWeight_3 ;
extern int   path_replay_trackNo_1;
extern int   path_replay_trackNo_2;
extern int   path_replay_trackNo_3;
extern int   path_replay_trackNo_4;
extern int   path_replay_trackNo_5;
extern int   path_replay_trackNo_6;
extern int   path_replay_trackNo_7;
extern bool  path_record_1       ;
extern bool  path_record_2       ;
extern bool  path_record_3       ;
extern bool  path_record_4       ;
extern bool  path_record_5       ;
extern bool  path_record_6       ;
extern bool  path_record_7       ;
extern bool  path_repop_0        ;
extern bool  path_repop_1        ;
extern bool  path_repop_2        ;
extern bool  path_repop_3        ;
extern bool  path_repop_4        ;
extern bool  path_repop_5        ;
extern bool  path_repop_6        ;
extern bool  path_repop_7        ;
extern bool  part_path_follow_0  ;
extern bool  part_path_follow_1  ;
extern bool  part_path_follow_2  ;
extern bool  part_path_follow_3  ;
extern bool  part_path_follow_4  ;
extern bool  part_path_follow_5  ;
extern bool  part_path_follow_6  ;
extern bool  part_path_follow_7  ;
extern bool  part_path_repulse_0 ;
extern bool  part_path_repulse_1 ;
extern bool  part_path_repulse_2 ;
extern bool  part_path_repulse_3 ;
extern bool  part_path_repulse_4 ;
extern bool  part_path_repulse_5 ;
extern bool  part_path_repulse_6 ;
extern bool  part_path_repulse_7 ;
extern bool  auto_beat           ;
extern float auto_beat_duration  ;
extern float CAdecay_sign        ;
extern float CAdecay             ;
extern float CAdecay_pulse       ;
extern bool  invertMovie         ;
extern int   cameraCumul         ;
extern float cameraThreshold     ;
extern float cameraGamma         ;
extern float cameraExposure      ;
extern float cameraGain          ;
extern float cameraBrightness    ;
extern float cameraSaturation    ;
extern float cameraContrast      ;
extern float cameraWB_R          ;
extern float cameraWB_B          ;
extern float video_satur         ;
extern float video_satur_pulse   ;
extern float cameraCaptFreq      ;
extern float movieCaptFreq       ;
extern float cameraWeight        ;
extern float movieWeight         ;
extern float cameraSobel         ;
extern float movieSobel          ;
extern int   playing_movieNo     ;
extern bool  BGSubtr             ;
extern int   CAstep              ;
extern bool  interfaceOnScreen   ;
extern bool  CAcolorSpread       ;
extern float sound_volume        ;
extern float sound_min           ;
extern float beat_threshold      ;
extern float beat_delay          ;
extern bool  freeze              ;
extern float photoWeight         ;
extern int   photo_diaporama     ;
extern float photo_diaporama_fade;
extern float photo_diaporama_plateau;
extern float sound_env_min       ;
extern float sound_env_max       ;
extern bool  currentBGCapture    ;
extern float partDecay_sign      ;
extern float partDecay           ;
extern int   part_initialization ;
extern float part_radius         ;
extern float part_radius_pulse   ;
extern bool  partMove_target     ;
extern bool  partMove_rand       ;
extern int   partExit_mode       ;
extern int   partStroke_mode     ;
extern int   partColor_mode      ;
extern float part_damp_targtRad  ;
extern float part_timeToTargt    ;
extern float part_field_weight   ;
extern float partRepopRadius     ;
extern float part_acc            ;
extern float part_damp_factor    ;
extern float part_acc_pulse      ;
extern bool  hide                ;
extern int   playing_soundtrackNo;
extern float audioInput_weight   ;
extern float soundtrack_weight   ;
extern int   track_x_transl_0    ;
extern int   track_x_transl_1    ;
extern int   track_y_transl_0    ;
extern int   track_y_transl_1    ;
extern float repop_path          ;
extern int   particle_type       ;
extern float sample_setUp        ;
extern int   sensor_layout       ;
extern int   sensor_activation   ;
extern float sensor_vol          ;
extern float pen_radius_replay   ;
extern float pen_radius_replay_pulse;
extern float pen_color_replay_pulse;
extern float echo_Hshift_pulse   ;
extern float part_Vshift_pulse   ;
extern float tracks_Hshift_pulse ;
extern float flashWeight_pulse   ;
extern float maskJitterAmpl      ;
extern float photoJitterAmpl     ;
extern float photo_value         ;
extern float photo_value_pulse   ;
extern float photo_satur         ;
extern float photo_satur_pulse   ;
extern float mask_scale          ;
extern float photo_scale         ;
extern float mask_contrast       ;
extern float photo_contrast      ;
extern bool  mute_screen         ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
