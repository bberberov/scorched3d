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

#if !defined(__INCLUDE_ModFileEntryh_INCLUDE__)
#define __INCLUDE_ModFileEntryh_INCLUDE__

#include <net/NetBuffer.h>
#include <string>

struct ModIdentifierEntry
{
	ModIdentifierEntry(bool a = true,
		const std::string &f = "",
		unsigned int l = 0,
		unsigned int c = 0) :
		addFile(a),
		fileName(f),
		length(l),
		crc(c)
	{
	};

	bool addFile;
	std::string fileName;
	unsigned int length;
	unsigned int crc;
};

class ModFileEntry
{
public:
	ModFileEntry(const std::string &name, 
		unsigned int uncompressedcrc, unsigned int uncompressedSize);
	virtual ~ModFileEntry();

	std::string &getFileName() { return fileName_; }
	unsigned int getUncompressedCrc() { return uncompressedcrc_; }
	unsigned int getUncompressedSize() { return uncompressedSize_; }

protected:
	std::string fileName_;
	unsigned int uncompressedcrc_;
	unsigned int uncompressedSize_;
};

#endif
