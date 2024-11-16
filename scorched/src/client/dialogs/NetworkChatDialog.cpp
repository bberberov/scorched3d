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

#include <dialogs/NetworkChatDialog.h>
#include <common/OptionsMasterListServer.h>
#include <net/NetBufferUtil.h>
#include <net/NetMessagePool.h>
#include <GLW/GLWChatView.h>
#include <GLEXT/GLViewPort.h>
#include <XML/XMLStringBuffer.h>


NetworkChatDialogTextRenderer::NetworkChatDialogTextRenderer(int len) :
	len_(len)
{
}

NetworkChatDialogTextRenderer::~NetworkChatDialogTextRenderer()
{
}
    
bool NetworkChatDialogTextRenderer::drawCharacter(
	unsigned int character,
	int charPosition, Vector &position, 
	GLFont2dStorage::CharEntry &charEntry, Vector4 &color)
{
	if (charPosition <= len_)
	{
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
	}
	return true;
}

NetworkChatDialog *NetworkChatDialog::instance_ = 0;

NetworkChatDialog *NetworkChatDialog::instance()
{
	if (!instance_)
	{
		instance_ = new NetworkChatDialog;
	}

	return instance_;
}

NetworkChatDialog::NetworkChatDialog() :
	GLWWindow("", 780.0f, 100.0f, eNoTitle | eNoDraw | eNoMove | eClickTransparent , ""),
	sendMessages_(false),
	lastMessageId_(0), messageDelay_(0)
{
	OptionsMasterListServer::instance();

	chatView_ = new GLWChatView();
	addWidget(chatView_);
	chatView_->setParentSized(true);
	chatView_->setAllowScroll(true);
	chatView_->setSplitLargeLines(true);
	chatView_->setDisplayTime(3600);
	chatView_->setVisibleLines(7);

	needCentered_ = true;
	mutex_ = SDL_CreateMutex();
	SDL_CreateThread(NetworkChatDialog::threadFunc, (void *) this);
}

NetworkChatDialog::~NetworkChatDialog()
{
}

void NetworkChatDialog::draw()
{
	if (needCentered_)
	{
		int wWidth = GLViewPort::getWidth();
		setX((wWidth - getW()) / 2.0f);
		setY(20.0f);
		needCentered_ = false;
	}

	SDL_LockMutex(mutex_);
	if (sendMessages_ &&
		!messages_.empty())
	{
		std::list<std::string>::iterator itor;
		for (itor = messages_.begin();
			itor != messages_.end();
			++itor)
		{
			std::string &message = *itor;
			int len = message.find("says :");
			NetworkChatDialogTextRenderer *renderer = new NetworkChatDialogTextRenderer(len + 4);
			chatView_->addLargeChat(Vector(1.0f, 1.0f, 1.0f), LANG_STRING(message), renderer);
		}
		messages_.clear();
	}
	SDL_UnlockMutex(mutex_);

	GLWWindow::draw();
}

void NetworkChatDialog::display()
{
	GLWWindow::display();

	chatView_->clearChat();
	SDL_LockMutex(mutex_);
	messages_.clear();
	SDL_UnlockMutex(mutex_);
	lastMessageId_ = 0;
	messageDelay_ = 0;
	sendMessages_ = true;
}

void NetworkChatDialog::hide()
{
	GLWWindow::hide();
	
	sendMessages_ = false;
	chatView_->clearChat();
}

int NetworkChatDialog::threadFunc(void *data)
{
	NetworkChatDialog *dialog = (NetworkChatDialog *) data;
	dialog->actualThreadFunc();
	return 0;
}

void NetworkChatDialog::actualThreadFunc()
{
	for (;;)
	{
		if (!sendMessages_) 
		{
			SDL_Delay(1000);
		}
		else
		{
			int lastMessageId = lastMessageId_;
			sendMessage();
			if (lastMessageId == lastMessageId_)
			{
				messageDelay_ += 1;
				if (messageDelay_ > 20) {
					messageDelay_ = 20;
				}
			}
			else
			{
				messageDelay_ = 0;
			}

			for (int i=0; i<=messageDelay_ && sendMessages_; i++)
			{
				SDL_Delay(500);
			}
		}
	}
}

void NetworkChatDialog::sendMessage()
{
	const char *chatServer = OptionsMasterListServer::instance()->getChatServer();
	const char *charServerURI = OptionsMasterListServer::instance()->getChatServerURI();

	IPaddress ipAddress;
	if (SDLNet_ResolveHost(&ipAddress, chatServer, 80) != 0)
	{
		return;
	}

	TCPsocket tcpsock = SDLNet_TCP_Open(&ipAddress);
	if (!tcpsock) 
	{
		return;		
	}
	NetBufferUtil::setBlockingIO(tcpsock);

	std::string request = S3D::formatStringBuffer(
		"GET %s?lastid=%i HTTP/1.0\r\n"
		"Cache-Control: no-cache\r\n"
		"Pragma: no-cache\r\n"
		"User-Agent: Scorched3D\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"\r\n"
		"\r\n",
		charServerURI, lastMessageId_, chatServer);
	if (SDLNet_TCP_Send(tcpsock, request.c_str(), request.size()) != request.size())
	{
		SDLNet_TCP_Close(tcpsock);
		return;	
	}

	std::string result;
	char buffer[256];
	for (;;)
	{
		int bytesRead = SDLNet_TCP_Recv(tcpsock, buffer, sizeof(buffer));
		if (bytesRead <=0) break;
		result.append(std::string(buffer, bytesRead));
	}

	if (result.find("200 OK") != -1)
	{
		const char *data = strstr(result.c_str(), "\r\n\r\n");
		if (data) 
		{
			data+=4;

			std::string result = data;
			XMLStringBuffer xml;
			if (xml.create(result.c_str(), result.size()))
			{
				if (xml.getRootNode())
				{
					std::list<XMLNode *> &children = xml.getRootNode()->getChildren();
					std::list<XMLNode *>::iterator itor;
					for (itor = children.begin();
						itor != children.end();
						++itor)
					{
						XMLNode *messageNode = *itor;
						std::string id, user, date, message;
						messageNode->getNamedParameter("id", id);
						messageNode->getNamedParameter("user", user);
						messageNode->getNamedParameter("date", date);
						message = messageNode->getContent();
						lastMessageId_ = atoi(id.c_str());

						std::string actualMessage = 
							S3D::formatStringBuffer("%s %s says : %s", 
								date.c_str(), user.c_str(), message.c_str());

						SDL_LockMutex(mutex_);
						messages_.push_back(actualMessage);
						SDL_UnlockMutex(mutex_);
					}
				}
			}
		} 
	}

	SDLNet_TCP_Close(tcpsock);
}