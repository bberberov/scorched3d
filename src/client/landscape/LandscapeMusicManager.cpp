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

#include <landscape/LandscapeMusicManager.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <graph/OptionsDisplay.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <common/Defines.h>
#include <sound/Sound.h>
#include <XML/XMLFile.h>

LandscapeMusicManager *LandscapeMusicManager::instance()
{
	static LandscapeMusicManager instance_;
	return &instance_;
}

LandscapeMusicManager::LandscapeMusicManager() : 
	GameStateI("LandscapeMusicManager")
{
	addMusics();
}

LandscapeMusicManager::~LandscapeMusicManager()
{
}

void LandscapeMusicManager::addMusics()
{
	stateMusic_.clear();
	
	// Read in the global music defaults
	// Do this first so that levels can override the music if required
	readGlobalMusicFile();

	// Load in the per level music
	LandscapeDefn *defn = 
		ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getDefn();
	LandscapeTex *tex = 
		ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();

	// Since we initialize before the landscapes are loaded these may be null
	if (tex) addMusic(tex->texDefn.includes);
	if (defn) addMusic(defn->texDefn.includes);
}

void LandscapeMusicManager::readGlobalMusicFile()
{
	std::string filePath = S3D::getModFile("data/music.xml");

	// Parse the XML file
	XMLFile file;
	if (!file.readFile(filePath))
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
			"ERROR: Failed to parse music file \"%s\"\n"
			"%s",
			filePath.c_str(),
			file.getParserError()));
	}

	// just return for an empty file
	if (!file.getRootNode()) return;

	XMLNode *musicNode = 0;
	while (file.getRootNode()->getNamedChild("music", musicNode, false))
	{
		LandscapeMusicType musicType;
		musicType.playstates.clear();
		if (!musicType.readXML(musicNode))
		{
			S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
				"ERROR: Failed to parse music file \"%s\"\n",
				filePath.c_str()));
		}
		addMusicType(&musicType);
	}
}

void LandscapeMusicManager::addMusic(std::vector<LandscapeInclude *> &musics)
{
	std::vector<LandscapeInclude *>::iterator includeitor;
	for (includeitor = musics.begin();
		includeitor != musics.end(); 
		++includeitor)
	{
		LandscapeInclude *include = (*includeitor);
		addMusicTypes(include->musics);
	}
}

void LandscapeMusicManager::addMusicTypes(std::vector<LandscapeMusicType *> &musics)
{
	std::vector<LandscapeMusicType *>::iterator itor;
	for (itor = musics.begin();
		itor != musics.end();
		++itor)
	{
		LandscapeMusicType *music = (*itor);
		addMusicType(music);
	}
}

void LandscapeMusicManager::addMusicType(LandscapeMusicType *music)
{
	std::vector<LandscapeMusicType::PlayState>::iterator stateitor;
	for (stateitor = music->playstates.begin();
		stateitor != music->playstates.end();
		++stateitor)
	{
		LandscapeMusicType::PlayState playState = (*stateitor);

		MusicStateEntry stateEntry;
		stateEntry.file = S3D::getModFile(music->file);
		stateEntry.gain = music->gain;
		if (stateMusic_.find(int(playState)) == stateMusic_.end())
		{
			stateMusic_.insert(std::pair<int, MusicStateEntry>(int(playState), stateEntry));
		}
		else
		{
			stateMusic_[int(playState)] = stateEntry;
		}
	}
}

