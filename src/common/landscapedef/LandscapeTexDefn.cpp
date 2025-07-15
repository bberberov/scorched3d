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

#include <landscapedef/LandscapeTexDefn.h>
#include <landscapedef/LandscapeDefinitions.h>

LandscapeTexDefn::LandscapeTexDefn()
{
}

LandscapeTexDefn::~LandscapeTexDefn()
{
}

bool LandscapeTexDefn::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	if (!readXML("events", "event", definitions, node)) return false;
	if (!readXML("placements", "placement", definitions, node)) return false;
	if (!readXML("ambientsounds", "ambientsound", definitions, node)) return false;
	if (!readXML("includes", "include", definitions, node)) return false;
	if (!readXML("boids", "boid", definitions, node)) return false;
	if (!readXML("shipgroups", "shipgroup", definitions, node)) return false;
	return true;
}

bool LandscapeTexDefn::readXML(const char *names, const char *name,
	LandscapeDefinitions *definitions, XMLNode *node)
{
	{
		XMLNode *includesNode;
		if (node->getNamedChild(names, includesNode, false))
		{
			std::string include;
			while (includesNode->getNamedChild(name, include, false))
			{
				LandscapeInclude *landscapeInclude = 
					definitions->getInclude(include.c_str(), true);
				if (!landscapeInclude) return false;
				includes.push_back(landscapeInclude);
			}
			if (!includesNode->failChildren()) return false;
		}
	}
	return true;
}
