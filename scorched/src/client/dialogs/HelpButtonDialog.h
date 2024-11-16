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

#if !defined(__INCLUDE_HelpButtonDialogh_INCLUDE__)
#define __INCLUDE_HelpButtonDialogh_INCLUDE__

#include <GLEXT/GLTexture.h>
#include <GLEXT/GLMenuI.h>

class HelpButtonDialog
{
public:
	static HelpButtonDialog *instance();

	struct HelpMenu : public GLMenuI
	{
		HelpMenu();

		// Inherited from GLMenuI
		virtual void menuSelection(const char* menuName, 
			const int position, GLMenuItem &item);
		virtual bool getMenuItems(const char* menuName, 
			std::list<GLMenuItem> &result);

	protected:
		GLTexture helpTexture_;
	} helpMenu_;

	struct VolumeMenu : public GLMenuI
	{
		VolumeMenu();

		// Inherited from GLMenuI
		virtual bool menuOpened(const char* menuName);

		GLTexture soundTexture_;

	} volumeMenu_;

	struct PerformanceMenu : public GLMenuI
	{
		PerformanceMenu();

		// Inherited from GLMenuI
		virtual void menuSelection(const char* menuName, 
			const int position, GLMenuItem &item);
		virtual bool getMenuItems(const char* menuName, 
			std::list<GLMenuItem> &result);
		virtual LangStringStorage *getMenuToolTip(const char* menuName);

	protected:
		GLTexture helpTexture_;
	} performanceMenu_;

protected:
	static HelpButtonDialog *instance_;

private:
	HelpButtonDialog();
	virtual ~HelpButtonDialog();
};

#endif
