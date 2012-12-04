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

/*template <class T>
void DeleteFromVector(std::vector<T*> vector, T* item)
{
    for(unsigned int i=0;i<vector.size();i++)
    {
        if(vector[i]==item)
        {
            vector.erase(vector.begin()+i);
            return true;
        }
    }
    return false;
}*/

System::System() : Component(true)
{
    this->lastModule = 0;
    //this->mutex = new QMutex(QMutex::Recursive);

	currentSys = this;
	
    ownedchilds = std::map<QUuid, Component*>();
	DBConnector::getInstance();
	SQLInsert();
}
System::System(const System& s) : Component(s, true)
{
    predecessors = s.predecessors;
    lastModule = s.lastModule;
    //mutex = new QMutex(QMutex::Recursive);
	SQLInsert();
	
    currentSys = this;

    std::map<Component*,Component*> childReplaceMap;

    std::map<QUuid, Component*> childmap = s.ownedchilds;
    for (std::map<QUuid,Component*>::iterator it=childmap.begin() ; it != childmap.end(); ++it )
    {
        Component *oldComp = it->second;
        switch(oldComp->getType())
        {
        case DM::COMPONENT:
            childReplaceMap[oldComp] = addComponent(oldComp->clone());
            break;
        case DM::NODE:
            childReplaceMap[oldComp] = addNode((Node*)oldComp->clone());
            break;
        //case DM::EDGE:      this->addEdge((Edge*)c); break;
        case DM::FACE:
            childReplaceMap[oldComp] = addFace((Face*)oldComp->clone());
            break;
        case DM::SUBSYSTEM:
            childReplaceMap[oldComp] = addSubSystem((System*)oldComp->clone());
            break;
        case DM::RASTERDATA:
            childReplaceMap[oldComp] = addRasterData((RasterData*)oldComp->clone());
            break;
        default:    break;
        }
    }
    std::map<QUuid,Edge*> edgemap = s.edges;
    for (std::map<QUuid,Edge*>::iterator it=edgemap.begin() ; it != edgemap.end(); ++it )
    {
        Edge *oldEdge = it->second;
        Edge *e = (Edge*)oldEdge->clone();
        QUuid points[2];
        e->getPoints(points);

        e->setStartpoint((Node*)childReplaceMap[s.findChild(points[0])]);
        e->setEndpoint((Node*)childReplaceMap[s.findChild(points[1])]);

        childReplaceMap[oldEdge] = addEdge(e);
    }

    // update view definitions
    std::map<std::string, View*> viewdefinitionMap = s.viewdefinitions;
    for (std::map<std::string,View*>::iterator it=viewdefinitionMap.begin() ; it != viewdefinitionMap.end(); ++it )
    {
        View* v = new View(*it->second);
        viewdefinitions[v->getName()] = v;
        ownedView.push_back(v);

        if(v->getDummyComponent()!=NULL)
            v->setDummyComponent(childReplaceMap[v->getDummyComponent()]);
    }
    // update views
    //foreach(Component* c, ownedchilds)
    for (std::map<QUuid,Component*>::iterator it=ownedchilds.begin() ; it != ownedchilds.end(); ++it )
        this->updateViews(it->second);
}
System::~System()
{
    while(ownedchilds.size())
    {
        delete (*ownedchilds.begin()).second;
        ownedchilds.erase(ownedchilds.begin());
    }

    foreach (DM::System * sys, this->sucessors)
        if (sys)
            delete sys;
    foreach (DM::View * v, ownedView)
        delete v;

    ownedView.clear();
    //delete mutex;

    Component::SQLDelete();
}

/*
void System::setUUID(std::string uuid)
{
    DBConnector::getInstance()->Update("systems", QString::fromStdString(this->uuid),
                                                QString::fromStdString(stateUuid),
                                       "uuid", uuid.toRfc4122());
    this->uuid = uuid;
}*/

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

