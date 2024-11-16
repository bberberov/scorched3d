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

#include <actions/ExplosionParams.h>
#include <lua/LUAUtil.h>
#include <XML/XMLNode.h>

ExplosionParams::ExplosionParams() :
	size_(1), deformSize_(1),
	multiColored_(false), hurtAmount_(0),
	deform_(DeformDown), explosionType_(ExplosionNormal),
	createDebris_(true), createMushroomAmount_(0),
	createSplash_(true), windAffected_(true),
	luminance_(true), animate_(true),
	onlyHurtShield_(false), explodeUnderGround_(true),
	noCameraTrack_(false),
	minLife_(true, 5000), maxLife_(1), shake_(0),
	explosionTexture_("exp00"),
	mushroomTexture_("smoke")
{
}

ExplosionParams::~ExplosionParams()
{
}

const char *ExplosionParams::getExplosionTexture()
{
	return explosionTexture_.c_str();
}

const char *ExplosionParams::getMushroomTexture()
{
	return mushroomTexture_.c_str();
}

const char *ExplosionParams::getExplosionSound()
{
	if (!explosionSound_.c_str()[0]) return 0;
	return explosionSound_.c_str();
}

FixedVector &ExplosionParams::getExplosionColor()
{
	static FixedVector white(1, 1, 1);
	if (!multiColored_) return white;

    static FixedVector red(1, 0, 0);
    static FixedVector green(0, 1, 0);
    static FixedVector blue(0, 0, 1);
    static FixedVector yellow(1, 1, 0);

    int color = int(RAND * 4.0f);
    switch (color)
    {
    case 0:
        return red;
    case 1:
        return green;
    case 2:
        return blue;
    case 3:
        return yellow;
    }
    return white;
}

bool ExplosionParams::parseXML(XMLNode *accessoryNode)
{
    // Get the accessory colored
    XMLNode *colorNode = 0;
	accessoryNode->getNamedChild("multicolor", colorNode, false);
    if (colorNode) multiColored_ = true;

	// Get the hurt shield node
	accessoryNode->getNamedChild("onlyhurtshield", onlyHurtShield_, false);

	accessoryNode->getNamedChild("explodeunderground", explodeUnderGround_, false);

	accessoryNode->getNamedChild("nocameratrack", noCameraTrack_, false);	

	// Get the no debris node
	XMLNode *noCreateDebrisNode = 0;
	accessoryNode->getNamedChild("nocreatedebris", noCreateDebrisNode, false);
	if (noCreateDebrisNode) createDebris_ = false;

	// Get the no splash node
	XMLNode *noCreateSplashNode = 0;
	accessoryNode->getNamedChild("nocreatesplash", noCreateSplashNode, false);
	if (noCreateSplashNode) createSplash_ = false;

	// Get the no windaffecting
	XMLNode *noWindAffectedNode = 0;
	accessoryNode->getNamedChild("nowindaffected", noWindAffectedNode, false);
	if (noWindAffectedNode) windAffected_ = false;

	// Get the no luminance node
	XMLNode *noLuminanceNode = 0;
	accessoryNode->getNamedChild("noluminance", noLuminanceNode, false);
	if (noLuminanceNode) luminance_ = false;

	// Get the animate node
	XMLNode *animateNode = 0;
	accessoryNode->getNamedChild("animate", animateNode, false);
	if (animateNode) animate_ = true;
	XMLNode *noAnimateNode = 0;
	accessoryNode->getNamedChild("noanimate", noAnimateNode, false);
	if (noAnimateNode) animate_ = false;

	// Get the deform texture
	if (accessoryNode->getNamedChild("deformtexture", deformTexture_, false))
	{
		if (!S3D::checkDataFile(getDeformTexture())) return false;
	}

	// Get the explosion texture
	accessoryNode->getNamedChild("explosiontexture", explosionTexture_, false);
	accessoryNode->getNamedChild("mushroomtexture", mushroomTexture_, false);

	// Get the explosion sound
	if (accessoryNode->getNamedChild("explosionsound", explosionSound_, false))
	{
		if (!S3D::checkDataFile(S3D::formatStringBuffer("data/wav/%s", getExplosionSound()))) return false;
	}

	// Get the deform
	XMLNode *deformNode = 0;
	if (!accessoryNode->getNamedChild("deform", deformNode)) return false;
    if (0 == strcmp(deformNode->getContent(), "down")) deform_ = DeformDown;
	else if (0 == strcmp(deformNode->getContent(), "up")) deform_ = DeformUp;
	else if (0 == strcmp(deformNode->getContent(), "none")) deform_ = DeformNone;
	else return deformNode->returnError(
		S3D::formatStringBuffer("Unknown deform type \"%s\" should be up, down or none",
		deformNode->getContent()));

	std::string explosionType = "normal";
	if (accessoryNode->getNamedChild("explosiontype", explosionType, false))
	{
		if (explosionType == "normal") explosionType_ = ExplosionNormal;
		else if (explosionType == "ring") explosionType_ = ExplosionRing;
		else if (explosionType == "directionalring") explosionType_ = ExplosionRingDirectional;
		else 
		{	
			return accessoryNode->returnError(
				S3D::formatStringBuffer("Unknown explosion type \"%s\" should be normal, ring or directionalring",
				explosionType.c_str()));
		}
	}

	return true;
}

