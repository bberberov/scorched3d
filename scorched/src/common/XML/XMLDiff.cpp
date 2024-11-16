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

#include <XML/XMLDiff.h>
#include <XML/XMLStringBuffer.h>
#include <common/Defines.h>
#include <stdlib.h>
#include <stdio.h>

std::string XMLDiff::diffString(const std::string &first, const std::string &second)
{
	XMLStringBuffer firstParser, secondParser;
	if (!firstParser.create(first.c_str(), first.size()) ||
		!secondParser.create(second.c_str(), second.size()))
	{
		return first + "\n" + second;
	}
	else
	{
		diffNodes(*firstParser.getRootNode(), *secondParser.getRootNode());
		FileLines firstLines, secondLines;
		firstParser.getRootNode()->addNodeToFile(firstLines, 4);
		secondParser.getRootNode()->addNodeToFile(secondLines, 4);

		std::string result;
		firstLines.getAsOneLine(result);
		secondLines.getAsOneLine(result);
		return result;
	}
}

void XMLDiff::diffNodes(XMLNode &first, XMLNode &second)
{
	nodesEqual(first, second);
}

bool XMLDiff::nodesEqual(XMLNode &first, XMLNode &second)
{
	if (!nodesEqualNoChildren(first, second)) return false;

	bool allEqual = true;
	std::list<XMLNode *>::iterator firstItor;
	for (firstItor = first.getChildren().begin();
		firstItor != first.getChildren().end();
		)
	{
		bool found = false;
		XMLNode *firstChild = *firstItor;
		std::list<XMLNode *>::iterator secondItor;
		for (secondItor = second.getChildren().begin();
			secondItor != second.getChildren().end();
			++secondItor)
		{
			XMLNode *secondChild = *secondItor;
			if (nodesEqual(*firstChild, *secondChild))
			{
				second.getChildren().remove(secondChild);
				delete secondChild;
				found = true;
				break;
			}
		}
		if (!found)
		{
			allEqual = false;
		}

		++firstItor;
		if (found) 
		{
			first.getChildren().remove(firstChild);
			delete firstChild;
		}
	}

	return allEqual;
}

bool XMLDiff::nodesEqualNoChildren(XMLNode &first, XMLNode &second)
{
	// Name
	if (0 != strcmp(first.getName(), second.getName())) return false;

	// Content
	if (0 != strcmp(first.getContent(), second.getContent())) return false;

	// Parameters
	if (first.getParameters().size() != second.getParameters().size()) return false;
	std::list<XMLNode *>::iterator firstParamItor;
	for (firstParamItor = first.getParameters().begin();
		firstParamItor != first.getParameters().end();
		++firstParamItor)
	{
		bool found = false;
		XMLNode *firstParam = *firstParamItor;
		std::list<XMLNode *>::iterator secondParamItor;
		for (secondParamItor = second.getParameters().begin();
			secondParamItor != second.getParameters().end();
			++secondParamItor)
		{
			XMLNode *secondParam = *secondParamItor;
			if (0 == strcmp(secondParam->getName(), firstParam->getName()) &&
				0 == strcmp(secondParam->getContent(), firstParam->getContent()))
			{
				found = true;
				break;
			}
		}
		if (!found) return false;
	}

	return true;
}
