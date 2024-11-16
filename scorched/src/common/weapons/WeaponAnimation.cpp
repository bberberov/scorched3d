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

#include <weapons/WeaponAnimation.h>
#include <weapons/Accessory.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#ifndef S3D_SERVER
#include <sprites/MetaActionRenderer.h>
#endif

REGISTER_ACCESSORY_SOURCE(WeaponAnimation);

WeaponAnimation::WeaponAnimation()
{

}

WeaponAnimation::~WeaponAnimation()
{

}

bool WeaponAnimation::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("data", data_)) return false;
	if (!accessoryNode->getNamedChild("animation", rendererName_)) return false;

	if (0 != strcmp(rendererName_.c_str(), "ExplosionLaserBeamRenderer"))
	{
		S3D::dialogMessage("Accessory", S3D::formatStringBuffer(
			"Failed to find animation named \"%s\" in accessory \"%s\"",
			rendererName_.c_str(),
			parent_->getName()));
		return false;
	}

	return true;
}

void WeaponAnimation::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{

#ifndef S3D_SERVER
	if (!context.getServerMode())
	{
		MetaActionRenderer *renderer = (MetaActionRenderer *) 
			MetaClassRegistration::getNewClass(rendererName_.c_str());

		if (renderer)
		{
			renderer->init(weaponContext.getPlayerId(), 
				position.asVector(), velocity.asVector(), data_.c_str());
			context.getActionController().addAction(new SpriteAction(renderer));
		}
		else
		{
			S3D::dialogMessage("Animation", S3D::formatStringBuffer(
						  "No renderer named \"%s\"",
						  rendererName_.c_str()));
		}
	}
#endif // #ifndef S3D_SERVER
}
