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

#ifndef _GLWBUTTON_H_
#define _GLWBUTTON_H_

#include <GLW/GLWidget.h>

class GLWButtonI
{
public:
	virtual ~GLWButtonI();

	virtual void buttonDown(unsigned int id) = 0;
};

class GLWButton : public GLWidget
{
public:
	enum
	{
		ButtonFlagOk = 1,
		ButtonFlagCancel = 2,
		ButtonFlagCenterX = 4,
		ButtonSquare = 8
	};

	GLWButton(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f, 
		GLWButtonI *handler = 0,
		unsigned flags = 0);
	virtual ~GLWButton();

	virtual void draw();	
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	bool &getPressed() { return pressed_; }

	bool getEnabled() { return enabled_; }
	void setEnabled(bool enabled);

	void setFlags(unsigned f) { flags_ = f; }
	unsigned int getFlags() { return flags_; }

	virtual void setHandler(GLWButtonI *handler);
	void setRepeatMode() { repeatMode_ = true; }

	REGISTER_CLASS_HEADER(GLWButton);

protected:
	GLWButtonI *handler_;
	unsigned flags_;
	bool startdrag_, pressed_;
	bool repeatMode_;
	bool enabled_;
	float repeatTime_;

};

#endif /* _GLWBUTTON_H_ */
