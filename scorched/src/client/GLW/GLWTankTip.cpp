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

#include <GLW/GLWTankTip.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <tanket/TanketAccessories.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankShotHistory.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <weapons/Weapon.h>
#include <weapons/Shield.h>
#include <weapons/AccessoryStore.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <lang/LangResource.h>

TankUndoMenu::TankUndoMenu(Tank *tank) :
	tank_(tank)
{
}

TankUndoMenu::~TankUndoMenu()
{
}

void TankUndoMenu::showItems(float x, float y)
{
	static ToolTip useTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("UNDO", "Undo"), 
		LANG_RESOURCE("UNDO_TOOLTIP", "Reverts back to the selected rotation,\n"
		"elevtaion and power."));

	std::list<GLWSelectorEntry> entries;
	std::vector<TankShotHistory::ShotEntry> &oldShots =
		tank_->getShotHistory().getOldShots();
	for (int i=0; i<(int) oldShots.size(); i++)
	{
		char buffer[128];
		snprintf(buffer, 128, "%s%i: Pwr:%.1f Ele:%.1f Rot:%.1f",
			(oldShots[i].current?"* ":"  "),
			i, oldShots[i].power.asFloat(), oldShots[i].ele.asFloat(),
			(360.0f - oldShots[i].rot.asFloat()));
		entries.push_back(
			GLWSelectorEntry(LANG_STRING(buffer), &useTip, 0, 0, (void *) 
				((unsigned int) (oldShots.size() - 1 - i))));
	}

	GLWSelector::instance()->showSelector(
		this, x, y, entries,
		ClientState::StatePlaying);
}

void TankUndoMenu::itemSelected(GLWSelectorEntry *entry, int position)
{
	tank_->getShotHistory().revertSettings((unsigned long) entry->getUserData());
}

TankFuelTip::TankFuelTip(Tank *tank) : 
	tank_(tank)
{
}

TankFuelTip::~TankFuelTip()
{
}

void TankFuelTip::populate()
{
	LangString fuelCount = LANG_RESOURCE("OFF", "Off");
	Accessory *weapon = tank_->getAccessories().getWeapons().getCurrent();
	if (weapon &&
		weapon->getPositionSelect() == Accessory::ePositionSelectFuel)
	{
		 fuelCount = tank_->getAccessories().getAccessoryAndCountString(weapon);
	}

	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("FUEL", "Fuel"), 
		LANG_RESOURCE("FUEL_TOOLTIP",
		"Allows the tank to move.\n"
		"Click to toggle movement mode.\n"
		"Current Fuel : ") + fuelCount);
}

void TankFuelTip::showItems(float x, float y)
{
	static ToolTip offTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("FUEL_OFF", "Fuel Off"), 
		LANG_RESOURCE("FUEL_OFF_TOOLTIP", 
		"Don't select fuel or\n"
		"turn off any fuel."));

	std::list<GLWSelectorEntry> entries;
	std::list<Accessory *> &fuels =
		tank_->getAccessories().getAllAccessoriesByGroup("fuel");
	ScorchedClient::instance()->getAccessoryStore().sortList(
		fuels,
		OptionsDisplay::instance()->getAccessorySortKey());
	std::list<Accessory *>::iterator itor;
	for (itor = fuels.begin();
		itor != fuels.end();
		++itor)
	{
		Accessory *current = (*itor);
		if (tank_->getAccessories().canUse(current))
		{
			entries.push_back(GLWSelectorEntry(
				tank_->getAccessories().getAccessoryAndCountString(current), 
				&current->getToolTip(), 
				(tank_->getAccessories().getWeapons().getCurrent() == current), 
				&current->getTexture(), current));
		}
	}
	entries.push_back(GLWSelectorEntry(LANG_RESOURCE("OFF", "Off"), &offTip, 0, 0, 0));
	GLWSelector::instance()->showSelector(this, x, y, entries,
		ClientState::StatePlaying);
}

void TankFuelTip::itemSelected(GLWSelectorEntry *entry, int position)
{
	Accessory *accessory = ((Accessory *)entry->getUserData());
	if (accessory)
	{
		tank_->getAccessories().getWeapons().setWeapon(accessory);
	}
	else
	{
		std::list<Accessory *> &entries =
			tank_->getAccessories().getAllAccessoriesByGroup("weapon");
		if (!entries.empty())
		{
			tank_->getAccessories().getWeapons().setWeapon(entries.front());
		}
		else
		{
			tank_->getAccessories().getWeapons().setWeapon(0);
		}
	}
}

