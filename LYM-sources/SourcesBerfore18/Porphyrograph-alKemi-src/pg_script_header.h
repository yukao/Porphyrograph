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
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63	64	65	66	67	68	69	70	71	72	73	74	75	76	77	78
// VERBATIM	drawing stroke	drawing on off	brush ID	brush radius	palette 	R color	G color	B color	A color	Blend Transparency	part mode	part acc	part texture	part acc center-x	part acc center-y	CA type	CA subtype	clear	invert	cursor size	echo	echo neg	color music	brush radius music	part music	brush radius music replay	Flash track0 freq	Flash CA freq	Flash back freq	Flash part freq	Flash track0 number	Flash CA track number	Flash part duration	FlashBack track weight	pen pressure on/off	pen angle on/off	pen pressure coef	pen angleH coef	pen angleV coef	current track	CA decay sign	track0 decay sign	track 1 decay sign	track 2 decay sign	CA decay	track0 decay	track1 decay	track2 decay	CA weight	track0 weight	track1 weight	track2 weight	track1 y_transl	track0 replay on/off	track1 replay on/off	track2 replay on/off	source_track0 record on/off	source_track1 record on/off	source_track2 record on/off	disp_track0 on/off	disp_track1 on/off	disp_track2 on/off	disp_track0 weight	disp_track1 weight	disp_track2 weight	disp_track0 rot	disp_track1 rot	disp_track2 rot	disp_track0 x_transl	disp_track1 x_transl	disp_track2 x_transl	disp_track0 y_transl	disp_track1 y_transl	disp_track2 y_transl	disp_track0 scale	disp_track1 scale	disp_track2 scale	auto_beat	CA step
enum InterpVarIDs{ 
  _currentDrawingStroke = 0,
  _drawing_on_off,
  _brushID,
  _brushRadius,
  _RGBspectrum,
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
  _CAType,
  _CASubType,
  _clearAllLayers,
  _invertAllLayers,
  _cursorSize,
  _echo,
  _echoNeg,
  _color_spectrum_coef,
  _brush_radius_vol_coef,
  _part_acc_attack_coef,
  _brush_radius_vol_replay_coef,
  _flashTrack0_freq,
  _flashCA_freq,
  _flashBack_freq,
  _flashPart_freq,
  _flashTrack0_number,
  _flashCA_number,
  _flashPart_duration,
  _flashBack_coef,
  _withTabletPressureRadius,
  _withTabletAngleRadius,
  _brush_radius_pressure_coef,
  _brush_radius_angleHor_coef,
  _brush_radius_angleVer_coef,
  _currentTrack,
  _CAdecay_sign,
  _trackdecay_sign_0,
  _trackdecay_sign_1,
  _trackdecay_sign_2,
  _CAdecay,
  _trackdecay_0,
  _trackdecay_1,
  _trackdecay_2,
  _CAweight,
  _trackweight_0,
  _trackweight_1,
  _trackweight_2,
  _track_y_transl_1,
  _track_replay_0,
  _track_replay_1,
  _track_replay_2,
  _source_track_record_0,
  _source_track_record_1,
  _source_track_record_2,
  _source_track_display_0,
  _source_track_display_1,
  _source_track_display_2,
  _track_display_weight_0,
  _track_display_weight_1,
  _track_display_weight_2,
  _track_display_rot_0,
  _track_display_rot_1,
  _track_display_rot_2,
  _track_display_x_transl_0,
  _track_display_x_transl_1,
  _track_display_x_transl_2,
  _track_display_y_transl_0,
  _track_display_y_transl_1,
  _track_display_y_transl_2,
  _track_display_scale_0,
  _track_display_scale_1,
  _track_display_scale_2,
  _auto_beat,
  _CAstep,
  _MaxInterpVarIDs};
extern int   currentDrawingStroke;
extern bool  drawing_on_off      ;
extern int   brushID             ;
extern float brushRadius         ;
extern float RGBspectrum         ;
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
extern int   CAType              ;
extern int   CASubType           ;
extern bool  clearAllLayers      ;
extern bool  invertAllLayers     ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern float color_spectrum_coef ;
extern float brush_radius_vol_coef;
extern float part_acc_attack_coef;
extern float brush_radius_vol_replay_coef;
extern int   flashTrack0_freq    ;
extern int   flashCA_freq        ;
extern int   flashBack_freq      ;
extern int   flashPart_freq      ;
extern int   flashTrack0_number  ;
extern int   flashCA_number      ;
extern int   flashPart_duration  ;
extern float flashBack_coef      ;
extern bool  withTabletPressureRadius;
extern bool  withTabletAngleRadius;
extern float brush_radius_pressure_coef;
extern float brush_radius_angleHor_coef;
extern float brush_radius_angleVer_coef;
extern int   currentTrack        ;
extern float CAdecay_sign        ;
extern float trackdecay_sign_0   ;
extern float trackdecay_sign_1   ;
extern float trackdecay_sign_2   ;
extern float CAdecay             ;
extern float trackdecay_0        ;
extern float trackdecay_1        ;
extern float trackdecay_2        ;
extern float CAweight            ;
extern float trackweight_0       ;
extern float trackweight_1       ;
extern float trackweight_2       ;
extern int   track_y_transl_1    ;
extern bool  track_replay_0      ;
extern bool  track_replay_1      ;
extern bool  track_replay_2      ;
extern bool  source_track_record_0;
extern bool  source_track_record_1;
extern bool  source_track_record_2;
extern bool  source_track_display_0;
extern bool  source_track_display_1;
extern bool  source_track_display_2;
extern float track_display_weight_0;
extern float track_display_weight_1;
extern float track_display_weight_2;
extern float track_display_rot_0 ;
extern float track_display_rot_1 ;
extern float track_display_rot_2 ;
extern float track_display_x_transl_0;
extern float track_display_x_transl_1;
extern float track_display_x_transl_2;
extern float track_display_y_transl_0;
extern float track_display_y_transl_1;
extern float track_display_y_transl_2;
extern float track_display_scale_0;
extern float track_display_scale_1;
extern float track_display_scale_2;
extern bool  auto_beat           ;
extern int   CAstep              ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
