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

#include <dialogs/TutorialDialog.h>
#include <dialogs/MainMenuDialog.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLMenuEntry.h>
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWListView.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <target/TargetRenderer.h>
#include <tankgraph/TargetRendererImpl.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <time.h>

TutorialDialog *TutorialDialog::instance_ = 0;

TutorialDialog *TutorialDialog::instance()
{
	if (!instance_)
	{
		instance_ = new TutorialDialog;
	}
	return instance_;
}

TutorialDialog::TutorialDialog() : 
	GLWWindow("Tutorial", 155.0f, -125.0f, 
		470.0f, 120.0f, eSemiTransparent | eNoTitle | eHideName,
		"The ingame tutorial."),
	triangleDir_(30.0f), triangleDist_(0.0f),
	current_(0), speed_(1.0f)
{
	windowLevel_ = 75000;

	Vector listColor(0.0f, 0.0f, 0.0f);
	float wid = float(GLViewPort::getWidth() - 330);
	listView_ = new GLWListView(0.0f, 0.0f, wid, 105.0f, -1, 12.0f, 32.0f);
	listView_->setColor(listColor);
	listView_->setHandler(this);
	addWidget(listView_, 0, SpaceAll, 10.0f);
	setLayout(GLWPanel::LayoutVerticle);
	layout();

	if (file_.parseFile(S3D::getDataFile(
		ScorchedClient::instance()->getOptionsGame().getTutorial())))
	{
		showPage(file_.getStartEntry());
	}
}

TutorialDialog::~TutorialDialog()
{

}

void TutorialDialog::showPage(TutorialFileEntry *entry)
{
	currentEvents_.clear();
	current_ = entry;

	listView_->clear();
	listView_->addXML(entry->text_);
}

void TutorialDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseDown(button, x, y, skipRest);
	speed_ = 3.0f;
}

void TutorialDialog::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseUp(button, x, y, skipRest);
	speed_ = 1.0f;
}

void TutorialDialog::display()
{
	GLWWindow::display();
	listView_->resetPosition();
}

void TutorialDialog::draw()
{
	GLWWindow::draw();

	processEvents();
}

static const char *getValue(const char *name,
	std::map<std::string, std::string> &event)
{
	std::map<std::string, std::string>::iterator itor =
		event.find(name);
	if (itor == event.end()) return 0;
	return (*itor).second.c_str();
}

void TutorialDialog::processEvents(bool log)
{
	TargetRendererImpl::setHighlightType(TargetRendererImpl::eNoHighlight);

	if (currentEvents_.empty()) return;

	const char *action = getValue("action", currentEvents_);
	if (!action)
	{
		S3D::dialogExit("TutorialDialog", "No action in event");
	}

	if (0 == strcmp(action, "highlight")) 
	{
		processHighlight(log);
	}
	else if (0 == strcmp(action, "targets"))
	{
		TargetRendererImpl::setHighlightType(TargetRendererImpl::eOtherHighlight);
	}
	else if (0 == strcmp(action, "player"))
	{
		TargetRendererImpl::setHighlightType(TargetRendererImpl::ePlayerHighlight);
	}
	else if (0 == strcmp(action, "menu"))
	{
		processMenu(log);
	}
	else
	{
		S3D::dialogExit("TutorialDialog",
			S3D::formatStringBuffer("Unknown tutorial event type \"%s\"",
			action));
	}
}

void TutorialDialog::processMenu(bool log)
{
	const char *menuName = getValue("menu", currentEvents_);
	if (!menuName)
	{
		S3D::dialogExit("TutorialDialog", "No menu in event");
	}

	GLMenuEntry *entry = MainMenuDialog::instance()->
		getMenu((char*) menuName);
	if (!entry) return;

	drawHighlight(
		entry->getX(), entry->getY() - entry->getH(), 
		entry->getW(), entry->getH());
}

