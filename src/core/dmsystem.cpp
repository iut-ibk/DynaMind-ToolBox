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

#include <dmattribute.h>
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

Edge* System::addEdge(Edge* edge, const DM::View & view)
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
	edges.insert(edge);

	if (!view.getName().empty())
		this->viewCaches[view.getName()].add(edge);

	return edge;
}
Edge* System::addEdge(Node * start, Node * end, const View &view)
{
	QMutexLocker ml(mutex);

	return this->addEdge(new Edge(start, end), view);;
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
	std::vector<Component*> comps;

	if (map_contains(&viewCaches, view.getName()))
	{
		const ViewCache &vc = viewCaches[view.getName()];
		foreach(Component* c, vc.filteredElements)
			comps.push_back(c);

	}

	return comps;
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

	foreach(Edge* c, edges)
		c->_moveToDb();
	edges.clear();

	foreach(Face* c, faces)
		c->_moveToDb();
	faces.clear();

	// nodes are still linked to edges and faces, thus we export at the end
	foreach(Node* c, nodes)
		c->_moveToDb();
	nodes.clear();

	// do not export views if this system got successors -> ambiguous views!
	if (this->getSucessors().size() == 0)
	{
		for (std::map<std::string, ViewCache >::const_iterator it = viewCaches.cbegin();
			it != viewCaches.cend(); ++it)
		{
			QSqlQuery* q = db->getQuery("DELETE FROM views WHERE viewname LIKE ?");
			q->addBindValue(QString::fromStdString(it->first));
			db->ExecuteQuery(q);

			foreach(QUuid quuid, it->second.rawElements)
				db->Insert("views", quuid, "viewname", (QVariant)QString::fromStdString(it->first));
		}
	}
	quuidMap.clear();
	viewCaches.clear();
	// rasterdatas won't get removed, as well as systems
}

std::vector<QUuid> GetVector(QByteArray qba)
{
	QDataStream stream(&qba, QIODevice::ReadOnly);
	QByteArray str;
	std::vector<QUuid> result;

	unsigned int len = 0;
	stream >> len;
	for (unsigned int i = 0; i < len; i++)
	{
		stream >> str;
		result.push_back(str);
	}
	return result;
}

