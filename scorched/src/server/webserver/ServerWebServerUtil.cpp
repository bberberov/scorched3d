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

#include <webserver/ServerWebServerUtil.h>
#include <server/ScorchedServer.h>
#include <net/NetMessagePool.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionEntry.h>

const char *ServerWebServerUtil::getField(
	std::map<std::string, std::string> &fields, const std::string &field)
{
	std::map<std::string, std::string>::iterator itor = 
		fields.find(field);
	if (itor != fields.end())
	{
		return (*itor).second.c_str();
	}
	return 0;
}

std::string ServerWebServerUtil::getFile(const std::string &filename)
{
	char buffer[100];
	std::string file;
	FILE *in = fopen(filename.c_str(), "r");
	if (in)
	{
		while (fgets(buffer, 100, in))
		{
			file += buffer;
		}
		fclose(in);
	}
	return file;
}

std::string ServerWebServerUtil::htmlLangString(const LangString &langstring)
{
	std::string result;
	for (const LangStringStorage *c=langstring.c_str();*c;c++)
	{
		if (*c < ' ' || 
			(*c > ' ' && *c < '0') ||
			(*c > '9' && *c < 'A') ||
			(*c > 'Z' && *c < 'a') ||
			*c > 'z')
		{
			result.append("&#");
			result.append(S3D::formatStringBuffer("%u", *c));
			result.append(";");
		}
		else
		{
			result.push_back((char) *c);
		}
	}
	return result;
}

std::string ServerWebServerUtil::concatLines(std::list<std::string> &lines)
{
	std::string result;
	std::list<std::string>::iterator itor;
	for (itor = lines.begin();
		itor != lines.end();
		++itor)
	{
		result.append(*itor).append("<br>");

	}
	return result;
}

const char *ServerWebServerUtil::strstrlen(const char *start, const char *find, int size)
{
	int findsize = strlen(find);
	const char *current = start;
	for (int i=0; i<size - findsize; i++, current++)
	{
		bool found = true;
		for (int j=0; j<findsize; j++)
		{
			if (current[j] != find[j])
			{
				found = false;
				break;
			}
		}
		if (found) return current;
	}
	return 0;
}

void ServerWebServerUtil::extractMultiPartPost(const char *start, 
	const char *boundry, int sizeleft, std::map<std::string, NetMessage *> &parts)
{
	int boundrylen = strlen(boundry);
	while (true)
	{
		// Find the first boundry
		const char *first = strstrlen(start, boundry, sizeleft);
		if (!first) return;

		// We've now got less to search
		first += boundrylen;
		sizeleft -= first - start; 
		start = first;

		// Find the name
		const char *namestart = strstrlen(start, "name=\"", sizeleft); 
		if (!namestart) return;
		namestart += 6;
		const char *nameend = strstrlen(namestart, "\"", sizeleft);
		if (!nameend) return;
		if (nameend-namestart < 1) return;
		std::string name(namestart, nameend - namestart);

		// Find the data start
		const char *data = strstrlen(start, "\r\n\r\n", sizeleft);
		if (!data) return;
		data += 4;

		// Find the second boundry
		const char *second = strstrlen(start, boundry, sizeleft);
		if (!second) return;

		// The message is from data to second
		int messagelen = second - data;
		NetMessage *message = NetMessagePool::instance()->getFromPool(
			NetMessage::BufferMessage, 0, 0, 0);
		message->getBuffer().addDataToBuffer(data, messagelen);
		parts[name] = message;
	}
}

void ServerWebServerUtil::extractQueryFields(std::map<std::string, std::string> &fields, char *sep)
{
	char *token = strtok(sep, "&");
	while(token)
	{
		char *eq = strchr(token, '=');
		if (eq)
		{
			*eq = '\0';
			std::string value;
			for (const char *valueStr = (eq + 1); *valueStr; valueStr++)
			{
				char c = *valueStr;
				if (c == '+') c = ' ';
				else if (c == '%')
				{
					char buf[3] = { 0, 0, 0 };

					buf[0] = *(valueStr + 1);
					if (!buf[0]) break;
					buf[1] = *(valueStr + 2);
					if (!buf[1]) break;

					c = (char) strtol(buf, 0, 16);

					valueStr += 2;
				}

				if (c != '\r') value += c;
			}

			if (fields.find(token) == fields.end())
			{
				fields[token] = value;
			}
			*eq = '=';
		}				
		token = strtok(0, "&");
	}
}

