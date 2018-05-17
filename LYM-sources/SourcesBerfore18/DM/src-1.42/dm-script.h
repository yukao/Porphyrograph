/*! \file dm-script.h
 *
 *
 *
 *
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-script.h
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
#ifndef DM_SCRIPT_H
#define DM_SCRIPT_H

/*!
 *  \brief the resolution defined  
 */
enum ResolutionMode{ staticReferenceResolution = 0 , dynamicReferenceResolution };

/// the stauts of the target
enum TargetStatus{ EmptyNodeList = 0 , EmptyNodeElement , NodeList , NodeElement , DynamicNodeList , DynamicNodeElement };

enum BaseActionType{ BaseActionNodeModification = 0 , BaseActionSendInternalMessage , BaseActionSendExternalMessage , BaseActionSystemCommand , EmptyBaseActionType };

enum ActionType{ ElementaryAction = 0 , RepeatAction , WhileAction , IfAction , BreakAction , EmptyActionType };

enum TargetType{ TargetNode = 0 , TargetHost , EmptyTargetType };

enum BaseActionID{ set_node_attribute_value = 0 , set_node_scalar_value , set_node_table_value , set_node_matrix_value , set_environment_attribute_value , set_user_attribute_value , set_schedule_attribute_value , set_material_attribute_value , set_video_attribute_value , set_internal_state , set_viewpoint , set_current_viewpoint , execute_message , send_message , forward_message , send_message_udp , forward_message_udp , write_log_file , write_console , write_frame , screenshot , write_message , start_record_source_track , start_record_target_track , start_interpolate_track , start_read_source_track , stop_record_source_track , stop_record_target_track , stop_interpolate_track , stop_read_source_track , read_speed_scale_track , size_interpolate_track , stepwise_interpolate_track , reset_interpolate_track , load_svg_source_track , load_svg_target_track , clean_exit , execute_system_command , flush_delayed_actions , EmptyBaseActionID };

extern const char *BaseActionIDString[EmptyBaseActionID + 1];
extern char EmptyString[1];

extern char currentFilenamePng[512];
extern char currentFilenameJpg[512];
extern char description[512];


/*! \name Group event_type
 *  \brief types of event
 */
// \{
enum EventType{ keystroke = 0 , time_limit , random_cyclic_time , cyclic_time , varying_random_cyclic_time , varying_cyclic_time , message_event , delayed_command_execution , EmptyEventType };
extern const char *EventTypeString[EmptyEventType + 1];
// \}

class dm_Script;
class dm_Command;
class dm_IPClient;

/// the link for remembering all the scripts generated
struct strAllScript{
	dm_Script* curtScript;
	strAllScript* nextScript;
};

/*! 
 *  \brief SCRIPTING ACTIONS
 *  \param id 			input action ID
 * 
 *  \return the action type conresponding to \a BaseActionID
 */
BaseActionType BaseActionIDType( BaseActionID id );

/*! 
 *  \brief defines the basic methods needed by all actions
 */
class  dm_BaseAction {
  /// action: sent event
  EventType          targetEventType;

  /// action: parameters for set value
  Operator           set_value_operator;

  /*! \name Group action_param
   *  \brief action: parameters
   */
  // \{
  dm_SceneNode      *targetData;
   /// check whether the target is deleted by the deconstructor automatically 
  /// It is speical for replace_node, add_node, delete_node, delete_graph and add_graph
  bool			isTargetReleasedAutomatically;
  int                targetDataStatus;

  int                targetIntValue;
  int                targetIntValue2;
  char               targetStringValue[XMLAttrValLength];
  char               targetStringValueAux[XMLAttrValLength];
  float              targetFloatValue;
  float             *targetTabFloatValue;
  dm_Scalar         *targetScalarIn;    /// for stdin or stdout 
  dm_Scalar         *targetScalarOut;   /// in execute_system_command
  // \}

 public:
  dm_XMLTag         *XMLtag_set_value;

  /// immediate argument interpretation (useful for delayed actions)
  bool               immediate_interpretation;

  /// action: chaining
  dm_BaseAction     *nextActionForTheSameTarget;

  /// ACTION
  /// action: type
  BaseActionID       actionID;

  dm_BaseAction( void );
  ~dm_BaseAction(void);

  void clean( void );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  bool readBaseActionParameters( int *fileDepth , int *curChar , 
				 char *chaine , int *ind ,
				 EventType sourceEvent , char * sourceEventString ,
				 dm_SceneNode *sourceNode );
  
