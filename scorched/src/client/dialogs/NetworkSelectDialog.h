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

#if !defined(__INCLUDE_NetworkSelectDialogh_INCLUDE__)
#define __INCLUDE_NetworkSelectDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWIconTable.h>
#include <GLW/GLWDropDownText.h>
#include <dialogs/TextBoxDialog.h>
#include <common/ToolTip.h>

class NetworkSelectDialog : 
	public GLWWindow,
	public GLWButtonI,
	public GLWIconTableI,
	public TextBoxDialogI,
	public GLWDropDownI
{
public:
	static NetworkSelectDialog *instance();

	virtual void draw();
	virtual void simulate(float frameTime);

	// GLWWindow
	virtual void display();
	virtual void hide();

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWIconTableI
	virtual void drawColumn(unsigned int id, int row, int column, float x, float y, float w);
	virtual void rowSelected(unsigned int id, int row);
	virtual void rowChosen(unsigned int id, int row);
	virtual void columnSelected(unsigned int id, int col);

	// GLWDropDownI
	virtual void select(unsigned int id, const int pos, GLWSelectorEntry value);

	// TextBoxDialogI
	virtual void textBoxResult(TextBoxDialog *dialog, const LangString &result);

protected:
	static NetworkSelectDialog *instance_;

	float totalTime_;
	GLTextureReference okTex_, questionTex_;
	GLTextureReference warningTex_, noentryTex_;
	GLTextureReference tankTex_, exclaimTex_;
	GLTextureReference keyTex_, cogTex_;
	GLWIconTable *gamesIconTable_;
	GLWIconTable *playersIconTable_;
	GLWTextButton *ok_, *refresh_, *favourites_, *connectTo_;
	GLWLabel *ipaddress_;
	GLWDropDownText *refreshType_;
	unsigned int invalidateId_;
	unsigned int cancelId_, addFavouriteId_;
	ToolTip colToolTip_;

	void updateTable();
	void startRefresh();
	void stopRefresh();
	bool serverCompatable(std::string pversion, std::string version);
	GLTexture *getTexture(int row, LangString *&message);
	void drawIcon(GLTexture *tex, float &x, float y, LangString &message);
	void setIPAddress(const LangString &text);

	void drawColumnGames(unsigned int id, int row, int column, float x, float y, float w);
	void drawColumnPlayers(unsigned int id, int row, int col, float x, float y, float w);
	void rowSelectedGames(unsigned int id, int row);
	void rowChosenGames(unsigned int id, int row);
	void columnSelectedGames(unsigned int id, int col);

private:
	NetworkSelectDialog();
	virtual ~NetworkSelectDialog();

};

#endif
