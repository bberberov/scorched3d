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


// GLLenseFlare.h: interface for the GLLenseFlare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLLENSEFLARE_H__34C2D198_3739_4402_B9FD_BEE4DF738466__INCLUDED_)
#define AFX_GLLENSEFLARE_H__34C2D198_3739_4402_B9FD_BEE4DF738466__INCLUDED_

#include <common/Vector.h>
#include <common/ProgressCounter.h>
#include <GLEXT/GLTexture.h>

class GLLenseFlare  
{
public:
	static GLLenseFlare *instance();
	void init(ProgressCounter *counter = 0);

	void draw(Vector &flarePos, bool fullFlare, 
		int color=0, float size = 1.0f, float alpha = 1.0f);
	void draw(Vector &flarePos, Vector &flareDirection, int color=0);

protected:
	struct Flare
	{
		int type;
		float scale;
		float loc;            /* postion on axis */
		Vector color1;
		Vector color2;
		Vector color3;
		Vector color4;
	} flare_[12];

	static bool flaresEnabled_;
	static GLLenseFlare *instance_;
	GLTexture flares_[6];
	GLTexture shines_[10];
	int shineTic_;

	void setFlare(int index, int type, float scale, float loc, 
		Vector &color1, Vector &color2, Vector &color3, Vector &color4, float colorScale);

private:
	GLLenseFlare();
	virtual ~GLLenseFlare();
};

#endif // !defined(AFX_GLLENSEFLARE_H__34C2D198_3739_4402_B9FD_BEE4DF738466__INCLUDED_)
