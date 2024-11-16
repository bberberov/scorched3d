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


#if !defined(__INCLUDE_OptionEntryh_INCLUDE__)
#define __INCLUDE_OptionEntryh_INCLUDE__

#include <net/NetBuffer.h>
#include <common/Vector.h>
#include <common/FixedVector.h>
#include <common/ARGParser.h>
#include <XML/XMLParser.h>

class OptionEntry
{
public:
	enum StandardData
	{
		DataDepricated = 128,
		DataProtected = 256
	};
	enum EntryType
	{
		OptionEntryIntType,
		OptionEntryStringType,
		OptionEntryTextType,
		OptionEntryBoolType,
		OptionEntryFloatType,
		OptionEntryFixedType,
		OptionEntryVectorType,
		OptionEntryFixedVectorType,
		OptionEntryEnumType,
		OptionEntryStringEnumType,
		OptionEntryBoundedIntType
	};

	OptionEntry(std::list<OptionEntry *> &group,
				const char *name,
				const char *description,
				unsigned int data);
	virtual ~OptionEntry();

	const char *getName() { return name_.c_str(); }
	virtual const char *getDescription() { return description_.c_str(); }
	virtual const char *getRangeDescription() { return 0; }
	virtual unsigned getData() { return data_; }

	virtual EntryType getEntryType() = 0;
	virtual const char *getDefaultValueAsString() = 0;
	virtual const char *getValueAsString() = 0;
	virtual bool isDefaultValue() = 0;
	virtual bool isChangedValue() { return changedValue_; }
	virtual bool setValueFromString(const std::string &string) = 0;
	virtual bool addToArgParser(ARGParser &parser) = 0;
	virtual void setNotChanged() { changedValue_ = false; }

	virtual const char *getComsBufferValue() 
		{ return getValueAsString(); }
	virtual bool setComsBufferValue(const char *string) 
		{ return setValueFromString(string); }

protected:
	bool changedValue_;
	unsigned int data_;
	std::string name_;
	std::string description_;
};

class OptionEntryHelper
{
public:
	static bool writeToBuffer(std::list<OptionEntry *> &options,
							  NetBuffer &buffer,
							  bool useprotected);
	static bool readFromBuffer(std::list<OptionEntry *> &options,
							   NetBufferReader &reader,
							   bool useprotected);

	static bool writeToXML(std::list<OptionEntry *> &options,
								XMLNode *node, bool allOptions);
	static bool readFromXML(std::list<OptionEntry *> &options,
								XMLNode *node);

	static bool writeToFile(std::list<OptionEntry *> &options,
		const std::string &fileName, bool allOptions);
	static bool readFromFile(std::list<OptionEntry *> &options,
		const std::string &fileName);

	static bool addToArgParser(std::list<OptionEntry *> &options,
								ARGParser &parser);

	static OptionEntry *getEntry(std::list<OptionEntry *> &options,
		const char *name);
};

class OptionEntryInt : public OptionEntry, public ARGParserIntI
{
public:
	OptionEntryInt(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   int defaultValue);
	virtual ~OptionEntryInt();

