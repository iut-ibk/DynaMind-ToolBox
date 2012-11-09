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
#include <math.h>
#include <algorithm>


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

    if (n1->compare2d(n) || n2->compare2d(n)) {
        delete e1;
        return;
    }

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

DM::Node TBVectorData::CaclulateCentroid(DM::System * sys, DM::Face * f) {
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
    double A6 = TBVectorData::CalculateArea(sys, f)*6.;
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
    return DM::Node(x/A6,y/A6,nodes[0]->getZ());
}
double TBVectorData::CalculateArea(std::vector<DM::Node * > const &nodes)
{

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[2]), E_to);

    double alphas[3][3];
    RotationMatrix(E, E_to, alphas);

    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }

    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;

    for (int i = 0; i < nodes.size(); i++) {
        DM::Node n = *(nodes[i]);
        DM::Node n_t = RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
    }



    DM::Node * pend = ns_t[nodes.size()-1];
    DM::Node * pstart = ns_t[0];
    bool startISEnd = true;
    if (pend != pstart)
        startISEnd = false;
    double A = 0;
    for (int i = 0; i< ns_t.size()-1;i++) {
        DM::Node * p_i = ns_t[i];
        DM::Node * p_i1 = ns_t[i+1];

        A+=p_i->getX()*p_i1->getY() - p_i1->getX()*p_i->getY();

    }
    if (!startISEnd)
        A+= pend->getX()*pstart->getY() - pstart->getX()*pend->getY();

    return A/2.;
}

double TBVectorData::CalculateArea(DM::System * sys, DM::Face * f)
{
    //Check if first is last
    if (f->getNodes().size() < 3)
        return 0;
    std::vector<std::string> NodeUUIDs = f->getNodes();
    std::vector<DM::Node *> nodes;
    foreach (std::string uuid, NodeUUIDs) {
        nodes.push_back(sys->getNode(uuid));
    }


    //Caclulate Area Total
    double A = CalculateArea(nodes);

    std::vector<std::vector<std::string > > holes = f->getHoles();

    foreach (std::vector<std::string> hole, holes) {
        std::vector<DM::Node *> nodes_H;
        foreach (std::string uuid, hole) {
            nodes_H.push_back(sys->getNode(uuid));
        }
        //Remove Holes
         A -= CalculateArea(nodes_H);
    }
    return A;
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


DM::Node TBVectorData::CentroidPlane3D(DM::System *sys, DM::Face *f)
{

    //Make Place Plane
    std::vector<DM::Node*> nodeList = TBVectorData::getNodeListFromFace(sys, f);

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodeList[0]), *(nodeList[1]), *(nodeList[2]), E_to);

    double alphas[3][3];
    RotationMatrix(E, E_to, alphas);

    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }

    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;

    for (int i = 0; i < nodeList.size(); i++) {
        DM::Node n = *(nodeList[i]);
        DM::Node n_t = RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
    }

    DM::Face * f_t = transformedSys.addFace(ns_t);
    DM::Node centroid_t = TBVectorData::CaclulateCentroid(&transformedSys, f_t);

    DM::Node centroid = RotateVector(alphas_t, centroid_t);
    return centroid;

}

DM::Node  TBVectorData::RotateVector(double (&R)[3][3], const DM::Node & node) {

    double n[3];


    n[0] = node.getX();
    n[1] = node.getY();
    n[2] = node.getZ();

    double n_t[3];

    for (int i = 0; i < 3; i++) {
        n_t[i] = 0;
        for (int j = 0; j < 3; j++) {
            n_t[i]  += R[i][j]*n[j];
        }
    }

    return DM::Node( n_t[0],  n_t[1],  n_t[2] );
}

DM::Node TBVectorData::NormalVector(const DM::Node & n1, const DM::Node & n2)
{

    double x = n1.getY()*n2.getZ() - n1.getZ()*n2.getY();
    double y = n1.getZ()*n2.getX() - n1.getX()*n2.getZ();
    double z = n1.getX()*n2.getY() - n1.getY()*n2.getX();
    double l = sqrt(x*x+y*y+z*z);
    return DM::Node(x/l,y/l,z/l);
}

