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

#include <extractnodesfromfaces.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <examinationroomviewdef.h>

DM_DECLARE_NODE_NAME(ExtractNodesFromFaces,Graph)

ExtractNodesFromFaces::ExtractNodesFromFaces()
{
	std::vector<DM::View> views;
	DM::View view;
	DM::GRAPH::ViewDefinitionHelper defhelper;
	DM::ER::ViewDefinitionHelper defhelper_er;

	//Define Parameter street network
	views.push_back(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

	//Define Parameter street network
	view = defhelper.getView(DM::GRAPH::NODES,DM::WRITE);
	views.push_back(view);
	viewdef[DM::GRAPH::NODES]=view;

	this->addData("Layout", views);
}

void ExtractNodesFromFaces::run()
{
	DM::ER::ViewDefinitionHelper defhelper_er;
	this->sys = this->getData("Layout");

	std::vector<DM::Component*> faces = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));
	std::vector<DM::Component*> nodesadded;

	typedef std::vector<DM::Component*>::iterator itr;

	DM::Logger(DM::Standard) << "Number of Faces found:" << faces.size();

	for(itr i = faces.begin(); i != faces.end(); i++)
	{
		DM::Face *face = static_cast<DM::Face*>((*i));
		extractNodes(face,this->sys,nodesadded);
	}

	DM::Logger(DM::Standard) << "Number of extracted nodes: " << nodesadded.size();
}

bool ExtractNodesFromFaces::extractNodes(DM::Face *face, DM::System *sys, std::vector<DM::Component *> &nodesadded)
{
	std::vector<DM::Node*> facenodes = face->getNodePointers();
	std::vector<DM::Face*> holes = face->getHolePointers();

	for(int index=0; index < facenodes.size(); index++)
	{
		DM::Node* node = facenodes[index];

		if(std::find(nodesadded.begin(),nodesadded.end(),node)==nodesadded.end())
		{
			sys->addComponentToView(node,viewdef[DM::GRAPH::NODES]);
			nodesadded.push_back(node);
		}
	}

	for(int index=0; index<holes.size(); index++)
		extractNodes(holes[index],sys,nodesadded);

	return true;
}


