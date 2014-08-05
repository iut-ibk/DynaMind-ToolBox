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

#include <skeletonize.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

DM_DECLARE_NODE_NAME(Skeletonize,Graph)

Skeletonize::Skeletonize()
{
	std::vector<DM::View> views;
	DM::View view;
	DM::GRAPH::ViewDefinitionHelper defhelper;
	this->viewname="EDGES";
	this->addParameter("View name", DM::STRING, &this->viewname);

	//Define Parameter street network
	view = DM::View(this->viewname,DM::EDGE,DM::MODIFY);
	views.push_back(view);
	viewdef[this->viewname]=view;

	this->addData("Layout", views);
}

void Skeletonize::run()
{
	this->sys = this->getData("Layout");

	std::vector<DM::Component*> edges = sys->getAllComponentsInView(viewdef[this->viewname]);

	typedef std::vector<DM::Component*>::iterator itr;

	DM::Logger(DM::Standard) << "Number of Edges found:" << edges.size();

	for(itr i = edges.begin(); i != edges.end(); i++)
	{
		std::vector<DM::Edge*> neighbours;
		DM::Edge *edge = static_cast<DM::Edge*>((*i));
		DM::Node *sname = edge->getStartNode();

		getNeighbourEdges(sname,neighbours,edges);

		if(neighbours.size()==2)
		{
			neighbours.erase(std::find(neighbours.begin(),neighbours.end(),edge));
			DM::Edge* nedge = neighbours[0];

			DM::Node* newsname = nedge->getStartNode();

			if(newsname==sname)
				newsname=nedge->getEndNode();


			edge->setStartpoint(newsname);
			sys->removeComponentFromView(nedge,viewdef[this->viewname]);
		}
	}
}

void Skeletonize::init()
{
	viewdef.clear();
	DM::View view = DM::View(this->viewname,DM::EDGE,DM::MODIFY);
	std::vector<DM::View> views;
	views.push_back(view);
	viewdef[this->viewname]=view;
	this->addData("Layout", views);
}

bool Skeletonize::getNeighbourEdges(DM::Node *node, std::vector<DM::Edge*> &neighbours, std::vector<DM::Component*> alledges)
{
	neighbours.clear();
	std::vector<DM::Edge*> e = node->getEdges();

	for(uint index=0; index < e.size(); index++)
		if(std::find(alledges.begin(),alledges.end(),e[index])!=alledges.end())
			neighbours.push_back(static_cast<DM::Edge*>(e[index]));
	return true;
}
