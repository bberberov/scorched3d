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

#include <weapons/WeaponScript.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <lua/LUAScriptFactory.h>
#include <lua/LUAScript.h>

REGISTER_ACCESSORY_SOURCE(WeaponScript);

WeaponScript::WeaponScript() :
	script_(0)
{

}

WeaponScript::~WeaponScript()
{
	delete script_;
	script_ = 0;
}

bool WeaponScript::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	std::string filename;
	if (!accessoryNode->getNamedChild("filename", filename)) return false;
	if (!accessoryNode->getNamedChild("entrypoint", entrypoint_)) return false;

	std::string luaErrorString;
	script_ = context.getScorchedContext().getLUAScriptFactory().createScript();
	script_->addWeaponFunctions();
	if (!script_->loadFromFile(S3D::getModFile(filename), luaErrorString))
	{
		return accessoryNode->returnError(
			S3D::formatStringBuffer("Failed to load lua script %s : %s", 
			filename.c_str(), luaErrorString.c_str()));
	}
	script_->setWeapon(this);

	XMLNode *variable;
	while (accessoryNode->getNamedChild("variable", variable, false))
	{
		std::string name;
		fixed value;
		if (!variable->getNamedChild("name", name)) return false;
		if (!variable->getNamedChild("value", value)) return false;

		script_->setGlobal(name, value);
	}

	return accessoryNode->failChildren();
}

void WeaponScript::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	fixed fPlayerId(true, weaponContext.getPlayerId());

	script_->startFunction(entrypoint_);
	script_->addNumberParameter(fPlayerId);
	script_->addVectorParameter(position);
	script_->addVectorParameter(velocity);
	script_->endFunction(3);
}
