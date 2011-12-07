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

System::System(std::string name, std::string view) : Component()
{
}


void System::updateViews(Component * c) {
    foreach (std::string view, c->getInViews()) {
        this->views[view][c->getName()] = c;
    }
}

std::vector<std::string> System::getNamesOfViews() {
    std::vector<std::string> names;

    for ( std::map<std::string, std::map<std::string, Component* > >::const_iterator it = this->views.begin(); it != this->views.end(); ++it  ) {
        names.push_back(it->first);
    }

    return names;

}

System::System(const System& s) : Component(s)
{

    subsystems=s.subsystems;
    nodes=s.nodes;
    edges=s.edges;
    viewdefinitions = s.viewdefinitions;

    //Update SubSystem
    std::map<std::string,System*>::iterator its;

    for ( its=subsystems.begin() ; its != subsystems.end(); its++ ) {
        System * s = static_cast<System*>(ownedchilds[(*its).first]);
        subsystems[(*its).first]=s;
        this->updateViews(s);

    }

    //Update Nodes
    std::map<std::string,Node*>::iterator itn;

    for ( itn=nodes.begin() ; itn != nodes.end(); itn++ ) {
        Node * n = static_cast<Node*>(ownedchilds[(*itn).first]);
        nodes[(*itn).first] = n;
        this->updateViews(n);
    }

    //Update Edges
    std::map<std::string,Edge*>::iterator ite;

    for ( ite=edges.begin() ; ite != edges.end(); ite++ ) {
        Edge * e = static_cast<Edge*>(ownedchilds[(*ite).first]);
        edges[(*ite).first]= e;
        this->updateViews(e);
    }

}

System::~System()
{

}

Node * System::addNode(Node* node)
{
    if(!addChild(node))
        return 0;

    nodes[node->getName()]=node;
    return node;
}

System* System::createSubSystem(std::string name, std::string view)
{
    System* newsystem = new System(name, view);
    this->addSubSystem(newsystem);
    return newsystem;
}

Node * System::addNode(double x, double y, double z, std::string view) {

    Node * n = this->addNode(new Node(x, y, z));

    if (n == 0)
        return 0;
    if (!view.empty()) {
        this->views[view][n->getName()] = n;
        n->addView(view);
    }


    return n;
}

Edge * System::addEdge(Edge* edge)
{
    if(!getNode(edge->getStartpointName()) || !getNode(edge->getEndpointName()))
        return 0;

    if(!addChild(edge))
        return 0;

    edges[edge->getName()]=edge;
    return edge;
}
Edge * System::addEdge(Node * start, Node * end, std::string view)
{
    Edge * e = this->addEdge(new Edge(start->getName(), end->getName()));

    if (e == 0)
        return 0;
    if (!view.empty()) {
        this->views[view][e->getName()] = e;
        e->addView(view);
    }
    return e;
}

Node* System::getNode(std::string name)
{
    if(nodes.find(name)==nodes.end())
        return 0;

    return nodes[name];
}

Edge* System::getEdge(std::string name)
{
    if(edges.find(name)==edges.end())
        return 0;

    return edges[name];
}

bool System::removeEdge(std::string name)
{
    //check if name is a edge instance
    if(edges.find(name)==edges.end())
        return false;

    if(!removeChild(name))
        return false;

    edges.erase(name);
    return true;
}

bool System::removeNode(std::string name)
{
    //check if name is a node instance
    if(nodes.find(name)==nodes.end())
        return false;

    //remove node
    if(!removeChild(name))
        return false;

    nodes.erase(name);

    //find all connected edges and remove them
    std::vector<std::string> connectededges;

    std::map<std::string,Edge*>::iterator ite;


    for ( ite=edges.begin() ; ite != edges.end(); ite++ )
    {
        Edge* tmpedge = edges[(*ite).first];

        if(!tmpedge->getStartpointName().compare(name) || !tmpedge->getEndpointName().compare(name))
            connectededges.push_back(tmpedge->getName());
    }

    for(int index=0; index<connectededges.size(); index++)
    {
        if(!removeEdge(connectededges[index]))
            return false;
    }

    return true;
}

bool System::addSubSystem(System *newsystem)
{
    if(!addChild(newsystem))
        return false;

    subsystems[newsystem->getName()]=newsystem;
    updateViews(newsystem);
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
    if(subsystems.find(name)==subsystems.end())
        return 0;

    return subsystems[name];
}

std::map<std::string, System*> System::getAllSubSystems()
{
    return subsystems;
}

System* System::createSuccessor()
{
    System* result = new System(*this);
    predecessors.push_back(this);
    return result;
}

Component* System::clone()
{
    return new System(*this);
}

bool System::addView(View view)
{

    //For each view one dummy element will be created
    //Check for existing View
    DM::View existingView = this->viewdefinitions[view.getName()];
    foreach(std::string a, existingView.getWriteAttributes()) {
        existingView.addAvalibleAttribute(a);
    }

    if (view.getWriteType() == DM::NODE) {
        //Add Dummy Node
        //Copy all writen Attributes to the Avalible

    }

    foreach (std::string a , view.getWriteAttributes()) {
        existingView.getAttributes(a);
    }
    foreach (std::string a , view.getReadAttributes()) {
        existingView.addAttributes(a);
    }
    this->viewdefinitions[view.getName()] = existingView;

    return true;
}

std::vector<System*> System::getPredecessorStates()
{
    return predecessors;
}

std::map<std::string, Node*> System::getAllNodes()
{
    return nodes;
}

std::map<std::string, Edge*> System::getAllEdges()
{
    return edges;
}
