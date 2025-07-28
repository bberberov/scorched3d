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

#if !defined(__INCLUDE_GLWCheckBoxTexth_INCLUDE__)
#define __INCLUDE_GLWCheckBoxTexth_INCLUDE__

#include <GLW/GLWLabel.h>
#include <GLW/GLWCheckBox.h>

class GLWCheckBoxText : public GLWidget
{
public:
	GLWCheckBoxText(float x = 0.0f, float y = 0.0f, 
		const LangString &text = LangString(),
		bool startState = true,
		float offset = 0.0f);
	virtual ~GLWCheckBoxText();

	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);

	virtual float getW();
	virtual void setX(float x);
	virtual void setY(float y);

	GLWCheckBox &getCheckBox() { return box_; }
	GLWLabel &getLabel() { return label_; }

	REGISTER_CLASS_HEADER(GLWCheckBoxText);

protected:
	float offset_;
	GLWCheckBox box_;
	GLWLabel label_;
};

#endif
