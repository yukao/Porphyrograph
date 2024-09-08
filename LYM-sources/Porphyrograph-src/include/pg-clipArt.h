/*! \file pg-clipArt.h
 *
 *
 *     File pg-clipArt.h
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

 /* based on nvpr_init.h - initialize NVPR API */
 /* Copyright NVIDIA Corporation, 2010. */

#ifndef __PG_CLIPARTS_H__
#define __PG_CLIPARTS_H__

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTs
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIPART MANAGEMENT CLASS
//////////////////////////////////////////////////////////////////////////////////////////////////////

class ClipArt {
public:
	// number of paths for each file
	int pg_nb_paths_in_ClipArt;
	// file names
	string pg_ClipArt_fileNames;
	// geometrical transformations
	float pg_ClipArt_Scale;
	float pg_ClipArt_Rotation;
	float pg_ClipArt_Translation_X;
	float pg_ClipArt_Translation_Y;
	// color
	std::string *pg_ClipArt_Colors;
	// subpath display
	bool* pg_ClipArt_SubPath;
	// base ID of the GPU files
	GLuint ClipArt_file_baseID;
	// fill color table
	unsigned int *pg_ClipArt_path_fill_color;
	ClipArt(void) {
		// number of paths for each file
		pg_nb_paths_in_ClipArt = 0;
		// file names
		pg_ClipArt_fileNames = "";
		// geometrical transformations
		pg_ClipArt_Scale = 1.f;
		pg_ClipArt_Rotation = 0.f;
		pg_ClipArt_Translation_X = 0.f;
		pg_ClipArt_Translation_Y = 0.f;
		// color
		pg_ClipArt_Colors = NULL;
		// subpath display
		pg_ClipArt_SubPath = NULL;
		// base ID of the GPU files
		ClipArt_file_baseID = NULL_ID;
		// fill color table
		pg_ClipArt_path_fill_color = NULL;
	}
	void pg_Display_One_ClipArt(int indLayer);
	~ClipArt(void) {
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////// 

extern vector<ClipArt>pg_ClipArts[PG_MAX_SCENARIOS];
extern bool pg_has_NV_path_rendering;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTED FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void pg_parseScenario_ClipArt(std::ifstream& scenarioFin, int indScenario);
void pg_aliasScript_ClipArt(string address_string, string string_argument_0,
	float float_arguments[PG_MAX_OSC_ARGUMENTS], int nb_arguments, int indVar);
void pg_loadAll_ClipArts(void);
void pg_listAll_ClipArts(void);
void pg_Display_ClipArt_Text(int* indActiveDisplayText, int mobile);
void pg_Display_All_ClipArt(int activeFiles);


#endif /* __PG_CLIPARTS_H__ */

