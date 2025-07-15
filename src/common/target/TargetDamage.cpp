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

#include <actions/TargetFalling.h>
#include <actions/TankSay.h>
#include <actions/CameraPositionAction.h>
#include <actions/Resurrection.h>
#ifndef S3D_SERVER
	#include <sprites/TextActionRenderer.h>
#endif
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <events/EventController.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankTeamScore.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tankai/TankAI.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketAccessories.h>
#include <target/TargetDamage.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <target/TargetParachute.h>
#include <target/TargetState.h>
#include <tankai/TankAIStrings.h>
#include <lang/LangResource.h>

void TargetDamage::damageTarget(ScorchedContext &context,
	Weapon *weapon, 
	unsigned int damagedPlayerId, WeaponFireContext &weaponContext,
	fixed damage, bool useShieldDamage, bool checkFall,
	bool shieldOnlyDamage)
{
	if (context.getOptionsGame().getActionSyncCheck())
	{
		context.getSimulator().addSyncCheck(
			S3D::formatStringBuffer("%u %s %s",
				damagedPlayerId, damage.asQuickString(), weapon->getParent()->getName()));
	}

	if (!context.getServerMode()) 
	{
		Target *damagedTarget = 
			context.getTargetContainer().getTargetById(damagedPlayerId);
		if (damagedTarget && damagedTarget->getType() == Target::TypeTank)
		{
			TankViewPointProvider *vPoint = new TankViewPointProvider();
			vPoint->setValues(damagedTarget->getLife().getTargetPosition());
			CameraPositionAction *pos = new CameraPositionAction(
				weaponContext.getPlayerId(),
				vPoint,
				4,
				15,
				false);
			context.getActionController().addAction(pos);
		}
	}

	unsigned int firedPlayerId = weaponContext.getPlayerId();

	// Find the tank that has been damaged
	Target *damagedTarget = 
		context.getTargetContainer().getTargetById(damagedPlayerId);
	if (!damagedTarget || !damagedTarget->getAlive()) return;

	// Tell this tanks ai that is has been hurt by another tank
	if (damagedTarget->getType() != Target::TypeTarget)
	{
		// If on server
		// Tell all AIs about this collision
		if (context.getServerMode())
		{
			std::map<unsigned int, Tanket *> tankets = 
				context.getTargetContainer().getTankets();
			std::map<unsigned int, Tanket *>::iterator itor;
			for (itor = tankets.begin();
				itor != tankets.end();
				++itor)
			{
				Tanket *tanket = (*itor).second;
				TankAI *ai = tanket->getTankAI();
				if (ai)
				{		
					if (tanket->getAlive())
					{
						ai->tankHurt(weapon, damage.asFloat(),
							damagedTarget->getPlayerId(), 
							firedPlayerId);
					}
				}
			}
		}
	}

	// Add the collision action
	addDamageAction(context, weaponContext, damagedTarget, damagedTarget->getCollisionAction());

	// Remove any damage from shield first
	if (damage > 0)
	{
		fixed shieldDamage = 0;
		Accessory *sh = damagedTarget->getShield().getCurrentShield();
		if (sh && useShieldDamage)
		{
			Shield *shield = (Shield *) sh->getAction();
			fixed shieldPowerRequired = 
				damage * shield->getHitPenetration();
			fixed shieldPower = 
				damagedTarget->getShield().getShieldPower();
			if (shieldPower > shieldPowerRequired)
			{
				shieldPower -= shieldPowerRequired;
				damage = 0;
			}
			else
			{
				fixed p = shieldPower / shield->getHitPenetration();
				shieldPower = 0;
				damage -= p;
			}

			damagedTarget->getShield().setShieldPower(shieldPower);
		}
	}

	// Remove the remaining damage from the tank
	if (damage > 0 && !shieldOnlyDamage)
	{
#ifndef S3D_SERVER
		if (!context.getServerMode() &&
			damagedTarget->getTargetState().getDisplayDamage())
		{
			Vector position = damagedTarget->getLife().getFloatPosition();
			position[0] += RAND * 5.0f - 2.5f;
			position[1] += RAND * 5.0f - 2.5f;
			position[2] += RAND * 5.0f - 2.5f;

			Vector redColor(0.75f, 0.0f, 0.0f);
			context.getActionController().addAction(
				new SpriteAction(
					new TextActionRenderer(
						position,
						redColor,
						S3D::formatStringBuffer("%.0f", damage.asFloat()))));
		}
#endif // #ifndef S3D_SERVER

		// Remove the life
		if (damage > damagedTarget->getLife().getLife()) damage = 
			damagedTarget->getLife().getLife();
		damagedTarget->getLife().setLife(damagedTarget->getLife().getLife() - damage);
		if (context.getOptionsGame().getLimitPowerByHealth() &&
			damagedTarget->getType() != Target::TypeTarget)
		{
			Tanket *damagedTank = (Tanket *) damagedTarget;
			damagedTank->getShotInfo().changePower(0, true);
		}

		if (context.getOptionsGame().getActionSyncCheck())
		{
			context.getSimulator().addSyncCheck(
				S3D::formatStringBuffer("TargetDamage: %u %u %s", 
					damagedTarget->getPlayerId(),
					weaponContext.getPlayerId(),
					damagedTarget->getLife().getLife().asQuickString()));
		}

		// Check if the tank is dead
		bool killedTank = (damagedTarget->getLife().getLife() == 0);

		// Add any score got from this endevour
		// Should always be a tank that has fired
		Tank *firedTank = 
			context.getTargetContainer().getTankById(firedPlayerId);
		if (firedTank && damagedTarget->getType() == Target::TypeTank)
		{	
			Tank *damagedTank = (Tank *) damagedTarget;

			// Add this tank as a tank that assisted in the kill
			damagedTank->getScore().getHurtBy().insert(firedTank->getPlayerId());

			// Calculate team kills
			bool selfKill = (damagedPlayerId ==  firedPlayerId);
			bool teamKill = ((context.getOptionsGame().getTeams() > 1) &&
				(firedTank->getTeam() == damagedTank->getTeam()));

			if (!killedTank)
			{
				// Calculate money won for not killing this tank
				int moneyPerHit = 
					context.getOptionsGame().getMoneyWonPerHitPoint() *
						weapon->getArmsLevel();
				if (context.getOptionsGame().getMoneyPerHealthPoint()) 
					moneyPerHit = (moneyPerHit * damage.asInt()) / 100;
				if (selfKill || teamKill) moneyPerHit *= -1;

				firedTank->getScore().setMoney(
					firedTank->getScore().getMoney() + moneyPerHit);
			}
			else 
			{
				int moneyPerKill = 
					context.getOptionsGame().getMoneyWonPerKillPoint() *
						weapon->getArmsLevel();
				if (!selfKill && !teamKill)
				{
					// Note this is done before turn kills is updated
					// so for the first kill turn kills will be 0
					// i.e. no multikill bonus for 1st kill
					moneyPerKill +=
						context.getOptionsGame().getMoneyWonPerMultiKillPoint() *
						weapon->getArmsLevel() *
						weaponContext.getInternalContext().getKillCount();
				}
				if (context.getOptionsGame().getMoneyPerHealthPoint()) 
					moneyPerKill = (moneyPerKill * damage.asInt()) / 100;
				int scorePerKill = context.getOptionsGame().getScorePerKill();

				int moneyPerAssist = 
					context.getOptionsGame().getMoneyWonPerAssistPoint() *
						weapon->getArmsLevel();
				int scorePerAssist = context.getOptionsGame().getScorePerAssist();

				// Update kills and score
				if (selfKill || teamKill)
				{
					firedTank->getScore().setKills(
						firedTank->getScore().getKills() - 1);
					firedTank->getScore().setMoney(
						firedTank->getScore().getMoney() - moneyPerKill);
					firedTank->getScore().setScore(
						firedTank->getScore().getScore() - scorePerKill);

					if (firedTank->getTeam() > 0)
					{
						context.getTankTeamScore().addScore(
							-scorePerKill, firedTank->getTeam());
					}
				}
				else
				{
					firedTank->getScore().setKills(
						firedTank->getScore().getKills() + 1);
					firedTank->getScore().setMoney(
						firedTank->getScore().getMoney() + moneyPerKill);
					firedTank->getScore().setScore(
						firedTank->getScore().getScore() + scorePerKill);

					weaponContext.getInternalContext().setKillCount(
						weaponContext.getInternalContext().getKillCount() + 1);

					if (firedTank->getTeam() > 0)
					{
						context.getTankTeamScore().addScore(
							scorePerKill, firedTank->getTeam());
					}
				}

				// Update assists
				std::set<unsigned int> &hurtBy = 
					damagedTank->getScore().getHurtBy();
				std::set<unsigned int>::iterator itor;
				for (itor = hurtBy.begin();
					itor != hurtBy.end();
					++itor)
				{
					unsigned int hurtByPlayer = (*itor);
					Tank *hurtByTank = 
						context.getTargetContainer().getTankById(hurtByPlayer);
					if (!hurtByTank) continue;

					// Only score when the tank does not hurt itself
					if (hurtByTank == damagedTank) continue;

					// You don't get an assist for your kill
					if (hurtByTank == firedTank) continue;

					// or a team member
					if ((context.getOptionsGame().getTeams() > 1) &&
						(hurtByTank->getTeam() == damagedTank->getTeam())) continue;

					// Update assist score
					hurtByTank->getScore().setAssists(
						hurtByTank->getScore().getAssists() + 1);
					hurtByTank->getScore().setMoney(
						hurtByTank->getScore().getMoney() + moneyPerAssist);
					hurtByTank->getScore().setScore(
						hurtByTank->getScore().getScore() + scorePerAssist);

					if (hurtByTank->getTeam() > 0)
					{
						context.getTankTeamScore().addScore(
							scorePerAssist, hurtByTank->getTeam());
					}
				}
			}
		} 

		if (killedTank)
		{
			// The tank has died, make it blow up etc.
			calculateDeath(context, weaponContext, weapon, damagedPlayerId);

			if (damagedTarget->getType() == Target::TypeTank)
			{
				// The tank is now dead
				Tank *damagedTank = (Tank *) damagedTarget;
				damagedTank->getState().setState(TankState::sDead);

				// This tank has lost a life
				if (damagedTank->getState().getMaxLives() > 0)
				{
					damagedTank->getState().setLives(
						damagedTank->getState().getLives() - 1);
				}
				Resurrection::checkResurection(&context, damagedTank);
			}
		}
	}

	// Check if the tank needs to fall
	if (checkFall && damagedTarget->getAlive())
	{
		// The tank is not dead check if it needs to fall
		FixedVector &position = damagedTarget->getLife().getTargetPosition();
		if (context.getLandscapeMaps().getGroundMaps().
			getInterpHeight(position[0], position[1]) < position[2])
		{
			// Check this tank is not already falling
			if (!damagedTarget->getTargetState().getFalling())
			{
				Parachute *parachute = 0;
				Accessory *paraAccessory = 
					damagedTarget->getParachute().getCurrentParachute();
				if (paraAccessory)
				{
					parachute = (Parachute *) paraAccessory->getAction();
				}

				// Tank falling
				context.getActionController().addAction(
					new TargetFalling(weapon, damagedPlayerId, weaponContext, parachute));
			}
		}
	}

	// DO LAST
	// If the tank is a target, remove the target
	if (!damagedTarget->getAlive() &&
		damagedTarget->getType() != Target::TypeTank)
	{
		Target *removedTarget = 
			context.getTargetContainer().
				removeTarget(damagedTarget->getPlayerId());
		if (context.getOptionsGame().getActionSyncCheck())
		{
			context.getSimulator().addSyncCheck(
				S3D::formatStringBuffer("RemoveTarget : %u %s", 
					removedTarget->getPlayerId(),
					removedTarget->getCStrName().c_str()));
		}

		delete removedTarget;
	}
}

