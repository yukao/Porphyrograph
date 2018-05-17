/*! \file dm-video.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-film.h
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


using namespace std;
#include <deque>


#ifndef DM_FILM_H
#define DM_FILM_H

#ifdef _WIN32
typedef HANDLE pthread_t;
typedef HANDLE pthread_mutex_t;
#endif

#define DM_VIDEO_PACKET_QUEUE_SIZE 8

#define DM_OUTSTREAM_FRAME_RATE 25 /* 25 images/s */

enum VideoType{ VIDEOFINITE = 0 , VIDEOLOOP , EmptyVideoType};
extern const char *VideoTypeString[EmptyVideoType + 1];

enum ReadNextFrameStatus{ DM_READ_FRAME_ERROR = 0 , DM_GOT_VIDEO_FRAME , DM_GOT_NO_VIDEO_FRAME };

enum VideoSource{ DM_VIDEOFILE = 0, DM_VIDEOLIVE , DM_VIDEOSTREAM , EmptyVideoSource };
extern const char *VideoSourceString[EmptyVideoSource + 1];

	#define FOURCC_BI_RGB			0x00000000
	#define FOURCC_BI_RLE8			0x00000001
	#define FOURCC_BI_RLE4			0x00000002
	#define FOURCC_BI_BITFIELDS		0x00000003
	#define FOURCC_Y8				0x20203859
	#define FOURCC_Y16				0x20363159
	#define FOURCC_Y800				0x30303859
	#define FOURCC_V210				0x30313256
	#define FOURCC_I420				0x30323449
	#define FOURCC_Y211				0x31313259
	#define FOURCC_Y411				0x31313459
	#define FOURCC_NV21				0x3132564E
	#define FOURCC_IY41				0x31345949
	#define FOURCC_IMC1				0x31434D49
	#define FOURCC_IYU1				0x31555949
	#define FOURCC_CXY1				0x31595843
	#define FOURCC_NV12				0x3231564E
	#define FOURCC_YV12				0x32315659
	#define FOURCC_V422				0x32323456
	#define FOURCC_Y422				0x32323459
	#define FOURCC_RGB				0x32424752
	#define FOURCC_IMC2				0x32434D49
	#define FOURCC_IYU2				0x32555949
	#define FOURCC_YUY2				0x32595559
	#define FOURCC_CXY2				0x32595842
	#define FOURCC_RAW				0x32776173
	#define FOURCC_IMC3				0x33434D49
	#define FOURCC_IMC4				0x34434D49
	#define FOURCC_RLE				0x34454C52
	#define FOURCC_V655				0x35353656
	#define FOURCC_YV16				0x36315659
	#define FOURCC_RLE8				0x38454C52
	#define FOURCC_IF09				0x39304649
	#define FOURCC_YVU9				0x39555659
	#define FOURCC_YUV9				0x39565559
	#define FOURCC_RGBA				0x41424752
	#define FOURCC_Y41B				0x42313459
	#define FOURCC_Y42B				0x42323459
	#define FOURCC_HDYC				0x43594448
	#define FOURCC_CLPL				0x4C504C43
	#define FOURCC_Y41P				0x50313459
	#define FOURCC_YUVP				0x50565559
	#define FOURCC_UYVP				0x50565955
	#define FOURCC_CLJR				0x524A4C43
	#define FOURCC_Y41T				0x54313459
	#define FOURCC_Y42T				0x54323459
	#define FOURCC_RGBT				0x54424752
	#define FOURCC_YVYU				0x55595659
	#define FOURCC_YUNV				0x564E5559
	#define FOURCC_UYNV				0x564E5955
	#define FOURCC_AYUV				0x56555941
	#define FOURCC_IYUV				0x56555949
	#define FOURCC_IUYV				0x56595549
	#define FOURCC_YUYV				0x56595559
	#define FOURCC_IRAW				0x57615349
	#define FOURCC_GREY				0x59455247
	#define FOURCC_VYUY				0x59555956
	#define FOURCC_UYVY				0x59565955
	#define FOURCC_CYUV				0x76757963


/*!
 * \brief video stream input
 *
 * Packet queue for audio and video packets
 */
struct dm_AudioVideoPacketQueue
{
  AVPacketList              *head;
  AVPacketList              *tail;
  int                        nbPackets;

  void            flushQueue( pthread_mutex_t queueMutex );
};

/// stores RGB image
struct dm_ImageRRGB
{
  int                        bytesPerPixel;
  int                        sizeX;
  int                        sizeY;
  int                        sizeZ;
  bool                       rectangle;
  bool                       external;
  int                        id_live_video;
  
  AVPicture                 *avPicture;
};

/// stores film parameters
struct dm_VideoParameters
{
  int                        sizeX;
  int                        sizeY;
  double                     frameDuration;
  double                     filmDuration;
};

