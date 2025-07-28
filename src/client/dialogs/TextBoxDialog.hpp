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

#if !defined(__INCLUDE_TextBoxDialogh_INCLUDE__)
#define __INCLUDE_TextBoxDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWTextBox.h>

class TextBoxDialog;
class TextBoxDialogI 
{
public:
	virtual void textBoxResult(TextBoxDialog *dialog, const LangString &result) = 0;
};

class TextBoxDialog : public GLWWindow,
	public GLWButtonI
{
public:
	static TextBoxDialog *instance();

	static void show(const LangString &message, const LangString &text, TextBoxDialogI *user);
	static LangString &getText() { return instance_->result_->getLangString(); }

	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	virtual void buttonDown(unsigned int id);

protected:
	static TextBoxDialog *instance_;

	GLWTextButton *okButton_, *cancelButton_;
	GLWTextBox *result_;
	GLWLabel *message_;
	TextBoxDialogI *user_;
	
private:
	TextBoxDialog();
	virtual ~TextBoxDialog();

};

#endif
