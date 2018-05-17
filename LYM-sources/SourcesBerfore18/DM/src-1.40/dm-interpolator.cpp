/*! \file dm-interpolator.cpp
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-interpolator.cpp
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

const char *ScheduleModeString[EmptySchedule + 1] = { "linear" , "sinus" , "linear-periodic" , "sinus-periodic" , "linear-random" , "sinus-random" , "polynomial", "emptyschedule" };

const char *FillModeString[EmptyFillMode + 1] = { "remove" , "freeze" , "hide" , "show" , "emptyfillmode" };

const char *InterpolatorTypeString[EmptyInterpolatorType + 1] = { "absolute-interpolation" , "stepwise-interpolation" , "emptyinterpolatortype" };

///////////////////////////////////////////////////////////
// SCHEDULE
///////////////////////////////////////////////////////////

dm_Schedule::dm_Schedule( void ) {
  clean();
}

dm_Schedule::~dm_Schedule(void) {
	/*
	if (completionCommand)
	{
		delete completionCommand;
		completionCommand = NULL;
	}
	
	if (updateCommand)
	{
		delete updateCommand;
		updateCommand = NULL;
	}
	*/
}

void dm_Schedule::clean( void ) {
  beginTime = Nil;
  period = Nil;
  repeatCount = Nil;
  paramSinusPolynom = Nil;
  mode = EmptySchedule;
  fill = EmptyFillMode;
  before = EmptyFillMode;
  indObjectIni = Nil;
  indObjectEnd = Nil;
  totalLength = 0;
  completionCommand = NULL;
  isCompletionScriptEnabled = true;
  updateCommand = NULL;
  isUpdateScriptEnabled = true;
  completionExecuted = false;
  coef = 1.0;
  precedingCoef = -1.0;
  coefComputed = false;
}
void dm_Schedule::defaultValues( void ) {
  beginTime = 0;
  period = 10;
  repeatCount = 100;
  paramSinusPolynom = 1.0;
  mode = SINUS_PERIODIC;
  fill = FREEZE;
  before = SHOW;
  indObjectIni = Nil;
  indObjectEnd = Nil;
  totalLength = 0;
  completionCommand = NULL;
  isCompletionScriptEnabled = true;
  updateCommand = NULL;
  isUpdateScriptEnabled = true;
  completionExecuted = false;
  coef = 1.0;
  precedingCoef = -1.0;
  coefComputed = false;
}

void dm_Schedule::setParameters( char *ID , int size ,
			      dm_XMLTag *XMLtag , bool atRunTime ,
			      FloatBinOp oper ,
			      IntBinOp operI ) {
  //printf( "Parse schedule [%s]  size [%d]\n" , ID , size );

  if( XMLtag->hasAttribute( "mode" ) ) {
    char *mode_str = 
      XMLtag->getAttributeStringValue( "mode" );
    mode = EmptySchedule;
    for( int ind = 0 ; ind < EmptySchedule ; ind++ ) {
      if( strcmp( mode_str , ScheduleModeString[ind] ) == 0 ) {
	mode = (ScheduleMode)ind;
	break;
      }
    }
    if( mode == EmptySchedule ) {
      sprintf( ErrorStr , "Error: unknown schedule mode [%s] in component ID [%s]!" , mode_str , ID ); ReportError( ErrorStr ); throw 58;
    }
  }

  if( XMLtag->hasAttribute( "begin" ) ) {
      oper( beginTime ,
	    (float)XMLtag->getAttributeDoubleValue( "begin" , 
					    atRunTime ));
  }

  if( XMLtag->hasAttribute( "dur" ) ) {
    oper( period , 
	  (float)XMLtag->getAttributeDoubleValue( "dur" , 
					  atRunTime ));
  }

  if( XMLtag->hasAttribute( "coef" ) ) {
    float coefFloat = (float)coef;
    oper( coefFloat , 
	  (float)XMLtag->getAttributeDoubleValue( "coef" , 
					  atRunTime ));
    coef = (double)coefFloat;
    // printf("coef %.3f\n" , coefFloat );
    if( coef < 0.0 ) {
      coef -= floor( coef );
    }
    coef = fmod( coef , 1.0 );
    coefComputed = true;
    // printf("coef updated %.3f\n" , coef );
  }

  if( XMLtag->hasAttribute( "before" ) ) {
    char *beforeMode = 
      XMLtag->getAttributeStringValue( "before" );
    before = EmptyFillMode;
    for( int ind = 0 ; ind < EmptyFillMode ; ind++ ) {
      if( strcmp(beforeMode , FillModeString[ind] ) == 0 ) {
	before = (FillMode)ind;
	break;
      }
    }
    if( before == EmptyFillMode ) {
      sprintf( ErrorStr , "Error: unknown schedule before mode [%s] in component ID [%s]!" , beforeMode , ID ); ReportError( ErrorStr ); throw 58;
    }
  }

  if( XMLtag->hasAttribute( "fill" ) ) {
    char *fillMode = 
      XMLtag->getAttributeStringValue( "fill" );
    fill = EmptyFillMode;
    for( int ind = 0 ; ind < EmptyFillMode ; ind++ ) {
      if( strcmp(fillMode , FillModeString[ind] ) == 0 ) {
	fill = (FillMode)ind;
	break;
      }
    }
    if( fill == EmptyFillMode ) {
      sprintf( ErrorStr , "Error: unknown schedule fill mode [%s] in component ID [%s]!" , fillMode , ID ); ReportError( ErrorStr ); throw 58;
    }
  }

  if( XMLtag->hasAttribute( "repeatCount" ) ) {
    oper( repeatCount ,
	  (float)XMLtag->getAttributeDoubleValue( "repeatCount" , 
					  atRunTime ));
  }
  else if( XMLtag->hasAttribute( "repeatDur" ) && period > 0.0) {
    oper( repeatCount ,
	  (float)XMLtag->getAttributeDoubleValue( "repeatDur" , 
					  atRunTime ));
    if( oper == assign ) {
      repeatCount /= period;
    }
  }
  
  if( XMLtag->hasAttribute( "param_sinus_polynom" ) ) {
    oper( paramSinusPolynom ,
	  (float)XMLtag->getAttributeDoubleValue( "param_sinus_polynom" , 
					  atRunTime ));
    //printf( "Read paramSinusPolynom (%.3f)\n" , paramSinusPolynom );
  }

  if( XMLtag->hasAttribute( "ini" ) ) {
    operI( indObjectIni ,
	   XMLtag->getAttributeIntValue( "ini" , 
				 atRunTime ));
    if( operI == assignI ) {
      indObjectIni -= 1;
    }
  }
  else {
    if( !atRunTime ) {
      indObjectIni = 0;
    }
  }

  if( XMLtag->hasAttribute( "end" ) ) {
    operI( indObjectEnd ,
	   XMLtag->getAttributeIntValue( "end" , 
				 atRunTime ));
    if( operI == assignI ) {
      indObjectEnd -= 1;
    }
  }
  else {
    if( !atRunTime ) {
      indObjectEnd = size - 1;
    }
  }

  if( !atRunTime ) {
    isCompletionScriptEnabled = true;
  }
  if( XMLtag->hasAttribute( "isCompletionScriptEnabled" ) ) {
    XMLtag->BooleanFieldValue ( "isCompletionScriptEnabled" ,
			&isCompletionScriptEnabled );
  }

  if( !atRunTime ) {
    isUpdateScriptEnabled = true;
  }
  if( XMLtag->hasAttribute( "isUpdateScriptEnabled" ) ) {
    XMLtag->BooleanFieldValue ( "isUpdateScriptEnabled" ,
			&isUpdateScriptEnabled );
  }
}

