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

#ifndef GDALCREATEBUILDING_H
#define GDALCREATEBUILDING_H

#include <dmmodule.h>
#include <dm.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>

class DM_HELPER_DLL_EXPORT OGRPolygon;

class DM_HELPER_DLL_EXPORT GDALCreateBuilding : public DM::Module
{
		DM_DECLARE_NODE(GDALCreateBuilding)
private:

	DM::ViewContainer parcel;
	DM::ViewContainer building;

	double width;
	double height;
	int residential_units;
	OGRGeometry *createBuilding(OGRPolygon *ogr_poly);
	OGRGeometry *addToSystem(SFCGAL::Polygon &poly);
public:
	GDALCreateBuilding();
	void run();
	std::string getHelpUrl();


};

#endif // GDALCREATEBUILDING_H
