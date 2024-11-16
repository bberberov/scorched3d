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

#include <console/Console.h>
#include <console/ConsoleRuleFn.h>
#include <common/Defines.h>
#include <stdio.h>

ConsoleRuleFnI::~ConsoleRuleFnI()
{
}

static std::vector<ConsoleRuleParam> generateFnParams(
	ConsoleRuleType type, bool write)
{
	std::vector<ConsoleRuleParam> params;
	if (!write)
	{
		switch(type)
		{
		case ConsoleRuleTypeBoolean:
			params.push_back(ConsoleRuleParam("<boolean>", ConsoleRuleTypeBoolean));
			break;
		case ConsoleRuleTypeNumber:
			params.push_back(ConsoleRuleParam("<number>", ConsoleRuleTypeNumber));
			break;
		default:
			params.push_back(ConsoleRuleParam("<string>", ConsoleRuleTypeString));
			break;
		}
	}
	return params;
}

ConsoleRuleFn::ConsoleRuleFn(const char *name, 
	ConsoleRuleFnI *user, 
	ConsoleRuleType type, 
	bool write) :
	ConsoleRule(name, generateFnParams(type, write)), 
	user_(user), 
	type_(type)
{

}

ConsoleRuleFn::~ConsoleRuleFn()
{

}

void ConsoleRuleFn::runRule(
	Console *console,
	const char *wholeLine,
	std::vector<ConsoleRuleValue> &values)
{
	if (values.size() == 2)
	{
		ConsoleRuleValue &value = values[1];
		setValue(value);
	}
	console->addLine(false, getValue());
}

void ConsoleRuleFn::setValue(ConsoleRuleValue &split)
{
	switch (type_)
	{
	case ConsoleRuleTypeBoolean:
		user_->setBoolParam(name_.c_str(), split.valueBool);
		break;
	case ConsoleRuleTypeNumber:
		user_->setNumberParam(name_.c_str(), split.valueNumber);
		break;
	case ConsoleRuleTypeString:
		user_->setStringParam(name_.c_str(), split.valueString.c_str());
	}
}

const char *ConsoleRuleFn::getValue()
{
	static std::string value;
	switch (type_)
	{
	case ConsoleRuleTypeBoolean:
		value = (user_->getBoolParam(name_.c_str())?"on":"off");
		break;
	case ConsoleRuleTypeNumber:
		static char buffer[10];
		snprintf(buffer, 10, "%.2f", user_->getNumberParam(name_.c_str()));
		value = buffer;
		break;
	case ConsoleRuleTypeString:
		value = user_->getStringParam(name_.c_str());
		break;
	default:
		value = "None";
	}

	static std::string result;
	result = name_ + "=" + value;

	return result.c_str();
}

