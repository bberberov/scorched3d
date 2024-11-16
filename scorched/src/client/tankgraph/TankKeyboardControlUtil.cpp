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

#include <tankgraph/TankKeyboardControlUtil.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <tank/TankShotHistory.h>
#include <tanket/TanketAccessories.h>
#include <tanket/TanketShotInfo.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsDefenseMessage.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <weapons/AccessoryStore.h>
#include <common/Keyboard.h>
#include <common/Line.h>
#include <landscapemap/LandscapeMaps.h>
#include <graph/OptionsDisplay.h>
#include <graph/MainCamera.h>
#include <sound/Sound.h>

VirtualSoundSource *TankKeyboardControlUtil::elevateSound_(0);
VirtualSoundSource *TankKeyboardControlUtil::rotateSound_(0);
VirtualSoundSource *TankKeyboardControlUtil::startSound_(0);
VirtualSoundSource *TankKeyboardControlUtil::powerSound_(0);

TankKeyboardControlUtil::TankKeyboardControlUtil()
{
}

TankKeyboardControlUtil::~TankKeyboardControlUtil()
{
}

void TankKeyboardControlUtil::keyboardCheck(
	Tank *tank,
	const unsigned state, 
	float frameTime, char *buffer, 
	unsigned int keyState)
{
	// Create sounds
	if (!elevateSound_) elevateSound_ = 
		new VirtualSoundSource(VirtualSoundPriority::eRotation, true, false);
	elevateSound_->setPosition(tank->getLife().getTargetPosition().asVector());
	if (!rotateSound_) rotateSound_ =
		new VirtualSoundSource(VirtualSoundPriority::eRotation, true, false);
	rotateSound_->setPosition(tank->getLife().getTargetPosition().asVector());
	if (!startSound_) startSound_ =
		new VirtualSoundSource(VirtualSoundPriority::eRotation, false, false);
	startSound_->setPosition(tank->getLife().getTargetPosition().asVector());
	if (!powerSound_) powerSound_ =
		new VirtualSoundSource(VirtualSoundPriority::eRotation, true, false);
	powerSound_->setPosition(tank->getLife().getTargetPosition().asVector());

	// Check key moves
	moveLeftRight(tank, buffer, keyState, frameTime);
	moveUpDown(tank, buffer, keyState, frameTime);
	movePower(tank, buffer, keyState, frameTime);

	KEYBOARDKEY("FIRE_WEAPON", fireKey);
	if (fireKey->keyDown(buffer, keyState, false))
	{
		// Only allow the standard fire key when we are not selecting a point
		// on the landscape.
		Accessory *currentWeapon = 
			tank->getAccessories().getWeapons().getCurrent();
		if (currentWeapon &&
			currentWeapon->getPositionSelect() == Accessory::ePositionSelectNone)
		{
			fireShot(tank);
		}
	}

	KEYBOARDKEY("AUTO_AIM", aimKey);
	if (aimKey->keyDown(buffer, keyState))
	{
		autoAim(tank);
	}

	KEYBOARDKEY("ENABLE_PARACHUTES", parachuteKey);
	if (parachuteKey->keyDown(buffer, keyState, false))
	{
		std::list<Accessory *> &parachutes =
			tank->getAccessories().getAllAccessoriesByType(
				AccessoryPart::AccessoryParachute);
		if (parachutes.size() == 1)
		{
			parachutesUpDown(tank->getPlayerId(), parachutes.front()->getAccessoryId());
		}
	}

	KEYBOARDKEY("ENABLE_SHIELDS", shieldKey);
	if (shieldKey->keyDown(buffer, keyState, false))
	{
		if (!tank->getShield().getCurrentShield())
		{
			std::list<Accessory *> &shields =
				tank->getAccessories().getAllAccessoriesByType(
					AccessoryPart::AccessoryShield);
			if (shields.size() == 1)
			{
				shieldsUpDown(tank->getPlayerId(), shields.front()->getAccessoryId());
			}
		}
	}

	KEYBOARDKEY("USE_BATTERY", batteryKey);
	if (batteryKey->keyDown(buffer, keyState, false))
	{
		if (tank->getLife().getLife() < 
			tank->getLife().getMaxLife())
		{
			std::list<Accessory *> &entries =
				tank->getAccessories().getAllAccessoriesByType(
					AccessoryPart::AccessoryBattery);
			if (!entries.empty())
			{
				useBattery(tank->getPlayerId(), entries.front()->getAccessoryId());
			}
		}
	}

	KEYBOARDKEY("UNDO_MOVE", undoKey);
	if (undoKey->keyDown(buffer, keyState, false))
	{
		tank->getShotHistory().undo();
	}

	KEYBOARDKEY("CHANGE_UP_WEAPON", weaponUpKey);
	KEYBOARDKEY("CHANGE_DOWN_WEAPON", weaponDownKey);
	bool upWeapon = weaponUpKey->keyDown(buffer, keyState, false);
	bool downWeapon = weaponDownKey->keyDown(buffer, keyState, false);
	if (upWeapon || downWeapon)
	{
		if (downWeapon)
		{
			prevWeapon(tank);
		}
		else
		{
			nextWeapon(tank);
		}

		TargetRendererImplTankHUD::setText("Weapon : ", 
			tank->getAccessories().getWeapons().getWeaponString());
	}
}

