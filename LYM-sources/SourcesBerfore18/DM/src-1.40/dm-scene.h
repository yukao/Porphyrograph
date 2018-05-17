 /*! \file dm-scene.h
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-scene.h
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
#ifndef DM_SCENE_H
#define DM_SCENE_H


enum TagType{ OpeningTag = 0, ClosingTag , EmptyTag };

extern const char *BooleanString[3];
int BooleanToInt( bool bool_val );

void SaveEnvironment( DrawingMode mode , char *id );
void restoreEnvironment( DrawingMode mode );

class dm_SceneNode;

typedef   dm_Schedule * PSchedule;

/*! \defgroup scene_nodes
 * SCENE NODES
 */
/* \{ */
class  dm_Script;
class  dm_Command;
/*\}*/

void SampleRecursivelyCalledFunction( int *counter , dm_SceneNode *obj );

/*!
 * \brief delete the \a targetID in the reference table
 * \param nodeAddress				the address of the node that will be deleted
 * \param nbDeleted					the returned number of nodes deleted finally
 */
void deleteReferecesDynamic (dm_SceneNode* nodeAddress, int *nbDeleted);

/*!
 * \brief run time finding references,
 * first consults a list of known references
 * \param targetId						the node id that need to find the reference
 * \param nbReferences			the number of references for \a targetId
 * \param cutIfFound					whether does the find process stop immedately when one reference founds 
 * (true means stopping immediately, false continuing)
 * \param atRunTime				to be added
 */
dm_SceneNode **findReferencesDynamic( const char *targetId , 
				   int *nbReferences , bool cutIfFound ,
				   bool atRunTime );

/*!
 * \biref find the reference of a node
 * \param rootNode				the node int the scene graph to search the reference
 * \param targetId					the target node ID that need to check its reference
 * \param nbReferences		the number of references found
 * \param cutIfFound				find one reference and then return (true: return, false: find out all)
 * \param reportErrors			the errors if existing
 * finding a reference by its id in a scene graph
 * given a reference (href), returns the node pointed to 
 */
dm_SceneNode **findAllReferences( dm_SceneNode *rootNode , char *targetId , 
			       int *nbReferences , bool cutIfFound ,
			       bool reportErrors );

/*! 
 * \brief the class for the node in the scene graph
 */
class  dm_SceneNode {
 protected:
  // these fields that concern geometrical objects
  // must be put at the node level because they
  // should not be shared in the case of shared geometries
  // through the use of <use> links to identical objects
  // interaction

  /// enables/disables updates at every frame
  bool                    updatable;

  /// vertex array invalidation 
  bool                    is_invalid;

  /// pointer in an openGL multi-environment table for model view matrix
  int                     modelViewMatrixIndex;
  
  /// matrix used to store the returned getParameter
  /// value for parameter "modelViewMatrix"
  dm_ValScalar           *valScalarModelViewMatrix;

 public:
  NodeType                type;
  dm_SceneObject         *sceneObject;

  /// script and internal state for message processin
  dm_Script              *script;
  char                   *internalState;
  // script executed during the course of the rendering
  dm_Command             *renderingCommand;

  /// scalars used in expressions or controls
  dm_Scalar              *lisScalars;

  /// scalar tables used in expressions or controls
  dm_Table               *lisTables;

  /// scalar matrixs used in expressions or controls
  dm_Matrix              *lisMatrices;

  dm_SceneNode           *daughter , *sister;
  char                   *id;

  bool                    displayed;

  /// table of booleans (as many as users)
  bool                   *user_exclusion_display;

  /// string value of the attributes for temporary storage
  /// after compiling and before dereferencing
  char                   *userString_exclusion_display;

  /// table of booleans for attribute evaluation and printing
  dm_Table               *userFloatTable_exclusion_display;

  /// depth test deactivation
  bool                    depth_test;

  /// automatic view vector facing
  bool                    self_oriented;

  dm_SceneNode( void );
  ~dm_SceneNode(void);
  dm_SceneNode( NodeType theType );
  dm_SceneNode( NodeType theType , dm_SceneObject *aSceneObject );

  void Init( void );
  void InitGeometricalAttributes( void );
  void SetType( NodeType theType );
  void SetObject( NodeType theType , dm_SceneObject *aSceneObject );

  /*!
   * \brief the special release function for release the environment node.
   * This function is called at the end of the terminate method
   */
  void releaseForEnvtNode(void);

