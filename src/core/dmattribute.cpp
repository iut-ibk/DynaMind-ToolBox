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

#include <dmmodule.h>
#include <dmcomponent.h>
#include <dmattribute.h>
#include <QVariant>
#include "dmdbconnector.h"
#include "dmlogger.h"
#include <dmsystem.h>

using namespace DM;

typedef std::pair<std::string, std::vector<Component*> > LinkVector;

/*
DbCache<Attribute*,Attribute::AttributeValue> Attribute::attributeCache(0);

typedef std::pair<std::string, std::vector<Component*> > LinkVector;

void Attribute::ResizeCache(unsigned long size)
{
	attributeCache.resize(size);
}
void Attribute::ClearCache()
{
	attributeCache.Clear();
}
unsigned long Attribute::GetCacheSize()
{
	return attributeCache.getSize();
}

void Attribute::PrintCacheStatistics()
{
#ifdef CACHE_PROFILING
	Logger(Standard) << "Attribute cache statistics:\t"
		<< "misses: " << (long)attributeCache.misses
		<< "\thits: " << (long)attributeCache.hits;
	attributeCache.ResetProfilingCounters();
#endif
}*/

QByteArray GetBinaryValue(std::vector<double> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<double>::fromStdVector(v);
	return bytes;
}

QByteArray GetBinaryValue(std::vector<std::string> v)
{
	std::vector<QString> qv;
	foreach(std::string item, v)
		qv.push_back(QString::fromStdString(item));

	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<QString>::fromStdVector(qv);
	return bytes;
}

QByteArray GetBinaryValue(LinkVector v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QString::fromStdString(v.first);
	s << (int)v.second.size();
	foreach(Component* c, v.second)
		s << c->getQUUID();

	return bytes;
}

QByteArray GetBinaryValue(TimeSeriesAttribute a)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	int size = a.timestamp.size();
	s << size;
	for(int i=0;i<size;i++)
	{
		s << QString::fromStdString(a.timestamp[i]);
		s << a.value[i];
	}
	return bytes;
}

LinkVector ToLinkVector(QVariant q, System* owningSystem)
{
	LinkVector result;
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	int size;
	QString viewName;
	s >> viewName;
	result.first = viewName.toStdString();
	s >> size;
	QUuid uuid;
	for(int i=0;i<size;i++)
	{
		s >> uuid;
		result.second.push_back(owningSystem->getChild(uuid));
	}
	return result;
}

std::vector<double> ToDoubleVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<double> v;
	s >> v;
	return v.toStdVector();
}

std::vector<std::string> ToStringVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<QString> qv;
	s >> qv;
	std::vector<std::string> sv;
	foreach(QString qstr, qv)
		sv.push_back(qstr.toStdString());

	return sv;
}

TimeSeriesAttribute ToTimeSeriesAttribute(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	TimeSeriesAttribute ts;
	int size;
	s >> size;
	for(int i=0;i<size;i++)
	{
		QString str;
		s >> str;
		double d;
		s >> d;

		ts.timestamp.push_back(str.toStdString());
		ts.value.push_back(d);
	}
	return ts;
}

Attribute::AttributeValue::AttributeValue()
{
	ptr = NULL;
	type = NOTYPE;
}

Attribute::AttributeValue::~AttributeValue()
{
	Free();
}

Attribute::AttributeValue::AttributeValue(const AttributeValue& ref)
{
	this->type = ref.type;
	switch(ref.type)
	{
	case NOTYPE:
		ptr = NULL;
		break;
	case DOUBLE:
		ptr = new double(*((double*)ref.ptr));
		break;
	case STRING:
		ptr = new std::string(*((std::string*)ref.ptr));
		break;
	case TIMESERIES:
		ptr = new TimeSeriesAttribute(*((TimeSeriesAttribute*)ref.ptr));
		break;
	case LINK:
		ptr = new LinkVector(*((LinkVector*)ref.ptr));
		break;
	case DOUBLEVECTOR:
		ptr = new std::vector<double>(*((std::vector<double>*)ref.ptr));
		break;
	case STRINGVECTOR:
		ptr = new std::vector<std::string>(*((std::vector<std::string>*)ref.ptr));
		break;
	default: 
		type = NOTYPE;
		ptr = NULL;
		break;
	}
}

