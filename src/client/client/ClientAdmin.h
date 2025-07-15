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

#if !defined(AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)
#define AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_

#include <console/ConsoleRuleMethodIAdapter.h>

class ClientAdmin
{
public:
	static ClientAdmin *instance();

protected:
	static ClientAdmin *instance_;

	void adminNoParams(std::vector<ConsoleRuleValue> &split, 
		unsigned int userData);
	void adminOneParam(std::vector<ConsoleRuleValue> &split, 
		unsigned int userData);
	void adminTwoParam(std::vector<ConsoleRuleValue> &split, 
		unsigned int userData);
	void adminHelp();

private:
	ClientAdmin();
	virtual ~ClientAdmin();

};

#endif // !defined(AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)

