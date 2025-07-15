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

#if !defined(AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
#define AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_

#include <map>
#include <string>

#include <engine/ScorchedContext.h>

class OptionsScorched;
class AccessoryStore;
class WeaponLabel;
class AccessoryCreateContext  
{
public:
	AccessoryCreateContext(ScorchedContext &context);
	virtual ~AccessoryCreateContext();

	ScorchedContext &getScorchedContext() { return context_; }
	OptionsScorched &getOptionsGame() { return context_.getOptionsGame(); }
	AccessoryStore &getAccessoryStore() { return context_.getAccessoryStore(); }
	WeaponLabel *getLabel(const char *label);
	void addLabel(const char *label, WeaponLabel *weapon);
	void removeLabel(const char *label);

protected:
	ScorchedContext &context_;
	std::map<std::string, WeaponLabel*> labels_;

private:
	AccessoryCreateContext(const AccessoryCreateContext&);
	AccessoryCreateContext &operator=(const AccessoryCreateContext &other);
};

#endif // !defined(AFX_ACCESSORYCREATECONTEXT_H__21765D5B_DB45_4275_AB63_BAD1E84C1790__INCLUDED_)
