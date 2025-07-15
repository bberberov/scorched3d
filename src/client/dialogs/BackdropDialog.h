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

#if !defined(__INCLUDE_BackdropDialogh_INCLUDE__)
#define __INCLUDE_BackdropDialogh_INCLUDE__

#include <GLEXT/GLTexture.h>
#include <GLW/GLWWindow.h>

class BackdropDialog : public GLWWindow
{
public:
	static BackdropDialog *instance();

	virtual void draw();
	void drawBackground();
	void drawLogo();
	void drawFooter();

	void capture();

protected:
	static BackdropDialog *instance_;
	GLTexture backTex_;
	GLTextureReference logoTex_, footerTex_;
	std::string lastMod_;

private:
	BackdropDialog();
	virtual ~BackdropDialog();
};


#endif
