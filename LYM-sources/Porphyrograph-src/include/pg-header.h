// constants used for table size and c++ non dynamic allocation
constexpr int PG_WINDOW_WIDTH = 1920;
constexpr int PG_WINDOW_HEIGHT = 1080;
constexpr int PG_MESSAGE_PIXEL_LENGTH = 128;
constexpr int PG_MAX_NETWROK_MESSAGE_LENGTH = 65535;
constexpr int PG_NB_PARTICLES = 200000;
constexpr int PG_NB_TRACKS = 4;
constexpr int PG_NB_PATHS = 12;
constexpr int PG_MAX_SCENARIOS = 10;
constexpr int PG_NB_CA_PARAMS = 8;
constexpr int PG_NB_MASTER_MASKS = 6;
constexpr int PG_NB_CLIPART_LAYERS = 8;
constexpr int PG_NB_LIGHTS = 8;
// maximal number of arguments for an OSC command
constexpr int PG_MAX_OSC_ARGUMENTS = 96;
// NB OF POSSIBLE CURSORS THROUGH HAND INTERFACE
constexpr int PG_NB_CURSORS_MAX = 5;
// different clips on right and left screens
constexpr int PG_NB_PARALLEL_CLIPS = 2;
constexpr int PG_NB_SENSORS = 16;
constexpr int PG_NB_MAX_SENSOR_ACTIVATIONS = 6;
constexpr int PG_NB_MAX_SENSOR_SAMPLE_SETUPS = 3;
constexpr int PG_NB_MAX_SENSOR_LAYOUTS = 4;
// max nb of bones in a skeleton (on C++ side and on glsl side in the mesh vertex shader)
constexpr int PG_MAX_BONES = 20;
