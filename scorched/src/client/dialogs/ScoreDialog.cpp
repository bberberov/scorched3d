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

#include <dialogs/ScoreDialog.h>
#include <tank/TankSort.h>
#include <target/TargetContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tanket/TanketShotInfo.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <tankai/TankAI.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWColors.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <graph/ShotCountDown.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <engine/GameState.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <actions/ShowScoreAction.h>
#include <server/ScorchedServer.h>
#include <lang/LangResource.h>
#include <stdio.h>

static const float rankLeft = 15.0f;
static const float iconLeft = 5.0f;
static const float nameLeft = 45.0f;
static const float livesLeft = 230.0f;
static const float killsLeft = 255.0f;
static const float assistsLeft = 280.0f;
static const float winsLeft = 305.0f;
static const float moneyLeft = 325.0f;
static const float scoreLeft = 405.0f;
static const float statsLeft = 475.0f;
static const float pingLeft = 525.0f;
static const float readyLeft = 565.0f;
static const float lineSpacer = 10.0f;

ScoreDialog *ScoreDialog::instance_ = 0;
ScoreDialog *ScoreDialog::instance2_ = 0;

ScoreDialog *ScoreDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ScoreDialog;
	}

	return instance_;
}

ScoreDialog *ScoreDialog::instance2()
{
	if (!instance2_)
	{
		instance2_ = new ScoreDialog;
	}

	return instance2_;
}

ScoreDialog::ScoreDialog() :
	GLWWindow("Score", 10.0f, 10.0f, 575.0f, 310.0f, eTransparent |eSmallTitle,
		"Shows the current score for all players."),
	lastScoreValue_(0), lastMoneyValue_(0), lastNoPlayers_(0)
{

}

ScoreDialog::~ScoreDialog()
{

}

void ScoreDialog::display()
{
	GLWWindow::display();
	calculateScores();

	if (OptionsDisplay::instance()->getHideFinalScore() &&
		ScorchedClient::instance()->getGameState().getState() ==
		ClientState::StateScore)
	{
		GLWWindowManager::instance()->hideWindow(getId());
	}
}

void ScoreDialog::calculateScores()
{
	lastScoreValue_ = lastMoneyValue_ = 0;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		lastMoneyValue_ += tank->getScore().getMoney();
		lastScoreValue_ += tank->getScore().getScore();
	}

	lastNoPlayers_ = tanks.size();
	sortedTanks_.clear();
	TankSort::getSortedTanksIds(
		ScorchedClient::instance()->getContext(), sortedTanks_);
}

void ScoreDialog::windowInit(const unsigned state)
{
	needCentered_ = true;
}

