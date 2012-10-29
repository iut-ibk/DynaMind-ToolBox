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
    //childsview = std::map<std::string,Component*>();
    //attributesview = std::map<std::string,Attribute*>();
    ownedattributes =  std::map<std::string,Attribute*>();
    inViews = std::set<std::string>();
    currentSys = NULL;
	
	DBConnector::getInstance();
	SQLInsertComponent();
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
	SQLSetName(name);
}

std::string Component::getName() const {
return name;
}

Component::Component(const Component& c)
{
    uuid=c.uuid;
    this->stateUuid = QUuid::createUuid().toString().toStdString();
    //attributesview=c.attributesview;
    //ownedchilds=c.ownedchilds;
    inViews = c.inViews;
	name = c.name;
	
	std::map<std::string,Attribute*> attrmap = c.ownedattributes;
	for (std::map<std::string,Attribute*>::iterator it=attrmap.begin() ; it != attrmap.end(); it++ )
    {
		this->addAttribute(*it->second);
    }

	SQLInsertComponent();
}

Component::~Component()
{
	while(ownedattributes.size())// && bLoadedAttributes)
    {
		//SQLDeleteAttribute((*ownedattributes.begin()).second);
        delete (*ownedattributes.begin()).second;
        ownedattributes.erase(ownedattributes.begin());
    }
	// if this class is not of type component, nothing will happen
	SQLDeleteComponentOnly();
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
	//SQLLoadAttributes();
    /*if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }*/
    if(ownedattributes.find(name)!=ownedattributes.end()) {
        return this->changeAttribute(name, val);
    }

    Attribute  attr(name, val);
    return this->addAttribute(attr);
	/*
    const Attribute* attr = new Attribute(name, val);
    bool b = this->addAttribute(*attr);
	delete attr;
	return b;*/
}

bool Component::addAttribute(std::string name, std::string val) 
{
	//SQLLoadAttributes();
    /*if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }*/
	if(ownedattributes.find(name)!=ownedattributes.end()) {
        return this->changeAttribute(name, val);
    }
    //Attribute  attr = Attribute(name, val);
    //return this->addAttribute(attr);

	Attribute* attr = new Attribute(name, val);
    bool b = this->addAttribute(*attr);
	delete attr;
	return b;
}

bool Component::addAttribute(Attribute &newattribute)
{
	//SQLLoadAttributes();
    //if(attributesview.find(newattribute.getName())!=attributesview.end())
	if(ownedattributes.find(newattribute.getName())!=ownedattributes.end())
        return this->changeAttribute(newattribute);

    Attribute * a = new Attribute(newattribute);
    //attributesview[newattribute.getName()] = a;
    ownedattributes[newattribute.getName()] = a;

	a->SetOwner(this);
    return true;
}

bool Component::changeAttribute(Attribute newattribute)
{
	//SQLLoadAttributes();
    //if(attributesview.find(newattribute.getName())==attributesview.end()) {
    //    return this->addAttribute(newattribute);
    //}
    if(ownedattributes.find(newattribute.getName())==ownedattributes.end()) {
        ownedattributes[newattribute.getName()] = new Attribute(*(ownedattributes[newattribute.getName()]));
        //attributesview[newattribute.getName()] = ownedattributes[newattribute.getName()];
    }
    //Attribute * attr = attributesview[newattribute.getName()];
    Attribute * attr = ownedattributes[newattribute.getName()];
    Attribute::AttributeType type = attr->getType();
    attr->setDouble(newattribute.getDouble());
    attr->setDoubleVector(newattribute.getDoubleVector());
    attr->setString(newattribute.getString());
    attr->setStringVector(newattribute.getStringVector());
    attr->setType(type);

	//SQLUpdateAttribute(newattribute.getName(), attr);

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
	//SQLLoadAttributes();
    //if(attributesview.find(name)!=attributesview.end())
    //{
        if(ownedattributes.find(name)!=ownedattributes.end())
        {
            delete ownedattributes[name];
            ownedattributes.erase(name);
			//SQLDeleteAttribute(name);
        }

       // attributesview.erase(name);
      //  return true;
   // }

    return false;
}

Attribute* Component::getAttribute(std::string name)
{
	//SQLLoadAttributes();
    //if(attributesview.find(name)==attributesview.end())
	if(ownedattributes.find(name)!=ownedattributes.end())
        this->addAttribute(Attribute(name));

    //return attributesview[name];
    return ownedattributes[name];
}

const std::map<std::string, Attribute*> & Component::getAllAttributes() const
{
    return ownedattributes;
    //return attributesview;
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

void Component::SQLSetName(std::string name)
{
	QSqlQuery q;
	q.prepare("UPDATE components SET name=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue(QString::fromStdString(this->getUUID()));
	q.addBindValue(QString::fromStdString(this->getStateUUID()));
	if(!q.exec())	PrintSqlError(&q);
}

void Component::SetOwner(Component *owner)
{
	SQLSetOwner(owner);
	currentSys = owner->getCurrentSystem();
	stateUuid = owner->getStateUUID();

	for (std::map<std::string,Attribute*>::iterator it=ownedattributes.begin() ; it != ownedattributes.end(); it++ )
    {
		it->second->SetOwner(this);
    }
}
void Component::SQLSetOwner(Component * owner)
{
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
	}
	/*
	if(owner->getType() == SUBSYSTEM)
	{
		QSqlQuery q;
		q.prepare("UPDATE systems SET stateuuid=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
		q.addBindValue(QString::fromStdString(owner->getStateUUID()));
		q.addBindValue(QString::fromStdString(this->getUUID()));
		q.addBindValue(QString::fromStdString(this->getStateUUID()));
		if(!q.exec())	PrintSqlError(&q);
	}
	*/
}

void Component::SQLInsertComponent()
{
	QSqlQuery q;
	q.prepare("INSERT INTO components (uuid,stateuuid,name) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));
	q.addBindValue(QString::fromStdString(this->name));

	if(!q.exec())	PrintSqlError(&q);
}

void Component::SQLDeleteComponentOnly()
{
	// note: if its not a component, it will just do nothing
	QSqlQuery q;
	q.prepare("DELETE FROM components WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));

	if(!q.exec())	PrintSqlError(&q);
}

void Component::SQLInsertAs(std::string type)
{
	QSqlQuery q;
	q.prepare("INSERT INTO "+QString::fromStdString(type)+"s (uuid,stateuuid,name) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));
	q.addBindValue(QString::fromStdString(this->name));

	if(!q.exec())	PrintSqlError(&q);
	// important: delete component entry in sql
	SQLDeleteComponentOnly();
}

void Component::SQLDeleteAs(std::string type)
{
	QSqlQuery q;
	q.prepare("DELETE FROM "+QString::fromStdString(type)+"s WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(this->uuid));
	q.addBindValue(QString::fromStdString(this->stateUuid));

	if(!q.exec())	PrintSqlError(&q);
}
