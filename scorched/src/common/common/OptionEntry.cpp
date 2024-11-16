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

#include <common/OptionEntry.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <XML/XMLFile.h>
#include <stdio.h>
#include <map>

OptionEntry::OptionEntry(std::list<OptionEntry *> &group,
						 const char *name,
						 const char *description,
						 unsigned int data) :
	name_(name),
	description_(description),
	data_(data),
	changedValue_(false)
{
	group.push_back(this);
}

OptionEntry::~OptionEntry()
{

}

bool OptionEntryHelper::addToArgParser(std::list<OptionEntry *> &options,
									   ARGParser &parser)
{
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		if (!(*itor)->addToArgParser(parser)) return false;
	}

	return true;
}

bool OptionEntryHelper::writeToBuffer(std::list<OptionEntry *> &options,
									  NetBuffer &buffer,
									  bool useprotected)
{
	// Write out all of the options
	// We do this as strings this make the message larger than it needs to be but
	// we always know how to read the options at the other end.
	// Even if the other end does not have all the options this end does.
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = *itor;
		// Only send data that is allowed to be sent
		if (!(entry->getData() & OptionEntry::DataProtected) || useprotected)
		{
			// Optimization, only send options that have changed from thier default
			if (!entry->isDefaultValue())
			{
				buffer.addToBuffer(entry->getName());
				buffer.addToBuffer(entry->getComsBufferValue());
			}
		}
	}
	buffer.addToBuffer("-"); // Marks the end of the entries

	return true;
}

bool OptionEntryHelper::readFromBuffer(std::list<OptionEntry *> &options,
									   NetBufferReader &reader,
									   bool useprotected)
{
	// Create a map from string name to existing options
	// So we can find the named option when reading the buffer
	std::map<std::string, OptionEntry *> entryMap;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = (*itor);
		if (!(entry->getData() & OptionEntry::DataProtected) || useprotected)
		{
			// Create map
			std::string name = entry->getName();
			entryMap[name] = entry;

			// Reset to the default value as only the non-default values are sent
			if (!entry->isDefaultValue())
			{
				entry->setValueFromString(entry->getDefaultValueAsString());
			}
		}
	}

	// Read the strings from the other end
	for (;;)
	{
		std::string name, value;
		if (!reader.getFromBuffer(name)) return false;
		if (strcmp(name.c_str(), "-") == 0) break; // The end symbol
		if (!reader.getFromBuffer(value)) return false;
		
		std::map<std::string, OptionEntry *>::iterator finditor =
			entryMap.find(name);
		if (finditor == entryMap.end())
		{
			Logger::log(S3D::formatStringBuffer("Warning:Does not support server option \"%s\"",
				name.c_str()));
		}
		else
		{
			OptionEntry *entry = (*finditor).second;
			if (!entry->setComsBufferValue(value.c_str())) return false;
		}
	}

	return true;
}

bool OptionEntryHelper::writeToXML(std::list<OptionEntry *> &options,
	XMLNode *node, bool allOptions)
{
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = (*itor);

		if (!(entry->getData() & OptionEntry::DataDepricated))
		{

			// Add the comments for this node
			node->addChild(new XMLNode("", 
				S3D::formatStringBuffer("%s: %s.  Default value : \"%s\" %s.", 
				entry->getName(), entry->getDescription(), 
				entry->getDefaultValueAsString(), entry->getRangeDescription()?entry->getRangeDescription():""), 
				XMLNode::XMLCommentType));
			
			if (!entry->isDefaultValue() || allOptions)
			{
				// Add the name and value
				XMLNode *optionNode = new XMLNode("option");
				node->addChild(optionNode);
				optionNode->addChild(
					new XMLNode("name", entry->getName()));
				optionNode->addChild(
					new XMLNode("value",entry->getValueAsString()));
			}
		}
	}
	return true;
}