void ExplosionParams::parseLUA(lua_State *L, int position)
{
	luaL_checktype(L, position, LUA_TTABLE);

	size_ = LUAUtil::getNumberFromTable(L, position, "size", size_);
	deformSize_ = LUAUtil::getNumberFromTable(L, position, "deformsize", size_);
	hurtAmount_ = LUAUtil::getNumberFromTable(L, position, "hurtamount", hurtAmount_);

	shake_ = LUAUtil::getNumberFromTable(L, position, "shake", shake_);
	minLife_ = LUAUtil::getNumberFromTable(L, position, "minlife", minLife_);
	maxLife_ = LUAUtil::getNumberFromTable(L, position, "maxlife", maxLife_);
	createMushroomAmount_ = LUAUtil::getNumberFromTable(L, position, "createmushroomamount", createMushroomAmount_);

	luminance_ = LUAUtil::getBoolFromTable(L, position, "luminance", luminance_);
	windAffected_ = LUAUtil::getBoolFromTable(L, position, "windaffected", windAffected_);
	multiColored_ = LUAUtil::getBoolFromTable(L, position, "multicolored", multiColored_);
	createDebris_ = LUAUtil::getBoolFromTable(L, position, "createdebris", createDebris_);
	createSplash_ = LUAUtil::getBoolFromTable(L, position, "createsplash", createSplash_);
	explodeUnderGround_ = LUAUtil::getBoolFromTable(L, position, "explodeunderground", explodeUnderGround_);
	onlyHurtShield_ = LUAUtil::getBoolFromTable(L, position, "onlyhurtshield", onlyHurtShield_);
	animate_ = LUAUtil::getBoolFromTable(L, position, "animate", animate_);
	
	deformTexture_ = LUAUtil::getStringFromTable(L, position, "deformtexture", deformTexture_);
	explosionTexture_ = LUAUtil::getStringFromTable(L, position, "explosiontexture", explosionTexture_);
	mushroomTexture_ = LUAUtil::getStringFromTable(L, position, "mushroomtexture", mushroomTexture_);
	explosionSound_ = LUAUtil::getStringFromTable(L, position, "explosionsound", explosionSound_);

	fixed deform = LUAUtil::getNumberFromTable(L, position, "deform", int(deform_));
	deform_ = (DeformType) deform.asInt();
	fixed explosiontype = LUAUtil::getNumberFromTable(L, position, "explosiontype", int(explosionType_));
	explosionType_ = (ExplosionType) explosiontype.asInt();
}