void ScoreDialog::draw()
{
	Tank *currentTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	float h = sortedTanks_.size() * lineSpacer + 80.0f;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		h += (lineSpacer * 2.0f) * (ScorchedClient::instance()->getOptionsGame().getTeams() - 1) +
			lineSpacer * 2.0f;
	}
	setH(h);

	static size_t noTanks = 0;
	if (noTanks != sortedTanks_.size())
	{
		needCentered_ = true;
		noTanks = sortedTanks_.size();
	}

	GLWWindow::draw();
	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	bool finished = false;
	if (ScorchedClient::instance()->getGameState().getState() == 
		ClientState::StateScore)
	{
		finished = ShowScoreAction::getFinalScore();
	}
	bool buying = false;
	{
		std::map<unsigned int, Tank *> &playingTanks =
			ScorchedClient::instance()->getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = playingTanks.begin();
			itor != playingTanks.end();
			++itor)
		{
			if (itor->second->getState().getState() ==
				TankState::sBuying)
			{
				buying = true;
			}
		}
	}

	Vector white(0.9f, 0.9f, 1.0f);
	bool server = (ClientParams::instance()->getConnectedToServer());
	{
		LANG_RESOURCE_VAR(CURRENT_RANKINGS, "CURRENT_RANKINGS", "Current Rankings");
		LANG_RESOURCE_VAR(FINAL_RANKINGS, "FINAL_RANKINGS", "Final Rankings");
		LANG_RESOURCE_VAR(WAITING_FOR_PLAYERS, "WAITING_FOR_PLAYERS", "Waiting for players");
		LANG_RESOURCE_VAR(WAITING_TO_JOIN, "WAITING_TO_JOIN", "Waiting to join game");
		LangString SERVER_NAME = LANG_STRING(ScorchedClient::instance()->getOptionsGame().getServerName());

		LangString *text = &CURRENT_RANKINGS;
		if (finished)
		{
			text = &FINAL_RANKINGS;
		}
		else if (server)
		{
			text = &SERVER_NAME;
		}

		GLWFont::instance()->getGameShadowFont()->draw(
			GLWColors::black,
				20,
				x_ + 8.0f - 2.0f, y_ + h_ - 27.0f + 2.0f, 0.0f,
				*text);
		GLWFont::instance()->getGameFont()->draw(
				white,
				20,
				x_ + 8.0f, y_ + h_ - 27.0f, 0.0f,
				*text);
	}

	if (!finished) 
	{
		LangString buffer;

		LANG_RESOURCE_VAR_2(ROUND_OF, "ROUND_OF", "Round {0} of {1}",
			S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo()),
			S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getNoRounds()));
		buffer.append(ROUND_OF);

		if (ScorchedClient::instance()->getOptionsGame().getNoTurns() > 0)
		{
			LANG_RESOURCE_VAR_2(MOVE_OF, "MOVE_OF", ",Move {0} of {1}",
				S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsTransient().getCurrentTurnNo()),
				S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getNoTurns()));
			buffer.append(MOVE_OF);
		}

		float roundsWidth = GLWFont::instance()->getGameFont()->getWidth(
			10, buffer);
		GLWFont::instance()->getGameFont()->draw(
			white,
			10,
			x_ + 470 - roundsWidth, y_ + h_ - 15.0f, 0.0f,
			buffer);
	}
	if (!finished) 
	{
		std::string str;
		if (ShotCountDown::instance()->getRoundTime(str))
		{
			LangString buffer;
			LANG_RESOURCE_VAR_1(ROUND_TIME, "ROUND_TIME", "Round time {0}",
				S3D::formatStringBuffer("%s", str.c_str()));
			buffer.append(ROUND_TIME);

			float roundsWidth = GLWFont::instance()->getGameFont()->getWidth(
				10, buffer);
			GLWFont::instance()->getGameFont()->draw(
				white,
				10,
				x_ + 470 - roundsWidth, y_ + h_ - 30.0f, 0.0f,
				buffer);
		}
	}

	LANG_RESOURCE_VAR(SCORE_NAME, "SCORE_NAME", "Name");
	LANG_RESOURCE_VAR(SCORE_K, "SCORE_K", "K");
	LANG_RESOURCE_VAR(SCORE_W, "SCORE_W", "W");
	LANG_RESOURCE_VAR(SCORE_A, "SCORE_A", "A");
	LANG_RESOURCE_VAR(SCORE_SCORE, "SCORE_SCORE", "Score");
	LANG_RESOURCE_VAR(SCORE_RANK, "SCORE_RANK", "Rank");
	LANG_RESOURCE_VAR(SCORE_PING, "SCORE_PING", "Ping");
	LANG_RESOURCE_VAR(SCORE_L, "SCORE_L", "L");

	LANG_RESOURCE_VAR(SCORE_NAME_TOOLTIP_TITLE, "NAME", "Name");
	LANG_RESOURCE_VAR(SCORE_NAME_TOOLTIP, "NAME_TOOLTIP", "The name of the player");
	LANG_RESOURCE_VAR(SCORE_KILLS_TOOLTIP_TITLE, "KILLS", "Kills");
	LANG_RESOURCE_VAR_1(SCORE_KILLS_TOOLTIP, "KILL_TOOLTIP", 
		"The number of players this player has killed.\n{0} score awarded per kill.",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getScorePerKill()));
	LANG_RESOURCE_VAR(SCORE_WINS_TOOLTIP_TITLE, "WINS", "Wins");
	LANG_RESOURCE_VAR_1(SCORE_WINS_TOOLTIP, "WINS_TOOLTIP", 
		"The number of rounds this player has won.\n{0} score awarded per round won.",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getScoreWonForRound()));
	LANG_RESOURCE_VAR(SCORE_ASSISTS_TOOLTIP_TITLE, "ASSISTS", "Assists");
	LANG_RESOURCE_VAR_1(SCORE_ASSISTS_TOOLTIP, "ASSISTS_TOOLTIP", 
		"The number of kills this player has assisted in.\n{0} score awarded per assist.",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getScorePerAssist()));
	LANG_RESOURCE_VAR(SCORE_MONEY_TOOLTIP_TITLE, "MONEY", "Money");
	LANG_RESOURCE_VAR_1(SCORE_MONEY_TOOLTIP, "MONEY_TOOLTIP", 
		"The amount of money this player has.\n%{0} score awarded per dollar.",
		S3D::formatStringBuffer("%.1f", float(ScorchedClient::instance()->getOptionsGame().getScorePerMoney()) / 1000.0f));
	LANG_RESOURCE_VAR(SCORE_SCORE_TOOLTIP_TITLE, "SCORE", "Score");
	LANG_RESOURCE_VAR(SCORE_SCORE_TOOLTIP, "SCORE_TOOLTIP", 
		"The current score for this player.\nCalculated from the number of kills, wins, money and bonus score awards.");
	LANG_RESOURCE_VAR(SCORE_RANK_TOOLTIP_TITLE, "RANK", "Rank");
	LANG_RESOURCE_VAR(SCORE_RANK_TOOLTIP, "RANK_TOOLTIP", 
		"The current online ranking for this player.");
	LANG_RESOURCE_VAR(SCORE_PING_TOOLTIP_TITLE, "PING", "Ping");
	LANG_RESOURCE_VAR(SCORE_PING_TOOLTIP, "PING_TOOLTIP", 
		"The current online ping this player.");
	LANG_RESOURCE_VAR(SCORE_LIVES_TOOLTIP_TITLE, "LIVES", "Lives");
	LANG_RESOURCE_VAR_1(SCORE_LIVES_TOOLTIP, "LIVES_TOOLTIP", 
		"The current number of lives this player has left.\n{0} score awarded for each life remaining.",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getScoreWonForLives()));

	float y = lineSpacer + 10.0f;
	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + nameLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_NAME);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_NAME_TOOLTIP_TITLE, 
		SCORE_NAME_TOOLTIP,
		x_ + nameLeft, y_ + h_ - y - lineSpacer - 26.0f, 100.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + killsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_K);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_KILLS_TOOLTIP_TITLE, 
		SCORE_KILLS_TOOLTIP,
		x_ + killsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + winsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_W);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_WINS_TOOLTIP_TITLE, 
		SCORE_WINS_TOOLTIP,
		x_ + winsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + assistsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_A);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_ASSISTS_TOOLTIP_TITLE, 
		SCORE_ASSISTS_TOOLTIP,
		x_ + assistsLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + moneyLeft + 10, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"$");
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_MONEY_TOOLTIP_TITLE, 
		SCORE_MONEY_TOOLTIP,
		x_ + moneyLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + scoreLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_SCORE);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_SCORE_TOOLTIP_TITLE,
		SCORE_SCORE_TOOLTIP,
		x_ + scoreLeft, y_ + h_ - y - lineSpacer - 26.0f, 80.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + statsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_RANK);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_RANK_TOOLTIP_TITLE, 
		SCORE_RANK_TOOLTIP,
		x_ + statsLeft, y_ + h_ - y - lineSpacer - 26.0f, 40.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
			white,
			12,
			x_ + pingLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			SCORE_PING);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_PING_TOOLTIP_TITLE, 
		SCORE_PING_TOOLTIP,
		x_ + pingLeft, y_ + h_ - y - lineSpacer - 26.0f, 40.0f, 16.0f);

	GLWFont::instance()->getGameFont()->draw(
		white,
		12,
		x_ + livesLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
		SCORE_L);
	GLWToolTip::instance()->addToolTip(ToolTip::ToolTipHelp, 
		SCORE_LIVES_TOOLTIP_TITLE,
		SCORE_LIVES_TOOLTIP,
		x_ + livesLeft, y_ + h_ - y - lineSpacer - 26.0f, 20.0f, 16.0f);
	y+= lineSpacer + lineSpacer;

	int tmpLastScoreValue = 0;
	int tmpLastMoneyValue = 0;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		int winningTeam = TankSort::getWinningTeam(
			ScorchedClient::instance()->getContext());

		for (int i=0; i<ScorchedClient::instance()->getOptionsGame().getTeams(); i++)
		{
			bool someTeam = false;
			std::list<unsigned int>::iterator itor;
			for (itor = sortedTanks_.begin();
				itor != sortedTanks_.end();
				++itor)
			{
				unsigned int playerId = (*itor);
				Tank *current = ScorchedClient::instance()->getTargetContainer().getTankById(playerId);
				if (current && current->getTeam() == (i + 1) && current->getState().getTankPlaying()) 
				{
					someTeam = true;
					addLine(currentTank, current, y, (char *)((winningTeam==(i+1))?"1":"2"), finished, buying);
					
					tmpLastScoreValue += current->getScore().getScore();
					tmpLastMoneyValue += current->getScore().getMoney();
					y+= lineSpacer;
				}
			}
			if (someTeam)
			{
				addScoreLine(y, 
					TankColorGenerator::getTeamColor(i + 1),
					ScorchedClient::instance()->getContext().getTankTeamScore().getScore(i+1));
				y+= lineSpacer;
				y+= lineSpacer;
			}
		}
	}
	else
	{
		// No Team
		int rank = 1;
		char strrank[10];
		std::list<unsigned int>::iterator itor;
		for (itor = sortedTanks_.begin();
			itor != sortedTanks_.end();
			++itor, rank++)
		{
			unsigned int playerId = (*itor);
			Tank *current = ScorchedClient::instance()->getTargetContainer().getTankById(playerId);
			if (current && current->getState().getTankPlaying()) 
			{
				snprintf(strrank, 10, "%i", rank);

				addLine(currentTank, current, y, strrank, finished, buying);
				tmpLastScoreValue += current->getScore().getScore();
				tmpLastMoneyValue += current->getScore().getMoney();
				y+= lineSpacer;
			}
		}	
	}
	y+= lineSpacer / 1.5f;
	// Spectators
	std::list<unsigned int>::iterator itor;
	for (itor = sortedTanks_.begin();
		itor != sortedTanks_.end();
		++itor)
	{
		unsigned int playerId = (*itor);
		Tank *current = ScorchedClient::instance()->
			getTargetContainer().getTankById(playerId);
		if (current && !current->getState().getTankPlaying()) 
		{
			addLine(currentTank, current, y, " ", false, buying);
			y+= lineSpacer;
		}
	}	

	std::map<unsigned int, Tank *> &realTanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	if (tmpLastScoreValue != lastScoreValue_ ||
		tmpLastMoneyValue != lastMoneyValue_ ||
		realTanks.size() != lastNoPlayers_)
	{
		calculateScores();
	}
}

