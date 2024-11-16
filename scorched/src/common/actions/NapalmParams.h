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

#if !defined(__INCLUDE_NapalmParamsh_INCLUDE__)
#define __INCLUDE_NapalmParamsh_INCLUDE__

#include <string>
#include <common/fixed.h>

struct lua_State;
class XMLNode;
class NapalmParams 
{
public:
	NapalmParams();
	~NapalmParams();

	bool parseXML(XMLNode *accessoryNode);
	void parseLUA(lua_State *L, int position);

	void setNapalmTime(fixed value) { napalmTime_ = value; }
	void setNapalmHeight(fixed value) { napalmHeight_ = value; }
	void setStepTime(fixed value) { stepTime_ = value; }
	void setHurtStepTime(fixed value) { hurtStepTime_ = value; }
	void setHurtPerSecond(fixed value) { hurtPerSecond_ = value; }
	void setLandscapeErosion(fixed value) { landscapeErosion_ = value; }

	fixed getNapalmTime() { return napalmTime_; }
	fixed getNapalmHeight() { return napalmHeight_; }
	fixed getStepTime() { return stepTime_; }
	fixed getHurtStepTime() { return hurtStepTime_; }
	fixed getHurtPerSecond() { return hurtPerSecond_; }
	fixed getGroundScorchPer() { return groundScorchPer_; }
	fixed getLandscapeErosion() { return landscapeErosion_; }
	int getEffectRadius() { return effectRadius_; }
	int getNumberParticles() { return numberParticles_; }
	bool getNoSmoke() { return noSmoke_; }
	bool getNoObjectDamage() { return noObjectDamage_; }
	bool getAllowUnderWater() { return allowUnderWater_; }
	bool getLuminance() { return luminance_; }
	bool getSingleFlow() { return singleFlow_; }
	bool getNoCameraTrack() { return noCameraTrack_; }
	const char *getNapalmTexture() { return napalmTexture_.c_str(); }
	const char *getDeformTexture() { return deformTexture_.c_str(); }

protected:
	fixed napalmTime_;   // The time to generate napalm
	fixed napalmHeight_; // The height of a napalm point
	fixed stepTime_;     // Add/rm napalm every StepTime secs
	fixed hurtStepTime_; // Calculate damage every HurtStepTime secs
	fixed hurtPerSecond_;// Damage per second
	fixed groundScorchPer_; // The percentage chance the ground will be scorched
	fixed landscapeErosion_; // How much height will be removed for the erosion
	int effectRadius_;   // How close do tanks take damage
	int numberParticles_;// How many napalm particles can be created
	bool noSmoke_;       // Turns off smoke emitted by napalm
	bool noObjectDamage_;// Turns off burning damage to landscape objects
	bool allowUnderWater_;// Turns on/off napalm working under water
	bool singleFlow_;    // Use a single flow of napalm or cover the whole downward area
	bool luminance_;     // The texutre luminance
	bool noCameraTrack_;
	std::string napalmTexture_;
	std::string deformTexture_;

};

#endif