bool dm_Schedule::getParameters( char *attribute , 
				    DataType * dataType ,
				    double * valDouble ,
				    int * valInt ,
				    bool * valBool ,
				    char ** valString ,
			      int * tableOrMatrixSize ,
			      dm_ValScalar ** valScalars ) {
  if( strcmp( attribute , "mode" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)ScheduleModeString[mode];
    return true;
  }
  else if( strcmp( attribute , "begin" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)beginTime;
    return true;
  }
  else if( strcmp( attribute , "dur" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)period;
    return true;
  }
  else if( strcmp( attribute , "before" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)FillModeString[before];
    return true;
  }
  else if( strcmp( attribute , "fill" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)FillModeString[fill];
    return true;
  }
  else if( strcmp( attribute , "repeatCount" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)repeatCount;
    return true;
  }
  else if( strcmp( attribute , "repeatDur" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)(repeatCount * period);
    return true;
  }
  else if( strcmp( attribute , "coef" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)coef;
    return true;
  }
  else if( strcmp( attribute , "param_sinus_polynom" ) == 0 ) {
    *dataType = DataTypeDouble;
    *valDouble = (double)(repeatCount * period);
    return true;
  }
  else if( strcmp( attribute , "ini" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = indObjectIni;
    return true;
  }
  else if( strcmp( attribute , "end" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = indObjectEnd;
    return true;
  }
  else if( strcmp( attribute , "isCompletionScriptEnabled" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = isCompletionScriptEnabled;
    return true;
  }
  else if( strcmp( attribute , "isUpdateScriptEnabled" ) == 0 ) {
    *dataType = DataTypeBool;
    *valBool = isUpdateScriptEnabled;
    return true;
  }
  return false;
}

void dm_Schedule::parse( int *fileDepth , int *p_c , 
			 dm_SceneNode *sourceNode ) {
  // in case of opening tag, the schedule can contain
  // a completion script
  // in this case it should be the last schedule in the list
  dm_XMLTag *XMLtag = new dm_XMLTag();
    
  while( true ) {
    // new tag XML : completion script
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
    if( ( strcmp( XMLtag->tag , "schedule" ) == 0 ) 
	&& (XMLtag->tagType == ClosingTag) ) {
      delete XMLtag;
	  XMLtag = NULL;

      return;
    }
    else if( ( strcmp( XMLtag->tag , "completionScript" ) == 0 ) 
	     && (XMLtag->tagType == OpeningTag) ) {
	
      // there is no trigger: this command is triggered by the
      // schdule completion
      
      // XML syntax for completion scripts
      {
	// new tag XML : opening action element
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
	
	if( ( strcmp( XMLtag->tag , "command" ) == 0 ) && (XMLtag->tagType == OpeningTag) ) {
	  ////////////////////////
	  // parsing the associated action(s)
	  completionCommand = ParseCommand( fileDepth , p_c , 
					    sourceNode , false , NULL , NULL );
	}
	else {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: incorrect tag [%s] in completion script of node [%s]!" , XMLtag->tag , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
	}
      }

      // expecting script closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  "completionScript", NULL , NULL  );
    }
    else if( ( strcmp( XMLtag->tag , "updateScript" ) == 0 ) 
	     && (XMLtag->tagType == OpeningTag) ) {
      // there is no head: this command is triggered by the
      // schdule update
      
      // XML syntax for completion scripts
      {
	// new tag XML : opening action element
	XMLtag->deallocateXMLTag();
	XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );
	
	if( ( strcmp( XMLtag->tag , "command" ) == 0 ) && (XMLtag->tagType == OpeningTag) ) {
	  ////////////////////////
	  // parsing the associated action(s)
	  updateCommand = ParseCommand( fileDepth , p_c , 
					sourceNode , false  , NULL , NULL );
	}
	else {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: incorrect tag [%s] in update script of node [%s]!" , XMLtag->tag , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
	}
	  }

      // expecting script closing tag
      ExpectClosingTagXML( fileDepth , p_c ,  "updateScript" , NULL , NULL );
	}
    else {
      // unknown closing tag
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: incorrect schedule script!" ); ReportError( ErrorStr ); throw 317;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_Schedule::print( FILE *file , int isLong , int depth ) {
  indent( file , depth );
  fprintf( file , "<schedule begin=\"%.2f\" dur=\"%.2f\" mode=\"%s\" before=\"%s\" fill=\"%s\"" , 
	   beginTime , period , ScheduleModeString[mode] , 
	   FillModeString[before] , FillModeString[fill] );
  if( repeatCount > 0 ) {
    fprintf( file , " repeatCount=\"%.2f\"" , repeatCount );
  }
  if( paramSinusPolynom != 0 ) {
    fprintf( file , " paramSinusPolynom=\"%.2f\"" , paramSinusPolynom );
  }
  if( indObjectIni >= 0 ) {
    fprintf( file , " ini=\"%d\"" , indObjectIni + 1 );
  }
  if( indObjectEnd >= 0 ) {
    fprintf( file , " end=\"%d\"" , indObjectEnd + 1 );
  }
  if( !completionCommand && !updateCommand ) {
    fprintf( file , "/>\n" );
  }  
  else {
    dm_Command *synchronous = NULL;
    
    fprintf( file , ">\n" );
      
    if( completionCommand ) {
      indent( file , depth + 1 );
      fprintf( file , "<completionScript>\n" );
      completionCommand->print( file , true , depth + 2 );
      synchronous = completionCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->print( file , true , depth + 2 );
	synchronous = synchronous->synchronousCommand;
      }
      indent( file , depth );
      fprintf( file , "</completionScript>\n" );
    }
    if( updateCommand ) {
      indent( file , depth + 1 );
      fprintf( file , "<updateScript>\n" );
      updateCommand->print( file , true , depth + 2 );
      synchronous = updateCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->print( file , true , depth + 2 );
	synchronous = synchronous->synchronousCommand;
      }
      indent( file , depth );
      fprintf( file , "</updateScript>\n" );
    }
  }
}

float dm_Schedule::endTime( void ) {
  if( repeatCount <= 0.0 ) {
    return beginTime + period;
  }
  else {
    return beginTime + period * repeatCount;
  }
}

void dm_Schedule::operator=(dm_Schedule& s) {
  beginTime = s.beginTime;
  period = s.period;
  repeatCount = s.repeatCount;
  paramSinusPolynom = s.paramSinusPolynom;
  mode = s.mode;
  before = s.before;
  coefComputed = s.coefComputed;
  fill = s.fill;
  indObjectIni = s.indObjectIni;
  indObjectEnd = s.indObjectEnd;
  totalLength = s.totalLength;
  completionCommand = s.completionCommand;
  isCompletionScriptEnabled = s.isCompletionScriptEnabled;
  updateCommand = s.updateCommand;
  isUpdateScriptEnabled = s.isUpdateScriptEnabled;
  completionExecuted = s.completionExecuted;
  coef = s.coef;
  precedingCoef = s.precedingCoef;
}

void dm_Schedule::percentageFunction( double *coef ) {
  // a value is given between 0 and 1
  // a new value is returned depending on the parameters of the
  // schedule function
  // linear interpolation
  if( mode == LINEAR || mode == LINEAR_PERIODIC || 
      mode == LINEAR_RANDOM ) {
    // nothing to be done
  }
  // sinus interpolation
  else if( mode == SINUS || mode == SINUS_PERIODIC || 
	   mode == SINUS_RANDOM ) {
    for( int ind = 1 ; ind <= paramSinusPolynom && ind <= 3 ; ind++ ) {
      *coef = (sin( (*coef) * M_PI - M_PI_2 ) + 1.0) / 2.0;
    }
    if( *coef > 1.0 ) {
      *coef = 1.0;
    }
    if( *coef < 0.0 ) {
      *coef = 0.0;
    }
  }
  // sinus interpolation
  else if( mode == POLYNOMIAL ) {
    //printf( "coef %.2f " , *coef );
    *coef = pow( (double)*coef , (double)paramSinusPolynom );
    //printf( "polyn %.2f\n" , *coef );
  }
  else {
      sprintf( ErrorStr , "Error: schedule has unknown schedule function (%s)!" , ScheduleModeString[mode] ); ReportError( ErrorStr ); throw 202;
  }
}

///////////////////////////////////////////////////////////
// INTERPOLATOR
///////////////////////////////////////////////////////////

dm_Interpolator::dm_Interpolator( void ): dm_NonGeometricalObject() {
  lastUpdate = Nil;
  nbSchedules = 0;

  initialInterpolationCompleted = false;
  forceUpdate = false;
  
  schedules = NULL;
  nbComponents = 0;

  nbWeights = 0;
  indActiveWeight = 0;
  indPrecedingActiveWeight = 0;

  previousNumberElapsedPeriods = 0;
  tabWeights = NULL;
  interpolatorType = ABSOLUTE_INTERPOLATION;
  isReferencedWeights = false;
}

dm_Interpolator::dm_Interpolator( int size ): dm_NonGeometricalObject() {
  lastUpdate = Nil;
  nbSchedules = 0;

  initialInterpolationCompleted = false;
  forceUpdate = false;
  
  schedules = new PSchedule[ size ];
  for( int ind = 0 ; ind < size ; ind++ ) {
    schedules[ind] = NULL;
  }
  nbComponents = size;

  nbWeights = 0;
  indActiveWeight = 0;
  indPrecedingActiveWeight = 0;

  previousNumberElapsedPeriods = 0;
  tabWeights = NULL;
  interpolatorType = ABSOLUTE_INTERPOLATION;
  isReferencedWeights = false;
}

dm_Interpolator::~dm_Interpolator( void ) {
  // printf("weight deletion\n");
  if ((tabWeights) && (!isReferencedWeights)) {
    float** pTmpWgt = tabWeights;
    for (int ind = 0; ind < nbWeights; ind++) {
      if (*pTmpWgt) {
	delete [] *pTmpWgt;
	*pTmpWgt = NULL;
      }
      pTmpWgt ++;
      /*
	if (tabWeights[ind])
	{
	delete [] tabWeights[ind];
	tabWeights[ind] = NULL;
	}
      */
    }
    delete [] tabWeights;
    tabWeights = NULL;
  }
  
  // printf("schedule deletion\n");
  if (schedules) {
    dm_Schedule** pTmpScd = schedules;
    for (int ind = 0; ind < nbComponents; ind++) {
      if (*pTmpScd) {
	delete *pTmpScd;
	*pTmpScd = NULL;
      }
      pTmpScd ++;
      /*
	delete schreedules[ind];
	schedules[ind] = NULL;
      */
    }
    
    delete [] schedules;
    schedules = NULL;
    nbComponents = 0;
  }
  // printf("end of interpolator deletion\n");
}

void dm_Interpolator::setParameters( dm_XMLTag *XMLtag , bool atRunTime ,
		    FloatBinOp oper ,
                    IntBinOp operI ) {
  if( !atRunTime || 
      XMLtag->hasAttribute( "id" ) ) {
    SetId( XMLtag->getAttributeStringValue( "id" ) );
  }
  if( !atRunTime ) {
    nbComponents = XMLtag->getAttributeIntValue( "size" , atRunTime );
    if( nbComponents < 2 ) {
      sprintf( ErrorStr , "Error: too few components [%d] in interpolator ID [%s]!" , nbComponents , id ); ReportError( ErrorStr ); throw 35;
    }
  }

  // type of interpolator
  if( !atRunTime ) {
    interpolatorType = ABSOLUTE_INTERPOLATION;
  }
  if( XMLtag->hasAttribute( "interpolator_type" ) ) {
    char *type_str = XMLtag->getAttributeStringValue( "interpolator_type" );
    interpolatorType = EmptyInterpolatorType;
    for( int ind = 0 ; ind < EmptyInterpolatorType ; ind++ ) {
      if( strcmp( type_str , InterpolatorTypeString[ind] ) == 0 ) {
	interpolatorType = (InterpolatorType)ind;
	break;
      }
    }
    if( interpolatorType == EmptyInterpolatorType ) {
      sprintf( ErrorStr , "Error: unknown interpolator type [%s] in component ID [%s]!" , type_str , id ); ReportError( ErrorStr ); throw 408;
    }
  }
  if( interpolatorType == STEPWISE_INTERPOLATION 
      && nbComponents != 2 ) {
    sprintf( ErrorStr , "Error: non binary (%d) random stepwise interpolator in component ID [%s]!" , nbComponents , id ); ReportError( ErrorStr ); throw 409;
  }
}

bool dm_Interpolator::getParameters( char *attribute , 
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
    *valString = GetId();
    return true;
  }
  else if( strcmp( attribute , "size" ) == 0 ) {
    *dataType = DataTypeInt;
    *valInt = nbComponents;
    return true;
  }
  else if( strcmp( attribute , "interpolator_type" ) == 0 ) {
    *dataType = DataTypeString;
    *valString = (char *)InterpolatorTypeString[interpolatorType];
    return true;
  }
  return false;
}

void dm_Interpolator::parseSchedule( int *fileDepth , int *p_c , dm_XMLTag *XMLtag ,
				     dm_SceneNode *sourceNode ) {
  // numerical characteristics of the interpolation 
  // default values of the interpolation
  if( nbSchedules >= nbComponents ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: excesssive number of schedules [%d] in interpolator ID [%s]!" , nbSchedules , id ); ReportError( ErrorStr ); throw 143;
  }
  schedules[nbSchedules] = new dm_Schedule();
  schedules[nbSchedules]->defaultValues();
      
  schedules[nbSchedules]->setParameters( id ,
					 nbComponents ,
					 XMLtag , false , 
					 assign , assignI );
  if( XMLtag->tagType == OpeningTag ) {
    schedules[nbSchedules]->parse( fileDepth , p_c , sourceNode );
  }

  if( ( schedules[nbSchedules]->mode == LINEAR_RANDOM
	|| schedules[nbSchedules]->mode == SINUS_RANDOM  )
      && nbSchedules > 0 ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: random interpolators are only alowed for 2 sets of weights at schedule %d in interpolator ID [%s]!" , nbSchedules + 1 , id ); ReportError( ErrorStr ); throw 146;
  }
  if( nbSchedules > 0
      && schedules[nbSchedules]->indObjectIni !=
      schedules[nbSchedules - 1]->indObjectEnd) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: schedule discontinuity at schedules %d(->%d)/%d(%d<-) in interpolator ID [%s]!" , nbSchedules , schedules[nbSchedules]->indObjectIni , nbSchedules + 1 , schedules[nbSchedules - 1]->indObjectEnd , id ); ReportError( ErrorStr ); throw 144;
  }
  if( nbSchedules > 0 && 
      schedules[nbSchedules - 1]->fill != FREEZE ) {
    delete XMLtag;
	XMLtag = NULL;

    sprintf( ErrorStr , "Error: intermediary schedule #%d must be in freeze mode in interpolator ID [%s]!" , nbSchedules , id ); ReportError( ErrorStr ); throw 145;
  }
  (nbSchedules)++;
}

