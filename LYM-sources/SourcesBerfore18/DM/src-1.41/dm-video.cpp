/*! \file dm-video.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-video.cpp - from VESS http://vess.ist.ucf.edu/
 *     and ffmpeg (ffplay,output_example) http://ffmpeg.sourceforge.net/
 * 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "dm-all_include.h"

#ifdef DM_HAVE_FFMPEG

// dummy external frame so that it compiles 
GLubyte *extern_frame = NULL;

/////////////////////////////////////////////////
// video file opening and intialzations (external video lib)
int extern_open_input_file( int id_live_video ) {
  // TODO: code for file opening, return negative value if error
  printf( "EXTERN VIDEO extern_open_input_file %d\n" , id_live_video );

#ifdef DM_EXTERN_LIVE_VIDEO
  //int nbDevices = 0;
  //char** names = grabberGetDevices( &nbDevices );

  //// printf("Nb devices %d\n" , nbDevices );
  //for( int ind = 0 ; ind < nbDevices ; ind++ ) {
	 // printf( "Device #%d name [%s]\n" , ind , names[ ind ] );
  //}
  //if( id_live_video  < 0 || id_live_video >= nbDevices ) {
  //  sprintf( ErrorStr , "Error: Unknown device no %d (max is %d)!" , id_live_video , nbDevices - 1); ReportError( ErrorStr ); throw 1;
  //}
		
  //grabberStartDevice(filmName,1024,768);
  if( CaptureVideo(1024,768) == S_OK ) {
	  // printf( "Successful device start #%d name [%s]\n" , id_live_video , names[ id_live_video ] );
	  printf( "Successful device start\n" );
  }
  else {
    // sprintf( ErrorStr , "Error: Unsuccessful device start #%d name [%s]\n" , id_live_video , names[ id_live_video ] ); ReportError( ErrorStr ); throw 1;
    sprintf( ErrorStr , "Error: Unsuccessful device start\n" ); ReportError( ErrorStr ); throw 1;
  }
#endif

  return 0;
}

/////////////////////////////////////////////////
// video file closing and stream release
int extern_close_all( void ) {
  // TODO: code for file closing, return negative value if error
  printf( "EXTERN VIDEO extern_close_all\n" );

#ifdef DM_EXTERN_LIVE_VIDEO
  grabberShutdownAll();
#endif

  return 0;
}

/////////////////////////////////////////////////
// thread for buffer filling (external video lib)
void extern_fillBuffer( int id_live_video ) {
  // TODO: code for camera frame copy into external buffer
  // printf( "EXTERN VIDEO extern_extern_fillBuffer %s\n" , filmName );
}

/////////////////////////////////////////////////
// context initializazion: called after video opening
// and used for passing video parameters to DM
void  extern_open_video_context( int *frameWidth , 
				 int *frameHeight ,
				 int *bytesPerPixel ,
				 bool rectangle ,
				 double *frameDuration ) {
  // TODO: code for frame parameters definition
  *frameWidth = 1024;
  *frameHeight = 768;
  *bytesPerPixel = 3;
  *frameDuration = 0.040;

//   printf( "EXTERN VIDEO extern_open_video_context %dx%d %.4f\n" , 
// 	  *frameWidth , *frameWidth , *frameDuration  );

#ifdef DM_EXTERN_LIVE_VIDEO
  grabberGetBitmapDesc(NULL,*frameWidth,*frameHeight,*bytesPerPixel);
#endif

  // TODO: code for external frame allocation and initialization
  // dummy allocation and initialization
  extern_frame = new GLubyte[ (*frameWidth) * (*frameHeight) * (*bytesPerPixel) ];
  for( int ind = 0 ; ind < (*frameWidth) * (*frameHeight) * (*bytesPerPixel) ; ind++ ) {
    extern_frame[ind] = (GLubyte)( ind/(*bytesPerPixel) % 256 );
  }
}

/////////////////////////////////////////////////
// video frame copy: called at each DM frame
// returns a boolean gotPicture whether or not a new frame is copied
// if gotPicture is true, copies the new frame in DM memory 
void extern_decode_video( dm_VideoParameters *videoParam , int id_live_video ,
						  dm_ImageRRGB *rgbFrame , int *gotPicture ) {
   // printf("EXTERN VIDEO extern_decode_video \n" );

  // TODO: boolean value that informs DM whether or
  // not a new frame is copied
  *gotPicture = true;

#ifdef DM_EXTERN_LIVE_VIDEO
  int frameNo;
  GLubyte* pCamPicture = (GLubyte*)grabberLockNewest( id_live_video , &frameNo);
  *gotPicture = (int)(pCamPicture != NULL);
  // printf("EXTERN VIDEO extern_decode_video #%d %d %x\n" ,id_live_video,*gotPicture,*pCamPicture);
#endif

  // TODO: code for copy from external buffer to DM buffer
  // replace extern_frame by the external frame
  if( *gotPicture ) {
    if( rgbFrame && rgbFrame->avPicture->data[0] ) {
      // rectangular texture: video frame and texture memory have the same size
      if( rgbFrame->rectangle ) {
 	printf("EXTERN VIDEO extern_decode_video %dx%d [%d] (%d)\n" , 
 	       rgbFrame->sizeX , rgbFrame->sizeY , rgbFrame->bytesPerPixel , 
 	       (int)(rgbFrame->avPicture->data[0][0]) );
#ifdef DM_EXTERN_LIVE_VIDEO
	memcpy( rgbFrame->avPicture->data[0] ,  
		pCamPicture , 
		rgbFrame->sizeX * rgbFrame->sizeY * rgbFrame->bytesPerPixel );
#else
	memcpy( rgbFrame->avPicture->data[0] ,  
		extern_frame , 
		rgbFrame->sizeX * rgbFrame->sizeY * rgbFrame->bytesPerPixel );
#endif
      }
      // non rectangular texture: texture memory has power of 2 sizes
      // computed from video frame size
      else {
	printf("EXTERN VIDEO extern_decode_video %dx%d -> %dx%d [%d] (%d)\n" , 
	       videoParam->sizeX , videoParam->sizeY , 
	       rgbFrame->sizeX , rgbFrame->sizeY , rgbFrame->bytesPerPixel , 
	       (int)(rgbFrame->avPicture->data[0][0]) );

	for( int ind = 0 ; ind < videoParam->sizeY ; ind++ ) {
#ifdef DM_EXTERN_LIVE_VIDEO
	  memcpy( rgbFrame->avPicture->data[0] 
		  + ind * rgbFrame->sizeX * rgbFrame->bytesPerPixel ,  
		  pCamPicture 
		  + ind * videoParam->sizeX * rgbFrame->bytesPerPixel ,  
		  videoParam->sizeX * rgbFrame->bytesPerPixel );
#else
	  memcpy( rgbFrame->avPicture->data[0] 
		  + ind * rgbFrame->sizeX * rgbFrame->bytesPerPixel ,  
		  extern_frame 
		  + ind * videoParam->sizeX * rgbFrame->bytesPerPixel ,  
		  videoParam->sizeX * rgbFrame->bytesPerPixel );
#endif
	}
      }
    }
#ifdef DM_EXTERN_LIVE_VIDEO
    grabberUnlock();
#endif
  }
}

/////////////////////////////////////////////////
// in case of recorded video, rewinds the video
// should be ignored for live video
bool extern_rewind_video( char *filmName ) {
  // probably nothing TODO here for live video
//   printf("EXTERN VIDEO extern_rewind_video %s\n" , filmName );
  return true;
}


const char *VideoTypeString[EmptyVideoType + 1] = { "videofinite" , "videoloop" , "emptyvideotype" };
bool   codecInitialized = false;

const char *VideoSourceString[EmptyVideoSource + 1] = { "videofile", "videolive", "videostream", "emptyvideosource" };

#ifdef _WIN32

bool liveVideoRectangle = false;

/*!
 * \brief callback function for live video on win32 with visual studio
 * \param hWnd				to be added
 * \param lpVHdr			to be added
 */
static LRESULT CALLBACK webcam_frame(HWND hWnd, LPVIDEOHDR lpVHdr) {
  // Get the video object associated to this webcam
  dm_Video * video = (dm_Video *)capGetUserData(hWnd);
  GLuint texID = DM_CompositionNode->GetTextureID(video->textureNo);

  // if it's the first frame we generate a new texture ID
  if(lpVHdr->dwTimeCaptured == 0) {
    // Generate the texture ID
    glGenTextures(1, &texID);
    DM_CompositionNode->SetTextureID(video->textureNo, texID);
  }

  GLubyte * data;
	
  if(video->convert_context != NULL && video->inFrame != NULL && video->outFrame != NULL) {
      // if it's the first frame we bind the webcam image buffer to the inpout frame that we'll use to convert image
      if(lpVHdr->dwTimeCaptured == 0)// PIX_FMT_YUV420P
	avpicture_fill((AVPicture *) (video->inFrame), lpVHdr->lpData, PIX_FMT_YUYV422, video->live_video_w, video->live_video_h);

      // Do the convertion
      sws_scale(video->convert_context, video->inFrame->data, video->inFrame->linesize, 0, video->live_video_h, video->outFrame->data, video->outFrame->linesize);

      // Store the result
      data = video->outFrame->data[0];
    }
  else
    data = lpVHdr->lpData;
	
  // Increment the frame counter
  lpVHdr->dwTimeCaptured++;

  // Give the texture to OpenGL in BGR24 format
  if( !liveVideoRectangle ) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);
#ifdef DM_HAVE_CG
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI, video->live_video_w, video->live_video_h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, video->live_video_w, video->live_video_h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
#endif
  }
  else {
    glEnable(GL_TEXTURE_RECTANGLE);
    glBindTexture(GL_TEXTURE_RECTANGLE, texID);
#ifdef DM_HAVE_CG
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA_FLOAT32_ATI, video->live_video_w, video->live_video_h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
#else
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB8, video->live_video_w, video->live_video_h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
#endif
  }

  return true;
}
#endif

#ifdef _WIN32
/*!
 * \brief threads for buffer filling
 * \param film			to be added
 *
 * video buffer processing,
 * fills frame buffer with images from a film
 */
DWORD WINAPI fillVideoBuffer(PVOID film) {
#else
void *fillVideoBuffer(void *film) {
#endif

  dm_Video                *currentFilm;
  dm_AudioVideoInData     *currentAudioVideoInData;

  AVPacket                 audioVideoPacket;
  int                      readVideoBufferStatus;
  bool                     needVideo;
 
  // Initialize readVideoBufferStatus
  readVideoBufferStatus = 0;
  
  // Get the currentAudioVideoInData of the reader object from the parameter
  currentFilm = (dm_Video *)film;
  currentAudioVideoInData = currentFilm->filmAudioVideo;

  // Keep looping until we're signaled to quit by the main thread
  while( true ) {

    if( currentAudioVideoInData->rgbFrame->external ) {
      extern_fillBuffer( currentAudioVideoInData->rgbFrame->id_live_video );
    } 
    else {
      // make sure the queues are always  full.  
      if( true /*currentFilm->on*/ ) {
	// Initialize the read status to zero (no error)
	readVideoBufferStatus = 0;
      
	// Initialize the flags indicating whether or not we need
	// video 
	needVideo = (currentAudioVideoInData->videoCodecContext != NULL) && 
	  (currentAudioVideoInData->videoQueue->nbPackets 
	   < DM_VIDEO_PACKET_QUEUE_SIZE);
      
	// Keep reading until we hit a read error, or we read enough
	// video and/or audio
	while( readVideoBufferStatus >= 0 && needVideo ) {
	  // lock the video file mutex
#ifdef _WIN32
	  WaitForSingleObject( currentAudioVideoInData->videoMutex, INFINITE );
#else
	  pthread_mutex_lock( &currentAudioVideoInData->videoMutex );
#endif
	
	  // printf( "Begin of Video av_read_frame\n" );
	  // Try to read a packet
	  readVideoBufferStatus 
	    = av_read_frame( currentAudioVideoInData->videoFormatContext, 
			     &audioVideoPacket );
	  // printf( "End of Video av_read_frame\n" );
	
	  // if correct read, adds to the corresponding queue
	  if (readVideoBufferStatus >= 0)  {
	    if ((audioVideoPacket.stream_index == 
		 currentAudioVideoInData->videoStreamIndex) &&
		(currentAudioVideoInData->videoCodecContext != NULL)) {
	      // adds the packet to the video queue
	      // printf( "queuePushPacket\n" , currentAudioVideoInData->filmName );
	      currentAudioVideoInData->queuePushPacket(
						       currentAudioVideoInData->videoQueue, 
						       &audioVideoPacket );
	    }
	    else {
	      // Throw the packet away
	      av_free_packet(&audioVideoPacket);
	    }
	  }
	  else {
	    printf( "Read error %d (%s)\n" , readVideoBufferStatus , currentAudioVideoInData->filmName );
	  }
	
	  // Update the video and audio flags
	  needVideo = (currentAudioVideoInData->videoCodecContext != NULL) && 
	    (currentAudioVideoInData->videoQueue->nbPackets < DM_VIDEO_PACKET_QUEUE_SIZE);

	  // release the video mutex
#ifdef _WIN32
	  ReleaseMutex(currentAudioVideoInData->videoMutex);
#else
	  pthread_mutex_unlock(&currentAudioVideoInData->videoMutex);
#endif
	}
      }
    
      // end of the file, sets the on value to false for the main thread
      if( readVideoBufferStatus < 0 && currentFilm->videoOn ) {
	// printf( "end of the file\n" );
	currentFilm->videoOn = false;
      }
    }
    // Sleep for a while to yield the processor to other threads
#ifdef _WIN32
    Sleep(10);
#else
    usleep(10000);
#endif
  }
  
  // Sleep for a while to yield the processor to other threads
#ifdef _WIN32
  return 100;
}
#else
  return NULL;
}
#endif


