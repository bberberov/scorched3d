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

#include <actions/TanketMovement.h>
#include <actions/TargetFalling.h>
#include <actions/ShotProjectile.h>
#include <actions/CameraPositionAction.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/WeaponMoveTank.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/MovementMap.h>
#include <landscapemap/DeformLandscape.h>
#ifndef S3D_SERVER
	#include <landscape/Smoke.h>
	#include <landscape/Landscape.h>
	#include <image/ImageStore.h>
	#include <GLEXT/GLImageModifier.h>
	#include <sound/Sound.h>
#endif
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankModelContainer.h>
#include <tanket/TanketAccessories.h>
#include <tank/TankViewPoints.h>
#include <tank/TankModelStore.h>
#include <target/TargetDamage.h>
#include <target/TargetLife.h>
#include <target/TargetState.h>
#include <target/TargetSpace.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

static const int NoMovementTransitions = 4;

TanketMovement::TanketMovement(WeaponFireContext &weaponContext,
	WeaponMoveTank *weapon,
	int positionX, int positionY) : 
	Action(weaponContext.getInternalContext().getReferenced()),
	weaponContext_(weaponContext), 
	positionX_(positionX), positionY_(positionY),
	timePassed_(0), weapon_(weapon),
	remove_(false), moving_(true), moveSoundSource_(0),
	smokeCounter_(0.1f, 0.1f), stepCount_(0),
	vPoint_(0)
{
}

TanketMovement::~TanketMovement()
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		delete moveSoundSource_;
		moveSoundSource_ = 0;
	}
#endif
	if (vPoint_) vPoint_->decrementReference();
}

void TanketMovement::init()
{
	Tanket *tanket = context_->getTargetContainer().getTanketById(weaponContext_.getPlayerId());
	if (!tanket) return;	

	tanket->getTargetState().setMoving(this);

	startPosition_ = tanket->getLife().getTargetPosition();

	// Start the tank movement sound
#ifndef S3D_SERVER
	if (!context_->getServerMode() &&
		tanket->getType() == Target::TypeTank) 
	{
		vPoint_ = new TankViewPointProvider();
		vPoint_->setValues(startPosition_);
		vPoint_->incrementReference();

		CameraPositionAction *positionAction = new CameraPositionAction(
			weaponContext_.getPlayerId(),
			vPoint_,
			5,
			10, 
			false);
		context_->getActionController().addAction(positionAction);

		SoundBuffer *moveSound = 
			Sound::instance()->fetchOrCreateBuffer(
				S3D::getModFile("data/wav/movement/tankmove.wav"));
		moveSoundSource_ = new VirtualSoundSource(VirtualSoundPriority::eAction, true, false);
		moveSoundSource_->setPosition(tanket->getLife().getTargetPosition().asVector());
		moveSoundSource_->play(moveSound);
	}
#endif // #ifndef S3D_SERVER

	// As with everything to do with movement
	// The xy position is stored as an unsigned int
	// to save space, z is calculated from the landscape
	// Lower 32 bits = y position
	// Upper 32 bits = x positions
	std::list<unsigned int> positions;
	MovementMap mmap(
		tanket, 
		*context_);
	FixedVector pos(positionX_, positionY_, 0);
	mmap.calculatePosition(pos, mmap.getFuel(weapon_));
	
	MovementMap::MovementMapEntry entry =
		mmap.getEntry(positionX_, positionY_);
	if (entry.type == MovementMap::eMovement)
	{
		// Add the end (destination) point to the list of points for the tank
		// to visit
		unsigned int pt = (positionX_ << 16) | (positionY_ & 0xffff);
		positions.push_front(pt);

		// Work backward to the source point and pre-pend them onto the
		// this of points
		while (entry.srcEntry)
		{
			pt = entry.srcEntry;
			unsigned int x = pt >> 16;
			unsigned int y = pt & 0xffff;
			positions.push_front(pt);
			entry = mmap.getEntry(x, y);
		}
	}
	
	// Expand these positions into a interpolated set of positions with
	// x, y and z
	std::list<unsigned int>::iterator itor;
	for (itor = positions.begin();
		itor != positions.end();)
	{
		unsigned int fistpt = (*itor);
		++itor;

		if (itor != positions.end())
		{
			unsigned int secpt = (*itor);

			int firstx = int(fistpt >> 16);
			int firsty = int(fistpt & 0xffff);
			int secx = int(secpt >> 16);
			int secy = int(secpt & 0xffff);
			int diffX = secx - firstx;
			int diffY = secy - firsty;
			fixed ang = (atan2x(fixed(diffY), fixed(diffX)) / fixed::XPI * 180) - 90;

			for (int i=0; i<NoMovementTransitions; i++)
			{
				fixed currentX = fixed(firstx) + fixed(diffX)/fixed(NoMovementTransitions)*fixed(i+1);
				fixed currentY = fixed(firsty) + fixed(diffY)/fixed(NoMovementTransitions)*fixed(i+1);
				expandedPositions_.push_back(
					PositionEntry(
						firstx, firsty, 
						secx, secy,
						currentX, currentY, 
						ang, (i==(NoMovementTransitions-1))));
			}
		}
	}

	// If this weapon is set to use a constant amount of fuel then use this amount
	if (weapon_->getUseFuel() > 0)
	{
		tanket->getAccessories().rm(weapon_->getParent(), weapon_->getUseFuel());
	}
}