Attribute::AttributeValue::AttributeValue(QVariant var, AttributeType type, System* owningSystem)
{
	this->type = type;
	switch(type)
	{
	case NOTYPE:	ptr = NULL;
		break;
	case DOUBLE:	ptr = new double(var.toDouble());
		break;
	case STRING:	ptr = new std::string(var.toString().toStdString());
		break;
	case TIMESERIES:ptr = new TimeSeriesAttribute(ToTimeSeriesAttribute(var));
		break;
	case LINK:		ptr = new LinkVector(ToLinkVector(var, owningSystem));
		break;
	case DOUBLEVECTOR:	ptr = new std::vector<double>(ToDoubleVector(var));
		break;
	case STRINGVECTOR:	ptr = new std::vector<std::string>(ToStringVector(var));
		break;
	default: ptr = NULL;
		type = NOTYPE;
		break;
	}
}
void Attribute::AttributeValue::Free()
{
	switch(type)
	{
	case DOUBLE:	delete (double*)ptr;
		break;
	case STRING:		delete (std::string*)ptr;
		break;
	case TIMESERIES:	delete (TimeSeriesAttribute*)ptr;
		break;
	case LINK:			delete (LinkVector*)ptr;
		break;
	case DOUBLEVECTOR:		delete (std::vector<double>*)ptr;
		break;
	case STRINGVECTOR:		delete (std::vector<std::string>*)ptr;
		break;
	default:
		break;
	}
	ptr = NULL;
	type = NOTYPE;
}
Attribute::AttributeValue::AttributeValue(double d)
{
	type = DOUBLE;
	ptr = new double(d);
}
Attribute::AttributeValue::AttributeValue(std::string string)
{
	type = STRING;
	ptr = new std::string(string);
}

QVariant Attribute::AttributeValue::toQVariant()
{
	switch(type)
	{
	case NOTYPE:	return QVariant();
	case DOUBLE:	return QVariant::fromValue(*(double*)ptr);
	case STRING:	return QString::fromStdString(*(std::string*)ptr);
	case TIMESERIES:return GetBinaryValue(*(TimeSeriesAttribute*)ptr);
	case LINK:		return GetBinaryValue(*(LinkVector*)ptr);
	case DOUBLEVECTOR:	return GetBinaryValue(*(std::vector<double>*)ptr);
	case STRINGVECTOR:	return GetBinaryValue(*(std::vector<std::string>*)ptr);
	default:		return QVariant();
	}
}

Attribute::Attribute()
{
//	_uuid = QUuid::createUuid();
	name="";
	owner = NULL;
	isInserted = false;
}

Attribute::Attribute(const Attribute &newattribute):
	value(newattribute.value)
{
//	_uuid = QUuid::createUuid();
	name=newattribute.name;
//	value = AttributeValue(newattribute.value);
	owner = NULL;
	isInserted = false;
}

Attribute::Attribute(std::string name)
{
//	_uuid = QUuid::createUuid();
	this->name=name;
	owner = NULL;
//	value = new AttributeValue();
	isInserted = false;
}


Attribute::Attribute(std::string name, double val):
	value(val)
{
//	_uuid = QUuid::createUuid();
	this->name=name;
	owner = NULL;
	isInserted = false;
//	value = new AttributeValue(val);
}
Attribute::Attribute(std::string name, std::string val):
	value(val)
{
//	_uuid = QUuid::createUuid();
	this->name=name;
	owner = NULL;
	isInserted = false;
//	value = new AttributeValue(val);
}

Attribute::~Attribute()
{
	if(isInserted)
		DBConnector::getInstance()->Delete("attributes", owner->getQUUID(), QString::fromStdString(name));
}

Attribute& Attribute::operator=(const Attribute& other)
{
	if(this != &other)
	{
		name = other.name;
		this->value = other.value;
	}
	return *this;
}

Attribute::AttributeType Attribute::getType() const
{
	return value.type;
}

void Attribute::setName(std::string name)
{
	this->name=name;
}

std::string Attribute::getName() const
{
	return name;
}

void Attribute::setDouble(double v)
{
	//AttributeValue* a = getValue();
	value.Free();
	//delete a->ptr;
	value.type = DOUBLE;
	value.ptr = new double(v);
}

