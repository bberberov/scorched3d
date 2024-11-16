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

#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/ModSelectDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <engine/ModDirs.h>
#include <common/Defines.h>

GLWIconListSubModItem::GLWIconListSubModItem(ModInfo::MenuEntry &modInfoEntry) :
	modInfoEntry_(modInfoEntry), 
	tip_(ToolTip::ToolTipHelp, 
		LANG_STRING(modInfoEntry.shortdescription), 
		LANG_STRING(modInfoEntry.description)),
	icon_(0.0f, 0.0f, 40.0f, 40.0f)
{
	if (S3D::fileExists(modInfoEntry_.icon.c_str()))
	{
		icon_.setTextureImage(ImageID(S3D::eAbsLocation, modInfoEntry_.icon));
	}
}

GLWIconListSubModItem::~GLWIconListSubModItem()
{
}

void GLWIconListSubModItem::draw(float x, float y, float w)
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
		10.0f, x + 50.0f, y + 18.0f, 0.0f, 
		modInfoEntry_.shortdescription.c_str());
}

ModSubSelectDialog *ModSubSelectDialog::instance_ = 0;

ModSubSelectDialog *ModSubSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ModSubSelectDialog;
	}
	return instance_;
}

ModSubSelectDialog::ModSubSelectDialog() : 
	GLWWindow("Mod Sub Select", 300.0f, 410.0f, eHideName, ""),
	modInfo_("None")
{
	iconList_ = new GLWIconList(10.0f, 40.0f, 280.0f, 360.0f, 50.0f);
	addWidget(iconList_);

	okId_ = addWidget(new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();
	cancelId_ = addWidget(new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 120, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();

	iconList_->setHandler(this);
}

ModSubSelectDialog::~ModSubSelectDialog()
{

}

void ModSubSelectDialog::setModInfo(ModInfo &modInfo)
{
	modInfo_ = modInfo;
}

void ModSubSelectDialog::display()
{
	iconList_->clear();

	std::list<ModInfo::MenuEntry>::iterator itor;
	for (itor = modInfo_.getMenuEntries().begin();
		itor != modInfo_.getMenuEntries().end();
		++itor)
	{
		ModInfo::MenuEntry &entry = (*itor);
		GLWIconListSubModItem *item = new GLWIconListSubModItem(entry);
		iconList_->addItem(item);
	}
}

void ModSubSelectDialog::selected(unsigned int id, int position)
{
}

void ModSubSelectDialog::chosen(unsigned int id, int position)
{
	buttonDown(okId_);
}

void ModSubSelectDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		GLWWindowManager::instance()->hideWindow(
			ModSelectDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(id_);

		GLWIconListSubModItem *selected = 
			(GLWIconListSubModItem *) iconList_->getSelected();
		if (selected)
		{
			const char *targetFilePath = selected->getModInfoEntry().gamefile.c_str();
			ClientParams::instance()->reset();
			ClientParams::instance()->setClientFile(targetFilePath);
			ClientMain::startClient();
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
}
