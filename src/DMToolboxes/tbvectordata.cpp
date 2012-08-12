/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich
 
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

#include "tbvectordata.h"
#include "dm.h"
#include <QtGlobal>


DM::Edge * TBVectorData::getEdge(DM::System * sys, DM::View & view, DM::Node * n1, DM::Node * n2, bool OrientationMatters) {

    DM::Edge * e1 = sys->getEdge(n1->getUUID(), n2->getUUID());

    if (e1!=0) {
        if (view.getName().empty()) {
            return e1;
        } else {
            std::set<std::string> inViews = e1->getInViews();
            if (inViews.find(view.getName()) == inViews.end())
                return 0;
            return e1;
        }

    }
    if (!OrientationMatters) {
        e1 = sys->getEdge(n2->getUUID(), n1->getUUID());
        if (e1!=0) {
            if (view.getName().empty()) {
                return e1;
            } else {
                std::set<std::string> inViews = e1->getInViews();
                if (inViews.find(view.getName()) == inViews.end())
                    return 0;
                return e1;
            }

        }
    }


    return e1;
}

DM::Edge * TBVectorData::getEdge(DM::System * sys, DM::View & view, DM::Edge * e, bool OrientationMatters) {

    return TBVectorData::getEdge(sys,
                                 view,
                                 (DM::Node *) sys->getNode(e->getStartpointName()),
                                 (DM::Node *) sys->getNode(e->getEndpointName()),
                                 OrientationMatters);
}



DM::Node * TBVectorData::getNode2D(DM::System *sys, DM::View &view, DM::Node n, double err) {



    if (view.getName().empty()) {
        DM::NodeMap nmap = sys->getAllNodes();
        for (DM::NodeMap::const_iterator it = nmap.begin(); it != nmap.end(); ++it) {
            DM::Node * n_1 = it->second;
            if (n_1->compare2d(n, err))
                return n_1;
        }
    }

    DM::ComponentMap cmap = sys->getAllComponentsInView(view);

    for (DM::ComponentMap::const_iterator it = cmap.begin(); it != cmap.end(); ++it) {
        DM::Node * n_1 = (DM::Node *) it->second;
        if (n_1->compare2d(n, err))
            return n_1;
    }
    return 0;

}

DM::Node * TBVectorData::addNodeToSystem2D(DM::System *sys,DM::View &view,DM::Node n1,   double err, bool CreateNewNode) {
    DM::Node * new_Node = 0;


    new_Node = TBVectorData::getNode2D(sys, view, n1, err);
    if (new_Node != 0 || !CreateNewNode) {
        return new_Node;
    }


    new_Node =  sys->addNode(n1.getX(), n1.getY(), n1.getZ(), view);

    return new_Node;
}



std::vector<DM::Node*> TBVectorData::getNodeListFromFace(DM::System *sys, DM::Face *face) {
    std::vector<DM::Node*> result;
    std::vector<std::string> nodelist= face->getNodes();
    foreach (std::string eid, nodelist) {
        DM::Node * n = sys->getNode(eid);
        result.push_back(n);

    }
    return result;
}

void TBVectorData::splitEdge(DM::System *sys, DM::Edge *e, DM::Node *n, DM::View &view) {
    DM::Edge * e1 = new DM::Edge(*e);
    e1->createNewUUID();

    DM::Node * n1 = sys->getNode(e1->getStartpointName());
    DM::Node * n2 = sys->getNode(e1->getEndpointName());

    if (n1->compare2d(n) || n2->compare2d(n))
        return;

    std::set<std::string> views = e1->getInViews();
    e1 = sys->addEdge(e1);
    foreach (std::string v, views) {
        sys->removeComponentFromView(e1, *sys->getViewDefinition(v));

    }
    e1->setEndpointName(n->getUUID());
    sys->addComponentToView(e1, view);

    DM::Edge * e2 = new DM::Edge(*e);
    e2->createNewUUID();
    e2 = sys->addEdge(e2);
    views = e2->getInViews();
    foreach (std::string v, views) {
        sys->removeComponentFromView(e2, *sys->getViewDefinition(v));
    }
    e2->setStartpointName(n->getUUID());
    sys->addComponentToView(e2, view);


    sys->removeComponentFromView(e, view);

}

DM::Node TBVectorData::caclulateCentroid(DM::System * sys, DM::Face * f) {
    //Check if first is last
    if (f->getNodes().size() < 3)
        return DM::Node(0,0,0);
    std::vector<std::string> NodeUUIDs = f->getNodes();
    std::vector<DM::Node *> nodes;
    foreach (std::string uuid, NodeUUIDs) {
        nodes.push_back(sys->getNode(uuid));
    }

    DM::Node * pend = nodes[nodes.size()-1];
    DM::Node * pstart = nodes[0];
    bool startISEnd = true;
    if (pend != pstart)
            startISEnd = false;
    double A6 = TBVectorData::calculateArea(sys, f)*6.;
    double x = 0;
    double y = 0;
    for (int i = 0; i< nodes.size()-1;i++) {
        DM::Node * p_i = nodes[i];
        DM::Node * p_i1 = nodes[i+1];

        x+= (p_i->getX() + p_i1->getX())*(p_i->getX() * p_i1->getY() - p_i1->getX() * p_i->getY());
        y+= (p_i->getY() + p_i1->getY())*(p_i->getX() * p_i1->getY() - p_i1->getX() * p_i->getY());


    }
    if (!startISEnd) {
        x+= (pend->getX() + pstart->getX())*(pend->getX() * pstart->getY() - pstart->getX() * pend->getY());
        y+= (pend->getY() + pstart->getY())*(pend->getX() * pstart->getY() - pstart->getX() * pend->getY());

       }
    return DM::Node(x/A6,y/A6,0);
}

double TBVectorData::calculateArea(DM::System * sys, DM::Face * f) {
    //Check if first is last
    if (f->getNodes().size() < 3)
        return 0;
    std::vector<std::string> NodeUUIDs = f->getNodes();
    std::vector<DM::Node *> nodes;
    foreach (std::string uuid, NodeUUIDs) {
        nodes.push_back(sys->getNode(uuid));
    }

    DM::Node * pend = nodes[nodes.size()-1];
    DM::Node * pstart = nodes[0];
    bool startISEnd = true;
    if (pend != pstart)
            startISEnd = false;
    double A = 0;
    for (int i = 0; i< nodes.size()-1;i++) {
        DM::Node * p_i = nodes[i];
        DM::Node * p_i1 = nodes[i+1];

        A+=p_i->getX()*p_i1->getY() - p_i1->getX()*p_i->getY();

    }
    if (!startISEnd)
        A+= pend->getX()*pstart->getY() - pstart->getX()*pend->getY();

    return A/2.;
}

QPolygonF TBVectorData::FaceAsQPolgonF(DM::System *sys, DM::Face *f)
{
    QPolygonF poly;

    std::vector<std::string> uuids = f->getNodes();
    foreach (std::string uuid, uuids) {
        DM::Node * n = sys->getNode(uuid);
        QPointF pf(n->getX(), n->getY());
        poly.push_back(pf);
    }

    return poly;
}

