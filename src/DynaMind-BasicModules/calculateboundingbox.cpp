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
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addParameter("NameOfExistingView", DM::STRING, & this->NameOfExistingView);
    this->addParameter("MinBounding", DM::BOOL, & this->MinBounding);

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

    std::stringstream ss;
    ss << NameOfExistingView << "_BOUNDING_BOX";

    newFaces = DM::View(ss.str(), DM::FACE, DM::WRITE);
    newFaces.addAttribute("l");
    newFaces.addAttribute("b");

    DM::View * v = city->getViewDefinition(NameOfExistingView);
    DM::View writeView = DM::View(v->getName(), v->getType(), DM::READ);
    writeView.addLinks(ss.str(), newFaces);

    newFaces.addLinks(writeView.getName(), writeView);


    std::vector<DM::View> data;
    data.push_back(writeView);
    data.push_back(newFaces);
    this->addData("Data", data);
    vData = writeView;

    this->updateParameter();
}

void CalculateBoundingBox::caculateBoundingBox()
{
    std::vector<std::string> uuids = this->city->getUUIDs(vData);

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

void CalculateBoundingBox::caculateMinBoundingBox()
{
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

void CalculateBoundingBox::run() {

    city = this->getData("Data");

    if (this->MinBounding) {
        caculateMinBoundingBox();
        return;
    }
    caculateBoundingBox();







}