/*!
 * \brief audio-video input packet queues
 * \param queueMutex			to be added
 *
 * empties a queue
 */
void dm_AudioVideoPacketQueue::flushQueue( pthread_mutex_t queueMutex ) {
  AVPacketList *packetQueueEntry;
  AVPacket packet;

  // Lock the queue mutex
#ifdef _WIN32
  WaitForSingleObject( queueMutex, INFINITE );
#else
  pthread_mutex_lock( &queueMutex );
#endif

  // Remove and free each packet in turn
  while( head && nbPackets > 0 ) {
    // Try to dequeue a packet, free its memory if successful
    packetQueueEntry = head;

    if (packetQueueEntry != NULL) {
      // Update the queue
      head = packetQueueEntry->next;
      if (head == NULL)
	tail = NULL;
      nbPackets--;

      // Extract the packet from the queue element, free the element 
      // and free the packet structure as well
      packet = packetQueueEntry->pkt;
      free(packetQueueEntry);
      av_free_packet(&packet);
    }
  }

  // Reset all queue pointers and counters
  head = tail = NULL;
  nbPackets = 0;

  // Unlock the queue mutex
#ifdef _WIN32
  ReleaseMutex(queueMutex);
#else
  pthread_mutex_unlock(&queueMutex);
#endif
}

dm_AudioVideoInData::~dm_AudioVideoInData(void) {
  if (videoFormatContext) {
    delete videoFormatContext;
    videoFormatContext = NULL;
  }

  if (videoCodecContext) {
    delete [] videoCodecContext;
    videoCodecContext = NULL;
  }
  
  if (videoCodec) {
    delete [] videoCodec;
    videoCodec = NULL;
  }

  if (videoQueue)	{
// 	printf("deleting video Queue\n");
    delete videoQueue;
    videoQueue = NULL;
  }
  
  if (filmName) {
    delete [] filmName;
    filmName = NULL;
  }
  
  
  if (videoFrame) {
    delete [] videoFrame;
    videoFrame = NULL;
  }

  if (rgbFrame)	{
    if (rgbFrame->avPicture) {
      free(rgbFrame->avPicture);
      rgbFrame->avPicture = NULL;
    }
    
    delete [] rgbFrame;
    rgbFrame = NULL;
  }
   
  if (videoParam) {
    delete videoParam;
    videoParam = NULL;
  }
    
}

dm_AudioVideoInData::dm_AudioVideoInData( VideoSource video_source ) {
  // libavcodec data initialization

  //////////////////////////////////
  // video
  videoFormatContext = NULL;
  videoCodecContext = NULL;
  videoCodec = NULL;

  filmName = NULL;
  InitString( &filmName );
  videoStreamIndex = -1;

  videoServer = NULL;

  liveVideo = (video_source == DM_VIDEOLIVE);
  streamVideo = (video_source == DM_VIDEOSTREAM);

  // current video frame allocation
  videoFrame = avcodec_alloc_frame();
  if(videoFrame ==NULL) {
    sprintf( ErrorStr , "Error: Frame not allocated!" ); ReportError( ErrorStr ); throw 1;
  } 

  // RGB frame
  rgbFrame = new dm_ImageRRGB;
  // allocates a frame that will hold the image converted from yuv to rgb
  // pixel buffer initialized in a second step when the image size is known
  rgbFrame->avPicture = (AVPicture *)malloc(sizeof(AVPicture));
  // resets the rgb frame
  memset(rgbFrame, 0, sizeof(rgbFrame));
  rgbFrame->bytesPerPixel = 3;
  rgbFrame->sizeX = 0;
  rgbFrame->sizeY = 0;
  rgbFrame->sizeZ = 0;
  rgbFrame->avPicture->data[0] = NULL;
  rgbFrame->avPicture->data[1] = NULL;
  rgbFrame->avPicture->data[2] = NULL;
  rgbFrame->avPicture->data[3] = NULL;
  rgbFrame->avPicture->linesize[0] = 0;
  rgbFrame->avPicture->linesize[1] = 0;
  rgbFrame->avPicture->linesize[2] = 0;
  rgbFrame->avPicture->linesize[3] = 0;
  rgbFrame->rectangle = false;
  rgbFrame->external = false;
  rgbFrame->id_live_video = -1;
  newFrame = false;
  // a priori a video is rewindable, unless a rewind sets it to false
  rewindable = true;

  // Initialize the SWS context to null until we want to use it for the first time
  context = NULL;
 
  // video packet queue initialization
  videoQueue = new dm_AudioVideoPacketQueue;
  videoQueue->head = NULL;
  videoQueue->tail = NULL;
  videoQueue->nbPackets = 0;

  // video parameters initialization
  videoParam = new dm_VideoParameters;
  videoParam->sizeX = 0;
  videoParam->sizeY = 0;
  videoParam->frameDuration = 0.0;
  videoParam->filmDuration = 0.0;

  ///////////////////////////////////////
  // thread initialization
  
  // video queue mutex objects
#ifdef _WIN32
  // mutex names (for Window$)
  lpszVideoMutex = (LPCTSTR)"VideoMutex";
  lpszQueueMutex = (LPCTSTR)"QueueMutex";
  videoMutex = CreateMutex( NULL, TRUE, lpszVideoMutex );
  queueMutex = CreateMutex( NULL, TRUE, lpszQueueMutex );
  if (videoMutex == NULL || queueMutex == NULL ) {
    sprintf( ErrorStr , "Error: Mutex initialization failed!" ); ReportError( ErrorStr ); throw 336;
  }
  ReleaseMutex( videoMutex );
  ReleaseMutex( queueMutex );
#else
  int res;
  res = pthread_mutex_init(&queueMutex, NULL);
  if (res != 0) {
    sprintf( ErrorStr , "Error: Mutex initialization failed!" ); ReportError( ErrorStr ); throw 336;
  }
  res = pthread_mutex_init(&videoMutex, NULL);
  if (res != 0) {
    sprintf( ErrorStr , "Error: Mutex initialization failed!" ); ReportError( ErrorStr ); throw 336;
  }
#endif
 
  ///////////////////////////////////////
  // dates
  
  // relative time (wrt film time) of the current frame
  currentFrameTime = 0.0;
  // relative total time (wrt film time) of the current film
  totalFileTime = 0.0;
  frameNo = 0;
  // absolute time of the last frame display
  double the_time =  RealTime();
  lastAbsoluteTime = (double)the_time;
  // openGL frame No of the last video update
  lastOpenGLFrameUpdate = FrameNo - 1;

}

bool dm_AudioVideoInData::loadVideo( char *fname ) {
  StrCpy( &filmName , fname );
  return true;
}

