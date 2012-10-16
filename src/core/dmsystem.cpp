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
	stateUuid = QUuid::createUuid().toString().toStdString();

	DBConnector::getInstance();
	SQLInsertThisSystem();
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
    subsystems=s.subsystems;
    components = s.components;
    nodes=s.nodes;
    edges=s.edges;
    faces = s.faces;
    rasterdata = s.rasterdata;
    EdgeNodeMap = s.EdgeNodeMap;
    viewdefinitions = s.viewdefinitions;
    predecessors = s.predecessors;
    views = s.views;
    lastModule = s.lastModule;

	stateUuid = QUuid::createUuid().toString().toStdString();

	SQLInsertThisSystem();
}

System::~System()
{
    foreach (DM::System * sys, sucessors)
        if (sys)
            delete sys;
    foreach (DM::View * v, ownedView) {
        delete v;
    }
    ownedView.clear();

	SQLDeleteThisSystem();
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
	SQLLoadComponents();
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
    if(components.find(uuid)!=components.end()) {
        Component * c = components[uuid];
        if (c->getCurrentSystem() != this) {
            c = static_cast<Component*>(updateChild(components[uuid]));
            components[uuid] = c;
            this->updateViews(c);
            c->setCurrentSystem(this);
        }
        return c;
    }
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
	SQLLoadComponents();
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
	SQLLoadComponents();
    return subsystems;
}

