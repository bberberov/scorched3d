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

#if !defined(AFX_FRAMETIMER_H__00CDF717_3628_42C1_B84E_6F9B15C4FC4E__INCLUDED_)
#define AFX_FRAMETIMER_H__00CDF717_3628_42C1_B84E_6F9B15C4FC4E__INCLUDED_

#include <engine/GameStateI.h>
#include <common/Clock.h>

class FrameTimer : public GameStateI
{
public:
	static FrameTimer *instance();

	float getFPS() { return fps_; }
	unsigned int getLastStateCount() { return lastStateCount_; }
	unsigned int getLastTris() { return lastTris_; }
	unsigned int getLastTextureSets() { return lastTextureSets_; }

	// Inherited from GameStateI
	virtual void draw(const unsigned state);
	virtual void simulate(const unsigned state, float frameTime);

protected:
	static FrameTimer *instance_;
	float totalTime_;
	int frameCount_;
	Clock frameClock_;
	unsigned int lastStateCount_, lastTris_, lastTextureSets_;
	float fps_;

private:
	FrameTimer();
	virtual ~FrameTimer();

};

#endif // !defined(AFX_FRAMETIMER_H__00CDF717_3628_42C1_B84E_6F9B15C4FC4E__INCLUDED_)
