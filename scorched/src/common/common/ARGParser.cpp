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

#include <common/ARGParser.h>
#include <common/Defines.h>
#include <stdio.h>

ARGParser::Entry::Entry(ARGParserBoolI *destBoolArg,
						ARGParserIntI *destIntArg,
						ARGParserStringI *destStringArg,
						char **destCArg,
						int *destIArg,
						bool *destBArg,
						char *helpArg) :
	destBool(destBoolArg),
	destInt(destIntArg),
	destString(destStringArg),
	destC(destCArg),
	destI(destIArg),
	destB(destBArg),
	help(helpArg)
{

}

ARGParser::ARGParser()
{

}

ARGParser::~ARGParser()
{

}

bool ARGParser::parse(int argc,char *argv[])
{
 	int i;
	std::string	cmd, space=(" "), quote=("\"");

    for (i=1; i<argc;i++)
 	        cmd += space + quote + argv[i] + quote;

	return parse(cmd.c_str());
}

bool ARGParser::parse(const char *lpCmdLine)
{
	std::list<std::string> cmdLine;

	if (!parseLineIntoStrings(lpCmdLine, cmdLine))
	{
			showArgs("ERROR: Unmatched parenthasis.\n\n");
			return false;
	}

	while (!cmdLine.empty())
	{
		std::string &firstCommand = cmdLine.front();
		if (strcmp(firstCommand.c_str(), "-h") == 0 ||
			strcmp(firstCommand.c_str(), "-help") == 0)
		{
			showArgs();
			return false;
		}

		std::map<std::string, Entry>::iterator itor = argMap_.find(firstCommand);
		if (itor != argMap_.end())
		{
			cmdLine.pop_front();
			if (!parseArg(itor->second, cmdLine))
			{
				return false;
			}
		}
		else if (!nonParamMap_.empty() &&
			firstCommand.c_str()[0] != '-')
		{
			if (!nonParamMap_.begin()->second.destString->
				setStringArgument(cmdLine.front().c_str()))
			{
				showArgs("ERROR: Parameter is not within allowed values\n\n");
				return false;
			}
			cmdLine.pop_front();
		}
		else
		{
			char buffer[255];
			snprintf(buffer, 255, "ERROR: Unknown parameter : \"%s\"\n\n", firstCommand.c_str());
			showArgs(buffer);
			return false;
		}
	}

	return true;
}

bool ARGParser::parseLineIntoStrings(const char *line, std::list<std::string> &cmdLine)
{
	bool inQuote = false;
	std::string currentEntry;
	for (int i=0; i<(int) strlen(line)+1; i++)
	{
		const char c = line[i];

		if (c == '\0')
		{
			if (!currentEntry.empty()) cmdLine.push_back(currentEntry.c_str());
			currentEntry = "";
		}
		else if ((c == ' ') && !inQuote)
		{
			if (!currentEntry.empty()) cmdLine.push_back(currentEntry.c_str());
			currentEntry = "";
		}
		else if (c == '\"')
		{
			inQuote = !inQuote;
		}
		else
		{
			currentEntry += c;
		}
	}

	return !inQuote;
}

