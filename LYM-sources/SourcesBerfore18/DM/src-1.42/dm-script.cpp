/*! \file dm-script.cpp
 * 
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

#include "dm-all_include.h"

const char *BaseActionIDString[EmptyBaseActionID + 1] = { "set_node_attribute_value", "set_node_scalar_value" , "set_node_table_value" , "set_node_matrix_value" , "set_environment_attribute_value", "set_user_attribute_value" , "set_schedule_attribute_value", "set_material_attribute_value" , "set_video_attribute_value" , "set_internal_state" , "set_viewpoint" , "set_current_viewpoint" , "execute_message" , "send_message" , "forward_message" , "send_message_udp" , "forward_message_udp" , "write_log_file" , "write_console" ,  "write_frame" ,  "screenshot" ,  "write_message" , "start_record_source_track" , "start_record_target_track" , "start_interpolate_track" , "start_read_source_track" , "stop_record_source_track" ,  "stop_record_target_track" , "stop_interpolate_track" , "stop_read_source_track" ,  "read_speed_scale_track", "size_interpolate_track" ,  "stepwise_interpolate_track" ,  "reset_interpolate_track" , "load_svg_source_track" , "load_svg_target_track" , "exit" , "execute_system_command" , "flush_delayed_actions" , "EmptyBaseActionID" };

const char *EventTypeString[EmptyEventType + 1] = { "keystroke" , "time_limit" , "random_cyclic_time" , "cyclic_time" , "varying_random_cyclic_time" , "varying_cyclic_time" , "message_event" , "delayed_command_execution" , "EmptyEventType" };

////////////////////////////////////////////////////////////
// RETURNS THE ID OF A NODE IF IT IS NOT NULL
// OTHERWISE RETURNS A POINTER TO AN EMPTY STRING
////////////////////////////////////////////////////////////
char EmptyString[1] = { 0 };

char * NodeIDOrEmptyString( dm_SceneNode *node ) {
  if( node )
    return node->id;
  else
    return EmptyString;
}

enum BaseActionType BaseActionIDType( BaseActionID id ) {
  switch( id ) {
  case set_node_attribute_value:
  case set_node_scalar_value:
  case set_node_table_value:
  case set_node_matrix_value:
  case set_environment_attribute_value:
  case set_user_attribute_value:
  case set_schedule_attribute_value:
  case set_material_attribute_value:
  case set_video_attribute_value:
  case set_internal_state:
  case execute_message:
  case send_message:
  case forward_message:
  case set_viewpoint:
    return BaseActionNodeModification;
  case send_message_udp:
  case forward_message_udp:
    return BaseActionSendExternalMessage;
  case write_log_file:
  case write_console:
  case write_frame:
  case screenshot:
  case write_message:
  case start_record_source_track:
  case start_record_target_track:
  case start_interpolate_track:
  case start_read_source_track:
  case stop_record_source_track:
  case stop_record_target_track:
  case stop_interpolate_track:
  case stop_read_source_track:
  case read_speed_scale_track:
  case size_interpolate_track:
  case stepwise_interpolate_track:
  case reset_interpolate_track:
  case load_svg_source_track:
  case load_svg_target_track:
  case clean_exit:
  case execute_system_command:
  case flush_delayed_actions:
  case set_current_viewpoint:
    return BaseActionSystemCommand;
  case EmptyBaseActionID:
    return EmptyBaseActionType;
  default:
    sprintf( ErrorStr , "Error: unknown action type [%s]!" , BaseActionIDString[id] ); ReportError( ErrorStr ); throw 68;
  }
}

dm_BaseAction::dm_BaseAction( void ) {
  XMLtag_set_value = new dm_XMLTag();
  clean();
}

dm_BaseAction::~dm_BaseAction(void) {
	
  if (XMLtag_set_value)  {
      delete XMLtag_set_value;
      XMLtag_set_value = NULL;
    }

  if (!isTargetReleasedAutomatically) 
  {
	  delete targetData;
	  targetData = NULL;
  }

  if (targetTabFloatValue) {
      delete [] targetTabFloatValue;
      targetTabFloatValue = NULL;
    }

  if (targetScalarOut) {
      delete [] targetScalarOut;
      targetScalarOut = NULL;
    }

  if (targetScalarIn) {
      delete [] targetScalarIn;
      targetScalarIn = NULL;
    }

  if (nextActionForTheSameTarget) {
      delete nextActionForTheSameTarget;
      nextActionForTheSameTarget = NULL;
    }
   
}

void dm_BaseAction::clean( void ) {
  actionID = EmptyBaseActionID;

  targetEventType = EmptyEventType;

  set_value_operator = Assign;
  XMLtag_set_value->deallocateXMLTag();

  targetData = NULL;
  isTargetReleasedAutomatically = true;
  targetDataStatus = EmptyNodeElement;
  targetIntValue = 0;
  *targetStringValue = 0;
  *targetStringValueAux = 0;
  targetFloatValue = 0.0;
  targetTabFloatValue = NULL;
  targetScalarOut = NULL;
  targetScalarIn = NULL;

  nextActionForTheSameTarget = NULL;
  immediate_interpretation = false;
}

void dm_BaseAction::deleteAllocatedComponents( void ) {
  if( targetTabFloatValue ) {
    delete [] targetTabFloatValue;
    targetTabFloatValue = NULL;
  }
}

void dm_BaseAction::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<%s " , BaseActionIDString[actionID] );

  switch( actionID ) {
  case set_node_attribute_value:
  case set_environment_attribute_value:
  case set_user_attribute_value:
    {
      fprintf( file , "operator=\"%s\">\n" , 
	       OperatorString[set_value_operator] );

      XMLtag_set_value->print( file , depth + 1 );
      
      indent( file , depth );
      fprintf( file , "</%s>\n" , BaseActionIDString[actionID] );
    }
    break;

  case set_node_scalar_value:
  case set_node_table_value:
  case set_node_matrix_value:
    {
      if( XMLtag_set_value->hasAttribute( "index" ) ) {
	fprintf( file , "id=\"%s\" value=\"%s\" index=\"%s\" operator=\"%s\" />\n" , 
		 targetStringValue ,
		 XMLtag_set_value->getAttributeStringValue( "value" ) ,
		 XMLtag_set_value->getAttributeStringValue( "index" ) ,
		 OperatorString[set_value_operator] );
      }
      else {
	fprintf( file , "id=\"%s\" value=\"%s\" operator=\"%s\" />\n" , 
		 targetStringValue ,
		 XMLtag_set_value->getAttributeStringValue( "value" ) ,
		 OperatorString[set_value_operator] );
      }
    }
    break;

  case set_schedule_attribute_value:
    {
      if( *targetStringValueAux ) {
	fprintf( file , "variable=\"%s\" " , targetStringValueAux );
      }
	
      fprintf( file , "operator=\"%s\" " , 
	       OperatorString[set_value_operator] );
      
      // index: schedule rank
      fprintf( file , "index=\"%s\" >\n" , targetStringValue  );

      XMLtag_set_value->print( file , depth + 1 );
      
      indent( file , depth );
      fprintf( file , "</%s>\n" , BaseActionIDString[actionID] );
    }
    break;

  case set_material_attribute_value:
  case set_video_attribute_value:
    {
      if( *targetStringValue ) {
	fprintf( file , "index=\"%s\" " , targetStringValue );
      }
      fprintf( file , "operator=\"%s\">\n" , 
	       OperatorString[set_value_operator] );
      
      XMLtag_set_value->print( file , depth + 1 );
      
      indent( file , depth );
      fprintf( file , "</%s>\n" , BaseActionIDString[actionID] );
    }
    break;
  case execute_system_command:
    fprintf( file , "value=\"%s\" " , targetStringValue );
    if( targetScalarOut ) {
      fprintf( file , "stdout=\"%s\" " , targetScalarOut->GetId() );
    }
    if( targetScalarIn ) {
      fprintf( file , "stdin=\"%s\" " , targetScalarIn->GetId() );
    }
    fprintf( file , "/>\n" );
    break;
  case flush_delayed_actions:
    fprintf( file , "/>\n" );
    break;
  case set_internal_state:
  case write_log_file:
  case write_console:
  case write_message:
    fprintf( file , "value=\"%s\" />\n" , targetStringValue );
    break;
  case stepwise_interpolate_track:
  case load_svg_source_track:
  case load_svg_target_track:
    fprintf( file , "index=\"%d\" value=\"%s\" />\n" , targetIntValue , targetStringValue );
    break;
  case reset_interpolate_track:
  case read_speed_scale_track:
  case size_interpolate_track:
  case start_record_source_track:
  case start_record_target_track:
  case start_interpolate_track:
  case start_read_source_track:
  case stop_record_source_track:
  case stop_record_target_track:
  case stop_interpolate_track:
  case stop_read_source_track:
    fprintf( file , "value=\"%s\" />\n" , targetStringValue );
    break;
  case write_frame:
    if( targetIntValue >= 0 ) {
      fprintf( file , "index=\"%d\" />\n" , targetIntValue + 1 );
    }
    else {
      fprintf( file , " />\n" );
    }
    break;
  case screenshot:
    fprintf( file , "value=\"%s\" type=\"%s\" />\n" , targetStringValue , targetStringValueAux );
    break;
  case execute_message:
  case send_message:
  case send_message_udp:
  case set_viewpoint:
  case set_current_viewpoint:
    fprintf( file , "index=\"%s\" />\n" , targetStringValue );
    break;
  case clean_exit:
  case forward_message:
  case forward_message_udp:
    fprintf( file , " />\n" );
    break;
  default:
    break;
  }
}

bool dm_BaseAction::ParseOperator( dm_XMLTag *XMLtag ) {
  // operator
  if( XMLtag->hasAttribute( "operator" ) ) {
    set_value_operator 
      = GetOperator( XMLtag->getAttributeStringValue( "operator" ) );
    if( set_value_operator == EmptyOperator ) {
      sprintf( ErrorStr , "Error:unknown operator [%s] in %s action!" , XMLtag->getAttributeStringValue( "operator" ) , XMLtag->tag ); ReportError( ErrorStr ); 
      return false;
    }
  }
  else {
    // default operator: assign
    set_value_operator = Assign;
  }
  return true;
}

bool dm_BaseAction::IndexIntValue( dm_XMLTag *XMLtag ) {
  // index
  if( XMLtag->hasAttribute( "index" ) ) {
    // the parameter atRunTime is set to false
    // atRunTime is only true for strings that are dynamically
    // evaluated. the index value is evaluated at compile time
    targetIntValue = XMLtag->getAttributeIntValue( "index" , 
						   false ) - 1;
  }
  else {
    // default schedule: the first one
    targetIntValue = 0;
  }
  return true;
}

bool dm_BaseAction::IndexStringValue( dm_XMLTag *XMLtag ) {
  // index
  if( XMLtag->hasAttribute( "index" ) ) {
    // the parameter atRunTime is set to false
    // atRunTime is only true for strings that are dynamically
    // evaluated. the index value is evaluated at compile time
    strcpy( targetStringValue , XMLtag->getAttributeStringValue( "index" ) );
  }
  else {
    // default schedule: the first one
    *targetStringValue = 0;
  }
  return true;
}

bool dm_BaseAction::ParseBaseAction( int *fileDepth , int *curChar , 
				     dm_XMLTag *XMLtag ,
				     char *chaine , int *ind ,
				     EventType sourceEvent , 
				     char *sourceEventString ,
				     dm_SceneNode *sourceNode ,
				     BaseActionType  * actionsType ) {

  actionID = EmptyBaseActionID;
  for( int indAux = 0 ; indAux < EmptyBaseActionID ; indAux++ ) {
    if( strcmp( XMLtag->tag , BaseActionIDString[indAux] ) == 0 ) {
      actionID = (BaseActionID) indAux;
      break;
    }
  }
  if( actionID == EmptyBaseActionID ) {
    sprintf( ErrorStr , "Error:unknown action string [%s]!" , XMLtag->tag ); ReportError( ErrorStr ); 
    return false;
  }

  // action tyoe
  if( *actionsType == EmptyBaseActionType ) {
    *actionsType = BaseActionIDType( actionID );
  }
  else if( BaseActionIDType( actionID ) != *actionsType ) {
    sprintf( ErrorStr , "Error: heteorgenous actions cannot be joined in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
  }

  /////////////////////
  // reading action parameters
  switch( actionID ) {
    // generic message: modifies the value of attribute of a dm_Scene Object
    // the message is made of an operator and 
    // an XML tag that contains a node type, an attribute name and an attribute value
  case set_node_attribute_value:
  case set_environment_attribute_value:
  case set_user_attribute_value:
    {
      // operator
      ParseOperator( XMLtag );

      // these actions contain an empty tag that defines the 
      // values that are modified in the target node(s)
      if( XMLtag->tagType == OpeningTag ) {
	// the xml tags that contains the attribute and its value
	// that will serve as parameter for the output
	XMLtag_set_value->deallocateXMLTag();
	XMLtag_set_value->ParseTagXML( fileDepth , curChar ,  
				       true , chaine , ind );
	if( XMLtag_set_value->tagType != EmptyTag ) {
	  sprintf( ErrorStr , "Error: Expecting empty XML tag content element of set_node_attribute_value, set_user_attribute_value or set_environment_attribute_value action [%d]!" , XMLtag_set_value->tagType ); ReportError( ErrorStr ); throw 68;
	}

	XMLtag_set_value->substituteParametersXML();

	// action closing tag
	ExpectClosingTagXML( fileDepth , curChar ,  XMLtag->tag , chaine , ind );
      }

      // empty tag: attributes of the node 
      // copies the attribute/values of the current tag
      else if( XMLtag->tagType == EmptyTag ) {
	XMLtag_set_value->deallocateXMLTag();
	XMLtag_set_value->copyXMLTag( XMLtag );
	XMLtag_set_value->substituteParametersXML();

	//       }
	// empty tag action: attributes of the <node>
	
	//	sprintf( ErrorStr , "Error: Expecting non empty empty XML tag for set_node_attribute_value, set_user_attribute_value or set_environment_attribute_value action [%d]!" , tagType ); ReportError( ErrorStr ); throw 68;
      }
    }
    break;
  case set_node_scalar_value:
  case set_node_table_value:
  case set_node_matrix_value:
    {
      // operator
      ParseOperator( XMLtag );

      // empty tag: attributes of the node 
      // copies the attribute/values of the current tag
      if( XMLtag->tagType == EmptyTag ) {
	// parameter values in the action strings
	XMLtag_set_value->deallocateXMLTag();
	// fprintf( stdout , "XMLtag:\n" );
	// XMLtag->print(stdout,1);
	XMLtag_set_value->copyXMLTag( XMLtag );
	// fprintf( stdout , "XMLtag_set_value:\n" );
	// XMLtag_set_value->print(stdout,1);
	XMLtag_set_value->substituteParametersXML();
	// fprintf( stdout , "XMLtag_set_value:\n" );
	// XMLtag_set_value->print(stdout,1);

	if( XMLtag_set_value->hasAttribute( "id" ) ) {
	  strcpy( targetStringValue , 
		  XMLtag_set_value->getAttributeStringValue( "id" ) );
	}
      }
      // error: non empty tag actions
      else {
	sprintf( ErrorStr , "Error: Expecting empty XML tag for set_node_scalar_value, set_node_table_value and set_node_matrix_value [%d]!" , XMLtag->tagType ); ReportError( ErrorStr ); throw 68;
      }
    }
    break;

    // generic message: modifies the value of schedule attributes
    // the message is made of a schedule rank, an operator and 
    // an XML tag that contains a node type, an attribute name and an attribute value
  case set_schedule_attribute_value:
    {
      // operator
      ParseOperator( XMLtag );

      // index: schedule rank
      IndexStringValue( XMLtag );

      // variable ID if there's one
      *targetStringValueAux = 0;
      if( XMLtag->hasAttribute( "variable" ) ) {
	strcpy( targetStringValueAux , 
		XMLtag->getAttributeStringValue( "variable" ) );
      }
      
      // these actions contain an empty tag that defines the 
      // values that are modified in the target node(s)
      if( XMLtag->tagType == OpeningTag ) {

	// the xml tags that contains the attribute and its value
	// that will serve as parameter for the output
	XMLtag_set_value->deallocateXMLTag();
	XMLtag_set_value->ParseTagXML( fileDepth , curChar , true , chaine , ind );
	if( XMLtag_set_value->tagType != EmptyTag ) {
	  sprintf( ErrorStr , "Error: Expecting empty XML tag content element of set_schedule_attribute_value [%d]!" , XMLtag_set_value->tagType ); ReportError( ErrorStr ); throw 68;
	}
	if( strcmp( XMLtag_set_value->tag , "schedule" ) != 0 ) {
	  sprintf( ErrorStr , "Error: schedule XML tag expected in set_schedule_attribute_value action [%s]!" , XMLtag_set_value->tag ); ReportError( ErrorStr ) ; throw 136;
	  return false;
	}

	// parameter values in the action strings
	XMLtag_set_value->substituteParametersXML();

	// action closing tag
	ExpectClosingTagXML( fileDepth , curChar ,  XMLtag->tag , chaine , ind );
      }
      // error: non opening tag actions
      else {
	sprintf( ErrorStr , "Error: Expecting non empty XML tag for set_schedule_attribute_value action [%d]!" , XMLtag->tagType ); ReportError( ErrorStr ); throw 68;
      }
    }
    break;
  case set_material_attribute_value:
  case set_video_attribute_value:
    {
      // index: shader material property rank (shader_source_video)
      IndexStringValue( XMLtag );

      // operator
      ParseOperator( XMLtag );
      
      // these actions contain an empty tag that defines the 
      // values that are modified in the target node(s)
      if( XMLtag->tagType == OpeningTag ) {

	// the xml tags that contains the attribute and its value
	// that will serve as parameter for the output
	XMLtag_set_value->deallocateXMLTag();
	XMLtag_set_value->ParseTagXML( fileDepth , curChar , true , chaine , ind );
	if( XMLtag_set_value->tagType != EmptyTag ) {
	  sprintf( ErrorStr , "Error: Expecting empty XML tag content element of set_material_attribute_value, set_video_attribute_value or action [%d]!" , XMLtag_set_value->tagType ); ReportError( ErrorStr ); throw 68;
	}

	// parameter values in the action strings
	XMLtag_set_value->substituteParametersXML();

	// action closing tag
	ExpectClosingTagXML( fileDepth , curChar ,  XMLtag->tag , chaine , ind );
      }
      // error: non opening tag actions
      else {
	sprintf( ErrorStr , "Error: Expecting non empty XML tag for set_material_attribute_value, set_video_attribute_value [%d]!" , XMLtag->tagType ); ReportError( ErrorStr ); throw 68;
      }

      if( (actionID == set_material_attribute_value)
	  && strcmp( XMLtag_set_value->tag , "shader_source" ) != 0 
	  && strcmp( XMLtag_set_value->tag , "ambient" ) != 0
	  && strcmp( XMLtag_set_value->tag , "diffuse" ) != 0
	  && strcmp( XMLtag_set_value->tag , "specular" ) != 0
	  && strcmp( XMLtag_set_value->tag , "emission" ) != 0 
	  && strcmp( XMLtag_set_value->tag , "shininess" ) != 0 
	  && strcmp( XMLtag_set_value->tag , "color" ) != 0  
	  && strcmp( XMLtag_set_value->tag , "video" ) != 0
	  && strcmp( XMLtag_set_value->tag , "texture" ) != 0
	  && strcmp( XMLtag_set_value->tag , "shader_source_video" ) != 0 ) {
	sprintf( ErrorStr , "Error: burst, shader_source, color, texture, video, shader_source_video, or material XML tag expected in set_material_attribute_value action [%s]!" , XMLtag_set_value->tag ); ReportError( ErrorStr ) ; throw 136;
      }  
      else if( (actionID == set_video_attribute_value)
	       && strcmp( XMLtag_set_value->tag , "video" ) != 0   ) {
	sprintf( ErrorStr , "Error: video XML tag expected in set_video_attribute_value action [%s]!" , XMLtag_set_value->tag ); ReportError( ErrorStr ) ; throw 136;
      }  
    }
    break;

  case execute_system_command:
    strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    if( XMLtag->hasAttribute( "stdin" ) ) {
      char                scalarString[XMLAttrValLength];
      strcpy( scalarString , XMLtag->getAttributeStringValue( "stdin" ) );
      if( !sourceNode 
	  || !(targetScalarIn = sourceNode->getScalarFromName( scalarString ) ) ) {
	sprintf( ErrorStr , "Error: Unknown scalar [%s] on execute_system_command stdin of node [%s]!" , scalarString , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
      }
    }
    if( XMLtag->hasAttribute( "stdout" ) ) {
      if( targetScalarIn ) {
	sprintf( ErrorStr , "Error: Command execute_system_command cannot have both stdin and stdout!" ); ReportError( ErrorStr );
      }
      char                scalarString[XMLAttrValLength];
      strcpy( scalarString , XMLtag->getAttributeStringValue( "stdout" ) );
      if( !sourceNode 
	  || !(targetScalarOut = sourceNode->getScalarFromName( scalarString ) ) ) {
	sprintf( ErrorStr , "Error: Unknown scalar [%s] on execute_system_command stdout of node [%s]!" , scalarString , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
      }
    }
    break;
  case flush_delayed_actions:
    break;
  case set_internal_state:
  case write_log_file:
  case write_console:
  case write_message:
    if( XMLtag->hasAttribute( "index" ) ) {
      IndexIntValue( XMLtag );
    }
    else {
      targetIntValue = 4;
    }
    strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    break;
  case stepwise_interpolate_track:
    if( XMLtag->hasAttribute( "value" ) ) {
      strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    }
    else {
      strcpy( targetStringValue , "1" );
    }
    // variable ID if there's one
    *targetStringValueAux = 0;
    if( XMLtag->hasAttribute( "variable" ) ) {
      strcpy( targetStringValueAux , 
	      XMLtag->getAttributeStringValue( "variable" ) );
    }
    else {
      strcpy( targetStringValueAux , "false" );
    }
    break;
  case size_interpolate_track:
  case read_speed_scale_track:
    if( XMLtag->hasAttribute( "value" ) ) {
      strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    }
    else {
      strcpy( targetStringValue , "1" );
    }
    // variable ID if there's one
    *targetStringValueAux = 0;
    if( XMLtag->hasAttribute( "variable" ) ) {
      strcpy( targetStringValueAux , 
	      XMLtag->getAttributeStringValue( "variable" ) );
    }
    else {
      strcpy( targetStringValueAux , "0.0" );
    }
    break;
  case reset_interpolate_track:
  case start_record_source_track:
  case start_record_target_track:
  case start_interpolate_track:
  case start_read_source_track:
  case stop_record_source_track:
  case stop_record_target_track:
  case stop_interpolate_track:
  case stop_read_source_track:
    if( XMLtag->hasAttribute( "value" ) ) {
      strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    }
    else {
      strcpy( targetStringValue , "1" );
    }
    break;
  case load_svg_source_track:
  case load_svg_target_track:
    if( XMLtag->hasAttribute( "value" ) ) {
      strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    }
    else {
      strcpy( targetStringValue , "1" );
    }
    if( XMLtag->hasAttribute( "index" ) ) {
      IndexIntValue( XMLtag );
      targetIntValue++;   // tracks are from 0 (bg) to Nb_max_tracks + 1
      if( targetIntValue >=  (DM_EnvironmentNode->Nb_max_tracks + 1)
	  || targetIntValue < 0  ) {
	sprintf( ErrorStr , "Error: incorrect track number in load_svg_target_track/load_svg_source_track command [%d]!" , targetIntValue ); ReportError( ErrorStr ); 
      }
    }
    else {
      targetIntValue = 0;
    }
    break;
  case write_frame:
    // image rank
    if( XMLtag->hasAttribute( "index" ) ) {
      IndexIntValue( XMLtag );
      if( targetIntValue >= 1 ) {
	sprintf( ErrorStr , "Error: incorrect window number in write_frame command [%d]!" , targetIntValue + 1 ); ReportError( ErrorStr ); 
      }
    }
    else {
      targetIntValue = -1;
    }
    break;
  case screenshot:
    strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    if( XMLtag->hasAttribute( "type" ) ) {
      strcpy( targetStringValueAux , XMLtag->getAttributeStringValue( "type" ) );
      if( strcmp( targetStringValueAux , "svg" ) != 0 
	  && strcmp( targetStringValueAux , "png" ) != 0 
	  && strcmp( targetStringValueAux , "jpg" ) != 0 ) {
	sprintf( ErrorStr , "Error: incorrect snapshot file type [%s]!" , targetStringValueAux ); ReportError( ErrorStr ); 
	strcpy( targetStringValueAux , "jpg" );
      }
    }
    else {
      strcpy( targetStringValueAux , "jpg" );
    }
    break;
  case execute_message:
  case send_message:
  case send_message_udp:
    if( XMLtag->hasAttribute( "pattern" ) ) {
      strcpy( targetStringValueAux , XMLtag->getAttributeStringValue( "pattern" ) );
    }
    else {
      *targetStringValueAux = 0;
    }
    strcpy( targetStringValue , XMLtag->getAttributeStringValue( "value" ) );
    targetEventType = message_event;
    break;
  case clean_exit:
    break;
  case set_viewpoint:
  case set_current_viewpoint:
    // index: target rank
    if( XMLtag->hasAttribute( "index" ) ) {
      strcpy( targetStringValue , XMLtag->getAttributeStringValue( "index" ) );
    }
    else if( XMLtag->hasAttribute( "xlink:href" ) ) {
      // references are solved in a second pass
      targetData = new dm_SceneNode(ANodeLink);
      StrCpy( &(targetData->id) , 
	      XMLtag->getAttributeStringValue( "xlink:href" ) );
      targetIntValue = -1;

	  // set the targetData to the deletion list
	  targetData->sister = NULL;
	  targetData->daughter = deletedNodeList;
	  deletedNodeList  = targetData;

    }
    else {
      sprintf( ErrorStr , "Error: unknown target set_current_viewpoint (index orxlink:hrefattribute expected)!" ); ReportError( ErrorStr ) ; throw 289;
      return false;
    }
    break;
  case forward_message:
    // nothing to read here: these actions have no arguments
    strcpy( targetStringValue , sourceEventString );
    substituteParameters( targetStringValue ) ;
    targetEventType = sourceEvent;
    //printf( "sourceEventString %s sourceEvent %d\n" , sourceEventString , sourceEvent );
    break;
  case forward_message_udp:
    // nothing to read here: these actions have no arguments
    if( sourceNode ) {
      sprintf( targetStringValue , "send_message \"%s\" to \"#%s\"" , 
	       sourceEventString , NodeIDOrEmptyString( sourceNode ) );
    }
    else  {
      sprintf( targetStringValue , "send_message \"%s\"" , sourceEventString );
    }
    substituteParameters( targetStringValue ) ;
    targetEventType = sourceEvent;
    //printf( "targetStringValue %s\n" , targetStringValue );
    break;
  default:
    sprintf( ErrorStr , "Error: unknown script action [%d,%s]!" , actionID , BaseActionIDString[ actionID ] ); throw 136;
    return false;
  }
  return true;
}


void dm_BaseAction::execute( dm_SceneNode **targetNodes , int nbTargetNodes ,
			     dm_IPClient **targetHosts , int nbTargetHosts ,
			     bool withExecutionValue , float executionValue ) {
  char tempString[XMLAttrValLength];

  // fprintf( stdout , "base action %s %d\n" , BaseActionIDString[actionID] , nbTargetNodes );
  switch( actionID ) {
    // classical message: modifies the value of an attribute of a dm_Scene Object
    // the message is made of an attribute name and an attribute value
  case set_node_attribute_value:
    {

      // fprintf( stdout , "action %s\n" ,XMLtag_set_value->tag );

      NodeType targetNode_type = GetNodeType( XMLtag_set_value->tag );
      dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      // tempTag->copyAndSubstitueExpressions( XMLtag_set_value );

      // AJOUT
      // parameter values in the action strings
      // the value of the "value" attribute should not be pre-substituted
      // because it can be a table value and should not be
      // treated as an expression. Substitution will be made by the
      // setParameters command that follows
      // fprintf( stdout , "XMLtag_set_value:\n" );
      // XMLtag_set_value->print(stdout,1);
      char *params[5];
      params[0] = new char[10]; strcpy( params[0] , "matrix" );
      params[1] = new char[10]; strcpy( params[1] , "m__1" );
      params[2] = new char[10]; strcpy( params[2] , "m__2" );
      params[3] = new char[10]; strcpy( params[3] , "m__3" );
      params[4] = new char[10]; strcpy( params[4] , "m__4" );
      tempTag->copyAndSubstitueExpressionsbutNotTable( XMLtag_set_value , 
						       (char **)params , 5 );
      delete [] params[0];
      delete [] params[1];
      delete [] params[2];
      delete [] params[3];
      delete [] params[4];
 
      // fprintf( stdout , "tempTag:\n" );
      // tempTag->print(stdout,1);
      // AJOUT

      dm_SceneNode       *targetNode;
      //fprintf( stdout , "Nb targets %d\n" , nbTargetNodes );
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  
	  // fprintf( stdout , "Target %s %s %s\n" , targetNode->id , NodeTypeString[targetNode_type] , NodeTypeString[targetNode->type] );
	  if( targetNode_type == EmptyNode ) {
	    //fprintf( stdout , "Empty node set parameters\n" );
	    targetNode
	      ->setParameters( tempTag , 
			       true ,      // at run time
			       FloatOperators[ set_value_operator ] , 
			       IntOperators[ set_value_operator ] );
	    // invalidation is only necessary when the values of the
	    // scene object are modified 
	    // value modification such as displayed, interactable, draggable do not need
	    // invalidate that makes the display list be regenerated
	    // targetNode->invalidate();
	  }
	  // sceneObject attributes
	  else {
	    //fprintf( stdout , "Full node set parameters\n" );
	    if( targetNode->type != targetNode_type ) { 
	      sprintf( ErrorStr , "Error: <set_node_attribute_value> action target node (%s) type [%s] has not the expected type [%s]!" , targetNode->id , NodeTypeString[targetNode->type] , NodeTypeString[targetNode_type] ); ReportError( ErrorStr ) ; for( int indAux = 0 ; indAux < XMLtag_set_value->nbAttributes ; indAux++ ) {printf("attribute/value %s/%s\n", XMLtag_set_value->attributes[indAux] , XMLtag_set_value->values[indAux]);}/* targetNode->print(stdout,1,1);*/break; 
	    }
	    // 	    printf( "%s %s %s %s -> %s\n" , 
	    // 		    BaseActionIDString[ actionID ] ,
	    // 		    targetNode->id ,
	    // 		    tempTag->attributes[2] ,
	    // 		    tempTag->values[2] , XMLtag_set_value->tag );
	    //  	    printf( "targetNode %s motherInterpolatorNode %d\n" , targetNode->id , targetNode->motherInterpolatorNode );
	    dm_SceneObject *asceneObject;
	    asceneObject = targetNode->sceneObject;
	    asceneObject
	      ->setParameters( tempTag , 
			       true ,      // at run time
			       FloatOperators[ set_value_operator ] , 
			       IntOperators[ set_value_operator ] );
	    // printf( "Target node update %s\n" , targetNode->id );	    
	    targetNode->forceUpdate( true );
	  }
	}
      }
      delete tempTag;
      tempTag = NULL;
    }
    break;
  case set_node_scalar_value:
  case set_node_table_value:
  case set_node_matrix_value:
    {
      //fprintf( stdout , "action %s\n" , XMLtag_set_value->tag );

      dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      // the value of the "value" attribute should not be pre-substituted
      // because it can be a table value and should not be
      // treated as an expression. Substitution will be made by the
      // setParameters command that follows
      // fprintf( stdout , "XMLtag_set_value:\n" );
      // XMLtag_set_value->print(stdout,1);

      char *params[1];
      params[0] = new char[10];
      strcpy( params[0] , "value" );
      tempTag->copyAndSubstitueExpressionsbutNotTable( XMLtag_set_value , 
						       (char **)params , 1 );
      delete [] params[0];

      // fprintf( stdout , "tempTag:\n" );
      // tempTag->print(stdout,1);

      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      
      dm_SceneNode       *targetNode;

      //fprintf( stdout , "Nb targets %d\n" , nbTargetNodes );
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  if( actionID == set_node_scalar_value ) {
	    dm_Scalar *scal = targetNode->getScalarFromName( tempString );
	    if( scal ) {
	      // fprintf( stdout , "dm_Scalar found %s\n" , tempString );
	      scal->setParameters( tempTag , 
				   true ,      // at run time
				   FloatOperators[ set_value_operator ] , 
				   IntOperators[ set_value_operator ] );
	    }
	  }
	  else if( actionID == set_node_table_value ) {
	    dm_Table *tab = targetNode->getTableFromName( tempString );
	    if( tab ) {
	      // fprintf( stdout , "tempTag:\n" );
	      // tempTag->print(stdout,1);
	      tab->setParameters( tempTag , 
				  true ,      // at run time
				  FloatOperators[ set_value_operator ] , 
				  IntOperators[ set_value_operator ] );
	    }
	  }
	  else if( actionID == set_node_matrix_value ) {
	    dm_Matrix *mat = targetNode->getMatrixFromName( tempString );
	    if( mat ) {
	      mat->setParameters( tempTag , 
				  true ,      // at run time
				  FloatOperators[ set_value_operator ] , 
				  IntOperators[ set_value_operator ] );
	    }
	  }
	  targetNode->forceUpdate( true );
	}
      }
      delete tempTag;
      tempTag = NULL;
    }
    break;
  case set_environment_attribute_value:
    {
      dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      tempTag->copyAndSubstitueExpressions( XMLtag_set_value );

      dm_SceneNode       *targetNode;
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  // message for the root node: modification of the configuration
	  if( targetNode->type == AEnvironment ) {
	    ((dm_Environment *)targetNode->sceneObject)
	      ->setApplicationParameters( 
					 tempTag , 
					 true ,  // script action
					 FloatOperators[ set_value_operator ] , 
					 IntOperators[ set_value_operator ] );
	    targetNode->forceUpdate( true );
	  }
	}
      }
      delete tempTag;
      tempTag = NULL;
    }
    break;
  case set_user_attribute_value:
    {
      dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      tempTag->copyAndSubstitueExpressions( XMLtag_set_value );

      dm_SceneNode       *targetNode;
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  // message for the root node: modification of the configuration
	  dm_User *user = (dm_User *)targetNode->sceneObject;
	  ViewvolumeType  vv_type = user->viewvolume_type;
	  if( targetNode->type == AUser ) {
	    user->setApplicationParameters( 
				   tempTag , 
				   true ,  // script action
				   FloatOperators[ set_value_operator ] , 
				   IntOperators[ set_value_operator ] );
	    targetNode->forceUpdate( true );
	  }
	  ViewvolumeType  vv_new_type = user->viewvolume_type;
	  // DM_FRUSTUM = 0, DM_ORTHO
	  if( vv_type != vv_new_type ) {
	    // printf("Change of vv type %d -> %d\n" , vv_type , vv_new_type );
	    if( CurrentOrtho ) {
	      CurrentOrtho = NULL;
	      CurrentFrustum = CurrentUser->frustum;
	    }
	    else if( CurrentFrustum ) {
	      CurrentFrustum = NULL;
	      CurrentOrtho = CurrentUser->ortho;
	    }
	  }
	}
      }
      delete tempTag;
      tempTag = NULL;
    }
    break;
  case set_schedule_attribute_value:
    {
      dm_SceneNode       *targetNode = NULL;
      dm_Schedule        *targetSchedule = NULL;
      dm_XMLTag          *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      tempTag->copyAndSubstitueExpressions( XMLtag_set_value );

      // default schedule rank is 0
      if( *targetStringValue ) {
	targetIntValue = stringToIntValue( targetStringValue , true ) - 1;
      }
      else {
	targetIntValue = 0;
      }
      if( targetIntValue < 0 
	  || targetIntValue >= DM_EnvironmentNode->Nb_max_path_components ) {
	sprintf( ErrorStr , "Error: incorrect schedule rank [%s/%d]!" , targetStringValue , targetIntValue ); ReportError( ErrorStr ) ; 
	break;
      }

      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  // printf("String variable [%s]\n" , targetStringValueAux );

	  // variable schedule
	  if( *targetStringValueAux ) {
	    StrCpyAndSubst( tempString , targetStringValueAux , 
			    XMLAttrValLength );
	    // printf("String variable [%s]\n" , tempString );

	    dm_Scalar * scal 
	      = targetNode->getScalarFromName( tempString );
	    if( scal && scal->interp ) {
	      if( targetIntValue < 0 || 
		  targetIntValue >= scal->interp->nbSchedules ) {
		delete tempTag;
		tempTag = NULL;
		sprintf( ErrorStr , "Error: incorrect schedule rank [%d/%d]!" , targetIntValue + 1 , scal->interp->nbSchedules ); ReportError( ErrorStr ) ; break;
	      }
	      targetSchedule = scal->interp->schedules[targetIntValue];
	    }
	    
	    if( !targetSchedule ) {
	      dm_Table * tab 
		= targetNode->getTableFromName( tempString );
	      if( tab && tab->interp ) {
		if( targetIntValue < 0 || 
		    targetIntValue >= tab->interp->nbSchedules ) {
		  delete tempTag;
		  tempTag = NULL;
		  sprintf( ErrorStr , "Error: incorrect schedule rank [%d/%d]!" , targetIntValue + 1 , tab->interp->nbSchedules ); ReportError( ErrorStr ) ; break;
		}
		targetSchedule = tab->interp->schedules[targetIntValue];
	      }
	    }
	    
	    if( !targetSchedule ) {
	      dm_Matrix * mat 
		= targetNode->getMatrixFromName( tempString );
	      if( mat && mat->interp ) {
		if( targetIntValue < 0 || 
		    targetIntValue >= mat->interp->nbSchedules ) {
		  delete tempTag;
		  tempTag = NULL;
		  sprintf( ErrorStr , "Error: incorrect schedule rank [%d/%d]!" , targetIntValue + 1 , mat->interp->nbSchedules ); ReportError( ErrorStr ) ; break;
		}
		targetSchedule = mat->interp->schedules[targetIntValue];
	      }
	    }
	  }

	  if( !targetSchedule ) {
	    delete tempTag;
	    tempTag = NULL;
	    sprintf( ErrorStr , "Error: script expects schedule on node [%s]!" , targetNode->id ); ReportError( ErrorStr ) ; break;
	  }
	  
	  targetSchedule
	    ->setParameters( XMLtag_set_value->tag , 
			     1 ,           // size: only used at compile time
			     tempTag , 
			     true ,        // at run time
			     FloatOperators[ set_value_operator ] , 
			     IntOperators[ set_value_operator ] );
	  // bounding geometric data regenerated
	  targetNode->forceUpdate( true );
	  // printf( "set_schedule_attribute_value Current time %.2f Current schedules %d nbTargetNodes %d (%s)\n" , CurrentClockTime , ind , nbTargetNodes , targetNode->id );
	  // targetSchedule->print( stdout , 0 , 1 );
	}
      }
      if( tempTag ) {
	delete tempTag;
	tempTag = NULL;
      }
    }
    break;
  case set_material_attribute_value:
    {
      dm_SceneNode       *targetNode = NULL;
      // dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      // the values of the attribute are not pre-substituted
      // because it can be a table value and should not be
      // treated as an expression. Substitution will be made by the
      // setParameters command that follows
      // fprintf( stdout , "XMLtag_set_value:\n" );
      // XMLtag_set_value->print(stdout,1);

      // tempTag->copyAndSubstitueExpressions( XMLtag_set_value );
      // tempTag->print( stdout,1);
      
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  // target node is a geometrical object with material properties
	  if( targetNode->IsGeometricalObject() ) {
	    // 	      printf( "dm_BaseAction (%d/%d) %s %s %s %s -> %s %d\n" , 
	    // 		      ind , nbTargetNodes ,
	    // 		      BaseActionIDString[ actionID ] ,
	    // 		      XMLtag_set_value->attribute ,
	    // 		      OperatorString[ set_value_operator ] ,
	    // 		      XMLtag_set_value->value , XMLtag_set_value->tag ,
	    // 		      targetIntValue + 1 );
	    if( strcmp( XMLtag_set_value->tag , "shader_source" ) == 0 ) {
	      dm_Shader * targetShader = targetNode->GetMaterialShader();
	      if( targetShader && targetShader->shaderSource ) {
		targetShader->shaderSource
		  ->setParameters( XMLtag_set_value , 
				   true ,        // at run time
				   FloatOperators[ set_value_operator ] , 
				   IntOperators[ set_value_operator ] );
		// shader update don't require any recompiling
		// since shaders are outside display lists
		// 	    targetShader->shaderSource->print( stdout , 1, 1 );
		// 	    printf("At time %.2f\n\n\n" , CurrentClockTime );
	      }
	    }
	    else if( strcmp( XMLtag_set_value->tag , "ambient" ) == 0
		     || strcmp( XMLtag_set_value->tag , "diffuse" ) == 0
		     || strcmp( XMLtag_set_value->tag , "specular" ) == 0
		     || strcmp( XMLtag_set_value->tag , "emission" ) == 0 
		     || strcmp( XMLtag_set_value->tag , "shininess" ) == 0 
		     || strcmp( XMLtag_set_value->tag , "color" ) == 0  ) {
	      dm_MaterialProperties *mat = targetNode->GetMaterial();
	      if( mat ) {
		mat->setParameters( XMLtag_set_value ,
				    true ,        // at run time
				    FloatOperators[ set_value_operator ] , 
				    IntOperators[ set_value_operator ] );
		targetNode->forceUpdate( true );
	      }
	    }
	    else if( strcmp( XMLtag_set_value->tag , "texture" ) == 0  ) {
	      dm_MaterialProperties *mat = targetNode->GetMaterial();
	      if( mat && mat->materialTexture ) {
		char textureName[XMLAttrValLength];
		
		// texture name  
		*textureName = '\0';
		if( XMLtag_set_value->hasAttribute( "xlink:href" ) ) {
		  char *fileName 
		    = XMLtag_set_value->getAttributeStringValue( "xlink:href" );
		  // strcpy( textureName , fileName );
		  StrCpyAndSubst( textureName , fileName , XMLAttrValLength );
		  
		}

		mat->materialTexture
		  ->setParameters( XMLtag_set_value ,
						     true ,        // at run time
						     false ,        // video texture
						     FloatOperators[ set_value_operator ] , 
						     IntOperators[ set_value_operator ] );

		if( *textureName ) {
		  mat->materialTexture->LoadOrGenerateTexture( textureName , 
							       NULL , 
							       NULL , (char *)"" , 
							       XMLtag_set_value ,
							       true );
		}

		
		targetNode->forceUpdate( true );
	      }
	    }
	    else if( strcmp( XMLtag_set_value->tag , "video" ) == 0  ) {
#ifdef DM_HAVE_FFMPEG
	      dm_Video *video = targetNode->GetVideo();
	      if( video ) {
		bool modified_geometry = false;
		dm_MaterialProperties *mat = targetNode->GetMaterial();
		// video update can involve texture parameter modification
		// such as tile_s, offset_s that can modify the geometry
		// and require DL recompiling
		if( mat && mat->materialTexture ) {
		  modified_geometry = mat->materialTexture
		    ->setParameters( XMLtag_set_value ,
				     true ,        // at run time
				     true ,        // video texture
				     FloatOperators[ set_value_operator ] , 
				     IntOperators[ set_value_operator ] );
		}
		video->setParameters( XMLtag_set_value ,
				      true ,        // at run time
				      false , // video is already known
				      // currently cannot involve video reload
				      FloatOperators[ set_value_operator ] , 
				      IntOperators[ set_value_operator ] );
		
		targetNode->forceUpdate( true );
		// video->print(stdout,1,1);
	      }
#endif
	    }
	    else if( strcmp( XMLtag_set_value->tag , "shader_source_video" ) == 0  ) {
#ifdef DM_HAVE_FFMPEG
	      targetIntValue = stringToIntValue( targetStringValue , true ) - 1;
	      dm_Shader * targetShader = targetNode->GetMaterialShader();
	      if( targetShader && targetShader->shaderSource ) {
		dm_Video *video 
		  = targetShader->shaderSource->materialVideo[ targetIntValue ];
		if( video ) {
		  video->setParameters( XMLtag_set_value ,
					true ,        // at run time
					false , // video is already known
					// currently cannot involve video reload
					FloatOperators[ set_value_operator ] , 
					IntOperators[ set_value_operator ] );
		  // shader update don't require any recompiling
		  // since shaders are outside display lists
		  // video->print(stdout,1,1);
		}
	      }
#endif
	    }
	    else  {
	      // delete tempTag;
	      // tempTag = NULL;

	      sprintf( ErrorStr , "Error: burst, shader_source, texture, video, shader_source_video, or material XML tag expected in set_material_attribute_value action [%s]!" , XMLtag_set_value->tag ); ReportError( ErrorStr ) ; break;
	    }  
	  }
	}
      }
      // delete tempTag;
      // tempTag = NULL;
    }
    break;


  case set_video_attribute_value:
    {
      dm_SceneNode       *targetNode = NULL;
      dm_XMLTag   *tempTag = new dm_XMLTag();

      // parameter values in the action strings
      tempTag->copyAndSubstitueExpressions( XMLtag_set_value );
      
      // printf("nb target nodes %d\n",nbTargetNodes );
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  // target node is a geometrical object with material properties
	  // printf(" target node %dI sGeometricalObject  %d\n",ind,targetNode->IsGeometricalObject()  );
	  if( targetNode->IsGeometricalObject() ) {
	    // 	      printf( "dm_BaseAction (%d/%d) %s %s %s %s -> %s %d\n" , 
	    // 		      ind , nbTargetNodes ,
	    // 		      BaseActionIDString[ actionID ] ,
	    // 		      XMLtag_set_value->attribute ,
	    // 		      OperatorString[ set_value_operator ] ,
	    // 		      XMLtag_set_value->value , XMLtag_set_value->tag ,
	    // 		      targetIntValue + 1 );
	    if( actionID == set_video_attribute_value ) {
#ifdef DM_HAVE_FFMPEG
	      dm_Video *video = targetNode->GetVideo();
	      if( video ) {
		video->setParameters( tempTag ,
				      true ,        // at run time
				      false , // video is already known
				      // currently cannot involve video reload
				      FloatOperators[ set_value_operator ] , 
				      IntOperators[ set_value_operator ] );
		targetNode->forceUpdate( true );
		// video->print(stdout,1,1);
	      }
#endif
	    }
	  }
	}
      }
      delete tempTag;
      tempTag = NULL;
    }
    break;

  case set_internal_state:
    {
      dm_SceneNode       *targetNode = NULL;
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      //printf( "set_internal_state tempString [%s] nbTargetNodes %d\n" , tempString , nbTargetNodes );
      
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) ) {
	  //printf( "set_internal_state targetNode [%s]\n" , targetNode->id );
	  StrCpy( &(targetNode->internalState) , tempString );
	}
      }
    }
    break;
      
  case execute_message:
    // printf("execute_message nbTargetNodes %d\n" , nbTargetNodes  );
    if( nbTargetNodes > 0 ) {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	dm_SceneNode   *targetNode = targetNodes[ind];
	if( targetNode->script ) {
	  // printf("message received by target: [%s] [%s]\n" , tempString , targetNode->id  );
	  targetNode->script->receive( targetEventType , tempString , targetNode );
	  
	}
      }
    }
    break;
  case send_message:
  case forward_message:
    if( nbTargetNodes > 0 ) {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      DM_CompositionNode->pushEvent( targetEventType , tempString , 
				     targetNodes , nbTargetNodes );
      // printf( "push event: time %.2f frame %d [%s] ,nbTargetNodes %d\n" , CurrentClockTime , FrameNo , tempString , nbTargetNodes );
    }
    break;

  case set_viewpoint:
    {
      dm_SceneNode       *targetNode = NULL;
      
      // dynamic reference
      if( targetDataStatus == DynamicNodeElement ) {
	int nbNodes = 0;
	dm_SceneNode ** auxNode = NULL;

	StrCpyAndSubst( tempString , targetData->id , 
			XMLAttrValLength );
	auxNode = findReferencesDynamic( tempString , &nbNodes ,
					 true , true );

	// printf("Nb discovered %s (%s) Nodes %d\n" , tempString , targetData->id , nbNodes );
	
	if( auxNode ) {
	  if( *(auxNode) ) {
	    for( int ind = 0 ; ind < DM_CompositionNode->nbMaxViewpoints ; 
		 ind++ ) {
	      if( ((dm_Viewpoint *)((*(auxNode))->sceneObject)) 
		  == DM_CompositionNode->viewpoints[ind] ) {
		targetIntValue = ind;
		break;
	      }
	    }
	  }
	  if( targetIntValue < 0 ) {
	    sprintf( ErrorStr , "Error: unknown viewpoint [%s]!" , targetData->id ); ReportError( ErrorStr ) ; 
	  }
	  delete [] auxNode;
	  auxNode = NULL;
	  // printf( "Target viewpoint %d\n" , targetIntValue );
	  // unknown viewpoint -> default viewpoint
	}
      }
      // dynamic reference

      // viewpoint number given by index attribute
     else if( *targetStringValue ) {
	targetIntValue = stringToIntValue( targetStringValue , true ) + 1;
      }

      for( int ind = 0 ; ind < nbTargetNodes ; ind++ ) {
	if( (targetNode = targetNodes[ind]) 
	    && targetIntValue >=0 ) {
	  // target node is an interpolator
	  if( targetNode->type == AUser ) {
	    dm_User * user = ((dm_User *)(targetNode->sceneObject));
	    user->manualViewpointControl = true;
	    user->indCurrentViewpoint = targetIntValue % DM_CompositionNode->nbRegisteredViewpoints;
	    // 	    if( user->indCurrentViewpoint == 0 ) {
	    // 	      // the case of navigation viewpoint
	    // 	      // stops ongoing automatic motion
	    // 	      user->manualMoveLinearStep = 0;
	    // 	    }
	    // printf( "Current viewpoint %d user %s\n" , user->indCurrentViewpoint , user->GetId() );
	    targetNode->invalidate();
	  }
	}
      }
    }
    break;
      
  case set_current_viewpoint:
    
    // viewpoint number given by index attribute
    if( *targetStringValue ) {
      targetIntValue = stringToIntValue( targetStringValue , true ) + 1;
    }
    else {
      targetIntValue = -1;
    }

    if( targetIntValue >=0 ) {
      if( CurrentUser ) {
	CurrentUser->manualViewpointControl = true;
	CurrentUser->indCurrentViewpoint 
	  = targetIntValue % DM_CompositionNode->nbRegisteredViewpoints;
	// 	if( CurrentUser->indCurrentViewpoint == 0 ) {
	// 	  // the case of navigation viewpoint
	// 	  // stops ongoing automatic motion
	// 	  CurrentUser->manualMoveLinearStep = 0;
	// 	}
	// printf( "Current viewpoint %d\n" , CurrentUser->indCurrentViewpoint );
      }
      // no current user: all the users are set to the
      // current viewpoint
      else {
	for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; ind++ ) {
	  dm_User * curUser = DM_CompositionNode->tabUsers[ ind ]->GetUser();
	  curUser->indCurrentViewpoint 
	    = targetIntValue % DM_CompositionNode->nbRegisteredViewpoints;
	}
	printf( "Initial viewpoint %d\n" , 
		targetIntValue % DM_CompositionNode->nbRegisteredViewpoints );
      }
    }
    break; 

  case execute_system_command:
    StrCpyAndSubst( tempString , targetStringValue , 
		    XMLAttrValLength );
    if( targetScalarOut ) {
      FILE *pipeOut;
      int charOut;
      char message[XMLAttrValLength];
      fflush( NULL );
#ifdef _WIN32
      pipeOut = _popen( tempString , "r" );
#else
      pipeOut = popen( tempString , "r" );
#endif
      int nbChar = 0;
      while( (charOut = fgetc( pipeOut ) ) != EOF 
	     && nbChar < XMLAttrValLength - 1 ) {
	message[ nbChar ] = charOut;
	nbChar++;
      }
#ifdef _WIN32
      _pclose( pipeOut );
#else
      pclose( pipeOut );
#endif
      message[ nbChar ] = 0;
      targetScalarOut->SetValue( message );
    }      
    else if( targetScalarIn ) {
      FILE *pipeIn;
      fflush( NULL );
#ifdef _WIN32
      pipeIn = _popen( tempString , "w" );
#else
      pipeIn = popen( tempString , "w" );
#endif
      fprintf( pipeIn , "%s" , targetScalarIn->GetStringValue() );
#ifdef _WIN32
      _pclose( pipeIn );
#else
      pclose( pipeIn );
#endif
    }
    else {
      if( withExecutionValue && executionValue ) {
	char message[XMLAttrValLength];
	sprintf( message , "%s %.2f" , tempString , executionValue );
	if( !system( message ) ) {
	  // sprintf( ErrorStr , "Error in system command %s!" , message ); ReportError( ErrorStr );
	}
      }
      else {
	if( !system( tempString ) ) {
	  // sprintf( ErrorStr , "Error in system command %s!" , tempString ); ReportError( ErrorStr );
	}
      }
    }
    break;
  case send_message_udp:
  case forward_message_udp:
    // printf( "send_message_udp %s %.2f %d\n" , targetStringValue , executionValue , nbTargetHosts );
    if( (actionID == send_message_udp 
	 || actionID == forward_message_udp) 
	&& nbTargetHosts > 0 ){
      StrCpyAndSubst( tempString , targetStringValue , XMLAttrValLength );
      for( int ind = 0 ; ind < nbTargetHosts ; ind++ ) {
	if( withExecutionValue && executionValue ) {
	  char message[XMLAttrValLength];
	  sprintf( message , "%s %.2f\n" , tempString , executionValue );
	  if( actionID == send_message_udp
	      && targetStringValue ) {
	    if( *targetStringValueAux ) {
	      targetHosts[ ind ]->storeIP_output_message( message ,
							  targetStringValueAux );
	    }
	    else {
	      targetHosts[ ind ]->storeIP_output_message( message ,
							  NULL );
	    }
	  }
	  else if( actionID == forward_message_udp ) {
	    targetHosts[ ind ]->storeIP_output_message( message , NULL );
	  }
	}
	else {
	  if( actionID == send_message_udp
	      && targetStringValue ) {
	    if( *targetStringValueAux ) {
	      targetHosts[ ind ]->storeIP_output_message( tempString ,
							  targetStringValueAux );
	    }
	    else {
	      targetHosts[ ind ]->storeIP_output_message( tempString ,
							  NULL );
	    }
	  }
	  else if( actionID == forward_message_udp ) {
	    targetHosts[ ind ]->storeIP_output_message( tempString , NULL );
	  }
	}
      }
    }
    break;
  case write_log_file:
    {
      StrCpyAndSubst( tempString , targetStringValue , XMLAttrValLength );
      if( DM_EnvironmentNode->log_date_output == false ) {
	fprintf( fileLog , "%.3f\t%s\n" , RealTime() , 
		 tempString  );
      }
      else {
	time_t t = time (NULL);
	char ch[128];
	strcpy( ch , asctime(localtime(&t)) );
	ch[ strlen(ch) - 1 ] = 0;
	fprintf( fileLog , "%s\t%.3f\t%s\n" , ch , RealTime() , 
		 tempString  );
      }
      //printf( "%ld\t%s\n" , t , targetStringValue  );
    }
    break;
  case flush_delayed_actions:
    {
      DM_CompositionNode->flushDelayedEvents();
    }
    break;
  case screenshot:
    {
      if( strcmp( targetStringValueAux , "svg" ) == 0 ) {
	draw_scene( _Svg );
      }
      else if( strcmp( targetStringValueAux , "png" ) == 0 ) {
	draw_scene( _Png );
      }
      else if( strcmp( targetStringValueAux , "jpg" ) == 0 ) {
	draw_scene( _Jpg );
      }
      else {
	sprintf( ErrorStr , "Incorrect screenshot type (%s): expected svg or png or jpg!" , targetStringValueAux ); ReportError( ErrorStr );
      }
    }
  case write_console:
    {
      //printf( "Expression before substitution: [%s]\n" , targetStringValue );
      StrCpyAndSubst( tempString , targetStringValue , XMLAttrValLength );
      //printf( "Expression after substitution: [%s]\n" , tempString );
      printf( "%.3f\t%s\n" , RealTime() , tempString  );
    }
    break;
  case write_frame:
    {
      WriteCurrentFrame = true;
      WriteCurrentFrameIndex = targetIntValue;
    }
    break;
  case write_message:
    {
      // printf( "Expression before substitution: [%s]\n" , targetStringValue );
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength , targetIntValue + 1 );
      // printf( "Expression after substitution: [%s]\n" , tempString );
      strncpy( ScreenMessage , tempString , XMLAttrValLength );
      double newtime = RealTime();

      ScreenMessageColor = 1.0;
      LastDecayTime = newtime;
      NewScreenMessage = true;
      // ScreenMessageY = 352;
    }
    break;
  case stepwise_interpolate_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      // continuous interpolation
      // or interpolation after each loop
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1)) {
	if( strcmp( targetStringValueAux , "true" ) == 0 ) {
	  TrackStatus[ indTrack ].isStepwiseInterpolation = true;
	}
	else if( strcmp( targetStringValueAux , "false" ) == 0 ) {
	  TrackStatus[ indTrack ].isStepwiseInterpolation = false;
	}
	else {
	  sprintf( ErrorStr , "Error: unknown boolean in stepwise_interpolate_track command [%s]!" , targetStringValueAux ); ReportError( ErrorStr );
	}
      }
    }
    break;
  case read_speed_scale_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus[ indTrack ].readSpeedScale = 
	  (float)stringToDoubleValue( targetStringValueAux , true );
      }
    }
    break;
  case size_interpolate_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	float nbTotFrameInterpolation =
	  ((float)stringToDoubleValue( targetStringValueAux , true )
	   * TrackStatus[ indTrack ].nbRecordedFrames);
	if( nbTotFrameInterpolation > 0 ) {
	  TrackStatus[ indTrack ].interpolationStep 
	    = 1.0f /nbTotFrameInterpolation;
	}
	else {
	  TrackStatus[ indTrack ].interpolationStep 
	    = 0.0001f;
	}
      }
    }
    break;
  case reset_interpolate_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus[ indTrack ].coefInterpolation = 0.0;
	TrackStatus[ indTrack ].indReading = 0;
      }
    }
    break;
  case start_record_source_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus[ indTrack ].isActiveRecording = true;
	TrackStatus[ indTrack ].nbRecordedFrames = 0;
	TrackStatus_target[ indTrack ].isNormalized = false;
	// printf("start recording track %d\n",indTrack);
      }
    }
    break;
  case start_record_target_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus_target[ indTrack ].isActiveRecording = true;
	TrackStatus_target[ indTrack ].nbRecordedFrames = 0;
	TrackStatus_target[ indTrack ].isNormalized = false;
      }
    }
    break;
  case start_interpolate_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus[ indTrack ].isActiveRecording = false;
	TrackStatus_target[ indTrack ].isActiveRecording = false;
	TrackStatus[ indTrack ].isActiveInterpolation = true;
	//TrackStatus[ indTrack ].coefInterpolation = 0;
	
	NormalizeTargetTrack( indTrack );
      }
    }
    break;
  case stop_interpolate_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	TrackStatus[ indTrack ].isActiveRecording = false;
	TrackStatus_target[ indTrack ].isActiveRecording = false;
	TrackStatus[ indTrack ].isActiveInterpolation = false;
	// TrackStatus[ indTrack ].coefInterpolation = 0;
      }
    }
    break;
  case start_read_source_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) 
	  && DM_EnvironmentNode->tracks_replay_table
	  ->GetValue( indTrack )->valFloat != 1.0F ) {
	DM_EnvironmentNode->tracks_replay_table
	  ->SetValue( 1.0F , indTrack );
	TrackStatus[ indTrack ].indReading = 0;
	TrackStatus[ indTrack ].isFirstFrame = true;
	// printf( "-> start_read_source_track\n"  );
      }
    }
    break;
  case stop_record_source_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) 
	  && TrackStatus[ indTrack ].isActiveRecording == true ) {
	TrackStatus[ indTrack ].isActiveRecording = false;
	// printf("start playing %d\n",indTrack);
	
	NormalizeTargetTrack( indTrack );
      }
    }
    break;
  case stop_record_target_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) 
	  && TrackStatus_target[ indTrack ].isActiveRecording == true ) {
	TrackStatus_target[ indTrack ].isActiveRecording = false;
	
	NormalizeTargetTrack( indTrack );
      }
    }
    break;
  case stop_read_source_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = stringToIntValue( tempString , true );
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) 
	  && DM_EnvironmentNode->
	  tracks_replay_table->GetValue( indTrack )->valFloat == 1.0F ) {
	DM_EnvironmentNode->tracks_replay_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_x_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_y_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_x_prev_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_y_prev_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_r_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_g_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_b_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_a_table->SetValue( 1.0F , indTrack );
	DM_EnvironmentNode->tracks_BrushID_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Radius_table->SetValue( 0.0F , indTrack );
	// printf( "-> stop_read_source_track\n"  );
      }
    }
    break;
  case load_svg_source_track:
  case load_svg_target_track:
    {
      StrCpyAndSubst( tempString , targetStringValue , 
		      XMLAttrValLength );
      int indTrack = targetIntValue;
      if( indTrack >= 0 
	  && indTrack < (DM_EnvironmentNode->Nb_max_tracks + 1) ) {
	DM_EnvironmentNode->tracks_replay_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_x_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_y_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_x_prev_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_y_prev_table->SetValue( -1.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_r_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_g_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_b_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Color_a_table->SetValue( 1.0F , indTrack );
	DM_EnvironmentNode->tracks_BrushID_table->SetValue( 0.0F , indTrack );
	DM_EnvironmentNode->tracks_Radius_table->SetValue( 0.0F , indTrack );
	// printf( "-> stop_read_source_track\n"  );

	// loads track
	int indDepth = 0;
	if( actionID == load_svg_source_track )
	  readsvg( &indDepth , indTrack , tempString , true );
	else
	  readsvg( &indDepth , indTrack , tempString , false );
      }
    }
    break;
  case clean_exit:
    {
      terminates_and_exits();
    }
    break;
  default:
    sprintf( ErrorStr , "Error: non implemented script action [%s,%d]!" , BaseActionIDString[actionID] , actionID ); ReportError( ErrorStr ) ; break;
  }
}

