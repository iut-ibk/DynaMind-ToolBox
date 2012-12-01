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
