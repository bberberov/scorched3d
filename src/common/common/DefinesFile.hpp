////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINESFILE__
#define __DEFINESFILE__

#include <time.h>
#include <string>

namespace S3D
{
	enum FileLocation
	{
		eInvalidLocation = 0,
		eAbsLocation,
		eDataLocation,
		eModLocation
	};

	void fileDos2Unix(std::string &file);
	bool fileExists(const std::string &file);
	bool dirExists(const std::string &file);
	bool dirMake(const std::string &file);
	bool removeFile(const std::string &file);
	const std::string getLocation(FileLocation imageLocation, const std::string &filename);
	time_t fileModTime(const std::string &file);
	std::string getHomeDir();
	std::string getOSDesc();
}

#endif // __DEFINESFILE__
