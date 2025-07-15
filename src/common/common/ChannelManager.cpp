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

#include <common/ChannelManager.h>
#include <engine/ScorchedContext.h>
#ifndef S3D_SERVER
	#include <client/ClientChannelManager.h>
#else
	#include <common/Logger.h>
#endif

ChannelManager::ChannelManager()
{
}

void ChannelManager::showText(ScorchedContext &context, const ChannelText &text)
{
#ifndef S3D_SERVER
	if (!context.getServerMode()) 
	{
		ClientChannelManager::instance()->showText(text);
	}
#else
	ChannelText &nonConst = (ChannelText &) text;
	Logger::log(LangStringUtil::convertFromLang(nonConst.getMessage()));
#endif
}
