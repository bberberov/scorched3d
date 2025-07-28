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

#if !defined(AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_)
#define AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_

#include <engine/GameStateI.h>
#include <common/Vector.h>

class GLCameraFrustum : public GameStateI
{
public:
	static GLCameraFrustum *instance();

	static Vector FrustrumRed;
	static Vector FrustrumBlue;
	static Vector FrustrumGreen;
	static Vector FrustrumWhite;

	virtual void draw(const unsigned state);

	bool sphereInFrustum(Vector &point, float fRadius = 1, Vector &color = FrustrumWhite);
	bool sphereInFrustumThreadSafe(Vector &point, float fRadius);
	void backupFrustum();
	void restoreFrustum();
	void drawBilboard(
		Vector &position, Vector &color, float alpha,
		float width, float height, bool additive, int texCoord);

	Vector &getBilboardVectorX();
	Vector &getBilboardVectorY();

	float *getBilboardMatrix() { return s.fBilboard; }
	float *getViewMatrix() { return s.fView; }

protected:
	static GLCameraFrustum *instance_;
	struct Settings
	{
		float frustum_[6][4];
		float fProj[16];
		float fView[16];
		float fClip[16];
		float fBilboard[16];
		float viewport[4];
		float aspect;
	} s, b;

	void normalize(float vector[4]);

private:
	GLCameraFrustum();
	virtual ~GLCameraFrustum();

};

#endif // !defined(AFX_GLCAMERAFRUSTUM_H__4C341296_0589_46D5_BFD7_3114534F7E2D__INCLUDED_)
