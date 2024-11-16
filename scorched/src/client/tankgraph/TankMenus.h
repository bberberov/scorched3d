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

#if !defined(AFX_TankMenus_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)
#define AFX_TankMenus_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_

#include <GLEXT/GLMenuI.h>
#include <console/ConsoleRule.h>
#include <weapons/Accessory.h>
#include <common/FileLogger.h>
#include <vector>
#include <list>

class TankMenus
{
public:
	TankMenus();
	virtual ~TankMenus();

protected:
	FileLogger logger_;

	struct PlayerMenu : public GLMenuI
	{
		PlayerMenu();

		// Inherited from GLMenuI
		virtual void menuSelection(const char* menuName, 
			const int position, GLMenuItem &item);
		virtual bool getEnabled(const char* menuName);
	} playerMenu_;	

	struct AccessoryMenu : public GLMenuI
	{
		AccessoryMenu();

		// Inherited from GLMenuI
		virtual void menuSelection(const char* menuName, 
			const int position, GLMenuItem &item);
		virtual bool getMenuItems(const char* menuName, 
			std::list<GLMenuItem> &result);
		virtual bool getEnabled(const char* menuName);
	} accessoryMenu_;	

	void showTankDetails();
	void showTargetDetails();
	void showInventory();
	void showTextureDetails();
	void resetLandscape();
	void clearTracerLines();
	void groupInfo();
	void logToFile();
	void runScriptConsole(std::list<ConsoleRuleValue> list);

};

#endif // !defined(AFX_TankMenus_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)
