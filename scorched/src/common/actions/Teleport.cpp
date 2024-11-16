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

#include <actions/Teleport.h>
#include <actions/CameraPositionAction.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankViewPoints.h>
#include <target/TargetDamageCalc.h>
#include <target/TargetLife.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <sound/SoundUtils.h>
	#include <sprites/TeleportRenderer.h>
#endif

Teleport::Teleport(FixedVector position,
		WeaponFireContext &weaponContext,
		WeaponTeleport *weapon) :
	Action(weaponContext.getInternalContext().getReferenced()),
	position_(position), 
	weaponContext_(weaponContext),
	weapon_(weapon),
	totalTime_(0),
	firstTime_(true)
{

}

Teleport::~Teleport()
{
}

void Teleport::init()
{
#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		Tank *tank = context_->getTargetContainer().getTankById(weaponContext_.getPlayerId());
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			Vector white(1.0f, 1.0f, 1.0f);
			TeleportRenderer *teleport = new TeleportRenderer(
				tank->getLife().getTargetPosition().asVector(),
				white);
			context_->getActionController().addAction(new SpriteAction(teleport));
		}

		TankViewPointProvider *vPoint = new TankViewPointProvider();
		vPoint->setValues(position_);
		CameraPositionAction *pos = new CameraPositionAction(
			weaponContext_.getPlayerId(),
			vPoint, 5, 5, false);
		context_->getActionController().addAction(pos);
	}
#endif
}

void Teleport::simulate(fixed frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;

#ifndef S3D_SERVER
		if (!context_->getServerMode())
		{
			Tank *tank = context_->getTargetContainer().getTankById(weaponContext_.getPlayerId());
			if (tank && tank->getState().getState() == TankState::sNormal)
			{
				SoundBuffer *activateSound = 
					Sound::instance()->fetchOrCreateBuffer(
						S3D::getModFile(weapon_->getSound()));
				SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
					activateSound, tank->getLife().getTargetPosition().asVector());
			}
		}
#endif // #ifndef S3D_SERVER
	}

	totalTime_ += frameTime;
	if (totalTime_ > weapon_->getDelay(*context_))
	{
		Tank *tank = context_->getTargetContainer().getTankById(weaponContext_.getPlayerId());
		if (tank && tank->getState().getState() == TankState::sNormal)
		{
			fixed height = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(
				position_[0], position_[1]);
			if (weapon_->getGroundOnly() || height >= position_[2])
			{
				// Set the position on the ground
				position_[2] = height;

				if (context_->getOptionsGame().getActionSyncCheck())
				{
					context_->getSimulator().addSyncCheck(
						S3D::formatStringBuffer("Telport: %u %s", 
							tank->getPlayerId(),
							position_.asQuickString()));
				}

				// Set this position and flatten the landscape
				tank->getLife().setTargetPosition(position_);
				DeformLandscape::flattenArea(*context_, position_);
			}
			else
			{
				if (context_->getOptionsGame().getActionSyncCheck())
				{
					context_->getSimulator().addSyncCheck(
						S3D::formatStringBuffer("Telport: %u %s", 
							tank->getPlayerId(),
							position_.asQuickString()));
				}

				// Set the position, what ever this is
				tank->getLife().setTargetPosition(position_);

				// Check if this tank can fall, this will result in flattening the area
				TargetDamageCalc::damageTarget(*context_, 
					weaponContext_.getPlayerId(), weapon_, 
					weaponContext_, 0, false, true, false);
			}
		}

		remove = true;
	}

	Action::simulate(frameTime, remove);
}

std::string Teleport::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s %s", 
		weaponContext_.getPlayerId(), 
		position_.asQuickString(),
		weapon_->getParent()->getName());
}
