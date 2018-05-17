////////////////////////////////////////////////////////////////////
//
//
//
//     File pg-texture.cpp
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; TrackDisplay even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "pg-all_include.h"

////////////////////////////////////////////////////////////////////
// IMAGE TEXTURES
////////////////////////////////////////////////////////////////////
// Images used for displaying or for swapping
// ImageBufferDataStruct describing a buffer of images stored in the GPU
// these images are used to make piled rendering
// they are doubled by swap images used to smoothly change between images
ImageBufferDataStruct image_buffer_data[PG_IMAGE_NB_TEXTURES] = { ImageBufferDataStruct() };
ImageBufferDataStruct image_mask_buffer_data[PG_IMAGE_NB_TEXTURES / 3] = { ImageBufferDataStruct() };
ImageBufferDataStruct image_swap_buffer_data[PG_IMAGE_NB_TEXTURES] = { ImageBufferDataStruct() };
// interpolation weight between image buffer swap buffer in each layer
GLfloat image_buffer_alphaSwap02[PG_IMAGE_NB_TEXTURES / 2] = { 0.0f, 0.0f, 0.0f };
GLfloat image_buffer_alphaSwap35[PG_IMAGE_NB_TEXTURES / 2] = { 0.0f, 0.0f, 0.0f };
// image buffer layer weights
GLfloat image_buffer_weights[PG_IMAGE_NB_TEXTURES] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat image_buffer_position_noises[PG_IMAGE_NB_TEXTURES*2] = { 0.0f };
GLfloat mask_buffer_position_noises[PG_IMAGE_NB_TEXTURES * 2] = { 0.0f };
// GLfloat mask_inverse_buffer_position_noises[PG_IMAGE_NB_TEXTURES * 2] = { 0.0f };

// the index from which an image available for swapping is looked for
int IndInitialSwapImage = 0;

////////////////////////////////////////////////////////////////////
// OPENGL CONTEXTS FOR THREADS MANAGEMENT
////////////////////////////////////////////////////////////////////
int GetFreeOpenGLContext( void ) {
  for( int indContext = 0 ; indContext < PG_MAX_NUMBER_OF_OPENGL_CONTEXTS ; indContext++ ) {
	if( ThreadOpenGLTextureData[indContext].is_free ) {
	  return indContext;
	}
  }
  return -1;
}

////////////////////////////////////////////////////////////////////
// IMAGE FILES
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// image index management for progessive image turnover
// according to image swap frequency and duration
// and according to directory size
int IndPlayedImageFileDir = 0;
int IndPlayedImageFile = 0;

std::string ImageDir = "./Data/TVW-data/images/TVW-mirror";
std::string MaskDir = "./Data/TVW-data/masks/TVW-mirror";


////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE INITIALIZATION
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool pg_initTextures(void) {
	pg_screen_msg_offset_texture =
		(GLubyte *)pg_generateTexture(&pg_screen_msg_offset_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_screen_msg_offset_texture) {
		sprintf(ErrorStr, "Error: screen message bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	// glGenTextures(1, &(pg_screen_msg_offset_texID));

	pg_displayMsg1_offset_texture =
		(GLubyte *)pg_generateTexture(&pg_displayMsg1_offset_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg1_offset_texture) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	//glGenTextures(1, &(pg_displayMsg1_offset_texID));
	pg_displayMsg2_offset_texture =
		(GLubyte *)pg_generateTexture(&pg_displayMsg2_offset_texID, pg_byte_tex_format,
			message_pixel_length, 1);
	if (!pg_displayMsg2_offset_texture) {
		sprintf(ErrorStr, "Error: display message bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	//glGenTextures(1, &(pg_displayMsg2_offset_texID));

	pg_hashtag_offset_texture =
		(GLubyte *)pg_generateTexture(&pg_hashtag_offset_texID, pg_byte_tex_format,
			hashtag_pixel_length, 2);
	if (!pg_hashtag_offset_texture) {
		sprintf(ErrorStr, "Error: hashtag bitmap not allocated!"); ReportError(ErrorStr); throw 336;
	}
	//glGenTextures(1, &(pg_hashtag_offset_texID));

	for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
		pg_tracks_Pos_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_Pos_Texture[indTrack] = NULL;
		pg_tracks_Col_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_Col_Texture[indTrack] = NULL;
		pg_tracks_RadBrushRendmode_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_RadBrushRendmode_Texture[indTrack] = NULL;

		pg_tracks_Pos_target_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_Pos_target_Texture[indTrack] = NULL;
		pg_tracks_Col_target_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_Col_target_Texture[indTrack] = NULL;
		pg_tracks_RadBrushRendmode_target_Texture_texID[indTrack] = NULL_ID;
		pg_tracks_RadBrushRendmode_target_Texture[indTrack] = NULL;
	}

	for (int indTrack = 0; indTrack < PG_NB_TRACKS; indTrack++) {
		pg_tracks_Pos_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_Pos_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_Pos_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_Pos_Texture not allocated!"); ReportError(ErrorStr); throw 336;
		}
		pg_tracks_Col_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_Col_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_Col_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_Col_Texture not allocated!"); ReportError(ErrorStr); throw 336;
		}
		pg_tracks_RadBrushRendmode_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_RadBrushRendmode_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_RadBrushRendmode_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_RadBrushRendmode_Texture not allocated!"); ReportError(ErrorStr); throw 336;
		}

		pg_tracks_Pos_target_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_Pos_target_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_Pos_target_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_Pos_Texture not allocated!"); ReportError(ErrorStr); throw 336;
		}
		pg_tracks_Col_target_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_Col_target_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_Col_target_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_Col_Texture not allocated!"); ReportError(ErrorStr);
		}
		pg_tracks_RadBrushRendmode_target_Texture[indTrack] =
			(GLfloat *)pg_generateTexture(&(pg_tracks_RadBrushRendmode_target_Texture_texID[indTrack]), pg_float_tex_format,
				max_mouse_recording_frames, 1);
		if (!pg_tracks_RadBrushRendmode_target_Texture[indTrack]) {
			sprintf(ErrorStr, "Error: pg_tracks_RadBrushRendmode_Texture not allocated!"); ReportError(ErrorStr);
		}
	}


	// unthreaded loading of font texture because of the following
	// loads that are not synchronized so that there is no conflict for glTexImage2D
	ImageBufferDataStruct *pDataFontMess = new ImageBufferDataStruct;
	strcpy(pDataFontMess->fname, screen_font_file_name.c_str());
	pDataFontMess->pg_loadImage(false, 128, 70, false, false);
	glGenTextures(1, &Screen_Font_texture_texID);
	pDataFontMess->pg_toGPUImage(Screen_Font_texture_texID, true,
		GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR);
	printOglError(602);

	// unthreaded loading of font texture because of the following
	// loads that are not synchronized so that there is no conflict for glTexImage2D
	ImageBufferDataStruct *pDataFontDisplay = new ImageBufferDataStruct;
	strcpy(pDataFontDisplay->fname, display_font_file_name.c_str());
	pDataFontDisplay->pg_loadImage(false, 256, 86, false, false);
	glGenTextures(1, &Display_Font_texture_texID);
	pDataFontDisplay->pg_toGPUImage(Display_Font_texture_texID, true,
		GL_RGB8, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, GL_LINEAR);
	printOglError(603);

	// unthreaded loading of font texture because of the following
	// loads that are not synchronized so that there is no conflict for glTexImage2D
	ImageBufferDataStruct *pDataFontHashtag = new ImageBufferDataStruct;
	strcpy(pDataFontHashtag->fname, hashtag_font_file_name.c_str());
	pDataFontHashtag->pg_loadImage(false, 600, 200, false, false);
	glGenTextures(1, &Hashtag_Font_texture_texID);
	pDataFontHashtag->pg_toGPUImage(Hashtag_Font_texture_texID, true,
		GL_RGBA8, GL_RGBA,
		GL_UNSIGNED_BYTE, GL_LINEAR);
	printOglError(603);

	// 3D texture upload (pens) - not threaded
	pg_loadTexture3D((char *)("./ProjectsBefore18/" + project_name + "/textures/Pen_" + project_name + "_3D").c_str(),
		".png", 1, 4, true,
		&Pen_texture_3D_texID, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 2048);
	printOglError(700);

