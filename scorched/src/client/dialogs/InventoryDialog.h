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

#if !defined(AFX_InventoryDialog_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_)
#define AFX_InventoryDialog_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_

#include <map>
#include <vector>
#include <weapons/Weapon.h>
#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWTab.h>
#include <GLW/GLWDropDownText.h>

class Tank;
class InventoryDialog : public GLWWindow,
						public GLWButtonI,
						public GLWDropDownI
{
public:
	static InventoryDialog *instance();

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);
	virtual void windowInit(const unsigned state);

	// Inherited from GLWDropDownI
	virtual void select(unsigned int id, const int pos, GLWSelectorEntry value);

	// Inherited from GLWPanel through GLWPanel
	virtual void display();

protected:
	static InventoryDialog *instance_;

	unsigned int okId_;
	GLWTab *sellTab_;
	GLWPanel *topPanel_;
	GLWDropDownText *sortDropDown_;

	Tank *getCurrentTank();
	void setupWindow();
	void playerRefresh();
	void addPlayerName();
	void addPlayerWeapons();

private:
	InventoryDialog();
	virtual ~InventoryDialog();
};

#endif // !defined(AFX_InventoryDialog_H__4B5E93CF_1DE2_4979_A629_AEBD725ABE65__INCLUDED_)