void System::_importViewElementsFromDB()
{
	DBConnector* db = DBConnector::getInstance();
	std::map<QUuid, Node*>	loadedNodes;

	for (std::map<std::string, ViewCache >::iterator viewItem = viewCaches.begin();
		viewItem != viewCaches.end(); ++viewItem)
	{
		if (!viewItem->second.view.reads())
			continue;

		std::map<QUuid, std::pair<QUuid, Vector3> > nodesInView;
		std::map<QUuid, std::pair<QUuid, std::vector<QUuid> > > facesInView;
		std::map<QUuid, Component*>	elementsInView;

		switch (viewItem->second.view.getType())
		{
		case COMPONENT:
			{
				QSqlQuery* q = db->getQuery(
					"SELECT components.* FROM components INNER JOIN views ON components.uuid=views.uuid WHERE views.viewname=?");
				q->addBindValue(QString::fromStdString(viewItem->first));
				if (db->ExecuteSelectQuery(q))
				{
					foreach(const QList<QVariant>& r, *db->getResults())
					{
						Component* c = new Component();
						c->setQUuid(r.at(0).toByteArray());
						c->isInserted = true;

						QUuid quuidOwner = r.at(1).toByteArray();

						System *sys = this;
						while (this->getQUUID() != quuidOwner && sys != NULL)
							sys = this->getPredecessor();

						if (sys && viewItem->second.add(c))
						{
							sys->addComponent(c);
							elementsInView[c->getQUUID()] = c;
						}
						else
						{
							if (!sys)
								Logger(Error) << "system " << quuidOwner.toString().toStdString() << "not found";
							delete c;
						}
					}
				}
					  }
			break;
		case NODE:
			{
				QSqlQuery* q = db->getQuery(
					"SELECT nodes.* FROM nodes INNER JOIN views ON nodes.uuid=views.uuid WHERE views.viewname=?");

				q->addBindValue(QString::fromStdString(viewItem->first));

				if (db->ExecuteSelectQuery(q))
					foreach(const QList<QVariant>& r, *db->getResults())
					nodesInView[r.at(0).toByteArray()] = std::pair<QUuid, Vector3>(r.at(1).toByteArray(),
					Vector3(r.at(2).toDouble(),
					r.at(3).toDouble(),
					r.at(4).toDouble()));
				 }
			break;
		case EDGE:
			{
				QSqlQuery* q = db->getQuery(
					"SELECT nodes.* FROM nodes \
										INNER JOIN edges ON edges.startnode = nodes.uuid OR edges.endnode = nodes.uuid \
															INNER JOIN views ON views.uuid = edges.uuid WHERE views.viewname = ?");

				q->addBindValue(QString::fromStdString(viewItem->first));

				if (db->ExecuteSelectQuery(q))
					foreach(const QList<QVariant>& r, *db->getResults())
					nodesInView[r.at(0).toByteArray()] = std::pair<QUuid, Vector3>(r.at(1).toByteArray(),
					Vector3(r.at(2).toDouble(),
					r.at(3).toDouble(),
					r.at(4).toDouble()));
				 }
			break;
		case FACE:
			{
				QSqlQuery* q = db->getQuery(
					"SELECT faces.* FROM faces INNER JOIN views ON faces.uuid=views.uuid WHERE views.viewname=?");

				q->addBindValue(QString::fromStdString(viewItem->first));

				if (db->ExecuteSelectQuery(q))
					foreach(const QList<QVariant>& r, *db->getResults())
					facesInView[r.at(0).toByteArray()] = std::pair < QUuid, std::vector<QUuid> >(r.at(1).toByteArray(),
					GetVector(r.at(2).toByteArray()));
				typedef std::pair < QUuid, std::vector<QUuid> > OwnerVectorPair;
				mforeach(const OwnerVectorPair& v, facesInView)
				{
					foreach(QUuid quuid, v.second)
					{
						QSqlQuery* qn = db->getQuery("SELECT * FROM nodes WHERE uuid=?");
						qn->addBindValue(quuid.toByteArray());
						if (db->ExecuteSelectQuery(qn))
						{
							nodesInView[db->getResults()->at(0).at(0).toByteArray()] = std::pair<QUuid, Vector3>(db->getResults()->at(0).at(1).toByteArray(),
								Vector3(db->getResults()->at(0).at(2).toDouble(),
								db->getResults()->at(0).at(3).toDouble(),
								db->getResults()->at(0).at(4).toDouble()));
						}
					}
				}
				 }
			break;
		}

		// create nodes
		bool nodeView = viewItem->second.view.getType() == NODE;
		if (nodesInView.size() > 0)
		{
			for (std::map < QUuid, std::pair < QUuid, Vector3 > > ::iterator nodeItem = nodesInView.begin(); nodeItem != nodesInView.end(); ++nodeItem)
			{
				Node* n = NULL;
				if (!map_contains(&loadedNodes, nodeItem->first, n))
				{
					Node* n = new Node(nodeItem->second.second.x, nodeItem->second.second.y, nodeItem->second.second.z);
					n->setQUuid(nodeItem->first);
					n->isInserted = true;

					QUuid quuidOwner = nodeItem->second.first;

					System* sys = this;
					while (sys != NULL && sys->getQUUID() != quuidOwner)
						sys = this->getPredecessor();

					if ((!nodeView || viewItem->second.add(n)) && sys)
					{
						loadedNodes[n->getQUUID()] = sys->addNode(n);
						if (nodeView)
							elementsInView[n->getQUUID()] = n;
					}
					else
					{
						if (!sys)
							Logger(Error) << "system " << quuidOwner.toString().toStdString() << "not found";
						delete n;
					}
				}
				else if (nodeView)
					viewItem->second.add(n);
			}
		}

		// link edges and faces
		switch (viewItem->second.view.getType())
		{
		case EDGE:
			{
				QSqlQuery* q = db->getQuery(
					"SELECT edges.* FROM edges INNER JOIN views ON edges.uuid=views.uuid WHERE views.viewname=?");
				q->addBindValue(QString::fromStdString(viewItem->first));
				if (db->ExecuteSelectQuery(q))
				{
					foreach(const QList<QVariant>& r, *db->getResults())
					{
						Node* s = loadedNodes[r.at(2).toByteArray()];
						Node* e = loadedNodes[r.at(3).toByteArray()];
						if (!s || !e)
						{
							Logger(Error) << "loading edge failed: missing start or end point";
							continue;
						}
						Edge* c = new Edge(s, e);
						c->setQUuid(r.at(0).toByteArray());
						c->isInserted = true;
						QUuid quuidOwner = r.at(1).toByteArray();

						System *sys = this;
						while (this->getQUUID() != quuidOwner && sys != NULL)
							sys = this->getPredecessor();

						if (viewItem->second.add(c) && sys)
						{
							this->addEdge(c);
							elementsInView[c->getQUUID()] = c;
						}
						else
						{
							if (!sys)
								Logger(Error) << "system " << quuidOwner.toString().toStdString() << "not found";
							delete c;
						}
					}
				}
				 }
			break;
		case FACE:
			{
				typedef std::map < QUuid, std::pair<QUuid, std::vector<QUuid> > > RawFaces;
				for (RawFaces::iterator it = facesInView.begin(); it != facesInView.end(); ++it)
				{
					std::vector<Node*> nodes;
					foreach(QUuid quuid, it->second.second)
						nodes.push_back(loadedNodes[quuid]);

					Face* f = new Face(nodes);
					f->setQUuid(it->first);
					f->isInserted = true;
					QUuid quuidOwner = it->second.first;

					System *sys = this;
					while (this->getQUUID() != quuidOwner && sys != NULL)
						sys = this->getPredecessor();

					if (viewItem->second.add(f) && sys)
					{
						this->addFace(f);
						elementsInView[f->getQUUID()] = f;
					}
					else
					{
						if (!sys)
							Logger(Error) << "system " << quuidOwner.toString().toStdString() << "not found";
						delete f;
					}
				}
				 }
			break;
		}

		foreach(std::string attributeName, viewItem->second.view.getAllAttributes())
		{
			QSqlQuery* q = db->getQuery("SELECT attributes.* FROM attributes \
										INNER JOIN nodes ON nodes.uuid = attributes.owner \
										INNER JOIN views ON views.uuid = nodes.uuid WHERE views.viewname = ? AND attributes.name = ?");
			q->addBindValue(QString::fromStdString(viewItem->first));
			q->addBindValue(QString::fromStdString(attributeName));

			if (db->ExecuteSelectQuery(q))
			{
				foreach(const QList<QVariant>& r, *db->getResults())
				{
					Component* c = NULL;
					if (map_contains(&elementsInView, QUuid(r.at(0).toByteArray()), c))
					{
						c->addAttribute(
							Attribute::_createAttribute(attributeName, c, r.at(3),
								(Attribute::AttributeType)r.at(2).toInt(), c->currentSys));
					}
				}
			}
		}
	}
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
				filteredElements.insert(c);
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
				filteredElements.insert(c);
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
	rawElements.insert(c->getQUUID());

	if(legal(c))
	{
		filteredElements.insert(c);
		return true;
	}
	return false;
}

bool System::ViewCache::remove(Component* c)
{
	rawElements.erase(c->getQUUID());

	if(legal(c))
	{
		filteredElements.erase(c);
		return true;
	}
	return false;
}

bool System::ViewCache::legal(Component* c)
{
	return eq.eval(c);
}

