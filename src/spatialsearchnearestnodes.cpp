#include "spatialsearchnearestnodes.h"

#include <list>
#include <cmath>
#include <tbvectordata.h>
#ifndef __clang__
#include <omp.h>
#endif

SpatialSearchNearestNodes::SpatialSearchNearestNodes(DM::System * sys, std::vector<DM::Node*> nodes)
{

    sphm = new DM::SpatialNodeHashMap(sys, 1, false);

    std::list<Point_d> points;

    foreach (DM::Node * n, nodes) {
        sphm->addNodeToSpatialNodeHashMap(n);
        points.push_back(Point_d(n->getX(), n->getY()));
    }

    searchTree = new Tree(points.begin(), points.end());

}

SpatialSearchNearestNodes::~SpatialSearchNearestNodes()
{
    delete searchTree;
}

DM::Node *SpatialSearchNearestNodes::findNearestNode(DM::Node *n, double treshhold)
{
    const unsigned int N = 1;

    Point_d query(n->getX(),n->getY());
    Neighbor_search search(*searchTree, query, N);
    Point_d p;
    for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it){
        p = it->first;
        double lenght =  std::sqrt(it->second);
         DM::Logger(DM::Debug) <<lenght;
        if (treshhold > 0 && treshhold < lenght )
            return 0;

        break;
    }

    DM::Logger(DM::Debug) << p.x() << "\t" << p.y();

    return sphm->findNode(p.x(),  p.y(), 0.0001);
}





