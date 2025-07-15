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

#include <weapons/AccessoryStore.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <lang/LangResource.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketType.h>
#include <tanket/Tanket.h>

TanketAccessories::TanketAccessories(ScorchedContext &context) :
	context_(context),
	tankWeapon_(context),
	tankAuto_(context),
	tankBatteries_(context)
{
}

TanketAccessories::~TanketAccessories()
{
	clearAccessories();
}

void TanketAccessories::setTanket(Tanket *tanket)
{
	tanket_ = tanket;
	tankWeapon_.setTanket(tanket);
	tankAuto_.setTanket(tanket);
	tankBatteries_.setTanket(tanket);
}

void TanketAccessories::newMatch()
{
	clearAccessories();

	tankWeapon_.newMatch();
	tankAuto_.newMatch();
	tankBatteries_.newMatch();

	// Add all the accessories the tank should start with
	// this is the accessories from the global accessories file
	// and also if give all accessories is set
	{
		std::list<Accessory *> accessories = 
			context_.getAccessoryStore().getAllAccessories();
		std::list<Accessory *>::iterator itor;
		for (itor = accessories.begin();
			itor != accessories.end();
			++itor)
		{
			Accessory *accessory = (*itor);
			if (accessory->getMaximumNumber() > 0)
			{
				int startingNumber = accessory->getStartingNumber();
				if (context_.getOptionsGame().getGiveAllWeapons())
				{
					startingNumber = -1;
				}

				if (startingNumber != 0)
				{
					add_(accessory, startingNumber, true);
				}
			}
		}
	}

	// Add all of the accessories that come from the tank's type
	{
		TanketType *type = tanket_->getTanketType();
		std::map<Accessory *, int> accessories = type->getAccessories();
		std::map<Accessory *, int>::iterator itor;
		for (itor = accessories.begin();
			itor != accessories.end();
			++itor)
		{
			Accessory *accessory = (*itor).first;
			int count = (*itor).second;

			add_(accessory, count, true);
		}
	}

	changed();
}

void TanketAccessories::clearAccessories()
{
	accessories_.clear();

	std::map<std::string, AccessoryList*>::iterator groupsItor;
	for (groupsItor = accessoryGroups_.begin();
		groupsItor != accessoryGroups_.end();
		++groupsItor)
	{
		delete (*groupsItor).second;
	}
	accessoryGroups_.clear();

	std::map<AccessoryPart::AccessoryType, AccessoryList*>::iterator typesItor;
	for (typesItor = accessoryTypes_.begin();
		typesItor != accessoryTypes_.end();
		++typesItor)
	{
		delete (*typesItor).second;
	}
	accessoryTypes_.clear();

}

void TanketAccessories::getAllAccessories(std::list<Accessory *> &result)
{
	std::map<Accessory *, int>::iterator itor;
	for (itor = accessories_.begin();
		itor != accessories_.end();
		++itor)
	{
		Accessory *accessory = (*itor).first;
		result.push_back(accessory);
	}
}

std::list<Accessory *> &TanketAccessories::getAllAccessoriesByType(
	AccessoryPart::AccessoryType type)
{
	std::map<AccessoryPart::AccessoryType, AccessoryList*>::iterator itor =
		accessoryTypes_.find(type);
	if (itor != accessoryTypes_.end())
	{
		return *((*itor).second);
	}

	static std::list<Accessory *> emptyList;
	return emptyList;
}

std::list<Accessory *> &TanketAccessories::getAllAccessoriesByGroup(
	const char *groupName)
{
	std::map<std::string, AccessoryList*>::iterator itor =
		accessoryGroups_.find(groupName);
	if (itor != accessoryGroups_.end())
	{
		return *((*itor).second);
	}

	static std::list<Accessory *> emptyList;
	return emptyList;
}

bool TanketAccessories::canUse(Accessory *accessory)
{
	int count = getAccessoryCount(accessory);
	if (count == 0) return false;
	if (count == -1 || count >= accessory->getUseNumber()) return true;
	return false;
}

int TanketAccessories::getAccessoryCount(Accessory *accessory)
{
	std::map<Accessory *, int>::iterator foundAccessory =
		accessories_.find(accessory);
	int currentNumber = 0;
	if (foundAccessory != accessories_.end())
	{
		currentNumber = foundAccessory->second;
	}
	return currentNumber;
}

bool TanketAccessories::accessoryAllowed(Accessory *accessory, int count)
{
	// Check if this tank type allows this accessory
	TanketType *type = tanket_->getTanketType();
	if (type->getAccessoryDisabled(accessory)) return false;

	// Check if this accessory is allowed at all
	if (accessory->getNoBuy()) return false;
	if (accessory->getMaximumNumber() == 0) return false;
	int currentCount = getAccessoryCount(accessory);
	if (currentCount + count >
		accessory->getMaximumNumber())
	{
		return false;
	}

	// Check if this accessory exceeds the current arms level
	if (10 - accessory->getArmsLevel() > 
		context_.getOptionsTransient().getArmsLevel())
	{
		return false;
	}

	// Check if an infinite weapon is being bought twice
	if (currentCount == -1)
	{
		return false;
	}

	// Check if this is an ai only weapon
	if (accessory->getAIOnly())
	{
		return false;
	}
	
	// Check if this is a bot only weapon
	if (accessory->getBotOnly())
	{
		if (tanket_->getDestinationId() != 0)
		{
	        return false;
		}
	}

	return true;
}

void TanketAccessories::add(Accessory *accessory, int count, bool check)
{
	add_(accessory, count, check);
	changed();
}

