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

#include <GLW/GLWSelectorPart.h>
#include <GLW/GLWSelector.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>

GLWSelectorPart::GLWSelectorPart(GLWSelectorI *user,
	int basePosition,
	float x, float y,
	std::list<GLWSelectorEntry> &entries,
	bool transparent,
	GLWSelectorPart *parent,
	int parentPosition) :
	user_(user),
	entries_(entries),
	transparent_(transparent),
	basePosition_(basePosition),
	parent_(parent), parentPosition_(parentPosition),
	child_(0)
{
	calculateDimensions(x, y);
}

GLWSelectorPart::~GLWSelectorPart()
{
}

void GLWSelectorPart::calculateDimensions(float drawX, float drawY)
{
	// Get the height and width of the selector
	GLFont2d &font = *GLWFont::instance()->getGameFont();
	float selectedHeight = 10.0f; // Padding
	float selectedWidth = 0.0f;
	float iconWidth = 0.0f;
	hasSelectedEntry_ = false;
	hasPopupEntry_ = false;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor =	entries_.begin();
		itor != entries_.end();
		++itor)
	{
		// Get height
		GLWSelectorEntry &item = (*itor);
		if (item.getSeperator()) selectedHeight += 8.0f;
		else selectedHeight += 18.0f;

		// Get width
		float currentwidth = 10.0f;
		if (!item.getText().empty())
		{
			currentwidth = (float) font.getWidth(12, item.getText()) + 20.0f;
		}
		if (item.getSelected()) hasSelectedEntry_ = true;
		if (!item.getPopups().empty()) hasPopupEntry_ = true;
		if (item.getIcon()) iconWidth = item.getTextureWidth() + 16.0f;
		if (currentwidth > selectedWidth) selectedWidth = currentwidth;
	}
	float indent = 0.0f;
	if (hasSelectedEntry_) indent += 10.0f;
	if (hasPopupEntry_) selectedWidth += 10.0f;
	indent += iconWidth;
	selectedWidth += indent;

	float selectedX = drawX;
	float selectedY = drawY;
	if (selectedX + selectedWidth > GLViewPort::getWidth())
	{
		selectedX -= (selectedX + selectedWidth) - GLViewPort::getWidth();
	}
	else if (selectedX < 0.0f) selectedX = 0.0f;
	if (selectedY - selectedHeight < 0)
	{
		selectedY -= (selectedY - selectedHeight);
	}

	selectedWidth_ = selectedWidth;
	selectedHeight_ = selectedHeight;
	selectedX_ = selectedX;
	selectedY_ = selectedY;
	selectedIndent_ = indent;
}