void TankKeyboardControlUtil::nextWeapon(Tank *tank)
{
	std::list<Accessory *> &result =
		tank->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryWeapon);
	ScorchedClient::instance()->getAccessoryStore().sortList(result,
		OptionsDisplay::instance()->getAccessorySortKey());

	std::list<Accessory *>::iterator itor;
	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		if (tank->getAccessories().getWeapons().getCurrent() == (*itor))
		{
			if (++itor == result.end())
			{
				itor = result.begin();
			}
			tank->getAccessories().getWeapons().setWeapon(*itor);
			break;
		}
	}
}

void TankKeyboardControlUtil::prevWeapon(Tank *tank)
{
	std::list<Accessory *> &result =
		tank->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryWeapon);
	ScorchedClient::instance()->getAccessoryStore().sortList(result,
		OptionsDisplay::instance()->getAccessorySortKey());

	std::list<Accessory *>::iterator itor;
	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		if (tank->getAccessories().getWeapons().getCurrent() == (*itor))
		{
			if (itor == result.begin())
			{
				itor = result.end();
			}

			--itor;
			tank->getAccessories().getWeapons().setWeapon(*itor);
			break;
		}
	}
}

void TankKeyboardControlUtil::endPlayMove(Tank *tank)
{
	if (elevateSound_) elevateSound_->stop();
	if (rotateSound_) rotateSound_->stop();
	if (powerSound_) powerSound_->stop();
}

void TankKeyboardControlUtil::autoAim(Tank *tank)
{
	int x = ScorchedClient::instance()->getGameState().getMouseX();
	int y = ScorchedClient::instance()->getGameState().getMouseY();
	Line direction;
	Vector intersect;

	MainCamera::instance()->getCamera().draw();
	if (MainCamera::instance()->getCamera().
		getDirectionFromPt((GLfloat) x, (GLfloat) y, direction))
	{
		if (ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getIntersect(direction, intersect))
        {
			Vector &position = tank->getLife().getTargetPosition().asVector();

			// Calculate direction
			Vector direction = intersect - position;
			float angleXYRads = atan2f(direction[1], direction[0]);
			float angleXYDegs = (angleXYRads / 3.14f) * 180.0f - 90.0f;
			
			tank->getShotInfo().rotateGunXY(fixed::fromFloat(angleXYDegs), false);
			leftRightHUD(tank);

			TargetRendererImplTankAIM::setAimPosition(intersect);
		}
	}
}

