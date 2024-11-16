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

#include <client/ClientReloadAdaptor.h>
#include <client/ScorchedClient.h>
#include <server/ScorchedServer.h>
#include <landscape/LandscapeSoundManager.h>
#include <landscape/LandscapeMusicManager.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <common/Logger.h>

ClientReloadAdaptor *ClientReloadAdaptor::instance_ = 0;

ClientReloadAdaptor *ClientReloadAdaptor::instance()
{
	if (!instance_)
	{
	  instance_ = new ClientReloadAdaptor();
	}

	return instance_;
}

ClientReloadAdaptor::ClientReloadAdaptor()
{
	//new ConsoleRuleMethodIAdapter<ClientReloadAdaptor>(
	//	this, &ClientReloadAdaptor::reloadLandscapeObjects, "ReloadLandscapeObjects");
	new ConsoleRuleMethodIAdapter<ClientReloadAdaptor>(
		this, &ClientReloadAdaptor::showLandscapeInfo, "ShowLandscapeInfo");
	new ConsoleRuleMethodIAdapter<ClientReloadAdaptor>(
		this, &ClientReloadAdaptor::showLandscapeAimInfo, "ShowLandscapeAimInfo");
}

ClientReloadAdaptor::~ClientReloadAdaptor()
{

}

void ClientReloadAdaptor::reloadLandscapeObjects()
{
	// Remove any old targets
	LandscapeDefinitionCache &serverDefinitions =
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions();
	LandscapeDefinitionCache &clientDefinitions =
		ScorchedClient::instance()->getLandscapeMaps().getDefinitions();

	// Re-read landscape defs
	ScorchedServer::instance()->getLandscapes().readLandscapeDefinitions();
	ScorchedClient::instance()->getLandscapes().readLandscapeDefinitions();
	serverDefinitions.setDefinition(ScorchedServer::instance()->getContext(),
		serverDefinitions.getDefinition());
	clientDefinitions.setDefinition(ScorchedClient::instance()->getContext(),
		serverDefinitions.getDefinition());

	// Re-generate landscape
	std::list<FixedVector> tankPositions;
	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), 
		serverDefinitions.getDefinition());
	ScorchedClient::instance()->getLandscapeMaps().generateMaps(
		ScorchedClient::instance()->getContext(),
		serverDefinitions.getDefinition());

	// Classes that store refs to defns and texs
	LandscapeSoundManager::instance()->addSounds();
	LandscapeMusicManager::instance()->addMusics();
}

void ClientReloadAdaptor::showLandscapeInfo()
{
	LandscapeDefinitionCache &serverDefinitions =
		ScorchedServer::instance()->getLandscapeMaps().getDefinitions();

	Logger::log(S3D::formatStringBuffer("Name : %s\nTex : %s\nDefn %s\n", 
		serverDefinitions.getDefinition().getName(),
		serverDefinitions.getDefinition().getTex(),
		serverDefinitions.getDefinition().getDefn()));
}

void ClientReloadAdaptor::showLandscapeAimInfo()
{
	if (!ScorchedClient::instance()->getOptionsGame().getDebugFeatures())
	{
		Logger::log("Debug features must be enabled to view aim position");
		return;
	}

	Logger::log(S3D::formatStringBuffer("Aim Position : %.4f,%.4f,%.4f\n", 
		TargetRendererImplTankAIM::aimPosition_[0],
		TargetRendererImplTankAIM::aimPosition_[1],
		TargetRendererImplTankAIM::aimPosition_[2]));
}
