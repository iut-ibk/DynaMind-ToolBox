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
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/SpatialLinking.md";
}

template<typename T>
void flip(T a, T b)
{
	T c = a;
	a = b;
	b = c;
}

void SpatialLinking::run() 
{
	city = this->getData("Data");

	//Node id that point to elements in the vector are stored in Hashmap for faster lookup
	typedef QPair<QPointF, std::vector<Component*> > Bucket;
	QHash<QPair<int, int>, Bucket > nodesMap;

	// Init Point List
	foreach(DM::Component* c, city->getAllComponentsInView(vbase))
	{
		double v[3];

		if (vbase.getType() == DM::FACE)
			DM::CGALGeometry::CalculateCentroid(city, (Face*)c, v[0], v[1], v[2]);
		else if (vbase.getType() == DM::NODE) 
			((Node*)c)->get(v);

		int x = v[0] / spatialL;
		int y = v[1] / spatialL;

		Bucket& it = nodesMap[QPair<int,int>(x,y)];
		it.first = QPointF(v[0], v[1]);
		it.second.push_back(c);
	}
	int CounterElementLinked = 0;

	foreach(Component* baseCmp, city->getAllComponentsInView(vlinkto))
	{
		QPolygonF qf = TBVectorData::FaceAsQPolgonF(city, (Face*)baseCmp);

		// Search Space
		double xb;
		double yb;
		double hb;
		double wb;
		TBVectorData::getBoundingBox(((Face*)baseCmp)->getNodePointers(), xb, yb, hb, wb, true);

		int xmin = (int) (xb) / spatialL-1;
		int ymin = (int) (yb) /spatialL-1;
		int xmax = (int) (xb+wb)/spatialL+1;
		int ymax = (int) (yb+hb)/spatialL+1;
		Logger(Debug) << xmin << "|" << ymin << "|" << xmax << "|"<< ymax;

		if (xmin > xmax) 
			flip(xmin, xmax);

		if (ymin > ymax)
			flip(ymin, ymax);
		
		Attribute* baseAttribute = baseCmp->getAttribute(base);

		int elementInSearchSpace = 0;
		for (int x = xmin; x <= xmax; x++) 
		{
			for (int y = ymin; y <= ymax; y++) 
			{
				const Bucket& bucket = nodesMap[QPair<int,int>(x,y)];
				const std::vector<Component*>& elementsInBucket = bucket.second;
				if (elementsInBucket.empty())
					continue;

				elementInSearchSpace = elementInSearchSpace + elementsInBucket.size();
				foreach (Component* it, elementsInBucket) 
				{
					if (qf.containsPoint(bucket.first, Qt::WindingFill)) 
					{
						Attribute * attr = it->getAttribute(linkto);
						std::vector<Component*> linkedComponents = attr->getLinkedComponents();
						if (std::find(linkedComponents.begin(), linkedComponents.end(), it) == linkedComponents.end()) 
							attr->addLink(it, linkto);

						baseAttribute->addLink(it, base);

						CounterElementLinked++;
					}

				}

			}
		}
		Logger(Debug) << "Element in search space " << elementInSearchSpace;
		Logger(Debug) << "Linked to " << baseAttribute->getLinkedComponents().size();
	}

	Logger(DM::Debug) << "Elements Linked " << CounterElementLinked;
}