void TankKeyboardControlUtil::moveLeftRight(Tank *tank, 
	char *buffer, unsigned int keyState, float frameTime)
{
	static bool LRMoving = false;
	bool currentLRMoving = false;

	KEYBOARDKEY("TURN_RIGHT", rightKey);
	KEYBOARDKEY("TURN_RIGHT_SLOW", rightSlowKey);
	KEYBOARDKEY("TURN_RIGHT_VSLOW", rightVSlowKey);
	KEYBOARDKEY("TURN_RIGHT_FAST", rightFastKey);
	KEYBOARDKEY("TURN_LEFT", leftKey);
	KEYBOARDKEY("TURN_LEFT_SLOW", leftSlowKey);
	KEYBOARDKEY("TURN_LEFT_VSLOW", leftVSlowKey);
	KEYBOARDKEY("TURN_LEFT_FAST", leftFastKey);

	bool rightK = rightKey->keyDown(buffer, keyState);
	bool rightKF = rightFastKey->keyDown(buffer, keyState);
	bool rightKS = rightSlowKey->keyDown(buffer, keyState);
	bool rightKVS = rightVSlowKey->keyDown(buffer, keyState);
	bool leftK = leftKey->keyDown(buffer, keyState);
	bool leftKF = leftFastKey->keyDown(buffer, keyState);
	bool leftKS = leftSlowKey->keyDown(buffer, keyState);
	bool leftKVS = leftVSlowKey->keyDown(buffer, keyState);

	if (rightK || rightKF || rightKS || rightKVS)
	{
		float mult = frameTime;
		if (rightKF) mult *= 4.0f;
		else if (rightKS) mult *= 0.25f;
		else if (rightKVS) mult *= 0.05f;

		tank->getShotInfo().rotateGunXY(fixed::fromFloat(-45.0f * mult));
		currentLRMoving = true;

		leftRightHUD(tank);
	}
	else if (leftK || leftKF || leftKS || leftKVS)
	{
		float mult = frameTime;
		if (leftKF) mult *= 4.0f;
		else if (leftKS) mult *= 0.25f;
		else if (leftKVS) mult *= 0.05f;

		tank->getShotInfo().rotateGunXY(fixed::fromFloat(45.0f * mult));
		currentLRMoving = true;

		leftRightHUD(tank);
	}

	if (LRMoving != currentLRMoving)
	{
		CACHE_SOUND(sound, S3D::getModFile("data/wav/movement/movement.wav"));
		CACHE_SOUND(turn, S3D::getModFile("data/wav/movement/turn.wav"));
		if (currentLRMoving)
		{
			startSound_->play(sound);
			rotateSound_->play(turn);
		}
		else
		{
			rotateSound_->stop();
		}

		LRMoving = currentLRMoving;
	}
} 

void TankKeyboardControlUtil::leftRightHUD(Tank *tank)
{		
	float rot = tank->getShotInfo().getRotationGunXY().asFloat() / 360.0f;
	TargetRendererImplTankHUD::setText("Rot:", 
		tank->getShotHistory().getRotationString(), rot * 100.0f);
}

