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

#include <connectnodes2graph.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>
#include <graphviewdef.h>

//CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

DM_DECLARE_NODE_NAME(ConnectNodes2Graph,Graph)

ConnectNodes2Graph::ConnectNodes2Graph()
{
	std::vector<DM::View> views;
	DM::View view;

	//Define Parameter street network
	view = DM::View("EDGES", DM::EDGE, DM::WRITE);
	views.push_back(view);
	viewdef["EDGES"]=view;

	//Define Parameter street network
	view = DM::View("NODES", DM::NODE, DM::WRITE);
	views.push_back(view);
	viewdef["NODES"]=view;

	//Nodes which should be connected to the graph
	view = DM::View("CONNECTINGNODES", DM::NODE, DM::READ);
	views.push_back(view);
	viewdef["CONNECTINGNODES"]=view;

	this->addData("Layout", views);
}

void ConnectNodes2Graph::run()
{
	typedef CGAL::Simple_cartesian<double> K;
	typedef K::Point_2  Point_2;
	typedef CGAL::Search_traits_2<K> TreeTraits;
	typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
	typedef Neighbor_search::Tree Tree;

	DM::GRAPH::ViewDefinitionHelper defhelper_graph;
	this->sys = this->getData("Layout");
	std::vector<DM::Component*> nodes(sys->getAllComponentsInView(viewdef["NODES"]));
	std::vector<DM::Component*> connectingnodes(sys->getAllComponentsInView(viewdef["CONNECTINGNODES"]));
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

	for(int index=0; index<nodes.size(); index++)
	{
		DM::Node *currentnode = dynamic_cast<DM::Node*>(nodes[index]);
		int x = currentnode->getX();
		int y = currentnode->getY();

		nodemap[std::pair<int,int>(x,y)]=currentnode;
		Lr.push_back(Point_2(x,y));
	}

	Tree tree(Lr.begin(),Lr.end());

	for(int index=0; index < connectingnodes.size(); index++)
	{
		DM::Node *connectingnode = dynamic_cast<DM::Node*>(connectingnodes[index]);
		int x = connectingnode->getX();
		int y = connectingnode->getY();

		Neighbor_search search(tree, Point_2(x,y), 1);

		Neighbor_search::iterator it = search.begin();

		Point_2 fn = it->first;

		DM::Node* nearest = nodemap[std::pair<int,int>(fn.x(),fn.y())];

		//std::string nearest = findNearestNode(nodes,connectingnode);
		DM::Edge* newedge = sys->addEdge(connectingnode,nearest,viewdef["EDGES"]);
		double weight = TBVectorData::calculateDistance(connectingnode,nearest);
		newedge->changeAttribute(defhelper_graph.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight),weight);
		sys->addComponentToView(connectingnode,viewdef["NODES"]);
	}

	DM::Logger(DM::Standard) << "Not connected nodes: " << notconnectedcounter;

}

std::string ConnectNodes2Graph::findNearestNode(std::vector<DM::Node*>& nodes, DM::Node *connectingNode)
{
	double currentdistance=TBVectorData::calculateDistance(nodes[0],connectingNode);
	DM::Node *node = nodes[0];

	for(int index=1; index<nodes.size(); index++)
	{
		double distance = TBVectorData::calculateDistance(nodes[index],connectingNode);

		if(currentdistance > distance)
		{
			currentdistance=distance;
			node=nodes[index];
		}
	}

	return node->getUUID();
}
