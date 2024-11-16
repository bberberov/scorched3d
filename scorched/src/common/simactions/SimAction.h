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

#if !defined(AFX_SIMACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
#define AFX_SIMACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_

#include <engine/MetaClass.h>
#include <engine/ScorchedContext.h>
#include <net/NetBuffer.h>

class SimAction : public MetaClass
{
public:
	SimAction();
	virtual ~SimAction();

	virtual bool invokeAction(ScorchedContext &context) = 0;
	virtual bool replayAction() { return true; }

	virtual bool writeMessage(NetBuffer &buffer) = 0;
	virtual bool readMessage(NetBufferReader &reader) = 0;
};

#endif // !defined(AFX_SIMACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