TankBatteryTip::TankBatteryTip(Tank *tank) : 
	tank_(tank)
{
}

TankBatteryTip::~TankBatteryTip()
{
}

void TankBatteryTip::populate()
{
	LangString batteryCount = LANG_RESOURCE("INF", "Inf");

	int count = tank_->getAccessories().getBatteries().getNoBatteries();
	if (count >= 0)
	{
		batteryCount = LANG_STRING(S3D::formatStringBuffer("%i", count));
	}

	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("BATTERIES", "Batteries"),
		LANG_RESOURCE("BATTERIES_TOOLTIP", 
		"Can be used to recharge life.\n"
		"Each battery gives back 10 life.\n"
		"Click to use some battery(s).\n"
		"Batteries : ") + batteryCount);
}

void TankBatteryTip::showItems(float x, float y)
{
	static ToolTip useTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("BATTERY", "Battery"), 
		LANG_RESOURCE("BATTERY_TOOLTIP", "Use some batteries"));
	static ToolTip offTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("BATTERY_CANCEL", "Battery Cancel"), 
		LANG_RESOURCE("BATTERY_CANCEL_TOOLTIP", "Don't use any batteries"));
	
	int count = tank_->getAccessories().getBatteries().getNoBatteries();
	if (count == -1) count = 10;

	std::list<GLWSelectorEntry> entries;
	if (tank_->getAccessories().getBatteries().canUse())
	{
		for (int i=1; i<=MIN(count,10); i++)
		{
			entries.push_back(GLWSelectorEntry(
				LANG_RESOURCE_1("USE_I", "Use {0}", S3D::formatStringBuffer("%i", i)), 
				&useTip, 0, 0, (void *) i));
		}
	}
	entries.push_back(GLWSelectorEntry(LANG_RESOURCE("CANCEL", "Cancel"), &offTip, 0, 0, (void *) 0));
	GLWSelector::instance()->showSelector(this, x, y, entries,
		ClientState::StatePlaying);		
}

void TankBatteryTip::itemSelected(GLWSelectorEntry *entry, int position)
{
	for (int i=1; i<=(long) entry->getUserData(); i++)
	{
		if (tank_->getLife().getLife() < 
			tank_->getLife().getMaxLife())
		{
			std::list<Accessory *> &entries =
				tank_->getAccessories().getAllAccessoriesByType(
					AccessoryPart::AccessoryBattery);
			if (!entries.empty())
			{

				TankKeyboardControlUtil::useBattery(tank_->getPlayerId(),
					entries.front()->getAccessoryId());
			}
		}
	}
}

TankShieldTip::TankShieldTip(Tank *tank) : 
	tank_(tank)
{
}

TankShieldTip::~TankShieldTip()
{
}

void TankShieldTip::showItems(float x, float y)
{
	static ToolTip offTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SHIELD_OFF", "Shield Off"), 
		LANG_RESOURCE("SHIELD_OFF_TOOLTIP", 
		"Don't select a shield or\n"
		"turn off any current shield"));

	Accessory *currentShield = 
		tank_->getShield().getCurrentShield();
	std::list<GLWSelectorEntry> entries;
	std::list<Accessory *> &shields = 
		tank_->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryShield);
	ScorchedClient::instance()->getAccessoryStore().sortList(
		shields, 
		OptionsDisplay::instance()->getAccessorySortKey());
	std::list<Accessory *>::iterator itor;
	for (itor = shields.begin();
		itor != shields.end();
		++itor)
	{
		Accessory *current = (*itor);
		if (tank_->getAccessories().canUse(current))
		{
			entries.push_back(GLWSelectorEntry(
				tank_->getAccessories().getAccessoryAndCountString(current), 
				&current->getToolTip(), 
				(currentShield == current), &current->getTexture(), current));
		}
	}
	entries.push_back(GLWSelectorEntry(LANG_RESOURCE("OFF", "Off"), &offTip, 0, 0, 0));
	GLWSelector::instance()->showSelector(this, x, y, entries,
		ClientState::StatePlaying);
}