void greenDot( uint8_t *pixMap , int x , int y , int width , int size );
float maxrgb( float r,float g,float b);
float minrgb( float r,float g,float b);
void rgb2hsv( float r, float g, float b,
	      float *hout, float *sout, float *vout );
float rgb2h(float r, float g, float b);
void hsv2rgb( float hin, float s, float v,
	      float *rout, float *gout, float *bout );

/// stores data for playing films or live videos with or without audio
class dm_AudioVideoInData {
 public:
        
  /*! \name Group audio-video
   *  \brief audio-video format
   */
  // \{
  AVFormatContext             *videoFormatContext;
  char                        *filmName;
  // \}

  /*! \name Group video
   *  \brief video
   */
  // \{
  AVCodecContext              *videoCodecContext;
  AVCodec                     *videoCodec;
  // \}

  /*! \name Group video_storage
   *  \brief temporary storage of a video frame
   * (used to store the yuv image before its conversion into rgb)
   */
  // \{
  AVFrame                     *videoFrame;
  int                          videoStreamIndex;
  AVPacket                     moviePacket;
  // \}

  /// boolean for live video
  bool                         liveVideo;

  /// boolean for stream video
  bool                         streamVideo;

  /*! \name Group cur_rgb_img
   *  \brief current rgb image
   */
  // \{
  dm_ImageRRGB                *rgbFrame;
  int                          NPOTSizeX;
  int                          NPOTSizeY;
  /// SWS Context for colorspace conversion
  SwsContext                 *context;
  // \}

  /// notices whether a new frame is loaded in order to load
  /// the corresponding texture in the graphic card
  bool                         newFrame;

  /// if the video is not rewindable, does not try to loop
  bool                         rewindable;

  dm_AudioVideoPacketQueue    *videoQueue;

  dm_VideoParameters          *videoParam;

  int                          indVideoStream;

 

  /*! \name Group video_thread
   *  \brief the video thread
   */
  // \{
  pthread_t                    videoThread;
  pthread_mutex_t              videoMutex;
  // \}

  /// queue mutual exclusive access
  pthread_mutex_t              queueMutex;

#ifdef _WIN32
  /*! \name Group mutex_name
   *  \brief mutex names (for Window$)
   */
  // \{
  LPCTSTR lpszVideoMutex;
  LPCTSTR lpszQueueMutex;
  // \}
#endif

  /*! \name Group player
   *  \brief player management
   */
  // \{

  double                       currentFrameTime;
  double                       totalFileTime;
  double                       lastAbsoluteTime;
  int                          frameNo;
  // \}

  /// stores the last OpenGL frame number
  /// where this video has been updated so that not to
  /// update more thant once per OpenGL frame
  int                          lastOpenGLFrameUpdate;

  /*! \name Group streaming
   *  \brief streaming
   */
  // \{
  dm_IPServer                *videoServer;
  // \}

   /*
    * \brief Film constructor
    * \param video_source			to be added
    */
  dm_AudioVideoInData(VideoSource video_source);
  ~dm_AudioVideoInData(void);

  void                     img_convert_and_resize( bool deinterlace );
  void                     img_convert_and_resize2( bool deinterlace );
  
  /// gets next frame from the video buffer
  ReadNextFrameStatus      readNextFrame( void );
  bool                     loadFilm( char *fname , int id_live_video ,
				     int live_video_w , int live_video_h  , 
				     bool rectangle , bool external );
  bool                     loadServer( char *servername , bool rectangle );

  /*!
   * \brief film loader
   * \param fname			to be added
   *
   * video access: load - frame by frame access - rewind
   */
  bool                     loadVideo( char *fname );

  /*!
   * \brief packet queue managementF
   * \param queue				to be added
   * \param packet			to be added
   *
   * adds to a queue
   */
  void                     queuePushPacket( dm_AudioVideoPacketQueue *queue,
					    AVPacket *packet );

  /*!
   * \brief removes from a queue
   * \param queue				to be added
   * \param packet			to be added
   *
   */
  bool                     queuePullPacket( dm_AudioVideoPacketQueue *queue,
					    AVPacket *packet );

  bool rewind_video( void );

  bool GoToNextFrame( bool *readError , bool deinterlace );
};

/// class that stores VirChor parameters for dynamic video play
class  dm_Video {
 private:
  char                       *id;
 public:
  char                       *fileName;

  /*! \name Group work_var
   *  \brief working variables
   */
  // \{
  int                        videoOn;
  int                        nbPeriods;
  float                      timeInPeriod;
  bool                       sourceInitialized;
  bool                       sourceOn;
  // \}

  /*! \name Group specific_attri
   *  \brief specific attributes for repeated videos
   */
  // \{
  float                      begin;
  float                      end;
  float                      period;
  float                      duration;
  // \}

  /*! \name Group dimension_video
   *  \brief dimensions of a live video (because they are not guessed from the stream
   */
  // \{
  int                        live_video_w;
  int                        live_video_h;

