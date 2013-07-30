/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#include "spatiallinking.h"
#include <tbvectordata.h>
#include <cgalgeometry.h>

#include <QHash>
#include <QPolygonF>

#ifndef __clang__
#include <omp.h>
#endif


DM_DECLARE_NODE_NAME(SpatialLinking, Modules)

SpatialLinking::SpatialLinking()
{
    this->city = 0;
    this->base = "";
    this->linkto = "";
    spatialL = 1000;

    this->addParameter("Base", DM::STRING, & this->base);
    this->addParameter("Link", DM::STRING, & this->linkto);

    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addData("Data", data);

}

void SpatialLinking::init() {
    /*city = this->getData("Data");
    if (city == 0)
        return;*/
    if (base.empty())
        return;
    if (linkto.empty())
        return;
	
    //DM::View * v1 = this->city->getViewDefinition(base);
    //DM::View * v2 = this->city->getViewDefinition(linkto);
	DM::View v1 = getViewInStream("Data", base);
    DM::View v2 = getViewInStream("Data", linkto);

	if (v1.getName().empty()) {
        Logger(Error) << "View " << base << "doesn't exist";
        return;
    }

    if (v2.getName().empty()) {
        Logger(Error) << "View " << linkto << "doesn't exist";
        return;
    }
    this->vbase = DM::View(v1.getName(), v1.getType(), READ);
    this->vlinkto = DM::View(v2.getName(), v2.getType(), READ);

	this->vbase.addLinks(linkto, vlinkto.getName());
    this->vlinkto.addLinks(base, vbase.getName());

    std::vector<DM::View> data;
    data.push_back(vbase);
    data.push_back(vlinkto);
    this->addData("Data", data);
}

string SpatialLinking::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/SpatialLinking";
}

void SpatialLinking::run() {
    city = this->getData("Data");
    std::vector<std::string> baseUUIDs = city->getUUIDsOfComponentsInView(vbase);
    std::vector<QPointF> centerPoints;

    //Node id that point to elements in the vector are stored in Hashmap for faster lookup
    QHash<QPair<int, int>, std::vector<int> * > nodesMap;

    //Init Point List
    int counterID = -1;
    foreach (std::string baseUUID, baseUUIDs) {
        counterID++;

		double v[3];

        if (vbase.getType() == DM::FACE) {
            Face * f = city->getFace(baseUUID);
            DM::CGALGeometry::CalculateCentroid(city, f, v[0], v[1]);
        }
        else if (vbase.getType() == DM::NODE) {
            city->getNode(baseUUID)->get(v);
        }
        centerPoints.push_back(QPointF(v[0], v[1]));

        int x = v[0] / spatialL;
        int y = v[1] / spatialL;


        QPair<int,int> key(x,y);
        std::vector<int> * vn;
        if (!nodesMap.contains(key)) {
            vn = new std::vector<int>();
            nodesMap[key] = vn;
        }
        vn = nodesMap[key];
        vn->push_back(counterID);
    }
    //Check if Center point is within toLink

    std::vector<std::string> linkUUIDs = city->getUUIDsOfComponentsInView(vlinkto);

    int CounterElementLinked = 0;
    int NumberOfLinks = linkUUIDs.size();

    for (int i = 0; i < NumberOfLinks; i++) {
        std::string linkUUID = linkUUIDs[i];
        QPolygonF qf = TBVectorData::FaceAsQPolgonF(city, city->getFace(linkUUID));

        //Search Space
        double xb;
        double yb;
        double hb;
        double wb;
        //TBVectorData::getBoundingBox(city->getFace(linkUUID)->getNodePointers(), xb, yb, hb, wb,true); TODODODODO
        /*int xmin = (int) (qf.boundingRect().left()) / spatialL-1;
        int ymin = (int) (qf.boundingRect().bottom()) /spatialL-1;
        int xmax = (int) (qf.boundingRect().right())/spatialL+1;
        int ymax = (int) (qf.boundingRect().top())/spatialL+1;*/

        int xmin = (int) (xb) / spatialL-1;
        int ymin = (int) (yb) /spatialL-1;
        int xmax = (int) (xb+wb)/spatialL+1;
        int ymax = (int) (yb+hb)/spatialL+1;
        Logger(Debug) << xmin << "|" << ymin << "|" << xmax << "|"<< ymax;

        if (xmin > xmax) {
            double tmp_x = xmin;
            xmin = xmax;
            xmax = tmp_x;
        }

        if (ymin > ymax) {
            double tmp_y = ymin;
            ymin = ymax;
            ymax = tmp_y;
        }
        std::vector<LinkAttribute> links;
        int elementInSearchSpace = 0;
        for (int x = xmin; x <= xmax; x++) {
            for (int y = ymin; y <= ymax; y++) {
                QPair<int,int> key(x,y);
                //Test Each Key
                std::vector<int> * centers = nodesMap[key];
                if (!centers) {
                    continue;
                }
                elementInSearchSpace=elementInSearchSpace+centers->size();
                foreach (int id, (*centers)) {
                    if (qf.containsPoint(centerPoints[id], Qt::WindingFill)) {
                        LinkAttribute lto;
                        lto.viewname = linkto;
                        lto.uuid = linkUUID;
                        Component * cmp = city->getComponent(baseUUIDs[id]);
                        Attribute * attr = cmp->getAttribute(linkto);
                        std::vector<LinkAttribute>  ls = attr->getLinks();
                        if (std::find(ls.begin(), ls.end(), lto) == ls.end()) ls.push_back(lto);
                        //else Logger(Standard) << "Link already existed";
                        attr->setLinks(ls);

                        LinkAttribute lbase;
                        lbase.viewname = base;
                        lbase.uuid = cmp->getUUID();

                        links.push_back(lbase);
                        CounterElementLinked++;
                    }

                }

            }
        }
        Logger(Debug) << "Element in search space " << elementInSearchSpace;
        Logger(Debug) << "Linked to " << links.size();
        Component * cmp = city->getComponent(linkUUID);
        Attribute * attr = cmp->getAttribute(base);
        attr->setLinks(links);
    }

    QList<QPair<int, int> >keys  = nodesMap.keys();

    for (int i = 0; i < keys.size(); i++) {
        delete nodesMap[keys[i]];
    }
    nodesMap.clear();

    Logger(DM::Debug) << "Elements Linked " << CounterElementLinked;



}