  void setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper , IntBinOp operI );

 /*!
   * \brief reading a dm_SceneNode
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

 /*! \name Group geometrical_property_retrieved_object_attached_node
  *  \brief geometrical properties that are retrieved from the geometrical object attached to the node
  */
  // \{
   /// objects that have a display list 
  bool IsGeometricalObject( void );
  int IsPostDrawable( void );
  
 /*! 
  * \brief  inverse transformation from viewpoint to the root node
  * \param viewpoint		the viewpoint
  * \param mode				to be added
  * function used for inverse transformation from
  * the viewpoint to the root node: recursive call
  * and transformation performed after call on the daughter node
  * in order to make the reverse composition of the inverse transformations
  */
  int transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode );
  // \}

  /*! \name Group scene_graph_traversals
    * \brief scene graph traversals for object collections
	*/
  // \{
  /*!
  * \brief collecting non local scripts
  * \param mommands to be added
  */
  void gatherScripts( dm_Command **commands );

  void RecursiveCall( void (*pt2Func)(int * , dm_SceneNode *) ,
		      int *counter );

  /*!
   * \brief counting geometrical objects for future interaction
   * \param nbMaxGeometricalObject			the returned number of geometrical objects
   */
  void CountGeometricalObjects( int *nbMaxGeometricalObject );

  /*!
   * \brief counting and registering viewpoints
   * \param indViewpoints						to be added
   * \param nbMaxViewpoints				max viewpoints returned
   * \param tabViewpoints						the structure storing the returned viewpoints
   * \param tabViewpointsNodes			the referece tables pointing to the viewpoints
   */
  void CountViewpoints( int *nbMaxViewpoints );
  void RegisterViewpoints( int *indViewpoints , int nbMaxViewpoints ,
			   dm_Viewpoint **tabViewpoints ,
			   dm_SceneNode **tabViewpointsNodes);

  /*!
   * \brief delete viewpoints
   * \param indViewpoints						the number of viewpoints existing
   * \param nbMaxViewpoints				max viewpoints returned
   * \param tabViewpoints						the structure storing the returned viewpoints
   * \param tabViewpointsNodes			the referece tables pointing to the viewpoints
   */
  void deleteRegisterViewpoints( int *indViewpoints , int nbMaxViewpoints ,
			   dm_Viewpoint **tabViewpoints ,
			   dm_SceneNode **tabViewpointsNodes);

  void CountHearpoints( int *nbMaxHearpoints );

  void CountUsers( int *nbMaxUser );

  /*!
   * \brief counting and registering users
   * \param indUser						to be added
   * \param nbMaxUsers				max users returned
   * \param tabUser						the structure storing the returned users
   */
  void RegisterUsers( int *indUser , int nbMaxUsers ,
		      dm_SceneNode **tabUser );

  /*!
   * \brief delete users
   * \param indUser						the number of users existing
   * \param nbMaxUsers				max users returned
   * \param tabUser						the structure storing the returned users
   */
  void deleteRegisterUsers( int *indUser , int nbMaxUsers ,
		      dm_SceneNode **tabUser );


  /*! 
   * \brief user-dependent non displayed  ****** to be added ******
   * \param theOperator			to be added
   * user-dependent non displayed node
   * the id of the user for which the node is non displayed 
   * is stored in userString_exclusion_display
   */
  void ComputeUserExclusionDisplay( FloatBinOp theOperator );
  void RegisterUserExclusionDisplay( void );

  /*! \name Gruo node_referenceing
   * \brief node referencing
   * \param searchedId				the node ID that needs searching
   * \param tabNodes				the returned nodes reference table
   * \param nbNodes					the returned number of references
   * \param cutIfFound				find one reference and then return (true: return, false: find out all)
   * \param justCount					whether it will only set the count value \a nbNodes and 
   * without set the the reference table \a tabNodes (true only count the value without set reference table)
   */
  // \{
  int findAllNodesInTree( char searchedId[XMLAttrValLength] ,
			  dm_SceneNode **tabNodes , int *nbNodes , 
			  bool cutIfFound , bool justCount , bool callOnSisters );

  /*!
   * \brief get the references of targetID in the rootNode
   * \param rootNode		the root node to search and build the references of \a targetID
   * \param targetID			the ID of the node that needs to build the references.
   * If it is NULL (default value),  all the reference of nodes in the scene graph will be recorded
   *
   * in the scene graph this method resolves references for node with ID \a targetID and is called when the user iteractively modify the scene graph.
   */
  void findAllScriptReferences(dm_SceneNode *rootNode, char* targetID = NULL );

 /*! 
  * \brief copies recursively a link to another node
  * \param auxNode			to be added
  *	 \param	depth					to be added
  * copies recursively a link to another node
  * only the dm_SceneNode part is copied
  * the geometry is coreferenced
  */
  void recursiveCopy( dm_SceneNode *auxNode , int depth );

  /*!
   * \brief resolves all the references
   * \param rootNode			to be added
   * resolves all the references attached to a node (recursively)
   */
  void resolveReferences( dm_SceneNode *rootNode );

  /*! \name Group display_functions
    * \brief display functions
	*/
  // \{
  void displayObject( DrawingMode mode );

 /*!
  * \brief display geometrical object in the second pass
  * \param mode			to be added
  * displays a geometrical object
  * called on each geometrical object in the second pass of
  * drawing 
  */
  void postDisplay( DrawingMode mode );

 /*!
  * \brief stores the model view matrices of each gometrical object and record its point
  * stores the model view matrices of each geometrical
  * object and records its pointer
  * or drraws non geom 
  */
  void display( DrawingMode mode , 
		bool saveEnvironment ,
		bool storeCurrentMatrix );
  void displayVideo( DrawingMode mode , int saveEnvironment );

  void storeModelViewMatrix( void );
  void restoreModelViewMatrix( void );
  float * getModelViewMatrix( void );
  int getModelViewMatrixIndex( void );
  void setModelViewMatrixIndex( int index );
  dm_matrix44 * getModelViewMatrix44( void );
  dm_Transformation * getTransformationNode( void );
  // \}

   /*! \name Group vertex array
   *  \brief vertex array
   */
  // \{
  GLuint GetvertexArrayID( void );
  GLuint GetelementArrayID( void );
  GLuint GetvertexBufferObjectID( void );
  void SetvertexArrayID( GLuint alistID );
  void SetelementArrayID( GLuint alistID );
  void SetvertexBufferObjectID( GLuint alistID );
  // \}

  /*! \name Group display_list_only
   *  \brief display lists only
   */
  // \{
  void deletevertexArray( void );
  void deleteelementArray( void );
  void deletevertexBufferObject( void );
  // \}

  /*! \name Group shader_material_properity
   *  \brief shader and material properties   
   */
  // \{
  dm_Shader *GetMaterialShader( void );
  // \}

  /// to be added
  dm_MaterialProperties *GetMaterial( void );
 
  /// return video in the scene node
