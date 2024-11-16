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

#if !defined(__INCLUDE_TutorialDialogh_INCLUDE__)
#define __INCLUDE_TutorialDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWListView.h>
#include <GLEXT/GLTexture.h>
#include <graph/TutorialFile.h>

class TutorialDialog : 
	public GLWWindow,
	public GLWListViewI
{
public:
	static TutorialDialog *instance();

	virtual void display();
	virtual void simulate(float frameTime);
	virtual void draw();

	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);

	virtual void url(const char *url);
	virtual void event(std::map<std::string, std::string> &event);

	void drawHighlight(float x, float y, float w, float h);
protected:
	static TutorialDialog *instance_;
	float triangleDist_, triangleDir_;
	float speed_;
	GLWListView *listView_;
	GLTexture triangleTex_;
	TutorialFile file_;
	TutorialFileEntry *current_;

	std::map<std::string, std::string> currentEvents_;

	void processEvents(bool log = false);
	void processHighlight(bool log);
	void processMenu(bool log);
	void showPage(TutorialFileEntry *entry);
	void drawTriangle(float x, float y, float size, int tex);

private:
	TutorialDialog();
	virtual ~TutorialDialog();

};

#endif