  VideoType                  type;
  VideoSource                video_source;
  int			     id_live_video;
  // \}

  /// boolean for deinterlacing
  bool                       deinterlace;

  /// boolean for thread initialization
  bool                       threadInit;

  /*! \name handle_video
   *  \brief Handle for the live video capture in Visual Studio
   * an environment map shares the same (OpenGL and) VC ID
   * for this reason we need to store this shared ID so that we can update
   * the texture if it is changed some time
   */
  // \{
#ifdef _WIN32
  HWND                       liveCapProc;
#endif
  AVFrame *		     inFrame;
  AVFrame *		     outFrame;
  struct SwsContext *	     convert_context;
  int		             textureNo;
  // \}

  dm_AudioVideoInData       *filmAudioVideo;
  bool		             rectangle;
  bool                       external;

  dm_Video( void );
  ~dm_Video(void);

  /// updating a video
  void update( void );

  /// parsing a video
  char *GetId( void );
  void SetId( char *newId );
  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      bool newText ,
		      FloatBinOp oper ,
		      IntBinOp operI );


  /*!
   * \brief reading a video
   * \param attribute								to be added
   * \param dataType							to be added
   * \param valDouble							to be added
   * \param valInt										to be added
   * \param valBool								to be added
   * \param valString								to be added
   * \param tableOrMatrixSize			to be added
   * \param valScalars							to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );
  void parse( int *fileDepth , int *p_c );
  void print( FILE *file , int isLong , int depth );
  bool rewind_video( void );

  /// video frame by frame openGL display
  void stop_video( void );
  void displayVideo( DrawingMode mode , int textureNo ,
		     int environmentMapTextureNo , 
		     bool rectangle );


  /*! 
   * \brief n-ary interpolation
   * \param tabVideo									to be added
   * \param weights1									to be added
   * \param weights2									to be added
   * \param numberOfComponents		to be added
   * \param coef											to be added
   */
  void interpolate( dm_Video **tabVideo , 
		    float *weights1 , 
		    float *weights2 , 
		    int numberOfComponents ,
		    float coef );
  void interpolate( dm_Video *n1 , dm_Video *n2 );

  /*!
   * \brief binds a frame to a texture (adn transfers the bitmap to the GPU)
   * \param textureNo						to be added
   * \param isVertexTexture			to be added
   */
  void LoadVideoFrameInGPUMemory( int textureNo  , 
				  int environmentMapTextureNo ,
				  ShaderTextureTarget isVertexTexture ,
				  bool rectangle );

  void operator=(dm_Video&);

  /*!
   * \brief initialise the frames and convertion context
   * \param input_pix_format			to be added
   */
  void init_convertion(PixelFormat input_px_format);
};

/*!
 * \brief video stream output
 *
 * stores data for creating films with or without audio
 */
class dm_AudioVideoOutData {
 public:
  /// audio-video format
  AVFormatContext             *videoFormatContext;

  /*! \name Group video
   *  \brief video
   */
  // \{
  AVStream                    *videoStream;
  AVCodecContext              *videoCodecContext;
  AVCodec                     *videoCodec;
  // \}

  /*! \name Group temporary_video
   *  \brief temporary storage of a video frame (used to store the yuv image before its conversion into rgb)
   */// \{
  AVFrame                     *videoFrame;
  AVPacket                     moviePacket;
  
  uint8_t                     *videoBuffer;
  int                          videoBuffer_size;
  // \}

  /// current rgb image
  dm_ImageRRGB                *rgbFrame;
  /// SWS Context for colorspace conversion
  SwsContext                 *context;
  
  dm_VideoParameters          *videoParam;

  /// player management
  int                          frameNo;

  dm_AudioVideoOutData();
  ~dm_AudioVideoOutData(void);

  /*!
   * \brief add a video output stream
   * \param codec_id					to be added
   * \param videoWidth				to be added
   * \param videoHeight			to be added
   */
  void add_video_stream( int codec_id ,
			 int videoWidth ,
			 int videoHeight );
  AVFrame *alloc_videoFrame( int pix_fmt, int width, int height );
  void open_video( void );
  void write_video_frame( void );

  /// closes video and release memory
  void close_video( void );

  /*!
   * \brief closes audio and video, writes trailer and release memory
   * \param withAudio			to be added
   * \param withVideo			to be added
   */
  void close_audio_video_out( void );
  void write_frame( void );
};

dm_AudioVideoOutData *open_audio_video_out( char *filename ,
					    int videoWidth ,
					    int videoHeight );

extern int 				*deadpixelflags;
extern int 				*newpixelflags;
extern int 				iNumDeadpixels;
extern int 				iNumNewpixels;
extern int 				*pixelLookupX;
//stores the lookup table.for coordinate x from pixelindex .
extern int 				*pixelLookupY;

extern int extern_close_all( void );

#endif