void TankShieldTip::populate()
{
	LangString shieldsCount = LANG_RESOURCE("OFF", "Off");

	if (tank_->getShield().getCurrentShield())
	{
		char buffer[128];
		int count = tank_->getAccessories().getAccessoryCount(
			tank_->getShield().getCurrentShield());
		if (count >= 0) snprintf(buffer, 128, "%i", count);
		else snprintf(buffer, 128, "Infinite");

		shieldsCount = tank_->getAccessories().getAccessoryAndCountString(
			tank_->getShield().getCurrentShield());
		shieldsCount.append(LANG_STRING("\n"));
		shieldsCount.append(
			LANG_RESOURCE_1("SHIELD_POWER", "Shield Power : {0}", 
			S3D::formatStringBuffer("%.0f", tank_->getShield().getShieldPower().asFloat())));
	}

	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SHIELDS", "Shields"), 
		LANG_RESOURCE("SHIELDS_TOOLTIP",
		"Protect the tank from taking shot damage.\n"
		"Shields must be enabled before they take\n"
		"effect.\n"
		"Click to enable/disable shields.\n"
		"Current Shield : ") + shieldsCount);
}

void TankShieldTip::itemSelected(GLWSelectorEntry *entry, int position)
{
	if (entry->getUserData() == 0) 
		TankKeyboardControlUtil::shieldsUpDown(tank_->getPlayerId(), 0);
	else 
		TankKeyboardControlUtil::shieldsUpDown(tank_->getPlayerId(),
			((Accessory *)entry->getUserData())->getAccessoryId());
}

TankRankTip::TankRankTip(Tank *tank) : 
	tank_(tank)
{
}

TankRankTip::~TankRankTip()
{
}

void TankRankTip::populate()
{
	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("RANK", "Rank"),
		LANG_RESOURCE("RANK_TOOLTIP",
		"The current online ranking of this player"));
}

TankHealthTip::TankHealthTip(Tank *tank) : 
	tank_(tank)
{
}

TankHealthTip::~TankHealthTip()
{
}

void TankHealthTip::populate()
{
	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("LIFE", "Life"),
		LANG_RESOURCE_2("LIFE_TOOLTIP",
		"The amount of life this player has.\n"
		"The tank explodes when life reaches 0.\n"
		"Less weapon power is available with less life.\n"
		"Life : {0}/{1}",
		S3D::formatStringBuffer("%.0f", tank_->getLife().getLife().asFloat()),
		S3D::formatStringBuffer("%.0f", tank_->getLife().getMaxLife().asFloat())));
}

TankParachutesTip::TankParachutesTip(Tank *tank) : 
	tank_(tank)
{
}

TankParachutesTip::~TankParachutesTip()
{
}

void TankParachutesTip::populate()
{
	LangString parachuteCount = LANG_RESOURCE("OFF", "Off");

	if (tank_->getParachute().getCurrentParachute())
	{
		parachuteCount = tank_->getAccessories().getAccessoryAndCountString(
			tank_->getParachute().getCurrentParachute());
	}

	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("PARACHUTES", "Parachutes"), 
		LANG_RESOURCE("PARACHUTES_TOOLTIP", 
		"Prevents the tank from taking damage\n"
		"when falling.  Must be enabled before\n"
		"they take effect.\n"
		"Click to enable/disable parachutes.\n"
		"Current Parachute : ") + parachuteCount);
}

void TankParachutesTip::showItems(float x, float y)
{
	static ToolTip offTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("PARACHUTES_OFF", "Parachutes Off"), 
		LANG_RESOURCE("PARACHUTES_OFF_TOOLTIP", 
		"Don't select a parachute or\n"
		"turn off any current parachute"));

	Accessory *currentParachute = 
		tank_->getParachute().getCurrentParachute();
	std::list<GLWSelectorEntry> entries;
	std::list<Accessory *> &parachutes =
		tank_->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryParachute);
	ScorchedClient::instance()->getAccessoryStore().sortList(
		parachutes, 
		OptionsDisplay::instance()->getAccessorySortKey());

	std::list<Accessory *>::iterator itor;
	for (itor = parachutes.begin();
		itor != parachutes.end();
		++itor)
	{
		Accessory *current = (*itor);
		if (tank_->getAccessories().canUse(*itor))
		{
			entries.push_back(GLWSelectorEntry(
				tank_->getAccessories().getAccessoryAndCountString(current), 
				&current->getToolTip(), 
				(currentParachute == current), &current->getTexture(), current));
		}
	}
	entries.push_back(GLWSelectorEntry(LANG_RESOURCE("OFF", "Off"), &offTip, 0, 0, 0));
	GLWSelector::instance()->showSelector(this, x, y, entries,
		ClientState::StatePlaying);
}

void TankParachutesTip::itemSelected(GLWSelectorEntry *entry, int position)
{
	if (entry->getUserData() == 0) 
		TankKeyboardControlUtil::parachutesUpDown(tank_->getPlayerId(), 0);
	else 
		TankKeyboardControlUtil::parachutesUpDown(tank_->getPlayerId(),
		((Accessory *)entry->getUserData())->getAccessoryId());
}

