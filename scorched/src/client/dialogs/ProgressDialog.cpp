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

#include <dialogs/ProgressDialog.h>
#include <dialogs/BackdropDialog.h>
#include <landscape/LandscapeMusicManager.h>
#include <sound/Sound.h>
#include <client/ScorchedClient.h>
#include <client/ClientChannelManager.h>
#include <graph/Main2DCamera.h>
#include <client/ClientMain.h>
#include <client/ClientState.h>
#include <client/ClientParams.h>
#include <client/ClientProcessingLoop.h>
#include <server/ScorchedServer.h>
#include <engine/MainLoop.h>
#include <common/OptionsScorched.h>
#include <common/Clock.h>
#include <common/Defines.h>
#include <common/ChannelText.h>
#include <lang/LangResource.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWColors.h>
#include <lang/LangResource.h>
#include <image/ImagePngFactory.h>
#include <image/ImageFactory.h>
#include <math.h>
#include <string.h>

ProgressDialog *ProgressDialog::instance_ = 0;

ProgressDialog *ProgressDialog::instance()
{
	if (!instance_) instance_ = new ProgressDialog();
	return instance_;
}

ProgressDialog::ProgressDialog() : 
	GLWWindow("Progress", 10.0f, 10.0f, 500.0f, 80.0f, eTransparent | eNoTitle | eHideName, ""),
	progressPercentage_(0)
{
	setUser(this);
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::operationChange(const LangString &op)
{
	progressChange(op, 0.0f);
}

void ProgressDialog::progressChange(const LangString &op, const float percentage)
{
	progressText_ = op;
	progressPercentage_ = percentage;
}

void ProgressDialog::changeTip()
{
	tips_.getLines().clear();
	tips_.readFile(S3D::getDataFile("data/tips.txt"));
	needsCentered();

	std::string tip = tips_.getLines()[rand() % tips_.getLines().size()].c_str();
	char *nl = (char *) strchr(tip.c_str(), ':');
	if (nl) *nl = ' ';

	ChannelText text("announce", LANG_RESOURCE_1("TIP_ENTRY", "[t:Tip:] {0}", tip));
	ClientChannelManager::instance()->showText(text);
}

void ProgressDialog::setIcon(Image image)
{
	Image newMap = image.createResize(64, 64);
	icon_.create(newMap);
}

void ProgressDialog::drawRules(ScorchedContext &context)
{
	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	LANG_RESOURCE_VAR(SINGLE_PLAYER_GAME, "SINGLE_PLAYER_GAME", "Single Player Game");
	LangString SERVER_NAME = LANG_STRING(context.getOptionsGame().getServerName());

	LangString *text = &SINGLE_PLAYER_GAME;
	if (ClientParams::instance()->getConnectedToServer())
	{
		text = &SERVER_NAME;
	}

	Vector offWhite(0.9f, 0.9f, 1.0f);
	GLWFont::instance()->getGameShadowFont()->draw(
		GLWColors::black,
			20,
			x_ + 13.0f - 2.0f, y_ + h_ - 32.0f + 2.0f, 0.0f,
			*text);
	GLWFont::instance()->getGameFont()->draw(
			offWhite,
			20,
			x_ + 13.0f, y_ + h_ - 32.0f, 0.0f,
			*text);

	float top = y_ + 225.0f;
	float left = x_ + 22.0f;
	Vector yellow(1.0f, 1.0f, 1.0f); // Hmm, thats not yellow

	const char *type = "Annihilate free for all";
	if (context.getOptionsGame().getTeams() > 1) type = "Annihilate opposing team(s)";

	LANG_RESOURCE_VAR_1(TYPE_LABEL, "TYPE_LABEL", "Type : {0}", type);
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 45.0f, 0.0f,
		TYPE_LABEL);

	LANG_RESOURCE_VAR_1(MOD_LABEL, "MOD_LABEL", "Mod : {0}", 
		context.getOptionsGame().getMod());
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 75.0f, 0.0f,
		MOD_LABEL);

	LANG_RESOURCE_VAR_1(GAME_TYPE_LABEL, "GAME_TYPE_LABEL", "Game type : {0}", 
		context.getOptionsGame().getTurnType().getValueAsString());
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 90.0f, 0.0f,
		GAME_TYPE_LABEL);

	LANG_RESOURCE_VAR_1(TEAMS_LABEL, "TEAMS_LABEL", "Teams : {0}", 
		S3D::formatStringBuffer("%i", context.getOptionsGame().getTeams()));
	LANG_RESOURCE_VAR(TEAMS_NONE, "TEAMS_NONE", "Teams : None");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 105.0f, 0.0f,
		(context.getOptionsGame().getTeams() > 1)?TEAMS_LABEL:TEAMS_NONE);

	LANG_RESOURCE_VAR_1(SHOT_TIME_LABEL, "SHOT_TIME_LABEL", "Shot Time : {0}", 
		S3D::formatStringBuffer("%i", context.getOptionsGame().getShotTime()));
	LANG_RESOURCE_VAR(SHOT_TIME_UNLIMITED, "SHOT_TIME_UNLIMITED", "Shot time : Unlimited");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 135.0f, 0.0f,
		(context.getOptionsGame().getShotTime() > 0)?SHOT_TIME_LABEL:SHOT_TIME_UNLIMITED);

	LANG_RESOURCE_VAR_1(BUYING_TIME_LABEL, "BUYING_TIME_LABEL", "Buying Time : {0}", 
		S3D::formatStringBuffer("%i", context.getOptionsGame().getBuyingTime()));
	LANG_RESOURCE_VAR(BUYING_TIME_UNLIMITED, "BUYING_TIME_UNLIMITED", "Buying time : Unlimited");
	GLWFont::instance()->getGameFont()->draw(
		yellow,
		12,
		left, top - 150.0f, 0.0f,
		(context.getOptionsGame().getBuyingTime() > 0)?BUYING_TIME_LABEL:BUYING_TIME_UNLIMITED);

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex2f(x_ + 10.0f, y_ + 55.0f);
		glVertex2f(x_ + 490.0f, y_ + 55.0f);
	glEnd();
}

