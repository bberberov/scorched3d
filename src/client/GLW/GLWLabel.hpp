////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_GLWLabel_hpp_INCLUDE__
#define __INCLUDE_GLWLabel_hpp_INCLUDE__

#include <string>
#include <vector>
#include <GLW/GLWidget.hpp>
#include <common/Vector.hpp>
#include <lang/LangResource.hpp>

class GLWLabel : public GLWidget
{
public:
	enum LabelFlags
	{
		eMultiLine = 1
	};

	GLWLabel(
		float x = 0.0f,
		float y = 0.0f,
		const LangString &labelText = LangString(),
		float size = 14.0f,
		unsigned int flags = 0
	);
	virtual ~GLWLabel();

	virtual float getW() { calcWidth(); return GLWidget::getW(); }
	virtual float getH() { calcWidth(); return GLWidget::getH(); }
	virtual void draw();

	bool getEmpty() { return labelText_.empty() && labelTexts_.size() == 0; }

	void setText(const LangString &text);
	void setColor(const Vector &color);
	void setSize(float size);
	void calcWidth();

	std::string &getText();
	LangString &getLangString() { return labelText_; }

	REGISTER_CLASS_HEADER(GLWLabel);

protected:
	float size_;
	unsigned int flags_;
	Vector color_;
	LangString labelText_;
	std::vector<LangString> labelTexts_;
};

#endif // __INCLUDE_GLWLabel_hpp_INCLUDE__
