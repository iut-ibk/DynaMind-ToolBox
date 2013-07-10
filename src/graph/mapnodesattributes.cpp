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

#include <mapnodesattributes.h>

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

DM_DECLARE_NODE_NAME(MapNodes2Graph,Graph)

MapNodes2Graph::MapNodes2Graph()
{   
    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter street network
    view = DM::View("EDGES", DM::EDGE, DM::READ);
    views.push_back(view);
    viewdef["EDGES"]=view;

    //Define Parameter street network
    view = DM::View("NODES", DM::NODE, DM::MODIFY);
    view.addAttribute("Weight");
    views.push_back(view);
    viewdef["NODES"]=view;

    //Nodes which should be connected to the graph
    view = DM::View("CONNECTINGNODES", DM::NODE, DM::MODIFY);
    views.push_back(view);
    viewdef["CONNECTINGNODES"]=view;

    this->addData("Layout", views);
    this->sum=true;
    this->addParameter("Sum attributes", DM::BOOL, &this->sum);
}

void MapNodes2Graph::run()
{
    typedef CGAL::Simple_cartesian<double> K;
    typedef K::Point_2  Point_2;
    typedef CGAL::Search_traits_2<K> TreeTraits;
    typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
    typedef Neighbor_search::Tree Tree;
    typedef std::map<std::string, DM::Component*>::iterator Itr;

    this->sys = this->getData("Layout");
    std::map<std::string, DM::Component*> nodes = sys->getAllComponentsInView(viewdef["NODES"]);
    std::map<std::string, DM::Component*> connectingnodes = sys->getAllComponentsInView(viewdef["CONNECTINGNODES"]);
    std::map<std::pair<int,int>,DM::Node*> nodemap;
    int notconnectedcounter=0;

    if(!nodes.size())
    {
        DM::Logger(DM::Error) << "Graph does not contain any nodes -> EMPTY GRAPH";
        return;
    }

    if(!connectingnodes.size())
    {
        DM::Logger(DM::Error) << "No connecting nodes are existing";
        return;
    }

    std::list<Point_2> Lr;

    for(Itr i = nodes.begin(); i != nodes.end(); ++i)
    {
        DM::Node *currentnode = static_cast<DM::Node*>((*i).second);
        int x = currentnode->getX();
        int y = currentnode->getY();

        nodemap[std::pair<int,int>(x,y)]=currentnode;
        Lr.push_back(Point_2(x,y));
    }

    Tree tree(Lr.begin(),Lr.end());

    std::list<DM::Node*> newconnecting;

    for(Itr i = connectingnodes.begin(); i != connectingnodes.end(); ++i)
    {
        DM::Node *connectingnode = static_cast<DM::Node*>((*i).second);
        int x = connectingnode->getX();
        int y = connectingnode->getY();

        Neighbor_search search(tree, Point_2(x,y), 1);

        Neighbor_search::iterator it = search.begin();

        Point_2 fn = it->first;

        DM::Node* nearest = nodemap[std::pair<int,int>(fn.x(),fn.y())];
        newconnecting.push_back(nearest);

        //calculate new attributes
        double newvalue =0;
        if(this->sum)
            newvalue = connectingnode->getAttribute("Weight")->getDouble() + nearest->getAttribute("Weight")->getDouble();
        else
            newvalue = connectingnode->getAttribute("Weight")->getDouble();
        nearest->changeAttribute("Weight",newvalue);
    }

    //delete old connecting view
    for(Itr i = connectingnodes.begin(); i != connectingnodes.end(); ++i)
        sys->removeComponentFromView((*i).second,viewdef["CONNECTINGNODES"]);

    //instert new connecting nodes
    for(std::list<DM::Node*>::iterator i = newconnecting.begin(); i != newconnecting.end(); ++i)
        sys->addComponentToView((*i),viewdef["CONNECTINGNODES"]);

    DM::Logger(DM::Standard) << "Not maped nodes: " << notconnectedcounter;

}
