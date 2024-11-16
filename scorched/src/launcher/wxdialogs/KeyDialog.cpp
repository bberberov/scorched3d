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

#include <wxdialogs/KeyDialog.h>
#include <wxdialogs/MainDialog.h>
#include <common/Defines.h>
#include <common/Keyboard.h>
#include <wx/wx.h>

extern char scorched3dAppName[128];
extern char *displayOptions;
static bool keyDialogShiftDown = false;
static bool keyDialogControlDown = false;
static bool keyDialogAltDown = false;
static unsigned int keyDialogKeyCode = 0;

class KeyFrame;
class TextWindow : public wxWindow
{
public:
    TextWindow(wxDialog *parent)
        : wxWindow(parent, -1, wxDefaultPosition, wxDefaultSize,
                   wxRAISED_BORDER),
		   frame_((KeyFrame *) parent)
    {
		wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

		wxStaticText *text = 
			new wxStaticText(this, -1, wxT("Press a key to bind to this function"));
		topsizer->Add(text, 0, wxALIGN_CENTER | wxALL, 5);

		SetSizer(topsizer); // use the sizer for layout
		topsizer->SetSizeHints(this); // set size hints to honour minimum size
    }

protected:
	KeyFrame *frame_;

    void OnKeyDown(wxKeyEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TextWindow, wxWindow)
    EVT_KEY_DOWN(TextWindow::OnKeyDown)
END_EVENT_TABLE()

class KeyFrame : public wxDialog
{
public:
	KeyFrame(wxDialog *);

protected:
	TextWindow *winText_;

	DECLARE_EVENT_TABLE()
};

unsigned int getKeyDialogKey()
{
	return keyDialogKeyCode;
}

bool getKeyDialogShift()
{
	return keyDialogShiftDown;
}

bool getKeyDialogControl()
{
	return keyDialogControlDown;
}

bool getKeyDialogAlt()
{
	return keyDialogAltDown;
}

void TextWindow::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case WXK_SHIFT:
	case WXK_CONTROL:
	case WXK_ALT:
	case WXK_MENU:
		break;
	default:
		keyDialogControlDown = event.ControlDown();
		keyDialogShiftDown = event.ShiftDown();
		keyDialogAltDown = event.AltDown();
		keyDialogKeyCode = event.GetKeyCode();
		frame_->EndModal(0);
	}
}

BEGIN_EVENT_TABLE(KeyFrame, wxDialog)
END_EVENT_TABLE()

KeyFrame::KeyFrame(wxDialog *dialog) :
	wxDialog(dialog, -1, wxString(scorched3dAppName, wxConvUTF8), 
		wxDefaultPosition, wxSize(190, 50), wxSIMPLE_BORDER )
{
#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(convertString(S3D::getDataFile("data/images/tank2.ico")), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	wxSize size = GetClientSize();
	winText_ = new TextWindow(this);
	winText_->SetSize(0, 0, size.x, size.y);

	CentreOnScreen();
}

void showKeyDialog(wxDialog *dialog)
{
	KeyFrame frame(dialog);
	frame.ShowModal();
}
