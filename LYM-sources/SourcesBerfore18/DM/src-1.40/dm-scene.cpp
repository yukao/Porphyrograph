/*! \file dm-scene.cpp
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 *
 *     File dm-scene.cpp
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

const char *EnvModeString[EmptyEnvMode + 1] = { "decal" , "modulate" , "replace" , "blend" , "blend_fragment" , "blend_cumulative" , "emptyenvmode" };

const char *BooleanString[3] = { "false" , "true" , "EmptyBoolean" };

const char *NodeTypeString[EmptyNode + 1] = { "quad" , "mesh" , "points" , "point" , "line" , "strip" , "transformation" , "viewpoint" , "texture_storage" , "pixel_reader" , "FBO_storage" , "FB_display" , "environment" , "user" , "link" , "node" , "emptynode" };

///////////////////////////////////////////////////////////
// Boolean to Int converter
///////////////////////////////////////////////////////////
int BooleanToInt( bool bool_val ) {
	if( bool_val ) {
		return 1;
	}
	else {
		return 0;
	}
}

///////////////////////////////////////////////////////////
// DEREFERENCING
///////////////////////////////////////////////////////////
void deleteReferecesDynamic (dm_SceneNode* nodeAddress, int *nbDeleted) {
  for (int ind = 0; ind < nbKnownTargets; ind++) {
		// if one target is found, delete it
    if (nodeAddress == knownTargets[ind]) {
      for (int i = ind + 1; i < nbKnownTargets; i++) {
				knownTargets[i - 1] = knownTargets[i];
			}
			nbKnownTargets --;
			nbDeleted ++;
		}
	}
}

dm_SceneNode **findReferencesDynamic( const char *targetId , int *nbReferences , 
				      bool cutIfFound , bool atRunTime ) {
  dm_SceneNode **tabNodes = NULL;

  *nbReferences = 0;

  // only dynamic references that are performed on 
  // complete scene graph can take advantage of 
  // known command list
  // dynamic references during compiling cannot use 
  // this list because the list of known references
  // changes as the scene graph is compiled
  if( atRunTime ) {
    //////////////////////////////////////////////
    // first tries to find the node(s) among the nodes
    // that have already received UDP messages
    char * knownId;
  
    // searches among the nodes that have already
    // have already been recorded
    // counts first
    for( int ind = 0 ; ind < nbKnownTargets ; ind++ ) {
      knownId = knownTargets[ ind ]->id;
      //printf("targetId [%s] knownId [%s] nbKnownTargets %d equal %d\n" , targetId + 1 , knownId , nbKnownTargets , stringEqual( targetId + 1 , knownId ) );
      if( stringEqual( (char *)(targetId + 1) , knownId ) ) {      
	(*nbReferences)++;
	if( cutIfFound ) {
	  break;
	}
      }
    }

    // then allocates and copies
    if( (*nbReferences) > 0 ) {
      if( cutIfFound ) {
	tabNodes = new dm_SceneNode*[ 1 ];
      }
      else {
	tabNodes = new dm_SceneNode*[ (*nbReferences) ];
      }

      // searches among the nodes that have already
      // received a udp message and that have already
      // been recorded
      *nbReferences = 0;
      for( int ind = 0 ; ind < nbKnownTargets ; ind++ ) {
	knownId = knownTargets[ ind ]->id;
	//printf("targetId [%s] knownId [%s] nbKnownTargets %d egal %d\n" , targetId + 1 , knownId , nbKnownTargets , stringEqual( targetId + 1 , knownId ));
	if( stringEqual( (char *)(targetId + 1) , knownId ) ) {      
	  tabNodes[ (*nbReferences) ] = knownTargets[ ind ];
	  (*nbReferences)++;

	  //printf("Nb known Nodes %d %s\n" , (*nbReferences) , targetId );
	  if( cutIfFound ) {
	    return tabNodes;
	  }
	}
      }
    }

    //printf("Nb known Nodes %d %s\n" , (*nbReferences) , targetId );
    if( (*nbReferences) > 0 ) {
      return tabNodes;
    }
  }
  
  /////////////////////////
  // resolving the node(s) reference(s)
  // cutIfFound should not be taken into account when
  // collecting references because further reference 
  // requirements might need all the references and not
  // only the first one
  for( int ind = -1 ; ind < DM_CompositionNode->NbDrawnScenes ; ind++ ) {
    // gets the nodes from the scene

    // environment node
    if( ind == -1 ) {
      // cutIfFound is not taken into account
      if( ( tabNodes = findAllReferences( envtNode , (char *)targetId , nbReferences , 
					  false , false ) ) ) {

	// only dynamic references can take advantage of 
	// known command list
	if( atRunTime ) {
	  for( int ind = 0 ; ind < *nbReferences ; ind ++ ) {
	    // copies the nodes in the global variable knownTargets
	    // so that they can be reused in a later call
	    // and copies them on the current command targetNodes
	    //printf("nbKnownTargets %d %s\n" , nbKnownTargets , tabNodes[ ind ]->id );
	    knownTargets[ nbKnownTargets++ ] = tabNodes[ ind ];
	    if( nbKnownTargets >= DM_EnvironmentNode->Nb_max_targets ) {
	      sprintf( ErrorStr , "Error: Insufficient parameter max_targets (%d)!" , DM_EnvironmentNode->Nb_max_targets ); ReportError( ErrorStr ); throw 139;
	    }
	  }
	}

	//printf("TargetNodes %d\n" , *nbReferences );
	return tabNodes;
      }	
      //printf("TargetNodes %d\n" , *nbReferences );
    }
    // regular root node
    else {
      // cutIfFound is not taken into account
      if( ( tabNodes = findAllReferences( 
				  DM_CompositionNode->drawn_scenes[ind]->root , 
				  (char *)targetId , nbReferences , 
				  false , false ) ) ) {

	// only dynamic references can take advantage of 
	// known command list
	if( atRunTime ) {
	  for( int ind = 0 ; ind < *nbReferences ; ind ++ ) {
	    // copies the nodes in the global variable knownTargets
	    // so that they can be reused in a later call
	    // and copies them on the current command targetNodes
	    //printf("nbKnownTargets %d %s\n" , nbKnownTargets , tabNodes[ ind ]->id );
	    knownTargets[ nbKnownTargets++ ] = tabNodes[ ind ];
	    if( nbKnownTargets >= DM_EnvironmentNode->Nb_max_targets ) {
	      sprintf( ErrorStr , "Error: Insufficient parameter max_targets (%d)!" , DM_EnvironmentNode->Nb_max_targets ); ReportError( ErrorStr ); throw 139;
	    }
	  }
	}

	//printf("TargetNodes %d\n" , *nbReferences );
	return tabNodes;
      }	
      //printf("TargetNodes %d\n" , *nbReferences );
    }
  }
  return NULL;
}

 
dm_SceneNode **findAllReferences( dm_SceneNode *rootNode , char *targetId ,
				  int *nbReferences , bool cutIfFound ,
				  bool reportErrors ) {
 char* fileName = NULL;
 char* nodeName = NULL;
 char tempNodeName[XMLAttrValLength];
 char tempFileName[XMLAttrValLength];

  dm_SceneNode *localRootNode;
  dm_SceneNode **tabNodes = NULL;

  // printf( "ANodeLink (%s)\n" , targetId );

  // separating file and node names
  // loading the file if necessary
  if( strstr( targetId , "xml:" ) ) {
    strcpy( tempNodeName , strstr( targetId , "xml:" ) + 4 );
    strcpy( tempFileName , targetId );
    char *endOfFileName =  strstr( tempFileName , "xml:") +3;
    *endOfFileName = 0;
     
	StrCpyAndSubst(&nodeName, tempNodeName);
	StrCpyAndSubst(&fileName, tempFileName);

	// save the reference variables in the filename and the node name.
/*
	char newfileName[XMLAttrChLength];
	char newnodeName[XMLAttrChLength];
	ReferenceNodeAttributeValue(fileName, true, newfileName);
	ReferenceNodeAttributeValue(nodeName, true, newnodeName);
*/ 
	// printf( "FindScene (%s)\n" , fileName );
    dm_Scene *scene = DM_CompositionNode->FindScene( fileName , rootNode );
		
    // printf( "end of FindScene (%s)\n" , targetId );
    if( scene && scene->root ) {
      localRootNode = scene->root;
    }
	
    else if( reportErrors ) {
      sprintf( ErrorStr , "Error: unsolved reference (file) [%s]!" , targetId ); ReportError( ErrorStr ); throw 85;
    }      
    else {
		if (nodeName) 
		{
			delete []nodeName;
			nodeName = NULL;
		}

		if (fileName)
		{
			delete []fileName;
			fileName = NULL;
		}
      return NULL;
    }
  }
  else {
    strcpy( tempNodeName , targetId );
	StrCpyAndSubst(&nodeName, tempNodeName);
    localRootNode = rootNode;
  }

  
  // printf( "node resolution (%s)\n" , nodeName );
  tabNodes = NULL;
  if ((nodeName != NULL) && ( *nodeName == '#' )) {
    // printf( "Link [%s]\n" , targetId );
    *nbReferences = 0;


    // searching for several nodes
    // (tabNodes not yet allocated: first count, then allocate, then load
    if( !cutIfFound ) {
      localRootNode->findAllNodesInTree( nodeName + 1 , NULL ,
					 nbReferences , cutIfFound , true , true );
      tabNodes = new dm_SceneNode*[ *nbReferences ];
      *nbReferences = 0;
      localRootNode->findAllNodesInTree( nodeName + 1 , tabNodes ,
					 nbReferences , cutIfFound , false , true );
    }
    // searching for a single node 
    // (tabNodes already allocated for 1 node pter) 
    else {
      tabNodes = new dm_SceneNode*[ 1 ];
      localRootNode->findAllNodesInTree( nodeName + 1 , tabNodes ,
					 nbReferences , true , false , true );
    }

    // printf( "Link nbReferences [%d]\n" , *nbReferences );
    if( *nbReferences ) {
		if (nodeName) 
		{
			delete []nodeName;
			nodeName = NULL;
		}

		if (fileName)
		{
			delete []fileName;
			fileName = NULL;
		}

      return tabNodes;
    }
    else if( reportErrors ) {
      sprintf( ErrorStr , "Error: unsolved reference (node) [%s]!" , nodeName + 1 ); ReportError( ErrorStr ); throw 84;
    }
    else {
      if (tabNodes) {
	delete [] tabNodes;
	tabNodes = NULL;
      }

		if (nodeName) 
		{
			delete []nodeName;
			nodeName = NULL;
		}

		if (fileName)
		{
			delete []fileName;
			fileName = NULL;
		}

      return NULL;
    }
  }
  else if( reportErrors ) {
    sprintf( ErrorStr , "Error: incorrect link named (node names should begin with # [%s]!" , targetId ); ReportError( ErrorStr ); throw 53;
  }
  else {

    if (tabNodes) {
      delete [] tabNodes;
      tabNodes = NULL;
    }
	if (nodeName) 
	{
		delete []nodeName;
		nodeName = NULL;
	}

	if (fileName)
	{
		delete []fileName;
		fileName = NULL;
	}
    
    return NULL;
  }
  
  if (nodeName) 
  {
     delete []nodeName;
     nodeName = NULL;
  }

 if (fileName)
 {
	delete []fileName;
	fileName = NULL;
 }

  return NULL;
}

