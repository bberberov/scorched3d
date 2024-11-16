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

#if !defined(__INCLUDE_TankModelh_INCLUDE__)
#define __INCLUDE_TankModelh_INCLUDE__

#include <common/ModelID.h>
#include <common/Vector.h>
#include <image/ImageID.h>
#include <set>
#include <string>

class ScorchedContext;
class Tank;
class TankModelStore;
class TankModel
{
public:
	TankModel();
	virtual ~TankModel();

	bool initFromXML(ScorchedContext &context, XMLNode *node);

	virtual bool lessThan(TankModel *other);

	const char *getName() { return tankName_.c_str(); }
	ModelID &getTankModelID() { return modelId_; }
	ModelID &getProjectileModelID() { return projectileModelId_; }
	ImageID &getTracksVId() { return tracksVId_; }
	ImageID &getTracksHId() { return tracksHId_; }
	ImageID &getTracksVHId() { return tracksVHId_; }
	ImageID &getTracksHVId() { return tracksHVId_; }

	bool getMovementSmoke() { return movementSmoke_; }
	bool isOfCatagory(const char *catagory);
	bool isOfAi(bool ai);
	bool isOfTankType(const char *tankType);
	bool isOfTeam(int team);

	bool availableForTank(Tank *tank);

protected:
	friend class TankModelStore;

	bool aiOnly_;
	bool movementSmoke_;
	std::string tankName_;
	ModelID modelId_;
	ModelID projectileModelId_;
	ImageID tracksVId_;
	ImageID tracksHId_;
	ImageID tracksVHId_;
	ImageID tracksHVId_;
	std::set<std::string> tankTypes_;
	std::set<std::string> catagories_;
	std::set<int> teams_;

	std::set<std::string> &getCatagories() { return catagories_; }

	bool loadImage(XMLNode *node, const char *nodeName, 
		ImageID &image, const char *backupImage);
};

#endif
