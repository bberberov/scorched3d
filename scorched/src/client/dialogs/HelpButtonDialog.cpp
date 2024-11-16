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

#include <stdio.h>
#include <graph/FrameTimer.h>
#include <graph/OptionsDisplay.h>
#include <graph/MainCamera.h>
#include <graph/ParticleEngine.h>
#include <dialogs/MainMenuDialog.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/SoundDialog.h>
#include <common/Defines.h>
#include <sound/Sound.h>
#include <GLEXT/GLInfo.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLMenu.h>
#include <client/ScorchedClient.h>
#include <client/ClientChannelManager.h>
#include <client/ClientSimulator.h>
#include <tankgraph/RenderTargets.h>
#include <engine/ActionController.h>
#include <image/ImageFactory.h>
#include <landscape/Landscape.h>
#include <landscape/ShadowMap.h>
#include <land/VisibilityPatchGrid.h>
#include <GLW/GLWWindowManager.h>

HelpButtonDialog *HelpButtonDialog::instance_ = 0;

HelpButtonDialog *HelpButtonDialog::instance()
{
	if (!instance_)
	{
		instance_ = new HelpButtonDialog();
	}
	return instance_;
}

HelpButtonDialog::HelpButtonDialog() : 
	performanceMenu_(), volumeMenu_(), helpMenu_()
{
}

HelpButtonDialog::~HelpButtonDialog()
{
}

HelpButtonDialog::HelpMenu::HelpMenu()
{
	Image *map = new Image(
		ImageFactory::loadImage(
		S3D::eDataLocation,
			"data/images/help.bmp",
			"data/images/helpa.bmp",
			false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu(LANG_RESOURCE("HELP", "Help"), 
			"Help", 
			LANG_RESOURCE("HELP_MENU", "Launch an external web browser containing the\n"
			"Scorched3D online help."),
			32.0f, 0, this, map,
			GLMenu::eMenuAlignRight);
}

bool HelpButtonDialog::HelpMenu::getMenuItems(const char* menuName, std::list<GLMenuItem> &result)
{
	result.push_back(GLMenuItem(LANG_RESOURCE("SHOW_ONLINE_HELP", "Show Online Help")));
	return true;
}

void HelpButtonDialog::HelpMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	S3D::showURL("http://www.scorched3d.co.uk/wiki");
}

HelpButtonDialog::VolumeMenu::VolumeMenu()
{
	Image *map = new Image(ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/sound.bmp",
		"data/images/sounda.bmp",
		false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu(LANG_RESOURCE("VOLUME", "Volume"), 
			"Volume",
			LANG_RESOURCE("VOLUME_MENU", "Change the sound and volume settings"),
			32.0f, 0, this, map,
			GLMenu::eMenuAlignRight);
}

bool HelpButtonDialog::VolumeMenu::menuOpened(const char* menuName)
{
	GLWWindowManager::instance()->showWindow(
		SoundDialog::instance()->getId());
	return false;
}

HelpButtonDialog::PerformanceMenu::PerformanceMenu()
{
	Image *map = new Image(ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/perf.bmp",
		"data/images/perfa.bmp",
		false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu(LANG_RESOURCE("PERFORMANCE", "Performance"),
			"Performance", 
			LANG_STRING(""),
			32.0f, 0, this, map,
			GLMenu::eMenuAlignRight);
}

bool HelpButtonDialog::PerformanceMenu::getMenuItems(const char* menuName, std::list<GLMenuItem> &result)
{
	return true;
}

void HelpButtonDialog::PerformanceMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
}

LangStringStorage *HelpButtonDialog::PerformanceMenu::getMenuToolTip(const char* menuName)
{
	static LangString result;

	unsigned int pOnScreen = 
		ScorchedClient::instance()->
			getParticleEngine().getParticlesOnScreen() +
		MainCamera::instance()->getTarget().
			getPrecipitationEngine().getParticlesOnScreen();

	result = LANG_STRING(S3D::formatStringBuffer(
		"%.2f Frames Per Second\n"
		"  %i Triangles Drawn\n"
		"  %i Particles Drawn\n"
		"  %i %i %i %i Land, Water, Roof, Total Patches\n"
		"  %i Trees Drawn\n"
		"  %i Targets Drawn\n"
		"  %i Playing Sound Channels\n"
		"  %u Shadows Drawn\n"
		"  %u OpenGL State Changes\n"
		"  %u OpenGL Texture Changes\n"
		"%.2f Server Ahead By\n"
		"  %.2f Server Round Trip Time\n"
		"  %.2f Server Simulation Step\n"
		"  %.2f%% Server Choke\n", 

		FrameTimer::instance()->getFPS(),
		FrameTimer::instance()->getLastTris(),
		pOnScreen,
		VisibilityPatchGrid::instance()->getVisibleLandPatchesCount(),
		VisibilityPatchGrid::instance()->getVisibleWaterPatchesCount(),
		VisibilityPatchGrid::instance()->getVisibleRoofPatchesCount(),
		VisibilityPatchGrid::instance()->getPatchesVisitedCount(),
		RenderTargets::instance()->getTreesDrawn(),
		RenderTargets::instance()->getTargetsDrawn(),
		Sound::instance()->getPlayingChannels(),
		Landscape::instance()->getShadowMap().getShadowCount(), 
		FrameTimer::instance()->getLastStateCount(),
		FrameTimer::instance()->getLastTextureSets(),
		ScorchedClient::instance()->getClientSimulator().getServerTimeDifference().asFloat(),
		ScorchedClient::instance()->getClientSimulator().getServerRoundTripTime().asFloat(),
		ScorchedClient::instance()->getClientSimulator().getServerStepTime().asFloat(),
		ScorchedClient::instance()->getClientSimulator().getServerChoke().asFloat()));

	return (LangStringStorage *) result.c_str();
}