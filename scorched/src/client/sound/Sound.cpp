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

#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <common/Logger.h>
#include <sound/Sound.h>
#include <sound/SoundBufferFactory.h>
#include <sound/PlayingSoundSource.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <graph/OptionsDisplay.h>
#ifdef __DARWIN__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif
#include <algorithm>

Sound *Sound::instance_ = 0;

Sound *Sound::instance()
{
	if (!instance_)
	{
		instance_ = new Sound;
	}

	return instance_;
}

Sound::Sound() : 
	init_(false), totalTime_(0.0f), playSounds_(true),
	GameStateI("Sound")
{
	new ConsoleRuleMethodIAdapter<Sound>(
		this, &Sound::showSoundBuffers, "SoundBuffers");
	new ConsoleRuleMethodIAdapterEx<Sound>(
		this, &Sound::soundPlay, "SoundPlay", 
		ConsoleUtil::formParams(
		ConsoleRuleParam("filename", ConsoleRuleTypeString)));
}

Sound::~Sound()
{

}

void Sound::destroy()
{
	if (init_)
	{
		{
			SourceList::iterator itor;
			for (itor = totalSources_.begin();
				itor != totalSources_.end();
				++itor)
			{
				SoundSource *source = (*itor);
				delete source;
			}
			totalSources_.clear();
			availableSources_.clear();
			managedSources_.clear();
			playingSources_.clear();
		}
		{
			BufferMap::iterator itor;
			for (itor = bufferMap_.begin();
				itor != bufferMap_.end();
				++itor)
			{
				SoundBuffer *buffer = (*itor).second;
				delete buffer;
			}
			bufferMap_.clear();
		}

		ALCcontext *context = alcGetCurrentContext();
		ALCdevice *device = alcGetContextsDevice(context);
        alcDestroyContext(context);
        alcCloseDevice(device);
	}
	init_ = false;
}

static char *checkString(char *x) 
{
	return (char *)(x?x:"null");
}

bool Sound::init(int channels)
{
	ALCdevice *soundDevice = alcOpenDevice(0);
	if (!soundDevice)
	{
		S3D::dialogMessage("Scorched3D", "Failed to open sound device");
		return false;
	}

	// Setting the frequency seems to cause screeching and
	// loss of stereo under linux!!
	/*int attrlist[] = 
	{ 
		ALC_FREQUENCY, 11025,
		ALC_INVALID
	};*/
	ALCcontext *soundContext = alcCreateContext(soundDevice, 0);
	if (!soundContext)
	{
		S3D::dialogMessage("Scorched3D", "Failed to create sound context");
		return false;
	}

	alcMakeContextCurrent(soundContext); 
	alDistanceModel(AL_INVERSE_DISTANCE);

	Logger::log(S3D::formatStringBuffer("AL_VENDOR:%s",
		checkString((char *) alGetString(AL_VENDOR))));
	Logger::log(S3D::formatStringBuffer("AL_VERSION:%s",
		checkString((char *) alGetString(AL_VERSION))));
	Logger::log(S3D::formatStringBuffer("AL_RENDERER:%s",
		checkString((char *) alGetString(AL_RENDERER))));
	Logger::log(S3D::formatStringBuffer("AL_EXTENSIONS:%s",
		checkString((char *) alGetString(AL_EXTENSIONS))));
	Logger::log(S3D::formatStringBuffer("ALC_DEVICE_SPECIFIER:%s",
		checkString((char *) alcGetString(soundDevice, ALC_DEVICE_SPECIFIER))));

	// Create all sound channels
	for (int i=1; i<=OptionsDisplay::instance()->getSoundChannels(); i++)
	{
		SoundSource *source = new SoundSource;
		if (!source->create())
		{
			S3D::dialogMessage("Scorched3D", 
				S3D::formatStringBuffer("Failed to create sound channel number %i", i));
			return false;
		}
		totalSources_.push_back(source);
		availableSources_.push_back(source);
	}

	init_ = true;
	return init_;
}

