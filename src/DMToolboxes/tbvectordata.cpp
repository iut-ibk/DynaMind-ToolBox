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
#include <cmath>
#include <algorithm>


#include "tbvectordata.h"
#include "dm.h"
#include <dmrasterdata.h>
#include <dmstdutilities.h>

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

std::vector<DM::Node*> TBVectorData::getNodeListFromFace(DM::System *sys, DM::Face *face) {
	return face->getNodePointers();
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

    //Offset Points to fix problem with big number
    DM::Node offsetN = MinCoordinates(nodes);

    DM::Node * pend = nodes[nodes.size()-1];
    DM::Node * pstart = nodes[0];
    bool startISEnd = true;
    if (pend != pstart)
        startISEnd = false;
    double A6 = TBVectorData::CalculateArea(sys, f)*6.;
    double x = 0;
    double y = 0;
    for (unsigned int i = 0; i< nodes.size()-1;i++) {
        DM::Node p_i = *nodes[i] - offsetN;
        DM::Node p_i1 = *nodes[i+1] - offsetN;

        x+= (p_i.getX() + p_i1.getX())*(p_i.getX() * p_i1.getY() - p_i1.getX() * p_i.getY());
        y+= (p_i.getY() + p_i1.getY())*(p_i.getX() * p_i1.getY() - p_i1.getX() * p_i.getY());


    }
    if (!startISEnd) {
        x+= (pend->getX() - offsetN.getX() + pstart->getX() - offsetN.getX())*( (pend->getX() - offsetN.getX()) * (pstart->getY() -  offsetN.getY()) - (pstart->getX() - offsetN.getX()) * (pend->getY() -  offsetN.getY()));
        y+= (pend->getY()-  offsetN.getY() + pstart->getY()-  offsetN.getY())*( (pend->getX() - offsetN.getX()) * (pstart->getY() -  offsetN.getY()) - (pstart->getX() - offsetN.getX()) * (pend->getY() -  offsetN.getY()));

    }

    DM::Node n1 = *(nodes[0]) - *(nodes[1]);
    DM::Node n2 = *(nodes[1]) - *(nodes[2]);
    DM::Node n = NormalVector(n1, n2);
    if (n.getZ() < 0) {
        x = x*-1;
        y = y*-1;
    }

    return DM::Node(x/A6 + offsetN.getX(),y/A6 + offsetN.getY(),nodes[0]->getZ());
}

void TBVectorData::CorrdinateSystem(std::vector<DM::Node * > const &nodes, double (&E)[3][3]) {
    //Avoid parallel lines
    int pos = -1;
    for (uint i = 2; i < nodes.size(); i++) {
        DM::Node n1 = *(nodes[0]) - *(nodes[1]);
        DM::Node n2 = *(nodes[1]) - *(nodes[i]);

        if(n1.getX() == n2.getX() && n1.getY() == n2.getY()) {
          continue;
        }
        if(n1.getX() == n2.getX() && n1.getZ() == n2.getZ()) {
            continue;
        }
        if(n1.getY() == n2.getY() && n1.getZ() == n2.getZ()) {
            continue;
        }
        pos=i;
        break;
    }
    if (pos == -1)
        DM::Logger(DM::Warning) << "couldn't create coorindate sytem";
    TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[pos]), E);
}

