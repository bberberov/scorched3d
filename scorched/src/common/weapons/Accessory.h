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

#if !defined(AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
#define AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_

#include <common/ToolTip.h>
#include <common/ModelID.h>
#include <XML/XMLFile.h>
#include <weapons/AccessoryPart.h>
#include <weapons/AccessoryCreateContext.h>
#include <lang/LangString.h>
#include <string>
#include <map>

#ifndef S3D_SERVER
#include <GLEXT/GLTextureReference.h>
#endif

class Tank;
class MissileMesh;
class Accessory  
{
public:
	Accessory();
	virtual ~Accessory();

	enum PositionSelectType
	{
		ePositionSelectNone = 0,
		ePositionSelectFuel = 1,
		ePositionSelectGeneric = 2,
		ePositionSelectLimit = 3,
		ePositionSelectFuelLimit = 4
	};

	bool parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode);

	const char *getActivationSound();
	const char *getName() { return name_.c_str(); }
	LangString &getStringName();
	const char *getDescription() { return description_.c_str(); }
	int getPrice() { return price_; }
	int getSellPrice() { return sellPrice_; }
	int getOriginalSellPrice() { return originalSellPrice_; }
	int getOriginalPrice() { return originalPrice_; }
	int getFreeMarketLimits() { return freemarketLimits_; }
	int getBundle() { return bundle_; }
	int getArmsLevel() { return armsLevel_; }
	int getMaximumNumber() { return maximumNumber_; }
	int getStartingNumber() { return startingNumber_; }
	int getUseNumber() { return useNumber_; }
	bool getAIOnly() { return aiOnly_; }
	bool getBotOnly() { return botOnly_; }
	bool getNoBuy() { return noBuy_; }
	PositionSelectType getPositionSelect() { return positionSelect_; }
	int getPositionSelectLimit() { return positionSelectLimit_; }

	ToolTip &getToolTip() { return toolTip_; }
	const char *getIconName() { return iconName_.c_str(); }
	const char *getGroupName() { return groupName_.c_str(); }
	const char *getTabGroupName() { return tabGroupName_.c_str(); }
	AccessoryPart *getAction() { return accessoryAction_; }
	fixed getModelScale() { return modelScale_; }
	ModelID &getModel() { return modelId_; }
	bool getMuzzleFlash() { return muzzleFlash_; }

	AccessoryPart::AccessoryType getType() { return accessoryAction_->getType(); }

	void setPrice(int p) { if (p>0) price_ = p; }
	void setSellPrice(int p) { if (p>0) sellPrice_ = p; }

	static void resetAccessoryIds() { nextAccessoryId_ = 0; }
	unsigned int getAccessoryId() { return accessoryId_; }

#ifndef S3D_SERVER
	GLTextureReference &getTexture() { return texture_; }
	static MissileMesh *getWeaponMesh(ModelID &id, Tank *currentPlayer);
	static std::map<std::string, MissileMesh *> loadedMeshes_;
	GLTextureReference texture_;
#endif

protected:
	static unsigned int nextAccessoryId_;
	unsigned int accessoryId_;
	bool aiOnly_;
	bool botOnly_;
	bool noBuy_;
	AccessoryPart *accessoryAction_;
	PositionSelectType positionSelect_;
	ToolTip toolTip_;
	ModelID modelId_;
	LangString stringName_;
	std::string iconName_;
	std::string groupName_, tabGroupName_;
	std::string name_;
	std::string description_;
	std::string activationSound_;
	int positionSelectLimit_;
	int price_;
	int originalPrice_;
	int bundle_;
	int armsLevel_;
	int sellPrice_;
	int originalSellPrice_;
	int freemarketLimits_;
	int maximumNumber_;
	int useNumber_;
	int startingNumber_;
	fixed modelScale_;
	bool muzzleFlash_;
};

#endif // !defined(AFX_ACCESSORY_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
