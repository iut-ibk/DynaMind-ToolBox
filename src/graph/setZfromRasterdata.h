/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#ifndef SetZfromRasterdata_H
#define SetZfromRasterdata_H

#include <dmmodule.h>
#include <dm.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangle_2.h>

class SetZfromRasterdata : public DM::Module
{
	DM_DECLARE_NODE(SetZfromRasterdata)

private:
	typedef std::map<std::string,DM::View> viewmap;
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
	typedef Delaunay::Face_handle Face_handle;
	typedef Delaunay::Vertex_handle Vertex_handle;
	typedef Delaunay::Locate_type Locate_type;
	typedef K::Point_2 Vertex;

	DM::System *sys;
	DM::RasterData * r;
	viewmap viewdef;
	double offset;
	bool interpolate;


public:
	SetZfromRasterdata();

	void run();
	void initmodel(){}
	bool setZWithRaster(DM::RasterData * r, std::map<std::string, DM::Component*> &nodes,double offset);
	bool setZWithDelaunay(DM::RasterData * r, std::map<std::string, DM::Component*> &nodes,double offset);
	double interpolateTriangle(DM::Node *point, Delaunay &dt, std::map<Vertex, K::FT, K::Less_xy_2> &function_values);
};

#endif // SetZfromRasterdata_H