void TargetDamage::calculateDeath(ScorchedContext &context, WeaponFireContext &weaponContext, 
	Weapon *weapon, unsigned int damagedPlayerId)
{
	Target *killedTarget = 
		context.getTargetContainer().getTargetById(damagedPlayerId);
	if (!killedTarget) return;

	// Log the death
	logDeath(context, weaponContext, weapon, damagedPlayerId);

	// Add the tank death explosion
	// Make the tank explode in one of many ways
	addDamageAction(context, weaponContext, killedTarget, killedTarget->getDeathAction());
}

void TargetDamage::addDamageAction(ScorchedContext &context, WeaponFireContext &originalWeaponContext, 
	Target *target, Accessory *accessory) 
{
	if (accessory)
	{
		Weapon *weapon = (Weapon *) accessory->getAction();
		if (context.getOptionsGame().getActionSyncCheck())
		{
			context.getSimulator().addSyncCheck(
				S3D::formatStringBuffer("DeathAction: %s", 
					weapon->getParent()->getName()));
		}

		FixedVector position = target->getLife().getTargetPosition();
		FixedVector velocity;
		WeaponFireContext newWeaponContext(originalWeaponContext.getPlayerId(), 
			originalWeaponContext.getInternalContext().getSelectPositionX(), 
			originalWeaponContext.getInternalContext().getSelectPositionY(), 
			originalWeaponContext.getInternalContext().getVelocityVector(),
			originalWeaponContext.getInternalContext().getReferenced(), 
			false);
		weapon->fire(context, newWeaponContext, position, velocity);

		if (target->getType() != Target::TypeTarget) 
		{
			Tanket *tanket = (Tanket *) target;
			if (accessory->getUseNumber() > 0)
			{
				// Actually use up one of the weapons
				// Fuel, is used up differently at the rate of one weapon per movement square
				// This is done sperately in the tank movement action
				tanket->getAccessories().rm(accessory, accessory->getUseNumber());
			}
		}
	}
}

