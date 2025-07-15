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

#if !defined(AFX_BuyAccessoryDialogTankInfo_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_)
#define AFX_BuyAccessoryDialogTankInfo_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_

#include <tanket/TanketAccessories.h>
#include <common/Vector.h>

class BuyAccessoryDialogTankInfo
{
public:
	static BuyAccessoryDialogTankInfo *instance();

	TanketAccessories tankAccessories;
	Vector tankColor;
	LangString tankName;
	int tankMoney;
	unsigned int tankId;
	unsigned int tankMoveId;

	void set();

protected:
	static BuyAccessoryDialogTankInfo *instance_;

	BuyAccessoryDialogTankInfo();
};

#endif // !defined(AFX_BuyAccessoryDialogTankInfo_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_)
