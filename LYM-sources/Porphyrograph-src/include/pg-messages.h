/*! \file pg-message.h
 *
 *
 *     File pg-message.h
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

#ifndef PG_MESSAGES_H
#define PG_MESSAGES_H

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

extern string pg_screenMessage;
extern GLfloat pg_messageTransparency;
extern double pg_LastScreenMessageDecayTime;

class pg_TextureData;
extern pg_TextureData pg_texDataScreenFont;

extern std::string pg_MessageFile[PG_MAX_SCENARIOS];

extern std::string* pg_displayTextList;
extern int* pg_displayTextFirstInChapter;
extern int pg_NbDisplayTexts;
extern int pg_displayText_maxLen;
extern int pg_Ind_Current_DisplayText;
extern float* pg_displayText_rand_translX;
extern float* pg_displayText_rand_translY;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

/// screen font initialization
void pg_init_texDataScreenFont(void);
/// screen_message initialization
void pg_init_screen_message(void);
// read a text file with messages
bool pg_ReadAllDisplayMessages(string basefilename);
// screen display of message
void pg_writeMessageOnScreen(string text);
/// screen message update
void pg_screenMessage_update(void);

/// screen message and font texture transfer to GPU
void pg_loadScreenMessageTexture(void);
void pg_loadScreenFontTexture(void);
void pg_bindFontTextureID(void);
void pg_bindMessageTextureID(void);


#endif