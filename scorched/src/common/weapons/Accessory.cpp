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

#include <common/ToolTip.h>
#include <common/ModelID.h>
#include <weapons/Accessory.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <lang/LangResource.h>
#include <tank/Tank.h>
#include <tank/TankModel.h>
#include <tank/TankModelContainer.h>
#include <stdlib.h>

unsigned int Accessory::nextAccessoryId_ = 0;

#ifndef S3D_SERVER
#include <3dsparse/Model.h>
#include <3dsparse/ModelStore.h>
#include <tankgraph/MissileMesh.h>
#include <image/ImageFactory.h>
#endif

Accessory::Accessory() :
	accessoryId_(++nextAccessoryId_),
	name_("NONAME"), description_("NODESC"), 
	accessoryAction_(0),
	toolTip_(ToolTip::ToolTipHelp, LangString(), LangString()),
	price_(0), bundle_(1), armsLevel_(9), freemarketLimits_(150),
	modelScale_(1),
	positionSelect_(ePositionSelectNone), positionSelectLimit_(10),
	maximumNumber_(0),
	startingNumber_(0),
	useNumber_(1),
	muzzleFlash_(true),
	aiOnly_(false),
	botOnly_(false),
	noBuy_(false)
{
}

Accessory::~Accessory()
{
	delete accessoryAction_;
	accessoryAction_ = 0;
}

bool Accessory::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	// Get the accessory name
	if (!accessoryNode->getNamedChild("name", name_)) return false;

	// Get the accessory armslevel
	accessoryNode->getNamedChild("armslevel", armsLevel_, false);

	// Get the optional muzzleflash
	XMLNode *muzzleFlashNode = 0;
	accessoryNode->getNamedChild("nomuzzleflash", muzzleFlashNode, false);
	if (muzzleFlashNode) muzzleFlash_ = false;

	// Get the accessory description
	accessoryNode->getNamedChild("description", description_, false);
	toolTip_.setText(ToolTip::ToolTipHelp, LANG_STRING(getName()), LANG_STRING(getDescription()));

	// Get the accessory icon
	if (accessoryNode->getNamedChild("icon", iconName_, false))
	{
		if (!S3D::checkDataFile(S3D::formatStringBuffer("data/textures/wicons/%s", getIconName()))) return false;
	}

#ifndef S3D_SERVER
	if (getIconName()[0])
	{
		texture_.setImageID(ImageID(
			S3D::eModLocation,
			"",
			S3D::formatStringBuffer("data/textures/wicons/%s", getIconName())));
	}
	else
	{
		texture_.setImageID(ImageID(
			S3D::eModLocation,
			"",
			S3D::formatStringBuffer("data/textures/wicons/%s", "tracer.bmp")));
	}