void GLWSelectorPart::draw()
{
	GLState currentStateBlend(GLState::TEXTURE_OFF | GLState::DEPTH_OFF | GLState::BLEND_ON);

	GLFont2d &font = *GLWFont::instance()->getGameFont();
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();

	// Draw the background
	{
		if (transparent_) glColor4f(0.4f, 0.6f, 0.8f, 0.6f);
		else glColor3f(0.8f, 0.8f, 1.0f);

		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - 25.0f + 5.0f);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - selectedHeight_ + 5.0f);
			GLWidget::drawRoundBox(
				selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
				selectedWidth_, selectedHeight_, 10.0f);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - selectedHeight_ + 5.0f);
		glEnd();

		glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
			if (transparent_)
			{
				GLWidget::drawRoundBox(
					selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
					selectedWidth_, selectedHeight_, 10.0f);
			}
			else
			{
				GLWidget::drawShadedRoundBox(
					selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
					selectedWidth_, selectedHeight_, 10.0f, true);
			}
		glEnd();
		glLineWidth(1.0f);
	}

	GLWToolTip::instance()->clearToolTip(
		selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
		selectedWidth_, selectedHeight_);

	// Draw the menu items
	float currentTop = selectedY_;
	int position = 1;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor =	entries_.begin();
		itor != entries_.end();
		++itor, position++)
	{
		GLWSelectorEntry &item = (*itor);

		// Check if the item is a seperator
		if (item.getSeperator())
		{
			// Draw a seperator
			glBegin(GL_LINES);
				glColor3f(0.0f, 0.0f, 0.0f);
				glVertex2f(selectedX_ + 5.0f, currentTop - 5.0f);
				glVertex2f(selectedX_ + selectedWidth_ - 5.0f, currentTop - 5.0f);
			glEnd();
			currentTop -= 8.0f;
		}
		else
		{
			// Draw the actual line
			bool selected = false;
			if (selectedX_ < mouseX && mouseX < selectedX_ + selectedWidth_ &&
				currentTop - 18.0f <= mouseY && mouseY < currentTop)
			{
				// This item is selected
				selected = true;

				// Check popup status
				if (!item.getPopups().empty())
				{
					// This is a popup entry, make sure
					// 1) This popup is shown
					// 2) Any popups that have this as a parent are removed
					if (child_ && child_->getParentPosition() != position)
					{
						// This child is incorrect, remove it
						GLWSelector::instance()->rmPart(child_);
						child_ = 0;
					}
					if (!child_)
					{
						// There is no child, create it
						child_ = new GLWSelectorPart(
							GLWSelector::instance()->getUser(),
							10000, 
							selectedX_ + selectedWidth_, currentTop,
							item.getPopups(),
							transparent_,
							this, 
							position);
						GLWSelector::instance()->addPart(child_);
					}
				}
				else
				{
					// This is not a popup entry, make sure
					// 1) Any popups that have this as a parent are removed
					if (child_)
					{
						GLWSelector::instance()->rmPart(child_);
						child_ = 0;
					}
				}
			}
			if (child_ && child_->getParentPosition() == position)
			{
				selected = true;
			}

			if (item.getToolTip())
			{
				GLWToolTip::instance()->addToolTip(item.getToolTip(),
					selectedX_, currentTop - 18.0f, selectedWidth_, 18.0f);
			}

			if (item.getIcon())
			{
				glColor3f(item.getColor()[0], item.getColor()[1], item.getColor()[2]);
				GLState textureOn(GLState::TEXTURE_ON);
				item.getIcon()->draw(true);

				float x = selectedX_ + (hasSelectedEntry_?15.0f:5.0f);
				float y = currentTop - 19.0f;
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex2f(x, y);
					glTexCoord2f(1.0f, 0.0f);
					glVertex2f(x + item.getTextureWidth() + 16.0f, y);
					glTexCoord2f(1.0f, 1.0f);
					glVertex2f(x + item.getTextureWidth() + 16.0f, y + 16.0f);
					glTexCoord2f(0.0f, 1.0f);
					glVertex2f(x, y + 16.0f);
				glEnd();
			}

			static Vector color(0.9f, 0.9f, 1.0f);
			static Vector itemcolor(0.1f, 0.1f, 0.4f);
			static Vector selectedColor(0.3f, 0.3f, 0.7f);
			Vector *c = 0;
			if (transparent_) c = (selected?&color:&itemcolor);
			else c = (selected?&selectedColor:&GLWFont::widgetFontColor);

			if (item.getSelected())
			{
				font.draw(*c, 12, selectedX_ + 5.0f, 
					currentTop - 16.0f, 0.0f, "x");
			}
			font.draw(*c, 12, selectedX_ + selectedIndent_ + 10.0f, 
				currentTop - 16.0f, 0.0f, item.getText());
			if (!item.getPopups().empty())
			{
				font.draw(*c, 12, selectedX_ + selectedWidth_ - 15.0f, 
					currentTop - 16.0f, 0.0f, ">");				
			}
			currentTop -= 18.0f;
		}
	}
}

void GLWSelectorPart::mouseDown(float mouseX, float mouseY, bool &hit)
{
	bool thisMenu = (mouseX > selectedX_ && mouseX < selectedX_ + selectedWidth_ && 
		mouseY < selectedY_ && mouseY > selectedY_ - selectedHeight_);
	if (thisMenu)
	{
		// Draw the menu items
		int position = 0;
		float currentTop = selectedY_;
		std::list<GLWSelectorEntry>::iterator itor;
		for (itor =	entries_.begin();
			 itor != entries_.end();
			 ++itor)
		{
			GLWSelectorEntry &item = (*itor);
		
			// Check if the item is a seperator
			if (item.getSeperator())
			{
				position++;
				currentTop -= 8.0f;
			}
			else
			{
				// Check if the item is selected
				if (selectedX_ < mouseX && mouseX < selectedX_ + selectedWidth_ &&
					currentTop - 18.0f <= mouseY && mouseY < currentTop)
				{
					hit = true;
					if (user_) user_->itemSelected(&item, 
						basePosition_ + position);
				}

				position++;
				currentTop -= 18.0f;
			}
		}
	}
}
