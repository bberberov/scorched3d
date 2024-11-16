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

#if !defined(AFX_TanketAccessories_H__60850A18_DED2_4BB0_B104_CB0941EF6D1D__INCLUDED_)
#define AFX_TanketAccessories_H__60850A18_DED2_4BB0_B104_CB0941EF6D1D__INCLUDED_

#include <weapons/AccessoryPart.h>
#include <tanket/TanketWeapon.h>
#include <tanket/TanketAutoDefense.h>
#include <tanket/TanketBatteries.h>
#include <lang/LangString.h>

class Tanket;
class ScorchedContext;
class TanketAccessories  
{
public:
	TanketAccessories(ScorchedContext &context);
	virtual ~TanketAccessories();

	void setTanket(Tanket *tanket);

	void newMatch();
	void add(Accessory *accessory, int count, bool check = true);
	void rm(Accessory *accessory, int count);
	void clearAccessories();

	bool canUse(Accessory *accessory);

	bool accessoryAllowed(Accessory *accessory, int count);
	int getAccessoryCount(Accessory *accessory);
	void getAllAccessories(std::list<Accessory *> &result);
	std::list<Accessory *> &getAllAccessoriesByType(
		AccessoryPart::AccessoryType type);
	std::list<Accessory *> &getAllAccessoriesByGroup(
		const char *groupName);

	TanketWeapon &getWeapons() { return tankWeapon_; }
	TanketAutoDefense &getAutoDefense() { return tankAuto_; }
	TanketBatteries &getBatteries() { return tankBatteries_; }

	LangString getAccessoryCountString(Accessory *accessory);
	LangString getAccessoryAndCountString(Accessory *accessory);

	// Serialize the tank accessories
	bool writeMessage(NamedNetBuffer &buffer, bool writeAccessories);
	bool readMessage(NetBufferReader &reader);

protected:
	ScorchedContext &context_;
	TanketWeapon tankWeapon_;
	TanketAutoDefense tankAuto_;
	TanketBatteries tankBatteries_;
	Tanket *tanket_;

	typedef std::list<Accessory *> AccessoryList;
	std::map<Accessory *, int> accessories_;
	std::map<std::string, AccessoryList*> accessoryGroups_;
	std::map<AccessoryPart::AccessoryType, AccessoryList*> accessoryTypes_;

	void changed();
	void add_(Accessory *accessory, int count, bool check);
};

#endif // !defined(AFX_TanketAccessories_H__60850A18_DED2_4BB0_B104_CB0941EF6D1D__INCLUDED_)
