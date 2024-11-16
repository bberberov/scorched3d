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

#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <dialogs/NetworkChatDialog.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWPanel.h>
#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLTextureStore.h>
#include <serverbrowser/ServerBrowser.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>

struct ColumnInfo
{
	GLWIconTable::Column col;
	const char *dataName;
};
static ColumnInfo *getGamesCols()
{
	static ColumnInfo gamescols[] = 
	{
		GLWIconTable::Column(LANG_STRING(""), 60), "",
		GLWIconTable::Column(LANG_RESOURCE("SERVER_NAME", "Server Name"), 255), "servername",
		GLWIconTable::Column(LANG_RESOURCE("PLYRS", "Plyrs"), 60), "noplayers",
		GLWIconTable::Column(LANG_RESOURCE("ROUND", "Round"), 55), "round",
		GLWIconTable::Column(LANG_RESOURCE("MOD", "Mod"), 95), "mod",
		GLWIconTable::Column(LANG_RESOURCE("GAME_TYPE", "Game Type"), 200), "gametype",
		GLWIconTable::Column(LANG_STRING(""), -1), ""
	};
	return gamescols;
}

static ColumnInfo *getPlayerCols() 
{
	static ColumnInfo playerscols[] =
	{
		GLWIconTable::Column(LANG_RESOURCE("PLAYER", "Player"), 270), "pn",
		GLWIconTable::Column(LANG_RESOURCE("SCORE", "Score"), 260), "ps",
		GLWIconTable::Column(LANG_RESOURCE("TIME", "Time"), 140), "pt",
		GLWIconTable::Column(LANG_RESOURCE("REAL", "Real"), 60), "pa",
		GLWIconTable::Column(LANG_STRING(""), -1), ""
	};
	return playerscols;
}

NetworkSelectDialog *NetworkSelectDialog::instance_ = 0;

NetworkSelectDialog *NetworkSelectDialog::instance()
{
	if (!instance_)
	{
		instance_ = new NetworkSelectDialog;
	}
	return instance_;
}

NetworkSelectDialog::NetworkSelectDialog() : 
	GLWWindow("Network", 780.0f, 560.0f, eHideName, ""),
	totalTime_(0.0f), invalidateId_(0),
	okTex_(ImageID(S3D::eDataLocation,
		"data/images/ok.bmp",
		"data/images/mask.bmp")),
	questionTex_(ImageID(S3D::eDataLocation,
		"data/images/question.bmp",
		"data/images/mask.bmp")),
	warningTex_(ImageID(S3D::eDataLocation,
		"data/images/warn.bmp",
		"data/images/mask.bmp")),
	noentryTex_(ImageID(S3D::eDataLocation,
		"data/images/noentry.bmp",
		"data/images/mask.bmp")),
	exclaimTex_(ImageID(S3D::eDataLocation,
		"data/images/exclaim.bmp",
		"data/images/mask.bmp")),
	keyTex_(ImageID(S3D::eDataLocation,
		"data/images/key.bmp",
		"data/images/keya.bmp",
		true)),
	cogTex_(ImageID(S3D::eDataLocation,
		"data/images/cog.bmp",
		"data/images/coga.bmp",
		true)),
	tankTex_(ImageID(S3D::eDataLocation,
		"",
		"data/images/tank2.png"))
{
	std::list<GLWIconTable::Column> gamescolumns, playerscolumns;
	for (int i=0;; i++)
	{
		if (getGamesCols()[i].col.width == -1) break;
		gamescolumns.push_back(getGamesCols()[i].col);
	}
	for (int i=0;; i++)
	{
		if (getPlayerCols()[i].col.width == -1) break;
		playerscolumns.push_back(getPlayerCols()[i].col);
	}

	gamesIconTable_ = new GLWIconTable(10.0f, 165.0f, 760.0f, 350.0f, &gamescolumns, 20.0f);
	addWidget(gamesIconTable_);
	gamesIconTable_->setHandler(this);

	playersIconTable_ = new GLWIconTable(10.0f, 45.0f, 760.0f, 110.0f, &playerscolumns, 20.0f);
	addWidget(playersIconTable_);
	playersIconTable_->setHandler(this);

	ok_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("JOIN_GAME", "Join Game"), 640, 10, 130, this, 
		GLWButton::ButtonFlagCenterX));
	cancelId_ = addWidget(
		new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 530, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX))->getId();
	refresh_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("REFRESH_LIST", "Refresh List"), 175, 10, 150, this, 
		GLWButton::ButtonFlagCenterX));
	favourites_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("ADD_FAVOURITE", "Add Favourite"), 10, 10, 155, this, 
		GLWButton::ButtonFlagCenterX));

	addWidget(new GLWPanel(220.0f, 525.0f, 300.0f, 25.0f, true));
	ipaddress_ = (GLWLabel *) addWidget(
		new GLWLabel(225.0f, 525.0f));
	connectTo_ = (GLWTextButton *) addWidget(
		new GLWTextButton(LANG_RESOURCE("CONNECT_TO_LABEL", "Connect To :"), 75.0f, 527.0f, 135, this,
		GLWButton::ButtonFlagCenterX));

	refreshType_ = (GLWDropDownText *) addWidget(
		new GLWDropDownText(530.0f, 525.0f, 150.0f));
	refreshType_->addText(LANG_RESOURCE("INTERNET", "Internet"), "Internet");
	refreshType_->addText(LANG_RESOURCE("LAN", "LAN"), "LAN");
	refreshType_->addText(LANG_RESOURCE("FAVOURITES", "Favourites"), "Favourites");
	refreshType_->setCurrentText(LANG_RESOURCE("INTERNET", "Internet"));
	refreshType_->setHandler(this);
}

