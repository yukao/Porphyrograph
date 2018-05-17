/*! \file dm-interpolator.h
 *  \brief the header for class ( \a dm_Interpolator, \a dm_NodeInterpolator, \a dm_VariableInterpolator 
 * \a dm_Schedule )
 * and structures ( \a FillMode, \a InterpolatorElementCategory, \a InterpolatorType, \a ScheduleMode )
 * 
 * 
 *     Part of this source file is taken from Virtual Choreographer
 *     http://virchor.sourceforge.net/
 * 
 *     File dm-interpolator.h
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

#ifndef DM_INTERPOLATOR_H
#define DM_INTERPOLATOR_H

enum ScheduleMode{ LINEAR = 0, SINUS , LINEAR_PERIODIC , SINUS_PERIODIC , LINEAR_RANDOM , SINUS_RANDOM , POLYNOMIAL , EmptySchedule };
extern const char *ScheduleModeString[EmptySchedule + 1];

enum FillMode{ REMOVE = 0 , FREEZE , HIDE , SHOW , EmptyFillMode };
extern const char *FillModeString[EmptyFillMode + 1];

enum InterpolatorType{ ABSOLUTE_INTERPOLATION = 0 , STEPWISE_INTERPOLATION , EmptyInterpolatorType };
extern const char *InterpolatorTypeString[EmptyInterpolatorType + 1];

enum InterpolatorElementCategory{ dm_VariableInterpolatorElement = 0 , EmptyInterpolatorElementCategory };

class dm_SceneNode;
class dm_Command;

class dm_Schedule {
 public:
  float                   beginTime;
  float                   period;
  float                   repeatCount;
  float                   paramSinusPolynom;
  ScheduleMode            mode;
  FillMode                before;
  FillMode                fill;
  int                     indObjectIni;
  int                     indObjectEnd;
  
  /// the length of the associated subpath
  float                   totalLength;
  
  dm_Command             *completionCommand;
  bool                    isCompletionScriptEnabled;
  dm_Command             *updateCommand;
  bool                    isUpdateScriptEnabled;

  /// working variables
  /// test whether a completion command has already
  /// been executed when an interpolator finishes
  int                     completionExecuted;
  
  /// stores the current value of the coefficient
  double                  coef;

  /// stores the preceding value of the coefficient
  /// in order to control the interpolation pace
  double                  precedingCoef;

  /// stores whether the computation of the coefficient
  /// has already be made, for instance by a script action that stores 
  /// value of the coefficient
  bool                    coefComputed;

  dm_Schedule( void );
  ~dm_Schedule(void);

  /*!
   * \brief 
   * \param ID							to be added
   * \param size						to be added
   * \param XMLtag				to be added
   * \param atRunTime			to be added
   * \param oper						to be added
   * \param operI						to be added

   * numerical characteristics of the interpolation 
   * atRunTime is a boolean indicating whether
   * the copy of the values is performed a compiling time
   * (with mandatory values) or at run tine (with only
   *  one provided attribute value and other attributes optional
   */
  void setParameters( char *ID , int size ,
		      dm_XMLTag *XMLtag , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  /*!
   * \brief reading a dm_Schedule
   * \param attribute							to be added
   * \param dataType						to be added
   * \param valDouble						to be added
   * \param valInt									to be added
   * \param valBool							to be added
   * \param valString							to be added
   * \param tableOrMatrixSize		to be added
   * \param valScalars						to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );
  void parse( int *fileDepth , int *p_c , dm_SceneNode *sourceNode );
  void clean( void );
  float endTime( void );

  /*!
   * \brief parameter value weighted by a percentage function
   * \param coef			to be added
    */
  void percentageFunction( double *coef );
  void defaultValues( void );
  void print( FILE *file , int isLong , int depth );
  void operator=(dm_Schedule&);
};

typedef dm_Schedule * PSchedule;
typedef dm_SceneNode * PSceneNode;

class dm_Interpolator: public dm_NonGeometricalObject {
  int                     previousNumberElapsedPeriods;
  bool                    forceUpdate;
 public:
  float                   lastUpdate;

  /// indicates whether the initial interpolation has been computed
  bool                    initialInterpolationCompleted;

  /// the number of elements between which interpolation is computed
  int                     nbComponents;

  /// the type of interpolator: blending or step by step
  InterpolatorType        interpolatorType;
  
  /*! \name weight_components
   *  \brief the weights of each components in interpolation computation
   */
  // \{
  int                     nbWeights;
  float                 **tabWeights;
  bool					isReferencedWeights;			/// check wether tabWeights is a reference or created by itself.
  int                     indActiveWeight;
  int                     indPrecedingActiveWeight;
  // \}

  /*! \name schedule_components
   *  \brief at most one schedule between each consecutive components
   */
  // \{
  int                     nbSchedules;
  dm_Schedule           **schedules;
  // \}

  dm_Interpolator( void );
  dm_Interpolator( int size );
  virtual ~dm_Interpolator( void );

   /*!
    * \brief parsing an interpolator
	* \param XMLtag					to be added
	* \param atRunTime			to be added
	* \param oper							to be added
	* \param operI						to be added
	*/ 
  void setParameters( dm_XMLTag *XMLtag  , bool atRunTime ,
		      FloatBinOp oper ,
		      IntBinOp operI );