double Attribute::getDouble()
{
//	AttributeValue* a = getValue();
	if(value.type == DOUBLE)	return *((double*)value.ptr);
	return 0;
}

void Attribute::setString(std::string s)
{
//	AttributeValue* a = getValue();
	value.Free();
	value.type = STRING;
	value.ptr = new std::string(s);
}

std::string Attribute::getString()
{	
	//AttributeValue* a = getValue();
	if(value.type == STRING)	return *((std::string*)value.ptr);
	return "";
}

void Attribute::setDoubleVector(std::vector<double> v)
{
	//AttributeValue* a = getValue();
	value.Free();
	value.type = DOUBLEVECTOR;
	value.ptr = new std::vector<double>(v);
}

std::vector<double> Attribute::getDoubleVector()
{
	//AttributeValue* a = getValue();
	if(value.type == DOUBLEVECTOR)	return *((std::vector<double>*)value.ptr);
	return std::vector<double>();
}

void Attribute::setStringVector(std::vector<std::string> s)
{
	//AttributeValue* a = getValue();
	value.Free();
	value.type = STRINGVECTOR;
	value.ptr = new std::vector<std::string>(s);
}

std::vector<std::string> Attribute::getStringVector()
{
	//AttributeValue* a = getValue();
	if(value.type == STRINGVECTOR)	return *((std::vector<std::string>*)value.ptr);
	return std::vector<string>();
}

bool Attribute::hasDouble()
{
	if (getType() == Attribute::DOUBLE)
		return true;
	return false;
}
bool Attribute::hasDoubleVector()
{
	if (getType() == Attribute::DOUBLEVECTOR)
		return true;
	return false;
}
bool Attribute::hasString()
{
	if (getType() == Attribute::STRING)
		return true;
	return false;
}
bool Attribute::hasStringVector()
{
	if (getType() == Attribute::STRINGVECTOR)
		return true;
	return false;
}

void Attribute::addLink(Component* target, const std::string& viewName)
{
	if(getType() != LINK)
		value.Free();

	if(!value.ptr)
	{
		value.ptr = new LinkVector(viewName, std::vector<Component*>());
		value.type = LINK;
	}

	LinkVector* lvalue = (LinkVector*)value.ptr;
	lvalue->second.push_back(target);
}

bool Attribute::removeLink(Component* target)
{
	if(value.type != LINK || !value.ptr)
		return false;

	std::vector<Component*>& linkedComponents = ((LinkVector*)value.ptr)->second;
	int size = linkedComponents.size();
	std::remove(linkedComponents.begin(), linkedComponents.end(), target);
	return size != linkedComponents.size();
}

void Attribute::clearLinks()
{
	if(value.type == LINK)
		if(LinkVector* lvalue = (LinkVector*)value.ptr)
			lvalue->second.clear();
}

std::vector<Component*> Attribute::getLinkedComponents()
{
	if(value.type == LINK)
		if(LinkVector* lvalue = (LinkVector*)value.ptr)
		{
			//foreach(Component* c, value->second)
			for(std::vector<Component*>::iterator it = lvalue->second.begin();
				it != lvalue->second.end(); ++it)
			{
				if((*it)->getCurrentSystem() != GetOwner()->getCurrentSystem())
				{
					// changed due to successor creation
					*it = GetOwner()->getCurrentSystem()->getSuccessingComponent((*it));
				}
			}
			return lvalue->second;
		}
	return std::vector<Component*>();
}

/*
void Attribute::setLink(string viewname, string uuid)
{
	std::vector<LinkAttribute> links = getLinks();
	LinkAttribute att;
	att.uuid = uuid;
	att.viewname = viewname;
	links.push_back(att);
	setLinks(links);
}

void Attribute::setLinks(std::vector<LinkAttribute> links)
{
//	AttributeValue* a = getValue();
	value.Free();
	value.type = LINK;
	value.ptr = new std::vector<LinkAttribute>(links);
}

LinkAttribute Attribute::getLink()
{
	//AttributeValue* a = getValue();
	if(value.type == LINK && (*((std::vector<LinkAttribute>*)value.ptr)).size() > 0)	
		return (*((std::vector<LinkAttribute>*)value.ptr))[0];
	return LinkAttribute();
}

std::vector<LinkAttribute> Attribute::getLinks()
{
	//AttributeValue* a = getValue();
	if(value.type == LINK)	return *((std::vector<LinkAttribute>*)value.ptr);
	return std::vector<LinkAttribute>();
}*/

