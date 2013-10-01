/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
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

#include <dmcomponent.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmface.h>
#include <dmrasterdata.h>
#include <dmsystem.h>
#include <dmlogger.h>
#include <dmderivedsystem.h>

#include <dmdbconnector.h>
#include <QSqlQuery>
#include <QUuid>

using namespace DM;

System::System() : Component(true)
{
	currentSys = this;

	DBConnector::getInstance();
	SQLInsert();
	isInserted = true;
}

System::System(const System& s) : Component(s, true)
{
	// copied systems don't get inserted into db
	isInserted = false;
}

System::~System()
{
	mforeach(Component* c, nodes)		delete c;
	mforeach(Component* c, edges)		delete c;
	mforeach(Component* c, faces)		delete c;
	mforeach(Component* c, rasterdata)	delete c;
	mforeach(Component* c, subsystems)	delete c;
	mforeach(Component* c, components)	delete c;

	foreach (DM::System * sys, this->sucessors)
		if (sys)	delete sys;

	//mforeach(View *v, viewdefinitions)
	//    delete v;
	// memory leak

	viewdefinitions.clear();

	if(isInserted)
		Component::SQLDelete();
}

DM::View * System::getViewDefinition(const std::string& name) 
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
		this->views[view.getName()].push_back(c);
		c->setView(view.getName());
	}

	return c;
}

const Edge * System::getEdgeReadOnly(Node* start, Node* end)
{
	return (const Edge*)getEdge(start,end);
}

Node* System::addNode(Node* node)
{
	QMutexLocker ml(mutex);

	if(!addChild(node)) {
		delete node;
		return 0;
	}
	nodes[node->getQUUID()] = node;

	return node;
}
Node* System::addNode(const Node &ref,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Node * n = this->addNode(new Node(ref));

	if (n == NULL)
		return NULL;
	if (!view.getName().empty()) {
		this->views[view.getName()].push_back(n);
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
		this->views[view.getName()].push_back(n);
		n->setView(view.getName());
	}
	return n;
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
		this->views[view.getName()].push_back(e);
		e->setView(view.getName());
	}

	return e;
}

Edge* System::getEdge(Node* start, Node* end)
{
	foreach(Edge* e,start->getEdges())
		if(e->getStartNode()==start || e->getEndNode()==end)
			return e;
	return 0;
}

Face* System::addFace(Face *f) 
{
	QMutexLocker ml(mutex);

	if(!addChild(f)) {
		delete f;
		return 0;
	}

	faces[f->getQUUID()] = f;
	return f;
}
Face* System::addFace(std::vector<DM::Node*> nodes,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Face *f = this->addFace(new Face(nodes));

	if (f == 0)
		return 0;
	if (!view.getName().empty()) {
		this->views[view.getName()].push_back(f);
		f->setView(view.getName());
	}
	return f;
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
		this->views[view.getName()].push_back(r);
		r->setView(view.getName());
	}
	return r;
}

std::vector<Component*>  System::getAllComponents()
{
	std::vector<Component*> comps;
	mforeach(Component* c, components)
		comps.push_back(c);

	return comps;
}
std::vector<Node*> System::getAllNodes()
{
	std::vector<Node*> n;
	mforeach(Node* it, nodes)	
		n.push_back(it);

	return n;
}
std::vector<Edge*> System::getAllEdges()
{
	std::vector<Edge*> e;
	mforeach(Edge* it, edges)	
		e.push_back(it);

	return e;
}
std::vector<Face*> System::getAllFaces()
{
	std::vector<Face*> f;
	mforeach(Face* it, faces)	
		f.push_back(it);

	return f;
}

bool System::addComponentToView(Component *comp, const View &view) 
{
	QMutexLocker ml(mutex);

	this->views[view.getName()].push_back(comp);
	comp->setView(view.getName());
	return true;
}

bool System::removeComponentFromView(Component *comp, const View &view) 
{
	return removeComponentFromView(comp, view.getName());
}
bool System::removeComponentFromView(Component *comp, const std::string& viewName) 
{
	QMutexLocker ml(mutex);

	comp->removeView(viewName);

	std::vector<Component*>& comps = this->views[viewName];
	std::vector<Component*>::iterator it = std::find(comps.begin(), comps.end(), comp);
	if(it != comps.end())
		comps.erase(it);

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

	subsystems[newsystem->getQUUID()] = newsystem;

	if (!view.getName().empty()) {
		this->views[view.getName()].push_back(newsystem);
		newsystem->setView(view.getName());
	}

	return newsystem;
}

std::vector<Component*> System::getAllComponentsInView(const DM::View & view)
{
	return views[view.getName()];
}

std::vector<System*> System::getAllSubSystems()
{
	std::vector<System*> syss;
	mforeach(System* s, subsystems)
		syss.push_back(s);

	return syss;
}

std::vector<RasterData*> System::getAllRasterData()
{
	std::vector<RasterData*> rasters;
	mforeach(RasterData* r, rasterdata)
		rasters.push_back(r);

	return rasters;
}

Component* System::clone()
{
	return new System(*this);
}

bool System::addView(const View& view)
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

	//extend Dummy Attribute
	foreach (std::string a , view.getWriteAttributes()) 
	{
		DM::Attribute attr(a);
		attr.setType(view.getAttributeType(a));
		if (view.getAttributeType(a) == Attribute::LINK)
			attr.setLink(view.getNameOfLinkedView(a), "");
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

	Logger(Debug) << "Create Sucessor ";
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

	newcomponent->SetOwner(this);
	// set componentNameMap - if the name is already initialized
	//if(newcomponent->HasAttribute(UUID_ATTRIBUTE_NAME))
	//	this->componentNameMap[newcomponent->getAttribute(UUID_ATTRIBUTE_NAME)->getString()] = newcomponent;

	return true;
}

bool System::removeChild(Component* c)
{
	QMutexLocker ml(mutex);

	QUuid id = c->getQUUID();

	switch (c->getType())
	{
	case COMPONENT:		components.erase(id);   break;
	case NODE:			nodes.erase(id);		break;
	case FACE:			faces.erase(id);		break;
	case EDGE:			edges.erase(id);		break;
	case RASTERDATA:	rasterdata.erase(id);   break;
	case SUBSYSTEM:		subsystems.erase(id);   break;
	}

	//if(c->HasAttribute(UUID_ATTRIBUTE_NAME))
	//	componentNameMap.erase(c->getUUID());

	typedef std::map<std::string, std::vector<Component*>> viewmap;
	
	for(viewmap::iterator it = views.begin(); it != views.end(); ++it)
		removeComponentFromView(c, it->first);
	
	delete c;
	return true;
}

std::vector<Component*> System::getAllChilds()
{
	return getChilds();
}

std::vector<Component*> System::getChilds()
{
	std::vector<Component*> resultVec;

	mforeach(Component* c,nodes)		resultVec.push_back(c);
	mforeach(Component* c,edges)		resultVec.push_back(c);
	mforeach(Component* c,faces)		resultVec.push_back(c);
	mforeach(Component* c,rasterdata)	resultVec.push_back(c);
	mforeach(Component* c,subsystems)	resultVec.push_back(c);
	mforeach(Component* c,components)	resultVec.push_back(c);

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
