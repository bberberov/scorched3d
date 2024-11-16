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

#include <GLW/GLWBitmap.h>
#include <GLEXT/GLState.h>
#include <common/Defines.h>

REGISTER_CLASS_SOURCE(GLWBitmap);

GLWBitmap::GLWBitmap(float x, float y, float w, float h, Image *bitmap) : 
	GLWidget(x, y, w, h),
	bitmap_(bitmap)
{
}

GLWBitmap::~GLWBitmap()
{
}

void GLWBitmap::draw()
{
	if (bitmap_)
	{
		GLState state(GLState::TEXTURE_OFF | GLState::BLEND_OFF);

		int width = MIN(bitmap_->getWidth(), (int) w_);
		int height = MIN(bitmap_->getHeight(), (int) h_);

		glPixelStorei(GL_PACK_ALIGNMENT, 4);    /* Force 4-byte alignment */
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

		glRasterPos2f(x_, y_);
		glDrawPixels(
			width, 
			height,
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			bitmap_->getBits());

		glPixelStorei(GL_PACK_ALIGNMENT, 4);    /* Force 4-byte alignment */
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_SKIP_ROWS, 0);
		glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	}

	GLWidget::draw();
}

bool GLWBitmap::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;
	return true;
}

