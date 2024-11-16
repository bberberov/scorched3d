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

#include <stdlib.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/process.h>
#include <wx/txtstrm.h>
#include <wx/msgdlg.h>
#include <wx/dcbuffer.h>
#include <wx/splash.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <SDL/SDL.h>
#include <wxdialogs/MainDialog.h>
#include <wxdialogs/DisplayDialog.h>
#include <wxdialogs/ServerSDialog.h>
#include <wxdialogs/TrueTypeFont.h>
#include <scorched/ScorchedParams.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>

extern char scorched3dAppName[128];
static wxFrame *mainDialog = 0;
bool wxWindowExit = false;

enum
{
	ID_MAIN_TIMER
};

wxString convertString(const std::string &input)
{
	wxString result(input.c_str(), wxConvUTF8);
	return result;
}

void addTitleToWindow(
	wxWindow *parent,
	wxSizer *sizer,
	const char *fileName,
	int buttonId)
{
	wxBitmap scorchedBitmap;
	if (scorchedBitmap.LoadFile(wxString(fileName, wxConvUTF8), 
		wxBITMAP_TYPE_BMP) &&
		scorchedBitmap.Ok())
	{
		wxBitmapButton *button = new wxBitmapButton(
			parent, buttonId, scorchedBitmap);
		wxBoxSizer *boxSizer = new wxBoxSizer(wxHORIZONTAL);
		boxSizer->Add(button, 0, wxALL, 5);
		sizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
	}
}

static SDL_mutex *messageMutex_ = 0;
static std::string messageString_;
static int exitCode_ = 0;

class ScorchedProcess : public wxProcess
{
public:
	ScorchedProcess(bool server) : 
		wxProcess(!server?wxPROCESS_REDIRECT:0),
		server_(server)
	{ 
		if (server_) serverProcessesRunning_++;
		else clientProcessesRunning_++;
	}
	
	virtual void OnTerminate(int pid, int status) 
	{
		if (server_) serverProcessesRunning_--;
		else clientProcessesRunning_--;

		if (status != 0)
		{
			SDL_LockMutex(messageMutex_);
			if (server_) exitCode_ = 64; // So it doesn't say to load failsafe
			else exitCode_ = status;

			if (status != 64)
			{
				messageString_ = "The Scorched3d process "
					"terminated unexpectedly.\n";
			}
			else
			{
				messageString_ = "The Scorched3d process "
					"terminated due to configuration errors.\n";
			}
			while (IsInputAvailable())
			{
				wxTextInputStream tis(*GetInputStream());
				wxString line = tis.ReadLine();
				messageString_.append((const char *) line.mb_str(wxConvUTF8));
				messageString_.append("\n");
			}
			SDL_UnlockMutex(messageMutex_);
		}
		Detach();
		wxProcess::OnTerminate(pid, status);
	}

	static unsigned int getClientProcessesRunning()
	{
		return clientProcessesRunning_;
	}

	static unsigned int getServerProcessesRunning()
	{
		return serverProcessesRunning_;
	}

protected:
	bool server_;
	static unsigned int clientProcessesRunning_;
	static unsigned int serverProcessesRunning_;
};

unsigned int ScorchedProcess::clientProcessesRunning_(0);
unsigned int ScorchedProcess::serverProcessesRunning_(0);

void runScorched3D(const char *text, bool server)
{
	if (!server &&
		ScorchedProcess::getClientProcessesRunning() > 0)
	{
		if (::wxMessageBox(
			wxT("You are already running the game, do you wish to run another copy?"),
			wxT("Scorched3D"),
			wxCANCEL | wxOK | wxICON_EXCLAMATION) == wxCANCEL)
		{
			return;
		}
	}

	std::string exeName = S3D::getExeName();
	const char *exePart = strstr(exeName.c_str(), ".exe");
	if (exePart) ((char *)exePart)[0] = '\0';
	exePart = strstr(exeName.c_str(), ".EXE");
	if (exePart) ((char *)exePart)[0] = '\0';

	char path[1024];
	snprintf(path, 1024, "\"%s%s%s\" %s -settingsdir %s %s", 
		exeName.c_str(), 
		(server?"s":"c"),
		(exePart?".exe":""),
		(ScorchedParams::instance()->getAllowExceptions()?" -allowexceptions":""),
		ScorchedParams::instance()->getSettingsDir(), 
		text);

	ScorchedProcess *process = new ScorchedProcess(server);
	long result = ::wxExecute(wxString(path, wxConvUTF8), wxEXEC_ASYNC, process);
	if (result == 0)
	{
		delete process;
		S3D::dialogMessage(scorched3dAppName, S3D::formatStringBuffer(
			"Error: Failed to execute scorched3d using commandline :-\n"
			"%s",
			path));
	}
}

