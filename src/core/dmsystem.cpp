/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair, Markus Sengthaler

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

#include <dmattribute.h>
#include <dmcomponent.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmface.h>
#include <dmrasterdata.h>
#include <dmsystem.h>
#include <dmlogger.h>
#include <dmmodule.h>

#include <dmdbconnector.h>
#include <QSqlQuery>
#include <QUuid>

using namespace DM;


System::System() : Component()
{
    this->lastModule = 0;
	currentSys = this;
	
    ownedchilds = std::map<std::string,Component*>();
	DBConnector::getInstance();
	SQLInsert();
}

void System::setUUID(std::string uuid)
{
	QSqlQuery q;
	q.prepare("UPDATE systems SET uuid=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);
	
	Component::setUUID(uuid);
}

std::string System::getStateUuid()
{
	return stateUuid;
}

void System::setAccessedByModule(Module * m) {
    this->lastModule = m;
}

Module * System::getLastModule() {
    return this->lastModule;
}

void System::updateViews(Component * c) {
    if (!c) {
        DM::Logger(DM::Error)  << "Component 0 in updateView";
        return;
    }
    foreach (std::string view, c->getInViews()) {
        this->views[view][c->getUUID()] = c;
    }
}

std::vector<std::string> System::getNamesOfViews() {
    std::vector<std::string> names;

    for ( std::map<std::string, View*>::const_iterator it = this->viewdefinitions.begin(); it != this->viewdefinitions.end(); ++it  ) {
        names.push_back(it->first);
    }
    return names;
}

DM::View * System::getViewDefinition(string name) {

    if (viewdefinitions.find(name) == viewdefinitions.end()) {
        Logger(Debug) << "Couldn't find view definition for " << name;
        return 0;
    }
    return viewdefinitions[name];
}

System::System(const System& s) : Component(s)
{
	/*
    subsystems=s.subsystems;
    components = s.components;
    nodes=s.nodes;
    edges=s.edges;
    faces = s.faces;
    rasterdata = s.rasterdata;
    EdgeNodeMap = s.EdgeNodeMap;*/

	// rebuild pointers
	
    viewdefinitions = s.viewdefinitions;
    predecessors = s.predecessors;
    views = s.views;
    lastModule = s.lastModule;
	SQLInsert();
	
	currentSys = this;
	std::map<std::string,Component*>::iterator it;
	std::map<std::string,Component*> childmap = s.ownedchilds;
	
    for (std::map<std::string,Component*>::iterator it=childmap.begin() ; it != childmap.end(); it++ )
    {
        //childsview[(*it).first]=ownedchilds[(*it).first];
		this->addChild(it->second->clone());
    }
    //ownedchilds.clear();
}

System::~System()
{
	while(ownedchilds.size())// && bLoadedChilds)
    {
		//SQLDeleteChild((*ownedchilds.begin()).second);
        delete (*ownedchilds.begin()).second;
        ownedchilds.erase(ownedchilds.begin());
    }

	foreach (DM::System * sys, sucessors)
        if (sys)	delete sys;
    foreach (DM::View * v, ownedView)
        delete v;

    ownedView.clear();

	SQLDelete();
}

Components System::getType()
{
	return DM::SUBSYSTEM;
}

Component * System::addComponent(Component* c, const DM::View & view)
{
    if(!addChild(c)) {
        delete c;
        return 0;
    }
    components[c->getUUID()]=c;

    if (!view.getName().empty()) {
        this->views[view.getName()][c->getUUID()] = c;
        c->setView(view.getName());
    }
    this->updateViews(c);

    return c;
}

Node* System::addNode(Node* node)
{
    if(!addChild(node)) {
        delete node;
        return 0;
    }

    nodes[node->getUUID()]=node;
    this->updateViews(node);
    return node;
}
Node* System::addNode(double x, double y, double z,  const DM::View & view) {

    Node * n = this->addNode(new Node(x, y, z));

    if (n == 0)
        return 0;
    if (!view.getName().empty()) {
        this->views[view.getName()][n->getUUID()] = n;
        n->setView(view.getName());
    }
    return n;
}
Node* System::addNode(Node n,  const DM::View & view) {

    return this->addNode(n.getX(), n.getY(), n.getZ(), view);

}
Node* System::getNode(std::string uuid)
{
    if(nodes.find(uuid)==nodes.end())
        return 0;
    Node * n = nodes[uuid];
    if (n->getCurrentSystem() != this) {
        n = static_cast<Node*>(updateChild(nodes[uuid]));
        nodes[uuid] = n;
        this->updateViews(n);
        n->setCurrentSystem(this);
    }

    return nodes[uuid];
}

Edge* System::addEdge(Edge* edge)
{
    if(!getNode(edge->getStartpointName()) || !getNode(edge->getEndpointName())) {
        delete edge;
        return 0;
    }

    if(!addChild(edge)) {
        delete edge;
        return 0;
    }

    edges[edge->getUUID()]=edge;
    foreach (std::string v, edge->getInViews()) {
        views[v][edge->getUUID()]=edge;
    }
    this->EdgeNodeMap[std::pair<std::string, std::string>(edge->getStartpointName(), edge->getEndpointName())] = edge;
    this->updateViews(edge);
    return edge;
}
Edge* System::addEdge(Node * start, Node * end, const View &view)
{
    Edge * e = this->addEdge(new Edge(start->getUUID(), end->getUUID()));

    if (e == 0)
        return 0;
    if (!view.getName().empty()) {
        this->views[view.getName()][e->getUUID()] = e;
        e->setView(view.getName());
    }

    this->EdgeNodeMap[std::pair<std::string, std::string>(start->getUUID(), end->getUUID())] = e;

    return e;
}
Edge* System::getEdge(std::string uuid)
{
    if(edges.find(uuid)==edges.end())
        return 0;
    Edge * e = edges[uuid];
    if (e->getCurrentSystem() != this) {
        e = static_cast<Edge*>(updateChild(edges[uuid]));
        edges[uuid] = e;
        this->updateViews(e);
        e->setCurrentSystem(this);
    }

    return edges[uuid];
}
Edge* System::getEdge(const std::string & startnode, const std::string & endnode)
{
    std::pair<std::string, std::string> key(startnode, endnode);
    if(EdgeNodeMap.find(key)==EdgeNodeMap.end())
        return 0;
    return EdgeNodeMap[key];
}

Face* System::addFace(Face *f) {
    if(!addChild(f)) {
        delete f;
        return 0;
    }

    faces[f->getUUID()]=f;
    this->updateViews(f);
    return f;
}
Face* System::addFace(vector<DM::Node*> nodes,  const DM::View & view)
{
    std::vector<std::string> stringNodes;

    foreach (Node* n, nodes)
        stringNodes.push_back(n->getUUID());
    Face * f = this->addFace(new Face(stringNodes));

    if (f == 0)
        return 0;
    if (!view.getName().empty()) {
        this->views[view.getName()][f->getUUID()] = f;
        f->setView(view.getName());
    }
    return f;
}
Face* System::getFace(std::string uuid)
{
    if(faces.find(uuid)==faces.end())
        return 0;
    Face * f = faces[uuid];
    if (f->getCurrentSystem() != this) {
        f = static_cast<Face*>(updateChild(faces[uuid]));
        faces[uuid] = f;
        this->updateViews(f);
        f->setCurrentSystem(this);
    }
    return faces[uuid];

}

RasterData * System::addRasterData(RasterData *r, const DM::View & view)
{
    if(!addChild(r)) {
        delete r;
        return 0;
    }

    rasterdata[r->getUUID()] = r;

    if (!view.getName().empty()) {
        this->views[view.getName()][r->getUUID()] = r;
        r->setView(view.getName());
    }
    this->updateViews(r);
    return r;
}

Component * System::getComponent(std::string uuid)
{
	//SQLLoadComponents();
    if(nodes.find(uuid)!=nodes.end())
        return this->getNode(uuid);
    if(edges.find(uuid)!=edges.end())
        return this->getEdge(uuid);
    if(faces.find(uuid)!=faces.end())
        return this->getFace(uuid);
    if(subsystems.find(uuid)!=subsystems.end())
        return this->getSubSystem(uuid);
    if(rasterdata.find(uuid)!=rasterdata.end())
        return rasterdata[uuid];
    if(components.find(uuid)!=components.end())
        return components[uuid];
	if(ownedchilds.find(uuid)!=ownedchilds.end())
		return ownedchilds[uuid];

    return 0;
}

bool System::removeFace(std::string name)
{
    //check if name is a edge instance
    if(faces.find(name)==faces.end())
        return false;

    if(!removeChild(name))
        return false;

    faces.erase(name);
    return true;
}

std::map<std::string, Component*>  System::getAllComponents()
{
    for (ComponentMap::const_iterator it = components.begin(); it != components.end(); ++it) {
        std::string uuid = it->first;
        DM::Component * c = it->second;
        if (c->getCurrentSystem() != this) {
            c = static_cast<Component*>(updateChild(components[uuid]));
            components[uuid] = c;
            this->updateViews(c);
            c->setCurrentSystem(this);
        }
    }
    return this->components;
}

bool System::removeComponent(std::string name)
{
    //check if name is a edge instance
    if(components.find(name)==components.end())
        return false;

    if(!removeChild(name))
        return false;

    components.erase(name);
    return true;
}


bool System::removeEdge(std::string name)
{
    //check if name is a edge instance
    if(edges.find(name)==edges.end())
        return false;

    if(!removeChild(name))
        return false;
    DM::Edge * e  = this->getEdge(name);
    this->EdgeNodeMap.erase(std::pair<std::string, std::string>(e->getUUID(), e->getUUID()));
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
            connectededges.push_back(tmpedge->getUUID());
    }

    for(unsigned int index=0; index<connectededges.size(); index++)
    {
        if(!removeEdge(connectededges[index]))
            return false;
    }

    return true;
}
bool System::addComponentToView(Component *comp, const View &view) {
    this->views[view.getName()][comp->getUUID()] = comp;
    comp->setView(view.getName());
    return true;
}

