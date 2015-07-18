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

#ifndef GDALCOPYELEMENTTOVIEW_H
#define GDALCOPYELEMENTTOVIEW_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>

class GDALCopyElementToView : public DM::Module
{
	DM_DECLARE_NODE(GDALCopyElementToView)
public:
	GDALCopyElementToView();
	void init();
	void run();
	std::string getHelpUrl();

private:
	DM::ViewContainer vc_from;
	DM::ViewContainer vc_to;
	std::string fromViewName;
	std::string toViewName;
	bool add_link;
	bool copy_attributes;
	std::string link_name;
};


#endif // GDALCOPYELEMENTTOVIEW_H
