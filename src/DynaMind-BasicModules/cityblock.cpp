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

#include "cityblock.h"
#include <dm.h>
#include <math.h>
#include <tbvectordata.h>
#include <dmgeometry.h>
#include <cgalgeometry.h>

DM_DECLARE_NODE_NAME(CityBlock,BlockCity)

CityBlock::CityBlock()
{
	this->width = 100;
	this->height = 100;
	this->createStreets = true;
	this->addParameter("Width", DM::DOUBLE, &this->width);
	this->addParameter("Height", DM::DOUBLE, &this->height);
	devider = 100;
	this->addParameter("CreateStreets", DM::BOOL, &this->createStreets);
	offset = 7.5;
	this->addParameter("Offset", DM::DOUBLE, &this->offset);
	SuperBlockName = "SUPERBLOCK";
	BlockName = "CITYBLOCK";
	EdgeName = "STREET";
	CenterCityblockName = "CENTERCITYBLOCK";
	InterSectionsName = "INTERSECTION";

	this->addParameter("SuperBlockName", DM::STRING, &this->SuperBlockName);
	this->addParameter("BlockName", DM::STRING, &this->BlockName);
	this->addParameter("EdgeName", DM::STRING, &this->EdgeName);
	this->addParameter("CenterCityblockName", DM::STRING, &this->CenterCityblockName);
	this->addParameter("InterSectionsName", DM::STRING, &this->InterSectionsName);
	std::vector<DM::View> views;
	views.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::MODIFY));
	this->addData("City", views);
}

void CityBlock::init()
{
	std::vector<DM::View> views;
	superblock = DM::View(SuperBlockName, DM::FACE, DM::READ);
	cityblock = DM::View(BlockName, DM::FACE, DM::WRITE);
	cityblock.addAttribute("area");
	cityblock.addAttribute("width");
	cityblock.addAttribute("height");



	intersections = DM::View(InterSectionsName, DM::NODE, DM::WRITE);
	centercityblock = DM::View(CenterCityblockName, DM::NODE, DM::WRITE);
	centercityblock.addAttribute("centroid_x");
	centercityblock.addAttribute("centroid_y");
	centercityblock.addLinks(BlockName, cityblock);
	cityblock.addLinks(CenterCityblockName, centercityblock);

	if (this->createStreets) {
		streets = DM::View(EdgeName, DM::EDGE, DM::WRITE);
		streets.addAttribute("width");

		cityblock.addLinks(EdgeName,streets);
		streets.addLinks(BlockName, cityblock);

		views.push_back(streets);
	}

	views.push_back(superblock);
	views.push_back(cityblock);

	views.push_back(intersections);
	views.push_back(centercityblock);




	this->addData("City", views);
}

string CityBlock::getHelpUrl()
{
	return  "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/CityBlock.md";
}

