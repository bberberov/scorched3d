////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_MainDialog_hpp_INCLUDE__
#define __INCLUDE_MainDialog_hpp_INCLUDE__

// NOTE: avoid -Wmissing-declarations -Wunused-const-variable -Wuseless-cast
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"

#include <wx/wx.h>

#pragma GCC diagnostic pop

#include <string>

void showMainDialog();
void runScorched3D( const char* flags, bool server );

wxString convertString( const std::string& input );

wxFrame* getMainDialog();
void     addTitleToWindow( wxWindow* parent, wxSizer* sizer, const char* fileName, int buttonId = -1 );

// NOTE: Inactive/unused function?
wxButton* addButtonToWindow(
	int              id,
	const char*      text,
	const char*      bitmapName,
	wxWindow*        parent,
	wxSizer*         sizer,
	wxObjectRefData* data = 0
);

#endif  // __INCLUDE_MainDialog_hpp_INCLUDE__