bool dm_AudioVideoInData::loadFilm( char *fname ,  int id_live_video ,
				    int live_video_w , int live_video_h  , 
				    bool rectangle , bool external ) {
  int errorCode = 0;

  AVFormatParameters formatParams;
  AVInputFormat *iformat = NULL;

  ////////////////////////////////////////////////
  // CODEC INITIALIZATIONS
  StrCpy( &filmName , fname );

  // initializes the codec list
  if( !codecInitialized ) {
    avcodec_init();
    // Register all formats and codecs
    avcodec_register_all();
    av_register_all();
	#ifndef _WIN32
		//Register all devices
		avdevice_register_all(); 
	#endif

    codecInitialized = true;
  }

  // Acquire the video mutex
#ifdef _WIN32
  WaitForSingleObject( videoMutex, INFINITE );
#else
  pthread_mutex_lock( &videoMutex );
#endif


  ////////////////////////////////////////////////////////////
  // FILE OPENING AND PARAMETER INITIALIZATIONS: LIVE VIDEO
  if( strncmp( fname , "/dev/video" , 10 ) == 0 || liveVideo ) {
      if( !external) {
	if( strncmp( fname , "/dev/video" , 10 ) != 0 ) {
	  sprintf( fname , "/dev/video%d" , id_live_video );
	}

	// AVFormatContext *ic;
	// ic =  avformat_alloc_context();

	memset(&formatParams, 0, sizeof(AVFormatParameters));

	// formatParams.prealloced_context = 1;

	// formatParams.device = fname;
	// formatParams.sample_rate = 44100;
	// formatParams.channels = 1;
	formatParams.channel = 0;
	formatParams.standard = 0;
	//formatParams.standard = "ntsc";
	formatParams.width = live_video_w;
	formatParams.height = live_video_h;
	// formatParams.prealloced_context = 0;
	formatParams.time_base.num = 1;
	formatParams.time_base.den = 30;
	// formatParams.frame_rate = 29;
	// formatParams.frame_rate_base = 1;
	formatParams.pix_fmt = PIX_FMT_YUYV422;

	iformat = av_find_input_format("video4linux2");
	if (!iformat) {
	  iformat = av_find_input_format("video4linux");
	}
	if (!iformat) {
	  sprintf( ErrorStr , "Error: input format not found [video4linux]!" ); ReportError( ErrorStr ); throw 1;
	}
	else {
	    printf( "Input format found [video4linux2]!\n" ); 
	}
    }
	  
    liveVideo = true;

/* get default parameters from command line */
#ifndef _WIN32
		  videoFormatContext = avformat_alloc_context();
#endif

    if( !external ) {
      errorCode = av_open_input_file( &videoFormatContext,
				      fname, iformat, 0, 
				      &formatParams);
      if( errorCode < 0 ) {
	char error_str[128];
	switch (errorCode) {
	  //case AVERROR_UNKNOWN: strcpy( error_str ,  "AVERROR_UNKNOWN"); break;   // same value as AVERROR_INVALIDDATA
	case AVERROR_IO: strcpy( error_str ,  "AVERROR_IO" ); break;
	case AVERROR_NUMEXPECTED: strcpy( error_str ,  "AVERROR_NUMEXPECTED"); break;
	case AVERROR_INVALIDDATA: strcpy( error_str ,  "AVERROR_INVALIDDATA"); break;
	case AVERROR_NOMEM: strcpy( error_str ,  "AVERROR_NOMEM"); break;
	case AVERROR_NOFMT: strcpy( error_str ,  "AVERROR_NOFMT"); break;
	case AVERROR_NOTSUPP: strcpy( error_str ,  "AVERROR_NOTSUPP"); break;
	case AVERROR_NOENT: strcpy( error_str ,  "AVERROR_NOENT"); break;
	case AVERROR_PATCHWELCOME: strcpy( error_str ,  "AVERROR_PATCHWELCOME"); break;
	default: strcpy( error_str ,  "Unknown error"); break;
	}

	sprintf( ErrorStr , "Error: live video not opened, no such device [%s (%dx%d)] error %s!" , fname , formatParams.width , formatParams.height , error_str ); ReportError( ErrorStr ); 
	videoFormatContext = NULL;
      }
    }
    else {
      errorCode = extern_open_input_file( id_live_video );
      if( errorCode < 0 ) {
	sprintf( ErrorStr , "Error: live video not opened, no such device number (id_live_video attribute) [%d error %d!" , id_live_video , errorCode ); ReportError( ErrorStr ); 
      }
    }
  }

  ////////////////////////////////////////////////////////////
  // FILE OPENING AND PARAMETER INITIALIZATIONS: DVD
  else if( strcmp( fname , "/dev/dv1394" ) == 0
	   || strcmp( fname , "/dev/dvd" ) == 0 ) {
    liveVideo = true;
    
    if( !external ) {
      // formatParams.device = "/dev/dv1394";
      iformat = av_find_input_format("dv1394");
      errorCode = av_open_input_file(&videoFormatContext,
				     "", iformat, 0, &formatParams);
      if( errorCode < 0 ) {
	sprintf( ErrorStr , "Error: dvd video not opened, no such device [%s] error %d!" , fname , errorCode ); ReportError( ErrorStr ); 
	videoFormatContext = NULL;
      }
    }
    else {
      errorCode = extern_open_input_file( id_live_video );
      if( errorCode < 0 ) {
	sprintf( ErrorStr , "Error: dvd video not opened, no such device [%s] error %d!" , fname , errorCode ); ReportError( ErrorStr ); 
      }
    }
  }

  ////////////////////////////////////////////////////////////
  // FILE OPENING AND PARAMETER INITIALIZATIONS: REGULAR FILE
  else {
    liveVideo = false;

    //if( !external ) {
      // file opening
      errorCode = av_open_input_file(&videoFormatContext, fname, 
				     NULL, 0, NULL);
      if( errorCode < 0 ) {
	sprintf( ErrorStr , "Error: video file not found [%s] error %d!" , fname , errorCode ); ReportError( ErrorStr ); 
	videoFormatContext = NULL;
	throw 0;
      }
 //   }
 //   else {
 //     errorCode = extern_open_input_file( id_live_video );
 //     if( errorCode < 0 ) {
	//sprintf( ErrorStr , "Error: video file not found [%s] error %d!" , fname , errorCode ); ReportError( ErrorStr ); 
	//videoFormatContext = NULL;
	//throw 0;
 //     }
 //   }
  }
  

  ////////////////////////////////////////////////////////////
  // STREAM INFORMATION DUMP

  if( !external ) {
    // codec retrieval
    if( !videoFormatContext ) {
      // video variables reset
      videoCodecContext = NULL;
      videoCodec = NULL;
      indVideoStream = -1;
      
      return false;
    }
    
    errorCode = av_find_stream_info(videoFormatContext);
    if( errorCode < 0 ) {
      sprintf( ErrorStr , "Error: codec of video file not supported [%s] error %d, see http://www1.mplayerhq.hu/MPlayer/releases/codecs/!" , fname , errorCode ); ReportError( ErrorStr ); throw 336;
    }
    else {
      // prints file details on the console
      dump_format(videoFormatContext, 0, fname, 0);
    }
  }
    
  ////////////////////////////////////////////////////////////
  // CODEC ACCESS & PARAMETER SET-UP

  // default video frame rate
  videoParam->frameDuration = 1.0 / 25.0;

  if( !external ) {
    // retrievaes a video stream from the file
    bool videoFound = false;
    printf("nb video streams %d for %s\n" , (int)videoFormatContext->nb_streams , fname );
    for( indVideoStream = 0 ; 
	 indVideoStream < (int)videoFormatContext->nb_streams ; 
	 indVideoStream++ ) {
      
      // Get the video codec context for this stream
      videoCodecContext = (videoFormatContext->streams[indVideoStream]->codec);
      
      if( videoCodecContext->codec_type == CODEC_TYPE_VIDEO ) {
	videoFound = true;
	videoStreamIndex = indVideoStream;
	break;
      }
    }
    
    // no video, probably a sound file
    if( !videoFound ) {
      // video variables reset
      videoCodecContext = NULL;
      videoCodec = NULL;
      indVideoStream = -1;
      
      sprintf( ErrorStr , "Error: No video found!" ); ReportError( ErrorStr ); 
      return false;
    }
    
    // there is a video stream
    else {
      // Find the appropriate video codec
      videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);
      if (videoCodec == NULL) {
	sprintf( ErrorStr , "Error: codec [%s/%d] of video file not supported [%s], see http://www1.mplayerhq.hu/MPlayer/releases/codecs/!" , videoCodecContext->codec_name , videoCodecContext->codec_id , fname ); ReportError( ErrorStr ); throw 336;
	// video variables reset
	videoCodecContext = NULL;
	videoCodec = NULL;
	indVideoStream = -1;
      }
      else {
	// Initialize the video codec
	errorCode = avcodec_open(videoCodecContext, videoCodec);
	if (errorCode < 0) {
	  sprintf( ErrorStr , "Error: codec initialization error [%s] error %d, see http://www1.mplayerhq.hu/MPlayer/releases/codecs/!" , fname , errorCode ); ReportError( ErrorStr ); throw 336;
	  // video variables reset
	  videoCodecContext = NULL;
	  videoCodec = NULL;
	  indVideoStream = -1;
	}
	else {
	  // video dimensions
	  videoParam->sizeX = videoCodecContext->width;
	  videoParam->sizeY = videoCodecContext->height;
	  
	  rgbFrame->external = external;
	  rgbFrame->id_live_video = id_live_video;
	  
	  rgbFrame->rectangle = rectangle;
	  if( rectangle ) {
	    // rgb non POT raw image allocation (without alpha channel)
	    rgbFrame->sizeX = videoCodecContext->width;
	    rgbFrame->sizeY = videoCodecContext->height;
	  }
	  else {
	    // rgb POT raw image allocation (without alpha channel)
	    rgbFrame->sizeX = smallestPOT( videoCodecContext->width );
	    rgbFrame->sizeY = smallestPOT( videoCodecContext->height );
	  }
	  printf( "Video %s %dx%d texture %dx%d\n" , fname ,
		  videoCodecContext->width , videoCodecContext->height , 
		  rgbFrame->sizeX , rgbFrame->sizeY );
	  rgbFrame->bytesPerPixel = 3;
	  rgbFrame->avPicture->data[0] 
	    = new GLubyte[rgbFrame->sizeX * rgbFrame->sizeY 
			  * rgbFrame->bytesPerPixel];
	  memset( rgbFrame->avPicture->data[0], (GLubyte)255, 
		  rgbFrame->sizeX * rgbFrame->sizeY 
		  * rgbFrame->bytesPerPixel );
	  rgbFrame->avPicture->linesize[0] 
	    = rgbFrame->sizeX * rgbFrame->bytesPerPixel;	  
	  
	  // if needed - rgb NPOT raw image allocation (without alpha channel)
	  if( rgbFrame->sizeX != videoCodecContext->width 
	      || rgbFrame->sizeY != videoCodecContext->height ) {
	    NPOTSizeX = videoCodecContext->width;
	    NPOTSizeY = videoCodecContext->height;
	  }
	  
	  // video frame duration in case of non null frame rate
	  if( videoCodecContext->time_base.den != 0) {
	    // does not correspond to the actual frame rate
	    // should be fixed in the future when ffmpeg 
	    // data structure is stable
	    videoParam->frameDuration = av_q2d(videoCodecContext->time_base);
	    // frame p unit
	    // double fps = 1.0/av_q2d(videoCodecContext->time_base);
	    // printf( "frame per sec %.8f\n" , fps);
	    
	    videoParam->frameDuration = 0.040;
	  }
	}
      }
    }
  }

  ////////////////////////////////////////////////////////////
  ///// EXTERNAL VIDEO
  else {
    int   externVideoBytesPerPixel;
    extern_open_video_context( 
			      // video dimensions
			      &(videoParam->sizeX) ,
			      &(videoParam->sizeY) ,
			      &externVideoBytesPerPixel ,
			      rectangle ,
			      &(videoParam->frameDuration) );
 
	printf("external video opened %dx%d Rectangle %d\n" , videoParam->sizeX , videoParam->sizeY , (int)rectangle );

    rgbFrame->external = external;
    rgbFrame->id_live_video = id_live_video;
    rgbFrame->rectangle = rectangle;
    if( rectangle ) {
      // rgb non POT raw image allocation (without alpha channel)
      rgbFrame->sizeX = videoParam->sizeX;
      rgbFrame->sizeY = videoParam->sizeY;
    }
    else {
      // rgb POT raw image allocation (without alpha channel)
      rgbFrame->sizeX = smallestPOT( videoParam->sizeX );
      rgbFrame->sizeY = smallestPOT( videoParam->sizeY );
    }
    
    printf( "Video %s %dx%d texture %dx%d\n" , fname ,
	    videoParam->sizeX , videoParam->sizeY , 
	    rgbFrame->sizeX , rgbFrame->sizeY );
    rgbFrame->bytesPerPixel = externVideoBytesPerPixel;
    rgbFrame->avPicture->data[0] 
      = new GLubyte[rgbFrame->sizeX * rgbFrame->sizeY 
		    * rgbFrame->bytesPerPixel];
    memset( rgbFrame->avPicture->data[0], (GLubyte)255, 
	    rgbFrame->sizeX * rgbFrame->sizeY 
	    * rgbFrame->bytesPerPixel );
    rgbFrame->avPicture->linesize[0] 
      = rgbFrame->sizeX * rgbFrame->bytesPerPixel;
  
    // if needed - rgb NPOT raw image allocation (without alpha channel)
    if( rgbFrame->sizeX != videoParam->sizeX 
	|| rgbFrame->sizeY != videoParam->sizeY ) {
      NPOTSizeX = videoParam->sizeX;
      NPOTSizeY = videoParam->sizeY;
    }
    
    videoParam->frameDuration = 0.040;
  } // extern video texture

  // release the video mutex
#ifdef _WIN32
  ReleaseMutex(videoMutex);
#else
  pthread_mutex_unlock(&videoMutex);
#endif

  // for the moment we are only interested in video streams
  if( rgbFrame->external || videoCodecContext ) {
    // reset the video time parameters 
    currentFrameTime = 0.0;
    totalFileTime = 0.0;
    frameNo = 0;
    double the_time = RealTime();
    lastAbsoluteTime = (double)the_time;
    lastOpenGLFrameUpdate = FrameNo - 1;

    newFrame = false;
    // a priori a video is rewindable, unless a rewind sets it to false
    rewindable = true; 

    //     // initiates the decoder by putting the first frame in the queue
    //     ReadNextFrameStatus readStatus = readStatus;
    //     if( (readStatus = readNextFrame()) != DM_READ_FRAME_ERROR ) {
    //       if( readStatus == DM_GOT_VIDEO_FRAME ) {
    // 	img_convert_and_resize();
    //       }
    //       else {
    // 	fprintf(stderr,"got no initial video frame...\n" );
    // 	return true;
    //       }
    //       // Release the packet
    //       av_free_packet(&moviePacket);
    //     }
    //     else {
    //       fprintf(stderr,"read initial frame failed...\n" );
    //       return true;
    //     }

    // Return true to indicate that we successfully opened the file
    return true;
  }
  else {
    sprintf( ErrorStr , "Error: file with no video or audio stream!" ); ReportError( ErrorStr ); throw 336;
  }

  // Return false to indicate that we couldn't open the file or that we
  // have nothing to play
  return false;
}

bool dm_AudioVideoInData::loadServer( char *servername ,
				      bool rectangle ) {
  for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Servers ; ind++ ) {
    if( DM_EnvironmentNode->IP_Servers[ ind ]->receive_format == Stream 
	&& strcmp( DM_EnvironmentNode->IP_Servers[ ind ]->id , servername ) == 0 ) {
      videoServer = DM_EnvironmentNode->IP_Servers[ ind ];

      streamVideo = true;

      // video dimensions
      videoParam->sizeX = 300;
      videoParam->sizeY = 200;
      
      rgbFrame->external = false;

      // rgb POT raw image allocation (without alpha channel)
      rgbFrame->rectangle = rectangle;
      if( !rectangle ) {
	rgbFrame->sizeX = smallestPOT( videoParam->sizeX );
	rgbFrame->sizeY = smallestPOT( videoParam->sizeY );
      }
      else {
	rgbFrame->sizeX = videoParam->sizeX;
	rgbFrame->sizeY = videoParam->sizeY;
      }
      printf( "Video %s %dx%d texture %dx%d\n" , servername ,
	      videoParam->sizeX , videoParam->sizeY , 
	      rgbFrame->sizeX , rgbFrame->sizeY );
      rgbFrame->bytesPerPixel = 3;
      rgbFrame->avPicture->data[0] 
	= new GLubyte[rgbFrame->sizeX * rgbFrame->sizeY 
			* rgbFrame->bytesPerPixel];
      memset( rgbFrame->avPicture->data[0], (GLubyte)255, 
	      rgbFrame->sizeX * rgbFrame->sizeY 
	      * rgbFrame->bytesPerPixel );
      rgbFrame->avPicture->linesize[0] 
	= rgbFrame->sizeY * rgbFrame->bytesPerPixel;

      return true;
    }
  }
      
  // Return false to indicate that we couldn't open the file or that we
  // have nothing to play
  return false;
}

