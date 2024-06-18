// CONFIGURATION CONSTANTS
// RANK,0,1,2,3,4,5,6,7,8,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
// VERBATIM,double window,wide screen,window width,window height,window x,window y,minimal interframe latency,time scale,initial time,message pixel length,trace output frame number,trace time,first frame number,last frame number,max mouse recording frames (for track drawing reording),max network message length,nb of particles,nb types of CA,nb of tracks,nb of paths,projet name,nb of configs,nb CA parameters,nb Master Masks,nb ClipArt layers
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
#define PG_NB_TRACKS                   4         
#define PG_NB_PATHS                    12        
constexpr auto project_name                   = "Ada";
#define PG_MAX_CONFIGURATIONS          10        
#define PG_NB_CA_PARAMS                8         
#define PG_NB_MASTER_MASKS             6         
#define PG_NB_CLIPART_LAYERS           8         
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
#define var_PG_NB_TRACKS
#define var_PG_NB_PATHS
#define var_project_name
#define var_PG_MAX_CONFIGURATIONS
#define var_PG_NB_CA_PARAMS
#define var_PG_NB_MASTER_MASKS
#define var_PG_NB_CLIPART_LAYERS
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
  _PG_NB_TRACKS,
  _PG_NB_PATHS,
  _project_name,
  _PG_MAX_CONFIGURATIONS,
  _PG_NB_CA_PARAMS,
  _PG_NB_MASTER_MASKS,
  _PG_NB_CLIPART_LAYERS,
  _MaxConfigurationVarIDs};
