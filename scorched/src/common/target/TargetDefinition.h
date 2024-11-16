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

#if !defined(__INCLUDE_TargetDefinitionh_INCLUDE__)
#define __INCLUDE_TargetDefinitionh_INCLUDE__

#include <common/ModelID.h>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>
#include <engine/ObjectGroupEntryDefinition.h>
#include <placement/PlacementShadowDefinition.h>

class Target;
class TargetDefinition
{
public:
	TargetDefinition();
	virtual ~TargetDefinition();

	virtual bool readXML(XMLNode *node);
	Target *createTarget(unsigned int playerId,
		FixedVector &position,
		FixedVector &velocity,
		ScorchedContext &context,
		RandomGenerator &generator);

	PlacementShadowDefinition &getShadow() { return shadow_; }
	ObjectGroupEntryDefinition &getGroups() { return groups_; }

protected:
	fixed life_;
	bool boundingsphere_;
	bool driveovertodestroy_, flattendestroy_;
	bool nocollision_, nodamageburn_, nofalling_;
	bool displaydamage_, displayshadow_;
	bool displayhardwareshadow_;
	bool nofallingdamage_;
	bool useNormalMoves_;
	bool billboard_;
	LangString name_;
	std::string parachute_;
	std::string shield_;
	std::string ainame_;
	int team_;
	FixedVector size_;
	fixed modelscale_, modelscalediff_;
	fixed modelrotation_, modelbrightness_;
	fixed modelrotationsnap_;
	fixed border_;
	std::string removeaction_;
	std::string burnaction_;
	std::string collisionaction_;
	ModelID modelId_;
	ModelID modelburntId_;
	PlacementShadowDefinition shadow_;
	ObjectGroupEntryDefinition groups_;
};

#endif // __INCLUDE_TargetDefinitionh_INCLUDE__
