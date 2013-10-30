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

Component::Component()
{
	mutex = new QMutex(QMutex::Recursive);
	DBConnector::getInstance();
	uuid = QUuid::createUuid();

	currentSys = NULL;
	isInserted = false;

	DBConnector::getInstance();
}

Component::Component(bool b)
{
	mutex = new QMutex(QMutex::Recursive);
	DBConnector::getInstance();
	uuid = QUuid::createUuid();

	currentSys = NULL;
	isInserted = false;
}

void Component::CopyFrom(const Component &c, bool successor)
{
	uuid = QUuid::createUuid();

	if(!successor)
	{
		foreach(Attribute* a, c.ownedattributes)
		{
			Attribute* newa = new Attribute(*a);
			ownedattributes.push_back(newa);
			newa->setOwner(this);
		}
	}
	else
		ownedattributes = c.ownedattributes;
}

Component::Component(const Component& c)
{
	currentSys = NULL;
	mutex = new QMutex(QMutex::Recursive);
	CopyFrom(c);
}

Component::Component(const Component& c, bool bInherited)
{
	currentSys = NULL;
	mutex = new QMutex(QMutex::Recursive);
	CopyFrom(c);
}

Component::~Component()
{
	mutex->lockInline();
	foreach(Attribute* a, ownedattributes)
		if(a->GetOwner() == this)
			delete a;

	ownedattributes.clear();
	// if this class is not of type component, nothing will happen
	SQLDelete();
	mutex->unlockInline();
	delete mutex;
}

Component& Component::operator=(const Component& other)
{
	QMutexLocker ml(mutex);

	if(this != &other)
	for (std::vector<Attribute*>::const_iterator it = other.ownedattributes.cbegin(); 
		it != other.ownedattributes.cend(); ++it)
			this->addAttribute(*it);

	return *this;
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
		//if(this->currentSys && currentSys != this)	// avoid self referencing
		//	currentSys->componentNameMap[name] = this;
	}
	return name;
}
QUuid Component::getQUUID() const
{
	return uuid;
}

DM::Components Component::getType() const
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

	if (Attribute* a = getExistingAttribute(name))
	{
		a->setDouble(val);
		return true;
	}

	return this->addAttribute(new Attribute(name, val));
}

bool Component::addAttribute(std::string name, std::string val) 
{
	QMutexLocker ml(mutex);

	if (Attribute* a = getExistingAttribute(name))
	{
		a->setString(val);
		return true;
	}

	return this->addAttribute(new Attribute(name, val));
}

bool Component::addAttribute(const Attribute &newattribute)
{
	QMutexLocker ml(mutex);

	if (Attribute* a = getExistingAttribute(newattribute.getName()))
	{
		a->Change(newattribute);
		return true;
	}

	Attribute * a = new Attribute(newattribute);

	ownedattributes.push_back(a);
	a->setOwner(this);

	return true;
}

bool Component::addAttribute(Attribute *pAttribute)
{
	QMutexLocker ml(mutex);

	removeAttribute(pAttribute->getName());

	ownedattributes.push_back(pAttribute);
	pAttribute->setOwner(this);
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

	for (std::vector<Attribute*>::iterator  it = ownedattributes.begin(); it != ownedattributes.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			if ((*it)->GetOwner() == this)
				delete *it;

			ownedattributes.erase(it);
			return true;
		}
	}

	return false;
}

Attribute* Component::getAttribute(std::string name)
{
	if (name.empty())
		return NULL;

	QMutexLocker ml(mutex);

	std::vector<Attribute*>::iterator it;
	for (it = ownedattributes.begin(); it != ownedattributes.end(); ++it)
		if ((*it)->getName() == name)
			break;

	if (it == ownedattributes.end())
	{
		// create new attribute
		Attribute* a = new Attribute(name);
		a->setOwner(this);
		ownedattributes.push_back(a);
		return a;
	}
	else if((*it)->GetOwner() != this)
	{
		// successor copy
		Attribute* a = new Attribute(**it);
		ownedattributes.erase(it);
		ownedattributes.push_back(a);
		a->setOwner(this);
		return a;
	}

	return *it;
}

const std::vector<Attribute*>& Component::getAllAttributes()
{
	// clone all attributes
	for (std::vector<Attribute*>::iterator it = ownedattributes.begin(); it != ownedattributes.end(); ++it)
	{
		if ((*it)->GetOwner() != this)
		{
			*it = new Attribute(**it);
			(*it)->setOwner(this);
		}
	}

	return ownedattributes;
}

Component* Component::clone() 
{
	return new Component(*this);
}

System * Component::getCurrentSystem() const
{
	return this->currentSys;
}

void Component::SetOwner(Component *owner)
{
	QMutexLocker ml(mutex);

	currentSys = owner->getCurrentSystem();
}

void Component::_moveToDb()
{
	// mutex will cause a crash
	//QMutexLocker ml(mutex);

	if(!isInserted)
	{
		DBConnector::getInstance()->Insert(	"components", uuid, 
			"owner", (QVariant)currentSys->getQUUID().toByteArray());
	}
	else
	{
		DBConnector::getInstance()->Update(	"components", uuid, 
			"owner", currentSys->getQUUID().toByteArray());
		isInserted = false;
	}
	_moveAttributesToDb();
	delete this;
}

void Component::_moveAttributesToDb()
{
	for (std::vector<Attribute*>::iterator it = ownedattributes.begin(); it != ownedattributes.end(); ++it)
		Attribute::_MoveAttribute(*it);
	ownedattributes.clear();
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

bool Component::HasAttribute(std::string name) const
{
	for (std::vector<Attribute*>::const_iterator it = ownedattributes.cbegin(); it != ownedattributes.cend(); ++it)
		if ((*it)->getName() == name)
			return true;

	return false;
}

Attribute* Component::getExistingAttribute(const std::string& name) const
{
	for (std::vector<Attribute*>::const_iterator it = ownedattributes.cbegin(); it != ownedattributes.cend(); ++it)
		if ((*it)->getName() == name)
			return *it;

	return NULL;
}