  /*!
   * \brief parsing a script action
   * \param XMLTag 			the input XML tag
   *
   * parsing the action tail: action performed
   * reading the parameters of the action: a subtask of parsing
   * 
   * \return true is successully parsered or return false
   */
  bool ParseOperator( dm_XMLTag *XMLtag );
  
  bool IndexIntValue( dm_XMLTag *XMLtag );
  bool IndexStringValue( dm_XMLTag *XMLtag );
  bool ParseBaseAction( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
			char *chaine , int *ind ,
			EventType sourceEvent , 
			char * sourceEventString ,
			dm_SceneNode *sourceNode ,
			BaseActionType  * actionsType );

  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  
  /*!
   *	\brief update the view matrix and the window association according to the modification of the scene graph
   *  
   *  It is called by execute(dm_SceneNode **targetNodes , int nbTargetNodes,
   *		  dm_IPClient **targetHosts, int nbTargetHosts ,
   *		  bool withExecutionValue, float executionValue)
   */
  void updateDisplayWindows( );

  /*!
   *	\brief update the user property for dynamically update of the scene graph
   */
  void updateUser();

  /*! 
   *	\brief update the command scripts according to the modification of the scene graph
   *   \param chrTargetID		the ID of the node that need to check. If it is NULL, all the node will be checked for update
   *
   *  It is called by execute(dm_SceneNode **targetNodes , int nbTargetNodes,
   *		  dm_IPClient **targetHosts, int nbTargetHosts ,
   *		  bool withExecutionValue, float executionValue)
   */
  void updateCommandScripts(char* chrTargetID = NULL);

  /*!
   *  \brief update the reference tables for all nodes in the graph
   *  \param		rootTargetGraph		the graph that will be updated
   */
  void updateReferenceTablesWithGraph(dm_SceneNode* rootTargetGraph);

  /*!
   *  \brief delete the reference tables for the particular graph in the graph
   *  \param		rootTargetGraph		the graph that will be updated
   */
  void deleteReferenceTablesWithGraph(dm_SceneNode* rootTargetGraph);

  /*! 
   *	\brief update the reference table according to the modification of the scene graph
   *   \param nodeTarget		the node that need to check
   *
   *  It is called by execute(dm_SceneNode **targetNodes , int nbTargetNodes,
   *		  dm_IPClient **targetHosts, int nbTargetHosts ,
   *		  bool withExecutionValue, float executionValue)
   */
  void updateReferenceTables( dm_SceneNode* nodeTarget );

  /*! 
   *	\brief delete the reference table according to the modification of the scene graph
   *   \param nodeTarget		the node that need to check
   *
   *  It is called by execute(dm_SceneNode **targetNodes , int nbTargetNodes,
   *		  dm_IPClient **targetHosts, int nbTargetHosts ,
   *		  bool withExecutionValue, float executionValue)
   */
  void deleteReferenceTables( dm_SceneNode* nodeTarget );

  /*!
   * \brief action execution 
   * \param targetNodes				the nodes that will receive the action
   * \param nbTargetNodes 			the number of \a targetNodes
   * \param targetHosts 				the target hosts that will receive the action message
   * \param nbTargetHosts 			the number of \a targetHosts
   * \param withExecutionValue 	whether there is an additional value accompanying the action
   * \param executionValue		 	the value for execution
   */
  void execute( dm_SceneNode **targetNodes , int nbTargetNodes,
		dm_IPClient **targetHosts, int nbTargetHosts ,
		bool withExecutionValue, float executionValue );
  void operator=(dm_BaseAction&);
};

/*! 
 *  \brief elementary acton: list of base actions and optional target
 */
class  dm_ElemAction {
  /// BASE ACTIONS
  dm_BaseAction     *actions;

  /*! \name Group target_node
   *  \brief target node(s)
   */
   // \{
   //   dm_SceneNode       targetNode; 
  char               targetNodeid[XMLAttrValLength];
  char               targetNodeidRef[XMLAttrValLength];
  dm_SceneNode     **targetNodeList;
  int                nbTargetNodes;
  int                targetStatus;
  // \}

  /*! \name Group target_host
   *  \brief target host(s)
   */
  // \{
  char               targetHostid[XMLAttrValLength];
  dm_IPClient      **targetHosts;
  int                nbTargetHosts;
  // \}

