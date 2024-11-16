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

#if !defined(__INCLUDE_RenderTracerh_INCLUDE__)
#define __INCLUDE_RenderTracerh_INCLUDE__

#include <list>
#include <map>
#include <common/Vector.h>
#include <GLEXT/GLState.h>
#include <engine/GameStateI.h>

class Tank;
class RenderTracer : public GameStateI
{
public:
	static RenderTracer *instance();

	struct TracerLinePoint
	{
		Vector position;
		Vector cross;
	};

	virtual void draw(const unsigned state);

	void drawTracerEnd(Vector &position);
	void drawSmokeTracer(std::list<TracerLinePoint> &positions);

	void newGame();
	void clearTracers();
	void clearTracerLines();
	void addTracer(unsigned int tank, 
		Vector &position);
	void addSmokeTracer(unsigned int tank, 
		Vector &position, std::list<TracerLinePoint> &positions);

protected:
	static RenderTracer *instance_;
	class TraceEntry
	{
	public:
		TraceEntry(unsigned int t = 0) : tank(t) {}

		unsigned int tank;
		std::list<Vector> points;
		std::list<std::list<TracerLinePoint> > lines;
	};

	std::map<unsigned int, TraceEntry> traceEntries_;
	TraceEntry *current_;
	GLUquadric *obj_;
	GLuint listNo_;

private:
	RenderTracer();
	virtual ~RenderTracer();

};

#endif