	virtual EntryType getEntryType() { return OptionEntryIntType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual int getValue();
	virtual bool setValue(int value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setIntArgument(int value);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator int () { return value_; }

protected:
	int defaultValue_;
	int value_;

};

class OptionEntryBoundedInt : public OptionEntryInt
{
public:
	OptionEntryBoundedInt(std::list<OptionEntry *> &group,
						  const char *name, 
						  const char *description,
						  unsigned int data,
						  int defaultValue,
						  int minValue, int maxValue, int stepValue);
	virtual ~OptionEntryBoundedInt();

	virtual const char *getRangeDescription();
	virtual EntryType getEntryType() { return OptionEntryBoundedIntType; }
	virtual bool setValue(int value);

	int getMinValue() { return minValue_; }
	int getMaxValue() { return maxValue_; }
	int getStepValue() { return stepValue_; }

protected:
	int minValue_, maxValue_;
	int stepValue_;

};

class OptionEntryEnum : public OptionEntryInt
{
public:
	struct EnumEntry
	{
		const char *description;
		int value;
	};

	OptionEntryEnum(std::list<OptionEntry *> &group,
						  const char *name, 
						  const char *description,
						  unsigned int data,
						  int value,
						  OptionEntryEnum::EnumEntry enums[]);
	virtual ~OptionEntryEnum();

	virtual EntryType getEntryType() { return OptionEntryEnumType; }
	virtual bool setValue(int value);
	virtual const char *getRangeDescription();

	virtual const char *getDefaultValueAsString();
	virtual const char *getValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual const char *getComsBufferValue() 
		{ return OptionEntryInt::getValueAsString(); }
	virtual bool setComsBufferValue(const char *string) 
		{ return OptionEntryInt::setValueFromString(string); }

	OptionEntryEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class OptionEntryBool : public OptionEntry, public ARGParserBoolI
{
public:
	OptionEntryBool(std::list<OptionEntry *> &group,
					const char *name, 
					const char *description,
					unsigned int data,
					bool defaultValue);
	virtual ~OptionEntryBool();

	virtual EntryType getEntryType() { return OptionEntryBoolType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual bool setValue(bool value);
	virtual bool getValue();

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setBoolArgument(bool value);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator bool () { return value_; }

protected:
	bool defaultValue_;
	bool value_;
};

class OptionEntryString : public OptionEntry, public ARGParserStringI
{
public:
	OptionEntryString(std::list<OptionEntry *> &group,
					  const char *name,
					  const char *description,
					  unsigned int data,
					  const char *defaultValue,
					  bool multiline = false);
	virtual ~OptionEntryString();

	virtual EntryType getEntryType() { return (multiline_?OptionEntryTextType:OptionEntryStringType); }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual const char *getValue();
	virtual bool setValue(const std::string &value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setStringArgument(const char* value);

	operator const char *() { return value_.c_str(); }
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

protected:
	std::string value_;
	std::string defaultValue_;
	bool multiline_;

};

class OptionEntryStringEnum : public OptionEntryString
{
public:
	struct EnumEntry
	{
		const char *value;
	};

	OptionEntryStringEnum(std::list<OptionEntry *> &group,
						  const char *name, 
						  const char *description,
						  unsigned int data,
						  const char *value,
						  OptionEntryStringEnum::EnumEntry enums[]);
	virtual ~OptionEntryStringEnum();

	virtual EntryType getEntryType() { return OptionEntryStringEnumType; }
	virtual bool setValue(const std::string &value);
	virtual const char *getRangeDescription();

	virtual bool setValueFromString(const std::string &string);

	OptionEntryStringEnum::EnumEntry *getEnums() { return enums_; }

protected:
	EnumEntry *enums_;

};

class OptionEntryFloat : public OptionEntry
{
public:
	OptionEntryFloat(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   float defaultValue,
				   bool truncate = false);
	virtual ~OptionEntryFloat();

	virtual EntryType getEntryType() { return OptionEntryFloatType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual float getValue();
	virtual bool setValue(float value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator float () { return value_; }

protected:
	bool truncate_;
	float defaultValue_;
	float value_;

};

class OptionEntryVector : public OptionEntry
{
public:
	OptionEntryVector(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   Vector defaultValue,
				   bool truncate = false);
	virtual ~OptionEntryVector();

	virtual EntryType getEntryType() { return OptionEntryVectorType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual Vector &getValue();
	virtual bool setValue(Vector value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator Vector() { return value_; }

protected:
	bool truncate_;
	Vector defaultValue_;
	Vector value_;

};

class OptionEntryFixed : public OptionEntry
{
public:
	OptionEntryFixed(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   fixed defaultValue);
	virtual ~OptionEntryFixed();

	virtual EntryType getEntryType() { return OptionEntryFixedType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual fixed getValue();
	virtual bool setValue(fixed value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator fixed () { return value_; }

protected:
	fixed defaultValue_;
	fixed value_;

};

class OptionEntryFixedVector : public OptionEntry
{
public:
	OptionEntryFixedVector(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   FixedVector defaultValue);
	virtual ~OptionEntryFixedVector();

	virtual EntryType getEntryType() { return OptionEntryFixedVectorType; }
	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const std::string &string);

	virtual FixedVector &getValue();
	virtual bool setValue(FixedVector value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool isDefaultValue() { return value_ == defaultValue_; }

	operator FixedVector() { return value_; }

protected:
	FixedVector defaultValue_;
	FixedVector value_;

};

#endif
