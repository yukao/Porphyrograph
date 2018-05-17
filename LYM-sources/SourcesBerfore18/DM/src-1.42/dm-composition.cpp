/*! \file dm-composition.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-composition.cpp
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

typedef dm_Scene * PScene;
typedef dm_GeometricalObject * PGeometricalObject;

/*!
 * interactive events queued up for later execution
 */
dm_ScheduledEvent::dm_ScheduledEvent( EventType thetype , 
				      char *theMessage ,
				      dm_Command  *command ,
				      dm_SceneNode **thetargetNodes ,
				      int thenbTargetNodes ,
				      dm_ElemAction *theDelayedElemAction ,
				      float thedelayedElemActiontimelimit ) {
  type = thetype;
  strcpy( message , theMessage );
  currentCommand = command;
  
  nbTargetNodes = thenbTargetNodes;

  targetNodes = new PSceneNode[ nbTargetNodes ];
  for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
    targetNodes[ ind ] = thetargetNodes[ ind ];
  }
  delayedElemAction = theDelayedElemAction;
  delayedElemActiontimelimit = thedelayedElemActiontimelimit;
  nextEvent = NULL;
}

/*!
 * interactive events queued up for later execution
 */
dm_ScheduledEvent::~dm_ScheduledEvent( void ) {
/*	
  if (currentCommand)
  {
  delete currentCommand;
  currentCommand = NULL;
  }
*/
  if (targetNodes) {
    nbTargetNodes = 0;
    delete [] targetNodes;
    targetNodes = NULL;
  }
  
  if (nextEvent) {
    delete nextEvent;
    nextEvent = NULL;
  }
}

/*!
 * \brief COMPOSITION
 * a composition: a set of scenes that is rendered in a frame
 */
dm_Composition::dm_Composition( void ) {
  textureIDs = NULL; // allocated dynamically
  modelViewMatrixPts = NULL; // allocated dynamically

  pImageJPG = new PtImageJPG[ DM_EnvironmentNode->Nb_max_textures ];
  pImagePNG = new PtImagePNG[ DM_EnvironmentNode->Nb_max_textures ];
  pImageRaw = new tImageRAW[ DM_EnvironmentNode->Nb_max_textures ];
  texture_names = new Pchar[ DM_EnvironmentNode->Nb_max_textures ];
#ifdef DM_HAVE_FFMPEG
  video_records = new dm_Video*[ DM_EnvironmentNode->Nb_max_textures ];
#endif 
 for( int ind = 0 ; ind < DM_EnvironmentNode->Nb_max_textures ; ind++ ) {
    //printf( "Init texture[%d]\n" , ind );
    pImageJPG[ind] = NULL;
    pImagePNG[ind] = NULL;
    pImageRaw[ind].dataByte = NULL;
    pImageRaw[ind].dataFloat = NULL;
    pImageRaw[ind].sizeX = 0;
    pImageRaw[ind].sizeY = 0;
    texture_names[ind] = NULL;
#ifdef DM_HAVE_FFMPEG
    video_records[ind] = NULL;
#endif 
  }

  NbTextures = 0;
  NbModelViewMatrixs = 0;

  selectBuf = NULL; 

  vertexShader_names = new Pchar[ DM_EnvironmentNode->Nb_max_shaders ];
  tabVertexShaders = new  PtVertexShader[ DM_EnvironmentNode->Nb_max_shaders ];
  for( int ind = 0 ; ind < DM_EnvironmentNode->Nb_max_shaders ; ind++ ) {
    //printf( "Init vertexShader[%d]\n" , ind );
    vertexShader_names[ind] = NULL;
    tabVertexShaders[ind] = NULL;
  }
  NbVertexShaders = 0;

  

  geometryShader_names = new Pchar[ DM_EnvironmentNode->Nb_max_shaders ];
  tabGeometryShaders = new  PtGeometryShader[ DM_EnvironmentNode->Nb_max_shaders ];
  for( int ind = 0 ; ind < DM_EnvironmentNode->Nb_max_shaders ; ind++ ) {
    //printf( "Init vertexShader[%d]\n" , ind );
    geometryShader_names[ind] = NULL;
    tabGeometryShaders[ind] = NULL;
  }
  NbGeometryShaders = 0;

  fragmentShader_names = new Pchar[ DM_EnvironmentNode->Nb_max_shaders ];
  tabFragmentShaders = new  PtFragmentShader[ DM_EnvironmentNode->Nb_max_shaders ];
  for( int ind = 0 ; ind < DM_EnvironmentNode->Nb_max_shaders ; ind++ ) {
    //printf( "Init fragmentShader[%d]\n" , ind );
    fragmentShader_names[ind] = NULL;
    tabFragmentShaders[ind] = NULL;
  }
  NbFragmentShaders = 0;

  // users in the current scene
  tabUsers = NULL;
  nbRegisteredUsers = 0;
  nbMaxUsers = 0;

  // viewpoints
  // the default viewpoint is viewpoint #0
  viewpoints = NULL;
  viewpointsNodes = NULL;
  viewpointsMatrices = NULL;
  nbRegisteredViewpoints = 0;
  nbMaxViewpoints = 0;

  // objects used for interaction and drawing in rendering mode
  tabGeometricalObjects = NULL;
  nbGeometricalObjects = 0;
  nbMaxGeometricalObjects = 0;
  indCurrentGeometricalObject = 0;

  selectedControlPoint = NULL;

  curentNbOSCParams = 0;
  curentOSCParams = NULL;

  scenes = new PScene[ DM_EnvironmentNode->Nb_max_scenes ];
  scene_names = new Pchar[ DM_EnvironmentNode->Nb_max_scenes ];
  NbScenes = 0;

  drawn_scenes = new PScene[ DM_EnvironmentNode->Nb_max_scenes ];
  NbDrawnScenes = 0;

  for( int ind = 0 ; ind < DM_EnvironmentNode->Nb_max_scenes ; ind++ ) {
    scenes[ind] = NULL;
    drawn_scenes[ind] = NULL;
    scene_names[ind] = NULL;
  }

  globalScript = new dm_Script();
  globalCommand = NULL;

  eventQueue = NULL;
  initializationCommand = NULL;
  terminationCommand = NULL;
  aliasCommand = NULL;

}

