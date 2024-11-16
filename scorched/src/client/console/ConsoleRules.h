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

#if !defined(AFX_ConsoleRULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_)
#define AFX_ConsoleRULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_

#include <console/ConsoleRule.h>

class Console;
class ConsoleRules  
{
public:
	ConsoleRules();
	virtual ~ConsoleRules();

	void addLine(Console *console, const char *line);

	void addRule(ConsoleRule *rule);
	void removeRule(ConsoleRule *rule);

	std::string matchRule(const char *line,
		std::vector<ConsoleRule *> &matches);
	void dump(std::vector<std::string> &resultList);

protected:
	typedef std::multimap<std::string, ConsoleRule *> RulesMap;
	RulesMap rules_;

	bool parseLine(const char *line, 
		std::vector<ConsoleRuleValue> &split);
	void parseAddLine(int position, const char *line, 
		std::vector<ConsoleRuleValue> &split);
	ConsoleRule *matchRule(std::vector<ConsoleRuleValue> &values,
		std::vector<ConsoleRule *> &closeMatches);
	void getMatchedRules(
		std::vector<ConsoleRule *> &result,
		std::multimap<int, ConsoleRule *> &matchedRules,
		int argCount);
};

#endif // !defined(AFX_ConsoleRULES_H__F75EC197_F0BD_459C_BC62_929D3A97FB5A__INCLUDED_)
