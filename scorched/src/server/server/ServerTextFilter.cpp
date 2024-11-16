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

#include <vector>
#include <server/ServerTextFilter.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>

#define IS_SPACE(c) ((c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || (c > 'z' && c <= 127)))

ServerTextFilter::ServerTextFilter() : lastReadTime_(0)
{
}

ServerTextFilter::~ServerTextFilter()
{
}

void ServerTextFilter::filterString(LangString &inputText)
{
	loadFile();

	if (words_.empty()) return;
	std::vector<TextPart> parts;

	// Split the string into parts (words) each seperated by the 
	// seperating characters
    LangString current;
	const unsigned int *pos = 0;
	for (const unsigned int *c = inputText.c_str(); *c; c++)
	{
		if (IS_SPACE(*c))
		{
			if (current.c_str()[0])
			{
				TextPart part;
				part.part = current;
				part.pos = (unsigned int *) pos;
				parts.push_back(part);
			}
			current.clear();
			pos = 0;
		}
		else
		{
			if (!pos) pos = c;
			current += *c;
		}
	}
	// The first/last part
	if (current.c_str()[0])
	{
		TextPart part;
		part.part = current;
		part.pos = (unsigned int *) pos;
		parts.push_back(part);
	}

	// For each part 
	std::vector<TextPart>::iterator itor;
	for (itor = parts.begin(); itor != parts.end(); ++itor)
	{
		TextPart &part = (*itor);
		const unsigned int *text = part.part.c_str();
		
		// Check that they don't contain the words
		std::list<LangString>::iterator witor;
		for (witor = words_.begin(); witor != words_.end(); ++witor)
		{
			const unsigned int *word = (*witor).c_str();
			unsigned int *pos = LangStringUtil::stristr(text, word);
			if (pos)
			{
				// Only filter out if the words is at the start or end of the word
				if (pos == text || (pos - text) + LangStringUtil::strlen(word) == LangStringUtil::strlen(text))
				{
					// If they do then * out the word
					for (int i=0; i<LangStringUtil::strlen(word); i++)
					{
						pos[i] = '*';
					}
				}
			}
		}
	}

	// For each combination of parts check that they don't add up to the words
	for (int i=0; i<(int) parts.size(); i++)
	{
		LangString sofar;
		for (int j=i; j<(int) parts.size(); j++)
		{
			sofar += parts[j].part;
			const unsigned int *text = sofar.c_str();

			// Check each word against the parts so far
			std::list<LangString>::iterator witor;
			for (witor = words_.begin(); witor != words_.end(); ++witor)
			{
				const unsigned int *word = (*witor).c_str();
				if (LangStringUtil::strcasecmp(text, word) == 0)
				{
					// If they match, * out all parts
					for (int k=i; k<=j; k++)
					{
						for (unsigned int *pos = (unsigned int *) parts[k].part.c_str();
							*pos; 
							pos++)
						{
							*pos = '*';
						}
					}

					break;
				}
			}
		}
	}

	// Re-form the words
	for (itor = parts.begin(); itor != parts.end(); ++itor)
	{
		TextPart &part = (*itor);
		const unsigned int *text = part.part.c_str();

		for (int i=0; i<(int) part.part.size(); i++)
		{
			part.pos[i] = text[i];
		}
	}
}

void ServerTextFilter::loadFile()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("filter-%i.txt", 
			ScorchedServer::instance()->getOptionsGame().getPortNo()));
	if (!::S3D::fileExists(filename)) return;

	time_t fileTime = S3D::fileModTime(filename);
	if (fileTime == lastReadTime_) return;

    FileLines lines;
	if (!lines.readFile(filename)) return;

	lastReadTime_ = fileTime;

	words_.clear();
	std::vector<std::string>::iterator itor;
	for (itor = lines.getLines().begin();
		itor != lines.getLines().end();
		++itor)
	{
		if ((*itor).c_str()[0])
		{
			words_.push_back(LANG_STRING(*itor));
		}
	}
}
