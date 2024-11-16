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


#if !defined(AFX_GLWTAB_H__5A717DEA_0AE4_4341_9991_A4575E3FF041__INCLUDED_)
#define AFX_GLWTAB_H__5A717DEA_0AE4_4341_9991_A4575E3FF041__INCLUDED_

#include <GLW/GLWScrollPanel.h>
#include <GLW/GLWLabel.h>

class GLWTabI
{
public:
	virtual ~GLWTabI();

	virtual void tabDown(unsigned int id) = 0;
};

class GLWTab : public GLWScrollPanel
{
public:
	GLWTab(const std::string &tabName = "", 
		const LangString &tabLabel = LangString(),
		float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h =  0.0f);
	virtual ~GLWTab();

	virtual void setParent(GLWPanel *parent);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);
	virtual void draw();
	virtual void setH(float h);

	float getTw();
	const char *getName() { return name_.c_str(); }
	
	void setDepressed();
	bool getDepressed() { return depressed_; }
	void setHandler(GLWTabI *handler) { handler_ = handler; }

	REGISTER_CLASS_HEADER(GLWTab);

protected:
	std::string name_;
	bool depressed_;
	GLWLabel label_;
	GLWTabI *handler_;
	float index_;

	void drawSurround();
	void drawNonSurround();
};

#endif // !defined(AFX_GLWTAB_H__5A717DEA_0AE4_4341_9991_A4575E3FF041__INCLUDED_)
