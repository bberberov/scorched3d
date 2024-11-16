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

#include <sprites/TextActionRenderer.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLState.h>

TextActionRenderer::TextActionRenderer(
		Vector &position,
		Vector &color,
		const std::string &text) :
	position_(position),
	color_(color),
	text_(text),
	frameTime_(0.0f)
{
}

TextActionRenderer::~TextActionRenderer()
{
}

void TextActionRenderer::simulate(Action *action, float timepassed, bool &remove)
{
	position_[2] += timepassed;
	frameTime_ += timepassed;
	remove = (frameTime_ > 6.0f);
}

void TextActionRenderer::draw(Action *action)
{
	if (!GLCameraFrustum::instance()->sphereInFrustum(position_))
	{
		return;
	}

	GLState currentState(GLState::DEPTH_ON | GLState::TEXTURE_ON);
	glDepthMask(GL_FALSE);
	GLWFont::instance()->getGameFont()->drawBilboard(color_, 1.0f - (frameTime_ / 6.0f), 1.0f, 
		position_[0], position_[1], position_[2],
		text_.c_str());

	glDepthMask(GL_TRUE);
}
