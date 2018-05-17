// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
// VERBATIM	double window	wide screen	window width	window height	window x	window y	minimal interframe latency	time scale	initial time	input volume	camera ID	message pixel length	trace output frame number	trace time	first frame number	last frame number	max mouse recording frames (for track drawing reording)	max network message length
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
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	46	47	48	49	50	51	52	53	54	55	56	57	58
// VERBATIM	drawing stroke	brush radius	palette 	R color	G color	B color	A color	Blend Transparency	part mode	part acc	part texture	part acc center-x	part acc center-y	radius particle	repopuplating track0	clear layer	clear all Layers	invert	cursor size	echo	echo neg	xy music	color music (val)	color music (hue)	track1 Flash track0 freq	Flash part freq	Flash part duration	pen pressure coef	pen angleH coef	pen angleV coef	tracks sync on/off	current drawing track	current video track	track0 decay sign	track 1 decay sign	track0 decay	track1 decay	track0 weight	track1 weight	track0 weight	track1 weight	track0 replay on/off	track1 replay on/off	source_track0 record on/off	source_track1 record on/off	auto_beat	beat duration	video_onOff	video_inverted	video threshold	video gamma	no vs initial Vs Current BG Subtraction	image jitter amplitude	mask jiter amplitude	image weights random	image weights scale	image swap duration	image swap frequency	text swap duration	text swap frequency
enum InterpVarIDs{ 
  _currentDrawingStroke = 0,
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
  _clearLayer,
  _clearAllLayers,
  _invertAllLayers,
  _cursorSize,
  _echo,
  _echoNeg,
  _xy_spectrum_coef,
  _RGBpalette_val_coef,
  _RGBpalette_hue_coef,
  _flashTrack0_freq_1,
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
  _trackdecay_0,
  _trackdecay_1,
  _trackCompositionWeight_0,
  _trackCompositionWeight_1,
  _trackFinalWeight_0,
  _trackFinalWeight_1,
  _track_replay_0,
  _track_replay_1,
  _source_track_record_0,
  _source_track_record_1,
  _auto_beat,
  _beat_duration,
  _is_videoOn,
  _is_videoInvert,
  _videoThreshold,
  _videoGamma,
  _BGSubtr,
  _imageJitterAmpl,
  _maskJitterAmpl,
  _images_weights_rand,
  _images_weights_scale,
  _image_swap_duration,
  _image_swap_freq,
  _text_swap_duration,
  _text_swap_freq,
  _MaxInterpVarIDs};
extern int   currentDrawingStroke;
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
extern bool  clearLayer          ;
extern bool  clearAllLayers      ;
extern bool  invertAllLayers     ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern float xy_spectrum_coef    ;
extern float RGBpalette_val_coef ;
extern float RGBpalette_hue_coef ;
extern int   flashTrack0_freq_1  ;
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
extern float trackdecay_0        ;
extern float trackdecay_1        ;
extern float trackCompositionWeight_0;
extern float trackCompositionWeight_1;
extern float trackFinalWeight_0  ;
extern float trackFinalWeight_1  ;
extern bool  track_replay_0      ;
extern bool  track_replay_1      ;
extern bool  source_track_record_0;
extern bool  source_track_record_1;
extern bool  auto_beat           ;
extern float beat_duration       ;
extern bool  is_videoOn          ;
extern bool  is_videoInvert      ;
extern float videoThreshold      ;
extern float videoGamma          ;
extern int   BGSubtr             ;
extern float imageJitterAmpl     ;
extern float maskJitterAmpl      ;
extern float images_weights_rand ;
extern float images_weights_scale;
extern float image_swap_duration ;
extern int   image_swap_freq     ;
extern float text_swap_duration  ;
extern int   text_swap_freq      ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
