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

#include <GLW/GLWImageList.h>
#include <GLW/GLWTranslate.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLState.h>
#include <common/FileList.h>
#include <string.h>

REGISTER_CLASS_SOURCE(GLWImageList);

GLWImageList::GLWImageList(float x, float y) :
	GLWidget(x, y, 32.0f, 32.0f), current_(0), enabled_(true)
{
}

GLWImageList::~GLWImageList()
{
	while (!entries_.empty())
	{
		GLWImageListEntry* entry = entries_.front();
		entries_.pop_front();
		delete entry;
	}
}

void GLWImageList::addDirectory(const std::string &directory)
{
	if (directory.empty()) return;

	FileList filelist(directory, "*.png");
	std::list<std::string>::iterator itor;
	for (itor = filelist.getFiles().begin();
		itor != filelist.getFiles().end();
		++itor)
	{
		const char *filename = (*itor).c_str();
		Image png = ImageFactory::loadImage(S3D::eAbsLocation, filename);
		if (png.getBits() &&
			png.getWidth() == 32 &&
			png.getHeight() == 32)
		{
			GLWImageListEntry *entry = new GLWImageListEntry;
			if (entry->texture.create(png))
			{
				entry->longFileName = filename;
				const char *sep = strrchr(filename, '/');
				if (sep) sep++;
				else sep = filename;
				entry->shortFileName = sep;
				entries_.push_back(entry);
			}
			else delete entry;
		}
	}

	setCurrentShortPath("");
}

void GLWImageList::draw()
{
	GLWidget::draw();

	if (!current_) return;
	current_->texture.draw();

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLState currentStateOff(GLState::TEXTURE_OFF);
	glColor3f(0.4f, 0.4f, 0.6f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(x_ - 4.0f, y_ - 3.0f);
		glVertex2f(x_ + w_ + 3.0f, y_ - 3.0f);
		glVertex2f(x_ + w_ + 3.0f, y_ + h_ + 3.0f);
		glVertex2f(x_ - 4.0f, y_ + h_ + 3.0f);
	glEnd();

	GLState currentStateOn(GLState::TEXTURE_ON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x_, y_);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x_ + w_, y_);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x_ + w_, y_ + h_);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x_, y_ + h_);
	glEnd();
}

bool GLWImageList::setCurrentShortPath(const char *current)
{
	std::list<GLWImageListEntry*>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		++itor)
	{
		GLWImageListEntry *entry = (*itor);
		if (0 == strcmp(entry->shortFileName.c_str(), current))
		{
			current_ = entry;
			return true;
		}
	}

	if (!current_ && !entries_.empty())
	{
		current_ = entries_.front();
	}
	return false;
}

const char *GLWImageList::getCurrentLongPath()
{
	if (current_) return current_->longFileName.c_str();
	return "";
}

const char *GLWImageList::getCurrentShortPath()
{
	if (current_) return current_->shortFileName.c_str();
	return "";
}

void GLWImageList::simulate(float frameTime)
{
	GLWidget::simulate(frameTime);
}

void GLWImageList::mouseDown(int button, float x, float y, 
							 bool &skipRest)
{
	GLWidget::mouseDown(button, x, y, skipRest);

	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (enabled_)
		{
			std::list<GLWSelectorEntry> entries;
			std::list<GLWImageListEntry*>::iterator itor;
			for (itor = entries_.begin();
				itor != entries_.end();
				++itor)
			{
				GLWImageListEntry *entry = (*itor);
				entries.push_back(
					GLWSelectorEntry(LANG_STRING(entry->shortFileName.c_str()),
					0, 0, &entry->texture, 0, entry->shortFileName));
			}

			GLWSelector::instance()->showSelector(
				this, 
				GLWTranslate::getPosX() + x, 
				GLWTranslate::getPosY() + y, 
				entries, 0, false);
		}
	}
}

void GLWImageList::mouseUp(int button, float x, float y, 
						   bool &skipRest)
{
	GLWidget::mouseUp(button, x, y, skipRest);
}

void GLWImageList::mouseDrag(int button, float mx, float my, 
							 float x, float y, bool &skipRest)
{
	GLWidget::mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWImageList::itemSelected(GLWSelectorEntry *entry, int position)
{
	setCurrentShortPath(entry->getDataText());
}