void CityBlock::run() 
{
	DM::System * city = this->getData("City");

	DM::SpatialNodeHashMap nodeList(city, devider);

	//std::vector<std::string> blockids = city->getUUIDsOfComponentsInView(superblock);

	foreach (DM::Component* c, city->getAllComponentsInView(superblock))
	{
		//calulculate height;
		DM::Face * fblock = (DM::Face*)c;

		double minX = 0;
		double maxX = 0;
		double minY = 0;
		double maxY = 0;

		int i = 0;
		double v[3];
		double x,y;

		foreach(DM::Node* n, fblock->getNodePointers())
		{
			n->get(v);
			x = v[0];
			y = v[1];

			if (i++ == 0) 
			{
				minX = maxX = x;
				minY = maxY = y;
			}
			else
			{
				minX = min(x,minX);
				minY = min(y,minY);
				maxX = max(x,maxX);
				maxY = max(y,maxY);
			}
		}

		double blockWidth = maxX - minX;
		double blockHeight = maxY - minY;

		DM::Logger(DM::Debug) << blockHeight;
		DM::Logger(DM::Debug) << blockWidth;

		//Create Parcels
		int elements_x = blockWidth/this->width;
		int elements_y = blockHeight/this->height;
		double realwidth = blockWidth / elements_x;
		double realheight = blockHeight / elements_y;
		fblock->addAttribute("width",realwidth);
		fblock->addAttribute("height",realheight);
		StartAndEndNodeList.clear();

		int counter = 0;
		for (int x = 0; x < elements_x; x++) {
			for (int y = 0; y < elements_y; y++) {
				counter++;
				DM::Node * n1 = nodeList.addNode(minX + realwidth*x,minY + realheight*y,0,
												 .001, intersections);
				DM::Node * n2 = nodeList.addNode( minX + realwidth*(x+1),minY + realheight*y,0,
												  .001, intersections);
				DM::Node * n3 = nodeList.addNode(minX + realwidth*(x+1),minY + realheight*(y+1),0,
												 .001, intersections);
				DM::Node * n4 = nodeList.addNode(minX + realwidth*x,minY + realheight*(y+1),0,
												 .001, intersections);

				DM::Edge * e1 = getAlreadyCreateEdge(n1, n2);
				DM::Edge * e2 = getAlreadyCreateEdge(n2, n3);
				DM::Edge * e3 = getAlreadyCreateEdge(n3, n4);
				DM::Edge * e4 = getAlreadyCreateEdge(n4, n1);

				//Every Edge is also a Street
				if (e1 == 0) {
					e1 = city->addEdge(n1, n2);
					if (createStreets) {
						city->addComponentToView(e1, streets);
						e1->addAttribute("width", this->offset*2);
					}
					this->addEdge(e1, n1, n2);
				}
				if (e2 == 0) {
					e2 = city->addEdge(n2, n3);
					if (createStreets) {
						city->addComponentToView(e2, streets);
						e2->addAttribute("width", this->offset*2);
					}
					this->addEdge(e2, n2, n3);
				}
				if (e3 == 0) {
					e3 = city->addEdge(n3, n4);
					if (createStreets) {
						city->addComponentToView(e3, streets);
						e3->addAttribute("width", this->offset*2);
					}
					this->addEdge(e3, n3, n4);
				}
				if (e4 == 0) {
					e4 = city->addEdge(n4, n1);
					if (createStreets) {
						city->addComponentToView(e4, streets);
						e4->addAttribute("width", this->offset*2);
					}
					this->addEdge(e4, n4, n1);
				}

				//Create Face
				std::vector<DM::Node*> ve;
				ve.push_back(n1);
				ve.push_back(n2);
				ve.push_back(n3);
				ve.push_back(n4);

				std::vector<DM::Node> offest_nodes;
				offest_nodes = DM::CGALGeometry::OffsetPolygon(ve, this->offset);

				std::vector<DM::Node*> face_nodes;
				foreach (DM::Node n, offest_nodes) {
					face_nodes.push_back(city->addNode(n));
				}
				if (face_nodes.size() < 3) {
					DM::Logger(DM::Warning) << "Couldn't create face";
					continue;
				}
				face_nodes.push_back(face_nodes[0]);

				DM::Face * f = city->addFace(face_nodes, cityblock);
				f->addAttribute("area", realwidth*realheight);
				DM::Node * n =city->addNode(minX + realwidth*(x+0.5),minY + realheight*(y+0.5),0, centercityblock);
				n->addAttribute("centroid_x", n->getX());
				n->addAttribute("centroid_y", n->getY());


				//Create Links
				n->getAttribute(cityblock.getName())->setLink(cityblock.getName(), f->getUUID());
				f->getAttribute(centercityblock.getName())->setLink(centercityblock.getName(), n->getUUID());

				if (createStreets) {
					e1->getAttribute(cityblock.getName())->setLink(cityblock.getName(),f->getUUID());
					e2->getAttribute(cityblock.getName())->setLink(cityblock.getName(),f->getUUID());
					e3->getAttribute(cityblock.getName())->setLink(cityblock.getName(),f->getUUID());
					e4->getAttribute(cityblock.getName())->setLink(cityblock.getName(),f->getUUID());

					f->getAttribute(streets.getName())->setLink(streets.getName(), e1->getUUID());
					f->getAttribute(streets.getName())->setLink(streets.getName(), e2->getUUID());
					f->getAttribute(streets.getName())->setLink(streets.getName(), e3->getUUID());
					f->getAttribute(streets.getName())->setLink(streets.getName(), e4->getUUID());
				}
			}
		}
	}

}


DM::Edge * CityBlock::getAlreadyCreateEdge(DM::Node * n1, DM::Node* n2) {
	if (StartAndEndNodeList.find(n1) == StartAndEndNodeList.end())
		return 0;
	std::map<DM::Node*, DM::Edge* > submap = StartAndEndNodeList[n1];
	if (submap.find(n2) == submap.end())
		return 0;
	return submap[n2];
}

void CityBlock::addEdge(DM::Edge *e, DM::Node * n1, DM::Node * n2) {
	if (StartAndEndNodeList.find(n1) == StartAndEndNodeList.end()) {
		StartAndEndNodeList[n1] = std::map<DM::Node*, DM::Edge* >();
	}
	std::map<DM::Node*, DM::Edge* > submap = StartAndEndNodeList[n1];
	submap[n2] = e;
	StartAndEndNodeList[n1] = submap;
	if (StartAndEndNodeList.find(n2) == StartAndEndNodeList.end()) {
		StartAndEndNodeList[n2] = std::map<DM::Node*, DM::Edge* >();
	}
	submap = StartAndEndNodeList[n2];
	submap[n1] = e;
	StartAndEndNodeList[n2] = submap;
}