bool OptionEntryHelper::writeToFile(std::list<OptionEntry *> &options,
									const std::string &filePath, 
									bool allOptions)
{
	XMLNode optionsNode("options");
	optionsNode.addParameter(
		new XMLNode("source", "Scorched3D", XMLNode::XMLParameterType));
	writeToXML(options, &optionsNode, allOptions);

	if (!optionsNode.writeToFile(filePath.c_str())) return false;

	return true;
}

bool OptionEntryHelper::readFromXML(std::list<OptionEntry *> &options,
									 XMLNode *rootnode)
{
	// Create a map from string name to existing options
	// So we can find the named option when parsing the file
	std::map<std::string, OptionEntry *> entryMap;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = (*itor);
		std::string name = entry->getName();
		_strlwr((char *) name.c_str());

		entryMap[name] = entry;
	}

	// Itterate all of the options in the file
	XMLNode *currentNode = 0;
	while (rootnode->getNamedChild("option", currentNode, false))
	{
		XMLNode *nameNode, *valueNode; 
		if (!currentNode->getNamedChild("name", nameNode)) return false;
		if (!currentNode->getNamedChild("value", valueNode)) return false;

		std::string name = nameNode->getContent();
		std::string value = valueNode->getContent();
		_strlwr((char *) name.c_str());

		std::map<std::string, OptionEntry *>::iterator finditor =
			entryMap.find(name);
		if (finditor == entryMap.end())
		{
			Logger::log(S3D::formatStringBuffer(
				"ERROR: Failed to parse file \"%s\". Cannot find option \"%s\"",
				currentNode->getSource(), name.c_str()));
		}
		else
		{
			if (!(*finditor).second->setValueFromString(value))
			{
				S3D::dialogMessage("Scorched3D Options", S3D::formatStringBuffer(
					"ERROR: Failed to parse file \"%s\"\n"
					"Failed to set option \"%s\" with \"%s\"",
					currentNode->getSource(), name.c_str(), value.c_str()));
				return false;		
			}	
		}
	}
	return true;
}								 

bool OptionEntryHelper::readFromFile(std::list<OptionEntry *> &options,
									 const std::string &filePath)
{
	// Parse the XML file
	XMLFile file;
	if (!file.readFile(filePath.c_str()))
	{
		S3D::dialogMessage("Scorched3D Options", S3D::formatStringBuffer(
			"ERROR: Failed to parse file \"%s\"\n"
			"%s",
			filePath.c_str(),
			file.getParserError()));
		return false;
	}

	// return true for an empty file
	if (!file.getRootNode()) return true;

	// Read the options from the XML node
	if (!readFromXML(options, file.getRootNode())) return false;

	return true;
}

OptionEntry *OptionEntryHelper::getEntry(
	std::list<OptionEntry *> &options, const char *name)
{
	std::list<OptionEntry *>::iterator itor;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = (*itor);
		if (0 == strcmp(entry->getName(), name)) return entry;
	}
	return 0;
}

OptionEntryInt::OptionEntryInt(std::list<OptionEntry *> &group,
							   const char *name,
							   const char *description,
							   unsigned int data,
							   int value) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

OptionEntryInt::~OptionEntryInt()
{

}

const char *OptionEntryInt::getValueAsString()
{
	static char value[256];
	snprintf(value, 256, "%i", value_);
	return value;
}

const char *OptionEntryInt::getDefaultValueAsString()
{
	static char value[256];
	snprintf(value, 256, "%i", defaultValue_);
	return value;
}

bool OptionEntryInt::setValueFromString(const std::string &string)
{
	int val;
	if (sscanf(string.c_str(), "%i", &val) != 1) return false;
	return setValue(val);
}

int OptionEntryInt::getValue() 
{
	return value_;
}