void TankKeyboardControlUtil::moveUpDown(Tank *tank,
	char *buffer, unsigned int keyState, float frameTime)
{
	static bool UDMoving = false;
	bool currentUDMoving = false;

	KEYBOARDKEY("ROTATE_UP", staticUpKey);
	KEYBOARDKEY("ROTATE_UP_SLOW", staticUpSlowKey);
	KEYBOARDKEY("ROTATE_UP_VSLOW", staticUpVSlowKey);
	KEYBOARDKEY("ROTATE_UP_FAST", staticUpFastKey);
	KEYBOARDKEY("ROTATE_DOWN", staticDownKey);
	KEYBOARDKEY("ROTATE_DOWN_SLOW", staticDownSlowKey);
	KEYBOARDKEY("ROTATE_DOWN_VSLOW", staticDownVSlowKey);
	KEYBOARDKEY("ROTATE_DOWN_FAST", staticDownFastKey);

	KeyboardKey *upKey = staticUpKey;
	KeyboardKey *upSlowKey = staticUpSlowKey;
	KeyboardKey *upVSlowKey = staticUpVSlowKey;
	KeyboardKey *upFastKey = staticUpFastKey;
	KeyboardKey *downKey = staticDownKey;
	KeyboardKey *downSlowKey = staticDownSlowKey;
	KeyboardKey *downVSlowKey = staticDownVSlowKey;
	KeyboardKey *downFastKey = staticDownFastKey;
	if (OptionsDisplay::instance()->getInvertElevation())
	{
		upKey = staticDownKey;
		upSlowKey = staticDownSlowKey;
		upVSlowKey = staticDownVSlowKey;
		upFastKey = staticDownFastKey;
		downKey = staticUpKey;
		downSlowKey = staticUpSlowKey;
		downVSlowKey = staticUpVSlowKey;
		downFastKey = staticUpFastKey;
	}

	bool upK = upKey->keyDown(buffer, keyState);
	bool upKS = upSlowKey->keyDown(buffer, keyState);
	bool upKVS = upVSlowKey->keyDown(buffer, keyState);
	bool upKF = upFastKey->keyDown(buffer, keyState);
	bool downK = downKey->keyDown(buffer, keyState);
	bool downKS = downSlowKey->keyDown(buffer, keyState);
	bool downKVS = downVSlowKey->keyDown(buffer, keyState);
	bool downKF = downFastKey->keyDown(buffer, keyState);

	if (upK || upKS || upKF || upKVS)
	{
		float mult = frameTime;
		if (upKF) mult *= 4.0f;
		else if (upKS) mult *= 0.25f;
		else if (upKVS) mult *= 0.05f;

		tank->getShotInfo().rotateGunYZ(fixed::fromFloat(-45.0f * mult));
		currentUDMoving = true;

		upDownHUD(tank);
	}
	else if (downK || downKS || downKF || downKVS)
	{
		float mult = frameTime;
		if (downKF) mult *= 4.0f;
		else if (downKS) mult *= 0.25f;
		else if (downKVS) mult *= 0.05f;

		tank->getShotInfo().rotateGunYZ(fixed::fromFloat(45.0f * mult));
		currentUDMoving = true;

		upDownHUD(tank);
	}

	if (UDMoving != currentUDMoving)
	{
		CACHE_SOUND(sound, S3D::getModFile("data/wav/movement/movement.wav"));
		CACHE_SOUND(elevate, S3D::getModFile("data/wav/movement/elevate.wav"));
		if (currentUDMoving)
		{
			startSound_->play(sound);
			elevateSound_->play(elevate);
		}
		else
		{
			elevateSound_->stop();
		}

		UDMoving = currentUDMoving;
	}
}

void TankKeyboardControlUtil::upDownHUD(Tank *tank)
{
	float rot = tank->getShotInfo().getRotationGunYZ().asFloat() / 90.0f;
	TargetRendererImplTankHUD::setText("Ele:", 
		tank->getShotHistory().getElevationString(), rot * 100.0f);
}

void TankKeyboardControlUtil::movePower(Tank *tank,
	char *buffer, unsigned int keyState, float frameTime)
{
	static bool PMoving = false;
	bool currentPMoving = false;

	KEYBOARDKEY("INCREASE_POWER", incKey);
	KEYBOARDKEY("INCREASE_POWER_SLOW", incSlowKey);
	KEYBOARDKEY("INCREASE_POWER_VSLOW", incVSlowKey);
	KEYBOARDKEY("INCREASE_POWER_FAST", incFastKey);
	KEYBOARDKEY("DECREASE_POWER", decKey);
	KEYBOARDKEY("DECREASE_POWER_SLOW", decSlowKey);
	KEYBOARDKEY("DECREASE_POWER_VSLOW", decVSlowKey);
	KEYBOARDKEY("DECREASE_POWER_FAST", decFastKey);
	bool incK = incKey->keyDown(buffer, keyState);
	bool incKS = incSlowKey->keyDown(buffer, keyState);
	bool incKVS = incVSlowKey->keyDown(buffer, keyState);
	bool incKF = incFastKey->keyDown(buffer, keyState);
	bool decK = decKey->keyDown(buffer, keyState);
	bool decKS = decSlowKey->keyDown(buffer, keyState);
	bool decKVS = decVSlowKey->keyDown(buffer, keyState);
	bool decKF = decFastKey->keyDown(buffer, keyState);

	if (incK || incKS || incKF || incKVS) 
	{
		float mult = frameTime;
		if (incKF) mult *= 4.0f;
		else if (incKS) mult *= 0.25f;
		else if (incKVS) mult *= 0.05f;

		tank->getShotInfo().changePower(fixed::fromFloat(250.0f * mult));
		currentPMoving = true;

		powerHUD(tank);
	}
	else if (decK || decKS || decKF || decKVS) 
	{
		float mult = frameTime;
		if (decKF) mult *= 4.0f;
		else if (decKS) mult *= 0.25f;
		else if (decKVS) mult *= 0.05f;

		tank->getShotInfo().changePower(fixed::fromFloat(-250.0f * mult));
		currentPMoving = true;

		powerHUD(tank);
	}

	if (PMoving != currentPMoving)
	{
		CACHE_SOUND(power, S3D::getModFile("data/wav/movement/power.wav"));
		if (currentPMoving)
		{
			powerSound_->play(power);
		}
		else
		{
			powerSound_->stop();
		}

		PMoving = currentPMoving;
	}
}

