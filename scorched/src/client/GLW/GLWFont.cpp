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

#include <GLW/GLWFont.h>
#include <common/Defines.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <common/Logger.h>

Vector GLWFont::widgetFontColor = Vector(0.2f, 0.2f, 0.2f);
Vector GLWFont::disabledWidgetFontColor = Vector(0.4f, 0.4f, 0.4f);

GLWFont *GLWFont::instance_ = 0;

GLWFont *GLWFont::instance()
{
	if (!instance_)
	{
		instance_ = new GLWFont;
	}

	return instance_;
}

GLWFont::GLWFont()
{
	gameFont_ = new GLFont2d;
	gameFont_->createFont(
		S3D::getDataFile("data/fonts/dejavusconbd.ttf"),
		16);
	gameShadowFont_ = new GLFont2d;
	gameShadowFont_->createFont(
		S3D::getDataFile("data/fonts/dejavusconbd.ttf"),
		16,
		true);
	courierFont_ = new GLFont2d;
	courierFont_->createFont(
		S3D::getDataFile("data/fonts/dejavusmobd.ttf"),
		16);
	normalFont_ = new GLFont2d;
	normalFont_->createFont(
		S3D::getDataFile("data/fonts/dejavusans.ttf"),
		16);
	normalShadowFont_ = new GLFont2d;
	normalShadowFont_->createFont(
		S3D::getDataFile("data/fonts/dejavusans.ttf"),
		16,
		true);

	new ConsoleRuleMethodIAdapter<GLWFont>(
		this, &GLWFont::displayCharacterInfo, "CharacterInfo");
}

GLWFont::~GLWFont()
{

}

void GLWFont::displayCharacterInfo()
{
	Logger::log(S3D::formatStringBuffer("Characters : %u\nCharacter Blocks : %u\n",
		GLFont2d::getTotalCharacters(),
		GLFont2dStorage::getTotalCharacterBlocks()));
}