void dm_BaseAction::deleteReferenceTablesWithGraph(dm_SceneNode* rootTargetGraph)
{
  if (!rootTargetGraph)  {
      return;
    }

	if (rootTargetGraph) 
	{
		deleteReferenceTables(rootTargetGraph);		
	}
}

void dm_BaseAction::updateReferenceTablesWithGraph(dm_SceneNode* rootTargetGraph)
{
    if (!rootTargetGraph)  {
      return;
    }
	
	if (rootTargetGraph->daughter)
	{
		updateReferenceTablesWithGraph(rootTargetGraph->daughter);

	}
	
	if (rootTargetGraph->sister)
	{
		updateReferenceTablesWithGraph(rootTargetGraph->sister);
	}

	if (rootTargetGraph) 
	{
		updateReferenceTables(rootTargetGraph);		
	}
}

void dm_BaseAction::deleteReferenceTables( dm_SceneNode* nodeTarget)
{
	if (!nodeTarget)  {
      return;
    }

	// viewpoint
	if (nodeTarget->type == AViewpoint) {
		nodeTarget->deleteRegisterViewpoints(
					     &(DM_CompositionNode->nbRegisteredViewpoints),
					     DM_CompositionNode->nbMaxViewpoints,
					     DM_CompositionNode->viewpoints,
					     DM_CompositionNode->viewpointsNodes);
		
    }
	
	// user
	if (nodeTarget->type == AUser)  {
		nodeTarget->deleteRegisterUsers(
					&(DM_CompositionNode->nbRegisteredUsers),
					DM_CompositionNode->nbMaxUsers,
					DM_CompositionNode->tabUsers);
    }
	
	// unrecoginized node, throw 68
	if ((nodeTarget->type >=EmptyNode) ) {
		sprintf( ErrorStr , "Error: unrecognized node type [%s] to be added! Unexpected end of file!" ,  
			NodeTypeString[nodeTarget->type] ); 
		ReportError( ErrorStr ); throw 68;
	}
}

