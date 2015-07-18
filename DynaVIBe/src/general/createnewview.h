/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014 Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef CREATENEWVIEW_H
#define CREATENEWVIEW_H

#include <dmmodule.h>
#include <dm.h>
/**
 * @ingroup Modules
 * @brief The CreateNewView class
 */
class DM_HELPER_DLL_EXPORT CreateNewView : public DM::Module{

	DM_DECLARE_NODE(CreateNewView)
	private:
		DM::System * sys_in;
		std::vector<DM::View> data;

	public:
		bool modifyExistingView;
		bool startNewStream;
		std::string NameOfView;
		int viewType;
		std::map<std::string,std::string> newAttributes;

	public:
		CreateNewView();
		void init();
		void run();
		bool createInputDialog();
		std::string getHelpUrl();
};

#endif // CREATENEWVIEW_H
