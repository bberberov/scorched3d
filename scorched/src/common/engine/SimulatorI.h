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

#if !defined(AFX_SimulatorI_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_SimulatorI_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#include <simactions/SimAction.h>

class SimulatorI
{
public:
	virtual void actionInvoked(fixed actionTime, SimAction *action) = 0;
};

template<class T>
class SimulatorIAdapter : public SimulatorI
{
public:
	SimulatorIAdapter(T *inst, 
		void (T::*call)(fixed actionTime, SimAction *action)) :
		inst_(inst), call_(call)
	{
	};
	virtual ~SimulatorIAdapter()
	{
	};

	virtual void actionInvoked(fixed actionTime, SimAction *action)
	{
		return (inst_->*call_)(actionTime, action);
	}

protected:
	T *inst_;
	void (T::*call_)(fixed actionTime, SimAction *action);
};

#endif // !defined(AFX_SimulatorI_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
