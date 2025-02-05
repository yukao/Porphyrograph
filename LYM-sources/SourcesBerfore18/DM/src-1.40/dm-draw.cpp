////////////////////////////////////////////////////////////////////
// 
// 
//     Part of this source file is taken from Virtual Choreographer
//     http://virchor.sourceforge.net/
// 
//     File dm-draw.cpp
// 
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#include "dm-all_include.h"

int user_id;
      
extern char currentFilename[512];
extern char description[512];

/////////////////////////////////////////////////////////////////
// Projection and view matrices for the shader
/////////////////////////////////////////////////////////////////
GLfloat matProj[16];
GLfloat matView[16];

/////////////////////////////////////////////////////////////////
// REDRAWING THE SCENE
/////////////////////////////////////////////////////////////////
void projectionMatrix( void ) {
  // OpenGL interactive drawing initialization 
  // projection matrix definition
  glMatrixMode (GL_PROJECTION);
  // the projection matrix is taken in charge by the 
  // evi3d library
  glLoadIdentity();
  if( CurrentOrtho ) {
    // printf("ortho x %.4f %.4f y %.4f %.4f n/f %.4f %.4f\n",CurrentOrtho->xMin, 	      CurrentOrtho->xMax,    CurrentOrtho->yMin,  CurrentOrtho->yMax,  CurrentOrtho->nearOrtho,    CurrentOrtho->farOrtho);
    // glOrtho(CurrentOrtho->xMin,
    // 	    CurrentOrtho->xMax,
    // 	    CurrentOrtho->yMin,
    // 	    CurrentOrtho->yMax,
    // 	    Currentortho->nearOrtho,
    // 	    CurrentOrtho->farOrtho);
    float n = CurrentOrtho->nearOrtho;
    float f = CurrentOrtho->farOrtho;
    float t = CurrentOrtho->yMax;
    float b = CurrentOrtho->yMin;
    float r = CurrentOrtho->xMax;
    float l = CurrentOrtho->xMin;
    GLfloat mat[] = { 
      (GLfloat)(2.0/(r-l)), 0.0, 0.0, 0.0, 
      0.0, (GLfloat)(2.0/(t-b)), 0.0, 0.0, 
      0.0, 0.0, (GLfloat)(2.0/(f-n)), 0.0, 
      (GLfloat)(-(r+l)/(r-l)), (GLfloat)(-(t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), 1.0 };
    memcpy( (char *)matProj , mat , 16 * sizeof( float ) );
    glMultMatrixf(matProj); 
  }
  else if( CurrentFrustum ) {
    // printf("frustum x %.4f %.4f y %.4f %.4f n/f %.4f %.4f\n",CurrentFrustum->xMin, 	      CurrentFrustum->xMax,    CurrentFrustum->yMin,  CurrentFrustum->yMax,  CurrentFrustum->nearFrustum,    CurrentFrustum->farFrustum);
    // glFrustum(CurrentFrustum->xMin, 
    // 	      CurrentFrustum->xMax, 
    // 	      CurrentFrustum->yMin, 
    // 	      CurrentFrustum->yMax, 
    // 	      CurrentFrustum->nearFrustum, 
    // 	      CurrentFrustum->farFrustum);
    float n = CurrentFrustum->nearFrustum;
    float f = CurrentFrustum->farFrustum;
    float t = CurrentFrustum->yMax;
    float b = CurrentFrustum->yMin;
    float r = CurrentFrustum->xMax;
    float l = CurrentFrustum->xMin;
    GLfloat mat[] = { 
      (GLfloat)((2.0*n)/(r-l)), 0.0, 0.0, 0.0, 
      0.0, (GLfloat)((2.0*n)/(t-b)), 0.0, 0.0, 
      (GLfloat)((r+l)/(r-l)), (GLfloat)((t+b)/(t-b)), (GLfloat)(-(f+n)/(f-n)), -1.0, 
      0.0, 0.0, (GLfloat)((-2.0*f*n)/(f-n)), 0.0 };
    memcpy( (char *)matProj , mat , 16 * sizeof( float ) );
    glMultMatrixf(matProj); 
  }
}


// computing the way from the current user position to
// the root node by composing the user position in
// the viewpoint reference and by composing it with the inverse
// transformation from the viewpoint to the root node
// This part is separated from the rest of the rendering
// in order to allow for dynamic change of viewpoint 
// through recomputation of the inverse modelViewMatrix of the 
// viewpoint

void cameraToRootMVMatrix( bool withPushMatrix ) {
  // modelview matrix initialization
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  depthTransformation = 0;
  CurrentHeight = 0;

  // the values of viewpoints[indCurrentViewpoint]->pitch_head 
  // and viewpoints[indCurrentViewpoint]->yaw_head 
  // are modified through mouse motion


  //////////////////////////////////////////////////////
  // transformations resulting from the displacements
  // of the user wrt to the viewpoint (vehicle) 
  //////////////////////////////////////////////////////


    
  // saves the current matrix
  // only for the initial viewpoint position
  // inside frame viewpoint changes don't push matrix because
  // they can occur inside the rendering graph
  if( withPushMatrix ) {
    glPushMatrix();
  }
  {
    float angle = -CurrentUser->start_pitch * DegtoRad;
    if( angle ) {
      GLfloat mat_pitch[] = { (GLfloat)cos( angle ) , (GLfloat)sin( angle ) , 0.0 , 0.0 , 
			 (GLfloat)-sin( angle ) , (GLfloat)cos( angle ) , 0.0 , 0.0 , 
			 0.0 , 0.0 , 1.0 , 0.0 , 
			 0.0 , 0.0 , 0.0 , 1.0 };
      glMultMatrixf(mat_pitch);  
    }
    // glRotatef( -CurrentUser->start_pitch , 0.0, 0.0, 1.0);
    
    //////////////////////////////////////////////////////
    // transformations resulting from the displacement 
    // of the viewpoint (vehicle) wrt the root node
    //////////////////////////////////////////////////////
      
    // QUAD navigation (OpenGL or evi3d)
    //printf("User %s Viewpoint %s Translatef %.2f %.2f %.2f\n" , CurrentUser->GetId() , CurrentViewPointNode->id , -CurrentUser->start_x - orig->z , -CurrentUser->start_y - orig->y ,-CurrentUser->start_z + orig->x );
    angle = CurrentUser->start_yaw * DegtoRad;
    if( angle ) {
      GLfloat mat_yaw[] = { (GLfloat)cos( angle ) , 0.0 , (GLfloat)-sin( angle ) , 0.0 , 
			 0.0 , 1.0 , 0.0 , 0.0 , 
			 (GLfloat)sin( angle ) , 0.0 , (GLfloat)cos( angle ) , 0.0 , 
			 0.0 , 0.0 , 0.0 , 1.0 };
      glMultMatrixf(mat_yaw); 
    } 
    // glRotatef( CurrentUser->start_yaw , 0.0, 1.0, 0.0);
    
    if( CurrentUser->start_x || CurrentUser->start_y 
	|| CurrentUser->start_z ) {
      GLfloat mat_translation[] = { 1.0 , 0.0 , 0.0 , 0.0 , 
			 0.0 , 1.0 , 0.0 , 0.0 , 
			 0.0 , 0.0 , 1.0 , 0.0 , 
			 -CurrentUser->start_x , -CurrentUser->start_y , 
			 -CurrentUser->start_z , 1.0 };
      glMultMatrixf(mat_translation); 
    }
    // glTranslatef( -CurrentUser->start_x , -CurrentUser->start_y , -CurrentUser->start_z )

    if( CurrentIndViewPoint > 0 ) {
      // the transformation path from the viewpoint
      // to the root node is stored in the transformation
      // matrix
      // 		  if (CurrentViewPointMatrix == NULL)
      // 		  {
      // 		     int ijj = 1;
      // 		  }
      DM_CompositionNode->transformToPath( CurrentViewPoint , _Render );
    }
    else if( CurrentIndViewPoint == 0 ) {
      // no transformation since the
      // default viewpoint is located at the root node
    }

    glGetFloatv( GL_MODELVIEW_MATRIX , CurrentViewPointMatrix );
    // float *val = CurrentViewPointMatrix;
    // printf( "\ncameraToRootMVMatrix curr MVM %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n\n" , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  }
}

//////////////////////////////////////////////////////
// generic interactive draw function (from the root)

// scene redisplay (controlled by a drawing mode: file, interactive
// or edition)

void draw_scene( DrawingMode mode ) {

  // ******************** Svg output ********************
  if( mode == _Svg ) {
    sprintf( currentFilename , "%s%s.%07d.svg" , 
	     DM_EnvironmentNode->Svg_shot_dir_name ,
	     DM_EnvironmentNode->Svg_file_name , 
	     FrameNo / DM_EnvironmentNode->stepSvg );
    sprintf( description , "Snapshot svg step %d (%s)" , 
	     FrameNo / DM_EnvironmentNode->stepSvg ,
	     currentFilename );
    // fprintf( stdout , "%s\n" , description );
    fprintf( fileLog , "%s\n" , description );
    writesvg(currentFilename, description,
	     CurrentWindow->getWindowWidth(),
	     CurrentWindow->getWindowHeight());
  }

  // ******************** Png output ********************
  else if( mode == _Png ) {
    sprintf( currentFilename , "%s%s.%07d.png" , 
	     DM_EnvironmentNode->Png_shot_dir_name ,
	     DM_EnvironmentNode->Png_file_name , 
	     FrameNo / DM_EnvironmentNode->stepPng );
    sprintf( description , "Snapshot png step %d (%s)" , 
	     FrameNo / DM_EnvironmentNode->stepPng ,
	     currentFilename );
    // fprintf( stdout , "%s\n" , description );
    fprintf( fileLog , "%s\n" , description );
    glReadBuffer(GL_BACK);
    writepng(currentFilename, description, 0,0,
	     CurrentWindow->getWindowWidth(),
	     CurrentWindow->getWindowHeight());
  }

  // ******************** Jpg output ********************
  else if( mode == _Jpg ) {
    sprintf( currentFilename , "%s%s.%07d.jpg" , 
	     DM_EnvironmentNode->Jpg_shot_dir_name ,
	     DM_EnvironmentNode->Jpg_file_name , 
	     FrameNo / DM_EnvironmentNode->stepJpg );
    sprintf( description , "Snapshot jpg step %d (%s)" , 
	     FrameNo / DM_EnvironmentNode->stepJpg ,
	     currentFilename );
    // fprintf( stdout , "%s\n" , description );
    fprintf( fileLog , "%s\n" , description );
    glReadBuffer(GL_BACK);
    writejpg(currentFilename, description, 0,0,
	     CurrentWindow->getWindowWidth(),
	     CurrentWindow->getWindowHeight(),75);
  }

  // ******************** Video output ********************
#ifdef DM_HAVE_FFMPEG
  else if( mode == _Video 
	   && DM_EnvironmentNode->audioVideoOutData ) {
    glReadBuffer(GL_BACK);
    DM_EnvironmentNode->audioVideoOutData
      ->write_frame();
  }
#endif

  // ******************** interactive output ********************
  // ********************  (OpenGL or evi3d) ********************
  else if( mode == _Render ) {

    // printf( "Window %s\n" , CurrentWindow->id );
    // printf("\nproj & camera\n");
    // ------------------ projection matrix ------------------ //
    projectionMatrix();

    // ------------------ camera to root modelview matrix ---- //
    // pushes a matrix
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glPushMatrix();
    cameraToRootMVMatrix( true );
    {
      // ------------------ shadow and physics ----------------- //
      dm_matrix44 obsToRoot;

      //////////////////////////////////////////////////////
      // OpenGL scene drawing (in the canonical position)
      //////////////////////////////////////////////////////
      // OpenGL initializations
      glDisable(GL_TEXTURE_1D);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_3D);
      glDisable(GL_FOG);

      // ------------------ object pre drawing ----------------- //
      // drawing: first step = scene graph + non geometrical objects
      // stores the matrices of each object
      DM_CompositionNode->nbGeometricalObjects = 0;
      // printf("\npre draws\n");
      DM_CompositionNode->draw( mode );
    				
      // ------------------ geometrical objects drawing ----------------- //
      // drawing: fourth step = geometrical objects
      // uses the matrices stored during the first step (draw)

      // printf("\npost draws %d objects:\n",DM_CompositionNode->nbGeometricalObjects );
      for( !(*DM_CompositionNode) ; (*DM_CompositionNode)() ; 
	   (*DM_CompositionNode)++ ) {
	// printf("post draws %s\n",(*DM_CompositionNode)()->id );
	// the node should not be hidden to the current user
	if( !((*DM_CompositionNode)()->user_exclusion_display)
	    || !(((*DM_CompositionNode)()->user_exclusion_display)[CurrentIndUserNode] ) ) {
	  // reloads the currrent matrix associated with the displayed object 
	  // printf("restore MV %s %d\n" ,(*DM_CompositionNode)()->id , DM_CompositionNode->indCurrentGeometricalObject );
	  
	  if( (*DM_CompositionNode)()->IsGeometricalObject() 
	      || ((*DM_CompositionNode)()->type == ATransformation 
		  && ((dm_Transformation *)(*DM_CompositionNode)()->sceneObject)->hasDrawablePrimitive() ) ) {
	    // float *val = ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( (*DM_CompositionNode)()->getModelViewMatrixIndex() ))->GetValue();
	    // printf( "MVM ob %d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" ,  (*DM_CompositionNode)()->getModelViewMatrixIndex() , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
	    // memcpy( (char *)matView , val , 16 * sizeof( float ) );
	    (*DM_CompositionNode)()->restoreModelViewMatrix();
	  }
	  
	  // transparent objects such as sprites
	  // that should blend with everything
	  if( (*DM_CompositionNode)()->depth_test == false ) {
	    glDisable( GL_DEPTH_TEST );
	    glDepthMask( GL_FALSE ); 
	  }
	  
	  // printf("Pre Obj postDisplay %s\n",(*DM_CompositionNode)()->id);
	  (*DM_CompositionNode)()->postDisplay( mode );
	  // printf("Post Obj postDisplay %s\n",(*DM_CompositionNode)()->id);

	  // transparent objects such as sprites
	  // that should blend with everything
	  if( (*DM_CompositionNode)()->depth_test == false ) {
	    glEnable( GL_DEPTH_TEST );
	    glDepthMask( GL_TRUE ); 
	  }
	}
      }
      // printf("post draws end\n" );
    }
    // pops the matrix pushed by cameraToRootMVMatrix
    glPopMatrix();
  }

  // flushes OpenGL commands
  glFlush();

}

///////////////////////////////////////////////////////
// GUI redisplay (only in interactive mode)
///////////////////////////////////////////////////////

void draw_GUI( void ) {
  GLubyte *ScreenMessageBitmap 
    = DM_CompositionNode->pImageRaw[ DM_EnvironmentNode->message_tex_no ].dataByte;
  if( !ScreenMessageBitmap ) {
    sprintf( ErrorStr , "Error: screen message bitmap not allocated (%s)!" , ScreenMessage ); ReportError( ErrorStr ); throw 336;
  }
  // GLbyte *xfont = NULL;
  GLubyte *yfont = NULL;
  GLubyte *wfont = NULL;
  GLubyte *hfont = NULL;
  GLubyte *sfont = NULL;
  GLubyte *tfont = NULL;
  // GLubyte *afont = NULL;

  if( NewScreenMessage ) {
    switch( DM_EnvironmentNode->font_size ) {
    case 10:
      // xfont = stb__arial_10_usascii_x;
      yfont = stb__arial_10_usascii_y;
      wfont = stb__arial_10_usascii_w;
      hfont = stb__arial_10_usascii_h;
      sfont = stb__arial_10_usascii_s;
      tfont = stb__arial_10_usascii_t;
      // afont = stb__arial_10_usascii_a;
      break;
    case 11:
      // xfont = stb__arial_11_usascii_x;
      yfont = stb__arial_11_usascii_y;
      wfont = stb__arial_11_usascii_w;
      hfont = stb__arial_11_usascii_h;
      sfont = stb__arial_11_usascii_s;
      tfont = stb__arial_11_usascii_t;
      // afont = stb__arial_11_usascii_a;
      break;
    case 12:
      // xfont = stb__arial_12_usascii_x;
      yfont = stb__arial_12_usascii_y;
      wfont = stb__arial_12_usascii_w;
      hfont = stb__arial_12_usascii_h;
      sfont = stb__arial_12_usascii_s;
      tfont = stb__arial_12_usascii_t;
      // afont = stb__arial_12_usascii_a;
      break;
    case 13:
      // xfont = stb__arial_13_usascii_x;
      yfont = stb__arial_13_usascii_y;
      wfont = stb__arial_13_usascii_w;
      hfont = stb__arial_13_usascii_h;
      sfont = stb__arial_13_usascii_s;
      tfont = stb__arial_13_usascii_t;
      // afont = stb__arial_13_usascii_a;
      break;
    case 14:
      // xfont = stb__arial_14_usascii_x;
      yfont = stb__arial_14_usascii_y;
      wfont = stb__arial_14_usascii_w;
      hfont = stb__arial_14_usascii_h;
      sfont = stb__arial_14_usascii_s;
      tfont = stb__arial_14_usascii_t;
      // afont = stb__arial_14_usascii_a;
      break;
    case 15:
      // xfont = stb__arial_15_usascii_x;
      yfont = stb__arial_15_usascii_y;
      wfont = stb__arial_15_usascii_w;
      hfont = stb__arial_15_usascii_h;
      sfont = stb__arial_15_usascii_s;
      tfont = stb__arial_15_usascii_t;
      // afont = stb__arial_15_usascii_a;
      break;
    case 16:
      // xfont = stb__arial_16_usascii_x;
      yfont = stb__arial_16_usascii_y;
      wfont = stb__arial_16_usascii_w;
      hfont = stb__arial_16_usascii_h;
      sfont = stb__arial_16_usascii_s;
      tfont = stb__arial_16_usascii_t;
      // afont = stb__arial_16_usascii_a;
      break;
    case 17:
      // xfont = stb__arial_17_usascii_x;
      yfont = stb__arial_17_usascii_y;
      wfont = stb__arial_17_usascii_w;
      hfont = stb__arial_17_usascii_h;
      sfont = stb__arial_17_usascii_s;
      tfont = stb__arial_17_usascii_t;
      // afont = stb__arial_17_usascii_a;
      break;
    default:
    case 18:
      // xfont = stb__arial_18_usascii_x;
      yfont = stb__arial_18_usascii_y;
      wfont = stb__arial_18_usascii_w;
      hfont = stb__arial_18_usascii_h;
      sfont = stb__arial_18_usascii_s;
      tfont = stb__arial_18_usascii_t;
      // afont = stb__arial_18_usascii_a;
      break;
    }
   
    int pixelRank = 0;
    int lengthMax = DM_EnvironmentNode->message_pixel_length;
    memset(ScreenMessageBitmap, (GLubyte)0, lengthMax * 4 );
    // strcpy( ScreenMessage , "abcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuvwxyabcdefghijklmnopqrstuv");

    for (char *c = ScreenMessage ; *c != '\0' && pixelRank < lengthMax ; c++) 
      {
      char cur_car = *c;
      if( cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'e';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'a';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'i';
      }
      if( cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'E';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'A';
      }
      if( cur_car == '�' || 
	  cur_car == '�' ) {
	cur_car = 'I';
      }

      // usable ascii starts at blank space
      int cur_car_rank = (int)cur_car - 32;
      cur_car_rank = (cur_car_rank < 0 ? 0 : cur_car_rank );
      cur_car_rank = (cur_car_rank > 94 ? 94 : cur_car_rank );

      // defines offset according to table
      for( int indPixel = 0 ; 
	   indPixel < wfont[ cur_car_rank ] && pixelRank + indPixel < lengthMax ; indPixel++ ) {
	int indPixelColor = (pixelRank + indPixel) * 4;
	ScreenMessageBitmap[ indPixelColor ] = sfont[ cur_car_rank ]+indPixel;
	ScreenMessageBitmap[ indPixelColor + 1 ] = tfont[ cur_car_rank ];
	ScreenMessageBitmap[ indPixelColor + 2 ] = hfont[ cur_car_rank ];
	ScreenMessageBitmap[ indPixelColor + 3 ] = yfont[ cur_car_rank ];
	// printf( "%d %d - " ,  sfont[ cur_car_rank ] ,  tfont[ cur_car_rank ] );
      }
      pixelRank += wfont[ cur_car_rank ] + 1;
    }
    // printf( "\nPrt %d\n" , (unsigned int) ScreenMessageBitmap);
    DM_CompositionNode->LoadTextureInGPUMemory( DM_EnvironmentNode->message_tex_no ,
						1 ,
						1 , 
						RAW ,
						DM_BITMAP_TEXTURE ,
						false , 
						true , 
						byte_tex_format ,
						nearest_filter ,
						nearest_filter ,
						GL_TEXTURE_2D );
    NewScreenMessage = false;
  }
}


///////////////////////////////////////////////////////
// GLUT draw function (from the viewpoint)

// the glut callback
// requires predrawing (drawing from the user to the root node)

///////////////////////////////////////////////////////
// generic draw function (from the root node)
// 1 function for each window

void window_display_blank(  void ) {
  SetGlutWindow();    
  //printf( "window_display_blank\n"  );
  glMatrixMode(GL_MODELVIEW); // back into modelview mode
  glClear(GL_COLOR_BUFFER_BIT);
  if( CurrentUser ) {
    glClearColor(CurrentUser->bg_color[0],CurrentUser->bg_color[1],
		 CurrentUser->bg_color[2],CurrentUser->bg_color[3]);
  }
  else {
    glClearColor( 0 , 0 , 0 , 1 );
  }
  // suppressed for fltk and evi3d
  glutSwapBuffers();
}

// ------------------------------------------------------------ //
// --------------- DISPLAYS WINDOWS ONE AFTER ANOTHER --------- //
// ------------------------------------------------------------ //
void window_display( void ) {
  SetGlutWindow();    
  DM_EnvironmentNode->windowDisplayed = true;

  fprintf(fileLog,"begin window_display %.10f\n" , RealTime() );

  // OpenGL initializations before redisplay
  OpenGLInit();
 
  // fprintf(fileLog,"after OPenGLInit %.10f\n" , RealTime() );

  // proper scene redrawing
  draw_scene( _Render );
  
  // fprintf(fileLog,"after draw scene %.10f\n" , RealTime() );

  // specific features for interactive environments with
  // messages
  
  // printf( "Window %s\n" , CurrentWindow->id );
  draw_GUI();

  // flushes OpenGL commands
  glFlush();

  // echo
  if( DM_EnvironmentNode->echo > 0.0 ) {
    // printf( "echo %f\n" , DM_EnvironmentNode->echo );
    glAccum(GL_ACCUM , DM_EnvironmentNode->echo);
    glAccum(GL_RETURN , 1.f);
    glAccum(GL_LOAD , 1.f - DM_EnvironmentNode->echo);
  }

  // fprintf(fileLog,"after echo %.10f\n" , RealTime() );

  glutSwapBuffers();
  // 

  // fprintf(fileLog,"after glutSwapBuffers %.10f\n" , RealTime() );

  // ------------------------------------------------------------ //
  // --------------- FRAME/SUBFRAME GRABBING -------------------- //

  // ---------------- frame by frame output --------------------- //
  // Svg screen shots
  // printf("Draw Svg\n" );
  if( ( DM_EnvironmentNode->outputSvg 
	&& FrameNo % DM_EnvironmentNode->stepSvg == 0
	&& FrameNo / DM_EnvironmentNode->stepSvg >= DM_EnvironmentNode->beginSvg && 
	FrameNo / DM_EnvironmentNode->stepSvg <= DM_EnvironmentNode->endSvg ) 
        || ( WriteCurrentFrame 
	     && ( WriteCurrentFrameIndex == -1 
		  || WriteCurrentFrameIndex + 1 == 0 ) ) ) {
    // printf("Draw Svg %d\n" , FrameNo  );
    if( WriteCurrentFrame && !(*DM_EnvironmentNode->Svg_file_name) ) {
      StrCpy( &(DM_EnvironmentNode->Svg_file_name) , "frame" );
    }
    draw_scene( _Svg );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Png screen shots
  // printf("Draw Png\n" );
  if( ( DM_EnvironmentNode->outputPng 
	&& FrameNo % DM_EnvironmentNode->stepPng == 0
	&& FrameNo / DM_EnvironmentNode->stepPng >= DM_EnvironmentNode->beginPng && 
	FrameNo / DM_EnvironmentNode->stepPng <= DM_EnvironmentNode->endPng ) 
        || ( WriteCurrentFrame 
	     && ( WriteCurrentFrameIndex == -1 
		  || WriteCurrentFrameIndex + 1 == 0 ) ) ) {
    // printf("Draw Png %d\n" , FrameNo  );
    if( WriteCurrentFrame && !(*DM_EnvironmentNode->Png_file_name) ) {
      StrCpy( &(DM_EnvironmentNode->Png_file_name) , "frame" );
    }
    draw_scene( _Png );
  }
    
  // ---------------- frame by frame output --------------------- //
  // Jpg screen shots
  // printf("Draw Jpg\n"  );
  if( ( DM_EnvironmentNode->outputJpg 
	&& FrameNo % DM_EnvironmentNode->stepJpg == 0
	&& FrameNo / DM_EnvironmentNode->stepJpg >= DM_EnvironmentNode->beginJpg && 
	FrameNo / DM_EnvironmentNode->stepJpg <= DM_EnvironmentNode->endJpg ) 
        || ( WriteCurrentFrame 
	     && ( WriteCurrentFrameIndex == -1 
		  || WriteCurrentFrameIndex + 1 == 0 ) ) ) {
    // printf("Draw Jpg %d\n" , FrameNo );
    if( WriteCurrentFrame && !(*DM_EnvironmentNode->Jpg_file_name) ) {
      StrCpy( &(DM_EnvironmentNode->Jpg_file_name) , "frame" );
    }
    draw_scene( _Jpg );
  }

  // fprintf(fileLog,"after Jpg screen shots %.10f\n" , RealTime() );

  // ---------------- frame by frame output --------------------- //
  // Video screen shots
#ifdef DM_HAVE_FFMPEG
  if( DM_EnvironmentNode->outputVideo_file 
      && FrameNo % DM_EnvironmentNode->stepVideo_file == 0
      && FrameNo / DM_EnvironmentNode->stepVideo_file >= DM_EnvironmentNode->beginVideo_file ) {

    // streaming is over
    // close output streams and release memory
    if( FrameNo / DM_EnvironmentNode->stepVideo_file > DM_EnvironmentNode->endVideo_file ) {
      if( DM_EnvironmentNode->audioVideoOutData ) {
	DM_EnvironmentNode->audioVideoOutData
	  ->close_audio_video_out();
	DM_EnvironmentNode->audioVideoOutData = NULL;
      }
    }

    // streaming period
    // video output
    else {
      // opens video
      if( !(DM_EnvironmentNode->audioVideoOutData) ) {
	char * extension = strrchr( DM_EnvironmentNode->Video_file_name , '.' );
	if( extension ) {
#ifdef _WIN32
	  _snprintf( currentFilename , 
		     strlen( DM_EnvironmentNode->Video_file_name ) - strlen( extension ) ,
		     "%s" , DM_EnvironmentNode->Video_file_name );
#else
	  snprintf( currentFilename , 
		    strlen( DM_EnvironmentNode->Video_file_name ) - strlen( extension ) ,
		    "%s" , DM_EnvironmentNode->Video_file_name );
#endif
	  sprintf( currentFilename , "%s%s" , 
		   currentFilename ,
		   extension );
	  sprintf( description , "Video film (%s)" , 
		   currentFilename );
	  fprintf( stdout , "%s\n" , description );
	  fprintf( fileLog , "%s\n" , description );

	  DM_EnvironmentNode->audioVideoOutData
	    = open_audio_video_out( currentFilename ,  
				    CurrentWindow->getWindowWidth() ,
				    CurrentWindow->getWindowHeight() );
	}
	else {
	  sprintf( ErrorStr , "Error: video file extension missing [%s]!" , DM_EnvironmentNode->Video_file_name ); ReportError( ErrorStr ); throw 336;
	}
      }

      // outputs frame
      // printf("Draw Video %d\n" , FrameNo );
      draw_scene( _Video );
    }
  }
#endif
}

/*!
 * \brief keyboard events (interactive mode)
 * \param key		the key pressed
 */
void dm_process_key( int key ) {
  int mod = glutGetModifiers();

  // gets the user that corresponds to the currently active window
  GetUserAndWindow();

  // printf( "key (%i)\n" , key );

  if( mod != GLUT_ACTIVE_ALT ) {
    // printf( "push event (%i)\n" , key );
    char    keyString[2];
    keyString[0] = key;
    keyString[1] = 0;
    DM_CompositionNode->pushEvent( keystroke , keyString , NULL , 0 );
    return;
  }

  switch (key) {
  
    /* ------------------------------- frame per second */
  case 'f':
    DisplayFramePerSecond = !DisplayFramePerSecond;
    break;

    /* ------------------------------- enter:  Full screen toggle */
  case 13:
    DM_EnvironmentNode->full_screen = !(DM_EnvironmentNode->full_screen);
    if( DM_EnvironmentNode->full_screen ) {
      // the first window: the container window
      CurrentWindow->windowNonFullScreenWidth 
	= CurrentWindow->getWindowWidth();
      CurrentWindow->windowNonFullScreenHeight 
	= CurrentWindow->getWindowHeight();
      glutFullScreen();
    }
    else {
      // the first window: the container window
      // printf("return to normal shape\n");
      glutReshapeWindow( CurrentWindow->windowNonFullScreenWidth , 
			 CurrentWindow->windowNonFullScreenHeight );
      glutPositionWindow( CurrentWindow->getWindow_x() ,
			  CurrentWindow->getWindow_y() );
    }
    break;

    /* ------------------------------- printing */
  case 'p':
    FILE *fileOut;
    if ((fileOut=fopen(DM_EnvironmentNode->default_print_file_name,"wb"))==NULL) {
      sprintf( ErrorStr , "File %s not opened!" , DM_EnvironmentNode->default_print_file_name); ReportError( ErrorStr ); throw 80;
    }
    printf(  "Scene dumped in file %s\n" , DM_EnvironmentNode->default_print_file_name); 
    DM_CompositionNode->print( fileOut , true );
    fclose( fileOut );
    break;

    /* ------------------------------- snapshot */
  case 's':
    draw_scene( _Jpg );
    break;
  case 'S':
    draw_scene( _Svg );
    break;

    /* ------------------------------- change viewpoint (forward) */
  case '+':
    if( DM_EnvironmentNode->DM_Window->userRank >= 0 ) {
      dm_SceneNode  *currentUserNode 
	= DM_CompositionNode->tabUsers[ DM_EnvironmentNode
					->DM_Window->userRank ];
      dm_User *currentUser = currentUserNode->GetUser();
      currentUser->manualViewpointControl = true;
      currentUser->indCurrentViewpoint 
	= (currentUser->indCurrentViewpoint + 1) % 
	DM_CompositionNode->nbRegisteredViewpoints;
    }
    GetUserAndWindow();
    if( CurrentIndViewPoint > 0 )
      printf( "Current viewpoint %d [%s]\n" , 
	      CurrentIndViewPoint , CurrentViewPoint->GetId() );
    else
      printf( "Current viewpoint %d Observer\n" , 
	      CurrentIndViewPoint );
    break; 

    /* ------------------------------- change viewpoint (backward) */
  case '-':
    if( DM_EnvironmentNode->DM_Window->userRank >= 0 ) {
      dm_SceneNode  *currentUserNode 
	= DM_CompositionNode->tabUsers[ DM_EnvironmentNode
					->DM_Window->userRank ];
      dm_User      *currentUser = currentUserNode->GetUser();
      currentUser->manualViewpointControl = true;
      currentUser->indCurrentViewpoint = (currentUser->indCurrentViewpoint - 1 + DM_CompositionNode->nbRegisteredViewpoints ) % DM_CompositionNode->nbRegisteredViewpoints;
    }
    GetUserAndWindow();
    if( CurrentIndViewPoint > 0 )
      printf( "Current viewpoint %d [%s]\n" , 
	      CurrentIndViewPoint , CurrentViewPoint->GetId() );
    else
      printf( "Current viewpoint %d Observer\n" , 
	      CurrentIndViewPoint );
    break; 

    /* ------------------------------- key >: increasing update speed */
  case '>':
    DM_EnvironmentNode->time_step *= 1.01F;
    break;

    /* ------------------------------- key <: decreasing update speed */
  case '<':
    DM_EnvironmentNode->time_step /= 1.01F;
    break;

    // ------------------------------- temporary key for enumeration
  case '$':
    for( !(*DM_CompositionNode) ; (*DM_CompositionNode)() ; (*DM_CompositionNode)++ ) {
      printf( "type %d %s\n" , (*DM_CompositionNode)()->type , NodeTypeString[ (*DM_CompositionNode)()->type] );
      printf( "Object %.2f\n",((*DM_CompositionNode)()->getModelViewMatrix())[0] );
    }
    break;

    /* ------------------------------- key !: increasing echo */
  case '!':
    DM_EnvironmentNode->echo *= 1.1F;
    // printf( "echo %f\n" , DM_EnvironmentNode->echo );
    break;
    /* ------------------------------- key !: decreasing echo */
  case '?':
    if( DM_EnvironmentNode->echo == 0 ) {
      DM_EnvironmentNode->echo = 1.0;
    }
    DM_EnvironmentNode->echo /= 1.1F;
    // printf( "echo %f\n" , DM_EnvironmentNode->echo );
    break;

  default:
    printf ("key %d is not active.\n", key);
    break;
  }     
}

/*!
 * \brief special keys events (interactive mode)
 * \param key			the key pressed
 */
void dm_process_special_key( int key ) {
  // switch (key) {    
  // default:
  //   printf ("special key %d is not active.\n", key);
  //   break;
  // }   
}