  /*!
   * \brief reading a dm_Interpolator
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

  /*! 
   * \brief parsing a schedule inside a dm_Interpolator
   * \param fileScene					to be added
   * \param p_c							to be added
   * \param XMLtag					to be added
   * \param sourceNode			to be added
   */
  void parseSchedule( int *fileDepth , int *p_c , dm_XMLTag *XMLtag ,
		      dm_SceneNode *sourceNode );

  /*! 
   * \brief printing a dm_Interpolator
   * \param file							to be added
   * \param isLong					to be added
   * \param depth					to be added
   * \param typeString			to be added
   */
  void print( FILE *file , int isLong , int depth , char *typeString );

  /*! 
   * \brief finds which schedule is active and the corresponding interpolation oeficient in[0,1]
   * \param schedule														to be added
   * \param coef																to be added
   * \param timeElapsedSinceLastWeight			to be added
   */
  void computesActiveScheduleRankAndCoeficient( 
		       dm_Schedule *schedule ,
		       double *coef , double * timeElapsedSinceLastWeight );

  /*! 
   * \brief updates interpolated components with the currently active schedule 
   * if there is on also exeutes update or compleion scripts
   * \param schedule											to be added
   * \param isFirstSchedule								to be added
   * \param isLastSchedule								to be added
   * \param targetTabWeight							to be added
   * \param with_forcedUpdate						to be added
   * \param interpolatedCurNode					to be added
   * \param interpolatedNodes						to be added
   * \param interpolatedCurElement				to be added
   * \param interpolatedElements					to be added
   * \param type													to be added
   * \param elementCategory							to be added
   *
   * returns true if the interpolator is active 
   * (between begin and end time)
   */

  int update( dm_Schedule *schedule , 
	      int isFirstSchedule , int isLastSchedule ,
	      float *targetTabWeight , 
	      bool with_forcedUpdate ,
	      dm_SceneNode *interpolatedCurNode , 
	      dm_SceneNode **interpolatedNodes ,
	      dm_Variable *interpolatedCurElement , 
	      dm_Variable **interpolatedElements , VariableType type ,
	      InterpolatorElementCategory elementCategory );

  /*!
   * \brief updates by chosing the first active schedule
   * \param forcedInterpolatorUpdate			to be added
   * \param interpolatedCurNode					to be added
   * \param interpolatedNodes						to be added
   * \param interpolatedCurElement				to be added
   * \param interpolatedElements					to be added
   * \param type													to be added
   * \param elementCategory							to be added
   */
  void update( bool forcedInterpolatorUpdate ,
	       dm_SceneNode *interpolatedCurNode , 
	       dm_SceneNode **interpolatedNodes ,
	       dm_Variable *interpolatedCurElement , 
	       dm_Variable **interpolatedElements , VariableType type ,
	       InterpolatorElementCategory elementCategory );

  /*! 
   * \brief modifes a line of the weight matrix
   * \param weightsRank					to be added
   * \param targetTabWeight			to be added
   * \param oper									to be added
   * \param operI									to be added
   */
  void updateWeights( int weightsRank ,
		      float *targetTabWeight ,
		      FloatBinOp oper ,
		      IntBinOp operI );
  void setForceUpdate( bool with_forcedUpdate );
  bool getForceUpdate( void );

  /*! 
   * \brief sets the references in an interpolator script
   * \param rootNode			to be added
   * \param targetID				to be added
   */
  void findAllScriptReferences( dm_SceneNode *rootNode, char *targetID = NULL );
  void operator=(dm_Interpolator&);
};

class dm_VariableInterpolator: public dm_Interpolator {
 public:
  dm_Variable               **variables;
  
  /*! \name Group interpolated_variable
   *  \brief current interpolated variable
   */
  // \{
  dm_Variable                *curVariable;
  VariableType                variableType;
  // \}

  dm_VariableInterpolator( int size );
  virtual ~dm_VariableInterpolator( void );

  /*! 
   * \brief reading a dm_VariableInterpolator
   * \param attribute							to be added
   * \param dataType						to be added
   * \param valDouble						to be added
   * \param valInt									to be added
   * \param valBool							to be added
   * \param valString							to be added
   * \param tableOrMatrixSize		to be added
   * \param valScalars						to be added
   */
  bool getParameters( char *attribute , 
		      DataType * dataType ,
		      double * valDouble ,
		      int * valInt ,
		      bool * valBool ,
		      char ** valString ,
		      int * tableOrMatrixSize ,
		      dm_ValScalar ** valScalars );

  /*! 
   * \brief parsing a dm_VariableInterpolator
   * \param fileScene					to be added
   * \param p_c							to be added
   * \param typeTag					to be added
   * \param sourceNode			to be added
   */
  void parse( int *fileDepth , int *p_c , char *typeTag ,
	      dm_SceneNode *sourceNode );

  /*!
   * \brief printing a dm_VariableInterpolator
   * param file					to be added
   * param isLong			to be added
   * param depth				to be added
   */
  void print( FILE *file , int isLong , int depth );
  int update( dm_Schedule *schedule , 
	      int isFirstSchedule , 
	      int isLastSchedule ,
	      float *targetTabWeight , 
	      bool with_forcedUpdate , 
	      VariableType type );
  void update( bool forcedInterpolatorUpdate , VariableType type );
  void operator=(dm_VariableInterpolator&);
};

#endif
