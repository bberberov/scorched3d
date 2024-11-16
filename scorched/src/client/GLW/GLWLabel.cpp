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

#include <GLW/GLWLabel.h>
#include <GLW/GLWFont.h>

REGISTER_CLASS_SOURCE(GLWLabel);

GLWLabel::GLWLabel(float x, float y, const LangString &labelText, 
	float size, unsigned int flags) : 
	GLWidget(x, y, 0.0f, 20.0f), size_(size),
	color_(GLWFont::widgetFontColor), flags_(flags)
{
	setText(labelText);
	tooltipTransparent_ = true;
}

GLWLabel::~GLWLabel()
{

}

void GLWLabel::setColor(const Vector &color)
{
	color_ = color;
}

void GLWLabel::setSize(float size)
{
	size_ = size;
	w_ = 0.0f;
}

void GLWLabel::setText(const LangString &text)
{ 
	labelText_ = text.c_str(); 
	if (flags_ & eMultiLine)
	{
		labelTexts_.clear();
		LangString current;
		for (unsigned int i=0; i<text.size(); i++)
		{
			if (text[i] != '\n') current.push_back(text[i]);
			else 
			{
				labelTexts_.push_back(current);
				current.clear();
			}
		}
		if (!current.empty())
		{
			labelTexts_.push_back(current);
		}
	}

	w_ = 0.0f;
}

std::string &GLWLabel::getText() 
{
	static std::string result; 
	result = LangStringUtil::convertFromLang(labelText_); 
	return result;
}

void GLWLabel::calcWidth()
{
	if (w_ == 0.0f)
	{
		if (flags_ & eMultiLine)
		{
			for (int i=0; i<(int) labelTexts_.size(); i++)
			{
				w_ = MAX(w_, GLWFont::instance()->getGameFont()->getWidth(
					size_, labelTexts_[i]));
			}
			h_ = float(labelTexts_.size()) * 20.0f / 14.0f * size_;
		}
		else
		{
			w_ = GLWFont::instance()->getGameFont()->getWidth(
				size_, labelText_);
			h_ = 20.0f / 14.0f * size_;
		}
	}
}

void GLWLabel::draw()
{
	GLWidget::draw();

	glColor3f(1.0f, 0.0f, 0.0f);
	calcWidth();

	if (flags_ & eMultiLine)
	{
		for (int i=0; i<(int) labelTexts_.size(); i++)
		{
			const LangString &text = labelTexts_[labelTexts_.size() - (i + 1)];
			GLWFont::instance()->getGameFont()->draw(
				color_, size_,
				x_, y_ + 6.0f + float(i) * 20.0f / 14.0f * size_, 0.0f, text);
		}
	}
	else
	{
		GLWFont::instance()->getGameFont()->draw(
			color_, size_, x_, y_ + 6.0f, 0.0f, labelText_);
	}
}
