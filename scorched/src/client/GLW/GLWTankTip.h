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

#ifndef _gLWTankTip_h
#define _gLWTankTip_h

#include <GLW/GLWToolTip.h>
#include <GLW/GLWSelector.h>
#include <tank/Tank.h>

class TankUndoMenu : public GLWSelectorI
{
public:
	TankUndoMenu(Tank *tank);
	virtual ~TankUndoMenu();

	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankFuelTip : public ToolTip, public GLWSelectorI
{
public:
	TankFuelTip(Tank *tank);
	virtual ~TankFuelTip();

	virtual void populate();
	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankBatteryTip : public ToolTip, public GLWSelectorI
{
public:
	TankBatteryTip(Tank *tank);
	virtual ~TankBatteryTip();

	virtual void populate();
	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankShieldTip : public ToolTip, public GLWSelectorI
{
public:
	TankShieldTip(Tank *tank);
	virtual ~TankShieldTip();

	virtual void populate();
	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankHealthTip : public ToolTip
{
public:
	TankHealthTip(Tank *tank);
	virtual ~TankHealthTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TankRankTip : public ToolTip
{
public:
	TankRankTip(Tank *tank);
	virtual ~TankRankTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TankParachutesTip : public ToolTip, public GLWSelectorI
{
public:
	TankParachutesTip(Tank *tank);
	virtual ~TankParachutesTip();

	virtual void populate();
	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankAutoDefenseTip : public ToolTip
{
public:
	TankAutoDefenseTip(Tank *tank);
	virtual ~TankAutoDefenseTip();

	void showItems(float x, float y);
	virtual void populate();
protected:
	Tank *tank_;
};

class TankWeaponTip : public ToolTip, public GLWSelectorI
{
public:
	TankWeaponTip(Tank *tank);
	virtual ~TankWeaponTip();

	virtual void populate();
	void showItems(float x, float y);
	virtual void itemSelected(GLWSelectorEntry *entry, int position);
protected:
	Tank *tank_;
};

class TankPowerTip : public ToolTip
{
public:
	TankPowerTip(Tank *tank);
	virtual ~TankPowerTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TankRotationTip : public ToolTip
{
public:
	TankRotationTip(Tank *tank);
	virtual ~TankRotationTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TankElevationTip : public ToolTip
{
public:
	TankElevationTip(Tank *tank);
	virtual ~TankElevationTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TankTip : public ToolTip
{
public:
	TankTip(Tank *tank);
	virtual ~TankTip();

	virtual void populate();
protected:
	Tank *tank_;
};

class TargetTip : public ToolTip
{
public:
	TargetTip(Target *target);
	virtual ~TargetTip();

	virtual void populate();
protected:
	Target *target_;
};

class GLWTargetTips
{
public:
	GLWTargetTips(Target *target);
	virtual ~GLWTargetTips();

	TargetTip targetTip;

private:
	GLWTargetTips(const GLWTargetTips &);
	const GLWTargetTips & operator=(const GLWTargetTips &);
};

class GLWTankTips 
{
public:
	GLWTankTips(Tank *tank);
	virtual ~GLWTankTips();

	TankTip tankTip;
	ToolTip nameTip;
	TankUndoMenu undoMenu;
	TankWeaponTip weaponTip;
	TankPowerTip powerTip;
	TankRotationTip rotationTip;
	TankElevationTip elevationTip;
	TankAutoDefenseTip autodTip;
	TankParachutesTip paraTip;
	TankHealthTip healthTip;
	TankShieldTip shieldTip;
	TankBatteryTip batteryTip;
	TankFuelTip fuelTip;
	TankRankTip rankTip;

private:
	GLWTankTips(const GLWTankTips &);
	const GLWTankTips & operator=(const GLWTankTips &);
};

#endif // _gLWTankTip_h

