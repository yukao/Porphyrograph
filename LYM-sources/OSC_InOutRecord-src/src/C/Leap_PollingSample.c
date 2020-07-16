/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>


#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <math.h>       /* sqrt */


#include "LeapC.h"
#include "Leap_Connection.h"

int64_t lastFrameID = 0; //The last frame received
void init_LeapMotion_clients_udp(char *Usine_IPaddress, unsigned int Usine_PortNumber, char *OSCRecord_IPaddress, unsigned int OSCRecord_PortNumber);
void send_message_upd(char *address, char *format, float float_argts[]);

int main(int argc, char** argv) {

	if (argc != 5) {
		printf("Usage Leap_OSC.exe <Usine_client_IP_adress> <Usine_client_Port_Number> <OSCRecord_client_IP_adress> <OSCRecord_client_Port_Number>\n");
		exit(0);
	}

  OpenConnection();
  while(!IsConnected)
    millisleep(100); //wait a bit to let the connection complete

  printf("Connected.\n");
  LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
  if(deviceProps)
    printf("Using device %s.\n", deviceProps->serial);

  init_LeapMotion_clients_udp(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));

  long frameCount = 0;
  for(;;){
    LEAP_TRACKING_EVENT *frame = GetFrame();
	if(frame && (frame->tracking_frame_id > lastFrameID)){
      lastFrameID = frame->tracking_frame_id;
	  //if (frameCount % 10 == 0)
	  //{
		 // printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
	  //}
      for(uint32_t h = 0; h < frame->nHands; h++){
        LEAP_HAND* hand = &frame->pHands[h];
		float velocity = sqrtf(hand->palm.velocity.x*hand->palm.velocity.x + hand->palm.velocity.y*hand->palm.velocity.y + hand->palm.velocity.z*hand->palm.velocity.z);
		if (velocity > 100000) {
			velocity = 0.f;
		}
#ifdef LEAPMOTION_FULLDATA
		float float_argts[19] = {
			hand->palm.position.x/1000.f,
			hand->palm.position.y/1000.f,
			hand->palm.position.z/1000.f,
			hand->thumb.stabilized_tip_position.x/1000.f,
			hand->thumb.stabilized_tip_position.y/1000.f,
			hand->thumb.stabilized_tip_position.z/1000.f,
			hand->index.stabilized_tip_position.x/1000.f,
			hand->index.stabilized_tip_position.y/1000.f,
			hand->index.stabilized_tip_position.z/1000.f,
			hand->middle.stabilized_tip_position.x/1000.f,
			hand->middle.stabilized_tip_position.y/1000.f,
			hand->middle.stabilized_tip_position.z/1000.f,
			hand->ring.stabilized_tip_position.x/1000.f,
			hand->ring.stabilized_tip_position.y/1000.f,
			hand->ring.stabilized_tip_position.z/1000.f,
			hand->pinky.stabilized_tip_position.x/1000.f,
			hand->pinky.stabilized_tip_position.y/1000.f,
			hand->pinky.stabilized_tip_position.z/1000.f,
			velocity/1000.f
		};
		if (frameCount % 1000 == 0) 
		{
			printf("hand %s palm velocity %.2f palm (%.2f, %.2f, %.2f) index (%.2f, %.2f, %.2f)\n",
				(hand->type == eLeapHandType_Left? "left": "right"),
				velocity/1000.f,
				hand->palm.position.x/1000.f,
				hand->palm.position.y/1000.f,
				hand->palm.position.z/1000.f,
				hand->index.stabilized_tip_position.x/1000.f,
				hand->index.stabilized_tip_position.y/1000.f,
				hand->index.stabilized_tip_position.z/1000.f);
		}
		if (hand->type == eLeapHandType_Left) {
			send_message_upd("/leap_motion_left", "fffffffffffffffffff", float_argts);
		}
		else {
			send_message_upd("/leap_motion_right", "fffffffffffffffffff", float_argts);
		}
#else
		float float_argts[4] = {
			hand->palm.position.x / 1000.f,
			hand->palm.position.y / 1000.f,
			hand->palm.position.z / 1000.f,
			velocity / 1000.f
		};
		if (frameCount % 1000 == 0)
		{
			printf("hand %s palm (%.2f, %.2f, %.2f) palm velocity %.2f\n",
				(hand->type == eLeapHandType_Left ? "left" : "right"),
				hand->palm.position.x / 1000.f,
				hand->palm.position.y / 1000.f,
				hand->palm.position.z / 1000.f,
				velocity / 1000.f);
		}
		if (hand->type == eLeapHandType_Left) {
			send_message_upd("/leap_motion_left", "ffff", float_argts);
		}
		else {
			send_message_upd("/leap_motion_right", "ffff", float_argts);
		}
#endif
		// send_message_upd("/leap_finger_x", "f", float_argts);
	  }
    }
	frameCount++;
  } //ctrl-c to exit
  return 0;
}
//End-of-Sample
