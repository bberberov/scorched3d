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

#include <common/Defines.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef _WIN32
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>
#else
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#endif
#include <common/FileList.h>

FileList::FileList(const std::string &directory, 
				   const std::string &filter, 
				   bool fullPath, bool recurse) 
{
	if (recurse)
	{
		status_ = addAllFiles(directory, directory, filter, fullPath);
	}
	else
	{
		status_ = readFiles(directory, filter, fullPath);
	}
}

FileList::~FileList()
{

}

bool FileList::addAllFiles(const std::string &baseDir, 
						   const std::string &directory, 
						   const std::string &filter, 
						   bool fullPath)
{
	FileList newList(directory, filter, true);
	if (newList.getStatus())
	{
		ListType::iterator itor;
		for (itor = newList.getFiles().begin();
			itor != newList.getFiles().end();
			++itor)
		{
			std::string &fileName = (*itor);

			struct stat buf;
			memset(&buf, 0, sizeof(buf));
			if (stat(fileName.c_str(), &buf) == 0)
			{
#ifdef WIN32
				if (buf.st_mode & _S_IFDIR)
#else
				if (buf.st_mode & S_IFDIR)
#endif
				{
					addAllFiles(baseDir, fileName.c_str(), filter, fullPath);
				}
				else
				{
					if (fullPath) files_.push_back(fileName);
					else
					{
						const char *add = fileName.c_str();
						add += MIN(baseDir.size() + 1, fileName.size());
						files_.push_back(add);
					}
				}
			}
		}

		return true;
	}
	return false;
}

bool FileList::getStatus()
{
	return status_;
}

bool FileList::readFiles(const std::string &directory, 
						 const std::string &filter, 
						 bool fullPath)
{
#ifndef _WIN32
	DIR *dirp;
   	struct dirent *direntp;
   	dirp = opendir(directory.c_str());
	if (!dirp)
		return false;
	while ( (direntp = readdir( dirp )) != NULL )
   	{
		if (fnmatch(filter.c_str(), (const char *)direntp->d_name,0))
			continue;
		
		if (direntp->d_name[0] != '.')
		{
			if (fullPath) files_.push_back(std::string(directory) + "/" + std::string(direntp->d_name));
			else files_.push_back(std::string(direntp->d_name));
		}
   	}

	closedir( dirp );
#else
	WIN32_FIND_DATA findFileData;
	HANDLE han = INVALID_HANDLE_VALUE;
	ZeroMemory(&findFileData, sizeof(findFileData));
	std::string fullName = std::string(directory) + "/" + std::string(filter);

	han = FindFirstFile(fullName.c_str(), &findFileData);
	if (han == INVALID_HANDLE_VALUE) return false;

	do 
	{
		if (findFileData.cFileName[0] != '.')
		{
			if (fullPath) files_.push_back(std::string(directory) + "/" + std::string(findFileData.cFileName));
			else files_.push_back(std::string(findFileData.cFileName));
		}
	}
	while (FindNextFile(han, &findFileData));
	FindClose(han);
#endif

	return true;
}