void ProgressDialog::draw()
{
	ScorchedContext *rulesContext = 0;
	unsigned int state = ScorchedClient::instance()->getGameState().getState();
	if (state >= ClientState::StateLoadFiles)
	{
		rulesContext = ScorchedClient::instance();
	}
	else if (ScorchedServer::instance() &&
		ScorchedServer::instance()->getNetInterfaceValid())
	{
		rulesContext = ScorchedServer::instance();
	}

	if (rulesContext)
	{
		needsCentered();
		h_ = 240.0f;
	}
	else
	{
		needsCentered();
		h_ = 55.0f;
	}

	GLWWindow::draw();

	if (rulesContext)
	{
		drawRules(*rulesContext);
	}

	{
		GLState state(GLState::DEPTH_OFF | GLState::TEXTURE_ON | GLState::BLEND_ON);

		if (!icon_.textureValid())
		{
			Image image = ImageFactory::loadImage(S3D::eDataLocation, "data/images/tank2.bmp");
			setIcon(image);

			Image bar1 = ImageFactory::loadAlphaImage(S3D::eDataLocation, "data/images/bar1.png");
			Image bar2 = ImageFactory::loadAlphaImage(S3D::eDataLocation, "data/images/bar2.png");
			bar1_.create(bar1);
			bar2_.create(bar2);

			bar1_.draw(true);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

			bar2_.draw(true);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		}

		// Draw the progress bars
		glPushMatrix();
			glTranslatef(x_ + 10.0f, y_ - 4.0f, 0.0f);

			bar1_.draw();
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0.0f, 15.0f);
				glTexCoord2f(progressPercentage_ / 100.0f, 0.0f);
				glVertex2f(480.0f * progressPercentage_ / 100.0f, 15.0f);
				glTexCoord2f(progressPercentage_ / 100.0f, 1.0f);
				glVertex2f(480.0f * progressPercentage_ / 100.0f, 26.0f);
				glTexCoord2f(0.0, 1.0f);
				glVertex2f(0.0f, 26.0f);
			glEnd();
			bar2_.draw();
			glBegin(GL_QUADS);
				glTexCoord2f(progressPercentage_ / 100.0f, 0.0f);
				glVertex2f(480.0f * progressPercentage_ / 100.0f, 15.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(480.0f, 15.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(480.0f, 26.0f);
				glTexCoord2f(progressPercentage_ / 100.0f, 1.0f);
				glVertex2f(480.0f * progressPercentage_ / 100.0f, 26.0f);
			glEnd();
			
			// Draw the progress text
			GLWFont::instance()->getGameShadowFont()->drawWidth(380.0f, 
				GLWColors::black, 
				14.0f, 5.0f - 2.0f, 33.0f + 2.0f, 0.0f, 
				progressText_);

			Vector white(1.0f, 1.0f, 1.0f);
			GLWFont::instance()->getGameFont()->drawWidth(380.0f, 
				white, 
				14.0f, 5.0f, 33.0f, 0.0f, 
				progressText_);
		glPopMatrix();

		// Draw the icon
		/*
		icon_.draw();
		glPushMatrix();
		{
			glTranslatef(x_ + 12.0f, y_ + 12.0f, 0.0f);
			glColor3f(1.0f, 1.0f, 1.0f);

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(56.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(56.0f, 56.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(0.0f, 56.0f);
			glEnd();
		}
		{
			GLState state2(GLState::TEXTURE_OFF);
			glColor3f(0.0f, 0.0f, 0.0f);
			glLineWidth(2.0f);
			glBegin(GL_LINE_LOOP);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(0.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(56.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(56.0f, 56.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(0.0f, 56.0f);
			glEnd();
			glLineWidth(1.0f);
		}
		glPopMatrix();
		*/
	}
}

ProgressDialogSync *ProgressDialogSync::noevents_instance()
{
	static ProgressDialogSync *instance = new ProgressDialogSync(false);
	return instance;
}

ProgressDialogSync *ProgressDialogSync::events_instance()
{
	static ProgressDialogSync *instance = new ProgressDialogSync(true);
	return instance;
}

ProgressDialogSync::ProgressDialogSync(bool processEvents) :
	processEvents_(processEvents)
{
	setUser(this);
}

ProgressDialogSync::~ProgressDialogSync()
{
}

void ProgressDialogSync::operationChange(const LangString &op)
{
	progressChange(op, 0.0f);
}

void ProgressDialogSync::progressChange(const LangString &op, const float percentage)
{
	static Clock localTimer;
	static float timeDelay = 0.0f;
	static float timeDelay2 = 0.0f;
	float frameTime = localTimer.getTimeDifference();
	timeDelay += frameTime;
	timeDelay2 += frameTime;

	ClientMain::clientEventLoop(frameTime);	
	ClientProcessingLoop::instance()->process(frameTime, processEvents_);

	ProgressDialog::instance()->progressChange(op, percentage);

	if ((timeDelay > 0.25f) || 
		(percentage > 99.0f))
	{
		Main2DCamera::instance()->draw(0);

		unsigned int state = ScorchedClient::instance()->getGameState().getState();
		if (state >= ClientState::StateLoadFiles)
		{
			GLWWindowManager::instance()->simulate(state, MIN(0.25f, timeDelay));
			GLWWindowManager::instance()->draw(state);
		}
		else
		{
			BackdropDialog::instance()->draw();
			ProgressDialog::instance()->draw();
		}

		LandscapeMusicManager::instance()->simulate(state, frameTime);
		Sound::instance()->simulate(state, frameTime);

		ScorchedClient::instance()->getMainLoop().swapBuffers();

		timeDelay = 0.0f;
	}

	if (timeDelay2 > 5.0f)
	{
		SDL_Delay(50);
		timeDelay2 = 0.0f;
	}
}
