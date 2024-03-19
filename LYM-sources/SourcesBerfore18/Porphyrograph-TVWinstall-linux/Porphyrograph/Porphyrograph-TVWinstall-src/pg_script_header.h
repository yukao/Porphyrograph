// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16
// VERBATIM	double window	wide screen	window width	window height	window x for 1360x768	window y	minimal interframe latency	time scale	initial time	camera ID	message pixel length	trace output frame number	trace time	first frame number	last frame number	max mouse recording frames (for track drawing reording)	max network message length
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
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_path };
extern VarTypes ConfigurationVarTypes[_MaxConfigurationVarIDs];
extern void * ConfigurationVarPointers[_MaxConfigurationVarIDs];
// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63
// VERBATIM	brush radius	palette 	Gray color	Blend Transparency	part mode	part acc	part texture	part acc center-x	part acc center-y	radius particle	repopuplating track0	clear all Layers	echo	echo neg	color music (coef)	color music (const)	track1 Flash track0 freq	Flash part freq	Flash part duration	pen pressure coef	pen angleH coef	pen angleV coef	track0 decay sign	track0 decay	track0 weight	track1 weight	track0 weight	track1 weight	auto_beat	video_onOff	video threshold	video gamma	video exposure	video flash length (in frames)	video flash bright (in frames)	video flash beat no	no vs initial Vs Current BG Subtraction	CA type	CA subtype	clear CA	track0 Flash CA freq	track1 Flash CA freq	Flash back freq	CA weight	CA weight	CA decay sign	CA decay	image weights random	image weights scale	mask jitter amplitude	image jitter amplitude	image value	image value pulse	image satur	image satur pulse	mask contrast	mask threshold	mask variance	image contrast	freeze	input volume	subscene duration	image_swap_duration	image_swap_freq
enum InterpVarIDs{ 
  _brushRadius = 0,
  _RGBpalette,
  _local_color_gray,
  _master,
  _particleMode,
  _part_acc_factor,
  _particle_texture_ID,
  _partAccCenter_0,
  _partAccCenter_1,
  _radiusParticle,
  _repopulatingBG,
  _clearAllLayers,
  _echo,
  _echoNeg,
  _RGBpalette_val_coef,
  _RGBpalette_val_const,
  _flashTrack0_freq,
  _flashPart_freq,
  _flashPart_duration,
  _brush_radius_pressure_coef,
  _brush_radius_angleHor_coef,
  _brush_radius_angleVer_coef,
  _trackdecay_sign_0,
  _trackdecay_0,
  _trackCompositionWeight_0,
  _trackCompositionWeight_1,
  _trackFinalWeight_0,
  _trackFinalWeight_1,
  _auto_beat,
  _is_videoOn,
  _videoThreshold,
  _videoGamma,
  _videoExposure,
  _flashVideoLength,
  _flashVideoBright,
  _flashVideoBeat,
  _camera_BG_subtr,
  _CAType,
  _CASubType,
  _clearCA,
  _flashCA_freq_0,
  _flashCA_freq_1,
  _flashBack_freq,
  _CACompositionWeight,
  _CAFinalWeight,
  _CAdecay_sign,
  _CAdecay,
  _images_weights_rand,
  _images_weights_scale,
  _maskJitterAmpl,
  _imageJitterAmpl,
  _image_value,
  _image_value_pulse,
  _image_satur,
  _image_satur_pulse,
  _mask_contrast,
  _mask_threshold,
  _mask_variance,
  _image_contrast,
  _freeze,
  _input_volume,
  _subscene_duration,
  _image_swap_duration,
  _image_swap_freq,
  _MaxInterpVarIDs};
extern float brushRadius         ;
extern float RGBpalette          ;
extern float local_color_gray    ;
extern float master         ;
extern int   particleMode        ;
extern float part_acc_factor     ;
extern float particle_texture_ID ;
extern float partAccCenter_0     ;
extern float partAccCenter_1     ;
extern float radiusParticle      ;
extern float repopulatingBG      ;
extern bool  clearAllLayers      ;
extern float echo                ;
extern float echoNeg             ;
extern float RGBpalette_val_coef ;
extern float RGBpalette_val_const;
extern int   flashTrack0_freq    ;
extern int   flashPart_freq      ;
extern int   flashPart_duration  ;
extern float brush_radius_pressure_coef;
extern float brush_radius_angleHor_coef;
extern float brush_radius_angleVer_coef;
extern float trackdecay_sign_0   ;
extern float trackdecay_0        ;
extern float trackCompositionWeight_0;
extern float trackCompositionWeight_1;
extern float trackFinalWeight_0  ;
extern float trackFinalWeight_1  ;
extern bool  auto_beat           ;
extern bool  is_videoOn          ;
extern float videoThreshold      ;
extern float videoGamma          ;
extern float videoExposure       ;
extern float flashVideoLength    ;
extern float flashVideoBright    ;
extern int   flashVideoBeat      ;
extern int   camera_BG_subtr             ;
extern int   CAType              ;
extern int   CASubType           ;
extern bool  clearCA             ;
extern int   flashCA_freq_0      ;
extern int   flashCA_freq_1      ;
extern int   flashBack_freq      ;
extern float CACompositionWeight ;
extern float CAFinalWeight       ;
extern float CAdecay_sign        ;
extern float CAdecay             ;
extern float images_weights_rand ;
extern float images_weights_scale;
extern float maskJitterAmpl      ;
extern float imageJitterAmpl     ;
extern float image_value         ;
extern float image_value_pulse   ;
extern float image_satur         ;
extern float image_satur_pulse   ;
extern float mask_contrast       ;
extern float mask_threshold      ;
extern float mask_variance       ;
extern float image_contrast      ;
extern bool  freeze              ;
extern float input_volume        ;
extern int   subscene_duration   ;
extern float image_swap_duration ;
extern int   image_swap_freq     ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
