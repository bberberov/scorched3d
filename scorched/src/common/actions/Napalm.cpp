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

#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <target/TargetContainer.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetRenderer.h>
#include <target/TargetState.h>
#include <target/TargetSpace.h>
#include <target/TargetLife.h>
#include <tank/TankViewPoints.h>
#include <actions/Napalm.h>
#include <actions/CameraPositionAction.h>
#ifndef S3D_SERVER
	#include <sprites/ExplosionTextures.h>
	#include <GLEXT/GLStateExtension.h>
	#include <landscape/Landscape.h>
	#include <landscape/DeformTextures.h>
	#include <landscape/Smoke.h>
	#include <client/ScorchedClient.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeTex.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

static const int deformSize = 3;
static DeformLandscape::DeformPoints deformMap;
static bool deformCreated = false;

#define XY_TO_UINT(x, y) ((((unsigned int) x) << 16) | (((unsigned int) y) & 0xffff))
#define XY2_TO_UINT(x, y) ((((unsigned int) x - x % 2) << 16) | (((unsigned int) y - y % 2) & 0xffff))
#define UINT_TO_X(pt) ((int)(pt >> 16))
#define UINT_TO_Y(pt) ((int)(pt & 0xffff))

Napalm::Napalm(int x, int y, Weapon *weapon, 
	NapalmParams *params,
	WeaponFireContext &weaponContext) :
	Action(weaponContext.getInternalContext().getReferenced()),
	startX_(x), startY_(y), napalmTime_(0), 
	weapon_(weapon), params_(params),
	weaponContext_(weaponContext), 
	totalTime_(0), hurtTime_(0),
	counter_(0.1f, 0.1f), set_(0),
	particleSet_(0), vPoint_(0)
{
}

Napalm::~Napalm()
{
	delete params_;
	if (vPoint_) vPoint_->decrementReference();
	while (!napalmPoints_.empty()) 
	{
		NapalmEntry *entry = napalmPoints_.front();
		delete entry;
		napalmPoints_.pop_front();
	}
}

void Napalm::init()
{
	if (!deformCreated)
	{
		deformCreated = true;

		Vector center(deformSize + 1, deformSize + 1);
		for (int a=0; a<(deformSize + 1) * 2; a++)
		{
			for (int b=0; b<(deformSize + 1) * 2; b++)
			{
				Vector pos(a, b);
				float dist = (center - pos).Magnitude();
				dist /= deformSize;
				dist = 1.0f - MIN(1.0f, dist);

				DIALOG_ASSERT(a < 100 && b < 100);
				deformMap.map[a][b] = fixed::fromFloat(dist);
			}
		}
	}

	edgePoints_.insert(XY_TO_UINT(startX_, startY_));

#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		if (!params_->getNoCameraTrack())
		{
			FixedVector position(fixed(startX_), fixed(startY_), context_->getLandscapeMaps().
				getGroundMaps().getHeight(startX_, startY_));
			vPoint_ = new TankViewPointProvider();
			vPoint_->setValues(position);
			vPoint_->incrementReference();

			CameraPositionAction *pos = new CameraPositionAction(
				weaponContext_.getPlayerId(),
				vPoint_,
				5, 5, true);
			context_->getActionController().addAction(pos);
		}

		set_ = ExplosionTextures::instance()->getTextureSetByName(
			params_->getNapalmTexture());
	}
#endif // #ifndef S3D_SERVER
}

std::string Napalm::getActionDetails()
{
	return S3D::formatStringBuffer("%i,%i %s",
		startX_, startY_, weapon_->getParent()->getName());
}

void Napalm::simulate(fixed frameTime, bool &remove)
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (!napalmPoints_.empty() &&
			!params_->getNoSmoke() &&
			counter_.nextDraw(frameTime.asFloat()))
		{
			NapalmEntry *entry = 0;
			int count = rand() % napalmPoints_.size();
			std::list<NapalmEntry *>::iterator itor;
			for (itor = napalmPoints_.begin();
				itor != napalmPoints_.end();
				itor++, count --)
			{
				entry = *itor;
				if (count <=0) break;
			}

			fixed posZ = 
				ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeight(
				entry->posX, entry->posY);
			Landscape::instance()->getSmoke().
				addSmoke(float(entry->posX), float(entry->posY), posZ.asFloat());
		}
	}
