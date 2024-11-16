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
#include <GLW/GLWFont.h>
#include <GLW/GLWTextBox.h>
#include <GLW/GLWPanel.h>
#include <common/Keyboard.h>
#include <common/DefinesString.h>

REGISTER_CLASS_SOURCE(GLWTextBox);

GLWTextBox::GLWTextBox(float x, float y, float w, 
	const LangString &startText, unsigned int flags) :
	GLWidget(x, y, w, 25.0f), ctime_(0.0f),
	flags_(flags),
	cursor_(false), maxTextLen_(0), handler_(0),
	current_(true), allowUnicode_(true)
{
	setText(startText);
}

GLWTextBox::~GLWTextBox()
{

}

void GLWTextBox::simulate(float frameTime)
{
	ctime_ += frameTime;
	if (ctime_ > 0.5f)
	{
		ctime_ = 0.0f;
		cursor_ = !cursor_;
	}
}

void GLWTextBox::draw()
{
	GLWidget::draw();
	if (current_) glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 10.0f, false);
	glEnd();
	if (current_) glLineWidth(1.0f);

	LangString text = text_;
	if (flags_ & eFlagPassword)
	{
		text = LangString(text_.size(), '*');
	}

	GLWFont::instance()->getGameFont()->drawWidth(
		w_,
		GLWFont::widgetFontColor, 14,
		x_ + 5.0f, y_ + 5.0f, 0.0f, 
		(cursor_&&current_)?text + LANG_STRING("_"):text);
}

void GLWTextBox::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (!current_) return;

	// By default swallow all text
	skipRest = true;

	for (int i=0; i<hisCount; i++)
	{
		unsigned int unicodeKey = history[i].representedUnicode;
		unsigned int dik = history[i].sdlKey;
		if (dik == SDLK_BACKSPACE || dik == SDLK_DELETE)
		{
			if (!text_.empty())
			{
				text_ = text_.substr(0, text_.length() - 1);
				if (handler_) handler_->textChanged(id_, text_);
			}
		}
		else if (dik == SDLK_TAB)
		{
			// Find all text boxes in scope
			unsigned int position = 0;
			std::vector<GLWTextBox *> textBoxes;
			std::list<GLWPanel::GLWPanelEntry> &widgets = parent_->getWidgets();
			std::list<GLWPanel::GLWPanelEntry>::iterator itor;
			for (itor = widgets.begin();
				itor != widgets.end();
				++itor)
			{
				GLWPanel::GLWPanelEntry &entry = *itor;
				if (entry.widget->getMetaClassId() == getMetaClassId())
				{
					GLWTextBox *textBox = (GLWTextBox *) entry.widget;
					if (this == textBox) position = (unsigned int) textBoxes.size();
					textBoxes.push_back(textBox);
				}
			}

			// Select the next text box
			position++;
			if (position >= textBoxes.size()) position = 0;
			textBoxes[position]->setCurrent();

			break;
		}
		else if (unicodeKey >= ' ' && (unicodeKey <= 127 || allowUnicode_))
		{
			if ((maxTextLen_==0) || ((int) text_.size() < maxTextLen_))
			{
				text_ += unicodeKey;
				if (handler_) handler_->textChanged(id_, text_);
			}
		}
		else 
		{
			skipRest = false; // Don't swallow return or escape
		}
	}
}

std::string &GLWTextBox::getText() 
{
	static std::string result; 
	result = LangStringUtil::convertFromLang(text_); 
	return result;
}

void GLWTextBox::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		setCurrent();
	}
}

void GLWTextBox::setText(const LangString &text)
{ 
	text_ = text; 
	if (!allowUnicode_)
	{
		for (unsigned int *c=(unsigned int *) text.c_str(); *c; c++)
		{
			if (*c > 127) *c = '?';
		}
	}
	if (handler_) handler_->textChanged(id_, text_);
}

void GLWTextBox::setParent(GLWPanel *parent)
{
	GLWidget::setParent(parent);
	setCurrent();
}

void GLWTextBox::setCurrent()
{
	std::list<GLWPanel::GLWPanelEntry> &widgets = parent_->getWidgets();
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = widgets.begin();
		itor != widgets.end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = *itor;
		if (entry.widget->getMetaClassId() == getMetaClassId())
		{
			GLWTextBox *textBox = (GLWTextBox *) entry.widget;
			textBox->current_ = false;
		}
	}
	current_ = true;
}
