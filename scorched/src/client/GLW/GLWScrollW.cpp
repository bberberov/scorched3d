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

#include <GLW/GLWScrollW.h>
#include <GLEXT/GLState.h>

REGISTER_CLASS_SOURCE(GLWScrollW);

GLWScrollWI::~GLWScrollWI()
{

}

GLWScrollW::GLWScrollW(float x, float y, float h, int min, int max, int see, GLWScrollWI *handler) :
	GLWidget(x, y, 15.0f, h),
	min_(min), max_(max), see_(see), current_(min),
	handler_(handler), dragCurrent_(0),
	bottomButton_(x_ + 2.0f, y_ + 2.0f, w_ - 4.0f, w_ - 4.0f),
	topButton_(x_ + 2.0f, y_ + h_ - w_ + 2.0f, w_ - 4.0f, w_ - 4.0f),
	middleButton_(x_ + 2.0f, 0.0f, w_ - 4.0f, 0.0f),
	backButtonTop_(x_ + 2.0f, y_ + 2.0f, w_ - 4.0f, h_ - 4.0f),
	backButtonBot_(x_ + 2.0f, y_ + 2.0f, w_ - 4.0f, h_ - 4.0f)
{
	bottomButton_.setHandler(this);
	topButton_.setHandler(this);
	backButtonTop_.setHandler(this);
	backButtonBot_.setHandler(this);
	middleButton_.setScrollHandler(this);
	bottomButton_.setRepeatMode();
	topButton_.setRepeatMode();
}

GLWScrollW::~GLWScrollW()
{

}

void GLWScrollW::setX(float x)
{
	GLWidget::setX(x);

	topButton_.setX(x_+2.0f);
	bottomButton_.setX(x_+2.0f);
	middleButton_.setX(x_+2.0f);
	backButtonTop_.setX(x_+2.0f);
	backButtonBot_.setX(x_+2.0f);
}

void GLWScrollW::setY(float y)
{
	GLWidget::setY(y);

	bottomButton_.setY(y_ + 2.0f);
	topButton_.setY(y_ + h_ - w_ + 2.0f);
	middleButton_.setY(0.0f);
	backButtonTop_.setY(y_ + 2.0f);
	backButtonBot_.setY(y_ + 2.0f);
}

void GLWScrollW::setW(float w)
{
	GLWidget::setW(w);
}

void GLWScrollW::setH(float h)
{
	GLWidget::setH(h);

	topButton_.setY(y_ + h_ - w_ + 2.0f);
}

void GLWScrollW::draw()
{
	const float buttonWidth = w_ - 4.0f;
	const int possibilites = (max_ - min_) - see_;

	// Draw scroll indicator
	if (possibilites <= 0)
	{
		// Can see all possibilites
		middleButton_.setY(y_ + buttonWidth + 4.0f);
		middleButton_.setH(h_ - buttonWidth - buttonWidth - 8.0f);

		backButtonTop_.setH(0);
		backButtonBot_.setH(0);
	}
	else
	{
		float totalheight = (h_ - buttonWidth - buttonWidth - 8.0f);
		float percentageSeen = float(see_) / float(max_ - min_);
		float barSize = percentageSeen  * totalheight;

		float heightLeft = totalheight - barSize;
		float heightParts = heightLeft / possibilites;
		float pos = heightParts * current_;

		float midY = y_ + buttonWidth + 4.0f + pos;
		float midH = barSize;
		middleButton_.setY(midY);
		middleButton_.setH(midH);

		backButtonTop_.setY(midY);
		backButtonTop_.setH(y_ + h_ - midY);
		backButtonBot_.setY(y_);
		backButtonBot_.setH(midY - y_);
	}

	bottomButton_.draw();
	topButton_.draw();
	middleButton_.draw();
	// backButton_.draw(); // Should be hidden

	glBegin(GL_LINE_LOOP);
		// Draw surround
		drawShadedRoundBox(x_, y_, w_, h_, 4.0f, false);
	glEnd();

	glColor3f(0.2f, 0.2f, 0.2f);
	float bottomOffset = 0.0f;
	if (bottomButton_.getPressed()) bottomOffset = 1.0f; 
	float topOffset = 0.0f;
	if (topButton_.getPressed()) topOffset = 1.0f; 
	glBegin(GL_TRIANGLES);
		// Bottom Triangle
		glVertex2f(x_ + (buttonWidth / 2.0f) + 2.0f + bottomOffset, y_ + 4.0f - bottomOffset);
		glVertex2f(x_ + buttonWidth + bottomOffset, y_ + buttonWidth - 2.0f - bottomOffset);
		glVertex2f(x_ + 4 + bottomOffset, y_ + buttonWidth - 2.0f - bottomOffset);

		// Top Triangle
		glVertex2f(x_ + buttonWidth + topOffset, y_ + 2.0f + h_ - buttonWidth - topOffset);
		glVertex2f(x_ + (buttonWidth / 2.0f) + 2.0f + topOffset, y_ - 4.0f + h_ - topOffset);
		glVertex2f(x_ + 4 + topOffset, y_ + 2.0f + h_ - buttonWidth - topOffset);
	glEnd();
}