ReadNextFrameStatus dm_AudioVideoInData::readNextFrame( void ) {
  int gotPicture;

  // extern video access
  printf( "EXTERN VIDEO RGB frame is external %d\n" , (int)rgbFrame->external );

  // printf( "Video readNextFrame %dx%d (%d) \n" , rgbFrame->sizeX , rgbFrame->sizeY , rgbFrame->bytesPerPixel );
  if( streamVideo && videoServer ) {
    if( videoServer->readNextStream( &(rgbFrame->sizeX) ,
				     &(rgbFrame->sizeY) ,
				     rgbFrame->bytesPerPixel ,
				     &(rgbFrame->avPicture->data[0]) ) ) {
      rgbFrame->avPicture->linesize[0] 
	= rgbFrame->sizeX * rgbFrame->bytesPerPixel;
      return DM_GOT_VIDEO_FRAME;
    }
    return DM_GOT_NO_VIDEO_FRAME;
  }
  // printf( "End of Video readNextFrame\n" );

  // Get the next video packet from the queue
  if( !rgbFrame->external && videoCodecContext != NULL ) {
    if( queuePullPacket( videoQueue , &moviePacket ) ) {
      // Allocate a video frame and decode the video packet
      /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
	 and this is the only method to use them because you cannot
	 know the compressed data size before analysing it. 

	 BUT some other codecs (msmpeg4, mpeg4) are inherently frame
	 based, so you must call them with all the data for one
	 frame exactly. You must also initialize 'width' and
	 'height' before initializing them. */

      /* NOTE2: some codecs allow the raw parameters (frame size,
	 sample rate) to be changed at any frame. We handle this, so
	 you should also take care of it */

      // printf( "Begin of Video readNextFrame\n" );
      avcodec_get_frame_defaults(videoFrame);
      int len = avcodec_decode_video(videoCodecContext, 
				     videoFrame, 
				     &gotPicture, moviePacket. data, 
				     moviePacket.size);

      // printf( "End of Video readNextFrame\n" );

      // incomplete frame
      if (len < 0) {
	sprintf( ErrorStr , "Error: incomplete frame decoding!" ); ReportError( ErrorStr ); 
	// Release the packet
	av_free_packet(&moviePacket);
	return DM_READ_FRAME_ERROR;
      }
      // full frame
      
      if( gotPicture ) {
	return DM_GOT_VIDEO_FRAME;
      }
      else {
	return DM_GOT_NO_VIDEO_FRAME;
      }
    }
    else {
      return DM_READ_FRAME_ERROR;
    }
  }

  if( rgbFrame->external ) {
    extern_decode_video( videoParam , rgbFrame->id_live_video , rgbFrame , &gotPicture );
    if( gotPicture ) {
      return DM_GOT_VIDEO_FRAME;
    }
    else {
      return DM_GOT_NO_VIDEO_FRAME;
    }
  }

  return DM_READ_FRAME_ERROR;
}

/*! \name Group fram_conv
 *  \brief frame conversion and filtering
 */
// @{ 
/*!
 * \param r			to be added
 * \param g			to be added
 * \param b			to be added
 *
 * Needed by rgb2hsv(),
 * Taken from Nadia Bianchi-Berthouze
 *  Image Processing course
 * http://halifax3.u-aizu.ac.jp/~~nadia/courses/DIP/index03.html
 */
float maxrgb(float r,float g,float b) {
  float max;
  
  if( r > g)
    max = r;
  else
    max = g;
  if( b > max )
    max = b;
  return( max );
}
 
 
/*!
 * \param r			to be added
 * \param g			to be added
 * \param b			to be added
 *
 * Needed by rgb2hsv()
 */
float minrgb(float r,float g,float b) {
  float min;
  
  if( r < g)
    min = r;
  else
    min = g;
  if( b < min )
    min = b;
  return( min );
}
 
/*!
 *Taken from "Fund'l of 3D Computer Graphics", Alan Watt (1989)
 * Assumes (r,g,b) range from 0.0 to 1.0
 * Sets h in [0.,1.]
 */
float rgb2h(float r, float g, float b) {
  float h=0,s=0,v=0;
  float max_v,min_v,diff,r_dist,g_dist,b_dist;
  float undefined = 0.0;
  
  max_v = maxrgb(r,g,b);
  min_v = minrgb(r,g,b);
  diff = max_v - min_v;
  v = max_v;
   
  if( max_v != 0 )
    s = diff/max_v;
  else
    s = 0.0;

  if( s == 0 )
    h = undefined;
  else {
    r_dist = (max_v - r)/diff;
    g_dist = (max_v - g)/diff;
    b_dist = (max_v - b)/diff;
    if( r == max_v ) 
      h = b_dist - g_dist;
    else
      if( g == max_v )
	h = 2 + r_dist - b_dist;
      else
	if( b == max_v )
	  h = 4 + g_dist - r_dist;
    h /= 6.0;
    if( h < 0)
      h += 1.0;
  }
  return h;
}

/*!
 * \param r				to be added
 * \param g				to be added
 * \param b				to be added
 * \param hout			to be added
 * \param sout			to be added
 * \param vout			to be added
 *
 * Taken from "Fund'l of 3D Computer Graphics", Alan Watt (1989)
 * Assumes (r,g,b) range from 0.0 to 1.0
 * Sets h in degrees: 0.0 to 360.;
 * s,v in [0.,1.]
 */
void rgb2hsv(float r, float g, float b,
	     float *hout, float *sout, float *vout) {
  float h=0,s=1.0,v=1.0;
  float max_v,min_v,diff,r_dist,g_dist,b_dist;
  float undefined = 0.0;
  
  max_v = maxrgb(r,g,b);
  min_v = minrgb(r,g,b);
  diff = max_v - min_v;
  v = max_v;
  
  if( max_v != 0 )
    s = diff/max_v;
  else
    s = 0.0;
  if( s == 0 )
    h = undefined;
  else {
    r_dist = (max_v - r)/diff;
    g_dist = (max_v - g)/diff;
    b_dist = (max_v - b)/diff;
    if( r == max_v ) 
      h = b_dist - g_dist;
    else
      if( g == max_v )
	h = 2 + r_dist - b_dist;
      else
	if( b == max_v )
	  h = 4 + g_dist - r_dist;
    h *= 60;
    if( h < 0)
      h += 360.0;
  }
  v=v/(float)255;
  *hout = h;
  *sout = s;
  *vout = v;
}
  
/*!
 * \param hin				to be added
 * \param s					to be added
 * \param v					to be added
 * \param rout				to be added
 * \param gout			to be added
 * \param bput			to be added
 *
 * Taken from "Fund'l of 3D Computer Graphics", Alan Watt (1989)
 * Assumes H in degrees, s,v in [0.,1.0];
 * (r,g,b) range from 0.0 to 1.0
 */
void hsv2rgb(float hin, float s, float v,
	     float *rout, float *gout, float *bout) {
  float h;
  float r=0,g=0,b=0;
  float f,p,q,t;
  int i;
  
  h = hin;
  if( s == 0 ) {
    r = v;
    g = v;
    b = v;
  }
  else {
    if(h == 360.)
      h = 0.0;
    h /= 60.;
    i = (int) h;
    f = h - i;
    p = v*(1-s);
    q = v*(1-(s*f));
    t = v*(1-s*(1-f));
    switch(i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
    default:
      r = 1.0;
      b = 1.0;
      b = 1.0;
      //printf("hsv2rgb::How did I get here?\n");
      // printf("h: %f, s: %f, v: %f; i:  %d\n",hin,s,v,i);
      break;
    }
  }
  *rout = r;
  *gout = g;
  *bout = b;
} 

void greenDot( uint8_t *pixMap , int x , int y , int width , int size ) {
  pixMap += ( (y - size / 2) * width + (x - size / 2) ) * 3;
  for( int ind_aux = - size/2 ; ind_aux < size/2 ; ind_aux ++ ) { 
    for( int ind = - size/2 ; ind < size/2 ; ind ++ ) { 
      pixMap[ ind * 3 + 0 ] = 0;
      pixMap[ ind * 3 + 1 ] = 255;
      pixMap[ ind * 3 + 2 ] = 0;
    }
    pixMap += width * 3;
  }
}

/*
 * \brief converts a frame from yuv to rgb
 * \param deinterlace			to be added
 */
void dm_AudioVideoInData::img_convert_and_resize( bool deinterlace ) {
  int frame_width, frame_height;
  // temporary buffer used to store the deinterlaced image
  // can be freed when the image is converted into rgb
  uint8_t *buf = NULL;

  if( rgbFrame->sizeX == videoParam->sizeX
      && rgbFrame->sizeY == videoParam->sizeY ) {
    frame_width = rgbFrame->sizeX;
    frame_height = rgbFrame->sizeY;
  }
  else {
    frame_width = NPOTSizeX;
    frame_height = NPOTSizeY;
  }

  // deinterlace in place
  //   if( deinterlace ) {
  //     //printf( "deinterlace\n" );
  //     if( avpicture_deinterlace( (AVPicture *)videoFrame, 
  // 			       (AVPicture *)videoFrame, 
  // 			       videoCodecContext->pix_fmt,
  // 			       frame_width,frame_height ) < 0) {
  //     }
  //   }

  // deinterlace in an auxiliary picture
  if( deinterlace ) {
    int size;
    AVPicture picture_tmp;
    AVPicture *picture2;
    AVPicture *picture = (AVPicture *)videoFrame;
    
    /* create temporary picture */ 
    size = avpicture_get_size(videoCodecContext->pix_fmt,
			      frame_width,frame_height);
    buf = (uint8_t *)av_malloc(size);
    if (!buf)
      return;
    
    picture2 = &picture_tmp;
    avpicture_fill( picture2, buf, videoCodecContext->pix_fmt,
		    frame_width,frame_height );
    
    //printf( "deinterlace\n" );
    if( avpicture_deinterlace( picture2, picture, 
			       videoCodecContext->pix_fmt,
			       frame_width,frame_height ) < 0) {
      /* if error, do not deinterlace */
      av_free(buf);
      buf = NULL;
    }
    else {
      *picture = *picture2;
    }
  }
}

/// converts a frame from bgr to rgb
void dm_AudioVideoInData::img_convert_and_resize2( bool deinterlace ) {
  int tsize = rgbFrame->sizeX * rgbFrame->sizeY;
  unsigned char temp;
  unsigned char *p = rgbFrame->avPicture->data[0];	 
 
  for(int i = 0; i < tsize; i++, p+=3 ) {
    temp = *(p + 0);
    *(p + 0) = *(p + 2);
    *(p + 2) = temp;
  }
}

/*!
 * \brief rewinds a video
 */
bool dm_AudioVideoInData::rewind_video( void ) {
  // neither audio nor video -> returns
  if (!videoCodecContext)
    return false;

  // no video opened
  if (!videoFormatContext) {
    return false;
  }

  // printf( "start video\n" );

  // flush both queues
  videoQueue->flushQueue( queueMutex );
  newFrame = false;

  // acquire the video mutex
#ifdef _WIN32
  WaitForSingleObject( videoMutex, INFINITE );
#else
  pthread_mutex_lock( &videoMutex );
#endif

  // seeks the beginning of the video file if the stream is valid
  if( !liveVideo && videoStreamIndex >= 0 ) {
    int ret = av_seek_frame(videoFormatContext,-1, 0, AVSEEK_FLAG_BACKWARD);
    rewindable = true;
    if( ret < 0 ) {
      strcpy( ErrorStr , "Error: incorrect video file rewind!" ); ReportError( ErrorStr );
      // release the video mutex
#ifdef _WIN32
      ReleaseMutex(videoMutex);
#else
      pthread_mutex_unlock(&videoMutex);
#endif
      rewindable = false;
      return false;
    }

    // flushes buffers
    if (videoCodecContext != NULL) {
      avcodec_flush_buffers(videoCodecContext);
    }
  }

  // release the video mutex
#ifdef _WIN32
  ReleaseMutex(videoMutex);
#else
  pthread_mutex_unlock(&videoMutex);
#endif

  // timer reset
  currentFrameTime = 0.0;
  totalFileTime = 0.0;
  frameNo = 0;
  double the_time = RealTime();
  lastAbsoluteTime = (double)the_time;
  lastOpenGLFrameUpdate = FrameNo - 1;
  
  // acquires the first frame
  if( !liveVideo ) {
    ReadNextFrameStatus readStatus;
    if( (readStatus = readNextFrame()) != DM_READ_FRAME_ERROR ) {
      if( readStatus == DM_GOT_VIDEO_FRAME ) {
	fprintf(stderr,"got video frame at startup...\n" );
	img_convert_and_resize( false );
      }
      else {
	fprintf(stderr,"got no video frame at startup...\n" );
	return true;
      }
      // Release the packet
      av_free_packet(&moviePacket);
    }
    else {
      fprintf(stderr,"read frame at startup failed...\n" );
      // for some video formats such as DivX, rewind 
      // is difficult and can take several frames
      // I suppose that the solution could be to close
      // and reopen the file...
      // another solution is to use an easily rewindable
      // format for video loops (such as cinepak or mjpeg)
      
      // this close/reopen does not work as it is
      // it would be necessary to make a reopen
      // different from load film that only
      // makes what has to be made for reopening
      //     if( !liveVideo ) {
      //       av_close_input_file(videoFormatContext);
      //       loadFilm( filmName , 0 , 0 );
      //     }
      return true;
    }
  }

  // acquires the first frame
  //   ReadNextFrameStatus readStatus;
  //   if( (readStatus = readNextFrame()) != DM_READ_FRAME_ERROR ) {
  //     if( readStatus == DM_GOT_VIDEO_FRAME ) {
  //       img_convert_and_resize();
  //     }
  //     else {
  //       fprintf(stderr,"got no video frame at startup...\n" );
  //       return true;
  //     }
  //     // Release the packet
  //     av_free_packet(&moviePacket);
  //   }
  //   else {
  //     fprintf(stderr,"read frame at startup failed...\n" );
  //     return true;
  //   }

  return true;
}

