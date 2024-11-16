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

#if !defined(__INCLUDE_GLWChannelTexth_INCLUDE__)
#define __INCLUDE_GLWChannelTexth_INCLUDE__

#include <GLW/GLWChannelView.h>
#include <GLW/GLWIconButton.h>
#include <GLW/GLWSelector.h>
#include <GLEXT/GLTextureReference.h>
#include <string>

class GLWChannelText : 
	public GLWidget, 
	public GLWButtonI, 
	public GLWSelectorI, 
	public GLWChannelViewI
{
public:
	GLWChannelText();
	virtual ~GLWChannelText();

	// GLWChannelViewI
	virtual void channelsChanged(unsigned int id);

	// GLWSelectorI
	virtual void itemSelected(GLWSelectorEntry *entry, int position);

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

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
	virtual void setParent(GLWPanel *parent);
	virtual void setX(float x);
	virtual void setY(float y);
	virtual void setW(float w);
	virtual void setH(float h);

	REGISTER_CLASS_HEADER(GLWChannelText);
protected:
	static std::list<ChannelText> lastMessages_;
	GLTextureReference colorTexture_;
	GLWChannelViewTextRenderer prompt_;
	GLWChannelView::CurrentChannelEntry channelEntry_;
	GLWIconButton button_;
	GLWChannelView view_;
	std::map<KeyboardKey *, std::string> keys_;
	LangString text_;
	float fontSize_, outlineFontSize_;
	float ctime_;
	bool cursor_, visible_;
	bool createdTexture_;
	int maxTextLen_, cursorPosition_, historyPosition_;
	unsigned int whisperDest_;
	LangString whisperDestStr_;

	void processNotVisibleKey(unsigned int unicode, unsigned int dik, bool &skipRest);
	void processVisibleKey(unsigned int keystate, unsigned int unicode, unsigned int dik);
	void processSpecialText();
	void processNormalText();
	bool checkCurrentChannel();
	bool channelValid(const char *channelName);
	void setVisible(bool visible);
	void setChannelEntry(GLWChannelView::CurrentChannelEntry &entry);
	void setHistoryText();
};

#endif // __INCLUDE_GLWChannelTexth_INCLUDE__
