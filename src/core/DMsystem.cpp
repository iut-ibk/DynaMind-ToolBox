/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#include <DMcomponent.h>
#include <DMnode.h>
#include <DMedge.h>
#include <DMsystem.h>

#include <vibe_logger.h>

using namespace DM;

System::System(std::string name, std::string id) : Component(name,id)
{
    vibens::Logger(vibens::Debug) << "Create System " << name << " " << id;
}

System::System(const System& s) : Component(s)
{

    subsystems=s.subsystems;
    nodes=s.nodes;
    edges=s.edges;

    std::map<std::string,System*>::iterator its;

    for ( its=subsystems.begin() ; its != subsystems.end(); its++ )
        subsystems[(*its).first]=static_cast<System*>(ownedchilds[(*its).first]);

    std::map<std::string,Node*>::iterator itn;

    for ( itn=nodes.begin() ; itn != nodes.end(); itn++ )
        nodes[(*itn).first]=static_cast<Node*>(ownedchilds[(*itn).first]);

    std::map<std::string,Edge*>::iterator ite;

    for ( ite=edges.begin() ; ite != edges.end(); ite++ )
        edges[(*ite).first]=static_cast<Edge*>(ownedchilds[(*ite).first]);
}

System::~System()
{

}

bool System::addNode(Node* node)
{
    if(!addChild(node))
        return false;

    nodes[node->getName()]=node;
    return true;
}

bool System::addEdge(Edge* edge)
{
    if(!getNode(edge->getStartpointName()) || !getNode(edge->getEndpointName()))
        return false;

    if(!addChild(edge))
        return false;

    edges[edge->getName()]=edge;
    return true;
}

Node* System::getNode(std::string name)
{
    return nodes[name];
}

Edge* System::getEdge(std::string name)
{
    return edges[name];
}

bool System::removeEdge(std::string name)
{
    if(!removeChild(name))
        return false;

    edges.erase(name);
    return true;
}

bool System::removeNode(std::string name)
{
    //TODO implement
    return false;
}

bool System::addSubSystem(System *newsystem)
{
    if(!addChild(newsystem))
        return false;

    subsystems[newsystem->getName()]=newsystem;
    return true;
}

bool System::removeSubSystem(std::string name)
{
    if(!removeChild(name))
        return false;

    subsystems.erase(name);

    return true;
}

System* System::getSubSystem(std::string name)
{
    return subsystems[name];
}

std::map<std::string, System*> System::getAllSubSystems()
{
    return subsystems;
}

System* System::createSuccessor()
{
    System* result = new System(*this);

    return result;
}

Component* System::clone()
{
    return new System(*this);
}