void LandscapeMusicManager::simulate(const unsigned state, float simTime)
{
	// Find the playstate that matches the current client state
	LandscapeMusicType::PlayState playState = LandscapeMusicType::StateNone;
	switch (state)
	{
	case ClientState::StateOptions:
	case ClientState::StateLoadFiles:
	case ClientState::StateLoadLevel:
	case ClientState::StateConnect:
		playState = LandscapeMusicType::StateLoading;
		break;
	case ClientState::StateWaitNoLandscape:
	case ClientState::StateWait:
		playState = LandscapeMusicType::StateWait;
		break;
	case ClientState::StateBuyWeapons:
	case ClientState::StateAutoDefense:
		playState = LandscapeMusicType::StateBuying;
		break;
	case ClientState::StatePlaying:
		playState = LandscapeMusicType::StatePlaying;
		break;
	//case ClientState::StateShot:
	//	playState = LandscapeMusicType::StateShot;
	//	break;
	case ClientState::StateScore:
		playState = LandscapeMusicType::StateScore;
		break;
	default:
		// So I can put a breakpoint in!!
		playState = LandscapeMusicType::StateNone;
		break;
	}

	// Find which music entry we should be playing in this state
	MusicStateEntry *wantedEntry = 0;
	if (!OptionsDisplay::instance()->getNoMusic() &&
		OptionsDisplay::instance()->getMusicVolume() > 0)
	{
		std::map<int, MusicStateEntry>::iterator findItor =
			stateMusic_.find(int(playState));
		if (findItor != stateMusic_.end())
		{
			wantedEntry = &(*findItor).second;				
		}
	}

	// Find if we are currently playing this music
	bool found = false;
	std::list<MusicPlayingEntry *> remove;
	std::list<MusicPlayingEntry *>::iterator itor;
	for (itor = currentMusic_.begin();
		itor != currentMusic_.end();
		++itor)
	{
		MusicPlayingEntry *playingEntry = *itor;

		float wantedGain = 0;
		if (wantedEntry &&
			wantedEntry->file == playingEntry->file)
		{
			found = true;
			wantedGain = 
				float(OptionsDisplay::instance()->getMusicVolume()) / 128.0f *
				wantedEntry->gain;
		}
		else
		{
			wantedGain = 0;
			if (playingEntry->currentGain == 0)
			{
				remove.push_back(playingEntry);
			}
		}

		if (playingEntry->currentGain < wantedGain)
		{
			playingEntry->currentGain += simTime * 0.2f;
			playingEntry->currentGain = MIN(
				wantedGain, 
				playingEntry->currentGain);
			playingEntry->currentSource->setGain(playingEntry->currentGain);
		}
		else if (playingEntry->currentGain > wantedGain)
		{
			playingEntry->currentGain -= simTime * 0.2f;
			playingEntry->currentGain = MAX(
				wantedGain, 
				playingEntry->currentGain);
			playingEntry->currentSource->setGain(playingEntry->currentGain);
		}
	}

	// Stop the finished playing sound sources (if any)
	// Since the source is managed it will be automatically deleted once it has stopped
	for (itor = remove.begin();
		itor != remove.end();
		++itor)
	{
		MusicPlayingEntry *playingEntry = *itor;
		playingEntry->currentSource->stop();
		delete playingEntry->currentSource;
		currentMusic_.remove(playingEntry);
		delete playingEntry;
	}

	// Create the new sounds sources (if any)
	if (!found && wantedEntry)
	{
		// Play the next set of music
		// Load the next sound buffer
		SoundBuffer *buffer = 
			Sound::instance()->fetchOrCreateBuffer(
				wantedEntry->file.c_str());
		if (buffer)
		{
			// Start a new sound source
			VirtualSoundSource *currentSource = new VirtualSoundSource(
				VirtualSoundPriority::eMusic, // A high music priority
				true, // Its a looping sound
				false); // Its not automatically deleted when finished

			// Add to currently playing
			MusicPlayingEntry *playingEntry = new MusicPlayingEntry();
			playingEntry->currentGain = 0;
			playingEntry->currentSource = currentSource;
			playingEntry->file = wantedEntry->file;
			currentMusic_.push_back(playingEntry);

			// Play it
			playingEntry->currentSource->setGain(0);
			playingEntry->currentSource->setRelative();
			playingEntry->currentSource->play(buffer);
		}
	}
}
