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

#ifndef __keyboardKey_h__
#define __keyboardKey_h__

#include <string>
#include <list>
#include <vector>

class KeyboardKey
{
public:
	struct KeyEntry
	{
		unsigned int key;
		unsigned int state;
	};

	KeyboardKey(const char *name, 
		const char *title,
		const char *description,
		int group,
		bool command);
	virtual ~KeyboardKey();

	bool keyDown(char *buffer, unsigned int keyState, bool repeat = true);
	bool keyMatch(unsigned key);
	
	bool addKeys(std::list<std::string> &keyNames,
		std::list<std::string> &keyStates);
	void addKey(unsigned int position,
		unsigned int key, unsigned int state);
	void removeKey(unsigned int position);
	int keyIndex(unsigned int key, unsigned int state);

	std::vector<KeyEntry> &getKeys() { return keys_; }
	const char *getName() { return name_.c_str(); }
	const char *getTitle() { return title_.c_str(); }
	const char *getDescription() { return description_.c_str(); }
	int getGroup() { return group_; }
	bool getNameIsCommand() { return command_; }
	bool getChanged() { return changed_; }
	void setChanged(bool changed) { changed_ = changed; }

	static bool translateKeyName(const char *name, unsigned int &key);
	static bool translateKeyState(const char *name, unsigned int &state);
	static bool translateKeyNameValue(unsigned int key, const char *&name);
	static bool translateKeyStateValue(unsigned int state, const char *&name);

protected:
	std::string name_;
	std::string title_;
	std::string description_;
	std::vector<KeyEntry> keys_;
	int group_;
	bool keyToogle_;
	bool command_;
	bool changed_;

private:
	KeyboardKey(const KeyboardKey &);
	const KeyboardKey & operator=(const KeyboardKey &);

};

#endif // __keyboardKey_h__

