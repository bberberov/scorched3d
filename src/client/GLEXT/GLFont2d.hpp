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

#ifndef _GLFONT2D_H_
#define _GLFONT2D_H_

#include <common/Vector.h>
#include <common/Vector4.h>
#include <lang/LangString.h>
#include <GLEXT/GLFont2dStorage.h>

class GLFont2dFreeType;
class GLFont2dI;
class GLFont2d  
{
public:
	GLFont2d();
	~GLFont2d();

	bool createFont(const std::string &typeFace, unsigned int h, bool makeShadow = false);

	void draw(Vector &color, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawA(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawWidth(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const LangString &text);
	float drawWidthRhs(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawSubStr(int start, int len,
		Vector &color, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawSubStrA(int start, int len,
		Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const LangString &text);
	void drawBilboard(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const LangString &text);

	float getWidth(float size, const unsigned int c);
	float getWidth(float size, const LangString &text, int len = 0);
	int getChars(float size, const LangString &text, float width);

	void draw(Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawA(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawA(GLFont2dI *handler, Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawWidth(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawWidthRhs(float width, 
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawSubStr(int start, int len,
		Vector &color, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawSubStrA(int start, int len,
		Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);
	void drawBilboard(Vector &color, float alpha, float size, 
		float x, float y, float z, 
		const std::string &text);

	float getWidth(float size, const std::string &text, int len = 0);
	int getChars(float size, const std::string &text, float width);

	static unsigned int getTotalCharacters() { return totalCharacters_; }

protected:
	static unsigned int totalCharacters_;
	GLFont2dFreeType *freetype_;
	GLFont2dStorage characters_;
	LangString langText_;
	float height_;

	GLFont2dStorage::CharEntry *getCharacter(unsigned int character);

	void drawLetter(GLFont2dStorage::CharEntry *entry);

	bool drawString(unsigned len,
		Vector &color, float alpha, 
		float size, 
		float x, float y, float z, 
		const unsigned int *string,
		bool bilboard);
	bool drawStringHandler(unsigned length, 
		GLFont2dI *handler, 
		Vector &color, float alpha, 
		float size, 
		float x, float y, float z, 
		const unsigned int *string);
};

class GLFont2dI
{
public:
	virtual ~GLFont2dI() {}

	virtual bool drawCharacter(
		unsigned int character,
		int charPosition, Vector &position, 
		GLFont2dStorage::CharEntry &charEntry, Vector4 &color) = 0;
};

#endif /* _GLFONT2D_H_ */
