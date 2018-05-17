////////////////////////////////////////////////////////////////////
// 
// 
// 
//     File pg_wav_RW.cpp
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

#include "pg-all_include.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// DIR SCANNING UTILITIES
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

// FUNCTIONS FOR TESTING SUBDIR FILES (MADE FOR SAMPLES AND IMAGES)

bool is_substring_index(std::string str, int ind) {
	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_substring_index(char * charstr, int ind) {
	std::string str(charstr);

	char string_number[16];
	sprintf(string_number, "%03d", ind);
	std::string string_number_s(string_number);

	if (str.find(string_number_s) != std::string::npos) {
		// std::cout << "*** FOUND: " + filepath << std::endl;
		return true;
	}
	return false;
}

bool is_subdir_index(struct dirent *dirp, std::string *dirpath, int inddir) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory and the name contains the integer 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode) && is_substring_index(dirp->d_name, inddir)) { // the file is a directory 
		// and contains the integer substring
		return true;
	}
	return false;
}

bool is_subfile_index(struct dirent *dirp, std::string *dirpath, int indfile) {
	string filepath = *dirpath + "/" + dirp->d_name;
	struct stat filestat;

	// If the file is a directory (or is in some way invalid) we'll skip it 
	if (stat(filepath.c_str(), &filestat)) return false; // colleccts file status and returns 0 on success
	if (S_ISDIR(filestat.st_mode)) { // the file is a directory 
		// and contains the integer substring
		return false;
	}
	ifstream fin;
	fin.open(filepath.c_str());
	if (fin) {
		if (is_substring_index(dirp->d_name, indfile)) {
			fin.close();
			return true;
		}
		fin.close();
	}
	return false;
}
string * is_subdir_subfile_index(std::string *dirpath, int inddir, int indfile) {
	DIR *dp = opendir(dirpath->c_str());
	if (dp == NULL)
	{
		std::cout << "Error(" << errno << ") opening " << *dirpath << std::endl;
		return NULL;
	}
	// std::cout << std::endl << "dir to get files of: " + dirpath << std::endl;
	struct dirent *dirp;
	while ((dirp = readdir(dp))) {
		if (is_subdir_index(dirp, dirpath, inddir)) {
			string subdirpath(*dirpath + "/" + dirp->d_name);
			DIR *subdp = opendir(subdirpath.c_str());
			if (subdp == NULL)
			{
				std::cout << "Error(" << errno << ") opening " << subdirpath << std::endl;
				return NULL;
			}
			struct dirent *subdirp;
			while ((subdirp = readdir(subdp))) {
				if (is_subfile_index(subdirp, &subdirpath, indfile)) {
					string * subdirfilepath = new string(subdirpath + "/" + subdirp->d_name);
					// std::cout << "found: (" << inddir << "," << indfile << ") in " << *subdirfilepath << std::endl;
					closedir(subdp);
					closedir(dp);
					return subdirfilepath;
				}
			}
			closedir(subdp);
		}
	}
	closedir(dp);
	return NULL;
}

string * nextFile(string *dirpath, int *currentDirIndex, int *currentFileIndex) {
	string * returnedString;
	if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
		(*currentFileIndex)++;
		return returnedString;
	}
	else {
		(*currentDirIndex)++;
		(*currentFileIndex) = 0;
		if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
			(*currentFileIndex)++;
			return returnedString;
		}
		else {
			(*currentDirIndex) = 0;
			(*currentFileIndex) = 0;
			if ((returnedString = is_subdir_subfile_index(dirpath, *currentDirIndex, *currentFileIndex))) {
				(*currentFileIndex)++;
				return returnedString;
			}
			else {
				std::cout << "Error(" << errno << ") opening " << dirpath << std::endl;
				return NULL;
			}
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// DIVERSE UTILITIES
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// REAL TIME
////////////////////////////////////////////////////////////////////
#ifdef _WIN32
static const double epoch = 116444736000000000.;
int gettimeofday(struct timeval* tp, void* tzp) {
	SYSTEMTIME system_time;
	FILETIME file_time;
	ULARGE_INTEGER ularge;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp->tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	/* 0 indicates that the call succeeded. */
	return 0;
}
#endif

double RealTime(void) {
	struct timeval time;
	gettimeofday(&time, 0);
	double realtime = (double)time.tv_sec + ((double)time.tv_usec / 1000000.);
	return realtime;
}

///////////////////////////////////////////////////////////////////
// ERROR

bool pg_audio_errorMsg(PaError err) {
	Pa_Terminate();
	printf("An error occured while using the portaudio stream\n");
	printf("Error number: %d\n", err);
	printf("Error message: %s\n", Pa_GetErrorText(err));
	return false;
}

