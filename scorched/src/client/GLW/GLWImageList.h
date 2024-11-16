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

#if !defined(__INCLUDE_GLWImageListh_INCLUDE__)
#define __INCLUDE_GLWImageListh_INCLUDE__

#include <GLW/GLWButton.h>
#include <GLW/GLWSelector.h>
#include <GLEXT/GLTexture.h>
#include <list>

class GLWImageList : 
	public GLWidget,
	public GLWSelectorI
{
public:
	GLWImageList(float x = 0.0f, float y = 0.0f);
	virtual ~GLWImageList();

	void addDirectory(const std::string &directory);

	const char *getCurrentShortPath();
	const char *getCurrentLongPath();
	bool setCurrentShortPath(const char *current);

	// GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);

	// GLWSelectorI
	virtual void itemSelected(GLWSelectorEntry *entry, int position);

	void setEnabled(bool enabled) { enabled_ = enabled; }

	REGISTER_CLASS_HEADER(GLWImageList);

protected:
	class GLWImageListEntry 
	{
	public:
		std::string shortFileName;
		std::string longFileName;
		GLTexture texture;
	};
	std::list<GLWImageListEntry*> entries_;
	GLWImageListEntry *current_;
	bool enabled_;
		
};

#endif // __INCLUDE_GLWImageListh_INCLUDE__
