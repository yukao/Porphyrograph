// RANK	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62
// VERBATIM	drawing stroke	brush ID	brush radius	scale	palette 	R color	G color	B color	A color	Blend Transparency	part mode	part acc	part texture	part acc center-x	part acc center-y	CA type	CA subtype	CA1 type-Subtype	CA2 type-Subtype	CA3 type-Subtype	CA1/CA2 weight	CA update step	clear	clearCA	invert	cursor size	echo	echo neg	xy music	color music	brush radius music	part music	xy music replay	color music replay	brush radius music replay	track0 Flash CA freq	Flash back freq	Flash part freq	track0 Flash CA on/off	Flash back on/off	Flash part on/off	Flash part duration	track0 Flash CA track weight	FlashBack track weight	pen pressure coef	pen angleH coef	pen angleV coef	tracks sync on/off	current track	CA decay sign	track0 decay sign	CA decay	track0 decay	CA weight	track0 weight	track0 replay on/off	source_track0 record on/off	auto_beat	sample setUp	tempo clock	sensor spatial pattern	sensor activation	Supercollider volume
enum InterpVarIDs{ 
  _currentDrawingStroke = 0,
  _brushID,
  _brushRadius,
  _scale,
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
  _CA1TypeSubType,
  _CA2TypeSubType,
  _CA3TypeSubType,
  _CA1_CA2_weight,
  _CAstep,
  _clearAllLayers,
  _clearCA,
  _invertAllLayers,
  _cursorSize,
  _echo,
  _echoNeg,
  _xy_spectrum_coef,
  _color_spectrum_coef,
  _brush_radius_vol_coef,
  _part_acc_attack_coef,
  _xy_spectrum_replay_coef,
  _color_spectrum_replay_coef,
  _brush_radius_vol_replay_coef,
  _flashCA_freq_0,
  _flashBack_freq,
  _flashPart_freq,
  _flashCA_onOff_0,
  _flashBack_onOff,
  _flashPart_onOff,
  _flashPart_duration,
  _flashCA_coef_0,
  _flashBack_coef,
  _brush_radius_pressure_coef,
  _brush_radius_angleHor_coef,
  _brush_radius_angleVer_coef,
  _tracksSync,
  _currentTrack,
  _CAdecay_sign,
  _trackdecay_sign_0,
  _CAdecay,
  _trackdecay_0,
  _CAweight,
  _trackweight_0,
  _track_replay_0,
  _source_track_record_0,
  _auto_beat,
  _sample_setUp,
  _TempoClock,
  _sensor_layout,
  _sensor_activation,
  _volSC,
  _MaxInterpVarIDs};
extern int   currentDrawingStroke;
extern int   brushID             ;
extern float brushRadius         ;
extern float scale               ;
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
extern int   CA1TypeSubType      ;
extern int   CA2TypeSubType      ;
extern int   CA3TypeSubType      ;
extern float CA1_CA2_weight      ;
extern int   CAstep              ;
extern bool  clearAllLayers      ;
extern bool  clearCA             ;
extern bool  invertAllLayers     ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern float xy_spectrum_coef    ;
extern float color_spectrum_coef ;
extern float brush_radius_vol_coef;
extern float part_acc_attack_coef;
extern float xy_spectrum_replay_coef;
extern float color_spectrum_replay_coef;
extern float brush_radius_vol_replay_coef;
extern int   flashCA_freq_0      ;
extern int   flashBack_freq      ;
extern int   flashPart_freq      ;
extern bool  flashCA_onOff_0     ;
extern bool  flashBack_onOff     ;
extern bool  flashPart_onOff     ;
extern int   flashPart_duration  ;
extern float flashCA_coef_0      ;
extern float flashBack_coef      ;
extern float brush_radius_pressure_coef;
extern float brush_radius_angleHor_coef;
extern float brush_radius_angleVer_coef;
extern bool  tracksSync          ;
extern int   currentTrack        ;
extern float CAdecay_sign        ;
extern float trackdecay_sign_0   ;
extern float CAdecay             ;
extern float trackdecay_0        ;
extern float CAweight            ;
extern float trackweight_0       ;
extern bool  track_replay_0      ;
extern bool  source_track_record_0;
extern bool  auto_beat           ;
extern float sample_setUp        ;
extern int   TempoClock          ;
extern int   sensor_layout       ;
extern int   sensor_activation   ;
extern float volSC               ;
enum VarTypes { _pg_bool = 0 , _pg_int , _pg_float , _pg_sign , _pg_track };