bool System::removeComponentFromView(Component *comp, const View &view) {
    std::map<std::string, Component*> entries = this->views[view.getName()];
    entries.erase(comp->getUUID());
    comp->removeView(view);
    this->views[view.getName()] = entries;
    return true;
}

System * System::addSubSystem(System *newsystem,  const DM::View & view)
{
    //TODO add View to subsystem
    if(!addChild(newsystem)) {
        delete newsystem;
        return 0;
    }

    subsystems[newsystem->getUUID()]=newsystem;



    if (!view.getName().empty()) {
        this->views[view.getName()][newsystem->getUUID()] = newsystem;
        newsystem->setView(view.getName());
    }

    return newsystem;
}

bool System::removeSubSystem(std::string name)
{
    if(!removeChild(name))
        return false;

    subsystems.erase(name);

    return true;
}
std::map<std::string, Component*> System::getAllComponentsInView(const DM::View & view) {

    return views[view.getName()];
}
std::vector<std::string> System::getUUIDsOfComponentsInView(DM::View view) {
    std::vector<std::string> names;
    std::map<std::string, DM::Component*> components_view = this->getAllComponentsInView(view);
    for (std::map<std::string, DM::Component*>::const_iterator it = components_view.begin(); it != components_view.end(); ++it) {
        names.push_back(it->first);
    }
    return names;
}

