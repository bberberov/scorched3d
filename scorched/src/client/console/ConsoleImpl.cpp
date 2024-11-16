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

#include <console/ConsoleImpl.h>
#include <common/Keyboard.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWToolTip.h>
#include <limits.h>

ConsoleImpl::ConsoleImpl() : 
	GameStateI("Console"),
	height_(0.0f), opening_(false), 
	lines_(1000), historyPosition_(-1), font_(0),
	showCursor_(true)
{
	Logger::addLogger(this);
}

ConsoleImpl::~ConsoleImpl()
{

}

void ConsoleImpl::init()
{
	methods_.init();
}

void ConsoleImpl::logMessage(LoggerInfo &info)
{
	addLine(false, info.getMessage());
}

void ConsoleImpl::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, int hisCount, 
							   bool &skipRest)
{
	// Use the history to ensure that no keystrokes are missed
	// regardless of the framerate.
	KEYBOARDKEY("CONSOLE", consoleKey);
	for (int i=0; i<hisCount; i++)
	{
		unsigned int dik = history[i].sdlKey;
		if (consoleKey->keyMatch(dik))
		{
			resetPositions();
			if (currentLine_.empty()) opening_ = !opening_;
			else currentLine_ = "";
			skipRest = true;
		}
	}

	if (height_ > 0.0f)
	{
		// Use the history to ensure that no keystrokes are missed
		// regardless of the framerate.
		if (!skipRest)
		for (int i=0; i<hisCount; i++)
		{
			char unicodeKey = history[i].representedUnicode;
			unsigned int dik = history[i].sdlKey;

			if (unicodeKey >= ' ')
			{
				resetPositions();
				if (unicodeKey < 127) currentLine_ += (char) unicodeKey;
			}
			else 
			{
				switch (dik)
				{
				case SDLK_ESCAPE:
					resetPositions();
					if (currentLine_.empty()) opening_ = false;
					else currentLine_ = "";
					break;
				case SDLK_TAB:
					resetPositions();
					{
						std::vector<ConsoleRule *> matches;
						std::string result = 
							rules_.matchRule(currentLine_.c_str(), matches);
						if (result.length() > 0) currentLine_ = result;

						addLine(false, "-------------------");
						std::vector<ConsoleRule *>::iterator itor;
						for (itor = matches.begin();
							itor != matches.end();
							++itor)
						{
							std::string text = (*itor)->toString();
							addLine(false, text.c_str());
						}
					}
					break;
				case SDLK_BACKSPACE:
				case SDLK_DELETE:
					resetPositions();
					currentLine_ = currentLine_.substr(0, currentLine_.length() - 1);
					break;
				case SDLK_RETURN:
					if (!currentLine_.empty())
					{
						resetPositions();
						addLine(true, currentLine_.c_str());
						history_.push_front(currentLine_.c_str());
						currentLine_.erase();
					}	
					break;
				case SDLK_UP:
					historyPosition_++;
					if (historyPosition_ >= int(history_.size()))
						historyPosition_ = int(history_.size()) - 1;
					if (historyPosition_ >= 0) 
						currentLine_ = history_[historyPosition_];
					break;
				case SDLK_DOWN:
					historyPosition_--;
					if (historyPosition_ <= 0)
						historyPosition_ = (history_.empty()?-1:0);
					if (historyPosition_ >= 0) 
						currentLine_ = history_[historyPosition_];
					break;
				case SDLK_PAGEUP:
					lines_.scroll(-5);
					break;
				case SDLK_PAGEDOWN:
					lines_.scroll(+5);
					break;	
				case SDLK_HOME:
					lines_.scroll(INT_MIN);
					break;
				case SDLK_END:
					lines_.scroll(INT_MAX);
					break;
				}
			}
		}
		skipRest = true;
	}
	else
	{
		std::list<KeyboardKey *> &keys = Keyboard::instance()->getCommandKeys();
		std::list<KeyboardKey *>::iterator keyItor = keys.begin();
		std::list<KeyboardKey *>::iterator keyEndItor = keys.end();
		for (; keyItor != keyEndItor; ++keyItor)
		{
			KeyboardKey *key = (*keyItor);
			if (key->keyDown(buffer, keyState, false))
			{
				resetPositions();
				addLine(true, key->getName());
			}
		}
	}
}

void ConsoleImpl::resetPositions()
{
	historyPosition_ = -1;
	lines_.resetScroll();
}

void ConsoleImpl::simulate(const unsigned state, float frameTime)
{
	const GLfloat dropSpeed = 1200.0f;
	if (opening_)
	{
		height_ += frameTime * dropSpeed;
	}
	else
	{
		height_ -= frameTime * dropSpeed;
		if (height_ < 0.0f) height_ = 0.0f;
	}

	static float ctime = 0;
	ctime += frameTime;
	if (ctime > 0.5f)
	{
		ctime = 0.0f;
		showCursor_ = !showCursor_;
	}
}

void ConsoleImpl::draw(const unsigned state)
{
	if (height_ <= 0.0f) return;

	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF | GLState::BLEND_ON);

	GLfloat width = (GLfloat) GLViewPort::getWidth();
	GLfloat top = (GLfloat) GLViewPort::getHeight();

	drawBackdrop(width, top);
	drawText(width, top);
}

void ConsoleImpl::drawBackdrop(float width, float top)
{
	if (height_ > top * .85f) height_ = top * .85f;

	GLWToolTip::instance()->clearToolTip(
		0.0f, top - height_ + 10.0f,
		width, height_);

	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, top - height_ + 10.0f);
		glVertex2f(width, top - height_ + 10.0f);
		glVertex2f(width, top);
		glVertex2f(0.0f, top);

		glVertex2f(10.0f, top - height_);
		glVertex2f(width - 10.0f, top - height_);
		glVertex2f(width, top - height_ + 10.0f);
		glVertex2f(0.0f, top - height_ + 10.0f);
	glEnd();

	glColor4f(0.1f, 0.1f, 0.1f, 1.0f);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0.0f, top - height_ + 10.0f);
		glVertex2f(10.0f, top - height_);
		glVertex2f(width - 10.0f, top - height_);
		glVertex2f(width, top - height_ + 10.0f);
	glEnd();
}

void ConsoleImpl::drawText(float width, float top)
{
	static Vector color(1.0f, 1.0f, 1.0f);
	if (!font_) font_ = GLWFont::instance()->getCourierFont();
	font_->draw(color, 14,
		10.0f, top - (height_ - 14.0f), 0.0f, 
		S3D::formatStringBuffer("> %s%c", 
		currentLine_.c_str(),
		(showCursor_?'_':' ')));

	lines_.drawLines(font_, top - (height_ - 14.0f), top, width);
}

void ConsoleImpl::addLine(bool parse, const std::string &text)
{
	lines_.addLine(text, parse);
	if (parse)
	{
		rules_.addLine(this, text.c_str());
	}
}

void ConsoleImpl::help()
{
	std::vector<std::string>::iterator itor;
	std::vector<std::string> result;
	rules_.dump(result);

	for (itor = result.begin();
		itor != result.end();
		++itor)
	{
		addLine(false, *itor);
	}
}
