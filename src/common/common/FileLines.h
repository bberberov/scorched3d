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


// FileLines.h: interface for the FileLines class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELINES_H__EE120EC6_091D_4625_A3D0_F24F0D701A62__INCLUDED_)
#define AFX_FILELINES_H__EE120EC6_091D_4625_A3D0_F24F0D701A62__INCLUDED_

#include <vector>
#include <string>

class FileLines  
{
public:
	FileLines();
	virtual ~FileLines();

	bool readFile(const std::string &filename);
	bool writeFile(const std::string &filename);

	void addLine(const std::string &text);
	std::vector<std::string> &getLines() { return fileLines_; }
	void getAsOneLine(std::string &output);

protected:
	std::vector<std::string> fileLines_;

};

#endif // !defined(AFX_FILELINES_H__EE120EC6_091D_4625_A3D0_F24F0D701A62__INCLUDED_)
