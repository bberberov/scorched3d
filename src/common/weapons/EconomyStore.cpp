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

#include <weapons/EconomyStore.h>
#include <server/ScorchedServer.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <stdlib.h>

EconomyStore::EconomyStore() : economy_(0)
{
}

EconomyStore::~EconomyStore()
{
	delete economy_;
	economy_ = 0;
}

void EconomyStore::loadEconomy()
{
	if (economy_)
	{
		economy_->savePrices();
		delete economy_;
	}

	economy_ = (Economy *) MetaClassRegistration::getNewClass(
		ScorchedServer::instance()->getOptionsGame().getEconomy());
	if (!economy_)
	{
		S3D::dialogExit("Failed to find an economy called \"%s\"",
			ScorchedServer::instance()->getOptionsGame().getEconomy());
	}
	economy_->loadPrices();
}

Economy *EconomyStore::getEconomy()
{ 
	if (economy_ == 0 ||
		0 != strcmp(ScorchedServer::instance()->getOptionsGame().getEconomy(),
		economy_->getClassName()))
	{
		Logger::log( "Loading new economy");
		loadEconomy();
	}

	return economy_; 
}