dm_Composition::~dm_Composition(void) {
  // printf( "delete textures\n" );
  if (textureIDs) {
    delete [] textureIDs;
    textureIDs = NULL;
  }
  
  // printf( "del MVM\n" );
  if (modelViewMatrixPts) {
    long* pTempViewMatrix = modelViewMatrixPts;
    for( int ind = 0 ;  ind < DM_EnvironmentNode->Nb_max_model_view_matrix ;
	 ind++ ) {
      if (*pTempViewMatrix) {
	delete (dm_matrix44*)(*pTempViewMatrix);
	*pTempViewMatrix = (long int) NULL_ID;
      }
      pTempViewMatrix ++;
				
/*			  if (modelViewMatrixPts[ind])
			  {
				  delete (dm_matrix44*)modelViewMatrixPts[ind];
				  modelViewMatrixPts[ind] = (long int)NULL_ID;
			  }
  */
    }

    
    delete [] modelViewMatrixPts;
    modelViewMatrixPts = NULL;
  }
  
  // printf( "del JPG\n" );
  if (pImageJPG ) {
    PtImageJPG* pTmpJpg = pImageJPG;
    for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_textures; ind++) {
      if (*pTmpJpg) {
	delete [](*pTmpJpg)->data;
	(*pTmpJpg)->data = NULL;
	
	delete *pTmpJpg;
	*pTmpJpg = NULL;
      }
      pTmpJpg++;
      
      /*
	if (pImageJPG[ind])
	{
	delete [] pImageJPG[ind]->data;
	pImageJPG[ind]->data = NULL;
	
	delete pImageJPG[ind];
	pImageJPG[ind] = NULL;
	}
      */
    }

    delete [] pImageJPG;
    pImageJPG = NULL;
  }

  // printf( "del PNG\n" );
  if (pImagePNG ) {
    PtImagePNG* pTmpPNG = pImagePNG;
    for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_textures; ind++) {
      if (*pTmpPNG) {
	// printf( "del PNG %d\n" , ind );
	delete [] (*pTmpPNG)->data;
	(*pTmpPNG)->data = NULL;
	
	delete *pTmpPNG;
	*pTmpPNG = NULL;
      }
      pTmpPNG ++;
      
      /*
	if (pImagePNG[ind])
	{
	delete [] pImagePNG[ind]->data;
	pImagePNG[ind]->data = NULL;
	
	delete pImagePNG[ind];
	pImagePNG[ind] = NULL;
	}
      */
    }
    
    delete [] pImagePNG;
    pImagePNG = NULL;
  }
  
  if (pImageRaw ) {
    tImageRAW* pTmpRaw = pImageRaw;
    for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_textures; ind++) {
      if ((*pTmpRaw).dataByte) {
	delete [] (*pTmpRaw).dataByte;
	(*pTmpRaw).dataByte = NULL;
      }
      
      if ((*pTmpRaw).dataFloat) {
	delete [] (*pTmpRaw).dataFloat;
	(*pTmpRaw).dataFloat = NULL;
      }
      pTmpRaw++;
      /*							
      if (pImageRaw[ind].dataByte)
      {
      delete [] pImageRaw[ind].dataByte;
      pImageRaw[ind].dataByte = NULL;
      }
      
      if (pImageRaw[ind].dataFloat)
      {
      delete [] pImageRaw[ind].dataFloat;
      pImageRaw[ind].dataFloat = NULL;
      }
      */
    }
    
    delete [] pImageRaw;
    pImageRaw = NULL;
  }
  
  // printf( "del text\n" );
  if (texture_names ) {
    char** pTmpTexture = texture_names;
    for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_textures; ind++)	{
      if (*pTmpTexture)	{
	delete [] *pTmpTexture;
	*pTmpTexture = NULL;
      }
      pTmpTexture ++;
      /*
	if ( texture_names[ind])
	{
	delete [] texture_names[ind];
	texture_names[ind] = NULL;
	}*/
    }
    
    delete [] texture_names;
    texture_names = NULL;
  }
  
#ifdef DM_HAVE_FFMPEG
  if (video_records ) {
    // dm_Video** pTempVideo = video_records;
    // for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_textures; ind++)	{
      // video_records are references to video properties
      // that will be deallocated by the nodes that contain them
      // should not be deallocated here
      /*
      if (*pTempVideo) {
	delete [] *pTempVideo;
	*pTempVideo = NULL;
      }
      pTempVideo++;
				*/
      /*
	if (video_records[ind])
	{
	delete [] video_records[ind];
	video_records[ind] = NULL;
	}
				*/
    // }
    
    delete [] video_records;
    video_records = NULL;
  }
#endif

  
  if (selectBuf) {
    delete [] selectBuf;
    selectBuf = NULL;
  }
  
  // printf( "del shaders\n" );
  if (DM_EnvironmentNode->Nb_max_shaders >0 ) {
    char** pTmpVtShaderName = vertexShader_names;
    dm_VertexShader** pTmpVtShader = tabVertexShaders;
    char ** pTmpFgShaderName = fragmentShader_names;
    dm_FragmentShader** pTmpFgShader = tabFragmentShaders;
    
    for (int ind = 0; ind < DM_EnvironmentNode->Nb_max_shaders; ind ++) {
      delete [] *pTmpVtShaderName;
      *pTmpVtShaderName = NULL;
      pTmpVtShaderName++;
      
      delete [] *pTmpVtShader;
      *pTmpVtShader = NULL;
      pTmpVtShader++;
      
      delete [] *pTmpFgShaderName;
      *pTmpFgShaderName = NULL;
      pTmpFgShaderName++;
      
      delete [] *pTmpFgShader;
      *pTmpFgShader = NULL;
      pTmpFgShader ++;
      /*
	delete [] vertexShader_names[ind];
	vertexShader_names[ind] = NULL;
	
	delete [] tabVertexShaders [ind];
	tabVertexShaders[ind] = NULL;
	
	delete [] fragmentShader_names[ind];
	fragmentShader_names[ind] = NULL;
	
	delete [] tabFragmentShaders[ind];
	tabFragmentShaders[ind] = NULL;
      */
    }
    
    delete [] vertexShader_names;
    vertexShader_names = NULL;
    
    delete [] tabVertexShaders;
    tabVertexShaders = NULL;
    
    delete [] fragmentShader_names;
    fragmentShader_names = NULL;
    
    delete [] tabFragmentShaders;
    tabFragmentShaders = NULL;
  }

  if (tabUsers)  {
    delete [] tabUsers;
    tabUsers = NULL;
  }
  
  // printf( "del VP\n" );
  if (nbMaxViewpoints > 0) {
    float** pTmpViewMat = viewpointsMatrices;
    for (int ind = 0; ind < nbMaxViewpoints; ind++) {
      if (*pTmpViewMat)	{
	delete [] *pTmpViewMat;
	*pTmpViewMat = NULL;
      }
      pTmpViewMat++;
    }
    
    // delete whole structure
    delete [] viewpoints;
    viewpoints = NULL;
    
    delete [] viewpointsNodes;
    viewpointsNodes = NULL;
    
    delete [] viewpointsMatrices;
    viewpointsMatrices = NULL;
    
  }
  
  // printf( "del GO\n" );
  if (tabGeometricalObjects) {
    
    delete [] tabGeometricalObjects;
    tabGeometricalObjects = NULL;
  }
  
  // printf( "del scenes\n" );
  if (DM_EnvironmentNode->Nb_max_scenes > 0) {
    for (int ind = 0; ind < NbScenes; ind++) {
      // scenes will be deleted by releaseSceneObjects
      // printf( "del scene %d\n" , ind + 1 );
      if (scenes[ind]) {
	delete scenes[ind];
	scenes[ind] = NULL;
      }
      
      if (scene_names[ind]) {
	delete [] scene_names[ind];
	scene_names[ind] = NULL;
      }
    }
    
    delete [] scenes;
    scenes = NULL;
    
    delete [] scene_names;
    scene_names = NULL;
  }
  
  // printf( "del drawn\n" );
  if (drawn_scenes) {
    delete [] drawn_scenes;
    drawn_scenes = NULL;
  }
  
  //   if (globalScript)
  //   {
  //	   delete globalScript;
  //	   globalScript = NULL;
  //   }
  
  // printf( "del eQ\n" );
  if (eventQueue) {
    delete eventQueue;
    eventQueue = NULL;
  }
  
  if (initializationCommand) {
    delete initializationCommand;
    initializationCommand = NULL;
  }
  
  if (terminationCommand) {
    delete terminationCommand;
    terminationCommand = NULL;
  }
  
  if (aliasCommand) {
    delete aliasCommand;
    aliasCommand = NULL;
  }
}
 
