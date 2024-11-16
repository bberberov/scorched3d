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

#include <tank/TankModelContainer.h>
#include <tank/TankModelStore.h>
#include <tank/Tank.h>
#include <tanket/TanketType.h>
#ifndef S3D_SERVER
	#include <tankgraph/TargetRendererImplTank.h>
#endif
#include <engine/ScorchedContext.h>
#include <common/DefinesString.h>

TankModelContainer::TankModelContainer(ScorchedContext &context) :
	context_(context),
	tankModel_(0), tank_(0)
{
}

TankModelContainer::~TankModelContainer()
{
}

TankModel *TankModelContainer::getTankModel() 
{ 
	if (!tankModel_)
	{
		tankModel_ = context_.getTankModels().getModelByName(modelName_.c_str());
		if (!tankModel_)
		{
			tankModel_ = context_.getTankModels().getRandomModel(
				tank_->getTeam(), tank_->getDestinationId() == 0, tank_->getTanketType()->getName());
		}
	}

	return tankModel_; 
}

void TankModelContainer::setTankModelName(const char *modelName)
{
	modelName_ = modelName;
	tankModel_ = 0;

#ifndef S3D_SERVER
	if (tank_->getRenderer())
	{
		TargetRendererImplTank *renderer = (TargetRendererImplTank *)
			tank_->getRenderer();
		renderer->resetModel();
	}
#endif
}

bool TankModelContainer::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankModelContainer");

	buffer.addToBufferNamed("modelName", modelName_);
	return true;
}

bool TankModelContainer::readMessage(NetBufferReader &reader)
{
	std::string modelName;
	if (!reader.getFromBuffer(modelName)) return false;
	setTankModelName(modelName.c_str());
	return true;
}
