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

#include <tankai/TankAIRandom.h>
#include <stdlib.h>

TankAIRandom::TankAIRandom()
{
	name_ = "Random";
	description_ = "A computer controlled player.\n"
		"Chooses a random computer AI from the\nlist of all available types.";
}

TankAIRandom::~TankAIRandom()
{
}

TankAI *TankAIRandom::createCopy(Tanket *tanket)
{
	TankAI *result = ais_[rand() % ais_.size()];
	return result->createCopy(tanket);
}

void TankAIRandom::addTankAI(TankAI *ai)
{
	ais_.push_back(ai);
}
