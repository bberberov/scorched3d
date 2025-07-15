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

#include <tankai/TankAIStrings.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <engine/Simulator.h>

TankAIStrings::TankAIStrings() :
	playerNamesCounter_(0),
	aiPlayerNamesCounter_(0)
{
}

TankAIStrings::~TankAIStrings()
{
}

void TankAIStrings::load()
{
	// Check we have init correctly
	bool s1 = deathLines_.readFile(S3D::getModFile("data/talk/talk2.txt"));
	bool s2 = attackLines_.readFile(S3D::getModFile("data/talk/talk1.txt"));
	bool s3 = aiPlayerNames_.readFile(S3D::getModFile("data/ainames.txt"));
	bool s4 = playerNames_.readFile(S3D::getModFile("data/playernames.txt"));
	DIALOG_ASSERT(s1 && s2 && s3 && s4);
}

const char *TankAIStrings::getPlayerName()
{
	const char *playerName = 
		playerNames_.getLines()[playerNamesCounter_++ % playerNames_.getLines().size()].c_str();
	return playerName;
}

const char *TankAIStrings::getAIPlayerName(ScorchedContext &context)
{
	if (context.getOptionsGame().getRandomizeBotNames())
	{
		aiPlayerNamesCounter_ = (int) rand();
	}

	const char *playerName = 
		aiPlayerNames_.getLines()[aiPlayerNamesCounter_++ % aiPlayerNames_.getLines().size()].c_str();
	return playerName;
}

const char *TankAIStrings::getDeathLine(ScorchedContext &context)
{
	RandomGenerator &generator = context.getSimulator().getRandomGenerator();
	const char *deathLine = 0;
	fixed percentage = 
		fixed(context.getOptionsGame().getComputersDeathTalk());
	fixed talkPer = generator.getRandFixed("TankAIStrings::getDeathLine1") * 100;
	if (talkPer < percentage)
	{
		deathLine = deathLines_.getLines()
			[generator.getRandUInt("TankAIStrings::getDeathLine2") % deathLines_.getLines().size()].c_str();
	}

	return deathLine;
}

const char *TankAIStrings::getAttackLine(ScorchedContext &context)
{
	RandomGenerator &generator = context.getSimulator().getRandomGenerator();
	const char *attackLine = 0;
	fixed percentage = 
		fixed(context.getOptionsGame().getComputersAttackTalk());
	fixed talkPer = generator.getRandFixed("TankAIStrings::getAttackLine1") * 100;
	if (talkPer < percentage)
	{
		attackLine = attackLines_.getLines()
			[generator.getRandUInt("TankAIStrings::getAttackLine2") % attackLines_.getLines().size()].c_str();
	}
	
	return attackLine;
}