void dm_BaseAction::updateReferenceTables( dm_SceneNode* nodeTarget)
{
	if (!nodeTarget)  {
      return;
    }

	// viewpoint
	if (nodeTarget->type == AViewpoint) {
		nodeTarget->RegisterViewpoints(
					     &(DM_CompositionNode->nbRegisteredViewpoints),
					     DM_CompositionNode->nbMaxViewpoints,
					     DM_CompositionNode->viewpoints,
					     DM_CompositionNode->viewpointsNodes);
		
    }
	
	// user
	if (nodeTarget->type == AUser)  {
		nodeTarget->RegisterUsers(
					&(DM_CompositionNode->nbRegisteredUsers),
					DM_CompositionNode->nbMaxUsers,
					DM_CompositionNode->tabUsers);
    }
	
	// unrecoginized node, throw 68
	if ((nodeTarget->type >=EmptyNode)) {
		sprintf( ErrorStr , "Error: unrecognized node type [%s] to be added! Unexpected end of file!" ,  
			NodeTypeString[nodeTarget->type] ); 
		ReportError( ErrorStr ); throw 68;
	}
}

void dm_BaseAction::updateUser()
{
	// users  check
	// verifies that the default viewpoint exists
	for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; ind++ )  {
		dm_User * curUser =  DM_CompositionNode->tabUsers[ ind ]->GetUser();
		
		// default viewpoint given by an URI
		if( curUser->default_viewpoint_node ) 
		{
			for( int ind = 0 ; ind < DM_CompositionNode->nbMaxViewpoints ; ind++ ) 
			{
				if( ((dm_Viewpoint *)(curUser->default_viewpoint_node->sceneObject)) 
					== DM_CompositionNode->viewpoints[ind] )
				{
					curUser->default_viewpoint = ind;
					break;
				}
			}
			
			if( curUser->default_viewpoint < 0 ) 
			{
				sprintf( ErrorStr , "Error: unknown user default viewpoint [%s]!" ,
					curUser->default_viewpoint_node->id ); ReportError( ErrorStr ) ;  throw 289;
			}
			
			printf( "Default viewpoint %d\n" , curUser->default_viewpoint );
		}
		
		if( curUser->default_viewpoint_stereo_left_node ) 
		{
			for( int ind = 0 ; ind < DM_CompositionNode->nbMaxViewpoints ; ind++ ) 
			{
				if( ((dm_Viewpoint *)(curUser->default_viewpoint_stereo_left_node->sceneObject)) 
					== DM_CompositionNode->viewpoints[ind] ) 
				{
					curUser->default_viewpoint_stereo_left = ind;
					break;
				}
			}
			
			if( curUser->default_viewpoint_stereo_left < 0 ) 
			{
				sprintf( ErrorStr , "Error: unknown user default stereo left viewpoint [%s]!" , 
					curUser->default_viewpoint_stereo_left_node->id ); ReportError( ErrorStr ) ;  throw 289;
			}		
	  
			printf( "Default stereo left viewpoint %d\n" , curUser->default_viewpoint_stereo_left );
		}
		
		if( curUser->default_viewpoint > DM_CompositionNode->nbRegisteredViewpoints ) 
		{
			sprintf( ErrorStr , "Default viewpoint #%d of user %s should be lower than %d!" ,
				curUser->default_viewpoint , curUser->GetId() , DM_CompositionNode->nbRegisteredViewpoints - 1 ); ReportError( ErrorStr ); throw 246;
		}	
     
		if( curUser->alternate_stereo_viewer
			&& curUser->default_viewpoint_stereo_left >= DM_CompositionNode->nbRegisteredViewpoints )
		{
			sprintf( ErrorStr , "Default viewpoint stereo left #%d of user %s should be lower than or equal to %d!" , 
				curUser->default_viewpoint_stereo_left , curUser->GetId() , DM_CompositionNode->nbRegisteredViewpoints - 1 ); ReportError( ErrorStr ); throw 246;
		}
	}     

	// ------------------ user-based node visibility ---------------------- //
	// nodes can be hidden to some users
	DM_CompositionNode->RegisterUserExclusionDisplay();
}