dm_SceneNode::dm_SceneNode( void ) {
  id = NULL;
  internalState = NULL;

  Init();
}
	
dm_SceneNode::~dm_SceneNode(void) {

  if (daughter)	{
    delete daughter;
    daughter = NULL;
  }
  
  if (sister) {
    delete sister;
    sister = NULL;
  }
  
  if (id) {
    delete []id;
    id = NULL;
  }
  
  if (internalState) {
    delete [] internalState;
    internalState = NULL;
  }

  // lisScalars/lisTables/lisMatrices will be deleted with a global list
  /*
    if (lisScalars)
    {
    delete lisScalars;
    lisScalars = NULL;
    }
  */
  /*
    if (lisTables)
    {
    delete [] lisTables;
    lisTables = NULL;
    }
  */
  /*
    if (lisMatrices) 
    {
    delete [] lisMatrices;
    lisMatrices = NULL;
    }
  */
  // the scene object need not be deleted 
  // because it will be deleted indepently by the release of the global link
  /*
    if (sceneObject) 
    {
    delete sceneObject;
    sceneObject = NULL;
    }
  */
  /*
    if (motherInterpolatorNode)
    {
    delete motherInterpolatorNode;
    motherInterpolatorNode = NULL;
    }
  */

  if (user_exclusion_display) {
    delete []user_exclusion_display;
    user_exclusion_display = NULL;
  }
  
  if (userString_exclusion_display) {
    delete []userString_exclusion_display;
    userString_exclusion_display = NULL;
  }

  if (valScalarModelViewMatrix)	{
    delete [] valScalarModelViewMatrix;
    valScalarModelViewMatrix = NULL;
  }
}

dm_SceneNode::dm_SceneNode( NodeType theType ) {
  id = NULL;
  internalState = NULL;
  Init();
  SetType( theType );
}

dm_SceneNode::dm_SceneNode( NodeType theType , dm_SceneObject *aSceneObject ) {
  id = NULL;
  internalState = NULL;
  Init();
  SetObject( theType , aSceneObject );
}

void dm_SceneNode::Init( void ) {
  InitString( &id );
  if (internalState) {
    delete [] internalState;
    internalState = NULL;
  }
  StrCpy( &internalState , "init" );
  script = NULL;
  renderingCommand = NULL;
  daughter = NULL;
  sister = NULL;

  lisScalars = NULL;
  lisTables = NULL;
  lisMatrices = NULL;

  type = EmptyNode;
  sceneObject = NULL;

  // visibillity
  displayed = true;
  depth_test = true;

  user_exclusion_display = NULL;
  userString_exclusion_display = NULL;
  userFloatTable_exclusion_display = NULL;

  // invalidation
  validate();

  // modelview matrix
  modelViewMatrixIndex = -1;
  // possibly returned model view matrix
  valScalarModelViewMatrix = NULL;
}

void dm_SceneNode::InitGeometricalAttributes( void ) {
  if( !sceneObject ) {
    return;
  }

  if( IsGeometricalObject() 
      || type == AViewpoint 
      || (type == ATransformation && sceneObject
	  && ((dm_Transformation *)sceneObject)->hasDrawablePrimitive() ) ) {
    // adds a new index in the multithread storage of modelview matrix
    modelViewMatrixIndex = DM_CompositionNode->AddModelViewMatrixPt();
//     printf("AddModelViewMatrixPt %s %d\n" , id ,modelViewMatrixIndex);
    // associates this index with a pointer 
    DM_CompositionNode->SetModelViewMatrixPt( modelViewMatrixIndex , 
				       (long int)new dm_matrix44() );
    // temporary check for pointer restoring
    // printf("load Ident %s\n" , id );
    ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( 
			   modelViewMatrixIndex ))->LoadIdentity();
   }

  if( type < 0 || type >= EmptyNode ) {
    sprintf( ErrorStr , "Error: unknown geometrical object type [%d]!" , type ); ReportError( ErrorStr ); throw 139;
  }
  
  invalidate();
}

void dm_SceneNode::SetType( NodeType theType ) {
  if( theType < 0 || theType >= EmptyNode ) {
    sprintf( ErrorStr , "Error: unknown elementary type [%d]!" , theType ); ReportError( ErrorStr ); throw 139;
  }
  type = theType;

  switch( type ) {
  case AUser:
    sceneObject = (dm_SceneObject *)(new dm_User());
    break;

  case AMesh:
    sceneObject = (dm_SceneObject *)(new dm_Mesh());
    break;
  case APoints:
    sceneObject = (dm_SceneObject *)(new dm_Points());
    break;
  case AQuad:
    sceneObject = (dm_SceneObject *)(new dm_Quad());
    break;
  case APoint:
    sceneObject = (dm_SceneObject *)(new dm_Point());
    break;
  case ALine:
    sceneObject = (dm_SceneObject *)(new dm_Line());
    break;
  case AStrip:
    sceneObject = (dm_SceneObject *)(new dm_Strip());
    break;
  case ATransformation:
    sceneObject = (dm_SceneObject *)(new dm_Transformation());
    break;
  case AEnvironment:
    sceneObject = (dm_SceneObject *)(new dm_Environment());
    break;
  case AViewpoint:
    sceneObject = (dm_SceneObject *)(new dm_Viewpoint());
    break;
  case ATextureStorage:
    sceneObject = (dm_SceneObject *)(new dm_TextureStorage());
    break;
  case APixelReader:
    sceneObject = (dm_SceneObject *)(new dm_PixelReader());
    break;
  case AFBOStorage:
    sceneObject = (dm_SceneObject *)(new dm_FBOStorage());
    break;
  case AFBDisplay:
    sceneObject = (dm_SceneObject *)(new dm_FBDisplay());
    break;
  case ANodeLink:
    sceneObject = NULL;
    break;
  case ANode:
    sprintf( ErrorStr , "Error: Node constructor not implemented!" ); ReportError( ErrorStr ); throw 139;
    break;
  case EmptyNode:
    break; 
  }

  InitGeometricalAttributes();
}

void dm_SceneNode::SetObject( NodeType theType , dm_SceneObject *aSceneObject ) {
  if( theType < 0 || theType >= EmptyNode ) {
    sprintf( ErrorStr , "Error: unknown elementary scene object type [%d]!" , theType ); ReportError( ErrorStr ); throw 139;
  }
  type = theType;

  if( !aSceneObject ) {
    sprintf( ErrorStr , "Error: Node component [%s] of type [%s] initialized without primitive!" , id , NodeTypeString[ type ] ); ReportError( ErrorStr ); throw 139;
  }
  sceneObject = aSceneObject;

  InitGeometricalAttributes();
}


void dm_SceneNode::releaseForEnvtNode(void)
{
  if (id) {
    delete [] id;
    id = NULL;
  }

  if (internalState) {
    delete [] internalState;
    internalState = NULL;
  }

/*
  if (lisScalars) {
  delete lisScalars;
  lisScalars = NULL;
  }
*/
  // set other released members to be NULL
  daughter = NULL;
  sister = NULL;
  //	sceneObject = NULL;
  //	lisMatrices = NULL;
  //	lisScalars = NULL;
  //	lisTables = NULL;
}

void dm_SceneNode::storeModelViewMatrix( void ) {
  glGetFloatv( GL_MODELVIEW_MATRIX , 
	       ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( modelViewMatrixIndex ))->GetValue() );
//   float * targetModelView = ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( modelViewMatrixIndex ))->GetValue();
//   printf( "targetModelView %s %d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , id , modelViewMatrixIndex, -targetModelView[0] , -targetModelView[1] , -targetModelView[2] , -targetModelView[3] , -targetModelView[4] , -targetModelView[5] , -targetModelView[6] , -targetModelView[7] , -targetModelView[8] , -targetModelView[9] , -targetModelView[10] , -targetModelView[11] , -targetModelView[12] , -targetModelView[13] , -targetModelView[14] , -targetModelView[15] );
 
}

