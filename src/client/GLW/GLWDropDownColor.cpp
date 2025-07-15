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

#include <GLW/GLWDropDownColor.h>
#include <image/ImageFactory.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(GLWDropDownColor);

GLWDropDownColor::GLWDropDownColor(float x, float y, float w) :
	GLWDropDown(x, y, w)
{
	colorTexture_.setImageID(
		ImageID(S3D::eDataLocation, 
		"data/images/white.bmp"),
		GLTextureReference::eTextureClamped);
}

GLWDropDownColor::~GLWDropDownColor()
{
}

void GLWDropDownColor::addColor(Vector &newColor)
{
	std::list<GLWSelectorEntry>::iterator coloritor;
	for (coloritor = texts_.begin();
		coloritor != texts_.end();
		++coloritor)
	{
		Vector &color = coloritor->getColor();
		if (newColor[0] > color[0]) break;
		else if (newColor[0] == color[0])
		{
			if (newColor[1] > color[1]) break;
			else if (newColor[1] == color[1])
			{
				if (newColor[2] > color[2]) break;
				else if (newColor[2] == color[2])
				{
					break;
				}
			}
		}
	}

	GLWSelectorEntry entry(LANG_STRING(""), 0, false, &colorTexture_, 0);
	entry.getColor() = newColor;
	entry.getTextureWidth() = 32;

	texts_.insert(coloritor, entry);
	if (!current_)
	{
		current_ = &texts_.back();
	}
}

Vector &GLWDropDownColor::getCurrentColor()
{
	if (!getCurrentEntry()) return Vector::getNullVector();
	return getCurrentEntry()->getColor();
}

void GLWDropDownColor::setCurrentColor(const Vector &color)
{
	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		++itor)
	{
		GLWSelectorEntry &entry = *itor;
		if (((Vector &) color) == entry.getColor())
		{
			current_ = &entry;
			break;
		}

		position++;
	}

	if (handler_ && current_)
	{
		handler_->select(id_, position, *current_);
	}
}

void GLWDropDownColor::draw()
{
	GLWDropDown::draw();

	if (getCurrentEntry())
	{
		GLState state(GLState::TEXTURE_OFF);

		Vector color = getCurrentColor();
		glColor3f(color[0], color[1], color[2]);

		float x = x_ + 12.0f;
		float y = y_ + 6.0f;
		glBegin(GL_QUADS);
			glVertex2f(x, y);
			glVertex2f(x + w_ - 44.0f, y);
			glVertex2f(x + w_ - 44.0f, y + 14.0f);
			glVertex2f(x, y + 14.0f);
		glEnd();
	}
}
