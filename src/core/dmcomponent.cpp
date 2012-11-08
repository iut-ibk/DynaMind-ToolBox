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
    DBConnector::getInstance();
    this->uuid = QUuid::createUuid().toString().toStdString();
    this->stateUuid = QUuid::createUuid().toString().toStdString();
    name = "";
    ownedattributes = std::map<std::string,Attribute*>();

    inViews = std::set<std::string>();
    currentSys = NULL;

    DBConnector::getInstance();
    SQLInsertComponent();
}
Component::Component(bool b)
{
    DBConnector::getInstance();
    this->uuid = QUuid::createUuid().toString().toStdString();
    this->stateUuid = QUuid::createUuid().toString().toStdString();
    name = "";
    ownedattributes = std::map<std::string,Attribute*>();

    inViews = std::set<std::string>();
    currentSys = NULL;
}

Component::Component(const Component& c)
{
    uuid=c.uuid;
    this->stateUuid = QUuid::createUuid().toString().toStdString();
    inViews = c.inViews;
    name = c.name;

    std::map<std::string,Attribute*> attrmap = c.ownedattributes;
    for (std::map<std::string,Attribute*>::iterator it=attrmap.begin() ; it != attrmap.end(); ++it )
        this->addAttribute(*it->second);

    SQLInsertComponent();
}
Component::Component(const Component& c, bool b)
{
    uuid=c.uuid;
    this->stateUuid = QUuid::createUuid().toString().toStdString();
    inViews = c.inViews;
    name = c.name;

    std::map<std::string,Attribute*> attrmap = c.ownedattributes;
    for (std::map<std::string,Attribute*>::iterator it=attrmap.begin() ; it != attrmap.end(); ++it )
        this->addAttribute(*it->second);
}

Component::~Component()
{
	while(ownedattributes.size())
    {
        delete (*ownedattributes.begin()).second;
        ownedattributes.erase(ownedattributes.begin());
    }
	// if this class is not of type component, nothing will happen
    SQLDeleteComponent();
}

void Component::createNewUUID() 
{
	this->setUUID(QUuid::createUuid().toString().toStdString());
}

bool Component::isInView(View view) const 
{
    foreach (std::string s, inViews) {
        if (view.getName().compare(s) == 0)
            return true;
    }

    return false;
}