void dm_SceneNode::restoreModelViewMatrix( void ) {
  // float *val = ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( modelViewMatrixIndex ))->GetValue();
  // printf( "MVM ob %s %d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" , id , modelViewMatrixIndex , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );

  float *val = ((dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( getModelViewMatrixIndex() ))->GetValue();
  glLoadMatrixf( val );
  // printf( "MVM ob %d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n" ,  (*DM_CompositionNode)()->getModelViewMatrixIndex() , val[0] , val[1] , val[2] , val[3] , val[4] , val[5] , val[6] , val[7] , val[8] , val[9] , val[10] , val[11] , val[12] , val[13] , val[14] , val[15] );
  memcpy( (char *)matView , val , 16 * sizeof( float ) );
}

float * dm_SceneNode::getModelViewMatrix( void ) {
  return( (dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( modelViewMatrixIndex ))->GetValue();
}

int dm_SceneNode::getModelViewMatrixIndex( void ) {
  return modelViewMatrixIndex;
}

void dm_SceneNode::setModelViewMatrixIndex( int index ) {
  modelViewMatrixIndex = index;
}

dm_matrix44 * dm_SceneNode::getModelViewMatrix44( void ) {
  return( (dm_matrix44 *)DM_CompositionNode->GetModelViewMatrixPt( modelViewMatrixIndex ));
}

dm_Transformation * dm_SceneNode::getTransformationNode( void ) {
  if( type == ATransformation ) {
    return (dm_Transformation *)sceneObject;
  }
  return NULL;
}

bool dm_SceneNode::IsGeometricalObject( void ) {
  if( sceneObject ) {
    return sceneObject->IsGeometricalObject();
  }
  return false;
}

void dm_SceneNode::deletevertexArray( void ) {
  if( IsGeometricalObject() ) {
    if( GetvertexArrayID() != NULL_ID ) {
      // printf("delete display list %d (%s)\n" , GetvertexArrayID() , id );
      sceneObject->DeletevertexArrayID();
    }
  }
}

void dm_SceneNode::deleteelementArray( void ) {
  if( IsGeometricalObject() ) {
    if( GetelementArrayID() != NULL_ID ) {
      // printf("delete display list %d (%s)\n" , GetelementArrayID() , id );
      sceneObject->DeleteelementArrayID();
    }
  }
}

void dm_SceneNode::deletevertexBufferObject( void ) {
  if( IsGeometricalObject() ) {
    if( GetvertexBufferObjectID() != NULL_ID ) {
      // printf("delete display list %d (%s)\n" , GetelementArrayID() , id );
      sceneObject->DeletevertexBufferObjectID();
    }
  }
}

GLuint dm_SceneNode::GetvertexArrayID( void ) {
  if( IsGeometricalObject() ) {
    return sceneObject->GetvertexArrayID();
  }
  else {
    // should be drawn, so returns NULL_ID
    return NULL_ID;
  }
}

GLuint dm_SceneNode::GetelementArrayID( void ) {
  if( IsGeometricalObject() ) {
    return sceneObject->GetelementArrayID();
  }
  else {
    // should be drawn, so returns NULL_ID
    return NULL_ID;
  }
}

GLuint dm_SceneNode::GetvertexBufferObjectID( void ) {
  if( IsGeometricalObject() ) {
    return sceneObject->GetvertexBufferObjectID();
  }
  else {
    // should be drawn, so returns NULL_ID
    return NULL_ID;
  }
}

void dm_SceneNode::SetvertexArrayID( GLuint alistID ) {
  if( IsGeometricalObject() ) {
    sceneObject->SetvertexArrayID( alistID );
  }
}

void dm_SceneNode::SetelementArrayID( GLuint alistID ) {
  if( IsGeometricalObject() ) {
    sceneObject->SetelementArrayID( alistID );
  }
}

void dm_SceneNode::SetvertexBufferObjectID( GLuint alistID ) {
  if( IsGeometricalObject() ) {
    sceneObject->SetvertexBufferObjectID( alistID );
  }
}

dm_Shader *dm_SceneNode::GetMaterialShader( void ) {
 if( IsGeometricalObject() ) {
    return sceneObject->GetMaterialShader();
  }
  return NULL;
}

dm_MaterialProperties *dm_SceneNode::GetMaterial( void ) {
  if( IsGeometricalObject() ) {
    return sceneObject->GetMaterial();
  }
  return NULL;
}

#ifdef DM_HAVE_FFMPEG
dm_Video *dm_SceneNode::GetVideo( void ) {
  dm_MaterialProperties * mat;
  if( (mat =  GetMaterial() ) ) {
    return mat->materialVideo;
  }
  dm_Shader *shader = GetMaterialShader();
  if( shader ) {
    // printf( "get shader video [%s]\n" , id );
    if( shader->shaderType == GENERALIZED_SHADER ) {
      if( shader->shaderSource ) {
	for( int textureRank = 0 ; 
	     textureRank < shader->shaderSource->nb_shaderTextures ; 
	     textureRank++ ) {
	  if( shader->shaderSource->materialVideo[ textureRank ] ) {
	    return shader->shaderSource->materialVideo[ textureRank ];
	  }
	}
      }
    }
  }
  return NULL;
}
#endif

dm_User *dm_SceneNode::GetUser( void ) {
  if( type == AUser ) {
    return (dm_User *)sceneObject;
  }
  else {
    sprintf( ErrorStr , "Error: Node %s not user node!" , id ); ReportError( ErrorStr ); throw 139;
  }
  return NULL;
}

void dm_SceneNode::invalidate( void ) {
  // invalidation
  is_invalid = true;
}

bool dm_SceneNode::isInvalid( void ) {
  // invalidation value for the current window
  return is_invalid;
}

void dm_SceneNode::validate( void ) {
  // invalidation
  is_invalid = false;
}

int dm_SceneNode::transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode ) {
  // no push or pop matrix for transformToPath
  // its only purpose is to gather the inverse
  // transformations on the way back from the path
  // to the root
  if( type == AViewpoint && (dm_Viewpoint *)sceneObject == viewpoint ) {
    return true;
  }

  if( daughter ) {
    if( daughter->transformToPath( viewpoint , mode ) ) {
      dm_Transformation * transf = NULL;
      if( (transf = getTransformationNode() ) ) {
	transf->drawInverseTransformation( mode );
      }
      return true;
    }
  }
  
  if( sister ) {
    if( sister->transformToPath( viewpoint , mode ) ) {
      return true;
    }
  }
  return false;
}

void SaveEnvironment( DrawingMode mode , char *id ) {
  glPushMatrix();
  // printf( " push matrix (dr)\n" );
  checkGLErrors( "PushMatrix" );
  depthTransformation++;
}

void restoreEnvironment( DrawingMode mode ) {
  glPopMatrix();
  checkGLErrors( "PopMatrix" );
  depthTransformation--;
}

void dm_SceneNode::displayVideo( DrawingMode mode , int saveEnvironment ) {
  GetMaterial()->displayVideo( mode );
}

void dm_SceneNode::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
				  FloatBinOp oper , IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    StrCpy( &id , XMLtag->getAttributeStringValue( "id" ) );
  }

  if( !atRunTime ) {
    displayed = true;
  }
  if( XMLtag->hasAttribute( "displayed" ) ) {
    XMLtag->BooleanFieldValue( "displayed" ,
			       &(displayed) );
  }

  if( !atRunTime ) {

    user_exclusion_display = NULL;
    userString_exclusion_display = NULL;
    userFloatTable_exclusion_display = NULL;
  }
  if( XMLtag->hasAttribute( "user_exclusion_display" ) ) {
    StrCpy( &userString_exclusion_display , 
	    XMLtag->getAttributeStringValue( "user_exclusion_display" ) );
    if( atRunTime ) {
      ComputeUserExclusionDisplay( oper );
    }
  }

  if( !atRunTime ) {
    depth_test = true;
  }
  if( XMLtag->hasAttribute( "depth_test" ) ) {
    XMLtag->BooleanFieldValue( "depth_test" ,
			&(depth_test) );
  }

}

bool dm_SceneNode::getParameters( char *attribute , 
				  DataType * dataType ,
				  double * valDouble ,
				  int * valInt ,
				  bool * valBool ,
				  char ** valString ,
				  int * tableOrMatrixSize ,
				  dm_ValScalar ** valScalars ) {
  *dataType = EmptyDataType;

  if( strcmp( attribute , "id" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = id;
    return true;
  }
  else if( strcmp( attribute , "displayed" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = displayed;
    return true;
  }
  else if( strcmp( attribute , "user_exclusion_display" ) == 0 ) {
    *dataType = DataTypeDouble;
    *tableOrMatrixSize = DM_CompositionNode->nbRegisteredUsers;
    if( !userFloatTable_exclusion_display ) {
      userFloatTable_exclusion_display
	= new dm_Table( DM_CompositionNode->nbRegisteredUsers );
    }
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; ind++ ) {
      userFloatTable_exclusion_display
	->SetValue( (float)user_exclusion_display[ind] , ind );
    }
    return true;
  }
  else if( strcmp( attribute , "depth_test" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = depth_test;
    return true;
  }
  else if( strcmp( attribute , "state" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = internalState;
    return true;
  }
  else if( strcmp( attribute , "modelViewMatrix" ) == 0 
	   && !IsGeometricalObject() ) {
    if( type == AViewpoint ) {
      if( CurrentViewPointMatrix ) {
	// printf( "get camera modelViewMatrix (%s)\n" , id );
	*tableOrMatrixSize = 16;
	float * matValues = CurrentViewPointMatrix;
	if( !valScalarModelViewMatrix ) {
	  valScalarModelViewMatrix = new dm_ValScalar[ 16 ];
	}
	*valScalars = valScalarModelViewMatrix;
	for( int ind = 0 ; ind < 16 ; ind++ ) {
	  valScalarModelViewMatrix[ind].SetType( DataTypeDouble );
	  valScalarModelViewMatrix[ ind ].SetValue( matValues[ ind ] );
	}
	return true;
      }
      else {
      sprintf( ErrorStr , "Error: non assigned viewpoint modelViewMatrix  [%s(%s)]!" , id , NodeTypeString[ type ] ); ReportError( ErrorStr ); return false;
	return false;
      }
    }
    else {
      sprintf( ErrorStr , "Error: get modelViewMatrix on a non geometrical object [%s(%s)]!" , id , NodeTypeString[ type ] ); ReportError( ErrorStr ); return false;
    }
  }
  else if( strcmp( attribute , "modelViewMatrix" ) == 0 
	   && IsGeometricalObject() ) {
    // printf( "get modelViewMatrix (%s)\n" , id );
    *dataType = DataTypeDouble;
    dm_matrix44 *returnedValue = getModelViewMatrix44();
    *tableOrMatrixSize = 16;
    float * matValues = returnedValue->GetValue();
    if( !valScalarModelViewMatrix ) {
      valScalarModelViewMatrix = new dm_ValScalar[ 16 ];
    }
    *valScalars = valScalarModelViewMatrix;
    for( int ind = 0 ; ind < 16 ; ind++ ) {
      valScalarModelViewMatrix[ind].SetType( DataTypeDouble );
      valScalarModelViewMatrix[ ind ].SetValue( matValues[ ind ] );
    }
    return true;
  }
  return false;
}


void dm_SceneNode::postDisplay( DrawingMode mode ) {
  ///////////////////::
  // rule for interpolators: before or during interpolation
  // during interpolation : must be redrawn without calling the
  // display list
  
  //   if( strcmp( "root" , id ) == 0 ) {
  //        printf( "postDisplay obj %s \n" , id);
  //   }
  
  //  glPushMatrix();

  // specific geometrical transformations applied to
  // some geometrical objects that cannot be located 
  // in the display function because they will
  // not be taken into account by the shader in case
  // of a display list

  // executes the rendering script if there is one
  // a rendering script contains commands 
  // that can be applied during the course of a rendering
  // One of the main interests of these commands is to
  // modify the frustum while a scene is rendered to
  // allow for multiscale rendering.
  // these commands must be bound to a geometrical objects
  // so that they are called during the fourth step
  // of drawing (the phase after storing the MVMatrices
  // during which postDisplay is called on geometrical
  // objects)
  if(IsGeometricalObject() && renderingCommand ) {
    dm_Command *synchronous = NULL;
    // printf( "rendering command\n" );
    renderingCommand->execute( staticReferenceResolution , false , (char *)"" );
    synchronous = renderingCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->execute( staticReferenceResolution , false , (char *)"" );
      synchronous = synchronous->synchronousCommand;
    }
      
    // reinitializes the projection matrix	  
    // ------------------ projection matrix ------------------ //
    projectionMatrix();
    glMatrixMode(GL_MODELVIEW);
    // the MVM is not modified -> this means that 
    // these command cannot change the scene graph
  }
  
  if( !IsGeometricalObject() ) {
    // printf( "display non geometrical object %s %s\n" , id , NodeTypeString[type] );
    // the path of a transformation must be
    // redrawn if the transformation has been modified
    bool invalidDL;
    sceneObject->display ( mode , &invalidDL );
  }
  else if( type == APixelReader ) {
    dm_PixelReader * aPixelReader = (dm_PixelReader *)sceneObject;
    bool invalidDL;
    if( aPixelReader ) {
      aPixelReader->display( mode , &invalidDL );
    }
  }
  else if( type == AFBOStorage ) {
    // printf( "pre FBO storage \n" );
    dm_FBOStorage * aFBOStorage = (dm_FBOStorage *)sceneObject;
    bool invalidDL;
    if( aFBOStorage ) {
      aFBOStorage->display( mode , &invalidDL );
    }
    // printf( "post FBO storage \n" );
  }
  else if( type == AFBDisplay ) {
    // printf( "pre FB display \n" );
    dm_FBDisplay * aFBDisplay = (dm_FBDisplay *)sceneObject;
    bool invalidDL;
    if( aFBDisplay ) {
      aFBDisplay->display( mode , &invalidDL );
    }
    // printf( "post FB display \n" );
  }
  // interpolator or non interpolator node
  // regenerates the vertex data if necessary
  else {  
    // printf( "bindVertexData geometrical object %s (%s) invalid %d vertexArrayID %d elementArrayID %d vertexBufferObjectID %d \n" , NodeTypeString[type] , id, isInvalid() , GetvertexArrayID() , GetelementArrayID(), GetvertexBufferObjectID() );

    bool invalidDL = isInvalid(); // invalid object
    sceneObject->display( mode , &invalidDL  );
    is_invalid = false;

    // printf( "after bindVertexData geometrical object %s (%s) invalid %d vertexArrayID %d elementArrayID %d vertexBufferObjectID %d \n \n" , NodeTypeString[type] , id, isInvalid() , GetvertexArrayID() , GetelementArrayID() , GetvertexBufferObjectID()  );
    // printf( "bindVertexData %d [%s/%s] %ld\n" , GetvertexArrayID() , id , NodeTypeString[type] , this );
  }
}

void dm_SceneNode::display( DrawingMode mode , 
			    bool saveEnvironment ,
			    bool storeCurrentMatrix ) {
  dm_SceneNode * currentNode;

  // saving the graphic context for restoring it when backtracking
  // interpolator do not call for environment save/restore
  // because they are empty nodes whose semantics is in their
  // daughter node
  if( saveEnvironment 
      && type != ATextureStorage
      && type != APixelReader
      && type != AFBOStorage
      && type != AFBDisplay ) {
    SaveEnvironment( mode , id );
  }

  currentNode = this;
  // printf( "draw %s %d\n" , id,displayed );

  // displayed node
  if( displayed ) {
    // printf( "draw %s %s\n" , id , NodeTypeString[type] );

    // drawing video  of geometrical objects
    if( currentNode->IsGeometricalObject() ) {
      // printf("Display video\n");
      currentNode->displayVideo( mode , true );
    }

    // store the model view matrix
    // particles are not stored
    // because of coreference and because of size
    dm_Transformation * transf = NULL;
    if( storeCurrentMatrix 
	&& ( currentNode->IsGeometricalObject() 
	     || type == AViewpoint 
	     || ((transf = getTransformationNode() )
		 && transf->hasDrawablePrimitive() ) ) ) {
      currentNode->storeModelViewMatrix();
      // for the environment map, the inverse matrix is stored for GPU rendering
    }
    
    // SINGLE PASS RENDERING
    // concerns mainly non geometrical object rendering
    // that do not have display lists such as transformations
    // and that must be rendered in a single pass
    // transformations with geometrical objects must also be drawn
    // in two passes because of their geometrical content
    if( !storeCurrentMatrix
	|| (!currentNode->IsGeometricalObject() 
	    && !((transf = getTransformationNode() ) 
		 && transf->hasDrawablePrimitive() ) ) ) {
      // transparent objects such as sprites
      // that should blend with everything
      if( currentNode->depth_test == false ) {
	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE ); 
      }
      
      if( !transf ) {
	currentNode->postDisplay( mode );
      }
      else {
	transf->preDisplay( mode );
      }
      
      if( currentNode->depth_test == false ) {
	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE ); 
      }
    }
    
    // TWO PASS RENDERING: here we store the modelview matrix and
    // a pointer to the object
    // in a second pass, the modelview matrix will be restored and
    // the object will be postdisplayed
    // Interactive  drawing is postponed to a second
    // phase so that computations such as shadowing can occur inbetween
    // memorizes the node pointer for second pass drawing
    // first pass: pointer memorization
    else {
      // if( type == ATransformation ) {
      //   printf( "stored geom obj %s \n", id );
      // }
      // the transformations with geometrical objects
      // are drawn in the first and second pass
      // first pass: stores the geometrical positionning
      // second pass: draws the geometrical primitive
      if( (transf = getTransformationNode() ) ) {
	transf->preDisplay( mode );
      }
      
      if( IsGeometricalObject()
	  || ( (transf = getTransformationNode() ) 
	       && transf->hasDrawablePrimitive() ) ) {
	if( DM_CompositionNode->nbGeometricalObjects 
	    < DM_CompositionNode->nbMaxGeometricalObjects ) {
	  // printf( "store obj %s (%d) \n" , id , DM_CompositionNode->nbGeometricalObjects);
	  DM_CompositionNode->tabGeometricalObjects[ 
						    (DM_CompositionNode->nbGeometricalObjects)++ ]
	    = currentNode;
	}
	else {
	  sprintf( ErrorStr , "Error: insufficient number of drawables %d, node cannot be drawn [%s]!" , DM_CompositionNode->nbMaxGeometricalObjects , id ); ReportError( ErrorStr ); throw 220;
	}
      }
    }
  
    if( daughter ) {
      // printf( "sprite daughter\n" );
      daughter->display( mode , true , storeCurrentMatrix );
    }
  }
  // displayed node
  
  if( saveEnvironment
      && type != ATextureStorage
      && type != APixelReader
      && type != AFBOStorage
      && type != AFBDisplay ) {
    restoreEnvironment( mode );
  }
  
  if( sister ) {
    // printf( "sprite sister\n" );
    sister->display( mode , true , storeCurrentMatrix );
  }
}

