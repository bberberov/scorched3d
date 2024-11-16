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

#include <XML/XMLFile.h>
#include <common/FileLines.h>

XMLFile::XMLFile(bool useContentNodes) :
	parser_(useContentNodes)
{
}

XMLFile::~XMLFile()
{
}

bool XMLFile::readFile(const std::string &fileName)
{
	parser_.setSource(fileName);
	FileLines lines;
	// Load the file
	// Failing to find the file is not an error
	if (!lines.readFile(fileName)) 
	{
		if (!S3D::fileExists(fileName))
		{
			fileError_ = std::string("Failed to find file \"") + fileName + "\"";
		}

		return true;
	}

	// Parse the file
	std::vector<std::string>::iterator itor;
	for (itor = lines.getLines().begin();
		itor != lines.getLines().end();
		++itor)
	{
		if (!parser_.parse((*itor).c_str(), (int) (*itor).size(), 0)) return false;

		// Hack to get it to count lines correctly :)
		if (!parser_.parse("\n", 1, 0)) return false;
	}
	if (!parser_.parse(0, 0, 1)) return false;

	return true;
}