void ServerWebServerUtil::generateSettingValue(OptionEntry *entry, std::string &value)
{
	if (entry->getEntryType() == OptionEntry::OptionEntryTextType)
	{
		value = S3D::formatStringBuffer("<textarea name='%s' cols=20 rows=5>%s</textarea>",
			entry->getName(),
			entry->getValueAsString());
	}
	else if (entry->getEntryType() == OptionEntry::OptionEntryBoolType)
	{
		OptionEntryBool *boolEntry = (OptionEntryBool *) entry;
		
		value = S3D::formatStringBuffer(
			"<input type='radio' name='%s' %s value='on'>On</input>"
			"<input type='radio' name='%s' %s value='off'>Off</input>",
			entry->getName(), (boolEntry->getValue()?"checked":""),
			entry->getName(), (!boolEntry->getValue()?"checked":""));
	}
	else if (entry->getEntryType() == OptionEntry::OptionEntryBoundedIntType)
	{
		OptionEntryBoundedInt *intEntry = (OptionEntryBoundedInt *) entry;
		
		value = S3D::formatStringBuffer("<select name='%s'>", entry->getName());
		bool found = false;
		for (int i=intEntry->getMinValue(); 
			i<=intEntry->getMaxValue();
			i+=intEntry->getStepValue())
		{
			if (intEntry->getValue() < i && !found)
			{
				found = true;
				value.append(S3D::formatStringBuffer("<option %s>%i</option>",
					"selected", 
					intEntry->getValue()));
			}
			else if (intEntry->getValue() == i)
			{
				found = true;
			}

			value.append(S3D::formatStringBuffer("<option %s>%i</option>",
				(intEntry->getValue() == i?"selected":""), 
				i));
		}
		value.append("</select>");
	}
	else if (entry->getEntryType() == OptionEntry::OptionEntryEnumType)
	{
		OptionEntryEnum *enumEntry = (OptionEntryEnum *) entry;

		value = S3D::formatStringBuffer("<select name='%s'>", entry->getName());
		OptionEntryEnum::EnumEntry *enums = enumEntry->getEnums();
		for (OptionEntryEnum::EnumEntry *current = enums; current->description[0]; current++)
		{
			value.append(S3D::formatStringBuffer("<option %s>%s</option>",
				(enumEntry->getValue() == current->value?"selected":""), 
				current->description));		
		}
		value.append("</select>");
	}
	else if (entry->getEntryType() == OptionEntry::OptionEntryStringEnumType)
	{
		OptionEntryStringEnum *enumEntry = (OptionEntryStringEnum *) entry;
		
		value = S3D::formatStringBuffer("<select name='%s'>", entry->getName());
		OptionEntryStringEnum::EnumEntry *enums = enumEntry->getEnums();
		for (OptionEntryStringEnum::EnumEntry *current = enums; current->value[0]; current++)
		{
			value.append(S3D::formatStringBuffer("<option %s>%s</option>",
				(0 == strcmp(enumEntry->getValue(), current->value)?"selected":""), 
				current->value));		
		}
		value.append("</select>");
	}
	else
	{
		value = S3D::formatStringBuffer("<input type='text' name='%s' value='%s'>",
			entry->getName(),
			entry->getValueAsString());
	}
}

void ServerWebServerUtil::getHtmlRedirect(
	const std::string &url,
	std::string &result)
{
	std::string header = 
		S3D::formatStringBuffer(
		"HTTP/1.1 302 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Type: text/html\r\n"
		"Connection: Close\r\n"
		"Location: %s\r\n"
		"\r\n", url.c_str());
	result.append(header);
}