bool dm_AudioVideoInData::GoToNextFrame( bool *readError , bool deinterlace ) {
  // printf( "Go to next frame\n" );
  // neither audio nor video -> returns
  if( !rgbFrame->external && !liveVideo && !streamVideo ) {
    // neither audio nor video -> returns
    if( !videoCodecContext ) {
      return false;
    }
  }

  // stream video
  if( streamVideo ) {
    readNextFrame();
    return true;
  }

  // Add the specified time to the video timer
  double the_time = RealTime();
  float seconds = (float)(the_time - lastAbsoluteTime);
  currentFrameTime += seconds;
  totalFileTime += seconds;
  lastAbsoluteTime += seconds;
  lastOpenGLFrameUpdate = FrameNo;
  // printf( "last -> current time %.3f %.3f last frame update %d frame duration %.3f \n" , lastAbsoluteTime , currentFrameTime , lastOpenGLFrameUpdate , videoParam->frameDuration );

  // no more than 10 frames jumped at a time
  if( !liveVideo ) {
    if( currentFrameTime / videoParam->frameDuration > 10 ) {
      currentFrameTime = 0;
      return false;
    }
  }

  // if the time for the current frame is greater than the video's
  // time-per-frame, then advance the frame
  bool packetAllocated = false;
  ReadNextFrameStatus readStatus = DM_GOT_NO_VIDEO_FRAME;
  // printf( "currentFrameTime %.5f  totalFileTime %.5f videoParam->frameDuration %.5f frame No %d\n" , currentFrameTime ,  totalFileTime , videoParam->frameDuration , frameNo );

  while( true ) {
    // computes the current frame time 
    // in order to check whether we have reached the current frame
    currentFrameTime -= videoParam->frameDuration;

    // printf( "DM_READ_FRAME_NEXT\n" );

	  // reads a frame
    if( (readStatus = readNextFrame()) != DM_READ_FRAME_ERROR ) {
      
      // printf( "DM_READ_FRAME_OK\n" );
      if( !liveVideo && currentFrameTime > videoParam->frameDuration  ) {
	// Release the packet because we are going to reade further frames
	// otherwise, packet release must be performed after rgb conversion
	if( !rgbFrame->external ) {
	  av_free_packet(&moviePacket);
	}
	frameNo++;
      } 
      else {
	packetAllocated = true;
	frameNo++;
	break;
      }
    }
    else  {
      // printf( "DM_READ_FRAME_ERROR\n" );
      *readError = true;
      break;
    }
  }
  // printf( "end of currentFrame\n" );

  if( !rgbFrame->external ) {

    if( readStatus == DM_GOT_VIDEO_FRAME ) {
      // converts to rgb only if a new frame is created
      img_convert_and_resize( deinterlace );
    }
    
    if( packetAllocated ) {
      // Release the packet
      av_free_packet(&moviePacket);
    }
  }

  //printf( "End of Goto next frame\n" );
  return (readStatus == DM_GOT_VIDEO_FRAME);
}

void dm_AudioVideoInData::queuePushPacket( dm_AudioVideoPacketQueue *queue, 
					   AVPacket *packet) {
  AVPacketList *packetQueueEntry;

  // Duplicate the packet to keep it from being invalidated by
  // the next av_read_frame() call
  av_dup_packet(packet);

  // Create a new packet queue entry
  packetQueueEntry = (AVPacketList *)malloc(sizeof(AVPacketList));
  packetQueueEntry->pkt = *packet;
  packetQueueEntry->next = NULL;

  // lock the queue mutex
#ifdef _WIN32
  WaitForSingleObject( queueMutex, INFINITE );
#else
  pthread_mutex_lock( &queueMutex );
#endif

  // Add the packet to the video packet queue
  if (queue->tail == NULL)
    queue->head = packetQueueEntry;
  else
    queue->tail->next = packetQueueEntry;

  // Update the tail of the queue
  queue->tail = packetQueueEntry;

  // Update the packet count
  queue->nbPackets++;

  // unlock the queue mutex
#ifdef _WIN32
  ReleaseMutex(queueMutex);
#else
  pthread_mutex_unlock(&queueMutex);
#endif
}

bool dm_AudioVideoInData::queuePullPacket( dm_AudioVideoPacketQueue *queue, 
					   AVPacket *packet) {
  AVPacketList *packetQueueEntry;

  // lock the queue mutex
#ifdef _WIN32
  WaitForSingleObject( queueMutex, INFINITE );
#else
  pthread_mutex_lock( &queueMutex );
#endif

  // Try to get the packet from the head of the queue
  packetQueueEntry = queue->head;

  if (packetQueueEntry != NULL) {
    // Update the queue
    queue->head = packetQueueEntry->next;
    if (queue->head == NULL)
      queue->tail = NULL;
    queue->nbPackets--;

    // Extract the packet from the queue element to the packet structure
    // provided and free the element
    *packet = packetQueueEntry->pkt;
    free(packetQueueEntry);

    // unlock the queue mutex
#ifdef _WIN32
    ReleaseMutex(queueMutex);
#else
    pthread_mutex_unlock(&queueMutex);
#endif

    // Return true to indicate a successful dequeue
    return true;
  }
  else {
    // unlock the queue mutex
#ifdef _WIN32
    ReleaseMutex(queueMutex);
#else
    pthread_mutex_unlock(&queueMutex);
#endif

    // Return false to indicate an empty queue
    return false;
  }
}
// @}

dm_Video::dm_Video( void ) {
  id = NULL;
  InitString( &id );
  fileName = NULL;
  InitString( &fileName );
  type = EmptyVideoType;
  video_source = DM_VIDEOFILE;
  id_live_video = 0;
  videoOn = false;
  nbPeriods = 0;
  timeInPeriod = 0;
  sourceInitialized = false;
  sourceOn = false;
  begin = 0;
  end = 0;
  duration = 0;
  period = 0;
  live_video_w = 640;
  live_video_w = 480;
  filmAudioVideo = new dm_AudioVideoInData(video_source);
  threadInit = false;
#ifdef _WIN32
  liveCapProc = NULL;
#endif
  convert_context = NULL;
  inFrame = NULL;
  outFrame = NULL;
  textureNo = -1;
  rectangle = false;
  external = false;
  id_live_video = -1;
}

dm_Video::~dm_Video(void) {
  if (id) {
    delete [] id;
    id = NULL;
  }
  
  if (fileName) {
    delete [] fileName;
    fileName = NULL;
  }
  
  if (filmAudioVideo) {
    if (filmAudioVideo->videoThread) {
      // 			delete filmAudioVideo->videoThread;
      // 			filmAudioVideo->videoThread = NULL;
    }
    //delete filmAudioVideo;
    //filmAudioVideo = NULL;
  }

#ifdef _WIN32
  if (liveCapProc) {
    delete liveCapProc;
    liveCapProc = NULL;
  }
#endif
  
//   if (convert_context) {
//     delete convert_context;
//     convert_context = NULL;
//   }
  
  if (inFrame) {
    delete inFrame;
    inFrame = NULL;
  }
  
  if (outFrame) {
    delete outFrame;
    outFrame = NULL;
  }
}

void dm_Video::update( void ) {
  // printf( "video name %s videoOn %d time %.5f frame %d\n" , 
//    fileName , videoOn , CurrentClockTime , FrameNo );

  // already updated in another instance of this video
  // nothing to do
  if( filmAudioVideo->lastOpenGLFrameUpdate == FrameNo ) {
    // printf( "video already updated \n" );
    return;
  }

  // notices whether a new frame is loaded in order to load
  // the corresponding texture in the graphic card
  filmAudioVideo->newFrame = false;

  // video loop: in case of spatializer: starts once/stops once
  // video loop: in case of playing interface: restarted at each period
  if( type == VIDEOLOOP ) {
    // outside the playing time
    if( CurrentClockTime < begin || CurrentClockTime > end ) {
      // printf( "outside interval CurrentClockTime %.2f begin %.2f end %.2f \n" , CurrentClockTime , begin , end  );
      if( videoOn ) {
	stop_video();
	videoOn = false;
      }
    }
    // inside the playing time
    else if( !videoOn ) {
      // printf( "off: loop restart\n" );
      if( filmAudioVideo->rewindable ) {
	if( !rewind_video() ) {
	  filmAudioVideo->rewindable = false;
	  // printf( "loop initial restart error\n" );
	}
      }
      else {
	// should not go to next frame
	// printf( "loop non initial restart error 1\n" );
	return;
      }
      videoOn = true;
      nbPeriods = 1;
      timeInPeriod = 0;
    }
     
    // computes the rank of the current period
    int indCurrentPeriod = (int)floorf((float)(CurrentClockTime - begin)/period) + 1; 
    // computes the time elapsed in the current period
    float currentTimeInPeriod 
      = (float)(CurrentClockTime - begin - (indCurrentPeriod - 1) * period);

    if( !videoOn ) {
      nbPeriods = 0;
      timeInPeriod = 0;
    }
    // printf( "  begin %.2f CurrentClockTime %.2f period %.2f nbPeriods %d indCurrentPeriod %d currentTimeInPeriod %.2f videoOn %d\n" , begin , CurrentClockTime , period , nbPeriods , indCurrentPeriod , currentTimeInPeriod , videoOn );

    // rewinds if a new period begins or if the
    // elapsed time in a period is less than the current elapsed time
    // as it is the case after a begin=now action
    // compares memorized time and period with current time and period
    if( videoOn && 
	(indCurrentPeriod != nbPeriods 
	 || currentTimeInPeriod < timeInPeriod) ) {
      if( filmAudioVideo->rewindable ) {
	if( !rewind_video() ) {
	  filmAudioVideo->rewindable = false;
	  // should not go to next frame
	  // printf( "loop non initial restart error 2\n" );
	  return;
	}
      }
      else {
	return;
      }
    }

    // memorizes the index of the current period 
    // and time elapsed in this period
    nbPeriods = indCurrentPeriod;
    timeInPeriod = currentTimeInPeriod;
    // inside the playing time
    // printf( "video loop name %s videoOn %d time [%.5f,%.5f] %.5f FrameNo %d\n" , fileName , videoOn , begin , end , CurrentClockTime,FrameNo);
    if( videoOn ) {
      // printf( "--> video loop name %s update %d time [%.5f,%.5f] %.5f FrameNo %d\n" , fileName , videoOn , begin , end , CurrentClockTime,FrameNo);

      bool readError = false;
      if( filmAudioVideo->GoToNextFrame( &readError , deinterlace ) ) {
	filmAudioVideo->newFrame = true;
	// printf( "GoToNextFrame true %d\n" , readError );
      }
      else {
	// printf( "GoToNextFrame false %d\n" , readError );
      }
      // it seems possible (at least for live video and avi films
      // to encounter read error without need to cancel reading
      // if( readError 
      //   && strcmp( fileName + strlen( fileName ) - 3 , "avi" ) != 0 ) {
      // printf( "Read error [%s]\n",fileName + strlen( fileName ) - 4);throw 0;
      // end = CurrentClockTime;
      // }
    }
  }
  // video loop

  // non looping video: starts once/stops once
  else if( type == VIDEOFINITE ) {
    // outside the playing time
    if( CurrentClockTime < begin || CurrentClockTime > end 
	|| CurrentClockTime > begin + duration ) {
      if( videoOn ) {
	stop_video();
	videoOn = false;
	// no more frame to read
	printf( "no more frame to read %.4f %.4f %.4f %.4f \n" , CurrentClockTime , begin , end , duration );
	return;
      }
    }
    // inside the playing time
    else {
      if( !videoOn ) {
	if( !rewind_video() ) {
	  filmAudioVideo->rewindable = false;
	  //printf( "finite restart error\n" );
	}
	videoOn = true;
      }
    }
    // inside the playing time
    if( videoOn ) {
      //printf( "video finite name %s update %d time [%.5f,%.5f] %.5f FrameNo %d\n" , fileName , videoOn , begin , end , CurrentClockTime,FrameNo);
      bool readError = false;
      if( filmAudioVideo->GoToNextFrame( &readError , deinterlace ) ) {
	filmAudioVideo->newFrame = true;
      }
      if( readError ) {
	printf( "read error\n" );
	// end = CurrentClockTime;
      }
    }
  } 
}

char *dm_Video::GetId( void ) {
  return id;
}

void dm_Video::SetId( char *newId ) {
  StrCpy( &id , newId );
}