Components System::getType()
{
	return DM::SUBSYSTEM;
}
QString System::getTableName()
{
    return "systems";
}
Component * System::addComponent(Component* c, const DM::View & view)
{
    //QMutexLocker locker(mutex);

    if(!addChild(c)) {
        delete c;
        return 0;
    }
    components[c->getQUUID()] = c;

    if (!view.getName().empty()) {
        this->views[view.getName()][c->getUUID()] = c;
        c->setView(view.getName());
    }
    this->updateViews(c);

    return c;
}
Component * System::getComponent(std::string uuid)
{
    /*
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
    if(ownedchilds.find(QUuid(QString::fromStdString(uuid)))!=ownedchilds.end())
        return ownedchilds[QUuid(QString::fromStdString(uuid))];
*/
    return this->getChild(uuid);
}
bool System::removeComponent(std::string name)
{
    //check if name is a edge instance
    /*
    if(components.find(name)==components.end())
        return false;

    if(!removeChild(name))
        return false;

    components.erase(name);
    return true;*/

    return removeChild(name);
}

Node* System::addNode(Node* node)
{
    //QMutexLocker locker(mutex);
    if(!addChild(node)) {
        delete node;
        return 0;
    }
    nodes[node->getQUUID()] = node;
    this->updateViews(node);
    return node;
}
Node* System::addNode(const Node &n,  const DM::View & view)
{
    double v[3];
    n.get(v);
    return this->addNode(v[0],v[1],v[2], view);
}

Node * System::addNode(double x, double y, double z,  const DM::View & view)
{
    //QMutexLocker locker(mutex);
    Node * n = this->addNode(new Node(x, y, z));

    if (n == NULL)
        return NULL;
    if (!view.getName().empty()) {
        this->views[view.getName()][n->getUUID()] = n;
        n->setView(view.getName());
    }
    return n;
}

Node* System::getNode(std::string uuid)
{
    /*if(nodes.find(uuid)==nodes.end())
        return 0;
    return nodes[uuid];*/
    Component* c = getChild(uuid);
    if(c && c->getType() == NODE)
        return (Node*)c;

    return NULL;
}

Node* System::getNode(QUuid uuid)
{
    /*if(nodes.find(uuid)==nodes.end())
        return 0;
    return nodes[uuid];*/
    Component* c = getChild(uuid);
    if(c && c->getType() == NODE)
        return (Node*)c;

    return NULL;
}
bool System::removeNode(std::string name)
{
    QUuid quuid(QString::fromStdString(name));
    //check if name is a node instance
    if(nodes.find(quuid)==nodes.end())
        return false;

    //remove node
    if(!removeChild(quuid))
        return false;


    //find all connected edges and remove them
    std::vector<std::string> connectededges;
    std::map<QUuid,Edge*>::iterator ite;
    for ( ite=edges.begin() ; ite != edges.end(); ite++ )
    {
        Edge* tmpedge = edges[(*ite).first];
        if(tmpedge->getStartpoint() == quuid
        || tmpedge->getEndpoint() == quuid)
            connectededges.push_back(tmpedge->getUUID());
    }
    nodes.erase(quuid);

    for(unsigned int index=0; index<connectededges.size(); index++)
    {
        if(!removeEdge(connectededges[index]))
            return false;
    }

    return true;
}

Edge* System::addEdge(Edge* edge)
{
    //QMutexLocker locker(mutex);
    if(!getNode(edge->getStartpoint()) || !getNode(edge->getEndpoint())){
        delete edge;
        return 0;
    }

    if(!addChild(edge)) {
        delete edge;
        return 0;
    }

    edges[edge->getQUUID()]=edge;
    foreach (std::string v, edge->getInViews()) {
        views[v][edge->getUUID()]=edge;
    }
    this->EdgeNodeMap[std::pair<std::string, std::string>(edge->getStartpointName(), edge->getEndpointName())] = edge;
    this->updateViews(edge);
    return edge;
}
Edge* System::addEdge(Node * start, Node * end, const View &view)
{
    //QMutexLocker locker(mutex);
    Edge * e = this->addEdge(new Edge(start, end));

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
    QUuid quuid(QString::fromStdString(uuid));
    return getEdge(quuid);
}
Edge* System::getEdge(QUuid uuid)
{
    if(edges.find(uuid)==edges.end())
        return 0;
    return edges[uuid];
}
Edge* System::getEdge(const std::string & startnode, const std::string & endnode)
{
    std::pair<std::string, std::string> key(startnode, endnode);
    if(EdgeNodeMap.find(key)==EdgeNodeMap.end())
        return 0;
    return EdgeNodeMap[key];
}
bool System::removeEdge(std::string name)
{
    QUuid quuid(QString::fromStdString(name));
    //check if name is a edge instance
    if(edges.find(quuid)==edges.end())
        return false;

    if(!removeChild(quuid))
        return false;
    DM::Edge * e  = this->getEdge(quuid);
    this->EdgeNodeMap.erase(std::pair<std::string, std::string>(e->getUUID(), e->getUUID()));
    edges.erase(quuid);
    return true;
}

