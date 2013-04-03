/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
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

class ComponentSyncMap: public Asynchron, public std::set<Component*>
{
public:
	void Synchronize()
	{
		foreach(Component* c, *this)
			c->SaveToDb();
	}
};

static ComponentSyncMap componentSyncMap;

Component::Component()
{
	mutex = new QMutex(QMutex::Recursive);
    DBConnector::getInstance();
    uuid = QUuid::createUuid();
    ownedattributes = std::map<std::string,Attribute*>();

    inViews = std::set<std::string>();
    currentSys = NULL;

    DBConnector::getInstance();
	isCached = true;
	componentSyncMap.insert(this);
}

Component::Component(bool b)
{
	mutex = new QMutex(QMutex::Recursive);
    DBConnector::getInstance();
    uuid = QUuid::createUuid();
    ownedattributes = std::map<std::string,Attribute*>();

    inViews = std::set<std::string>();
    currentSys = NULL;
	isCached = false;
}


void Component::CopyFrom(const Component &c)
{
	uuid = QUuid::createUuid();
    inViews = c.inViews;
	currentSys = NULL;

	mforeach(Attribute* a, c.ownedattributes)
	{
		Attribute* newa = new Attribute(*a);
		ownedattributes[newa->getName()] = newa;
		newa->SetOwner(this);
	}
}

Component::Component(const Component& c)
{
	mutex = new QMutex(QMutex::Recursive);
	CopyFrom(c);
	componentSyncMap.insert(this);
	isCached = true;
}

Component::Component(const Component& c, bool bInherited)
{
	mutex = new QMutex(QMutex::Recursive);
    CopyFrom(c);
	isCached = false;
}

Component::~Component()
{
	mutex->lockInline();
	mforeach(Attribute* a, ownedattributes)
		delete a;

	ownedattributes.clear();
	if(isCached)
		componentSyncMap.erase(this);
	// if this class is not of type component, nothing will happen
    SQLDelete();
	mutex->unlockInline();
	delete mutex;
}

Component& Component::operator=(const Component& other)
{
	QMutexLocker ml(mutex);

	if(this != &other)
	{
		mforeach(Attribute* a, other.ownedattributes)
			this->addAttribute(*a);
	}
	return *this;
}

bool Component::isInView(View view) const 
{
    foreach (std::string s, inViews) {
        if (view.getName().compare(s) == 0)
            return true;
    }
    return false;
}
std::string Component::getUUID()
{
	Attribute* a = this->getAttribute(UUID_ATTRIBUTE_NAME);
	std::string name = a->getString();
	if(name == "")	
	{
		QMutexLocker ml(mutex);

		name = QUuid::createUuid().toString().toStdString();
		a->setString(name);
		if(this->currentSys && currentSys != this)	// avoid self referencing
			currentSys->componentNameMap[name] = this;
	}
	return name;
}
QUuid Component::getQUUID() const
{
    return uuid;
}

DM::Components Component::getType()
{
    return DM::COMPONENT;
}
QString Component::getTableName()
{
    return "components";
}

bool Component::addAttribute(std::string name, double val) 
{
	QMutexLocker ml(mutex);

	if(HasAttribute(name))
        return this->changeAttribute(name, val);

    return this->addAttribute(new Attribute(name, val));
}

bool Component::addAttribute(std::string name, std::string val) 
{
	QMutexLocker ml(mutex);

	if(HasAttribute(name))
        return this->changeAttribute(name, val);

    return this->addAttribute(new Attribute(name, val));
}

bool Component::addAttribute(const Attribute &newattribute)
{
	QMutexLocker ml(mutex);

    if(HasAttribute(newattribute.getName()))
        return this->changeAttribute(newattribute);

    Attribute * a = new Attribute(newattribute);
    ownedattributes[newattribute.getName()] = a;

    a->SetOwner(this);
    return true;
}

bool Component::addAttribute(Attribute *pAttribute)
{
	QMutexLocker ml(mutex);

    if(HasAttribute(pAttribute->getName()))
        delete ownedattributes[pAttribute->getName()];

	ownedattributes[pAttribute->getName()] = pAttribute;
    pAttribute->SetOwner(this);
    return true;
}

bool Component::changeAttribute(const Attribute &newattribute)
{
	QMutexLocker ml(mutex);

    getAttribute(newattribute.getName())->Change(newattribute);
    return true;
}

bool Component::changeAttribute(std::string s, double val)
{
	QMutexLocker ml(mutex);

    getAttribute(s)->setDouble(val);
    return true;
}

bool Component::changeAttribute(std::string s, std::string val)
{
	QMutexLocker ml(mutex);

    getAttribute(s)->setString(val);
    return true;
}

bool Component::removeAttribute(std::string name)
{
	QMutexLocker ml(mutex);

	return delete_element(&ownedattributes, name);
}

Attribute* Component::getAttribute(std::string name)
{
	Attribute* a;
	if(!map_contains(&ownedattributes, name, a))
	{
		QMutexLocker ml(mutex);

		a = new Attribute(name);
		a->SetOwner(this);
		ownedattributes[name] = a;
	}
	return a;
}

const std::map<std::string, Attribute*> & Component::getAllAttributes() const
{
    return ownedattributes;
}

Component* Component::clone() 
{
    return new Component(*this);
}

void Component::setView(std::string view)
{
	QMutexLocker ml(mutex);

    this->inViews.insert(view);
}

void Component::setView(const DM::View & view)
{
	QMutexLocker ml(mutex);

    this->inViews.insert(view.getName());
}

void Component::removeView(const View &view)
{
	QMutexLocker ml(mutex);

    this->inViews.erase(view.getName());
}

const set<std::string> &Component::getInViews() const 
{
    return this->inViews;
}

System * Component::getCurrentSystem() 
{
    return this->currentSys;
}

void Component::setCurrentSystem(System *sys) 
{
	QMutexLocker ml(mutex);

    this->currentSys = sys;
}

void Component::SetOwner(Component *owner)
{
	QMutexLocker ml(mutex);

    currentSys = owner->getCurrentSystem();

    for (std::map<std::string,Attribute*>::iterator it=ownedattributes.begin() ; it != ownedattributes.end(); ++it )
		it->second->SetOwner(this);
}

void Component::SaveToDb()
{
	QMutexLocker ml(mutex);

	if(this->getType() != DM::COMPONENT || !currentSys)
		return;

	if(!isInserted)
	{
		isInserted = true;
		DBConnector::getInstance()->Insert(	"components", uuid, 
											"owner", currentSys->getQUUID().toByteArray());
	}
	else
		DBConnector::getInstance()->Update(	"components", uuid, 
											"owner", currentSys->getQUUID().toByteArray());
}
void Component::SQLDelete()
{
	QMutexLocker ml(mutex);

	if(isInserted)
	{
		isInserted = false;
		DBConnector::getInstance()->Delete(getTableName(), uuid);
	}
}

bool Component::HasAttribute(std::string name)
{
	return ownedattributes.find(name)!=ownedattributes.end();
}