void dm_Interpolator::print( FILE *file , int isLong , int depth , char *typeString ) {
  indent( file , depth );
  fprintf( file , "<interpolator id=\"%s\" type=\"%s\" size=\"%d \"interpolator_type=\"%s\">\n" , 
	   id , typeString  , nbComponents ,
	   InterpolatorTypeString[interpolatorType]);

  for( int ind = 0 ; ind < nbSchedules ; ind++ ) {
    schedules[ind]->print( file , isLong , depth );
  }
  if( nbWeights > 0 && tabWeights ) {
    //printf( "|%d|\n" ,  tabWeights );
    indent( file , depth + 2 );
    fprintf( file , "<tabweights size=%d>\n" , nbWeights );
    for( int indWeight = 0 ; indWeight < nbWeights ; indWeight++ ) {
      indent( file , depth + 3 );
      fprintf( file , "<weights dim=%d index=%d>\n" , 
	       nbComponents , indWeight + 1 );
      indent( file , depth + 4 );
      //printf( "|%d|\n" , tabWeights[indWeight] );
      for( int indComponent = 0 ; indComponent < nbComponents ; 
	   indComponent++ ) {
	fprintf( file , "%.5f " , tabWeights[indWeight][indComponent] );
      }
      fprintf( file , "\n" );
      indent( file , depth + 3 );
      fprintf( file , "</weights>\n" );
    }
    indent( file , depth + 2 );
    fprintf( file , "</tabweights>\n" );
  }
}