Face* System::addFace(Face *f) {
    if(!addChild(f)) {
        delete f;
        return 0;
    }

    faces[f->getQUUID()]=f;
    this->updateViews(f);
    return f;
}
Face* System::addFace(std::vector<DM::Node*> nodes,  const DM::View & view)
{
    /*std::vector<std::string> stringNodes;
    foreach (Node* n, nodes)
        stringNodes.push_back(n->getUUID());
    Face * f = this->addFace(new Face(stringNodes));*/
    Face *f = this->addFace(new Face(nodes));

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
    QUuid quuid(QString::fromStdString(uuid));
    if(faces.find(quuid)==faces.end())
        return 0;
    return faces[quuid];

}
bool System::removeFace(std::string name)
{
    QUuid quuid(QString::fromStdString(name));
    //check if name is a edge instance
    if(faces.find(quuid)==faces.end())
        return false;

    if(!removeChild(quuid))
        return false;

    faces.erase(quuid);
    return true;
}

RasterData * System::addRasterData(RasterData *r, const DM::View & view)
{
    if(!addChild(r)) {
        delete r;
        return 0;
    }

    rasterdata[r->getQUUID()] = r;

    if (!view.getName().empty()) {
        this->views[view.getName()][r->getUUID()] = r;
        r->setView(view.getName());
    }
    this->updateViews(r);
    return r;
}

std::map<std::string, Component*>  System::getAllComponents()
{
    std::map<std::string, Component*> comps;
    mforeach(Component* c, components)
            comps[c->getUUID()] = c;

    return comps;
}
std::map<std::string, Node*> System::getAllNodes()
{
    std::map<std::string, Node*> n;
    for (std::map<QUuid,Node*>::iterator it=nodes.begin() ; it != nodes.end(); ++it )
        n[it->second->getUUID()] = it->second;
    return n;
}
std::map<std::string, Edge*> System::getAllEdges()
{
    std::map<std::string, Edge*> e;
    for (std::map<QUuid,Edge*>::iterator it=edges.begin() ; it != edges.end(); ++it )
        e[it->second->getUUID()] = it->second;
    return e;
}
std::map<std::string, Face*> System::getAllFaces()
{
    std::map<std::string, Face*> f;
    for (std::map<QUuid,Face*>::iterator it=faces.begin() ; it != faces.end(); ++it )
        f[it->second->getUUID()] = it->second;
    return f;
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

    subsystems[newsystem->getQUUID()]=newsystem;

    if (!view.getName().empty()) {
        this->views[view.getName()][newsystem->getUUID()] = newsystem;
        newsystem->setView(view.getName());
    }

    return newsystem;
}
System* System::getSubSystem(std::string uuid)
{
    return getSubSystem(QUuid(QString::fromStdString(uuid)));
}
System* System::getSubSystem(QUuid uuid)
{
    if(subsystems.find(uuid)==subsystems.end())
        return 0;

    return subsystems[uuid];
}
bool System::removeSubSystem(std::string name)
{
    if(!removeChild(name))
        return false;

    subsystems.erase(QUuid(QString::fromStdString(name)));

    return true;
}

