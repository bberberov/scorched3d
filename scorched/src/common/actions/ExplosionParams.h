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

#if !defined(__INCLUDE_ExplosionParamsh_INCLUDE__)
#define __INCLUDE_ExplosionParamsh_INCLUDE__

#include <common/FixedVector.h>

struct lua_State;
class XMLNode;
class ExplosionParams
{
public:
	enum DeformType
	{
		DeformNone = 0,
		DeformDown = 1,
		DeformUp = 2,
	};
	enum ExplosionType
	{
		ExplosionNormal = 0,
		ExplosionRing = 1,
		ExplosionRingDirectional = 2
	};

	ExplosionParams();
	~ExplosionParams();

	bool parseXML(XMLNode *accessoryNode);
	void parseLUA(lua_State *L, int position);

	void setSize(fixed size) { size_ = size; }
	void setDeformSize(fixed size) { deformSize_ = size; }
	void setShake(fixed shake) { shake_ = shake; }
	void setMinLife(fixed minLife) { minLife_ = minLife; }
	void setMaxLife(fixed maxLife) { maxLife_ = maxLife; }
	void setHurtAmount(fixed amount) { hurtAmount_ = amount; }
	void setCreateMushroomAmount(fixed amount) { createMushroomAmount_ = amount; }

	const char *getExplosionTexture();
	const char *getMushroomTexture();
	const char *getExplosionSound();
	FixedVector &getExplosionColor();

	fixed getSize() { return size_; }
	fixed getDeformSize() { return deformSize_; }
	fixed getMaxLife() { return maxLife_; }
	fixed getMinLife() { return minLife_; }
	fixed getHurtAmount() { return hurtAmount_; }
	fixed getShake() { return shake_; }
	fixed getCreateMushroomAmount() { return createMushroomAmount_; }

	DeformType getDeformType() { return deform_; }
	ExplosionType getExplosionType() { return explosionType_; }
	bool getCreateDebris() { return createDebris_; }
	bool getCreateSplash() { return createSplash_; }
	bool getWindAffected() { return windAffected_; }
	bool getOnlyHurtShield() { return onlyHurtShield_; }
	bool getLuminance() { return luminance_; }
	bool getAnimate() { return animate_; }
	bool getExplodeUnderGround() { return explodeUnderGround_; }
	bool getNoCameraTrack() { return noCameraTrack_; }
	
	const char *getDeformTexture() { return deformTexture_.c_str(); }

protected:
	fixed size_, deformSize_;
	fixed hurtAmount_;
	fixed shake_;
	fixed minLife_, maxLife_;
	fixed createMushroomAmount_;
	bool luminance_;
	bool windAffected_;
	bool multiColored_;
	bool createDebris_;
	bool createSplash_;
	bool explodeUnderGround_;
	bool onlyHurtShield_;
	bool animate_;
	bool noCameraTrack_;
	
	std::string deformTexture_;
	std::string explosionTexture_;
	std::string explosionSound_;
	std::string mushroomTexture_;
	DeformType deform_;
	ExplosionType explosionType_;
	
};

#endif