void dm_Interpolator::computesActiveScheduleRankAndCoeficient( 
		dm_Schedule *schedule , 
		double *coef , double * timeSpentInSchedule ) {
  // the weighs are taken in a sequential order
  // time relative to a unit that consists of a single period
  double relativeTime = ((double)CurrentClockTime - (double)schedule->beginTime) / 
                       (double)schedule->period;
//   printf( "relativeTime (%.7f) CurrentClockTime (%.7f) beginTime (%.7f) endTime (%.7f) period (%.7f) repeatCount (%.7f)\n" , relativeTime , CurrentClockTime , schedule->beginTime , schedule->endTime() , schedule->period  , schedule->repeatCount  );

  // Manages computation unaccuracy
  // in case of a CurrentClockTime very close or equal to end time
  // rounds it to the closest integer so that it stays in the
  // correct period index
  if( relativeTime > (double)schedule->repeatCount ) {
    relativeTime = (double)schedule->repeatCount - 0.00000001;
  }

  // number of periods elapsed since the beginning
  int numberElapsedPeriods = (int)floor( relativeTime );
  (*timeSpentInSchedule) 
    = (double)CurrentClockTime - ((double)numberElapsedPeriods
				  * (double)schedule->period) 
    - (double)schedule->beginTime;
  //printf( "relativeTime (%.2f) numberElapsedPeriods (%d) timeSpentInSchedule (%.2f)\n" , relativeTime , numberElapsedPeriods  , *timeSpentInSchedule  );
					       
  // weights need to be regenerated in case of change of
  // active weight
  if( numberElapsedPeriods != previousNumberElapsedPeriods
      && ( schedule->mode == LINEAR_RANDOM 
	   || schedule->mode == SINUS_RANDOM ) ) {
    //printf( "Generation numberElapsedPeriods (%d) previousNumberElapsedPeriods (%d) index (%d)\n" ,
    //numberElapsedPeriods  , previousNumberElapsedPeriods , 
    //(numberElapsedPeriods +1) % 2  );
    generateRandomWeights( tabWeights[ (numberElapsedPeriods+1) % 2 ] ,
			   nbComponents );
    previousNumberElapsedPeriods = numberElapsedPeriods;
  }
      
  if( schedule->indObjectIni < 0 
      || schedule->indObjectIni > schedule->indObjectEnd ) {
    sprintf( ErrorStr , "Error: schedule in interpolator [%s] exceeds weight table size (%d/%d/%d/%d)!" , id , schedule->indObjectIni , schedule->indObjectEnd , nbWeights , nbComponents ); ReportError( ErrorStr ); throw 147;
  }
  // random walks only have 2 sets of weights
  // schedule dimension should be updated accordingly
  if( schedule->indObjectEnd >= nbWeights ) {
    // printf( "indObjectEnd update for random walk (%d/%d/%d/%d)!\n" , schedule->indObjectIni , schedule->indObjectEnd , nbWeights , nbComponents ); 
    schedule->indObjectEnd = nbWeights - 1;
  }
      
  // the global coefficient is the decimal value of the
  // relative time multiplied by 
  // (schedule->indObjectEnd - schedule->indObjectIni)
  // and schedule->indObjectIni is added
  // in order to walk from component #schedule->indObjectIni to weight
  // (schedule->indObjectEnd) in a single period
  double coefNweights = ((double)relativeTime - (double)numberElapsedPeriods)
    * (double)(schedule->indObjectEnd - schedule->indObjectIni)
    + (double)schedule->indObjectIni;

  // in case of periodic movement, the interpolation, 
  // backtracks to the initial position
  if( schedule->mode == LINEAR_PERIODIC || schedule->mode == SINUS_PERIODIC  
      || schedule->mode == LINEAR_RANDOM || schedule->mode == SINUS_RANDOM ){
    // even period
    if( numberElapsedPeriods % 2 == 1 ) {
      coefNweights = (double)(nbWeights - 1) - coefNweights;
    }
  }
      
  // each component is active in turn
  indActiveWeight = (int)floor( coefNweights );
  if(  indActiveWeight < 0 ) {
     indActiveWeight = 0;
  }
  if(  indActiveWeight >= nbWeights - 1 ) {
     indActiveWeight = nbWeights - 2;
  }
  // printf( "indActiveWeight (%d) coefNweights (%.8f) relativeTime (%.8f) numberElapsedPeriods (%d) \n" , indActiveWeight , coefNweights , relativeTime , numberElapsedPeriods );
      
  // the single coefficient between the active component and the
  // next one is the decimal value of the global coefficient
  (*coef) = (coefNweights - (float) indActiveWeight);
  indPrecedingActiveWeight = indActiveWeight;

  // the coefficient is remapped to [0,1] through the
  // function associated with the current schedule
  schedule->percentageFunction( coef );
}

