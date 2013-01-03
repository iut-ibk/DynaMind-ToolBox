/**
 * @file
 * @author  Christian Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cgalsearchoperations.h"
#include "dmgeometry.h"
#include <dmnode.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>


std::vector<DM::Node>  CGALSearchOperations::NearestPoints(const std::vector<DM::Node*>  & nodes,  DM::Node * sn)
{
    std::vector<DM::Node> nearestPoints;
    typedef CGAL::Simple_cartesian<double> K;
    typedef K::Point_2 Point_d;
    typedef CGAL::Search_traits_2<K> TreeTraits;
    typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
    typedef Neighbor_search::Tree Tree;

    const unsigned int N = 1;

    std::list<Point_d> points;

    foreach (DM::Node * n, nodes) {
        points.push_back(Point_d(n->getX(), n->getY()));
    }

    Tree tree(points.begin(), points.end());

    Point_d query(sn->getX(),sn->getY());

    Neighbor_search search(tree, query, N);
    for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it){
        Point_d d (it->first);
        DM::Node nd(CGAL::to_double(d.x()), CGAL::to_double(d.y()), 0.);
        nearestPoints.push_back(nd);
    }

    return nearestPoints;

}
