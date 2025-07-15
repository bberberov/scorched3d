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

#include <landscapedef/LandscapeDefinitions.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeInclude.h>
#include <target/TargetContainer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

unsigned int LandscapeDefinitions::lastDefinitionNumber_(0);

LandscapeDefinitions::LandscapeDefinitions() :
	lastDefinition_(0),
	texs_("Tex"),
	defns_("Defns"),
	include_("Include")
{
}

LandscapeDefinitions::~LandscapeDefinitions()
{
	clearLandscapeDefinitions();
}

void LandscapeDefinitions::clearLandscapeDefinitions()
{
	LandscapeDefinitionsBase::clearLandscapeDefinitions();

	defns_.clearItems();
	texs_.clearItems();
	include_.clearItems();
}

LandscapeTex *LandscapeDefinitions::getTex(const char *file, bool load)
{
	return texs_.getItem(this, file, load);
}

LandscapeInclude *LandscapeDefinitions::getInclude(const char *file, bool load)
{
	return include_.getItem(this, file, load);
}

LandscapeDefn *LandscapeDefinitions::getDefn(const char *file, bool load)
{
	return defns_.getItem(this, file, load);
}

bool LandscapeDefinitions::readLandscapeDefinitionsEntry(LandscapeDefinitionsEntry &entry)
{
	std::vector<std::string>::iterator itor2;

	std::vector<std::string> &defns = entry.defns;
	for (itor2 = defns.begin();
		itor2 != defns.end();
		++itor2)
	{
		const char *landscapeDefnFile = (*itor2).c_str();
		LandscapeDefn *landscapeDefn = getDefn(landscapeDefnFile, true);
		if (!landscapeDefn) return false;
	}

	std::vector<std::string> &texs = entry.texs;
	for (itor2 = texs.begin();
		itor2 != texs.end();
		++itor2)
	{
		const char *landscapeTexFile = (*itor2).c_str();
		LandscapeTex *landscapeTex = getTex(landscapeTexFile, true);
		if (!landscapeTex) return false;
	}
	return true;
}

bool LandscapeDefinitions::readLandscapeDefinitions()
{
	// Clear existing landscapes
	clearLandscapeDefinitions();

	// Parse base landscape information
	if (!LandscapeDefinitionsBase::readLandscapeDefinitions()) return false;

	// Now check that the landscape tex and defn files parse correctly
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		LandscapeDefinitionsEntry &entry = (*itor);
		if (!readLandscapeDefinitionsEntry(entry)) return false;
	}

	// Read blank landscape definiton
	{
		XMLFile file;
		if (!file.readFile(S3D::getModFile("data/landscapeblank.xml")) ||
			!file.getRootNode())
		{
			S3D::dialogMessage("Scorched Landscape", S3D::formatStringBuffer(
						  "Failed to parse \"data/landscapeblank.xml\"\n%s", 
						  file.getParserError()));
			return false;
		}
		if (!blankDefinition_.readXML(file.getRootNode())) return false;
		if (!readLandscapeDefinitionsEntry(blankDefinition_)) return false;
	}

	return true;
}

const std::string LandscapeDefinitions::getLeastUsedFile(
	OptionsScorched &context, std::vector<std::string> &files)
{
	DIALOG_ASSERT(!files.empty());

	std::vector<std::string> results;
	int usedTimes = INT_MAX;

	std::vector<std::string>::iterator itor;
	for (itor = files.begin();
		itor != files.end();
		++itor)
	{
		std::string &file = (*itor);

		int used = 0;
		std::map<std::string, int>::iterator findItor = 
			usedFiles_.find(file);
		if (findItor != usedFiles_.end())
		{
			used = (*findItor).second;
		}

		if (used <= usedTimes)
		{
			if (used < usedTimes) results.clear();
			usedTimes = used;
			results.push_back(file);
		}
	}
	
	DIALOG_ASSERT(!results.empty());
	std::string result = results[0];
	if (!context.getCycleMaps())
	{
		result = results[rand() % results.size()];
	}

	usedFiles_[result] = usedTimes + 1;
	return result;
}

