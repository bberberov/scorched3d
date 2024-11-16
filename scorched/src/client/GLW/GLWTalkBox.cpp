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

#include <GLW/GLWTalkBox.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWWindowManager.h>
#include <GLEXT/GLState.h>
#include <XML/XMLParser.h>
#include <console/Console.h>
#include <common/Defines.h>
#include <client/ClientChannelManager.h>

REGISTER_CLASS_SOURCE(GLWTalkBox);

GLWTalkBox::GLWTalkBox() :
	ctime_(0.0f), cursor_(false), maxTextLen_(0),
	parentSized_(false), mode_(eSay), defaultMode_(eSay)
{
}

GLWTalkBox::~GLWTalkBox()
{
}

void GLWTalkBox::simulate(float frameTime)
{
	ctime_ += frameTime;
	if (ctime_ > 0.5f)
	{
		ctime_ = 0.0f;
		cursor_ = !cursor_;
	}
}

void GLWTalkBox::draw()
{
	if (parent_ && parentSized_)
	{
		w_ = parent_->getW();
		h_ = parent_->getH();
	}

	GLWidget::draw();
	glColor4f(0.4f, 0.6f, 0.8f, 0.6f);

	{
		GLState currentStateBlend(GLState::BLEND_ON);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x_ + 20.0f, y_ + 5.0f);
			glVertex2f(x_ + 20.0f, y_);
			drawRoundBox(x_, y_, w_, h_, 10.0f);
			glVertex2f(x_ + 20.0f, y_);
		glEnd();
	}

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		drawRoundBox(x_, y_, w_, h_, 10.0f);
	glEnd();

	Vector white(0.9f, 0.9f, 1.0f);
	GLWFont::instance()->getGameFont()->drawWidthRhs(
		w_ - 10.0f,
		white, 12,
		x_ + 5.0f, y_ + 5.0f, 0.0f, 
		S3D::formatStringBuffer("%s: %s%s", 
		((mode_ == eSay)?"Say":"Team Say"), text_.c_str(), cursor_?" ":"_"));
}

void GLWTalkBox::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	for (int i=0; i<hisCount; i++)
	{
		unsigned int dik = history[i].sdlKey;
		unsigned int unicodeChar = history[i].representedUnicode;
		if (dik == SDLK_BACKSPACE || dik == SDLK_DELETE)
		{
			if (!text_.empty())
			{
				text_ = text_.substr(0, text_.length() - 1);
			}
		}
		else if (dik == SDLK_ESCAPE)
		{
			GLWWindowManager::instance()->hideWindow(parent_->getId());
		}
		else if (dik == SDLK_RETURN)
		{
			if (!text_.empty())
			{
				for (unsigned int *text = (unsigned int *) text_.c_str();
					*text;
					text++)
				{
					if (*text == '#') *text = '\'';
				}

				ChannelText message("general", text_);
				ClientChannelManager::instance()->sendText(message);

				text_.clear();
			}
			GLWWindowManager::instance()->hideWindow(parent_->getId());
		}
		else if (unicodeChar >= ' ')
		{
			if ((maxTextLen_==0) || ((int) text_.size() < maxTextLen_))
			{
				text_ += unicodeChar;

				if (text_[0] == '\\' || text_[0] == '/')
				{
					LangString part(&text_[1]);

					if (part == LANG_STRING("say ") ||
						part == LANG_STRING("s "))
					{
						text_.clear();
						mode_ = eSay;
					}
					else if (part == LANG_STRING("team ") ||
						part == LANG_STRING("t "))
					{
						text_.clear();
						mode_ = eTeamSay;
					}
				}
			}
		}
	}
	skipRest = true;
}

void GLWTalkBox::display()
{
	mode_ = defaultMode_;
	text_.clear();
}

bool GLWTalkBox::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	if (!node->getNamedChild("parentsized", parentSized_)) return false;

	std::string mode;
	if (!node->getNamedChild("mode", mode)) return false;
	defaultMode_ = ((0 == strcmp(mode.c_str(), "say"))?eSay:eTeamSay);

	return true;
}