bool OptionEntryInt::setValue(int value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryInt::setIntArgument(int value)
{
	return setValue(value);
}

bool OptionEntryInt::addToArgParser(ARGParser &parser)
{
	char name[256], description[1024];
	snprintf(name, 256, "-%s", getName());
	snprintf(description, 1024, "%s (Default : %s)",
			(char *) getDescription(), 
			(char *) getDefaultValueAsString());

	parser.addEntry(name , this, description);
	return true;
}

OptionEntryBoundedInt::OptionEntryBoundedInt(std::list<OptionEntry *> &group,
											 const char *name,
											 const char *description,
											 unsigned int data,
											 int value,
											 int minValue, int maxValue, int stepValue) :
	OptionEntryInt(group, name, description, data, value), 
	minValue_(minValue), maxValue_(maxValue), stepValue_(stepValue)
{
}

OptionEntryBoundedInt::~OptionEntryBoundedInt()
{
}

const char *OptionEntryBoundedInt::getRangeDescription()
{
	static std::string result;
	result = S3D::formatStringBuffer("Max = %i, Min = %i", getMaxValue(), getMinValue());
	return result.c_str();
}

bool OptionEntryBoundedInt::setValue(int value)
{
	if (value < minValue_ || value > maxValue_) return false;
	return OptionEntryInt::setValue(value);
}

OptionEntryEnum::OptionEntryEnum(std::list<OptionEntry *> &group,
											 const char *name,
											 const char *description,
											 unsigned int data,
											 int value,
											 OptionEntryEnum::EnumEntry enums[]) :
	OptionEntryInt(group, name, description, data, value), 
	enums_(enums)
{	
}

OptionEntryEnum::~OptionEntryEnum()
{
}

const char *OptionEntryEnum::getRangeDescription()
{
	static std::string result;
	result = "possible values = [";
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->description, current->value);
	}
	result += "]";
	return result.c_str();
}

bool OptionEntryEnum::setValue(int value)
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == value)
		{
			return OptionEntryInt::setValue(value);
		}
	}
	return false;
}

const char *OptionEntryEnum::getDefaultValueAsString()
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == defaultValue_)
		{
			return current->description;
		}
	}
	return "-";
}

const char *OptionEntryEnum::getValueAsString()
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (current->value == value_)
		{
			return current->description;
		}
	}
	return "-";
}

bool OptionEntryEnum::setValueFromString(const std::string &string)
{
	for (EnumEntry *current = enums_; current->description[0]; current++)
	{
		if (0 == strcmp(current->description, string.c_str()))
		{
			setValue(current->value);
			return true;
		}
	}

	int val;
	if (sscanf(string.c_str(), "%i", &val) != 1)
	{
		return setValueFromString(getDefaultValueAsString());
	}
	return setValue(val);
}

OptionEntryBool::OptionEntryBool(std::list<OptionEntry *> &group,
								 const char *name, 
								 const char *description, 
								 unsigned int data,
								 bool value) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

OptionEntryBool::~OptionEntryBool()
{
	
}

const char *OptionEntryBool::getDefaultValueAsString()
{
	return (defaultValue_?"on":"off");
}

const char *OptionEntryBool::getValueAsString()
{
	return (value_?"on":"off");
}

bool OptionEntryBool::setValueFromString(const std::string &string)
{
	if (stricmp("on", string.c_str()) == 0)
	{
		setValue(true);
		return true;
	}
	if (stricmp("off", string.c_str()) == 0)
	{
		setValue(false);
		return true;
	}
	return false;
}

bool OptionEntryBool::setValue(bool value)
{
	changedValue_ = true;
	value_ = (value?1:0);
	return true;
}

bool OptionEntryBool::getValue()
{
	return ((value_==0)?false:true);
}

bool OptionEntryBool::setBoolArgument(bool value)
{
	return setValue(value);
}

bool OptionEntryBool::addToArgParser(ARGParser &parser)
{
	char name[256], description[1024];
	snprintf(name, 256, "-%s", getName());
	snprintf(description, 1024, "%s (Default : %s)",
			(char *) getDescription(), 
			(char *) getDefaultValueAsString());

	parser.addEntry(name , this, description);
	return true;
}