NetworkSelectDialog::~NetworkSelectDialog()
{

}

void NetworkSelectDialog::draw()
{
	if (needCentered_)
	{
		int wWidth = GLViewPort::getWidth();
		int wHeight = GLViewPort::getHeight();
		setX((wWidth - getW()) / 2.0f);
		setY(MAX(0, (wHeight - getH()) - 25.0f));
		needCentered_ = false;
	}

	GLWWindow::draw();
}

void NetworkSelectDialog::simulate(float frameTime)
{
	GLWWindow::simulate(frameTime);

	totalTime_ += frameTime;
	if (totalTime_ > 1.0f)
	{
		updateTable();
	}
}

void NetworkSelectDialog::drawIcon(GLTexture *tex, float &x, float y, LangString &message)
{
	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
	glColor3f(1.0f, 1.0f, 1.0f);
	tex->draw();

	float w = 18.0f;
	float h = 18.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + w, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + w, y + h);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + h);
	glEnd();

	if (GLWToolTip::instance()->addToolTip(&colToolTip_, 
		GLWTranslate::getPosX() + x, 
		GLWTranslate::getPosY() + y, 
		w, h))
	{
		colToolTip_.setText(ToolTip::ToolTipInfo, 
			LANG_RESOURCE("STATUS_ICON", "Status Icon"), message);
	}

	x += w;
}

GLTexture *NetworkSelectDialog::getTexture(int row, LangString *&message)
{
	std::string pversion =
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "protocolversion");
	std::string version =
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "version");
	if (!serverCompatable(pversion, version))
	{
		LANG_RESOURCE_CONST_VAR(INCOMPATIBLE, 
			"INCOMPATIBLE_VERSION", "Incompatible version.");
		message = &INCOMPATIBLE;
		return noentryTex_.getTexture();
	}

	std::string clients = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "noplayers");
	std::string maxclients = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "maxplayers");
	if (clients.size() > 0 &&
		0 == strcmp(clients.c_str(), maxclients.c_str()))
	{
		LANG_RESOURCE_CONST_VAR(SERVER_FULL, 
			"SERVER_FULL", "Server is full.");
		message = &SERVER_FULL;
		return exclaimTex_.getTexture();
	}
	
	std::string state = 
		ServerBrowser::instance()->getServerList().
			getEntryValue(row, "state");
	if (0 == strcmp(state.c_str(), "Waiting"))
	{
		LANG_RESOURCE_CONST_VAR(NOT_STARTED, 
			"GAME_NOT_STARTED", "Game has not started.");
		message = &NOT_STARTED;
		return warningTex_.getTexture();
	}
	if (0 == strcmp(state.c_str(), "Started"))
	{
		LANG_RESOURCE_CONST_VAR(GAME_PROGESS, 
			"GAME_IN_PROGRESS", "Game in progress and spaces on server.");
		message = &GAME_PROGESS;
		return okTex_.getTexture();
	}

	LANG_RESOURCE_CONST_VAR(CANNOT_CONTACT_SERVER, 
		"CANNOT_CONTACT_SERVER", "Cannot contact server.");
	message = &CANNOT_CONTACT_SERVER;
	return questionTex_.getTexture();
}

