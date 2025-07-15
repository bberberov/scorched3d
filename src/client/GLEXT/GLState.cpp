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

#include <GLEXT/GLState.h>
#include <common/DefinesString.h>
#include <common/DefinesAssert.h>
#include <string.h>
#include <string>

unsigned GLState::currentState_ = 
	GLState::TEXTURE_OFF | GLState::BLEND_OFF | 
	GLState::DEPTH_OFF | GLState::CUBEMAP_OFF | 
	GLState::LIGHTING_OFF | GLState::NORMALIZE_OFF | 
	GLState::LIGHT1_OFF | GLState::ALPHATEST_OFF;
unsigned int GLState::stateSwitches_ = 0;

GLState::GLState(unsigned wantedState)
{
	returnState_ = currentState_;

	if (wantedState == 0) return;
	setState(wantedState);
}

GLState::~GLState()
{
	setState(returnState_);
}

void GLState::setState(unsigned wanted)
{
	if (wanted == currentState_) return;

	GL_ASSERT();

	if ((wanted & ALPHATEST_ON) && (currentState_ & ALPHATEST_OFF))
	{
		currentState_ ^= ALPHATEST_OFF;
		currentState_ |= ALPHATEST_ON;
		glEnable(GL_ALPHA_TEST);
		stateSwitches_++;
	}
	else if ((wanted & ALPHATEST_OFF) && (currentState_ & ALPHATEST_ON))
	{
		currentState_ ^= ALPHATEST_ON;
		currentState_ |= ALPHATEST_OFF;
		glDisable(GL_ALPHA_TEST);
		stateSwitches_++;
	}

	if ((wanted & LIGHT1_ON) && (currentState_ & LIGHT1_OFF))
	{
		currentState_ ^= LIGHT1_OFF;
		currentState_ |= LIGHT1_ON;
		glEnable(GL_LIGHT1);
		stateSwitches_++;
	}
	else if ((wanted & LIGHT1_OFF) && (currentState_ & LIGHT1_ON))
	{
		currentState_ ^= LIGHT1_ON;
		currentState_ |= LIGHT1_OFF;
		glDisable(GL_LIGHT1);
		stateSwitches_++;
	}

	if ((wanted & LIGHTING_ON) && (currentState_ & LIGHTING_OFF))
	{
		currentState_ ^= LIGHTING_OFF;
		currentState_ |= LIGHTING_ON;
		glEnable(GL_LIGHTING);
		stateSwitches_++;
	}
	else if ((wanted & LIGHTING_OFF) && (currentState_ & LIGHTING_ON))
	{
		currentState_ ^= LIGHTING_ON;
		currentState_ |= LIGHTING_OFF;
		glDisable(GL_LIGHTING);
		stateSwitches_++;
	}

	if ((wanted & NORMALIZE_ON) && (currentState_ & NORMALIZE_OFF))
	{
		currentState_ ^= NORMALIZE_OFF;
		currentState_ |= NORMALIZE_ON;
		glEnable(GL_NORMALIZE);
		stateSwitches_++;
	}
	else if ((wanted & NORMALIZE_OFF) && (currentState_ & NORMALIZE_ON))
	{
		currentState_ ^= NORMALIZE_ON;
		currentState_ |= NORMALIZE_OFF;
		glDisable(GL_NORMALIZE);
		stateSwitches_++;
	}

	if ((wanted & TEXTURE_ON) && (currentState_ & TEXTURE_OFF))
	{
		currentState_ ^= TEXTURE_OFF;
		currentState_ |= TEXTURE_ON;
		glEnable(GL_TEXTURE_2D);
		stateSwitches_++;
	}
	else if ((wanted & TEXTURE_OFF) && (currentState_ & TEXTURE_ON))
	{
		currentState_ ^= TEXTURE_ON;
		currentState_ |= TEXTURE_OFF;
		glDisable(GL_TEXTURE_2D);
		stateSwitches_++;
	}

	if ((wanted & DEPTH_ON) && (currentState_ & DEPTH_OFF))
	{
		currentState_ ^= DEPTH_OFF;
		currentState_ |= DEPTH_ON;
		glEnable(GL_DEPTH_TEST);
		stateSwitches_++;
	}
	else if ((wanted & DEPTH_OFF) && (currentState_ & DEPTH_ON))
	{
		currentState_ ^= DEPTH_ON;
		currentState_ |= DEPTH_OFF;
		glDisable(GL_DEPTH_TEST);
		stateSwitches_++;
	}

	if ((wanted & BLEND_ON) && (currentState_ & BLEND_OFF))
	{
		currentState_ ^= BLEND_OFF;
		currentState_ |= BLEND_ON;
		glEnable(GL_BLEND);
		stateSwitches_++;
	}
	else if ((wanted & BLEND_OFF) && (currentState_ & BLEND_ON))
	{
		currentState_ ^= BLEND_ON;
		currentState_ |= BLEND_OFF;
		glDisable(GL_BLEND);
		stateSwitches_++;
	}

	if ((wanted & CUBEMAP_ON) && (currentState_ & CUBEMAP_OFF))
	{
		currentState_ ^= CUBEMAP_OFF;
		currentState_ |= CUBEMAP_ON;
		glEnable(GL_TEXTURE_CUBE_MAP_EXT);
		stateSwitches_++;
	}
	else if ((wanted & CUBEMAP_OFF) && (currentState_ & CUBEMAP_ON))
	{
		currentState_ ^= CUBEMAP_ON;
		currentState_ |= CUBEMAP_OFF;
		glDisable(GL_TEXTURE_CUBE_MAP_EXT);
		stateSwitches_++;
	}

	GL_ASSERT();
}

static void addToBuffer(std::string &buffer, unsigned int value, const char *name)
{
	if (GLState::getState() & value) buffer.append(name).append("\n");
}

const char *GLState::getStateString()
{
	static std::string buffer;
	buffer = "";
	addToBuffer(buffer, NORMALIZE_ON, "NORMALIZE_ON");
	addToBuffer(buffer, NORMALIZE_OFF, "NORMALIZE_OFF");
	addToBuffer(buffer, LIGHTING_ON, "LIGHTING_ON");
	addToBuffer(buffer, LIGHTING_OFF, "LIGHTING_OFF");
	addToBuffer(buffer, LIGHT1_ON, "LIGHT1_ON");
	addToBuffer(buffer, LIGHT1_OFF, "LIGHT1_OFF");
	addToBuffer(buffer, TEXTURE_ON, "TEXTURE_ON");
	addToBuffer(buffer, TEXTURE_OFF, "TEXTURE_OFF");
	addToBuffer(buffer, DEPTH_ON, "DEPTH_ON");
	addToBuffer(buffer, DEPTH_OFF, "DEPTH_OFF");
	addToBuffer(buffer, BLEND_ON, "BLEND_ON");
	addToBuffer(buffer, BLEND_OFF, "BLEND_OFF");
	addToBuffer(buffer, CUBEMAP_ON, "CUBEMAP_ON");
	addToBuffer(buffer, CUBEMAP_OFF, "CUBEMAP_OFF");

	return buffer.c_str();
}

void GLState::setBaseState(unsigned bs)
{
	setState(bs);
}