double TBVectorData::CalculateArea(std::vector<DM::Node * > const &nodes)
{

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( nodes, E_to);

    double alphas[3][3];
    RotationMatrix(E, E_to, alphas);

    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;

    for (unsigned int i = 0; i < nodes.size(); i++) {
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
    for (unsigned int i = 0; i< ns_t.size()-1;i++) {
        DM::Node * p_i = ns_t[i];
        DM::Node * p_i1 = ns_t[i+1];

        A+=p_i->getX()*p_i1->getY() - p_i1->getX()*p_i->getY();

    }
    if (!startISEnd)
        A+= pend->getX()*pstart->getY() - pstart->getX()*pend->getY();

    //if (std::isnan(A))
    //    DM::Logger(DM::Error) << "nan area";
    return fabs(A/2.);
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

    //if (std::isnan(A))
    //    DM::Logger(DM::Warning) << "TBVectorData::CalculateArea nan area";
    return fabs(A);
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


DM::Node TBVectorData::CentroidPlane(const std::vector<DM::Node*> & nodes) {
    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    //TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[ nodes.size()-2]), E_to);
    TBVectorData::CorrdinateSystem( nodes, E_to);
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

    for (unsigned int i = 0; i < nodes.size(); i++) {
        DM::Node n = *(nodes[i]);
        DM::Node n_t = RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
    }

    DM::Face * f_t = transformedSys.addFace(ns_t);
    DM::Node centroid_t = TBVectorData::CaclulateCentroid(&transformedSys, f_t);

    DM::Node centroid = RotateVector(alphas_t, centroid_t);
    return centroid;

}

DM::Node TBVectorData::CentroidPlane3D(DM::System *sys, DM::Face *f)
{

    //Make Place Plane
    std::vector<DM::Node*> nodeList = TBVectorData::getNodeListFromFace(sys, f);
    return TBVectorData::CentroidPlane(nodeList);

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
    if (l == 0)
        DM::Logger(DM::Warning) << "Normal vector l in 0";
    return DM::Node(x/l,y/l,z/l);
}

void TBVectorData::CorrdinateSystem(const DM::Node &node0, const DM::Node &node1, const DM::Node &node2, double (&E)[3][3])
{
    DM::Node n1 = node0 - node1;
    DM::Node n2 = node1 - node2;

    if(n1.getX() == n2.getX() && n1.getY() == n2.getY()) {
        DM::Logger(DM::Warning) << "failed to create transformation n1 and n2 are parallel";
    }
    if(n1.getX() == n2.getX() && n1.getZ() == n2.getZ()) {
        DM::Logger(DM::Warning) << "failed to create transformation n1 and n2 are parallel";
    }
    if(n1.getY() == n2.getY() && n1.getZ() == n2.getZ()) {
        DM::Logger(DM::Warning) << "failed to create transformation n1 and n2 are parallel";
    }

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

std::vector<DM::Face*> TBVectorData::ExtrudeFace(DM::System * sys, const DM::View & view, const std::vector<DM::Node*> &vp,  double height,  double offset, bool withLid){

    std::vector<DM::Node * > basePoints;
    //Create new base points if offset != 0
    if (offset != 0) {
        foreach (DM::Node * n, vp)
            basePoints.push_back(sys->addNode(n->getX(), n->getY(), n->getZ() + offset));
    } else {
        basePoints = vp;
    }
    //Create Upper Points

    std::vector<DM::Node*> opposite_ids;
    //Face refF = vf[0];
    foreach(DM::Node * n, basePoints) {
        DM::Node * n_new = sys->addNode(n->getX(), n->getY(), n->getZ() + height);
        opposite_ids.push_back(n_new);
    }

    //Create Sides
    std::vector<DM::Face*> newFaces;
    for (unsigned int i = 0; i < basePoints.size(); i++) {
        if (i != 0) {
            std::vector<DM::Node *> f_side;
            f_side.push_back(basePoints[i]);
            f_side.push_back(opposite_ids[i]);
            f_side.push_back(opposite_ids[i-1]);
            f_side.push_back(basePoints[i-1]);
            f_side.push_back(basePoints[i]);
            newFaces.push_back(sys->addFace(f_side, view));
        }

    }

    //Create Lid
    if (!withLid)
            return newFaces;
    newFaces.push_back(sys->addFace(opposite_ids, view));

    return newFaces;
}

double TBVectorData::calculateDistance(DM::Node *a, DM::Node *b)
{
	double p0[3], p1[3];
	a->get(p0);
	b->get(p1);
	p0[0] -= p1[0];
	p0[1] -= p1[1];
	p0[2] -= p1[2];
	return sqrt(p0[0]*p0[0] + p0[1]*p0[1] + p0[2]*p0[2]);
    //return sqrt(pow(a->getX()-b->getX(),2)+pow(a->getY()-b->getY(),2) + pow(a->getZ()-b->getZ(),2));
}

bool TBVectorData::PointWithinFace(DM::Face *f, DM::Node *n)
{
    std::vector<DM::Node*> face = f->getNodePointers();

    if(CalculateWindingNumber(face,n) == 0)
        return false;

    std::vector<DM::Face*> holes = f->getHolePointers();

    for(uint index=0; index < holes.size(); index++)
        if(CalculateWindingNumber(holes[index]->getNodePointers(),n) != 0)
            return false;

    return true;
}

bool TBVectorData::PointWithinAnyFace(std::map<std::string, DM::Component *> fv, DM::Node *n)
{
    //typedef std::pair<std::string,DM::Component*> Cp;
    typedef std::map<std::string,DM::Component*>::iterator CompItr;

    for(CompItr i = fv.begin(); i != fv.end(); i++)
    {
        DM::Face* currentface = static_cast<DM::Face*>((*i).second);

        if(TBVectorData::PointWithinFace(currentface,n))
            return true;
    }

    return false;
}

bool TBVectorData::EdgeWithinFace(DM::Face *f, DM::Edge *e)
{
    if(!TBVectorData::PointWithinFace(f,e->getStartNode()))
        return false;

    if(!TBVectorData::PointWithinFace(f,e->getEndNode()))
        return false;

    return true;
}

bool TBVectorData::EdgeWithinAnyFace(std::map<string, DM::Component *> fv, DM::Edge *e)
{
    //typedef std::pair<std::string,DM::Component*> Cp;
    typedef std::map<std::string,DM::Component*>::iterator CompItr;

    for(CompItr i = fv.begin(); i != fv.end(); i++)
    {
        DM::Face* currentface = static_cast<DM::Face*>((*i).second);

        if(TBVectorData::EdgeWithinFace(currentface,e))
            return true;
    }

    return false;
}

int TBVectorData::CalculateWindingNumber(std::vector<DM::Node *> poly, DM::Node *n)
{
    if(!poly.size())
        return 0;

    int windindnumber=0;
    DM::Node *center = n;

    for(uint index=0; index<poly.size(); ++index)
    {
        DM::Node *point1 = poly[index];
        DM::Node *point2;

        if(index == (poly.size()-1))
            point2 = poly[0];
        else
            point2 = poly[index+1];

        if(point1->getY() <= center->getY())
        {
            if(point2->getY() > center->getY())
            {
                double isleft = ((point2->getX() - point1->getX()) * (center->getY() - point1->getY()) -
                              (center->getX() - point1->getX()) * (point2->getY() - point1->getY()));

                if(isleft > 0)
                    windindnumber++;

            }
        }
        else
        {
            if(point2->getY() <= center->getY())
            {
                double isleft = ((point2->getX() - point1->getX()) * (center->getY() - point1->getY()) -
                              (center->getX() - point1->getX()) * (point2->getY() - point1->getY()));

                if(isleft < 0)
                    windindnumber--;
            }
        }

    }

    return windindnumber;
}

vector<DM::Node> TBVectorData::CreateCircle(DM::Node *c, double radius, int segments)
{
    const double pi =  3.14159265358979323846;
    std::vector<DM::Node> ressNodes;
    double delta_phi = pi*2/segments;

    for (double phi = 0; phi < 2*pi; phi+=delta_phi) {
        double x = radius * cos(phi) + c->getX();
        double y = radius * sin(phi) + c->getY();
        DM::Node n(x,y,0);
        ressNodes.push_back(n);
    }
    return ressNodes;
}

std::vector<DM::Node> TBVectorData::CreateRaster(DM::System *sys, DM::Face *f, double gridSize)
{
    //Make Place Plane
    std::vector<DM::Node*> nodeList = TBVectorData::getNodeListFromFace(sys, f);

    std::vector<DM::Node> returnNodes_t;

    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    //TBVectorData::CorrdinateSystem( *(nodeList[0]), *(nodeList[1]), *(nodeList[ nodeList.size()-2]), E_to);
    TBVectorData::CorrdinateSystem( nodeList, E_to);
    double alphas[3][3];
    TBVectorData::RotationMatrix(E, E_to, alphas);

    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }

    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;
    double const_height;
    for (unsigned int i = 0; i < nodeList.size(); i++) {
        DM::Node n = *(nodeList[i]);
        DM::Node n_t =  TBVectorData::RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
        const_height = n_t.getZ();
    }

    double minX = 0;
    double maxX = 0;
    double minY = 0;
    double maxY = 0;
    for (unsigned int i = 0; i < ns_t.size(); i++){
        DM::Node * n1 = ns_t[i];
        if (i == 0) {
            minX = n1->getX();
            maxX = n1->getX();
            minY = n1->getY();
            maxY = n1->getY();
        }

        if(minX > n1->getX())
            minX = n1->getX();
        if(maxX < n1->getX())
            maxX = n1->getX();

        if(minY > n1->getY())
            minY = n1->getY();
        if(maxY < n1->getY())
            maxY = n1->getY();
    }
    double blockWidth = maxX - minX;
    double blockHeight = maxY - minY;


    //Create Raster

    unsigned int elements_x = blockWidth/gridSize + 1;
    unsigned int elements_y = blockHeight/gridSize + 1;
    double realwidth = blockWidth / elements_x;
    double realheight = blockHeight / elements_y;

    QPolygonF h1;
    for (unsigned int i = 0; i < ns_t.size()-1; i++){
        DM::Node * n1 = ns_t[i];
        h1.push_back(QPointF(n1->getX(), n1->getY()));
    }

    std::vector<QPolygonF> holes;
    foreach (std::vector<std::string> hole, f->getHoles()) {
        QPolygonF h;
        for (unsigned int i = 0; i < hole.size(); i++) {
            DM::Node n = *(sys->getNode(hole[i]));
            DM::Node n_t =  TBVectorData::RotateVector(alphas, n);
            h.push_back(QPointF(n_t.getX(), n_t.getY()));
        }
        holes.push_back(h);
    }

    for (unsigned int x = 0; x < elements_x; x++) {
        for (unsigned int y = 0; y < elements_y; y++) {
            double x_p = minX + ((double)x+0.5) * realwidth;
            double y_p = minY + ((double)y+0.5) * realheight;
            //Cehck if Point is valid
            QPointF p(x_p, y_p);

            if (!h1.containsPoint(p, Qt::WindingFill))
                continue;
            bool isInHole = false;
            foreach (QPolygonF h1, holes) {
                if (h1.containsPoint(p, Qt::WindingFill)) {
                    isInHole = true;
                    break;
                }
            }
            if (isInHole)
                continue;
            returnNodes_t.push_back(DM::Node(x_p,y_p, const_height));
        }
    }

    std::vector<DM::Node> returnNodes;
    for (unsigned int  i = 0; i <returnNodes_t.size(); i++ ) {
        returnNodes.push_back(TBVectorData::RotateVector(alphas_t,returnNodes_t[i]));
    }

    return returnNodes;

}

DM::Face * TBVectorData::AddFaceToSystem(DM::System *sys, std::vector<DM::Node> nodes)
{
    if (nodes.size() == 0)
        return 0;

    std::vector<DM::Node*> nodes_p;
    foreach (DM::Node n, nodes) {
        nodes_p.push_back(sys->addNode(n));
    }
    nodes_p.push_back(nodes_p[0]);

    return sys->addFace(nodes_p);


}

DM::Node TBVectorData::MinCoordinates(std::vector<DM::Node*> & nodes)
{
    if (!nodes.size()) {
        DM::Logger(DM::Warning) << "no nodes";
        return DM::Node();
    }
	
	double min[3];
	double tmp[3];
	nodes[0]->get(min);

	foreach(DM::Node* n, nodes)
	{
		n->get(tmp);
		if(min[0] > tmp[0]) min[0] = tmp[0];
		if(min[1] > tmp[1]) min[1] = tmp[1];
		if(min[2] > tmp[2]) min[2] = tmp[2];
	}
    return DM::Node(min[0],min[1],min[2]);
}

bool TBVectorData::GetViewExtend(DM::System * sys, DM::View & view, double & x_min, double & y_min, double & x_max, double & y_max)
{
    x_min = 0;
    y_min = 0;
    x_max = 0;
    y_max = 0;

    if (view.getType() == DM::RASTERDATA) {
        std::map<std::string, DM::Component*> components = sys->getAllComponentsInView(view);
        mforeach(DM::Component* c, components) {
            if(c->getType() == DM::RASTERDATA) {
                DM::RasterData * r = (DM::RasterData*)c;
                x_min = r->getXOffset();
                y_min = r->getYOffset();
                x_max = r->getWidth()*r->getCellSizeX() + r->getXOffset();
                y_max = r->getHeight()*r->getCellSizeY() + r->getYOffset();
                return true;
            }
        }
        return false;


    }

    std::vector<DM::Node * > nodes;
    if (view.getType() == DM::NODE) TBVectorData::GetNodesFromNodes(sys, view, nodes);
    if (view.getType() == DM::EDGE) TBVectorData::GetNodesFromEdges(sys, view, nodes);
    if (view.getType() == DM::FACE) TBVectorData::GetNodesFromFaces(sys, view, nodes);

    if (nodes.size() < 2)  {
        DM::Logger(DM::Warning) << "Number of Nodes < 2 no bounding box created";
        return false;
    }

    x_min = nodes[0]->getX();
    y_min = nodes[0]->getY();
    x_max = nodes[0]->getX();
    y_max = nodes[0]->getY();

    foreach (DM::Node *n , nodes) {
        if (x_min > n->getX()) x_min = n->getX();
        if (x_max < n->getX()) x_max = n->getX();
        if (y_min > n->getY()) y_min = n->getY();
        if (y_max < n->getY()) y_max = n->getY();
    }

    return true;
}

std::vector<DM::Node *> TBVectorData::GetNodesFromNodes(DM::System *sys, DM::View &view, std::vector<DM::Node *> &nodes)
{
    nodes.clear();
    std::vector<std::string> uuids = sys->getUUIDs(view);
    foreach (std::string uuid, uuids) {
        DM::Node * n = sys->getNode(uuid);
        nodes.push_back(n);
    }
    return nodes;

}

std::vector<DM::Node *> TBVectorData::GetNodesFromEdges(DM::System *sys, DM::View &view, std::vector<DM::Node *> &nodes)
{
    nodes.clear();
    std::vector<std::string> uuids = sys->getUUIDs(view);
    foreach (std::string uuid, uuids) {
        DM::Edge * e = sys->getEdge(uuid);
        nodes.push_back(sys->getNode(e->getStartpointName()));
        nodes.push_back(sys->getNode(e->getEndpointName()));
    }
    return nodes;
}

std::vector<DM::Node *> TBVectorData::GetNodesFromFaces(DM::System *sys, DM::View &view, std::vector<DM::Node *> &nodes)
{
    nodes.clear();
    std::vector<std::string> uuids = sys->getUUIDs(view);
    foreach (std::string uuid, uuids) {
        DM::Face * f = sys->getFace(uuid);
        std::vector<DM::Node*> nl = TBVectorData::getNodeListFromFace(sys, f);
        foreach (DM::Node * n, nl)
            nodes.push_back(n);
    }
    return nodes;
}

std::vector<DM::Node*> TBVectorData::findNearestNeighbours(DM::Node *root, double maxdistance, std::vector<DM::Node *> nodefield)
{
    typedef std::map<DM::Node*,double>::iterator It;
    std::map<DM::Node*,double> distances;
    std::vector<DM::Node*> result;

    //find nearest nodes
    for(uint i=0; i < nodefield.size(); i++)
    {
        double currentdistance=TBVectorData::calculateDistance(root,nodefield[i]);
        if(currentdistance <= maxdistance)
            distances[nodefield[i]]=currentdistance;
    }

    //sort
    while(result.size() < distances.size())
    {
        It i = distances.begin();
        DM::Node* minnode = (*i).first;
        double mindistance = (*i).second;

        for(It i = distances.begin(); i != distances.end(); ++i)
        {
            if((*i).second < mindistance)
            {
                minnode = (*i).first;
                mindistance = (*i).second;
            }
        }
        result.push_back(minnode);
        distances.erase(minnode);
    }

    return result;
}

bool TBVectorData::getBoundingBox(std::vector<DM::Node *> nodes, double &x, double &y, double &h, double &width, bool init=true)
{
    double boundaries[4] = {x,x+h,y,x+width}; //(xmin,xmax,ymin,ymax)
    bool initboundaries=init;

    for(uint index=0; index < nodes.size(); index++)
    {
        DM::Node* currentnode = nodes[index];

        if(initboundaries)
        {
            initboundaries=false;
            boundaries[0] = currentnode->getX();
            boundaries[1] = boundaries[0];
            boundaries[2] = currentnode->getY();
            boundaries[3] = boundaries[2];
        }
        else
        {
            if(boundaries[0] > currentnode->getX())
                boundaries[0] = currentnode->getX();

            if(boundaries[1] < currentnode->getX())
                boundaries[1] = currentnode->getX();

            if(boundaries[2] > currentnode->getY())
                boundaries[2] = currentnode->getY();

            if(boundaries[3] < currentnode->getY())
                boundaries[3] = currentnode->getY();
        }
    }

    x=boundaries[0];
    width=boundaries[1]-boundaries[0];
    y=boundaries[2];
    h=boundaries[3]-boundaries[2];
    return true;
}

double TBVectorData::maxDistance(std::vector<DM::Node *> pointfield, DM::Node *centernode)
{
    double result = 0;
    for(uint index=0; index < pointfield.size(); index++)
    {
        double distance = calculateDistance(centernode, pointfield[index]);

        if(distance > result)
            result = distance;
    }

    return result;
}

