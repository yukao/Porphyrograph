// RANK	0	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18
// VERBATIM	double window	Flat view	wide screen	window width	window height	window x	window y	minimal interframe latency	time scale	initial time	input volume	camera ID	video threshold	message pixel length	trace output frame number	trace time	first frame number	last frame number	max mouse recording frames (for track drawing reording)	max network message length
enum ConfigurationVarIDs{ 
  _double_window = 0,
  _flat_view,
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
extern bool  flat_view           ;
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
// RANK	1	2	3	4	5	6	7	8	9			1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60	61	62	63	64	65	66	67	68	69	70	71	72	73	74	75	76	77	78	79	80	81	82	83	84	85	86	87
// VERBATIM	part mode	part texture	part texture noise	part acc coef	part acc center-x	part acc center-y	radius particle	breathing	repopuplating	repopuplating CA	hidingCoef	Hiding target X	Hiding target Y	drawing stroke	brush ID	brush radius	impact period	impact random	impact_X	impact_Y	palette 	R color	G color	B color	A color	CA type	CA subtype	clear	invert	cursor size	echo	echo neg	color music	part music	brush radius music	CA decay sign (+decay -incay)	track0 decay sign (+decay -incay)	CA decay	track0 decay	CA weight	track0 weight	repopuplating image	display lookAt target	with_mesh	with_mesh	with_whiteText	scene dimming	Right VP location	Right VP location	Right VP location	Right VP up 2nd comp	Right reversed	projLookAtX Right	projLookAtY Right	projLookAtZ Right	projection width top at 1m	projection width bottom at 1m	projection top at 1m	projection bottom at 1m	projKeystoneXTopLeft	projKeystoneXBottomLeft	projKeystoneYTopLeft	projKeystoneYBottomLeft	projKeystoneXTopRight	projKeystoneXBottomRight	projKeystoneYTopRight	projKeystoneYBottomRight	Left VP location	Left VP location	Left VP location	Left VP up 2nd comp	Left reversed	proj2LookAtX Left	proj2LookAtY Left	proj2LookAtZ Left	projection width top at 1m	projection width bottom at 1m	projection top at 1m	projection bottom at 1m	proj2KeystoneXTopLeft	proj2KeystoneXBottomLeft	proj2KeystoneYTopLeft	proj2KeystoneYBottomLeft	proj2KeystoneXTopRight	proj2KeystoneXBottomRight	proj2KeystoneYTopRight	proj2KeystoneYBottomRight	texture width min	texture width max	texture height min	texture height max	texture width min border	texture width max border	texture height min border	texture height max border	mesh junction point	texture width min border	near plane	far plane
enum InterpVarIDs{ 
  _particleMode = 0,
  _particle_texture_ID,
  _particle_texture_noise,
  _partAccCoef,
  _partAccCenterX,
  _partAccCenterY,
  _radiusParticle,
  _breathing,
  _repopulating,
  _repopulatingCA,
  _hidingCoef,
  _hidingX,
  _hidingY,
  _currentDrawingStroke,
  _brushID,
  _brushRadius,
  _impact_period,
  _impact_random,
  _impactX,
  _impactY,
  _RGBspectrum,
  _local_color_r,
  _local_color_g,
  _local_color_b,
  _local_color_a,
  _CAType,
  _CASubType,
  _clearAllLayers,
  _invertAllLayers,
  _cursorSize,
  _echo,
  _echoNeg,
  _color_spectrum_coef,
  _part_acc_attack_coef,
  _brush_radius_vol_coef,
  _CAdecay_sign,
  _trackdecay_sign_0,
  _CAdecay,
  _trackdecay_0,
  _CAweight,
  _trackweight_0,
  _repopulatingImage,
  _isDisplayLookAt,
  _with_mesh,
  _with_blue,
  _with_whiteText,
  _master,
  _projLocX,
  _projLocY,
  _projLocZ,
  _projUpY,
  _projReversed,
  _projLookAtX,
  _projLookAtY,
  _projLookAtZ,
  _projWidthTopAt1m,
  _projWidthBottomAt1m,
  _projTopAt1m,
  _projBottomAt1m,
  _projKeystoneXTopLeft,
  _projKeystoneXBottomLeft,
  _projKeystoneYTopLeft,
  _projKeystoneYBottomLeft,
  _projKeystoneXTopRight,
  _projKeystoneXBottomRight,
  _projKeystoneYTopRight,
  _projKeystoneYBottomRight,
  _proj2LocX,
  _proj2LocY,
  _proj2LocZ,
  _proj2UpY,
  _proj2Reversed,
  _proj2LookAtX,
  _proj2LookAtY,
  _proj2LookAtZ,
  _proj2WidthTopAt1m,
  _proj2WidthBottomAt1m,
  _proj2TopAt1m,
  _proj2BottomAt1m,
  _proj2KeystoneXTopLeft,
  _proj2KeystoneXBottomLeft,
  _proj2KeystoneYTopLeft,
  _proj2KeystoneYBottomLeft,
  _proj2KeystoneXTopRight,
  _proj2KeystoneXBottomRight,
  _proj2KeystoneYTopRight,
  _proj2KeystoneYBottomRight,
  _textureFrontier_wmin,
  _textureFrontier_wmax,
  _textureFrontier_hmin,
  _textureFrontier_hmax,
  _textureFrontier_wmin_width,
  _textureFrontier_wmax_width,
  _textureFrontier_hmin_width,
  _textureFrontier_hmax_width,
  _meshFrontier_X,
  _meshFrontier_width,
  _nearPlane,
  _farPlane,
  _MaxInterpVarIDs};
extern int   particleMode        ;
extern float particle_texture_ID ;
extern float particle_texture_noise;
extern float partAccCoef         ;
extern float partAccCenterX      ;
extern float partAccCenterY      ;
extern float radiusParticle      ;
extern float breathing           ;
extern float repopulating        ;
extern float repopulatingCA      ;
extern float hidingCoef          ;
extern float hidingX             ;
extern float hidingY             ;
extern int   currentDrawingStroke;
extern int   brushID             ;
extern float brushRadius         ;
extern int   impact_period       ;
extern float impact_random       ;
extern float impactX             ;
extern float impactY             ;
extern float RGBspectrum         ;
extern float local_color_r       ;
extern float local_color_g       ;
extern float local_color_b       ;
extern float local_color_a       ;
extern int   CAType              ;
extern int   CASubType           ;
extern float clearAllLayers      ;
extern float invertAllLayers     ;
extern int   cursorSize          ;
extern float echo                ;
extern float echoNeg             ;
extern float color_spectrum_coef ;
extern float part_acc_attack_coef;
extern float brush_radius_vol_coef;
extern float CAdecay_sign        ;
extern float trackdecay_sign_0   ;
extern float CAdecay             ;
extern float trackdecay_0        ;
extern float CAweight            ;
extern float trackweight_0       ;
extern float repopulatingImage   ;
extern float isDisplayLookAt     ;
extern float with_mesh           ;
extern float with_blue           ;
extern float with_whiteText      ;
extern float blendTransp         ;
extern float projLocX            ;
extern float projLocY            ;
extern float projLocZ            ;
extern float projUpY             ;
extern float projReversed        ;
extern float projLookAtX         ;
extern float projLookAtY         ;
extern float projLookAtZ         ;
extern float projWidthTopAt1m    ;
extern float projWidthBottomAt1m ;
extern float projTopAt1m         ;
extern float projBottomAt1m      ;
extern float projKeystoneXTopLeft;
extern float projKeystoneXBottomLeft;
extern float projKeystoneYTopLeft;
extern float projKeystoneYBottomLeft;
extern float projKeystoneXTopRight;
extern float projKeystoneXBottomRight;
extern float projKeystoneYTopRight;
extern float projKeystoneYBottomRight;
extern float proj2LocX           ;
extern float proj2LocY           ;
extern float proj2LocZ           ;
extern float proj2UpY            ;
extern float proj2Reversed       ;
extern float proj2LookAtX        ;
extern float proj2LookAtY        ;
extern float proj2LookAtZ        ;
extern float proj2WidthTopAt1m   ;
extern float proj2WidthBottomAt1m;
extern float proj2TopAt1m        ;
extern float proj2BottomAt1m     ;
extern float proj2KeystoneXTopLeft;
extern float proj2KeystoneXBottomLeft;
extern float proj2KeystoneYTopLeft;
extern float proj2KeystoneYBottomLeft;
extern float proj2KeystoneXTopRight;
extern float proj2KeystoneXBottomRight;
extern float proj2KeystoneYTopRight;
extern float proj2KeystoneYBottomRight;
extern float textureFrontier_wmin;
extern float textureFrontier_wmax;
extern float textureFrontier_hmin;
extern float textureFrontier_hmax;
extern float textureFrontier_wmin_width;
extern float textureFrontier_wmax_width;
extern float textureFrontier_hmin_width;
extern float textureFrontier_hmax_width;
extern float meshFrontier_X      ;
extern float meshFrontier_width  ;
extern float nearPlane           ;
extern float farPlane            ;
extern VarTypes ScenarioVarTypes[_MaxInterpVarIDs];
extern void * ScenarioVarPointers[_MaxInterpVarIDs];
extern char *ScenarioVarMessages[_MaxInterpVarIDs];
extern char *CmdString[_MaxInterpVarIDs];