std::string TanketMovement::getActionDetails()
{
	return S3D::formatStringBuffer("%u %i,%i %s",
		weaponContext_.getPlayerId(), positionX_, positionY_, 
		weapon_->getParent()->getName());
}

void TanketMovement::simulate(fixed frameTime, bool &remove)
{
	if (!remove_)
	{
		if (moving_)
		{
			simulationMove(frameTime);
		}
	}
	else
	{
		remove = true;
	}

#ifndef S3D_SERVER
	if (remove && moveSoundSource_)
	{
		moveSoundSource_->stop();
	}
#endif // #ifndef S3D_SERVER
	
	Action::simulate(frameTime, remove);
}

void TanketMovement::simulationMove(fixed frameTime)
{
	Tanket *tanket = 
		context_->getTargetContainer().getTanketById(weaponContext_.getPlayerId());
	if (tanket)
	{
		// Stop moving if the tank is dead
		if (tanket->getAlive())
		{
			// Check to see if this tank is falling
			// If it is then we wait until the fall is over
			if (!tanket->getTargetState().getFalling() &&
				tanket->getTargetState().getMoving() == this)
			{
				// Add a smoke trail
				// Check if we are not on the server
#ifndef S3D_SERVER
				if (!context_->getServerMode() &&
					tanket->getType() == Target::TypeTank)
				{
					Tank *tank = (Tank *) tanket;

					// Check if this tank type allows smoke trails
					TankModel *model = tank->getModelContainer().getTankModel();
					if (model && model->getMovementSmoke())
					{
						if (smokeCounter_.nextDraw(frameTime.asFloat()))
						{
							Landscape::instance()->getSmoke().addSmoke(
								tanket->getLife().getFloatPosition()[0],
								tanket->getLife().getFloatPosition()[1],
								tanket->getLife().getFloatPosition()[2]);
						}
					}
				}
#endif // S3D_SERVER

				// Move the tank one position every stepTime seconds
				// i.e. 1/stepTime positions a second
				timePassed_ += frameTime;
				fixed stepTime = weapon_->getStepTime();
				while (timePassed_ >= stepTime)
				{
					timePassed_ -= stepTime;
					if (!expandedPositions_.empty())
					{
						moveTanket(tanket);
					}
					else break;
				}

				if (expandedPositions_.empty()) moving_ = false;
			}
		}
		else moving_ = false;
	}
	else moving_ = false;

	if (moving_ == false)
	{
		if (tanket)
		{
			if (tanket->getTargetState().getMoving() == this) 
			{
				tanket->getTargetState().setMoving(0);
			}
			tanket->getLife().setRotation(0);
			if (tanket->getAlive())
			{
				// Move the tank to the final position
				DeformLandscape::flattenArea(*context_, tanket->getLife().getTargetPosition());
			}
		}

		remove_ = true;
	}
}

