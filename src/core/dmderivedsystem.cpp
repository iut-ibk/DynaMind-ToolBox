/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMinde
 *
 * Copyright (C) 2013   Markus Sengthaler

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

#include "dmderivedsystem.h"
#include "dmnode.h"
#include "dmedge.h"
#include "dmface.h"
#include "dmrasterdata.h"

using namespace DM;

DerivedSystem::DerivedSystem(System* sys): System()
{
	predecessorSys = sys;
	allComponentsLoaded = false;
	allEdgesLoaded = false;
	allFacesLoaded = false;
	allNodesLoaded = false;
	allSubSystemsLoaded = false;
	//allRasterDataLoaded = false;

	viewdefinitions = sys->viewdefinitions;
	//predecessors = sys->predecessors;
	views = sys->views;

	//lastModule = sys->lastModule;

	CopyFrom(*sys, true);

	// copy from overwrites current system, fixes a bug
	currentSys = this;
}

Component* DerivedSystem::getChild(std::string name)
{
	return getComponent(name);
}

Component* DerivedSystem::SuccessorCopy(const Component *src)
{
	Component *c = new Component;
	c->CopyFrom(*src, true);
	return addComponent(c);
}
Node* DerivedSystem::SuccessorCopy(const Node *src)
{
	Node* n = new Node();
	*n = *src;
	n->CopyFrom(*src, true);
	return addNode(n);
}
Edge* DerivedSystem::SuccessorCopy(const Edge *src)
{
	Edge* e = new Edge(getNode(src->getStartpointName()), getNode(src->getEndpointName()));
	e->CopyFrom(*src, true);
	return addEdge(e);
}
Face* DerivedSystem::SuccessorCopy(const Face *src)
{
	std::vector<Node*> newNodes;
	foreach(Node* node, src->getNodePointers())
		newNodes.push_back(getNode(node->getUUID()));

	Face* newf = new Face(newNodes);
	newf->CopyFrom(*src,true);

	foreach(Face *hole, src->getHolePointers())
		newf->addHole(getFace(hole->getUUID()));

	return addFace(newf);
}

const Component* DerivedSystem::getComponentReadOnly(std::string uuid) const
{
	if(const Component* n = System::getComponentReadOnly(uuid))
		return n;

	return predecessorSys->getComponentReadOnly(uuid);
}
const Edge* DerivedSystem::getEdgeReadOnly(Node* start, Node* end)
{
	if(const Edge* e = System::getEdgeReadOnly(start,end))
		return e;

	return predecessorSys->getEdgeReadOnly(start,end);
}

Component* DerivedSystem::getComponent(std::string uuid)
{
	Component* n = System::getComponent(uuid);
	if(!n)
	{
		QMutexLocker ml(mutex);
		const Component *nconst = predecessorSys->getComponentReadOnly(uuid);
		if(nconst)
		{
			switch(nconst->getType())
			{
			case NODE:
				return SuccessorCopy((Node*)nconst);
			case EDGE:
				return SuccessorCopy((Edge*)nconst);
			case FACE:
				return SuccessorCopy((Face*)nconst);
			case RASTERDATA:
				return (Component*)nconst;
			case SUBSYSTEM:
			case COMPONENT:
				return SuccessorCopy(nconst);
				//case RASTERDATA:
			default:
				return NULL;
			}
		}
	}
	return n;
}

Node* DerivedSystem::getNode(std::string uuid)
{
	return (Node*)getComponent(uuid);
}
Edge* DerivedSystem::getEdge(std::string uuid)
{
	return (Edge*)getComponent(uuid);
}

// here is a little bottleneck, as we copy edges through all successor states
Edge* DerivedSystem::getEdge(Node* start, Node* end)
{
	if(Edge* e = System::getEdge(start,end))
		return e;
	{
		QMutexLocker ml(mutex);
		const Edge *e = predecessorSys->getEdgeReadOnly(start,end);
		if(e)
			return SuccessorCopy(e);
	}
	return NULL;
}

Face * DerivedSystem::getFace(std::string uuid)
{
	return (Face*)getComponent(uuid);
}
std::map<std::string, Component*> DerivedSystem::getAllComponents()
{
	if(!allComponentsLoaded)
	{
		QMutexLocker ml(mutex);

		// load all components
		std::map<std::string, Component*> comps = predecessorSys->getAllComponents();
		mforeach(Component* c, comps)
			getComponent(c->getUUID());
		allComponentsLoaded = true;
	}
	return System::getAllComponents();
}

std::map<std::string, Component*> DerivedSystem::getAllComponentsInView(const DM::View & view)
{
	const std::map<std::string, Component*> &predec_comps = System::getAllComponentsInView(view);

	if(view.getWriteAttributes().size() == 0 && view.getAccessType() == READ)
		return predec_comps;
	else
	{
		std::map<std::string, Component*> comps = views[view.getName()];
		for(std::map<std::string, Component*>::iterator it = comps.begin(); it != comps.end(); ++it)
			it->second = getComponent(it->first);

		return comps;
	}
}
std::map<std::string, Node*> DerivedSystem::getAllNodes()
{
	if(!allNodesLoaded)
	{
		QMutexLocker ml(mutex);

		// load all nodes
		std::map<std::string, Node*> nodes = predecessorSys->getAllNodes();
		mforeach(Node* n, nodes)
			getNode(n->getUUID());
		allNodesLoaded = true;
	}
	return System::getAllNodes();
}
std::map<std::string, Edge*> DerivedSystem::getAllEdges()
{
	if(!allEdgesLoaded)
	{
		QMutexLocker ml(mutex);

		// load all nodes
		std::map<std::string, Edge*> edges = predecessorSys->getAllEdges();
		mforeach(Edge* n, edges)
			getEdge(n->getUUID());
		allEdgesLoaded = true;
	}
	return System::getAllEdges();
}
std::map<std::string, Face*> DerivedSystem::getAllFaces()
{
	if(!allFacesLoaded)
	{
		QMutexLocker ml(mutex);

		// load all nodes
		std::map<std::string, Face*> faces = predecessorSys->getAllFaces();
		mforeach(Face* n, faces)
			getFace(n->getUUID());
		allFacesLoaded = true;
	}
	return System::getAllFaces();
}
std::map<std::string, System*> DerivedSystem::getAllSubSystems()
{
	if(!allSubSystemsLoaded)
	{
		QMutexLocker ml(mutex);

		// load all nodes
		std::map<std::string, System*> subsystems = predecessorSys->getAllSubSystems();
		mforeach(System* sys, subsystems)
			getSubSystem(sys->getUUID());
		allSubSystemsLoaded = true;
	}
	return System::getAllSubSystems();
}

std::map<std::string, RasterData*> DerivedSystem::getAllRasterData()
{
	std::map<std::string, RasterData*> pred_rd = this->predecessorSys->getAllRasterData();
	std::map<std::string, RasterData*> rd = System::getAllRasterData();

	for(std::map<std::string, RasterData*>::iterator it = rd.begin();
		it != rd.end(); ++it)
	{
		pred_rd[it->first] = it->second;
	}

	return pred_rd;
}