GLuint dm_Composition::GetTextureID( int indTexture ) {
  if( indTexture < 0 || indTexture >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: out of bounds texture index [%d]!" , indTexture ); ReportError( ErrorStr ); throw 60;
  }
  if (textureIDs == NULL) {
    textureIDs = new GLuint[ DM_EnvironmentNode->Nb_max_textures ];
    for( int ind = 0 ;
	 ind < DM_EnvironmentNode->Nb_max_textures ; 
	 ind++ ) {
      textureIDs[ ind ] = (GLuint)NULL_ID;
    }
  }
  return textureIDs[ indTexture ];
}

void dm_Composition::SetTextureID( int indTexture , GLuint aTextureID ) {
  // printf("texture #%d ID %d\n" , indTexture , aTextureID );
  if( indTexture < 0 || indTexture >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: out of bounds texture index [%d]!" , indTexture ); ReportError( ErrorStr ); throw 60;
  }
  if (textureIDs == NULL) {
    textureIDs = new GLuint[ DM_EnvironmentNode->Nb_max_textures ];
    for( int ind = 0 ; 
	 ind < DM_EnvironmentNode->Nb_max_textures; 
	 ind++ ) {
      textureIDs[ ind ] = (GLuint)NULL_ID;
    }
  }
  textureIDs[ indTexture ] = aTextureID;
}

void checkGLErrors(const char *label) {
  GLenum errCode;
  const GLubyte *errStr;
  if( false ) {
    if ((errCode = glGetError()) != GL_NO_ERROR) {
      errStr = gluErrorString(errCode);
      printf("OpenGL ERROR: %s (Label: %s)\n." , (char*)errStr , label );
    }
  }
}

bool checkFramebufferStatus() {
  GLenum status;
  status=(GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status) {
  case GL_FRAMEBUFFER_COMPLETE_EXT:
    return true;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    printf("Framebuffer incomplete,incomplete attachment\n");
    return false;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    printf("Unsupported framebuffer format\n");
    return false;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    printf("Framebuffer incomplete,missing attachment\n");
    return false;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    printf("Framebuffer incomplete,attached images \
                    must have same dimensions\n");
    return false;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    printf("Framebuffer incomplete,attached images \
                     must have same format\n");
    return false;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    printf("Framebuffer incomplete,missing draw buffer\n");
    return false;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    printf("Framebuffer incomplete,missing read buffer\n");
    return false;
  }
  return true;
}

long int dm_Composition::GetModelViewMatrixPt( int indModelViewMatrix ) {

  if( indModelViewMatrix < 0 || 
      indModelViewMatrix >= DM_EnvironmentNode->Nb_max_model_view_matrix ) {
    sprintf( ErrorStr , "Error: out of bounds ModelViewMatrix index [%d]!" , indModelViewMatrix ); ReportError( ErrorStr ); throw 60;
  }

  if (modelViewMatrixPts == NULL) {
    modelViewMatrixPts = new long int[ DM_EnvironmentNode->Nb_max_model_view_matrix ];
    for( int ind = 0 ; 
	 ind < DM_EnvironmentNode->Nb_max_model_view_matrix;
	 ind++ ) {
      modelViewMatrixPts[ ind ] = (long int)NULL_ID;
    }
  }
  // if accesses a non allocated pointer
  // allocates a new pointer
  // initializes its content with identity
  if( modelViewMatrixPts[ indModelViewMatrix ] == (long int)NULL_ID ) {
    dm_matrix44 * mat = new dm_matrix44();
    mat->LoadIdentity();
    modelViewMatrixPts[ indModelViewMatrix ] = (long int)mat;
  }
  // returns the pointer associated with the input index
  // printf("restore %d %ld\n" ,  indModelViewMatrix , (long int)(*modelViewMatrixPts)( indModelViewMatrix ) );
  return modelViewMatrixPts[ indModelViewMatrix ];
}

void dm_Composition::SetModelViewMatrixPt( int indModelViewMatrix , 
					long int aModelViewMatrixPt ) {
  // printf("store %d %ld\n" , indModelViewMatrix , aModelViewMatrixPt );
  if( indModelViewMatrix < 0 
      || indModelViewMatrix >= DM_EnvironmentNode->Nb_max_model_view_matrix ) {
    sprintf( ErrorStr , "Error: out of bounds ModelViewMatrix index [%d]!" , indModelViewMatrix ); ReportError( ErrorStr ); throw 60;
  }

  if (modelViewMatrixPts == NULL) {
    modelViewMatrixPts = new long int[ DM_EnvironmentNode->Nb_max_model_view_matrix ];
    //printf("modelViewMatrixPts initXMLTag %d %ld\n" , modelViewMatrixPts );
    for( int ind = 0 ; 
	 ind < DM_EnvironmentNode->Nb_max_model_view_matrix;
	 ind++ ) {
      modelViewMatrixPts[ ind ] = (long int)NULL_ID;
    }
  }
  modelViewMatrixPts[ indModelViewMatrix ] = aModelViewMatrixPt;
}

