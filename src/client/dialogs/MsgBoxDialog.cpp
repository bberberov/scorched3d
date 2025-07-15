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

#include <dialogs/MsgBoxDialog.h>
#include <GLW/GLWWindowManager.h>

MsgBoxDialog *MsgBoxDialog::instance_ = 0;

MsgBoxDialog *MsgBoxDialog::instance()
{
	if (!instance_)
	{
		instance_ = new MsgBoxDialog;
	}
	return instance_;
}

MsgBoxDialog::MsgBoxDialog() : 
	GLWWindow("MsgBox", 210.0f, 150.0f, eHideName, "")
{
	GLWPanel *topPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);

	icon_ = new GLWIcon(0.0f, 0.0f, 32.0f, 32.0f);
	topPanel->addWidget(icon_, 0, SpaceLeft | SpaceTop | AlignTop, 10.0f);

	message_ = new GLWLabel(0.0f, 0.0f, LANG_STRING(""), 8.0f, GLWLabel::eMultiLine);
	topPanel->addWidget(message_, 0, SpaceTop | SpaceLeft | SpaceRight | AlignCenterLeftRight, 10.0f);

	topPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(topPanel);

	okButton_ = (GLWTextButton *) 
		addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagOk | 
		GLWButton::ButtonFlagCenterX), 0, SpaceAll | AlignRight, 10.0f);
	okButton_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CANCEL", "Cancel"),
		LANG_RESOURCE("CANCEL_TOOLTIP", "Return to the game.")));

	windowLevel_ = 1000;

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

MsgBoxDialog::~MsgBoxDialog()
{

}

void MsgBoxDialog::show(const LangString &message, ShowType type)
{
	if (type == eError)
	{
		instance()->icon_->setTextureImage(
			ImageID(S3D::eDataLocation,
			"data/images/exclaim.bmp",
			"data/images/mask.bmp"));
	}
	else
	{
		instance()->icon_->setTextureImage(
			ImageID(S3D::eDataLocation,
			"data/images/ok.bmp",
			"data/images/mask.bmp"));
	}

	instance()->message_->setText(message);
	instance()->message_->calcWidth();
	instance()->layout();

	GLWWindowManager::instance()->showWindow(instance()->getId());
}

void MsgBoxDialog::buttonDown(unsigned int id)
{
	if (id == okButton_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}

void MsgBoxDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseDown(button, x, y, skipRest);
	skipRest = true;
}

void MsgBoxDialog::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseUp(button, x,y, skipRest);
	skipRest = true;
}

void MsgBoxDialog::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	GLWWindow::keyDown(buffer, keyState, history, hisCount, skipRest);
	skipRest = true;
}

void MsgBoxDialog::mouseWheel(float x, float y, float z, bool &skipRest)
{
	GLWWindow::mouseWheel(x, y, z, skipRest);
	skipRest = true;
}

