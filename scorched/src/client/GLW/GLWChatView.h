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

#if !defined(__INCLUDE_GLWChatViewh_INCLUDE__)
#define __INCLUDE_GLWChatViewh_INCLUDE__

#include <GLW/GLWidget.h>
#include <GLW/GLWIconButton.h>
#include <GLEXT/GLFont2d.h>
#include <GLEXT/GLTextureReference.h>
#include <common/KeyboardKey.h>
#include <common/Vector.h>
#include <lang/LangString.h>
#include <list>

class GLWChatView : 
	public GLWidget,
	public GLWButtonI
{
public:
	GLWChatView(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f);
	virtual ~GLWChatView();

	void addLargeChat(const Vector &color, const LangString &text, GLFont2dI *render = 0);
	void addChat(const Vector &color, const LangString &text, GLFont2dI *render = 0);
	void clearChat();

	virtual bool initFromXMLInternal(XMLNode *node);

	void setAllowScroll(bool allowScroll) { allowScroll_ = allowScroll; }
	void setDisplayTime(float displayTime) { displayTime_ = displayTime; }
	void setSplitLargeLines(bool splitLargeLines) { splitLargeLines_ = splitLargeLines; }
	void setVisibleLines(int visibleLines) { visibleLines_ = visibleLines; }

	bool getParentSized() { return parentSized_; }
	void setParentSized(bool parentSized) { parentSized_ = parentSized; }

	// GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual bool initFromXML(XMLNode *node);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void setX(float x);
	virtual void setY(float y);

	// ButtonI
	virtual void buttonDown(unsigned int id);

	REGISTER_CLASS_HEADER(GLWChatView);

protected:
	bool init_;

	int splitLine(const LangString &message);
private:
	class GLWChatViewEntry
	{
	public:
		GLWChatViewEntry(const Vector &color, const LangString &text, 
			float timeRemaining, GLFont2dI *renderer) 
		{
			this->color = color;
			this->text = text;
			this->timeRemaining = timeRemaining;
			this->renderer = renderer;
		}
		virtual ~GLWChatViewEntry()
		{
			delete renderer;
		}

		Vector color;
		LangString text;
		float timeRemaining;
		GLFont2dI *renderer;
	};

	GLWIconButton upButton_;
	GLWIconButton downButton_;
	GLWIconButton resetButton_;

	bool alignTop_, parentSized_;
	bool splitLargeLines_, allowScroll_;
	int lineDepth_;
	int scrollPosition_;
	float displayTime_;
	float fontSize_, outlineFontSize_;
	int visibleLines_, totalLines_;
	int currentVisible_;
	std::list<GLWChatViewEntry *> textLines_;
	KeyboardKey *scrollUpKey_;
	KeyboardKey *scrollDownKey_;
	KeyboardKey *scrollResetKey_;
};

#endif // __INCLUDE_GLWChatViewh_INCLUDE__
