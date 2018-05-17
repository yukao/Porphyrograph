/*! \file dm-composition.h
 * \brief the header file of classes ( \a dm_ScheduledEvent, \a dm_Composition) and 
 * structures (  )
 *
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
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


#ifndef DM_COMPOSITION_H
#define DM_COMPOSITION_H

class  dm_SceneNode;


/*! \class dm_ScheduledEvent
 *  \brief		to be added
 */
class dm_ScheduledEvent {
 public:
  EventType           type;
  char                message[XMLAttrValLength];
  dm_Command         *currentCommand;
  dm_SceneNode      **targetNodes;
  int                 nbTargetNodes;
  dm_ElemAction      *delayedElemAction;
  float               delayedElemActiontimelimit;
  dm_ScheduledEvent  *nextEvent;

  dm_ScheduledEvent( EventType thetype , char *theMessage ,
		     dm_Command  *command ,
		     dm_SceneNode **thetargetNodes ,
		     int thenbTargetNodes , 
		     dm_ElemAction *theDelayedElemAction ,
		     float thedelayedElemActiontimelimit );
  ~dm_ScheduledEvent( void );
};

typedef tImageJPG   *PtImageJPG;
typedef tImagePNG   *PtImagePNG;
typedef GLubyte     *PGLubyte;

/*!
 * \brief OpenGL Error check
 * \param label 			to be added
 */
void checkGLErrors(const char *label);
bool checkFramebufferStatus();

/*!
 * COMPOSITION
 */
class dm_VertexShader;
class dm_FragmentShader;
class dm_GeometryShader;
class dm_Environment;

typedef dm_VertexShader     *PtVertexShader;
typedef dm_GeometryShader     *PtGeometryShader;
typedef dm_FragmentShader   *PtFragmentShader;

/*! \class dm_Composition
   \brief  remember the common structures of VirChor scene graph, such as quads and so on.
 */
class dm_Composition {
 public:
  /// textures
  GLuint           *textureIDs;
  long int         *modelViewMatrixPts;

  PtImageJPG        *pImageJPG;
  PtImagePNG        *pImagePNG;
  tImageRAW         *pImageRaw;
  char            **texture_names; 
#ifdef DM_HAVE_FFMPEG
  dm_Video        **video_records; 
#endif
  int                NbTextures;
  int                NbModelViewMatrixs;

  dm_VertexShader   **tabVertexShaders;
  int                 NbVertexShaders;
  char              **vertexShader_names; 

  dm_GeometryShader **tabGeometryShaders;
  int                 NbGeometryShaders;
  char              **geometryShader_names; 

  dm_FragmentShader **tabFragmentShaders;
  int                 NbFragmentShaders;
  char              **fragmentShader_names; 
  // \}

  /// interaction mode for agents
  GLuint             *selectBuf;

  /*! \name Group users
   *  \brief users in the current scene
   */
  // \{
  dm_SceneNode      **tabUsers;				/// the table used to remember the users
  int                 nbRegisteredUsers;			/// the actual number of useres registered
  int                 nbMaxUsers;					/// the maximum number of users
  // \}

  /*! \name Group default_viewpoint
   *  \brief the default viewpoint is viewpoint #0
   */
  // \{
  dm_Viewpoint      **viewpoints;
  dm_SceneNode      **viewpointsNodes;
  float             **viewpointsMatrices;
  int                 nbRegisteredViewpoints;
  int                 nbMaxViewpoints;
  // \}

  /*! \name Group objects_interaction_drawing
   *  \brief objects used for interaction and drawing in rendering mode
   */
  // \{
  dm_SceneNode      **tabGeometricalObjects;			/// the table used to remember geometrical objects
  int                 nbGeometricalObjects;						/// used for rendering
  int                 nbMaxGeometricalObjects;                /// the maximun number of geometrical objects
  int                 indCurrentGeometricalObject;           /// to be added
  // \}

  // \{
  dm_point3          *selectedControlPoint;
  // \}

  /*! \name Group scenes
   *  \brief scenes
   */
  // \{
  dm_Scene          **scenes; 
  char              **scene_names; 
  int                 NbScenes;
  
  dm_Scene          **drawn_scenes; 
  int                 NbDrawnScenes;
  // \}

  /*! 
   *  \brief elemActions that are not dependent on a specific object
   *  e.g. keystroke elemActions
   */ 
  dm_Script          *globalScript;
  
  /// the global command variable to remember all the command used in the scene file
  dm_Command         *globalCommand;

  dm_ScheduledEvent  *eventQueue;

  /*! \name Group OSC_parameters
   *  \brief list of OSC parameters of the currently executed action 
   *  if there is one, CurentAction_NbOSCParams is > 0
   */
  // \{
  int                 curentNbOSCParams;
  dm_ValScalar       *curentOSCParams;
  // \}

