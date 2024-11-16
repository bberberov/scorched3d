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

#include <math.h>
#include <tankai/TankAIAdder.h>
#include <target/Target.h>
#include <target/TargetState.h>
#include <target/TargetRenderer.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <target/TargetGroup.h>
#include <XML/XmlNamedNetBuffer.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>

Target::Target(unsigned int playerId, 
	const LangString &name,
	ScorchedContext &context) :
	playerId_(playerId),
	context_(context),
	deathAction_(0), burnAction_(0), collisionAction_(0),
	renderer_(0), 
	border_(0)
{
	setName(name);

	life_ = new TargetLife(context.getServerMode(), context.getTargetSpace(), playerId);
	shield_ = new TargetShield(context, playerId);
	parachute_ = new TargetParachute(context);
	group_ = new TargetGroup(context);
	targetState_ = new TargetState();

	life_->setTarget(this);
	life_->setBoundingSphere(true);
	shield_->setTarget(this);
	shield_->setCurrentShield(0);
	group_->setTarget(this);
}

Target::~Target()
{
	life_->setLife(0);

	delete renderer_; renderer_ = 0;
	delete life_; life_ = 0;
	delete shield_; shield_ = 0;
	delete group_; group_ = 0;
	delete parachute_; parachute_ = 0;
	delete targetState_; targetState_ = 0;
	playerId_ = 0;
}

void Target::loaded()
{
	shield_->loaded();
	parachute_->loaded();
}

void Target::newGame()
{
	life_->newGame();
}

bool Target::getVisible()
{
	return getAlive();
}

bool Target::getPlaying()
{
	return getAlive();
}

bool Target::getAlive()
{
	return (life_->getLife() > 0);
}

void Target::setName(const LangString &name)
{
	name_ = name;
	if (!context_.getOptionsGame().getAllowMultiLingualNames() &&
		getType() == Target::TypeTank)
	{
		for (unsigned int *c = (unsigned int *)name_.c_str();
			*c;
			c++)
		{
			if (*c > 127) *c = '?';
		}
	}
}

const std::string &Target::getCStrName()
{
	cStrName_ = LangStringUtil::convertFromLang(name_);
	return cStrName_;
}

void Target::toString(std::string &str)
{
	XmlNamedNetBuffer buffer;
	writeMessage(buffer);
	FileLines fileLines;
	buffer.getBuffer().addNodeToFile(fileLines, 4);
	fileLines.getAsOneLine(str);
}

bool Target::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "Target");

	buffer.addToBufferNamed("name", name_);
	if (!shield_->writeMessage(buffer)) return false;
	if (!life_->writeMessage(buffer)) return false;
	if (!parachute_->writeMessage(buffer)) return false;
	if (!targetState_->writeMessage(buffer)) return false;
	if (!group_->writeMessage(buffer)) return false;
	buffer.addToBufferNamed("border", border_);
	if (!context_.getAccessoryStore().writeAccessory(buffer, deathAction_)) return false;
	if (!context_.getAccessoryStore().writeAccessory(buffer, burnAction_)) return false;
	if (!context_.getAccessoryStore().writeAccessory(buffer, collisionAction_)) return false;

	return true;
}

bool Target::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(name_))
	{
		Logger::log("Target::name_ read failed");
		return false;
	}
	if (!shield_->readMessage(reader))
	{
		Logger::log("Target::shield_ read failed");
		return false;
	}
	if (!life_->readMessage(reader))
	{
		Logger::log("Target::life_ read failed");
		return false;
	}
	if (!parachute_->readMessage(reader))
	{
		Logger::log("Target::parachute_ read failed");
		return false;
	}
	if (!targetState_->readMessage(reader))
	{
		Logger::log("Target::targetState_ read failed");
		return false;
	}
	if (!group_->readMessage(reader))
	{
		Logger::log("Target::group_ read failed");
		return false;
	}
	if (!reader.getFromBuffer(border_))
	{
		Logger::log("Target::border read failed");
		return false;
	}
	if (!context_.getAccessoryStore().readAccessory(reader, deathAction_))
	{
		Logger::log("Target::deathAction read failed");
		return false;
	}
	if (!context_.getAccessoryStore().readAccessory(reader, burnAction_))
	{
		Logger::log("Target::burnAction read failed");
		return false;
	}
	if (!context_.getAccessoryStore().readAccessory(reader, collisionAction_))
	{
		Logger::log("Target::collisionAction read failed");
		return false;
	}

	return true;
}
