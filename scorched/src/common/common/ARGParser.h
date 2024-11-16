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


// ARGParser.h: interface for the ARGParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARGPARSER_H__F5D7DE2F_967A_44B0_A2DF_423F96B02EC0__INCLUDED_)
#define AFX_ARGPARSER_H__F5D7DE2F_967A_44B0_A2DF_423F96B02EC0__INCLUDED_

#include <list>
#include <map>
#include <string>

class ARGParserBoolI
{
public:
	virtual ~ARGParserBoolI() {}
	virtual bool setBoolArgument(bool value) = 0;
};

class ARGParserIntI
{
public:
	virtual ~ARGParserIntI() {}
	virtual bool setIntArgument(int value) = 0;
};

class ARGParserStringI
{
public:
	virtual ~ARGParserStringI() {}
	virtual bool setStringArgument(const char *value) = 0;
};

class ARGParser  
{
public:
	ARGParser();
	virtual ~ARGParser();

	bool parse(const char *lpCmdLine);
	bool parse(int argc,char *argv[]);
	void addEntry(char *cmd, char **destStr, char *help = "");
	void addEntry(char *cmd, int *destI, char *help = "");
	void addEntry(char *cmd, bool *destB, char *help = "");

	void addEntry(char *cmd, ARGParserBoolI *destBool, char *help = "");
	void addEntry(char *cmd, ARGParserIntI *destInt, char *help = "");
	void addEntry(char *cmd, ARGParserStringI *destString, char *help = "");

	void addNonParamEntry(char *cmd, ARGParserStringI *destString, char *help = "");

	void showArgs(const char *topString = NULL);

protected:
	struct Entry
	{
		Entry(ARGParserBoolI *destBoolArg = 0,
			  ARGParserIntI *destIntArg = 0,
			  ARGParserStringI *destStringArg = 0,
			  char **destCArg = 0,
			  int *destIArg = 0,
			  bool *destBArg = 0,
			  char *helpArg = "");

		ARGParserBoolI *destBool;
		ARGParserIntI *destInt;
		ARGParserStringI *destString;
		char **destC;
		int *destI;
		bool *destB;
		std::string help;
	};

	std::map<std::string, Entry> argMap_;
	std::map<std::string, Entry> nonParamMap_;
	void addNewEntry(const char *cmd, ARGParser::Entry &entry);
	bool parseLineIntoStrings(const char *line, std::list<std::string> &cmdLine);
	bool parseArg(ARGParser::Entry &newEntry, std::list<std::string> &cmdLine);
};

#endif // !defined(AFX_ARGPARSER_H__F5D7DE2F_967A_44B0_A2DF_423F96B02EC0__INCLUDED_)