  /*! \name Group commands
   *  \brief the commands in the script
   */
  // \{
  /// the commands of the intialization part in the script
  dm_Command         *initializationCommand;
  /// the command of the termination part in the script
  dm_Command         *terminationCommand;
  /// commands outside of the initialization or termination part
  dm_Command         *aliasCommand;
  // \}
  
  
  dm_Composition( void );
  ~dm_Composition(void);
  
  /*! 
   * \brief Initialiation of the data structure for remembering the geometrical objects
   */
  void InitGeometricalObects(void);
  
  /*!
  * \brief counting and registering editable objects for future selection
  * \param nbMaxGeometricalObject  	 the number of geometrical objects in the scene graph
  */
  void CountGeometricalObjects( int *nbMaxGeometricalObject );
  
  /*!
   * \brief initialization the data structure for remembering users
   * (the default user is also initialized here to be user0)
   */
  void InitUsers(void);
  /*!
   * \brief collecting the users in a scene graph
   * \param nbMaxUser				the number of users after counting
   */
  void CountUsers( int *nbMaxUser );

  dm_SceneNode *FindUserReference( char *userId );
  void RegisterUsers( int *indUser , int nbMaxUsers ,
		      dm_SceneNode **tabUser );
  
  /*!
   * \brief user-based node visibility instantiated in a scene graph
   *
   * nodes can be hidden to some users
   */
  void RegisterUserExclusionDisplay( void );

  /*!
   * \brief initialization of the data structure for remembering the viewpoints existing 
   *  (one default viewpoint is set in this method)
   */ 
  void InitViewpoints(void);
  
  /*!
   * \brief collecting the viewpoints in a scene graph
   * \param nbMaxViewpoints			the number of viewpoints after counting
   */
  void CountViewpoints( int *nbMaxViewpoints );
  
  void RegisterViewpoints( int *indViewpoints , int nbMaxViewpoints ,
			   dm_Viewpoint **tabViewpoints ,
			   dm_SceneNode **tabViewpointsNodes );


  /*!
   * \brief loading the current scene
   * \param argc			to be added
   * \param argv 			to be added
   */
  void LoadSceneFiles( int argc, char **argv );
  
  /*!
   * \brief scene loading
   * \param fileName			to be added
   * \param rootScene 		to be added
   *
   * rootScene is the lowest root
   * so that reference resolution starts from this node
   */
  dm_Scene *FindScene( char *fileName , dm_SceneNode *rootScene );
  
  /*!
   * \brief scene printing
   * \param file 			to be added
   * \param isLong		to be added
   */
  void print( FILE *file , int isLong );
  void print( dm_Environment *env , int isLong );

  /*!
   * \brief multi-context OpenGL Model View dm_Matrix management
   * \param indModelViewMatrixPt			to be added
   * 
   * returns a pointer in the multithread storage table
   * allocates a new pointer if it has not yet been allocated
   * and copies the identity matrix in this case
   */
  long int GetModelViewMatrixPt( int indModelViewMatrixPt );
  void SetModelViewMatrixPt( int indModelViewMatrixPt , long int aModelViewMatrixPt );
  

  /*!
   * \brief returns a new index in the multithread storage table
   * does not allocates a new pointer 
   * (in some cases the pointer can be copied from another node
   */
  int AddModelViewMatrixPt( void );

  /*!
   * \brief multi-context OpenGL texture ID management
   * \param indTexture 		to be added
   */  
  GLuint GetTextureID( int indTexture );
  void SetTextureID( int indTextture , GLuint aTextureID );
  
  /*!
   * \brief video referencing
   * \param fileName		 to be added
   */
#ifdef DM_HAVE_FFMPEG
  dm_Video *FindVideo( char *fileName );
#endif  
  /*!
   * \brief texture loading
   * \param fileName 					to be added
   * \param theTextureEncoding 			to be added
   * \param textureDimension		to be added
   * \param texturing					to be added
   * \param newTexture				to be added
   */
  int FindTexture( char *fileName , 
		   TextureEncoding *theTextureEncoding ,
		   int textureDimension ,
		   bool is_rectangle ,
		   Texturing texturing  ,
		   bool *newTexture ,
		   bool invert );

  int GenerateTexture( bool is_rectangle , 
		       TextureFormat texture_format ,
		       int nb_attachments );
  int GenerateTexture( TextureFormat texture_format ,
		       int nb_attachments ,
		       int sizeX ,
		       int sizeY );
  int GenerateTexture( int vertexShaderParameterSize ,
		       bool is_rectangle );
  /*!
   * \brief texture:hard disk->RAM
   * \param strFileName					to be added
   * \param textureNo						to be added
   * \param theTextureEncoding				to be added
   * \param textureDimension			to be added
   * \param texturing							to be added
   */
  void LoadTextureInRAM( Pchar strFileName , int textureNo , 
			 TextureEncoding *theTextureEncoding  ,
			 int textureDimension ,
			 bool is_rectangle ,
			 Texturing texturing ,
			 bool invert );