void NetworkSelectDialog::drawColumn(unsigned int id, int row, int col,
	float x, float y, float w)
{
	if (id == gamesIconTable_->getId()) drawColumnGames(id, row, col, x, y, w);
	else if (id == playersIconTable_->getId()) drawColumnPlayers(id, row, col, x, y, w);
}

void NetworkSelectDialog::drawColumnGames(unsigned int id, int row, int col, 
	float x, float y, float w)
{
	if (row < 0 || row >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	std::string value, tipValue;
	if (col == 0)
	{
		LANG_RESOURCE_CONST_VAR(NONE, "NONE", "None");

		{
			LangString *message = &NONE;
			GLTexture *tex = getTexture(row, message);
			drawIcon(tex, x, y, *message);
		}

		std::string key = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "password");
		if (0 == strcmp(key.c_str(), "On"))
		{
			LANG_RESOURCE_CONST_VAR(
				PASSWORD_PROTECTED, "PASSWORD_PROTECTED", "Password protected.");
			drawIcon(keyTex_.getTexture(), x, y, PASSWORD_PROTECTED);
		}

		std::string officialStr = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "type");
		if (officialStr == "official")
		{
			LANG_RESOURCE_CONST_VAR(
				OFFICAL_SERVER, "OFFICAL_SERVER", "An offical server.");
			drawIcon(tankTex_.getTexture(), x, y, OFFICAL_SERVER);
		}
		else if (officialStr == "mod")
		{
			LANG_RESOURCE_CONST_VAR(
				MOD_SERVER, "MOD_SERVER", "Home of mod server.");
			drawIcon(cogTex_.getTexture(), x, y, MOD_SERVER);
		}
	}
	else if (col == 2)
	{
		std::string clients = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "noplayers");
		std::string maxclients = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "maxplayers");
		std::string compplayers = 
			ServerBrowser::instance()->getServerList().
				getEntryValue(row, "compplayers");

		std::string name;
		if (compplayers.c_str()[0])
		{
			name = S3D::formatStringBuffer("%s/%s (%i)", clients.c_str(), maxclients.c_str(),
				(atoi(clients.c_str()) - atoi(compplayers.c_str())));
		}
		else
		{
			name = S3D::formatStringBuffer("%s/%s (?)", clients.c_str(), maxclients.c_str());
		}

		std::string message = name;
		int noplayers = atoi(clients.c_str());
		for (int i=0; i<noplayers; i++)
		{
			std::string pn = 
				ServerBrowser::instance()->getServerList().getEntryValue(
					row, S3D::formatStringBuffer("pn%i", i));
			std::string pa = 
				ServerBrowser::instance()->getServerList().getEntryValue(
					row, S3D::formatStringBuffer("pa%i", i));

			message.append(S3D::formatStringBuffer("\n%i: %s: %s", 
				i, pa.c_str(), pn.c_str()));
		}

		value = S3D::formatStringBuffer("%s", name.c_str());
		tipValue = S3D::formatStringBuffer("%s", message.c_str());
	}
	else
	{
		tipValue = value = ServerBrowser::instance()->getServerList().getEntryValue(row, getGamesCols()[col].dataName);
		if (value.empty() && col == 1)
		{
			tipValue = value = ServerBrowser::instance()->getServerList().getEntryValue(row, "address");
		}
	}

	if (!value.empty())
	{
		Vector color(0.3f, 0.3f, 0.3f);
		GLWFont::instance()->getGameFont()->drawWidth(w, 
			color, 
			10.0f, x + 3.0f, y + 5.0f, 0.0f, value);

		if (!tipValue.empty())
		{
			if (GLWToolTip::instance()->addToolTip(&colToolTip_, 
				GLWTranslate::getPosX() + x, 
				GLWTranslate::getPosY() + y, 
				w, 20.0f))
			{
				colToolTip_.setText(ToolTip::ToolTipInfo, 
					getGamesCols()[col].col.name, 
					LANG_STRING(tipValue));
			}
		}
	}
}

