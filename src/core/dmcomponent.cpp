/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
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

#include <QtCore>
#include <dmcomponent.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmrasterdata.h>
#include <dmattribute.h>
#include <dmsystem.h>
#include <assert.h>
#include <dmlogger.h>

#include "dmface.h"
#include <QUuid>

#include <dmdbconnector.h>
#include <QSqlQuery>

using namespace DM;

Component::Component()
{
    this->uuid = QUuid::createUuid().toString().toStdString();
    name = "";
    childsview = std::map<std::string,Component*>();
    attributesview = std::map<std::string,Attribute*>();
    ownedchilds = std::map<std::string,Component*>();
    ownedattributes =  std::map<std::string,Attribute*>();
    inViews = std::set<std::string>();
    currentSys = NULL;
}

void Component::createNewUUID() {
	this->setUUID(QUuid::createUuid().toString().toStdString());
}

bool Component::isInView(View view) const {
    foreach (std::string s, inViews) {
        if (view.getName().compare(s) == 0)
            return true;
    }

    return false;
}

void Component::setName(std::string name) {
    this->name = name;
	
	QSqlQuery q;
	q.prepare("UPDATE components SET name=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);
}

std::string Component::getName() const {
return name;
}

Component::Component(const Component& c)
{
    this->uuid=c.uuid;
    attributesview=c.attributesview;
    ownedchilds=c.ownedchilds;
    inViews = c.inViews;
	currentSys = NULL;

    std::map<std::string,Component*>::iterator it;

    for ( it=ownedchilds.begin() ; it != ownedchilds.end(); it++ )
    {
        childsview[(*it).first]=ownedchilds[(*it).first];
    }
    ownedchilds.clear();
}
Component * Component::updateChild(Component * c) {
    if (ownedchilds.find(c->getUUID()) != ownedchilds.end())
        return c;
    Component * c_new = c->clone();
    changeChild(c_new);

    return c_new;
}

Component::~Component()
{
    while(ownedchilds.size())
    {
        delete (*ownedchilds.begin()).second;
        ownedchilds.erase(ownedchilds.begin());
    }

    while(ownedattributes.size())
    {
        delete (*ownedattributes.begin()).second;
        ownedattributes.erase(ownedattributes.begin());
    }
}

void Component::setUUID(std::string uuid)
{
	QSqlQuery q;
	q.prepare("UPDATE components SET uuid=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);

    this->uuid=uuid;
}

std::string Component::getUUID()
{
    return uuid;
}
std::string Component::getStateUUID()
{
	return stateUuid;
}

DM::Components Component::getType()
{
    return DM::COMPONENT;
}
/*
void Component::getRawData(QBuffer* buf)
{
	return;
}*/

bool Component::addAttribute(std::string name, double val) 
{
	SQLLoadAttributes();
    if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }
    Attribute  attr = Attribute(name, val);
    return this->addAttribute(attr);
}

bool Component::addAttribute(std::string name, std::string val) 
{
	SQLLoadAttributes();
    if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }
    Attribute  attr = Attribute(name, val);
    return this->addAttribute(attr);
}

bool Component::addAttribute(Attribute newattribute)
{
	SQLLoadAttributes();
    if(attributesview.find(newattribute.getName())!=attributesview.end())
        return this->changeAttribute(newattribute);
    Attribute * a = new Attribute(newattribute);
    attributesview[newattribute.getName()] = a;
    ownedattributes[newattribute.getName()] = a;

	SQLInsertAttribute(a);
    return true;
}

bool Component::changeAttribute(Attribute newattribute)
{
	SQLLoadAttributes();
    if(attributesview.find(newattribute.getName())==attributesview.end()) {
        return this->addAttribute(newattribute);
    }
    if(ownedattributes.find(newattribute.getName())==ownedattributes.end()) {
        ownedattributes[newattribute.getName()] = new Attribute(*(attributesview[newattribute.getName()]));
        attributesview[newattribute.getName()] = ownedattributes[newattribute.getName()];
    }
    Attribute * attr = attributesview[newattribute.getName()];
    Attribute::AttributeType type = attr->getType();
    attr->setDouble(newattribute.getDouble());
    attr->setDoubleVector(newattribute.getDoubleVector());
    attr->setString(newattribute.getString());
    attr->setStringVector(newattribute.getStringVector());
    attr->setType(type);

	SQLUpdateAttribute(newattribute.getName(), attr);

    return true;
}