void ScoreDialog::addScoreLine(float y, Vector &color, int score)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 30.0f;

	// Print the name on the screen
	GLWFont::instance()->getGameFont()->draw(
		color,
		10,
		textX + scoreLeft, textY, 0.0f,
		S3D::formatStringBuffer("%i", score));
}

void ScoreDialog::addLine(Tank *currentPlayer, Tank *linePlayer, float y, char *rank, bool finished, bool buying)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 25.0f;

	TargetRendererImplTank *renderer = (TargetRendererImplTank *)
		linePlayer->getRenderer();
	if (renderer)
	{
		GLWToolTip::instance()->addToolTip(&renderer->getTips()->tankTip,
			textX,
			textY,
			x_ + w_ - textX, 
			10.0f);
	}

	// Print a highlight behind the current clients player
	if (linePlayer->getState().getTankPlaying() &&
		linePlayer->getDestinationId() == 
		ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId())
	{
		if (!currentPlayer || linePlayer == currentPlayer)
		{
			GLState state(GLState::BLEND_ON); 

			glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
			glBegin(GL_QUADS);
				glVertex2f(x_ + w_ - 3.0f, textY + lineSpacer - 1.0f);
				glVertex2f(x_ + 3.0f, textY + lineSpacer - 1.0f);
				glVertex2f(x_ + 3.0f, textY - 1.0f);
				glVertex2f(x_ + w_ - 3.0f, textY - 1.0f);
			glEnd();
		}
	}

	// Form the name
	static LangString name;
	name.clear();
	if (finished && ! ClientParams::instance()->getConnectedToServer())
	{
		name.append(LANG_STRING("("));
		Tank *serverTank = 
			ScorchedServer::instance()->getTargetContainer().getTankById(
			linePlayer->getPlayerId());
		TankAI *tankAI = serverTank->getTankAI();
		if (tankAI) name.append(LANG_STRING(tankAI->getName()));
		else name.append(LANG_RESOURCE("HUMAN", "Human"));
		name.append(LANG_STRING(") "));
	}
	else if (linePlayer->getState().getState() != TankState::sNormal)
	{
		name.append(LANG_STRING("("));
		name.append(linePlayer->getState().getSmallStateLangString());
		name.append(LANG_STRING(") "));
	}
	name.append(linePlayer->getTargetName());

	if (!linePlayer->getState().getTankPlaying())
	{
		if (name.size() > 50) name = name.substr(0, 50); // Limit length

		// Print the name on the screen
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			name);
	}
	else
	{
		GLState state(GLState::TEXTURE_ON);
		glColor3f(1.0f, 1.0f, 1.0f);
		linePlayer->getAvatar().getTexture()->draw();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(textX + iconLeft,
				textY + 2.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(textX + iconLeft + 8.0f, 
				textY + 2.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(textX + iconLeft + 8.0f,
				textY + 10.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(textX + iconLeft,
				textY + 10.0f);
        glEnd();

		// Print the name on the screen
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + rankLeft, textY, 0.0f,
			rank);
		GLWFont::instance()->getGameFont()->drawWidth(
			killsLeft - nameLeft,
			linePlayer->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			name);
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + killsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", linePlayer->getScore().getKills()));
		if (!buying ||
			currentPlayer == linePlayer ||
			(currentPlayer && linePlayer->getTeam() > 0 && 
			linePlayer->getTeam() == currentPlayer->getTeam()))
		{
			GLWFont::instance()->getGameFont()->draw(
				linePlayer->getColor(),
				10,
				textX + moneyLeft, textY, 0.0f,
				S3D::formatMoney(linePlayer->getScore().getMoney()));
		}
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + winsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", linePlayer->getScore().getWins()));
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + scoreLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", linePlayer->getScore().getScore()));
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + assistsLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", linePlayer->getScore().getAssists()));
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + readyLeft, textY, 0.0f,
			linePlayer->getShotInfo().getMoveId()!=0?"*":" ");
		if (linePlayer->getScore().getRank() >= 0)
		{
			GLWFont::instance()->getGameFont()->draw(
				linePlayer->getColor(),
				10,
				textX + statsLeft, textY, 0.0f,
				S3D::formatStringBuffer("%i", linePlayer->getScore().getRank()));
		}
		GLWFont::instance()->getGameFont()->draw(
			linePlayer->getColor(),
			10,
			textX + livesLeft, textY, 0.0f,
			S3D::formatStringBuffer("%i", linePlayer->getState().getLives()));
		if (linePlayer->getScore().getPing() >= 0)
		{
			GLWFont::instance()->getGameFont()->draw(
				linePlayer->getColor(),
				10,
				textX + pingLeft, textY, 0.0f,
				S3D::formatStringBuffer("%i", linePlayer->getScore().getPing()));
		}
	}
}
