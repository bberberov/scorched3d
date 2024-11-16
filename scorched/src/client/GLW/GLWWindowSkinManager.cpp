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

#include <GLW/GLWWindowSkinManager.h>
#include <GLW/GLWWindowManager.h>
#include <XML/XMLFile.h>
#include <common/Defines.h>

GLWWindowSkinManager *GLWWindowSkinManager::defaultinstance_ = 0;
GLWWindowSkinManager *GLWWindowSkinManager::modinstance_ = 0;

GLWWindowSkinManager *GLWWindowSkinManager::defaultinstance()
{
	if (!defaultinstance_) defaultinstance_ = new GLWWindowSkinManager();
	return defaultinstance_;
}

GLWWindowSkinManager *GLWWindowSkinManager::modinstance()
{
	if (!modinstance_) modinstance_ = new GLWWindowSkinManager();
	return modinstance_;
}

GLWWindowSkinManager::GLWWindowSkinManager()
{
}

GLWWindowSkinManager::~GLWWindowSkinManager()
{
}

bool GLWWindowSkinManager::loadWindows()
{
	while (!windows_.empty())
	{
		GLWWindowSkin *window = windows_.back();
		GLWWindowManager::instance()->removeWindow(window);
		delete window;
		windows_.pop_back();
	}

	XMLFile file;
	std::string fileName = S3D::getModFile("data/windows.xml");
	if (!file.readFile(fileName) ||
		!file.getRootNode())
	{
		S3D::dialogMessage("GLWWindowSkinManager", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  fileName.c_str(),
					  file.getParserError()));
		return false;
	}

	// Itterate all of the windows in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        ++childrenItor)
    {
		// For each node named window
        XMLNode *currentNode = (*childrenItor);

		GLWWindowSkin *window = new GLWWindowSkin;
		if (!window->initFromXML(currentNode))
		{
			return false;
		}

		if (!currentNode->failChildren()) return false;
		windows_.push_back(window);
	}

	return true;
}

std::list<GLWWindowSkin *> GLWWindowSkinManager::getStateWindows(const char *state)
{
	std::list<GLWWindowSkin *> windows;
	std::list<GLWWindowSkin *>::iterator itor;
	for (itor = windows_.begin();
		itor != windows_.end();
		++itor)
	{
		GLWWindowSkin *window = (GLWWindowSkin *) *itor;
		if (window->inState(state))
		{
			windows.push_back(window);
		}
	}
	return windows;
}