void TanketMovement::moveTanket(Tanket *tanket)
{
	fixed x = expandedPositions_.front().x;
	fixed y = expandedPositions_.front().y;
	fixed a = expandedPositions_.front().ang;
	bool useF = expandedPositions_.front().useFuel;

	int firstx = expandedPositions_.front().firstX;
	int firsty = expandedPositions_.front().firstY;
	fixed firstz = context_->getLandscapeMaps().getGroundMaps().getHeight(firstx, firsty);

	int secondx = expandedPositions_.front().secondX;
	int secondy = expandedPositions_.front().secondY;
	fixed secondz = context_->getLandscapeMaps().getGroundMaps().getHeight(secondx, secondy);
	fixed z = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(x, y);
	expandedPositions_.pop_front();

	// Form the new tank position
	FixedVector newPos(x, y, z);

	// Check we are not trying to climb to high (this may be due
	// to the landscape changing after we started move)
	if (secondz - firstz > context_->getOptionsGame().getMaxClimbingDistance())
	{
		expandedPositions_.clear();
		return;
	}

	// Check to see we are not moving into water with a movement restriction
	// in place
	if (context_->getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLand ||
		context_->getOptionsGame().getMovementRestriction() ==
		OptionsGame::MovementRestrictionLandOrAbove)
	{
		fixed waterHeight = -10;
		LandscapeTex &tex = *context_->getLandscapeMaps().getDefinitions().getTex();
		if (tex.border->getType() == LandscapeTexType::eWater)
		{
			LandscapeTexBorderWater *water = 
				(LandscapeTexBorderWater *) tex.border;
			waterHeight = water->height;
		}

		if (context_->getOptionsGame().getMovementRestriction() ==
			OptionsGame::MovementRestrictionLandOrAbove)
		{
			if (waterHeight > startPosition_[2] - fixed(true, 1000))
			{
				waterHeight = startPosition_[2] - fixed(true, 1000);
			}
		}

		if (secondz < waterHeight)
		{
			expandedPositions_.clear();
			return;
		}
	}

	// Check this new position is allowed
	if (!MovementMap::allowedPosition(*context_, tanket, newPos))
	{
		expandedPositions_.clear();
		return;
	}

	// Move the tank to this new position
	// Use up one unit of fuel
	// -1 means use 1 unit of fuel per movement square
	if (useF && (weapon_->getUseFuel() == -1))
	{
		tanket->getAccessories().rm(weapon_->getParent(), 1);
	}

	// Actually move the tank
	tanket->getLife().setRotation(a);
	tanket->getLife().setTargetPosition(newPos);

	// Remove the targets that this tank "drives over"
	std::map<unsigned int, Target *> collisionTargets;
	context_->getTargetSpace().getCollisionSet(
		tanket->getLife().getTargetPosition(), 3, collisionTargets, false);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		++itor)
	{
		// Check that this is a target we have driven over
		// and we can destroy it
		Target *target = (*itor).second;
		if (target->getType() != Target::TypeTank &&
			target->getTargetState().getDriveOverToDestroy())
		{
			// Kill the target we've driven over
			TargetDamage::damageTarget(*context_,
				weapon_, target->getPlayerId(), weaponContext_, 
				target->getLife().getLife(),
				false, false, false);

			// Do a small explosion where we remove this target
			Accessory *accessory = 
				context_->getAccessoryStore().findByPrimaryAccessoryName("DriveOverDestroy");
			if (accessory && accessory->getType() == AccessoryPart::AccessoryWeapon)
			{
				Weapon *weapon = (Weapon *) accessory->getAction();
				weapon->fire(*context_, 
					weaponContext_,
					tanket->getLife().getTargetPosition(), 
					FixedVector::getNullVector());
			}
		}
	}

	// Add tracks
#ifndef S3D_SERVER
	if (!context_->getServerMode() &&
		tanket->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) tanket;
		stepCount_++;
		if (stepCount_ % 5 == 0)
		{
			TankModel *model = tank->getModelContainer().getTankModel();
			if (model)
			{
				Image image;
				if (firstx == secondx)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksVId());
				}
				else if (firsty == secondy)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksHId());
				}
				else if (firsty - secondy == firstx - secondx)
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksVHId());
				}
				else 
				{
					image = ImageStore::instance()->
						loadImage(model->getTracksHVId());
				}

				ImageModifier::addBitmapToLandscape(
					*context_,
					image,
					newPos[0].asFloat(), 
					newPos[1].asFloat(),
					0.04f, 0.04f,
					true);
			}
		}

		if (vPoint_) vPoint_->setValues(newPos);
	}

	if (moveSoundSource_) moveSoundSource_->setPosition(newPos.asVector());

#endif // #ifndef S3D_SERVER
}