#ifdef DM_HAVE_FFMPEG
  dm_Video *GetVideo( void );
#endif
  
  /// return the user in the scene node
  dm_User *GetUser( void );

  /*! \name Group interaction
   *  \brief interaction
   */
  // \{
  bool Interactable( void );
  void SetInteractable(bool state);
  bool Draggable( void );
  void SetDraggable(bool state);
  bool Testable( void );
  void SetTestable(bool state);
  bool Updatable( void );
  void SetUpdatable(bool state);
  void Catch( void );
  void EndCatch( void );
  bool Caught( void );
  void UpdatePositionFromLaser(dm_matrix44& Base);
  void UpdatePositionFromHand(dm_matrix44 &Base);
  void GetPosition( dm_matrix44& pos );
  void SetPosition( dm_matrix44& pos );
  
  /*!
   * \brief update the position of node according to the translation of the mouse
   * \param tx				the x translation of the mouse
   * \param ty				the y translation of the mouse
   * receives as input the mouse translation in the
   * projection plane coordinates
   * updates accordingly the new position of the
   * node in its coordinate system d_position (a matrix
   * that stores the local position of the object in its
   * coordinates during a drag. d_position is used at
   * the end of a catch to update the origine of an object)
   */
  void UpdatePositionFromPointer(float tx , float ty);
  // \}

  dm_SceneNode* nthNode( int rank );

  /*! 
   * \brief node printing
   * \param file					the file that will accept the print result
   * \param isLong			to be added
   * \param depth			to be added
   */
  void print( FILE *file , int isLong , int depth );

  /*! 
   * \brief non recursive printing of a node 
   * \param file					the file that will accept the print result
   * \param isLong			to be added
   * \param depth			to be added
   *
   * non recursive printing of a node,
   * printing only the component
   * for printings such as script printing
   */
  void printComponent( FILE *file , int isLong , int depth );

  /*! \name Group updating
   *  \brief updating
   */
  // @{
  void forceUpdate( bool to_invalidate );

  /*!
   * \param hidden			to be added
   *
   * displayed = the node has a non displayed ancestor
   * and is therefore not displayed
   */
  void update( bool hidden );
  // @}

 /*!
  * \brief invalidate the compiled data
  * invalidates the compiled data (
  * display list)
  * in case of modification of the geometric properties
  * (interpolation, script command...)
  */
  void invalidate( void );
  void validate( void );
  bool isInvalid( void );

  /*!
   * \brief used for copying interpolator node fields to the curNode
   * \param n			to be added
   */
  void CopyLocalVariables(dm_SceneNode * n);
  void operator=(dm_SceneNode&);
  
  /*! \name Group scalars_tables
   *  \brief scalars & tables
   */
  // \{
  dm_Scalar * getScalarFromName( const char * scalarName );
  dm_Table  * getTableFromName( const char * tableName );
  dm_Matrix * getMatrixFromName( const char * matrixName );
  // \}
};