#ifdef PG_TRACK_DISPLAY
	// unthreaded loading of track texture because of the following
	// loads that are not synchronized so that there is no conflict for glTexImage2D
	ImageBufferDataStruct *pDataTrack = new ImageBufferDataStruct;
	strcpy(pDataTrack->fname, (char *)("ProjectsBefore18/" + "./ProjectsBefore18/" + project_name + "/textures/trackBrushes.png").c_str());
	pDataTrack->pg_loadImage(true, 128, 512, false, false);
	glGenTextures(1, &trackBrush_texture_2D_texID);
	GLenum format = (img.channels() == 4) ? GL_RGBA :
		(img.channels() == 3) ? GL_RGB : GL_LUMINANCE;
	pData->pg_toGPUImage(trackBrush_texture_2D_texID, false,
		GL_RGBA8, format,
		GL_UNSIGNED_BYTE, GL_LINEAR);
	printOglError(800);
#endif

	// 3D texture upload (acceleration/force vector field) - not threaded
	pg_loadTexture3D((char *)("./ProjectsBefore18/" + project_name + "/textures/ParticleAcceleration_" + project_name + "_3D").c_str(),
		".png", 7, 4, true,
		&Particle_acceleration_texture_3D, GL_RGBA8, GL_RGBA,
		GL_LINEAR,
		2048, 1024);
	printOglError(900);

	// texture buffer for moire
	// texture_buffer_texID;

	return true;
}

/////////////////////////////////////////////////////////////////
// TEXTURE GENERATION
/////////////////////////////////////////////////////////////////

