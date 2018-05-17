/*! \file pg-conf.h
 * 
 * 
* 
 *     File pg-conf.h
 * 
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as publifscenesshed by the Free Software Foundation; either version 2.1
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

#ifndef PG_CONF_H
#define PG_CONF_H

// UDP servers and clients
extern pg_IPServer            **IP_Servers;
extern int                      nb_IP_Servers;
extern pg_IPClient            **IP_Clients;
extern int                      nb_IP_Clients;

////////////////////////////////////////////////////////////////////
// AUDIO SAMPLE DATA
// short samples typically for sequences, loaded at ones
extern int NbShortTracks;
extern AudioSampleDataStruct **ShortTrackBufferData;
// long smaples, typically for soundtrack, streamed progressively
extern int NbLongTracks;
extern AudioSampleDataStruct **LongTrackBufferData;
extern AudioLongTrackStreaming **LongTrackStreamings;

extern int NbAudioSequences;
extern AudioSequenceStreaming **AudioSequences;
extern bool atLeastOneTerrainsVaguesSequence;

void ReportError(char *errorString);

void saveInitialTimesAndDurations(void);
void restoreInitialTimesAndDurations( void );

void parseConfigurationFile( std::ifstream& confFin );
void LoadConfigurationFile( string confFileName );

#endif
