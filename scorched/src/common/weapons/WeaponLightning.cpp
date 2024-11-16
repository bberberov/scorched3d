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

#include <weapons/WeaponLightning.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <actions/Lightning.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponLightning);

WeaponLightning::WeaponLightning() :
	coneLengthExp_("WeaponLightning::coneLengthExp"),
	segLengthExp_("WeaponLightning::segLengthExp"),
	segVarExp_("WeaponLightning::segVarExp"),
	sizeExp_("WeaponLightning::sizeExp"),
	sizeVarExp_("WeaponLightning::sizeVarExp"),
	minSizeExp_("WeaponLightning::minSizeExp"),
	splitProbExp_("WeaponLightning::splitProbExp"),
	splitVarExp_("WeaponLightning::splitVarExp"),
	deathProbExp_("WeaponLightning::deathProbExp"),
	derivAngleExp_("WeaponLightning::derivAngleExp"),
	angleVarExp_("WeaponLightning::angleVarExp"),
	totalTimeExp_("WeaponLightning::totalTimeExp"),
	segHurtExp_("WeaponLightning::segHurtExp"),
	segHurtRadiusExp_("WeaponLightning::segHurtRadiusExp")
{
}

WeaponLightning::~WeaponLightning()
{
}

bool WeaponLightning::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;
	if (!accessoryNode->getNamedChild("conelength", coneLengthExp_)) return false;
	if (!accessoryNode->getNamedChild("seglength", segLengthExp_)) return false;
	if (!accessoryNode->getNamedChild("segvar", segVarExp_)) return false;
	if (!accessoryNode->getNamedChild("size", sizeExp_)) return false;
	if (!accessoryNode->getNamedChild("sizevar", sizeVarExp_)) return false;
	if (!accessoryNode->getNamedChild("minsize", minSizeExp_)) return false;
	if (!accessoryNode->getNamedChild("splitprob", splitProbExp_)) return false;
	if (!accessoryNode->getNamedChild("splitvar", splitVarExp_)) return false;
	if (!accessoryNode->getNamedChild("deathprob", deathProbExp_)) return false;
	if (!accessoryNode->getNamedChild("derivangle", derivAngleExp_)) return false;
	if (!accessoryNode->getNamedChild("anglevar", angleVarExp_)) return false;
	if (!accessoryNode->getNamedChild("totaltime", totalTimeExp_)) return false;
	if (!accessoryNode->getNamedChild("seghurt", segHurtExp_)) return false;
	if (!accessoryNode->getNamedChild("seghurtradius", segHurtRadiusExp_)) return false;
	if (!accessoryNode->getNamedChild("sound", sound_)) return false;

	texture_ = "data/textures/lightning.bmp";
	accessoryNode->getNamedChild("texture", texture_, false);

	if (!S3D::checkDataFile(getSound())) return false;
	return true;
}

void WeaponLightning::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	coneLength_ = coneLengthExp_.getValue(context);
	segLength_ = segLengthExp_.getValue(context);
	segVar_ = segVarExp_.getValue(context);
	size_ = sizeExp_.getValue(context);
	sizeVar_ = sizeVarExp_.getValue(context);
	minSize_ = minSizeExp_.getValue(context);
	splitProb_ = splitProbExp_.getValue(context);
	splitVar_ = splitVarExp_.getValue(context);
	deathProb_ = deathProbExp_.getValue(context);
	derivAngle_ = derivAngleExp_.getValue(context);
	angleVar_ = angleVarExp_.getValue(context);
	totalTime_ = totalTimeExp_.getValue(context);
	segHurt_ = segHurtExp_.getValue(context);
	segHurtRadius_ = segHurtRadiusExp_.getValue(context);

	Action *action = new Lightning(
		this, weaponContext, position, velocity); 
	context.getActionController().addAction(action);	
}