/*! 
 * \brief the class defining the scene graph
 */
class  dm_Scene {
 public:
  dm_SceneNode           *root;

  /*! \name Group command_aliases
   *  \brief command aliases 
   */
  // \{
  dm_Scene( char *name );
  ~dm_Scene(void);

  void referenceResolution( dm_SceneNode *rootScene );

  /*!
   * \brief start from \a currentNode and recursively delete all the node before \a deletedEndNode
   * \param currentNode				the current start node
   * \param deleteEndNode		the end node
   */
  void deleteTillNode(dm_SceneNode* currentNode, dm_SceneNode* deleteEndNode);
  int transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode );
  void display( DrawingMode mode );
  void print( FILE *file , int isLong );
  void update( void );
  void gatherScripts( dm_Command **commands );
   // \}

  /*! 
   * \brief find the reference of a node used in the script and then record it 
   * \param targetID the id of the node that will be recorded.
   *  If it is NULL, all the references of the nodes in the scene graph will berecoded.
   */
  void findAllScriptReferences( char * targetID = NULL);


  void CountGeometricalObjects( int *nbMaxGeometricalObject );
  void CountViewpoints( int *nbMaxViewpoints );
  void RegisterViewpoints( int *indViewpoints , int nbMaxViewpoints ,
			   dm_Viewpoint **tabViewpoints ,
			   dm_SceneNode **tabViewpointsNodes);
  void CountUsers( int *nbMaxUser );
  void RegisterUsers( int *indUser , int nbMaxUsers ,
		      dm_SceneNode **tabUser );
  void RegisterUserExclusionDisplay( void );

  /*!
   * \brief delete a grpah in the scene
   * \param rootTargetGraph				the root of the graph to be deleted
   * \param startNode							the start position of the node to seek the target graph
   * \param nbDeleteNodes				the number of nodes deleted
   * \param deleteNodeAddress		the address(pointer) of the node deleted
   * \pram nbDeletedAddressIndex the index fro storing the index of the will-be deleted node in \a deleteNodeAddress
   */
  void deleteGraph(dm_SceneNode* rootTargetGraph, 
						   dm_SceneNode* startNode,
						   int*nbDeleteNodes, 
						   dm_SceneNode** deleteNodeAddress,
						   int* nbDeletedAddressIndex);

  /*!
   * \brief copy a graph into another graph without modify the original graph
   * \param sourceGraph			the source/input graph
   * \param targetGraph			the target/output graph
   */
  void copyGraph(dm_SceneNode* sourceGraph, dm_SceneNode** targetGraph);


  /*!
   * \brief delete the node with the specified ID
   * \param targetID											the ID of the node that will be deleted
   * \param currentNode									the current search position in the scene graph
   * \param nbDeletedNodes							return value, the number of nodes deleted
   * \param deletedNodeAddress					the address array for storing the node that will be deleted
   * \param nbDeletedAddressIndex			the index fro storing the index of the will-be deleted node in \a deleteNodeAddress
   */
  void deleteNode(char* targetID, 
	  dm_SceneNode** currentNode, 
	  int* nbDeletedNodes,  
	  dm_SceneNode** deletedNodeAddress,
	  int* nbDeletedAddressIndex);

  /*!
   * \brief delete all the nodes start from the node (a graph) with the specified ID
   * \param targetID											the ID of the graph that start to be deleted
   * \param currentNode									the current search position in the scene graph
   * \param nbDeletedNodes							return value, the number of nodes deleted
   * \param deletedNodeAddress					the address array for storing the node that will be deleted
   * \param nbDeletedAddressIndex			the index fro storing the index of the will-be deleted node in \a deleteNodeAddress
   */
  void searchGraphAndDelete(char* targetID, 
	  dm_SceneNode* currentNode, 
	  int* nbDeletedNodes,  
	  dm_SceneNode** deletedNodeAddress,
	  int* nbDeletedAddressIndex);

  /*!
   * \brief delete all the nodes start from the node (a graph)
   * \param parentNode					    the parent node 
   * \param currentNode						the graph node that will be deleted
   * \param nbDeletedNodes					return value, the number of nodes deleted
   * \param deletedNodeAddress				the address array for storing the node that will be deleted
   * \param nbDeletedAddressIndex			the index fro storing the index of the will-be deleted node in \a deleteNodeAddress
   */
  void removeGraph(dm_SceneNode* parentNode,
	 dm_SceneNode* graphNode, 	 
     int* nbDeletedNodes,
     dm_SceneNode** deleteNodeAddress, 
     int* nbDeletedAddressIndex);

  /*! 
   * \brief parsing a scene
   * \param fileName			to input file to be parsed
   */
  void parse( char *fileName );
};

#endif
