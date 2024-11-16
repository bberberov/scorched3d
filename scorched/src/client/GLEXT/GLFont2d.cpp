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

#include <stdio.h> 
#include <GLEXT/GLFont2d.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLTextureBase.h>
#include <GLEXT/GLFont2dFreeType.h>
#include <common/Defines.h>

unsigned int GLFont2d::totalCharacters_(0);

GLFont2d::GLFont2d()
{
	freetype_ = new GLFont2dFreeType();
}

GLFont2d::~GLFont2d()
{
	delete freetype_;
}

float GLFont2d::getWidth(float size, const unsigned int c)
{
	float width = float(getCharacter(c)->advances) * size / height_;
	return width;
}

float GLFont2d::getWidth(float size, const LangString &text, int len)
{
	float width = 0.0f;
	if (len == 0) len = (int) text.size();

	const unsigned int *a = text.c_str();
	for (int i=0; i<len; i++, a++)
	{
		width += float(getCharacter(*a)->advances) * size / height_;
	}
	return width;
}

int GLFont2d::getChars(float size, const LangString &text, float len)
{
	int l = 0;
	float width = 0.0f;
	for (const unsigned int *a=text.c_str(); *a; a++)
	{
		width += float(getCharacter(*a)->advances) * size / height_;
		if (width < len) l++;
	}
	return l;
}

void GLFont2d::draw(Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, 1.0f, size, x, y, z, text.c_str(), false);
}

void GLFont2d::drawA(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, alpha, size, x, y, z, text.c_str(), false);
}

void GLFont2d::drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
	  float x, float y, float z, 
	  const LangString &text)
{
	drawStringHandler((GLsizei) text.size(), handler, color, alpha, size, x, y, z, text.c_str());
}

void GLFont2d::drawSubStr(int start, int len,
	 Vector &color, float size, 
	 float x, float y, float z, 
	 const LangString &text)
{
	drawSubStrA(start, len, color, 1.0f, size, x, y, z, text);
}

void GLFont2d::drawSubStrA(int start, int len,
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString(len, color, alpha, size, x, y, z, text.c_str() + start, false);
}

void GLFont2d::drawWidth(float len, Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	int l = getChars(size, text, len);
	drawString(l, color, 1.0f, size, x, y, z, text.c_str(), false);
}

float GLFont2d::drawWidthRhs(float len, Vector &color, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	float width = 0.0f;
	int slen = (int) text.size();
	if (slen > 0)
	{
		int l = 0;
		const unsigned int *a=& (text.c_str())[slen-1];
		for (; a >= text.c_str(); a--, l++)
		{
			float charWidth = float(getCharacter(*a)->advances) * size / height_;
			width += charWidth;
			if (width > len) 
			{
				width -= charWidth;
				break;
			}
		}
		a++;

		drawString(l, color, 1.0f, size, x, y, z, a, false);
	}
	return width;
}

void GLFont2d::drawBilboard(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const LangString &text)
{
	drawString((GLsizei) text.size(), color, alpha, size, x, y, z, text.c_str(), true);
}

float GLFont2d::getWidth(float size, const std::string &text, int len)
{
	LangStringUtil::replaceToLang(langText_, text);
	return getWidth(size, langText_, len);
}

int GLFont2d::getChars(float size, const std::string &text, float len)
{
	LangStringUtil::replaceToLang(langText_, text);
	return getChars(size, langText_, len);
}

void GLFont2d::draw(Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	draw(color, size, x, y, z, langText_);
}

