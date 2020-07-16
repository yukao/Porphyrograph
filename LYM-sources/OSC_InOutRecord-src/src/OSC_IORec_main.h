void millisleep(int milliseconds);
enum Activity { recordOSC = 0, replayOSC };
extern Activity  currentActivity;
extern bool isReplayOn;
#define _NON_INITIALIZED_INITIAL_TIME -1
extern double InitialReplayTime;
extern double InitialPauseTime;
extern double InitialRecordTime;
extern std::ifstream FILE_ReplayCommand;
extern std::string ReplayCommandFile_line;
extern double ReplayCommand_timeStamp;
extern std::string ReplayCommand_commandString;
extern std::string ReplayCommand_OSCaddress;