  /*!
   * \brief texture:RAM->GPU, frame buffer texture
   * \param textureNo			to be added
   */
  void GenerateTextureInRAM( int textureNo ,
			     bool is_rectangle ,
			     TextureFormat texture_format ,
			     int nb_attachments );

  /*!
   * \brief texture:RAM->GPU, vertex shader texture, for param assign
   * \param vertexShaderParameterSize			to be added
   * \param textureNo												to be added
   */
  void GenerateTextureInRAM( int vertexShaderParameterSize , 
			     int textureNo ,
			     bool is_rectangle );

  // general texture allocation
  void GenerateTextureInRAM( int textureNo ,
			     TextureFormat texture_format ,
			     int nb_attachments ,
			     int sizeX ,
			     int sizeY );

  void TextureParameters( int              *width,
			  int              *height,
			  int              *depth,
			  bool             *has_alpha,
			  const void      **data,
			  GLint            *components,
			  GLenum           *format,
			  GLenum           *datatype,
			  GLenum           *texturefilter,
			  GLenum           *mipmapfilter,
			  int               textureNo , 
			  TextureEncoding   encoding,
			  TextureFormat     texture_format ,
			  TextureFilter  texture_min_filter ,
			  TextureFilter  texture_max_filter );
  
  /*!
   * texture:RAM->GPU, Interpolation defintition / Transfer to GPU + possible mipmaps
   * \param textureNo						to be added
   * \param theTextureEncoding				to be added
   * \param isMipMapped				to be added
   * \param dimension						to be added
   */

  void LoadTextureInGPUMemory( int textureNo, 
			       int attachment,
			       int nb_attachments,
			       TextureEncoding encoding , 
			       TextureGenerationMode textureGenerationMode ,
			       int isMipMapped ,
			       int is_rectangle ,
			       TextureFormat texture_format ,
			       TextureFilter  texture_min_filter ,
			       TextureFilter  texture_max_filter ,
			       GLenum textureTarget );

  /*!
   * \brief texture: free RAM
   * \param theTextureEncoding				to be added
   * \param textureNo						to be added
   */
  void FreeTextureInRAM( TextureEncoding theTextureEncoding , int textureNo ,
			 int nb_attachments );


  /*! \name Group shader_load
   * \brief shader loading
   */
  // @{
  dm_VertexShader *FindVertexShader( 
				  char *fileName ,
				  dm_ShaderSourceObject *shaderSource );
  
  dm_GeometryShader *FindGeometryShader(
				  char *fileName ,char * geometry_program_output_type,
				  int geometry_max_vertex,
				  dm_ShaderSourceObject *shaderSource );

  dm_FragmentShader *FindFragmentShader(
				      char *fileName  , 
				      dm_ShaderSourceObject   *shaderSource ); 
  // @}

  /*!
   * \brief updating (interpolations)
   */
  void update( void );
  
  /*!
   * \brief scene drawing
   * \param mode 			to be added
   */
  void draw( DrawingMode mode );

  void transformToPath( dm_Viewpoint *viewpoint , DrawingMode mode );

  /*!
   * \brief Collects non-object specific scripts that are 
   * looked up after each non-object specific event 
   * reception such as keystroke or message
   */
  void gatherScripts( void );
  
  /*! 
   * \brief used by functions that wish to push an event in the stack
   * \param thetype						to be added
   * \param theMessage				to be added
   * \param thetargetNode			to be added
   * \param nbTargetNodes			to be added
   */
  void pushEvent( EventType thetype , char *theMessage ,
		  dm_SceneNode **thetargetNode , int nbTargetNodes );
  
  /*!
   * \brief delayed actions are pushed in the stack
   * and will be executed when the deadline is reached
   * \param thetype					to be added
   * \param theElemAction   	to be added
   * \param theMessage			to be added
   * 
   * the initial calling message and the embedding command are
   * stored in order to access
   * the OSC parameters if there are some
   */
  void pushEvent( EventType thetype , 
		  dm_ElemAction * theElemAction , 
		  char *theMessage , 
		  dm_Command *theCommand ,
		  float thedelayedElemActiontimelimit );

  void popOneEvent( dm_ScheduledEvent **nextEvent ,
		    dm_ScheduledEvent *precedingEvent );
  void popEvents( void );
  void flushDelayedEvents( void);

  /*!
   * \brief PuceMuse experiment
   * \param x			to be added
   * \param y			to be added
   */
  void spotTriangle( int x, int y );


  /*!
   * \brief iterator oved geometrical objects for drawing
   */
  void operator !( void );
  dm_SceneNode * operator ++( int );
  dm_SceneNode * operator --( int );
  dm_SceneNode * operator ()( void );
};
#endif
