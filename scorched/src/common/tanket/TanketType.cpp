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

#include <tanket/TanketType.h>
#include <XML/XMLParser.h>
#include <common/ToolTip.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>

TanketType::TanketType() : tooltip_(0), default_(false)
{
}

TanketType::~TanketType()
{
}

bool TanketType::initFromXML(ScorchedContext &context, XMLNode *node)
{
	node->getNamedChild("default", default_, false);
	if (!node->getNamedChild("name", name_)) return false;
	if (!node->getNamedChild("life", life_)) return false;
	if (!node->getNamedChild("power", power_)) return false;
	if (!node->getNamedChild("description", description_)) return false;

	XMLNode *accessoryNode = 0;
	while (node->getNamedChild("accessory", accessoryNode, false))
	{
		std::string name;
		int count;
		if (!accessoryNode->getNamedChild("name", name)) return false;
		if (!accessoryNode->getNamedChild("count", count)) return false;
		if (!accessoryNode->failChildren()) return false;

		Accessory *accessory = context.getAccessoryStore().
			findByPrimaryAccessoryName(name.c_str());
		if (!accessory)
		{
			return accessoryNode->returnError("Failed to find named accessory");
		}

		accessories_[accessory] = count;
	}
	while (node->getNamedChild("disableaccessory", accessoryNode, false))
	{
		std::string name;
		if (!accessoryNode->getNamedChild("name", name)) return false;
		if (!accessoryNode->failChildren()) return false;

		Accessory *accessory = context.getAccessoryStore().
			findByPrimaryAccessoryName(name.c_str());
		if (!accessory)
		{
			return accessoryNode->returnError("Failed to find named accessory");
		}

		disabledAccessories_.insert(accessory);
	}

	formTooltip();

	return node->failChildren();
}

bool TanketType::getAccessoryDisabled(Accessory *accessory)
{
	if (disabledAccessories_.empty()) return false;
	return (disabledAccessories_.find(accessory) != disabledAccessories_.end());
}

void TanketType::formTooltip()
{
	delete tooltip_;

	std::string accessoryBuffer;
	{
		if (!accessories_.empty()) accessoryBuffer.append("\n");

		unsigned int count = 0;
		std::map<Accessory *, int>::iterator itor;
		for (itor = accessories_.begin();
			itor != accessories_.end();
			++itor, count++)
		{
			Accessory *accessory = (*itor).first;
			accessoryBuffer.append("+ ").append(accessory->getName());
			if (count + 1 < accessories_.size()) accessoryBuffer.append("\n");
		}
	}
	{
		if (!disabledAccessories_.empty()) accessoryBuffer.append("\n");

		unsigned int count = 0;
		std::set<Accessory *>::iterator itor;
		for (itor = disabledAccessories_.begin();
			itor != disabledAccessories_.end();
			++itor, count++)
		{
			Accessory *accessory = (*itor);
			accessoryBuffer.append("- ").append(accessory->getName());
			if (count + 1 < disabledAccessories_.size()) accessoryBuffer.append("\n");
		}
	}

	tooltip_ = new ToolTip(
		ToolTip::ToolTipNone,
		LANG_STRING(name_),
		LANG_STRING(S3D::formatStringBuffer(
			"%s\n"
			"Life : %.0f\n"
			"Power : %.0f%s",
			description_.c_str(),
			getLife().asFloat(),
			getPower().asFloat(),
			accessoryBuffer.c_str())));
}
