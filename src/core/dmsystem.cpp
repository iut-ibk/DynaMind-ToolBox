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
#include <QRegExp>

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
	foreach(Component* c, nodes)		delete c;
	foreach(Component* c, edges)		delete c;
	foreach(Component* c, faces)		delete c;
	foreach(Component* c, rasterdata)	delete c;
	foreach(Component* c, subsystems)	delete c;
	foreach(Component* c, components)	delete c;

	foreach (DM::System * sys, this->sucessors)
		if (sys)	delete sys;

	if(isInserted)
		Component::SQLDelete();
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

	components.insert(c);

	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(c);

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
	nodes.insert(node);

	return node;
}
Node* System::addNode(const Node &ref,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Node * n = this->addNode(new Node(ref));

	if (n == NULL)
		return NULL;
	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(n);

	return n;
}

Node * System::addNode(double x, double y, double z,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Node * n = this->addNode(new Node(x, y, z));

	if (n == NULL)
		return NULL;
	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(n);

	return n;
}

Edge* System::addEdge(Edge* edge)
{
	QMutexLocker ml(mutex);

	if(	nodes.find(edge->getStartNode()) == nodes.end() || 
		nodes.find(edge->getEndNode()) == nodes.end())
	{
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
		this->viewCaches[view.getName()].add(e);

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

	faces.insert(f);
	return f;
}
Face* System::addFace(std::vector<DM::Node*> nodes,  const DM::View & view)
{
	QMutexLocker ml(mutex);

	Face *f = this->addFace(new Face(nodes));

	if (f == 0)
		return 0;
	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(f);

	return f;
}

RasterData * System::addRasterData(RasterData *r, const DM::View & view)
{
	QMutexLocker ml(mutex);

	if(!addChild(r)) {
		delete r;
		return 0;
	}

	rasterdata.insert(r);

	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(r);

	return r;
}

std::vector<Component*>  System::getAllComponents()
{
	return std::vector<Component*>(components.begin(), components.end());
}
std::vector<Node*> System::getAllNodes()
{
	return std::vector<Node*>(nodes.begin(), nodes.end());
}
std::vector<Edge*> System::getAllEdges()
{
	return std::vector<Edge*>(edges.begin(), edges.end());
}
std::vector<Face*> System::getAllFaces()
{
	return std::vector<Face*>(faces.begin(), faces.end());
}

bool System::addComponentToView(Component *comp, const View &view) 
{
	QMutexLocker ml(mutex);

	this->viewCaches[view.getName()].add(comp);
	return true;
}

bool System::removeComponentFromView(Component *comp, const View &view) 
{
	return removeComponentFromView(comp, view.getName());
}
bool System::removeComponentFromView(Component *comp, const std::string& viewName) 
{
	QMutexLocker ml(mutex);

	viewCaches[viewName].remove(comp);

	/*std::vector<Component*>& comps = this->views[viewName];
	std::vector<Component*>::iterator it = std::find(comps.begin(), comps.end(), comp);
	if(it != comps.end())
		comps.erase(it);*/

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

	subsystems.insert(newsystem);

	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(newsystem);

	return newsystem;
}

std::vector<Component*> System::getAllComponentsInView(const DM::View & view)
{
	return viewCaches[view.getName()].filteredElements;
}

std::vector<System*> System::getAllSubSystems()
{
	return std::vector<System*>(subsystems.begin(), subsystems.end());
}

std::vector<RasterData*> System::getAllRasterData()
{
	return std::vector<RasterData*>(rasterdata.begin(), rasterdata.end());
}

Component* System::clone()
{
	return new System(*this);
}

System* System::createSuccessor()
{
	QMutexLocker ml(mutex);

	Logger(Debug) << "Create Sucessor ";
	System* result = new DerivedSystem(this);
	this->sucessors.push_back(result);
	this->SQLUpdateStates();
	result->SQLUpdateStates();

	return result;
}

std::vector<System*> System::getSucessors() const
{
	return sucessors;
}

System* System::getPredecessor() const
{
	// only derived systems will have a predecessor
	return NULL;
}

bool System::addChild(Component *newcomponent)
{
	QMutexLocker ml(mutex);

	if(!newcomponent)
		return false;

	quuidMap[newcomponent->getQUUID()] = newcomponent;
	newcomponent->SetOwner(this);

	return true;
}

bool System::removeChild(Component* c)
{
	QMutexLocker ml(mutex);

	quuidMap.erase(c->getQUUID());

	switch (c->getType())
	{
	case COMPONENT:		components.erase(c);				break;
	case NODE:			nodes.erase((Node*)c);				break;
	case FACE:			faces.erase((Face*)c);				break;
	case EDGE:			edges.erase((Edge*)c);				break;
	case RASTERDATA:	rasterdata.erase((RasterData*)c);  break;
	case SUBSYSTEM:		subsystems.erase((System*)c);		break;
	}

	typedef std::map<std::string, ViewCache > viewmap;
	
	for(viewmap::iterator it = viewCaches.begin(); it != viewCaches.end(); ++it)
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
	std::vector<Component*> resultVec(components.begin(), components.end());
	resultVec.insert(resultVec.end(), nodes.begin(), nodes.end());
	resultVec.insert(resultVec.end(), edges.begin(), edges.end());
	resultVec.insert(resultVec.end(), faces.begin(), faces.end());
	resultVec.insert(resultVec.end(), rasterdata.begin(), rasterdata.end());
	resultVec.insert(resultVec.end(), subsystems.begin(), subsystems.end());

	return resultVec;
}

Component* System::getChild(QUuid quuid)
{
	Component* c;
	if(map_contains(&quuidMap, quuid, c))	
		return c;
	return NULL;
}

Component* System::getSuccessingComponent(const Component* formerComponent)
{
	// this is a root system => no predec. info available
	return NULL;
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

	QString pre;
	if(System* sys = getPredecessor())
		pre = sys->getQUUID().toString();

	DBConnector::getInstance()->Update("systems",       uuid,
		"sucessors",     sucList,
		"predecessors",  pre);
}

void System::updateView(const View& view)
{
	viewCaches[view.getName()].sys = this;
	viewCaches[view.getName()].apply(view);
}

void System::_moveToDb()
{
	DBConnector* db = DBConnector::getInstance();
	foreach(Component* c, components)
		c->_moveToDb();
	components.clear();

	foreach(Node* c, nodes)
		c->_moveToDb();
	nodes.clear();

	foreach(Edge* c, edges)
		c->_moveToDb();
	edges.clear();

	foreach(Face* c, faces)
		c->_moveToDb();
	faces.clear();

	for (std::map<std::string, ViewCache >::const_iterator it = viewCaches.begin();
		it != viewCaches.end(); ++it)
		foreach(QUuid quuid, it->second.rawElements)
			DBConnector::getInstance()->Insert(	"views",	quuid,
												"viewname", (QVariant)QString::fromStdString(it->first));




	viewCaches.clear();

	// rasterdatas won't get removed, as well as systems
}

void System::_importViewElementsFromDB()
{

}


void System::ViewCache::apply(const View& view)
{
	if(this->view.getName().length() != 0)
	{
		// if not initializing 
		if(view.getName() != this->view.getName())
		{
			Logger(Error) << "view filter map corrupt";
			return;
		}
		// filter stays the same, continue
		if(view.getFilter() == this->view.getFilter())
		{
			this->view = view;
			return;
		}
	}

	this->view = view;

	QString filterString = QString::fromStdString(view.getFilter());
	if(filterString.length() == 0)
	{
		// just copy raw elements
		filteredElements.clear();
		foreach(QUuid quuid, rawElements)
			if(Component* c = sys->getChild(quuid))
				filteredElements.push_back(c);
	}
	else
	{
		QRegExp rx("([\\w\\[\\]]+)\\s*([><=]){1,1}\\s*([\\d\\.\\-]+)");

		if(rx.indexIn(filterString) > -1)
		{
			QString op = rx.cap(2);
			if(op == "=")
				eq.op = ViewCache::Equation::EQUAL;
			else if(op == "<=")
				eq.op = ViewCache::Equation::LEQUAL;
			else if(op == ">=")
				eq.op = ViewCache::Equation::HEQUAL;
			else if(op == ">")
				eq.op = ViewCache::Equation::HIGHER;
			else if(op == "<")
				eq.op = ViewCache::Equation::LOWER;
			else 
			{
				Logger(Error) << "unknown filter operator " << op;
				return;
			}
		
			std::string str = rx.cap(1).toStdString();

			if(str == "[x]")		eq.axis = Equation::X;
			else if(str == "[y]")	eq.axis = Equation::Y;
			else if(str == "[z]")	eq.axis = Equation::Z;
			else
			{
				eq.axis = Equation::NONE;
				eq.varName = str;
			}

			eq.val = rx.cap(3).toDouble();
		}
		else 
		{
			Logger(Error) << "invalid filter expression " << filterString;
			return;
		}

		// renew filtered elements
		filteredElements.clear();
		foreach(QUuid quuid, rawElements)
		{
			Component* c = sys->getChild(quuid);
			if(c && eq.eval(c))
				filteredElements.push_back(c);
		}
	}
}

bool System::ViewCache::Equation::eval(Component* c) const
{
	if(this->axis == NONE && this->varName.length() == 0)
		return true;

	double d;

	if(c->getType() == NODE)
	{
		Node* n = (Node*)c;

		switch(axis)
		{
		case X:	d = n->getX();	break;
		case Y:	d = n->getY();	break;
		case Z:	d = n->getZ();	break;
		}
	}

	if(axis == NONE)
	{
		Attribute* a = c->getAttribute(varName);
		if(a->getType() == Attribute::DOUBLE)
			d = a->getDouble();

	}

	switch(op)
	{
	case EQUAL:	return d == val;
	case LEQUAL:	return d <= val;
	case HEQUAL:	return d >= val;
	case LOWER:	return d < val;
	case HIGHER:	return d > val;
	}
	return false;
}

bool System::ViewCache::add(Component* c)
{
	rawElements.push_back(c->getQUUID());

	if(legal(c))
	{
		filteredElements.push_back(c);
		return true;
	}
	return false;
}

bool System::ViewCache::remove(Component* c)
{
	rawElements.erase(find(rawElements.begin(), rawElements.end(), c->getQUUID()));

	if(legal(c))
	{
		filteredElements.erase(find(filteredElements.begin(), filteredElements.end(), c));
		return true;
	}
	return false;
}

bool System::ViewCache::legal(Component* c)
{
	return eq.eval(c);
}