TankAutoDefenseTip::TankAutoDefenseTip(Tank *tank) : 
	tank_(tank)
{
}

TankAutoDefenseTip::~TankAutoDefenseTip()
{
}

void TankAutoDefenseTip::populate()
{
	LangString status = 
		tank_->getAccessories().getAutoDefense().haveDefense()?
		LANG_RESOURCE("ON", "On"):
		LANG_RESOURCE("OFF_NOT_BOUGHT", "Off (Not Bought)");


	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("AUTO_DEFENSE", "Auto Defense"), 
		LANG_RESOURCE("AUTO_DEFENSE_TOOLTIP",
			"Allows the tank to raise shields and\n"
			"activate parachutes before the round\n"
			"starts.\n"
			"Click to see auto defense status.\n"
			"Status : ") + status);
}

void TankAutoDefenseTip::showItems(float x, float y)
{
	static ToolTip useTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("AUTO_DEFENSE_ON", "Auto Defense On"), 
		LANG_RESOURCE("AUTO_DEFENSE_ENABLE", "Enable the auto defense."));
	static ToolTip offTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("AUTO_DEFENSE_OFF", "Auto Defense Off"), 
		LANG_RESOURCE("AUTO_DEFENSE_DISABLE", "Disable the auto defense."));
	
	std::list<GLWSelectorEntry> entries;
	if (tank_->getAccessories().getAutoDefense().haveDefense()) 
		entries.push_back(GLWSelectorEntry(LANG_RESOURCE("ON", "On"), &useTip));
	else entries.push_back(GLWSelectorEntry(LANG_RESOURCE("OFF", "Off"), &offTip));
	GLWSelector::instance()->showSelector(0, x, y, entries,
		ClientState::StatePlaying);
}

TankWeaponTip::TankWeaponTip(Tank *tank) : 
	tank_(tank)
{
}

TankWeaponTip::~TankWeaponTip()
{
}

void TankWeaponTip::populate()
{
	LangString weapon = LANG_RESOURCE("NONE", "None");
	if (tank_->getAccessories().getWeapons().getCurrent())
	{
		weapon = tank_->getAccessories().getAccessoryAndCountString(
			tank_->getAccessories().getWeapons().getCurrent());
		weapon.append(LANG_STRING("\n"));
		weapon.append(LANG_RESOURCE_1("DESCRIPTION", "Description : {0}", 
			tank_->getAccessories().getWeapons().getCurrent()->getDescription()));
	}

	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("WEAPON", "Weapon"), 
		LANG_RESOURCE("WEAPON_TOOLTIP", 
		"The currently selected weapon.\n"
		"Click to change weapon.\n"
		"Weapon : ") + weapon);
}

void TankWeaponTip::showItems(float x, float y)
{
	std::list<GLWSelectorEntry> entries;

	Accessory *currentWeapon = 
		tank_->getAccessories().getWeapons().getCurrent();
	std::list<Accessory *> &weapons =
		tank_->getAccessories().getAllAccessoriesByGroup("weapon");
	ScorchedClient::instance()->getAccessoryStore().sortList(
		weapons,
		OptionsDisplay::instance()->getAccessorySortKey());
	std::list<Accessory *>::iterator itor;
	for (itor = weapons.begin();
		itor != weapons.end();
		++itor)
	{
		Accessory *weapon = (*itor);
		if (tank_->getAccessories().canUse(weapon))
		{
			GLWSelectorEntry newEntry(
				tank_->getAccessories().getAccessoryAndCountString(weapon), 
				&weapon->getToolTip(), 
				(currentWeapon == weapon), &weapon->getTexture(), weapon);
			entries.push_back(newEntry);
		}
	}
	GLWSelector::instance()->showSelector(this, x, y, entries,
		ClientState::StatePlaying);
}

void TankWeaponTip::itemSelected(GLWSelectorEntry *entry, int position)
{
	tank_->getAccessories().getWeapons().setWeapon((Accessory *) entry->getUserData());
}

TankPowerTip::TankPowerTip(Tank *tank) : 
	tank_(tank)
{
}

TankPowerTip::~TankPowerTip()
{
}

void TankPowerTip::populate()
{
	{
		setText(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("POWER", "Power"), 
			LANG_RESOURCE_1("POWER_TOOLTIP", 
			"The power used to fire the current weapon.\n"
			"Click to revert back to previous settings.\n"
			"Power : {0}",
			tank_->getShotHistory().getPowerString()));
	}
}

TankRotationTip::TankRotationTip(Tank *tank) : 
	tank_(tank)
{
}