bool ARGParser::parseArg(ARGParser::Entry &newEntry, std::list<std::string> &cmdLine)
{
	if (newEntry.destB)
	{
		*newEntry.destB = true;
	}
	else if (newEntry.destI)
	{
		if (!cmdLine.empty())
		{
			*newEntry.destI = atoi(cmdLine.front().c_str());
			cmdLine.pop_front();
		}
		else
		{
			showArgs("ERROR: Expected <int> parameter\n\n");
			return false;
		}
	}
	else if (newEntry.destC)
	{
		if (!cmdLine.empty())
		{
			*newEntry.destC = new char[strlen(cmdLine.front().c_str())+1];
			strcpy(*newEntry.destC, cmdLine.front().c_str());
			cmdLine.pop_front();
		}
		else
		{
			showArgs("ERROR: Expected <string> parameter\n\n");
			return false;
		}
	}
	else if (newEntry.destBool)
	{
		if (!newEntry.destBool->setBoolArgument(true))
		{
			showArgs("ERROR: Parameter is not within allowed values\n\n");
			return false;
		}
	}
	else if (newEntry.destInt)
	{
		if (!cmdLine.empty())
		{
			if (!newEntry.destInt->setIntArgument(atoi(cmdLine.front().c_str())))
			{
				showArgs("ERROR: Parameter is not within allowed values\n\n");
				return false;
			}
			cmdLine.pop_front();
		}
		else
		{
			showArgs("ERROR: Expected <int> parameter\n\n");
			return false;
		}
	}
	else if (newEntry.destString)
	{
		if (!cmdLine.empty())
		{
			if (!newEntry.destString->setStringArgument(cmdLine.front().c_str()))
			{
				showArgs("ERROR: Parameter is not within allowed values\n\n");
				return false;
			}
			cmdLine.pop_front();
		}
		else
		{
			showArgs("ERROR: Expected <string> parameter\n\n");
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void ARGParser::showArgs(const char *topString)
{
	char buffer[4048];
	if (topString)
	{
		strcpy(buffer, topString);
	}
	else
	{
		buffer[0] = '\0';
	}
	strcat(buffer, "Usage : \n");

	std::map<std::string, Entry>::iterator itor;
	for (itor = argMap_.begin();
		itor != argMap_.end();
		++itor)
	{
		const char *type = "unknown";
		if (itor->second.destC || itor->second.destString) type = "<string>";
		else if (itor->second.destB || itor->second.destBool) type = "";
		else if (itor->second.destI || itor->second.destInt) type = "<int>";

		char buffer2[255];
		snprintf(buffer2, 255, "\t%s %s", itor->first.c_str(), type);
		if (itor->second.help.size())
		{
			strcat(buffer2, std::string(abs(10 - int(strlen(buffer2))), ' ').c_str());
			strcat(buffer2, itor->second.help.c_str());
		}
		strcat(buffer2, "\n");

		strcat(buffer, buffer2);
	}
	for (itor = nonParamMap_.begin();
		itor != nonParamMap_.end();
		++itor)
	{
		char buffer2[255];
		buffer2[0] = '\0';
		strcat(buffer2, "\t[");
		strcat(buffer2, itor->first.c_str());
		strcat(buffer2, "]");
		if (itor->second.help.size())
		{
			strcat(buffer2, std::string(abs(10 - int(strlen(buffer2))), ' ').c_str());
			strcat(buffer2, itor->second.help.c_str());
		}
		strcat(buffer2, "\n");

		strcat(buffer, buffer2);
	}

	S3D::dialogMessage("Arguments", buffer, false);
}

void ARGParser::addEntry(char *cmd, char **destStr, char *help)
{
	Entry newEntry(NULL, NULL, NULL, destStr, NULL, NULL, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addEntry(char *cmd, int *destI, char *help)
{
	Entry newEntry(NULL, NULL, NULL, NULL, destI, NULL, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addEntry(char *cmd, bool *destB, char *help)
{
	Entry newEntry(NULL, NULL, NULL, NULL, NULL, destB, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addEntry(char *cmd, ARGParserBoolI *destBool, char *help)
{
	Entry newEntry(destBool, NULL, NULL, NULL, NULL, NULL, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addEntry(char *cmd, ARGParserIntI *destInt, char *help)
{
	Entry newEntry(NULL, destInt, NULL, NULL, NULL, NULL, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addEntry(char *cmd, ARGParserStringI *destString, char *help)
{
	Entry newEntry(NULL, NULL, destString, NULL, NULL, NULL, help);
	addNewEntry(cmd, newEntry);
}

void ARGParser::addNewEntry(const char *cmd, ARGParser::Entry &newEntry)
{
	argMap_[cmd] = newEntry;
}

void ARGParser::addNonParamEntry(char *cmd, ARGParserStringI *destString, char *help)
{
	Entry newEntry(NULL, NULL, destString, NULL, NULL, NULL, help);
	nonParamMap_[cmd] = newEntry;
}
