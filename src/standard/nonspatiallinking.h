/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014  Christian Urich

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

#ifndef NONSPATIALLINKING_H
#define NONSPATIALLINKING_H


#include <dmmodule.h>
#include <dm.h>


class DM_HELPER_DLL_EXPORT NonSpatialLinking : public DM::Module
{
	DM_DECLARE_NODE(NonSpatialLinking)
private:
	std::string leadingViewName;
	std::string leadingattributeName;
	std::string joinViewName;
	std::string joinAttributeName;

	std::string alternative_link_name;

	DM::ViewContainer leadingView;
	DM::ViewContainer joinView;

	bool fullJoin;

	std::vector<std::string> attribute_names;

public:
	NonSpatialLinking();
	void run();
	void init();
	std::string getHelpUrl();
};

#endif // NONSPATIALLINKING_H
