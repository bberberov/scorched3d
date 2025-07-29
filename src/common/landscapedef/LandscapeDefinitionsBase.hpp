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

#if !defined(__INCLUDE_LandscapeDefinitionsBaseh_INCLUDE__)
#define __INCLUDE_LandscapeDefinitionsBaseh_INCLUDE__

#include <common/OptionsScorched.h>
#include <XML/XMLFile.h>
#include <string>
#include <vector>
#include <list>
#include <map>

class LandscapeDefinitionsEntry
{
public:
	std::string name; 
	std::vector<std::string> texs; 
	std::vector<std::string> defns; 
	float weight; // The posibility this defn will be choosen
	std::string description;  // Description of this landscape definition type
	std::string picture; // Visible view of this landscape definition type

	virtual bool readXML(XMLNode *node);
};

class LandscapeDefinitionsBase
{
public:
	LandscapeDefinitionsBase();
	virtual ~LandscapeDefinitionsBase();

	virtual bool readLandscapeDefinitions();
	virtual void clearLandscapeDefinitions();

	bool landscapeEnabled(OptionsGame &context, const char *name);
	LandscapeDefinitionsEntry *getLandscapeByName(const char *name);
	std::list<LandscapeDefinitionsEntry> &getAllLandscapes() 
		{ return entries_; }

protected:
	std::list<LandscapeDefinitionsEntry> entries_;
};

#endif // __INCLUDE_LandscapeDefinitionsBaseh_INCLUDE__
