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

#if !defined(__INCLUDE_ComsOperationMessageh_INCLUDE__)
#define __INCLUDE_ComsOperationMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsOperationMessage : public ComsMessage
{
public:
	enum ComsOperationMessageType
	{
		OperationNone = 0,
		OperationTestSave = 1
	};

	static ComsMessageType ComsOperationMessageTyper;

	ComsOperationMessage(
		ComsOperationMessageType type = OperationNone, 
		const char *param1_ = "",
		const char *param2_ = "");
	virtual ~ComsOperationMessage();

	ComsOperationMessageType getType() { return type_; }
	const char *getParam1() { return param1_.c_str(); }
	const char *getParam2() { return param2_.c_str(); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	ComsOperationMessageType type_;
	std::string param1_;
	std::string param2_;

private:
	ComsOperationMessage(const ComsOperationMessage &);
	const ComsOperationMessage & operator=(const ComsOperationMessage &);

};

#endif // __INCLUDE_ComsOperationMessageh_INCLUDE__

