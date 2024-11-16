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

#include <dialogs/StartDialog.h>
#include <dialogs/ModSelectDialog.h>
#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/SaveSelectDialog.h>
#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/SettingsSubSelectDialog.h>
#include <dialogs/QuitDialog.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <engine/GameState.h>
#include <engine/MainLoop.h>
#include <GLW/GLWColors.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWWindowManager.h>
#include <lang/LangResource.h>

StartDialog *StartDialog::instance_ = 0;

StartDialog *StartDialog::instance()
{
	if (!instance_)
	{
		instance_ = new StartDialog;
	}
	return instance_;
}

StartDialog::StartDialog() : 
	GLWWindow("Start", 10.0f, 10.0f, 640.0f, 480.0f, eNoTitle | eHideName),
	selected_(-1)
{
	OptionDefinition defs[] = 
	{
		LANG_RESOURCE("TUTORIAL", "Tutorial"), "- Start the tutorial to learn how to play.", 50.0f, 60.0f, 0.0f,
		LANG_RESOURCE("PLAY_GAME", "Play Game"), "- Play a game against the computer or other local players.", 50.0f, 100.0f, 0.0f,
		LANG_RESOURCE("PLAY_ONLINE", "Play Online"), "- Play online game against other players, gain stats and win awards.", 50.0f, 140.0f, 0.0f,
		LANG_RESOURCE("LOAD SAVE", "Load Save"), "- Continue playing a saved game.", 50.0f, 180.0f, 0.0f,
		LANG_RESOURCE("HELP", "Help"), "- View the online help.", 50.0f, 250.0f, 0.0f,
		LANG_RESOURCE("DONATE", "Donate"), "- Show support for Scorched3D.", 50.0f, 290.0f, 0.0f,
		LANG_RESOURCE("QUIT", "Quit"), "- Exit the game.", 50.0f, 360.0f, 0.0f
	};
	for (int i=0; i<sizeof(defs) / sizeof(OptionDefinition); i++)
	{
		definitions_.push_back(defs[i]);
	}
}

StartDialog::~StartDialog()
{
}

void StartDialog::draw()
{
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	
	GLState state(GLState::DEPTH_OFF);

	selected_ = -1;
	float size = 18.0f;
	float smallSize = 14.0f;
	Vector white(0.8f, 0.8f, 0.8f);
	for (int i=0; i<(int) definitions_.size(); i++)
	{
		OptionDefinition &definition = definitions_[i];

		// Draw shadow
		GLWFont::instance()->getNormalShadowFont()->draw(
			GLWColors::black, size, 
			definition.x - 2, h_ - definition.y + 2, 0.0f,
			definition.option);

		// Check if option is selected
		Vector *color = &white;
		if (inBox(
			mouseX - GLWTranslate::getPosX(), 
			mouseY - GLWTranslate::getPosY(), 
			definition.x, (h_ - definition.y), 
			definition.width, 20.0f))
		{
			color = &GLWColors::white;
			selected_ = i;
		}

		// Draw Option
		GLWFont::instance()->getNormalFont()->draw(
			*color, size, 
			definition.x, h_ - definition.y, 0.0f,
			definition.option);
		definition.width = 
			GLWFont::instance()->getNormalFont()->getWidth(
			size, definition.option);

		// Draw Description
		if (selected_ == i)
		{
			GLWFont::instance()->getNormalShadowFont()->draw(
				GLWColors::black, smallSize, 
				definition.x + 200.0f - 1.5f, h_ - definition.y + 1.5f, 0.0f,
				definition.description);

			GLWFont::instance()->getNormalFont()->draw(
				*color, smallSize, 
				definition.x + 200.0f, h_ - definition.y, 0.0f,
				definition.description);
		}
	}
}

void StartDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (selected_ != -1)
	{
		skipRest = true;

		// Make sure all other options are hidden
		GLWWindowManager::instance()->hideWindow(
			ModSelectDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(
			ModSubSelectDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(
			SaveSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			NetworkSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			SettingsSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			SettingsSubSelectDialog::instance()->getId());	
	}

	switch (selected_)
	{
	case 0:
		{
		std::string targetFilePath = S3D::getDataFile("data/singletutorial.xml");
		ClientParams::instance()->reset();
		ClientParams::instance()->setClientFile(targetFilePath.c_str());
		ClientMain::startClient();
		}
		break;
	case 1:
		GLWWindowManager::instance()->showWindow(
			ModSelectDialog::instance()->getId());
		break;
	case 2:
		GLWWindowManager::instance()->showWindow(
			NetworkSelectDialog::instance()->getId());
		break;
	case 3:
		GLWWindowManager::instance()->showWindow(
			SaveSelectDialog::instance()->getId());
		break;
	case 4:
		{
		S3D::showURL("http://www.scorched3d.co.uk/wiki");
		}
		break;
	case 5:
		{
		const char *exec = 
			"\"https://www.paypal.com/xclick/business=donations%40"
			"scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP\"";
		S3D::showURL(exec);
		}
		break;
	case 6:
		GLWWindowManager::instance()->showWindow(
			QuitDialog::instance()->getId());
		break;
	}
}