void TankKeyboardControlUtil::powerHUD(Tank *tank)
{
	float power = tank->getShotInfo().getPower().asFloat() / 
		tank->getShotInfo().getMaxPower().asFloat();
	TargetRendererImplTankHUD::setText("Pwr:", 
		tank->getShotHistory().getPowerString(), power * 100.0f);
}


void TankKeyboardControlUtil::fireShot(Tank *tank)
{
	Accessory *currentWeapon = 
		tank->getAccessories().getWeapons().getCurrent();
	if (currentWeapon)
	{
		// send message saying we are finished with shot
		ComsPlayedMoveMessage comsMessage(
			tank->getPlayerId(), 
			tank->getShotInfo().getMoveId(),
			ComsPlayedMoveMessage::eShot);
		comsMessage.setShot(
			currentWeapon->getAccessoryId(),
			tank->getShotInfo().getRotationGunXY(),
			tank->getShotInfo().getRotationGunYZ(),
			tank->getShotInfo().getPower(),
			tank->getShotInfo().getSelectPositionX(),
			tank->getShotInfo().getSelectPositionY());
		tank->getShotHistory().madeShot();

		// If so we send this move to the server
		ComsMessageSender::sendToServer(comsMessage);

		// Stimulate into the next state waiting for all the shots
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	}
}

void TankKeyboardControlUtil::skipShot(Tank *tank)
{
	// send message saying we are finished with shot
	ComsPlayedMoveMessage comsMessage(
		tank->getPlayerId(), 
		tank->getShotInfo().getMoveId(),
		ComsPlayedMoveMessage::eSkip);

	// Check if we are running in a NET/LAN environment
	// If so we send this move to the server
	ComsMessageSender::sendToServer(comsMessage);

	// Stimulate into the next state waiting for all the shots
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
}

void TankKeyboardControlUtil::resign(Tank *tank)
{
	// send message saying we are finished with shot
	ComsPlayedMoveMessage comsMessage(
		tank->getPlayerId(), 
		tank->getShotInfo().getMoveId(),
		ComsPlayedMoveMessage::eResign);

	// Check if we are running in a NET/LAN environment
	// If so we send this move to the server
	ComsMessageSender::sendToServer(comsMessage);
	
	// Stimulate into the next state waiting for all the shots
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
}

void TankKeyboardControlUtil::parachutesUpDown(unsigned int playerId, unsigned int paraId)
{
	ComsDefenseMessage defenseMessage(
		playerId,
		(paraId!=0)?ComsDefenseMessage::eParachutesUp:ComsDefenseMessage::eParachutesDown,
		paraId);
	ComsMessageSender::sendToServer(defenseMessage);
}

void TankKeyboardControlUtil::shieldsUpDown(unsigned int playerId, unsigned int shieldId)
{
	ComsDefenseMessage defenseMessage(
		playerId,
		(shieldId!=0)?ComsDefenseMessage::eShieldUp:ComsDefenseMessage::eShieldDown,
		shieldId);
	ComsMessageSender::sendToServer(defenseMessage);
}

void TankKeyboardControlUtil::useBattery(unsigned int playerId, unsigned int batteryId)
{
	ComsDefenseMessage defenseMessage(
		playerId,
		ComsDefenseMessage::eBatteryUse,
		batteryId);
	ComsMessageSender::sendToServer(defenseMessage);
}