void TanketAccessories::add_(Accessory *accessory, int count, bool check)
{
	if (check)
	{
		// Check if this tank is allowed this accessory
		if (!accessoryAllowed(accessory, count)) return;
	}
	else
	{
		// Check if an infinite weapon is being bought twice
		if (getAccessoryCount(accessory) == -1)
		{
			return;
		}
	}

	// Add this accessory
	std::map<Accessory *, int>::iterator itor = accessories_.find(accessory);
	if (itor == accessories_.end() || count < 0)
	{
		accessories_[accessory] = count;
	}
	else
	{
		accessories_[accessory] += count;
	}

	// Add to groups
	{
		std::map<std::string, AccessoryList*>::iterator groupItor =
			accessoryGroups_.find(accessory->getGroupName());
		if (groupItor != accessoryGroups_.end())
		{
			(*groupItor).second->remove(accessory);
			(*groupItor).second->push_back(accessory);
		}
		else
		{
			AccessoryList *newList = new AccessoryList();
			accessoryGroups_[accessory->getGroupName()] = newList;
			newList->push_back(accessory);
		}
	}

	// Add to types
	{
		std::map<AccessoryPart::AccessoryType, AccessoryList*>::iterator typeItor =
			accessoryTypes_.find(accessory->getType());
		if (typeItor != accessoryTypes_.end())
		{
			(*typeItor).second->remove(accessory);
			(*typeItor).second->push_back(accessory);
		}
		else
		{
			AccessoryList *newList = new AccessoryList();
			accessoryTypes_[accessory->getType()] = newList;
			newList->push_back(accessory);
		}
	}
}

void TanketAccessories::rm(Accessory *accessory, int count)
{
	std::map<Accessory *, int>::iterator itor = 
		accessories_.find(accessory);
	if (itor != accessories_.end())
	{
		if (accessories_[accessory] > 0)
		{
			accessories_[accessory] -= count;
			if (accessories_[accessory] <= 0)
			{
				accessories_.erase(accessory);

				{ // Remove from groups
					std::map<std::string, AccessoryList*>::iterator groupItor =
						accessoryGroups_.find(accessory->getGroupName());
					if (groupItor != accessoryGroups_.end())
					{
						(*groupItor).second->remove(accessory);
					}
				}
				{ // Remove from types
					std::map<AccessoryPart::AccessoryType, AccessoryList*>::iterator typeItor =
						accessoryTypes_.find(accessory->getType());
					if (typeItor != accessoryTypes_.end())
					{
						(*typeItor).second->remove(accessory);
					}
				}
			}
		}
	}

	changed();
}

LangString TanketAccessories::getAccessoryCountString(Accessory *accessory)
{
	int count = getAccessoryCount(accessory);
	LangString buffer;
	if (count >= 0)
	{
		buffer.append(LANG_STRING(S3D::formatStringBuffer("%i", count)));
	}
	else
	{
		LANG_RESOURCE_VAR(INF, "INF", "In");
		buffer.append(INF);
	}

	return buffer;
}

LangString TanketAccessories::getAccessoryAndCountString(Accessory *accessory)
{
	LangString buffer;
	buffer.append(LANG_RESOURCE(accessory->getName(), accessory->getName())).
		append(LANG_STRING(" ("));
	buffer.append(getAccessoryCountString(accessory));
	buffer.append(LANG_STRING(")"));

	return buffer;
}

void TanketAccessories::changed()
{
	// Tell the appropriate container that the count has changed
	tankAuto_.changed();
	tankWeapon_.changed();
	tankBatteries_.changed();
}

bool TanketAccessories::writeMessage(NamedNetBuffer &buffer, bool writeAccessories)
{
	NamedNetBufferSection section(buffer, "TanketAccessories");

	// Send the fact we are not sending any accessories
	if (!writeAccessories)
	{
		buffer.addToBufferNamed("noAccessories", (int) -1);
		return true;
	}

	// Write accessories in partid order
	std::map<unsigned int, int> ordered;
	std::map<unsigned int, int>::iterator itor2;
	std::map<Accessory *, int>::iterator itor;
	for (itor = accessories_.begin();
		itor != accessories_.end();
		++itor)
	{
		ordered[itor->first->getAccessoryId()] = itor->second;
	}

	// Add all the accessories that are sent to the client
	buffer.addToBufferNamed("noAccessories", (int) ordered.size());
	for (itor2 = ordered.begin();
		itor2 != ordered.end();
		++itor2)
	{
		buffer.addToBufferNamed("accessory", itor2->first);
		buffer.addToBufferNamed("count", itor2->second);

		if (context_.getOptionsGame().getAccessoryNameSyncCheck())
		{
			Accessory *accessory = context_.getAccessoryStore().findByAccessoryId(itor2->first);
			buffer.addToBufferNamed("name", accessory->getName());
		}
	}

	return true;
}

bool TanketAccessories::readMessage(NetBufferReader &reader)
{
	int totalAccessories = 0;
	if (!reader.getFromBuffer(totalAccessories)) return false;

	// Check if we've been sent any accessories, if not return
	if (totalAccessories == -1) 
	{
		return true;
	}

	// Clear all the current accessories
	clearAccessories();

	// Check all the servers accessories exist
	for (int w=0; w<totalAccessories; w++)
	{
		unsigned int accessoryId = 0;
		int accessoryCount = 0;
		if (!reader.getFromBuffer(accessoryId)) return false;
		if (!reader.getFromBuffer(accessoryCount)) return false;

		if (context_.getOptionsGame().getAccessoryNameSyncCheck())
		{
			std::string accessoryName;
			if (!reader.getFromBuffer(accessoryName)) return false;
		}

		Accessory *accessory = 
			context_.getAccessoryStore().findByAccessoryId(accessoryId);
		if (!accessory)
		{
			return false;
		}

		add_(accessory, accessoryCount, false);
	}

	changed();
	return true;
}
