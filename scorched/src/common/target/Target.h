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

#if !defined(AFX_TARGET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
#define AFX_TARGET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_

#include <engine/ScorchedContext.h>
#include <common/FixedVector.h>
#include <lang/LangString.h>

namespace TargetID
{
	const int MIN_TANK_ID = 1;
	const int START_TRANSIENT_TANK_ID = 9000000;
	const int SPEC_TANK_ID = START_TRANSIENT_TANK_ID - 1;
	const int MAX_TANK_ID = 10000000;
	const int MIN_TARGET_ID = 20000000;
	const int MIN_TARGET_TRANSIENT_ID = 39000000;
	const int MAX_TARGET_ID = 40000000;
};

class NamedNetBuffer;
class NetBufferReader;
class TargetState;
class TargetLife;
class TargetShield;
class TargetGroup;
class TargetParachute;
class TargetRenderer;
class Accessory;
class Target  
{
public:
	enum TargetType
	{
		TypeTarget = 1,
		TypeTank = 2,
		TypeTanket = 3
	};

	Target(unsigned int playerId, 
		const LangString &name, 
		ScorchedContext &context);
	virtual ~Target();

	virtual void loaded();
	virtual void newGame();

	virtual bool getAlive();
	virtual bool getPlaying();
	virtual bool getVisible();
	virtual TargetType getType() { return Target::TypeTarget; }
	unsigned int getPlayerId() { return playerId_; }

	// Weapons
	TargetLife &getLife() { return *life_; }
	TargetShield &getShield() { return *shield_; }
	TargetParachute &getParachute() { return *parachute_; }
	TargetGroup &getGroup() { return *group_; }
	TargetState &getTargetState() { return *targetState_; }

	// Actions
	void setDeathAction(Accessory *deathAction) { deathAction_ = deathAction; }
	void setBurnAction(Accessory *burnAction) { burnAction_ = burnAction; }
	void setCollisionAction(Accessory *collisionAction) { collisionAction_ = collisionAction; }
	virtual Accessory *getDeathAction() { return deathAction_; }
	virtual Accessory *getBurnAction() { return burnAction_; }
	virtual Accessory *getCollisionAction() { return collisionAction_; }

	// Renderer
	fixed getBorder() { return border_; }
	void setBorder(fixed b) { border_ = b; }
	TargetRenderer *getRenderer() { return renderer_; }
	void setRenderer(TargetRenderer *renderer) { renderer_ = renderer; }

	// Name
	const LangString &getTargetName() { return name_; }
	const std::string &getCStrName();
	void setName(const LangString &name);
	unsigned int getNameLen() { return (unsigned int) name_.size(); }

	virtual void toString(std::string &str);

	// Serialize the target
	virtual bool writeMessage(NamedNetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	fixed border_;
	ScorchedContext &context_;
	TargetLife *life_;
	TargetShield *shield_;
	TargetParachute *parachute_;
	TargetRenderer *renderer_;
	TargetState *targetState_;
	TargetGroup *group_;
	LangString name_;
	std::string cStrName_;
	Accessory *deathAction_, *burnAction_, *collisionAction_;

};

#endif // !defined(AFX_TARGET_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
