/*! \file pg-audio.h
 *
 *
 *     File pg-audio.h
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

extern string pg_screenMessage;

bool pg_ReadAllDisplayMessages(string basefilename);
int pg_displayMessage_update(int indMesg);
void pg_screenMessage_update(void);
void pg_writeMessageOnScreen(string text);
void pg_loadScreenMessageTexture(void);
void pg_bindFontTextureID(void);
void pg_bindMessageTextureID(void);
void pg_init_texDataScreenFont(void);
void pg_loadScreenFontTexture(void);

#endif