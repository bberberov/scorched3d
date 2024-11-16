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

#if !defined(AFX_TankKeyboardControlUtil_H__F71C91A5_B74D_4109_8F7B_ACD471025C28__INCLUDED_)
#define AFX_TankKeyboardControlUtil_H__F71C91A5_B74D_4109_8F7B_ACD471025C28__INCLUDED_

class Tank;
class VirtualSoundSource;
class TankKeyboardControlUtil
{
public:
	static void keyboardCheck(Tank *tank,
		const unsigned state, 
		float frameTime, char *buffer, unsigned int keyState);
	static void endPlayMove(Tank *tank);

	// Tank move methods and tank defense methods
	//
	// The tankAI and its derived classes call these methods to perform
	// the actions associated with the method name.
	// Humans can only be on a client machine so we
	// don't need to check if we are on the server etc..
	static void fireShot(Tank *tank);
	static void skipShot(Tank *tank);
	static void resign(Tank *tank);
	static void parachutesUpDown(unsigned int playerId, unsigned int paraId=0);
	static void shieldsUpDown(unsigned int playerId, unsigned int shieldId=0);
	static void useBattery(unsigned int playerId, unsigned int batteryId);

protected:
	static void movePower(Tank *tank, char *buffer, unsigned int keyState, float frameTime);
	static void moveUpDown(Tank *tank, char *buffer, unsigned int keyState, float frameTime);
	static void moveLeftRight(Tank *tank, char *buffer, unsigned int keyState, float frameTime);
	static void leftRightHUD(Tank *tank);
	static void upDownHUD(Tank *tank);
	static void powerHUD(Tank *tank);
	static void autoAim(Tank *tank);
	static void prevWeapon(Tank *tank);
	static void nextWeapon(Tank *tank);

	static VirtualSoundSource *elevateSound_;
	static VirtualSoundSource *rotateSound_;
	static VirtualSoundSource *startSound_;
	static VirtualSoundSource *powerSound_;

private:
	TankKeyboardControlUtil();
	virtual ~TankKeyboardControlUtil();
};

#endif // !defined(AFX_TankKeyboardControlUtil_H__F71C91A5_B74D_4109_8F7B_ACD471025C28__INCLUDED_)
