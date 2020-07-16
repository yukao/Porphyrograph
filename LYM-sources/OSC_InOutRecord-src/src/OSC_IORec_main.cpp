/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <algorithm>
using std::min;
using std::max;
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <math.h>       /* sqrt */
#include <time.h>       /* sqrt */

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <netdb.h>
#include <unistd.h>
#else // !_WIN32
//#define socklen_t int
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <wspiapi.h>
#endif // _WIN32
// \}
#include <conio.h>

#include "lo/lo.h"
#include "OSC_IORec_UDPClientServer.h"
#include "OSC_IORec_main.h"

Activity  currentActivity;

FILE *FILE_record = NULL;
std::ifstream FILE_ReplayCommand;

bool isReplayOn = false;
double InitialReplayTime = _NON_INITIALIZED_INITIAL_TIME;
double InitialPauseTime = _NON_INITIALIZED_INITIAL_TIME;
double InitialRecordTime = _NON_INITIALIZED_INITIAL_TIME;

std::string ReplayCommandFile_line = std::string("");
double ReplayCommand_timeStamp = 0.;
std::string ReplayCommand_commandString = std::string("");
std::string ReplayCommand_OSCaddress = std::string("");

// non-blocking get char
int getch_noblock() {
	if (_kbhit()) {
		char c = _getch();
		return int(c);
	}
	return 0;
}