void dm_Composition::InitGeometricalObects()
{
  // empty existing space 
  if (tabGeometricalObjects != NULL) { 
    delete [] tabGeometricalObjects;
    tabGeometricalObjects = NULL;
  }
  
  // count objects
  nbMaxGeometricalObjects = 0;
  CountGeometricalObjects(&nbMaxGeometricalObjects); 
  // printf( "nbMaxGeometricalObjects #%d\n" , nbMaxGeometricalObjects );
  
  // inital space
  tabGeometricalObjects = new PSceneNode[nbMaxGeometricalObjects];
  for (int ind = 0; ind < nbMaxGeometricalObjects; ind ++) {
    tabGeometricalObjects[ind] = NULL;
  }
}

void dm_Composition::CountGeometricalObjects( int *nbMaxGeometricalObject ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->CountGeometricalObjects( nbMaxGeometricalObject );
  }
}

void dm_Composition::operator !( void ) {
  indCurrentGeometricalObject = 0;
}
dm_SceneNode * dm_Composition::operator ++( int ) {
  indCurrentGeometricalObject++;
  return (*this)();
}
dm_SceneNode * dm_Composition::operator --( int ) {
  indCurrentGeometricalObject--;
  return (*this)();
}
dm_SceneNode * dm_Composition::operator ()( void ){
  if( indCurrentGeometricalObject >= nbGeometricalObjects 
      || indCurrentGeometricalObject < 0 ) {
    return NULL;
  }
  return tabGeometricalObjects[ indCurrentGeometricalObject ];
}

void dm_Composition::LoadSceneFiles( int argc, char **argv ) {
  // loading the scenes
  // the scene name must be terminated by ".xml"
  for( int ind = 0 ; ind < argc - 1 ; ind++ ) {
    if( strstr( argv[ind + 1] , ".xml" ) ) {
      char *substr = strstr( argv[ind + 1] , ".xml" );
      
      if( strcmp( substr , ".xml" ) == 0 ) {
	//printf( "LoadSceneFiles %s\n" , argv[ind + 1] );

	dm_Scene *curScene = FindScene( argv[ind + 1] , NULL );
	//printf( "End of LoadSceneFiles %s\n" , argv[ind + 1] );
	//curScene->root->print( stdout , true , 0 );
	
	// adding the scene to the table of to draw scenes
	// printf( "adding the scene to the table of to draw scenes\n" );
	int sceneFound = false;
	for( int indAux = 0 ; indAux < NbDrawnScenes ; indAux++ ) {
	  if( drawn_scenes[indAux] && drawn_scenes[indAux] == curScene ) {
	    sceneFound = true;
	    break;
	  }
	}
	if( !sceneFound ) {
	  drawn_scenes[NbDrawnScenes] = curScene;
	  NbDrawnScenes++;
	}
      }
    }
    else if( !(strstr( argv[ind + 1] , ".conf" ) ) ) {
      sprintf( ErrorStr , "Warning: scene file names must have an xml extension [%s]!" , argv[ind + 1] ); ReportError( ErrorStr );
    }
  }

  // geometrical objects
  DM_CompositionNode->InitGeometricalObects();
  DM_CompositionNode->nbGeometricalObjects = 0;
  DM_CompositionNode->indCurrentGeometricalObject = 0;

  // viewpoints
  DM_CompositionNode->InitViewpoints();
  DM_CompositionNode->nbRegisteredViewpoints = 1;		// it should be set to 1 
  // because one default viewpoint
  // has been set
  DM_CompositionNode->RegisterViewpoints(
					 &(DM_CompositionNode->nbRegisteredViewpoints),
					 DM_CompositionNode->nbMaxViewpoints,
					 DM_CompositionNode->viewpoints,
					 DM_CompositionNode->viewpointsNodes);

  // users
  DM_CompositionNode->InitUsers();
  DM_CompositionNode->nbRegisteredUsers = 1;		// it should be set to 1 
  // because one default user has been set
  DM_CompositionNode->RegisterUsers(
				    &(DM_CompositionNode->nbRegisteredUsers),
				    DM_CompositionNode->nbMaxUsers,
				    DM_CompositionNode->tabUsers);

  // ------------------ users check ---------------------- //
  // verifies that the default viewpoint exists
  for( int ind = 0 ; ind < nbRegisteredUsers ; ind++ ) {
    dm_User * curUser =  tabUsers[ ind ]->GetUser();
    
    // default viewpoint given by an URI
    if( curUser->default_viewpoint_node ) {
      for( int ind = 0 ; ind < nbMaxViewpoints ; ind++ ) {
	if( ((dm_Viewpoint *)(curUser->default_viewpoint_node->sceneObject)) 
	    == viewpoints[ind] ) {
	  curUser->default_viewpoint = ind;
	  break;
	}
      }
      if( curUser->default_viewpoint < 0 ) {
	sprintf( ErrorStr , "Error: unknown user default viewpoint [%s]!" , curUser->default_viewpoint_node->id ); ReportError( ErrorStr ) ;  throw 289;
      }
      printf( "Default viewpoint %d\n" , curUser->default_viewpoint );
    }
    if( curUser->default_viewpoint_stereo_left_node ) {
      for( int ind = 0 ; ind < nbMaxViewpoints ; ind++ ) {
	if( ((dm_Viewpoint *)(curUser->default_viewpoint_stereo_left_node->sceneObject)) 
	    == viewpoints[ind] ) {
	  curUser->default_viewpoint_stereo_left = ind;
	  break;
	}
      }
      if( curUser->default_viewpoint_stereo_left < 0 ) {
	sprintf( ErrorStr , "Error: unknown user default stereo left viewpoint [%s]!" , curUser->default_viewpoint_stereo_left_node->id ); ReportError( ErrorStr ) ;  throw 289;
      }
      printf( "Default stereo left viewpoint %d\n" , curUser->default_viewpoint_stereo_left );
    }

    if( curUser->default_viewpoint >= nbRegisteredViewpoints ) {
      sprintf( ErrorStr , "Default viewpoint #%d of user %s should be lower than or equal to %d!" , curUser->default_viewpoint , curUser->GetId() , nbRegisteredViewpoints ); ReportError( ErrorStr ); throw 246;
    }
    if( curUser->alternate_stereo_viewer
	&& curUser->default_viewpoint_stereo_left >= nbRegisteredViewpoints ) {
      sprintf( ErrorStr , "Default viewpoint stereo left #%d of user %s should be lower than or equal to %d!" , curUser->default_viewpoint_stereo_left , curUser->GetId() , nbRegisteredViewpoints - 1 ); ReportError( ErrorStr ); throw 246;
    }

    // the current viewpoint
    if( nbRegisteredViewpoints > 1 ) {
      if( curUser->default_viewpoint >= 0 
	  && curUser->default_viewpoint < nbRegisteredViewpoints ) {
	curUser->indCurrentViewpoint = curUser->default_viewpoint;
      }
      else {
	curUser->indCurrentViewpoint = 1;
      }
    }
    else {
      curUser->indCurrentViewpoint = 0;
//       curUser->indCurrentViewpoint_stereo_left = 0;
    }
  }     

  // ------------------ user-based node visibility ---------------------- //
  // nodes can be hidden to some users
  RegisterUserExclusionDisplay();

  // ------------------ script reference resolution ---------------------- //
  // loading the scenes
  // the scene name must be terminated by ".xml"
  for( int ind = 0 ; ind < DM_CompositionNode->NbDrawnScenes ; ind++ ) {
    dm_Scene * curScene = DM_CompositionNode->drawn_scenes[ind];
    // script references are not resolved at
    // the preceding pass because they require
    // pointer sharing and percolateInterpolation
    // makes copies of the nodes
    // printf( "findAllScriptReferences\n" );
    //curScene->print( stdout , 0 );
    curScene->findAllScriptReferences();
    if( initializationCommand ) {
      initializationCommand->findScriptReferences( curScene->root );
      dm_Command *synchronous = initializationCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->findScriptReferences( curScene->root );
	synchronous = synchronous->synchronousCommand;
      }
    }
    if( terminationCommand ) {
      terminationCommand->findScriptReferences( curScene->root );
      dm_Command *synchronous = terminationCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->findScriptReferences( curScene->root );
	synchronous = synchronous->synchronousCommand;
      }
    }
    if( aliasCommand ) {
      dm_Command *curCommand = aliasCommand;
      dm_Command *synchronous = NULL;
      while( curCommand ) {
	curCommand->findScriptReferences( curScene->root );
	synchronous = curCommand->synchronousCommand;
	while( synchronous ) {
	  synchronous->findScriptReferences( curScene->root );
	  synchronous = synchronous->synchronousCommand;
	}
	curCommand = curCommand->nextCommand;
      }
    }
  }

  // ------------------ user/window association ---------------------- //
  // association between windows and users
  if( !( *(DM_EnvironmentNode->DM_Window->user_id) ) ) {
    // kept for backward compatibility
    printf( "Deprecated: window [%s] not associated with user_id -> associated with default user!\n" , DM_EnvironmentNode->DM_Window->id );
    DM_EnvironmentNode->DM_Window->userRank = 0;
  }
  else if( strcmp( DM_EnvironmentNode->DM_Window->user_id , "NULL" ) == 0 ) {
    // NULL user: content window that is not displayed
    // printf( "indWin %d no user_id\n" , indWin ); 
    DM_EnvironmentNode->DM_Window->userRank = -1;
  }
  else {
    // retrieves user node from user ID
    dm_SceneNode * associatedUser 
      = FindUserReference( DM_EnvironmentNode->DM_Window->user_id );
    
    if( associatedUser ) {
      DM_EnvironmentNode->DM_Window->userRank = -1;
      for( int ind = 0 ; ind < nbRegisteredUsers ; ind++ ) {
	if( tabUsers[ ind ] == associatedUser ) {
	  DM_EnvironmentNode->DM_Window->userRank = ind;
	  break;
	}
      }
    }
    else {
      sprintf( ErrorStr , "Error: window [%s] associated with unknown user [%s]!" , DM_EnvironmentNode->DM_Window->id , DM_EnvironmentNode->DM_Window->user_id ); ReportError( ErrorStr ); throw 83;
    }
  }
}

