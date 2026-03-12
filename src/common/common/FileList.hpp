////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_FileList_hpp_INCLUDE__
#define __INCLUDE_FileList_hpp_INCLUDE__

#include <string>
#include <list>

class FileList
{
public:
	typedef std::list< std::string > ListType;

	FileList(
		const std::string& directory,
		const std::string& filter   = "*.*",
		bool               fullPath = true,
		bool               recurse  = false
	);
	virtual ~FileList();

	bool getStatus();

	ListType& getFiles() { return files_; }

protected:
	ListType files_;
	bool     status_;

	bool readFiles( const std::string& directory, const std::string& filter, bool fullPath );
	bool addAllFiles(
		const std::string& baseDir,
		const std::string& directory,
		const std::string& filter,
		bool               fullPath
	);
};

#endif  // __INCLUDE_FileList_hpp_INCLUDE__
