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
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWToolTip.h>
#include <GLW/GLWidget.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <string.h>

static Vector color(0.1f, 0.1f, 0.4f);
static Vector selectedColor(0.9f, 0.9f, 1.0f);

GLWToolTip *GLWToolTip::instance_ = 0;

GLWToolTip *GLWToolTip::instance()
{
	if (!instance_)
	{
		instance_ = new GLWToolTip;
	}
	return instance_;
}

GLWToolTip::GLWToolTip() : 
	GameStateI("GLWToolTip"),
	lastTip_(0), currentTip_(0),
	timeDrawn_(0.0f), timeSeen_(0.0),
	refreshTime_(100.0f),
	tipX_(0.0f), tipY_(0.0f),
	tipW_(0.0f), tipH_(0.0f),
	tipOffX_(0.0f), tipOffY_(0.0f)
{
}

GLWToolTip::~GLWToolTip()
{
}

bool GLWToolTip::addToolTip(ToolTip::ToolTipType type, 
	const LangString &title, const LangString &text,
	float x, float y, float w, float h)
{
	if (!OptionsDisplay::instance()->getShowContextInfo() &&
		(type & ToolTip::ToolTipInfo)) return false;
	if (!OptionsDisplay::instance()->getShowContextHelp() &&
		(type & ToolTip::ToolTipHelp)) return false;

	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	
	bool result = false;
	if (x < mouseX && mouseX < x + w &&
		y < mouseY && mouseY < y + h)
	{
		static ToolTip singleTip;
		singleTip.setText(type, title, text);

		currentX_ = x;
		currentY_ = y;
		currentW_ = w;
		currentH_ = h;
		currentTip_ = &singleTip;

		result = true;
	}
	return result;
}

bool GLWToolTip::addToolTip(ToolTip *tip, float x, float y, float w, float h)
{
	if (!OptionsDisplay::instance()->getShowContextInfo() &&
		(tip->getType() & ToolTip::ToolTipInfo)) return false;
	if (!OptionsDisplay::instance()->getShowContextHelp() &&
		(tip->getType() & ToolTip::ToolTipHelp)) return false;

	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	
	bool result = false;
	if (x < mouseX && mouseX < x + w &&
		y < mouseY && mouseY < y + h)
	{
		currentX_ = x;
		currentY_ = y;
		currentW_ = w;
		currentH_ = h;
		currentTip_ = tip;

		result = true;
	}
	return result;
}

void GLWToolTip::setupTip(ToolTip *tip)
{
	currentTip_ = tip;
	tipTextWidth_ = 0.0f;
	tipTextHeight_ = 0.0f;
	tipTitle_ = tip->getTitle();
	tipText_ = tip->getText();
	tipTexts_.clear();
}

void GLWToolTip::calculateTip(ToolTip *tip)
{
	if (tipTextWidth_ != 0.0f) return;

	tipTextHeight_ = 24.0f;

	size_t pos, startpos = 0;
	LangString tipText = tipText_;
	tipText.append(LANG_STRING("\n"));
	while ((pos = tipText.find(LANG_STRING("\n"), startpos)) != LangString::npos)
	{
		LangString part = LangString(tipText, startpos, pos - startpos);
		tipTexts_.push_back(part);
		tipTextHeight_ += 10.0f;

		startpos = pos + 1;
	}

	std::list<LangString>::iterator itor;
	std::list<LangString>::iterator enditor = tipTexts_.end();
	for (itor = tipTexts_.begin(); itor != enditor; ++itor)
	{
		float width = float(GLWFont::instance()->getGameFont()->
			getWidth(9,(*itor))) + 10.0f;
		if (width > tipTextWidth_) tipTextWidth_ = width;
	}

	float width = float(GLWFont::instance()->getGameFont()->
		getWidth(11, tipTitle_)) + 10.0f; 
	if (width > tipTextWidth_) tipTextWidth_ = width;
}

void GLWToolTip::clearToolTip(float x, float y, float w, float h)
{
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	
	if (x < mouseX && mouseX < x + w &&
		y < mouseY && mouseY < y + h)
	{
		currentTip_ = 0;
	}
}