dm_Scene *dm_Composition::FindScene( char *fileName , 
				     dm_SceneNode *rootScene ) {
  int indCurScene;

  for( int ind = 0 ; ind < NbScenes ; ind++ ) {
    //printf( "Ind %d scenes[ind] %ld scene_names %s\n" , ind , scenes[ind] , scene_names[ind]  );
    if( scene_names[ind] 
	&& strcmp( scene_names[ind] , fileName ) == 0 ) {
      if( scenes[ind] ) {
	return scenes[ind];
      }
      else {
	for( int ind = 0 ; ind < NbScenes ; ind++ ) {
	  if( scene_names[ind] && !scenes[ind] ) {
	    printf( "depth %d %s\n" , NbScenes - ind , scene_names[ind]  );
	  }
	}
	sprintf( ErrorStr , "Error: unable to load scene [%s]: cyclic node references, consider merging some of the preceding files!" , fileName ); fprintf( stderr , "%s\n" , ErrorStr ); throw 83;
      }
    }
  }
  if( NbScenes >= DM_EnvironmentNode->Nb_max_scenes ) {
    sprintf( ErrorStr , "Error: unable to load scene [%s] exceeds max_scenes parameter!" , fileName ); ReportError( ErrorStr ); throw 83;
  }

  // scene loading can be recursive because of cross-references
  // therefore NbScenes must be incremented and
  // local scene index must be used
  printf( "Loading %s\n" , fileName );
  indCurScene = NbScenes;
  NbScenes++;
  scene_names[indCurScene] = new char[XMLAttrValLength];
  strcpy( scene_names[indCurScene] , fileName );
  scenes[indCurScene] = new dm_Scene( fileName );

  if( scenes[indCurScene] ) {
    scenes[indCurScene]->referenceResolution( rootScene );
    //printf( "End of loading %s\n" , fileName );
    return scenes[indCurScene];
  }
  else {
    sprintf( ErrorStr , "Error: unable to load scene [%s]!" , fileName ); ReportError( ErrorStr ); throw 87;
  }
}

#ifdef DM_HAVE_FFMPEG
dm_Video *dm_Composition::FindVideo( char *fileName ) {
  for( int ind = 0 ; ind < NbTextures ; ind++ ) {
    if( texture_names[ind] 
	&& strcmp( texture_names[ind] , fileName ) == 0 ) {
      return video_records[ind];
    }
  }
  return NULL;
}
#endif

int dm_Composition::FindTexture( char *fileName , 
				 TextureEncoding *theTextureEncoding ,
				 int textureDimension ,
				 bool is_rectangle ,
				 Texturing texturing ,
				 bool *newTexture ,
				 bool invert ) {
  *newTexture = true;
  // looks for previous textures with the same name
  // except for keypoint animation textures that are
  // allocated for each mesh
  for( int ind = 0 ; ind < NbTextures ; ind++ ) {
    if( texture_names[ind] 
	&& strcmp( texture_names[ind] , fileName ) == 0 ) {
      *newTexture = false;
      return ind;
    }
  }
  
  if( NbTextures >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: unable to load texture [%s] exceeds max_textures parameter!" , fileName ); ReportError( ErrorStr ); throw 83;
  }
  printf( "Loading %s\n" , fileName );

//   GLuint texID = 0;
  // non video texture: loaded from a bitmap file
  if( *theTextureEncoding != RAW ) {
    LoadTextureInRAM( fileName , NbTextures , 
		      theTextureEncoding , 
		      textureDimension , 
		      is_rectangle , 
		      texturing ,
		      invert );
  }

  texture_names[NbTextures] = new char[XMLAttrValLength];
  strcpy( texture_names[NbTextures] , fileName );
  NbTextures++;
  // printf( "end of Texture Loading %s\n" , fileName );
  return NbTextures - 1;
}

