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

#include <GLW/GLWSelector.h>
#include <GLW/GLWSelectorPart.h>
#include <GLEXT/GLViewPort.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>

GLWSelectorEntry::GLWSelectorEntry(const LangString &text, 
	ToolTip *tooltip, 
	bool selected,
	GLTextureBase *icon,
	void *userData,
	const std::string &dataText) : 
	text_(text),
	tip_(tooltip),
	selected_(selected),
	icon_(icon),
	userData_(userData),
	dataText_(dataText),
	color_(1.0f, 1.0f, 1.0f),
	textureWidth_(0),
	seperator_(false)
{

}

GLWSelector *GLWSelector::instance_ = 0;

GLWSelector *GLWSelector::instance()
{
	if (!instance_)
	{
		instance_ = new GLWSelector();
	}
	return instance_;
}

GLWSelector::GLWSelector() :
	GLWWindow("Selector", 0.0f, 0.0f, 0.0f, 0.0f, eHideName, ""), 
	showState_(0)
{
	visible_ = false;
	windowLevel_ = 1000;
}

GLWSelector::~GLWSelector()
{
}

void GLWSelector::showSelector(GLWSelectorI *user,
	float x, float y,
	std::list<GLWSelectorEntry> &entries,
	unsigned int showState,
	bool transparent)

{
	hideSelector();

	showState_ = showState;
	visible_ = true;
	w_ = 100000.0f;
	h_ = 100000.0f;	
	user_ = user;
	
	int basePosition = 0;
	float left = x;
	std::list<GLWSelectorEntry> partEntries;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = entries.begin();
		itor != entries.end();
		++itor)
	{
		partEntries.push_back(*itor);
		if ((int) partEntries.size() * 20 >= GLViewPort::getHeight() - 40)
		{
			GLWSelectorPart *part = new GLWSelectorPart(
				user, basePosition, 
				left, y, 
				partEntries,
				transparent,
				0, 0); // Parent
			left += part->getSelectedWidth() + 10.0f;
			addPart(part);
			basePosition += (int) partEntries.size();
			partEntries.clear();
		}
	}
	if (!partEntries.empty())
	{
		GLWSelectorPart *part = new GLWSelectorPart(
			user, basePosition,
			left, y, 
			partEntries,
			transparent,
			0, 0); // Parent
		addPart(part);
	}
}

void GLWSelector::hideSelector()
{
	visible_ = false;
	w_ = 0;
	h_ = 0; 
	user_ = 0;
	
	while (!parts_.empty())
	{
		GLWSelectorPart *part = parts_.front();
		parts_.pop_front();
		delete part;
	}
}

void GLWSelector::addPart(GLWSelectorPart *part)
{
	parts_.push_back(part);
}

void GLWSelector::rmPart(GLWSelectorPart *part)
{
	if (part->getChild()) rmPart(part->getChild());
	parts_.remove(part);
	delete part;
}

void GLWSelector::draw()
{
	if (!visible_) return;
	
	if (showState_ != 0 &&
		ScorchedClient::instance()->getGameState().getState() != showState_)
	{
		if (user_) user_->noItemSelected();
		hideSelector();
		return;
	}

	unsigned int state = 0;
	if (OptionsDisplay::instance()->getSmoothLines())
	{
		state |= GLState::BLEND_ON;
		glEnable(GL_LINE_SMOOTH);
	}
	GLState currentState(state);

	std::list<GLWSelectorPart *>::iterator itor;
	for (itor = parts_.begin();
		itor != parts_.end();
		++itor)
	{
		GLWSelectorPart *part = (*itor);
		part->draw();
	}

	if (OptionsDisplay::instance()->getSmoothLines())
	{
		glDisable(GL_LINE_SMOOTH);
	}
}

void GLWSelector::mouseDown(int button, float mouseX, float mouseY, bool &hitMenu)
{
	// Override default window behaviour
	if (!visible_) return;
	
	hitMenu = true;
	
	if ((showState_ != 0) &&
		(ScorchedClient::instance()->getGameState().getState() != showState_))
	{
		if (user_) user_->noItemSelected();
		hideSelector();
		return;
	}
	
	bool hit = false;
	std::list<GLWSelectorPart *>::reverse_iterator itor;
	for (itor = parts_.rbegin();
		itor != parts_.rend();
		++itor)
	{
		GLWSelectorPart *part = (*itor);
		part->mouseDown(mouseX, mouseY, hit);
		if (hit) break;
	}
	
	if (!hit && user_) user_->noItemSelected();

	hideSelector();
}

void GLWSelector::mouseUp(int button, float x, float y, bool &skipRest)
{
	// Override and disable default window behaviour
}

void GLWSelector::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	// Override and disable default window behaviour
}

void GLWSelector::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	// Override and disable default window behaviour
}
