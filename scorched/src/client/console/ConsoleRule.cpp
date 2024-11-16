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

#include <console/ConsoleRule.h>
#include <common/Defines.h>
#include <string.h>

ConsoleRuleParam::ConsoleRuleParam(const std::string &constant) :
	name_(constant), type_(ConsoleRuleTypeNone)
{
}

ConsoleRuleParam::ConsoleRuleParam(const std::string &name, ConsoleRuleType type) :
	name_(name), type_(type)
{
}

ConsoleRule::ConsoleRule(const char *name,
	const std::vector<ConsoleRuleParam> &params,
	unsigned int userData) : 
	name_(name), params_(params), 
	userData_(userData)
{

}

ConsoleRule::~ConsoleRule()
{

}

std::string ConsoleRule::toString()
{
	std::string result;
	result.append(getName());
	for (int p=0; p<(int) params_.size(); p++)
	{
		ConsoleRuleParam &param = params_[p];
		result.append(" ");
		switch (param.getType())
		{
		case ConsoleRuleTypeBoolean:
			result.append("<on|off>");
			break;
		case ConsoleRuleTypeNumber:
			result.append("<number>");
			break;
		case ConsoleRuleTypeString:
			result.append("<").append(param.getName()).append(">");
			break;
		case ConsoleRuleTypeNone:
			result.append(param.getName());
			break;
		}
	}
	return result;
}

std::string ConsoleRule::toString(std::vector<ConsoleRuleValue> &values)
{
	std::string result;
	result.append(getName()).append(" ");
	for (int p=1; p<(int) values.size(); p++)
	{
		ConsoleRuleParam &param = params_[p - 1];
		ConsoleRuleValue &value = values[p];
		switch (param.getType())
		{
		case ConsoleRuleTypeBoolean:
			result.append(value.valueString.c_str());
			break;
		case ConsoleRuleTypeNumber:
			result.append(value.valueString.c_str());
			break;
		case ConsoleRuleTypeString:
			result.append(value.valueString.c_str());
			break;
		case ConsoleRuleTypeNone:
			result.append(param.getName());
			break;
		}
		result.append(" ");
	}
	return result;
}

std::string ConsoleRule::valuesToString(std::vector<ConsoleRuleValue> &values)
{
	if (values.empty()) return "";

	std::string result;
	result.append(values[0].valueString);
	for (int p=1; p<(int) values.size(); p++)
	{
		ConsoleRuleValue &value = values[p];
		result.append(" ");
		switch (value.type)
		{
		case ConsoleRuleTypeBoolean:
			result.append("<on|off>");
			break;
		case ConsoleRuleTypeNumber:
			result.append("<number>");
			break;
		case ConsoleRuleTypeString:
			result.append("<string>");
			break;
		case ConsoleRuleTypeNone:
			result.append(value.valueString);
			break;
		}
	}
	return result;
}

bool ConsoleRule::matchesExactParams(std::vector<ConsoleRuleValue> &values)
{
	if (values.size() != params_.size() + 1) return false;

	for (int v=1; v<(int) values.size(); v++)
	{
		ConsoleRuleParam &param = params_[v-1];
		ConsoleRuleValue &value = values[v];

		switch (param.getType())
		{
		case ConsoleRuleTypeNone:
			if (0 != stricmp(param.getName(), value.valueString.c_str())) return false;
			break;
		case ConsoleRuleTypeString:
			break;
		case ConsoleRuleTypeBoolean:
			if (value.type != ConsoleRuleTypeBoolean) return false;
			break;
		case ConsoleRuleTypeNumber:
			if (value.type != ConsoleRuleTypeNumber) return false;
			break;
		}
	}

	return true;
}

bool ConsoleRule::matchesPartialParams(std::vector<ConsoleRuleValue> &values)
{
	if (values.size() > params_.size() + 1) return false;

	for (int v=1; v<(int) values.size(); v++)
	{
		ConsoleRuleParam &param = params_[v-1];
		ConsoleRuleValue &value = values[v];

		switch (param.getType())
		{
		case ConsoleRuleTypeNone:
			if (v == int(values.size()) - 1 &&
				value.valueString.length() < strlen(param.getName()))
			{
				if (0 != _strnicmp(param.getName(), 
					value.valueString.c_str(), value.valueString.length())) return false;
			}
			else
			{
				if (0 != stricmp(param.getName(), 
					value.valueString.c_str())) return false;
			}
			break;
		case ConsoleRuleTypeString:
			break;
		case ConsoleRuleTypeBoolean:
			if (value.valueString.length() == 1 &&
				0 != _strnicmp(value.valueString.c_str(), "o", 1)) return false;
			else if (value.valueString.length() == 2 &&
				(0 != _strnicmp(value.valueString.c_str(), "on", 2) &&
				0 != _strnicmp(value.valueString.c_str(), "of", 2))) return false;
			else if (value.valueString.length() == 3 &&
				0 != _strnicmp(value.valueString.c_str(), "off", 3)) return false;
			else if (value.valueString.length() > 3) return false;
			return true;
			break;
		case ConsoleRuleTypeNumber:
			if (value.type != ConsoleRuleTypeNumber) return false;
			break;
		}
	}

	return true;
}

std::vector<ConsoleRuleParam> ConsoleUtil::formParams(
	const ConsoleRuleParam &param1)
{
	std::vector<ConsoleRuleParam> result;
	result.push_back(param1);
	return result;
}

std::vector<ConsoleRuleParam> ConsoleUtil::formParams(
	const ConsoleRuleParam &param1, 
	const ConsoleRuleParam &param2)
{
	std::vector<ConsoleRuleParam> result;
	result.push_back(param1);
	result.push_back(param2);
	return result;
}

std::vector<ConsoleRuleParam> ConsoleUtil::formParams(
	const ConsoleRuleParam &param1, 
	const ConsoleRuleParam &param2, 
	const ConsoleRuleParam &param3)
{
	std::vector<ConsoleRuleParam> result;
	result.push_back(param1);
	result.push_back(param2);
	result.push_back(param3);
	return result;
}

std::vector<ConsoleRuleParam> ConsoleUtil::formParams(
	const ConsoleRuleParam &param1, 
	const ConsoleRuleParam &param2, 
	const ConsoleRuleParam &param3, 
	const ConsoleRuleParam &param4)
{
	std::vector<ConsoleRuleParam> result;
	result.push_back(param1);
	result.push_back(param2);
	result.push_back(param3);
	result.push_back(param4);
	return result;
}

std::vector<ConsoleRuleParam> ConsoleUtil::formParams(
	const ConsoleRuleParam &param1, 
	const ConsoleRuleParam &param2, 
	const ConsoleRuleParam &param3, 
	const ConsoleRuleParam &param4, 
	const ConsoleRuleParam &param5)
{
	std::vector<ConsoleRuleParam> result;
	result.push_back(param1);
	result.push_back(param2);
	result.push_back(param3);
	result.push_back(param4);
	result.push_back(param5);
	return result;
}
