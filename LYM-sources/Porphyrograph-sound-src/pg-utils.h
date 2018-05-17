////////////////////////////////////////////////////////////////////
// 
// 
//     File pg-utils.h.cpp
// 
//
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
////////////////////////////////////////////////////////////////////

#ifndef PG_UTILS_H
#define PG_UTILS_H

////////////////////////////////////////////////////////////////////
// DIR SCANNING UTILITIES
////////////////////////////////////////////////////////////////////
bool is_substring_index(char * charstr, int ind);
bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir);
bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile);
string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile);
string * nextFile(string *dirpath, int *currentDirIndex, int *currentFileIndex);

////////////////////////////////////////////////////////////////////
// DIVERSE UTILITIES
////////////////////////////////////////////////////////////////////
bool pg_audio_errorMsg(PaError err);
#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp);
#endif
double RealTime(void);

#endif