#endif // #ifndef S3D_SERVER

	// Add napalm for the period of the time interval
	// once the time interval has expired then start taking it away
	// Once all napalm has disapeared the simulation is over
	totalTime_ += frameTime;
	while (totalTime_ > params_->getStepTime())
	{
		totalTime_ -= params_->getStepTime();
		napalmTime_ += params_->getStepTime();
		if (napalmTime_ < params_->getNapalmTime())
		{
			// Still within the time period, add more napalm
			if (int(napalmPoints_.size()) < params_->getNumberParticles()) 
			{
				simulateAddStep();
			}

			// Check for the case where we land in water
			if (napalmPoints_.empty())
			{
				remove = true;
				break;
			}
		}
		else
		{
			// Not within the time period remove napalm
			if (!napalmPoints_.empty())
			{
				simulateRmStep();
			}
			else
			{
				remove = true;
				break;
			}
		}
	}

	// Calculate how much damage to make to the tanks
	hurtTime_ += frameTime;
	while (hurtTime_ > params_->getHurtStepTime())
	{
		hurtTime_ -= params_->getHurtStepTime();

		simulateDamage();
	}

	Action::simulate(frameTime, remove);
}

fixed Napalm::getHeight(int x, int y)
{
	LandscapeMaps *hmap = &context_->getLandscapeMaps();
	if (x < 0 || y < 0 ||
		x > hmap->getGroundMaps().getLandscapeWidth() ||
		y > hmap->getGroundMaps().getLandscapeHeight())
	{
		// The height at the sides of the landscape is huge
		// so we will never go there with the napalm
		return fixed::MAX_FIXED;
	}

	// Return the correct height the square + the
	// height of all the napalm on this square
	// the napalm builds up and get higher so
	// we can go over small bumps
	return hmap->getGroundMaps().getHeight(x, y) +
		hmap->getGroundMaps().getNapalmHeight(x, y);
}

void Napalm::simulateRmStep()
{
	int pset = napalmPoints_.front()->pset;
	while (!napalmPoints_.empty()) 
	{
		// Check if the entry should be removed
		NapalmEntry *entry = napalmPoints_.front();
		if (pset != entry->pset) break;

		// Remove the first napalm point from the list
		// and remove the height from the landscape
		napalmPoints_.pop_front();
		int x = entry->posX;
		int y = entry->posY;
		delete entry;

		unsigned int pointsCount = XY2_TO_UINT(x, y);
		std::map<unsigned int, int>::iterator countItor =
			napalmPointsCount_.find(pointsCount);
		if (countItor != napalmPointsCount_.end())
		{
			countItor->second--;
			if (countItor->second == 0) napalmPointsCount_.erase(countItor);
		}

		context_->getLandscapeMaps().getGroundMaps().getNapalmHeight(x, y) -= params_->getNapalmHeight();
	}
}

void Napalm::simulateAddStep()
{
	particleSet_++;

	std::set<unsigned int> currentEdges = edgePoints_;
	edgePoints_.clear();

	std::set<unsigned int>::iterator itor;
	for (itor = currentEdges.begin();
		itor != currentEdges.end();
		++itor)
	{
		unsigned int currentEdge = *itor;
		int x = UINT_TO_X(currentEdge);
		int y = UINT_TO_Y(currentEdge);

		simulateAddEdge(x, y);
	}
}

