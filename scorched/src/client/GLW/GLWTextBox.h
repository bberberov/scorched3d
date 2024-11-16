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

#ifndef _GLWTEXTBOX_H_
#define _GLWTEXTBOX_H_

#include <string>
#include <GLW/GLWidget.h>

class GLWTextBoxI
{
public:
	virtual void textChanged(unsigned int id, const LangString &text) = 0;
};

class GLWTextBox : public GLWidget
{
public:
	enum TextFlags
	{
		eFlagPassword = 1
	};

	GLWTextBox(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, 
		const LangString &startText = LangString(),
		unsigned int flags = 0);
	virtual ~GLWTextBox();

	void setHandler(GLWTextBoxI *handler) { handler_ = handler; }

	virtual void setParent(GLWPanel *parent);
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);

	void setCurrent();
	void setAllowUnicode(bool allowUnicode) { allowUnicode_ = allowUnicode; }

	std::string &getText();
	LangString &getLangString() { return text_; }
	void setText(const LangString &text);
	void setMaxTextLen(unsigned int maxLen) { maxTextLen_ = maxLen; }

	REGISTER_CLASS_HEADER(GLWTextBox);

protected:
	float ctime_;
	bool cursor_, current_;
	bool allowUnicode_;
	unsigned int maxTextLen_, flags_;
	LangString text_;
	GLWTextBoxI *handler_;

};

#endif /* _GLWTEXTBOX_H_ */