void Sound::soundPlay(std::vector<ConsoleRuleValue> &values)
{
	ConsoleRuleValue &fileName = values[1];

	SoundBuffer *buffer = 
		fetchOrCreateBuffer(fileName.valueString);
	VirtualSoundSource *source = 
		new VirtualSoundSource(10000, false, true);
	source->setRelative();
	source->play(buffer);
}

void Sound::showSoundBuffers()
{
	// Show some debug of the current playing sounds
	int i = 1;
	Logger::log(S3D::formatStringBuffer("%i sounds playing, %i sources free",
		getPlayingChannels(),
		getAvailableChannels()));
	PlayingSourceList::iterator itor;
	for (itor = playingSources_.begin();
		itor != playingSources_.end();
		++itor, i++)
	{
		PlayingSoundSource *source = (*itor);
		if (source->getVirtualSource())
		{
			Logger::log(S3D::formatStringBuffer("%i - %u,%f - %s%s:%s",
				i, 
				source->getVirtualSource()->getPriority(),
				source->getVirtualSource()->getDistance(),
				(source->getStopped()?"Finished":(source->getVirtualSource()->getPlaying()?"Playing":"Stopped")),
				(source->getVirtualSource()->getLooping()?"(Looped)":""),
				source->getVirtualSource()->getBuffer()->getFileName()));
		}
		else
		{
			Logger::log(S3D::formatStringBuffer("%i - Pending Removal"));
		}
	}
}

void Sound::simulate(const unsigned state, float frameTime)
{
	// Simulate all the current sources
	// This is only applicable for streams
	PlayingSourceList::iterator playingitor;
	for (playingitor = playingSources_.begin();
		playingitor != playingSources_.end();
		++playingitor)
	{
		SoundSource *source = (*playingitor)->getActualSource();
		if (source && source->getPlaying())
		{
			source->simulate();
		}
	}

	// Check the buffers every so often
	totalTime_ += frameTime;
	if (totalTime_ < 0.2f) return;
	totalTime_ = 0.0f;

	updateSources();
}

static inline bool lt_virt(PlayingSoundSource *p2, PlayingSoundSource *p1)
{ 
	float dist1 = 0.0f;
	float dist2 = 0.0f;
	unsigned int priority1 = 0;
	unsigned int priority2 = 0;

    VirtualSoundSource *v1 = p1->getVirtualSource();
	VirtualSoundSource *v2 = p2->getVirtualSource();

	if (v1 && !p1->getStopped()) priority1 = v1->getPriority();
	if (v2 && !p2->getStopped()) priority2 = v2->getPriority();
	if (v1) dist1 = v1->getDistance();
	if (v2) dist2 = v2->getDistance();

	return (priority1 < priority2 ||
		(priority1 == priority2 && dist1 > dist2));
}

void Sound::addPlaying(VirtualSoundSource *virt)
{
	if (playSounds_)
	{
		// Add the new source
		PlayingSoundSource *source = new PlayingSoundSource(virt);
		playingSources_.push_back(source);
		virt->setPlayingSource(source); // Need to do this before updateSources
	}

	updateSources();
}

