/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#include <calculatelength.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

DM_DECLARE_NODE_NAME(CalculateEdgeLength,Graph)

CalculateEdgeLength::CalculateEdgeLength()
{
	std::vector<DM::View> views;
	DM::View view;

	view = defhelper.getView(DM::GRAPH::EDGES,DM::READ);
	defhelper.setAttribute(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight,view,DM::WRITE);
	views.push_back(view);
	viewdef[DM::GRAPH::EDGES]=view;

	this->addData("Layout", views);
}

void CalculateEdgeLength::run()
{
	sys = this->getData("Layout");
	std::vector<DM::Component*> edges(sys->getAllComponentsInView(viewdef[DM::GRAPH::EDGES]));

	if(!edges.size())
	{
		DM::Logger(DM::Error) << "Could not find any edges";
		return;
	}

	for(uint index=0; index < edges.size(); index++)
	{
		DM::Edge *currentedge = dynamic_cast<DM::Edge*>(edges[index]);
		DM::Node *start = currentedge->getStartNode();
		DM::Node *end = currentedge->getEndNode();
		double length = TBVectorData::calculateDistance(start,end);
		currentedge->addAttribute(defhelper.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight),length);
	}
	return;
}