/*
*/
void dm_BaseAction::updateCommandScripts(char* chrTargetID )
{
  //	printf("Target node %s type %s\n" , targetNode->id , NodeTypeString[targetNode->type] );
  //	printf("Added node %s type %s\n" , targetData->id , NodeTypeString[targetData->type] );

  ///// --------- added by Xianyong Fang --------- //////
  // update script reference tables
	
  // update the reference table
  for (int ind=0; ind < DM_CompositionNode->NbDrawnScenes; ind++)
  {
      // script references are not resolved at
      // the preceding pass because they require
      // pointer sharing and percolateInterpolation
      // makes copies of the nodes
      // printf( "findAllScriptReferences\n" );
      //curScene->print( stdout , 0 );

      // update references used in the script without keystroke
      DM_CompositionNode->drawn_scenes[ind]->findAllScriptReferences(chrTargetID);
		
      // update the key stroke related scripts
      // initialization script
      dm_Command* tempCommand = DM_CompositionNode->globalScript->commandList;
      while (tempCommand != NULL) 
	{
	  tempCommand->findScriptReferences(DM_CompositionNode->drawn_scenes[ind]->root, chrTargetID);

	  tempCommand = tempCommand->nextCommand;
	}

      if( DM_CompositionNode->initializationCommand ) {
	DM_CompositionNode->initializationCommand->findScriptReferences( 
									DM_CompositionNode->drawn_scenes[ind]->root , chrTargetID );
	dm_Command *synchronous = DM_CompositionNode->initializationCommand->synchronousCommand;
			
	while( synchronous ) {
	  synchronous->findScriptReferences( DM_CompositionNode->drawn_scenes[ind]->root, chrTargetID  );
	  synchronous = synchronous->synchronousCommand;
	}
      }

      // termination script
      if( DM_CompositionNode->terminationCommand ) {
	DM_CompositionNode->terminationCommand->findScriptReferences( DM_CompositionNode->drawn_scenes[ind]->root, chrTargetID  );
	dm_Command *synchronous = DM_CompositionNode->terminationCommand->synchronousCommand;
	while( synchronous ) {
	  synchronous->findScriptReferences( DM_CompositionNode->drawn_scenes[ind]->root , chrTargetID );
	  synchronous = synchronous->synchronousCommand;
	}
      }

      // alias script
      if( DM_CompositionNode->aliasCommand ) 
	{
	  dm_Command *curCommand = DM_CompositionNode->aliasCommand;
	  dm_Command *synchronous = NULL;
			
	  while( curCommand ) {
	    curCommand->findScriptReferences( DM_CompositionNode->drawn_scenes[ind]->root , chrTargetID );
	    synchronous = curCommand->synchronousCommand;

	    while( synchronous ) {
	      synchronous->findScriptReferences( DM_CompositionNode->drawn_scenes[ind]->root, chrTargetID  );
	      synchronous = synchronous->synchronousCommand;
	    }
				
	    curCommand = curCommand->nextCommand;
	  }
	}
  }
}

void dm_BaseAction::updateDisplayWindows( ) {	 
  // ------------------ user/window association ---------------------- //
  // association between windows and users
  if( !( *(DM_EnvironmentNode->DM_Window->user_id) ) ) 
    {
      // kept for backward compatibility
      printf( "Deprecated: window [%s] not associated with user_id -> associated with default user!\n" , 
	      DM_EnvironmentNode->DM_Window->id );
      DM_EnvironmentNode->DM_Window->userRank = 0;
    }
  else
    if( strcmp( DM_EnvironmentNode->DM_Window->user_id , "NULL" ) == 0 ) 
      {
	// NULL user: content window that is not displayed
	// printf( "indWin %d no user_id\n" , indWin ); 
	DM_EnvironmentNode->DM_Window->userRank = -1;
      }
    else
      {
	// retrieves user node from user ID
	dm_SceneNode * associatedUser 
	  = DM_CompositionNode->FindUserReference( DM_EnvironmentNode->DM_Window->user_id );
	
	if( associatedUser ) 
	  {
	    DM_EnvironmentNode->DM_Window->userRank = -1;
	    for( int ind = 0 ; ind < DM_CompositionNode->nbRegisteredUsers ; ind++ ) 
	      {
		if( DM_CompositionNode->tabUsers[ ind ] == associatedUser )
		  {
		    DM_EnvironmentNode->DM_Window->userRank = ind;
		    break;
		  }
	      }
	  }
	else
	  {
	    sprintf( ErrorStr , "Error: window [%s] associated with unknown user [%s]!" ,
		     DM_EnvironmentNode->DM_Window->id , 
		     DM_EnvironmentNode->DM_Window->user_id ); ReportError( ErrorStr ); throw 83;
	  }
      }
}

void dm_BaseAction::operator=(dm_BaseAction& c) {
  targetEventType = c.targetEventType;

  set_value_operator = c.set_value_operator;

  targetData = c.targetData;
  isTargetReleasedAutomatically = c.isTargetReleasedAutomatically;
  targetIntValue = c.targetIntValue;
  targetIntValue2 = c.targetIntValue2;
  strcpy( targetStringValue , c.targetStringValue );
  targetFloatValue = c.targetFloatValue;
  targetTabFloatValue = c.targetTabFloatValue;
  targetScalarIn = c.targetScalarIn;
  targetScalarOut = c.targetScalarOut;

  XMLtag_set_value->deallocateXMLTag();
  XMLtag_set_value->copyXMLTag( c.XMLtag_set_value );

  immediate_interpretation = c.immediate_interpretation;

  actionID = c.actionID;
}

dm_ElemAction::dm_ElemAction( void ) {
  delayString = NULL;
  clean();
}

dm_ElemAction::~dm_ElemAction(void) {
  if (delayString) {
      delete [] delayString;
      delayString = NULL;
    }

  if (actions) {
      delete actions;
      actions = NULL;
    }

  if (targetNodeList) {
      delete [] targetNodeList;
      targetNodeList = NULL;
    }

  if (targetHosts) {
      delete [] targetHosts;
      targetHosts = NULL;
    }
}

void dm_ElemAction::clean( void ) {
  cleanActionsAndTargets();

  InitString( &delayString );
  immediate_interpretation = false;
}

void dm_ElemAction::cleanActionsAndTargets( void ) {
  actions = NULL;

  targetNodeList = NULL;
  nbTargetNodes = 0;
  targetNodeid[0] = 0;
  targetNodeidRef[0] = 0;
  targetStatus = EmptyNodeList;

  targetHosts = NULL;
  nbTargetHosts = 0;
  targetHostid[0] = 0;
}

void dm_ElemAction::deleteAllocatedComponents( void ) {
  dm_BaseAction * ptAction = actions;
  dm_BaseAction * ptNextAction;
  while( ptAction ) {
    ptNextAction = ptAction->nextActionForTheSameTarget;
    delete ptAction;
    ptAction = ptNextAction;
  }
  actions = NULL;

  if( targetNodeList ) {
    delete [] targetNodeList;
    targetNodeList = NULL;
  }
  nbTargetNodes = 0;

  if( targetHosts ) {
    delete [] targetHosts;
    targetHosts = NULL;
  }
  nbTargetHosts = 0;
  targetHostid[0] = 0;
}

void dm_ElemAction::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<action" );
  if( *delayString ) {
    fprintf( file , " delay=\"%s\"" , delayString);
  }
  fprintf( file , " immediate_interpretation=\"%s\"" , BooleanString[BooleanToInt(immediate_interpretation)] );
  fprintf( file , ">\n" );

  printActionsAndTargets( file , isLong , depth + 1 );

  indent( file , depth );
  fprintf( file , "</action>\n" );
}

void dm_ElemAction::printActionsAndTargets( FILE *file , 
					    int isLong , int depth ) {
  
  dm_BaseAction * ptAction = actions;
  while( ptAction ) {
    ptAction->print( file , isLong , depth );
    ptAction  = ptAction->nextActionForTheSameTarget;
  }

  if( *targetNodeidRef ) {
    indent( file , depth );
    if( targetStatus == EmptyNodeElement
	|| targetStatus == NodeElement 
	|| targetStatus == DynamicNodeElement ) {
      fprintf( file , "<target type=\"single_node\" value=\"%s\" />\n" , targetNodeidRef );
    }
    else if( targetStatus == EmptyNodeList 
	     || targetStatus == NodeList
	     || targetStatus == DynamicNodeList ) {
      fprintf( file , "<target type=\"multiple_nodes\" value=\"%s\" />\n" , targetNodeidRef );
    }
  }

  if( targetHosts ) {
    indent( file , depth );
	  
    if( nbTargetHosts == 1 ) 
      {
	fprintf( file , "<target type=\"single_host\" value=\"%s\" />\n" , targetHostid );
      }
    else if( nbTargetHosts > 1 ) 
      {
	fprintf( file , "<target type=\"multiple_hosts\" value=\"%s\" />\n" , targetHostid );
      }
  }

  ////// ------------ added by Xianyong Fang -------------///////
  // fprintf(file, "nbTargetNodes %d \n", nbTargetNodes);
  //////////// ---------------- end of addition ------------------- /////////////
}

TargetType dm_ElemAction::ParseTargets( dm_SceneNode *sourceNode ,
					int *fileDepth , int *curChar , 
					dm_XMLTag *XMLtag ,
					char *chaine , int *ind ) {
  char * type = XMLtag->getAttributeStringValue( "type" );

  /////////////////////
  // target nodes for message emission
  if( strcmp( type , "single_node" ) == 0 
      || strcmp( type , "multiple_nodes" ) == 0 ) {

    if( strcmp( type , "single_node" ) == 0 ) {
      targetStatus = EmptyNodeElement;
    }
    else if( strcmp( type , "multiple_nodes" ) == 0 ) {
      targetStatus = EmptyNodeList;
    }

    //     targetNodeList = new dm_SceneNode*[1];
    //     targetNodeList[0] = new dm_SceneNode(ANodeLink);
    strcpy( targetNodeidRef , XMLtag->getAttributeStringValue( "value" ) );

    // references are solved in a second pass
    return TargetNode;
  }

  /////////////////////
  // target hosts for UDP message broadcast
  else if( strcmp( type , "single_host" ) == 0 
	   || strcmp( type , "multiple_hosts" ) == 0 ) {
    int multipleLinks = false;
    if( strcmp( type , "single_host" ) == 0 ) {
      multipleLinks = false;
    }
    else if( strcmp( type , "multiple_hosts" ) == 0 ) {
      multipleLinks = true;
    }

    strcpy( targetHostid , XMLtag->getAttributeStringValue( "value" ) );

    nbTargetHosts = 0;
    if( DM_EnvironmentNode ) {
      for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Clients ; ind++ ) {
	if( stringEqual( targetHostid , DM_EnvironmentNode->IP_Clients[ ind ]->id ) ) {
	  nbTargetHosts++;
	  if( !multipleLinks ) {
	    break;
	  }
	}
      }
      targetHosts = new dm_IPClient* [ nbTargetHosts ];
      nbTargetHosts = 0;
      for( int ind = 0 ; ind < DM_EnvironmentNode->nb_IP_Clients ; ind++ ) {
	if( stringEqual( targetHostid , DM_EnvironmentNode->IP_Clients[ ind ]->id ) ) {
	  targetHosts[ nbTargetHosts++ ] = DM_EnvironmentNode->IP_Clients[ ind ];
	  // printf( "target host %d name %s (%s)\n" , ind , targetHosts[ ind ]->id , ActionIDString[actions->actionID]);
	  if( !multipleLinks ) {
	    break;
	  }
	}
      }    
    }

    return TargetHost;
  }

  /////////////////////
  // unknown target type
  else {
    sprintf( ErrorStr , "Error: unknown target type (\"single_node\", \"multiple_nodes\", \"single_host\", \"multiple_hosts\", expected) [%s]!" , type ); ReportError( ErrorStr ) ; throw 134;
  }

  return EmptyTargetType;
}
 