int dm_Interpolator::update( dm_Schedule *schedule , 
			     int isFirstSchedule , 
			     int isLastSchedule ,
			     float *targetTabWeight , 
			     bool with_forcedUpdate ,
			     dm_SceneNode *curNode , 
			     dm_SceneNode **nodes ,
			     dm_Variable *curVar , 
			     dm_Variable **vars , 
			     VariableType type ,
			     InterpolatorElementCategory elementCategory ) {
  
  float endTime = schedule->endTime();
  dm_Command *synchronous = NULL;

//   printf( "interpolator CurrentClockTime %.4f beginTime  %.4f endTime %.4f \n" , CurrentClockTime , schedule->beginTime , endTime );

//   if( with_forcedUpdate ) {
//     printf( "with_forcedUpdate\n" );
//   }

  ///////////////////////////////////////////////////////
  // NON INTERPOLATION BEFORE BEGIN TIME

  if( !isFirstSchedule && CurrentClockTime < schedule->beginTime ) {
    // should not happen if schedules are continuous
    // printf( "Non continuous schedules in interpolator (%s)\n" , id );
    // just leaves things as they are
    schedule->completionExecuted = false;
    return true;
  }

  //printf( "No update at time %.2f\n" , CurrentClockTime );
  if( isFirstSchedule && CurrentClockTime < schedule->beginTime ) {
    schedule->completionExecuted = false;
    
    //     printf( "Before begin time %.2f %s\n" , CurrentClockTime , id );
    //     if( with_forcedUpdate ) {
    //       printf( "with_forcedUpdate\n" );
    //     }

    // HIDE before mode : invisible before the schedule begins
    // begins to be visible when the schedule begins
    if( schedule->before == HIDE ) {
      // printf( "No update at time %.2f (HIDE)\n" , CurrentClockTime );
      return true;
    }

    // VISIBLE before mode : visible before the schedule begins
    // allows display list to be built
    //  list can be rebuilt
    if( !initialInterpolationCompleted || with_forcedUpdate ) {
      //       if( with_forcedUpdate ) {
      // 	printf( "Initial interpolation %s index %d nb of components %d: " , 
      // 		id , schedule->indObjectIni , nbComponents );
      // 	for( int ind = 0 ; ind < nbComponents ; ind++ ) {
      // 	  printf( "%.2f " , tabWeights[schedule->indObjectIni][ind] );
      // 	}
      // 	printf( "\n" );
      //       }
      if( elementCategory == dm_VariableInterpolatorElement ) {
	if( type == dm_ScalarType )
	  ((dm_Scalar *)curVar)->interpolate( (dm_Scalar **)vars , 
			       tabWeights[schedule->indObjectIni] , 
			       tabWeights[schedule->indObjectIni] , 
			       nbComponents ,
			       0.0 );
	else if( type == dm_TableType )
	  ((dm_Table *)curVar)->interpolate( (dm_Table **)vars , 
			       tabWeights[schedule->indObjectIni] , 
			       tabWeights[schedule->indObjectIni] , 
			       nbComponents ,
			       0.0 );
	else if( type == dm_MatrixType )
	  ((dm_Matrix *)curVar)->interpolate( (dm_Matrix **)vars , 
			       tabWeights[schedule->indObjectIni] , 
			       tabWeights[schedule->indObjectIni] , 
			       nbComponents ,
			       0.0 );
      }
      initialInterpolationCompleted = true;
    }
    return true;
  }

  ///////////////////////////////////////////////////////
  // INTERPOLATION BETWEEN BEGIN AND END TIME

  // printf( "update at time %.2f %.2f %.2f\n" , CurrentClockTime , schedule->beginTime , endTime );
  if( CurrentClockTime >= schedule->beginTime && CurrentClockTime < endTime ) {

    // interpolation coeficient 
    // default value: on the first node
    schedule->coef = 1.0;

    /////////////////////////////////////////////////////
    // CLASSICAL INTERPOLATOR

    if( interpolatorType == ABSOLUTE_INTERPOLATION ) {
      // searches the rank of the current active weight
      // values
      double timeSpentInSchedule;
      // computes the current value of the coeficient
      // if it has not been imposed by an external action (in a script)
      if( !schedule->coefComputed ) {
	computesActiveScheduleRankAndCoeficient( 
             schedule , &(schedule->coef) , &timeSpentInSchedule);
      }
      else {
	schedule->coefComputed = false;
      }
      // printf( "Current time %.2f coef %.8f active weight %d nb weights %d\n" , CurrentClockTime , schedule->coef ,  indActiveWeight , nbWeights );
      
      // In the case of a command that redefines the current target, 
      // the weight table of the interpolator is modified as follows:
      // - the weight table of the current active weight becomes 
      //   the current point in the interpolation space 
      // - the weight table of the next active weight becomes 
      //   the target weight table 
      // - all the begin times of the schedules are set back  
      //   of the time elapsed since the beginning of the current active weight
      // Scheduling is expected to restart from the beginning of the 
      // current active weight with a value equal to the current point
      // in the interpolation space
      if( targetTabWeight ) {
	for( int ind = 0 ; ind < nbComponents ; ind++ ) {
	  // initial point in the intermolation space is a 
	  // copy of the current point
	  tabWeights[ schedule->indObjectIni ][ind]
	    = (float)(1.0 - schedule->coef) * tabWeights[ indActiveWeight ][ind] 
	    + (float)schedule->coef * tabWeights[ indActiveWeight + 1 ][ind];
	  // second point in the interpolation space is a 
	  // copy of new target
	  tabWeights[ schedule->indObjectIni + 1 ][ind]
	    = targetTabWeight[ind];
	}
	// compute the time elapsed since indActiveWeight
	for( int ind = 0 ; ind < nbSchedules ; ind++ ) {
	  schedule->beginTime += (float)timeSpentInSchedule;
	}  
	// number of periods elapsed since the beginning
	int numberElapsedPeriods 
	  = (int)floor( (CurrentClockTime - schedule->beginTime) / 
			schedule->period );
	// in case of periodic movement, the interpolation, 
	// backtracks to the initial position
	if( schedule->mode == LINEAR_PERIODIC 
	    || schedule->mode == SINUS_PERIODIC  
	    || schedule->mode == LINEAR_RANDOM 
	    || schedule->mode == SINUS_RANDOM ){
	  // even period
	  // must restart at an odd period
	  if( numberElapsedPeriods % 2 == 1 ) {
	    schedule->beginTime -= schedule->period;
	  }
	}
	// now that schedule is modified can return
	// new update call will restart from the beginning of the 
	// current schedule at the location where the interpolator
	// is currently

	// launches the update command if there is one
	if( schedule->updateCommand 
	    && schedule->isUpdateScriptEnabled 
	    && fabsf((float)schedule->coef - (float)schedule->precedingCoef)
	    >= DM_EnvironmentNode->InterpolationRefreshingVariation ) {
	  schedule->updateCommand->execute( staticReferenceResolution , 
					    false , (char *)"" );
	  synchronous = schedule->updateCommand->synchronousCommand;
	  while( synchronous ) {
	    synchronous->execute( staticReferenceResolution , 
				  false , (char *)"" );
	    synchronous = synchronous->synchronousCommand;
	  }
	  schedule->precedingCoef = schedule->coef;
	}
      }
      // the computed values for the rank in the
      // wieght table and the coefficient are used
      // to interpolate the coponents of the current interpolator
      else {
// 	printf( "Weights: " ); 
// 	for( int ind = 0 ; ind < nbComponents ; ind++ ) {
// 	  printf( "%.2f " , (1.0 - schedule->coef) * tabWeights[ indActiveWeight ][ind] 
// 	    + schedule->coef * tabWeights[ indActiveWeight + 1 ][ind]);
// 	}
// 	printf( "\n" ); 
	if( elementCategory == dm_VariableInterpolatorElement ) {
	  if( curVar ) {
	    if( type == dm_ScalarType )
	      ((dm_Scalar *)curVar)->interpolate( (dm_Scalar **)vars , 
				 tabWeights[ indActiveWeight ] , 
				 tabWeights[ indActiveWeight + 1 ] , 
				 nbComponents ,
				 1.0F - (float)schedule->coef );
	    else if( type == dm_TableType ) {
// 	      printf( "Weights (%.2f,%s): " , (1.0 - schedule->coef) , curVar->id ); 
// 	      for( int ind = 0 ; ind < nbComponents ; ind++ ) {
// 		printf( "%.2f " , (1.0 - schedule->coef) * tabWeights[ indActiveWeight ][ind] 
// 			+ schedule->coef * tabWeights[ indActiveWeight + 1 ][ind]);
// 	      }
// 	      printf( "\n" ); 
	      ((dm_Table *)curVar)->interpolate( (dm_Table **)vars , 
				 tabWeights[ indActiveWeight ] , 
				 tabWeights[ indActiveWeight + 1 ] , 
				 nbComponents ,
				 1.0F - (float)schedule->coef );
	    }
	    else if( type == dm_MatrixType )
	      ((dm_Matrix *)curVar)->interpolate( (dm_Matrix **)vars , 
				 tabWeights[ indActiveWeight ] , 
				 tabWeights[ indActiveWeight + 1 ] , 
				 nbComponents ,
				 1.0F - (float)schedule->coef );
	  }
	}
      }

      // launches the update command if there is one
      if( schedule->updateCommand 
	  && schedule->isUpdateScriptEnabled 
	  && fabsf((float)schedule->coef - (float)schedule->precedingCoef)
	  >= DM_EnvironmentNode->InterpolationRefreshingVariation ) {
	schedule->updateCommand->execute( staticReferenceResolution , 
					  false , (char *)"" );
	synchronous = schedule->updateCommand->synchronousCommand;
	while( synchronous ) {
	  synchronous->execute( staticReferenceResolution , 
				false , (char *)"" );
	  synchronous = synchronous->synchronousCommand;
	}
	schedule->precedingCoef = schedule->coef;
      }
      
      // the completion commmand will be executed when the interpolator
      // is finished
      schedule->completionExecuted = false;
    
      return true;
    }

    /////////////////////////////////////////////////////
    // STEPWISE INTERPOLATOR

    else if( interpolatorType == STEPWISE_INTERPOLATION ) {
      if( nbComponents == 2 ) {
	if( elementCategory == dm_VariableInterpolatorElement ) {
	  if( curVar ) {
	    if( type == dm_ScalarType )
	      ((dm_Scalar *)curVar)->interpolate( (dm_Scalar *)vars[0] , 
						(dm_Scalar *)vars[1] );
	    else if( type == dm_TableType )
	      ((dm_Table *)curVar)->interpolate( (dm_Table *)vars[0] , 
					       (dm_Table *)vars[1] );
	    else if( type == dm_MatrixType )
	      ((dm_Matrix *)curVar)->interpolate( (dm_Matrix *)vars[0] , 
						(dm_Matrix *)vars[1] );
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: non binary stepwise interpolator\n" ); ReportError( ErrorStr ); throw 407;
      }
    }

    // launches the update command if there is one
    if( schedule->updateCommand 
	&& schedule->isUpdateScriptEnabled 
	&& fabsf((float)schedule->coef - (float)schedule->precedingCoef)
	            >= DM_EnvironmentNode->InterpolationRefreshingVariation ) {
      schedule->updateCommand->execute( staticReferenceResolution , 
					false , (char *)"" );
      synchronous = schedule->updateCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
      schedule->precedingCoef = schedule->coef;
    }

    // the completion commmand will be executed when the interpolator
    // is finished
    schedule->completionExecuted = false;
    
    // printf( "end of update at time %.2f\n" , CurrentClockTime );
    return true;
  }

  ///////////////////////////////////////////////////////
  // END OF INTERPOLATION

  if( CurrentClockTime >= endTime ) {
    if( with_forcedUpdate ) {
      if( elementCategory == dm_VariableInterpolatorElement ) {
	if( curVar ) {
	  if( type == dm_ScalarType )
	    ((dm_Scalar *)curVar)->interpolate( (dm_Scalar **)vars , 
				 tabWeights[indActiveWeight] , 
				 tabWeights[indActiveWeight + 1] , 
				 nbComponents ,
				 0.0 );
	  else if( type == dm_TableType )
	    ((dm_Table *)curVar)->interpolate( (dm_Table **)vars , 
				 tabWeights[indActiveWeight] , 
				 tabWeights[indActiveWeight + 1] , 
				 nbComponents ,
				 0.0 );
	  else if( type == dm_MatrixType )
	    ((dm_Matrix *)curVar)->interpolate( (dm_Matrix **)vars , 
				 tabWeights[indActiveWeight] , 
				 tabWeights[indActiveWeight + 1] , 
				 nbComponents ,
				 0.0 );
	}
      }
    }

    // COMPLETION ALREADY MADE
    if( schedule->completionExecuted  ) {
      // NOT THE LAST SCHEDULE, NEXT ONE WILL BE ACTIVATED
      // IN THE LOOP BECAUSE OF FALSE RETURN
      if( !isLastSchedule ) {
	return false;
      }
      else {
	return false;
      }
    }
  }

  ///////////////////////////////////////////////////////
  // END OF INTERPOLATION - REMOVE MODE

  // only last interpolator should be in remove mode
  // REMOVE mode : returns to initial position
  if( CurrentClockTime >= endTime && schedule->fill == REMOVE ) {
    // printf( "Current time %.2f indObjectEnd %d nb weights %d\n" ,
	    // CurrentClockTime , schedule->indObjectEnd , nbWeights );
    // classical interpolator
    if( interpolatorType == ABSOLUTE_INTERPOLATION ) {
      if( elementCategory == dm_VariableInterpolatorElement ) {
	if( curVar ) {
	  if( type == dm_ScalarType )
	    ((dm_Scalar *)curVar)->interpolate( (dm_Scalar **)vars , 
					      tabWeights[0] , tabWeights[0] , 
					      nbComponents ,
					      1.0 );
	  else if( type == dm_TableType )
	    ((dm_Table *)curVar)->interpolate( (dm_Table **)vars , 
					     tabWeights[0] , tabWeights[0] , 
					     nbComponents ,
					     1.0 );
	  else if( type == dm_MatrixType )
	    ((dm_Matrix *)curVar)->interpolate( (dm_Matrix **)vars , 
					      tabWeights[0] , tabWeights[0] , 
					      nbComponents ,
					      1.0 );
	}
      }
    }

    // stepwise interpolator
    else if( interpolatorType == STEPWISE_INTERPOLATION ) {
      if( nbComponents == 2 ) {
	if( elementCategory == dm_VariableInterpolatorElement ) {
	  if( curVar ) {
	    if( type == dm_ScalarType )
	      ((dm_Scalar *)curVar)->interpolate( (dm_Scalar *)vars[0] , 
						(dm_Scalar *)vars[1] );
	    else if( type == dm_TableType )
	      ((dm_Table *)curVar)->interpolate( (dm_Table *)vars[0] , 
					       (dm_Table *)vars[1] );
	    else if( type == dm_MatrixType )
	      ((dm_Matrix *)curVar)->interpolate( (dm_Matrix *)vars[0] , 
						(dm_Matrix *)vars[1] );
	  }
	}
      }
      else {
	sprintf( ErrorStr , "Error: non binary stepwise interpolator!" ); ReportError( ErrorStr ); throw 407;
      }
    }

    // launches the update command if there is one
    if( schedule->updateCommand 
	&& schedule->isUpdateScriptEnabled ) {
      schedule->updateCommand->execute( staticReferenceResolution , 
					false , (char *)"" );
      synchronous = schedule->updateCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
      schedule->precedingCoef = schedule->coef;
    }

    // launches the completion command if there is one
    if( schedule->completionCommand 
	&& schedule->isCompletionScriptEnabled ) {
      // printf( "completion command\n" );
      schedule->completionCommand->execute( staticReferenceResolution , 
					    false , (char *)"" );
      synchronous = schedule->completionCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
    }

    schedule->completionExecuted = true;

    return false;
  }

  ///////////////////////////////////////////////////////
  // END OF INTERPOLATION - FREEZE MODE

  // FREEZE mode : remains in the last position
  if( CurrentClockTime >= endTime && schedule->fill == FREEZE ) {
    // printf( "Current time %.2f FREEZE indObjectEnd %d nb weights %d indActiveWeight %d indPrecedingActiveWeight %d\n" , CurrentClockTime , schedule->indObjectEnd , nbWeights , indActiveWeight , indPrecedingActiveWeight );

    // classical interpolator
    if( interpolatorType == ABSOLUTE_INTERPOLATION ) {

      // we restore the active weight to its value at the
      // preceding step in case it has moved to the next interpolation step
      if( indActiveWeight != indPrecedingActiveWeight ) {
	indActiveWeight = indPrecedingActiveWeight;
      }

      if( elementCategory == dm_VariableInterpolatorElement ) {
	// printf( "freeze variable final update [%s] at time %.2f indActiveWeight %d\n" , id , CurrentClockTime , indActiveWeight );
	  if( type == dm_ScalarType )
	    ((dm_Scalar *)curVar)->interpolate( (dm_Scalar **)vars , 
			     tabWeights[indActiveWeight] , 
			     tabWeights[indActiveWeight + 1] , 
			     nbComponents ,
			     0.0 );
	  else if( type == dm_TableType )
	    ((dm_Table *)curVar)->interpolate( (dm_Table **)vars , 
			     tabWeights[indActiveWeight] , 
			     tabWeights[indActiveWeight + 1] , 
			     nbComponents ,
			     0.0 );
	  else if( type == dm_MatrixType )
	    ((dm_Matrix *)curVar)->interpolate( (dm_Matrix **)vars , 
			     tabWeights[indActiveWeight] , 
			     tabWeights[indActiveWeight + 1] , 
			     nbComponents ,
			     0.0 );
      }
    }
    // stepwise interpolator
    else if( interpolatorType == STEPWISE_INTERPOLATION ) {
      if( nbComponents == 2 ) {
	if( elementCategory == dm_VariableInterpolatorElement ) {
	    if( type == dm_ScalarType )
	      ((dm_Scalar *)curVar)->interpolate( (dm_Scalar *)vars[0] , 
						(dm_Scalar *)vars[1] );
	    else if( type == dm_TableType )
	      ((dm_Table *)curVar)->interpolate( (dm_Table *)vars[0] , 
					       (dm_Table *)vars[1] );
	    else if( type == dm_MatrixType )
	      ((dm_Matrix *)curVar)->interpolate( (dm_Matrix *)vars[0] , 
						(dm_Matrix *)vars[1] );
	}
      }
      else {
	sprintf( ErrorStr , "Error: non binary stepwise interpolator\n" ); ReportError( ErrorStr ); throw 407;
      }
    }

    // launches the update command if there is one
    if( schedule->updateCommand 
	&& schedule->isUpdateScriptEnabled ) {
      schedule->updateCommand->execute( staticReferenceResolution , 
					false , (char *)"" );
      synchronous = schedule->updateCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
      schedule->precedingCoef = schedule->coef;
    }

    // launches the completion command if there is one
    if( schedule->completionCommand 
	&& schedule->isCompletionScriptEnabled ) {
      schedule->completionCommand->execute( staticReferenceResolution , 
					    false , (char *)"" );
      synchronous = schedule->completionCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
    }

    schedule->completionExecuted = true;

    return false;
  }

  ///////////////////////////////////////////////////////
  // END OF INTERPOLATION - HIDE MODE

  // HIDE mode : hidden when the schedule is over
  if( CurrentClockTime >= endTime && schedule->fill == HIDE ) {

    // launches the update command if there is one
    if( schedule->updateCommand 
	&& schedule->isUpdateScriptEnabled ) {
      schedule->updateCommand->execute( staticReferenceResolution ,
					false , (char *)"" );
      synchronous = schedule->updateCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
      schedule->precedingCoef = schedule->coef;
    }

    // launches the completion command if there is one
    if( schedule->completionCommand 
	&& schedule->isCompletionScriptEnabled ) {
      schedule->completionCommand->execute( staticReferenceResolution , 
					    false , (char *)"" );
      synchronous = schedule->completionCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->execute( staticReferenceResolution , 
			      false , (char *)"" );
	synchronous = synchronous->synchronousCommand;
      }
    }

    schedule->completionExecuted = true;

    return false;
  }

  sprintf( ErrorStr , "Error: unexpected schedule termination node ID:%s isLastSchedule %d CurrentClockTime %.2f beginTime %.2f  endTime %.2f schedule->fill %d\n" , curNode->id , isLastSchedule , CurrentClockTime , schedule->beginTime , endTime , schedule->fill); ReportError( ErrorStr ); throw 407;
}

void dm_Interpolator::update( bool forcedInterpolatorUpdate ,
			      dm_SceneNode *curNode , 
			      dm_SceneNode **nodes ,
			      dm_Variable *curVar , 
			      dm_Variable **vars , 
			      VariableType type ,
			      InterpolatorElementCategory elementCategory ) {
  //printf( "Current time %.2f\n" , CurrentClockTime );
  if( nbSchedules <= 0 || nbWeights <= 0
      || lastUpdate == CurrentClockTime ) {
    return;
  }

  lastUpdate = (float)CurrentClockTime;

  for( int ind = 0 ; ind < nbSchedules ; ind++ ) {
    if( schedules[ind]->period > 0 ) {
      if( update( schedules[ind] , (ind == 0) , 
		  (ind == (nbSchedules - 1) ) , 
		  NULL , forcedInterpolatorUpdate ,
		  curNode , nodes , 
		  curVar , vars , type , 
		  elementCategory ) ) {
	// printf( "Node %s Current time %.2f Current schedules %d invalid %d\n" , id , CurrentClockTime , ind , curNode->isInvalid() );
	break;
      }
    }
  }  
}

void dm_Interpolator::updateWeights( int weightsRank ,
				 float *targetTabWeight ,
				 FloatBinOp oper ,
				 IntBinOp operI ) {
//   printf( "tab weight before %.6f modif %.6f " ,  
// 	  tabWeights[ weightsRank ][0] , targetTabWeight[0] );
  if( targetTabWeight ) {
    for( int ind = 0 ; ind < nbComponents ; ind++ ) {
      // copy of new target
      oper( tabWeights[ weightsRank ][ind] , targetTabWeight[ind] );
    }
  }
//   printf( "after %.6f modif %.6f\n" ,
// 	  tabWeights[ weightsRank ][0] , targetTabWeight[0] );
}

void dm_Interpolator::setForceUpdate( bool with_forcedUpdateValue ) {
  forceUpdate = with_forcedUpdateValue;
}
bool dm_Interpolator::getForceUpdate( void ) {
  return forceUpdate;
}

void dm_Interpolator::findAllScriptReferences( dm_SceneNode *rootNode, char * targetID ) {
  for( int ind = 0 ; ind < nbSchedules ; ind++ ) {
    if( schedules[ind]->completionCommand ) {
      dm_Command *curCommand = schedules[ind]->completionCommand;
      dm_Command *synchronous = NULL;
      curCommand->findScriptReferences( rootNode );
      synchronous = curCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->findScriptReferences( rootNode );
	synchronous = synchronous->synchronousCommand;
      }
    }
    if( schedules[ind]->updateCommand ) {
      dm_Command *curCommand = schedules[ind]->updateCommand;
      dm_Command *synchronous = NULL;
      curCommand->findScriptReferences( rootNode );
      synchronous = curCommand->synchronousCommand;
      while( synchronous ) {
	synchronous->findScriptReferences( rootNode );
	synchronous = synchronous->synchronousCommand;
      }
    }
  }
}

