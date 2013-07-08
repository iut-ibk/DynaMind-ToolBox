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

#include "calculateboundingbox.h"
#include "sstream"
#include "tbvectordata.h"
#include <cgalgeometry.h>

DM_DECLARE_NODE_NAME(CalculateBoundingBox, Geometry)




CalculateBoundingBox::CalculateBoundingBox()
{
    this->city = 0;
    this->NameOfExistingView = "";
    this->NameOfNewView = "";
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addParameter("NameOfExistingView", DM::STRING, & this->NameOfExistingView);
    this->addParameter("MinBounding", DM::BOOL, & this->MinBounding);
    this->addParameter("NameOfNewView", DM::STRING, & this->NameOfNewView);
    this->overAll = false;
    this->addParameter("overAll", DM::BOOL, &this->overAll);

    this->addData("Data", data);
    changed = true;
    MinBounding = false;


}

void CalculateBoundingBox::init() {
    city = this->getData("Data");
    if (city == 0)
        return;
    std::vector<std::string> views = city->getNamesOfViews();

    foreach (std::string s, views)
        DM::Logger(DM::Debug) << s;

    if (this->NameOfExistingView.empty())
        return;

    if (NameOfNewView.empty()) {
        std::stringstream ss;
        ss << NameOfExistingView << "_BOUNDING_BOX";
        newFaces = DM::View(ss.str(), DM::FACE, DM::WRITE);
    }
    else
    {
         newFaces = DM::View(NameOfNewView, DM::FACE, DM::WRITE);
    }
    newFaces.addAttribute("l");
    newFaces.addAttribute("b");
    newFaces.addAttribute("x_min");
    newFaces.addAttribute("x_max");
    newFaces.addAttribute("y_min");
    newFaces.addAttribute("y_max");

    DM::View * v = city->getViewDefinition(NameOfExistingView);
    DM::View writeView = DM::View(v->getName(), v->getType(), DM::READ);
    if (!this->overAll) {
        writeView.addLinks(newFaces.getName(), newFaces);
        newFaces.addLinks(writeView.getName(), writeView);
    }

    std::vector<DM::View> data;
    data.push_back(writeView);
    data.push_back(newFaces);
    this->addData("Data", data);
    vData = writeView;

    this->updateParameter();
}

void CalculateBoundingBox::caculateBoundingBox()
{

    if (this->vData.getType() == DM::RASTERDATA) {
        double x1 = 0;
        double y1 = 0;
        double x2 = 0;
        double y2 = 0;

        TBVectorData::GetViewExtend(this->city, this->vData, x1, y1, x2, y2);

        DM::Node * n1 = this->city->addNode(x1, y1, 0);
        DM::Node * n2 = this->city->addNode(x1, y2, 0);
        DM::Node * n3 = this->city->addNode(x2, y2, 0);
        DM::Node * n4 = this->city->addNode(x2, y1, 0);


        std::vector<DM::Node *> vF;
        vF.push_back(n1);
        vF.push_back(n2);
        vF.push_back(n3);
        vF.push_back(n4);
        vF.push_back(n1);

        DM::Face * bF = city->addFace(vF, newFaces);

        bF->addAttribute("l", x2-x1);
        bF->addAttribute("b", y2-y1);

        bF->addAttribute("x_min", x1);
        bF->addAttribute("x_max", x2);
        bF->addAttribute("y_min", y1);
        bF->addAttribute("y_max", y2);



    }

    if (this->overAll) {
        std::vector<DM::Node * > nodes;
        if (this->vData.getType() == DM::NODE) this->getNodesFromNodes(nodes);
        if (this->vData.getType() == DM::EDGE) this->getNodesFromEdges(nodes);
        if (this->vData.getType() == DM::FACE) this->getNodesFromFaces(nodes);

        if (nodes.size() < 2)  {
            Logger(Warning) << "Number of Nodes < 2 no bounding box created";
            return;
        }



        double x1 = nodes[0]->getX();
        double y1 = nodes[0]->getY();
        double x2 = nodes[0]->getX();
        double y2 = nodes[0]->getY();

        foreach (DM::Node *n , nodes) {
            if (x1 > n->getX()) x1 = n->getX();
            if (x2 < n->getX()) x2 = n->getX();
            if (y1 > n->getY()) y1 = n->getY();
            if (y2 < n->getY()) y2 = n->getY();
        }


        DM::Node * n1 = this->city->addNode(x1, y1, 0);
        DM::Node * n2 = this->city->addNode(x1, y2, 0);
        DM::Node * n3 = this->city->addNode(x2, y2, 0);
        DM::Node * n4 = this->city->addNode(x2, y1, 0);


        std::vector<DM::Node *> vF;
        vF.push_back(n1);
        vF.push_back(n2);
        vF.push_back(n3);
        vF.push_back(n4);
        vF.push_back(n1);

        DM::Face * bF = city->addFace(vF, newFaces);

        bF->addAttribute("l", x2-x1);
        bF->addAttribute("b", y2-y1);

        bF->addAttribute("x_min", x1);
        bF->addAttribute("x_max", x2);
        bF->addAttribute("y_min", y1);
        bF->addAttribute("y_max", y2);


    }



    std::vector<std::string> uuids = this->city->getUUIDs(vData);
    if (!this->overAll && this->vData.getType() == DM::FACE) {
        foreach (std::string uuid, uuids) {
            DM::Face * f = city->getFace(uuid);
            QPolygonF poly = TBVectorData::FaceAsQPolgonF(city, f);
            QRectF br = poly.boundingRect();
            qreal * x1 = new double;
            qreal * x2 = new double;
            qreal * y1 = new double;
            qreal * y2 = new double;
            br.getCoords(x1, y1, x2, y2);
            DM::Node * n1 = this->city->addNode(*x1, *y1, 0);
            DM::Node * n2 = this->city->addNode(*x1, *y2, 0);
            DM::Node * n3 = this->city->addNode(*x2, *y2, 0);
            DM::Node * n4 = this->city->addNode(*x2, *y1, 0);


            std::vector<DM::Node *> vF;
            vF.push_back(n1);
            vF.push_back(n2);
            vF.push_back(n3);
            vF.push_back(n4);
            vF.push_back(n1);

            DM::Face * bF = city->addFace(vF, newFaces);

            bF->addAttribute("l", x2-x1);
            bF->addAttribute("b", y2-y1);

            bF->getAttribute(vData.getName())->setLink(vData.getName(), f->getUUID());
            f->getAttribute(newFaces.getName())->setLink(newFaces.getName(), bF->getUUID());

        }
    }
}