wxButton *addButtonToWindow(
	int id,
	const char *text,
	const char *bitmapName,
	wxWindow *parent,
	wxSizer *sizer,
	wxObjectRefData *data)
{
	wxButton *button = 0;
	wxBitmap bitmap;
	if (bitmap.LoadFile(wxString(bitmapName, wxConvUTF8), wxBITMAP_TYPE_BMP) &&
		bitmap.Ok())
	{
		button = new wxBitmapButton(parent, id, bitmap);
	}
	else
	{
		button = new wxButton(parent, id, wxT("Select"));
	}
	if (data) button->SetRefData(data);

	wxStaticText *staticText = new wxStaticText(
		parent, -1, 
		wxString(text, wxConvUTF8));

	sizer->Add(button, 0, wxRIGHT, 5);
	sizer->Add(staticText, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

	return button;
}

class MainFrame: public wxFrame
{
public:
	MainFrame();

	void onTimer(wxTimerEvent &event);
	void onPaint(wxPaintEvent& event);
	void onMotion(wxMouseEvent &event);	
	void onEraseBackground(wxEraseEvent &event);

	void onDisplayButton();
	void onQuitButton();
	void onSingleButton();
	void onServerButton();
	void onDonateClick();
	void onHelpButton();

private:
	DECLARE_EVENT_TABLE()

	struct ImageData
	{
		int x, y;
		wxImage loadedImage;
		wxImage descriptionImage;
		wxBitmap cachedBitmap1;
		wxBitmap cachedBitmap2;
		wxBitmap cachedDescription;
	};

	wxTimer timer_;
	wxBitmap backdropBitmap_;
	wxImage backdropImage_;
	std::list<ImageData *> images_;
	long mouseX_, mouseY_;
	int lastPos_;

	void generateCachedImage(int x, int y, 
		wxImage &src, wxBitmap &destBitamp, 
		bool highlight = false);
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_TIMER(ID_MAIN_TIMER, MainFrame::onTimer)
	EVT_PAINT(MainFrame::onPaint) 
	EVT_MOUSE_EVENTS(MainFrame::onMotion)
	EVT_ERASE_BACKGROUND(MainFrame::onEraseBackground)
END_EVENT_TABLE()

MainFrame::MainFrame() :
	wxFrame((wxFrame *)NULL, -1, wxString(scorched3dAppName, wxConvUTF8), 
		wxDefaultPosition, wxDefaultSize,
		wxMINIMIZE_BOX | wxCAPTION | wxSYSTEM_MENU),
	mouseX_(0), mouseY_(0), lastPos_(-1)
{
	if (!messageMutex_) messageMutex_ = SDL_CreateMutex();

	// Set the frame's icon
	wxIcon icon(convertString(S3D::getDataFile("data/images/tank2.ico")), wxBITMAP_TYPE_ICO);
	SetIcon(icon);

#if wxCHECK_VERSION(2, 6, 0)
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
#endif

	// Load the backdrop bitmaps
	if (!backdropImage_.LoadFile(
		convertString(S3D::getDataFile("data/images/backdrop.gif")), 
		wxBITMAP_TYPE_GIF))
	{
		S3D::dialogMessage("Scorched", "Failed to load backdrop");
	}
	backdropBitmap_ = wxBitmap(backdropImage_, -1);

	// Load all of the button bitmaps
	struct ImageDefinition
	{
		const char *name;
		const char *description;
		int x, y;
	} imageDefinitions[] = {
		"Play", "- Play a game.", 30, 150,
		"Start Server", "- Start a LAN or internet server.", 30, 180,
		"Settings", "- Change the display, sound or other settings.", 30, 210,
		"Help", "- View the online help.", 30, 260,
		"Donate", "- Show support for Scorched3D.", 30, 290,
		"Quit", "- Exit the game.", 30, 340
	};

	TrueTypeFont largeImageFont(S3D::getDataFile("data/fonts/dejavusans.ttf"), 14);
	TrueTypeFont smallImageFont(S3D::getDataFile("data/fonts/dejavusans.ttf"), 12);

	for (int i=0; i<sizeof(imageDefinitions) / sizeof(ImageDefinition); i++)
	{
		ImageData *image = new ImageData();
		image->x = imageDefinitions[i].x;
		image->y = imageDefinitions[i].y;

		if (!largeImageFont.getImageForText(imageDefinitions[i].name, image->loadedImage) ||
			!smallImageFont.getImageForText(imageDefinitions[i].description, image->descriptionImage))
		{
			S3D::dialogMessage("Scorched", 
				S3D::formatStringBuffer("Failed to load button %s", imageDefinitions[i].name));
		}
		else
		{
			images_.push_back(image);
		}
	}

	// Setup timer
	timer_.SetOwner(this, ID_MAIN_TIMER);
	timer_.Start(1000, false);

	// use the sizer for layout
	// Create the positioning sizer
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
	topsizer->SetMinSize(533, 400);
	SetSizer(topsizer); 
	topsizer->SetSizeHints(this); // set size hints to honour minimum size

	CentreOnScreen();
}

void MainFrame::onMotion(wxMouseEvent &event)
{
	mouseX_ = event.m_x;
	mouseY_ = event.m_y;

	int foundPos = -1;
	int pos = 0;
	std::list<ImageData *>::iterator itor;
	for (itor = images_.begin();
		itor != images_.end();
		++itor, pos++)
	{
		ImageData *imageData = (*itor);
		if (mouseX_ > imageData->x &&
			mouseY_ > imageData->y &&
			mouseX_ < imageData->x + imageData->cachedBitmap1.GetWidth() &&
			mouseY_ < imageData->y + imageData->cachedBitmap1.GetHeight())
		{
			foundPos = pos;
		}
	}

	if (event.ButtonDown() &&
		foundPos != -1)
	{
		switch (foundPos)
		{
		case 0:
			onSingleButton();
			break;
		case 1:
			onServerButton();
			break;
		case 2:
			onDisplayButton();
			break;
		case 3:
			onHelpButton();
			break;
		case 4:
			onDonateClick();
			break;
		case 5:
			onQuitButton();
			break;
		}
	}

	if (lastPos_ != foundPos)
	{
		lastPos_ = foundPos;
		Refresh();
	}
}

void MainFrame::generateCachedImage(int x, int y, 
	wxImage &src, wxBitmap &destBitamp,
	bool highlight)
{
	wxImage dest(
		src.GetWidth(),
		src.GetHeight());

	unsigned char *backdropdata = backdropImage_.GetData();
	backdropdata += 
		3 * x +
		3 * y * backdropImage_.GetWidth();
	
	unsigned char *srcdata = src.GetData();
	unsigned char *destdata = dest.GetData();
	for (int y=0; y<src.GetHeight(); y++)
	{
		unsigned char *backdropstart = backdropdata;
		for (int x=0; x<src.GetWidth(); x++)
		{
			float alpha = srcdata[0] + srcdata[1] + srcdata[2];
			alpha /= 255.0f + 255.0f + 255.0f;

			float mult = (highlight?1.5f:1.0f);
			float src0 = float(srcdata[0]) * mult; if (src0 > 255.0f) src0 = 255.0f;
			float src1 = float(srcdata[1]) * mult; if (src1 > 255.0f) src1 = 255.0f;
			float src2 = float(srcdata[2]) * mult; if (src2 > 255.0f) src2 = 255.0f;
			destdata[0] = (unsigned char) ((1.0f - alpha) * float(backdropdata[0]) + alpha * float(src0));
			destdata[1] = (unsigned char) ((1.0f - alpha) * float(backdropdata[1]) + alpha * float(src1));
			destdata[2] = (unsigned char) ((1.0f - alpha) * float(backdropdata[2]) + alpha * float(src2));

			srcdata += 3;
			destdata += 3;
			backdropdata += 3;
		}
		backdropdata = backdropstart + 3 * backdropImage_.GetWidth();
		
	}

	destBitamp = wxBitmap(dest, -1);
}

void MainFrame::onPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	dc.DrawBitmap(backdropBitmap_, 0, 0, false);

	// So its easy to display an alpha blended bitmap in wxWindows then!!!
	std::list<ImageData *>::iterator itor;
	for (itor = images_.begin();
		itor != images_.end();
		++itor)
	{
		ImageData *imageData = (*itor);

		if (!imageData->cachedBitmap1.Ok())
		{
			generateCachedImage(imageData->x, imageData->y, 
				imageData->loadedImage, imageData->cachedBitmap1, false);
			generateCachedImage(imageData->x, imageData->y, 
				imageData->loadedImage, imageData->cachedBitmap2, true);
			generateCachedImage(imageData->x + 135, imageData->y + 2, 
				imageData->descriptionImage, imageData->cachedDescription, true);
		}

		if (mouseX_ > imageData->x &&
			mouseY_ > imageData->y &&
			mouseX_ < imageData->x + imageData->cachedBitmap1.GetWidth() &&
			mouseY_ < imageData->y + imageData->cachedBitmap1.GetHeight())
		{
			dc.DrawBitmap(imageData->cachedBitmap2, imageData->x, imageData->y, false);
			dc.DrawBitmap(imageData->cachedDescription, imageData->x + 135, imageData->y + 2, false);
		}
		else
		{
			dc.DrawBitmap(imageData->cachedBitmap1, imageData->x, imageData->y, false);
		}
	}
}

void MainFrame::onEraseBackground(wxEraseEvent &event)
{
}

void MainFrame::onTimer(wxTimerEvent &event)
{
	std::string newString;
	SDL_LockMutex(messageMutex_);
	if (!messageString_.empty())
	{
		newString = messageString_;
		messageString_ = "";
	}
	SDL_UnlockMutex(messageMutex_);

	if (!newString.empty())
	{
		if (exitCode_ != 64)
		{
			newString.append("\n"
				"Would you like to load the failsafe "
				"scorched3d settings?\n"
				"This gives the best chance of working but "
				"at the cost of graphical detail.\n"
				"You can adjust this later in the Scorched3D "
				"display settings dialog.\n"
				"Note: Most problems can be fixed by using "
				"the very latest drivers\n"
				"for your graphics card.");
			int answer = ::wxMessageBox(
				wxString(newString.c_str(), wxConvUTF8),
				wxT("Scorched3D Abnormal Termination"),
				wxYES_NO | wxICON_ERROR);
			if (answer == wxYES)
			{
				OptionsDisplay::instance()->loadSafeValues();
				OptionsDisplay::instance()->writeOptionsToFile(ScorchedParams::instance()->getWriteFullOptions());
			}
		}
		else
		{
			::wxMessageBox(
				wxString(newString.c_str(), wxConvUTF8),
				wxT("Scorched3D Termination"),
				wxICON_ERROR);
		}
	}
}

void MainFrame::onDonateClick()
{
	const char *exec = 
		"\"https://www.paypal.com/xclick/business=donations%40"
		"scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP\"";
	S3D::showURL(exec);
}

void MainFrame::onDisplayButton()
{
	showDisplayDialog();
}

void MainFrame::onSingleButton()
{
	runScorched3D("", false);
}

void MainFrame::onServerButton()
{
	showServerSDialog();
}

void MainFrame::onQuitButton()
{
	wxWindowExit = true;
	Close();
}

void MainFrame::onHelpButton()
{
	S3D::showURL("http://www.scorched3d.co.uk/wiki");
}

extern bool newVersion;
void showMainDialog()
{
	mainDialog = new MainFrame;
	mainDialog->Show(TRUE);

	if (newVersion)
	{
		wxBitmap bitmap;
		if (bitmap.LoadFile(convertString(S3D::getDataFile("data/images/splash.gif")), 
			wxBITMAP_TYPE_GIF))
		{
			new wxSplashScreen(bitmap,
				wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
				9000, NULL, -1, wxDefaultPosition, wxDefaultSize,
				wxSIMPLE_BORDER | wxSTAY_ON_TOP);
		}
	}
}

wxFrame *getMainDialog()
{
	return mainDialog;
}
