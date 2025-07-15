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

#if !defined(__INCLUDE_ProgressDialogh_INCLUDE__)
#define __INCLUDE_ProgressDialogh_INCLUDE__

#include <common/ProgressCounter.h>
#include <common/FileLines.h>
#include <GLW/GLWWindow.h>
#include <GLEXT/GLTexture.h>

class ScorchedContext;
class ProgressDialog : 
	public GLWWindow,
	public ProgressCounterI,
	public ProgressCounter
{
public:
	static ProgressDialog *instance();

	virtual void operationChange(const LangString &op);
	virtual void progressChange(const LangString &op, const float percentage);
	virtual void draw();

	void changeTip();
	void setIcon(Image iconName);

protected:
	static ProgressDialog *instance_;

	GLTexture icon_;
	GLTexture bar1_, bar2_;
	FileLines tips_;
	LangString progressText_;
	float progressPercentage_;

	void drawRules(ScorchedContext &context);

private:
	ProgressDialog();
	virtual ~ProgressDialog();
};

class ProgressDialogSync :
	public ProgressCounterI,
	public ProgressCounter
{
public:
	static ProgressDialogSync *events_instance();
	static ProgressDialogSync *noevents_instance();

	virtual void operationChange(const LangString &op);
	virtual void progressChange(const LangString &op, const float percentage);
protected:
	bool processEvents_;

private:
	ProgressDialogSync(bool processEvents);
	virtual ~ProgressDialogSync();
};

#endif