void TargetDamage::logDeath(ScorchedContext &context, WeaponFireContext &weaponContext,
	Weapon *weapon, unsigned int damagedPlayerId)
{
	unsigned int firedPlayerId = weaponContext.getPlayerId();

	Target *killedTarget = 
		context.getTargetContainer().getTargetById(damagedPlayerId);
	if (killedTarget->getType() != Target::TypeTank) return;

	Tank *killedTank = (Tank *) killedTarget;

	if (killedTank->getDestinationId() == 0)
	{
		const char *line = context.getTankAIStrings().getDeathLine(context);
		if (line)
		{
			context.getActionController().addAction(
				new TankSay(killedTank->getPlayerId(), 
				LANG_STRING(line)));
		}
	}

	Tank *firedTank = 0;
	if (firedPlayerId != 0) firedTank = context.getTargetContainer().getTankById(firedPlayerId);
	else
	{
		Vector white(1.0f, 1.0f, 1.0f);
		static Tank envTank(context, 0, 0, 
			LANG_STRING("Environment"), 
			white);
		envTank.setUniqueId("Environment");
		firedTank = &envTank;
	}

	if (firedTank)
	{
		if (damagedPlayerId == firedPlayerId)
		{
			int skillChange = context.getOptionsGame().getSkillForSelfKill();
			fixed weight = (fixed(weapon->getArmsLevel()) / 10) + 1;
			skillChange = (fixed(skillChange) * weight).asInt();

			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + skillChange);

			context.getEventController().tankSelfKilled(firedTank, weapon);
			{
				ChannelText text("combat",
					LANG_RESOURCE_3(
						"TANK_KILLED_SELF", 
						"[p:{0}] killed self with a [w:{1}] ({2} skill)",
						firedTank->getTargetName(),
						weapon->getParent()->getName(),
						S3D::formatStringBuffer("%i", skillChange)));
				ChannelManager::showText(context, text);
			}
		}
		else if ((context.getOptionsGame().getTeams() > 1) &&
				(firedTank->getTeam() == killedTank->getTeam())) 
		{
			int skillChange = context.getOptionsGame().getSkillForTeamKill();
			fixed weight = (fixed(weapon->getArmsLevel()) / 10) + 1;
			skillChange = (fixed(skillChange) * weight).asInt();

			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + skillChange);

			context.getEventController().tankTeamKilled(firedTank, killedTank, weapon);
			{
				ChannelText text("combat", 
					LANG_RESOURCE_4(
						"TANK_KILLED_TEAM",
						"[p:{0}] team killed [p:{1}] with a [w:{2}] ({3} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon->getParent()->getName(),
						S3D::formatStringBuffer("%i", skillChange)));
				ChannelManager::showText(context, text);
			}
		}
		else
		{
			int kbonus = 0; // Killer
			int vbonus = 0; // Victim
			if (firedTank->getPlayerId() != 0 && killedTank->getPlayerId() != 0) 
			{
				int kskill = firedTank->getScore().getSkill();
				int vskill = killedTank->getScore().getSkill();

				if (kskill > vskill) 
				{
					// killer is better than the victim
					kbonus = ((kskill + vskill)*(kskill + vskill)) / (kskill*kskill);
					vbonus = kbonus * vskill / (vskill + kskill);
				} 
				else 
				{
					// the victim is better than the killer
					kbonus = ((vskill + kskill)*(vskill + kskill)) / (vskill*vskill) * vskill / kskill;
					vbonus = kbonus * (vskill + 1000) / (vskill + kskill);
				}

				if (vbonus > context.getOptionsGame().getMaxSkillLost())
				{
					vbonus = context.getOptionsGame().getMaxSkillLost();
				}
				if (kbonus > context.getOptionsGame().getMaxSkillGained())
				{
					kbonus = context.getOptionsGame().getMaxSkillGained();
				}

				//$vbonus = $vskill if $vbonus > $vskill;
				//$kbonus = $kskill if $kbonus > $kskill;
				fixed weight = (fixed(weapon->getArmsLevel()) / 10) + 5;
				kbonus = (fixed(kbonus) * weight).asInt();
				vbonus = (fixed(vbonus) * weight).asInt();
			}
			firedTank->getScore().setSkill(firedTank->getScore().getSkill() + kbonus);
			killedTank->getScore().setSkill(killedTank->getScore().getSkill() - vbonus);

			context.getEventController().tankKilled(firedTank, killedTank, weapon);
			{
				if (weaponContext.getInternalContext().getKillCount() > 1)
				{
					ChannelText text("combat", 
						LANG_RESOURCE_5(
						"TANK_KILLED_MULTIOTHER",
						"[p:{0}] multi-killed [p:{1}] with a [w:{2}] ({3}, {4} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon->getParent()->getName(),
						S3D::formatStringBuffer("%i", kbonus),
						S3D::formatStringBuffer("%i", -vbonus)));
					ChannelManager::showText(context, text);
				}
				else
				{
					ChannelText text("combat", 
						LANG_RESOURCE_5(
						"TANK_KILLED_OTHER",
						"[p:{0}] killed [p:{1}] with a [w:{2}] ({3}, {4} skill)",
						firedTank->getTargetName(),
						killedTank->getTargetName(),
						weapon->getParent()->getName(),
						S3D::formatStringBuffer("%i", kbonus),
						S3D::formatStringBuffer("%i", -vbonus)));
					ChannelManager::showText(context, text);
				}
			}
		}
	}
}
