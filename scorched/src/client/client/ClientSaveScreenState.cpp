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

#include <client/ClientSaveScreenState.h>
#include <dialogs/BackdropDialog.h>

ClientSaveScreenState *ClientSaveScreenState::instance_ = 0;

ClientSaveScreenState *ClientSaveScreenState::instance()
{
	if (!instance_)
	{
		instance_ = new ClientSaveScreenState;
	}
	return instance_;
}

ClientSaveScreenState::ClientSaveScreenState() :
	GameStateI("ClientSaveScreenState")
{
}

ClientSaveScreenState::~ClientSaveScreenState()
{
}

void ClientSaveScreenState::draw(const unsigned state)
{
	if (saveScreen_)
	{
		saveScreen_ = false;
		BackdropDialog::instance()->capture();
	}
}

