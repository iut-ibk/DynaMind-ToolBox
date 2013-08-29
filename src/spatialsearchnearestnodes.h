#ifndef SPATIALSEARCHNEARESTNODES_H
#define SPATIALSEARCHNEARESTNODES_H

#include <dm.h>
#include <dmgeometry.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>



class DM_HELPER_DLL_EXPORT SpatialSearchNearestNodes
{
	typedef CGAL::Simple_cartesian<double> K;
	typedef K::Point_2 Point_d;
	typedef CGAL::Search_traits_2<K> TreeTraits;
	typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
	typedef Neighbor_search::Tree Tree;

public:
	SpatialSearchNearestNodes(DM::System * sys, std::vector<DM::Node *> nodes);
	~SpatialSearchNearestNodes();

	DM::Node * findNearestNode(DM::Node * n, double treshhold = -1);

private:
	Tree * searchTree;
	std::list<Point_d> points;

	DM::SpatialNodeHashMap * sphm;
};

#endif // SPATIALSEARCHNEARESTNODES_H
