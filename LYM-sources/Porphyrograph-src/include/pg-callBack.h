/*! \file pg-callBack.h
 *
 *
 *     File pg-callBack.h
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

#ifndef PG_CALLBACK_H
#define PG_CALLBACK_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

// +++++++++++++++++++++ CLEAR +++++++++++++++++++++++++++
extern float pg_isClearCA;
extern float pg_isClearLayer;
extern float pg_isClearAllLayers;
extern float pg_isClearEcho;

// currently running script: NULLL if none
extern string pg_currently_runnings_script_1;

// +++++++++++++++++++++++ scripts for external control through python programming +++++
extern PROCESS_INFORMATION pg_pi_script_1;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////// INITIALIZES ALL SCENARIO VARIABLES CALLBACKS
void pg_initializationCallBacks(void);


#endif