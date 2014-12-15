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

#ifndef GDALOFFSET_H
#define GDALOFFSET_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/approximated_offset_2.h>

#include <SFCGAL/MultiPolygon.h>
#include <SFCGAL/io/wkt.h>

typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
//typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
//typedef std::list<Polygon_with_holes_2>              Pwh_list_2;
typedef CGAL::Gps_circle_segment_traits_2<SFCGAL::Kernel>	Gps_traits_2;
typedef Gps_traits_2::Polygon_2								Offset_polygon_2;





class DM_HELPER_DLL_EXPORT GDALOffset: public DM::Module
{
	DM_DECLARE_NODE(GDALOffset)
private:
	DM::ViewContainer cityblocks;
	DM::ViewContainer parcels;
	int counter_added;


	std::string blockName;
	std::string subdevisionName;

	double offset;

	std::list<Polygon_2> offsetPolygon(Polygon_2 poly, double offset);
	Polygon_2 approximate(const Offset_polygon_2 &polygon, const int &n = 0);
public:
	GDALOffset();
	void run();
	void init();

	void addToSystem(SFCGAL::Polygon &poly);

};

#endif // GDALOFFSET_H