int dm_Composition::GenerateTexture( int vertexShaderParameterSize ,
				     bool is_rectangle ) {
  if( NbTextures >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: unable to load texture exceeds max_textures parameter!" ); ReportError( ErrorStr ); throw 83;
  }

//   GLuint texID = 0;
  GenerateTextureInRAM( vertexShaderParameterSize  , NbTextures ,
			is_rectangle );

  texture_names[NbTextures] = new char[XMLAttrValLength];
  strcpy( texture_names[NbTextures] , "vertex_shader param texture" );
  NbTextures++;
  //printf( "end of Texture Loading %s\n" , fileName );
  return NbTextures - 1;
}

int dm_Composition::GenerateTexture( bool is_rectangle , 
				     TextureFormat texture_format ,
				     int nb_attachments ) {
  if( NbTextures >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: unable to load texture (frame_buffer) exceeds max_textures parameter!" ); ReportError( ErrorStr ); throw 83;
  }

  int valreturn = NbTextures;

  //   GLuint texID = 0;
  GenerateTextureInRAM( NbTextures ,
			is_rectangle , 
			texture_format ,
			nb_attachments );

  for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
    texture_names[NbTextures] = new char[XMLAttrValLength];
    strcpy( texture_names[NbTextures] , "frame_buffer" );
    NbTextures++;
  }

  // printf( "Texture Loading No %d nb_attachments %d\n" , valreturn , nb_attachments );
  return valreturn;
}

int dm_Composition::GenerateTexture( TextureFormat texture_format ,
				     int nb_attachments ,
				     int sizeX ,
				     int sizeY ) {
  if( NbTextures >= DM_EnvironmentNode->Nb_max_textures ) {
    sprintf( ErrorStr , "Error: unable to load texture (frame_buffer) exceeds max_textures parameter!" ); ReportError( ErrorStr ); throw 83;
  }

  int valreturn = NbTextures;

  //   GLuint texID = 0;
  GenerateTextureInRAM( NbTextures ,
			texture_format ,
			nb_attachments ,
			sizeX ,
			sizeY );

  for( int ind = 0 ; ind < nb_attachments ; ind++ ) {
    texture_names[NbTextures] = new char[XMLAttrValLength];
    strcpy( texture_names[NbTextures] , "empty_texture" );
    NbTextures++;
  }

  // printf( "Texture Loading No %d nb_attachments %d\n" , valreturn , nb_attachments );
  return valreturn;
}

int dm_Composition::AddModelViewMatrixPt( void ) {
  if( NbModelViewMatrixs >= DM_EnvironmentNode->Nb_max_model_view_matrix ) {
    sprintf( ErrorStr , "Error: unable to add modelview matrix [%d] exceeds max modelview matrix parameter!" , DM_EnvironmentNode->Nb_max_model_view_matrix ); ReportError( ErrorStr ); throw 83;
  }
  SetModelViewMatrixPt( NbModelViewMatrixs , NULL_ID );
  NbModelViewMatrixs++;
  return NbModelViewMatrixs - 1;
}

void dm_Composition::print( FILE *file , int isLong ) {
  globalScript->print( file , isLong , 0 );
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    drawn_scenes[ind]->print( file , isLong );
    // scenes[ind]->print( stdout , true );
  }
}

void dm_Composition::print( dm_Environment *env , int isLong ) {
  // if the composition_output_file_name string is not empty
  // tries to dump the composition into this file
  if( *(env->composition_output_file_name) ) {
    FILE *fic = fopen( env->composition_output_file_name , "wb" );
    if( !fic ) {
      sprintf( ErrorStr , "Error: file [%s] not opened for writing!" , env->composition_output_file_name ); ReportError( ErrorStr ); throw 88;
    }
    else {
      printf( "Writing %s\n" , env->composition_output_file_name );    
      //DM_CompositionNode->print( stdout , true );
      //DM_EnvironmentNode->print( fic , true , 0 );
      env->print( fic , true , 1 );
      DM_CompositionNode->print( fic , true );
      fclose(fic);
    }
  }
}

void dm_Composition::update( void ) {
  envtNode->update( false );
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    drawn_scenes[ind]->update();
  }
}

void dm_Composition::transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    if( drawn_scenes[ind]->transformToPath( viewpoint , mode ) ) {
      return;
    }
  }
}

void dm_Composition::InitViewpoints(void) {
  // empty viewpoints
  if (viewpoints != NULL) {
    delete [] viewpoints;
    delete [] viewpointsNodes;

    for (int ind = 0; ind < nbMaxViewpoints; ind++)
      {
	if (viewpointsMatrices[ind])
	  {
	    delete [] viewpointsMatrices[ind];
	    viewpointsMatrices[ind] = NULL;
	  }
      }
    delete [] viewpointsMatrices;

    viewpoints = NULL;
    viewpointsNodes = NULL;
    viewpointsMatrices = NULL;
  }

  // count viewpoints
  nbMaxViewpoints = 1;
  CountViewpoints(&nbMaxViewpoints);
	
  // initialize viewpoints
  viewpoints = new PViewpoint[nbMaxViewpoints];
  viewpointsNodes = new PSceneNode[nbMaxViewpoints];
  viewpointsMatrices = new float*[nbMaxViewpoints];

  for (int ind = 0; ind < nbMaxViewpoints; ind++)  {
    viewpoints[ind] = NULL;
    viewpointsNodes[ind] = NULL;
    viewpointsMatrices[ind] = new float[16];
    memset( (char *)viewpointsMatrices[ind] , 0 , 16 *sizeof( float) );
  }
	
  // set the default viewpoint
  viewpoints[0] = new dm_Viewpoint();
  viewpointsNodes[0] = new dm_SceneNode();
  viewpointsNodes[0]->SetObject(AViewpoint, (dm_SceneObject *)viewpoints[0]);
	
  // add the viewpoint node to the tobe deleted node list
  viewpointsNodes[0]->daughter = deletedNodeList;
  viewpointsNodes[0]->sister = NULL;
  deletedNodeList = viewpointsNodes[0];

}

void dm_Composition::CountViewpoints( int *nbMaxViewpoints ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    drawn_scenes[ind]->CountViewpoints( nbMaxViewpoints );
    // printf( "MaxViewpoints %d\n" , *nbMaxViewpoints );
  }
}
void dm_Composition::RegisterViewpoints( int *indViewpoints , 
				      int nbMaxViewpoints ,
				      dm_Viewpoint **tabViewpoints,
				      dm_SceneNode **tabViewpointsNodes ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->RegisterViewpoints( indViewpoints , nbMaxViewpoints , 
					   tabViewpoints , 
					   tabViewpointsNodes );
  }
}

