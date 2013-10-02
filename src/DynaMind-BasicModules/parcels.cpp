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

#include "parcels.h"
#include <tbvectordata.h>

DM_DECLARE_NODE_NAME(Parcels,BlockCity)

Parcels::Parcels()
{
	cityblock = DM::View("CITYBLOCK", DM::FACE, DM::READ);
	parcels = DM::View("PARCEL", DM::FACE, DM::WRITE);

	std::vector<DM::View> views;

	width = 22;
	this->addParameter("Width", DM::DOUBLE, &width);
	p_height = 18.2;
	this->addParameter("Height", DM::DOUBLE, &p_height);

	views.push_back(cityblock);
	views.push_back(parcels);


	this->addData("City", views);

}
/**
  * Input Shape
  *
  * 3----2
  * |    |
  * 0----1
  *
  */
void Parcels::run() {
	DM::System * city = this->getData("City");
	foreach (DM::Component* c, city->getAllComponentsInView(cityblock))
	{
		DM::Face * block = (DM::Face*)c;
		//Get minx and miny as origin
		double minx;
		double miny;
		double maxx;
		double maxy;
		std::vector<DM::Node *> nodelist =  TBVectorData::getNodeListFromFace(city, block);
		for (unsigned int i = 0; i < nodelist.size(); i++) {
			if (i == 0) {
				minx = nodelist[i]->getX();
				miny = nodelist[i]->getY();
				maxx = nodelist[i]->getX();
				maxy = nodelist[i]->getY();
				continue;
			}
			if (minx > nodelist[i]->getX())
				minx = nodelist[i]->getX();
			if (miny > nodelist[i]->getY())
				miny = nodelist[i]->getY();
			if (maxx < nodelist[i]->getX())
				maxx = nodelist[i]->getX();
			if (maxy < nodelist[i]->getY())
				maxy = nodelist[i]->getY();
		}
		DM::Node origin = DM::Node(minx, miny, 0);
		DM::Node delta = DM::Node(maxx, maxy,0) -origin;
		double length = delta.getX();
		double height = delta.getY();

		//Create Parcels
		int elements_x = length/width;
		int elements_y = height/p_height;
		double realwidth = length / elements_x;
		double realheight = height / elements_y;

		/**Generation Process -> left to right
	  *     |    |
	  * ----|----|
	  *     |    |
	  *     1    2
	*/
		for (int x = 0; x < elements_x; x++) {
			for (int y = 0; y < elements_y; y++) {
				DM::Node * n1 = city->addNode( origin+DM::Node(realwidth*x,realheight*y,0));
				DM::Node * n2 = city->addNode( origin+DM::Node(realwidth*(x+1), realheight*y,0));
				DM::Node * n3 = city->addNode( origin+DM::Node( realwidth*(x+1), realheight*(y+1),0));
				DM::Node * n4 = city->addNode( origin+DM::Node (realwidth*x,realheight*(y+1),0));

				std::vector<DM::Node*> ve;
				ve.push_back(n1);
				ve.push_back(n2);
				ve.push_back(n3);
				ve.push_back(n4);
				ve.push_back(n1);

				DM::Face * f = city->addFace(ve, parcels);

			}
		}
	}
	DM::Logger(DM::Debug) << "Number of Parcels " << city->getAllComponentsInView(parcels).size();
}

string Parcels::getHelpUrl()
{
		return  "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/Parcels.md";
}