void GLWScrollW::simulate(float frameTime)
{
	topButton_.simulate(frameTime);
	bottomButton_.simulate(frameTime);
}

void GLWScrollW::mouseDown(int button, float x, float y, bool &skipRest)
{
	topButton_.mouseDown(button, x, y, skipRest);
	if (skipRest) return;

	bottomButton_.mouseDown(button, x, y, skipRest);
	if (skipRest) return;

	middleButton_.mouseDown(button, x, y, skipRest);
	if (skipRest) return;

	backButtonTop_.mouseDown(button, x, y, skipRest);
	if (skipRest) return;

	backButtonBot_.mouseDown(button, x, y, skipRest);
}

void GLWScrollW::mouseUp(int button, float x, float y, bool &skipRest)
{
	topButton_.mouseUp(button, x, y, skipRest);
	if (skipRest) return;

	bottomButton_.mouseUp(button, x, y, skipRest);
	if (skipRest) return;

	middleButton_.mouseUp(button, x, y, skipRest);
	if (skipRest) return;

	backButtonTop_.mouseUp(button, x, y, skipRest);
	if (skipRest) return;

	backButtonBot_.mouseUp(button, x, y, skipRest);
}

void GLWScrollW::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	topButton_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;

	bottomButton_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;

	middleButton_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;

	backButtonTop_.mouseDrag(button, mx, my, x, y, skipRest);
	if (skipRest) return;

	backButtonBot_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWScrollW::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (z < 0.0f) GLWScrollW::setCurrent(current_ + 1);
		else GLWScrollW::setCurrent(current_ - 1);
	}
}

void GLWScrollW::startDrag(unsigned int id)
{
	dragCurrent_ = current_;
}

void GLWScrollW::setCurrent(int newCurrent)
{
	if (newCurrent < min_) newCurrent = min_;
	else if (newCurrent > max_ - see_) newCurrent = max_ - see_;
	if (newCurrent < min_) newCurrent = min_; // Incase the max line changed this

	int diffCurrent = current_ - newCurrent;
	current_ = newCurrent;
	if (handler_) handler_->positionChange(getId(), current_, -diffCurrent);
}

void GLWScrollW::buttonDrag(unsigned int id, float x, float y)
{
	const int possibilites = (max_ - min_) - see_;
	if (possibilites > 0)
	{
		const float buttonWidth = w_ - 4.0f;
		float totalheight = (h_ - buttonWidth - buttonWidth - 8.0f);
		float percentageSeen = float(see_) / float(max_ - min_);
		float barSize = percentageSeen  * totalheight;

		float heightLeft = totalheight - barSize;
		float eachPosSize = heightLeft / float(possibilites);

		int diff = int(y / eachPosSize);
		int newCurrent = dragCurrent_ + diff;

		if (newCurrent < min_) newCurrent = min_;
		else if (newCurrent > max_ - see_) newCurrent = max_ - see_;

		if (newCurrent != current_)
		{
			GLWScrollW::setCurrent(newCurrent);
		}
	}
}

void GLWScrollW::buttonDown(unsigned int id)
{
	if (id == bottomButton_.getId())
	{
		const int possibilites = (max_ - min_) - see_;
		if (possibilites > 0)
		{
			if (current_ > min_) 
			{
				current_--;
				if (handler_) handler_->positionChange(getId(), current_, -1);
			}
		}
	}
	else if (id == topButton_.getId())
	{
		const int possibilites = (max_ - min_) - see_;
		if (possibilites > 0)
		{
			if (current_ < max_ - see_) 
			{
				current_++;
				if (handler_) handler_->positionChange(getId(), current_, +1);
			}
		}
	}
	else if (id == backButtonTop_.getId())
	{
		const int possibilites = (max_ - min_) - see_;
		if (possibilites > 0)
		{
			int oldcurrent = current_;
			if (current_ < max_ - see_)
			{
				current_ += see_;
				if (current_ > max_ - see_) current_ = max_ - see_;
				if (handler_) handler_->
					positionChange(getId(), current_, current_ - oldcurrent);
			}
		}
	}
	else if (id == backButtonBot_.getId())
	{
		const int possibilites = (max_ - min_) - see_;
		if (possibilites > 0)
		{
			int oldcurrent = current_;
			if (current_ > min_)
			{
				current_ -= see_;
				if (current_ < min_) current_ = min_;
				if (handler_) handler_->
					positionChange(getId(), current_, current_ - oldcurrent);
			}
		}
	}
}
