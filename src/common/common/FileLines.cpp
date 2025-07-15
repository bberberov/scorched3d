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

#include <common/FileLines.h>
#include <common/Defines.h>
#include <stdio.h>
#include <stdarg.h>

FileLines::FileLines()
{

}

FileLines::~FileLines()
{

}

bool FileLines::readFile(const std::string &filename)
{
	fileLines_.clear();
	FILE *in = fopen(filename.c_str(), "r");
	if (in)
	{
		char buffer[10048];
		while (fgets(buffer, 10048, in))
		{
			char *wincr = strchr(buffer,'\r');
			if (wincr)
			{ 
				*wincr = '\n'; 
				*(wincr + 1) = '\0'; 
			} 

			char *nl = strchr(buffer, '\n');
			if (nl) *nl = '\0';
			fileLines_.push_back(buffer);
		}
		fclose(in);
	}
	else
	{
		return false;
	}
	
	return true;
}

bool FileLines::writeFile(const std::string &filename)
{
	FILE *out = fopen(filename.c_str(), "w");
	if (!out) return false;

	std::vector<std::string>::iterator itor;
	for (itor = fileLines_.begin();
		itor != fileLines_.end();
		++itor)
	{
		fprintf(out, "%s\n", (*itor).c_str());
	}

	fclose(out);
	return true;
}

void FileLines::addLine(const std::string &text)
{
	fileLines_.push_back(text);
}

void FileLines::getAsOneLine(std::string &output)
{
	std::vector<std::string>::iterator itor;
	for (itor = fileLines_.begin();
		 itor != fileLines_.end();
		 ++itor)
	{
		output += (*itor);
		output += "\n";
	}
}