std::vector<std::string> System::getUUIDs(const DM::View  & view)
{
    return this->getUUIDsOfComponentsInView(view);
}
System* System::getSubSystem(std::string uuid)
{
    if(subsystems.find(uuid)==subsystems.end())
        return 0;

    System * s= static_cast<System*>(updateChild(subsystems[uuid]));
    subsystems[uuid] = s;
    this->updateViews(s);
    return subsystems[uuid];


}

std::map<std::string, System*> System::getAllSubSystems()
{
	//SQLLoadComponents();
    return subsystems;
}

std::map<std::string, RasterData*> System::getAllRasterData()
{
	//SQLLoadComponents();
    return rasterdata;
}

System* System::createSuccessor()
{
    Logger(Debug) << "Create Sucessor " << this->getUUID();
    System* result = new System(*this);
	//result->statepath.push_back(this->numCopies++);
    this->sucessors.push_back(result);
	this->SQLUpdateStates();
	
    result->addPredecessors(this);
	result->SQLUpdateStates();
	//result->SQLInsertDeepCopy();

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
    if (this->viewdefinitions.find(view.getName()) == this->viewdefinitions.end()) {
        this->viewdefinitions[view.getName()] = new View(view);
        ownedView.push_back(this->viewdefinitions[view.getName()]);
    }

    DM::View  * existingView = this->viewdefinitions[view.getName()];
    if (!existingView) {
        this->viewdefinitions[view.getName()] = new View(view);
        existingView = this->viewdefinitions[view.getName()];
        ownedView.push_back(this->viewdefinitions[view.getName()]);
    }

    if (!view.writes()) {
        return true;
    }
    DM::Component * dummy  = 0;
    if (!existingView->getIdOfDummyComponent().empty()) {
        dummy = this->getComponent(existingView->getIdOfDummyComponent());
    } else {
        if ( DM::COMPONENT == view.getType()) {
            dummy = this->addComponent(new Component());
        }
        if ( DM::NODE == view.getType()) {
            dummy = this->addNode(0,0,0);
        }
        if (  DM::EDGE == view.getType()) {
            DM::Node * n1 =this->addNode(0,0,0);
			n1->setName("dummy");
            DM::Node * n2 =this->addNode(0,0,0);
			n2->setName("dummy");
            dummy = this->addEdge(n1,n2);
        }
        if (  DM::FACE == view.getType()) {
            DM::Node * n1 =this->addNode(0,0,0);
			n1->setName("dummy");
            std::vector<Node*> ve;
            ve.push_back(n1);
            dummy = this->addFace(ve);
        }
        if (  DM::SUBSYSTEM == view.getType()) {
            dummy = new DM::System();
            this->addSubSystem((DM::System*) dummy);
        }
        if (  DM::RASTERDATA == view.getType()) {
            dummy = new DM::RasterData();
            this->addRasterData((DM::RasterData*) dummy);
        }
		dummy->setName("dummy");
    }
    existingView->setIdOfDummyComponent(dummy->getUUID());


    //extend Dummy Attribute
    foreach (std::string a , view.getWriteAttributes()) {
        DM::Attribute attr(a);
        attr.setType(view.getAttributeType(a));
        if (view.getAttributeType(a) == Attribute::LINK) {
            attr.setLink(view.getLinkName(a), "");
        }
        dummy->addAttribute(attr);
    }

    return true;
}