  /// delay between trigger reception and command execution
  char              *delayString;

  //float              delay;

  /// immediate argument interpretation (useful for delayed actions)
  bool               immediate_interpretation;

 public:
  dm_ElemAction( void );
  ~dm_ElemAction(void);

  void clean( void );
  void cleanActionsAndTargets( void );
  void printActionsAndTargets( FILE *file , int isLong , int depth );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  bool readActionParameters( int *fileDepth , int *curChar , 
			     char *chaine , int *ind );
  
  /*! 
   *  \brief parsing a script command
   *  \param sourceNode 		the node that need parsered
   *  \param fileScene 			the input XML files
   *  \param curChar 			to be added
   *  \param XMLtag 			to be added
   *  \param chaine 				to be added
   *  \param ind  					to be added
   * parsing the target nodes or targets hosts of a command
   * 
   * \return target type
   */
  TargetType ParseTargets( dm_SceneNode *sourceNode, 
			   int *fileDepth , 
			   int *curChar, dm_XMLTag *XMLtag, char *chaine , int *ind);

  /*! 
   * \brief parsing a sequence of actions: actions performed + target
   * \param fileScene 					input file
   * \param curChar 					to be added
   * \param XMLtag 					to be added
   * \param chaine 						to be added
   * \param ind 							to be added
   * \param sourceEventString 	to be added
   * \param sourceNode 				to be added
   */
  bool ParseElemAction( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
			char *chaine , int *ind ,
			EventType sourceEvent , 
			char * sourceEventString ,
			dm_SceneNode *sourceNode );
  
  /*!
   * \brief ELEMENTARY ACTION dynamic target reference resolution (run time)
   * 
   * finding references of nodes involved in a script description
   * at runtime time: commands received through udp
   * or expressions that contain an expression with a dynamic reference
   * such as an attribute or a scalar value
   */
  void findScriptReferencesDynamic( void );
  
  /*!
   * \brief ELEMENTARY ACTION  target reference resolution (compile time)
   * \param rootNode 			the start node in the scene graph 
   * \param targetID				the id of the target node.
   * If it is NULL (default value), the references of all nodes are recorded.
   *
   * finding references of nodes involved in a script description
   * at compile time for internal scripts
   */
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  
  /*! 
   *  \brief command execution
   *  \param mode 					to be added
   *  \param isDelayed 			to be added
   *  \param message 			to be added
   *  \param theCommand 	to be added
   * 
   */
  void execute(ResolutionMode mode, bool isDelayed, char* message, dm_Command *theCommand);

  void operator=(dm_ElemAction&);
};

/*! 
 *  \brief COMPOSITE ACTIONS: WHILE & REPEAT
 */
BaseActionType BaseActionIDType( BaseActionID id );

/*!
 * \brief scripting actions: either elementary, repeat, or while actions
 */
class  dm_Action;

/*!
 *  \brief the class processing repeat action 
 */
class  dm_ActionRepeat {
 public:
  dm_Scalar          *controlScalar;
  char               *scalarString;
  char               *begin;
  char               *end;
  char               *step;
  dm_Action          *actions;
  bool                isActionBroken;

  dm_ActionRepeat( void );
  ~dm_ActionRepeat(void);

  void clean( void );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  bool ParseActionRepeat( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
			  char *chaine , int *ind ,
			  EventType sourceEvent , 
			  char * sourceEventString ,
			  dm_SceneNode *sourceNode ,
			  dm_Action *motherLoopAction );
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message ,
		dm_Command *theCommand );
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_ActionRepeat&);
};

/*! 
 *  \brief the class processing if action 
 */
class  dm_ActionIf {
 public:
  char                     *value1;
  char                     *value2;
  BooleanOperator           oper;
  dm_Action                *actions;
  dm_Action                *actionsElse;

  dm_ActionIf( void );
  ~dm_ActionIf(void);

  void clean( void );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  bool ParseActionIf( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
		      char *chaine , int *ind ,
		      EventType sourceEvent , 
		      char * sourceEventString ,
		      dm_SceneNode *sourceNode ,
		      dm_Action *motherLoopAction );
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message ,
		dm_Command *theCommand );
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_ActionIf&);
};

/*! 
 *  \brief the class processing loop break 
 */
class  dm_ActionBreak {
 public:
  dm_Action                 *motherLoop;
  dm_ActionBreak( void );
  ~dm_ActionBreak(void);

