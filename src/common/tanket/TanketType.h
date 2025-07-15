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

#if !defined(__INCLUDE_TanketTypeh_INCLUDE__)
#define __INCLUDE_TanketTypeh_INCLUDE__

#include <common/fixed.h>
#include <string>
#include <map>
#include <set>

class ToolTip;
class XMLNode;
class Accessory;
class ScorchedContext;
class TanketType
{
public:
	TanketType();
	virtual ~TanketType();

	bool getUseAsDefault() { return default_; }

	const char *getName() { return name_.c_str(); }
	ToolTip *getTooltip() { return tooltip_; }
	fixed getLife() { return life_; }
	fixed getPower() { return power_; }

	std::map<Accessory *, int> &getAccessories() { return accessories_; }
	bool getAccessoryDisabled(Accessory *accessory);

	bool initFromXML(ScorchedContext &context, XMLNode *node);

protected:
	bool default_;
	ToolTip *tooltip_;
	std::string name_, description_;
	std::map<Accessory *, int> accessories_;
	std::set<Accessory *> disabledAccessories_;
	fixed life_;
	fixed power_;

	void formTooltip();
};

#endif // __INCLUDE_TanketTypeh_INCLUDE__
