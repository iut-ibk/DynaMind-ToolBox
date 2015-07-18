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

#ifndef SpatialPressurePerformance_H
#define SpatialPressurePerformance_H

#include <dmmodule.h>
#include <dm.h>

#include <watersupplyviewdef.h>

//cgal includes
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangle_2.h>

class SpatialPressurePerformance : public DM::Module
{
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
    typedef Delaunay::Face_handle Face_handle;
    typedef Delaunay::Vertex_handle Vertex_handle;
    typedef Delaunay::Locate_type Locate_type;
    typedef K::Point_2 Vertex;

    DM_DECLARE_NODE(SpatialPressurePerformance)

    DM::WS::ViewDefinitionHelper wsd;
    DM::System * sys1;
    DM::System * sys2;
    std::string resultfilepath;

public:
    SpatialPressurePerformance();
    void run();
    bool getCellValueVector(std::vector<DM::Node*> &nodes, std::vector<double> &values, double x,double y,double h,double w, double dh, double dw);
    bool createDelaunay(std::vector<DM::Node*> &nodes, Delaunay &dt, std::map<Vertex,DM::Node*> &mapping, std::map<Vertex, K::FT, K::Less_xy_2> &functionvalues);
    double interpolateTriangle(std::vector<DM::Node*> triangle,DM::Node* point);
    double interpolateTriangle(DM::Node *point, SpatialPressurePerformance::Delaunay &dt, std::map<Vertex, K::FT, K::Less_xy_2> &function_values);
};

#endif // SpatialPressurePerformance_H
