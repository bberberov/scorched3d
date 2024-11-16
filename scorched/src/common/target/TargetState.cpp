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

#include <target/TargetState.h>
#include <net/NetBuffer.h>

TargetStateMovement::TargetStateMovement()
{
}

TargetStateMovement::~TargetStateMovement()
{
}

TargetState::TargetState() :
	falling_(0), moving_(0),
	displayDamage_(true), displayShadow_(true),
	noDamageBurn_(false), noCollision_(false), noFalling_(false),
	movement_(0), noFallingDamage_(false), 
	driveOverToDestroy_(false), flattenDestroy_(false),
	displayHardwareShadow_(true)
{
}

TargetState::~TargetState()
{
	delete movement_;
}

bool TargetState::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TargetState");
	unsigned int value =
		(displayDamage_?1:0) |
		(displayShadow_?2:0) |
		(noDamageBurn_?4:0) |
		(noCollision_?8:0) |
		(noFalling_?16:0) |
		//(movement_?32:0) |
		(noFallingDamage_?64:0) |
		(driveOverToDestroy_?128:0) |
		(flattenDestroy_?256:0);
	buffer.addToBufferNamed("state", value);
	return true;
}

bool TargetState::readMessage(NetBufferReader &reader)
{
	unsigned int value = 0;
	if (!reader.getFromBuffer(value)) return false;

	displayDamage_ = (value & 1) != 0;
	displayShadow_ = (value & 2) != 0;
	noDamageBurn_ = (value & 4) != 0;
	noCollision_ = (value & 8) != 0;
	noFalling_ = (value & 16) != 0;
	//movement_ = (value & 32) != 0;
	noFallingDamage_ = (value & 64) != 0;
	driveOverToDestroy_ = (value & 128) != 0;
	flattenDestroy_ = (value & 256) != 0;

	return true;
}

