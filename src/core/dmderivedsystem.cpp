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

    //predecessors = sys->predecessors;
    viewCaches = sys->viewCaches;

    CopyFrom(*sys, true);

    // copy from overwrites current system, fixes a bug
    currentSys = this;

    this->SQLUpdateStates();
}

System* DerivedSystem::getPredecessor() const
{
    return predecessorSys;
}

Component* DerivedSystem::SuccessorCopyTypesafe(const Component *src)
{
    if(!src)
        return NULL;
    switch(src->getType())
    {
    case NODE:
        return SuccessorCopy((Node*)src);
    case EDGE:
        return SuccessorCopy((Edge*)src);
    case FACE:
        return SuccessorCopy((Face*)src);
    case RASTERDATA:
        return (Component*)src;
    case SUBSYSTEM:
    case COMPONENT:
        return SuccessorCopy(src);
    default:
        return NULL;
    }
}

Component* DerivedSystem::SuccessorCopy(const Component *src)
{
	Component* existing_c = NULL;
	if (map_contains(&predecessorComponentMap, src, existing_c))
		return existing_c;

    Component *c = new Component;
    c->CopyFrom(*src, true);

    predecessorComponentMap[src] = c;
	quuidMap[c->getQUUID()] = c;

    return addComponent(c);
}
Node* DerivedSystem::SuccessorCopy(const Node *src)
{
	Component* existing_c = NULL;
	if (map_contains(&predecessorComponentMap, (const Component*)src, existing_c))
		return (Node*)existing_c;

    Node* n = new Node();
    *n = *src;
	n->CopyFrom(*src, true);

	predecessorComponentMap[src] = n;
	quuidMap[n->getQUUID()] = n;

    return addNode(n);
}
Edge* DerivedSystem::SuccessorCopy(const Edge *src)
{
	Component* existing_c = NULL;
	if (map_contains(&predecessorComponentMap, (const Component*)src, existing_c))
		return (Edge*)existing_c;

    Node* start = src->getStartNode();
    Node* end = src->getEndNode();

    if(start && start->getCurrentSystem() != this)
        start = SuccessorCopy(start);
    if(end && end->getCurrentSystem() != this)
        end = SuccessorCopy(end);

    Edge* e = new Edge(start, end);
	e->CopyFrom(*src, true);

	predecessorComponentMap[src] = e;
	quuidMap[e->getQUUID()] = e;

    return addEdge(e);
}
Face* DerivedSystem::SuccessorCopy(const Face *src)
{
	Component* existing_c = NULL;
	if (map_contains(&predecessorComponentMap, (const Component*)src, existing_c))
		return (Face*)existing_c;

    std::vector<Node*> newNodes;
    foreach(Node* n, src->getNodePointers())
    {
        if(n->getCurrentSystem() != this)
            n = SuccessorCopy(n);
        newNodes.push_back(n);
    }

    Face* newf = new Face(newNodes);
    newf->CopyFrom(*src,true);

    std::vector<Face*> newHoles;
    foreach(Face* h, src->getHolePointers())
    {
        if(h->getCurrentSystem() != this)
            h = SuccessorCopy(h);
        newf->addHole(h);
    }

	predecessorComponentMap[src] = newf;
	quuidMap[newf->getQUUID()] = newf;

    return addFace(newf);
}

const Edge* DerivedSystem::getEdgeReadOnly(Node* start, Node* end)
{
    if(const Edge* e = System::getEdgeReadOnly(start,end))
        return e;

    return predecessorSys->getEdgeReadOnly(start,end);
}

std::vector<Component*> DerivedSystem::getAllComponents()
{
    if(!allComponentsLoaded)
    {
        QMutexLocker ml(mutex);

        foreach(Component* c, predecessorSys->getAllComponents())
            if(c->getCurrentSystem() != this)
                SuccessorCopy(c);

        allComponentsLoaded = true;
    }
    return System::getAllComponents();
}

std::vector<Node*> DerivedSystem::getAllNodes()
{
    if(!allNodesLoaded)
    {
        QMutexLocker ml(mutex);

        foreach(Node* c, predecessorSys->getAllNodes())
            if(c->getCurrentSystem() != this)
                SuccessorCopy(c);

        allNodesLoaded = true;
    }
    return System::getAllNodes();
}
std::vector<Edge*> DerivedSystem::getAllEdges()
{
    if(!allEdgesLoaded)
    {
        QMutexLocker ml(mutex);

        foreach(Edge* c, predecessorSys->getAllEdges())
            if(c->getCurrentSystem() != this)
                SuccessorCopy(c);

        allEdgesLoaded = true;
    }
    return System::getAllEdges();
}
std::vector<Face*> DerivedSystem::getAllFaces()
{
    if(!allFacesLoaded)
    {
        QMutexLocker ml(mutex);

        foreach(Face* c, predecessorSys->getAllFaces())
            if(c->getCurrentSystem() != this)
                SuccessorCopy(c);

        allFacesLoaded = true;
    }
    return System::getAllFaces();
}
std::vector<System*> DerivedSystem::getAllSubSystems()
{
    if(!allSubSystemsLoaded)
    {
        QMutexLocker ml(mutex);

        foreach(System* c, predecessorSys->getAllSubSystems())
            if(c->getCurrentSystem() != this)
                SuccessorCopy(c);

        allSubSystemsLoaded = true;
    }
    return System::getAllSubSystems();
}

std::vector<RasterData*> DerivedSystem::getAllRasterData()
{
    return this->predecessorSys->getAllRasterData();
}

Component* DerivedSystem::_getChild(QUuid quuid)
{
	if (Component* c = System::_getChild(quuid))
		return c;
	else
		return SuccessorCopyTypesafe(predecessorSys->_getChild(quuid));
}

Component* DerivedSystem::_getChildReadOnly(QUuid quuid)
{
	if (Component* c = System::_getChild(quuid))
		return c;
	else
		return predecessorSys->_getChild(quuid);
}

Component* DerivedSystem::getSuccessingComponent(const Component* formerComponent)
{
	Component* successingComponent = NULL;
	if (map_contains(&predecessorComponentMap, formerComponent, successingComponent))
		return successingComponent;
	else
	{
		// copy function automatically adds predecessorComponentMap entry fur future access
		return SuccessorCopyTypesafe(formerComponent);
	}
}

void DerivedSystem::_moveToDb()
{
    predecessorSys->_moveToDb();
    System::_moveToDb();
}
