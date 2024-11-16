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

#if !defined(__INCLUDE_GLViewPorth_INCLUDE__)
#define __INCLUDE_GLViewPorth_INCLUDE__

class GLViewPort
{
public:
	GLViewPort();
	virtual ~GLViewPort();

	void draw();

	/**
	The size of the viewport in pixels and actual
	physcial window size in pixels.
	This viewport may not be the actual size of the window,
	in which case the graphics will seem larger or smaller
	than usual.
	*/
	void setWindowSize(int width, int height,
		int awidth = 0, int aheight = 0);

	static int getWidth() { return width_; }
	static int getHeight() { return height_; }
	static int getActualWidth() { return actualWidth_; }
	static int getActualHeight() { return actualHeight_; }
	static float getWidthMult() { return widthMult_; }
	static float getHeightMult() { return heightMult_; }

protected:
	static int width_, height_;
	static float widthMult_, heightMult_;
	static int actualWidth_, actualHeight_;

};

#endif