TankRotationTip::~TankRotationTip()
{
}

void TankRotationTip::populate()
{
	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("ROTATION", "Rotation"), 
		LANG_RESOURCE_1("ROTATION_TOOLTIP", 
		"The rotation of the current player's tank turret.\n"
		"Click to revert back to previous settings.\n"
		"Rotation : {0}",
		tank_->getShotHistory().getRotationString()));
}

TankElevationTip::TankElevationTip(Tank *tank) : 
	tank_(tank)
{
}

TankElevationTip::~TankElevationTip()
{
}

void TankElevationTip::populate()
{
	setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("ELEVATION", "Elevation"), 
		LANG_RESOURCE_1("ELEVATION_TOOLTIP", 
		"The elevation of the current player's gun.\n"
		"Click to revert back to previous settings.\n"
		"Elevation : {0}",
		tank_->getShotHistory().getElevationString()));
}

static void generateTargetTip(LangString &tip, Target *target)
{
	tip.append(LANG_RESOURCE_2("TARGET_LIFE", "Life   : {0}/{1}",
		S3D::formatStringBuffer("%.0f", target->getLife().getLife().asFloat()),
		S3D::formatStringBuffer("%.0f", target->getLife().getMaxLife().asFloat())));

	Accessory *shieldAccessory = target->getShield().getGraphicalCurrentShield();
	if (shieldAccessory)
	{
		Shield *shield = (Shield*) shieldAccessory->getAction();

		tip.append(LANG_RESOURCE_2("TARGET_SHIELD", "\nShield   : {0}/{1}",
			S3D::formatStringBuffer("%.0f", target->getShield().getGraphicalShieldPower().asFloat()),
			S3D::formatStringBuffer("%.0f", shield->getPower().asFloat())));
	}
	if (target->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) target;

		if (tank->getState().getState() != TankState::sNormal)
		{
			tip.append(LANG_RESOURCE_1("TARGET_STATE",
				"\nState : {0}",
				tank->getState().getSmallStateString()));;
		}

		tip.append(LANG_RESOURCE_2("TARGET_LIVES", "\nLives   : {0}/{1}",
			S3D::formatStringBuffer("%i", tank->getState().getLives()),
			S3D::formatStringBuffer("%i", tank->getState().getMaxLives())));

		tip.append(LANG_RESOURCE_1("TARGET_SCORE", "\nScore   : {0}",
			S3D::formatStringBuffer("%i", tank->getScore().getScore())));
	}
}

TankTip::TankTip(Tank *tank) : 
	tank_(tank)
{
}

TankTip::~TankTip()
{
}

void TankTip::populate()
{
	LangString tip;
	generateTargetTip(tip, tank_);

	if (tank_->getScore().getSkill() > 0)
	{
		tip.append(LANG_RESOURCE_2("TANK_SKILL", "\nSkill   : {0} ({1})",
			S3D::formatStringBuffer("%i", tank_->getScore().getSkill()),
			S3D::formatStringBuffer("%i", tank_->getScore().getStartSkill())));
	}

	if (tank_->getScore().getRank() > 0) 
	{
		tip.append(LANG_RESOURCE_1("TANK_RANK",  "\nRank    : {0}",
			S3D::formatStringBuffer("%i", tank_->getScore().getRank())));
	}

	setText(ToolTip::ToolTipInfo, tank_->getTargetName(), tip.c_str());
}

TargetTip::TargetTip(Target *target) : 
	target_(target)
{
}

TargetTip::~TargetTip()
{
}

void TargetTip::populate()
{
	LangString tip;
	generateTargetTip(tip, target_);
	setText(ToolTip::ToolTipInfo, target_->getTargetName(), tip);
}

GLWTargetTips::GLWTargetTips(Target *target) : 
	targetTip(target)
{
}

GLWTargetTips::~GLWTargetTips()
{
}

GLWTankTips::GLWTankTips(Tank *tank) : 
	tankTip(tank),
	undoMenu(tank),
	rotationTip(tank),
	elevationTip(tank),
	powerTip(tank),
	weaponTip(tank),
	autodTip(tank),
	paraTip(tank),
	healthTip(tank),
	shieldTip(tank),
	batteryTip(tank),
	fuelTip(tank),
	rankTip(tank),
	nameTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("PLAYER_NAME", "Player Name"),
		LANG_RESOURCE("PLAYER_CURRENTLY_PLAYING", 
		"Shows the name of the player currently\n"
		"making their move."))
{
}

GLWTankTips::~GLWTankTips()
{
}