  void print( FILE *file , int isLong , int depth );
  bool ParseActionBreak( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
			 char *chaine , int *ind ,
			 EventType sourceEvent , 
			 char * sourceEventString ,
			 dm_SceneNode *sourceNode ,
			 dm_Action *motherLoopAction );
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message ,
		dm_Command *theCommand );
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_ActionBreak&);
};

/*! 
 *  \brief  the class processing while action
 */
class  dm_ActionWhile {
 public:
  char                      *value1;
  char                      *value2;
  BooleanOperator            oper;
  dm_Action                 *actions;
  bool                       isActionBroken;

  dm_ActionWhile( void );
  ~dm_ActionWhile(void);

  void clean( void );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  bool ParseActionWhile( int *fileDepth , int *curChar , dm_XMLTag *XMLtag ,
			 char *chaine , int *ind ,
			 EventType sourceEvent , 
			 char * sourceEventString ,
			 dm_SceneNode *sourceNode ,
			 dm_Action *motherLoopAction );
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message ,
		dm_Command *theCommand );
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_ActionWhile&);
};

/*!
 *  \brief ACTION: COMPOSITE OR ELEMENTARY ACTION
 */
class dm_Action {
 public:
  ActionType             type;
  dm_ElemAction         *elemAction;
  dm_ActionRepeat       *actionRepeat;
  dm_ActionWhile        *actionWhile;
  dm_ActionIf           *actionIf;
  dm_ActionBreak        *actionBreak;

  dm_Action              *nextAction;

  dm_Action( void );
  ~dm_Action(void);

  void clean( void );
  void deleteAllocatedComponents( void );
  bool ParseAction( int *fileDepth , int *curChar , 
		    dm_XMLTag *XMLtag ,
		    char *chaine , int *ind ,
		    EventType sourceEvent , 
		    char * sourceEventString ,
		    dm_SceneNode *sourceNode ,
		    dm_Action *motherLoopAction );
  void print( FILE *file , int isLong , int depth );
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message ,
		dm_Command *theCommand );
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_Action& c);
};


class  dm_Command;

/*! 
 *  \brief trigger: triggers a list of synchronous actions
 */
class  dm_Trigger {
 public:

  /*! \name Group trigger_event
   *  \brief triggering events
   */
  // \{
  /// regular events
  EventType       sourceEvent;
  char            sourceEventString[XMLAttrValLength];
  // \}

  /*! \name Group scheduled_evt
   *  \param scheduled events
   */
  // \{
  float           sourcebegintime;
  float           sourcetimelimit;
  float           sourceperiod;
  float           sourcevariation;
  // \}

  /*! \name Group state
   *  \brief state
   */
  // \{
  char           *expectedState;
  char           *stateOperator;
  BooleanOperator stateOperatorValue;
  // \}

  dm_Trigger( void );
  ~dm_Trigger( void );

  void clean( void );
  void print( FILE *file , int isLong , int depth );
  bool ParseTrigger( dm_SceneNode *sourceNode ,
		     int *fileDepth , int *curChar , 
		     dm_XMLTag *XMLtag ,
		     char *chaine , int *ind );
  void operator=(dm_Trigger&);
};

/*! 
 *  \brief a command: a trigger and a list of synchronous actions
 * 
 * basically, a command is made of a trigger and a list of syncrhonous actions
 * (enclosed between an opening and a closing <atcion> tag)
 * commands can be delayed by setting the value of the attribute
 * delay of the opening <action> element* 
 * 
 */
class  dm_Command {
 public:
  /*!
   *  command chaining
   * nextCommand is used to chain commands that belong to the same
   * script but can have different triggers
   */
  dm_Command        *nextCommand;
  
  /*! 
   * synchronousCommand is used to chain commands that have the
   * same trigger but correspond to different actions
   */
  dm_Command        *synchronousCommand;

  /// TRIGGER
  dm_Trigger         trigger;

  /// ACTIONS
  dm_Action         *actions;

  /// PATTERN + ADDRESS AND PARAMETERS IN CASE OF COMMAND ALIAS
  char              *aliasPattern;
  /// OSC tag e.g.   /setrootvalue
  char              *CommandAliasOSCTag;
  /// command parameters (for commands such as /address param1 param2 param3...)
  int                commandNbOSCParams;
  dm_ValScalar      *commandOSCParams;

