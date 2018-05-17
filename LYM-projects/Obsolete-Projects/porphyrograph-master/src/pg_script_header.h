// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18
// VERBATIM	double window	wide screen	window width	window height	window x	window y	minimal interframe latency	time scale	initial time	input volume	camera ID	video threshold	message pixel length	trace output frame number	trace time	first frame number	last frame number	max mouse recording frames (for track drawing reording)	max network message length
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
  _input_volume,
  _camID,
  _videoThreshold,
  _message_pixel_length,
  _trace_output_frame_number,
  _trace_time,
  _first_frame_number,
  _last_frame_number,
  _max_mouse_recording_frames,
  _max_network_message_length,
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
extern float input_volume        ;
extern int   camID               ;
extern float videoThreshold      ;
extern int   message_pixel_length;
extern bool  trace_output_frame_number;
extern bool  trace_time          ;
extern int   first_frame_number  ;
extern int   last_frame_number   ;
extern int   max_mouse_recording_frames;
extern int   max_network_message_length;
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_track };
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	58	59	60	61	62	63	64	65	66	67	68	69	70	71	72	72	73	74	75	76	77	78	79	80	81	82	83	84	85	86
// VERBATIM	drawing stroke	brush ID	brush radius	palette 	R color	G color	B color	A color	Blend Transparency	part mode	part acc	part texture	part acc center-x	part acc center-y	radius particle	repopuplating track0	repopuplating CA	CA type	CA subtype	clear CA	clear layer	clear all Layers	copy track 1<->2	invert	cursor size	echo	echo neg	xy music	color music (val)	brush radius music	part music	color music (hue)	xy music replay	color music replay	brush radius music replay	track1 Flash track0 freq	track2 Flash track0 freq	track0 Flash CA freq	track1 Flash CA freq	track2 Flash CA freq	Flash back freq	Flash part freq	Flash part duration	pen pressure coef	pen angleH coef	pen angleV coef	tracks sync on/off	current drawing track	current video track	track0 decay sign	track 1 decay sign	track 2 decay sign	track0 decay	track1 decay	track2 decay	CA weight	track0 weight	track1 weight	track2 weight	CA weight	track0 weight	track1 weight	track2 weight	image capture weight	track0 replay on/off	track1 replay on/off	track2 replay on/off	source_track0 record on/off	source_track1 record on/off	source_track2 record on/off	target_track0 record on/off	target_track1 record on/off	target_track2 record on/off	auto_beat	beat duration	sample setUp	sensor activation	sensor spatial pattern	CA decay sign	CA decay	CAParams1	CAParams2	CAParams3	CAParams4	CAParams5	CAParams6	CAParams7	CAParams8	video_onOff	no vs initial Vs Current BG Subtraction	CA update step	interface mode shift	fast vs slow CA color spreading
enum InterpVarIDs{ 
  _currentDrawingStroke = 0,
  _brushID,
  _brushRadius,
  _RGBpalette,
  _local_color_r,
  _local_color_g,
  _local_color_b,
  _local_color_a,
  _blendTransp,
  _particleMode,
  _part_acc_factor,
  _particle_texture_ID,
  _partAccCenter_0,
  _partAccCenter_1,
  _radiusParticle,
  _repopulatingBG,
  _repopulatingCA,
  _CAType,
  _CASubType,
  _clearCA,
  _clearLayer,
  _clearAllLayers,
  _copyTrack12,
  _invertAllLayers,
  _cursorSize,
  _echo,
  _echoNeg,
  _xy_spectrum_coef,
  _RGBpalette_val_coef,
  _brush_radius_vol_coef,
  _part_acc_attack_coef,
  _RGBpalette_hue_coef,
  _xy_spectrum_replay_coef,
  _RGBpalette_replay_coef,
  _brush_radius_vol_replay_coef,
  _flashTrack0_freq_1,
  _flashTrack0_freq_2,
  _flashCA_freq_0,
  _flashCA_freq_1,
  _flashCA_freq_2,
  _flashBack_freq,
  _flashPart_freq,
  _flashPart_duration,
  _brush_radius_pressure_coef,
  _brush_radius_angleHor_coef,
  _brush_radius_angleVer_coef,
  _tracksSync,
  _currentDrawingTrack,
  _currentVideoTrack,
  _trackdecay_sign_0,
  _trackdecay_sign_1,
  _trackdecay_sign_2,
  _trackdecay_0,
  _trackdecay_1,
  _trackdecay_2,
  _CACompositionWeight,
  _trackCompositionWeight_0,
  _trackCompositionWeight_1,
  _trackCompositionWeight_2,
  _CAFinalWeight,
  _trackFinalWeight_0,
  _trackFinalWeight_1,
  _trackFinalWeight_2,
  _FBCaptureWeight,
  _track_replay_0,
  _track_replay_1,
  _track_replay_2,
  _source_track_record_0,
  _source_track_record_1,
  _source_track_record_2,
  _target_track_record_0,
  _target_track_record_1,
  _target_track_record_2,
  _auto_beat,
  _beat_duration,
  _sample_setUp,
  _sensor_activation,
  _sensor_layout,
  _CAdecay_sign,
  _CAdecay,
  _CAParams1,
  _CAParams2,
  _CAParams3,
  _CAParams4,
  _CAParams5,
  _CAParams6,
  _CAParams7,
  _CAParams8,
  _is_videoOn,
  _BGSubtr,
  _CAstep,
  _interfaceMode,
  _CAcolorSpread,
  _MaxInterpVarIDs};
