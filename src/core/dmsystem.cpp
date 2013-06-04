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
    //predecessors = s.predecessors;
    lastModule = s.lastModule;
    //mutex = new QMutex(QMutex::Recursive);
	SQLInsert();
	
    currentSys = this;

    std::map<Component*,Component*> childReplaceMap;

	mforeach(Component* oldComp, s.ownedchilds)
    {
		// init name generation
		oldComp->getUUID();
		// copy elements
        switch(oldComp->getType())
        {
        case DM::COMPONENT:
            childReplaceMap[oldComp] = addComponent(oldComp->clone());
            break;
        case DM::NODE:
            childReplaceMap[oldComp] = addNode((Node*)oldComp->clone());
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
	// copy edges
	mforeach(Edge* oldEdge, s.edges)
    {
        Edge *e = (Edge*)oldEdge->clone();
		e->setStartpoint((Node*)childReplaceMap[s.findChild(e->getStartNode()->getQUUID())]);
        e->setEndpoint((Node*)childReplaceMap[s.findChild(e->getEndNode()->getQUUID())]);

        childReplaceMap[oldEdge] = addEdge(e);
    }
	// copy faces
	mforeach(Face* oldFace, s.faces)
	{
		std::vector<Node*> faceNodes = oldFace->getNodePointers();
		for(int i=0;i<faceNodes.size();i++)
			faceNodes[i] = (Node*)childReplaceMap[faceNodes[i]];

		Face* newFace = this->addFace(faceNodes);
        childReplaceMap[oldFace] = newFace;
		mforeach(Attribute* a, oldFace->getAllAttributes())
			newFace->addAttribute(*a);
	}
	// after all faces are initialized, we can copy the holes
	mforeach(Face* oldFace, s.faces)
	{
		Face* f = (Face*)childReplaceMap[oldFace];
		if(!f)
		{
			Logger(Error) << "Not found in child replace map: " << oldFace->getUUID();
			continue;
		}
		std::vector<Face*> faceHoles = oldFace->getHolePointers();
		foreach(Face* h, oldFace->getHolePointers())
			f->addHole( (Face*)childReplaceMap[h] );
	}
    // update view definitions
	mforeach(View* v, s.viewdefinitions)
	{
		View *newv = new View(*v);
        viewdefinitions[v->getName()] = newv;
		Component* dummy = v->getDummyComponent();
		if(dummy)	newv->setDummyComponent(childReplaceMap[dummy]);
    }
	// copy component views
    for ( std::map<Component*, Component*>::const_iterator it = childReplaceMap.begin(); it != childReplaceMap.end(); ++it  )
		it->second->inViews = it->first->inViews;
	
    // update views
    mforeach(Component* c, ownedchilds)
        this->updateViews(c);

	// update componentNameMap
	mforeach(Component *c, s.componentNameMap)
	{
		Component *newc = NULL;
		if(map_contains(&childReplaceMap, c, newc))
			this->componentNameMap[newc->getUUID()] = newc;
		else
			Logger(Error) << "Not found in child replace map: " << c->getUUID();
	}
}
System::~System()
{
	mforeach(Component* c, ownedchilds)
		delete c;

	ownedchilds.clear();

    foreach (DM::System * sys, this->sucessors)
        if (sys)	delete sys;

	//mforeach(View *v, viewdefinitions)
    //    delete v;
	// memory leak

	viewdefinitions.clear();

    Component::SQLDelete();
}

void System::setAccessedByModule(Module * m) 
{
	QMutexLocker ml(mutex);

    this->lastModule = m;
}

Module * System::getLastModule() const 
{
    return this->lastModule;
}

void System::updateViews(Component * c) 
{
	QMutexLocker ml(mutex);

    if (!c) 
	{
        DM::Logger(DM::Error)  << "Component NULL in updateView";
        return;
    }
    foreach (std::string view, c->getInViews())
        this->views[view][c->getUUID()] = c;
}

std::vector<std::string> System::getNamesOfViews() 
{
    std::vector<std::string> names;
    for ( std::map<std::string, View*>::const_iterator it = this->viewdefinitions.begin(); it != this->viewdefinitions.end(); ++it  )
        names.push_back(it->first);

    return names;
}

DM::View * System::getViewDefinition(string name) 
{
    if (viewdefinitions.find(name) == viewdefinitions.end()) {
        Logger(Debug) << "Couldn't find view definition for " << name;
        return 0;
    }
    return viewdefinitions[name];
}

Components System::getType() const
{
	return DM::SUBSYSTEM;
}
QString System::getTableName()
{
    return "systems";
}
Component * System::addComponent(Component* c, const DM::View & view)
{
	QMutexLocker ml(mutex);

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
    return this->getChild(uuid);
}
const Component * System::getComponentReadOnly(std::string uuid) const
{
    return this->getChild(uuid);
}
const Edge * System::getEdgeReadOnly(Node* start, Node* end)
{
    return (const Edge*)getEdge(start,end);
}
bool System::removeComponent(std::string name)
{
    return removeChild(name);
}

Node* System::addNode(Node* node)
{
	QMutexLocker ml(mutex);

    if(!addChild(node)) {
        delete node;
        return 0;
    }
    nodes[node->getQUUID()] = node;

    this->updateViews(node);
    return node;
}
Node* System::addNode(const Node &ref,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Node * n = this->addNode(new Node(ref));

    if (n == NULL)
        return NULL;
    if (!view.getName().empty()) {
        this->views[view.getName()][n->getUUID()] = n;
        n->setView(view.getName());
    }
    return n;
}

Node * System::addNode(double x, double y, double z,  const DM::View & view)
{
	QMutexLocker ml(mutex);

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
    Component* c = getChild(uuid);
    if(c && c->getType() == NODE)
        return (Node*)c;

    return NULL;
}

Node* System::getNode(QUuid uuid)
{
    Component* c = getChild(uuid);
    if(c && c->getType() == NODE)
        return (Node*)c;

    return NULL;
}
bool System::removeNode(std::string name)
{
	QMutexLocker ml(mutex);

	QUuid quuid = getChild(name)->getQUUID();
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
	QMutexLocker ml(mutex);

    if(!map_contains(&nodes, edge->getStartpoint()) || !map_contains(&nodes, edge->getEndpoint())){
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
    this->updateViews(edge);
    return edge;
}
Edge* System::addEdge(Node * start, Node * end, const View &view)
{
	QMutexLocker ml(mutex);

    Edge * e = this->addEdge(new Edge(start, end));

    if (e == 0)
        return 0;
    if (!view.getName().empty()) 
	{
        this->views[view.getName()][e->getUUID()] = e;
        e->setView(view.getName());
    }

    return e;
}
Edge* System::getEdge(std::string uuid)
{
	return (Edge*)getChild(uuid);
}
Edge* System::getEdge(QUuid uuid)
{
    if(edges.find(uuid)==edges.end())
        return 0;
    return edges[uuid];
}
Edge* System::getEdge(const std::string & startnode, const std::string & endnode)
{
	return getEdge(getNode(startnode),getNode(endnode));
}
Edge* System::getEdge(Node* start, Node* end)
{
	foreach(Edge* e,start->getEdges())
		if(e->getStartNode()==start || e->getEndNode()==end)
			return e;
	return 0;
}

bool System::removeEdge(std::string name)
{
	QMutexLocker ml(mutex);

	QUuid quuid = getChild(name)->getQUUID();
    //check if name is a edge instance
    if(edges.find(quuid)==edges.end())
        return false;

    if(!removeChild(quuid))
        return false;
    DM::Edge * e  = this->getEdge(quuid);
    edges.erase(quuid);
    return true;
}

Face* System::addFace(Face *f) 
{
	QMutexLocker ml(mutex);

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
	QMutexLocker ml(mutex);

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
    Component* c = getChild(uuid);
    if(c && c->getType() == FACE)
        return (Face*)c;

    return NULL;
}
bool System::removeFace(std::string name)
{
	QMutexLocker ml(mutex);

	QUuid quuid = getChild(name)->getQUUID();
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
	QMutexLocker ml(mutex);

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
	mforeach(Node* it, nodes)	
		n[it->getUUID()] = it;

    return n;
}
std::map<std::string, Edge*> System::getAllEdges()
{
    std::map<std::string, Edge*> e;
	mforeach(Edge* it, edges)	
		e[it->getUUID()] = it;

    return e;
}
std::map<std::string, Face*> System::getAllFaces()
{
    std::map<std::string, Face*> f;
	mforeach(Face* it, faces)	
		f[it->getUUID()] = it;

    return f;
}

bool System::addComponentToView(Component *comp, const View &view) 
{
	QMutexLocker ml(mutex);

    this->views[view.getName()][comp->getUUID()] = comp;
    comp->setView(view.getName());
    return true;
}

bool System::removeComponentFromView(Component *comp, const View &view) 
{
	QMutexLocker ml(mutex);

    comp->removeView(view);
	this->views[view.getName()].erase(comp->getUUID());
    return true;
}

System * System::addSubSystem(System *newsystem,  const DM::View & view)
{
	QMutexLocker ml(mutex);

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
	return (System*)getChild(uuid);
}
System* System::getSubSystem(QUuid uuid)
{
    if(subsystems.find(uuid)==subsystems.end())
        return 0;

    return subsystems[uuid];
}
bool System::removeSubSystem(std::string name)
{
	QMutexLocker ml(mutex);

    if(!removeChild(name))
        return false;
    return true;
}

std::map<std::string, Component*> System::getAllComponentsInView(const DM::View & view)
{
    return views[view.getName()];
}
std::vector<std::string> System::getUUIDsOfComponentsInView(DM::View view) 
{
    std::vector<std::string> names;
    std::map<std::string, DM::Component*> components_view = this->getAllComponentsInView(view);
    for (std::map<std::string, DM::Component*>::const_iterator it = components_view.begin(); it != components_view.end(); ++it)
        names.push_back(it->first);

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
	QMutexLocker ml(mutex);

    //For each view create one dummy element
    DM::View  * existingView = this->viewdefinitions[view.getName()];
    if (!existingView) 
	{
		existingView = new View(view);
        this->viewdefinitions[view.getName()] = existingView;
    }

    if (!view.writes())
        return true;
    
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
    foreach (std::string a , view.getWriteAttributes()) 
	{
        DM::Attribute attr(a);
        attr.setType(view.getAttributeType(a));
        if (view.getAttributeType(a) == Attribute::LINK)
            attr.setLink(view.getNameOfLinkedView(a), "");
        
        dummy->addAttribute(attr);
    }

    return true;
}
const std::vector<DM::View> System::getViews()  
{
    std::vector<DM::View> viewlist;

	mforeach(View* v, viewdefinitions)
		viewlist.push_back(View(*v));

    return viewlist;
}

System* System::createSuccessor()
{
	QMutexLocker ml(mutex);

    Logger(Debug) << "Create Sucessor ";// << this->getUUID();
    System* result = new DerivedSystem(this);
    this->sucessors.push_back(result);
	this->SQLUpdateStates();
    result->addPredecessors(this);
    result->SQLUpdateStates();

    return result;
}
std::vector<System*> System::getSucessors() const
{
    return sucessors;
}
std::vector<System*> System::getPredecessors() const
{
    return predecessors;
}
void System::addPredecessors(System *s)
{
	QMutexLocker ml(mutex);

    this->predecessors.push_back(s);
	this->SQLUpdateStates();
}


bool System::addChild(Component *newcomponent)
{
	QMutexLocker ml(mutex);

    if(!newcomponent)
        return false;
    ownedchilds[newcomponent->getQUUID()] = newcomponent;

	newcomponent->SetOwner(this);
	// set componentNameMap - if the name is already initialized
	if(newcomponent->HasAttribute(UUID_ATTRIBUTE_NAME))
		this->componentNameMap[newcomponent->getAttribute(UUID_ATTRIBUTE_NAME)->getString()] = newcomponent;

    return true;
}
bool System::removeChild(std::string name)
{
	QMutexLocker ml(mutex);

	return removeChild(getChild(name));
}

bool System::removeChild(Component* c)
{
	QMutexLocker ml(mutex);

    QUuid id = c->getQUUID();
    if(ownedchilds.find(id)==ownedchilds.end())
        return false;

	ownedchilds.erase(id);

    switch (c->getType())
    {
    case COMPONENT: components.erase(id);   break;
    case NODE:      nodes.erase(id);   break;
    case FACE:      faces.erase(id);   break;
    case EDGE:      edges.erase(id);   break;
    case RASTERDATA: rasterdata.erase(id);   break;
    case SUBSYSTEM:    subsystems.erase(id);   break;
    }

	if(c->HasAttribute(UUID_ATTRIBUTE_NAME))
		componentNameMap.erase(c->getUUID());

    delete c;
    return true;
}

bool System::removeChild(QUuid uuid)
{
	QMutexLocker ml(mutex);

    if(ownedchilds.find(uuid)==ownedchilds.end())
        return false;
    Component *c = ownedchilds[uuid];
    return removeChild(c);
}


Component* System::getChild(std::string name) const
{
	Component *c = NULL;
	map_contains(&componentNameMap, name, c);
	return c;
}

Component* System::getChild(QUuid uuid) const
{
	Component *c = NULL;
	map_contains(&ownedchilds, uuid, c);
	return c;
}
Component* System::findChild(QUuid uuid) const
{
    if(ownedchilds.find(uuid)==ownedchilds.end())
        return NULL;
    return ownedchilds.find(uuid)->second;
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
	isInserted = true;

    DBConnector::getInstance()->Insert("systems", uuid);
}
void System::SQLUpdateStates()
{
	QStringList sucList;
	foreach(System* sys, sucessors)
        sucList.push_back(sys->getQUUID().toString());
	
	QStringList preList;
	foreach(System* sys, predecessors)
        preList.push_back(sys->getQUUID().toString());

    DBConnector::getInstance()->Update("systems",       uuid,
                                       "sucessors",     sucList,
                                       "predecessors",  preList);
}


/******************************
 * Derived System
 *******************************/

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

	lastModule = sys->lastModule;

	CopyFrom(*sys, true);

	// copy from overwrites current system, fixes a bug
	currentSys = this;
}

/*
Node* DerivedSystem::getNode(QUuid uuid)
{
	Node* n = System::getNode(uuid);
	if(!n)
	{
		QMutexLocker ml(mutex);

		n = predecessorSys->getNode(uuid);
		if(n)
			n = addNode(new Node(*n));
	}
	return n;
}*/

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