void LandscapeDefinitions::checkEnabled(OptionsScorched &context)
{
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		LandscapeDefinitionsEntry &result = *itor;
		if (landscapeEnabled(context.getMainOptions(), result.name.c_str()))
		{
			return;
		}
	}

	context.getChangedOptions().getLandscapesEntry().setValue("");
	Logger::log(S3D::formatStringBuffer(
		"Warning: No existing landscapes are enabled (Landscapes : %s)",
		context.getLandscapes()));
}

LandscapeDefinition LandscapeDefinitions::getBlankLandscapeDefn()
{
	LandscapeDefinitionsEntry *result = &blankDefinition_;

	// Return the chosen definition
	std::string tex = result->texs[rand() % result->texs.size()];
	std::string defn = result->defns[rand() % result->defns.size()];
	unsigned int seed = (unsigned int) rand();

	LandscapeDefinition entry(
		tex.c_str(), defn.c_str(), seed, result->name.c_str(), 
		++lastDefinitionNumber_);
	return entry;
}

LandscapeDefinition LandscapeDefinitions::getRandomLandscapeDefn(
	OptionsScorched &context, TargetContainer &TargetContainer)
{
	// Build a list of the maps that are enabled
	int players = TargetContainer.getNoOfNonSpectatorTanks();
	std::list<LandscapeDefinitionsEntry *> allPassedLandscapes;
	std::list<LandscapeDefinitionsEntry *> minMaxPassedLandscapes;
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		LandscapeDefinitionsEntry &current = *itor;
		if (landscapeEnabled(context.getMainOptions(), current.name.c_str()))
		{
			allPassedLandscapes.push_back(&current);

			// Check that min/max players are ok
			std::vector<std::string>::iterator defnitor;
			for (defnitor = current.defns.begin();
				defnitor != current.defns.end();
				++defnitor)
			{
				LandscapeDefn *defn = getDefn(defnitor->c_str());
				if (players >= defn->getMinPlayers() && players <= defn->getMaxPlayers())
				{
					minMaxPassedLandscapes.push_back(&current);
					break;
				}
			}
		}
	}

	// Check we have a least one map
	DIALOG_ASSERT(!allPassedLandscapes.empty());

	// Try min max maps
	LandscapeDefinitionsEntry *result = getRandomLandscapeDefnEntry(
		context, minMaxPassedLandscapes);
	if (!result)
	{
		// Try all maps
		Logger::log("Warning: Cannot find any landscapes for number of players");
		result = getRandomLandscapeDefnEntry(context, allPassedLandscapes);
	}

	// Check we found map
	if (!result)
	{
		S3D::dialogExit("Scorched3D",
			"Failed to select a landscape definition");
	}

	// Return the chosen definition
	std::string tex = getLeastUsedFile(context, result->texs);
	std::string defn = getLeastUsedFile(context, result->defns);
	unsigned int seed = (unsigned int) rand();
	LandscapeTex *landscapeTex = getTex(tex.c_str());
	if (landscapeTex->seed != 0) seed = landscapeTex->seed;

	LandscapeDefinition entry(
		tex.c_str(), defn.c_str(), seed, result->name.c_str(),
		++lastDefinitionNumber_);
	return entry;
}


LandscapeDefinitionsEntry *LandscapeDefinitions::getRandomLandscapeDefnEntry(
	OptionsScorched &context,
	std::list<LandscapeDefinitionsEntry *> passedLandscapes)
{
	// Map cycle mode
	LandscapeDefinitionsEntry *result = 0;
	if (context.getCycleMaps())
	{
		// Just cycle through the maps
		bool next = false;
		result = passedLandscapes.front();
		std::list<LandscapeDefinitionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDefinitionsEntry *current = *passedItor;
			if (next) 
			{
				result = current;
				break;
			}
			if (current == lastDefinition_) next = true;
		}
		lastDefinition_ = result;
	}
	else
	{
		float totalWeight = 0.0f;
		std::list<LandscapeDefinitionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDefinitionsEntry *current = *passedItor;
			totalWeight += current->weight;
		}

		// Choose a map based on probablity
		float pos = RAND * totalWeight;
		float soFar = 0.0f;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			++passedItor)
		{
			LandscapeDefinitionsEntry *current = *passedItor;
			soFar += current->weight;

			if (pos <= soFar)
			{
				result = current;
				break;
			}
		}
	}

	return result;
}