int dm_SceneNode::findAllNodesInTree( char searchedId[XMLAttrValLength] ,
				      dm_SceneNode **tabNodes ,
				      int *nbNodes , 
				      bool cutIfFound , bool justCount , bool callOnSisters ) {
//   printf( "Cur node [%s]/[%s] type %s\n" , id , searchedId , 
//              NodeTypeString[type] );
	
  if( type != ANodeLink 
      && ( stringEqual( searchedId , id ) ) ) {
    if( !justCount ) {
       //printf( "Cur node [%s]/[%s] type %s %ld\n" , id , searchedId , 
 	//      NodeTypeString[type] , this );
      tabNodes[ *nbNodes ] = this;
    }
    (*nbNodes)++;
  }

  if( cutIfFound && (*nbNodes) ) {
    return (*nbNodes);
  }
  
  if( type == ANodeLink ) {
    // separating file name
    if( strchr( id , ':' ) ) {
      char fileName[XMLAttrValLength];
      strcpy( fileName , id );
      char *endOfFileName =  strchr( fileName , ':' );
      *endOfFileName = 0;
      
      //printf( "FindScene (%s)\n" , fileName );
      dm_Scene *scene = NULL;
      for( int ind = 0 ; ind < DM_CompositionNode->NbScenes ; ind++ ) {
	// printf( "ind %d scene name (%s) file name %s \n" , ind , DM_CompositionNode->scene_names[ind] , fileName );
	if( DM_CompositionNode->scene_names[ind] 
	    && strcmp( DM_CompositionNode->scene_names[ind] , fileName ) == 0 ) {
	  if( DM_CompositionNode->scenes[ind] ) {
	    scene = DM_CompositionNode->scenes[ind];
	  }
	}
      }
      
      if( scene && scene->root ) {
	scene->root->findAllNodesInTree( searchedId , tabNodes , nbNodes , 
					 cutIfFound , justCount , true );
	if( cutIfFound && (*nbNodes) ) {
	  return (*nbNodes);
	}
      }
    }
  }

  if( daughter ) {
    daughter->findAllNodesInTree( searchedId , tabNodes , nbNodes , 
				  cutIfFound , justCount , true );
    if( cutIfFound && (*nbNodes) ) {
      return (*nbNodes);
    }
  }

  if( sister && callOnSisters ) {
	dm_SceneNode *sisterNode = sister;
	while( sisterNode ) {
		sisterNode->findAllNodesInTree( searchedId , tabNodes , nbNodes , 
				  cutIfFound , justCount , false );
		if( cutIfFound && (*nbNodes) ) {
		return (*nbNodes);
		}
		sisterNode = sisterNode->sister;
	}
  }
  return (*nbNodes);
}


void dm_SceneNode::recursiveCopy( dm_SceneNode *auxNode , int depth ) {
  // temporary storage of the type
  // in case of dm_Leaf Node, only content is co-referenced
  // the nodes receives a temporary type ANodeLink
  // so that the = operator only copies the content
  // of the dm_SceneNode attributes except the content of the dm_SceneObject 
  NodeType  curType = auxNode->type;
  auxNode->type = ANodeLink;

  // at depth 0 (root node, copy does not concern the sister node
  dm_SceneNode * sis = sister;

  // copies all the attributes of the current dm_SceneNode
  // sceneObject attributes is NULL because of type ANodeLink
  *this = *auxNode;

  // pointer sharing for sceneObject component

  sceneObject = auxNode->sceneObject;

  // restores types
  auxNode->type = curType;
  type = curType;
  
  // copies daughter node
  if( auxNode->daughter ) {
    daughter = new dm_SceneNode( ANodeLink );
    daughter->recursiveCopy( auxNode->daughter , depth + 1 );
  }
  else {
    daughter = NULL;
  }

  // at the root level, sister node is not copied
  // sisterhood is preserved from original link node
  if( depth == 0 ) {
    sister = sis;
  }
  else {
    if( auxNode->sister ) {
      sister = new dm_SceneNode( ANodeLink );
      sister->recursiveCopy( auxNode->sister , depth + 1 );
    }
    else {
      sister = NULL;
    }
  }
}


