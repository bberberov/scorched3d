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
#include <common/Keyboard.h>
#include <GLW/GLWButton.h>

GLWButtonI::~GLWButtonI()
{

}

REGISTER_CLASS_SOURCE(GLWButton);

GLWButton::GLWButton(float x, float y, float w, float h, GLWButtonI *handler,
					 unsigned flags) : 
	handler_(handler),
	GLWidget(x, y, w, h),
	flags_(flags), pressed_(false), startdrag_(false),
	repeatMode_(false), repeatTime_(0.0f),
	enabled_(true)
{

}

GLWButton::~GLWButton()
{

}

void GLWButton::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void GLWButton::setHandler(GLWButtonI *handler)
{ 
	handler_ = handler; 
}

void GLWButton::draw()
{
	GLWidget::draw();

	float size = 10.0f;
	if (w_ < 16 || h_ < 16) size = 6.0f;
	else if (w_ < 12 || h_ < 12) size = 4.0f;

	glLineWidth((flags_&ButtonFlagOk)?2.0f:1.0f);
	if (flags_&ButtonSquare)
	{
		glBegin(GL_LINE_LOOP);
			drawBox(x_, y_, w_, h_, !pressed_);
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_LOOP);
			drawShadedRoundBox(x_, y_, w_, h_, size, !pressed_);
		glEnd();
	}
	glLineWidth(1.0f);
}

void GLWButton::simulate(float frameTime)
{
	repeatTime_ += frameTime;
	if (repeatMode_)
	{
		const float RepeatTimeInterval = 0.15f;
		while (repeatTime_ > RepeatTimeInterval)
		{
			repeatTime_ -= RepeatTimeInterval;
			if (pressed_ && handler_) handler_->buttonDown(getId());
		}
	}
}

void GLWButton::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (enabled_ &&
		inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		startdrag_ = true;
		pressed_ = true;

		if (repeatMode_)
		{
			repeatTime_ = -0.85f;
			if (handler_) handler_->buttonDown(getId());
		}
	}
}

void GLWButton::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (startdrag_)
	{
		if (inBox(mx, my, x_, y_, w_, h_))
		{
			pressed_ = true;
			skipRest = true;
		}
		else
		{
			pressed_ = false;
		}
	}
}

void GLWButton::mouseUp(int button, float x, float y, bool &skipRest)
{
	startdrag_ = false;
	if (pressed_)
	{
		pressed_ = false;	
		skipRest = true;

		if (!repeatMode_)
		{
			if (handler_) handler_->buttonDown(getId());
		}
	}
}

void GLWButton::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (enabled_ && handler_ && 
		((flags_&ButtonFlagOk) || (flags_&ButtonFlagCancel)))
	{
		for (int i=0; i<hisCount; i++)
		{
			unsigned int dik = history[i].sdlKey;
			switch (dik)
			{
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					if ((flags_&ButtonFlagOk)) 
					{ 
						handler_->buttonDown(getId()); 
						skipRest = true; 
					}
					break;
				case SDLK_ESCAPE:
					if ((flags_&ButtonFlagCancel)) 
					{ 
						handler_->buttonDown(getId()); 
						skipRest = true; 
					}
					break;	
			}
		}
	}
}