  /*!
   *  cyclic time based triggers must know the node that
   * carried them before being reported to the global script
   * the state of this node can be used to control the 
   * execution of this command
   */
  dm_SceneNode      *initialCarrierNode; 

  dm_Command( void );
  ~dm_Command(void);

  void clean( void );
  void cleanActionsAndTargets( void );
  void deleteAllocatedComponents( void );
  void print( FILE *file , int isLong , int depth );
  void printActions( FILE *file , int isLong , int depth );
  bool readActionParameters( int *fileDepth , int *curChar , 
			     char *chaine , int *ind );
  
  /*!
   * \brief parsing a script command
   * \param fileScene 			to be added
   * \param curChar 			to be added
   * \param XMLtag 			to be added
   * \param sourceNode 		to be added
   * \param chaine 				to be added
   * \param ind 					to be added
   *  
   * parsing the command tail: actions performed + target
   * 
   */    
  bool ParseCommandTail( int *fileDepth , int *curChar , 
			 dm_XMLTag *XMLtag ,
			 dm_SceneNode *sourceNode ,
			 char *chaine , int *ind );
  
  /*!
   * \brief COMMAND static reference resolution (compile time)
   * \param rootNode 		the node that starts to build the references
   * \param targetID			the ID of the node that needs to build the references
   * If it is NULL (default value), the references of all nodes in the scene graph will be recorded
   *
   * finding references of the node  with ID \a targetID involved in a script description
   * at compile time for internal scripts
   */
  void findScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  
  /*!
   * \brief storage of OSC parameters on the composition node for parameter
   * value substitution ({$1}), ({$2})...)
   * \param pch 			to be added
   * 
   */
  void StoresOSCParameters( char *pch );
  
  /*!
   * \brief command execution
   * \param mode 				to be added
   * \param isDelayed 		to be added
   * \param message 		to be added
   * 
   * mesage is stored so that it can be transmitted to
   * delayed actions that will use the OSC parameters
   * when they are executed lated
   */
  void execute( ResolutionMode mode , bool isDelayed ,
		char * message );
  void operator=(dm_Command&);
};


/*!
 * \brief script: a list of commands
 */
class  dm_Script {
 public:
  dm_Command   *commandList;
  char         *id;

  dm_Script( void );
  dm_Script( char *ID );
  ~dm_Script(void);

  void cleanScript( void );
  void print( FILE *file , int isLong , int depth );
  

  /*!
   * \brief SCRIPT static reference resolution (compile time)
   * \param rootNode	the root node that starts to build the references
   * \param targetID		the ID of the node that need to build the references. 
   *  If it is NULL (default value), the references of all nodes in the scene graph will be recorded
   *
   * finding references of the node with ID \a targetID involved in a script description at compile time for internal scripts
   */
  void findScriptReferences( dm_SceneNode *rootNode, char * targetID = NULL);
  
  /*!
   * \brief EVENT RECEPTION
   * \param event 			input event
   * \param messge 		to be added
   * \param receiver 		to be added
   * 
   * event reception by a node
   * if it is an event without target, the event is received by the 
   * global script
   * if it is an event with a target, the node that contains the
   * script is given as input parameter so that the matching between
   * the internal state of the node and the expected state can be verified

   * should receive the calling command as well
   * for OSC parameter instantiation
   */
  void receive( EventType event , char *message ,
		dm_SceneNode *receiver );
  bool triggerScheduledCommands( float time );
};

/*!
 * \briefSCRIPT PARSING: XML SYNTAX
 * \param fileScene 			to be added
 * \param curChar 				to be added
 * \param sourceNode 		to be added
 * \param ID			 			to be added
 */
dm_Script *ParseScript( int *fileDepth , int *curChar , 
			dm_SceneNode *sourceNode , char *ID );

/*!
 * \brief COMMAND PARSING: XML SYNTAX
 * \param fileScene 				to be added
 * \param curChar 				to be added
 * \param sourceNode 		to be added
 * \param requireTrigger 	to be added
 * \param chaine 					to be added
 * \param ind 						to be added
 * 
 * parses a list of commands that share the same trigger
 * consists of one trigger and one or more associated actions 
 */
dm_Command * ParseCommand( int *fileDepth , int *curChar , 
			   dm_SceneNode *sourceNode , bool requireTrigger ,
			   char *chaine , int *ind );

#endif
