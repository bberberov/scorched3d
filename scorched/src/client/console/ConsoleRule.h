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

#if !defined(AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_)
#define AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_

#include <string>
#include <vector>
#include <map>

enum ConsoleRuleType
{
	ConsoleRuleTypeNone = 0,
	ConsoleRuleTypeBoolean,
	ConsoleRuleTypeNumber,
	ConsoleRuleTypeString
};

struct ConsoleRuleValue
{
	int position;

	std::string valueString;
	bool valueBool;
	float valueNumber;

	ConsoleRuleType type;
};

class ConsoleRuleParam
{
public:
	ConsoleRuleParam(const std::string &constant);
	ConsoleRuleParam(const std::string &name, ConsoleRuleType type);

	const char *getName() { return name_.c_str(); }
	ConsoleRuleType getType() { return type_; }

private:
	std::string name_;
	ConsoleRuleType type_;
};

class Console;
class ConsoleRule
{
public:
	ConsoleRule(const char *name, 
		const std::vector<ConsoleRuleParam> &params,
		unsigned int userData = 0);
	virtual ~ConsoleRule();

	virtual void runRule(
		Console *console,
		const char *wholeLine,
		std::vector<ConsoleRuleValue> &values) = 0;

	std::string toString();
	std::string toString(std::vector<ConsoleRuleValue> &values);
	static std::string valuesToString(std::vector<ConsoleRuleValue> &values);

	const char *getName() { return name_.c_str(); }
	std::vector<ConsoleRuleParam> &getParams() { return params_; }
	unsigned int getUserData() { return userData_; }

	bool matchesPartialParams(std::vector<ConsoleRuleValue> &values);
	bool matchesExactParams(std::vector<ConsoleRuleValue> &values);
protected:
	std::string name_;
	std::vector<ConsoleRuleParam> params_;
	unsigned int userData_;

};

class ConsoleUtil
{
public:
	static std::vector<ConsoleRuleParam> formParams(
		const ConsoleRuleParam &param1);
	static std::vector<ConsoleRuleParam> formParams(
		const ConsoleRuleParam &param1, 
		const ConsoleRuleParam &param2);
	static std::vector<ConsoleRuleParam> formParams(
		const ConsoleRuleParam &param1, 
		const ConsoleRuleParam &param2, 
		const ConsoleRuleParam &param3);
	static std::vector<ConsoleRuleParam> formParams(
		const ConsoleRuleParam &param1, 
		const ConsoleRuleParam &param2, 
		const ConsoleRuleParam &param3, 
		const ConsoleRuleParam &param4);
	static std::vector<ConsoleRuleParam> formParams(
		const ConsoleRuleParam &param1, 
		const ConsoleRuleParam &param2, 
		const ConsoleRuleParam &param3, 
		const ConsoleRuleParam &param4, 
		const ConsoleRuleParam &param5);
};

#endif // !defined(AFX_ConsoleRULE_H__7A8C972C_069D_44F7_A604_A8A8D1B4773A__INCLUDED_)
