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

#include <console/ConsoleRules.h>
#include <console/Console.h>
#include <common/Defines.h>

ConsoleRules::ConsoleRules()
{

}

ConsoleRules::~ConsoleRules()
{

}

void ConsoleRules::addRule(ConsoleRule *rule)
{
	removeRule(rule);

	std::string addName = rule->getName();
	_strlwr((char *) addName.c_str());
	rules_.insert(std::pair<const std::string, ConsoleRule *>(addName, rule));
}

void ConsoleRules::removeRule(ConsoleRule *rule) 
{
	std::multimap<std::string, ConsoleRule *>::iterator itor;
	for (itor = rules_.begin();
		itor != rules_.end();
		++itor)
	{
		ConsoleRule *r = itor->second;
		if (r == rule)
		{
			rules_.erase(itor);
			break;
		}
	}
}

std::string ConsoleRules::matchRule(const char *line, 
	std::vector<ConsoleRule *> &matches)
{
	std::vector<ConsoleRuleValue> values;
	parseLine(line, values);

	if (values.empty())
	{
		std::multimap<std::string, ConsoleRule *>::iterator itor;
		for (itor = rules_.begin();
			itor != rules_.end();
			++itor)
		{
			ConsoleRule *rule = itor->second;
			matches.push_back(rule);
		}
		return "";
	}
	else 
	{
		std::multimap<std::string, ConsoleRule *>::iterator itor;
		for (itor = rules_.begin();
			itor != rules_.end();
			++itor)
		{
			ConsoleRule *rule = itor->second;
			ConsoleRuleValue &nameValue = values[0];

			if (values.size() == 1)
			{
				size_t nameLen = strlen(rule->getName());
				if (nameLen >= nameValue.valueString.length() &&
					_strnicmp(line, rule->getName(), nameValue.valueString.length()) == 0)
				{
					matches.push_back((*itor).second);
				}
			}
			else
			{
				if (0 == stricmp(rule->getName(), nameValue.valueString.c_str()))
				{
					if (rule->matchesPartialParams(values))
					{
						matches.push_back((*itor).second);
					}
				}
			}
		}

		if (matches.empty()) return "";
		if (matches.size() == 1) return matches[0]->toString(values);

		ConsoleRuleValue &firstValue = values[0];
		for (int i=(int) firstValue.valueString.length();; i++)
		{
			ConsoleRule *firstRule = matches[0];
			for (int j=0; j<(int)matches.size(); j++)
			{
				ConsoleRule *secondRule = matches[j];
				std::string firstString = firstRule->toString(values);
				std::string secondString = secondRule->toString(values);

				if ((int) strlen(secondString.c_str()) < i ||
					0 != _strnicmp(secondString.c_str(), firstString.c_str(), i))
				{
					std::string buffer;
					buffer.append(secondString.c_str(), i - 1);
					return buffer;					
				}
			}
		}
	}

	return "";
}

void ConsoleRules::addLine(Console *console, const char *line)
{
	std::vector<ConsoleRuleValue> values;
	if (!parseLine(line, values))
	{
		console->addLine(false, S3D::formatStringBuffer(
			"Non terminated quote in : %s",
			line));
		return;
	}

	if (values.empty()) return; // Should never happen!

	std::vector<ConsoleRule *> closeMatches;
	ConsoleRule *exactMatch = matchRule(values, closeMatches);
	if (exactMatch)
	{
		exactMatch->runRule(console, line, values);
	}
	else
	{
		std::string valuesString = ConsoleRule::valuesToString(values);
		console->addLine(false, "Unrecognised function :");
		console->addLine(false, S3D::formatStringBuffer(
			"  %s",
			valuesString.c_str()));
		if (!closeMatches.empty())
		{
			console->addLine(false, "Possible matches are :");
			std::vector<ConsoleRule *>::iterator itor;
			for (itor = closeMatches.begin();
				itor != closeMatches.end();
				++itor)
			{
				std::string text = (*itor)->toString();
				console->addLine(false, S3D::formatStringBuffer(
					"  %s", text.c_str()));				
			}
		}
	}
}

