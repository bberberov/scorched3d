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

#include <common/ProgressCounter.h>
#include <common/Logger.h>
#include <lang/LangResource.h>

ProgressCounterI::~ProgressCounterI()
{

}

ProgressCounter::ProgressCounter(ProgressCounterI *user) : 
	currentOp_(LANG_RESOURCE("NONE", "None")), 
	currentPercentage_(0.0f), user_(user)
{

}

ProgressCounter::~ProgressCounter()
{

}

void ProgressCounter::setNewOp(const LangString &op)
{
	currentOp_ = op;
	Logger::log(LangStringUtil::convertFromLang(op));
	if (user_) user_->operationChange(currentOp_);
}

void ProgressCounter::setNewPercentage(float percentage)
{
	currentPercentage_ = percentage;
	if (user_) user_->progressChange(currentOp_, currentPercentage_);
}
