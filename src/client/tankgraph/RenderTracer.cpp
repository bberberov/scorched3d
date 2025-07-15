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

#include <tankgraph/RenderTracer.h>
#include <target/TargetContainer.h>
#include <GLEXT/GLTexture.h>
#include <client/ScorchedClient.h>
#include <tank/Tank.h>

RenderTracer *RenderTracer::instance_ = 0;

RenderTracer *RenderTracer::instance()
{
	if (!instance_) instance_ = new RenderTracer;
	return instance_;
}

RenderTracer::RenderTracer() : 
	GameStateI("RenderTracer"),
	current_(0), listNo_(0)
{
	obj_ = gluNewQuadric();
}

RenderTracer::~RenderTracer()
{
	gluDeleteQuadric(obj_);
}

void RenderTracer::clearTracers()
{
	if (current_) 
	{
		current_->lines.clear();
		current_->points.clear();
	}
}

void RenderTracer::clearTracerLines()
{
	if (current_)
	{
		current_->lines.clear();
	}
}

void RenderTracer::draw(const unsigned state)
{
	Tank *current = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current) return;

	if (!current_ || 
		current_->tank != current->getPlayerId())
	{
		std::map<unsigned int, TraceEntry>::iterator itor = 
			traceEntries_.find(current->getPlayerId());
		if (itor == traceEntries_.end())
		{
			TraceEntry entry(current->getPlayerId());
			traceEntries_[current->getPlayerId()] = entry;
			current_ = &traceEntries_[current->getPlayerId()];
		}
		else
		{
			current_ = &(*itor).second;
		}
	}
	if (current_->points.empty()) return;

	glColor3fv(current->getColor());
	
	std::list<Vector>::iterator itor = current_->points.begin();
	std::list<Vector>::iterator itorend = current_->points.end();
	for (;itor != itorend; ++itor)
	{
		drawTracerEnd(*itor);
	}

	std::list<std::list<TracerLinePoint> >::iterator itor2 = current_->lines.begin();
	std::list<std::list<TracerLinePoint> >::iterator itorend2 = current_->lines.end();
	for (;itor2 != itorend2; ++itor2)
	{
		drawSmokeTracer(*itor2);
	}
}

void RenderTracer::drawTracerEnd(Vector &position)
{
	GLState currentState(GLState::TEXTURE_OFF | GLState::BLEND_OFF);

	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		if (!listNo_)
		{
			glNewList(listNo_ = glGenLists(1), GL_COMPILE);
				gluSphere(obj_, 0.5f, 4, 2);
			glEndList();
		}
		glCallList(listNo_);
	glPopMatrix();
}

void RenderTracer::drawSmokeTracer(std::list<TracerLinePoint> &positions)
{
	GLState currentState(GLState::TEXTURE_OFF | GLState::BLEND_OFF);
	/*glEnable(GL_ALPHA_TEST);

	static GLTexture arrowTexture;
	if (!arrowTexture.textureValid())
	{
		std::string file1 = S3D::getModFile("data/windows/arrow.bmp");
		std::string file2 = S3D::getModFile("data/windows/arrowi.bmp");
		ImageBitmap bitmap(file1.c_str(), file2.c_str(), true);
		arrowTexture.create(bitmap);
	}
	arrowTexture.draw();*/

	// Draw twice (for front and back facing)
	for (int i=0; i<2; i++)
	{
		glBegin(GL_QUAD_STRIP);
		Vector lastPos;
		float totalDist = 0.0f;
		std::list<TracerLinePoint>::iterator itor = positions.begin();
		std::list<TracerLinePoint>::iterator itorend = positions.end();
		for (;itor != itorend; ++itor)
		{
			Vector &currentPos = (*itor).position;
			Vector &cross = (*itor).cross;
			if (itor == positions.begin()) lastPos = currentPos;

			float dist = (lastPos - currentPos).Magnitude();
			if (dist < 100.0f)
			{
				if (i==0)
				{
					glTexCoord2f(0.0f, totalDist);
					glVertex3fv(currentPos - cross / 2.0f);
					glTexCoord2f(1.0f, totalDist);
					glVertex3fv(currentPos + cross / 2.0f);
				}
				else
				{
					glTexCoord2f(0.0f, totalDist);
					glVertex3fv(currentPos + cross / 2.0f);
					glTexCoord2f(1.0f, totalDist);
					glVertex3fv(currentPos - cross / 2.0f);
				}
			}
			else
			{
				glEnd();
				glBegin(GL_QUAD_STRIP);
			}

			totalDist += dist / 10.0f;
			lastPos = currentPos;
		}
		glEnd();
	}
	//glDisable(GL_ALPHA_TEST);
}

void RenderTracer::newGame()
{
	traceEntries_.clear();
	current_ = 0;
}

void RenderTracer::addTracer(unsigned int tank, Vector &position)
{
	std::map<unsigned int, TraceEntry>::iterator itor = 
		traceEntries_.find(tank);
	if (itor == traceEntries_.end())
	{
		TraceEntry entry(tank);
		entry.points.push_back(position);
		traceEntries_[tank] = entry;
	}
	else
	{
		(*itor).second.points.push_back(position);
	}
}

void RenderTracer::addSmokeTracer(unsigned int tank, 
	Vector &position, std::list<TracerLinePoint> &positions)
{
	std::map<unsigned int, TraceEntry>::iterator itor = 
		traceEntries_.find(tank);
	if (itor == traceEntries_.end())
	{
		TraceEntry entry(tank);
		entry.lines.push_back(positions);
		entry.points.push_back(position);
		traceEntries_[tank] = entry;
	}
	else
	{
		(*itor).second.lines.push_back(positions);
		(*itor).second.points.push_back(position);
	}
}