OptionEntryString::OptionEntryString(std::list<OptionEntry *> &group,
									 const char *name, 
									 const char *description, 
									 unsigned int data,
									 const char *value,
									 bool multiline) :
	OptionEntry(group, name, description, data),
	value_(value), defaultValue_(value), multiline_(multiline)
{
	
}

OptionEntryString::~OptionEntryString()
{

}

const char *OptionEntryString::getValueAsString()
{
	return value_.c_str();
}

const char *OptionEntryString::getDefaultValueAsString()
{
	return defaultValue_.c_str();
}

bool OptionEntryString::setValueFromString(const std::string &string)
{
	return setValue(string);
}

const char *OptionEntryString::getValue() 
{
	return value_.c_str();
}

bool OptionEntryString::setValue(const std::string &value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryString::setStringArgument(const char* value)
{
	return setValue(value);
}

bool OptionEntryString::addToArgParser(ARGParser &parser)
{
	char name[256], description[1024];
	snprintf(name, 256, "-%s", getName());
	snprintf(description, 1024, "%s (Default : %s)",
			(char *) getDescription(), 
			(char *) getDefaultValueAsString());

	parser.addEntry(name , this, description);
	return true;
}

OptionEntryStringEnum::OptionEntryStringEnum(std::list<OptionEntry *> &group,
											 const char *name,
											 const char *description,
											 unsigned int data,
											 const char *value,
											 OptionEntryStringEnum::EnumEntry enums[]) :
	OptionEntryString(group, name, description, data, value), 
	enums_(enums)
{	
}

OptionEntryStringEnum::~OptionEntryStringEnum()
{
}

const char *OptionEntryStringEnum::getRangeDescription()
{
	static std::string result;
	result = "possible values = [";
	for (EnumEntry *current = enums_; current->value[0]; current++)
	{
		result += S3D::formatStringBuffer(" \"%s\" ",
			current->value);
	}
	result += "]";
	return result.c_str();
}

bool OptionEntryStringEnum::setValue(const std::string &value)
{
	for (EnumEntry *current = enums_; current->value[0]; current++)
	{
		if (0 == strcmp(current->value, value.c_str()))
		{
			return OptionEntryString::setValue(value);
		}
	}
	return false;
}

bool OptionEntryStringEnum::setValueFromString(const std::string &string)
{
	return setValue(string);
}

OptionEntryFloat::OptionEntryFloat(std::list<OptionEntry *> &group,
							   const char *name,
							   const char *description,
							   unsigned int data,
							   float value,
							   bool truncate) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value), truncate_(truncate)
{
	
}

OptionEntryFloat::~OptionEntryFloat()
{

}

const char *OptionEntryFloat::getValueAsString()
{
	static char value[256];
	if (truncate_) snprintf(value, 256, "%.2f", value_);
	else snprintf(value, 256, "%.8f", value_);
	return value;
}

const char *OptionEntryFloat::getDefaultValueAsString()
{
	static char value[256];
	if (truncate_) snprintf(value, 256, "%.2f", defaultValue_);
	else snprintf(value, 256, "%.8f", defaultValue_);
	return value;
}

bool OptionEntryFloat::setValueFromString(const std::string &string)
{
	float val;
	if (sscanf(string.c_str(), "%f", &val) != 1) return false;
	return setValue(val);
}

float OptionEntryFloat::getValue() 
{
	return value_;
}

bool OptionEntryFloat::setValue(float value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryFloat::addToArgParser(ARGParser &parser)
{
	DIALOG_ASSERT(0);
	return false;
}

OptionEntryVector::OptionEntryVector(std::list<OptionEntry *> &group,
							   const char *name,
							   const char *description,
							   unsigned int data,
							   Vector value,
							   bool truncate) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value), truncate_(truncate)
{
	
}

