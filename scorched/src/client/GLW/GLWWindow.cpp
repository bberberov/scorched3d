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
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLViewPort.h>
#include <image/ImageFactory.h>
#include <XML/XMLParser.h>
#include <GLW/GLWWindow.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <lang/LangResource.h>

static const float roundSize = 20.0f;
static const float smallRoundSize = 10.0f;
static const float smallTitleHeight = 14.0f;
static const float minWindowSize = 50.0f;
static const float titleWidth = 100.0f;
static const float titleHeight = 20.0f;
static const float shadowWidth = 10.0f;

GLTextureReference GLWWindow::moveTexture_(ImageID(S3D::eModLocation,"", "data/windows/move.bmp"));
GLTextureReference GLWWindow::resizeTexture_(ImageID(S3D::eModLocation, "", "data/windows/resize.bmp"));

GLWWindow::GLWWindow(const std::string &name, float x, float y, 
					 float w, float h,
					 unsigned int states,
					 const std::string &description) : 
	GLWPanel(x, y, w, h), dragging_(NoDrag), 
	needCentered_(false), showTitle_(false), 
	disabled_(false), windowState_(states), maxWindowSize_(0.0f),
	description_(description), toolTip_(ToolTip::ToolTipHelp, 
	LANG_RESOURCE(name, name), LANG_RESOURCE(name + "_window", description)),
	initPosition_(false), windowLevel_(100000)
{
	setName(name);
	getDrawPanel() = false;
}

GLWWindow::GLWWindow(const std::string &name, float w, float h,
					 unsigned int states,
					 const std::string &description) :
	GLWPanel(0.0f, 0.0f, w, h), dragging_(NoDrag), 
	needCentered_(true), showTitle_(false), 
	disabled_(false), windowState_(states),
	description_(description), toolTip_(ToolTip::ToolTipHelp, 
	LANG_RESOURCE(name, name), LANG_RESOURCE(name + "_window", description)),
	initPosition_(false), windowLevel_(100000)
{
	setName(name);
	getDrawPanel() = false;
}

GLWWindow::~GLWWindow()
{

}

void GLWWindow::windowInit(const unsigned state)
{

}

void GLWWindow::drawOutlinePoints(float x, float y, float w, float h)
{
	float tw = titleWidth;
	if (w < titleWidth + 25.0f) tw = w - 25.0f;

	float th = titleHeight;
	float sr = roundSize;
	float sz = 20.0f;
	if (windowState_ & eSmallTitle)
	{
		th = smallTitleHeight;
		sr = smallRoundSize;
		sz = 10.0f;
	}

	if (windowState_ & eResizeable)
		drawCircle(8, 4, x + w - smallRoundSize, y + smallRoundSize, 10.0f);
	else
		drawCircle(8, 4, x + w - roundSize, y + roundSize, 20.0f);
	drawCircle(4, 0, x + w - roundSize, y + h - roundSize, 20.0f);
	if (((tw > 70.0f) || (tw > 50.0f && windowState_ & eSmallTitle)) && 
		!(windowState_ & eNoTitle))
	{
		showTitle_ = true;
		drawCircle(8, 10, x + tw, y + h + sr, sz);
		drawCircle(2, 0, x + tw - sr - sr, y + h + th - sr, sz);
		drawCircle(0, -4, x + roundSize, y + h + th - roundSize, 20.0f);
	}
	else
	{
		showTitle_ = false;
		drawCircle(0, -4, x + roundSize, y + h - roundSize, 20.0f);
	}
	drawCircle(-4, -8, x + roundSize, y + roundSize, 20.0f);
}

void GLWWindow::drawBackSurface(float x, float y, float w, float h)
{
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x + roundSize, y + roundSize);
		glVertex2f(x + roundSize, y);
		drawOutlinePoints(x, y, w, h);
		glVertex2f(x + roundSize, y);
	glEnd();
}

void GLWWindow::drawSurround(float x, float y, float w, float h)
{
	glBegin(GL_LINE_LOOP);
		drawOutlinePoints(x, y, w, h);
	glEnd();
}

void GLWWindow::drawTitleBar(float x, float y, float w, float h)
{
	// Draw the indented lines in the header
	glBegin(GL_LINES);
		for (int i=0; i<=8; i+=4)
		{
			float top = y + h + titleHeight - i - 6.0f;

			glColor3f(0.4f, 0.4f, 0.6f);
			glVertex2f(x + 10.0f, top);
			glVertex2f(x + titleWidth - 32.0f, top);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x + 10.0f, top - 1.0f);
			glVertex2f(x + titleWidth - 32.0f, top - 1.0f);
		}
	glEnd();
}