// general texture allocation
void *pg_generateTexture( GLuint *textureID , pg_TextureFormat texture_format ,
		       int sizeX , int sizeY ) {
  glGenTextures( 1, textureID );

  // rgb POT raw image allocation (without alpha channel)
  // printf( "Texture %dx%d PG_NB_ATTACHMENTS %d\n" ,
  // 	  sizeX , sizeY , PG_NB_ATTACHMENTS );
  void *returnvalue = NULL;

  // Return from the function if no file name was passed in
  // Load the image and store the data
  if( texture_format == pg_byte_tex_format ) {
    GLubyte *ptr = new GLubyte[ sizeX * sizeY * 4 ];
    // If we can't load the file, quit!
    if(ptr == NULL) {
      strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    int indTex = 0;
    for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
      ptr[ indTex ] = 0;
      ptr[ indTex + 1 ] = 0;
      ptr[ indTex + 2 ] = 0;
      ptr[ indTex + 3 ] = 0;
      indTex += 4;
    }
    returnvalue = (void *)ptr;
  }
  else if( texture_format == pg_float_tex_format ) {
    float *ptr = new float[ sizeX * sizeY * 4 ];
    // If we can't load the file, quit!
    if(ptr == NULL) {
      strcpy( ErrorStr , "Texture allocation error!" ); ReportError( ErrorStr ); throw 425;
    }
    int indTex = 0;
    for( int i = 0 ; i < sizeX * sizeY ; i++ ) {
      ptr[ indTex ] = 0.0f;
      ptr[ indTex + 1 ] = 0.0f;
      ptr[ indTex + 2 ] = 0.0f;
      ptr[ indTex + 3 ] = 0.0f;
      indTex += 4;
    }
    returnvalue = (void *)ptr;
  }

  return returnvalue;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// IMAGE FILE PROCESSING
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// IMAGE FILE WRITING (JPG OR PNG) (THREADED)
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// Save PNG to file
#ifdef WIN32
DWORD WINAPI threaded_write_png( LPVOID lpParam ) {
#else
void* threaded_write_png(void * lpParam) {
#endif
  threadWriteImageData * pDataArray = (threadWriteImageData *)lpParam;

  cv::Mat result;
  cv::flip(*(pDataArray->img), result , 0); // vertical flip
#ifndef OPENCV_3
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_PNG_COMPRESSION);
#else
  cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

  std::vector<int> params;
  params.push_back(cv::IMWRITE_PNG_COMPRESSION);
#endif
  params.push_back(9);
  params.push_back(0);
  cv::imwrite( pDataArray->fname , result );

  delete pDataArray->img;
  delete pDataArray->fname;
  delete pDataArray;

  #ifndef WIN32
      pthread_exit(NULL);
  #endif
  return 0;
}

/////////////////////////////////////////////
// Save JPG to file
#ifdef WIN32
DWORD WINAPI threaded_write_jpg( LPVOID lpParam ) {
#else
void* threaded_write_jpg(void * lpParam) {
#endif
  threadWriteImageData * pDataArray = (threadWriteImageData *)lpParam;

  cv::Mat result;
  cv::flip(*(pDataArray->img), result , 0); // vertical flip
#ifndef OPENCV_3
  cv::cvtColor(result, result, CV_RGB2BGR);

  std::vector<int> params;
  params.push_back(CV_IMWRITE_JPEG_QUALITY);
#else
  cv::cvtColor(result, result, cv::COLOR_RGB2BGR);

  std::vector<int> params;
  params.push_back(cv::IMWRITE_JPEG_QUALITY);
#endif
  params.push_back(70);   // that's percent, so 100 == no compression, 1 == full
  // printf("write jpg (%s)\n", pDataArray->fname);
  cv::imwrite( pDataArray->fname , result );

  delete pDataArray->img;
  delete pDataArray->fname;
  delete pDataArray;

  #ifndef WIN32
      pthread_exit(NULL);
  #endif
  return 0;
}

/////////////////////////////////////////////////////////////////
// TEXTURE LOADING
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////
// 3D texture loading + transfer to GPU
bool pg_loadTexture3D( string filePrefixName , string fileSuffixName ,
		    int nbTextures , int bytesperpixel ,
		    bool invert ,
		    GLuint *textureID ,
		    GLint components, GLenum format ,
		    GLenum texturefilter ,
		    int width , int height ) {
  // data type is assumed to be GL_UNSIGNED_BYTE

  char filename[500];

  long size = width * height * bytesperpixel;
  GLubyte * bitmap = new unsigned char[size * nbTextures];
  long ind = 0;

  glEnable(GL_TEXTURE_3D);
  glGenTextures( 1, textureID );

  for( int indTex = 0 ; indTex < nbTextures ; indTex++ ) {
    sprintf( filename , "%s_%03d%s" ,  filePrefixName.c_str() ,
	     indTex + 1 ,  fileSuffixName.c_str() );
    char * ptr = strrchr( filename , '/' );
    printf("Loading [%s]\n", (ptr? ptr + 1: filename));

	// texture load through OpenCV
#ifndef OPENCV_3
    cv::Mat img = cv::imread( filename, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
    int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
      (img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
    // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
    cv::cvtColor(img, img, colorTransform);
    cv::Mat result;
    if( invert )
      cv::flip(img, result , 0); // vertical flip
    else
      result = img;
#else
    cv::Mat img = cv::imread( filename, cv::IMREAD_UNCHANGED );   // Read the file
    int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
      (img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
    // int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
    cv::cvtColor(img, img, colorTransform);
    cv::Mat result;
    if( invert )
      cv::flip(img, result , 0); // vertical flip
    else
      result = img;
#endif

    if(! result.data ) {                              // Check for invalid input
      sprintf( ErrorStr , "Could not open or find the 3D image %s!" , filename ); ReportError( ErrorStr ); throw 425;
      return false;
    }
    if( result.cols != width
	|| result.rows != height ) {   // Check for invalid input
      sprintf( ErrorStr , "Unexpected image size %s %d/%d %d/%d!" , filename , result.cols , width , result.rows , height); ReportError( ErrorStr ); throw 425;
      return false;
    }

    GLubyte * bytes = result.data;
    for(long int i = 0; i < size ; i++)
      bitmap[ind++] = bytes[i];
  }
  // printf("Final index %ld / %ld\n" , ind , size * nbTextures );
  // glActiveTexture (GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_3D, *textureID );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S , GL_REPEAT );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT );
  glTexImage3D(GL_TEXTURE_3D, // Type of texture
	       0,                 // Pyramid level (for mip-mapping) - 0 is the top level
	       components,        // Components: Internal colour format to convert to
	       width,             // Image width
	       height,            // Image heigh
	       nbTextures,        // Image depth
	       0,                 // Border width in pixels (can either be 1 or 0)
	       format,            // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
	       GL_UNSIGNED_BYTE,  // Image data type
	       bitmap );          // The actual image data itself

   printOglError( 0 );

  // memory release
  delete [] bitmap;

  // glGenerateMipmap(GL_TEXTURE_2D);
  return true;
}

//////////////////////////////////////////////////////////////////////
// THREAD LOAD IMAGE FILE (JPG OR PNG) THRU OPENCV
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
DWORD WINAPI pg_readImageData(LPVOID lpParam) {
#else
void* pg_readImageData(void * lpParam) {
#endif
	ImageBufferDataStruct * pDataArray = (ImageBufferDataStruct *)lpParam;

	// texture load through OpenCV
#ifndef OPENCV_3
	cv::Mat img = cv::imread(pDataArray->fname, CV_LOAD_IMAGE_UNCHANGED);   // Read the file
	// Check for invalid input
	if (!img.data) {
		sprintf(ErrorStr, "Could not open or find the 2D image %s!", pDataArray->fname); ReportError(ErrorStr);
		pDataArray->loadedInRAM = false;
		img.release();
		*(pDataArray->fname) = 0;
		return 0;
	}
	// Check for image size
	if (img.cols != pDataArray->w || img.rows != pDataArray->h) {
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
			pDataArray->fname, img.cols, pDataArray->w, img.rows,
			pDataArray->h); ReportError(ErrorStr);
		pDataArray->loadedInRAM = false;
		img.release();
		*(pDataArray->fname) = 0;
		return 0;
	}

	int colorTransform = (img.channels() == 4) ? CV_BGRA2RGBA :
		(img.channels() == 3) ? CV_BGR2RGB : CV_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if (img.channels() >= 3) {
		cv::cvtColor(img, img, colorTransform);
	}
	if (pDataArray->invert)
		cv::flip(img, *(pDataArray->img), 0); // vertical flip
	else
		*(pDataArray->img) = img;
#else
	cv::Mat img = cv::imread(pDataArray->fname, cv::IMREAD_UNCHANGED);   // Read the file
	// Check for invalid input
	if (!img.data) {
		sprintf(ErrorStr, "Could not open or find the 2D image %s!", pDataArray->fname); ReportError(ErrorStr);
		pDataArray->loadedInRAM = false;
		img.release();
		*(pDataArray->fname) = 0;
		return 0;
	}
	// Check for image size
	if (img.cols != pDataArray->w || img.rows != pDataArray->h) {
		sprintf(ErrorStr, "Unexpected image size %s %d/%d %d/%d!",
			pDataArray->fname, img.cols, pDataArray->w, img.rows,
			pDataArray->h); ReportError(ErrorStr);
		pDataArray->loadedInRAM = false;
		img.release();
		*(pDataArray->fname) = 0;
		return 0;
	}

	int colorTransform = (img.channels() == 4) ? cv::COLOR_BGRA2RGBA :
		(img.channels() == 3) ? cv::COLOR_BGR2RGB : cv::COLOR_GRAY2RGB;
	// int glColorType = (img.channels() == 4) ? GL_RGBA : GL_RGB;
	if( img.channels() >= 3 ) {
		cv::cvtColor(img, img, colorTransform);
	}
	if( pDataArray->invert )
		cv::flip(img, *(pDataArray->img) , 0); // vertical flip
	else
		*(pDataArray->img) = img;
#endif
	img.release();

	// when this boolean is set to true, the content of the image can be transfered to GPU
	pDataArray->transfering = false;
	pDataArray->transferred = false;
	pDataArray->loadedInRAM = true;
//	char * ptr = strrchr( pDataArray->fname , '/' );
//	printf( "-> RAM image [%s] size %dx%d ch %d\n" , (ptr? ptr + 1: pDataArray->fname) ,
//		img.cols  , img.rows , img.channels() );
//      printf("Loaded in RAM %d\n",pDataArray->loadedInRAM);

#ifndef WIN32
	if(pDataArray->threaded) {
	  pthread_exit(NULL);
	}
#endif

	return 0;
}

/////////////////////////////////////////////
// 2D image loading
bool ImageBufferDataStruct::pg_loadImage( bool toBeInverted , int width ,
					  int height , bool is_threaded , bool verbose ) {
	bool valret = true;


	h = height;
	w = width;
	invert = toBeInverted;
	threaded = is_threaded;
	// threaded = false;
	img = new cv::Mat(height, width, CV_8UC3);

	const char * ptr = strrchr(fname, '/');

	if (verbose) {
		printf("Loading [%s]\n", (ptr ? ptr + 1 : fname));
	}

	if( threaded ) {
	  #ifdef WIN32
	    DWORD rc;
	    HANDLE  hThread = CreateThread(
		    NULL,                   // default security attributes
		    0,                      // use default stack size
		    pg_readImageData,		    // thread function name
		    (void *)this,		    // argument to thread function
		    0,                      // use default creation flags
		    &rc);   // returns the thread identifier
	    if (hThread == NULL){
		    std::cout << "Error:unable to create thread pg_readImageData" << std::endl;
		    *fname = 0;
		    return NULL;
	    }
	    CloseHandle(hThread);
		  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	      // should only be closed when the thread is finished + assigned NULL value
		  // make code so that only one reading (or writing thread) is called at a time
		  // because of concurrent call to glTexImage2D that might be a source of problem
		  // a texture has been loaded in RAM
	      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  #else
	    pthread_t reading_thread;
	    int rc;
	    rc = pthread_create(&reading_thread, NULL,
				pg_readImageData, (void *)this);
	    if (rc){
		    std::cout << "Error:unable to create thread pg_readImageData" << rc << std::endl;
		    pthread_exit(NULL);
	    }
	  #endif
	}
	else {
//      printf("Loading [%s]\n", (ptr ? ptr + 1 : fname));
	  pg_readImageData( (void *)this);
//      printf("Loaded in RAM %d\n",this->loadedInRAM);
	}
	return valret;
}

/////////////////////////////////////////////
// 2D texture tranfer to GPU
#ifdef WIN32
DWORD WINAPI pg_toGPUImageThreaded( LPVOID lpParam ) {
#else
void* pg_toGPUImageThreaded(void * lpParam) {
#endif
    struct ThreadTextureData *texData = (struct ThreadTextureData *)lpParam;

	///////////////////////////////////////////
	// sets the current OpenGL context
#ifdef _WIN32
	wglMakeCurrent( MainOpenGLDeviceContext,
	                SharedOpenGLRenderingContext[texData->indOpenGLContext]);
#else
	glXMakeCurrent( glXGetCurrentDisplay(),
			MainOpenGLDeviceContext,
			SharedOpenGLRenderingContext[texData->indOpenGLContext]);
#endif

	//char * ptr = strrchr(fname, '/');
	// printf("Transfer to GPU (%s)\n", texData->imageStr->fname);
	texData->imageStr->pg_toGPUImage(texData->textureID, texData->is_rectangle,
		texData->components, texData->format,
		texData->datatype,texData->texturefilter);
	texData->is_free = true;
	texData->imageStr->transferred = true;
	texData->imageStr->transfering = false;

	// releases OpenGL context
#ifdef _WIN32
	wglMakeCurrent( NULL, NULL );
#else
	glXMakeCurrent( glXGetCurrentDisplay(),
			None, NULL );
#endif

  // delete pDataArray;
  #ifndef WIN32
      pthread_exit(NULL);
  #endif
  return 0;
}

bool ImageBufferDataStruct::pg_toGPUImage(GLuint textureID, bool is_rectangle,
		GLint components, GLenum format,
		GLenum datatype, GLenum texturefilter) {
	bool valret = true;
    glEnable(GL_TEXTURE_2D);

	// glActiveTexture (GL_TEXTURE0 + index);
	if (is_rectangle) {
		glEnable(GL_TEXTURE_RECTANGLE);
		glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, (GLfloat)texturefilter);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_RECTANGLE,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			img->cols,          // Image width
			img->rows,          // Image heigh
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			img->ptr());        // The actual image data itself
		printOglError( 4 );
		// printf("filename %s\n", fname);
		valret = false;
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (float)texturefilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,     // Type of texture
			0,                 // Pyramid level (for mip-mapping) - 0 is the top level
			components,            // Components: Internal colour format to convert to
			img->cols,          // Image width
			img->rows,          // Image height
			0,                 // Border width in pixels (can either be 1 or 0)
			format, // Format: Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			datatype,  // Image data type
			img->ptr());        // The actual image data itself
		printOglError( 5 );
		valret = false;
	}

	// glGenerateMipmap(GL_TEXTURE_2D);
	return valret;
}

/////////////////////////////////////////////////////////////////
// TEXTURE BUFFER MANAGEMENT
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// PROCESSES IMAGE SWAPPING

// updates the swapping weights from 0 to 1 until the
// swapped image is visible
// if it reaches 1, the image contents are swapped and
// the swap image is freed
bool update_image_buffer_swapping (void) {
	bool valRet = false;

	for (int indImage = 0; indImage < PG_IMAGE_NB_TEXTURES;
		indImage++) {
//		    					printf("Image %d - loaded %d %d tranferred %d %d  swapping %d\n" ,
//					  indImage , image_swap_buffer_data[indImage].loadedInRAM ,
//			image_buffer_data[indImage].loadedInRAM ,
//			image_swap_buffer_data[indImage].transferred ,
//			image_buffer_data[indImage].transferred ,
//			image_swap_buffer_data[indImage].swapping );

		bool swapImages = false;
		// ongoing swapping
		if (image_swap_buffer_data[indImage].loadedInRAM == true
			&& image_buffer_data[indImage].loadedInRAM == true
			&& image_swap_buffer_data[indImage].transferred == true
			&& image_buffer_data[indImage].transferred == true
			&& image_swap_buffer_data[indImage].swapping == true) {
			if (indImage < 3) {
				if (image_buffer_alphaSwap02[indImage] < 1.0) {
					image_buffer_alphaSwap02[indImage]
					= std::min(1.0f, (float)((CurrentClockTime
					  - image_swap_buffer_data[indImage].swapStart)/
						  image_swap_buffer_data[indImage].swapDuration));
//					printf("Image %d time %.2f alpha %.2f (start %.2f dur %.2f)\n" ,
//					  indImage , CurrentClockTime , image_buffer_alphaSwap02[indImage] ,
//					  image_swap_buffer_data[indImage].swapStart,
//					  image_swap_buffer_data[indImage].swapDuration );
					valRet = true;
				}
				else {
					swapImages = true;
				}
			}
			else {
				if (image_buffer_alphaSwap35[indImage - 3] < 1.0) {
					image_buffer_alphaSwap35[indImage - 3]
					= std::min(1.0f, (float)((CurrentClockTime
					  - image_swap_buffer_data[indImage].swapStart)/
						  image_swap_buffer_data[indImage].swapDuration));
//					printf("Image %d time %.2f alpha %.2f (start %.2f dur %.2f)\n" ,
//					  indImage , CurrentClockTime , image_buffer_alphaSwap35[indImage - 3] ,
//					  image_swap_buffer_data[indImage].swapStart,
//					  image_swap_buffer_data[indImage].swapDuration  );
					valRet = true;
				}
				else {
					swapImages = true;
				}
			}
		}
		// swapping is finished: the swapping alpha coeficient has reached 1.O
		// the swap image is exchanged with buffer image and the swp image (ex buffer image)
		// is released
		if (swapImages == true) {
			valRet = true;
			image_swap_buffer_data[indImage].swapping = false; // makes it so that there is one frame
			// that displays the full swapped image

//			char * ptr1 = strrchr( image_swap_buffer_data[indImage].fname , '/' );
//			char * ptr2 = strrchr( image_buffer_data[indImage].fname , '/' );
//			printf("Image swapped %s -> %s #%d (dur %.2f)\n" ,
//			  (ptr1? ptr1 + 1: image_swap_buffer_data[indImage].fname) ,
//			  (ptr2? ptr2 + 1: image_buffer_data[indImage].fname), indImage ,
//			  image_swap_buffer_data[indImage].swapDuration );

			// exchanges image pointers between buffer image and swap image
            			cv::Mat *imgTmp = image_swap_buffer_data[indImage].img;
            			image_swap_buffer_data[indImage].img = image_buffer_data[indImage].img;
            			image_buffer_data[indImage].img = imgTmp;
            // stores swapped image in main image and releases the previous one
            //delete [] image_buffer_data[indImage].img->data;
			// image_buffer_data[indImage].img = image_swap_buffer_data[indImage].img;

			// exchanges texture IDs between buffer image and swap image
			GLuint IDTmp = image_swap_buffer_data[indImage].texBuffID;
			image_swap_buffer_data[indImage].texBuffID = image_buffer_data[indImage].texBuffID;
			image_buffer_data[indImage].texBuffID = IDTmp;

			//GLenum format = (image_swap_buffer_data[indImage].img->channels() == 4) ? GL_RGBA :
			//	(image_swap_buffer_data[indImage].img->channels() == 3) ? GL_RGB : GL_LUMINANCE;
			//image_swap_buffer_data[indImage].pg_toGPUImage(image_swap_buffer_data[indImage].texBuffID, true,
			//			GL_RGB8, format,
			//			GL_UNSIGNED_BYTE, GL_LINEAR);

			// data cooy
			// buffer image takes the name of swap image
			strcpy(image_buffer_data[indImage].fname, image_swap_buffer_data[indImage].fname);
			*(image_swap_buffer_data[indImage].fname) = 0;

			// resests swap weight
			if (indImage < 3) {
				image_buffer_alphaSwap02[indImage] = 0.0f;
			}
			else {
				image_buffer_alphaSwap35[indImage - 3] = 0.0f;
			}
			// releases the swap image
			image_swap_buffer_data[indImage].release();
		}
	}
	return valRet;
}

//////////////////////////////////////////////////////////////////
// LOOKS FOR AVAILABLE SWAP IMAGE

// tries to find a buffer that has been invalidated
// by expanding the search around the image that is
// opposite to random_image_buffer_index
// returns available index if not returns negative
int available_random_swap_image_buffer(int indOpposedCenter) {
	for (int indWindow = 0;
		indWindow < PG_IMAGE_NB_TEXTURES / 2 + 1;
		indWindow++) {
		int curInd = (indOpposedCenter + indWindow) % PG_IMAGE_NB_TEXTURES;
		if (*(image_swap_buffer_data[curInd].fname) == 0) {
			return curInd;
		}
		curInd = (indOpposedCenter - indWindow) % PG_IMAGE_NB_TEXTURES;
		if (*(image_swap_buffer_data[curInd].fname) == 0) {
			return curInd;
		}
	}
	return -1;
}
int available_swap_image_buffer(int indInitialImage) {
	for (int indWindow = indInitialImage;
		indWindow < indInitialImage + PG_IMAGE_NB_TEXTURES ;
		indWindow++) {
		int curInd = indWindow % PG_IMAGE_NB_TEXTURES;
		if (*(image_swap_buffer_data[curInd].fname) == 0) {
			return curInd;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////
// LOADS BUFFER IMAGE FROM FILE

// threaded upload of the free swap image from a randomly chosen available layer
bool pg_load_and_swap_image(std::string * fileName, double duration) {
	// printf("Image dir %d # %d dur %.2f\n" , dir_index, file_index, duration);
	// chooses a random layer from which to start the search for an available swapping layer
	// RANDOM CHOICE OF FIRST IMAGE
	// int random_image_buffer_index = (int)floor(((double)rand() / (RAND_MAX)) * PG_IMAGE_NB_TEXTURES);

	// **** COULD HAVE SOMETHING FOR MASKS AS WELL ****

	// RANDOM CHOICE OF FIRST OPPOSED IMAGE
	// int ind_opposed_center = (random_image_buffer_index + PG_IMAGE_NB_TEXTURES / 2)
	//	% PG_IMAGE_NB_TEXTURES;
	// we take in turn a new start index for available swap images so that it changes regularly
	int indAvailableSwapImage;
	if ((indAvailableSwapImage = available_swap_image_buffer(IndInitialSwapImage)) >= 0) {
		// sets the start index for available image layer to the next layer
		IndInitialSwapImage = (IndInitialSwapImage + 1) % PG_IMAGE_NB_TEXTURES;

		const char * ptr1 = strrchr( fileName->c_str() , '/' );
	    char * ptr2 = strrchr( image_buffer_data[indAvailableSwapImage].fname , '/' );
	    printf("Swap image (%s replaces %s layer %d (dur %.2f))\n" ,
	      (ptr1? ptr1 + 1: fileName->c_str()) ,
	      (ptr2? ptr2 + 1: image_buffer_data[indAvailableSwapImage].fname),
	      indAvailableSwapImage , duration );

	    strcpy(image_swap_buffer_data[indAvailableSwapImage].fname, fileName->c_str());
	    image_swap_buffer_data[indAvailableSwapImage].swapDuration = duration;
	    // NEW has been made as non threaded for OPTIMUS
	    // 4th parameter could be "true"
		if (!image_swap_buffer_data[indAvailableSwapImage].pg_loadImage(
			true, leftWindowWidth/2, window_height, false, false)) {
			return false;
		}
		// printf("Loaded in RAM %d\n",image_swap_buffer_data[indAvailableSwapImage].loadedInRAM);
	    // this is made after GPU transfer
	    //image_swap_buffer_data[indAvailableSwapImage].swapStart = CurrentClockTime;
	    //image_swap_buffer_data[indAvailableSwapImage].swapping = true;
	    //if (indAvailableSwapImage < 3) {
		   // image_buffer_alphaSwap02[indAvailableSwapImage] = 0.0f;
	    //}
	    //else {
		   // image_buffer_alphaSwap35[indAvailableSwapImage - 3] = 0.0f;
	    //}
		return true;
	}
	else {
	   printf("Image swapping not possible (all layers currently swap)\n");
	   return false;
	}
	return false;
}

/////////////////////////////////////////////////////////
// INITIAL IMAGE UPLOADING
/////////////////////////////////////////////////////////
bool  pg_ReadInitalImageTextures( int ind_dir ) {
	printf("Read Initial Images %d %s\n" , ind_dir, ImageDir.c_str());
	std::string * fileName;

	// assigns standard weights to each image buffer
	for(int ind = 0 ; ind < PG_IMAGE_NB_TEXTURES; ind++ ) {
	  image_buffer_weights[ind] = 1.0f / PG_IMAGE_NB_TEXTURES;
	}

	// fileName = dir + basefilename + string_number_s + ".jpg";

	bool valret = true;
	for (int indImage = 0; indImage < PG_IMAGE_NB_TEXTURES; indImage++) {
		if (*(image_buffer_data[indImage].fname) == 0) {
			if (!image_buffer_data[indImage].IDallocated) {
				glGenTextures(1, &(image_buffer_data[indImage].texBuffID));
				image_buffer_data[indImage].IDallocated = true;

				if ((fileName = nextFile(&ImageDir, &IndPlayedImageFileDir, &IndPlayedImageFile))) {
                    //printf("fileName %s ind_dir %d ind_file %d filename %s\n" , ImageDir.c_str() ,
                    //       IndPlayedImageFileDir , IndPlayedImageFile, fileName->c_str());
					strcpy(image_buffer_data[indImage].fname, fileName->c_str());
					valret &= image_buffer_data[indImage].pg_loadImage(
						true, leftWindowWidth/2, window_height, false, false);
					delete fileName;
					fileName = NULL;
				}
			}
		}
	}

	// loads all the associated masks
	for (int indBuffer = 0; indBuffer < PG_IMAGE_NB_TEXTURES/3; indBuffer++) {
		if (*(image_mask_buffer_data[indBuffer].fname) == 0) {
			if (!image_mask_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(image_mask_buffer_data[indBuffer].texBuffID));
				image_mask_buffer_data[indBuffer].IDallocated = true;

				int indDir = 0;
				int indFile = indBuffer;
				if ((fileName = nextFile(&MaskDir, &indDir, &indFile))) {
					strcpy(image_mask_buffer_data[indBuffer].fname, fileName->c_str());
					valret &= image_mask_buffer_data[indBuffer].pg_loadImage(
						true, leftWindowWidth/2, window_height, false, true);
					delete fileName;
					fileName = NULL;
				}
			}
		}
	}
	//for (int indBuffer = 0; indBuffer < PG_IMAGE_NB_TEXTURES;
	//	indBuffer++) {
	//	if (*(image_swap_buffer_data[indBuffer].fname) == 0) {
	//		if (!image_swap_buffer_data[indBuffer].IDallocated) {
	//			glGenTextures(1, &(image_swap_buffer_data[indBuffer].texBuffID));
	//			image_swap_buffer_data[indBuffer].IDallocated = true;
	//			sprintf(string_number, "_%03d", (int)(indBuffer + PG_IMAGE_NB_TEXTURES));
	//			std::string string_number_s(string_number);
	//			std::string fileName = dir + basefilename + string_number_s + ".jpg";
	//			strcpy(image_swap_buffer_data[indBuffer].fname, fileName.c_str());
	//			valret &= image_swap_buffer_data[indBuffer].pg_loadImage(
	//				true, leftWindowWidth, window_height, false, false);
	//			delete fileName;
	//			fileName = NULL;
	//		}
	//	}
	//}
	return valret;
}

//////////////////////////////////////////////////////////////////
// TRANSFERS BUFFER TO GPU
// should be called repeatedly and will transfer to the GPU
// any buffer image whose uploading thread has finished

bool GPUtransfer_image_buffer_data(void) {
	bool valret = false;
	for (int indBuffer = 0; indBuffer < PG_IMAGE_NB_TEXTURES; indBuffer++) {
		if (image_buffer_data[indBuffer].loadedInRAM
			&& !image_buffer_data[indBuffer].transfering
			&& !image_buffer_data[indBuffer].transferred){
			if (!image_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(image_buffer_data[indBuffer].texBuffID));
				image_buffer_data[indBuffer].IDallocated = true;
			}
			GLenum format = (image_buffer_data[indBuffer].img->channels() == 4) ? GL_RGBA :
				(image_buffer_data[indBuffer].img->channels() == 3) ? GL_RGB : GL_LUMINANCE;

#ifdef WIN32
			// thread for the transfer to GPU
			int indFreeOpenGLContext = GetFreeOpenGLContext();
			if (indFreeOpenGLContext >= 0) {
				//char * ptr = strrchr(image_buffer_data[indBuffer].fname, '/');
				//printf("-> threaded GPU base image %d [%s] Frame %d\n", indBuffer, (ptr ? ptr + 1 : image_buffer_data[indBuffer].fname), FrameNo);
				image_buffer_data[indBuffer].transfering = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_free = false;
				ThreadOpenGLTextureData[indFreeOpenGLContext].imageStr = &image_buffer_data[indBuffer];
				ThreadOpenGLTextureData[indFreeOpenGLContext].textureID
					= image_buffer_data[indBuffer].texBuffID;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB8;
				ThreadOpenGLTextureData[indFreeOpenGLContext].format = format;
				ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
				ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;

				DWORD rc;
				HANDLE  hThread = CreateThread(
					NULL,         // default security attributes
					0,            // use default stack size
					pg_toGPUImageThreaded,  // thread function name
					(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function
					0,            // use default creation flags
					&rc);   // returns the thread identifier
				if (hThread == NULL){
					std::cout << "Error:unable to create thread pg_readSampleData" << std::endl;
					// exit(-1);
				}
				CloseHandle(hThread);
			}
#else
            // NEW
            image_buffer_data[indBuffer].pg_toGPUImage(image_buffer_data[indBuffer].texBuffID,
                      true,
                      GL_RGB8,
                      format,
                      GL_UNSIGNED_BYTE,
                      GL_LINEAR);
            image_buffer_data[indBuffer].transferred = true;
            image_buffer_data[indBuffer].transfering = false;

            /*
            pthread_t readWavData_thread;
            int rc;
            rc = pthread_create(&readWavData_thread, NULL,
                pg_toGPUImageThreaded, (void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
            if (rc){
                std::cout << "Error:unable to create thread pg_readSampleData" << rc << std::endl;
                pthread_exit(NULL);
            }
            */
#endif

            printOglError(6);
            valret = true;
        }
	}

	for (int indBuffer = 0; indBuffer < PG_IMAGE_NB_TEXTURES / 3; indBuffer++) {
		if (image_mask_buffer_data[indBuffer].loadedInRAM
			&& !image_mask_buffer_data[indBuffer].transfering
			&& !image_mask_buffer_data[indBuffer].transferred){
			if (!image_mask_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(image_mask_buffer_data[indBuffer].texBuffID));
				image_mask_buffer_data[indBuffer].IDallocated = true;
			}
			GLenum format = (image_mask_buffer_data[indBuffer].img->channels() == 4) ? GL_RGBA :
				(image_mask_buffer_data[indBuffer].img->channels() == 3) ? GL_RGB : GL_LUMINANCE;


#ifdef WIN32
			// thread for the transfer to GPU
			int indFreeOpenGLContext = GetFreeOpenGLContext();
			if (indFreeOpenGLContext >= 0) {
				//char * ptr = strrchr( image_mask_buffer_data[indBuffer].fname , '/' );
				//printf("-> threaded GPU mask image %d [%s] Frame %d\n", indBuffer, (ptr ? ptr + 1 : image_mask_buffer_data[indBuffer].fname), FrameNo);
				image_mask_buffer_data[indBuffer].transfering = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_free = false;
				ThreadOpenGLTextureData[indFreeOpenGLContext].imageStr = &image_mask_buffer_data[indBuffer];
				ThreadOpenGLTextureData[indFreeOpenGLContext].textureID
					= image_mask_buffer_data[indBuffer].texBuffID;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB8;
				ThreadOpenGLTextureData[indFreeOpenGLContext].format = format;
				ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
				ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;
				DWORD rc;
				HANDLE  hThread = CreateThread(
					NULL,         // default security attributes
					0,            // use default stack size
					pg_toGPUImageThreaded,  // thread function name
					(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function
					0,            // use default creation flags
					&rc);   // returns the thread identifier
				if (hThread == NULL){
					std::cout << "Error:unable to create thread pg_readSampleData" << std::endl;
					// exit(-1);
				}
				CloseHandle(hThread);
			}
#else
            // NEW
            image_mask_buffer_data[indBuffer].pg_toGPUImage(image_mask_buffer_data[indBuffer].texBuffID,
                      true,
                      GL_RGB8,
                      format,
                      GL_UNSIGNED_BYTE,
                      GL_LINEAR);
            image_mask_buffer_data[indBuffer].transferred = true;
            image_mask_buffer_data[indBuffer].transfering = false;

            /*
            pthread_t readWavData_thread;
            int rc;
            rc = pthread_create(&readWavData_thread, NULL,
                pg_toGPUImageThreaded, (void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
            if (rc){
                std::cout << "Error:unable to create thread pg_readSampleData" << rc << std::endl;
                pthread_exit(NULL);
            }
            */
#endif
            printOglError(6);
            valret = true;
        }
	}


	for (int indBuffer = 0; indBuffer < PG_IMAGE_NB_TEXTURES;
		indBuffer++) {
		if (image_swap_buffer_data[indBuffer].loadedInRAM
			&& !image_swap_buffer_data[indBuffer].transfering
			&& !image_swap_buffer_data[indBuffer].transferred) {
			if (!image_swap_buffer_data[indBuffer].IDallocated) {
				glGenTextures(1, &(image_swap_buffer_data[indBuffer].texBuffID));
				image_swap_buffer_data[indBuffer].IDallocated = true;
			}
			GLenum format = (image_swap_buffer_data[indBuffer].img->channels() == 4) ? GL_RGBA :
				(image_swap_buffer_data[indBuffer].img->channels() == 3) ? GL_RGB : GL_LUMINANCE;


#ifdef WIN32
			// thread for the transfer to GPU
			int indFreeOpenGLContext = GetFreeOpenGLContext();
			if (indFreeOpenGLContext >= 0) {
				//char * ptr = strrchr(image_swap_buffer_data[indBuffer].fname, '/');
				//printf("-> threaded GPU swap image %d [%s] Frame %d\n", indBuffer, (ptr ? ptr + 1 : image_swap_buffer_data[indBuffer].fname), FrameNo);
				image_swap_buffer_data[indBuffer].transfering = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_free = false;
				ThreadOpenGLTextureData[indFreeOpenGLContext].imageStr = &image_swap_buffer_data[indBuffer];
				ThreadOpenGLTextureData[indFreeOpenGLContext].textureID
					= image_swap_buffer_data[indBuffer].texBuffID;
				ThreadOpenGLTextureData[indFreeOpenGLContext].is_rectangle = true;
				ThreadOpenGLTextureData[indFreeOpenGLContext].components = GL_RGB8;
				ThreadOpenGLTextureData[indFreeOpenGLContext].format = format;
				ThreadOpenGLTextureData[indFreeOpenGLContext].datatype = GL_UNSIGNED_BYTE;
				ThreadOpenGLTextureData[indFreeOpenGLContext].texturefilter = GL_LINEAR;

				DWORD rc;
				HANDLE  hThread = CreateThread(
					NULL,         // default security attributes
					0,            // use default stack size
					pg_toGPUImageThreaded,  // thread function name
					(void *)& ThreadOpenGLTextureData[indFreeOpenGLContext],// argument to thread function
					0,            // use default creation flags
					&rc);   // returns the thread identifier
				if (hThread == NULL){
					std::cout << "Error:unable to create thread pg_readSampleData" << std::endl;
					// exit(-1);
				}
				CloseHandle(hThread);
			}
#else
            // NEW
            image_swap_buffer_data[indBuffer].pg_toGPUImage(image_swap_buffer_data[indBuffer].texBuffID,
                      true,
                      GL_RGB8,
                      format,
                      GL_UNSIGNED_BYTE,
                      GL_LINEAR);
            image_swap_buffer_data[indBuffer].transferred = true;
            image_swap_buffer_data[indBuffer].transfering = false;
            /*
            pthread_t readWavData_thread;
            int rc;
            rc = pthread_create(&readWavData_thread, NULL,
                pg_toGPUImageThreaded, (void *)& ThreadOpenGLTextureData[indFreeOpenGLContext]);
            if (rc){
                std::cout << "Error:unable to create thread pg_readSampleData" << rc << std::endl;
                pthread_exit(NULL);
            }
            */
#endif

            printOglError(600);
            valret = true;
        }

		// launches swapping
		if (image_swap_buffer_data[indBuffer].loadedInRAM
			&& image_swap_buffer_data[indBuffer].transferred
			&& !image_swap_buffer_data[indBuffer].swapping) {
			image_swap_buffer_data[indBuffer].swapStart = CurrentClockTime;
			image_swap_buffer_data[indBuffer].swapping = true;
			if (indBuffer < 3) {
				image_buffer_alphaSwap02[indBuffer] = 0.0f;
			}
			else {
				image_buffer_alphaSwap35[indBuffer - 3] = 0.0f;
			}
			printOglError(601);

			//          char * ptr = strrchr( image_swap_buffer_data[indBuffer].fname , '/' );
            //			printf("-> GPU swap image %d [%s]\n", indBuffer,
            //						 (ptr? ptr + 1: image_swap_buffer_data[indBuffer].fname));
            //              printf("swap1 %.1f %.1f %.1f\n", image_buffer_alphaSwap02[0], image_buffer_alphaSwap02[1],
            //                  image_buffer_alphaSwap02[2]);
            //              printf("swap2 %.1f %.1f %.1f\n", image_buffer_alphaSwap35[0], image_buffer_alphaSwap35[1],
            //                  image_buffer_alphaSwap35[2]);

			valret = true;
		}
	}
	return valret;
}