std::map<std::string, Component*> System::getAllComponentsInView(const DM::View & view)
{
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

std::map<std::string, System*> System::getAllSubSystems()
{
    std::map<std::string, System*> syss;
    mforeach(System* s, subsystems)
            syss[s->getUUID()] = s;

    return syss;
}

std::map<std::string, RasterData*> System::getAllRasterData()
{
    std::map<std::string, RasterData*> rasters;
    mforeach(RasterData* r, rasterdata)
            rasters[r->getUUID()] = r;

    return rasters;
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
    DM::Component * dummy  = existingView->getDummyComponent();
    if (existingView->getDummyComponent() == NULL)
    {
        switch(view.getType())
        {
        case DM::COMPONENT:
            dummy = this->addComponent(new Component());
            break;
        case DM::NODE:
            dummy = this->addNode(0,0,0);
            break;
        case DM::EDGE:{
            DM::Node * n1 = this->addNode(0,0,0);
            DM::Node * n2 = this->addNode(0,0,0);
            dummy = this->addEdge(n1,n2);}
            break;
        case DM::FACE:{
            DM::Node * n1 =this->addNode(0,0,0);
            std::vector<Node*> ve;
            ve.push_back(n1);
            dummy = this->addFace(ve);}
            break;
        case DM::SUBSYSTEM:
            dummy = new DM::System();
            this->addSubSystem((DM::System*) dummy);
            break;
        case DM::RASTERDATA:
            dummy = new DM::RasterData();
            this->addRasterData((DM::RasterData*) dummy);
            break;
        default:
            break;
        }
    }
    if(dummy==NULL)
        Logger(Error) << "Error: dummy object could not be initialized";

    existingView->setDummyComponent(dummy);


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
const std::vector<DM::View> System::getViews()  {

    std::vector<DM::View> viewlist;

    for (std::map<std::string, View*>::const_iterator it = viewdefinitions.begin(); it != viewdefinitions.end(); ++it) {
        viewlist.push_back(View(*it->second));
    }
    return viewlist;
}

System* System::createSuccessor()
{
    Logger(Debug) << "Create Sucessor " << this->getUUID();
    System* result = new System(*this);
    this->sucessors.push_back(result);
	this->SQLUpdateStates();
    result->addPredecessors(this);
    result->SQLUpdateStates();

    return result;
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


bool System::addChild(Component *newcomponent)
{
    if(!newcomponent)
        return false;
    ownedchilds[newcomponent->getQUUID()] = newcomponent;

	newcomponent->SetOwner(this);

    return true;
}
bool System::removeChild(std::string name)
{
    return removeChild(QUuid(QString::fromStdString(name)));
}

bool System::removeChild(Component* c)
{
    if(ownedchilds.find(c->getQUUID())==ownedchilds.end())
        return false;

    ownedchilds.erase(c->getQUUID());
    /*switch(c->getType())
    {
    case COMPONENT:

    }*/

    delete c;
    return true;
}

bool System::removeChild(QUuid uuid)
{
    if(ownedchilds.find(uuid)==ownedchilds.end())
        return false;
    Component *c = ownedchilds[uuid];
    return removeChild(c);
}


Component* System::getChild(std::string name)
{
    return getChild(QUuid(QString::fromStdString(name)));
}

Component* System::getChild(QUuid uuid)
{
    return ownedchilds[uuid];
}
Component* System::findChild(QUuid uuid) const
{
    if(ownedchilds.find(uuid)==ownedchilds.end())
        return NULL;
    return ownedchilds.find(uuid)->second;
    //return ownedchilds[uuid];
}

std::map<std::string, Component*> System::getAllChilds()
{
    std::map<std::string, Component*> resultMap;

    mforeach(Component* c,ownedchilds)
        resultMap[c->getUUID()] = c;

    return resultMap;
}
std::vector<Component*> System::getChilds()
{
     std::vector<Component*> resultVec;

     mforeach(Component* c,ownedchilds)
         resultVec.push_back(c);

    return resultVec;
}

void System::SQLInsert()
{
    DBConnector::getInstance()->Insert("systems", uuid.toRfc4122());
}
void System::SQLUpdateStates()
{
	QStringList sucList;
	foreach(System* sys, sucessors)
	{
        sucList.push_back(sys->getQUUID().toString());
	}
	QStringList preList;
	foreach(System* sys, predecessors)
	{
        preList.push_back(sys->getQUUID().toString());
	}
    DBConnector::getInstance()->Update("systems",       uuid.toRfc4122(),
                                       "sucessors",     sucList,
                                       "predecessors",  preList);
}
