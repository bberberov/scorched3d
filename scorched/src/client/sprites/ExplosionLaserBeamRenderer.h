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

#if !defined(__INCLUDE_ExplosionLaserRendererh_INCLUDE__)
#define __INCLUDE_ExplosionLaserRendererh_INCLUDE__

#include <sprites/MetaActionRenderer.h>
#include <GLEXT/GLTextureReference.h>

#define sides 8
#define layers 5
#define star_interval 10

class ExplosionLaserBeamRenderer : public MetaActionRenderer
{
public:
	ExplosionLaserBeamRenderer();
	virtual ~ExplosionLaserBeamRenderer();

	virtual void init(unsigned int playerId,
		Vector &position, Vector &velocity,
		const char *data);

	virtual void draw(Action *action);
	virtual void simulate(Action *action, float frameTime, bool &remove);

	REGISTER_CLASS_HEADER(ExplosionLaserBeamRenderer);
private:

	Vector position_;
	float size_;
	float time_;
	float totalTime_;
	Vector points[layers][sides];
	float angle_;
	static GLTextureReference _texture;

};
#endif
