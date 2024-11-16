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

#include <dialogs/SaveSelectDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <GLEXT/GLTextureStore.h>
#include <common/Defines.h>
#include <common/FileList.h>

GLWIconListSaveItem::GLWIconListSaveItem(
	const char *file, const char *time) :
	tip_(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SAVE", "Save"), 
		LANG_RESOURCE("SAVE_TOOLTIP", "Load a previously saved game.")),
	icon_(0.0f, 0.0f, 40.0f, 40.0f),
	file_(file), time_(time)
{
	icon_.setTextureImage(ImageID(S3D::eDataLocation, "data/images/save.bmp"));
}

GLWIconListSaveItem::~GLWIconListSaveItem()
{
}

void GLWIconListSaveItem::draw(float x, float y, float w)
{
	icon_.setX(x + 2.0f);
	icon_.setY(y + 2.0f);
	icon_.draw();

	GLWToolTip::instance()->addToolTip(&tip_, 
		GLWTranslate::getPosX() + x, 
		GLWTranslate::getPosY() + y, w, 50.0f);

	GLWFont::instance()->getGameFont()->drawWidth(
		w - 50.0f,
		GLWFont::widgetFontColor, 
		8.0f, x + 50.0f, y + 23.0f, 0.0f, 
		file_.c_str());
	GLWFont::instance()->getGameFont()->drawWidth(
		w - 50.0f,
		GLWFont::widgetFontColor, 
		8.0f, x + 50.0f, y + 12.0f, 0.0f, 
		time_.c_str());
}

SaveSelectDialog *SaveSelectDialog::instance_ = 0;

SaveSelectDialog *SaveSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SaveSelectDialog;
	}
	return instance_;
}

SaveSelectDialog::SaveSelectDialog() : 
	GLWWindow("Save", 300.0f, 410.0f, eHideName, "")
{
	iconList_ = new GLWIconList(10.0f, 40.0f, 280.0f, 360.0f, 50.0f);
	addWidget(iconList_);

	ok_ = (GLWButton *) addWidget(new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX));
	cancelId_ = addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 120, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();
	iconList_->setHandler(this);
}

SaveSelectDialog::~SaveSelectDialog()
{

}

void SaveSelectDialog::display()
{
	iconList_->clear();
	ok_->setEnabled(false);

	FileList fileList(
		S3D::getSaveFile(""),
		"*.s3d",
		false, // Full path
		false); // Recurse
	if (fileList.getStatus())
	{
		FileList::ListType &files = fileList.getFiles();
		FileList::ListType::iterator itor;
		for (itor = files.begin();
			itor != files.end();
			++itor)
		{
			const char *shortPath = (*itor).c_str();
			std::string fullPath = S3D::getSaveFile(shortPath);

			time_t modTime = S3D::fileModTime(fullPath);
			const char *modTimeStr = ctime(&modTime);
			char *nl = (char *) strchr(modTimeStr, '\n');
			if (nl) *nl = '\0';

			GLWIconListSaveItem *item = new GLWIconListSaveItem(shortPath, modTimeStr);
			iconList_->addItem(item);
		}
	}
}

void SaveSelectDialog::selected(unsigned int id, int position)
{
	ok_->setEnabled(true);
}

void SaveSelectDialog::chosen(unsigned int id, int position)
{
	buttonDown(ok_->getId());
}

void SaveSelectDialog::buttonDown(unsigned int id)
{
	if (id == ok_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);

		GLWIconListSaveItem *selected = 
			(GLWIconListSaveItem *) iconList_->getSelected();
		if (selected)
		{
			std::string targetFilePath = S3D::getSaveFile(selected->getFile());
			ClientParams::instance()->reset();
			ClientParams::instance()->setSaveFile(targetFilePath.c_str());
			ClientMain::startClient();
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