void Attribute::addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value)
{
	if(timestamp.size()!=value.size())
	{
		DM::Logger(DM::Error) << "Length of time and value vector are not equal";
		return;
	}
	//AttributeValue* a = getValue();
	this->value.Free();
	this->value.type = TIMESERIES;
	this->value.ptr = new TimeSeriesAttribute(&timestamp, &value);
}

void Attribute::getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value)
{
	//AttributeValue* a = getValue();
	if(this->value.type == TIMESERIES)	
	{
		*timestamp = ((TimeSeriesAttribute*)this->value.ptr)->timestamp;
		*value = ((TimeSeriesAttribute*)this->value.ptr)->value;
	}
}

void Attribute::setType(AttributeType type)
{
	//AttributeValue* a = getValue();
	value.Free();
	value.type = type;
	switch(type)
	{
	case NOTYPE:
		value.ptr = NULL;
		break;
	case DOUBLE:
		value.ptr = new double(0);
		break;
	case STRING:
		value.ptr = new std::string();
		break;
	case TIMESERIES:
		value.ptr = new TimeSeriesAttribute();
		break;
	case LINK:
		value.ptr = new LinkVector();
		break;
	case DOUBLEVECTOR:
		value.ptr = new std::vector<double>();
		break;
	case STRINGVECTOR:
		value.ptr = new std::vector<std::string>();
		break;
	default: value.ptr = NULL;
		value.type = NOTYPE;
		break;
	}
}
void Attribute::Change(const Attribute &attribute)
{
	//name = attribute.name; name should never be changed!
	value.Free();
	AttributeValue* val = new AttributeValue(attribute.value);
	value = *val;
	val->ptr = NULL;

	/*AttributeValue* newValue = new AttributeValue(*attribute.value);
	if(value)
	{
		delete value;
		value = newValue;
	}
	else
	{
		if(!attributeCache.replace(this, newValue))
			this->SaveToDb(newValue);
	}*/
}

const char *Attribute::getTypeName() const
{
	return Attribute::getTypeName(this->getType());
}

const char *Attribute::getTypeName(Attribute::AttributeType type)
{
	const char *arr[] = {
		"NOTYPE",
		"DOUBLE",
		"STRING",
		"TIMESERIES",
		"LINK",
		"DOUBLEVECTOR",
		"STRINGVECTOR"
	};
	return arr[type];
}
/*
Attribute::AttributeValue* Attribute::getValue() const
{
	if(value)	return value;
	else		return attributeCache.get((Attribute*)this);
}*/

void Attribute::setOwner(Component* owner)
{
	/*if(!this->owner)
	{
		this->owner = owner;
		attributeCache.add(this, value);
		value = 0;
	}
	else*/
		this->owner = owner;

	//value = NULL;
	// TODO: make shure its not bound to another component
}

Component* Attribute::GetOwner()
{
	return owner;
}

Attribute* Attribute::LoadAttribute(Component* c, const std::string& attributeName)
{
	QVariant t,v;
	DBConnector::getInstance()->Select("attributes", c->getQUUID(), QString::fromStdString(attributeName),
		"type",     &t,
		"value",     &v);

	Attribute* a = new Attribute(attributeName);
	a->setOwner(c);
	a->isInserted = true;

	AttributeValue* val = new AttributeValue(v,(AttributeType)t.toInt(), c->getCurrentSystem());
	a->value = *val;
	val->ptr = NULL;
	return a;
}

void Attribute::_MoveAttribute(Attribute* a)
{
	if(!a || !a->owner)
		return;
	
	QVariant qval = a->value.toQVariant();
	QVariant qtype = QVariant::fromValue((int)a->value.type);
	QString qname = QString::fromStdString(a->name);

	if(a->isInserted)
	{
		DBConnector::getInstance()->Update(
			"attributes",	a->owner->getQUUID(), qname,
			"type",			&qtype,
			"value",		&qval);
		a->isInserted = false;
	}
	else
	{
		DBConnector::getInstance()->Insert(
			"attributes",	a->owner->getQUUID(), qname,
			"type",			&qtype,
			"value",		&qval);
	}
	delete a;
}