bool Component::changeAttribute(std::string s, double val)
{
    return this->changeAttribute(Attribute(s, val));
}

bool Component::changeAttribute(std::string s, std::string val)
{
    return this->changeAttribute(Attribute(s, val));
}

bool Component::removeAttribute(std::string name)
{
	SQLLoadAttributes();
    if(attributesview.find(name)!=attributesview.end())
    {
        if(ownedattributes.find(name)!=ownedattributes.end())
        {
            delete ownedattributes[name];
            ownedattributes.erase(name);

			SQLDeleteAttribute(name);
        }

        attributesview.erase(name);
        return true;
    }

    return false;
}

Attribute* Component::getAttribute(std::string name)
{
	SQLLoadAttributes();
    if(attributesview.find(name)==attributesview.end()) {
        this->addAttribute(Attribute(name));
    }
    return attributesview[name];
}

const std::map<std::string, Attribute*> & Component::getAllAttributes() const
{
    return attributesview;
}

bool Component::addChild(Component *newcomponent)
{
	SQLLoadChilds();
    if(!newcomponent)
        return false;

    if(childsview.find(newcomponent->getUUID())!=childsview.end())
        return false;

    childsview[newcomponent->getUUID()] = newcomponent;
    ownedchilds[newcomponent->getUUID()] = newcomponent;
	newcomponent->stateUuid = this->getStateUUID();

	SQLInsertChild(newcomponent);
    return true;
}

Component* Component::clone() {
    return new Component(*this);
}

bool Component::changeChild(Component *newcomponent)
{
	SQLLoadChilds();
    if(!newcomponent)
        return false;

    if(ownedchilds.find(newcomponent->getUUID())!=ownedchilds.end())
	{
		SQLDeleteChild(newcomponent);
        delete ownedchilds[newcomponent->getUUID()];
	}
	
    ownedchilds[newcomponent->getUUID()] = newcomponent;
    childsview[newcomponent->getUUID()] = newcomponent;

	// currentSystem and statuuid are not set - if the change results from 
	// allocating a new successor state component;
	newcomponent->uuid = this->getStateUUID();
	SQLInsertChild(newcomponent);

    return true;
}

bool Component::removeChild(std::string name)
{
	SQLLoadChilds();
    if(childsview.find(name)!=childsview.end())
    {
        if(ownedchilds.find(name)!=ownedchilds.end())
        {
			SQLDeleteChild(ownedchilds[name]);
            delete ownedchilds[name];
            ownedchilds.erase(name);
        }

        childsview.erase(name);
        return true;
    }
    return false;
}

Component* Component::getChild(std::string name)
{
	SQLLoadChilds();
    if(childsview.find(name)==childsview.end())
        return 0;
    return childsview[name];
}

void Component::setView(std::string view)
{
    this->inViews.insert(view);
}

void Component::setView(const DM::View & view)
{
    this->inViews.insert(view.getName());
}

void Component::removeView(const View &view)
{
    this->inViews.erase(view.getName());
}

const set<std::string> &Component::getInViews() const {
    return this->inViews;

}
std::map<std::string, Component*> Component::getAllChilds()
{
    return childsview;
}

System * Component::getCurrentSystem() {
    return this->currentSys;
}

void Component::setCurrentSystem(System *sys) {
    this->currentSys = sys;
}

