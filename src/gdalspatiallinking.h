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

#ifndef GDALSPATIALLINKING_H
#define GDALSPATIALLINKING_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALSpatialLinking : public DM::Module
{
	DM_DECLARE_NODE(GDALSpatialLinking)
public:
	GDALSpatialLinking();
	void init();
	void run();
	bool checkCentroid(OGRGeometry* geo, OGRGeometry *lead_geo);
	bool checkIntersection(OGRGeometry* geo, OGRGeometry * lead_geo);
private:
	bool properInit; // is true when init succeeded

	std::string leadingViewName;
	std::string linkViewName;
	bool withCentroid;

	DM::ViewContainer leadingView;
	DM::ViewContainer linkView;

	std::string link_name;

};

#endif // GDALSPATIALLINKING_H