void TBVectorData::CorrdinateSystem(const DM::Node &node0, const DM::Node &node1, const DM::Node &node2, double (&E)[3][3])
{
    DM::Node n1 = node0 - node1;
    DM::Node n2 = node1 - node2;



    DM::Node e3 = NormalVector(n1, n2);
    DM::Node e2 = NormalVector(n1, e3);
    DM::Node e1 = NormalVector(e2, e3);

    E[0][0] = e1.getX();
    E[1][0] = e1.getY();
    E[2][0] = e1.getZ();
    E[0][1] = e2.getX();
    E[1][1] = e2.getY();
    E[2][1] = e2.getZ();
    E[0][2] = e3.getX();
    E[1][2] = e3.getY();
    E[2][2] = e3.getZ();
}

double TBVectorData::DirectionCosine(const DM::Node &n1, const DM::Node &n2)
{
    double val1 = n1.getX()*n2.getX()+n1.getY()*n2.getY()+n1.getZ()*n2.getZ();
    double N1 = n1.getX()*n1.getX()+n1.getY()*n1.getY()+n1.getZ()*n1.getZ();
    double N2 = n2.getX()*n2.getX()+n2.getY()*n2.getY()+n2.getZ()*n2.getZ();
    if (N1 == 0 || N2 == 0) {
        DM::Logger(DM::Warning) << "n1 or n2 is null!";
        return -1;
    }

    double cosangel = val1/(sqrt(N1)*sqrt(N2));

    return cosangel;
}

double TBVectorData::AngelBetweenVectors(const DM::Node & n1, const DM::Node & n2)
{
    return  acos(DirectionCosine(n1, n2));
}

void TBVectorData::RotationMatrix(const double (&E_from)[3][3], const double (&E_to)[3][3], double (&alphas)[3][3])
{

    for (int i = 0; i < 3; i++) {
        double l_orig2 = 0;
        for (int j = 0; j < 3; j++) {
            l_orig2+=E_to[j][i] * E_to[j][i];
        }
        double l_orig = sqrt(l_orig2);
        for (int k = 0; k < 3; k++){
            double l_orig2_k = 0;
            for (int j = 0; j < 3; j++) {
                l_orig2_k+=E_from[j][k] * E_from[j][k];
            }
            double l_orig_k = sqrt(l_orig2_k);
            double val=0;
            for (int j = 0; j < 3; j++) {
                val+=E_to[j][i]* E_from[k][j];
            }
            alphas[i][k] = val/(l_orig*l_orig_k);
        }
    }

}

std::vector<DM::Face*> TBVectorData::ExtrudeFace(DM::System * sys, const DM::View & view, const std::vector<DM::Node*> &vp, const float & height, bool withLid){
    //Create Upper Points
    std::vector<DM::Node*> opposite_ids;
    //Face refF = vf[0];
    foreach(DM::Node * n, vp) {
        DM::Node * n_new = sys->addNode(n->getX(), n->getY(), n->getZ() + height);
        opposite_ids.push_back(n_new);
    }

    //Create Sides
    std::vector<DM::Face*> newFaces;
    for (unsigned int i = 0; i < vp.size(); i++) {
        if (i != 0) {
            std::vector<DM::Node *> f_side;
            f_side.push_back(vp[i]);
            f_side.push_back(opposite_ids[i]);
            f_side.push_back(opposite_ids[i-1]);
            f_side.push_back(vp[i-1]);
            f_side.push_back(vp[i]);
            newFaces.push_back(sys->addFace(f_side, view));
        }

    }

    //Create Lid
    if (!withLid)
            return newFaces;
    //reverse(opposite_ids.begin(),opposite_ids.end());
    newFaces.push_back(sys->addFace(opposite_ids, view));

    return newFaces;


}

double TBVectorData::calculateDistance(DM::Node *a, DM::Node *b)
{
    return sqrt(pow(a->getX()-b->getX(),2)+pow(a->getY()-b->getY(),2));
}
