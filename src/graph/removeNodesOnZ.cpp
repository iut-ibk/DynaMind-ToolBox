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

#include <removeNodesOnZ.h>

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

DM_DECLARE_NODE_NAME(RemoveNodesOnZ,Modules)

RemoveNodesOnZ::RemoveNodesOnZ()
{
	std::vector<DM::View> views;
	DM::View view;

	view = DM::View("NODES", DM::NODE, DM::MODIFY);
	views.push_back(view);
	viewdef["NODES"]=view;

	this->addData("Layout", views);

	this->max=0;
	this->addParameter("Max height [m]", DM::DOUBLE, &this->max);
}

void RemoveNodesOnZ::run()
{
	sys = this->getData("Layout");
	std::vector<DM::Component*> nodes(sys->getAllComponentsInView(viewdef["NODES"]));

	if(!nodes.size())
	{
		DM::Logger(DM::Error) << "Could not find nodes";
		return;
	}

	for(int index=0; index < nodes.size(); index++)
	{
		DM::Node *currentnode = dynamic_cast<DM::Node*>(nodes[index]);
		if(currentnode->getZ()>max)
			sys->removeComponentFromView(currentnode,viewdef["NODES"]);
	}
	return;
}