std::map<std::string, RasterData*> System::getAllRasterData()
{
	SQLLoadComponents();
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
	result->SQLInsertDeepCopy();

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
            DM::Node * n2 =this->addNode(0,0,0);
            dummy = this->addEdge(n1,n2);
        }
        if (  DM::FACE == view.getType()) {
            DM::Node * n1 =this->addNode(0,0,0);
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
	
	QSqlQuery q;
	q.prepare("UPDATE systems SET predecessors=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(DBConnector::GetStringList(predecessors));
	q.addBindValue(QString::fromStdString(getUUID()));
	q.addBindValue(QString::fromStdString(getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);
}

std::map<std::string, Node*> System::getAllNodes()
{
	SQLLoadComponents();
    return nodes;
}

std::map<std::string, Edge*> System::getAllEdges()
{
	SQLLoadComponents();
    return edges;
}

std::map<std::string, Face*> System::getAllFaces()
{
	SQLLoadComponents();
    return faces;
}

const std::vector<DM::View> System::getViews()  {

    std::vector<DM::View> viewlist;

    for (std::map<std::string, View*>::const_iterator it = viewdefinitions.begin(); it != viewdefinitions.end(); ++it) {
        viewlist.push_back(View(*it->second));
    }
    return viewlist;
}

/*
void System::LoadComponents()
{
	if(bComponents==true)	return;
		bComponents = true;

	QSqlQuery q("SELECT uuid,type,vale FROM components WHERE owner LIKE ?");
	q.addBindValue(QString::fromStdString(getUUID()));

	while(q.next())
	{
		int type = q.value(1).toInt();
		std::string uuid = q.value(0).toString().toStdString();
		QByteArray value = q.value(2).toByteArray();
		switch(type)
		{
		case COMPONENT: components[uuid]  = new Component(uuid);
			break;
		case NODE:  nodes[uuid] = new Node(uuid, value);
			break;
		case EDGE: edges[uuid] = new Edge(uuid, value);
			break;
		case FACE: faces[uuid]  = new Face(uuid, value);
			break;
		case RASTERDATA: rasterdata[uuid] = new RasterData(uuid, value);
			break;
		case SUBSYSTEM: subsystems[uuid] = new System(uuid);
			break;
		}
	}
}

void System::FreeComponents()
{
	this->bComponents = false;
	FreeMap(components);
	FreeMap(edges);
	FreeMap(nodes);
	FreeMap(faces);
	FreeMap(rasterdata);
	FreeMap(subsystems);
}*/

void System::SQLInsertThisSystem()
{
	// prepare is mandatory, otherwise exec() will return an error
	QSqlQuery q;
	q.prepare("INSERT INTO systems (uuid,stateuuid,predecessors) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUuid()));
	q.addBindValue(DBConnector::GetStringList(predecessors));
	
	if(!q.exec())	PrintSqlError(&q);

	QSqlQuery q2;
	q2.prepare("INSERT INTO components (uuid,stateuuid,owner,type,name) VALUES (?,?,?,?,?)");
	q2.addBindValue(QString::fromStdString(this->getUUID()));
	q2.addBindValue(QString::fromStdString(this->getStateUuid()));
	q2.addBindValue(QString::fromStdString(this->getUUID()));
	q2.addBindValue(SUBSYSTEM);
	q2.addBindValue(QString::fromStdString(this->getName()));

	if(!q2.exec())	PrintSqlError(&q2);

	//DBConnector::getInstance()->endTransaction();
}
void System::SQLDeleteThisSystem()
{
	QSqlQuery q;
	q.prepare("DELETE FROM systems WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUuid()));
	if(!q.exec())	PrintSqlError(&q);

	QSqlQuery q2;
	q2.prepare("DELETE FROM components WHERE uuid LIKE ? AND owner like ? AND stateuuid like ?");
	q2.addBindValue(QString::fromStdString(this->getUUID()));
	q2.addBindValue(QString::fromStdString(this->getUUID()));
	q2.addBindValue(QString::fromStdString(this->getStateUuid()));
	if(!q2.exec())	PrintSqlError(&q2);
}
void System::SQLUpdateStates()
{
	QSqlQuery q;
	q.prepare("UPDATE systems SET sucessors=?,predecessors=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(DBConnector::GetStringList(sucessors));
	q.addBindValue(DBConnector::GetStringList(predecessors));
	q.addBindValue(QString::fromStdString(getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUuid()));
	if(!q.exec())	PrintSqlError(&q);
}

void System::SQLInsertDeepCopy()
{
	Component::SQLInsertDeepCopy();
}


void System::SQLLoadComponents()
{
	SQLLoadChilds();
	foreach(ComponentPair item, ownedchilds)
	{
		Component *c = item.second;
		switch(c->getType())
		{
		case COMPONENT: components[uuid]  = c;
			break;
		case NODE:  nodes[uuid] = (Node*)c;
			break;
		case EDGE: edges[uuid] = (Edge*)c;
			break;
		case FACE: faces[uuid]  = (Face*)c;
			break;
		case RASTERDATA: rasterdata[uuid] = (RasterData*)c;
			break;
		case SUBSYSTEM: subsystems[uuid] = (System*)c;
			break;
		}
	}
}
void System::SQLFreeComponents()
{
	SQLFreeChilds();
}

/*
void System::SQLLoadSystems()
{
	if(bSystems==true)	return;
		bSystems = true;

	QSqlQuery q1("SELECT sucessors FROM systems WHERE uuid LIKE ?");
	q1.addBindValue(QString::fromStdString(getUUID()));

	if(q1.next())
	{
		QStringList list = q1.value(0).toStringList();
		foreach(QString item, list)
		{
			sucessors.push_back(new System(item.toStdString()));
		}
	}
	QSqlQuery q2("SELECT predecessors FROM systems WHERE uuid LIKE ?");
	q2.addBindValue(QString::fromStdString(getUUID()));

	if(q2.next())
	{
		QStringList list = q2.value(0).toStringList();
		foreach(QString item, list)
		{
			predecessors.push_back(new System(item.toStdString()));
		}
	}
}
void System::SQLFreeSystems()
{
	FreeVector(sucessors);
	FreeVector(predecessors);
	bSystems = true;
}
*/
/*
System::System(std::string uuid):Component(uuid)
{
	this->bComponents = false;
	this->bSystems = false;
	System();
	QSqlQuery q("SELECT uuid FROM systems WHERE uuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
}
*/
