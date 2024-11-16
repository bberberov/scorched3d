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

#include <common/ChannelTextParser.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <lang/LangResource.h>

ChannelTextParser::ChannelTextParser()
{
}

ChannelTextParser::~ChannelTextParser()
{
}

ChannelTextParser::ChannelTextEntry *ChannelTextParser::getEntry(int position)
{
	if (position >= (int) entryIndex_.size()) return 0;

	unsigned int index = entryIndex_[position];
	if (index == 0) return 0;

	ChannelTextEntry &textEntry = entries_[index - 1];
	return &textEntry;
}

void ChannelTextParser::subset(ChannelTextParser &other, int start, int len)
{
	entries_ = other.entries_;

	// Create this channel text from a subset of another channel text
	for (int i=start; i<start+len; i++)
	{
		text_.push_back(other.text_.c_str()[i]);
		entryIndex_.push_back(other.entryIndex_[i]);
	}
}

void ChannelTextParser::addIndex(int number, unsigned char index)
{
	for (int i=0; i<number; i++)
	{
		entryIndex_.push_back(index);
	}
}

void ChannelTextParser::parseText(ScorchedContext &context, const LangString &text)
{
	// Clear any existing items
	text_.clear();
	entryIndex_.clear();
	entries_.clear();

	// Parse out all of the urls
	const unsigned int *pos = text.c_str();
	const unsigned int *start = 0;

	// Find the next [
	while ((start = LangStringUtil::strstr(pos, LANG_STRING("["))))
	{
		// Add all text before [
		text_.append(pos, start - pos);
		addIndex(start - pos, 0);
		pos = start; // Skip to this point

		// Find the next ]
		const unsigned int *end = LangStringUtil::strstr(start, LANG_STRING("]"));
		if (end)
		{
			// Create the url
			LangString url;
			url.append(pos, end - pos + 1);

			// Parse the url
			ChannelTextEntry newEntry;
			if (parseUrl(context, url, newEntry))
			{
				// Update the entryIndex array to point to the newEntry
				entries_.push_back(newEntry);
				addIndex((int) newEntry.text.size(), (unsigned char) entries_.size());

				// Add the new url text
				text_.append(newEntry.text);
			}
			else
			{
				// Add the normal text
				text_.append(url);
				addIndex((int) url.size(), 0);
			}

			// Skip the url
			pos = end + 1;
		}
	}

	// Add all remaining text
	text_.append(pos);
	addIndex(LangStringUtil::strlen(pos), 0);
}

bool ChannelTextParser::parseUrl(ScorchedContext &context, 
	const LangString &url, ChannelTextEntry &entry)
{
	// Find the url seperator
	const unsigned int *colon = LangStringUtil::strstr(url.c_str(), LANG_STRING(":"));
	if (!colon) return false;

	// Strip the [] chars and the :
	LangString urlPart(url.c_str() + 1, colon - url.c_str() - 1);
	LangString otherPart(colon + 1, LangStringUtil::strlen(colon) - 2);
	entry.part = otherPart;

	// Find a url handler based on the url
	// Hardcoded for now
	if (urlPart == LANG_STRING("c")) // A channel
	{
		return createChannelEntry(context, otherPart, entry);
	}
	else if (urlPart == LANG_STRING("p")) // A player
	{
		return createPlayerEntry(context, otherPart, entry);
	}
	else if (urlPart == LANG_STRING("t")) // A tip
	{
		return createTipEntry(context, otherPart, entry);
	}
	else if (urlPart == LANG_STRING("w")) // A weapon
	{
		return createWeaponEntry(context, otherPart, entry);
	} 
	else if (urlPart == LANG_STRING("a")) // An admin
	{
		return createAdminEntry(context, otherPart, entry);
	}
	return false;
}

bool ChannelTextParser::createPlayerEntry(ScorchedContext &context, 
	const LangString &part, ChannelTextEntry &entry)
{
	entry.type = ePlayerEntry;
	entry.text.push_back('[');
	entry.text.push_back(3473);
	entry.text.append(part);
	entry.text.push_back(']');
	entry.data = 0;

	Tank *tank = context.getTargetContainer().getTankByName(part);
	if (tank)
	{
		entry.data = tank->getPlayerId();
		entry.color = tank->getColor();
	}

	return true;
}

bool ChannelTextParser::createWeaponEntry(ScorchedContext &context, 
	const LangString &part, ChannelTextEntry &entry)
{
	entry.type = eWeaponEntry;
	entry.text.push_back('[');
	entry.text.append(part);
	entry.text.push_back(']');
	entry.color = Vector(1.0f, 1.0f, 1.0f);
	std::string accessoryName(LangStringUtil::convertFromLang(part));
	Accessory *accessory = context.getAccessoryStore().
		findByPrimaryAccessoryName(accessoryName.c_str());
	entry.data = accessory?accessory->getAccessoryId():0;

	return true;
}

bool ChannelTextParser::createTipEntry(ScorchedContext &context, 
	const LangString &part, ChannelTextEntry &entry)
{
	entry.type = eTipEntry;
	entry.text = part;
	entry.color = Vector(1.0f, 1.0f, 0.0f);

	return true;
}

bool ChannelTextParser::createChannelEntry(ScorchedContext &context, 
	const LangString &part, ChannelTextEntry &entry)
{
	entry.type = eChannelEntry;
	entry.text.push_back('[');
	entry.text.append(part);
	entry.text.push_back(']');

	return true;
}

bool ChannelTextParser::createAdminEntry(ScorchedContext &context, 
	const LangString &part, ChannelTextEntry &entry)
{
	entry.type = eAdminEntry;
	entry.text.append(part);
	entry.text.push_back(' ');
	entry.text.append(LANG_RESOURCE("ADMIN_BRK", "(Admin)"));
	entry.color = Vector(1.0f, 1.0f, 1.0f);

	return true;
}