ConsoleRule *ConsoleRules::matchRule(
	std::vector<ConsoleRuleValue> &values,
	std::vector<ConsoleRule *> &closeMatches)
{
	std::multimap<int, ConsoleRule *> matchedRules;

	ConsoleRuleValue firstValue = values.front();
	_strlwr((char *)firstValue.valueString.c_str());
	std::pair<RulesMap::iterator, RulesMap::iterator> itp = 
		rules_.equal_range(firstValue.valueString);
	for (RulesMap::iterator itor = itp.first; itor != itp.second; ++itor) 
	{
		ConsoleRule *rule = itor->second;
		matchedRules.insert(
			std::pair<const int, ConsoleRule *>(
				(int) rule->getParams().size(), rule));
	}

	if (matchedRules.empty()) return 0;

	std::vector<ConsoleRule *>::iterator ruleItor;
	std::vector<ConsoleRule *> sameNumberArgs;
	getMatchedRules(sameNumberArgs, matchedRules, (int) values.size() - 1);
	if (!sameNumberArgs.empty())
	{
		for (ruleItor = sameNumberArgs.begin();
			ruleItor != sameNumberArgs.end();
			++ruleItor)
		{
			ConsoleRule *rule = *ruleItor;
			closeMatches.push_back(rule);
			if (rule->matchesExactParams(values)) 
			{
				closeMatches.clear();
				return rule;
			}
		}
	}

	return 0;
}

void ConsoleRules::getMatchedRules(
	std::vector<ConsoleRule *> &result,
	std::multimap<int, ConsoleRule *> &matchedRules,
	int argCount)
{
	result.clear();
	std::pair<
		std::multimap<int, ConsoleRule *>::iterator, 
		std::multimap<int, ConsoleRule *>::iterator> itp = 
		matchedRules.equal_range(argCount);
	for (std::multimap<int, ConsoleRule *>::iterator itor = itp.first; 
		itor != itp.second; 
		++itor) 
	{
		ConsoleRule *rule = itor->second;
		result.push_back(rule);
	}
}

bool ConsoleRules::parseLine(const char *line, 
	std::vector<ConsoleRuleValue> &split)
{
	int pos = -1;
	bool inQuote = false;
	std::string currentEntry;
	for (int i=0; i<(int) strlen(line)+1; i++)
	{
		const char c = line[i];

		if (c == '\0')
		{
			if (inQuote) return false;
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;

			return true;
		}
		else if (c == '=' && 
			((split.size() == 1 &&  currentEntry.empty()) || 
			 (split.size() == 0 && !currentEntry.empty())))
		{
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;
		}
		else if ((c == ' ') && !inQuote)
		{
			parseAddLine(pos, currentEntry.c_str(), split);
			currentEntry = ""; pos = -1;
		}
		else if (c == '\"')
		{
			inQuote = !inQuote;
		}
		else
		{
			if (pos == -1) pos = i;
			currentEntry += c;
		}
	}

	return !inQuote;
}

void ConsoleRules::parseAddLine(int position, const char *line, 
	std::vector<ConsoleRuleValue> &split)
{
	int n = (int) strlen(line);
	if (n == 0) return;

	ConsoleRuleValue newSplit;
	newSplit.valueString = line;
	newSplit.position = position;

	if (strcmp(line, "on") == 0)
	{
		newSplit.type = ConsoleRuleTypeBoolean;
		newSplit.valueBool = true;
	}
	else if (strcmp(line, "off") == 0)
	{
		newSplit.type = ConsoleRuleTypeBoolean;
		newSplit.valueBool = false;
	}
	else
	{
		bool numbersOnly = true;
		for (int i=0; i<n; i++)
		{
			if ((line[i] < '0') || (line[i] > '9'))
			{
				if (line[i]!='.')
				{
					numbersOnly = false;
					break;
				}
			}
		}

		if (numbersOnly)
		{
			newSplit.type = ConsoleRuleTypeNumber;
			newSplit.valueNumber = (float) atof(line);
		}
		else
		{
			newSplit.type = ConsoleRuleTypeString;
		}
	}

	split.push_back(newSplit);
}

void ConsoleRules::dump(std::vector<std::string> &resultList)
{
	std::multimap<std::string, ConsoleRule *>::iterator itor;
	for (itor = rules_.begin();
		itor != rules_.end();
		++itor)
	{
		ConsoleRule *rule = itor->second;
		resultList.push_back(rule->toString());
	}
}