void Component::SQLLoadChilds()
{
	if(bChilds) return;
		bChilds= true;

	QSqlQuery q;
	q.prepare("SELECT uuid,name,type,value FROM components WHERE owner LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(getUUID()));
	q.addBindValue(QString::fromStdString(getStateUUID()));

	while(q.next())
	{
		int type = q.value(2).toInt();
		QByteArray value = q.value(3).toByteArray();
		switch(type)
		{
		case COMPONENT: ownedchilds[uuid]  = new Component();
			break;
		case NODE:  ownedchilds[uuid] = new Node(value);
			break;
		case EDGE: ownedchilds[uuid] = new Edge(value);
			break;
		case FACE: ownedchilds[uuid]  = new Face(value);
			break;
		case RASTERDATA: ownedchilds[uuid] = new RasterData(value);
			break;
		case SUBSYSTEM: ownedchilds[uuid] = new System();
			break;
		}
		ownedchilds[uuid]->setUUID(q.value(0).toString().toStdString());
		ownedchilds[uuid]->setName(q.value(1).toString().toStdString());
	}
}

void Component::SQLFreeChilds()
{
	FreeMap(ownedchilds);
	bChilds = false;
}

void Component::SQLInsertChild(Component* c)
{
	QSqlQuery q;
	q.prepare("INSERT INTO components (uuid,owner,stateuuid,type,name,value) VALUES (?,?,?,?,?,?)");
	q.addBindValue(QString::fromStdString(c->getUUID()));
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(c->getStateUUID()));
	q.addBindValue(c->getType());
	q.addBindValue(QString::fromStdString(c->getName()));
	q.addBindValue(c->GetValue());

	if(!q.exec())	PrintSqlError(&q);
}

void Component::SQLDeleteChild(Component* c)
{
	QSqlQuery q;
	q.prepare("DELETE FROM components WHERE uuid LIKE ? AND owner LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(c->getUUID()));
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(c->getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);
}

void Component::SQLLoadAttributes()
{
	if(bAttributes) return;
	bAttributes = true;

	QSqlQuery q;
	q.prepare("SELECT name,type,value FROM attributes WHERE owning_component LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(getUUID()));
	q.addBindValue(QString::fromStdString(getStateUUID()));

	while(q.next())
	{
		int type = q.value(1).toInt();
		std::string name = q.value(0).toString().toStdString();

		this->ownedattributes[name] = new Attribute(name, type, q.value(3).toByteArray());
	}
}
void Component::SQLFreeAttributes()
{
	FreeMap(ownedattributes);
	bAttributes = false;
}

void Component::SQLInsertAttribute(Attribute *newAttribute)
{
	QSqlQuery q;
	q.prepare("INSERT INTO attributes (owner,stateuuid,name,type,value) VALUES (?,?,?,?,?)");
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	q.addBindValue(QString::fromStdString(newAttribute->getName()));
	q.addBindValue(newAttribute->getType());
	q.addBindValue(newAttribute->getValue());
	if(!q.exec())	PrintSqlError(&q);
}
void Component::SQLUpdateAttribute(std::string name, Attribute *newAttribute)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET name=?,type=?,value=? WHERE owner LIKE ? AND stateuuid LIKE ? AND name LIKE ?");
	q.addBindValue(QString::fromStdString(newAttribute->getName()));
	q.addBindValue(newAttribute->getType());
	q.addBindValue(newAttribute->getValue());
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	q.addBindValue(QString::fromStdString(name));
	if(!q.exec())	PrintSqlError(&q);
}
void Component::SQLDeleteAttribute(std::string name)
{
	QSqlQuery q;
	q.prepare("DELETE FROM attributes WHERE owner LIKE ? AND stateuuid LIKE ? AND name LIKE ?");
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	q.addBindValue(QString::fromStdString(name));
	if(!q.exec())	PrintSqlError(&q);
}


void Component::SQLInsertDeepCopy()
{
	// in case of a root system, this->stateUuid is already the new stateuuid (see copy constructor)
	// thus we dont have to create it here again, just go on to the components itself
	foreach(ComponentPair p, this->ownedchilds)
	{
		Component *copy = new Component(*p.second);
		copy->stateUuid = this->getStateUUID();
		copy->SQLInsertDeepCopy();
		delete copy;
	}

	foreach(AttributePair p, this->ownedattributes)
	{
		QSqlQuery q;
		q.prepare("INSERT INTO attributes (owner,stateuuid,name,type,value) VALUES ?,?,?,?,?");
		q.addBindValue(QString::fromStdString(this->getUUID()));
		q.addBindValue(QString::fromStdString(this->getStateUUID()));
		q.addBindValue(QString::fromStdString(p.second->getName()));
		q.addBindValue(p.second->getType());
		q.addBindValue(p.second->getValue());
	if(!q.exec())	PrintSqlError(&q);
	}
}