void NetworkSelectDialog::drawColumnPlayers(unsigned int id, int row, int col, 
	float x, float y, float w)
{
	int gamesrow = gamesIconTable_->getSelected();
	if (gamesrow < 0 || gamesrow >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	std::string valuestr = 
		ServerBrowser::instance()->getServerList().getEntryValue(
			gamesrow, S3D::formatStringBuffer("%s%i", getPlayerCols()[col].dataName, row));
	const char *value = valuestr.c_str();

	Vector color(0.3f, 0.3f, 0.3f);
	GLWFont::instance()->getGameFont()->drawWidth(w, 
		color, 
		10.0f, x + 3.0f, y + 5.0f, 0.0f, value);
}

bool NetworkSelectDialog::serverCompatable(std::string pversion, std::string version)
{
	if (pversion.size() > 0 && 
		pversion != S3D::ScorchedProtocolVersion)
	{
		return false;
	}

	return true;
}

void NetworkSelectDialog::rowSelected(unsigned int id, int row)
{
	if (id == gamesIconTable_->getId()) rowSelectedGames(id, row);
}

void NetworkSelectDialog::rowSelectedGames(unsigned int id, int row)
{
	if (row < 0 || row >= ServerBrowser::instance()->getServerList().getNoEntries())
	{
		return;
	}

	// Check if we have any information for this row
	std::string ipaddress = ServerBrowser::instance()->
		getServerList().getEntryValue(row, "address");
	if (ipaddress.empty()) return;

	// Check if this server is compatible
	std::string protocolVersion = 
		ServerBrowser::instance()->getServerList().getEntryValue(row, "protocolversion");
	std::string version =
		ServerBrowser::instance()->getServerList().getEntryValue(row, "version");
	if (!serverCompatable(protocolVersion, version))
	{
		LangString msg = LANG_RESOURCE_4("INCOMPATIBLE_VERSION_WARNING",
			"Warning: This server is running a incompatible version of Scorched3D.\n"
			"You cannot connect to this server.\n\n"
			"This server is running Scorched build {0} ({1}).\n"
			"You are running Scorched build {2} ({3}).\n\n"					
			"The latest version of Scorched3D can be downloaded from \n"
			"http://www.scorched3d.co.uk\n",
			version, protocolVersion,
			S3D::ScorchedVersion, S3D::ScorchedProtocolVersion);
		MsgBoxDialog::show(msg);
	}

	// Set ip address for this server
	setIPAddress(LANG_STRING(ipaddress));

	// Set players for this server
	std::string players = 
		ServerBrowser::instance()->getServerList().getEntryValue(row, "noplayers");
	int noplayers = atoi(players.c_str());
	playersIconTable_->setItemCount(noplayers);
}

void NetworkSelectDialog::rowChosen(unsigned int id, int row)
{
	if (id == gamesIconTable_->getId()) rowChosenGames(id, row);
}

void NetworkSelectDialog::rowChosenGames(unsigned int id, int row)
{
	if (ok_->getEnabled()) buttonDown(ok_->getId());
}

void NetworkSelectDialog::columnSelected(unsigned int id, int col)
{
	if (id == gamesIconTable_->getId()) columnSelectedGames(id, col);
}

void NetworkSelectDialog::columnSelectedGames(unsigned int id, int col)
{
	ServerBrowser::instance()->getServerList().sortEntries(getGamesCols()[col].dataName);
}

void NetworkSelectDialog::display()
{
	GLWWindowManager::instance()->showWindow(NetworkChatDialog::instance()->getId());

	setIPAddress(LangString());
	refreshType_->setCurrentText(LANG_RESOURCE("INTERNET", "Internet"));
}

void NetworkSelectDialog::hide()
{
	GLWWindowManager::instance()->hideWindow(NetworkChatDialog::instance()->getId());

	stopRefresh();
}

void NetworkSelectDialog::updateTable()
{
	// Check if we are refreshing
	// Set the button accordingly
	if (!ServerBrowser::instance()->getRefreshing())
	{
		refresh_->setText(LANG_RESOURCE("REFRESH_LIST", "Refresh List"));
	}
	else
	{
		refresh_->setText(LANG_RESOURCE("STOP_REFRESH", "Stop Refresh"));
	}

	// Check if we have more items to display
	if (invalidateId_ != ServerBrowser::instance()->
		getServerList().getRefreshId())
	{
		invalidateId_ = ServerBrowser::instance()->
			getServerList().getRefreshId();
		gamesIconTable_->setItemCount(
			ServerBrowser::instance()->getServerList().getNoEntries());
	}
}

void NetworkSelectDialog::select(unsigned int id, const int pos, GLWSelectorEntry value)
{
	if (refreshType_->isSelected(LANG_RESOURCE("FAVOURITES", "Favourites")))
	{
		favourites_->setText(LANG_RESOURCE("DELETE_FAVOURITE", "Del Favourite"));
	}
	else
	{
		favourites_->setText(LANG_RESOURCE("ADD_FAVOURITE", "Add Favourite"));
	}

	startRefresh();
}

void NetworkSelectDialog::startRefresh()
{
	stopRefresh();

	gamesIconTable_->setItemCount(0);
	playersIconTable_->setItemCount(0);

	ServerBrowser::RefreshType t = ServerBrowser::RefreshNone;
	if (refreshType_->isSelected(LANG_RESOURCE("LAN", "LAN"))) t = ServerBrowser::RefreshLan;
	else if (refreshType_->isSelected(LANG_RESOURCE("INTERNET", "Internet"))) t = ServerBrowser::RefreshNet;
	else if (refreshType_->isSelected(LANG_RESOURCE("FAVOURITES", "Favourites"))) t = ServerBrowser::RefreshFavourites;

	ServerBrowser::instance()->refreshList(t);
	updateTable();
}

void NetworkSelectDialog::stopRefresh()
{
	ServerBrowser::instance()->cancel();
	updateTable();
}

void NetworkSelectDialog::setIPAddress(const LangString &text)
{
	ok_->setEnabled(text[0]!='\0');
	favourites_->setEnabled(text[0]!='\0');
	ipaddress_->setText(text);
}

void NetworkSelectDialog::textBoxResult(TextBoxDialog *dialog, const LangString &result)
{
	setIPAddress(result);
}

void NetworkSelectDialog::buttonDown(unsigned int id)
{
	if (id == refresh_->getId())
	{
		if (ServerBrowser::instance()->getRefreshing())
		{
			stopRefresh();
		}
		else
		{
			startRefresh();
		}
	}
	else if (id == ok_->getId())
	{
		GLWWindowManager::instance()->hideWindow(id_);

		if (!ipaddress_->getText().empty())
		{
			ClientParams::instance()->reset();
			ClientParams::instance()->setConnect(ipaddress_->getText().c_str());
			ClientMain::startClient();
		}
	}
	else if (id == favourites_->getId())
	{
		if (!ipaddress_->getText().empty())
		{
			std::set<std::string> favs = 
				ServerBrowser::instance()->getCollect().getFavourites();

			if (refreshType_->isSelected(LANG_RESOURCE("FAVOURITES", "Favourites")))
			{
				favs.erase(ipaddress_->getText().c_str());
			}
			else 
			{
				favs.insert(ipaddress_->getText().c_str());
			}
			ServerBrowser::instance()->getCollect().setFavourites(favs);

			if (refreshType_->isSelected(LANG_RESOURCE("FAVOURITES", "Favourites")))
			{
				startRefresh();
			}
		}
	}
	else if (id == cancelId_)
	{
		GLWWindowManager::instance()->hideWindow(id_);
	}
	else if (id == connectTo_->getId())
	{
		TextBoxDialog::instance()->show(LANG_RESOURCE("EDIT_CONNECTION", 
			"Edit connection details : HostName or HostName:Port\n"
			"e.g. 192.168.1.1:27270 or 192.168.1.2 or scorched3d.co.uk:27270"),
			ipaddress_->getLangString(), this);
	}
}