void dm_Video::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
			      bool newText ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }

  *fileName = 0;
  if( !atRunTime && 
      XMLtag->hasAttribute( "xlink:href" ) ) {
    StrCpy( &fileName , XMLtag->getAttributeStringValue( "xlink:href" ) );
  }
      
  if( !atRunTime ) {
    deinterlace = false;
  }
  if( XMLtag->hasAttribute( "deinterlace" ) ) {
    XMLtag->BooleanFieldValue( "deinterlace" ,
			       &(deinterlace) );
  }

  // rectangular videos 
  if( !atRunTime ) {
    rectangle = false;
  }
  if( XMLtag->hasAttribute( "rectangle" ) ) {
    XMLtag->BooleanFieldValue( "rectangle" ,
			       &(rectangle) );
  }

  // external videos 
  if( !atRunTime ) {
    external = false;
  }
  if( XMLtag->hasAttribute( "external" ) ) {
    XMLtag->BooleanFieldValue( "external" , &(external) );
  }
  // printf( "EXTERN VIDEO is external %d\n" , (int)external );

  if( !atRunTime || 
      XMLtag->hasAttribute( "type" ) ) {
    char *thetype = XMLtag->getAttributeStringValue( "type" );
    // default type is "EmptyVideoType"
    type = EmptyVideoType;
    if( strcmp( thetype , "" ) == 0 ) {
      type = VIDEOFINITE; 
    }
    else {
      for( int ind = 0 ; ind < EmptyVideoType ; ind++ ) {
	if( strcmp( thetype , VideoTypeString[ind] ) == 0 ) {
	  type = (VideoType)ind;
	  break;
	}
      }
      if( type == EmptyVideoType ) {
	sprintf( ErrorStr , "Error: unknown video type [%s] in component ID [%s]!" , thetype , id ); ReportError( ErrorStr ); throw 249;
      }
    }
  }

  if( !atRunTime && 
      XMLtag->hasAttribute( "video_source" ) ) {
    char *thesource = XMLtag->getAttributeStringValue( "video_source" );
    // default type is "EmptyVideoSource"
    video_source = EmptyVideoSource;
    if( strcmp( thesource , "" ) == 0 ) {
      video_source = DM_VIDEOFILE; 
    }
    else {
      for( int ind = 0 ; ind < EmptyVideoSource ; ind++ ) {
	if( strcmp( thesource , VideoSourceString[ind] ) == 0 ) {
	  video_source = (VideoSource)ind;
	  break;
	}
      }
      if( video_source == EmptyVideoSource ) {
	sprintf( ErrorStr , "Error: unknown video source [%s] in component ID [%s]!" , thesource , id ); ReportError( ErrorStr ); throw 249;
      }
    }
    filmAudioVideo->liveVideo = (video_source == DM_VIDEOLIVE);
  }

  if(XMLtag->hasAttribute( "id_live_video" ) ) {
    operI( id_live_video ,
	   XMLtag->getAttributeIntValue( "id_live_video" , 
					 atRunTime ));
  }

  if( !atRunTime ) {
    begin = 0.0;
    end = 0.0;
    duration = 0.0;
    period = 0.0;
  }
  if( !atRunTime || 
      XMLtag->hasAttribute( "begin" ) ) {
    oper( begin ,
	  (float)XMLtag->getAttributeDoubleValue( "begin" , 
						  atRunTime ));
  }
  if( !atRunTime || 
      XMLtag->hasAttribute( "end" ) ) {
    oper( end , (float)XMLtag->getAttributeDoubleValue( "end" , 
							atRunTime ));
  }
  if( !atRunTime || 
      XMLtag->hasAttribute( "dur" ) ) {
    oper( duration , (float)XMLtag->getAttributeDoubleValue( "dur" , 
							     atRunTime ));
  }
  if( type == VIDEOLOOP ) {
    if( !atRunTime || 
	XMLtag->hasAttribute( "period" ) ) {
      oper( period 
	    , (float)XMLtag->getAttributeDoubleValue( "period" , 
						      atRunTime ));
      
      if( !period ) {
	sprintf( ErrorStr , "Error: null period [%.5f] in videoloop ID [%s]!" , period , id ); ReportError( ErrorStr ); throw 250;
      }
    }
  }
  if( !atRunTime ) {
    live_video_w = 640;
    live_video_h = 480;
  }
  if( XMLtag->hasAttribute( "live_video_w" ) ) {
    operI( live_video_w ,
	   XMLtag->getAttributeIntValue( "live_video_w" , 
					 atRunTime ));
  }
  if( XMLtag->hasAttribute( "live_video_h" ) ) {
    operI( live_video_h , 
	   XMLtag->getAttributeIntValue( "live_video_h" , 
					 atRunTime ));
  }

  // video  and audio threads
  if(!threadInit && newText)
    {
#ifdef _WIN32
      if(video_source == DM_VIDEOLIVE)
	{
	  // Create the capture driver
	  liveCapProc = capCreateCaptureWindow(NULL, WS_POPUP, 0, 0, 1, 1, 0, 0);//;
	  if(!capDriverConnect(liveCapProc, id_live_video))
	    {
	      sprintf(ErrorStr, "Error: Can't connect to capture driver!");
	      ReportError(ErrorStr);
	      throw 336;
	    }
	  // #################################### //
	  // This line display a dialog box to set the webcam image size and pixel format
	  //capDlgVideoFormat(liveCapProc);
	  // #################################### //
	  // Attach this video object to the capture driver
	  capSetUserData(liveCapProc, this);
	  // Set the callback function when a new frame is available
	  // from local to global variable because it cannot be
	  // passed as a parameter
	  liveVideoRectangle = rectangle;
	  capSetCallbackOnFrame(liveCapProc, webcam_frame);
	  // Set the image format information for the capture driver
	  PBITMAPINFO bInfo = new BITMAPINFO;
	  capGetVideoFormat(liveCapProc, bInfo, capGetVideoFormatSize(liveCapProc));

	  printf("Live video format %d x %d proposed %d x %d \n" , bInfo->bmiHeader.biWidth , bInfo->bmiHeader.biHeight , live_video_w, live_video_h);
	  bInfo->bmiHeader.biWidth = live_video_w;
	  bInfo->bmiHeader.biHeight = live_video_h;
	  bInfo->bmiHeader.biSizeImage = (DWORD)(live_video_w * live_video_h * ((float)(bInfo->bmiHeader.biBitCount) * 0.125F));

	  // Complete this switch with others image format if needed. Note that the output format must be BGR24.
	  switch(bInfo->bmiHeader.biCompression)
	    {
	    case FOURCC_BI_RGB:
	    case FOURCC_RGB:
	      init_convertion(PIX_FMT_NONE);
	      break;
	    case FOURCC_I420:
	    case FOURCC_IYUV:
	      init_convertion(PIX_FMT_YUV410P);
	      break;
		case FOURCC_YUY2: 
		  // PIX_FMT_YUV410P PIX_FMT_YUV444P 
		  // PIX_FMT_YUV420P PIX_FMT_YUV422P
		  // PIX_FMT_YUVJ444P PIX_FMT_YUV422
	      // PIX_FMT_YUV411P PIX_FMT_YUVJ420P
		  // PIX_FMT_UYVY422 PIX_FMT_UYVY411
	      // PIX_FMT_YUVJ422P
	      // PIX_FMT_RGB24 PIX_FMT_RGBA32
		  // PIX_FMT_NONE PIX_FMT_GRAY8
		  // PIX_FMT_PAL8
	      init_convertion(PIX_FMT_YUYV422);
	      break;
	    default:
	      sprintf(ErrorStr, "Error: Webcam image pixel format unrecognized %x\n" , bInfo->bmiHeader.biCompression);
	      ReportError(ErrorStr);
	      throw 336;
	      break;
	    }
	  capSetVideoFormat(liveCapProc, bInfo, capGetVideoFormatSize(liveCapProc));
	}
      else
	{
#endif
#ifdef _WIN32
	  DWORD a_threadId;
	  filmAudioVideo->videoThread 
	    = (HANDLE)CreateThread( NULL, 0, fillVideoBuffer, 
				    (PVOID)this, 0, &a_threadId);
	  if (filmAudioVideo->videoThread == NULL) {
	    sprintf( ErrorStr , "Error: Thread creation failed!" ); ReportError( ErrorStr ); throw 336;
	  }
	}
#else
      int res;
      res = pthread_create( &(filmAudioVideo->videoThread), NULL, 
			    fillVideoBuffer, this);
      if (res != 0) {
	sprintf( ErrorStr , "Error: Thread creation failed!" ); ReportError( ErrorStr ); throw 336;
      }
#endif
      threadInit = true;
    }
}