#endif

	// Get the accessory sound 
	if (accessoryNode->getNamedChild("activationsound", activationSound_, false))
	{
		if (!S3D::checkDataFile(S3D::formatStringBuffer("data/wav/%s", getActivationSound()))) return false;
	}

	// Get the accessory bundle
	accessoryNode->getNamedChild("bundlesize", bundle_, false);

	// Get ai only
	accessoryNode->getNamedChild("aionly", aiOnly_, false);

	// Get bot only (ie: only for bots, not for tank objects)
	accessoryNode->getNamedChild("botonly", botOnly_, false);

	// Get no buy 
	accessoryNode->getNamedChild("nobuy", noBuy_, false);

	// Get the maximum number
	maximumNumber_ = context.getOptionsGame().getMaxNumberWeapons();
	accessoryNode->getNamedChild("maximumnumber", maximumNumber_, false);

	// Get the starting number
	accessoryNode->getNamedChild("startingnumber", startingNumber_, false);

	// Get the number to use of firing
	accessoryNode->getNamedChild("usenumber", useNumber_, false);

	// Freemarket limits
	accessoryNode->getNamedChild("freemarketlimits", freemarketLimits_, false);

	// Get the accessory cost
	accessoryNode->getNamedChild("cost", price_, false);

	// Get the weapon model scale
	accessoryNode->getNamedChild("modelscale", modelScale_, false);

	// Get the weapon model
	XMLNode *modelNode = 0;
	if (accessoryNode->getNamedChild("model", modelNode, false))
	{
		if (!modelId_.initFromNode(modelNode)) return false;
	}

	// Get action
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("accessoryaction", subNode)) return false;
	accessoryAction_ = context.getAccessoryStore().createAccessoryPart(context, this, subNode);
	if (!accessoryAction_)
	{
		S3D::dialogMessage("Accessory", S3D::formatStringBuffer(
			"Failed to create action \"%s\"", name_.c_str()));
		return false;
	}

	// Setup price
	sellPrice_ = 0;
	if (price_ > 0 && bundle_ > 0) sellPrice_ = int((price_ / bundle_) * 0.8f);
	originalPrice_ = price_;
	originalSellPrice_ = sellPrice_;

	// Position Selection Type
	std::string positionSelection;
	if (accessoryNode->getNamedChild("positionselection", positionSelection, false))
	{
		if (0 == strcmp(positionSelection.c_str(), "none"))
		{
			positionSelect_ = ePositionSelectNone;
		}
		else if (0 == strcmp(positionSelection.c_str(), "generic"))
		{
			positionSelect_ = ePositionSelectGeneric;
		}
		else if (0 == strcmp(positionSelection.c_str(), "fuel"))
		{
			positionSelect_ = ePositionSelectFuel;

			// Make sure there is a "WeaponMoveTank" under here somewhere
			if (!context.getAccessoryStore().findAccessoryPartByAccessoryId(
				getAccessoryId(), "WeaponMoveTank"))
			{
				return accessoryNode->returnError(
					"Fuel selection can only be used with WeaponMoveTank weapons");
			}
		}
		else if (0 == strcmp(positionSelection.c_str(), "fuellimit"))
		{
			positionSelect_ = ePositionSelectFuelLimit;
			if (!accessoryNode->getNamedChild("positionselectionlimit", positionSelectLimit_)) return false;
		}
		else if (0 == strcmp(positionSelection.c_str(), "limit"))
		{
			positionSelect_ = ePositionSelectLimit;
			if (!accessoryNode->getNamedChild("positionselectionlimit", positionSelectLimit_)) return false;
		}
		else
		{
			return accessoryNode->returnError(S3D::formatStringBuffer(
				"Unknown accessory position selection type \"%s\"", 
				positionSelection.c_str()));
		}
	}

	// Get the accessory tabgroupname
	if (!accessoryNode->getNamedChild("tabgroup", tabGroupName_, false))
	{
		if (accessoryAction_->getType() == AccessoryPart::AccessoryWeapon)
		{
			tabGroupName_ = "weapon";
		}
		else
		{
			tabGroupName_ = "defense";
		}
	}

	// Get the accessory groupname
	if (!accessoryNode->getNamedChild("group", groupName_, false))
	{
		switch (accessoryAction_->getType())
		{
		case AccessoryPart::AccessoryWeapon:
			groupName_ = "weapon";
			break;
		case AccessoryPart::AccessoryParachute:
			groupName_ = "parachute";
			break;
		case AccessoryPart::AccessoryShield:
			groupName_ = "shield";
			break;
		case AccessoryPart::AccessoryAutoDefense:
			groupName_ = "autodefense";
			break;
		case AccessoryPart::AccessoryBattery:
			groupName_ = "battery";
			break;
		default:
			groupName_ = "none";
			break;
		}
	}

	return true;
}

LangString &Accessory::getStringName()
{
	if (stringName_.size() == 0)
	{
		stringName_ = LANG_RESOURCE(getName(), getName());
	}
	return stringName_;
}

const char *Accessory::getActivationSound()
{
	if (!activationSound_.c_str()[0]) return 0;
	return activationSound_.c_str();
}

#ifndef S3D_SERVER

std::map<std::string, MissileMesh *> Accessory::loadedMeshes_;

MissileMesh *Accessory::getWeaponMesh(ModelID &id, Tank *currentPlayer)
{
	// Set the default model to use if neither the tank
	// or weapon have one
	static ModelID defaultModelId;
	if (!defaultModelId.modelValid())
	{
		defaultModelId.initFromString(
			"MilkShape",
			"data/accessories/v2missile/v2missile.txt",
			"");
	}

	// Set the model to use as the default model id
	ModelID *usedModelId = &defaultModelId;

	// Get the model to use from the weapon (if there is one)
	if (id.modelValid())
	{
		usedModelId = &id;
	}
	else
	{
		// The weapon does not have a model defined for it
		// check the player to see if they have a default model
		if (currentPlayer)
		{
			TankModel *model = currentPlayer->getModelContainer().getTankModel();
			if (model && 
				model->getProjectileModelID().modelValid())
			{
				usedModelId = &model->getProjectileModelID();
			}
		}
	}

	// Load or find the correct missile mesh
	MissileMesh *mesh = 0;
	const char *name = usedModelId->getStringHash();
	std::map<std::string, MissileMesh *>::iterator itor =
		loadedMeshes_.find(name);
	if (itor == loadedMeshes_.end())
	{
		mesh = new MissileMesh(*usedModelId);
		loadedMeshes_[name] = mesh;
	}
	else
	{
		// Find
		mesh = (*itor).second;
	}
	return mesh;
}
#endif // S3D_SERVER