OptionEntryVector::~OptionEntryVector()
{

}

const char *OptionEntryVector::getValueAsString()
{
	static char value[256];
	if (truncate_)
	{
		snprintf(value, 256, "%.2f %.2f %.2f", 
			value_[0], value_[1], value_[2]);
	}
	else
	{
		snprintf(value, 256, "%.8f %.8f %.8f", 
			value_[0], value_[1], value_[2]);
	}
	return value;
}

const char *OptionEntryVector::getDefaultValueAsString()
{
	static char value[256];
	if (truncate_)
	{
		snprintf(value, 256, "%.2f %.2f %.2f", 
			defaultValue_[0], defaultValue_[1], defaultValue_[2]);
	}
	else
	{
		snprintf(value, 256, "%.8f %.8f %.8f", 
			defaultValue_[0], defaultValue_[1], defaultValue_[2]);
	}
	return value;
}

bool OptionEntryVector::setValueFromString(const std::string &string)
{
	Vector vec;
	if (sscanf(string.c_str(), "%f %f %f", 
		&vec[0], &vec[1], &vec[2]) != 3) return false;
	return setValue(vec);
}

Vector &OptionEntryVector::getValue() 
{
	return value_;
}

bool OptionEntryVector::setValue(Vector value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryVector::addToArgParser(ARGParser &parser)
{
	DIALOG_ASSERT(0);
	return false;
}

OptionEntryFixed::OptionEntryFixed(std::list<OptionEntry *> &group,
							   const char *name,
							   const char *description,
							   unsigned int data,
							   fixed value) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

OptionEntryFixed::~OptionEntryFixed()
{

}

const char *OptionEntryFixed::getValueAsString()
{
	return value_.asString();
}

const char *OptionEntryFixed::getDefaultValueAsString()
{
	return defaultValue_.asString();
}

bool OptionEntryFixed::setValueFromString(const std::string &string)
{
	return setValue(fixed(string.c_str()));
}

fixed OptionEntryFixed::getValue() 
{
	return value_;
}

bool OptionEntryFixed::setValue(fixed value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryFixed::addToArgParser(ARGParser &parser)
{
	DIALOG_ASSERT(0);
	return false;
}

OptionEntryFixedVector::OptionEntryFixedVector(std::list<OptionEntry *> &group,
							   const char *name,
							   const char *description,
							   unsigned int data,
							   FixedVector value) :
	OptionEntry(group, name, description, data), 
	value_(value), defaultValue_(value)
{
	
}

OptionEntryFixedVector::~OptionEntryFixedVector()
{

}

const char *OptionEntryFixedVector::getValueAsString()
{
	std::string a = value_[0].asString();
	std::string b = value_[1].asString();
	std::string c = value_[2].asString();

	static char value[256];
	snprintf(value, 256, "%s %s %s", 
		a.c_str(), b.c_str(), c.c_str());
	return value;
}

const char *OptionEntryFixedVector::getDefaultValueAsString()
{
	std::string a = defaultValue_[0].asString();
	std::string b = defaultValue_[1].asString();
	std::string c = defaultValue_[2].asString();

	static char value[256];
	snprintf(value, 256, "%s %s %s", 
		a.c_str(), b.c_str(), c.c_str());
	return value;
}

bool OptionEntryFixedVector::setValueFromString(const std::string &string)
{
	FixedVector value;

	int i=0;
	char *token = strtok((char *) string.c_str(), " ");
	while(token != 0)
	{
		value[i++] = fixed(token);		
		token = strtok(0, " ");
	}

	return setValue(value);
}

FixedVector &OptionEntryFixedVector::getValue() 
{
	return value_;
}

bool OptionEntryFixedVector::setValue(FixedVector value)
{
	changedValue_ = true;
	value_ = value;
	return true;
}

bool OptionEntryFixedVector::addToArgParser(ARGParser &parser)
{
	DIALOG_ASSERT(0);
	return false;
}
