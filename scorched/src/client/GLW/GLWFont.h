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

#if !defined(AFX_GLWFONT_H__0BCF1F78_3D58_47EC_8B98_EB39AB3CADD4__INCLUDED_)
#define AFX_GLWFONT_H__0BCF1F78_3D58_47EC_8B98_EB39AB3CADD4__INCLUDED_

#include <GLEXT/GLFont2d.h>

class GLWFont
{
public:
	static GLWFont *instance();

	static Vector widgetFontColor;
	static Vector disabledWidgetFontColor;

	GLFont2d *getGameFont() { return gameFont_; }
	GLFont2d *getGameShadowFont() { return gameShadowFont_; }
	GLFont2d *getCourierFont() { return courierFont_; }
	GLFont2d *getNormalFont() { return normalFont_; }
	GLFont2d *getNormalShadowFont() { return normalShadowFont_; }

	void displayCharacterInfo();

protected:
	static  GLWFont *instance_;
	GLFont2d *gameFont_;
	GLFont2d *gameShadowFont_;
	GLFont2d *courierFont_;
	GLFont2d *normalFont_;
	GLFont2d *normalShadowFont_;

private:
	GLWFont();
	virtual ~GLWFont();

};

#endif // !defined(AFX_GLWFONT_H__0BCF1F78_3D58_47EC_8B98_EB39AB3CADD4__INCLUDED_)
