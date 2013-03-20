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

#include <setZfromRasterdata.h>

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

DM_DECLARE_NODE_NAME(SetZfromRasterdata,Modules)

SetZfromRasterdata::SetZfromRasterdata()
{   
    std::vector<DM::View> views;
    DM::View view;

    view = DM::View("NODES", DM::NODE, DM::MODIFY);
    views.push_back(view);
    viewdef["NODES"]=view;

    //Nodes which should be connected to the graph
    view = DM::View("ELEVATION", DM::RASTERDATA, DM::READ);
    views.push_back(view);
    viewdef["ELEVATION"]=view;

    this->addData("Layout", views);
}

void SetZfromRasterdata::run()
{
    sys = this->getData("Layout");
    r = this->getRasterData("Layout", viewdef["ELEVATION"]);
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef["NODES"]));

    if(!r || !nodes.size())
    {
        DM::Logger(DM::Error) << "Could not find Rasterdata and/or nodes";
        return;
    }

    for(int index=0; index < nodes.size(); index++)
    {
        DM::Node *currentnode = sys->getNode(nodes[index]);
        currentnode->setZ(r->getValue(currentnode->getX(),currentnode->getY()));
    }
    return;
}
