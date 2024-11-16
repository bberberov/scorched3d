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

#include <common/KeyboardKey.h>
#include <common/KeyTranslate.h>
#include <common/KeyStateTranslate.h>
#include <common/Defines.h>

KeyboardKey::KeyboardKey(const char *name,
	const char *title,
	const char *description,
	int group,
	bool command) :
	name_(name), title_(title), description_(description), 
	keyToogle_(false), command_(command), group_(group),
	changed_(false)
{
}

KeyboardKey::~KeyboardKey()
{
}

bool KeyboardKey::addKeys(std::list<std::string> &keyNames,
						  std::list<std::string> &keyStates)
{
	DIALOG_ASSERT(keyNames.size() == keyStates.size());
	keys_.clear();

	unsigned int i = 0;
	std::list<std::string>::iterator itor;
	std::list<std::string>::iterator itorState;
	for (itor = keyNames.begin(), itorState = keyStates.begin();
		 itor != keyNames.end() && itorState != keyStates.end();
		 ++itor, i++, ++itorState)
	{
		unsigned int keyValue = 0;
		std::string &name = *itor;
		if (!translateKeyName(name.c_str(), keyValue)) 
		{
			S3D::dialogMessage("KeyboardKey", S3D::formatStringBuffer(
						  "Failed to find key names \"%s\" for \"%s\"",
						  name.c_str(),
						  name_.c_str()));
			return false;
		}

		unsigned int keyState = 0;
		std::string &state = *itorState;
		if (!translateKeyState(state.c_str(), keyState)) 
		{
			S3D::dialogMessage("KeyboardKey", S3D::formatStringBuffer(
						  "Failed to find key state \"%s\" for \"%s\"",
						  state.c_str(),
						  name_.c_str()));
			return false;
		}

		addKey(i, keyValue, keyState);
	}
	return true;
}

int KeyboardKey::keyIndex(unsigned int key, unsigned int state)
{
	int i=0;
	std::vector<KeyEntry>::iterator itor;
	for (itor = keys_.begin();
		itor != keys_.end();
		++itor, i++)
	{
		KeyEntry &entry = *itor;
		if (entry.key == key &&
			entry.state == state) return i;
	}
	return -1;
}

void KeyboardKey::addKey(unsigned int position,
	unsigned int key, unsigned int state)
{
	KeyEntry entry;
	entry.key = key;
	entry.state = state;
	
	if (position < keys_.size())
	{ 
		keys_[position] = entry;
	}
	else
	{
		keys_.push_back(entry);
	}
	changed_ = true;
}

void KeyboardKey::removeKey(unsigned int position)
{
	if (position < keys_.size())
	{
		unsigned int i = 0;
		std::vector<KeyEntry>::iterator itor;
		for (itor = keys_.begin();
			itor != keys_.end();
			++itor, i++)
		{
			if (i == position)
			{
				keys_.erase(itor);
				break;
			}
		}
	}
	changed_ = true;
}

bool KeyboardKey::translateKeyName(const char *name, unsigned int &key)
{
	for (unsigned int i=0; i<sizeof(KeyTranslationTable)/sizeof(KeyTranslation); i++)
	{
		if (strcmp(KeyTranslationTable[i].keyName, name)==0)
		{
			key = KeyTranslationTable[i].keySym;
			return true;
		}
	}
	return false;
}

bool KeyboardKey::translateKeyState(const char *name, unsigned int &state)
{
	for (unsigned int i=0; i<sizeof(KeyStateTranslationTable)/sizeof(KeyStateTranslation); i++)
	{
		if (strcmp(KeyStateTranslationTable[i].keyStateName, name)==0)
		{
			state = KeyStateTranslationTable[i].keyStateSym;
			return true;
		}
	}
	return false;
}

bool KeyboardKey::translateKeyNameValue(unsigned int key, const char *&name)
{
	for (unsigned int i=0; i<sizeof(KeyTranslationTable)/sizeof(KeyTranslation); i++)
	{
		if (KeyTranslationTable[i].keySym == key)
		{
			name = KeyTranslationTable[i].keyName;
		}
	}
	return false;
}

bool KeyboardKey::translateKeyStateValue(unsigned int state, const char *&name)
{
	for (unsigned int i=0; i<sizeof(KeyStateTranslationTable)/sizeof(KeyStateTranslation); i++)
	{
		if (KeyStateTranslationTable[i].keyStateSym == state)
		{
			name = KeyStateTranslationTable[i].keyStateName;
		}
	}
	return false;
}

bool KeyboardKey::keyDown(char *buffer, unsigned int keyState, bool repeat)
{
	for (unsigned int i=0; i<keys_.size(); i++)
	{
		if (keyState == keys_[i].state && buffer[keys_[i].key])
		{
			if (!repeat && keyToogle_) return false;

			keyToogle_ = true;
			return true;
		}
	}

	keyToogle_ = false;
	return false;
}

bool KeyboardKey::keyMatch(unsigned key)
{
	for (unsigned int i=0; i<keys_.size(); i++)
	{
		if (keys_[i].key == key) return true;
	}
	return false;
}
