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

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <common/Defines.h>
#include <sys/types.h>
#include <sys/stat.h>

#pragma warning(disable : 4996)

#ifdef WIN32
#include <direct.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

std::string S3D::getHomeDir()
{
#ifdef _WIN32
	if (getenv("USERPROFILE")) return getenv("USERPROFILE");
	if (getenv("HOMEPATH") && getenv("HOMEDRIVE"))
	{
		if (0 != strcmp(getenv("HOMEPATH"), getenv("HOMEDRIVE")))
		{
			// Path not set the root
			return getenv("HOMEPATH");
		}
	}
#endif
	if (getenv("HOME")) return getenv("HOME");

	return ".";
}

void S3D::fileDos2Unix(std::string &file)
{
    for (char *f=(char *) file.c_str(); *f; f++)
    {
        if (*f == '\\') *f = '/';
    }
}

bool S3D::dirMake(const std::string &file)
{
#ifdef WIN32
	_mkdir(file.c_str());
#else
	mkdir(file.c_str(), 0755);
#endif
	return true;
}

bool S3D::removeFile(const std::string &file)
{
	return (remove(file.c_str()) == 0);
}

bool S3D::fileExists(const std::string &file)
{
	return (S3D::fileModTime(file) != 0);
}

bool S3D::dirExists(const std::string &file)
{
	return (S3D::fileModTime(file) != 0);
}

time_t S3D::fileModTime(const std::string &file)
{
	struct stat buf;
	memset(&buf, 0, sizeof(buf));
	stat(file.c_str(), &buf );

	return buf.st_mtime;
}

std::string S3D::getOSDesc()
{
#ifdef _WIN32
	return "Windows";
#else
#ifdef __DARWIN__
	return "OSX";
#else
	return "Unix";
#endif
#endif
}

const std::string S3D::getLocation(FileLocation imageLocation, const std::string &filename)
{
	std::string expandedFilename;
	switch (imageLocation)
	{
	case eDataLocation:
		expandedFilename = S3D::getDataFile(filename);
		break;
	case eModLocation:
		expandedFilename = S3D::getModFile(filename);
		break;
	case eAbsLocation:
		expandedFilename = filename;
		break;
	default:
		S3D::dialogExit("S3D::getLocation", 
			S3D::formatStringBuffer("Unknown imagelocation %u", imageLocation));
		break;
	}
	return expandedFilename;
}