void dm_SceneNode::resolveReferences( dm_SceneNode *rootNode ) {
  dm_SceneNode **auxNode;
  int nbRefs = 0;
  
  // printf( "resolveReferences (%s) %d\n" , id , GetMaterial());
  dm_MaterialProperties *mat = NULL;
  if( (mat = GetMaterial()) && mat->materialTexture ) {
    mat->materialTexture->ResolveReferences( rootNode );
    // printf( "ResolveReferences (%s) %d\n" , id , 
    //    mat->materialTexture->GetTextureNo() );
  }
  dm_Shader *shader = GetMaterialShader();
  if( shader ) {
    // printf( "get shader video [%s]\n" , id );
    if( shader->shaderSource ) {
      for( int textureRank = 0 ; 
	   textureRank < shader->shaderSource->nb_shaderTextures ; 
	   textureRank++ ) {
	if( shader->shaderSource->materialTextures[ textureRank ] ) {
	  // printf( "Shader resolveReferences rank %d (%s)\n" , textureRank , id );
	  shader->shaderSource
	    ->materialTextures[ textureRank ]
	    ->ResolveReferences( rootNode );
	}
      }
    }
  }

  // resolving node references (<use xlink:href="...">)
  if( type == ANodeLink ) {
    nbRefs = 0;
    auxNode = findAllReferences( rootNode , id ,
				 &nbRefs , true , true );
    
    if( auxNode ) {
      //printf( "Daughter node [%d] \n" , auxNode->daughter );
      //auxNode->print( stdout );
      
      // the current version makes a recursive copy of the scene graph
      // and only shares the geometrical part of the terminal nodes
      // A future version could rely on a table tabGeometricalObjects
      // that contains 
      // 1) the pointer to the geometrical part of the leaf nodes
      // 2) a scene node with autonomous ModelViewMatrix, 
      recursiveCopy( auxNode[0] , 0 );
      
      // only sister node is recursively resolved
      // in order to avoid infinite loop
      if( sister ) {
	sister->resolveReferences( rootNode );
      }
      
      //printf( "Link resolved \n" );
      delete [] auxNode;
	  auxNode = NULL;
    }
  }

  // TEXTURE STORAGE:  frame buffer texture storage 
  else if( type == ATextureStorage && sceneObject ) {
    dm_TextureStorage * aTextureStorage = (dm_TextureStorage *)sceneObject;

    // resolves the reference of the user node
    if( aTextureStorage->TextureStorageScalarNode 
	&& aTextureStorage->TextureStorageScalarNode->type == ANodeLink ) {
      nbRefs = 0;
      auxNode = findAllReferences( rootNode , aTextureStorage->TextureStorageScalarNode->id ,
				   &nbRefs , true , true );
      if( !auxNode || nbRefs <= 0 ) {
	sprintf( ErrorStr , "Error: TextureStorageScalar node reference of texture storage %s not found\n" , aTextureStorage->TextureStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
      }
      if( nbRefs <= 0 || nbRefs >= 2 ) {
	sprintf( ErrorStr , "Error: more than one TextureStorageScalar node reference %s found\n" , aTextureStorage->TextureStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
      }
      
      delete aTextureStorage->TextureStorageScalarNode;
      aTextureStorage->TextureStorageScalarNode = NULL;

      aTextureStorage->TextureStorageScalarNode = auxNode[0];
      delete [] auxNode;
      auxNode = NULL;

      // printf( "TextureStorageScalar node found %s\n" , aTextureStorage->TextureStorageScalarNode->id );
    }

    // resolves the reference of the TextureStorageScalar variable
    if( aTextureStorage->GetId() ) {
      // searches among the variables
      // TextureStorageScalar variables
      aTextureStorage->TextureStorageScalar 
	= aTextureStorage->TextureStorageScalarNode->getScalarFromName( aTextureStorage->GetId() );
      if( aTextureStorage->TextureStorageScalar ) {
	// printf( "TextureStorageScalar found %s\n" , aTextureStorage->GetId() );
	if( aTextureStorage->TextureStorageScalar->GetType() != DataTypeTexture ) {
	  sprintf( ErrorStr , "Error: non texture TextureStorageScalar %s found on node %s\n" , aTextureStorage->GetId() , aTextureStorage->TextureStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
	}
      }
      // printf( "texture TextureStorageScalar found %s\n" , aTextureStorage->TextureStorageScalar->id );
    }
  }

  // FBO STORAGE:  frame buffer object texture storage 
  else if( type == AFBOStorage && sceneObject ) {
    dm_FBOStorage * aFBOStorage = (dm_FBOStorage *)sceneObject;

    // resolves the reference of the user node
    if( aFBOStorage->FBOStorageScalarNode 
	&& aFBOStorage->FBOStorageScalarNode->type == ANodeLink ) {
      nbRefs = 0;
      auxNode = findAllReferences( rootNode , aFBOStorage->FBOStorageScalarNode->id ,
				   &nbRefs , true , true );
      if( !auxNode || nbRefs <= 0 ) {
	sprintf( ErrorStr , "Error: FBOStorageScalar node reference of texture storage %s not found\n" , aFBOStorage->FBOStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
      }
      if( nbRefs <= 0 || nbRefs >= 2 ) {
	sprintf( ErrorStr , "Error: more than one FBOStorageScalar node reference %s found\n" , aFBOStorage->FBOStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
      }
      
      delete aFBOStorage->FBOStorageScalarNode;
      aFBOStorage->FBOStorageScalarNode = NULL;

      aFBOStorage->FBOStorageScalarNode = auxNode[0];
      delete [] auxNode;
      auxNode = NULL;

      // printf( "FBOStorageScalar node found %s\n" , aFBOStorage->FBOStorageScalarNode->id );
    }

    // resolves the reference of the TextureStorageScalar variables
    if( *(aFBOStorage->color_id) ) {
      // searches among the variables
      // FBOStorageScalar variables
      aFBOStorage->color_FBOStorageScalar 
	= aFBOStorage->FBOStorageScalarNode->getScalarFromName( aFBOStorage->color_id );
      if( aFBOStorage->color_FBOStorageScalar ) {
	// printf( "FBOStorageScalar found %s\n" , aFBOStorage->GetId() );
	if( aFBOStorage->color_FBOStorageScalar->GetType() != DataTypeTexture ) {
	  sprintf( ErrorStr , "Error: non texture color_FBOStorageScalar %s found on node %s\n" , aFBOStorage->GetId() , aFBOStorage->FBOStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
	}
      }
      // printf( "texture FBOStorageScalar found %s\n" , aFBOStorage->FBOStorageScalar->id );
    }
    // resolves the reference of the TextureStorageScalar variables
    if( *(aFBOStorage->depth_id) ) {
      // searches among the variables
      // FBOStorageScalar variables
      aFBOStorage->depth_FBOStorageScalar 
	= aFBOStorage->FBOStorageScalarNode->getScalarFromName( aFBOStorage->depth_id );
      if( aFBOStorage->depth_FBOStorageScalar ) {
	// printf( "FBOStorageScalar found %s\n" , aFBOStorage->GetId() );
	if( aFBOStorage->depth_FBOStorageScalar->GetType() != DataTypeTexture ) {
	  sprintf( ErrorStr , "Error: non texture depth_FBOStorageScalar %s found on node %s\n" , aFBOStorage->GetId() , aFBOStorage->FBOStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
	}
      }
      // printf( "texture FBOStorageScalar found %s\n" , aFBOStorage->FBOStorageScalar->id );
    }
    // resolves the reference of the TextureStorageScalar variables
    if( *(aFBOStorage->stencil_id) ) {
      // searches among the variables
      // FBOStorageScalar variables
      aFBOStorage->stencil_FBOStorageScalar 
	= aFBOStorage->FBOStorageScalarNode->getScalarFromName( aFBOStorage->stencil_id );
      if( aFBOStorage->stencil_FBOStorageScalar ) {
	// printf( "FBOStorageScalar found %s\n" , aFBOStorage->GetId() );
	if( aFBOStorage->stencil_FBOStorageScalar->GetType() != DataTypeTexture ) {
	  sprintf( ErrorStr , "Error: non texture stencil_FBOStorageScalar %s found on node %s\n" , aFBOStorage->GetId() , aFBOStorage->FBOStorageScalarNode->id ); ReportError( ErrorStr ); throw 139;
	}
      }
      // printf( "texture FBOStorageScalar found %s\n" , aFBOStorage->FBOStorageScalar->id );
    }
  }
  
  // FB DISPLAY:  frame buffer object texture storage end
  else if( type == AFBDisplay && sceneObject ) {
    // nothing to do here....
  }
  
  // Pixel Reader:  frame buffer or frame buffer object lookup
  else if( type == APixelReader && sceneObject ) {
    // nothing to do here....
  }
  
  // USER:  default viewpoints reference resolution
  else if( type == AUser && sceneObject ) {
    dm_User * aUser = (dm_User *)sceneObject;
    if( aUser->default_viewpoint_node 
	&& aUser->default_viewpoint_node->type == ANodeLink ) {
      nbRefs = 0;
      auxNode = findAllReferences( rootNode , 
				   aUser->default_viewpoint_node->id ,
				   &nbRefs , true , true );
      if( !auxNode || nbRefs <= 0 ) {
	sprintf( ErrorStr , "Error: user default viewpoint reference %s not found\n" , aUser->default_viewpoint_node->id ); ReportError( ErrorStr ); throw 139;
      }
      if( nbRefs <= 0 || nbRefs >= 2 ) {
	sprintf( ErrorStr , "Error: more than one user default viewpoint reference %s found\n" , aUser->default_viewpoint_node->id ); ReportError( ErrorStr ); throw 139;
      }
      
      delete aUser->default_viewpoint_node;
	  aUser->default_viewpoint_node = NULL;

      aUser->default_viewpoint_node = auxNode[0];
      delete [] auxNode;
	  auxNode = NULL;

    }
    if( aUser->default_viewpoint_stereo_left_node 
	&& aUser->default_viewpoint_stereo_left_node->type == ANodeLink ) {
      nbRefs = 0;
      auxNode = findAllReferences( rootNode , 
				   aUser->default_viewpoint_stereo_left_node->id ,
				   &nbRefs , true , true );
      if( !auxNode || nbRefs <= 0 ) {
	sprintf( ErrorStr , "Error: user default viewpoint reference %s not found\n" , aUser->default_viewpoint_stereo_left_node->id ); ReportError( ErrorStr ); throw 139;
      }
      if( nbRefs <= 0 || nbRefs >= 2 ) {
	sprintf( ErrorStr , "Error: more than one user default viewpoint reference %s found\n" , aUser->default_viewpoint_stereo_left_node->id ); ReportError( ErrorStr ); throw 139;
      }
      
      delete aUser->default_viewpoint_stereo_left_node;
	  aUser->default_viewpoint_stereo_left_node = NULL;

      aUser->default_viewpoint_stereo_left_node = auxNode[0];
      delete [] auxNode;
	  auxNode = NULL;
    }
  }
  
  //printf( "daughter\n" );
  if( daughter ) {
    daughter->resolveReferences( rootNode );
  }

  //printf( "sister\n" );
  if( sister ) {
    sister->resolveReferences( rootNode );
  }
  //printf( "end of resolve references\n" );
}


dm_SceneNode* dm_SceneNode::nthNode( int rank ) {
  dm_SceneNode* returnNode = this;
  for( int ind = 0 ; ind < rank ; ind++ ) {
    if( returnNode->sister ) {
      returnNode = returnNode->sister;
    }
  }
  return returnNode;
}

void dm_SceneNode::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<node id=\"%s\" displayed=\"%s\" depth_test=\"%s\"" , 
	   id , BooleanString[BooleanToInt(displayed)] , BooleanString[BooleanToInt(depth_test)] );
  if( userString_exclusion_display ) {
    fprintf( file , "user_exclusion_display=\"%s\" " , 
	     userString_exclusion_display );
  }
  fprintf( file , ">\n" );

  dm_Scalar *varAux = lisScalars;
  while( varAux ) {
    varAux->print( file , depth + 1 );
    varAux = (dm_Scalar *)varAux->nextVariable;
  }

  dm_Table *tabAux = lisTables;
  while( tabAux ) {
    tabAux->print( file , depth + 1 );
    tabAux = (dm_Table *)tabAux->nextVariable;
  }

  dm_Matrix *matAux = lisMatrices;
  while( matAux ) {
    matAux->print( file , depth + 1 );
    matAux = (dm_Matrix *)matAux->nextVariable;
  }

  //fprintf( file , "pteur %d\n" , this );
  if( script ) {
    script->print( file , isLong , depth + 1 );
  }
  if( renderingCommand ) {
    indent( file , depth + 1 );
    fprintf( file , "<renderingCommand>\n" );
    dm_Command * synchronous 
      = renderingCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->print( file , true , depth + 2 );
      synchronous = synchronous->synchronousCommand;
    }
    indent( file , depth );
    fprintf( file , "</renderingCommand>\n" );
  }

  if( DM_CompositionNode
      && DM_CompositionNode->drawn_scenes[0]
      && this == DM_CompositionNode->drawn_scenes[0]->root 
      && DM_CompositionNode->initializationCommand ) {
    if( DM_CompositionNode->initializationCommand ) {
      indent( file , depth + 1 );
      fprintf( file , "<initializationScript>\n" );
      DM_CompositionNode->initializationCommand->print( file , true , depth + 2 );
      dm_Command * synchronous 
	= DM_CompositionNode->initializationCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->print( file , true , depth + 2 );
	synchronous = synchronous->synchronousCommand;
      }
      indent( file , depth );
      fprintf( file , "</initializationScript>\n" );
    }
  }
  if( DM_CompositionNode
      && DM_CompositionNode->drawn_scenes[0]
      && this == DM_CompositionNode->drawn_scenes[0]->root 
      && DM_CompositionNode->terminationCommand ) {
    if( DM_CompositionNode->terminationCommand ) {
      indent( file , depth + 1 );
      fprintf( file , "<terminationScript>\n" );
      DM_CompositionNode->terminationCommand->print( file , true , depth + 2 );
      dm_Command * synchronous
		  = DM_CompositionNode->terminationCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->print( file , true , depth + 2 );
	synchronous = synchronous->synchronousCommand;
      }
      indent( file , depth );
      fprintf( file , "</terminationScript>\n" );
    }
  }
  if( DM_CompositionNode
      && DM_CompositionNode->drawn_scenes[0]
      && this == DM_CompositionNode->drawn_scenes[0]->root 
      && DM_CompositionNode->aliasCommand ) {
    if( DM_CompositionNode->aliasCommand ) {
      dm_Command    *commandsAliases = DM_CompositionNode->aliasCommand;
      indent( file , depth + 1 );
      fprintf( file , "<aliasScript>\n" );
      while( commandsAliases ) {
	commandsAliases->print( file , true , depth + 2 );
	dm_Command * synchronous 
	  = commandsAliases->synchronousCommand;
	while( synchronous ) {
	  synchronous->print( file , true , depth + 2 );
	  synchronous = synchronous->synchronousCommand;
	}
	commandsAliases = commandsAliases->nextCommand;
      }
      indent( file , depth );
      fprintf( file , "</aliasScript>\n" );
    }
  }

  switch( type ) {
    // GEOMETRICAL OBJECTS
  case AMesh:
  case APoints:
  case AQuad:
  case APoint:
  case ALine:
  case AStrip:

    // scne OBJECTS
  case ATransformation:
  case AViewpoint:
    // viewpoints are not drawn but their current model view matrix is stored
  case ATextureStorage:
  case APixelReader:
  case AFBOStorage:
  case AFBDisplay:
  case AUser:
  case AEnvironment:
    // virtual function called according to the actual 
    // type of the pointed object
    sceneObject->print( file , isLong , depth );
    break;

  case ANodeLink:
    indent( file , depth );
    fprintf( file , "<use xlink:href=\"%s\" />\n" , id );
    break;
  case ANode:
    // do nothing
    break;
  case EmptyNode:
    break; 
  }

  if( daughter ) {
    daughter->print( file , isLong , depth + 2 );
  }
  indent( file , depth );
  fprintf( file , "</node>\n" );
  if( sister ) {
    sister->print( file , isLong , depth );
  }
}

void dm_SceneNode::printComponent( FILE *file , int isLong , int depth ) {
  sceneObject->print( file , isLong , depth + 1 );
  indent( file , depth );
  fprintf( file , "</node>\n" );
}

void dm_SceneNode::forceUpdate( bool to_invalidate ) {
  if( to_invalidate ) {
    // printf("Invalidate node %s\n" , id );
    invalidate();
  }
}

void dm_SceneNode::update( bool hidden ) {
  // update interpolated variables
  dm_Scalar *varAux = lisScalars;
  while( varAux ) {
    if( varAux->interp ) {
      bool  forcedInterpolatorUpdate = varAux->interp->getForceUpdate();
      varAux->interp->setForceUpdate( false );
      varAux->interp->update( forcedInterpolatorUpdate , dm_ScalarType );
    }
    varAux = (dm_Scalar *)varAux->nextVariable;
  }

  dm_Table *tabAux = lisTables;
  while( tabAux ) {
    if( tabAux->interp ) {
      bool  forcedInterpolatorUpdate = tabAux->interp->getForceUpdate();
      tabAux->interp->setForceUpdate( false );
      tabAux->interp->update( forcedInterpolatorUpdate , dm_TableType );
    }
    tabAux = (dm_Table *)tabAux->nextVariable;
  }

  dm_Matrix *matAux = lisMatrices;
  while( matAux ) {
    if( matAux->interp ) {
      bool  forcedInterpolatorUpdate = matAux->interp->getForceUpdate();
      matAux->interp->setForceUpdate( false );
      matAux->interp->update( forcedInterpolatorUpdate , dm_MatrixType );
    }
    matAux = (dm_Matrix *)matAux->nextVariable;
  }

  // non displayed nodes must be updated in
  // case they are awaken (eg scheduler filled with hide that is restarted
  // if( displayed ) 
  // updates properties
  if( IsGeometricalObject() ) {
    // printf( "update object [%s/%s]\n" , id , NodeTypeString[type] );
    // material update (material display must be postponed
    // to the post display phase because it applies to a single object
    if( !hidden && displayed ) {
      // printf( "update object [%s/%s] %d %d\n" , id , NodeTypeString[type] , hidden , displayed );
      // updates the material properties
      // printf( "update material [%s]\n" , id );
      GetMaterial()->update();
      
      dm_Shader *shader = GetMaterialShader();
      if( shader ) {
	// printf( "update shader [%s]\n" , id );
	shader->update();
      }
    }
  }

  if( daughter ) {
    daughter->update( hidden || !displayed );
  }
  if( sister ) {
    sister->update( hidden );
  }
}

void dm_SceneNode::RegisterViewpoints( int *indViewpoints ,
				       int nbMaxViewpoints ,
				       dm_Viewpoint **tabViewpoints ,
				       dm_SceneNode **tabViewpointsNodes) {
  if( type == AViewpoint ) {
    if( (*indViewpoints) >= nbMaxViewpoints ) {
      sprintf( ErrorStr , "Error: excessive number of viewpoints in scene!" ); ReportError( ErrorStr ); throw 139;
    }
    tabViewpoints[*indViewpoints] = (dm_Viewpoint *)sceneObject;
    tabViewpointsNodes[*indViewpoints] = this;
    (*indViewpoints)++;
  }

  if( daughter ) {
    daughter->RegisterViewpoints( indViewpoints , nbMaxViewpoints ,
				  tabViewpoints , tabViewpointsNodes );
  }
  
  if( sister ) {
    sister->RegisterViewpoints( indViewpoints , nbMaxViewpoints ,
				tabViewpoints , tabViewpointsNodes );
  }
}

void dm_SceneNode::deleteRegisterViewpoints( int *indViewpoints ,
				       int nbMaxViewpoints ,
				       dm_Viewpoint **tabViewpoints ,
				       dm_SceneNode **tabViewpointsNodes) {
  if( type == AViewpoint ) {
	for (int ind = 0; ind < *indViewpoints; ind++)
	{
		// if one target is found, delete it
		if (this == tabViewpointsNodes[ind])
		{
			for (int i = ind + 1; i < *indViewpoints; i++)
			{
				tabViewpoints[i - 1] = tabViewpoints[i];
				tabViewpointsNodes[i - 1] = tabViewpointsNodes[i];
			}
			(*indViewpoints) --;
		}
	}
  }

  if( daughter ) {
    daughter->RegisterViewpoints( indViewpoints , nbMaxViewpoints ,
				  tabViewpoints , tabViewpointsNodes );
  }
  
  if( sister ) {
    sister->RegisterViewpoints( indViewpoints , nbMaxViewpoints ,
				tabViewpoints , tabViewpointsNodes );
  }
}

void dm_SceneNode::RecursiveCall( void (*pt2Func)(int * , dm_SceneNode *) , 
				  int *counter ) {
  (*pt2Func)( counter , this );

  if( daughter ) {
    daughter->RecursiveCall( pt2Func , counter );
  }
  if( sister ) {
    sister->RecursiveCall( pt2Func , counter );
  }
}

void SampleRecursivelyCalledFunction( int *counter , dm_SceneNode *obj ) {
  if( obj->type == AViewpoint ) {
    (*counter)++;
  }
}

void dm_SceneNode::CountViewpoints( int *nbMaxViewpoints ) {
  if( type == AViewpoint ) {
    (*nbMaxViewpoints)++;
  }

  if( daughter ) {
    daughter->CountViewpoints( nbMaxViewpoints );
  }
  if( sister ) {
    sister->CountViewpoints( nbMaxViewpoints );
  }
}

void dm_SceneNode::RegisterUsers( int *indUser , 
			       int nbMaxUsers ,
			       dm_SceneNode **tabUsers ) {
  if( type == AUser ) {
    if( (*indUser) >= nbMaxUsers ) {
      sprintf( ErrorStr , "Error: excessive number of users in scene (%d)!" , (*indUser) ); ReportError( ErrorStr ); throw 139;
    }
    tabUsers[*indUser] = this;
    (*indUser)++;
  }

  if( daughter ) {
    daughter->RegisterUsers( indUser , nbMaxUsers , tabUsers );
  }
  
  if( sister ) {
    sister->RegisterUsers( indUser , nbMaxUsers , tabUsers  );
  }
}

void dm_SceneNode::deleteRegisterUsers( int *indUser , 
			       int nbMaxUsers ,
			       dm_SceneNode **tabUsers ) {
  if( type == AUser ) {
	for (int ind = 0; ind < *indUser; ind++)
	{
		// if one target is found, delete it
		if (this == tabUsers[ind])
		{
			for (int i = ind + 1; i < *indUser; i++)
			{
				tabUsers[i - 1] = tabUsers[i];
			}
			(*indUser) --;
		}
	}
  }

  if( daughter ) {
    daughter->RegisterUsers( indUser , nbMaxUsers , tabUsers );
  }
  
  if( sister ) {
    sister->RegisterUsers( indUser , nbMaxUsers , tabUsers  );
  }
}

void dm_SceneNode::CountUsers( int *nbMaxUsers ) {
  if( type == AUser ) {
    (*nbMaxUsers)++;
  }

  if( daughter ) {
    daughter->CountUsers( nbMaxUsers );
  }
  if( sister ) {
    sister->CountUsers( nbMaxUsers );
  }
}



void dm_SceneNode::ComputeUserExclusionDisplay( FloatBinOp theOperator ) {

  // operators: assign , add , sub

  // allocates the boolean table that will contain the boolean
  // values (ind value true ==  the node is not displayed for user #ind
  if( !user_exclusion_display ) {

    user_exclusion_display
      = new bool[ DM_CompositionNode->nbRegisteredUsers ];
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      user_exclusion_display[ ind ] = false;
    }
  }

  // allocates the corresponding table of floats that will be used
  // to print the value of the tag
  if( !userFloatTable_exclusion_display ) {
    userFloatTable_exclusion_display
      = new dm_Table( DM_CompositionNode->nbRegisteredUsers );
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      userFloatTable_exclusion_display->SetValue( 0.0 , ind );
    }
  }

  // assign: clears the boolean values
  if( theOperator == assign ) {
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      userFloatTable_exclusion_display->SetValue( 0.0 , ind );
      user_exclusion_display[ ind ] = false;
    }
  }

  // assign or add: replaces the non null boolean values by true
  if( theOperator == assign || theOperator == add ) {
    // excluded users expressed as a table of boolean values
    // excluded users expressed as a string (possibly containing joker char)
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      if( stringEqual( userString_exclusion_display , 
		       DM_CompositionNode->tabUsers[ ind ]->id ) 
	  // current value
	  || (bool)userFloatTable_exclusion_display->GetNumericalValue( ind )
	  // preceding value
	  ) {
	
	userFloatTable_exclusion_display->SetValue( 1.0 , ind );
	user_exclusion_display[ ind ] = true;
      }
    }
  }

  else if( theOperator == sub ) {
    // excluded users expressed as a string (possibly containing joker char)
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      if( stringEqual( userString_exclusion_display , 
		       DM_CompositionNode->tabUsers[ ind ]->id )
	  // current value
	  && (bool)userFloatTable_exclusion_display->GetNumericalValue( ind )
	  // preceding value
	  ) {
	
	userFloatTable_exclusion_display->SetValue( 0.0 , ind );
	user_exclusion_display[ ind ] = false;
      }
    }
  }
  else {
    sprintf( ErrorStr , "Error: *= and /= operators do not operate on \"user_exclusion_display attribute\"!" ); ReportError( ErrorStr );
  }

