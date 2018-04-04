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

#include <setoutfallatdeepestpoint.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <sewerviewdef.h>
#include <dynamindboostgraphhelper.h>

using namespace DM;

DM_DECLARE_NODE_NAME(SetOutfallAtDeepestPoint,Sewersystems)

SetOutfallAtDeepestPoint::SetOutfallAtDeepestPoint()
{
}

void SetOutfallAtDeepestPoint::init()
{
	S::ViewDefinitionHelper sd;
	std::vector<DM::View> views;

	DM::View jview = sd.getCompleteView(S::JUNCTION,DM::MODIFY);

	views.push_back(jview);
	views.push_back(sd.getCompleteView(S::OUTFALL,DM::MODIFY));
	views.push_back(sd.getCompleteView(S::CONDUIT,DM::MODIFY));

	this->addData("Sewer", views);
}

void SetOutfallAtDeepestPoint::run()
{
	this->sys = this->getData("Sewer");
	S::ViewDefinitionHelper sd;
	DynamindBoostGraph::Compmap j = sys->getAllComponentsInView(sd.getView(DM::S::JUNCTION,DM::MODIFY));

	float deepesthight;
	Node *deepest = 0;

	for(DynamindBoostGraph::Compitr itr = j.begin(); itr != j.end(); ++itr)
	{
		Node* currentnode = static_cast<Node*>(*itr);

		if(deepest==0 || deepesthight > currentnode->getZ())
		{
			deepest = currentnode;
			deepesthight = deepest->getZ();
		}
	}

	if(deepest!=0)
	{
		Node *outfall = new Node(*deepest);
		Edge *outfallconduit  = new Edge(deepest,outfall);
		outfallconduit->addAttribute("Diameter",0.15);
		outfallconduit->addAttribute("Length",10);
		sys->addComponentToView(outfall,sd.getView(DM::S::OUTFALL,DM::MODIFY));
		sys->addComponentToView(outfallconduit,sd.getView(DM::S::CONDUIT,DM::MODIFY));
	}
}