void Napalm::simulateAddEdge(int x, int y)
{
	// Get the height of this point
	fixed height = getHeight(x, y);

	if (!params_->getAllowUnderWater())
	{
		// Check napalm is under water 
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;
       	 	waterHeight = water->height;
		}

		if (height < waterHeight) // Water height
		{
			// Perhaps we could add a boiling water sound at some point
			return;
		}
	} 

	// Add this current point to the napalm map
	RandomGenerator &random = context_->getSimulator().getRandomGenerator();
	int offset = (random.getRandFixed("Napalm") * 31).asInt();
	NapalmEntry *newEntry = new NapalmEntry(x, y, offset, particleSet_);
	napalmPoints_.push_back(newEntry);

	unsigned int pointsCount = XY2_TO_UINT(x, y);
	std::map<unsigned int, int>::iterator countItor =
		napalmPointsCount_.find(pointsCount);
	if (countItor == napalmPointsCount_.end())
	{
		napalmPointsCount_.insert(std::pair<unsigned int, int>(pointsCount, 1));
	}
	else
	{
		countItor->second++;
	}

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		ParticleEmitter emitter;
		emitter.setAttributes(
			params_->getNapalmTime().asFloat(), params_->getNapalmTime().asFloat(),
			0.5f, 1.0f, // Mass
			0.01f, 0.02f, // Friction
			Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
			Vector(1.0f, 1.0f, 1.0f), 0.9f, // StartColor1
			Vector(1.0f, 1.0f, 1.0f), 0.6f, // StartColor2
			Vector(1.0f, 1.0f, 1.0f), 0.0f, // EndColor1
			Vector(1.0f, 1.0f, 1.0f), 0.1f, // EndColor2
			1.5f, 1.5f, 1.5f, 1.5f, // Start Size
			1.5f, 1.5f, 1.5f, 1.5f, // EndSize
			Vector(0.0f, 0.0f, 0.0f), // Gravity
			params_->getLuminance(),
			false);
		Vector position1(float(x) + 0.5f, float(y) - 0.2f, 0.0f);
		Vector position2(float(x) - 0.5f, float(y) - 0.2f, 0.0f);
		Vector position3(float(x) + 0.0f, float(y) + 0.5f, 0.0f);
		emitter.emitNapalm(
			position1, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);
		emitter.emitNapalm(
			position2, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);
		emitter.emitNapalm(
			position3, 
			ScorchedClient::instance()->getParticleEngine(),
			set_);

		if (vPoint_) vPoint_->setValues(FixedVector::fromVector(position1));

		// Add the ground scorch
		if (!GLStateExtension::getNoTexSubImage())
		{
			if (height == context_->getLandscapeMaps().getGroundMaps().getHeight(x, y)) 
			{
				if (RAND < params_->getGroundScorchPer().asFloat())
				{
					Vector pos(x, y);
					DeformTextures::deformLandscape(pos, 
						(int) (deformSize + 1),
						ExplosionTextures::instance()->getScorchBitmap(
							params_->getDeformTexture()),
						deformMap);
				}
			}
		}
	}
#endif // #ifndef S3D_SERVER

	context_->getLandscapeMaps().getGroundMaps().getNapalmHeight(x, y) += params_->getNapalmHeight();

	// Calculate every time as the landscape may change
	// due to other actions
	fixed heightL = getHeight(x-1, y);
	fixed heightR = getHeight(x+1, y);
	fixed heightU = getHeight(x, y+1);
	fixed heightD = getHeight(x, y-1);

	if (params_->getSingleFlow()) 
	{
		fixed *heightLR = 0;
		int LR = 0;
		if (heightL < heightR)
		{
			heightLR = &heightL;
			LR = -1;
		}
		else if (heightL == heightR)
		{
			if (random.getRandUInt("Napalm") % 2 == 0)
			{
				heightLR = &heightL;
				LR = -1;
			}
			else
			{
				heightLR = &heightR;
				LR = +1;
			}
		}
		else
		{
			heightLR = &heightR;
			LR = +1;
		}

		fixed *heightUD = 0;
		int UD = 0;
		if (heightU < heightD)
		{
			heightUD = &heightU;	
			UD = +1;
		}
		else if (heightU == heightD)
		{
			if (random.getRandUInt("Napalm") % 2 == 0)
			{
				heightUD = &heightU;	
				UD = +1;
			}
			else
			{
				heightUD = &heightD;	
				UD = -1;
			}
		}
		else 
		{
			heightUD = &heightD;	
			UD = -1;
		}

		enum Direction
		{
			eUD,
			eLR,
			eNone
		} dir = eNone;
		if (*heightLR < *heightUD)
		{
			if (*heightLR < height) dir = eLR;
		}
		else if (*heightLR == *heightUD)
		{
			if (*heightLR < height) 
			{
				if (random.getRandUInt("Napalm") % 2 == 0)
				{
					dir = eUD;
				}
				else
				{
					dir = eLR;
				}				
			}
		}
		else
		{
			if (*heightUD < height) 
			{
				if (*heightLR < height) dir = eUD;
			}
		}

		switch (dir)
		{
		case eUD:
			edgePoints_.insert(XY_TO_UINT(x, y + UD));
			break;
		case eLR:
			edgePoints_.insert(XY_TO_UINT(x + LR, y));
			break;
		default:
			// None of the landscape is currently lower than the current point
			// Just wait, as this point will be now be covered in napalm
			// and may get higher and higher until it is
			edgePoints_.insert(XY_TO_UINT(x, y));
			break;
		}
	}
	else
	{
		int addedCount = 0;
		if (heightL < height)
		{
			// Move left
			addedCount++;
			edgePoints_.insert(XY_TO_UINT(x - 1, y));
		}
		if (heightR < height)
		{
			// Move right
			addedCount++;
			edgePoints_.insert(XY_TO_UINT(x + 1, y));
		}
		if (heightU < height)
		{
			// Move up
			addedCount++;
			edgePoints_.insert(XY_TO_UINT(x, y + 1));
		}
		if (heightD < height)
		{
			// Move down
			addedCount++;
			edgePoints_.insert(XY_TO_UINT(x, y - 1));
		}
		if (addedCount == 0)
		{
			// None of the landscape is currently lower than the current point
			// Just wait, as this point will be now be covered in napalm
			// and may get higher and higher until it is
			edgePoints_.insert(XY_TO_UINT(x, y));
		}
	}
}

