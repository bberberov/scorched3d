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

#include <weapons/Shield.h>
#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

Shield::Shield() : 
	laserProof_(ShieldLaserProofNone), 
	movementProof_(ShieldMovementAll)
{
}

Shield::~Shield()
{
}

bool Shield::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	// Get the remove power 
	if (!accessoryNode->getNamedChild("removepower", removePower_)) return false;

	// Get the penetration
	if (!accessoryNode->getNamedChild("penetration", penetration_)) return false;

	// Get the penetration
	if (!accessoryNode->getNamedChild("power", power_)) return false;

	// Get the collision sound
	if (!accessoryNode->getNamedChild("collisionsound", collisionSound_)) return false;
	if (!S3D::checkDataFile(S3D::formatStringBuffer("data/wav/%s", getCollisionSound()))) return false;

	// Get the accessory color
	XMLNode *colorNode = 0;
	if (!accessoryNode->getNamedChild("color", colorNode)) return false;
	if (!colorNode->getNamedChild("r", color_[0])) return false;
	if (!colorNode->getNamedChild("g", color_[1])) return false;
	if (!colorNode->getNamedChild("b", color_[2])) return false;

	std::string laserproof;
	if (accessoryNode->getNamedChild("laserproof", laserproof, false))
	{
		if (0 == strcmp(laserproof.c_str(), "false"))
			laserProof_ = ShieldLaserProofNone;
		else if (0 == strcmp(laserproof.c_str(), "true"))
			laserProof_ = ShieldLaserProofStop;
		else if (0 == strcmp(laserproof.c_str(), "total"))
			laserProof_ = ShieldLaserProofTotal;
		else return accessoryNode->returnError("Unknown laserproof type");
	}

	std::string movementproof;
	if (accessoryNode->getNamedChild("movementproof", movementproof, false))
	{
		if (0 == strcmp(movementproof.c_str(), "false"))
			movementProof_ = ShieldMovementAll;
		else if (0 == strcmp(movementproof.c_str(), "true"))
			movementProof_ = ShieldMovementSame;
		else if (0 == strcmp(movementproof.c_str(), "none"))
			movementProof_ = ShieldMovementNone;
		else if (0 == strcmp(movementproof.c_str(), "team1")) 
			movementProof_ = ShieldMovementTeam1;
		else if (0 == strcmp(movementproof.c_str(), "team2")) 
			movementProof_ = ShieldMovementTeam2;
		else if (0 == strcmp(movementproof.c_str(), "team3")) 
			movementProof_ = ShieldMovementTeam3;
		else if (0 == strcmp(movementproof.c_str(), "team4")) 
			movementProof_ = ShieldMovementTeam4;
		else return accessoryNode->returnError("Unknown movementproof type");
	}

	return true;
}

const char *Shield::getCollisionSound()
{
	if (!collisionSound_.c_str()[0]) return 0;
	return collisionSound_.c_str();
}
