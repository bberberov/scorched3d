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

#include <server/ServerBrowserInfo.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <net/NetInterface.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <string.h>

ServerBrowserInfo *ServerBrowserInfo::instance_ = 0;

ServerBrowserInfo *ServerBrowserInfo::instance()
{
	if (!instance_)
	{
		instance_ = new ServerBrowserInfo();
	}
	return instance_;
}

ServerBrowserInfo::ServerBrowserInfo() : udpsock_(0)
{
	packetV_ = SDLNet_AllocPacketV(4, 10000);
	packetVOut_ = SDLNet_AllocPacketV(20, 10000);
}

ServerBrowserInfo::~ServerBrowserInfo()
{
}

bool ServerBrowserInfo::start()
{
	udpsock_ = SDLNet_UDP_Open(ScorchedServer::instance()->getOptionsGame().getPortNo() + 1);
	if(!udpsock_) return false;
	return true;
}

void ServerBrowserInfo::processMessages()
{ 
	if (!udpsock_) return;

	int numrecv = SDLNet_UDP_RecvV(udpsock_, packetV_);
	if(numrecv <=0) return;
	
	int numsent = 0;
	for(int i=0; i<numrecv; i++) 
	{
		if (packetV_[i]->len == 0) packetV_[i]->data[0] = '\0';

		//printf("Packet = %s\n", (char *) packetV_[i]->data);
		NetInterface::getBytesIn() += packetV_[i]->len;
		NetInterface::getPings() ++;

		// Only compare the first few characters so a NULL is not needed
		std::list<std::string> reply;
		const char *wrapper = "pong";
		if (0 == strncmp((char *) packetV_[i]->data, "status", 6))
		{
			wrapper = "status";
			processStatusMessage(reply);
		}
		else if (0 == strncmp((char *) packetV_[i]->data, "info", 4))
		{
			wrapper = "info";
			processInfoMessage(reply);
		}
		else if (0 == strncmp((char *) packetV_[i]->data, "players", 7))
		{
			wrapper = "players";
			processPlayerMessage(reply);
		}

		bool first = true;
		while (!reply.empty() || first)
		{
			first = false;
			
			std::string buffer = "<";
			buffer += wrapper;
			buffer += " ";
			while (!reply.empty())
			{
				std::string next = reply.front();
				reply.pop_front();
				if (strlen(next.c_str()) <= 75)
				{
					buffer += next;
				}
			
				if (strlen(buffer.c_str()) > 800) break;
			}
			buffer += "/>";
			
			int len = (int) strlen(buffer.c_str())+1;
			memcpy(packetVOut_[numsent]->data, buffer.c_str(), len);
			packetVOut_[numsent]->len = len;
			packetVOut_[numsent]->address.host = packetV_[i]->address.host;
			packetVOut_[numsent]->address.port = packetV_[i]->address.port;
			packetVOut_[numsent]->channel = packetV_[i]->channel;
			
			NetInterface::getBytesOut() += len;
			//printf("Packet len = %i\n", packetVOut_[numsent]->len);
			//printf("Packet = %s\n", packetVOut_[numsent]->data);
			
			if (++numsent > 20) break;
		}
	}
	
	//printf("Sending %i packets\n", numsent);
	SDLNet_UDP_SendV(udpsock_, packetVOut_, numsent);
}