std::vector<System*> System::getSucessors()
{
    return sucessors;
}

std::vector<System*> System::getPredecessors()
{
    return predecessors;
}

void System::addPredecessors(System *s)
{
    this->predecessors.push_back(s);
	this->SQLUpdateStates();
}

std::map<std::string, Node*> System::getAllNodes()
{
    //Update All Nodes
    for (NodeMap::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
        std::string uuid = it->first;
        DM::Node * n = it->second;
        if (n->getCurrentSystem() != this) {
            n = static_cast<Node*>(updateChild(nodes[uuid]));
            nodes[uuid] = n;
            this->updateViews(n);
            n->setCurrentSystem(this);
        }
    }
    return nodes;
}

std::map<std::string, Edge*> System::getAllEdges()
{

    //Update all Edges
    for (EdgeMap::const_iterator it = edges.begin(); it != edges.end(); ++it) {
        std::string uuid = it->first;
        DM::Edge * e = it->second;
        if (e->getCurrentSystem() != this) {
            e = static_cast<Edge*>(updateChild(edges[uuid]));
            edges[uuid] = e;
            this->updateViews(e);
            e->setCurrentSystem(this);
        }
    }
    return edges;
}

std::map<std::string, Face*> System::getAllFaces()
{

    for (FaceMap::const_iterator it = faces.begin(); it != faces.end(); ++it) {
        std::string uuid = it->first;
        DM::Face * f = it->second;
        if (f->getCurrentSystem() != this) {
            f = static_cast<Face*>(updateChild(faces[uuid]));
            faces[uuid] = f;
            this->updateViews(f);
            f->setCurrentSystem(this);
        }
    }
    return faces;
}

