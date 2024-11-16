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

#if !defined(__INCLUDE_NetworkChatDialogh_INCLUDE__)
#define __INCLUDE_NetworkChatDialogh_INCLUDE__

#include <SDL/SDL_thread.h>
#include <SDL/SDL.h>
#include <GLW/GLWWindow.h>
#include <GLW/GLWChatView.h>

class NetworkChatDialogTextRenderer : public GLFont2dI
{
public:
	NetworkChatDialogTextRenderer(int len);
	virtual ~NetworkChatDialogTextRenderer();
    
	// GLFont2d
	virtual bool drawCharacter(
		unsigned int character,
		int charPosition, Vector &position, 
		GLFont2dStorage::CharEntry &charEntry, Vector4 &color);

private:
	int len_;
};

class NetworkChatDialog : public GLWWindow 
{
public:
	static NetworkChatDialog *instance();

	// Inherited from GLWWindow
	virtual void draw();
	virtual void display();
	virtual void hide();

	static int threadFunc(void *);

protected:
	static NetworkChatDialog *instance_;
	std::list<std::string> messages_;
	GLWChatView *chatView_;
	SDL_mutex *mutex_;
	bool sendMessages_;
	int lastMessageId_, messageDelay_;

	void actualThreadFunc();
	void sendMessage();

private:
	NetworkChatDialog();
	virtual ~NetworkChatDialog();
};

#endif