/** Cross-platform sleep function */
void millisleep(int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

bool consoleHandler(int signal) {
	if (signal == CTRL_C_EVENT) {
		if (currentActivity == replayOSC) {
			FILE_ReplayCommand.close();
			printf("Exiting replay (Ctrl C)\n");
		}
		else if (currentActivity == recordOSC) {
			fclose(FILE_record);
			printf("Exiting record (Ctrl C)\n");
		}
		exit(0);
	}
	return true;
}
int main(int argc, char** argv) {
	int currentConsoleChar = 0;

	if (argc != 4 && argc != 8) {
		printf("Incorrect number of arguments %d. Usage OSC_RecordReplay.exe <replay|record> (<Porphyrograph_client_IP_adress> <Porphyrograph_client_Port_Number> <Usine_client_IP_adress> <Usine_client_Port_Number>|<local_server_port>) <file_name>\n", argc);
		exit(0);
	}

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);;

	if (strcmp(argv[1], "replay") == 0 && argc == 8) {
		Porphyrograph_Replay_client = new pg_IPClient(argv[2], atoi(argv[3]));
		Porphyrograph_Replay_client->InitClient();
		Usine_Replay_client = new pg_IPClient(argv[4], atoi(argv[5]));
		Usine_Replay_client->InitClient();
		TouchOSC_Replay_server = new pg_IPServer(atoi(argv[6]));
		TouchOSC_Replay_server->InitServer();
		currentActivity = replayOSC;
		FILE_ReplayCommand = std::ifstream(argv[7]);
		if (!FILE_ReplayCommand) {
			sprintf_s(ErrorStr, string_length, "Error: file [%s] not found!", argv[7]); ReportError(ErrorStr); throw 11;
		}
	}
	else if (strcmp(argv[1], "record") == 0 && argc == 4) {
		Leap_porphyrograph_Record_server = new pg_IPServer(atoi(argv[2]));
		Leap_porphyrograph_Record_server->InitServer();
		currentActivity = recordOSC;

		std::string fileNameString = std::string(argv[3]);
		// current date/time based on current system
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		std::stringstream date_stringStream;
		date_stringStream << std::setfill('0') << std::setw(2) << 1 + ltm.tm_mon << "-"
			<< std::setfill('0') << std::setw(2) << ltm.tm_mday << "-"
			<< std::setfill('0') << std::setw(2) << (1900 + ltm.tm_year) << "_"
			<< std::setfill('0') << std::setw(2) << ltm.tm_hour << "-"
			<< std::setfill('0') << std::setw(2) << ltm.tm_min << "-"
			<< ltm.tm_year - 100 << std::flush;
		size_t pos_dot = fileNameString.find_last_of('.');
		if (pos_dot != std::string::npos) {
			fileNameString.erase(pos_dot, fileNameString.length() - pos_dot);
		}
		fileNameString = fileNameString + "_" + date_stringStream.str() + ".txt";

		fopen_s(&FILE_record, fileNameString.c_str(), "w");
		if (!FILE_record) {
			printf("Recording file %s not opened\n", fileNameString.c_str());
			exit(0);
		}
		else {
			printf("Recording file %s\n", fileNameString.c_str());
		}
	}
	else {
		printf("incorrect argument pattern. Usage Usage OSC_RecordReplay.exe <replay|record> (<Porphyrograph_client_IP_adress> <Porphyrograph_client_Port_Number> <Usine_client_IP_adress> <Usine_client_Port_Number>|<local_server_port>) <file_name>\n");
		exit(0);
	}

	long frameCount = 0;
	for (;;) {
		if (currentActivity == replayOSC) {
			// play/pause/quit
			if ((currentConsoleChar = getch_noblock())) {
				switch (currentConsoleChar) {
					case int(' ') :
						// playing in replay mode 
						if (!isReplayOn) {
							isReplayOn = true;
							if (InitialReplayTime == _NON_INITIALIZED_INITIAL_TIME) {
								InitialReplayTime = RealTime();
							}
							else if (InitialPauseTime != _NON_INITIALIZED_INITIAL_TIME) {
								InitialReplayTime = InitialReplayTime + (RealTime() - InitialPauseTime);
								InitialPauseTime = _NON_INITIALIZED_INITIAL_TIME;
							}
						}
						// pausing in replay mode 
						else {
							isReplayOn = false;
							InitialPauseTime = RealTime();
						}
						break;
					case int('q') :
					case int('Q') :
						// quitting in replay mode 
						printf("Quitting replay (q/Q command received)\n");
						Porphyrograph_Replay_client->sendIPmessages("/quit", "", NULL, NULL, NULL);
						for (int i = 0; i < 4; i++) {
							std::string address = std::string("/new_scene_") + std::to_string(i);
							Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
							millisleep(5);
						}
						exit(0);
						break;
					case int('1') :
					case int('2') :
					case int('3') :
					case int('4') : 
						{
							int scene_no = int(currentConsoleChar - '0' - 1);
							printf("Usine scene %d\n", scene_no);
							for (int i = 0; i < 4; i++) {
								std::string address = std::string("/new_scene_") + std::to_string(i);
								if (i == scene_no) {
									Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 1);
								}
								else {
									Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
								}
								millisleep(5);
							}
						}
						break;
					case int('0') :
						for (int i = 0; i < 4; i++) {
							std::string address = std::string("/new_scene_") + std::to_string(i);
							Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
							millisleep(5);
						}
						break;
					case int('+') :
						InitialPauseTime = ReplayCommand_timeStamp;
						while (ReplayCommand_OSCaddress.compare("/new_scene") != 0) {
							std::getline(FILE_ReplayCommand, ReplayCommandFile_line);
							std::stringstream  sstrem(ReplayCommandFile_line);
							// time stamp
							sstrem >> ReplayCommand_commandString;
							ReplayCommand_timeStamp = atof(ReplayCommand_commandString.c_str());
							// address
							sstrem >> ReplayCommand_OSCaddress;
						}
						InitialReplayTime = InitialReplayTime - (ReplayCommand_timeStamp - InitialPauseTime);
						break;
				}
			}

			// OSC reception
			// possible TouchOSC controls
			while (lo_server_recv_noblock(TouchOSC_Replay_server->lo_local_server, 0)) {
				if (frameCount % 1000 == 0) {
					printf("%s\n", TouchOSC_Replay_server->Input_Message_Local_Commande_String);
				}
			}

			// recorded commands forwarding to porphyrograph and Usine
			// string is not empty time stamp has already been calculated
			if (isReplayOn) {
				do {
					if (ReplayCommandFile_line.length() == 0) {
						std::getline(FILE_ReplayCommand, ReplayCommandFile_line);
						std::stringstream  sstrem(ReplayCommandFile_line);
						// time stamp
						sstrem >> ReplayCommand_commandString;
						ReplayCommand_timeStamp = atof(ReplayCommand_commandString.c_str());
						// address
						sstrem >> ReplayCommand_OSCaddress;
					}
					// printf("1st command line %s\n", ReplayCommandFile_line.c_str());

					if (ReplayCommand_timeStamp <= RealTime() - InitialReplayTime && ReplayCommandFile_line.length() != 0) {
						// dispatches the message between both clients depending on the OSC address
						if (ReplayCommand_OSCaddress.compare("/abs_pen_xy") == 0
							|| ReplayCommand_OSCaddress.compare("/new_scene") == 0) {
							// printf("line for porphyrograph %s\n", ReplayCommandFile_line.c_str());
							Porphyrograph_Replay_client->sendOSCMessageFromLine(ReplayCommandFile_line);
							// sends a message different to Usine: /new_scene_0, /new_scene_1, /new_scene_2... 
							if (ReplayCommand_OSCaddress.compare("/new_scene") == 0) {
								std::string argt = std::string("");
								// address
								std::stringstream  sstrem(ReplayCommandFile_line);
								// time stamp
								sstrem >> argt;
								// address
								sstrem >> argt;
								// nb argts
								sstrem >> argt;
								// pattern
								sstrem >> argt;
								// scene no
								sstrem >> argt;
								int scene_no = atoi(argt.c_str());
								for (int i = 0; i < 4; i++ ) {
									std::string address = std::string("/new_scene_") + std::to_string(i);
									if (i == scene_no) {
										//printf("Usine receives %s 1\n", address.c_str());
										Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 1);
									}
									else {
										//printf("Usine receives %s 0\n", address.c_str());
										Usine_Replay_client->sendIPmessages(address.c_str(), "i", NULL, NULL, 0);
									}
									millisleep(5);
								}
							}
						}
						// quitting in replay mode -> causes replay looping
						// quit is not transmitted so that the replaying apps (Usine and porphyrograph) do not stop
						else if (ReplayCommand_OSCaddress.compare("/quit") == 0) {
							// printf("Rewinding replay (quit command received)\n");
							FILE_ReplayCommand.clear();
							FILE_ReplayCommand.seekg(0);
							InitialReplayTime = RealTime();
						}
						else if (ReplayCommand_OSCaddress.compare("/leap_motion_left") == 0
							|| ReplayCommand_OSCaddress.compare("/leap_motion_right") == 0) {
							// printf("line for Usine %s\n", ReplayCommandFile_line.c_str());
							Usine_Replay_client->sendOSCMessageFromLine(ReplayCommandFile_line);
						}

						std::getline(FILE_ReplayCommand, ReplayCommandFile_line);
						std::stringstream  sstrem(ReplayCommandFile_line);
						// time stamp
						sstrem >> ReplayCommand_commandString;
						ReplayCommand_timeStamp = atof(ReplayCommand_commandString.c_str());
						// address
						sstrem >> ReplayCommand_OSCaddress;
					}
					else {
						break;
					}
				} while (ReplayCommand_timeStamp <= RealTime() - InitialReplayTime && !FILE_ReplayCommand.eof());

				if (FILE_ReplayCommand.eof()) {                      // check for EOF
					printf("Rewinding replay (end of command file reached)\n");
					FILE_ReplayCommand.clear();
					FILE_ReplayCommand.seekg(0);
					InitialReplayTime = RealTime();
				}
			}
		}
		// records the contatenated line
		else if (currentActivity == recordOSC) {
			while (lo_server_recv_noblock(Leap_porphyrograph_Record_server->lo_local_server, 0)) {
				if (frameCount % 1000 == 0) 
				{
					printf("%s\n", Leap_porphyrograph_Record_server->Input_Message_Local_Commande_String);
				}
			}
		}
		millisleep(5);
		frameCount++;
		// printf("Frame count %d\n", frameCount);
	} 
	// non  ctrl-c to exit
	if (currentActivity == replayOSC) {
		FILE_ReplayCommand.close();
		printf("Exiting replay (end of file)\n");
	}
	else if (currentActivity == recordOSC) {
		fclose(FILE_record);
		printf("Exiting record\n");
	}
	return 0;
}
//End-of-Sample