const std::vector<DM::View> System::getViews()  {

    std::vector<DM::View> viewlist;

    for (std::map<std::string, View*>::const_iterator it = viewdefinitions.begin(); it != viewdefinitions.end(); ++it) {
        viewlist.push_back(View(*it->second));
    }
    return viewlist;
}
bool System::addChild(Component *newcomponent)
{
	//SQLLoadChilds();
    if(!newcomponent)
        return false;

    //if(childsview.find(newcomponent->getUUID())!=childsview.end())
    //    return false;

    //childsview[newcomponent->getUUID()] = newcomponent;
    ownedchilds[newcomponent->getUUID()] = newcomponent;

	newcomponent->SetOwner(this);

    return true;
}

bool System::changeChild(Component *newcomponent)
{
	//SQLLoadChilds();
    if(!newcomponent)
        return false;

    if(ownedchilds.find(newcomponent->getUUID())!=ownedchilds.end())
	{
		//SQLDeleteChild(newcomponent);
        delete ownedchilds[newcomponent->getUUID()];
	}
	
    ownedchilds[newcomponent->getUUID()] = newcomponent;
    //childsview[newcomponent->getUUID()] = newcomponent;

	// currentSystem and statuuid are not set - if the change results from 
	// allocating a new successor state component;
	newcomponent->stateUuid = this->getStateUUID();
	//SQLInsertChild(newcomponent);

    return true;
}
Component * System::updateChild(Component * c) {
    if (ownedchilds.find(c->getUUID()) != ownedchilds.end())
        return c;
    Component * c_new = c->clone();
    changeChild(c_new);

    return c_new;
}

bool System::removeChild(std::string name)
{
	//SQLLoadChilds();
    //if(childsview.find(name)!=childsview.end())
    //{
        if(ownedchilds.find(name)!=ownedchilds.end())
        {
			//SQLDeleteChild(ownedchilds[name]);
            delete ownedchilds[name];
            ownedchilds.erase(name);
			return true;
        }

        //childsview.erase(name);
        //return true;
    //}
    return false;
}

Component* System::getChild(std::string name)
{
	//SQLLoadChilds();
    //if(childsview.find(name)==childsview.end())
    //    return 0;
   // return childsview[name];

	return ownedchilds[name];
}
std::map<std::string, Component*> System::getAllChilds()
{
    //return childsview;
	return ownedchilds;
}
void System::SQLInsert()
{
	SQLInsertAs("system");
}
void System::SQLDelete()
{
	SQLDeleteAs("system");
}

void System::SQLUpdateStates()
{
	QStringList sucList;
	foreach(System* sys, sucessors)
	{
		sucList.push_back(QString::fromStdString(sys->getStateUUID()));
	}
	QStringList preList;
	foreach(System* sys, predecessors)
	{
		preList.push_back(QString::fromStdString(sys->getStateUUID()));
	}

	QSqlQuery q;
	q.prepare("UPDATE systems SET sucessors=?,predecessors=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(sucList);
	q.addBindValue(preList);
	q.addBindValue(QString::fromStdString(getUUID()));
	q.addBindValue(QString::fromStdString(getStateUuid()));
	if(!q.exec())	PrintSqlError(&q);
}
