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

#include <server/ServerAuthHandlerForumLogin.h>
#include <server/ScorchedServer.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <XML/XMLFile.h>

#ifdef HAVE_MYSQL

ServerAuthHandlerForumLogin::ServerAuthHandlerForumLogin() : 
	mysql_(0), success_(false)
{
}

ServerAuthHandlerForumLogin::~ServerAuthHandlerForumLogin()
{
	if (mysql_) mysql_close(mysql_);
}

void ServerAuthHandlerForumLogin::createAuthentication(ComsConnectAuthMessage &authMessage)
{
	authMessage.setPassword("required");
	authMessage.setUserName("required");
}

bool ServerAuthHandlerForumLogin::authenticateUser(ComsConnectAuthMessage &authMessage, 
	std::string &message)
{
	if (!connectHandler()) return false;

	// Check to see if a username has been provided
	if (!authMessage.getUserName()[0] ||
		!authMessage.getPassword()[0])
	{
		message = S3D::formatStringBuffer(
			"This server is running in secure mode.\n"
			"You need to supply a username and password to connect.\n"
			"These should match your forum username and password.\n"
			"Please go to %s to register a forum account.\n", 
			authMessage.getUserName());;
		return false;
	}

	// Check to see if username exists
	int user_id = 0;
	{
		std::string text = S3D::formatStringBuffer(
			"SELECT user_id FROM phpbb2_users WHERE username = \"%s\"",
			authMessage.getUserName());
		mysql_real_query(mysql_, text.c_str(), text.size());
		MYSQL_RES *result = mysql_store_result(mysql_);
		if (result)
		{
			int rows = (int) mysql_num_rows(result);
			for (int r=0; r<rows; r++)
			{
				MYSQL_ROW row = mysql_fetch_row(result);
				user_id = atoi(row[0]);
			}
			mysql_free_result(result);
		}		
	}
	if (!user_id)
	{
		message = S3D::formatStringBuffer(
			"This server is running in secure mode.\n"
			"Your supplied username was not recognised.\n"
			"Please go to %s to register a forum account.\n", name_.c_str());;
		return false;
	}

	// Check to see if password matches for username
	int password_user_id = 0;
	int scorched3dbanned = 0;
	std::string password_user_statsid;
	{
		std::string text = S3D::formatStringBuffer(
			"SELECT user_id, user_scorched3duid, user_scorched3dbanned "
			"FROM phpbb2_users WHERE username = \"%s\" "
			"AND user_password = MD5(\"%s\")",
			authMessage.getUserName(), authMessage.getPassword());
		mysql_real_query(mysql_, text.c_str(), text.size());
		MYSQL_RES *result = mysql_store_result(mysql_);
		if (result)
		{
			int rows = (int) mysql_num_rows(result);
			for (int r=0; r<rows; r++)
			{
				MYSQL_ROW row = mysql_fetch_row(result);
				password_user_id = atoi(row[0]);
				password_user_statsid = row[1];
				scorched3dbanned = atoi(row[2]);
			}
			mysql_free_result(result);
		}		
	}
	if (!password_user_id)
	{
		message = 
			"This server is running in secure mode.\n"
			"Your supplied password was not correct.\n"
			"Please ensure you are using the same password as your forum account.\n";
		return false;
	}
	// Check to see if this user has been banned
	if (scorched3dbanned == 1)
	{
		message = 
			"This server is running in secure mode.\n"
			"Your supplied username has been banned.\n";
		return false;
	}

	// Update unique id on forum or from fourm
	if (password_user_statsid.c_str()[0])
	{
		authMessage.setUniqueId(password_user_statsid.c_str());
	}
	else if (authMessage.getUniqueId()[0])
	{
		std::string text = S3D::formatStringBuffer(
			"UPDATE phpbb2_users SET user_scorched3duid = \"%s\" WHERE user_id = %i ",
			authMessage.getUniqueId(),
			password_user_id);
		mysql_real_query(mysql_, text.c_str(), text.size());
	}

	// Check if this unique id has been banned
	if (authMessage.getUniqueId()[0])
	{
		bool bannedUniqueId = false;
		{
			std::string text = S3D::formatStringBuffer(
				"SELECT user_id, user_scorched3dbanned "
				"FROM phpbb2_users WHERE user_scorched3duid = \"%s\" ",
				authMessage.getUniqueId());
			mysql_real_query(mysql_, text.c_str(), text.size());
			MYSQL_RES *result = mysql_store_result(mysql_);
			if (result)
			{
				int rows = (int) mysql_num_rows(result);
				for (int r=0; r<rows; r++)
				{
					MYSQL_ROW row = mysql_fetch_row(result);
					if (atoi(row[1]) == 1) bannedUniqueId = true;
				}
				mysql_free_result(result);
			}		
		}
		if (bannedUniqueId)
		{
			message = 
				"This server is running in secure mode.\n"
				"Your supplied uniqueid has been banned.\n";
			return false;
		}
	}

	return true;
}

