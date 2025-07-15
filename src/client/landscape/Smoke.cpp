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

#include <stdlib.h>
#include <landscape/Smoke.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>

Smoke::Smoke()
{
	emitter_.setAttributes(
		2.0f, 4.0f, // Life
		0.2f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		Vector(0.8f, 0.8f, 0.8f), 0.6f, // StartColor1
		Vector(0.8f, 0.8f, 0.8f), 0.8f, // StartColor2
		Vector(0.8f, 0.8f, 0.8f), 0.0f, // EndColor1
		Vector(0.8f, 0.8f, 0.8f), 0.0f, // EndColor2
		0.2f, 0.2f, 0.5f, 0.5f, // Start Size
		1.2f, 1.2f, 1.5f, 1.5f, // EndSize
		Vector(0.0f, 0.0f, 400.0f), // Gravity
		false,
		true);
}

void Smoke::addSmoke(float x, float y, float z)
{
	Vector position(x, y, z);
	emitter_.emitSmoke(1, position, 
		ScorchedClient::instance()->getParticleEngine());
}