void dm_Composition::RegisterUserExclusionDisplay( void ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->RegisterUserExclusionDisplay();
  }
}

void dm_Composition::InitUsers(void) {
	// empty users
	if (tabUsers != NULL) {
		delete [] tabUsers;
		tabUsers = NULL;
	}
	
	// count users
	nbMaxUsers = 1;
	CountUsers(&nbMaxUsers);
	
	// initialize users
	tabUsers = new PSceneNode[nbMaxUsers];
	for (int ind = 0; ind < nbMaxUsers; ind++)  {
		tabUsers[ind] = NULL;
	}
	
	// set the default user to be user #0 when the values given in the environment node
	// in order to take into accout the deprecated labels navigation and viewpoints
	// in the configureation file
	 tabUsers[0] = new dm_SceneNode( AUser );
	 // add this node to the tobedelete list
	 tabUsers[0] ->daughter = deletedNodeList;
	 tabUsers[0] ->sister = NULL;
	 deletedNodeList = tabUsers[0] ;

	 *((dm_User *)(tabUsers[0]->sceneObject)) = *(DM_EnvironmentNode->default_user);
}

void dm_Composition::CountUsers( int *nbMaxUsers ) {
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->CountUsers( nbMaxUsers );
  }
}
void dm_Composition::RegisterUsers( int *indUser ,
				    int nbMaxUsers ,
				    dm_SceneNode **tabUsers ) {
  for( int ind = 0; ind < NbDrawnScenes ; ind++ ) {
    // printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->RegisterUsers( indUser , nbMaxUsers ,
				      tabUsers );
  }
}
dm_SceneNode *dm_Composition::FindUserReference( char *userId ) {
  for( int ind = 0 ; ind < nbRegisteredUsers ; ind++ ) {
    // printf( "user_id [%s] ind %d/%d\n" , tabUsers[ ind ]->id , ind , nbRegisteredUsers ); 
    if( strcmp( tabUsers[ ind ]->id , userId ) == 0 ){
      return tabUsers[ ind ];
      // 	  printf( "user_id [%s] [%d]\n" , 
      // 		  DM_EnvironmentNode->DM_Window->user_id , ind ); 
    }
  }
  return NULL;
}

void dm_Composition::draw( DrawingMode mode ) {
  //printf( "draw scenes %d\n" , NbDrawnScenes );
  for( int ind = 0 ; ind < NbDrawnScenes ; ind++ ) {
    //printf( "draw scene #%d\n" , ind );
    drawn_scenes[ind]->display( mode );
  }
}

void dm_Composition::gatherScripts( void ) {
  char chAux[128];
  globalScript->cleanScript();
  sprintf( chAux , "script %d" , GenSym++ );
  StrCpy( &(globalScript->id) , chAux );
  
  for( int ind = 0 ; ind < NbScenes ; ind++ ) {
    scenes[ind]->gatherScripts( &(globalScript->commandList) );
  }
}
  
void dm_Composition::pushEvent( EventType thetype , 
				char *theMessage , 
				dm_SceneNode **thetargetNode , 
				int nbTargetNodes ) {
  // if( theMessage ) {
  //   printf( "pushEvent (%s)\n" , theMessage );
  // }
  if( !eventQueue ) {
    eventQueue = new dm_ScheduledEvent( thetype , 
					theMessage ,
					NULL ,
					thetargetNode , nbTargetNodes ,
					NULL , 0.0 );
  }
  else {
    dm_ScheduledEvent *curEvent = eventQueue;
    while( curEvent->nextEvent ) {
      curEvent = curEvent->nextEvent;
    }
    curEvent->nextEvent = new dm_ScheduledEvent( thetype , 
						 theMessage ,
						 NULL ,
						 thetargetNode , 
						 nbTargetNodes ,
						 NULL , 0.0 );
  }
}

void dm_Composition::pushEvent( EventType thetype , 
				dm_ElemAction * theElemAction , 
				char * message ,
				dm_Command *theCommand ,
				float thedelayedElemActiontimelimit ) {
  if( !eventQueue ) {
    eventQueue = new dm_ScheduledEvent( thetype , 
					message ,
					theCommand ,
					NULL , 0 ,
					theElemAction , 
					thedelayedElemActiontimelimit );
  }
  else {
    dm_ScheduledEvent *curEvent = eventQueue;
    while( curEvent->nextEvent ) {
      curEvent = curEvent->nextEvent;
    }
    curEvent->nextEvent = new dm_ScheduledEvent( thetype , 
						 message ,
						 theCommand ,
						 NULL , 0 ,
						 theElemAction , 
						 thedelayedElemActiontimelimit );
  }
}

void dm_Composition::popOneEvent( dm_ScheduledEvent **nextEvent ,
				  dm_ScheduledEvent *precedingEvent ) {
  dm_ScheduledEvent *currentEvent = NULL;
  if( (*nextEvent) == eventQueue ) {
    // chaining
    eventQueue = eventQueue->nextEvent;
    // element deletion
    (*nextEvent)->nextEvent = NULL;
    delete (*nextEvent);
    // next event
    (*nextEvent) = eventQueue;
  }
  else {
    // saving the current event
    currentEvent = (*nextEvent);
    // chaining
    (*nextEvent) = (*nextEvent)->nextEvent;
    precedingEvent->nextEvent = (*nextEvent);
    // element deletion
    currentEvent->nextEvent = NULL;
    delete currentEvent;
	currentEvent = NULL;
  }
}