void Sound::updateSources()
{
	// Update all of the distances
	Vector listenerPosition = listener_.getPosition();
	PlayingSourceList::iterator fitor;
	for (fitor = playingSources_.begin();
		fitor != playingSources_.end();
		++fitor)
	{
		PlayingSoundSource *source = (*fitor);
		if (source->getVirtualSource())
		{
			source->getVirtualSource()->updateDistance(listenerPosition);
		}
	}

	// Sort the queue by priority and distance
	std::sort(playingSources_.begin(), playingSources_.end(), lt_virt); 

	// Start and stop the relevant sources
	int totalSources = (int) totalSources_.size();
	int totalPlaying = (int) playingSources_.size();
	int count = 0;
	PlayingSourceList::reverse_iterator ritor;
	for (ritor = playingSources_.rbegin();
		ritor != playingSources_.rend();
		++ritor, count++)
	{
		PlayingSoundSource *source = (*ritor);

		bool stopSource = false;

		// Check if we have been stopped
		if (source->getStopped())
		{
			stopSource = true;
		}
		// Check if we should be playing
		else if (totalPlaying - count <= totalSources)
		{
			if (source->getActualSource())
			{
				if (source->getActualSource()->getPlaying())
				{
					// It should be playing and is playing
				}
				else
				{
					// It should be playing, but its finished playing
					stopSource = true;
				}
			}
			else if (!source->getActualSource())
			{
				// Its not playing and should be playing
				DIALOG_ASSERT(!availableSources_.empty());
				source->setActualSource(availableSources_.back());
				availableSources_.pop_back();
				source->getVirtualSource()->actualPlay();
			}
		}
		// We should not be playing this one
		else
		{
			stopSource = true;
		}

		// We should not be playing this sound
		if (stopSource)
		{
			// We are currently playing
			if (source->getActualSource())
			{
				// Stop it
				source->getActualSource()->stop();
				availableSources_.push_back(source->getActualSource());
				source->setActualSource(0);
			}

			// If we are not looped so stop for good
			if (source->getVirtualSource())
			{
				if (!source->getVirtualSource()->getLooping())
				{
					source->setStopped(true);
				}
			}
		}
	}

	// Remove any finished sources
	while (!playingSources_.empty())
	{
		PlayingSoundSource *source = playingSources_.back();
		if (source->getStopped())
		{
			if (source->getVirtualSource())
			{
				source->getVirtualSource()->setPlayingSource(0);
			}

			DIALOG_ASSERT(!(source->getActualSource()));
			delete source;
			playingSources_.pop_back();
		}
		else break;
	}

	// Tidy any managed sources that have stopped playing
	// Managed sources are virtualsources that are not kept by the user
	// and should be deleted if they stop playing
	bool repeat = true;
	while (repeat)
	{
		repeat = false;
		VirtualSourceList::iterator manitor;
		for (manitor = managedSources_.begin();
			manitor != managedSources_.end();
			++manitor)
		{
			VirtualSoundSource *source = (*manitor);
			if (!source->getPlaying())
			{
				managedSources_.erase(manitor);
				delete source;
				repeat = true;
				break;
			}
		}
	}
}

void Sound::removePlaying(VirtualSoundSource *virt)
{
	if (virt->getPlayingSource())
	{
		virt->getPlayingSource()->setStopped(true);
		virt->getPlayingSource()->setVirtualSource(0);
	}
	virt->setPlayingSource(0);

	updateSources();
}

void Sound::addManaged(VirtualSoundSource *source)
{
	managedSources_.push_back(source);
}

int Sound::getAvailableChannels()
{
	return (int) availableSources_.size();
}

int Sound::getPlayingChannels()
{
	return (int) playingSources_.size();
}

SoundListener *Sound::getDefaultListener()
{
	return &listener_;
}

SoundBuffer *Sound::createBuffer(char *fileName)
{
	// Return a buffer full of sound :)
	SoundBuffer *buffer = SoundBufferFactory::createBuffer(
		(const char *) fileName);
	if (!buffer)
	{
		S3D::dialogExit("Failed to load sound",
			S3D::formatStringBuffer("\"%s\"", fileName));

		delete buffer;
		return 0;
	}
	return buffer;
}

SoundBuffer *Sound::fetchOrCreateBuffer(const std::string &filename)
{
	BufferMap::iterator itor = bufferMap_.find(filename);
	if (itor != bufferMap_.end())
	{
		return (*itor).second;
	}

	SoundBuffer *buffer = createBuffer((char *) filename.c_str());
	bufferMap_[filename] = buffer;
	return buffer;
}