void dm_Video::init_convertion(PixelFormat input_pix_format)
{
  if(input_pix_format == PIX_FMT_NONE)
    {
      inFrame = NULL;
      outFrame = NULL;
      convert_context = NULL;
      return;
    }

  // Allocate the in and out frames
  inFrame = avcodec_alloc_frame();
  outFrame = avcodec_alloc_frame();

  if(inFrame == NULL || outFrame == NULL)
    {
      sprintf(ErrorStr, "Error: AV Frame allocation ERROR!");
      ReportError(ErrorStr);
      throw 336;
    }
  // Fill the in frame with the webcam image buffer
  //avpicture_fill((AVPicture *) iFrame, lpVHdr->lpData, PIX_FMT_YUV420P, video->live_video_w, video->live_video_h);

  // Fill the out frame with an empty buffer
  GLubyte * buffer = (GLubyte *)av_malloc(live_video_w * live_video_h * 3 * sizeof(GLubyte));
  avpicture_fill((AVPicture *) outFrame, buffer, PIX_FMT_BGR24, live_video_w, live_video_h);
  convert_context = sws_getContext(live_video_w, live_video_h, input_pix_format, live_video_w, live_video_h, PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
}

bool dm_Video::getParameters( char *attribute , 
			      DataType * dataType ,
			      double * valDouble ,
			      int * valInt ,
			      bool * valBool ,
			      char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "video_source" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)VideoSourceString[video_source];
    return true;
  }
  else if( strcmp( attribute , "xlink:href" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = fileName;
    return true;
  }
  else if( strcmp( attribute , "begin" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)begin;
    return true;
  }
  else if( strcmp( attribute , "end" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)end;
    return true;
  }
  else if( strcmp( attribute , "dur" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)duration;
    return true;
  }
  else if( strcmp( attribute , "period" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)period;
    return true;
  }
  else if( strcmp( attribute , "deinterlace" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = deinterlace;
    return true;
  }
  else if( strcmp( attribute , "live_video_w" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = live_video_w;
    return true;
  }
  else if( strcmp( attribute , "id_live_video" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = id_live_video;
    return true;
  }
  else if( strcmp( attribute , "live_video_h" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = live_video_h;
    return true;
  }
  return false;
}

void dm_Video::parse( int *fileDepth , int *p_c ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  // printf( "Parse video \n" );
  while( true ) {
    // new tag XML : video component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    // video closing tag
    if( strcmp( XMLtag->tag , "video" ) == 0 && (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
      XMLtag = NULL;
      return;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Video::print( FILE *file , int isLong , int depth ) {
  if( !(*id) ) {
    return;
  }
  indent( file , depth );
  fprintf( file , "<video id=\"%s\" xlink:href=\"%s\" type=\"%s\" begin=\"%.5f\" end=\"%.5f\" duration=\"%.5f\" period=\"%.5f\" live_video_w=\"%d\" live_video_h=\"%d\" deinterlace=\"%s\" video_source=\"%s\" rectangle=\"%s\" external=\"%s\"/>\n" , 
	   id , fileName , VideoTypeString[type]  , begin , end , duration , period  , live_video_w , live_video_h , BooleanString[BooleanToInt(deinterlace)], VideoSourceString[video_source] , BooleanString[BooleanToInt(rectangle)] , BooleanString[BooleanToInt(external)] );
}

// binary interpolation: random stepwise motion from an orgin
void dm_Video::interpolate( dm_Video *n1 , dm_Video *n2 ) {
  if( strcmp( n1->fileName , n2->fileName ) != 0 ) {
    strcpy( ErrorStr , "Error: Only identical videos can be interpolated!" ); ReportError( ErrorStr ); throw 270;
  }    
  begin += generateRandomFloat( -n2->begin , n2->begin );
  end += generateRandomFloat( -n2->end , n2->end );
  period += generateRandomFloat( -n2->period , n2->period );
  duration += generateRandomFloat( -n2->duration , n2->duration );
}

void dm_Video::interpolate( dm_Video **tabVideo , 
			    float *weights1 , 
			    float *weights2 , 
			    int numberOfComponents ,
			    float coef ) {
  for( int ind = 1 ; ind < numberOfComponents ; ind++ ) {
    if( strcmp( tabVideo[0]->fileName ,  tabVideo[ind]->fileName ) != 0 ) {
      strcpy( ErrorStr , "Error: Only identical videos can be interpolated!" ); ReportError( ErrorStr ); throw 271;
    }
  }    

  float coef2 = 1.0F - coef;
  float begin1 = 0.0;
  float begin2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    begin1 += weights1[ind] * tabVideo[ind]->begin;
    begin2 += weights2[ind] * tabVideo[ind]->begin;
  }
  begin = begin1 * coef + begin2 * coef2;
  
  float end1 = 0.0;
  float end2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    end1 += weights1[ind] * tabVideo[ind]->end;
    end2 += weights2[ind] * tabVideo[ind]->end;
  }
  end = end1 * coef + end2 * coef2;
  
  float duration1 = 0.0;
  float duration2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    duration1 += weights1[ind] * tabVideo[ind]->duration;
    duration2 += weights2[ind] * tabVideo[ind]->duration;
  }
  duration = duration1 * coef + duration2 * coef2;
  
  float period1 = 0.0;
  float period2 = 0.0;
  for( int ind = 0 ; ind < numberOfComponents ; ind++ ) {
    period1 += weights1[ind] * tabVideo[ind]->period;
    period2 += weights2[ind] * tabVideo[ind]->period;
  }
  period = period1 * coef + period2 * coef2;
}

void dm_Video::stop_video( void ) {
  // printf( "stop video\n" );
  videoOn = false;
}

bool dm_Video::rewind_video( void ) {
  if( !(filmAudioVideo->rgbFrame->external ) ) {
    if( !filmAudioVideo->rewind_video() ) {
      // printf( "rewind error\n" );
      videoOn = false;
      return false;
    }
  }
  else {
    if( !extern_rewind_video( filmAudioVideo->filmName ) ) {
      // printf( "rewind error\n" );
      videoOn = false;
      return false;
    }
  }

  // sets the video status videoOn
  videoOn = true;
  return true;
}


void dm_Video::displayVideo( DrawingMode mode , int textureNo ,
			     bool rectangle ) {
  // printf( "Display Video %s ID %d New frame %d\n" , fileName , DM_CompositionNode->GetTextureID( textureNo ),filmAudioVideo->newFrame);
#ifdef _WIN32
  if(filmAudioVideo->liveVideo)// && videoOn)
    {
      //printf("Capture webcam image\n");
      this->textureNo = textureNo;
      this->rectangle = rectangle;
      capGrabFrame(liveCapProc);
    }
  else
    {
#endif
      if( filmAudioVideo->newFrame && FrameNo > 10  ) {
	LoadVideoFrameInGPUMemory( textureNo  , 
				   DM_FRAGMENT_TEXTURE ,
				   rectangle );
      }
#ifdef _WIN32
    }
#endif
}

void dm_Video::LoadVideoFrameInGPUMemory( int textureNo , 
					  ShaderTextureTarget isVertexTexture ,
					  bool rectangle ) {
  // printf( "Display Video %s ID %d New frame %d\n" , fileName , DM_CompositionNode->GetTextureID( textureNo ),filmAudioVideo->newFrame);

  // not on: no new texture to display
  if ( !videoOn || textureNo < 0 )
    return;

  if( DM_CompositionNode->GetTextureID( textureNo ) == NULL_ID ) {
    GLuint texID = 0;
    glGenTextures(1, &texID);
    DM_CompositionNode->SetTextureID( textureNo , texID );
  }

  // these instructions are executed by  dm_Texture::draw that precedes dm_Video::bindTexture
  // binds the texture id
  // bind the texture to the texture arrays index and initXMLTag the texture
  // printf("Video no %d rect %d OpenGlID %d\n" , textureNo , rectangle , DM_CompositionNode->GetTextureID( textureNo ) );
  if( !rectangle ) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, DM_CompositionNode->GetTextureID( textureNo ));
  }
  else {
    glEnable(GL_TEXTURE_RECTANGLE);
    glBindTexture(GL_TEXTURE_RECTANGLE, DM_CompositionNode->GetTextureID( textureNo ));
  }
  GLenum format = GL_RGB;

#ifdef DM_HAVE_CG
  // VTF (vertex texture fetch)
  if( isVertexTexture == DM_VERTEX_TEXTURE ) {
    if( !rectangle ) {
      // printf( "Nearest interpolation %d\n" , GetTextureID( textureNo ));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
		      GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		      GL_NEAREST);
      
      // printf( "Loads glTexImage2D GL_RGB GL_RGBA_FLOAT32_ATI texture No %d internal_format=%d (%s)\n" , textureNo , GL_RGBA_FLOAT32_ATI , DM_CompositionNode->texture_names[textureNo] );
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI,
		   filmAudioVideo->rgbFrame->sizeX ,
		   filmAudioVideo->rgbFrame->sizeY , 
		   0,
		   format, GL_UNSIGNED_BYTE, 
		   (const void *)filmAudioVideo->rgbFrame->avPicture->data[0] );
    }
    else {
      // printf( "Nearest interpolation %d\n" , GetTextureID( textureNo ));
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
		      GL_NEAREST);
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
		      GL_NEAREST);
      
      // printf( "Loads glTexImage2D GL_RGB GL_RGBA_FLOAT32_ATI texture No %d internal_format=%d (%s)\n" , textureNo , GL_RGBA_FLOAT32_ATI , DM_CompositionNode->texture_names[textureNo] );
      glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA_FLOAT32_ATI,
		   filmAudioVideo->rgbFrame->sizeX ,
		   filmAudioVideo->rgbFrame->sizeY , 
		   0,
		   format, GL_UNSIGNED_BYTE, 
		   (const void *)filmAudioVideo->rgbFrame->avPicture->data[0] );
    }
  }
  else {
#endif
    if( !rectangle ) {
      // printf( "Loads GL_TEXTURE_2D" );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  
		      GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		      GL_LINEAR);
      
      // printf( "Loads GL_TEXTURE_2D texture No %d internal_format=%d (%s)\n" , textureNo ,format  , DM_CompositionNode->texture_names[textureNo] );
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
		   filmAudioVideo->rgbFrame->sizeX ,
		   filmAudioVideo->rgbFrame->sizeY , 
		   0,
		   format, GL_UNSIGNED_BYTE, 
		   (const void *)filmAudioVideo->rgbFrame->avPicture->data[0] );
      
    }
    else {
      // printf( "Loads GL_TEXTURE_RECTANGLE\n" );
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,  
		      GL_LINEAR);
      glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, 
		      GL_LINEAR);
      
      // printf( "Loads GL_TEXTURE_RECTANGLE texture No %d internal_format=%d (%s) %dx%d (%d)\n" , textureNo ,format  , DM_CompositionNode->texture_names[textureNo] ,filmAudioVideo->rgbFrame->sizeX , filmAudioVideo->rgbFrame->sizeY, (int)(filmAudioVideo->rgbFrame->avPicture->data[0][0]));
      glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB8,
		   filmAudioVideo->rgbFrame->sizeX ,
		   filmAudioVideo->rgbFrame->sizeY , 
		   0,
		   GL_RGB, GL_UNSIGNED_BYTE, 
		   (const void *)filmAudioVideo->rgbFrame->avPicture->data[0] );
    }
#ifdef DM_HAVE_CG
  }
#endif
}

void dm_Video::operator=(dm_Video& c) {
  StrCpy( &fileName , c.fileName );
  StrCpy( &id , c.id );
  begin = c.begin;
  end = c.end;
  period = c.period;
  duration = c.duration;
  live_video_w = c.live_video_w;
  live_video_h = c.live_video_h;
  deinterlace = c.deinterlace;
  type = c.type;
  video_source = c.video_source;
  id_live_video = c.id_live_video;
#ifdef _WIN32
  liveCapProc = c.liveCapProc;
#endif
  convert_context = c.convert_context;
  inFrame = c.inFrame;
  outFrame = c.outFrame;
  textureNo = c.textureNo;
  rectangle = c.rectangle;
  external = c.external;
}

dm_AudioVideoOutData::~dm_AudioVideoOutData(void) {
  if (videoFormatContext)
    {
      delete videoFormatContext;
      videoFormatContext = NULL;
    }

  if (videoStream)
    {
      delete videoStream;
      videoStream = NULL;
    }

  if (videoCodecContext)
    {
      delete videoCodecContext;
      videoCodecContext = NULL;
    }

  if (videoCodec)
    {
      delete videoCodec;
      videoCodec = NULL;
    }

  if (videoFrame)
    {
      delete videoFrame;
      videoFrame = NULL;
    }

  if (videoBuffer)
    {
      delete [] videoBuffer;
      videoBuffer = NULL;
    }
  videoBuffer_size = 0;

  if (videoParam)
    {
      delete videoParam;
      videoParam = NULL;
    }

  if (rgbFrame)
    {
      if (rgbFrame->avPicture)
	{
	  free(rgbFrame->avPicture);
	  rgbFrame->avPicture = NULL;
	}

      delete rgbFrame;
      rgbFrame = NULL;
    }
}

dm_AudioVideoOutData::dm_AudioVideoOutData( void ) {
  videoFormatContext = NULL;

  // VIDEO
  videoStream = NULL;
  videoCodecContext = NULL;
  videoCodec = NULL;
  // temporary storage of a video frame
  // (used to store the yuv image before its conversion into rgb)
  videoFrame = NULL;
  
  videoBuffer = NULL;
  videoBuffer_size = 0;

  // video parameters initialization
  videoParam = new dm_VideoParameters;
  videoParam->sizeX = 0;
  videoParam->sizeY = 0;
  videoParam->frameDuration = 0.0;
  videoParam->filmDuration = 0.0;

  // RGB frame
  rgbFrame = new dm_ImageRRGB;
  // allocates a frame that will hold the rgb image
  // pixel buffer initialized in a second step when the image size is known
  rgbFrame->avPicture = (AVPicture *)malloc(sizeof(AVPicture));
  // resets the rgb frame
  memset(rgbFrame, 0, sizeof(rgbFrame));
  rgbFrame->bytesPerPixel = 3;
  rgbFrame->sizeX = 0;
  rgbFrame->sizeY = 0;
  rgbFrame->sizeZ = 0;
  rgbFrame->avPicture->data[0] = NULL;
  rgbFrame->avPicture->data[1] = NULL;
  rgbFrame->avPicture->data[2] = NULL;
  rgbFrame->avPicture->data[3] = NULL;
  rgbFrame->avPicture->linesize[0] = 0;
  rgbFrame->avPicture->linesize[1] = 0;
  rgbFrame->avPicture->linesize[2] = 0;
  rgbFrame->avPicture->linesize[3] = 0;
  rgbFrame->rectangle = false;
  rgbFrame->external = false;
  rgbFrame->id_live_video = -1;

  // Initialize the SWS context to null until we want to use it for the first time
  context = NULL;

  // player management
  frameNo = 0;
}

void dm_AudioVideoOutData::add_video_stream( int codec_id ,
					     int videoWidth ,
					     int videoHeight ) {
  videoStream = av_new_stream(videoFormatContext, 0);
  if (!videoStream) {
    sprintf( ErrorStr , "Error: Could not alloc video stream!" ); ReportError( ErrorStr ); throw 336;
  }
    
  videoCodecContext = videoStream->codec;
  videoCodecContext->codec_id = (CodecID)codec_id;
  videoCodecContext->codec_type = CODEC_TYPE_VIDEO;

  /* put sample parameters */
  videoCodecContext->bit_rate = 400000;

  /* resolution must be a multiple of two */
  if( videoCodecContext->width % 2
      || videoCodecContext->height %2 ) {
    sprintf( ErrorStr , "Error: resolution must be a multiple of two!" ); ReportError( ErrorStr ); throw 336;
  }

  videoCodecContext->width = videoWidth;  
  videoCodecContext->height = videoHeight;
  if( videoCodecContext->width > 1024 
      || videoCodecContext->height > 1024 ) {
    sprintf( ErrorStr , "Error: screen size (%dx%d) exceeds max video size (1024x1024)!" , videoCodecContext->width , videoCodecContext->height ); ReportError( ErrorStr ); throw 336;
  }
  /* frames per second */
  // videoCodecContext->frame_rate = DM_OUTSTREAM_FRAME_RATE;  
  // videoCodecContext->frame_rate_base = 1;
  videoCodecContext->time_base.num = 1;
  videoCodecContext->time_base.den = DM_OUTSTREAM_FRAME_RATE;
  videoCodecContext->gop_size = 12; 

  /* compression level */
  //videoCodecContext->compression_level = 10; 

  /* format for mpeg video */
  videoCodecContext->pix_fmt = PIX_FMT_YUV420P;

  /* emit one intra frame every twelve frames at most */
  if (videoCodecContext->codec_id == CODEC_ID_MPEG2VIDEO) {
    /* just for testing, we also add B frames */
    videoCodecContext->max_b_frames = 2;
  }

  if (videoCodecContext->codec_id == CODEC_ID_MPEG1VIDEO){
    /* needed to avoid using macroblocks in which some coeffs overflow 
       this doesnt happen with normal video, it just happens here as the 
       motion of the chroma plane doesnt match the luma plane */
    videoCodecContext->mb_decision=2;
  }
  // some formats want stream headers to be seperate
  if(!strcmp(videoFormatContext->oformat->name, "mp4") 
     || !strcmp(videoFormatContext->oformat->name, "mov") 
     || !strcmp(videoFormatContext->oformat->name, "3gp"))
    videoCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
}

