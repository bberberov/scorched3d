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

#if !defined(__INCLUDE_VirtualSoundSourceh_INCLUDE__)
#define __INCLUDE_VirtualSoundSourceh_INCLUDE__

#include <sound/VirtualSoundPriority.h>
#include <sound/SoundBuffer.h>
#include <common/Vector.h>

class PlayingSoundSource;
class VirtualSoundSource
{
public:
	VirtualSoundSource(
		unsigned int priority, bool looping, bool managed);
	virtual ~VirtualSoundSource();

	void play(SoundBuffer *buffer);
	void stop();

	bool getPlaying();
	bool getManaged() { return managed_; }
	bool getLooping() { return looping_; }
	bool getRelative() { return relative_; }
	Vector &getPosition() { return position_; }
	unsigned int getPriority() { return priority_; }
	SoundBuffer *getBuffer() { return buffer_; }
	PlayingSoundSource *getPlayingSource() { return playingSource_; }
	float getDistance() { return distance_; }
	float getGain() { return gain_; }

	void setRelative();
	void setPosition(Vector &position);
	void setVelocity(Vector &velocity);
	void setGain(float gain);
	void setReferenceDistance(float refDist);
	void setRolloff(float rolloff);

	// Internal
	void actualPlay();
	void setPlayingSource(PlayingSoundSource *s);
	void updateDistance(Vector &listener);

protected:
	unsigned int priority_;
	PlayingSoundSource *playingSource_;
	SoundBuffer *buffer_;
	Vector position_, velocity_;
	float distance_;
	float gain_, refDist_, rolloff_;
	bool relative_;
	bool looping_;
	bool managed_;
};

#endif // __INCLUDE_VirtualSoundSourceh_INCLUDE__
