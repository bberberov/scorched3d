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

#include <landscapedef/LandscapeSound.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <graph/MainCamera.h>
	#include <sound/Sound.h>
	#include <water/Water.h>
	#include <client/ScorchedClient.h>
#endif
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <target/Target.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetGroup.h>
#include <common/Defines.h>
#include <math.h>

LandscapeSoundPositionSetItem::LandscapeSoundPositionSetItem(
	ObjectGroupEntryReference *reference) : reference_(reference)
{
}

LandscapeSoundPositionSetItem::~LandscapeSoundPositionSetItem()
{
	delete reference_;
}

bool LandscapeSoundPositionSet::readXML(XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	if (!node->getNamedChild("maxsounds", maxsounds)) return false;
	
	return node->failChildren();
}

bool LandscapeSoundPositionSet::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	if (!data) return false;
	LandscapeSoundPositionSetItem *item = (LandscapeSoundPositionSetItem *) data;
	ObjectGroupEntryReference *reference = item->getReference();
	ObjectGroupEntry *entry = reference->getEntry();
	if (!entry) return false;

	Vector position = entry->getPosition().asVector();
	Vector velocity = entry->getVelocity().asVector();
	source->setPosition(position);
	source->setVelocity(velocity);
#endif

	return true;
}

int LandscapeSoundPositionSet::getInitCount()
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.c_str());
	if (objectGroup) return (MIN(maxsounds, objectGroup->getObjectCount()));	
#endif

	return 0;
}

LandscapeSoundItem *LandscapeSoundPositionSet::getInitData(int count)
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.c_str());
	if (objectGroup)
	{
		ObjectGroupEntry *entry = objectGroup->getObjectByPos(count % objectGroup->getObjectCount());
		if (entry)
		{
			ObjectGroupEntryReference *reference = new ObjectGroupEntryReference(entry);
			return new LandscapeSoundPositionSetItem(reference);
		}
	}
#endif

	return 0;
}

bool LandscapeSoundPositionGroup::readXML(XMLNode *node)
{
	if (!node->getNamedChild("falloff", falloff)) return false;
	if (!node->getNamedChild("name", name)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionGroup::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	ObjectGroup *objectGroup =
		ScorchedClient::instance()->getObjectGroups().getGroup(name.c_str());
	if (!objectGroup) return false;
	if (objectGroup->getObjectCount() <= 0) return false;

	Vector &cameraPos = 
		MainCamera::instance()->getCamera().getCurrentPos();

	float distance = 255.0f;
	std::map<unsigned int, Target *> collisionTargets;
	FixedVector fixedCameraPos = FixedVector::fromVector(cameraPos);
	ScorchedClient::instance()->getTargetSpace().getCollisionSet(
		fixedCameraPos, 5, collisionTargets, false);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		itor++)
	{
		TargetGroup &targetGroup = itor->second->getGroup();
		if (targetGroup.getAllGroups().find(objectGroup) != targetGroup.getAllGroups().end())
		{
			float newDistance = (fixedCameraPos - itor->second->getLife().getTargetPosition()).Magnitude().asFloat();
			if (newDistance < distance)
			{
				distance = newDistance;
			}
		}
	}

	distance *= 4.0f * falloff;
	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
#endif

	return true;
}

bool LandscapeSoundPositionWater::readXML(XMLNode *node)
{
	if (!node->getNamedChild("falloff", falloff)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionWater::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	Vector &cameraPos = 
		MainCamera::instance()->getCamera().getCurrentPos();

	float distance = Landscape::instance()->getWater().
		getWaveDistance(int(cameraPos[0]), int(cameraPos[1]));
	distance *= 4.0f * falloff;

	Vector position(0.0f, 0.0f, distance + cameraPos[2]);

	source->setRelative();
	source->setPosition(position);
#endif

	return true;
}

bool LandscapeSoundPositionAmbient::readXML(XMLNode *node)
{
	return node->failChildren();
}

bool LandscapeSoundPositionAmbient::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	source->setRelative();
	source->setPosition(Vector::getNullVector());
#endif

	return true;
}