void Component::setName(std::string name) 
{
    DBConnector::getInstance()->Update(getTableName(),
                                       QString::fromStdString(uuid),
                                       QString::fromStdString(stateUuid),
                                       "name", QString::fromStdString(name));
    /*
    QSqlQuery q;
	q.prepare("UPDATE components SET name=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}

std::string Component::getName() const 
{
	return name;
}

void Component::setUUID(std::string uuid)
{
    DBConnector::getInstance()->Update(getTableName(),
                                       QString::fromStdString(uuid),
                                       QString::fromStdString(stateUuid),
                                       "uuid", QString::fromStdString(uuid));
    /*
	QSqlQuery q;
	q.prepare("UPDATE components SET uuid=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
    if(!q.exec())	PrintSqlError(&q);*/

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
/*std::string Component::getTableName()
{
    switch(getType())
    {
        case COMPONENT:	return "components";
        case NODE:		return "nodes";
        case EDGE:		return "edges";
        case FACE:		return "faces";
        case SUBSYSTEM:	return "systems";
        case RASTERDATA:return "rasterdatas";
    }
    return "";
}*/
QString Component::getTableName()
{
    return "components";
}

bool Component::addAttribute(std::string name, double val) 
{
	if(HasAttribute(name))
        return this->changeAttribute(name, val);

    Attribute tmp(name, val);
    return this->addAttribute(tmp);
}

bool Component::addAttribute(std::string name, std::string val) 
{
	if(HasAttribute(name))
        return this->changeAttribute(name, val);

    Attribute tmp(name, val);
    return this->addAttribute(tmp);
}

bool Component::addAttribute(Attribute &newattribute)
{
	if(HasAttribute(newattribute.getName()))
        return this->changeAttribute(newattribute);

    Attribute * a = new Attribute(newattribute);
    ownedattributes[newattribute.getName()] = a;

	a->SetOwner(this);
    return true;
}

bool Component::changeAttribute(Attribute newattribute)
{
	if(!HasAttribute(newattribute.getName()))
		return this->addAttribute(newattribute);

	Attribute * attr = ownedattributes[newattribute.getName()];
	Attribute::AttributeType type = attr->getType();
	std::vector<std::string> vecStr;
	std::vector<double> vecDbl;
	switch(type)
	{
	case Attribute::DOUBLE:
		attr->setDouble(newattribute.getDouble());
		break;
	case Attribute::STRING:
		attr->setString(newattribute.getString());
		break;
	case Attribute::DOUBLEVECTOR:
		attr->setDoubleVector(newattribute.getDoubleVector());
		break;
	case Attribute::STRINGVECTOR:
		attr->setStringVector(newattribute.getStringVector());
		break;
	case Attribute::TIMESERIES:
		newattribute.getTimeSeries(&vecStr, &vecDbl);
		attr->addTimeSeries(vecStr,vecDbl);
		break;
	case Attribute::LINK:
		attr->setLinks(newattribute.getLinks());
		break;
	default:	
		attr->setType(type);
		break;
	}
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
	if(HasAttribute(name))
	{
		delete ownedattributes[name];
		ownedattributes.erase(name);
	}

    return false;
}

Attribute* Component::getAttribute(std::string name)
{
    if(!HasAttribute(name))
    {
        Attribute tmp(name);
        this->addAttribute(tmp);
    }

    return ownedattributes[name];
}

const std::map<std::string, Attribute*> & Component::getAllAttributes() const
{
    return ownedattributes;
}

Component* Component::clone() {
    return new Component(*this);
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

System * Component::getCurrentSystem() {
    return this->currentSys;
}

void Component::setCurrentSystem(System *sys) {
    this->currentSys = sys;
}

void Component::SetOwner(Component *owner)
{
	SQLSetOwner(owner);
	currentSys = owner->getCurrentSystem();
	stateUuid = owner->getStateUUID();

    for (std::map<std::string,Attribute*>::iterator it=ownedattributes.begin() ; it != ownedattributes.end(); ++it )
    {
		it->second->SetOwner(this);
    }
}
void Component::SQLSetOwner(Component * owner)
{
    /*
	QString strType;
	switch(getType())
	{
		case COMPONENT:	strType = "components";	break;
		case NODE:		strType = "nodes";	break;
		case EDGE:		strType = "edges";	break;
		case FACE:		strType = "faces";	break;
		case SUBSYSTEM:	strType = "systems";	break;
		case RASTERDATA:strType = "rasterdatas";	break;
	}
	{
		QSqlQuery q;
		q.prepare("UPDATE "+strType+" SET owner=?,stateuuid=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
		q.addBindValue(QString::fromStdString(owner->getUUID()));
		q.addBindValue(QString::fromStdString(owner->getStateUUID()));
		q.addBindValue(QString::fromStdString(this->getUUID()));
		q.addBindValue(QString::fromStdString(this->getStateUUID()));
		if(!q.exec())	PrintSqlError(&q);
    }*/
    DBConnector::getInstance()->Update(getTableName(),
                                       QString::fromStdString(uuid),
                                       QString::fromStdString(stateUuid),
                                       "owner", QString::fromStdString(owner->uuid),
                                       "stateuuid", QString::fromStdString(owner->stateUuid));
}

void Component::SQLInsertComponent()
{
    DBConnector::getInstance()->Insert("components",
                                       QString::fromStdString(uuid),
                                       QString::fromStdString(stateUuid),
                                       "name", QString::fromStdString(name));
    /*
	QSqlQuery q;
	q.prepare("INSERT INTO components (uuid,stateuuid,name) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));
	q.addBindValue(QString::fromStdString(this->name));
    if(!q.exec())	PrintSqlError(&q);*/
}

void Component::SQLDeleteComponent()
{
    // note: if its not a component, it will just do nothing
    DBConnector::getInstance()->Delete("components",
                                       QString::fromStdString(uuid),
                                       QString::fromStdString(stateUuid));
    /*
	QSqlQuery q;
	q.prepare("DELETE FROM components WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));
    if(!q.exec())	PrintSqlError(&q);*/
}
/*
void Component::SQLInsertAs(std::string type)
{
	QSqlQuery q;
	q.prepare("INSERT INTO "+QString::fromStdString(type)+"s (uuid,stateuuid,name) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));
	q.addBindValue(QString::fromStdString(this->name));
	if(!q.exec())	PrintSqlError(&q);
	// important: delete component entry in sql
    // SQLDeleteComponentOnly();
}*/

void Component::SQLDelete(QString type)
{
    DBConnector::getInstance()->Delete(type, QString::fromStdString(uuid),
                                                QString::fromStdString(stateUuid));
}

bool Component::HasAttribute(std::string name)
{
	return ownedattributes.find(name)!=ownedattributes.end();
}

