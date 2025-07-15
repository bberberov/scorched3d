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

#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <engine/ScorchedContext.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapedef/LandscapeOptions.h>
#include <landscapedef/LandscapeInclude.h>
#include <net/NetBufferPool.h>

OptionsScorched::OptionsScorched()
{
}

OptionsScorched::~OptionsScorched()
{
}

void OptionsScorched::updateLevelOptions(ScorchedContext &context, LandscapeDefinition &defn)
{
	// Get the current level data
	LandscapeTex *ltex = context.getLandscapes().getTex(defn.getTex());
	LandscapeDefn *ldefn = context.getLandscapes().getDefn(defn.getDefn());

	// Get all of the options specified in the current level
	std::map<std::string, OptionEntry *> values;
	updateLevelOptions(ltex->texDefn.includes, values);
	updateLevelOptions(ldefn->texDefn.includes, values);

	// Iterate over the level and current options
	std::list<OptionEntry *> &levelOptions = getLevelOptions().getOptions();
	std::list<OptionEntry *> &mainoptions = getMainOptions().getOptions();
	std::list<OptionEntry *>::iterator levelitor;
	std::list<OptionEntry *>::iterator mainitor;
	for (levelitor = levelOptions.begin(), mainitor = mainoptions.begin();
		levelitor != levelOptions.end() && mainitor != mainoptions.end();
		++levelitor, ++mainitor)
	{
		OptionEntry *mainEntry = (*mainitor);
		OptionEntry *levelEntry = (*levelitor);

		// Get the current settings value that is in use
		OptionEntry *currentEntry = mainEntry;
		if (levelEntry->isChangedValue()) currentEntry = levelEntry;
		std::string oldValue = currentEntry->getValueAsString();

		// Reset the level entry
		levelEntry->setNotChanged();

		// If this level entry has changed set its new value
		std::map<std::string, OptionEntry *>::iterator findItor = 
			values.find(mainEntry->getName());
		if (findItor != values.end())
		{
			levelEntry->setValueFromString((*findItor).second->getValueAsString());
		}

		// Find out the new settings value that is in use
		currentEntry = mainEntry;
		if (levelEntry->isChangedValue()) currentEntry = levelEntry;
		std::string newValue = currentEntry->getValueAsString();

		// Log if the value has changed
		if (0 != strcmp(newValue.c_str(), oldValue.c_str()))
		{
			Logger::log(S3D::formatStringBuffer("Level option %s has been changed from %s to %s",
				mainEntry->getName(),
				oldValue.c_str(), newValue.c_str()));
		}
	}
}

void OptionsScorched::updateLevelOptions(std::vector<LandscapeInclude *> &options,
	std::map<std::string, OptionEntry *> &values)
{
	// For each include
	std::vector<LandscapeInclude *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		LandscapeInclude *option = (*itor);

		// For each set of options
		std::vector<LandscapeOptionsType *>::iterator typeItor;
		for (typeItor = option->options.begin();
			typeItor != option->options.end();
			++typeItor)
		{
			LandscapeOptionsType *optionType = (*typeItor);

			// For each option
			std::list<OptionEntry *>::iterator srcitor;
			for (srcitor = optionType->options.getOptions().begin();
				srcitor != optionType->options.getOptions().end();
				++srcitor)
			{
				OptionEntry *srcEntry = (*srcitor);
				if (srcEntry->isChangedValue())
				{
					values[srcEntry->getName()] = srcEntry;
				}
			}
		}
	}
}

void OptionsScorched::updateChangeSet()
{
	NetBuffer *defaultBuffer = NetBufferPool::instance()->getFromPool();

	defaultBuffer->reset();
	mainOptions_.writeToBuffer(*defaultBuffer, true, true);
	NetBufferReader reader(*defaultBuffer);
	changedOptions_.readFromBuffer(reader, true, true);

	NetBufferPool::instance()->addToPool(defaultBuffer);
}

bool OptionsScorched::commitChanges()
{
	bool different = false;

	// Compare buffers
	std::list<OptionEntry *> &options = mainOptions_.getOptions();
	std::list<OptionEntry *> &otheroptions = changedOptions_.getOptions();
	std::list<OptionEntry *>::iterator itor;
	std::list<OptionEntry *>::iterator otheritor;
	for (itor=options.begin(), otheritor=otheroptions.begin();
		itor!=options.end() && otheritor!=otheroptions.end();
		++itor, ++otheritor)
	{
		OptionEntry *entry = *itor;
		OptionEntry *otherentry = *otheritor;

		DIALOG_ASSERT(0 == strcmp(entry->getName(), otherentry->getName()));

		std::string str = entry->getValueAsString();
		std::string otherstr = otherentry->getValueAsString();
		if (str != otherstr)
		{
			if (!(entry->getData() & OptionEntry::DataProtected) &&
				!(otherentry->getData() & OptionEntry::DataProtected))
			{
				if (strlen(str.c_str()) < 20 && strlen(otherstr.c_str()) < 20)
				{
					Logger::log(S3D::formatStringBuffer("Option %s has been changed from %s to %s",
						entry->getName(), str.c_str(), otherstr.c_str()));
				}
				else
				{
					Logger::log(S3D::formatStringBuffer("Option %s has been changed.",
						entry->getName()));
				}
			}

			different = true;
			entry->setValueFromString(otherentry->getValueAsString());
		}
	}

	return different;
}