bool LandscapeSoundPositionAbsoulte::readXML(XMLNode *node)
{
	if (!node->getNamedChild("position", position)) return false;
	return node->failChildren();
}

bool LandscapeSoundPositionAbsoulte::setPosition(VirtualSoundSource *source, LandscapeSoundItem *data)
{
#ifndef S3D_SERVER
	source->setPosition(position);
#endif
	return true;
}

bool LandscapeSoundTimingLooped::readXML(XMLNode *node)
{
	return node->failChildren();
}

float LandscapeSoundTimingLooped::getNextEventTime()
{
	return -1.0f;
}

bool LandscapeSoundTimingRepeat::readXML(XMLNode *node)
{
	if (!node->getNamedChild("min", min)) return false;
	if (!node->getNamedChild("max", max)) return false;
	return node->failChildren();
}

float LandscapeSoundTimingRepeat::getNextEventTime()
{
	return min + max * RAND;
}

bool LandscapeSoundSoundFile::readXML(XMLNode *node)
{
	std::string file;
	while (node->getNamedChild("file", file, false))
	{
		if (!S3D::checkDataFile(file.c_str())) return false;
		files.push_back(S3D::getModFile(file));
	}
	if (files.empty()) return node->returnError("No file node");

	gain = 1.0f;
	node->getNamedChild("gain", gain, false);
	referencedistance = 75.0f;
	node->getNamedChild("referencedistance", referencedistance, false);
	rolloff = 1.0f;
	node->getNamedChild("rolloff", rolloff, false);

	return node->failChildren();
}

bool LandscapeSoundSoundFile::play(VirtualSoundSource *source, float ambientGain)
{
#ifndef S3D_SERVER
	std::string &file = files[rand() % files.size()];
	SoundBuffer *buffer = 
		Sound::instance()->fetchOrCreateBuffer(file.c_str());
	if (!buffer) return false;

	source->setGain(gain * ambientGain);
	source->setRolloff(rolloff);
	source->setReferenceDistance(referencedistance);
	source->play(buffer);
#endif

	return true;
}

LandscapeSoundType::LandscapeSoundType() : 
	position(0), timing(0), sound(0)
{
}

LandscapeSoundType::~LandscapeSoundType()
{
	delete position;
	delete timing;
	delete sound;
}

bool LandscapeSoundType::readXML(XMLNode *node)
{
	{
		std::string soundtype;
		XMLNode *soundNode;
		if (!node->getNamedChild("sound", soundNode)) return false;
		if (!soundNode->getNamedParameter("type", soundtype)) return false;
		if (0 == strcmp(soundtype.c_str(), "file"))
			sound = new LandscapeSoundSoundFile;
		else return false;

		if (!sound->readXML(soundNode)) return false;
	}

	{
		std::string positiontype;
		XMLNode *positionNode;
		if (!node->getNamedChild("position", positionNode)) return false;
		if (!positionNode->getNamedParameter("type", positiontype)) return false;
		if (0 == strcmp(positiontype.c_str(), "ambient"))
			position = new LandscapeSoundPositionAmbient;
		else if (0 == strcmp(positiontype.c_str(), "absolute"))
			position = new LandscapeSoundPositionAbsoulte;
		else if (0 == strcmp(positiontype.c_str(), "water"))
			position = new LandscapeSoundPositionWater;
		else if (0 == strcmp(positiontype.c_str(), "group"))
			position = new LandscapeSoundPositionGroup;
		else if (0 == strcmp(positiontype.c_str(), "set"))
			position = new LandscapeSoundPositionSet;
		else return false;
		if (!position->readXML(positionNode)) return false;
	}
	{
		std::string timingtype;
		XMLNode *timingNode;
		if (!node->getNamedChild("timing", timingNode)) return false;
		if (!timingNode->getNamedParameter("type", timingtype)) return false;
		if (0 == strcmp(timingtype.c_str(), "looped"))
			timing = new LandscapeSoundTimingLooped;
		else if (0 == strcmp(timingtype.c_str(), "repeat"))
			timing = new LandscapeSoundTimingRepeat;
		else return false;
		if (!timing->readXML(timingNode)) return false;
	}
	return node->failChildren();
}