void ServerWebServerUtil::getHtmlNotFound(
	std::string &result)
{
	std::string header = 
		S3D::formatStringBuffer(
		"HTTP/1.1 404 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Type: text/html\r\n"
		"Connection: Close\r\n"
		"\r\n");
	result.append(header);
}

bool ServerWebServerUtil::getHtmlTemplate(
	ServerAdminSessions::SessionParams *session,
	const std::string &name,
	std::map<std::string, std::string> &fields,
	std::string &result)
{
	const char *header = 
		"HTTP/1.1 200 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Type: text/html\r\n"
		"Connection: Close\r\n"
		"\r\n";
	result.append(header);

	return getTemplate(session, name, fields, result);
}

bool ServerWebServerUtil::getTemplate(
	ServerAdminSessions::SessionParams *session,
	const std::string &name,
	std::map<std::string, std::string> &fields,
	std::string &result)
{
	// Perhaps cache this
	std::string fileName = S3D::getDataFile(
		S3D::formatStringBuffer("data/html/server/%s", name.c_str()));
	FILE *in = fopen(fileName.c_str(), "r");
	if (!in) 
	{
		Logger::log(S3D::formatStringBuffer("ERROR: Failed to open web template \"%s\"", fileName.c_str()));
		return false;
	}

	char buffer[1024], include[256];
	while (fgets(buffer, 1024, in))
	{
		// Check for an include line
		if (sscanf(buffer, "#include %s",
			include) == 1)
		{
			// Add the included file
			std::string tmp;
			if (!getTemplate(session, include, fields, tmp))
			{
				return false;
			}

			result += tmp;
		}
		else
		{
			// Check for any value replacements
			char *position = buffer;
			for (;;)
			{
				char *start, *end;
				if ((start = strstr(position, "[[")) &&
					(end = strstr(position, "]]")) &&
					(end > start))
				{
					// Replace the text [[name]] with the value
					*start = '\0';
					*end = '\0';
					result += position;
					position = end + 2;

					char *name = start + 2;

					// First check to see if it is in the supplied fields
					if (fields.find(name) != fields.end())
					{
						result += fields[name];
					}
					else
					{
						// Then in the scorched3d settings
						std::list<OptionEntry *>::iterator itor;
						std::list<OptionEntry *> &options = 
							ScorchedServer::instance()->getOptionsGame().
								getChangedOptions().getOptions();
						for (itor = options.begin();
							itor != options.end();
							++itor)
						{
							OptionEntry *entry = (*itor);
							if (!(entry->getData() & OptionEntry::DataProtected))
							{
								if (strcmp(entry->getName(), name) == 0)
								{
									result += entry->getValueAsString();
								}
								else
								{
									std::string newName(entry->getName());
									newName.append("_set");
									if (strcmp(newName.c_str(), name) == 0)
									{
										std::string value;
										generateSettingValue(entry,value);
										result += value;
									}
								}
							}
						}						
					}
				}
				else
				{
					// No replacements
					result += position;
					break;
				}
			}
		}
	}
	fclose(in);

	while (true)
	{
		// Find start {{permission}}
		size_t start1 = result.find("{{");
		if (start1 == std::string::npos) break;
		size_t end1 = result.find("}}", start1);
		if (end1 == std::string::npos) break;
		std::string perm(result, start1 + 2, end1 - start1 - 2);
		result.replace(start1, end1 - start1 + 2, "");

		// Find end {{permission}}
		size_t start2 = result.find(
			S3D::formatStringBuffer("{{%s}}", perm.c_str()), start1);
		if (start2 == std::string::npos) break;
		result.replace(start2, 4 + perm.size(), "");

		if (session->credentials.permissions.find(perm) == 
			session->credentials.permissions.end())
		{
			result.replace(start1, start2 - start1, "");
		}
	}

	return true;
}

bool ServerWebServerUtil::getHtmlMessage(
	ServerAdminSessions::SessionParams *session,
	const std::string &title,
	const std::string &text,
	std::map<std::string, std::string> &fields,
	std::string &result)
{
	fields["MESSAGE"] = text;
	fields["TITLE"] = title;
	return getHtmlTemplate(session, "message.html", fields, result);
}