void ServerBrowserInfo::processStatusMessage(std::list<std::string> &reply)
{
	char *serverName = (char *) ScorchedServer::instance()->getOptionsGame().getServerName();
	char version[256];
	snprintf(version, 256, "%s (%s)", S3D::ScorchedVersion.c_str(), S3D::ScorchedProtocolVersion.c_str());
	char players[25];
	snprintf(players, 25, "%i", ScorchedServer::instance()->getTargetContainer().getNoOfTanks());
	char maxplayers[25];
	snprintf(maxplayers, 25, "%i", ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());
	char type[100];
	snprintf(type, 100, "%s (%s)", 
		ScorchedServer::instance()->getOptionsGame().getTurnType().getValueAsString(),
		((ScorchedServer::instance()->getOptionsGame().getTeams() > 1)?"Teams":"No Teams"));
	bool stats = (0 != strcmp(ScorchedServer::instance()->getOptionsGame().getStatsLogger(), "none"));
	unsigned currentState = ScorchedServer::instance()->getServerState().getState();
	bool started = (currentState > ServerState::ServerMatchCountDownState);

	int compplayers = 0;
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator tanksitor;
	for (tanksitor = tanks.begin();
		tanksitor != tanks.end();
		++tanksitor)
	{
		Tank *tank = (*tanksitor).second;
		if (tank->getTankAI()) compplayers++;
	}

	reply.push_back(addTag("gametype", type));
	reply.push_back(addTag("state", (started?"Started":"Waiting")));
	reply.push_back(addTag("servername", serverName));
	reply.push_back(addTag("fullversion", version));
	reply.push_back(addTag("version", S3D::ScorchedVersion));
	reply.push_back(addTag("protocolversion", S3D::ScorchedProtocolVersion));
	reply.push_back(addTag("mod", 
		ScorchedServer::instance()->getOptionsGame().getMod()));
	reply.push_back(addTag("password", 
		ScorchedServer::instance()->getOptionsGame().getServerPassword()[0]?"On":"Off"));
	reply.push_back(addTag("noplayers", players));
	reply.push_back(addTag("maxplayers", maxplayers));
	reply.push_back(addTag("compplayers", S3D::formatStringBuffer("%i", compplayers)));
	reply.push_back(addTag("stats", (stats?"yes":"no")));
	reply.push_back(addTag("round", S3D::formatStringBuffer("%i/%i",
		ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo(),
		ScorchedServer::instance()->getOptionsGame().getNoRounds())));
	reply.push_back(addTag("os", S3D::getOSDesc()));
}

void ServerBrowserInfo::processInfoMessage(std::list<std::string> &reply)
{
	// Add all of the other tank options
	// Currently nothing on the client uses this info
	std::list<OptionEntry *> &options = ScorchedServer::instance()->getOptionsGame().
		getMainOptions().getOptions();
	std::list<OptionEntry *>::iterator optionItor;
	for (optionItor = options.begin();
		optionItor != options.end();
		++optionItor)
	{
		OptionEntry *entry = (*optionItor);
		if (!(entry->getData() & OptionEntry::DataProtected) &&
			!(entry->getData() & OptionEntry::DataDepricated))
		{
			reply.push_back(
				addTag(entry->getName(), entry->getValueAsString()));
		}
	}	
}

void ServerBrowserInfo::processPlayerMessage(std::list<std::string> &reply)
{
	// Add all of the player information
	char tmp[128];
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	int i=0;
	for (tankItor =  tanks.begin();
		tankItor != tanks.end();
		++tankItor, i++)
	{
		Tank *tank = (*tankItor).second;

		snprintf(tmp, 128, "pn%i", i);
		reply.push_back(addTag(tmp, 
			LangStringUtil::convertFromLang(tank->getTargetName())));

		snprintf(tmp, 128, "ps%i", i);
		reply.push_back(addTag(tmp, tank->getScore().getScoreString()));

		snprintf(tmp, 128, "pt%i", i);
		reply.push_back(addTag(tmp, tank->getScore().getTimePlayedString()));

		snprintf(tmp, 128, "pm%i", i);
		reply.push_back(addTag(tmp, TankColorGenerator::getTeamName(tank->getTeam()))); 

		snprintf(tmp, 128, "pa%i", i);
		reply.push_back(addTag(tmp, (tank->getTankAI()?"N":"Y")));

		snprintf(tmp, 128, "pr%i", i);
		reply.push_back(addTag(tmp, S3D::formatStringBuffer("%i", tank->getScore().getRank())));
	}

}

std::string ServerBrowserInfo::addTag(const std::string &name, const std::string &value)
{
	std::string content(value), result;
	XMLNode::removeSpecialChars(content, result);

	return S3D::formatStringBuffer("%s='%s' ", name.c_str(), result.c_str());
}
