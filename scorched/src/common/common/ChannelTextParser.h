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

#if !defined(__INCLUDE_ChannelTextParserh_INCLUDE__)
#define __INCLUDE_ChannelTextParserh_INCLUDE__

#include <lang/LangString.h>
#include <common/Vector.h>
#include <vector>

class ScorchedContext;
class ChannelTextParser
{
public:
	enum ChannelTextEntryType
	{
		ePlayerEntry,
		eWeaponEntry,
		eChannelEntry,
		eAdminEntry,
		eTipEntry
	};
	struct ChannelTextEntry
	{
		ChannelTextEntry() : data(0) {}

		ChannelTextEntryType type;
		unsigned int data; // Not a pointer incase for e.g. the tank disconects 
		LangString text;
		LangString part;
		Vector color;
	};

	ChannelTextParser();
	virtual ~ChannelTextParser();

	void parseText(ScorchedContext &context, const LangString &text);
	void subset(ChannelTextParser &other, int start, int len);

	const LangString &getString() { return text_; }

	ChannelTextEntry *getEntry(int position);
    
protected:
	LangString text_;
	std::vector<unsigned int> entryIndex_;
	std::vector<ChannelTextEntry> entries_;

	bool parseUrl(ScorchedContext &context, 
		const LangString &url, ChannelTextEntry &entry);
	bool createPlayerEntry(ScorchedContext &context,
		const LangString &part, ChannelTextEntry &entry);
	bool createWeaponEntry(ScorchedContext &context, 
		const LangString &partt, ChannelTextEntry &entry);
	bool createChannelEntry(ScorchedContext &context, 
		const LangString &part, ChannelTextEntry &entry);
	bool createTipEntry(ScorchedContext &context, 
		const LangString &part, ChannelTextEntry &entry);
	bool createAdminEntry(ScorchedContext &context, 
		const LangString &part, ChannelTextEntry &entry);
	void addIndex(int number, unsigned char index);
};

#endif