AVFrame *dm_AudioVideoOutData::alloc_videoFrame( int pix_fmt, 
						 int width, int height ) {
  AVFrame *videoFr;
  uint8_t *videoFrame_buf;
  int size;
    
  videoFr = avcodec_alloc_frame();
  if (!videoFr)
    return NULL;
  size = avpicture_get_size(pix_fmt, width, height);
  videoFrame_buf = new uint8_t[ size ];
  if (!videoFrame_buf) {
    av_free(videoFr);
    return NULL;
  }
  avpicture_fill((AVPicture *)videoFr, videoFrame_buf, 
		 pix_fmt, width, height);
  return videoFr;
}
    
void dm_AudioVideoOutData::open_video( void ) {
  videoCodecContext = videoStream->codec;

  /* find the video encoder */
  videoCodec = avcodec_find_encoder(videoCodecContext->codec_id);
  if (!videoCodec) {
    sprintf( ErrorStr , "Error: video codec not found!" ); ReportError( ErrorStr ); throw 336;
  }

  /* open the codec */
  if (avcodec_open(videoCodecContext, videoCodec) < 0) {
    sprintf( ErrorStr , "Error: video codec not opened!" ); ReportError( ErrorStr ); throw 336;
  }

  videoBuffer = NULL;
  if (!(videoFormatContext->oformat->flags & AVFMT_RAWPICTURE)) {
    /* allocate output buffer */
    /* XXX: API change will be done */
    videoBuffer_size = 200000;
    videoBuffer = new uint8_t[ videoBuffer_size ];
  }

  // rgb frame points to back buffer
  rgbFrame->sizeX = videoCodecContext->width;
  rgbFrame->sizeY = videoCodecContext->height;
  rgbFrame->bytesPerPixel = 3;
  rgbFrame->avPicture->data[0] 
    = new GLubyte[ videoCodecContext->width 
		   * videoCodecContext->height * 3 ];
  rgbFrame->avPicture->linesize[ 0 ] 
    = videoCodecContext->width * 3;

  /* allocate the encoded raw videoFrame */
  videoFrame = alloc_videoFrame( videoCodecContext->pix_fmt, 
				 videoCodecContext->width, 
				 videoCodecContext->height);
  if (!videoFrame ) {
    sprintf( ErrorStr , "Error: video frame not allocated (fmt %d, %dx%d)!" , videoCodecContext->pix_fmt, videoCodecContext->width, videoCodecContext->height); ReportError( ErrorStr ); throw 336;
  }

  //   /* if the output format is not YUV420P, then a temporary YUV420P
  //      video frame is needed too. It is then converted to the required
  //      output format */
  //   aux_videoFrame = NULL;
  //   if (videoCodecContext->pix_fmt != PIX_FMT_YUV420P) {
  //     aux_videoFrame = alloc_videoFrame( PIX_FMT_YUV420P, 
  // 				       videoCodecContext->width, 
  // 				       videoCodecContext->height);
  //     if (!aux_videoFrame) {
  //       sprintf( ErrorStr , "Error: temporary video frame not allocated!" ); ReportError( ErrorStr ); throw 336;
  //     }
  //   }
}

void dm_AudioVideoOutData::write_video_frame( void ) {
  int out_size, ret;
  AVFrame *videoFrame_ptr;
    
  videoCodecContext = videoStream->codec;
    
  if (frameNo >= videoParam->filmDuration / videoParam->frameDuration ) {
    /* no more frame to compress. The codec has a latency of a few
       frames if using B frames, so we get the last frames by
       passing a NULL videoFrame */
    videoFrame_ptr = NULL;
  } else {
    /* OpenGL's default 4 byte pack alignment would leave extra bytes at the
       end of each image row so that each full row contained a number of bytes
       divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit componets would
       be laid out like "RGBRGBRGBxxx" where the last three "xxx" bytes exist
       just to pad the row out to 12 bytes (12 is divisible by 4). To make sure
       the rows are packed as tight as possible (no row padding), set the pack
       alignment to 1. */
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    //     memset( rgbFrame->avPicture->data[0], (GLubyte)255, 
    // 	    rgbFrame->sizeX * rgbFrame->sizeY 
    // 	    * rgbFrame->bytesPerPixel );

    if (videoCodecContext->pix_fmt != PIX_FMT_RGB24 ) {
      /* as we only generate a YUV420P videoFrame, we must convert it
	 to the codec pixel format if needed */
      //             fill_yuv_image( aux_videoFrame, frameNo, 
      // 			    videoCodecContext->width, 
      // 			    videoCodecContext->height );
      // copies the back buffer
      uint8_t bufferVideo[1024 * 1024 * 3];
      glReadPixels(0, 0, videoCodecContext->width, videoCodecContext->height, 
		   GL_RGB, GL_UNSIGNED_BYTE, bufferVideo);

      // image is upside down
      uint8_t *dest = rgbFrame->avPicture->data[0];
      for(int n = 0 ; n < videoCodecContext->height ; n++ ) {
#ifdef _WIN32
	memmove ((void *) &bufferVideo[n * videoCodecContext->width * 3 ], 
		 (void *) &dest[(videoCodecContext->height - (n + 1)) 
				* videoCodecContext->width * 3 ], 
		 videoCodecContext->width * 3);
#else
	bcopy( &bufferVideo[n * videoCodecContext->width * 3 ], 
	       &dest[(videoCodecContext->height - (n + 1)) 
		     * videoCodecContext->width * 3 ], 
	       videoCodecContext->width * 3 );
#endif
      }

      if(!context)
	{
	  context = sws_getContext(videoCodecContext->width, videoCodecContext->height, PIX_FMT_RGB24, videoCodecContext->width, videoCodecContext->height, videoCodecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
	  if(!context)
	    {
	      sprintf(ErrorStr, "Error: can't initialize SWS context!");
	      ReportError(ErrorStr);
	      throw 336;
	    }
	}
      sws_scale(context, rgbFrame->avPicture->data, rgbFrame->avPicture->linesize, 0, videoCodecContext->height, videoFrame->data, videoFrame->linesize);
    } else {
      glReadPixels(0, 0, videoCodecContext->width, videoCodecContext->height, 
		   GL_RGB, GL_UNSIGNED_BYTE, 
		   videoFrame->data[0]);
      //             fill_yuv_image(videoFrame, frameNo, 
      // 			   videoCodecContext->width, 
      // 			   videoCodecContext->height);
    }
    videoFrame_ptr = videoFrame;
  }

    
  if (videoFormatContext->oformat->flags & AVFMT_RAWPICTURE) {
    /* raw video case. The API will change slightly in the near
       futur for that */
    av_init_packet(&moviePacket);
        
    moviePacket.flags |= PKT_FLAG_KEY;
    moviePacket.stream_index= videoStream->index;
    moviePacket.data= (uint8_t *)videoFrame_ptr;
    moviePacket.size= sizeof(AVPicture);
        
    ret = av_write_frame(videoFormatContext, &moviePacket);
  } else {
    for( int ind = 0 ; ind < 2 ; ind++ )
      {
    /* encode the image */
    out_size = avcodec_encode_video( videoCodecContext, videoBuffer, 
				     videoBuffer_size, videoFrame_ptr);
    /* if zero size, it means the image was buffered */
    if (out_size != 0) {
      av_init_packet(&moviePacket);
            
      moviePacket.pts= videoCodecContext->coded_frame->pts;
      if(videoCodecContext->coded_frame->key_frame)
	moviePacket.flags |= PKT_FLAG_KEY;
      moviePacket.stream_index= videoStream->index;
      moviePacket.data= videoBuffer;
      moviePacket.size= out_size;
            
      /* write the compressed frame in the media file */
      ret = av_write_frame(videoFormatContext, &moviePacket);
    } else {
      ret = 0;
    }
      }
  }
  if (ret != 0) {
    sprintf( ErrorStr , "Error: error while writing video frame!" ); ReportError( ErrorStr ); throw 336;
  }
  frameNo++;
}

void dm_AudioVideoOutData::close_video( void ) {
  avcodec_close(videoStream->codec);
  av_free(videoFrame->data[0]);
  av_free(videoFrame);
  if (rgbFrame) {
	  if( rgbFrame->external ) {
		extern_close_all();
	 }
    if (rgbFrame->avPicture) {
      av_free(rgbFrame->avPicture->data[0]);
      av_free(rgbFrame->avPicture);
    }
    av_free(rgbFrame);
  }
  av_free(videoBuffer);
}

void dm_AudioVideoOutData::close_audio_video_out( void ) {
  /* close each codec */
  if( videoStream )
    close_video();

  /* close the output file */
  printf("close video context\n");
  if( videoFormatContext ) {
    /* write the trailer, if any */
    av_write_trailer(videoFormatContext);
    
    // free the stream
    av_free(videoFormatContext);
    videoFormatContext = NULL;
  }
  printf("close video end\n");
}

/**************************************************************/
/* media file output */

// /* prepare a dummy image */
// void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
// {
//   int x, y, i;

//   i = frame_index;

//   /* Y */
//   for(y=0;y<height;y++) {
//     for(x=0;x<width;x++) {
//       pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
//     }
//   }
    
//   /* Cb and Cr */
//   for(y=0;y<height/2;y++) {
//     for(x=0;x<width/2;x++) {
//       pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
//       pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
//     }
//   }
// }


dm_AudioVideoOutData *open_audio_video_out( char *filename ,
					    int videoWidth ,
					    int videoHeight ) {
  dm_AudioVideoOutData *avOutData;
  AVOutputFormat *fmt;
  
  // initializes the codec list
  if( !codecInitialized ) {
    avcodec_init();
    // Register all formats and codecs
    avcodec_register_all();
    av_register_all();
	#ifndef _WIN32
		//Register all devices
		avdevice_register_all(); 
	#endif

    codecInitialized = true;
  }
  
  /* auto detect the output format from the name. default is
     mpeg. */
  fmt = guess_format(NULL, filename, NULL);
  if (!fmt) {
    printf("Could not deduce output format from file extension: using MPEG.\n");
    fmt = guess_format("mpeg", NULL, NULL);
  }
  if (!fmt) {
    sprintf( ErrorStr , "Error: suitable video output format not found, check file extension %s!" , filename ); ReportError( ErrorStr ); throw 336;
  }
    
  /* allocate the output media context */
  avOutData = new dm_AudioVideoOutData();

  #ifndef _WIN32
	avOutData->videoFormatContext = avformat_alloc_context();
  #endif
  
  if (!avOutData->videoFormatContext) {
    sprintf( ErrorStr , "Error: audio/video memory error!" ); ReportError( ErrorStr ); throw 336;
  }
  avOutData->videoFormatContext->oformat = fmt;
#ifdef _WIN32
  _snprintf(avOutData->videoFormatContext->filename, 
	    sizeof(avOutData->videoFormatContext->filename), 
	    "%s", filename);
#else
  snprintf(avOutData->videoFormatContext->filename, 
	   sizeof(avOutData->videoFormatContext->filename), 
	   "%s", filename);
#endif

  /* add the audio and video streams using the default format codecs
     and initialize the codecs */
  // printf( "Guessed format %d\n" , fmt->video_codec );
  if( fmt->video_codec != CODEC_ID_NONE ) {
    avOutData->add_video_stream(fmt->video_codec , videoWidth , videoHeight );
  }

  /* set the output parameters (must be done even if no
     parameters). */
  if (av_set_parameters(avOutData->videoFormatContext, NULL) < 0) {
    sprintf( ErrorStr , "Error: Invalid video output format parameters!" ); ReportError( ErrorStr ); throw 336;
  }

  dump_format( avOutData->videoFormatContext , 0 , filename , 1 );

  /* now that all the parameters are set, we can open the
     video codecs and allocate the necessary encode buffers */
  if( avOutData->videoStream )
    avOutData->open_video();

  /* open the output file, if needed */
  if (!(fmt->flags & AVFMT_NOFILE)) {
    if (url_fopen(&avOutData->videoFormatContext->pb, filename, URL_WRONLY) < 0) {
      sprintf( ErrorStr , "Error: video output file not opened %s!" , filename ); ReportError( ErrorStr ); throw 336;
    }
  }
    
  /* write the stream header, if any */
  av_write_header( avOutData->videoFormatContext );

  return avOutData;
}

void dm_AudioVideoOutData::write_frame( void ) {
  double video_pts = 0.0;

    if (videoStream)
      video_pts = (double)videoStream->pts.val 
	* videoStream->time_base.num / videoStream->time_base.den;
    else
      video_pts = 0.0;

    write_video_frame();

}


#endif  // DM_HAVE_FFMPEG