void TutorialDialog::processHighlight(bool log)
{
	const char *windowName = getValue("window", currentEvents_);
	const char *controlName = getValue("control", currentEvents_);
	if (!windowName || !controlName)
	{
		S3D::dialogExit("TutorialDialog", "No window or control in event");
	}

	GLWWindow *window = GLWWindowManager::instance()->getWindowByName(windowName);
	if (!window)
	{
		if (log)
		{
			Logger::log(S3D::formatStringBuffer("Tutorial cannot find window \"%s\"", windowName));
		}
		return;
	}

	GLWidget *control = window->getWidgetByName(controlName);
	if (!control)
	{
		if (log)
		{
			Logger::log(S3D::formatStringBuffer("Tutorial cannot find control \"%s\" in window \"%s\"", 
				controlName, windowName));
		}
		return;
	}

	float x = control->getX();
	float y = control->getY();
	GLWPanel *parent = control->getParent();
	while (parent)
	{
		x += parent->getX();
		y += parent->getY();
		parent = parent->getParent();
	}

	drawHighlight(
		x, y, control->getW(), control->getH());
}

void TutorialDialog::simulate(float frameTime)
{
	GLWWindow::simulate(frameTime * speed_);

	triangleDist_ += frameTime * triangleDir_;
	if (triangleDist_ < 0.0f)
	{
		triangleDist_ = -triangleDist_;
		triangleDir_ = -triangleDir_;
	}
	if (triangleDist_ > 10.0f) 
	{
		triangleDist_ = 10.0f;
		triangleDir_ = -triangleDir_;
	}

	if (current_)
	{
		TutorialFileEntry *entry = current_->checkConditions();
		if (entry) showPage(entry);
	}
}

void TutorialDialog::url(const char *url)
{
	if (0 == strcmp(url, "exit"))
	{
		GLWWindowManager::instance()->hideWindow(getId());
		return;
	}

	TutorialFileEntry *entry = file_.getEntry(url);
	if (entry)
	{
		showPage(entry);
	}
	else
	{
		S3D::dialogExit("TutorialDialog", 
			S3D::formatStringBuffer("Unknown url \"%s\"", url));
	}
}

void TutorialDialog::drawHighlight(float x, float y, float w, float h)
{
	if (!triangleTex_.textureValid())
	{
		Image maps = ImageFactory::loadImage(
			S3D::eDataLocation,
			"data/images/triangle.bmp",
			"data/images/triangle.bmp",
			false);
		triangleTex_.create(maps, true);
	}

	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON | GLState::DEPTH_OFF);
	glColor4f(1.0f, 0.0f, 0.0f, 0.7f);

	triangleTex_.draw();

	const float size = 20.0f;
	drawTriangle(x - size - triangleDist_, y + (h - size) / 2.0f, size, 1);
	drawTriangle(x + w + triangleDist_, y + (h - size) / 2.0f, size, 3);
	drawTriangle(x + (w - size) / 2.0f, y + h + triangleDist_, size, 2);
	drawTriangle(x + (w - size) / 2.0f, y - size - triangleDist_, size, 0);
}

void TutorialDialog::drawTriangle(float x, float y, float size, int tex)
{
	glBegin(GL_QUADS);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(0.0f, 0.0f); break;
			case 1: glTexCoord2f(1.0f, 0.0f); break;
			case 2: glTexCoord2f(1.0f, 1.0f); break;
			case 3: glTexCoord2f(0.0f, 1.0f); break;
		}
		glVertex2f(x, y);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(1.0f, 0.0f); break;
			case 1: glTexCoord2f(1.0f, 1.0f); break;
			case 2: glTexCoord2f(0.0f, 1.0f); break;
			case 3: glTexCoord2f(0.0f, 0.0f); break;
		}
		glVertex2f(x + size, y);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(1.0f, 1.0f); break;
			case 1: glTexCoord2f(0.0f, 1.0f); break;
			case 2: glTexCoord2f(0.0f, 0.0f); break;
			case 3: glTexCoord2f(1.0f, 0.0f); break;
		}
		glVertex2f(x + size, y + size);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(0.0f, 1.0f); break;
			case 1: glTexCoord2f(0.0f, 0.0f); break;
			case 2: glTexCoord2f(1.0f, 0.0f); break;
			case 3: glTexCoord2f(1.0f, 1.0f); break;
		}
		glVertex2f(x, y + size);
	glEnd();
}

void TutorialDialog::event(std::map<std::string, std::string> &event)
{
	currentEvents_ = event;
}

