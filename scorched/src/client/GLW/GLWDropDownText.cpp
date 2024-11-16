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

#include <GLW/GLWDropDownText.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(GLWDropDownText);

GLWDropDownText::GLWDropDownText(float x, float y, float w) :
	GLWDropDown(x, y, w)
{
}

GLWDropDownText::~GLWDropDownText()
{
}

void GLWDropDownText::addText(const LangString &text, const std::string &datatext)
{
	GLWSelectorEntry entry(text, 0, false, 0, 0, datatext);
	addEntry(entry);
}

LangString &GLWDropDownText::getCurrentText()
{
	static LangString empty;
	if (!getCurrentEntry()) return empty;
	return getCurrentEntry()->getText();
}

const char *GLWDropDownText::getCurrentDataText()
{
	if (!getCurrentEntry()) return "";
	return getCurrentEntry()->getDataText();
}

bool GLWDropDownText::isSelected(const LangString &text)
{
	LangString current = getCurrentText();
	return (current == text);
}

void GLWDropDownText::setCurrentText(const LangString &text)
{
	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		++itor)
	{
		GLWSelectorEntry &entry = *itor;
		if (entry.getText() == text)
		{
			current_ = &entry;
			break;
		}

		position++;
	}

	if (handler_)
	{
		handler_->select(id_, position, *current_);
	}
}

void GLWDropDownText::draw()
{
	GLWDropDown::draw();

	if (getCurrentEntry())
	{
		GLWFont::instance()->getGameFont()->drawWidth(
			w_ - 25.0f,
			GLWFont::widgetFontColor, 14,
			x_ + 5.0f, y_ + 5.0f, 0.0f,
			getCurrentEntry()->getText());
	}
}