//   printf( "user_exclusion_display: " );
//   for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
//        ind++ ) {
//     printf( "%d " , user_exclusion_display[ ind ] );
//   }
//   printf( "\n" );
}

void dm_SceneNode::RegisterUserExclusionDisplay( void ) {
  // resolving user references for the user-based non displayed nodes
  if( userString_exclusion_display ) {
    ComputeUserExclusionDisplay( assign );
  }

  if( daughter ) {
    daughter->RegisterUserExclusionDisplay();
  }
  if( sister ) {
    sister->RegisterUserExclusionDisplay();
  }
}

void dm_SceneNode::CountGeometricalObjects( int *nbMaxGeometricalObject ) {
  dm_Transformation * transf = NULL;
    
  if( IsGeometricalObject()
      || ( (transf = getTransformationNode() ) 
	   && transf->hasDrawablePrimitive() ) ) {
    (*nbMaxGeometricalObject)++;
  }

  if( daughter ) {
    daughter->CountGeometricalObjects( nbMaxGeometricalObject );
  }
  if( sister ) {
    sister->CountGeometricalObjects( nbMaxGeometricalObject );
  }
}

void dm_SceneNode::CopyLocalVariables(dm_SceneNode * n) {
  displayed = n->displayed;
  depth_test = n->depth_test;
  script = n->script;
  renderingCommand = n->renderingCommand;

  /*
  if (lisScalars)
  {
	  delete lisScalars;
	  lisScalars = NULL;
  }

  if (n->lisScalars)
  {
	  lisScalars = new dm_Scalar;
	  *lisScalars = *(n->lisScalars);
  }
*/
  lisScalars = n->lisScalars;
  lisTables = n->lisTables;
  lisMatrices = n->lisMatrices;
}