void dm_Interpolator::operator=(dm_Interpolator& i) {
  this->dm_NonGeometricalObject::operator=(i);

  lastUpdate = i.lastUpdate;
  nbSchedules = i.nbSchedules;
  initialInterpolationCompleted = i.initialInterpolationCompleted;
  forceUpdate = i.forceUpdate;
  for( int ind  = 0 ; ind < nbComponents ; ind++ ) {
	  if (schedules[ind])
	  {
		  delete schedules[ind];
		  schedules[ind] = NULL;
	  }
    // N'alloue que ceux qui sont necessaires
    if( i.schedules[ind] ) {
		schedules[ind] = new dm_Schedule();
		*(schedules[ind]) = *(i.schedules[ind]);
	}
  }
  
/*
  if (tabWeights)
  {
	  for(int indWeight = 0 ; indWeight < nbWeights ; indWeight++ ) 
	  {
		  if (tabWeights[indWeight])
		  {
			  delete [] tabWeights[indWeight];
			  tabWeights[indWeight] = NULL;
		  }
	  }
		  delete [] tabWeights;
		  tabWeights = NULL;	  
  }
*/
  nbComponents = i.nbComponents;
  nbWeights = i.nbWeights;
 /* if (nbWeights > 0)
  {
	  	tabWeights = new float*[nbWeights];

		for(int indWeight = 0 ; indWeight < nbWeights ; indWeight++ ) 
		{
			tabWeights[indWeight] = NULL;

			if (i.tabWeights[indWeight])
			{
				tabWeights[indWeight]  = new float[nbComponents];
				for (int ind = 0; ind < indWeight; ind++)
				{
					tabWeights[indWeight][ind] = i.tabWeights[indWeight][ind];
				}
			}
		}
  }
  */
  isReferencedWeights = true;
  tabWeights = i.tabWeights;
  interpolatorType = i.interpolatorType;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// VARIABLE INTERPOLATOR
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

dm_VariableInterpolator::dm_VariableInterpolator( int size ): dm_Interpolator( size ) {

  variables = new dm_Variable *[ size ];
  for( int ind  = 0 ; ind < size ; ind++ ) {
    // only allocates necessary variables
    variables[ind] = NULL;
  }
  curVariable = NULL;
  variableType = EmptyVariableType;
}

dm_VariableInterpolator::~dm_VariableInterpolator( void ) {
  if (variables) {
/*	  for( int ind  = 0 ; ind < nbComponents ; ind++ ) 
	  {
		  if (variables[ind])
		  {
			  delete variables[ind];
			  variables[ind] = NULL;
		  }
	  }
*/	  
    delete [] variables;
    variables = NULL;
  }
  
  /*
  if (curVariable) 
  {
	  delete curVariable;
	  curVariable = NULL;
  }*/
}

bool dm_VariableInterpolator::getParameters( char *attribute , 
					 DataType * dataType ,
					 double * valDouble ,
					 int * valInt ,
					 bool * valBool ,
					 char ** valString ,
					 int * tableOrMatrixSize ,
					 dm_ValScalar ** valScalars ) {
  *dataType = EmptyDataType;
  if( this->dm_Interpolator::getParameters( attribute , dataType , valDouble , 
					    valInt , valBool , 
					    valString , tableOrMatrixSize , valScalars ) ) {
    return true;
  }

  // first tries to get the base parameters from the superclass method
  // then accesses the subvariable parameters
  if( curVariable ) {
    //printf("interp var id/attribute %s/%s\n" , id , attribute );
    if( curVariable->getParameters( attribute , 
				    dataType ,
				    valDouble ,
				    valInt ,
				    valBool ,
				    valString ,
				    tableOrMatrixSize ,
				    valScalars ) ) {
      return true;
    }
  }
  return false;
}

void dm_VariableInterpolator::parse( int *fileDepth , 
				     int *p_c , 
				     char *typeTag ,
				     dm_SceneNode *sourceNode ) {
  dm_XMLTag *XMLtag = new dm_XMLTag();
  int indComponent = 0;

  if( variableType == EmptyVariableType ) {
    sprintf( ErrorStr , "Error: unknown!" ); ReportError( ErrorStr ); throw 34;
  }

  while( true ) {
    // new tag XML : interpolator component or closing tag
    XMLtag->deallocateXMLTag();
    XMLtag->ParseTagXML( fileDepth , p_c , true , NULL , NULL );

    // interpolator closing tag
    if( strcmp( XMLtag->tag , "interpolator" ) == 0 
	&& (XMLtag->tagType == ClosingTag) ) {
      if( nbComponents <= 0 ) {
	sprintf( ErrorStr , "Error: empty interpolator ID [%s]!" , id ); ReportError( ErrorStr ); throw 34;
      }

      // checks whether the components are allocated
      for( int ind = 0 ; ind < nbComponents ; ind++ ) {
	if( !variables[ind] ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: component #[%d] of interpolator ID [%s] not allocated!" , ind + 1 , id ); ReportError( ErrorStr ); throw 34;
	}
      }

//       // only last schedule can have a completion script
//       for( int ind = 0 ; ind < nbSchedules - 1 ; ind++ ) {
// 	if( schedules[ind]->completionCommand ) {
// 	  sprintf( ErrorStr , "Error: non terminal schedule #[%d] has a completion script in interpolator ID [%s]!" , ind + 1 , id ); ReportError( ErrorStr ); throw 34;
// 	}
//       }
      
      // allocates an empty current variable that will
      // carry the current interpolation during the dynamic process
      if( nbComponents > 0 ) {
	if( !curVariable ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: interpolated variable not allocated in interpolator ID [%s]!" , id ); ReportError( ErrorStr ); throw 34;
	}
	curVariable->interpolatorVariable = true;
      }

      // no weights are provided
      if( nbWeights <= 0 || !(tabWeights) ) {
	// if no weights are provided, identity matrix is chosen
	// as weight matrix: each component in turn 
	// printf ("Automatic allocation\n");
	if( schedules[0]->mode != LINEAR_RANDOM
	    && schedules[0]->mode != SINUS_RANDOM ) {
	  nbWeights = nbComponents;
	}
	// in case of random walk only 2 sets of weights are allocated
	// they will be randomly instantiated during the walk
	else {
	  nbWeights = 2;
	}
	tabWeights = new float*[nbWeights];
	for(int indWeight = 0 ; indWeight < nbWeights ; indWeight++ ) {
	  tabWeights[indWeight] 
	    = new float[nbComponents];
	  // identity matrix
	  if( schedules[0]->mode != LINEAR_RANDOM
	      && schedules[0]->mode != SINUS_RANDOM ) {
	    for(int indComponent = 0 ; indComponent < nbComponents ; indComponent++ ) {
	      tabWeights[indWeight][indComponent] = 0.0;
	    }
	    if( indWeight < nbComponents ) {
	      tabWeights[indWeight][indWeight] = 1.0;
	    }
	  }
	  // random weights
	  else {
	    generateRandomWeights( tabWeights[ indWeight ] , nbComponents );
	  }
	}
      }

      delete XMLtag;
	  XMLtag = NULL;

      return;
    }

    // variable interpolation 
    else  if( strcmp( XMLtag->tag , "scalar" ) == 0 && (variableType == dm_ScalarType) ) {

      if( XMLtag->tagType != EmptyTag ) {
	sprintf( ErrorStr , "Error: empty tag expected for scalar element in node [%s]!" , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
      char * ID = XMLtag->getAttributeStringValue( "id" );
      if( ID && (sourceNode->getScalarFromName( ID ) 
		 || sourceNode->getTableFromName( ID ) ) ) {
	sprintf( ErrorStr , "Error: duplicated scalar or table element (%s) in node [%s]!" , ID , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }

      dm_Scalar *var = new dm_Scalar();
      var->setParameters( XMLtag , false , assign , assignI );
      if( var->GetType() != ((dm_Scalar *)curVariable)->GetType() ) {
	sprintf( ErrorStr , "Error: all interpolated scalar must have the same type in node [%s]!" , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }

      // associating the current variable with the corresponding interpolator
      // component
      if( indComponent < nbComponents ) {

	if( variables[indComponent] ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: component #[%d] of interpolator ID [%s] already allocated!" , indComponent + 1 , id ); ReportError( ErrorStr ); throw 37;
	}
	variables[indComponent] = var;
	// pointer towards the mother interpolator variable
	// stores a pointer to the mother variable in order to
	// recompute the curVariable if one of the variables[i] is modified
	variables[indComponent]->motherVariable = curVariable;
	indComponent++;
      }
      else {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: out of bounds component [%d] (max %d) in interpolator ID [%s]!" , indComponent + 1 , nbComponents , id ); ReportError( ErrorStr ); throw 2;
      }
    }

    // weights
    else if( strcmp( XMLtag->tag , "tabweights" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // parsing a table of weights
      nbWeights = 
	XMLtag->getAttributeIntValue( "size" , false );
      
//       if( nbWeights > 1 ) {
	// parsing the weight values
	tabWeights = new float*[nbWeights];
	//printf( "|%d|\n" ,  tabWeights );
	ParseTabWeights( fileDepth , p_c , tabWeights , nbWeights , 
			 &nbComponents , false , NULL , NULL );
//       }
//       else {
// 	sprintf( ErrorStr , "Error: insufficient weight table [%d] (min %d) in interpolator ID [%s]!" , nbWeights , 2 , id ); ReportError( ErrorStr ); throw 2;
//       }
    }

    // variable interpolation 
    else  if( strcmp( XMLtag->tag , "table" ) == 0 && (variableType == dm_TableType) ) {

      if( XMLtag->tagType != EmptyTag ) {
	sprintf( ErrorStr , "Error: empty tag expected for table element in node [%s]!" , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }
      char * ID = XMLtag->getAttributeStringValue( "id" );
      if( ID && (sourceNode->getTableFromName( ID ) 
		 || sourceNode->getTableFromName( ID ) ) ) {
	sprintf( ErrorStr , "Error: duplicated table or table element (%s) in node [%s]!" , ID , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }

      int size = XMLtag->getAttributeIntValue( "size" , false );
      if( size != ((dm_Table *)curVariable)->GetSize() ) {
	sprintf( ErrorStr , "Error: all interpolated table must have the same size in node [%s]!" , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }

      dm_Table *var = new dm_Table(size);
      var->setParameters( XMLtag , false , assign , assignI );
      if( var->GetType() != ((dm_Table *)curVariable)->GetType() ) {
	sprintf( ErrorStr , "Error: all interpolated table must have the same type in node [%s]!" , sourceNode->id ); ReportError( ErrorStr ) ; throw 298;
      }

      // associating the current variable with the corresponding interpolator
      // component
      if( indComponent < nbComponents ) {

	if( variables[indComponent] ) {
	  delete XMLtag;
	  XMLtag = NULL;

	  sprintf( ErrorStr , "Error: component #[%d] of interpolator ID [%s] already allocated!" , indComponent + 1 , id ); ReportError( ErrorStr ); throw 37;
	}
	variables[indComponent] = var;
	// pointer towards the mother interpolator variable
	// stores a pointer to the mother variable in order to
	// recompute the curVariable if one of the variables[i] is modified
	variables[indComponent]->motherVariable = curVariable;
	indComponent++;
      }
      else {
	delete XMLtag;
	XMLtag = NULL;

	sprintf( ErrorStr , "Error: out of bounds component [%d] (max %d) in interpolator ID [%s]!" , indComponent + 1 , nbComponents , id ); ReportError( ErrorStr ); throw 2;
      }
    }

    // weights
    else if( strcmp( XMLtag->tag , "tabweights" ) == 0 && (XMLtag->tagType == OpeningTag) ) {
      // parsing a table of weights
      nbWeights = 
	XMLtag->getAttributeIntValue( "size" , false );
      
//       if( nbWeights > 1 ) {
	// parsing the weight values
	tabWeights = new float*[nbWeights];
	//printf( "|%d|\n" ,  tabWeights );
	ParseTabWeights( fileDepth , p_c , tabWeights , nbWeights , 
			 &nbComponents , false , NULL , NULL );
//       }
//       else {
// 	sprintf( ErrorStr , "Error: insufficient weight table [%d] (min %d) in interpolator ID [%s]!" , nbWeights , 2 , id ); ReportError( ErrorStr ); throw 2;
//       }
    }

    // interpolation schedule
    else if( strcmp( XMLtag->tag , "schedule" ) == 0 
	     && ( (XMLtag->tagType == EmptyTag) || (XMLtag->tagType == OpeningTag) ) ) {

      this->dm_Interpolator::parseSchedule( fileDepth , p_c , XMLtag , sourceNode );
    }
    else {
      delete XMLtag;
	  XMLtag = NULL;

      sprintf( ErrorStr , "Error: unknown interpolator component tag [%s] in interpolator ID [%s]!" , XMLtag->tag , id ); ReportError( ErrorStr ); throw 50;
    }
  }
  delete XMLtag;
  XMLtag = NULL;
}

void dm_VariableInterpolator::print( FILE *file , int isLong , int depth ) {
  curVariable->print( file , depth );

  // first prints the attributes from the superclass method
  this->dm_Interpolator::print( file , isLong , depth + 1 , 
				(char *)VariableTypeString[variableType] );

  if( isLong ) {
    for( int ind = 0 ; ind < nbComponents ; ind++ ) {
      variables[ind]->print( file , depth + 2 );
    }
  }

  indent( file , depth );
  fprintf( file , "</interpolator>\n" );
}

int dm_VariableInterpolator::update( dm_Schedule *schedule , 
				     int isFirstSchedule , 
				     int isLastSchedule ,
				     float *targetTabWeight , 
				     bool with_forcedUpdate , 
				     VariableType type ) {
  return true;
}

void dm_VariableInterpolator::update( bool forcedInterpolatorUpdate , 
				      VariableType type ) {
  this->dm_Interpolator::update( forcedInterpolatorUpdate ,
				 NULL , NULL ,
				 curVariable , variables , type ,
				 dm_VariableInterpolatorElement );
}

void dm_VariableInterpolator::operator=(dm_VariableInterpolator& i) {
  this->dm_Interpolator::operator=(i);

  if( variables ) {
//	  for( int ind = 0 ; ind < nbComponents ; ind++ ) {
//		delete variables[ind];
//		variables[ind] = NULL;
//	  }

	  delete []variables;
	  variables = NULL;
	  nbComponents = 0;
  }

  /*
  if (i.variables)
  {
	  nbComponents = i.nbComponents;
	  variables = new dm_Variable *[ nbComponents ];

	  for( int ind = 0 ; ind < nbComponents ; ind++ ) 
	  {
		  variables[ind] = new dm_Variable;
		  *(variables[ind]) = *(i.variables[ind]);
	  }
  }
*/
  /*
  if (curVariable)
  {
	  delete curVariable;
	  curVariable = NULL;
  }

  if (i.curVariable)
  {
	  curVariable = new dm_Variable;
	  *curVariable = *(i.curVariable);
  }
*/
  if (i.variables)
  {
	  nbComponents = i.nbComponents;
	  variables = new dm_Variable *[ nbComponents ];

	  for( int ind = 0 ; ind < nbComponents ; ind++ ) 
	  {
		  variables[ind] = i.variables[ind];
	  }
  }

  curVariable = i.curVariable;
  variableType = i.variableType;
};
