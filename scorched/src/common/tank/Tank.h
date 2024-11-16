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

#if !defined(AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
#define AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_

#include <tanket/Tanket.h>

class TankScore;
class TankState;
class TankShotHistory;
class TankModelContainer;
class TankAvatar;
class TankCamera;
class TankViewPointsCollection;
class Tank : public Tanket
{
public:
	// Constructor for tank
	// The name and color are copied
	Tank(ScorchedContext &context, 
		unsigned int playerId, 
		unsigned int destinationId,
		const LangString &name, 
		Vector &color);
	virtual ~Tank();

	virtual TargetType getType() { return Target::TypeTank; }

	// Called when the state changes
	virtual void newMatch();
	virtual void newGame();
	virtual void clientNewGame();
	void rezTank();

	// Serialize the tank
	virtual bool writeMessage(NamedNetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

	// The base attributes of the tank
	virtual bool getVisible();
	virtual bool getAlive();
	virtual bool getPlaying();
	virtual bool isTarget() { return false; }

	const char *getUniqueId() { return uniqueId_.c_str(); }
	void setUniqueId(const char *id) { uniqueId_ = id; }
	const char *getSUI() { return SUI_.c_str(); }
	void setSUI(const char *SecID) { SUI_ = SecID; }
	const char *getHostDesc() { return hostDesc_.c_str(); }
	void setHostDesc(const char *id) { hostDesc_ = id; }
	Vector &getColor();
	void setColor(const Vector &color) { color_ = color; }
	unsigned int getIpAddress() { return ipAddress_; }
	void setIpAddress(unsigned int ipAddress) { ipAddress_ = ipAddress; }
	virtual Accessory *getDeathAction();

	// Other attributes
	TankScore &getScore() { return *score_; }
	TankShotHistory &getShotHistory() { return *shotHistory_; }
	TankState &getState() { return *state_; }
	TankAvatar &getAvatar() { return *avatar_; }
	TankCamera &getCamera() { return *camera_; }
	TankModelContainer &getModelContainer() { return *modelContainer_; }
	TankViewPointsCollection &getViewPoints() { return *viewPoints_; }

protected:
	ScorchedContext &context_;
	TankViewPointsCollection *viewPoints_;
	TankModelContainer *modelContainer_;
	TankScore *score_;
	TankShotHistory *shotHistory_;
	TankState *state_;
	TankAvatar *avatar_;
	TankCamera *camera_;
	Vector color_;
	std::string uniqueId_;
	std::string SUI_;
	std::string hostDesc_;
	unsigned int ipAddress_;

};

#endif // !defined(AFX_TANK_H__52F37177_46EA_49C8_9B58_E6C57ABDB78A__INCLUDED_)