bool dm_ElemAction::ParseElemAction( int *fileDepth , int *curChar , 
				     dm_XMLTag *XMLtag ,
				     char *chaine , int *ind ,
				     EventType sourceEvent , 
				     char *sourceEventString ,
				     dm_SceneNode *sourceNode ) {
  dm_BaseAction * currentAction = NULL;

  enum BaseActionType  actionsType = EmptyBaseActionType;

  enum TargetType  targetType = EmptyTargetType;
  bool targetParsed = false;
  bool endOfString = false;

  // delay
  if( XMLtag->hasAttribute( "delay" ) )  {
    // delay between trigger reception and command execution   
    StrCpy( &delayString , XMLtag->getAttributeStringValue( "delay" ) );
    // printf( "delay %s\n" , delayString );
  }
  else  {
    // default schedule: the first one
    *delayString = 0;
  }
  
  // immediate interpretation
  if( XMLtag->hasAttribute( "immediate_interpretation" ) )  {
    // argument interpretation at posting time and not at execution time  
    XMLtag->BooleanFieldValue( "immediate_interpretation" ,
			       &immediate_interpretation );
  }
  
  while( true ) {
    // endless script
    if( !chaine && *curChar == EOF ) 
      {
	sprintf( ErrorStr , "Error: missing action closing tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
	return false;
      }

    // udp message, end of string reached
    if( chaine && *ind >= (int)strlen( chaine ) - 1 ) 
      {
	endOfString = true;
      }
    else 
      {
	// new tag XML 
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
      }

    // printf( "dm_Action tag %s\n" , XMLtag->tag );

	  
    /////////////////////////
    // action closing tag
    if( ( strcmp( XMLtag->tag , "action" ) == 0  && (XMLtag->tagType == ClosingTag) ) 	|| endOfString ) 
      {
	// actions that expect a target node on which to perform modifications
	if( actionsType ==  BaseActionNodeModification
	    || actionsType ==  BaseActionSendInternalMessage ) 
	  {
	    if( targetType != TargetNode ) 
	      {
		sprintf( ErrorStr , "Error: missing target nodes in node modification or internal message script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
	      }
	  }
	  
	// actions that expect a target host to which to send a message
	else if( actionsType ==  BaseActionSendExternalMessage ) 
	  {
	    if( targetType != TargetHost && targetType != EmptyTargetType ) 
	      {
		sprintf( ErrorStr , "Error: incorrect target hosts in send external message script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
	      }
			  
	    // default target host: if there is at least one host in the conf file
	    // the first host is the target
	    if( targetType == EmptyTargetType ) 
	      {
		if( DM_EnvironmentNode->nb_IP_Clients >= 1 ) 
		  {
		    nbTargetHosts = 1;
		    targetHosts = new dm_IPClient* [ nbTargetHosts ];
		    targetHosts[ 0 ] = DM_EnvironmentNode->IP_Clients[ 0 ];
		    // printf( "target host unique  name %s (%s)\n" , targetHosts[ 0 ]->id  , ActionIDString[actions->actionID]);
		  }
		else 
		  {
		    nbTargetHosts = 0;
		  }
	      }
	  }

	// actions that perform system command do not need any target
	else if( actionsType ==  BaseActionSystemCommand ) 
	  {
	    if( targetType != EmptyTargetType ) 
	      {
		sprintf( ErrorStr , "Error: system command does not require target in script in node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
	      }
	  }
    
	// unknown action type
	else 
	  {
	    sprintf( ErrorStr , "Error: unknown action type (%d) in script of node [%s]!" ,  actionsType , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
	  }
		  
	return true;
      }

    /////////////////////////
    // target
    else if( strcmp( XMLtag->tag , "target" ) == 0 && (XMLtag->tagType == EmptyTag) ) 
      {
	if( targetParsed )
	  {
	    sprintf( ErrorStr , "Error: multiple action target in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 298;
	    return false;
	  }

	// parsing the triggers
	targetType = ParseTargets( sourceNode , fileDepth , curChar , XMLtag , NULL , NULL );
		  
	if( targetType == EmptyTargetType ) 
	  {
	    sprintf( ErrorStr , "Error: incorrect action target in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 298;
	    return false;
	  }
		  
	targetParsed = true;
      }

    /////////////////////////
    // base action
    /////////////////////////
    // base action opening tag
    else 
      {
	dm_BaseAction * parsedAction = new dm_BaseAction();
	bool resul = parsedAction->ParseBaseAction( fileDepth , curChar , 
						    XMLtag ,
						    chaine , ind , 
						    sourceEvent , 
						    sourceEventString ,
						    sourceNode , 
						    &actionsType );
		  
	// parse error
	if( !resul )
	  {
	    sprintf( ErrorStr , "Error: Incorrect action script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
	    return false;
	  }
		  
	//parsedAction->print( stdout , 1 , 1 );
		  
	// first action
	if( !actions ) 
	  {
	    actions = parsedAction;
	    currentAction = actions;
	  }
	// next actions
	else 
	  {
	    // action chaining
	    currentAction->nextActionForTheSameTarget = parsedAction;
	    currentAction = currentAction->nextActionForTheSameTarget;
	  }
      }
  }
}

void dm_ElemAction::execute( ResolutionMode mode , 
			     bool isDelayed ,
			     char * message ,
			     dm_Command *theCommand ) {
  //fprintf( stdout , "base action mode (%d) targetStatus (%d)\n" , mode ,targetStatus);
  // commands received by udp (deprecated syntax in which the XML
  // tags are transfered through UDP)
  // does not concern aliasScripts in which the references are resolved at compile time
  if( mode == dynamicReferenceResolution ) {
    findScriptReferencesDynamic();
  }	  
  else if( mode == staticReferenceResolution 
	   && (targetStatus == DynamicNodeElement 
	       || targetStatus == DynamicNodeList ) ) {
    //printf("execute\n" );
    //printf("Dynamic TargetNode (%s)\n" , targetNodeid );
    StrCpyAndSubst( targetNodeidRef   , targetNodeid , XMLAttrValLength );
    //printf("Dynamic TargetNode (%s -> %s)\n" , targetNodeid , targetNodeidRef);
    findScriptReferencesDynamic();
  }	  

  // argument interpretation at posting time and not at execution time  
  if( immediate_interpretation ) {
    // new actions must be allocated because the attribute values are
    // modified in the XMLtag_set_value tag
    dm_BaseAction * ptAction = actions;
    dm_BaseAction * ptNewActionPrec = NULL;
    while( ptAction ) {
      dm_BaseAction * ptNewAction = new dm_BaseAction();
      *ptNewAction = *ptAction;
      ptNewAction->XMLtag_set_value->deallocateXMLTag();
      ptNewAction->XMLtag_set_value->copyAndSubstitueExpressions( ptAction->XMLtag_set_value );
      ptNewAction->immediate_interpretation = true;
      // fprintf( stdout , "action idscript action [%s,%d]!" , BaseActionIDString[actionID] , actionID );
      // ptNewAction->print( stdout , 1 , 1 );

      // chaining of new actions
      if( ptNewActionPrec ) {
	ptNewActionPrec->nextActionForTheSameTarget = ptNewAction;
      }
      else {
	actions = ptNewAction;
      }
      ptNewActionPrec = ptNewAction;
      ptAction  = ptAction->nextActionForTheSameTarget;
    }
  }

  // delayed commands are not delayed again
  if( isDelayed || !(*delayString) ) {
    // fprintf( stdout , "non redelayed action %s\n" , BaseActionIDString[actions->actionID]);
    dm_BaseAction * ptAction = actions;
    while( ptAction ) {
      // fprintf( stdout , "action targetNodes: nodes %d hosts %d\n" , nbTargetNodes , nbTargetHosts );
      // ptAction->print( stdout , 1 , 1 );
      ptAction->execute( targetNodeList , nbTargetNodes , 
			 targetHosts , nbTargetHosts , false , 0.0 );

      if( ptAction->immediate_interpretation ) {
	dm_BaseAction * ptNextAction  = ptAction->nextActionForTheSameTarget;
	ptNextAction = ptAction->nextActionForTheSameTarget;

	ptAction->XMLtag_set_value->deallocateXMLTag();
	delete ptAction;

	ptAction  = ptNextAction;
      }
      else {
	ptAction  = ptAction->nextActionForTheSameTarget;
      }
    }
  }
  else {
    // fprintf( stdout , "delayed action %s %s Delta %.3f\n" , BaseActionIDString[actions->actionID] , delayString , CurrentClockTime + stringToDoubleValue( delayString , true ) );
    DM_CompositionNode->pushEvent( delayed_command_execution , 
				   this , 
				   message ,
				   theCommand ,
				   (float)CurrentClockTime 
				   + (float)stringToDoubleValue( delayString , 
							  true ) );
  }

  if( mode == staticReferenceResolution 
      && (targetStatus == DynamicNodeElement 
	  || targetStatus == DynamicNodeList ) ) {
    if( targetNodeList ) {
      delete [] targetNodeList;
      targetNodeList = NULL;
    }
    nbTargetNodes = 0;
  }
}

void dm_ElemAction::operator=(dm_ElemAction& c) {
  actions = c.actions;

  nbTargetNodes = c.nbTargetNodes;
  targetNodeList = c.targetNodeList;
  targetStatus = c.targetStatus;
  strcpy( targetNodeid , c.targetNodeid );
  strcpy( targetNodeidRef , c.targetNodeidRef );

  nbTargetHosts = c.nbTargetHosts;
  targetHosts = c.targetHosts;
  strcpy( targetHostid , c.targetHostid );

  StrCpy( &delayString , c.delayString );
  immediate_interpretation = c.immediate_interpretation;
}

////////////////////////////////////////////////////////////
// SCRIPTING REPEAT ACTIONS
////////////////////////////////////////////////////////////
dm_ActionRepeat::dm_ActionRepeat( void ) {
  begin = NULL;
  end = NULL;
  step = NULL;
  scalarString = NULL;
  isActionBroken = false;

  clean();
}

dm_ActionRepeat::~dm_ActionRepeat(void) {
  if (begin) {
      delete [] begin;
      begin = NULL;
    }

  if (end) {
      delete [] end;
      end = NULL;
    }

  if (step) {
      delete [] step;
      step = NULL;
    }

  if (scalarString) {
      delete [] scalarString;
      scalarString = NULL;
    }

  if (actions)  {
      delete actions;
      actions = NULL;
    }
/*
  if (controlScalar) {
      delete controlScalar;
      controlScalar = NULL;
    }
  */
}

void dm_ActionRepeat::clean( void ) {
  controlScalar = NULL;
  InitString( &begin );
  InitString( &end );
  InitString( &step );
  InitString( &scalarString );
  actions = NULL;
}

void dm_ActionRepeat::deleteAllocatedComponents( void ) {
  dm_Action * ptAction = actions;
  dm_Action * ptNextAction;
  while( ptAction ) {
    ptNextAction = ptAction->nextAction;
    ptAction->deleteAllocatedComponents();
    delete ptAction;
    ptAction = ptNextAction;
  }
  actions = NULL;
}

void dm_ActionRepeat::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<repeatAction id=\"%s\" begin=\"%s\" end=\"%s\" step=\"%s\" >\n" , scalarString , begin , end , step );

  dm_Action *a = actions;
  while( a ) {
    a->print( file , isLong , depth + 1 );
    a = a->nextAction;
  }
  
  indent( file , depth );
  fprintf( file , "</repeatAction>\n" );

}

bool dm_ActionRepeat::ParseActionRepeat( int *fileDepth , int *curChar , 
					 dm_XMLTag *XMLtag ,
					 char *chaine , int *ind ,
					 EventType sourceEvent , 
					 char *sourceEventString ,
					 dm_SceneNode *sourceNode ,
					 dm_Action *motherLoopAction ) {
  dm_Action * currentAction = NULL;
  bool endOfString = false;

  StrCpy( &scalarString , XMLtag->getAttributeStringValue( "id" ) );
  if( !sourceNode 
      || !(controlScalar = sourceNode->getScalarFromName( scalarString)))  {
    sprintf( ErrorStr , "Error: Unknown scalar [%s] on repeat script of node [%s]!" , scalarString , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 303;
  }
  /*
  else
  {
	  if (controlScalar)
	  {
		  delete controlScalar;
		  controlScalar = NULL;
	  }
	  
	  controlScalar = new dm_Scalar;
	  *controlScalar = *(sourceNode->getScalarFromName( scalarString ) );
  }
  */
    
  StrCpy( &begin , XMLtag->getAttributeStringValue( "begin" ) );
  StrCpy( &end , XMLtag->getAttributeStringValue( "end" ) );
  StrCpy( &step , XMLtag->getAttributeStringValue( "step" ) );

  // printf( "parse repeat action\n" );

  while( true ) {
    // endless script
    if( !chaine && *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing repeat action closing tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
    }

    // udp message, end of string reached
    if( chaine && *ind >= (int)strlen( chaine ) - 1 ) {
      endOfString = true;
    }
    else {
      // new tag XML 
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
    }

    /////////////////////////
    // action closing tag
    if( ( strcmp( XMLtag->tag , "repeatAction" ) == 0 
	  && (XMLtag->tagType == ClosingTag) ) 
	|| endOfString ) {

      //printf( "correct end of repeat action\n" );
      if( !actions ) {
	sprintf( ErrorStr , "Error: action expected inside repeatAction of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
      }

      return true;
    }

    /////////////////////////
    // action
    /////////////////////////
    // action opening tag
    else if( (strcmp( XMLtag->tag , "action" ) == 0 
	      && (XMLtag->tagType == OpeningTag) )  
	     || (strcmp( XMLtag->tag , "repeatAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "whileAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "ifAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) ) ) { 
      dm_Action * parsedAction = new dm_Action();
      bool resul
	= parsedAction->ParseAction( fileDepth , curChar , 
				     XMLtag ,
				     chaine , ind , 
				     sourceEvent , 
				     sourceEventString ,
				     sourceNode ,
				     motherLoopAction );
      // parse error
      if( !resul ) {
	return false;
      }
  
      // first action
      if( !actions ) {
	actions = parsedAction;
	currentAction = actions;
      }
      // next actions
      else {
	// action chaining
	currentAction->nextAction = parsedAction;
	currentAction = currentAction->nextAction;
      }
    }
  }
}

void dm_ActionRepeat::execute( ResolutionMode mode , bool isDelayed ,
			       char * message ,
			       dm_Command *theCommand ) {
  if (stringToIntValue(step, true) >= 0) {
    isActionBroken = false;
    // when the end value is bigger than the start value
    for( int ind = stringToIntValue( begin , true ) ;
	 ind <= stringToIntValue( end , true ) && !isActionBroken ;
	 ind += stringToIntValue( step , true ) ) {
      // printf("ActionRepeat %d\n" , ind );
      
      if( controlScalar ) {
	controlScalar->SetValue( (float)ind );
      }
      
      dm_Action *a = actions;
      while( a ) {
	//printf("ActionRepeat %.2f %d\n" , controlScalar->GetNumericalValue() ,isDelayed);
	a->execute( mode , isDelayed , message , theCommand );
	a = a->nextAction;
      }
    }
  }
  else { 
    isActionBroken = false;
    // when the end value is smaller than the start value
    for( int ind = stringToIntValue( begin , true ) ;
	 ind >= stringToIntValue( end , true ) && !isActionBroken ;
	 ind += stringToIntValue( step , true ) ) {
      // printf("ActionRepeat %d\n" , ind );
      
      if( controlScalar ) {
	controlScalar->SetValue( (float)ind );
      }
      
      dm_Action *a = actions;
      while( a ) {
	//printf("ActionRepeat %.2f %d\n" , controlScalar->GetNumericalValue() ,isDelayed);
	a->execute( mode , isDelayed , message , theCommand );
	a = a->nextAction;
      }
    }
  }
}

void dm_ActionRepeat::findScriptReferences( dm_SceneNode *rootNode, char *targetID) {
  dm_Action *a = actions;
  while( a ) {
    a->findScriptReferences( rootNode, targetID);
    a = a->nextAction;
  }
}

void dm_ActionRepeat::operator=(dm_ActionRepeat& c) {

	if (controlScalar)
	{
		delete controlScalar;
		controlScalar = NULL;
	}
	if (c.controlScalar)
	{
		controlScalar = new dm_Scalar;
		*controlScalar = *(c.controlScalar);
	}

	controlScalar = c.controlScalar;
  StrCpy( &begin , c.begin );
  StrCpy( &end , c.end );
  StrCpy( &step , c.step );
  StrCpy( &scalarString , c.scalarString );
  actions = c.actions;
  isActionBroken = c.isActionBroken;
}

dm_ActionWhile::dm_ActionWhile( void ) {
  value1 = NULL;
  value2 = NULL;
  isActionBroken = false;

  clean();
}

dm_ActionWhile::~dm_ActionWhile(void) {
  if (value1) {
      delete [] value1;
      value1 = NULL;
    }

  if (value2) {
      delete [] value2;
      value2 = NULL;
    }

  if (actions)  {
      delete actions;
      actions = NULL;
    }
}

void dm_ActionWhile::clean( void ) {
  InitString( &value1 );
  InitString( &value2 );
  oper = EmptyBooleanOperator;
  actions = NULL;
}

void dm_ActionWhile::deleteAllocatedComponents( void ) {
  dm_Action * ptAction = actions;
  dm_Action * ptNextAction;
  while( ptAction ) {
    ptNextAction = ptAction->nextAction;
    ptAction->deleteAllocatedComponents();
    delete ptAction;
    ptAction = ptNextAction;
  }
  actions = NULL;
}

void dm_ActionWhile::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<whileAction value1=\"%s\" value2=\"%s\" bool_operator=\"%s\" >\n" , value1 , value2 , BooleanOperatorString[ oper ] );

  dm_Action *a = actions;
  while( a ) {
    a->print( file , isLong , depth + 1 );
    a = a->nextAction;
  }
  
  indent( file , depth );
  fprintf( file , "</whileAction>\n" );

}

bool dm_ActionWhile::ParseActionWhile( int *fileDepth , int *curChar , 
				       dm_XMLTag *XMLtag ,
				       char *chaine , int *ind ,
				       EventType sourceEvent , 
				       char *sourceEventString ,
				       dm_SceneNode *sourceNode ,
				       dm_Action *motherLoopAction ) {
  dm_Action * currentAction = NULL;
  bool endOfString = false;

  StrCpy( &value1 , XMLtag->getAttributeStringValue( "value1" ) );
  StrCpy( &value2 , XMLtag->getAttributeStringValue( "value2" ) );

  if( XMLtag->hasAttribute( "operator" ) ) {
    oper = GetBooleanOperator( XMLtag->getAttributeStringValue( "operator" ) );
  }
  else if( XMLtag->hasAttribute( "bool_operator" ) ) {
    oper = GetBooleanOperator( XMLtag->getAttributeStringValue( "bool_operator" ) );
  }
  else {
    // default operator: Equal
    oper = Equal;
  }

  while( true ) {
    // endless script
    if( !chaine && *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing while action closing tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
    }

    // udp message, end of string reached
    if( chaine && *ind >= (int)strlen( chaine ) - 1 ) {
      endOfString = true;
    }
    else {
      // new tag XML 
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
    }

    /////////////////////////
    // action closing tag
    if( ( strcmp( XMLtag->tag , "whileAction" ) == 0 
	  && (XMLtag->tagType == ClosingTag) ) 
	|| endOfString ) {
      if( !actions ) {
	sprintf( ErrorStr , "Error: action expected inside whileAction of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
      }
      return true;
    }

    /////////////////////////
    // action
    /////////////////////////
    // action opening tag
    else if( (strcmp( XMLtag->tag , "action" ) == 0 
	      && (XMLtag->tagType == OpeningTag) )  
	     || (strcmp( XMLtag->tag , "repeatAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "whileAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "ifAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) ) ) { 
      dm_Action * parsedAction = new dm_Action();
      bool resul
	= parsedAction->ParseAction( fileDepth , curChar , 
				     XMLtag ,
				     chaine , ind , 
				     sourceEvent , 
				     sourceEventString ,
				     sourceNode ,
				     motherLoopAction );
      // parse error
      if( !resul ) {
	return false;
      }
  
      // first action
      if( !actions ) {
	actions = parsedAction;
	currentAction = actions;
      }
      // next actions
      else {
	// action chaining
	currentAction->nextAction = parsedAction;
	currentAction = currentAction->nextAction;
      }
    }
  }
}

void dm_ActionWhile::execute( ResolutionMode mode , bool isDelayed ,
			      char * message ,
			      dm_Command *theCommand ) {
  isActionBroken = false;
  while( booleanNumericalExpression( (float)stringToDoubleValue( value1 , true ) , 
				     oper , 
				     (float)stringToDoubleValue( value2 , true ) ) 
	 && !isActionBroken ) {
    dm_Action *a = actions;
    while( a ) {
      a->execute( mode , isDelayed , message , theCommand );
      a = a->nextAction;
    }
  }
}

void dm_ActionWhile::findScriptReferences( dm_SceneNode *rootNode, char *targetID ) {
  dm_Action *a = actions;
  while( a ) {
    a->findScriptReferences( rootNode, targetID);
    a = a->nextAction;
  }
}

void dm_ActionWhile::operator=(dm_ActionWhile& c) {
  StrCpy( &value1 , c.value1 );
  StrCpy( &value2 , c.value2 );
  oper = c.oper;
  actions = c.actions;
  isActionBroken = c.isActionBroken;
}

dm_ActionIf::dm_ActionIf( void ) {
  value1 = NULL;
  value2 = NULL;

  clean();
}

dm_ActionIf::~dm_ActionIf(void) {
  if (value1)  {
      delete [] value1;
      value1 = NULL;
    }

  if (value2) {
      delete [] value2;
      value2 = NULL;
    }

  if (actions)  {
      delete actions;
      actions = NULL;
    }
	
  if (actionsElse)  {
      delete actionsElse;
      actionsElse = NULL;
    }
}

void dm_ActionIf::clean( void ) {
  InitString( &value1 );
  InitString( &value2 );
  oper = EmptyBooleanOperator;
  actions = NULL;
  actionsElse = NULL;
}

void dm_ActionIf::deleteAllocatedComponents( void ) {
  dm_Action * ptAction;
  dm_Action * ptNextAction;

  ptAction = actions;
  while( ptAction ) {
    ptNextAction = ptAction->nextAction;
    ptAction->deleteAllocatedComponents();
    delete ptAction;
    ptAction = ptNextAction;
  }
  actions = NULL;

  ptAction = actionsElse;
  while( ptAction ) {
    ptNextAction = ptAction->nextAction;
    ptAction->deleteAllocatedComponents();
    delete ptAction;
    ptAction = ptNextAction;
  }
  actionsElse = NULL;
}

void dm_ActionIf::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<ifAction value1=\"%s\" value2=\"%s\" bool_operator=\"%s\" >\n" , value1 , value2 , BooleanOperatorString[ oper ] );

  dm_Action *a = actions;
  while( a ) {
    a->print( file , isLong , depth + 1 );
    a = a->nextAction;
  }
  
  if( actionsElse ) {
    indent( file , depth );
    fprintf( file , "<elseAction/>\n" );
    dm_Action *aElse = actionsElse;
    while( aElse ) {
      aElse->print( file , isLong , depth + 1 );
      aElse = aElse->nextAction;
    }
  }
  
  indent( file , depth );
  fprintf( file , "</ifAction>\n" );

}

bool dm_ActionIf::ParseActionIf( int *fileDepth , int *curChar , 
				 dm_XMLTag *XMLtag ,
				 char *chaine , int *ind ,
				 EventType sourceEvent , 
				 char *sourceEventString ,
				 dm_SceneNode *sourceNode ,
				 dm_Action *motherLoopAction ) {
  dm_Action * currentAction = NULL;
  dm_Action * currentActionElse = NULL;
  bool endOfString = false;

  StrCpy( &value1 , XMLtag->getAttributeStringValue( "value1" ) );
  StrCpy( &value2 , XMLtag->getAttributeStringValue( "value2" ) );

  if( XMLtag->hasAttribute( "operator" ) ) {
    oper = GetBooleanOperator( XMLtag->getAttributeStringValue( "operator" ) );
  }
  else if( XMLtag->hasAttribute( "bool_operator" ) ) {
    oper = GetBooleanOperator( XMLtag->getAttributeStringValue( "bool_operator" ) );
  }
  else {
    // default operator: Equal
    oper = Equal;
  }

  bool isElseAction = false;

  while( true ) {
    // endless script
    if( !chaine && *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing if action closing tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
    }

    // udp message, end of string reached
    if( chaine && *ind >= (int)strlen( chaine ) - 1 ) {
      endOfString = true;
    }
    else {
      // new tag XML 
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
    }

    /////////////////////////
    // action closing tag
    if( ( strcmp( XMLtag->tag , "ifAction" ) == 0 
	  && (XMLtag->tagType == ClosingTag) ) 
	|| endOfString ) {
      if( !actions ) {
	sprintf( ErrorStr , "Error: action expected inside ifAction of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
      }
      return true;
    }

    /////////////////////////
    // action else tag
    if( strcmp( XMLtag->tag , "elseAction" ) == 0 
	&& (XMLtag->tagType == EmptyTag) ) {
      if( !isElseAction ) {
	isElseAction = true;
      }
      else {
	sprintf( ErrorStr , "Error: multiple else action tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
      }
    }

    /////////////////////////
    // action
    /////////////////////////
    // action opening tag
    else if( (strcmp( XMLtag->tag , "action" ) == 0 
	      && (XMLtag->tagType == OpeningTag) )  
	     || (strcmp( XMLtag->tag , "repeatAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "whileAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "ifAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) ) ) { 
      dm_Action * parsedAction = new dm_Action();
      bool resul
	= parsedAction->ParseAction( fileDepth , curChar , 
				     XMLtag ,
				     chaine , ind , 
				     sourceEvent , 
				     sourceEventString ,
				     sourceNode ,
				     motherLoopAction );
      // parse error
      if( !resul ) {
	return false;
      }
  
      if( !isElseAction ) {
	// first action
	if( !actions ) {
	  actions = parsedAction;
	  currentAction = actions;
	}
	// next actions
	else {
	  // action chaining
	  currentAction->nextAction = parsedAction;
	  currentAction = currentAction->nextAction;
	}
      }
      else {
	// first action
	if( !actionsElse ) {
	  actionsElse = parsedAction;
	  currentActionElse = actionsElse;
	}
	// next actions
	else {
	  // action chaining
	  currentActionElse->nextAction = parsedAction;
	  currentActionElse = currentActionElse->nextAction;
	}
      }
    }
  }
}

void dm_ActionIf::execute( ResolutionMode mode , bool isDelayed ,
			   char * message ,
			   dm_Command *theCommand ) {
//     printf("booleanNumericalExpression %.2f %d %.2f = %d\n" ,  
//   	 stringToDoubleValue( value1 , true ) , 
//   	 oper , 
//   	 stringToDoubleValue( value2 , true ) ,
//   	 booleanNumericalExpression( stringToDoubleValue( value1 , true ) , 
//   				     oper , 
//   				     stringToDoubleValue( value2 , true ) ) );

  if( booleanNumericalExpression( (float)stringToDoubleValue( value1 , true ) , 
				  oper , 
				  (float)stringToDoubleValue( value2 , true ) ) ) {
    dm_Action *a = actions;
    while( a ) {
      a->execute( mode , isDelayed , message , theCommand );
      a = a->nextAction;
    }
  }
  else {
    dm_Action *a = actionsElse;
    while( a ) {
      a->execute( mode , isDelayed , message , theCommand );
      a = a->nextAction;
    }
  }
}

void dm_ActionIf::findScriptReferences( dm_SceneNode *rootNode, char *targetID ) {
  dm_Action *a = actions;
  while( a ) {
    a->findScriptReferences( rootNode, targetID );
    a = a->nextAction;
  }
  a = actionsElse;
  while( a ) {
    a->findScriptReferences( rootNode, targetID );
    a = a->nextAction;
  }
}

void dm_ActionIf::operator=(dm_ActionIf& c) {
  StrCpy( &value1 , c.value1 );
  StrCpy( &value2 , c.value2 );
  oper = c.oper;
  actions = c.actions;
  actionsElse = c.actionsElse;
}

dm_ActionBreak::dm_ActionBreak( void ) {
  motherLoop = NULL;
}

dm_ActionBreak::~dm_ActionBreak(void) {
}

void dm_ActionBreak::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "</break>\n" );
}

bool dm_ActionBreak::ParseActionBreak( int *fileDepth , int *curChar , 
				       dm_XMLTag *XMLtag ,
				       char *chaine , int *ind ,
				       EventType sourceEvent , 
				       char *sourceEventString ,
				       dm_SceneNode *sourceNode ,
				       dm_Action *motherLoopAction ) {
  motherLoop = motherLoopAction;
  return true;
}

void dm_ActionBreak::execute( ResolutionMode mode , bool isDelayed ,
			      char * message ,
			      dm_Command *theCommand ) {
  if( !motherLoop ) {
    printf("nothing to stop here\n" );
  }
  else {
    if( motherLoop->type == WhileAction ) {
      motherLoop->actionWhile->isActionBroken = true;      
    }
    else if( motherLoop->type == RepeatAction ) {
      motherLoop->actionRepeat->isActionBroken = true;      
    }
    else {
      sprintf( ErrorStr , "Error: Unexpected loop action type [%d]!" , motherLoop->type ); ReportError( ErrorStr ) ; 
    }
  }
}

void dm_ActionBreak::operator=(dm_ActionBreak& c) {
  motherLoop = c.motherLoop;
}

dm_Action::dm_Action( void ) {
  clean();
}

dm_Action::~dm_Action(void) {
	
  if (elemAction) {
      delete elemAction;
      elemAction = NULL;
    }

  if (actionRepeat) {
    delete actionRepeat;
    actionRepeat = NULL;
  }

  if (actionWhile) {
      delete actionWhile;
      actionWhile = NULL;
    }

  if (actionIf) {
      delete actionIf;
      actionIf = NULL;
    }

  if (actionBreak) {
      delete actionBreak;
      actionBreak = NULL;
    }

  if (nextAction)  {
      delete nextAction;
      nextAction = NULL;
    }
	
}

void dm_Action::clean( void ) {
  type = EmptyActionType;
  elemAction = NULL;
  actionRepeat = NULL;
  actionWhile = NULL;
  actionIf = NULL;
  actionBreak = NULL;

  // action: chaining
  nextAction = NULL;
}

void dm_Action::deleteAllocatedComponents( void ) {
  if( type == ElementaryAction && elemAction ) {
    elemAction->deleteAllocatedComponents();
    elemAction = NULL;
  }
  else if( type == RepeatAction && actionRepeat ) {
    actionRepeat->deleteAllocatedComponents();
    actionRepeat = NULL;
  }
  else if( type == WhileAction && actionWhile ) {
    actionWhile->deleteAllocatedComponents();
    actionWhile = NULL;
  }
  else if( type == IfAction && actionIf ) {
    actionIf->deleteAllocatedComponents();
    actionIf = NULL;
  }
  else if( type == BreakAction && actionBreak ) {
    actionBreak = NULL;
  }
  else {
    sprintf( ErrorStr , "Error: unknown action type [%d]!" , type ); ReportError( ErrorStr ) ; throw 129;
  }
}

void dm_Action::print( FILE *file , int isLong , int depth ) {
  if( type == ElementaryAction && elemAction ) {
    elemAction->print( file , isLong , depth );
  }
  else if( type == RepeatAction && actionRepeat ) {
    actionRepeat->print( file , isLong , depth );
  }
  else if( type == WhileAction && actionWhile ) {
    actionWhile->print( file , isLong , depth );
  }
  else if( type == IfAction && actionIf ) {
    actionIf->print( file , isLong , depth );
  }
  else if( type == BreakAction && actionBreak ) {
    actionBreak->print( file , isLong , depth );
  }
  else {
    sprintf( ErrorStr , "Error: unknown action type [%d]!" , type ); ReportError( ErrorStr ) ; throw 129;
  }
}

bool dm_Action::ParseAction( int *fileDepth , int *curChar , 
			     dm_XMLTag *XMLtag ,
			     char *chaine , int *ind ,
			     EventType sourceEvent , 
			     char *sourceEventString ,
			     dm_SceneNode *sourceNode ,
			     dm_Action *motherLoopAction ) {

  // printf( "parse action (%s)\n" , NodeIDOrEmptyString( sourceNode ) );

  if( strcmp( XMLtag->tag , "action" ) == 0 
      && (XMLtag->tagType == OpeningTag) ) {
    // action allocation
    elemAction = new dm_ElemAction();
    type = ElementaryAction;
    
    // action parsing
    if( !elemAction
	->ParseElemAction( fileDepth , 
			   curChar , 
			   XMLtag ,
			   chaine , ind , 
			   sourceEvent , 
			   sourceEventString ,
			   sourceNode ) ) {
      sprintf( ErrorStr , "Error: Incorrect action sequence script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
      return false;
    }
    // elemAction->print( stdout , 1 , 1 );
  }
  else if( strcmp( XMLtag->tag , "repeatAction" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    // action allocation
    actionRepeat = new dm_ActionRepeat();
    type = RepeatAction;
    
    // action parsing
    if( !actionRepeat
	->ParseActionRepeat( fileDepth , 
			     curChar , 
			     XMLtag ,
			     chaine , ind , 
			     sourceEvent , 
			     sourceEventString ,
			     sourceNode ,
			     this ) ) {
      return false;
    }
  }
  else if( strcmp( XMLtag->tag , "whileAction" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    // action allocation
    actionWhile = new dm_ActionWhile();
    type = WhileAction;
    
    // action parsing
    if( !actionWhile
	->ParseActionWhile( fileDepth , 
			    curChar , 
			    XMLtag ,
			    chaine , ind , 
			    sourceEvent , 
			    sourceEventString ,
			    sourceNode ,
			    this ) ) {
      return false;
    }
  }
  else if( strcmp( XMLtag->tag , "ifAction" ) == 0 
	   && (XMLtag->tagType == OpeningTag) ) {
    // action allocation
    actionIf = new dm_ActionIf();
    type = IfAction;
    
    // action parsing
    if( !actionIf
	->ParseActionIf( fileDepth , 
			 curChar , 
			 XMLtag ,
			 chaine , ind , 
			 sourceEvent , 
			 sourceEventString ,
			 sourceNode ,
			 motherLoopAction ) ) {
      return false;
    }
  }
  else if( strcmp( XMLtag->tag , "breakAction" ) == 0 
	   && (XMLtag->tagType == EmptyTag) ) {
    // action allocation
    actionBreak = new dm_ActionBreak();
    type = BreakAction;
    
    // action parsing
    if( !actionBreak
	->ParseActionBreak( fileDepth , 
			    curChar , 
			    XMLtag ,
			    chaine , ind , 
			    sourceEvent , 
			    sourceEventString ,
			    sourceNode ,
			    motherLoopAction ) ) {
      return false;
    }
  }
  else {
    sprintf( ErrorStr , "Error: unknown action tag [%s]!" , XMLtag->tag ); ReportError( ErrorStr ) ;  throw 129;
  }
  return true;
}

void dm_Action::execute( ResolutionMode mode , bool isDelayed ,
			 char * message ,
			 dm_Command *theCommand ) {
  if( type == ElementaryAction && elemAction ) {
    elemAction->execute( mode , isDelayed ,
			 message , theCommand );
  }
  else if( type == RepeatAction && actionRepeat ) {
    actionRepeat->execute( mode , isDelayed ,
			   message , theCommand );
  }
  else if( type == WhileAction && actionWhile ) {
    actionWhile->execute( mode , isDelayed ,
			  message , theCommand );
  }
  else if( type == IfAction && actionIf ) {
    actionIf->execute( mode , isDelayed ,
		       message , theCommand );
  }
  else if( type == BreakAction && actionBreak ) {
    actionBreak->execute( mode , isDelayed ,
		       message , theCommand );
  }
  else {
    sprintf( ErrorStr , "Error: unknown action type [%d]!" , type ); ReportError( ErrorStr ) ;  throw 129;
  }
}

void dm_Action::findScriptReferences( dm_SceneNode *rootNode, char *targetID ) {
  // action tyoe
  if( type == ElementaryAction && elemAction ) {
    elemAction->findScriptReferences( rootNode, targetID );
  }
  else if( type == RepeatAction && actionRepeat ) {
    actionRepeat->findScriptReferences( rootNode, targetID );
  }
  else if( type == WhileAction && actionWhile ) {
    actionWhile->findScriptReferences( rootNode, targetID );
  }
  else if( type == IfAction && actionIf ) {
    actionIf->findScriptReferences( rootNode, targetID );
  }
  else if( type == BreakAction && actionBreak ) {
    // references (empty tag without attributes
  }
}

void dm_Action::operator=(dm_Action& c) {
  type = c.type;
  elemAction = c.elemAction;
  actionRepeat = c.actionRepeat;
  actionWhile = c.actionWhile;
  actionIf = c.actionIf;
  actionBreak = c.actionBreak;
  nextAction = c.nextAction;
}

dm_Trigger::dm_Trigger( void  ) {
  expectedState = NULL;
  stateOperator = NULL;
  clean();
}

dm_Trigger::~dm_Trigger( void ) {
  if (expectedState) {
    delete [] expectedState;
    expectedState = NULL;
  }

  if (stateOperator) {
    delete [] stateOperator;
    stateOperator = NULL;
  }
}

void dm_Trigger::clean( void  ) {
  sourceEvent = EmptyEventType;
  *sourceEventString = 0;

  sourcebegintime = 0;
  sourcetimelimit = 0;
  sourceperiod = 0;
  sourcevariation = 1.0;

  InitString( &expectedState );
  InitString( &stateOperator );
  StrCpy( &stateOperator , "==" );
  stateOperatorValue = Equal;
}

bool dm_Trigger::ParseTrigger( dm_SceneNode *sourceNode ,
			       int *fileDepth , int *curChar , 
			       dm_XMLTag *XMLtag ,
			       char *chaine , int *ind ) {
  char * type = XMLtag->getAttributeStringValue( "type" );

  /////////////////////
  // trigger: message event
  // on-event command
  if( strcmp( type , "message_event" ) == 0 ) {
    strcpy( sourceEventString , XMLtag->getAttributeStringValue( "value" ) );
    sourceEvent = message_event;
  }
  /////////////////////
  // trigger: keystroke event
  // on-event command
  else if( strcmp( type , "keystroke" ) == 0 ) {
    strcpy( sourceEventString , XMLtag->getAttributeStringValue( "value" ) );
    //sourceEventString[1] = 0;
    sourceEvent = keystroke;
  }
  /////////////////////
  // trigger: deadline
  // at-time command
  // atRunTime is set to false in each XMLtag->getAttributeDoubleValue
  // atRunTime is only true for strings that are dynamically
  // evaluated. the values of temporal limits are evaluated at compile time
  // triggers are not part of dynamically evaluated script commands
  // their values are computed at compile time
  else if( strcmp( type , "time_limit" ) == 0 ) {
    sourcetimelimit = (float)XMLtag->getAttributeDoubleValue( "value" , false );
    sourceEvent = time_limit;
  }
  /////////////////////
  // trigger: regularly repeated deadline
  // at-time command
  else if( strcmp( type , "cyclic_time" ) == 0 ) {
    sourcebegintime = (float)XMLtag->getAttributeDoubleValue( "begin" , false );
    sourceperiod = (float)XMLtag->getAttributeDoubleValue( "period" , false );
    sourcevariation = 1.0;
    sourcetimelimit = sourcebegintime;
    sourceEvent = cyclic_time;
  }
  else if( strcmp( type , "random_cyclic_time" ) == 0 ) {
    sourcebegintime = (float)XMLtag->getAttributeDoubleValue( "begin" , false );
    sourceperiod = (float)XMLtag->getAttributeDoubleValue( "period" , false );
    sourcevariation = 1.0;
    sourcetimelimit = sourcebegintime;
    sourceEvent = random_cyclic_time;
  }
  /////////////////////
  // trigger: increasingly or decreasingly repeated deadline
  // at-time command
  else if( strcmp( type , "varying_cyclic_time" ) == 0 ) {
    sourcebegintime = (float)XMLtag->getAttributeDoubleValue( "begin" , false );
    sourceperiod = (float)XMLtag->getAttributeDoubleValue( "period" , false );
    sourcevariation = (float)XMLtag->getAttributeDoubleValue( "variation" , false );
    sourcetimelimit = sourcebegintime;
    sourceEvent = varying_cyclic_time;
  }
  else if( strcmp( type , "varying_random_cyclic_time" ) == 0 ) {
    sourcebegintime = (float)XMLtag->getAttributeDoubleValue( "begin" , false );
    sourceperiod = (float)XMLtag->getAttributeDoubleValue( "period" , false );
    sourcevariation = (float)XMLtag->getAttributeDoubleValue( "variation" , false );
    sourcetimelimit = sourcebegintime;
    sourceEvent = varying_random_cyclic_time;
  }
  else {
    sprintf( ErrorStr , "Error: unknown trigger type [%s]!" , type ); ReportError( ErrorStr ) ; throw 129;
    return false;
  } 

  /////////////////////
  // optional addition of an expected state
  // if no state is provided, the command is applied 
  // whatever the internal state
  if( XMLtag->hasAttribute( "state" ) ) {
    StrCpy( &expectedState , XMLtag->getAttributeStringValue( "state" ) );
  }
  if( XMLtag->hasAttribute( "bool_operator" ) ) {
    StrCpy( &stateOperator , XMLtag->getAttributeStringValue( "bool_operator" ) );
    stateOperatorValue = GetBooleanOperator( stateOperator );
  }

  return true;
}

void dm_Trigger::print( FILE *file , int isLong , int depth ) {
  bool printed = false;
  if( sourceEvent == keystroke ) {
    indent( file , depth );
    fprintf( file , "<trigger type=\"%s\" value=\"%s\" " , 
	     EventTypeString[sourceEvent] ,
	     sourceEventString );
    printed = true;
  }
  else if( sourceEvent == message_event ) {
    indent( file , depth );
    fprintf( file , "<trigger type=\"%s\" value=\"%s\" " , 
	     EventTypeString[sourceEvent] ,
	     sourceEventString );
    printed = true;
  }
  else if( sourceEvent == time_limit ) {
    indent( file , depth );
    fprintf( file , "<trigger type=\"%s\" value=\"%.5f\" " , 
	     EventTypeString[sourceEvent] ,
	     sourcetimelimit );
    printed = true;
  }
  else if( sourceEvent == cyclic_time 
	   || sourceEvent == random_cyclic_time ) {
    indent( file , depth );
    fprintf( file , "<trigger type=\"%s\"  begin=\"%.5f\" period=\"%.5f\" " , 
	     EventTypeString[sourceEvent] ,
	     sourcebegintime , sourceperiod );
    printed = true;
  }
  else if( sourceEvent == varying_cyclic_time 
	   || sourceEvent == varying_random_cyclic_time ) {
    indent( file , depth );
    fprintf( file , "<trigger type=\"%s\"  begin=\"%.5f\" period=\"%.5f\" variation=\"%.5f\" " , 
	     EventTypeString[sourceEvent] ,
	     sourcebegintime , sourceperiod , sourcevariation );
    printed = true;
  }

  if( printed ) {
    if( *expectedState ) {
      fprintf( file , "state=\"%s\" bool_operator=\"%s\" />\n" , expectedState , stateOperator );
    }
    else {
      fprintf( file , "/>\n" );
    }
  }
}

void dm_Trigger::operator=(dm_Trigger& c) {
  sourceEvent = c.sourceEvent;
  strcpy( sourceEventString , c.sourceEventString );
  sourcetimelimit = c.sourcetimelimit;
  sourcebegintime = c.sourcebegintime;
  sourceperiod = c.sourceperiod;
  sourcevariation = c.sourcevariation;

  StrCpy( &expectedState , c.expectedState );
  StrCpy( &stateOperator , c.stateOperator );
  stateOperatorValue = c.stateOperatorValue;
}

dm_Command::dm_Command( void ) {
	aliasPattern = NULL;
	CommandAliasOSCTag = NULL;
	clean();
}


dm_Command::~dm_Command(void) {
  if (aliasPattern) {
      delete [] aliasPattern;
      aliasPattern = NULL;
    }

  if (CommandAliasOSCTag) {
      delete [] CommandAliasOSCTag;
      CommandAliasOSCTag = NULL;
    }

  if (commandOSCParams) {
      delete [] commandOSCParams;
      commandOSCParams = NULL;
    }

  if (nextCommand) {
      delete nextCommand;
      nextCommand = NULL;
    }

  if (synchronousCommand) {
      delete synchronousCommand;
      synchronousCommand = NULL;
    }

  if (actions) {
      delete actions;
      actions = NULL;
    }
}


void dm_Command::clean( void ) {
  trigger.clean();

  // ALIAS PATTERN
  InitString( &aliasPattern );
  // OSC patterns e.g.   /id
  InitString( &CommandAliasOSCTag );
  // command parameters (pointers inside CommandAlias)
  commandNbOSCParams = 0;
  commandOSCParams = NULL;

  actions = NULL;

  initialCarrierNode = NULL;

  cleanActionsAndTargets();
}

void dm_Command::cleanActionsAndTargets( void ) {
  nextCommand = NULL;
  synchronousCommand = NULL;

  actions = NULL;
}

void dm_Command::deleteAllocatedComponents( void ) {
  dm_Action * ptAction = actions;
  dm_Action * ptNextAction;
  while( ptAction ) {
    ptNextAction = ptAction->nextAction;
    ptAction->deleteAllocatedComponents();
    delete ptAction;
    ptAction = ptNextAction;
  }
  actions = NULL;
}

void dm_Command::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<command" );
  if( *aliasPattern ) {
    fprintf( file , " pattern=\"%s\"" , aliasPattern );
  }
  fprintf( file , ">\n" );

  trigger.print( file , isLong , depth + 1 );
    
  printActions( file , isLong , depth + 1 );

  indent( file , depth );
  fprintf( file , "</command>\n" );
}

void dm_Command::printActions( FILE *file , int isLong , int depth ) {
  dm_Action * ptAction = actions;
  while( ptAction ) {
    ptAction->print( file , isLong , depth + 1 );
    ptAction  = ptAction->nextAction;
  }
}

bool dm_Command::ParseCommandTail( int *fileDepth , int *curChar , 
				   dm_XMLTag *XMLtag ,
				   dm_SceneNode *sourceNode , 
				   char *chaine , int *ind ) {
  dm_Action * currentAction = NULL;
  bool endOfString = false;
  bool firstTag = true; // first tag is read when calling ParseCommandTail

  while( true ) {
    // endless script
    if( !chaine && *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing action closing tag in script of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 89;
      return false;
    }

    // udp message, end of string reached
    if( !firstTag ) {
      if( chaine && *ind >= (int)strlen( chaine ) - 1 ) {
	endOfString = true;
      }
      else {
	// new tag XML 
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
      }
    }
    firstTag = false;

    /////////////////////////
    // action closing tag
    if( ( strcmp( XMLtag->tag , "command" ) == 0 
	  && (XMLtag->tagType == ClosingTag) ) 
	|| endOfString ) {
      return true;
    }

    /////////////////////////
    // action
    /////////////////////////
    // action opening tag
    else if( (strcmp( XMLtag->tag , "action" ) == 0
	      && (XMLtag->tagType == OpeningTag) )  
	     || (strcmp( XMLtag->tag , "repeatAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "whileAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "ifAction" ) == 0 
		 && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) ) ) {
      // printf( "action opening tag (%s,%d)\n" , XMLtag->tag , XMLtag->tagType );

      dm_Action * parsedAction = new dm_Action();
      bool resul
	= parsedAction->ParseAction( fileDepth , curChar , 
				     XMLtag ,
				     chaine , ind , 
				     trigger.sourceEvent , 
				     trigger.sourceEventString ,
				     sourceNode ,
				     NULL );
      // parse error
      if( !resul ) {
		  delete parsedAction;
		  return false;
      }

      // first action
      if( !actions ) {
	actions = parsedAction;
	currentAction = actions;
      }
      // next actions
      else {
	// action chaining
	currentAction->nextAction = parsedAction;
	currentAction = currentAction->nextAction;
      }
    }
    else {
      sprintf( ErrorStr , "Error: unknown script tag (%s) in node [%s]!" , XMLtag->tag , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; throw 298;
      return false;
    }
  }
}

void dm_Command::StoresOSCParameters( char *pch ) {
  // parameters are first stored on the command and
  // then transfered to the composition (without copy,
  // just pointer assignment)

  // printf( "OSC parameters %s\n" , pch );
  int indOSCParam = 0;
  int indChar = 0;
  int p_c = ' ';
  SpaceCommentsInclude( NULL , &p_c , pch , &indChar );
  while( indOSCParam < commandNbOSCParams && p_c ) {
    if( commandOSCParams[ indOSCParam ].type ==  DataTypeInt ) {
      commandOSCParams[ indOSCParam ].valInt
	= ReadInteger( NULL , &p_c , true ,pch , &indChar );
      // printf( "argt #%d int %d\n" , indOSCParam + 1 , commandOSCParams[ indOSCParam ].valInt );
    }
    else if( commandOSCParams[ indOSCParam ].type ==  DataTypeDouble ) {
      commandOSCParams[ indOSCParam ].valFloat
	= (float) ReadFloat( NULL , &p_c , true ,pch , &indChar , 
			     true );
      // printf( "argt #%d float %.4f\n" , indOSCParam + 1 , commandOSCParams[ indOSCParam ].valFloat );
    }
    else if( commandOSCParams[ indOSCParam ].type ==  DataTypeString ) {
      char *temporaryValue = NULL;
      // printf( "ReadStringWithFrontiers2 p_c %c\n" , p_c );
      if( p_c == '"' ) {
	ReadStringWithFrontiers( NULL , &p_c , &temporaryValue ,
				 '"' , '"' , true , 
				 pch , &indChar );
      }
      else {
	ReadStringWithTermination( NULL , &p_c , &temporaryValue ,
				   ' ' , true , 
				   pch , &indChar );
      }
      if( temporaryValue ) {
	StrCpy( &(commandOSCParams[ indOSCParam ].valString) ,
		temporaryValue );
	delete [] temporaryValue;
	temporaryValue = NULL;
      }

      
      // printf( "argt #%d str %s\n" , indOSCParam + 1 , commandOSCParams[ indOSCParam ].valString );
    }
    indOSCParam++;
  }
}

void dm_Command::execute( ResolutionMode mode , 
			  bool isDelayed ,
			  char * message ) {
  dm_Action * ptAction = actions;
  while( ptAction ) {
    ptAction->execute( mode , isDelayed , message , this );
    ptAction  = ptAction->nextAction;
  }
}

void dm_Command::operator=(dm_Command& c) {
  trigger = c.trigger;
  actions = c.actions;

  nextCommand = c.nextCommand;
  synchronousCommand = c.synchronousCommand;
  StrCpy( &aliasPattern , c.aliasPattern );
  StrCpy( &CommandAliasOSCTag , c.CommandAliasOSCTag );
  commandNbOSCParams = c.commandNbOSCParams;
  if (commandOSCParams) {
      delete [] commandOSCParams;
      commandOSCParams = NULL;
    }

  if( commandNbOSCParams > 0 ) {
    if( !commandOSCParams ) {
      commandOSCParams = new dm_ValScalar[ commandNbOSCParams ];
    }
    for( int ind = 0 ; ind < commandNbOSCParams ; ind++ ) {
      commandOSCParams[ ind ] = c.commandOSCParams[ ind ];
    }
  }
  initialCarrierNode = c.initialCarrierNode;
}

dm_Script::dm_Script( void ) {
	// save the new script into the header of the AllScript link for later removal
	strAllScript* newScript = new strAllScript;
	newScript->curtScript = this;
	newScript->nextScript = NULL;

	newScript->nextScript = AllScript->nextScript;
	AllScript->nextScript = newScript;

	// other initialization
	id = NULL;
	cleanScript();
}

dm_Script::dm_Script( char *ID ) {
	// save the new script into the header of the AllScript link for later removal
	strAllScript* newScript = new strAllScript;
	newScript->curtScript = this;
	newScript->nextScript = NULL;

	newScript->nextScript = AllScript->nextScript;
	AllScript->nextScript = newScript;

	// other initialization
	id = NULL;
	cleanScript();
	StrCpy( &id , ID );
}


dm_Script::~dm_Script(void) {
  if (commandList) {
      delete commandList;
      commandList = NULL;
    }

  if (id) {
      delete [] id;
      id = NULL;
    }
}


void dm_Script::cleanScript( void ) {
  commandList = NULL;
  InitString( &id );
}

void dm_Script::print( FILE *file , int isLong , int depth ) {
  dm_Command *curCommand = commandList;
  dm_Command *synchronous = NULL;
  if( curCommand ) {
    indent( file , depth );
    fprintf( file , "<script id=\"%s\">\n" , id );
	  
    if( isLong ) {
      while( curCommand ) {
	curCommand->print( file , isLong , depth + 1 );
	synchronous = curCommand->synchronousCommand;
			  
	while( synchronous ) {
	  synchronous->printActions( file , isLong , depth + 2 );
	  synchronous = synchronous->synchronousCommand;
	}
			  
	curCommand = curCommand->nextCommand;
      }
    }
	  
    indent( file , depth );
    fprintf( file , "</script>\n" );
  }
}

void dm_Script::receive( EventType event ,
			 char *message ,
			 dm_SceneNode *receiver ) {
  dm_Command *curCommand = commandList;
  dm_Command *synchronous = NULL;

  char *receiverInternalState = NULL;
  

  if( receiver ) {
    StrCpy( &receiverInternalState ,  receiver->internalState );
    //printf("\nevent %s receiver state [%s] id [%s] interpolator %d\n" , message , receiver->internalState , receiver->id , receiver->interpolatorNode);
  }
  else {
    InitString( &receiverInternalState );
    //printf("\nevent %s void receiver \n" , message);
  }
  
  

  // message parameter extraction
  char messageCopy[XMLAttrValLength];
  char *messageParameters = NULL;
  
  // message with parameters
  // printf("\nmessage (%s)\n" , message );
  strcpy( messageCopy , message );
  if( *messageCopy == '/' 
      && (messageParameters = strpbrk( messageCopy ,  "\n \t\13" ) ) ) {
    // only keeps the message address for comparison
    // with current command 
    *messageParameters = 0;

    // printf("messageAddress (%s) param: (%s)\n" , message , messageParameters );
  }
  // message parameter extraction
  ///////////////////////////////////////////////////////////

  while( curCommand ) {

    // the appropriate command is found
    // the OSC parameters must be instantiated
    // printf("message comparison (%s) (%s) (%i) (%i) == [%d]\n" , curCommand->trigger.sourceEventString , messageCopy , *curCommand->trigger.sourceEventString , *messageCopy , stringEqual( curCommand->trigger.sourceEventString , messageCopy ));
    if( curCommand->trigger.sourceEvent == event 
	&& stringEqual( curCommand->trigger.sourceEventString , 
			messageCopy ) ) {
      // if( messageParameters ) {
      // printf("\nmessageAddress (%s) (%s)\n" , 
      // messageCopy , messageParameters + 1);
      // }
      // else {
      // printf("messageAddress (%s) no parameters\n" , messageCopy );
      // }

      if( !(*(curCommand->trigger.expectedState)) 
	  || !(*receiverInternalState) 
	  || booleanStringExpression( curCommand->trigger.expectedState , 
				      curCommand->trigger.stateOperatorValue , 
				      receiverInternalState ) ) {

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
	  curCommand->StoresOSCParameters( messageParameters + 1 );

	  // printf("OSC param: " );
	  // for( int ind = 0 ; ind < curCommand->commandNbOSCParams ; ind++ ) {
	  //   curCommand->commandOSCParams[ ind ].print( stdout );
	  // }
	  // printf("\n");

	  localNbOSCParams = curCommand->commandNbOSCParams;
	  localOSCParams = new dm_ValScalar[ curCommand->commandNbOSCParams ];
	  // copies locally the param values
	  for( int ind = 0 ; ind < curCommand->commandNbOSCParams ; ind++ ) {
	    localOSCParams[ ind ] = curCommand->commandOSCParams[ ind ];
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
	// 	printf("execute\n" );
	curCommand->execute( staticReferenceResolution , false , message );
	synchronous = curCommand->synchronousCommand;
	while( synchronous ) {
	  synchronous->execute( staticReferenceResolution , false , message );
	  synchronous = synchronous->synchronousCommand;
	}
	// execution
	///////////////////////////////////////////////////////////

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

      }
    }

    curCommand = curCommand->nextCommand;
  }

  if (receiverInternalState) {
      delete [] receiverInternalState;
    }
}

bool dm_Script::triggerScheduledCommands( float time ) {
  dm_Command *curCommand = commandList;
  dm_Command *precCommand = NULL;
  dm_Command *synchronous = NULL;
  bool      commandExecuted = false;
  bool      singleCommandExecuted = false;
  char     *carrierInternalState = NULL;

  // printf("\nNEW LOOP\n" );
  // loops over all the commands
  while( curCommand ) {
    singleCommandExecuted = false;

    // scheduled commands
    if( ( curCommand->trigger.sourceEvent == time_limit 
	  || curCommand->trigger.sourceEvent == random_cyclic_time 
	  || curCommand->trigger.sourceEvent == cyclic_time 
	  || curCommand->trigger.sourceEvent == varying_random_cyclic_time 
	  || curCommand->trigger.sourceEvent == varying_cyclic_time )
	&& curCommand->trigger.sourcetimelimit <= time ) {

      // sets the command internal state
      if( curCommand->initialCarrierNode ) {
	StrCpy( &carrierInternalState ,  curCommand->initialCarrierNode->internalState );
	// printf("\nevent %d carrier state [%s] expected state [%s]\n" , curCommand->trigger.sourceEvent ,carrierInternalState , curCommand->trigger.expectedState );
      }
      else {
	InitString( &carrierInternalState );
	// printf("\nevent %d void receiver  expected state [%s]\n" , curCommand->trigger.sourceEvent , curCommand->trigger.expectedState );
      }

      // check the internal state of the 
      if( !(*(curCommand->trigger.expectedState)) 
	  || !(*carrierInternalState) 
	  || booleanStringExpression( curCommand->trigger.expectedState , 
				      curCommand->trigger.stateOperatorValue , 
				      carrierInternalState ) ) {
	// execution of the command and its synchronous commands 
	// triggered by the same event or time limit
	// printf( "time source %.2f current %.2f type %d \n" , curCommand->trigger.sourcetimelimit , time , curCommand->trigger.sourceEvent);
	curCommand->execute( staticReferenceResolution , false , (char *)"" );
	synchronous = curCommand->synchronousCommand;
	while( synchronous ) {
	  // printf( "synchronous time source %.2f current %.2f type %d \n" , synchronous->trigger.sourcetimelimit , time , synchronous->trigger.sourceEvent);
	  synchronous->execute( staticReferenceResolution , false , (char *)"" );
	  synchronous = synchronous->synchronousCommand;
	}
	commandExecuted = true;
	singleCommandExecuted = true;
      }

      if( singleCommandExecuted ) {
	// scheduled commands: one execution
	// time commands are removed after exec because 
	// they are only executed once
	if( curCommand->trigger.sourceEvent == time_limit ) {
	  if( precCommand == NULL ) {
 	    //printf( "Initial command removed \n");
	    commandList = curCommand->nextCommand;
	    curCommand = commandList;
	  }
	  else {
 	    //printf( "Subsequent command removed\n");
	    // 	    precCommand->print(stdout,1,1);
	    // 	    if( curCommand->nextCommand ) {
	    // 	      curCommand->nextCommand->print(stdout,1,1);
	    // 	    }
	    precCommand->nextCommand = curCommand->nextCommand;
	    curCommand = curCommand->nextCommand;
	  }
	  // preCommand and curCommand do not change
	}
	// scheduled commands: one execution

	// cyclic time commands are only executed once per frame
	// cyclic time commands are rescheduled at each frame
	else {
	  if( curCommand->trigger.sourceEvent == cyclic_time ) {
	    curCommand->trigger.sourcetimelimit 
	      = time + curCommand->trigger.sourceperiod;
	    // printf( "new time source %.5f \n" , curCommand->trigger.sourcetimelimit);
	  }
	  else if( curCommand->trigger.sourceEvent == random_cyclic_time ) {
	    curCommand->trigger.sourcetimelimit 
	      = time + generateRandomFloat( 0 , curCommand->trigger.sourceperiod );
	    //printf( "new time source %.2f \n" , curCommand->trigger.sourcetimelimit);
	  }
	  else if( curCommand->trigger.sourceEvent == varying_cyclic_time ) {
	    curCommand->trigger.sourceperiod *= curCommand->trigger.sourcevariation;
	    if( DM_EnvironmentNode->time_step != 0 ) {
	      if( curCommand->trigger.sourceperiod 
		  < DM_EnvironmentNode->time_step ) {
		curCommand->trigger.sourceperiod 
		  = DM_EnvironmentNode->time_step;
	      }
	    }
	    curCommand->trigger.sourcetimelimit 
	      = time + curCommand->trigger.sourceperiod;
	    //printf( "new time source %.2f \n" , curCommand->trigger.sourcetimelimit);
	  }
	  else if( curCommand->trigger.sourceEvent == varying_random_cyclic_time ) {
	    curCommand->trigger.sourceperiod *= curCommand->trigger.sourcevariation;
	    if( DM_EnvironmentNode->time_step != 0 ) {
	      if( curCommand->trigger.sourceperiod 
		  < DM_EnvironmentNode->time_step ) {
		curCommand->trigger.sourceperiod 
		  = DM_EnvironmentNode->time_step;
	      }
	    }
	    curCommand->trigger.sourcetimelimit 
	      = time + generateRandomFloat( 0 , curCommand->trigger.sourceperiod );
	    //printf( "new time source %.2f \n" , curCommand->trigger.sourcetimelimit);
	  }
	  // printf( "skip command \n");
	  precCommand = curCommand;
	  curCommand = curCommand->nextCommand;
	}
	// cyclic time commands are only executed once per frame
      }
      // command not executed
      else {
	precCommand = curCommand;
	curCommand = curCommand->nextCommand;
      }
    }
    // non scheduled command
    else {
      precCommand = curCommand;
      curCommand = curCommand->nextCommand;
    }
  }

  if (carrierInternalState)
  {
	  delete [] carrierInternalState;
  }
  return commandExecuted;
}

dm_Command * ParseCommand( int *fileDepth , int *curChar , 
			   dm_SceneNode *sourceNode , bool requireTrigger ,
			   char *chaine , int *ind ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  dm_Command    *auxCommand;
  dm_Command    *headOfCommandList;
  dm_Command    *parsedCommand;
  dm_Trigger     parsedTrigger;

  auxCommand = NULL;
  headOfCommandList = NULL;
  parsedCommand = new dm_Command();

  bool triggerParsed = false;
  parsedTrigger.clean();

  // new tag XML 
  XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
  
  // printf( "parsecommand sourceNode (%s)\n" , NodeIDOrEmptyString( sourceNode ) );

  while( true ) {
    /////////////////////////
    // command closing tag
    if( strcmp( XMLtag->tag , "command" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {
      if( !triggerParsed && requireTrigger ) {
	sprintf( ErrorStr , "Error: missing command trigger in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ;
	delete XMLtag;
	XMLtag = NULL;
	throw 298;
      }

      // in case of message event, processes the pattern
      // and stores the message address
      // message that can contain a pattern
      if( parsedTrigger.sourceEventString[0] == '/' ) {
	PatternProcessing( headOfCommandList , 
			   parsedTrigger.sourceEventString ,
			   false );
	//printf( "Trigger (%s)\n" , parsedTrigger.sourceEventString );
	strcpy( parsedTrigger.sourceEventString ,
		headOfCommandList->CommandAliasOSCTag );
	//printf( "Trigger (%s)\n" , parsedTrigger.sourceEventString );
      }

      // associates all the commands with the parsed trigger
      auxCommand = headOfCommandList;
      while( auxCommand ) {
	auxCommand->trigger = parsedTrigger;
	auxCommand = auxCommand->synchronousCommand;
      }

      delete XMLtag;
      XMLtag = NULL;

      return headOfCommandList;
    }

    /////////////////////////
    // trigger
    else if( strcmp( XMLtag->tag , "trigger" ) == 0 
	     && (XMLtag->tagType == EmptyTag) ) {
      if( triggerParsed ) {
	sprintf( ErrorStr , "Error: multiple trigger in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
	delete XMLtag;
	XMLtag = NULL;
	throw 298;
      }

      // parsing the triggers
      if( !parsedTrigger.ParseTrigger( sourceNode , 
				       fileDepth , curChar , 
				       XMLtag ,
				       chaine , ind ) ) {
	sprintf( ErrorStr , "Error: incorrect command trigger in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
	delete XMLtag;
	XMLtag = NULL;
	throw 298;
      }
      triggerParsed = true;

      // new tag XML 
      XMLtag->deallocateXMLTag();
      XMLtag->ParseTagXML( fileDepth , curChar , true , chaine , ind );
  
    }

    /////////////////////////
    // action
    else if( (strcmp( XMLtag->tag , "action" ) == 0 
	      && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "repeatAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "whileAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "ifAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) )  ) {
      while( (strcmp( XMLtag->tag , "action" ) == 0 
	      && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "repeatAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "whileAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || ( strcmp( XMLtag->tag , "ifAction" ) == 0 
		  && (XMLtag->tagType == OpeningTag) )
	     || (strcmp( XMLtag->tag , "breakAction" ) == 0 
		 && (XMLtag->tagType == EmptyTag) )  ) {
	// cleaning the fields of the working command 
	parsedCommand->clean();
	
	// parsing the actions
	if( !parsedCommand->ParseCommandTail( fileDepth , curChar , 
					      XMLtag ,
					      sourceNode ,
					      chaine , ind ) ) {
	  sprintf( ErrorStr , "Error: incorrect command action in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
	  delete XMLtag;
	  XMLtag = NULL;
	  throw 298;
	}
	
	// chaining commands
	if( !headOfCommandList ) {
	  //headOfCommandList = new dm_Command();
	  headOfCommandList = parsedCommand;
	  //  	  printf ("headOfCommandList->targetNodeList[0] %d\n" , 
	  //  		  headOfCommandList->targetNodeList[0] );
	}
	else {
	  auxCommand = headOfCommandList;
	  while( auxCommand->synchronousCommand ) {
	    auxCommand = auxCommand->synchronousCommand;
	  }
	  //auxCommand->synchronousCommand = new dm_Command();
	  (auxCommand->synchronousCommand) = parsedCommand;
	  //  	  printf ("(auxCommand->synchronousCommand)->targetNodeList[0] %d\n" , 
	  //  		  (auxCommand->synchronousCommand)->targetNodeList[0] );
	}
	// no XML tag to read, next XML tag is read when parsing actions
      }
    }
    else {
      sprintf( ErrorStr , "Error: incorrect tag [%s,%d] in script command of node [%s]!" , XMLtag->tag , XMLtag->tagType , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ;
      delete XMLtag;
      XMLtag = NULL;
      throw 298;
    }

    // endless script
    if( *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing closing tag in script command of node [%s]!", NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
      delete XMLtag;
      XMLtag = NULL;
      throw 89;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
  return NULL;
}


dm_Script *ParseScript( int *fileDepth , int *curChar , 
			dm_SceneNode *sourceNode , char *ID ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();

  dm_Script     *returnScript;
  dm_Command    *auxCommand;
  dm_Command    *headOfCommandList;
  dm_Command    *commandList;

  returnScript = NULL;
  auxCommand = NULL;
  headOfCommandList = NULL;
  commandList = NULL;
  
  returnScript = new dm_Script( ID );

  while( true ) {
    // endless script
    if( *curChar == EOF ) {
      sprintf( ErrorStr , "Error: missing closing tag in script of node [%s]!" , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
      delete XMLtag;
      XMLtag = NULL;
      throw 89;
    }

    // new tag XML : closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , curChar , true , NULL , NULL );
    
    // node closing tag
    if( strcmp( XMLtag->tag , "script" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {
      returnScript->commandList = headOfCommandList;
      // returnScript->print( stdout , true , 1 );
      delete XMLtag;
      XMLtag = NULL;

      return returnScript;
    }
    else if( strcmp( XMLtag->tag , "command" ) == 0 
	     && (XMLtag->tagType == OpeningTag) ) {
      // reading a script node
      commandList = ParseCommand( fileDepth , curChar , sourceNode , true ,
				  NULL , NULL );
      
      // chaining commands
      if( !headOfCommandList ) {
	headOfCommandList = commandList;
      }
      else {
	auxCommand = headOfCommandList;
	while( auxCommand->nextCommand ) {
	  auxCommand = auxCommand->nextCommand;
	}
	auxCommand->nextCommand = commandList;
      }
    }
    else {
      sprintf( ErrorStr , "Error: incorrect tag [%s] in script of node [%s]!" , XMLtag->tag , NodeIDOrEmptyString( sourceNode ) ); ReportError( ErrorStr ) ; 
      delete XMLtag;
      XMLtag = NULL;
      throw 298;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_SceneNode::gatherScripts( dm_Command **commands ) {
  // scene node with a script
  if( script ) {
    dm_Command *curLocalCommand = script->commandList;
    dm_Command **handleLocalCommand = &(script->commandList);
    dm_Command *nextLocalCommand = NULL;

    while( curLocalCommand ) {
      nextLocalCommand = curLocalCommand->nextCommand;

      // collecting at a global level the commands that are
      // not specific to an object
      if( curLocalCommand->trigger.sourceEvent == keystroke 
	  || curLocalCommand->trigger.sourceEvent == time_limit
	  || curLocalCommand->trigger.sourceEvent == random_cyclic_time 
	  || curLocalCommand->trigger.sourceEvent == cyclic_time
	  || curLocalCommand->trigger.sourceEvent == varying_random_cyclic_time 
	  || curLocalCommand->trigger.sourceEvent == varying_cyclic_time ) {

	// reminds the node that carried the command
	// because the command execution can depend on this node's state
	curLocalCommand->initialCarrierNode = this;

	// chaining the local command with the global ones
	if( *commands ) {
	  dm_Command *curGatheredCommand = NULL;
	  curGatheredCommand = *commands;
	  while( curGatheredCommand->nextCommand ) {
	    curGatheredCommand = curGatheredCommand->nextCommand;
	  }
	  curGatheredCommand->nextCommand = curLocalCommand;
	}
	else {
	  (*commands) = curLocalCommand;
	}
	curLocalCommand->nextCommand = NULL;
	// and removing the local command from the local list
	(*handleLocalCommand) = nextLocalCommand;
	  }
      else {
	handleLocalCommand = &((*handleLocalCommand)->nextCommand);
	  }
      curLocalCommand = nextLocalCommand;
	}
  }

  // recursive call
  if( daughter ) {
    daughter->gatherScripts( commands );
  }
  if( sister ) {
    sister->gatherScripts( commands );
  }
}

void dm_Scene::gatherScripts( dm_Command **commands ) {
  if( root ) {
    root->gatherScripts( commands );
  }
}

void dm_BaseAction::findScriptReferences( dm_SceneNode *rootNode, char *targetID) {
  if( targetData  
      && targetData->type == ANodeLink ) {
    int nbLinks = 0;
    // atRunTime is set to false because 
    // these references are resolved at run time

    ////////////////////////
    // string that contains expressions: must be 
    // resolved dynamically

    // dynamic reference
    if( *(targetData->id) && containsExpression( targetData->id ) ) {
      targetDataStatus = DynamicNodeElement;
    }

    // static reference
    else {
      dm_SceneNode **auxNode = findAllReferences( rootNode , 
						  targetData->id , &nbLinks , 
						  true , true );
      targetDataStatus = NodeElement;

      if( auxNode ) {
	targetData = *(auxNode);
	delete [] auxNode;
	auxNode = NULL;
      }
      
      // post processing of special references
      switch( actionID ) {
	// viewpoint reference -> viewpoit index
      case set_viewpoint :
      case set_current_viewpoint :
	if( targetData ) {
	  for( int ind = 0 ; ind < DM_CompositionNode->nbMaxViewpoints ; ind++ ) {
	    if( ((dm_Viewpoint *)(targetData->sceneObject)) 
		== DM_CompositionNode->viewpoints[ind] ) {
	      targetIntValue = ind;
	      break;
	    }
	  }
	}
	if( targetIntValue < 0 ) {
	  sprintf( ErrorStr , "Error: unknown viewpoint [%s]!" , targetData->id ); ReportError( ErrorStr ) ;  throw 289;
	}
	// printf( "Target viewpoint %d\n" , targetIntValue );
	// unknown viewpoint -> default viewpoint
	break;
      default:
	break;
      }
    }
    // static reference
  }
}

void dm_ElemAction::findScriptReferencesDynamic( void ) {
  bool stopIfOneIsFound = (targetStatus == EmptyNodeElement
			   || targetStatus == NodeElement 
			   || targetStatus == DynamicNodeElement);
  int nbNodes = 0;
  dm_SceneNode ** auxNode = NULL;

  auxNode = findReferencesDynamic( targetNodeidRef , &nbNodes ,
				   stopIfOneIsFound , true );

  // printf("Nb discovered Nodes %d\n" , nbNodes );

  if( auxNode ) {
    // allocates a list of target nodes that
    // will be instantiated by the actual pointers in
    // the scene graph
    targetNodeList = new PSceneNode[ nbNodes ];
    
    // copies the nodes in the current command targetNodeList
    nbTargetNodes = 0;
    for( int indAux = 0 ; indAux < nbNodes ; indAux++ ) {
      // printf("    targeted Nodes %d\n" , indAux );
      targetNodeList[ nbTargetNodes++ ] = auxNode[ indAux ];
      if( stopIfOneIsFound ) {
	// throws away the nodes that were allocated during
	// the resolution of the reference
	delete [] auxNode;
	auxNode = NULL;
	return;
      }
    }

    // throws away the nodes that were allocated during
    // the resolution of the reference
    delete [] auxNode;
    auxNode = NULL;
  }
}

void dm_ElemAction::findScriptReferences( dm_SceneNode *rootNode, char *targetID ) {

  //printf("findScriptReferences\n" );
  //printf("TargetNode (%s)\n" , targetNodeidRef);
  ////////////////////////
  // string that contains expressions: must be 
  // resolved dynamically
  if( *targetNodeidRef && containsExpression( targetNodeidRef ) ) {
    // targetNodeidRef will be used to store the string with
    // substituted expressions for dynamic ref resolutin
    //printf("Requires dynamic referencing\n");
    strcpy( targetNodeid , targetNodeidRef );
    if( targetStatus == EmptyNodeElement ) {
      targetStatus = DynamicNodeElement;
    }
    else if( targetStatus == EmptyNodeList ) {
      targetStatus = DynamicNodeList;
    }
  }
  ////////////////////////
  // string without expressions: can be resolved statically
  // single node
  else if( ( targetStatus == EmptyNodeElement && *targetNodeidRef )  ||
	   ( targetStatus == NodeElement && targetID)) {
      if ((targetID != NULL)  && (targetNodeList != NULL))
	{
	  nbTargetNodes = 0;
	  delete [] targetNodeList;
	  targetNodeList = NULL;
	}

      // printf("TargetNodes EmptyNodeElement %s\n" , targetNodeidRef);
      targetStatus = NodeElement;

      // saves the link node
      int nbTargetNodesConfig = 0;
      // atRunTime is set to false because 
      // these references are resolved at run time
      dm_SceneNode **auxNodeConfiguration =
	findAllReferences( envtNode , 
			   targetNodeidRef , &nbTargetNodesConfig , 
			   true , false );
	
      // printf("nbTargetNodesConfig %d ID %s\n" , nbTargetNodesConfig , targetNodeList[0]->id);
      int nbTargetNodesAux = 0;
      // atRunTime is set to false because 
      // these references are resolved at run time
      dm_SceneNode **auxNode = findAllReferences( rootNode , 
						  targetNodeidRef , &nbTargetNodesAux , 
						  true , false );
      // printf("nbTargetNodesAux %d\n" , nbTargetNodesAux);
      if( auxNode || auxNodeConfiguration ) {
	if( !auxNodeConfiguration ) {
	  nbTargetNodes = nbTargetNodesAux;
	  targetNodeList = auxNode;
	}
	else if( !auxNode ) {
	  nbTargetNodes = nbTargetNodesConfig;
	  targetNodeList= auxNodeConfiguration;
	}
	else {
	  nbTargetNodes = nbTargetNodesAux + nbTargetNodesConfig;
	  targetNodeList = new dm_SceneNode* [ nbTargetNodes ];
	  for( int ind = 0 ; ind < nbTargetNodesAux ; ind++ ) {
	    targetNodeList[ ind ] = auxNode[ind ];
	  }
	  for( int ind = nbTargetNodesAux ; 
	       ind < nbTargetNodesAux + nbTargetNodesConfig ; ind++ ) {
	    targetNodeList[ ind ] = auxNodeConfiguration[ind - nbTargetNodesAux ];
	  }
	  delete [] auxNode;
	  auxNode = NULL;

	  delete [] auxNodeConfiguration;
	  auxNodeConfiguration = NULL;
	}
      }
      // printf("nbTargetNodes %d ID %s\n" , nbTargetNodes , targetNodeList[0]->id);
    }
  ////////////////////////
  // string without expressions: can be resolved statically
  // node list
  else if( ( targetStatus == EmptyNodeList && *targetNodeidRef ) ||
	   (targetStatus == NodeList && targetID)) {
      if ((targetID != NULL) && (targetNodeList != NULL))
	{
	  nbTargetNodes = 0;
	  delete [] targetNodeList;
	  targetNodeList = NULL;
	}

      // printf("TargetNodes EmptyNodeList %s %s\n" , targetNodeList[0]->id , NodeTypeString[targetNodeList[0]->type]);
      targetStatus = NodeList;
    
      // printf("TargetNodes findAllReferences\n");
      // counts the number of nodes with this id and stores them
      int nbTargetNodesConfig = 0;
      // atRunTime is set to false because 
      // these references are resolved at run time
      dm_SceneNode **auxNodeConfiguration =
	findAllReferences( envtNode , 
			   targetNodeidRef , &nbTargetNodesConfig , 
			   false , false );
      int nbTargetNodesAux = 0;
      dm_SceneNode **auxNode = findAllReferences( rootNode , 
						  targetNodeidRef , &nbTargetNodesAux , 
						  false , false );
      if( auxNode || auxNodeConfiguration ) {
	if( !auxNodeConfiguration ) {
	  nbTargetNodes = nbTargetNodesAux;
	  targetNodeList = auxNode;
	}
	else if( !auxNode ) {
	  nbTargetNodes = nbTargetNodesConfig;
	  targetNodeList = auxNodeConfiguration;
	}
	else {
	  nbTargetNodes = nbTargetNodesAux + nbTargetNodesConfig;
	  targetNodeList = new dm_SceneNode* [ nbTargetNodes ];
	  for( int ind = 0 ; ind < nbTargetNodesAux ; ind++ ) {
	    targetNodeList[ ind ] = auxNode[ind ];
	  }
	  for( int ind = nbTargetNodesAux ; 
	       ind < nbTargetNodesAux + nbTargetNodesConfig ; ind++ ) {
	    targetNodeList[ ind ] = auxNodeConfiguration[ind - nbTargetNodesAux ];
	  }
	  delete [] auxNode;
	  auxNode = NULL;

	  delete [] auxNodeConfiguration;
	  auxNodeConfiguration = NULL;
	}
      }
      // printf("nbTargetNodes %d ID %s\n" , nbTargetNodes , targetNodeList[0]->id);
    }
  // printf("End of targetNodeList\n");

  dm_BaseAction * ptAction = actions;
  while( ptAction ) {
    ptAction->findScriptReferences( rootNode, targetID);
    ptAction  = ptAction->nextActionForTheSameTarget;
  }
}



void dm_Command::findScriptReferences( dm_SceneNode *rootNode, char *targetID ) {
  dm_Action * ptAction = actions;
  while( ptAction ) {
    ptAction->findScriptReferences( rootNode, targetID);
    ptAction  = ptAction->nextAction;
  }
}


void dm_Script::findScriptReferences( dm_SceneNode *rootNode, char* targetID ) {
  dm_Command *curCommand = commandList;
  dm_Command *synchronous = NULL;
  while( curCommand ) {
    curCommand->findScriptReferences( rootNode, targetID );
    synchronous = curCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->findScriptReferences( rootNode, targetID );
      synchronous = synchronous->synchronousCommand;
    }
    curCommand = curCommand->nextCommand;
  }
}

void dm_SceneNode::findAllScriptReferences( dm_SceneNode *rootNode, char * targetID ) {
  if( script ) {
    script->findScriptReferences( rootNode, targetID);
  }
  if( renderingCommand ) {
    renderingCommand->findScriptReferences( rootNode );
    dm_Command *synchronous = renderingCommand->synchronousCommand;
    while( synchronous ) {
      synchronous->findScriptReferences( rootNode );
      synchronous = synchronous->synchronousCommand;
    }
  }

  // update interpolated variables
  dm_Scalar *varAux = lisScalars;
  while( varAux ) {
    if( varAux->interp ) {
      varAux->interp->findAllScriptReferences( rootNode, targetID );
    }
    varAux = (dm_Scalar *)varAux->nextVariable;
  }

  dm_Table *tabAux = lisTables;
  while( tabAux ) {
    if( tabAux->interp ) {
      tabAux->interp->findAllScriptReferences( rootNode, targetID );
    }
    tabAux = (dm_Table *)tabAux->nextVariable;
  }

  dm_Matrix *matAux = lisMatrices;
  while( matAux ) {
    if( matAux->interp ) {
      matAux->interp->findAllScriptReferences( rootNode, targetID );
    }
    matAux = (dm_Matrix *)matAux->nextVariable;
  }

  if( daughter ) {
    daughter->findAllScriptReferences( rootNode, targetID );
  }
  if( sister ) {
    sister->findAllScriptReferences( rootNode, targetID );
  }
}

void dm_Scene::findAllScriptReferences( char *targetID ) {
  if( root ) {
    root->findAllScriptReferences( root, targetID );
  }
}