void Napalm::simulateDamage()
{
	const int EffectRadius = params_->getEffectRadius();

	// Store how much each tank is damaged
	// Keep in a map so we don't need to create multiple
	// damage actions.  Now we only create one per tank
	static std::map<unsigned int, fixed> TargetDamageCalc;
	TargetDamageCalc.clear();

	// Add damage into the damage map for each napalm point that is near to
	// the tanks
	std::map<unsigned int, int>::iterator itor =
		napalmPointsCount_.begin();
	std::map<unsigned int, int>::iterator endItor = 
		napalmPointsCount_.end();
	for (;itor != endItor; ++itor)
	{
		unsigned int pointsCount = itor->first;
		fixed count = fixed(itor->second);
		int x = UINT_TO_X(pointsCount);
		int y = UINT_TO_Y(pointsCount);

		fixed height = context_->getLandscapeMaps().getGroundMaps().
			getHeight(x, y);
		FixedVector position(
			fixed(x), 
			fixed(y),
			height);

		if (params_->getLandscapeErosion() > 0)
		{
			DeformLandscape::deformLandscape(*context_, position, 
				1, true, params_->getLandscapeErosion(), params_->getDeformTexture());
			TargetDamageCalc::explosion(
				*context_, weapon_, weaponContext_, 
				position, 1, 0, true, false);
		}

		std::map<unsigned int, Target *> collisionTargets;
		context_->getTargetSpace().getCollisionSet(position, 
			fixed(EffectRadius), collisionTargets);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			++itor)
		{
			Target *target = (*itor).second;
			if (target->getAlive())
			{
				std::map<unsigned int, fixed>::iterator damageItor = 
					TargetDamageCalc.find(target->getPlayerId());
				if (damageItor == TargetDamageCalc.end())
				{
					TargetDamageCalc[target->getPlayerId()] = count * params_->getHurtPerSecond();
				}
				else
				{
					TargetDamageCalc[target->getPlayerId()] += count * params_->getHurtPerSecond();
				}
			}
		}
	}

	// Add all the damage to the tanks (if any)
	if (!TargetDamageCalc.empty())
	{
		std::map<unsigned int, fixed>::iterator damageItor;
		for (damageItor = TargetDamageCalc.begin();
			damageItor != TargetDamageCalc.end();
			++damageItor)
		{
			Target *target = 
				context_->getTargetContainer().getTargetById(damageItor->first);
			fixed damage = (*damageItor).second;

			// Set this target to burnt
			if (target->getRenderer() &&
				!params_->getNoObjectDamage())
			{
				target->getRenderer()->targetBurnt();
			}

			// Add damage to the tank
			// If allowed for this target type (mainly for trees)
			if (!target->getTargetState().getNoDamageBurn())
			{
				if (burnedTargets_.find(target->getPlayerId()) == burnedTargets_.end()) 
				{
					burnedTargets_.insert(target->getPlayerId());
					addBurnAction(target);
				}

				// Do last as it may remove the target
				TargetDamageCalc::damageTarget(*context_, target->getPlayerId(), weapon_, 
					weaponContext_, damage, true, false, false);
			}
		}
		TargetDamageCalc.clear();
	}
}

void Napalm::addBurnAction(Target *target)
{
	Accessory *accessory = target->getBurnAction();
	if (accessory)
	{
		Weapon *weapon = (Weapon *) accessory->getAction();
		if (context_->getOptionsGame().getActionSyncCheck())
		{
			context_->getSimulator().addSyncCheck(
				S3D::formatStringBuffer("BurnAction: %u %s", 
					target->getPlayerId(),
					weapon->getParent()->getName()));
		}

		FixedVector position = target->getLife().getTargetPosition();
		FixedVector velocity;
		WeaponFireContext weaponContext(weaponContext_.getPlayerId(), 
			weaponContext_.getInternalContext().getSelectPositionX(), 
			weaponContext_.getInternalContext().getSelectPositionY(), 
			weaponContext_.getInternalContext().getVelocityVector(),
			weaponContext_.getInternalContext().getReferenced(), 
			false);
		weapon->fire(*context_, weaponContext, 
			position, velocity);
	}
}
