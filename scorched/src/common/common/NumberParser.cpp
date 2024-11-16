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

#include <common/RandomGenerator.h>
#include <common/NumberParser.h>
#include <engine/Simulator.h>
#include <string>
#include <list>
#include <sstream>

NumberParser::NumberParser(const char *valueName) :
	valueName_(valueName)
{

}

NumberParser::NumberParser(const char *valueName, fixed value) :
	valueName_(valueName)
{
	setExpression(value);
}

NumberParser::~NumberParser()
{

}


bool NumberParser::getOperands()
{
	operands_.clear();
	size_t nextPos = 0;
        std::string value;
        size_t pos = expression_.find('(',0);
	if (pos == std::string::npos)
	{
		//value = expression_.substr(pos + 1, nextPos - pos + 1);
		operands_.push_back(fixed(expression_.c_str()));
		return true;
	}

	pos += 1;
        while (pos < expression_.length())
        {
                nextPos = expression_.find_first_of(",)", pos);
                if (nextPos == std::string::npos)
                        nextPos = expression_.length() -1;
                value = expression_.substr(pos, nextPos - pos);
                operands_.push_back(fixed(value.c_str()));
                pos = nextPos + 1;
        }
        return true;
}


bool NumberParser::setExpression(const char *expression)
{
	expression_ = expression;
	// test to ensure it's valid! TODO
	this->getOperands();

	return true;
}

bool NumberParser::setExpression(fixed value)
{
	expression_ = value.asString();
	this->getOperands();

	return true;
}

fixed NumberParser::getValue(ScorchedContext &context) //RandomGenerator &generator)
{
        // Examples: 10, RANGE(1,10)
        fixed value = 0;
        // Constant
        if (operands_.size() == 1)
                return operands_.front();

        step_ = 0;
        std::list<fixed>::iterator itor;
        itor = operands_.begin();
		RandomGenerator &random = context.getSimulator().getRandomGenerator();

        if (expression_.find("RANGE",0) != std::string::npos)
        {
                min_ = *itor;
                max_ = *(++itor);
                if (operands_.size() >= 3)
                        step_ = *(++itor);

                if (step_ == 0)
                        value = random.getRandFixed(valueName_) * (max_ - min_) + min_;
                else
					value = fixed(((random.getRandFixed(valueName_) * (max_ - min_) /  step_ ).asInt()) * step_.asInt()) + min_;
                return value;
        }

        else if (expression_.find("DISTRIBUTION",0) != std::string::npos)
        {
                if (operands_.size() < 2)
        		S3D::dialogExit("NumberParser.cpp",
		                S3D::formatStringBuffer("Invalid DISTRIBUTION expression: \"%s\"",
				expression_.c_str()));
				int operandNo = (random.getRandFixed(valueName_) * fixed((unsigned int) operands_.size())).asInt();
                for (int i = 0; i <= operandNo; i++) ++itor;
                value = *itor;
                return value;
        }

        S3D::dialogExit("NumberParser.cpp",
	        S3D::formatStringBuffer("Invalid fixed expression: \"%s\"",
                expression_.c_str()));
	return false;  // VC++ complains
}

// Allow for default values to be passed along for optional attributes
fixed NumberParser::getValue(ScorchedContext &context, fixed defaultValue)
{
	if (expression_.size() == 0)
		this->setExpression(defaultValue);

	return getValue(context);
}

unsigned int NumberParser::getUInt(ScorchedContext &context)
{
	fixed result = getValue(context);
	return result.asInt();
}

int NumberParser::getInt(ScorchedContext &context)
{
	fixed result = getValue(context);
	return result.asInt();
}