extern int   currentDrawingStroke;
extern int   brushID             ;
extern float brushRadius         ;
extern float RGBpalette          ;
extern float local_color_r       ;
extern float local_color_g       ;
extern float local_color_b       ;
extern float local_color_a       ;
extern float blendTransp         ;
extern int   particleMode        ;
extern float part_acc_factor     ;
extern float particle_texture_ID ;
extern float partAccCenter_0     ;
extern float partAccCenter_1     ;
extern float radiusParticle      ;
extern float repopulatingBG      ;
extern float repopulatingCA      ;
extern int   CAType              ;
extern int   CASubType           ;
extern bool  clearCA             ;
extern bool  clearLayer          ;
extern bool  clearAllLayers      ;
extern bool  copyTrack12         ;
extern bool  invertAllLayers     ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern float xy_spectrum_coef    ;
extern float RGBpalette_val_coef ;
extern float brush_radius_vol_coef;
extern float part_acc_attack_coef;
extern float RGBpalette_hue_coef ;
extern float xy_spectrum_replay_coef;
extern float RGBpalette_replay_coef;
extern float brush_radius_vol_replay_coef;
extern int   flashTrack0_freq_1  ;
extern int   flashTrack0_freq_2  ;
extern int   flashCA_freq_0      ;
extern int   flashCA_freq_1      ;
extern int   flashCA_freq_2      ;
extern int   flashBack_freq      ;
extern int   flashPart_freq      ;
extern int   flashPart_duration  ;
extern float brush_radius_pressure_coef;
extern float brush_radius_angleHor_coef;
extern float brush_radius_angleVer_coef;
extern bool  tracksSync          ;
extern int   currentDrawingTrack ;
extern int   currentVideoTrack   ;
extern float trackdecay_sign_0   ;
extern float trackdecay_sign_1   ;
extern float trackdecay_sign_2   ;
extern float trackdecay_0        ;
extern float trackdecay_1        ;
extern float trackdecay_2        ;
extern float CACompositionWeight ;
extern float trackCompositionWeight_0;
extern float trackCompositionWeight_1;
extern float trackCompositionWeight_2;
extern float CAFinalWeight       ;
extern float trackFinalWeight_0  ;
extern float trackFinalWeight_1  ;
extern float trackFinalWeight_2  ;
extern float FBCaptureWeight     ;
extern bool  track_replay_0      ;
extern bool  track_replay_1      ;
extern bool  track_replay_2      ;
extern bool  source_track_record_0;
extern bool  source_track_record_1;
extern bool  source_track_record_2;
extern bool  target_track_record_0;
extern bool  target_track_record_1;
extern bool  target_track_record_2;
extern bool  auto_beat           ;
extern float beat_duration       ;
extern float sample_setUp        ;
extern int   sensor_activation   ;
extern int   sensor_layout       ;
extern float CAdecay_sign        ;
extern float CAdecay             ;
extern float CAParams1           ;
extern float CAParams2           ;
extern float CAParams3           ;
extern float CAParams4           ;
extern float CAParams5           ;
extern float CAParams6           ;
extern float CAParams7           ;
extern float CAParams8           ;
extern bool  is_videoOn          ;
extern int   BGSubtr             ;
extern int   CAstep              ;
extern bool  interfaceMode       ;
extern bool  CAcolorSpread       ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