void GLWToolTip::simulate(const unsigned state, float frameTime)
{
	timeDrawn_ += frameTime;
}

void GLWToolTip::draw(const unsigned state)
{
	if (currentTip_ != lastTip_) refreshTime_ = 100.0f;
	lastTip_ = currentTip_;
	currentTip_ = 0;

	if (lastTip_) timeSeen_ += timeDrawn_;
	else timeSeen_ -= timeDrawn_;
	refreshTime_ += timeDrawn_;
	timeDrawn_ = 0.0f;

	float showTime = 
		float(OptionsDisplay::instance()->getToolTipTime()) / 1000.0f;
	if (timeSeen_ <= -showTime)
	{
		timeSeen_ = -showTime;
		return;
	}

	if ((refreshTime_ > 1.0f || 
		tipX_ != currentX_ || 
		tipY_ != currentY_) && lastTip_)
	{
		tipX_ = currentX_;
		tipY_ = currentY_;
		tipW_ = currentW_;
		tipH_ = currentH_;

		lastTip_->populate();
		setupTip(lastTip_);
		refreshTime_ = 0.0f;
	}
	if (lastTip_) 
	{
		calculateTip(lastTip_);

		if (lastTip_->getType() & ToolTip::ToolTipAlignLeft)
			tipOffX_ = -currentW_ - tipTextWidth_ - 10;
		else tipOffX_ = 0.0f;

		if (lastTip_->getType() & ToolTip::ToolTipAlignBottom)
			tipOffY_ = 15.0f;
		else tipOffY_ = 0.0f;
	}

	float alpha = timeSeen_ * 
		float(OptionsDisplay::instance()->getToolTipSpeed());
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
		timeSeen_ = 1.0f / 
			float(OptionsDisplay::instance()->getToolTipSpeed());
	}

	GLState currentState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	float posX = tipX_ + tipOffX_;
	float posY = tipY_ + tipOffY_;
	float posW = tipTextWidth_;
	float posH = tipTextHeight_;

	int camWidth = GLViewPort::getWidth();
	if (posX > camWidth / 2)
	{
		posX -= posW + 5.0f;
	}
	else
	{
		posX += tipW_ + 5.0f;
	}
	int camHeight = GLViewPort::getHeight();
	if (posY > camHeight / 2)
	{
		posY -= posH;
	}
	else
	{
		posY += 15.0f;
	}

	if (posX < 0) posX = 0;
	else if (posX + posW > camWidth) posX -= posX + posW - camWidth;

	{
		if (OptionsDisplay::instance()->getSmoothLines())
		{
			glEnable(GL_LINE_SMOOTH);
		}

		GLState currentStateBlend(GLState::BLEND_ON);
		glColor4f(0.5f, 0.5f, 1.0f, 0.8f * alpha);	
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(posX + 10.0f, posY + 2.0f);
			glVertex2f(posX + 10.0f, posY);
			GLWidget::drawRoundBox(
				posX, posY,
				posW, posH, 10.0f);
			glVertex2f(posX + 10.0f, posY);
		glEnd();
		glColor4f(0.9f, 0.9f, 1.0f, 0.5f * alpha);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
			GLWidget::drawRoundBox(
				posX, posY,
				posW, posH, 10.0f);
		glEnd();
		glLineWidth(1.0f);

		if (OptionsDisplay::instance()->getSmoothLines())
		{
			glDisable(GL_LINE_SMOOTH);
		}
	}

	float pos = posY + posH - 16.0f;
	GLWFont::instance()->getGameFont()->drawA(selectedColor, alpha, 11, posX + 3.0f, 
		pos, 0.0f, tipTitle_);
	pos -= 2.0f;

	std::list<LangString>::iterator itor;
	std::list<LangString>::iterator enditor = tipTexts_.end();
	for (itor = tipTexts_.begin(); itor != enditor; ++itor)
	{
		pos -= 10.0f;

		GLWFont::instance()->getGameFont()->drawA(
			color, alpha, 9, posX + 6.0f, 
			pos, 0.0f, (*itor));
	}
}
