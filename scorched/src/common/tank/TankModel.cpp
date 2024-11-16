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

#include <tank/TankModel.h>
#include <tank/Tank.h>
#include <tanket/TanketType.h>
#include <XML/XMLNode.h>
#include <engine/ScorchedContext.h>

TankModel::TankModel() :
	aiOnly_(false), 
	movementSmoke_(true)
{
	catagories_.insert("All");
}

TankModel::~TankModel()
{
}

bool TankModel::initFromXML(ScorchedContext &context, XMLNode *node)
{
	// Get the name of tank
	if (!node->getNamedChild("name", tankName_)) return false;

	// Get the tank type (if any)
	std::string typeName;
	while (node->getNamedChild("type", typeName, false))
	{
		tankTypes_.insert(typeName);
	}

	// Get the model node
	XMLNode *modelNode;
	if (!node->getNamedChild("model", modelNode)) return false;

	// Parse the modelId which tells us which files and
	// 3d type the model actuall is
	// The model files are not parsed until later
	if (!modelId_.initFromNode(modelNode))
	{
		return modelNode->returnError(
			S3D::formatStringBuffer("Failed to load mesh for tank \"%s\"",
			tankName_.c_str()));
	}

	// Get the projectile model node (if any)
	XMLNode *projectileModelNode;
	if (node->getNamedChild("projectilemodel", projectileModelNode, false))
	{
		if (!projectileModelId_.initFromNode(projectileModelNode))
		{
			return projectileModelNode->returnError(
				S3D::formatStringBuffer("Failed to load projectile mesh for tank \"%s\"",
				tankName_.c_str()));
		}
	}

	// Get the tracks node (if any)
	if (!loadImage(node, "tracksv", tracksVId_, "data/tanks/tracksv.bmp")) return false;
	if (!loadImage(node, "tracksh", tracksHId_, "data/tanks/tracksh.bmp")) return false;
	if (!loadImage(node, "tracksvh", tracksVHId_, "data/tanks/tracksvh.bmp")) return false;
	if (!loadImage(node, "trackshv", tracksHVId_, "data/tanks/trackshv.bmp")) return false;

	// Read all of the tank display catagories
	std::string catagory;
	while (node->getNamedChild("catagory", catagory, false))
	{
		catagories_.insert(catagory);
	}

	// Read all of the tank team catagories
	int team;
	while (node->getNamedChild("team", team, false))
	{
		teams_.insert(team);
	}

	// Read aionly attribute
	aiOnly_ = false;
	node->getNamedChild("aionly", aiOnly_, false);

	// Read movementSmoke attr
	movementSmoke_ = true;
	node->getNamedChild("movementsmoke", movementSmoke_, false);

	// Check there are no more nodes in this node
	return node->failChildren();
}

bool TankModel::loadImage(XMLNode *node, const char *nodeName, 
	ImageID &image, const char *backupImage)
{
	image.initFromString(S3D::eModLocation, backupImage, backupImage, true);
	return true;
}

bool TankModel::lessThan(TankModel *other)
{
	return (strcmp(getName(), other->getName()) < 0);
}

bool TankModel::isOfCatagory(const char *catagory)
{
	std::set<std::string>::iterator itor =
		catagories_.find(catagory);
	return (itor != catagories_.end());
}

bool TankModel::isOfTankType(const char *tankType)
{
	if (tankTypes_.empty()) return true;
	std::set<std::string>::iterator itor =
		tankTypes_.find(tankType);
	return (itor != tankTypes_.end());
}

bool TankModel::isOfAi(bool ai)
{
	if (!aiOnly_) return true;
	if (ai) return true;
	return false;
}

bool TankModel::isOfTeam(int team)
{
	if (team == 0) return true; // No Team
	if (teams_.empty()) return true; // Tank not in a team
	std::set<int>::iterator itor =
		teams_.find(team);
	return (itor != teams_.end());
}

bool TankModel::availableForTank(Tank *tank)
{
	if (isOfTeam(tank->getTeam()) &&
		isOfAi(tank->getDestinationId() == 0) &&
		isOfTankType(tank->getTanketType()->getName()))
	{
		return true;
	}

	return false;
}
