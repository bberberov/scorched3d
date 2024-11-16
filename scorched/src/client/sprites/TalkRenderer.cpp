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

#include <sprites/TalkRenderer.h>
#include <graph/ParticleEmitter.h>
#include <client/ScorchedClient.h>

TalkRenderer::TalkRenderer(Vector &position, Vector &color) : 
	position_(position), color_(color)
{
}

TalkRenderer::~TalkRenderer()
{
}

void TalkRenderer::draw(Action *action)
{
}

void TalkRenderer::simulate(Action *action, float timepassed, bool &remove)
{
	remove = true;
	ParticleEmitter emitter;
	emitter.setAttributes(
		8.5f, 8.0f, // Life
		0.2f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		color_, 1.0f, // StartColor1
		color_, 1.0f, // StartColor2
		color_, 0.0f, // EndColor1
		color_, 0.0f, // EndColor2
		2.0f, 2.0f, 2.0f, 2.0f, // Start Size
		2.0f, 2.0f, 2.0f, 2.0f, // EndSize
		Vector(0.0f, 0.0f, 0.0f), // Gravity
		false,
		false);

	position_[2] += 10.0f;
	emitter.emitTalk(position_,
		ScorchedClient::instance()->getParticleEngine());
}