bool ServerAuthHandlerForumLogin::authenticateUserName(const char *uniqueId, 
	const LangString &lsplayername)
{
	if (!connectHandler()) return false;

	std::string splayername = LangStringUtil::convertFromLang(lsplayername);
	const char *playername = splayername.c_str();

	bool userResult = true;
	{
		std::string text = S3D::formatStringBuffer(
			"SELECT user_id , user_scorched3duid "
			"FROM phpbb2_users WHERE username = \"%s\"",
			playername);
		mysql_real_query(mysql_, text.c_str(), text.size());
		MYSQL_RES *result = mysql_store_result(mysql_);
		if (result)
		{
			int rows = (int) mysql_num_rows(result);
			for (int r=0; r<rows; r++)
			{
				MYSQL_ROW row = mysql_fetch_row(result);
				userResult = (strcmp(row[1], uniqueId) == 0);
			}
			mysql_free_result(result);
		}		
	}	

	return userResult;
}

void ServerAuthHandlerForumLogin::banUser(const char *uniqueId)
{
	if (!connectHandler()) return;

	{
		std::string text = S3D::formatStringBuffer(
			"UPDATE phpbb2_users SET user_scorched3dbanned = 1 WHERE user_scorched3duid = \"%s\" ",
			uniqueId);
		mysql_real_query(mysql_, text.c_str(), text.size());
	}
}

bool ServerAuthHandlerForumLogin::connectHandler()
{
	if (mysql_) return success_;

    mysql_ = mysql_init(0);
	if (!mysql_)
	{
		Logger::log( "Failed to init mysql");
		return false;
	}

	XMLFile file;
	std::string fileName = S3D::getSettingsFile(S3D::formatStringBuffer("forumlogin-%i.xml",
		ScorchedServer::instance()->getOptionsGame().getPortNo()));

	std::string host, user, passwd, db;
	if (!file.readFile(fileName) ||
		!file.getRootNode())
	{
		Logger::log(S3D::formatStringBuffer("Failed to parse %s settings file. Error: %s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}

	if (!file.getRootNode()->getNamedChild("host", host) ||
		!file.getRootNode()->getNamedChild("user", user) ||
		!file.getRootNode()->getNamedChild("name", name_) ||
		!file.getRootNode()->getNamedChild("passwd", passwd) ||
		!file.getRootNode()->getNamedChild("db", db)) 
	{
		Logger::log(S3D::formatStringBuffer("Failed to parse %s settings file.", fileName.c_str()));
		return false;
	}

	if (!mysql_real_connect(
		mysql_,
		host.c_str(),
		user.c_str(),
		passwd.c_str(),
		db.c_str(),
		0, "/tmp/mysql.sock", 0))
	{
		Logger::log(S3D::formatStringBuffer("forum login auth handler failed to start. "
			"Error: %s",
			mysql_error(mysql_)));
		Logger::log(S3D::formatStringBuffer("mysql params : host %s, user %s, passwd %s, db %s",
			host.c_str(), user.c_str(),
			passwd.c_str(), db.c_str()));
		return false;
	}

	success_ = true;
	return success_;
}

#endif // #ifdef HAVE_MYSQL

