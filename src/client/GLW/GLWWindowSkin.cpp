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

#include <GLW/GLWWindowSkin.h>
#include <GLW/GLWidget.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWToolTip.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <client/ScorchedClient.h>

GLWWindowSkin::GLWWindowSkin() : 
	GLWWindow("None", 0.0f, 0.0f,
		0.0f, 0.0f, eNoDraw | eSavePosition, "")
{
	setToolTip(&toolTip_);
}

GLWWindowSkin::~GLWWindowSkin()
{
}

bool GLWWindowSkin::inState(const char *state)
{
	return (states_.find(state) != states_.end());
}

bool GLWWindowSkin::initFromXML(XMLNode *node)
{
	if (!GLWWindow::initFromXML(node)) return false;

	// States
	std::string state;
	while (node->getNamedChild("state", state, false))
	{
		if (strcmp(state.c_str(), "playing") != 0 &&
			strcmp(state.c_str(), "all") != 0 &&
			strcmp(state.c_str(), "start") != 0)
		{
			S3D::dialogExit("Unrecognized window state %s",
				state.c_str());
		}

		states_.insert(state);
	}
	if (states_.empty())
	{	
		S3D::dialogExit("No window states defined for window %s",
			name_.c_str());
	}

	// Key
	if (!node->getNamedChild("key", key_)) return false;

	return true;
}