void dm_SceneNode::operator=(dm_SceneNode& n) {
  StrCpy( &id , n.id );
  StrCpy( &internalState , n.internalState );
  script = n.script;
  renderingCommand = n.renderingCommand;
  daughter = n.daughter;
  sister = n.sister;
  type = n.type;

  // properties
  lisScalars = n.lisScalars;
  // scalar tables used in expressions or controls
  lisTables = n.lisTables;
  // scalar matrixs used in expressions or controls
  lisMatrices = n.lisMatrices;

  // visibility
  displayed = n.displayed;
  if( n.user_exclusion_display ) {
    delete []n.user_exclusion_display;
    
    user_exclusion_display
      = new bool[ DM_CompositionNode->nbRegisteredUsers ];
    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; 
	 ind++ ) {
      user_exclusion_display[ ind ] = n.user_exclusion_display[ind];
    }
  }  
  
  StrCpy(&userString_exclusion_display, n.userString_exclusion_display);
  userFloatTable_exclusion_display = n.userFloatTable_exclusion_display;
//  if( n.userFloatTable_exclusion_display ) {
//	delete userFloatTable_exclusion_display;
//      userFloatTable_exclusion_display
//	= new dm_Table( DM_CompositionNode->nbRegisteredUsers );
//  }
//  for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; ind++ ) {
//	  userFloatTable_exclusion_display
//		  ->SetValue( (double)user_exclusion_display[ind] , ind );
//  }

  // depth testing
  depth_test = n.depth_test;

  // invalidation
  is_invalid = n.is_invalid;

  modelViewMatrixIndex = -1;

  if( n.modelViewMatrixIndex >= 0 ) {
    // because it corresponds to a pointer in a table
    // otherwise all the copied objects must have the
    // same mmview matrix
    
    // adds a new index in the multithread storage of modelview matrix
    modelViewMatrixIndex = DM_CompositionNode->AddModelViewMatrixPt();
//     printf("AddModelViewMatrixPt (2) %s %d\n" , id ,modelViewMatrixIndex);
    // associates this index with a pointer 
    DM_CompositionNode->SetModelViewMatrixPt( modelViewMatrixIndex , 
				       (long int)new dm_matrix44() );

    //   // associates this index with the same pointer as n
    //   DM_CompositionNode->SetModelViewMatrixPt( modelViewMatrixIndex , 
    // 		  DM_CompositionNode->GetModelViewMatrixPt( n.modelViewMatrixIndex ) );
  }

  valScalarModelViewMatrix = n.valScalarModelViewMatrix;

  // the only case of incompatible should be between a link node 
  // and another node
  if( type != EmptyNode && type != n.type ) {
    // printf( "equality between incompatible nodes %s/%s!",NodeTypeString[type], NodeTypeString[n.type] );
    sceneObject = NULL;
  }

  switch( type ) {
  case AMesh : {
    dm_Mesh * aMesh = (dm_Mesh *)sceneObject;
    if( !aMesh ) {
      aMesh = new dm_Mesh();
    }
    *aMesh = *((dm_Mesh *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aMesh;
  } break;
  case APoints : {
    dm_Points * aPoints = (dm_Points *)sceneObject;
    if( !aPoints ) {
      aPoints = new dm_Points();
    }
    *aPoints = *((dm_Points *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aPoints;
  } break;
  case AQuad : {
    dm_Quad * aQuad = (dm_Quad *)sceneObject;
    if( !aQuad ) {
      aQuad = new dm_Quad();
    }
    *aQuad = *((dm_Quad *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aQuad;
  } break;
  case APoint : {
    dm_Point * aPoint = (dm_Point *)sceneObject;
    if( !aPoint ) {
      aPoint = new dm_Point();
    }
    *aPoint = *((dm_Point *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aPoint;
  } break;
  case ALine : {
    dm_Line * aLine = (dm_Line *)sceneObject;
    if( !aLine ) {
      aLine = new dm_Line();
    }
    *aLine = *((dm_Line *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aLine;
  } break;
  case AStrip : {
    dm_Strip * aStrip = (dm_Strip *)sceneObject;
    if( !aStrip ) {
      aStrip = new dm_Strip();
    }
    *aStrip = *((dm_Strip *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aStrip;
  } break;
  case ATransformation : {
    dm_Transformation * aTransformation = (dm_Transformation *)sceneObject;
    if( !aTransformation ) {
      aTransformation = new dm_Transformation();
    }
    *aTransformation = *((dm_Transformation *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aTransformation;
  } break;
  case AEnvironment : {
    dm_Environment * aEnvironment = (dm_Environment *)sceneObject;
    if( !aEnvironment ) {
      aEnvironment = new dm_Environment();
    }
    *aEnvironment = *((dm_Environment *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aEnvironment;
  } break;
  case AViewpoint : {
    dm_Viewpoint * aViewpoint = (dm_Viewpoint *)sceneObject;
    if( !aViewpoint ) {
      aViewpoint = new dm_Viewpoint();
    }
    *aViewpoint = *((dm_Viewpoint *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aViewpoint;
  } break;
  case ATextureStorage : {
    dm_TextureStorage * aTextureStorage = (dm_TextureStorage *)sceneObject;
    if( !aTextureStorage ) {
      aTextureStorage = new dm_TextureStorage();
    }
    *aTextureStorage = *((dm_TextureStorage *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aTextureStorage;
  } break;
  case APixelReader : {
    dm_PixelReader * aPixelReader = (dm_PixelReader *)sceneObject;
    if( !aPixelReader ) {
      aPixelReader = new dm_PixelReader();
    }
    *aPixelReader = *((dm_PixelReader *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aPixelReader;
  } break;
  case AFBOStorage : {
    dm_FBOStorage * aFBOStorage = (dm_FBOStorage *)sceneObject;
    if( !aFBOStorage ) {
      aFBOStorage = new dm_FBOStorage();
    }
    *aFBOStorage = *((dm_FBOStorage *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aFBOStorage;
  } break;
  case AFBDisplay : {
    dm_FBDisplay * aFBDisplay = (dm_FBDisplay *)sceneObject;
    if( !aFBDisplay ) {
      aFBDisplay = new dm_FBDisplay();
    }
    *aFBDisplay = *((dm_FBDisplay *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aFBDisplay;
  } break;
  case AUser : {
    dm_User * aUser = (dm_User *)sceneObject;
    if( !aUser ) {
      aUser = new dm_User();
    }
    *aUser = *((dm_User *)(n.sceneObject));
    sceneObject = (dm_SceneObject *)aUser;
  } break;
  case ANodeLink : {
    // nothing to do here
  } break;
  default:
    sprintf( ErrorStr , "Error: incorrect type for assignment[%s]!" , NodeTypeString[type] ); ReportError( ErrorStr ); throw 54;
  }
}

dm_Scene::dm_Scene( char *name ) {
  // --------- command aliases ----------- //
  root = NULL;

  parse( name );

}

dm_Scene::~dm_Scene(void)
{
  if (root) {
    delete root;
    root = NULL;
  }
}

void dm_Scene::referenceResolution( dm_SceneNode *rootScene ) {
  if( root ) {
    //printf( "resolving node references [%s]\n" , root->id);
    //if( strcmp( root->id , "main node" ) == 0 ) {
    //  root->print( stdout , true , 0 );
    //}

    // resolves the references by copying the links
    // must use the lowest root
    if( !rootScene ) {
      root->resolveReferences( root );
    }
    else {
      root->resolveReferences( rootScene );
    }
    //printf( "end of resolving node references [%s]\n" , root->id );

    //printf( "Percolation of (%s) (%d)\n" , root->id , root );
	/* add by Xianyong Fang 
    dm_SceneNode* nodePercolate = root->percolateInterpolation( &root , NULL , 2 );
	if ((nodePercolate != NULL) && (nodePercolate != root))
	{
		deleteTillNode(root, nodePercolate);
		root = nodePercolate;
	}
	*/

  }
}

void dm_Scene::deleteTillNode(dm_SceneNode* currentNode, dm_SceneNode* deleteEndNode)
{
	if ((deleteEndNode == NULL) || (currentNode == NULL)) return;

	if (currentNode != deleteEndNode)
	{
			delete currentNode;
	}
	else {		
		/////////// debug by Xianyong Fang ///////////////
		// int jjj = 1;
		/////////////// end of debug ///////////////
		return;
	}

}
int dm_Scene::transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode ) {
 if( root ) {
   return root->transformToPath( viewpoint , mode );
 }
 return false;
}

void dm_Scene::display( DrawingMode mode ) { 
 if( root ) {
	//printf ("root\n" );
    root->display( mode , true , true );
  }
}

void dm_Scene::print( FILE *file , int isLong ) {
  //fprintf( file , "scene\n" );
  if( root ) {
    //fprintf( file , "root\n" );
    root->print( file , isLong , 0 );
  }
}

void dm_Scene::update( void) {
  if( root ) {
    root->update( false );
  }
}

void dm_Scene::CountGeometricalObjects( int *nbMaxGeometricalObject ) {
  if( root ) {
    root->CountGeometricalObjects( nbMaxGeometricalObject );
  }
}

void dm_Scene::CountViewpoints( int *nbMaxViewpoint ) {
  if( root ) {
    root->CountViewpoints( nbMaxViewpoint );
//     root->RecursiveCall( SampleRecursivelyCalledFunction ,
// 			 nbMaxViewpoint );
  }
}
void dm_Scene::RegisterViewpoints( int *indViewpoint , 
				   int nbMaxViewpoints ,
				   dm_Viewpoint **tabViewpoint ,
				   dm_SceneNode **tabViewpointsNodes) {
  if( root ) {
    root->RegisterViewpoints( indViewpoint , nbMaxViewpoints , 
			       tabViewpoint , tabViewpointsNodes );
  }
}

void dm_Scene::CountUsers( int *nbMaxUser ) {
  if( root ) {
    root->CountUsers( nbMaxUser );
  }
}
void dm_Scene::RegisterUsers( int *indUser , 
			   int nbMaxUsers ,
			   dm_SceneNode **tabUsers ) {
  if( root ) {
    root->RegisterUsers( indUser , nbMaxUsers , 
			 tabUsers );
  }
}
void dm_Scene::RegisterUserExclusionDisplay( void ) {
  if( root ) {
    root->RegisterUserExclusionDisplay();
  }
}

void dm_Scene::parse( char *fileName ) {
  dm_SceneNode  *auxNode, *currentNode;
  int            curChar;

  dm_XMLTag     *XMLtag = new dm_XMLTag();
  bool           tagLoaded;

  root = NULL;

  int fileDepth = 0;
  for( int i = 0 ; i < DM_NB_MAX_EMBEDDED_XML_FILES ; i++ ) {
    fileXML[ i ] = NULL;
  }
  fileXML[ fileDepth ] = fopen( fileName , "rb" );

  if( !fileXML[ fileDepth ] ) {
    delete XMLtag;
    XMLtag = NULL;

    sprintf( ErrorStr , "Error: scene file [%s] not found!" , fileName ); ReportError( ErrorStr ); throw 11;
  }

  // reading the XML file
  curChar = ReadChar( &fileDepth , NULL , NULL);
  SpaceCommentsInclude( &fileDepth , &curChar , NULL , NULL );
  
  // reading the tag of a component
  tagLoaded = false;
  if( curChar != EOF ) {
    do {
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( &fileDepth , &curChar , true , NULL , NULL );
      if( *(XMLtag->tag) != '?' ) {
	tagLoaded = true;
      }
    } while( !tagLoaded );
  }

  while( curChar != EOF ) {
    // reading the tag of a component
    if( !tagLoaded ) {
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( &fileDepth , &curChar , true , NULL , NULL );
    }
    else {
      tagLoaded = false;
    }

    // global parameter
    if( strcmp( XMLtag->tag , "set" ) == 0 && XMLtag->tagType == EmptyTag ) {
      if( !DM_EnvironmentNode ) {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: environment must be defined prior to parameter definitions!" ); ReportError( ErrorStr ); throw 10;
      }

      // reading a set node
      int nbReturnedNodes;
      currentNode = ParseSet( &fileDepth , &curChar , 
			      XMLtag , &nbReturnedNodes );
    }

    // elementary component: the root node(s) for a scene file
    // elementary component: the environment node for a configuration file
    else if( strcmp( XMLtag->tag , "node" ) == 0 && XMLtag->tagType == OpeningTag ) {
      // printf( "parse node\n" );
      currentNode = ParseNode( &fileDepth , &curChar , false , XMLtag );

//       // regular scene description file
//       if( !isConfigurationFile ) {
	// node chaining
      if( !root ) {
	root = currentNode;
      }
      else {
	auxNode = root;
	while( auxNode->sister ) {
	  auxNode = auxNode->sister;
	}
	auxNode->sister = currentNode;
      }
//       }

    }

    else if( XMLtag->tagType == ClosingTag ) {
      sprintf( ErrorStr , "Error: unbalanced tag [%s] (excessive number of closing tags)!" , XMLtag->tag ); ReportError( ErrorStr ); 
      delete XMLtag;
      XMLtag = NULL;
      throw 62;
    }

    else {
      sprintf( ErrorStr , "Error: unknown root tag [%s] (node expected)!" , XMLtag->tag ); ReportError( ErrorStr ); 
      delete XMLtag;
      XMLtag = NULL;
      throw 62;throw 62;
    }
  }
  //printf("scene parsed\n" );

  // returned value
  delete XMLtag;
  XMLtag = NULL;

  fclose( fileXML[ fileDepth ] );
}

void dm_Scene::deleteGraph(dm_SceneNode* rootTargetGraph, 
						   dm_SceneNode* startNode,
						   int*nbDeletedNodes, 
						   dm_SceneNode** deletedNodeAddress,
						   int* nbDeletedAddressIndex)
{
	if (rootTargetGraph == NULL) return;

    char tempDataName[XMLAttrValLength];
    char* newDataName = NULL;
    newDataName = new char[XMLAttrValLength];
    strcpy( tempDataName , rootTargetGraph->id );
    StrCpyAndSubst(&newDataName, tempDataName);

	searchGraphAndDelete(newDataName, startNode, nbDeletedNodes, deletedNodeAddress, nbDeletedAddressIndex);

	if (newDataName) {
		delete [] newDataName;
	}
}

void dm_Scene::copyGraph(dm_SceneNode* sourceGraph, dm_SceneNode** targetGraph) {
	if (sourceGraph == NULL) return;

	*targetGraph = new dm_SceneNode;
	**targetGraph = *sourceGraph;	

	if (sourceGraph->daughter)
	{
		copyGraph(sourceGraph->daughter, &((*targetGraph)->daughter));
	}

	if (sourceGraph->sister)
	{
		copyGraph(sourceGraph->sister, &((*targetGraph)->sister));
	}
}


void dm_Scene::searchGraphAndDelete(char* targetID, 
						  dm_SceneNode* currentNode, 
						  int* nbDeletedNodes,
						  dm_SceneNode** deleteNodeAddress, 
						  int* nbDeletedAddressIndex)
{
	// no target is to be deleted
	if (targetID == NULL) return;

	// rearch the leaf of the scene graph
	if (currentNode == NULL) return;

	// search the sister node
	if (currentNode->sister)
	{
		if (strcmp(currentNode->sister->id, targetID) == 0) 
		{
			dm_SceneNode* tobedeletedNode = currentNode->sister;

			removeGraph(currentNode->sister, currentNode->sister->daughter, 
				nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
			currentNode->sister = currentNode->sister->sister;

			// add the to-be-deleted node to the header of deleted node list
			tobedeletedNode->sister = NULL;
			tobedeletedNode->daughter = deletedNodeList;
			deletedNodeList  = tobedeletedNode;

			// save the deleted node address for delete it in the knownTargets
			deleteNodeAddress[*nbDeletedAddressIndex] = tobedeletedNode;
			(*nbDeletedAddressIndex)++;

			(*nbDeletedNodes)++;

		}
		else
		{
			// continue search the scene graph in the sister node
			searchGraphAndDelete(targetID, currentNode->sister, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
	}

	// if the daughter has the same ID with the targetID, 
	// then delete it,, and move all the nodes of the daughter one level above
	if (currentNode->daughter != NULL) 
	{
		if ( strcmp(currentNode->daughter->id, targetID) == 0)
		{
			dm_SceneNode* tobedeletedNode = currentNode->daughter;

			removeGraph(currentNode->daughter, currentNode->daughter->daughter, 
				nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
			currentNode->daughter = currentNode->daughter->sister;

			// add the to-be-deleted node to the header of deleted node list
			tobedeletedNode->sister = NULL;
			tobedeletedNode->daughter = deletedNodeList;
			deletedNodeList  = tobedeletedNode;

			// save the deleted node address for delete it in the knownTargets
			deleteNodeAddress[*nbDeletedAddressIndex] = tobedeletedNode;
			(*nbDeletedAddressIndex)++;

			(*nbDeletedNodes)++;
		}
		else
		{
			// continue search and delete in the daugher node
			searchGraphAndDelete(targetID, currentNode->daughter, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
	}	
}

void dm_Scene::deleteNode(char* targetID, 
						  dm_SceneNode** currentNode, 
						  int* nbDeletedNodes,
						  dm_SceneNode** deleteNodeAddress, 
						  int* nbDeletedAddressIndex)
{
	// no target is to be deleted
	if (targetID == NULL) return;

	// rearch the leaf of the scene graph
	if ((*currentNode) == NULL) return;

	// search the sister node
	if ((*currentNode)->sister)
	{
		if (strcmp((*currentNode)->sister->id, targetID) == 0) 
		{
			dm_SceneNode* tobedeletedNode = (*currentNode)->sister;
		
			// if the daughter of deleted node is not empty, move them one level up
			if ((*currentNode)->sister->daughter != NULL) 
			{
				dm_SceneNode* sistNode = (*currentNode)->sister->daughter;

				while (sistNode->sister != NULL)
				{
					sistNode = sistNode->sister;
				}

				sistNode->sister = (*currentNode)->sister->sister;
				(*currentNode)->sister = (*currentNode)->sister->daughter;
			}
			else
			{
				(*currentNode)->sister = (*currentNode)->sister->sister;
			}

			// add the to-be-deleted node to the header of deleted node list
			tobedeletedNode->sister = NULL;
			tobedeletedNode->daughter = deletedNodeList;
			deletedNodeList  = tobedeletedNode;

			// save the deleted node address for delete it in the knownTargets
			deleteNodeAddress[*nbDeletedAddressIndex] = tobedeletedNode;
			(*nbDeletedAddressIndex)++;

			(*nbDeletedNodes)++;
		
			// continue search the scene graph in the current node
			deleteNode(targetID, currentNode, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
		else
		{
			// continue search the scene graph in the sister node
			deleteNode(targetID, &((*currentNode)->sister), nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
	}

	// if the daughter has the same ID with the targetID, 
	// then delete it,, and move all the nodes of the daughter one level above
	if ((*currentNode)->daughter != NULL) 
	{
		if ( strcmp((*currentNode)->daughter->id, targetID) == 0)
		{
			dm_SceneNode* tobedeletedNode = (*currentNode)->daughter;

			// move the sisters to the daughter daugther's sister
			if ((*currentNode)->daughter->daughter) {
				dm_SceneNode * daugNode = (*currentNode)->daughter->daughter;
				
				while (daugNode->sister != NULL) 
				{
					daugNode = daugNode->sister;
				}

				daugNode ->sister = (*currentNode)->daughter->sister;
				(*currentNode)->daughter = (*currentNode)->daughter->daughter;
			}
			// without sister node, then move the daughter daughter one level up
			else
			{
				(*currentNode)->daughter = (*currentNode)->daughter->sister;
			}

			// add the to-be-deleted node to the header of deleted node list
			tobedeletedNode->sister = NULL;
			tobedeletedNode->daughter = deletedNodeList;
			deletedNodeList  = tobedeletedNode;

			// save the deleted node address for delete it in the knownTargets
			deleteNodeAddress[*nbDeletedAddressIndex] = tobedeletedNode;
			(*nbDeletedAddressIndex)++;

			(*nbDeletedNodes) ++;

			// continue search and delete here
			deleteNode(targetID, currentNode, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
		else
		{
			// continue search and delete in the daugher node
			deleteNode(targetID, &((*currentNode)->daughter), nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
		}
	}	
}

void dm_Scene::removeGraph(dm_SceneNode* parentNode,
						   dm_SceneNode* graphNode,
						   int* nbDeletedNodes,
						   dm_SceneNode** deleteNodeAddress, 
						   int* nbDeletedAddressIndex){
	if (graphNode == NULL) return;

	removeGraph(graphNode, graphNode->sister, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);
	removeGraph(graphNode, graphNode->daughter, nbDeletedNodes, deleteNodeAddress, nbDeletedAddressIndex);

	// add the to-be-deleted node to the header of deleted node list
	graphNode->sister = NULL;	
	graphNode->daughter = deletedNodeList;
	deletedNodeList  = graphNode;

	// save the deleted node address for delete it in the knownTargets
	deleteNodeAddress[*nbDeletedAddressIndex] = graphNode;
	(*nbDeletedAddressIndex)++;
	(*nbDeletedNodes) ++;
}