void CalculateBoundingBox::caculateMinBoundingBox()
{

    if (this->overAll) {
        std::vector<std::string> uuids = this->city->getUUIDs(vData);
        std::vector<DM::Node *> nodes;
        foreach (std::string uuid, uuids) {
            DM::Face * f = city->getFace(uuid);
            foreach (DM::Node * n, f->getNodePointers()) nodes.push_back(n);

        }
        std::vector<double> size;
        std::vector<DM::Node> ress_nodes;

        CGALGeometry::CalculateMinBoundingBox( nodes, ress_nodes, size );
        std::vector<DM::Node*> vF;
        foreach (DM::Node  n, ress_nodes) {
            vF.push_back(city->addNode(n));
        }
        vF.push_back(vF[0]);


        DM::Face * bF = city->addFace(vF, newFaces);

        bF->addAttribute("l",size[0]);
        bF->addAttribute("b",size[1]);

        return;
    }

    std::vector<std::string> uuids = this->city->getUUIDs(vData);
    foreach (std::string uuid, uuids) {
        DM::Face * f = city->getFace(uuid);
        std::vector<double> size;
        std::vector<DM::Node> ress_nodes;

        CGALGeometry::CalculateMinBoundingBox( TBVectorData::getNodeListFromFace(city, f), ress_nodes, size );
        std::vector<DM::Node*> vF;
        foreach (DM::Node  n, ress_nodes) {
            vF.push_back(city->addNode(n));
        }
        vF.push_back(vF[0]);


        DM::Face * bF = city->addFace(vF, newFaces);

        bF->addAttribute("l",size[0]);
        bF->addAttribute("b",size[1]);

        bF->getAttribute(vData.getName())->setLink(vData.getName(), f->getUUID());
        f->getAttribute(newFaces.getName())->setLink(newFaces.getName(), bF->getUUID());

    }
}

void CalculateBoundingBox::getNodesFromNodes(std::vector<Node *> &nodes)
{
    std::vector<std::string> uuids = this->city->getUUIDs(vData);
    foreach (std::string uuid, uuids) {
        DM::Node * n = city->getNode(uuid);
        nodes.push_back(n);
    }
}

void CalculateBoundingBox::getNodesFromEdges(std::vector<Node *> &nodes)
{
    std::vector<std::string> uuids = this->city->getUUIDs(vData);
    foreach (std::string uuid, uuids) {
        DM::Edge * e = city->getEdge(uuid);
        nodes.push_back(city->getNode(e->getStartpointName()));
        nodes.push_back(city->getNode(e->getEndpointName()));
    }
}

void CalculateBoundingBox::getNodesFromFaces(std::vector<Node *> &nodes)
{
    std::vector<std::string> uuids = this->city->getUUIDs(vData);
    foreach (std::string uuid, uuids) {
        DM::Face * f = city->getFace(uuid);
        std::vector<DM::Node*> nl = TBVectorData::getNodeListFromFace(city, f);
        foreach (DM::Node * n, nl)
            nodes.push_back(n);

    }
}

void CalculateBoundingBox::run() {
    city = this->getData("Data");

    if (this->MinBounding) {
        caculateMinBoundingBox();
        return;
    }
    caculateBoundingBox();
}

std::string CalculateBoundingBox::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/CalculateBoundingBox";
}