void dm_Composition::popEvents( void ) {

  // elemActions triggered by a deadline
  //   while( DM_CompositionNode->globalScript->triggerScheduledCommands( CurrentClockTime ) ) {
  //     //printf( "Scheduled event (%.2f)\n" , CurrentClockTime );
  //   }
  DM_CompositionNode->globalScript->triggerScheduledCommands( (float)CurrentClockTime );

  if( !eventQueue ) {
    return;
  }

  dm_ScheduledEvent *nextEvent = eventQueue;
  dm_ScheduledEvent *precedingEvent = NULL;

  dm_ScheduledEvent *curev = nextEvent;
  int stackDepth = 0;
  while( curev ) {
    curev = curev->nextEvent;
    stackDepth++;
  }
  if( stackDepth > 1000 ) {
    // if( true ) {
    printf( "stackDepth (%d): " , stackDepth );
    stackDepth = 1;
    curev = nextEvent;
    while( curev && stackDepth < 10 ) {
      printf("event [%s] ",curev->message);
      curev = curev->nextEvent;
      stackDepth++;
    }
    printf(" \n");
  }    
  
  while( nextEvent ) {
    bool trace = false;
    // delayed elemAction
    if( nextEvent->type == delayed_command_execution ) {
      if( trace )
	printf( "CurrentClockTime %.5f nextEvent->delayedElemActiontimelimite %.5f\n" , CurrentClockTime , nextEvent->delayedElemActiontimelimit );
      // printf("delayed event [%s] ",nextEvent->message);
      if( nextEvent->delayedElemActiontimelimit <= CurrentClockTime  ) {
	// printf( "execute delayed action (%s)\n" , nextEvent->message );
	// nextEvent->delayedElemAction->print( stdout , 1 , 1 );
	// nextEvent->currentCommand->trigger.print( stdout , 1 , 1 );

	///////////////////////////////////////////////////////////
	// message parameter extraction
	char messageCopy[XMLAttrValLength];
	char *messageParameters = NULL;
	
	// message with parameters
	// printf("\nmessage (%s)\n" , message );
	strcpy( messageCopy , nextEvent->message );
	if( *messageCopy == '/' 
	    && (messageParameters = strpbrk( messageCopy ,  "\n \t\13" ) ) ) {
	  // only keeps the message address for comparison
	  // with current command 
	  *messageParameters = 0;
	  
	  // printf("messageAddress (%s) param: (%s)\n" , message , messageParameters );
	}
	// message parameter extraction
	///////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////
	// OSC parameter instantiation

	// saves the OSC parameters of the calling command
	// in case of recursive call
	int                 previousNbOSCParams = 0;
	dm_ValScalar       *previousOSCParams = NULL;

	// allocates new local OSC variables to copy the parameters
	int                 localNbOSCParams = 0;
	dm_ValScalar       *localOSCParams = NULL;

	// if there are some, stores parameters on the composition node
	// they will be used for parameter substitution in the evaluation
	if( messageParameters ) {
	  // saves the OSC parameters of the calling command
	  previousNbOSCParams = DM_CompositionNode->curentNbOSCParams;
	  previousOSCParams = DM_CompositionNode->curentOSCParams;

	  // allocates new local OSC variables to copy the parameters

	  // stores parameters on the command node
	  nextEvent->currentCommand->StoresOSCParameters( messageParameters + 1 );

	  // printf("OSC param: " );
	  // for( int ind = 0 ; ind < curCommand->commandNbOSCParams ; ind++ ) {
	  //   curCommand->commandOSCParams[ ind ].print( stdout );
	  // }
	  // printf("\n");

	  localNbOSCParams = nextEvent->currentCommand->commandNbOSCParams;
	  localOSCParams 
	    = new dm_ValScalar[ nextEvent->currentCommand->commandNbOSCParams ];
	  // copies locally the param values
	  for( int ind = 0 ; 
	       ind < nextEvent->currentCommand->commandNbOSCParams ; ind++ ) {
	    localOSCParams[ ind ] 
	      = nextEvent->currentCommand->commandOSCParams[ ind ];
	  }

	  // stores parameters on the composition node
	  DM_CompositionNode->curentNbOSCParams = localNbOSCParams;
	  DM_CompositionNode->curentOSCParams = localOSCParams;

	  // printf("OSC param: " );
	  // for( int ind = 0 ; ind < DM_CompositionNode->curentNbOSCParams ; 
	  //      ind++ ) {
	  //   DM_CompositionNode->curentOSCParams[ ind ].print( stdout );
	  // }
	  // printf("\n");
	}
	// OSC parameter instantiation
	///////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////
	// execution
	nextEvent->delayedElemAction->execute( staticReferenceResolution , 
					       true ,
					       nextEvent->message ,
					       nextEvent->currentCommand );


	///////////////////////////////////////////////////////////
	// OSC parameter restoration

	// restores the OSC parameters of the calling command
	// in case of recursive call
	if( messageParameters ) {
	  localNbOSCParams = 0;
	  delete [] localOSCParams;
	  localOSCParams = NULL;

	  DM_CompositionNode->curentNbOSCParams = previousNbOSCParams;
	  DM_CompositionNode->curentOSCParams = previousOSCParams;
	}

	// OSC parameter restoration
	///////////////////////////////////////////////////////////


	popOneEvent( &nextEvent , precedingEvent );
      }
      else {
	// printf( "delay\n" );
	// goes to next event
	precedingEvent = nextEvent;
	nextEvent = nextEvent->nextEvent;
      }
    }
    // delayed elemAction
    
    // non-delayed elemAction
    else {
      if( trace )
	printf("non delayed event [%s] \n",nextEvent->message);
      // printf("non delayed event [%s] [%i]\n",
      // 	     nextEvent->message, nextEvent->message[0] );
      // elemActions triggered by an event without target node
      // eg keypressed
      if( !(nextEvent->nbTargetNodes) ) {
	if( DM_CompositionNode->globalScript ) {
	  // printf("global message: [%s] [%s] [%i]\n" ,
	  // 	 EventTypeString[ nextEvent->type ] , 
	  // 	 nextEvent->message, nextEvent->message[0] );
	  DM_CompositionNode->globalScript->receive( nextEvent->type , 
						     nextEvent->message ,
						     NULL );

	}
	popOneEvent( &nextEvent , precedingEvent );
      }
    
      // elemActions triggered by an event for a target node
      else {
	//     printf("nbTargetNodes %d\n" , nextEvent->nbTargetNodes  );
	for( int ind = 0 ; ind < nextEvent->nbTargetNodes ; ind++ ) {
	  dm_SceneNode   *targetNode = nextEvent->targetNodes[ind];
	  if( targetNode->script ) {
	    // printf("message received by target: [%s] [%s]\n" , EventTypeString[ nextEvent->type ] , nextEvent->message  );
	    targetNode->script->receive( nextEvent->type , 
					 nextEvent->message ,
					 targetNode );

	  }
	}
	popOneEvent( &nextEvent , precedingEvent );
      }
      // elemActions triggered by an event for a target node

    }
    // non-delayed elemAction

  }
  // event loop
}

void dm_Composition::flushDelayedEvents( void ) {
  if( !eventQueue ) {
    return;
  }

  dm_ScheduledEvent *nextEvent = eventQueue;
  dm_ScheduledEvent *precedingEvent = NULL;
  while( nextEvent ) {
    
    // delayed elemAction
    if( nextEvent->type == delayed_command_execution ) {
      // printf("delayed event [%s] ",nextEvent->message);
      popOneEvent( &nextEvent , precedingEvent );
    }
    // non-delayed elemAction
    else {
      // goes to next event
      // printf("non-delayed event [%s] ",nextEvent->message);
      precedingEvent = nextEvent;
      nextEvent = nextEvent->nextEvent;
    }
  }
  // event loop
}
