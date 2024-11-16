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

#ifndef _SOUND_H_
#define _SOUND_H_

#include <map>
#include <vector>
#include <string>
#include <engine/GameStateI.h>
#include <sound/VirtualSoundSource.h>
#include <sound/SoundBuffer.h>
#include <sound/SoundListener.h>
#include <sound/SoundSource.h>
#include <console/ConsoleRule.h>

#define CACHE_SOUND(var, filename) 										\
		static SoundBuffer* var = Sound::instance()->fetchOrCreateBuffer(filename);

class PlayingSoundSource;
class Sound : public GameStateI
{
public:
	static Sound *instance();

	bool init(int channels);
	bool getInit() { return init_; }
	void destroy();

	void showSoundBuffers();
	void soundPlay(std::vector<ConsoleRuleValue> &values);

	SoundBuffer *fetchOrCreateBuffer(const std::string &filename);
	SoundListener *getDefaultListener();
	
	void addManaged(VirtualSoundSource *source);
	void addPlaying(VirtualSoundSource *source);
	void removePlaying(VirtualSoundSource *source);

	void setPlaySounds(bool playSounds) { playSounds_ = playSounds; }

	void simulate(const unsigned state, float simTime);
	int getAvailableChannels();
	int getPlayingChannels();

protected:
	static Sound *instance_;
	typedef std::map<std::string, SoundBuffer *> BufferMap;
	typedef std::vector<SoundSource *> SourceList;
	typedef std::vector<VirtualSoundSource *> VirtualSourceList;
	typedef std::vector<PlayingSoundSource *> PlayingSourceList;

	float totalTime_;
	BufferMap bufferMap_;
	SourceList totalSources_;
	SourceList availableSources_;
	SoundListener listener_;
	VirtualSourceList managedSources_;
	PlayingSourceList playingSources_;
	bool init_;
	bool playSounds_;

	void updateSources();
	SoundBuffer *createBuffer(char *fileName);

private:
	Sound();
	virtual ~Sound();

};

#endif /* _SOUND_H_ */
