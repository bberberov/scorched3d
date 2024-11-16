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

#if !defined(__INCLUDE_LaserParamsh_INCLUDE__)
#define __INCLUDE_LaserParamsh_INCLUDE__

#include <string>
#include <common/fixed.h>
#include <common/Vector.h>

struct lua_State;
class XMLNode;
class LaserParams 
{
public:
	LaserParams();
	~LaserParams();

	bool parseXML(XMLNode *accessoryNode);
	void parseLUA(lua_State *L, int position);

	void setMinimumHurt(fixed value) { minimumHurt_ = value; }
	void setMaximumHurt(fixed value) { maximumHurt_ = value; }
	void setMinimumDistance(fixed value) { minimumDistance_ = value; }
	void setMaximumDistance(fixed value) { maximumDistance_ = value; }
	void setHurtRadius(fixed value) { hurtRadius_ = value; }
	void setTotalTime(fixed value) { totalTime_ = value; }

	fixed getMinimumHurt() { return minimumHurt_; }
	fixed getMaximumHurt() { return maximumHurt_; }
	fixed getMinimumDistance() { return minimumDistance_; }
	fixed getMaximumDistance() { return maximumDistance_; }
	fixed getHurtRadius() { return hurtRadius_; }
	fixed getTotalTime() { return totalTime_; }
	bool getHurtFirer() { return hurtFirer_; }
	Vector &getColor() { return color_; }

	float getRingRadius() { return ringRadius_; }
	std::string &getRingTexture() { return ringTexture_; }

protected:
	std::string ringTexture_;
	fixed minimumHurt_, maximumHurt_;
	fixed minimumDistance_, maximumDistance_;
	fixed hurtRadius_;
	fixed totalTime_;
	bool hurtFirer_;
	Vector color_;
	float ringRadius_;
};

#endif // __INCLUDE_LaserParamsh_INCLUDE__
