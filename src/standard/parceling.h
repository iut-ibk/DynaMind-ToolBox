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

#ifndef PARCELING_H
#define PARCELING_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>


typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>              Pwh_list_2;


class DM_HELPER_DLL_EXPORT GDALParceling: public DM::Module
{
	DM_DECLARE_NODE(GDALParceling)
private:
	DM::ViewContainer cityblocks;
	DM::ViewContainer parcels;
	int counter_added;
	double length;
	double width;

	std::string blockName;
	std::string subdevisionName;


public:
	GDALParceling();
	void run();
	void init();
	std::string getHelpUrl();
	void splitePoly(Polygon_with_holes_2 &poly);



	void addToSystem(SFCGAL::Polygon &poly);
	Pwh_list_2 splitter(Polygon_2 & rect);
	void split_left(Point_2 &p3, Pwh_list_2 &ress, Point_2 &p2, Point_2 &p4, Point_2 &p1, Vector_2 &v1);
	void split_up(Pwh_list_2 &ress, Point_2 &p3, Point_2 &p1, Point_2 &p4, Vector_2 &v2, Point_2 &p2);
};

#endif // PARCELING_H
