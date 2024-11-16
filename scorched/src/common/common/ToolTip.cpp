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

#include <common/ToolTip.h>

unsigned int ToolTip::nextId_ = 0;

ToolTipI::~ToolTipI()
{

}

ToolTip::ToolTip(unsigned int type, const LangString &tit, const LangString &tex) 
	: id_(++nextId_), handler_(0)
{
	setText(type, tit, tex);
}

ToolTip::~ToolTip()
{
}

void ToolTip::populate()
{
	if (handler_) handler_->populateCalled(id_);
}

void ToolTip::setText(unsigned int type, 
	const LangString &title, const LangString &text)
{
	type_ = type; title_ = title; text_ = text;
}