void GLFont2d::drawA(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawA(color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawA(handler, color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawWidth(float width, 
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawWidth(width, color, size, x, y, z, langText_);
}

void GLFont2d::drawWidthRhs(float width, 
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawWidthRhs(width, color, size, x, y, z, langText_);
}

void GLFont2d::drawSubStr(int start, int len,
	Vector &color, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawSubStr(start, len, color, size, x, y, z, langText_);
}

void GLFont2d::drawSubStrA(int start, int len,
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawSubStrA(start, len, color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawBilboard(Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const std::string &text)
{
	LangStringUtil::replaceToLang(langText_, text);
	drawBilboard(color, alpha, size, x, y, z, langText_);
}

void GLFont2d::drawLetter(GLFont2dStorage::CharEntry *entry)
{
	Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX();
	Vector &bilY = GLCameraFrustum::instance()->getBilboardVectorY();

	glBindTexture(GL_TEXTURE_2D, entry->texture);
	glPushMatrix();

	Vector trans = bilX * (float) entry->left + 
		bilY * (float) entry->rows;
	glTranslatef(trans[0], trans[1], trans[2]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); 
		glVertex3fv(bilY * entry->height);

		glTexCoord2f(0.0f, entry->y); 
		glVertex3f(0.0f,0.0f,0.0f);

		glTexCoord2f(entry->x,entry->y); 
		glVertex3fv(bilX * entry->width);

		glTexCoord2f(entry->x,0.0f); 
		glVertex3fv(bilX * entry->width + bilY * entry->height);
	glEnd();
	glPopMatrix();

	bilX *= (float)(entry->advances);
	glTranslatef(bilX[0], bilX[1], bilX[2]);
}

bool GLFont2d::drawStringHandler(unsigned length, 
	GLFont2dI *handler, 
	Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const unsigned int *string)
{
	GLTextureBase::setLastBind(0); // Clear so no texture is cached

	GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);
	Vector4 acolor;
	Vector position(x, y, z);

	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glScalef(size / height_, size / height_, size / height_);

		int pos = 0;
		for (;*string; string++, pos++)
		{
			acolor[0] = color[0];
			acolor[1] = color[1];
			acolor[2] = color[2];
			acolor[3] = alpha;

			GLFont2dStorage::CharEntry *charEntry = getCharacter(*string);
			if (handler->drawCharacter(*string, pos, position, *charEntry, acolor))
			{
				glColor4fv(acolor);
				glCallList(charEntry->displaylist);
			}

			position[0] += float(charEntry->advances) * size / height_;
		}
	glPopMatrix();

	return true;
}

bool GLFont2d::drawString(unsigned length, Vector &color, float alpha, float size, 
	float x, float y, float z, 
	const unsigned int *string,
	bool bilboard)
{
	GLTextureBase::setLastBind(0); // Clear so no texture is cached

	GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);
	glColor4f(color[0], color[1], color[2], alpha);

	if (bilboard)
	{
		glPushMatrix();
			glTranslatef(x,y,z);
			glScalef(size / height_, size / height_, size / height_);

			for (const unsigned int *current = string; *current; current++)
			{
				GLFont2dStorage::CharEntry *entry = getCharacter(*current);
				drawLetter(entry);
			}
		glPopMatrix();
	}
	else 
	{
		static unsigned int lists[1024];
		unsigned int *currentList = lists;
		for (const unsigned int *current = string; *current; current++, currentList++)
		{
			GLFont2dStorage::CharEntry *entry = getCharacter(*current);
			*currentList = entry->displaylist;
		}

		glPushMatrix();
			glTranslatef(x,y,z);
			glScalef(size / height_, size / height_, size / height_);
			glCallLists(length, GL_UNSIGNED_INT, lists);
		glPopMatrix();
	}

	return true;
}

GLFont2dStorage::CharEntry *GLFont2d::getCharacter(unsigned int character)
{
	GLFont2dStorage::CharEntry *entry = characters_.getEntry(character);
	if (!entry->displaylist)
	{
		totalCharacters_++;
		freetype_->createCharacter(character, entry);
	}

	return entry;
}

bool GLFont2d::createFont(const std::string &typeFace, unsigned int h, bool makeShadow)
{
	height_ = float(h);
	return freetype_->createFont(typeFace, h, makeShadow);
}