void GLWWindow::drawWindowCircle(float x, float y, float w, float h)
{
	glColor4f(0.5f, 0.5f, 1.0f, 0.3f);	
	float halfWidth = w / 2.0f;
	float halfHeight = h / 2.0f;
	glPushMatrix();
		glTranslatef(x + halfWidth, y + halfHeight, 0.0f);
		glScalef(halfWidth, halfHeight, 0.0f);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(0.0f, 0.0f);
			drawWholeCircle(true);
		glEnd();
		glColor4f(0.9f, 0.9f, 1.0f, 0.5f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
			drawWholeCircle(false);
		glEnd();
		glLineWidth(1.0f);
	glPopMatrix();
}

void GLWWindow::drawMaximizedWindow()
{
	if (windowState_ & eCircle ||
		windowState_ & eNoDraw)
	{
		if (windowState_ & eCircle)
		{
			GLState state(GLState::BLEND_ON);
			drawWindowCircle(x_, y_, w_, h_);
		}

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();

		int x = ScorchedClient::instance()->getGameState().getMouseX();
		int y = ScorchedClient::instance()->getGameState().getMouseY();
		if (GLWWindowManager::instance()->getFocus(x, y) == getId())
		{
			if (windowState_ & eResizeable &&
				windowState_ & eCircle)
			{
				drawWindowCircle(x_ + w_ - 12.0f, y_, 12.0f, 12.0f);
			}
			if (!(windowState_ & eNoMove) && !(windowState_ & eNoTitle) && !disabled_)
			{
				float sizeX = 20.0f;
				float sizeY = 20.0f;

				static ToolTip moveTip(ToolTip::ToolTipHelp, 
					LANG_RESOURCE("WINDOW_MOVE", "Move Window"),
					LANG_RESOURCE("WINDOW_MOVE_TOOLTIP", "Left click and drag to move the window."));
				GLWToolTip::instance()->addToolTip(&moveTip, 
					x_, y_ + h_ - sizeY, sizeX, sizeY);

				GLState currentStateBlend(GLState::BLEND_ON | GLState::TEXTURE_ON);
				moveTexture_.draw();
				glColor4f(0.8f, 0.0f, 0.0f, 0.8f);
				glPushMatrix();
					glTranslatef(x_, y_ + h_ - sizeY, 0.0f);
					glScalef(sizeX / 16.0f, sizeY / 16.0f, 1.0f);
					drawIconBox(0.0f, 0.0f);
				glPopMatrix();
			}
			if ((windowState_ & eResizeable) && !disabled_)
			{
				float sizeX = 12.0f;
				float sizeY = 12.0f;

				static ToolTip moveTip(ToolTip::ToolTipHelp, 
					LANG_RESOURCE("WINDOW_RESIZE", "Resize Window"),
					LANG_RESOURCE("WINDOW_RESIZE_TOOLTIP", "Left click and drag to resize the window."));
				GLWToolTip::instance()->addToolTip(&moveTip, 
					x_ + w_ - sizeX, y_, sizeX, sizeY);

				GLState currentStateBlend(GLState::BLEND_ON | GLState::TEXTURE_ON);
				resizeTexture_.draw();
				glColor4f(0.8f, 0.0f, 0.0f, 0.8f);
				glPushMatrix();
					glTranslatef(x_ + w_ - sizeX, y_, 0.0f);
					glScalef(sizeX / 16.0f, sizeY / 16.0f, 1.0f);
					drawIconBox(0.0f, 0.0f);
				glPopMatrix();
			}
		}
	}
	else if (windowState_ & eTransparent || windowState_ & eSemiTransparent)
	{
		{
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 0.0f);
				{
					GLState currentStateBlend(GLState::BLEND_ON);
					glColor4f(0.4f, 0.6f, 0.8f, 
						((windowState_ & eTransparent)?0.8f:0.8f));
					drawBackSurface(x_, y_, w_, h_);
				}
				glLineWidth(2.0f);
				glColor4f(0.0f, 0.0f, 0.0f, 
					((windowState_ & eTransparent)?1.0f:1.0f));
				drawSurround(x_, y_, w_, h_);
				glLineWidth(1.0f);
			glPopMatrix();
		}

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();
	}
	else
	{
		GLState currentStateBlend(GLState::BLEND_ON);
		{
			glColor3f(0.8f, 0.8f, 1.0f);
			drawBackSurface(x_, y_, w_, h_);
		}
		if (showTitle_ && !(windowState_ & eSmallTitle)) drawTitleBar(x_, y_, w_, h_);
		glLineWidth(2.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		drawSurround(x_, y_, w_, h_);
		glLineWidth(1.0f);

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();
	}
}

void GLWWindow::draw()
{
	unsigned int state = GLState::DEPTH_OFF | GLState::TEXTURE_OFF;
	if (OptionsDisplay::instance()->getSmoothLines())
	{
		state |= GLState::BLEND_ON;
		glEnable(GL_LINE_SMOOTH);
	}
	GLState currentState(state);

	if (!initPosition_)
	{
		if (x_ < 0.0f) setX(GLViewPort::getWidth() + x_);
		if (y_ < 0.0f) setY(GLViewPort::getHeight() + y_);
		if (w_ < 0.0f) setW(GLViewPort::getWidth() + w_);
		if (h_ < 0.0f) setH(GLViewPort::getHeight() + h_);

		initPosition_ = true;
	}
	if (needCentered_)
	{
		int wWidth = GLViewPort::getWidth();
		int wHeight = GLViewPort::getHeight();
		setX((wWidth - getW()) / 2.0f);
		setY((wHeight - getH()) / 2.0f);
		needCentered_ = false;
	}

	drawMaximizedWindow();

	if (OptionsDisplay::instance()->getSmoothLines())
	{
		glDisable(GL_LINE_SMOOTH);
	}
}

void GLWWindow::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (disabled_) return;

	if (x > x_ && x < x_ + w_)
	{
		if (windowState_ & eCircle ||
			windowState_ & eNoDraw)
		{
			float sizeX = w_ / 5.6f;
			float sizeY = w_ / 5.6f;
			if (x < x_ + sizeX &&
				y < y_ + h_ &&
				y > y_ + h_ - sizeY &&
				!(eNoMove & windowState_))
			{
				// Start window drag
				dragging_ = TitleDrag;
				skipRest = !(eClickTransparent & windowState_) || skipRest;
			}
			else if (y > y_ && y < y_ + h_)
			{
				if ((windowState_ & eResizeable) && 
					inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
				{
					// Start resize window drag
					dragging_ = SizeDrag;
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
				else if (y > y_ && y < y_ + h_)
				{
					// There is a mouse down in the actual window
					GLWPanel::mouseDown(button, x, y, skipRest);
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
			}
		}
		else
		{
			float th = titleHeight;
			if (windowState_ & eSmallTitle) th = smallTitleHeight;
			if (y > y_ + h_ && y < y_ + h_ + th && showTitle_ && x < x_ + titleWidth &&
				!(eNoMove & windowState_))
			{
				// Start window drag
				dragging_ = TitleDrag;
				skipRest = !(eClickTransparent & windowState_) || skipRest;
			}
			else
			{
				if ((windowState_ & eResizeable) && 
					inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
				{
					// Start resize window drag
					dragging_ = SizeDrag;
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
				else if (y > y_ && y < y_ + h_)
				{
					// There is a mouse down in the actual window
					GLWPanel::mouseDown(button, x, y, skipRest);
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
			}	
		}
	}
}

void GLWWindow::mouseUp(int button, float x, float y, bool &skipRest)
{
	if (disabled_) return;

	dragging_ = NoDrag;
	if (x > x_ && x < x_ + w_)
	{
		if (windowState_ & eCircle ||
			windowState_ & eNoDraw)
		{
			float sizeX = w_ / 5.6f;
			float sizeY = w_ / 5.6f;
			if (x < x_ + sizeX &&
				y < y_ + h_ &&
				y > y_ + h_ - sizeY &&
				!(eNoMove & windowState_))
			{
				skipRest = !(eClickTransparent & windowState_) || skipRest;
			}
			else if (y > y_ && y < y_ + h_)
			{
				// There is a mouse up in the actual window
				GLWPanel::mouseUp(button, x, y, skipRest);
				skipRest = !(eClickTransparent & windowState_) || skipRest;
			}
		}
		else
		{
			float th = titleHeight;
			if (windowState_ & eSmallTitle) th = smallTitleHeight;
			if (y > y_ + h_ && y < y_ + h_ + th && showTitle_ && x < x_ + titleWidth &&
				!(eNoMove & windowState_))
			{
				skipRest = !(eClickTransparent & windowState_) || skipRest;
			}
			else
			{
				if ((windowState_ & eResizeable) && 
					inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
				{
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
				else if (y > y_ && y < y_ + h_)
				{
					GLWPanel::mouseUp(button, x, y, skipRest);
					skipRest = !(eClickTransparent & windowState_) || skipRest;
				}
			}	
		}
	}
}

void GLWWindow::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (disabled_) return;

	switch(dragging_)
	{
	case TitleDrag:
		if (x_ + x > 0.0f && y_ - y + h_ - 20.0f > 0 && 
			x_ + x + 50.0f < GLViewPort::getWidth() &&
			y_ - y + h_ + 20.0f < GLViewPort::getHeight())
		{
			x_ += x;
			y_ -= y;
		}
		skipRest = true;
		break;
	case SizeDrag:
		if (w_ + x > minWindowSize &&
			(maxWindowSize_ == 0.0f || w_ + x < maxWindowSize_))
		{
			w_ += x;
		}
		if (h_ + y > minWindowSize &&
			(maxWindowSize_ == 0.0f || h_ + y < maxWindowSize_))
		{
			h_ += y;
			y_ -= y;
		}

		skipRest = true;
		break;
	default:
		GLWPanel::mouseDrag(button, mx, my, x, -y, skipRest);
		break;
	}
}

void GLWWindow::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (disabled_) return;

	GLWPanel::mouseWheel(x, y, z, skipRest);
}

void GLWWindow::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (disabled_) return;

	GLWPanel::keyDown(buffer, keyState, history, hisCount, skipRest);
}

void GLWWindow::drawInfoBox(float x, float y, float w)
{
	glColor4f(0.5f, 0.5f, 1.0f, 0.3f);	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x + 20.0f, y - 8.0f);
		glVertex2f(x + 20.0f, y - 18.0f);
		drawRoundBox(x, y - 18.0f, w, 18.0f, 9.0f);
		glVertex2f(x + 20.0f, y - 18.0f);
	glEnd();

	glColor4f(0.9f, 0.9f, 1.0f, 0.5f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
		drawRoundBox(x, y - 18.0f, w, 18.0f, 9.0f);
	glEnd();
	glLineWidth(1.0f);
}

void GLWWindow::drawJoin(float x, float y)
{
	glColor4f(0.8f, 0.8f, 0.9f, 0.8f);
	glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glScalef(3.0f, 3.0f, 3.0f);
		glBegin(GL_TRIANGLE_FAN);
			drawWholeCircle(true);
		glEnd();
	glPopMatrix();
}

void GLWWindow::drawIconBox(float x, float y)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + 16.0f, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + 16.0f, y + 16.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + 16.0f);
	glEnd();
}

bool GLWWindow::initFromXML(XMLNode *node)
{
	if (!GLWPanel::initFromXML(node)) return false;

	if (name_.c_str()[0] == 0)
	{
		// Get name again, just to display the error message
		if (!node->getNamedChild("name", name_)) return false;
	}

	bool noTooltip = false;
	node->getNamedChild("notooltip", noTooltip, false);

	// Desc
	if (!node->getNamedChild("description", description_)) return false;
	if (!noTooltip) toolTip_.setText(ToolTip::ToolTipHelp, 
		LANG_RESOURCE(name_, name_), LANG_RESOURCE(name_ + "_window", description_));

	// Disabled
	XMLNode *disabled = 0;
	if (node->getNamedChild("disabled", disabled, false))
	{
		disabled_ = true;
	}

	node->getNamedChild("windowlevel", windowLevel_, false);

	unsigned int windowstate = 0;
	if (node->getNamedChild("windowstate", windowstate, false))
	{
		windowState_ = windowstate;
	}

	return true;
}

void GLWWindow::saveSettings(XMLNode *node)
{
	GLWPanel::saveSettings(node);

	if (getWindowState() & GLWWindow::eSavePosition &&
		getX() >= 0 && getY() >= 0)
	{
		bool visible = GLWWindowManager::instance()->windowVisible(getId());

		node->addChild(new XMLNode("x", int(getX())));
		node->addChild(new XMLNode("y", int(getY())));
		node->addChild(new XMLNode("visible", visible));
	}
}

void GLWWindow::loadSettings(XMLNode *node, bool resetPositions)
{
	GLWPanel::loadSettings(node, resetPositions);

	if (getWindowState() & GLWWindow::eSavePosition &&
		!resetPositions)
	{
		int x, y;
		bool visible;
		if (node->getNamedChild("x", x, false)) setX(float(x));
		if (node->getNamedChild("y", y, false)) setY(float(y));
		if (node->getNamedChild("visible", visible, false))
		{
			if (!visible) GLWWindowManager::instance()->hideWindow(getId());
			else GLWWindowManager::instance()->showWindow(getId());
		}
	}
}
